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

#ifndef  __AUD_REG_H__
#define  __AUD_REG_H__

#include "ipro7.h"

/* 0xC00 : audpdm_top */
#define AUDPDM_TOP_OFFSET                                       (0xC00)
#define AUDIO_CKG_EN                                            AUDIO_CKG_EN
#define AUDIO_CKG_EN_POS                                        (0U)
#define AUDIO_CKG_EN_LEN                                        (1U)
#define AUDIO_CKG_EN_MSK                                        (((1U<<AUDIO_CKG_EN_LEN)-1)<<AUDIO_CKG_EN_POS)
#define AUDIO_CKG_EN_UMSK                                       (~(((1U<<AUDIO_CKG_EN_LEN)-1)<<AUDIO_CKG_EN_POS))
#define AUD_ADC_ITF_INV_SEL                                     AUD_ADC_ITF_INV_SEL
#define AUD_ADC_ITF_INV_SEL_POS                                 (2U)
#define AUD_ADC_ITF_INV_SEL_LEN                                 (1U)
#define AUD_ADC_ITF_INV_SEL_MSK                                 (((1U<<AUD_ADC_ITF_INV_SEL_LEN)-1)<<AUD_ADC_ITF_INV_SEL_POS)
#define AUD_ADC_ITF_INV_SEL_UMSK                                (~(((1U<<AUD_ADC_ITF_INV_SEL_LEN)-1)<<AUD_ADC_ITF_INV_SEL_POS))
#define AUD_PDM_ITF_INV_SEL                                     AUD_PDM_ITF_INV_SEL
#define AUD_PDM_ITF_INV_SEL_POS                                 (3U)
#define AUD_PDM_ITF_INV_SEL_LEN                                 (1U)
#define AUD_PDM_ITF_INV_SEL_MSK                                 (((1U<<AUD_PDM_ITF_INV_SEL_LEN)-1)<<AUD_PDM_ITF_INV_SEL_POS)
#define AUD_PDM_ITF_INV_SEL_UMSK                                (~(((1U<<AUD_PDM_ITF_INV_SEL_LEN)-1)<<AUD_PDM_ITF_INV_SEL_POS))
#define AUD_ADC_RATE                                            AUD_ADC_RATE
#define AUD_ADC_RATE_POS                                        (28U)
#define AUD_ADC_RATE_LEN                                        (4U)
#define AUD_ADC_RATE_MSK                                        (((1U<<AUD_ADC_RATE_LEN)-1)<<AUD_ADC_RATE_POS)
#define AUD_ADC_RATE_UMSK                                       (~(((1U<<AUD_ADC_RATE_LEN)-1)<<AUD_ADC_RATE_POS))

/* 0xC04 : audpdm_itf */
#define AUDPDM_ITF_OFFSET                                       (0xC04)
#define AUD_ADC_0_EN                                            AUD_ADC_0_EN
#define AUD_ADC_0_EN_POS                                        (0U)
#define AUD_ADC_0_EN_LEN                                        (1U)
#define AUD_ADC_0_EN_MSK                                        (((1U<<AUD_ADC_0_EN_LEN)-1)<<AUD_ADC_0_EN_POS)
#define AUD_ADC_0_EN_UMSK                                       (~(((1U<<AUD_ADC_0_EN_LEN)-1)<<AUD_ADC_0_EN_POS))
#define AUD_ADC_ITF_EN                                          AUD_ADC_ITF_EN
#define AUD_ADC_ITF_EN_POS                                      (30U)
#define AUD_ADC_ITF_EN_LEN                                      (1U)
#define AUD_ADC_ITF_EN_MSK                                      (((1U<<AUD_ADC_ITF_EN_LEN)-1)<<AUD_ADC_ITF_EN_POS)
#define AUD_ADC_ITF_EN_UMSK                                     (~(((1U<<AUD_ADC_ITF_EN_LEN)-1)<<AUD_ADC_ITF_EN_POS))

/* 0xC08 : pdm_adc_0 */
#define AUD_PDM_ADC_0_OFFSET                                    (0xC08)
#define AUD_ADC_0_FIR_MODE                                      AUD_ADC_0_FIR_MODE
#define AUD_ADC_0_FIR_MODE_POS                                  (0U)
#define AUD_ADC_0_FIR_MODE_LEN                                  (1U)
#define AUD_ADC_0_FIR_MODE_MSK                                  (((1U<<AUD_ADC_0_FIR_MODE_LEN)-1)<<AUD_ADC_0_FIR_MODE_POS)
#define AUD_ADC_0_FIR_MODE_UMSK                                 (~(((1U<<AUD_ADC_0_FIR_MODE_LEN)-1)<<AUD_ADC_0_FIR_MODE_POS))

/* 0xC0C : pdm_adc_1 */
#define AUD_PDM_ADC_1_OFFSET                                    (0xC0C)
#define AUD_ADC_0_K1                                            AUD_ADC_0_K1
#define AUD_ADC_0_K1_POS                                        (0U)
#define AUD_ADC_0_K1_LEN                                        (4U)
#define AUD_ADC_0_K1_MSK                                        (((1U<<AUD_ADC_0_K1_LEN)-1)<<AUD_ADC_0_K1_POS)
#define AUD_ADC_0_K1_UMSK                                       (~(((1U<<AUD_ADC_0_K1_LEN)-1)<<AUD_ADC_0_K1_POS))
#define AUD_ADC_0_K1_EN                                         AUD_ADC_0_K1_EN
#define AUD_ADC_0_K1_EN_POS                                     (4U)
#define AUD_ADC_0_K1_EN_LEN                                     (1U)
#define AUD_ADC_0_K1_EN_MSK                                     (((1U<<AUD_ADC_0_K1_EN_LEN)-1)<<AUD_ADC_0_K1_EN_POS)
#define AUD_ADC_0_K1_EN_UMSK                                    (~(((1U<<AUD_ADC_0_K1_EN_LEN)-1)<<AUD_ADC_0_K1_EN_POS))
#define AUD_ADC_0_K2                                            AUD_ADC_0_K2
#define AUD_ADC_0_K2_POS                                        (5U)
#define AUD_ADC_0_K2_LEN                                        (4U)
#define AUD_ADC_0_K2_MSK                                        (((1U<<AUD_ADC_0_K2_LEN)-1)<<AUD_ADC_0_K2_POS)
#define AUD_ADC_0_K2_UMSK                                       (~(((1U<<AUD_ADC_0_K2_LEN)-1)<<AUD_ADC_0_K2_POS))
#define AUD_ADC_0_K2_EN                                         AUD_ADC_0_K2_EN
#define AUD_ADC_0_K2_EN_POS                                     (9U)
#define AUD_ADC_0_K2_EN_LEN                                     (1U)
#define AUD_ADC_0_K2_EN_MSK                                     (((1U<<AUD_ADC_0_K2_EN_LEN)-1)<<AUD_ADC_0_K2_EN_POS)
#define AUD_ADC_0_K2_EN_UMSK                                    (~(((1U<<AUD_ADC_0_K2_EN_LEN)-1)<<AUD_ADC_0_K2_EN_POS))

