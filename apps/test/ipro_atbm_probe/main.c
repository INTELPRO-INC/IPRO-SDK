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
 * ipro_atbm_probe — ATBM6162 (Altobeam OCEANUS) Wi-Fi 6 SDIO bring-up probe.
 *
 * Phase 0/1: sets up the SDH bus pinmux + ATBM power/reset, then runs the chip
 * bring-up (SDIO enumerate -> firmware download -> handshake) via the
 * wifi_atbm6162 component's public atbm6162_bringup() entry.
 *
 * Shell:
 *   atbm_init   - SDH pinmux + clock + ATBM power-on/reset sequence
 *   atbm_probe  - atbm_init, then full chip bring-up
 *
 * SDH bus = GPIO28-33 (CMD/CLK/DAT0-3). The pinmux and the chip-enable power
 * cycle come from components/network/sdio_host_probe (board layer only:
 * CONFIG_SDIO_HOST_PROBE_BUS=n, because the vendor driver keeps its own
 * SDIO_Host_Type in atbm_os_sdio.c and a second descriptor on the same
 * controller would race it).
 * See .claude/plans/atbm6162-integration.md.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"
#include "platform_common.h"
#include "hal_boot2.h"
#include "hal_uart.h"
#include "freertos_app_hooks.h"

#include "drv_glb.h"
#include "lwip/tcpip.h"
#include "atbm6162.h"
#include "sdio_probe.h"

/* ATBM6162 carrier wiring (confirmed 2026-06-25):
 *   SDH bus GPIO28-33 (DAT1=28, DAT0=29, CLK=30, CMD=31, DAT3=32, DAT2=33)
 *   CS / chip-enable -> GPIO24  (SDIO 4-bit has no chip-select; this is the
 *   module enable line. Driven HIGH to bring the chip up before SDIO init.
 *   NOTE: enable polarity (active-high assumed) — flip if HIF register R/W fails.) */
#define ATBM_PIN_CS         GLB_GPIO_PIN_24   /* chip enable (assume active high) */

static int s_board_ready = 0;

/* ------------------------------------------------------------------ */
/* Board bring-up (SDH pinmux + ATBM chip-enable)                      */
/*                                                                     */
/* Both steps come from the shared board layer. The carrier has no     */
/* separate RESET# line, so the chip-enable IS the power control: the  */
/* "power cycle" is a low pulse on CS, shaped by reset_low_ms (the     */
/* de-asserted window) and reset_post_ms (settle).                     */
/* ------------------------------------------------------------------ */

#define ATBM_CS_LOW_MS      50   /* enable held low - clears wedged firmware */
#define ATBM_CS_SETTLE_MS   30   /* settle after re-enabling */

static void atbm_board_config(sdio_probe_config_t *cfg)
{
    sdio_probe_config_default(cfg);
    cfg->pin_power_en      = ATBM_PIN_CS;
    cfg->pin_reset_n       = SDIO_PROBE_PIN_NONE;   /* carrier has no RESET# */
    cfg->pin_wake          = SDIO_PROBE_PIN_NONE;
    cfg->power_active_high = true;                  /* assumed; flip if HIF R/W fails */
    cfg->reset_low_ms      = ATBM_CS_LOW_MS;
    cfg->reset_post_ms     = ATBM_CS_SETTLE_MS;
}

/* Power-CYCLE the chip: hold enable low to reset its SDIO/firmware state, then
 * re-enable and let it settle. Without the low pulse, re-running atbm_probe only
 * re-enumerates a chip still running the previous (possibly wedged) firmware. */
static void atbm_board_power_on(void)
{
    (void)sdio_probe_power_cycle();
}

/* ------------------------------------------------------------------ */
/* Shell commands                                                      */
/* ------------------------------------------------------------------ */

static int cmd_atbm_init(int argc, char **argv)
{
    (void)argc; (void)argv;

    sdio_probe_config_t cfg;
    atbm_board_config(&cfg);

    int rc = sdio_probe_board_init(&cfg);
    if (rc != SDIO_PROBE_OK) {
        printf("atbm: board_init failed (%d)\r\n", rc);
        return rc;
    }
    atbm_board_power_on();
    s_board_ready = 1;
    printf("atbm: SDH pinmux (GPIO28-33) + chip-enable CS=GPIO%u asserted\r\n",
           ATBM_PIN_CS);
    return 0;
}

