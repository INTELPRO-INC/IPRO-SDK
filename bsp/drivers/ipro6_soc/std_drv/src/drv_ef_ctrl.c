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

#define EF_CTRL_EFUSE_CYCLE_PROTECT (0xbf << 24)
#define EF_CTRL_EFUSE_CTRL_PROTECT  (0xbf << 8)
#define EF_CTRL_DFT_TIMEOUT_VAL     (160 * 1000)
#ifndef BOOTROM
#define EF_CTRL_LOAD_BEFORE_READ_R0 EF_Ctrl_Load_Efuse_R0()
#else
#define EF_CTRL_LOAD_BEFORE_READ_R0
#endif
#define EF_CTRL_DATA0_CLEAR EF_Ctrl_Clear(0, EF_CTRL_EFUSE_R0_SIZE / 4)

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION EF_Ctrl_Get_Customer_PIDVID(uint16_t pid[1], uint16_t vid[1])
{
    uint32_t tmpVal;

    EF_Ctrl_Read_Sw_Usage(4, &tmpVal);

    pid[0] = (uint16_t)(tmpVal & 0xFFFF);
    vid[0] = (uint16_t)(tmpVal >> 16);

    return SUCCESS;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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

    /* Wait for efuse control idle */
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION EF_Ctrl_Busy(void)
{
    if (reg_is_bit_set(readl(EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET), EF_CTRL_EF_IF_0_BUSY)) {
        return SET;
    }

    return RESET;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

void EF_Ctrl_Write_Dbg_Pwd(uint8_t slot, uint32_t passWdLow, uint32_t passWdHigh, uint8_t program)
{
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (slot == 0) {
        writel(passWdLow, EF_DATA_BASE + EF_DATA_EF_DBG_PWD_LOW_OFFSET);
        writel(passWdHigh, EF_DATA_BASE + EF_DATA_EF_DBG_PWD_HIGH_OFFSET);
    } else if (slot == 1) {
        writel(passWdLow, EF_DATA_BASE + EF_DATA_EF_DBG_PWD2_LOW_OFFSET);
        writel(passWdHigh, EF_DATA_BASE + EF_DATA_EF_DBG_PWD2_HIGH_OFFSET);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_Dbg_Pwd(uint8_t slot, uint32_t *passWdLow, uint32_t *passWdHigh)
{
    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    if (slot == 0) {
        *passWdLow = readl(EF_DATA_BASE + EF_DATA_EF_DBG_PWD_LOW_OFFSET);
        *passWdHigh = readl(EF_DATA_BASE + EF_DATA_EF_DBG_PWD_HIGH_OFFSET);
    } else if (slot == 1) {
        *passWdLow = readl(EF_DATA_BASE + EF_DATA_EF_DBG_PWD2_LOW_OFFSET);
        *passWdHigh = readl(EF_DATA_BASE + EF_DATA_EF_DBG_PWD2_HIGH_OFFSET);
    }
}

void EF_Ctrl_Readlock_Dbg_Pwd(uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_DATA_RD_LOCK_DBG_PWD);
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
    tmpVal = reg_set_bit(tmpVal, EF_DATA_WR_LOCK_DBG_PWD);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Write_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg, uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EF_DATA_EF_DBG_MODE, cfg->ef_dbg_mode);
    tmpVal = reg_set_bits(tmpVal, EF_DATA_EF_DBG_JTAG_0_DIS, cfg->ef_dbg_jtag_0_dis);
    tmpVal = reg_set_bits(tmpVal, EF_DATA_EF_DBG_JTAG_1_DIS, cfg->ef_dbg_jtag_1_dis);
    tmpVal = reg_set_bits(tmpVal, EF_DATA_EF_SBOOT_EN, cfg->ef_sboot_en);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg)
{
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);

    cfg->ef_dbg_mode = (uint8_t)reg_get_bits(tmpVal, EF_DATA_EF_DBG_MODE);
    cfg->ef_dbg_jtag_0_dis = reg_get_bits(tmpVal, EF_DATA_EF_DBG_JTAG_0_DIS);
    cfg->ef_dbg_jtag_1_dis = reg_get_bits(tmpVal, EF_DATA_EF_DBG_JTAG_1_DIS);
    cfg->ef_sboot_en = reg_get_bits(tmpVal, EF_DATA_EF_SBOOT_EN);
}

void EF_Ctrl_Write_Secure_Boot(uint8_t sign[1], uint8_t aes[1], uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (aes[0] != EF_CTRL_SF_AES_NONE) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);
        tmpVal = reg_set_bits(tmpVal, EF_DATA_EF_SF_AES_MODE, aes[0]);
        writel(tmpVal, EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);
    }

    if (EF_CTRL_SIGN_NONE != sign[0]) {
        tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_0_OFFSET);
        tmpVal = reg_set_bits(tmpVal, EF_DATA_EF_SBOOT_SIGN_MODE, sign[0]);
        writel(tmpVal, EF_DATA_BASE + EF_DATA_EF_SW_USAGE_0_OFFSET);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_Secure_Boot(uint8_t aes[2])
{
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);

    aes[1] = aes[0] = (uint8_t)reg_get_bits(tmpVal, EF_DATA_EF_SF_AES_MODE);
}

