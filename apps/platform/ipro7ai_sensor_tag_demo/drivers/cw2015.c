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
 * @file cw2015.c
 * @brief CellWise CW2015 I2C fuel-gauge driver (bring-up subset).
 *
 * Register map + conversions follow the mainline Linux cw2015_battery
 * driver. We only implement what this board needs: presence check, wake
 * from sleep, SOC read, VCELL read. The 64-byte battery profile (BATINFO)
 * is left at the chip's power-on default — SOC settles to a usable estimate
 * within a few seconds and the caller can fall back to the cell voltage (and
 * then internal VBAT/2) when the gauge hasn't converged or is absent.
 */

#include <string.h>

#include "cw2015.h"
#include "ipro_log.h"

#include "FreeRTOS.h"
#include "task.h"

#include <generated/autoconf.h>

#include "hal_i2c.h"
#include "drv_glb.h"
#include "drv_gpio.h"

#define LOG_TAG "GAUGE"

/* ---- I2C wiring (overridable from .config) ---- */
#ifndef CONFIG_CW2015_I2C_BUS
#define CONFIG_CW2015_I2C_BUS 0
#endif
#ifndef CONFIG_CW2015_GPIO_SDA
#define CONFIG_CW2015_GPIO_SDA 29
#endif
#ifndef CONFIG_CW2015_GPIO_SCL
#define CONFIG_CW2015_GPIO_SCL 30
#endif
/* Datasheet 8-bit write addr 0xC4 → 7-bit 0x62 for the SDK HAL. */
#define CW2015_I2C_ADDR7  0x62

/* ---- Register map ---- */
#define CW2015_REG_VERSION    0x00
#define CW2015_REG_VCELL      0x02  /* 2 bytes, 14-bit, 305 µV/LSB */
#define CW2015_REG_SOC        0x04  /* 2 bytes: [0]=int %, [1]=frac/256 */
#define CW2015_REG_RRT_ALERT  0x06
#define CW2015_REG_CONFIG     0x08
#define CW2015_REG_MODE       0x0A

#define CW2015_MODE_SLEEP     (0x03 << 6)  /* bits[7:6]=11 → sleep */
#define CW2015_MODE_NORMAL    (0x00 << 6)
#define CW2015_MODE_QSTRT     (0x03 << 4)  /* bits[5:4]=11 → quick-start */
#define CW2015_MODE_RESTART   0x0F         /* bits[3:0]=1111 → POR/restart */

#define CW2015_REG_BATINFO    0x10         /* 64-byte battery profile */
#define CW2015_BATINFO_SIZE   64
#define CW2015_CONFIG_UFG     (1 << 1)     /* CONFIG: profile-loaded flag */
#define CW2015_ATHD_SHIFT     3            /* CONFIG[7:3] = ALRT threshold % */

#ifndef CONFIG_BATTERY_LOW_THRESHOLD_PCT
#define CONFIG_BATTERY_LOW_THRESHOLD_PCT 15
#endif

/* 64-byte BATINFO battery profile.
 *
 * Source: Rockchip vendor cw2015 driver default config_info (rk3188-kernel,
 * generic 3.7 V / 4.2 V-full LiPo tablet cell) — chemistry-matched to our
 * 602040 pouch, but NOT characterized for it. This is a "much better than
 * the chip's POR default" generic curve: SOC becomes usable, absolute
 * accuracy is moderate. The proper end state is a CellWise-characterized
 * profile for the actual 602040 cell (mail-in service) — drop those 64
 * bytes in here when they arrive. battery_monitor's VCELL cross-check
 * stays active as the guardrail either way. */
static const uint8_t k_batinfo[CW2015_BATINFO_SIZE] = {
    0x15, 0x4C, 0x5D, 0x5D, 0x5A, 0x59, 0x55, 0x51,
    0x4E, 0x48, 0x46, 0x41, 0x3C, 0x39, 0x33, 0x2D,
    0x25, 0x1E, 0x19, 0x19, 0x1A, 0x2C, 0x44, 0x4A,
    0x43, 0x40, 0x0C, 0xCD, 0x22, 0x43, 0x56, 0x82,
    0x78, 0x6F, 0x62, 0x60, 0x42, 0x19, 0x37, 0x31,
    0x00, 0x1D, 0x59, 0x85, 0x8F, 0x91, 0x91, 0x18,
    0x58, 0x82, 0x94, 0xA5, 0xFF, 0xAF, 0xE8, 0xCB,
    0x2F, 0x7D, 0x72, 0xA5, 0xB5, 0xC1, 0x46, 0xAE,
};

