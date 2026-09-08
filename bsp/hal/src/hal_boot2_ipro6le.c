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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "hal_ptmgr.h"
#include "hal_flash.h"
#include "hal_boot2.h"

#include "drv_ef_cfg.h"
#include "drv_ef_ctrl.h"
#include "drv_glb.h"
#ifdef CONFIG_USE_PSRAM
#include "drv_psram.h"
#endif
#include "soc_base.h"
//#include "drv_tzc_sec.h"


#define USER_UNUSED(a) ((void)(a))

#define PARTITION_BOOT2_RAM_ADDR_ACTIVE (0x4202DC00)
#define PARTITION_HEADER_BOOT2_RAM_ADDR (0x4202DC04)
#define PARTITION_BOOT2_FLASH_HEADER    (0x4202DD14)
#define PARTITION_BOOT2_FLASH_CONFIG    (0x4202DD18)
#define PARTITION_MAGIC                 (0x54505049)
#define PARTITION_FW_PART_NAME          "FW"
#define PARTITION_FW_PART_HEADER_SIZE   (0x1000)
//TODO use header file from project
#define FW_XIP_ADDRESS                  (0x23000000)
#define PARTITION_TABLE_PARAM_ADDR      (0x21010000)

static struct {
    uint8_t partition_active_idx;
    uint8_t pad[3];
    PtTable_Stuff_Config table;
} ipro_partition_table;//XXX Dont change the name of varaible, since we refer this ipro_partition_table in linker script

boot2_efuse_hw_config g_efuse_cfg;

#define HAL_BOOT2_PSRAM_ID1_WINBOND_4MB  0x5f
#define HAL_BOOT2_PSRAM_ID2_WINBOND_32MB 0xe86

int hal_boot2_update_ptable(PtTable_Entry_Config *ptEntry_hal)
{
    int ret;
    //FIXME force covert
    PtTable_Entry_Config *ptEntry = (PtTable_Entry_Config*)ptEntry_hal;
    ptEntry->activeIndex = !ptEntry->activeIndex;
    (ptEntry->age)++;
    printf("pEntry %s active %d, age %ld\r\n", ptEntry->name, ptEntry->activeIndex, ptEntry->age);
    ret = PtTable_Update_Entry(NULL,!ipro_partition_table.partition_active_idx, &ipro_partition_table.table, ptEntry);
    return ret;
}

static void _dump_partition(void)
{
    int i;
    PtTable_Stuff_Config *part = &ipro_partition_table.table;

    USER_UNUSED(i);
    USER_UNUSED(part);

    printf("======= PtTable_Config @%p=======\r\n", part);
    printf("magicCode 0x%08X;", (unsigned int)(part->ptTable.magicCode));
    printf(" version 0x%04X;", part->ptTable.version);
    printf(" entryCnt %u;", part->ptTable.entryCnt);
    printf(" age %lu;", part->ptTable.age);
    printf(" crc32 0x%08X\r\n", (unsigned int)part->ptTable.crc32);
    if (part->ptTable.magicCode == PT_MAGIC_CODE) {
        printf("idx  type device activeIndex     name   Address[0]  Address[1]  Length[0]   Length[1]   age\r\n");
        for (i = 0; i < part->ptTable.entryCnt; i++) {
            printf("[%02d] ", i);
            printf(" %02u", part->ptEntries[i].type);
            printf("     %u", part->ptEntries[i].device);
            printf("         %u", part->ptEntries[i].activeIndex);
            printf("      %8s", part->ptEntries[i].name);
            printf("  %p", (void*)(part->ptEntries[i].Address[0]));
            printf("  %p", (void*)(part->ptEntries[i].Address[1]));
            printf("  %p", (void*)(part->ptEntries[i].maxLen[0]));
            printf("  %p", (void*)(part->ptEntries[i].maxLen[1]));
            printf("  %lu\r\n", (part->ptEntries[i].age));
        }
    }
}

uint32_t hal_boot2_get_flash_addr(void)
{
    extern uint8_t __boot2_pt_addr_src;

    return (uint32_t)(&__boot2_pt_addr_src + 4 +
                      sizeof(PtTable_Config) + sizeof(PtTable_Entry_Config) * ipro_partition_table.table.ptTable.entryCnt + 4);
}


