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
 * @file gh7007_init_seq.h
 * @brief Panel power-on init sequences, selected by GH_PANEL (gh7007_port.h).
 *
 *   GH7007  : GoHi GH7007-01 driving a Tianma TM050 panel — vendor's LATEST
 *             reference "GH7007_TM050 SPI 20260629" (parse-generated, byte-
 *             verified, 197 register ops). 320x240 SPI input upscaled by the
 *             scaler + external GSRAM to 1024x600. The first test module
 *             showed banding with every init variant and is suspected
 *             defective — that history says nothing about this sequence.
 *             The paged (0xEE) manufacturer registers are undocumented in the
 *             chip datasheet, so this vendor sequence is the only source.
 *   ILI9341 : generic 240x320 TFT (native, no scaler).
 *
 * Encoding:  cmd = command/register byte (DC=0)
 *            nargs = number of parameter bytes (DC=1)
 *            args  = parameter bytes
 *            delay_ms = delay AFTER this entry (0 = none)
 *
 * Included by gh7007.c only.
 */
#ifndef GH7007_INIT_SEQ_H
#define GH7007_INIT_SEQ_H

#include <stdint.h>
#include "gh7007_port.h"      /* GH_PANEL selector */

typedef struct {
    uint8_t  cmd;
    uint8_t  nargs;
    uint8_t  args[16];   /* up to 15 for ILI9341 gamma tables; GH7007 uses <=4 */
    uint16_t delay_ms;
} gh_init_cmd_t;

/* clang-format off */
#if GH_PANEL == GH_PANEL_ILI9341
/* ILI9341 240x320 init — from apps/multimedia/ipro_agora_sentino ili9341_spi.c. */
static const gh_init_cmd_t GH7007_INIT_SEQ[] = {
    { 0x01, 0, { 0 }, 10 },                                  /* SWRESET       */
    { 0x11, 0, { 0 }, 120 },                                 /* SLPOUT        */
    { 0xCF, 3, { 0x00, 0xC1, 0x30 }, 0 },
    { 0xED, 4, { 0x64, 0x03, 0x12, 0x81 }, 0 },
    { 0xE8, 3, { 0x85, 0x00, 0x78 }, 0 },
    { 0xCB, 5, { 0x39, 0x2C, 0x00, 0x34, 0x02 }, 0 },
    { 0xF7, 1, { 0x20 }, 0 },
    { 0xEA, 2, { 0x00, 0x00 }, 0 },
    { 0xC0, 1, { 0x23 }, 0 },
    { 0xC1, 1, { 0x10 }, 0 },
    { 0xC5, 2, { 0x3E, 0x28 }, 0 },
    { 0xC7, 1, { 0x86 }, 0 },
    { 0x36, 1, { 0x88 }, 0 },
    { 0x3A, 1, { 0x55 }, 0 },
    { 0xB1, 2, { 0x00, 0x18 }, 0 },
    { 0xB6, 3, { 0x08, 0x82, 0x27 }, 0 },
    { 0xF2, 1, { 0x00 }, 0 },
    { 0x26, 1, { 0x01 }, 0 },
    { 0xE0, 15, { 0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00 }, 0 },
    { 0xE1, 15, { 0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F }, 0 },
    { 0x20, 0, { 0 }, 0 },
    { 0x29, 0, { 0 }, 10 },
};
#else
/* GH7007-01 + TM050 vendor LATEST init (GH7007_TM050 SPI 20260629). Paged via 0xEE.
 * Byte-verified (197 ops) vs the vendor source. Both this and the older V0.3 band
 * identically on this module -> module DC/DC power issue, not the init. */
