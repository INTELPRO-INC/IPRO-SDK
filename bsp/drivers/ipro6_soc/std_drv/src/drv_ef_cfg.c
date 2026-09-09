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



#include "string.h"
#include "drv_ef_ctrl.h"
#include "drv_ef_cfg.h"

#define EF_CTRL_LOAD_BEFORE_READ_R0 EF_Ctrl_Load_Efuse_R0()

static Efuse_Common_Trim_Cfg trim_lit[] = {
    {
        .name = "ldo15",
        .en_addr = 0x68 * 8 + 31,
        .parity_addr = 0x68 * 8 + 30,
        .value_addr = 0x68 * 8 + 27,
        .value_len = 3,
    },
    {
        .name = "iptat",
        .en_addr = 0x74 * 8 + 31,
        .parity_addr = 0x74 * 8 + 30,
        .value_addr = 0x68 * 8 + 22,
        .value_len = 5,
    },
    {
        .name = "icx",
        .en_addr = 0x74 * 8 + 29,
        .parity_addr = 0x74 * 8 + 28,
        .value_addr = 0x74 * 8 + 22,
        .value_len = 6,
    },
    {
        .name = "dcdc_trim",
        .en_addr = 0x78 * 8 + 31,
        .parity_addr = 0x78 * 8 + 30,
        .value_addr = 0x78 * 8 + 26,
        .value_len = 4,
    },
    {
        .name = "ldo18_sel",
        .en_addr = 0x78 * 8 + 25,
        .parity_addr = 0x78 * 8 + 24,
        .value_addr = 0x78 * 8 + 20,
        .value_len = 4,
    },
    {
        .name = "ldo18_trim",
        .en_addr = 0x78 * 8 + 19,
        .parity_addr = 0x78 * 8 + 18,
        .value_addr = 0x78 * 8 + 14,
        .value_len = 4,
    },
    {
        .name = "ldo33_trim",
        .en_addr = 0x78 * 8 + 13,
        .parity_addr = 0x78 * 8 + 12,
        .value_addr = 0x78 * 8 + 8,
        .value_len = 4,
    },
    {
        .name = "ldo11_tirm",
        .en_addr = 0x78 * 8 + 7,
        .parity_addr = 0x78 * 8 + 6,
        .value_addr = 0x78 * 8 + 2,
        .value_len = 4,
    },
    {
        .name = "rc32m",
        .en_addr = 0x78 * 8 + 1,
        .parity_addr = 0x78 * 8 + 0,
        .value_addr = 0x7C * 8 + 4,
        .value_len = 8,
    },
    {
        .name = "hp_poffset0",
        .en_addr = 0xCC * 8 + 26,
        .parity_addr = 0xC0 * 8 + 15,
        .value_addr = 0xC0 * 8 + 0,
        .value_len = 15,
    },
    {
        .name = "hp_poffset1",
        .en_addr = 0xCC * 8 + 27,
        .parity_addr = 0xC0 * 8 + 31,
        .value_addr = 0xC0 * 8 + 16,
        .value_len = 15,
    },
    {
        .name = "hp_poffset2",
        .en_addr = 0xCC * 8 + 28,
        .parity_addr = 0xC4 * 8 + 15,
        .value_addr = 0xC4 * 8 + 0,
        .value_len = 15,
    },
    {
        .name = "lp_poffset0",
        .en_addr = 0xCC * 8 + 29,
        .parity_addr = 0xC4 * 8 + 31,
        .value_addr = 0xC4 * 8 + 16,
        .value_len = 15,
    },
    {
        .name = "lp_poffset1",
        .en_addr = 0xCC * 8 + 30,
        .parity_addr = 0xC8 * 8 + 15,
        .value_addr = 0xC8 * 8 + 0,
        .value_len = 15,
    },
    {
        .name = "lp_poffset2",
        .en_addr = 0xCC * 8 + 31,
        .parity_addr = 0xC8 * 8 + 31,
        .value_addr = 0xC8 * 8 + 16,
        .value_len = 15,
    },
    {
        .name = "bz_poffset0",
        .en_addr = 0xD0 * 8 + 26,
        .parity_addr = 0xCC * 8 + 25,
        .value_addr = 0xCC * 8 + 0,
        .value_len = 25,
    },
    {
        .name = "bz_poffset1",
        .en_addr = 0xD0 * 8 + 27,
        .parity_addr = 0xD0 * 8 + 25,
        .value_addr = 0xD0 * 8 + 0,
        .value_len = 25,
    },
    {
        .name = "bz_poffset2",
        .en_addr = 0xD0 * 8 + 28,
        .parity_addr = 0xD4 * 8 + 25,
        .value_addr = 0xD4 * 8 + 0,
        .value_len = 25,
    },
    {
        .name = "tmp_mp0",
        .en_addr = 0xD8 * 8 + 9,
        .parity_addr = 0xD8 * 8 + 8,
        .value_addr = 0xD8 * 8 + 0,
        .value_len = 8,
    },
    {
        .name = "tmp_mp1",
        .en_addr = 0xD8 * 8 + 19,
        .parity_addr = 0xD8 * 8 + 18,
        .value_addr = 0xD8 * 8 + 10,
        .value_len = 8,
    },
    {
        .name = "tmp_mp2",
        .en_addr = 0xD8 * 8 + 29,
        .parity_addr = 0xD8 * 8 + 28,
        .value_addr = 0xD8 * 8 + 20,
        .value_len = 8,
    },
    {
        .name = "auadc_gain",
        .en_addr = 0xDC * 8 + 25,
        .parity_addr = 0xDC * 8 + 24,
        .value_addr = 0xDC * 8 + 0,
        .value_len = 24,
    },
    {
        .name = "auadc_offset",
        .en_addr = 0xE0 * 8 + 25,
        .parity_addr = 0xE0 * 8 + 24,
        .value_addr = 0xE0 * 8 + 0,
        .value_len = 24,
    },
    {
        .name = "psram_trim",
        .en_addr = 0xE8 * 8 + 12,
        .parity_addr = 0xE8 * 8 + 11,
        .value_addr = 0xE8 * 8 + 0,
        .value_len = 11,
    },
    {
        .name = "rc32k",
        .en_addr = 0xEC * 8 + 19,
        .parity_addr = 0xEC * 8 + 18,
        .value_addr = 0xEC * 8 + 8,
        .value_len = 10,
    },
    {
        .name = "xtal0",
        .en_addr = 0xEC * 8 + 7,
        .parity_addr = 0xEC * 8 + 6,
        .value_addr = 0xEC * 8 + 0,
        .value_len = 6,
    },
    {
        .name = "xtal1",
        .en_addr = 0xF0 * 8 + 31,
        .parity_addr = 0xF0 * 8 + 30,
        .value_addr = 0xF4 * 8 + 26,
        .value_len = 6,
    },
    {
        .name = "xtal2",
        .en_addr = 0xF0 * 8 + 29,
        .parity_addr = 0xF0 * 8 + 28,
        .value_addr = 0xF4 * 8 + 20,
        .value_len = 6,
    },
    {
        .name = "gpadc_gain",
        .en_addr = 0xF0 * 8 + 27,
        .parity_addr = 0xF0 * 8 + 26,
        .value_addr = 0xF0 * 8 + 14,
        .value_len = 12,
    },
    {
        .name = "tsen",
        .en_addr = 0xF0 * 8 + 13,
        .parity_addr = 0xF0 * 8 + 12,
        .value_addr = 0xF0 * 8 + 0,
        .value_len = 12,
    },
    {
        .name = "dcdc_dis",
        .en_addr = 0xF4 * 8 + 19,
        .parity_addr = 0xF4 * 8 + 18,
        .value_addr = 0xF4 * 8 + 17,
        .value_len = 1,
    },
    {
        .name = "dcdc_vout",
        .en_addr = 0xF4 * 8 + 16,
        .parity_addr = 0xF4 * 8 + 15,
        .value_addr = 0xF4 * 8 + 10,
        .value_len = 5,
    },
    {
        .name = "ldo18_bypass",
        .en_addr = 0xF4 * 8 + 9,
        .parity_addr = 0xF4 * 8 + 8,
        .value_addr = 0xF4 * 8 + 4,
        .value_len = 1,
    },
    {
        .name = "usb20",
        .en_addr = 0xF8 * 8 + 15,
        .parity_addr = 0xF8 * 8 + 14,
        .value_addr = 0xF8 * 8 + 8,
        .value_len = 6,
    }
};

