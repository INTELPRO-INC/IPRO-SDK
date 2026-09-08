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
 * @file vbus.c
 * @brief USB-C power presence from the transceiver comparator. See vbus.h.
 */

#include "vbus.h"

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ipro7.h"
#include "drv_common.h"
#include "ipro_log.h"

#define LOG_TAG "VBUS"

/*
 * Register detail, mirrored from bsp drv_usb_v1.c rather than included from
 * it: that file is only compiled with CONFIG_COMPONENTS_CHERRYUSB_ENABLE, and
 * pulling in the whole device stack to read one comparator bit is not a
 * trade worth making. These three constants are all of it.
 */
#define GLB_USB_XCVR_OFFSET      (0x228)     /* GLB usb_xcvr            */
#define GLB_PU_USB               (1u << 23)  /* power up the transceiver */
#define USB_XCVR_IF_CONFIG       (0x1FC)     /* USB xcvr_if_config      */
#define USB_STS_VBUS_DET         (1u << 31)  /* live comparator output  */

/* The comparator is analog and needs a moment after power-up before its
 * output means anything. USB spec-level debounce is 100 ms; this only has to
 * outlast the comparator settling, and the value is small enough to sit in
 * front of a boot-time read without being noticed. */
#define VBUS_SETTLE_MS           2u

/* How often vbus_wait() looks. Fast enough that plugging the cable feels
 * immediate, slow enough to leave the CPU alone while it waits. */
#define VBUS_POLL_MS             50u

/* How often the wait says it is still waiting. A board sitting silently is
 * indistinguishable from a hung one, and this one is meant to be stared at
 * across a trade-show table. */
#define VBUS_REPORT_MS           5000u

static bool s_ready;

/* Set by `vbus skip` to release a wait that is never going to be satisfied -
 * a demo board with no cable to hand, or a supply that feeds VBAT directly.
 * Sticky: once the operator has said to go without it, later waits go too. */
static volatile bool s_skip;

void vbus_init(void)
{
    uint32_t regval;

    if (s_ready) {
        return;
    }

    /* GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB) and GLB_Set_USB_CLK() already
     * ran in BOARD_CLOCK_GATING_INIT / BOARD_PERIPHERAL_CLOCK_INIT, so the
     * block is clocked and this is the only bit left to set. */
    regval = getreg32(GLB_BASE + GLB_USB_XCVR_OFFSET);
    if (!(regval & GLB_PU_USB)) {
        putreg32(regval | GLB_PU_USB, GLB_BASE + GLB_USB_XCVR_OFFSET);
        arch_delay_ms(VBUS_SETTLE_MS);
    }

    s_ready = true;
}

bool vbus_present(void)
{
    vbus_init();
    return (getreg32(USB_BASE + USB_XCVR_IF_CONFIG) & USB_STS_VBUS_DET) != 0u;
}

bool vbus_wait(uint32_t timeout_ms)
{
    uint32_t waited = 0u;
    uint32_t since_report = 0u;

    if (vbus_present() || s_skip) {
        return true;
    }

    IPRO_LOGI(LOG_TAG, "waiting for USB-C power on CN1 - the console and BLE "
                       "stay up, and `vbus skip` starts without it");

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(VBUS_POLL_MS));

        if (vbus_present()) {
            IPRO_LOGI(LOG_TAG, "USB-C powered after %u.%us - starting",
                      (unsigned)(waited / 1000u), (unsigned)((waited % 1000u) / 100u));
            return true;
        }

        if (s_skip) {
            IPRO_LOGI(LOG_TAG, "skipped by request - starting on battery");
            return false;
        }

        waited       += VBUS_POLL_MS;
        since_report += VBUS_POLL_MS;

        if (since_report >= VBUS_REPORT_MS) {
            since_report = 0u;
            IPRO_LOGI(LOG_TAG, "still on battery after %us - plug CN1, or run "
                               "`vbus skip`", (unsigned)(waited / 1000u));
        }

        if (timeout_ms != 0u && waited >= timeout_ms) {
            IPRO_LOGW(LOG_TAG, "no USB-C power after %us - starting anyway",
                      (unsigned)(timeout_ms / 1000u));
            return false;
        }
    }
}

#ifdef CONFIG_SHELL

#include "shell.h"
#include <string.h>

int cmd_vbus(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "skip") == 0) {
        s_skip = true;
        printf("vbus  : wait released - the board will start on battery.\n");
        return 0;
    }

    if (argc > 1) {
        printf("usage: vbus         is the USB-C connector powered?\n"
               "       vbus skip    stop waiting for it and start anyway\n");
        return 0;
    }

    printf("VBUS    : %s\n", vbus_present() ? "present - running on USB-C"
                                            : "absent - running on the cell");
    if (s_skip) {
        printf("wait    : skipped by request this session\n");
    }
    printf("source  : USB transceiver comparator on the SoC's VBUS pin\n");
    printf("note    : this is connector power only. Charge state is not\n"
           "          readable - PGOOD and CHG drive LEDs D14/D15 and reach\n"
           "          no GPIO. `vbat` reads 3V3_SOC, not the cell.\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_vbus, vbus, USB-C power presence);

#endif /* CONFIG_SHELL */
