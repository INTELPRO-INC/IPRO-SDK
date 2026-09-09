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
 * @file enc28j60_cli.c
 * @brief UART shell commands for ENC28J60 bring-up:
 *
 *   enc init [half|full]   reset + configure the chip (default: half-duplex)
 *   enc id                 read EREVID + PHY id + link (SPI sanity check)
 *   enc stat               EPKTCNT / EIR / ESTAT / link snapshot
 *   enc link               PHY link up/down
 *   enc reg <hex>          read one control register (packed addr, e.g. 0x72)
 *   enc phy <hex>          read one PHY register (raw addr, e.g. 0x11)
 *   enc prom <on|off>      promiscuous receive (accept every frame)
 *   enc tx [count]         send broadcast test frame(s)
 *   enc rx                 drain + dump all pending received frames
 *   enc mon [secs]         poll-and-dump received frames for N seconds
 */

#include "enc28j60.h"
#include "enc28j60_regs.h"

#include <FreeRTOS.h>
#include <task.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Locally-administered station MAC (bit 1 of first octet set). */
static const uint8_t s_mac[6] = { 0x02, 0x49, 0x50, 0x37, 0x00, 0x01 };
static bool s_inited;

static bool require_init(void)
{
    if (!s_inited) {
        printf("ENC28J60 not initialised — run `enc init` first.\n");
        return false;
    }
    return true;
}

static void dump_frame(const uint8_t *f, int len)
{
    printf("  RX %d bytes  dst=%02X:%02X:%02X:%02X:%02X:%02X"
           " src=%02X:%02X:%02X:%02X:%02X:%02X type=0x%02X%02X\n",
           len, f[0], f[1], f[2], f[3], f[4], f[5],
           f[6], f[7], f[8], f[9], f[10], f[11], f[12], f[13]);
    int n = len < 32 ? len : 32;
    printf("    ");
    for (int i = 0; i < n; i++) {
        printf("%02X ", f[i]);
    }
    printf("%s\n", len > 32 ? "..." : "");
}

static void cmd_init(const char *mode)
{
    bool full = (mode && strcmp(mode, "full") == 0);
    int rc = enc28j60_init(s_mac, full);
    if (rc == ENC28J60_OK) {
        s_inited = true;
        printf("init OK (rev=0x%02X, %s-duplex). PHY id=0x%08lX, link=%s\n",
               enc28j60_read_revid(), full ? "full" : "half",
               (unsigned long)enc28j60_read_phid(),
               enc28j60_link_up() ? "UP" : "down");
    } else {
        s_inited = false;
        printf("init FAILED (rc=%d) — check power + wiring (SCK19 MOSI16 "
               "MISO17 CS18 RST14 INT15).\n", rc);
    }
}

static void cmd_id(void)
{
    if (!require_init()) {
        return;
    }
    uint8_t  rev = enc28j60_read_revid();
    uint32_t pid = enc28j60_read_phid();
    printf("EREVID = 0x%02X  (expect 0x02..0x07 for a real chip)\n", rev);
    printf("PHY id = 0x%08lX  (expect 0x0083_14xx)\n", (unsigned long)pid);
    printf("link   = %s\n", enc28j60_link_up() ? "UP" : "down");
}

static void cmd_stat(void)
{
    if (!require_init()) {
        return;
    }
    printf("EPKTCNT=%u  EIR=0x%02X  ESTAT=0x%02X  ECON1=0x%02X  link=%s\n",
           enc28j60_pktcnt(),
           enc28j60_read_reg(ENC_EIR),
           enc28j60_read_reg(ENC_ESTAT),
           enc28j60_read_reg(ENC_ECON1),
           enc28j60_link_up() ? "UP" : "down");
}

