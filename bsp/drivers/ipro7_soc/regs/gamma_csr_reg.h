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

#ifndef  __GAMMA_CSR_REG_H__
#define  __GAMMA_CSR_REG_H__

#include "ipro7.h"

/* 0x0 : GAMMA_CSR_0 */
#define GAMMA_CSR_0_OFFSET                                      (0x0)
#define GAMMA_CSR_CR_GAMMA_EN                                   GAMMA_CSR_CR_GAMMA_EN
#define GAMMA_CSR_CR_GAMMA_EN_POS                               (0U)
#define GAMMA_CSR_CR_GAMMA_EN_LEN                               (1U)
#define GAMMA_CSR_CR_GAMMA_EN_MSK                               (((1U<<GAMMA_CSR_CR_GAMMA_EN_LEN)-1)<<GAMMA_CSR_CR_GAMMA_EN_POS)
#define GAMMA_CSR_CR_GAMMA_EN_UMSK                              (~(((1U<<GAMMA_CSR_CR_GAMMA_EN_LEN)-1)<<GAMMA_CSR_CR_GAMMA_EN_POS))
#define GAMMA_CSR_CR_R_GAMMA_256                                GAMMA_CSR_CR_R_GAMMA_256
#define GAMMA_CSR_CR_R_GAMMA_256_POS                            (16U)
#define GAMMA_CSR_CR_R_GAMMA_256_LEN                            (12U)
#define GAMMA_CSR_CR_R_GAMMA_256_MSK                            (((1U<<GAMMA_CSR_CR_R_GAMMA_256_LEN)-1)<<GAMMA_CSR_CR_R_GAMMA_256_POS)
#define GAMMA_CSR_CR_R_GAMMA_256_UMSK                           (~(((1U<<GAMMA_CSR_CR_R_GAMMA_256_LEN)-1)<<GAMMA_CSR_CR_R_GAMMA_256_POS))

/* 0x4 : GAMMA_CSR_ */
#define GAMMA_CSR__OFFSET                                       (0x4)
#define GAMMA_CSR_CR_G_GAMMA_256                                GAMMA_CSR_CR_G_GAMMA_256
#define GAMMA_CSR_CR_G_GAMMA_256_POS                            (0U)
#define GAMMA_CSR_CR_G_GAMMA_256_LEN                            (12U)
#define GAMMA_CSR_CR_G_GAMMA_256_MSK                            (((1U<<GAMMA_CSR_CR_G_GAMMA_256_LEN)-1)<<GAMMA_CSR_CR_G_GAMMA_256_POS)
#define GAMMA_CSR_CR_G_GAMMA_256_UMSK                           (~(((1U<<GAMMA_CSR_CR_G_GAMMA_256_LEN)-1)<<GAMMA_CSR_CR_G_GAMMA_256_POS))
#define GAMMA_CSR_CR_B_GAMMA_256                                GAMMA_CSR_CR_B_GAMMA_256
#define GAMMA_CSR_CR_B_GAMMA_256_POS                            (16U)
#define GAMMA_CSR_CR_B_GAMMA_256_LEN                            (12U)
#define GAMMA_CSR_CR_B_GAMMA_256_MSK                            (((1U<<GAMMA_CSR_CR_B_GAMMA_256_LEN)-1)<<GAMMA_CSR_CR_B_GAMMA_256_POS)
#define GAMMA_CSR_CR_B_GAMMA_256_UMSK                           (~(((1U<<GAMMA_CSR_CR_B_GAMMA_256_LEN)-1)<<GAMMA_CSR_CR_B_GAMMA_256_POS))

/* 0xFC : WDR_W1P */
#define GAMMA_CSR_WDR_W1P_OFFSET                                (0xFC)
#define GAMMA_CSR_CR_SW_SHD                                     GAMMA_CSR_CR_SW_SHD
#define GAMMA_CSR_CR_SW_SHD_POS                                 (0U)
#define GAMMA_CSR_CR_SW_SHD_LEN                                 (1U)
#define GAMMA_CSR_CR_SW_SHD_MSK                                 (((1U<<GAMMA_CSR_CR_SW_SHD_LEN)-1)<<GAMMA_CSR_CR_SW_SHD_POS)
#define GAMMA_CSR_CR_SW_SHD_UMSK                                (~(((1U<<GAMMA_CSR_CR_SW_SHD_LEN)-1)<<GAMMA_CSR_CR_SW_SHD_POS))
#define GAMMA_CSR_CR_SRAM_SHADOW_EN                             GAMMA_CSR_CR_SRAM_SHADOW_EN
#define GAMMA_CSR_CR_SRAM_SHADOW_EN_POS                         (2U)
#define GAMMA_CSR_CR_SRAM_SHADOW_EN_LEN                         (1U)
#define GAMMA_CSR_CR_SRAM_SHADOW_EN_MSK                         (((1U<<GAMMA_CSR_CR_SRAM_SHADOW_EN_LEN)-1)<<GAMMA_CSR_CR_SRAM_SHADOW_EN_POS)
#define GAMMA_CSR_CR_SRAM_SHADOW_EN_UMSK                        (~(((1U<<GAMMA_CSR_CR_SRAM_SHADOW_EN_LEN)-1)<<GAMMA_CSR_CR_SRAM_SHADOW_EN_POS))
#define GAMMA_CSR_STS_SRAM_SHADOW                               GAMMA_CSR_STS_SRAM_SHADOW
#define GAMMA_CSR_STS_SRAM_SHADOW_POS                           (8U)
#define GAMMA_CSR_STS_SRAM_SHADOW_LEN                           (1U)
#define GAMMA_CSR_STS_SRAM_SHADOW_MSK                           (((1U<<GAMMA_CSR_STS_SRAM_SHADOW_LEN)-1)<<GAMMA_CSR_STS_SRAM_SHADOW_POS)
#define GAMMA_CSR_STS_SRAM_SHADOW_UMSK                          (~(((1U<<GAMMA_CSR_STS_SRAM_SHADOW_LEN)-1)<<GAMMA_CSR_STS_SRAM_SHADOW_POS))


struct  gamma_csr_reg {
    /* 0x0 : GAMMA_CSR_0 */
    union {
        struct {
            uint32_t cr_gamma_en                    :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_15                  : 15; /* [15: 1],       rsvd,        0x0 */
            uint32_t cr_r_gamma_256                 : 12; /* [27:16],        r/w,      0xfff */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GAMMA_CSR_0;

    /* 0x4 : GAMMA_CSR_ */
    union {
        struct {
            uint32_t cr_g_gamma_256                 : 12; /* [11: 0],        r/w,      0xfff */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t cr_b_gamma_256                 : 12; /* [27:16],        r/w,      0xfff */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GAMMA_CSR_;

    /* 0x8  reserved */
    uint8_t RESERVED0x8[244];

    /* 0xFC : WDR_W1P */
    union {
        struct {
            uint32_t cr_sw_shd                      :  1; /* [    0],        w1p,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t cr_sram_shadow_en              :  1; /* [    2],        w1p,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t sts_sram_shadow                :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_W1P;

};

typedef volatile struct gamma_csr_reg gamma_csr_reg_t;


#endif  /* __GAMMA_CSR_REG_H__ */
