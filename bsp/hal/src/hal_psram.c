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
 * @file hal_psram.c
 * @brief PSRAM Hardware Abstraction Layer Implementation
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <generated/autoconf.h>
#include "hal_psram.h"
#include "hal_lp.h"
#include "drv_psram.h"
#include "drv_glb.h"
#include "soc_base.h"

#define PSRAM_BASIC_ADDR    (0x1e000000)
#define TEST_MEMORY_SIZE    (1 * 1024 * 1024)

#ifdef CONFIG_USE_PSRAM
/*
 * Early log buffer for PSRAM calibration
 * Uses heap region which is not yet used by FreeRTOS at boot time
 */
#define PSRAM_EARLY_LOG_MAGIC   0x50535243  /* "PSRC" */
#define PSRAM_EARLY_LOG_SIZE    256

typedef struct {
    uint32_t magic;
    uint16_t len;
    uint16_t reserved;
    char log[PSRAM_EARLY_LOG_SIZE];
} psram_early_log_t;

extern char __heap_start[];
#define PSRAM_EARLY_LOG_BUF ((psram_early_log_t *)__heap_start)

static void psram_early_log_append(const char *fmt, ...)
{
    psram_early_log_t *buf = PSRAM_EARLY_LOG_BUF;
    va_list args;
    int remaining;

    if (buf->magic != PSRAM_EARLY_LOG_MAGIC) {
        buf->magic = PSRAM_EARLY_LOG_MAGIC;
        buf->len = 0;
        buf->log[0] = '\0';
    }

    remaining = PSRAM_EARLY_LOG_SIZE - buf->len - 1;
    if (remaining <= 0) {
        return;
    }

    va_start(args, fmt);
    int written = vsnprintf(&buf->log[buf->len], remaining, fmt, args);
    va_end(args);

    if (written > 0) {
        buf->len += (written < remaining) ? written : remaining;
    }
}

void hal_psram_print_calibration_log(void)
{
    psram_early_log_t *buf = PSRAM_EARLY_LOG_BUF;

    if (buf->magic == PSRAM_EARLY_LOG_MAGIC && buf->len > 0) {
        printf("[PSRAM] %s", buf->log);
        buf->magic = 0;  /* Clear to avoid re-print */
    }
}
#endif

#ifdef CONFIG_USE_PSRAM
/* Stores the actual DQS config after calibration (may differ from efuse) */
static ATTR_OCRAM_BSS_SECTION uint16_t g_psram_dqs_cfg;

static uint32_t test_pattern_val[] = {
    0xaa55aa55,
    0x33cc33cc,
    0x55aa55aa,
    0xcc33cc33,
};

static const uint16_t dqs_val_table[HAL_PSRAM_DQS_TABLE_SIZE] = {
    0x8000,
    0xC000,
    0xE000,
    0xF000,
    0xF800,
    0xFC00,
    0xFE00,
    0xFF00,
    0xFF80,
    0xFFC0,
    0xFFE0,
    0xFFF0,
    0xFFF8,
    0xFFFC,
    0xFFFE,
    0xFFFF,
};

uint16_t hal_psram_get_dqs_value(uint8_t index)
{
    if (index >= HAL_PSRAM_DQS_TABLE_SIZE) {
        return 0xFFC0; /* default safe value */
    }
    return dqs_val_table[index];
}

uint16_t hal_psram_get_dqs_cfg(void)
{
    return g_psram_dqs_cfg;
}

void hal_psram_set_dqs_cfg(uint16_t cfg)
{
    g_psram_dqs_cfg = cfg;
}
#endif

#ifdef CONFIG_USE_PSRAM
void hal_psram_gpio_init(void)
{
    GLB_GPIO_Cfg_Type cfg;

    PDS_Clear_PSRAM_Pad_Cfg();

    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 0;
    cfg.smtCtrl = 1;

    for (uint8_t i = 0; i < 12; i++) {
        cfg.gpioPin = 50 + i;
        cfg.gpioMode = GPIO_MODE_INPUT;

        GLB_GPIO_Init(&cfg);
    }
}

void hal_psram_enter_hybridsleep(void)
{
    PSram_Ctrl_Winbond_Eneter_Hybrid_Sleep(PSRAM0_ID, PSRAM_WINBOND_REG_CR1, PSRAM_PARTIAL_REFRESH_FULL);
}

void hal_psram_enter_deepsleep(void)
{
    PSram_Ctrl_Winbond_Eneter_Deep_Sleep(PSRAM0_ID);
}

void hal_psram_exit_hybridsleep(void)
{
    PSram_Ctrl_Winbond_Exit_Hybrid_Sleep(PSRAM0_ID);
    arch_delay_us(120);
}

