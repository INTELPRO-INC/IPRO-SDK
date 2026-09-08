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


#include "core_compatiable.h"
#include "drv_sf_ctrl.h"

#define SF_CTRL_BUSY_STATE_TIMEOUT      (5 * 160 * 1000)
#define SF_Ctrl_Get_AES_Region(addr, r) (addr + SF_CTRL_AES_REGION_OFFSET + (r)*0x80)

/****************************************************************************/ /**
 * @brief  Enable serail flash controller
 *
 * @param  cfg: serial flash controller config
 *
 * @return None
 *
*******************************************************************************/
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

#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Enable(const SF_Ctrl_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    if (cfg == NULL) {
        return;
    }

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

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

    /* Set do di and oe delay */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IO_DLY_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_0_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_0_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_0_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IO_DLY_1_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IO_DLY_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_1_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_1_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_1_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IO_DLY_2_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IO_DLY_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_2_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_2_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_2_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IO_DLY_3_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IO_DLY_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_3_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_3_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IO_3_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IO_DLY_4_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF2_IF_IO_DLY_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_0_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_0_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_0_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF2_IF_IO_DLY_1_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF2_IF_IO_DLY_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_1_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_1_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_1_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF2_IF_IO_DLY_2_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF2_IF_IO_DLY_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_2_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_2_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_2_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF2_IF_IO_DLY_3_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF2_IF_IO_DLY_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_3_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_3_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IO_3_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF2_IF_IO_DLY_4_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF3_IF_IO_DLY_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_0_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_0_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_0_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF3_IF_IO_DLY_1_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF3_IF_IO_DLY_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_1_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_1_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_1_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF3_IF_IO_DLY_2_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF3_IF_IO_DLY_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_2_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_2_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_2_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF3_IF_IO_DLY_3_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF3_IF_IO_DLY_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_3_DO_DLY_SEL, cfg->doDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_3_DI_DLY_SEL, cfg->diDelay);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF3_IO_3_OE_DLY_SEL, cfg->oeDelay);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF3_IF_IO_DLY_4_OFFSET);

    /* Enable AHB access sram buffer and enable sf interface */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AHB2SRAM_EN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_EN);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    SF_Ctrl_Set_Owner(cfg->owner);
}
#endif

/****************************************************************************/ /**
 * @brief  Enable serail psram controller
 *
 * @param  sfCtrlPsramCfg: serial psram controller config
 *
 * @return None
 *
*******************************************************************************/
//#ifndef DRV_USE_ROM_DRIVER
//__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Psram_Init(SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg)
{
    uint32_t tmpVal = 0;

    SF_Ctrl_Select_Pad(sfCtrlPsramCfg->padSel);
    SF_Ctrl_Select_Bank(sfCtrlPsramCfg->bankSel);

    /* Enable psram dual bank mode */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);
    tmpVal=reg_set_bit(tmpVal,SF_CTRL_SF_IF_BK2_EN);
    tmpVal=reg_set_bit(tmpVal,SF_CTRL_SF_IF_BK2_MODE);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);

    /* Select psram clock delay */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_12_OFFSET);

    if (sfCtrlPsramCfg->psramRxClkInvertSrc) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SRC);

        if (sfCtrlPsramCfg->psramRxClkInvertSel) {
            tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SEL);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SEL);
        }
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SRC);
    }

    if (sfCtrlPsramCfg->psramDelaySrc) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF2_IF_READ_DLY_SRC);

        if (sfCtrlPsramCfg->psramClkDelay > 0) {
            tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF2_IF_READ_DLY_EN);
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF2_IF_READ_DLY_N, sfCtrlPsramCfg->psramClkDelay - 1);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF2_IF_READ_DLY_EN);
        }
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF2_IF_READ_DLY_SRC);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_12_OFFSET);

    /* Enable AHB access sram buffer and enable sf interface */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AHB2SRAM_EN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_EN);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    SF_Ctrl_Set_Owner(sfCtrlPsramCfg->owner);
}
//#endif

