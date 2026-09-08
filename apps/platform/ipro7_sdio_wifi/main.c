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
 * ipro7_sdio_wifi - example app and bench for the ipronet_host component.
 *
 * The SDIO-WiFi host flow (SMID transport, control plane, lwIP netif) lives in
 * components/network/ipronet_host. This app shows the customer-facing use
 * (ipronet_start / ipronet_connect + callbacks - see cmd_ipronet_up) and keeps
 * the bench: the step-by-step ipro6s_* commands, raw SDH probes, counters and
 * the IPRO6S reset / BROM-download control that this EVB wires to IPRO7 GPIOs.
 *
 * Board facts (EVB): IPRO7 GPIO15 switches the SD 3.3 V rail (0 = ON) and the
 * IPRO6S BROM download strap is SD_D0 = IPRO7 GPIO29. The rail is handed to
 * the component (config.pwr_en_gpio); the strap is a bench-only trick kept
 * here in ipro6_dl.
 *
 * Shell (customer path):
 *   ipronet_up <ssid> <psk>                    ipronet_start(EVB cfg) + connect
 *   ipronet_down                               ipronet_stop()
 * Shell (step by step - the component's bring-up stages as commands):
 *   ipro6s_init / ipro6s_enum / ipro6s_setup   host bring-up (F1 blk=512)
 *   ipro6s_rx_start                            spawn the doorbell-drain rx_task
 *   ipro6s_send_hello                          CMD_HELLO to engage the bridge
 *   ipro6s_get_mac                             GET_MAC handshake -> iproeth0 MAC
 *   ipro6s_net_up                              bring up iproeth0 lwIP netif
 *   ipro6s_scan / ipro6s_connect / ...         host-driven control plane
 *   ipro6s_probe                               ipronet_start() without a power-cycle
 * Shell (instrumentation):
 *   ipro6s_doorbell                            one-shot doorbell read
 *   ipro6s_flood_rx [secs]                     RX ceiling test (read, no dispatch)
 *   ipro6s_tx_ramp <count> <len>               TX ramp test through the agg path
 *   ipro6s_rx_stats / ipro6s_tx_stats          counters
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
#include "hal_mtimer.h"
#include "freertos_app_hooks.h"

#include "hal_sdio.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "compiler/common.h"   /* ATTR_PSRAM_NOINIT_SECTION */

#include "lwip/opt.h"     /* s16_t + TCP_TMR_INTERVAL for the tcprto knob */
#include "lwip/tcp.h"     /* pcb fields for the tcpstat congestion census */
#include "lwip/priv/tcp_priv.h" /* tcp_active_pcbs */
#include "lwip/stats.h"   /* lwip_stats for the silent-drop census */
#include "lwip/memp.h"    /* MEMP_NETBUF & friends */
#include "lwip/icmp.h"    /* dest-unreach rate limiter knob + counter */

#include "ipronet_host.h"
#include "ipronet_host_diag.h"

#define IPRO6_DL_STRAP_GPIO    GLB_GPIO_PIN_29   /* SD_D0 = IPRO6S BROM download strap */
#define IPRO6_PWR_OFF_MS_DFLT  300U              /* rail discharge time */

/* ---- app callbacks: print exactly what the pre-component app printed ---- */
static void app_on_connected(void *arg)
{
    (void)arg;
    printf("[ctrl] STA CONNECTED\r\n");
}

static void app_on_disconnected(void *arg)
{
    (void)arg;
    printf("[ctrl] STA DISCONNECTED\r\n");
}

static void app_on_got_ip(void *arg, const ipronet_ip_info_t *ip)
{
    (void)arg;
    printf("[ctrl] IP %u.%u.%u.%u / %u.%u.%u.%u gw %u.%u.%u.%u\r\n",
           ip->ip4[0], ip->ip4[1], ip->ip4[2], ip->ip4[3],
           ip->mask4[0], ip->mask4[1], ip->mask4[2], ip->mask4[3],
           ip->gw4[0], ip->gw4[1], ip->gw4[2], ip->gw4[3]);
}

/* Callbacks run in the component's RX task, whose printf is not reliable on
 * this bench console; the last scan is kept so ipro6s_scan_last can print it
 * from the shell. 45 = the most the device sends in one SCAN_RESULTS. */
#define APP_SCAN_KEEP 45U
static ATTR_PSRAM_NOINIT_SECTION ipronet_ap_t s_scan_last[APP_SCAN_KEEP];
static volatile uint16_t s_scan_last_n = 0;
static volatile uint32_t s_scan_count  = 0;    /* on_scan_result calls */

static void app_on_scan_result(void *arg, const ipronet_ap_t *aps, uint16_t n)
{
    (void)arg;
    uint16_t keep = (n < APP_SCAN_KEEP) ? n : APP_SCAN_KEEP;
    memcpy(s_scan_last, aps, keep * sizeof(*aps));
    s_scan_last_n = keep;
    s_scan_count++;
    printf("[ctrl] scan: %u APs\r\n", n);
    for (uint16_t i = 0; i < n; i++) {
        printf("  ssid=%-32s rssi=%d ch=%u auth=%u\r\n",
               aps[i].ssid, (int)aps[i].rssi, (unsigned)aps[i].channel,
               (unsigned)aps[i].auth_mode);
    }
}

static const ipronet_callbacks_t s_app_cb = {
    .on_connected    = app_on_connected,
    .on_disconnected = app_on_disconnected,
    .on_got_ip       = app_on_got_ip,
    .on_scan_result  = app_on_scan_result,
    .arg             = NULL,
};

/* ---- the customer path: one command does what a customer app does ------- */
static int cmd_ipronet_up(int argc, char **argv)
{
    ipronet_config_t cfg;
    ipronet_config_default(&cfg);            /* EVB: GPIO15 rail, 25 MHz, reconnect on, 16 Mbps pacer */
    int rc = ipronet_start(&cfg, &s_app_cb);
    printf("ipronet_start -> %d\r\n", rc);
    if (rc != IPRONET_OK) return rc;
    if (argc >= 3) {
        rc = ipronet_connect(argv[1], argv[2]);
        printf("ipronet_connect '%s' -> %d\r\n", argv[1], rc);
    }
    return rc;
}

static int cmd_ipronet_down(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = ipronet_stop();
    printf("ipronet_stop -> %d\r\n", rc);
    return rc;
}

/* ------------------------------------------------------------------ */
/* IPRO6 DUT reset / download control (no relay on this bench)        */
/*                                                                     */
/* The IPRO6 DUT runs off the SD-card 3.3V rail, switched by IPRO7    */
/* GPIO15 (SD PWR EN, 0 = rail ON). "Reset" is a power cycle. The     */
/* IPRO6 BROM download strap shares its SD_D0 pad, i.e. IPRO7 GPIO29  */
/* on the SDIO bus: hold it HIGH across power-on and the BROM enters  */
/* UART download mode instead of booting from flash.                  */
/*                                                                     */
/* The rail itself (park the SDH pads, switch, 200 ms hold, pinmux    */
/* restore) is the component's ipronet_device_power(); only the strap */
/* lives here.                                                         */
/* ------------------------------------------------------------------ */

static void ipro6_gpio_out(uint8_t pin, uint32_t level)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin  = pin,
        .gpioFun  = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = GPIO_PULL_NONE,
        .drive    = 0,
        .smtCtrl  = 1,
    };
    GLB_GPIO_Write(pin, level);   /* data reg first — no glitch on OE */
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_OUTPUT_Enable(pin);
}

/* The component owns the rail through config.pwr_en_gpio, and ipronet_start()
 * re-registers it from its config every time (-1 for ipro6s_probe, which must
 * not power-cycle). The bench needs the EVB rail before and between starts, so
 * register it again on every use. */
static void bench_power_ready(void)
{
    ipronet_config_t cfg;
    ipronet_config_default(&cfg);
    ipronet_sdio_power_configure(cfg.pwr_en_gpio, cfg.pwr_active_low);
}

