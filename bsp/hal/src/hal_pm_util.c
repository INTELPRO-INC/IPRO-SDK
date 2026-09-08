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
#include "drv_glb.h"
#include "hal_clock.h"
#include "hal_pm.h"
#include "hal_pm_util.h"

extern uint8_t cacheWayDisable;
extern uint32_t psramIoCfg;
extern uint32_t flash_offset;

extern void pm_pds31_fast_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time);

extern SPI_Flash_Cfg_Type *flash_cfg;

void ATTR_PDS_RAM_SECTION pm_pds_fastboot_entry(void);

void (*hardware_recovery)(void) = NULL;

uint32_t hal_pds_enter_with_time_compensation(uint32_t pdsLevel, uint32_t pdsSleepCycles)
{
    uint32_t rtcLowBeforeSleep = 0, rtcHighBeforeSleep = 0;
    uint32_t rtcLowAfterSleep = 0, rtcHighAfterSleep = 0;
    uint32_t actualSleepDuration_32768cycles = 0;
    uint32_t actualSleepDuration_ms = 0;

    pm_set_wakeup_callback(pm_pds_fastboot_entry);

    HBN_Get_RTC_Timer_Val(&rtcLowBeforeSleep, &rtcHighBeforeSleep);

    pm_pds31_fast_mode_enter(pdsLevel, pdsSleepCycles);

    HBN_Get_RTC_Timer_Val(&rtcLowAfterSleep, &rtcHighAfterSleep);

    if ((rtcHighAfterSleep - rtcHighBeforeSleep) <= 1) // make sure sleep less than 1 hour (2^32 us > 1 hour)
        return 0;

    actualSleepDuration_32768cycles = (rtcLowAfterSleep - rtcLowBeforeSleep);

    actualSleepDuration_ms = (actualSleepDuration_32768cycles >> 5) - (actualSleepDuration_32768cycles >> 11) - (actualSleepDuration_32768cycles >> 12);

    // vTaskStepTick(actualSleepDuration_ms);

    return actualSleepDuration_ms;
}

static uint8_t ATTR_PDS_RAM_SECTION __spi_flash_get_delay_val(uint8_t delay_index)
{
    switch (delay_index) {
        case 0:
            return 0x00;
        case 1:
            return 0x80;
        case 2:
            return 0xc0;
        case 3:
            return 0xe0;
        case 4:
            return 0xf0;
        case 5:
            return 0xf8;
        case 6:
            return 0xfc;
        case 7:
            return 0xfe;
        default:
            return 0x00;
    }
}

static void ATTR_PDS_RAM_SECTION __spi_flash_set_sf_ctrl(SPI_Flash_Cfg_Type *pFlashCfg)
{
    SF_Ctrl_Cfg_Type sfCtrlCfg;
    uint8_t delay_index;

    sfCtrlCfg.owner = SF_CTRL_OWNER_SAHB;

    /* bit0-3 for clk delay */
    sfCtrlCfg.clkDelay = (pFlashCfg->clkDelay & 0x0f);
    /* bit0 for clk invert */
    sfCtrlCfg.clkInvert = pFlashCfg->clkInvert & 0x01;
    /* bit1 for rx clk invert */
    sfCtrlCfg.rxClkInvert = (pFlashCfg->clkInvert >> 1) & 0x01;
    /* bit4-6 for do delay */
    delay_index = (pFlashCfg->clkDelay >> 4) & 0x07;
    sfCtrlCfg.doDelay = __spi_flash_get_delay_val(delay_index);
    /* bit2-4 for di delay */
    delay_index = (pFlashCfg->clkInvert >> 2) & 0x07;
    sfCtrlCfg.diDelay = __spi_flash_get_delay_val(delay_index);
    /* bit5-7 for oe delay */
    delay_index = (pFlashCfg->clkInvert >> 5) & 0x07;
    sfCtrlCfg.oeDelay = __spi_flash_get_delay_val(delay_index);

    SFlash_Init(&sfCtrlCfg);
}

int32_t ATTR_PDS_RAM_SECTION pm_spi_flash_init(uint8_t media_boot)
{
    uint32_t stat;
    uint32_t jdecId = 0;
    uint32_t flash_read_try = 0;

    /*use fclk as flash clok */
    GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_XCLK, 0); // 32M
    SF_Ctrl_Set_Clock_Delay(0);

    __spi_flash_set_sf_ctrl(flash_cfg);

    /* Wake flash up from power down */
    SFlash_Releae_Powerdown(flash_cfg);
    //ARCH_Delay_US(15*((pFlashCfg->pdDelay&0x7)+1));
    arch_delay_us(120);

    do {
        if (flash_read_try > 4) {
            // __bootrom_printd("Flash read id TO\r\n");
            break;
        } else if (flash_read_try > 0) {
            arch_delay_us(500);
        }

        /* Exit form continous read for accepting command */
        SFlash_Reset_Continue_Read(flash_cfg);
        /* Send software reset command(80bv has no this command)to deburst wrap for ISSI like */
        SFlash_Software_Reset(flash_cfg);
        /* Disable burst may be removed(except for 80BV) and only work with winbond,but just for make sure */
        SFlash_Write_Enable(flash_cfg);
        /* For disable command that is setting register instaed of send command, we need write enable */
        SFlash_DisableBurstWrap(flash_cfg);

        stat = SFlash_SetSPIMode(SF_CTRL_SPI_MODE);
        if (SUCCESS != stat) {
            // __bootrom_printe("enter spi mode fail %d\r\n", stat);
            return -1;
        }

        SFlash_GetJedecId(flash_cfg, (uint8_t *)&jdecId);

        /* Dummy disable burstwrap for make sure */
        SFlash_Write_Enable(flash_cfg);
        /* For disable command that is setting register instead of send command, we need write enable */
        SFlash_DisableBurstWrap(flash_cfg);

        jdecId = jdecId & 0xffffff;
        // __bootrom_printd("ID =%08x\r\n", jdecId);
        flash_read_try++;
    } while ((jdecId & 0x00ffff) == 0 || (jdecId & 0xffff00) == 0 || (jdecId & 0x00ffff) == 0xffff || (jdecId & 0xffff00) == 0xffff00);

    /*clear offset setting*/

    // reset image offset
    SF_Ctrl_Set_Flash_Image_Offset(flash_offset);

    /* set read mode */
    if ((flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        stat = SFlash_Qspi_Enable(flash_cfg);
    }

    if (media_boot) {
        SFlash_Cache_Read_Enable(flash_cfg, flash_cfg->ioMode & 0xf, 0, 0x00);
    }

    return jdecId;
}

void ATTR_PDS_RAM_SECTION sf_io_select(void)
{
    uint8_t flashCfg = 0;
    uint8_t psramCfg = 0;
    uint8_t isInternalFlash = 0;
    uint8_t isInternalPsram = 0;
    Efuse_Device_Info_Type dev_info;

    EF_Ctrl_Read_Device_Info(&dev_info);
    flashCfg = dev_info.flash_cfg;
    psramCfg = dev_info.psram_cfg;

    if (flashCfg == 0) {
        isInternalFlash = 1;
    } else {
        isInternalFlash = 0;
    }
    if (psramCfg == 1) {
        isInternalPsram = 1;
    } else {
        isInternalPsram = 0;
    }

    if (isInternalFlash == 1 && isInternalPsram == 0) {
        GLB_Select_Internal_Flash();
    } else {
        GLB_Select_External_Flash();
    }
}

void pm_set_hardware_recovery_callback(void (*hardware_recovery_cb)(void))
{
    hardware_recovery = hardware_recovery_cb;
}
