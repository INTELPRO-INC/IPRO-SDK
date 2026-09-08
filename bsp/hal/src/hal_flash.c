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

#include "drv_glb.h"
#include "drv_xip_sflash.h"
#include "drv_xip_sflash_ext.h"
#include "drv_sf_cfg.h"
#include "drv_sf_cfg_ext.h"
#include "drv_ef_ctrl.h"
#include "hal_flash.h"
#include "compiler/common.h"
#include <generated/autoconf.h>
#ifdef CONFIG_IPRO7
#include "boot2_param.h"
#endif

static uint32_t g_jedec_id = 0;
static ATTR_OCRAM_SECTION SPI_Flash_Cfg_Type g_flash_cfg = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0x0,

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
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 0,
    .cReadMode = 0xFF,

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
    //.clkDelay = 1,
    .clkDelay = 0,
    .clkInvert = 0x3d,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
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
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

uint32_t flash_get_jedecid(void)
{
    uint32_t jid = 0;

    jid = ((g_jedec_id&0xff)<<16) + (g_jedec_id&0xff00) + ((g_jedec_id&0xff0000)>>16);
    return jid;
}

int flash_get_cfg(uint8_t **cfg_addr, uint32_t *len)
{
    *cfg_addr = (uint8_t *)&g_flash_cfg;
    *len = sizeof(SPI_Flash_Cfg_Type);

    return SUCCESS;
}

static int ATTR_TCM_SECTION flash_set_qspi_enable(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        SFlash_Qspi_Enable(p_flash_cfg);
    }

    return SUCCESS;
}

static int ATTR_TCM_SECTION flash_set_burst_wrap(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if (((p_flash_cfg->ioMode >> 4) & 0x01) != 1) {
        if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(p_flash_cfg);
        }
    }

    return SUCCESS;
}

static uint32_t ATTR_TCM_SECTION flash_get_size_from_jedecid(uint32_t jedec_id)
{
    uint8_t flash_size_level = 0;
    uint32_t flash_size = 0;
    uint32_t jid = 0;

    jid = ((jedec_id&0xff)<<16) + (jedec_id&0xff00) + ((jedec_id&0xff0000)>>16);

    if (jid == 0) {
        return 0;
    }

    flash_size_level = (jid & 0x1f);
    flash_size_level -= 0x13;
    flash_size = (1 << flash_size_level) * 512 * 1024;

    return flash_size;
}

static int ATTR_TCM_SECTION flash_sf2_gpio_init(void)
{
    return SUCCESS;
}

static int ATTR_TCM_SECTION flash_config_init(SPI_Flash_Cfg_Type *p_flash_cfg, uint8_t *jedec_id)
{
    int ret = ERROR;
    uint32_t jid = 0;
    uint32_t offset = 0;
    uint8_t aesEnable;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter(&aesEnable);
    XIP_SFlash_State_Save(p_flash_cfg, &offset, 0, 0);
    flash_sf2_gpio_init();
    SFlash_GetJedecId(p_flash_cfg, (uint8_t *)&jid);
    memcpy(jedec_id, (uint8_t *)&jid, 3);
    jid &= 0xFFFFFF;
    g_jedec_id = jid;
    ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jid, p_flash_cfg);
    if (ret == SUCCESS) {
        p_flash_cfg->mid = (jid & 0xff);
    }

    /* Set flash controler from p_flash_cfg */
    flash_set_qspi_enable(p_flash_cfg);
    flash_set_burst_wrap(p_flash_cfg);

    XIP_SFlash_State_Restore(p_flash_cfg, offset, 0, 0);
    XIP_SFlash_Opt_Exit(aesEnable);
    cpu_global_irq_enable();

    return ret;
}

