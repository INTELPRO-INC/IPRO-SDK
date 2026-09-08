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
 * sdio_probe_board.c - SDH pinmux and module power/reset sequencing.
 *
 * Collected verbatim from apps/test/ipro_sdio_probe, ipro_sdio_host_probe and
 * ipro_atbm_probe, which carried three byte-identical copies of the pinmux and
 * three near-identical copies of the GPIO sequencing. The pinmux body is the
 * ipronet_host variant - the only one of the four that also re-asserts OE after
 * GLB_GPIO_Init(); see the comment on that loop.
 */
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "drv_glb.h"
#include "drv_gpio.h"

#include "sdio_probe.h"

/* Fixed by the IPRO7 SDH pinmux. GPIO30 is the one line left without a pull-up
 * (see the header for the surviving CMD-vs-CLK disagreement in the comments we
 * inherited; the electrical requirement is the same either way). */
static const uint8_t s_sdh_pins[] = {
    GLB_GPIO_PIN_28, GLB_GPIO_PIN_29, GLB_GPIO_PIN_30,
    GLB_GPIO_PIN_31, GLB_GPIO_PIN_32, GLB_GPIO_PIN_33,
};
#define SDH_PIN_NO_PULLUP   GLB_GPIO_PIN_30

static sdio_probe_config_t s_cfg;
static int s_board_ready;

void sdio_probe_config_default(sdio_probe_config_t *cfg)
{
    if (cfg == NULL) {
        return;
    }
    memset(cfg, 0, sizeof(*cfg));

    /* IPRO7 EVB wiring used through the AIC/generic bring-ups. */
    cfg->pin_power_en      = GLB_GPIO_PIN_16;
    cfg->pin_reset_n       = GLB_GPIO_PIN_17;
    cfg->pin_wake          = GLB_GPIO_PIN_18;
    cfg->power_active_high = true;
    cfg->reset_active_low  = true;

    cfg->power_settle_ms   = 10;
    cfg->reset_low_ms      = 5;
    cfg->reset_post_ms     = 20;

    cfg->data_func         = SDIO_FUNC_1;
    cfg->block_size        = 512;
    cfg->clock_hz          = 25000000U;
    cfg->bus_width         = SDIO_BUS_WIDTH_4BIT;
    cfg->high_speed        = false;
}

const sdio_probe_config_t *sdio_probe_config(void)
{
    return s_board_ready ? &s_cfg : NULL;
}

/* ------------------------------------------------------------------ */
/* GPIO helpers                                                        */
/* ------------------------------------------------------------------ */

static bool pin_present(uint8_t pin)
{
    return pin != SDIO_PROBE_PIN_NONE;
}

static void pin_out_init(uint8_t pin)
{
    if (!pin_present(pin)) {
        return;
    }
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = GPIO_PULL_NONE,
        .drive    = 1,
        .smtCtrl  = 0,
        .gpioFun  = GPIO_FUN_GPIO,
        .gpioPin  = pin,
    };
    GLB_GPIO_Init(&cfg);
}

/* Drive `pin` to the asserted(1)/de-asserted(0) level for its polarity. */
static void pin_drive(uint8_t pin, bool active_high, int assert)
{
    if (!pin_present(pin)) {
        return;
    }
    GLB_GPIO_Write(pin, (assert != 0) == active_high ? 1 : 0);
}

/* ------------------------------------------------------------------ */
/* Pinmux                                                              */
/* ------------------------------------------------------------------ */

static void sdh_pinmux_init(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive    = 2,
        .smtCtrl  = 1,
        .gpioFun  = GPIO_FUN_SDC,
    };
    for (size_t i = 0; i < sizeof(s_sdh_pins) / sizeof(s_sdh_pins[0]); i++) {
        cfg.gpioPin  = s_sdh_pins[i];
        cfg.pullType = (s_sdh_pins[i] == SDH_PIN_NO_PULLUP) ? GPIO_PULL_NONE
                                                            : GPIO_PULL_UP;
        GLB_GPIO_Init(&cfg);
    }

    GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 1);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);

    /* GLB_GPIO_Init() clears OE for AF-mode pins (see the SPI1 master bring-up
     * memo). For GPIO_FUN_SDC, CMD52 already works before this line runs, so
     * the SDH pad-mux clearly drives CMD/CLK regardless of the OE bit. Set OE
     * anyway as defensive hygiene - one register write per pin, harmless if the
     * SDC function path already overrides it.
     *
     * DELIBERATE BEHAVIOUR CHANGE: none of the three probe apps this component
     * replaced did this; ipronet_host, the shipping SDIO-WiFi host, does. A
     * bring-up probe that pinmuxes differently from the production driver is a
     * probe whose PASS does not predict the driver working, so the probes move
     * to the production sequence rather than the other way round. If a module
     * ever behaves differently under the probe than it did before this change,
     * this loop is the first thing to bisect. */
    for (size_t i = 0; i < sizeof(s_sdh_pins) / sizeof(s_sdh_pins[0]); i++) {
        GLB_GPIO_OUTPUT_Enable(s_sdh_pins[i]);
    }
}