#define CW2015_VCELL_LSB_UV   305u         /* 305 µV per LSB */

static uint8_t s_bus;
static bool    s_present;

/* ---- I2C helpers (same pattern as the SDK MPU-6500 driver) ---- */
static int reg_write(uint8_t reg, uint8_t val)
{
    i2c_msg_t msg = {
        .slaveaddr = CW2015_I2C_ADDR7,
        .flags     = SUB_ADDR_1BYTE | I2C_WR,
        .subaddr   = reg,
        .buf       = &val,
        .len       = 1,
    };
    return i2c_transfer(s_bus, &msg, 1);
}

static int reg_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
    i2c_msg_t msg = {
        .slaveaddr = CW2015_I2C_ADDR7,
        .flags     = SUB_ADDR_1BYTE | I2C_RD,
        .subaddr   = reg,
        .buf       = buf,
        .len       = len,
    };
    return i2c_transfer(s_bus, &msg, 1);
}

static void i2c_gpio_init(uint8_t bus)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,     /* gauge board has no strong externals */
        .drive    = 0,
        .smtCtrl  = 1,
        .gpioFun  = (bus == 0) ? GPIO_FUN_I2C : GPIO_FUN_I2C1,
    };
    uint8_t pins[2] = { CONFIG_CW2015_GPIO_SDA, CONFIG_CW2015_GPIO_SCL };
    for (int i = 0; i < 2; i++) {
        cfg.gpioPin = pins[i];
        GLB_GPIO_Init(&cfg);
    }
}

/* Upload k_batinfo following the Rockchip cw_update_config_info sequence:
 * write 64 bytes to BATINFO (byte-wise, like the vendor driver), verify
 * readback, set CONFIG = ATHD | UFG, then MODE restart (0x0F) → normal.
 * After the restart the SOC re-seeds from VCELL and converges within
 * minutes using the new curve. */
static int upload_profile(void)
{
    for (int i = 0; i < CW2015_BATINFO_SIZE; i++) {
        if (reg_write((uint8_t)(CW2015_REG_BATINFO + i), k_batinfo[i]) != 0) {
            return -1;
        }
    }

    uint8_t check[CW2015_BATINFO_SIZE] = { 0 };
    if (reg_read(CW2015_REG_BATINFO, check, CW2015_BATINFO_SIZE) != 0 ||
        memcmp(check, k_batinfo, CW2015_BATINFO_SIZE) != 0) {
        return -2;                              /* readback mismatch */
    }

    uint8_t cfg = (uint8_t)((CONFIG_BATTERY_LOW_THRESHOLD_PCT << CW2015_ATHD_SHIFT)
                            | CW2015_CONFIG_UFG);
    if (reg_write(CW2015_REG_CONFIG, cfg) != 0) return -3;

    if (reg_write(CW2015_REG_MODE, CW2015_MODE_RESTART) != 0) return -4;
    vTaskDelay(pdMS_TO_TICKS(10));
    if (reg_write(CW2015_REG_MODE, CW2015_MODE_NORMAL) != 0) return -4;
    return 0;
}

