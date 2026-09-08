/**
 * @file tsen_demo.c
 * @brief On-chip temperature sensor (GPADC TSEN_P) demo.
 *
 * Shell command:
 *   tsen                               N-sample average read, default N=8
 *   tsen <n>                           override sample count (1..32)
 *   tsen log [duration_s] [period_ms]  periodic log, default 60 s / 1000 ms
 *
 * Reads the on-chip T-sensor via the ADC TSEN channel. The eFuse trim
 * offset is loaded once; if unprogrammed (offset==0) we fall back to
 * an empirical default (see dvt_auto.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"
#include "hal_adc.h"
#include "drv_glb.h"
#include "drv_adc.h"

#define TSEN_SAMPLES_DEFAULT    8
#define TSEN_SAMPLES_MAX        32

#define TSEN_LOG_DURATION_S     60
#define TSEN_LOG_PERIOD_MS      1000
#define TSEN_LOG_PERIOD_MIN_MS  50

/*
 * Empirical default offset for dies with unprogrammed eFuse TSEN trim.
 * Source: apps/test/dvt/dvt_auto.c — raw_diff ~= 1504 at ~25 degC,
 * offset = raw_diff - temp * 7.753 ~= 1310.
 */
#define TSEN_DEFAULT_OFFSET     1310

/*
 * TSEN_Get_Temp() internally does its own ADC_Start() twice
 * (TSVBE_LOW then TSVBE_HIGH). Must NOT use continuous mode
 * or CHANNEL_START — they fill the FIFO with stale data
 * before the TSVBE switch takes effect. See dvt_auto.c.
 */
static void tsen_adc_open(void)
{
    adc_device_t adc0 = {
        .id                   = 0,
        .caps                 = COMMON_CAPS_INT_RX,
        .continuous_conv_mode = 0,
        .callback             = NULL,
        .clk_div              = ADC_CLOCK_DIV_32,
        .vref                 = ADC_VREF_3V2,
        .differential_mode    = false,
        .data_width           = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE,
        .fifo_threshold       = ADC_FIFO_THRESHOLD_1BYTE,
        .gain                 = ADC_GAIN_1,
        .rx_dma               = -1,
    };
    uint8_t pos_list[1] = { ADC_CHANNEL_TSEN_P };
    uint8_t neg_list[1] = { ADC_CHANNEL_GND };
    adc_channel_cfg_t ch_cfg = {
        .pos_channel = pos_list,
        .neg_channel = neg_list,
        .num         = 1,
    };

    hal_adc_setConfig(&adc0);
    hal_adc_init(0);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_CONFIG, &ch_cfg);
    hal_adc_ioctl(0, ADC_CTRL_TSEN_ON, NULL);
}

static void tsen_adc_close(void)
{
    hal_adc_ioctl(0, ADC_CTRL_TSEN_OFF, NULL);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_STOP, NULL);
}

static uint16_t tsen_load_offset(int *calibrated)
{
    uint16_t offset = 0;
    hal_adc_trim_tsen(&offset);
    *calibrated = (offset != 0);
    if (!*calibrated)
        offset = TSEN_DEFAULT_OFFSET;
    return offset;
}

/* Split a float temp into sign-aware integer and 1-decimal fraction. */
static void tsen_split(float t, int *i_part, int *f_part)
{
    int ip = (int)t;
    int fp = (int)((t - (float)ip) * 10.0f);
    if (fp < 0) fp = -fp;
    *i_part = ip;
    *f_part = fp;
}

/* ------------------------------------------------------------------ */
/* Raw one-phase read: set TSVBE, kick ADC, read one sample.          */
/* mode_cont=0: single-shot (ADC_Start stops after one sample)         */
/* mode_cont=1: continuous (ADC is already running; discard `flush`    */
/*              in-flight samples after TSVBE switch before sampling). */
/* ------------------------------------------------------------------ */
static uint32_t tsen_phase_read(int tsvbe_high, int mode_cont, int flush)
{
    ADC_FIFO_Clear();

    if (tsvbe_high)
        ADC_SET_TSVBE_HIGH();
    else
        ADC_SET_TSVBE_LOW();

    if (mode_cont) {
        /* In continuous mode CONV_START stays 1. Samples already in
         * the analog pipeline may carry the *old* TSVBE bias. Discard
         * `flush` of them, then purge FIFO once more to guarantee
         * the next FIFO word was sampled *after* the TSVBE switch. */
        for (int i = 0; i < flush; i++) {
            while (ADC_Get_FIFO_Count() == 0)
                ;
            (void)ADC_Read_FIFO();
        }
        ADC_FIFO_Clear();
    } else {
        /* Single-shot: explicitly re-trigger. */
        ADC_Start();
    }

    while (ADC_Get_FIFO_Count() == 0)
        ;
    uint32_t regVal = ADC_Read_FIFO();

    ADC_Result_Type result;
    ADC_Parse_Result(&regVal, 1, &result);
    return result.value;
}

