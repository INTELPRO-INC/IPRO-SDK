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

#ifndef  __M154_AES_REG_H__
#define  __M154_AES_REG_H__

#include "ipro6.h"

/* 0x800 : AES control */
#define M154_AES_ACR_OFFSET                                     (0x800)
#define M154_AES_AESDMA_TRIG                                    M154_AES_AESDMA_TRIG
#define M154_AES_AESDMA_TRIG_POS                                (0U)
#define M154_AES_AESDMA_TRIG_LEN                                (1U)
#define M154_AES_AESDMA_TRIG_MSK                                (((1ULL<<M154_AES_AESDMA_TRIG_LEN)-1)<<M154_AES_AESDMA_TRIG_POS)
#define M154_AES_AESDMA_TRIG_UMSK                               (~(((1ULL<<M154_AES_AESDMA_TRIG_LEN)-1)<<M154_AES_AESDMA_TRIG_POS))
#define M154_AES_AES_DMA_MODE                                   M154_AES_AES_DMA_MODE
#define M154_AES_AES_DMA_MODE_POS                               (4U)
#define M154_AES_AES_DMA_MODE_LEN                               (2U)
#define M154_AES_AES_DMA_MODE_MSK                               (((1ULL<<M154_AES_AES_DMA_MODE_LEN)-1)<<M154_AES_AES_DMA_MODE_POS)
#define M154_AES_AES_DMA_MODE_UMSK                              (~(((1ULL<<M154_AES_AES_DMA_MODE_LEN)-1)<<M154_AES_AES_DMA_MODE_POS))
#define M154_AES_RXKEY_RDY                                      M154_AES_RXKEY_RDY
#define M154_AES_RXKEY_RDY_POS                                  (8U)
#define M154_AES_RXKEY_RDY_LEN                                  (1U)
#define M154_AES_RXKEY_RDY_MSK                                  (((1ULL<<M154_AES_RXKEY_RDY_LEN)-1)<<M154_AES_RXKEY_RDY_POS)
#define M154_AES_RXKEY_RDY_UMSK                                 (~(((1ULL<<M154_AES_RXKEY_RDY_LEN)-1)<<M154_AES_RXKEY_RDY_POS))
#define M154_AES_SW_RST_AES                                     M154_AES_SW_RST_AES
#define M154_AES_SW_RST_AES_POS                                 (16U)
#define M154_AES_SW_RST_AES_LEN                                 (1U)
#define M154_AES_SW_RST_AES_MSK                                 (((1ULL<<M154_AES_SW_RST_AES_LEN)-1)<<M154_AES_SW_RST_AES_POS)
#define M154_AES_SW_RST_AES_UMSK                                (~(((1ULL<<M154_AES_SW_RST_AES_LEN)-1)<<M154_AES_SW_RST_AES_POS))
#define M154_AES_SW_RST_MST                                     M154_AES_SW_RST_MST
#define M154_AES_SW_RST_MST_POS                                 (17U)
#define M154_AES_SW_RST_MST_LEN                                 (1U)
#define M154_AES_SW_RST_MST_MSK                                 (((1ULL<<M154_AES_SW_RST_MST_LEN)-1)<<M154_AES_SW_RST_MST_POS)
#define M154_AES_SW_RST_MST_UMSK                                (~(((1ULL<<M154_AES_SW_RST_MST_LEN)-1)<<M154_AES_SW_RST_MST_POS))

/* 0x804 : AES mode */
#define M154_AES_AMR_OFFSET                                     (0x804)
#define M154_AES_AES_ENABLE                                     M154_AES_AES_ENABLE
#define M154_AES_AES_ENABLE_POS                                 (0U)
#define M154_AES_AES_ENABLE_LEN                                 (1U)
#define M154_AES_AES_ENABLE_MSK                                 (((1ULL<<M154_AES_AES_ENABLE_LEN)-1)<<M154_AES_AES_ENABLE_POS)
#define M154_AES_AES_ENABLE_UMSK                                (~(((1ULL<<M154_AES_AES_ENABLE_LEN)-1)<<M154_AES_AES_ENABLE_POS))
#define M154_AES_RXDMA_EN                                       M154_AES_RXDMA_EN
#define M154_AES_RXDMA_EN_POS                                   (1U)
#define M154_AES_RXDMA_EN_LEN                                   (1U)
#define M154_AES_RXDMA_EN_MSK                                   (((1ULL<<M154_AES_RXDMA_EN_LEN)-1)<<M154_AES_RXDMA_EN_POS)
#define M154_AES_RXDMA_EN_UMSK                                  (~(((1ULL<<M154_AES_RXDMA_EN_LEN)-1)<<M154_AES_RXDMA_EN_POS))
#define M154_AES_RXDEC_EN                                       M154_AES_RXDEC_EN
#define M154_AES_RXDEC_EN_POS                                   (2U)
#define M154_AES_RXDEC_EN_LEN                                   (1U)
#define M154_AES_RXDEC_EN_MSK                                   (((1ULL<<M154_AES_RXDEC_EN_LEN)-1)<<M154_AES_RXDEC_EN_POS)
#define M154_AES_RXDEC_EN_UMSK                                  (~(((1ULL<<M154_AES_RXDEC_EN_LEN)-1)<<M154_AES_RXDEC_EN_POS))
#define M154_AES_AHB_BYTEWR_EN                                  M154_AES_AHB_BYTEWR_EN
#define M154_AES_AHB_BYTEWR_EN_POS                              (3U)
#define M154_AES_AHB_BYTEWR_EN_LEN                              (1U)
#define M154_AES_AHB_BYTEWR_EN_MSK                              (((1ULL<<M154_AES_AHB_BYTEWR_EN_LEN)-1)<<M154_AES_AHB_BYTEWR_EN_POS)
#define M154_AES_AHB_BYTEWR_EN_UMSK                             (~(((1ULL<<M154_AES_AHB_BYTEWR_EN_LEN)-1)<<M154_AES_AHB_BYTEWR_EN_POS))
#define M154_AES_AES_MODE                                       M154_AES_AES_MODE
#define M154_AES_AES_MODE_POS                                   (4U)
#define M154_AES_AES_MODE_LEN                                   (2U)
#define M154_AES_AES_MODE_MSK                                   (((1ULL<<M154_AES_AES_MODE_LEN)-1)<<M154_AES_AES_MODE_POS)
#define M154_AES_AES_MODE_UMSK                                  (~(((1ULL<<M154_AES_AES_MODE_LEN)-1)<<M154_AES_AES_MODE_POS))
#define M154_AES_AES_KEY_SIZE                                   M154_AES_AES_KEY_SIZE
#define M154_AES_AES_KEY_SIZE_POS                               (6U)
#define M154_AES_AES_KEY_SIZE_LEN                               (2U)
#define M154_AES_AES_KEY_SIZE_MSK                               (((1ULL<<M154_AES_AES_KEY_SIZE_LEN)-1)<<M154_AES_AES_KEY_SIZE_POS)
#define M154_AES_AES_KEY_SIZE_UMSK                              (~(((1ULL<<M154_AES_AES_KEY_SIZE_LEN)-1)<<M154_AES_AES_KEY_SIZE_POS))

/* 0x808 : AES length */
#define M154_AES_ALR_OFFSET                                     (0x808)
#define M154_AES_AES_M_LEN                                      M154_AES_AES_M_LEN
#define M154_AES_AES_M_LEN_POS                                  (0U)
#define M154_AES_AES_M_LEN_LEN                                  (11U)
#define M154_AES_AES_M_LEN_MSK                                  (((1ULL<<M154_AES_AES_M_LEN_LEN)-1)<<M154_AES_AES_M_LEN_POS)
#define M154_AES_AES_M_LEN_UMSK                                 (~(((1ULL<<M154_AES_AES_M_LEN_LEN)-1)<<M154_AES_AES_M_LEN_POS))
#define M154_AES_AES_A_LEN                                      M154_AES_AES_A_LEN
#define M154_AES_AES_A_LEN_POS                                  (12U)
#define M154_AES_AES_A_LEN_LEN                                  (11U)
#define M154_AES_AES_A_LEN_MSK                                  (((1ULL<<M154_AES_AES_A_LEN_LEN)-1)<<M154_AES_AES_A_LEN_POS)
#define M154_AES_AES_A_LEN_UMSK                                 (~(((1ULL<<M154_AES_AES_A_LEN_LEN)-1)<<M154_AES_AES_A_LEN_POS))
#define M154_AES_AES_NONCE_LEN                                  M154_AES_AES_NONCE_LEN
#define M154_AES_AES_NONCE_LEN_POS                              (24U)
#define M154_AES_AES_NONCE_LEN_LEN                              (4U)
#define M154_AES_AES_NONCE_LEN_MSK                              (((1ULL<<M154_AES_AES_NONCE_LEN_LEN)-1)<<M154_AES_AES_NONCE_LEN_POS)
#define M154_AES_AES_NONCE_LEN_UMSK                             (~(((1ULL<<M154_AES_AES_NONCE_LEN_LEN)-1)<<M154_AES_AES_NONCE_LEN_POS))
#define M154_AES_AES_MIC_LEN                                    M154_AES_AES_MIC_LEN
#define M154_AES_AES_MIC_LEN_POS                                (28U)
#define M154_AES_AES_MIC_LEN_LEN                                (2U)
#define M154_AES_AES_MIC_LEN_MSK                                (((1ULL<<M154_AES_AES_MIC_LEN_LEN)-1)<<M154_AES_AES_MIC_LEN_POS)
#define M154_AES_AES_MIC_LEN_UMSK                               (~(((1ULL<<M154_AES_AES_MIC_LEN_LEN)-1)<<M154_AES_AES_MIC_LEN_POS))