int hal_boot2_partition_bus_addr(const char *name, uint32_t *addr0, uint32_t *addr1, uint32_t *size0, uint32_t *size1, int *active)
{
    int i;
    uint32_t addr0_t, addr1_t;

    if (PARTITION_MAGIC != ipro_partition_table.table.ptTable.magicCode) {
        return -EIO;
    }

    /*Get Target partition*/
    for (i = 0; i < ipro_partition_table.table.ptTable.entryCnt; i++) {
        if (0 == strcmp((char *)&(ipro_partition_table.table.ptEntries[i].name[0]), name)) {
            break;
        }
    }
    if (ipro_partition_table.table.ptTable.entryCnt == i) {
        return -ENOENT;
    }
    addr0_t = ipro_partition_table.table.ptEntries[i].Address[0];
    addr1_t = ipro_partition_table.table.ptEntries[i].Address[1];
    *active = ipro_partition_table.table.ptEntries[i].activeIndex;
    *size0 = ipro_partition_table.table.ptEntries[i].maxLen[0];
    *size1 = ipro_partition_table.table.ptEntries[i].maxLen[1];

    /*cal partition address*/
    for (i = 0; i < ipro_partition_table.table.ptTable.entryCnt; i++) {
        if (0 == strcmp((char *)&(ipro_partition_table.table.ptEntries[i].name[0]), PARTITION_FW_PART_NAME)) {
            break;
        }
    }
    if (ipro_partition_table.table.ptTable.entryCnt == i) {
        return -ECANCELED;
    }
    /*Make sure target partition is after FW partition*/
    if ( (addr0_t && (addr0_t < ipro_partition_table.table.ptEntries[i].Address[0])) ||
         (addr0_t && (addr0_t < ipro_partition_table.table.ptEntries[i].Address[1])) ||
         (addr1_t && (addr1_t < ipro_partition_table.table.ptEntries[i].Address[0])) ||
         (addr1_t && (addr1_t < ipro_partition_table.table.ptEntries[i].Address[1]))) {
        return -EINVAL;
    }
    if ((0 != ipro_partition_table.table.ptEntries[i].activeIndex) &&
        (1 != ipro_partition_table.table.ptEntries[i].activeIndex)) {
        return -EFAULT;
    }
    //*addr0 = addr0_t - ipro_partition_table.table.ptEntries[i].Address[ipro_partition_table.table.ptEntries[i].activeIndex] - PARTITION_FW_PART_HEADER_SIZE + FW_XIP_ADDRESS;
    //*addr1 = addr1_t - ipro_partition_table.table.ptEntries[i].Address[ipro_partition_table.table.ptEntries[i].activeIndex] - PARTITION_FW_PART_HEADER_SIZE + FW_XIP_ADDRESS;
    // Use flash image offset to suport firmware booting from either FW entry or mfg entry
    *addr0 = addr0_t - SF_Ctrl_Get_Flash_Image_Offset(0, 0) + FW_XIP_ADDRESS;
    *addr1 = addr1_t - SF_Ctrl_Get_Flash_Image_Offset(0, 0) + FW_XIP_ADDRESS;

    return 0;
}

int hal_boot2_partition_bus_addr_active(const char *name, uint32_t *addr, uint32_t *size)
{
    uint32_t addr0, addr1;
    uint32_t size0, size1;
    int active, ret;

    if ((ret = hal_boot2_partition_bus_addr(name, &addr0, &addr1, &size0, &size1, &active))) {
        return ret;
    }
    *addr = active ? addr1 : addr0;
    *size = active ? size1 : size0;

    return 0;
}

int hal_boot2_partition_bus_addr_inactive(const char *name, uint32_t *addr, uint32_t *size)
{
    uint32_t addr0, addr1;
    uint32_t size0, size1;
    int active, ret;

    if ((ret = hal_boot2_partition_bus_addr(name, &addr0, &addr1, &size0, &size1, &active))) {
        return ret;
    }
    *addr = active ? addr0 : addr1;
    *size = active ? size0 : size1;

    return 0;
}