int EF_Ctrl_Read_Bus_Remap(void)
{
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);

    if (reg_is_bit_set(tmpVal, EF_DATA_EF_BUS_RMP_SW_EN)) {
        return ENABLE;
    }
    return DISABLE;
}

void EF_Ctrl_Set_sf_key_re_sel(uint8_t ef_sf_key_re_sel)
{
    uint32_t tmpVal;

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, EF_DATA_EF_SF_KEY_RE_SEL, ef_sf_key_re_sel);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_EF_CFG_0_OFFSET);
}

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_DcdcDis_Trim(Efuse_Ana_DcdcDis_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_DAT_1_RSVD_1_OFFSET);
    trim->dcdcDisAonEn = (tmpVal >> 19) & 0x01;
    trim->dcdcDisAonParity = (tmpVal >> 18) & 0x01;
    trim->dcdcDisAon = (tmpVal >> 17) & 0x01;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_DcdcVoutSel_Trim(Efuse_Ana_DcdcVoutSel_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_DAT_1_RSVD_1_OFFSET);
    trim->dcdcVoutSelAonEn = (tmpVal >> 16) & 0x01;
    trim->dcdcVoutSelAonParity = (tmpVal >> 15) & 0x01;
    trim->dcdcVoutSelAon = (tmpVal >> 10) & 0x1F;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_Ldo18ioVoutSel_Trim(Efuse_Ana_Ldo18ioVoutSel_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W3_OFFSET);
    trim->ldo18ioVoutSelEn = (tmpVal >> 25) & 0x01;
    trim->ldo18ioVoutSelParity = (tmpVal >> 24) & 0x01;
    trim->ldo18ioVoutSelAon = (tmpVal >> 20) & 0x0F;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_Ldo18ioBypass_Trim(Efuse_Ana_Ldo18ioBypass_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_DAT_1_RSVD_1_OFFSET);
    trim->ldo18ioBypassEn = (tmpVal >> 9) & 0x01;
    trim->ldo18ioBypassParity = (tmpVal >> 8) & 0x01;
    trim->ldo18ioBypassAon = (tmpVal >> 4) & 0x01;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_Ldo33VoutTrim_Trim(Efuse_Ana_Ldo33VoutTrim_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W3_OFFSET);
    trim->ldo33VoutTrimAonEn = (tmpVal >> 13) & 0x01;
    trim->ldo33VoutTrimAonParity = (tmpVal >> 12) & 0x01;
    trim->ldo33VoutTrimAon = (tmpVal >> 8) & 0x0F;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_Ldo18ioVoutTrim_Trim(Efuse_Ana_Ldo18ioVoutTrim_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W3_OFFSET);
    trim->ldo18ioVoutTrimEn = (tmpVal >> 19) & 0x01;
    trim->ldo18ioVoutTrimParity = (tmpVal >> 18) & 0x01;
    trim->ldo18ioVoutTrimAon = (tmpVal >> 14) & 0x0F;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_DcdcVoutTrim_Trim(Efuse_Ana_DcdcVoutTrim_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W3_OFFSET);
    trim->dcdcVoutTrimAonEn = (tmpVal >> 31) & 0x01;
    trim->dcdcVoutTrimAonParity = (tmpVal >> 30) & 0x01;
    trim->dcdcVoutTrimAon = (tmpVal >> 26) & 0x0F;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_Ldo11socVoutTrim_Trim(Efuse_Ana_Ldo11socVoutTrim_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W3_OFFSET);
    trim->ldo11socVoutTrimAonEn = (tmpVal >> 7) & 0x01;
    trim->ldo11socVoutTrimAonTrim = (tmpVal >> 6) & 0x01;
    trim->ldo11socVoutTrimAon = (tmpVal >> 2) & 0x0F;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_RC32M_Trim(Efuse_Ana_RC32M_Trim_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W3_OFFSET);
    trim->rc32mCodeFrExt2En = (tmpVal >> 1) & 0x01;
    trim->rc32mCodeFrExt2Parity = (tmpVal >> 0) & 0x01;
    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    trim->rc32mCodeFrExt2 = (tmpVal >> 4) & 0xFF;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_RC32K_Trim(Efuse_Ana_RC32K_Trim_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_10_W3_OFFSET);
    trim->rc32kCodeFrExtEn = (tmpVal >> 19) & 0x01;
    trim->rc32kCodeFrExtParity = (tmpVal >> 18) & 0x01;
    trim->rc32kCodeFrExt = (tmpVal >> 8) & 0x3FF;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_Usb20RcalCode_Trim(Efuse_Ana_Usb20RcalCode_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_DAT_1_RSVD_2_OFFSET);
    trim->usb20RcalCodeAonEn = (tmpVal >> 15) & 0x01;
    trim->usb20RcalCodeAonParity = (tmpVal >> 14) & 0x01;
    trim->usb20RcalCodeAon = (tmpVal >> 8) & 0x3F;
}
#endif