/* 0xC10 : pdm_dac_0 */
#define AUD_PDM_DAC_0_OFFSET                                    (0xC10)
#define AUD_ADC_PDM_H                                           AUD_ADC_PDM_H
#define AUD_ADC_PDM_H_POS                                       (0U)
#define AUD_ADC_PDM_H_LEN                                       (4U)
#define AUD_ADC_PDM_H_MSK                                       (((1U<<AUD_ADC_PDM_H_LEN)-1)<<AUD_ADC_PDM_H_POS)
#define AUD_ADC_PDM_H_UMSK                                      (~(((1U<<AUD_ADC_PDM_H_LEN)-1)<<AUD_ADC_PDM_H_POS))
#define AUD_ADC_PDM_L                                           AUD_ADC_PDM_L
#define AUD_ADC_PDM_L_POS                                       (6U)
#define AUD_ADC_PDM_L_LEN                                       (4U)
#define AUD_ADC_PDM_L_MSK                                       (((1U<<AUD_ADC_PDM_L_LEN)-1)<<AUD_ADC_PDM_L_POS)
#define AUD_ADC_PDM_L_UMSK                                      (~(((1U<<AUD_ADC_PDM_L_LEN)-1)<<AUD_ADC_PDM_L_POS))
#define AUD_ADC_0_SRC                                           AUD_ADC_0_SRC
#define AUD_ADC_0_SRC_POS                                       (12U)
#define AUD_ADC_0_SRC_LEN                                       (1U)
#define AUD_ADC_0_SRC_MSK                                       (((1U<<AUD_ADC_0_SRC_LEN)-1)<<AUD_ADC_0_SRC_POS)
#define AUD_ADC_0_SRC_UMSK                                      (~(((1U<<AUD_ADC_0_SRC_LEN)-1)<<AUD_ADC_0_SRC_POS))

/* 0xC1C : pdm_pdm_0 */
#define AUD_PDM_PDM_0_OFFSET                                    (0xC1C)
#define AUD_PDM_0_EN                                            AUD_PDM_0_EN
#define AUD_PDM_0_EN_POS                                        (0U)
#define AUD_PDM_0_EN_LEN                                        (1U)
#define AUD_PDM_0_EN_MSK                                        (((1U<<AUD_PDM_0_EN_LEN)-1)<<AUD_PDM_0_EN_POS)
#define AUD_PDM_0_EN_UMSK                                       (~(((1U<<AUD_PDM_0_EN_LEN)-1)<<AUD_PDM_0_EN_POS))
#define AUD_ADC_0_PDM_SEL                                       AUD_ADC_0_PDM_SEL
#define AUD_ADC_0_PDM_SEL_POS                                   (3U)
#define AUD_ADC_0_PDM_SEL_LEN                                   (3U)
#define AUD_ADC_0_PDM_SEL_MSK                                   (((1U<<AUD_ADC_0_PDM_SEL_LEN)-1)<<AUD_ADC_0_PDM_SEL_POS)
#define AUD_ADC_0_PDM_SEL_UMSK                                  (~(((1U<<AUD_ADC_0_PDM_SEL_LEN)-1)<<AUD_ADC_0_PDM_SEL_POS))

/* 0xC20 : pdm_rsvd0 */
#define AUD_PDM_RSVD0_OFFSET                                    (0xC20)

/* 0xC24 : pdm_dbg_0 */
#define AUD_PDM_DBG_0_OFFSET                                    (0xC24)
#define AUD_ADC_TEST_CLKIN_EN                                   AUD_ADC_TEST_CLKIN_EN
#define AUD_ADC_TEST_CLKIN_EN_POS                               (21U)
#define AUD_ADC_TEST_CLKIN_EN_LEN                               (1U)
#define AUD_ADC_TEST_CLKIN_EN_MSK                               (((1U<<AUD_ADC_TEST_CLKIN_EN_LEN)-1)<<AUD_ADC_TEST_CLKIN_EN_POS)
#define AUD_ADC_TEST_CLKIN_EN_UMSK                              (~(((1U<<AUD_ADC_TEST_CLKIN_EN_LEN)-1)<<AUD_ADC_TEST_CLKIN_EN_POS))
#define AUD_ADC_TEST_DIN_EN                                     AUD_ADC_TEST_DIN_EN
#define AUD_ADC_TEST_DIN_EN_POS                                 (23U)
#define AUD_ADC_TEST_DIN_EN_LEN                                 (1U)
#define AUD_ADC_TEST_DIN_EN_MSK                                 (((1U<<AUD_ADC_TEST_DIN_EN_LEN)-1)<<AUD_ADC_TEST_DIN_EN_POS)
#define AUD_ADC_TEST_DIN_EN_UMSK                                (~(((1U<<AUD_ADC_TEST_DIN_EN_LEN)-1)<<AUD_ADC_TEST_DIN_EN_POS))
#define AUD_TEST_READ_SEL                                       AUD_TEST_READ_SEL
#define AUD_TEST_READ_SEL_POS                                   (24U)
#define AUD_TEST_READ_SEL_LEN                                   (6U)
#define AUD_TEST_READ_SEL_MSK                                   (((1U<<AUD_TEST_READ_SEL_LEN)-1)<<AUD_TEST_READ_SEL_POS)
#define AUD_TEST_READ_SEL_UMSK                                  (~(((1U<<AUD_TEST_READ_SEL_LEN)-1)<<AUD_TEST_READ_SEL_POS))

/* 0xC28 : pdm_dbg_1 */
#define AUD_PDM_DBG_1_OFFSET                                    (0xC28)
#define AUD_TEST_READ                                           AUD_TEST_READ
#define AUD_TEST_READ_POS                                       (0U)
#define AUD_TEST_READ_LEN                                       (32U)
#define AUD_TEST_READ_MSK                                       (((1U<<AUD_TEST_READ_LEN)-1)<<AUD_TEST_READ_POS)
#define AUD_TEST_READ_UMSK                                      (~(((1U<<AUD_TEST_READ_LEN)-1)<<AUD_TEST_READ_POS))

/* 0xC2C : pdm_dbg_2 */
#define AUD_PDM_DBG_2_OFFSET                                    (0xC2C)
#define AUD_ADC_FIR_4S_VAL                                      AUD_ADC_FIR_4S_VAL
#define AUD_ADC_FIR_4S_VAL_POS                                  (0U)
#define AUD_ADC_FIR_4S_VAL_LEN                                  (20U)
#define AUD_ADC_FIR_4S_VAL_MSK                                  (((1U<<AUD_ADC_FIR_4S_VAL_LEN)-1)<<AUD_ADC_FIR_4S_VAL_POS)
#define AUD_ADC_FIR_4S_VAL_UMSK                                 (~(((1U<<AUD_ADC_FIR_4S_VAL_LEN)-1)<<AUD_ADC_FIR_4S_VAL_POS))
#define AUD_ADC_0_FIR_4S_EN                                     AUD_ADC_0_FIR_4S_EN
#define AUD_ADC_0_FIR_4S_EN_POS                                 (20U)
#define AUD_ADC_0_FIR_4S_EN_LEN                                 (1U)
#define AUD_ADC_0_FIR_4S_EN_MSK                                 (((1U<<AUD_ADC_0_FIR_4S_EN_LEN)-1)<<AUD_ADC_0_FIR_4S_EN_POS)
#define AUD_ADC_0_FIR_4S_EN_UMSK                                (~(((1U<<AUD_ADC_0_FIR_4S_EN_LEN)-1)<<AUD_ADC_0_FIR_4S_EN_POS))

