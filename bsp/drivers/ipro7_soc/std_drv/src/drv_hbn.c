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

#include "drv_hbn.h"
#include "compiler/common.h"
#include "drv_glb.h"
#include "drv_xip_sflash.h"
#include "drv_ef_cfg.h"
//#include "risc-v/Core/Include/clic.h"

#define HBN_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }

void ATTR_TCM_SECTION HBN_Mode_Enter(HBN_APP_CFG_Type *cfg)
{
    if (cfg->useXtal32k) {
        HBN_32K_Sel(HBN_32K_XTAL);
    } else {
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
    }
    if ((cfg->hbnLevel) >= HBN_LEVEL_2) {
        HBN_Power_Off_RC32K();
    } else {
        HBN_Power_On_RC32K();
    }
    HBN_Hw_Pu_Pd_Cfg(cfg->hw_pu_pd_en);   
    GLB_Set_System_CLK(GLB_DLL_XTAL_RC32M, GLB_SYS_CLK_RC32M);
    //AON_Power_Off_XTAL();
    HBN_Enable(cfg->ldoLevel, cfg->hbnLevel);
}

/**
 * @brief Set Wakeup HBN or PDS by GPIOX
 *
 * @param gpio_wakeup_src PIN Wakeup PDS or HBN
 * @param gpio_trig_type HBN GPIO interrupt trigger type
 */
void ATTR_TCM_SECTION HBN_GPIO_Wakeup_Set(uint16_t gpio_wakeup_src, HBN_GPIO_INT_Trigger_Type gpio_trig_type)
{
    HBN_Pin_WakeUp_Mask(~(gpio_wakeup_src));

    if (gpio_wakeup_src != 0) {
        HBN_Aon_Pad_IeSmt_Cfg((uint8_t)(gpio_wakeup_src & 0x7f));
        HBN_Comm_Pad_Wakeup_En_Cfg((uint8_t)((gpio_wakeup_src & 0x380) >> 7));
        HBN_GPIO_INT_Enable(gpio_trig_type);
    } else {
        HBN_Aon_Pad_IeSmt_Cfg(0);
        HBN_Comm_Pad_Wakeup_En_Cfg(0);
    }
}

void ATTR_TCM_SECTION HBN_Power_Down_Flash(SPI_Flash_Cfg_Type *flashCfg)
{
    SPI_Flash_Cfg_Type bhFlashCfg;

    if (flashCfg == NULL) {
        /* fix this some time */
        /* SFlash_Cache_Flush(); */
        XIP_SFlash_Read_Via_Cache_Need_Lock(DRV_FLASH_XIP_BASE + 8 + 4, (uint8_t *)(&bhFlashCfg), sizeof(SPI_Flash_Cfg_Type));
        /* fix this some time */
        /* SFlash_Cache_Flush(); */

        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        SFlash_Reset_Continue_Read(&bhFlashCfg);
    } else {
        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        SFlash_Reset_Continue_Read(flashCfg);
    }

    SFlash_Powerdown();
}

