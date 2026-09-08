/**
  ******************************************************************************
  * @file    bz_phy_agc_reg.h
  * @version V1.2
  * @date    2020-03-30
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 IntelPro</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of IntelPro nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef  __BZ_PHY_AGC_REG_H__
#define  __BZ_PHY_AGC_REG_H__

#include "ipro7.h"

/* 0xC00 : agc_ctrl_0 */
#define BZ_PHY_AGC_AGC_CTRL_0_OFFSET                            (0xC00)
#define BZ_PHY_AGC_BZ_AGC_EN                                    BZ_PHY_AGC_BZ_AGC_EN
#define BZ_PHY_AGC_BZ_AGC_EN_POS                                (0U)
#define BZ_PHY_AGC_BZ_AGC_EN_LEN                                (1U)
#define BZ_PHY_AGC_BZ_AGC_EN_MSK                                (((1U<<BZ_PHY_AGC_BZ_AGC_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_EN_UMSK                               (~(((1U<<BZ_PHY_AGC_BZ_AGC_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND                          BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_POS                      (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_LEN                      (6U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_MSK                      (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_UMSK                     (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN                       BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN_POS                   (7U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN_LEN                   (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_DG_IND_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND                         BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_POS                     (8U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_LEN                     (5U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN                      BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN_POS                  (13U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN_LEN                  (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_RBB_IND_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND                         BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_POS                     (14U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_LEN                     (4U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN                      BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN_POS                  (18U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN_LEN                  (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_LNA_IND_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG                     BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_POS                 (19U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_LEN                 (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN                  BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN_POS              (20U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN_LEN              (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN_MSK              (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN_UMSK             (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_SAT_FLAG_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG                    BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_POS                (21U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_LEN                (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN                 BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN_POS             (22U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN_LEN             (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN_MSK             (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN_UMSK            (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_SAT_FLAG_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG                      BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_POS                  (23U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_LEN                  (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN                   BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN_POS               (24U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN_LEN               (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_PKDET_FLAG_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG                        BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_POS                    (25U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_LEN                    (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN                     BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN_POS                 (26U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN_LEN                 (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_PIP_FLAG_EN_POS))