void ATTR_CLOCK_SECTION EF_Ctrl_Read_TSEN_Trim(Efuse_TSEN_Refcode_Corner_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_DAT_1_RSVD_0_OFFSET);
    trim->tsenRefcodeCorner = (tmpVal >> 0) & 0xfff;
    trim->tsenRefcodeCornerParity = (tmpVal >> 12) & 0x01;
    trim->tsenRefcodeCornerEn = (tmpVal >> 13) & 0x01;
}

void ATTR_CLOCK_SECTION EF_Ctrl_Read_ADC_Gain_Trim(Efuse_ADC_Gain_Coeff_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_DAT_1_RSVD_0_OFFSET);
    trim->adcGainCoeff = (tmpVal >> 14) & 0xfff;
    trim->adcGainCoeffParity = (tmpVal >> 26) & 0x01;
    trim->adcGainCoeffEn = (tmpVal >> 27) & 0x01;
}

void EF_Ctrl_Write_Sw_Usage(uint32_t index, uint32_t usage, uint8_t program)
{
    /* switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    switch (index) {
        case 0:
            writel(usage, EF_DATA_BASE + EF_DATA_EF_SW_USAGE_0_OFFSET);
            break;

        case 1:
            writel(usage, EF_DATA_BASE + EF_DATA_EF_SW_USAGE_1_OFFSET);
            break;

        case 2:
            writel(usage, EF_DATA_BASE + EF_DATA_EF_SW_USAGE_2_OFFSET);
            break;

        case 3:
            writel(usage, EF_DATA_BASE + EF_DATA_EF_SW_USAGE_3_OFFSET);
            break;

        case 4:
            writel(usage, EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W0_OFFSET);
            break;

        default:
            break;
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_Sw_Usage(uint32_t index, uint32_t *usage)
{
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    switch (index) {
        case 0:
            *usage = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_0_OFFSET);
            break;

        case 1:
            *usage = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_1_OFFSET);
            break;

        case 2:
            *usage = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_2_OFFSET);
            break;

        case 3:
            *usage = readl(EF_DATA_BASE + EF_DATA_EF_SW_USAGE_3_OFFSET);
            break;

        case 4:
            *usage = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_11_W0_OFFSET);
            break;

        default:
            break;
    }
}

void EF_Ctrl_Writelock_Sw_Usage(uint32_t index, uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (index == 0) {
        tmpVal = reg_set_bit(tmpVal, EF_DATA_WR_LOCK_SW_USAGE_0);
    } else if (index == 1) {
        tmpVal = reg_set_bit(tmpVal, EF_DATA_WR_LOCK_SW_USAGE_1);
    } else if (index == 2) {
        tmpVal = reg_set_bit(tmpVal, EF_DATA_WR_LOCK_SW_USAGE_2);
    } else if (index == 3) {
        tmpVal = reg_set_bit(tmpVal, EF_DATA_WR_LOCK_SW_USAGE_3);
    }

    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Write_MAC_Address(uint8_t mac[6], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* The low 32 bits */
    writel(DRV_WORD_FROM_BYTES(maclow), EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_LOW_OFFSET);
    /* The high 16 bits */
    tmpVal = machigh[0] + (machigh[1] << 8);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

static uint32_t EF_Ctrl_Get_Byte_Zero_Cnt(uint8_t val)
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

int EF_Ctrl_Read_MAC_Address(uint8_t mac[6])
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;
    uint32_t i = 0;
    uint32_t cnt = 0;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_LOW_OFFSET);
    DRV_WORD_TO_BYTES(maclow, tmpVal);

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);
    machigh[0] = tmpVal & 0xff;
    machigh[1] = (tmpVal >> 8) & 0xff;

    /* Check parity */
    for (i = 0; i < 6; i++) {
        cnt += EF_Ctrl_Get_Byte_Zero_Cnt(mac[i]);
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

int EF_Ctrl_Read_MAC_Address_Raw(uint8_t mac[7])
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_LOW_OFFSET);
    DRV_WORD_TO_BYTES(maclow, tmpVal);

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);
    machigh[0] = tmpVal & 0xff;
    machigh[1] = (tmpVal >> 8) & 0xff;
    machigh[2] = (tmpVal >> 16) & 0xff;

    return SUCCESS;
}

