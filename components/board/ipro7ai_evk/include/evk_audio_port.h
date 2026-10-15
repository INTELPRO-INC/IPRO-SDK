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
 * @file evk_audio_port.h
 * @brief Speaker (DAC + class-D amp) and PDM microphone wiring on the
 *        IPRO7AI EVK sub-board (schematic IPRO7AI_MODULE_SUB_0812A).
 *
 * SPEAKER
 *   GPIO23  SPK_DAC_OUT -> R27 470R / C20 3.3nF (RC, fc ~102 kHz) -> C19 2.2uF
 *                       -> R26 4.7k -> TPA2037D1 IN+ (single-ended, IN- AC-grounded)
 *   GPIO22  SPK_EN      -> 1k / 10k divider -> TPA2037D1 EN.  ACTIVE HIGH: R25
 *                          pulls EN to ground, the amp is off until GPIO22 is
 *                          driven high. GPIO22 is also DAC channel B, so using
 *                          it as the enable makes the board mono.
 *   PA_VDD <- VBAT (the amplifier runs off the battery rail, not 3V3).
 *
 *   components/audio's DAC backend drives the enable with the polarity the
 *   board header declares (BOARD_SPK_EN_ACTIVE = 1 here); evk_spk passes the
 *   polarity explicitly as well, so it does not depend on which board header
 *   the app was compiled against.
 *
 * PDM MICROPHONE
 *   GPIO18  DMIC_CLK -> IM69D129FV01 CLK       (even pin = PDM_CLK)
 *   GPIO19  DMIC_DAT <- IM69D129FV01 DATA      (odd pin  = PDM_DIN)
 *   SELECT tied to GND -> the mic drives the LEFT slot. VDD = DMIC_1V8.
 *
 *   GPIO18/19 are the two highest AON pads and the 32.768 kHz crystal pins.
 *   Out of reset the always-on block owns them with their input receivers
 *   off (HBN_PAD_CTRL_2 en_aon_ctrl_msb = 0b11, ie_smt_msb = 0), so muxing
 *   them to PDM does nothing until they are handed back - see
 *   evk_mic_claim_pads() in evk_mic.c. Bench-proven on this board.
 *
 * RATES - NEITHER SIDE IS EXACTLY 16 kHz
 *   PDM capture: 2.032 MHz / 128 = 15875 Hz for a nominal 16000.
 *   DAC:         root clock ~31.88 MHz; 32000 nominal is ~31985 Hz real.
 *   Nothing here needs sample-accurate sync; a recording played back is a
 *   fraction of a percent off in pitch and nobody can hear it.
 */
#ifndef EVK_AUDIO_PORT_H
#define EVK_AUDIO_PORT_H

/* ---- Speaker ---- */
#define EVK_SPK_PIN_DAC        23   /* DAC channel A analogue out            */
#define EVK_SPK_PIN_EN         22   /* TPA2037D1 EN - ACTIVE HIGH            */
#define EVK_SPK_EN_ACTIVE      1    /* level that turns the amplifier ON     */

/* ---- PDM microphone ---- */
#define EVK_MIC_PIN_CLK        18   /* even pin -> PDM_CLK                   */
#define EVK_MIC_PIN_DAT        19   /* odd pin  -> PDM_DIN                   */
#define EVK_MIC_PDM_CHANNEL    0    /* SELECT tied to GND -> left slot       */

/* ---- Stream geometry ---- */
#define EVK_MIC_RATE_HZ        16000  /* nominal; real capture is ~15875 Hz  */
#define EVK_MIC_FRAME_MS       20
#define EVK_MIC_FRAME_SAMPLES  (EVK_MIC_RATE_HZ / 1000 * EVK_MIC_FRAME_MS)  /* 320 */
#define EVK_SPK_NATIVE_RATE_HZ 32000  /* DAC native; other rates go through
                                         components/audio's linear resampler  */

/* AUADC digital gain: +18 dB is the hardware maximum and the only stage that
 * improves resolution (applied before the 16-bit truncation). Normal speech at
 * arm's length then peaks around -36 dBFS on this board. */
#define EVK_MIC_DIGITAL_GAIN_DB 18

#endif /* EVK_AUDIO_PORT_H */