static int cmd_ipro6_rst(int argc, char **argv)
{
    uint32_t off_ms = (argc > 1) ? (uint32_t)strtoul(argv[1], NULL, 0)
                                 : IPRO6_PWR_OFF_MS_DFLT;
    bench_power_ready();
    printf("ipro6: power-cycle (rail off %u ms)\r\n", (unsigned)off_ms);
    (void)ipronet_stop();                    /* close TX mode before device reboot */
    ipronet_device_power(0);
    vTaskDelay(pdMS_TO_TICKS(off_ms));
    ipronet_device_power(1);
    printf("ipro6: back up — SDIO session lost, rerun ipro6s_probe / ipronet_up\r\n");
    return 0;
}

static int cmd_ipro6_dl(int argc, char **argv)
{
    uint32_t off_ms = (argc > 1) ? (uint32_t)strtoul(argv[1], NULL, 0)
                                 : IPRO6_PWR_OFF_MS_DFLT;
    bench_power_ready();
    printf("ipro6: power-cycle into BROM download (SD_D0 strap high)\r\n");
    (void)ipronet_stop();                    /* close TX mode before device reboot */
    ipronet_device_power(0);                 /* pads parked, rail off */
    vTaskDelay(pdMS_TO_TICKS(off_ms));
    ipro6_gpio_out(IPRO6_DL_STRAP_GPIO, 1U); /* strap high before the rail */
    ipronet_device_power(1);                 /* rail on, 200 ms hold, pinmux restore releases the strap */
    printf("ipro6: BROM download mode — flash via IPRO6 UART now\r\n");
    return 0;
}

static int cmd_ipro6_pwr(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: ipro6_pwr <0|1>\r\n");
        return -1;
    }
    bench_power_ready();
    int on = (atoi(argv[1]) != 0);
    if (!on) (void)ipronet_stop();
    ipronet_device_power(on);
    printf("ipro6: 3.3V rail %s\r\n", on ? "ON (SDH pinmux restored)" : "OFF (SDH pads parked)");
    return 0;
}

/* ------------------------------------------------------------------ */
/* Step-by-step bring-up over the diag API                             */
/* ------------------------------------------------------------------ */

static int cmd_ipro6s_init(int argc, char **argv)
{
    (void)argc; (void)argv;
    ipronet_diag_set_callbacks(&s_app_cb);   /* same callbacks as the customer path */
    int rc = ipronet_diag_sdio_init();
    printf("ipro6s_init -> %d\r\n", rc);
    return rc;
}

static int cmd_ipro6s_enum(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = ipronet_diag_sdio_enum();
    printf("ipro6s_enum -> %d\r\n", rc);
    return rc;
}

/* "ipro6s_setup hs" -> SDR50 / 50 MHz high-speed; default 25 MHz. */
static int cmd_ipro6s_setup(int argc, char **argv)
{
    uint32_t hz = ((argc >= 2) && !strcmp(argv[1], "hs")) ? 50000000U : 25000000U;
    int rc = ipronet_diag_sdio_setup(hz);
    printf("ipro6s_setup(%lu) -> %d\r\n", (unsigned long)hz, rc);
    return rc;
}

static int cmd_ipro6s_rx_start(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = ipronet_diag_rx_start();
    printf("[rx] rx_start -> %d\r\n", rc);
    return rc;
}

static int cmd_ipro6s_send_hello(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = ipronet_diag_send_hello();
    printf("[tx] send_hello -> %d\r\n", rc);
    return rc;
}

/* GET_MAC handshake: learn the device STA MAC (random per boot) into iproeth0.
 * rx_task MUST already be running - the reply lands on the doorbell RX path. */
static int cmd_ipro6s_get_mac(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = ipronet_diag_get_mac();
    if (rc != IPRONET_OK) {
        printf("[ctrl] no MAC ind (%d) - rx_task running? device up?\r\n", rc);
        return rc;
    }
    uint8_t m[6];
    ipronet_get_mac(m);
    printf("[ctrl] MAC learned sta=%02X:%02X:%02X:%02X:%02X:%02X\r\n",
           m[0], m[1], m[2], m[3], m[4], m[5]);
    return 0;
}

static int cmd_ipro6s_mac(int argc, char **argv)
{
    (void)argc; (void)argv;
    /* SMID has no scratch MAC register (0x60/0x70 alias the read-count doorbell
     * and must never be read). The WiFi STA/AP MAC arrives via the GET_MAC
     * handshake — run ipro6s_get_mac first. */
    uint8_t s[6], a[6];
    if (ipronet_get_mac(s) != IPRONET_OK) {
        printf("SMID: MAC not learned yet — run ipro6s_get_mac\r\n");
        return -1;
    }
    ipronet_diag_ap_mac(a);
    printf("SMID MAC: sta=%02X:%02X:%02X:%02X:%02X:%02X "
           "ap=%02X:%02X:%02X:%02X:%02X:%02X\r\n",
           s[0], s[1], s[2], s[3], s[4], s[5],
           a[0], a[1], a[2], a[3], a[4], a[5]);
    return 0;
}

/* Zero-copy opt-in: 1 = single-frame, 2 = TX stride, 3 = TX+RX stride. Confirm
 * the grant on the DEVICE side ("user_ext: ZCEN3 -> ZCOK3 ...") or by
 * tx_pool_drops staying near zero in its `sdio_diag`. */
static int cmd_ipro6s_zcen(int argc, char **argv)
{
    int gen = (argc >= 2) ? (int)strtoul(argv[1], NULL, 0) : 1;
    int rc = ipronet_diag_zcen(gen);
    if (rc == IPRONET_ERR_PARAM) {
        printf("usage: ipro6s_zcen [1|2|3]\r\n");
        return rc;
    }
    printf("[zc] ZCEN%d sent -> %d (single-frame TX armed)\r\n", gen, rc);
    return rc;
}

/* Bring up the iproeth0 lwIP netif (tcpip + placeholder static IP). Not needed
 * after ipro6s_probe / ipronet_up: start() already brings the netif up. */
static int cmd_ipro6s_net_up(int argc, char **argv)
{
    (void)argc; (void)argv;
    uint8_t m[6];
    int have_mac = (ipronet_get_mac(m) == IPRONET_OK);
    /* iproeth0 MAC must equal the device STA MAC or the AP drops our frames.
     * Proceed either way, but warn if the GET_MAC handshake has not run. */
    if (!have_mac) {
        printf("iproeth0 WARN: MAC not learned — run ipro6s_get_mac "
               "(netif uses default 12:23:34:00:45:56)\r\n");
    }
    ipronet_diag_netif_up();
    if (have_mac) {
        printf("iproeth0 up=1  ip=%s  mac=%02X:%02X:%02X:%02X:%02X:%02X "
               "(placeholder until IP_UPDATE_IND)\r\n", ipronet_diag_netif_ip_str(),
               m[0], m[1], m[2], m[3], m[4], m[5]);
    } else {
        printf("iproeth0 up=1  ip=%s  mac=12:23:34:00:45:56 (default)\r\n",
               ipronet_diag_netif_ip_str());
    }
    return 0;
}

/* Full bring-up the way the pre-component app did it: no power-cycle (the
 * bench resets the device explicitly with ipro6_rst), same callbacks. */
static int cmd_ipro6s_probe(int argc, char **argv)
{
    (void)argc; (void)argv;
    ipronet_config_t cfg;
    ipronet_config_default(&cfg);
    cfg.pwr_en_gpio = -1;
    printf("\r\n=== ipro6s_probe — full SMID bring-up ===\r\n");
    int rc = ipronet_start(&cfg, &s_app_cb);
    if (rc != IPRONET_OK) {
        printf("=== probe FAIL (%d) ===\r\n", rc);
        return rc;
    }
    uint8_t m[6], a[6];
    ipronet_get_mac(m);
    ipronet_diag_ap_mac(a);
    printf("[ctrl] MAC sta=%02X:%02X:%02X:%02X:%02X:%02X "
           "ap=%02X:%02X:%02X:%02X:%02X:%02X\r\n",
           m[0], m[1], m[2], m[3], m[4], m[5],
           a[0], a[1], a[2], a[3], a[4], a[5]);
    printf("=== probe OK — SMID transport + control handshake up ===\r\n");
    printf("    Now: ipro6s_connect <ssid> <psk> (iproeth0 is already up)\r\n");
    return 0;
}

/* ipro6s_connect <ssid> <psk>: host commands the device to associate. */
static int cmd_ipro6s_connect(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: ipro6s_connect <ssid> <psk>\r\n");
        return -1;
    }
    int rc = ipronet_connect(argv[1], argv[2]);
    printf("[ctrl] connect '%s' -> %d (CONNECTED_IND + IP_UPDATE_IND arrive async)\r\n",
           argv[1], rc);
    return rc;
}