/* 0x80C : AES RX length */
#define M154_AES_RXALR_OFFSET                                   (0x80C)
#define M154_AES_RX_M_LEN_R                                     M154_AES_RX_M_LEN_R
#define M154_AES_RX_M_LEN_R_POS                                 (0U)
#define M154_AES_RX_M_LEN_R_LEN                                 (8U)
#define M154_AES_RX_M_LEN_R_MSK                                 (((1ULL<<M154_AES_RX_M_LEN_R_LEN)-1)<<M154_AES_RX_M_LEN_R_POS)
#define M154_AES_RX_M_LEN_R_UMSK                                (~(((1ULL<<M154_AES_RX_M_LEN_R_LEN)-1)<<M154_AES_RX_M_LEN_R_POS))
#define M154_AES_RX_A_LEN_R                                     M154_AES_RX_A_LEN_R
#define M154_AES_RX_A_LEN_R_POS                                 (12U)
#define M154_AES_RX_A_LEN_R_LEN                                 (8U)
#define M154_AES_RX_A_LEN_R_MSK                                 (((1ULL<<M154_AES_RX_A_LEN_R_LEN)-1)<<M154_AES_RX_A_LEN_R_POS)
#define M154_AES_RX_A_LEN_R_UMSK                                (~(((1ULL<<M154_AES_RX_A_LEN_R_LEN)-1)<<M154_AES_RX_A_LEN_R_POS))
#define M154_AES_RX_MIC_LEN_R                                   M154_AES_RX_MIC_LEN_R
#define M154_AES_RX_MIC_LEN_R_POS                               (28U)
#define M154_AES_RX_MIC_LEN_R_LEN                               (2U)
#define M154_AES_RX_MIC_LEN_R_MSK                               (((1ULL<<M154_AES_RX_MIC_LEN_R_LEN)-1)<<M154_AES_RX_MIC_LEN_R_POS)
#define M154_AES_RX_MIC_LEN_R_UMSK                              (~(((1ULL<<M154_AES_RX_MIC_LEN_R_LEN)-1)<<M154_AES_RX_MIC_LEN_R_POS))

/* 0x810 : AES a data source address */
#define M154_AES_AASAR_OFFSET                                   (0x810)
#define M154_AES_AES_ASA                                        M154_AES_AES_ASA
#define M154_AES_AES_ASA_POS                                    (0U)
#define M154_AES_AES_ASA_LEN                                    (32U)
#define M154_AES_AES_ASA_MSK                                    (((1ULL<<M154_AES_AES_ASA_LEN)-1)<<M154_AES_AES_ASA_POS)
#define M154_AES_AES_ASA_UMSK                                   (~(((1ULL<<M154_AES_AES_ASA_LEN)-1)<<M154_AES_AES_ASA_POS))

/* 0x814 : AES m data source address */
#define M154_AES_AMSAR_OFFSET                                   (0x814)
#define M154_AES_AES_MSA                                        M154_AES_AES_MSA
#define M154_AES_AES_MSA_POS                                    (0U)
#define M154_AES_AES_MSA_LEN                                    (32U)
#define M154_AES_AES_MSA_MSK                                    (((1ULL<<M154_AES_AES_MSA_LEN)-1)<<M154_AES_AES_MSA_POS)
#define M154_AES_AES_MSA_UMSK                                   (~(((1ULL<<M154_AES_AES_MSA_LEN)-1)<<M154_AES_AES_MSA_POS))

/* 0x818 : AES m data destination address */
#define M154_AES_AMDAR_OFFSET                                   (0x818)
#define M154_AES_AES_MDA                                        M154_AES_AES_MDA
#define M154_AES_AES_MDA_POS                                    (0U)
#define M154_AES_AES_MDA_LEN                                    (32U)
#define M154_AES_AES_MDA_MSK                                    (((1ULL<<M154_AES_AES_MDA_LEN)-1)<<M154_AES_AES_MDA_POS)
#define M154_AES_AES_MDA_UMSK                                   (~(((1ULL<<M154_AES_AES_MDA_LEN)-1)<<M154_AES_AES_MDA_POS))

/* 0x81C : AES RX a data destination address */
#define M154_AES_RXAADAR_OFFSET                                 (0x81C)
#define M154_AES_AES_RXADA                                      M154_AES_AES_RXADA
#define M154_AES_AES_RXADA_POS                                  (0U)
#define M154_AES_AES_RXADA_LEN                                  (32U)
#define M154_AES_AES_RXADA_MSK                                  (((1ULL<<M154_AES_AES_RXADA_LEN)-1)<<M154_AES_AES_RXADA_POS)
#define M154_AES_AES_RXADA_UMSK                                 (~(((1ULL<<M154_AES_AES_RXADA_LEN)-1)<<M154_AES_AES_RXADA_POS))

/* 0x820 : AES RX m data destination address */
#define M154_AES_RXAMDAR_OFFSET                                 (0x820)
#define M154_AES_AES_RXMDA                                      M154_AES_AES_RXMDA
#define M154_AES_AES_RXMDA_POS                                  (0U)
#define M154_AES_AES_RXMDA_LEN                                  (32U)
#define M154_AES_AES_RXMDA_MSK                                  (((1ULL<<M154_AES_AES_RXMDA_LEN)-1)<<M154_AES_AES_RXMDA_POS)
#define M154_AES_AES_RXMDA_UMSK                                 (~(((1ULL<<M154_AES_AES_RXMDA_LEN)-1)<<M154_AES_AES_RXMDA_POS))

/* 0x830 : AES nonce */
#define M154_AES_ANR0_OFFSET                                    (0x830)
#define M154_AES_ANR0                                           M154_AES_ANR0
#define M154_AES_ANR0_POS                                       (0U)
#define M154_AES_ANR0_LEN                                       (32U)
#define M154_AES_ANR0_MSK                                       (((1ULL<<M154_AES_ANR0_LEN)-1)<<M154_AES_ANR0_POS)
#define M154_AES_ANR0_UMSK                                      (~(((1ULL<<M154_AES_ANR0_LEN)-1)<<M154_AES_ANR0_POS))

/* 0x834 : m154_aes_anr1 */
#define M154_AES_ANR1_OFFSET                                    (0x834)
#define M154_AES_ANR1                                           M154_AES_ANR1
#define M154_AES_ANR1_POS                                       (0U)
#define M154_AES_ANR1_LEN                                       (32U)
#define M154_AES_ANR1_MSK                                       (((1ULL<<M154_AES_ANR1_LEN)-1)<<M154_AES_ANR1_POS)
#define M154_AES_ANR1_UMSK                                      (~(((1ULL<<M154_AES_ANR1_LEN)-1)<<M154_AES_ANR1_POS))

/* 0x838 : m154_aes_anr2 */
#define M154_AES_ANR2_OFFSET                                    (0x838)
#define M154_AES_ANR2                                           M154_AES_ANR2
#define M154_AES_ANR2_POS                                       (0U)
#define M154_AES_ANR2_LEN                                       (32U)
#define M154_AES_ANR2_MSK                                       (((1ULL<<M154_AES_ANR2_LEN)-1)<<M154_AES_ANR2_POS)
#define M154_AES_ANR2_UMSK                                      (~(((1ULL<<M154_AES_ANR2_LEN)-1)<<M154_AES_ANR2_POS))

/* 0x83C : m154_aes_anr3 */
#define M154_AES_ANR3_OFFSET                                    (0x83C)
#define M154_AES_ANR3                                           M154_AES_ANR3
#define M154_AES_ANR3_POS                                       (0U)
#define M154_AES_ANR3_LEN                                       (16U)
#define M154_AES_ANR3_MSK                                       (((1ULL<<M154_AES_ANR3_LEN)-1)<<M154_AES_ANR3_POS)
#define M154_AES_ANR3_UMSK                                      (~(((1ULL<<M154_AES_ANR3_LEN)-1)<<M154_AES_ANR3_POS))

/* 0x840 : AES key */
#define M154_AES_AKR0_OFFSET                                    (0x840)
#define M154_AES_AKR0                                           M154_AES_AKR0
#define M154_AES_AKR0_POS                                       (0U)
#define M154_AES_AKR0_LEN                                       (32U)
#define M154_AES_AKR0_MSK                                       (((1ULL<<M154_AES_AKR0_LEN)-1)<<M154_AES_AKR0_POS)
#define M154_AES_AKR0_UMSK                                      (~(((1ULL<<M154_AES_AKR0_LEN)-1)<<M154_AES_AKR0_POS))

/* 0x844 : m154_aes_akr1 */
#define M154_AES_AKR1_OFFSET                                    (0x844)
#define M154_AES_AKR1                                           M154_AES_AKR1
#define M154_AES_AKR1_POS                                       (0U)
#define M154_AES_AKR1_LEN                                       (32U)
#define M154_AES_AKR1_MSK                                       (((1ULL<<M154_AES_AKR1_LEN)-1)<<M154_AES_AKR1_POS)
#define M154_AES_AKR1_UMSK                                      (~(((1ULL<<M154_AES_AKR1_LEN)-1)<<M154_AES_AKR1_POS))

