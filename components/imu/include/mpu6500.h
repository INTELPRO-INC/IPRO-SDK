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

#ifndef __MPU6500_H__
#define __MPU6500_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* I2C address (AD0 pin: LOW=0x68, HIGH=0x69) */
#define MPU6500_ADDR_AD0_LOW    0x68
#define MPU6500_ADDR_AD0_HIGH   0x69

/* WHO_AM_I expected value */
#define MPU6500_WHO_AM_I_VAL    0x70

/* Accelerometer full-scale range */
typedef enum {
    MPU6500_ACCEL_FS_2G  = 0,   /* ±2g  — 16384 LSB/g */
    MPU6500_ACCEL_FS_4G  = 1,   /* ±4g  — 8192  LSB/g */
    MPU6500_ACCEL_FS_8G  = 2,   /* ±8g  — 4096  LSB/g */
    MPU6500_ACCEL_FS_16G = 3,   /* ±16g — 2048  LSB/g */
} mpu6500_accel_fs_t;

/* Gyroscope full-scale range */
typedef enum {
    MPU6500_GYRO_FS_250  = 0,   /* ±250°/s  — 131   LSB/°/s */
    MPU6500_GYRO_FS_500  = 1,   /* ±500°/s  — 65.5  LSB/°/s */
    MPU6500_GYRO_FS_1000 = 2,   /* ±1000°/s — 32.8  LSB/°/s */
    MPU6500_GYRO_FS_2000 = 3,   /* ±2000°/s — 16.4  LSB/°/s */
} mpu6500_gyro_fs_t;

/* DLPF bandwidth setting (applies to both gyro CONFIG and ACCEL_CONFIG2) */
typedef enum {
    MPU6500_DLPF_250HZ = 0,
    MPU6500_DLPF_184HZ = 1,
    MPU6500_DLPF_92HZ  = 2,
    MPU6500_DLPF_41HZ  = 3,
    MPU6500_DLPF_20HZ  = 4,
    MPU6500_DLPF_10HZ  = 5,
    MPU6500_DLPF_5HZ   = 6,
} mpu6500_dlpf_t;

/* Configuration */
typedef struct {
    uint8_t             i2c_bus;    /* I2C bus index (I2C0_INDEX / I2C1_INDEX) */
    uint8_t             i2c_addr;   /* 7-bit I2C address (0x68 or 0x69) */
    mpu6500_accel_fs_t  accel_fs;   /* Accelerometer full-scale */
    mpu6500_gyro_fs_t   gyro_fs;    /* Gyroscope full-scale */
    mpu6500_dlpf_t      dlpf;       /* Digital low-pass filter bandwidth */
    uint8_t             smplrt_div; /* Sample rate divider: rate = 1kHz / (1 + div) */
} mpu6500_cfg_t;

/* Raw sensor data (16-bit signed, big-endian from device) */
typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temp;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} mpu6500_raw_t;

/* Scaled sensor data (float, SI units) */
typedef struct {
    float accel_x;      /* m/s² */
    float accel_y;
    float accel_z;
    float temp;         /* °C */
    float gyro_x;       /* °/s */
    float gyro_y;
    float gyro_z;
} mpu6500_data_t;

/**
 * @brief  Initialize MPU-6500 with given configuration.
 *         Also configures I2C GPIO pins for the selected bus.
 * @return 0 on success, -1 on error (WHO_AM_I mismatch or I2C failure)
 */
int mpu6500_init(const mpu6500_cfg_t *cfg);

/**
 * @brief  Read WHO_AM_I register (should return 0x70 for MPU-6500).
 * @return WHO_AM_I value, or -1 on I2C error
 */
int mpu6500_who_am_i(void);

/** Read back any register (diagnostics). Returns value 0..255 or -1. */
int mpu6500_reg_get(uint8_t reg);

/**
 * @brief  Read all sensor axes in a single burst (14 bytes).
 * @return 0 on success, -1 on error
 */
