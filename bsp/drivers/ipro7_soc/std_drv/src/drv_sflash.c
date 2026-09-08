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
#include "drv_sflash.h"
#include "drv_sf_ctrl.h"

#define SFCTRL_BUSY_STATE_TIMEOUT (5 * 160 * 1000)

void ATTR_TCM_SECTION SFlash_Init(const SF_Ctrl_Cfg_Type *pSfCtrlCfg)
{
    SF_Ctrl_Enable(pSfCtrlCfg);
}

int ATTR_TCM_SECTION SFlash_SetSPIMode(SF_Ctrl_Mode_Type mode)
{
    int stat = SUCCESS;

    return stat;
}

int ATTR_TCM_SECTION SFlash_Read_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t cnt = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->readRegCmd[regIndex]) << 24;
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
        arch_delay_us(1);
        cnt++;

        if (cnt > 1000) {
            return ERROR;
        }
    }

    arch_memcpy(regValue, flashCtrlBuf, regLen);
    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Write_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    arch_memcpy(flashCtrlBuf, regValue, regLen);

    flashCmd.cmdBuf[0] = (flashCfg->writeRegCmd[regIndex]) << 24;
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    /* take 40ms for tw(write status register) as default */
    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(100);
        cnt++;

        if (cnt > 400) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Read_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t readRegCmd, uint8_t *regValue, uint8_t regLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t cnt = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = readRegCmd << 24;
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
        arch_delay_us(1);
        cnt++;

        if (cnt > 1000) {
            return ERROR;
        }
    }

    arch_memcpy(regValue, flashCtrlBuf, regLen);
    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Write_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue, uint8_t regLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    arch_memcpy(flashCtrlBuf, regValue, regLen);

    flashCmd.cmdBuf[0] = writeRegCmd << 24;
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    /* take 40ms for tw(write status register) as default */
    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(100);
        cnt++;

        if (cnt > 400) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Busy(SPI_Flash_Cfg_Type *flashCfg)
{
    uint32_t stat = 0;
    SFlash_Read_Reg(flashCfg, flashCfg->busyIndex, (uint8_t *)&stat, flashCfg->busyReadRegLen);

    if ((stat & (1 << flashCfg->busyBit)) == 0) {
        return RESET;
    }

    return SET;
}

int ATTR_TCM_SECTION SFlash_Write_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    uint32_t stat = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    /* Write enable*/
    flashCmd.cmdBuf[0] = (flashCfg->writeEnableCmd) << 24;
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    SF_Ctrl_SendCmd(&flashCmd);

    SFlash_Read_Reg(flashCfg, flashCfg->wrEnableIndex, (uint8_t *)&stat, flashCfg->wrEnableReadRegLen);

    if ((stat & (1 << flashCfg->wrEnableBit)) != 0) {
        return SUCCESS;
    }

    return ERROR;
}

int ATTR_TCM_SECTION SFlash_Qspi_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    uint32_t stat = 0, ret;

    if (flashCfg->qeReadRegLen == 0) {
        ret = SFlash_Write_Enable(flashCfg);

        if (SUCCESS != ret) {
            return ERROR;
        }

        SFlash_Write_Reg(flashCfg, flashCfg->qeIndex, (uint8_t *)&stat, flashCfg->qeWriteRegLen);
        return SUCCESS;
    }

    SFlash_Read_Reg(flashCfg, flashCfg->qeIndex, (uint8_t *)&stat, flashCfg->qeReadRegLen);

    if (flashCfg->qeData == 0) {
        if ((stat & (1 << flashCfg->qeBit)) != 0) {
            return SUCCESS;
        }
    } else {
        if (((stat >> (flashCfg->qeBit & 0x08)) & 0xff) == flashCfg->qeData) {
            return SUCCESS;
        }
    }

    if (flashCfg->qeWriteRegLen != 1) {
        /* This is  read r0,read r1 write r0,r1 case*/
        SFlash_Read_Reg(flashCfg, 0, (uint8_t *)&stat, 1);
        SFlash_Read_Reg(flashCfg, 1, ((uint8_t *)&stat) + 1, 1);

        if (flashCfg->qeData == 0) {
            stat |= (1 << (flashCfg->qeBit + 8 * flashCfg->qeIndex));
        } else {
            stat = stat & (~(0xff << (8 * flashCfg->qeIndex)));
            stat |= (flashCfg->qeData << (8 * flashCfg->qeIndex));
        }
    } else {
        if (flashCfg->qeData == 0) {
            stat |= (1 << (flashCfg->qeBit % 8));
        } else {
            stat = flashCfg->qeData;
        }
    }

    ret = SFlash_Write_Enable(flashCfg);

    if (SUCCESS != ret) {
        return ERROR;
    }

    SFlash_Write_Reg(flashCfg, flashCfg->qeIndex, (uint8_t *)&stat, flashCfg->qeWriteRegLen);
    SFlash_Read_Reg(flashCfg, flashCfg->qeIndex, (uint8_t *)&stat, flashCfg->qeReadRegLen);

    if (flashCfg->qeData == 0) {
        if ((stat & (1 << flashCfg->qeBit)) != 0) {
            return SUCCESS;
        }
    } else {
        if (((stat >> (flashCfg->qeBit & 0x08)) & 0xff) == flashCfg->qeData) {
            return SUCCESS;
        }
    }

    return ERROR;
}

