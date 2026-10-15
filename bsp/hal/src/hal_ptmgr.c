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

#include <string.h>
#include <utils_crc.h>
#include <hal_flash.h>
#include <hal_ptmgr.h>

static uint32_t pt_table_addr(const PtTable_Flash_Ops *ops,
                              PtTable_ID_Type id)
{
    uint32_t addr = (id == PT_TABLE_ID_0) ? PT_TABLE0_ADDRESS
                                          : PT_TABLE1_ADDRESS;
#if defined(CONFIG_IPRO7)
    /* ipro7 keeps the PT0/PT1 addresses in the boot2 bootheader at
     * offsets 164/168; other chips use the fixed addresses. */
    ops->read((id == PT_TABLE_ID_0) ? 164 : 168, (uint8_t *)&addr, 4);
#else
    (void)ops;
#endif
    return addr;
}

static int pt_valid(const PtTable_Flash_Ops *ops, uint32_t addr,
                    PtTable_Stuff_Config *pt)
{
    uint32_t len;
    uint32_t crc;

    if (ops->read(addr, (uint8_t *)pt, sizeof(*pt)) != SUCCESS ||
        pt->ptTable.magicCode != PT_MAGIC_CODE ||
        pt->ptTable.entryCnt == 0 || pt->ptTable.entryCnt > PT_ENTRY_MAX ||
        utils_crc32((uint8_t *)&pt->ptTable, sizeof(PtTable_Config) - 4) !=
            pt->ptTable.crc32) {
        return 0;
    }
    len = pt->ptTable.entryCnt * sizeof(PtTable_Entry_Config);
    memcpy(&crc, (uint8_t *)pt->ptEntries + len, sizeof(crc));
    return utils_crc32((uint8_t *)pt->ptEntries, len) == crc;
}

PtTable_ID_Type PtTable_Get_Active_Partition_Ops(const PtTable_Flash_Ops *ops,
                                                 PtTable_Stuff_Config ptStuff[2])
{
    int v0 = pt_valid(ops, pt_table_addr(ops, PT_TABLE_ID_0), &ptStuff[0]);
    int v1 = pt_valid(ops, pt_table_addr(ops, PT_TABLE_ID_1), &ptStuff[1]);

    if (v0 && v1) {
        return (ptStuff[0].ptTable.age >= ptStuff[1].ptTable.age)
               ? PT_TABLE_ID_0 : PT_TABLE_ID_1;
    }
    if (v0) {
        return PT_TABLE_ID_0;
    }
    return v1 ? PT_TABLE_ID_1 : PT_TABLE_ID_INVALID;
}

PtTable_Error_Type PtTable_Update_Entry_Ops(const PtTable_Flash_Ops *ops,
                                            PtTable_ID_Type targetTableID,
                                            PtTable_Stuff_Config *ptStuff,
                                            PtTable_Entry_Config *ptEntry)
{
    uint32_t i = 0;
    uint32_t writeAddr;
    uint32_t entriesLen;
    PtTable_Config *ptTable;
    PtTable_Entry_Config *ptEntries;
    uint32_t *pCrc32;

    if (ops == NULL || ptEntry == NULL || ptStuff == NULL) {
        return PT_ERROR_PARAMETER;
    }
    if (targetTableID == PT_TABLE_ID_INVALID) {
        return PT_ERROR_TABLE_NOT_VALID;
    }
    ptTable = &ptStuff->ptTable;
    ptEntries = ptStuff->ptEntries;
    writeAddr = pt_table_addr(ops, targetTableID);

    for (i = 0; i < ptTable->entryCnt; i++) {
        if (ptEntries[i].type == ptEntry->type) {
            memcpy(&ptEntries[i], ptEntry, sizeof(PtTable_Entry_Config));
            break;
        }
    }
    if (i == ptTable->entryCnt) {
        if (ptTable->entryCnt < PT_ENTRY_MAX) {
            memcpy(&ptEntries[ptTable->entryCnt], ptEntry,
                   sizeof(PtTable_Entry_Config));
            ptTable->entryCnt++;
        } else {
            return PT_ERROR_ENTRY_UPDATE_FAIL;
        }
    }

    ptTable->age++;
    ptTable->crc32 = utils_crc32((uint8_t *)ptTable, sizeof(PtTable_Config) - 4);
    entriesLen = ptTable->entryCnt * sizeof(PtTable_Entry_Config);
    pCrc32 = (uint32_t *)((uint32_t)ptEntries + entriesLen);
    *pCrc32 = utils_crc32((uint8_t *)&ptEntries[0], entriesLen);

    if (ops->erase(writeAddr, sizeof(PtTable_Stuff_Config)) != SUCCESS) {
        return PT_ERROR_FALSH_WRITE;
    }
    if (ops->write(writeAddr, (uint8_t *)ptStuff,
                   sizeof(PtTable_Stuff_Config)) != SUCCESS) {
        return PT_ERROR_FALSH_WRITE;
    }
    return PT_ERROR_SUCCESS;
}

static int hal_ops_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    return flash_read(addr, data, len);
}

static int hal_ops_erase(uint32_t addr, uint32_t len)
{
    return flash_erase(addr, len);
}

static int hal_ops_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    return flash_write(addr, data, len);
}

PtTable_Error_Type PtTable_Update_Entry(const SPI_Flash_Cfg_Type *pFlashCfg,
                                        PtTable_ID_Type targetTableID,
                                        PtTable_Stuff_Config *ptStuff,
                                        PtTable_Entry_Config *ptEntry)
{
    static const PtTable_Flash_Ops hal_ops = {
        hal_ops_read, hal_ops_erase, hal_ops_write,
    };

    (void)pFlashCfg;
    return PtTable_Update_Entry_Ops(&hal_ops, targetTableID, ptStuff, ptEntry);
}

PtTable_Error_Type PtTable_Get_Active_Entries(PtTable_Stuff_Config *ptStuff,
                                            PtTable_Entry_Type type,
                                            PtTable_Entry_Config *ptEntry)
{
    uint32_t i=0;
    
    if(ptStuff==NULL||ptEntry==NULL){
        return PT_ERROR_PARAMETER;
    }
    for (i=0; i < ptStuff->ptTable.entryCnt; i++) {
        if (ptStuff->ptEntries[i].type == type){
            memcpy(ptEntry,&ptStuff->ptEntries[i],sizeof(PtTable_Entry_Config));
            return PT_ERROR_SUCCESS;
        }
    }
    return PT_ERROR_ENTRY_NOT_FOUND;
}

PtTable_Error_Type PtTable_Get_Active_Entries_By_Name(PtTable_Stuff_Config *ptStuff,
                                                    uint8_t *name,
                                                    PtTable_Entry_Config *ptEntry)
{
    uint32_t i=0;
    uint32_t len=strlen((char *)name);

    if(ptStuff==NULL||ptEntry==NULL){
        return PT_ERROR_PARAMETER;
    }   
    for (i=0; i < ptStuff->ptTable.entryCnt; i++) {
        if (strlen((char *)ptStuff->ptEntries[i].name) == len &&
        memcmp((char *)ptStuff->ptEntries[i].name,(char *)name,len) == 0){ 
            arch_memcpy_fast(ptEntry,&ptStuff->ptEntries[i],sizeof(PtTable_Entry_Config));
            return PT_ERROR_SUCCESS;
        }   
    }   
    return PT_ERROR_ENTRY_NOT_FOUND;
}