/* 0xC30 : pdm_dbg_3 */
#define AUD_PDM_DBG_3_OFFSET                                    (0xC30)

/* 0xC34 : pdm_dbg_4 */
#define AUD_PDM_DBG_4_OFFSET                                    (0xC34)

/* 0xC38 : pdm_adc_s0 */
#define AUD_PDM_ADC_S0_OFFSET                                   (0xC38)
#define AUD_ADC_S0_VOLUME                                       AUD_ADC_S0_VOLUME
#define AUD_ADC_S0_VOLUME_POS                                   (0U)
#define AUD_ADC_S0_VOLUME_LEN                                   (9U)
#define AUD_ADC_S0_VOLUME_MSK                                   (((1U<<AUD_ADC_S0_VOLUME_LEN)-1)<<AUD_ADC_S0_VOLUME_POS)
#define AUD_ADC_S0_VOLUME_UMSK                                  (~(((1U<<AUD_ADC_S0_VOLUME_LEN)-1)<<AUD_ADC_S0_VOLUME_POS))

/* 0xC3C : pdm_adc_s1 */
#define AUD_PDM_ADC_S1_OFFSET                                   (0xC3C)

/* 0xC40 : pdm_adc_s2 */
#define AUD_PDM_ADC_S2_OFFSET                                   (0xC40)

/* 0xC60 : audadc_ana_cfg1 */
#define AUDADC_ANA_CFG1_OFFSET                                  (0xC60)
#define AUDADC_PGA_CHOP_CKSEL                                   AUDADC_PGA_CHOP_CKSEL
#define AUDADC_PGA_CHOP_CKSEL_POS                               (0U)
#define AUDADC_PGA_CHOP_CKSEL_LEN                               (1U)
#define AUDADC_PGA_CHOP_CKSEL_MSK                               (((1U<<AUDADC_PGA_CHOP_CKSEL_LEN)-1)<<AUDADC_PGA_CHOP_CKSEL_POS)
#define AUDADC_PGA_CHOP_CKSEL_UMSK                              (~(((1U<<AUDADC_PGA_CHOP_CKSEL_LEN)-1)<<AUDADC_PGA_CHOP_CKSEL_POS))
#define AUDADC_PGA_CHOP_FREQ                                    AUDADC_PGA_CHOP_FREQ
#define AUDADC_PGA_CHOP_FREQ_POS                                (1U)
#define AUDADC_PGA_CHOP_FREQ_LEN                                (3U)
#define AUDADC_PGA_CHOP_FREQ_MSK                                (((1U<<AUDADC_PGA_CHOP_FREQ_LEN)-1)<<AUDADC_PGA_CHOP_FREQ_POS)
#define AUDADC_PGA_CHOP_FREQ_UMSK                               (~(((1U<<AUDADC_PGA_CHOP_FREQ_LEN)-1)<<AUDADC_PGA_CHOP_FREQ_POS))
#define AUDADC_PGA_CHOP_EN                                      AUDADC_PGA_CHOP_EN
#define AUDADC_PGA_CHOP_EN_POS                                  (4U)
#define AUDADC_PGA_CHOP_EN_LEN                                  (1U)
#define AUDADC_PGA_CHOP_EN_MSK                                  (((1U<<AUDADC_PGA_CHOP_EN_LEN)-1)<<AUDADC_PGA_CHOP_EN_POS)
#define AUDADC_PGA_CHOP_EN_UMSK                                 (~(((1U<<AUDADC_PGA_CHOP_EN_LEN)-1)<<AUDADC_PGA_CHOP_EN_POS))
#define AUDADC_PGA_CHOP_CFG                                     AUDADC_PGA_CHOP_CFG
#define AUDADC_PGA_CHOP_CFG_POS                                 (5U)
#define AUDADC_PGA_CHOP_CFG_LEN                                 (2U)
#define AUDADC_PGA_CHOP_CFG_MSK                                 (((1U<<AUDADC_PGA_CHOP_CFG_LEN)-1)<<AUDADC_PGA_CHOP_CFG_POS)
#define AUDADC_PGA_CHOP_CFG_UMSK                                (~(((1U<<AUDADC_PGA_CHOP_CFG_LEN)-1)<<AUDADC_PGA_CHOP_CFG_POS))
#define AUDADC_PGA_RHPAS_SEL                                    AUDADC_PGA_RHPAS_SEL
#define AUDADC_PGA_RHPAS_SEL_POS                                (8U)
#define AUDADC_PGA_RHPAS_SEL_LEN                                (2U)
#define AUDADC_PGA_RHPAS_SEL_MSK                                (((1U<<AUDADC_PGA_RHPAS_SEL_LEN)-1)<<AUDADC_PGA_RHPAS_SEL_POS)
#define AUDADC_PGA_RHPAS_SEL_UMSK                               (~(((1U<<AUDADC_PGA_RHPAS_SEL_LEN)-1)<<AUDADC_PGA_RHPAS_SEL_POS))
#define AUDADC_PGA_NOIS_CTRL                                    AUDADC_PGA_NOIS_CTRL
#define AUDADC_PGA_NOIS_CTRL_POS                                (12U)
#define AUDADC_PGA_NOIS_CTRL_LEN                                (2U)
#define AUDADC_PGA_NOIS_CTRL_MSK                                (((1U<<AUDADC_PGA_NOIS_CTRL_LEN)-1)<<AUDADC_PGA_NOIS_CTRL_POS)
#define AUDADC_PGA_NOIS_CTRL_UMSK                               (~(((1U<<AUDADC_PGA_NOIS_CTRL_LEN)-1)<<AUDADC_PGA_NOIS_CTRL_POS))
#define AUDADC_ICTRL_PGA_AAF                                    AUDADC_ICTRL_PGA_AAF
#define AUDADC_ICTRL_PGA_AAF_POS                                (16U)
#define AUDADC_ICTRL_PGA_AAF_LEN                                (2U)
#define AUDADC_ICTRL_PGA_AAF_MSK                                (((1U<<AUDADC_ICTRL_PGA_AAF_LEN)-1)<<AUDADC_ICTRL_PGA_AAF_POS)
#define AUDADC_ICTRL_PGA_AAF_UMSK                               (~(((1U<<AUDADC_ICTRL_PGA_AAF_LEN)-1)<<AUDADC_ICTRL_PGA_AAF_POS))
#define AUDADC_ICTRL_PGA_MIC                                    AUDADC_ICTRL_PGA_MIC
#define AUDADC_ICTRL_PGA_MIC_POS                                (20U)
#define AUDADC_ICTRL_PGA_MIC_LEN                                (2U)
#define AUDADC_ICTRL_PGA_MIC_MSK                                (((1U<<AUDADC_ICTRL_PGA_MIC_LEN)-1)<<AUDADC_ICTRL_PGA_MIC_POS)
#define AUDADC_ICTRL_PGA_MIC_UMSK                               (~(((1U<<AUDADC_ICTRL_PGA_MIC_LEN)-1)<<AUDADC_ICTRL_PGA_MIC_POS))
#define AUDADC_PGA_LP_EN                                        AUDADC_PGA_LP_EN
#define AUDADC_PGA_LP_EN_POS                                    (24U)
#define AUDADC_PGA_LP_EN_LEN                                    (1U)
#define AUDADC_PGA_LP_EN_MSK                                    (((1U<<AUDADC_PGA_LP_EN_LEN)-1)<<AUDADC_PGA_LP_EN_POS)
#define AUDADC_PGA_LP_EN_UMSK                                   (~(((1U<<AUDADC_PGA_LP_EN_LEN)-1)<<AUDADC_PGA_LP_EN_POS))
#define AUDADC_CKB_EN                                           AUDADC_CKB_EN
#define AUDADC_CKB_EN_POS                                       (28U)
#define AUDADC_CKB_EN_LEN                                       (1U)
#define AUDADC_CKB_EN_MSK                                       (((1U<<AUDADC_CKB_EN_LEN)-1)<<AUDADC_CKB_EN_POS)
#define AUDADC_CKB_EN_UMSK                                      (~(((1U<<AUDADC_CKB_EN_LEN)-1)<<AUDADC_CKB_EN_POS))
#define AUDADC_SEL_EDGE                                         AUDADC_SEL_EDGE
#define AUDADC_SEL_EDGE_POS                                     (29U)
#define AUDADC_SEL_EDGE_LEN                                     (1U)
#define AUDADC_SEL_EDGE_MSK                                     (((1U<<AUDADC_SEL_EDGE_LEN)-1)<<AUDADC_SEL_EDGE_POS)
#define AUDADC_SEL_EDGE_UMSK                                    (~(((1U<<AUDADC_SEL_EDGE_LEN)-1)<<AUDADC_SEL_EDGE_POS))