void ATTR_TCM_SECTION SFlash_Volatile_Reg_Write_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->writeVregEnableCmd) << 24;
    flashCmd.rwFlag = SF_CTRL_WRITE;

    SF_Ctrl_SendCmd(&flashCmd);
}

int ATTR_TCM_SECTION SFlash_Chip_Erase(SPI_Flash_Cfg_Type *flashCfg)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t cnt = 0;
    int stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->chipEraseCmd) << 24;
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(500);
        cnt++;

        if (cnt > flashCfg->timeCe * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Sector_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t secNum)
{
    uint32_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    int stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->sectorEraseCmd << 24) | (flashCfg->sectorSize * 1024 * secNum);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(500);
        cnt++;

        if (cnt > flashCfg->timeEsector * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Blk32_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum)
{
    uint32_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    int stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->blk32EraseCmd << 24) | (SPIFLASH_BLK32K_SIZE * blkNum);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(500);
        cnt++;

        if (cnt > flashCfg->timeE32k * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Blk64_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t cnt = 0;
    int stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->blk64EraseCmd << 24) | (SPIFLASH_BLK64K_SIZE * blkNum);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(500);
        cnt++;

        if (cnt > flashCfg->timeE64k * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr)
{
    uint32_t len = 0;
    uint32_t eraseLen = 0;
    int ret = SUCCESS;

    while (startaddr <= endaddr) {
        len = endaddr - startaddr + 1;

        if (flashCfg->blk64EraseCmd != SPIFLASH_CMD_INVALID &&
            (startaddr & (SPIFLASH_BLK64K_SIZE - 1)) == 0 &&
            len > (SPIFLASH_BLK64K_SIZE - flashCfg->sectorSize * 1024)) {
            /* 64K margin address,and length > 64K-sector size, erase one first */
            ret = SFlash_Blk64_Erase(flashCfg, startaddr / SPIFLASH_BLK64K_SIZE);
            eraseLen = SPIFLASH_BLK64K_SIZE;
        } else if (flashCfg->blk32EraseCmd != SPIFLASH_CMD_INVALID &&
                   (startaddr & (SPIFLASH_BLK32K_SIZE - 1)) == 0 &&
                   len > (SPIFLASH_BLK32K_SIZE - flashCfg->sectorSize * 1024)) {
            /* 32K margin address,and length > 32K-sector size, erase one first */
            ret = SFlash_Blk32_Erase(flashCfg, startaddr / SPIFLASH_BLK32K_SIZE);
            eraseLen = SPIFLASH_BLK32K_SIZE;
        } else {
            /* Sector erase */
            startaddr = ((startaddr) & (~(flashCfg->sectorSize * 1024 - 1)));
            ret = SFlash_Sector_Erase(flashCfg, startaddr / flashCfg->sectorSize / 1024);
            eraseLen = flashCfg->sectorSize * 1024;
        }

        startaddr += eraseLen;

        if (ret != SUCCESS) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Program(SPI_Flash_Cfg_Type *flashCfg,
                                            SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t i = 0, curLen = 0;
    uint32_t cnt = 0;
    int stat;
    uint8_t cmd;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode || SF_CTRL_DO_MODE == ioMode || SF_CTRL_DIO_MODE == ioMode) {
        cmd = flashCfg->pageProgramCmd;
    } else if (SF_CTRL_QIO_MODE == ioMode || SF_CTRL_QO_MODE == ioMode) {
        flashCmd.addrMode = (SF_Ctrl_Addr_Mode_Type)flashCfg->qppAddrMode;
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->qpageProgramCmd;
    } else {
        return ERROR;
    }

    /* Prepare command */
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.addrSize = 3;

    for (i = 0; i < len;) {
        /* Write enable is needed for every program */
        stat = SFlash_Write_Enable(flashCfg);

        if (stat != SUCCESS) {
            return stat;
        }

        /* Get current programmed length within page size */
        curLen = flashCfg->pageSize - addr % flashCfg->pageSize;

        if (curLen > len - i) {
            curLen = len - i;
        }

        /* Prepare command */
        arch_memcpy_fast(flashCtrlBuf, data, curLen);
        flashCmd.cmdBuf[0] = (cmd << 24) | (addr);
        flashCmd.nbData = curLen;

        SF_Ctrl_SendCmd(&flashCmd);

        /* Adjust address and programmed length */
        addr += curLen;
        i += curLen;
        data += curLen;

        /* Wait for write done */
        cnt = 0;

        while (SET == SFlash_Busy(flashCfg)) {
            arch_delay_us(100);
            cnt++;

            if (cnt > flashCfg->timePagePgm * 20) {
                return ERROR;
            }
        }
    }

    return SUCCESS;
}

void ATTR_TCM_SECTION SFlash_GetUniqueId(uint8_t *data, uint8_t idLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    dummyClks = 4;
    cmd = 0x4B;
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = idLen;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    arch_memcpy(data, flashCtrlBuf, idLen);
}

void ATTR_TCM_SECTION SFlash_GetJedecId(SPI_Flash_Cfg_Type *flashCfg, uint8_t *data)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    dummyClks = flashCfg->jedecIdCmdDmyClk;
    cmd = flashCfg->jedecIdCmd;
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    arch_memcpy(data, flashCtrlBuf, 3);
}

