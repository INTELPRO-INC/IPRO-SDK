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
 * sdio_probe.h - shared IPRO7 SDIO host bring-up layer.
 *
 * IPRO7 drives its SDH controller as an SDIO *host*; every external module we
 * have brought up (AIC8800D80, ATBM6162, the IPRO6S ipronet bridge) starts from
 * the same three steps: put GPIO28-33 on the SDC function, sequence the module's
 * power/reset lines, then enumerate and negotiate the bus. This component owns
 * those steps so a new module's probe app is a pin table plus its own quirks.
 *
 * Two layers, separately selectable (see Kconfig):
 *
 *   BOARD  sdio_probe_board_init / _power / _reset / _power_cycle
 *          SDH pinmux + module control GPIOs. No card I/O, no host descriptor.
 *
 *   BUS    sdio_probe_start / _enumerate / _negotiate / _run / _stop
 *          Owns one SDIO_Host_Type and the CCCR/CIS/function dumps.
 *
 * IMPORTANT: select BOARD only (CONFIG_SDIO_HOST_PROBE_BUS=n) for modules whose
 * vendor driver calls sdio_host_init() itself - ATBM6162 keeps its own g_host in
 * atbm_os_sdio.c, and a second descriptor on the same controller would race it.
 *
 * Module-specific bring-up registers do NOT belong here. Issue them from the
 * on_post_enable callback, or from the app's own shell commands.
 *
 * Bus pin assignment is fixed by the IPRO7 SDH pinmux:
 *   GPIO28..33 = SDH bus (CMD / CLK / DAT0..3)  [GPIO_FUN_SDC]
 * The one pin that must NOT get a pull-up is GPIO30 (the host drives it; a host
 * pull-up fights a module-side keeper). Note the surviving disagreement in the
 * apps this was collected from: ipro_sdio_probe and ipro_sdio_host_probe called
 * GPIO30 the CMD line, ipro_atbm_probe called it CLK (carrier wiring confirmed
 * 2026-06-25). Both agree on which pin is bare, so the code is the same either
 * way - but do not quote either label as authoritative without the pinmux xlsx.
 */
#ifndef SDIO_PROBE_H
#define SDIO_PROBE_H

#include <stdint.h>
#include <stdbool.h>

#include "hal_sdio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return codes. Anything <0 is a failure; the SDIO_Status_Type behind an
 * SDIO_PROBE_ERR_IO is available from sdio_probe_last_status(). */
#define SDIO_PROBE_OK            0
#define SDIO_PROBE_ERR_ARG     (-1)   /* bad argument */
#define SDIO_PROBE_ERR_STATE   (-2)   /* called out of order */
#define SDIO_PROBE_ERR_IO      (-3)   /* the SDIO layer said no */

/** A board that has no such control line. */
#define SDIO_PROBE_PIN_NONE      0xFFU

/**
 * @brief Board wiring and the bus defaults sdio_probe_negotiate() applies.
 *
 * Fill with sdio_probe_config_default() and override what the carrier differs
 * on. Pins are latched by sdio_probe_board_init().
 */
typedef struct {
    /* Module control pins, or SDIO_PROBE_PIN_NONE. */
    uint8_t  pin_power_en;
    uint8_t  pin_reset_n;
    uint8_t  pin_wake;
    bool     power_active_high;   /*!< false = POWER_EN asserted low */
    bool     reset_active_low;    /*!< false = RESET asserted high */

    /* Sequencing, milliseconds. sdio_probe_power_cycle() reuses reset_low_ms
     * as its de-asserted window and reset_post_ms as its settle, so a board
     * with only an enable line (ATBM6162: CS on GPIO24, no reset) still shapes
     * its power cycle through these two. */
    uint16_t power_settle_ms;     /*!< wait after asserting power */
    uint16_t reset_low_ms;        /*!< reset held asserted / de-asserted window */
    uint16_t reset_post_ms;       /*!< wait after releasing reset / settle */

    /* Bus defaults. */
    uint8_t  data_func;           /*!< function carrying data, usually F1 */
    uint16_t block_size;
    uint32_t clock_hz;
    uint8_t  bus_width;           /*!< SDIO_BUS_WIDTH_1BIT / _4BIT */
    bool     high_speed;
} sdio_probe_config_t;

/**
 * @brief Hooks into the full sequence. Every field may be NULL.
 */
