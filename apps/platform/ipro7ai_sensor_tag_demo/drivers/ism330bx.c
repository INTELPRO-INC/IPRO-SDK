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
 * @file ism330bx.c
 * @brief ISM330BX driver — bring-up subset
 */

#include "ism330bx.h"
#include "spi_bitbang.h"

#include <FreeRTOS.h>
#include <task.h>

#include <string.h>

/* Internal state */
static uint8_t           s_spi_id;
static uint8_t           s_cs_gpio;
static ism330bx_fs_xl_t  s_fs_xl;
static ism330bx_fs_g_t   s_fs_g;
static bool              s_initialized;

static int rd(uint8_t reg, uint8_t *buf, uint32_t len)
{
    return spi_bitbang_read(s_spi_id, reg, buf, len);
}

static int wr1(uint8_t reg, uint8_t val)
{
    return spi_bitbang_write(s_spi_id, reg, &val, 1);
}

/* CTRL1 / CTRL2 layout on LSM6DSV16BX (= ISM330BX industrial):
 *   bits 3:0  ODR_XL / ODR_G       (0xA = 1920 Hz)
 *   bits 6:4  OP_MODE_XL / OP_MODE_G (0x0 = High-performance)
 *   bit  7    reserved
 * FS bits live in CTRL6 (gyro) and CTRL8 (accel).
 *
 * Earlier versions of this driver placed ODR in the high nibble (LSM6DSOX
 * layout). That left the ODR field at 0 → sensor stayed powered down and
 * no FIFO words were ever produced.
 */
#define CTRL_VAL(odr_nibble, op_mode_nibble) \
    (((odr_nibble) & 0x0F) | (((op_mode_nibble) & 0x07) << 4))

#define OP_MODE_HIGH_PERF  0x0

/* CTRL3: SW_RESET bit 0, IF_INC bit 2, BDU bit 6, BOOT bit 7 */
#define CTRL3_SW_RESET  (1 << 0)
#define CTRL3_IF_INC    (1 << 2)
#define CTRL3_BDU       (1 << 6)
#define CTRL3_BOOT      (1 << 7)

int ism330bx_init(uint8_t spi_id, uint8_t cs_gpio,
                  ism330bx_fs_xl_t fs_xl, ism330bx_fs_g_t fs_g)
{
    s_spi_id      = spi_id;
    s_cs_gpio     = cs_gpio;
    s_fs_xl       = fs_xl;
    s_fs_g        = fs_g;
    s_initialized = false;

    /* 1. WHO_AM_I */
    uint8_t id = 0;
    int rc = rd(ISM330BX_REG_WHO_AM_I, &id, 1);
    if (rc != 0) return -2;
    if (id != ISM330BX_WHO_AM_I_VAL) return -3;

    /* 2. Soft reset, wait for clear */
    rc = wr1(ISM330BX_REG_CTRL3, CTRL3_SW_RESET | CTRL3_IF_INC);
    if (rc != 0) return -2;
    for (int i = 0; i < 10; i++) {
        vTaskDelay(1);
        uint8_t v = 0;
        if (rd(ISM330BX_REG_CTRL3, &v, 1) == 0 && (v & CTRL3_SW_RESET) == 0)
            break;
    }

    /* 3. IF_INC + BDU */
    rc = wr1(ISM330BX_REG_CTRL3, CTRL3_IF_INC | CTRL3_BDU);
    if (rc != 0) return -2;

    /* 4. Apply full scales (CTRL8 = XL, CTRL6 = Gyro). ODR stays off until start(). */
    rc = wr1(ISM330BX_REG_CTRL8, (uint8_t)(fs_xl & 0x03));
    if (rc != 0) return -2;
    rc = wr1(ISM330BX_REG_CTRL6, (uint8_t)(fs_g & 0x0F));
    if (rc != 0) return -2;

    /* 5. Leave ODR off until start() */
    rc = wr1(ISM330BX_REG_CTRL1, CTRL_VAL(ISM330BX_ODR_XL_OFF, OP_MODE_HIGH_PERF));
    if (rc != 0) return -2;
    rc = wr1(ISM330BX_REG_CTRL2, CTRL_VAL(ISM330BX_ODR_XL_OFF, OP_MODE_HIGH_PERF));
    if (rc != 0) return -2;

    s_initialized = true;
    (void)cs_gpio;
    return 0;
}

