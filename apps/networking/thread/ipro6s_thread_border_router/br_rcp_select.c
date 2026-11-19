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

/*
 * Which UART RCP the border router drives (`rcp_select`).
 *
 * A board can carry more than one RCP on UART1-capable pads, e.g. an IPRO7
 * RCP on GPIO0/1 and a second RCP on GPIO2/3. The choice is kept in EasyFlash and
 * applied when the Spinel UART opens at OT start (ot_spinel_uart_get_link()
 * overrides the Kconfig pads of the OpenThread port); with nothing stored the
 * Kconfig link (CONFIG_THREAD_RCP_UART_*) is used.
 *
 * An RCP left running without its host keeps the BR's PAN ID, extended and
 * short address with RX on and keeps auto-ACKing frames meant for the BR (a
 * "ghost acker" that answers before the real RCP). So switching turns the
 * current RCP's radio off before the reboot, and on a board where a selection
 * is stored every boot also resets the other known RCPs (Spinel platform
 * reset: they come back with the radio off) before the selected link opens.
 * A board with a single RCP (nothing stored) never drives the other pads.
 */
#include <generated/autoconf.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "hal_uart.h"
#include <easyflash.h>
#include <openthread/ip6.h>
#include <openthread/thread.h>
#include "openthread_port.h"
#include "br_rcp_select.h"
#include "ipro_log.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define TAG "RCPSEL"
#define BR_RCP_KEY "br-rcp-v1"
#define BR_RCP_MAGIC "RCS1"
#define BR_RCP_BAUD_MIN 9600u
#define BR_RCP_BAUD_MAX 4000000u
/* Let the radio-off Spinel exchange reach the old RCP before the reset. */
#define BR_RCP_QUIESCE_MS 300u
#define BR_RCP_UART 1
/* Time for the reset frame to leave the UART FIFO (7 bytes, >= 9600 baud). */
#define BR_RCP_RESET_TX_MS 10u

/* Presets: IPRO7 RCP on GPIO0/1, a second ("alt") RCP on GPIO2/3,
 * both at 1 Mbaud (TX/RX from the BR's side). */
#ifndef BR_RCP_IPRO7_TX_PIN
#define BR_RCP_IPRO7_TX_PIN 1
#define BR_RCP_IPRO7_RX_PIN 0
#endif
#ifndef BR_RCP_ALT_TX_PIN
#define BR_RCP_ALT_TX_PIN 3
#define BR_RCP_ALT_RX_PIN 2
#endif
#define BR_RCP_PRESET_BAUD 1000000u

struct br_rcp_link {
    char magic[4];
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint8_t reserved[2];
    uint32_t baud;
    char name[8];
};

static const struct br_rcp_link sPresets[] = {
    {BR_RCP_MAGIC, BR_RCP_IPRO7_TX_PIN, BR_RCP_IPRO7_RX_PIN, {0}, BR_RCP_PRESET_BAUD, "ipro7"},
    {BR_RCP_MAGIC, BR_RCP_ALT_TX_PIN, BR_RCP_ALT_RX_PIN, {0}, BR_RCP_PRESET_BAUD, "alt"},
};

/* The link the Spinel UART was opened with (valid once sActiveSet). */
static struct br_rcp_link sActive;
static bool sActiveSet;

static bool link_valid(const struct br_rcp_link *link)
{
    return memcmp(link->magic, BR_RCP_MAGIC, sizeof(link->magic)) == 0 &&
           link->tx_pin < GLB_GPIO_PIN_MAX && link->rx_pin < GLB_GPIO_PIN_MAX &&
           (link->tx_pin % 8) != (link->rx_pin % 8) && link->baud >= BR_RCP_BAUD_MIN &&
           link->baud <= BR_RCP_BAUD_MAX && memchr(link->name, '\0', sizeof(link->name)) != NULL;
}

/* Name a link after the preset it matches, or "custom". */
static void link_name(struct br_rcp_link *link)
{
    for (size_t i = 0; i < sizeof(sPresets) / sizeof(sPresets[0]); i++) {
        if (sPresets[i].tx_pin == link->tx_pin && sPresets[i].rx_pin == link->rx_pin &&
            sPresets[i].baud == link->baud) {
            memcpy(link->name, sPresets[i].name, sizeof(link->name));
            return;
        }
    }
    snprintf(link->name, sizeof(link->name), "custom");
}