static int cmd_ipro6s_disconnect(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = ipronet_disconnect();
    printf("[ctrl] disconnect -> %d\r\n", rc);
    return rc;
}

/* ipro6s_scan: start a scan, then request the results (printed async). */
static int cmd_ipro6s_scan(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = ipronet_scan();
    printf("[ctrl] scan -> %d (list prints async)\r\n", rc);
    return rc;
}

/* ipro6s_scan_last: the last on_scan_result, printed from the shell task. */
static int cmd_ipro6s_scan_last(int argc, char **argv)
{
    (void)argc; (void)argv;
    uint16_t n = s_scan_last_n;
    printf("[ctrl] scan_last: %u APs (on_scan_result fired %lu times)\r\n",
           (unsigned)n, (unsigned long)s_scan_count);
    for (uint16_t i = 0; i < n; i++) {
        printf("  %2u ssid=%-32s rssi=%d ch=%u auth=%u\r\n", (unsigned)i,
               s_scan_last[i].ssid, (int)s_scan_last[i].rssi,
               (unsigned)s_scan_last[i].channel, (unsigned)s_scan_last[i].auth_mode);
    }
    return 0;
}

static int cmd_ipro6s_link_status(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = ipronet_diag_link_status();
    printf("[ctrl] link_status req -> %d (host connected=%d ip=%d)\r\n", rc,
           (int)ipronet_is_connected(), (int)ipronet_has_ip());
    return rc;
}

/* Egress pacer knob + stats. */
static int cmd_ipro6s_pace(int argc, char **argv)
{
    if (argc >= 2) {
        ipronet_diag_set_pace((uint32_t)atoi(argv[1]));
    }
    ipronet_tx_stats_t t;
    ipronet_diag_tx_stats(&t);
    printf("[pace] kbps=%lu drops=%lu\r\n",
           (unsigned long)t.pace_kbps, (unsigned long)t.pace_drops);
    return 0;
}

/* Auto-reconnect knob + status. */
static int cmd_ipro6s_reconn(int argc, char **argv)
{
    if (argc >= 2) {
        ipronet_diag_set_reconnect(atoi(argv[1]));
    }
    ipronet_reconn_stats_t r;
    ipronet_diag_reconn_stats(&r);
    printf("[reconn] en=%d armed=%d ssid='%s' attempts=%lu restores=%lu\r\n",
           (int)r.enabled, (int)r.armed, r.ssid,
           (unsigned long)r.attempts, (unsigned long)r.restores);
    return 0;
}

/* Ask the DEVICE to enable/disable its own autoconnect (RNM
 * STA_SET_AUTO_RECONNECT). ipronet_connect() sends 0 by itself; this is the
 * isolated knob for the bench. */
static int cmd_ipro6s_devauto(int argc, char **argv)
{
    if (argc < 2) { printf("usage: ipro6s_devauto <0|1>\r\n"); return -1; }
    int en = (int)strtol(argv[1], NULL, 0);
    int rc = ipronet_diag_device_autoconnect(en);
    vTaskDelay(pdMS_TO_TICKS(500));            /* let the device ACK land */
    uint16_t acmd = 0, astatus = 0;
    ipronet_diag_last_ack(&acmd, &astatus);
    printf("[ctrl] device autoconnect %s -> %d (last ACK cmd=%u status=%u)\r\n",
           en ? "ENABLE" : "DISABLE", rc, (unsigned)acmd, (unsigned)astatus);
    return rc;
}

/* TX-aggregation batch cap (A/B). ipro6s_aggmax 1 flushes every frame; higher
 * values pack up to N frames/CMD53, bounded by the negotiated ZCEN2 slots. */
static int cmd_ipro6s_aggmax(int argc, char **argv)
{
    if (argc >= 2) {
        ipronet_diag_set_aggmax((uint32_t)strtoul(argv[1], NULL, 0));
    }
    ipronet_tx_stats_t t;
    ipronet_diag_tx_stats(&t);
    uint32_t tx_agg_max = t.agg_max;
    printf("tx_agg_max=%lu (%s)\r\n", (unsigned long)tx_agg_max,
           tx_agg_max == 0U ? "BYPASS agg (true send_pdu path)" :
           tx_agg_max == 1U ? "no batching (agg path, flush each)" : "aggregate");
    return 0;
}

/* A/B + stats knob for the data-ready IRQ side-channel. `ipro6s_girq 1` =
 * IRQ-wake ON (unmask GPIO0), `ipro6s_girq 0` = OFF (mask GPIO0 -> rx_task
 * falls back to the pure backstop poll). No arg = print live stats.
 * The GPIO is configured by ipro6s_rx_start; run that first. */
static int cmd_ipro6s_girq(int argc, char **argv)
{
    if (!ipronet_diag_rx_task_running()) {
        printf("run ipro6s_rx_start first (GPIO0 IRQ is armed there)\r\n");
        return -1;
    }
    if (argc >= 2) {
        int en = (int)strtol(argv[1], NULL, 0);
        ipronet_diag_girq(en);
        printf("girq wake %s\r\n", en ? "ON (IRQ)" : "OFF (poll backstop only)");
    }
    ipronet_rx_stats_t r;
    ipronet_diag_rx_stats(&r);
    printf("girq pin=0 level=%lu irq_count=%lu int_mask1=0x%08lX int_stat1=0x%08lX\r\n",
           (unsigned long)GLB_GPIO_Read(0), (unsigned long)r.girq_count,
           (unsigned long)(*(volatile uint32_t *)0x300001A0),
           (unsigned long)(*(volatile uint32_t *)0x300001A8));
    return 0;
}

/* ------------------------------------------------------------------ */
/* Counters                                                            */
/* ------------------------------------------------------------------ */

/* Throughput testing uses the SDK's esp_iperf component, which registers a
 * standard `iperf` shell command (BSD sockets, reliable interval report):
 *   host RX:  iperf -s -u   (or TCP: iperf -s)   on this DUT; peer = iperf -c <host> ...
 *   host TX:  iperf -c <peer> -u -b <rate> -t <secs>   (peer = iperf -s ...) */

static int cmd_ipro6s_tx_stats(int argc, char **argv)
{
    (void)argc; (void)argv;
    ipronet_tx_stats_t t;
    ipronet_diag_tx_stats(&t);
    printf("[tx] counters:\r\n");
    printf("  tx_writes            = %lu  (CMD53 writes: single + agg)\r\n",
           (unsigned long)t.writes);
    printf("  tx_agg_frames        = %lu  (frames across all writes)\r\n",
           (unsigned long)t.agg_frames);
    printf("  tx_cmd_frames        = %lu  (type=0xef01)\r\n",
           (unsigned long)t.cmd_frames);
    printf("  tx_eth_frames        = %lu  (type=0xabcd)\r\n",
           (unsigned long)t.eth_frames);
    printf("  tx_bytes             = %lu  (on-wire, padded blocks)\r\n",
           (unsigned long)t.bytes);
    printf("  tx_errors            = %lu\r\n", (unsigned long)t.errors);
    printf("  tx_agg_flushes       = %lu  (batch writes; to_flush=%lu)\r\n",
           (unsigned long)t.agg_flushes, (unsigned long)t.agg_to_flush);
    {
        uint32_t w = t.writes ? t.writes : 1U;
        printf("  frames/write         = %lu.%02lu\r\n",
               (unsigned long)(t.agg_frames / w),
               (unsigned long)((t.agg_frames * 100UL / w) % 100UL));
        printf("  write_us/write       = %lu  (max=%lu)\r\n",
               (unsigned long)(t.write_us / w),
               (unsigned long)t.write_max_us);
    }
    return 0;
}

/* Card-int IRQ stub. The IPRO7 SDH never reliably raises SDH_INT_CARD for the
 * SMID DAT1 in-band interrupt (see project_ipronet_phase_d_blocker), so RX runs
 * off the doorbell poll in the component's rx_task, not this ISR path. Kept as
 * a counter-only stub so ipro6s_irq_on / ipro6s_irq_wait remain usable as
 * diagnostics. */
static volatile uint32_t s_func1_irq_count = 0;   /* card-int IRQ stub counter */