int hal_boot2_partition_addr(const char *name, uint32_t *addr0, uint32_t *addr1, uint32_t *size0, uint32_t *size1, int *active)
{
    int i;

    if (PARTITION_MAGIC != ipro_partition_table.table.ptTable.magicCode) {
        return -EIO;
    }

    /*Get Target partition*/
    for (i = 0; i < ipro_partition_table.table.ptTable.entryCnt; i++) {
        if (0 == strcmp((char *)&(ipro_partition_table.table.ptEntries[i].name[0]), name)) {
            break;
        }
    }
    if (ipro_partition_table.table.ptTable.entryCnt == i) {
        return -ENOENT;
    }
    *addr0 = ipro_partition_table.table.ptEntries[i].Address[0];
    *addr1 = ipro_partition_table.table.ptEntries[i].Address[1];
    *size0 = ipro_partition_table.table.ptEntries[i].maxLen[0];
    *size1 = ipro_partition_table.table.ptEntries[i].maxLen[1];
    *active = ipro_partition_table.table.ptEntries[i].activeIndex;

    return 0;
}

int hal_boot2_partition_addr_active(const char *name, uint32_t *addr, uint32_t *size)
{
    uint32_t addr0, addr1;
    uint32_t size0, size1;
    int active, ret;

    if ((ret = hal_boot2_partition_addr(name, &addr0, &addr1, &size0, &size1, &active))) {
        return ret;
    }
    *addr = active ? addr1 : addr0;
    *size = active ? size1 : size0;

    return 0;
}

int hal_boot2_partition_addr_inactive(const char *name, uint32_t *addr, uint32_t *size)
{
    uint32_t addr0, addr1;
    uint32_t size0, size1;
    int active, ret;

    if ((ret = hal_boot2_partition_addr(name, &addr0, &addr1, &size0, &size1, &active))) {
        return ret;
    }
    *addr = active ? addr0 : addr1;
    *size = active ? size0 : size1;

    return 0;
}

uint8_t hal_boot2_get_active_partition(void)
{
    return ipro_partition_table.partition_active_idx;
}

int hal_boot2_get_active_entries_byname(uint8_t *name, PtTable_Entry_Config *ptEntry_hal)
{
    PtTable_Entry_Config *ptEntry = (PtTable_Entry_Config*)ptEntry_hal;
    if (PtTable_Get_Active_Entries_By_Name(&ipro_partition_table.table, name, ptEntry)) {
        return -1;
    }
    return 0;
}

int hal_boot2_get_active_entries(int type, PtTable_Entry_Config *ptEntry_hal)
{
    PtTable_Entry_Config *ptEntry = (PtTable_Entry_Config*)ptEntry_hal;
    if (PtTable_Get_Active_Entries(&ipro_partition_table.table, type, ptEntry)) {
        return -1;
    }
    return 0;
}

int hal_boot2_dump(void)
{
    _dump_partition();
    return 0;
}

static void showFlashInfo(void)
{
    SPI_Flash_Cfg_Type flashCfg;
    uint8_t *pFlashCfg = NULL;
    uint32_t flashCfgLen = 0;
    uint32_t flashJedecId = 0;

    flashJedecId = flash_get_jedecid();
    flash_get_cfg(&pFlashCfg, &flashCfgLen);
    arch_memcpy((void *)&flashCfg, pFlashCfg, flashCfgLen);
    printf("show flash cfg:\r\n");
    printf("jedec id   0x%06lX\r\n", flashJedecId);
    printf("mid            0x%02X\r\n", flashCfg.mid);
    printf("iomode         0x%02X\r\n", flashCfg.ioMode);
    printf("clk delay      0x%02X\r\n", flashCfg.clkDelay);
    printf("clk invert     0x%02X\r\n", flashCfg.clkInvert);
    printf("read reg cmd0  0x%02X\r\n", flashCfg.readRegCmd[0]);
    printf("read reg cmd1  0x%02X\r\n", flashCfg.readRegCmd[1]);
    printf("write reg cmd0 0x%02X\r\n", flashCfg.writeRegCmd[0]);
    printf("write reg cmd1 0x%02X\r\n", flashCfg.writeRegCmd[1]);
    printf("qe write len   0x%02X\r\n", flashCfg.qeWriteRegLen);
    printf("cread support  0x%02X\r\n", flashCfg.cReadSupport);
    printf("cread code     0x%02X\r\n", flashCfg.cReadMode);
    printf("burst wrap cmd 0x%02X\r\n", flashCfg.burstWrapCmd);
    printf("-------------------\r\n");
}

