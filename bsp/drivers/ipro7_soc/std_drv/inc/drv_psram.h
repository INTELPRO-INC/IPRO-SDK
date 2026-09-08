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

#ifndef __DRV_PSRAM_H__
#define __DRV_PSRAM_H__

#include "sf_ctrl_reg.h"
#include "drv_common.h"
#include "drv_sf_ctrl.h"

typedef enum {
    PSRAM_DRIVE_STRENGTH_50_OHMS,  /*!< Drive strength 50 ohms(default) */
    PSRAM_DRIVE_STRENGTH_100_OHMS, /*!< Drive strength 100 ohms */
    PSRAM_DRIVE_STRENGTH_200_OHMS, /*!< Drive strength 200 ohms */
} PSRAM_Drive_Strength;

typedef enum {
    PSRAM_BURST_LENGTH_16_BYTES,  /*!< Burst Length 16 bytes */
    PSRAM_BURST_LENGTH_32_BYTES,  /*!< Burst Length 32 bytes */
    PSRAM_BURST_LENGTH_64_BYTES,  /*!< Burst Length 64 bytes */
    PSRAM_BURST_LENGTH_512_BYTES, /*!< Burst Length 512 bytes(default) */
} PSRAM_Burst_Length;

typedef enum {
    PSRAM_SPI_CTRL_MODE, /*!< Psram SPI ctrl mode */
    PSRAM_QPI_CTRL_MODE, /*!< Psram QPI ctrl mode */
} PSRAM_Ctrl_Mode;

typedef struct
{
    uint8_t readIdCmd;                  /*!< Read ID command */
    uint8_t readIdDmyClk;               /*!< Read ID command dummy clock */
    uint8_t burstToggleCmd;             /*!< Burst toggle length command */
    uint8_t resetEnableCmd;             /*!< Psram reset enable command */
    uint8_t resetCmd;                   /*!< Psram reset command */
    uint8_t enterQuadModeCmd;           /*!< Psram enter quad mode command */
    uint8_t exitQuadModeCmd;            /*!< Psram exit quad mode command */
    uint8_t readRegCmd;                 /*!< Read register command */
    uint8_t readRegDmyClk;              /*!< Read register command dummy clock */
    uint8_t writeRegCmd;                /*!< Write register command */
    uint8_t readCmd;                    /*!< Psram read command */
    uint8_t readDmyClk;                 /*!< Psram read command dummy clock */
    uint8_t fReadCmd;                   /*!< Psram fast read command */
    uint8_t fReadDmyClk;                /*!< Psram fast read command dummy clock */
    uint8_t fReadQuadCmd;               /*!< Psram fast read quad command */
    uint8_t fReadQuadDmyClk;            /*!< Psram fast read quad command dummy clock */
    uint8_t writeCmd;                   /*!< Psram write command */
    uint8_t quadWriteCmd;               /*!< Psram quad write command */
    uint16_t pageSize;                  /*!< Psram page size */
    PSRAM_Ctrl_Mode ctrlMode;           /*!< Psram ctrl mode */
    PSRAM_Drive_Strength driveStrength; /*!< Psram drive strength */
    PSRAM_Burst_Length burstLength;     /*!< Psram burst length size */
} SPI_Psram_Cfg_Type;

void Psram_Init(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg);
void Psram_ReadReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue);
void Psram_WriteReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue);
int Psram_SetDriveStrength(SPI_Psram_Cfg_Type *psramCfg);
int Psram_SetBurstWrap(SPI_Psram_Cfg_Type *psramCfg);
void Psram_ReadId(SPI_Psram_Cfg_Type *psramCfg, uint8_t *data);
int Psram_EnterQuadMode(SPI_Psram_Cfg_Type *psramCfg);
int Psram_ExitQuadMode(SPI_Psram_Cfg_Type *psramCfg);
int Psram_ToggleBurstLength(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode);
int Psram_SoftwareReset(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode);
int Psram_Set_IDbus_Cfg(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint32_t len);
int Psram_Cache_Write_Set(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, int wtEn,
                                  int wbEn, int waEn);
int Psram_Write(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len);
int Psram_Read(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len);

#endif /* __DRV_PSRAM_H__ */