static uint32_t EF_Cfg_Get_Byte_Zero_Cnt(uint8_t val)
{
    uint32_t cnt = 0;
    uint32_t i = 0;

    for (i = 0; i < 8; i++) {
        if ((val & (1 << i)) == 0) {
            cnt += 1;
        }
    }

    return cnt;
}

uint32_t EF_Ctrl_Get_Common_Trim_List(Efuse_Common_Trim_Cfg **trim_list)
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
        if (memcmp(name, trim_lit[i].name, strlen(name)) == 0) {
            trim->len = trim_lit[i].value_len;
            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4);
            if (tmpVal & (1 << (trim_lit[i].en_addr % 32))) {
                trim->en = 1;
            }
            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4);
            if (tmpVal & (1 << (trim_lit[i].parity_addr % 32))) {
                trim->parity = 1;
            }
            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
            tmpVal = tmpVal >> (trim_lit[i].value_addr % 32);
            trim->value = tmpVal & ((1 << trim_lit[i].value_len) - 1);
            if (trim->en == 0 && trim->parity == 0 && trim->value == 0) {
                trim->empty = 1;
            } else {
                trim->empty = 0;
            }
        }
    }
}

void EF_Ctrl_Write_Common_Trim(char *name, uint32_t value, uint8_t program)
{
    uint32_t tmpVal;
    uint32_t i = 0;
    uint8_t parity = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    for (i = 0; i < sizeof(trim_lit) / sizeof(trim_lit[0]); i++) {
        if (memcmp(name, trim_lit[i].name, strlen(name)) == 0) {
            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4);
            tmpVal |= (1 << (trim_lit[i].en_addr % 32));
            writel(tmpVal, EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4);

            parity = EF_Ctrl_Get_Trim_Parity(value, trim_lit[i].value_len);
            if (parity) {
                tmpVal = readl(EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4);
                tmpVal |= (1 << (trim_lit[i].parity_addr % 32));
                writel(tmpVal, EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4);
            }

            tmpVal = readl(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
            tmpVal |= (value << (trim_lit[i].value_addr % 32));
            writel(tmpVal, EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
        }
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
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

        tmp1 = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_LOW_OFFSET);
        tmp2 = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);
    } else if (slot == 1) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_2_OFFSET);
        tmp2 = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_3_OFFSET);
    } else if (slot == 2) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W1_OFFSET);
        tmp2 = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W2_OFFSET);
    }

    part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 0, 32));
    part2Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp2, 0, 22));

    return (part1Empty && part2Empty);
}

