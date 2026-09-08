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
#include "ef_data_0_reg.h"

#define EF_CTRL_EFUSE_CYCLE_PROTECT (0xbf << 24)
#define EF_CTRL_EFUSE_CTRL_PROTECT  (0xbf << 8)
#define EF_CTRL_DFT_TIMEOUT_VAL     (160 * 1000)
#ifndef BOOTROM
#define EF_CTRL_LOAD_BEFORE_READ_R0 EF_Ctrl_Load_Efuse_R0()
#else
#define EF_CTRL_LOAD_BEFORE_READ_R0
#endif
#define EF_CTRL_DATA0_CLEAR EF_Ctrl_Clear(0, EF_CTRL_EFUSE_R0_SIZE / 4)

void ATTR_TCM_SECTION EF_Ctrl_Sw_AHB_Clk_0(void)
{
    uint32_t tmpVal;
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

    while (EF_Ctrl_Busy() == SET) {
        timeout--;

        if (timeout == 0) {
            break;
        }
    }

    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
}

void ATTR_TCM_SECTION EF_Ctrl_Program_Efuse_0(void)
{
    uint32_t tmpVal;

    /* Select auto mode and select ef clock */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);
    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    /* Program */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (1 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (1 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);
    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    /* Add delay for POR to be stable */
    arch_delay_us(4);

    /* Trigger */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (1 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (1 << EF_CTRL_EF_IF_0_RW_POS) |
             (1 << EF_CTRL_EF_IF_0_TRIG_POS);
    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
}

void ATTR_TCM_SECTION EF_Ctrl_Load_Efuse_R0(void)
{
    uint32_t tmpVal;
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

    EF_CTRL_DATA0_CLEAR;

    /* Trigger read */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);
    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (1 << EF_CTRL_EF_IF_0_TRIG_POS);
    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    arch_delay_us(10);

    /* Wait for efuse control idle*/
    do {
        tmpVal = readl(EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
        timeout--;

        if (timeout == 0) {
            break;
        }
    } while (reg_is_bit_set(tmpVal, EF_CTRL_EF_IF_0_BUSY) ||
             (!reg_is_bit_set(tmpVal, EF_CTRL_EF_IF_0_AUTOLOAD_DONE)));

    /* Switch to AHB clock */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);
    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
}

int ATTR_TCM_SECTION EF_Ctrl_Busy(void)
{
    uint32_t tmpVal;

    tmpVal = readl(EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    if (reg_is_bit_set(tmpVal, EF_CTRL_EF_IF_0_BUSY)) {
        return SET;
    }

    return RESET;
}

int ATTR_TCM_SECTION EF_Ctrl_AutoLoad_Done(void)
{
    uint32_t tmpVal;

    tmpVal = readl(EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    if (reg_is_bit_set(tmpVal, EF_CTRL_EF_IF_0_AUTOLOAD_DONE)) {
        return SET;
    } else {
        return RESET;
    }
}

void EF_Ctrl_Write_Dbg_Pwd(uint32_t passWdLow, uint32_t passWdHigh, uint8_t program)
{
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    writel(passWdLow, EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W0_OFFSET);
    writel(passWdHigh, EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W1_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_Dbg_Pwd(uint32_t *passWdLow, uint32_t *passWdHigh)
{
    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    *passWdLow = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W0_OFFSET);
    *passWdHigh = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W1_OFFSET);
}

void EF_Ctrl_Readlock_Dbg_Pwd(uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_DATA_0_RD_LOCK_KEY_SLOT_5);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Writelock_Dbg_Pwd(uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_DATA_0_WR_LOCK_KEY_SLOT_5);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Write_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg, uint8_t program)
{
    uint32_t tmpVal;
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_CFG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EF_DATA_0_EF_DBG_MODE, cfg->ef_dbg_mode);
    tmpVal = reg_set_bits(tmpVal, EF_DATA_0_EF_DBG_JTAG_0_DIS, cfg->ef_dbg_jtag_0_dis);
    tmpVal = reg_set_bits(tmpVal, EF_DATA_0_EF_SBOOT_EN, cfg->ef_sboot_en);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_EF_CFG_0_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg)
{
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_CFG_0_OFFSET);

    cfg->ef_dbg_mode = (EF_Ctrl_Dbg_Mode_Type)reg_get_bits(tmpVal, EF_DATA_0_EF_DBG_MODE);
    cfg->ef_dbg_jtag_0_dis = reg_get_bits(tmpVal, EF_DATA_0_EF_DBG_JTAG_0_DIS);
    cfg->ef_sboot_en = reg_get_bits(tmpVal, EF_DATA_0_EF_SBOOT_EN);
}

