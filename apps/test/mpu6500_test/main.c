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
 * MPU-6500 (GY-6500) IMU Test Application
 *
 * Shell commands:
 *   mpu_init [bus] [addr]  — Initialize MPU-6500 (default: I2C0, 0x68)
 *   mpu_read               — Single read (accel + gyro + temp)
 *   mpu_stream [count]     — Continuous read N samples (default: 50)
 *   mpu_raw                — Single raw read (16-bit integer values)
 *   mpu_whoami             — Read WHO_AM_I register
 *   mpu_accel_fs <0-3>     — Set accel range (0=2g,1=4g,2=8g,3=16g)
 *   mpu_gyro_fs <0-3>      — Set gyro range (0=250,1=500,2=1000,3=2000)
 *   mpu_hires [seconds]    — 2kHz FIFO sampling, report stats every 1s
 *   mpu_sleep              — Enter sleep mode
 *   mpu_wake               — Wake up
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "mpu6500.h"
#include "vibration_analysis.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "shell.h"
#include "platform_common.h"
#include "hal_boot2.h"
#include "freertos_app_hooks.h"
#include "ipro_log.h"

#define TAG "MPU6500"

static bool s_initialized = false;
static mpu6500_accel_fs_t s_accel_fs;
static mpu6500_gyro_fs_t  s_gyro_fs;