static int cmd_atbm_probe(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc;

    /* Always re-init so the chip is power-cycled to a clean state each probe (avoids
     * carrying wedged SDIO/firmware state across A/B test runs). */
    cmd_atbm_init(0, NULL);

    printf("atbm: starting bring-up...\r\n");
    rc = atbm6162_bringup();
    if (rc == 0) {
        printf("=== ATBM6162 bring-up OK ===\r\n");
    } else {
        printf("=== ATBM6162 bring-up FAILED (%d) — check carrier wiring/power ===\r\n", rc);
    }
    return rc;
}

/* ---- Phase 2: STA mode (scan / connect) ---------------------------------- */

static int cmd_atbm_sta(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = atbm6162_sta_start();
    printf(rc == 0 ? "atbm: STA mode started\r\n"
                   : "atbm: STA start FAILED (%d)\r\n", rc);
    return rc;
}

static int cmd_atbm_scan(int argc, char **argv)
{
    (void)argc; (void)argv;
    char *buf = pvPortMalloc(4096);
    if (buf == NULL) {
        printf("atbm: scan buffer alloc failed\r\n");
        return -1;
    }
    memset(buf, 0, 4096);
    int n = atbm6162_scan(buf, 4096);
    printf("atbm: scan rc=%d\r\n--- AP list ---\r\n%s\r\n--- end ---\r\n", n, buf);
    vPortFree(buf);
    return 0;
}

static int cmd_atbm_connect(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_connect <ssid>   (OPEN AP only)\r\n");
        return -1;
    }
    int rc = atbm6162_connect_open(argv[1]);
    printf("atbm: connect(open) '%s' rc=%d\r\n", argv[1], rc);
    return rc;
}

static int cmd_atbm_wconnect(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: atbm_wconnect <ssid> <password>   (WPA2-PSK/AES)\r\n");
        return -1;
    }
    int rc = atbm6162_connect_wpa2(argv[1], argv[2]);
    printf("atbm: connect(WPA2) '%s' rc=%d\r\n", argv[1], rc);
    return rc;
}

static int cmd_atbm_status(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("atbm: %s\r\n", atbm6162_is_connected() ? "CONNECTED" : "not connected");
    return 0;
}

/* Sweep the SDH 4-bit read-capture phase, then re-run atbm_probe to bring up with it. */
static int cmd_atbm_tap(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: atbm_tap <inv 0|1> <clkdly 0-3> <datdly 0-3>, then atbm_probe\r\n");
        return -1;
    }
    atbm6162_set_rdphase(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    printf("atbm: rdphase set; now run atbm_probe\r\n");
    return 0;
}

/* Per-direction phase calibration sweep: power-cycles the chip per candidate,
 * measures reads and writes in isolation, prints a matrix + recommendation. */
static int cmd_atbm_physweep(int argc, char **argv)
{
    unsigned k = (argc >= 2) ? (unsigned)atoi(argv[1]) : 0;
    cmd_atbm_init(0, NULL);   /* pinmux + power in case this runs cold */
    return atbm6162_physweep(k, atbm_board_power_on);
}

/* Per-direction phase split: reads at the given phase, writes at the proven default. */
static int cmd_atbm_phasesplit(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_phasesplit <0|1> [inv clkdly datdly], then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    if (en && argc < 5) {
        printf("Usage: atbm_phasesplit 1 <inv 0|1> <clkdly 0-3> <datdly 0-3>\r\n");
        return -1;
    }
    atbm6162_set_phasesplit(en, en ? atoi(argv[2]) : 0,
                            en ? atoi(argv[3]) : 0, en ? atoi(argv[4]) : 0);
    return 0;
}


/* A/B the OCRAM bounce buffer for SDIO transfers, then re-run atbm_probe. */
static int cmd_atbm_bounce(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_bounce <0|1>  (OCRAM bounce buffer off/on), then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_bounce(en);
    printf("atbm: bounce %s; now run atbm_probe\r\n", en ? "ON (OCRAM)" : "OFF (direct PSRAM)");
    return 0;
}

/* Route small register transfers via CMD52 (no data phase), then re-run atbm_probe. */
static int cmd_atbm_cmd52(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_cmd52 <0|1>  (small xfers via CMD52, no data phase), then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_cmd52(en);
    printf("atbm: cmd52 %s; now run atbm_probe\r\n", en ? "ON (CMD52)" : "OFF (CMD53)");
    return 0;
}