/****************************************************************************/ /**
 * @brief  Enable HBN mode
 *
 * @param  aGPIOIeCfg: AON GPIO IE config,Bit0->GPIO18. Bit(s) of Wakeup GPIO(s) must not be set to
 *                     0(s),say when use GPIO7 as wake up pin,aGPIOIeCfg should be 0x01.
 * @param  ldoLevel: LDO volatge level
 * @param  hbnLevel: HBN work level
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Enable(HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel)
{
    uint32_t tmpVal;


    /* HBN mode LDO level */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO11_AON_VOUT_SEL, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* Select RC32M */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, 0);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Set HBN flag */
    writel(HBN_STATUS_ENTER_FLAG, HBN_BASE + HBN_RSV0_OFFSET);

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);

    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbnLevel) {
        case HBN_LEVEL_0:
            tmpVal = reg_clr_bit(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = reg_clr_bit(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_1:
            tmpVal = reg_set_bit(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = reg_clr_bit(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_2:
            tmpVal = reg_set_bit(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = reg_set_bit(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_3:
            tmpVal = reg_set_bit(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = reg_set_bit(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        default:
            break;
    }

    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = reg_clr_bit(tmpVal, HBN_PWR_ON_OPTION);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    CPU_Interrupt_Disable(HBN_OUT0_IRQn);
    CPU_Interrupt_Disable(HBN_OUT1_IRQn);

    writel(0xffffffff, HBN_BASE + HBN_IRQ_CLR_OFFSET);
    writel(0, HBN_BASE + HBN_IRQ_CLR_OFFSET);

    /* Enable HBN mode */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_MODE);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    while (1) {
        arch_delay_ms(1000);
    }
}

int ATTR_TCM_SECTION HBN_Reset(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Reset HBN mode */
    tmpVal = reg_clr_bit(tmpVal, HBN_SW_RST);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    tmpVal = reg_set_bit(tmpVal, HBN_SW_RST);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, HBN_SW_RST);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int HBN_App_Reset(uint8_t npXtalType, uint8_t bclkDiv, uint8_t apXtalType, uint8_t fclkDiv)
{
    uint32_t tmp[12];

    tmp[0] = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmp[1] = readl(HBN_BASE + HBN_TIME_L_OFFSET);
    tmp[2] = readl(HBN_BASE + HBN_TIME_H_OFFSET);
    tmp[3] = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmp[4] = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
    tmp[5] = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmp[6] = readl(HBN_BASE + HBN_PIR_VTH_OFFSET);
    tmp[7] = readl(HBN_BASE + HBN_PIR_INTERVAL_OFFSET);
    tmp[8] = readl(HBN_BASE + HBN_SRAM_OFFSET);
    tmp[9] = readl(HBN_BASE + HBN_RSV0_OFFSET);
    tmp[10] = readl(HBN_BASE + HBN_RSV1_OFFSET);
    tmp[11] = readl(HBN_BASE + HBN_RSV2_OFFSET);
    /* DO HBN reset */
    HBN_Reset();
    /* HBN need 3 32k cyclce to recovery */
    arch_delay_us(100);
    /* Recover HBN value */
    writel(tmp[1], HBN_BASE + HBN_TIME_L_OFFSET);
    writel(tmp[2], HBN_BASE + HBN_TIME_H_OFFSET);
    writel(tmp[0], HBN_BASE + HBN_CTL_OFFSET);

    writel(tmp[3], HBN_BASE + HBN_IRQ_MODE_OFFSET);
    writel(tmp[4], HBN_BASE + HBN_IRQ_CLR_OFFSET);
    writel(tmp[5], HBN_BASE + HBN_PIR_CFG_OFFSET);
    writel(tmp[6], HBN_BASE + HBN_PIR_VTH_OFFSET);
    writel(tmp[7], HBN_BASE + HBN_PIR_INTERVAL_OFFSET);
    writel(tmp[8], HBN_BASE + HBN_SRAM_OFFSET);
    writel(tmp[9], HBN_BASE + HBN_RSV0_OFFSET);
    writel(tmp[10], HBN_BASE + HBN_RSV1_OFFSET);
    writel(tmp[11], HBN_BASE + HBN_RSV2_OFFSET);

    return SUCCESS;
}

int HBN_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Disable HBN mode */
    tmpVal = reg_clr_bit(tmpVal, HBN_MODE);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}


int HBN_Get_BOR_OUT_State(void)
{
    return reg_get_bits(readl(HBN_BASE + HBN_MISC_OFFSET), HBN_R_BOD_OUT) ? SET : RESET;
}

int HBN_Set_BOR_Config(uint8_t enable, HBN_BOR_THRES_Type threshold, HBN_BOR_MODE_Type mode)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_MISC_OFFSET);

    if (enable) {
        tmpVal = reg_set_bits(tmpVal, HBN_PU_BOD, 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, HBN_PU_BOD, 0);
    }

    tmpVal = reg_set_bits(tmpVal, HBN_BOD_VTH, threshold);
    tmpVal = reg_set_bits(tmpVal, HBN_BOD_SEL, mode);
    writel(tmpVal, HBN_BASE + HBN_MISC_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Ldo11_Aon_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11_AON_VOUT_SEL, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Ldo11_Soc_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11SOC_VOUT_SEL_AON, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11_AON_VOUT_SEL, ldoLevel);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11SOC_VOUT_SEL_AON, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_DIS_PWROFF_Ldo11RT_Vout(int onoff)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_DIS_PWR_OFF_LDO11_RT, onoff);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_32K_Sel(HBN_32K_CLK_Type clkType)
{
    uint32_t tmpVal;

    HBN_Trim_RC32K();

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_F32K_SEL, clkType);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int HBN_Set_UART_CLK_Sel(HBN_UART_CLK_Type clkSel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    if (clkSel == HBN_UART_CLK_XCLK) {
        tmpVal = reg_set_bit(tmpVal, HBN_UART_CLK_SEL2);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_UART_CLK_SEL2);
        tmpVal = reg_set_bits(tmpVal, HBN_UART_CLK_SEL, clkSel);
    }
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_Type xClk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, HBN_ROOT_CLK_SEL);

    switch (xClk) {
        case HBN_XCLK_CLK_RC32M:
            tmpVal2 &= (~(1 << 0));
            break;

        case HBN_XCLK_CLK_XTAL:
            tmpVal2 |= (1 << 0);
            break;

        default:
            break;
    }

    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Set_ROOT_CLK(HBN_ROOT_CLK_Type rootClk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, HBN_ROOT_CLK_SEL);

    switch (rootClk) {
        case HBN_ROOT_CLK_RC32M:
            tmpVal2 = 0x0;
            break;

        case HBN_ROOT_CLK_XTAL:
            tmpVal2 = 0x1;
            break;

        case HBN_ROOT_CLK_DLL:
            tmpVal2 |= (1 << 1);
            break;

        default:
            break;
    }

    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

uint32_t ATTR_CLOCK_SECTION HBN_Get_ROOT_CLK(void)
{
    return reg_get_bits(readl(HBN_BASE + HBN_GLB_OFFSET), HBN_ROOT_CLK_SEL);
}


int HBN_Set_HRAM_slp(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_SRAM_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_RETRAM_PGEN);
    tmpVal = reg_clr_bit(tmpVal, HBN_RETRAM_RET2);
    writel(tmpVal, HBN_BASE + HBN_SRAM_OFFSET);

    return SUCCESS;
}

int HBN_Set_HRAM_Ret(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_SRAM_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_RETRAM_PGEN);
    tmpVal = reg_set_bit(tmpVal, HBN_RETRAM_RET2);
    writel(tmpVal, HBN_BASE + HBN_SRAM_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Power_On_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    /* Match HW vendor's IPRO7_CCI_RC32K_Cal_hbn_f32k_sel_RC32K_260430.py:
     *   ac_cap_short=1     (NEW [260430])
     *   capbank=0x20       (load cap)
     *   reg=3              (NEW; was 1, now stronger drive — half stable time)
     *   amp_ctrl=3         (NEW [260406])
     *   inv_stre=3         (reduce stable time to 0.6 s)
     * Settle 500 ms (vendor's latest value).
     */
    tmpVal = readl(HBN_BASE + HBN_XTAL32K_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_XTAL32K_HIZ_EN);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_XTAL32K);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_XTAL32K_BUF);
    tmpVal = reg_set_bit(tmpVal, HBN_XTAL32K_AC_CAP_SHORT);
    tmpVal = reg_set_bits(tmpVal, HBN_XTAL32K_INV_STRE, 3);
    tmpVal = reg_set_bits(tmpVal, HBN_XTAL32K_CAPBANK,  0x20);
    tmpVal = reg_set_bits(tmpVal, HBN_XTAL32K_REG,      3);
    tmpVal = reg_set_bits(tmpVal, HBN_XTAL32K_AMP_CTRL, 3);
    writel(tmpVal, HBN_BASE + HBN_XTAL32K_OFFSET);

    arch_delay_ms(500);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Power_Off_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_XTAL32K_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_XTAL32K);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_XTAL32K_BUF);
    writel(tmpVal, HBN_BASE + HBN_XTAL32K_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Power_On_RC32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_RC32K);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    /* Delay >800us */
    arch_delay_us(880);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Power_Off_RC32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_RC32K);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

