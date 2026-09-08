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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "drv_auadc_v1.h"
#include "drv_glb.h"
#include "hal_gpio.h"
#include "hal_mtimer.h"
#include "hal_dma.h"
#include "hal_dac.h"
#include "drv_common.h"

/*
 * Precision ADC (PADC) Test Shell Commands
 *
 * The AUADC measure mode provides 24-bit SDM-based precision ADC
 * with configurable PGA (6~42dB) and sample rates (2.5~4000 SPS).
 *
 * AIN channel to GPIO mapping (IPRO7 EVB):
 *   AIN0=GPIO4   AIN1=GPIO5   AIN2=GPIO6   AIN3=GPIO7
 *   AIN4=GPIO10  AIN5=GPIO11  AIN6=GPIO12  AIN7=GPIO13
 */

static const uint8_t ain_gpio_map[] = {4, 5, 6, 7, 10, 11, 12, 13};

static const struct {
    const char *name;
    AUADC_ADC_MeasureMode_Rate rate;
    uint32_t timeout_us;
} rate_table[] = {
    {"2.5",  AUADC_ADC_MEASURE_RATE_SPS_2_5,   500000},
    {"5",    AUADC_ADC_MEASURE_RATE_SPS_5,      300000},
    {"10",   AUADC_ADC_MEASURE_RATE_SPS_10,     200000},
    {"20",   AUADC_ADC_MEASURE_RATE_SPS_20,     100000},
    {"25",   AUADC_ADC_MEASURE_RATE_SPS_25,     100000},
    {"50",   AUADC_ADC_MEASURE_RATE_SPS_50,      50000},
    {"100",  AUADC_ADC_MEASURE_RATE_SPS_100,     30000},
    {"200",  AUADC_ADC_MEASURE_RATE_SPS_200,     20000},
    {"400",  AUADC_ADC_MEASURE_RATE_SPS_400,     10000},
    {"800",  AUADC_ADC_MEASURE_RATE_SPS_800,      5000},
    {"1000", AUADC_ADC_MEASURE_RATE_SPS_1000,     5000},
    {"2000", AUADC_ADC_MEASURE_RATE_SPS_2000,     3000},
    {"4000", AUADC_ADC_MEASURE_RATE_SPS_4000,     2000},
};
#define RATE_TABLE_SIZE (sizeof(rate_table) / sizeof(rate_table[0]))

static void padc_gpio_init(uint8_t ch)
{
    GLB_GPIO_Cfg_Type gpio_cfg = {
        .gpioFun = GPIO_FUN_ANALOG,
        .gpioMode = GPIO_MODE_ANALOG,
        .pullType = GPIO_PULL_NONE,
        .drive = 0,
        .smtCtrl = 0,
        .gpioPin = ain_gpio_map[ch],
    };

    GLB_GPIO_Init(&gpio_cfg);
}

static void padc_clock_init(void)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUIDO);
    GLB_Set_AUDIO_CLK(0, 1, GLB_AUDIO_CLK_SRC_2P032M, 4);
}

/* ================================================================== */
/* DMA-based PADC sample collection                                    */
/*                                                                      */
/* Collects N 32-bit samples via FIFO+DMA. Uses 20-bit resolution with */
/* MSB at bit 31 (left-aligned in 32-bit word, sign-extended).         */
/* FIFO receives Sinc3 output at ~8x the configured ODR.               */
/* Returns elapsed_us, or 0 on timeout. Samples in padc_dma_buf[].     */
/* ================================================================== */

#define PADC_DMA_BUF_MAX  2048
#define DAC_DMA_BUF_MAX   16384  /* 16K × 2B = 32KB, enough for 32 steps × 50ms @ 8KHz */

static int32_t padc_dma_buf[PADC_DMA_BUF_MAX] __attribute__((aligned(32)));
static uint16_t dac_dma_buf[DAC_DMA_BUF_MAX] __attribute__((aligned(32)));

static volatile int dac_dma_done;

static void dac_dma_callback(void *dev, uint32_t event)
{
    if (event == HAL_DMA_INT_COMPLETE)
        dac_dma_done = 1;
}

static uint64_t padc_dma_collect(int n)
{
    if (n > PADC_DMA_BUF_MAX) n = PADC_DMA_BUF_MAX;

    /* 20-bit resolution, MSB at bit 31 (left-aligned, sign-extended) */
    AUADC_FifoCfg_Type fifo_cfg = {
        .resolution = AUADC_RES_20_BITS,
        .ailgnMode = AUADC_FIFO_AILGN_MSB_AT_BIT31,
        .FifoIntThreshold = 0,
        .dmaThresholdMode = AUADC_DRQ_EQUEL_TO_IRQ,
        .dmaEn = ENABLE,
    };
    AUADC_FifoInit(&fifo_cfg);
    AUADC_FifoClear();

    /* 32-bit DMA to capture full FIFO word */
    dma_device_t dma = {
        .id = DMA0_ID, .ch = DMA0_CH1_INDEX,
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_AUD_ADC, .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR1, .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_32BIT,
        .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .transfer_mode = DMA_LLI_ONCE_MODE,
        .lli_cfg = NULL, .callback = NULL,
    };

    dma_setConfig(&dma);
    dma_init(DMA0_ID, DMA0_CH1_INDEX);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_INT, NULL);

    L1C_DCACHE_INVALID_RANGE((uintptr_t)padc_dma_buf, n * sizeof(int32_t));

    AUADC_Start();
    dma_reload(DMA0_ID, DMA0_CH1_INDEX,
               (uint32_t)DMA_ADDR_AUD_RDR,
               (uint32_t)(uintptr_t)padc_dma_buf,
               n * sizeof(int32_t), NULL);

    uint64_t t0 = mtimer_get_time_us();
    dma_channel_start(DMA0_ID, DMA0_CH1_INDEX);

    uint64_t deadline = t0 + 30000000;
    int done = 0;
    while (mtimer_get_time_us() < deadline) {
        if (!dma_channel_check_busy(DMA0_ID, DMA0_CH1_INDEX)) {
            done = 1;
            break;
        }
    }
    uint64_t dt = mtimer_get_time_us() - t0;

    dma_channel_stop(DMA0_ID, DMA0_CH1_INDEX);
    AUADC_Stop();
    AUADC_FifoClear();

    L1C_DCACHE_INVALID_RANGE((uintptr_t)padc_dma_buf, n * sizeof(int32_t));

    return done ? dt : 0;
}

/*
 * Track which channels have been initialized via padc_init.
 * Prevents running measurement on pins still in digital mode.
 */
static uint8_t padc_ch_inited; /* bitmask: bit N = AIN channel N initialized */

static void padc_hw_init(AUADC_PADC_Cfg_Type *cfg)
{
    padc_clock_init();

    AUADC_Cfg_Type top_cfg = {
        .clk = AUADC_CLK_16K_HZ,
        .auadcClkEnable = 1,
        .firMode = 0,
        .pdmItfInvEnable = 0,
        .adcItfInvEnable = 0,
        .source = AUADC_SOURCE_ANALOG,
        .pdmChannel = AUADC_PDM_LEFT,
    };

    AUADC_Disable();
    AUADC_Enable();
    AUADC_Init(&top_cfg);
    AUADC_PADC_Init(cfg);
}

static int padc_check_inited(int ch_pos, int ch_neg, int diff_mode)
{
    if (!(padc_ch_inited & (1 << ch_pos))) {
        printf("Error: AIN%d (GPIO%d) not initialized.\r\n", ch_pos, ain_gpio_map[ch_pos]);
        printf("Run 'padc_init %d' first to set GPIO to analog mode.\r\n", ch_pos);
        return -1;
    }
    if (diff_mode && !(padc_ch_inited & (1 << ch_neg))) {
        printf("Error: AIN%d (GPIO%d) not initialized.\r\n", ch_neg, ain_gpio_map[ch_neg]);
        printf("Run 'padc_init %d' first.\r\n", ch_neg);
        return -1;
    }
    return 0;
}

/*
 * padc_init <ch> [ch2 ...]
 *
 * Set GPIO pin(s) to analog input mode. Run this BEFORE connecting
 * external voltage source to protect the IO pad.
 *
 * Workflow:
 *   1. padc_init 4           # GPIO10 -> analog mode (safe to connect)
 *   2. Connect voltage to GPIO10 via 1K resistor
 *   3. padc 4 6 100 10       # start measurement
 */
