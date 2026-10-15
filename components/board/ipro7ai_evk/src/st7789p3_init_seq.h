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
 * @file st7789p3_init_seq.h
 * @brief ST7789P3 power-on sequences, grounded in the Sitronix datasheet
 *        (ST7789P3, Version 0.0, 2021/11).
 *
 * The module vendor's spec (LCM-T2D4BP-084 V1.0) supplies NO init sequence at
 * all - its section 8 "Timing characteristics" reads, verbatim, "Reference
 * Resources ST7789P3 Datasheet". So both sequences below are derived from the
 * IC datasheet, not copied from a vendor demo.
 *
 * ============================================================================
 * WHAT THE CHIP ALREADY DOES FOR YOU
 * ============================================================================
 * Every ST7789P3 Command-Table-2 register was checked against its datasheet
 * "Default Value" box. They are all sensible for a 240x320 panel out of reset:
 *
 *   B0h RAMCTRL   00h F0h      B7h GCTRL   35h  (VGH 13.26 V / VGL -10.43 V)
 *   B2h PORCTRL   0C 0C 00 33 33            BBh VCOMS   20h  (0.9 V)
 *   C0h LCMCTRL   2Ch          C2h VRHEN 01h FFh   C3h VRHS  0Bh
 *   C6h FRCTRL2   0Fh (60 Hz)  D0h PWCTRL1 A4h 81h (AVDD 6.8 / AVCL -4.4 / VDS 2.3)
 *   E4h GATECTRL  27h 00h 10h  -> NL=27h = 320 gate lines, SCN=0 (no offset)
 *   E7h SPI2EN    00h          -> 2-data-lane mode OFF, so single SDA works
 *   E8h PWCTRL2   51h          E9h EQCTRL 0Eh 0Eh 07h
 *
 * E0h/E1h gamma are NOT zeroed at reset either - the datasheet lists a real
 * monotonic default curve (transcribed as ST7789P3_GAMMA_P/N_DEFAULT below).
 * That is why ST_INIT_MINIMAL is expected to produce a correct picture on its
 * own, and why it is the default: on a brand-new panel, writing folklore
 * register values from a *different* module can only make things worse.
 *
 * ============================================================================
 * ST7789V COMPATIBILITY - ONE REAL TRAP
 * ============================================================================
 * The ST7789P3 command set matches ST7789V closely enough that copy-pasted
 * ST7789V init code mostly works... except:
 *
 *   >>> There is NO C4h (VDVS) command on the ST7789P3. <<<
 *
 * "VDVS" and "C4h" appear ZERO times in all 276 pages of the datasheet. Nearly
 * every ST7789V init sequence in the wild contains `0xC4, 0x20` right after
 * `0xC3 VRHS`. Sending it here writes an undefined command. Neither sequence
 * below contains C4h - do not "restore" it.
 *
 * Also absent vs. ST7789V: nothing else material. C5h VCMOFSET, D6h GATESEL,
 * DFh CMD2EN, E8h PWCTRL2 and E9h EQCTRL all exist with matching semantics.
 */
#ifndef ST7789P3_INIT_SEQ_H
#define ST7789P3_INIT_SEQ_H

#include <stdint.h>
#include <stddef.h>

/** One step of an init sequence. delay_ms runs AFTER the command+args. */
typedef struct {
    uint8_t        cmd;
    uint8_t        nargs;
    uint16_t       delay_ms;
    const uint8_t *args;
} st7789p3_cmd_t;

/* ---------------------------------------------------------------------------
 * Datasheet default gamma, transcribed from the E0h/E1h "Default value" tables
 * and packed into wire order per the parameter bit layouts:
 *   P1 = (V63<<4)|V0   P6 = (J0<<4)|V13   P8 = (V36<<4)|V27   P10 = (J1<<4)|V50
 * Not sent by either sequence - kept so `lcd gamma default` can restore the
 * silicon curve after experimenting, and as a reference for what "no gamma
 * write" actually means on this part.
 * -------------------------------------------------------------------------*/
static const uint8_t ST7789P3_GAMMA_P_DEFAULT[14] = {
    0xD0, 0x00, 0x02, 0x07, 0x0B, 0x1A, 0x31,
    0x54, 0x40, 0x29, 0x12, 0x12, 0x12, 0x17
};
static const uint8_t ST7789P3_GAMMA_N_DEFAULT[14] = {
    0xD0, 0x00, 0x02, 0x07, 0x05, 0x25, 0x2D,
    0x44, 0x44, 0x1C, 0x18, 0x16, 0x1C, 0x1D
};

/* ---------------------------------------------------------------------------
 * Sequence 0: MINIMAL (default)
 *
 * Touches only what a host MUST set, and leaves every power/timing/gamma
 * register at the silicon default documented above. This is the sequence to
 * trust first on an unproven panel.
 *
 * 3Ah COLMOD = 55h : 16 bit/pixel control interface (RGB565). The H/W-reset
 *                    default is 18 bit/pixel, so this one IS mandatory.
 * 36h MADCTL = 00h : portrait, top-left origin, RGB channel order. Flip to
 *                    08h if red and blue come out swapped (`lcd bgr on`).
 * 21h INVON        : ST7789 drives normally-black IPS glass inverted - without
 *                    this the picture comes out as a photographic negative.
 *                    Toggle with `lcd inv off` if this panel disagrees.
 * -------------------------------------------------------------------------*/