HBN_32K_CLK_Type ATTR_CLOCK_SECTION HBN_Get_32K_Sel(void)
{
    return reg_get_bits(readl(HBN_BASE + HBN_GLB_OFFSET), HBN_F32K_SEL);
}

int ATTR_CLOCK_SECTION HBN_Trim_RC32K(void)
{
    Efuse_Common_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Common_Trim("rc32k", &trim, 1);

    if (trim.en) {
        if (trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            tmpVal = readl(HBN_BASE + HBN_RC32K_CTRL0_OFFSET);
            tmpVal = reg_set_bits(tmpVal, HBN_RC32K_CAP_SEL, trim.value);
            writel(tmpVal, HBN_BASE + HBN_RC32K_CTRL0_OFFSET);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_CLOCK_SECTION HBN_Set_RC32K_CapSel(int capsel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RC32K_CTRL0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_RC32K_CAP_SEL, capsel);
    writel(tmpVal, HBN_BASE + HBN_RC32K_CTRL0_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Set_RC32K_CalEnable(int enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RC32K_CTRL0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, HBN_RC32K_CAL_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_RC32K_CAL_EN);
    }
    writel(tmpVal, HBN_BASE + HBN_RC32K_CTRL0_OFFSET);

    return SUCCESS;
}

uint32_t HBN_Get_Status_Flag(void)
{
    return readl(HBN_BASE + HBN_RSV0_OFFSET);
}

int HBN_Set_Status_Flag(uint32_t flag)
{
    writel(flag, HBN_BASE + HBN_RSV0_OFFSET);

    return SUCCESS;
}

uint32_t HBN_Get_Wakeup_Addr(void)
{
    return readl(HBN_BASE + HBN_RSV1_OFFSET);
}

int HBN_Set_Wakeup_Addr(uint32_t addr)
{
    writel(addr, HBN_BASE + HBN_RSV1_OFFSET);

    return SUCCESS;
}

uint8_t HBN_Get_User_Boot_Config(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV2_OFFSET);
    if (HBN_RELEASE_CORE_FLAG == reg_get_bits(tmpVal, HBN_RELEASE_CORE)) {
        return reg_get_bits(tmpVal, HBN_USER_BOOT_SEL);
    }

    return 0x00;
}

int HBN_Set_User_Boot_Config(uint8_t ubCfg)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_RELEASE_CORE, HBN_RELEASE_CORE_FLAG);
    tmpVal = reg_set_bits(tmpVal, HBN_USER_BOOT_SEL, ubCfg);
    writel(tmpVal, HBN_BASE + HBN_RSV2_OFFSET);

    return SUCCESS;
}