/* ------------------------------------------------------------------ */
/* `tsen raw`      — single-shot, print v0/v1/diff/temp (debug)       */
/* `tsen cont <f>` — continuous mode with `f` sample discard per phase*/
/* ------------------------------------------------------------------ */
static int tsen_run_probe(int mode_cont, int flush, int count)
{
    if (count < 1) count = 1;
    if (count > TSEN_SAMPLES_MAX) count = TSEN_SAMPLES_MAX;

    /*
     * Mode-specific ADC bring-up.
     *  - single-shot: continuous_conv_mode=0, no CHANNEL_START
     *  - continuous : continuous_conv_mode=1, CHANNEL_START ONCE
     *    (and we must start AFTER TSEN_ON so the analog front-end is
     *    already biased for TSEN when conversions begin)
     */
    adc_device_t adc0 = {
        .id                   = 0,
        .caps                 = COMMON_CAPS_INT_RX,
        .continuous_conv_mode = mode_cont,
        .callback             = NULL,
        .clk_div              = ADC_CLOCK_DIV_32,
        .vref                 = ADC_VREF_3V2,
        .differential_mode    = false,
        .data_width           = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE,
        .fifo_threshold       = ADC_FIFO_THRESHOLD_1BYTE,
        .gain                 = ADC_GAIN_1,
        .rx_dma               = -1,
    };
    uint8_t pos_list[1] = { ADC_CHANNEL_TSEN_P };
    uint8_t neg_list[1] = { ADC_CHANNEL_GND };
    adc_channel_cfg_t ch_cfg = {
        .pos_channel = pos_list,
        .neg_channel = neg_list,
        .num         = 1,
    };

    hal_adc_setConfig(&adc0);
    hal_adc_init(0);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_CONFIG, &ch_cfg);
    hal_adc_ioctl(0, ADC_CTRL_TSEN_ON, NULL);

    if (mode_cont) {
        /* One-time kick; subsequent TSVBE switches do NOT touch CONV_START. */
        hal_adc_ioctl(0, ADC_CTRL_CHANNEL_START, NULL);
    }

    int calibrated;
    uint16_t offset = tsen_load_offset(&calibrated);

    printf("=== TSEN %s%s ===\r\n",
           mode_cont ? "cont" : "raw",
           mode_cont ? "" : " (single-shot)");
    printf("offset : %u (%s)\r\n",
           (unsigned)offset, calibrated ? "eFuse" : "default");
    if (mode_cont)
        printf("flush  : %d samples per phase\r\n", flush);
    printf("samples: %d\r\n", count);
    printf("  idx      v0      v1    diff    temp\r\n");

    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        uint32_t v0 = tsen_phase_read(/*tsvbe_high=*/0, mode_cont, flush);
        uint32_t v1 = tsen_phase_read(/*tsvbe_high=*/1, mode_cont, flush);

        int32_t diff = (v0 >= v1) ? (int32_t)(v0 - v1) : -(int32_t)(v1 - v0);
        uint32_t abs_diff = (diff < 0) ? (uint32_t)(-diff) : (uint32_t)diff;
        float t = ((float)abs_diff - (float)offset) / 7.753f;
        sum += t;

        int ti, tf;
        tsen_split(t, &ti, &tf);
        printf("  %3d  %6lu  %6lu  %+6ld   %d.%01d\r\n",
               i, (unsigned long)v0, (unsigned long)v1,
               (long)diff, ti, tf);
    }

    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_STOP, NULL);
    hal_adc_ioctl(0, ADC_CTRL_TSEN_OFF, NULL);

    float avg = sum / (float)count;
    int ai, af;
    tsen_split(avg, &ai, &af);
    printf("avg temp: %d.%01d degC\r\n", ai, af);
    printf("============\r\n");
    return 0;
}

/* ------------------------------------------------------------------ */
/* `tsen`          — single-shot with N-sample averaging              */
/* `tsen <n>`      — override N                                       */
/* ------------------------------------------------------------------ */
static int tsen_run_oneshot(int count)
{
    if (count < 1) count = 1;
    if (count > TSEN_SAMPLES_MAX) count = TSEN_SAMPLES_MAX;

    tsen_adc_open();

    int calibrated;
    uint16_t offset = tsen_load_offset(&calibrated);

    float sum = 0.0f;
    float tmin = 0.0f, tmax = 0.0f;
    for (int i = 0; i < count; i++) {
        float t = hal_adc_get_tsen(offset);
        if (i == 0) { tmin = t; tmax = t; }
        if (t < tmin) tmin = t;
        if (t > tmax) tmax = t;
        sum += t;
    }
    float avg = sum / (float)count;

    tsen_adc_close();

    int ai, af, mi, mf, xi, xf;
    tsen_split(avg,  &ai, &af);
    tsen_split(tmin, &mi, &mf);
    tsen_split(tmax, &xi, &xf);

    printf("=== TSEN ===\r\n");
    printf("offset : %u (%s)\r\n",
           (unsigned)offset, calibrated ? "eFuse" : "default");
    printf("samples: %d\r\n", count);
    printf("avg    : %d.%01d degC\r\n", ai, af);
    printf("min    : %d.%01d degC\r\n", mi, mf);
    printf("max    : %d.%01d degC\r\n", xi, xf);
    printf("============\r\n");
    return 0;
}

