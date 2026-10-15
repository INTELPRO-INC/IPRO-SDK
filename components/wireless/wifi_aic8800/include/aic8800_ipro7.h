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
 * AIC8800 D80 over SDIO — public bring-up API for IPRO7 apps.
 *
 * Everything here is implemented inside aic8800_release/lib/libsoftutil-aic_wifi.a.
 * The full Wi-Fi API (wifi_api.h, netif_api.h, ...) stays behind
 * AIC8800_VENDOR_INCLUDE_DIRS; Bluetooth is a separate, transport-only interface
 * in the vendor's own public headers, which this component also puts on the
 * include path:
 *
 *   #include "aic_hci_mailbox.h"     aic_btsdio_init() / _ready() / _deinit()
 *   #include "aic_hci_transport.h"   attach / send / detach, HCI_FLOW_HOST
 *
 * The HCI transport carries Command / Event / ACL only — no SCO/eSCO/ISO, no
 * codec. The Bluetooth host stack is the application's.
 */
#ifndef AIC8800_IPRO7_H
#define AIC8800_IPRO7_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Bring-up --------------------------------------------------------------
 * lwIP must already be running (tcpip_init) before aic_wifi_demo_test().
 * It powers the module (WIFI_PWR_EN = GPIO16), enumerates the SDIO card on the
 * SDC pads GPIO28-33, downloads the firmware, creates the driver tasks, adds the
 * lwIP netif and records 192.168.13.1/24 as the driver's default interface
 * address (used when it brings up SoftAP). Blocking, takes a few seconds;
 * returns 0 on success. Call once.
 *
 * The 2026-10-02 release comes up in the vendor's conservative transport mode:
 * 1-bit reads AND writes at 10 MHz with single-block RX through PIO (it logs
 * "diagnostic v2 ... RX1/TX1 clock=10MHz"). The tuned 4-bit path is still here
 * but is now opt-in at run time — aic_ipro_sdio_set_rx_width(4, ...) or
 * aic_ipro_sdio_set_rx_phase(), plus aic_ipro_sdio_set_clock() — so expect the
 * default configuration to be slow until a board has been re-qualified. */
int aic_wifi_demo_test(void);

/* Forward one line to the AIC command interpreter (sta/ap/scan/iperf/perfmon/...).
 * Responses are printed with an "[aic-cmd]" prefix. */
void cmd_test(const char *command);

/* ---- SDIO transport tunables (all runtime, bus must be initialised) ---------
 * IPRO7's SDH card-interrupt line does not fire, so the shim polls CCCR
 * INT_PENDING: `ms` while traffic is active, 5 ms when idle. 1..20. */
int aic_ipro_sdio_set_poll_ms(unsigned int ms);
unsigned int aic_ipro_sdio_get_poll_ms(void);

/* Reads of at most `bytes` go 1-bit, larger reads 4-bit (512-byte multiple,
 * 512..8192). Default 4096. */
int aic_ipro_sdio_set_hybrid_max(unsigned int bytes);
unsigned int aic_ipro_sdio_get_hybrid_max(void);

/* 1 = every read AND write at 1-bit; 0 = writes 4-bit, reads per hybrid rule.
 * The release default is already fixed 1-bit, so this is the way back to it
 * after an experiment. */
int aic_ipro_sdio_set_fixed_1bit(unsigned int enable);

/* Per-line 4-bit read capture taps (0..3 each) + clock inversion / sampling-clock
 * select (0/1). Switching to 4-bit also turns on the hybrid rule and read-CRC
 * ignore. The tap values that were qualified on the fly-wire bring-up board are
 * clk=3 d0..d3=1/1/2/1 inv=1 samp=1; re-sweep them on a real PCB. */
int aic_ipro_sdio_set_rx_phase(unsigned int clk_tap,
                               unsigned int d0_tap, unsigned int d1_tap,
                               unsigned int d2_tap, unsigned int d3_tap,
                               unsigned int clk_inv, unsigned int samp_sel);
int aic_ipro_sdio_set_rx_width(unsigned int width, unsigned int tap,
                               unsigned int clk_inv, unsigned int samp_sel);
/* 10, 12, 16, 20, 24 or 25 MHz. */
int aic_ipro_sdio_set_clock(unsigned int mhz);

/* ---- Counters ---------------------------------------------------------- */
void aic_ipro_sdio_get_stats(uint32_t *rd_count, uint32_t *rd_bytes,
                             uint32_t *wr_count, uint32_t *wr_bytes,
                             uint32_t *rd_avg_us, uint32_t *wr_avg_us);
void aic_ipro_sdio_get_irq_stats(uint32_t *wake, uint32_t *timeout,
                                 uint32_t *poll_hit, uint32_t *raw_card);
void aic_ipro_sdio_get_poll_adapt_stats(uint32_t *fast, uint32_t *slow,
                                        uint32_t *active);
void aic_ipro_sdio_reset_stats(void);

#ifdef __cplusplus
}
#endif

#endif /* AIC8800_IPRO7_H */
