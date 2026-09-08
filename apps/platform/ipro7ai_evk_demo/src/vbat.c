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
 * @file vbat.c
 * @brief Battery-rail voltage via the internal VBAT/2 ADC channel. See vbat.h.
 */

#include "vbat.h"

#include <string.h>
#include <stdio.h>

#include "hal_adc.h"
#include "ipro_log.h"

#define LOG_TAG "VBAT"

/* Samples per reading. Eight is enough for a stable trimmed mean without
 * making the shell command feel slow. */
#define VBAT_SAMPLES  8

int vbat_read_detail(uint32_t *out_mv, uint32_t *out_min,
                     uint32_t *out_max, uint32_t *out_n)
{
    adc_device_t adc0;
    uint8_t      pos_list[1] = { ADC_CHANNEL_VABT_HALF };
    uint8_t      neg_list[1] = { ADC_CHANNEL_GND };
    adc_channel_cfg_t ch_cfg = { .pos_channel = pos_list,
                                 .neg_channel = neg_list,
                                 .num = 1 };
    uint32_t sum = 0, n = 0, mn = 0xFFFFFFFFu, mx = 0;

    memset(&adc0, 0, sizeof(adc0));
    adc0.id                   = 0;
    adc0.caps                 = COMMON_CAPS_INT_RX;
    adc0.continuous_conv_mode = 1;
    adc0.callback             = NULL;
    adc0.clk_div              = ADC_CLOCK_DIV_32;
    adc0.vref                 = ADC_VREF_3V2;
    adc0.differential_mode    = false;
    adc0.data_width           = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE;
    adc0.fifo_threshold       = ADC_FIFO_THRESHOLD_1BYTE;
    adc0.gain                 = ADC_GAIN_1;
    adc0.rx_dma               = -1;

    hal_adc_setConfig(&adc0);
    hal_adc_init(0);

    /* The on-die 2:1 divider is gated; without this the channel reads noise. */
    hal_adc_ioctl(0, ADC_CTRL_VBAT_ON, NULL);

    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_CONFIG, &ch_cfg);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_START, NULL);

    for (int i = 0; i < VBAT_SAMPLES; i++) {
        adc_channel_val_t v[1];
        if (hal_adc_read(0, v, 1) > 0) {
            /* volt is in volts at the tap; x1000 for mV, x2 to undo the
             * on-die divider. */
            uint32_t mv = (uint32_t)(v[0].volt * 2000.0f);
            sum += mv;
            n++;
            if (mv < mn) { mn = mv; }
            if (mv > mx) { mx = mv; }
        }
    }

    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_STOP, NULL);
    /* Turn the divider back off so it stops loading the rail. */
    hal_adc_ioctl(0, ADC_CTRL_VBAT_OFF, NULL);

    if (n == 0u) {
        return VBAT_ERR_READ;
    }

    if (out_min) { *out_min = mn; }
    if (out_max) { *out_max = mx; }
    if (out_n)   { *out_n = n; }

    /* Trimmed mean: a class-D amplifier on the same rail makes single shots
     * jump around, and one outlier in eight moves a plain average noticeably. */
    if (n >= 4u) {
        sum -= mn;
        sum -= mx;
        n -= 2u;
    }
    if (out_mv) { *out_mv = sum / n; }
    return VBAT_OK;
}

int vbat_read_mv(uint32_t *out_mv)
{
    return vbat_read_detail(out_mv, NULL, NULL, NULL);
}

#ifdef CONFIG_SHELL

#include "shell.h"
#include <stdlib.h>

