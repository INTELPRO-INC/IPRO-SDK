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

#include "drv_psram.h"

void ATTR_TCM_SECTION Psram_Init(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg)
{
    SF_Ctrl_Psram_Init(sfCtrlPsramCfg);
    SF_Ctrl_Cmds_Set(cmdsCfg, SF_CTRL_SEL_PSRAM);

    Psram_SetDriveStrength(psramCfg);
    Psram_SetBurstWrap(psramCfg);
}

void ATTR_TCM_SECTION Psram_ReadReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue)
{
    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    if (psramCfg->ctrlMode == PSRAM_QPI_CTRL_MODE) {
        psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
    }

    psramCmd.cmdBuf[0] = (psramCfg->readRegCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;
    psramCmd.addrSize = 3;
    psramCmd.dummyClks = psramCfg->readRegDmyClk;
    psramCmd.nbData = 1;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    arch_memcpy(regValue, psramCtrlBuf, 1);
}

void ATTR_TCM_SECTION Psram_WriteReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue)
{
    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    arch_memcpy(psramCtrlBuf, regValue, 1);

    if (psramCfg->ctrlMode == PSRAM_QPI_CTRL_MODE) {
        psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
    }

    psramCmd.cmdBuf[0] = (psramCfg->writeRegCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_WRITE;
    psramCmd.addrSize = 3;
    psramCmd.nbData = 1;

    SF_Ctrl_SendCmd(&psramCmd);
}

int ATTR_TCM_SECTION Psram_SetDriveStrength(SPI_Psram_Cfg_Type *psramCfg)
{
    uint32_t stat = 0;

    Psram_ReadReg(psramCfg, (uint8_t *)&stat);

    if ((stat & 0x3) == psramCfg->driveStrength) {
        return SUCCESS;
    }

    stat &= (~0x3);
    stat |= psramCfg->driveStrength;

    Psram_WriteReg(psramCfg, (uint8_t *)&stat);
    /* Wait for write done */

    Psram_ReadReg(psramCfg, (uint8_t *)&stat);

    if ((stat & 0x3) == psramCfg->driveStrength) {
        return SUCCESS;
    }

    return ERROR;
}

int ATTR_TCM_SECTION Psram_SetBurstWrap(SPI_Psram_Cfg_Type *psramCfg)
{
    uint32_t stat = 0;

    Psram_ReadReg(psramCfg, (uint8_t *)&stat);

    if (((stat >> 5) & 0x3) == psramCfg->burstLength) {
        return SUCCESS;
    }

    stat &= (~(0x3 << 5));
    stat |= (psramCfg->burstLength << 5);

    Psram_WriteReg(psramCfg, (uint8_t *)&stat);
    /* Wait for write done */

    Psram_ReadReg(psramCfg, (uint8_t *)&stat);

    if (((stat >> 5) & 0x3) == psramCfg->burstLength) {
        return SUCCESS;
    }

    return ERROR;
}

void ATTR_TCM_SECTION Psram_ReadId(SPI_Psram_Cfg_Type *psramCfg, uint8_t *data)
{
    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    psramCmd.cmdBuf[0] = (psramCfg->readIdCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;
    psramCmd.addrSize = 3;
    psramCmd.dummyClks = psramCfg->readIdDmyClk;
    psramCmd.nbData = 8;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    arch_memcpy(data, psramCtrlBuf, 8);
}

int ATTR_TCM_SECTION Psram_EnterQuadMode(SPI_Psram_Cfg_Type *psramCfg)
{
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    psramCmd.cmdBuf[0] = (psramCfg->enterQuadModeCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION Psram_ExitQuadMode(SPI_Psram_Cfg_Type *psramCfg)
{
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
    psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
    psramCmd.dataMode = SF_CTRL_DATA_4_LINES;

    psramCmd.cmdBuf[0] = (psramCfg->exitQuadModeCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION Psram_ToggleBurstLength(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode)
{
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    if (ctrlMode == PSRAM_QPI_CTRL_MODE) {
        psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
    }

    psramCmd.cmdBuf[0] = (psramCfg->burstToggleCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION Psram_SoftwareReset(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode)
{
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    if (ctrlMode == PSRAM_QPI_CTRL_MODE) {
        psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
    }

    /* Reset enable */
    psramCmd.cmdBuf[0] = (psramCfg->resetEnableCmd) << 24;
    /* rwFlag don't care */
    psramCmd.rwFlag = SF_CTRL_READ;
    /* Wait for write done */

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    /* Reset */
    psramCmd.cmdBuf[0] = (psramCfg->resetCmd) << 24;
    /* rwFlag don't care */
    psramCmd.rwFlag = SF_CTRL_READ;
    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    arch_delay_us(50);
    return SUCCESS;
}

int ATTR_TCM_SECTION Psram_Set_IDbus_Cfg(SPI_Psram_Cfg_Type *psramCfg,
                                                 SF_Ctrl_IO_Type ioMode, uint32_t addr, uint32_t len)
{
    uint8_t cmd, dummyClks;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;
    uint8_t cmdValid = 1;

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_IAHB);

    /* read mode cache set */
    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = psramCfg->fReadCmd;
        dummyClks = psramCfg->fReadDmyClk;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = psramCfg->fReadQuadCmd;
        dummyClks = psramCfg->fReadQuadDmyClk;
    } else {
        return ERROR;
    }

    /* prepare command */
    psramCmd.rwFlag = SF_CTRL_READ;
    psramCmd.addrSize = 3;
    psramCmd.cmdBuf[0] = (cmd << 24) | addr;
    psramCmd.dummyClks = dummyClks;
    psramCmd.nbData = len;
    SF_Ctrl_Psram_Read_Icache_Set(&psramCmd, cmdValid);

    /* write mode cache set */
    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = psramCfg->writeCmd;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = psramCfg->quadWriteCmd;
    } else {
        return ERROR;
    }

    dummyClks = 0;

    /* prepare command */
    psramCmd.rwFlag = SF_CTRL_WRITE;
    psramCmd.addrSize = 3;
    psramCmd.cmdBuf[0] = (cmd << 24) | addr;
    psramCmd.dummyClks = dummyClks;
    psramCmd.nbData = len;
    SF_Ctrl_Psram_Write_Icache_Set(&psramCmd, cmdValid);
    return SUCCESS;
}

int ATTR_TCM_SECTION Psram_Cache_Write_Set(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode,
                                                   int wtEn, int wbEn, int waEn)
{
    int stat;

    /* Cache now only support 32 bytes read */
    stat = Psram_Set_IDbus_Cfg(psramCfg, ioMode, 0, 32);

    if (SUCCESS != stat) {
        return stat;
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION Psram_Write(SPI_Psram_Cfg_Type *psramCfg,
                                         SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t i = 0, curLen = 0;
    uint32_t burstLen = 512;
    uint8_t cmd;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = psramCfg->writeCmd;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = psramCfg->quadWriteCmd;
    } else {
        return ERROR;
    }

    /* Prepare command */
    psramCmd.rwFlag = SF_CTRL_WRITE;
    psramCmd.addrSize = 3;

    if (psramCfg->burstLength == PSRAM_BURST_LENGTH_16_BYTES) {
        burstLen = 16;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_32_BYTES) {
        burstLen = 32;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_64_BYTES) {
        burstLen = 64;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_512_BYTES) {
        burstLen = 512;
    }

    for (i = 0; i < len;) {
        /* Get current programmed length within page size */
        curLen = burstLen - addr % burstLen;

        if (curLen > len - i) {
            curLen = len - i;
        }

        /* Prepare command */
        arch_memcpy_fast(psramCtrlBuf, data, curLen);
        psramCmd.cmdBuf[0] = (cmd << 24) | (addr);
        psramCmd.nbData = curLen;

        SF_Ctrl_SendCmd(&psramCmd);

        /* Adjust address and programmed length */
        addr += curLen;
        i += curLen;
        data += curLen;

        /* Wait for write done */
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION Psram_Read(SPI_Psram_Cfg_Type *psramCfg,
                                        SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t curLen, i;
    uint32_t burstLen = 512;
    uint8_t cmd, dummyClks;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        arch_memset4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        arch_memset(&psramCmd, 0, sizeof(psramCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = psramCfg->fReadCmd;
        dummyClks = psramCfg->fReadDmyClk;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = psramCfg->fReadQuadCmd;
        dummyClks = psramCfg->fReadQuadDmyClk;
    } else {
        return ERROR;
    }

    /* Prepare command */
    psramCmd.rwFlag = SF_CTRL_READ;
    psramCmd.addrSize = 3;
    psramCmd.dummyClks = dummyClks;

    if (psramCfg->burstLength == PSRAM_BURST_LENGTH_16_BYTES) {
        burstLen = 16;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_32_BYTES) {
        burstLen = 32;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_64_BYTES) {
        burstLen = 64;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_512_BYTES) {
        burstLen = 512;
    }

    /* Read data */
    for (i = 0; i < len;) {
        /* Prepare command */
        psramCmd.cmdBuf[0] = (cmd << 24) | (addr);
        curLen = burstLen - addr % burstLen;

        if (curLen > len - i) {
            curLen = len - i;
        }

        if (curLen >= FLASH_CTRL_BUF_SIZE) {
            curLen = FLASH_CTRL_BUF_SIZE;
            psramCmd.nbData = curLen;
        } else {
            /* Make sf_ctrl word read */
            psramCmd.nbData = ((curLen + 3) >> 2) << 2;
        }

        SF_Ctrl_SendCmd(&psramCmd);

        while (SET == SF_Ctrl_GetBusyState()) {
        }

        arch_memcpy_fast(data, psramCtrlBuf, curLen);

        addr += curLen;
        i += curLen;
        data += curLen;
    }

    return SUCCESS;
}