/*
 * AON Watchdog Timer APIs
 *
 * Key 0x7A must be written in bits[31:24] of HBN_WDT in the same write
 * as other field values, otherwise HW ignores the write.
 */
#define AON_WDT_KEY  (0x7AUL << HBN_CR_AON_WDT_KEY_POS)

static void hbn_aon_wdt_write(uint32_t val)
{
    writel((val & HBN_CR_AON_WDT_KEY_UMSK) | AON_WDT_KEY, HBN_BASE + HBN_WDT_OFFSET);
}

int HBN_AON_WDT_Init(uint8_t mask_bit)
{
    uint32_t tmpVal;

    /* Disable */
    HBN_AON_WDT_Disable();

    /* Clear status */
    tmpVal = readl(HBN_BASE + HBN_WDT_OFFSET);
    tmpVal |= HBN_CR_AON_WDT_CLR_MSK;
    hbn_aon_wdt_write(tmpVal);

    /* Set timeout + enable */
    tmpVal = readl(HBN_BASE + HBN_WDT_OFFSET);
    tmpVal &= HBN_CR_CNT_MASK_BIT_UMSK;
    tmpVal |= ((uint32_t)(mask_bit & 0x3F)) << HBN_CR_CNT_MASK_BIT_POS;
    tmpVal |= HBN_CR_ANO_WDT_EN_MSK;
    hbn_aon_wdt_write(tmpVal);

    /* Enable watchdog timeout in HBN_GLB */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal |= (1UL << 14);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    /* Initial kick */
    HBN_AON_WDT_Feed();

    return SUCCESS;
}

