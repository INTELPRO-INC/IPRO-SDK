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
 * ipro_epd_test — bring-up shell for GDEY0213F52 (2.13" BWRY).
 *
 * Wiring (IPRO7AI PDB J2, RES & BUSY NOT wired):
 *   J2.1 VCC=3V3   J2.2 CS=GPIO10   J2.3 SCK=GPIO15
 *   J2.4 MOSI=GPIO16   J2.5 D/C=GPIO17 (was MISO)   J2.6 GND
 *   E-paper RES pin: tie to VCC. BUSY pin: leave open.
 *
 * Shell:
 *   epd_init           — power-on settle + LOCK + PON
 *   epd_white | epd_black | epd_red | epd_yellow
 *   epd_stripes        — 4-color horizontal bands (K/W/Y/R)
 *   epd_pins           — read CS/DC pin levels
 *   epd_sleep          — POF (panel idles, woken by next epd_init)
 *
 * Stubbed (RES/BUSY not wired on PDB): epd_busy, epd_reset,
 *   epd_busy_watch, epd_diagnose, epd_drf_watch — they print a notice.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"
#include "freertos_app_hooks.h"
#include "hal_boot2.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "ipro_log.h"

/* Force CS/DC pins to plain-GPIO function before any read.
 * Some IPRO7 GPIOs power up in alt-function mode (clock out, USB, etc.).
 * RES/BUSY not wired on PDB so they are not touched. */
static void diag_force_gpio_function(void)
{
    uint8_t pins[] = { 10, 17 };  /* CS, DC */
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pins, sizeof(pins));
}

#include "src/epd_gdey0213f52.h"
#include "src/badge_data.h"
#include "src/price_data.h"
#include "src/pricetag_render.h"
#include "src/templates.h"

#define TAG "EPD_TEST"

static int s_ready = 0;

static int cmd_epd_init(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("[epd_init] starting...\r\n");
    int rc = epd_init();
    s_ready = (rc == 0);
    printf("[epd_init] -> %d (%s)\r\n", rc, s_ready ? "OK" : "FAIL");
    return rc;
}

static int do_fill(epd_color_t c, const char *name)
{
    if (!s_ready) { printf("run epd_init first\r\n"); return -1; }
    printf("[fill %s] full update ~18s (fixed delay, BUSY not wired)\r\n", name);
    int rc = epd_fill(c);
    printf("[fill %s] -> %d\r\n", name, rc);
    return rc;
}

static int cmd_epd_white  (int a, char **v) { (void)a; (void)v; return do_fill(EPD_WHITE,  "white");  }
static int cmd_epd_black  (int a, char **v) { (void)a; (void)v; return do_fill(EPD_BLACK,  "black");  }
static int cmd_epd_red    (int a, char **v) { (void)a; (void)v; return do_fill(EPD_RED,    "red");    }
static int cmd_epd_yellow (int a, char **v) { (void)a; (void)v; return do_fill(EPD_YELLOW, "yellow"); }

static int cmd_epd_stripes(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (!s_ready) { printf("run epd_init first\r\n"); return -1; }
    printf("[stripes] drawing K/W/Y/R bands top-to-bottom\r\n");
    int rc = epd_stripes_4color();
    printf("[stripes] -> %d\r\n", rc);
    return rc;
}

static int cmd_epd_busy(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("BUSY pin not wired on PDB; driver assumes idle.\r\n");
    return 0;
}

static int cmd_epd_pins(int argc, char **argv)
{
    (void)argc; (void)argv;
    diag_force_gpio_function();
    gpio_set_mode(10, 3); gpio_set_mode(17, 3);
    printf("CS  (GPIO10) = %d\r\n", gpio_read(10));
    printf("DC  (GPIO17) = %d\r\n", gpio_read(17));
    printf("RES/BUSY: not wired (RES tied to VCC externally).\r\n");
    return 0;
}

static int cmd_epd_reset(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("[reset] RES pin not wired on PDB. Power-cycle the board to reset the panel.\r\n");
    return 0;
}

/* RES/BUSY diagnostic — no-op on PDB because neither pin is wired. */
static int cmd_epd_busy_watch(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("[busy_watch] RES & BUSY not wired on PDB — diagnostic disabled.\r\n");
    return 0;
}

static int cmd_epd_sleep(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = epd_sleep();
    s_ready = 0;
    printf("[sleep] -> %d (run epd_init to wake)\r\n", rc);
    return rc;
}

SHELL_CMD_EXPORT_ALIAS(cmd_epd_init,    epd_init,    Initialize panel);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_white,   epd_white,   Fill all-white);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_black,   epd_black,   Fill all-black);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_red,     epd_red,     Fill all-red);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_yellow,  epd_yellow,  Fill all-yellow);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_stripes, epd_stripes, 4-color bands);

/* Triggers R17H AUTO (PON->DRF->POF) without sending DTM data. Tests the
 * panel's refresh engine end-to-end. Should take ~16s if booster + LUT MTP
 * are both healthy. If it returns instantly the panel itself is broken /
 * unpowered, regardless of any DTM logic. */
static int cmd_epd_auto(int argc, char **argv)
{
    (void)argc; (void)argv;
    /* Need to re-init only if panel is asleep. Bring up GPIO+SPI either way. */
    if (!s_ready && epd_init() != 0) return -1;
    s_ready = 1;

    printf("[auto] R17H + 0xA5 (PON->DRF->POF) — should take ~16s\r\n");
    int rc = epd_auto_refresh();
    printf("[auto] -> %d\r\n", rc);
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_auto, epd_auto, R17H AUTO refresh test);

