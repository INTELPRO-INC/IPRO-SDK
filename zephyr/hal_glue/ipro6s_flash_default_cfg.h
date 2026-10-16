/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Generic single/quad SPI NOR configuration (Winbond W25Q80EW/16FW/32JW/
 * 32FW/32FV entry of bsp/drivers/ipro6_soc/std_drv/src/drv_sf_cfg.c), used
 * only to issue the JEDEC-ID read that lets flash_init() pick the real
 * configuration from the ROM table. The IPRO6 ROM build does not compile
 * drv_sf_cfg.c, hence the copy.
 */

#ifndef IPRO6S_FLASH_DEFAULT_CFG_H_
#define IPRO6S_FLASH_DEFAULT_CFG_H_

#include "drv_sf_cfg.h"

static const SPI_Flash_Cfg_Type ipro6s_flash_cfg_default = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 400,
    .timeE32k = 1600,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

#endif /* IPRO6S_FLASH_DEFAULT_CFG_H_ */