int ATTR_TCM_SECTION flash_init(void)
{
    int ret = ERROR;
    uint8_t clkDelay = 1;
    uint8_t clkInvert = 1;
    uint32_t jedec_id = 0;

#ifdef CONFIG_FLASH_CFG_FROM_BOOTROM
    /* Use flash config from bootrom via boot2 parameters.
     * This allows adding new flash chips with tool-only changes
     * (no SDK driver modification needed). */
    SPI_Flash_Cfg_Type *boot2FlashCfg = Boot2_Param_Get_Flash_Cfg();
    if (boot2FlashCfg != NULL) {
        memcpy(&g_flash_cfg, boot2FlashCfg, sizeof(SPI_Flash_Cfg_Type));
        g_flash_cfg.mid = readl(0x30000078) & 0xFF;
        g_jedec_id = readl(0x30000078) & 0xFFFFFF;
        return SUCCESS;
    }
    printf("boot2 flash cfg not available, falling back to detection\r\n");
#endif

    /* Fall back to flash detection */
    cpu_global_irq_disable();
    SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(0, &g_flash_cfg);
    cpu_global_irq_enable();
    if (g_flash_cfg.mid != 0xff && g_flash_cfg.mid != 0x00) {
        /* Brom already detected JEDEC and latched it in GLB regfile 0x30000078
         * (same register the CONFIG_FLASH_CFG_FROM_BOOTROM path reads).
         * Without this, g_jedec_id stays 0 and flash_get_size() returns 0. */
        g_jedec_id = readl(0x30000078) & 0xFFFFFF;
        return SUCCESS;
    }
    clkDelay = g_flash_cfg.clkDelay;
    clkInvert = g_flash_cfg.clkInvert;
    g_flash_cfg.ioMode = g_flash_cfg.ioMode & 0x0f;

    SF_Ctrl_SetDelay(clkDelay);

    ret = flash_config_init(&g_flash_cfg, (uint8_t *)&jedec_id);

    printf("flash ID = %08lx\r\n", jedec_id);
    if (ret != SUCCESS) {
        printf("flash config init fail!\r\n");
    }
    g_jedec_id = jedec_id & 0xFFFFFF;

    g_flash_cfg.clkDelay = clkDelay;
    g_flash_cfg.clkInvert = clkInvert;

    return ret;
}

uint32_t flash_get_size(void)
{
    return flash_get_size_from_jedecid(g_jedec_id);
}

int ATTR_TCM_SECTION flash_read_jedec_id(uint8_t *data)
{
    uint8_t aesEnable;
    uint32_t jid = 0;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter(&aesEnable);
    XIP_SFlash_GetJedecId_Need_Lock(&g_flash_cfg, (uint8_t *)&jid, 0, 0);
    XIP_SFlash_Opt_Exit(aesEnable);
    cpu_global_irq_enable();
    jid &= 0xFFFFFF;
    memcpy(data, (void *)&jid, 4);

    return SUCCESS;
}

int ATTR_TCM_SECTION flash_read_via_xip(uint32_t addr, uint8_t *data, uint32_t len)
{
    cpu_global_irq_disable();
    XIP_SFlash_Read_Via_Cache_Need_Lock(addr, data, len);
    cpu_global_irq_enable();

    return SUCCESS;
}

int ATTR_TCM_SECTION flash_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t aesEnable;
    int ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter(&aesEnable);
    ret = XIP_SFlash_Read_Need_Lock(&g_flash_cfg, addr, data, len, 0, 0);
    XIP_SFlash_Opt_Exit(aesEnable);
    cpu_global_irq_enable();

    return ret;
}

int ATTR_TCM_SECTION flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t aesEnable;
    int ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter(&aesEnable);
    ret = XIP_SFlash_Write_Need_Lock(&g_flash_cfg, addr, data, len, 0, 0);
    XIP_SFlash_Opt_Exit(aesEnable);
    cpu_global_irq_enable();

    return ret;
}

int ATTR_TCM_SECTION flash_erase(uint32_t startaddr, uint32_t len)
{
    uint8_t aesEnable;
    int ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter(&aesEnable);
    ret = XIP_SFlash_Erase_Need_Lock(&g_flash_cfg, startaddr, len, 0, 0);
    XIP_SFlash_Opt_Exit(aesEnable);
    cpu_global_irq_enable();

    return ret;
}

int ATTR_TCM_SECTION flash_clear_status_register(void)
{
    uint8_t aesEnable;
    int ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter(&aesEnable);
    ret = XIP_SFlash_Clear_Status_Register_Need_Lock(&g_flash_cfg, 0, 0);
    XIP_SFlash_Opt_Exit(aesEnable);
    cpu_global_irq_enable();

    return ret;
}