/* 0xC04 : agc_ctrl_1 */
#define BZ_PHY_AGC_AGC_CTRL_1_OFFSET                            (0xC04)
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG                            BZ_PHY_AGC_BZ_AGC_FORCE_TRIG
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_POS                        (0U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_LEN                        (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_MSK                        (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_UMSK                       (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN                         BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN_POS                     (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN_LEN                     (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_TRIG_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE                              BZ_PHY_AGC_BZ_AGC_FORCE_FE
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_POS                          (2U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_LEN                          (8U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_MSK                          (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_UMSK                         (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN                           BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN_POS                       (10U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN_LEN                       (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC                             BZ_PHY_AGC_BZ_AGC_FORCE_ADC
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_POS                         (11U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_LEN                         (8U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_MSK                         (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_UMSK                        (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN                          BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN_POS                      (19U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN_LEN                      (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN_MSK                      (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN_UMSK                     (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_ADC_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_DBG_SEL                               BZ_PHY_AGC_BZ_AGC_DBG_SEL
#define BZ_PHY_AGC_BZ_AGC_DBG_SEL_POS                           (20U)
#define BZ_PHY_AGC_BZ_AGC_DBG_SEL_LEN                           (4U)
#define BZ_PHY_AGC_BZ_AGC_DBG_SEL_MSK                           (((1U<<BZ_PHY_AGC_BZ_AGC_DBG_SEL_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DBG_SEL_POS)
#define BZ_PHY_AGC_BZ_AGC_DBG_SEL_UMSK                          (~(((1U<<BZ_PHY_AGC_BZ_AGC_DBG_SEL_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DBG_SEL_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL                           BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL
#define BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL_POS                       (31U)
#define BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL_LEN                       (1U)
#define BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_AMP_SEL_POS))

/* 0xC08 : agc_ctrl_2 */
#define BZ_PHY_AGC_AGC_CTRL_2_OFFSET                            (0xC08)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI                         BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_POS                     (0U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_LEN                     (8U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN                      BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN_POS                  (8U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN_LEN                  (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_RSSI_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG                           BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_POS                       (9U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_LEN                       (6U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_POS))
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN                        BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN_POS                    (15U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN_LEN                    (1U)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FORCE_FE_DG_EN_POS))

/* 0xC0C : agc_pkdet_0 */
#define BZ_PHY_AGC_AGC_PKDET_0_OFFSET                           (0xC0C)
#define BZ_PHY_AGC_BZ_AGC_PKDET_CNT                             BZ_PHY_AGC_BZ_AGC_PKDET_CNT
#define BZ_PHY_AGC_BZ_AGC_PKDET_CNT_POS                         (0U)
#define BZ_PHY_AGC_BZ_AGC_PKDET_CNT_LEN                         (4U)
#define BZ_PHY_AGC_BZ_AGC_PKDET_CNT_MSK                         (((1U<<BZ_PHY_AGC_BZ_AGC_PKDET_CNT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PKDET_CNT_POS)
#define BZ_PHY_AGC_BZ_AGC_PKDET_CNT_UMSK                        (~(((1U<<BZ_PHY_AGC_BZ_AGC_PKDET_CNT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PKDET_CNT_POS))
#define BZ_PHY_AGC_BZ_AGC_PKDET_WIN                             BZ_PHY_AGC_BZ_AGC_PKDET_WIN
#define BZ_PHY_AGC_BZ_AGC_PKDET_WIN_POS                         (4U)
#define BZ_PHY_AGC_BZ_AGC_PKDET_WIN_LEN                         (4U)
#define BZ_PHY_AGC_BZ_AGC_PKDET_WIN_MSK                         (((1U<<BZ_PHY_AGC_BZ_AGC_PKDET_WIN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PKDET_WIN_POS)
#define BZ_PHY_AGC_BZ_AGC_PKDET_WIN_UMSK                        (~(((1U<<BZ_PHY_AGC_BZ_AGC_PKDET_WIN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PKDET_WIN_POS))
#define BZ_PHY_AGC_BZ_AGC_PKDET_MODE                            BZ_PHY_AGC_BZ_AGC_PKDET_MODE
#define BZ_PHY_AGC_BZ_AGC_PKDET_MODE_POS                        (8U)
#define BZ_PHY_AGC_BZ_AGC_PKDET_MODE_LEN                        (2U)
#define BZ_PHY_AGC_BZ_AGC_PKDET_MODE_MSK                        (((1U<<BZ_PHY_AGC_BZ_AGC_PKDET_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PKDET_MODE_POS)
#define BZ_PHY_AGC_BZ_AGC_PKDET_MODE_UMSK                       (~(((1U<<BZ_PHY_AGC_BZ_AGC_PKDET_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PKDET_MODE_POS))
#define BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN                         BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN
#define BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN_POS                     (10U)
#define BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN_LEN                     (1U)
#define BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PKDET_FLAG_EN_POS))

/* 0xC10 : agc_adc_sat_0 */
#define BZ_PHY_AGC_AGC_ADC_SAT_0_OFFSET                         (0xC10)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE                          BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE_POS                      (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE_LEN                      (2U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE_MSK                      (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE_UMSK                     (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_MODE_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN                       BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN_POS                   (2U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN_LEN                   (1U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_FLAG_EN_POS))

/* 0xC14 : agc_adc_sat_1_zb */
#define BZ_PHY_AGC_AGC_ADC_SAT_1_ZB_OFFSET                      (0xC14)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB                        BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB_POS                    (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB_LEN                    (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB                        BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB_POS                    (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB_LEN                    (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB                        BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB_POS                    (8U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB_LEN                    (9U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_ZB_POS))

/* 0xC18 : agc_adc_sat_1_ble1 */
#define BZ_PHY_AGC_AGC_ADC_SAT_1_BLE1_OFFSET                    (0xC18)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1_POS                  (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1_LEN                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1_POS                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1_LEN                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1_POS                  (8U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1_LEN                  (9U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE1_POS))

/* 0xC1C : agc_adc_sat_1_ble2 */
#define BZ_PHY_AGC_AGC_ADC_SAT_1_BLE2_OFFSET                    (0xC1C)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2_POS                  (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2_LEN                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2_POS                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2_LEN                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2_POS                  (8U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2_LEN                  (9U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLE2_POS))

/* 0xC20 : agc_adc_sat_1_blec */
#define BZ_PHY_AGC_AGC_ADC_SAT_1_BLEC_OFFSET                    (0xC20)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC_POS                  (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC_LEN                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_WIN_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC_POS                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC_LEN                  (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_CNT_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC                      BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC_POS                  (8U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC_LEN                  (9U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SAT_THR_BLEC_POS))

/* 0xC24 : agc_fe_sat_0 */
#define BZ_PHY_AGC_AGC_FE_SAT_0_OFFSET                          (0xC24)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE                           BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE_POS                       (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE_LEN                       (2U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_MODE_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN                        BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN_POS                    (2U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN_LEN                    (1U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_FLAG_EN_POS))

/* 0xC28 : agc_fe_sat_1_zb */
#define BZ_PHY_AGC_AGC_FE_SAT_1_ZB_OFFSET                       (0xC28)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB                         BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB_POS                     (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB_LEN                     (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB                         BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB_POS                     (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB_LEN                     (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB                         BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB_POS                     (8U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB_LEN                     (9U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_ZB_POS))

/* 0xC2C : agc_fe_sat_1_ble1 */
#define BZ_PHY_AGC_AGC_FE_SAT_1_BLE1_OFFSET                     (0xC2C)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1                       BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1_POS                   (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1_LEN                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1                       BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1_POS                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1_LEN                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1                       BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1_POS                   (8U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1_LEN                   (9U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE1_POS))

/* 0xC30 : agc_fe_sat_1_ble2 */
#define BZ_PHY_AGC_AGC_FE_SAT_1_BLE2_OFFSET                     (0xC30)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2                       BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2_POS                   (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2_LEN                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2                       BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2_POS                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2_LEN                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2                       BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2_POS                   (8U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2_LEN                   (9U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLE2_POS))

/* 0xC34 : agc_fe_sat_1_blec */
#define BZ_PHY_AGC_AGC_FE_SAT_1_BLEC_OFFSET                     (0xC34)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC                       BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC_POS                   (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC_LEN                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_WIN_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC                       BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC_POS                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC_LEN                   (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_CNT_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC                       BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC_POS                   (8U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC_LEN                   (9U)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SAT_THR_BLEC_POS))

/* 0xC38 : agc_pip_0 */
#define BZ_PHY_AGC_AGC_PIP_0_OFFSET                             (0xC38)
#define BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH                           BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH
#define BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH_POS                       (0U)
#define BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH_LEN                       (8U)
#define BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH_POS)
#define BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_RSSI_TH_POS))
#define BZ_PHY_AGC_BZ_AGC_PIP_WIN                               BZ_PHY_AGC_BZ_AGC_PIP_WIN
#define BZ_PHY_AGC_BZ_AGC_PIP_WIN_POS                           (8U)
#define BZ_PHY_AGC_BZ_AGC_PIP_WIN_LEN                           (4U)
#define BZ_PHY_AGC_BZ_AGC_PIP_WIN_MSK                           (((1U<<BZ_PHY_AGC_BZ_AGC_PIP_WIN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_WIN_POS)
#define BZ_PHY_AGC_BZ_AGC_PIP_WIN_UMSK                          (~(((1U<<BZ_PHY_AGC_BZ_AGC_PIP_WIN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_WIN_POS))
#define BZ_PHY_AGC_BZ_AGC_PIP_CNT                               BZ_PHY_AGC_BZ_AGC_PIP_CNT
#define BZ_PHY_AGC_BZ_AGC_PIP_CNT_POS                           (12U)
#define BZ_PHY_AGC_BZ_AGC_PIP_CNT_LEN                           (4U)
#define BZ_PHY_AGC_BZ_AGC_PIP_CNT_MSK                           (((1U<<BZ_PHY_AGC_BZ_AGC_PIP_CNT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_CNT_POS)
#define BZ_PHY_AGC_BZ_AGC_PIP_CNT_UMSK                          (~(((1U<<BZ_PHY_AGC_BZ_AGC_PIP_CNT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_CNT_POS))
#define BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH                            BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH
#define BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH_POS                        (16U)
#define BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH_LEN                        (9U)
#define BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH_MSK                        (((1U<<BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH_POS)
#define BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH_UMSK                       (~(((1U<<BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_MAG_TH_POS))
#define BZ_PHY_AGC_BZ_AGC_PIP_MODE                              BZ_PHY_AGC_BZ_AGC_PIP_MODE
#define BZ_PHY_AGC_BZ_AGC_PIP_MODE_POS                          (25U)
#define BZ_PHY_AGC_BZ_AGC_PIP_MODE_LEN                          (2U)
#define BZ_PHY_AGC_BZ_AGC_PIP_MODE_MSK                          (((1U<<BZ_PHY_AGC_BZ_AGC_PIP_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_MODE_POS)
#define BZ_PHY_AGC_BZ_AGC_PIP_MODE_UMSK                         (~(((1U<<BZ_PHY_AGC_BZ_AGC_PIP_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_MODE_POS))
#define BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN                           BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN
#define BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN_POS                       (27U)
#define BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN_LEN                       (1U)
#define BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PIP_FLAG_EN_POS))

/* 0xC3C : agc_disapp_0 */
#define BZ_PHY_AGC_AGC_DISAPP_0_OFFSET                          (0xC3C)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH                         BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH
#define BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH_POS                     (0U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH_LEN                     (9U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH_POS)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_ADC_TH_POS))
#define BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH                         BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH
#define BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH_POS                     (9U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH_LEN                     (5U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH_POS)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_RBB_TH_POS))
#define BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH                         BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH
#define BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH_POS                     (14U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH_LEN                     (4U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH_POS)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_LNA_TH_POS))
#define BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH                         BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH
#define BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH_POS                     (18U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH_LEN                     (10U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH_POS)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_CNT_TH_POS))
#define BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN                         BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN
#define BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN_POS                     (28U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN_LEN                     (1U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_TOP_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN                      BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN
#define BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN_POS                  (29U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN_LEN                  (1U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_FREEZE_EN_POS))
#define BZ_PHY_AGC_BZ_AGC_DISAPP_EN                             BZ_PHY_AGC_BZ_AGC_DISAPP_EN
#define BZ_PHY_AGC_BZ_AGC_DISAPP_EN_POS                         (30U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_EN_LEN                         (1U)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_EN_MSK                         (((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_DISAPP_EN_UMSK                        (~(((1U<<BZ_PHY_AGC_BZ_AGC_DISAPP_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DISAPP_EN_POS))

/* 0xC40 : agc_gain_unlock_0_zb */
#define BZ_PHY_AGC_AGC_GAIN_UNLOCK_0_ZB_OFFSET                  (0xC40)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB                    BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB_POS                (0U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB_LEN                (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB                     BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB_POS                 (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB_LEN                 (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB                   BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB_POS               (16U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB_LEN               (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB                   BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB_POS               (20U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB_LEN               (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_ZB_POS))
#define BZ_PHY_AGC_RSSI_UPD_CYC_ZB                              BZ_PHY_AGC_RSSI_UPD_CYC_ZB
#define BZ_PHY_AGC_RSSI_UPD_CYC_ZB_POS                          (24U)
#define BZ_PHY_AGC_RSSI_UPD_CYC_ZB_LEN                          (6U)
#define BZ_PHY_AGC_RSSI_UPD_CYC_ZB_MSK                          (((1U<<BZ_PHY_AGC_RSSI_UPD_CYC_ZB_LEN)-1)<<BZ_PHY_AGC_RSSI_UPD_CYC_ZB_POS)
#define BZ_PHY_AGC_RSSI_UPD_CYC_ZB_UMSK                         (~(((1U<<BZ_PHY_AGC_RSSI_UPD_CYC_ZB_LEN)-1)<<BZ_PHY_AGC_RSSI_UPD_CYC_ZB_POS))

/* 0xC44 : agc_gain_lock_0_zb */
#define BZ_PHY_AGC_AGC_GAIN_LOCK_0_ZB_OFFSET                    (0xC44)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB                      BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB_POS                  (0U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB_LEN                  (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB                       BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB_POS                   (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB_LEN                   (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB                     BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB_POS                 (16U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB_LEN                 (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB                     BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB_POS                 (20U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB_LEN                 (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_ZB_POS))

/* 0xC48 : agc_gain_unlock_0_ble1 */
#define BZ_PHY_AGC_AGC_GAIN_UNLOCK_0_BLE1_OFFSET                (0xC48)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1                  BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1_POS              (0U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1_LEN              (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1_MSK              (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1_UMSK             (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1                   BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1_POS               (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1_LEN               (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1                 BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1_POS             (16U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1_LEN             (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1_MSK             (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1_UMSK            (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1                 BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1_POS             (20U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1_LEN             (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1_MSK             (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1_UMSK            (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE1_POS))
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE1                            BZ_PHY_AGC_RSSI_UPD_CYC_BLE1
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE1_POS                        (24U)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE1_LEN                        (6U)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE1_MSK                        (((1U<<BZ_PHY_AGC_RSSI_UPD_CYC_BLE1_LEN)-1)<<BZ_PHY_AGC_RSSI_UPD_CYC_BLE1_POS)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE1_UMSK                       (~(((1U<<BZ_PHY_AGC_RSSI_UPD_CYC_BLE1_LEN)-1)<<BZ_PHY_AGC_RSSI_UPD_CYC_BLE1_POS))

/* 0xC4C : agc_gain_lock_0_ble1 */
#define BZ_PHY_AGC_AGC_GAIN_LOCK_0_BLE1_OFFSET                  (0xC4C)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1                    BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1_POS                (0U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1_LEN                (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1                     BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1_POS                 (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1_LEN                 (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1                   BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1_POS               (16U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1_LEN               (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1                   BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1_POS               (20U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1_LEN               (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE1_POS))

/* 0xC50 : agc_gain_unlock_0_ble2 */
#define BZ_PHY_AGC_AGC_GAIN_UNLOCK_0_BLE2_OFFSET                (0xC50)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2                  BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2_POS              (0U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2_LEN              (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2_MSK              (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2_UMSK             (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2                   BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2_POS               (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2_LEN               (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2                 BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2_POS             (16U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2_LEN             (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2_MSK             (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2_UMSK            (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2                 BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2_POS             (20U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2_LEN             (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2_MSK             (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2_UMSK            (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLE2_POS))
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE2                            BZ_PHY_AGC_RSSI_UPD_CYC_BLE2
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE2_POS                        (24U)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE2_LEN                        (6U)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE2_MSK                        (((1U<<BZ_PHY_AGC_RSSI_UPD_CYC_BLE2_LEN)-1)<<BZ_PHY_AGC_RSSI_UPD_CYC_BLE2_POS)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLE2_UMSK                       (~(((1U<<BZ_PHY_AGC_RSSI_UPD_CYC_BLE2_LEN)-1)<<BZ_PHY_AGC_RSSI_UPD_CYC_BLE2_POS))

/* 0xC54 : agc_gain_lock_0_ble2 */
#define BZ_PHY_AGC_AGC_GAIN_LOCK_0_BLE2_OFFSET                  (0xC54)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2                    BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2_POS                (0U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2_LEN                (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2                     BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2_POS                 (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2_LEN                 (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2                   BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2_POS               (16U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2_LEN               (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2                   BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2_POS               (20U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2_LEN               (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLE2_POS))

/* 0xC58 : agc_gain_unlock_0_blec */
#define BZ_PHY_AGC_AGC_GAIN_UNLOCK_0_BLEC_OFFSET                (0xC58)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC                  BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC_POS              (0U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC_LEN              (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC_MSK              (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC_UMSK             (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_WIN_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC                   BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC_POS               (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC_LEN               (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_TH_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC                 BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC_POS             (16U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC_LEN             (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC_MSK             (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC_UMSK            (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL2_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC                 BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC_POS             (20U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC_LEN             (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC_MSK             (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC_UMSK            (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_UNLOCK_NDL1_BLEC_POS))
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLEC                            BZ_PHY_AGC_RSSI_UPD_CYC_BLEC
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLEC_POS                        (24U)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLEC_LEN                        (6U)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLEC_MSK                        (((1U<<BZ_PHY_AGC_RSSI_UPD_CYC_BLEC_LEN)-1)<<BZ_PHY_AGC_RSSI_UPD_CYC_BLEC_POS)
#define BZ_PHY_AGC_RSSI_UPD_CYC_BLEC_UMSK                       (~(((1U<<BZ_PHY_AGC_RSSI_UPD_CYC_BLEC_LEN)-1)<<BZ_PHY_AGC_RSSI_UPD_CYC_BLEC_POS))

/* 0xC5C : agc_gain_lock_0_blec */
#define BZ_PHY_AGC_AGC_GAIN_LOCK_0_BLEC_OFFSET                  (0xC5C)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC                    BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC_POS                (0U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC_LEN                (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_WIN_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC                     BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC_POS                 (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC_LEN                 (8U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_TH_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC                   BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC_POS               (16U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC_LEN               (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL2_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC                   BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC_POS               (20U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC_LEN               (4U)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_GAIN_LOCK_NDL1_BLEC_POS))

/* 0xC60 : agc_adc_loop_0 */
#define BZ_PHY_AGC_AGC_ADC_LOOP_0_OFFSET                        (0xC60)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH                         BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH_POS                     (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH_LEN                     (10U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_TH_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST                           BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST_POS                       (10U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST_LEN                       (8U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_HYST_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE                       BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE_POS                   (18U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE_LEN                   (1U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ERR_MODE_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN                           BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN
#define BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN_POS                       (19U)
#define BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN_LEN                       (1U)
#define BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_LOOP_EN_POS))

/* 0xC64 : agc_adc_loop_1_zb */
#define BZ_PHY_AGC_AGC_ADC_LOOP_1_ZB_OFFSET                     (0xC64)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB                             BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB_POS                         (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB_LEN                         (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB_MSK                         (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB_UMSK                        (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB                     BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB_POS                 (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB_LEN                 (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB                     BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB_POS                 (10U)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB_LEN                 (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB                       BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB_POS                   (16U)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB_LEN                   (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB                       BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB_POS                   (22U)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB_LEN                   (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_ZB_POS))

/* 0xC68 : agc_adc_loop_1_ble1 */
#define BZ_PHY_AGC_AGC_ADC_LOOP_1_BLE1_OFFSET                   (0xC68)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1                           BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1_POS                       (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1_LEN                       (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1                   BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1_POS               (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1_LEN               (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1                   BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1_POS               (10U)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1_LEN               (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1                     BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1_POS                 (16U)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1_LEN                 (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1                     BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1_POS                 (22U)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1_LEN                 (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE1_POS))

/* 0xC6C : agc_adc_loop_1_ble2 */
#define BZ_PHY_AGC_AGC_ADC_LOOP_1_BLE2_OFFSET                   (0xC6C)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2                           BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2_POS                       (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2_LEN                       (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2                   BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2_POS               (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2_LEN               (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2                   BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2_POS               (10U)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2_LEN               (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2                     BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2_POS                 (16U)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2_LEN                 (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2                     BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2_POS                 (22U)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2_LEN                 (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLE2_POS))

/* 0xC70 : agc_adc_loop_1_blec */
#define BZ_PHY_AGC_AGC_ADC_LOOP_1_BLEC_OFFSET                   (0xC70)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC                           BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC_POS                       (0U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC_LEN                       (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_GT_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC                   BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC_POS               (4U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC_LEN               (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_SLOW_COEFF_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC                   BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC_POS               (10U)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC_LEN               (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_FAST_COEFF_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC                     BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC_POS                 (16U)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC_LEN                 (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_DN_COEFF_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC                     BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC_POS                 (22U)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC_LEN                 (6U)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC_MSK                 (((1U<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC_UMSK                (~(((1U<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_ADC_UP_COEFF_BLEC_POS))

/* 0xC74 : agc_fe_loop_0 */
#define BZ_PHY_AGC_AGC_FE_LOOP_0_OFFSET                         (0xC74)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH                          BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH_POS                      (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH_LEN                      (10U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH_MSK                      (((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH_UMSK                     (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_TH_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_GT_HYST                            BZ_PHY_AGC_BZ_AGC_FE_GT_HYST
#define BZ_PHY_AGC_BZ_AGC_FE_GT_HYST_POS                        (10U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_HYST_LEN                        (8U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_HYST_MSK                        (((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_HYST_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_HYST_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_HYST_UMSK                       (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_HYST_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_HYST_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE                        BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE_POS                    (18U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE_LEN                    (1U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_ERR_MODE_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN                            BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN
#define BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN_POS                        (19U)
#define BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN_LEN                        (1U)
#define BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN_MSK                        (((1U<<BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN_UMSK                       (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_LOOP_EN_POS))

/* 0xC78 : agc_fe_loop_1_zb */
#define BZ_PHY_AGC_AGC_FE_LOOP_1_ZB_OFFSET                      (0xC78)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ZB                              BZ_PHY_AGC_BZ_AGC_FE_GT_ZB
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ZB_POS                          (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ZB_LEN                          (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ZB_MSK                          (((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_ZB_UMSK                         (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB                        BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB_POS                    (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB_LEN                    (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB                        BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB_POS                    (10U)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB_LEN                    (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB                      BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB_POS                  (16U)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB                      BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB_POS                  (22U)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_ZB_POS))

/* 0xC7C : agc_fe_loop_1_ble1 */
#define BZ_PHY_AGC_AGC_FE_LOOP_1_BLE1_OFFSET                    (0xC7C)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1                            BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1_POS                        (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1_LEN                        (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1_MSK                        (((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1_UMSK                       (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1                      BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1_POS                  (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1                      BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1_POS                  (10U)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1                    BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1_POS                (16U)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1_LEN                (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1                    BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1_POS                (22U)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1_LEN                (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE1_POS))

/* 0xC80 : agc_fe_loop_1_ble2 */
#define BZ_PHY_AGC_AGC_FE_LOOP_1_BLE2_OFFSET                    (0xC80)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2                            BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2_POS                        (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2_LEN                        (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2_MSK                        (((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2_UMSK                       (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2                      BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2_POS                  (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2                      BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2_POS                  (10U)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2                    BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2_POS                (16U)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2_LEN                (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2                    BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2_POS                (22U)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2_LEN                (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLE2_POS))

/* 0xC84 : agc_fe_loop_1_blec */
#define BZ_PHY_AGC_AGC_FE_LOOP_1_BLEC_OFFSET                    (0xC84)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC                            BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC_POS                        (0U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC_LEN                        (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC_MSK                        (((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC_UMSK                       (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_GT_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC                      BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC_POS                  (4U)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_DN_COEFF_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC                      BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC_POS                  (10U)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_UP_COEFF_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC                    BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC_POS                (16U)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC_LEN                (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_SLOW_COEFF_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC                    BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC_POS                (22U)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC_LEN                (6U)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_FE_FAST_COEFF_BLEC_POS))

/* 0xC88 : agc_sat_rst_zb */
#define BZ_PHY_AGC_AGC_SAT_RST_ZB_OFFSET                        (0xC88)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB                       BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB_POS                   (0U)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB_LEN                   (2U)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB_MSK                   (((1U<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB_LEN)-1)<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB_POS)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB_UMSK                  (~(((1U<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB_LEN)-1)<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_ZB_POS))
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB                       BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB_POS                   (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB_LEN                   (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB_MSK                   (((1U<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB_POS)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB_UMSK                  (~(((1U<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_ZB_POS))
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB                      BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB_POS                  (4U)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB_LEN                  (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB_MSK                  (((1U<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB_POS)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB_UMSK                 (~(((1U<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_ZB_POS))

/* 0xC8C : agc_sat_rst_ble1 */
#define BZ_PHY_AGC_AGC_SAT_RST_BLE1_OFFSET                      (0xC8C)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1                     BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1_POS                 (0U)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1_LEN                 (2U)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1_MSK                 (((1U<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1_LEN)-1)<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1_POS)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1_UMSK                (~(((1U<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1_LEN)-1)<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE1_POS))
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1                     BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1_POS                 (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1_LEN                 (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1_MSK                 (((1U<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1_POS)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1_UMSK                (~(((1U<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE1_POS))
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1                    BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1_POS                (4U)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1_LEN                (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1_MSK                (((1U<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1_POS)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1_UMSK               (~(((1U<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE1_POS))

/* 0xC90 : agc_sat_rst_ble2 */
#define BZ_PHY_AGC_AGC_SAT_RST_BLE2_OFFSET                      (0xC90)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2                     BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2_POS                 (0U)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2_LEN                 (2U)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2_MSK                 (((1U<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2_LEN)-1)<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2_POS)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2_UMSK                (~(((1U<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2_LEN)-1)<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLE2_POS))
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2                     BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2_POS                 (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2_LEN                 (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2_MSK                 (((1U<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2_POS)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2_UMSK                (~(((1U<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLE2_POS))
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2                    BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2_POS                (4U)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2_LEN                (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2_MSK                (((1U<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2_POS)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2_UMSK               (~(((1U<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLE2_POS))

/* 0xC94 : agc_sat_rst_blec */
#define BZ_PHY_AGC_AGC_SAT_RST_BLEC_OFFSET                      (0xC94)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC                     BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC_POS                 (0U)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC_LEN                 (2U)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC_MSK                 (((1U<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC_LEN)-1)<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC_POS)
#define BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC_UMSK                (~(((1U<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC_LEN)-1)<<BZ_PHY_AGC_ADC_SAT_RST_DG_MODE_BLEC_POS))
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC                     BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC_POS                 (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC_LEN                 (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC_MSK                 (((1U<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC_POS)
#define BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC_UMSK                (~(((1U<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_DG_MODE_BLEC_POS))
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC                    BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC_POS                (4U)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC_LEN                (2U)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC_MSK                (((1U<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC_POS)
#define BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC_UMSK               (~(((1U<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC_LEN)-1)<<BZ_PHY_AGC_TIA_SAT_RST_RBB_MODE_BLEC_POS))

/* 0xC98 : agc_proc_dly_0 */
#define BZ_PHY_AGC_AGC_PROC_DLY_0_OFFSET                        (0xC98)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT                       BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_POS                   (0U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_LEN                   (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_POS))
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT                      BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_POS                  (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_LEN                  (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_POS))
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT                      BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_POS                  (16U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_LEN                  (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_POS))

/* 0xC9C : agc_proc_dly_1 */
#define BZ_PHY_AGC_AGC_PROC_DLY_1_OFFSET                        (0xC9C)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB                   BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB_POS               (0U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB_LEN               (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_RBB_POS))
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB                  BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB_POS              (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB_LEN              (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB_MSK              (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB_UMSK             (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_RBB_POS))
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB                  BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB_POS              (16U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB_LEN              (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB_MSK              (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB_UMSK             (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_RBB_POS))

/* 0xCA0 : agc_proc_dly_2 */
#define BZ_PHY_AGC_AGC_PROC_DLY_2_OFFSET                        (0xCA0)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG                    BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG_POS                (0U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG_LEN                (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG_MSK                (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG_UMSK               (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_SAT_DG_POS))
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG                   BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG_POS               (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG_LEN               (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_ADC_SAT_DG_POS))
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG                   BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG_POS               (16U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG_LEN               (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_TIA_SAT_DG_POS))

/* 0xCA4 : agc_proc_dly_3 */
#define BZ_PHY_AGC_AGC_PROC_DLY_3_OFFSET                        (0xCA4)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ                       BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ_POS                   (0U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ_LEN                   (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_DG_ADJ_POS))
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG                   BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG_POS               (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG_LEN               (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_DG_POS))
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB                   BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB_POS               (16U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB_LEN               (8U)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB_MSK               (((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB_POS)
#define BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB_UMSK              (~(((1U<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_PROC_DLY_RBB_ADJ_BB_POS))

/* 0xCA8 : agc_ctrl_lna */
#define BZ_PHY_AGC_AGC_CTRL_LNA_OFFSET                          (0xCA8)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN                           BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN_POS                       (0U)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN_LEN                       (4U)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN_POS)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_LNA_IND_MIN_POS))
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX                           BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX_POS                       (8U)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX_LEN                       (4U)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX_POS)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_LNA_IND_MAX_POS))
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT                          BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT_POS                      (24U)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT_LEN                      (4U)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT_MSK                      (((1U<<BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT_POS)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT_UMSK                     (~(((1U<<BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_LNA_IND_INIT_POS))

/* 0xCAC : agc_ctrl_rbb */
#define BZ_PHY_AGC_AGC_CTRL_RBB_OFFSET                          (0xCAC)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN                           BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN_POS                       (0U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN_LEN                       (5U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN_POS)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_MIN_POS))
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX                           BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX_POS                       (8U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX_LEN                       (5U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX_MSK                       (((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX_POS)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX_UMSK                      (~(((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_MAX_POS))
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP                          BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP_POS                      (16U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP_LEN                      (5U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP_MSK                      (((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP_POS)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP_UMSK                     (~(((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_STEP_POS))
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT                          BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT_POS                      (24U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT_LEN                      (5U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT_MSK                      (((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT_POS)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT_UMSK                     (~(((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_INIT_POS))

/* 0xCB0 : agc_ctrl_dg_zb */
#define BZ_PHY_AGC_AGC_CTRL_DG_ZB_OFFSET                        (0xCB0)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB                         BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB_POS                     (0U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB_LEN                     (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB                         BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB_POS                     (8U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB_LEN                     (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB_MSK                     (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB_UMSK                    (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB                        BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB_POS                    (16U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB_LEN                    (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_ZB_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB                        BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB_POS                    (24U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB_LEN                    (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB_MSK                    (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB_UMSK                   (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_ZB_POS))

/* 0xCB4 : agc_ctrl_dg_ble1 */
#define BZ_PHY_AGC_AGC_CTRL_DG_BLE1_OFFSET                      (0xCB4)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1                       BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1_POS                   (0U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1_LEN                   (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1                       BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1_POS                   (8U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1_LEN                   (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1                      BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1_POS                  (16U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE1_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1                      BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1_POS                  (24U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE1_POS))

/* 0xCB8 : agc_ctrl_dg_ble2 */
#define BZ_PHY_AGC_AGC_CTRL_DG_BLE2_OFFSET                      (0xCB8)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2                       BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2_POS                   (0U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2_LEN                   (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2                       BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2_POS                   (8U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2_LEN                   (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2                      BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2_POS                  (16U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLE2_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2                      BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2_POS                  (24U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLE2_POS))

/* 0xCBC : agc_ctrl_dg_blec */
#define BZ_PHY_AGC_AGC_CTRL_DG_BLEC_OFFSET                      (0xCBC)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC                       BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC_POS                   (0U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC_LEN                   (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MIN_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC                       BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC_POS                   (8U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC_LEN                   (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC_MSK                   (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC_UMSK                  (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_MAX_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC                      BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC_POS                  (16U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_STEP_BLEC_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC                      BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC_POS                  (24U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC_LEN                  (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC_MSK                  (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC_UMSK                 (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_INIT_BLEC_POS))

/* 0xCC0 : agc_status_0 */
#define BZ_PHY_AGC_AGC_STATUS_0_OFFSET                          (0xCC0)
#define BZ_PHY_AGC_BZ_AGC_RSSI_OUT                              BZ_PHY_AGC_BZ_AGC_RSSI_OUT
#define BZ_PHY_AGC_BZ_AGC_RSSI_OUT_POS                          (0U)
#define BZ_PHY_AGC_BZ_AGC_RSSI_OUT_LEN                          (8U)
#define BZ_PHY_AGC_BZ_AGC_RSSI_OUT_MSK                          (((1U<<BZ_PHY_AGC_BZ_AGC_RSSI_OUT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RSSI_OUT_POS)
#define BZ_PHY_AGC_BZ_AGC_RSSI_OUT_UMSK                         (~(((1U<<BZ_PHY_AGC_BZ_AGC_RSSI_OUT_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RSSI_OUT_POS))
#define BZ_PHY_AGC_BZ_AGC_DG_IND                                BZ_PHY_AGC_BZ_AGC_DG_IND
#define BZ_PHY_AGC_BZ_AGC_DG_IND_POS                            (8U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_LEN                            (6U)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_MSK                            (((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_POS)
#define BZ_PHY_AGC_BZ_AGC_DG_IND_UMSK                           (~(((1U<<BZ_PHY_AGC_BZ_AGC_DG_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_DG_IND_POS))
#define BZ_PHY_AGC_BZ_AGC_RBB_IND                               BZ_PHY_AGC_BZ_AGC_RBB_IND
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_POS                           (16U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_LEN                           (5U)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_MSK                           (((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_POS)
#define BZ_PHY_AGC_BZ_AGC_RBB_IND_UMSK                          (~(((1U<<BZ_PHY_AGC_BZ_AGC_RBB_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_RBB_IND_POS))
#define BZ_PHY_AGC_BZ_AGC_LNA_IND                               BZ_PHY_AGC_BZ_AGC_LNA_IND
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_POS                           (24U)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_LEN                           (4U)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_MSK                           (((1U<<BZ_PHY_AGC_BZ_AGC_LNA_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_LNA_IND_POS)
#define BZ_PHY_AGC_BZ_AGC_LNA_IND_UMSK                          (~(((1U<<BZ_PHY_AGC_BZ_AGC_LNA_IND_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_LNA_IND_POS))

/* 0xCC4 : agc_status_1 */
#define BZ_PHY_AGC_AGC_STATUS_1_OFFSET                          (0xCC4)
#define BZ_PHY_AGC_BZ_AGC_STATUS_1                              BZ_PHY_AGC_BZ_AGC_STATUS_1
#define BZ_PHY_AGC_BZ_AGC_STATUS_1_POS                          (0U)
#define BZ_PHY_AGC_BZ_AGC_STATUS_1_LEN                          (32U)
#define BZ_PHY_AGC_BZ_AGC_STATUS_1_MSK                          (((1U<<BZ_PHY_AGC_BZ_AGC_STATUS_1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_STATUS_1_POS)
#define BZ_PHY_AGC_BZ_AGC_STATUS_1_UMSK                         (~(((1U<<BZ_PHY_AGC_BZ_AGC_STATUS_1_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_STATUS_1_POS))

/* 0xCC8 : agc_status_2 */
#define BZ_PHY_AGC_AGC_STATUS_2_OFFSET                          (0xCC8)
#define BZ_PHY_AGC_BZ_AGC_STATUS_2                              BZ_PHY_AGC_BZ_AGC_STATUS_2
#define BZ_PHY_AGC_BZ_AGC_STATUS_2_POS                          (0U)
#define BZ_PHY_AGC_BZ_AGC_STATUS_2_LEN                          (32U)
#define BZ_PHY_AGC_BZ_AGC_STATUS_2_MSK                          (((1U<<BZ_PHY_AGC_BZ_AGC_STATUS_2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_STATUS_2_POS)
#define BZ_PHY_AGC_BZ_AGC_STATUS_2_UMSK                         (~(((1U<<BZ_PHY_AGC_BZ_AGC_STATUS_2_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_STATUS_2_POS))

/* 0xCCC : agc_status_3 */
#define BZ_PHY_AGC_AGC_STATUS_3_OFFSET                          (0xCCC)
#define BZ_PHY_AGC_BZ_AGC_STATUS_3                              BZ_PHY_AGC_BZ_AGC_STATUS_3
#define BZ_PHY_AGC_BZ_AGC_STATUS_3_POS                          (0U)
#define BZ_PHY_AGC_BZ_AGC_STATUS_3_LEN                          (32U)
#define BZ_PHY_AGC_BZ_AGC_STATUS_3_MSK                          (((1U<<BZ_PHY_AGC_BZ_AGC_STATUS_3_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_STATUS_3_POS)
#define BZ_PHY_AGC_BZ_AGC_STATUS_3_UMSK                         (~(((1U<<BZ_PHY_AGC_BZ_AGC_STATUS_3_LEN)-1)<<BZ_PHY_AGC_BZ_AGC_STATUS_3_POS))

/* 0xCD0 : agc_ctrl_reserved */
#define BZ_PHY_AGC_AGC_CTRL_RESERVED_OFFSET                     (0xCD0)
#define BZ_PHY_AGC_AGC_CTRL_RESERVED                            BZ_PHY_AGC_AGC_CTRL_RESERVED
#define BZ_PHY_AGC_AGC_CTRL_RESERVED_POS                        (0U)
#define BZ_PHY_AGC_AGC_CTRL_RESERVED_LEN                        (32U)
#define BZ_PHY_AGC_AGC_CTRL_RESERVED_MSK                        (((1U<<BZ_PHY_AGC_AGC_CTRL_RESERVED_LEN)-1)<<BZ_PHY_AGC_AGC_CTRL_RESERVED_POS)
#define BZ_PHY_AGC_AGC_CTRL_RESERVED_UMSK                       (~(((1U<<BZ_PHY_AGC_AGC_CTRL_RESERVED_LEN)-1)<<BZ_PHY_AGC_AGC_CTRL_RESERVED_POS))

/* 0xCE0 : lna_gain_table_0 */
#define BZ_PHY_AGC_LNA_GAIN_TABLE_0_OFFSET                      (0xCE0)
#define BZ_PHY_AGC_LNA_GAIN_0                                   BZ_PHY_AGC_LNA_GAIN_0
#define BZ_PHY_AGC_LNA_GAIN_0_POS                               (0U)
#define BZ_PHY_AGC_LNA_GAIN_0_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_0_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_0_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_0_POS)
#define BZ_PHY_AGC_LNA_GAIN_0_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_0_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_0_POS))
#define BZ_PHY_AGC_LNA_GAIN_1                                   BZ_PHY_AGC_LNA_GAIN_1
#define BZ_PHY_AGC_LNA_GAIN_1_POS                               (8U)
#define BZ_PHY_AGC_LNA_GAIN_1_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_1_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_1_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_1_POS)
#define BZ_PHY_AGC_LNA_GAIN_1_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_1_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_1_POS))
#define BZ_PHY_AGC_LNA_GAIN_2                                   BZ_PHY_AGC_LNA_GAIN_2
#define BZ_PHY_AGC_LNA_GAIN_2_POS                               (16U)
#define BZ_PHY_AGC_LNA_GAIN_2_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_2_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_2_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_2_POS)
#define BZ_PHY_AGC_LNA_GAIN_2_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_2_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_2_POS))
#define BZ_PHY_AGC_LNA_GAIN_3                                   BZ_PHY_AGC_LNA_GAIN_3
#define BZ_PHY_AGC_LNA_GAIN_3_POS                               (24U)
#define BZ_PHY_AGC_LNA_GAIN_3_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_3_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_3_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_3_POS)
#define BZ_PHY_AGC_LNA_GAIN_3_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_3_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_3_POS))

/* 0xCE4 : lna_gain_tale_1 */
#define BZ_PHY_AGC_LNA_GAIN_TALE_1_OFFSET                       (0xCE4)
#define BZ_PHY_AGC_LNA_GAIN_4                                   BZ_PHY_AGC_LNA_GAIN_4
#define BZ_PHY_AGC_LNA_GAIN_4_POS                               (0U)
#define BZ_PHY_AGC_LNA_GAIN_4_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_4_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_4_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_4_POS)
#define BZ_PHY_AGC_LNA_GAIN_4_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_4_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_4_POS))
#define BZ_PHY_AGC_LNA_GAIN_5                                   BZ_PHY_AGC_LNA_GAIN_5
#define BZ_PHY_AGC_LNA_GAIN_5_POS                               (8U)
#define BZ_PHY_AGC_LNA_GAIN_5_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_5_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_5_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_5_POS)
#define BZ_PHY_AGC_LNA_GAIN_5_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_5_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_5_POS))
#define BZ_PHY_AGC_LNA_GAIN_6                                   BZ_PHY_AGC_LNA_GAIN_6
#define BZ_PHY_AGC_LNA_GAIN_6_POS                               (16U)
#define BZ_PHY_AGC_LNA_GAIN_6_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_6_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_6_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_6_POS)
#define BZ_PHY_AGC_LNA_GAIN_6_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_6_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_6_POS))
#define BZ_PHY_AGC_LNA_GAIN_7                                   BZ_PHY_AGC_LNA_GAIN_7
#define BZ_PHY_AGC_LNA_GAIN_7_POS                               (24U)
#define BZ_PHY_AGC_LNA_GAIN_7_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_7_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_7_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_7_POS)
#define BZ_PHY_AGC_LNA_GAIN_7_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_7_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_7_POS))

/* 0xCE8 : lna_gain_tale_2 */
#define BZ_PHY_AGC_LNA_GAIN_TALE_2_OFFSET                       (0xCE8)
#define BZ_PHY_AGC_LNA_GAIN_8                                   BZ_PHY_AGC_LNA_GAIN_8
#define BZ_PHY_AGC_LNA_GAIN_8_POS                               (0U)
#define BZ_PHY_AGC_LNA_GAIN_8_LEN                               (6U)
#define BZ_PHY_AGC_LNA_GAIN_8_MSK                               (((1U<<BZ_PHY_AGC_LNA_GAIN_8_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_8_POS)
#define BZ_PHY_AGC_LNA_GAIN_8_UMSK                              (~(((1U<<BZ_PHY_AGC_LNA_GAIN_8_LEN)-1)<<BZ_PHY_AGC_LNA_GAIN_8_POS))


struct  bz_phy_agc_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[3072];

    /* 0xC00 : agc_ctrl_0 */
    union {
        struct {
            uint32_t bz_agc_en                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t bz_agc_force_dg_ind            :  6; /* [ 6: 1],        r/w,        0x0 */
            uint32_t bz_agc_force_dg_ind_en         :  1; /* [    7],        r/w,        0x0 */
            uint32_t bz_agc_force_rbb_ind           :  5; /* [12: 8],        r/w,        0x0 */
            uint32_t bz_agc_force_rbb_ind_en        :  1; /* [   13],        r/w,        0x0 */
            uint32_t bz_agc_force_lna_ind           :  4; /* [17:14],        r/w,        0x0 */
            uint32_t bz_agc_force_lna_ind_en        :  1; /* [   18],        r/w,        0x0 */
            uint32_t bz_agc_force_fe_sat_flag       :  1; /* [   19],        r/w,        0x0 */
            uint32_t bz_agc_force_fe_sat_flag_en    :  1; /* [   20],        r/w,        0x0 */
            uint32_t bz_agc_force_adc_sat_flag      :  1; /* [   21],        r/w,        0x0 */
            uint32_t bz_agc_force_adc_sat_flag_en   :  1; /* [   22],        r/w,        0x0 */
            uint32_t bz_agc_force_pkdet_flag        :  1; /* [   23],        r/w,        0x0 */
            uint32_t bz_agc_force_pkdet_flag_en     :  1; /* [   24],        r/w,        0x0 */
            uint32_t bz_agc_force_pip_flag          :  1; /* [   25],        r/w,        0x0 */
            uint32_t bz_agc_force_pip_flag_en       :  1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_0;

    /* 0xC04 : agc_ctrl_1 */
    union {
        struct {
            uint32_t bz_agc_force_trig              :  1; /* [    0],        r/w,        0x0 */
            uint32_t bz_agc_force_trig_en           :  1; /* [    1],        r/w,        0x0 */
            uint32_t bz_agc_force_fe                :  8; /* [ 9: 2],        r/w,        0x0 */
            uint32_t bz_agc_force_fe_en             :  1; /* [   10],        r/w,        0x0 */
            uint32_t bz_agc_force_adc               :  8; /* [18:11],        r/w,        0x0 */
            uint32_t bz_agc_force_adc_en            :  1; /* [   19],        r/w,        0x0 */
            uint32_t bz_agc_dbg_sel                 :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reserved_24_30                 :  7; /* [30:24],       rsvd,        0x0 */
            uint32_t bz_agc_adc_amp_sel             :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_1;

    /* 0xC08 : agc_ctrl_2 */
    union {
        struct {
            uint32_t bz_agc_force_fe_rssi           :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t bz_agc_force_fe_rssi_en        :  1; /* [    8],        r/w,        0x0 */
            uint32_t bz_agc_force_fe_dg             :  6; /* [14: 9],        r/w,        0x0 */
            uint32_t bz_agc_force_fe_dg_en          :  1; /* [   15],        r/w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_2;

    /* 0xC0C : agc_pkdet_0 */
    union {
        struct {
            uint32_t bz_agc_pkdet_cnt               :  4; /* [ 3: 0],        r/w,        0x3 */
            uint32_t bz_agc_pkdet_win               :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_pkdet_mode              :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t bz_agc_pkdet_flag_en           :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_pkdet_0;

    /* 0xC10 : agc_adc_sat_0 */
    union {
        struct {
            uint32_t bz_agc_adc_sat_mode            :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t bz_agc_adc_sat_flag_en         :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3_31                  : 29; /* [31: 3],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_sat_0;

    /* 0xC14 : agc_adc_sat_1_zb */
    union {
        struct {
            uint32_t bz_agc_adc_sat_win_zb          :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t bz_agc_adc_sat_cnt_zb          :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_adc_sat_thr_zb          :  9; /* [16: 8],        r/w,      0x100 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_sat_1_zb;

    /* 0xC18 : agc_adc_sat_1_ble1 */
    union {
        struct {
            uint32_t bz_agc_adc_sat_win_ble1        :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t bz_agc_adc_sat_cnt_ble1        :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_adc_sat_thr_ble1        :  9; /* [16: 8],        r/w,      0x100 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_sat_1_ble1;

    /* 0xC1C : agc_adc_sat_1_ble2 */
    union {
        struct {
            uint32_t bz_agc_adc_sat_win_ble2        :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t bz_agc_adc_sat_cnt_ble2        :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_adc_sat_thr_ble2        :  9; /* [16: 8],        r/w,      0x100 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_sat_1_ble2;

    /* 0xC20 : agc_adc_sat_1_blec */
    union {
        struct {
            uint32_t bz_agc_adc_sat_win_blec        :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t bz_agc_adc_sat_cnt_blec        :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_adc_sat_thr_blec        :  9; /* [16: 8],        r/w,      0x100 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_sat_1_blec;

    /* 0xC24 : agc_fe_sat_0 */
    union {
        struct {
            uint32_t bz_agc_fe_sat_mode             :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t bz_agc_fe_sat_flag_en          :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3_31                  : 29; /* [31: 3],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_sat_0;

    /* 0xC28 : agc_fe_sat_1_zb */
    union {
        struct {
            uint32_t bz_agc_fe_sat_win_zb           :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t bz_agc_fe_sat_cnt_zb           :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_fe_sat_thr_zb           :  9; /* [16: 8],        r/w,      0x140 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_sat_1_zb;

    /* 0xC2C : agc_fe_sat_1_ble1 */
    union {
        struct {
            uint32_t bz_agc_fe_sat_win_ble1         :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t bz_agc_fe_sat_cnt_ble1         :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_fe_sat_thr_ble1         :  9; /* [16: 8],        r/w,      0x140 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_sat_1_ble1;

    /* 0xC30 : agc_fe_sat_1_ble2 */
    union {
        struct {
            uint32_t bz_agc_fe_sat_win_ble2         :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t bz_agc_fe_sat_cnt_ble2         :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_fe_sat_thr_ble2         :  9; /* [16: 8],        r/w,      0x140 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_sat_1_ble2;

    /* 0xC34 : agc_fe_sat_1_blec */
    union {
        struct {
            uint32_t bz_agc_fe_sat_win_blec         :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t bz_agc_fe_sat_cnt_blec         :  4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t bz_agc_fe_sat_thr_blec         :  9; /* [16: 8],        r/w,      0x140 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_sat_1_blec;

    /* 0xC38 : agc_pip_0 */
    union {
        struct {
            uint32_t bz_agc_pip_rssi_th             :  8; /* [ 7: 0],        r/w,       0x32 */
            uint32_t bz_agc_pip_win                 :  4; /* [11: 8],        r/w,        0x6 */
            uint32_t bz_agc_pip_cnt                 :  4; /* [15:12],        r/w,        0x6 */
            uint32_t bz_agc_pip_mag_th              :  9; /* [24:16],        r/w,      0x140 */
            uint32_t bz_agc_pip_mode                :  2; /* [26:25],        r/w,        0x1 */
            uint32_t bz_agc_pip_flag_en             :  1; /* [   27],        r/w,        0x1 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_pip_0;

    /* 0xC3C : agc_disapp_0 */
    union {
        struct {
            uint32_t bz_agc_disapp_adc_th           :  9; /* [ 8: 0],        r/w,       0x10 */
            uint32_t bz_agc_disapp_rbb_th           :  5; /* [13: 9],        r/w,        0x3 */
            uint32_t bz_agc_disapp_lna_th           :  4; /* [17:14],        r/w,        0x7 */
            uint32_t bz_agc_disapp_cnt_th           : 10; /* [27:18],        r/w,       0x40 */
            uint32_t bz_agc_disapp_top_en           :  1; /* [   28],        r/w,        0x1 */
            uint32_t bz_agc_disapp_freeze_en        :  1; /* [   29],        r/w,        0x1 */
            uint32_t bz_agc_disapp_en               :  1; /* [   30],        r/w,        0x1 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_disapp_0;

    /* 0xC40 : agc_gain_unlock_0_zb */
    union {
        struct {
            uint32_t bz_agc_gain_unlock_win_zb      :  8; /* [ 7: 0],        r/w,       0x3c */
            uint32_t bz_agc_gain_unlock_th_zb       :  8; /* [15: 8],        r/w,        0x4 */
            uint32_t bz_agc_gain_unlock_ndl2_zb     :  4; /* [19:16],        r/w,        0x4 */
            uint32_t bz_agc_gain_unlock_ndl1_zb     :  4; /* [23:20],        r/w,        0x1 */
            uint32_t bz_phy_agc_rssi_upd_cyc_zb     :  6; /* [29:24],        r/w,        0x8 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_gain_unlock_0_zb;

    /* 0xC44 : agc_gain_lock_0_zb */
    union {
        struct {
            uint32_t bz_agc_gain_lock_win_zb        :  8; /* [ 7: 0],        r/w,       0x10 */
            uint32_t bz_agc_gain_lock_th_zb         :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t bz_agc_gain_lock_ndl2_zb       :  4; /* [19:16],        r/w,        0x8 */
            uint32_t bz_agc_gain_lock_ndl1_zb       :  4; /* [23:20],        r/w,        0x2 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_gain_lock_0_zb;

    /* 0xC48 : agc_gain_unlock_0_ble1 */
    union {
        struct {
            uint32_t bz_agc_gain_unlock_win_ble1    :  8; /* [ 7: 0],        r/w,       0x3c */
            uint32_t bz_agc_gain_unlock_th_ble1     :  8; /* [15: 8],        r/w,        0x4 */
            uint32_t bz_agc_gain_unlock_ndl2_ble1   :  4; /* [19:16],        r/w,        0x4 */
            uint32_t bz_agc_gain_unlock_ndl1_ble1   :  4; /* [23:20],        r/w,        0x1 */
            uint32_t bz_phy_agc_rssi_upd_cyc_ble1   :  6; /* [29:24],        r/w,        0x8 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_gain_unlock_0_ble1;

    /* 0xC4C : agc_gain_lock_0_ble1 */
    union {
        struct {
            uint32_t bz_agc_gain_lock_win_ble1      :  8; /* [ 7: 0],        r/w,       0x10 */
            uint32_t bz_agc_gain_lock_th_ble1       :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t bz_agc_gain_lock_ndl2_ble1     :  4; /* [19:16],        r/w,        0x4 */
            uint32_t bz_agc_gain_lock_ndl1_ble1     :  4; /* [23:20],        r/w,        0x1 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_gain_lock_0_ble1;

    /* 0xC50 : agc_gain_unlock_0_ble2 */
    union {
        struct {
            uint32_t bz_agc_gain_unlock_win_ble2    :  8; /* [ 7: 0],        r/w,       0x3c */
            uint32_t bz_agc_gain_unlock_th_ble2     :  8; /* [15: 8],        r/w,        0x4 */
            uint32_t bz_agc_gain_unlock_ndl2_ble2   :  4; /* [19:16],        r/w,        0x4 */
            uint32_t bz_agc_gain_unlock_ndl1_ble2   :  4; /* [23:20],        r/w,        0x1 */
            uint32_t bz_phy_agc_rssi_upd_cyc_ble2   :  6; /* [29:24],        r/w,        0x8 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_gain_unlock_0_ble2;

    /* 0xC54 : agc_gain_lock_0_ble2 */
    union {
        struct {
            uint32_t bz_agc_gain_lock_win_ble2      :  8; /* [ 7: 0],        r/w,       0x10 */
            uint32_t bz_agc_gain_lock_th_ble2       :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t bz_agc_gain_lock_ndl2_ble2     :  4; /* [19:16],        r/w,        0x4 */
            uint32_t bz_agc_gain_lock_ndl1_ble2     :  4; /* [23:20],        r/w,        0x1 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_gain_lock_0_ble2;

    /* 0xC58 : agc_gain_unlock_0_blec */
    union {
        struct {
            uint32_t bz_agc_gain_unlock_win_blec    :  8; /* [ 7: 0],        r/w,       0x3c */
            uint32_t bz_agc_gain_unlock_th_blec     :  8; /* [15: 8],        r/w,        0x4 */
            uint32_t bz_agc_gain_unlock_ndl2_blec   :  4; /* [19:16],        r/w,        0x4 */
            uint32_t bz_agc_gain_unlock_ndl1_blec   :  4; /* [23:20],        r/w,        0x1 */
            uint32_t bz_phy_agc_rssi_upd_cyc_blec   :  6; /* [29:24],        r/w,        0x8 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_gain_unlock_0_blec;

    /* 0xC5C : agc_gain_lock_0_blec */
    union {
        struct {
            uint32_t bz_agc_gain_lock_win_blec      :  8; /* [ 7: 0],        r/w,       0x10 */
            uint32_t bz_agc_gain_lock_th_blec       :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t bz_agc_gain_lock_ndl2_blec     :  4; /* [19:16],        r/w,        0x4 */
            uint32_t bz_agc_gain_lock_ndl1_blec     :  4; /* [23:20],        r/w,        0x1 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_gain_lock_0_blec;

    /* 0xC60 : agc_adc_loop_0 */
    union {
        struct {
            uint32_t bz_agc_adc_gt_err_th           : 10; /* [ 9: 0],        r/w,       0x10 */
            uint32_t bz_agc_adc_gt_hyst             :  8; /* [17:10],        r/w,       0x50 */
            uint32_t bz_agc_adc_gt_err_mode         :  1; /* [   18],        r/w,        0x1 */
            uint32_t bz_agc_adc_loop_en             :  1; /* [   19],        r/w,        0x1 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_loop_0;

    /* 0xC64 : agc_adc_loop_1_zb */
    union {
        struct {
            uint32_t bz_agc_adc_gt_zb               :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t bz_agc_adc_slow_coeff_zb       :  6; /* [ 9: 4],        r/w,       0x20 */
            uint32_t bz_agc_adc_fast_coeff_zb       :  6; /* [15:10],        r/w,       0x20 */
            uint32_t bz_agc_adc_dn_coeff_zb         :  6; /* [21:16],        r/w,        0x4 */
            uint32_t bz_agc_adc_up_coeff_zb         :  6; /* [27:22],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_loop_1_zb;

    /* 0xC68 : agc_adc_loop_1_ble1 */
    union {
        struct {
            uint32_t bz_agc_adc_gt_ble1             :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t bz_agc_adc_slow_coeff_ble1     :  6; /* [ 9: 4],        r/w,        0x8 */
            uint32_t bz_agc_adc_fast_coeff_ble1     :  6; /* [15:10],        r/w,       0x20 */
            uint32_t bz_agc_adc_dn_coeff_ble1       :  6; /* [21:16],        r/w,        0x4 */
            uint32_t bz_agc_adc_up_coeff_ble1       :  6; /* [27:22],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_loop_1_ble1;

    /* 0xC6C : agc_adc_loop_1_ble2 */
    union {
        struct {
            uint32_t bz_agc_adc_gt_ble2             :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t bz_agc_adc_slow_coeff_ble2     :  6; /* [ 9: 4],        r/w,        0x8 */
            uint32_t bz_agc_adc_fast_coeff_ble2     :  6; /* [15:10],        r/w,       0x20 */
            uint32_t bz_agc_adc_dn_coeff_ble2       :  6; /* [21:16],        r/w,        0x4 */
            uint32_t bz_agc_adc_up_coeff_ble2       :  6; /* [27:22],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_loop_1_ble2;

    /* 0xC70 : agc_adc_loop_1_blec */
    union {
        struct {
            uint32_t bz_agc_adc_gt_blec             :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t bz_agc_adc_slow_coeff_blec     :  6; /* [ 9: 4],        r/w,        0x8 */
            uint32_t bz_agc_adc_fast_coeff_blec     :  6; /* [15:10],        r/w,       0x20 */
            uint32_t bz_agc_adc_dn_coeff_blec       :  6; /* [21:16],        r/w,        0x4 */
            uint32_t bz_agc_adc_up_coeff_blec       :  6; /* [27:22],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_adc_loop_1_blec;

    /* 0xC74 : agc_fe_loop_0 */
    union {
        struct {
            uint32_t bz_agc_fe_gt_err_th            : 10; /* [ 9: 0],        r/w,       0x10 */
            uint32_t bz_agc_fe_gt_hyst              :  8; /* [17:10],        r/w,       0x30 */
            uint32_t bz_agc_fe_gt_err_mode          :  1; /* [   18],        r/w,        0x1 */
            uint32_t bz_agc_fe_loop_en              :  1; /* [   19],        r/w,        0x1 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_loop_0;

    /* 0xC78 : agc_fe_loop_1_zb */
    union {
        struct {
            uint32_t bz_agc_fe_gt_zb                :  4; /* [ 3: 0],        r/w,        0x9 */
            uint32_t bz_agc_fe_dn_coeff_zb          :  6; /* [ 9: 4],        r/w,        0x2 */
            uint32_t bz_agc_fe_up_coeff_zb          :  6; /* [15:10],        r/w,        0x4 */
            uint32_t bz_agc_fe_slow_coeff_zb        :  6; /* [21:16],        r/w,        0x8 */
            uint32_t bz_agc_fe_fast_coeff_zb        :  6; /* [27:22],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_loop_1_zb;

    /* 0xC7C : agc_fe_loop_1_ble1 */
    union {
        struct {
            uint32_t bz_agc_fe_gt_ble1              :  4; /* [ 3: 0],        r/w,        0x9 */
            uint32_t bz_agc_fe_dn_coeff_ble1        :  6; /* [ 9: 4],        r/w,        0x2 */
            uint32_t bz_agc_fe_up_coeff_ble1        :  6; /* [15:10],        r/w,        0x4 */
            uint32_t bz_agc_fe_slow_coeff_ble1      :  6; /* [21:16],        r/w,        0x8 */
            uint32_t bz_agc_fe_fast_coeff_ble1      :  6; /* [27:22],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_loop_1_ble1;

    /* 0xC80 : agc_fe_loop_1_ble2 */
    union {
        struct {
            uint32_t bz_agc_fe_gt_ble2              :  4; /* [ 3: 0],        r/w,        0x9 */
            uint32_t bz_agc_fe_dn_coeff_ble2        :  6; /* [ 9: 4],        r/w,        0x2 */
            uint32_t bz_agc_fe_up_coeff_ble2        :  6; /* [15:10],        r/w,        0x4 */
            uint32_t bz_agc_fe_slow_coeff_ble2      :  6; /* [21:16],        r/w,        0x8 */
            uint32_t bz_agc_fe_fast_coeff_ble2      :  6; /* [27:22],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_loop_1_ble2;

    /* 0xC84 : agc_fe_loop_1_blec */
    union {
        struct {
            uint32_t bz_agc_fe_gt_blec              :  4; /* [ 3: 0],        r/w,        0x9 */
            uint32_t bz_agc_fe_dn_coeff_blec        :  6; /* [ 9: 4],        r/w,        0x2 */
            uint32_t bz_agc_fe_up_coeff_blec        :  6; /* [15:10],        r/w,        0x4 */
            uint32_t bz_agc_fe_slow_coeff_blec      :  6; /* [21:16],        r/w,        0x8 */
            uint32_t bz_agc_fe_fast_coeff_blec      :  6; /* [27:22],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_fe_loop_1_blec;

    /* 0xC88 : agc_sat_rst_zb */
    union {
        struct {
            uint32_t adc_sat_rst_dg_mode_zb         :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t tia_sat_rst_dg_mode_zb         :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t tia_sat_rst_rbb_mode_zb        :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_sat_rst_zb;

    /* 0xC8C : agc_sat_rst_ble1 */
    union {
        struct {
            uint32_t adc_sat_rst_dg_mode_ble1       :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t tia_sat_rst_dg_mode_ble1       :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t tia_sat_rst_rbb_mode_ble1      :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_sat_rst_ble1;

    /* 0xC90 : agc_sat_rst_ble2 */
    union {
        struct {
            uint32_t adc_sat_rst_dg_mode_ble2       :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t tia_sat_rst_dg_mode_ble2       :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t tia_sat_rst_rbb_mode_ble2      :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_sat_rst_ble2;

    /* 0xC94 : agc_sat_rst_blec */
    union {
        struct {
            uint32_t adc_sat_rst_dg_mode_blec       :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t tia_sat_rst_dg_mode_blec       :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t tia_sat_rst_rbb_mode_blec      :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_sat_rst_blec;

    /* 0xC98 : agc_proc_dly_0 */
    union {
        struct {
            uint32_t bz_agc_proc_dly_dg_sat         :  8; /* [ 7: 0],        r/w,        0x2 */
            uint32_t bz_agc_proc_dly_adc_sat        :  8; /* [15: 8],        r/w,        0x4 */
            uint32_t bz_agc_proc_dly_tia_sat        :  8; /* [23:16],        r/w,        0xa */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_proc_dly_0;

    /* 0xC9C : agc_proc_dly_1 */
    union {
        struct {
            uint32_t bz_agc_proc_dly_dg_sat_rbb     :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t bz_agc_proc_dly_adc_sat_rbb    :  8; /* [15: 8],        r/w,        0x4 */
            uint32_t bz_agc_proc_dly_tia_sat_rbb    :  8; /* [23:16],        r/w,        0xa */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_proc_dly_1;

    /* 0xCA0 : agc_proc_dly_2 */
    union {
        struct {
            uint32_t bz_agc_proc_dly_dg_sat_dg      :  8; /* [ 7: 0],        r/w,        0x2 */
            uint32_t bz_agc_proc_dly_adc_sat_dg     :  8; /* [15: 8],        r/w,        0x6 */
            uint32_t bz_agc_proc_dly_tia_sat_dg     :  8; /* [23:16],        r/w,        0xf */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_proc_dly_2;

    /* 0xCA4 : agc_proc_dly_3 */
    union {
        struct {
            uint32_t bz_agc_proc_dly_dg_adj         :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t bz_agc_proc_dly_rbb_adj_dg     :  8; /* [15: 8],        r/w,        0x6 */
            uint32_t bz_agc_proc_dly_rbb_adj_bb     :  8; /* [23:16],        r/w,        0x4 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_proc_dly_3;

    /* 0xCA8 : agc_ctrl_lna */
    union {
        struct {
            uint32_t bz_agc_lna_ind_min             :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t bz_agc_lna_ind_max             :  4; /* [11: 8],        r/w,        0x7 */
            uint32_t reserved_12_23                 : 12; /* [23:12],       rsvd,        0x0 */
            uint32_t bz_agc_lna_ind_init            :  4; /* [27:24],        r/w,        0x7 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_lna;

    /* 0xCAC : agc_ctrl_rbb */
    union {
        struct {
            uint32_t bz_agc_rbb_ind_min             :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t bz_agc_rbb_ind_max             :  5; /* [12: 8],        r/w,        0xf */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t bz_agc_rbb_ind_step            :  5; /* [20:16],        r/w,        0x3 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t bz_agc_rbb_ind_init            :  5; /* [28:24],        r/w,        0xc */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_rbb;

    /* 0xCB0 : agc_ctrl_dg_zb */
    union {
        struct {
            uint32_t bz_agc_dg_ind_min_zb           :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_max_zb           :  6; /* [13: 8],        r/w,       0x35 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_step_zb          :  6; /* [21:16],        r/w,        0x6 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_init_zb          :  6; /* [29:24],        r/w,       0x30 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_dg_zb;

    /* 0xCB4 : agc_ctrl_dg_ble1 */
    union {
        struct {
            uint32_t bz_agc_dg_ind_min_ble1         :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_max_ble1         :  6; /* [13: 8],        r/w,       0x35 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_step_ble1        :  6; /* [21:16],        r/w,        0x6 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_init_ble1        :  6; /* [29:24],        r/w,       0x30 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_dg_ble1;

    /* 0xCB8 : agc_ctrl_dg_ble2 */
    union {
        struct {
            uint32_t bz_agc_dg_ind_min_ble2         :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_max_ble2         :  6; /* [13: 8],        r/w,       0x35 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_step_ble2        :  6; /* [21:16],        r/w,        0x6 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_init_ble2        :  6; /* [29:24],        r/w,       0x30 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_dg_ble2;

    /* 0xCBC : agc_ctrl_dg_blec */
    union {
        struct {
            uint32_t bz_agc_dg_ind_min_blec         :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_max_blec         :  6; /* [13: 8],        r/w,       0x35 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_step_blec        :  6; /* [21:16],        r/w,        0x6 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t bz_agc_dg_ind_init_blec        :  6; /* [29:24],        r/w,       0x30 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_ctrl_dg_blec;

    /* 0xCC0 : agc_status_0 */
    union {
        struct {
            uint32_t bz_agc_rssi_out                :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t bz_agc_dg_ind                  :  6; /* [13: 8],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t bz_agc_rbb_ind                 :  5; /* [20:16],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t bz_agc_lna_ind                 :  4; /* [27:24],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_status_0;

    /* 0xCC4 : agc_status_1 */
    union {
        struct {
            uint32_t bz_agc_status_1                : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_status_1;

    /* 0xCC8 : agc_status_2 */
    union {
        struct {
            uint32_t bz_agc_status_2                : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_status_2;

    /* 0xCCC : agc_status_3 */
    union {
        struct {
            uint32_t bz_agc_status_3                : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } agc_status_3;

    /* 0xCD0 : agc_ctrl_reserved */
    union {
        struct {
            uint32_t agc_ctrl_reserved              : 32; /* [31: 0],        r/w,     0xffff */
        }BF;
        uint32_t WORD;
    } agc_ctrl_reserved;

    /* 0xcd4  reserved */
    uint8_t RESERVED0xcd4[12];

    /* 0xCE0 : lna_gain_table_0 */
    union {
        struct {
            uint32_t lna_gain_0                     :  6; /* [ 5: 0],        r/w,        0xa */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lna_gain_1                     :  6; /* [13: 8],        r/w,        0xf */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lna_gain_2                     :  6; /* [21:16],        r/w,       0x15 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lna_gain_3                     :  6; /* [29:24],        r/w,       0x1b */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lna_gain_table_0;

    /* 0xCE4 : lna_gain_tale_1 */
    union {
        struct {
            uint32_t lna_gain_4                     :  6; /* [ 5: 0],        r/w,       0x21 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lna_gain_5                     :  6; /* [13: 8],        r/w,       0x27 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lna_gain_6                     :  6; /* [21:16],        r/w,       0x2d */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lna_gain_7                     :  6; /* [29:24],        r/w,       0x32 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lna_gain_tale_1;

    /* 0xCE8 : lna_gain_tale_2 */
    union {
        struct {
            uint32_t lna_gain_8                     :  6; /* [ 5: 0],        r/w,       0x37 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lna_gain_tale_2;

};

typedef volatile struct bz_phy_agc_reg bz_phy_agc_reg_t;


#endif  /* __BZ_PHY_AGC_REG_H__ */