void EF_Ctrl_Write_Secure_Boot(uint8_t sign[1], uint8_t aes[1], uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_CFG_0_OFFSET);

    tmpVal = reg_set_bits(tmpVal, EF_DATA_0_EF_SBOOT_SIGN_MODE, sign[0]);

    if (aes[0] != EF_CTRL_SF_AES_NONE) {
        tmpVal = reg_set_bits(tmpVal, EF_DATA_0_EF_SF_AES_MODE, aes[0]);
        tmpVal = reg_set_bits(tmpVal, EF_DATA_0_EF_CPU0_ENC_EN, 1);
    }

    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_EF_CFG_0_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_Secure_Boot(uint8_t sign[1], uint8_t aes[1])
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_CFG_0_OFFSET);

    tmpVal2 = reg_get_bits(tmpVal, EF_DATA_0_EF_SBOOT_SIGN_MODE);
    sign[0] = (EF_Ctrl_Sign_Type)(tmpVal2 & 0x01);

    tmpVal2 = reg_get_bits(tmpVal, EF_DATA_0_EF_CPU0_ENC_EN);

    if (tmpVal2) {
        aes[0] = (EF_Ctrl_SF_AES_Type)reg_get_bits(tmpVal, EF_DATA_0_EF_SF_AES_MODE);
    } else {
        aes[0] = EF_CTRL_SF_AES_NONE;
    }
}

void EF_Ctrl_Write_Sw_Usage(uint32_t index, uint32_t usage, uint8_t program)
{
    /* switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (index == 0) {
        writel(usage, EF_DATA_BASE + EF_DATA_0_EF_SW_USAGE_0_OFFSET);
    } else if (index == 1) {
        writel(usage, EF_DATA_BASE + EF_DATA_0_EF_SW_USAGE_1_OFFSET);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_Sw_Usage(uint32_t index, uint32_t *usage)
{
 
    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    if (index == 0) {
        *usage = readl(EF_DATA_BASE + EF_DATA_0_EF_SW_USAGE_0_OFFSET);
    } else if (index == 1) {
        *usage = readl(EF_DATA_BASE + EF_DATA_0_EF_SW_USAGE_1_OFFSET);
    }
}

void EF_Ctrl_Writelock_Sw_Usage(uint32_t index, uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (index == 0) {
        tmpVal = reg_set_bit(tmpVal, EF_DATA_0_WR_LOCK_SW_USAGE_0);
    }

    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Write_MAC_Address(uint8_t mac[8], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* The low 32 bits */
    writel(DRV_WORD_FROM_BYTES(maclow), EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_LOW_OFFSET);
    /* The high 16 bits */
    writel(DRV_WORD_FROM_BYTES(machigh), EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_HIGH_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

uint32_t EF_Ctrl_Get_Byte_Zero_Cnt(uint8_t val)
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

uint8_t EF_Ctrl_Is_All_Bits_Zero(uint32_t val, uint8_t start, uint8_t len)
{
    uint32_t mask = 0;

    val = (val >> start);

    if (len >= 32) {
        mask = 0xffffffff;
    } else {
        mask = (1 << len) - 1;
    }

    if ((val & mask) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int EF_Ctrl_Read_MAC_Address(uint8_t mac[8])
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;
    uint32_t i = 0;
    uint32_t cnt = 0;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_LOW_OFFSET);
    DRV_WORD_TO_BYTES(maclow, tmpVal);

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_WIFI_MAC_HIGH_OFFSET);
    DRV_WORD_TO_BYTES(machigh, tmpVal);

    /* Get original parity */
    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_EF_SW_USAGE_1_OFFSET);

    /* Check parity */
    for (i = 0; i < 8; i++) {
        cnt += EF_Ctrl_Get_Byte_Zero_Cnt(mac[i]);
    }
    if ((cnt & 0x3f) == (tmpVal & 0x3f)) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}
void EF_Ctrl_Writelock_MAC_Address(uint8_t program)
{
    uint32_t tmpVal;
    EF_Ctrl_Sw_AHB_Clk_0();
    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_DATA_0_WR_LOCK_WIFI_MAC);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