uint16_t hal_psram_winbond_init_dqs(int8_t burst_len, uint8_t is_fixLatency,
                                    uint8_t latency, uint16_t dqs_delay)
{
    uint16_t reg_read = 0;
    PSRAM_Ctrl_Cfg_Type psramCtrlCfg = {
        .vendor = PSRAM_CTRL_VENDOR_WINBOND,
        .ioMode = PSRAM_CTRL_X8_MODE,
        .size = PSRAM_SIZE_8MB,
        .dqs_delay = 0xffc0,
    };

    PSRAM_Winbond_Cfg_Type winbondCfg = {
        .rst = DISABLE,
        .clockType = PSRAM_CLOCK_SINGLE,
        .inputPowerDownMode = DISABLE,
        .hybridSleepMode = DISABLE,
        .linear_dis = ENABLE,
        .PASR = PSRAM_PARTIAL_REFRESH_FULL,
        .disDeepPowerDownMode = ENABLE,
        .fixedLatency = DISABLE,
        .brustLen = PSRAM_WINBOND_BURST_LENGTH_64_BYTES,
        .brustType = PSRAM_WRAPPED_BURST,
        .latency = PSRAM_WINBOND_6_CLOCKS_LATENCY,
        .driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M_115_OHMS_FOR_8M,
    };

    winbondCfg.brustLen = burst_len;
    winbondCfg.fixedLatency = is_fixLatency;
    winbondCfg.latency = latency;

    psramCtrlCfg.dqs_delay = dqs_delay;
    PSram_Ctrl_Init(PSRAM0_ID, &psramCtrlCfg);
    PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &winbondCfg);
    PSram_Ctrl_Winbond_Read_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_ID0, &reg_read);

    return reg_read;
}

uint8_t hal_psram_rw_check(void)
{
    int i = 0;
    volatile uint32_t *p = (uint32_t *)PSRAM_BASIC_ADDR;
    volatile int q = 0;

    p[0] = 0xaa55aa55;
    p[1] = 0x33cc33cc;
    p[2] = 0x55aa55aa;
    p[3] = 0xcc33cc33;

    __ISB();

    for (i = 0; i < 4; i++) {
        q = p[i];
        if (q != test_pattern_val[i]) {
            /* RW mismatch at index 0x%x (expected != actual) — normal during DQS sweep */
            return ERROR;
        }
    }

    /* clear PSRAM 1M Byte */
    for (i = 0; i < TEST_MEMORY_SIZE/4; i++) {
        p[i] = 0xffffffff;
    }

    /* check PSRAM 1M Byte */
    for (i = 0; i < TEST_MEMORY_SIZE/4; i++) {
        p[i] = i;
    }

    for (i = 0; i < TEST_MEMORY_SIZE/4; i++) {
        q = p[i];
        if (q != i) {
            /* RW mismatch at index 0x%x (expected != actual) — normal during DQS sweep */
            return ERROR;
        }
    }

    return SUCCESS;
}

uint16_t hal_psram_x8_calibration(uint16_t psram_dqs_cfg, int32_t *psram_dqs_win_num, uint16_t *new_dqs_cfg)
{
    int16_t psram_id = 0;
    int left_flag = 0, right_flag = 0, c_val = 0;
    int dqs_win_min = 16, dqs_win_max = 0;

    if (psram_dqs_cfg != 0xffff) {
        /* Use efuse calibration values */
        left_flag = ((psram_dqs_cfg & (0xf0)) >> 0x4);
        right_flag = (psram_dqs_cfg & (0xf));
        c_val = ((left_flag + right_flag) >> 0x1);
        *psram_dqs_win_num = right_flag - left_flag;
        psram_early_log_append("efuse dqs cfg 0x%04x found, window: 0x%02x ~ 0x%02x; c_val: 0x%02x; dqs:0x%04x; code num:%d\r\n",
               psram_dqs_cfg, left_flag, right_flag, c_val, dqs_val_table[c_val], (right_flag - left_flag));
        psram_id = hal_psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0,
                                              PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val_table[c_val]);
        if (psram_id != HAL_PSRAM_ID_WINBOND_8MB) {
            return ERROR;
        }
        if (new_dqs_cfg) {
            *new_dqs_cfg = psram_dqs_cfg;
        }
    } else {
        /* Perform calibration */
        for (int dqs_index = 0; dqs_index < 16; dqs_index++) {
            psram_id = hal_psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0,
                                                  PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val_table[dqs_index]);
            if (psram_id == HAL_PSRAM_ID_WINBOND_8MB) {
                if (hal_psram_rw_check() == SUCCESS) {
                    if (dqs_index < dqs_win_min) {
                        dqs_win_min = dqs_index;
                        dqs_win_max = dqs_index;
                    } else if (dqs_index > dqs_win_max) {
                        dqs_win_max = dqs_index;
                    }
                }
            }
        }

        left_flag = dqs_win_min;
        right_flag = dqs_win_max;
        c_val = ((left_flag + right_flag) >> 1);

        *psram_dqs_win_num = right_flag - left_flag;
        if (((*psram_dqs_win_num) <= 4) || ((*psram_dqs_win_num) > 0xf)) {
            return ERROR;
        }

        if (new_dqs_cfg) {
            *new_dqs_cfg = (((left_flag << 0x4) | (right_flag)) & (0xff));
        }
        psram_early_log_append("calibration window: 0x%02x ~ 0x%02x; c_val: 0x%02x; dqs:0x%04x; code num:%d\r\n",
               left_flag, right_flag, c_val, dqs_val_table[c_val], (right_flag - left_flag));
        psram_id = hal_psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0,
                                              PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val_table[c_val]);
        if (psram_id != HAL_PSRAM_ID_WINBOND_8MB) {
            return ERROR;
        }
    }
    return psram_id;
}