/* 0x848 : m154_aes_akr2 */
#define M154_AES_AKR2_OFFSET                                    (0x848)
#define M154_AES_AKR2                                           M154_AES_AKR2
#define M154_AES_AKR2_POS                                       (0U)
#define M154_AES_AKR2_LEN                                       (32U)
#define M154_AES_AKR2_MSK                                       (((1ULL<<M154_AES_AKR2_LEN)-1)<<M154_AES_AKR2_POS)
#define M154_AES_AKR2_UMSK                                      (~(((1ULL<<M154_AES_AKR2_LEN)-1)<<M154_AES_AKR2_POS))

/* 0x84C : m154_aes_akr3 */
#define M154_AES_AKR3_OFFSET                                    (0x84C)
#define M154_AES_AKR3                                           M154_AES_AKR3
#define M154_AES_AKR3_POS                                       (0U)
#define M154_AES_AKR3_LEN                                       (32U)
#define M154_AES_AKR3_MSK                                       (((1ULL<<M154_AES_AKR3_LEN)-1)<<M154_AES_AKR3_POS)
#define M154_AES_AKR3_UMSK                                      (~(((1ULL<<M154_AES_AKR3_LEN)-1)<<M154_AES_AKR3_POS))

/* 0x850 : m154_aes_akr4 */
#define M154_AES_AKR4_OFFSET                                    (0x850)
#define M154_AES_AKR4                                           M154_AES_AKR4
#define M154_AES_AKR4_POS                                       (0U)
#define M154_AES_AKR4_LEN                                       (32U)
#define M154_AES_AKR4_MSK                                       (((1ULL<<M154_AES_AKR4_LEN)-1)<<M154_AES_AKR4_POS)
#define M154_AES_AKR4_UMSK                                      (~(((1ULL<<M154_AES_AKR4_LEN)-1)<<M154_AES_AKR4_POS))

/* 0x854 : m154_aes_akr5 */
#define M154_AES_AKR5_OFFSET                                    (0x854)
#define M154_AES_AKR5                                           M154_AES_AKR5
#define M154_AES_AKR5_POS                                       (0U)
#define M154_AES_AKR5_LEN                                       (32U)
#define M154_AES_AKR5_MSK                                       (((1ULL<<M154_AES_AKR5_LEN)-1)<<M154_AES_AKR5_POS)
#define M154_AES_AKR5_UMSK                                      (~(((1ULL<<M154_AES_AKR5_LEN)-1)<<M154_AES_AKR5_POS))

/* 0x858 : m154_aes_akr6 */
#define M154_AES_AKR6_OFFSET                                    (0x858)
#define M154_AES_AKR6                                           M154_AES_AKR6
#define M154_AES_AKR6_POS                                       (0U)
#define M154_AES_AKR6_LEN                                       (32U)
#define M154_AES_AKR6_MSK                                       (((1ULL<<M154_AES_AKR6_LEN)-1)<<M154_AES_AKR6_POS)
#define M154_AES_AKR6_UMSK                                      (~(((1ULL<<M154_AES_AKR6_LEN)-1)<<M154_AES_AKR6_POS))

/* 0x85C : m154_aes_akr7 */
#define M154_AES_AKR7_OFFSET                                    (0x85C)
#define M154_AES_AKR7                                           M154_AES_AKR7
#define M154_AES_AKR7_POS                                       (0U)
#define M154_AES_AKR7_LEN                                       (32U)
#define M154_AES_AKR7_MSK                                       (((1ULL<<M154_AES_AKR7_LEN)-1)<<M154_AES_AKR7_POS)
#define M154_AES_AKR7_UMSK                                      (~(((1ULL<<M154_AES_AKR7_LEN)-1)<<M154_AES_AKR7_POS))

/* 0x860 : AES mic */
#define M154_AES_AMICR0_OFFSET                                  (0x860)
#define M154_AES_AMICR0                                         M154_AES_AMICR0
#define M154_AES_AMICR0_POS                                     (0U)
#define M154_AES_AMICR0_LEN                                     (32U)
#define M154_AES_AMICR0_MSK                                     (((1ULL<<M154_AES_AMICR0_LEN)-1)<<M154_AES_AMICR0_POS)
#define M154_AES_AMICR0_UMSK                                    (~(((1ULL<<M154_AES_AMICR0_LEN)-1)<<M154_AES_AMICR0_POS))

/* 0x864 : m154_aes_amicr1 */
#define M154_AES_AMICR1_OFFSET                                  (0x864)
#define M154_AES_AMICR1                                         M154_AES_AMICR1
#define M154_AES_AMICR1_POS                                     (0U)
#define M154_AES_AMICR1_LEN                                     (32U)
#define M154_AES_AMICR1_MSK                                     (((1ULL<<M154_AES_AMICR1_LEN)-1)<<M154_AES_AMICR1_POS)
#define M154_AES_AMICR1_UMSK                                    (~(((1ULL<<M154_AES_AMICR1_LEN)-1)<<M154_AES_AMICR1_POS))

/* 0x868 : m154_aes_amicr2 */
#define M154_AES_AMICR2_OFFSET                                  (0x868)
#define M154_AES_AMICR2                                         M154_AES_AMICR2
#define M154_AES_AMICR2_POS                                     (0U)
#define M154_AES_AMICR2_LEN                                     (32U)
#define M154_AES_AMICR2_MSK                                     (((1ULL<<M154_AES_AMICR2_LEN)-1)<<M154_AES_AMICR2_POS)
#define M154_AES_AMICR2_UMSK                                    (~(((1ULL<<M154_AES_AMICR2_LEN)-1)<<M154_AES_AMICR2_POS))

/* 0x86C : m154_aes_amicr3 */
#define M154_AES_AMICR3_OFFSET                                  (0x86C)
#define M154_AES_AMICR3                                         M154_AES_AMICR3
#define M154_AES_AMICR3_POS                                     (0U)
#define M154_AES_AMICR3_LEN                                     (32U)
#define M154_AES_AMICR3_MSK                                     (((1ULL<<M154_AES_AMICR3_LEN)-1)<<M154_AES_AMICR3_POS)
#define M154_AES_AMICR3_UMSK                                    (~(((1ULL<<M154_AES_AMICR3_LEN)-1)<<M154_AES_AMICR3_POS))