int EF_Ctrl_Read_Chip_ID(uint8_t chipID[8])
{
    return EF_Ctrl_Read_MAC_Address(chipID);
}
void ATTR_TCM_SECTION EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo)
{
    uint32_t tmpVal;
    uint32_t *p = (uint32_t *)deviceInfo;

    EF_Ctrl_Read_Sw_Usage(1, &tmpVal);

    *p = tmpVal;
}

uint8_t EF_Ctrl_Is_CapCode_Empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp = 0xffffffff;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
        return (EF_Ctrl_Is_All_Bits_Zero(tmp, 0, 7));
    } else if (slot == 1) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
        return (EF_Ctrl_Is_All_Bits_Zero(tmp, 10, 7));
    } else if (slot == 2) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
        return (EF_Ctrl_Is_All_Bits_Zero(tmp, 20, 7));
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  Efuse write Cap code
 *
 * @param  slot: Cap code slot
 * @param  code: Cap code value
 * @param  program: Whether program
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
int EF_Ctrl_Write_CapCode_Opt(uint8_t slot, uint8_t code, uint8_t program)
{
    uint32_t tmp;
    uint8_t trim;

    if (slot >= 3) {
        return ERROR;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();
    EF_CTRL_LOAD_BEFORE_READ_R0;

    trim = (code << 1);
    trim |= (1 << 0);

    if (slot == 0) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
        writel(tmp | (trim << 0), EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
    } else if (slot == 1) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
        writel(tmp | (trim << 10), EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
    } else if (slot == 2) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
        writel(tmp | (trim << 20), EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
    while (SET == EF_Ctrl_Busy())
        ;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read Cap code
 *
 * @param  slot: Cap code slot
 * @param  code: Cap code pointer
 * @param  reload: Whether reload
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
int EF_Ctrl_Read_CapCode_Opt(uint8_t slot, uint8_t *code, uint8_t reload)
{
    uint32_t tmp;
    uint8_t trim = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_5_W3_OFFSET);
        trim = (tmp >> 25) & 0x7f;
    } else if (slot == 1) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
        trim = (tmp >> 9) & 0x7f;
    } else if (slot == 2) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET);
        trim = (tmp >> 25) & 0x7f;
    }

    if (trim & 0x01) {
        *code = trim >> 1;
        return SUCCESS;
    }
    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Whether power offset slot is empty
 *
 * @param  slot: Power offset code slot
 * @param  reload: Whether reload
 *
 * @return 0 for all slots full,1 for others
 *
*******************************************************************************/
uint8_t EF_Ctrl_Is_PowerOffset_Slot_Empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp1 = 0xffffffff;
    uint32_t part1Empty = 0, part2Empty = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmp1 = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET);
        part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 0, 16));
        part2Empty = 1;
    } else if (slot == 1) {
        tmp1 = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET);
        part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 18, 31));
        tmp1 = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET);
        part2Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 0, 3));
    } else if (slot == 2) {
        tmp1 = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET);
        part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 6, 21));
        part2Empty = 1;
    }

    return (part1Empty && part2Empty);
}

/****************************************************************************/ /**
 * @brief  Efuse write power offset
 *
 * @param  slot: Power offset slot
 * @param  pwrOffset[2]: Power offset value array
 * @param  program: Whether program
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
int EF_Ctrl_Write_PowerOffset_Opt(uint8_t slot, int8_t pwrOffset[4], uint8_t program)
{
    uint32_t tmp = 0;
    uint32_t k = 0;
    uint32_t Value = 0;

    if (slot >= 3) {
        return ERROR;
    }

    for (k = 0; k < 4; k++) {
        /* Use 4 bits as signed value */
        if (pwrOffset[k] > 7) {
            pwrOffset[k] = 7;
        }
        if (pwrOffset[k] < -8) {
            pwrOffset[k] = -8;
        }
        Value += (uint32_t)(pwrOffset[k] & 0x0f) << (k * 4);
    }

    if (slot == 0) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET);
        tmp |= (Value << 0);
        tmp |= (1 << 17);
        writel(tmp, EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET);
    } else if (slot == 1) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET);
        tmp |= ((Value & 0xfff) << 18);
        writel(tmp, EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET);

        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET);
        tmp |= ((Value >> 12) & 0xf);
        tmp |= (1 << 5);
        writel(tmp, EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET);
    } else if (slot == 2) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET);
        tmp |= (Value << 6);
        tmp |= (1 << 23);
        writel(tmp, EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }

    while (SET == EF_Ctrl_Busy())
        ;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read poweroffset value
 *
 * @param  slot: Power offset slot
 * @param  pwrOffset[4]: Power offset array
 * @param  reload: Whether reload
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
int EF_Ctrl_Read_PowerOffset_Opt(uint8_t slot, int8_t pwrOffset[4], uint8_t reload)
{
    uint32_t pwrOffsetValue = 0;
    uint32_t tmp = 0, k;
    uint8_t en = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET);
        en = (tmp >> 17) & 0x01;
        pwrOffsetValue = (tmp >> 0) & 0xffff;
    } else if (slot == 1) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET);
        pwrOffsetValue = (tmp >> 18) & 0xfff;
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET);
        en = (tmp >> 5) & 0x01;
        pwrOffsetValue = (((tmp >> 0) & 0xf) << 12) | pwrOffsetValue;
    } else if (slot == 2) {
        tmp = readl(EF_DATA_BASE + EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET);
        en = (tmp >> 23) & 0x01;
        pwrOffsetValue = (tmp >> 6) & 0xffff;

    }

    if (en) {
        for (k = 0; k < 4; k++) {
            tmp = (pwrOffsetValue >> (k * 4)) & 0x0f;
            if (tmp >= 8) {
                pwrOffset[k] = tmp - 16;
            } else {
                pwrOffset[k] = tmp;
            }
        }
        return SUCCESS;
    }
    return ERROR;
}

