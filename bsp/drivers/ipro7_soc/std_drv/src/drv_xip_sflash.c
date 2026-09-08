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
#include "drv_xip_sflash.h"

void ATTR_TCM_SECTION XIP_SFlash_Opt_Enter(uint8_t *aesEnable)
{
    *aesEnable = SF_Ctrl_Is_AES_Enable();

    if (*aesEnable) {
        SF_Ctrl_AES_Disable();
    }
}

void ATTR_TCM_SECTION XIP_SFlash_Opt_Exit(uint8_t aesEnable)
{
    if (aesEnable) {
        SF_Ctrl_AES_Enable();
    }
}

int ATTR_TCM_SECTION XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset,
                                                   uint8_t group, uint8_t bank)
{
    /* XIP_SFlash_Delay */
    volatile uint32_t i = 32 * 2;

    while (i--)
        ;

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);

    /* Exit form continous read for accepting command */
    SFlash_Reset_Continue_Read(pFlashCfg);
    /* Send software reset command(80bv has no this command)to deburst wrap for ISSI like */
    /* For disable command that is setting register instaed of send command, we need write enable */
    SFlash_DisableBurstWrap(pFlashCfg);
    //SFlash_Set32BitsAddrMode(pFlashCfg, ENABLE);
    if ((pFlashCfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        /* Enable QE again in case reset command make it reset */
        SFlash_Qspi_Enable(pFlashCfg);
    }
    /* Deburst again to make sure */
    SFlash_DisableBurstWrap(pFlashCfg);

    /* Clear offset setting*/
    *offset = SF_Ctrl_Get_Flash_Image_Offset(group, bank);
    SF_Ctrl_Set_Flash_Image_Offset(0, group, bank);

    return SUCCESS;
}

int ATTR_TCM_SECTION XIP_SFlash_State_Restore(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t offset,
                                                      uint8_t group, uint8_t bank)
{
    uint32_t tmp[1];
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    SF_Ctrl_Set_Flash_Image_Offset(offset, group, bank);

    if(((pFlashCfg->ioMode>>4)&0x01) == 0) {
        if((pFlashCfg->ioMode&0x0f)==SF_CTRL_QO_MODE || (pFlashCfg->ioMode&0x0f)==SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(pFlashCfg);
        }
    }
    //SFlash_Set32BitsAddrMode(pFlashCfg, ENABLE);
    SFlash_Read(pFlashCfg, ioMode, 1, 0x0, (uint8_t *)tmp, sizeof(tmp));
    SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);


    return SUCCESS;
}

int ATTR_TCM_SECTION XIP_SFlash_Erase_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t startaddr, int len,
                                                        uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Erase(pFlashCfg, startaddr, startaddr + len - 1);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

int ATTR_TCM_SECTION XIP_SFlash_Write_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len,
                                                        uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Program(pFlashCfg, ioMode, addr, data, len);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);
    return stat;
}

int ATTR_TCM_SECTION XIP_SFlash_Read_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len,
                                                       uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Read(pFlashCfg, ioMode, 0, addr, data, len);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);
    return stat;
}

int ATTR_TCM_SECTION XIP_SFlash_GetJedecId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data,
                                                             uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetJedecId(pFlashCfg, data);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);
    return SUCCESS;
}

int ATTR_TCM_SECTION XIP_SFlash_GetDeviceId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, int is32BitsAddr, uint8_t *data,
                                                              uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetDeviceId(data);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}


int ATTR_TCM_SECTION XIP_SFlash_GetUniqueId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t idLen,
                                                              uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetUniqueId(data, idLen);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}

int ATTR_TCM_SECTION XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t offset;

    /* Caller passes a flash physical address (e.g. partition base from PT).
     * The XIP window is mapped at DRV_FLASH_XIP_BASE through SF_Ctrl with a
     * runtime image offset (boot2/OTA may set it non-zero so XIP_BASE points
     * at the active FW slot). Zero the offset for the duration of the read
     * so XIP_BASE+phys resolves to the requested flash address, then restore
     * it. Cache aliasing across the toggle is avoided by the caller running
     * with IRQs disabled and this function executing from TCM — no XIP code
     * runs while the offset is wrong, and arch_memcpy_fast' lines are tagged
     * under the temporary zero mapping (only an issue if subsequent reads
     * via XIP cache hit the same line addresses, which they don't here). */
    addr = addr & (DRV_FLASH_XIP_END - DRV_FLASH_XIP_BASE - 1);
    addr |= DRV_FLASH_XIP_BASE;

    offset = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    SF_Ctrl_Set_Flash_Image_Offset(0, 0, 0);
    arch_memcpy_fast(data, (void *)(addr), len);
    SF_Ctrl_Set_Flash_Image_Offset(offset, 0, 0);

    return SUCCESS;
}