/* Route small CMD53 transfers via PIO instead of ADMA, then re-run atbm_probe. */
static int cmd_atbm_pio(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_pio <0|1>  (small xfers via PIO, not ADMA), then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_pio(en);
    printf("atbm: pio %s; now run atbm_probe\r\n", en ? "ON (PIO)" : "OFF (ADMA)");
    return 0;
}

/* Toggle SDH High-Speed sampling (DS removes the half-clock shift), then re-run atbm_probe. */
static int cmd_atbm_hs(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_hs <0|1>  (0=Default-Speed, 1=High-Speed), then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_hs(en);
    printf("atbm: %s; now run atbm_probe\r\n", en ? "HIGH-SPEED" : "DEFAULT-SPEED");
    return 0;
}

/* Set the 4-bit bus clock in MHz (widen sample margin), then re-run atbm_probe. */
static int cmd_atbm_clk(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_clk <MHz e.g. 24 12 6 3>, then atbm_probe\r\n");
        return -1;
    }
    unsigned mhz = (unsigned)atoi(argv[1]);
    atbm6162_set_clk(mhz * 1000000u);
    printf("atbm: 4-bit clock = %u MHz; now run atbm_probe\r\n", mhz);
    return 0;
}

/* Small register transfers at 1-bit (bulk stays 4-bit), then re-run atbm_probe. */
static int cmd_atbm_rem1bit(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_rem1bit <0|1>  (small xfers at 1-bit, bulk 4-bit), then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_rem1bit(en);
    printf("atbm: small-xfer 1-bit %s; now run atbm_probe\r\n", en ? "ON" : "OFF");
    return 0;
}

/* Select the delayed sampling clock so the read taps take effect, then re-run atbm_probe. */
static int cmd_atbm_samp(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_samp <0|1>  (delayed sampling clock for read taps), then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_samp(en);
    printf("atbm: sampling-clk-sel %s; now run atbm_probe\r\n", en ? "ON (delayed)" : "OFF (fixed)");
    return 0;
}

/* On 4-bit read CRC error, re-read at 1-bit and compare (real corruption vs false CRC). */
static int cmd_atbm_crcdiag(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_crcdiag <0|1>, then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_crcdiag(en);
    printf("atbm: crc-diag %s; now run atbm_probe\r\n", en ? "ON" : "OFF");
    return 0;
}

/* Accept read data despite the false SDH CRC flag (4-bit extra-nibble bug), then atbm_probe. */
static int cmd_atbm_ignorecrc(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_ignorecrc <0|1>, then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_ignorecrc(en);
    printf("atbm: ignore-rd-crc %s; now run atbm_probe\r\n", en ? "ON" : "OFF");
    return 0;
}

/* Read-twice-verify register reads (reliable 4-bit), then atbm_probe. */
static int cmd_atbm_rdverify(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_rdverify <0|1>, then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_rdverify(en);
    printf("atbm: rd-verify %s; now run atbm_probe\r\n", en ? "ON" : "OFF");
    return 0;
}

/* All reads at 1-bit (clean), writes 4-bit — thread-safe; then atbm_probe. */
static int cmd_atbm_reads1bit(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_reads1bit <0|1>, then atbm_probe\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    atbm6162_set_reads1bit(en);
    printf("atbm: reads-1bit %s (writes stay 4-bit); now run atbm_probe\r\n", en ? "ON" : "OFF");
    return 0;
}

/* A/B: clear BLK_CNT_EN for single-block transfers, then atbm_probe. */
static int cmd_atbm_noblkcnt(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: atbm_noblkcnt <0|1>, then atbm_probe\r\n"); return -1; }
    int en = atoi(argv[1]);
    atbm6162_set_noblkcnt(en);
    printf("atbm: single-no-blkcnt %s; now run atbm_probe\r\n", en ? "ON" : "OFF");
    return 0;
}

/* Native-4-bit read-phase auto-calibration, then atbm_probe. */
static int cmd_atbm_rdcal(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: atbm_rdcal <0|1> (reads-1bit must be OFF), then atbm_probe\r\n"); return -1; }
    int en = atoi(argv[1]);
    atbm6162_set_rdcal(en);
    printf("atbm: read-phase auto-cal %s; now run atbm_probe\r\n", en ? "ON" : "OFF");
    return 0;
}

