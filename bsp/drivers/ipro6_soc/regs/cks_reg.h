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

#ifndef  __CKS_REG_H__
#define  __CKS_REG_H__

#include "ipro6.h"

/* 0x0 : cks_config */
#define CKS_CONFIG_OFFSET                                       (0x0)
#define CKS_CR_CKS_CLR                                          CKS_CR_CKS_CLR
#define CKS_CR_CKS_CLR_POS                                      (0U)
#define CKS_CR_CKS_CLR_LEN                                      (1U)
#define CKS_CR_CKS_CLR_MSK                                      (((1ULL<<CKS_CR_CKS_CLR_LEN)-1)<<CKS_CR_CKS_CLR_POS)
#define CKS_CR_CKS_CLR_UMSK                                     (~(((1ULL<<CKS_CR_CKS_CLR_LEN)-1)<<CKS_CR_CKS_CLR_POS))
#define CKS_CR_CKS_BYTE_SWAP                                    CKS_CR_CKS_BYTE_SWAP
#define CKS_CR_CKS_BYTE_SWAP_POS                                (1U)
#define CKS_CR_CKS_BYTE_SWAP_LEN                                (1U)
#define CKS_CR_CKS_BYTE_SWAP_MSK                                (((1ULL<<CKS_CR_CKS_BYTE_SWAP_LEN)-1)<<CKS_CR_CKS_BYTE_SWAP_POS)
#define CKS_CR_CKS_BYTE_SWAP_UMSK                               (~(((1ULL<<CKS_CR_CKS_BYTE_SWAP_LEN)-1)<<CKS_CR_CKS_BYTE_SWAP_POS))

/* 0x4 : data_in */
#define CKS_DATA_IN_OFFSET                                      (0x4)
#define CKS_DATA_IN                                             CKS_DATA_IN
#define CKS_DATA_IN_POS                                         (0U)
#define CKS_DATA_IN_LEN                                         (8U)
#define CKS_DATA_IN_MSK                                         (((1ULL<<CKS_DATA_IN_LEN)-1)<<CKS_DATA_IN_POS)
#define CKS_DATA_IN_UMSK                                        (~(((1ULL<<CKS_DATA_IN_LEN)-1)<<CKS_DATA_IN_POS))

/* 0x8 : cks_out */
#define CKS_OUT_OFFSET                                          (0x8)
#define CKS_OUT                                                 CKS_OUT
#define CKS_OUT_POS                                             (0U)
#define CKS_OUT_LEN                                             (16U)
#define CKS_OUT_MSK                                             (((1ULL<<CKS_OUT_LEN)-1)<<CKS_OUT_POS)
#define CKS_OUT_UMSK                                            (~(((1ULL<<CKS_OUT_LEN)-1)<<CKS_OUT_POS))


struct  cks_reg {
    /* 0x0 : cks_config */
    union {
        struct {
            uint32_t cr_cks_clr                     :  1; /* [    0],        w1c,        0x0 */
            uint32_t cr_cks_byte_swap               :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cks_config;

    /* 0x4 : data_in */
    union {
        struct {
            uint32_t data_in                        :  8; /* [ 7: 0],          w,          x */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } data_in;

    /* 0x8 : cks_out */
    union {
        struct {
            uint32_t cks_out                        : 16; /* [15: 0],          r,     0xffff */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cks_out;

};

typedef volatile struct cks_reg cks_reg_t;


#endif  /* __CKS_REG_H__ */
