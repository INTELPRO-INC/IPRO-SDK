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


#ifndef __DRV_SFLAH_H__
#define __DRV_SFLAH_H__

#include "drv_common.h"
#include "drv_sf_ctrl.h"

typedef struct
{
    uint8_t ioMode;               /*!< Serail flash interface mode,bit0-3:IF mode,bit4:unwrap,bit5:32-bits addr mode support */
    uint8_t cReadSupport;         /*!< Support continuous read mode,bit0:continuous read mode support,bit1:read mode cfg */
    uint8_t clkDelay;             /*!< SPI clock delay,bit0-3:delay,bit4-6:pad delay */
    uint8_t clkInvert;            /*!< SPI clock phase invert,bit0:clck invert,bit1:rx invert,bit2-4:pad delay,bit5-7:pad delay */
    uint8_t resetEnCmd;           /*!< Flash enable reset command */
    uint8_t resetCmd;             /*!< Flash reset command */
    uint8_t resetCreadCmd;        /*!< Flash reset continuous read command */
    uint8_t resetCreadCmdSize;    /*!< Flash reset continuous read command size */
    uint8_t jedecIdCmd;           /*!< JEDEC ID command */
    uint8_t jedecIdCmdDmyClk;     /*!< JEDEC ID command dummy clock */
    uint8_t enter32BitsAddrCmd;   /*!< Enter 32-bits addr command */
    uint8_t exit32BitsAddrCmd;    /*!< Exit 32-bits addr command */
    uint8_t sectorSize;           /*!< *1024bytes */
    uint8_t mid;                  /*!< Manufacturer ID */
    uint16_t pageSize;            /*!< Page size */
    uint8_t chipEraseCmd;         /*!< Chip erase cmd */
    uint8_t sectorEraseCmd;       /*!< Sector erase command */
    uint8_t blk32EraseCmd;        /*!< Block 32K erase command,some Micron not support */
    uint8_t blk64EraseCmd;        /*!< Block 64K erase command */
    uint8_t writeEnableCmd;       /*!< Need before every erase or program */
    uint8_t pageProgramCmd;       /*!< Page program cmd */
    uint8_t qpageProgramCmd;      /*!< QIO page program cmd */
    uint8_t qppAddrMode;          /*!< QIO page program address mode */
    uint8_t fastReadCmd;          /*!< Fast read command */
    uint8_t frDmyClk;             /*!< Fast read command dummy clock */
    uint8_t qpiFastReadCmd;       /*!< QPI fast read command */
    uint8_t qpiFrDmyClk;          /*!< QPI fast read command dummy clock */
    uint8_t fastReadDoCmd;        /*!< Fast read dual output command */
    uint8_t frDoDmyClk;           /*!< Fast read dual output command dummy clock */
    uint8_t fastReadDioCmd;       /*!< Fast read dual io comamnd */
    uint8_t frDioDmyClk;          /*!< Fast read dual io command dummy clock */
    uint8_t fastReadQoCmd;        /*!< Fast read quad output comamnd */
    uint8_t frQoDmyClk;           /*!< Fast read quad output comamnd dummy clock */
    uint8_t fastReadQioCmd;       /*!< Fast read quad io comamnd */
    uint8_t frQioDmyClk;          /*!< Fast read quad io comamnd dummy clock */
    uint8_t qpiFastReadQioCmd;    /*!< QPI fast read quad io comamnd */
    uint8_t qpiFrQioDmyClk;       /*!< QPI fast read QIO dummy clock */
    uint8_t qpiPageProgramCmd;    /*!< QPI program command */
    uint8_t writeVregEnableCmd;   /*!< Enable write reg */
    uint8_t wrEnableIndex;        /*!< Write enable register index */
    uint8_t qeIndex;              /*!< Quad mode enable register index */
    uint8_t busyIndex;            /*!< Busy status register index */
    uint8_t wrEnableBit;          /*!< Write enable bit pos */
    uint8_t qeBit;                /*!< Quad enable bit pos */
    uint8_t busyBit;              /*!< Busy status bit pos */
    uint8_t wrEnableWriteRegLen;  /*!< Register length of write enable */
    uint8_t wrEnableReadRegLen;   /*!< Register length of write enable status */
    uint8_t qeWriteRegLen;        /*!< Register length of contain quad enable */
    uint8_t qeReadRegLen;         /*!< Register length of contain quad enable status */
    uint8_t releasePowerDown;     /*!< Release power down command */
    uint8_t busyReadRegLen;       /*!< Register length of contain busy status */
    uint8_t readRegCmd[4];        /*!< Read register command buffer */
    uint8_t writeRegCmd[4];       /*!< Write register command buffer */
    uint8_t enterQpi;             /*!< Enter qpi command */
    uint8_t exitQpi;              /*!< Exit qpi command */
    uint8_t cReadMode;            /*!< Config data for continuous read mode */
    uint8_t cRExit;               /*!< Config data for exit continuous read mode */
    uint8_t burstWrapCmd;         /*!< Enable burst wrap command */
    uint8_t burstWrapCmdDmyClk;   /*!< Enable burst wrap command dummy clock */
    uint8_t burstWrapDataMode;    /*!< Data and address mode for this command */
    uint8_t burstWrapData;        /*!< Data to enable burst wrap */
    uint8_t deBurstWrapCmd;       /*!< Disable burst wrap command */
    uint8_t deBurstWrapCmdDmyClk; /*!< Disable burst wrap command dummy clock */
    uint8_t deBurstWrapDataMode;  /*!< Data and address mode for this command */
    uint8_t deBurstWrapData;      /*!< Data to disable burst wrap */
    uint16_t timeEsector;         /*!< 4K erase time */
    uint16_t timeE32k;            /*!< 32K erase time */
    uint16_t timeE64k;            /*!< 64K erase time */
    uint16_t timePagePgm;         /*!< Page program time */
    uint16_t timeCe;              /*!< Chip erase time in ms */
    uint8_t pdDelay;              /*!< Release power down command delay time for wake up */
    uint8_t qeData;               /*!< QE set data */
} __attribute__((packed)) SPI_Flash_Cfg_Type;