void EF_Ctrl_Writelock_MAC_Address(uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_DATA_WR_LOCK_WIFI_MAC);
    writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
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

int EF_Ctrl_Read_Chip_ID(uint8_t chipID[8])
{
    chipID[6] = 0;
    chipID[7] = 0;
    return EF_Ctrl_Read_MAC_Address_Raw(chipID);
}

uint8_t EF_Ctrl_Read_FlashDlyCoe(void)
{
    EF_CTRL_LOAD_BEFORE_READ_R0;

    return reg_get_bits(readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET), EF_DATA_FLASH_DLY_COE);
}

void EF_Ctrl_Write_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program)
{
    uint32_t *pAESKeyStart0 = (uint32_t *)(EF_DATA_BASE + 0x1C);
    uint32_t *pAESKeyStart1 = (uint32_t *)(EF_DATA_BASE + 0x80);

    /* slot_w0~slot_w3,slot_w11 in ef_data0
       slot_w4~slot_w10,in ef_data1 */

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if ((index <= 3) || (index == 11)) {
        index = ((index == 11) ? 5 : index);

        /* Every key is 4 words len*/
        ARCH_MemCpy4(pAESKeyStart0 + index * 4, keyData, len);
    } else if ((index < 11) && (index > 3)) {
        index = index - 4;

        /* Every key is 4 words len*/
        ARCH_MemCpy4(pAESKeyStart1 + index * 4, keyData, len);
    }
    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len)
{
    uint32_t *pAESKeyStart0 = (uint32_t *)(EF_DATA_BASE + 0x1C);
    uint32_t *pAESKeyStart1 = (uint32_t *)(EF_DATA_BASE + 0x80);

    /* Trigger read data from efuse*/
    EF_CTRL_LOAD_BEFORE_READ_R0;

    if ((index <= 3) || (index == 11)) {
        index = ((index == 11) ? 5 : index);

        /* Every key is 4 words len*/
        ARCH_MemCpy4(keyData, pAESKeyStart0 + index * 4, len);
    } else if ((index < 11) && (index > 3)) {
        index = index - 4;

        /* Every key is 4 words len*/
        ARCH_MemCpy4(keyData, pAESKeyStart1 + index * 4, len);
    }
}