static void cmd_tx(int count)
{
    if (!require_init()) {
        return;
    }
    uint8_t frame[64];
    memset(frame, 0, sizeof(frame));
    memset(frame, 0xFF, 6);                 /* broadcast destination */
    memcpy(frame + 6, s_mac, 6);            /* source */
    frame[12] = 0x88;                       /* experimental EtherType 0x88B5 */
    frame[13] = 0xB5;
    static const char msg[] = "IPRO7 ENC28J60 raw TX test";
    memcpy(frame + 14, msg, sizeof(msg) - 1);
    uint16_t len = (uint16_t)(14 + sizeof(msg) - 1);   /* MAC pads to 60 */

    for (int i = 0; i < count; i++) {
        int rc = enc28j60_send(frame, len);
        printf("tx #%d: %s (rc=%d)\n", i + 1,
               rc == ENC28J60_OK ? "sent" : "FAILED", rc);
        if (rc != ENC28J60_OK) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static int cmd_rx_drain(void)
{
    static uint8_t buf[ENC_MAX_FRAMELEN];
    int total = 0;
    int len;
    while ((len = enc28j60_recv(buf, sizeof(buf))) > 0) {
        dump_frame(buf, len);
        total++;
    }
    return total;
}

static void cmd_rx(void)
{
    if (!require_init()) {
        return;
    }
    int n = cmd_rx_drain();
    if (n == 0) {
        printf("no packets waiting (EPKTCNT=0)\n");
    } else {
        printf("drained %d packet(s)\n", n);
    }
}

static void cmd_mon(int secs)
{
    if (!require_init()) {
        return;
    }
    printf("monitoring for %d s (any key context-switch; Ctrl-C to stop shell)\n", secs);
    TickType_t end = xTaskGetTickCount() + pdMS_TO_TICKS(secs * 1000);
    int total = 0;
    while (xTaskGetTickCount() < end) {
        total += cmd_rx_drain();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    printf("monitor done: %d packet(s) total\n", total);
}

static void cmd_loop(void)
{
    static const uint8_t pat[8] = { 0x00, 0xFF, 0xA5, 0x5A, 0x12, 0x34, 0x55, 0xAA };
    uint8_t rx[8] = { 0 };
    printf("SPI loopback test: jumper GPIO16(MOSI) <-> GPIO17(MISO) first.\n");
    enc28j60_loopback(pat, rx, sizeof(pat));
    int match = 0;
    printf("  tx:");
    for (unsigned i = 0; i < sizeof(pat); i++) printf(" %02X", pat[i]);
    printf("\n  rx:");
    for (unsigned i = 0; i < sizeof(pat); i++) {
        printf(" %02X", rx[i]);
        if (rx[i] == pat[i]) match++;
    }
    printf("\n  %d/%u match -> ", match, (unsigned)sizeof(pat));
    if (match == (int)sizeof(pat)) {
        printf("PASS: SoC SPI works. Problem is chip-side (power/wiring/MISO).\n");
    } else {
        int allff = 1, all00 = 1;
        for (unsigned i = 0; i < sizeof(pat); i++) {
            if (rx[i] != 0xFF) allff = 0;
            if (rx[i] != 0x00) all00 = 0;
        }
        if (allff)      printf("all 0xFF: MISO never pulled by MOSI (jumper missing? swap? SPI not clocking).\n");
        else if (all00) printf("all 0x00: MISO stuck low.\n");
        else            printf("garbled: clocking but bit-shifted (mode/swap/clk too high).\n");
    }
}

static void cmd_pin(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: enc pin <gpio> [0|1]   (omit level to read)\n"
               "  drive a SoC pin so you can multimeter-check the wire to the\n"
               "  module: CS=18 SCK=19 MOSI/SI=16 RST=14 (SO/MISO=17 is input)\n");
        return;
    }
    uint8_t pin = (uint8_t)strtoul(argv[2], NULL, 0);
    if (argc >= 4) {
        int lvl = (argv[3][0] == '1');
        enc28j60_pin_out(pin, lvl);
        printf("GPIO%u driven %s — measure it at the module pin.\n", pin, lvl ? "HIGH (~3.3V)" : "LOW (0V)");
    } else {
        printf("GPIO%u reads %d\n", pin, enc28j60_pin_in(pin));
    }
}

static void cmd_swap(const char *arg)
{
    bool on = (arg && (arg[0] == '1' || strcmp(arg, "on") == 0));
    enc28j60_set_pin_swap(on);
    printf("MOSI/MISO swap = %d. Now run `enc init` (or `enc loop`) to apply.\n", on);
}

static void usage(void)
{
    printf("usage: enc <init|id|stat|link|reg|phy|prom|tx|rx|mon|swap|loop>\n"
           "  init [half|full]   reset + configure (default half)\n"
           "  id                 EREVID + PHY id + link\n"
           "  stat               EPKTCNT/EIR/ESTAT/ECON1/link\n"
           "  link               PHY link status\n"
           "  reg <hex>          read control register (packed, e.g. 0x72)\n"
           "  phy <hex>          read PHY register (raw, e.g. 0x11)\n"
           "  prom <on|off>      promiscuous receive\n"
           "  tx [count]         send broadcast test frame(s)\n"
           "  rx                 drain pending received frames\n"
           "  mon [secs]         poll + dump frames for N seconds\n"
           "  swap <0|1>         set MOSI/MISO swap, then re-run init\n"
           "  loop               SPI loopback self-test (bridge GPIO16<->17)\n"
           "  pin <gpio> [0|1]   drive/read a pin for multimeter wire checks\n");
}

static int cmd_enc(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        return 0;
    }
    const char *sub = argv[1];

    if (strcmp(sub, "init") == 0) {
        cmd_init(argc > 2 ? argv[2] : "half");
    } else if (strcmp(sub, "id") == 0) {
        cmd_id();
    } else if (strcmp(sub, "stat") == 0) {
        cmd_stat();
    } else if (strcmp(sub, "link") == 0) {
        if (require_init()) {
            printf("link = %s\n", enc28j60_link_up() ? "UP" : "down");
        }
    } else if (strcmp(sub, "reg") == 0) {
        if (require_init() && argc > 2) {
            uint8_t r = (uint8_t)strtoul(argv[2], NULL, 0);
            printf("reg[0x%02X] = 0x%02X\n", r, enc28j60_read_reg(r));
        } else if (argc <= 2) {
            printf("usage: enc reg <hex packed addr>\n");
        }
    } else if (strcmp(sub, "phy") == 0) {
        if (require_init() && argc > 2) {
            uint8_t a = (uint8_t)strtoul(argv[2], NULL, 0);
            printf("phy[0x%02X] = 0x%04X\n", a, enc28j60_phy_read(a));
        } else if (argc <= 2) {
            printf("usage: enc phy <hex addr>\n");
        }
    } else if (strcmp(sub, "prom") == 0) {
        if (require_init()) {
            bool on = (argc > 2 && strcmp(argv[2], "on") == 0);
            enc28j60_set_promiscuous(on);
            printf("promiscuous %s\n", on ? "ON" : "off");
        }
    } else if (strcmp(sub, "tx") == 0) {
        cmd_tx(argc > 2 ? atoi(argv[2]) : 1);
    } else if (strcmp(sub, "rx") == 0) {
        cmd_rx();
    } else if (strcmp(sub, "mon") == 0) {
        cmd_mon(argc > 2 ? atoi(argv[2]) : 5);
    } else if (strcmp(sub, "swap") == 0) {
        cmd_swap(argc > 2 ? argv[2] : "1");
    } else if (strcmp(sub, "loop") == 0) {
        cmd_loop();
    } else if (strcmp(sub, "pin") == 0) {
        cmd_pin(argc, argv);
    } else {
        usage();
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_enc, enc, ENC28J60 SPI Ethernet test);

#endif /* CONFIG_SHELL */