/* Stored selection; false (and *link untouched) when none is stored or valid. */
static bool link_load(struct br_rcp_link *link)
{
    struct br_rcp_link stored;
    size_t saved = 0;

    if (easyflash_init() != EF_NO_ERR) {
        return false;
    }
    memset(&stored, 0, sizeof(stored));
    if (ef_get_env_blob(BR_RCP_KEY, &stored, sizeof(stored), &saved) != sizeof(stored) ||
        saved != sizeof(stored) || !link_valid(&stored)) {
        return false;
    }
    *link = stored;
    return true;
}

static void link_default(struct br_rcp_link *link)
{
    memset(link, 0, sizeof(*link));
    memcpy(link->magic, BR_RCP_MAGIC, sizeof(link->magic));
    link->tx_pin = CONFIG_THREAD_RCP_UART_TX_PIN;
    link->rx_pin = CONFIG_THREAD_RCP_UART_RX_PIN;
    link->baud = CONFIG_THREAD_RCP_UART_BAUD;
    link_name(link);
}

/* HDLC-lite Spinel CMD_RESET(SPINEL_RESET_PLATFORM), TID 0, FCS 0xc271. */
static const uint8_t kSpinelResetPlatform[] = {0x7e, 0x80, 0x01, 0x01, 0x71, 0xc2, 0x7e};

static bool links_share_pad(const struct br_rcp_link *a, const struct br_rcp_link *b)
{
    return a->tx_pin == b->tx_pin || a->tx_pin == b->rx_pin || a->rx_pin == b->tx_pin ||
           a->rx_pin == b->rx_pin;
}

/* Reset every other preset RCP so none of them keeps answering as this BR
 * while the selected one takes over. Their pads go back to plain GPIO so two
 * pads never carry one UART1 signal once the Spinel UART opens. */
static void reset_other_rcps(const struct br_rcp_link *selected)
{
    for (size_t i = 0; i < sizeof(sPresets) / sizeof(sPresets[0]); i++) {
        const struct br_rcp_link *other = &sPresets[i];
        uint8_t pins[] = {other->tx_pin, other->rx_pin};

        if (links_share_pad(other, selected)) {
            continue;
        }
        GLB_GPIO_Func_Init(GPIO_FUN_UART, pins, 2);
        GLB_UART_Fun_Sel_Swap(pins[0] % 8, GLB_UART_SIG_FUN_UART1_TXD);
        GLB_UART_Fun_Sel_Swap(pins[1] % 8, GLB_UART_SIG_FUN_UART1_RXD);
        uart_set_baudrate(BR_RCP_UART, other->baud);
        uart_init(BR_RCP_UART, 0);
        uart_write(BR_RCP_UART, kSpinelResetPlatform, sizeof(kSpinelResetPlatform));
        vTaskDelay(pdMS_TO_TICKS(BR_RCP_RESET_TX_MS));
        GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pins, 2);
        IPRO_LOGI(TAG, "reset the %s RCP (TX=GPIO%u) so it cannot answer as the BR", other->name,
                  other->tx_pin);
    }
}

void ot_spinel_uart_get_link(uint8_t *tx_pin, uint8_t *rx_pin, uint32_t *baud)
{
    /* The Spinel UART asks again on every RouteUart() (after an RCP platform
     * reset or a recovery): resolve the link and reset the other RCPs once
     * per boot, then keep answering with the same link. */
    if (!sActiveSet) {
        struct br_rcp_link link;

        link_default(&link);
        if (link_load(&link)) {
            reset_other_rcps(&link);
            IPRO_LOGI(TAG, "RCP %s: TX=GPIO%u RX=GPIO%u %lu baud (stored)", link.name, link.tx_pin,
                      link.rx_pin, (unsigned long)link.baud);
        } else {
            IPRO_LOGI(TAG, "RCP %s: TX=GPIO%u RX=GPIO%u %lu baud (build default)", link.name,
                      link.tx_pin, link.rx_pin, (unsigned long)link.baud);
        }
        sActive = link;
        sActiveSet = true;
    }
    *tx_pin = sActive.tx_pin;
    *rx_pin = sActive.rx_pin;
    *baud = sActive.baud;
}

void br_rcp_active_link(uint8_t *tx_pin, uint8_t *rx_pin, uint32_t *baud)
{
    struct br_rcp_link link;

    if (sActiveSet) {
        link = sActive;
    } else {
        link_default(&link);
        (void)link_load(&link);
    }
    *tx_pin = link.tx_pin;
    *rx_pin = link.rx_pin;
    *baud = link.baud;
}

#ifdef CONFIG_SHELL

extern void hal_reboot(void);

/* Radio off on the RCP in use (Thread stop + interface down reaches the RCP as
 * a Spinel PHY disable), so it does not keep answering as the BR. */
