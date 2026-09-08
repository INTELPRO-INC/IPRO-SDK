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

#include <hal_sec_trng.h>
#include <stdint.h>
#include "hal_sec_common.h"
#include "hal_sec_hw_common.h"
#include "drv_sec_eng.h"
#include "sec_eng_reg.h"
#include <generated/autoconf.h>

/* TRNG ROSC configuration for optimal entropy (NIST SP 800-22 validated) */
#define TRNG_ROSC_C1_VALUE  0x0F00

#define TRNG_SIZE_IN_WORD (8)
#define TRNG_SIZE_IN_BYTES (32)
#define TRNG_BUF_LEFT (TRNG_SIZE_IN_BYTES - trng_idx)
static uint8_t trng_buffer[TRNG_SIZE_IN_BYTES];
static uint8_t trng_idx = 0;

static int trng_refill(void)
{
    uint8_t val = 0;

    if (SUCCESS != Sec_Eng_Trng_Read(trng_buffer))
        return -1;

    for (size_t i = 0; i < TRNG_SIZE_IN_BYTES; ++i)
        val |= trng_buffer[i];
    // assert would be better?
    if (val == 0)
        return 1;

    trng_idx = 0;

    return 0;
}

static int feed_trng_buffer()
{
    int ret = trng_refill();

    if (ret) {
        /* PDS power-cycles SEC_ENG and clears SE_TRNG_0_CTRL_0.EN: measured
         * 0x104 -> 0x000 across a single `lp sleep`. hal_sec_trng_init() only
         * ever runs at boot (system_soc.c), so after the first standby cycle
         * every read returns all zeros and this function fails forever --
         * nothing here reset trng_idx, so the buffer stayed "drained" and each
         * later call failed the same way. wpa_supplicant then cannot derive an
         * SNonce (wpa.c wpa_supplicant_process_1_of_4), and every
         * re-association deauths from 4WAY_HANDSHAKE with reason=1, mislabelled
         * "pre-shared key may be incorrect". Cold boot never saw it because the
         * first association happens before any PDS entry.
         *
         * Re-arm lazily rather than in the PDS resume path: the DTIM standby
         * loop enters PDS thousands of times per session but needs randomness
         * only when it re-associates, and Sec_Eng_Trng_Enable() busy-waits for
         * a generation with interrupts masked. A still-broken engine keeps
         * reporting the failure, and the next call retries from scratch. */
        hal_sec_trng_init();
        ret = trng_refill();
    }

    return ret;
}

int hal_rand_stream(uint8_t *buf, int len)
{
    int ret = 0;
    hal_sec_enter_critical();
    while (len > 0) {
        int left = TRNG_BUF_LEFT;
        int this_len = len;
        if (this_len > left) {
            this_len = left;
        }
        memcpy(buf, &trng_buffer[trng_idx], this_len);
        trng_idx += this_len;
        buf += this_len;
        len -= this_len;
        if (TRNG_BUF_LEFT == 0) {
            if ((ret = feed_trng_buffer())) {
                goto exit;
            }
        }
    }
exit:
    hal_sec_exit_critical(0);

    return ret;
}

int hal_rand()
{
    int val;

    hal_rand_stream((uint8_t *)&val, sizeof(val));
    return val;
}

uint32_t hal_sec_get_random_word(void)
{
    uint32_t val;

    hal_rand_stream((uint8_t *)&val, sizeof(val));
    return val;
}

int hal_sec_trng_init(void)
{
    //Sec_Eng_Group0_Request_Trng_Access();
#ifdef CONFIG_IPRO7
    /* Select ROSC4-7 (FiGaRO) before EN. Sec_Eng_Trng_Enable() runs the
     * first generation, i.e. the DRBG instantiate, so a mask written after
     * it only reaches the entropy source at the next reseed (reseed_n ~ 2^40).
     * Reset default 0xF000 leaves only the legacy ROSC0-3 on. */
    Sec_Eng_Trng_Set_ROSC_Config(TRNG_ROSC_C1_VALUE);
#endif
    Sec_Eng_Trng_Enable();

    trng_idx = TRNG_SIZE_IN_BYTES;
    return 0;
}

#ifdef CONFIG_IPRO7
#define ROSC_XTAL_FREQ_HZ  32000000UL

uint32_t hal_rosc_measure(uint8_t rosc_idx, uint16_t *t_cnt, uint16_t *g_cnt)
{
    uint32_t tmpVal;
    uint16_t t_count, g_count;

    if (rosc_idx > 3)
        return 0;

    SEC_Eng_Turn_On_Sec_Ring();
    Sec_Eng_Group0_Request_CDET_Access();

    /* CTRL_0: clear EN, set MODE=1 (manual mode) */
    tmpVal = readl(SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SEC_ENG_SE_CDET_0_EN);
    tmpVal = reg_set_bit(tmpVal, SEC_ENG_SE_CDET_0_MODE);
    writel(tmpVal, SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);

    /* CTRL_1: g_loop_min=3, g_loop_max=0xFFFF (ATE values) */
    writel((3U << 16) | 0xFFFF,
           SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_1_OFFSET);

    /* CTRL_2: g_slp_n=0xFF, t_dly_n=3, t_loop_n=0x3EB (ATE values) */
    writel((0xFFU << 24) | (3U << 16) | 0x3EB,
           SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_2_OFFSET);

    /* Gold=XTAL(1), Test=ROSC0..3(4+rosc_idx) */
    Sec_Eng_CDET_Set_Clk_Sel(1, 4 + rosc_idx);

    Sec_Eng_CDET_Start();
    Sec_Eng_CDET_Get_Count(&t_count, &g_count);

    Sec_Eng_Group0_Release_CDET_Access();
    SEC_Eng_Turn_Off_Sec_Ring();

    if (t_cnt)
        *t_cnt = t_count;
    if (g_cnt)
        *g_cnt = g_count;

    if (g_count == 0)
        return 0;

    return (uint32_t)(((uint64_t)ROSC_XTAL_FREQ_HZ * t_count) / (g_count + 1));
}
#endif /* CONFIG_IPRO7 */