/****************************************************************************/ /**
 * @brief  Get flash controller clock delay value
 *
 * @param  None
 *
 * @return Clock delay value
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
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

/****************************************************************************/ /**
 * @brief  Set flash controller clock delay value
 *
 * @param  delay: Clock delay value
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
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

/****************************************************************************/ /**
 * @brief  SF Ctrl set cmds config
 *
 * @param  cmdsCfg: SF Ctrl cmds config
 * @param  sel: select set flash or psram cmds
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Cmds_Set(SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Select sel)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_3_OFFSET);

    if (sel == SF_CTRL_SEL_FLASH) {
        if (cmdsCfg->cmdsEn) {
            tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_CMDS_1_EN);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_CMDS_1_EN);
        }
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_1_WRAP_MODE, cmdsCfg->wrapMode);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_1_WRAP_LEN, cmdsCfg->wrapLen);
    } else {
        if (cmdsCfg->cmdsEn) {
            tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_CMDS_2_EN);
        } else {
            tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_CMDS_2_EN);
        }
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_2_WRAP_MODE, cmdsCfg->wrapMode);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_2_WRAP_LEN, cmdsCfg->wrapLen);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_3_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  SF Ctrl burst toggle select
 *
 * @param  burstToggleEn: burst toggle enable or not
 * @param  mode: SPI or QPI mode
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Burst_Toggle_Set(uint8_t burstToggleEn, uint8_t mode)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_CMDS_2_BT_EN, burstToggleEn);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_3_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_6_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_3_QPI_MODE_EN, mode);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_6_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  SF Ctrl pad select
 *
 * @param  sel: pad select type
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Select_Pad(SF_Ctrl_Pad_Select sel)
{
    /* TODO: sf_if_bk_swap */
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);

    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_PAD_SEL, sel);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  SF Ctrl bank select
 *
 * @param  sel: bank select type
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Select_Bank(SF_Ctrl_Select sel)
{
    /* TODO: sf_if_bk_swap */
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_2_OFFSET);

    if (sel == SF_CTRL_SEL_FLASH) {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_0_BK_SEL);
    } else {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_0_BK_SEL);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_2_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Set flash controller owner:I/D AHB or system AHB
 *
 * @param  owner: owner type
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_Owner(SF_Ctrl_Owner_Type owner)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

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

/****************************************************************************/ /**
 * @brief  Disable flash controller
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_EN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_1_OFFSET);
}
#endif

void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Mode(SF_Ctrl_AES_Mode_Type mode)
{
    uint32_t tmpVal;
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (mode == SF_CTRL_AES_CTR_MODE) {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_BLK_MODE);
    } else {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_BLK_MODE);
    }

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
}

/****************************************************************************/ /**
 * @brief  Enable flash controller AES with big indian
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable_BE(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_KEY_ENDIAN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_IV_ENDIAN);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_DOUT_ENDIAN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_0_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Enable flash controller AES with little indian
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable_LE(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_0_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_KEY_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_IV_ENDIAN);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_DOUT_ENDIAN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_0_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES region
 *
 * @param  region: region number
 * @param  enable: enable or not
 * @param  hwKey: hardware key or software key
 * @param  startAddr: region start address
 * @param  endAddr: region end address
 * @param  locked: lock this region or not
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
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

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES key
 *
 * @param  region: region number
 * @param  key: key data pointer
 * @param  keyType: flash controller AES key type:128 bits,192 bits or 256 bits
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Key(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 0;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_MODE, keyType);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (NULL != key) {
        if (keyType == SF_CTRL_AES_128BITS) {
            i = 4;
        } else if (keyType == SF_CTRL_AES_256BITS) {
            i = 8;
        } else if (keyType == SF_CTRL_AES_192BITS) {
            i = 6;
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

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES key with big endian
 *
 * @param  region: region number
 * @param  key: key data pointer
 * @param  keyType: flash controller AES key type:128 bits,192 bits or 256 bits
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Key_BE(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 0;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_AES_MODE, keyType);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (NULL != key) {
        if (keyType == SF_CTRL_AES_128BITS) {
            i = 4;
        } else if (keyType == SF_CTRL_AES_256BITS) {
            i = 8;
        } else if (keyType == SF_CTRL_AES_192BITS) {
            i = 6;
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

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES iv
 *
 * @param  region: region number
 * @param  iv: iv data pointer
 * @param  addrOffset: flash address offset
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
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

        writel(addrOffset, regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
        iv += 4;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES iv with big endian
 *
 * @param  region: region number
 * @param  iv: iv data pointer
 * @param  addrOffset: flash address offset
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
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

        writel(__REV(addrOffset), regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);
        iv += 4;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Enable serial flash controller AES
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_AES_EN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Disable serial flash controller AES
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_AES_EN);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Check is serial flash controller AES enable
 *
 * @param  None
 *
 * @return Wether AES is enable
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_TCM_SECTION SF_Ctrl_Is_AES_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    return reg_is_bit_set(tmpVal, SF_CTRL_SF_AES_EN);
}
#endif

/****************************************************************************/ /**
 * @brief  Set flash image offset
 *
 * @param  addrOffset: Address offset value
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset, uint8_t group, uint8_t bank)
{
    writel(addrOffset, SF_CTRL_BASE + SF_CTRL_SF_ID0_OFFSET_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Get flash image offset
 *
 * @param  None
 *
 * @return :Address offset value
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
uint32_t ATTR_TCM_SECTION SF_Ctrl_Get_Flash_Image_Offset(uint8_t group, uint8_t bank)
{
    return readl(SF_CTRL_BASE + SF_CTRL_SF_ID0_OFFSET_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  SF controller send one command
 *
 * @param  cfg: Serial flash controller command configuration pointer
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_1_OFFSET);

    if (reg_get_bits(tmpVal, SF_CTRL_SF_IF_FN_SEL) != SF_CTRL_OWNER_SAHB) {
        return;
    }

    /* Clear trigger */
    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_SAHB_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_0_TRIG);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_SAHB_0_OFFSET);

    /* Copy command buffer */
    writel(cfg->cmdBuf[0], SF_CTRL_BASE + SF_CTRL_SF_IF_SAHB_1_OFFSET);
    writel(cfg->cmdBuf[1], SF_CTRL_BASE + SF_CTRL_SF_IF_SAHB_2_OFFSET);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    /* Configure cmd */
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_0_CMD_EN);
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_CMD_BYTE, 0);

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_0_ADR_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_0_ADR_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_ADR_BYTE, 0);
    }

    /* Configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_0_DMY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_0_DMY_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_0_DAT_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_DAT_BYTE, cfg->nbData - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, SF_CTRL_SF_IF_0_DAT_EN);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_DAT_BYTE, 0);
    }

    /* Set read write flag */
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_0_DAT_RW, cfg->rwFlag);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_SAHB_0_OFFSET);

    /* Trigger */
    tmpVal = reg_set_bit(tmpVal, SF_CTRL_SF_IF_0_TRIG);
    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_SAHB_0_OFFSET);

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Config SF controller for flash I/D cache read
 *
 * @param  cfg: Serial flash controller command configuration pointer
 * @param  cmdValid: command valid or not, for continous read, cache may need no command
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Flash_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

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

