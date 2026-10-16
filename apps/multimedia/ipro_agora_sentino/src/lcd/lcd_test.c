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
 * @file lcd_test.c
 * @brief LCD test shell command — fills screen with solid colors
 */

#include <generated/autoconf.h>

#if defined(CONFIG_APP_LCD_ENABLE) && defined(CONFIG_SHELL)

#include "lcd.h"
#include "shell.h"

#include <FreeRTOS.h>
#include <task.h>

static int cmd_lcd_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("LCD test: init...\r\n");
    lcd_init();
    lcd_set_dir(0, 0);

    const struct {
        const char *name;
        lcd_color_t color;
    } colors[] = {
        { "RED",     LCD_COLOR_RGB(255, 0, 0)     },
        { "GREEN",   LCD_COLOR_RGB(0, 255, 0)     },
        { "BLUE",    LCD_COLOR_RGB(0, 0, 255)     },
        { "WHITE",   LCD_COLOR_RGB(255, 255, 255) },
        { "BLACK",   LCD_COLOR_RGB(0, 0, 0)       },
        { "YELLOW",  LCD_COLOR_RGB(255, 255, 0)   },
        { "CYAN",    LCD_COLOR_RGB(0, 255, 255)   },
        { "MAGENTA", LCD_COLOR_RGB(255, 0, 255)   },
    };

    for (uint32_t i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
        printf("  %s (0x%04x)...\r\n", colors[i].name, colors[i].color);
        lcd_clear(colors[i].color);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    printf("LCD test done.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lcd_test, lcd_test, Fill screen with solid colors for visual verification);

#endif /* CONFIG_APP_LCD_ENABLE && CONFIG_SHELL */
