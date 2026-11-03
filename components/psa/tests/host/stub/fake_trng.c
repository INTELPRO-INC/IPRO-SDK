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
 * TRNG register, console, clock and image fakes of test_huk_prov.
 * se_dbg_dis (raised through ef_sw_se_dbg_dis) makes rosc_c1 and the
 * test register read-only; generation n returns g_trng.out[n].
 */
#include <string.h>

#include "fake_hukp.h"
#include "huk_prov.h"

#define CTRL0_EN        (1u << 2)
#define CTRL0_HT_ERROR  (1u << 4)
#define SW_SE_DBG_DIS   (1u << 22)
#define CTRL3_RESET     0x037a4203u
#define ROSC_C1_RESET   0xF000u

struct fake_trng g_trng;
struct fake_misc g_misc;

void fake_trng_reset(uint32_t seed)
{
    uint32_t x = seed != 0 ? seed : 0x9e3779b9u;

    memset(&g_trng, 0, sizeof(g_trng));
    g_trng.ctrl3 = CTRL3_RESET;
    g_trng.rosc_c1 = ROSC_C1_RESET;
    for (uint32_t g = 0; g < FAKE_GENS; g++) {
        for (uint32_t i = 0; i < 8; i++) {
            x ^= x << 13;
            x ^= x >> 17;
            x ^= x << 5;
            g_trng.out[g][i] = x;
        }
    }
}

uint32_t hukp_reg_read(enum hukp_reg r)
{
    int locked = g_trng.locked || g_trng.efuse_locked;

    switch (r) {
    case HUKP_TRNG_CTRL_0:
        return g_trng.ctrl0;
    case HUKP_TRNG_TEST:
        return g_trng.test;
    case HUKP_TRNG_CTRL_3:
        return g_trng.ctrl3;
    case HUKP_TRNG_ROSC_C1:
        return g_trng.rosc_c1;
    case HUKP_SE_PROT_RD:
        return locked ? 1u << 31 : 0;
    case HUKP_EF_SW_CFG_0:
        return g_trng.sw_cfg0;
    }
    return 0;
}

void hukp_reg_write(enum hukp_reg r, uint32_t v)
{
    int locked = g_trng.locked || g_trng.efuse_locked;

    switch (r) {
    case HUKP_TRNG_CTRL_0:
        g_trng.ctrl0 = v;
        break;
    case HUKP_TRNG_TEST:
        g_trng.test = locked ? g_trng.test : v;
        break;
    case HUKP_TRNG_CTRL_3:
        g_trng.ctrl3 = g_trng.ctrl3_ro ? g_trng.ctrl3 : v;
        break;
    case HUKP_TRNG_ROSC_C1:
        g_trng.rosc_c1 = locked ? g_trng.rosc_c1 : v & 0x1FFFFFu;
        break;
    case HUKP_SE_PROT_RD:
        break;
    case HUKP_EF_SW_CFG_0:
        g_trng.sw_cfg0 = v;
        g_trng.locked |= (v & SW_SE_DBG_DIS) != 0 && !g_trng.lock_ignored;
        break;
    }
}

int hukp_trng_enable(int en)
{
    if (!en) {
        g_trng.ctrl0 &= ~(CTRL0_EN | CTRL0_HT_ERROR);
        return 0;
    }
    if (g_trng.en_timeout) {
        return -1;
    }
    g_trng.ctrl0 |= CTRL0_EN;
    g_trng.instantiates++;
    return 0;
}

int hukp_trng_generate(uint32_t out[8])
{
    int n = g_trng.gens;

    if ((g_trng.ctrl0 & CTRL0_EN) == 0 || n >= (int)FAKE_GENS ||
        g_trng.fail_gen[n]) {
        return -1;
    }
    g_trng.gens++;
    memcpy(out, g_trng.out[n], 32);
    memcpy(g_trng.dout, g_trng.out[n], 32);
    if (g_trng.ht_error[n]) {
        g_trng.ctrl0 |= CTRL0_HT_ERROR;
    }
    if (g_trng.on_gen != NULL) {
        g_trng.on_gen(n);
    }
    return 0;
}

uint32_t hukp_bclk_mhz(void)
{
    return g_misc.bclk_mhz;
}

int hukp_uart_getc(void)
{
    if (g_misc.rx == NULL || *g_misc.rx == '\0') {
        return -1;
    }
    return (unsigned char)*g_misc.rx++;
}

void hukp_delay_ms(uint32_t ms)
{
    g_misc.ms += ms;
}

int hukp_chip_id(uint8_t id[8])
{
    memcpy(id, g_misc.chip, 8);
    return g_misc.chip_bad ? -1 : 0;
}

const uint8_t *hukp_image(size_t *len)
{
    *len = sizeof(g_misc.image);
    return g_misc.image;
}

void hukp_irq_off(void)
{
    g_misc.irq_off = 1;
}