static void app_func1_irq(uint8_t func_num, void *arg)
{
    (void)func_num; (void)arg;
    s_func1_irq_count++;
}

static int cmd_ipro6s_rx_stats(int argc, char **argv)
{
    (void)argc; (void)argv;
    ipronet_rx_stats_t r;
    ipronet_diag_rx_stats(&r);
    printf("[rx] counters:\r\n");
    printf("  doorbell_polls      = %lu\r\n", (unsigned long)r.doorbell_polls);
    printf("  doorbell_zero       = %lu\r\n", (unsigned long)r.doorbell_zero);
    printf("  rx_transfers        = %lu (CMD53 reads)\r\n",
           (unsigned long)r.transfers);
    printf("  rx_bytes            = %lu\r\n", (unsigned long)r.bytes);
    printf("  rx_pdus             = %lu (valid headers)\r\n",
           (unsigned long)r.pdus);
    printf("  rx_data_frames      = %lu\r\n", (unsigned long)r.data_frames);
    printf("  rx_amsdu_drop       = %lu\r\n", (unsigned long)r.amsdu_drop);
    printf("  rx_bad_type         = %lu\r\n", (unsigned long)r.bad_type);
    printf("  rx_bad_subtype      = %lu\r\n", (unsigned long)r.bad_subtype);
    printf("  rx_desync           = %lu (has_twin/pld_off/len)\r\n",
           (unsigned long)r.desync);
    printf("  rx_unaligned_cnt    = %lu (doorbell cnt not 512-aligned)\r\n",
           (unsigned long)r.unaligned_cnt);
    /* Silent drops between rx_task and the lwIP sink. pool = PBUF_POOL
     * exhausted; mbox = tcpip_input mbox full (tcpip thread behind). */
    printf("  rx_pool_drop        = %lu (PBUF_POOL exhausted)\r\n",
           (unsigned long)r.pool_drop);
    printf("  rx_mbox_drop        = %lu (tcpip_input mbox full)\r\n",
           (unsigned long)r.mbox_drop);
    printf("  last_pdu len=%u subtype=0x%04X\r\n",
           r.last_pdu_len, r.last_pdu_subtype);
    printf("  func1_irq_count     = %lu (card-int stub)\r\n",
           (unsigned long)s_func1_irq_count);
    return 0;
}

/* Where the received datagrams actually end up. rx_pool_drop / rx_mbox_drop
 * only cover netif -> tcpip_input; everything past that (udp_input, the
 * netconn recvmbox, MEMP_NETBUF) drops SILENTLY in lwIP — recv_udp() frees the
 * pbuf and returns with no counter of its own. MEMP_NETBUF err is the one
 * counter that does record it, because a full socket queue means the 32
 * netbufs are all outstanding. */
static int cmd_ipro6s_lwipstat(int argc, char **argv)
{
    (void)argc; (void)argv;
    static const struct { int id; const char *name; } pools[] = {
        { MEMP_NETBUF,          "NETBUF"     },
        { MEMP_PBUF,            "PBUF(ref)"  },
        { MEMP_PBUF_POOL,       "PBUF_POOL"  },
        { MEMP_TCPIP_MSG_INPKT, "MSG_INPKT"  },
        { MEMP_TCPIP_MSG_API,   "MSG_API"    },
    };
    printf("[lwip] pool          used   max  avail   err\r\n");
    for (unsigned i = 0; i < sizeof(pools) / sizeof(pools[0]); i++) {
        const struct stats_mem *m = lwip_stats.memp[pools[i].id];
        printf("[lwip] %-11s %6u %5u %6u %5u\r\n", pools[i].name,
               (unsigned)m->used, (unsigned)m->max, (unsigned)m->avail,
               (unsigned)m->err);
    }
    printf("[lwip] link recv=%u drop=%u memerr=%u\r\n",
           (unsigned)lwip_stats.link.recv, (unsigned)lwip_stats.link.drop,
           (unsigned)lwip_stats.link.memerr);
    printf("[lwip] ip   recv=%u drop=%u memerr=%u lenerr=%u chkerr=%u\r\n",
           (unsigned)lwip_stats.ip.recv, (unsigned)lwip_stats.ip.drop,
           (unsigned)lwip_stats.ip.memerr, (unsigned)lwip_stats.ip.lenerr,
           (unsigned)lwip_stats.ip.chkerr);
    printf("[lwip] udp  recv=%u drop=%u memerr=%u chkerr=%u proterr=%u\r\n",
           (unsigned)lwip_stats.udp.recv, (unsigned)lwip_stats.udp.drop,
           (unsigned)lwip_stats.udp.memerr, (unsigned)lwip_stats.udp.chkerr,
           (unsigned)lwip_stats.udp.proterr);
    printf("[lwip] sys  mbox used=%u max=%u err=%u\r\n",
           (unsigned)lwip_stats.sys.mbox.used, (unsigned)lwip_stats.sys.mbox.max,
           (unsigned)lwip_stats.sys.mbox.err);
    return 0;
}

static int cmd_ipro6s_rxprof(int argc, char **argv)
{
    int reset = (argc >= 2) && (strtol(argv[1], NULL, 0) != 0);
    ipronet_diag_rx_profile(reset);
    return 0;
}

/* A/B knob for where rx_task spins through a CMD53 upload: 1 = at IDLE+1
 * (default; the consumer gets the CPU), 0 = at its own top priority (the
 * original behaviour). Same image both ways. */
static int cmd_ipro6s_rxwait(int argc, char **argv)
{
    int on = (argc >= 2) ? (strtol(argv[1], NULL, 0) != 0) : -1;
    printf("[rx] CMD53 wait: %s\r\n",
           ipronet_diag_rx_xfer_lowprio(on) ? "lowprio (IDLE+1)" : "top prio (spin)");
    return 0;
}

/* A/B knob for the ICMP destination-unreachable cap: <n> messages per second,
 * 0 = unlimited (stock lwIP). Same image both ways, so the two arms can be
 * alternated inside one boot. Every datagram that arrives for a closed port
 * costs one frame transmitted out of the tcpip thread without it. */
#if ICMP_DEST_UNREACH_RATE_LIMIT
static int cmd_ipro6s_icmplim(int argc, char **argv)
{
    if (argc >= 2) {
        icmp_dur_limit_per_s = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    printf("[icmp] dest-unreach cap = %lu/s (0=off), suppressed = %lu\r\n",
           (unsigned long)icmp_dur_limit_per_s,
           (unsigned long)icmp_dur_suppressed);
    return 0;
}
#endif

/* ------------------------------------------------------------------ */
/* Raw SDH probes over ipronet_diag_host()                             */
/* ------------------------------------------------------------------ */

static int cmd_ipro6s_irq_on(int argc, char **argv)
{
    (void)argc; (void)argv;
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL || !host->funcs[IPRONET_FUNC_DATA].enabled) {
        printf("run ipro6s_setup first\r\n");
        return -1;
    }

    s_func1_irq_count = 0;
    SDIO_Status_Type st =
        sdio_claim_irq(host, IPRONET_FUNC_DATA, app_func1_irq, NULL);
    printf("  claim_irq(F1) -> %d\r\n", (int)st);
    if (st != Status_SDIO_Success) return -1;

    st = sdio_enable_irq(host);
    printf("  enable_irq -> %d (ECLIC SDH_IRQn unmasked, SDH_INT_CARD source on)\r\n",
           (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

static int cmd_ipro6s_irq_wait(int argc, char **argv)
{
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }
    uint32_t ms = (argc >= 2) ? (uint32_t)strtoul(argv[1], NULL, 0) : 5000U;
    uint32_t prev = s_func1_irq_count;

    SDIO_Status_Type st = sdio_wait_irq(host, pdMS_TO_TICKS(ms));
    if (st != Status_SDIO_Success) {
        printf("ipro6s: wait_irq timeout/err after %lu ms (irq_count=%lu)\r\n",
               (unsigned long)ms, (unsigned long)s_func1_irq_count);
        return -1;
    }
    printf("ipro6s: wait_irq fired; F1 handler invocations: prev=%lu now=%lu\r\n",
           (unsigned long)prev, (unsigned long)s_func1_irq_count);
    return 0;
}

/* Probe whether the SDH data engine can do ANY CMD53 at all, by issuing
 * a byte-mode CMD53 READ of 16 bytes from CCCR (F0, incrementing). If
 * this also hangs, the data engine is broken regardless of write/read.
 * If it succeeds, the bug is specific to the WRITE direction or to the
 * IPRO6S F1 data port.  Usage: ipro6s_cmd53_read */
static int cmd_ipro6s_cmd53_read(int argc, char **argv)
{
    (void)argc; (void)argv;
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }
    static __attribute__((aligned(4))) uint8_t buf[16];
    memset(buf, 0xa5, sizeof(buf));
    SDIO_Status_Type st = sdio_read_bytes(host, SDIO_FUNC_0,
                                          0x00U, buf, 16, false);
    printf("[cmd53-rd] F0[0x00..0x0F] byte-mode read -> %d\r\n", (int)st);
    if (st == Status_SDIO_Success) {
        printf("  data: ");
        for (int i = 0; i < 16; i++) printf("%02x ", buf[i]);
        printf("\r\n");
    }
    return 0;
}

/* Raw SDH-engine READ probe: ONE block-mode CMD53 (1x512 B) at the fixed FIFO
 * address. The data is meaningless unless the device has a pending upload, but
 * the CMD53 itself should still clock 512 B. */
static int cmd_ipro6s_cmd53_blkread(int argc, char **argv)
{
    (void)argc; (void)argv;
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }
    static __attribute__((aligned(4))) uint8_t buf[IPRONET_FUNC_BLOCK_SIZE];
    memset(buf, 0xa5, sizeof(buf));
    SDIO_Status_Type st = sdio_read_blocks(host, IPRONET_FUNC_DATA,
                                           IPRONET_FIFO_ADDR, buf, 1, true);
    printf("[cmd53-blkrd] F1 FIFO (1 blk = %u B) -> %d\r\n",
           IPRONET_FUNC_BLOCK_SIZE, (int)st);
    if (st == Status_SDIO_Success) {
        printf("  head: ");
        for (int i = 0; i < 16; i++) printf("%02x ", buf[i]);
        printf("\r\n");
    }
    return 0;
}

