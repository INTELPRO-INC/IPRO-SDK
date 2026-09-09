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

#include <generated/autoconf.h>
#include <stdbool.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "mpu6500.h"
#include "hal_i2c.h"
#include "drv_glb.h"

/* ------------------------------------------------------------------ */
/* MPU-6500 Register Map                                              */
/* ------------------------------------------------------------------ */
#define REG_SMPLRT_DIV      0x19
#define REG_CONFIG          0x1A
#define REG_GYRO_CONFIG     0x1B
#define REG_ACCEL_CONFIG    0x1C
#define REG_ACCEL_CONFIG2   0x1D
#define REG_FIFO_EN         0x23
#define REG_INT_PIN_CFG     0x37
#define REG_INT_ENABLE      0x38
#define REG_ACCEL_XOUT_H    0x3B    /* 14 bytes: accel(6) + temp(2) + gyro(6) */
#define REG_SIGNAL_PATH_RST 0x68
#define REG_USER_CTRL       0x6A
#define REG_FIFO_COUNT_H    0x72
#define REG_FIFO_R_W        0x74
#define REG_PWR_MGMT_1      0x6B
#define REG_PWR_MGMT_2      0x6C
#define REG_WHO_AM_I        0x75

/* PWR_MGMT_1 bits */
#define BIT_H_RESET         0x80
#define BIT_SLEEP           0x40
#define BIT_CLKSEL_AUTO     0x01    /* PLL with auto clock source */

/* USER_CTRL bits */
#define BIT_FIFO_EN         0x40
#define BIT_FIFO_RST        0x04

/* FIFO_EN bits */
#define BIT_FIFO_ACCEL      0x08
#define BIT_FIFO_GYRO_ALL   0x70    /* XG + YG + ZG */
#define BIT_FIFO_TEMP       0x80

/* FIFO constants */
#define MPU6500_FIFO_SIZE       512
#define FIFO_SAMPLE_BYTES       6   /* accel only: [AX_H AX_L AY_H AY_L AZ_H AZ_L] */
#define FIFO_SAMPLE_BYTES_FULL  14  /* accel(6) + temp(2) + gyro(6) */

/* INT_STATUS register and FIFO overflow bit */
#define REG_INT_STATUS      0x3A
#define BIT_FIFO_OVERFLOW   0x10

/* Gravity constant for m/s² conversion */
#define GRAVITY_MS2         9.80665f

/* ------------------------------------------------------------------ */
/* Module state                                                       */
/* ------------------------------------------------------------------ */
static struct {
    uint8_t             i2c_bus;
    uint8_t             i2c_addr;
    mpu6500_accel_fs_t  accel_fs;
    mpu6500_gyro_fs_t   gyro_fs;
    bool                initialized;
} s_mpu;

/* ------------------------------------------------------------------ */
/* I2C helpers                                                        */
/* ------------------------------------------------------------------ */
static int reg_write(uint8_t reg, uint8_t val)
{
    i2c_msg_t msg = {
        .slaveaddr = s_mpu.i2c_addr,
        .flags     = SUB_ADDR_1BYTE | I2C_WR,
        .subaddr   = reg,
        .buf       = &val,
        .len       = 1,
    };
    return i2c_transfer(s_mpu.i2c_bus, &msg, 1);
}

static int reg_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
    i2c_msg_t msg = {
        .slaveaddr = s_mpu.i2c_addr,
        .flags     = SUB_ADDR_1BYTE | I2C_RD,
        .subaddr   = reg,
        .buf       = buf,
        .len       = len,
    };
    return i2c_transfer(s_mpu.i2c_bus, &msg, 1);
}

/* ------------------------------------------------------------------ */
/* I2C GPIO setup                                                     */
/* ------------------------------------------------------------------ */

/* Default GPIO mapping — override via Kconfig if needed */
#ifndef CONFIG_MPU6500_GPIO_SDA
#define CONFIG_MPU6500_GPIO_SDA 3
#endif
#ifndef CONFIG_MPU6500_GPIO_SCL
#define CONFIG_MPU6500_GPIO_SCL 2
#endif

static void i2c_gpio_init(uint8_t bus)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive    = 0,
        .smtCtrl  = 1,
        .gpioFun  = (bus == 0) ? GPIO_FUN_I2C : GPIO_FUN_I2C1,
    };

    uint8_t pins[2] = { CONFIG_MPU6500_GPIO_SDA, CONFIG_MPU6500_GPIO_SCL };
    for (int i = 0; i < 2; i++) {
        cfg.gpioPin = pins[i];
        GLB_GPIO_Init(&cfg);
    }
}

