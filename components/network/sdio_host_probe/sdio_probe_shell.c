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
 * sdio_probe_shell.c - the generic sdio_* command table.
 *
 * ipro_sdio_probe and ipro_sdio_host_probe each carried their own copy of this;
 * the host_probe one was the superset, so that is what survives here, with the
 * argument parsing standardised on strtoul (ipro_sdio_probe parsed the function
 * number with atoi, which quietly rejects the 0x form the addresses use).
 *
 * A module's own bring-up registers stay in that module's app: bind an
 * on_post_enable callback with sdio_probe_shell_bind() and sdio_probe drives it.
 */
/* Kconfig says SDIO_HOST_PROBE_SHELL depends on SDIO_HOST_PROBE_BUS && SHELL,
 * but per-app .config files are transcribed to -D flags without resolving
 * `depends on` (cmake/generate_autoconf.cmake). A hand-merged .config that sets
 * SHELL without BUS would otherwise fail with implicit-declaration noise from
 * halfway down this file; say what is actually wrong instead. */
#if !defined(CONFIG_SDIO_HOST_PROBE_BUS)
#error "CONFIG_SDIO_HOST_PROBE_SHELL requires CONFIG_SDIO_HOST_PROBE_BUS"
#endif
#if !defined(CONFIG_SHELL)
#error "CONFIG_SDIO_HOST_PROBE_SHELL requires CONFIG_SHELL"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"

#include "sdio_probe.h"

/* CMD53 scratch. ADMA2 requires 4-byte alignment; 8 blocks of 512 B. */
#define PROBE_XFER_BUF_BYTES    4096U

static uint8_t s_xfer_buf[PROBE_XFER_BUF_BYTES] __attribute__((aligned(4)));

static sdio_probe_config_t    s_cfg;
static sdio_probe_callbacks_t s_cb;
static int                    s_bound;

void sdio_probe_shell_bind(const sdio_probe_config_t *cfg,
                           const sdio_probe_callbacks_t *cb)
{
    if (cfg != NULL) {
        s_cfg = *cfg;
    } else {
        sdio_probe_config_default(&s_cfg);
    }
    if (cb != NULL) {
        s_cb = *cb;
    } else {
        memset(&s_cb, 0, sizeof(s_cb));
    }
    s_bound = 1;
}

/* The app normally binds at boot; fall back to the EVB defaults if it did not
 * so the commands still work on a board that matches them. */
static void ensure_bound(void)
{
    if (!s_bound) {
        sdio_probe_shell_bind(NULL, NULL);
    }
}

/* Every command below needs the host; centralise the "did you run sdio_init"
 * complaint so the table stays readable. */
static SDIO_Host_Type *need_host(void)
{
    SDIO_Host_Type *host = sdio_probe_host();
    if (host == NULL) {
        printf("run sdio_init first\r\n");
    }
    return host;
}

/* ------------------------------------------------------------------ */
/* Setup                                                               */
/* ------------------------------------------------------------------ */

static int cmd_sdio_pins(int argc, char **argv)
{
    ensure_bound();

    if (argc == 4) {
        if (sdio_probe_is_started()) {
            printf("sdio: pins are latched at sdio_init; reboot to change\r\n");
            return -1;
        }
        s_cfg.pin_power_en = (uint8_t)strtoul(argv[1], NULL, 0);
        s_cfg.pin_reset_n  = (uint8_t)strtoul(argv[2], NULL, 0);
        s_cfg.pin_wake     = (uint8_t)strtoul(argv[3], NULL, 0);
    } else if (argc != 1) {
        printf("Usage: sdio_pins [power_en reset_n wake]\r\n");
        return -1;
    }
    printf("sdio: POWER_EN=GPIO%u  RESET#=GPIO%u  WAKE=GPIO%u\r\n",
           s_cfg.pin_power_en, s_cfg.pin_reset_n, s_cfg.pin_wake);
    return 0;
}

/* sdio_init's body, so the full sequence can reuse it without faking argv. */
static int probe_ensure_started(void)
{
    if (sdio_probe_is_started()) {
        return 0;
    }
    ensure_bound();
    if (sdio_probe_start(&s_cfg, &s_cb) != SDIO_PROBE_OK) {
        printf("sdio: host_init fail (%d)\r\n", (int)sdio_probe_last_status());
        return -1;
    }
    printf("sdio: host_init OK (sd_id=0)\r\n");
    return 0;
}

static int cmd_sdio_init(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (sdio_probe_is_started()) {
        printf("sdio: already initialized\r\n");
        return 0;
    }
    return probe_ensure_started();
}

