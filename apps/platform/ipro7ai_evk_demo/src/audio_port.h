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
 * @file audio_port.h
 * @brief Speaker (DAC + class-D amp) and PDM microphone wiring for the
 *        IPRO7AI_FMB board, read off the "Speaker" / "DMIC" schematic sheet.
 *
 * ---------------------------------------------------------------------------
 * SPEAKER
 * ---------------------------------------------------------------------------
 *   GPIO23  SPK_DAC_OUT -> R27 470R -> C20 3.3nF to GND (RC reconstruction
 *                          filter, fc ~= 102 kHz) -> C19 2.2uF DC block
 *                       -> R26 4.7k -> U6 IN+ (A1)
 *   GPIO22  SPK_EN      -> R23 0R -> R24 1k -> U6 EN (C2), R25 10k to GND
 *   U6 = TPA2037D1 mono class-D. IN- (C1) is AC-grounded through C22 2.2uF,
 *        so the input is single-ended.
 *        OUT+ (C3) / OUT- (A3) -> FB3/FB4 BTLK1608300T ferrites -> R28/R30 0R
 *        -> SPK_P / SPK_N -> J1 (Molex 532610271), with D4/D5 D1250-5R5B TVS.
 *   PA_VDD <- R32 0R <- VBAT (the amp runs off the battery rail, not 3V3).
 *
 * >>> SPK_EN IS ACTIVE HIGH ON THIS BOARD, AND THE SDK ASSUMES ACTIVE LOW. <<<
 *
 * R25 10k pulls the EN node to ground, so the amp is off until GPIO22 is
 * driven high (the 1k/10k divider lands EN at ~0.91 x VIO). But
 * components/audio/src/audio_output_dac.c hardcodes the IPRO7 EVB's LM4871,
 * whose SHUTDOWN pin is active low: it writes the enable GPIO LOW on
 * audio_output_start() and HIGH on stop. On this board that is exactly
 * inverted - the amp would be muted while playing and unmuted while idle.
 * audio_output_cfg_t has a dac_spk_gpio field but NO polarity field, so the
 * only fix is to re-assert GPIO22 after every start/stop. audio.c does that
 * in dac_open()/dac_close(); do not remove it.
 *
 * GPIO22 is also the SoC's DAC channel B pin, so using it as the amp enable
 * makes this board mono - the same trade-off the IPRO7 EVB makes.
 *
 * ---------------------------------------------------------------------------
 * PDM MICROPHONE
 * ---------------------------------------------------------------------------
 *   GPIO18  DMIC_CLK -> R31 0R -> DMIC1 pin 4 (CLK)
 *   GPIO19  DMIC_DAT <- R29 0R <- DMIC1 pin 1 (DATA)
 *   DMIC1 = Infineon IM69D129FV01. VDD (pin 5) <- DMIC_1V8, C21 100nF.
 *           SELECT (pin 2) is tied to GND -> the mic drives DATA in the LEFT
 *           channel slot, so AUADC must be told PDM_CH_LEFT.
 *   DMIC_1V8 <- FB2 BTLK1005121T <- 1V8_SOC.
 *
 * IM69D129 clocking, which matters when testing it by hand: normal mode needs
 * roughly 1.0-3.25 MHz on CLK. Below that the part drops into low-power and
 * then sleep mode, and in sleep its DATA output is HIGH-Z - so a hand-clocked
 * test running under ~1 MHz reads a flat line whether the mic is healthy or
 * dead, and proves nothing. The AUADC clocks PDM at 2.032 MHz, comfortably in
 * spec, which is why `mic probe` borrows the AUADC's clock rather than
 * bit-banging its own.
 *
 * The AUADC PDM roles follow pin parity, like I2C: EVEN pin = PDM_CLK, ODD
 * pin = PDM_DIN. GPIO18/19 fit that. (The "pdm_clk_pin: 0, 2, 8" comment in
 * bsp/hal/inc/hal_auadc.h is an incomplete example list, not a constraint -
 * hal_auadc.c muxes whatever pins it is handed, and ipro7_demo already uses
 * clk=12 / data=11.)
 *
 * >>> THE MIC IS ON A 1.8 V RAIL BUT SHARES A VDDIO BANK WITH THE 3.3 V LCD. <<<
 *
 * On IPRO7 the pad banks are VDDIO_1 = GPIO0-9 + GPIO36-41, VDDIO_2 =
 * GPIO10-19, VDDIO_3 = GPIO20-35. DMIC_CLK/DAT are GPIO18/19, i.e. VDDIO_2 -
 * the same bank as the LCD on GPIO10-17, which needs 3.3 V logic. If VDDIO_2
 * sits at 3.3 V then the SoC clocks a 1.8 V-supplied microphone at 3.3 V, and
 * reads its 1.8 V data back against a 3.3 V input threshold. Worth confirming
 * against the board's rail assignment before chasing a silent mic in
 * firmware - see the README.
 *
 * ---------------------------------------------------------------------------
 * RATES - NEITHER SIDE IS EXACTLY 16 kHz
 * ---------------------------------------------------------------------------
 * Bench-measured on this SoC (recorded in the SDK's ipro_pdm_mic_demo work):
 *   PDM capture: 2.032 MHz / 128 = 15875 Hz, 0.8 % slow of nominal 16 kHz.
 *   DAC:         rate = 1992465 / div, because the DAC root clock is really
 *                ~31.88 MHz, not the nominal 32 MHz. 16000 Hz would need
 *                div = 124.53 - unreachable. div 125 gives 15938 Hz.
 * Nothing here needs sample-accurate sync, so both are left at their nominal
 * settings; just do not expect a recording saved as "16 kHz" to play back at
 * exactly the right pitch.
 */
#ifndef AUDIO_PORT_H
#define AUDIO_PORT_H

/* ---- Speaker ---- */
#define SPK_PIN_DAC        23   /* DAC channel A analogue out */
#define SPK_PIN_EN         22   /* TPA2037D1 EN - ACTIVE HIGH */
#define SPK_EN_ACTIVE      1    /* level that turns the amplifier ON */

/* ---- PDM microphone ---- */
#define MIC_PIN_CLK        18   /* even pin -> PDM_CLK  */
#define MIC_PIN_DAT        19   /* odd pin  -> PDM_DIN  */
#define MIC_PDM_CHANNEL    0    /* SELECT tied to GND -> left slot */

/* ---- Stream geometry ---- */
#define AUDIO_RATE         16000  /* nominal; real capture is ~15875 Hz */
#define AUDIO_FRAME_SAMP   320    /* 20 ms at the nominal rate */
#define AUDIO_TONE_RATE    32000  /* DAC native rate - skips the resampler */

/* ---- Gain defaults, both bench-proven on the SDK's PDM mic work ----
 * A PDM MEMS mic sits around -26 dBFS at 94 dB SPL, so normal speech at 30 cm
 * lands near -54 dBFS. With every stage at unity that is inaudible; the fix is
 * +18 dB of AUADC digital gain (the hardware maximum, applied before the
 * 16-bit truncation so it is the only stage that improves resolution) plus
 * ~16 dB of software make-up. Above about +19 dB of make-up the ~18.7 dB crest
 * factor of speech starts clipping. */
#define MIC_DIGITAL_GAIN_DB   18    /* AUADC digital gain, hardware maximum */

/*
 * Software make-up applied on playback.
 *
 * The SDK's PDM demo settled on +16 dB, but that was matched to a recording
 * whose input peaked at -17.9 dBFS. Measured on THIS board, normal speech at
 * arm's length peaks at about -36 dBFS after the +18 dB AUADC gain - which is
 * right on the SDK's own speech reference of -35.2 dBFS, so the microphone is
 * behaving exactly as expected. That leaves ~36 dB of headroom, and +16 dB
 * simply throws most of it away, which is why playback sounded quiet.
 *
 * 30 dB keeps roughly 6 dB of margin against a -36 dBFS peak. Louder or closer
 * sources need less: use `mic play auto`, which derives the gain from what was
 * actually recorded instead of assuming a speaking distance.
 */
#define SPK_MAKEUP_GAIN_DB    30

/*
 * Keep the amplifier muted across the DAC's own steps.
 *
 * The DAC idles at mid-scale (code 512) and sits at zero when stopped, so
 * every open and close is a half-rail step through the DC-blocking cap. With
 * the amplifier live that step is a pop. Muting either side of it costs a
 * fraction of a second and removes the pop entirely.
 *
 * The open-side figure is the one that matters: it has to cover the cap
 * charging to mid-scale, which is an RC and not instant.
 */
#define SPK_UNMUTE_SETTLE_MS  60
#define SPK_MUTE_SETTLE_MS    10

#endif /* AUDIO_PORT_H */