/* Raw SDH-engine WRITE probe: ONE block-mode CMD53 (1x512 B) at the fixed FIFO
 * address. The device parses it as a (malformed) download aggregate and drops
 * it (smid_rx_agg_ovf) — a pure data-engine test, no framing. */
static int cmd_ipro6s_cmd53_blkwrite(int argc, char **argv)
{
    (void)argc; (void)argv;
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }
    static __attribute__((aligned(4))) uint8_t buf[IPRONET_FUNC_BLOCK_SIZE];
    memset(buf, 0x5a, sizeof(buf));
    SDIO_Status_Type st = sdio_write_blocks(host, IPRONET_FUNC_DATA,
                                            IPRONET_FIFO_ADDR, buf, 1, true);
    printf("[cmd53-blkwr] F1 FIFO (1 blk = %u B) -> %d\r\n",
           IPRONET_FUNC_BLOCK_SIZE, (int)st);
    return 0;
}

/* Diagnostic knobs in drv_sdh.c for the trigger-less LA write-CRC-status
 * capture. Lower the poll timeout so a wedging CMD53 write returns fast, then
 * spam writes so the bus carries a burst inside any LA sampling window. */
extern uint32_t g_sdh_data_poll_timeout_ms;
extern volatile int g_sdh_data_quiet_timeout;

/* Usage: ipro6s_set_poll_timeout <ms>  (default 5000). */
static int cmd_ipro6s_set_poll_timeout(int argc, char **argv)
{
    if (argc < 2) { printf("usage: ipro6s_set_poll_timeout <ms>\r\n"); return -1; }
    uint32_t ms = (uint32_t)strtoul(argv[1], NULL, 0);
    if (ms == 0) ms = 1;
    g_sdh_data_poll_timeout_ms = ms;
    printf("g_sdh_data_poll_timeout_ms = %lu\r\n", (unsigned long)g_sdh_data_poll_timeout_ms);
    return 0;
}

/* Fire CMD53 512B single-block WRITE in a tight loop so the bus carries a write
 * burst every poll-timeout window — lets the trigger-less U3Pro16 reliably
 * sample the DAT0 write-CRC-status window. Run `ipro6s_set_poll_timeout 20`
 * first. quiet=1 suppresses the per-timeout register dump.
 * Usage: ipro6s_cmd53_blkwrite_spam [count=200]. */
static int cmd_ipro6s_cmd53_blkwrite_spam(int argc, char **argv)
{
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }
    uint32_t n = (argc >= 2) ? (uint32_t)strtoul(argv[1], NULL, 0) : 200U;
    static __attribute__((aligned(4))) uint8_t buf[IPRONET_FUNC_BLOCK_SIZE];
    memset(buf, 0x5a, sizeof(buf));
    printf("[spam] %lu writes, poll_timeout=%lu ms (quiet)\r\n",
           (unsigned long)n, (unsigned long)g_sdh_data_poll_timeout_ms);
    g_sdh_data_quiet_timeout = 1;
    uint32_t ok = 0;
    for (uint32_t i = 0; i < n; i++) {
        SDIO_Status_Type st = sdio_write_blocks(host, IPRONET_FUNC_DATA,
                                                IPRONET_FIFO_ADDR, buf, 1, true);
        if (st == Status_SDIO_Success) ok++;
    }
    g_sdh_data_quiet_timeout = 0;
    printf("[spam] done: %lu/%lu completed\r\n", (unsigned long)ok, (unsigned long)n);
    return 0;
}

/* Raw multi-block (4x512 B = 2048 B) READ at the fixed FIFO address. */
static int cmd_ipro6s_cmd53_blkread8(int argc, char **argv)
{
    (void)argc; (void)argv;
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }
    static __attribute__((aligned(4))) uint8_t buf[2048];
    uint16_t blocks = (uint16_t)(sizeof(buf) / IPRONET_FUNC_BLOCK_SIZE);
    memset(buf, 0xa5, sizeof(buf));
    SDIO_Status_Type st = sdio_read_blocks(host, IPRONET_FUNC_DATA,
                                           IPRONET_FIFO_ADDR, buf, blocks, true);
    printf("[cmd53-blkrd8] F1 FIFO (%u blk = %u B) -> %d\r\n",
           blocks, (unsigned)sizeof(buf), (int)st);
    if (st == Status_SDIO_Success) {
        printf("  head: ");
        for (int i = 0; i < 16; i++) printf("%02x ", buf[i]);
        printf("\r\n");
    }
    return 0;
}

/* One-shot: read the upload doorbell and print the pending transfer size. */
static int cmd_ipro6s_doorbell(int argc, char **argv)
{
    (void)argc; (void)argv;
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL || !host->funcs[IPRONET_FUNC_DATA].enabled) {
        printf("run ipro6s_setup first\r\n"); return -1;
    }
    sdio_claim_host(host);
    uint8_t lo = 0, hi = 0;
    SDIO_Status_Type st1 = sdio_readb(host, IPRONET_FUNC_DATA, IPRONET_DOORBELL_LO, &lo);
    SDIO_Status_Type st2 = sdio_readb(host, IPRONET_FUNC_DATA, IPRONET_DOORBELL_HI, &hi);
    sdio_release_host(host);
    if (st1 != Status_SDIO_Success || st2 != Status_SDIO_Success) {
        printf("[doorbell] read fail (%d/%d)\r\n", (int)st1, (int)st2);
        return -1;
    }
    uint16_t cnt = (uint16_t)lo | ((uint16_t)hi << 8);
    printf("[doorbell] cnt=%u bytes%s\r\n", cnt, cnt ? "" : " (nothing pending)");
    return 0;
}

/* Host side of the device smid_flood ceiling test: for <secs> seconds drain the
 * doorbell as fast as possible — read + header-validate every transfer but do
 * NOT dispatch to lwIP — then print transfers/bytes/throughput. Every SDH error
 * breaks the loop with counts printed (never wedges). Run WITHOUT rx_task (both
 * would race the same doorbell). */