int cmd_vbat(int argc, char **argv)
{
    uint32_t mv = 0, mn = 0, mx = 0, n = 0;

    (void)argc;
    (void)argv;

    if (vbat_read_detail(&mv, &mn, &mx, &n) != VBAT_OK) {
        printf("VBAT read failed - no samples came back from the ADC\n");
        return 0;
    }

    printf("SoC VBAT: %u.%03u V  (%u mV)\n",
           (unsigned)(mv / 1000u), (unsigned)(mv % 1000u), (unsigned)mv);
    printf("samples : %u taken, spread %u..%u mV (highest and lowest dropped)\n",
           (unsigned)n, (unsigned)mn, (unsigned)mx);
    printf("source  : INTERNAL VBAT/2 tap, GPADC ch18. This is the rail the\n"
           "          SoC itself runs on - almost certainly 3V3_SOC on this\n"
           "          package. It is NOT the board's VBAT net that feeds\n"
           "          PA_VDD through R32.\n");

    if (mv > 3150u && mv < 3450u && (mx - mn) < 60u) {
        printf("reading : steady and near 3.3 V - consistent with a regulated\n"
               "          supply, i.e. 3V3_SOC rather than a cell.\n");
    } else if (mv >= 3450u && mv <= 4300u) {
        printf("reading : in Li-ion range and above a 3.3 V regulator, so this\n"
               "          package may bring VBAT out separately after all.\n"
               "          Confirm against the board before relying on it.\n");
    }

    if (mx - mn > 100u) {
        printf("note    : %u mV of spread is a lot for a supply rail. If the\n"
               "          amplifier is on, its switching current modulates\n"
               "          what you are measuring - retry with `spk off`.\n",
               (unsigned)(mx - mn));
    }
    printf("hint    : to measure the real battery/PA_VDD rail, fit an external\n"
           "          divider into a GPADC pin and use `adc <ch> [ratio_x100]`.\n");
    return 0;
}

/*
 * Generic GPADC read, for any rail brought in on a pin through a divider.
 * Channels 0-11 are the externally routed ones; ratio_x100 is the divider's
 * inverse scaled by 100, so a 1:2 divider (two equal resistors) is 200.
 */
int cmd_adc(int argc, char **argv)
{
    adc_device_t adc0;
    uint8_t      pos_list[1];
    uint8_t      neg_list[1] = { ADC_CHANNEL_GND };
    adc_channel_cfg_t ch_cfg = { .pos_channel = pos_list,
                                 .neg_channel = neg_list,
                                 .num = 1 };
    int      ch;
    uint32_t ratio = 100u;
    uint32_t sum = 0, n = 0;

    if (argc < 2) {
        printf("usage: adc <channel 0..11> [ratio_x100]\n"
               "  ratio_x100 scales the reading back up through an external\n"
               "  divider: two equal resistors (1:2) is 200. Default 100.\n"
               "  GPADC channels 0-11 are the pin-routed ones; channel 18 is\n"
               "  the internal SoC supply and has its own command, `vbat`.\n");
        return 0;
    }
    ch = atoi(argv[1]);
    if (ch < 0 || ch > 11) {
        printf("channel must be 0..11 (those are the externally routed ones)\n");
        return 0;
    }
    if (argc > 2) {
        ratio = (uint32_t)atoi(argv[2]);
        if (ratio == 0u) {
            ratio = 100u;
        }
    }
    pos_list[0] = (uint8_t)ch;

    memset(&adc0, 0, sizeof(adc0));
    adc0.id                   = 0;
    adc0.caps                 = COMMON_CAPS_INT_RX;
    adc0.continuous_conv_mode = 1;
    adc0.clk_div              = ADC_CLOCK_DIV_32;
    adc0.vref                 = ADC_VREF_3V2;
    adc0.differential_mode    = false;
    adc0.data_width           = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE;
    adc0.fifo_threshold       = ADC_FIFO_THRESHOLD_1BYTE;
    adc0.gain                 = ADC_GAIN_1;
    adc0.rx_dma               = -1;

    hal_adc_setConfig(&adc0);
    hal_adc_init(0);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_CONFIG, &ch_cfg);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_START, NULL);

    for (int i = 0; i < VBAT_SAMPLES; i++) {
        adc_channel_val_t v[1];
        if (hal_adc_read(0, v, 1) > 0) {
            sum += (uint32_t)(v[0].volt * 1000.0f);
            n++;
        }
    }
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_STOP, NULL);

    if (n == 0u) {
        printf("ADC ch%d: no samples\n", ch);
        return 0;
    }
    {
        uint32_t pin_mv = sum / n;
        uint32_t rail   = pin_mv * ratio / 100u;
        printf("ADC ch%d : %u mV at the pin", ch, (unsigned)pin_mv);
        if (ratio != 100u) {
            printf(", %u.%03u V at the rail (ratio %u.%02ux)",
                   (unsigned)(rail / 1000u), (unsigned)(rail % 1000u),
                   (unsigned)(ratio / 100u), (unsigned)(ratio % 100u));
        }
        printf("\n");
    }
    printf("note    : vref is 3.2 V, so keep the divided voltage under that.\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_vbat, vbat, SoC supply rail voltage);
SHELL_CMD_EXPORT_ALIAS(cmd_adc, adc, Read a GPADC channel);

#endif /* CONFIG_SHELL */