int ism330bx_who_am_i(uint8_t *out)
{
    if (!out) return -1;
    return rd(ISM330BX_REG_WHO_AM_I, out, 1);
}

int ism330bx_start(ism330bx_odr_xl_t odr)
{
    if (!s_initialized) return -1;
    int rc = wr1(ISM330BX_REG_CTRL1, CTRL_VAL(odr, OP_MODE_HIGH_PERF));
    if (rc != 0) return rc;
    return wr1(ISM330BX_REG_CTRL2, CTRL_VAL(odr, OP_MODE_HIGH_PERF));
}

int ism330bx_stop(void)
{
    if (!s_initialized) return -1;
    int rc = wr1(ISM330BX_REG_CTRL1, CTRL_VAL(ISM330BX_ODR_XL_OFF, OP_MODE_HIGH_PERF));
    if (rc != 0) return rc;
    return wr1(ISM330BX_REG_CTRL2, CTRL_VAL(ISM330BX_ODR_XL_OFF, OP_MODE_HIGH_PERF));
}

int ism330bx_read_sample(ism330bx_sample_t *s)
{
    if (!s || !s_initialized) return -1;

    /* Gyro then accel — registers contiguous with IF_INC on */
    uint8_t raw[12];
    int rc = rd(ISM330BX_REG_OUTX_L_G, raw, sizeof(raw));
    if (rc != 0) return -2;

    s->gx = (int16_t)((uint16_t)raw[1]  << 8 | raw[0]);
    s->gy = (int16_t)((uint16_t)raw[3]  << 8 | raw[2]);
    s->gz = (int16_t)((uint16_t)raw[5]  << 8 | raw[4]);
    s->ax = (int16_t)((uint16_t)raw[7]  << 8 | raw[6]);
    s->ay = (int16_t)((uint16_t)raw[9]  << 8 | raw[8]);
    s->az = (int16_t)((uint16_t)raw[11] << 8 | raw[10]);
    return 0;
}

/* -------------------------------------------------------------------------
 * FIFO streaming — XL + gyro, TAG mode (each FIFO word is 1 tag + 6 data).
 * TAG_SENSOR values we parse:
 *    0x01  gyro NC
 *    0x02  accel NC
 * We pair consecutive gyro+accel words into one 6-axis sample. The datasheet
 * guarantees that at the same BDR the two sensors produce paired entries.
 * ------------------------------------------------------------------------- */
#define ISM330BX_FIFO_CTRL3_BDR(g_odr, xl_odr) \
    ((((g_odr) & 0xF) << 4) | ((xl_odr) & 0xF))
#define ISM330BX_FIFO_MODE_CONTINUOUS   (0x6 << 0)
#define ISM330BX_FIFO_TAG_GYRO          0x01
#define ISM330BX_FIFO_TAG_ACCEL         0x02
#define ISM330BX_FIFO_BURST_MAX         36    /* 36 words * 7 bytes = 252 B */

/* A gyro word waiting for its accel partner. Kept across reads: a burst can end
 * between the two words of a pair, and dropping the partner there would lose a
 * sample at every burst boundary once the stream is misaligned. */
static int16_t s_pending_g[3];
static bool    s_has_gyro;

int ism330bx_fifo_start(ism330bx_odr_xl_t odr, uint16_t watermark)
{
    if (!s_initialized) return -1;
    s_has_gyro = false;
    if (watermark == 0) watermark = 32;
    if (watermark > 512) watermark = 512;

    int rc = wr1(ISM330BX_REG_FIFO_CTRL1, (uint8_t)(watermark & 0xFF));
    if (rc != 0) return rc;
    rc = wr1(ISM330BX_REG_FIFO_CTRL2, (uint8_t)((watermark >> 8) & 0x01));
    if (rc != 0) return rc;

    /* Both gyro and accel batch to FIFO at the same BDR = ODR. */
    rc = wr1(ISM330BX_REG_FIFO_CTRL3, ISM330BX_FIFO_CTRL3_BDR(odr, odr));
    if (rc != 0) return rc;

    /* Continuous mode */
    rc = wr1(ISM330BX_REG_FIFO_CTRL4, ISM330BX_FIFO_MODE_CONTINUOUS);
    if (rc != 0) return rc;

    /* Arm ODRs */
    rc = wr1(ISM330BX_REG_CTRL1, CTRL_VAL(odr, OP_MODE_HIGH_PERF));
    if (rc != 0) return rc;
    return  wr1(ISM330BX_REG_CTRL2, CTRL_VAL(odr, OP_MODE_HIGH_PERF));
}