static int cmd_ipro6s_flood_rx(int argc, char **argv)
{
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL || !host->funcs[IPRONET_FUNC_DATA].enabled) {
        printf("run ipro6s_setup first\r\n"); return -1;
    }
    if (ipronet_diag_rx_task_running()) {
        printf("[flood_rx] rx_task owns the doorbell — stop it first\r\n");
        return -1;
    }
    /* Borrow the transport's RX buffer, as this command did before the move:
     * rx_task is stopped, so nothing else is using it. */
    uint8_t *s_flood_buf = ipronet_diag_rx_buf();
    uint32_t secs = (argc >= 2) ? (uint32_t)strtoul(argv[1], NULL, 0) : 10U;
    if (secs == 0) secs = 10U;

    uint32_t xfers = 0, zero = 0, desync = 0, errs = 0;
    uint64_t bytes = 0, t_first = 0, t_last = 0;
    uint64_t t0 = mtimer_get_time_us();
    uint64_t t_end = t0 + (uint64_t)secs * 1000000ULL;

    printf("[flood_rx] draining doorbell for %lus (no dispatch)...\r\n",
           (unsigned long)secs);
    while (mtimer_get_time_us() < t_end) {
        sdio_claim_host(host);
        uint8_t lo = 0, hi = 0;
        if (sdio_readb(host, IPRONET_FUNC_DATA, IPRONET_DOORBELL_LO, &lo)
                != Status_SDIO_Success ||
            sdio_readb(host, IPRONET_FUNC_DATA, IPRONET_DOORBELL_HI, &hi)
                != Status_SDIO_Success) {
            sdio_release_host(host);
            errs++;
            printf("[flood_rx] doorbell read fail — stop\r\n");
            break;
        }
        uint16_t cnt = (uint16_t)lo | ((uint16_t)hi << 8);
        if (cnt == 0) {
            sdio_release_host(host);
            zero++;
            continue;
        }
        uint32_t rd = ((uint32_t)cnt + IPRONET_FUNC_BLOCK_SIZE - 1U)
                      & ~(IPRONET_FUNC_BLOCK_SIZE - 1U);
        if (rd == 0 || rd > IPRONET_RX_BUF_SIZE) {
            sdio_release_host(host);
            desync++;
            continue;
        }
        SDIO_Status_Type st = sdio_read_blocks(host, IPRONET_FUNC_DATA,
                                               IPRONET_FIFO_ADDR, s_flood_buf,
                                               (uint16_t)(rd / IPRONET_FUNC_BLOCK_SIZE),
                                               true);
        sdio_release_host(host);
        if (st != Status_SDIO_Success) {
            errs++;
            printf("[flood_rx] CMD53 read fail cnt=%u (%d) — stop\r\n", cnt, (int)st);
            break;
        }
        /* Header sanity only (no dispatch). */
        const struct ipronet_top_msg *m = (const struct ipronet_top_msg *)s_flood_buf;
        if (cnt < sizeof(*m) || m->has_twin) desync++;
        /* Burst window: first data transfer -> last data transfer. The fixed
         * poll window includes idle head/tail that would dilute the Mbps. */
        uint64_t now = mtimer_get_time_us();
        if (t_first == 0) t_first = now;
        t_last = now;
        xfers++;
        bytes += cnt;
    }
    uint64_t dt = mtimer_get_time_us() - t0;
    if (dt == 0) dt = 1;
    printf("[flood_rx] done: xfers=%lu bytes=%lu zero=%lu desync=%lu errs=%lu "
           "in %lu.%03lus\r\n",
           (unsigned long)xfers, (unsigned long)bytes, (unsigned long)zero,
           (unsigned long)desync, (unsigned long)errs,
           (unsigned long)(dt / 1000000U), (unsigned long)((dt / 1000U) % 1000U));
    uint64_t burst = (t_last > t_first) ? (t_last - t_first) : 1;
    uint32_t mbps10 = (uint32_t)((bytes * 8ULL * 10ULL) / burst);
    printf("[flood_rx] burst = %lu.%03lus -> %lu.%lu Mbps\r\n",
           (unsigned long)(burst / 1000000U),
           (unsigned long)((burst / 1000U) % 1000U),
           (unsigned long)(mbps10 / 10U), (unsigned long)(mbps10 % 10U));
    return 0;
}

/* Send <count> DATA frames of <len> ramp bytes (byte i = i & 0xFF) through the
 * normal aggregated TX path; print frames/bytes/throughput/errors. The device
 * smid_rx_agg_frames counter verifies delivery. */
static int cmd_ipro6s_tx_ramp(int argc, char **argv)
{
    if (!ipronet_diag_host() || !ipronet_diag_rx_task_running()) {
        printf("run ipro6s_setup + ipro6s_rx_start first\r\n"); return -1;
    }
    if (argc < 3) {
        printf("usage: ipro6s_tx_ramp <count> <len>\r\n"); return -1;
    }
    uint32_t count = (uint32_t)strtoul(argv[1], NULL, 0);
    uint32_t len   = (uint32_t)strtoul(argv[2], NULL, 0);
    if (count == 0 || len == 0 || len > 1500U) {
        printf("[tx_ramp] need count>=1 and 1<=len<=1500\r\n"); return -1;
    }
    static uint8_t rampbuf[1500] __attribute__((aligned(4)));
    for (uint32_t i = 0; i < len; i++) rampbuf[i] = (uint8_t)(i & 0xFF);

    uint32_t sent = 0, errs = 0;
    uint64_t t0 = mtimer_get_time_us();
    for (uint32_t i = 0; i < count; i++) {
        if (ipronet_diag_eth_tx(rampbuf, (uint16_t)len) == 0) sent++;
        else errs++;
    }
    ipronet_diag_tx_agg_flush();      /* push any partial batch left in the agg buffer */
    uint64_t dt = mtimer_get_time_us() - t0;
    if (dt == 0) dt = 1;
    uint64_t bytes = (uint64_t)sent * len;
    uint32_t mbps10 = (uint32_t)((bytes * 8ULL * 10ULL) / dt);
    printf("[tx_ramp] sent=%lu errs=%lu bytes=%lu in %lu.%03lus -> %lu.%lu Mbps\r\n",
           (unsigned long)sent, (unsigned long)errs, (unsigned long)bytes,
           (unsigned long)(dt / 1000000U), (unsigned long)((dt / 1000U) % 1000U),
           (unsigned long)(mbps10 / 10U), (unsigned long)(mbps10 % 10U));
    return 0;
}

/* CPU-load probe via an idle-hog counter (the prebuilt libfreertos.a has per-task
 * run-time accumulation compiled out, so uxTaskGetSystemState %% is unusable).
 * A task at priority 1 (just above IDLE) spins a counter; every real task
 * (rx_task/tcpip/udp_tx/shell are all higher prio) preempts it, so the hog only
 * runs in genuine idle time and does NOT steal CPU from real work -> its count
 * rate is proportional to the idle fraction. CPU busy%% = 100*(1 - rate/idle_rate),
 * where idle_rate is the max rate seen (measure once with no traffic first). */
static volatile uint32_t s_cpuhog = 0;
static uint32_t s_cpuhog_max_rate = 0;   /* calibrated idle baseline (counts/s) */
static TaskHandle_t s_cpuhog_task = NULL;

static void cpuhog_task(void *arg)
{
    (void)arg;
    for (;;) {
        s_cpuhog++;
    }
}

static int cmd_ipro6s_cpu(int argc, char **argv)
{
    uint32_t secs = (argc >= 2) ? (uint32_t)strtoul(argv[1], NULL, 0) : 5U;
    if (secs == 0) secs = 5U;
    if (s_cpuhog_task == NULL) {
        /* prio 1 = just above IDLE(0); below every real task. */
        if (xTaskCreate(cpuhog_task, "cpuhog", 512, NULL, tskIDLE_PRIORITY + 1,
                        &s_cpuhog_task) != pdPASS) {
            printf("cpu: hog spawn fail\r\n"); return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(200));   /* let it start */
    }
    uint32_t c0 = s_cpuhog;
    vTaskDelay(pdMS_TO_TICKS(secs * 1000U));
    uint32_t rate = (s_cpuhog - c0) / secs;
    if (rate > s_cpuhog_max_rate) s_cpuhog_max_rate = rate;   /* track idle baseline */
    uint32_t busy10 = 0;
    if (s_cpuhog_max_rate > 0) {
        uint32_t b = (uint32_t)(((uint64_t)(s_cpuhog_max_rate - rate) * 1000U) / s_cpuhog_max_rate);
        busy10 = (rate >= s_cpuhog_max_rate) ? 0U : b;
    }
    printf("[cpu] %lus: hog_rate=%lu/s idle_base=%lu/s -> CPU busy ~%lu.%01lu%%\r\n",
           (unsigned long)secs, (unsigned long)rate, (unsigned long)s_cpuhog_max_rate,
           (unsigned long)(busy10 / 10U), (unsigned long)(busy10 % 10U));
    return 0;
}