/* Hold panel in PON state for 10s so the user can probe PREVGH/PREVGL test
 * points with a multimeter. Booster should be actively running during this
 * window — gates to ~+/-20V if hardware is healthy. */
static int cmd_epd_pon_hold(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (!s_ready && epd_init() != 0) return -1;
    s_ready = 1;

    printf("[pon_hold] panel POWERED — measure PREVGH/PREVGL now\r\n");
    printf("           expect: PREVGH = +18~+22V, PREVGL = -18~-22V\r\n");
    for (int i = 10; i > 0; i--) {
        printf("           %d sec remaining\r\n", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    printf("[pon_hold] sending POF\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_pon_hold, epd_pon_hold, Hold PON 10s for voltage probing);

/* Display the prototype electronic badge (122x250 BWRY image generated by
 * tools/gen_badge.py + tools/img_to_epd.py). The driver's epd_display()
 * already runs the AUTO chain twice to fully cycle the BWRY waveform,
 * so a separate white-clear pass isn't needed for clean rendering. */
static int cmd_epd_badge(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (!s_ready && epd_init() != 0) return -1;
    s_ready = 1;

    printf("[badge] rendering electronic badge\r\n");
    int rc = epd_display(epd_badge_data, sizeof(epd_badge_data));
    printf("[badge] -> %d\r\n", rc);
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_badge, epd_badge, Render employee badge);

/* Display the prototype electronic price tag (landscape design rotated to
 * 122x250 portrait by tools/gen_price.py + tools/img_to_epd.py). Mounted
 * with FPC pointing right, the panel reads correctly as a horizontal
 * shelf label. */
static int cmd_epd_price(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (!s_ready && epd_init() != 0) return -1;
    s_ready = 1;

    printf("[price] rendering price tag\r\n");
    int rc = epd_display(epd_price_data, sizeof(epd_price_data));
    printf("[price] -> %d\r\n", rc);
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_price, epd_price, Render electronic price tag);

/* Render a pre-baked product template with a dynamically-overlaid sale
 * price. Demonstrates the local rendering path that the BLE-driven ESL
 * tag firmware will reuse — pick a template index, pass any ASCII price
 * string ("$79", "$129.5"), see it on the panel. */
static int cmd_epd_pricetag(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: epd_pricetag <idx> <price>\r\n");
        printf("  idx in [0..%d]:\r\n", PRICETAG_TEMPLATE_COUNT - 1);
        for (int i = 0; i < PRICETAG_TEMPLATE_COUNT; i++) {
            printf("    %d  %s (%s)\r\n",
                   i,
                   pricetag_templates[i].name_zh,
                   pricetag_templates[i].name_en);
        }
        return -1;
    }

    int idx = atoi(argv[1]);
    const char *price = argv[2];

    if (!s_ready && epd_init() != 0) return -1;
    s_ready = 1;

    printf("[pricetag] %d (%s) %s\r\n",
           idx,
           (idx >= 0 && idx < PRICETAG_TEMPLATE_COUNT)
               ? pricetag_templates[idx].name_zh : "?",
           price);
    int rc = pricetag_render(idx, price);
    printf("[pricetag] -> %d\r\n", rc);
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_pricetag, epd_pricetag, Template + dynamic price);

static int cmd_epd_clear(int argc, char **argv)
{
    int cycles = (argc >= 2) ? atoi(argv[1]) : 3;
    if (!s_ready && epd_init() != 0) return -1;
    s_ready = 1;

    printf("[clear] %d white refreshes\r\n", cycles);
    int rc = epd_clear(cycles);
    printf("[clear] -> %d\r\n", rc);
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_clear, epd_clear, Triple-white refresh to wipe ghosting);

/* BUSY trace — no-op on PDB because BUSY pin is not wired. */
static int cmd_epd_drf_watch(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("[drf_watch] BUSY not wired on PDB — diagnostic disabled.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_drf_watch, epd_drf_watch, High-res BUSY trace after DRF);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_busy,    epd_busy,    Read BUSY pin);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_pins,    epd_pins,    Read all signal pins);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_reset,   epd_reset,   Toggle RES line only);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_busy_watch, epd_busy_watch, Sample BUSY for 3s after reset);

/* RES->BUSY causality test — no-op on PDB. */
static int cmd_epd_diagnose(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("[diagnose] RES & BUSY not wired on PDB — diagnostic disabled.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_epd_diagnose, epd_diagnose, RES->BUSY causality test);
SHELL_CMD_EXPORT_ALIAS(cmd_epd_sleep,   epd_sleep,   Deep sleep);

static void main_task(void *pv)
{
    (void)pv;

    /* Initialize IPRO log subsystem so IPRO_LOGI/W/E inside the driver work.
     * Without this, all log calls are silently dropped. printf still works
     * because it goes through the lower-level UART path. */
    ipro_log_init(NULL);

    printf("\r\n========================================\r\n");
    printf("  GDEY0213F52 EPD bring-up (IPRO7AI PDB J2)\r\n");
    printf("========================================\r\n");
    printf("Wiring:  CS=10 SCK=15 MOSI=16 D/C=17  RES->VCC  BUSY=NC\r\n");
    printf("Quick start:\r\n");
    printf("  1) epd_init         — power-on settle + LOCK + PON (~150ms)\r\n");
    printf("  2) epd_stripes      — 4-color test pattern (~18s refresh, fixed delay)\r\n");
    printf("========================================\r\n");

    shell_init_with_task(UART0_INDEX);
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_setup_heap();
    platform_init(0);
    hal_boot2_init();

    xTaskCreate(main_task, "main", 2048, NULL,
                configMAX_PRIORITIES - 1, NULL);

    vTaskStartScheduler();
    while (1) {}
    return 0;
}
