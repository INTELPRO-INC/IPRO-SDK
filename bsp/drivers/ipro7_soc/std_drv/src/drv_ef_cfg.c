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

#include <string.h>
#include "drv_ef_ctrl.h"
#include "drv_ef_cfg.h"
#include "ef_data_0_reg.h"

#ifndef BOOTROM
#define EF_CTRL_LOAD_BEFORE_READ_R0 EF_Ctrl_Load_Efuse_R0()
#else
#define EF_CTRL_LOAD_BEFORE_READ_R0
#endif

static const Efuse_Common_Trim_Cfg trim_lit[] = {
    {
        .name = "rc32m",
        .en_addr = 0x0C * 8 + 19,
        .parity_addr = 0x0C * 8 + 18,
        .value_addr = 0x0C * 8 + 10,
        .value_len = 8,
    },
    {
        .name = "rc32k",
        .en_addr = 0x0C * 8 + 25,
        .parity_addr = 0x0C * 8 + 24,
        .value_addr = 0x0C * 8 + 20,
        .value_len = 4,
    },
    /* rc32k_code: 13-bit RC32K code_fr_cal split across two efuse words.
     * en/parity bits are SHARED across both halves (parity covers full
     * 13-bit composed value, validated by the caller after composing).
     *   ef_ana_trim_0    (0x0C): [9]=en, [8]=parity, [7:2]=code[12:7]
     *   ef_key_slot_3_w1 (0x50): [30:24]=code[6:0]
     */
    {
        .name = "rc32k_code_h",
        .en_addr     = 0x0C * 8 + 9,
        .parity_addr = 0x0C * 8 + 8,
        .value_addr  = 0x0C * 8 + 2,
        .value_len   = 6,
    },
    {
        .name = "rc32k_code_l",
        .en_addr     = 0x0C * 8 + 9,    /* shared with rc32k_code_h */
        .parity_addr = 0x0C * 8 + 8,    /* shared (over full 13-bit) */
        .value_addr  = 0x50 * 8 + 24,
        .value_len   = 7,
    },
    {
        .name = "ldo14_trim",
        .en_addr = 0x0C * 8 + 31,
        .parity_addr = 0x0C * 8 + 30,
        .value_addr = 0x0C * 8 + 26,
        .value_len = 4,
    },
    {
        .name = "xtal",
        .en_addr = 0x58 * 8 + 9,
        .parity_addr = 0x58 * 8 + 8,
        .value_addr = 0x58 * 8 + 0,
        .value_len = 8,
    },
    {
        .name = "ldo11_trim",
        .en_addr = 0x10 * 8 + 31,
        .parity_addr = 0x10 * 8 + 30,
        .value_addr = 0x10 * 8 + 26,
        .value_len = 4,
    },
    {
        .name = "psram_trim",
        .en_addr = 0x58 * 8 + 29,
        .parity_addr = 0x58 * 8 + 28,
        .value_addr = 0x58 * 8 + 20,
        .value_len = 8,
    },
    {
        .name = "gpadc_gain",
        .en_addr = 0x10 * 8 + 25,
        .parity_addr = 0x10 * 8 + 24,
        .value_addr = 0x10 * 8 + 12,
        .value_len = 12,
    },
    {
        .name = "tsen",
        .en_addr = 0x0C * 8 + 1,
        .parity_addr = 0x0C * 8 + 0,
        .value_addr = 0x10 * 8 + 0,
        .value_len = 12,
    },
};

uint8_t ATTR_CLOCK_SECTION EF_Ctrl_Get_Trim_Parity(uint32_t val, uint8_t len)
{
    uint8_t cnt = 0;
    uint8_t i = 0;

    for (i = 0; i < len; i++) {
        if (val & (1 << i)) {
            cnt++;
        }
    }

    return cnt & 0x01;
}

uint32_t EF_Ctrl_Get_Common_Trim_List(const Efuse_Common_Trim_Cfg **trim_list)
{
    *trim_list = &trim_lit[0];
    return sizeof(trim_lit) / sizeof(trim_lit[0]);
}