static int cmd_ipro6s_cccr(int argc, char **argv)
{
    (void)argc; (void)argv;
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }

    printf("CCCR dump (F0, 0x00..0x16):\r\n");
    for (uint32_t a = 0x00; a <= 0x16; a++) {
        uint8_t v = 0;
        SDIO_Status_Type st = sdio_readb(host, SDIO_FUNC_0, a, &v);
        if (st != Status_SDIO_Success) {
            printf("  0x%02lX: ERR %d\r\n", (unsigned long)a, (int)st);
            return -1;
        }
        printf("  0x%02lX: 0x%02X\r\n", (unsigned long)a, v);
    }
    return 0;
}

static int cmd_ipro6s_func1(int argc, char **argv)
{
    (void)argc; (void)argv;
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL || host->num_funcs == 0) {
        printf("run ipro6s_enum first\r\n"); return -1;
    }
    SDIO_Func_Info_Type *fi = &host->funcs[1];
    printf("F1 info: enabled=%u  cur_blk=%u  max_blk=%u\r\n",
           fi->enabled, fi->cur_blk_size, fi->max_blk_size);
    printf("         class=0x%02X manf=0x%04X prod=0x%04X cis=0x%06lX\r\n",
           fi->func_class, fi->manf_id, fi->prod_id,
           (unsigned long)fi->cis_ptr);
    return 0;
}

static int cmd_ipro6s_readb(int argc, char **argv)
{
    if (argc < 3) { printf("Usage: ipro6s_readb <func> <addr_hex>\r\n"); return -1; }
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }
    uint8_t func  = (uint8_t)atoi(argv[1]);
    uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 0);
    uint8_t v = 0;
    SDIO_Status_Type st = sdio_readb(host, func, addr, &v);
    if (st != Status_SDIO_Success) {
        printf("readb F%u[0x%lX] ERR %d\r\n", func, (unsigned long)addr, (int)st);
        return -1;
    }
    printf("readb F%u[0x%lX] = 0x%02X\r\n", func, (unsigned long)addr, v);
    return 0;
}

static int cmd_ipro6s_writeb(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: ipro6s_writeb <func> <addr_hex> <val_hex>\r\n");
        return -1;
    }
    SDIO_Host_Type *host = ipronet_diag_host();
    if (host == NULL) { printf("run ipro6s_init first\r\n"); return -1; }
    uint8_t func  = (uint8_t)atoi(argv[1]);
    uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 0);
    uint8_t val   = (uint8_t)strtoul(argv[3], NULL, 0);
    SDIO_Status_Type st = sdio_writeb(host, func, addr, val);
    printf("writeb F%u[0x%lX] = 0x%02X -> %d\r\n",
           func, (unsigned long)addr, val, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

/* Generic 32-bit MMIO read (debug — e.g. SDH/ECLIC registers). */
static int cmd_ipro6s_peek(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: ipro6s_peek <addr_hex>\r\n"); return -1; }
    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 0);
    uint32_t val  = *(volatile uint32_t *)addr;
    printf("[0x%08lX] = 0x%08lX\r\n", (unsigned long)addr, (unsigned long)val);
    return 0;
}

/* Generic 32-bit MMIO write (debug). */
static int cmd_ipro6s_poke(int argc, char **argv)
{
    if (argc < 3) { printf("Usage: ipro6s_poke <addr_hex> <val_hex>\r\n"); return -1; }
    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 0);
    uint32_t val  = (uint32_t)strtoul(argv[2], NULL, 0);
    *(volatile uint32_t *)addr = val;
    printf("[0x%08lX] <- 0x%08lX (rd 0x%08lX)\r\n", (unsigned long)addr,
           (unsigned long)val, (unsigned long)*(volatile uint32_t *)addr);
    return 0;
}

/* Dump SDH controller interrupt registers — root-causing why SDH_INT_CARD
 * (bit 8 of NORMAL_INT_STATUS) never fires for the IPRO6S DAT1 in-band SDIO
 * interrupt. SDH_BASE = 0x30085000. Decisive: with the device asserting (RD
 * bitmap full, host NOT draining) is CARD_INT latched in NIS(0x30) bit 8?
 *   set   -> controller detects DAT1 but doesn't signal CPU (ECLIC/signal-en)
 *   clear -> controller never detects the DAT1 card interrupt (clock/mode) */
static int cmd_ipro6s_sdh_int(int argc, char **argv)
{
    (void)argc; (void)argv;
    const uint32_t base = 0x30085000U;
    uint32_t present = *(volatile uint32_t *)(base + 0x24);
    uint16_t clk     = *(volatile uint16_t *)(base + 0x2C);
    uint32_t nis     = *(volatile uint32_t *)(base + 0x30); /* normal lo16 + err hi16 */
    uint32_t nis_en  = *(volatile uint32_t *)(base + 0x34);
    uint32_t nis_sig = *(volatile uint32_t *)(base + 0x38);
    printf("SDH int regs (base 0x%08lX):\r\n", (unsigned long)base);
    printf("  present_state(0x24) = 0x%08lX\r\n", (unsigned long)present);
    printf("  clk_control  (0x2C) = 0x%04X\r\n", clk);
    printf("  norm_int_stat(0x30) = 0x%08lX  CARD_INT(bit8)=%d\r\n",
           (unsigned long)nis, (int)((nis >> 8) & 1));
    printf("  nis_status_en(0x34) = 0x%08lX  bit8=%d\r\n",
           (unsigned long)nis_en, (int)((nis_en >> 8) & 1));
    printf("  nis_signal_en(0x38) = 0x%08lX  bit8=%d\r\n",
           (unsigned long)nis_sig, (int)((nis_sig >> 8) & 1));
    return 0;
}

/* ------------------------------------------------------------------ */
/* lwIP TCP knobs (bench)                                              */
/* ------------------------------------------------------------------ */

/* Rexmit RTO floor for lwIP's _LP_TCP_OPTIMIZE path, in 50 ms slow-timer ticks
 * (TCP_TMR_INTERVAL is 25 here). The fork pins it at 1 tick and never backs off,
 * which is below this link's RTT jitter (6.7 ms min, 217 ms max measured), so
 * every expiry resends the whole unacked window. Runtime so a leg is a console
 * write instead of a reflash. */
static int cmd_tcprto(int argc, char **argv)
{
    extern s16_t lp_tcp_rexmit_rto_ticks;
    extern u8_t  lp_tcp_dupack_thresh;

    if (argc >= 2) {
        int n = atoi(argv[1]);
        if (n < 1)   n = 1;
        if (n > 200) n = 200;
        lp_tcp_rexmit_rto_ticks = (s16_t)n;
    }
    if (argc >= 3) {
        int d = atoi(argv[2]);
        if (d < 1)  d = 1;
        if (d > 16) d = 16;
        lp_tcp_dupack_thresh = (u8_t)d;
    }
    printf("tcprto ticks=%d (%d ms) dupack=%d\r\n", (int)lp_tcp_rexmit_rto_ticks,
           (int)lp_tcp_rexmit_rto_ticks * (2 * TCP_TMR_INTERVAL),
           (int)lp_tcp_dupack_thresh);
    return 0;
}

/* Congestion state of the active TCP connections, read from the sender itself.
 *
 * Everything about the uplink deficit has so far been inferred from a capture
 * at the receiver, which can only show what arrived. It cannot distinguish a
 * sender that is congestion-limited (cwnd small) from one that is window- or
 * application-limited (cwnd large, nothing queued) -- and those want opposite
 * fixes. Sample this during a run: cwnd against ssthresh says which regime the
 * connection is in, and snd_queuelen against the send buffer says whether
 * there was even anything to send. */
