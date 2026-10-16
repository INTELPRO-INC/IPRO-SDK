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
 * @file app_lcd.c
 * @brief LCD display module — LVGL integration for ILI9341 SPI
 *
 * Initializes the ILI9341 LCD over polling SPI, sets up LVGL v9 with
 * the SDK port layer, and provides state update APIs for the gateway UI.
 *
 * SPI bus sharing: lv_port_tick_pause() suspends the LVGL handler task
 * so no SPI transfers occur during audio RX DMA operations.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_APP_LCD_ENABLE

#include "app_lcd.h"
#include "app_log.h"

#include <string.h>
#include <time.h>
#include <sys/time.h>

/* LCD driver */
#include "lcd/lcd.h"

/* LVGL */
#include "lvgl/lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_tick.h"

/* UI */
#include "ui/ui_screen.h"

#include <FreeRTOS.h>
#include <task.h>

#define LOG_TAG "LCD"

/* ------------------------------------------------------------------ */
/* LVGL flush callback — bridges LVGL to the ILI9341 SPI driver       */
/* ------------------------------------------------------------------ */

/**
 * Polling SPI flush — CPU feeds SPI FIFO directly, NO DMA.
 * Frees DMA0 entirely for DAC → no more audio stutter.
 * Same throughput as DMA (SPI clock is the bottleneck).
 * CPU runs at LVGL priority 2 — audio tasks preempt via tick ISR.
 */
static void lcd_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    (void)disp;
    ili9341_spi_draw_picture_polling(
        area->x1, area->y1,
        area->x2, area->y2,
        (ili9341_spi_color_t *)px_map
    );
}

/* ------------------------------------------------------------------ */
/* Clock update timer — reads SNTP time every 30s                      */
/* ------------------------------------------------------------------ */

static void clock_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0 || tv.tv_sec < 1700000000) {
        /* SNTP not synced yet (epoch < 2023) */
        return;
    }
    time_t now = tv.tv_sec;
    /* UTC+8 for China Standard Time */
    now += 8 * 3600;
    struct tm t;
    gmtime_r(&now, &t);

    char buf[12];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
    ui_screen_set_time(buf);
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

app_err_t app_lcd_init(void)
{
    IPRO_LOGI(LOG_TAG, "Initializing LCD (ILI9341 SPI polling)...");

    /* Step 1: Hardware init */
    int ret = lcd_init();
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "lcd_init failed: %d", ret);
        return APP_ERR_INIT;
    }
    lcd_set_dir(0, 0);  /* Portrait, no mirror */

    /* Clear to black before LVGL takes over */
    lcd_clear(LCD_COLOR_RGB(0, 0, 0));

    IPRO_LOGI(LOG_TAG, "LCD HW OK (%dx%d)", LCD_W, LCD_H);

    /* Step 2: LVGL init */
    lv_init();

    /* Step 3: Display port — blocking flush (coexists with audio DMA) */
    lv_port_disp_cfg_t disp_cfg = {
        .hor_res         = LCD_W,
        .ver_res         = LCD_H,
        .flush_cb        = lcd_flush_cb,
        .async           = false,
        .register_dma_cb = NULL,
        .rgb565_swapped  = true,   /* ILI9341 SPI expects byte-swapped RGB565 */
    };
    lv_display_t *disp = lv_port_disp_init(&disp_cfg);
    if (!disp) {
        IPRO_LOGE(LOG_TAG, "lv_port_disp_init failed");
        return APP_ERR_INIT;
    }

    /* Step 4: Create the UI layout */
    lv_lock();
    ui_screen_create();
    lv_unlock();

    /* Step 5: Start LVGL handler task (tick + timer_handler) */
    lv_port_tick_init();

    /* Step 6: Register clock update timer (30s interval) */
    lv_lock();
    lv_timer_create(clock_timer_cb, 1000, NULL);  /* Update every second */
    lv_unlock();

    IPRO_LOGI(LOG_TAG, "LVGL display initialized");
    return APP_OK;
}

void app_lcd_pause(bool pause)
{
    lv_port_tick_pause(pause);
}

void app_lcd_set_ai_state(rtc_ai_state_t state)
{
    lv_lock();
    ui_screen_set_ai_state(state);
    lv_unlock();
}

void app_lcd_set_tts_text(const char *text)
{
    if (!text) return;
    lv_lock();
    ui_screen_set_tts_text(text);
    lv_unlock();
}

void app_lcd_set_network_status(bool connected)
{
    lv_lock();
    ui_screen_set_mqtt_status(connected);
    lv_unlock();
}

void app_lcd_set_cloud_status(bool connected)
{
    lv_lock();
    ui_screen_set_agora_status(connected);
    lv_unlock();
}

/* ------------------------------------------------------------------ */
/* Shell test commands                                                  */
/* ------------------------------------------------------------------ */

#ifdef CONFIG_SHELL
#include "shell.h"

static int cmd_lcd_state(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: lcd_state <0-3>\r\n");
        printf("  0=IDLE, 1=LISTENING, 2=THINKING, 3=SPEAKING\r\n");
        return 0;
    }
    int state = atoi(argv[1]);
    if (state < 0 || state > 3) {
        printf("Invalid state: %d\r\n", state);
        return -1;
    }
    app_lcd_set_ai_state((rtc_ai_state_t)state);
    printf("AI state set to %d\r\n", state);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lcd_state, lcd_state, Set AI state 0=IDLE 1=LISTEN 2=THINK 3=SPEAK);

static int cmd_lcd_text(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: lcd_text <utf8_text>\r\n");
        return 0;
    }
    /* Concatenate all args as one string */
    static char buf[256];
    int pos = 0;
    for (int i = 1; i < argc && pos < (int)sizeof(buf) - 1; i++) {
        if (i > 1 && pos < (int)sizeof(buf) - 1)
            buf[pos++] = ' ';
        int len = strlen(argv[i]);
        if (pos + len >= (int)sizeof(buf) - 1)
            len = (int)sizeof(buf) - 1 - pos;
        memcpy(&buf[pos], argv[i], len);
        pos += len;
    }
    buf[pos] = '\0';
    app_lcd_set_tts_text(buf);
    printf("TTS text set: %s\r\n", buf);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lcd_text, lcd_text, Set TTS subtitle text);

static int cmd_lcd_monk(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Monk animation demo: cycling through states...\r\n");
    for (int i = 0; i <= 3; i++) {
        printf("  State %d...\r\n", i);
        app_lcd_set_ai_state((rtc_ai_state_t)i);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    printf("  Back to IDLE\r\n");
    app_lcd_set_ai_state(RTC_AI_STATE_IDLE);
    printf("Monk demo done.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lcd_monk, lcd_monk, Demo monk animation cycling through AI states);

#endif /* CONFIG_SHELL */

#endif /* CONFIG_APP_LCD_ENABLE */