static bool quiesce_current_rcp(void)
{
    otInstance *instance = otrGetInstanceIfReady();
    otError error = OT_ERROR_NONE;

    if (instance == NULL) {
        return false;
    }
    otrLock();
    if (otThreadGetDeviceRole(instance) != OT_DEVICE_ROLE_DISABLED) {
        error = otThreadSetEnabled(instance, false);
    }
    if (error == OT_ERROR_NONE && otIp6IsEnabled(instance)) {
        error = otIp6SetEnabled(instance, false);
    }
    otrUnlock();
    if (error != OT_ERROR_NONE) {
        IPRO_LOGW(TAG, "could not stop the current RCP (error %d)", (int)error);
        return false;
    }
    vTaskDelay(pdMS_TO_TICKS(BR_RCP_QUIESCE_MS));
    return true;
}

static void print_link(const char *what, const struct br_rcp_link *link)
{
    printf("%s: %s TX=GPIO%u RX=GPIO%u %lu baud\r\n", what, link->name, link->tx_pin, link->rx_pin,
           (unsigned long)link->baud);
}

static void cmd_rcp_select(int argc, char **argv)
{
    struct br_rcp_link want, stored;
    bool have_stored;

    link_default(&stored);
    have_stored = link_load(&stored);
    if (argc < 2) {
        struct br_rcp_link active;

        if (sActiveSet) {
            active = sActive;
        } else {
            active = stored;
        }
        print_link("active", &active);
        print_link(have_stored ? "stored" : "build default", &stored);
        for (size_t i = 0; i < sizeof(sPresets) / sizeof(sPresets[0]); i++) {
            print_link("preset", &sPresets[i]);
        }
        printf("usage: rcp_select ipro7|alt|<tx> <rx> [baud] (saves, stops the current RCP's radio, reboots)\r\n");
        return;
    }

    memset(&want, 0, sizeof(want));
    if (argc == 2) {
        size_t i;

        for (i = 0; i < sizeof(sPresets) / sizeof(sPresets[0]); i++) {
            if (strcmp(argv[1], sPresets[i].name) == 0) {
                want = sPresets[i];
                break;
            }
        }
        if (i == sizeof(sPresets) / sizeof(sPresets[0])) {
            printf("unknown RCP '%s' (ipro7, alt, or <tx> <rx> [baud])\r\n", argv[1]);
            return;
        }
    } else {
        char *end1, *end2, *end3 = NULL;
        long tx = strtol(argv[1], &end1, 10);
        long rx = strtol(argv[2], &end2, 10);
        long baud = argc > 3 ? strtol(argv[3], &end3, 10) : (long)BR_RCP_PRESET_BAUD;

        if (*argv[1] == '\0' || *end1 != '\0' || *argv[2] == '\0' || *end2 != '\0' ||
            (argc > 3 && (*argv[3] == '\0' || *end3 != '\0')) || tx < 0 || rx < 0 || baud <= 0) {
            printf("pins and baud must be numbers\r\n");
            return;
        }
        memcpy(want.magic, BR_RCP_MAGIC, sizeof(want.magic));
        want.tx_pin = (uint8_t)(tx < 255 ? tx : 255);
        want.rx_pin = (uint8_t)(rx < 255 ? rx : 255);
        want.baud = (uint32_t)baud;
        link_name(&want);
    }
    if (!link_valid(&want)) {
        printf("invalid link: GPIO 0..%d on different UART slots (pin %% 8), %u..%u baud\r\n",
               GLB_GPIO_PIN_MAX - 1, BR_RCP_BAUD_MIN, BR_RCP_BAUD_MAX);
        return;
    }
    if (sActiveSet && sActive.tx_pin == want.tx_pin && sActive.rx_pin == want.rx_pin &&
        sActive.baud == want.baud && have_stored) {
        print_link("already active", &want);
        return;
    }

    if (!quiesce_current_rcp()) {
        printf("warning: the current RCP's radio may still be on; power it off or reset it "
               "so it cannot answer as the BR\r\n");
    }
    if (ef_set_env_blob(BR_RCP_KEY, &want, sizeof(want)) != EF_NO_ERR) {
        printf("saving the selection failed; nothing changed (reboot to resume Thread)\r\n");
        return;
    }
    print_link("selected", &want);
    printf("rebooting to open the Spinel link on the new pins\r\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    hal_reboot();
}
SHELL_CMD_EXPORT_ALIAS(cmd_rcp_select, rcp_select, Select the UART RCP: ipro7 | alt | tx rx [baud]);

#endif /* CONFIG_SHELL */