void HBN_AON_WDT_Feed(void)
{
    uint32_t tmpVal = readl(HBN_BASE + HBN_WDT_OFFSET);
    tmpVal |= HBN_CR_SW_KICK_POS_MSK;
    hbn_aon_wdt_write(tmpVal);
}

int HBN_AON_WDT_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_WDT_OFFSET);
    if (!(tmpVal & HBN_CR_ANO_WDT_EN_MSK))
        return 0;

    tmpVal &= HBN_CR_ANO_WDT_EN_UMSK;
    hbn_aon_wdt_write(tmpVal);

    return 1;
}

int HBN_AON_WDT_Get_Reset_Status(void)
{
    return (readl(HBN_BASE + HBN_WDT_OFFSET) & HBN_ST_AON_WDT_MSK) ? 1 : 0;
}

void HBN_AON_WDT_Clear_Reset_Status(void)
{
    uint32_t tmpVal = readl(HBN_BASE + HBN_WDT_OFFSET);
    tmpVal |= HBN_CR_AON_WDT_CLR_MSK;
    hbn_aon_wdt_write(tmpVal);
}

int HBN_Clear_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Clear RTC control bit0 */
    writel(tmpVal | (1 << 5), HBN_BASE + HBN_CTL_OFFSET);
    arch_delay_us(1);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int HBN_Enable_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    if (tmpVal & 0x01) {
        return SUCCESS;
    }
    /* Set RTC control bit0 */
    writel(tmpVal | 0x01 | (1 << 5), HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int HBN_Set_RTC_Timer(HBN_RTC_INT_Delay_Type delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode)
{
    uint32_t tmpVal;

    writel(compValLow, HBN_BASE + HBN_TIME_L_OFFSET);
    writel(compValHigh & 0x1fff, HBN_BASE + HBN_TIME_H_OFFSET);

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Set interrupt delay option */
    tmpVal = reg_set_bits(tmpVal, HBN_RTC_DLY_OPTION, delay);
    /* Set RTC compare mode */
    tmpVal |= (compMode << 1);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int HBN_Disable_RTC_Compare(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Set RTC compare mode */
    tmpVal &= ~((4 << 1) | (5 << 1) | (7 << 1));
    tmpVal &= ~(0x3 << 1);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

static int HBN_Get_RTC_Timer_Async_Val(uint32_t *valLow, uint32_t *valHigh)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_RTC_TIME_H_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_RTC_TIME_LATCH);
    writel(tmpVal, HBN_BASE + HBN_RTC_TIME_H_OFFSET);

    /* Tigger RTC val read */
    tmpVal = readl(HBN_BASE + HBN_RTC_TIME_H_OFFSET);
    *valHigh = reg_get_bits(tmpVal, HBN_RTC_TIME_LATCH_H);
    *valLow = readl(HBN_BASE + HBN_RTC_TIME_L_OFFSET);

    return SUCCESS;
}

int HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh)
{
    uint32_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
    uint64_t val, val1;
    /* Read-until-stable across the low/high split: two consecutive async
     * reads should settle in one or two passes under normal operation, since
     * the counter only moves forward. Bounded rather than an unconditional
     * do-while: an infinite retry on a monotonic hardware counter with no
     * escape is a hazard on its own — this function is called from dozens of
     * sites across the tree — and on the (should-be-unreachable) bound being
     * hit, falling back to the last read is a rounding error, not a hang.
     *
     * Tested unbounded (2026-08-18) against the hang where hal_lp_enter()
     * never returns on the first connected-state PDS31 entry: identical
     * signature, same ABCDEFGHIJKLMN entry-path checkpoints, same stopping
     * point, current probe still confirmed PDS-level power with no wake. Not
     * the cause; restored the bound since it costs nothing and this call sits
     * in the entry path either way. */
    uint32_t spin = 1000;

    do {
        HBN_Get_RTC_Timer_Async_Val(&tmpValLow, &tmpValHigh);
        val = ((uint64_t)tmpValHigh << 32) | ((uint64_t)tmpValLow);
        HBN_Get_RTC_Timer_Async_Val(&tmpValLow1, &tmpValHigh1);
        val1 = ((uint64_t)tmpValHigh1 << 32) | ((uint64_t)tmpValLow1);
    } while (val1 < val && --spin);

    *valLow = tmpValLow1;
    *valHigh = tmpValHigh1;

    return SUCCESS;
}