/* 0xC64 : audadc_ana_cfg2 */
#define AUDADC_ANA_CFG2_OFFSET                                  (0xC64)
#define AUDADC_DITHER_ORDER                                     AUDADC_DITHER_ORDER
#define AUDADC_DITHER_ORDER_POS                                 (0U)
#define AUDADC_DITHER_ORDER_LEN                                 (1U)
#define AUDADC_DITHER_ORDER_MSK                                 (((1U<<AUDADC_DITHER_ORDER_LEN)-1)<<AUDADC_DITHER_ORDER_POS)
#define AUDADC_DITHER_ORDER_UMSK                                (~(((1U<<AUDADC_DITHER_ORDER_LEN)-1)<<AUDADC_DITHER_ORDER_POS))
#define AUDADC_DITHER_SEL                                       AUDADC_DITHER_SEL
#define AUDADC_DITHER_SEL_POS                                   (1U)
#define AUDADC_DITHER_SEL_LEN                                   (2U)
#define AUDADC_DITHER_SEL_MSK                                   (((1U<<AUDADC_DITHER_SEL_LEN)-1)<<AUDADC_DITHER_SEL_POS)
#define AUDADC_DITHER_SEL_UMSK                                  (~(((1U<<AUDADC_DITHER_SEL_LEN)-1)<<AUDADC_DITHER_SEL_POS))
#define AUDADC_DITHER_ENA                                       AUDADC_DITHER_ENA
#define AUDADC_DITHER_ENA_POS                                   (3U)
#define AUDADC_DITHER_ENA_LEN                                   (1U)
#define AUDADC_DITHER_ENA_MSK                                   (((1U<<AUDADC_DITHER_ENA_LEN)-1)<<AUDADC_DITHER_ENA_POS)
#define AUDADC_DITHER_ENA_UMSK                                  (~(((1U<<AUDADC_DITHER_ENA_LEN)-1)<<AUDADC_DITHER_ENA_POS))
#define AUDADC_QUAN_GAIN                                        AUDADC_QUAN_GAIN
#define AUDADC_QUAN_GAIN_POS                                    (4U)
#define AUDADC_QUAN_GAIN_LEN                                    (2U)
#define AUDADC_QUAN_GAIN_MSK                                    (((1U<<AUDADC_QUAN_GAIN_LEN)-1)<<AUDADC_QUAN_GAIN_POS)
#define AUDADC_QUAN_GAIN_UMSK                                   (~(((1U<<AUDADC_QUAN_GAIN_LEN)-1)<<AUDADC_QUAN_GAIN_POS))
#define AUDADC_DEM_EN                                           AUDADC_DEM_EN
#define AUDADC_DEM_EN_POS                                       (8U)
#define AUDADC_DEM_EN_LEN                                       (1U)
#define AUDADC_DEM_EN_MSK                                       (((1U<<AUDADC_DEM_EN_LEN)-1)<<AUDADC_DEM_EN_POS)
#define AUDADC_DEM_EN_UMSK                                      (~(((1U<<AUDADC_DEM_EN_LEN)-1)<<AUDADC_DEM_EN_POS))
#define AUDADC_NCTRL_ADC2                                       AUDADC_NCTRL_ADC2
#define AUDADC_NCTRL_ADC2_POS                                   (12U)
#define AUDADC_NCTRL_ADC2_LEN                                   (2U)
#define AUDADC_NCTRL_ADC2_MSK                                   (((1U<<AUDADC_NCTRL_ADC2_LEN)-1)<<AUDADC_NCTRL_ADC2_POS)
#define AUDADC_NCTRL_ADC2_UMSK                                  (~(((1U<<AUDADC_NCTRL_ADC2_LEN)-1)<<AUDADC_NCTRL_ADC2_POS))
#define AUDADC_NCTRL_ADC1                                       AUDADC_NCTRL_ADC1
#define AUDADC_NCTRL_ADC1_POS                                   (16U)
#define AUDADC_NCTRL_ADC1_LEN                                   (3U)
#define AUDADC_NCTRL_ADC1_MSK                                   (((1U<<AUDADC_NCTRL_ADC1_LEN)-1)<<AUDADC_NCTRL_ADC1_POS)
#define AUDADC_NCTRL_ADC1_UMSK                                  (~(((1U<<AUDADC_NCTRL_ADC1_LEN)-1)<<AUDADC_NCTRL_ADC1_POS))
#define AUDADC_ICTRL_ADC                                        AUDADC_ICTRL_ADC
#define AUDADC_ICTRL_ADC_POS                                    (20U)
#define AUDADC_ICTRL_ADC_LEN                                    (2U)
#define AUDADC_ICTRL_ADC_MSK                                    (((1U<<AUDADC_ICTRL_ADC_LEN)-1)<<AUDADC_ICTRL_ADC_POS)
#define AUDADC_ICTRL_ADC_UMSK                                   (~(((1U<<AUDADC_ICTRL_ADC_LEN)-1)<<AUDADC_ICTRL_ADC_POS))
#define AUDADC_SDM_LP_EN                                        AUDADC_SDM_LP_EN
#define AUDADC_SDM_LP_EN_POS                                    (24U)
#define AUDADC_SDM_LP_EN_LEN                                    (1U)
#define AUDADC_SDM_LP_EN_MSK                                    (((1U<<AUDADC_SDM_LP_EN_LEN)-1)<<AUDADC_SDM_LP_EN_POS)
#define AUDADC_SDM_LP_EN_UMSK                                   (~(((1U<<AUDADC_SDM_LP_EN_LEN)-1)<<AUDADC_SDM_LP_EN_POS))
#define AUDADC_RESERVED                                         AUDADC_RESERVED
#define AUDADC_RESERVED_POS                                     (28U)
#define AUDADC_RESERVED_LEN                                     (2U)
#define AUDADC_RESERVED_MSK                                     (((1U<<AUDADC_RESERVED_LEN)-1)<<AUDADC_RESERVED_POS)
#define AUDADC_RESERVED_UMSK                                    (~(((1U<<AUDADC_RESERVED_LEN)-1)<<AUDADC_RESERVED_POS))

