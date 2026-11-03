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
 * Host fakes for huk_prov.c and huk.c: eFuse array and shadow, SEC_ENG
 * hardware key, TZC, TRNG registers, console input, clock and image.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#define FAKE_EF_WORDS   32u
#define FAKE_GENS       8u

/* SEC_ENG hardware-key source of KEY_SEL 1 */
enum fake_engine {
    FAKE_ENG_LIVE,      /* slot 4 of the array at each block */
    FAKE_ENG_LATCH,     /* slot 4 at the last fake_power_on */
    FAKE_ENG_THIRD,     /* a key that is neither */
    FAKE_ENG_LOST,      /* live until the slot 4 read lock is burned */
};

struct fake_efuse {
    uint32_t fuse[FAKE_EF_WORDS];
    uint32_t shadow[FAKE_EF_WORDS];
    uint32_t drop[FAKE_EF_WORDS];   /* bits a program never sets */
    uint32_t add[FAKE_EF_WORDS];    /* bits a program sets as well */
    uint32_t lock_boot;             /* word 0x7C at the last power-on */
    uint32_t key_boot[4];           /* slot 4 at the last power-on */
    int rdlock_at_reset;            /* read locks act after a reset */
    int not_ready;
    int programs;
    int busy_at;                    /* program n (1-based) stays busy */
    int ctrl_after_busy;            /* load / set / program after it */
    int loads;
    uint32_t tzc;                   /* TZC ROM_TZSRG_CTRL */
    int tzc_stuck;                  /* writes to the field are lost */
    enum fake_engine engine;
    uint8_t key_src;                /* SE_AES_SBOOT key select */
};

struct fake_trng {
    uint32_t ctrl0;
    uint32_t test;
    uint32_t ctrl3;
    uint32_t rosc_c1;
    uint32_t sw_cfg0;
    int locked;             /* se_dbg_dis */
    int efuse_locked;       /* eFuse se_dbg_dis: locked from reset */
    int lock_ignored;       /* ef_sw_se_dbg_dis has no effect */
    int ctrl3_ro;           /* ctrl_3 writes are lost */
    int en_timeout;
    uint32_t out[FAKE_GENS][8];
    int ht_error[FAKE_GENS];
    int fail_gen[FAKE_GENS];
    uint32_t dout[8];       /* last generation, kept for the wipe check */
    int gens;
    int instantiates;
    void (*on_gen)(int n);  /* called after generation n */
};

struct fake_misc {
    const char *rx;         /* console input, NUL ends it */
    uint64_t ms;
    uint32_t bclk_mhz;
    uint8_t chip[8];
    uint8_t image[64];
    int irq_off;
    int chip_bad;           /* chip id parity error */
};

extern struct fake_efuse g_ef;
extern struct fake_trng g_trng;
extern struct fake_misc g_misc;

/* array -> shadow, read locks and engine key latched */
void fake_power_on(void);
/* TRNG registers at reset, generation script from seed */
void fake_trng_reset(uint32_t seed);
