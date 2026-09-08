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

#ifndef __HAL_BOOT2_H__
#define __HAL_BOOT2_H__
#include <stdint.h>
#include <hal_ptmgr.h>
typedef int (*fptr_Flash_Erase_t)(uint32_t startaddr,uint32_t endaddr);
typedef int (*fptr_Flash_Write_t)(uint32_t addr,uint8_t *data, uint32_t len);
typedef int (*fptr_Flash_Read_t) (uint32_t addr,uint8_t *data, uint32_t len);

#define HAL_BOOT2_CPU_GROUP_MAX     1
#define HAL_BOOT2_CPU_MAX           1
#define HAL_BOOT2_RAM_IMG_COUNT_MAX 2

#define HAL_BOOT2_PK_HASH_SIZE  256 / 8

/**
 * @brief Error type definition
 */
typedef enum 
{
  HAL_SUCCESS  = 0, 
  HAL_ERROR   = 1,
} HAL_Err_Type;

struct __attribute__((packed, aligned(4))) boot_efuse_sw_cfg0_t {
    uint32_t bootrom_protect   : 1; /* ef_sw_usage_0 bit [0] */
    uint32_t uart_log_disable  : 1; /* ef_sw_usage_0 bit [1] */
    uint32_t boot_pin_cfg      : 1; /* ef_sw_usage_0 bit [2] */
    uint32_t mediaboot_disable : 1; /* ef_sw_usage_0 bit [3] */

    uint32_t uartboot_disable : 1; /* ef_sw_usage_0 bit [4] */
    uint32_t usbboot_enable   : 1; /* ef_sw_usage_0 bit [5] */
    uint32_t uart_log_reopen  : 1; /* ef_sw_usage_1 bit [6] */
    uint32_t sign_cfg         : 1; /* ef_sw_usage_0 bit [7] */

    uint32_t dcache_disable : 1; /* ef_sw_usage_0 bit [8] */
    uint32_t jtag_cfg       : 3; /* ef_sw_usage_0 bit [11:9] */

    uint32_t fix_key_sel : 1; /* ef_sw_usage_0 bit [12] */

    uint32_t sdh_en     : 1; /* ef_sw_usage_1 bit [13] */
    uint32_t sf_pin_cfg : 6; /* ef_sw_usage_0 bit [19:14] */

    uint32_t boot_pin_dly       : 2; /* ef_sw_usage_0 bit [21:20] */
    uint32_t power_trim_disable : 1; /* ef_sw_usage_0 bit [22] */
    uint32_t trim_enable        : 1; /* ef_sw_usage_0 bit [23] */

    uint32_t flash_power_delay : 2; /* ef_sw_usage_0 bit [25:24] */
    uint32_t boot_level_revert : 1; /* ef_sw_usage_0 bit [26] */
    uint32_t tz_boot           : 1; /* ef_sw_usage_0 bit [27] */

    uint32_t usb_desc_cfg         : 1; /* ef_sw_usage_0 bit [28] */
    uint32_t hbn_check_sign       : 1; /* ef_sw_usage_0 bit [29] */
    uint32_t keep_dbg_port_closed : 1; /* ef_sw_usage_0 bit [30] */
    uint32_t hbn_jump_disable     : 1; /* ef_sw_usage_0 bit [31] */
};

struct __attribute__((packed, aligned(4))) boot_efuse_sw_cfg1_t {
    uint32_t xtal_type  : 3; /* ef_sw_usage_1 bit [2:0] */
    uint32_t wifipll_pu : 1; /* ef_sw_usage_1 bit [3] */

    uint32_t aupll_pu        : 1; /* ef_sw_usage_1 bit [4] */
    uint32_t product_id      : 2; /* ef_sw_usage_1 bit [6:5] */
    uint32_t sdioboot_enable : 1; /* ef_sw_usage_1 bit [7] */

    uint32_t mcu_clk     : 3; /* ef_sw_usage_1 bit [10:8] */
    uint32_t mcu_clk_div : 1; /* ef_sw_usage_1 bit [11] */

    uint32_t mcu_pbclk_div       : 2; /* ef_sw_usage_1 bit [13:12] */
    uint32_t uart_download_cfg   : 2; /* ef_sw_usage_1 bit [15:14] */
    uint32_t pin_func_0_init     : 1; /* ef_sw_usage_1 bit [16] */
    uint32_t always_uart         : 1; /* ef_sw_usage_1 bit [17] */
    uint32_t abt_shake_hands_dis : 1; /* ef_sw_usage_1 bit [18] */
    uint32_t no_hd_boot_en       : 1; /* ef_sw_usage_1 bit [19] */

    uint32_t ocram_way_dis_cfg : 2; /* ef_sw_usage_1 bit [21:20] */
    uint32_t xtal_level_revert : 1; /* ef_sw_usage_1 bit [22] */
    uint32_t flash_clk_type    : 3; /* ef_sw_usage_1 bit [25:23] */
    uint32_t flash_clk_div     : 1; /* ef_sw_usage_1 bit [26] */
    uint32_t ldo18io_cfg_dis   : 1; /* ef_sw_usage_1 bit [27] */

    uint32_t bootlog_pin_cfg    : 1; /* ef_sw_usage_1 bit [28] */
    uint32_t abt_offset         : 1; /* ef_sw_usage_1 bit [29] */
    uint32_t boot_pull_cfg      : 1; /* ef_sw_usage_1 bit [30] */
    uint32_t usb_if_int_disable : 1; /* ef_sw_usage_1 bit [31] */
};

typedef struct
{
    uint8_t encrypted[HAL_BOOT2_CPU_GROUP_MAX];
    uint8_t sign[HAL_BOOT2_CPU_GROUP_MAX];
    uint8_t hbn_check_sign;
    uint8_t rsvd[3];
    uint8_t chip_id[8];
    uint8_t pk_hash_cpu0[HAL_BOOT2_PK_HASH_SIZE];
    uint8_t pk_hash_cpu1[HAL_BOOT2_PK_HASH_SIZE];
    uint8_t uart_download_cfg;
    uint8_t sf_pin_cfg;
    uint8_t keep_dbg_port_closed;
    uint8_t boot_pin_cfg;
    uint32_t psram_dqs_cfg;
    uint32_t dev_info;
} boot2_efuse_hw_config;


int hal_boot2_partition_bus_addr_active(const char *name, uint32_t *addr, uint32_t *size);
int hal_boot2_partition_bus_addr_inactive(const char *name, uint32_t *addr, uint32_t *size);
int hal_boot2_partition_bus_addr(const char *name, uint32_t *addr0, uint32_t *addr1, uint32_t *size0, uint32_t *size1, int *active);
int hal_boot2_partition_addr_active(const char *name, uint32_t *addr, uint32_t *size);
int hal_boot2_partition_addr_inactive(const char *name, uint32_t *addr, uint32_t *size);
int hal_boot2_partition_addr(const char *name, uint32_t *addr0, uint32_t *addr1, uint32_t *size0, uint32_t *size1, int *active);
uint8_t hal_boot2_get_active_partition(void);
int hal_boot2_get_active_entries_byname(uint8_t *name, PtTable_Entry_Config *ptEntry_hal);
int hal_boot2_get_active_entries(int type, PtTable_Entry_Config *ptEntry);
int hal_boot2_update_ptable(PtTable_Entry_Config *ptEntry);
int hal_boot2_dump(void);
uint32_t hal_boot2_psram(void);
int hal_boot2_init(void);
void hal_update_mfg_ptable(void);
unsigned int hal_boot2_get_psram_dqs_cfg(void);
void hal_boot2_init_efuse(void);
#endif