/* Native 4-bit read stress test: atbm_rdtest [n] [hexaddr]. Run AFTER atbm_probe;
 * set reads-1bit OFF (atbm_reads1bit 0) first to measure native 4-bit reads. */
static int cmd_atbm_rdtest(int argc, char **argv)
{
    unsigned n    = (argc > 1) ? (unsigned)strtoul(argv[1], NULL, 0) : 1000u;
    unsigned addr = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 0) : 0u;
    printf("atbm: read-stress n=%u addr=0x%x ...\r\n", n, addr ? addr : 0xab0016cu);
    atbm6162_rdtest(n, addr);
    return 0;
}

static int cmd_atbm_pm(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: atbm_pm <0|1>   (0 = power-save OFF / low latency, 1 = ON / low power)\r\n");
        return -1;
    }
    int en = atoi(argv[1]);
    int rc = atbm6162_set_powersave(en);
    printf("atbm: power-save %s rc=%d\r\n", en ? "ON" : "OFF", rc);
    return rc;
}

static int cmd_atbm_ip(int argc, char **argv)
{
    (void)argc; (void)argv;
    char ip[16];
    if (atbm6162_get_ip(ip, sizeof(ip)) == 0) {
        printf("atbm: IP %s\r\n", ip);
    } else {
        printf("atbm: no IP (not connected / DHCP pending)\r\n");
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_atbm_init,    atbm_init,    "SDH pinmux + ATBM power/reset");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_probe,   atbm_probe,   "Full ATBM6162 SDIO bring-up");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_sta,     atbm_sta,     "Start STA mode");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_scan,    atbm_scan,    "Scan for APs");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_connect, atbm_connect, "Connect to OPEN AP <ssid>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_wconnect,atbm_wconnect,"Connect to WPA2 AP <ssid> <pass>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_status,  atbm_status,  "Show connection status");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_tap,     atbm_tap,     "SDH 4-bit read phase <inv clkdly datdly>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_phasesplit, atbm_phasesplit, "Per-direction phase <0|1> [inv clkdly datdly]");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_physweep, atbm_physweep, "Power-cycled per-direction phase sweep [k]");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_bounce,  atbm_bounce,  "OCRAM bounce buffer <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_cmd52,   atbm_cmd52,   "Small xfers via CMD52 <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_pio,     atbm_pio,     "Small xfers via PIO <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_hs,      atbm_hs,      "SDH High-Speed sampling <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_clk,     atbm_clk,     "4-bit bus clock <MHz>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_rem1bit, atbm_rem1bit, "Small xfers at 1-bit <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_samp,    atbm_samp,    "Delayed sampling clock <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_crcdiag, atbm_crcdiag, "CRC fail re-read diag <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_ignorecrc,atbm_ignorecrc,"Accept read despite CRC <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_rdverify, atbm_rdverify, "Read-twice-verify regs <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_reads1bit,atbm_reads1bit,"All reads 1-bit, writes 4-bit <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_noblkcnt, atbm_noblkcnt, "Clear BLK_CNT_EN single-block <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_rdcal,   atbm_rdcal,   "Native 4-bit read-phase auto-cal <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_rdtest,  atbm_rdtest,  "Native 4-bit read stress [n] [hexaddr]");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_pm,      atbm_pm,      "WiFi power-save <0=off|1=on>");
SHELL_CMD_EXPORT_ALIAS(cmd_atbm_ip,      atbm_ip,      "Show DHCP IPv4 address");

/* ------------------------------------------------------------------ */
/* Boot                                                                */
/* ------------------------------------------------------------------ */

static void main_task(void *arg)
{
    (void)arg;
    printf("\r\n==== ipro_atbm_probe (ATBM6162 OCEANUS Wi-Fi6 SDIO) ====\r\n");
    printf("Bring-up:  atbm_probe\r\n");
    printf("STA flow:  atbm_sta -> atbm_scan -> atbm_connect <ssid> -> atbm_status -> atbm_ip\r\n");

    /* Start the lwIP tcpip_thread once, before any netif work. Pass NULL callback
     * (never do netif bring-up in the tcpip_init callback — core-locking deadlock). */
    tcpip_init(NULL, NULL);

    shell_init_with_task(UART0_INDEX);
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_setup_heap();
    platform_init(0);
    hal_boot2_init();

    xTaskCreate(main_task, "main", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