/* 0x870 : AES status 1 */
#define M154_AES_STATUS1_OFFSET                                 (0x870)
#define M154_AES_AES_BUSY                                       M154_AES_AES_BUSY
#define M154_AES_AES_BUSY_POS                                   (0U)
#define M154_AES_AES_BUSY_LEN                                   (1U)
#define M154_AES_AES_BUSY_MSK                                   (((1ULL<<M154_AES_AES_BUSY_LEN)-1)<<M154_AES_AES_BUSY_POS)
#define M154_AES_AES_BUSY_UMSK                                  (~(((1ULL<<M154_AES_AES_BUSY_LEN)-1)<<M154_AES_AES_BUSY_POS))
#define M154_AES_MIC_ERROR                                      M154_AES_MIC_ERROR
#define M154_AES_MIC_ERROR_POS                                  (1U)
#define M154_AES_MIC_ERROR_LEN                                  (1U)
#define M154_AES_MIC_ERROR_MSK                                  (((1ULL<<M154_AES_MIC_ERROR_LEN)-1)<<M154_AES_MIC_ERROR_POS)
#define M154_AES_MIC_ERROR_UMSK                                 (~(((1ULL<<M154_AES_MIC_ERROR_LEN)-1)<<M154_AES_MIC_ERROR_POS))
#define M154_AES_AES_MST_CS                                     M154_AES_AES_MST_CS
#define M154_AES_AES_MST_CS_POS                                 (4U)
#define M154_AES_AES_MST_CS_LEN                                 (4U)
#define M154_AES_AES_MST_CS_MSK                                 (((1ULL<<M154_AES_AES_MST_CS_LEN)-1)<<M154_AES_AES_MST_CS_POS)
#define M154_AES_AES_MST_CS_UMSK                                (~(((1ULL<<M154_AES_AES_MST_CS_LEN)-1)<<M154_AES_AES_MST_CS_POS))
#define M154_AES_AES_CTRL_CS                                    M154_AES_AES_CTRL_CS
#define M154_AES_AES_CTRL_CS_POS                                (8U)
#define M154_AES_AES_CTRL_CS_LEN                                (4U)
#define M154_AES_AES_CTRL_CS_MSK                                (((1ULL<<M154_AES_AES_CTRL_CS_LEN)-1)<<M154_AES_AES_CTRL_CS_POS)
#define M154_AES_AES_CTRL_CS_UMSK                               (~(((1ULL<<M154_AES_AES_CTRL_CS_LEN)-1)<<M154_AES_AES_CTRL_CS_POS))
#define M154_AES_DMA_CTRL_CS                                    M154_AES_DMA_CTRL_CS
#define M154_AES_DMA_CTRL_CS_POS                                (12U)
#define M154_AES_DMA_CTRL_CS_LEN                                (4U)
#define M154_AES_DMA_CTRL_CS_MSK                                (((1ULL<<M154_AES_DMA_CTRL_CS_LEN)-1)<<M154_AES_DMA_CTRL_CS_POS)
#define M154_AES_DMA_CTRL_CS_UMSK                               (~(((1ULL<<M154_AES_DMA_CTRL_CS_LEN)-1)<<M154_AES_DMA_CTRL_CS_POS))
#define M154_AES_LAST_M_BLK                                     M154_AES_LAST_M_BLK
#define M154_AES_LAST_M_BLK_POS                                 (16U)
#define M154_AES_LAST_M_BLK_LEN                                 (1U)
#define M154_AES_LAST_M_BLK_MSK                                 (((1ULL<<M154_AES_LAST_M_BLK_LEN)-1)<<M154_AES_LAST_M_BLK_POS)
#define M154_AES_LAST_M_BLK_UMSK                                (~(((1ULL<<M154_AES_LAST_M_BLK_LEN)-1)<<M154_AES_LAST_M_BLK_POS))
#define M154_AES_LAST_A_BLK                                     M154_AES_LAST_A_BLK
#define M154_AES_LAST_A_BLK_POS                                 (17U)
#define M154_AES_LAST_A_BLK_LEN                                 (1U)
#define M154_AES_LAST_A_BLK_MSK                                 (((1ULL<<M154_AES_LAST_A_BLK_LEN)-1)<<M154_AES_LAST_A_BLK_POS)
#define M154_AES_LAST_A_BLK_UMSK                                (~(((1ULL<<M154_AES_LAST_A_BLK_LEN)-1)<<M154_AES_LAST_A_BLK_POS))
#define M154_AES_FIRST_M_BLK                                    M154_AES_FIRST_M_BLK
#define M154_AES_FIRST_M_BLK_POS                                (18U)
#define M154_AES_FIRST_M_BLK_LEN                                (1U)
#define M154_AES_FIRST_M_BLK_MSK                                (((1ULL<<M154_AES_FIRST_M_BLK_LEN)-1)<<M154_AES_FIRST_M_BLK_POS)
#define M154_AES_FIRST_M_BLK_UMSK                               (~(((1ULL<<M154_AES_FIRST_M_BLK_LEN)-1)<<M154_AES_FIRST_M_BLK_POS))
#define M154_AES_FIRST_A_BLK                                    M154_AES_FIRST_A_BLK
#define M154_AES_FIRST_A_BLK_POS                                (19U)
#define M154_AES_FIRST_A_BLK_LEN                                (1U)
#define M154_AES_FIRST_A_BLK_MSK                                (((1ULL<<M154_AES_FIRST_A_BLK_LEN)-1)<<M154_AES_FIRST_A_BLK_POS)
#define M154_AES_FIRST_A_BLK_UMSK                               (~(((1ULL<<M154_AES_FIRST_A_BLK_LEN)-1)<<M154_AES_FIRST_A_BLK_POS))
#define M154_AES_TX_MPDU_AES_DDT_DONE                           M154_AES_TX_MPDU_AES_DDT_DONE
#define M154_AES_TX_MPDU_AES_DDT_DONE_POS                       (20U)
#define M154_AES_TX_MPDU_AES_DDT_DONE_LEN                       (1U)
#define M154_AES_TX_MPDU_AES_DDT_DONE_MSK                       (((1ULL<<M154_AES_TX_MPDU_AES_DDT_DONE_LEN)-1)<<M154_AES_TX_MPDU_AES_DDT_DONE_POS)
#define M154_AES_TX_MPDU_AES_DDT_DONE_UMSK                      (~(((1ULL<<M154_AES_TX_MPDU_AES_DDT_DONE_LEN)-1)<<M154_AES_TX_MPDU_AES_DDT_DONE_POS))

/* 0x874 : AES status 2 */
#define M154_AES_STATUS2_OFFSET                                 (0x874)
#define M154_AES_U_CTR_CNT                                      M154_AES_U_CTR_CNT
#define M154_AES_U_CTR_CNT_POS                                  (0U)
#define M154_AES_U_CTR_CNT_LEN                                  (16U)
#define M154_AES_U_CTR_CNT_MSK                                  (((1ULL<<M154_AES_U_CTR_CNT_LEN)-1)<<M154_AES_U_CTR_CNT_POS)
#define M154_AES_U_CTR_CNT_UMSK                                 (~(((1ULL<<M154_AES_U_CTR_CNT_LEN)-1)<<M154_AES_U_CTR_CNT_POS))
#define M154_AES_U_C_CNT                                        M154_AES_U_C_CNT
#define M154_AES_U_C_CNT_POS                                    (16U)
#define M154_AES_U_C_CNT_LEN                                    (9U)
#define M154_AES_U_C_CNT_MSK                                    (((1ULL<<M154_AES_U_C_CNT_LEN)-1)<<M154_AES_U_C_CNT_POS)
#define M154_AES_U_C_CNT_UMSK                                   (~(((1ULL<<M154_AES_U_C_CNT_LEN)-1)<<M154_AES_U_C_CNT_POS))

/* 0x878 : AES status 3 */
#define M154_AES_STATUS3_OFFSET                                 (0x878)
#define M154_AES_U_AM_CNT                                       M154_AES_U_AM_CNT
#define M154_AES_U_AM_CNT_POS                                   (0U)
#define M154_AES_U_AM_CNT_LEN                                   (10U)
#define M154_AES_U_AM_CNT_MSK                                   (((1ULL<<M154_AES_U_AM_CNT_LEN)-1)<<M154_AES_U_AM_CNT_POS)
#define M154_AES_U_AM_CNT_UMSK                                  (~(((1ULL<<M154_AES_U_AM_CNT_LEN)-1)<<M154_AES_U_AM_CNT_POS))
#define M154_AES_U_CNT                                          M154_AES_U_CNT
#define M154_AES_U_CNT_POS                                      (12U)
#define M154_AES_U_CNT_LEN                                      (2U)
#define M154_AES_U_CNT_MSK                                      (((1ULL<<M154_AES_U_CNT_LEN)-1)<<M154_AES_U_CNT_POS)
#define M154_AES_U_CNT_UMSK                                     (~(((1ULL<<M154_AES_U_CNT_LEN)-1)<<M154_AES_U_CNT_POS))

/* 0x880 : Interrupt enable */
#define M154_AES_IER_OFFSET                                     (0x880)
#define M154_AES_AESENC_DONE_FW_INT_EN                          M154_AES_AESENC_DONE_FW_INT_EN
#define M154_AES_AESENC_DONE_FW_INT_EN_POS                      (0U)
#define M154_AES_AESENC_DONE_FW_INT_EN_LEN                      (1U)
#define M154_AES_AESENC_DONE_FW_INT_EN_MSK                      (((1ULL<<M154_AES_AESENC_DONE_FW_INT_EN_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_EN_POS)
#define M154_AES_AESENC_DONE_FW_INT_EN_UMSK                     (~(((1ULL<<M154_AES_AESENC_DONE_FW_INT_EN_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_EN_POS))
#define M154_AES_AESDEC_DONE_FW_INT_EN                          M154_AES_AESDEC_DONE_FW_INT_EN
#define M154_AES_AESDEC_DONE_FW_INT_EN_POS                      (1U)
#define M154_AES_AESDEC_DONE_FW_INT_EN_LEN                      (1U)
#define M154_AES_AESDEC_DONE_FW_INT_EN_MSK                      (((1ULL<<M154_AES_AESDEC_DONE_FW_INT_EN_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_EN_POS)
#define M154_AES_AESDEC_DONE_FW_INT_EN_UMSK                     (~(((1ULL<<M154_AES_AESDEC_DONE_FW_INT_EN_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_EN_POS))
#define M154_AES_AESTX_DONE_HW_INT_EN                           M154_AES_AESTX_DONE_HW_INT_EN
#define M154_AES_AESTX_DONE_HW_INT_EN_POS                       (2U)
#define M154_AES_AESTX_DONE_HW_INT_EN_LEN                       (1U)
#define M154_AES_AESTX_DONE_HW_INT_EN_MSK                       (((1ULL<<M154_AES_AESTX_DONE_HW_INT_EN_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_EN_POS)
#define M154_AES_AESTX_DONE_HW_INT_EN_UMSK                      (~(((1ULL<<M154_AES_AESTX_DONE_HW_INT_EN_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_EN_POS))
#define M154_AES_AESRX_DONE_HW_INT_EN                           M154_AES_AESRX_DONE_HW_INT_EN
#define M154_AES_AESRX_DONE_HW_INT_EN_POS                       (3U)
#define M154_AES_AESRX_DONE_HW_INT_EN_LEN                       (1U)
#define M154_AES_AESRX_DONE_HW_INT_EN_MSK                       (((1ULL<<M154_AES_AESRX_DONE_HW_INT_EN_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_EN_POS)
#define M154_AES_AESRX_DONE_HW_INT_EN_UMSK                      (~(((1ULL<<M154_AES_AESRX_DONE_HW_INT_EN_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_EN_POS))
#define M154_AES_AESRX_DONE_RXERR_INT_EN                        M154_AES_AESRX_DONE_RXERR_INT_EN
#define M154_AES_AESRX_DONE_RXERR_INT_EN_POS                    (4U)
#define M154_AES_AESRX_DONE_RXERR_INT_EN_LEN                    (1U)
#define M154_AES_AESRX_DONE_RXERR_INT_EN_MSK                    (((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_EN_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_EN_POS)
#define M154_AES_AESRX_DONE_RXERR_INT_EN_UMSK                   (~(((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_EN_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_EN_POS))
#define M154_AES_DMATX_DONE_HW_INT_EN                           M154_AES_DMATX_DONE_HW_INT_EN
#define M154_AES_DMATX_DONE_HW_INT_EN_POS                       (5U)
#define M154_AES_DMATX_DONE_HW_INT_EN_LEN                       (1U)
#define M154_AES_DMATX_DONE_HW_INT_EN_MSK                       (((1ULL<<M154_AES_DMATX_DONE_HW_INT_EN_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_EN_POS)
#define M154_AES_DMATX_DONE_HW_INT_EN_UMSK                      (~(((1ULL<<M154_AES_DMATX_DONE_HW_INT_EN_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_EN_POS))
#define M154_AES_DMARX_DONE_HW_INT_EN                           M154_AES_DMARX_DONE_HW_INT_EN
#define M154_AES_DMARX_DONE_HW_INT_EN_POS                       (6U)
#define M154_AES_DMARX_DONE_HW_INT_EN_LEN                       (1U)
#define M154_AES_DMARX_DONE_HW_INT_EN_MSK                       (((1ULL<<M154_AES_DMARX_DONE_HW_INT_EN_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_EN_POS)
#define M154_AES_DMARX_DONE_HW_INT_EN_UMSK                      (~(((1ULL<<M154_AES_DMARX_DONE_HW_INT_EN_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_EN_POS))

