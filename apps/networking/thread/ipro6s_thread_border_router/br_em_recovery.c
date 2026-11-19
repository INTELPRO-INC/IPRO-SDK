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
 * @file br_em_recovery.c
 * @brief Bounded software-reboot retry for the BLE EM self-test failure.
 */
#include <stdio.h>
#include "br_em_recovery.h"
#include "hal_hbnram_mgnt.h"
#include "hal_sys.h"
#include "drv_common.h"

/* reserved[2] layout: upper 16 bits tag, lower 16 bits consecutive failures.
 * The tag guards against stale HBN RAM content that survived a warm reset
 * from an image that never wrote this word. */
#define EM_RECOVERY_SLOT   2
#define EM_RECOVERY_TAG    0x454D0000u   /* 'EM' */
#define EM_RECOVERY_MASK   0xFFFF0000u

static uint32_t read_count(void)
{
    uint32_t word = hal_hbnram_sysinfo->reserved[EM_RECOVERY_SLOT];
    return ((word & EM_RECOVERY_MASK) == EM_RECOVERY_TAG) ? (word & 0xFFFFu) : 0u;
}

static void write_count(uint32_t count)
{
    hal_hbnram_sysinfo->reserved[EM_RECOVERY_SLOT] = EM_RECOVERY_TAG | (count & 0xFFFFu);
}

uint32_t br_em_recovery_count(void)
{
    return read_count();
}

void br_em_recovery_on_success(void)
{
    uint32_t spent = read_count();
    if (spent) {
        printf("[BR-EM-RECOVERY] controller up after %lu software reboot(s)\r\n", (unsigned long)spent);
    }
    write_count(0);
}

uint32_t br_em_recovery_on_failure(int rc)
{
    uint32_t spent = read_count();
    if (spent >= BR_EM_RECOVERY_MAX_REBOOTS) {
        printf("[BR-EM-RECOVERY] btdm_init rc=%d; %lu reboots exhausted, halting for post-mortem\r\n",
               rc, (unsigned long)spent);
        return spent;
    }
    write_count(spent + 1u);
    printf("[BR-EM-RECOVERY] btdm_init rc=%d; software reboot %lu/%u\r\n",
           rc, (unsigned long)(spent + 1u), (unsigned)BR_EM_RECOVERY_MAX_REBOOTS);
    /* Let the UART drain so the reboot reason is visible on the console. */
    arch_delay_ms(200);
    hal_reboot();
    return spent + 1u; /* not reached */
}
