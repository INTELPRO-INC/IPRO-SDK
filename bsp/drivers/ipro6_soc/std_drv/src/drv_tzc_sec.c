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
#include "drv_tzc_sec.h"
#include "drv_glb.h"

void Tzc_Sec_Set_Sboot_Done(void)
{
    uint32_t tmpVal;
    /* Set Sboot done */
    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TZC_SEC_TZC_SBOOT_DONE, 0xf);
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_Set_Bus_Remap(uint8_t busRmpEn)
{
    uint32_t tmpVal;

    /* Set bus remap */
    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
    if (busRmpEn) {
        tmpVal = reg_set_bit(tmpVal, TZC_SEC_TZC_BUS_RMP_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, TZC_SEC_TZC_BUS_RMP_EN);
    }
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);

    /* Set Lock */
    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, TZC_SEC_TZC_BUS_RMP_EN_LOCK);
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_Set_Master_Group(TZC_SEC_Master_Type masterType, uint8_t group)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_TZMID_OFFSET);
    tmpVal2 = readl(TZ1_BASE + TZC_SEC_TZC_BMX_TZMID_LOCK_OFFSET);

    if (group == 0) {
        tmpVal &= (~(1 << masterType));
    } else {
        tmpVal |= (1 << masterType);
    }
    tmpVal |= (1 << (masterType + 16));
    tmpVal2 |= (1 << masterType);

    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_TZMID_OFFSET);
    writel(tmpVal2, TZ1_BASE + TZC_SEC_TZC_BMX_TZMID_LOCK_OFFSET);
}

void Tzc_Sec_Set_Slave_Group(TZC_SEC_Slave_Type slaveType, uint8_t group)
{
    uint32_t tmpVal;

    if (group > TZC_SEC_MAX_AUTH_GRP || slaveType >= TZC_SEC_SLAVE_MAX) {
        return;
    }
    group = 1 << (group);

    if (slaveType < TZC_SEC_SLAVE_S1_GLB) {
        tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_S0_OFFSET);
        /* set group */
        tmpVal &= (~(3 << (slaveType * 2)));
        tmpVal |= (group << (slaveType * 2));
        /* set lock*/
        tmpVal |= (1 << (slaveType + 16));
        writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_S0_OFFSET);
    } else if ((slaveType >= TZC_SEC_SLAVE_S1_GLB) && (slaveType < TZC_SEC_SLAVE_S2_EMI_MISC)) {
        slaveType -= TZC_SEC_SLAVE_S1_GLB;
        tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_S1_OFFSET);
        /* set group */
        tmpVal &= (~(3 << (slaveType * 2)));
        tmpVal |= (group << (slaveType * 2));
        writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_S1_OFFSET);

        tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_S_LOCK_OFFSET);
        /* set lock */
        tmpVal |= (1 << slaveType);
        writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_S_LOCK_OFFSET);
    } else if ((slaveType >= TZC_SEC_SLAVE_S2_EMI_MISC) && (slaveType < TZC_SEC_SLAVE_S1A_UART0)) {
        slaveType -= TZC_SEC_SLAVE_S2_EMI_MISC;

        tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_S2_OFFSET);
        /* set group */
        tmpVal &= (~(3 << (slaveType * 2)));
        tmpVal |= (group << (slaveType * 2));
        writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_S2_OFFSET);

        tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_S_LOCK_OFFSET);
        /* set lock */
        tmpVal |= (1 << (slaveType + (TZC_SEC_SLAVE_S2_EMI_MISC - TZC_SEC_SLAVE_S1_GLB)));
        writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_S_LOCK_OFFSET);
    } else {
        slaveType -= TZC_SEC_SLAVE_S1A_UART0;

        tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_S1A_OFFSET);
        /* set group */
        tmpVal &= (~(3 << (slaveType * 2)));
        tmpVal |= (group << (slaveType * 2));
        writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_S1A_OFFSET);

        tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_S1A_LOCK_OFFSET);
        /* set lock */
        tmpVal |= (1 << slaveType);
        writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_S1A_LOCK_OFFSET);
    }
}

void Tzc_Sec_Set_Codec_Group(uint8_t group)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_MM_BMX_TZMID_OFFSET);
    tmpVal2 = readl(TZ1_BASE + TZC_SEC_TZC_MM_BMX_TZMID_LOCK_OFFSET);

    if (group == 0) {
        tmpVal &= (~(1 << 2));
    } else {
        tmpVal |= (1 << 2);
    }
    tmpVal |= (1 << (2 + 16));
    tmpVal2 |= (1 << 2);

    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_MM_BMX_TZMID_OFFSET);
    writel(tmpVal2, TZ1_BASE + TZC_SEC_TZC_MM_BMX_TZMID_LOCK_OFFSET);
}

