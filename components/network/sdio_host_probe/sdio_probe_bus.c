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
 * sdio_probe_bus.c - the one SDIO host descriptor and the bring-up sequence.
 *
 * Owns a single SDIO_Host_Type on SDH controller 0. Not compiled when
 * CONFIG_SDIO_HOST_PROBE_BUS=n, which is how a module whose vendor driver calls
 * sdio_host_init() itself (ATBM6162) uses the board layer without a second
 * descriptor racing its own on the same controller.
 */
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "sdio_probe.h"

static SDIO_Host_Type          s_host;
static sdio_probe_callbacks_t  s_cb;
static SDIO_Status_Type        s_last_status = Status_SDIO_Success;
/* Two flags, not one: s_inited says the descriptor's RTOS objects exist (never
 * cleared, because sdio_host_init() has no destructor), s_started says the host
 * is usable. sdio_probe_stop() clears only the second, so a following
 * sdio_probe_start() re-enters through sdio_host_reinit() instead of leaking a
 * fresh mutex and semaphore. */
static int                     s_inited;
static int                     s_started;
static int                     s_enumerated;

static void step(const char *name, int rc)
{
    if (s_cb.on_step != NULL) {
        s_cb.on_step(name, rc, s_cb.arg);
    }
}

/* Record the SDIO status and map it onto the probe's return codes. */
static int io(SDIO_Status_Type st)
{
    s_last_status = st;
    return (st == Status_SDIO_Success) ? SDIO_PROBE_OK : SDIO_PROBE_ERR_IO;
}

SDIO_Status_Type sdio_probe_last_status(void) { return s_last_status; }
bool             sdio_probe_is_started(void)  { return s_started != 0; }
SDIO_Host_Type  *sdio_probe_host(void)        { return s_started ? &s_host : NULL; }

int sdio_probe_start(const sdio_probe_config_t *cfg,
                     const sdio_probe_callbacks_t *cb)
{
    int rc;

    if (cb != NULL) {
        s_cb = *cb;
    } else {
        memset(&s_cb, 0, sizeof(s_cb));
    }

    rc = sdio_probe_board_init(cfg);
    step("board_init", rc);
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }

    if (s_inited) {
        /* sdio_host_init() allocates a mutex and a semaphore with no
         * destructor, so calling it twice leaks both (hal_sdio.h). Re-running
         * a probe goes through sdio_host_reinit(), which redoes the controller
         * bring-up and clears enumeration state without touching them. */
        rc = io(sdio_host_reinit(&s_host));
        step("host_reinit", rc);
    } else {
        rc = io(sdio_host_init(&s_host, 0));
        step("host_init", rc);
    }
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }

    s_inited = 1;
    s_started = 1;
    s_enumerated = 0;
    return SDIO_PROBE_OK;
}

int sdio_probe_stop(void)
{
    if (!s_started) {
        return SDIO_PROBE_OK;
    }
    /* The descriptor stays (s_inited): only the card state is dropped, so a
     * later sdio_probe_start() re-enters through sdio_host_reinit(). */
    (void)sdio_probe_power(0);
    s_started = 0;
    s_enumerated = 0;
    step("stop", SDIO_PROBE_OK);
    return SDIO_PROBE_OK;
}

int sdio_probe_enumerate(void)
{
    int rc;

    if (!s_started) {
        return SDIO_PROBE_ERR_STATE;
    }
    rc = io(sdio_card_enumerate(&s_host));
    step("enumerate", rc);
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }
    s_enumerated = 1;
    return SDIO_PROBE_OK;
}

int sdio_probe_negotiate(void)
{
    const sdio_probe_config_t *cfg = sdio_probe_config();
    int rc;

    if (!s_started || !s_enumerated || cfg == NULL) {
        return SDIO_PROBE_ERR_STATE;
    }

    rc = io(sdio_set_bus_width(&s_host, cfg->bus_width));
    step("bus_width", rc);
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }

    /* Clock and high speed are advisory: a module that rejects them is still
     * worth probing at the speed it did accept, so they never abort. */
    rc = io(sdio_set_clock(&s_host, cfg->clock_hz));
    step("clock", rc);

    if (cfg->high_speed) {
        rc = io(sdio_set_high_speed(&s_host, true));
        step("high_speed", rc);
    }

    rc = io(sdio_set_block_size(&s_host, cfg->data_func, cfg->block_size));
    step("block_size", rc);
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }

    rc = io(sdio_enable_func(&s_host, cfg->data_func));
    step("enable_func", rc);
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }

    if (s_cb.on_post_enable != NULL) {
        rc = s_cb.on_post_enable(&s_host, s_cb.arg);
        step("post_enable", rc);
        if (rc != 0) {
            return SDIO_PROBE_ERR_IO;
        }
    }
    return SDIO_PROBE_OK;
}

int sdio_probe_run(void)
{
    int rc;

    if (!s_started) {
        return SDIO_PROBE_ERR_STATE;
    }
    rc = sdio_probe_power(1);
    step("power_on", rc);
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }

    rc = sdio_probe_reset(0, 0);
    step("reset", rc);
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }

    rc = sdio_probe_enumerate();
    if (rc != SDIO_PROBE_OK) {
        return rc;
    }
    return sdio_probe_negotiate();
}
