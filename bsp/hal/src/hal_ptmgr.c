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

PtTable_Error_Type PtTable_Update_Entry(const SPI_Flash_Cfg_Type *pFlashCfg,
                                                PtTable_ID_Type targetTableID,
                                                PtTable_Stuff_Config *ptStuff,
                                                PtTable_Entry_Config *ptEntry)
{
    uint32_t i=0;
    int ret;
    uint32_t writeAddr;
    uint32_t entriesLen;
    PtTable_Config *ptTable;
    PtTable_Entry_Config *ptEntries;
    uint32_t *pCrc32;

    if(ptEntry==NULL||ptStuff==NULL){
        return PT_ERROR_PARAMETER;
    }
    
    ptTable=&ptStuff->ptTable;
    ptEntries=ptStuff->ptEntries;
    
    if(targetTableID==PT_TABLE_ID_INVALID){
        return PT_ERROR_TABLE_NOT_VALID;
    }
    
    if(targetTableID==PT_TABLE_ID_0){
        writeAddr=PT_TABLE0_ADDRESS;
#if defined(CONFIG_IPRO7)
        /* ipro7: PT0 address lives in the boot2 bootheader at offset 164.
         * These offsets are ipro7-bootheader-specific — on ipro6 bytes
         * 164/168 fall inside the basic_cfg hash, so other chips keep the
         * fixed PT_TABLE0/1_ADDRESS. */
        flash_read(164, (uint8_t *)&writeAddr, 4);
#endif
    }else{
        writeAddr=PT_TABLE1_ADDRESS;
#if defined(CONFIG_IPRO7)
        /* ipro7: PT1 address from bootheader offset 168 (see above) */
        flash_read(168, (uint8_t *)&writeAddr, 4);
#endif
    }
    for (i=0; i < ptTable->entryCnt; i++) {
        if (ptEntries[i].type == ptEntry->type){
            memcpy(&ptEntries[i],ptEntry,sizeof(PtTable_Entry_Config));
            break;
        }
    }
    if(i==ptTable->entryCnt){
        /* Not found this entry ,add new one */
        if(ptTable->entryCnt<PT_ENTRY_MAX){
            memcpy(&ptEntries[ptTable->entryCnt],ptEntry,sizeof(PtTable_Entry_Config));
            ptTable->entryCnt++;
        }else{
            return PT_ERROR_ENTRY_UPDATE_FAIL;
        }
    }
    
    /* Prepare write back to flash */
    /* Update age */
    ptTable->age++;
    ptTable->crc32=utils_crc32((uint8_t*)ptTable,sizeof(PtTable_Config)-4);

    /* Update entries CRC */
    entriesLen=ptTable->entryCnt*sizeof(PtTable_Entry_Config);
    pCrc32=(uint32_t *)((uint32_t)ptEntries+entriesLen);
    *pCrc32=utils_crc32((uint8_t *)&ptEntries[0],entriesLen);

    /* Write back to flash */
    /* Erase flash first */
    ret = flash_erase(writeAddr, sizeof(PtTable_Stuff_Config));
    if(ret!=SUCCESS){
        //MSG_ERR("Flash Erase error\r\n");
        return PT_ERROR_FALSH_WRITE;
    }
    /* Write flash */
    ret= flash_write(writeAddr,(uint8_t *)ptStuff,sizeof(PtTable_Stuff_Config));
    if(ret!=SUCCESS){
        //MSG_ERR("Flash Write error\r\n");
        return PT_ERROR_FALSH_WRITE;
    }

    return PT_ERROR_SUCCESS;
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