/* 0xC68 : audadc_cmd */
#define AUDADC_CMD_OFFSET                                       (0xC68)
#define AUDADC_MEAS_ODR_SEL                                     AUDADC_MEAS_ODR_SEL
#define AUDADC_MEAS_ODR_SEL_POS                                 (0U)
#define AUDADC_MEAS_ODR_SEL_LEN                                 (4U)
#define AUDADC_MEAS_ODR_SEL_MSK                                 (((1U<<AUDADC_MEAS_ODR_SEL_LEN)-1)<<AUDADC_MEAS_ODR_SEL_POS)
#define AUDADC_MEAS_ODR_SEL_UMSK                                (~(((1U<<AUDADC_MEAS_ODR_SEL_LEN)-1)<<AUDADC_MEAS_ODR_SEL_POS))
#define AUDADC_MEAS_FILTER_TYPE                                 AUDADC_MEAS_FILTER_TYPE
#define AUDADC_MEAS_FILTER_TYPE_POS                             (4U)
#define AUDADC_MEAS_FILTER_TYPE_LEN                             (1U)
#define AUDADC_MEAS_FILTER_TYPE_MSK                             (((1U<<AUDADC_MEAS_FILTER_TYPE_LEN)-1)<<AUDADC_MEAS_FILTER_TYPE_POS)
#define AUDADC_MEAS_FILTER_TYPE_UMSK                            (~(((1U<<AUDADC_MEAS_FILTER_TYPE_LEN)-1)<<AUDADC_MEAS_FILTER_TYPE_POS))
#define AUDADC_MEAS_FILTER_EN                                   AUDADC_MEAS_FILTER_EN
#define AUDADC_MEAS_FILTER_EN_POS                               (5U)
#define AUDADC_MEAS_FILTER_EN_LEN                               (1U)
#define AUDADC_MEAS_FILTER_EN_MSK                               (((1U<<AUDADC_MEAS_FILTER_EN_LEN)-1)<<AUDADC_MEAS_FILTER_EN_POS)
#define AUDADC_MEAS_FILTER_EN_UMSK                              (~(((1U<<AUDADC_MEAS_FILTER_EN_LEN)-1)<<AUDADC_MEAS_FILTER_EN_POS))
#define AUDADC_AUDIO_OSR_SEL                                    AUDADC_AUDIO_OSR_SEL
#define AUDADC_AUDIO_OSR_SEL_POS                                (6U)
#define AUDADC_AUDIO_OSR_SEL_LEN                                (1U)
#define AUDADC_AUDIO_OSR_SEL_MSK                                (((1U<<AUDADC_AUDIO_OSR_SEL_LEN)-1)<<AUDADC_AUDIO_OSR_SEL_POS)
#define AUDADC_AUDIO_OSR_SEL_UMSK                               (~(((1U<<AUDADC_AUDIO_OSR_SEL_LEN)-1)<<AUDADC_AUDIO_OSR_SEL_POS))
#define AUDADC_PGA_GAIN                                         AUDADC_PGA_GAIN
#define AUDADC_PGA_GAIN_POS                                     (8U)
#define AUDADC_PGA_GAIN_LEN                                     (4U)
#define AUDADC_PGA_GAIN_MSK                                     (((1U<<AUDADC_PGA_GAIN_LEN)-1)<<AUDADC_PGA_GAIN_POS)
#define AUDADC_PGA_GAIN_UMSK                                    (~(((1U<<AUDADC_PGA_GAIN_LEN)-1)<<AUDADC_PGA_GAIN_POS))
#define AUDADC_PGA_MODE                                         AUDADC_PGA_MODE
#define AUDADC_PGA_MODE_POS                                     (12U)
#define AUDADC_PGA_MODE_LEN                                     (2U)
#define AUDADC_PGA_MODE_MSK                                     (((1U<<AUDADC_PGA_MODE_LEN)-1)<<AUDADC_PGA_MODE_POS)
#define AUDADC_PGA_MODE_UMSK                                    (~(((1U<<AUDADC_PGA_MODE_LEN)-1)<<AUDADC_PGA_MODE_POS))
#define AUDADC_CHANNEL_SELN                                     AUDADC_CHANNEL_SELN
#define AUDADC_CHANNEL_SELN_POS                                 (16U)
#define AUDADC_CHANNEL_SELN_LEN                                 (3U)
#define AUDADC_CHANNEL_SELN_MSK                                 (((1U<<AUDADC_CHANNEL_SELN_LEN)-1)<<AUDADC_CHANNEL_SELN_POS)
#define AUDADC_CHANNEL_SELN_UMSK                                (~(((1U<<AUDADC_CHANNEL_SELN_LEN)-1)<<AUDADC_CHANNEL_SELN_POS))
#define AUDADC_CHANNEL_SELP                                     AUDADC_CHANNEL_SELP
#define AUDADC_CHANNEL_SELP_POS                                 (20U)
#define AUDADC_CHANNEL_SELP_LEN                                 (3U)
#define AUDADC_CHANNEL_SELP_MSK                                 (((1U<<AUDADC_CHANNEL_SELP_LEN)-1)<<AUDADC_CHANNEL_SELP_POS)
#define AUDADC_CHANNEL_SELP_UMSK                                (~(((1U<<AUDADC_CHANNEL_SELP_LEN)-1)<<AUDADC_CHANNEL_SELP_POS))
#define AUDADC_CHANNEL_EN                                       AUDADC_CHANNEL_EN
#define AUDADC_CHANNEL_EN_POS                                   (24U)
#define AUDADC_CHANNEL_EN_LEN                                   (2U)
#define AUDADC_CHANNEL_EN_MSK                                   (((1U<<AUDADC_CHANNEL_EN_LEN)-1)<<AUDADC_CHANNEL_EN_POS)
#define AUDADC_CHANNEL_EN_UMSK                                  (~(((1U<<AUDADC_CHANNEL_EN_LEN)-1)<<AUDADC_CHANNEL_EN_POS))
#define AUDADC_CONV                                             AUDADC_CONV
#define AUDADC_CONV_POS                                         (28U)
#define AUDADC_CONV_LEN                                         (1U)
#define AUDADC_CONV_MSK                                         (((1U<<AUDADC_CONV_LEN)-1)<<AUDADC_CONV_POS)
#define AUDADC_CONV_UMSK                                        (~(((1U<<AUDADC_CONV_LEN)-1)<<AUDADC_CONV_POS))
#define AUDADC_SDM_PU                                           AUDADC_SDM_PU
#define AUDADC_SDM_PU_POS                                       (29U)
#define AUDADC_SDM_PU_LEN                                       (1U)
#define AUDADC_SDM_PU_MSK                                       (((1U<<AUDADC_SDM_PU_LEN)-1)<<AUDADC_SDM_PU_POS)
#define AUDADC_SDM_PU_UMSK                                      (~(((1U<<AUDADC_SDM_PU_LEN)-1)<<AUDADC_SDM_PU_POS))
#define AUDADC_PGA_PU                                           AUDADC_PGA_PU
#define AUDADC_PGA_PU_POS                                       (30U)
#define AUDADC_PGA_PU_LEN                                       (1U)
#define AUDADC_PGA_PU_MSK                                       (((1U<<AUDADC_PGA_PU_LEN)-1)<<AUDADC_PGA_PU_POS)
#define AUDADC_PGA_PU_UMSK                                      (~(((1U<<AUDADC_PGA_PU_LEN)-1)<<AUDADC_PGA_PU_POS))