int cw2015_init(void)
{
    if (s_present) return 0;

    s_bus = CONFIG_CW2015_I2C_BUS;
    i2c_gpio_init(s_bus);
    i2c_init(s_bus);

    /* Wake from sleep: MODE bits[7:6]=00. Do this before reading VERSION so a
     * gauge that powered up asleep still answers. */
    (void)reg_write(CW2015_REG_MODE, CW2015_MODE_NORMAL);
    vTaskDelay(pdMS_TO_TICKS(5));

    uint8_t ver = 0;
    if (reg_read(CW2015_REG_VERSION, &ver, 1) != 0 || ver == 0x00 || ver == 0xFF) {
        IPRO_LOGE(LOG_TAG, "CW2015 not responding (VERSION=0x%02X) — check I2C "
                  "wiring (SDA=GPIO%d SCL=GPIO%d addr 0x%02X/7b 0x%02X)",
                  ver, (int)CONFIG_CW2015_GPIO_SDA, (int)CONFIG_CW2015_GPIO_SCL,
                  0xC4, CW2015_I2C_ADDR7);
        return -1;
    }

    s_present = true;

    /* Battery profile: upload when the gauge has none loaded (UFG clear) or
     * holds different bytes. The gauge keeps BATINFO while powered from the
     * cell, so this normally runs once per battery insertion. */
    uint8_t cfg = 0;
    uint8_t cur[CW2015_BATINFO_SIZE] = { 0 };
    bool have_cfg = (reg_read(CW2015_REG_CONFIG, &cfg, 1) == 0);
    bool same = (reg_read(CW2015_REG_BATINFO, cur, CW2015_BATINFO_SIZE) == 0) &&
                (memcmp(cur, k_batinfo, CW2015_BATINFO_SIZE) == 0);

    if (!have_cfg || !(cfg & CW2015_CONFIG_UFG) || !same) {
        int prc = upload_profile();
        if (prc == 0) {
            IPRO_LOGI(LOG_TAG, "BATINFO profile uploaded (generic 4.2V curve) — "
                      "SOC re-seeding");
        } else {
            IPRO_LOGW(LOG_TAG, "BATINFO upload failed rc=%d — running profile-less "
                      "(VCELL cross-check covers it)", prc);
        }
    } else if (((cfg >> CW2015_ATHD_SHIFT) & 0x1F) != CONFIG_BATTERY_LOW_THRESHOLD_PCT) {
        /* Profile already in — just align the ALRT threshold (drives ALRTB). */
        cfg = (uint8_t)((cfg & ~(0x1F << CW2015_ATHD_SHIFT))
                        | (CONFIG_BATTERY_LOW_THRESHOLD_PCT << CW2015_ATHD_SHIFT));
        (void)reg_write(CW2015_REG_CONFIG, cfg);
    }

    IPRO_LOGI(LOG_TAG, "CW2015 ready (I2C%d SDA%d/SCL%d, VERSION=0x%02X, ATHD=%d%%)",
              s_bus, (int)CONFIG_CW2015_GPIO_SDA, (int)CONFIG_CW2015_GPIO_SCL, ver,
              CONFIG_BATTERY_LOW_THRESHOLD_PCT);
    return 0;
}

bool cw2015_present(void) { return s_present; }

int cw2015_read_soc(uint8_t *pct)
{
    if (!pct) return -1;
    if (!s_present) return -2;

    uint8_t soc[2] = { 0, 0 };
    if (reg_read(CW2015_REG_SOC, soc, 2) != 0) return -3;

    /* soc[0] = integer percent; clamp defensively. */
    *pct = (soc[0] > 100) ? 100 : soc[0];
    return 0;
}

int cw2015_read_vcell_mv(uint16_t *mv)
{
    if (!mv) return -1;
    if (!s_present) return -2;

    uint8_t v[2] = { 0, 0 };
    if (reg_read(CW2015_REG_VCELL, v, 2) != 0) return -3;

    /* 14-bit value across [13:8]|[7:0], 305 µV/LSB → millivolts. */
    uint32_t raw = ((uint32_t)(v[0] & 0x3F) << 8) | v[1];
    *mv = (uint16_t)((raw * CW2015_VCELL_LSB_UV) / 1000u);
    return 0;
}

int cw2015_quick_start(void)
{
    if (!s_present) return -1;

    /* Quick-start (MODE[5:4]=11 then back to normal) re-seeds the SOC
     * algorithm from the current VCELL — the recovery for an unconverged /
     * profile-less gauge whose SOC has drifted away from reality. SOC is
     * approximate for the first minutes after this. */
    int rc = reg_write(CW2015_REG_MODE, CW2015_MODE_QSTRT);
    if (rc == 0) rc = reg_write(CW2015_REG_MODE, CW2015_MODE_NORMAL);
    IPRO_LOGW(LOG_TAG, "quick-start issued (SOC re-seeds from VCELL) rc=%d", rc);
    return rc;
}