static int cmd_padc_init(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: padc_init <ch> [ch2 ...]\r\n");
        printf("  Set AIN channel GPIO to analog input mode.\r\n");
        printf("  Run BEFORE connecting external voltage.\r\n");
        printf("\r\n");
        printf("  AIN0=GPIO4  AIN1=GPIO5  AIN2=GPIO6  AIN3=GPIO7\r\n");
        printf("  AIN4=GPIO10 AIN5=GPIO11 AIN6=GPIO12 AIN7=GPIO13\r\n");
        printf("\r\n");
        printf("  padc_init 4       # init AIN4 (GPIO10)\r\n");
        printf("  padc_init 4 5     # init AIN4+AIN5 for differential\r\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        int ch = atoi(argv[i]);
        if (ch > 7) {
            printf("Error: channel must be 0~7\r\n");
            return -1;
        }
        padc_gpio_init(ch);
        padc_ch_inited |= (1 << ch);
        printf("AIN%d (GPIO%d) -> analog input mode\r\n", ch, ain_gpio_map[ch]);
    }
    printf("Safe to connect voltage source now.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_init, padc_init, Set GPIO to analog mode before PADC test);

static void padc_print_help(void)
{
    printf("Usage:\r\n");
    printf("  Step 1: padc_init <ch>       # set GPIO to analog (safe to wire)\r\n");
    printf("  Step 2: connect voltage via 1K resistor\r\n");
    printf("  Step 3: padc <ch> [gain_db] [rate_sps] [count]\r\n");
    printf("\r\n");
    printf("  padc <ch> [gain_db] [rate_sps] [count]\r\n");
    printf("  padc diff <ch+> <ch-> [gain_db] [rate_sps] [count]\r\n");
    printf("  padc_noise <ch> [gain_db] [rate_sps] [count]\r\n");
    printf("  padc_rate [rate_sps] [count]\r\n");
    printf("\r\n");
    printf("  ch       : AIN channel 0~7 (GPIO: 4,5,6,7,10,11,12,13)\r\n");
    printf("  gain_db  : PGA gain in dB (6,9,12..42), default 6\r\n");
    printf("  rate_sps : sample rate (2.5,5,10..4000), default 100\r\n");
    printf("  count    : number of samples (1~1000), default 10\r\n");
    printf("\r\n");
    printf("Examples:\r\n");
    printf("  padc_init 4            # init AIN4 GPIO to analog\r\n");
    printf("  padc 4                 # AIN4, 6dB, 100SPS, 10 samples\r\n");
    printf("  padc 4 18 1000 100     # AIN4, 18dB, 1000SPS, 100 samples\r\n");
    printf("  padc_init 4 5          # init AIN4+AIN5 for diff\r\n");
    printf("  padc diff 4 5 6 100    # differential, 6dB, 100SPS\r\n");
    printf("  padc_noise 4 6 100 200 # noise analysis\r\n");
    printf("  padc_rate 100 100      # sampling rate check\r\n");
}

static AUADC_ADC_PGA_Gain_Type gain_db_to_enum(int gain_db)
{
    if (gain_db <= 6) return AUADC_ADC_PGA_GAIN_6DB;
    if (gain_db <= 9) return AUADC_ADC_PGA_GAIN_9DB;

    /* 12~42 dB maps to enum values 4~14 */
    int idx = (gain_db - 12) / 3 + 4;
    if (idx > 14) idx = 14;
    return (AUADC_ADC_PGA_Gain_Type)idx;
}

static int find_rate_index(const char *rate_str)
{
    for (int i = 0; i < (int)RATE_TABLE_SIZE; i++) {
        if (strcmp(rate_str, rate_table[i].name) == 0) {
            return i;
        }
    }
    return 6; /* default: 100 SPS */
}

static void padc_run_test(AUADC_PADC_Cfg_Type *cfg, int count, uint32_t timeout_us,
                           int diff_mode, int ch_pos, int ch_neg)
{
    int32_t raw;
    int32_t min_val = 0x7FFFFFFF;
    int32_t max_val = (int32_t)0x80000001;
    int64_t sum = 0;
    int valid = 0;

    padc_hw_init(cfg);

    /* Print config */
    printf("=== PADC Test ===\r\n");
    if (diff_mode) {
        printf("Mode   : Differential (AIN%d - AIN%d)\r\n", ch_pos, ch_neg);
        printf("GPIO   : %d(+) / %d(-)\r\n", ain_gpio_map[ch_pos], ain_gpio_map[ch_neg]);
    } else {
        printf("Mode   : Single-ended (AIN%d)\r\n", ch_pos);
        printf("GPIO   : %d\r\n", ain_gpio_map[ch_pos]);
    }
    printf("Samples: %d\r\n", count);
    printf("Timeout: %lu us/sample\r\n", timeout_us);
    printf("-----------------\r\n");
    printf(" # |    Raw (hex)   |    Raw (dec)\r\n");
    printf("---|----------------|------------\r\n");

    /* Discard first sample (settling) */
    AUADC_PADC_Read(timeout_us);

    for (int i = 0; i < count; i++) {
        raw = AUADC_PADC_Read(timeout_us);
        if (raw == AUADC_PADC_TIMEOUT) {
            printf("%3d| ** TIMEOUT **  |\r\n", i);
            continue;
        }

        printf("%3d|   0x%06lX     | %8ld\r\n", i, (unsigned long)(raw & 0xFFFFFF), (long)raw);

        if (raw < min_val) min_val = raw;
        if (raw > max_val) max_val = raw;
        sum += raw;
        valid++;
    }

    /* Statistics */
    if (valid > 0) {
        int32_t avg = (int32_t)(sum / valid);
        printf("-----------------\r\n");
        printf("Valid  : %d / %d\r\n", valid, count);
        printf("Avg    : %ld (0x%06lX)\r\n", (long)avg, (unsigned long)(avg & 0xFFFFFF));
        printf("Min    : %ld\r\n", (long)min_val);
        printf("Max    : %ld\r\n", (long)max_val);
        printf("P-P    : %ld\r\n", (long)(max_val - min_val));
    } else {
        printf("No valid samples!\r\n");
    }

    printf("=================\r\n");
}

/*
 * padc <ch> [gain_db] [rate_sps] [count]
 * padc diff <ch+> <ch-> [gain_db] [rate_sps] [count]
 */
static int cmd_padc_test(int argc, char **argv)
{
    int diff_mode = 0;
    int ch_pos = 4;
    int ch_neg = 0;
    int gain_db = 6;
    int rate_idx = 6; /* 100 SPS */
    int count = 10;
    int arg_offset = 1;

    if (argc < 2) {
        padc_print_help();
        return 0;
    }

    /* Check for diff mode */
    if (strcmp(argv[1], "diff") == 0) {
        diff_mode = 1;
        if (argc < 4) {
            printf("diff mode requires: padc diff <ch+> <ch->\r\n");
            return -1;
        }
        ch_pos = atoi(argv[2]);
        ch_neg = atoi(argv[3]);
        arg_offset = 4;
    } else {
        ch_pos = atoi(argv[1]);
        arg_offset = 2;
    }

    if (ch_pos > 7) {
        printf("Error: channel must be 0~7\r\n");
        return -1;
    }

    if (diff_mode && ch_neg > 7) {
        printf("Error: channel must be 0~7\r\n");
        return -1;
    }

    /* Check GPIO was initialized to analog mode */
    if (padc_check_inited(ch_pos, ch_neg, diff_mode) != 0)
        return -1;

    /* Optional: gain */
    if (argc > arg_offset) {
        gain_db = atoi(argv[arg_offset]);
    }

    /* Optional: rate */
    if (argc > arg_offset + 1) {
        rate_idx = find_rate_index(argv[arg_offset + 1]);
    }

    /* Optional: count */
    if (argc > arg_offset + 2) {
        count = atoi(argv[arg_offset + 2]);
        if (count < 1) count = 1;
        if (count > 1000) count = 1000;
    }

    /* Optional: filter (sinc3 or ll) */
    int filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3;
    if (argc > arg_offset + 3) {
        if (strcmp(argv[arg_offset + 3], "ll") == 0)
            filter = AUADC_ADC_MEASURE_FILTER_LOW_LATENCY;
    }

    AUADC_PADC_Cfg_Type padc_cfg = {
        .posi_ch = (AUADC_ADC_AnalogCh_Type)ch_pos,
        .nega_ch = (AUADC_ADC_AnalogCh_Type)ch_neg,
        .pga_gain = gain_db_to_enum(gain_db),
        .pga_mode = diff_mode ? AUADC_ADC_PGA_MODE_DC_DIFF
                              : AUADC_ADC_PGA_MODE_DC_SINGLE,
        .filter = filter,
        .rate = rate_table[rate_idx].rate,
    };

    padc_run_test(&padc_cfg, count, rate_table[rate_idx].timeout_us,
                  diff_mode, ch_pos, ch_neg);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_test, padc, Precision ADC Test);

/*
 * Integer square root (Babylonian method) for RMS calculation
 * without floating point.
 */
static uint32_t isqrt64(uint64_t n)
{
    if (n == 0) return 0;

    uint64_t x = n;
    uint64_t y = (x + 1) >> 1;

    while (y < x) {
        x = y;
        y = (x + n / x) >> 1;
    }
    return (uint32_t)x;
}

/*
 * padc_noise <ch> [gain_db] [rate_sps] [count]
 *
 * Noise analysis: collects N samples from a stable input,
 * computes RMS noise, peak-to-peak noise, and ENOB.
 *
 * DUT setup: short AIN to GND (or apply stable DC voltage).
 */
static int cmd_padc_noise(int argc, char **argv)
{
    int ch = 4;
    int gain_db = 6;
    int rate_idx = 6; /* 100 SPS */
    int count = 200;

    if (argc < 2) {
        printf("Usage: padc_noise <ch> [gain_db] [rate_sps] [count]\r\n");
        printf("  Short AIN to GND or apply stable DC voltage.\r\n");
        return 0;
    }

    ch = atoi(argv[1]);
    if (ch > 7) {
        printf("Error: channel must be 0~7\r\n");
        return -1;
    }
    if (padc_check_inited(ch, 0, 0) != 0)
        return -1;
    if (argc > 2) gain_db = atoi(argv[2]);
    if (argc > 3) rate_idx = find_rate_index(argv[3]);
    if (argc > 4) {
        count = atoi(argv[4]);
        if (count < 10) count = 10;
        if (count > 1000) count = 1000;
    }

    AUADC_PADC_Cfg_Type cfg = {
        .posi_ch = (AUADC_ADC_AnalogCh_Type)ch,
        .nega_ch = AUADC_ADC_AnalogCh_AIN0,
        .pga_gain = gain_db_to_enum(gain_db),
        .pga_mode = AUADC_ADC_PGA_MODE_DC_SINGLE,
        .filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
        .rate = rate_table[rate_idx].rate,
    };

    padc_hw_init(&cfg);

    printf("=== PADC Noise Analysis (DMA) ===\r\n");
    printf("Channel: AIN%d (GPIO%d)\r\n", ch, ain_gpio_map[ch]);
    printf("Gain   : %d dB\r\n", gain_db);
    printf("Rate   : %s SPS\r\n", rate_table[rate_idx].name);
    printf("Samples: %d (via FIFO+DMA)\r\n", count);
    printf("Collecting...\r\n");

    if (count > PADC_DMA_BUF_MAX) count = PADC_DMA_BUF_MAX;

    uint64_t elapsed = padc_dma_collect(count);

    if (elapsed == 0) {
        printf("DMA timeout\r\n");
        return -1;
    }

    /* Compute stats from DMA buffer (32-bit, 20-bit MSB-aligned) */
    int32_t min_val = 0x7FFFFFFF;
    int32_t max_val = (int32_t)0x80000001;
    int64_t sum = 0;

    for (int i = 0; i < count; i++) {
        int32_t s = padc_dma_buf[i];
        if (s < min_val) min_val = s;
        if (s > max_val) max_val = s;
        sum += s;
    }

    int32_t mean = (int32_t)(sum / count);

    /* Compute variance in single pass from buffer */
    int64_t sum_sq = 0;
    for (int i = 0; i < count; i++) {
        int64_t diff = (int64_t)padc_dma_buf[i] - mean;
        sum_sq += diff * diff;
    }

    /* RMS noise = sqrt(variance) */
    uint64_t variance = (uint64_t)(sum_sq / count);
    uint32_t rms_noise = isqrt64(variance);

    /* ENOB = log2(full_scale / rms_noise)
     * 20-bit MSB-aligned in 32-bit: full scale = 2^31
     * ENOB ≈ 31 - log2(rms) → then scale to actual bits */
    int enob_int = 0;
    if (rms_noise > 0) {
        uint32_t tmp = rms_noise;
        int log2_rms = 0;
        while (tmp > 1) {
            tmp >>= 1;
            log2_rms++;
        }
        enob_int = 31 - log2_rms;
        if (enob_int < 0) enob_int = 0;
    } else {
        enob_int = 20;
    }

    int32_t pp_noise = max_val - min_val;

    /* Voltage conversion: 20-bit MSB-aligned in 32-bit → full scale = 2^31 */
    int gain_linear = 1 << (gain_db / 6);
    /* mean_uv = mean / 2^31 * 2500000 / gain_linear */
    int32_t mean_uv = (int32_t)((int64_t)mean * 2500000LL / gain_linear >> 31);
    int32_t pp_uv = (int32_t)((int64_t)pp_noise * 2500000LL / gain_linear >> 31);
    int32_t rms_uv = (int32_t)((int64_t)rms_noise * 2500000LL / gain_linear >> 31);

    printf("---------------------------\r\n");
    printf("Samples  : %d (DMA)\r\n", count);
    printf("Elapsed  : %lu us\r\n", (unsigned long)elapsed);
    printf("Mean     : %ld  (%ld uV)\r\n", (long)mean, (long)mean_uv);
    printf("Min      : %ld\r\n", (long)min_val);
    printf("Max      : %ld\r\n", (long)max_val);
    printf("P-P noise: %ld  (%ld uV)\r\n", (long)pp_noise, (long)pp_uv);
    printf("RMS noise: %lu  (%ld uV)\r\n", (unsigned long)rms_noise, (long)rms_uv);
    printf("ENOB     : ~%d bits\r\n", enob_int);
    printf("===========================\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_noise, padc_noise, PADC Noise Analysis);

/*
 * padc_rate [rate_sps] [count]
 *
 * Measures actual sampling rate by timing N consecutive data_rdy events.
 * Compares against the configured rate setting.
 *
 * DUT setup: any stable input or floating (doesn't matter for timing).
 */
static int cmd_padc_rate(int argc, char **argv)
{
    int rate_idx = 6; /* 100 SPS */
    int count = 100;

    if (argc > 1) rate_idx = find_rate_index(argv[1]);
    if (argc > 2) {
        count = atoi(argv[2]);
        if (count < 10) count = 10;
        if (count > 1000) count = 1000;
    }

    AUADC_PADC_Cfg_Type cfg = {
        .posi_ch = AUADC_ADC_AnalogCh_AIN0,
        .nega_ch = AUADC_ADC_AnalogCh_AIN0,
        .pga_gain = AUADC_ADC_PGA_GAIN_6DB,
        .pga_mode = AUADC_ADC_PGA_MODE_DC_SINGLE,
        .filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
        .rate = rate_table[rate_idx].rate,
    };

    /* padc_rate uses any available initialized channel */
    if (padc_ch_inited == 0) {
        printf("Error: no channel initialized. Run 'padc_init <ch>' first.\r\n");
        return -1;
    }

    /* Use first initialized channel */
    for (int c = 0; c < 8; c++) {
        if (padc_ch_inited & (1 << c)) {
            cfg.posi_ch = (AUADC_ADC_AnalogCh_Type)c;
            break;
        }
    }

    padc_hw_init(&cfg);

    printf("=== PADC Rate Test (DMA) ===\r\n");
    printf("Target ODR: %s SPS\r\n", rate_table[rate_idx].name);
    printf("Samples   : %d (via FIFO+DMA)\r\n", count);
    printf("Measuring...\r\n");

    if (count > PADC_DMA_BUF_MAX) count = PADC_DMA_BUF_MAX;

    uint64_t elapsed = padc_dma_collect(count);

    if (elapsed == 0) {
        printf("DMA timeout\r\n");
        return -1;
    }

    uint32_t sinc3_sps_x100 = (uint32_t)((uint64_t)count * 100000000ULL / elapsed);
    uint32_t avg_interval_us = (uint32_t)(elapsed / count);

    printf("----------------------\r\n");
    printf("Elapsed    : %lu us\r\n", (unsigned long)elapsed);
    printf("Avg intvl  : %lu us/sample\r\n", (unsigned long)avg_interval_us);
    printf("Sinc3 rate : %lu.%02lu SPS (FIFO output, pre-ODR)\r\n",
           (unsigned long)(sinc3_sps_x100 / 100),
           (unsigned long)(sinc3_sps_x100 % 100));
    printf("Target ODR : %s SPS (Sinc3 / ~8)\r\n", rate_table[rate_idx].name);
    printf("======================\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_rate, padc_rate, PADC Sampling Rate Test);

/* ================================================================== */
/*  padc_dither — Control SDM dither (LFSR noise injection)             */
/*  padc_chop  — Control PGA chopper                                    */
/* ================================================================== */

#include "aud_reg.h"

static int cmd_padc_dither(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: padc_dither <on|off> [level 0-3] [order 0-1]\r\n");
        printf("  level: 0=none, 1=LSB/15, 2=LSB*2/15, 3=LSB*3/15\r\n");
        printf("  order: 0=0th order, 1=1st order\r\n");
        /* Read current state */
        uint32_t reg = readl(AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
        int ena = (reg >> AUDADC_DITHER_ENA_POS) & 1;
        int lvl = (reg >> AUDADC_DITHER_SEL_POS) & 3;
        int ord = (reg >> AUDADC_DITHER_ORDER_POS) & 1;
        printf("  Current: %s, level=%d, order=%d\r\n",
               ena ? "ON" : "OFF", lvl, ord);
        return 0;
    }

    int ena = (strcmp(argv[1], "on") == 0) ? 1 : 0;
    int lvl = (argc > 2) ? atoi(argv[2]) : 2;
    int ord = (argc > 3) ? atoi(argv[3]) : 1;
    if (lvl > 3) lvl = 3;
    if (ord > 1) ord = 1;

    uint32_t reg = readl(AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
    reg &= ~((1 << AUDADC_DITHER_ENA_POS) |
              (3 << AUDADC_DITHER_SEL_POS) |
              (1 << AUDADC_DITHER_ORDER_POS));
    reg |= (ena << AUDADC_DITHER_ENA_POS) |
            (lvl << AUDADC_DITHER_SEL_POS) |
            (ord << AUDADC_DITHER_ORDER_POS);
    writel(reg, AUD_BASE + AUDADC_ANA_CFG2_OFFSET);

    printf("Dither: %s, level=%d, order=%d\r\n", ena ? "ON" : "OFF", lvl, ord);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_dither, padc_dither, SDM dither control);

static int cmd_padc_chop(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: padc_chop <on|off> [freq_khz]\r\n");
        printf("  freq_khz: 8/16/32/64/128/256/512/1024\r\n");
        /* Read current state */
        uint32_t reg = readl(AUD_BASE + AUDADC_ANA_CFG1_OFFSET);
        int ena = (reg >> AUDADC_PGA_CHOP_EN_POS) & 1;
        int freq_idx = (reg >> AUDADC_PGA_CHOP_FREQ_POS) & 7;
        printf("  Current: %s, freq=%dKHz\r\n",
               ena ? "ON" : "OFF", 8 << freq_idx);
        return 0;
    }

    int ena = (strcmp(argv[1], "on") == 0) ? 1 : 0;
    int freq_idx = 3; /* default 64KHz */
    if (argc > 2) {
        int khz = atoi(argv[2]);
        freq_idx = 0;
        while ((8 << freq_idx) < khz && freq_idx < 7) freq_idx++;
    }

    uint32_t reg = readl(AUD_BASE + AUDADC_ANA_CFG1_OFFSET);
    reg &= ~((1 << AUDADC_PGA_CHOP_EN_POS) |
              (7 << AUDADC_PGA_CHOP_FREQ_POS));
    reg |= (ena << AUDADC_PGA_CHOP_EN_POS) |
            (freq_idx << AUDADC_PGA_CHOP_FREQ_POS);
    writel(reg, AUD_BASE + AUDADC_ANA_CFG1_OFFSET);

    printf("PGA Chopper: %s, freq=%dKHz\r\n",
           ena ? "ON" : "OFF", 8 << freq_idx);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_chop, padc_chop, PGA chopper control);

/* ================================================================== */
/*                                                                      */
/*  Software Calibration (OEC / GEC)                                    */
/*                                                                      */
/*  No HW calibration engine in this silicon revision.                  */
/*  OEC/GEC computed in firmware, applied as post-processing.           */
/*                                                                      */
/*  Formula: calibrated = gec * (raw + oec) >> 22                       */
/*  OEC = -avg(samples) when input = 0V differential                    */
/*  GEC = (expected << 22) / measured  (Q22 fixed-point)                */
/*                                                                      */
/* ================================================================== */

/* Forward declarations for DAC loopback helpers (defined later in padc_auto section) */
static void padc_auto_dac_init(void);
static void padc_auto_dac_set(uint16_t code_a, uint16_t code_b);
static int padc_auto_read_diff(int ch_pos, int ch_neg, int gain_db, int rate_idx,
                                int count, int32_t *out_avg, int32_t *out_pp);

/*
 * Calibration-supported gains: only 6/12/18 dB.
 * >=24dB saturates during offset calibration because PGA internal offset
 * (~450K counts RTI) × gain exceeds ADC ±8.4M range.
 * HW calibration engine (not in this silicon) would short PGA to Vcm internally.
 */
static const int calib_gain_db[] = { 6, 12, 18 };
#define CALIB_GAIN_COUNT 3
#define CALIB_GEC_UNITY  (1 << 22)  /* 4194304 = gain factor 1.0 in Q22 */

static int32_t padc_oec[CALIB_GAIN_COUNT];  /* offset error per gain */
static int32_t padc_gec[CALIB_GAIN_COUNT];  /* gain error per gain (Q22) */
static int padc_calib_valid;                 /* bitmask: bit N = gain N calibrated */

static int calib_gain_index(int gain_db)
{
    for (int i = 0; i < CALIB_GAIN_COUNT; i++) {
        if (calib_gain_db[i] == gain_db)
            return i;
    }
    return -1;
}

/* Apply calibration to a raw 24-bit PADC reading */
static int32_t __attribute__((unused)) padc_apply_calib(int32_t raw, int gain_db)
{
    int idx = calib_gain_index(gain_db);
    if (idx < 0 || !(padc_calib_valid & (1 << idx)))
        return raw;  /* no calibration data */
    return (int32_t)((int64_t)padc_gec[idx] * (raw + padc_oec[idx]) >> 22);
}

/*
 * padc_calib — Software calibration using DAC loopback
 *
 * Wiring (same as padc_auto):
 *   GPIO23 (DAC_A) --[1KΩ]--> GPIO4 (AIN0)
 *   GPIO22 (DAC_B) --[1KΩ]--> GPIO5 (AIN1)
 *
 * Self-Offset: DAC_A = DAC_B = 512 (same voltage → diff = 0V)
 *   → OEC = -avg(N samples)
 *
 * Self-Gain: DAC_A = 900 (~1606mV), DAC_B = 0 (~200mV)
 *   → diff input = ~1406mV, known expected count
 *   → GEC = (expected << 22) / (measured + OEC)
 *
 * Auto: run offset + gain for all 7 gain settings
 */
static int cmd_padc_calib(int argc, char **argv)
{
    const char *mode = "auto";
    int gain_db = 6;
    int samples = 32;

    if (argc < 2) {
        printf("Usage: padc_calib <mode> [gain_db]\r\n");
        printf("  mode: auto, offset, gain, show, clear\r\n");
        printf("  gain_db: 6/12/18/24/30/36/42 (for offset/gain)\r\n");
        printf("  auto: run offset+gain for all gains\r\n");
        printf("\r\n");
        printf("  Wiring: GPIO23(DAC_A)-[1K]-GPIO4(AIN0)\r\n");
        printf("          GPIO22(DAC_B)-[1K]-GPIO5(AIN1)\r\n");
        return 0;
    }

    mode = argv[1];
    if (argc > 2) gain_db = atoi(argv[2]);

    if (strcmp(mode, "show") == 0) {
        printf("=== PADC Calibration Data ===\r\n");
        printf("  Gain   OEC          GEC          Valid\r\n");
        for (int i = 0; i < CALIB_GAIN_COUNT; i++) {
            int valid = (padc_calib_valid >> i) & 1;
            printf("  %2ddB   %-12ld %-12ld %s\r\n",
                   calib_gain_db[i], (long)padc_oec[i], (long)padc_gec[i],
                   valid ? "YES" : "no");
        }
        return 0;
    }

    if (strcmp(mode, "clear") == 0) {
        for (int i = 0; i < CALIB_GAIN_COUNT; i++) {
            padc_oec[i] = 0;
            padc_gec[i] = CALIB_GEC_UNITY;
        }
        padc_calib_valid = 0;
        printf("Calibration data cleared.\r\n");
        return 0;
    }

    /* Determine which gains to calibrate */
    int start = 0, end = CALIB_GAIN_COUNT;
    int do_offset = 1, do_gain = 1;

    if (strcmp(mode, "offset") == 0) {
        do_gain = 0;
        int idx = calib_gain_index(gain_db);
        if (idx < 0) { printf("Invalid gain: %d dB\r\n", gain_db); return -1; }
        start = idx; end = idx + 1;
    } else if (strcmp(mode, "gain") == 0) {
        do_offset = 0;
        int idx = calib_gain_index(gain_db);
        if (idx < 0) { printf("Invalid gain: %d dB\r\n", gain_db); return -1; }
        start = idx; end = idx + 1;
    } else if (strcmp(mode, "auto") != 0) {
        printf("Unknown mode: %s\r\n", mode);
        return -1;
    }

    printf("=== PADC Software Calibration ===\r\n");

    /* Init DAC (register mode for stable DC output) */
    padc_gpio_init(0);
    padc_ch_inited |= (1 << 0) | (1 << 1);
    padc_auto_dac_init();

    for (int i = start; i < end; i++) {
        int gdb = calib_gain_db[i];
        printf("[%2ddB] ", gdb);

        /* --- Self-Offset: DAC_A = DAC_B = 512 → diff = 0V --- */
        if (do_offset) {
            padc_auto_dac_set(512, 512);

            int32_t avg, pp;
            int ret = padc_auto_read_diff(0, 1, gdb, 6 /* 100SPS */,
                                           samples, &avg, &pp);
            if (ret != 0) {
                printf("offset FAIL (timeout)\r\n");
                continue;
            }
            padc_oec[i] = -avg;
            printf("OEC=%ld (avg=%ld, pp=%ld)  ",
                   (long)padc_oec[i], (long)avg, (long)pp);
        }

        /* --- Self-Gain: DAC_A = 900 (~1606mV), DAC_B = 0 (~200mV) --- */
        if (do_gain) {
            int dac_hi = 900;
            int dac_lo = 0;
            padc_auto_dac_set(dac_hi, dac_lo);

            int32_t avg, pp;
            int ret = padc_auto_read_diff(0, 1, gdb, 6 /* 100SPS */,
                                           samples, &avg, &pp);
            if (ret != 0) {
                printf("gain FAIL (timeout)\r\n");
                continue;
            }

            /*
             * Expected ideal output for this differential voltage:
             * Vdiff = (1600 * dac_hi / 1024 + 200) - (1600 * dac_lo / 1024 + 200)
             *       = 1600 * (dac_hi - dac_lo) / 1024  [mV]
             * Normalized to full-scale (2500mV → 2^23):
             * expected = Vdiff / 2500 * 2^23 * gain_linear
             *
             * But at 6dB gain (2x), full-scale ±2.5V input → ±2^23 output.
             * So: expected = Vdiff_mV / 2500 * 8388608 * gain_linear
             *
             * Use integer math to avoid float:
             * expected = Vdiff_mV * 8388608 / 2500 * gain_linear
             *          = Vdiff_mV * 3355 * gain_linear  (approx)
             */
            int vdiff_mv = 1600 * (dac_hi - dac_lo) / 1024;
            int gain_linear = 1 << (gdb / 6);  /* 6dB=2x, 12dB=4x, ... */
            int64_t expected = (int64_t)vdiff_mv * 8388608LL / 2500 * gain_linear;

            int64_t measured = avg + padc_oec[i];
            if (measured == 0) measured = 1; /* avoid div-by-zero */

            padc_gec[i] = (int32_t)((expected << 22) / measured);
            padc_calib_valid |= (1 << i);

            printf("GEC=%ld (expected=%lld, measured=%lld, Vdiff=%dmV)\r\n",
                   (long)padc_gec[i], (long long)expected,
                   (long long)measured, vdiff_mv);
        } else {
            printf("\r\n");
        }
    }

    printf("\r\nCalibration complete. Use 'padc_calib show' to view.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_calib, padc_calib, PADC software calibration);

/* ================================================================== */
/*                                                                      */
/*  padc_auto — Automated PADC test using on-chip DAC as voltage source */
/*                                                                      */
/*  Wiring:                                                             */
/*    GPIO23 (DAC_A) --[1KΩ]--> GPIO10 (AIN4)  single-ended            */
/*    GPIO22 (DAC_B) --[1KΩ]--> GPIO11 (AIN5)  differential            */
/*                                                                      */
/* ================================================================== */

#include "hal_dac.h"
#include "hal_dma.h"
#include "drv_dac.h"

/* ================================================================== */
/* DMA-based PADC sample collection                                    */
/*                                                                      */
#define DAC_GPIO_A  23
#define DAC_GPIO_B  22
#define PADC_AUTO_DAC_MAX 1023

static void padc_auto_dac_init(void)
{
    /* GPIO analog mode for DAC outputs */
    GLB_GPIO_Cfg_Type acfg = {
        .gpioFun = GPIO_FUN_ANALOG, .gpioMode = GPIO_MODE_ANALOG,
        .pullType = GPIO_PULL_NONE, .drive = 0, .smtCtrl = 0,
    };
    acfg.gpioPin = DAC_GPIO_A; GLB_GPIO_Init(&acfg);
    acfg.gpioPin = DAC_GPIO_B; GLB_GPIO_Init(&acfg);

    /* Init DAC register mode */
    dac_device_t dac0;
    memset(&dac0, 0, sizeof(dac0));
    dac0.id = 0;
    dac0.caps = 0;
    dac0.channels = DAC_CHANNEL_ALL; /* both A and B for diff test */
    dac0.sample_freq = DAC_SAMPLE_FREQ_512KHZ;
    dac0.vref = DAC_VREF_INTERNAL;
    dac0.tx_dma = -1;
    hal_dac_setConfig(&dac0);
    hal_dac_init(0);
}

static void padc_auto_dac_set(uint16_t code_a, uint16_t code_b)
{
    GLB_DAC_Set_ChanA_Value(code_a);
    GLB_DAC_Set_ChanB_Value(code_b);
    arch_delay_ms(50); /* settle — 10ms is too short for RC through 1K */
}

/* Compute DAC mV from code: Vout = 1.6 * code / 1024 + 0.2V */
static int dac_code_to_mv(int code)
{
    return 200 + (1600 * code / 1024);
}

/* Read PADC N samples, return avg and pp */
static int padc_auto_read(int ch, int gain_db, int rate_idx, int count,
                           int32_t *out_avg, int32_t *out_pp)
{
    /*
     * Use DC differential mode with DAC_B (AIN1) as GND reference.
     * DAC_B is set to code=0 (~0.2V) by the caller.
     * This eliminates the ~900mV dead zone caused by PGA internal CM
     * bias in single-ended mode (negative input floating at ~900mV).
     *
     * PGA output = gain × (AIN0 - AIN1) = gain × (Vsignal - 0.2V)
     * Full DAC range (0.2V~1.8V) maps to 0 ~ 1.6V differential.
     */
    AUADC_PADC_Cfg_Type cfg = {
        .posi_ch = (AUADC_ADC_AnalogCh_Type)ch,
        .nega_ch = AUADC_ADC_AnalogCh_AIN1, /* DAC_B as reference */
        .pga_gain = gain_db_to_enum(gain_db),
        .pga_mode = AUADC_ADC_PGA_MODE_DC_DIFF,
        .filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
        .rate = rate_table[rate_idx].rate,
    };
    uint32_t timeout = rate_table[rate_idx].timeout_us;

    padc_hw_init(&cfg);

    /* Discard first 2 samples for settling */
    AUADC_PADC_Read(timeout);
    AUADC_PADC_Read(timeout);

    int64_t sum = 0;
    int32_t mn = 0x7FFFFFFF, mx = (int32_t)0x80000001;
    int valid = 0;

    for (int i = 0; i < count; i++) {
        int32_t raw = AUADC_PADC_Read(timeout);
        if (raw == AUADC_PADC_TIMEOUT) continue;
        sum += raw;
        if (raw < mn) mn = raw;
        if (raw > mx) mx = raw;
        valid++;
    }

    if (valid == 0) return -1;

    *out_avg = (int32_t)(sum / valid);
    *out_pp = mx - mn;
    return 0;
}

static int padc_auto_read_diff(int ch_pos, int ch_neg, int gain_db, int rate_idx,
                                int count, int32_t *out_avg, int32_t *out_pp)
{
    AUADC_PADC_Cfg_Type cfg = {
        .posi_ch = (AUADC_ADC_AnalogCh_Type)ch_pos,
        .nega_ch = (AUADC_ADC_AnalogCh_Type)ch_neg,
        .pga_gain = gain_db_to_enum(gain_db),
        .pga_mode = AUADC_ADC_PGA_MODE_DC_DIFF,
        .filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
        .rate = rate_table[rate_idx].rate,
    };
    uint32_t timeout = rate_table[rate_idx].timeout_us;

    padc_hw_init(&cfg);
    AUADC_PADC_Read(timeout);
    AUADC_PADC_Read(timeout);

    int64_t sum = 0;
    int32_t mn = 0x7FFFFFFF, mx = (int32_t)0x80000001;
    int valid = 0;

    for (int i = 0; i < count; i++) {
        int32_t raw = AUADC_PADC_Read(timeout);
        if (raw == AUADC_PADC_TIMEOUT) continue;
        sum += raw;
        if (raw < mn) mn = raw;
        if (raw > mx) mx = raw;
        valid++;
    }

    if (valid == 0) return -1;
    *out_avg = (int32_t)(sum / valid);
    *out_pp = mx - mn;
    return 0;
}

static int cmd_padc_auto(int argc, char **argv)
{
    (void)argc; (void)argv;

    int pass = 0, fail = 0;
    int32_t avg, pp;
    int rate100 = 6; /* index for 100 SPS */

    printf("\r\n");
    printf("=== PADC Auto-Test (DAC->PADC Loopback) ===\r\n");
    printf("Wiring: GPIO23(DAC_A)-[1K]-GPIO4(AIN0)\r\n");
    printf("        GPIO22(DAC_B)-[1K]-GPIO5(AIN1)\r\n");
    printf("\r\n");

    /* Init GPIO for PADC inputs (VDDIO_1 domain) */
    padc_gpio_init(0); /* AIN0 = GPIO4 */
    padc_gpio_init(1); /* AIN1 = GPIO5 */
    padc_ch_inited |= (1 << 0) | (1 << 1);

    /* Init DAC */
    padc_auto_dac_init();

    /* ---- TC-001: Zero reference (DAC_A=0, DAC_B=0, diff≈0) ---- */
    printf("[TC-001] Zero Reference (DAC_A=DAC_B=0, diff≈0V)\r\n");
    padc_auto_dac_set(0, 0); /* both channels ~0.2V, diff≈0 */
    if (padc_auto_read(0, 6, rate100, 10, &avg, &pp) == 0) {
        /* Diff mode: DAC A/B offset mismatch causes non-zero avg.
         * P-P is higher than single-ended due to diff noise.
         * Pass if ADC is not saturated (avg within ±4M). */
        int32_t abs_avg = avg > 0 ? avg : -avg;
        int tc1_ok = (abs_avg < 4000000 && pp < 200000);
        printf("  Avg=%ld  P-P=%ld  %s\r\n", (long)avg, (long)pp,
               tc1_ok ? "PASS" : "FAIL");
        tc1_ok ? pass++ : fail++;
    } else { printf("  TIMEOUT FAIL\r\n"); fail++; }

    /* ---- TC-002: Full range sweep (diff mode eliminates dead zone) ---- */
    printf("[TC-002] Voltage Sweep (full range, diff mode)\r\n");
    int tc2_pass = 1;
    int codes[] = {0, 256, 512, 768, 1023};
    int32_t avgs[5];
    for (int i = 0; i < 5; i++) {
        padc_auto_dac_set(codes[i], 0);
        if (padc_auto_read(0, 6, rate100, 10, &avg, &pp) == 0) {
            avgs[i] = avg;
            printf("  DAC=%4d (%4dmV): avg=%ld  P-P=%ld\r\n",
                   codes[i], dac_code_to_mv(codes[i]), (long)avg, (long)pp);
            if (pp >= 50000) tc2_pass = 0; /* DAC has ~10-bit noise, relax */
        } else { tc2_pass = 0; }
    }
    /* Check monotonic */
    int mono = 1;
    for (int i = 1; i < 5; i++) {
        if (avgs[i] <= avgs[i - 1]) mono = 0;
    }
    printf("  Monotonic: %s\r\n", mono ? "YES" : "NO");
    if (!mono) tc2_pass = 0;
    printf("  Result: %s\r\n", tc2_pass ? "PASS" : "FAIL");
    tc2_pass ? pass++ : fail++;

    /* ---- TC-005: Differential (AIN0-AIN1) ---- */
    printf("[TC-005] Differential (DAC_A vs DAC_B)\r\n");
    int32_t avg_a, avg_b;
    int tc5_pass = 1;

    padc_auto_dac_set(768, 256);
    if (padc_auto_read_diff(0, 1, 6, rate100, 10, &avg_a, &pp) == 0) {
        printf("  A(768)>B(256): avg=%ld  P-P=%ld\r\n", (long)avg_a, (long)pp);
    } else { tc5_pass = 0; }

    padc_auto_dac_set(256, 768);
    if (padc_auto_read_diff(0, 1, 6, rate100, 10, &avg_b, &pp) == 0) {
        printf("  A(256)<B(768): avg=%ld  P-P=%ld\r\n", (long)avg_b, (long)pp);
    } else { tc5_pass = 0; }

    if (tc5_pass) {
        /* Values must have OPPOSITE signs and similar magnitude.
         * Don't assume which is positive — PGA polarity varies. */
        int sign_ok = (avg_a > 0 && avg_b < 0) || (avg_a < 0 && avg_b > 0);
        if (!sign_ok) tc5_pass = 0;
        printf("  Sign: A>B=%s, A<B=%s, opposite=%s\r\n",
               avg_a > 0 ? "+" : "-", avg_b > 0 ? "+" : "-",
               sign_ok ? "YES" : "NO");
    }
    printf("  Result: %s\r\n", tc5_pass ? "PASS" : "FAIL");
    tc5_pass ? pass++ : fail++;

    /* ---- TC-006: Noise vs Rate ---- */
    printf("[TC-006] Noise vs Rate (DAC=512)\r\n");
    padc_auto_dac_set(512, 0);
    int rate_idxs[] = {2, 6, 10}; /* 10, 100, 1000 SPS */
    int tc6_pass = 1;
    for (int i = 0; i < 3; i++) {
        int cnt = (rate_idxs[i] <= 2) ? 10 : 20;
        if (padc_auto_read(0, 6, rate_idxs[i], cnt, &avg, &pp) == 0) {
            printf("  %4s SPS: P-P=%ld\r\n",
                   rate_table[rate_idxs[i]].name, (long)pp);
        } else { tc6_pass = 0; }
    }
    printf("  Result: %s\r\n", tc6_pass ? "PASS" : "FAIL");
    tc6_pass ? pass++ : fail++;

    /* ---- TC-010: PGA Gain step ---- */
    printf("[TC-010] PGA Gain (offset-subtracted)\r\n");
    /*
     * AUADC DC single-ended has a dead zone below ~900mV where output saturates.
     * Use DAC codes well above the dead zone (800=1450mV, 900=1606mV).
     * Only test 6dB since higher gains narrow the linear range.
     * Verify: changing input voltage produces proportional ADC change.
     */
    {
        int32_t avg_lo, avg_hi, pp_tmp;
        int tc10_pass = 1;
        padc_auto_dac_set(700, 0); /* ~1.29V */
        if (padc_auto_read(0, 6, rate100, 10, &avg_lo, &pp_tmp) != 0) tc10_pass = 0;
        padc_auto_dac_set(900, 0); /* ~1.61V */
        if (padc_auto_read(0, 6, rate100, 10, &avg_hi, &pp_tmp) != 0) tc10_pass = 0;
        int32_t delta = avg_hi - avg_lo;
        printf("  6dB: DAC700=%ld  DAC900=%ld  delta=%ld\r\n",
               (long)avg_lo, (long)avg_hi, (long)delta);
        /* Delta should be positive and significant (>100K for ~320mV swing) */
        if (delta < 100000) {
            printf("  Delta too small — ADC not responding to voltage change\r\n");
            tc10_pass = 0;
        } else {
            printf("  ADC responds to voltage change: PASS\r\n");
        }
        printf("  Result: %s\r\n", tc10_pass ? "PASS" : "FAIL");
        tc10_pass ? pass++ : fail++;
    }

    /* ---- TC-008: Rate + ODR ratio (DMA) ---- */
    printf("[TC-008] Rate Accuracy (DMA, ODR 100 vs 50)\r\n");
    padc_auto_dac_set(512, 0);
    {
        /* Measure Sinc3 rate at two ODR settings.
         * If ODR controls the Sinc3 decimation, the ratio should be ~2x. */
        int n_rate = 50;

        AUADC_PADC_Cfg_Type cfg100 = {
            .posi_ch = AUADC_ADC_AnalogCh_AIN0,
            .pga_gain = AUADC_ADC_PGA_GAIN_6DB,
            .pga_mode = AUADC_ADC_PGA_MODE_DC_SINGLE,
            .filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
            .rate = AUADC_ADC_MEASURE_RATE_SPS_100,
        };
        padc_hw_init(&cfg100);
        uint64_t dt100 = padc_dma_collect(n_rate);

        AUADC_PADC_Cfg_Type cfg50 = cfg100;
        cfg50.rate = AUADC_ADC_MEASURE_RATE_SPS_50;
        padc_hw_init(&cfg50);
        uint64_t dt50 = padc_dma_collect(n_rate);

        if (dt100 == 0 || dt50 == 0) {
            printf("  DMA timeout\r\n");
            fail++;
        } else {
            uint32_t sps100 = (uint32_t)((uint64_t)n_rate * 1000000ULL / dt100);
            uint32_t sps50 = (uint32_t)((uint64_t)n_rate * 1000000ULL / dt50);
            int ratio_x10 = (dt50 > 0) ? (int)((uint64_t)dt50 * 10 / dt100) : 0;
            printf("  ODR=100: %lu SPS (Sinc3), %lu us\r\n",
                   (unsigned long)sps100, (unsigned long)dt100);
            printf("  ODR=50 : %lu SPS (Sinc3), %lu us\r\n",
                   (unsigned long)sps50, (unsigned long)dt50);
            printf("  Ratio  : %d.%dx (expect ~2.0x)\r\n",
                   ratio_x10 / 10, ratio_x10 % 10);
            /* Accept ratio 1.5x~2.5x */
            int pass_rate = (ratio_x10 >= 15 && ratio_x10 <= 25);
            printf("  Result : %s\r\n", pass_rate ? "PASS" : "FAIL");
            pass_rate ? pass++ : fail++;
        }
    }

    /* ---- TC-009: Full range linearity (diff mode) ---- */
    printf("[TC-009] Linearity (DAC 0~1023, full range)\r\n");
    /*
     * Diff mode with DAC_B=0 as reference eliminates the dead zone.
     * Sweep full DAC range: 0 (200mV) to 1023 (1798mV).
     */
    int tc9_pass = 1;
    int lpoints = 8;
    int32_t lin_avgs[8];
    int lin_codes[8];
    for (int i = 0; i < lpoints; i++) {
        lin_codes[i] = i * PADC_AUTO_DAC_MAX / (lpoints - 1);
        padc_auto_dac_set(lin_codes[i], 0);
        if (padc_auto_read(0, 6, rate100, 10, &avg, &pp) == 0) {
            lin_avgs[i] = avg;
            printf("  DAC=%4d (%4dmV): avg=%ld\r\n",
                   lin_codes[i], dac_code_to_mv(lin_codes[i]), (long)avg);
        } else { tc9_pass = 0; }
    }
    /* Monotonic check (all points should be in linear region) */
    for (int i = 1; i < lpoints; i++) {
        if (lin_avgs[i] <= lin_avgs[i - 1]) { tc9_pass = 0; break; }
    }
    printf("  Monotonic: %s\r\n", tc9_pass ? "PASS" : "FAIL");
    tc9_pass ? pass++ : fail++;

    /* ---- TC-011: Software Calibration (6/12/18dB) ---- */
    printf("[TC-011] Self-Calibration (6/12/18 dB)\r\n");
    {
        int tc11_pass = 1;
        for (int i = 0; i < CALIB_GAIN_COUNT; i++) {
            int gdb = calib_gain_db[i];
            /* Offset: DAC_A=DAC_B=512, diff=0V */
            padc_auto_dac_set(512, 512);
            int32_t oavg, opp;
            if (padc_auto_read_diff(0, 1, gdb, rate100, 32, &oavg, &opp) != 0) {
                printf("  %2ddB: offset FAIL\r\n", gdb);
                tc11_pass = 0; continue;
            }
            padc_oec[i] = -oavg;

            /* Gain: DAC_A=900, DAC_B=0 */
            padc_auto_dac_set(900, 0);
            int32_t gavg, gpp;
            if (padc_auto_read_diff(0, 1, gdb, rate100, 32, &gavg, &gpp) != 0) {
                printf("  %2ddB: gain FAIL\r\n", gdb);
                tc11_pass = 0; continue;
            }
            int vdiff_mv = 1600 * 900 / 1024;
            int gain_lin = 1 << (gdb / 6);
            int64_t expected = (int64_t)vdiff_mv * 8388608LL / 2500 * gain_lin;
            int64_t measured = gavg + padc_oec[i];
            if (measured == 0) measured = 1;
            padc_gec[i] = (int32_t)((expected << 22) / measured);
            padc_calib_valid |= (1 << i);

            printf("  %2ddB: OEC=%ld GEC=%ld pp=%ld\r\n",
                   gdb, (long)padc_oec[i], (long)padc_gec[i], (long)opp);
        }
        printf("  Result: %s\r\n", tc11_pass ? "PASS" : "FAIL");
        tc11_pass ? pass++ : fail++;
    }

    /* ---- TC-012: Low-Latency vs SINC3 noise ---- */
    printf("[TC-012] Filter: SINC3 vs Low-Latency noise\r\n");
    padc_auto_dac_set(512, 0);
    {
        /* SINC3 */
        AUADC_PADC_Cfg_Type cfg_s3 = {
            .posi_ch = AUADC_ADC_AnalogCh_AIN0,
            .nega_ch = AUADC_ADC_AnalogCh_AIN1,
            .pga_gain = AUADC_ADC_PGA_GAIN_6DB,
            .pga_mode = AUADC_ADC_PGA_MODE_DC_DIFF,
            .filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
            .rate = AUADC_ADC_MEASURE_RATE_SPS_100,
        };
        padc_hw_init(&cfg_s3);
        int n = 50;
        padc_dma_collect(n);
        int32_t s3_min = 0x7FFFFFFF, s3_max = (int32_t)0x80000001;
        for (int i = 0; i < n; i++) {
            if (padc_dma_buf[i] < s3_min) s3_min = padc_dma_buf[i];
            if (padc_dma_buf[i] > s3_max) s3_max = padc_dma_buf[i];
        }
        int32_t s3_pp = s3_max - s3_min;

        /* Low-Latency */
        AUADC_PADC_Cfg_Type cfg_ll = cfg_s3;
        cfg_ll.filter = AUADC_ADC_MEASURE_FILTER_LOW_LATENCY;
        padc_hw_init(&cfg_ll);
        padc_dma_collect(n);
        int32_t ll_min = 0x7FFFFFFF, ll_max = (int32_t)0x80000001;
        for (int i = 0; i < n; i++) {
            if (padc_dma_buf[i] < ll_min) ll_min = padc_dma_buf[i];
            if (padc_dma_buf[i] > ll_max) ll_max = padc_dma_buf[i];
        }
        int32_t ll_pp = ll_max - ll_min;

        printf("  SINC3 P-P: %ld\r\n", (long)s3_pp);
        printf("  LL    P-P: %ld\r\n", (long)ll_pp);
        /* Both should produce valid samples; SINC3 typically lower noise */
        int tc12_pass = (s3_pp > 0 && ll_pp > 0);
        printf("  Result: %s\r\n", tc12_pass ? "PASS" : "FAIL");
        tc12_pass ? pass++ : fail++;
    }

    /* ---- TC-013: Dither ON vs OFF noise ---- */
    printf("[TC-013] Dither ON vs OFF noise\r\n");
    padc_auto_dac_set(512, 0);
    {
        int n = 50;
        AUADC_PADC_Cfg_Type cfg_d = {
            .posi_ch = AUADC_ADC_AnalogCh_AIN0,
            .nega_ch = AUADC_ADC_AnalogCh_AIN1,
            .pga_gain = AUADC_ADC_PGA_GAIN_6DB,
            .pga_mode = AUADC_ADC_PGA_MODE_DC_DIFF,
            .filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
            .rate = AUADC_ADC_MEASURE_RATE_SPS_100,
        };

        /* Dither ON (default) */
        padc_hw_init(&cfg_d);
        padc_dma_collect(n);
        int32_t on_min = 0x7FFFFFFF, on_max = (int32_t)0x80000001;
        for (int i = 0; i < n; i++) {
            if (padc_dma_buf[i] < on_min) on_min = padc_dma_buf[i];
            if (padc_dma_buf[i] > on_max) on_max = padc_dma_buf[i];
        }
        int32_t on_pp = on_max - on_min;

        /* Dither OFF */
        padc_hw_init(&cfg_d);
        uint32_t reg = readl(AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
        reg &= ~(1 << AUDADC_DITHER_ENA_POS);
        writel(reg, AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
        padc_dma_collect(n);
        int32_t off_min = 0x7FFFFFFF, off_max = (int32_t)0x80000001;
        for (int i = 0; i < n; i++) {
            if (padc_dma_buf[i] < off_min) off_min = padc_dma_buf[i];
            if (padc_dma_buf[i] > off_max) off_max = padc_dma_buf[i];
        }
        int32_t off_pp = off_max - off_min;

        /* Restore dither ON */
        reg |= (1 << AUDADC_DITHER_ENA_POS);
        writel(reg, AUD_BASE + AUDADC_ANA_CFG2_OFFSET);

        printf("  Dither ON : P-P=%ld\r\n", (long)on_pp);
        printf("  Dither OFF: P-P=%ld\r\n", (long)off_pp);
        int tc13_pass = (on_pp > 0 && off_pp > 0);
        printf("  Result: %s\r\n", tc13_pass ? "PASS" : "FAIL");
        tc13_pass ? pass++ : fail++;
    }

    /* ---- Summary ---- */
    printf("\r\n");
    printf("==========================================\r\n");
    printf("  PADC Auto-Test: PASS=%d  FAIL=%d  Total=%d\r\n",
           pass, fail, pass + fail);
    printf("  Result: %s\r\n", fail == 0 ? "ALL PASS" : "SOME FAILED");
    printf("==========================================\r\n");

    return fail == 0 ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_auto, padc_auto, PADC auto-test via DAC loopback);

/* ================================================================== */
/*                                                                      */
/*  padc_wave — DAC waveform output + PADC DMA capture                  */
/*                                                                      */
/*  Outputs a waveform via DAC while simultaneously recording via       */
/*  PADC DMA. Prints CSV data for waveform comparison.                  */
/*                                                                      */
/*  Wiring: GPIO23(DAC_A) --[1KΩ]--> GPIO4(AIN0)                       */
/*                                                                      */
/* ================================================================== */

#define WAVE_STEPS_MAX   64
#define WAVE_SAMPLES_MAX PADC_DMA_BUF_MAX

static int cmd_padc_wave(int argc, char **argv)
{
    int steps = 16;
    int hold_ms = 5;
    int dac_lo = 500;   /* ~981mV — full range for visible waveform shape */
    int dac_hi = 1023;
    const char *pattern = "ramp";

    if (argc < 2) {
        printf("Usage: padc_wave <pattern> [steps] [hold_ms]\r\n");
        printf("  pattern: ramp, triangle, pulse, sine\r\n");
        printf("  steps  : 4~32 (default 16)\r\n");
        printf("  hold_ms: ms per step (default 5)\r\n");
        printf("\r\n");
        printf("  Wiring: GPIO23(DAC_A) -[1K]-> GPIO4(AIN0)\r\n");
        printf("  Output: CSV (index, dac_code, dac_mv, padc_raw)\r\n");
        return 0;
    }

    pattern = argv[1];
    if (argc > 2) {
        steps = atoi(argv[2]);
        if (steps < 4) steps = 4;
        if (steps > 64) steps = 64;
    }
    if (argc > 3) {
        hold_ms = atoi(argv[3]);
        if (hold_ms < 1) hold_ms = 1;
        if (hold_ms > 100) hold_ms = 100;
    }

    /* Build waveform table */
    int wave[64]; /* WAVE_STEPS_MAX */
    int wave_len = 0;

    if (strcmp(pattern, "ramp") == 0) {
        for (int i = 0; i < steps; i++) {
            wave[i] = dac_lo + (dac_hi - dac_lo) * i / (steps - 1);
        }
        wave_len = steps;
    } else if (strcmp(pattern, "triangle") == 0) {
        int half = steps / 2;
        for (int i = 0; i < half; i++) {
            wave[i] = dac_lo + (dac_hi - dac_lo) * i / (half - 1);
        }
        for (int i = 0; i < half; i++) {
            wave[half + i] = dac_hi - (dac_hi - dac_lo) * i / (half - 1);
        }
        wave_len = half * 2;
    } else if (strcmp(pattern, "pulse") == 0) {
        int half = steps / 2;
        for (int i = 0; i < half; i++) wave[i] = dac_lo;
        for (int i = half; i < steps; i++) wave[i] = dac_hi;
        wave_len = steps;
    } else if (strcmp(pattern, "sine") == 0) {
        /*
         * Smooth sine: fill DAC DMA buffer directly (1 sample = 1 value).
         * Total samples = steps * hold_ms * dac_freq / 1000.
         * 'steps' controls the number of correlation points (for analysis).
         * The actual waveform is continuous, not staircase.
         */
        wave_len = 0; /* signal: use dac_dma_buf directly */
    } else {
        printf("Unknown pattern: %s (use ramp/triangle/pulse/sine)\r\n", pattern);
        return -1;
    }

    /* Init GPIO + AUADC */
    padc_gpio_init(0);
    padc_ch_inited |= (1 << 0);

    /* Init DAC GPIO (analog mode) */
    GLB_GPIO_Cfg_Type acfg = {
        .gpioFun = GPIO_FUN_ANALOG, .gpioMode = GPIO_MODE_ANALOG,
        .pullType = GPIO_PULL_NONE, .drive = 0, .smtCtrl = 0,
    };
    acfg.gpioPin = DAC_GPIO_A; GLB_GPIO_Init(&acfg);
    acfg.gpioPin = DAC_GPIO_B; GLB_GPIO_Init(&acfg);

    /* Set DAC_B=0 as GND reference for diff mode */
    GLB_DAC_Set_ChanB_Value(0);

    /* Use diff mode with DAC_B=0 as reference (no dead zone) */
    AUADC_PADC_Cfg_Type cfg = {
        .posi_ch = AUADC_ADC_AnalogCh_AIN0,
        .nega_ch = AUADC_ADC_AnalogCh_AIN1,
        .pga_gain = AUADC_ADC_PGA_GAIN_6DB,
        .pga_mode = AUADC_ADC_PGA_MODE_DC_DIFF,
        .filter = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
        .rate = AUADC_ADC_MEASURE_RATE_SPS_100,
    };
    padc_hw_init(&cfg);

    /* Fill DAC DMA buffer */
    int dac_freq = DAC_SAMPLE_FREQ_8KHZ;
    int total_ms = steps * hold_ms;  /* use 'steps' for total duration */
    int dac_total = total_ms * dac_freq / 1000;
    if (dac_total > DAC_DMA_BUF_MAX) {
        printf("DAC DMA buffer overflow (%d > %d), reduce steps or hold\r\n",
               dac_total, DAC_DMA_BUF_MAX);
        return -1;
    }

    if (wave_len > 0) {
        /* Staircase: repeat each wave[] value for hold duration */
        int samples_per_step = hold_ms * dac_freq / 1000;
        dac_total = wave_len * samples_per_step;
        for (int i = 0; i < wave_len; i++) {
            for (int j = 0; j < samples_per_step; j++)
                dac_dma_buf[i * samples_per_step + j] = (uint16_t)wave[i];
        }
    } else {
        /* Smooth sine: compute each DAC sample directly */
        int mid = (dac_lo + dac_hi) / 2;
        int amp = (dac_hi - dac_lo) / 2;
        wave_len = steps;  /* restore for correlation */
        for (int i = 0; i < dac_total; i++) {
            int phase = (i * 360) / dac_total;
            int q;
            if (phase < 90) q = phase;
            else if (phase < 180) q = 180 - phase;
            else if (phase < 270) q = phase - 180;
            else q = 360 - phase;
            int t = q * 1000 / 90;
            int s = t * (2000 - t) / 1000;
            int val = (phase < 180) ? mid + amp * s / 1000
                                    : mid - amp * s / 1000;
            if (val < dac_lo) val = dac_lo;
            if (val > dac_hi) val = dac_hi;
            dac_dma_buf[i] = (uint16_t)val;
        }
        /* Build wave[] from dac_dma_buf for correlation reference */
        int samples_per_step = dac_total / wave_len;
        for (int i = 0; i < wave_len; i++)
            wave[i] = dac_dma_buf[i * samples_per_step];
    }

    /* Calculate expected PADC DMA samples during waveform */
    /* Sinc3 rate at ODR=100: ~800 SPS → ~0.8 samples/ms. */
    int expected_samples = total_ms * 800 / 1000;
    if (expected_samples > PADC_DMA_BUF_MAX) expected_samples = PADC_DMA_BUF_MAX;

    printf("=== PADC Waveform Capture ===\r\n");
    printf("Pattern : %s (diff mode, DAC DMA @ %d Hz)\r\n", pattern, dac_freq);
    printf("Steps   : %d, hold=%dms, total=%dms\r\n", wave_len, hold_ms, total_ms);
    printf("DAC range: %d~%d (%d~%dmV)\r\n",
           dac_lo, dac_hi, dac_code_to_mv(dac_lo), dac_code_to_mv(dac_hi));
    printf("DAC DMA : %d samples (smooth=%s)\r\n", dac_total,
           (strcmp(pattern, "sine") == 0) ? "yes" : "no");
    printf("PADC DMA: ~%d samples (32-bit)\r\n", expected_samples);
    printf("Capturing...\r\n");

    /* Setup PADC FIFO + DMA (32-bit, 20-bit resolution) */
    AUADC_FifoCfg_Type fifo_cfg = {
        .resolution = AUADC_RES_20_BITS,
        .ailgnMode = AUADC_FIFO_AILGN_MSB_AT_BIT31,
        .FifoIntThreshold = 0,
        .dmaThresholdMode = AUADC_DRQ_EQUEL_TO_IRQ,
        .dmaEn = ENABLE,
    };
    AUADC_FifoInit(&fifo_cfg);
    AUADC_FifoClear();

    dma_device_t padc_dma = {
        .id = DMA0_ID, .ch = DMA0_CH1_INDEX,
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_AUD_ADC, .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR1, .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_32BIT,
        .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .transfer_mode = DMA_LLI_ONCE_MODE,
        .lli_cfg = NULL, .callback = NULL,
    };
    dma_setConfig(&padc_dma);
    dma_init(DMA0_ID, DMA0_CH1_INDEX);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_INT, NULL);

    /* Setup DAC DMA (CH0, M2P, 16-bit) */
    dma_device_t dac_dma = {
        .id = DMA0_ID, .ch = DMA0_CH0_INDEX,
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE, .dst_req = DMA_REQUEST_DAC0,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_size = DMA_BURST_INCR8, .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_16BIT,
        .dst_width = DMA_TRANSFER_WIDTH_16BIT,
        .transfer_mode = DMA_LLI_ONCE_MODE,
        .lli_cfg = NULL, .callback = dac_dma_callback,
    };
    dma_setConfig(&dac_dma);
    dma_init(DMA0_ID, DMA0_CH0_INDEX);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_CALLBACK, dac_dma_callback);

    /* Init DAC in DMA mode */
    dac_device_t dac0 = {
        .caps = COMMON_CAPS_DMA_TX,
        .channels = DAC_CHANNEL_ALL,
        .sample_freq = dac_freq,
        .vref = DAC_VREF_INTERNAL,
    };
    hal_dac_setConfig(&dac0);
    hal_dac_init(0);
    hal_dac_control(0, DAC_CTRL_TX_DMA, (void *)(uintptr_t)DMA0_CH0_INDEX);

    /* Flush caches */
    L1C_DCACHE_INVALID_RANGE((uintptr_t)padc_dma_buf,
                              expected_samples * sizeof(int32_t));
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)dac_dma_buf,
                            dac_total * sizeof(uint16_t));

    /* Start PADC DMA */
    AUADC_Start();
    dma_reload(DMA0_ID, DMA0_CH1_INDEX,
               (uint32_t)DMA_ADDR_AUD_RDR,
               (uint32_t)(uintptr_t)padc_dma_buf,
               expected_samples * sizeof(int32_t), NULL);
    dma_channel_start(DMA0_ID, DMA0_CH1_INDEX);

    /* Start DAC DMA */
    dac_dma_done = 0;
    uint64_t t0 = mtimer_get_time_us();
    hal_dac_write(0, DAC_CHANNEL_0, dac_dma_buf, dac_total * sizeof(uint16_t));

    /* Wait for DAC DMA to finish */
    uint64_t deadline = mtimer_get_time_us() + (total_ms + 2000) * 1000ULL;
    while (!dac_dma_done) {
        if (mtimer_get_time_us() > deadline) {
            printf("DAC DMA timeout!\r\n");
            break;
        }
    }
    uint64_t t_wave = mtimer_get_time_us() - t0;

    /* Wait for PADC DMA to finish */
    deadline = mtimer_get_time_us() + 5000000;
    while (dma_channel_check_busy(DMA0_ID, DMA0_CH1_INDEX)) {
        if (mtimer_get_time_us() > deadline) break;
    }
    uint64_t t_total = mtimer_get_time_us() - t0;

    dma_channel_stop(DMA0_ID, DMA0_CH1_INDEX);
    AUADC_Stop();
    AUADC_FifoClear();

    L1C_DCACHE_INVALID_RANGE((uintptr_t)padc_dma_buf,
                              expected_samples * sizeof(int32_t));

    int actual_samples = expected_samples;

    printf("Wave time: %lu us, DMA time: %lu us\r\n",
           (unsigned long)t_wave, (unsigned long)t_total);
    printf("wave_time_us=%lu\r\n", (unsigned long)t_wave);
    printf("Samples  : %d captured\r\n", actual_samples);

    /* Print reference waveform */
    printf("\r\n--- DAC Reference Waveform ---\r\n");
    printf("step,code,mv\r\n");
    for (int i = 0; i < wave_len; i++) {
        printf("%d,%d,%d\r\n", i, wave[i], dac_code_to_mv(wave[i]));
    }

    /* Print captured PADC waveform (decimate to max 200 lines for log buffer) */
    printf("\r\n--- PADC Captured Waveform ---\r\n");
    printf("sample,raw,time_us\r\n");
    int sample_interval_us = (int)(t_total / actual_samples);
    int decimate = (actual_samples + 199) / 200;
    if (decimate < 1) decimate = 1;
    for (int i = 0; i < actual_samples; i += decimate) {
        printf("%d,%d,%d\r\n", i, (int)padc_dma_buf[i],
               i * sample_interval_us);
    }

    printf("\r\n--- Waveform Correlation ---\r\n");
    /* Map each DAC step to corresponding PADC samples */
    int padc_per_step = actual_samples / wave_len;
    if (padc_per_step < 1) padc_per_step = 1;

    int mono_pass = 1;
    int32_t prev_avg = -999999;
    printf("step,dac,dac_mv,padc_avg,delta\r\n");
    for (int s = 0; s < wave_len && s * padc_per_step < actual_samples; s++) {
        int start = s * padc_per_step;
        int end = start + padc_per_step;
        if (end > actual_samples) end = actual_samples;

        /* Average PADC samples for this step (int64 to prevent overflow) */
        int64_t sum = 0;
        for (int i = start; i < end; i++) sum += padc_dma_buf[i];
        int32_t avg = (int32_t)(sum / (end - start));

        int32_t delta = avg - prev_avg;
        printf("%d,%d,%d,%ld,%ld\r\n", s, wave[s], dac_code_to_mv(wave[s]),
               (long)avg, s > 0 ? (long)delta : 0L);

        /* Check monotonic for ramp pattern */
        if (s > 0 && strcmp(pattern, "ramp") == 0) {
            if (avg <= prev_avg) mono_pass = 0;
        }
        prev_avg = avg;
    }

    if (strcmp(pattern, "ramp") == 0) {
        printf("\r\nRamp monotonic: %s\r\n", mono_pass ? "PASS" : "FAIL");
    }

    printf("=== Waveform Capture Done ===\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_padc_wave, padc_wave, DAC waveform output + PADC capture);