int HBN_Clear_RTC_INT(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Clear RTC commpare:bit1-3 for clearing Delayed RTC IRQ */
    writel(tmpVal & 0xfffffff1, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int HBN_GPIO_INT_Enable(HBN_GPIO_INT_Trigger_Type gpioIntTrigType)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MODE, gpioIntTrigType);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_GPIO_INT_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MASK, 0);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Get_INT_State(HBN_INT_Type irqType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(HBN_BASE + HBN_IRQ_STAT_OFFSET);

    if (tmpVal & (1 << irqType)) {
        return SET;
    } else {
        return RESET;
    }
}

uint8_t HBN_Get_Pin_Wakeup_Mode(void)
{
    return reg_get_bits(readl(HBN_BASE + HBN_IRQ_MODE_OFFSET), HBN_PIN_WAKEUP_MODE);
}

int HBN_Clear_IRQ(HBN_INT_Type irqType)
{
    uint32_t tmpVal;

    /* set clear bit */
    tmpVal = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
    tmpVal |= (1 << irqType);
    writel(tmpVal, HBN_BASE + HBN_IRQ_CLR_OFFSET);

    /* unset clear bit */
    tmpVal = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
    tmpVal &= (~(1 << irqType));
    writel(tmpVal, HBN_BASE + HBN_IRQ_CLR_OFFSET);

    return SUCCESS;
}

int HBN_Clear_All_IRQ(void)
{
    /* HBN_IRQ_CLR is a level-sensitive clear bank on IPRO7.  Match
     * HBN_Clear_IRQ(): assert the clear bits, then release them.  Leaving all
     * ones written suppresses a subsequently armed RTC compare and makes
     * repeated PDS entries eventually park in WFI without a wake source. */
    writel(0xFFFFFFFF, HBN_BASE + HBN_IRQ_CLR_OFFSET);
    writel(0x00000000, HBN_BASE + HBN_IRQ_CLR_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Hw_Pu_Pd_Cfg(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);

    if (enable) {
        tmpVal = reg_set_bit(tmpVal, HBN_REG_EN_HW_PU_PD);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_REG_EN_HW_PU_PD);
    }

    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Comm_Pad_Wakeup_En_Cfg(uint8_t padCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_MISC_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_GPIO_WAKEUP_EN_AON, padCfg);
    writel(tmpVal, HBN_BASE + HBN_MISC_OFFSET);

    return SUCCESS;
}

