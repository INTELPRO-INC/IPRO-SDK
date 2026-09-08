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

#include <generated/autoconf.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "hal_ptmgr.h"
#include "hal_flash.h"
#include "hal_boot2.h"
#include "hal_psram.h"
#include "hal_sys.h"

#include "drv_ef_cfg.h"
#include "drv_ef_ctrl.h"
#include "drv_glb.h"
#include "drv_psram.h"
#include "soc_base.h"
#include "boot2_param.h"
#include "compiler/common.h"
#include "utils_crc.h"
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
#define PARTITION_TABLE0_FLASH_FALLBACK (0x00001000)
#define PARTITION_TABLE1_FLASH_FALLBACK (0x00002000)

static struct {
    uint8_t partition_active_idx;
    uint8_t pad[3];
    PtTable_Stuff_Config table;
} ipro_partition_table;//XXX Dont change the name of varaible, since we refer this ipro_partition_table in linker script

/* g_efuse_cfg is accessed in hal_boot2_init_efuse() which is called from SystemInit
 * before PSRAM initialization, so it must be in OCRAM */
ATTR_OCRAM_BSS_SECTION boot2_efuse_hw_config g_efuse_cfg;

#define HAL_BOOT2_PSRAM_ID1_WINBOND_8MB 0xc86

/* Print PSRAM calibration log saved during early boot (before UART init) */
#define HAL_BOOT2_PRINT_PSRAM_CALIBRATION_LOG   0

uint32_t hal_boot2_psram(void);

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

static int hal_boot2_is_ptable_valid(const PtTable_Stuff_Config *pt)
{
    uint32_t entries_len;
    uint32_t table_crc;
    uint32_t entries_crc;

    if (pt == NULL) {
        return 0;
    }

    if (pt->ptTable.magicCode != PT_MAGIC_CODE ||
        pt->ptTable.entryCnt == 0 ||
        pt->ptTable.entryCnt > PT_ENTRY_MAX) {
        return 0;
    }

    table_crc = utils_crc32((uint8_t *)&pt->ptTable, sizeof(PtTable_Config) - 4);
    if (table_crc != pt->ptTable.crc32) {
        return 0;
    }

    entries_len = pt->ptTable.entryCnt * sizeof(PtTable_Entry_Config);
    entries_crc = utils_crc32((uint8_t *)&pt->ptEntries[0], entries_len);
    if (entries_crc != pt->crc32) {
        return 0;
    }

    return 1;
}

static int hal_boot2_read_ptable_from_flash(uint32_t addr, PtTable_Stuff_Config *pt)
{
    uint32_t entries_len;
    uint32_t entries_crc;

    if (pt == NULL) {
        return -EINVAL;
    }

    memset(pt, 0, sizeof(*pt));

    if (flash_read(addr, (uint8_t *)&pt->ptTable, sizeof(PtTable_Config)) != SUCCESS) {
        return -EIO;
    }

    if (pt->ptTable.magicCode != PT_MAGIC_CODE ||
        pt->ptTable.entryCnt == 0 ||
        pt->ptTable.entryCnt > PT_ENTRY_MAX) {
        return -EINVAL;
    }

    if (utils_crc32((uint8_t *)&pt->ptTable, sizeof(PtTable_Config) - 4) !=
        pt->ptTable.crc32) {
        return -EIO;
    }

    entries_len = pt->ptTable.entryCnt * sizeof(PtTable_Entry_Config);
    if (flash_read(addr + sizeof(PtTable_Config),
                   (uint8_t *)&pt->ptEntries[0],
                   entries_len) != SUCCESS) {
        return -EIO;
    }

    if (flash_read(addr + sizeof(PtTable_Config) + entries_len,
                   (uint8_t *)&entries_crc,
                   sizeof(entries_crc)) != SUCCESS) {
        return -EIO;
    }

    pt->crc32 = entries_crc;
    if (!hal_boot2_is_ptable_valid(pt)) {
        return -EIO;
    }

    return 0;
}

static int hal_boot2_flash_pt_addr_valid(uint32_t addr)
{
    uint32_t flash_size = flash_get_size();

    return addr != 0 &&
           addr != 0xffffffffu &&
           addr < flash_size &&
           (addr + sizeof(PtTable_Config) + sizeof(uint32_t)) <= flash_size;
}