typedef struct
{
    uint8_t eraseCmd;       /*!< Erase security register command */
    uint8_t programCmd;     /*!< Program security register command */
    uint8_t readCmd;        /*!< Read security register command */
    uint8_t enterSecOptCmd; /*!< Enter security register option mode command */
    uint8_t exitSecOptCmd;  /*!< Exit security register option mode command */
    uint8_t blockNum;       /*!< Security register block number */
    uint8_t *data;          /*!< Data pointer to be program/read */
    uint32_t addr;          /*!< Start address to be program/read */
    uint32_t len;           /*!< Data length to be program/read */
} SFlash_Sec_Reg_Cfg;

#define IPRO_SPIFLASH_BLK32K_SIZE (32 * 1024)
#define IPRO_SPIFLASH_BLK64K_SIZE (64 * 1024)
#define IPRO_SPIFLASH_CMD_INVALID 0xff

void SFlash_Init(const SF_Ctrl_Cfg_Type *pSfCtrlCfg, const SF_Ctrl_Bank2_Cfg *pBank2Cfg);
int SFlash_SetSPIMode(uint8_t mode);
int SFlash_Read_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen);
int SFlash_Write_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen);
int SFlash_Read_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t readRegCmd, uint8_t *regValue,
                                     uint8_t regLen);
int SFlash_Write_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue,
                                      uint8_t regLen);
int SFlash_Busy(SPI_Flash_Cfg_Type *flashCfg);
int SFlash_Write_Enable(SPI_Flash_Cfg_Type *flashCfg);
int SFlash_Qspi_Enable(SPI_Flash_Cfg_Type *flashCfg);
int SFlash_Qspi_Disable(SPI_Flash_Cfg_Type *flashCfg);
void SFlash_Volatile_Reg_Write_Enable(SPI_Flash_Cfg_Type *flashCfg);
int SFlash_Chip_Erase(SPI_Flash_Cfg_Type *flashCfg);
int SFlash_Sector_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t secNum);
int SFlash_Blk32_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum);
int SFlash_Blk64_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum);
int SFlash_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr);
void SFlash_GetUniqueId(uint8_t *data, uint8_t idLen);
void SFlash_GetJedecId(SPI_Flash_Cfg_Type *flashCfg, uint8_t *data);
void SFlash_GetDeviceId(uint8_t *data, int is32BitsAddr);
void SFlash_Powerdown(void);
void SFlash_Release_Powerdown(SPI_Flash_Cfg_Type *flashCfg);
int SFlash_Restore_From_Powerdown(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t flashContRead,
                                          uint8_t bank);
void SFlash_SetBurstWrap(SPI_Flash_Cfg_Type *flashCfg);
void SFlash_DisableBurstWrap(SPI_Flash_Cfg_Type *flashCfg);
int SFlash_Set32BitsAddrMode(SPI_Flash_Cfg_Type *flashCfg, int en32BitsAddr);
int SFlash_Software_Reset(SPI_Flash_Cfg_Type *flashCfg);
void SFlash_Reset_Continue_Read(SPI_Flash_Cfg_Type *flashCfg);
int SFlash_Set_IDbus_Cfg(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead, uint32_t addr,
                                 uint32_t len, uint8_t bank);
int SFlash_IDbus_Read_Enable(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead,
                                     uint8_t bank);
void SFlash_IDbus_Read_Disable(void);
int SFlash_RCV_Enable(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t rCmd, uint8_t wCmd, uint8_t bitPos);
int SFlash_Erase_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Sec_Reg_Cfg *pSecRegCfg);
int SFlash_Program_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg,
                                             SFlash_Sec_Reg_Cfg *pSecRegCfg);
int SFlash_Read_Security_Register(SFlash_Sec_Reg_Cfg *pSecRegCfg);
int SFlash_Read(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead, uint32_t addr, uint8_t *data,
                        uint32_t len);
int SFlash_Program(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint32_t addr, uint8_t *data, uint32_t len);

int SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *flashCfg);

#endif /* __DRV_SFLAH_H__ */