/* 0xC6C : audadc_data */
#define AUDADC_DATA_OFFSET                                      (0xC6C)
#define AUDADC_RAW_DATA                                         AUDADC_RAW_DATA
#define AUDADC_RAW_DATA_POS                                     (0U)
#define AUDADC_RAW_DATA_LEN                                     (24U)
#define AUDADC_RAW_DATA_MSK                                     (((1U<<AUDADC_RAW_DATA_LEN)-1)<<AUDADC_RAW_DATA_POS)
#define AUDADC_RAW_DATA_UMSK                                    (~(((1U<<AUDADC_RAW_DATA_LEN)-1)<<AUDADC_RAW_DATA_POS))
#define AUDADC_DATA_RDY                                         AUDADC_DATA_RDY
#define AUDADC_DATA_RDY_POS                                     (24U)
#define AUDADC_DATA_RDY_LEN                                     (1U)
#define AUDADC_DATA_RDY_MSK                                     (((1U<<AUDADC_DATA_RDY_LEN)-1)<<AUDADC_DATA_RDY_POS)
#define AUDADC_DATA_RDY_UMSK                                    (~(((1U<<AUDADC_DATA_RDY_LEN)-1)<<AUDADC_DATA_RDY_POS))
#define AUDADC_SOFT_RST                                         AUDADC_SOFT_RST
#define AUDADC_SOFT_RST_POS                                     (29U)
#define AUDADC_SOFT_RST_LEN                                     (1U)
#define AUDADC_SOFT_RST_MSK                                     (((1U<<AUDADC_SOFT_RST_LEN)-1)<<AUDADC_SOFT_RST_POS)
#define AUDADC_SOFT_RST_UMSK                                    (~(((1U<<AUDADC_SOFT_RST_LEN)-1)<<AUDADC_SOFT_RST_POS))
#define AUDADC_VALID_4S_VAL                                     AUDADC_VALID_4S_VAL
#define AUDADC_VALID_4S_VAL_POS                                 (30U)
#define AUDADC_VALID_4S_VAL_LEN                                 (1U)
#define AUDADC_VALID_4S_VAL_MSK                                 (((1U<<AUDADC_VALID_4S_VAL_LEN)-1)<<AUDADC_VALID_4S_VAL_POS)
#define AUDADC_VALID_4S_VAL_UMSK                                (~(((1U<<AUDADC_VALID_4S_VAL_LEN)-1)<<AUDADC_VALID_4S_VAL_POS))
#define AUDADC_VALID_4S_EN                                      AUDADC_VALID_4S_EN
#define AUDADC_VALID_4S_EN_POS                                  (31U)
#define AUDADC_VALID_4S_EN_LEN                                  (1U)
#define AUDADC_VALID_4S_EN_MSK                                  (((1U<<AUDADC_VALID_4S_EN_LEN)-1)<<AUDADC_VALID_4S_EN_POS)
#define AUDADC_VALID_4S_EN_UMSK                                 (~(((1U<<AUDADC_VALID_4S_EN_LEN)-1)<<AUDADC_VALID_4S_EN_POS))

/* 0xC80 : audadc_rx_fifo_ctrl */
#define AUDADC_RX_FIFO_CTRL_OFFSET                              (0xC80)
#define AUD_RX_FIFO_FLUSH                                       AUD_RX_FIFO_FLUSH
#define AUD_RX_FIFO_FLUSH_POS                                   (0U)
#define AUD_RX_FIFO_FLUSH_LEN                                   (1U)
#define AUD_RX_FIFO_FLUSH_MSK                                   (((1U<<AUD_RX_FIFO_FLUSH_LEN)-1)<<AUD_RX_FIFO_FLUSH_POS)
#define AUD_RX_FIFO_FLUSH_UMSK                                  (~(((1U<<AUD_RX_FIFO_FLUSH_LEN)-1)<<AUD_RX_FIFO_FLUSH_POS))
#define AUD_RXO_INT_EN                                          AUD_RXO_INT_EN
#define AUD_RXO_INT_EN_POS                                      (1U)
#define AUD_RXO_INT_EN_LEN                                      (1U)
#define AUD_RXO_INT_EN_MSK                                      (((1U<<AUD_RXO_INT_EN_LEN)-1)<<AUD_RXO_INT_EN_POS)
#define AUD_RXO_INT_EN_UMSK                                     (~(((1U<<AUD_RXO_INT_EN_LEN)-1)<<AUD_RXO_INT_EN_POS))
#define AUD_RXU_INT_EN                                          AUD_RXU_INT_EN
#define AUD_RXU_INT_EN_POS                                      (2U)
#define AUD_RXU_INT_EN_LEN                                      (1U)
#define AUD_RXU_INT_EN_MSK                                      (((1U<<AUD_RXU_INT_EN_LEN)-1)<<AUD_RXU_INT_EN_POS)
#define AUD_RXU_INT_EN_UMSK                                     (~(((1U<<AUD_RXU_INT_EN_LEN)-1)<<AUD_RXU_INT_EN_POS))
#define AUD_RXA_INT_EN                                          AUD_RXA_INT_EN
#define AUD_RXA_INT_EN_POS                                      (3U)
#define AUD_RXA_INT_EN_LEN                                      (1U)
#define AUD_RXA_INT_EN_MSK                                      (((1U<<AUD_RXA_INT_EN_LEN)-1)<<AUD_RXA_INT_EN_POS)
#define AUD_RXA_INT_EN_UMSK                                     (~(((1U<<AUD_RXA_INT_EN_LEN)-1)<<AUD_RXA_INT_EN_POS))
#define AUD_RX_DRQ_EN                                           AUD_RX_DRQ_EN
#define AUD_RX_DRQ_EN_POS                                       (4U)
#define AUD_RX_DRQ_EN_LEN                                       (1U)
#define AUD_RX_DRQ_EN_MSK                                       (((1U<<AUD_RX_DRQ_EN_LEN)-1)<<AUD_RX_DRQ_EN_POS)
#define AUD_RX_DRQ_EN_UMSK                                      (~(((1U<<AUD_RX_DRQ_EN_LEN)-1)<<AUD_RX_DRQ_EN_POS))
#define AUD_RX_DATA_RES                                         AUD_RX_DATA_RES
#define AUD_RX_DATA_RES_POS                                     (5U)
#define AUD_RX_DATA_RES_LEN                                     (2U)
#define AUD_RX_DATA_RES_MSK                                     (((1U<<AUD_RX_DATA_RES_LEN)-1)<<AUD_RX_DATA_RES_POS)
#define AUD_RX_DATA_RES_UMSK                                    (~(((1U<<AUD_RX_DATA_RES_LEN)-1)<<AUD_RX_DATA_RES_POS))
#define AUD_RX_CH_EN                                            AUD_RX_CH_EN
#define AUD_RX_CH_EN_POS                                        (8U)
#define AUD_RX_CH_EN_LEN                                        (1U)
#define AUD_RX_CH_EN_MSK                                        (((1U<<AUD_RX_CH_EN_LEN)-1)<<AUD_RX_CH_EN_POS)
#define AUD_RX_CH_EN_UMSK                                       (~(((1U<<AUD_RX_CH_EN_LEN)-1)<<AUD_RX_CH_EN_POS))
#define AUD_RX_DRQ_CNT                                          AUD_RX_DRQ_CNT
#define AUD_RX_DRQ_CNT_POS                                      (14U)
#define AUD_RX_DRQ_CNT_LEN                                      (2U)
#define AUD_RX_DRQ_CNT_MSK                                      (((1U<<AUD_RX_DRQ_CNT_LEN)-1)<<AUD_RX_DRQ_CNT_POS)
#define AUD_RX_DRQ_CNT_UMSK                                     (~(((1U<<AUD_RX_DRQ_CNT_LEN)-1)<<AUD_RX_DRQ_CNT_POS))
#define AUD_RX_TRG_LEVEL                                        AUD_RX_TRG_LEVEL
#define AUD_RX_TRG_LEVEL_POS                                    (16U)
#define AUD_RX_TRG_LEVEL_LEN                                    (4U)
#define AUD_RX_TRG_LEVEL_MSK                                    (((1U<<AUD_RX_TRG_LEVEL_LEN)-1)<<AUD_RX_TRG_LEVEL_POS)
#define AUD_RX_TRG_LEVEL_UMSK                                   (~(((1U<<AUD_RX_TRG_LEVEL_LEN)-1)<<AUD_RX_TRG_LEVEL_POS))
#define AUD_RX_DATA_MODE                                        AUD_RX_DATA_MODE
#define AUD_RX_DATA_MODE_POS                                    (24U)
#define AUD_RX_DATA_MODE_LEN                                    (2U)
#define AUD_RX_DATA_MODE_MSK                                    (((1U<<AUD_RX_DATA_MODE_LEN)-1)<<AUD_RX_DATA_MODE_POS)
#define AUD_RX_DATA_MODE_UMSK                                   (~(((1U<<AUD_RX_DATA_MODE_LEN)-1)<<AUD_RX_DATA_MODE_POS))