/* ------------------------------------------------------------------ */
/* Shell: mpu_init                                                    */
/* ------------------------------------------------------------------ */
static int cmd_mpu_init(int argc, char **argv)
{
    uint8_t bus  = 0;       /* I2C0 */
    uint8_t addr = 0x68;    /* AD0 = LOW */

    if (argc >= 2) bus  = atoi(argv[1]);
    if (argc >= 3) addr = strtol(argv[2], NULL, 16);

    IPRO_LOGI(TAG, "Init: I2C%d, addr=0x%02X", bus, addr);

    mpu6500_cfg_t cfg = {
        .i2c_bus    = bus,
        .i2c_addr   = addr,
        .accel_fs   = MPU6500_ACCEL_FS_4G,
        .gyro_fs    = MPU6500_GYRO_FS_500,
        .dlpf       = MPU6500_DLPF_92HZ,
        .smplrt_div = 9,    /* 1kHz / (1+9) = 100 Hz */
    };

    int rc = mpu6500_init(&cfg);
    if (rc != 0) {
        IPRO_LOGE(TAG, "Init FAILED (rc=%d) — check wiring and address", rc);
        s_initialized = false;
        return -1;
    }

    s_initialized = true;
    s_accel_fs = cfg.accel_fs;
    s_gyro_fs  = cfg.gyro_fs;
    IPRO_LOGI(TAG, "Init OK — accel=±4g, gyro=±500°/s, rate=100Hz");
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell: mpu_whoami                                                  */
/* ------------------------------------------------------------------ */
static int cmd_mpu_whoami(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (!s_initialized) {
        IPRO_LOGW(TAG, "Not initialized — run mpu_init first");
        return -1;
    }

    int id = mpu6500_who_am_i();
    if (id < 0) {
        IPRO_LOGE(TAG, "I2C read failed");
        return -1;
    }

    printf("WHO_AM_I = 0x%02X (%s)\r\n", id,
           (id == MPU6500_WHO_AM_I_VAL) ? "MPU-6500 OK" : "UNEXPECTED");
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell: mpu_read                                                    */
/* ------------------------------------------------------------------ */
static int cmd_mpu_read(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (!s_initialized) {
        IPRO_LOGW(TAG, "Not initialized — run mpu_init first");
        return -1;
    }

    mpu6500_data_t d;
    if (mpu6500_read(&d) != 0) {
        IPRO_LOGE(TAG, "Read failed");
        return -1;
    }

    printf("Accel: X=%+8.3f  Y=%+8.3f  Z=%+8.3f  m/s²\r\n",
           d.accel_x, d.accel_y, d.accel_z);
    printf("Gyro:  X=%+8.2f  Y=%+8.2f  Z=%+8.2f  °/s\r\n",
           d.gyro_x, d.gyro_y, d.gyro_z);
    printf("Temp:  %.1f °C\r\n", d.temp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell: mpu_raw                                                     */
/* ------------------------------------------------------------------ */
static int cmd_mpu_raw(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (!s_initialized) {
        IPRO_LOGW(TAG, "Not initialized — run mpu_init first");
        return -1;
    }

    mpu6500_raw_t r;
    if (mpu6500_read_raw(&r) != 0) {
        IPRO_LOGE(TAG, "Read failed");
        return -1;
    }

    printf("Raw Accel: X=%+6d  Y=%+6d  Z=%+6d\r\n",
           r.accel_x, r.accel_y, r.accel_z);
    printf("Raw Gyro:  X=%+6d  Y=%+6d  Z=%+6d\r\n",
           r.gyro_x, r.gyro_y, r.gyro_z);
    printf("Raw Temp:  %d\r\n", r.temp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell: mpu_stream                                                  */
/* ------------------------------------------------------------------ */
static int cmd_mpu_stream(int argc, char **argv)
{
    if (!s_initialized) {
        IPRO_LOGW(TAG, "Not initialized — run mpu_init first");
        return -1;
    }

    int count = 50;
    if (argc >= 2) count = atoi(argv[1]);
    if (count <= 0 || count > 10000) count = 50;

    printf("Streaming %d samples (10ms interval)...\r\n", count);
    printf("%6s  %8s %8s %8s  %8s %8s %8s  %6s\r\n",
           "#", "AX", "AY", "AZ", "GX", "GY", "GZ", "T(C)");

    for (int i = 0; i < count; i++) {
        mpu6500_data_t d;
        if (mpu6500_read(&d) != 0) {
            IPRO_LOGE(TAG, "Read failed at sample %d", i);
            return -1;
        }

        printf("%6d  %+8.3f %+8.3f %+8.3f  %+8.2f %+8.2f %+8.2f  %5.1f\r\n",
               i, d.accel_x, d.accel_y, d.accel_z,
               d.gyro_x, d.gyro_y, d.gyro_z, d.temp);

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    printf("Done.\r\n");
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell: mpu_accel_fs / mpu_gyro_fs                                  */
/* ------------------------------------------------------------------ */
static int cmd_mpu_accel_fs(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: mpu_accel_fs <0-3>  (0=2g,1=4g,2=8g,3=16g)\r\n");
        return -1;
    }
    int fs = atoi(argv[1]);
    if (fs < 0 || fs > 3) { printf("Invalid range\r\n"); return -1; }

    static const char *names[] = { "±2g", "±4g", "±8g", "±16g" };
    if (mpu6500_set_accel_fs((mpu6500_accel_fs_t)fs) != 0) {
        IPRO_LOGE(TAG, "Set accel FS failed");
        return -1;
    }
    printf("Accel range: %s\r\n", names[fs]);
    return 0;
}

static int cmd_mpu_gyro_fs(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: mpu_gyro_fs <0-3>  (0=250,1=500,2=1000,3=2000)\r\n");
        return -1;
    }
    int fs = atoi(argv[1]);
    if (fs < 0 || fs > 3) { printf("Invalid range\r\n"); return -1; }

    static const char *names[] = { "±250°/s", "±500°/s", "±1000°/s", "±2000°/s" };
    if (mpu6500_set_gyro_fs((mpu6500_gyro_fs_t)fs) != 0) {
        IPRO_LOGE(TAG, "Set gyro FS failed");
        return -1;
    }
    printf("Gyro range: %s\r\n", names[fs]);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell: mpu_hires — 2kHz FIFO sampling with 1s statistics report    */
/* ------------------------------------------------------------------ */

/* Per-axis statistics accumulator */
typedef struct {
    double sum;
    double sum_sq;
    int16_t min;
    int16_t max;
} axis_stat_t;

static void axis_stat_init(axis_stat_t *s)
{
    s->sum = 0;
    s->sum_sq = 0;
    s->min = INT16_MAX;
    s->max = INT16_MIN;
}

static void axis_stat_add(axis_stat_t *s, int16_t val)
{
    s->sum += val;
    s->sum_sq += (double)val * val;
    if (val < s->min) s->min = val;
    if (val > s->max) s->max = val;
}

/*
 * FIFO drain buffer — 42 samples max per FIFO read (512 / 12).
 * Main sample buffer in PSRAM — 2500 samples per second headroom.
 */
#define HIRES_TARGET_RATE   2000
#define HIRES_BUF_SIZE      2500
#define FIFO_CHUNK          42

static mpu6500_fifo_sample_t s_hires_buf[HIRES_BUF_SIZE] __attribute__((section(".psram_data")));

static int cmd_mpu_hires(int argc, char **argv)
{
    if (!s_initialized) {
        IPRO_LOGW(TAG, "Not initialized — run mpu_init first");
        return -1;
    }

    int seconds = 10;
    if (argc >= 2) seconds = atoi(argv[1]);
    if (seconds <= 0 || seconds > 3600) seconds = 10;

    /* Start FIFO at 2kHz */
    int actual_rate = mpu6500_fifo_start(HIRES_TARGET_RATE);
    if (actual_rate < 0) {
        IPRO_LOGE(TAG, "FIFO start failed");
        return -1;
    }

    /* Sensitivity LUTs for raw → physical unit conversion */
    static const float accel_sens[] = { 16384.0f, 8192.0f, 4096.0f, 2048.0f };
    static const float gyro_sens[]  = { 131.0f, 65.5f, 32.8f, 16.4f };
    float a_scale = 9.80665f / accel_sens[s_accel_fs & 0x03];  /* raw → m/s² */
    float g_scale = 1.0f / gyro_sens[s_gyro_fs & 0x03];        /* raw → °/s  */

    printf("FIFO sampling at %d Hz, reporting every 1s for %ds\r\n",
           actual_rate, seconds);
    printf("%-4s %5s  "
           "%8s %8s %8s  "
           "%8s %8s %8s\r\n",
           "sec", "N",
           "AX(m/s²)", "AY(m/s²)", "AZ(m/s²)",
           "GX(°/s)", "GY(°/s)", "GZ(°/s)");
    printf("%-4s %5s  "
           "%8s %8s %8s  "
           "%8s %8s %8s\r\n",
           "", "",
           "mean", "mean", "mean",
           "rms", "rms", "rms");

    for (int sec = 0; sec < seconds; sec++) {
        int total_samples = 0;
        axis_stat_t ax, ay, az, gx, gy, gz;
        axis_stat_init(&ax); axis_stat_init(&ay); axis_stat_init(&az);
        axis_stat_init(&gx); axis_stat_init(&gy); axis_stat_init(&gz);

        mpu6500_fifo_reset();

        /* Collect for 1 second, draining FIFO every 10ms */
        TickType_t start = xTaskGetTickCount();
        while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(1000)) {
            vTaskDelay(pdMS_TO_TICKS(10));

            int n = mpu6500_fifo_read(s_hires_buf, HIRES_BUF_SIZE);
            if (n < 0) {
                IPRO_LOGE(TAG, "FIFO read error");
                mpu6500_fifo_stop();
                return -1;
            }

            for (int i = 0; i < n; i++) {
                axis_stat_add(&ax, s_hires_buf[i].accel_x);
                axis_stat_add(&ay, s_hires_buf[i].accel_y);
                axis_stat_add(&az, s_hires_buf[i].accel_z);
                axis_stat_add(&gx, s_hires_buf[i].gyro_x);
                axis_stat_add(&gy, s_hires_buf[i].gyro_y);
                axis_stat_add(&gz, s_hires_buf[i].gyro_z);
            }
            total_samples += n;
        }

        if (total_samples == 0) {
            printf("%3d  %5d  (no data)\r\n", sec + 1, 0);
            continue;
        }

        double cnt = total_samples;

        /* Convert raw stats to physical units */
        float ax_mean = (float)(ax.sum / cnt) * a_scale;
        float ay_mean = (float)(ay.sum / cnt) * a_scale;
        float az_mean = (float)(az.sum / cnt) * a_scale;
        float gx_rms  = sqrtf((float)(gx.sum_sq / cnt)) * g_scale;
        float gy_rms  = sqrtf((float)(gy.sum_sq / cnt)) * g_scale;
        float gz_rms  = sqrtf((float)(gz.sum_sq / cnt)) * g_scale;

        printf("%3d  %5d  "
               "%+8.3f %+8.3f %+8.3f  "
               "%8.3f %8.3f %8.3f\r\n",
               sec + 1, total_samples,
               ax_mean, ay_mean, az_mean,
               gx_rms, gy_rms, gz_rms);
    }

    mpu6500_fifo_stop();
    printf("Done.\r\n");
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell: mpu_phm — PHM vibration analysis (2kHz, 1s window)         */
/* ------------------------------------------------------------------ */

/* Separate per-axis buffers for vib_compute (PSRAM) */
static int16_t s_ax_buf[HIRES_BUF_SIZE] __attribute__((section(".psram_data")));
static int16_t s_ay_buf[HIRES_BUF_SIZE] __attribute__((section(".psram_data")));
static int16_t s_az_buf[HIRES_BUF_SIZE] __attribute__((section(".psram_data")));

static int phm_collect_one_second(void)
{
    int total = 0;
    mpu6500_fifo_reset();

    TickType_t start = xTaskGetTickCount();
    while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(1000)) {
        vTaskDelay(pdMS_TO_TICKS(10));

        int n = mpu6500_fifo_read(s_hires_buf, HIRES_BUF_SIZE - total);
        if (n < 0) return -1;

        /* De-interleave into per-axis buffers */
        for (int i = 0; i < n && (total + i) < HIRES_BUF_SIZE; i++) {
            s_ax_buf[total + i] = s_hires_buf[i].accel_x;
            s_ay_buf[total + i] = s_hires_buf[i].accel_y;
            s_az_buf[total + i] = s_hires_buf[i].accel_z;
        }
        total += n;
    }
    return total;
}

static int cmd_mpu_phm(int argc, char **argv)
{
    if (!s_initialized) {
        IPRO_LOGW(TAG, "Not initialized — run mpu_init first");
        return -1;
    }

    int seconds = 10;
    bool csv_mode = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-csv") == 0) csv_mode = true;
        else seconds = atoi(argv[i]);
    }
    if (seconds <= 0 || seconds > 3600) seconds = 10;

    /* Sensitivity scale: raw → m/s² */
    static const float accel_sens[] = { 16384.0f, 8192.0f, 4096.0f, 2048.0f };
    float accel_scale = 9.80665f / accel_sens[s_accel_fs & 0x03];

    /* Init vibration analysis */
    if (vib_init(NULL) != 0) {
        IPRO_LOGE(TAG, "vib_init failed");
        return -1;
    }

    /* Start 2kHz FIFO */
    int actual_rate = mpu6500_fifo_start(HIRES_TARGET_RATE);
    if (actual_rate < 0) {
        IPRO_LOGE(TAG, "FIFO start failed");
        return -1;
    }

    printf("PHM monitoring: %d Hz, %ds%s\r\n",
           actual_rate, seconds, csv_mode ? " (CSV)" : "");

    if (csv_mode)
        vib_print_csv_header();

    for (int sec = 0; sec < seconds; sec++) {
        int n = phm_collect_one_second();
        if (n < 0) {
            IPRO_LOGE(TAG, "Collection error at sec %d", sec + 1);
            mpu6500_fifo_stop();
            return -1;
        }

        vib_features_t feat;
        feat.timestamp = xTaskGetTickCount();
        vib_compute(s_ax_buf, s_ay_buf, s_az_buf, n, accel_scale, &feat);

        if (csv_mode) {
            vib_print_csv(&feat);
        } else {
            printf("\n--- Second %d ---\r\n", sec + 1);
            vib_print_summary(&feat);
        }
    }

    mpu6500_fifo_stop();
    printf("PHM Done.\r\n");
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell: mpu_sleep / mpu_wake                                        */
/* ------------------------------------------------------------------ */
static int cmd_mpu_sleep(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (mpu6500_sleep() != 0) { IPRO_LOGE(TAG, "Sleep failed"); return -1; }
    printf("MPU-6500 sleeping\r\n");
    return 0;
}

static int cmd_mpu_wake(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (mpu6500_wakeup() != 0) { IPRO_LOGE(TAG, "Wake failed"); return -1; }
    printf("MPU-6500 awake\r\n");
    return 0;
}

/* ------------------------------------------------------------------ */
/* Shell registration                                                 */
/* ------------------------------------------------------------------ */
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_init,     mpu_init,     Init MPU-6500);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_whoami,   mpu_whoami,   Read WHO_AM_I);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_read,     mpu_read,     Read accel+gyro+temp);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_raw,      mpu_raw,      Read raw 16-bit values);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_stream,   mpu_stream,   Stream N samples);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_hires,   mpu_hires,    2kHz FIFO sampling);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_phm,    mpu_phm,     PHM vibration analysis);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_accel_fs, mpu_accel_fs, Set accel full-scale);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_gyro_fs,  mpu_gyro_fs,  Set gyro full-scale);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_sleep,    mpu_sleep,    Enter sleep mode);
SHELL_CMD_EXPORT_ALIAS(cmd_mpu_wake,     mpu_wake,     Wake from sleep);