/* ------------------------------------------------------------------ */
/* `tsen log [duration_s] [period_ms]` — periodic logging             */
/* Blocks the shell for the full duration; one line per sample.       */
/* ------------------------------------------------------------------ */
static int tsen_run_log(int duration_s, int period_ms)
{
    if (duration_s < 1) duration_s = 1;
    if (period_ms < TSEN_LOG_PERIOD_MIN_MS) period_ms = TSEN_LOG_PERIOD_MIN_MS;

    int iters = (duration_s * 1000) / period_ms;
    if (iters < 1) iters = 1;

    tsen_adc_open();

    int calibrated;
    uint16_t offset = tsen_load_offset(&calibrated);

    printf("=== TSEN log ===\r\n");
    printf("offset  : %u (%s)\r\n",
           (unsigned)offset, calibrated ? "eFuse" : "default");
    printf("duration: %d s\r\n", duration_s);
    printf("period  : %d ms\r\n", period_ms);
    printf("samples : %d\r\n", iters);
    printf("  idx   t_ms    temp\r\n");

    float sum = 0.0f;
    float tmin = 0.0f, tmax = 0.0f;
    TickType_t t0 = xTaskGetTickCount();
    TickType_t next = t0;
    const TickType_t period_ticks = pdMS_TO_TICKS(period_ms);

    for (int i = 0; i < iters; i++) {
        float t = hal_adc_get_tsen(offset);
        if (i == 0) { tmin = t; tmax = t; }
        if (t < tmin) tmin = t;
        if (t > tmax) tmax = t;
        sum += t;

        TickType_t now = xTaskGetTickCount();
        uint32_t elapsed_ms = (uint32_t)((now - t0) * portTICK_PERIOD_MS);

        int ti, tf;
        tsen_split(t, &ti, &tf);
        printf("  %3d  %6lu   %d.%01d degC\r\n",
               i, (unsigned long)elapsed_ms, ti, tf);

        next += period_ticks;
        TickType_t cur = xTaskGetTickCount();
        if ((int32_t)(next - cur) > 0)
            vTaskDelay(next - cur);
    }

    tsen_adc_close();

    float avg = sum / (float)iters;
    int ai, af, mi, mf, xi, xf;
    tsen_split(avg,  &ai, &af);
    tsen_split(tmin, &mi, &mf);
    tsen_split(tmax, &xi, &xf);

    printf("---\r\n");
    printf("avg    : %d.%01d degC\r\n", ai, af);
    printf("min    : %d.%01d degC\r\n", mi, mf);
    printf("max    : %d.%01d degC\r\n", xi, xf);
    printf("================\r\n");
    return 0;
}

static void tsen_usage(void)
{
    printf("usage:\r\n");
    printf("  tsen                              one-shot, 8-sample avg\r\n");
    printf("  tsen <n>                          one-shot, n-sample avg (1..%d)\r\n",
           TSEN_SAMPLES_MAX);
    printf("  tsen log [duration_s] [period_ms] periodic log (default %d/%d)\r\n",
           TSEN_LOG_DURATION_S, TSEN_LOG_PERIOD_MS);
    printf("  tsen raw [n]                      debug: print v0/v1/diff (single-shot)\r\n");
    printf("  tsen cont <flush> [n]             debug: continuous mode w/ flush samples\r\n");
}

static int cmd_tsen(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "log") == 0) {
        int dur = TSEN_LOG_DURATION_S;
        int per = TSEN_LOG_PERIOD_MS;
        if (argc >= 3) dur = atoi(argv[2]);
        if (argc >= 4) per = atoi(argv[3]);
        return tsen_run_log(dur, per);
    }

    if (argc >= 2 && strcmp(argv[1], "raw") == 0) {
        int n = (argc >= 3) ? atoi(argv[2]) : 4;
        return tsen_run_probe(/*mode_cont=*/0, /*flush=*/0, n);
    }

    if (argc >= 2 && strcmp(argv[1], "cont") == 0) {
        if (argc < 3) {
            printf("usage: tsen cont <flush> [n]\r\n");
            return -1;
        }
        int flush = atoi(argv[2]);
        int n = (argc >= 4) ? atoi(argv[3]) : 4;
        if (flush < 0) flush = 0;
        return tsen_run_probe(/*mode_cont=*/1, flush, n);
    }

    if (argc >= 2 && (argv[1][0] == '-' || strcmp(argv[1], "help") == 0)) {
        tsen_usage();
        return 0;
    }

    int count = TSEN_SAMPLES_DEFAULT;
    if (argc >= 2)
        count = atoi(argv[1]);
    return tsen_run_oneshot(count);
}
SHELL_CMD_EXPORT_ALIAS(cmd_tsen, tsen, On-chip temperature sensor read);