static const gh_init_cmd_t GH7007_INIT_SEQ[] = {

    /* ---- page 01 ---- */
    { 0xEE, 1, { 0x01 }, 0 },
    { 0xEA, 1, { 0x07 }, 0 },
    { 0xEB, 1, { 0x12 }, 0 },
    { 0x02, 1, { 0x38 }, 0 },
    { 0x05, 1, { 0x40 }, 0 },   /* bist osc */
    { 0x0A, 1, { 0x4A }, 0 },   /* vcom */
    { 0x0E, 1, { 0x00 }, 0 },
    { 0x10, 1, { 0x60 }, 0 },
    { 0x16, 1, { 0x08 }, 0 },
    { 0x14, 1, { 0x04 }, 0 },   /* source i */
    { 0x19, 1, { 0x77 }, 0 },   /* source i */
    { 0x1A, 1, { 0x7E }, 0 },   /* source i */
    { 0x1B, 1, { 0x7E }, 0 },   /* source i */
    { 0x1C, 1, { 0x81 }, 0 },   /* zigzag */
    { 0x1D, 1, { 0x04 }, 0 },   /* zigzag */
    { 0x21, 1, { 0x01 }, 0 },   /* GAS */
    { 0x27, 1, { 0x11 }, 0 },   /* vgh/L pump & RT */
    { 0x28, 1, { 0x25 }, 0 },   /* vgh 17.5 */
    { 0x29, 1, { 0x10 }, 0 },   /* vgl 12 */
    { 0x39, 1, { 0x08 }, 0 },   /* VDDD */
    { 0x4C, 1, { 0xFF }, 0 },

    /* ---- page 02 (gamma 2.2) ---- */
    { 0xEE, 1, { 0x02 }, 0 },
    { 0x39, 1, { 0x30 }, 0 },   /* 4.5V */
    { 0x3C, 1, { 0x30 }, 0 },   /* 4.5V */
    { 0x00, 1, { 0x00 }, 0 },
    { 0x01, 1, { 0x02 }, 0 },
    { 0x02, 1, { 0x02 }, 0 },
    { 0x03, 1, { 0x06 }, 0 },
    { 0x04, 1, { 0x0B }, 0 },
    { 0x05, 1, { 0x1A }, 0 },
    { 0x06, 1, { 0x0C }, 0 },
    { 0x07, 1, { 0x0D }, 0 },
    { 0x08, 1, { 0x0F }, 0 },
    { 0x09, 1, { 0x0E }, 0 },
    { 0x0A, 1, { 0x11 }, 0 },
    { 0x0B, 1, { 0x3B }, 0 },
    { 0x0C, 1, { 0x33 }, 0 },
    { 0x0D, 1, { 0x39 }, 0 },
    { 0x0E, 1, { 0x1D }, 0 },
    { 0x0F, 1, { 0x22 }, 0 },
    { 0x10, 1, { 0x3F }, 0 },
    { 0x20, 1, { 0x00 }, 0 },
    { 0x21, 1, { 0x02 }, 0 },
    { 0x22, 1, { 0x02 }, 0 },
    { 0x23, 1, { 0x06 }, 0 },
    { 0x24, 1, { 0x0B }, 0 },
    { 0x25, 1, { 0x1A }, 0 },
    { 0x26, 1, { 0x0C }, 0 },
    { 0x27, 1, { 0x0D }, 0 },
    { 0x28, 1, { 0x0F }, 0 },
    { 0x29, 1, { 0x0E }, 0 },
    { 0x2A, 1, { 0x11 }, 0 },
    { 0x2B, 1, { 0x3B }, 0 },
    { 0x2C, 1, { 0x33 }, 0 },
    { 0x2D, 1, { 0x39 }, 0 },
    { 0x2E, 1, { 0x1D }, 0 },
    { 0x2F, 1, { 0x22 }, 0 },
    { 0x30, 1, { 0x3F }, 0 },

    /* ---- page 04 ---- */
    { 0xEE, 1, { 0x04 }, 0 },
    { 0x00, 1, { 0x0E }, 0 },   /* 1024 ch */
    { 0x01, 1, { 0x00 }, 0 },
    { 0x02, 1, { 0x78 }, 0 },   /* 120 */
    { 0x03, 1, { 0x01 }, 0 },
    { 0x04, 1, { 0x40 }, 0 },   /* 320 */
    { 0x06, 1, { 0x14 }, 0 },
    { 0x07, 1, { 0x05 }, 0 },
    { 0x08, 1, { 0x12 }, 0 },
    { 0x0A, 1, { 0x27 }, 0 },   /* inv */
    { 0x12, 1, { 0xCC }, 0 },
    { 0x13, 1, { 0x0C }, 0 },
    { 0x16, 1, { 0x48 }, 0 },
    { 0x20, 1, { 0x40 }, 0 },   /* use bist osc */
    { 0x25, 1, { 0x40 }, 0 },
    /* 0x40/0x41 BIST left commented in the 20260629 source -> omitted */

    /* ---- page 05 ---- */
    { 0xEE, 1, { 0x05 }, 0 },
    { 0x00, 1, { 0x04 }, 0 },
    { 0x01, 1, { 0x0C }, 0 },
    { 0x02, 1, { 0x05 }, 0 },
    { 0x03, 1, { 0x05 }, 0 },
    { 0x07, 1, { 0x0C }, 0 },
    { 0x08, 1, { 0x14 }, 0 },
    { 0x09, 1, { 0x00 }, 0 },
    { 0x10, 1, { 0x14 }, 0 },
    { 0x11, 1, { 0x1C }, 0 },
    { 0x12, 1, { 0x04 }, 0 },
    { 0x13, 1, { 0x04 }, 0 },
    { 0x19, 1, { 0x2E }, 0 },
    { 0x1A, 1, { 0xF7 }, 0 },
    { 0x1B, 1, { 0x02 }, 0 },
    { 0x43, 1, { 0x07 }, 0 },
    { 0x44, 1, { 0xC0 }, 0 },
    { 0x46, 1, { 0x77 }, 0 },

    /* ---- page 06 ---- */
    { 0xEE, 1, { 0x06 }, 0 },
    { 0x00, 1, { 0x01 }, 0 },
    { 0x02, 1, { 0x67 }, 0 },
    { 0x03, 1, { 0x45 }, 0 },
    { 0x06, 1, { 0xCD }, 0 },
    { 0x08, 1, { 0x67 }, 0 },
    { 0x09, 1, { 0x45 }, 0 },
    { 0x0A, 1, { 0x23 }, 0 },
    { 0x0B, 1, { 0x01 }, 0 },
    { 0x0C, 1, { 0xEF }, 0 },
    { 0x0D, 1, { 0xCD }, 0 },
    { 0x0E, 1, { 0xAB }, 0 },
    { 0x0F, 1, { 0x89 }, 0 },

    /* ---- page 07 (GIP) ---- */
    { 0xEE, 1, { 0x07 }, 0 },
    { 0x00, 1, { 0x19 }, 0 },
    { 0x01, 1, { 0x1B }, 0 },
    { 0x02, 1, { 0x1D }, 0 },
    { 0x03, 1, { 0x1F }, 0 },
    { 0x04, 1, { 0x11 }, 0 },
    { 0x05, 1, { 0x13 }, 0 },
    { 0x06, 1, { 0x15 }, 0 },
    { 0x07, 1, { 0x17 }, 0 },
    { 0x08, 1, { 0x05 }, 0 },
    { 0x09, 1, { 0x07 }, 0 },
    { 0x0A, 1, { 0x0C }, 0 },
    { 0x0B, 1, { 0x0D }, 0 },
    { 0x0C, 1, { 0x3C }, 0 },
    { 0x0D, 1, { 0x3C }, 0 },
    { 0x0E, 1, { 0x3C }, 0 },
    { 0x0F, 1, { 0x3C }, 0 },
    { 0x10, 1, { 0x3C }, 0 },
    { 0x11, 1, { 0x01 }, 0 },
    { 0x12, 1, { 0x3C }, 0 },
    { 0x13, 1, { 0x3C }, 0 },
    { 0x14, 1, { 0x3C }, 0 },
    { 0x15, 1, { 0x3C }, 0 },
    { 0x20, 1, { 0x18 }, 0 },
    { 0x21, 1, { 0x1A }, 0 },
    { 0x22, 1, { 0x1C }, 0 },
    { 0x23, 1, { 0x1E }, 0 },
    { 0x24, 1, { 0x10 }, 0 },
    { 0x25, 1, { 0x12 }, 0 },
    { 0x26, 1, { 0x14 }, 0 },
    { 0x27, 1, { 0x16 }, 0 },
    { 0x28, 1, { 0x04 }, 0 },
    { 0x29, 1, { 0x06 }, 0 },
    { 0x2A, 1, { 0x0C }, 0 },
    { 0x2B, 1, { 0x0D }, 0 },
    { 0x2C, 1, { 0x3C }, 0 },
    { 0x2D, 1, { 0x3C }, 0 },
    { 0x2E, 1, { 0x3C }, 0 },
    { 0x2F, 1, { 0x3C }, 0 },
    { 0x30, 1, { 0x3C }, 0 },
    { 0x31, 1, { 0x00 }, 0 },
    { 0x32, 1, { 0x3C }, 0 },
    { 0x33, 1, { 0x3C }, 0 },
    { 0x34, 1, { 0x3C }, 0 },
    { 0x35, 1, { 0x3C }, 0 },

    /* ---- page 08 ---- */
    { 0xEE, 1, { 0x08 }, 0 },
    { 0x12, 1, { 0xDE }, 0 },
    { 0x01, 1, { 0x14 }, 0 },
    { 0x20, 1, { 0x00 }, 0 },
    { 0x35, 1, { 0x03 }, 0 },
    { 0x4D, 1, { 0x2F }, 0 },

    /* ---- page 0E (scaler): in 320x240 -> out 1024x600 ---- */
    { 0xEE, 1, { 0x0E }, 0 },
    { 0x80, 1, { 0x01 }, 0 },
    { 0x84, 1, { 0xF0 }, 0 },   /* in 240 */
    { 0x85, 1, { 0x40 }, 0 },
    { 0x86, 1, { 0x01 }, 0 },   /* in 320 */
    { 0x87, 1, { 0x58 }, 0 },   /* out 600 */
    { 0x88, 1, { 0x00 }, 0 },   /* out 1024 */
    { 0x89, 1, { 0x24 }, 0 },

    /* ---- page 01 (gsram osc) ---- */
    { 0xEE, 1, { 0x01 }, 0 },
    { 0x08, 1, { 0x30 }, 0 },   /* oscram 100M (1a=66M 25=86M 30=100M 4b=133M) */
    { 0x3A, 1, { 0xEF }, 0 },   /* psram io ldo */

    /* ---- page 0E (gsram timing) ---- */
    { 0xEE, 1, { 0x0E }, 0 },
    { 0x62, 1, { 0x11 }, 0 },   /* RCSH,WCSH */
    { 0x63, 1, { 0x11 }, 0 },   /* RCSS,WCSS */
    { 0x64, 1, { 0x11 }, 0 },   /* RCHI,RCSHI */
    { 0x65, 1, { 0xC8 }, 0 },   /* wr,rd burst max */
    { 0x67, 1, { 0x72 }, 0 },   /* psram bist, rps_delay */
    { 0x68, 1, { 0x2B }, 0 },   /* driver strong */
    { 0x6A, 1, { 0x07 }, 0 },   /* RBX,BT,BL */
    { 0x6E, 1, { 0x20 }, 0 },   /* rwdsi +ckp delay */
    { 0x6F, 1, { 0x20 }, 0 },   /* clk sel, hbp */
    { 0x70, 1, { 0x20 }, 0 },   /* hfp */
    { 0x72, 1, { 0x1F }, 0 },   /* high speed +vfp */
    { 0x74, 1, { 0x06 }, 0 },   /* output hactive 15:8 */
    { 0x75, 1, { 0x40 }, 0 },   /* output hactive 7:0 */
    { 0x76, 1, { 0x10 }, 0 },   /* dq out delay, ckp out delay */
    { 0x77, 1, { 0x01 }, 0 },   /* dq in delay, dm out delay */
    { 0x6D, 1, { 0x8F }, 0 },   /* oscram skew, DQ driver ldo_en */

    /* ---- page 0F ---- */
    { 0xEE, 1, { 0x0F }, 0 },
    { 0x08, 1, { 0x60 }, 0 },
    { 0x09, 1, { 0x00 }, 0 },
    { 0x0F, 1, { 0x00 }, 0 },
    { 0x10, 1, { 0x00 }, 0 },

    /* ---- page 00 (standard commands) ---- */
    { 0xEA, 1, { 0x00 }, 0 },
    { 0xEB, 1, { 0x00 }, 0 },
    { 0xEE, 1, { 0x00 }, 0 },
    { 0x3A, 1, { 0x55 }, 0 },   /* COLMOD: 16-bit both IFs (vendor file had the
                                 * ambiguous 0x75 = 24-bit RGB-IF nibble; bench
                                 * 2026-07-30: 0x55 turned yellow from green to
                                 * yellow-green — major mix-color improvement) */   /* COLMOD RGB565 */
    { 0x30, 4, { 0x00, 0x60, 0x00, 0x90 }, 0 },
    { 0x31, 4, { 0x00, 0x80, 0x00, 0xC0 }, 0 },
    { 0x11, 0, { 0 }, 400 },    /* SLPOUT, wait 400 ms */
    { 0x29, 0, { 0 }, 0 },      /* DISPON */
    { 0x36, 1, { 0x08 }, 1 },   /* MADCTL: BGR only (bench: red rendered blue
                                 * without BGR). LR bit dropped — the vendor's
                                 * 0x02 mirrored the image (ruler ticks showed
                                 * up on the wrong side). */
};
#endif /* GH_PANEL */
/* clang-format on */

#define GH7007_INIT_SEQ_LEN (sizeof(GH7007_INIT_SEQ) / sizeof(GH7007_INIT_SEQ[0]))

#endif /* GH7007_INIT_SEQ_H */