void ATTR_TCM_SECTION SFlash_GetDeviceId(uint8_t *data)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t addr = 0x00000001;
    uint8_t readMode = 0xFF;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.addrMode = SF_CTRL_ADDR_4_LINES;
    flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
    dummyClks = 2;
    cmd = 0x94;
    flashCmd.cmdBuf[0] = (cmd << 24) | (addr);
    flashCmd.cmdBuf[1] = (readMode << 24);
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 4;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = 2;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    arch_memcpy(data, flashCtrlBuf, 2);
}

void ATTR_TCM_SECTION SFlash_Powerdown(void)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint8_t cmd = 0;
    uint32_t timeOut = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    cmd = 0xB9;
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_WRITE;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }
}

void ATTR_TCM_SECTION SFlash_Releae_Powerdown(SPI_Flash_Cfg_Type *flashCfg)
{
    uint8_t cmd;
    uint32_t timeOut = 0;

    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    cmd = flashCfg->releasePowerDown;
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_WRITE;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }
}

int ATTR_TCM_SECTION SFlash_Restore_From_Powerdown(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t flashContRead)
{
    int stat = SUCCESS;
    uint32_t jdecId = 0;
    uint8_t tmp[8];
    uint8_t ioMode = pFlashCfg->ioMode & 0xf;

    /* Wake flash up from power down */
    SFlash_Releae_Powerdown(pFlashCfg);
    arch_delay_us(120);

    SFlash_GetJedecId(pFlashCfg, (uint8_t *)&jdecId);

    if (SF_CTRL_QO_MODE == ioMode || SF_CTRL_QIO_MODE == ioMode) {
        SFlash_Qspi_Enable(pFlashCfg);
    }

    if (((pFlashCfg->ioMode >> 4) & 0x01) == 1) {
        /* unwrap */
    } else {
        /* burst wrap */
        /* For command that is setting register instead of send command, we need write enable */
        SFlash_Write_Enable(pFlashCfg);
        SFlash_SetBurstWrap(pFlashCfg);
    }

    if (flashContRead) {
        stat = SFlash_Read(pFlashCfg, ioMode, 1, 0x00000000, (uint8_t *)tmp, sizeof(tmp));
        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_IAHB);
    } else {
        stat = SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 0, 0, 32, 0);
    }

    return stat;
}