/* 0x884 : Interrupt clear */
#define M154_AES_ICR_OFFSET                                     (0x884)
#define M154_AES_AESENC_DONE_FW_INT_CLR                         M154_AES_AESENC_DONE_FW_INT_CLR
#define M154_AES_AESENC_DONE_FW_INT_CLR_POS                     (0U)
#define M154_AES_AESENC_DONE_FW_INT_CLR_LEN                     (1U)
#define M154_AES_AESENC_DONE_FW_INT_CLR_MSK                     (((1ULL<<M154_AES_AESENC_DONE_FW_INT_CLR_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_CLR_POS)
#define M154_AES_AESENC_DONE_FW_INT_CLR_UMSK                    (~(((1ULL<<M154_AES_AESENC_DONE_FW_INT_CLR_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_CLR_POS))
#define M154_AES_AESDEC_DONE_FW_INT_CLR                         M154_AES_AESDEC_DONE_FW_INT_CLR
#define M154_AES_AESDEC_DONE_FW_INT_CLR_POS                     (1U)
#define M154_AES_AESDEC_DONE_FW_INT_CLR_LEN                     (1U)
#define M154_AES_AESDEC_DONE_FW_INT_CLR_MSK                     (((1ULL<<M154_AES_AESDEC_DONE_FW_INT_CLR_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_CLR_POS)
#define M154_AES_AESDEC_DONE_FW_INT_CLR_UMSK                    (~(((1ULL<<M154_AES_AESDEC_DONE_FW_INT_CLR_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_CLR_POS))
#define M154_AES_AESTX_DONE_HW_INT_CLR                          M154_AES_AESTX_DONE_HW_INT_CLR
#define M154_AES_AESTX_DONE_HW_INT_CLR_POS                      (2U)
#define M154_AES_AESTX_DONE_HW_INT_CLR_LEN                      (1U)
#define M154_AES_AESTX_DONE_HW_INT_CLR_MSK                      (((1ULL<<M154_AES_AESTX_DONE_HW_INT_CLR_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_CLR_POS)
#define M154_AES_AESTX_DONE_HW_INT_CLR_UMSK                     (~(((1ULL<<M154_AES_AESTX_DONE_HW_INT_CLR_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_CLR_POS))
#define M154_AES_AESRX_DONE_HW_INT_CLR                          M154_AES_AESRX_DONE_HW_INT_CLR
#define M154_AES_AESRX_DONE_HW_INT_CLR_POS                      (3U)
#define M154_AES_AESRX_DONE_HW_INT_CLR_LEN                      (1U)
#define M154_AES_AESRX_DONE_HW_INT_CLR_MSK                      (((1ULL<<M154_AES_AESRX_DONE_HW_INT_CLR_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_CLR_POS)
#define M154_AES_AESRX_DONE_HW_INT_CLR_UMSK                     (~(((1ULL<<M154_AES_AESRX_DONE_HW_INT_CLR_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_CLR_POS))
#define M154_AES_AESRX_DONE_RXERR_INT_CLR                       M154_AES_AESRX_DONE_RXERR_INT_CLR
#define M154_AES_AESRX_DONE_RXERR_INT_CLR_POS                   (4U)
#define M154_AES_AESRX_DONE_RXERR_INT_CLR_LEN                   (1U)
#define M154_AES_AESRX_DONE_RXERR_INT_CLR_MSK                   (((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_CLR_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_CLR_POS)
#define M154_AES_AESRX_DONE_RXERR_INT_CLR_UMSK                  (~(((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_CLR_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_CLR_POS))
#define M154_AES_DMATX_DONE_HW_INT_CLR                          M154_AES_DMATX_DONE_HW_INT_CLR
#define M154_AES_DMATX_DONE_HW_INT_CLR_POS                      (5U)
#define M154_AES_DMATX_DONE_HW_INT_CLR_LEN                      (1U)
#define M154_AES_DMATX_DONE_HW_INT_CLR_MSK                      (((1ULL<<M154_AES_DMATX_DONE_HW_INT_CLR_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_CLR_POS)
#define M154_AES_DMATX_DONE_HW_INT_CLR_UMSK                     (~(((1ULL<<M154_AES_DMATX_DONE_HW_INT_CLR_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_CLR_POS))
#define M154_AES_DMARX_DONE_HW_INT_CLR                          M154_AES_DMARX_DONE_HW_INT_CLR
#define M154_AES_DMARX_DONE_HW_INT_CLR_POS                      (6U)
#define M154_AES_DMARX_DONE_HW_INT_CLR_LEN                      (1U)
#define M154_AES_DMARX_DONE_HW_INT_CLR_MSK                      (((1ULL<<M154_AES_DMARX_DONE_HW_INT_CLR_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_CLR_POS)
#define M154_AES_DMARX_DONE_HW_INT_CLR_UMSK                     (~(((1ULL<<M154_AES_DMARX_DONE_HW_INT_CLR_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_CLR_POS))

/* 0x888 : Interrupt mask */
#define M154_AES_IMR_OFFSET                                     (0x888)
#define M154_AES_AESENC_DONE_FW_INT_MASK                        M154_AES_AESENC_DONE_FW_INT_MASK
#define M154_AES_AESENC_DONE_FW_INT_MASK_POS                    (0U)
#define M154_AES_AESENC_DONE_FW_INT_MASK_LEN                    (1U)
#define M154_AES_AESENC_DONE_FW_INT_MASK_MSK                    (((1ULL<<M154_AES_AESENC_DONE_FW_INT_MASK_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_MASK_POS)
#define M154_AES_AESENC_DONE_FW_INT_MASK_UMSK                   (~(((1ULL<<M154_AES_AESENC_DONE_FW_INT_MASK_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_MASK_POS))
#define M154_AES_AESDEC_DONE_FW_INT_MASK                        M154_AES_AESDEC_DONE_FW_INT_MASK
#define M154_AES_AESDEC_DONE_FW_INT_MASK_POS                    (1U)
#define M154_AES_AESDEC_DONE_FW_INT_MASK_LEN                    (1U)
#define M154_AES_AESDEC_DONE_FW_INT_MASK_MSK                    (((1ULL<<M154_AES_AESDEC_DONE_FW_INT_MASK_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_MASK_POS)
#define M154_AES_AESDEC_DONE_FW_INT_MASK_UMSK                   (~(((1ULL<<M154_AES_AESDEC_DONE_FW_INT_MASK_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_MASK_POS))
#define M154_AES_AESTX_DONE_HW_INT_MASK                         M154_AES_AESTX_DONE_HW_INT_MASK
#define M154_AES_AESTX_DONE_HW_INT_MASK_POS                     (2U)
#define M154_AES_AESTX_DONE_HW_INT_MASK_LEN                     (1U)
#define M154_AES_AESTX_DONE_HW_INT_MASK_MSK                     (((1ULL<<M154_AES_AESTX_DONE_HW_INT_MASK_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_MASK_POS)
#define M154_AES_AESTX_DONE_HW_INT_MASK_UMSK                    (~(((1ULL<<M154_AES_AESTX_DONE_HW_INT_MASK_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_MASK_POS))
#define M154_AES_AESRX_DONE_HW_INT_MASK                         M154_AES_AESRX_DONE_HW_INT_MASK
#define M154_AES_AESRX_DONE_HW_INT_MASK_POS                     (3U)
#define M154_AES_AESRX_DONE_HW_INT_MASK_LEN                     (1U)
#define M154_AES_AESRX_DONE_HW_INT_MASK_MSK                     (((1ULL<<M154_AES_AESRX_DONE_HW_INT_MASK_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_MASK_POS)
#define M154_AES_AESRX_DONE_HW_INT_MASK_UMSK                    (~(((1ULL<<M154_AES_AESRX_DONE_HW_INT_MASK_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_MASK_POS))
#define M154_AES_AESRX_DONE_RXERR_INT_MASK                      M154_AES_AESRX_DONE_RXERR_INT_MASK
#define M154_AES_AESRX_DONE_RXERR_INT_MASK_POS                  (4U)
#define M154_AES_AESRX_DONE_RXERR_INT_MASK_LEN                  (1U)
#define M154_AES_AESRX_DONE_RXERR_INT_MASK_MSK                  (((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_MASK_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_MASK_POS)
#define M154_AES_AESRX_DONE_RXERR_INT_MASK_UMSK                 (~(((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_MASK_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_MASK_POS))
#define M154_AES_DMATX_DONE_HW_INT_MASK                         M154_AES_DMATX_DONE_HW_INT_MASK
#define M154_AES_DMATX_DONE_HW_INT_MASK_POS                     (5U)
#define M154_AES_DMATX_DONE_HW_INT_MASK_LEN                     (1U)
#define M154_AES_DMATX_DONE_HW_INT_MASK_MSK                     (((1ULL<<M154_AES_DMATX_DONE_HW_INT_MASK_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_MASK_POS)
#define M154_AES_DMATX_DONE_HW_INT_MASK_UMSK                    (~(((1ULL<<M154_AES_DMATX_DONE_HW_INT_MASK_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_MASK_POS))
#define M154_AES_DMARX_DONE_HW_INT_MASK                         M154_AES_DMARX_DONE_HW_INT_MASK
#define M154_AES_DMARX_DONE_HW_INT_MASK_POS                     (6U)
#define M154_AES_DMARX_DONE_HW_INT_MASK_LEN                     (1U)
#define M154_AES_DMARX_DONE_HW_INT_MASK_MSK                     (((1ULL<<M154_AES_DMARX_DONE_HW_INT_MASK_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_MASK_POS)
#define M154_AES_DMARX_DONE_HW_INT_MASK_UMSK                    (~(((1ULL<<M154_AES_DMARX_DONE_HW_INT_MASK_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_MASK_POS))

