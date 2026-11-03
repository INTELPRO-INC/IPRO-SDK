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

#include <string.h>
#include "hal_sec_trng_v2.h"
#include "hal_sec_v2_priv.h"
#include "drv_sec_eng.h"

/* ROSC4-7 (FiGaRO) on before the first generation (IPRO7). */
#define TRNG_V2_ROSC_C1 0x0F00u

static const uint16_t trng_v2_reg_off[HAL_SEC_TRNG_V2_REGS] = {
    [HAL_SEC_TRNG_V2_CTRL_0] = SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET,
    [HAL_SEC_TRNG_V2_TEST] = SEC_ENG_SE_TRNG_0_TEST_OFFSET,
    [HAL_SEC_TRNG_V2_CTRL_3] = SEC_ENG_SE_TRNG_0_CTRL_3_OFFSET,
    [HAL_SEC_TRNG_V2_ROSC_C1] = SEC_ENG_SE_TRNG_0_ROSC_C1_OFFSET,
    [HAL_SEC_TRNG_V2_PROT_RD] = SEC_ENG_SE_CTRL_PROT_RD_OFFSET,
};

int hal_sec_trng_v2_init(void)
{
#ifdef CONFIG_IPRO7
    Sec_Eng_Trng_Set_ROSC_Config(TRNG_V2_ROSC_C1);
#endif
    return Sec_Eng_Trng_Enable() == SUCCESS ? HAL_SEC_V2_OK :
                                              HAL_SEC_V2_ERR_HW;
}

/* ERR_HW, out zeroed, on busy timeout, ht_error or all-zero output */
static int trng_v2_block(uint8_t out[32])
{
    uint8_t any = 0;

    if (Sec_Eng_Trng_Read(out) == SUCCESS &&
        !(sec_v2_rd(SEC_V2_TRNG_CTRL) & SEC_V2_TRNG_HT_ERR)) {
        for (size_t i = 0; i < 32; i++) {
            any |= out[i];
        }
    }
    if (!any) {
        hal_sec_v2_zeroize(out, 32);
        return HAL_SEC_V2_ERR_HW;
    }
    return HAL_SEC_V2_OK;
}

int hal_sec_trng_v2_read(uint8_t *buf, size_t len)
{
    uint8_t tmp[32];
    int ret = HAL_SEC_V2_OK;

    if (!buf && len) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (sec_v2_lock(HAL_SEC_V2_TRNG) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    while (len && ret == HAL_SEC_V2_OK) {
        size_t n = len < sizeof(tmp) ? len : sizeof(tmp);

        ret = trng_v2_block(tmp);
        if (ret != HAL_SEC_V2_OK) {
            /* re-instantiate (clears ht_error), retry once */
            Sec_Eng_Trng_Disable();
            if (hal_sec_trng_v2_init() == HAL_SEC_V2_OK) {
                ret = trng_v2_block(tmp);
            }
        }
        if (ret == HAL_SEC_V2_OK) {
            memcpy(buf, tmp, n);
            buf += n;
            len -= n;
        }
    }
    hal_sec_v2_zeroize(tmp, sizeof(tmp));
    sec_v2_unlock(HAL_SEC_V2_TRNG);
    return ret;
}

int hal_sec_trng_v2_enable(int on)
{
    if (!on) {
        Sec_Eng_Trng_Disable();
        return HAL_SEC_V2_OK;
    }
    return Sec_Eng_Trng_Enable() == SUCCESS ? HAL_SEC_V2_OK :
                                              HAL_SEC_V2_ERR_HW;
}

int hal_sec_trng_v2_generate(uint8_t out[32])
{
    int ret;

    if (!out) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (sec_v2_lock(HAL_SEC_V2_TRNG) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    ret = trng_v2_block(out);
    sec_v2_unlock(HAL_SEC_V2_TRNG);
    return ret;
}

uint32_t hal_sec_trng_v2_reg_read(unsigned int r)
{
    return r < HAL_SEC_TRNG_V2_REGS ? sec_v2_rd(trng_v2_reg_off[r]) : 0;
}

void hal_sec_trng_v2_reg_write(unsigned int r, uint32_t v)
{
    if (r < HAL_SEC_TRNG_V2_REGS) {
        sec_v2_wr(trng_v2_reg_off[r], v);
    }
}