void ATTR_TCM_SECTION SFlash_SetBurstWrap(SPI_Flash_Cfg_Type *flashCfg)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t wrapData;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((flashCfg->ioMode >> 4) & 0x01) == 1) {
        /* Disable burst wrap ,just return */
        return;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.addrMode = (SF_Ctrl_Addr_Mode_Type)flashCfg->burstWrapDataMode;
    flashCmd.dataMode = (SF_Ctrl_Data_Mode_Type)flashCfg->burstWrapDataMode;
    dummyClks = flashCfg->burstWrapCmdDmyClk;
    cmd = flashCfg->burstWrapCmd;
    wrapData = flashCfg->burstWrapData;
    arch_memcpy4((uint32_t *)flashCtrlBuf, &wrapData, 4);
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = 1;

    SF_Ctrl_SendCmd(&flashCmd);
}

void ATTR_TCM_SECTION SFlash_DisableBurstWrap(SPI_Flash_Cfg_Type *flashCfg)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t wrapData;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.addrMode = (SF_Ctrl_Addr_Mode_Type)flashCfg->deBurstWrapDataMode;
    flashCmd.dataMode = (SF_Ctrl_Data_Mode_Type)flashCfg->deBurstWrapDataMode;
    dummyClks = flashCfg->deBurstWrapCmdDmyClk;
    cmd = flashCfg->deBurstWrapCmd;
    wrapData = flashCfg->deBurstWrapData;
    arch_memcpy4((uint32_t *)flashCtrlBuf, &wrapData, 4);
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = 1;

    SF_Ctrl_SendCmd(&flashCmd);
}

int ATTR_TCM_SECTION SFlash_Software_Reset(SPI_Flash_Cfg_Type *flashCfg)
{
    uint16_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    /* Reset enable */
    flashCmd.cmdBuf[0] = (flashCfg->resetEnCmd) << 24;
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;

    /* Wait for write done */
    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(100);
        cnt++;

        if (cnt > 20) {
            return ERROR;
        }
    }

    SF_Ctrl_SendCmd(&flashCmd);

    /* Reset */
    flashCmd.cmdBuf[0] = (flashCfg->resetCmd) << 24;
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    SF_Ctrl_SendCmd(&flashCmd);

    arch_delay_us(50);

    return SUCCESS;
}

void ATTR_TCM_SECTION SFlash_Reset_Continue_Read(SPI_Flash_Cfg_Type *flashCfg)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    /* Reset continous read */
    arch_memset(&flashCmd.cmdBuf[0], flashCfg->resetCreadCmd, 4);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = flashCfg->resetCreadCmdSize;
    SF_Ctrl_SendCmd(&flashCmd);
}

int ATTR_TCM_SECTION SFlash_Set_IDbus_Cfg(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead,
                                                  uint32_t addr, uint32_t len, uint8_t bank)