void hal_boot2_get_efuse_cfg(boot2_efuse_hw_config *efuse_cfg)
{
    uint32_t timeout_cnt = 0;
    uint32_t i = 0;
    struct boot_efuse_sw_cfg0_t sw_cfg0;
    struct boot_efuse_sw_cfg1_t sw_cfg1;

    /* #define EF_CTRL_LOAD_BEFORE_READ_R0 in ef_ctrl to speed up */
    EF_Ctrl_Load_Efuse_R0();

    while (EF_Ctrl_AutoLoad_Done() != SET && timeout_cnt < 80) {
        arch_delay_us(1);
        timeout_cnt++;
    }

    /* get hw cfg (signature and aes type) */
    {
        uint8_t encrypted_tmp[2] = {0};
        EF_Ctrl_Read_Secure_Boot(encrypted_tmp);
        efuse_cfg->encrypted[0] = encrypted_tmp[0];
    }

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        if (efuse_cfg->encrypted[i] == EF_CTRL_SF_AES_192) {
            efuse_cfg->encrypted[i] = SF_CTRL_AES_192BITS + 1;
        } else if (efuse_cfg->encrypted[i] == EF_CTRL_SF_AES_256) {
            efuse_cfg->encrypted[i] = SF_CTRL_AES_256BITS + 1;
        }
    }

    /* get sw uasge 0 */
    EF_Ctrl_Read_Sw_Usage(0, (uint32_t *)&sw_cfg0);
    /* get sw uasge 1 */
    EF_Ctrl_Read_Sw_Usage(1, (uint32_t *)&sw_cfg1);

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        efuse_cfg->sign[i] = ((struct boot_efuse_sw_cfg0_t)sw_cfg0).sign_cfg;
    }
    for (i = 1; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        efuse_cfg->encrypted[i] = EF_CTRL_SF_AES_NONE;
    }

    efuse_cfg->hbn_check_sign = (uint8_t)(sw_cfg0.hbn_check_sign);
    efuse_cfg->sf_pin_cfg = (uint8_t)(sw_cfg0.sf_pin_cfg);
    efuse_cfg->uart_download_cfg = (uint8_t)(sw_cfg1.uart_download_cfg);
    efuse_cfg->keep_dbg_port_closed = (uint8_t)(sw_cfg0.keep_dbg_port_closed);
    efuse_cfg->boot_pin_cfg = (uint8_t)(sw_cfg0.boot_pin_cfg);

    // /* get psram dqs delay info */
    // EF_Ctrl_Read_Psram_Trim((Efuse_Psram_Trim_Type *)&efuse_cfg->psram_dqs_cfg);

    Efuse_Common_Trim_Type trim;
    efuse_cfg->psram_dqs_cfg = 0xffff;
    EF_Ctrl_Read_Common_Trim("psram_trim", &trim, 1);
    if (trim.en) {
        if(trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value,trim.len)){
            efuse_cfg->psram_dqs_cfg = trim.value;
        }
    }

    /* get device info */
    EF_Ctrl_Read_Device_Info((Efuse_Device_Info_Type *)&efuse_cfg->dev_info);

    /* get chip id */
    EF_Ctrl_Read_Chip_ID(efuse_cfg->chip_id);

    /* get public key hash */
    EF_Ctrl_Read_AES_Key(0, (uint32_t *)efuse_cfg->pk_hash_cpu0, HAL_BOOT2_PK_HASH_SIZE / 4);
    //EF_Ctrl_Read_AES_Key(8, (uint32_t *)efuse_cfg->pk_hash_cpu1, HAL_EFUSE_PK_HASH_SIZE / 4);
}

void hal_boot2_init_efuse(void)
{
    hal_boot2_get_efuse_cfg(&g_efuse_cfg);
}

int hal_boot2_init(void)
{

    hal_boot2_get_efuse_cfg(&g_efuse_cfg);

    memcpy(&ipro_partition_table, (void *)PARTITION_TABLE_PARAM_ADDR+4, sizeof(ipro_partition_table));   
    printf("Active Partition[%u], Size %d Bytes\r\n",
            ipro_partition_table.partition_active_idx,
            sizeof(PtTable_Config) + sizeof(PtTable_Entry_Config) * ipro_partition_table.table.ptTable.entryCnt + 4
    );
    _dump_partition();
#if 0
    hal_flash_config_update(PARTITION_TABLE_PARAM_ADDR + (*(uint32_t *)PARTITION_TABLE_PARAM_ADDR));
#else
    if (flash_init() != SUCCESS) {
        printf("flash init fail!!!\r\n");
    }

    showFlashInfo();
#endif

    return 0;
}