static int cmd_sdio_pwr(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: sdio_pwr <on|off>\r\n"); return -1; }

    int on;
    if (strcmp(argv[1], "on") == 0) {
        on = 1;
    } else if (strcmp(argv[1], "off") == 0) {
        on = 0;
    } else {
        printf("Usage: sdio_pwr <on|off>\r\n");
        return -1;
    }
    if (sdio_probe_power(on) != SDIO_PROBE_OK) {
        printf("run sdio_init first\r\n");
        return -1;
    }
    printf("sdio: module power %s\r\n", on ? "ON" : "OFF");
    return 0;
}

static int cmd_sdio_reset(int argc, char **argv)
{
    uint32_t low_ms  = (argc > 1) ? (uint32_t)strtoul(argv[1], NULL, 0) : 0;
    uint32_t post_ms = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0;

    if (sdio_probe_reset(low_ms, post_ms) != SDIO_PROBE_OK) {
        printf("run sdio_init first\r\n");
        return -1;
    }
    printf("sdio: reset pulse done\r\n");
    return 0;
}

static int cmd_sdio_cycle(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (sdio_probe_power_cycle() != SDIO_PROBE_OK) {
        printf("run sdio_init first\r\n");
        return -1;
    }
    printf("sdio: module power-cycled\r\n");
    return 0;
}

/* Reaches sdio_probe_stop() and, on the next sdio_init, the sdio_host_reinit()
 * branch of sdio_probe_start(). Without this command both are unreachable, and
 * the leak sdio_host_reinit() exists to avoid (hal_sdio.h: sdio_host_init()
 * allocates a mutex and a semaphore with no destructor) is never actually
 * exercised on a bench that re-enumerates repeatedly. */
static int cmd_sdio_stop(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (!sdio_probe_is_started()) {
        printf("sdio: not started\r\n");
        return 0;
    }
    (void)sdio_probe_stop();
    printf("sdio: stopped (module powered off; run sdio_init to restart)\r\n");
    return 0;
}

static int cmd_sdio_enum(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (need_host() == NULL) { return -1; }

    if (sdio_probe_enumerate() != SDIO_PROBE_OK) {
        printf("sdio: enumerate fail (%d)\r\n", (int)sdio_probe_last_status());
        return -1;
    }
    printf("sdio: enumerate OK\r\n");
    sdio_probe_dump_enum();
    return 0;
}