static int hal_boot2_load_ptable_from_flash(void)
{
    PtTable_Stuff_Config pt0;
    PtTable_Stuff_Config pt1;
    uint32_t pt0_addr = PARTITION_TABLE0_FLASH_FALLBACK;
    uint32_t pt1_addr = PARTITION_TABLE1_FLASH_FALLBACK;
    int valid0 = 0;
    int valid1 = 0;

    (void)flash_read(164, (uint8_t *)&pt0_addr, sizeof(pt0_addr));
    (void)flash_read(168, (uint8_t *)&pt1_addr, sizeof(pt1_addr));

    if (!hal_boot2_flash_pt_addr_valid(pt0_addr) ||
        !hal_boot2_flash_pt_addr_valid(pt1_addr) ||
        pt0_addr == pt1_addr) {
        pt0_addr = PARTITION_TABLE0_FLASH_FALLBACK;
        pt1_addr = PARTITION_TABLE1_FLASH_FALLBACK;
    }

    valid0 = (hal_boot2_read_ptable_from_flash(pt0_addr, &pt0) == 0);
    valid1 = (hal_boot2_read_ptable_from_flash(pt1_addr, &pt1) == 0);

    if (!valid0 && !valid1) {
        printf("Warning: flash partition table unavailable (pt0=0x%08lx pt1=0x%08lx)\r\n",
               (unsigned long)pt0_addr,
               (unsigned long)pt1_addr);
        return -EIO;
    }

    if (valid1 && (!valid0 || pt1.ptTable.age >= pt0.ptTable.age)) {
        ipro_partition_table.partition_active_idx = PT_TABLE_ID_1;
        memcpy(&ipro_partition_table.table, &pt1, sizeof(ipro_partition_table.table));
        printf("Active Partition[%u] loaded from flash PT1 @0x%08lx, entryCnt %u\r\n",
               ipro_partition_table.partition_active_idx,
               (unsigned long)pt1_addr,
               ipro_partition_table.table.ptTable.entryCnt);
    } else {
        ipro_partition_table.partition_active_idx = PT_TABLE_ID_0;
        memcpy(&ipro_partition_table.table, &pt0, sizeof(ipro_partition_table.table));
        printf("Active Partition[%u] loaded from flash PT0 @0x%08lx, entryCnt %u\r\n",
               ipro_partition_table.partition_active_idx,
               (unsigned long)pt0_addr,
               ipro_partition_table.table.ptTable.entryCnt);
    }

    _dump_partition();
    return 0;
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
    EF_Ctrl_Read_Secure_Boot(efuse_cfg->sign, efuse_cfg->encrypted);

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
    Boot2_Param_Type *boot2_param = Boot2_Param_Get();

#if defined(CONFIG_IPRO_BOOT2_PARAM_DIAG)
    Boot2_Param_Debug_Dump();
#endif

#if defined(CONFIG_USE_PSRAM) && HAL_BOOT2_PRINT_PSRAM_CALIBRATION_LOG
    hal_psram_print_calibration_log();
#endif

    /* Load partition table from boot2 parameters */
    if (boot2_param != NULL) {
        /* Copy active ID */
        ipro_partition_table.partition_active_idx = (uint8_t)boot2_param->activeID;

        /* Copy partition table config */
        ipro_partition_table.table.ptTable.magicCode = boot2_param->ptTable.magicCode;
        ipro_partition_table.table.ptTable.version = boot2_param->ptTable.version;
        ipro_partition_table.table.ptTable.entryCnt = boot2_param->ptTable.entryCnt;
        ipro_partition_table.table.ptTable.age = boot2_param->ptTable.age;
        ipro_partition_table.table.ptTable.crc32 = boot2_param->ptTable.crc32;

        /* Copy partition entries */
        uint16_t entryCnt = boot2_param->ptTable.entryCnt;
        if (entryCnt > PT_ENTRY_MAX) {
            entryCnt = PT_ENTRY_MAX;
        }
        for (uint16_t i = 0; i < entryCnt; i++) {
            ipro_partition_table.table.ptEntries[i].type = boot2_param->ptEntries[i].type;
            ipro_partition_table.table.ptEntries[i].device = boot2_param->ptEntries[i].device;
            ipro_partition_table.table.ptEntries[i].activeIndex = boot2_param->ptEntries[i].activeIndex;
            memcpy(ipro_partition_table.table.ptEntries[i].name, boot2_param->ptEntries[i].name, 9);
            ipro_partition_table.table.ptEntries[i].Address[0] = boot2_param->ptEntries[i].Address[0];
            ipro_partition_table.table.ptEntries[i].Address[1] = boot2_param->ptEntries[i].Address[1];
            ipro_partition_table.table.ptEntries[i].maxLen[0] = boot2_param->ptEntries[i].maxLen[0];
            ipro_partition_table.table.ptEntries[i].maxLen[1] = boot2_param->ptEntries[i].maxLen[1];
            ipro_partition_table.table.ptEntries[i].len = boot2_param->ptEntries[i].len;
            ipro_partition_table.table.ptEntries[i].age = boot2_param->ptEntries[i].age;
        }

        /* Copy entries CRC32 */
        ipro_partition_table.table.crc32 = boot2_param->ptEntriesCrc32;

        printf("Active Partition[%u], entryCnt %u\r\n",
               ipro_partition_table.partition_active_idx,
               ipro_partition_table.table.ptTable.entryCnt);
        _dump_partition();
    } else {
        printf("Warning: boot2 parameters not available\r\n");
    }

    /* Initialize flash - bootrom already configured flash, but we need to
     * update g_flash_cfg for SDK usage. flash_init will detect and configure. */
    if (flash_init() != SUCCESS) {
        printf("flash init fail!!!\r\n");
    }

    if (!hal_boot2_is_ptable_valid(&ipro_partition_table.table)) {
        (void)hal_boot2_load_ptable_from_flash();
    }

    showFlashInfo();

    /* Reset reason was detected earlier in SystemInit (system_soc.c) so
     * main() sees a valid value before this point.  Avoid double-detect
     * which would clobber the cached g_rst_reason. */
#if 0
    /* Compare boot2 flash config with detected flash config */
    SPI_Flash_Cfg_Type *b2 = Boot2_Param_Get_Flash_Cfg();
    if (b2 != NULL) {
        uint8_t *pFlashCfg = NULL;
        uint32_t flashCfgLen = 0;
        flash_get_cfg(&pFlashCfg, &flashCfgLen);
        SPI_Flash_Cfg_Type *dt = (SPI_Flash_Cfg_Type *)pFlashCfg;
        int diff = 0;

        printf("=== Flash Config Comparison (boot2 vs detected) ===\r\n");
#define CMP_FIELD(field, fmt) \
        if (b2->field != dt->field) { \
            printf("  " #field ": boot2=" fmt ", detected=" fmt "\r\n", b2->field, dt->field); \
            diff++; \
        }
#define CMP_FIELD_HEX(field) CMP_FIELD(field, "0x%02x")
#define CMP_FIELD_DEC(field) CMP_FIELD(field, "%u")

        CMP_FIELD_HEX(ioMode);
        CMP_FIELD_HEX(cReadSupport);
        CMP_FIELD_HEX(clkDelay);
        CMP_FIELD_HEX(clkInvert);
        CMP_FIELD_HEX(resetEnCmd);
        CMP_FIELD_HEX(resetCmd);
        CMP_FIELD_HEX(resetCreadCmd);
        CMP_FIELD_DEC(resetCreadCmdSize);
        CMP_FIELD_HEX(jedecIdCmd);
        CMP_FIELD_DEC(jedecIdCmdDmyClk);
        CMP_FIELD_HEX(qpiJedecIdCmd);
        CMP_FIELD_DEC(qpiJedecIdCmdDmyClk);
        CMP_FIELD_DEC(sectorSize);
        CMP_FIELD_HEX(mid);
        CMP_FIELD_DEC(pageSize);
        CMP_FIELD_HEX(chipEraseCmd);
        CMP_FIELD_HEX(sectorEraseCmd);
        CMP_FIELD_HEX(blk32EraseCmd);
        CMP_FIELD_HEX(blk64EraseCmd);
        CMP_FIELD_HEX(writeEnableCmd);
        CMP_FIELD_HEX(pageProgramCmd);
        CMP_FIELD_HEX(qpageProgramCmd);
        CMP_FIELD_HEX(qppAddrMode);
        CMP_FIELD_HEX(fastReadCmd);
        CMP_FIELD_DEC(frDmyClk);
        CMP_FIELD_HEX(qpiFastReadCmd);
        CMP_FIELD_DEC(qpiFrDmyClk);
        CMP_FIELD_HEX(fastReadDoCmd);
        CMP_FIELD_DEC(frDoDmyClk);
        CMP_FIELD_HEX(fastReadDioCmd);
        CMP_FIELD_DEC(frDioDmyClk);
        CMP_FIELD_HEX(fastReadQoCmd);
        CMP_FIELD_DEC(frQoDmyClk);
        CMP_FIELD_HEX(fastReadQioCmd);
        CMP_FIELD_DEC(frQioDmyClk);
        CMP_FIELD_HEX(qpiFastReadQioCmd);
        CMP_FIELD_DEC(qpiFrQioDmyClk);
        CMP_FIELD_HEX(qpiPageProgramCmd);
        CMP_FIELD_HEX(writeVregEnableCmd);
        CMP_FIELD_DEC(wrEnableIndex);
        CMP_FIELD_DEC(qeIndex);
        CMP_FIELD_DEC(busyIndex);
        CMP_FIELD_DEC(wrEnableBit);
        CMP_FIELD_DEC(qeBit);
        CMP_FIELD_DEC(busyBit);
        CMP_FIELD_DEC(wrEnableWriteRegLen);
        CMP_FIELD_DEC(wrEnableReadRegLen);
        CMP_FIELD_DEC(qeWriteRegLen);
        CMP_FIELD_DEC(qeReadRegLen);
        CMP_FIELD_HEX(releasePowerDown);
        CMP_FIELD_DEC(busyReadRegLen);
        /* readRegCmd[4] */
        for (int i = 0; i < 4; i++) {
            if (b2->readRegCmd[i] != dt->readRegCmd[i]) {
                printf("  readRegCmd[%d]: boot2=0x%02x, detected=0x%02x\r\n", i, b2->readRegCmd[i], dt->readRegCmd[i]);
                diff++;
            }
        }
        /* writeRegCmd[4] */
        for (int i = 0; i < 4; i++) {
            if (b2->writeRegCmd[i] != dt->writeRegCmd[i]) {
                printf("  writeRegCmd[%d]: boot2=0x%02x, detected=0x%02x\r\n", i, b2->writeRegCmd[i], dt->writeRegCmd[i]);
                diff++;
            }
        }
        CMP_FIELD_HEX(enterQpi);
        CMP_FIELD_HEX(exitQpi);
        CMP_FIELD_HEX(cReadMode);
        CMP_FIELD_HEX(cRExit);
        CMP_FIELD_HEX(burstWrapCmd);
        CMP_FIELD_DEC(burstWrapCmdDmyClk);
        CMP_FIELD_HEX(burstWrapDataMode);
        CMP_FIELD_HEX(burstWrapData);
        CMP_FIELD_HEX(deBurstWrapCmd);
        CMP_FIELD_DEC(deBurstWrapCmdDmyClk);
        CMP_FIELD_HEX(deBurstWrapDataMode);
        CMP_FIELD_HEX(deBurstWrapData);
        CMP_FIELD_DEC(timeEsector);
        CMP_FIELD_DEC(timeE32k);
        CMP_FIELD_DEC(timeE64k);
        CMP_FIELD_DEC(timePagePgm);
        CMP_FIELD_DEC(timeCe);
        CMP_FIELD_DEC(pdDelay);
        CMP_FIELD_HEX(qeData);

#undef CMP_FIELD
#undef CMP_FIELD_HEX
#undef CMP_FIELD_DEC

        if (diff == 0) {
            printf("  Flash configs are identical\r\n");
        } else {
            printf("  Total differences: %d fields\r\n", diff);
        }
        printf("0x30000074 = %lx\r\n", readl(0x30000074));
        printf("0x30000078 = %lx\r\n", readl(0x30000078));
        printf("0x3000007c = %lx\r\n", readl(0x3000007c));
        printf("===================================================\r\n");
    }
#endif
    /* PSRAM initialization moved to _premain_init() in system_soc.c */
    /* This allows PSRAM sections to be loaded after PSRAM hardware is initialized */
    hal_rc32k_cal();
    hal_rc32m_cal();
    return 0;
}

unsigned int hal_boot2_get_psram_dqs_cfg(void)
{
    return g_efuse_cfg.psram_dqs_cfg;
}

uint32_t ATTR_TCM_SECTION hal_boot2_psram(void)
{
#ifdef CONFIG_USE_PSRAM
    return hal_psram_init(g_efuse_cfg.psram_dqs_cfg);
#else
    return 0;
#endif
}