void EF_Ctrl_Read_Common_Trim(char *name, Efuse_Common_Trim_Type *trim, uint8_t reload)
{
    uint32_t tmpVal;
    uint32_t i = 0;

    if (reload) {
	    /* Trigger read data from efuse */
	    EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    trim->en = 0;
    trim->parity = 0;
    trim->empty = 1;
    trim->len = 0;

    for (i = 0; i < sizeof(trim_lit) / sizeof(trim_lit[0]); i++) {
        if (strcmp(name, trim_lit[i].name) == 0) {
            trim->len = trim_lit[i].value_len;
            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4);
            if (tmpVal & (1 << (trim_lit[i].en_addr % 32))) {
                trim->en = 1;
            }
            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4);
            if (tmpVal & (1 << (trim_lit[i].parity_addr % 32))) {
                trim->parity = 1;
            }

            if (((trim_lit[i].value_addr % 32) + trim_lit[i].value_len) > 32) {
                uint64_t tmpVal64 = (uint64_t)readl(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
                tmpVal64 |= (((uint64_t)readl(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4 + 4)) << 32);
                tmpVal64 = tmpVal64 >> (trim_lit[i].value_addr % 32);
                trim->value = (uint32_t)(tmpVal64 & (((uint64_t)1 << trim_lit[i].value_len) - 1));
            } else {
                tmpVal = readl(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
                tmpVal = tmpVal >> (trim_lit[i].value_addr % 32);
                trim->value = tmpVal & (((uint64_t)1 << trim_lit[i].value_len) - 1);
            }

            if (trim->en == 0 && trim->parity == 0 && trim->value == 0) {
                trim->empty = 1;
            } else {
                trim->empty = 0;
            }
        }
    }
}

void EF_Ctrl_Write_Common_Trim_Shadow(char *name, uint8_t trim_en,
                                       uint8_t parity, uint32_t trim_value)
{
    uint32_t tmpVal;
    uint32_t i;
    uint8_t  len;

    /* Caller responsibilities for multi-call sequences:
     *   1. Set controller to AHB mode (EF_Ctrl_Sw_AHB_Clk_0()).
     *   2. Reload shadow from OTP (EF_Ctrl_Read_* with reload=1, or
     *      EF_CTRL_LOAD_BEFORE_READ_R0).
     *   3. After all _Shadow calls, trigger EF_Ctrl_Program_Efuse_0().
     *
     * This function intentionally avoids Sw_AHB_Clk_0 / LOAD here,
     * because either would interfere with shadow modifications from
     * prior _Shadow calls in the same sequence (split-storage trims). */

    for (i = 0; i < sizeof(trim_lit) / sizeof(trim_lit[0]); i++) {
        if (strcmp(name, trim_lit[i].name) != 0) {
            continue;
        }
        len = trim_lit[i].value_len;
        if (trim_value > (((uint64_t)1 << len) - 1)) {
            return;
        }

        if (trim_en) {
            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4);
            tmpVal |= (1U << (trim_lit[i].en_addr % 32));
            writel(tmpVal, EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4);
        }

        if (parity) {
            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4);
            tmpVal |= (1U << (trim_lit[i].parity_addr % 32));
            writel(tmpVal, EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4);
        }

        if (trim_value) {
            if (((trim_lit[i].value_addr % 32) + len) > 32) {
                tmpVal = readl(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
                tmpVal |= (trim_value << (trim_lit[i].value_addr % 32));
                writel(tmpVal, EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);

                tmpVal = readl(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4 + 4);
                tmpVal |= (trim_value >> (32 - (trim_lit[i].value_addr % 32)));
                writel(tmpVal, EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4 + 4);
            } else {
                tmpVal = readl(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
                tmpVal |= (trim_value << (trim_lit[i].value_addr % 32));
                writel(tmpVal, EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
            }
        }
        return;
    }
}

void EF_Ctrl_Write_Common_Trim(char *name, uint8_t trim_en, uint32_t trim_value)
{
    uint32_t i;

    if (trim_en == 0 && trim_value == 0) {
        return;
    }

    /* Single-call path: reload shadow from OTP first so RMW starts from
     * a clean state. Multi-call _Shadow users (e.g. split-storage trims)
     * load shadow once externally before the sequence and skip this. */
    EF_Ctrl_Sw_AHB_Clk_0();
    EF_CTRL_LOAD_BEFORE_READ_R0;

    for (i = 0; i < sizeof(trim_lit) / sizeof(trim_lit[0]); i++) {
        if (strcmp(name, trim_lit[i].name) != 0) {
            continue;
        }
        uint8_t parity = EF_Ctrl_Get_Trim_Parity(trim_value, trim_lit[i].value_len);
        EF_Ctrl_Write_Common_Trim_Shadow(name, trim_en, parity, trim_value);
        EF_Ctrl_Program_Efuse_0();
        return;
    }
}

uint8_t EF_Ctrl_Is_MAC_Address_Slot_Empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp1 = 0xffffffff, tmp2 = 0xffffffff;
    uint32_t part1Empty = 0, part2Empty = 0;

    if (slot == 0) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = readl(EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_LOW_OFFSET);
        tmp2 = readl(EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_HIGH_OFFSET);
    } else if (slot == 1) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W2_OFFSET);
        tmp2 = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W3_OFFSET);
    }

    part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 0, 32));
    part2Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp2, 0, 22));

    return (part1Empty && part2Empty);
}