int mpu6500_read_raw(mpu6500_raw_t *raw);

/**
 * @brief  Read and convert to physical units (m/s², °C, °/s).
 * @return 0 on success, -1 on error
 */
int mpu6500_read(mpu6500_data_t *data);

/**
 * @brief  Update accelerometer full-scale range at runtime.
 */
int mpu6500_set_accel_fs(mpu6500_accel_fs_t fs);

/**
 * @brief  Update gyroscope full-scale range at runtime.
 */
int mpu6500_set_gyro_fs(mpu6500_gyro_fs_t fs);

/**
 * @brief  Enter low-power sleep mode.
 */
int mpu6500_sleep(void);

/**
 * @brief  Wake up from sleep mode.
 */
int mpu6500_wakeup(void);

/* ------------------------------------------------------------------ */
/* FIFO high-rate sampling API                                        */
/* ------------------------------------------------------------------ */

/* FIFO sample: accel(6) + gyro(6), no temp — 12 bytes per sample.
 * Both sensors run at the same ODR (SMPLRT_DIV governed) so interleave is uniform. */
typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} mpu6500_fifo_sample_t;

/**
 * @brief  Configure MPU-6500 for high-rate FIFO sampling (accel + gyro).
 *         Gyro bypasses DLPF (FCHOICE_B=01 → 8kHz base); accel uses DLPF path
 *         (accel_fchoice_b=0, DLPF_CFG=0 → 250Hz BW, 4kHz internal).
 *         Both sensors are governed by SMPLRT_DIV → same ODR in FIFO.
 * @param  rate_hz  Desired sample rate (1000, 2000, 4000, 8000).
 *                  Actual rate depends on divider rounding.
 * @return Actual sample rate on success, -1 on error
 */
int mpu6500_fifo_start(uint16_t rate_hz);

/**
 * @brief  Stop FIFO sampling, disable FIFO.
 */
int mpu6500_fifo_stop(void);

/**
 * @brief  Read current FIFO byte count and check overflow.
 * @return Number of bytes in FIFO, -1 on I2C error, -2 on FIFO overflow
 *         (caller must call mpu6500_fifo_reset() on -2)
 */
int mpu6500_fifo_count(void);

/**
 * @brief  Read samples from FIFO.
 * @param  buf          Output buffer for samples
 * @param  max_samples  Maximum samples to read
 * @return Number of samples actually read, -1 on error, -2 on overflow
 *         (FIFO is automatically reset on overflow before returning -2)
 */
int mpu6500_fifo_read(mpu6500_fifo_sample_t *buf, uint16_t max_samples);

/**
 * @brief  Reset (flush) the FIFO.
 */
int mpu6500_fifo_reset(void);

/**
 * @brief  Configure MPU-6500 for 7-axis FIFO sampling (accel + temp + gyro).
 *         Each FIFO sample is 14 bytes: ACCEL(6) + TEMP(2) + GYRO(6).
 *         For rate >= 2kHz: gyro FCHOICE_B=01 (8kHz base, no DLPF),
 *                           accel FCHOICE_B=1 (4kHz, no DLPF).
 *         For rate <= 1kHz: both sensors use DLPF path, SMPLRT_DIV governs rate.
 * @param  rate_hz  Desired sample rate in Hz.
 * @return Actual sample rate on success, -1 on error
 */
int mpu6500_fifo_start_full(uint16_t rate_hz);

/**
 * @brief  Read 7-axis samples (14 bytes each) from FIFO into mpu6500_raw_t.
 *         Parse order: ACCEL XYZ (bytes 0-5), TEMP (6-7), GYRO XYZ (8-13).
 * @param  buf          Output buffer for raw samples
 * @param  max_samples  Maximum number of samples to read
 * @return Number of samples actually read, -1 on error
 */
int mpu6500_fifo_read_full(mpu6500_raw_t *buf, uint16_t max_samples);

#ifdef __cplusplus
}
#endif

#endif /* __MPU6500_H__ */
