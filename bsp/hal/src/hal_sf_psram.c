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

#include "hal_sf_psram.h"
#include "platform_common.h"
#include "drv_psram.h"
#include "drv_l1c.h"
#include "drv_sec_eng.h"
#include "drv_glb.h"

#define EXTFLASH_CS_GPIO    GLB_GPIO_PIN_25
#define EXTPSRAM_CLK_GPIO   GLB_GPIO_PIN_27
#define EXTPSRAM_CS_GPIO    GLB_GPIO_PIN_17
#define EXTPSRAM_DATA0_GPIO GLB_GPIO_PIN_28
#define EXTPSRAM_DATA1_GPIO GLB_GPIO_PIN_24
#define EXTPSRAM_DATA2_GPIO GLB_GPIO_PIN_23
#define EXTPSRAM_DATA3_GPIO GLB_GPIO_PIN_26

SPI_Psram_Cfg_Type apMemory1604 = {
    .readIdCmd = 0x9F,
    .readIdDmyClk = 0,
    .burstToggleCmd = 0xC0,
    .resetEnableCmd = 0x66,
    .resetCmd = 0x99,
    .enterQuadModeCmd = 0x35,
    .exitQuadModeCmd = 0xF5,
    .readRegCmd = 0xB5,
    .readRegDmyClk = 1,
    .writeRegCmd = 0xB1,
    .readCmd = 0x03,
    .readDmyClk = 0,
    .fReadCmd = 0x0B,
    .fReadDmyClk = 1,
    .fReadQuadCmd = 0xEB,
    .fReadQuadDmyClk = 3,
    .writeCmd = 0x02,
    .quadWriteCmd = 0x38,
    .pageSize = 512,
    .ctrlMode = PSRAM_SPI_CTRL_MODE,
    .driveStrength = PSRAM_DRIVE_STRENGTH_50_OHMS,
    .burstLength = PSRAM_BURST_LENGTH_512_BYTES,
};

SF_Ctrl_Cmds_Cfg cmdsCfg = {
    .cmdsEn = ENABLE,
    .wrapMode = DISABLE,
    .wrapLen = SF_CTRL_WRAP_LEN_512,
};
SF_Ctrl_Psram_Cfg sfCtrlPsramCfg = {
    .owner = SF_CTRL_OWNER_SAHB,
    .padSel = SF_CTRL_PAD_SEL_SF2,
    .bankSel = SF_CTRL_SEL_PSRAM,
    .psramRxClkInvertSrc = ENABLE,
    .psramRxClkInvertSel = ENABLE,
    .psramDelaySrc = ENABLE,
    .psramClkDelay = 0,
};

static void ATTR_TCM_SECTION hal_sf_psram_gpio_init(void)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[7];
    uint8_t i = 0;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 3;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_FLASH_PSRAM;

    gpiopins[0] = EXTPSRAM_CLK_GPIO;
    gpiopins[1] = EXTPSRAM_CS_GPIO;
    gpiopins[2] = EXTPSRAM_DATA0_GPIO;
    gpiopins[3] = EXTPSRAM_DATA1_GPIO;
    gpiopins[4] = EXTPSRAM_DATA2_GPIO;
    gpiopins[5] = EXTPSRAM_DATA3_GPIO;
    gpiopins[6] = EXTFLASH_CS_GPIO;

    for (i = 0; i < sizeof(gpiopins); i++) {
        cfg.gpioPin = gpiopins[i];

        if (i == 0 || i == 1 || i == 6) {
            /*flash clk and cs is output*/
            cfg.gpioMode = GPIO_MODE_OUTPUT;
        } else {
            /*data are bidir*/
            cfg.gpioMode = GPIO_MODE_AF;
        }

        GLB_GPIO_Init(&cfg);
    }
}

void ATTR_TCM_SECTION hal_sf_psram_init(uint8_t sw_reset)
{
    uint8_t psramId[8] = { 0 };
    hal_sf_psram_gpio_init();

    Psram_Init(&apMemory1604, &cmdsCfg, &sfCtrlPsramCfg);

    Psram_SoftwareReset(&apMemory1604, apMemory1604.ctrlMode);

    Psram_ReadId(&apMemory1604, psramId);
    SF_Ctrl_Select_Bank(SF_CTRL_SEL_FLASH);
    Psram_Cache_Write_Set(&apMemory1604, SF_CTRL_QIO_MODE, ENABLE, DISABLE, DISABLE);
    //L1C_Cache_Enable_Set(L1C_WAY_DISABLE_NONE);
    printf("psramId %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n", psramId[0], psramId[1], psramId[2], psramId[3], psramId[4], psramId[5], psramId[6], psramId[7]);
}

void ATTR_TCM_SECTION bsp_sf_psram_read_id(uint8_t *data)
{
    Psram_ReadId(&apMemory1604, data);
}