/* 0xC84 : audadc_rx_fifo_status */
#define AUDADC_RX_FIFO_STATUS_OFFSET                            (0xC84)
#define AUD_RXO_INT                                             AUD_RXO_INT
#define AUD_RXO_INT_POS                                         (1U)
#define AUD_RXO_INT_LEN                                         (1U)
#define AUD_RXO_INT_MSK                                         (((1U<<AUD_RXO_INT_LEN)-1)<<AUD_RXO_INT_POS)
#define AUD_RXO_INT_UMSK                                        (~(((1U<<AUD_RXO_INT_LEN)-1)<<AUD_RXO_INT_POS))
#define AUD_RXU_INT                                             AUD_RXU_INT
#define AUD_RXU_INT_POS                                         (2U)
#define AUD_RXU_INT_LEN                                         (1U)
#define AUD_RXU_INT_MSK                                         (((1U<<AUD_RXU_INT_LEN)-1)<<AUD_RXU_INT_POS)
#define AUD_RXU_INT_UMSK                                        (~(((1U<<AUD_RXU_INT_LEN)-1)<<AUD_RXU_INT_POS))
#define AUD_RXA_INT                                             AUD_RXA_INT
#define AUD_RXA_INT_POS                                         (4U)
#define AUD_RXA_INT_LEN                                         (1U)
#define AUD_RXA_INT_MSK                                         (((1U<<AUD_RXA_INT_LEN)-1)<<AUD_RXA_INT_POS)
#define AUD_RXA_INT_UMSK                                        (~(((1U<<AUD_RXA_INT_LEN)-1)<<AUD_RXA_INT_POS))
#define AUD_RXA_CNT                                             AUD_RXA_CNT
#define AUD_RXA_CNT_POS                                         (16U)
#define AUD_RXA_CNT_LEN                                         (4U)
#define AUD_RXA_CNT_MSK                                         (((1U<<AUD_RXA_CNT_LEN)-1)<<AUD_RXA_CNT_POS)
#define AUD_RXA_CNT_UMSK                                        (~(((1U<<AUD_RXA_CNT_LEN)-1)<<AUD_RXA_CNT_POS))
#define AUD_RXA                                                 AUD_RXA
#define AUD_RXA_POS                                             (24U)
#define AUD_RXA_LEN                                             (1U)
#define AUD_RXA_MSK                                             (((1U<<AUD_RXA_LEN)-1)<<AUD_RXA_POS)
#define AUD_RXA_UMSK                                            (~(((1U<<AUD_RXA_LEN)-1)<<AUD_RXA_POS))

/* 0xC88 : audadc_rx_fifo_data */
#define AUDADC_RX_FIFO_DATA_OFFSET                              (0xC88)
#define AUD_RX_DATA                                             AUD_RX_DATA
#define AUD_RX_DATA_POS                                         (0U)
#define AUD_RX_DATA_LEN                                         (32U)
#define AUD_RX_DATA_MSK                                         (((1U<<AUD_RX_DATA_LEN)-1)<<AUD_RX_DATA_POS)
#define AUD_RX_DATA_UMSK                                        (~(((1U<<AUD_RX_DATA_LEN)-1)<<AUD_RX_DATA_POS))