static int cmd_tcpstat(int argc, char **argv)
{
    struct tcp_pcb *pcb;
    int n = 0;

    (void)argc; (void)argv;
    for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
        printf("tcp[%d] state=%d cwnd=%u(%u mss) ssth=%u snd_wnd=%u "
               "unacked=%u unsent=%u qlen=%u rto=%d nrtx=%u dupacks=%u "
               "sa=%d sv=%d\r\n",
               n++, (int)pcb->state,
               (unsigned)pcb->cwnd, (unsigned)(pcb->cwnd / (pcb->mss ? pcb->mss : 1)),
               (unsigned)pcb->ssthresh, (unsigned)pcb->snd_wnd,
               (unsigned)(pcb->snd_nxt - pcb->lastack),
               (unsigned)(pcb->unsent ? 1 : 0),
               (unsigned)pcb->snd_queuelen,
               (int)pcb->rto, (unsigned)pcb->nrtx, (unsigned)pcb->dupacks,
               (int)pcb->sa, (int)pcb->sv);
    }
    if (n == 0) {
        printf("tcpstat: no active pcb\r\n");
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_ipronet_up,      ipronet_up,      "Customer path: ipronet_start(EVB cfg) [+ ipronet_connect <ssid> <psk>]");
SHELL_CMD_EXPORT_ALIAS(cmd_ipronet_down,    ipronet_down,    "ipronet_stop()");
SHELL_CMD_EXPORT_ALIAS(cmd_tcpstat,         tcpstat,         "Congestion state of active TCP pcbs (cwnd/ssthresh/in-flight/rto)");
SHELL_CMD_EXPORT_ALIAS(cmd_tcprto,          tcprto,          "lwIP _LP_TCP_OPTIMIZE tuning: <rto_slow_ticks 1..200> [dupack_thresh 1..16]");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_init,     ipro6s_init,     "Init SDH pinmux + sdio host");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_enum,     ipro6s_enum,     "Enumerate + check IPRO6S vendor/device");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_setup,    ipro6s_setup,    "4-bit / F1 enable / F1 blk=512; arg 'hs' = SDR50 50 MHz (default 25)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_mac,      ipro6s_mac,      "Print cached GET_MAC handshake MAC (run ipro6s_get_mac first)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_irq_on,   ipro6s_irq_on,   "claim_irq(F1) + enable_irq (ECLIC + INT_SOURCE)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_irq_wait, ipro6s_irq_wait, "Wait once on SDH card-int sem [ms=5000]");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_rx_start, ipro6s_rx_start, "Spawn rx_task (blocks on sdio_wait_irq, dispatches to F1 handler)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_rx_stats, ipro6s_rx_stats, "Print RX counters");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_lwipstat, ipro6s_lwipstat, "lwIP pool/proto census (where silently-dropped RX datagrams go)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_rxprof,   ipro6s_rxprof,   "rx_task per-stage us census; arg 1 = reset after printing");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_rxwait,   ipro6s_rxwait,   "rx_task CMD53 wait A/B: <1=spin at IDLE+1 (default)|0=spin at top prio>");
#if ICMP_DEST_UNREACH_RATE_LIMIT
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_icmplim,  ipro6s_icmplim,  "ICMP dest-unreach cap A/B: <msgs/s, 0=unlimited>; also prints suppressed");
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_send_hello, ipro6s_send_hello, "Send IPRO_CMD_HELLO sentinel to engage DUT bridge");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_get_mac,  ipro6s_get_mac,  "GET_MAC handshake: learn device STA MAC -> iproeth0 (needs rx_task)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_doorbell,  ipro6s_doorbell,  "One-shot: read the SMID upload doorbell (pending transfer size)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_flood_rx,  ipro6s_flood_rx,  "RX ceiling test: drain doorbell for [secs=10], no dispatch, print Mbps");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_tx_ramp,   ipro6s_tx_ramp,   "TX test: send <count> DATA frames of <len> ramp bytes; print Mbps");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_aggmax,    ipro6s_aggmax,    "TX aggregation batch cap: <N> (0=bypass aggregation)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_zcen,      ipro6s_zcen,      "Zero-copy opt-in: 1=single-frame, 2=TX stride, 3=TX+RX stride");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_girq,      ipro6s_girq,      "Data-ready IRQ A/B: <1=on|0=off>, no-arg=stats (level/count/mask)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_cpu,       ipro6s_cpu,       "Per-task CPU %% over <secs> (run during a test: IDLE ~0%%=CPU-bound)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_net_up,    ipro6s_net_up,    "Phase E: bring up iproeth0 lwIP netif (static IP)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_scan,        ipro6s_scan,        "Control: scan WiFi APs (host-driven), list prints async");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_scan_last,   ipro6s_scan_last,   "Control: print the last scan result from the shell task");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_connect,     ipro6s_connect,     "Control: ipro6s_connect <ssid> <psk> (host drives device assoc + DHCP)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_reconn,      ipro6s_reconn,      "Auto-reconnect on link loss: ipro6s_reconn [0|1] (default 1, armed by connect)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_devauto,     ipro6s_devauto,     "Device's own autoconnect: ipro6s_devauto <0|1> (connect sends 0 by itself)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_pace,        ipro6s_pace,        "Egress pacer: ipro6s_pace [kbps] (default 16000, 0=off; sheds device-killing overload)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_disconnect,  ipro6s_disconnect,  "Control: disconnect STA from AP");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_link_status, ipro6s_link_status, "Control: query STA link status");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_tx_stats,  ipro6s_tx_stats,  "Print TX counters (writes, agg frames, bytes, errors)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_cccr,     ipro6s_cccr,     "Dump CCCR 0x00..0x16");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_func1,    ipro6s_func1,    "Dump F1 metadata");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_readb,    ipro6s_readb,    "CMD52 read: <func> <addr_hex>");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_writeb,   ipro6s_writeb,   "CMD52 write: <func> <addr> <val>");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_peek,     ipro6s_peek,     "32-bit MMIO read: <addr_hex>");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_poke,     ipro6s_poke,     "32-bit MMIO write: <addr_hex> <val_hex>");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_sdh_int,  ipro6s_sdh_int,  "Dump SDH int regs (CARD_INT root-cause)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_cmd53_read,    ipro6s_cmd53_read,    "CMD53 byte-mode read F0 0x00..0x0F (data engine probe)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_cmd53_blkread, ipro6s_cmd53_blkread, "CMD53 block-mode read F1 FIFO (1x512 B) — raw SDH engine probe");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_cmd53_blkwrite,  ipro6s_cmd53_blkwrite,  "CMD53 block-mode WRITE F1 FIFO (1x512 B) — raw SDH engine probe");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_set_poll_timeout, ipro6s_set_poll_timeout, "Set SDH data poll timeout ms (LA: lower so wedging writes return fast)");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_cmd53_blkwrite_spam, ipro6s_cmd53_blkwrite_spam, "Spam CMD53 512B writes [count=200] for trigger-less LA capture");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_cmd53_blkread8,  ipro6s_cmd53_blkread8,  "CMD53 block-mode read F1 FIFO (4x512 B) — raw SDH engine probe");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_probe,    ipro6s_probe,    "Full SMID bring-up: ipronet_start() without a device power-cycle");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6_rst,        ipro6_rst,        "Power-cycle IPRO6 via GPIO15 SD 3.3V switch [off_ms=300]");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6_dl,         ipro6_dl,         "Power-cycle IPRO6 into BROM download (SD_D0/GPIO29 strap high) [off_ms=300]");
SHELL_CMD_EXPORT_ALIAS(cmd_ipro6_pwr,        ipro6_pwr,        "IPRO6 3.3V rail manual control: <0|1>");

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

static void main_task(void *arg)
{
    (void)arg;

    printf("\r\n=========================================\r\n");
    printf("  ipro7_sdio_wifi — ipronet_host example\r\n");
    printf("=========================================\r\n");
    printf("Device: ipro6_if_wifi auto-start FW, nothing to type there.\r\n");
    printf("Here:   ipronet_up <ssid> <psk>     (the customer path)\r\n");
    printf("   or:  ipro6s_probe / ipro6s_connect <ssid> <psk>   (step by step)\r\n");
    printf("=========================================\r\n");

    shell_init_with_task(UART0_INDEX);
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_setup_heap();
    platform_init(0);
    hal_boot2_init();

    xTaskCreate(main_task, "main", 4096, NULL,
                configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();

    while (1) ;
    return 0;
}
