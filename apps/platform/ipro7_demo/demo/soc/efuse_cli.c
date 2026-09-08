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

/**
 * @file efuse_cli.c
 * @brief eFuse read/write CLI commands for flash configuration
 *
 * Shell commands:
 *   efuse_info       — Read and display all efuse device info
 *   efuse_keyslots   — Dump the key slots and the lock word as the CPU sees them
 *   efuse_set_sf3    — Set flash_cfg=2 (SF3 external flash) [WARNING: OTP!]
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "drv_ef_ctrl.h"
#include "drv_ef_cfg.h"

static void efuse_load_r0(void)
{
    EF_Ctrl_Sw_AHB_Clk_0();
    EF_Ctrl_Load_Efuse_R0();
    for (int i = 0; i < 500; i++) {
        if (EF_Ctrl_AutoLoad_Done()) return;
        for (volatile int d = 0; d < 100; d++) {}
    }
    printf("WARNING: efuse autoload timeout\r\n");
}

/* efuse_info — read and display device info, trims, MAC */
static int cmd_efuse_info(int argc, char **argv)
{
    (void)argc; (void)argv;

    efuse_load_r0();

    printf("\r\n=== eFuse Device Info ===\r\n");

    /* Device info */
    Efuse_Device_Info_Type info;
    EF_Ctrl_Read_Device_Info(&info);

    const char *flash_str[] = {"0:SF1(internal 1MB)", "1:SF2(external)", "2:SF3(external)", "3:?", "4:?", "5:?", "6:?", "7:?"};
    const char *psram_str[] = {"0:none", "1:2MB", "2:external", "3:reserved"};
    const char *pkg_str[] = {"0:QFN32", "1:QFN40", "2:QFN48", "3:reserved"};

    printf("  flash_cfg:   %s\r\n", flash_str[info.flash_cfg & 7]);
    printf("  psram_cfg:   %s\r\n", psram_str[info.psram_cfg & 3]);
    printf("  sf_swap_cfg: %u\r\n", info.sf_swap_cfg);
    printf("  sf_reverse:  %u\r\n", info.sf_reverse);
    printf("  pkg_info:    %s\r\n", pkg_str[info.pkg_info & 3]);

    /* Chip ID */
    uint8_t id[8] = {0};
    EF_Ctrl_Read_Chip_ID(id);
    printf("  chip_id:     ");
    for (int i = 7; i >= 0; i--) printf("%02x", id[i]);
    printf("\r\n");

    /* MAC slot 0 */
    uint8_t mac[8] = {0};
    int mac_empty = EF_Ctrl_Is_MAC_Address_Slot_Empty(0, 0);
    if (!mac_empty) {
        int ret = EF_Ctrl_Read_MAC_Address_Opt(0, mac, 0);
        printf("  mac_slot0:   %02x:%02x:%02x:%02x:%02x:%02x %s\r\n",
               mac[5], mac[4], mac[3], mac[2], mac[1], mac[0],
               ret == 0 ? "ok" : "crc_err");
    } else {
        printf("  mac_slot0:   empty\r\n");
    }

    /* SW_USAGE raw values */
    uint32_t sw0, sw1;
    EF_Ctrl_Read_Sw_Usage(0, &sw0);
    EF_Ctrl_Read_Sw_Usage(1, &sw1);
    printf("  sw_usage[0]: 0x%08lx\r\n", (unsigned long)sw0);
    printf("  sw_usage[1]: 0x%08lx (dev_info in [31:22])\r\n", (unsigned long)sw1);

    /* Trims */
    const Efuse_Common_Trim_Cfg *list;
    uint32_t count = EF_Ctrl_Get_Common_Trim_List(&list);
    for (uint32_t i = 0; i < count; i++) {
        Efuse_Common_Trim_Type trim;
        EF_Ctrl_Read_Common_Trim(list[i].name, &trim, 0);
        if (trim.empty) {
            printf("  trim %-12s empty\r\n", list[i].name);
        } else {
            printf("  trim %-12s val=0x%lx en=%u\r\n",
                   list[i].name, (unsigned long)trim.value, trim.en);
        }
    }

    printf("========================\r\n\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_efuse_info, efuse_info, Read and display eFuse info);

/*
 * efuse_keyslots — read the key slots straight off the eFuse shadow registers.
 *
 * This is the CPU's own view, taken by application code with no tooling in the
 * path: whatever a read-locked slot shows here is what any firmware, and any
 * attacker with code execution, can get. Before rd_lock the key reads back; once
 * rd_lock is burned every word reads 0x00000000 and stays that way, while the
 * XIP and SEC engines keep using the key internally.
 *
 * Slot map (ipro7, XIP AES-128): 0+1 = PSA-RoT public key hash, 2 = XIP region 0
 * key, 3 = XIP region 1 key AND boot2_anti_rollback_version, 4+5 = SEC engine
 * key 0. Addresses are ef_data words, EF_DATA_BASE + offset.
 */
static int cmd_efuse_keyslots(int argc, char **argv)
{
    static const struct {
        const char *name;
        uint32_t off;
        uint32_t words;
        const char *role;
    } slots[] = {
        { "key_slot_0", 0x1C, 4, "PK hash [31:0]"   },
        { "key_slot_1", 0x2C, 4, "PK hash [63:32]"  },
        { "key_slot_2", 0x3C, 4, "XIP AES region 0" },
        { "key_slot_3", 0x4C, 4, "XIP region 1 / anti-rollback" },
        { "key_slot_4", 0x5C, 4, "SEC engine key 0" },
        { "key_slot_5", 0x6C, 4, "SEC engine key 0" },
    };
    uint32_t lock;

    (void)argc; (void)argv;
    efuse_load_r0();

    printf("\r\n=== eFuse key slots, as read by the CPU ===\r\n");
    lock = *(volatile uint32_t *)(EF_DATA_BASE + 0x7C);

    for (unsigned i = 0; i < sizeof(slots) / sizeof(slots[0]); i++) {
        int wr = 0, rd = 0;
        int any = 0;

        /* lock word 0x7C: wr_lock key_slot_N at bit 19+N, rd_lock at bit 26+N */
        wr = (lock >> (19 + i)) & 1;
        rd = (lock >> (26 + i)) & 1;

        printf("  %s @0x%02lX  ", slots[i].name, (unsigned long)slots[i].off);
        for (unsigned w = 0; w < slots[i].words; w++) {
            uint32_t v = *(volatile uint32_t *)(EF_DATA_BASE + slots[i].off + w * 4);

            any |= (v != 0);
            printf("%08lX ", (unsigned long)v);
        }
        printf(" wr_lock=%d rd_lock=%d  %s%s\r\n", wr, rd, slots[i].role,
               (rd && !any) ? "  <- read-locked, invisible to the CPU" : "");
    }

    printf("  lock word @0x7C  %08lX\r\n", (unsigned long)lock);
    printf("===========================================\r\n\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_efuse_keyslots, efuse_keyslots, Dump eFuse key slots as the CPU sees them);

/* efuse_set_sf3 — program flash_cfg=2 to switch to SF3 external flash */
static int cmd_efuse_set_sf3(int argc, char **argv)
{
    efuse_load_r0();

    /* Read current device info */
    Efuse_Device_Info_Type info;
    EF_Ctrl_Read_Device_Info(&info);

    printf("\r\nCurrent flash_cfg = %u", info.flash_cfg);
    if (info.flash_cfg == 0) printf(" (SF1 internal 1MB)");
    else if (info.flash_cfg == 2) printf(" (SF3 external - already set!)");
    printf("\r\n");

    if (info.flash_cfg == 2) {
        printf("Already SF3. Nothing to do.\r\n");
        return 0;
    }

    if (info.flash_cfg != 0) {
        printf("WARNING: flash_cfg is %u (not 0). Unexpected state.\r\n", info.flash_cfg);
        printf("eFuse bits can only go 0->1. Proceeding may set unexpected value.\r\n");
    }

    /* Safety: require explicit "confirm" argument */
    if (argc < 2 || strcmp(argv[1], "confirm") != 0) {
        printf("\r\n*** WARNING: eFuse is OTP (One-Time Programmable) ***\r\n");
        printf("*** This will PERMANENTLY set flash_cfg=2 (SF3) ***\r\n");
        printf("*** After this, the chip will ONLY boot from SF3 ***\r\n");
        printf("\r\nTo proceed, type:  efuse_set_sf3 confirm\r\n\r\n");
        return 0;
    }

    /* Program: read current sw_usage[1], OR in flash_cfg=2 at bits [28:26] */
    uint32_t sw1;
    EF_Ctrl_Read_Sw_Usage(1, &sw1);

    /* flash_cfg=2 → bit 27 set (binary 010 at bits [28:26]) */
    uint32_t new_val = sw1 | (2u << 26);

    printf("Programming sw_usage[1]: 0x%08lx -> 0x%08lx\r\n",
           (unsigned long)sw1, (unsigned long)new_val);
    printf("  flash_cfg bits [28:26]: %u -> 2 (SF3)\r\n", info.flash_cfg);

    EF_Ctrl_Write_Sw_Usage(1, new_val, 1);

    printf("DONE. Verifying...\r\n");

    /* Re-read to verify */
    efuse_load_r0();
    EF_Ctrl_Read_Device_Info(&info);
    printf("Readback flash_cfg = %u %s\r\n\r\n",
           info.flash_cfg,
           info.flash_cfg == 2 ? "(SF3 OK!)" : "(UNEXPECTED!)");

    if (info.flash_cfg == 2) {
        printf("SUCCESS! Board will now boot from SF3 external flash.\r\n");
        printf("Next steps:\r\n");
        printf("  1. Power off\r\n");
        printf("  2. Ensure SF3 flash (GPIO28-33) is connected\r\n");
        printf("  3. Flash firmware to SF3 using ipro_iot_tool_lite\r\n");
        printf("  4. Power on — board boots from SF3\r\n\r\n");
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_efuse_set_sf3, efuse_set_sf3, Set flash to SF3 external [confirm]);
