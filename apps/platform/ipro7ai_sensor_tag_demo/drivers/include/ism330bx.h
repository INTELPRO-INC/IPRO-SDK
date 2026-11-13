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

/**
 * @file ism330bx.h
 * @brief STMicroelectronics ISM330BX driver (bring-up subset)
 *
 * Datasheet: DS14469 — iNEMO 6DoF IMU with embedded ML core.
 * ODRs up to 3.84 kHz (accelerometer + gyroscope), SPI (Mode 3).
 *
 * This is the bring-up subset: init, WHO_AM_I, single-shot 6-axis reads,
 * soft reset. FIFO/tag-mode support added once basic SPI channel is verified.
 */

#ifndef ISM330BX_H
#define ISM330BX_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Register addresses (subset) ---- */
#define ISM330BX_REG_WHO_AM_I        0x0F
#define ISM330BX_REG_CTRL1           0x10  /* ODR_XL[7:4] + OP_MODE_XL[3:0] */
#define ISM330BX_REG_CTRL2           0x11  /* ODR_G[7:4] + OP_MODE_G[3:0] */
#define ISM330BX_REG_CTRL3           0x12  /* BDU, IF_INC, SW_RESET */
#define ISM330BX_REG_CTRL6           0x15  /* FS_G[3:0] */
#define ISM330BX_REG_CTRL8           0x17  /* FS_XL[1:0] */
#define ISM330BX_REG_OUTX_L_G        0x22  /* Gyro output, 6 bytes */
#define ISM330BX_REG_OUTX_L_A        0x28  /* Accel output, 6 bytes */
#define ISM330BX_REG_FIFO_CTRL1      0x07
#define ISM330BX_REG_FIFO_CTRL2      0x08
#define ISM330BX_REG_FIFO_CTRL3      0x09  /* BDR_GY[7:4] + BDR_XL[3:0] */
#define ISM330BX_REG_FIFO_CTRL4      0x0A  /* FIFO_MODE[2:0] */
#define ISM330BX_REG_FIFO_STATUS1    0x1B
#define ISM330BX_REG_FIFO_STATUS2    0x1C
#define ISM330BX_REG_FIFO_DATA_OUT_TAG 0x78

#define ISM330BX_WHO_AM_I_VAL        0x71

/* ---- Accel ODR (CTRL1[7:4]) ---- */
typedef enum {
    ISM330BX_ODR_XL_OFF  = 0x0,
    ISM330BX_ODR_XL_1_875 = 0x1,
    ISM330BX_ODR_XL_7_5  = 0x2,
    ISM330BX_ODR_XL_15   = 0x3,
    ISM330BX_ODR_XL_30   = 0x4,
    ISM330BX_ODR_XL_60   = 0x5,
    ISM330BX_ODR_XL_120  = 0x6,
    ISM330BX_ODR_XL_240  = 0x7,
    ISM330BX_ODR_XL_480  = 0x8,
    ISM330BX_ODR_XL_960  = 0x9,
    ISM330BX_ODR_XL_1920 = 0xA,   /* default for "2 kHz" target */
    ISM330BX_ODR_XL_3840 = 0xB,
} ism330bx_odr_xl_t;

/* ---- Gyro ODR (CTRL2[7:4]) — same ladder as XL ---- */
typedef ism330bx_odr_xl_t ism330bx_odr_g_t;

/* ---- Full scales ---- */
typedef enum {
    ISM330BX_FS_XL_2G  = 0,
    ISM330BX_FS_XL_4G  = 1,
    ISM330BX_FS_XL_8G  = 2,
    ISM330BX_FS_XL_16G = 3,
} ism330bx_fs_xl_t;

typedef enum {
    ISM330BX_FS_G_125DPS  = 0x0,
    ISM330BX_FS_G_250DPS  = 0x1,
    ISM330BX_FS_G_500DPS  = 0x2,
    ISM330BX_FS_G_1000DPS = 0x3,
    ISM330BX_FS_G_2000DPS = 0x4,
    ISM330BX_FS_G_4000DPS = 0xC,
} ism330bx_fs_g_t;

/* Combined 6-axis sample */
typedef struct {
    int16_t gx, gy, gz;
    int16_t ax, ay, az;
} ism330bx_sample_t;

/**
 * Initialize the ISM330BX. WHO_AM_I check + soft reset + BDU/IF_INC.
 * Leaves ODRs at power-down; caller calls ism330bx_start() to begin.
 */
int ism330bx_init(uint8_t spi_id, uint8_t cs_gpio,
                  ism330bx_fs_xl_t fs_xl, ism330bx_fs_g_t fs_g);

int ism330bx_who_am_i(uint8_t *out);

/** Start with accelerometer + gyro both at the given ODR (1920 Hz by default). */
int ism330bx_start(ism330bx_odr_xl_t odr);

/** Stop sampling (ODRs → off). */
int ism330bx_stop(void);

/** Single 6-axis sample read. */
int ism330bx_read_sample(ism330bx_sample_t *s);

/* --------------------------------------------------------------------
 * FIFO streaming API
 * -------------------------------------------------------------------- */

/**
 * Start FIFO at given ODR for both XL and gyro with the supplied watermark.
 *   watermark: 1..512 samples (0 → default 32).
 * Returns 0 on success.
 */
int ism330bx_fifo_start(ism330bx_odr_xl_t odr, uint16_t watermark);

int ism330bx_fifo_stop(void);

/** Read up to `max` 6-axis samples from the FIFO. Returns count (>=0) or <0. */
int ism330bx_fifo_read_batch(ism330bx_sample_t *buf, uint16_t max);

int ism330bx_fifo_level(void);
int ism330bx_fifo_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* ISM330BX_H */