/* ------------------------------------------------------------------ */
/* Main                                                               */
/* ------------------------------------------------------------------ */
static void main_task(void *pvParameters)
{
    (void)pvParameters;

    printf("\r\n========================================\r\n");
    printf("  MPU-6500 (GY-6500) IMU Test\r\n");
    printf("========================================\r\n");
    printf("Commands:\r\n");
    printf("  mpu_init [bus] [addr]   — default I2C0, 0x68\r\n");
    printf("  mpu_read                — single sample\r\n");
    printf("  mpu_stream [count]      — continuous (default 50)\r\n");
    printf("  mpu_raw                 — raw 16-bit values\r\n");
    printf("  mpu_whoami              — device ID check\r\n");
    printf("  mpu_accel_fs <0-3>      — 2/4/8/16g\r\n");
    printf("  mpu_gyro_fs <0-3>       — 250/500/1000/2000\r\n");
    printf("  mpu_hires [seconds]     — 2kHz FIFO (default 10s)\r\n");
    printf("  mpu_phm [seconds] [-csv] — PHM features\r\n");
    printf("  mpu_sleep / mpu_wake\r\n");
    printf("========================================\r\n");

    shell_init_with_task(UART0_INDEX);
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_setup_heap();
    platform_init(0);
    hal_boot2_init();

    xTaskCreate(main_task, "main", 2048, NULL,
                configMAX_PRIORITIES - 1, NULL);

    vTaskStartScheduler();

    while (1) ;
    return 0;
}