/* 0x88C : Interrupt status */
#define M154_AES_ISR_OFFSET                                     (0x88C)
#define M154_AES_AESENC_DONE_FW_INT_RAW                         M154_AES_AESENC_DONE_FW_INT_RAW
#define M154_AES_AESENC_DONE_FW_INT_RAW_POS                     (0U)
#define M154_AES_AESENC_DONE_FW_INT_RAW_LEN                     (1U)
#define M154_AES_AESENC_DONE_FW_INT_RAW_MSK                     (((1ULL<<M154_AES_AESENC_DONE_FW_INT_RAW_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_RAW_POS)
#define M154_AES_AESENC_DONE_FW_INT_RAW_UMSK                    (~(((1ULL<<M154_AES_AESENC_DONE_FW_INT_RAW_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_RAW_POS))
#define M154_AES_AESDEC_DONE_FW_INT_RAW                         M154_AES_AESDEC_DONE_FW_INT_RAW
#define M154_AES_AESDEC_DONE_FW_INT_RAW_POS                     (1U)
#define M154_AES_AESDEC_DONE_FW_INT_RAW_LEN                     (1U)
#define M154_AES_AESDEC_DONE_FW_INT_RAW_MSK                     (((1ULL<<M154_AES_AESDEC_DONE_FW_INT_RAW_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_RAW_POS)
#define M154_AES_AESDEC_DONE_FW_INT_RAW_UMSK                    (~(((1ULL<<M154_AES_AESDEC_DONE_FW_INT_RAW_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_RAW_POS))
#define M154_AES_AESTX_DONE_HW_INT_RAW                          M154_AES_AESTX_DONE_HW_INT_RAW
#define M154_AES_AESTX_DONE_HW_INT_RAW_POS                      (2U)
#define M154_AES_AESTX_DONE_HW_INT_RAW_LEN                      (1U)
#define M154_AES_AESTX_DONE_HW_INT_RAW_MSK                      (((1ULL<<M154_AES_AESTX_DONE_HW_INT_RAW_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_RAW_POS)
#define M154_AES_AESTX_DONE_HW_INT_RAW_UMSK                     (~(((1ULL<<M154_AES_AESTX_DONE_HW_INT_RAW_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_RAW_POS))
#define M154_AES_AESRX_DONE_HW_INT_RAW                          M154_AES_AESRX_DONE_HW_INT_RAW
#define M154_AES_AESRX_DONE_HW_INT_RAW_POS                      (3U)
#define M154_AES_AESRX_DONE_HW_INT_RAW_LEN                      (1U)
#define M154_AES_AESRX_DONE_HW_INT_RAW_MSK                      (((1ULL<<M154_AES_AESRX_DONE_HW_INT_RAW_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_RAW_POS)
#define M154_AES_AESRX_DONE_HW_INT_RAW_UMSK                     (~(((1ULL<<M154_AES_AESRX_DONE_HW_INT_RAW_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_RAW_POS))
#define M154_AES_AESRX_DONE_RXERR_INT_RAW                       M154_AES_AESRX_DONE_RXERR_INT_RAW
#define M154_AES_AESRX_DONE_RXERR_INT_RAW_POS                   (4U)
#define M154_AES_AESRX_DONE_RXERR_INT_RAW_LEN                   (1U)
#define M154_AES_AESRX_DONE_RXERR_INT_RAW_MSK                   (((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_RAW_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_RAW_POS)
#define M154_AES_AESRX_DONE_RXERR_INT_RAW_UMSK                  (~(((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_RAW_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_RAW_POS))
#define M154_AES_DMATX_DONE_HW_INT_RAW                          M154_AES_DMATX_DONE_HW_INT_RAW
#define M154_AES_DMATX_DONE_HW_INT_RAW_POS                      (5U)
#define M154_AES_DMATX_DONE_HW_INT_RAW_LEN                      (1U)
#define M154_AES_DMATX_DONE_HW_INT_RAW_MSK                      (((1ULL<<M154_AES_DMATX_DONE_HW_INT_RAW_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_RAW_POS)
#define M154_AES_DMATX_DONE_HW_INT_RAW_UMSK                     (~(((1ULL<<M154_AES_DMATX_DONE_HW_INT_RAW_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_RAW_POS))
#define M154_AES_DMARX_DONE_HW_INT_RAW                          M154_AES_DMARX_DONE_HW_INT_RAW
#define M154_AES_DMARX_DONE_HW_INT_RAW_POS                      (6U)
#define M154_AES_DMARX_DONE_HW_INT_RAW_LEN                      (1U)
#define M154_AES_DMARX_DONE_HW_INT_RAW_MSK                      (((1ULL<<M154_AES_DMARX_DONE_HW_INT_RAW_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_RAW_POS)
#define M154_AES_DMARX_DONE_HW_INT_RAW_UMSK                     (~(((1ULL<<M154_AES_DMARX_DONE_HW_INT_RAW_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_RAW_POS))
#define M154_AES_SET_AESENC_DONE_FW                             M154_AES_SET_AESENC_DONE_FW
#define M154_AES_SET_AESENC_DONE_FW_POS                         (16U)
#define M154_AES_SET_AESENC_DONE_FW_LEN                         (1U)
#define M154_AES_SET_AESENC_DONE_FW_MSK                         (((1ULL<<M154_AES_SET_AESENC_DONE_FW_LEN)-1)<<M154_AES_SET_AESENC_DONE_FW_POS)
#define M154_AES_SET_AESENC_DONE_FW_UMSK                        (~(((1ULL<<M154_AES_SET_AESENC_DONE_FW_LEN)-1)<<M154_AES_SET_AESENC_DONE_FW_POS))
#define M154_AES_SET_AESDEC_DONE_FW                             M154_AES_SET_AESDEC_DONE_FW
#define M154_AES_SET_AESDEC_DONE_FW_POS                         (17U)
#define M154_AES_SET_AESDEC_DONE_FW_LEN                         (1U)
#define M154_AES_SET_AESDEC_DONE_FW_MSK                         (((1ULL<<M154_AES_SET_AESDEC_DONE_FW_LEN)-1)<<M154_AES_SET_AESDEC_DONE_FW_POS)
#define M154_AES_SET_AESDEC_DONE_FW_UMSK                        (~(((1ULL<<M154_AES_SET_AESDEC_DONE_FW_LEN)-1)<<M154_AES_SET_AESDEC_DONE_FW_POS))
#define M154_AES_SET_AESTX_DONE_HW                              M154_AES_SET_AESTX_DONE_HW
#define M154_AES_SET_AESTX_DONE_HW_POS                          (18U)
#define M154_AES_SET_AESTX_DONE_HW_LEN                          (1U)
#define M154_AES_SET_AESTX_DONE_HW_MSK                          (((1ULL<<M154_AES_SET_AESTX_DONE_HW_LEN)-1)<<M154_AES_SET_AESTX_DONE_HW_POS)
#define M154_AES_SET_AESTX_DONE_HW_UMSK                         (~(((1ULL<<M154_AES_SET_AESTX_DONE_HW_LEN)-1)<<M154_AES_SET_AESTX_DONE_HW_POS))
#define M154_AES_SET_AESRX_DONE_HW                              M154_AES_SET_AESRX_DONE_HW
#define M154_AES_SET_AESRX_DONE_HW_POS                          (19U)
#define M154_AES_SET_AESRX_DONE_HW_LEN                          (1U)
#define M154_AES_SET_AESRX_DONE_HW_MSK                          (((1ULL<<M154_AES_SET_AESRX_DONE_HW_LEN)-1)<<M154_AES_SET_AESRX_DONE_HW_POS)
#define M154_AES_SET_AESRX_DONE_HW_UMSK                         (~(((1ULL<<M154_AES_SET_AESRX_DONE_HW_LEN)-1)<<M154_AES_SET_AESRX_DONE_HW_POS))
#define M154_AES_SET_AESRX_DONE_RXERR                           M154_AES_SET_AESRX_DONE_RXERR
#define M154_AES_SET_AESRX_DONE_RXERR_POS                       (20U)
#define M154_AES_SET_AESRX_DONE_RXERR_LEN                       (1U)
#define M154_AES_SET_AESRX_DONE_RXERR_MSK                       (((1ULL<<M154_AES_SET_AESRX_DONE_RXERR_LEN)-1)<<M154_AES_SET_AESRX_DONE_RXERR_POS)
#define M154_AES_SET_AESRX_DONE_RXERR_UMSK                      (~(((1ULL<<M154_AES_SET_AESRX_DONE_RXERR_LEN)-1)<<M154_AES_SET_AESRX_DONE_RXERR_POS))
#define M154_AES_SET_DMATX_DONE_HW                              M154_AES_SET_DMATX_DONE_HW
#define M154_AES_SET_DMATX_DONE_HW_POS                          (21U)
#define M154_AES_SET_DMATX_DONE_HW_LEN                          (1U)
#define M154_AES_SET_DMATX_DONE_HW_MSK                          (((1ULL<<M154_AES_SET_DMATX_DONE_HW_LEN)-1)<<M154_AES_SET_DMATX_DONE_HW_POS)
#define M154_AES_SET_DMATX_DONE_HW_UMSK                         (~(((1ULL<<M154_AES_SET_DMATX_DONE_HW_LEN)-1)<<M154_AES_SET_DMATX_DONE_HW_POS))
#define M154_AES_SET_DMARX_DONE_HW                              M154_AES_SET_DMARX_DONE_HW
#define M154_AES_SET_DMARX_DONE_HW_POS                          (22U)
#define M154_AES_SET_DMARX_DONE_HW_LEN                          (1U)
#define M154_AES_SET_DMARX_DONE_HW_MSK                          (((1ULL<<M154_AES_SET_DMARX_DONE_HW_LEN)-1)<<M154_AES_SET_DMARX_DONE_HW_POS)
#define M154_AES_SET_DMARX_DONE_HW_UMSK                         (~(((1ULL<<M154_AES_SET_DMARX_DONE_HW_LEN)-1)<<M154_AES_SET_DMARX_DONE_HW_POS))