/****************************************************************************/ /**
 * @brief  Config psram controller for psram I/D cache write
 *
 * @param  cfg: Serial flash controller command configuration pointer
 * @param  cmdValid: command valid or not, cache may need no command
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Psram_Write_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

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
    writel(cfg->cmdBuf[0], SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_4_OFFSET);
    writel(cfg->cmdBuf[1], SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_5_OFFSET);

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_3_OFFSET);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    if (cmdValid) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_CMD_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_CMD_BYTE, 0);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_CMD_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_CMD_BYTE, 0);
    }

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_ADR_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_ADR_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_ADR_BYTE, 0);
    }

    /* configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_DMY_EN, 1);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_DMY_EN, 0);
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_DAT_EN, 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_DAT_EN, 0);
    }

    /* Set read write flag */
    tmpVal = reg_set_bits(tmpVal, SF_CTRL_SF_IF_2_DAT_RW, cfg->rwFlag);

    writel(tmpVal, SF_CTRL_BASE + SF_CTRL_SF_IF_IAHB_3_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Config psram controller for psram I/D cache read
 *
 * @param  cfg: Serial flash controller command configuration pointer
 * @param  cmdValid: command valid or not, for continous read, cache may need no command
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Psram_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

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

/****************************************************************************/ /**
 * @brief  Get SF Ctrl busy state
 *
 * @param  None
 *
 * @return SET  for SF ctrl busy or RESET for SF ctrl not busy
 *
*******************************************************************************/
#ifndef DRV_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION SF_Ctrl_GetBusyState(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SF_CTRL_BASE + SF_CTRL_SF_IF_SAHB_0_OFFSET);

    if (reg_is_bit_set(tmpVal, SF_CTRL_SF_IF_BUSY)) {
        return SET;
    } else {
        return RESET;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  SF Controller interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef DRV_USE_HAL_DRIVER
void SF_Ctrl_IRQHandler(void)
{
    /* TODO: Not implemented */
}
#endif

/*@} end of group SF_CTRL_Public_Functions */

/*@} end of group SF_CTRL */

/*@} end of group IPRO7_Peripheral_Driver */
