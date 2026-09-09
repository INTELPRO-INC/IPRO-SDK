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



#include "drv_sf_ctrl.h"

#define SF_CTRL_BUSY_STATE_TIMEOUT      (5 * 160 * 1000)
#define SF_Ctrl_Get_AES_Region(addr, r) (addr + SF_CTRL_AES_REGION_OFFSET + (r)*0x80)

void ATTR_TCM_SECTION SF_Ctrl_SetDelay(uint8_t clkDelay)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    if (clkDelay > 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_READ_DLY_N, clkDelay - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
    }
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_0_OFFSET);
}

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Enable(const SF_Ctrl_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    if (cfg == NULL) {
        return;
    }

    /* Check the parameters */

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    if (cfg->en32bAddr) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_32B_ADR_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_32B_ADR_EN);
    }

    if (cfg->clkDelay > 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_READ_DLY_N, cfg->clkDelay - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
    }

    /* Serail out inverted, so sf ctrl send on negative edge */
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CLK_OUT_INV_SEL, cfg->clkInvert);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CLK_SF_RX_INV_SEL, cfg->rxClkInvert);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    /* Enable AHB access sram buffer and enable sf interface */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AHB2SRAM_EN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_EN);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    SF_Ctrl_Set_Owner(cfg->owner);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Bank2_Enable(const SF_Ctrl_Bank2_Cfg *bank2Cfg)
{
    uint32_t tmpVal = 0;

    if (bank2Cfg == NULL) {
        return;
    }

    /* Select if1 bank2 clock delay */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_12_OFFSET);

    if (bank2Cfg->bank2RxClkInvertSrc) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SRC);

        if (bank2Cfg->bank2RxClkInvertSel) {
            tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SEL);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SEL);
        }
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SRC);
    }

    if (bank2Cfg->bank2DelaySrc) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF2_IF_READ_DLY_SRC);

        if (bank2Cfg->bank2ClkDelay > 0) {
            tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF2_IF_READ_DLY_EN);
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IF_READ_DLY_N, bank2Cfg->bank2ClkDelay - 1);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF2_IF_READ_DLY_EN);
        }
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF2_IF_READ_DLY_SRC);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_12_OFFSET);

    /* Select sbus2 clock delay */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_0_OFFSET);

    if (bank2Cfg->bank2RxClkInvertSel) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_CLK_SF_IF2_RX_INV_SEL);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_CLK_SF_IF2_RX_INV_SEL);
    }

    if (bank2Cfg->bank2ClkDelay > 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF2_READ_DLY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF2_READ_DLY_N, bank2Cfg->bank2ClkDelay - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_READ_DLY_EN);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_0_OFFSET);

    /* Dual flash mode, enable bank2, select pad1 and pad2 */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_BK2_EN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_BK2_MODE);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_PAD_SEL, 0);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_IO_Delay(uint8_t pad, uint8_t doDelay, uint8_t diDelay, uint8_t oeDelay)
{
    uint32_t tmpVal = 0;
    uint32_t offset = 0;

    /* Check the parameters */

    if (pad == SF_CTRL_PAD1) {
        offset = SF_CTRL_BASE + SF_CTRL_IF_IO_DLY_1_OFFSET;
    } else if (pad == SF_CTRL_PAD2) {
        offset = SF_CTRL_BASE + SF_CTRL_IF_IO_DLY_2_OFFSET;
    } else {
        offset = SF_CTRL_BASE + SF_CTRL_IF_IO_DLY_3_OFFSET;
    }

    /* Set do di and oe delay */
    tmpVal = readl(offset + SF_CTRL_IO_DLY_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_0_DO_DLY_SEL, doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_0_DI_DLY_SEL, diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_0_OE_DLY_SEL, oeDelay);
    writel(tmpVal, offset + SF_CTRL_IO_DLY_1_OFFSET);

    tmpVal = readl(offset + SF_CTRL_IO_DLY_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_1_DO_DLY_SEL, doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_1_DI_DLY_SEL, diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_1_OE_DLY_SEL, oeDelay);
    writel(tmpVal, offset + SF_CTRL_IO_DLY_2_OFFSET);

    tmpVal = readl(offset + SF_CTRL_IO_DLY_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_2_DO_DLY_SEL, doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_2_DI_DLY_SEL, diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_2_OE_DLY_SEL, oeDelay);
    writel(tmpVal, offset + SF_CTRL_IO_DLY_3_OFFSET);

    tmpVal = readl(offset + SF_CTRL_IO_DLY_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_3_DO_DLY_SEL, doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_3_DI_DLY_SEL, diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IO_3_OE_DLY_SEL, oeDelay);
    writel(tmpVal, offset + SF_CTRL_IO_DLY_4_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Sbus2_Hold_Sram(void)
{
    uint32_t tmpVal;

    /* Sbus2 hold sram */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF2_FN_SEL);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Sbus2_Release_Sram(void)
{
    uint32_t tmpVal;

    /* Sbus2 release sram */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_FN_SEL);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION SF_Ctrl_Is_Sbus2_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);

    if (reg_is_bit_set(tmpVal, SF_CTRL_SF_IF2_EN)) {
        if (reg_is_bit_set(tmpVal, SF_CTRL_SF_IF2_FN_SEL)) {
            return SET;
        } else {
            return RESET;
        }
    }

    return RESET;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Sbus2_Replace(uint8_t pad)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */

    /* Sbus2 enable */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF2_EN);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);

    SF_Ctrl_Sbus2_Hold_Sram();

    /* Sbus2 repalce pad */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF1);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF2);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF3);

    if (pad == SF_CTRL_PAD1) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF1);
    } else if (pad == SF_CTRL_PAD2) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF2);
    } else {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF3);
    }

    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF2_PAD_SEL, pad);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_0_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Sbus2_Revoke_replace(void)
{
    uint32_t tmpVal = 0;

    SF_Ctrl_Sbus2_Release_Sram();

    /* Sbus2 clear repalce pad */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF1);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF2);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_REPLACE_SF3);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_0_OFFSET);

    /* Sbus2 disable */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_EN);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_1_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Sbus2_Set_Delay(uint8_t clkDelay, uint8_t rxClkInvert)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_0_OFFSET);

    if (clkDelay > 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF2_READ_DLY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF2_READ_DLY_N, clkDelay - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF2_READ_DLY_EN);
    }

    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CLK_SF_IF2_RX_INV_SEL, rxClkInvert);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF2_CTRL_0_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_32bits_Addr_En(int en32BitsAddr)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    if (en32BitsAddr) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_32B_ADR_EN);

    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_32B_ADR_EN);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_0_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_TCM_SECTION SF_Ctrl_Get_Clock_Delay(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    if (reg_get_bits(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN) == 0) {
        return 0;
    } else {
        return reg_get_bits(tmpVal, SF_CTRL_SF_IF_READ_DLY_N) + 1;
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_Clock_Delay(uint8_t delay)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    if (delay > 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_READ_DLY_N, delay - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_0_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_TCM_SECTION SF_Ctrl_Get_Wrap_Queue_Value(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_3_OFFSET);

    return reg_get_bits(tmpVal, SF_CTRL_SF_CMDS_2_WRAP_Q);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Cmds_Set(SF_Ctrl_Cmds_Cfg *cmdsCfg, uint8_t bank)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_3_OFFSET);

    if (cmdsCfg->ackLatency) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_1_ACK_LAT);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_1_ACK_LAT);
    }

    if (cmdsCfg->cmdsCoreEn) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_CMDS_CORE_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_CMDS_CORE_EN);
    }

    if (bank == SF_CTRL_FLASH_BANK1) {
        if (cmdsCfg->cmdsEn) {
            tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_CMDS_2_EN);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_CMDS_2_EN);
        }

        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_2_WRAP_MODE, cmdsCfg->cmdsWrapMode);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_2_WRAP_LEN, cmdsCfg->cmdsWrapLen);
    } else {
        if (cmdsCfg->cmdsEn) {
            tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_CMDS_1_EN);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_CMDS_1_EN);
        }

        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_1_WRAP_MODE, cmdsCfg->cmdsWrapMode);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_1_WRAP_LEN, cmdsCfg->cmdsWrapLen);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_3_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Select_Pad(uint8_t sel)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);

    if (sel <= SF_IO_EXT_SF3 || sel == SF_IO_EXT_SF2) {
        /* Single flash mode, disable bank2 */
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_BK2_EN);

        if (sel <= SF_IO_EMB_SWAP_IO2CS) {
            /* Select embedded pad1 */
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_PAD_SEL, 0);
        } else {
            /* Select pad2 or pad3 */
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_PAD_SEL, ((sel&0xf) >> 2));
        }
    } else if ((sel >= SF_IO_EMB_SWAP_IO3IO0_AND_SF2_SWAP_IO3IO0 && sel <= SF_IO_EMB_SWAP_IO2CS_AND_SF2_SWAP_IO3IO0)
             ||(sel >= SF_IO_EMB_SWAP_IO3IO0_AND_SF2 && sel <= SF_IO_EMB_SWAP_IO2CS_AND_SF2)) {
        /* Dual flash mode, enable bank2, select pad1 and pad2 */
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_BK2_EN);
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_BK2_MODE);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_PAD_SEL, 0);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Remap_Set(uint8_t remap, uint8_t lock)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AHB2SIF_REMAP, remap);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);

    if (lock) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AHB2SIF_REMAP_LOCK);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AHB2SIF_REMAP_LOCK);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Sbus_Select_Bank(uint8_t bank)
{
    /* TODO: sf_if_bk_swap */
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);

    if (bank == SF_CTRL_FLASH_BANK0) {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_0_BK_SEL);
    } else {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_0_BK_SEL);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_Owner(uint8_t owner)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    /* Check the parameters */

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    /* Set owner */
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_FN_SEL, owner);

    /* Set iahb to flash interface */
    if (owner == SF_CTRL_OWNER_IAHB) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AHB2SIF_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AHB2SIF_EN);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_1_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_EN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_1_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable_BE(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_KEY_ENDIAN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_IV_ENDIAN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_DIN_ENDIAN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_DOUT_ENDIAN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_0_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable_LE(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_KEY_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_IV_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_DIN_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_DOUT_ENDIAN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_0_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Region(uint8_t region, uint8_t enable,
                                             uint8_t hwKey, uint32_t startAddr, uint32_t endAddr, uint8_t locked)
{
    /* Do flash key eco */
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal;

    if (!hwKey) {
        regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    }

    tmpVal = readl(regionRegBase + SF_CTRL_SF_AES_END_OFFSET);
    /* sf_aes_end =1 means 1,11,1111,1111 */
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_REGION_END, endAddr / 1024);
    writel(tmpVal, regionRegBase + SF_CTRL_SF_AES_END_OFFSET);

    tmpVal = readl(regionRegBase + SF_CTRL_SF_AES_START_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_REGION_HW_KEY_EN, hwKey);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_REGION_START, startAddr / 1024);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_REGION_EN, enable);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_REGION_LOCK, locked);
    writel(tmpVal, regionRegBase + SF_CTRL_SF_AES_START_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Key(uint8_t region, uint8_t *key, uint8_t keyType)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 0;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_MODE, keyType);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (NULL != key) {
        if (keyType == SF_CTRL_AES_128BITS) {
            i = 4;
            /*
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_7_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_6_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_5_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_4_OFFSET);
			key+=4;
			*/
        } else if (keyType == SF_CTRL_AES_256BITS) {
            i = 8;
            /*
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_7_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_6_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_5_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_4_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_3_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_2_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_1_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_0_OFFSET);
			key+=4;
			*/
        } else if (keyType == SF_CTRL_AES_192BITS) {
            i = 6;
            /*
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_7_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_6_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_5_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_4_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_3_OFFSET);
			key+=4;
			writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + SF_CTRL_SF_AES_KEY_2_OFFSET);
			key+=4;
			*/
        }

        tmpVal = SF_CTRL_SF_AES_KEY_7_OFFSET;

        while (i--) {
            writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + tmpVal);
            key += 4;
            tmpVal -= 4;
        }
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_XTS_Set_Key(uint8_t region, uint8_t *key, uint8_t keyType)
{
    /* Do flash key eco */
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 0;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_MODE, keyType);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (NULL != key) {
        i = 8;
        tmpVal = SF_CTRL_SF_AES_KEY_7_OFFSET;

        while (i--) {
            writel(__REV(DRV_WORD_FROM_BYTES(key)), regionRegBase + tmpVal);
            key += 4;
            tmpVal -= 4;
        }
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Key_BE(uint8_t region, uint8_t *key, uint8_t keyType)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 0;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_MODE, keyType);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (NULL != key) {
        if (keyType == SF_CTRL_AES_128BITS) {
            i = 4;
            /*
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_0_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_1_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_2_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_3_OFFSET);
			key+=4;
			*/
        } else if (keyType == SF_CTRL_AES_256BITS) {
            i = 8;
            /*
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_0_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_1_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_2_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_3_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_4_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_5_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_6_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_7_OFFSET);
			key+=4;
			*/
        } else if (keyType == SF_CTRL_AES_192BITS) {
            i = 6;
            /*
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_0_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_1_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_2_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_3_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_4_OFFSET);
			key+=4;
			writel(DRV_WORD_FROM_BYTES(key), regionRegBase + SF_CTRL_SF_AES_KEY_5_OFFSET);
			*/
        }

        tmpVal = SF_CTRL_SF_AES_KEY_0_OFFSET;

        while (i--) {
            writel(DRV_WORD_FROM_BYTES(key), regionRegBase + tmpVal);
            key += 4;
            tmpVal += 4;
        }
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_XTS_Set_Key_BE(uint8_t region, uint8_t *key, uint8_t keyType)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 0;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_MODE, keyType);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (NULL != key) {
        i = 8;
        tmpVal = SF_CTRL_SF_AES_KEY_0_OFFSET;

        while (i--) {
            writel(DRV_WORD_FROM_BYTES(key), regionRegBase + tmpVal);
            key += 4;
            tmpVal += 4;
        }
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 3;

    if (iv != NULL) {
        tmpVal = SF_CTRL_SF_AES_IV_W3_OFFSET;

        while (i--) {
            writel(__REV(DRV_WORD_FROM_BYTES(iv)), regionRegBase + tmpVal);
            iv += 4;
            tmpVal -= 4;
        }

        /*
		writel(__REV(DRV_WORD_FROM_BYTES(iv)), regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);
		iv+=4;
		writel(__REV(DRV_WORD_FROM_BYTES(iv)), regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET);
		iv+=4;
		writel(__REV(DRV_WORD_FROM_BYTES(iv)), regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET);
		iv+=4;
		*/
        writel(addrOffset, regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
        iv += 4;
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_XTS_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 3;

    if (iv != NULL) {
        tmpVal = SF_CTRL_SF_AES_IV_W1_OFFSET;

        while (i--) {
            iv += 4;
            writel((DRV_WORD_FROM_BYTES(iv)), regionRegBase + tmpVal);
            tmpVal += 4;
        }

        writel(addrOffset, regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
        iv += 4;
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 3;

    if (iv != NULL) {
        tmpVal = SF_CTRL_SF_AES_IV_W0_OFFSET;

        while (i--) {
            writel(DRV_WORD_FROM_BYTES(iv), regionRegBase + tmpVal);
            iv += 4;
            tmpVal += 4;
        }

        /*
		writel(DRV_WORD_FROM_BYTES(iv), regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
		iv+=4;
		writel(DRV_WORD_FROM_BYTES(iv), regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET);
		iv+=4;
		writel(DRV_WORD_FROM_BYTES(iv), regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET);
		iv+=4;
		*/
        writel(__REV(addrOffset), regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);
        iv += 4;
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_XTS_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 3;

    if (iv != NULL) {
        tmpVal = SF_CTRL_SF_AES_IV_W2_OFFSET;

        while (i--) {
            iv += 4;
            writel(__REV(DRV_WORD_FROM_BYTES(iv)), regionRegBase + tmpVal);
            tmpVal -= 4;
        }

        writel(__REV(addrOffset), regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);
        iv += 4;
    }
}
#endif
#ifndef IPRO_USE_ROM_DRIVER

__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (mode == SF_CTRL_AES_CTR_MODE) {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_BLK_MODE);
    } else {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_BLK_MODE);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_EN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_EN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION SF_Ctrl_Is_AES_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (reg_is_bit_set(tmpVal, SF_CTRL_SF_AES_EN)) {
        return SET;
    }

    return RESET;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset, uint8_t group, uint8_t bank)
{
    /* Check the parameters */

    if (group) {
        if (bank == SF_CTRL_FLASH_BANK0) {
            writel(addrOffset, SF_CTRL_BASE + SF_CTRL_SF_ID1_OFFSET_OFFSET);
        } else {
            writel(addrOffset, SF_CTRL_BASE + SF_CTRL_SF_BK2_ID1_OFFSET_OFFSET);
        }
    } else {
        if (bank == SF_CTRL_FLASH_BANK0) {
            writel(addrOffset, SF_CTRL_BASE + SF_CTRL_SF_ID0_OFFSET_OFFSET);
        } else {
            writel(addrOffset, SF_CTRL_BASE + SF_CTRL_SF_BK2_ID0_OFFSET_OFFSET);
        }
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
uint32_t ATTR_TCM_SECTION SF_Ctrl_Get_Flash_Image_Offset(uint8_t group, uint8_t bank)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */

    if (group) {
        if (bank == SF_CTRL_FLASH_BANK0) {
            tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_ID1_OFFSET_OFFSET);
        } else {
            tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_BK2_ID1_OFFSET_OFFSET);
        }
    } else {
        if (bank == SF_CTRL_FLASH_BANK0) {
            tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_ID0_OFFSET_OFFSET);
        } else {
            tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_BK2_ID0_OFFSET_OFFSET);
        }
    }

    return tmpVal;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Lock_Flash_Image_Offset(uint8_t lock)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);
    if (lock) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_ID_OFFSET_LOCK);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_ID_OFFSET_LOCK);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;
    uint32_t cmdOffset = 0;

    /* Check the parameters */

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    if (SF_Ctrl_Is_Sbus2_Enable() == RESET) {
        tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);

        if (reg_get_bits(tmpVal, SF_CTRL_SF_IF_FN_SEL) != SF_CTRL_OWNER_SAHB) {
            return;
        }

        cmdOffset = SF_CTRL_BASE + SF_CTRL_IF1_SAHB_OFFSET;
    } else {
        cmdOffset = SF_CTRL_BASE + SF_CTRL_IF2_SAHB_OFFSET;
    }

    /* Clear trigger */
    tmpVal = readl(cmdOffset + SF_CTRL_IF_SAHB_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_IF_0_TRIG);
    writel(tmpVal, cmdOffset + SF_CTRL_IF_SAHB_0_OFFSET);

    /* Copy command buffer */
    writel(cfg->cmdBuf[0], cmdOffset + SF_CTRL_IF_SAHB_1_OFFSET);
    writel(cfg->cmdBuf[1], cmdOffset + SF_CTRL_IF_SAHB_2_OFFSET);

    tmpVal = readl(cmdOffset + SF_CTRL_IF_SAHB_0_OFFSET);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    /* Configure cmd */
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_IF_0_CMD_EN);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_CMD_BYTE, 0);

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_IF_0_ADR_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_IF_0_ADR_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_ADR_BYTE, 0);
    }

    /* Configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_IF_0_DMY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_IF_0_DMY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_IF_0_DAT_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_DAT_BYTE, cfg->nbData - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_IF_0_DAT_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_DAT_BYTE, 0);
    }

    /* Set read write flag */
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_IF_0_DAT_RW, cfg->rwFlag);
    writel(tmpVal, cmdOffset + SF_CTRL_IF_SAHB_0_OFFSET);

    /* Trigger */
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_IF_0_TRIG);
    writel(tmpVal, cmdOffset + SF_CTRL_IF_SAHB_0_OFFSET);

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Disable_Wrap_Access(uint8_t disable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    if (disable) {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AHB2SIF_DISWRAP);
    } else {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AHB2SIF_DISWRAP);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_1_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Flash_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    /* Check the parameters */

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    if (reg_get_bits(tmpVal, SF_CTRL_SF_IF_FN_SEL) != SF_CTRL_OWNER_IAHB) {
        return;
    }

    /* Copy command buffer */
    writel(cfg->cmdBuf[0], SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_1_OFFSET);
    writel(cfg->cmdBuf[1], SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_2_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_0_OFFSET);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    if (cmdValid) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_CMD_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_CMD_BYTE, 0);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_CMD_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_CMD_BYTE, 0);
    }

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_ADR_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_ADR_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_ADR_BYTE, 0);
    }

    /* configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DMY_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DMY_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DAT_EN, 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DAT_EN, 0);
    }

    /* Set read write flag */
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DAT_RW, cfg->rwFlag);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_0_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Flash2_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    /* Check the parameters */

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    if (reg_get_bits(tmpVal, SF_CTRL_SF_IF_FN_SEL) != SF_CTRL_OWNER_IAHB) {
        return;
    }

    /* Copy command buffer */
    writel(cfg->cmdBuf[0], SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_10_OFFSET);
    writel(cfg->cmdBuf[1], SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_11_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_9_OFFSET);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    if (cmdValid) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_CMD_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_CMD_BYTE, 0);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_CMD_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_CMD_BYTE, 0);
    }

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_ADR_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_ADR_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_ADR_BYTE, 0);
    }

    /* configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DMY_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DMY_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DAT_EN, 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DAT_EN, 0);
    }

    /* Set read write flag */
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_1_DAT_RW, cfg->rwFlag);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_9_OFFSET);
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION SF_Ctrl_GetBusyState(void)
{
    uint32_t tmpVal = 0;
    uint32_t cmdOffset = 0;

    if (SF_Ctrl_Is_Sbus2_Enable() == RESET) {
        cmdOffset = SF_CTRL_BASE + SF_CTRL_IF1_SAHB_OFFSET;
    } else {
        cmdOffset = SF_CTRL_BASE + SF_CTRL_IF2_SAHB_OFFSET;
    }

    tmpVal = readl(cmdOffset + SF_CTRL_IF_SAHB_0_OFFSET);

    if (reg_is_bit_set(tmpVal, SF_CTRL_IF_BUSY)) {
        return SET;
    }

    return RESET;
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void SF_Ctrl_IRQHandler(void)
{
    /* TODO: Not implemented */
}
#endif

void ATTR_TCM_SECTION SF_Ctrl_AES_Get_IV_BE(uint8_t region, uint8_t *iv)
{
    uint32_t regionRegBase = SF_CTRL_BASE + SF_CTRL_AES_REGION_OFFSET + region * 0x80;
    uint32_t iv_w[4] = {0};

    if (iv != NULL) {
        iv_w[0] = readl(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
        iv_w[1] = readl(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET);
        iv_w[2] = readl(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET);
        iv_w[3] = readl(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);
        arch_memcpy(iv, (uint8_t *)&iv_w, sizeof(iv_w));
    }
}

void ATTR_TCM_SECTION SF_Ctrl_AES_Get_IV_LE(uint8_t region, uint8_t *iv)
{
    uint32_t regionRegBase = SF_CTRL_BASE + SF_CTRL_AES_REGION_OFFSET + region * 0x80;
    uint32_t iv_w[4] = {0};

    if (iv != NULL) {
        iv_w[3] = __REV(readl(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET));
        iv_w[2] = __REV(readl(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET));
        iv_w[1] = __REV(readl(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET));
        iv_w[0] = __REV(readl(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET));
        arch_memcpy(iv, (uint8_t *)&iv_w, sizeof(iv_w));
    }
}

int32_t ATTR_TCM_SECTION SF_Ctrl_AES_Set_Decrypt_Region_BE(SF_Ctrl_Decrypt_Type *parm)
{
    uint8_t temp_iv[16] = {0};
    uint8_t *p_iv = NULL;
    uint32_t mstatus = 0;
    uint32_t irq_flag = 0;

    /* assert, sf_ctrl support max flash size 64M */
    if((NULL==parm)||((parm->addr+parm->len)>(64*1024*1024))){
        return -1;
    }

    /* lock */
    mstatus = __get_MSTATUS();
    if(0x8&mstatus){
        irq_flag = 1;
        __disable_irq();
    }

    if(parm->len){
        if(NULL==parm->iv){
            /* get iv from region 0 */
            SF_Ctrl_AES_Get_IV_BE(0, temp_iv);
            p_iv = temp_iv;
        }else{
            p_iv = parm->iv;
        }

        /* set decrypt */
        SF_Ctrl_AES_Disable();
        if(SF_CTRL_AES_XTS_MODE == parm->mode){
            /* aes xts mode */
            SF_Ctrl_Disable_Wrap_Access(0);
            SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_XTS_MODE);
            SF_Ctrl_AES_XTS_Set_Key_BE(parm->aes_region, NULL, parm->type);
            SF_Ctrl_AES_XTS_Set_IV_BE(parm->aes_region, p_iv, parm->addr);
        }else{
            /* aes ctr mode */
            SF_Ctrl_Disable_Wrap_Access(1);
            SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_CTR_MODE);
            SF_Ctrl_AES_Set_Key_BE(parm->aes_region, NULL, parm->type);
            SF_Ctrl_AES_Set_IV_BE(parm->aes_region, p_iv, parm->addr);
        }
        SF_Ctrl_AES_Set_Region(parm->aes_region, 1 /*enable this region*/, 1 /*hardware key*/, parm->addr, parm->addr+parm->len-1, 0 /*lock*/);
        SF_Ctrl_AES_Enable_BE();
        SF_Ctrl_AES_Enable();
    }

    /* unlock */
    if(irq_flag){
        __enable_irq();
    }

    return 0;
}