int EF_Ctrl_Write_MAC_Address_Opt(uint8_t slot, uint8_t mac[6], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;
    uint32_t i = 0, cnt;

    if (slot >= 3) {
        return ERROR;
    }

    /* Change to local order */
    for (i = 0; i < 3; i++) {
        tmpVal = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = tmpVal;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* The low 32 bits */
    if (slot == 0) {
        writel(DRV_WORD_FROM_BYTES(maclow), EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_LOW_OFFSET);
    } else if (slot == 1) {
        writel(DRV_WORD_FROM_BYTES(maclow), EF_DATA_BASE + EF_DATA_EF_SW_USAGE_2_OFFSET);
    } else if (slot == 2) {
        writel(DRV_WORD_FROM_BYTES(maclow), EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W1_OFFSET);
    }

    /* The high 16 bits */
    tmpVal = machigh[0] + (machigh[1] << 8);
    cnt = 0;

    for (i = 0; i < 6; i++) {
        cnt += EF_Cfg_Get_Byte_Zero_Cnt(mac[i]);
    }

    tmpVal |= ((cnt & 0x3f) << 16);

    if (slot == 0) {
        writel(tmpVal, EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);
    } else if (slot == 1) {
        writel(tmpVal, EF_DATA_BASE + EF_DATA_EF_SW_USAGE_3_OFFSET);
    } else if (slot == 2) {
        writel(tmpVal, EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W2_OFFSET);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }

    return SUCCESS;
}

int EF_Ctrl_Read_MAC_Address_Opt(uint8_t slot, uint8_t mac[6], uint8_t reload)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;

    if (slot >= 3) {
        return ERROR;
    }

    /* Trigger read data from efuse */
    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_LOW_OFFSET);
    } else if (slot == 1) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_2_OFFSET);
    } else if (slot == 2) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W1_OFFSET);
    }

    DRV_WORD_TO_BYTES(maclow, tmpVal);

    if (slot == 0) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);
    } else if (slot == 1) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_3_OFFSET);
    } else if (slot == 2) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W2_OFFSET);
    }

    machigh[0] = tmpVal & 0xff;
    machigh[1] = (tmpVal >> 8) & 0xff;

    /* Check parity */
    for (i = 0; i < 6; i++) {
        cnt += EF_Cfg_Get_Byte_Zero_Cnt(mac[i]);
    }

    if ((cnt & 0x3f) == ((tmpVal >> 16) & 0x3f)) {
        /* Change to network order */
        for (i = 0; i < 3; i++) {
            tmpVal = mac[i];
            mac[i] = mac[5 - i];
            mac[5 - i] = tmpVal;
        }

        return SUCCESS;
    } else {
        return ERROR;
    }
}