void EF_Ctrl_Write_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program)
{
    uint32_t *pAESKeyStart0 = (uint32_t *)(EF_DATA_BASE + 0x1C);

    if (index > 5) {
        return;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Every key is 4 words len*/
    arch_memcpy4(pAESKeyStart0 + index * 4, keyData, len);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len)
{
    uint32_t *pAESKeyStart0 = (uint32_t *)(EF_DATA_BASE + 0x1C);

    if (index > 5) {
        return;
    }

    /* Trigger read data from efuse*/
    EF_CTRL_LOAD_BEFORE_READ_R0;

    /* Every key is 4 words len*/
    arch_memcpy4(keyData, pAESKeyStart0 + index * 4, len);
}

void EF_Ctrl_Writelock_AES_Key(uint8_t index, uint8_t program)
{
    uint32_t tmpVal;

    if (index > 5) {
        return;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (index <= 3) {
        tmpVal |= (1 << (index + 19));
    } else {
        tmpVal |= (1 << (index + 19));
        tmpVal |= (1 << (index - 4 + 13));
    }

    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Readlock_AES_Key(uint8_t index, uint8_t program)
{
    uint32_t tmpVal;

    if (index > 5) {
        return;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    tmpVal |= (1 << (index + 26));
    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Program_Direct_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Add delay for CLK to be stable */
    arch_delay_us(4);

    if (data != NULL)
        arch_memcpy4(pEfuseStart0 + index, data, len);

    EF_Ctrl_Program_Efuse_0();
}

void EF_Ctrl_Read_Direct_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);

    EF_CTRL_LOAD_BEFORE_READ_R0;

    arch_memcpy4(data, pEfuseStart0 + index, len);
}
void ATTR_TCM_SECTION EF_Ctrl_Clear(uint32_t index, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);
    uint32_t i = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Clear data */
    for (i = 0; i < len; i++) {
        pEfuseStart0[index + i] = 0;
    }
}
void EF_Ctrl_Crc_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(EF_DATA_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_CTRL_EF_CRC_TRIG);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_MODE);
    tmpVal = reg_set_bit(tmpVal, EF_CTRL_EF_CRC_DOUT_INV_EN);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_DOUT_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_DIN_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_INT_CLR);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_INT_SET);
    writel(tmpVal, EF_DATA_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);

    tmpVal = readl(EF_DATA_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_CTRL_EF_CRC_EN);
    writel(tmpVal, EF_DATA_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
}

int EF_Ctrl_Crc_Is_Busy(void)
{
    uint32_t tmpVal;

    tmpVal = readl(EF_DATA_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
    return (int)reg_is_bit_set(tmpVal, EF_CTRL_EF_CRC_BUSY);
}

void EF_Ctrl_Crc_Set_Golden(uint32_t goldenValue)
{
    writel(goldenValue, EF_DATA_BASE + EF_CTRL_EF_CRC_CTRL_4_OFFSET);
}

int EF_Ctrl_Crc_Result(void)
{
    uint32_t tmpVal;

    tmpVal = readl(EF_DATA_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
    return (int)reg_is_bit_set(tmpVal, EF_CTRL_EF_CRC_ERROR);
}