void Tzc_Sec_Set_Glb_Ctrl_Group(TZC_SEC_GLB_Ctrl_Type slaveType, uint8_t group)
{
    uint32_t tmpVal;

    if (slaveType >= TZC_SEC_GLB_CTRL_MAX || group > TZC_SEC_MAX_AUTH_GRP) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_GLB_CTRL_0_OFFSET);
    /* set group */
    tmpVal &= (~(3 << (slaveType * 2)));
    tmpVal |= (group << (slaveType * 2));
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_GLB_CTRL_0_OFFSET);

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_GLB_CTRL_2_OFFSET);
    /* set lock */
    tmpVal |= (1 << slaveType);
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_GLB_CTRL_2_OFFSET);
}

void Tzc_Sec_Set_CPU_Group(uint8_t cpu, uint8_t group)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    if (cpu == GLB_CORE_ID_M0) {
        tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_BMX_TZMID_OFFSET);
        tmpVal2 = readl(TZ1_BASE + TZC_SEC_TZC_BMX_TZMID_LOCK_OFFSET);

        tmpVal = reg_set_bits(tmpVal, TZC_SEC_TZC_CPU_TZMID, group);
        tmpVal2 = reg_set_bits(tmpVal2, TZC_SEC_TZC_CPU_TZMID_LOCK, 1);

        writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_BMX_TZMID_OFFSET);
        writel(tmpVal2, TZ1_BASE + TZC_SEC_TZC_BMX_TZMID_LOCK_OFFSET);
    }
}

void Tzc_Sec_ROM_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t alignEnd = (startAddr+length+1023)&~0x3FF;

    /* check the parameter */
        if (region >= 3 || group > TZC_SEC_MAX_AUTH_GRP) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);

    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_R0_OFFSET + region * 4);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 16);
    tmpVal |= 1 << (region + 24);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_ROM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t alignEnd = (startAddr+length+1023)&~0x3FF;

    /* check the parameter */
        if (region >= 3) {
        return;
    }
    group = group & 0xf;

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);

    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_R0_OFFSET + region * 4);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 16);
    tmpVal |= 1 << (region + 24);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_OCRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t alignEnd = (startAddr+length+1023)&~0x3FF;

    /* check the parameter */
        if (region >= 3) {
        return;
    }
    group = group & 0xf;

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_CTRL_OFFSET);

    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_R0_OFFSET + region * 4);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 16);
    tmpVal |= 1 << (region + 20);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_OCRAM_Access_Set_Regionx(uint8_t group)
{
    uint32_t tmpVal = 0;
    uint8_t region = 3;

    /* check the parameter */
        if (group > TZC_SEC_MAX_AUTH_GRP) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_CTRL_OFFSET);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 16);
    tmpVal |= 1 << (region + 20);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_OCRAM_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_WRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t alignEnd = (startAddr+length+1023)&~0x3FF;

    /* check the parameter */
        if (region >= 3) {
        return;
    }
    group = group & 0xf;

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_CTRL_OFFSET);

    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_R0_OFFSET + region * 4);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 16);
    tmpVal |= 1 << (region + 20);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_WRAM_Access_Set_Regionx(uint8_t group)
{
    uint32_t tmpVal = 0;
    uint8_t region = 3;

    /* check the parameter */
        if (group > TZC_SEC_MAX_AUTH_GRP) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_CTRL_OFFSET);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 16);
    tmpVal |= 1 << (region + 20);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_WRAM_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_Flash_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t tmpVal2 = 0;
    uint32_t alignEnd = (startAddr+length+1023)&~0x3FF;

    /* check the parameter */
        if (region >= 3 || group > TZC_SEC_MAX_AUTH_GRP) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);

    /* Set range */
    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_R0_OFFSET + region * 4);

    /* Set range MSB */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_MSB_OFFSET);
    tmpVal = tmpVal & (0xff << (8 * region));
    tmpVal2 = ((alignEnd >> 26) & 0x7) | (((startAddr >> 26) & 0x7) << 3);
    tmpVal2 = tmpVal2 << (8 * region);
    writel(tmpVal | tmpVal2, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_MSB_OFFSET);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 20);
    tmpVal |= 1 << (region + 25);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_Flash_Access_Set_Regionx(uint8_t group)
{
    uint32_t tmpVal = 0;
    uint8_t region = 4;

    /* check the parameter */
        if (group > TZC_SEC_MAX_AUTH_GRP) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 20);
    tmpVal |= 1 << (region + 25);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_Flash_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t tmpVal2 = 0;
    uint32_t alignEnd = (startAddr+length+1023)&~0x3FF;

    /* check the parameter */
        if (region >= 3) {
        return;
    }
    group = group & 0xf;

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);

    /* Set range */
    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_R0_OFFSET + region * 4);

    /* Set range MSB */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_MSB_OFFSET);
    tmpVal = tmpVal & (0xff << (8 * region));
    tmpVal2 = ((alignEnd >> 26) & 0x7) | (((startAddr >> 26) & 0x7) << 3);
    tmpVal2 = tmpVal2 << (8 * region);
    writel(tmpVal | tmpVal2, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_MSB_OFFSET);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 20);
    tmpVal |= 1 << (region + 25);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SF_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_PSRAMB_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t alignEnd = (startAddr+length+1023)&~0x3FF;

    /* check the parameter */
        if (region >= 3 || group > TZC_SEC_MAX_AUTH_GRP) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);

    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_R0_OFFSET + region * 4);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 16);
    tmpVal |= 1 << (region + 24);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_PSRAMB_Access_Release(void)
{
    uint32_t tmpVal = 0;
    uint32_t region = 0;
    /* set disable */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(1 << (region + 16)));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_PSRAMB_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t alignEnd = (startAddr+length+1023)&~0x3FF;

    /* check the parameter */
        if (region >= 3 || group > TZC_SEC_MAX_AUTH_GRP) {
        return;
    }
    group = 0xf & group;

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);

    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_R0_OFFSET + region * 4);

    /* set enable and lock */
    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);
    tmpVal |= 1 << (region + 16);
    tmpVal |= 1 << (region + 24);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_CTRL_OFFSET);
}