int32_t ATTR_TCM_SECTION SF_Ctrl_AES_Set_Decrypt_Region_LE(SF_Ctrl_Decrypt_Type *parm)
{
    uint8_t temp_iv[16] = {0};
    uint8_t *p_iv = NULL;
    uint32_t mstatus = 0;
    uint32_t irq_flag = 0;

    /* assert, sf_ctrl support max flash size 64M */
    if((NULL==parm)||((parm->addr+parm->len)>(64*1024*1024))){
        return -1;
    }

    /* lock */
    mstatus = __get_MSTATUS();
    if(0x8&mstatus){
        irq_flag = 1;
        __disable_irq();
    }

    if(parm->len){
        if(NULL==parm->iv){
            /* get iv from region 0 */
            SF_Ctrl_AES_Get_IV_LE(0, temp_iv);
            p_iv = temp_iv;
        }else{
            p_iv = parm->iv;
        }

        /* set decrypt */
        SF_Ctrl_AES_Disable();
        if(SF_CTRL_AES_XTS_MODE == parm->mode){
            /* aes xts mode */
            SF_Ctrl_Disable_Wrap_Access(0);
            SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_XTS_MODE);
            SF_Ctrl_AES_XTS_Set_Key(parm->aes_region, NULL, parm->type);
            SF_Ctrl_AES_XTS_Set_IV(parm->aes_region, p_iv, parm->addr);
        }else{
            /* aes ctr mode */
            SF_Ctrl_Disable_Wrap_Access(1);
            SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_CTR_MODE);
            SF_Ctrl_AES_Set_Key(parm->aes_region, NULL, parm->type);
            SF_Ctrl_AES_Set_IV(parm->aes_region, p_iv, parm->addr);
        }
        SF_Ctrl_AES_Set_Region(parm->aes_region, 1 /*enable this region*/, 1 /*hardware key*/, parm->addr, parm->addr+parm->len-1, 0 /*lock*/);
        SF_Ctrl_AES_Enable_LE();
        SF_Ctrl_AES_Enable();
    }

    /* unlock */
    if(irq_flag){
        __enable_irq();
    }

    return 0;
}

