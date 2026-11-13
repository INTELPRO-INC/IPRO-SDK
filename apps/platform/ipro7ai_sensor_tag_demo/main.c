/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/*
 * ipro7ai_sensor_tag_demo: the sensors of the IPRO7AI Sensor Tag board over its own USB.
 *
 * Every 50 ms (20 Hz, `rate` changes it) the board prints one line of
 * `name:value` pairs:
 *
 *   ax:0.012 ay:-0.004 az:1.003 gx:0.3 gy:-0.1 gz:0.0 temp:27.4 batt:87 vbat:4.012 btn:0
 *
 *   ax ay az   acceleration in g          gx gy gz   rotation in degrees per second
 *   temp       the IMU's temperature, C   batt vbat  battery in percent and in volts
 *   btn        1 while the button is held
 *
 * That is the format IPRO Workbench's Serial Plotter and Sensor Dashboard read
 * (dashboard.yaml says how to show each value), and a spreadsheet or a script
 * reads it as easily.
 *
 * Shell commands: stream on|off, rate <1..100>, led red|green on|off, beep [ms].
 */
#include <generated/autoconf.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "shell.h"
#include "usb_console.h"

#include "cw2015.h"
#include "tag_board.h"
#include "ism330bx.h"
#include "spi_bitbang.h"

/* The IMU's own bus: three wires and a chip select, driven as GPIO. */
#define IMU_BUS             0xB0
#define IMU_CLOCK_HZ        2000000u
/* +-4 g and +-500 dps: 0.122 mg and 17.5 mdps per count (ISM330BX datasheet). */
#define ACCEL_UG_PER_COUNT  122
#define GYRO_UDPS_PER_COUNT 17500
/* OUT_TEMP_L: 256 counts per degree, 0 at 25 C. */
#define IMU_REG_OUT_TEMP_L  0x20

#define RATE_MIN_HZ         1
#define RATE_MAX_HZ         100
#define BATTERY_PERIOD_MS   1000

static volatile bool     s_stream = true;
static volatile uint32_t s_period_ms = 50;
static volatile uint32_t s_beep_ms;
static bool              s_imu_ok;
static bool              s_gauge_ok;

/* A fixed USB serial number is enough to tell one board from other devices. */
void device_serial_get(uint8_t out[8])
{
    static const uint8_t serial[8] = { 'T', 'A', 'G', 0x00, 0x00, 0x00, 0x00, 0x01 };

    memcpy(out, serial, sizeof(serial));
}

/* "-0.004" from -4 with three decimals: printf here has no floating point. */
static const char *fixed(char *text, size_t size, int32_t value, int decimals)
{
    int32_t scale = 1;
    uint32_t magnitude = (uint32_t)(value < 0 ? -value : value);

    for (int i = 0; i < decimals; i++) {
        scale *= 10;
    }
    snprintf(text, size, "%s%lu.%0*lu", value < 0 ? "-" : "",
             (unsigned long)(magnitude / (uint32_t)scale), decimals, (unsigned long)(magnitude % (uint32_t)scale));
    return text;
}

static int imu_start(void)
{
    uint8_t id = 0;

    if (spi_bitbang_init(IMU_BUS, TAG_IMU_SCLK_GPIO, TAG_IMU_MISO_GPIO, TAG_IMU_MOSI_GPIO,
                         TAG_IMU_CS_GPIO, IMU_CLOCK_HZ) != 0) {
        return -1;
    }
    if (ism330bx_init(IMU_BUS, TAG_IMU_CS_GPIO, ISM330BX_FS_XL_4G, ISM330BX_FS_G_500DPS) != 0) {
        return -2;
    }
    (void)ism330bx_who_am_i(&id);
    printf("[tag] IMU ISM330BX, WHO_AM_I 0x%02x\r\n", id);
    return ism330bx_start(ISM330BX_ODR_XL_120) == 0 ? 0 : -3;
}

/* Tenths of a degree Celsius, or INT32_MIN when the read failed. */
static int32_t imu_temperature(void)
{
    uint8_t raw[2];

    if (spi_bitbang_read(IMU_BUS, IMU_REG_OUT_TEMP_L, raw, sizeof(raw)) != 0) {
        return INT32_MIN;
    }
    return 250 + ((int32_t)(int16_t)((uint16_t)raw[0] | ((uint16_t)raw[1] << 8)) * 10) / 256;
}