void EF_Ctrl_Writelock_AES_Key(uint8_t index, uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if ((index <= 3) || (index == 11)) {
        index = ((index == 11) ? 8 : index);

        tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
        tmpVal |= (1 << (index + 17));
        writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    } else if ((index < 11) && (index > 3)) {
        index = index - 4;

        tmpVal = readl(EF_DATA_BASE + EF_DATA_1_LOCK_OFFSET);
        tmpVal |= (1 << (index + 15));
        writel(tmpVal, EF_DATA_BASE + EF_DATA_1_LOCK_OFFSET);
    }
    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Readlock_AES_Key(uint8_t index, uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if ((index <= 3) || (index == 11)) {
        index = ((index == 11) ? 4 : index);

        tmpVal = readl(EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
        tmpVal |= (1 << (index + 27));
        writel(tmpVal, EF_DATA_BASE + EF_DATA_0_LOCK_OFFSET);
    } else if ((index < 11) && (index > 3)) {
        index = index - 4;

        tmpVal = readl(EF_DATA_BASE + EF_DATA_1_LOCK_OFFSET);
        tmpVal |= (1 << (index + 25));
        writel(tmpVal, EF_DATA_BASE + EF_DATA_1_LOCK_OFFSET);
    }
    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

void EF_Ctrl_Program_Direct(uint32_t offset, uint32_t *pword, uint32_t count)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + offset);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Add delay for CLK to be stable */
    arch_delay_us(4);

    if (pword != NULL) {
        ARCH_MemCpy4(pEfuseStart0, pword, count);
    }

    EF_Ctrl_Program_Efuse_0();
}

void EF_Ctrl_Program_Direct_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Add delay for CLK to be stable */
    arch_delay_us(4);

    if (data != NULL) {
        ARCH_MemCpy4(pEfuseStart0 + index, data, len);
    }

    EF_Ctrl_Program_Efuse_0();
}

void EF_Ctrl_Read_Direct_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);

    EF_CTRL_LOAD_BEFORE_READ_R0;

    ARCH_MemCpy4(data, pEfuseStart0 + index, len);
}

void EF_Ctrl_Read_Direct(uint32_t offset, uint32_t *pword, uint32_t count)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + offset);

    EF_CTRL_LOAD_BEFORE_READ_R0;

    ARCH_MemCpy4(pword, pEfuseStart0, count);
}

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

void EF_Ctrl_Crc_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(EF_CTRL_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_CTRL_EF_CRC_TRIG);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_MODE);
    tmpVal = reg_set_bit(tmpVal, EF_CTRL_EF_CRC_DOUT_INV_EN);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_DOUT_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_DIN_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_INT_CLR);
    tmpVal = reg_clr_bit(tmpVal, EF_CTRL_EF_CRC_INT_SET);
    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);

    tmpVal = readl(EF_CTRL_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, EF_CTRL_EF_CRC_EN);
    writel(tmpVal, EF_CTRL_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
}

int EF_Ctrl_Crc_Is_Busy(void)
{
    uint32_t tmpVal;

    tmpVal = readl(EF_CTRL_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
    return (int)reg_is_bit_set(tmpVal, EF_CTRL_EF_CRC_BUSY);
}

void EF_Ctrl_Crc_Set_Golden(uint32_t goldenValue)
{
    writel(goldenValue, EF_CTRL_BASE + EF_CTRL_EF_CRC_CTRL_4_OFFSET);
}

int EF_Ctrl_Crc_Result(void)
{
    uint32_t tmpVal;

    tmpVal = readl(EF_CTRL_BASE + EF_CTRL_EF_CRC_CTRL_0_OFFSET);
    return (int)reg_is_bit_set(tmpVal, EF_CTRL_EF_CRC_ERROR);
}

void EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo)
{
    uint32_t tmpVal;
    uint32_t *p = (uint32_t *)deviceInfo;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);
    *p = tmpVal;
}

