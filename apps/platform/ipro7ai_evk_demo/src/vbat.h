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
 * @file vbat.h
 * @brief Battery-rail voltage via the SoC's internal VBAT/2 ADC channel.
 *
 * >>> READ THIS BEFORE TRUSTING THE NUMBER. <<<
 *
 * GPADC channel 18 is an INTERNAL node. ADC_Vbat_Enable() sets
 * GLB_GPADC_VBAT_EN (GLB_GPADC_REG_CONFIG2 bit 4), which switches the SoC's
 * own VBAT supply into the converter through an on-die 2:1 divider. No GPIO
 * is involved; the externally measurable channels are 0-11.
 *
 * So this reads *the voltage the chip itself is running on*. On this 49-pin
 * package the only supply pins brought out are VDDIO1 (36), 3V3_SOC (37) and
 * VBUS (35), so in practice this is almost certainly 3V3_SOC.
 *
 * It is NOT the board's `VBAT` net - the one that reaches PA_VDD through R32
 * and powers the class-D amplifier. Those two share a name in the schematic
 * and nothing else. Measuring that rail needs an external divider into one of
 * GPADC channels 0-11; see `adc` in vbat.c.
 *
 * The divider is gated: ADC_CTRL_VBAT_ON must be issued before the channel
 * reads anything meaningful, and turned back off afterwards so it stops
 * loading the rail.
 */
#ifndef VBAT_H
#define VBAT_H

#include <stdint.h>

#define VBAT_OK          0
#define VBAT_ERR_READ   -1

/**
 * @brief One trimmed-mean reading of the SoC's own supply rail.
 *
 * Takes several samples, discards the highest and lowest, and averages the
 * rest - a switching amplifier on the same rail makes single shots jumpy.
 *
 * @param out_mv  Supply voltage in millivolts (already doubled back up from
 *                the VBAT/2 tap).
 * @return VBAT_OK, or VBAT_ERR_READ if no sample came back.
 */
int vbat_read_mv(uint32_t *out_mv);

/**
 * @brief Same reading, with the raw per-sample spread for diagnostics.
 * @param out_mv   Trimmed mean, millivolts.
 * @param out_min  Lowest sample seen, millivolts.
 * @param out_max  Highest sample seen, millivolts.
 * @param out_n    Samples that read back successfully.
 */
int vbat_read_detail(uint32_t *out_mv, uint32_t *out_min,
                     uint32_t *out_max, uint32_t *out_n);

#endif /* VBAT_H */