int ism330bx_fifo_stop(void)
{
    if (!s_initialized) return -1;
    (void)wr1(ISM330BX_REG_FIFO_CTRL4, 0);
    (void)wr1(ISM330BX_REG_CTRL1, CTRL_VAL(ISM330BX_ODR_XL_OFF, OP_MODE_HIGH_PERF));
    return  wr1(ISM330BX_REG_CTRL2, CTRL_VAL(ISM330BX_ODR_XL_OFF, OP_MODE_HIGH_PERF));
}

int ism330bx_fifo_reset(void)
{
    if (!s_initialized) return -1;
    s_has_gyro = false;
    int rc = wr1(ISM330BX_REG_FIFO_CTRL4, 0);
    if (rc != 0) return rc;
    return wr1(ISM330BX_REG_FIFO_CTRL4, ISM330BX_FIFO_MODE_CONTINUOUS);
}

int ism330bx_fifo_level(void)
{
    if (!s_initialized) return -1;
    uint8_t st[2] = { 0 };
    int rc = rd(ISM330BX_REG_FIFO_STATUS1, st, 2);
    if (rc != 0) return -1;
    return (int)(((uint16_t)(st[1] & 0x03) << 8) | st[0]);
}

int ism330bx_fifo_read_batch(ism330bx_sample_t *buf, uint16_t max)
{
    if (!s_initialized || !buf || max == 0) return -1;

    int lvl = ism330bx_fifo_level();
    if (lvl <= 0) return lvl;
    /* We need gyro+accel pair for each output sample → read 2 words per pair. */
    uint16_t words_avail = (uint16_t)lvl;
    if (words_avail < 2) return 0;
    uint16_t pairs_avail = words_avail / 2;
    if (pairs_avail > max) pairs_avail = max;

    uint16_t words_to_read = (uint16_t)(pairs_avail * 2);
    if (words_to_read > ISM330BX_FIFO_BURST_MAX) {
        words_to_read = ISM330BX_FIFO_BURST_MAX;
        pairs_avail = words_to_read / 2;
    }

    uint8_t raw[ISM330BX_FIFO_BURST_MAX * 7];
    uint16_t bytes = (uint16_t)(words_to_read * 7);
    int rc = rd(ISM330BX_REG_FIFO_DATA_OUT_TAG, raw, bytes);
    if (rc != 0) return -2;

    /* Walk words pair-by-pair; skip orphan words that don't match the expected
     * sequence (they're resync markers after an overflow, etc.). */
    uint16_t kept = 0;

    for (uint16_t i = 0; i < words_to_read && kept < pairs_avail; i++) {
        const uint8_t *w = &raw[i * 7];
        uint8_t tag = (w[0] >> 3) & 0x1F;
        int16_t x = (int16_t)((uint16_t)w[2] << 8 | w[1]);
        int16_t y = (int16_t)((uint16_t)w[4] << 8 | w[3]);
        int16_t z = (int16_t)((uint16_t)w[6] << 8 | w[5]);

        if (tag == ISM330BX_FIFO_TAG_GYRO) {
            s_pending_g[0] = x; s_pending_g[1] = y; s_pending_g[2] = z;
            s_has_gyro = true;
        } else if (tag == ISM330BX_FIFO_TAG_ACCEL) {
            if (s_has_gyro) {
                buf[kept].gx = s_pending_g[0];
                buf[kept].gy = s_pending_g[1];
                buf[kept].gz = s_pending_g[2];
                buf[kept].ax = x;
                buf[kept].ay = y;
                buf[kept].az = z;
                kept++;
                s_has_gyro = false;
            }
            /* else: accel without prior gyro → drop */
        }
    }
    return (int)kept;
}