/* 0x890 : Interrupt */
#define M154_AES_IR_OFFSET                                      (0x890)
#define M154_AES_AESENC_DONE_FW_INT                             M154_AES_AESENC_DONE_FW_INT
#define M154_AES_AESENC_DONE_FW_INT_POS                         (0U)
#define M154_AES_AESENC_DONE_FW_INT_LEN                         (1U)
#define M154_AES_AESENC_DONE_FW_INT_MSK                         (((1ULL<<M154_AES_AESENC_DONE_FW_INT_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_POS)
#define M154_AES_AESENC_DONE_FW_INT_UMSK                        (~(((1ULL<<M154_AES_AESENC_DONE_FW_INT_LEN)-1)<<M154_AES_AESENC_DONE_FW_INT_POS))
#define M154_AES_AESDEC_DONE_FW_INT                             M154_AES_AESDEC_DONE_FW_INT
#define M154_AES_AESDEC_DONE_FW_INT_POS                         (1U)
#define M154_AES_AESDEC_DONE_FW_INT_LEN                         (1U)
#define M154_AES_AESDEC_DONE_FW_INT_MSK                         (((1ULL<<M154_AES_AESDEC_DONE_FW_INT_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_POS)
#define M154_AES_AESDEC_DONE_FW_INT_UMSK                        (~(((1ULL<<M154_AES_AESDEC_DONE_FW_INT_LEN)-1)<<M154_AES_AESDEC_DONE_FW_INT_POS))
#define M154_AES_AESTX_DONE_HW_INT                              M154_AES_AESTX_DONE_HW_INT
#define M154_AES_AESTX_DONE_HW_INT_POS                          (2U)
#define M154_AES_AESTX_DONE_HW_INT_LEN                          (1U)
#define M154_AES_AESTX_DONE_HW_INT_MSK                          (((1ULL<<M154_AES_AESTX_DONE_HW_INT_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_POS)
#define M154_AES_AESTX_DONE_HW_INT_UMSK                         (~(((1ULL<<M154_AES_AESTX_DONE_HW_INT_LEN)-1)<<M154_AES_AESTX_DONE_HW_INT_POS))
#define M154_AES_AESRX_DONE_HW_INT                              M154_AES_AESRX_DONE_HW_INT
#define M154_AES_AESRX_DONE_HW_INT_POS                          (3U)
#define M154_AES_AESRX_DONE_HW_INT_LEN                          (1U)
#define M154_AES_AESRX_DONE_HW_INT_MSK                          (((1ULL<<M154_AES_AESRX_DONE_HW_INT_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_POS)
#define M154_AES_AESRX_DONE_HW_INT_UMSK                         (~(((1ULL<<M154_AES_AESRX_DONE_HW_INT_LEN)-1)<<M154_AES_AESRX_DONE_HW_INT_POS))
#define M154_AES_AESRX_DONE_RXERR_INT                           M154_AES_AESRX_DONE_RXERR_INT
#define M154_AES_AESRX_DONE_RXERR_INT_POS                       (4U)
#define M154_AES_AESRX_DONE_RXERR_INT_LEN                       (1U)
#define M154_AES_AESRX_DONE_RXERR_INT_MSK                       (((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_POS)
#define M154_AES_AESRX_DONE_RXERR_INT_UMSK                      (~(((1ULL<<M154_AES_AESRX_DONE_RXERR_INT_LEN)-1)<<M154_AES_AESRX_DONE_RXERR_INT_POS))
#define M154_AES_DMATX_DONE_HW_INT                              M154_AES_DMATX_DONE_HW_INT
#define M154_AES_DMATX_DONE_HW_INT_POS                          (5U)
#define M154_AES_DMATX_DONE_HW_INT_LEN                          (1U)
#define M154_AES_DMATX_DONE_HW_INT_MSK                          (((1ULL<<M154_AES_DMATX_DONE_HW_INT_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_POS)
#define M154_AES_DMATX_DONE_HW_INT_UMSK                         (~(((1ULL<<M154_AES_DMATX_DONE_HW_INT_LEN)-1)<<M154_AES_DMATX_DONE_HW_INT_POS))
#define M154_AES_DMARX_DONE_HW_INT                              M154_AES_DMARX_DONE_HW_INT
#define M154_AES_DMARX_DONE_HW_INT_POS                          (6U)
#define M154_AES_DMARX_DONE_HW_INT_LEN                          (1U)
#define M154_AES_DMARX_DONE_HW_INT_MSK                          (((1ULL<<M154_AES_DMARX_DONE_HW_INT_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_POS)
#define M154_AES_DMARX_DONE_HW_INT_UMSK                         (~(((1ULL<<M154_AES_DMARX_DONE_HW_INT_LEN)-1)<<M154_AES_DMARX_DONE_HW_INT_POS))


