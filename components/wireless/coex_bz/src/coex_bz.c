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
 * @file coex_bz.c
 * @brief IPRO7 BLE / 802.15.4 PTA arbiter (see coex_bz.h).
 *
 * Registers (IPRO7AI register manual, sheets "glb" / "m154"):
 *   BZ_COEX_CTRL @ GLB 0x300000C0: [11:8] polarities (reset 0xF, left alone),
 *     [7:4] priority threshold, [1] wlan_en (no WLAN block: 0), [0] coex_en
 *   M154 COEX_PRIORITY @ 0x3C000180: [16] rx_on_after_rx_abort_en,
 *     [15:12] tx, [11:8] rx_unicast, [7:4] rx_sfd, [3:0] rx_idle,
 *     [29:28] abort status (read only here)
 * Only these two: the MAC's ACK-others / filter-fail interrupts must stay off
 * (the HAL does not handle them; received ACKs would be dropped).
 */
#include <generated/autoconf.h>
#include <stdio.h>

#include "coex_bz.h"

#ifndef CONFIG_COEX_BZ_TX_PRI
#define CONFIG_COEX_BZ_TX_PRI 12
#endif
#ifndef CONFIG_COEX_BZ_RX_UNICAST_PRI
#define CONFIG_COEX_BZ_RX_UNICAST_PRI 11
#endif
#ifndef CONFIG_COEX_BZ_RX_SFD_PRI
#define CONFIG_COEX_BZ_RX_SFD_PRI 8
#endif
#ifndef CONFIG_COEX_BZ_RX_IDLE_PRI
#define CONFIG_COEX_BZ_RX_IDLE_PRI 8
#endif
#if defined(CONFIG_COEX_BZ_RX_ON_AFTER_ABORT)
#define COEX_BZ_RX_ON_AFTER_ABORT true
#else
#define COEX_BZ_RX_ON_AFTER_ABORT false
#endif

#define COEX_BZ_CTRL_REG    ((volatile uint32_t *)0x300000C0u)
#define COEX_BZ_M154_PRI    ((volatile uint32_t *)0x3C000180u)
#define COEX_BZ_CTRL_EN     (1u << 0)
#define COEX_BZ_PRI_MASK    0x0001FFFFu
#define COEX_BZ_PRI_TX_MASK (0xFu << 12)

static bool s_enabled;
static uint32_t s_pri_word;
/* written back after a PDS: the GLB arbiter enable, the MAC priorities */
volatile uint32_t coex_bz_en_restores;
volatile uint32_t coex_bz_pri_restores;

static uint32_t coex_bz_pri_word(const coex_bz_pri_t *p)
{
    return (p->rx_on_after_abort ? (1u << 16) : 0u) | ((uint32_t)(p->tx & 0xFu) << 12) |
           ((uint32_t)(p->rx_unicast & 0xFu) << 8) | ((uint32_t)(p->rx_sfd & 0xFu) << 4) |
           (uint32_t)(p->rx_idle & 0xFu);
}

int coex_bz_enable(const coex_bz_pri_t *pri)
{
    static const coex_bz_pri_t def = {
        .tx = CONFIG_COEX_BZ_TX_PRI,
        .rx_unicast = CONFIG_COEX_BZ_RX_UNICAST_PRI,
        .rx_sfd = CONFIG_COEX_BZ_RX_SFD_PRI,
        .rx_idle = CONFIG_COEX_BZ_RX_IDLE_PRI,
        .rx_on_after_abort = COEX_BZ_RX_ON_AFTER_ABORT,
    };

    s_pri_word = coex_bz_pri_word((pri != NULL) ? pri : &def);
    *COEX_BZ_M154_PRI = (*COEX_BZ_M154_PRI & ~COEX_BZ_PRI_MASK) | s_pri_word;
    *COEX_BZ_CTRL_REG |= COEX_BZ_CTRL_EN;
    s_enabled = true;
    return 0;
}

void coex_bz_disable(void)
{
    *COEX_BZ_CTRL_REG &= ~COEX_BZ_CTRL_EN;
    s_enabled = false;
}

bool coex_bz_enabled(void)
{
    return s_enabled;
}

bool coex_bz_resume(void)
{
    bool wrote = false;
    uint32_t cur;

    if (!s_enabled) {
        return false;
    }
    if ((*COEX_BZ_CTRL_REG & COEX_BZ_CTRL_EN) == 0u) {
        *COEX_BZ_CTRL_REG |= COEX_BZ_CTRL_EN;
        coex_bz_en_restores++;
        wrote = true;
    }
    /* lmac154 raises the TX nibble after each arbiter abort and keeps it
     * (sticky escalation): a higher TX priority is not a lost register */
    cur = *COEX_BZ_M154_PRI;
    if ((((cur ^ s_pri_word) & COEX_BZ_PRI_MASK & ~COEX_BZ_PRI_TX_MASK) != 0u) ||
        ((cur & COEX_BZ_PRI_TX_MASK) < (s_pri_word & COEX_BZ_PRI_TX_MASK))) {
        *COEX_BZ_M154_PRI = (cur & ~COEX_BZ_PRI_MASK) | s_pri_word;
        coex_bz_pri_restores++;
        wrote = true;
    }
    return wrote;
}

#if defined(CONFIG_SHELL)
#include "shell.h"

#include <stdlib.h>
#include <string.h>

/* coex [on|off|pri <tx> <rx_unicast> <rx_sfd> <rx_idle>] */
static int cmd_coex(int argc, char **argv)
{
    if ((argc >= 2) && (strcmp(argv[1], "off") == 0)) {
        coex_bz_disable();
    } else if ((argc >= 2) && (strcmp(argv[1], "on") == 0)) {
        (void)coex_bz_enable(NULL);
    } else if ((argc >= 6) && (strcmp(argv[1], "pri") == 0)) {
        coex_bz_pri_t pri = {
            .tx = (uint8_t)strtoul(argv[2], NULL, 0),
            .rx_unicast = (uint8_t)strtoul(argv[3], NULL, 0),
            .rx_sfd = (uint8_t)strtoul(argv[4], NULL, 0),
            .rx_idle = (uint8_t)strtoul(argv[5], NULL, 0),
            .rx_on_after_abort = COEX_BZ_RX_ON_AFTER_ABORT,
        };

        (void)coex_bz_enable(&pri);
    }
    printf("coex_bz %s | BZ_COEX_CTRL %08lx M154 COEX_PRIORITY %08lx (want %05lx) | restored: enable %lu"
           " priorities %lu\r\n",
           s_enabled ? "on" : "off", (unsigned long)*COEX_BZ_CTRL_REG, (unsigned long)*COEX_BZ_M154_PRI,
           (unsigned long)s_pri_word, (unsigned long)coex_bz_en_restores, (unsigned long)coex_bz_pri_restores);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_coex, coex, BLE / 802.15.4 arbiter state);
#endif