static int cmd_sdio_width(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: sdio_width <1|4>\r\n"); return -1; }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    unsigned long w = strtoul(argv[1], NULL, 0);
    if (w != 1 && w != 4) { printf("Usage: sdio_width <1|4>\r\n"); return -1; }

    uint8_t code = (w == 4) ? SDIO_BUS_WIDTH_4BIT : SDIO_BUS_WIDTH_1BIT;
    SDIO_Status_Type st = sdio_set_bus_width(host, code);
    printf("sdio: set_bus_width(%lu) -> %d\r\n", w, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

static int cmd_sdio_clk(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: sdio_clk <hz>\r\n"); return -1; }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint32_t hz = (uint32_t)strtoul(argv[1], NULL, 0);
    SDIO_Status_Type st = sdio_set_clock(host, hz);
    printf("sdio: set_clock(%lu) -> %d\r\n", (unsigned long)hz, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

static int cmd_sdio_hs(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: sdio_hs <0|1>\r\n"); return -1; }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    bool en = strtoul(argv[1], NULL, 0) != 0;
    SDIO_Status_Type st = sdio_set_high_speed(host, en);
    printf("sdio: set_high_speed(%d) -> %d\r\n", (int)en, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

static int cmd_sdio_blksz(int argc, char **argv)
{
    if (argc < 3) { printf("Usage: sdio_blksz <func> <bytes>\r\n"); return -1; }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint8_t func = (uint8_t)strtoul(argv[1], NULL, 0);
    uint16_t sz  = (uint16_t)strtoul(argv[2], NULL, 0);
    SDIO_Status_Type st = sdio_set_block_size(host, func, sz);
    printf("sdio: set_block_size(F%u, %u) -> %d\r\n", func, sz, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

static int cmd_sdio_en(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: sdio_en <func>\r\n"); return -1; }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint8_t func = (uint8_t)strtoul(argv[1], NULL, 0);
    SDIO_Status_Type st = sdio_enable_func(host, func);
    printf("sdio: enable_func(F%u) -> %d\r\n", func, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

static int cmd_sdio_dis(int argc, char **argv)
{
    if (argc < 2) { printf("Usage: sdio_dis <func>\r\n"); return -1; }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint8_t func = (uint8_t)strtoul(argv[1], NULL, 0);
    SDIO_Status_Type st = sdio_disable_func(host, func);
    printf("sdio: disable_func(F%u) -> %d\r\n", func, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

/* ------------------------------------------------------------------ */
/* Inspection                                                          */
/* ------------------------------------------------------------------ */

static int cmd_sdio_cccr(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (need_host() == NULL) { return -1; }
    return sdio_probe_dump_cccr() == SDIO_PROBE_OK ? 0 : -1;
}

static int cmd_sdio_cis(int argc, char **argv)
{
    if (need_host() == NULL) { return -1; }
    uint8_t func = (argc > 1) ? (uint8_t)strtoul(argv[1], NULL, 0) : SDIO_FUNC_0;
    return sdio_probe_dump_cis(func) == SDIO_PROBE_OK ? 0 : -1;
}

static int cmd_sdio_func(int argc, char **argv)
{
    if (need_host() == NULL) { return -1; }
    uint8_t func = (argc > 1) ? (uint8_t)strtoul(argv[1], NULL, 0) : 1;
    return sdio_probe_dump_func(func) == SDIO_PROBE_OK ? 0 : -1;
}

/* ------------------------------------------------------------------ */
/* Transfers                                                           */
/* ------------------------------------------------------------------ */

static int cmd_sdio_readb(int argc, char **argv)
{
    if (argc < 3) { printf("Usage: sdio_readb <func> <addr_hex>\r\n"); return -1; }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint8_t func  = (uint8_t)strtoul(argv[1], NULL, 0);
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

static int cmd_sdio_writeb(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: sdio_writeb <func> <addr_hex> <val_hex>\r\n");
        return -1;
    }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint8_t func  = (uint8_t)strtoul(argv[1], NULL, 0);
    uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 0);
    uint8_t val   = (uint8_t)strtoul(argv[3], NULL, 0);
    SDIO_Status_Type st = sdio_writeb(host, func, addr, val);
    printf("writeb F%u[0x%lX] = 0x%02X -> %d\r\n",
           func, (unsigned long)addr, val, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

static int cmd_sdio_rdbyte(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: sdio_rdbyte <func> <addr_hex> <len> [fixed]\r\n");
        return -1;
    }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint8_t func  = (uint8_t)strtoul(argv[1], NULL, 0);
    uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 0);
    uint32_t len  = (uint32_t)strtoul(argv[3], NULL, 0);
    bool fixed    = (argc > 4) && strtoul(argv[4], NULL, 0) != 0;

    if (len == 0 || len > PROBE_XFER_BUF_BYTES || len > 512) {
        printf("sdio: len must be 1..512\r\n");
        return -1;
    }
    memset(s_xfer_buf, 0, len);
    SDIO_Status_Type st = sdio_read_bytes(host, func, addr, s_xfer_buf,
                                          (uint16_t)len, fixed);
    printf("rdbyte F%u[0x%lX] %lu B fixed=%d -> %d\r\n", func,
           (unsigned long)addr, (unsigned long)len, (int)fixed, (int)st);
    if (st != Status_SDIO_Success) { return -1; }
    sdio_probe_hexdump(s_xfer_buf, len);
    return 0;
}

/* Shared bounds check for the two block-mode commands. Returns the byte count
 * or 0 after printing why the request does not fit. */
static uint32_t blk_xfer_bytes(SDIO_Host_Type *host, uint8_t func, uint32_t blks)
{
    if (func > SDIO_MAX_FUNCS) {
        printf("sdio: F%u out of range\r\n", func);
        return 0;
    }
    uint32_t blk_sz = host->funcs[func].cur_blk_size;
    if (blk_sz == 0) {
        printf("sdio: F%u block size not set - run sdio_blksz first\r\n", func);
        return 0;
    }
    if (blks == 0 || blks * blk_sz > PROBE_XFER_BUF_BYTES) {
        printf("sdio: blocks must be 1..%lu at blk_sz=%lu\r\n",
               (unsigned long)(PROBE_XFER_BUF_BYTES / blk_sz),
               (unsigned long)blk_sz);
        return 0;
    }
    return blks * blk_sz;
}

static int cmd_sdio_rdblk(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: sdio_rdblk <func> <addr_hex> <blocks> [fixed]\r\n");
        return -1;
    }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint8_t func  = (uint8_t)strtoul(argv[1], NULL, 0);
    uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 0);
    uint32_t blks = (uint32_t)strtoul(argv[3], NULL, 0);
    bool fixed    = (argc > 4) && strtoul(argv[4], NULL, 0) != 0;

    uint32_t bytes = blk_xfer_bytes(host, func, blks);
    if (bytes == 0) { return -1; }
    uint32_t blk_sz = bytes / blks;

    memset(s_xfer_buf, 0, bytes);
    SDIO_Status_Type st = sdio_read_blocks(host, func, addr, s_xfer_buf,
                                           (uint16_t)blks, fixed);
    printf("rdblk F%u[0x%lX] %lu x %lu B fixed=%d -> %d\r\n", func,
           (unsigned long)addr, (unsigned long)blks, (unsigned long)blk_sz,
           (int)fixed, (int)st);
    if (st != Status_SDIO_Success) { return -1; }
    /* First block only - enough to see whether the data engine is sane. */
    sdio_probe_hexdump(s_xfer_buf, blk_sz > 64 ? 64 : blk_sz);
    return 0;
}

static int cmd_sdio_wrblk(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: sdio_wrblk <func> <addr_hex> <blocks> [fixed]\r\n");
        return -1;
    }
    SDIO_Host_Type *host = need_host();
    if (host == NULL) { return -1; }

    uint8_t func  = (uint8_t)strtoul(argv[1], NULL, 0);
    uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 0);
    uint32_t blks = (uint32_t)strtoul(argv[3], NULL, 0);
    bool fixed    = (argc > 4) && strtoul(argv[4], NULL, 0) != 0;

    uint32_t bytes = blk_xfer_bytes(host, func, blks);
    if (bytes == 0) { return -1; }
    uint32_t blk_sz = bytes / blks;

    /* Ramp pattern: a wrong bus width or a bad sample phase shows up as a
     * shifted or duplicated ramp far more clearly than constant data. */
    for (uint32_t i = 0; i < bytes; i++) {
        s_xfer_buf[i] = (uint8_t)i;
    }
    SDIO_Status_Type st = sdio_write_blocks(host, func, addr, s_xfer_buf,
                                            (uint16_t)blks, fixed);
    printf("wrblk F%u[0x%lX] %lu x %lu B fixed=%d -> %d\r\n", func,
           (unsigned long)addr, (unsigned long)blks, (unsigned long)blk_sz,
           (int)fixed, (int)st);
    return st == Status_SDIO_Success ? 0 : -1;
}

/* ------------------------------------------------------------------ */
/* Full sequence                                                       */
/* ------------------------------------------------------------------ */

static int cmd_sdio_probe(int argc, char **argv)
{
    (void)argc; (void)argv;

    printf("\r\n=== sdio_probe - full sequence ===\r\n");

    if (probe_ensure_started() != 0) {
        return -1;
    }
    if (sdio_probe_run() != SDIO_PROBE_OK) {
        printf("=== probe FAIL (sdio status %d) ===\r\n",
               (int)sdio_probe_last_status());
        return -1;
    }
    const sdio_probe_config_t *cfg = sdio_probe_config();

    sdio_probe_dump_enum();
    (void)sdio_probe_dump_cccr();
    (void)sdio_probe_dump_cis(SDIO_FUNC_0);
    (void)sdio_probe_dump_func(cfg != NULL ? cfg->data_func : SDIO_FUNC_1);

    printf("=== probe OK - SDIO transport verified ===\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_sdio_pins,   sdio_pins,   "Show/set module ctrl GPIOs: [pwr rst wake]");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_init,   sdio_init,   "Init SDH pinmux + sdio host");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_pwr,    sdio_pwr,    "Drive module POWER_EN <on|off>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_reset,  sdio_reset,  "Pulse module RESET# [low_ms] [post_ms]");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_cycle,  sdio_cycle,  "Power-cycle the module (clears wedged firmware)");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_stop,   sdio_stop,   "Drop card state + power off; sdio_init re-enters via reinit");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_enum,   sdio_enum,   "Run SDIO card enumeration");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_width,  sdio_width,  "Set bus width: <1|4>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_clk,    sdio_clk,    "Set bus clock: <hz>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_hs,     sdio_hs,     "High-speed mode: <0|1>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_blksz,  sdio_blksz,  "Set block size: <func> <bytes>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_en,     sdio_en,     "IO Enable a function: <func>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_dis,    sdio_dis,    "IO Disable a function: <func>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_cccr,   sdio_cccr,   "Dump CCCR 0x00..0x16");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_cis,    sdio_cis,    "Walk the CIS tuple chain: [func]");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_func,   sdio_func,   "Dump function metadata: <n>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_readb,  sdio_readb,  "CMD52 read: <func> <addr_hex>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_writeb, sdio_writeb, "CMD52 write: <func> <addr> <val>");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_rdbyte, sdio_rdbyte, "CMD53 byte read: <func> <addr> <len> [fixed]");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_rdblk,  sdio_rdblk,  "CMD53 block read: <func> <addr> <blks> [fixed]");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_wrblk,  sdio_wrblk,  "CMD53 block write: <func> <addr> <blks> [fixed]");
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_probe,  sdio_probe,  "Full bring-up sequence");