uint32_t flash_readReg(uint8_t readRegCmd, uint8_t len)
{
    uint32_t val;
    uint32_t offset;

    XIP_SFlash_State_Save(&g_flash_cfg, &offset, 0, 0);
    SFlash_Read_Reg_With_Cmd(&g_flash_cfg, readRegCmd, (uint8_t *)&val, len);
    XIP_SFlash_State_Restore(&g_flash_cfg, offset, 0, 0);

    return val;
}

void flash_writeReg(uint8_t writeRegCmd, uint32_t writeVal, uint8_t len)
{
    uint32_t offset;

    XIP_SFlash_State_Save(&g_flash_cfg, &offset, 0, 0);
    SFlash_Write_Reg_With_Cmd(&g_flash_cfg, writeRegCmd, (uint8_t *)&writeVal, len);
    XIP_SFlash_State_Restore(&g_flash_cfg, offset, 0, 0);
}

int hal_flash_config_update(uint32_t flashCfgAddr)
{
    uint32_t magic = *(uint32_t *) (flashCfgAddr + 4);
    if (magic == FLASH_MAGIC_CODE) {
        printf("=======FlashCfg Found=======\r\n");
        printf("mid=0x%x\r\n", g_flash_cfg.mid);
        printf("sectorSize=%d K\r\n", g_flash_cfg.sectorSize);
        printf("pageSize=%d K\r\n", g_flash_cfg.pageSize);
        printf("ioMode = %x\r\n", g_flash_cfg.ioMode);
#if 0
        printf("cReadSupport=0x%x\r\n", g_flash_cfg.cReadSupport);
        printf("clkDelay=0x%x\r\n", g_flash_cfg.clkDelay);
        printf("clkInvert=0x%x\r\n", g_flash_cfg.clkInvert);
        printf("resetEnCmd=0x%x\r\n", g_flash_cfg.resetEnCmd);
        printf("resetCmd=0x%x\r\n", g_flash_cfg.resetCmd);
        printf("resetCreadCmd=0x%x\r\n", g_flash_cfg.resetCreadCmd);
        printf("resetCreadCmdSize=0x%x\r\n", g_flash_cfg.resetCreadCmdSize);
        printf("jedecIdCmd=0x%x\r\n", g_flash_cfg.jedecIdCmd);
        printf("jedecIdCmdDmyClk=0x%x\r\n", g_flash_cfg.jedecIdCmdDmyClk);
        printf("chipEraseCmd=0x%x\r\n", g_flash_cfg.chipEraseCmd);
        printf("sectorEraseCmd=0x%x\r\n", g_flash_cfg.sectorEraseCmd);
        printf("blk32EraseCmd=0x%x\r\n", g_flash_cfg.blk32EraseCmd);
        printf("blk64EraseCmd=0x%x\r\n", g_flash_cfg.blk64EraseCmd);
        printf("writeEnableCmd=0x%x\r\n", g_flash_cfg.writeEnableCmd);
        printf("pageProgramCmd=0x%x\r\n", g_flash_cfg.pageProgramCmd);
        printf("qpageProgramCmd=0x%x\r\n", g_flash_cfg.qpageProgramCmd);
        printf("qppAddrMode=0x%x\r\n", g_flash_cfg.qppAddrMode);
        printf("fastReadCmd=0x%x\r\n", g_flash_cfg.fastReadCmd);
        printf("frDmyClk=0x%x\r\n", g_flash_cfg.frDmyClk);
        printf("qpiFastReadCmd=0x%x\r\n", g_flash_cfg.qpiFastReadCmd);
        printf("qpiFrDmyClk=0x%x\r\n", g_flash_cfg.qpiFrDmyClk);
        printf("fastReadDoCmd=0x%x\r\n", g_flash_cfg.fastReadDoCmd);
        printf("frDoDmyClk=0x%x\r\n", g_flash_cfg.frDoDmyClk);
        printf("fastReadDioCmd=0x%x\r\n", g_flash_cfg.fastReadDioCmd);
        printf("frDioDmyClk=0x%x\r\n", g_flash_cfg.frDioDmyClk);
        printf("fastReadQoCmd=0x%x\r\n", g_flash_cfg.fastReadQoCmd);
        printf("frQoDmyClk=0x%x\r\n", g_flash_cfg.frQoDmyClk);
        printf("fastReadQioCmd=0x%x\r\n", g_flash_cfg.fastReadQioCmd);
        printf("frQioDmyClk=0x%x\r\n", g_flash_cfg.frQioDmyClk);
        printf("qpiFastReadQioCmd=0x%x\r\n", g_flash_cfg.qpiFastReadQioCmd);
        printf("qpiFrQioDmyClk=0x%x\r\n", g_flash_cfg.qpiFrQioDmyClk);
        printf("qpiPageProgramCmd=0x%x\r\n", g_flash_cfg.qpiPageProgramCmd);
        printf("writeVregEnableCmd=0x%x\r\n", g_flash_cfg.writeVregEnableCmd);
        printf("wrEnableIndex=0x%x\r\n", g_flash_cfg.wrEnableIndex);
        printf("qeIndex=0x%x\r\n", g_flash_cfg.qeIndex);
        printf("busyIndex=0x%x\r\n", g_flash_cfg.busyIndex);
        printf("wrEnableBit=0x%x\r\n", g_flash_cfg.wrEnableBit);
        printf("qeBit=0x%x\r\n", g_flash_cfg.qeBit);
        printf("busyBit=0x%x\r\n", g_flash_cfg.busyBit);
        printf("wrEnableWriteRegLen=0x%x\r\n", g_flash_cfg.wrEnableWriteRegLen);
        printf("wrEnableReadRegLen=0x%x\r\n", g_flash_cfg.wrEnableReadRegLen);
        printf("qeWriteRegLen=0x%x\r\n", g_flash_cfg.qeWriteRegLen);
        printf("qeReadRegLen=0x%x\r\n", g_flash_cfg.qeReadRegLen);
        printf("releasePowerDown=0x%x\r\n", g_flash_cfg.releasePowerDown);
        printf("busyReadRegLen=0x%x\r\n", g_flash_cfg.busyReadRegLen);
        printf("readRegCmd=0x%x 0x%x\r\n", g_flash_cfg.readRegCmd[0], g_flash_cfg.readRegCmd[1]);
        printf("writeRegCmd=0x%x 0x%x\r\n", g_flash_cfg.writeRegCmd[0], g_flash_cfg.writeRegCmd[1]);
        printf("enterQpi=0x%x\r\n", g_flash_cfg.enterQpi);
        printf("exitQpi=0x%x\r\n", g_flash_cfg.exitQpi);
        printf("cReadMode=0x%x\r\n", g_flash_cfg.cReadMode);
        printf("cRExit=0x%x\r\n", g_flash_cfg.cRExit);
        printf("burstWrapCmd=0x%x\r\n", g_flash_cfg.burstWrapCmd);
        printf("burstWrapCmdDmyClk=0x%x\r\n", g_flash_cfg.burstWrapCmdDmyClk);
        printf("burstWrapDataMode=0x%x\r\n", g_flash_cfg.burstWrapDataMode);
        printf("burstWrapData=0x%x\r\n", g_flash_cfg.burstWrapData);
        printf("deBurstWrapCmd=0x%x\r\n", g_flash_cfg.deBurstWrapCmd);
        printf("deBurstWrapCmdDmyClk=0x%x\r\n", g_flash_cfg.deBurstWrapCmdDmyClk);
        printf("deBurstWrapDataMode=0x%x\r\n", g_flash_cfg.deBurstWrapDataMode);
        printf("deBurstWrapData=0x%x\r\n", g_flash_cfg.deBurstWrapData);
        printf("timeEsector=%d\r\n", g_flash_cfg.timeEsector);
        printf("timeE32k=%d\r\n", g_flash_cfg.timeE32k);
        printf("timeE64k=%d\r\n", g_flash_cfg.timeE64k);
        printf("timePagePgm=%d\r\n", g_flash_cfg.timePagePgm);
        printf("timeCe=%d\r\n", g_flash_cfg.timeCe);
        printf("pdDelay=0x%x\r\n", g_flash_cfg.pdDelay);
        printf("qeData=0x%x\r\n", g_flash_cfg.qeData);
#endif
        printf("---------------------------------------------------------------\r\n");
    }

    return 0;
}