uint32_t ATTR_TCM_SECTION hal_psram_init(uint16_t psram_dqs_cfg)
{
    uint16_t ret = 0;
    int32_t psram_dqs_win_num = 0;

    hal_psram_gpio_init();
    GLB_Set_PSRAMB_CLK_Sel(ENABLE, GLB_PSRAMB_PLL_480M, 0);

    /* Initial PSRAM with default DQS delay */
    hal_psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, 0xffc0);

    /* Perform calibration, store result in global for LP restore */
    ret = hal_psram_x8_calibration(psram_dqs_cfg, &psram_dqs_win_num, &g_psram_dqs_cfg);

    /* Flush i-cache in case branch prediction logic is wrong when
       psram is not inited but cpu has already prefetch psram */
    __ISB();

    if (ret == ERROR) {
        while (1) {
            printf("psram:%d\r\n", (int)psram_dqs_win_num);
            arch_delay_ms(500);
        }
    }

    return ret;
}

/*============================ LP Framework overrides =====================*/

/**
 * @brief Strong override: get PSRAM DQS config for LP framework
 *
 * Overrides the weak default in hal_lp.c so LP init stores
 * the calibrated DQS config for restore after wakeup.
 */
uint16_t hal_lp_get_psram_dqs_cfg(void)
{
    return g_psram_dqs_cfg;
}

/**
 * @brief Strong override: restore PSRAM after LP wakeup
 *
 * Overrides the weak no-op in hal_lp.c. Re-initializes PSRAM
 * controller and exits hybrid sleep using saved DQS calibration.
 */
void ATTR_TCM_SECTION hal_lp_psram_restore(void)
{
    uint16_t dqs_cfg = hal_lp_param->psram_dqs_cfg;

    int left_flag = ((dqs_cfg & 0xf0) >> 4);
    int right_flag = (dqs_cfg & 0xf);
    int c_val = (left_flag + right_flag) >> 1;

    hal_psram_gpio_init();
    GLB_Set_PSRAMB_CLK_Sel(ENABLE, GLB_PSRAMB_PLL_480M, 0);
    hal_psram_exit_hybridsleep();

    {
        PSRAM_Ctrl_Cfg_Type psramCtrlCfg = {
            .vendor = PSRAM_CTRL_VENDOR_WINBOND,
            .ioMode = PSRAM_CTRL_X8_MODE,
            .size = PSRAM_SIZE_8MB,
            .dqs_delay = hal_psram_get_dqs_value(c_val),
        };
        PSRAM_Winbond_Cfg_Type winbondCfg = {
            .rst = DISABLE,
            .clockType = PSRAM_CLOCK_SINGLE,
            .inputPowerDownMode = DISABLE,
            .linear_dis = ENABLE,
            .hybridSleepMode = DISABLE,
            .PASR = PSRAM_PARTIAL_REFRESH_FULL,
            .disDeepPowerDownMode = ENABLE,
            .fixedLatency = DISABLE,
            .brustLen = PSRAM_WINBOND_BURST_LENGTH_64_BYTES,
            .brustType = PSRAM_WRAPPED_BURST,
            .driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M_115_OHMS_FOR_8M,
            .latency = PSRAM_WINBOND_6_CLOCKS_LATENCY,
        };
        PSram_Ctrl_Init(PSRAM0_ID, &psramCtrlCfg);
        PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &winbondCfg);
        PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR1, &winbondCfg);
    }
}
#endif