/* ------------------------------------------------------------------ */
/* Public board layer                                                  */
/* ------------------------------------------------------------------ */

int sdio_probe_board_init(const sdio_probe_config_t *cfg)
{
    if (s_board_ready) {
        return SDIO_PROBE_OK;   /* pins are latched by the first call */
    }
    if (cfg == NULL) {
        sdio_probe_config_default(&s_cfg);
    } else {
        s_cfg = *cfg;
    }

    pin_out_init(s_cfg.pin_power_en);
    pin_out_init(s_cfg.pin_reset_n);
    pin_out_init(s_cfg.pin_wake);

    /* Start unpowered and held in reset, WAKE low. */
    pin_drive(s_cfg.pin_power_en, s_cfg.power_active_high, 0);
    pin_drive(s_cfg.pin_reset_n, !s_cfg.reset_active_low, 1);
    if (pin_present(s_cfg.pin_wake)) {
        GLB_GPIO_Write(s_cfg.pin_wake, 0);
    }

    sdh_pinmux_init();

    s_board_ready = 1;
    return SDIO_PROBE_OK;
}

int sdio_probe_power(int on)
{
    if (!s_board_ready) {
        return SDIO_PROBE_ERR_STATE;
    }
    if (!pin_present(s_cfg.pin_power_en)) {
        return SDIO_PROBE_OK;   /* board has no switchable rail */
    }
    pin_drive(s_cfg.pin_power_en, s_cfg.power_active_high, on);
    if (on) {
        vTaskDelay(pdMS_TO_TICKS(s_cfg.power_settle_ms));
    }
    return SDIO_PROBE_OK;
}

int sdio_probe_reset(uint32_t low_ms, uint32_t post_ms)
{
    if (!s_board_ready) {
        return SDIO_PROBE_ERR_STATE;
    }
    if (!pin_present(s_cfg.pin_reset_n)) {
        return SDIO_PROBE_OK;
    }
    if (low_ms == 0) {
        low_ms = s_cfg.reset_low_ms;
    }
    if (post_ms == 0) {
        post_ms = s_cfg.reset_post_ms;
    }
    pin_drive(s_cfg.pin_reset_n, !s_cfg.reset_active_low, 1);
    vTaskDelay(pdMS_TO_TICKS(low_ms));
    pin_drive(s_cfg.pin_reset_n, !s_cfg.reset_active_low, 0);
    vTaskDelay(pdMS_TO_TICKS(post_ms));
    return SDIO_PROBE_OK;
}

int sdio_probe_power_cycle(void)
{
    if (!s_board_ready) {
        return SDIO_PROBE_ERR_STATE;
    }
    /* Without the de-asserted window, re-running a probe only re-enumerates a
     * module still running the previous (possibly wedged) firmware. */
    pin_drive(s_cfg.pin_power_en, s_cfg.power_active_high, 0);
    pin_drive(s_cfg.pin_reset_n, !s_cfg.reset_active_low, 1);
    vTaskDelay(pdMS_TO_TICKS(s_cfg.reset_low_ms));

    pin_drive(s_cfg.pin_power_en, s_cfg.power_active_high, 1);
    pin_drive(s_cfg.pin_reset_n, !s_cfg.reset_active_low, 0);
    vTaskDelay(pdMS_TO_TICKS(s_cfg.reset_post_ms));
    return SDIO_PROBE_OK;
}

int sdio_probe_wake(int level)
{
    if (!s_board_ready) {
        return SDIO_PROBE_ERR_STATE;
    }
    if (!pin_present(s_cfg.pin_wake)) {
        return SDIO_PROBE_OK;
    }
    GLB_GPIO_Write(s_cfg.pin_wake, level ? 1 : 0);
    return SDIO_PROBE_OK;
}