void Tzc_Sec_Set_Se_Ctrl_Mode(TZC_SEC_SE_Ctrl_Mode mode)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SE_CTRL_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TZC_SEC_TZC_SE_TZSID_CRMD, mode);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SE_CTRL_0_OFFSET);

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_SE_CTRL_2_OFFSET);
    /* set lock */
    tmpVal = reg_set_bits(tmpVal, TZC_SEC_TZC_SE_TZSID_CRMD_LOCK, 1);
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_SE_CTRL_2_OFFSET);
}

void Tzc_Sec_Set_Sf_Ctrl_Mode(TZC_SEC_SF_Ctrl_Mode mode)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SE_CTRL_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TZC_SEC_TZC_SF_TZSID_CRMD, mode);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SE_CTRL_1_OFFSET);

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_SE_CTRL_2_OFFSET);
    /* set lock */
    tmpVal = reg_set_bits(tmpVal, TZC_SEC_TZC_SF_TZSID_CRMD_LOCK, 1);
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_SE_CTRL_2_OFFSET);
}

void Tzc_Sec_Set_Se_Group(TZC_SEC_SE_Ctrl_Type slaveType, uint8_t group)
{
    uint32_t tmpVal;

    if (group > TZC_SEC_MAX_AUTH_GRP || slaveType >= TZC_SEC_SE_CTRL_MAX) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_SE_CTRL_0_OFFSET);
    /* set group */
    tmpVal &= (~(3 << (slaveType * 2)));
    tmpVal |= (group << (slaveType * 2));
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_SE_CTRL_0_OFFSET);

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_SE_CTRL_2_OFFSET);
    /* set lock */
    tmpVal |= (1 << (slaveType));
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_SE_CTRL_2_OFFSET);
}

void Tzc_Sec_Set_Sf_Group(TZC_SEC_SF_Ctrl_Type slaveType, uint8_t group)
{
    uint32_t tmpVal;

    if (group > TZC_SEC_MAX_AUTH_GRP || slaveType >= TZC_SEC_SF_CTRL_MAX) {
        return;
    }
    group = 1 << (group);

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_SE_CTRL_1_OFFSET);
    /* set group */
    tmpVal &= (~(3 << (slaveType * 2)));
    tmpVal |= (group << (slaveType * 2));
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_SE_CTRL_1_OFFSET);

    tmpVal = readl(TZ1_BASE + TZC_SEC_TZC_SE_CTRL_2_OFFSET);
    /* set lock */
    tmpVal |= (1 << (slaveType + 16));
    writel(tmpVal, TZ1_BASE + TZC_SEC_TZC_SE_CTRL_2_OFFSET);
}

void Tzc_Sec_Set_WTD_Rst_Delay(uint16_t delayValue)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(TZC_SEC_BASE + TZC_SEC_TZC_SE_CTRL_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TZC_SEC_TZC_SE_WDT_DLY, delayValue);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_SE_CTRL_0_OFFSET);
}