{
    uint8_t cmd, dummyClks;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint8_t cmdValid = 1;
    uint8_t noReadModeCfg = 0;
    uint8_t cReadSupport = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_IAHB);

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = flashCfg->fastReadCmd;
        dummyClks = flashCfg->frDmyClk;
    } else if (SF_CTRL_DO_MODE == ioMode) {
        flashCmd.dataMode = SF_CTRL_DATA_2_LINES;
        cmd = flashCfg->fastReadDoCmd;
        dummyClks = flashCfg->frDoDmyClk;
    } else if (SF_CTRL_DIO_MODE == ioMode) {
        flashCmd.addrMode = SF_CTRL_ADDR_2_LINES;
        flashCmd.dataMode = SF_CTRL_DATA_2_LINES;
        cmd = flashCfg->fastReadDioCmd;
        dummyClks = flashCfg->frDioDmyClk;
    } else if (SF_CTRL_QO_MODE == ioMode) {
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->fastReadQoCmd;
        dummyClks = flashCfg->frQoDmyClk;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        flashCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->fastReadQioCmd;
        dummyClks = flashCfg->frQioDmyClk;
    } else {
        return ERROR;
    }

    /*prepare command**/
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;
    flashCmd.cmdBuf[0] = (cmd << 24) | addr;

    if (SF_CTRL_QIO_MODE == ioMode || SF_CTRL_DIO_MODE == ioMode) {
        noReadModeCfg = flashCfg->cReadSupport & 0x02;
        cReadSupport = flashCfg->cReadSupport & 0x01;

        if (noReadModeCfg == 0) {
            /* Read mode must be set*/
            if (cReadSupport == 0) {
                /* Not support cont read,but we still need set read mode(winbond 80dv)*/
                flashCmd.cmdBuf[1] = (flashCfg->cReadMode << 24);
            } else {
                /* Flash support cont read, setting depend on user parameter */
                if (contRead) {
                    flashCmd.cmdBuf[0] = (addr << 8) | flashCfg->cReadMode;
                    cmdValid = 0;
                } else {
                    flashCmd.cmdBuf[1] = ((!flashCfg->cReadMode) << 24);
                }
            }

            flashCmd.addrSize++;
        }
    }

    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = len;
    SF_Ctrl_Flash_Read_Icache_Set(&flashCmd, cmdValid);

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_IDbus_Read_Enable(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode,
                                                      uint8_t contRead, uint8_t bank)
{
    int stat;

    stat = SFlash_Set_IDbus_Cfg(flashCfg, ioMode, contRead, 0, 32, bank);

    if (SUCCESS != stat) {
        return stat;
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Cache_Read_Enable(SPI_Flash_Cfg_Type *flashCfg,
                                                      SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint8_t wayDisable)
{
    int stat;

    /* Cache now only support 32 bytes read */
    stat = SFlash_Set_IDbus_Cfg(flashCfg, ioMode, contRead, 0, 32, 0);

    if (SUCCESS != stat) {
        return stat;
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Read(SPI_Flash_Cfg_Type *flashCfg,
                                         SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t curLen, i;
    uint8_t cmd, dummyClks;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint8_t noReadModeCfg = 0;
    uint8_t cReadSupport = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        arch_memset(&flashCmd, 0, sizeof(flashCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = flashCfg->fastReadCmd;
        dummyClks = flashCfg->frDmyClk;
    } else if (SF_CTRL_DO_MODE == ioMode) {
        flashCmd.dataMode = SF_CTRL_DATA_2_LINES;
        cmd = flashCfg->fastReadDoCmd;
        dummyClks = flashCfg->frDoDmyClk;
    } else if (SF_CTRL_DIO_MODE == ioMode) {
        flashCmd.addrMode = SF_CTRL_ADDR_2_LINES;
        flashCmd.dataMode = SF_CTRL_DATA_2_LINES;
        cmd = flashCfg->fastReadDioCmd;
        dummyClks = flashCfg->frDioDmyClk;
    } else if (SF_CTRL_QO_MODE == ioMode) {
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->fastReadQoCmd;
        dummyClks = flashCfg->frQoDmyClk;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        flashCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->fastReadQioCmd;
        dummyClks = flashCfg->frQioDmyClk;
    } else {
        return ERROR;
    }

    /* Prepare command */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    if (SF_CTRL_QIO_MODE == ioMode || SF_CTRL_DIO_MODE == ioMode) {
        noReadModeCfg = flashCfg->cReadSupport & 0x02;
        cReadSupport = flashCfg->cReadSupport & 0x01;

        if (noReadModeCfg == 0) {
            /* Read mode must be set*/
            if (cReadSupport == 0) {
                /* Not support cont read,but we still need set read mode(winbond 80dv)*/
                flashCmd.cmdBuf[1] = (flashCfg->cReadMode << 24);
            } else {
                /* Flash support cont read, setting depend on user parameter */
                if (contRead) {
                    flashCmd.cmdBuf[1] = (flashCfg->cReadMode << 24);
                } else {
                    flashCmd.cmdBuf[1] = ((!flashCfg->cReadMode) << 24);
                }
            }

            flashCmd.addrSize++;
        }
    }

    flashCmd.dummyClks = dummyClks;

    /* Read data */
    for (i = 0; i < len;) {
        /* Prepare command */
        flashCmd.cmdBuf[0] = (cmd << 24) | (addr);
        curLen = len - i;

        if (curLen >= FLASH_CTRL_BUF_SIZE) {
            curLen = FLASH_CTRL_BUF_SIZE;
            flashCmd.nbData = curLen;
        } else {
            /* Make sf_ctrl word read */
            flashCmd.nbData = ((curLen + 3) >> 2) << 2;
        }

        SF_Ctrl_SendCmd(&flashCmd);

        timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

        while (SET == SF_Ctrl_GetBusyState()) {
            timeOut--;

            if (timeOut == 0) {
                return TIMEOUT;
            }
        }

        arch_memcpy_fast(data, flashCtrlBuf, curLen);

        addr += curLen;
        i += curLen;
        data += curLen;
    }

    return SUCCESS;
}