/* ------------------------------------------------------------------ */
/* Sensitivity LUTs                                                   */
/* ------------------------------------------------------------------ */
static float accel_sensitivity(mpu6500_accel_fs_t fs)
{
    static const float lut[] = { 16384.0f, 8192.0f, 4096.0f, 2048.0f };
    return lut[fs & 0x03];
}

static float gyro_sensitivity(mpu6500_gyro_fs_t fs)
{
    static const float lut[] = { 131.0f, 65.5f, 32.8f, 16.4f };
    return lut[fs & 0x03];
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */
int mpu6500_init(const mpu6500_cfg_t *cfg)
{
    if (!cfg)
        return -1;

    s_mpu.i2c_bus  = cfg->i2c_bus;
    s_mpu.i2c_addr = cfg->i2c_addr;
    s_mpu.accel_fs = cfg->accel_fs;
    s_mpu.gyro_fs  = cfg->gyro_fs;

    /* Setup I2C GPIO and peripheral */
    i2c_gpio_init(cfg->i2c_bus);
    i2c_init(cfg->i2c_bus);

    /* Reset device */
    if (reg_write(REG_PWR_MGMT_1, BIT_H_RESET) != 0)
        return -1;

    /* Wait for reset (datasheet: 100ms max) */
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Wake up, select auto clock source */
    if (reg_write(REG_PWR_MGMT_1, BIT_CLKSEL_AUTO) != 0)
        return -1;

    /* Verify WHO_AM_I */
    int id = mpu6500_who_am_i();
    if (id != MPU6500_WHO_AM_I_VAL)
        return -1;

    /* Enable all axes */
    reg_write(REG_PWR_MGMT_2, 0x00);

    /* Sample rate divider */
    reg_write(REG_SMPLRT_DIV, cfg->smplrt_div);

    /* DLPF config */
    reg_write(REG_CONFIG, cfg->dlpf & 0x07);
    reg_write(REG_ACCEL_CONFIG2, cfg->dlpf & 0x07);

    /* Full-scale ranges */
    reg_write(REG_GYRO_CONFIG, (cfg->gyro_fs & 0x03) << 3);
    reg_write(REG_ACCEL_CONFIG, (cfg->accel_fs & 0x03) << 3);

    s_mpu.initialized = true;
    return 0;
}

int mpu6500_reg_get(uint8_t reg)
{
    uint8_t val;
    if (reg_read(reg, &val, 1) != 0)
        return -1;
    return val;
}

int mpu6500_who_am_i(void)
{
    uint8_t val;
    if (reg_read(REG_WHO_AM_I, &val, 1) != 0)
        return -1;
    return val;
}

int mpu6500_read_raw(mpu6500_raw_t *raw)
{
    uint8_t buf[14];

    if (!raw || !s_mpu.initialized)
        return -1;

    /* Burst read: ACCEL(6) + TEMP(2) + GYRO(6) = 14 bytes */
    if (reg_read(REG_ACCEL_XOUT_H, buf, 14) != 0)
        return -1;

    raw->accel_x = (int16_t)((buf[0]  << 8) | buf[1]);
    raw->accel_y = (int16_t)((buf[2]  << 8) | buf[3]);
    raw->accel_z = (int16_t)((buf[4]  << 8) | buf[5]);
    raw->temp    = (int16_t)((buf[6]  << 8) | buf[7]);
    raw->gyro_x  = (int16_t)((buf[8]  << 8) | buf[9]);
    raw->gyro_y  = (int16_t)((buf[10] << 8) | buf[11]);
    raw->gyro_z  = (int16_t)((buf[12] << 8) | buf[13]);

    return 0;
}

int mpu6500_read(mpu6500_data_t *data)
{
    mpu6500_raw_t raw;

    if (!data)
        return -1;

    if (mpu6500_read_raw(&raw) != 0)
        return -1;

    float a_sens = accel_sensitivity(s_mpu.accel_fs);
    float g_sens = gyro_sensitivity(s_mpu.gyro_fs);

    data->accel_x = (raw.accel_x / a_sens) * GRAVITY_MS2;
    data->accel_y = (raw.accel_y / a_sens) * GRAVITY_MS2;
    data->accel_z = (raw.accel_z / a_sens) * GRAVITY_MS2;
    data->temp    = (raw.temp / 333.87f) + 21.0f;
    data->gyro_x  = raw.gyro_x / g_sens;
    data->gyro_y  = raw.gyro_y / g_sens;
    data->gyro_z  = raw.gyro_z / g_sens;

    return 0;
}

int mpu6500_set_accel_fs(mpu6500_accel_fs_t fs)
{
    if (reg_write(REG_ACCEL_CONFIG, (fs & 0x03) << 3) != 0)
        return -1;
    s_mpu.accel_fs = fs;
    return 0;
}

int mpu6500_set_gyro_fs(mpu6500_gyro_fs_t fs)
{
    if (reg_write(REG_GYRO_CONFIG, (fs & 0x03) << 3) != 0)
        return -1;
    s_mpu.gyro_fs = fs;
    return 0;
}

int mpu6500_sleep(void)
{
    return reg_write(REG_PWR_MGMT_1, BIT_SLEEP | BIT_CLKSEL_AUTO);
}

int mpu6500_wakeup(void)
{
    return reg_write(REG_PWR_MGMT_1, BIT_CLKSEL_AUTO);
}

/* ------------------------------------------------------------------ */
/* FIFO high-rate sampling                                            */
/* ------------------------------------------------------------------ */
int mpu6500_fifo_start(uint16_t rate_hz)
{
    if (!s_mpu.initialized)
        return -1;

    /*
     * Accel-only FIFO mode.  Gyro is NOT pushed to FIFO — PHM only uses accel.
     *
     * With accel_fchoice_b=1, accel bypasses DLPF and runs at a fixed 4kHz,
     * independent of SMPLRT_DIV.  This is fine because there is no gyro in the
     * FIFO to get out of sync.  6 bytes/sample × 4kHz = 24KB/s; the 512-byte
     * FIFO holds ~21ms of data, so the 5ms read interval is sufficient.
     *
     * If a lower rate is requested, use accel_fchoice_b=0 (DLPF enabled,
     * 1kHz base) with SMPLRT_DIV.
     */
    int actual;
    if (rate_hz >= 2000) {
        /* 4kHz accel — bypass DLPF (accel_fchoice_b=1) */
        reg_write(REG_ACCEL_CONFIG2, 0x08);
        actual = 4000;
    } else {
        /* 1kHz accel — DLPF enabled (accel_fchoice_b=0), DLPF_CFG=0 (460Hz BW) */
        reg_write(REG_ACCEL_CONFIG2, 0x00);
        uint8_t div = (rate_hz >= 1000) ? 0 : (uint8_t)(1000 / rate_hz - 1);
        reg_write(REG_SMPLRT_DIV, div);
        actual = 1000 / (1 + div);
    }

    /* Reset FIFO first */
    reg_write(REG_USER_CTRL, BIT_FIFO_RST);
    vTaskDelay(pdMS_TO_TICKS(1));

    /* Enable FIFO for accel only (no gyro, no temp) */
    reg_write(REG_FIFO_EN, BIT_FIFO_ACCEL);

    /* Clear any latched overflow flag */
    uint8_t dummy;
    reg_read(REG_INT_STATUS, &dummy, 1);

    /* Enable FIFO in USER_CTRL */
    reg_write(REG_USER_CTRL, BIT_FIFO_EN);

    return actual;
}

int mpu6500_fifo_stop(void)
{
    /* Disable FIFO data sources */
    reg_write(REG_FIFO_EN, 0x00);
    /* Disable FIFO + reset */
    reg_write(REG_USER_CTRL, BIT_FIFO_RST);
    return 0;
}

int mpu6500_fifo_count(void)
{
    uint8_t buf[2];
    if (reg_read(REG_FIFO_COUNT_H, buf, 2) != 0)
        return -1;

    return (buf[0] << 8) | buf[1];
}

int mpu6500_fifo_read(mpu6500_fifo_sample_t *buf, uint16_t max_samples)
{
    if (!buf)
        return -1;

    int fifo_bytes = mpu6500_fifo_count();
    if (fifo_bytes <= 0)
        return (fifo_bytes < 0) ? -1 : 0;

    int avail = fifo_bytes / FIFO_SAMPLE_BYTES;
    if (avail > max_samples)
        avail = max_samples;

    /* Read accel-only FIFO: [AX_H AX_L AY_H AY_L AZ_H AZ_L] per sample */
    for (int i = 0; i < avail; i++) {
        uint8_t raw[FIFO_SAMPLE_BYTES];
        if (reg_read(REG_FIFO_R_W, raw, FIFO_SAMPLE_BYTES) != 0)
            return i;   /* return what we got so far */

        buf[i].accel_x = (int16_t)((raw[0] << 8) | raw[1]);
        buf[i].accel_y = (int16_t)((raw[2] << 8) | raw[3]);
        buf[i].accel_z = (int16_t)((raw[4] << 8) | raw[5]);
        buf[i].gyro_x  = 0;
        buf[i].gyro_y  = 0;
        buf[i].gyro_z  = 0;
    }

    return avail;
}

int mpu6500_fifo_reset(void)
{
    uint8_t val;
    if (reg_read(REG_USER_CTRL, &val, 1) != 0)
        return -1;
    int rc = reg_write(REG_USER_CTRL, val | BIT_FIFO_RST);
    /* Clear latched overflow flag in INT_STATUS (read-to-clear register) */
    reg_read(REG_INT_STATUS, &val, 1);
    return rc;
}

/* ------------------------------------------------------------------ */
/* 7-axis FIFO (accel + temp + gyro)                                   */
/* ------------------------------------------------------------------ */
int mpu6500_fifo_start_full(uint16_t rate_hz)
{
    if (!s_mpu.initialized)
        return -1;

    /*
     * 7-axis FIFO: accel + temp + gyro → 14 bytes/sample.
     *
     * For rate >= 2kHz:
     *   - Gyro: FCHOICE_B = 01 in GYRO_CONFIG → 8kHz base, DLPF bypassed
     *   - Accel: accel_fchoice_b = 1 in ACCEL_CONFIG2 → 4kHz, DLPF bypassed
     *   - SMPLRT_DIV is irrelevant (both sensors free-run)
     *   - Effective rate is 4kHz (limited by accel)
     *
     * For rate <= 1kHz:
     *   - Gyro: FCHOICE_B = 00, DLPF governs (1kHz base), SMPLRT_DIV applies
     *   - Accel: accel_fchoice_b = 0, DLPF governs (1kHz base), SMPLRT_DIV applies
     *   - Both sensors share the same SMPLRT_DIV → synchronized in FIFO
     */
    /*
     * 7-axis FIFO requires all sensors at the same sample rate.
     * Must use DLPF path (1kHz base) — bypassing DLPF gives different
     * rates for gyro (8kHz) vs accel (4kHz) which breaks FIFO sync.
     * Max effective rate: 1kHz.  Clamp rate_hz to 1000.
     */
    if (rate_hz > 1000)
        rate_hz = 1000;

    /* Gyro: FCHOICE_B = 00 (use DLPF), DLPF_CFG = 0 (250Hz BW, 1kHz base) */
    uint8_t gyro_cfg = (s_mpu.gyro_fs & 0x03) << 3;
    reg_write(REG_GYRO_CONFIG, gyro_cfg);
    reg_write(REG_CONFIG, 0x00);

    /* Accel: accel_fchoice_b = 0 (use DLPF), DLPF_CFG = 0 (460Hz BW, 1kHz base) */
    reg_write(REG_ACCEL_CONFIG2, 0x00);

    uint8_t div = (rate_hz >= 1000) ? 0 : (uint8_t)(1000 / rate_hz - 1);
    reg_write(REG_SMPLRT_DIV, div);
    int actual = 1000 / (1 + div);

    /* Reset FIFO */
    reg_write(REG_USER_CTRL, BIT_FIFO_RST);
    vTaskDelay(pdMS_TO_TICKS(1));

    /* Enable FIFO for accel + temp + gyro */
    reg_write(REG_FIFO_EN, BIT_FIFO_ACCEL | BIT_FIFO_GYRO_ALL | BIT_FIFO_TEMP);

    /* Clear any latched overflow flag */
    uint8_t dummy;
    reg_read(REG_INT_STATUS, &dummy, 1);

    /* Enable FIFO in USER_CTRL */
    reg_write(REG_USER_CTRL, BIT_FIFO_EN);

    return actual;
}

int mpu6500_fifo_read_full(mpu6500_raw_t *buf, uint16_t max_samples)
{
    if (!buf)
        return -1;

    int fifo_bytes = mpu6500_fifo_count();
    if (fifo_bytes <= 0)
        return (fifo_bytes < 0) ? -1 : 0;

    int avail = fifo_bytes / FIFO_SAMPLE_BYTES_FULL;
    if (avail > max_samples)
        avail = max_samples;

    /* Read 14-byte samples: ACCEL(6) + TEMP(2) + GYRO(6) */
    for (int i = 0; i < avail; i++) {
        uint8_t raw[FIFO_SAMPLE_BYTES_FULL];
        if (reg_read(REG_FIFO_R_W, raw, FIFO_SAMPLE_BYTES_FULL) != 0)
            return i;   /* return what we got so far */

        buf[i].accel_x = (int16_t)((raw[0]  << 8) | raw[1]);
        buf[i].accel_y = (int16_t)((raw[2]  << 8) | raw[3]);
        buf[i].accel_z = (int16_t)((raw[4]  << 8) | raw[5]);
        buf[i].temp    = (int16_t)((raw[6]  << 8) | raw[7]);
        buf[i].gyro_x  = (int16_t)((raw[8]  << 8) | raw[9]);
        buf[i].gyro_y  = (int16_t)((raw[10] << 8) | raw[11]);
        buf[i].gyro_z  = (int16_t)((raw[12] << 8) | raw[13]);
    }

    return avail;
}