static void sensor_task(void *arg)
{
    TickType_t last_battery = 0;
    uint8_t percent = 0;
    uint16_t millivolts = 0;
    bool have_battery = false;
    char t[10][16];

    (void)arg;
    tag_board_init();
    s_imu_ok = imu_start() == 0;
    s_gauge_ok = cw2015_init() == 0;
    printf("[tag] IPRO7AI Sensor Tag demo: IMU %s, fuel gauge %s. Commands: stream, rate, led, beep\r\n",
           s_imu_ok ? "ready" : "NOT FOUND", s_gauge_ok ? "ready" : "NOT FOUND");
    tag_led_green(true);

    for (;;) {
        TickType_t now = xTaskGetTickCount();
        ism330bx_sample_t s = { 0 };
        int32_t temperature = INT32_MIN;
        bool have_imu = s_imu_ok && ism330bx_read_sample(&s) == 0;
        uint32_t beep = s_beep_ms;

        if (beep) {
            /* The sensor task makes the tone, so no second task touches the PWM. */
            s_beep_ms = 0;
            tag_buzzer(true);
            vTaskDelay(pdMS_TO_TICKS(beep));
            tag_buzzer(false);
        }
        if (have_imu) {
            temperature = imu_temperature();
        }
        if (s_gauge_ok && (last_battery == 0 || now - last_battery >= pdMS_TO_TICKS(BATTERY_PERIOD_MS))) {
            last_battery = now ? now : 1;
            have_battery = cw2015_read_soc(&percent) == 0 && cw2015_read_vcell_mv(&millivolts) == 0;
        }
        /* The red LED follows the button: a first sign of life without any tool. */
        tag_led_red(tag_button_pressed());

        if (s_stream) {
            /* One printf per line: lines from the shell never land inside one. */
            char line[200];
            int n = 0;

            if (have_imu) {
                n += snprintf(line + n, sizeof(line) - (size_t)n, "ax:%s ay:%s az:%s gx:%s gy:%s gz:%s ",
                              fixed(t[0], sizeof(t[0]), (int32_t)s.ax * ACCEL_UG_PER_COUNT / 1000, 3),
                              fixed(t[1], sizeof(t[1]), (int32_t)s.ay * ACCEL_UG_PER_COUNT / 1000, 3),
                              fixed(t[2], sizeof(t[2]), (int32_t)s.az * ACCEL_UG_PER_COUNT / 1000, 3),
                              fixed(t[3], sizeof(t[3]), (int32_t)s.gx * (GYRO_UDPS_PER_COUNT / 100) / 1000, 1),
                              fixed(t[4], sizeof(t[4]), (int32_t)s.gy * (GYRO_UDPS_PER_COUNT / 100) / 1000, 1),
                              fixed(t[5], sizeof(t[5]), (int32_t)s.gz * (GYRO_UDPS_PER_COUNT / 100) / 1000, 1));
                if (temperature != INT32_MIN) {
                    n += snprintf(line + n, sizeof(line) - (size_t)n, "temp:%s ", fixed(t[6], sizeof(t[6]), temperature, 1));
                }
            }
            if (have_battery) {
                n += snprintf(line + n, sizeof(line) - (size_t)n, "batt:%u vbat:%s ", (unsigned)percent,
                              fixed(t[7], sizeof(t[7]), (int32_t)millivolts, 3));
            }
            snprintf(line + n, sizeof(line) - (size_t)n, "btn:%d", tag_button_pressed() ? 1 : 0);
            printf("%s\r\n", line);
        }
        vTaskDelay(pdMS_TO_TICKS(s_period_ms));
    }
}

static int cmd_stream(int argc, char **argv)
{
    if (argc == 2 && (strcmp(argv[1], "on") == 0 || strcmp(argv[1], "off") == 0)) {
        s_stream = argv[1][1] == 'n';
    } else {
        printf("usage: stream on|off\r\n");
        return -1;
    }
    printf("[tag] stream %s\r\n", s_stream ? "on" : "off");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_stream, stream, sensor lines on or off);

static int cmd_rate(int argc, char **argv)
{
    int hz = argc == 2 ? atoi(argv[1]) : 0;

    if (hz < RATE_MIN_HZ || hz > RATE_MAX_HZ) {
        printf("usage: rate <%d..%d>  (lines per second; now %lu)\r\n", RATE_MIN_HZ, RATE_MAX_HZ,
               (unsigned long)(1000u / s_period_ms));
        return -1;
    }
    s_period_ms = 1000u / (uint32_t)hz;
    printf("[tag] rate %d Hz\r\n", hz);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rate, rate, sensor lines per second);

static int cmd_led(int argc, char **argv)
{
    bool on;

    if (argc != 3 || (strcmp(argv[2], "on") != 0 && strcmp(argv[2], "off") != 0)) {
        printf("usage: led red|green on|off\r\n");
        return -1;
    }
    on = argv[2][1] == 'n';
    if (strcmp(argv[1], "green") == 0) {
        tag_led_green(on);
    } else if (strcmp(argv[1], "red") == 0) {
        /* The red LED follows the button; what is set here lasts until the next sample. */
        tag_led_red(on);
    } else {
        printf("usage: led red|green on|off\r\n");
        return -1;
    }
    printf("[tag] led %s %s\r\n", argv[1], on ? "on" : "off");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_led, led, an LED on or off);

static int cmd_beep(int argc, char **argv)
{
    int ms = argc == 2 ? atoi(argv[1]) : 200;

    if (ms < 10 || ms > 2000) {
        printf("usage: beep [10..2000 ms]\r\n");
        return -1;
    }
    s_beep_ms = (uint32_t)ms;
    printf("[tag] beep %d ms\r\n", ms);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_beep, beep, a tone on the buzzer);

int main(void)
{
    ipro_platform_init();

    /* Before anything prints: printf, the log and the shell go to the USB port,
     * and what is printed before a PC opens the port waits in the console's buffer. */
    usb_console_init();

    xTaskCreate(sensor_task, "sensors", 1536, NULL, 5, NULL);
    vTaskStartScheduler();
    while (1) {}
}
