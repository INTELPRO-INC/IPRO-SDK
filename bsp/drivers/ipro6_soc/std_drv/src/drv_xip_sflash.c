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

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset,
                                                   uint8_t group, uint8_t bank)
{
    /* XIP_SFlash_Delay */
    volatile uint32_t i = 32 * 2;

    while (i--)
        ;

    if (bank == SF_CTRL_FLASH_BANK1) {
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
    }
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    /* Exit form continous read for accepting command */
    SFlash_Reset_Continue_Read(pFlashCfg);
    /* For disable command that is setting register instaed of send command, we need write enable */
    SFlash_DisableBurstWrap(pFlashCfg);
    /* Enable 32Bits addr mode again in case reset command make it reset */
    SFlash_Set32BitsAddrMode(pFlashCfg, ENABLE);
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_State_Restore(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t offset,
                                                      uint8_t group, uint8_t bank)
{
    uint32_t tmp[1];
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    SF_Ctrl_Set_Flash_Image_Offset(offset, group, bank);

    if(((pFlashCfg->ioMode>>4)&0x01) == 0) {
        if((pFlashCfg->ioMode&0x0f)==SF_CTRL_QO_MODE || (pFlashCfg->ioMode&0x0f)==SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(pFlashCfg);
        }
    }
    SFlash_Set32BitsAddrMode(pFlashCfg, ENABLE);
    SFlash_Read(pFlashCfg, ioMode, 1, 0x0, (uint8_t *)tmp, sizeof(tmp));
    SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    if (bank == SF_CTRL_FLASH_BANK1) {
        SF_Ctrl_Sbus2_Revoke_replace();
    }

    return SUCCESS;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
        SFlash_GetDeviceId(data, is32BitsAddr);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
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
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t offset;

    addr = addr & (IPRO6_FLASH_XIP_END - IPRO6_FLASH_XIP_BASE - 1);
    addr |= IPRO6_FLASH_XIP_BASE;

    offset = SF_Ctrl_Get_Flash_Image_Offset(0, SF_CTRL_FLASH_BANK0);
    SF_Ctrl_Set_Flash_Image_Offset(0, 0, SF_CTRL_FLASH_BANK0);
    /* Flash read */
    ARCH_MemCpy_Fast(data, (void *)(uintptr_t)(addr - SF_Ctrl_Get_Flash_Image_Offset(0, SF_CTRL_FLASH_BANK0)), len);
    SF_Ctrl_Set_Flash_Image_Offset(offset, 0, SF_CTRL_FLASH_BANK0);

    return SUCCESS;
}
#endif

__WEAK
void ATTR_TCM_SECTION XIP_SFlash_Opt_Enter(uint8_t *aesEnable)
{
    *aesEnable = SF_Ctrl_Is_AES_Enable();

    if (*aesEnable) {
        SF_Ctrl_AES_Disable();
    }
}

__WEAK
void ATTR_TCM_SECTION XIP_SFlash_Opt_Exit(uint8_t aesEnable)
{
    if (aesEnable) {
        SF_Ctrl_AES_Enable();
    }
}

__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg,
                                                                        uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode&0xf;

    stat=XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat=SFlash_Clear_Status_Register(pFlashCfg);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    return stat;
}