typedef struct {
    /**
     * Called after IO Enable of the data function and before the sequence
     * reports success - the place for module-specific bring-up registers
     * (AIC8800D80's FN0[0xF2]=0x7F and friends). Return 0 to continue, <0 to
     * fail the sequence.
     */
    int  (*on_post_enable)(SDIO_Host_Type *host, void *arg);

    /** Called once per step with that step's return code. Tracing only. */
    void (*on_step)(const char *step, int rc, void *arg);

    void *arg;
} sdio_probe_callbacks_t;

/** Fill cfg with the IPRO7 EVB defaults: PWR/RST/WAKE = GPIO16/17/18 active
 *  high/low, 5 ms reset, F1 @ 512 B, 4-bit, 25 MHz, high speed off. */
void sdio_probe_config_default(sdio_probe_config_t *cfg);

/** The latched configuration, or NULL before sdio_probe_board_init(). */
const sdio_probe_config_t *sdio_probe_config(void);

/* ------------------------------------------------------------------ */
/* Board layer                                                         */
/* ------------------------------------------------------------------ */

/**
 * @brief Latch cfg, drive the module control pins to their de-asserted state
 * (unpowered, in reset) and put GPIO28-33 on the SDC function + SDH clock.
 *
 * Idempotent; the pin assignment is latched by the first call.
 */
int sdio_probe_board_init(const sdio_probe_config_t *cfg);

/** @brief Assert(1)/de-assert(0) POWER_EN, honouring power_active_high, then
 *  wait power_settle_ms on assert. No-op if the board has no such pin. */
int sdio_probe_power(int on);

/** @brief Pulse RESET. 0 for either argument uses the configured default. */
int sdio_probe_reset(uint32_t low_ms, uint32_t post_ms);

/** @brief De-assert power and reset, wait reset_low_ms, re-assert and wait
 *  reset_post_ms. The only way to clear a module still running wedged firmware
 *  from a previous run; plain re-enumeration does not. */
int sdio_probe_power_cycle(void);

/** @brief Drive the optional WAKE line. */
int sdio_probe_wake(int level);

#ifdef CONFIG_SDIO_HOST_PROBE_BUS

/* ------------------------------------------------------------------ */
/* Bus layer                                                           */
/* ------------------------------------------------------------------ */

/** @brief sdio_probe_board_init() + sdio_host_init() on SDH controller 0.
 *  cb may be NULL. Idempotent. */
int sdio_probe_start(const sdio_probe_config_t *cfg,
                     const sdio_probe_callbacks_t *cb);

/** @brief Drop the card state and de-assert module power. The descriptor's RTOS
 *  objects are kept - sdio_host_init() has no destructor - so a following
 *  sdio_probe_start() re-enters through sdio_host_reinit() rather than leaking
 *  a mutex and a semaphore per cycle. sdio_probe_is_started() reads false in
 *  between. */
int sdio_probe_stop(void);

/** @brief CMD0/8/5/3/7 + CCCR + CIS. Leaves every function present-disabled. */
int sdio_probe_enumerate(void);

/** @brief Apply the configured bus width, clock, high speed and block size,
 *  then IO Enable the data function and run the on_post_enable callback. */
int sdio_probe_negotiate(void);

/** @brief power on -> reset -> enumerate -> negotiate, tracing each step. */
int sdio_probe_run(void);

bool             sdio_probe_is_started(void);
SDIO_Host_Type  *sdio_probe_host(void);
SDIO_Status_Type sdio_probe_last_status(void);

/* ------------------------------------------------------------------ */
/* Dumps (console)                                                     */
/* ------------------------------------------------------------------ */

void sdio_probe_dump_enum(void);            /*!< rca / funcs / ocr / CIS ptrs */
int  sdio_probe_dump_cccr(void);            /*!< F0 0x00..0x16 */
int  sdio_probe_dump_cis(uint8_t func);     /*!< walk + decode the tuple chain */
int  sdio_probe_dump_func(uint8_t func);    /*!< per-function metadata */
void sdio_probe_hexdump(const uint8_t *buf, uint32_t len);

#ifdef CONFIG_SDIO_HOST_PROBE_SHELL
/**
 * @brief Bind the config and callbacks the generic sdio_init / sdio_probe shell
 * commands pass to sdio_probe_start(). Call once at boot, before the shell can
 * reach them; without it those commands fall back to the EVB defaults and no
 * callbacks. Either argument may be NULL.
 */
void sdio_probe_shell_bind(const sdio_probe_config_t *cfg,
                           const sdio_probe_callbacks_t *cb);
#endif

#endif /* CONFIG_SDIO_HOST_PROBE_BUS */

#ifdef __cplusplus
}
#endif

#endif /* SDIO_PROBE_H */
