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

#ifndef __HAL_PTMGR_H__
#define __HAL_PTMGR_H__

#include "stdint.h"
#include "drv_sflash.h"
#include "drv_xip_sflash.h"

typedef enum {
    PT_ERROR_SUCCESS,                       /*!< Partition table error type:success */
    PT_ERROR_TABLE_NOT_VALID,               /*!< Partition table error type:entry not found */
    PT_ERROR_ENTRY_NOT_FOUND,               /*!< Partition table error type:entry not found */
    PT_ERROR_ENTRY_UPDATE_FAIL,             /*!< Partition table error type:entry update fail */
    PT_ERROR_CRC32,                         /*!< Partition table error type:crc32 error */
    PT_ERROR_PARAMETER,                     /*!< Partition table error type:input parameter error */
    PT_ERROR_FALSH_READ,                    /*!< Partition table error type:flash read error */
    PT_ERROR_FALSH_WRITE,                   /*!< Partition table error type:flash write error */
    PT_ERROR_FALSH_ERASE,                   /*!< Partition table error type:flash erase error */
}PtTable_Error_Type;

typedef enum {
    PT_TABLE_ID_0,                          /*!< Partition table ID 0 */
    PT_TABLE_ID_1,                          /*!< Partition table ID 1 */
    PT_TABLE_ID_INVALID,                    /*!< Partition table ID invalid */
}PtTable_ID_Type;

typedef enum {
    PT_ENTRY_FW_CPU0,                       /*!< Partition entry type:CPU0 firmware */
    PT_ENTRY_FW_CPU1,                       /*!< Partition entry type:CPU1 firmware */
    PT_ENTRY_MAX=16,                         /*!< Partition entry type:Max */
}PtTable_Entry_Type;

typedef struct {
    uint32_t magicCode;                     /*!< Partition table magic code */
    uint16_t version;                       /*!< Partition table verdion */
    uint16_t entryCnt;                      /*!< Partition table entry count */
    uint32_t age;                           /*!< Partition table age */
    uint32_t crc32;                         /*!< Partition table CRC32 value */
}PtTable_Config;

typedef struct {
    uint8_t type;                           /*!< Partition entry type */
    uint8_t device;                         /*!< Partition entry device */
    uint8_t activeIndex;                    /*!< Partition entry active index */
    uint8_t name[9];                        /*!< Partition entry name */
    uint32_t Address[2];                    /*!< Partition entry start address */
    uint32_t maxLen[2];                     /*!< Partition entry max length */
    uint32_t len;                           /*!< Partition entry length */
    uint32_t age;                           /*!< Partition entry age */
}PtTable_Entry_Config;

typedef struct {
    PtTable_Config ptTable;                 /*!< Partition table */
    PtTable_Entry_Config ptEntries[PT_ENTRY_MAX];    /*!< Partition entries */
    uint32_t crc32;                         /*!< Partition entries crc32 */
}PtTable_Stuff_Config;

#if defined(CONFIG_IPRO6)
/* ipro6 64K-boot2 layout (mirrors ipro7): bootheader 4K @0x0, boot2 code
 * 60K @0x1000, PT0 @0x10000, PT1 @0x11000, FW @0x12000. Must match the
 * flash tool's partition_cfg_*.toml. */
#define PT_TABLE0_ADDRESS                               0x10000
#define PT_TABLE1_ADDRESS                               0x11000
#else
/* Legacy fixed addresses. ipro7 overrides at runtime from the boot2
 * bootheader (offsets 164/168), so these are only a fallback there. */
#define PT_TABLE0_ADDRESS                               0xE000
#define PT_TABLE1_ADDRESS                               0xF000
#endif
#define PT_MAGIC_CODE                                   0x54505049
typedef int (*pPtTable_Flash_Erase)(uint32_t startaddr,uint32_t endaddr);
typedef int (*pPtTable_Flash_Write)(uint32_t addr,uint8_t *data, uint32_t len);
typedef int (*pPtTable_Flash_Read)(uint32_t addr,uint8_t *data, uint32_t len);

void PtTable_Set_Flash_Operation(pPtTable_Flash_Erase erase,pPtTable_Flash_Write write);
PtTable_ID_Type PtTable_Get_Active_Partition(const SPI_Flash_Cfg_Type *pFlashCfg,
                                             PtTable_Stuff_Config ptStuff[2]);
PtTable_Error_Type PtTable_Get_Active_Entries(PtTable_Stuff_Config *ptStuff,
PtTable_Entry_Type type,
                                              PtTable_Entry_Config *ptEntry);
PtTable_Error_Type PtTable_Update_Entry(const SPI_Flash_Cfg_Type *pFlashCfg,
PtTable_ID_Type targetTableID,
                                        PtTable_Stuff_Config *ptStuff,
PtTable_Entry_Config *ptEntry);
PtTable_Error_Type PtTable_Create(const SPI_Flash_Cfg_Type *pFlashCfg,PtTable_ID_Type ptID);
PtTable_Error_Type PtTable_Get_Active_Entries_By_Name(PtTable_Stuff_Config *ptStuff,                                                             
                                                    uint8_t *name,                                                     
                                                    PtTable_Entry_Config *ptEntry);
#endif /* __HAL_PTMGR_H__ */