struct  aud_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[3072];

    /* 0xC00 : audpdm_top */
    union {
        struct {
            uint32_t audio_ckg_en                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t adc_itf_inv_sel                :  1; /* [    2],        r/w,        0x0 */
            uint32_t pdm_itf_inv_sel                :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_27                  : 24; /* [27: 4],       rsvd,        0x0 */
            uint32_t adc_rate                       :  4; /* [31:28],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } audpdm_top;

    /* 0xC04 : audpdm_itf */
    union {
        struct {
            uint32_t adc_0_en                       :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_29                  : 29; /* [29: 1],       rsvd,        0x0 */
            uint32_t adc_itf_en                     :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } audpdm_itf;

    /* 0xC08 : pdm_adc_0 */
    union {
        struct {
            uint32_t adc_0_fir_mode                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_adc_0;

    /* 0xC0C : pdm_adc_1 */
    union {
        struct {
            uint32_t adc_0_k1                       :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t adc_0_k1_en                    :  1; /* [    4],        r/w,        0x1 */
            uint32_t adc_0_k2                       :  4; /* [ 8: 5],        r/w,        0xd */
            uint32_t adc_0_k2_en                    :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_adc_1;

    /* 0xC10 : pdm_dac_0 */
    union {
        struct {
            uint32_t adc_pdm_h                      :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t reserved_4_5                   :  2; /* [ 5: 4],       rsvd,        0x0 */
            uint32_t adc_pdm_l                      :  4; /* [ 9: 6],        r/w,        0xa */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adc_0_src                      :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_dac_0;

    /* 0xc14  reserved */
    uint8_t RESERVED0xc14[8];

    /* 0xC1C : pdm_pdm_0 */
    union {
        struct {
            uint32_t pdm_0_en                       :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_2                   :  2; /* [ 2: 1],       rsvd,        0x0 */
            uint32_t adc_0_pdm_sel                  :  3; /* [ 5: 3],        r/w,        0x0 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_pdm_0;

    /* 0xC20 : pdm_rsvd0 */
    union {
        struct {
            uint32_t rsvd0                          : 32; /* [31: 0],        r/w,     0xffff */
        }BF;
        uint32_t WORD;
    } pdm_rsvd0;

    /* 0xC24 : pdm_dbg_0 */
    union {
        struct {
            uint32_t reserved_0_20                  : 21; /* [20: 0],       rsvd,        0x0 */
            uint32_t adc_test_clkin_en              :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22                    :  1; /* [   22],       rsvd,        0x0 */
            uint32_t adc_test_din_en                :  1; /* [   23],        r/w,        0x0 */
            uint32_t aud_test_read_sel              :  6; /* [29:24],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_dbg_0;

    /* 0xC28 : pdm_dbg_1 */
    union {
        struct {
            uint32_t aud_test_read                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_dbg_1;

    /* 0xC2C : pdm_dbg_2 */
    union {
        struct {
            uint32_t adc_fir_4s_val                 : 20; /* [19: 0],        r/w,        0x0 */
            uint32_t adc_0_fir_4s_en                :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_dbg_2;

    /* 0xC30 : pdm_dbg_3 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_dbg_3;

    /* 0xC34 : pdm_dbg_4 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_dbg_4;

    /* 0xC38 : pdm_adc_s0 */
    union {
        struct {
            uint32_t adc_s0_volume                  :  9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_adc_s0;

    /* 0xC3C : pdm_adc_s1 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_adc_s1;

    /* 0xC40 : pdm_adc_s2 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pdm_adc_s2;

    /* 0xc44  reserved */
    uint8_t RESERVED0xc44[28];

    /* 0xC60 : audadc_ana_cfg1 */
    union {
        struct {
            uint32_t audadc_pga_chop_cksel          :  1; /* [    0],        r/w,        0x0 */
            uint32_t audadc_pga_chop_freq           :  3; /* [ 3: 1],        r/w,        0x4 */
            uint32_t audadc_pga_chop_en             :  1; /* [    4],        r/w,        0x1 */
            uint32_t audadc_pga_chop_cfg            :  2; /* [ 6: 5],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t audadc_pga_rhpas_sel           :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t audadc_pga_nois_ctrl           :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t audadc_ictrl_pga_aaf           :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t audadc_ictrl_pga_mic           :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t audadc_pga_lp_en               :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t audadc_ckb_en                  :  1; /* [   28],        r/w,        0x0 */
            uint32_t audadc_sel_edge                :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } audadc_ana_cfg1;

    /* 0xC64 : audadc_ana_cfg2 */
    union {
        struct {
            uint32_t audadc_dither_order            :  1; /* [    0],        r/w,        0x0 */
            uint32_t audadc_dither_sel              :  2; /* [ 2: 1],        r/w,        0x2 */
            uint32_t audadc_dither_ena              :  1; /* [    3],        r/w,        0x1 */
            uint32_t audadc_quan_gain               :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t audadc_dem_en                  :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t audadc_nctrl_adc2              :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t audadc_nctrl_adc1              :  3; /* [18:16],        r/w,        0x3 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t audadc_ictrl_adc               :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t audadc_sdm_lp_en               :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t audadc_reserved                :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } audadc_ana_cfg2;

    /* 0xC68 : audadc_cmd */
    union {
        struct {
            uint32_t audadc_meas_odr_sel            :  4; /* [ 3: 0],        r/w,        0x3 */
            uint32_t audadc_meas_filter_type        :  1; /* [    4],        r/w,        0x0 */
            uint32_t audadc_meas_filter_en          :  1; /* [    5],        r/w,        0x0 */
            uint32_t audadc_audio_osr_sel           :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t audadc_pga_gain                :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t audadc_pga_mode                :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t audadc_channel_seln            :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t audadc_channel_selp            :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t audadc_channel_en              :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t audadc_conv                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t audadc_sdm_pu                  :  1; /* [   29],        r/w,        0x0 */
            uint32_t audadc_pga_pu                  :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } audadc_cmd;

    /* 0xC6C : audadc_data */
    union {
        struct {
            uint32_t audadc_raw_data                : 24; /* [23: 0],          r,        0x0 */
            uint32_t audadc_data_rdy                :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_28                 :  4; /* [28:25],       rsvd,        0x0 */
            uint32_t audadc_soft_rst                :  1; /* [   29],        r/w,        0x0 */
            uint32_t audadc_valid_4s_val            :  1; /* [   30],        r/w,        0x0 */
            uint32_t audadc_valid_4s_en             :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } audadc_data;

    /* 0xc70  reserved */
    uint8_t RESERVED0xc70[16];

    /* 0xC80 : audadc_rx_fifo_ctrl */
    union {
        struct {
            uint32_t rx_fifo_flush                  :  1; /* [    0],        w1c,        0x0 */
            uint32_t rxo_int_en                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t rxu_int_en                     :  1; /* [    2],        r/w,        0x0 */
            uint32_t rxa_int_en                     :  1; /* [    3],        r/w,        0x0 */
            uint32_t rx_drq_en                      :  1; /* [    4],        r/w,        0x0 */
            uint32_t rx_data_res                    :  2; /* [ 6: 5],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rx_ch_en                       :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_13                  :  5; /* [13: 9],       rsvd,        0x0 */
            uint32_t rx_drq_cnt                     :  2; /* [15:14],        r/w,        0x0 */
            uint32_t rx_trg_level                   :  4; /* [19:16],        r/w,        0x3 */
            uint32_t reserved_20_23                 :  4; /* [23:20],       rsvd,        0x0 */
            uint32_t rx_data_mode                   :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } audadc_rx_fifo_ctrl;

    /* 0xC84 : audadc_rx_fifo_status */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t rxo_int                        :  1; /* [    1],          r,        0x0 */
            uint32_t rxu_int                        :  1; /* [    2],          r,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t rxa_int                        :  1; /* [    4],          r,        0x0 */
            uint32_t reserved_5_15                  : 11; /* [15: 5],       rsvd,        0x0 */
            uint32_t rxa_cnt                        :  4; /* [19:16],          r,        0x0 */
            uint32_t reserved_20_23                 :  4; /* [23:20],       rsvd,        0x0 */
            uint32_t rxa                            :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } audadc_rx_fifo_status;

    /* 0xC88 : audadc_rx_fifo_data */
    union {
        struct {
            uint32_t rx_data                        : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } audadc_rx_fifo_data;

};

typedef volatile struct aud_reg aud_reg_t;


#endif  /* __AUD_REG_H__ */
