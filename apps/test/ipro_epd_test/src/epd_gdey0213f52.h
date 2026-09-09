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

/*
 * GoodDisplay GDEY0213F52 — 2.13" 250x122 BWRY e-paper (JD79676 controller)
 *
 * Pinout (IPRO7AI PDB J2 SPI0 header — RES & BUSY NOT connected):
 *   J2.1  VCC  = 3V3
 *   J2.2  CS   = GPIO10 (software output)
 *   J2.3  SCK  = GPIO15 (SPI0_SCLK)
 *   J2.4  MOSI = GPIO16 (SPI0_MOSI)
 *   J2.5  D/C  = GPIO17 (software output — re-purposed MISO line; EPD is write-only)
 *   J2.6  GND
 *   RES  = tie e-paper module RES pin to VCC (or 10k pull-up; do NOT leave floating)
 *   BUSY = leave open; driver uses worst-case timing delays instead
 *
 * Tradeoffs of dropping RES/BUSY:
 *   - No HW reset path -> driver skips DSLP (DSLP requires RES to wake). Panel
 *     stays in POF after refresh, woken by PON next cycle. Idle current is a
 *     bit higher than DSLP but still uA-range.
 *   - All waits are fixed worst-case delays (DRF = 18s) — no early-exit.
 *   - epd_busy_level() always returns 1 (assumed idle).
 */

#ifndef EPD_GDEY0213F52_H
#define EPD_GDEY0213F52_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Native panel resolution (active area). The controller buffer is 128x250
 * because rows are byte-aligned at 4 px / byte (2 bpp). The trailing 6 px
 * per row are off-glass — write any value, they won't show. */
#define EPD_PANEL_W            122
#define EPD_PANEL_H            250
#define EPD_BUF_W              128                 /* rounded up to 4-px boundary */
#define EPD_BYTES_PER_ROW      (EPD_BUF_W / 4)     /* 2 bpp -> 4 px / byte */
#define EPD_FRAME_BYTES        (EPD_BYTES_PER_ROW * EPD_PANEL_H) /* 8000 bytes */

/* 4-color packed values (2 bits per pixel) — datasheet default DDX=1 mapping.
 * Verified on a real GDEY0213F52 by individually filling each color: 0x1
 * yields White, 0x3 yields Red, etc. */
typedef enum {
    EPD_BLACK  = 0x0,
    EPD_WHITE  = 0x1,
    EPD_YELLOW = 0x2,
    EPD_RED    = 0x3,
} epd_color_t;

int  epd_init(void);
int  epd_fill(epd_color_t color);
int  epd_stripes_4color(void);                     /* horizontal K/W/Y/R bands */
int  epd_display(const uint8_t *buf, uint32_t len);/* len must be EPD_FRAME_BYTES */
int  epd_clear(int cycles);                        /* N full white refreshes (default 3) */
int  epd_sleep(void);
int  epd_busy_level(void);                         /* 1 = idle, 0 = busy */
int  epd_auto_refresh(void);                       /* R17H: PON->DRF->POF, no DTM */

#ifdef __cplusplus
}
#endif
#endif /* EPD_GDEY0213F52_H */