struct  m154_aes_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[2048];

    /* 0x800 : AES control */
    union {
        struct {
            uint32_t aesdma_trig                    :  1; /* [    0],        w1c,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t aes_dma_mode                   :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rxkey_rdy                      :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t sw_rst_aes                     :  1; /* [   16],        r/w,        0x0 */
            uint32_t sw_rst_mst                     :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_acr;

    /* 0x804 : AES mode */
    union {
        struct {
            uint32_t aes_enable                     :  1; /* [    0],        r/w,        0x0 */
            uint32_t rxdma_en                       :  1; /* [    1],        r/w,        0x0 */
            uint32_t rxdec_en                       :  1; /* [    2],        r/w,        0x0 */
            uint32_t ahb_bytewr_en                  :  1; /* [    3],        r/w,        0x1 */
            uint32_t aes_mode                       :  2; /* [ 5: 4],        r/w,        0x3 */
            uint32_t aes_key_size                   :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_amr;

    /* 0x808 : AES length */
    union {
        struct {
            uint32_t aes_m_len                      : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t aes_a_len                      : 11; /* [22:12],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t aes_nonce_len                  :  4; /* [27:24],        r/w,        0xd */
            uint32_t aes_mic_len                    :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_alr;

    /* 0x80C : AES RX length */
    union {
        struct {
            uint32_t rx_m_len_r                     :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_11                  :  4; /* [11: 8],       rsvd,        0x0 */
            uint32_t rx_a_len_r                     :  8; /* [19:12],          r,        0x0 */
            uint32_t reserved_20_27                 :  8; /* [27:20],       rsvd,        0x0 */
            uint32_t rx_mic_len_r                   :  2; /* [29:28],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_rxalr;

    /* 0x810 : AES a data source address */
    union {
        struct {
            uint32_t aes_asa                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_aasar;

    /* 0x814 : AES m data source address */
    union {
        struct {
            uint32_t aes_msa                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_amsar;

    /* 0x818 : AES m data destination address */
    union {
        struct {
            uint32_t aes_mda                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_amdar;

    /* 0x81C : AES RX a data destination address */
    union {
        struct {
            uint32_t aes_rxada                      : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_rxaadar;

    /* 0x820 : AES RX m data destination address */
    union {
        struct {
            uint32_t aes_rxmda                      : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_rxamdar;

    /* 0x824  reserved */
    uint8_t RESERVED0x824[12];

    /* 0x830 : AES nonce */
    union {
        struct {
            uint32_t anr0                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_anr0;

    /* 0x834 : m154_aes_anr1 */
    union {
        struct {
            uint32_t anr1                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_anr1;

    /* 0x838 : m154_aes_anr2 */
    union {
        struct {
            uint32_t anr2                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_anr2;

    /* 0x83C : m154_aes_anr3 */
    union {
        struct {
            uint32_t anr3                           : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_anr3;

    /* 0x840 : AES key */
    union {
        struct {
            uint32_t akr0                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_akr0;

    /* 0x844 : m154_aes_akr1 */
    union {
        struct {
            uint32_t akr1                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_akr1;

    /* 0x848 : m154_aes_akr2 */
    union {
        struct {
            uint32_t akr2                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_akr2;

    /* 0x84C : m154_aes_akr3 */
    union {
        struct {
            uint32_t akr3                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_akr3;

    /* 0x850 : m154_aes_akr4 */
    union {
        struct {
            uint32_t akr4                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_akr4;

    /* 0x854 : m154_aes_akr5 */
    union {
        struct {
            uint32_t akr5                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_akr5;

    /* 0x858 : m154_aes_akr6 */
    union {
        struct {
            uint32_t akr6                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_akr6;

    /* 0x85C : m154_aes_akr7 */
    union {
        struct {
            uint32_t akr7                           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_akr7;

    /* 0x860 : AES mic */
    union {
        struct {
            uint32_t amicr0                         : 32; /* [31: 0],          r,          x */
        }BF;
        uint32_t WORD;
    } m154_aes_amicr0;

    /* 0x864 : m154_aes_amicr1 */
    union {
        struct {
            uint32_t amicr1                         : 32; /* [31: 0],          r,          x */
        }BF;
        uint32_t WORD;
    } m154_aes_amicr1;

    /* 0x868 : m154_aes_amicr2 */
    union {
        struct {
            uint32_t amicr2                         : 32; /* [31: 0],          r,          x */
        }BF;
        uint32_t WORD;
    } m154_aes_amicr2;

    /* 0x86C : m154_aes_amicr3 */
    union {
        struct {
            uint32_t amicr3                         : 32; /* [31: 0],          r,          x */
        }BF;
        uint32_t WORD;
    } m154_aes_amicr3;

    /* 0x870 : AES status 1 */
    union {
        struct {
            uint32_t aes_busy                       :  1; /* [    0],          r,        0x0 */
            uint32_t mic_error                      :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t aes_mst_cs                     :  4; /* [ 7: 4],          r,        0x0 */
            uint32_t aes_ctrl_cs                    :  4; /* [11: 8],          r,        0x0 */
            uint32_t dma_ctrl_cs                    :  4; /* [15:12],          r,        0x0 */
            uint32_t last_m_blk                     :  1; /* [   16],          r,        0x0 */
            uint32_t last_a_blk                     :  1; /* [   17],          r,        0x0 */
            uint32_t first_m_blk                    :  1; /* [   18],          r,        0x0 */
            uint32_t first_a_blk                    :  1; /* [   19],          r,        0x0 */
            uint32_t tx_mpdu_aes_ddt_done           :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_status1;

    /* 0x874 : AES status 2 */
    union {
        struct {
            uint32_t u_ctr_cnt                      : 16; /* [15: 0],          r,        0x0 */
            uint32_t u_c_cnt                        :  9; /* [24:16],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_status2;

    /* 0x878 : AES status 3 */
    union {
        struct {
            uint32_t u_am_cnt                       : 10; /* [ 9: 0],          r,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t u_cnt                          :  2; /* [13:12],          r,        0x0 */
            uint32_t reserved_14_31                 : 18; /* [31:14],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_status3;

    /* 0x87c  reserved */
    uint8_t RESERVED0x87c[4];

    /* 0x880 : Interrupt enable */
    union {
        struct {
            uint32_t aesenc_done_fw_int_en          :  1; /* [    0],        r/w,        0x1 */
            uint32_t aesdec_done_fw_int_en          :  1; /* [    1],        r/w,        0x1 */
            uint32_t aestx_done_hw_int_en           :  1; /* [    2],        r/w,        0x0 */
            uint32_t aesrx_done_hw_int_en           :  1; /* [    3],        r/w,        0x1 */
            uint32_t aesrx_done_rxerr_int_en        :  1; /* [    4],        r/w,        0x1 */
            uint32_t dmatx_done_hw_int_en           :  1; /* [    5],        r/w,        0x0 */
            uint32_t dmarx_done_hw_int_en           :  1; /* [    6],        r/w,        0x1 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_ier;

    /* 0x884 : Interrupt clear */
    union {
        struct {
            uint32_t aesenc_done_fw_int_clr         :  1; /* [    0],        w1c,        0x0 */
            uint32_t aesdec_done_fw_int_clr         :  1; /* [    1],        w1c,        0x0 */
            uint32_t aestx_done_hw_int_clr          :  1; /* [    2],        w1c,        0x0 */
            uint32_t aesrx_done_hw_int_clr          :  1; /* [    3],        w1c,        0x0 */
            uint32_t aesrx_done_rxerr_int_clr       :  1; /* [    4],        w1c,        0x0 */
            uint32_t dmatx_done_hw_int_clr          :  1; /* [    5],        w1c,        0x0 */
            uint32_t dmarx_done_hw_int_clr          :  1; /* [    6],        w1c,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_icr;

    /* 0x888 : Interrupt mask */
    union {
        struct {
            uint32_t aesenc_done_fw_int_mask        :  1; /* [    0],        r/w,        0x0 */
            uint32_t aesdec_done_fw_int_mask        :  1; /* [    1],        r/w,        0x0 */
            uint32_t aestx_done_hw_int_mask         :  1; /* [    2],        r/w,        0x1 */
            uint32_t aesrx_done_hw_int_mask         :  1; /* [    3],        r/w,        0x0 */
            uint32_t aesrx_done_rxerr_int_mask      :  1; /* [    4],        r/w,        0x0 */
            uint32_t dmatx_done_hw_int_mask         :  1; /* [    5],        r/w,        0x1 */
            uint32_t dmarx_done_hw_int_mask         :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_imr;

    /* 0x88C : Interrupt status */
    union {
        struct {
            uint32_t aesenc_done_fw_int_raw         :  1; /* [    0],          r,        0x0 */
            uint32_t aesdec_done_fw_int_raw         :  1; /* [    1],          r,        0x0 */
            uint32_t aestx_done_hw_int_raw          :  1; /* [    2],          r,        0x0 */
            uint32_t aesrx_done_hw_int_raw          :  1; /* [    3],          r,        0x0 */
            uint32_t aesrx_done_rxerr_int_raw       :  1; /* [    4],          r,        0x0 */
            uint32_t dmatx_done_hw_int_raw          :  1; /* [    5],          r,        0x0 */
            uint32_t dmarx_done_hw_int_raw          :  1; /* [    6],          r,        0x0 */
            uint32_t reserved_7_15                  :  9; /* [15: 7],       rsvd,        0x0 */
            uint32_t set_aesenc_done_fw             :  1; /* [   16],          r,        0x0 */
            uint32_t set_aesdec_done_fw             :  1; /* [   17],          r,        0x0 */
            uint32_t set_aestx_done_hw              :  1; /* [   18],          r,        0x0 */
            uint32_t set_aesrx_done_hw              :  1; /* [   19],          r,        0x0 */
            uint32_t set_aesrx_done_rxerr           :  1; /* [   20],          r,        0x0 */
            uint32_t set_dmatx_done_hw              :  1; /* [   21],          r,        0x0 */
            uint32_t set_dmarx_done_hw              :  1; /* [   22],          r,        0x0 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_isr;

    /* 0x890 : Interrupt */
    union {
        struct {
            uint32_t aesenc_done_fw_int             :  1; /* [    0],          r,        0x0 */
            uint32_t aesdec_done_fw_int             :  1; /* [    1],          r,        0x0 */
            uint32_t aestx_done_hw_int              :  1; /* [    2],          r,        0x0 */
            uint32_t aesrx_done_hw_int              :  1; /* [    3],          r,        0x0 */
            uint32_t aesrx_done_rxerr_int           :  1; /* [    4],          r,        0x0 */
            uint32_t dmatx_done_hw_int              :  1; /* [    5],          r,        0x0 */
            uint32_t dmarx_done_hw_int              :  1; /* [    6],          r,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_aes_ir;

};

typedef volatile struct m154_aes_reg m154_aes_reg_t;


#endif  /* __M154_AES_REG_H__ */