int EF_Ctrl_Write_MAC_Address_Opt(uint8_t slot, uint8_t mac[8], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;

    if (slot >= 3) {
        return ERROR;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* The low 32 bits */
    if (slot == 0) {
        writel(DRV_WORD_FROM_BYTES(maclow), EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_LOW_OFFSET);
    } else if (slot == 1) {
        writel(DRV_WORD_FROM_BYTES(maclow), EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W2_OFFSET);
    }

    /* The high 32 bits */
    if (slot == 0) {
        writel(DRV_WORD_FROM_BYTES(machigh), EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_HIGH_OFFSET);
    } else if (slot == 1) {
        writel(DRV_WORD_FROM_BYTES(machigh), EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W3_OFFSET);
    }

    cnt = 0;
    for (i = 0; i < 8; i++) {
        cnt += EF_Ctrl_Get_Byte_Zero_Cnt(mac[i]);
    }
    cnt &= 0x3f;
    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_SW_USAGE_1_OFFSET);
    if (slot == 0) {
        tmpVal |= (cnt << 0);
    } else if (slot == 1) {
        tmpVal |= (cnt << 6);
    }
    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_EF_SW_USAGE_1_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
    return SUCCESS;
}

int EF_Ctrl_Read_MAC_Address_Opt(uint8_t slot, uint8_t mac[8], uint8_t reload)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;
    uint32_t crc = 0;

    if (slot >= 3) {
        return ERROR;
    }

    /* Trigger read data from efuse */
    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_LOW_OFFSET);
    } else if (slot == 1) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W2_OFFSET);
    }
    DRV_WORD_TO_BYTES(maclow, tmpVal);

    if (slot == 0) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_HIGH_OFFSET);
    } else if (slot == 1) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W3_OFFSET);
    }
    DRV_WORD_TO_BYTES(machigh, tmpVal);

    /* Get original parity */
    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_SW_USAGE_1_OFFSET);
    if (slot == 0) {
        crc = ((tmpVal >> 0) & 0x3f);
    } else if (slot == 1) {
        crc = ((tmpVal >> 6) & 0x3f);
    }

    /* Check parity */
    for (i = 0; i < 8; i++) {
        cnt += EF_Ctrl_Get_Byte_Zero_Cnt(mac[i]);
    }
    if ((cnt & 0x3f) == crc) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}