int HBN_Aon_Pad_IeSmt_Cfg(uint8_t padCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_REG_AON_GPIO_IE_SMT, padCfg);
    writel(tmpVal, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Pin_WakeUp_Mask(uint16_t maskVal)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Enable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP0_EN);
    tmpVal2 = tmpVal2 | (1 << edge);
    tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Disable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP0_EN);
    tmpVal2 = tmpVal2 & (~(1 << edge));
    tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Enable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP1_EN);
    tmpVal2 = tmpVal2 | (1 << edge);
    tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Disable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP1_EN);
    tmpVal2 = tmpVal2 & (~(1 << edge));
    tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Enable_BOR_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_IRQ_BOD_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}


int HBN_Disable_BOR_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_IRQ_BOD_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Get_Reset_Event(HBN_RST_EVENT_Type event)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_get_bits(tmpVal, HBN_RESET_EVENT);

    return (tmpVal & (1 << event)) ? SET : RESET;
}

int HBN_Clear_Reset_Event(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_CLEAR_RESET_EVENT);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_CLEAR_RESET_EVENT);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_CLEAR_RESET_EVENT);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_GPIO_Dbg_Pull_Cfg(int pupdEn, int dlyEn, uint8_t dlySec, HBN_INT_Type gpioIrq, int gpioMask)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_DLY_EN, dlyEn);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_DLY_SEL, dlySec);
    tmpVal = reg_set_bits(tmpVal, HBN_REG_EN_HW_PU_PD, pupdEn);

    if (gpioMask != UNMASK) {
        tmpVal = tmpVal | (1 << (gpioIrq + 8));
    } else {
        tmpVal = tmpVal & ~(1 << (gpioIrq + 8));
    }

    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}


int HBN_Set_BOR_Cfg(HBN_BOR_CFG_Type *cfg)
{
    uint32_t tmpVal = 0;

    if (cfg->enableBorInt) {
        HBN_Enable_BOR_IRQ();
    } else {
        HBN_Disable_BOR_IRQ();
    }

    tmpVal = readl(HBN_BASE + HBN_MISC_OFFSET);

    tmpVal = reg_set_bits(tmpVal, HBN_BOD_VTH, cfg->borThreshold);

    if (cfg->enablePorInBor) {
        tmpVal = reg_set_bit(tmpVal, HBN_BOD_SEL);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_BOD_SEL);
    }

    if (cfg->enableBor) {
        tmpVal = reg_set_bit(tmpVal, HBN_PU_BOD);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_PU_BOD);
    }

    writel(tmpVal, HBN_BASE + HBN_MISC_OFFSET);

    return SUCCESS;
}

/**
 * @brief Control VDD14 bond option. When set, the LDO14 output stage is
 *        physically isolated from VDD14 so LDO14 cannot leak while
 *        pu_ldo14_aon=0.
 */
int HBN_Set_VDD14_Bond_Option(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_MISC_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, HBN_VDD14_BOND_OPTION_ENB_AON);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_VDD14_BOND_OPTION_ENB_AON);
    }
    writel(tmpVal, HBN_BASE + HBN_MISC_OFFSET);

    return SUCCESS;
}

int HBN_Power_On_DCDC(void)
{
    uint32_t tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_DCDC_AON);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);
    return SUCCESS;
}

int HBN_Power_Off_DCDC(void)
{
    uint32_t tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_DCDC_AON);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);
    return SUCCESS;
}

int HBN_Power_On_LDO14(void)
{
    uint32_t tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_LDO14_AON);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);
    return SUCCESS;
}

int HBN_Power_Off_LDO14(void)
{
    uint32_t tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_LDO14_AON);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);
    return SUCCESS;
}