static const uint8_t st_arg_colmod_565[]  = { 0x55 };
static const uint8_t st_arg_madctl_rgb[]  = { 0x00 };

static const st7789p3_cmd_t st7789p3_init_minimal[] = {
    { 0x01, 0, 150, NULL },                  /* SWRESET - 120 ms blanking max  */
    { 0x11, 0, 120, NULL },                  /* SLPOUT  - 120 ms before next   */
    { 0x3A, 1,   0, st_arg_colmod_565 },     /* COLMOD  = RGB565               */
    { 0x36, 1,   0, st_arg_madctl_rgb },     /* MADCTL  = portrait, RGB        */
    { 0x21, 0,  10, NULL },                  /* INVON   - IPS normally black   */
    { 0x13, 0,  10, NULL },                  /* NORON   - normal display mode  */
    { 0x29, 0,  20, NULL },                  /* DISPON                         */
};

/* ---------------------------------------------------------------------------
 * Sequence 1: TUNED
 *
 * The minimal sequence plus the power/timing/gamma block that ST7789-family
 * modules are conventionally shipped with. Use it if the minimal sequence
 * shows a washed-out, dim, or flickery image - i.e. if this particular glass
 * wants more drive than the silicon defaults give.
 *
 * Every value below is legal per THIS datasheet; the deltas from default are:
 *   BBh VCOMS   20h -> 19h   VCOM 0.9 V -> 0.725 V
 *   C3h VRHS    0Bh -> 12h   VRH  ~4.1 V -> ~4.45 V (more source swing)
 *   D0h PWCTRL1 ..81h -> ..A1h  AVCL -4.4 V -> -4.8 V (AVDD/VDS unchanged)
 *   E0h/E1h gamma      generic ST7789 module curve, not the silicon default
 *
 * CAVEAT, stated plainly: the gamma table is the widely used ST7789 module
 * curve, NOT a curve characterised for this glass. The module vendor supplies
 * none. If colours look off with sequence 1, sequence 0 is the honest baseline.
 * -------------------------------------------------------------------------*/
static const uint8_t st_arg_porctrl[]  = { 0x0C, 0x0C, 0x00, 0x33, 0x33 };
static const uint8_t st_arg_gctrl[]    = { 0x35 };
static const uint8_t st_arg_vcoms[]    = { 0x19 };
static const uint8_t st_arg_lcmctrl[]  = { 0x2C };
static const uint8_t st_arg_vrhen[]    = { 0x01 };
static const uint8_t st_arg_vrhs[]     = { 0x12 };
static const uint8_t st_arg_frctrl2[]  = { 0x0F };
static const uint8_t st_arg_pwctrl1[]  = { 0xA4, 0xA1 };
static const uint8_t st_arg_gamma_p[]  = { 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F,
                                           0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23 };
static const uint8_t st_arg_gamma_n[]  = { 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F,
                                           0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23 };

static const st7789p3_cmd_t st7789p3_init_tuned[] = {
    { 0x01, 0, 150, NULL },                  /* SWRESET                        */
    { 0x11, 0, 120, NULL },                  /* SLPOUT                         */

    { 0x3A, 1,   0, st_arg_colmod_565 },     /* COLMOD  = RGB565               */
    { 0x36, 1,   0, st_arg_madctl_rgb },     /* MADCTL  = portrait, RGB        */

    { 0xB2, 5,   0, st_arg_porctrl },        /* PORCTRL  (= default)           */
    { 0xB7, 1,   0, st_arg_gctrl },          /* GCTRL    (= default)           */
    { 0xBB, 1,   0, st_arg_vcoms },          /* VCOMS    0.725 V               */
    { 0xC0, 1,   0, st_arg_lcmctrl },        /* LCMCTRL  (= default)           */
    { 0xC2, 1,   0, st_arg_vrhen },          /* VRHEN    VRH from C3h          */
    { 0xC3, 1,   0, st_arg_vrhs },           /* VRHS     ~4.45 V               */
    /* NOTE: no 0xC4 here on purpose - see the header comment. */
    { 0xC6, 1,   0, st_arg_frctrl2 },        /* FRCTRL2  60 Hz (= default)     */
    { 0xD0, 2,   0, st_arg_pwctrl1 },        /* PWCTRL1  AVDD 6.8 / AVCL -4.8  */

    { 0xE0, 14,  0, st_arg_gamma_p },        /* PVGAMCTRL                      */
    { 0xE1, 14,  0, st_arg_gamma_n },        /* NVGAMCTRL                      */

    { 0x21, 0,  10, NULL },                  /* INVON                          */
    { 0x13, 0,  10, NULL },                  /* NORON                          */
    { 0x29, 0,  20, NULL },                  /* DISPON                         */
};

#define ST7789P3_INIT_MINIMAL_LEN \
    (sizeof(st7789p3_init_minimal) / sizeof(st7789p3_init_minimal[0]))
#define ST7789P3_INIT_TUNED_LEN \
    (sizeof(st7789p3_init_tuned) / sizeof(st7789p3_init_tuned[0]))

#endif /* ST7789P3_INIT_SEQ_H */
