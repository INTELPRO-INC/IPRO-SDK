/**
  ******************************************************************************
  * @file    rf_reg.h
  * @version V1.0
  * @date    2022-03-01
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2024 IPRO</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of IPRO nor the names of its contributors
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
#ifndef  __RF_REG_H__
#define  __RF_REG_H__

#include <stdint.h>

/* 0x0 : Silicon revision */
#define RF_REV_OFFSET                                           (0x0)
#define RF_ID                                                   RF_ID
#define RF_ID_POS                                               (0U)
#define RF_ID_LEN                                               (8U)
#define RF_ID_MSK                                               (((1U<<RF_ID_LEN)-1)<<RF_ID_POS)
#define RF_ID_UMSK                                              (~(((1U<<RF_ID_LEN)-1)<<RF_ID_POS))
#define RF_FW_REV                                               RF_FW_REV
#define RF_FW_REV_POS                                           (8U)
#define RF_FW_REV_LEN                                           (8U)
#define RF_FW_REV_MSK                                           (((1U<<RF_FW_REV_LEN)-1)<<RF_FW_REV_POS)
#define RF_FW_REV_UMSK                                          (~(((1U<<RF_FW_REV_LEN)-1)<<RF_FW_REV_POS))
#define RF_HW_REV                                               RF_HW_REV
#define RF_HW_REV_POS                                           (16U)
#define RF_HW_REV_LEN                                           (8U)
#define RF_HW_REV_MSK                                           (((1U<<RF_HW_REV_LEN)-1)<<RF_HW_REV_POS)
#define RF_HW_REV_UMSK                                          (~(((1U<<RF_HW_REV_LEN)-1)<<RF_HW_REV_POS))

/* 0x4 : Digital Control */
#define RF_FSM_CTRL_HW_OFFSET                                   (0x4)
#define RF_FSM_CTRL_EN                                          RF_FSM_CTRL_EN
#define RF_FSM_CTRL_EN_POS                                      (1U)
#define RF_FSM_CTRL_EN_LEN                                      (1U)
#define RF_FSM_CTRL_EN_MSK                                      (((1U<<RF_FSM_CTRL_EN_LEN)-1)<<RF_FSM_CTRL_EN_POS)
#define RF_FSM_CTRL_EN_UMSK                                     (~(((1U<<RF_FSM_CTRL_EN_LEN)-1)<<RF_FSM_CTRL_EN_POS))
#define RF_FSM_T2R_CAL_MODE                                     RF_FSM_T2R_CAL_MODE
#define RF_FSM_T2R_CAL_MODE_POS                                 (2U)
#define RF_FSM_T2R_CAL_MODE_LEN                                 (2U)
#define RF_FSM_T2R_CAL_MODE_MSK                                 (((1U<<RF_FSM_T2R_CAL_MODE_LEN)-1)<<RF_FSM_T2R_CAL_MODE_POS)
#define RF_FSM_T2R_CAL_MODE_UMSK                                (~(((1U<<RF_FSM_T2R_CAL_MODE_LEN)-1)<<RF_FSM_T2R_CAL_MODE_POS))
#define RF_FSM_STATE                                            RF_FSM_STATE
#define RF_FSM_STATE_POS                                        (4U)
#define RF_FSM_STATE_LEN                                        (3U)
#define RF_FSM_STATE_MSK                                        (((1U<<RF_FSM_STATE_LEN)-1)<<RF_FSM_STATE_POS)
#define RF_FSM_STATE_UMSK                                       (~(((1U<<RF_FSM_STATE_LEN)-1)<<RF_FSM_STATE_POS))
#define RF_RC_STATE_DBG                                         RF_RC_STATE_DBG
#define RF_RC_STATE_DBG_POS                                     (8U)
#define RF_RC_STATE_DBG_LEN                                     (3U)
#define RF_RC_STATE_DBG_MSK                                     (((1U<<RF_RC_STATE_DBG_LEN)-1)<<RF_RC_STATE_DBG_POS)
#define RF_RC_STATE_DBG_UMSK                                    (~(((1U<<RF_RC_STATE_DBG_LEN)-1)<<RF_RC_STATE_DBG_POS))
#define RF_RC_STATE_DBG_EN                                      RF_RC_STATE_DBG_EN
#define RF_RC_STATE_DBG_EN_POS                                  (11U)
#define RF_RC_STATE_DBG_EN_LEN                                  (1U)
#define RF_RC_STATE_DBG_EN_MSK                                  (((1U<<RF_RC_STATE_DBG_EN_LEN)-1)<<RF_RC_STATE_DBG_EN_POS)
#define RF_RC_STATE_DBG_EN_UMSK                                 (~(((1U<<RF_RC_STATE_DBG_EN_LEN)-1)<<RF_RC_STATE_DBG_EN_POS))
#define RF_FSM_ST_INT_SEL                                       RF_FSM_ST_INT_SEL
#define RF_FSM_ST_INT_SEL_POS                                   (12U)
#define RF_FSM_ST_INT_SEL_LEN                                   (3U)
#define RF_FSM_ST_INT_SEL_MSK                                   (((1U<<RF_FSM_ST_INT_SEL_LEN)-1)<<RF_FSM_ST_INT_SEL_POS)
#define RF_FSM_ST_INT_SEL_UMSK                                  (~(((1U<<RF_FSM_ST_INT_SEL_LEN)-1)<<RF_FSM_ST_INT_SEL_POS))
#define RF_FSM_ST_INT                                           RF_FSM_ST_INT
#define RF_FSM_ST_INT_POS                                       (16U)
#define RF_FSM_ST_INT_LEN                                       (1U)
#define RF_FSM_ST_INT_MSK                                       (((1U<<RF_FSM_ST_INT_LEN)-1)<<RF_FSM_ST_INT_POS)
#define RF_FSM_ST_INT_UMSK                                      (~(((1U<<RF_FSM_ST_INT_LEN)-1)<<RF_FSM_ST_INT_POS))
#define RF_FSM_ST_INT_CLR                                       RF_FSM_ST_INT_CLR
#define RF_FSM_ST_INT_CLR_POS                                   (20U)
#define RF_FSM_ST_INT_CLR_LEN                                   (1U)
#define RF_FSM_ST_INT_CLR_MSK                                   (((1U<<RF_FSM_ST_INT_CLR_LEN)-1)<<RF_FSM_ST_INT_CLR_POS)
#define RF_FSM_ST_INT_CLR_UMSK                                  (~(((1U<<RF_FSM_ST_INT_CLR_LEN)-1)<<RF_FSM_ST_INT_CLR_POS))
#define RF_FSM_ST_INT_SET                                       RF_FSM_ST_INT_SET
#define RF_FSM_ST_INT_SET_POS                                   (24U)
#define RF_FSM_ST_INT_SET_LEN                                   (1U)
#define RF_FSM_ST_INT_SET_MSK                                   (((1U<<RF_FSM_ST_INT_SET_LEN)-1)<<RF_FSM_ST_INT_SET_POS)
#define RF_FSM_ST_INT_SET_UMSK                                  (~(((1U<<RF_FSM_ST_INT_SET_LEN)-1)<<RF_FSM_ST_INT_SET_POS))
#define RF_RC_STATE_VALUE                                       RF_RC_STATE_VALUE
#define RF_RC_STATE_VALUE_POS                                   (28U)
#define RF_RC_STATE_VALUE_LEN                                   (3U)
#define RF_RC_STATE_VALUE_MSK                                   (((1U<<RF_RC_STATE_VALUE_LEN)-1)<<RF_RC_STATE_VALUE_POS)
#define RF_RC_STATE_VALUE_UMSK                                  (~(((1U<<RF_RC_STATE_VALUE_LEN)-1)<<RF_RC_STATE_VALUE_POS))

/* 0x8 : rfsm status reg */
#define RF_FSM_CTRL_SW_OFFSET                                   (0x8)
#define RF_FSM_SW_ST                                            RF_FSM_SW_ST
#define RF_FSM_SW_ST_POS                                        (0U)
#define RF_FSM_SW_ST_LEN                                        (5U)
#define RF_FSM_SW_ST_MSK                                        (((1U<<RF_FSM_SW_ST_LEN)-1)<<RF_FSM_SW_ST_POS)
#define RF_FSM_SW_ST_UMSK                                       (~(((1U<<RF_FSM_SW_ST_LEN)-1)<<RF_FSM_SW_ST_POS))
#define RF_FSM_SW_ST_VLD                                        RF_FSM_SW_ST_VLD
#define RF_FSM_SW_ST_VLD_POS                                    (8U)
#define RF_FSM_SW_ST_VLD_LEN                                    (1U)
#define RF_FSM_SW_ST_VLD_MSK                                    (((1U<<RF_FSM_SW_ST_VLD_LEN)-1)<<RF_FSM_SW_ST_VLD_POS)
#define RF_FSM_SW_ST_VLD_UMSK                                   (~(((1U<<RF_FSM_SW_ST_VLD_LEN)-1)<<RF_FSM_SW_ST_VLD_POS))
#define RF_FULL_CAL_EN                                          RF_FULL_CAL_EN
#define RF_FULL_CAL_EN_POS                                      (12U)
#define RF_FULL_CAL_EN_LEN                                      (1U)
#define RF_FULL_CAL_EN_MSK                                      (((1U<<RF_FULL_CAL_EN_LEN)-1)<<RF_FULL_CAL_EN_POS)
#define RF_FULL_CAL_EN_UMSK                                     (~(((1U<<RF_FULL_CAL_EN_LEN)-1)<<RF_FULL_CAL_EN_POS))
#define RF_INC_CAL_TIMEOUT                                      RF_INC_CAL_TIMEOUT
#define RF_INC_CAL_TIMEOUT_POS                                  (16U)
#define RF_INC_CAL_TIMEOUT_LEN                                  (1U)
#define RF_INC_CAL_TIMEOUT_MSK                                  (((1U<<RF_INC_CAL_TIMEOUT_LEN)-1)<<RF_INC_CAL_TIMEOUT_POS)
#define RF_INC_CAL_TIMEOUT_UMSK                                 (~(((1U<<RF_INC_CAL_TIMEOUT_LEN)-1)<<RF_INC_CAL_TIMEOUT_POS))
#define RF_LO_UNLOCKED                                          RF_LO_UNLOCKED
#define RF_LO_UNLOCKED_POS                                      (20U)
#define RF_LO_UNLOCKED_LEN                                      (1U)
#define RF_LO_UNLOCKED_MSK                                      (((1U<<RF_LO_UNLOCKED_LEN)-1)<<RF_LO_UNLOCKED_POS)
#define RF_LO_UNLOCKED_UMSK                                     (~(((1U<<RF_LO_UNLOCKED_LEN)-1)<<RF_LO_UNLOCKED_POS))

/* 0xC : Control logic switch */
#define RFCTRL_HW_EN_OFFSET                                     (0xC)
#define RF_RESERVED3                                            RF_RESERVED3
#define RF_RESERVED3_POS                                        (0U)
#define RF_RESERVED3_LEN                                        (32U)
#define RF_RESERVED3_MSK                                        (((1U<<RF_RESERVED3_LEN)-1)<<RF_RESERVED3_POS)
#define RF_RESERVED3_UMSK                                       (~(((1U<<RF_RESERVED3_LEN)-1)<<RF_RESERVED3_POS))

/* 0x10 : temp_comp */
#define RF_TEMP_COMP_OFFSET                                     (0x10)
#define RF_CONST_ACAL                                           RF_CONST_ACAL
#define RF_CONST_ACAL_POS                                       (0U)
#define RF_CONST_ACAL_LEN                                       (8U)
#define RF_CONST_ACAL_MSK                                       (((1U<<RF_CONST_ACAL_LEN)-1)<<RF_CONST_ACAL_POS)
#define RF_CONST_ACAL_UMSK                                      (~(((1U<<RF_CONST_ACAL_LEN)-1)<<RF_CONST_ACAL_POS))
#define RF_CONST_FCAL                                           RF_CONST_FCAL
#define RF_CONST_FCAL_POS                                       (8U)
#define RF_CONST_FCAL_LEN                                       (8U)
#define RF_CONST_FCAL_MSK                                       (((1U<<RF_CONST_FCAL_LEN)-1)<<RF_CONST_FCAL_POS)
#define RF_CONST_FCAL_UMSK                                      (~(((1U<<RF_CONST_FCAL_LEN)-1)<<RF_CONST_FCAL_POS))
#define RF_TEMP_COMP_EN                                         RF_TEMP_COMP_EN
#define RF_TEMP_COMP_EN_POS                                     (16U)
#define RF_TEMP_COMP_EN_LEN                                     (1U)
#define RF_TEMP_COMP_EN_MSK                                     (((1U<<RF_TEMP_COMP_EN_LEN)-1)<<RF_TEMP_COMP_EN_POS)
#define RF_TEMP_COMP_EN_UMSK                                    (~(((1U<<RF_TEMP_COMP_EN_LEN)-1)<<RF_TEMP_COMP_EN_POS))

/* 0x14 : rfcal_status */
#define RFCAL_STATUS_OFFSET                                     (0x14)
#define RF_RCAL_STATUS                                          RF_RCAL_STATUS
#define RF_RCAL_STATUS_POS                                      (0U)
#define RF_RCAL_STATUS_LEN                                      (2U)
#define RF_RCAL_STATUS_MSK                                      (((1U<<RF_RCAL_STATUS_LEN)-1)<<RF_RCAL_STATUS_POS)
#define RF_RCAL_STATUS_UMSK                                     (~(((1U<<RF_RCAL_STATUS_LEN)-1)<<RF_RCAL_STATUS_POS))
#define RF_ADC_OSCAL_STATUS                                     RF_ADC_OSCAL_STATUS
#define RF_ADC_OSCAL_STATUS_POS                                 (2U)
#define RF_ADC_OSCAL_STATUS_LEN                                 (2U)
#define RF_ADC_OSCAL_STATUS_MSK                                 (((1U<<RF_ADC_OSCAL_STATUS_LEN)-1)<<RF_ADC_OSCAL_STATUS_POS)
#define RF_ADC_OSCAL_STATUS_UMSK                                (~(((1U<<RF_ADC_OSCAL_STATUS_LEN)-1)<<RF_ADC_OSCAL_STATUS_POS))
#define RF_FCAL_STATUS                                          RF_FCAL_STATUS
#define RF_FCAL_STATUS_POS                                      (4U)
#define RF_FCAL_STATUS_LEN                                      (2U)
#define RF_FCAL_STATUS_MSK                                      (((1U<<RF_FCAL_STATUS_LEN)-1)<<RF_FCAL_STATUS_POS)
#define RF_FCAL_STATUS_UMSK                                     (~(((1U<<RF_FCAL_STATUS_LEN)-1)<<RF_FCAL_STATUS_POS))
#define RF_ACAL_STATUS                                          RF_ACAL_STATUS
#define RF_ACAL_STATUS_POS                                      (6U)
#define RF_ACAL_STATUS_LEN                                      (2U)
#define RF_ACAL_STATUS_MSK                                      (((1U<<RF_ACAL_STATUS_LEN)-1)<<RF_ACAL_STATUS_POS)
#define RF_ACAL_STATUS_UMSK                                     (~(((1U<<RF_ACAL_STATUS_LEN)-1)<<RF_ACAL_STATUS_POS))
#define RF_INC_FCAL_STATUS                                      RF_INC_FCAL_STATUS
#define RF_INC_FCAL_STATUS_POS                                  (8U)
#define RF_INC_FCAL_STATUS_LEN                                  (2U)
#define RF_INC_FCAL_STATUS_MSK                                  (((1U<<RF_INC_FCAL_STATUS_LEN)-1)<<RF_INC_FCAL_STATUS_POS)
#define RF_INC_FCAL_STATUS_UMSK                                 (~(((1U<<RF_INC_FCAL_STATUS_LEN)-1)<<RF_INC_FCAL_STATUS_POS))
#define RF_INC_ACAL_STATUS                                      RF_INC_ACAL_STATUS
#define RF_INC_ACAL_STATUS_POS                                  (10U)
#define RF_INC_ACAL_STATUS_LEN                                  (2U)
#define RF_INC_ACAL_STATUS_MSK                                  (((1U<<RF_INC_ACAL_STATUS_LEN)-1)<<RF_INC_ACAL_STATUS_POS)
#define RF_INC_ACAL_STATUS_UMSK                                 (~(((1U<<RF_INC_ACAL_STATUS_LEN)-1)<<RF_INC_ACAL_STATUS_POS))
#define RF_CLKPLL_CAL_STATUS                                    RF_CLKPLL_CAL_STATUS
#define RF_CLKPLL_CAL_STATUS_POS                                (12U)
#define RF_CLKPLL_CAL_STATUS_LEN                                (2U)
#define RF_CLKPLL_CAL_STATUS_MSK                                (((1U<<RF_CLKPLL_CAL_STATUS_LEN)-1)<<RF_CLKPLL_CAL_STATUS_POS)
#define RF_CLKPLL_CAL_STATUS_UMSK                               (~(((1U<<RF_CLKPLL_CAL_STATUS_LEN)-1)<<RF_CLKPLL_CAL_STATUS_POS))
#define RF_ROS_STATUS                                           RF_ROS_STATUS
#define RF_ROS_STATUS_POS                                       (14U)
#define RF_ROS_STATUS_LEN                                       (2U)
#define RF_ROS_STATUS_MSK                                       (((1U<<RF_ROS_STATUS_LEN)-1)<<RF_ROS_STATUS_POS)
#define RF_ROS_STATUS_UMSK                                      (~(((1U<<RF_ROS_STATUS_LEN)-1)<<RF_ROS_STATUS_POS))
#define RF_TOS_STATUS                                           RF_TOS_STATUS
#define RF_TOS_STATUS_POS                                       (16U)
#define RF_TOS_STATUS_LEN                                       (2U)
#define RF_TOS_STATUS_MSK                                       (((1U<<RF_TOS_STATUS_LEN)-1)<<RF_TOS_STATUS_POS)
#define RF_TOS_STATUS_UMSK                                      (~(((1U<<RF_TOS_STATUS_LEN)-1)<<RF_TOS_STATUS_POS))
#define RF_RCCAL_STATUS                                         RF_RCCAL_STATUS
#define RF_RCCAL_STATUS_POS                                     (18U)
#define RF_RCCAL_STATUS_LEN                                     (2U)
#define RF_RCCAL_STATUS_MSK                                     (((1U<<RF_RCCAL_STATUS_LEN)-1)<<RF_RCCAL_STATUS_POS)
#define RF_RCCAL_STATUS_UMSK                                    (~(((1U<<RF_RCCAL_STATUS_LEN)-1)<<RF_RCCAL_STATUS_POS))
#define RF_LO_LEAKCAL_STATUS                                    RF_LO_LEAKCAL_STATUS
#define RF_LO_LEAKCAL_STATUS_POS                                (20U)
#define RF_LO_LEAKCAL_STATUS_LEN                                (2U)
#define RF_LO_LEAKCAL_STATUS_MSK                                (((1U<<RF_LO_LEAKCAL_STATUS_LEN)-1)<<RF_LO_LEAKCAL_STATUS_POS)
#define RF_LO_LEAKCAL_STATUS_UMSK                               (~(((1U<<RF_LO_LEAKCAL_STATUS_LEN)-1)<<RF_LO_LEAKCAL_STATUS_POS))
#define RF_TIQCAL_STATUS_RESV                                   RF_TIQCAL_STATUS_RESV
#define RF_TIQCAL_STATUS_RESV_POS                               (22U)
#define RF_TIQCAL_STATUS_RESV_LEN                               (2U)
#define RF_TIQCAL_STATUS_RESV_MSK                               (((1U<<RF_TIQCAL_STATUS_RESV_LEN)-1)<<RF_TIQCAL_STATUS_RESV_POS)
#define RF_TIQCAL_STATUS_RESV_UMSK                              (~(((1U<<RF_TIQCAL_STATUS_RESV_LEN)-1)<<RF_TIQCAL_STATUS_RESV_POS))
#define RF_RIQCAL_STATUS_RESV                                   RF_RIQCAL_STATUS_RESV
#define RF_RIQCAL_STATUS_RESV_POS                               (24U)
#define RF_RIQCAL_STATUS_RESV_LEN                               (2U)
#define RF_RIQCAL_STATUS_RESV_MSK                               (((1U<<RF_RIQCAL_STATUS_RESV_LEN)-1)<<RF_RIQCAL_STATUS_RESV_POS)
#define RF_RIQCAL_STATUS_RESV_UMSK                              (~(((1U<<RF_RIQCAL_STATUS_RESV_LEN)-1)<<RF_RIQCAL_STATUS_RESV_POS))
#define RF_PWDET_CAL_STATUS                                     RF_PWDET_CAL_STATUS
#define RF_PWDET_CAL_STATUS_POS                                 (26U)
#define RF_PWDET_CAL_STATUS_LEN                                 (2U)
#define RF_PWDET_CAL_STATUS_MSK                                 (((1U<<RF_PWDET_CAL_STATUS_LEN)-1)<<RF_PWDET_CAL_STATUS_POS)
#define RF_PWDET_CAL_STATUS_UMSK                                (~(((1U<<RF_PWDET_CAL_STATUS_LEN)-1)<<RF_PWDET_CAL_STATUS_POS))
#define RF_TENSCAL_STATUS                                       RF_TENSCAL_STATUS
#define RF_TENSCAL_STATUS_POS                                   (28U)
#define RF_TENSCAL_STATUS_LEN                                   (2U)
#define RF_TENSCAL_STATUS_MSK                                   (((1U<<RF_TENSCAL_STATUS_LEN)-1)<<RF_TENSCAL_STATUS_POS)
#define RF_TENSCAL_STATUS_UMSK                                  (~(((1U<<RF_TENSCAL_STATUS_LEN)-1)<<RF_TENSCAL_STATUS_POS))
#define RF_DPD_STATUS                                           RF_DPD_STATUS
#define RF_DPD_STATUS_POS                                       (30U)
#define RF_DPD_STATUS_LEN                                       (2U)
#define RF_DPD_STATUS_MSK                                       (((1U<<RF_DPD_STATUS_LEN)-1)<<RF_DPD_STATUS_POS)
#define RF_DPD_STATUS_UMSK                                      (~(((1U<<RF_DPD_STATUS_LEN)-1)<<RF_DPD_STATUS_POS))

/* 0x18 : rfcal_status2 */
#define RFCAL_STATUS2_OFFSET                                    (0x18)
#define RF_DL_RFCAL_TABLE_STATUS                                RF_DL_RFCAL_TABLE_STATUS
#define RF_DL_RFCAL_TABLE_STATUS_POS                            (0U)
#define RF_DL_RFCAL_TABLE_STATUS_LEN                            (2U)
#define RF_DL_RFCAL_TABLE_STATUS_MSK                            (((1U<<RF_DL_RFCAL_TABLE_STATUS_LEN)-1)<<RF_DL_RFCAL_TABLE_STATUS_POS)
#define RF_DL_RFCAL_TABLE_STATUS_UMSK                           (~(((1U<<RF_DL_RFCAL_TABLE_STATUS_LEN)-1)<<RF_DL_RFCAL_TABLE_STATUS_POS))

/* 0x1C : Calibration mode register */
#define RFCAL_CTRLEN_OFFSET                                     (0x1C)
#define RF_RCAL_EN_RESV                                         RF_RCAL_EN_RESV
#define RF_RCAL_EN_RESV_POS                                     (0U)
#define RF_RCAL_EN_RESV_LEN                                     (1U)
#define RF_RCAL_EN_RESV_MSK                                     (((1U<<RF_RCAL_EN_RESV_LEN)-1)<<RF_RCAL_EN_RESV_POS)
#define RF_RCAL_EN_RESV_UMSK                                    (~(((1U<<RF_RCAL_EN_RESV_LEN)-1)<<RF_RCAL_EN_RESV_POS))
#define RF_ADC_OSCAL_EN                                         RF_ADC_OSCAL_EN
#define RF_ADC_OSCAL_EN_POS                                     (1U)
#define RF_ADC_OSCAL_EN_LEN                                     (1U)
#define RF_ADC_OSCAL_EN_MSK                                     (((1U<<RF_ADC_OSCAL_EN_LEN)-1)<<RF_ADC_OSCAL_EN_POS)
#define RF_ADC_OSCAL_EN_UMSK                                    (~(((1U<<RF_ADC_OSCAL_EN_LEN)-1)<<RF_ADC_OSCAL_EN_POS))
#define RF_DL_RFCAL_TABLE_EN                                    RF_DL_RFCAL_TABLE_EN
#define RF_DL_RFCAL_TABLE_EN_POS                                (2U)
#define RF_DL_RFCAL_TABLE_EN_LEN                                (1U)
#define RF_DL_RFCAL_TABLE_EN_MSK                                (((1U<<RF_DL_RFCAL_TABLE_EN_LEN)-1)<<RF_DL_RFCAL_TABLE_EN_POS)
#define RF_DL_RFCAL_TABLE_EN_UMSK                               (~(((1U<<RF_DL_RFCAL_TABLE_EN_LEN)-1)<<RF_DL_RFCAL_TABLE_EN_POS))
#define RF_FCAL_EN                                              RF_FCAL_EN
#define RF_FCAL_EN_POS                                          (3U)
#define RF_FCAL_EN_LEN                                          (1U)
#define RF_FCAL_EN_MSK                                          (((1U<<RF_FCAL_EN_LEN)-1)<<RF_FCAL_EN_POS)
#define RF_FCAL_EN_UMSK                                         (~(((1U<<RF_FCAL_EN_LEN)-1)<<RF_FCAL_EN_POS))
#define RF_ACAL_EN                                              RF_ACAL_EN
#define RF_ACAL_EN_POS                                          (4U)
#define RF_ACAL_EN_LEN                                          (1U)
#define RF_ACAL_EN_MSK                                          (((1U<<RF_ACAL_EN_LEN)-1)<<RF_ACAL_EN_POS)
#define RF_ACAL_EN_UMSK                                         (~(((1U<<RF_ACAL_EN_LEN)-1)<<RF_ACAL_EN_POS))
#define RF_FCAL_INC_EN                                          RF_FCAL_INC_EN
#define RF_FCAL_INC_EN_POS                                      (5U)
#define RF_FCAL_INC_EN_LEN                                      (1U)
#define RF_FCAL_INC_EN_MSK                                      (((1U<<RF_FCAL_INC_EN_LEN)-1)<<RF_FCAL_INC_EN_POS)
#define RF_FCAL_INC_EN_UMSK                                     (~(((1U<<RF_FCAL_INC_EN_LEN)-1)<<RF_FCAL_INC_EN_POS))
#define RF_ACAL_INC_EN                                          RF_ACAL_INC_EN
#define RF_ACAL_INC_EN_POS                                      (6U)
#define RF_ACAL_INC_EN_LEN                                      (1U)
#define RF_ACAL_INC_EN_MSK                                      (((1U<<RF_ACAL_INC_EN_LEN)-1)<<RF_ACAL_INC_EN_POS)
#define RF_ACAL_INC_EN_UMSK                                     (~(((1U<<RF_ACAL_INC_EN_LEN)-1)<<RF_ACAL_INC_EN_POS))
#define RF_ROSCAL_INC_EN                                        RF_ROSCAL_INC_EN
#define RF_ROSCAL_INC_EN_POS                                    (7U)
#define RF_ROSCAL_INC_EN_LEN                                    (1U)
#define RF_ROSCAL_INC_EN_MSK                                    (((1U<<RF_ROSCAL_INC_EN_LEN)-1)<<RF_ROSCAL_INC_EN_POS)
#define RF_ROSCAL_INC_EN_UMSK                                   (~(((1U<<RF_ROSCAL_INC_EN_LEN)-1)<<RF_ROSCAL_INC_EN_POS))
#define RF_CLKPLL_CAL_EN                                        RF_CLKPLL_CAL_EN
#define RF_CLKPLL_CAL_EN_POS                                    (8U)
#define RF_CLKPLL_CAL_EN_LEN                                    (1U)
#define RF_CLKPLL_CAL_EN_MSK                                    (((1U<<RF_CLKPLL_CAL_EN_LEN)-1)<<RF_CLKPLL_CAL_EN_POS)
#define RF_CLKPLL_CAL_EN_UMSK                                   (~(((1U<<RF_CLKPLL_CAL_EN_LEN)-1)<<RF_CLKPLL_CAL_EN_POS))
#define RF_ROSCAL_EN                                            RF_ROSCAL_EN
#define RF_ROSCAL_EN_POS                                        (9U)
#define RF_ROSCAL_EN_LEN                                        (1U)
#define RF_ROSCAL_EN_MSK                                        (((1U<<RF_ROSCAL_EN_LEN)-1)<<RF_ROSCAL_EN_POS)
#define RF_ROSCAL_EN_UMSK                                       (~(((1U<<RF_ROSCAL_EN_LEN)-1)<<RF_ROSCAL_EN_POS))
#define RF_TOSCAL_EN                                            RF_TOSCAL_EN
#define RF_TOSCAL_EN_POS                                        (10U)
#define RF_TOSCAL_EN_LEN                                        (1U)
#define RF_TOSCAL_EN_MSK                                        (((1U<<RF_TOSCAL_EN_LEN)-1)<<RF_TOSCAL_EN_POS)
#define RF_TOSCAL_EN_UMSK                                       (~(((1U<<RF_TOSCAL_EN_LEN)-1)<<RF_TOSCAL_EN_POS))
#define RF_RCCAL_EN                                             RF_RCCAL_EN
#define RF_RCCAL_EN_POS                                         (11U)
#define RF_RCCAL_EN_LEN                                         (1U)
#define RF_RCCAL_EN_MSK                                         (((1U<<RF_RCCAL_EN_LEN)-1)<<RF_RCCAL_EN_POS)
#define RF_RCCAL_EN_UMSK                                        (~(((1U<<RF_RCCAL_EN_LEN)-1)<<RF_RCCAL_EN_POS))
#define RF_LO_LEAKCAL_EN                                        RF_LO_LEAKCAL_EN
#define RF_LO_LEAKCAL_EN_POS                                    (12U)
#define RF_LO_LEAKCAL_EN_LEN                                    (1U)
#define RF_LO_LEAKCAL_EN_MSK                                    (((1U<<RF_LO_LEAKCAL_EN_LEN)-1)<<RF_LO_LEAKCAL_EN_POS)
#define RF_LO_LEAKCAL_EN_UMSK                                   (~(((1U<<RF_LO_LEAKCAL_EN_LEN)-1)<<RF_LO_LEAKCAL_EN_POS))
#define RF_TIQCAL_EN                                            RF_TIQCAL_EN
#define RF_TIQCAL_EN_POS                                        (13U)
#define RF_TIQCAL_EN_LEN                                        (1U)
#define RF_TIQCAL_EN_MSK                                        (((1U<<RF_TIQCAL_EN_LEN)-1)<<RF_TIQCAL_EN_POS)
#define RF_TIQCAL_EN_UMSK                                       (~(((1U<<RF_TIQCAL_EN_LEN)-1)<<RF_TIQCAL_EN_POS))
#define RF_RIQCAL_EN                                            RF_RIQCAL_EN
#define RF_RIQCAL_EN_POS                                        (14U)
#define RF_RIQCAL_EN_LEN                                        (1U)
#define RF_RIQCAL_EN_MSK                                        (((1U<<RF_RIQCAL_EN_LEN)-1)<<RF_RIQCAL_EN_POS)
#define RF_RIQCAL_EN_UMSK                                       (~(((1U<<RF_RIQCAL_EN_LEN)-1)<<RF_RIQCAL_EN_POS))
#define RF_PWDET_CAL_EN                                         RF_PWDET_CAL_EN
#define RF_PWDET_CAL_EN_POS                                     (15U)
#define RF_PWDET_CAL_EN_LEN                                     (1U)
#define RF_PWDET_CAL_EN_MSK                                     (((1U<<RF_PWDET_CAL_EN_LEN)-1)<<RF_PWDET_CAL_EN_POS)
#define RF_PWDET_CAL_EN_UMSK                                    (~(((1U<<RF_PWDET_CAL_EN_LEN)-1)<<RF_PWDET_CAL_EN_POS))
#define RF_TSENCAL_EN                                           RF_TSENCAL_EN
#define RF_TSENCAL_EN_POS                                       (16U)
#define RF_TSENCAL_EN_LEN                                       (1U)
#define RF_TSENCAL_EN_MSK                                       (((1U<<RF_TSENCAL_EN_LEN)-1)<<RF_TSENCAL_EN_POS)
#define RF_TSENCAL_EN_UMSK                                      (~(((1U<<RF_TSENCAL_EN_LEN)-1)<<RF_TSENCAL_EN_POS))
#define RF_DPD_EN                                               RF_DPD_EN
#define RF_DPD_EN_POS                                           (17U)
#define RF_DPD_EN_LEN                                           (1U)
#define RF_DPD_EN_MSK                                           (((1U<<RF_DPD_EN_LEN)-1)<<RF_DPD_EN_POS)
#define RF_DPD_EN_UMSK                                          (~(((1U<<RF_DPD_EN_LEN)-1)<<RF_DPD_EN_POS))

/* 0x20 : rf calibration state enabl in full cal list */
#define RFCAL_STATEEN_OFFSET                                    (0x20)
#define RF_RCAL_STEN_RESV                                       RF_RCAL_STEN_RESV
#define RF_RCAL_STEN_RESV_POS                                   (0U)
#define RF_RCAL_STEN_RESV_LEN                                   (1U)
#define RF_RCAL_STEN_RESV_MSK                                   (((1U<<RF_RCAL_STEN_RESV_LEN)-1)<<RF_RCAL_STEN_RESV_POS)
#define RF_RCAL_STEN_RESV_UMSK                                  (~(((1U<<RF_RCAL_STEN_RESV_LEN)-1)<<RF_RCAL_STEN_RESV_POS))
#define RF_ADC_OSCAL_STEN                                       RF_ADC_OSCAL_STEN
#define RF_ADC_OSCAL_STEN_POS                                   (1U)
#define RF_ADC_OSCAL_STEN_LEN                                   (1U)
#define RF_ADC_OSCAL_STEN_MSK                                   (((1U<<RF_ADC_OSCAL_STEN_LEN)-1)<<RF_ADC_OSCAL_STEN_POS)
#define RF_ADC_OSCAL_STEN_UMSK                                  (~(((1U<<RF_ADC_OSCAL_STEN_LEN)-1)<<RF_ADC_OSCAL_STEN_POS))
#define RF_DL_RFCAL_TABLE_STEN                                  RF_DL_RFCAL_TABLE_STEN
#define RF_DL_RFCAL_TABLE_STEN_POS                              (2U)
#define RF_DL_RFCAL_TABLE_STEN_LEN                              (1U)
#define RF_DL_RFCAL_TABLE_STEN_MSK                              (((1U<<RF_DL_RFCAL_TABLE_STEN_LEN)-1)<<RF_DL_RFCAL_TABLE_STEN_POS)
#define RF_DL_RFCAL_TABLE_STEN_UMSK                             (~(((1U<<RF_DL_RFCAL_TABLE_STEN_LEN)-1)<<RF_DL_RFCAL_TABLE_STEN_POS))
#define RF_FCAL_STEN                                            RF_FCAL_STEN
#define RF_FCAL_STEN_POS                                        (3U)
#define RF_FCAL_STEN_LEN                                        (1U)
#define RF_FCAL_STEN_MSK                                        (((1U<<RF_FCAL_STEN_LEN)-1)<<RF_FCAL_STEN_POS)
#define RF_FCAL_STEN_UMSK                                       (~(((1U<<RF_FCAL_STEN_LEN)-1)<<RF_FCAL_STEN_POS))
#define RF_ACAL_STEN                                            RF_ACAL_STEN
#define RF_ACAL_STEN_POS                                        (4U)
#define RF_ACAL_STEN_LEN                                        (1U)
#define RF_ACAL_STEN_MSK                                        (((1U<<RF_ACAL_STEN_LEN)-1)<<RF_ACAL_STEN_POS)
#define RF_ACAL_STEN_UMSK                                       (~(((1U<<RF_ACAL_STEN_LEN)-1)<<RF_ACAL_STEN_POS))
#define RF_INC_FCAL_STEN                                        RF_INC_FCAL_STEN
#define RF_INC_FCAL_STEN_POS                                    (5U)
#define RF_INC_FCAL_STEN_LEN                                    (1U)
#define RF_INC_FCAL_STEN_MSK                                    (((1U<<RF_INC_FCAL_STEN_LEN)-1)<<RF_INC_FCAL_STEN_POS)
#define RF_INC_FCAL_STEN_UMSK                                   (~(((1U<<RF_INC_FCAL_STEN_LEN)-1)<<RF_INC_FCAL_STEN_POS))
#define RF_INC_ACAL_STEN                                        RF_INC_ACAL_STEN
#define RF_INC_ACAL_STEN_POS                                    (6U)
#define RF_INC_ACAL_STEN_LEN                                    (1U)
#define RF_INC_ACAL_STEN_MSK                                    (((1U<<RF_INC_ACAL_STEN_LEN)-1)<<RF_INC_ACAL_STEN_POS)
#define RF_INC_ACAL_STEN_UMSK                                   (~(((1U<<RF_INC_ACAL_STEN_LEN)-1)<<RF_INC_ACAL_STEN_POS))
#define RF_CLKPLL_CAL_STEN                                      RF_CLKPLL_CAL_STEN
#define RF_CLKPLL_CAL_STEN_POS                                  (7U)
#define RF_CLKPLL_CAL_STEN_LEN                                  (1U)
#define RF_CLKPLL_CAL_STEN_MSK                                  (((1U<<RF_CLKPLL_CAL_STEN_LEN)-1)<<RF_CLKPLL_CAL_STEN_POS)
#define RF_CLKPLL_CAL_STEN_UMSK                                 (~(((1U<<RF_CLKPLL_CAL_STEN_LEN)-1)<<RF_CLKPLL_CAL_STEN_POS))
#define RF_ROSCAL_STEN                                          RF_ROSCAL_STEN
#define RF_ROSCAL_STEN_POS                                      (8U)
#define RF_ROSCAL_STEN_LEN                                      (1U)
#define RF_ROSCAL_STEN_MSK                                      (((1U<<RF_ROSCAL_STEN_LEN)-1)<<RF_ROSCAL_STEN_POS)
#define RF_ROSCAL_STEN_UMSK                                     (~(((1U<<RF_ROSCAL_STEN_LEN)-1)<<RF_ROSCAL_STEN_POS))
#define RF_TOSCAL_STEN_RESV                                     RF_TOSCAL_STEN_RESV
#define RF_TOSCAL_STEN_RESV_POS                                 (9U)
#define RF_TOSCAL_STEN_RESV_LEN                                 (1U)
#define RF_TOSCAL_STEN_RESV_MSK                                 (((1U<<RF_TOSCAL_STEN_RESV_LEN)-1)<<RF_TOSCAL_STEN_RESV_POS)
#define RF_TOSCAL_STEN_RESV_UMSK                                (~(((1U<<RF_TOSCAL_STEN_RESV_LEN)-1)<<RF_TOSCAL_STEN_RESV_POS))
#define RF_RCCAL_STEN                                           RF_RCCAL_STEN
#define RF_RCCAL_STEN_POS                                       (10U)
#define RF_RCCAL_STEN_LEN                                       (1U)
#define RF_RCCAL_STEN_MSK                                       (((1U<<RF_RCCAL_STEN_LEN)-1)<<RF_RCCAL_STEN_POS)
#define RF_RCCAL_STEN_UMSK                                      (~(((1U<<RF_RCCAL_STEN_LEN)-1)<<RF_RCCAL_STEN_POS))
#define RF_LO_LEAKCAL_STEN                                      RF_LO_LEAKCAL_STEN
#define RF_LO_LEAKCAL_STEN_POS                                  (11U)
#define RF_LO_LEAKCAL_STEN_LEN                                  (1U)
#define RF_LO_LEAKCAL_STEN_MSK                                  (((1U<<RF_LO_LEAKCAL_STEN_LEN)-1)<<RF_LO_LEAKCAL_STEN_POS)
#define RF_LO_LEAKCAL_STEN_UMSK                                 (~(((1U<<RF_LO_LEAKCAL_STEN_LEN)-1)<<RF_LO_LEAKCAL_STEN_POS))
#define RF_TIQCAL_STEN                                          RF_TIQCAL_STEN
#define RF_TIQCAL_STEN_POS                                      (12U)
#define RF_TIQCAL_STEN_LEN                                      (1U)
#define RF_TIQCAL_STEN_MSK                                      (((1U<<RF_TIQCAL_STEN_LEN)-1)<<RF_TIQCAL_STEN_POS)
#define RF_TIQCAL_STEN_UMSK                                     (~(((1U<<RF_TIQCAL_STEN_LEN)-1)<<RF_TIQCAL_STEN_POS))
#define RF_RIQCAL_STEN                                          RF_RIQCAL_STEN
#define RF_RIQCAL_STEN_POS                                      (13U)
#define RF_RIQCAL_STEN_LEN                                      (1U)
#define RF_RIQCAL_STEN_MSK                                      (((1U<<RF_RIQCAL_STEN_LEN)-1)<<RF_RIQCAL_STEN_POS)
#define RF_RIQCAL_STEN_UMSK                                     (~(((1U<<RF_RIQCAL_STEN_LEN)-1)<<RF_RIQCAL_STEN_POS))
#define RF_PWDET_CAL_STEN                                       RF_PWDET_CAL_STEN
#define RF_PWDET_CAL_STEN_POS                                   (14U)
#define RF_PWDET_CAL_STEN_LEN                                   (1U)
#define RF_PWDET_CAL_STEN_MSK                                   (((1U<<RF_PWDET_CAL_STEN_LEN)-1)<<RF_PWDET_CAL_STEN_POS)
#define RF_PWDET_CAL_STEN_UMSK                                  (~(((1U<<RF_PWDET_CAL_STEN_LEN)-1)<<RF_PWDET_CAL_STEN_POS))
#define RF_TSENCAL_STEN                                         RF_TSENCAL_STEN
#define RF_TSENCAL_STEN_POS                                     (15U)
#define RF_TSENCAL_STEN_LEN                                     (1U)
#define RF_TSENCAL_STEN_MSK                                     (((1U<<RF_TSENCAL_STEN_LEN)-1)<<RF_TSENCAL_STEN_POS)
#define RF_TSENCAL_STEN_UMSK                                    (~(((1U<<RF_TSENCAL_STEN_LEN)-1)<<RF_TSENCAL_STEN_POS))
#define RF_DPD_STEN                                             RF_DPD_STEN
#define RF_DPD_STEN_POS                                         (16U)
#define RF_DPD_STEN_LEN                                         (1U)
#define RF_DPD_STEN_MSK                                         (((1U<<RF_DPD_STEN_LEN)-1)<<RF_DPD_STEN_POS)
#define RF_DPD_STEN_UMSK                                        (~(((1U<<RF_DPD_STEN_LEN)-1)<<RF_DPD_STEN_POS))
#define RFCAL_LEVEL                                             RFCAL_LEVEL
#define RFCAL_LEVEL_POS                                         (30U)
#define RFCAL_LEVEL_LEN                                         (2U)
#define RFCAL_LEVEL_MSK                                         (((1U<<RFCAL_LEVEL_LEN)-1)<<RFCAL_LEVEL_POS)
#define RFCAL_LEVEL_UMSK                                        (~(((1U<<RFCAL_LEVEL_LEN)-1)<<RFCAL_LEVEL_POS))

/* 0x24 : SARADC Control Registers */
#define RF_SARADC_RESV_OFFSET                                   (0x24)

/* 0x28 : ZRF Control register 0 */
#define RF_BASE_CTRL1_OFFSET                                    (0x28)
#define RF_AUPLL_SDM_RST_DLY                                    RF_AUPLL_SDM_RST_DLY
#define RF_AUPLL_SDM_RST_DLY_POS                                (0U)
#define RF_AUPLL_SDM_RST_DLY_LEN                                (2U)
#define RF_AUPLL_SDM_RST_DLY_MSK                                (((1U<<RF_AUPLL_SDM_RST_DLY_LEN)-1)<<RF_AUPLL_SDM_RST_DLY_POS)
#define RF_AUPLL_SDM_RST_DLY_UMSK                               (~(((1U<<RF_AUPLL_SDM_RST_DLY_LEN)-1)<<RF_AUPLL_SDM_RST_DLY_POS))
#define RF_LO_SDM_RST_DLY                                       RF_LO_SDM_RST_DLY
#define RF_LO_SDM_RST_DLY_POS                                   (2U)
#define RF_LO_SDM_RST_DLY_LEN                                   (2U)
#define RF_LO_SDM_RST_DLY_MSK                                   (((1U<<RF_LO_SDM_RST_DLY_LEN)-1)<<RF_LO_SDM_RST_DLY_POS)
#define RF_LO_SDM_RST_DLY_UMSK                                  (~(((1U<<RF_LO_SDM_RST_DLY_LEN)-1)<<RF_LO_SDM_RST_DLY_POS))
#define RF_PPU_LEAD                                             RF_PPU_LEAD
#define RF_PPU_LEAD_POS                                         (8U)
#define RF_PPU_LEAD_LEN                                         (2U)
#define RF_PPU_LEAD_MSK                                         (((1U<<RF_PPU_LEAD_LEN)-1)<<RF_PPU_LEAD_POS)
#define RF_PPU_LEAD_UMSK                                        (~(((1U<<RF_PPU_LEAD_LEN)-1)<<RF_PPU_LEAD_POS))
#define RF_PUD_VCO_DLY                                          RF_PUD_VCO_DLY
#define RF_PUD_VCO_DLY_POS                                      (10U)
#define RF_PUD_VCO_DLY_LEN                                      (2U)
#define RF_PUD_VCO_DLY_MSK                                      (((1U<<RF_PUD_VCO_DLY_LEN)-1)<<RF_PUD_VCO_DLY_POS)
#define RF_PUD_VCO_DLY_UMSK                                     (~(((1U<<RF_PUD_VCO_DLY_LEN)-1)<<RF_PUD_VCO_DLY_POS))
#define RF_PUD_IREF_DLY                                         RF_PUD_IREF_DLY
#define RF_PUD_IREF_DLY_POS                                     (12U)
#define RF_PUD_IREF_DLY_LEN                                     (2U)
#define RF_PUD_IREF_DLY_MSK                                     (((1U<<RF_PUD_IREF_DLY_LEN)-1)<<RF_PUD_IREF_DLY_POS)
#define RF_PUD_IREF_DLY_UMSK                                    (~(((1U<<RF_PUD_IREF_DLY_LEN)-1)<<RF_PUD_IREF_DLY_POS))
#define RF_PUD_PA_DLY                                           RF_PUD_PA_DLY
#define RF_PUD_PA_DLY_POS                                       (14U)
#define RF_PUD_PA_DLY_LEN                                       (2U)
#define RF_PUD_PA_DLY_MSK                                       (((1U<<RF_PUD_PA_DLY_LEN)-1)<<RF_PUD_PA_DLY_POS)
#define RF_PUD_PA_DLY_UMSK                                      (~(((1U<<RF_PUD_PA_DLY_LEN)-1)<<RF_PUD_PA_DLY_POS))
#define RF_PUD_VBUF_FBDV_DLY                                    RF_PUD_VBUF_FBDV_DLY
#define RF_PUD_VBUF_FBDV_DLY_POS                                (16U)
#define RF_PUD_VBUF_FBDV_DLY_LEN                                (2U)
#define RF_PUD_VBUF_FBDV_DLY_MSK                                (((1U<<RF_PUD_VBUF_FBDV_DLY_LEN)-1)<<RF_PUD_VBUF_FBDV_DLY_POS)
#define RF_PUD_VBUF_FBDV_DLY_UMSK                               (~(((1U<<RF_PUD_VBUF_FBDV_DLY_LEN)-1)<<RF_PUD_VBUF_FBDV_DLY_POS))
#define RF_PUD_VBUF_LODIST_DLY                                  RF_PUD_VBUF_LODIST_DLY
#define RF_PUD_VBUF_LODIST_DLY_POS                              (18U)
#define RF_PUD_VBUF_LODIST_DLY_LEN                              (2U)
#define RF_PUD_VBUF_LODIST_DLY_MSK                              (((1U<<RF_PUD_VBUF_LODIST_DLY_LEN)-1)<<RF_PUD_VBUF_LODIST_DLY_POS)
#define RF_PUD_VBUF_LODIST_DLY_UMSK                             (~(((1U<<RF_PUD_VBUF_LODIST_DLY_LEN)-1)<<RF_PUD_VBUF_LODIST_DLY_POS))
#define RF_MBG_TRIM                                             RF_MBG_TRIM
#define RF_MBG_TRIM_POS                                         (27U)
#define RF_MBG_TRIM_LEN                                         (2U)
#define RF_MBG_TRIM_MSK                                         (((1U<<RF_MBG_TRIM_LEN)-1)<<RF_MBG_TRIM_POS)
#define RF_MBG_TRIM_UMSK                                        (~(((1U<<RF_MBG_TRIM_LEN)-1)<<RF_MBG_TRIM_POS))

/* 0x2C : ZRF Control register 0 */
#define RF_BASE_CTRL2_OFFSET                                    (0x2C)
#define RF_PU_CTRL_HW                                           RF_PU_CTRL_HW
#define RF_PU_CTRL_HW_POS                                       (0U)
#define RF_PU_CTRL_HW_LEN                                       (1U)
#define RF_PU_CTRL_HW_MSK                                       (((1U<<RF_PU_CTRL_HW_LEN)-1)<<RF_PU_CTRL_HW_POS)
#define RF_PU_CTRL_HW_UMSK                                      (~(((1U<<RF_PU_CTRL_HW_LEN)-1)<<RF_PU_CTRL_HW_POS))
#define RF_RX_GAIN_CTRL_HW                                      RF_RX_GAIN_CTRL_HW
#define RF_RX_GAIN_CTRL_HW_POS                                  (1U)
#define RF_RX_GAIN_CTRL_HW_LEN                                  (1U)
#define RF_RX_GAIN_CTRL_HW_MSK                                  (((1U<<RF_RX_GAIN_CTRL_HW_LEN)-1)<<RF_RX_GAIN_CTRL_HW_POS)
#define RF_RX_GAIN_CTRL_HW_UMSK                                 (~(((1U<<RF_RX_GAIN_CTRL_HW_LEN)-1)<<RF_RX_GAIN_CTRL_HW_POS))
#define RF_TX_GAIN_CTRL_HW                                      RF_TX_GAIN_CTRL_HW
#define RF_TX_GAIN_CTRL_HW_POS                                  (2U)
#define RF_TX_GAIN_CTRL_HW_LEN                                  (1U)
#define RF_TX_GAIN_CTRL_HW_MSK                                  (((1U<<RF_TX_GAIN_CTRL_HW_LEN)-1)<<RF_TX_GAIN_CTRL_HW_POS)
#define RF_TX_GAIN_CTRL_HW_UMSK                                 (~(((1U<<RF_TX_GAIN_CTRL_HW_LEN)-1)<<RF_TX_GAIN_CTRL_HW_POS))
#define RF_LNA_CTRL_HW                                          RF_LNA_CTRL_HW
#define RF_LNA_CTRL_HW_POS                                      (3U)
#define RF_LNA_CTRL_HW_LEN                                      (1U)
#define RF_LNA_CTRL_HW_MSK                                      (((1U<<RF_LNA_CTRL_HW_LEN)-1)<<RF_LNA_CTRL_HW_POS)
#define RF_LNA_CTRL_HW_UMSK                                     (~(((1U<<RF_LNA_CTRL_HW_LEN)-1)<<RF_LNA_CTRL_HW_POS))
#define RF_RBB_BW_CTRL_HW                                       RF_RBB_BW_CTRL_HW
#define RF_RBB_BW_CTRL_HW_POS                                   (4U)
#define RF_RBB_BW_CTRL_HW_LEN                                   (1U)
#define RF_RBB_BW_CTRL_HW_MSK                                   (((1U<<RF_RBB_BW_CTRL_HW_LEN)-1)<<RF_RBB_BW_CTRL_HW_POS)
#define RF_RBB_BW_CTRL_HW_UMSK                                  (~(((1U<<RF_RBB_BW_CTRL_HW_LEN)-1)<<RF_RBB_BW_CTRL_HW_POS))
#define RF_RXCAL_CTRL_HW                                        RF_RXCAL_CTRL_HW
#define RF_RXCAL_CTRL_HW_POS                                    (5U)
#define RF_RXCAL_CTRL_HW_LEN                                    (1U)
#define RF_RXCAL_CTRL_HW_MSK                                    (((1U<<RF_RXCAL_CTRL_HW_LEN)-1)<<RF_RXCAL_CTRL_HW_POS)
#define RF_RXCAL_CTRL_HW_UMSK                                   (~(((1U<<RF_RXCAL_CTRL_HW_LEN)-1)<<RF_RXCAL_CTRL_HW_POS))
#define RF_LO_CTRL_HW                                           RF_LO_CTRL_HW
#define RF_LO_CTRL_HW_POS                                       (6U)
#define RF_LO_CTRL_HW_LEN                                       (1U)
#define RF_LO_CTRL_HW_MSK                                       (((1U<<RF_LO_CTRL_HW_LEN)-1)<<RF_LO_CTRL_HW_POS)
#define RF_LO_CTRL_HW_UMSK                                      (~(((1U<<RF_LO_CTRL_HW_LEN)-1)<<RF_LO_CTRL_HW_POS))
#define RF_INC_ACAL_CTRL_EN_HW                                  RF_INC_ACAL_CTRL_EN_HW
#define RF_INC_ACAL_CTRL_EN_HW_POS                              (7U)
#define RF_INC_ACAL_CTRL_EN_HW_LEN                              (1U)
#define RF_INC_ACAL_CTRL_EN_HW_MSK                              (((1U<<RF_INC_ACAL_CTRL_EN_HW_LEN)-1)<<RF_INC_ACAL_CTRL_EN_HW_POS)
#define RF_INC_ACAL_CTRL_EN_HW_UMSK                             (~(((1U<<RF_INC_ACAL_CTRL_EN_HW_LEN)-1)<<RF_INC_ACAL_CTRL_EN_HW_POS))
#define RF_INC_FCAL_CTRL_EN_HW                                  RF_INC_FCAL_CTRL_EN_HW
#define RF_INC_FCAL_CTRL_EN_HW_POS                              (8U)
#define RF_INC_FCAL_CTRL_EN_HW_LEN                              (1U)
#define RF_INC_FCAL_CTRL_EN_HW_MSK                              (((1U<<RF_INC_FCAL_CTRL_EN_HW_LEN)-1)<<RF_INC_FCAL_CTRL_EN_HW_POS)
#define RF_INC_FCAL_CTRL_EN_HW_UMSK                             (~(((1U<<RF_INC_FCAL_CTRL_EN_HW_LEN)-1)<<RF_INC_FCAL_CTRL_EN_HW_POS))
#define RF_SDM_CTRL_HW                                          RF_SDM_CTRL_HW
#define RF_SDM_CTRL_HW_POS                                      (9U)
#define RF_SDM_CTRL_HW_LEN                                      (1U)
#define RF_SDM_CTRL_HW_MSK                                      (((1U<<RF_SDM_CTRL_HW_LEN)-1)<<RF_SDM_CTRL_HW_POS)
#define RF_SDM_CTRL_HW_UMSK                                     (~(((1U<<RF_SDM_CTRL_HW_LEN)-1)<<RF_SDM_CTRL_HW_POS))
#define RF_RBB_PKDET_EN_CTRL_HW                                 RF_RBB_PKDET_EN_CTRL_HW
#define RF_RBB_PKDET_EN_CTRL_HW_POS                             (10U)
#define RF_RBB_PKDET_EN_CTRL_HW_LEN                             (1U)
#define RF_RBB_PKDET_EN_CTRL_HW_MSK                             (((1U<<RF_RBB_PKDET_EN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_EN_CTRL_HW_POS)
#define RF_RBB_PKDET_EN_CTRL_HW_UMSK                            (~(((1U<<RF_RBB_PKDET_EN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_EN_CTRL_HW_POS))
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW                           RF_RBB_PKDET_OUT_RSTN_CTRL_HW
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS                       (11U)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN                       (1U)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_MSK                       (((1U<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_UMSK                      (~(((1U<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS))
#define RF_ADDA_CTRL_HW                                         RF_ADDA_CTRL_HW
#define RF_ADDA_CTRL_HW_POS                                     (12U)
#define RF_ADDA_CTRL_HW_LEN                                     (1U)
#define RF_ADDA_CTRL_HW_MSK                                     (((1U<<RF_ADDA_CTRL_HW_LEN)-1)<<RF_ADDA_CTRL_HW_POS)
#define RF_ADDA_CTRL_HW_UMSK                                    (~(((1U<<RF_ADDA_CTRL_HW_LEN)-1)<<RF_ADDA_CTRL_HW_POS))
#define RF_TXCAL_CTRL_HW                                        RF_TXCAL_CTRL_HW
#define RF_TXCAL_CTRL_HW_POS                                    (14U)
#define RF_TXCAL_CTRL_HW_LEN                                    (1U)
#define RF_TXCAL_CTRL_HW_MSK                                    (((1U<<RF_TXCAL_CTRL_HW_LEN)-1)<<RF_TXCAL_CTRL_HW_POS)
#define RF_TXCAL_CTRL_HW_UMSK                                   (~(((1U<<RF_TXCAL_CTRL_HW_LEN)-1)<<RF_TXCAL_CTRL_HW_POS))

/* 0x30 : pucr1 */
#define RF_PUCR1_OFFSET                                         (0x30)
#define RF_PU_SFREG                                             RF_PU_SFREG
#define RF_PU_SFREG_POS                                         (0U)
#define RF_PU_SFREG_LEN                                         (1U)
#define RF_PU_SFREG_MSK                                         (((1U<<RF_PU_SFREG_LEN)-1)<<RF_PU_SFREG_POS)
#define RF_PU_SFREG_UMSK                                        (~(((1U<<RF_PU_SFREG_LEN)-1)<<RF_PU_SFREG_POS))
#define RF_PU_RCAL                                              RF_PU_RCAL
#define RF_PU_RCAL_POS                                          (1U)
#define RF_PU_RCAL_LEN                                          (1U)
#define RF_PU_RCAL_MSK                                          (((1U<<RF_PU_RCAL_LEN)-1)<<RF_PU_RCAL_POS)
#define RF_PU_RCAL_UMSK                                         (~(((1U<<RF_PU_RCAL_LEN)-1)<<RF_PU_RCAL_POS))
#define RF_PU_LNA                                               RF_PU_LNA
#define RF_PU_LNA_POS                                           (8U)
#define RF_PU_LNA_LEN                                           (1U)
#define RF_PU_LNA_MSK                                           (((1U<<RF_PU_LNA_LEN)-1)<<RF_PU_LNA_POS)
#define RF_PU_LNA_UMSK                                          (~(((1U<<RF_PU_LNA_LEN)-1)<<RF_PU_LNA_POS))
#define RF_PU_RMXGM                                             RF_PU_RMXGM
#define RF_PU_RMXGM_POS                                         (9U)
#define RF_PU_RMXGM_LEN                                         (1U)
#define RF_PU_RMXGM_MSK                                         (((1U<<RF_PU_RMXGM_LEN)-1)<<RF_PU_RMXGM_POS)
#define RF_PU_RMXGM_UMSK                                        (~(((1U<<RF_PU_RMXGM_LEN)-1)<<RF_PU_RMXGM_POS))
#define RF_PU_RMX                                               RF_PU_RMX
#define RF_PU_RMX_POS                                           (10U)
#define RF_PU_RMX_LEN                                           (1U)
#define RF_PU_RMX_MSK                                           (((1U<<RF_PU_RMX_LEN)-1)<<RF_PU_RMX_POS)
#define RF_PU_RMX_UMSK                                          (~(((1U<<RF_PU_RMX_LEN)-1)<<RF_PU_RMX_POS))
#define RF_PU_RBB                                               RF_PU_RBB
#define RF_PU_RBB_POS                                           (11U)
#define RF_PU_RBB_LEN                                           (1U)
#define RF_PU_RBB_MSK                                           (((1U<<RF_PU_RBB_LEN)-1)<<RF_PU_RBB_POS)
#define RF_PU_RBB_UMSK                                          (~(((1U<<RF_PU_RBB_LEN)-1)<<RF_PU_RBB_POS))
#define RF_PU_ADDA_LDO                                          RF_PU_ADDA_LDO
#define RF_PU_ADDA_LDO_POS                                      (12U)
#define RF_PU_ADDA_LDO_LEN                                      (1U)
#define RF_PU_ADDA_LDO_MSK                                      (((1U<<RF_PU_ADDA_LDO_LEN)-1)<<RF_PU_ADDA_LDO_POS)
#define RF_PU_ADDA_LDO_UMSK                                     (~(((1U<<RF_PU_ADDA_LDO_LEN)-1)<<RF_PU_ADDA_LDO_POS))
#define RF_ADC_CLK_EN                                           RF_ADC_CLK_EN
#define RF_ADC_CLK_EN_POS                                       (13U)
#define RF_ADC_CLK_EN_LEN                                       (1U)
#define RF_ADC_CLK_EN_MSK                                       (((1U<<RF_ADC_CLK_EN_LEN)-1)<<RF_ADC_CLK_EN_POS)
#define RF_ADC_CLK_EN_UMSK                                      (~(((1U<<RF_ADC_CLK_EN_LEN)-1)<<RF_ADC_CLK_EN_POS))
#define RF_PU_ADC                                               RF_PU_ADC
#define RF_PU_ADC_POS                                           (14U)
#define RF_PU_ADC_LEN                                           (1U)
#define RF_PU_ADC_MSK                                           (((1U<<RF_PU_ADC_LEN)-1)<<RF_PU_ADC_POS)
#define RF_PU_ADC_UMSK                                          (~(((1U<<RF_PU_ADC_LEN)-1)<<RF_PU_ADC_POS))
#define RF_PU_OP_ATEST                                          RF_PU_OP_ATEST
#define RF_PU_OP_ATEST_POS                                      (15U)
#define RF_PU_OP_ATEST_LEN                                      (1U)
#define RF_PU_OP_ATEST_MSK                                      (((1U<<RF_PU_OP_ATEST_LEN)-1)<<RF_PU_OP_ATEST_POS)
#define RF_PU_OP_ATEST_UMSK                                     (~(((1U<<RF_PU_OP_ATEST_LEN)-1)<<RF_PU_OP_ATEST_POS))
#define RF_PU_PA                                                RF_PU_PA
#define RF_PU_PA_POS                                            (16U)
#define RF_PU_PA_LEN                                            (1U)
#define RF_PU_PA_MSK                                            (((1U<<RF_PU_PA_LEN)-1)<<RF_PU_PA_POS)
#define RF_PU_PA_UMSK                                           (~(((1U<<RF_PU_PA_LEN)-1)<<RF_PU_PA_POS))
#define RF_PU_TMX                                               RF_PU_TMX
#define RF_PU_TMX_POS                                           (17U)
#define RF_PU_TMX_LEN                                           (1U)
#define RF_PU_TMX_MSK                                           (((1U<<RF_PU_TMX_LEN)-1)<<RF_PU_TMX_POS)
#define RF_PU_TMX_UMSK                                          (~(((1U<<RF_PU_TMX_LEN)-1)<<RF_PU_TMX_POS))
#define RF_PU_TBB                                               RF_PU_TBB
#define RF_PU_TBB_POS                                           (18U)
#define RF_PU_TBB_LEN                                           (1U)
#define RF_PU_TBB_MSK                                           (((1U<<RF_PU_TBB_LEN)-1)<<RF_PU_TBB_POS)
#define RF_PU_TBB_UMSK                                          (~(((1U<<RF_PU_TBB_LEN)-1)<<RF_PU_TBB_POS))
#define RF_PU_DAC                                               RF_PU_DAC
#define RF_PU_DAC_POS                                           (19U)
#define RF_PU_DAC_LEN                                           (1U)
#define RF_PU_DAC_MSK                                           (((1U<<RF_PU_DAC_LEN)-1)<<RF_PU_DAC_POS)
#define RF_PU_DAC_UMSK                                          (~(((1U<<RF_PU_DAC_LEN)-1)<<RF_PU_DAC_POS))
#define RF_PU_VCO                                               RF_PU_VCO
#define RF_PU_VCO_POS                                           (20U)
#define RF_PU_VCO_LEN                                           (1U)
#define RF_PU_VCO_MSK                                           (((1U<<RF_PU_VCO_LEN)-1)<<RF_PU_VCO_POS)
#define RF_PU_VCO_UMSK                                          (~(((1U<<RF_PU_VCO_LEN)-1)<<RF_PU_VCO_POS))
#define RF_PU_FBDV                                              RF_PU_FBDV
#define RF_PU_FBDV_POS                                          (21U)
#define RF_PU_FBDV_LEN                                          (1U)
#define RF_PU_FBDV_MSK                                          (((1U<<RF_PU_FBDV_LEN)-1)<<RF_PU_FBDV_POS)
#define RF_PU_FBDV_UMSK                                         (~(((1U<<RF_PU_FBDV_LEN)-1)<<RF_PU_FBDV_POS))
#define RF_PU_PFD                                               RF_PU_PFD
#define RF_PU_PFD_POS                                           (22U)
#define RF_PU_PFD_LEN                                           (1U)
#define RF_PU_PFD_MSK                                           (((1U<<RF_PU_PFD_LEN)-1)<<RF_PU_PFD_POS)
#define RF_PU_PFD_UMSK                                          (~(((1U<<RF_PU_PFD_LEN)-1)<<RF_PU_PFD_POS))
#define RF_PU_OSMX                                              RF_PU_OSMX
#define RF_PU_OSMX_POS                                          (23U)
#define RF_PU_OSMX_LEN                                          (1U)
#define RF_PU_OSMX_MSK                                          (((1U<<RF_PU_OSMX_LEN)-1)<<RF_PU_OSMX_POS)
#define RF_PU_OSMX_UMSK                                         (~(((1U<<RF_PU_OSMX_LEN)-1)<<RF_PU_OSMX_POS))
#define RF_PU_RXBUF                                             RF_PU_RXBUF
#define RF_PU_RXBUF_POS                                         (24U)
#define RF_PU_RXBUF_LEN                                         (1U)
#define RF_PU_RXBUF_MSK                                         (((1U<<RF_PU_RXBUF_LEN)-1)<<RF_PU_RXBUF_POS)
#define RF_PU_RXBUF_UMSK                                        (~(((1U<<RF_PU_RXBUF_LEN)-1)<<RF_PU_RXBUF_POS))
#define RF_PU_TXBUF                                             RF_PU_TXBUF
#define RF_PU_TXBUF_POS                                         (25U)
#define RF_PU_TXBUF_LEN                                         (1U)
#define RF_PU_TXBUF_MSK                                         (((1U<<RF_PU_TXBUF_LEN)-1)<<RF_PU_TXBUF_POS)
#define RF_PU_TXBUF_UMSK                                        (~(((1U<<RF_PU_TXBUF_LEN)-1)<<RF_PU_TXBUF_POS))
#define RF_TRSW_EN                                              RF_TRSW_EN
#define RF_TRSW_EN_POS                                          (26U)
#define RF_TRSW_EN_LEN                                          (2U)
#define RF_TRSW_EN_MSK                                          (((1U<<RF_TRSW_EN_LEN)-1)<<RF_TRSW_EN_POS)
#define RF_TRSW_EN_UMSK                                         (~(((1U<<RF_TRSW_EN_LEN)-1)<<RF_TRSW_EN_POS))
#define RF_PU_PKDET                                             RF_PU_PKDET
#define RF_PU_PKDET_POS                                         (28U)
#define RF_PU_PKDET_LEN                                         (1U)
#define RF_PU_PKDET_MSK                                         (((1U<<RF_PU_PKDET_LEN)-1)<<RF_PU_PKDET_POS)
#define RF_PU_PKDET_UMSK                                        (~(((1U<<RF_PU_PKDET_LEN)-1)<<RF_PU_PKDET_POS))
#define RF_PU_ROSDAC                                            RF_PU_ROSDAC
#define RF_PU_ROSDAC_POS                                        (29U)
#define RF_PU_ROSDAC_LEN                                        (1U)
#define RF_PU_ROSDAC_MSK                                        (((1U<<RF_PU_ROSDAC_LEN)-1)<<RF_PU_ROSDAC_POS)
#define RF_PU_ROSDAC_UMSK                                       (~(((1U<<RF_PU_ROSDAC_LEN)-1)<<RF_PU_ROSDAC_POS))
#define RF_PU_PWRMX                                             RF_PU_PWRMX
#define RF_PU_PWRMX_POS                                         (30U)
#define RF_PU_PWRMX_LEN                                         (1U)
#define RF_PU_PWRMX_MSK                                         (((1U<<RF_PU_PWRMX_LEN)-1)<<RF_PU_PWRMX_POS)
#define RF_PU_PWRMX_UMSK                                        (~(((1U<<RF_PU_PWRMX_LEN)-1)<<RF_PU_PWRMX_POS))
#define RF_PU_TOSDAC                                            RF_PU_TOSDAC
#define RF_PU_TOSDAC_POS                                        (31U)
#define RF_PU_TOSDAC_LEN                                        (1U)
#define RF_PU_TOSDAC_MSK                                        (((1U<<RF_PU_TOSDAC_LEN)-1)<<RF_PU_TOSDAC_POS)
#define RF_PU_TOSDAC_UMSK                                       (~(((1U<<RF_PU_TOSDAC_LEN)-1)<<RF_PU_TOSDAC_POS))

/* 0x34 : read only from hardware logic */
#define RF_PUCR1_HW_OFFSET                                      (0x34)
#define RF_PU_SFREG_HW                                          RF_PU_SFREG_HW
#define RF_PU_SFREG_HW_POS                                      (0U)
#define RF_PU_SFREG_HW_LEN                                      (1U)
#define RF_PU_SFREG_HW_MSK                                      (((1U<<RF_PU_SFREG_HW_LEN)-1)<<RF_PU_SFREG_HW_POS)
#define RF_PU_SFREG_HW_UMSK                                     (~(((1U<<RF_PU_SFREG_HW_LEN)-1)<<RF_PU_SFREG_HW_POS))
#define RF_PU_RCAL_HW                                           RF_PU_RCAL_HW
#define RF_PU_RCAL_HW_POS                                       (1U)
#define RF_PU_RCAL_HW_LEN                                       (1U)
#define RF_PU_RCAL_HW_MSK                                       (((1U<<RF_PU_RCAL_HW_LEN)-1)<<RF_PU_RCAL_HW_POS)
#define RF_PU_RCAL_HW_UMSK                                      (~(((1U<<RF_PU_RCAL_HW_LEN)-1)<<RF_PU_RCAL_HW_POS))
#define RF_PU_LNA_HW                                            RF_PU_LNA_HW
#define RF_PU_LNA_HW_POS                                        (8U)
#define RF_PU_LNA_HW_LEN                                        (1U)
#define RF_PU_LNA_HW_MSK                                        (((1U<<RF_PU_LNA_HW_LEN)-1)<<RF_PU_LNA_HW_POS)
#define RF_PU_LNA_HW_UMSK                                       (~(((1U<<RF_PU_LNA_HW_LEN)-1)<<RF_PU_LNA_HW_POS))
#define RF_PU_RMXGM_HW                                          RF_PU_RMXGM_HW
#define RF_PU_RMXGM_HW_POS                                      (9U)
#define RF_PU_RMXGM_HW_LEN                                      (1U)
#define RF_PU_RMXGM_HW_MSK                                      (((1U<<RF_PU_RMXGM_HW_LEN)-1)<<RF_PU_RMXGM_HW_POS)
#define RF_PU_RMXGM_HW_UMSK                                     (~(((1U<<RF_PU_RMXGM_HW_LEN)-1)<<RF_PU_RMXGM_HW_POS))
#define RF_PU_RMX_HW                                            RF_PU_RMX_HW
#define RF_PU_RMX_HW_POS                                        (10U)
#define RF_PU_RMX_HW_LEN                                        (1U)
#define RF_PU_RMX_HW_MSK                                        (((1U<<RF_PU_RMX_HW_LEN)-1)<<RF_PU_RMX_HW_POS)
#define RF_PU_RMX_HW_UMSK                                       (~(((1U<<RF_PU_RMX_HW_LEN)-1)<<RF_PU_RMX_HW_POS))
#define RF_PU_RBB_HW                                            RF_PU_RBB_HW
#define RF_PU_RBB_HW_POS                                        (11U)
#define RF_PU_RBB_HW_LEN                                        (1U)
#define RF_PU_RBB_HW_MSK                                        (((1U<<RF_PU_RBB_HW_LEN)-1)<<RF_PU_RBB_HW_POS)
#define RF_PU_RBB_HW_UMSK                                       (~(((1U<<RF_PU_RBB_HW_LEN)-1)<<RF_PU_RBB_HW_POS))
#define RF_PU_ADDA_LDO_HW                                       RF_PU_ADDA_LDO_HW
#define RF_PU_ADDA_LDO_HW_POS                                   (12U)
#define RF_PU_ADDA_LDO_HW_LEN                                   (1U)
#define RF_PU_ADDA_LDO_HW_MSK                                   (((1U<<RF_PU_ADDA_LDO_HW_LEN)-1)<<RF_PU_ADDA_LDO_HW_POS)
#define RF_PU_ADDA_LDO_HW_UMSK                                  (~(((1U<<RF_PU_ADDA_LDO_HW_LEN)-1)<<RF_PU_ADDA_LDO_HW_POS))
#define RF_ADC_CLK_EN_HW                                        RF_ADC_CLK_EN_HW
#define RF_ADC_CLK_EN_HW_POS                                    (13U)
#define RF_ADC_CLK_EN_HW_LEN                                    (1U)
#define RF_ADC_CLK_EN_HW_MSK                                    (((1U<<RF_ADC_CLK_EN_HW_LEN)-1)<<RF_ADC_CLK_EN_HW_POS)
#define RF_ADC_CLK_EN_HW_UMSK                                   (~(((1U<<RF_ADC_CLK_EN_HW_LEN)-1)<<RF_ADC_CLK_EN_HW_POS))
#define RF_PU_ADC_HW                                            RF_PU_ADC_HW
#define RF_PU_ADC_HW_POS                                        (14U)
#define RF_PU_ADC_HW_LEN                                        (1U)
#define RF_PU_ADC_HW_MSK                                        (((1U<<RF_PU_ADC_HW_LEN)-1)<<RF_PU_ADC_HW_POS)
#define RF_PU_ADC_HW_UMSK                                       (~(((1U<<RF_PU_ADC_HW_LEN)-1)<<RF_PU_ADC_HW_POS))
#define RF_PU_PA_HW                                             RF_PU_PA_HW
#define RF_PU_PA_HW_POS                                         (16U)
#define RF_PU_PA_HW_LEN                                         (1U)
#define RF_PU_PA_HW_MSK                                         (((1U<<RF_PU_PA_HW_LEN)-1)<<RF_PU_PA_HW_POS)
#define RF_PU_PA_HW_UMSK                                        (~(((1U<<RF_PU_PA_HW_LEN)-1)<<RF_PU_PA_HW_POS))
#define RF_PU_TMX_HW                                            RF_PU_TMX_HW
#define RF_PU_TMX_HW_POS                                        (17U)
#define RF_PU_TMX_HW_LEN                                        (1U)
#define RF_PU_TMX_HW_MSK                                        (((1U<<RF_PU_TMX_HW_LEN)-1)<<RF_PU_TMX_HW_POS)
#define RF_PU_TMX_HW_UMSK                                       (~(((1U<<RF_PU_TMX_HW_LEN)-1)<<RF_PU_TMX_HW_POS))
#define RF_PU_TBB_HW                                            RF_PU_TBB_HW
#define RF_PU_TBB_HW_POS                                        (18U)
#define RF_PU_TBB_HW_LEN                                        (1U)
#define RF_PU_TBB_HW_MSK                                        (((1U<<RF_PU_TBB_HW_LEN)-1)<<RF_PU_TBB_HW_POS)
#define RF_PU_TBB_HW_UMSK                                       (~(((1U<<RF_PU_TBB_HW_LEN)-1)<<RF_PU_TBB_HW_POS))
#define RF_PU_DAC_HW                                            RF_PU_DAC_HW
#define RF_PU_DAC_HW_POS                                        (19U)
#define RF_PU_DAC_HW_LEN                                        (1U)
#define RF_PU_DAC_HW_MSK                                        (((1U<<RF_PU_DAC_HW_LEN)-1)<<RF_PU_DAC_HW_POS)
#define RF_PU_DAC_HW_UMSK                                       (~(((1U<<RF_PU_DAC_HW_LEN)-1)<<RF_PU_DAC_HW_POS))
#define RF_PU_VCO_HW                                            RF_PU_VCO_HW
#define RF_PU_VCO_HW_POS                                        (20U)
#define RF_PU_VCO_HW_LEN                                        (1U)
#define RF_PU_VCO_HW_MSK                                        (((1U<<RF_PU_VCO_HW_LEN)-1)<<RF_PU_VCO_HW_POS)
#define RF_PU_VCO_HW_UMSK                                       (~(((1U<<RF_PU_VCO_HW_LEN)-1)<<RF_PU_VCO_HW_POS))
#define RF_PU_FBDV_HW                                           RF_PU_FBDV_HW
#define RF_PU_FBDV_HW_POS                                       (21U)
#define RF_PU_FBDV_HW_LEN                                       (1U)
#define RF_PU_FBDV_HW_MSK                                       (((1U<<RF_PU_FBDV_HW_LEN)-1)<<RF_PU_FBDV_HW_POS)
#define RF_PU_FBDV_HW_UMSK                                      (~(((1U<<RF_PU_FBDV_HW_LEN)-1)<<RF_PU_FBDV_HW_POS))
#define RF_PU_PFD_HW                                            RF_PU_PFD_HW
#define RF_PU_PFD_HW_POS                                        (22U)
#define RF_PU_PFD_HW_LEN                                        (1U)
#define RF_PU_PFD_HW_MSK                                        (((1U<<RF_PU_PFD_HW_LEN)-1)<<RF_PU_PFD_HW_POS)
#define RF_PU_PFD_HW_UMSK                                       (~(((1U<<RF_PU_PFD_HW_LEN)-1)<<RF_PU_PFD_HW_POS))
#define RF_PU_OSMX_HW                                           RF_PU_OSMX_HW
#define RF_PU_OSMX_HW_POS                                       (23U)
#define RF_PU_OSMX_HW_LEN                                       (1U)
#define RF_PU_OSMX_HW_MSK                                       (((1U<<RF_PU_OSMX_HW_LEN)-1)<<RF_PU_OSMX_HW_POS)
#define RF_PU_OSMX_HW_UMSK                                      (~(((1U<<RF_PU_OSMX_HW_LEN)-1)<<RF_PU_OSMX_HW_POS))
#define RF_PU_RXBUF_HW                                          RF_PU_RXBUF_HW
#define RF_PU_RXBUF_HW_POS                                      (24U)
#define RF_PU_RXBUF_HW_LEN                                      (1U)
#define RF_PU_RXBUF_HW_MSK                                      (((1U<<RF_PU_RXBUF_HW_LEN)-1)<<RF_PU_RXBUF_HW_POS)
#define RF_PU_RXBUF_HW_UMSK                                     (~(((1U<<RF_PU_RXBUF_HW_LEN)-1)<<RF_PU_RXBUF_HW_POS))
#define RF_PU_TXBUF_HW                                          RF_PU_TXBUF_HW
#define RF_PU_TXBUF_HW_POS                                      (25U)
#define RF_PU_TXBUF_HW_LEN                                      (1U)
#define RF_PU_TXBUF_HW_MSK                                      (((1U<<RF_PU_TXBUF_HW_LEN)-1)<<RF_PU_TXBUF_HW_POS)
#define RF_PU_TXBUF_HW_UMSK                                     (~(((1U<<RF_PU_TXBUF_HW_LEN)-1)<<RF_PU_TXBUF_HW_POS))
#define RF_TRSW_EN_HW                                           RF_TRSW_EN_HW
#define RF_TRSW_EN_HW_POS                                       (26U)
#define RF_TRSW_EN_HW_LEN                                       (2U)
#define RF_TRSW_EN_HW_MSK                                       (((1U<<RF_TRSW_EN_HW_LEN)-1)<<RF_TRSW_EN_HW_POS)
#define RF_TRSW_EN_HW_UMSK                                      (~(((1U<<RF_TRSW_EN_HW_LEN)-1)<<RF_TRSW_EN_HW_POS))
#define RF_PU_PKDET_HW                                          RF_PU_PKDET_HW
#define RF_PU_PKDET_HW_POS                                      (28U)
#define RF_PU_PKDET_HW_LEN                                      (1U)
#define RF_PU_PKDET_HW_MSK                                      (((1U<<RF_PU_PKDET_HW_LEN)-1)<<RF_PU_PKDET_HW_POS)
#define RF_PU_PKDET_HW_UMSK                                     (~(((1U<<RF_PU_PKDET_HW_LEN)-1)<<RF_PU_PKDET_HW_POS))
#define RF_PU_ROSDAC_HW                                         RF_PU_ROSDAC_HW
#define RF_PU_ROSDAC_HW_POS                                     (29U)
#define RF_PU_ROSDAC_HW_LEN                                     (1U)
#define RF_PU_ROSDAC_HW_MSK                                     (((1U<<RF_PU_ROSDAC_HW_LEN)-1)<<RF_PU_ROSDAC_HW_POS)
#define RF_PU_ROSDAC_HW_UMSK                                    (~(((1U<<RF_PU_ROSDAC_HW_LEN)-1)<<RF_PU_ROSDAC_HW_POS))
#define RF_PU_TOSDAC_HW                                         RF_PU_TOSDAC_HW
#define RF_PU_TOSDAC_HW_POS                                     (31U)
#define RF_PU_TOSDAC_HW_LEN                                     (1U)
#define RF_PU_TOSDAC_HW_MSK                                     (((1U<<RF_PU_TOSDAC_HW_LEN)-1)<<RF_PU_TOSDAC_HW_POS)
#define RF_PU_TOSDAC_HW_UMSK                                    (~(((1U<<RF_PU_TOSDAC_HW_LEN)-1)<<RF_PU_TOSDAC_HW_POS))

/* 0x38 : pucr2 */
#define RF_PUCR2_OFFSET                                         (0x38)

/* 0x3C : pucr2_hw */
#define RF_PUCR2_HW_OFFSET                                      (0x3C)

/* 0x40 : ppu_ctrl_hw */
#define RF_PPU_CTRL_HW_OFFSET                                   (0x40)
#define RF_PPU_LNA_HW                                           RF_PPU_LNA_HW
#define RF_PPU_LNA_HW_POS                                       (8U)
#define RF_PPU_LNA_HW_LEN                                       (1U)
#define RF_PPU_LNA_HW_MSK                                       (((1U<<RF_PPU_LNA_HW_LEN)-1)<<RF_PPU_LNA_HW_POS)
#define RF_PPU_LNA_HW_UMSK                                      (~(((1U<<RF_PPU_LNA_HW_LEN)-1)<<RF_PPU_LNA_HW_POS))
#define RF_PPU_RMXGM_HW                                         RF_PPU_RMXGM_HW
#define RF_PPU_RMXGM_HW_POS                                     (9U)
#define RF_PPU_RMXGM_HW_LEN                                     (1U)
#define RF_PPU_RMXGM_HW_MSK                                     (((1U<<RF_PPU_RMXGM_HW_LEN)-1)<<RF_PPU_RMXGM_HW_POS)
#define RF_PPU_RMXGM_HW_UMSK                                    (~(((1U<<RF_PPU_RMXGM_HW_LEN)-1)<<RF_PPU_RMXGM_HW_POS))
#define RF_PPU_RBB_HW                                           RF_PPU_RBB_HW
#define RF_PPU_RBB_HW_POS                                       (11U)
#define RF_PPU_RBB_HW_LEN                                       (1U)
#define RF_PPU_RBB_HW_MSK                                       (((1U<<RF_PPU_RBB_HW_LEN)-1)<<RF_PPU_RBB_HW_POS)
#define RF_PPU_RBB_HW_UMSK                                      (~(((1U<<RF_PPU_RBB_HW_LEN)-1)<<RF_PPU_RBB_HW_POS))
#define RF_PPU_VCO_HW                                           RF_PPU_VCO_HW
#define RF_PPU_VCO_HW_POS                                       (20U)
#define RF_PPU_VCO_HW_LEN                                       (1U)
#define RF_PPU_VCO_HW_MSK                                       (((1U<<RF_PPU_VCO_HW_LEN)-1)<<RF_PPU_VCO_HW_POS)
#define RF_PPU_VCO_HW_UMSK                                      (~(((1U<<RF_PPU_VCO_HW_LEN)-1)<<RF_PPU_VCO_HW_POS))
#define RF_PPU_FBDV_HW                                          RF_PPU_FBDV_HW
#define RF_PPU_FBDV_HW_POS                                      (21U)
#define RF_PPU_FBDV_HW_LEN                                      (1U)
#define RF_PPU_FBDV_HW_MSK                                      (((1U<<RF_PPU_FBDV_HW_LEN)-1)<<RF_PPU_FBDV_HW_POS)
#define RF_PPU_FBDV_HW_UMSK                                     (~(((1U<<RF_PPU_FBDV_HW_LEN)-1)<<RF_PPU_FBDV_HW_POS))
#define RF_PPU_PFD_HW                                           RF_PPU_PFD_HW
#define RF_PPU_PFD_HW_POS                                       (22U)
#define RF_PPU_PFD_HW_LEN                                       (1U)
#define RF_PPU_PFD_HW_MSK                                       (((1U<<RF_PPU_PFD_HW_LEN)-1)<<RF_PPU_PFD_HW_POS)
#define RF_PPU_PFD_HW_UMSK                                      (~(((1U<<RF_PPU_PFD_HW_LEN)-1)<<RF_PPU_PFD_HW_POS))
#define RF_PPU_OSMX_HW                                          RF_PPU_OSMX_HW
#define RF_PPU_OSMX_HW_POS                                      (23U)
#define RF_PPU_OSMX_HW_LEN                                      (1U)
#define RF_PPU_OSMX_HW_MSK                                      (((1U<<RF_PPU_OSMX_HW_LEN)-1)<<RF_PPU_OSMX_HW_POS)
#define RF_PPU_OSMX_HW_UMSK                                     (~(((1U<<RF_PPU_OSMX_HW_LEN)-1)<<RF_PPU_OSMX_HW_POS))
#define RF_PPU_RXBUF_HW                                         RF_PPU_RXBUF_HW
#define RF_PPU_RXBUF_HW_POS                                     (24U)
#define RF_PPU_RXBUF_HW_LEN                                     (1U)
#define RF_PPU_RXBUF_HW_MSK                                     (((1U<<RF_PPU_RXBUF_HW_LEN)-1)<<RF_PPU_RXBUF_HW_POS)
#define RF_PPU_RXBUF_HW_UMSK                                    (~(((1U<<RF_PPU_RXBUF_HW_LEN)-1)<<RF_PPU_RXBUF_HW_POS))
#define RF_PPU_TXBUF_HW                                         RF_PPU_TXBUF_HW
#define RF_PPU_TXBUF_HW_POS                                     (25U)
#define RF_PPU_TXBUF_HW_LEN                                     (1U)
#define RF_PPU_TXBUF_HW_MSK                                     (((1U<<RF_PPU_TXBUF_HW_LEN)-1)<<RF_PPU_TXBUF_HW_POS)
#define RF_PPU_TXBUF_HW_UMSK                                    (~(((1U<<RF_PPU_TXBUF_HW_LEN)-1)<<RF_PPU_TXBUF_HW_POS))

/* 0x44 : pud_ctrl_hw */
#define RF_PUD_CTRL_HW_OFFSET                                   (0x44)
#define RF_PUD_VCO_HW                                           RF_PUD_VCO_HW
#define RF_PUD_VCO_HW_POS                                       (20U)
#define RF_PUD_VCO_HW_LEN                                       (1U)
#define RF_PUD_VCO_HW_MSK                                       (((1U<<RF_PUD_VCO_HW_LEN)-1)<<RF_PUD_VCO_HW_POS)
#define RF_PUD_VCO_HW_UMSK                                      (~(((1U<<RF_PUD_VCO_HW_LEN)-1)<<RF_PUD_VCO_HW_POS))

/* 0x48 : gain control1 */
#define RF_TRX_GAIN1_OFFSET                                     (0x48)
#define RF_GC_LNA                                               RF_GC_LNA
#define RF_GC_LNA_POS                                           (0U)
#define RF_GC_LNA_LEN                                           (3U)
#define RF_GC_LNA_MSK                                           (((1U<<RF_GC_LNA_LEN)-1)<<RF_GC_LNA_POS)
#define RF_GC_LNA_UMSK                                          (~(((1U<<RF_GC_LNA_LEN)-1)<<RF_GC_LNA_POS))
#define RF_GC_RMXGM                                             RF_GC_RMXGM
#define RF_GC_RMXGM_POS                                         (3U)
#define RF_GC_RMXGM_LEN                                         (2U)
#define RF_GC_RMXGM_MSK                                         (((1U<<RF_GC_RMXGM_LEN)-1)<<RF_GC_RMXGM_POS)
#define RF_GC_RMXGM_UMSK                                        (~(((1U<<RF_GC_RMXGM_LEN)-1)<<RF_GC_RMXGM_POS))
#define RF_RMXGM_RATT                                           RF_RMXGM_RATT
#define RF_RMXGM_RATT_POS                                       (5U)
#define RF_RMXGM_RATT_LEN                                       (3U)
#define RF_RMXGM_RATT_MSK                                       (((1U<<RF_RMXGM_RATT_LEN)-1)<<RF_RMXGM_RATT_POS)
#define RF_RMXGM_RATT_UMSK                                      (~(((1U<<RF_RMXGM_RATT_LEN)-1)<<RF_RMXGM_RATT_POS))
#define RF_GC_RBB1                                              RF_GC_RBB1
#define RF_GC_RBB1_POS                                          (8U)
#define RF_GC_RBB1_LEN                                          (2U)
#define RF_GC_RBB1_MSK                                          (((1U<<RF_GC_RBB1_LEN)-1)<<RF_GC_RBB1_POS)
#define RF_GC_RBB1_UMSK                                         (~(((1U<<RF_GC_RBB1_LEN)-1)<<RF_GC_RBB1_POS))
#define RF_GC_RBB2                                              RF_GC_RBB2
#define RF_GC_RBB2_POS                                          (12U)
#define RF_GC_RBB2_LEN                                          (3U)
#define RF_GC_RBB2_MSK                                          (((1U<<RF_GC_RBB2_LEN)-1)<<RF_GC_RBB2_POS)
#define RF_GC_RBB2_UMSK                                         (~(((1U<<RF_GC_RBB2_LEN)-1)<<RF_GC_RBB2_POS))
#define RF_GC_TMX                                               RF_GC_TMX
#define RF_GC_TMX_POS                                           (16U)
#define RF_GC_TMX_LEN                                           (3U)
#define RF_GC_TMX_MSK                                           (((1U<<RF_GC_TMX_LEN)-1)<<RF_GC_TMX_POS)
#define RF_GC_TMX_UMSK                                          (~(((1U<<RF_GC_TMX_LEN)-1)<<RF_GC_TMX_POS))
#define RF_GC_TBB                                               RF_GC_TBB
#define RF_GC_TBB_POS                                           (20U)
#define RF_GC_TBB_LEN                                           (5U)
#define RF_GC_TBB_MSK                                           (((1U<<RF_GC_TBB_LEN)-1)<<RF_GC_TBB_POS)
#define RF_GC_TBB_UMSK                                          (~(((1U<<RF_GC_TBB_LEN)-1)<<RF_GC_TBB_POS))
#define RF_GC_TBB_BOOST                                         RF_GC_TBB_BOOST
#define RF_GC_TBB_BOOST_POS                                     (28U)
#define RF_GC_TBB_BOOST_LEN                                     (2U)
#define RF_GC_TBB_BOOST_MSK                                     (((1U<<RF_GC_TBB_BOOST_LEN)-1)<<RF_GC_TBB_BOOST_POS)
#define RF_GC_TBB_BOOST_UMSK                                    (~(((1U<<RF_GC_TBB_BOOST_LEN)-1)<<RF_GC_TBB_BOOST_POS))

/* 0x4C : trx gain hardware readback */
#define RF_TRX_GAIN_HW_OFFSET                                   (0x4C)
#define RF_GC_LNA_HW                                            RF_GC_LNA_HW
#define RF_GC_LNA_HW_POS                                        (0U)
#define RF_GC_LNA_HW_LEN                                        (3U)
#define RF_GC_LNA_HW_MSK                                        (((1U<<RF_GC_LNA_HW_LEN)-1)<<RF_GC_LNA_HW_POS)
#define RF_GC_LNA_HW_UMSK                                       (~(((1U<<RF_GC_LNA_HW_LEN)-1)<<RF_GC_LNA_HW_POS))
#define RF_GC_RMXGM_HW                                          RF_GC_RMXGM_HW
#define RF_GC_RMXGM_HW_POS                                      (3U)
#define RF_GC_RMXGM_HW_LEN                                      (2U)
#define RF_GC_RMXGM_HW_MSK                                      (((1U<<RF_GC_RMXGM_HW_LEN)-1)<<RF_GC_RMXGM_HW_POS)
#define RF_GC_RMXGM_HW_UMSK                                     (~(((1U<<RF_GC_RMXGM_HW_LEN)-1)<<RF_GC_RMXGM_HW_POS))
#define RF_RMXGM_RATT_HW                                        RF_RMXGM_RATT_HW
#define RF_RMXGM_RATT_HW_POS                                    (5U)
#define RF_RMXGM_RATT_HW_LEN                                    (3U)
#define RF_RMXGM_RATT_HW_MSK                                    (((1U<<RF_RMXGM_RATT_HW_LEN)-1)<<RF_RMXGM_RATT_HW_POS)
#define RF_RMXGM_RATT_HW_UMSK                                   (~(((1U<<RF_RMXGM_RATT_HW_LEN)-1)<<RF_RMXGM_RATT_HW_POS))
#define RF_GC_RBB1_HW                                           RF_GC_RBB1_HW
#define RF_GC_RBB1_HW_POS                                       (8U)
#define RF_GC_RBB1_HW_LEN                                       (2U)
#define RF_GC_RBB1_HW_MSK                                       (((1U<<RF_GC_RBB1_HW_LEN)-1)<<RF_GC_RBB1_HW_POS)
#define RF_GC_RBB1_HW_UMSK                                      (~(((1U<<RF_GC_RBB1_HW_LEN)-1)<<RF_GC_RBB1_HW_POS))
#define RF_GC_RBB2_HW                                           RF_GC_RBB2_HW
#define RF_GC_RBB2_HW_POS                                       (12U)
#define RF_GC_RBB2_HW_LEN                                       (3U)
#define RF_GC_RBB2_HW_MSK                                       (((1U<<RF_GC_RBB2_HW_LEN)-1)<<RF_GC_RBB2_HW_POS)
#define RF_GC_RBB2_HW_UMSK                                      (~(((1U<<RF_GC_RBB2_HW_LEN)-1)<<RF_GC_RBB2_HW_POS))
#define RF_GC_TMX_HW                                            RF_GC_TMX_HW
#define RF_GC_TMX_HW_POS                                        (16U)
#define RF_GC_TMX_HW_LEN                                        (3U)
#define RF_GC_TMX_HW_MSK                                        (((1U<<RF_GC_TMX_HW_LEN)-1)<<RF_GC_TMX_HW_POS)
#define RF_GC_TMX_HW_UMSK                                       (~(((1U<<RF_GC_TMX_HW_LEN)-1)<<RF_GC_TMX_HW_POS))
#define RF_GC_TBB_HW                                            RF_GC_TBB_HW
#define RF_GC_TBB_HW_POS                                        (20U)
#define RF_GC_TBB_HW_LEN                                        (5U)
#define RF_GC_TBB_HW_MSK                                        (((1U<<RF_GC_TBB_HW_LEN)-1)<<RF_GC_TBB_HW_POS)
#define RF_GC_TBB_HW_UMSK                                       (~(((1U<<RF_GC_TBB_HW_LEN)-1)<<RF_GC_TBB_HW_POS))
#define RF_GC_TBB_BOOST_HW                                      RF_GC_TBB_BOOST_HW
#define RF_GC_TBB_BOOST_HW_POS                                  (28U)
#define RF_GC_TBB_BOOST_HW_LEN                                  (2U)
#define RF_GC_TBB_BOOST_HW_MSK                                  (((1U<<RF_GC_TBB_BOOST_HW_LEN)-1)<<RF_GC_TBB_BOOST_HW_POS)
#define RF_GC_TBB_BOOST_HW_UMSK                                 (~(((1U<<RF_GC_TBB_BOOST_HW_LEN)-1)<<RF_GC_TBB_BOOST_HW_POS))
#define RF_DAC_BIAS_SEL_HW                                      RF_DAC_BIAS_SEL_HW
#define RF_DAC_BIAS_SEL_HW_POS                                  (30U)
#define RF_DAC_BIAS_SEL_HW_LEN                                  (2U)
#define RF_DAC_BIAS_SEL_HW_MSK                                  (((1U<<RF_DAC_BIAS_SEL_HW_LEN)-1)<<RF_DAC_BIAS_SEL_HW_POS)
#define RF_DAC_BIAS_SEL_HW_UMSK                                 (~(((1U<<RF_DAC_BIAS_SEL_HW_LEN)-1)<<RF_DAC_BIAS_SEL_HW_POS))

/* 0x50 : dc test register */
#define RF_TEN_DC_OFFSET                                        (0x50)
#define RF_DC_TP_33_EN                                          RF_DC_TP_33_EN
#define RF_DC_TP_33_EN_POS                                      (1U)
#define RF_DC_TP_33_EN_LEN                                      (1U)
#define RF_DC_TP_33_EN_MSK                                      (((1U<<RF_DC_TP_33_EN_LEN)-1)<<RF_DC_TP_33_EN_POS)
#define RF_DC_TP_33_EN_UMSK                                     (~(((1U<<RF_DC_TP_33_EN_LEN)-1)<<RF_DC_TP_33_EN_POS))
#define RF_DC_TP_25_EN                                          RF_DC_TP_25_EN
#define RF_DC_TP_25_EN_POS                                      (2U)
#define RF_DC_TP_25_EN_LEN                                      (1U)
#define RF_DC_TP_25_EN_MSK                                      (((1U<<RF_DC_TP_25_EN_LEN)-1)<<RF_DC_TP_25_EN_POS)
#define RF_DC_TP_25_EN_UMSK                                     (~(((1U<<RF_DC_TP_25_EN_LEN)-1)<<RF_DC_TP_25_EN_POS))
#define RF_DC_TP_18_EN                                          RF_DC_TP_18_EN
#define RF_DC_TP_18_EN_POS                                      (3U)
#define RF_DC_TP_18_EN_LEN                                      (1U)
#define RF_DC_TP_18_EN_MSK                                      (((1U<<RF_DC_TP_18_EN_LEN)-1)<<RF_DC_TP_18_EN_POS)
#define RF_DC_TP_18_EN_UMSK                                     (~(((1U<<RF_DC_TP_18_EN_LEN)-1)<<RF_DC_TP_18_EN_POS))
#define RF_DC_TP_15_EN                                          RF_DC_TP_15_EN
#define RF_DC_TP_15_EN_POS                                      (4U)
#define RF_DC_TP_15_EN_LEN                                      (1U)
#define RF_DC_TP_15_EN_MSK                                      (((1U<<RF_DC_TP_15_EN_LEN)-1)<<RF_DC_TP_15_EN_POS)
#define RF_DC_TP_15_EN_UMSK                                     (~(((1U<<RF_DC_TP_15_EN_LEN)-1)<<RF_DC_TP_15_EN_POS))
#define RF_TEN_RRF_0                                            RF_TEN_RRF_0
#define RF_TEN_RRF_0_POS                                        (12U)
#define RF_TEN_RRF_0_LEN                                        (1U)
#define RF_TEN_RRF_0_MSK                                        (((1U<<RF_TEN_RRF_0_LEN)-1)<<RF_TEN_RRF_0_POS)
#define RF_TEN_RRF_0_UMSK                                       (~(((1U<<RF_TEN_RRF_0_LEN)-1)<<RF_TEN_RRF_0_POS))
#define RF_TEN_RRF_1                                            RF_TEN_RRF_1
#define RF_TEN_RRF_1_POS                                        (13U)
#define RF_TEN_RRF_1_LEN                                        (1U)
#define RF_TEN_RRF_1_MSK                                        (((1U<<RF_TEN_RRF_1_LEN)-1)<<RF_TEN_RRF_1_POS)
#define RF_TEN_RRF_1_UMSK                                       (~(((1U<<RF_TEN_RRF_1_LEN)-1)<<RF_TEN_RRF_1_POS))
#define RF_TEN_PA                                               RF_TEN_PA
#define RF_TEN_PA_POS                                           (14U)
#define RF_TEN_PA_LEN                                           (1U)
#define RF_TEN_PA_MSK                                           (((1U<<RF_TEN_PA_LEN)-1)<<RF_TEN_PA_POS)
#define RF_TEN_PA_UMSK                                          (~(((1U<<RF_TEN_PA_LEN)-1)<<RF_TEN_PA_POS))
#define RF_TEN_TMX                                              RF_TEN_TMX
#define RF_TEN_TMX_POS                                          (15U)
#define RF_TEN_TMX_LEN                                          (1U)
#define RF_TEN_TMX_MSK                                          (((1U<<RF_TEN_TMX_LEN)-1)<<RF_TEN_TMX_POS)
#define RF_TEN_TMX_UMSK                                         (~(((1U<<RF_TEN_TMX_LEN)-1)<<RF_TEN_TMX_POS))
#define RF_TEN_TIA                                              RF_TEN_TIA
#define RF_TEN_TIA_POS                                          (16U)
#define RF_TEN_TIA_LEN                                          (1U)
#define RF_TEN_TIA_MSK                                          (((1U<<RF_TEN_TIA_LEN)-1)<<RF_TEN_TIA_POS)
#define RF_TEN_TIA_UMSK                                         (~(((1U<<RF_TEN_TIA_LEN)-1)<<RF_TEN_TIA_POS))
#define RF_TEN_BQ                                               RF_TEN_BQ
#define RF_TEN_BQ_POS                                           (17U)
#define RF_TEN_BQ_LEN                                           (1U)
#define RF_TEN_BQ_MSK                                           (((1U<<RF_TEN_BQ_LEN)-1)<<RF_TEN_BQ_POS)
#define RF_TEN_BQ_UMSK                                          (~(((1U<<RF_TEN_BQ_LEN)-1)<<RF_TEN_BQ_POS))
#define RF_TEN_ATEST                                            RF_TEN_ATEST
#define RF_TEN_ATEST_POS                                        (18U)
#define RF_TEN_ATEST_LEN                                        (1U)
#define RF_TEN_ATEST_MSK                                        (((1U<<RF_TEN_ATEST_LEN)-1)<<RF_TEN_ATEST_POS)
#define RF_TEN_ATEST_UMSK                                       (~(((1U<<RF_TEN_ATEST_LEN)-1)<<RF_TEN_ATEST_POS))
#define RF_TEN_TBB                                              RF_TEN_TBB
#define RF_TEN_TBB_POS                                          (19U)
#define RF_TEN_TBB_LEN                                          (1U)
#define RF_TEN_TBB_MSK                                          (((1U<<RF_TEN_TBB_LEN)-1)<<RF_TEN_TBB_POS)
#define RF_TEN_TBB_UMSK                                         (~(((1U<<RF_TEN_TBB_LEN)-1)<<RF_TEN_TBB_POS))
#define RF_TEN_ADC                                              RF_TEN_ADC
#define RF_TEN_ADC_POS                                          (20U)
#define RF_TEN_ADC_LEN                                          (1U)
#define RF_TEN_ADC_MSK                                          (((1U<<RF_TEN_ADC_LEN)-1)<<RF_TEN_ADC_POS)
#define RF_TEN_ADC_UMSK                                         (~(((1U<<RF_TEN_ADC_LEN)-1)<<RF_TEN_ADC_POS))
#define RF_TEN_DAC_I                                            RF_TEN_DAC_I
#define RF_TEN_DAC_I_POS                                        (21U)
#define RF_TEN_DAC_I_LEN                                        (1U)
#define RF_TEN_DAC_I_MSK                                        (((1U<<RF_TEN_DAC_I_LEN)-1)<<RF_TEN_DAC_I_POS)
#define RF_TEN_DAC_I_UMSK                                       (~(((1U<<RF_TEN_DAC_I_LEN)-1)<<RF_TEN_DAC_I_POS))
#define RF_TEN_DAC_Q                                            RF_TEN_DAC_Q
#define RF_TEN_DAC_Q_POS                                        (22U)
#define RF_TEN_DAC_Q_LEN                                        (1U)
#define RF_TEN_DAC_Q_MSK                                        (((1U<<RF_TEN_DAC_Q_LEN)-1)<<RF_TEN_DAC_Q_POS)
#define RF_TEN_DAC_Q_UMSK                                       (~(((1U<<RF_TEN_DAC_Q_LEN)-1)<<RF_TEN_DAC_Q_POS))
#define RF_TEN_VCO_0                                            RF_TEN_VCO_0
#define RF_TEN_VCO_0_POS                                        (24U)
#define RF_TEN_VCO_0_LEN                                        (1U)
#define RF_TEN_VCO_0_MSK                                        (((1U<<RF_TEN_VCO_0_LEN)-1)<<RF_TEN_VCO_0_POS)
#define RF_TEN_VCO_0_UMSK                                       (~(((1U<<RF_TEN_VCO_0_LEN)-1)<<RF_TEN_VCO_0_POS))
#define RF_TEN_VCO_1                                            RF_TEN_VCO_1
#define RF_TEN_VCO_1_POS                                        (25U)
#define RF_TEN_VCO_1_LEN                                        (1U)
#define RF_TEN_VCO_1_MSK                                        (((1U<<RF_TEN_VCO_1_LEN)-1)<<RF_TEN_VCO_1_POS)
#define RF_TEN_VCO_1_UMSK                                       (~(((1U<<RF_TEN_VCO_1_LEN)-1)<<RF_TEN_VCO_1_POS))
#define RF_TEN_PFDCP                                            RF_TEN_PFDCP
#define RF_TEN_PFDCP_POS                                        (26U)
#define RF_TEN_PFDCP_LEN                                        (1U)
#define RF_TEN_PFDCP_MSK                                        (((1U<<RF_TEN_PFDCP_LEN)-1)<<RF_TEN_PFDCP_POS)
#define RF_TEN_PFDCP_UMSK                                       (~(((1U<<RF_TEN_PFDCP_LEN)-1)<<RF_TEN_PFDCP_POS))
#define RF_TEN_LF                                               RF_TEN_LF
#define RF_TEN_LF_POS                                           (27U)
#define RF_TEN_LF_LEN                                           (1U)
#define RF_TEN_LF_MSK                                           (((1U<<RF_TEN_LF_LEN)-1)<<RF_TEN_LF_POS)
#define RF_TEN_LF_UMSK                                          (~(((1U<<RF_TEN_LF_LEN)-1)<<RF_TEN_LF_POS))
#define RF_TEN_LODIST                                           RF_TEN_LODIST
#define RF_TEN_LODIST_POS                                       (28U)
#define RF_TEN_LODIST_LEN                                       (1U)
#define RF_TEN_LODIST_MSK                                       (((1U<<RF_TEN_LODIST_LEN)-1)<<RF_TEN_LODIST_POS)
#define RF_TEN_LODIST_UMSK                                      (~(((1U<<RF_TEN_LODIST_LEN)-1)<<RF_TEN_LODIST_POS))

/* 0x54 : digital test register */
#define RF_TEN_DIG_OFFSET                                       (0x54)
#define RF_DTEN_LO_FSDM                                         RF_DTEN_LO_FSDM
#define RF_DTEN_LO_FSDM_POS                                     (6U)
#define RF_DTEN_LO_FSDM_LEN                                     (1U)
#define RF_DTEN_LO_FSDM_MSK                                     (((1U<<RF_DTEN_LO_FSDM_LEN)-1)<<RF_DTEN_LO_FSDM_POS)
#define RF_DTEN_LO_FSDM_UMSK                                    (~(((1U<<RF_DTEN_LO_FSDM_LEN)-1)<<RF_DTEN_LO_FSDM_POS))
#define RF_DTEN_LO_FREF                                         RF_DTEN_LO_FREF
#define RF_DTEN_LO_FREF_POS                                     (8U)
#define RF_DTEN_LO_FREF_LEN                                     (1U)
#define RF_DTEN_LO_FREF_MSK                                     (((1U<<RF_DTEN_LO_FREF_LEN)-1)<<RF_DTEN_LO_FREF_POS)
#define RF_DTEN_LO_FREF_UMSK                                    (~(((1U<<RF_DTEN_LO_FREF_LEN)-1)<<RF_DTEN_LO_FREF_POS))
#define RF_DTEST_PULL_DOWN_RF                                   RF_DTEST_PULL_DOWN_RF
#define RF_DTEST_PULL_DOWN_RF_POS                               (9U)
#define RF_DTEST_PULL_DOWN_RF_LEN                               (1U)
#define RF_DTEST_PULL_DOWN_RF_MSK                               (((1U<<RF_DTEST_PULL_DOWN_RF_LEN)-1)<<RF_DTEST_PULL_DOWN_RF_POS)
#define RF_DTEST_PULL_DOWN_RF_UMSK                              (~(((1U<<RF_DTEST_PULL_DOWN_RF_LEN)-1)<<RF_DTEST_PULL_DOWN_RF_POS))
#define RF_DTEST_EN                                             RF_DTEST_EN
#define RF_DTEST_EN_POS                                         (23U)
#define RF_DTEST_EN_LEN                                         (1U)
#define RF_DTEST_EN_MSK                                         (((1U<<RF_DTEST_EN_LEN)-1)<<RF_DTEST_EN_POS)
#define RF_DTEST_EN_UMSK                                        (~(((1U<<RF_DTEST_EN_LEN)-1)<<RF_DTEST_EN_POS))

/* 0x58 : ac test register */
#define RF_TEN_AC_OFFSET                                        (0x58)
#define RF_ATEST_OP_CC                                          RF_ATEST_OP_CC
#define RF_ATEST_OP_CC_POS                                      (0U)
#define RF_ATEST_OP_CC_LEN                                      (4U)
#define RF_ATEST_OP_CC_MSK                                      (((1U<<RF_ATEST_OP_CC_LEN)-1)<<RF_ATEST_OP_CC_POS)
#define RF_ATEST_OP_CC_UMSK                                     (~(((1U<<RF_ATEST_OP_CC_LEN)-1)<<RF_ATEST_OP_CC_POS))
#define RF_ATEST_DAC_EN                                         RF_ATEST_DAC_EN
#define RF_ATEST_DAC_EN_POS                                     (4U)
#define RF_ATEST_DAC_EN_LEN                                     (1U)
#define RF_ATEST_DAC_EN_MSK                                     (((1U<<RF_ATEST_DAC_EN_LEN)-1)<<RF_ATEST_DAC_EN_POS)
#define RF_ATEST_DAC_EN_UMSK                                    (~(((1U<<RF_ATEST_DAC_EN_LEN)-1)<<RF_ATEST_DAC_EN_POS))
#define RF_ATEST_IN_TRX_SW                                      RF_ATEST_IN_TRX_SW
#define RF_ATEST_IN_TRX_SW_POS                                  (5U)
#define RF_ATEST_IN_TRX_SW_LEN                                  (1U)
#define RF_ATEST_IN_TRX_SW_MSK                                  (((1U<<RF_ATEST_IN_TRX_SW_LEN)-1)<<RF_ATEST_IN_TRX_SW_POS)
#define RF_ATEST_IN_TRX_SW_UMSK                                 (~(((1U<<RF_ATEST_IN_TRX_SW_LEN)-1)<<RF_ATEST_IN_TRX_SW_POS))
#define RF_ATEST_IN_EN                                          RF_ATEST_IN_EN
#define RF_ATEST_IN_EN_POS                                      (6U)
#define RF_ATEST_IN_EN_LEN                                      (1U)
#define RF_ATEST_IN_EN_MSK                                      (((1U<<RF_ATEST_IN_EN_LEN)-1)<<RF_ATEST_IN_EN_POS)
#define RF_ATEST_IN_EN_UMSK                                     (~(((1U<<RF_ATEST_IN_EN_LEN)-1)<<RF_ATEST_IN_EN_POS))
#define RF_ATEST_GAIN_R9                                        RF_ATEST_GAIN_R9
#define RF_ATEST_GAIN_R9_POS                                    (8U)
#define RF_ATEST_GAIN_R9_LEN                                    (2U)
#define RF_ATEST_GAIN_R9_MSK                                    (((1U<<RF_ATEST_GAIN_R9_LEN)-1)<<RF_ATEST_GAIN_R9_POS)
#define RF_ATEST_GAIN_R9_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R9_LEN)-1)<<RF_ATEST_GAIN_R9_POS))
#define RF_ATEST_GAIN_R8                                        RF_ATEST_GAIN_R8
#define RF_ATEST_GAIN_R8_POS                                    (10U)
#define RF_ATEST_GAIN_R8_LEN                                    (2U)
#define RF_ATEST_GAIN_R8_MSK                                    (((1U<<RF_ATEST_GAIN_R8_LEN)-1)<<RF_ATEST_GAIN_R8_POS)
#define RF_ATEST_GAIN_R8_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R8_LEN)-1)<<RF_ATEST_GAIN_R8_POS))
#define RF_ATEST_GAIN_R7                                        RF_ATEST_GAIN_R7
#define RF_ATEST_GAIN_R7_POS                                    (12U)
#define RF_ATEST_GAIN_R7_LEN                                    (2U)
#define RF_ATEST_GAIN_R7_MSK                                    (((1U<<RF_ATEST_GAIN_R7_LEN)-1)<<RF_ATEST_GAIN_R7_POS)
#define RF_ATEST_GAIN_R7_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R7_LEN)-1)<<RF_ATEST_GAIN_R7_POS))
#define RF_ATEST_GAIN_R6                                        RF_ATEST_GAIN_R6
#define RF_ATEST_GAIN_R6_POS                                    (14U)
#define RF_ATEST_GAIN_R6_LEN                                    (2U)
#define RF_ATEST_GAIN_R6_MSK                                    (((1U<<RF_ATEST_GAIN_R6_LEN)-1)<<RF_ATEST_GAIN_R6_POS)
#define RF_ATEST_GAIN_R6_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R6_LEN)-1)<<RF_ATEST_GAIN_R6_POS))
#define RF_ATEST_GAIN_R5                                        RF_ATEST_GAIN_R5
#define RF_ATEST_GAIN_R5_POS                                    (16U)
#define RF_ATEST_GAIN_R5_LEN                                    (3U)
#define RF_ATEST_GAIN_R5_MSK                                    (((1U<<RF_ATEST_GAIN_R5_LEN)-1)<<RF_ATEST_GAIN_R5_POS)
#define RF_ATEST_GAIN_R5_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R5_LEN)-1)<<RF_ATEST_GAIN_R5_POS))
#define RF_ATEST_OUT_EN_Q                                       RF_ATEST_OUT_EN_Q
#define RF_ATEST_OUT_EN_Q_POS                                   (20U)
#define RF_ATEST_OUT_EN_Q_LEN                                   (1U)
#define RF_ATEST_OUT_EN_Q_MSK                                   (((1U<<RF_ATEST_OUT_EN_Q_LEN)-1)<<RF_ATEST_OUT_EN_Q_POS)
#define RF_ATEST_OUT_EN_Q_UMSK                                  (~(((1U<<RF_ATEST_OUT_EN_Q_LEN)-1)<<RF_ATEST_OUT_EN_Q_POS))
#define RF_ATEST_OUT_EN_I                                       RF_ATEST_OUT_EN_I
#define RF_ATEST_OUT_EN_I_POS                                   (21U)
#define RF_ATEST_OUT_EN_I_LEN                                   (1U)
#define RF_ATEST_OUT_EN_I_MSK                                   (((1U<<RF_ATEST_OUT_EN_I_LEN)-1)<<RF_ATEST_OUT_EN_I_POS)
#define RF_ATEST_OUT_EN_I_UMSK                                  (~(((1U<<RF_ATEST_OUT_EN_I_LEN)-1)<<RF_ATEST_OUT_EN_I_POS))
#define RF_ATEST_IN_EN_Q                                        RF_ATEST_IN_EN_Q
#define RF_ATEST_IN_EN_Q_POS                                    (22U)
#define RF_ATEST_IN_EN_Q_LEN                                    (1U)
#define RF_ATEST_IN_EN_Q_MSK                                    (((1U<<RF_ATEST_IN_EN_Q_LEN)-1)<<RF_ATEST_IN_EN_Q_POS)
#define RF_ATEST_IN_EN_Q_UMSK                                   (~(((1U<<RF_ATEST_IN_EN_Q_LEN)-1)<<RF_ATEST_IN_EN_Q_POS))
#define RF_ATEST_IN_EN_I                                        RF_ATEST_IN_EN_I
#define RF_ATEST_IN_EN_I_POS                                    (23U)
#define RF_ATEST_IN_EN_I_LEN                                    (1U)
#define RF_ATEST_IN_EN_I_MSK                                    (((1U<<RF_ATEST_IN_EN_I_LEN)-1)<<RF_ATEST_IN_EN_I_POS)
#define RF_ATEST_IN_EN_I_UMSK                                   (~(((1U<<RF_ATEST_IN_EN_I_LEN)-1)<<RF_ATEST_IN_EN_I_POS))

/* 0x5C : RX normal bias mode registers */
#define RF_CIP_0_OFFSET                                         (0x5C)
#define RF_VG11_SEL                                             RF_VG11_SEL
#define RF_VG11_SEL_POS                                         (0U)
#define RF_VG11_SEL_LEN                                         (2U)
#define RF_VG11_SEL_MSK                                         (((1U<<RF_VG11_SEL_LEN)-1)<<RF_VG11_SEL_POS)
#define RF_VG11_SEL_UMSK                                        (~(((1U<<RF_VG11_SEL_LEN)-1)<<RF_VG11_SEL_POS))
#define RF_VG13_SEL                                             RF_VG13_SEL
#define RF_VG13_SEL_POS                                         (2U)
#define RF_VG13_SEL_LEN                                         (2U)
#define RF_VG13_SEL_MSK                                         (((1U<<RF_VG13_SEL_LEN)-1)<<RF_VG13_SEL_POS)
#define RF_VG13_SEL_UMSK                                        (~(((1U<<RF_VG13_SEL_LEN)-1)<<RF_VG13_SEL_POS))
#define RF_RCAL_ICX_CODE                                        RF_RCAL_ICX_CODE
#define RF_RCAL_ICX_CODE_POS                                    (4U)
#define RF_RCAL_ICX_CODE_LEN                                    (6U)
#define RF_RCAL_ICX_CODE_MSK                                    (((1U<<RF_RCAL_ICX_CODE_LEN)-1)<<RF_RCAL_ICX_CODE_POS)
#define RF_RCAL_ICX_CODE_UMSK                                   (~(((1U<<RF_RCAL_ICX_CODE_LEN)-1)<<RF_RCAL_ICX_CODE_POS))
#define RF_RCAL_ICX_OUT_EN                                      RF_RCAL_ICX_OUT_EN
#define RF_RCAL_ICX_OUT_EN_POS                                  (12U)
#define RF_RCAL_ICX_OUT_EN_LEN                                  (1U)
#define RF_RCAL_ICX_OUT_EN_MSK                                  (((1U<<RF_RCAL_ICX_OUT_EN_LEN)-1)<<RF_RCAL_ICX_OUT_EN_POS)
#define RF_RCAL_ICX_OUT_EN_UMSK                                 (~(((1U<<RF_RCAL_ICX_OUT_EN_LEN)-1)<<RF_RCAL_ICX_OUT_EN_POS))
#define RF_RCAL_ICX_TSWITCH_EN                                  RF_RCAL_ICX_TSWITCH_EN
#define RF_RCAL_ICX_TSWITCH_EN_POS                              (13U)
#define RF_RCAL_ICX_TSWITCH_EN_LEN                              (1U)
#define RF_RCAL_ICX_TSWITCH_EN_MSK                              (((1U<<RF_RCAL_ICX_TSWITCH_EN_LEN)-1)<<RF_RCAL_ICX_TSWITCH_EN_POS)
#define RF_RCAL_ICX_TSWITCH_EN_UMSK                             (~(((1U<<RF_RCAL_ICX_TSWITCH_EN_LEN)-1)<<RF_RCAL_ICX_TSWITCH_EN_POS))
#define RF_RCAL_IPTAT_CODE                                      RF_RCAL_IPTAT_CODE
#define RF_RCAL_IPTAT_CODE_POS                                  (16U)
#define RF_RCAL_IPTAT_CODE_LEN                                  (5U)
#define RF_RCAL_IPTAT_CODE_MSK                                  (((1U<<RF_RCAL_IPTAT_CODE_LEN)-1)<<RF_RCAL_IPTAT_CODE_POS)
#define RF_RCAL_IPTAT_CODE_UMSK                                 (~(((1U<<RF_RCAL_IPTAT_CODE_LEN)-1)<<RF_RCAL_IPTAT_CODE_POS))
#define RF_RCAL_IPTAT_OUT_EN                                    RF_RCAL_IPTAT_OUT_EN
#define RF_RCAL_IPTAT_OUT_EN_POS                                (24U)
#define RF_RCAL_IPTAT_OUT_EN_LEN                                (1U)
#define RF_RCAL_IPTAT_OUT_EN_MSK                                (((1U<<RF_RCAL_IPTAT_OUT_EN_LEN)-1)<<RF_RCAL_IPTAT_OUT_EN_POS)
#define RF_RCAL_IPTAT_OUT_EN_UMSK                               (~(((1U<<RF_RCAL_IPTAT_OUT_EN_LEN)-1)<<RF_RCAL_IPTAT_OUT_EN_POS))
#define RF_RCAL_IPTAT_TSWITCH_EN                                RF_RCAL_IPTAT_TSWITCH_EN
#define RF_RCAL_IPTAT_TSWITCH_EN_POS                            (25U)
#define RF_RCAL_IPTAT_TSWITCH_EN_LEN                            (1U)
#define RF_RCAL_IPTAT_TSWITCH_EN_MSK                            (((1U<<RF_RCAL_IPTAT_TSWITCH_EN_LEN)-1)<<RF_RCAL_IPTAT_TSWITCH_EN_POS)
#define RF_RCAL_IPTAT_TSWITCH_EN_UMSK                           (~(((1U<<RF_RCAL_IPTAT_TSWITCH_EN_LEN)-1)<<RF_RCAL_IPTAT_TSWITCH_EN_POS))

/* 0x60 : pa0 */
#define RF_PA0_OFFSET                                           (0x60)
#define RF_PA_PWRMX_OSDAC_BM                                    RF_PA_PWRMX_OSDAC_BM
#define RF_PA_PWRMX_OSDAC_BM_POS                                (0U)
#define RF_PA_PWRMX_OSDAC_BM_LEN                                (2U)
#define RF_PA_PWRMX_OSDAC_BM_MSK                                (((1U<<RF_PA_PWRMX_OSDAC_BM_LEN)-1)<<RF_PA_PWRMX_OSDAC_BM_POS)
#define RF_PA_PWRMX_OSDAC_BM_UMSK                               (~(((1U<<RF_PA_PWRMX_OSDAC_BM_LEN)-1)<<RF_PA_PWRMX_OSDAC_BM_POS))

/* 0x64 : pa1 */
#define RF_PA1_OFFSET                                           (0x64)
#define RF_PA_LZ_BIAS_EN                                        RF_PA_LZ_BIAS_EN
#define RF_PA_LZ_BIAS_EN_POS                                    (17U)
#define RF_PA_LZ_BIAS_EN_LEN                                    (1U)
#define RF_PA_LZ_BIAS_EN_MSK                                    (((1U<<RF_PA_LZ_BIAS_EN_LEN)-1)<<RF_PA_LZ_BIAS_EN_POS)
#define RF_PA_LZ_BIAS_EN_UMSK                                   (~(((1U<<RF_PA_LZ_BIAS_EN_LEN)-1)<<RF_PA_LZ_BIAS_EN_POS))
#define RF_PA_PWRMX_OSDAC                                       RF_PA_PWRMX_OSDAC
#define RF_PA_PWRMX_OSDAC_POS                                   (18U)
#define RF_PA_PWRMX_OSDAC_LEN                                   (5U)
#define RF_PA_PWRMX_OSDAC_MSK                                   (((1U<<RF_PA_PWRMX_OSDAC_LEN)-1)<<RF_PA_PWRMX_OSDAC_POS)
#define RF_PA_PWRMX_OSDAC_UMSK                                  (~(((1U<<RF_PA_PWRMX_OSDAC_LEN)-1)<<RF_PA_PWRMX_OSDAC_POS))
#define RF_PA_PWRMX_DAC_PN_SWITCH                               RF_PA_PWRMX_DAC_PN_SWITCH
#define RF_PA_PWRMX_DAC_PN_SWITCH_POS                           (23U)
#define RF_PA_PWRMX_DAC_PN_SWITCH_LEN                           (1U)
#define RF_PA_PWRMX_DAC_PN_SWITCH_MSK                           (((1U<<RF_PA_PWRMX_DAC_PN_SWITCH_LEN)-1)<<RF_PA_PWRMX_DAC_PN_SWITCH_POS)
#define RF_PA_PWRMX_DAC_PN_SWITCH_UMSK                          (~(((1U<<RF_PA_PWRMX_DAC_PN_SWITCH_LEN)-1)<<RF_PA_PWRMX_DAC_PN_SWITCH_POS))
#define RF_PA_PWRMX_BM                                          RF_PA_PWRMX_BM
#define RF_PA_PWRMX_BM_POS                                      (24U)
#define RF_PA_PWRMX_BM_LEN                                      (3U)
#define RF_PA_PWRMX_BM_MSK                                      (((1U<<RF_PA_PWRMX_BM_LEN)-1)<<RF_PA_PWRMX_BM_POS)
#define RF_PA_PWRMX_BM_UMSK                                     (~(((1U<<RF_PA_PWRMX_BM_LEN)-1)<<RF_PA_PWRMX_BM_POS))
#define RF_PA_ATT_GC                                            RF_PA_ATT_GC
#define RF_PA_ATT_GC_POS                                        (28U)
#define RF_PA_ATT_GC_LEN                                        (4U)
#define RF_PA_ATT_GC_MSK                                        (((1U<<RF_PA_ATT_GC_LEN)-1)<<RF_PA_ATT_GC_POS)
#define RF_PA_ATT_GC_UMSK                                       (~(((1U<<RF_PA_ATT_GC_LEN)-1)<<RF_PA_ATT_GC_POS))

/* 0x68 : RX normal bias mode registers */
#define RF_PA2_OFFSET                                           (0x68)
#define RF_PA_VBCAS                                             RF_PA_VBCAS
#define RF_PA_VBCAS_POS                                         (0U)
#define RF_PA_VBCAS_LEN                                         (3U)
#define RF_PA_VBCAS_MSK                                         (((1U<<RF_PA_VBCAS_LEN)-1)<<RF_PA_VBCAS_POS)
#define RF_PA_VBCAS_UMSK                                        (~(((1U<<RF_PA_VBCAS_LEN)-1)<<RF_PA_VBCAS_POS))
#define RF_PA_IAQ                                               RF_PA_IAQ
#define RF_PA_IAQ_POS                                           (4U)
#define RF_PA_IAQ_LEN                                           (3U)
#define RF_PA_IAQ_MSK                                           (((1U<<RF_PA_IAQ_LEN)-1)<<RF_PA_IAQ_POS)
#define RF_PA_IAQ_UMSK                                          (~(((1U<<RF_PA_IAQ_LEN)-1)<<RF_PA_IAQ_POS))
#define RF_PA_IET                                               RF_PA_IET
#define RF_PA_IET_POS                                           (8U)
#define RF_PA_IET_LEN                                           (4U)
#define RF_PA_IET_MSK                                           (((1U<<RF_PA_IET_LEN)-1)<<RF_PA_IET_POS)
#define RF_PA_IET_UMSK                                          (~(((1U<<RF_PA_IET_LEN)-1)<<RF_PA_IET_POS))
#define RF_PA_VBPMOS_IET                                        RF_PA_VBPMOS_IET
#define RF_PA_VBPMOS_IET_POS                                    (12U)
#define RF_PA_VBPMOS_IET_LEN                                    (2U)
#define RF_PA_VBPMOS_IET_MSK                                    (((1U<<RF_PA_VBPMOS_IET_LEN)-1)<<RF_PA_VBPMOS_IET_POS)
#define RF_PA_VBPMOS_IET_UMSK                                   (~(((1U<<RF_PA_VBPMOS_IET_LEN)-1)<<RF_PA_VBPMOS_IET_POS))
#define RF_PA_VBPMOS                                            RF_PA_VBPMOS
#define RF_PA_VBPMOS_POS                                        (16U)
#define RF_PA_VBPMOS_LEN                                        (4U)
#define RF_PA_VBPMOS_MSK                                        (((1U<<RF_PA_VBPMOS_LEN)-1)<<RF_PA_VBPMOS_POS)
#define RF_PA_VBPMOS_UMSK                                       (~(((1U<<RF_PA_VBPMOS_LEN)-1)<<RF_PA_VBPMOS_POS))
#define RF_PA_VBCORE                                            RF_PA_VBCORE
#define RF_PA_VBCORE_POS                                        (20U)
#define RF_PA_VBCORE_LEN                                        (4U)
#define RF_PA_VBCORE_MSK                                        (((1U<<RF_PA_VBCORE_LEN)-1)<<RF_PA_VBCORE_POS)
#define RF_PA_VBCORE_UMSK                                       (~(((1U<<RF_PA_VBCORE_LEN)-1)<<RF_PA_VBCORE_POS))
#define RF_PA_ETB_EN                                            RF_PA_ETB_EN
#define RF_PA_ETB_EN_POS                                        (24U)
#define RF_PA_ETB_EN_LEN                                        (1U)
#define RF_PA_ETB_EN_MSK                                        (((1U<<RF_PA_ETB_EN_LEN)-1)<<RF_PA_ETB_EN_POS)
#define RF_PA_ETB_EN_UMSK                                       (~(((1U<<RF_PA_ETB_EN_LEN)-1)<<RF_PA_ETB_EN_POS))
#define RF_PA_LP_EN                                             RF_PA_LP_EN
#define RF_PA_LP_EN_POS                                         (25U)
#define RF_PA_LP_EN_LEN                                         (1U)
#define RF_PA_LP_EN_MSK                                         (((1U<<RF_PA_LP_EN_LEN)-1)<<RF_PA_LP_EN_POS)
#define RF_PA_LP_EN_UMSK                                        (~(((1U<<RF_PA_LP_EN_LEN)-1)<<RF_PA_LP_EN_POS))
#define RF_PA_IB_FIX                                            RF_PA_IB_FIX
#define RF_PA_IB_FIX_POS                                        (28U)
#define RF_PA_IB_FIX_LEN                                        (1U)
#define RF_PA_IB_FIX_MSK                                        (((1U<<RF_PA_IB_FIX_LEN)-1)<<RF_PA_IB_FIX_POS)
#define RF_PA_IB_FIX_UMSK                                       (~(((1U<<RF_PA_IB_FIX_LEN)-1)<<RF_PA_IB_FIX_POS))
#define RF_GC_PA                                                RF_GC_PA
#define RF_GC_PA_POS                                            (29U)
#define RF_GC_PA_LEN                                            (3U)
#define RF_GC_PA_MSK                                            (((1U<<RF_GC_PA_LEN)-1)<<RF_GC_PA_POS)
#define RF_GC_PA_UMSK                                           (~(((1U<<RF_GC_PA_LEN)-1)<<RF_GC_PA_POS))

/* 0x6C : pa3 */
#define RF_PA3_OFFSET                                           (0x6C)
#define RF_PA_VBCAS_HW                                          RF_PA_VBCAS_HW
#define RF_PA_VBCAS_HW_POS                                      (0U)
#define RF_PA_VBCAS_HW_LEN                                      (3U)
#define RF_PA_VBCAS_HW_MSK                                      (((1U<<RF_PA_VBCAS_HW_LEN)-1)<<RF_PA_VBCAS_HW_POS)
#define RF_PA_VBCAS_HW_UMSK                                     (~(((1U<<RF_PA_VBCAS_HW_LEN)-1)<<RF_PA_VBCAS_HW_POS))
#define RF_PA_IAQ_HW                                            RF_PA_IAQ_HW
#define RF_PA_IAQ_HW_POS                                        (4U)
#define RF_PA_IAQ_HW_LEN                                        (3U)
#define RF_PA_IAQ_HW_MSK                                        (((1U<<RF_PA_IAQ_HW_LEN)-1)<<RF_PA_IAQ_HW_POS)
#define RF_PA_IAQ_HW_UMSK                                       (~(((1U<<RF_PA_IAQ_HW_LEN)-1)<<RF_PA_IAQ_HW_POS))
#define RF_PA_IET_HW                                            RF_PA_IET_HW
#define RF_PA_IET_HW_POS                                        (8U)
#define RF_PA_IET_HW_LEN                                        (4U)
#define RF_PA_IET_HW_MSK                                        (((1U<<RF_PA_IET_HW_LEN)-1)<<RF_PA_IET_HW_POS)
#define RF_PA_IET_HW_UMSK                                       (~(((1U<<RF_PA_IET_HW_LEN)-1)<<RF_PA_IET_HW_POS))
#define RF_PA_VBPMOS_IET_HW                                     RF_PA_VBPMOS_IET_HW
#define RF_PA_VBPMOS_IET_HW_POS                                 (12U)
#define RF_PA_VBPMOS_IET_HW_LEN                                 (2U)
#define RF_PA_VBPMOS_IET_HW_MSK                                 (((1U<<RF_PA_VBPMOS_IET_HW_LEN)-1)<<RF_PA_VBPMOS_IET_HW_POS)
#define RF_PA_VBPMOS_IET_HW_UMSK                                (~(((1U<<RF_PA_VBPMOS_IET_HW_LEN)-1)<<RF_PA_VBPMOS_IET_HW_POS))
#define RF_PA_VBPMOS_HW                                         RF_PA_VBPMOS_HW
#define RF_PA_VBPMOS_HW_POS                                     (16U)
#define RF_PA_VBPMOS_HW_LEN                                     (4U)
#define RF_PA_VBPMOS_HW_MSK                                     (((1U<<RF_PA_VBPMOS_HW_LEN)-1)<<RF_PA_VBPMOS_HW_POS)
#define RF_PA_VBPMOS_HW_UMSK                                    (~(((1U<<RF_PA_VBPMOS_HW_LEN)-1)<<RF_PA_VBPMOS_HW_POS))
#define RF_PA_VBCORE_HW                                         RF_PA_VBCORE_HW
#define RF_PA_VBCORE_HW_POS                                     (20U)
#define RF_PA_VBCORE_HW_LEN                                     (4U)
#define RF_PA_VBCORE_HW_MSK                                     (((1U<<RF_PA_VBCORE_HW_LEN)-1)<<RF_PA_VBCORE_HW_POS)
#define RF_PA_VBCORE_HW_UMSK                                    (~(((1U<<RF_PA_VBCORE_HW_LEN)-1)<<RF_PA_VBCORE_HW_POS))
#define RF_PA_ETB_EN_HW                                         RF_PA_ETB_EN_HW
#define RF_PA_ETB_EN_HW_POS                                     (24U)
#define RF_PA_ETB_EN_HW_LEN                                     (1U)
#define RF_PA_ETB_EN_HW_MSK                                     (((1U<<RF_PA_ETB_EN_HW_LEN)-1)<<RF_PA_ETB_EN_HW_POS)
#define RF_PA_ETB_EN_HW_UMSK                                    (~(((1U<<RF_PA_ETB_EN_HW_LEN)-1)<<RF_PA_ETB_EN_HW_POS))
#define RF_PA_LP_EN_HW                                          RF_PA_LP_EN_HW
#define RF_PA_LP_EN_HW_POS                                      (25U)
#define RF_PA_LP_EN_HW_LEN                                      (1U)
#define RF_PA_LP_EN_HW_MSK                                      (((1U<<RF_PA_LP_EN_HW_LEN)-1)<<RF_PA_LP_EN_HW_POS)
#define RF_PA_LP_EN_HW_UMSK                                     (~(((1U<<RF_PA_LP_EN_HW_LEN)-1)<<RF_PA_LP_EN_HW_POS))
#define RF_PA_IB_FIX_HW                                         RF_PA_IB_FIX_HW
#define RF_PA_IB_FIX_HW_POS                                     (28U)
#define RF_PA_IB_FIX_HW_LEN                                     (1U)
#define RF_PA_IB_FIX_HW_MSK                                     (((1U<<RF_PA_IB_FIX_HW_LEN)-1)<<RF_PA_IB_FIX_HW_POS)
#define RF_PA_IB_FIX_HW_UMSK                                    (~(((1U<<RF_PA_IB_FIX_HW_LEN)-1)<<RF_PA_IB_FIX_HW_POS))
#define RF_GC_PA_HW                                             RF_GC_PA_HW
#define RF_GC_PA_HW_POS                                         (29U)
#define RF_GC_PA_HW_LEN                                         (3U)
#define RF_GC_PA_HW_MSK                                         (((1U<<RF_GC_PA_HW_LEN)-1)<<RF_GC_PA_HW_POS)
#define RF_GC_PA_HW_UMSK                                        (~(((1U<<RF_GC_PA_HW_LEN)-1)<<RF_GC_PA_HW_POS))

/* 0x70 : tmx */
#define RF_TMX_OFFSET                                           (0x70)
#define RF_TMX_CS                                               RF_TMX_CS
#define RF_TMX_CS_POS                                           (0U)
#define RF_TMX_CS_LEN                                           (4U)
#define RF_TMX_CS_MSK                                           (((1U<<RF_TMX_CS_LEN)-1)<<RF_TMX_CS_POS)
#define RF_TMX_CS_UMSK                                          (~(((1U<<RF_TMX_CS_LEN)-1)<<RF_TMX_CS_POS))
#define RF_TMX_BM_SW                                            RF_TMX_BM_SW
#define RF_TMX_BM_SW_POS                                        (4U)
#define RF_TMX_BM_SW_LEN                                        (3U)
#define RF_TMX_BM_SW_MSK                                        (((1U<<RF_TMX_BM_SW_LEN)-1)<<RF_TMX_BM_SW_POS)
#define RF_TMX_BM_SW_UMSK                                       (~(((1U<<RF_TMX_BM_SW_LEN)-1)<<RF_TMX_BM_SW_POS))
#define RF_TMX_BM_CAS                                           RF_TMX_BM_CAS
#define RF_TMX_BM_CAS_POS                                       (8U)
#define RF_TMX_BM_CAS_LEN                                       (3U)
#define RF_TMX_BM_CAS_MSK                                       (((1U<<RF_TMX_BM_CAS_LEN)-1)<<RF_TMX_BM_CAS_POS)
#define RF_TMX_BM_CAS_UMSK                                      (~(((1U<<RF_TMX_BM_CAS_LEN)-1)<<RF_TMX_BM_CAS_POS))
#define RF_TMX_BM_CAS_BULK                                      RF_TMX_BM_CAS_BULK
#define RF_TMX_BM_CAS_BULK_POS                                  (12U)
#define RF_TMX_BM_CAS_BULK_LEN                                  (3U)
#define RF_TMX_BM_CAS_BULK_MSK                                  (((1U<<RF_TMX_BM_CAS_BULK_LEN)-1)<<RF_TMX_BM_CAS_BULK_POS)
#define RF_TMX_BM_CAS_BULK_UMSK                                 (~(((1U<<RF_TMX_BM_CAS_BULK_LEN)-1)<<RF_TMX_BM_CAS_BULK_POS))
#define RF_TX_TSENSE_EN                                         RF_TX_TSENSE_EN
#define RF_TX_TSENSE_EN_POS                                     (16U)
#define RF_TX_TSENSE_EN_LEN                                     (1U)
#define RF_TX_TSENSE_EN_MSK                                     (((1U<<RF_TX_TSENSE_EN_LEN)-1)<<RF_TX_TSENSE_EN_POS)
#define RF_TX_TSENSE_EN_UMSK                                    (~(((1U<<RF_TX_TSENSE_EN_LEN)-1)<<RF_TX_TSENSE_EN_POS))
#define RF_TMX_R_ATT                                            RF_TMX_R_ATT
#define RF_TMX_R_ATT_POS                                        (20U)
#define RF_TMX_R_ATT_LEN                                        (4U)
#define RF_TMX_R_ATT_MSK                                        (((1U<<RF_TMX_R_ATT_LEN)-1)<<RF_TMX_R_ATT_POS)
#define RF_TMX_R_ATT_UMSK                                       (~(((1U<<RF_TMX_R_ATT_LEN)-1)<<RF_TMX_R_ATT_POS))

/* 0x74 : tbb */
#define RF_TBB_OFFSET                                           (0x74)
#define RF_TBB_BM_SF                                            RF_TBB_BM_SF
#define RF_TBB_BM_SF_POS                                        (0U)
#define RF_TBB_BM_SF_LEN                                        (2U)
#define RF_TBB_BM_SF_MSK                                        (((1U<<RF_TBB_BM_SF_LEN)-1)<<RF_TBB_BM_SF_POS)
#define RF_TBB_BM_SF_UMSK                                       (~(((1U<<RF_TBB_BM_SF_LEN)-1)<<RF_TBB_BM_SF_POS))
#define RF_TBB_BM_CG                                            RF_TBB_BM_CG
#define RF_TBB_BM_CG_POS                                        (4U)
#define RF_TBB_BM_CG_LEN                                        (2U)
#define RF_TBB_BM_CG_MSK                                        (((1U<<RF_TBB_BM_CG_LEN)-1)<<RF_TBB_BM_CG_POS)
#define RF_TBB_BM_CG_UMSK                                       (~(((1U<<RF_TBB_BM_CG_LEN)-1)<<RF_TBB_BM_CG_POS))
#define RF_TBB_VCM                                              RF_TBB_VCM
#define RF_TBB_VCM_POS                                          (8U)
#define RF_TBB_VCM_LEN                                          (2U)
#define RF_TBB_VCM_MSK                                          (((1U<<RF_TBB_VCM_LEN)-1)<<RF_TBB_VCM_POS)
#define RF_TBB_VCM_UMSK                                         (~(((1U<<RF_TBB_VCM_LEN)-1)<<RF_TBB_VCM_POS))
#define RF_TBB_CFLT                                             RF_TBB_CFLT
#define RF_TBB_CFLT_POS                                         (12U)
#define RF_TBB_CFLT_LEN                                         (2U)
#define RF_TBB_CFLT_MSK                                         (((1U<<RF_TBB_CFLT_LEN)-1)<<RF_TBB_CFLT_POS)
#define RF_TBB_CFLT_UMSK                                        (~(((1U<<RF_TBB_CFLT_LEN)-1)<<RF_TBB_CFLT_POS))
#define RF_TBB_IQ_BIAS_SHORT                                    RF_TBB_IQ_BIAS_SHORT
#define RF_TBB_IQ_BIAS_SHORT_POS                                (14U)
#define RF_TBB_IQ_BIAS_SHORT_LEN                                (1U)
#define RF_TBB_IQ_BIAS_SHORT_MSK                                (((1U<<RF_TBB_IQ_BIAS_SHORT_LEN)-1)<<RF_TBB_IQ_BIAS_SHORT_POS)
#define RF_TBB_IQ_BIAS_SHORT_UMSK                               (~(((1U<<RF_TBB_IQ_BIAS_SHORT_LEN)-1)<<RF_TBB_IQ_BIAS_SHORT_POS))
#define RF_TBB_ATEST_OUT_EN                                     RF_TBB_ATEST_OUT_EN
#define RF_TBB_ATEST_OUT_EN_POS                                 (15U)
#define RF_TBB_ATEST_OUT_EN_LEN                                 (1U)
#define RF_TBB_ATEST_OUT_EN_MSK                                 (((1U<<RF_TBB_ATEST_OUT_EN_LEN)-1)<<RF_TBB_ATEST_OUT_EN_POS)
#define RF_TBB_ATEST_OUT_EN_UMSK                                (~(((1U<<RF_TBB_ATEST_OUT_EN_LEN)-1)<<RF_TBB_ATEST_OUT_EN_POS))
#define RF_TBB_TOSDAC_Q                                         RF_TBB_TOSDAC_Q
#define RF_TBB_TOSDAC_Q_POS                                     (16U)
#define RF_TBB_TOSDAC_Q_LEN                                     (6U)
#define RF_TBB_TOSDAC_Q_MSK                                     (((1U<<RF_TBB_TOSDAC_Q_LEN)-1)<<RF_TBB_TOSDAC_Q_POS)
#define RF_TBB_TOSDAC_Q_UMSK                                    (~(((1U<<RF_TBB_TOSDAC_Q_LEN)-1)<<RF_TBB_TOSDAC_Q_POS))
#define RF_TBB_TOSDAC_I                                         RF_TBB_TOSDAC_I
#define RF_TBB_TOSDAC_I_POS                                     (24U)
#define RF_TBB_TOSDAC_I_LEN                                     (6U)
#define RF_TBB_TOSDAC_I_MSK                                     (((1U<<RF_TBB_TOSDAC_I_LEN)-1)<<RF_TBB_TOSDAC_I_POS)
#define RF_TBB_TOSDAC_I_UMSK                                    (~(((1U<<RF_TBB_TOSDAC_I_LEN)-1)<<RF_TBB_TOSDAC_I_POS))

/* 0x78 : lna */
#define RF_LNA_OFFSET                                           (0x78)
#define RF_LNA_BM                                               RF_LNA_BM
#define RF_LNA_BM_POS                                           (0U)
#define RF_LNA_BM_LEN                                           (4U)
#define RF_LNA_BM_MSK                                           (((1U<<RF_LNA_BM_LEN)-1)<<RF_LNA_BM_POS)
#define RF_LNA_BM_UMSK                                          (~(((1U<<RF_LNA_BM_LEN)-1)<<RF_LNA_BM_POS))
#define RF_LNA_BM_HW                                            RF_LNA_BM_HW
#define RF_LNA_BM_HW_POS                                        (4U)
#define RF_LNA_BM_HW_LEN                                        (4U)
#define RF_LNA_BM_HW_MSK                                        (((1U<<RF_LNA_BM_HW_LEN)-1)<<RF_LNA_BM_HW_POS)
#define RF_LNA_BM_HW_UMSK                                       (~(((1U<<RF_LNA_BM_HW_LEN)-1)<<RF_LNA_BM_HW_POS))
#define RF_LNA_LOAD_CSW                                         RF_LNA_LOAD_CSW
#define RF_LNA_LOAD_CSW_POS                                     (8U)
#define RF_LNA_LOAD_CSW_LEN                                     (4U)
#define RF_LNA_LOAD_CSW_MSK                                     (((1U<<RF_LNA_LOAD_CSW_LEN)-1)<<RF_LNA_LOAD_CSW_POS)
#define RF_LNA_LOAD_CSW_UMSK                                    (~(((1U<<RF_LNA_LOAD_CSW_LEN)-1)<<RF_LNA_LOAD_CSW_POS))
#define RF_LNA_LOAD_CSW_HW                                      RF_LNA_LOAD_CSW_HW
#define RF_LNA_LOAD_CSW_HW_POS                                  (12U)
#define RF_LNA_LOAD_CSW_HW_LEN                                  (4U)
#define RF_LNA_LOAD_CSW_HW_MSK                                  (((1U<<RF_LNA_LOAD_CSW_HW_LEN)-1)<<RF_LNA_LOAD_CSW_HW_POS)
#define RF_LNA_LOAD_CSW_HW_UMSK                                 (~(((1U<<RF_LNA_LOAD_CSW_HW_LEN)-1)<<RF_LNA_LOAD_CSW_HW_POS))
#define RF_LNA_RFB_MATCH                                        RF_LNA_RFB_MATCH
#define RF_LNA_RFB_MATCH_POS                                    (16U)
#define RF_LNA_RFB_MATCH_LEN                                    (3U)
#define RF_LNA_RFB_MATCH_MSK                                    (((1U<<RF_LNA_RFB_MATCH_LEN)-1)<<RF_LNA_RFB_MATCH_POS)
#define RF_LNA_RFB_MATCH_UMSK                                   (~(((1U<<RF_LNA_RFB_MATCH_LEN)-1)<<RF_LNA_RFB_MATCH_POS))
#define RF_LNA_CAP_LG                                           RF_LNA_CAP_LG
#define RF_LNA_CAP_LG_POS                                       (20U)
#define RF_LNA_CAP_LG_LEN                                       (2U)
#define RF_LNA_CAP_LG_MSK                                       (((1U<<RF_LNA_CAP_LG_LEN)-1)<<RF_LNA_CAP_LG_POS)
#define RF_LNA_CAP_LG_UMSK                                      (~(((1U<<RF_LNA_CAP_LG_LEN)-1)<<RF_LNA_CAP_LG_POS))
#define RF_LNA_LG_GSEL                                          RF_LNA_LG_GSEL
#define RF_LNA_LG_GSEL_POS                                      (24U)
#define RF_LNA_LG_GSEL_LEN                                      (3U)
#define RF_LNA_LG_GSEL_MSK                                      (((1U<<RF_LNA_LG_GSEL_LEN)-1)<<RF_LNA_LG_GSEL_POS)
#define RF_LNA_LG_GSEL_UMSK                                     (~(((1U<<RF_LNA_LG_GSEL_LEN)-1)<<RF_LNA_LG_GSEL_POS))
#define RF_LNA_CMATCH_PREIND                                    RF_LNA_CMATCH_PREIND
#define RF_LNA_CMATCH_PREIND_POS                                (28U)
#define RF_LNA_CMATCH_PREIND_LEN                                (2U)
#define RF_LNA_CMATCH_PREIND_MSK                                (((1U<<RF_LNA_CMATCH_PREIND_LEN)-1)<<RF_LNA_CMATCH_PREIND_POS)
#define RF_LNA_CMATCH_PREIND_UMSK                               (~(((1U<<RF_LNA_CMATCH_PREIND_LEN)-1)<<RF_LNA_CMATCH_PREIND_POS))

/* 0x7C : rmxgm */
#define RF_RMXGM_OFFSET                                         (0x7C)
#define RF_RMX_BM                                               RF_RMX_BM
#define RF_RMX_BM_POS                                           (0U)
#define RF_RMX_BM_LEN                                           (3U)
#define RF_RMX_BM_MSK                                           (((1U<<RF_RMX_BM_LEN)-1)<<RF_RMX_BM_POS)
#define RF_RMX_BM_UMSK                                          (~(((1U<<RF_RMX_BM_LEN)-1)<<RF_RMX_BM_POS))
#define RF_RMXGM_BM                                             RF_RMXGM_BM
#define RF_RMXGM_BM_POS                                         (4U)
#define RF_RMXGM_BM_LEN                                         (3U)
#define RF_RMXGM_BM_MSK                                         (((1U<<RF_RMXGM_BM_LEN)-1)<<RF_RMXGM_BM_POS)
#define RF_RMXGM_BM_UMSK                                        (~(((1U<<RF_RMXGM_BM_LEN)-1)<<RF_RMXGM_BM_POS))
#define RF_RMXGM_10M_MODE_EN                                    RF_RMXGM_10M_MODE_EN
#define RF_RMXGM_10M_MODE_EN_POS                                (8U)
#define RF_RMXGM_10M_MODE_EN_LEN                                (1U)
#define RF_RMXGM_10M_MODE_EN_MSK                                (((1U<<RF_RMXGM_10M_MODE_EN_LEN)-1)<<RF_RMXGM_10M_MODE_EN_POS)
#define RF_RMXGM_10M_MODE_EN_UMSK                               (~(((1U<<RF_RMXGM_10M_MODE_EN_LEN)-1)<<RF_RMXGM_10M_MODE_EN_POS))
#define RF_PU_RRF_AVDD15                                        RF_PU_RRF_AVDD15
#define RF_PU_RRF_AVDD15_POS                                    (12U)
#define RF_PU_RRF_AVDD15_LEN                                    (1U)
#define RF_PU_RRF_AVDD15_MSK                                    (((1U<<RF_PU_RRF_AVDD15_LEN)-1)<<RF_PU_RRF_AVDD15_POS)
#define RF_PU_RRF_AVDD15_UMSK                                   (~(((1U<<RF_PU_RRF_AVDD15_LEN)-1)<<RF_PU_RRF_AVDD15_POS))
#define RF_PU_RRFLDO                                            RF_PU_RRFLDO
#define RF_PU_RRFLDO_POS                                        (13U)
#define RF_PU_RRFLDO_LEN                                        (1U)
#define RF_PU_RRFLDO_MSK                                        (((1U<<RF_PU_RRFLDO_LEN)-1)<<RF_PU_RRFLDO_POS)
#define RF_PU_RRFLDO_UMSK                                       (~(((1U<<RF_PU_RRFLDO_LEN)-1)<<RF_PU_RRFLDO_POS))
#define RF_RRFLDO_BYPASS                                        RF_RRFLDO_BYPASS
#define RF_RRFLDO_BYPASS_POS                                    (14U)
#define RF_RRFLDO_BYPASS_LEN                                    (1U)
#define RF_RRFLDO_BYPASS_MSK                                    (((1U<<RF_RRFLDO_BYPASS_LEN)-1)<<RF_RRFLDO_BYPASS_POS)
#define RF_RRFLDO_BYPASS_UMSK                                   (~(((1U<<RF_RRFLDO_BYPASS_LEN)-1)<<RF_RRFLDO_BYPASS_POS))
#define RF_RRFLDO_PULLDOWN_EN                                   RF_RRFLDO_PULLDOWN_EN
#define RF_RRFLDO_PULLDOWN_EN_POS                               (15U)
#define RF_RRFLDO_PULLDOWN_EN_LEN                               (1U)
#define RF_RRFLDO_PULLDOWN_EN_MSK                               (((1U<<RF_RRFLDO_PULLDOWN_EN_LEN)-1)<<RF_RRFLDO_PULLDOWN_EN_POS)
#define RF_RRFLDO_PULLDOWN_EN_UMSK                              (~(((1U<<RF_RRFLDO_PULLDOWN_EN_LEN)-1)<<RF_RRFLDO_PULLDOWN_EN_POS))
#define RF_RRFLDO_VOUT_SEL                                      RF_RRFLDO_VOUT_SEL
#define RF_RRFLDO_VOUT_SEL_POS                                  (16U)
#define RF_RRFLDO_VOUT_SEL_LEN                                  (3U)
#define RF_RRFLDO_VOUT_SEL_MSK                                  (((1U<<RF_RRFLDO_VOUT_SEL_LEN)-1)<<RF_RRFLDO_VOUT_SEL_POS)
#define RF_RRFLDO_VOUT_SEL_UMSK                                 (~(((1U<<RF_RRFLDO_VOUT_SEL_LEN)-1)<<RF_RRFLDO_VOUT_SEL_POS))

/* 0x80 : rbb1 */
#define RF_RBB1_OFFSET                                          (0x80)
#define RF_ROSDAC_Q                                             RF_ROSDAC_Q
#define RF_ROSDAC_Q_POS                                         (0U)
#define RF_ROSDAC_Q_LEN                                         (6U)
#define RF_ROSDAC_Q_MSK                                         (((1U<<RF_ROSDAC_Q_LEN)-1)<<RF_ROSDAC_Q_POS)
#define RF_ROSDAC_Q_UMSK                                        (~(((1U<<RF_ROSDAC_Q_LEN)-1)<<RF_ROSDAC_Q_POS))
#define RF_ROSDAC_I                                             RF_ROSDAC_I
#define RF_ROSDAC_I_POS                                         (8U)
#define RF_ROSDAC_I_LEN                                         (6U)
#define RF_ROSDAC_I_MSK                                         (((1U<<RF_ROSDAC_I_LEN)-1)<<RF_ROSDAC_I_POS)
#define RF_ROSDAC_I_UMSK                                        (~(((1U<<RF_ROSDAC_I_LEN)-1)<<RF_ROSDAC_I_POS))
#define RF_ROSDAC_Q_HW                                          RF_ROSDAC_Q_HW
#define RF_ROSDAC_Q_HW_POS                                      (16U)
#define RF_ROSDAC_Q_HW_LEN                                      (6U)
#define RF_ROSDAC_Q_HW_MSK                                      (((1U<<RF_ROSDAC_Q_HW_LEN)-1)<<RF_ROSDAC_Q_HW_POS)
#define RF_ROSDAC_Q_HW_UMSK                                     (~(((1U<<RF_ROSDAC_Q_HW_LEN)-1)<<RF_ROSDAC_Q_HW_POS))
#define RF_ROSDAC_I_HW                                          RF_ROSDAC_I_HW
#define RF_ROSDAC_I_HW_POS                                      (24U)
#define RF_ROSDAC_I_HW_LEN                                      (6U)
#define RF_ROSDAC_I_HW_MSK                                      (((1U<<RF_ROSDAC_I_HW_LEN)-1)<<RF_ROSDAC_I_HW_POS)
#define RF_ROSDAC_I_HW_UMSK                                     (~(((1U<<RF_ROSDAC_I_HW_LEN)-1)<<RF_ROSDAC_I_HW_POS))
#define RF_RBB_MFB_R2_BYPASS                                    RF_RBB_MFB_R2_BYPASS
#define RF_RBB_MFB_R2_BYPASS_POS                                (30U)
#define RF_RBB_MFB_R2_BYPASS_LEN                                (1U)
#define RF_RBB_MFB_R2_BYPASS_MSK                                (((1U<<RF_RBB_MFB_R2_BYPASS_LEN)-1)<<RF_RBB_MFB_R2_BYPASS_POS)
#define RF_RBB_MFB_R2_BYPASS_UMSK                               (~(((1U<<RF_RBB_MFB_R2_BYPASS_LEN)-1)<<RF_RBB_MFB_R2_BYPASS_POS))
#define RF_ROSDAC_RANGE                                         RF_ROSDAC_RANGE
#define RF_ROSDAC_RANGE_POS                                     (31U)
#define RF_ROSDAC_RANGE_LEN                                     (1U)
#define RF_ROSDAC_RANGE_MSK                                     (((1U<<RF_ROSDAC_RANGE_LEN)-1)<<RF_ROSDAC_RANGE_POS)
#define RF_ROSDAC_RANGE_UMSK                                    (~(((1U<<RF_ROSDAC_RANGE_LEN)-1)<<RF_ROSDAC_RANGE_POS))

/* 0x84 : rbb2 */
#define RF_RBB2_OFFSET                                          (0x84)
#define RF_RBB_CAP2_FC_Q                                        RF_RBB_CAP2_FC_Q
#define RF_RBB_CAP2_FC_Q_POS                                    (0U)
#define RF_RBB_CAP2_FC_Q_LEN                                    (6U)
#define RF_RBB_CAP2_FC_Q_MSK                                    (((1U<<RF_RBB_CAP2_FC_Q_LEN)-1)<<RF_RBB_CAP2_FC_Q_POS)
#define RF_RBB_CAP2_FC_Q_UMSK                                   (~(((1U<<RF_RBB_CAP2_FC_Q_LEN)-1)<<RF_RBB_CAP2_FC_Q_POS))
#define RF_RBB_CAP2_FC_I                                        RF_RBB_CAP2_FC_I
#define RF_RBB_CAP2_FC_I_POS                                    (8U)
#define RF_RBB_CAP2_FC_I_LEN                                    (6U)
#define RF_RBB_CAP2_FC_I_MSK                                    (((1U<<RF_RBB_CAP2_FC_I_LEN)-1)<<RF_RBB_CAP2_FC_I_POS)
#define RF_RBB_CAP2_FC_I_UMSK                                   (~(((1U<<RF_RBB_CAP2_FC_I_LEN)-1)<<RF_RBB_CAP2_FC_I_POS))
#define RF_RBB_CAP1_FC_Q                                        RF_RBB_CAP1_FC_Q
#define RF_RBB_CAP1_FC_Q_POS                                    (16U)
#define RF_RBB_CAP1_FC_Q_LEN                                    (6U)
#define RF_RBB_CAP1_FC_Q_MSK                                    (((1U<<RF_RBB_CAP1_FC_Q_LEN)-1)<<RF_RBB_CAP1_FC_Q_POS)
#define RF_RBB_CAP1_FC_Q_UMSK                                   (~(((1U<<RF_RBB_CAP1_FC_Q_LEN)-1)<<RF_RBB_CAP1_FC_Q_POS))
#define RF_RBB_CAP1_FC_I                                        RF_RBB_CAP1_FC_I
#define RF_RBB_CAP1_FC_I_POS                                    (24U)
#define RF_RBB_CAP1_FC_I_LEN                                    (6U)
#define RF_RBB_CAP1_FC_I_MSK                                    (((1U<<RF_RBB_CAP1_FC_I_LEN)-1)<<RF_RBB_CAP1_FC_I_POS)
#define RF_RBB_CAP1_FC_I_UMSK                                   (~(((1U<<RF_RBB_CAP1_FC_I_LEN)-1)<<RF_RBB_CAP1_FC_I_POS))

/* 0x88 : rbb3 */
#define RF_RBB3_OFFSET                                          (0x88)
#define RF_RBB_MFB_DEQ                                          RF_RBB_MFB_DEQ
#define RF_RBB_MFB_DEQ_POS                                      (0U)
#define RF_RBB_MFB_DEQ_LEN                                      (4U)
#define RF_RBB_MFB_DEQ_MSK                                      (((1U<<RF_RBB_MFB_DEQ_LEN)-1)<<RF_RBB_MFB_DEQ_POS)
#define RF_RBB_MFB_DEQ_UMSK                                     (~(((1U<<RF_RBB_MFB_DEQ_LEN)-1)<<RF_RBB_MFB_DEQ_POS))
#define RF_RBB_STARTUP_SEL                                      RF_RBB_STARTUP_SEL
#define RF_RBB_STARTUP_SEL_POS                                  (4U)
#define RF_RBB_STARTUP_SEL_LEN                                  (1U)
#define RF_RBB_STARTUP_SEL_MSK                                  (((1U<<RF_RBB_STARTUP_SEL_LEN)-1)<<RF_RBB_STARTUP_SEL_POS)
#define RF_RBB_STARTUP_SEL_UMSK                                 (~(((1U<<RF_RBB_STARTUP_SEL_LEN)-1)<<RF_RBB_STARTUP_SEL_POS))
#define RF_RBB_BT_FIF_TUNE                                      RF_RBB_BT_FIF_TUNE
#define RF_RBB_BT_FIF_TUNE_POS                                  (5U)
#define RF_RBB_BT_FIF_TUNE_LEN                                  (2U)
#define RF_RBB_BT_FIF_TUNE_MSK                                  (((1U<<RF_RBB_BT_FIF_TUNE_LEN)-1)<<RF_RBB_BT_FIF_TUNE_POS)
#define RF_RBB_BT_FIF_TUNE_UMSK                                 (~(((1U<<RF_RBB_BT_FIF_TUNE_LEN)-1)<<RF_RBB_BT_FIF_TUNE_POS))
#define RF_RBB_DEQ                                              RF_RBB_DEQ
#define RF_RBB_DEQ_POS                                          (8U)
#define RF_RBB_DEQ_LEN                                          (2U)
#define RF_RBB_DEQ_MSK                                          (((1U<<RF_RBB_DEQ_LEN)-1)<<RF_RBB_DEQ_POS)
#define RF_RBB_DEQ_UMSK                                         (~(((1U<<RF_RBB_DEQ_LEN)-1)<<RF_RBB_DEQ_POS))
#define RF_RBB_BM_OP                                            RF_RBB_BM_OP
#define RF_RBB_BM_OP_POS                                        (12U)
#define RF_RBB_BM_OP_LEN                                        (3U)
#define RF_RBB_BM_OP_MSK                                        (((1U<<RF_RBB_BM_OP_LEN)-1)<<RF_RBB_BM_OP_POS)
#define RF_RBB_BM_OP_UMSK                                       (~(((1U<<RF_RBB_BM_OP_LEN)-1)<<RF_RBB_BM_OP_POS))
#define RF_RBB_VCM                                              RF_RBB_VCM
#define RF_RBB_VCM_POS                                          (16U)
#define RF_RBB_VCM_LEN                                          (2U)
#define RF_RBB_VCM_MSK                                          (((1U<<RF_RBB_VCM_LEN)-1)<<RF_RBB_VCM_POS)
#define RF_RBB_VCM_UMSK                                         (~(((1U<<RF_RBB_VCM_LEN)-1)<<RF_RBB_VCM_POS))
#define RF_RBB_BQ_IQBIAS_SHORT                                  RF_RBB_BQ_IQBIAS_SHORT
#define RF_RBB_BQ_IQBIAS_SHORT_POS                              (20U)
#define RF_RBB_BQ_IQBIAS_SHORT_LEN                              (1U)
#define RF_RBB_BQ_IQBIAS_SHORT_MSK                              (((1U<<RF_RBB_BQ_IQBIAS_SHORT_LEN)-1)<<RF_RBB_BQ_IQBIAS_SHORT_POS)
#define RF_RBB_BQ_IQBIAS_SHORT_UMSK                             (~(((1U<<RF_RBB_BQ_IQBIAS_SHORT_LEN)-1)<<RF_RBB_BQ_IQBIAS_SHORT_POS))
#define RF_RBB_TIA_IQBIAS_SHORT                                 RF_RBB_TIA_IQBIAS_SHORT
#define RF_RBB_TIA_IQBIAS_SHORT_POS                             (21U)
#define RF_RBB_TIA_IQBIAS_SHORT_LEN                             (1U)
#define RF_RBB_TIA_IQBIAS_SHORT_MSK                             (((1U<<RF_RBB_TIA_IQBIAS_SHORT_LEN)-1)<<RF_RBB_TIA_IQBIAS_SHORT_POS)
#define RF_RBB_TIA_IQBIAS_SHORT_UMSK                            (~(((1U<<RF_RBB_TIA_IQBIAS_SHORT_LEN)-1)<<RF_RBB_TIA_IQBIAS_SHORT_POS))
#define RF_RBB_BW                                               RF_RBB_BW
#define RF_RBB_BW_POS                                           (24U)
#define RF_RBB_BW_LEN                                           (2U)
#define RF_RBB_BW_MSK                                           (((1U<<RF_RBB_BW_LEN)-1)<<RF_RBB_BW_POS)
#define RF_RBB_BW_UMSK                                          (~(((1U<<RF_RBB_BW_LEN)-1)<<RF_RBB_BW_POS))
#define RF_RXIQCAL_EN                                           RF_RXIQCAL_EN
#define RF_RXIQCAL_EN_POS                                       (28U)
#define RF_RXIQCAL_EN_LEN                                       (1U)
#define RF_RXIQCAL_EN_MSK                                       (((1U<<RF_RXIQCAL_EN_LEN)-1)<<RF_RXIQCAL_EN_POS)
#define RF_RXIQCAL_EN_UMSK                                      (~(((1U<<RF_RXIQCAL_EN_LEN)-1)<<RF_RXIQCAL_EN_POS))
#define RF_PWR_DET_EN                                           RF_PWR_DET_EN
#define RF_PWR_DET_EN_POS                                       (31U)
#define RF_PWR_DET_EN_LEN                                       (1U)
#define RF_PWR_DET_EN_MSK                                       (((1U<<RF_PWR_DET_EN_LEN)-1)<<RF_PWR_DET_EN_POS)
#define RF_PWR_DET_EN_UMSK                                      (~(((1U<<RF_PWR_DET_EN_LEN)-1)<<RF_PWR_DET_EN_POS))

/* 0x8C : rbb4 */
#define RF_RBB4_OFFSET                                          (0x8C)
#define RF_RBB_PKDET_VTH                                        RF_RBB_PKDET_VTH
#define RF_RBB_PKDET_VTH_POS                                    (0U)
#define RF_RBB_PKDET_VTH_LEN                                    (4U)
#define RF_RBB_PKDET_VTH_MSK                                    (((1U<<RF_RBB_PKDET_VTH_LEN)-1)<<RF_RBB_PKDET_VTH_POS)
#define RF_RBB_PKDET_VTH_UMSK                                   (~(((1U<<RF_RBB_PKDET_VTH_LEN)-1)<<RF_RBB_PKDET_VTH_POS))
#define RF_RBB_PKDET_OUT_RSTN                                   RF_RBB_PKDET_OUT_RSTN
#define RF_RBB_PKDET_OUT_RSTN_POS                               (4U)
#define RF_RBB_PKDET_OUT_RSTN_LEN                               (1U)
#define RF_RBB_PKDET_OUT_RSTN_MSK                               (((1U<<RF_RBB_PKDET_OUT_RSTN_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_POS)
#define RF_RBB_PKDET_OUT_RSTN_UMSK                              (~(((1U<<RF_RBB_PKDET_OUT_RSTN_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_POS))
#define RF_RBB_PKDET_EN                                         RF_RBB_PKDET_EN
#define RF_RBB_PKDET_EN_POS                                     (5U)
#define RF_RBB_PKDET_EN_LEN                                     (1U)
#define RF_RBB_PKDET_EN_MSK                                     (((1U<<RF_RBB_PKDET_EN_LEN)-1)<<RF_RBB_PKDET_EN_POS)
#define RF_RBB_PKDET_EN_UMSK                                    (~(((1U<<RF_RBB_PKDET_EN_LEN)-1)<<RF_RBB_PKDET_EN_POS))
#define RF_RBB_MFB_C1_FC                                        RF_RBB_MFB_C1_FC
#define RF_RBB_MFB_C1_FC_POS                                    (6U)
#define RF_RBB_MFB_C1_FC_LEN                                    (6U)
#define RF_RBB_MFB_C1_FC_MSK                                    (((1U<<RF_RBB_MFB_C1_FC_LEN)-1)<<RF_RBB_MFB_C1_FC_POS)
#define RF_RBB_MFB_C1_FC_UMSK                                   (~(((1U<<RF_RBB_MFB_C1_FC_LEN)-1)<<RF_RBB_MFB_C1_FC_POS))
#define RF_RBB_PKDET_OUT_RSTN_HW                                RF_RBB_PKDET_OUT_RSTN_HW
#define RF_RBB_PKDET_OUT_RSTN_HW_POS                            (12U)
#define RF_RBB_PKDET_OUT_RSTN_HW_LEN                            (1U)
#define RF_RBB_PKDET_OUT_RSTN_HW_MSK                            (((1U<<RF_RBB_PKDET_OUT_RSTN_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_HW_POS)
#define RF_RBB_PKDET_OUT_RSTN_HW_UMSK                           (~(((1U<<RF_RBB_PKDET_OUT_RSTN_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_HW_POS))
#define RF_RBB_PKDET_EN_HW                                      RF_RBB_PKDET_EN_HW
#define RF_RBB_PKDET_EN_HW_POS                                  (16U)
#define RF_RBB_PKDET_EN_HW_LEN                                  (1U)
#define RF_RBB_PKDET_EN_HW_MSK                                  (((1U<<RF_RBB_PKDET_EN_HW_LEN)-1)<<RF_RBB_PKDET_EN_HW_POS)
#define RF_RBB_PKDET_EN_HW_UMSK                                 (~(((1U<<RF_RBB_PKDET_EN_HW_LEN)-1)<<RF_RBB_PKDET_EN_HW_POS))
#define RF_PKDET_OUT_RAW                                        RF_PKDET_OUT_RAW
#define RF_PKDET_OUT_RAW_POS                                    (20U)
#define RF_PKDET_OUT_RAW_LEN                                    (1U)
#define RF_PKDET_OUT_RAW_MSK                                    (((1U<<RF_PKDET_OUT_RAW_LEN)-1)<<RF_PKDET_OUT_RAW_POS)
#define RF_PKDET_OUT_RAW_UMSK                                   (~(((1U<<RF_PKDET_OUT_RAW_LEN)-1)<<RF_PKDET_OUT_RAW_POS))
#define RF_PKDET_OUT_LATCH                                      RF_PKDET_OUT_LATCH
#define RF_PKDET_OUT_LATCH_POS                                  (24U)
#define RF_PKDET_OUT_LATCH_LEN                                  (1U)
#define RF_PKDET_OUT_LATCH_MSK                                  (((1U<<RF_PKDET_OUT_LATCH_LEN)-1)<<RF_PKDET_OUT_LATCH_POS)
#define RF_PKDET_OUT_LATCH_UMSK                                 (~(((1U<<RF_PKDET_OUT_LATCH_LEN)-1)<<RF_PKDET_OUT_LATCH_POS))
#define RF_RBB_BT_MODE_HW                                       RF_RBB_BT_MODE_HW
#define RF_RBB_BT_MODE_HW_POS                                   (25U)
#define RF_RBB_BT_MODE_HW_LEN                                   (1U)
#define RF_RBB_BT_MODE_HW_MSK                                   (((1U<<RF_RBB_BT_MODE_HW_LEN)-1)<<RF_RBB_BT_MODE_HW_POS)
#define RF_RBB_BT_MODE_HW_UMSK                                  (~(((1U<<RF_RBB_BT_MODE_HW_LEN)-1)<<RF_RBB_BT_MODE_HW_POS))
#define RF_RBB_BT_MODE                                          RF_RBB_BT_MODE
#define RF_RBB_BT_MODE_POS                                      (26U)
#define RF_RBB_BT_MODE_LEN                                      (1U)
#define RF_RBB_BT_MODE_MSK                                      (((1U<<RF_RBB_BT_MODE_LEN)-1)<<RF_RBB_BT_MODE_POS)
#define RF_RBB_BT_MODE_UMSK                                     (~(((1U<<RF_RBB_BT_MODE_LEN)-1)<<RF_RBB_BT_MODE_POS))
#define RF_RBB_CAP1_DEQ_I                                       RF_RBB_CAP1_DEQ_I
#define RF_RBB_CAP1_DEQ_I_POS                                   (27U)
#define RF_RBB_CAP1_DEQ_I_LEN                                   (1U)
#define RF_RBB_CAP1_DEQ_I_MSK                                   (((1U<<RF_RBB_CAP1_DEQ_I_LEN)-1)<<RF_RBB_CAP1_DEQ_I_POS)
#define RF_RBB_CAP1_DEQ_I_UMSK                                  (~(((1U<<RF_RBB_CAP1_DEQ_I_LEN)-1)<<RF_RBB_CAP1_DEQ_I_POS))
#define RF_RBB_CAP1_DEQ_Q                                       RF_RBB_CAP1_DEQ_Q
#define RF_RBB_CAP1_DEQ_Q_POS                                   (28U)
#define RF_RBB_CAP1_DEQ_Q_LEN                                   (1U)
#define RF_RBB_CAP1_DEQ_Q_MSK                                   (((1U<<RF_RBB_CAP1_DEQ_Q_LEN)-1)<<RF_RBB_CAP1_DEQ_Q_POS)
#define RF_RBB_CAP1_DEQ_Q_UMSK                                  (~(((1U<<RF_RBB_CAP1_DEQ_Q_LEN)-1)<<RF_RBB_CAP1_DEQ_Q_POS))

/* 0x90 : adda1 */
#define RF_ADDA1_OFFSET                                         (0x90)
#define RF_DAC_DVDD_SEL                                         RF_DAC_DVDD_SEL
#define RF_DAC_DVDD_SEL_POS                                     (0U)
#define RF_DAC_DVDD_SEL_LEN                                     (3U)
#define RF_DAC_DVDD_SEL_MSK                                     (((1U<<RF_DAC_DVDD_SEL_LEN)-1)<<RF_DAC_DVDD_SEL_POS)
#define RF_DAC_DVDD_SEL_UMSK                                    (~(((1U<<RF_DAC_DVDD_SEL_LEN)-1)<<RF_DAC_DVDD_SEL_POS))
#define RF_DAC_BIAS_SEL                                         RF_DAC_BIAS_SEL
#define RF_DAC_BIAS_SEL_POS                                     (4U)
#define RF_DAC_BIAS_SEL_LEN                                     (2U)
#define RF_DAC_BIAS_SEL_MSK                                     (((1U<<RF_DAC_BIAS_SEL_LEN)-1)<<RF_DAC_BIAS_SEL_POS)
#define RF_DAC_BIAS_SEL_UMSK                                    (~(((1U<<RF_DAC_BIAS_SEL_LEN)-1)<<RF_DAC_BIAS_SEL_POS))
#define RF_DAC_CLK_SEL                                          RF_DAC_CLK_SEL
#define RF_DAC_CLK_SEL_POS                                      (8U)
#define RF_DAC_CLK_SEL_LEN                                      (2U)
#define RF_DAC_CLK_SEL_MSK                                      (((1U<<RF_DAC_CLK_SEL_LEN)-1)<<RF_DAC_CLK_SEL_POS)
#define RF_DAC_CLK_SEL_UMSK                                     (~(((1U<<RF_DAC_CLK_SEL_LEN)-1)<<RF_DAC_CLK_SEL_POS))
#define RF_DAC_RCCALSEL                                         RF_DAC_RCCALSEL
#define RF_DAC_RCCALSEL_POS                                     (12U)
#define RF_DAC_RCCALSEL_LEN                                     (1U)
#define RF_DAC_RCCALSEL_MSK                                     (((1U<<RF_DAC_RCCALSEL_LEN)-1)<<RF_DAC_RCCALSEL_POS)
#define RF_DAC_RCCALSEL_UMSK                                    (~(((1U<<RF_DAC_RCCALSEL_LEN)-1)<<RF_DAC_RCCALSEL_POS))
#define RF_DAC_CLK_SYNC_INV                                     RF_DAC_CLK_SYNC_INV
#define RF_DAC_CLK_SYNC_INV_POS                                 (13U)
#define RF_DAC_CLK_SYNC_INV_LEN                                 (1U)
#define RF_DAC_CLK_SYNC_INV_MSK                                 (((1U<<RF_DAC_CLK_SYNC_INV_LEN)-1)<<RF_DAC_CLK_SYNC_INV_POS)
#define RF_DAC_CLK_SYNC_INV_UMSK                                (~(((1U<<RF_DAC_CLK_SYNC_INV_LEN)-1)<<RF_DAC_CLK_SYNC_INV_POS))
#define RF_DAC_RCCAL_IQSWAP                                     RF_DAC_RCCAL_IQSWAP
#define RF_DAC_RCCAL_IQSWAP_POS                                 (14U)
#define RF_DAC_RCCAL_IQSWAP_LEN                                 (1U)
#define RF_DAC_RCCAL_IQSWAP_MSK                                 (((1U<<RF_DAC_RCCAL_IQSWAP_LEN)-1)<<RF_DAC_RCCAL_IQSWAP_POS)
#define RF_DAC_RCCAL_IQSWAP_UMSK                                (~(((1U<<RF_DAC_RCCAL_IQSWAP_LEN)-1)<<RF_DAC_RCCAL_IQSWAP_POS))
#define RF_ADDA_LDO_BYPS                                        RF_ADDA_LDO_BYPS
#define RF_ADDA_LDO_BYPS_POS                                    (16U)
#define RF_ADDA_LDO_BYPS_LEN                                    (1U)
#define RF_ADDA_LDO_BYPS_MSK                                    (((1U<<RF_ADDA_LDO_BYPS_LEN)-1)<<RF_ADDA_LDO_BYPS_POS)
#define RF_ADDA_LDO_BYPS_UMSK                                   (~(((1U<<RF_ADDA_LDO_BYPS_LEN)-1)<<RF_ADDA_LDO_BYPS_POS))
#define RF_ADDA_LDO_DVDD_SEL                                    RF_ADDA_LDO_DVDD_SEL
#define RF_ADDA_LDO_DVDD_SEL_POS                                (20U)
#define RF_ADDA_LDO_DVDD_SEL_LEN                                (3U)
#define RF_ADDA_LDO_DVDD_SEL_MSK                                (((1U<<RF_ADDA_LDO_DVDD_SEL_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_POS)
#define RF_ADDA_LDO_DVDD_SEL_UMSK                               (~(((1U<<RF_ADDA_LDO_DVDD_SEL_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_POS))
#define RF_ADDA_LDO_DVDD_SEL_HW                                 RF_ADDA_LDO_DVDD_SEL_HW
#define RF_ADDA_LDO_DVDD_SEL_HW_POS                             (24U)
#define RF_ADDA_LDO_DVDD_SEL_HW_LEN                             (3U)
#define RF_ADDA_LDO_DVDD_SEL_HW_MSK                             (((1U<<RF_ADDA_LDO_DVDD_SEL_HW_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_HW_POS)
#define RF_ADDA_LDO_DVDD_SEL_HW_UMSK                            (~(((1U<<RF_ADDA_LDO_DVDD_SEL_HW_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_HW_POS))
#define RF_DAC_VLOW_SEL                                         RF_DAC_VLOW_SEL
#define RF_DAC_VLOW_SEL_POS                                     (28U)
#define RF_DAC_VLOW_SEL_LEN                                     (2U)
#define RF_DAC_VLOW_SEL_MSK                                     (((1U<<RF_DAC_VLOW_SEL_LEN)-1)<<RF_DAC_VLOW_SEL_POS)
#define RF_DAC_VLOW_SEL_UMSK                                    (~(((1U<<RF_DAC_VLOW_SEL_LEN)-1)<<RF_DAC_VLOW_SEL_POS))
#define RF_DAC_NSINK_SEL                                        RF_DAC_NSINK_SEL
#define RF_DAC_NSINK_SEL_POS                                    (30U)
#define RF_DAC_NSINK_SEL_LEN                                    (2U)
#define RF_DAC_NSINK_SEL_MSK                                    (((1U<<RF_DAC_NSINK_SEL_LEN)-1)<<RF_DAC_NSINK_SEL_POS)
#define RF_DAC_NSINK_SEL_UMSK                                   (~(((1U<<RF_DAC_NSINK_SEL_LEN)-1)<<RF_DAC_NSINK_SEL_POS))

/* 0x94 : adda2 */
#define RF_ADDA2_OFFSET                                         (0x94)
#define RF_ADC_VREF_SEL                                         RF_ADC_VREF_SEL
#define RF_ADC_VREF_SEL_POS                                     (0U)
#define RF_ADC_VREF_SEL_LEN                                     (2U)
#define RF_ADC_VREF_SEL_MSK                                     (((1U<<RF_ADC_VREF_SEL_LEN)-1)<<RF_ADC_VREF_SEL_POS)
#define RF_ADC_VREF_SEL_UMSK                                    (~(((1U<<RF_ADC_VREF_SEL_LEN)-1)<<RF_ADC_VREF_SEL_POS))
#define RF_ADC_DLY_CTL                                          RF_ADC_DLY_CTL
#define RF_ADC_DLY_CTL_POS                                      (4U)
#define RF_ADC_DLY_CTL_LEN                                      (2U)
#define RF_ADC_DLY_CTL_MSK                                      (((1U<<RF_ADC_DLY_CTL_LEN)-1)<<RF_ADC_DLY_CTL_POS)
#define RF_ADC_DLY_CTL_UMSK                                     (~(((1U<<RF_ADC_DLY_CTL_LEN)-1)<<RF_ADC_DLY_CTL_POS))
#define RF_ADC_DVDD_SEL                                         RF_ADC_DVDD_SEL
#define RF_ADC_DVDD_SEL_POS                                     (8U)
#define RF_ADC_DVDD_SEL_LEN                                     (2U)
#define RF_ADC_DVDD_SEL_MSK                                     (((1U<<RF_ADC_DVDD_SEL_LEN)-1)<<RF_ADC_DVDD_SEL_POS)
#define RF_ADC_DVDD_SEL_UMSK                                    (~(((1U<<RF_ADC_DVDD_SEL_LEN)-1)<<RF_ADC_DVDD_SEL_POS))
#define RF_ADC_SAR_ASCAL_EN                                     RF_ADC_SAR_ASCAL_EN
#define RF_ADC_SAR_ASCAL_EN_POS                                 (12U)
#define RF_ADC_SAR_ASCAL_EN_LEN                                 (1U)
#define RF_ADC_SAR_ASCAL_EN_MSK                                 (((1U<<RF_ADC_SAR_ASCAL_EN_LEN)-1)<<RF_ADC_SAR_ASCAL_EN_POS)
#define RF_ADC_SAR_ASCAL_EN_UMSK                                (~(((1U<<RF_ADC_SAR_ASCAL_EN_LEN)-1)<<RF_ADC_SAR_ASCAL_EN_POS))
#define RF_ADC_GT_RM                                            RF_ADC_GT_RM
#define RF_ADC_GT_RM_POS                                        (16U)
#define RF_ADC_GT_RM_LEN                                        (1U)
#define RF_ADC_GT_RM_MSK                                        (((1U<<RF_ADC_GT_RM_LEN)-1)<<RF_ADC_GT_RM_POS)
#define RF_ADC_GT_RM_UMSK                                       (~(((1U<<RF_ADC_GT_RM_LEN)-1)<<RF_ADC_GT_RM_POS))
#define RF_ADC_DCLK_INV                                         RF_ADC_DCLK_INV
#define RF_ADC_DCLK_INV_POS                                     (20U)
#define RF_ADC_DCLK_INV_LEN                                     (1U)
#define RF_ADC_DCLK_INV_MSK                                     (((1U<<RF_ADC_DCLK_INV_LEN)-1)<<RF_ADC_DCLK_INV_POS)
#define RF_ADC_DCLK_INV_UMSK                                    (~(((1U<<RF_ADC_DCLK_INV_LEN)-1)<<RF_ADC_DCLK_INV_POS))
#define RF_ADC_DCLK_SEL                                         RF_ADC_DCLK_SEL
#define RF_ADC_DCLK_SEL_POS                                     (21U)
#define RF_ADC_DCLK_SEL_LEN                                     (1U)
#define RF_ADC_DCLK_SEL_MSK                                     (((1U<<RF_ADC_DCLK_SEL_LEN)-1)<<RF_ADC_DCLK_SEL_POS)
#define RF_ADC_DCLK_SEL_UMSK                                    (~(((1U<<RF_ADC_DCLK_SEL_LEN)-1)<<RF_ADC_DCLK_SEL_POS))
#define RF_ADC_CLK_INV                                          RF_ADC_CLK_INV
#define RF_ADC_CLK_INV_POS                                      (24U)
#define RF_ADC_CLK_INV_LEN                                      (1U)
#define RF_ADC_CLK_INV_MSK                                      (((1U<<RF_ADC_CLK_INV_LEN)-1)<<RF_ADC_CLK_INV_POS)
#define RF_ADC_CLK_INV_UMSK                                     (~(((1U<<RF_ADC_CLK_INV_LEN)-1)<<RF_ADC_CLK_INV_POS))
#define RF_ADC_VREF_COMP_EN                                     RF_ADC_VREF_COMP_EN
#define RF_ADC_VREF_COMP_EN_POS                                 (25U)
#define RF_ADC_VREF_COMP_EN_LEN                                 (1U)
#define RF_ADC_VREF_COMP_EN_MSK                                 (((1U<<RF_ADC_VREF_COMP_EN_LEN)-1)<<RF_ADC_VREF_COMP_EN_POS)
#define RF_ADC_VREF_COMP_EN_UMSK                                (~(((1U<<RF_ADC_VREF_COMP_EN_LEN)-1)<<RF_ADC_VREF_COMP_EN_POS))
#define RF_ADC_CLK_DIV_SEL                                      RF_ADC_CLK_DIV_SEL
#define RF_ADC_CLK_DIV_SEL_POS                                  (28U)
#define RF_ADC_CLK_DIV_SEL_LEN                                  (1U)
#define RF_ADC_CLK_DIV_SEL_MSK                                  (((1U<<RF_ADC_CLK_DIV_SEL_LEN)-1)<<RF_ADC_CLK_DIV_SEL_POS)
#define RF_ADC_CLK_DIV_SEL_UMSK                                 (~(((1U<<RF_ADC_CLK_DIV_SEL_LEN)-1)<<RF_ADC_CLK_DIV_SEL_POS))
#define RF_ADC_TI_EN                                            RF_ADC_TI_EN
#define RF_ADC_TI_EN_POS                                        (29U)
#define RF_ADC_TI_EN_LEN                                        (1U)
#define RF_ADC_TI_EN_MSK                                        (((1U<<RF_ADC_TI_EN_LEN)-1)<<RF_ADC_TI_EN_POS)
#define RF_ADC_TI_EN_UMSK                                       (~(((1U<<RF_ADC_TI_EN_LEN)-1)<<RF_ADC_TI_EN_POS))
#define RF_ADC_VBUF_SEL                                         RF_ADC_VBUF_SEL
#define RF_ADC_VBUF_SEL_POS                                     (30U)
#define RF_ADC_VBUF_SEL_LEN                                     (2U)
#define RF_ADC_VBUF_SEL_MSK                                     (((1U<<RF_ADC_VBUF_SEL_LEN)-1)<<RF_ADC_VBUF_SEL_POS)
#define RF_ADC_VBUF_SEL_UMSK                                    (~(((1U<<RF_ADC_VBUF_SEL_LEN)-1)<<RF_ADC_VBUF_SEL_POS))

/* 0x98 : adda3 */
#define RF_ADDA3_OFFSET                                         (0x98)
#define RF_ADC_DOUT_I                                           RF_ADC_DOUT_I
#define RF_ADC_DOUT_I_POS                                       (0U)
#define RF_ADC_DOUT_I_LEN                                       (11U)
#define RF_ADC_DOUT_I_MSK                                       (((1U<<RF_ADC_DOUT_I_LEN)-1)<<RF_ADC_DOUT_I_POS)
#define RF_ADC_DOUT_I_UMSK                                      (~(((1U<<RF_ADC_DOUT_I_LEN)-1)<<RF_ADC_DOUT_I_POS))
#define RF_ADC_DOUT_Q                                           RF_ADC_DOUT_Q
#define RF_ADC_DOUT_Q_POS                                       (16U)
#define RF_ADC_DOUT_Q_LEN                                       (11U)
#define RF_ADC_DOUT_Q_MSK                                       (((1U<<RF_ADC_DOUT_Q_LEN)-1)<<RF_ADC_DOUT_Q_POS)
#define RF_ADC_DOUT_Q_UMSK                                      (~(((1U<<RF_ADC_DOUT_Q_LEN)-1)<<RF_ADC_DOUT_Q_POS))

/* 0xA0 : vco1 */
#define RF_VCO1_OFFSET                                          (0xA0)
#define RF_LO_VCO_FREQ_CW                                       RF_LO_VCO_FREQ_CW
#define RF_LO_VCO_FREQ_CW_POS                                   (0U)
#define RF_LO_VCO_FREQ_CW_LEN                                   (8U)
#define RF_LO_VCO_FREQ_CW_MSK                                   (((1U<<RF_LO_VCO_FREQ_CW_LEN)-1)<<RF_LO_VCO_FREQ_CW_POS)
#define RF_LO_VCO_FREQ_CW_UMSK                                  (~(((1U<<RF_LO_VCO_FREQ_CW_LEN)-1)<<RF_LO_VCO_FREQ_CW_POS))
#define RF_LO_VCO_FREQ_CW_HW                                    RF_LO_VCO_FREQ_CW_HW
#define RF_LO_VCO_FREQ_CW_HW_POS                                (8U)
#define RF_LO_VCO_FREQ_CW_HW_LEN                                (8U)
#define RF_LO_VCO_FREQ_CW_HW_MSK                                (((1U<<RF_LO_VCO_FREQ_CW_HW_LEN)-1)<<RF_LO_VCO_FREQ_CW_HW_POS)
#define RF_LO_VCO_FREQ_CW_HW_UMSK                               (~(((1U<<RF_LO_VCO_FREQ_CW_HW_LEN)-1)<<RF_LO_VCO_FREQ_CW_HW_POS))
#define RF_LO_VCO_IDAC_CW                                       RF_LO_VCO_IDAC_CW
#define RF_LO_VCO_IDAC_CW_POS                                   (16U)
#define RF_LO_VCO_IDAC_CW_LEN                                   (6U)
#define RF_LO_VCO_IDAC_CW_MSK                                   (((1U<<RF_LO_VCO_IDAC_CW_LEN)-1)<<RF_LO_VCO_IDAC_CW_POS)
#define RF_LO_VCO_IDAC_CW_UMSK                                  (~(((1U<<RF_LO_VCO_IDAC_CW_LEN)-1)<<RF_LO_VCO_IDAC_CW_POS))
#define RF_LO_VCO_IDAC_CW_HW                                    RF_LO_VCO_IDAC_CW_HW
#define RF_LO_VCO_IDAC_CW_HW_POS                                (24U)
#define RF_LO_VCO_IDAC_CW_HW_LEN                                (6U)
#define RF_LO_VCO_IDAC_CW_HW_MSK                                (((1U<<RF_LO_VCO_IDAC_CW_HW_LEN)-1)<<RF_LO_VCO_IDAC_CW_HW_POS)
#define RF_LO_VCO_IDAC_CW_HW_UMSK                               (~(((1U<<RF_LO_VCO_IDAC_CW_HW_LEN)-1)<<RF_LO_VCO_IDAC_CW_HW_POS))

/* 0xA4 : vco2 */
#define RF_VCO2_OFFSET                                          (0xA4)
#define RF_LO_VCO_VBIAS_CW                                      RF_LO_VCO_VBIAS_CW
#define RF_LO_VCO_VBIAS_CW_POS                                  (0U)
#define RF_LO_VCO_VBIAS_CW_LEN                                  (3U)
#define RF_LO_VCO_VBIAS_CW_MSK                                  (((1U<<RF_LO_VCO_VBIAS_CW_LEN)-1)<<RF_LO_VCO_VBIAS_CW_POS)
#define RF_LO_VCO_VBIAS_CW_UMSK                                 (~(((1U<<RF_LO_VCO_VBIAS_CW_LEN)-1)<<RF_LO_VCO_VBIAS_CW_POS))
#define RF_LO_VCO_IDAC_BOOT                                     RF_LO_VCO_IDAC_BOOT
#define RF_LO_VCO_IDAC_BOOT_POS                                 (4U)
#define RF_LO_VCO_IDAC_BOOT_LEN                                 (1U)
#define RF_LO_VCO_IDAC_BOOT_MSK                                 (((1U<<RF_LO_VCO_IDAC_BOOT_LEN)-1)<<RF_LO_VCO_IDAC_BOOT_POS)
#define RF_LO_VCO_IDAC_BOOT_UMSK                                (~(((1U<<RF_LO_VCO_IDAC_BOOT_LEN)-1)<<RF_LO_VCO_IDAC_BOOT_POS))
#define RF_LO_VCO_SHORT_VBIAS_FILTER                            RF_LO_VCO_SHORT_VBIAS_FILTER
#define RF_LO_VCO_SHORT_VBIAS_FILTER_POS                        (5U)
#define RF_LO_VCO_SHORT_VBIAS_FILTER_LEN                        (1U)
#define RF_LO_VCO_SHORT_VBIAS_FILTER_MSK                        (((1U<<RF_LO_VCO_SHORT_VBIAS_FILTER_LEN)-1)<<RF_LO_VCO_SHORT_VBIAS_FILTER_POS)
#define RF_LO_VCO_SHORT_VBIAS_FILTER_UMSK                       (~(((1U<<RF_LO_VCO_SHORT_VBIAS_FILTER_LEN)-1)<<RF_LO_VCO_SHORT_VBIAS_FILTER_POS))
#define RF_LO_VCO_SHORT_IDAC_FILTER                             RF_LO_VCO_SHORT_IDAC_FILTER
#define RF_LO_VCO_SHORT_IDAC_FILTER_POS                         (6U)
#define RF_LO_VCO_SHORT_IDAC_FILTER_LEN                         (1U)
#define RF_LO_VCO_SHORT_IDAC_FILTER_MSK                         (((1U<<RF_LO_VCO_SHORT_IDAC_FILTER_LEN)-1)<<RF_LO_VCO_SHORT_IDAC_FILTER_POS)
#define RF_LO_VCO_SHORT_IDAC_FILTER_UMSK                        (~(((1U<<RF_LO_VCO_SHORT_IDAC_FILTER_LEN)-1)<<RF_LO_VCO_SHORT_IDAC_FILTER_POS))
#define RF_ACAL_VREF_CW                                         RF_ACAL_VREF_CW
#define RF_ACAL_VREF_CW_POS                                     (8U)
#define RF_ACAL_VREF_CW_LEN                                     (4U)
#define RF_ACAL_VREF_CW_MSK                                     (((1U<<RF_ACAL_VREF_CW_LEN)-1)<<RF_ACAL_VREF_CW_POS)
#define RF_ACAL_VREF_CW_UMSK                                    (~(((1U<<RF_ACAL_VREF_CW_LEN)-1)<<RF_ACAL_VREF_CW_POS))
#define RF_ACAL_VCO_UD                                          RF_ACAL_VCO_UD
#define RF_ACAL_VCO_UD_POS                                      (12U)
#define RF_ACAL_VCO_UD_LEN                                      (1U)
#define RF_ACAL_VCO_UD_MSK                                      (((1U<<RF_ACAL_VCO_UD_LEN)-1)<<RF_ACAL_VCO_UD_POS)
#define RF_ACAL_VCO_UD_UMSK                                     (~(((1U<<RF_ACAL_VCO_UD_LEN)-1)<<RF_ACAL_VCO_UD_POS))
#define RF_ACAL_INC_EN_HW                                       RF_ACAL_INC_EN_HW
#define RF_ACAL_INC_EN_HW_POS                                   (16U)
#define RF_ACAL_INC_EN_HW_LEN                                   (1U)
#define RF_ACAL_INC_EN_HW_MSK                                   (((1U<<RF_ACAL_INC_EN_HW_LEN)-1)<<RF_ACAL_INC_EN_HW_POS)
#define RF_ACAL_INC_EN_HW_UMSK                                  (~(((1U<<RF_ACAL_INC_EN_HW_LEN)-1)<<RF_ACAL_INC_EN_HW_POS))
#define RF_LO_VCO_LDO_BYPASS                                    RF_LO_VCO_LDO_BYPASS
#define RF_LO_VCO_LDO_BYPASS_POS                                (29U)
#define RF_LO_VCO_LDO_BYPASS_LEN                                (1U)
#define RF_LO_VCO_LDO_BYPASS_MSK                                (((1U<<RF_LO_VCO_LDO_BYPASS_LEN)-1)<<RF_LO_VCO_LDO_BYPASS_POS)
#define RF_LO_VCO_LDO_BYPASS_UMSK                               (~(((1U<<RF_LO_VCO_LDO_BYPASS_LEN)-1)<<RF_LO_VCO_LDO_BYPASS_POS))
#define RF_LO_VCO_LDO_SEL                                       RF_LO_VCO_LDO_SEL
#define RF_LO_VCO_LDO_SEL_POS                                   (30U)
#define RF_LO_VCO_LDO_SEL_LEN                                   (2U)
#define RF_LO_VCO_LDO_SEL_MSK                                   (((1U<<RF_LO_VCO_LDO_SEL_LEN)-1)<<RF_LO_VCO_LDO_SEL_POS)
#define RF_LO_VCO_LDO_SEL_UMSK                                  (~(((1U<<RF_LO_VCO_LDO_SEL_LEN)-1)<<RF_LO_VCO_LDO_SEL_POS))

/* 0xA8 : vco3 */
#define RF_VCO3_OFFSET                                          (0xA8)
#define RF_FCAL_DIV                                             RF_FCAL_DIV
#define RF_FCAL_DIV_POS                                         (0U)
#define RF_FCAL_DIV_LEN                                         (16U)
#define RF_FCAL_DIV_MSK                                         (((1U<<RF_FCAL_DIV_LEN)-1)<<RF_FCAL_DIV_POS)
#define RF_FCAL_DIV_UMSK                                        (~(((1U<<RF_FCAL_DIV_LEN)-1)<<RF_FCAL_DIV_POS))
#define RF_FCAL_CNT_OP                                          RF_FCAL_CNT_OP
#define RF_FCAL_CNT_OP_POS                                      (16U)
#define RF_FCAL_CNT_OP_LEN                                      (16U)
#define RF_FCAL_CNT_OP_MSK                                      (((1U<<RF_FCAL_CNT_OP_LEN)-1)<<RF_FCAL_CNT_OP_POS)
#define RF_FCAL_CNT_OP_UMSK                                     (~(((1U<<RF_FCAL_CNT_OP_LEN)-1)<<RF_FCAL_CNT_OP_POS))

/* 0xAC : vco4 */
#define RF_VCO4_OFFSET                                          (0xAC)
#define RF_FCAL_CNT_START                                       RF_FCAL_CNT_START
#define RF_FCAL_CNT_START_POS                                   (4U)
#define RF_FCAL_CNT_START_LEN                                   (1U)
#define RF_FCAL_CNT_START_MSK                                   (((1U<<RF_FCAL_CNT_START_LEN)-1)<<RF_FCAL_CNT_START_POS)
#define RF_FCAL_CNT_START_UMSK                                  (~(((1U<<RF_FCAL_CNT_START_LEN)-1)<<RF_FCAL_CNT_START_POS))
#define RF_FCAL_INC_EN_HW                                       RF_FCAL_INC_EN_HW
#define RF_FCAL_INC_EN_HW_POS                                   (8U)
#define RF_FCAL_INC_EN_HW_LEN                                   (1U)
#define RF_FCAL_INC_EN_HW_MSK                                   (((1U<<RF_FCAL_INC_EN_HW_LEN)-1)<<RF_FCAL_INC_EN_HW_POS)
#define RF_FCAL_INC_EN_HW_UMSK                                  (~(((1U<<RF_FCAL_INC_EN_HW_LEN)-1)<<RF_FCAL_INC_EN_HW_POS))
#define RF_FCAL_INC_LARGE_RANGE                                 RF_FCAL_INC_LARGE_RANGE
#define RF_FCAL_INC_LARGE_RANGE_POS                             (16U)
#define RF_FCAL_INC_LARGE_RANGE_LEN                             (1U)
#define RF_FCAL_INC_LARGE_RANGE_MSK                             (((1U<<RF_FCAL_INC_LARGE_RANGE_LEN)-1)<<RF_FCAL_INC_LARGE_RANGE_POS)
#define RF_FCAL_INC_LARGE_RANGE_UMSK                            (~(((1U<<RF_FCAL_INC_LARGE_RANGE_LEN)-1)<<RF_FCAL_INC_LARGE_RANGE_POS))
#define RF_FCAL_CNT_RDY                                         RF_FCAL_CNT_RDY
#define RF_FCAL_CNT_RDY_POS                                     (20U)
#define RF_FCAL_CNT_RDY_LEN                                     (1U)
#define RF_FCAL_CNT_RDY_MSK                                     (((1U<<RF_FCAL_CNT_RDY_LEN)-1)<<RF_FCAL_CNT_RDY_POS)
#define RF_FCAL_CNT_RDY_UMSK                                    (~(((1U<<RF_FCAL_CNT_RDY_LEN)-1)<<RF_FCAL_CNT_RDY_POS))
#define RF_FCAL_INC_VCTRL_UD                                    RF_FCAL_INC_VCTRL_UD
#define RF_FCAL_INC_VCTRL_UD_POS                                (24U)
#define RF_FCAL_INC_VCTRL_UD_LEN                                (2U)
#define RF_FCAL_INC_VCTRL_UD_MSK                                (((1U<<RF_FCAL_INC_VCTRL_UD_LEN)-1)<<RF_FCAL_INC_VCTRL_UD_POS)
#define RF_FCAL_INC_VCTRL_UD_UMSK                               (~(((1U<<RF_FCAL_INC_VCTRL_UD_LEN)-1)<<RF_FCAL_INC_VCTRL_UD_POS))

/* 0xB0 : pfdcp */
#define RF_PFDCP_OFFSET                                         (0xB0)
#define RF_LO_CP_SEL                                            RF_LO_CP_SEL
#define RF_LO_CP_SEL_POS                                        (0U)
#define RF_LO_CP_SEL_LEN                                        (1U)
#define RF_LO_CP_SEL_MSK                                        (((1U<<RF_LO_CP_SEL_LEN)-1)<<RF_LO_CP_SEL_POS)
#define RF_LO_CP_SEL_UMSK                                       (~(((1U<<RF_LO_CP_SEL_LEN)-1)<<RF_LO_CP_SEL_POS))
#define RF_LO_CP_SEL_HW                                         RF_LO_CP_SEL_HW
#define RF_LO_CP_SEL_HW_POS                                     (1U)
#define RF_LO_CP_SEL_HW_LEN                                     (1U)
#define RF_LO_CP_SEL_HW_MSK                                     (((1U<<RF_LO_CP_SEL_HW_LEN)-1)<<RF_LO_CP_SEL_HW_POS)
#define RF_LO_CP_SEL_HW_UMSK                                    (~(((1U<<RF_LO_CP_SEL_HW_LEN)-1)<<RF_LO_CP_SEL_HW_POS))
#define RF_LO_CP_LP_MODE_EN                                     RF_LO_CP_LP_MODE_EN
#define RF_LO_CP_LP_MODE_EN_POS                                 (2U)
#define RF_LO_CP_LP_MODE_EN_LEN                                 (1U)
#define RF_LO_CP_LP_MODE_EN_MSK                                 (((1U<<RF_LO_CP_LP_MODE_EN_LEN)-1)<<RF_LO_CP_LP_MODE_EN_POS)
#define RF_LO_CP_LP_MODE_EN_UMSK                                (~(((1U<<RF_LO_CP_LP_MODE_EN_LEN)-1)<<RF_LO_CP_LP_MODE_EN_POS))
#define RF_LO_CP_OFFSET_N_SEL                                   RF_LO_CP_OFFSET_N_SEL
#define RF_LO_CP_OFFSET_N_SEL_POS                               (4U)
#define RF_LO_CP_OFFSET_N_SEL_LEN                               (2U)
#define RF_LO_CP_OFFSET_N_SEL_MSK                               (((1U<<RF_LO_CP_OFFSET_N_SEL_LEN)-1)<<RF_LO_CP_OFFSET_N_SEL_POS)
#define RF_LO_CP_OFFSET_N_SEL_UMSK                              (~(((1U<<RF_LO_CP_OFFSET_N_SEL_LEN)-1)<<RF_LO_CP_OFFSET_N_SEL_POS))
#define RF_LO_CP_OFFSET_P_SEL                                   RF_LO_CP_OFFSET_P_SEL
#define RF_LO_CP_OFFSET_P_SEL_POS                               (6U)
#define RF_LO_CP_OFFSET_P_SEL_LEN                               (2U)
#define RF_LO_CP_OFFSET_P_SEL_MSK                               (((1U<<RF_LO_CP_OFFSET_P_SEL_LEN)-1)<<RF_LO_CP_OFFSET_P_SEL_POS)
#define RF_LO_CP_OFFSET_P_SEL_UMSK                              (~(((1U<<RF_LO_CP_OFFSET_P_SEL_LEN)-1)<<RF_LO_CP_OFFSET_P_SEL_POS))
#define RF_LO_CP_STARTUP_EN                                     RF_LO_CP_STARTUP_EN
#define RF_LO_CP_STARTUP_EN_POS                                 (8U)
#define RF_LO_CP_STARTUP_EN_LEN                                 (1U)
#define RF_LO_CP_STARTUP_EN_MSK                                 (((1U<<RF_LO_CP_STARTUP_EN_LEN)-1)<<RF_LO_CP_STARTUP_EN_POS)
#define RF_LO_CP_STARTUP_EN_UMSK                                (~(((1U<<RF_LO_CP_STARTUP_EN_LEN)-1)<<RF_LO_CP_STARTUP_EN_POS))
#define RF_LO_CP_OTA_EN                                         RF_LO_CP_OTA_EN
#define RF_LO_CP_OTA_EN_POS                                     (12U)
#define RF_LO_CP_OTA_EN_LEN                                     (1U)
#define RF_LO_CP_OTA_EN_MSK                                     (((1U<<RF_LO_CP_OTA_EN_LEN)-1)<<RF_LO_CP_OTA_EN_POS)
#define RF_LO_CP_OTA_EN_UMSK                                    (~(((1U<<RF_LO_CP_OTA_EN_LEN)-1)<<RF_LO_CP_OTA_EN_POS))
#define RF_LO_CP_OPAMP_EN                                       RF_LO_CP_OPAMP_EN
#define RF_LO_CP_OPAMP_EN_POS                                   (16U)
#define RF_LO_CP_OPAMP_EN_LEN                                   (1U)
#define RF_LO_CP_OPAMP_EN_MSK                                   (((1U<<RF_LO_CP_OPAMP_EN_LEN)-1)<<RF_LO_CP_OPAMP_EN_POS)
#define RF_LO_CP_OPAMP_EN_UMSK                                  (~(((1U<<RF_LO_CP_OPAMP_EN_LEN)-1)<<RF_LO_CP_OPAMP_EN_POS))
#define RF_LO_CP_HIZ                                            RF_LO_CP_HIZ
#define RF_LO_CP_HIZ_POS                                        (20U)
#define RF_LO_CP_HIZ_LEN                                        (1U)
#define RF_LO_CP_HIZ_MSK                                        (((1U<<RF_LO_CP_HIZ_LEN)-1)<<RF_LO_CP_HIZ_POS)
#define RF_LO_CP_HIZ_UMSK                                       (~(((1U<<RF_LO_CP_HIZ_LEN)-1)<<RF_LO_CP_HIZ_POS))
#define RF_LO_PFD_RVDD_BOOST                                    RF_LO_PFD_RVDD_BOOST
#define RF_LO_PFD_RVDD_BOOST_POS                                (24U)
#define RF_LO_PFD_RVDD_BOOST_LEN                                (1U)
#define RF_LO_PFD_RVDD_BOOST_MSK                                (((1U<<RF_LO_PFD_RVDD_BOOST_LEN)-1)<<RF_LO_PFD_RVDD_BOOST_POS)
#define RF_LO_PFD_RVDD_BOOST_UMSK                               (~(((1U<<RF_LO_PFD_RVDD_BOOST_LEN)-1)<<RF_LO_PFD_RVDD_BOOST_POS))
#define RF_LO_PFD_RST_CSD                                       RF_LO_PFD_RST_CSD
#define RF_LO_PFD_RST_CSD_POS                                   (28U)
#define RF_LO_PFD_RST_CSD_LEN                                   (1U)
#define RF_LO_PFD_RST_CSD_MSK                                   (((1U<<RF_LO_PFD_RST_CSD_LEN)-1)<<RF_LO_PFD_RST_CSD_POS)
#define RF_LO_PFD_RST_CSD_UMSK                                  (~(((1U<<RF_LO_PFD_RST_CSD_LEN)-1)<<RF_LO_PFD_RST_CSD_POS))
#define RF_LO_PFD_RST_CSD_HW                                    RF_LO_PFD_RST_CSD_HW
#define RF_LO_PFD_RST_CSD_HW_POS                                (29U)
#define RF_LO_PFD_RST_CSD_HW_LEN                                (1U)
#define RF_LO_PFD_RST_CSD_HW_MSK                                (((1U<<RF_LO_PFD_RST_CSD_HW_LEN)-1)<<RF_LO_PFD_RST_CSD_HW_POS)
#define RF_LO_PFD_RST_CSD_HW_UMSK                               (~(((1U<<RF_LO_PFD_RST_CSD_HW_LEN)-1)<<RF_LO_PFD_RST_CSD_HW_POS))

/* 0xB4 : lo */
#define RF_LO_OFFSET                                            (0xB4)
#define RF_LO_LF_RZ_HW                                          RF_LO_LF_RZ_HW
#define RF_LO_LF_RZ_HW_POS                                      (0U)
#define RF_LO_LF_RZ_HW_LEN                                      (3U)
#define RF_LO_LF_RZ_HW_MSK                                      (((1U<<RF_LO_LF_RZ_HW_LEN)-1)<<RF_LO_LF_RZ_HW_POS)
#define RF_LO_LF_RZ_HW_UMSK                                     (~(((1U<<RF_LO_LF_RZ_HW_LEN)-1)<<RF_LO_LF_RZ_HW_POS))
#define RF_LO_LF_R4_HW                                          RF_LO_LF_R4_HW
#define RF_LO_LF_R4_HW_POS                                      (4U)
#define RF_LO_LF_R4_HW_LEN                                      (2U)
#define RF_LO_LF_R4_HW_MSK                                      (((1U<<RF_LO_LF_R4_HW_LEN)-1)<<RF_LO_LF_R4_HW_POS)
#define RF_LO_LF_R4_HW_UMSK                                     (~(((1U<<RF_LO_LF_R4_HW_LEN)-1)<<RF_LO_LF_R4_HW_POS))
#define RF_LO_LF_CZ_HW                                          RF_LO_LF_CZ_HW
#define RF_LO_LF_CZ_HW_POS                                      (8U)
#define RF_LO_LF_CZ_HW_LEN                                      (2U)
#define RF_LO_LF_CZ_HW_MSK                                      (((1U<<RF_LO_LF_CZ_HW_LEN)-1)<<RF_LO_LF_CZ_HW_POS)
#define RF_LO_LF_CZ_HW_UMSK                                     (~(((1U<<RF_LO_LF_CZ_HW_LEN)-1)<<RF_LO_LF_CZ_HW_POS))
#define RF_LO_LF_RZ                                             RF_LO_LF_RZ
#define RF_LO_LF_RZ_POS                                         (11U)
#define RF_LO_LF_RZ_LEN                                         (3U)
#define RF_LO_LF_RZ_MSK                                         (((1U<<RF_LO_LF_RZ_LEN)-1)<<RF_LO_LF_RZ_POS)
#define RF_LO_LF_RZ_UMSK                                        (~(((1U<<RF_LO_LF_RZ_LEN)-1)<<RF_LO_LF_RZ_POS))
#define RF_LO_LF_CZ                                             RF_LO_LF_CZ
#define RF_LO_LF_CZ_POS                                         (14U)
#define RF_LO_LF_CZ_LEN                                         (2U)
#define RF_LO_LF_CZ_MSK                                         (((1U<<RF_LO_LF_CZ_LEN)-1)<<RF_LO_LF_CZ_POS)
#define RF_LO_LF_CZ_UMSK                                        (~(((1U<<RF_LO_LF_CZ_LEN)-1)<<RF_LO_LF_CZ_POS))
#define RF_LO_LF_R4                                             RF_LO_LF_R4
#define RF_LO_LF_R4_POS                                         (16U)
#define RF_LO_LF_R4_LEN                                         (2U)
#define RF_LO_LF_R4_MSK                                         (((1U<<RF_LO_LF_R4_LEN)-1)<<RF_LO_LF_R4_POS)
#define RF_LO_LF_R4_UMSK                                        (~(((1U<<RF_LO_LF_R4_LEN)-1)<<RF_LO_LF_R4_POS))
#define RF_LO_LF_R4_SHORT                                       RF_LO_LF_R4_SHORT
#define RF_LO_LF_R4_SHORT_POS                                   (18U)
#define RF_LO_LF_R4_SHORT_LEN                                   (1U)
#define RF_LO_LF_R4_SHORT_MSK                                   (((1U<<RF_LO_LF_R4_SHORT_LEN)-1)<<RF_LO_LF_R4_SHORT_POS)
#define RF_LO_LF_R4_SHORT_UMSK                                  (~(((1U<<RF_LO_LF_R4_SHORT_LEN)-1)<<RF_LO_LF_R4_SHORT_POS))
#define RF_LO_SLIPPED_DN                                        RF_LO_SLIPPED_DN
#define RF_LO_SLIPPED_DN_POS                                    (20U)
#define RF_LO_SLIPPED_DN_LEN                                    (1U)
#define RF_LO_SLIPPED_DN_MSK                                    (((1U<<RF_LO_SLIPPED_DN_LEN)-1)<<RF_LO_SLIPPED_DN_POS)
#define RF_LO_SLIPPED_DN_UMSK                                   (~(((1U<<RF_LO_SLIPPED_DN_LEN)-1)<<RF_LO_SLIPPED_DN_POS))
#define RF_LO_SLIPPED_UP                                        RF_LO_SLIPPED_UP
#define RF_LO_SLIPPED_UP_POS                                    (24U)
#define RF_LO_SLIPPED_UP_LEN                                    (1U)
#define RF_LO_SLIPPED_UP_MSK                                    (((1U<<RF_LO_SLIPPED_UP_LEN)-1)<<RF_LO_SLIPPED_UP_POS)
#define RF_LO_SLIPPED_UP_UMSK                                   (~(((1U<<RF_LO_SLIPPED_UP_LEN)-1)<<RF_LO_SLIPPED_UP_POS))

/* 0xB8 : fbdv */
#define RF_FBDV_OFFSET                                          (0xB8)
#define RF_LO_FBDV_HALFSTEP_EN_HW                               RF_LO_FBDV_HALFSTEP_EN_HW
#define RF_LO_FBDV_HALFSTEP_EN_HW_POS                           (0U)
#define RF_LO_FBDV_HALFSTEP_EN_HW_LEN                           (1U)
#define RF_LO_FBDV_HALFSTEP_EN_HW_MSK                           (((1U<<RF_LO_FBDV_HALFSTEP_EN_HW_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_HW_POS)
#define RF_LO_FBDV_HALFSTEP_EN_HW_UMSK                          (~(((1U<<RF_LO_FBDV_HALFSTEP_EN_HW_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_HW_POS))
#define RF_LO_FBDV_HALFSTEP_EN                                  RF_LO_FBDV_HALFSTEP_EN
#define RF_LO_FBDV_HALFSTEP_EN_POS                              (4U)
#define RF_LO_FBDV_HALFSTEP_EN_LEN                              (1U)
#define RF_LO_FBDV_HALFSTEP_EN_MSK                              (((1U<<RF_LO_FBDV_HALFSTEP_EN_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_POS)
#define RF_LO_FBDV_HALFSTEP_EN_UMSK                             (~(((1U<<RF_LO_FBDV_HALFSTEP_EN_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_POS))
#define RF_LO_FBDV_SEL_SAMPLE_CLK                               RF_LO_FBDV_SEL_SAMPLE_CLK
#define RF_LO_FBDV_SEL_SAMPLE_CLK_POS                           (8U)
#define RF_LO_FBDV_SEL_SAMPLE_CLK_LEN                           (2U)
#define RF_LO_FBDV_SEL_SAMPLE_CLK_MSK                           (((1U<<RF_LO_FBDV_SEL_SAMPLE_CLK_LEN)-1)<<RF_LO_FBDV_SEL_SAMPLE_CLK_POS)
#define RF_LO_FBDV_SEL_SAMPLE_CLK_UMSK                          (~(((1U<<RF_LO_FBDV_SEL_SAMPLE_CLK_LEN)-1)<<RF_LO_FBDV_SEL_SAMPLE_CLK_POS))
#define RF_LO_FBDV_SEL_FB_CLK                                   RF_LO_FBDV_SEL_FB_CLK
#define RF_LO_FBDV_SEL_FB_CLK_POS                               (12U)
#define RF_LO_FBDV_SEL_FB_CLK_LEN                               (2U)
#define RF_LO_FBDV_SEL_FB_CLK_MSK                               (((1U<<RF_LO_FBDV_SEL_FB_CLK_LEN)-1)<<RF_LO_FBDV_SEL_FB_CLK_POS)
#define RF_LO_FBDV_SEL_FB_CLK_UMSK                              (~(((1U<<RF_LO_FBDV_SEL_FB_CLK_LEN)-1)<<RF_LO_FBDV_SEL_FB_CLK_POS))
#define RF_LO_FBDV_RST                                          RF_LO_FBDV_RST
#define RF_LO_FBDV_RST_POS                                      (16U)
#define RF_LO_FBDV_RST_LEN                                      (1U)
#define RF_LO_FBDV_RST_MSK                                      (((1U<<RF_LO_FBDV_RST_LEN)-1)<<RF_LO_FBDV_RST_POS)
#define RF_LO_FBDV_RST_UMSK                                     (~(((1U<<RF_LO_FBDV_RST_LEN)-1)<<RF_LO_FBDV_RST_POS))
#define RF_LO_FBDV_RST_HW                                       RF_LO_FBDV_RST_HW
#define RF_LO_FBDV_RST_HW_POS                                   (20U)
#define RF_LO_FBDV_RST_HW_LEN                                   (1U)
#define RF_LO_FBDV_RST_HW_MSK                                   (((1U<<RF_LO_FBDV_RST_HW_LEN)-1)<<RF_LO_FBDV_RST_HW_POS)
#define RF_LO_FBDV_RST_HW_UMSK                                  (~(((1U<<RF_LO_FBDV_RST_HW_LEN)-1)<<RF_LO_FBDV_RST_HW_POS))
#define RF_PU_VBUF_FBDV                                         RF_PU_VBUF_FBDV
#define RF_PU_VBUF_FBDV_POS                                     (24U)
#define RF_PU_VBUF_FBDV_LEN                                     (1U)
#define RF_PU_VBUF_FBDV_MSK                                     (((1U<<RF_PU_VBUF_FBDV_LEN)-1)<<RF_PU_VBUF_FBDV_POS)
#define RF_PU_VBUF_FBDV_UMSK                                    (~(((1U<<RF_PU_VBUF_FBDV_LEN)-1)<<RF_PU_VBUF_FBDV_POS))
#define RF_PU_VBUF_FBDV_HW                                      RF_PU_VBUF_FBDV_HW
#define RF_PU_VBUF_FBDV_HW_POS                                  (28U)
#define RF_PU_VBUF_FBDV_HW_LEN                                  (1U)
#define RF_PU_VBUF_FBDV_HW_MSK                                  (((1U<<RF_PU_VBUF_FBDV_HW_LEN)-1)<<RF_PU_VBUF_FBDV_HW_POS)
#define RF_PU_VBUF_FBDV_HW_UMSK                                 (~(((1U<<RF_PU_VBUF_FBDV_HW_LEN)-1)<<RF_PU_VBUF_FBDV_HW_POS))
#define RF_PPU_VBUF_FBDV_HW                                     RF_PPU_VBUF_FBDV_HW
#define RF_PPU_VBUF_FBDV_HW_POS                                 (29U)
#define RF_PPU_VBUF_FBDV_HW_LEN                                 (1U)
#define RF_PPU_VBUF_FBDV_HW_MSK                                 (((1U<<RF_PPU_VBUF_FBDV_HW_LEN)-1)<<RF_PPU_VBUF_FBDV_HW_POS)
#define RF_PPU_VBUF_FBDV_HW_UMSK                                (~(((1U<<RF_PPU_VBUF_FBDV_HW_LEN)-1)<<RF_PPU_VBUF_FBDV_HW_POS))

/* 0xBC : lodist */
#define RF_LODIST_OFFSET                                        (0xBC)
#define RF_LO_OSMX_XGM_BOOST                                    RF_LO_OSMX_XGM_BOOST
#define RF_LO_OSMX_XGM_BOOST_POS                                (0U)
#define RF_LO_OSMX_XGM_BOOST_LEN                                (1U)
#define RF_LO_OSMX_XGM_BOOST_MSK                                (((1U<<RF_LO_OSMX_XGM_BOOST_LEN)-1)<<RF_LO_OSMX_XGM_BOOST_POS)
#define RF_LO_OSMX_XGM_BOOST_UMSK                               (~(((1U<<RF_LO_OSMX_XGM_BOOST_LEN)-1)<<RF_LO_OSMX_XGM_BOOST_POS))
#define RF_LO_OSMX_EN_XGM                                       RF_LO_OSMX_EN_XGM
#define RF_LO_OSMX_EN_XGM_POS                                   (4U)
#define RF_LO_OSMX_EN_XGM_LEN                                   (1U)
#define RF_LO_OSMX_EN_XGM_MSK                                   (((1U<<RF_LO_OSMX_EN_XGM_LEN)-1)<<RF_LO_OSMX_EN_XGM_POS)
#define RF_LO_OSMX_EN_XGM_UMSK                                  (~(((1U<<RF_LO_OSMX_EN_XGM_LEN)-1)<<RF_LO_OSMX_EN_XGM_POS))
#define RF_LO_OSMX_FIX_CAP                                      RF_LO_OSMX_FIX_CAP
#define RF_LO_OSMX_FIX_CAP_POS                                  (8U)
#define RF_LO_OSMX_FIX_CAP_LEN                                  (1U)
#define RF_LO_OSMX_FIX_CAP_MSK                                  (((1U<<RF_LO_OSMX_FIX_CAP_LEN)-1)<<RF_LO_OSMX_FIX_CAP_POS)
#define RF_LO_OSMX_FIX_CAP_UMSK                                 (~(((1U<<RF_LO_OSMX_FIX_CAP_LEN)-1)<<RF_LO_OSMX_FIX_CAP_POS))
#define RF_LO_OSMX_VBUF_STRE                                    RF_LO_OSMX_VBUF_STRE
#define RF_LO_OSMX_VBUF_STRE_POS                                (12U)
#define RF_LO_OSMX_VBUF_STRE_LEN                                (1U)
#define RF_LO_OSMX_VBUF_STRE_MSK                                (((1U<<RF_LO_OSMX_VBUF_STRE_LEN)-1)<<RF_LO_OSMX_VBUF_STRE_POS)
#define RF_LO_OSMX_VBUF_STRE_UMSK                               (~(((1U<<RF_LO_OSMX_VBUF_STRE_LEN)-1)<<RF_LO_OSMX_VBUF_STRE_POS))
#define RF_LO_OSMX_CAPBANK_BIAS                                 RF_LO_OSMX_CAPBANK_BIAS
#define RF_LO_OSMX_CAPBANK_BIAS_POS                             (16U)
#define RF_LO_OSMX_CAPBANK_BIAS_LEN                             (2U)
#define RF_LO_OSMX_CAPBANK_BIAS_MSK                             (((1U<<RF_LO_OSMX_CAPBANK_BIAS_LEN)-1)<<RF_LO_OSMX_CAPBANK_BIAS_POS)
#define RF_LO_OSMX_CAPBANK_BIAS_UMSK                            (~(((1U<<RF_LO_OSMX_CAPBANK_BIAS_LEN)-1)<<RF_LO_OSMX_CAPBANK_BIAS_POS))
#define RF_LO_OSMX_CAP                                          RF_LO_OSMX_CAP
#define RF_LO_OSMX_CAP_POS                                      (20U)
#define RF_LO_OSMX_CAP_LEN                                      (4U)
#define RF_LO_OSMX_CAP_MSK                                      (((1U<<RF_LO_OSMX_CAP_LEN)-1)<<RF_LO_OSMX_CAP_POS)
#define RF_LO_OSMX_CAP_UMSK                                     (~(((1U<<RF_LO_OSMX_CAP_LEN)-1)<<RF_LO_OSMX_CAP_POS))
#define RF_LO_LODIST_TXBUF_STRE                                 RF_LO_LODIST_TXBUF_STRE
#define RF_LO_LODIST_TXBUF_STRE_POS                             (24U)
#define RF_LO_LODIST_TXBUF_STRE_LEN                             (1U)
#define RF_LO_LODIST_TXBUF_STRE_MSK                             (((1U<<RF_LO_LODIST_TXBUF_STRE_LEN)-1)<<RF_LO_LODIST_TXBUF_STRE_POS)
#define RF_LO_LODIST_TXBUF_STRE_UMSK                            (~(((1U<<RF_LO_LODIST_TXBUF_STRE_LEN)-1)<<RF_LO_LODIST_TXBUF_STRE_POS))
#define RF_LO_LODIST_RXBUF_STRE                                 RF_LO_LODIST_RXBUF_STRE
#define RF_LO_LODIST_RXBUF_STRE_POS                             (27U)
#define RF_LO_LODIST_RXBUF_STRE_LEN                             (1U)
#define RF_LO_LODIST_RXBUF_STRE_MSK                             (((1U<<RF_LO_LODIST_RXBUF_STRE_LEN)-1)<<RF_LO_LODIST_RXBUF_STRE_POS)
#define RF_LO_LODIST_RXBUF_STRE_UMSK                            (~(((1U<<RF_LO_LODIST_RXBUF_STRE_LEN)-1)<<RF_LO_LODIST_RXBUF_STRE_POS))
#define RF_LO_LODIST_DPD_EN                                     RF_LO_LODIST_DPD_EN
#define RF_LO_LODIST_DPD_EN_POS                                 (28U)
#define RF_LO_LODIST_DPD_EN_LEN                                 (1U)
#define RF_LO_LODIST_DPD_EN_MSK                                 (((1U<<RF_LO_LODIST_DPD_EN_LEN)-1)<<RF_LO_LODIST_DPD_EN_POS)
#define RF_LO_LODIST_DPD_EN_UMSK                                (~(((1U<<RF_LO_LODIST_DPD_EN_LEN)-1)<<RF_LO_LODIST_DPD_EN_POS))
#define RF_PU_VBUF_LODIST                                       RF_PU_VBUF_LODIST
#define RF_PU_VBUF_LODIST_POS                                   (29U)
#define RF_PU_VBUF_LODIST_LEN                                   (1U)
#define RF_PU_VBUF_LODIST_MSK                                   (((1U<<RF_PU_VBUF_LODIST_LEN)-1)<<RF_PU_VBUF_LODIST_POS)
#define RF_PU_VBUF_LODIST_UMSK                                  (~(((1U<<RF_PU_VBUF_LODIST_LEN)-1)<<RF_PU_VBUF_LODIST_POS))
#define RF_PU_VBUF_LODIST_HW                                    RF_PU_VBUF_LODIST_HW
#define RF_PU_VBUF_LODIST_HW_POS                                (30U)
#define RF_PU_VBUF_LODIST_HW_LEN                                (1U)
#define RF_PU_VBUF_LODIST_HW_MSK                                (((1U<<RF_PU_VBUF_LODIST_HW_LEN)-1)<<RF_PU_VBUF_LODIST_HW_POS)
#define RF_PU_VBUF_LODIST_HW_UMSK                               (~(((1U<<RF_PU_VBUF_LODIST_HW_LEN)-1)<<RF_PU_VBUF_LODIST_HW_POS))
#define RF_PPU_VBUF_LODIST_HW                                   RF_PPU_VBUF_LODIST_HW
#define RF_PPU_VBUF_LODIST_HW_POS                               (31U)
#define RF_PPU_VBUF_LODIST_HW_LEN                               (1U)
#define RF_PPU_VBUF_LODIST_HW_MSK                               (((1U<<RF_PPU_VBUF_LODIST_HW_LEN)-1)<<RF_PPU_VBUF_LODIST_HW_POS)
#define RF_PPU_VBUF_LODIST_HW_UMSK                              (~(((1U<<RF_PPU_VBUF_LODIST_HW_LEN)-1)<<RF_PPU_VBUF_LODIST_HW_POS))

/* 0xC0 : sdm1 */
#define RF_SDM1_OFFSET                                          (0xC0)
#define RF_LO_SDM_DITHER_SEL_HW                                 RF_LO_SDM_DITHER_SEL_HW
#define RF_LO_SDM_DITHER_SEL_HW_POS                             (0U)
#define RF_LO_SDM_DITHER_SEL_HW_LEN                             (2U)
#define RF_LO_SDM_DITHER_SEL_HW_MSK                             (((1U<<RF_LO_SDM_DITHER_SEL_HW_LEN)-1)<<RF_LO_SDM_DITHER_SEL_HW_POS)
#define RF_LO_SDM_DITHER_SEL_HW_UMSK                            (~(((1U<<RF_LO_SDM_DITHER_SEL_HW_LEN)-1)<<RF_LO_SDM_DITHER_SEL_HW_POS))
#define RF_LO_SDM_BYPASS_HW                                     RF_LO_SDM_BYPASS_HW
#define RF_LO_SDM_BYPASS_HW_POS                                 (4U)
#define RF_LO_SDM_BYPASS_HW_LEN                                 (1U)
#define RF_LO_SDM_BYPASS_HW_MSK                                 (((1U<<RF_LO_SDM_BYPASS_HW_LEN)-1)<<RF_LO_SDM_BYPASS_HW_POS)
#define RF_LO_SDM_BYPASS_HW_UMSK                                (~(((1U<<RF_LO_SDM_BYPASS_HW_LEN)-1)<<RF_LO_SDM_BYPASS_HW_POS))
#define RF_LO_SDM_DITHER_SEL                                    RF_LO_SDM_DITHER_SEL
#define RF_LO_SDM_DITHER_SEL_POS                                (8U)
#define RF_LO_SDM_DITHER_SEL_LEN                                (2U)
#define RF_LO_SDM_DITHER_SEL_MSK                                (((1U<<RF_LO_SDM_DITHER_SEL_LEN)-1)<<RF_LO_SDM_DITHER_SEL_POS)
#define RF_LO_SDM_DITHER_SEL_UMSK                               (~(((1U<<RF_LO_SDM_DITHER_SEL_LEN)-1)<<RF_LO_SDM_DITHER_SEL_POS))
#define RF_LO_SDM_BYPASS                                        RF_LO_SDM_BYPASS
#define RF_LO_SDM_BYPASS_POS                                    (12U)
#define RF_LO_SDM_BYPASS_LEN                                    (1U)
#define RF_LO_SDM_BYPASS_MSK                                    (((1U<<RF_LO_SDM_BYPASS_LEN)-1)<<RF_LO_SDM_BYPASS_POS)
#define RF_LO_SDM_BYPASS_UMSK                                   (~(((1U<<RF_LO_SDM_BYPASS_LEN)-1)<<RF_LO_SDM_BYPASS_POS))
#define RF_LO_SDM_RSTB                                          RF_LO_SDM_RSTB
#define RF_LO_SDM_RSTB_POS                                      (16U)
#define RF_LO_SDM_RSTB_LEN                                      (1U)
#define RF_LO_SDM_RSTB_MSK                                      (((1U<<RF_LO_SDM_RSTB_LEN)-1)<<RF_LO_SDM_RSTB_POS)
#define RF_LO_SDM_RSTB_UMSK                                     (~(((1U<<RF_LO_SDM_RSTB_LEN)-1)<<RF_LO_SDM_RSTB_POS))
#define RF_LO_SDM_RSTB_HW                                       RF_LO_SDM_RSTB_HW
#define RF_LO_SDM_RSTB_HW_POS                                   (17U)
#define RF_LO_SDM_RSTB_HW_LEN                                   (1U)
#define RF_LO_SDM_RSTB_HW_MSK                                   (((1U<<RF_LO_SDM_RSTB_HW_LEN)-1)<<RF_LO_SDM_RSTB_HW_POS)
#define RF_LO_SDM_RSTB_HW_UMSK                                  (~(((1U<<RF_LO_SDM_RSTB_HW_LEN)-1)<<RF_LO_SDM_RSTB_HW_POS))
#define RF_LO_SDM_FLAG                                          RF_LO_SDM_FLAG
#define RF_LO_SDM_FLAG_POS                                      (20U)
#define RF_LO_SDM_FLAG_LEN                                      (1U)
#define RF_LO_SDM_FLAG_MSK                                      (((1U<<RF_LO_SDM_FLAG_LEN)-1)<<RF_LO_SDM_FLAG_POS)
#define RF_LO_SDM_FLAG_UMSK                                     (~(((1U<<RF_LO_SDM_FLAG_LEN)-1)<<RF_LO_SDM_FLAG_POS))

/* 0xC4 : sdm2 */
#define RF_SDM2_OFFSET                                          (0xC4)
#define RF_LO_SDMIN                                             RF_LO_SDMIN
#define RF_LO_SDMIN_POS                                         (0U)
#define RF_LO_SDMIN_LEN                                         (30U)
#define RF_LO_SDMIN_MSK                                         (((1U<<RF_LO_SDMIN_LEN)-1)<<RF_LO_SDMIN_POS)
#define RF_LO_SDMIN_UMSK                                        (~(((1U<<RF_LO_SDMIN_LEN)-1)<<RF_LO_SDMIN_POS))

/* 0xC8 : sdm3 */
#define RF_SDM3_OFFSET                                          (0xC8)
#define RF_LO_SDMIN_HW                                          RF_LO_SDMIN_HW
#define RF_LO_SDMIN_HW_POS                                      (0U)
#define RF_LO_SDMIN_HW_LEN                                      (30U)
#define RF_LO_SDMIN_HW_MSK                                      (((1U<<RF_LO_SDMIN_HW_LEN)-1)<<RF_LO_SDMIN_HW_POS)
#define RF_LO_SDMIN_HW_UMSK                                     (~(((1U<<RF_LO_SDMIN_HW_LEN)-1)<<RF_LO_SDMIN_HW_POS))

/* 0xCC : refbuf */
#define RF_REFBUF_OFFSET                                        (0xCC)
#define RF_LO_REFBUF_LOCLK_EN                                   RF_LO_REFBUF_LOCLK_EN
#define RF_LO_REFBUF_LOCLK_EN_POS                               (21U)
#define RF_LO_REFBUF_LOCLK_EN_LEN                               (1U)
#define RF_LO_REFBUF_LOCLK_EN_MSK                               (((1U<<RF_LO_REFBUF_LOCLK_EN_LEN)-1)<<RF_LO_REFBUF_LOCLK_EN_POS)
#define RF_LO_REFBUF_LOCLK_EN_UMSK                              (~(((1U<<RF_LO_REFBUF_LOCLK_EN_LEN)-1)<<RF_LO_REFBUF_LOCLK_EN_POS))

/* 0xD0 : glb_misc */
#define RF_GLB_MISC_OFFSET                                      (0xD0)
#define RF_WIFIPLL_SDM_BYPASS_RX_BZ                             RF_WIFIPLL_SDM_BYPASS_RX_BZ
#define RF_WIFIPLL_SDM_BYPASS_RX_BZ_POS                         (0U)
#define RF_WIFIPLL_SDM_BYPASS_RX_BZ_LEN                         (1U)
#define RF_WIFIPLL_SDM_BYPASS_RX_BZ_MSK                         (((1U<<RF_WIFIPLL_SDM_BYPASS_RX_BZ_LEN)-1)<<RF_WIFIPLL_SDM_BYPASS_RX_BZ_POS)
#define RF_WIFIPLL_SDM_BYPASS_RX_BZ_UMSK                        (~(((1U<<RF_WIFIPLL_SDM_BYPASS_RX_BZ_LEN)-1)<<RF_WIFIPLL_SDM_BYPASS_RX_BZ_POS))
#define RF_WIFIPLL_SDM_BYPASS_TX_BZ                             RF_WIFIPLL_SDM_BYPASS_TX_BZ
#define RF_WIFIPLL_SDM_BYPASS_TX_BZ_POS                         (1U)
#define RF_WIFIPLL_SDM_BYPASS_TX_BZ_LEN                         (1U)
#define RF_WIFIPLL_SDM_BYPASS_TX_BZ_MSK                         (((1U<<RF_WIFIPLL_SDM_BYPASS_TX_BZ_LEN)-1)<<RF_WIFIPLL_SDM_BYPASS_TX_BZ_POS)
#define RF_WIFIPLL_SDM_BYPASS_TX_BZ_UMSK                        (~(((1U<<RF_WIFIPLL_SDM_BYPASS_TX_BZ_LEN)-1)<<RF_WIFIPLL_SDM_BYPASS_TX_BZ_POS))
#define RF_WIFIPLL_SDM_BYPASS_RX_WF                             RF_WIFIPLL_SDM_BYPASS_RX_WF
#define RF_WIFIPLL_SDM_BYPASS_RX_WF_POS                         (2U)
#define RF_WIFIPLL_SDM_BYPASS_RX_WF_LEN                         (1U)
#define RF_WIFIPLL_SDM_BYPASS_RX_WF_MSK                         (((1U<<RF_WIFIPLL_SDM_BYPASS_RX_WF_LEN)-1)<<RF_WIFIPLL_SDM_BYPASS_RX_WF_POS)
#define RF_WIFIPLL_SDM_BYPASS_RX_WF_UMSK                        (~(((1U<<RF_WIFIPLL_SDM_BYPASS_RX_WF_LEN)-1)<<RF_WIFIPLL_SDM_BYPASS_RX_WF_POS))
#define RF_WIFIPLL_SDM_BYPASS_TX_WF                             RF_WIFIPLL_SDM_BYPASS_TX_WF
#define RF_WIFIPLL_SDM_BYPASS_TX_WF_POS                         (3U)
#define RF_WIFIPLL_SDM_BYPASS_TX_WF_LEN                         (1U)
#define RF_WIFIPLL_SDM_BYPASS_TX_WF_MSK                         (((1U<<RF_WIFIPLL_SDM_BYPASS_TX_WF_LEN)-1)<<RF_WIFIPLL_SDM_BYPASS_TX_WF_POS)
#define RF_WIFIPLL_SDM_BYPASS_TX_WF_UMSK                        (~(((1U<<RF_WIFIPLL_SDM_BYPASS_TX_WF_LEN)-1)<<RF_WIFIPLL_SDM_BYPASS_TX_WF_POS))

/* 0xD4 : RX normal bias mode registers */
#define RF_CIP_1_OFFSET                                         (0xD4)
#define RF_PA_IMIX_BIAS_SEL                                     RF_PA_IMIX_BIAS_SEL
#define RF_PA_IMIX_BIAS_SEL_POS                                 (0U)
#define RF_PA_IMIX_BIAS_SEL_LEN                                 (1U)
#define RF_PA_IMIX_BIAS_SEL_MSK                                 (((1U<<RF_PA_IMIX_BIAS_SEL_LEN)-1)<<RF_PA_IMIX_BIAS_SEL_POS)
#define RF_PA_IMIX_BIAS_SEL_UMSK                                (~(((1U<<RF_PA_IMIX_BIAS_SEL_LEN)-1)<<RF_PA_IMIX_BIAS_SEL_POS))
#define RF_PA_ICX_SCALE                                         RF_PA_ICX_SCALE
#define RF_PA_ICX_SCALE_POS                                     (4U)
#define RF_PA_ICX_SCALE_LEN                                     (4U)
#define RF_PA_ICX_SCALE_MSK                                     (((1U<<RF_PA_ICX_SCALE_LEN)-1)<<RF_PA_ICX_SCALE_POS)
#define RF_PA_ICX_SCALE_UMSK                                    (~(((1U<<RF_PA_ICX_SCALE_LEN)-1)<<RF_PA_ICX_SCALE_POS))
#define RF_PA_IPTAT_SCALE                                       RF_PA_IPTAT_SCALE
#define RF_PA_IPTAT_SCALE_POS                                   (8U)
#define RF_PA_IPTAT_SCALE_LEN                                   (4U)
#define RF_PA_IPTAT_SCALE_MSK                                   (((1U<<RF_PA_IPTAT_SCALE_LEN)-1)<<RF_PA_IPTAT_SCALE_POS)
#define RF_PA_IPTAT_SCALE_UMSK                                  (~(((1U<<RF_PA_IPTAT_SCALE_LEN)-1)<<RF_PA_IPTAT_SCALE_POS))
#define RF_DAC_IMIX_BIAS_SEL                                    RF_DAC_IMIX_BIAS_SEL
#define RF_DAC_IMIX_BIAS_SEL_POS                                (12U)
#define RF_DAC_IMIX_BIAS_SEL_LEN                                (1U)
#define RF_DAC_IMIX_BIAS_SEL_MSK                                (((1U<<RF_DAC_IMIX_BIAS_SEL_LEN)-1)<<RF_DAC_IMIX_BIAS_SEL_POS)
#define RF_DAC_IMIX_BIAS_SEL_UMSK                               (~(((1U<<RF_DAC_IMIX_BIAS_SEL_LEN)-1)<<RF_DAC_IMIX_BIAS_SEL_POS))
#define RF_DAC_ICX_SCALE                                        RF_DAC_ICX_SCALE
#define RF_DAC_ICX_SCALE_POS                                    (16U)
#define RF_DAC_ICX_SCALE_LEN                                    (4U)
#define RF_DAC_ICX_SCALE_MSK                                    (((1U<<RF_DAC_ICX_SCALE_LEN)-1)<<RF_DAC_ICX_SCALE_POS)
#define RF_DAC_ICX_SCALE_UMSK                                   (~(((1U<<RF_DAC_ICX_SCALE_LEN)-1)<<RF_DAC_ICX_SCALE_POS))
#define RF_DAC_IPTAT_SCALE                                      RF_DAC_IPTAT_SCALE
#define RF_DAC_IPTAT_SCALE_POS                                  (20U)
#define RF_DAC_IPTAT_SCALE_LEN                                  (4U)
#define RF_DAC_IPTAT_SCALE_MSK                                  (((1U<<RF_DAC_IPTAT_SCALE_LEN)-1)<<RF_DAC_IPTAT_SCALE_POS)
#define RF_DAC_IPTAT_SCALE_UMSK                                 (~(((1U<<RF_DAC_IPTAT_SCALE_LEN)-1)<<RF_DAC_IPTAT_SCALE_POS))

/* 0xEC : rf_resv_reg_0 */
#define RF_RESV_REG_0_OFFSET                                    (0xEC)
#define RF_RESERVED0                                            RF_RESERVED0
#define RF_RESERVED0_POS                                        (0U)
#define RF_RESERVED0_LEN                                        (32U)
#define RF_RESERVED0_MSK                                        (((1U<<RF_RESERVED0_LEN)-1)<<RF_RESERVED0_POS)
#define RF_RESERVED0_UMSK                                       (~(((1U<<RF_RESERVED0_LEN)-1)<<RF_RESERVED0_POS))

/* 0xF0 : rf_resv_reg_1 */
#define RF_RESV_REG_1_OFFSET                                    (0xF0)
#define RF_RESERVED1                                            RF_RESERVED1
#define RF_RESERVED1_POS                                        (0U)
#define RF_RESERVED1_LEN                                        (32U)
#define RF_RESERVED1_MSK                                        (((1U<<RF_RESERVED1_LEN)-1)<<RF_RESERVED1_POS)
#define RF_RESERVED1_UMSK                                       (~(((1U<<RF_RESERVED1_LEN)-1)<<RF_RESERVED1_POS))

/* 0xF4 : rf_resv_reg_2 */
#define RF_RESV_REG_2_OFFSET                                    (0xF4)
#define RF_RESERVED2                                            RF_RESERVED2
#define RF_RESERVED2_POS                                        (0U)
#define RF_RESERVED2_LEN                                        (32U)
#define RF_RESERVED2_MSK                                        (((1U<<RF_RESERVED2_LEN)-1)<<RF_RESERVED2_POS)
#define RF_RESERVED2_UMSK                                       (~(((1U<<RF_RESERVED2_LEN)-1)<<RF_RESERVED2_POS))

/* 0x100 : rrf_gain_index1 */
#define RF_RRF_GAIN_INDEX1_OFFSET                               (0x100)
#define RF_GAIN_CTRL0_GC_RMXGM_RATT                             RF_GAIN_CTRL0_GC_RMXGM_RATT
#define RF_GAIN_CTRL0_GC_RMXGM_RATT_POS                         (0U)
#define RF_GAIN_CTRL0_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL0_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL0_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL0_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL0_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL0_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL0_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL0_GC_RMXGM                                  RF_GAIN_CTRL0_GC_RMXGM
#define RF_GAIN_CTRL0_GC_RMXGM_POS                              (3U)
#define RF_GAIN_CTRL0_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL0_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL0_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL0_GC_RMXGM_POS)
#define RF_GAIN_CTRL0_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL0_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL0_GC_RMXGM_POS))
#define RF_GAIN_CTRL0_GC_LNA                                    RF_GAIN_CTRL0_GC_LNA
#define RF_GAIN_CTRL0_GC_LNA_POS                                (5U)
#define RF_GAIN_CTRL0_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL0_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL0_GC_LNA_LEN)-1)<<RF_GAIN_CTRL0_GC_LNA_POS)
#define RF_GAIN_CTRL0_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL0_GC_LNA_LEN)-1)<<RF_GAIN_CTRL0_GC_LNA_POS))
#define RF_GAIN_CTRL1_GC_RMXGM_RATT                             RF_GAIN_CTRL1_GC_RMXGM_RATT
#define RF_GAIN_CTRL1_GC_RMXGM_RATT_POS                         (8U)
#define RF_GAIN_CTRL1_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL1_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL1_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL1_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL1_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL1_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL1_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL1_GC_RMXGM                                  RF_GAIN_CTRL1_GC_RMXGM
#define RF_GAIN_CTRL1_GC_RMXGM_POS                              (11U)
#define RF_GAIN_CTRL1_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL1_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL1_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL1_GC_RMXGM_POS)
#define RF_GAIN_CTRL1_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL1_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL1_GC_RMXGM_POS))
#define RF_GAIN_CTRL1_GC_LNA                                    RF_GAIN_CTRL1_GC_LNA
#define RF_GAIN_CTRL1_GC_LNA_POS                                (13U)
#define RF_GAIN_CTRL1_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL1_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL1_GC_LNA_LEN)-1)<<RF_GAIN_CTRL1_GC_LNA_POS)
#define RF_GAIN_CTRL1_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL1_GC_LNA_LEN)-1)<<RF_GAIN_CTRL1_GC_LNA_POS))
#define RF_GAIN_CTRL2_GC_RMXGM_RATT                             RF_GAIN_CTRL2_GC_RMXGM_RATT
#define RF_GAIN_CTRL2_GC_RMXGM_RATT_POS                         (16U)
#define RF_GAIN_CTRL2_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL2_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL2_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL2_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL2_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL2_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL2_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL2_GC_RMXGM                                  RF_GAIN_CTRL2_GC_RMXGM
#define RF_GAIN_CTRL2_GC_RMXGM_POS                              (19U)
#define RF_GAIN_CTRL2_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL2_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL2_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL2_GC_RMXGM_POS)
#define RF_GAIN_CTRL2_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL2_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL2_GC_RMXGM_POS))
#define RF_GAIN_CTRL2_GC_LNA                                    RF_GAIN_CTRL2_GC_LNA
#define RF_GAIN_CTRL2_GC_LNA_POS                                (21U)
#define RF_GAIN_CTRL2_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL2_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL2_GC_LNA_LEN)-1)<<RF_GAIN_CTRL2_GC_LNA_POS)
#define RF_GAIN_CTRL2_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL2_GC_LNA_LEN)-1)<<RF_GAIN_CTRL2_GC_LNA_POS))
#define RF_GAIN_CTRL3_GC_RMXGM_RATT                             RF_GAIN_CTRL3_GC_RMXGM_RATT
#define RF_GAIN_CTRL3_GC_RMXGM_RATT_POS                         (24U)
#define RF_GAIN_CTRL3_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL3_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL3_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL3_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL3_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL3_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL3_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL3_GC_RMXGM                                  RF_GAIN_CTRL3_GC_RMXGM
#define RF_GAIN_CTRL3_GC_RMXGM_POS                              (27U)
#define RF_GAIN_CTRL3_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL3_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL3_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL3_GC_RMXGM_POS)
#define RF_GAIN_CTRL3_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL3_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL3_GC_RMXGM_POS))
#define RF_GAIN_CTRL3_GC_LNA                                    RF_GAIN_CTRL3_GC_LNA
#define RF_GAIN_CTRL3_GC_LNA_POS                                (29U)
#define RF_GAIN_CTRL3_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL3_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL3_GC_LNA_LEN)-1)<<RF_GAIN_CTRL3_GC_LNA_POS)
#define RF_GAIN_CTRL3_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL3_GC_LNA_LEN)-1)<<RF_GAIN_CTRL3_GC_LNA_POS))

/* 0x104 : rrf_gain_index2 */
#define RF_RRF_GAIN_INDEX2_OFFSET                               (0x104)
#define RF_GAIN_CTRL4_GC_RMXGM_RATT                             RF_GAIN_CTRL4_GC_RMXGM_RATT
#define RF_GAIN_CTRL4_GC_RMXGM_RATT_POS                         (0U)
#define RF_GAIN_CTRL4_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL4_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL4_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL4_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL4_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL4_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL4_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL4_GC_RMXGM                                  RF_GAIN_CTRL4_GC_RMXGM
#define RF_GAIN_CTRL4_GC_RMXGM_POS                              (3U)
#define RF_GAIN_CTRL4_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL4_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL4_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL4_GC_RMXGM_POS)
#define RF_GAIN_CTRL4_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL4_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL4_GC_RMXGM_POS))
#define RF_GAIN_CTRL4_GC_LNA                                    RF_GAIN_CTRL4_GC_LNA
#define RF_GAIN_CTRL4_GC_LNA_POS                                (5U)
#define RF_GAIN_CTRL4_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL4_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL4_GC_LNA_LEN)-1)<<RF_GAIN_CTRL4_GC_LNA_POS)
#define RF_GAIN_CTRL4_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL4_GC_LNA_LEN)-1)<<RF_GAIN_CTRL4_GC_LNA_POS))
#define RF_GAIN_CTRL5_GC_RMXGM_RATT                             RF_GAIN_CTRL5_GC_RMXGM_RATT
#define RF_GAIN_CTRL5_GC_RMXGM_RATT_POS                         (8U)
#define RF_GAIN_CTRL5_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL5_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL5_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL5_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL5_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL5_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL5_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL5_GC_RMXGM                                  RF_GAIN_CTRL5_GC_RMXGM
#define RF_GAIN_CTRL5_GC_RMXGM_POS                              (11U)
#define RF_GAIN_CTRL5_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL5_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL5_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL5_GC_RMXGM_POS)
#define RF_GAIN_CTRL5_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL5_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL5_GC_RMXGM_POS))
#define RF_GAIN_CTRL5_GC_LNA                                    RF_GAIN_CTRL5_GC_LNA
#define RF_GAIN_CTRL5_GC_LNA_POS                                (13U)
#define RF_GAIN_CTRL5_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL5_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL5_GC_LNA_LEN)-1)<<RF_GAIN_CTRL5_GC_LNA_POS)
#define RF_GAIN_CTRL5_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL5_GC_LNA_LEN)-1)<<RF_GAIN_CTRL5_GC_LNA_POS))
#define RF_GAIN_CTRL6_GC_RMXGM_RATT                             RF_GAIN_CTRL6_GC_RMXGM_RATT
#define RF_GAIN_CTRL6_GC_RMXGM_RATT_POS                         (16U)
#define RF_GAIN_CTRL6_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL6_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL6_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL6_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL6_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL6_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL6_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL6_GC_RMXGM                                  RF_GAIN_CTRL6_GC_RMXGM
#define RF_GAIN_CTRL6_GC_RMXGM_POS                              (19U)
#define RF_GAIN_CTRL6_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL6_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL6_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL6_GC_RMXGM_POS)
#define RF_GAIN_CTRL6_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL6_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL6_GC_RMXGM_POS))
#define RF_GAIN_CTRL6_GC_LNA                                    RF_GAIN_CTRL6_GC_LNA
#define RF_GAIN_CTRL6_GC_LNA_POS                                (21U)
#define RF_GAIN_CTRL6_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL6_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL6_GC_LNA_LEN)-1)<<RF_GAIN_CTRL6_GC_LNA_POS)
#define RF_GAIN_CTRL6_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL6_GC_LNA_LEN)-1)<<RF_GAIN_CTRL6_GC_LNA_POS))
#define RF_GAIN_CTRL7_GC_RMXGM_RATT                             RF_GAIN_CTRL7_GC_RMXGM_RATT
#define RF_GAIN_CTRL7_GC_RMXGM_RATT_POS                         (24U)
#define RF_GAIN_CTRL7_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL7_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL7_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL7_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL7_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL7_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL7_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL7_GC_RMXGM                                  RF_GAIN_CTRL7_GC_RMXGM
#define RF_GAIN_CTRL7_GC_RMXGM_POS                              (27U)
#define RF_GAIN_CTRL7_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL7_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL7_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL7_GC_RMXGM_POS)
#define RF_GAIN_CTRL7_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL7_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL7_GC_RMXGM_POS))
#define RF_GAIN_CTRL7_GC_LNA                                    RF_GAIN_CTRL7_GC_LNA
#define RF_GAIN_CTRL7_GC_LNA_POS                                (29U)
#define RF_GAIN_CTRL7_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL7_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL7_GC_LNA_LEN)-1)<<RF_GAIN_CTRL7_GC_LNA_POS)
#define RF_GAIN_CTRL7_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL7_GC_LNA_LEN)-1)<<RF_GAIN_CTRL7_GC_LNA_POS))

/* 0x108 : rrf_gain_index3 */
#define RF_RRF_GAIN_INDEX3_OFFSET                               (0x108)
#define RF_GAIN_CTRL8_GC_RMXGM_RATT                             RF_GAIN_CTRL8_GC_RMXGM_RATT
#define RF_GAIN_CTRL8_GC_RMXGM_RATT_POS                         (0U)
#define RF_GAIN_CTRL8_GC_RMXGM_RATT_LEN                         (3U)
#define RF_GAIN_CTRL8_GC_RMXGM_RATT_MSK                         (((1U<<RF_GAIN_CTRL8_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL8_GC_RMXGM_RATT_POS)
#define RF_GAIN_CTRL8_GC_RMXGM_RATT_UMSK                        (~(((1U<<RF_GAIN_CTRL8_GC_RMXGM_RATT_LEN)-1)<<RF_GAIN_CTRL8_GC_RMXGM_RATT_POS))
#define RF_GAIN_CTRL8_GC_RMXGM                                  RF_GAIN_CTRL8_GC_RMXGM
#define RF_GAIN_CTRL8_GC_RMXGM_POS                              (3U)
#define RF_GAIN_CTRL8_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL8_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL8_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL8_GC_RMXGM_POS)
#define RF_GAIN_CTRL8_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL8_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL8_GC_RMXGM_POS))
#define RF_GAIN_CTRL8_GC_LNA                                    RF_GAIN_CTRL8_GC_LNA
#define RF_GAIN_CTRL8_GC_LNA_POS                                (5U)
#define RF_GAIN_CTRL8_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL8_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL8_GC_LNA_LEN)-1)<<RF_GAIN_CTRL8_GC_LNA_POS)
#define RF_GAIN_CTRL8_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL8_GC_LNA_LEN)-1)<<RF_GAIN_CTRL8_GC_LNA_POS))

/* 0x10C : lna_ctrl_hw_mux */
#define RF_LNA_CTRL_HW_MUX_OFFSET                               (0x10C)
#define RF_LNA_BM_HG                                            RF_LNA_BM_HG
#define RF_LNA_BM_HG_POS                                        (0U)
#define RF_LNA_BM_HG_LEN                                        (4U)
#define RF_LNA_BM_HG_MSK                                        (((1U<<RF_LNA_BM_HG_LEN)-1)<<RF_LNA_BM_HG_POS)
#define RF_LNA_BM_HG_UMSK                                       (~(((1U<<RF_LNA_BM_HG_LEN)-1)<<RF_LNA_BM_HG_POS))
#define RF_LNA_BM_LG                                            RF_LNA_BM_LG
#define RF_LNA_BM_LG_POS                                        (4U)
#define RF_LNA_BM_LG_LEN                                        (4U)
#define RF_LNA_BM_LG_MSK                                        (((1U<<RF_LNA_BM_LG_LEN)-1)<<RF_LNA_BM_LG_POS)
#define RF_LNA_BM_LG_UMSK                                       (~(((1U<<RF_LNA_BM_LG_LEN)-1)<<RF_LNA_BM_LG_POS))
#define RF_LNA_LOAD_CSW_HG                                      RF_LNA_LOAD_CSW_HG
#define RF_LNA_LOAD_CSW_HG_POS                                  (8U)
#define RF_LNA_LOAD_CSW_HG_LEN                                  (4U)
#define RF_LNA_LOAD_CSW_HG_MSK                                  (((1U<<RF_LNA_LOAD_CSW_HG_LEN)-1)<<RF_LNA_LOAD_CSW_HG_POS)
#define RF_LNA_LOAD_CSW_HG_UMSK                                 (~(((1U<<RF_LNA_LOAD_CSW_HG_LEN)-1)<<RF_LNA_LOAD_CSW_HG_POS))
#define RF_LNA_LOAD_CSW_LG                                      RF_LNA_LOAD_CSW_LG
#define RF_LNA_LOAD_CSW_LG_POS                                  (12U)
#define RF_LNA_LOAD_CSW_LG_LEN                                  (4U)
#define RF_LNA_LOAD_CSW_LG_MSK                                  (((1U<<RF_LNA_LOAD_CSW_LG_LEN)-1)<<RF_LNA_LOAD_CSW_LG_POS)
#define RF_LNA_LOAD_CSW_LG_UMSK                                 (~(((1U<<RF_LNA_LOAD_CSW_LG_LEN)-1)<<RF_LNA_LOAD_CSW_LG_POS))

/* 0x110 : rbb_gain_index1 */
#define RF_RBB_GAIN_INDEX1_OFFSET                               (0x110)
#define RF_GAIN_CTRL0_GC_RBB1                                   RF_GAIN_CTRL0_GC_RBB1
#define RF_GAIN_CTRL0_GC_RBB1_POS                               (0U)
#define RF_GAIN_CTRL0_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL0_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL0_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL0_GC_RBB1_POS)
#define RF_GAIN_CTRL0_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL0_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL0_GC_RBB1_POS))
#define RF_GAIN_CTRL0_GC_RBB2                                   RF_GAIN_CTRL0_GC_RBB2
#define RF_GAIN_CTRL0_GC_RBB2_POS                               (4U)
#define RF_GAIN_CTRL0_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL0_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL0_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL0_GC_RBB2_POS)
#define RF_GAIN_CTRL0_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL0_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL0_GC_RBB2_POS))
#define RF_GAIN_CTRL1_GC_RBB1                                   RF_GAIN_CTRL1_GC_RBB1
#define RF_GAIN_CTRL1_GC_RBB1_POS                               (8U)
#define RF_GAIN_CTRL1_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL1_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL1_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL1_GC_RBB1_POS)
#define RF_GAIN_CTRL1_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL1_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL1_GC_RBB1_POS))
#define RF_GAIN_CTRL1_GC_RBB2                                   RF_GAIN_CTRL1_GC_RBB2
#define RF_GAIN_CTRL1_GC_RBB2_POS                               (12U)
#define RF_GAIN_CTRL1_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL1_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL1_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL1_GC_RBB2_POS)
#define RF_GAIN_CTRL1_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL1_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL1_GC_RBB2_POS))
#define RF_GAIN_CTRL2_GC_RBB1                                   RF_GAIN_CTRL2_GC_RBB1
#define RF_GAIN_CTRL2_GC_RBB1_POS                               (16U)
#define RF_GAIN_CTRL2_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL2_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL2_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL2_GC_RBB1_POS)
#define RF_GAIN_CTRL2_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL2_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL2_GC_RBB1_POS))
#define RF_GAIN_CTRL2_GC_RBB2                                   RF_GAIN_CTRL2_GC_RBB2
#define RF_GAIN_CTRL2_GC_RBB2_POS                               (20U)
#define RF_GAIN_CTRL2_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL2_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL2_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL2_GC_RBB2_POS)
#define RF_GAIN_CTRL2_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL2_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL2_GC_RBB2_POS))
#define RF_GAIN_CTRL3_GC_RBB1                                   RF_GAIN_CTRL3_GC_RBB1
#define RF_GAIN_CTRL3_GC_RBB1_POS                               (24U)
#define RF_GAIN_CTRL3_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL3_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL3_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL3_GC_RBB1_POS)
#define RF_GAIN_CTRL3_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL3_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL3_GC_RBB1_POS))
#define RF_GAIN_CTRL3_GC_RBB2                                   RF_GAIN_CTRL3_GC_RBB2
#define RF_GAIN_CTRL3_GC_RBB2_POS                               (28U)
#define RF_GAIN_CTRL3_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL3_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL3_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL3_GC_RBB2_POS)
#define RF_GAIN_CTRL3_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL3_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL3_GC_RBB2_POS))

/* 0x114 : rbb_gain_index2 */
#define RF_RBB_GAIN_INDEX2_OFFSET                               (0x114)
#define RF_GAIN_CTRL4_GC_RBB1                                   RF_GAIN_CTRL4_GC_RBB1
#define RF_GAIN_CTRL4_GC_RBB1_POS                               (0U)
#define RF_GAIN_CTRL4_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL4_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL4_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL4_GC_RBB1_POS)
#define RF_GAIN_CTRL4_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL4_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL4_GC_RBB1_POS))
#define RF_GAIN_CTRL4_GC_RBB2                                   RF_GAIN_CTRL4_GC_RBB2
#define RF_GAIN_CTRL4_GC_RBB2_POS                               (4U)
#define RF_GAIN_CTRL4_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL4_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL4_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL4_GC_RBB2_POS)
#define RF_GAIN_CTRL4_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL4_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL4_GC_RBB2_POS))
#define RF_GAIN_CTRL5_GC_RBB1                                   RF_GAIN_CTRL5_GC_RBB1
#define RF_GAIN_CTRL5_GC_RBB1_POS                               (8U)
#define RF_GAIN_CTRL5_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL5_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL5_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL5_GC_RBB1_POS)
#define RF_GAIN_CTRL5_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL5_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL5_GC_RBB1_POS))
#define RF_GAIN_CTRL5_GC_RBB2                                   RF_GAIN_CTRL5_GC_RBB2
#define RF_GAIN_CTRL5_GC_RBB2_POS                               (12U)
#define RF_GAIN_CTRL5_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL5_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL5_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL5_GC_RBB2_POS)
#define RF_GAIN_CTRL5_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL5_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL5_GC_RBB2_POS))
#define RF_GAIN_CTRL6_GC_RBB1                                   RF_GAIN_CTRL6_GC_RBB1
#define RF_GAIN_CTRL6_GC_RBB1_POS                               (16U)
#define RF_GAIN_CTRL6_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL6_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL6_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL6_GC_RBB1_POS)
#define RF_GAIN_CTRL6_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL6_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL6_GC_RBB1_POS))
#define RF_GAIN_CTRL6_GC_RBB2                                   RF_GAIN_CTRL6_GC_RBB2
#define RF_GAIN_CTRL6_GC_RBB2_POS                               (20U)
#define RF_GAIN_CTRL6_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL6_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL6_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL6_GC_RBB2_POS)
#define RF_GAIN_CTRL6_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL6_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL6_GC_RBB2_POS))
#define RF_GAIN_CTRL7_GC_RBB1                                   RF_GAIN_CTRL7_GC_RBB1
#define RF_GAIN_CTRL7_GC_RBB1_POS                               (24U)
#define RF_GAIN_CTRL7_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL7_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL7_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL7_GC_RBB1_POS)
#define RF_GAIN_CTRL7_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL7_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL7_GC_RBB1_POS))
#define RF_GAIN_CTRL7_GC_RBB2                                   RF_GAIN_CTRL7_GC_RBB2
#define RF_GAIN_CTRL7_GC_RBB2_POS                               (28U)
#define RF_GAIN_CTRL7_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL7_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL7_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL7_GC_RBB2_POS)
#define RF_GAIN_CTRL7_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL7_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL7_GC_RBB2_POS))

/* 0x118 : rbb_gain_index3 */
#define RF_RBB_GAIN_INDEX3_OFFSET                               (0x118)
#define RF_GAIN_CTRL8_GC_RBB1                                   RF_GAIN_CTRL8_GC_RBB1
#define RF_GAIN_CTRL8_GC_RBB1_POS                               (0U)
#define RF_GAIN_CTRL8_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL8_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL8_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL8_GC_RBB1_POS)
#define RF_GAIN_CTRL8_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL8_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL8_GC_RBB1_POS))
#define RF_GAIN_CTRL8_GC_RBB2                                   RF_GAIN_CTRL8_GC_RBB2
#define RF_GAIN_CTRL8_GC_RBB2_POS                               (4U)
#define RF_GAIN_CTRL8_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL8_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL8_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL8_GC_RBB2_POS)
#define RF_GAIN_CTRL8_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL8_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL8_GC_RBB2_POS))
#define RF_GAIN_CTRL9_GC_RBB1                                   RF_GAIN_CTRL9_GC_RBB1
#define RF_GAIN_CTRL9_GC_RBB1_POS                               (8U)
#define RF_GAIN_CTRL9_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL9_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL9_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL9_GC_RBB1_POS)
#define RF_GAIN_CTRL9_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL9_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL9_GC_RBB1_POS))
#define RF_GAIN_CTRL9_GC_RBB2                                   RF_GAIN_CTRL9_GC_RBB2
#define RF_GAIN_CTRL9_GC_RBB2_POS                               (12U)
#define RF_GAIN_CTRL9_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL9_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL9_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL9_GC_RBB2_POS)
#define RF_GAIN_CTRL9_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL9_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL9_GC_RBB2_POS))
#define RF_GAIN_CTRL10_GC_RBB1                                  RF_GAIN_CTRL10_GC_RBB1
#define RF_GAIN_CTRL10_GC_RBB1_POS                              (16U)
#define RF_GAIN_CTRL10_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL10_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL10_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL10_GC_RBB1_POS)
#define RF_GAIN_CTRL10_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL10_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL10_GC_RBB1_POS))
#define RF_GAIN_CTRL10_GC_RBB2                                  RF_GAIN_CTRL10_GC_RBB2
#define RF_GAIN_CTRL10_GC_RBB2_POS                              (20U)
#define RF_GAIN_CTRL10_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL10_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL10_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL10_GC_RBB2_POS)
#define RF_GAIN_CTRL10_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL10_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL10_GC_RBB2_POS))
#define RF_GAIN_CTRL11_GC_RBB1                                  RF_GAIN_CTRL11_GC_RBB1
#define RF_GAIN_CTRL11_GC_RBB1_POS                              (24U)
#define RF_GAIN_CTRL11_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL11_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL11_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL11_GC_RBB1_POS)
#define RF_GAIN_CTRL11_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL11_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL11_GC_RBB1_POS))
#define RF_GAIN_CTRL11_GC_RBB2                                  RF_GAIN_CTRL11_GC_RBB2
#define RF_GAIN_CTRL11_GC_RBB2_POS                              (28U)
#define RF_GAIN_CTRL11_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL11_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL11_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL11_GC_RBB2_POS)
#define RF_GAIN_CTRL11_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL11_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL11_GC_RBB2_POS))

/* 0x11C : rbb_gain_index4 */
#define RF_RBB_GAIN_INDEX4_OFFSET                               (0x11C)
#define RF_GAIN_CTRL12_GC_RBB1                                  RF_GAIN_CTRL12_GC_RBB1
#define RF_GAIN_CTRL12_GC_RBB1_POS                              (0U)
#define RF_GAIN_CTRL12_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL12_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL12_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL12_GC_RBB1_POS)
#define RF_GAIN_CTRL12_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL12_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL12_GC_RBB1_POS))
#define RF_GAIN_CTRL12_GC_RBB2                                  RF_GAIN_CTRL12_GC_RBB2
#define RF_GAIN_CTRL12_GC_RBB2_POS                              (4U)
#define RF_GAIN_CTRL12_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL12_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL12_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL12_GC_RBB2_POS)
#define RF_GAIN_CTRL12_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL12_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL12_GC_RBB2_POS))
#define RF_GAIN_CTRL13_GC_RBB1                                  RF_GAIN_CTRL13_GC_RBB1
#define RF_GAIN_CTRL13_GC_RBB1_POS                              (8U)
#define RF_GAIN_CTRL13_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL13_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL13_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL13_GC_RBB1_POS)
#define RF_GAIN_CTRL13_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL13_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL13_GC_RBB1_POS))
#define RF_GAIN_CTRL13_GC_RBB2                                  RF_GAIN_CTRL13_GC_RBB2
#define RF_GAIN_CTRL13_GC_RBB2_POS                              (12U)
#define RF_GAIN_CTRL13_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL13_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL13_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL13_GC_RBB2_POS)
#define RF_GAIN_CTRL13_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL13_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL13_GC_RBB2_POS))
#define RF_GAIN_CTRL14_GC_RBB1                                  RF_GAIN_CTRL14_GC_RBB1
#define RF_GAIN_CTRL14_GC_RBB1_POS                              (16U)
#define RF_GAIN_CTRL14_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL14_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL14_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL14_GC_RBB1_POS)
#define RF_GAIN_CTRL14_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL14_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL14_GC_RBB1_POS))
#define RF_GAIN_CTRL14_GC_RBB2                                  RF_GAIN_CTRL14_GC_RBB2
#define RF_GAIN_CTRL14_GC_RBB2_POS                              (20U)
#define RF_GAIN_CTRL14_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL14_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL14_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL14_GC_RBB2_POS)
#define RF_GAIN_CTRL14_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL14_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL14_GC_RBB2_POS))
#define RF_GAIN_CTRL15_GC_RBB1                                  RF_GAIN_CTRL15_GC_RBB1
#define RF_GAIN_CTRL15_GC_RBB1_POS                              (24U)
#define RF_GAIN_CTRL15_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL15_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL15_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL15_GC_RBB1_POS)
#define RF_GAIN_CTRL15_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL15_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL15_GC_RBB1_POS))
#define RF_GAIN_CTRL15_GC_RBB2                                  RF_GAIN_CTRL15_GC_RBB2
#define RF_GAIN_CTRL15_GC_RBB2_POS                              (28U)
#define RF_GAIN_CTRL15_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL15_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL15_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL15_GC_RBB2_POS)
#define RF_GAIN_CTRL15_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL15_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL15_GC_RBB2_POS))

/* 0x120 : rbb_gain_index5 */
#define RF_RBB_GAIN_INDEX5_OFFSET                               (0x120)
#define RF_GAIN_CTRL16_GC_RBB1                                  RF_GAIN_CTRL16_GC_RBB1
#define RF_GAIN_CTRL16_GC_RBB1_POS                              (0U)
#define RF_GAIN_CTRL16_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL16_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL16_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL16_GC_RBB1_POS)
#define RF_GAIN_CTRL16_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL16_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL16_GC_RBB1_POS))
#define RF_GAIN_CTRL16_GC_RBB2                                  RF_GAIN_CTRL16_GC_RBB2
#define RF_GAIN_CTRL16_GC_RBB2_POS                              (4U)
#define RF_GAIN_CTRL16_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL16_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL16_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL16_GC_RBB2_POS)
#define RF_GAIN_CTRL16_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL16_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL16_GC_RBB2_POS))

/* 0x134 : adda_reg_ctrl_hw */
#define RF_ADDA_REG_CTRL_HW_OFFSET                              (0x134)
#define RF_ADDA_LDO_DVDD_SEL_RX                                 RF_ADDA_LDO_DVDD_SEL_RX
#define RF_ADDA_LDO_DVDD_SEL_RX_POS                             (0U)
#define RF_ADDA_LDO_DVDD_SEL_RX_LEN                             (3U)
#define RF_ADDA_LDO_DVDD_SEL_RX_MSK                             (((1U<<RF_ADDA_LDO_DVDD_SEL_RX_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_RX_POS)
#define RF_ADDA_LDO_DVDD_SEL_RX_UMSK                            (~(((1U<<RF_ADDA_LDO_DVDD_SEL_RX_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_RX_POS))
#define RF_ADDA_LDO_DVDD_SEL_TX                                 RF_ADDA_LDO_DVDD_SEL_TX
#define RF_ADDA_LDO_DVDD_SEL_TX_POS                             (4U)
#define RF_ADDA_LDO_DVDD_SEL_TX_LEN                             (3U)
#define RF_ADDA_LDO_DVDD_SEL_TX_MSK                             (((1U<<RF_ADDA_LDO_DVDD_SEL_TX_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_TX_POS)
#define RF_ADDA_LDO_DVDD_SEL_TX_UMSK                            (~(((1U<<RF_ADDA_LDO_DVDD_SEL_TX_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_TX_POS))

/* 0x138 : lo_reg_ctrl_hw1 */
#define RF_LO_REG_CTRL_HW1_OFFSET                               (0x138)
#define RF_LO_FBDV_HALFSTEP_EN_RX                               RF_LO_FBDV_HALFSTEP_EN_RX
#define RF_LO_FBDV_HALFSTEP_EN_RX_POS                           (0U)
#define RF_LO_FBDV_HALFSTEP_EN_RX_LEN                           (1U)
#define RF_LO_FBDV_HALFSTEP_EN_RX_MSK                           (((1U<<RF_LO_FBDV_HALFSTEP_EN_RX_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_RX_POS)
#define RF_LO_FBDV_HALFSTEP_EN_RX_UMSK                          (~(((1U<<RF_LO_FBDV_HALFSTEP_EN_RX_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_RX_POS))
#define RF_LO_FBDV_HALFSTEP_EN_TX                               RF_LO_FBDV_HALFSTEP_EN_TX
#define RF_LO_FBDV_HALFSTEP_EN_TX_POS                           (1U)
#define RF_LO_FBDV_HALFSTEP_EN_TX_LEN                           (1U)
#define RF_LO_FBDV_HALFSTEP_EN_TX_MSK                           (((1U<<RF_LO_FBDV_HALFSTEP_EN_TX_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_TX_POS)
#define RF_LO_FBDV_HALFSTEP_EN_TX_UMSK                          (~(((1U<<RF_LO_FBDV_HALFSTEP_EN_TX_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_TX_POS))
#define RF_LO_CP_SEL_RX                                         RF_LO_CP_SEL_RX
#define RF_LO_CP_SEL_RX_POS                                     (2U)
#define RF_LO_CP_SEL_RX_LEN                                     (1U)
#define RF_LO_CP_SEL_RX_MSK                                     (((1U<<RF_LO_CP_SEL_RX_LEN)-1)<<RF_LO_CP_SEL_RX_POS)
#define RF_LO_CP_SEL_RX_UMSK                                    (~(((1U<<RF_LO_CP_SEL_RX_LEN)-1)<<RF_LO_CP_SEL_RX_POS))
#define RF_LO_CP_SEL_TX                                         RF_LO_CP_SEL_TX
#define RF_LO_CP_SEL_TX_POS                                     (3U)
#define RF_LO_CP_SEL_TX_LEN                                     (1U)
#define RF_LO_CP_SEL_TX_MSK                                     (((1U<<RF_LO_CP_SEL_TX_LEN)-1)<<RF_LO_CP_SEL_TX_POS)
#define RF_LO_CP_SEL_TX_UMSK                                    (~(((1U<<RF_LO_CP_SEL_TX_LEN)-1)<<RF_LO_CP_SEL_TX_POS))
#define RF_LO_LF_CZ_RX                                          RF_LO_LF_CZ_RX
#define RF_LO_LF_CZ_RX_POS                                      (4U)
#define RF_LO_LF_CZ_RX_LEN                                      (2U)
#define RF_LO_LF_CZ_RX_MSK                                      (((1U<<RF_LO_LF_CZ_RX_LEN)-1)<<RF_LO_LF_CZ_RX_POS)
#define RF_LO_LF_CZ_RX_UMSK                                     (~(((1U<<RF_LO_LF_CZ_RX_LEN)-1)<<RF_LO_LF_CZ_RX_POS))
#define RF_LO_LF_CZ_TX                                          RF_LO_LF_CZ_TX
#define RF_LO_LF_CZ_TX_POS                                      (8U)
#define RF_LO_LF_CZ_TX_LEN                                      (2U)
#define RF_LO_LF_CZ_TX_MSK                                      (((1U<<RF_LO_LF_CZ_TX_LEN)-1)<<RF_LO_LF_CZ_TX_POS)
#define RF_LO_LF_CZ_TX_UMSK                                     (~(((1U<<RF_LO_LF_CZ_TX_LEN)-1)<<RF_LO_LF_CZ_TX_POS))
#define RF_LO_LF_RZ_RX                                          RF_LO_LF_RZ_RX
#define RF_LO_LF_RZ_RX_POS                                      (12U)
#define RF_LO_LF_RZ_RX_LEN                                      (3U)
#define RF_LO_LF_RZ_RX_MSK                                      (((1U<<RF_LO_LF_RZ_RX_LEN)-1)<<RF_LO_LF_RZ_RX_POS)
#define RF_LO_LF_RZ_RX_UMSK                                     (~(((1U<<RF_LO_LF_RZ_RX_LEN)-1)<<RF_LO_LF_RZ_RX_POS))
#define RF_LO_LF_RZ_TX                                          RF_LO_LF_RZ_TX
#define RF_LO_LF_RZ_TX_POS                                      (16U)
#define RF_LO_LF_RZ_TX_LEN                                      (3U)
#define RF_LO_LF_RZ_TX_MSK                                      (((1U<<RF_LO_LF_RZ_TX_LEN)-1)<<RF_LO_LF_RZ_TX_POS)
#define RF_LO_LF_RZ_TX_UMSK                                     (~(((1U<<RF_LO_LF_RZ_TX_LEN)-1)<<RF_LO_LF_RZ_TX_POS))
#define RF_LO_LF_R4_RX                                          RF_LO_LF_R4_RX
#define RF_LO_LF_R4_RX_POS                                      (20U)
#define RF_LO_LF_R4_RX_LEN                                      (2U)
#define RF_LO_LF_R4_RX_MSK                                      (((1U<<RF_LO_LF_R4_RX_LEN)-1)<<RF_LO_LF_R4_RX_POS)
#define RF_LO_LF_R4_RX_UMSK                                     (~(((1U<<RF_LO_LF_R4_RX_LEN)-1)<<RF_LO_LF_R4_RX_POS))
#define RF_LO_LF_R4_TX                                          RF_LO_LF_R4_TX
#define RF_LO_LF_R4_TX_POS                                      (24U)
#define RF_LO_LF_R4_TX_LEN                                      (2U)
#define RF_LO_LF_R4_TX_MSK                                      (((1U<<RF_LO_LF_R4_TX_LEN)-1)<<RF_LO_LF_R4_TX_POS)
#define RF_LO_LF_R4_TX_UMSK                                     (~(((1U<<RF_LO_LF_R4_TX_LEN)-1)<<RF_LO_LF_R4_TX_POS))

/* 0x13C : lo_cal_ctrl_hw1 */
#define RF_LO_CAL_CTRL_HW1_OFFSET                               (0x13C)
#define RF_LO_VCO_IDAC_CW_2404                                  RF_LO_VCO_IDAC_CW_2404
#define RF_LO_VCO_IDAC_CW_2404_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2404_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2404_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2404_LEN)-1)<<RF_LO_VCO_IDAC_CW_2404_POS)
#define RF_LO_VCO_IDAC_CW_2404_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2404_LEN)-1)<<RF_LO_VCO_IDAC_CW_2404_POS))
#define RF_LO_VCO_FREQ_CW_2404                                  RF_LO_VCO_FREQ_CW_2404
#define RF_LO_VCO_FREQ_CW_2404_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2404_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2404_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2404_LEN)-1)<<RF_LO_VCO_FREQ_CW_2404_POS)
#define RF_LO_VCO_FREQ_CW_2404_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2404_LEN)-1)<<RF_LO_VCO_FREQ_CW_2404_POS))
#define RF_LO_VCO_IDAC_CW_2408                                  RF_LO_VCO_IDAC_CW_2408
#define RF_LO_VCO_IDAC_CW_2408_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2408_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2408_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2408_LEN)-1)<<RF_LO_VCO_IDAC_CW_2408_POS)
#define RF_LO_VCO_IDAC_CW_2408_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2408_LEN)-1)<<RF_LO_VCO_IDAC_CW_2408_POS))
#define RF_LO_VCO_FREQ_CW_2408                                  RF_LO_VCO_FREQ_CW_2408
#define RF_LO_VCO_FREQ_CW_2408_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2408_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2408_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2408_LEN)-1)<<RF_LO_VCO_FREQ_CW_2408_POS)
#define RF_LO_VCO_FREQ_CW_2408_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2408_LEN)-1)<<RF_LO_VCO_FREQ_CW_2408_POS))

/* 0x140 : lo_cal_ctrl_hw2 */
#define RF_LO_CAL_CTRL_HW2_OFFSET                               (0x140)
#define RF_LO_VCO_IDAC_CW_2412                                  RF_LO_VCO_IDAC_CW_2412
#define RF_LO_VCO_IDAC_CW_2412_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2412_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2412_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2412_LEN)-1)<<RF_LO_VCO_IDAC_CW_2412_POS)
#define RF_LO_VCO_IDAC_CW_2412_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2412_LEN)-1)<<RF_LO_VCO_IDAC_CW_2412_POS))
#define RF_LO_VCO_FREQ_CW_2412                                  RF_LO_VCO_FREQ_CW_2412
#define RF_LO_VCO_FREQ_CW_2412_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2412_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2412_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2412_LEN)-1)<<RF_LO_VCO_FREQ_CW_2412_POS)
#define RF_LO_VCO_FREQ_CW_2412_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2412_LEN)-1)<<RF_LO_VCO_FREQ_CW_2412_POS))
#define RF_LO_VCO_IDAC_CW_2416                                  RF_LO_VCO_IDAC_CW_2416
#define RF_LO_VCO_IDAC_CW_2416_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2416_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2416_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2416_LEN)-1)<<RF_LO_VCO_IDAC_CW_2416_POS)
#define RF_LO_VCO_IDAC_CW_2416_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2416_LEN)-1)<<RF_LO_VCO_IDAC_CW_2416_POS))
#define RF_LO_VCO_FREQ_CW_2416                                  RF_LO_VCO_FREQ_CW_2416
#define RF_LO_VCO_FREQ_CW_2416_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2416_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2416_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2416_LEN)-1)<<RF_LO_VCO_FREQ_CW_2416_POS)
#define RF_LO_VCO_FREQ_CW_2416_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2416_LEN)-1)<<RF_LO_VCO_FREQ_CW_2416_POS))

/* 0x144 : lo_cal_ctrl_hw3 */
#define RF_LO_CAL_CTRL_HW3_OFFSET                               (0x144)
#define RF_LO_VCO_IDAC_CW_2420                                  RF_LO_VCO_IDAC_CW_2420
#define RF_LO_VCO_IDAC_CW_2420_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2420_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2420_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2420_LEN)-1)<<RF_LO_VCO_IDAC_CW_2420_POS)
#define RF_LO_VCO_IDAC_CW_2420_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2420_LEN)-1)<<RF_LO_VCO_IDAC_CW_2420_POS))
#define RF_LO_VCO_FREQ_CW_2420                                  RF_LO_VCO_FREQ_CW_2420
#define RF_LO_VCO_FREQ_CW_2420_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2420_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2420_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2420_LEN)-1)<<RF_LO_VCO_FREQ_CW_2420_POS)
#define RF_LO_VCO_FREQ_CW_2420_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2420_LEN)-1)<<RF_LO_VCO_FREQ_CW_2420_POS))
#define RF_LO_VCO_IDAC_CW_2424                                  RF_LO_VCO_IDAC_CW_2424
#define RF_LO_VCO_IDAC_CW_2424_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2424_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2424_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2424_LEN)-1)<<RF_LO_VCO_IDAC_CW_2424_POS)
#define RF_LO_VCO_IDAC_CW_2424_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2424_LEN)-1)<<RF_LO_VCO_IDAC_CW_2424_POS))
#define RF_LO_VCO_FREQ_CW_2424                                  RF_LO_VCO_FREQ_CW_2424
#define RF_LO_VCO_FREQ_CW_2424_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2424_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2424_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2424_LEN)-1)<<RF_LO_VCO_FREQ_CW_2424_POS)
#define RF_LO_VCO_FREQ_CW_2424_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2424_LEN)-1)<<RF_LO_VCO_FREQ_CW_2424_POS))

/* 0x148 : lo_cal_ctrl_hw4 */
#define RF_LO_CAL_CTRL_HW4_OFFSET                               (0x148)
#define RF_LO_VCO_IDAC_CW_2428                                  RF_LO_VCO_IDAC_CW_2428
#define RF_LO_VCO_IDAC_CW_2428_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2428_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2428_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2428_LEN)-1)<<RF_LO_VCO_IDAC_CW_2428_POS)
#define RF_LO_VCO_IDAC_CW_2428_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2428_LEN)-1)<<RF_LO_VCO_IDAC_CW_2428_POS))
#define RF_LO_VCO_FREQ_CW_2428                                  RF_LO_VCO_FREQ_CW_2428
#define RF_LO_VCO_FREQ_CW_2428_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2428_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2428_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2428_LEN)-1)<<RF_LO_VCO_FREQ_CW_2428_POS)
#define RF_LO_VCO_FREQ_CW_2428_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2428_LEN)-1)<<RF_LO_VCO_FREQ_CW_2428_POS))
#define RF_LO_VCO_IDAC_CW_2432                                  RF_LO_VCO_IDAC_CW_2432
#define RF_LO_VCO_IDAC_CW_2432_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2432_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2432_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2432_LEN)-1)<<RF_LO_VCO_IDAC_CW_2432_POS)
#define RF_LO_VCO_IDAC_CW_2432_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2432_LEN)-1)<<RF_LO_VCO_IDAC_CW_2432_POS))
#define RF_LO_VCO_FREQ_CW_2432                                  RF_LO_VCO_FREQ_CW_2432
#define RF_LO_VCO_FREQ_CW_2432_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2432_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2432_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2432_LEN)-1)<<RF_LO_VCO_FREQ_CW_2432_POS)
#define RF_LO_VCO_FREQ_CW_2432_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2432_LEN)-1)<<RF_LO_VCO_FREQ_CW_2432_POS))

/* 0x14C : lo_cal_ctrl_hw5 */
#define RF_LO_CAL_CTRL_HW5_OFFSET                               (0x14C)
#define RF_LO_VCO_IDAC_CW_2436                                  RF_LO_VCO_IDAC_CW_2436
#define RF_LO_VCO_IDAC_CW_2436_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2436_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2436_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2436_LEN)-1)<<RF_LO_VCO_IDAC_CW_2436_POS)
#define RF_LO_VCO_IDAC_CW_2436_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2436_LEN)-1)<<RF_LO_VCO_IDAC_CW_2436_POS))
#define RF_LO_VCO_FREQ_CW_2436                                  RF_LO_VCO_FREQ_CW_2436
#define RF_LO_VCO_FREQ_CW_2436_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2436_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2436_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2436_LEN)-1)<<RF_LO_VCO_FREQ_CW_2436_POS)
#define RF_LO_VCO_FREQ_CW_2436_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2436_LEN)-1)<<RF_LO_VCO_FREQ_CW_2436_POS))
#define RF_LO_VCO_IDAC_CW_2440                                  RF_LO_VCO_IDAC_CW_2440
#define RF_LO_VCO_IDAC_CW_2440_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2440_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2440_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2440_LEN)-1)<<RF_LO_VCO_IDAC_CW_2440_POS)
#define RF_LO_VCO_IDAC_CW_2440_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2440_LEN)-1)<<RF_LO_VCO_IDAC_CW_2440_POS))
#define RF_LO_VCO_FREQ_CW_2440                                  RF_LO_VCO_FREQ_CW_2440
#define RF_LO_VCO_FREQ_CW_2440_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2440_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2440_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2440_LEN)-1)<<RF_LO_VCO_FREQ_CW_2440_POS)
#define RF_LO_VCO_FREQ_CW_2440_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2440_LEN)-1)<<RF_LO_VCO_FREQ_CW_2440_POS))

/* 0x150 : lo_cal_ctrl_hw6 */
#define RF_LO_CAL_CTRL_HW6_OFFSET                               (0x150)
#define RF_LO_VCO_IDAC_CW_2444                                  RF_LO_VCO_IDAC_CW_2444
#define RF_LO_VCO_IDAC_CW_2444_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2444_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2444_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2444_LEN)-1)<<RF_LO_VCO_IDAC_CW_2444_POS)
#define RF_LO_VCO_IDAC_CW_2444_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2444_LEN)-1)<<RF_LO_VCO_IDAC_CW_2444_POS))
#define RF_LO_VCO_FREQ_CW_2444                                  RF_LO_VCO_FREQ_CW_2444
#define RF_LO_VCO_FREQ_CW_2444_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2444_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2444_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2444_LEN)-1)<<RF_LO_VCO_FREQ_CW_2444_POS)
#define RF_LO_VCO_FREQ_CW_2444_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2444_LEN)-1)<<RF_LO_VCO_FREQ_CW_2444_POS))
#define RF_LO_VCO_IDAC_CW_2448                                  RF_LO_VCO_IDAC_CW_2448
#define RF_LO_VCO_IDAC_CW_2448_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2448_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2448_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2448_LEN)-1)<<RF_LO_VCO_IDAC_CW_2448_POS)
#define RF_LO_VCO_IDAC_CW_2448_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2448_LEN)-1)<<RF_LO_VCO_IDAC_CW_2448_POS))
#define RF_LO_VCO_FREQ_CW_2448                                  RF_LO_VCO_FREQ_CW_2448
#define RF_LO_VCO_FREQ_CW_2448_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2448_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2448_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2448_LEN)-1)<<RF_LO_VCO_FREQ_CW_2448_POS)
#define RF_LO_VCO_FREQ_CW_2448_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2448_LEN)-1)<<RF_LO_VCO_FREQ_CW_2448_POS))

/* 0x154 : lo_cal_ctrl_hw7 */
#define RF_LO_CAL_CTRL_HW7_OFFSET                               (0x154)
#define RF_LO_VCO_IDAC_CW_2452                                  RF_LO_VCO_IDAC_CW_2452
#define RF_LO_VCO_IDAC_CW_2452_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2452_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2452_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2452_LEN)-1)<<RF_LO_VCO_IDAC_CW_2452_POS)
#define RF_LO_VCO_IDAC_CW_2452_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2452_LEN)-1)<<RF_LO_VCO_IDAC_CW_2452_POS))
#define RF_LO_VCO_FREQ_CW_2452                                  RF_LO_VCO_FREQ_CW_2452
#define RF_LO_VCO_FREQ_CW_2452_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2452_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2452_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2452_LEN)-1)<<RF_LO_VCO_FREQ_CW_2452_POS)
#define RF_LO_VCO_FREQ_CW_2452_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2452_LEN)-1)<<RF_LO_VCO_FREQ_CW_2452_POS))
#define RF_LO_VCO_IDAC_CW_2456                                  RF_LO_VCO_IDAC_CW_2456
#define RF_LO_VCO_IDAC_CW_2456_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2456_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2456_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2456_LEN)-1)<<RF_LO_VCO_IDAC_CW_2456_POS)
#define RF_LO_VCO_IDAC_CW_2456_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2456_LEN)-1)<<RF_LO_VCO_IDAC_CW_2456_POS))
#define RF_LO_VCO_FREQ_CW_2456                                  RF_LO_VCO_FREQ_CW_2456
#define RF_LO_VCO_FREQ_CW_2456_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2456_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2456_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2456_LEN)-1)<<RF_LO_VCO_FREQ_CW_2456_POS)
#define RF_LO_VCO_FREQ_CW_2456_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2456_LEN)-1)<<RF_LO_VCO_FREQ_CW_2456_POS))

/* 0x158 : lo_cal_ctrl_hw8 */
#define RF_LO_CAL_CTRL_HW8_OFFSET                               (0x158)
#define RF_LO_VCO_IDAC_CW_2460                                  RF_LO_VCO_IDAC_CW_2460
#define RF_LO_VCO_IDAC_CW_2460_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2460_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2460_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2460_LEN)-1)<<RF_LO_VCO_IDAC_CW_2460_POS)
#define RF_LO_VCO_IDAC_CW_2460_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2460_LEN)-1)<<RF_LO_VCO_IDAC_CW_2460_POS))
#define RF_LO_VCO_FREQ_CW_2460                                  RF_LO_VCO_FREQ_CW_2460
#define RF_LO_VCO_FREQ_CW_2460_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2460_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2460_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2460_LEN)-1)<<RF_LO_VCO_FREQ_CW_2460_POS)
#define RF_LO_VCO_FREQ_CW_2460_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2460_LEN)-1)<<RF_LO_VCO_FREQ_CW_2460_POS))
#define RF_LO_VCO_IDAC_CW_2464                                  RF_LO_VCO_IDAC_CW_2464
#define RF_LO_VCO_IDAC_CW_2464_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2464_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2464_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2464_LEN)-1)<<RF_LO_VCO_IDAC_CW_2464_POS)
#define RF_LO_VCO_IDAC_CW_2464_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2464_LEN)-1)<<RF_LO_VCO_IDAC_CW_2464_POS))
#define RF_LO_VCO_FREQ_CW_2464                                  RF_LO_VCO_FREQ_CW_2464
#define RF_LO_VCO_FREQ_CW_2464_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2464_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2464_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2464_LEN)-1)<<RF_LO_VCO_FREQ_CW_2464_POS)
#define RF_LO_VCO_FREQ_CW_2464_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2464_LEN)-1)<<RF_LO_VCO_FREQ_CW_2464_POS))

/* 0x15C : lo_cal_ctrl_hw9 */
#define RF_LO_CAL_CTRL_HW9_OFFSET                               (0x15C)
#define RF_LO_VCO_IDAC_CW_2468                                  RF_LO_VCO_IDAC_CW_2468
#define RF_LO_VCO_IDAC_CW_2468_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2468_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2468_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2468_LEN)-1)<<RF_LO_VCO_IDAC_CW_2468_POS)
#define RF_LO_VCO_IDAC_CW_2468_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2468_LEN)-1)<<RF_LO_VCO_IDAC_CW_2468_POS))
#define RF_LO_VCO_FREQ_CW_2468                                  RF_LO_VCO_FREQ_CW_2468
#define RF_LO_VCO_FREQ_CW_2468_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2468_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2468_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2468_LEN)-1)<<RF_LO_VCO_FREQ_CW_2468_POS)
#define RF_LO_VCO_FREQ_CW_2468_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2468_LEN)-1)<<RF_LO_VCO_FREQ_CW_2468_POS))
#define RF_LO_VCO_IDAC_CW_2472                                  RF_LO_VCO_IDAC_CW_2472
#define RF_LO_VCO_IDAC_CW_2472_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2472_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2472_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2472_LEN)-1)<<RF_LO_VCO_IDAC_CW_2472_POS)
#define RF_LO_VCO_IDAC_CW_2472_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2472_LEN)-1)<<RF_LO_VCO_IDAC_CW_2472_POS))
#define RF_LO_VCO_FREQ_CW_2472                                  RF_LO_VCO_FREQ_CW_2472
#define RF_LO_VCO_FREQ_CW_2472_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2472_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2472_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2472_LEN)-1)<<RF_LO_VCO_FREQ_CW_2472_POS)
#define RF_LO_VCO_FREQ_CW_2472_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2472_LEN)-1)<<RF_LO_VCO_FREQ_CW_2472_POS))

/* 0x160 : lo_cal_ctrl_hw10 */
#define RF_LO_CAL_CTRL_HW10_OFFSET                              (0x160)
#define RF_LO_VCO_IDAC_CW_2476                                  RF_LO_VCO_IDAC_CW_2476
#define RF_LO_VCO_IDAC_CW_2476_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2476_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2476_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2476_LEN)-1)<<RF_LO_VCO_IDAC_CW_2476_POS)
#define RF_LO_VCO_IDAC_CW_2476_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2476_LEN)-1)<<RF_LO_VCO_IDAC_CW_2476_POS))
#define RF_LO_VCO_FREQ_CW_2476                                  RF_LO_VCO_FREQ_CW_2476
#define RF_LO_VCO_FREQ_CW_2476_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2476_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2476_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2476_LEN)-1)<<RF_LO_VCO_FREQ_CW_2476_POS)
#define RF_LO_VCO_FREQ_CW_2476_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2476_LEN)-1)<<RF_LO_VCO_FREQ_CW_2476_POS))
#define RF_LO_VCO_IDAC_CW_2480                                  RF_LO_VCO_IDAC_CW_2480
#define RF_LO_VCO_IDAC_CW_2480_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2480_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2480_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2480_LEN)-1)<<RF_LO_VCO_IDAC_CW_2480_POS)
#define RF_LO_VCO_IDAC_CW_2480_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2480_LEN)-1)<<RF_LO_VCO_IDAC_CW_2480_POS))
#define RF_LO_VCO_FREQ_CW_2480                                  RF_LO_VCO_FREQ_CW_2480
#define RF_LO_VCO_FREQ_CW_2480_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2480_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2480_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2480_LEN)-1)<<RF_LO_VCO_FREQ_CW_2480_POS)
#define RF_LO_VCO_FREQ_CW_2480_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2480_LEN)-1)<<RF_LO_VCO_FREQ_CW_2480_POS))

/* 0x164 : lo_cal_ctrl_hw11 */
#define RF_LO_CAL_CTRL_HW11_OFFSET                              (0x164)
#define RF_LO_VCO_IDAC_CW_2484                                  RF_LO_VCO_IDAC_CW_2484
#define RF_LO_VCO_IDAC_CW_2484_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2484_LEN                              (6U)
#define RF_LO_VCO_IDAC_CW_2484_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2484_LEN)-1)<<RF_LO_VCO_IDAC_CW_2484_POS)
#define RF_LO_VCO_IDAC_CW_2484_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2484_LEN)-1)<<RF_LO_VCO_IDAC_CW_2484_POS))
#define RF_LO_VCO_FREQ_CW_2484                                  RF_LO_VCO_FREQ_CW_2484
#define RF_LO_VCO_FREQ_CW_2484_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2484_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2484_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2484_LEN)-1)<<RF_LO_VCO_FREQ_CW_2484_POS)
#define RF_LO_VCO_FREQ_CW_2484_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2484_LEN)-1)<<RF_LO_VCO_FREQ_CW_2484_POS))

/* 0x168 : rosdac_ctrl_hw1 */
#define RF_ROSDAC_CTRL_HW1_OFFSET                               (0x168)
#define RF_ROSDAC_I_GC0                                         RF_ROSDAC_I_GC0
#define RF_ROSDAC_I_GC0_POS                                     (0U)
#define RF_ROSDAC_I_GC0_LEN                                     (6U)
#define RF_ROSDAC_I_GC0_MSK                                     (((1U<<RF_ROSDAC_I_GC0_LEN)-1)<<RF_ROSDAC_I_GC0_POS)
#define RF_ROSDAC_I_GC0_UMSK                                    (~(((1U<<RF_ROSDAC_I_GC0_LEN)-1)<<RF_ROSDAC_I_GC0_POS))
#define RF_ROSDAC_Q_GC0                                         RF_ROSDAC_Q_GC0
#define RF_ROSDAC_Q_GC0_POS                                     (8U)
#define RF_ROSDAC_Q_GC0_LEN                                     (6U)
#define RF_ROSDAC_Q_GC0_MSK                                     (((1U<<RF_ROSDAC_Q_GC0_LEN)-1)<<RF_ROSDAC_Q_GC0_POS)
#define RF_ROSDAC_Q_GC0_UMSK                                    (~(((1U<<RF_ROSDAC_Q_GC0_LEN)-1)<<RF_ROSDAC_Q_GC0_POS))
#define RF_ROSDAC_I_GC1                                         RF_ROSDAC_I_GC1
#define RF_ROSDAC_I_GC1_POS                                     (16U)
#define RF_ROSDAC_I_GC1_LEN                                     (6U)
#define RF_ROSDAC_I_GC1_MSK                                     (((1U<<RF_ROSDAC_I_GC1_LEN)-1)<<RF_ROSDAC_I_GC1_POS)
#define RF_ROSDAC_I_GC1_UMSK                                    (~(((1U<<RF_ROSDAC_I_GC1_LEN)-1)<<RF_ROSDAC_I_GC1_POS))
#define RF_ROSDAC_Q_GC1                                         RF_ROSDAC_Q_GC1
#define RF_ROSDAC_Q_GC1_POS                                     (24U)
#define RF_ROSDAC_Q_GC1_LEN                                     (6U)
#define RF_ROSDAC_Q_GC1_MSK                                     (((1U<<RF_ROSDAC_Q_GC1_LEN)-1)<<RF_ROSDAC_Q_GC1_POS)
#define RF_ROSDAC_Q_GC1_UMSK                                    (~(((1U<<RF_ROSDAC_Q_GC1_LEN)-1)<<RF_ROSDAC_Q_GC1_POS))

/* 0x16C : rosdac_ctrl_hw2 */
#define RF_ROSDAC_CTRL_HW2_OFFSET                               (0x16C)
#define RF_ROSDAC_I_GC2                                         RF_ROSDAC_I_GC2
#define RF_ROSDAC_I_GC2_POS                                     (0U)
#define RF_ROSDAC_I_GC2_LEN                                     (6U)
#define RF_ROSDAC_I_GC2_MSK                                     (((1U<<RF_ROSDAC_I_GC2_LEN)-1)<<RF_ROSDAC_I_GC2_POS)
#define RF_ROSDAC_I_GC2_UMSK                                    (~(((1U<<RF_ROSDAC_I_GC2_LEN)-1)<<RF_ROSDAC_I_GC2_POS))
#define RF_ROSDAC_Q_GC2                                         RF_ROSDAC_Q_GC2
#define RF_ROSDAC_Q_GC2_POS                                     (8U)
#define RF_ROSDAC_Q_GC2_LEN                                     (6U)
#define RF_ROSDAC_Q_GC2_MSK                                     (((1U<<RF_ROSDAC_Q_GC2_LEN)-1)<<RF_ROSDAC_Q_GC2_POS)
#define RF_ROSDAC_Q_GC2_UMSK                                    (~(((1U<<RF_ROSDAC_Q_GC2_LEN)-1)<<RF_ROSDAC_Q_GC2_POS))
#define RF_ROSDAC_I_GC3                                         RF_ROSDAC_I_GC3
#define RF_ROSDAC_I_GC3_POS                                     (16U)
#define RF_ROSDAC_I_GC3_LEN                                     (6U)
#define RF_ROSDAC_I_GC3_MSK                                     (((1U<<RF_ROSDAC_I_GC3_LEN)-1)<<RF_ROSDAC_I_GC3_POS)
#define RF_ROSDAC_I_GC3_UMSK                                    (~(((1U<<RF_ROSDAC_I_GC3_LEN)-1)<<RF_ROSDAC_I_GC3_POS))
#define RF_ROSDAC_Q_GC3                                         RF_ROSDAC_Q_GC3
#define RF_ROSDAC_Q_GC3_POS                                     (24U)
#define RF_ROSDAC_Q_GC3_LEN                                     (6U)
#define RF_ROSDAC_Q_GC3_MSK                                     (((1U<<RF_ROSDAC_Q_GC3_LEN)-1)<<RF_ROSDAC_Q_GC3_POS)
#define RF_ROSDAC_Q_GC3_UMSK                                    (~(((1U<<RF_ROSDAC_Q_GC3_LEN)-1)<<RF_ROSDAC_Q_GC3_POS))

/* 0x170 : rxiq_ctrl_hw1 */
#define RF_RXIQ_CTRL_HW1_OFFSET                                 (0x170)
#define RF_RX_IQ_PHASE_COMP_GC0                                 RF_RX_IQ_PHASE_COMP_GC0
#define RF_RX_IQ_PHASE_COMP_GC0_POS                             (0U)
#define RF_RX_IQ_PHASE_COMP_GC0_LEN                             (10U)
#define RF_RX_IQ_PHASE_COMP_GC0_MSK                             (((1U<<RF_RX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC0_POS)
#define RF_RX_IQ_PHASE_COMP_GC0_UMSK                            (~(((1U<<RF_RX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC0_POS))
#define RF_RX_IQ_GAIN_COMP_GC0                                  RF_RX_IQ_GAIN_COMP_GC0
#define RF_RX_IQ_GAIN_COMP_GC0_POS                              (16U)
#define RF_RX_IQ_GAIN_COMP_GC0_LEN                              (11U)
#define RF_RX_IQ_GAIN_COMP_GC0_MSK                              (((1U<<RF_RX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC0_POS)
#define RF_RX_IQ_GAIN_COMP_GC0_UMSK                             (~(((1U<<RF_RX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC0_POS))

/* 0x174 : rxiq_ctrl_hw2 */
#define RF_RXIQ_CTRL_HW2_OFFSET                                 (0x174)
#define RF_RX_IQ_PHASE_COMP_GC1                                 RF_RX_IQ_PHASE_COMP_GC1
#define RF_RX_IQ_PHASE_COMP_GC1_POS                             (0U)
#define RF_RX_IQ_PHASE_COMP_GC1_LEN                             (10U)
#define RF_RX_IQ_PHASE_COMP_GC1_MSK                             (((1U<<RF_RX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC1_POS)
#define RF_RX_IQ_PHASE_COMP_GC1_UMSK                            (~(((1U<<RF_RX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC1_POS))
#define RF_RX_IQ_GAIN_COMP_GC1                                  RF_RX_IQ_GAIN_COMP_GC1
#define RF_RX_IQ_GAIN_COMP_GC1_POS                              (16U)
#define RF_RX_IQ_GAIN_COMP_GC1_LEN                              (11U)
#define RF_RX_IQ_GAIN_COMP_GC1_MSK                              (((1U<<RF_RX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC1_POS)
#define RF_RX_IQ_GAIN_COMP_GC1_UMSK                             (~(((1U<<RF_RX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC1_POS))

/* 0x178 : rxiq_ctrl_hw3 */
#define RF_RXIQ_CTRL_HW3_OFFSET                                 (0x178)
#define RF_RX_IQ_PHASE_COMP_GC2                                 RF_RX_IQ_PHASE_COMP_GC2
#define RF_RX_IQ_PHASE_COMP_GC2_POS                             (0U)
#define RF_RX_IQ_PHASE_COMP_GC2_LEN                             (10U)
#define RF_RX_IQ_PHASE_COMP_GC2_MSK                             (((1U<<RF_RX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC2_POS)
#define RF_RX_IQ_PHASE_COMP_GC2_UMSK                            (~(((1U<<RF_RX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC2_POS))
#define RF_RX_IQ_GAIN_COMP_GC2                                  RF_RX_IQ_GAIN_COMP_GC2
#define RF_RX_IQ_GAIN_COMP_GC2_POS                              (16U)
#define RF_RX_IQ_GAIN_COMP_GC2_LEN                              (11U)
#define RF_RX_IQ_GAIN_COMP_GC2_MSK                              (((1U<<RF_RX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC2_POS)
#define RF_RX_IQ_GAIN_COMP_GC2_UMSK                             (~(((1U<<RF_RX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC2_POS))

/* 0x17C : rxiq_ctrl_hw4 */
#define RF_RXIQ_CTRL_HW4_OFFSET                                 (0x17C)
#define RF_RX_IQ_PHASE_COMP_GC3                                 RF_RX_IQ_PHASE_COMP_GC3
#define RF_RX_IQ_PHASE_COMP_GC3_POS                             (0U)
#define RF_RX_IQ_PHASE_COMP_GC3_LEN                             (10U)
#define RF_RX_IQ_PHASE_COMP_GC3_MSK                             (((1U<<RF_RX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC3_POS)
#define RF_RX_IQ_PHASE_COMP_GC3_UMSK                            (~(((1U<<RF_RX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC3_POS))
#define RF_RX_IQ_GAIN_COMP_GC3                                  RF_RX_IQ_GAIN_COMP_GC3
#define RF_RX_IQ_GAIN_COMP_GC3_POS                              (16U)
#define RF_RX_IQ_GAIN_COMP_GC3_LEN                              (11U)
#define RF_RX_IQ_GAIN_COMP_GC3_MSK                              (((1U<<RF_RX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC3_POS)
#define RF_RX_IQ_GAIN_COMP_GC3_UMSK                             (~(((1U<<RF_RX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC3_POS))

/* 0x180 : tosdac_ctrl_hw1 */
#define RF_TOSDAC_CTRL_HW1_OFFSET                               (0x180)

/* 0x184 : tosdac_ctrl_hw2 */
#define RF_TOSDAC_CTRL_HW2_OFFSET                               (0x184)

/* 0x188 : tosdac_ctrl_hw3 */
#define RF_TOSDAC_CTRL_HW3_OFFSET                               (0x188)

/* 0x18C : tosdac_ctrl_hw4 */
#define RF_TOSDAC_CTRL_HW4_OFFSET                               (0x18C)

/* 0x190 : tx_iq_gain_hw0 */
#define RF_TX_IQ_GAIN_HW0_OFFSET                                (0x190)

/* 0x194 : tx_iq_gain_hw1 */
#define RF_TX_IQ_GAIN_HW1_OFFSET                                (0x194)

/* 0x198 : tx_iq_gain_hw2 */
#define RF_TX_IQ_GAIN_HW2_OFFSET                                (0x198)

/* 0x19C : tx_iq_gain_hw3 */
#define RF_TX_IQ_GAIN_HW3_OFFSET                                (0x19C)

/* 0x1A0 : tx_iq_gain_hw4 */
#define RF_TX_IQ_GAIN_HW4_OFFSET                                (0x1A0)

/* 0x1A4 : tx_iq_gain_hw5 */
#define RF_TX_IQ_GAIN_HW5_OFFSET                                (0x1A4)

/* 0x1A8 : tx_iq_gain_hw6 */
#define RF_TX_IQ_GAIN_HW6_OFFSET                                (0x1A8)

/* 0x1AC : tx_iq_gain_hw7 */
#define RF_TX_IQ_GAIN_HW7_OFFSET                                (0x1AC)

/* 0x1B0 : lo_sdm_ctrl_hw1 */
#define RF_LO_SDM_CTRL_HW1_OFFSET                               (0x1B0)
#define RF_LO_SDM_DITHER_SEL_WLAN_2412                          RF_LO_SDM_DITHER_SEL_WLAN_2412
#define RF_LO_SDM_DITHER_SEL_WLAN_2412_POS                      (0U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2412_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2412_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2412_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2412_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2412_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2412_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2412_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2417                          RF_LO_SDM_DITHER_SEL_WLAN_2417
#define RF_LO_SDM_DITHER_SEL_WLAN_2417_POS                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2417_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2417_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2417_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2417_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2417_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2417_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2417_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2422                          RF_LO_SDM_DITHER_SEL_WLAN_2422
#define RF_LO_SDM_DITHER_SEL_WLAN_2422_POS                      (4U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2422_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2422_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2422_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2422_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2422_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2422_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2422_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2427                          RF_LO_SDM_DITHER_SEL_WLAN_2427
#define RF_LO_SDM_DITHER_SEL_WLAN_2427_POS                      (6U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2427_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2427_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2427_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2427_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2427_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2427_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2427_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2432                          RF_LO_SDM_DITHER_SEL_WLAN_2432
#define RF_LO_SDM_DITHER_SEL_WLAN_2432_POS                      (8U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2432_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2432_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2432_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2432_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2432_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2432_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2432_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2437                          RF_LO_SDM_DITHER_SEL_WLAN_2437
#define RF_LO_SDM_DITHER_SEL_WLAN_2437_POS                      (10U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2437_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2437_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2437_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2437_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2437_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2437_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2437_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2442                          RF_LO_SDM_DITHER_SEL_WLAN_2442
#define RF_LO_SDM_DITHER_SEL_WLAN_2442_POS                      (12U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2442_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2442_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2442_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2442_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2442_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2442_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2442_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2447                          RF_LO_SDM_DITHER_SEL_WLAN_2447
#define RF_LO_SDM_DITHER_SEL_WLAN_2447_POS                      (14U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2447_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2447_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2447_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2447_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2447_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2447_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2447_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2452                          RF_LO_SDM_DITHER_SEL_WLAN_2452
#define RF_LO_SDM_DITHER_SEL_WLAN_2452_POS                      (16U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2452_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2452_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2452_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2452_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2452_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2452_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2452_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2457                          RF_LO_SDM_DITHER_SEL_WLAN_2457
#define RF_LO_SDM_DITHER_SEL_WLAN_2457_POS                      (18U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2457_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2457_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2457_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2457_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2457_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2457_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2457_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2462                          RF_LO_SDM_DITHER_SEL_WLAN_2462
#define RF_LO_SDM_DITHER_SEL_WLAN_2462_POS                      (20U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2462_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2462_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2462_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2462_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2462_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2462_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2462_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2467                          RF_LO_SDM_DITHER_SEL_WLAN_2467
#define RF_LO_SDM_DITHER_SEL_WLAN_2467_POS                      (22U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2467_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2467_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2467_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2467_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2467_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2467_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2467_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2472                          RF_LO_SDM_DITHER_SEL_WLAN_2472
#define RF_LO_SDM_DITHER_SEL_WLAN_2472_POS                      (24U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2472_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2472_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2472_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2472_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2472_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2472_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2472_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2484                          RF_LO_SDM_DITHER_SEL_WLAN_2484
#define RF_LO_SDM_DITHER_SEL_WLAN_2484_POS                      (26U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2484_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2484_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2484_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2484_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2484_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2484_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2484_POS))

/* 0x1B4 : lo_sdm_ctrl_hw2 */
#define RF_LO_SDM_CTRL_HW2_OFFSET                               (0x1B4)
#define RF_LO_SDM_DITHER_SEL_BLE_2402                           RF_LO_SDM_DITHER_SEL_BLE_2402
#define RF_LO_SDM_DITHER_SEL_BLE_2402_POS                       (0U)
#define RF_LO_SDM_DITHER_SEL_BLE_2402_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2402_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2402_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2402_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2402_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2402_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2402_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2404                           RF_LO_SDM_DITHER_SEL_BLE_2404
#define RF_LO_SDM_DITHER_SEL_BLE_2404_POS                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2404_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2404_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2404_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2404_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2404_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2404_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2404_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2406                           RF_LO_SDM_DITHER_SEL_BLE_2406
#define RF_LO_SDM_DITHER_SEL_BLE_2406_POS                       (4U)
#define RF_LO_SDM_DITHER_SEL_BLE_2406_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2406_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2406_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2406_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2406_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2406_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2406_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2408                           RF_LO_SDM_DITHER_SEL_BLE_2408
#define RF_LO_SDM_DITHER_SEL_BLE_2408_POS                       (6U)
#define RF_LO_SDM_DITHER_SEL_BLE_2408_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2408_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2408_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2408_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2408_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2408_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2408_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2410                           RF_LO_SDM_DITHER_SEL_BLE_2410
#define RF_LO_SDM_DITHER_SEL_BLE_2410_POS                       (8U)
#define RF_LO_SDM_DITHER_SEL_BLE_2410_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2410_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2410_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2410_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2410_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2410_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2410_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2412                           RF_LO_SDM_DITHER_SEL_BLE_2412
#define RF_LO_SDM_DITHER_SEL_BLE_2412_POS                       (10U)
#define RF_LO_SDM_DITHER_SEL_BLE_2412_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2412_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2412_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2412_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2412_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2412_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2412_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2414                           RF_LO_SDM_DITHER_SEL_BLE_2414
#define RF_LO_SDM_DITHER_SEL_BLE_2414_POS                       (12U)
#define RF_LO_SDM_DITHER_SEL_BLE_2414_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2414_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2414_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2414_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2414_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2414_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2414_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2416                           RF_LO_SDM_DITHER_SEL_BLE_2416
#define RF_LO_SDM_DITHER_SEL_BLE_2416_POS                       (14U)
#define RF_LO_SDM_DITHER_SEL_BLE_2416_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2416_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2416_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2416_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2416_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2416_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2416_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2418                           RF_LO_SDM_DITHER_SEL_BLE_2418
#define RF_LO_SDM_DITHER_SEL_BLE_2418_POS                       (16U)
#define RF_LO_SDM_DITHER_SEL_BLE_2418_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2418_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2418_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2418_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2418_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2418_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2418_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2420                           RF_LO_SDM_DITHER_SEL_BLE_2420
#define RF_LO_SDM_DITHER_SEL_BLE_2420_POS                       (18U)
#define RF_LO_SDM_DITHER_SEL_BLE_2420_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2420_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2420_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2420_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2420_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2420_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2420_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2422                           RF_LO_SDM_DITHER_SEL_BLE_2422
#define RF_LO_SDM_DITHER_SEL_BLE_2422_POS                       (20U)
#define RF_LO_SDM_DITHER_SEL_BLE_2422_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2422_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2422_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2422_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2422_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2422_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2422_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2424                           RF_LO_SDM_DITHER_SEL_BLE_2424
#define RF_LO_SDM_DITHER_SEL_BLE_2424_POS                       (22U)
#define RF_LO_SDM_DITHER_SEL_BLE_2424_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2424_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2424_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2424_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2424_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2424_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2424_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2426                           RF_LO_SDM_DITHER_SEL_BLE_2426
#define RF_LO_SDM_DITHER_SEL_BLE_2426_POS                       (24U)
#define RF_LO_SDM_DITHER_SEL_BLE_2426_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2426_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2426_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2426_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2426_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2426_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2426_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2428                           RF_LO_SDM_DITHER_SEL_BLE_2428
#define RF_LO_SDM_DITHER_SEL_BLE_2428_POS                       (26U)
#define RF_LO_SDM_DITHER_SEL_BLE_2428_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2428_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2428_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2428_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2428_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2428_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2428_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2430                           RF_LO_SDM_DITHER_SEL_BLE_2430
#define RF_LO_SDM_DITHER_SEL_BLE_2430_POS                       (28U)
#define RF_LO_SDM_DITHER_SEL_BLE_2430_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2430_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2430_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2430_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2430_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2430_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2430_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2432                           RF_LO_SDM_DITHER_SEL_BLE_2432
#define RF_LO_SDM_DITHER_SEL_BLE_2432_POS                       (30U)
#define RF_LO_SDM_DITHER_SEL_BLE_2432_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2432_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2432_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2432_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2432_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2432_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2432_POS))

/* 0x1B8 : lo_sdm_ctrl_hw3 */
#define RF_LO_SDM_CTRL_HW3_OFFSET                               (0x1B8)
#define RF_LO_SDM_DITHER_SEL_BLE_2434                           RF_LO_SDM_DITHER_SEL_BLE_2434
#define RF_LO_SDM_DITHER_SEL_BLE_2434_POS                       (0U)
#define RF_LO_SDM_DITHER_SEL_BLE_2434_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2434_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2434_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2434_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2434_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2434_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2434_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2436                           RF_LO_SDM_DITHER_SEL_BLE_2436
#define RF_LO_SDM_DITHER_SEL_BLE_2436_POS                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2436_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2436_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2436_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2436_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2436_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2436_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2436_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2438                           RF_LO_SDM_DITHER_SEL_BLE_2438
#define RF_LO_SDM_DITHER_SEL_BLE_2438_POS                       (4U)
#define RF_LO_SDM_DITHER_SEL_BLE_2438_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2438_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2438_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2438_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2438_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2438_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2438_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2440                           RF_LO_SDM_DITHER_SEL_BLE_2440
#define RF_LO_SDM_DITHER_SEL_BLE_2440_POS                       (6U)
#define RF_LO_SDM_DITHER_SEL_BLE_2440_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2440_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2440_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2440_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2440_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2440_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2440_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2442                           RF_LO_SDM_DITHER_SEL_BLE_2442
#define RF_LO_SDM_DITHER_SEL_BLE_2442_POS                       (8U)
#define RF_LO_SDM_DITHER_SEL_BLE_2442_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2442_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2442_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2442_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2442_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2442_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2442_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2444                           RF_LO_SDM_DITHER_SEL_BLE_2444
#define RF_LO_SDM_DITHER_SEL_BLE_2444_POS                       (10U)
#define RF_LO_SDM_DITHER_SEL_BLE_2444_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2444_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2444_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2444_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2444_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2444_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2444_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2446                           RF_LO_SDM_DITHER_SEL_BLE_2446
#define RF_LO_SDM_DITHER_SEL_BLE_2446_POS                       (12U)
#define RF_LO_SDM_DITHER_SEL_BLE_2446_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2446_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2446_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2446_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2446_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2446_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2446_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2448                           RF_LO_SDM_DITHER_SEL_BLE_2448
#define RF_LO_SDM_DITHER_SEL_BLE_2448_POS                       (14U)
#define RF_LO_SDM_DITHER_SEL_BLE_2448_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2448_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2448_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2448_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2448_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2448_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2448_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2450                           RF_LO_SDM_DITHER_SEL_BLE_2450
#define RF_LO_SDM_DITHER_SEL_BLE_2450_POS                       (16U)
#define RF_LO_SDM_DITHER_SEL_BLE_2450_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2450_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2450_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2450_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2450_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2450_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2450_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2452                           RF_LO_SDM_DITHER_SEL_BLE_2452
#define RF_LO_SDM_DITHER_SEL_BLE_2452_POS                       (18U)
#define RF_LO_SDM_DITHER_SEL_BLE_2452_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2452_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2452_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2452_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2452_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2452_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2452_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2454                           RF_LO_SDM_DITHER_SEL_BLE_2454
#define RF_LO_SDM_DITHER_SEL_BLE_2454_POS                       (20U)
#define RF_LO_SDM_DITHER_SEL_BLE_2454_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2454_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2454_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2454_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2454_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2454_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2454_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2456                           RF_LO_SDM_DITHER_SEL_BLE_2456
#define RF_LO_SDM_DITHER_SEL_BLE_2456_POS                       (22U)
#define RF_LO_SDM_DITHER_SEL_BLE_2456_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2456_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2456_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2456_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2456_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2456_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2456_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2458                           RF_LO_SDM_DITHER_SEL_BLE_2458
#define RF_LO_SDM_DITHER_SEL_BLE_2458_POS                       (24U)
#define RF_LO_SDM_DITHER_SEL_BLE_2458_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2458_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2458_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2458_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2458_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2458_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2458_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2460                           RF_LO_SDM_DITHER_SEL_BLE_2460
#define RF_LO_SDM_DITHER_SEL_BLE_2460_POS                       (26U)
#define RF_LO_SDM_DITHER_SEL_BLE_2460_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2460_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2460_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2460_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2460_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2460_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2460_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2462                           RF_LO_SDM_DITHER_SEL_BLE_2462
#define RF_LO_SDM_DITHER_SEL_BLE_2462_POS                       (28U)
#define RF_LO_SDM_DITHER_SEL_BLE_2462_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2462_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2462_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2462_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2462_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2462_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2462_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2464                           RF_LO_SDM_DITHER_SEL_BLE_2464
#define RF_LO_SDM_DITHER_SEL_BLE_2464_POS                       (30U)
#define RF_LO_SDM_DITHER_SEL_BLE_2464_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2464_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2464_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2464_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2464_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2464_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2464_POS))

/* 0x1BC : lo_sdm_ctrl_hw4 */
#define RF_LO_SDM_CTRL_HW4_OFFSET                               (0x1BC)
#define RF_LO_SDM_DITHER_SEL_BLE_2466                           RF_LO_SDM_DITHER_SEL_BLE_2466
#define RF_LO_SDM_DITHER_SEL_BLE_2466_POS                       (0U)
#define RF_LO_SDM_DITHER_SEL_BLE_2466_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2466_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2466_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2466_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2466_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2466_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2466_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2468                           RF_LO_SDM_DITHER_SEL_BLE_2468
#define RF_LO_SDM_DITHER_SEL_BLE_2468_POS                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2468_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2468_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2468_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2468_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2468_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2468_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2468_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2470                           RF_LO_SDM_DITHER_SEL_BLE_2470
#define RF_LO_SDM_DITHER_SEL_BLE_2470_POS                       (4U)
#define RF_LO_SDM_DITHER_SEL_BLE_2470_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2470_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2470_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2470_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2470_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2470_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2470_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2472                           RF_LO_SDM_DITHER_SEL_BLE_2472
#define RF_LO_SDM_DITHER_SEL_BLE_2472_POS                       (6U)
#define RF_LO_SDM_DITHER_SEL_BLE_2472_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2472_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2472_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2472_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2472_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2472_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2472_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2474                           RF_LO_SDM_DITHER_SEL_BLE_2474
#define RF_LO_SDM_DITHER_SEL_BLE_2474_POS                       (8U)
#define RF_LO_SDM_DITHER_SEL_BLE_2474_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2474_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2474_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2474_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2474_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2474_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2474_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2476                           RF_LO_SDM_DITHER_SEL_BLE_2476
#define RF_LO_SDM_DITHER_SEL_BLE_2476_POS                       (10U)
#define RF_LO_SDM_DITHER_SEL_BLE_2476_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2476_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2476_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2476_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2476_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2476_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2476_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2478                           RF_LO_SDM_DITHER_SEL_BLE_2478
#define RF_LO_SDM_DITHER_SEL_BLE_2478_POS                       (12U)
#define RF_LO_SDM_DITHER_SEL_BLE_2478_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2478_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2478_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2478_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2478_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2478_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2478_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2480                           RF_LO_SDM_DITHER_SEL_BLE_2480
#define RF_LO_SDM_DITHER_SEL_BLE_2480_POS                       (14U)
#define RF_LO_SDM_DITHER_SEL_BLE_2480_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2480_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2480_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2480_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2480_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2480_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2480_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_TX                             RF_LO_SDM_DITHER_SEL_BLE_TX
#define RF_LO_SDM_DITHER_SEL_BLE_TX_POS                         (16U)
#define RF_LO_SDM_DITHER_SEL_BLE_TX_LEN                         (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_TX_MSK                         (((1U<<RF_LO_SDM_DITHER_SEL_BLE_TX_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_TX_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_TX_UMSK                        (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_TX_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_TX_POS))

/* 0x1C0 : lo_sdm_ctrl_hw5 */
#define RF_LO_SDM_CTRL_HW5_OFFSET                               (0x1C0)
#define RF_LO_CENTER_FREQ_MHZ                                   RF_LO_CENTER_FREQ_MHZ
#define RF_LO_CENTER_FREQ_MHZ_POS                               (0U)
#define RF_LO_CENTER_FREQ_MHZ_LEN                               (12U)
#define RF_LO_CENTER_FREQ_MHZ_MSK                               (((1U<<RF_LO_CENTER_FREQ_MHZ_LEN)-1)<<RF_LO_CENTER_FREQ_MHZ_POS)
#define RF_LO_CENTER_FREQ_MHZ_UMSK                              (~(((1U<<RF_LO_CENTER_FREQ_MHZ_LEN)-1)<<RF_LO_CENTER_FREQ_MHZ_POS))
#define RF_LO_SDM_BYPASS_MODE                                   RF_LO_SDM_BYPASS_MODE
#define RF_LO_SDM_BYPASS_MODE_POS                               (12U)
#define RF_LO_SDM_BYPASS_MODE_LEN                               (6U)
#define RF_LO_SDM_BYPASS_MODE_MSK                               (((1U<<RF_LO_SDM_BYPASS_MODE_LEN)-1)<<RF_LO_SDM_BYPASS_MODE_POS)
#define RF_LO_SDM_BYPASS_MODE_UMSK                              (~(((1U<<RF_LO_SDM_BYPASS_MODE_LEN)-1)<<RF_LO_SDM_BYPASS_MODE_POS))

/* 0x1C4 : lo_sdm_ctrl_hw6 */
#define RF_LO_SDM_CTRL_HW6_OFFSET                               (0x1C4)
#define RF_LO_SDMIN_CENTER                                      RF_LO_SDMIN_CENTER
#define RF_LO_SDMIN_CENTER_POS                                  (0U)
#define RF_LO_SDMIN_CENTER_LEN                                  (29U)
#define RF_LO_SDMIN_CENTER_MSK                                  (((1U<<RF_LO_SDMIN_CENTER_LEN)-1)<<RF_LO_SDMIN_CENTER_POS)
#define RF_LO_SDMIN_CENTER_UMSK                                 (~(((1U<<RF_LO_SDMIN_CENTER_LEN)-1)<<RF_LO_SDMIN_CENTER_POS))

/* 0x1C8 : lo_sdm_ctrl_hw7 */
#define RF_LO_SDM_CTRL_HW7_OFFSET                               (0x1C8)
#define RF_LO_SDMIN_1M                                          RF_LO_SDMIN_1M
#define RF_LO_SDMIN_1M_POS                                      (0U)
#define RF_LO_SDMIN_1M_LEN                                      (20U)
#define RF_LO_SDMIN_1M_MSK                                      (((1U<<RF_LO_SDMIN_1M_LEN)-1)<<RF_LO_SDMIN_1M_POS)
#define RF_LO_SDMIN_1M_UMSK                                     (~(((1U<<RF_LO_SDMIN_1M_LEN)-1)<<RF_LO_SDMIN_1M_POS))

/* 0x1CC : lo_sdm_ctrl_hw8 */
#define RF_LO_SDM_CTRL_HW8_OFFSET                               (0x1CC)
#define RF_LO_SDMIN_IF                                          RF_LO_SDMIN_IF
#define RF_LO_SDMIN_IF_POS                                      (0U)
#define RF_LO_SDMIN_IF_LEN                                      (20U)
#define RF_LO_SDMIN_IF_MSK                                      (((1U<<RF_LO_SDMIN_IF_LEN)-1)<<RF_LO_SDMIN_IF_POS)
#define RF_LO_SDMIN_IF_UMSK                                     (~(((1U<<RF_LO_SDMIN_IF_LEN)-1)<<RF_LO_SDMIN_IF_POS))

/* 0x1D0 : rbb_bw_ctrl_hw */
#define RF_RBB_BW_CTRL_HW_OFFSET                                (0x1D0)
#define RF_RBB_BT_MODE_BLE                                      RF_RBB_BT_MODE_BLE
#define RF_RBB_BT_MODE_BLE_POS                                  (0U)
#define RF_RBB_BT_MODE_BLE_LEN                                  (1U)
#define RF_RBB_BT_MODE_BLE_MSK                                  (((1U<<RF_RBB_BT_MODE_BLE_LEN)-1)<<RF_RBB_BT_MODE_BLE_POS)
#define RF_RBB_BT_MODE_BLE_UMSK                                 (~(((1U<<RF_RBB_BT_MODE_BLE_LEN)-1)<<RF_RBB_BT_MODE_BLE_POS))

/* 0x208 : rf_top_ctrl */
#define RF_TOP_CTRL_OFFSET                                      (0x208)
#define RFCKG_CLK_PHY_EN                                        RFCKG_CLK_PHY_EN
#define RFCKG_CLK_PHY_EN_POS                                    (0U)
#define RFCKG_CLK_PHY_EN_LEN                                    (1U)
#define RFCKG_CLK_PHY_EN_MSK                                    (((1U<<RFCKG_CLK_PHY_EN_LEN)-1)<<RFCKG_CLK_PHY_EN_POS)
#define RFCKG_CLK_PHY_EN_UMSK                                   (~(((1U<<RFCKG_CLK_PHY_EN_LEN)-1)<<RFCKG_CLK_PHY_EN_POS))

/* 0x20C : singen_ctrl0 */
#define RF_SINGEN_CTRL0_OFFSET                                  (0x20C)
#define RF_SINGEN_INC_STEP1                                     RF_SINGEN_INC_STEP1
#define RF_SINGEN_INC_STEP1_POS                                 (0U)
#define RF_SINGEN_INC_STEP1_LEN                                 (10U)
#define RF_SINGEN_INC_STEP1_MSK                                 (((1U<<RF_SINGEN_INC_STEP1_LEN)-1)<<RF_SINGEN_INC_STEP1_POS)
#define RF_SINGEN_INC_STEP1_UMSK                                (~(((1U<<RF_SINGEN_INC_STEP1_LEN)-1)<<RF_SINGEN_INC_STEP1_POS))
#define RF_SINGEN_INC_STEP0                                     RF_SINGEN_INC_STEP0
#define RF_SINGEN_INC_STEP0_POS                                 (16U)
#define RF_SINGEN_INC_STEP0_LEN                                 (10U)
#define RF_SINGEN_INC_STEP0_MSK                                 (((1U<<RF_SINGEN_INC_STEP0_LEN)-1)<<RF_SINGEN_INC_STEP0_POS)
#define RF_SINGEN_INC_STEP0_UMSK                                (~(((1U<<RF_SINGEN_INC_STEP0_LEN)-1)<<RF_SINGEN_INC_STEP0_POS))
#define RF_SINGEN_UNSIGN_EN                                     RF_SINGEN_UNSIGN_EN
#define RF_SINGEN_UNSIGN_EN_POS                                 (28U)
#define RF_SINGEN_UNSIGN_EN_LEN                                 (1U)
#define RF_SINGEN_UNSIGN_EN_MSK                                 (((1U<<RF_SINGEN_UNSIGN_EN_LEN)-1)<<RF_SINGEN_UNSIGN_EN_POS)
#define RF_SINGEN_UNSIGN_EN_UMSK                                (~(((1U<<RF_SINGEN_UNSIGN_EN_LEN)-1)<<RF_SINGEN_UNSIGN_EN_POS))
#define RF_SINGEN_CLKDIV_N                                      RF_SINGEN_CLKDIV_N
#define RF_SINGEN_CLKDIV_N_POS                                  (29U)
#define RF_SINGEN_CLKDIV_N_LEN                                  (2U)
#define RF_SINGEN_CLKDIV_N_MSK                                  (((1U<<RF_SINGEN_CLKDIV_N_LEN)-1)<<RF_SINGEN_CLKDIV_N_POS)
#define RF_SINGEN_CLKDIV_N_UMSK                                 (~(((1U<<RF_SINGEN_CLKDIV_N_LEN)-1)<<RF_SINGEN_CLKDIV_N_POS))
#define RF_SINGEN_EN                                            RF_SINGEN_EN
#define RF_SINGEN_EN_POS                                        (31U)
#define RF_SINGEN_EN_LEN                                        (1U)
#define RF_SINGEN_EN_MSK                                        (((1U<<RF_SINGEN_EN_LEN)-1)<<RF_SINGEN_EN_POS)
#define RF_SINGEN_EN_UMSK                                       (~(((1U<<RF_SINGEN_EN_LEN)-1)<<RF_SINGEN_EN_POS))

/* 0x210 : singen_ctrl1 */
#define RF_SINGEN_CTRL1_OFFSET                                  (0x210)
#define RF_SINGEN_CLKDIV_Q                                      RF_SINGEN_CLKDIV_Q
#define RF_SINGEN_CLKDIV_Q_POS                                  (0U)
#define RF_SINGEN_CLKDIV_Q_LEN                                  (10U)
#define RF_SINGEN_CLKDIV_Q_MSK                                  (((1U<<RF_SINGEN_CLKDIV_Q_LEN)-1)<<RF_SINGEN_CLKDIV_Q_POS)
#define RF_SINGEN_CLKDIV_Q_UMSK                                 (~(((1U<<RF_SINGEN_CLKDIV_Q_LEN)-1)<<RF_SINGEN_CLKDIV_Q_POS))
#define RF_SINGEN_MODE_Q                                        RF_SINGEN_MODE_Q
#define RF_SINGEN_MODE_Q_POS                                    (12U)
#define RF_SINGEN_MODE_Q_LEN                                    (4U)
#define RF_SINGEN_MODE_Q_MSK                                    (((1U<<RF_SINGEN_MODE_Q_LEN)-1)<<RF_SINGEN_MODE_Q_POS)
#define RF_SINGEN_MODE_Q_UMSK                                   (~(((1U<<RF_SINGEN_MODE_Q_LEN)-1)<<RF_SINGEN_MODE_Q_POS))
#define RF_SINGEN_CLKDIV_I                                      RF_SINGEN_CLKDIV_I
#define RF_SINGEN_CLKDIV_I_POS                                  (16U)
#define RF_SINGEN_CLKDIV_I_LEN                                  (10U)
#define RF_SINGEN_CLKDIV_I_MSK                                  (((1U<<RF_SINGEN_CLKDIV_I_LEN)-1)<<RF_SINGEN_CLKDIV_I_POS)
#define RF_SINGEN_CLKDIV_I_UMSK                                 (~(((1U<<RF_SINGEN_CLKDIV_I_LEN)-1)<<RF_SINGEN_CLKDIV_I_POS))
#define RF_SINGEN_MODE_I                                        RF_SINGEN_MODE_I
#define RF_SINGEN_MODE_I_POS                                    (28U)
#define RF_SINGEN_MODE_I_LEN                                    (4U)
#define RF_SINGEN_MODE_I_MSK                                    (((1U<<RF_SINGEN_MODE_I_LEN)-1)<<RF_SINGEN_MODE_I_POS)
#define RF_SINGEN_MODE_I_UMSK                                   (~(((1U<<RF_SINGEN_MODE_I_LEN)-1)<<RF_SINGEN_MODE_I_POS))

/* 0x214 : singen_ctrl2 */
#define RF_SINGEN_CTRL2_OFFSET                                  (0x214)
#define RF_SINGEN_GAIN_I                                        RF_SINGEN_GAIN_I
#define RF_SINGEN_GAIN_I_POS                                    (0U)
#define RF_SINGEN_GAIN_I_LEN                                    (11U)
#define RF_SINGEN_GAIN_I_MSK                                    (((1U<<RF_SINGEN_GAIN_I_LEN)-1)<<RF_SINGEN_GAIN_I_POS)
#define RF_SINGEN_GAIN_I_UMSK                                   (~(((1U<<RF_SINGEN_GAIN_I_LEN)-1)<<RF_SINGEN_GAIN_I_POS))
#define RF_SINGEN_START_ADDR1_I                                 RF_SINGEN_START_ADDR1_I
#define RF_SINGEN_START_ADDR1_I_POS                             (12U)
#define RF_SINGEN_START_ADDR1_I_LEN                             (10U)
#define RF_SINGEN_START_ADDR1_I_MSK                             (((1U<<RF_SINGEN_START_ADDR1_I_LEN)-1)<<RF_SINGEN_START_ADDR1_I_POS)
#define RF_SINGEN_START_ADDR1_I_UMSK                            (~(((1U<<RF_SINGEN_START_ADDR1_I_LEN)-1)<<RF_SINGEN_START_ADDR1_I_POS))
#define RF_SINGEN_START_ADDR0_I                                 RF_SINGEN_START_ADDR0_I
#define RF_SINGEN_START_ADDR0_I_POS                             (22U)
#define RF_SINGEN_START_ADDR0_I_LEN                             (10U)
#define RF_SINGEN_START_ADDR0_I_MSK                             (((1U<<RF_SINGEN_START_ADDR0_I_LEN)-1)<<RF_SINGEN_START_ADDR0_I_POS)
#define RF_SINGEN_START_ADDR0_I_UMSK                            (~(((1U<<RF_SINGEN_START_ADDR0_I_LEN)-1)<<RF_SINGEN_START_ADDR0_I_POS))

/* 0x218 : singen_ctrl3 */
#define RF_SINGEN_CTRL3_OFFSET                                  (0x218)
#define RF_SINGEN_GAIN_Q                                        RF_SINGEN_GAIN_Q
#define RF_SINGEN_GAIN_Q_POS                                    (0U)
#define RF_SINGEN_GAIN_Q_LEN                                    (11U)
#define RF_SINGEN_GAIN_Q_MSK                                    (((1U<<RF_SINGEN_GAIN_Q_LEN)-1)<<RF_SINGEN_GAIN_Q_POS)
#define RF_SINGEN_GAIN_Q_UMSK                                   (~(((1U<<RF_SINGEN_GAIN_Q_LEN)-1)<<RF_SINGEN_GAIN_Q_POS))
#define RF_SINGEN_START_ADDR1_Q                                 RF_SINGEN_START_ADDR1_Q
#define RF_SINGEN_START_ADDR1_Q_POS                             (12U)
#define RF_SINGEN_START_ADDR1_Q_LEN                             (10U)
#define RF_SINGEN_START_ADDR1_Q_MSK                             (((1U<<RF_SINGEN_START_ADDR1_Q_LEN)-1)<<RF_SINGEN_START_ADDR1_Q_POS)
#define RF_SINGEN_START_ADDR1_Q_UMSK                            (~(((1U<<RF_SINGEN_START_ADDR1_Q_LEN)-1)<<RF_SINGEN_START_ADDR1_Q_POS))
#define RF_SINGEN_START_ADDR0_Q                                 RF_SINGEN_START_ADDR0_Q
#define RF_SINGEN_START_ADDR0_Q_POS                             (22U)
#define RF_SINGEN_START_ADDR0_Q_LEN                             (10U)
#define RF_SINGEN_START_ADDR0_Q_MSK                             (((1U<<RF_SINGEN_START_ADDR0_Q_LEN)-1)<<RF_SINGEN_START_ADDR0_Q_POS)
#define RF_SINGEN_START_ADDR0_Q_UMSK                            (~(((1U<<RF_SINGEN_START_ADDR0_Q_LEN)-1)<<RF_SINGEN_START_ADDR0_Q_POS))

/* 0x21C : singen_ctrl4 */
#define RF_SINGEN_CTRL4_OFFSET                                  (0x21C)
#define RF_SINGEN_FIX_Q                                         RF_SINGEN_FIX_Q
#define RF_SINGEN_FIX_Q_POS                                     (0U)
#define RF_SINGEN_FIX_Q_LEN                                     (12U)
#define RF_SINGEN_FIX_Q_MSK                                     (((1U<<RF_SINGEN_FIX_Q_LEN)-1)<<RF_SINGEN_FIX_Q_POS)
#define RF_SINGEN_FIX_Q_UMSK                                    (~(((1U<<RF_SINGEN_FIX_Q_LEN)-1)<<RF_SINGEN_FIX_Q_POS))
#define RF_SINGEN_FIX_EN_Q                                      RF_SINGEN_FIX_EN_Q
#define RF_SINGEN_FIX_EN_Q_POS                                  (12U)
#define RF_SINGEN_FIX_EN_Q_LEN                                  (1U)
#define RF_SINGEN_FIX_EN_Q_MSK                                  (((1U<<RF_SINGEN_FIX_EN_Q_LEN)-1)<<RF_SINGEN_FIX_EN_Q_POS)
#define RF_SINGEN_FIX_EN_Q_UMSK                                 (~(((1U<<RF_SINGEN_FIX_EN_Q_LEN)-1)<<RF_SINGEN_FIX_EN_Q_POS))
#define RF_SINGEN_FIX_I                                         RF_SINGEN_FIX_I
#define RF_SINGEN_FIX_I_POS                                     (16U)
#define RF_SINGEN_FIX_I_LEN                                     (12U)
#define RF_SINGEN_FIX_I_MSK                                     (((1U<<RF_SINGEN_FIX_I_LEN)-1)<<RF_SINGEN_FIX_I_POS)
#define RF_SINGEN_FIX_I_UMSK                                    (~(((1U<<RF_SINGEN_FIX_I_LEN)-1)<<RF_SINGEN_FIX_I_POS))
#define RF_SINGEN_FIX_EN_I                                      RF_SINGEN_FIX_EN_I
#define RF_SINGEN_FIX_EN_I_POS                                  (28U)
#define RF_SINGEN_FIX_EN_I_LEN                                  (1U)
#define RF_SINGEN_FIX_EN_I_MSK                                  (((1U<<RF_SINGEN_FIX_EN_I_LEN)-1)<<RF_SINGEN_FIX_EN_I_POS)
#define RF_SINGEN_FIX_EN_I_UMSK                                 (~(((1U<<RF_SINGEN_FIX_EN_I_LEN)-1)<<RF_SINGEN_FIX_EN_I_POS))

/* 0x220 : rfif_dfe_ctrl0 */
#define RFIF_DFE_CTRL0_OFFSET                                   (0x220)
#define RFCKG_RXCLK_4S_ON                                       RFCKG_RXCLK_4S_ON
#define RFCKG_RXCLK_4S_ON_POS                                   (0U)
#define RFCKG_RXCLK_4S_ON_LEN                                   (1U)
#define RFCKG_RXCLK_4S_ON_MSK                                   (((1U<<RFCKG_RXCLK_4S_ON_LEN)-1)<<RFCKG_RXCLK_4S_ON_POS)
#define RFCKG_RXCLK_4S_ON_UMSK                                  (~(((1U<<RFCKG_RXCLK_4S_ON_LEN)-1)<<RFCKG_RXCLK_4S_ON_POS))
#define RFCKG_TXCLK_4S_ON                                       RFCKG_TXCLK_4S_ON
#define RFCKG_TXCLK_4S_ON_POS                                   (1U)
#define RFCKG_TXCLK_4S_ON_LEN                                   (1U)
#define RFCKG_TXCLK_4S_ON_MSK                                   (((1U<<RFCKG_TXCLK_4S_ON_LEN)-1)<<RFCKG_TXCLK_4S_ON_POS)
#define RFCKG_TXCLK_4S_ON_UMSK                                  (~(((1U<<RFCKG_TXCLK_4S_ON_LEN)-1)<<RFCKG_TXCLK_4S_ON_POS))
#define RFCKG_ADC_AFIFO_INV                                     RFCKG_ADC_AFIFO_INV
#define RFCKG_ADC_AFIFO_INV_POS                                 (2U)
#define RFCKG_ADC_AFIFO_INV_LEN                                 (1U)
#define RFCKG_ADC_AFIFO_INV_MSK                                 (((1U<<RFCKG_ADC_AFIFO_INV_LEN)-1)<<RFCKG_ADC_AFIFO_INV_POS)
#define RFCKG_ADC_AFIFO_INV_UMSK                                (~(((1U<<RFCKG_ADC_AFIFO_INV_LEN)-1)<<RFCKG_ADC_AFIFO_INV_POS))
#define RFCKG_ADC_CLKOUT_SEL                                    RFCKG_ADC_CLKOUT_SEL
#define RFCKG_ADC_CLKOUT_SEL_POS                                (3U)
#define RFCKG_ADC_CLKOUT_SEL_LEN                                (1U)
#define RFCKG_ADC_CLKOUT_SEL_MSK                                (((1U<<RFCKG_ADC_CLKOUT_SEL_LEN)-1)<<RFCKG_ADC_CLKOUT_SEL_POS)
#define RFCKG_ADC_CLKOUT_SEL_UMSK                               (~(((1U<<RFCKG_ADC_CLKOUT_SEL_LEN)-1)<<RFCKG_ADC_CLKOUT_SEL_POS))
#define RFCKG_DAC_AFIFO_INV                                     RFCKG_DAC_AFIFO_INV
#define RFCKG_DAC_AFIFO_INV_POS                                 (4U)
#define RFCKG_DAC_AFIFO_INV_LEN                                 (1U)
#define RFCKG_DAC_AFIFO_INV_MSK                                 (((1U<<RFCKG_DAC_AFIFO_INV_LEN)-1)<<RFCKG_DAC_AFIFO_INV_POS)
#define RFCKG_DAC_AFIFO_INV_UMSK                                (~(((1U<<RFCKG_DAC_AFIFO_INV_LEN)-1)<<RFCKG_DAC_AFIFO_INV_POS))
#define RF_RX_DFE_EN_4S                                         RF_RX_DFE_EN_4S
#define RF_RX_DFE_EN_4S_POS                                     (5U)
#define RF_RX_DFE_EN_4S_LEN                                     (1U)
#define RF_RX_DFE_EN_4S_MSK                                     (((1U<<RF_RX_DFE_EN_4S_LEN)-1)<<RF_RX_DFE_EN_4S_POS)
#define RF_RX_DFE_EN_4S_UMSK                                    (~(((1U<<RF_RX_DFE_EN_4S_LEN)-1)<<RF_RX_DFE_EN_4S_POS))
#define RF_RX_DFE_EN_4S_EN                                      RF_RX_DFE_EN_4S_EN
#define RF_RX_DFE_EN_4S_EN_POS                                  (6U)
#define RF_RX_DFE_EN_4S_EN_LEN                                  (1U)
#define RF_RX_DFE_EN_4S_EN_MSK                                  (((1U<<RF_RX_DFE_EN_4S_EN_LEN)-1)<<RF_RX_DFE_EN_4S_EN_POS)
#define RF_RX_DFE_EN_4S_EN_UMSK                                 (~(((1U<<RF_RX_DFE_EN_4S_EN_LEN)-1)<<RF_RX_DFE_EN_4S_EN_POS))
#define RF_TX_DFE_EN_4S                                         RF_TX_DFE_EN_4S
#define RF_TX_DFE_EN_4S_POS                                     (7U)
#define RF_TX_DFE_EN_4S_LEN                                     (1U)
#define RF_TX_DFE_EN_4S_MSK                                     (((1U<<RF_TX_DFE_EN_4S_LEN)-1)<<RF_TX_DFE_EN_4S_POS)
#define RF_TX_DFE_EN_4S_UMSK                                    (~(((1U<<RF_TX_DFE_EN_4S_LEN)-1)<<RF_TX_DFE_EN_4S_POS))
#define RF_TX_DFE_EN_4S_EN                                      RF_TX_DFE_EN_4S_EN
#define RF_TX_DFE_EN_4S_EN_POS                                  (8U)
#define RF_TX_DFE_EN_4S_EN_LEN                                  (1U)
#define RF_TX_DFE_EN_4S_EN_MSK                                  (((1U<<RF_TX_DFE_EN_4S_EN_LEN)-1)<<RF_TX_DFE_EN_4S_EN_POS)
#define RF_TX_DFE_EN_4S_EN_UMSK                                 (~(((1U<<RF_TX_DFE_EN_4S_EN_LEN)-1)<<RF_TX_DFE_EN_4S_EN_POS))
#define RF_RX_TEST_SEL                                          RF_RX_TEST_SEL
#define RF_RX_TEST_SEL_POS                                      (9U)
#define RF_RX_TEST_SEL_LEN                                      (2U)
#define RF_RX_TEST_SEL_MSK                                      (((1U<<RF_RX_TEST_SEL_LEN)-1)<<RF_RX_TEST_SEL_POS)
#define RF_RX_TEST_SEL_UMSK                                     (~(((1U<<RF_RX_TEST_SEL_LEN)-1)<<RF_RX_TEST_SEL_POS))
#define RF_TX_TEST_SEL                                          RF_TX_TEST_SEL
#define RF_TX_TEST_SEL_POS                                      (11U)
#define RF_TX_TEST_SEL_LEN                                      (2U)
#define RF_TX_TEST_SEL_MSK                                      (((1U<<RF_TX_TEST_SEL_LEN)-1)<<RF_TX_TEST_SEL_POS)
#define RF_TX_TEST_SEL_UMSK                                     (~(((1U<<RF_TX_TEST_SEL_LEN)-1)<<RF_TX_TEST_SEL_POS))
#define RF_PAD_ADC_CLKOUT_INV_EN                                RF_PAD_ADC_CLKOUT_INV_EN
#define RF_PAD_ADC_CLKOUT_INV_EN_POS                            (13U)
#define RF_PAD_ADC_CLKOUT_INV_EN_LEN                            (1U)
#define RF_PAD_ADC_CLKOUT_INV_EN_MSK                            (((1U<<RF_PAD_ADC_CLKOUT_INV_EN_LEN)-1)<<RF_PAD_ADC_CLKOUT_INV_EN_POS)
#define RF_PAD_ADC_CLKOUT_INV_EN_UMSK                           (~(((1U<<RF_PAD_ADC_CLKOUT_INV_EN_LEN)-1)<<RF_PAD_ADC_CLKOUT_INV_EN_POS))
#define RF_PAD_DAC_CLKOUT_INV_EN                                RF_PAD_DAC_CLKOUT_INV_EN
#define RF_PAD_DAC_CLKOUT_INV_EN_POS                            (14U)
#define RF_PAD_DAC_CLKOUT_INV_EN_LEN                            (1U)
#define RF_PAD_DAC_CLKOUT_INV_EN_MSK                            (((1U<<RF_PAD_DAC_CLKOUT_INV_EN_LEN)-1)<<RF_PAD_DAC_CLKOUT_INV_EN_POS)
#define RF_PAD_DAC_CLKOUT_INV_EN_UMSK                           (~(((1U<<RF_PAD_DAC_CLKOUT_INV_EN_LEN)-1)<<RF_PAD_DAC_CLKOUT_INV_EN_POS))
#define RF_CH_IND_BLE_4S                                        RF_CH_IND_BLE_4S
#define RF_CH_IND_BLE_4S_POS                                    (15U)
#define RF_CH_IND_BLE_4S_LEN                                    (7U)
#define RF_CH_IND_BLE_4S_MSK                                    (((1U<<RF_CH_IND_BLE_4S_LEN)-1)<<RF_CH_IND_BLE_4S_POS)
#define RF_CH_IND_BLE_4S_UMSK                                   (~(((1U<<RF_CH_IND_BLE_4S_LEN)-1)<<RF_CH_IND_BLE_4S_POS))
#define RF_CH_IND_BLE_4S_EN                                     RF_CH_IND_BLE_4S_EN
#define RF_CH_IND_BLE_4S_EN_POS                                 (22U)
#define RF_CH_IND_BLE_4S_EN_LEN                                 (1U)
#define RF_CH_IND_BLE_4S_EN_MSK                                 (((1U<<RF_CH_IND_BLE_4S_EN_LEN)-1)<<RF_CH_IND_BLE_4S_EN_POS)
#define RF_CH_IND_BLE_4S_EN_UMSK                                (~(((1U<<RF_CH_IND_BLE_4S_EN_LEN)-1)<<RF_CH_IND_BLE_4S_EN_POS))
#define RF_WIFIBW                                               RF_WIFIBW
#define RF_WIFIBW_POS                                           (24U)
#define RF_WIFIBW_LEN                                           (1U)
#define RF_WIFIBW_MSK                                           (((1U<<RF_WIFIBW_LEN)-1)<<RF_WIFIBW_POS)
#define RF_WIFIBW_UMSK                                          (~(((1U<<RF_WIFIBW_LEN)-1)<<RF_WIFIBW_POS))
#define RF_WIFIMODE_4S_EN                                       RF_WIFIMODE_4S_EN
#define RF_WIFIMODE_4S_EN_POS                                   (25U)
#define RF_WIFIMODE_4S_EN_LEN                                   (1U)
#define RF_WIFIMODE_4S_EN_MSK                                   (((1U<<RF_WIFIMODE_4S_EN_LEN)-1)<<RF_WIFIMODE_4S_EN_POS)
#define RF_WIFIMODE_4S_EN_UMSK                                  (~(((1U<<RF_WIFIMODE_4S_EN_LEN)-1)<<RF_WIFIMODE_4S_EN_POS))
#define RF_BBMODE_4S                                            RF_BBMODE_4S
#define RF_BBMODE_4S_POS                                        (26U)
#define RF_BBMODE_4S_LEN                                        (1U)
#define RF_BBMODE_4S_MSK                                        (((1U<<RF_BBMODE_4S_LEN)-1)<<RF_BBMODE_4S_POS)
#define RF_BBMODE_4S_UMSK                                       (~(((1U<<RF_BBMODE_4S_LEN)-1)<<RF_BBMODE_4S_POS))
#define RF_BBMODE_4S_EN                                         RF_BBMODE_4S_EN
#define RF_BBMODE_4S_EN_POS                                     (27U)
#define RF_BBMODE_4S_EN_LEN                                     (1U)
#define RF_BBMODE_4S_EN_MSK                                     (((1U<<RF_BBMODE_4S_EN_LEN)-1)<<RF_BBMODE_4S_EN_POS)
#define RF_BBMODE_4S_EN_UMSK                                    (~(((1U<<RF_BBMODE_4S_EN_LEN)-1)<<RF_BBMODE_4S_EN_POS))
#define RF_TEST_SEL                                             RF_TEST_SEL
#define RF_TEST_SEL_POS                                         (28U)
#define RF_TEST_SEL_LEN                                         (4U)
#define RF_TEST_SEL_MSK                                         (((1U<<RF_TEST_SEL_LEN)-1)<<RF_TEST_SEL_POS)
#define RF_TEST_SEL_UMSK                                        (~(((1U<<RF_TEST_SEL_LEN)-1)<<RF_TEST_SEL_POS))

/* 0x224 : rfif_test_read */
#define RFIF_TEST_READ_OFFSET                                   (0x224)
#define RF_TEST_READ                                            RF_TEST_READ
#define RF_TEST_READ_POS                                        (0U)
#define RF_TEST_READ_LEN                                        (32U)
#define RF_TEST_READ_MSK                                        (((1U<<RF_TEST_READ_LEN)-1)<<RF_TEST_READ_POS)
#define RF_TEST_READ_UMSK                                       (~(((1U<<RF_TEST_READ_LEN)-1)<<RF_TEST_READ_POS))

/* 0x228 : rfif_dig_ctrl */
#define RFIF_DIG_CTRL_OFFSET                                    (0x228)
#define RF_TEST_FROM_PAD_EN                                     RF_TEST_FROM_PAD_EN
#define RF_TEST_FROM_PAD_EN_POS                                 (0U)
#define RF_TEST_FROM_PAD_EN_LEN                                 (1U)
#define RF_TEST_FROM_PAD_EN_MSK                                 (((1U<<RF_TEST_FROM_PAD_EN_LEN)-1)<<RF_TEST_FROM_PAD_EN_POS)
#define RF_TEST_FROM_PAD_EN_UMSK                                (~(((1U<<RF_TEST_FROM_PAD_EN_LEN)-1)<<RF_TEST_FROM_PAD_EN_POS))
#define RF_TEST_GC_FROM_PAD_EN                                  RF_TEST_GC_FROM_PAD_EN
#define RF_TEST_GC_FROM_PAD_EN_POS                              (1U)
#define RF_TEST_GC_FROM_PAD_EN_LEN                              (1U)
#define RF_TEST_GC_FROM_PAD_EN_MSK                              (((1U<<RF_TEST_GC_FROM_PAD_EN_LEN)-1)<<RF_TEST_GC_FROM_PAD_EN_POS)
#define RF_TEST_GC_FROM_PAD_EN_UMSK                             (~(((1U<<RF_TEST_GC_FROM_PAD_EN_LEN)-1)<<RF_TEST_GC_FROM_PAD_EN_POS))
#define RFCKG_RXCLK_DIV2_MODE                                   RFCKG_RXCLK_DIV2_MODE
#define RFCKG_RXCLK_DIV2_MODE_POS                               (2U)
#define RFCKG_RXCLK_DIV2_MODE_LEN                               (1U)
#define RFCKG_RXCLK_DIV2_MODE_MSK                               (((1U<<RFCKG_RXCLK_DIV2_MODE_LEN)-1)<<RFCKG_RXCLK_DIV2_MODE_POS)
#define RFCKG_RXCLK_DIV2_MODE_UMSK                              (~(((1U<<RFCKG_RXCLK_DIV2_MODE_LEN)-1)<<RFCKG_RXCLK_DIV2_MODE_POS))
#define RFIF_INT_LO_UNLOCKED_MASK                               RFIF_INT_LO_UNLOCKED_MASK
#define RFIF_INT_LO_UNLOCKED_MASK_POS                           (3U)
#define RFIF_INT_LO_UNLOCKED_MASK_LEN                           (1U)
#define RFIF_INT_LO_UNLOCKED_MASK_MSK                           (((1U<<RFIF_INT_LO_UNLOCKED_MASK_LEN)-1)<<RFIF_INT_LO_UNLOCKED_MASK_POS)
#define RFIF_INT_LO_UNLOCKED_MASK_UMSK                          (~(((1U<<RFIF_INT_LO_UNLOCKED_MASK_LEN)-1)<<RFIF_INT_LO_UNLOCKED_MASK_POS))
#define RF_WIFIMODE_4S                                          RF_WIFIMODE_4S
#define RF_WIFIMODE_4S_POS                                      (4U)
#define RF_WIFIMODE_4S_LEN                                      (11U)
#define RF_WIFIMODE_4S_MSK                                      (((1U<<RF_WIFIMODE_4S_LEN)-1)<<RF_WIFIMODE_4S_POS)
#define RF_WIFIMODE_4S_UMSK                                     (~(((1U<<RF_WIFIMODE_4S_LEN)-1)<<RF_WIFIMODE_4S_POS))
#define RFIF_PPUD_CNT2                                          RFIF_PPUD_CNT2
#define RFIF_PPUD_CNT2_POS                                      (16U)
#define RFIF_PPUD_CNT2_LEN                                      (9U)
#define RFIF_PPUD_CNT2_MSK                                      (((1U<<RFIF_PPUD_CNT2_LEN)-1)<<RFIF_PPUD_CNT2_POS)
#define RFIF_PPUD_CNT2_UMSK                                     (~(((1U<<RFIF_PPUD_CNT2_LEN)-1)<<RFIF_PPUD_CNT2_POS))
#define RFIF_PPUD_CNT1                                          RFIF_PPUD_CNT1
#define RFIF_PPUD_CNT1_POS                                      (25U)
#define RFIF_PPUD_CNT1_LEN                                      (5U)
#define RFIF_PPUD_CNT1_MSK                                      (((1U<<RFIF_PPUD_CNT1_LEN)-1)<<RFIF_PPUD_CNT1_POS)
#define RFIF_PPUD_CNT1_UMSK                                     (~(((1U<<RFIF_PPUD_CNT1_LEN)-1)<<RFIF_PPUD_CNT1_POS))
#define RFIF_PPUD_MANAUAL_EN                                    RFIF_PPUD_MANAUAL_EN
#define RFIF_PPUD_MANAUAL_EN_POS                                (30U)
#define RFIF_PPUD_MANAUAL_EN_LEN                                (1U)
#define RFIF_PPUD_MANAUAL_EN_MSK                                (((1U<<RFIF_PPUD_MANAUAL_EN_LEN)-1)<<RFIF_PPUD_MANAUAL_EN_POS)
#define RFIF_PPUD_MANAUAL_EN_UMSK                               (~(((1U<<RFIF_PPUD_MANAUAL_EN_LEN)-1)<<RFIF_PPUD_MANAUAL_EN_POS))

/* 0x22C : rf_data_temp_0 */
#define RF_DATA_TEMP_0_OFFSET                                   (0x22C)
#define RF_DATA_TEMP_0                                          RF_DATA_TEMP_0
#define RF_DATA_TEMP_0_POS                                      (0U)
#define RF_DATA_TEMP_0_LEN                                      (32U)
#define RF_DATA_TEMP_0_MSK                                      (((1U<<RF_DATA_TEMP_0_LEN)-1)<<RF_DATA_TEMP_0_POS)
#define RF_DATA_TEMP_0_UMSK                                     (~(((1U<<RF_DATA_TEMP_0_LEN)-1)<<RF_DATA_TEMP_0_POS))

/* 0x230 : rf_data_temp_1 */
#define RF_DATA_TEMP_1_OFFSET                                   (0x230)
#define RF_DATA_TEMP_1                                          RF_DATA_TEMP_1
#define RF_DATA_TEMP_1_POS                                      (0U)
#define RF_DATA_TEMP_1_LEN                                      (32U)
#define RF_DATA_TEMP_1_MSK                                      (((1U<<RF_DATA_TEMP_1_LEN)-1)<<RF_DATA_TEMP_1_POS)
#define RF_DATA_TEMP_1_UMSK                                     (~(((1U<<RF_DATA_TEMP_1_LEN)-1)<<RF_DATA_TEMP_1_POS))

/* 0x234 : rf_data_temp_2 */
#define RF_DATA_TEMP_2_OFFSET                                   (0x234)
#define RF_DATA_TEMP_2                                          RF_DATA_TEMP_2
#define RF_DATA_TEMP_2_POS                                      (0U)
#define RF_DATA_TEMP_2_LEN                                      (32U)
#define RF_DATA_TEMP_2_MSK                                      (((1U<<RF_DATA_TEMP_2_LEN)-1)<<RF_DATA_TEMP_2_POS)
#define RF_DATA_TEMP_2_UMSK                                     (~(((1U<<RF_DATA_TEMP_2_LEN)-1)<<RF_DATA_TEMP_2_POS))

/* 0x238 : rf_data_temp_3 */
#define RF_DATA_TEMP_3_OFFSET                                   (0x238)
#define RF_DATA_TEMP_3                                          RF_DATA_TEMP_3
#define RF_DATA_TEMP_3_POS                                      (0U)
#define RF_DATA_TEMP_3_LEN                                      (32U)
#define RF_DATA_TEMP_3_MSK                                      (((1U<<RF_DATA_TEMP_3_LEN)-1)<<RF_DATA_TEMP_3_POS)
#define RF_DATA_TEMP_3_UMSK                                     (~(((1U<<RF_DATA_TEMP_3_LEN)-1)<<RF_DATA_TEMP_3_POS))

/* 0x23C : rf_sram_ctrl0 */
#define RF_SRAM_CTRL0_OFFSET                                    (0x23C)
#define RF_SRAM_LINK_DLY                                        RF_SRAM_LINK_DLY
#define RF_SRAM_LINK_DLY_POS                                    (0U)
#define RF_SRAM_LINK_DLY_LEN                                    (16U)
#define RF_SRAM_LINK_DLY_MSK                                    (((1U<<RF_SRAM_LINK_DLY_LEN)-1)<<RF_SRAM_LINK_DLY_POS)
#define RF_SRAM_LINK_DLY_UMSK                                   (~(((1U<<RF_SRAM_LINK_DLY_LEN)-1)<<RF_SRAM_LINK_DLY_POS))
#define RF_SRAM_LINK_MODE                                       RF_SRAM_LINK_MODE
#define RF_SRAM_LINK_MODE_POS                                   (16U)
#define RF_SRAM_LINK_MODE_LEN                                   (2U)
#define RF_SRAM_LINK_MODE_MSK                                   (((1U<<RF_SRAM_LINK_MODE_LEN)-1)<<RF_SRAM_LINK_MODE_POS)
#define RF_SRAM_LINK_MODE_UMSK                                  (~(((1U<<RF_SRAM_LINK_MODE_LEN)-1)<<RF_SRAM_LINK_MODE_POS))
#define RF_SRAM_SWAP                                            RF_SRAM_SWAP
#define RF_SRAM_SWAP_POS                                        (18U)
#define RF_SRAM_SWAP_LEN                                        (1U)
#define RF_SRAM_SWAP_MSK                                        (((1U<<RF_SRAM_SWAP_LEN)-1)<<RF_SRAM_SWAP_POS)
#define RF_SRAM_SWAP_UMSK                                       (~(((1U<<RF_SRAM_SWAP_LEN)-1)<<RF_SRAM_SWAP_POS))
#define RF_SRAM_EXT_CLR                                         RF_SRAM_EXT_CLR
#define RF_SRAM_EXT_CLR_POS                                     (19U)
#define RF_SRAM_EXT_CLR_LEN                                     (1U)
#define RF_SRAM_EXT_CLR_MSK                                     (((1U<<RF_SRAM_EXT_CLR_LEN)-1)<<RF_SRAM_EXT_CLR_POS)
#define RF_SRAM_EXT_CLR_UMSK                                    (~(((1U<<RF_SRAM_EXT_CLR_LEN)-1)<<RF_SRAM_EXT_CLR_POS))
#define RF_SRAM_EXT_SET                                         RF_SRAM_EXT_SET
#define RF_SRAM_EXT_SET_POS                                     (20U)
#define RF_SRAM_EXT_SET_LEN                                     (1U)
#define RF_SRAM_EXT_SET_MSK                                     (((1U<<RF_SRAM_EXT_SET_LEN)-1)<<RF_SRAM_EXT_SET_POS)
#define RF_SRAM_EXT_SET_UMSK                                    (~(((1U<<RF_SRAM_EXT_SET_LEN)-1)<<RF_SRAM_EXT_SET_POS))
#define RF_SRAM_EXT_SRC                                         RF_SRAM_EXT_SRC
#define RF_SRAM_EXT_SRC_POS                                     (21U)
#define RF_SRAM_EXT_SRC_LEN                                     (2U)
#define RF_SRAM_EXT_SRC_MSK                                     (((1U<<RF_SRAM_EXT_SRC_LEN)-1)<<RF_SRAM_EXT_SRC_POS)
#define RF_SRAM_EXT_SRC_UMSK                                    (~(((1U<<RF_SRAM_EXT_SRC_LEN)-1)<<RF_SRAM_EXT_SRC_POS))
#define RF_TX_TEST_2RX_SEL                                      RF_TX_TEST_2RX_SEL
#define RF_TX_TEST_2RX_SEL_POS                                  (23U)
#define RF_TX_TEST_2RX_SEL_LEN                                  (2U)
#define RF_TX_TEST_2RX_SEL_MSK                                  (((1U<<RF_TX_TEST_2RX_SEL_LEN)-1)<<RF_TX_TEST_2RX_SEL_POS)
#define RF_TX_TEST_2RX_SEL_UMSK                                 (~(((1U<<RF_TX_TEST_2RX_SEL_LEN)-1)<<RF_TX_TEST_2RX_SEL_POS))
#define RF_TX_TEST_2RX_EN                                       RF_TX_TEST_2RX_EN
#define RF_TX_TEST_2RX_EN_POS                                   (25U)
#define RF_TX_TEST_2RX_EN_LEN                                   (1U)
#define RF_TX_TEST_2RX_EN_MSK                                   (((1U<<RF_TX_TEST_2RX_EN_LEN)-1)<<RF_TX_TEST_2RX_EN_POS)
#define RF_TX_TEST_2RX_EN_UMSK                                  (~(((1U<<RF_TX_TEST_2RX_EN_LEN)-1)<<RF_TX_TEST_2RX_EN_POS))
#define RF_SRAM_SEL_DMA                                         RF_SRAM_SEL_DMA
#define RF_SRAM_SEL_DMA_POS                                     (31U)
#define RF_SRAM_SEL_DMA_LEN                                     (1U)
#define RF_SRAM_SEL_DMA_MSK                                     (((1U<<RF_SRAM_SEL_DMA_LEN)-1)<<RF_SRAM_SEL_DMA_POS)
#define RF_SRAM_SEL_DMA_UMSK                                    (~(((1U<<RF_SRAM_SEL_DMA_LEN)-1)<<RF_SRAM_SEL_DMA_POS))

/* 0x240 : rf_sram_ctrl1 */
#define RF_SRAM_CTRL1_OFFSET                                    (0x240)
#define RF_SRAM_ADC_DONE                                        RF_SRAM_ADC_DONE
#define RF_SRAM_ADC_DONE_POS                                    (0U)
#define RF_SRAM_ADC_DONE_LEN                                    (1U)
#define RF_SRAM_ADC_DONE_MSK                                    (((1U<<RF_SRAM_ADC_DONE_LEN)-1)<<RF_SRAM_ADC_DONE_POS)
#define RF_SRAM_ADC_DONE_UMSK                                   (~(((1U<<RF_SRAM_ADC_DONE_LEN)-1)<<RF_SRAM_ADC_DONE_POS))
#define RF_SRAM_ADC_EN                                          RF_SRAM_ADC_EN
#define RF_SRAM_ADC_EN_POS                                      (1U)
#define RF_SRAM_ADC_EN_LEN                                      (1U)
#define RF_SRAM_ADC_EN_MSK                                      (((1U<<RF_SRAM_ADC_EN_LEN)-1)<<RF_SRAM_ADC_EN_POS)
#define RF_SRAM_ADC_EN_UMSK                                     (~(((1U<<RF_SRAM_ADC_EN_LEN)-1)<<RF_SRAM_ADC_EN_POS))
#define RF_SRAM_ADC_LOOP_EN                                     RF_SRAM_ADC_LOOP_EN
#define RF_SRAM_ADC_LOOP_EN_POS                                 (2U)
#define RF_SRAM_ADC_LOOP_EN_LEN                                 (1U)
#define RF_SRAM_ADC_LOOP_EN_MSK                                 (((1U<<RF_SRAM_ADC_LOOP_EN_LEN)-1)<<RF_SRAM_ADC_LOOP_EN_POS)
#define RF_SRAM_ADC_LOOP_EN_UMSK                                (~(((1U<<RF_SRAM_ADC_LOOP_EN_LEN)-1)<<RF_SRAM_ADC_LOOP_EN_POS))
#define RF_SRAM_ADC_STS_CLR                                     RF_SRAM_ADC_STS_CLR
#define RF_SRAM_ADC_STS_CLR_POS                                 (3U)
#define RF_SRAM_ADC_STS_CLR_LEN                                 (1U)
#define RF_SRAM_ADC_STS_CLR_MSK                                 (((1U<<RF_SRAM_ADC_STS_CLR_LEN)-1)<<RF_SRAM_ADC_STS_CLR_POS)
#define RF_SRAM_ADC_STS_CLR_UMSK                                (~(((1U<<RF_SRAM_ADC_STS_CLR_LEN)-1)<<RF_SRAM_ADC_STS_CLR_POS))
#define RF_SRAM_ADC_DONE_CNT                                    RF_SRAM_ADC_DONE_CNT
#define RF_SRAM_ADC_DONE_CNT_POS                                (16U)
#define RF_SRAM_ADC_DONE_CNT_LEN                                (16U)
#define RF_SRAM_ADC_DONE_CNT_MSK                                (((1U<<RF_SRAM_ADC_DONE_CNT_LEN)-1)<<RF_SRAM_ADC_DONE_CNT_POS)
#define RF_SRAM_ADC_DONE_CNT_UMSK                               (~(((1U<<RF_SRAM_ADC_DONE_CNT_LEN)-1)<<RF_SRAM_ADC_DONE_CNT_POS))

/* 0x244 : rf_sram_ctrl2 */
#define RF_SRAM_CTRL2_OFFSET                                    (0x244)
#define RF_SRAM_ADC_ADDR_END                                    RF_SRAM_ADC_ADDR_END
#define RF_SRAM_ADC_ADDR_END_POS                                (0U)
#define RF_SRAM_ADC_ADDR_END_LEN                                (16U)
#define RF_SRAM_ADC_ADDR_END_MSK                                (((1U<<RF_SRAM_ADC_ADDR_END_LEN)-1)<<RF_SRAM_ADC_ADDR_END_POS)
#define RF_SRAM_ADC_ADDR_END_UMSK                               (~(((1U<<RF_SRAM_ADC_ADDR_END_LEN)-1)<<RF_SRAM_ADC_ADDR_END_POS))
#define RF_SRAM_ADC_ADDR_START                                  RF_SRAM_ADC_ADDR_START
#define RF_SRAM_ADC_ADDR_START_POS                              (16U)
#define RF_SRAM_ADC_ADDR_START_LEN                              (16U)
#define RF_SRAM_ADC_ADDR_START_MSK                              (((1U<<RF_SRAM_ADC_ADDR_START_LEN)-1)<<RF_SRAM_ADC_ADDR_START_POS)
#define RF_SRAM_ADC_ADDR_START_UMSK                             (~(((1U<<RF_SRAM_ADC_ADDR_START_LEN)-1)<<RF_SRAM_ADC_ADDR_START_POS))

/* 0x248 : rf_sram_ctrl3 */
#define RF_SRAM_CTRL3_OFFSET                                    (0x248)
#define RF_SRAM_ADC_STS                                         RF_SRAM_ADC_STS
#define RF_SRAM_ADC_STS_POS                                     (0U)
#define RF_SRAM_ADC_STS_LEN                                     (32U)
#define RF_SRAM_ADC_STS_MSK                                     (((1U<<RF_SRAM_ADC_STS_LEN)-1)<<RF_SRAM_ADC_STS_POS)
#define RF_SRAM_ADC_STS_UMSK                                    (~(((1U<<RF_SRAM_ADC_STS_LEN)-1)<<RF_SRAM_ADC_STS_POS))

/* 0x24C : rf_sram_ctrl4 */
#define RF_SRAM_CTRL4_OFFSET                                    (0x24C)
#define RF_SRAM_DAC_DONE                                        RF_SRAM_DAC_DONE
#define RF_SRAM_DAC_DONE_POS                                    (0U)
#define RF_SRAM_DAC_DONE_LEN                                    (1U)
#define RF_SRAM_DAC_DONE_MSK                                    (((1U<<RF_SRAM_DAC_DONE_LEN)-1)<<RF_SRAM_DAC_DONE_POS)
#define RF_SRAM_DAC_DONE_UMSK                                   (~(((1U<<RF_SRAM_DAC_DONE_LEN)-1)<<RF_SRAM_DAC_DONE_POS))
#define RF_SRAM_DAC_EN                                          RF_SRAM_DAC_EN
#define RF_SRAM_DAC_EN_POS                                      (1U)
#define RF_SRAM_DAC_EN_LEN                                      (1U)
#define RF_SRAM_DAC_EN_MSK                                      (((1U<<RF_SRAM_DAC_EN_LEN)-1)<<RF_SRAM_DAC_EN_POS)
#define RF_SRAM_DAC_EN_UMSK                                     (~(((1U<<RF_SRAM_DAC_EN_LEN)-1)<<RF_SRAM_DAC_EN_POS))
#define RF_SRAM_DAC_LOOP_EN                                     RF_SRAM_DAC_LOOP_EN
#define RF_SRAM_DAC_LOOP_EN_POS                                 (2U)
#define RF_SRAM_DAC_LOOP_EN_LEN                                 (1U)
#define RF_SRAM_DAC_LOOP_EN_MSK                                 (((1U<<RF_SRAM_DAC_LOOP_EN_LEN)-1)<<RF_SRAM_DAC_LOOP_EN_POS)
#define RF_SRAM_DAC_LOOP_EN_UMSK                                (~(((1U<<RF_SRAM_DAC_LOOP_EN_LEN)-1)<<RF_SRAM_DAC_LOOP_EN_POS))
#define RF_SRAM_DAC_STS_CLR                                     RF_SRAM_DAC_STS_CLR
#define RF_SRAM_DAC_STS_CLR_POS                                 (3U)
#define RF_SRAM_DAC_STS_CLR_LEN                                 (1U)
#define RF_SRAM_DAC_STS_CLR_MSK                                 (((1U<<RF_SRAM_DAC_STS_CLR_LEN)-1)<<RF_SRAM_DAC_STS_CLR_POS)
#define RF_SRAM_DAC_STS_CLR_UMSK                                (~(((1U<<RF_SRAM_DAC_STS_CLR_LEN)-1)<<RF_SRAM_DAC_STS_CLR_POS))
#define RF_SRAM_DAC_DONE_CNT_LIMIT                              RF_SRAM_DAC_DONE_CNT_LIMIT
#define RF_SRAM_DAC_DONE_CNT_LIMIT_POS                          (4U)
#define RF_SRAM_DAC_DONE_CNT_LIMIT_LEN                          (12U)
#define RF_SRAM_DAC_DONE_CNT_LIMIT_MSK                          (((1U<<RF_SRAM_DAC_DONE_CNT_LIMIT_LEN)-1)<<RF_SRAM_DAC_DONE_CNT_LIMIT_POS)
#define RF_SRAM_DAC_DONE_CNT_LIMIT_UMSK                         (~(((1U<<RF_SRAM_DAC_DONE_CNT_LIMIT_LEN)-1)<<RF_SRAM_DAC_DONE_CNT_LIMIT_POS))
#define RF_SRAM_DAC_DONE_CNT                                    RF_SRAM_DAC_DONE_CNT
#define RF_SRAM_DAC_DONE_CNT_POS                                (16U)
#define RF_SRAM_DAC_DONE_CNT_LEN                                (16U)
#define RF_SRAM_DAC_DONE_CNT_MSK                                (((1U<<RF_SRAM_DAC_DONE_CNT_LEN)-1)<<RF_SRAM_DAC_DONE_CNT_POS)
#define RF_SRAM_DAC_DONE_CNT_UMSK                               (~(((1U<<RF_SRAM_DAC_DONE_CNT_LEN)-1)<<RF_SRAM_DAC_DONE_CNT_POS))

/* 0x250 : rf_sram_ctrl5 */
#define RF_SRAM_CTRL5_OFFSET                                    (0x250)
#define RF_SRAM_DAC_ADDR_END                                    RF_SRAM_DAC_ADDR_END
#define RF_SRAM_DAC_ADDR_END_POS                                (0U)
#define RF_SRAM_DAC_ADDR_END_LEN                                (16U)
#define RF_SRAM_DAC_ADDR_END_MSK                                (((1U<<RF_SRAM_DAC_ADDR_END_LEN)-1)<<RF_SRAM_DAC_ADDR_END_POS)
#define RF_SRAM_DAC_ADDR_END_UMSK                               (~(((1U<<RF_SRAM_DAC_ADDR_END_LEN)-1)<<RF_SRAM_DAC_ADDR_END_POS))
#define RF_SRAM_DAC_ADDR_START                                  RF_SRAM_DAC_ADDR_START
#define RF_SRAM_DAC_ADDR_START_POS                              (16U)
#define RF_SRAM_DAC_ADDR_START_LEN                              (16U)
#define RF_SRAM_DAC_ADDR_START_MSK                              (((1U<<RF_SRAM_DAC_ADDR_START_LEN)-1)<<RF_SRAM_DAC_ADDR_START_POS)
#define RF_SRAM_DAC_ADDR_START_UMSK                             (~(((1U<<RF_SRAM_DAC_ADDR_START_LEN)-1)<<RF_SRAM_DAC_ADDR_START_POS))

/* 0x254 : rf_sram_ctrl6 */
#define RF_SRAM_CTRL6_OFFSET                                    (0x254)
#define RF_SRAM_DAC_STS                                         RF_SRAM_DAC_STS
#define RF_SRAM_DAC_STS_POS                                     (0U)
#define RF_SRAM_DAC_STS_LEN                                     (32U)
#define RF_SRAM_DAC_STS_MSK                                     (((1U<<RF_SRAM_DAC_STS_LEN)-1)<<RF_SRAM_DAC_STS_POS)
#define RF_SRAM_DAC_STS_UMSK                                    (~(((1U<<RF_SRAM_DAC_STS_LEN)-1)<<RF_SRAM_DAC_STS_POS))

/* 0x258 : rf_ical_ctrl0 */
#define RF_ICAL_CTRL0_OFFSET                                    (0x258)
#define RF_ICAL_R_CNT_N                                         RF_ICAL_R_CNT_N
#define RF_ICAL_R_CNT_N_POS                                     (0U)
#define RF_ICAL_R_CNT_N_LEN                                     (10U)
#define RF_ICAL_R_CNT_N_MSK                                     (((1U<<RF_ICAL_R_CNT_N_LEN)-1)<<RF_ICAL_R_CNT_N_POS)
#define RF_ICAL_R_CNT_N_UMSK                                    (~(((1U<<RF_ICAL_R_CNT_N_LEN)-1)<<RF_ICAL_R_CNT_N_POS))
#define RF_ICAL_A_CNT_N                                         RF_ICAL_A_CNT_N
#define RF_ICAL_A_CNT_N_POS                                     (10U)
#define RF_ICAL_A_CNT_N_LEN                                     (10U)
#define RF_ICAL_A_CNT_N_MSK                                     (((1U<<RF_ICAL_A_CNT_N_LEN)-1)<<RF_ICAL_A_CNT_N_POS)
#define RF_ICAL_A_CNT_N_UMSK                                    (~(((1U<<RF_ICAL_A_CNT_N_LEN)-1)<<RF_ICAL_A_CNT_N_POS))
#define RF_ICAL_F_CNT_N                                         RF_ICAL_F_CNT_N
#define RF_ICAL_F_CNT_N_POS                                     (20U)
#define RF_ICAL_F_CNT_N_LEN                                     (10U)
#define RF_ICAL_F_CNT_N_MSK                                     (((1U<<RF_ICAL_F_CNT_N_LEN)-1)<<RF_ICAL_F_CNT_N_POS)
#define RF_ICAL_F_CNT_N_UMSK                                    (~(((1U<<RF_ICAL_F_CNT_N_LEN)-1)<<RF_ICAL_F_CNT_N_POS))
#define RF_ICAL_A_UD_INV_EN                                     RF_ICAL_A_UD_INV_EN
#define RF_ICAL_A_UD_INV_EN_POS                                 (30U)
#define RF_ICAL_A_UD_INV_EN_LEN                                 (1U)
#define RF_ICAL_A_UD_INV_EN_MSK                                 (((1U<<RF_ICAL_A_UD_INV_EN_LEN)-1)<<RF_ICAL_A_UD_INV_EN_POS)
#define RF_ICAL_A_UD_INV_EN_UMSK                                (~(((1U<<RF_ICAL_A_UD_INV_EN_LEN)-1)<<RF_ICAL_A_UD_INV_EN_POS))
#define RF_ICAL_F_UD_INV_EN                                     RF_ICAL_F_UD_INV_EN
#define RF_ICAL_F_UD_INV_EN_POS                                 (31U)
#define RF_ICAL_F_UD_INV_EN_LEN                                 (1U)
#define RF_ICAL_F_UD_INV_EN_MSK                                 (((1U<<RF_ICAL_F_UD_INV_EN_LEN)-1)<<RF_ICAL_F_UD_INV_EN_POS)
#define RF_ICAL_F_UD_INV_EN_UMSK                                (~(((1U<<RF_ICAL_F_UD_INV_EN_LEN)-1)<<RF_ICAL_F_UD_INV_EN_POS))

/* 0x25C : rf_ical_ctrl1 */
#define RF_ICAL_CTRL1_OFFSET                                    (0x25C)
#define RF_ICAL_R_AVG_N                                         RF_ICAL_R_AVG_N
#define RF_ICAL_R_AVG_N_POS                                     (0U)
#define RF_ICAL_R_AVG_N_LEN                                     (5U)
#define RF_ICAL_R_AVG_N_MSK                                     (((1U<<RF_ICAL_R_AVG_N_LEN)-1)<<RF_ICAL_R_AVG_N_POS)
#define RF_ICAL_R_AVG_N_UMSK                                    (~(((1U<<RF_ICAL_R_AVG_N_LEN)-1)<<RF_ICAL_R_AVG_N_POS))
#define RF_ICAL_R_OS_Q                                          RF_ICAL_R_OS_Q
#define RF_ICAL_R_OS_Q_POS                                      (10U)
#define RF_ICAL_R_OS_Q_LEN                                      (11U)
#define RF_ICAL_R_OS_Q_MSK                                      (((1U<<RF_ICAL_R_OS_Q_LEN)-1)<<RF_ICAL_R_OS_Q_POS)
#define RF_ICAL_R_OS_Q_UMSK                                     (~(((1U<<RF_ICAL_R_OS_Q_LEN)-1)<<RF_ICAL_R_OS_Q_POS))
#define RF_ICAL_R_OS_I                                          RF_ICAL_R_OS_I
#define RF_ICAL_R_OS_I_POS                                      (21U)
#define RF_ICAL_R_OS_I_LEN                                      (11U)
#define RF_ICAL_R_OS_I_MSK                                      (((1U<<RF_ICAL_R_OS_I_LEN)-1)<<RF_ICAL_R_OS_I_POS)
#define RF_ICAL_R_OS_I_UMSK                                     (~(((1U<<RF_ICAL_R_OS_I_LEN)-1)<<RF_ICAL_R_OS_I_POS))

/* 0x260 : rf_ical_ctrl2 */
#define RF_ICAL_CTRL2_OFFSET                                    (0x260)
#define RF_ICAL_PERIOD_N                                        RF_ICAL_PERIOD_N
#define RF_ICAL_PERIOD_N_POS                                    (0U)
#define RF_ICAL_PERIOD_N_LEN                                    (16U)
#define RF_ICAL_PERIOD_N_MSK                                    (((1U<<RF_ICAL_PERIOD_N_LEN)-1)<<RF_ICAL_PERIOD_N_POS)
#define RF_ICAL_PERIOD_N_UMSK                                   (~(((1U<<RF_ICAL_PERIOD_N_LEN)-1)<<RF_ICAL_PERIOD_N_POS))
#define RF_FSM_IC_BLERX_DIS                                     RF_FSM_IC_BLERX_DIS
#define RF_FSM_IC_BLERX_DIS_POS                                 (29U)
#define RF_FSM_IC_BLERX_DIS_LEN                                 (1U)
#define RF_FSM_IC_BLERX_DIS_MSK                                 (((1U<<RF_FSM_IC_BLERX_DIS_LEN)-1)<<RF_FSM_IC_BLERX_DIS_POS)
#define RF_FSM_IC_BLERX_DIS_UMSK                                (~(((1U<<RF_FSM_IC_BLERX_DIS_LEN)-1)<<RF_FSM_IC_BLERX_DIS_POS))
#define RF_FSM_IC_BLETX_DIS                                     RF_FSM_IC_BLETX_DIS
#define RF_FSM_IC_BLETX_DIS_POS                                 (30U)
#define RF_FSM_IC_BLETX_DIS_LEN                                 (1U)
#define RF_FSM_IC_BLETX_DIS_MSK                                 (((1U<<RF_FSM_IC_BLETX_DIS_LEN)-1)<<RF_FSM_IC_BLETX_DIS_POS)
#define RF_FSM_IC_BLETX_DIS_UMSK                                (~(((1U<<RF_FSM_IC_BLETX_DIS_LEN)-1)<<RF_FSM_IC_BLETX_DIS_POS))
#define RF_FSM_IC_WIFI_DIS                                      RF_FSM_IC_WIFI_DIS
#define RF_FSM_IC_WIFI_DIS_POS                                  (31U)
#define RF_FSM_IC_WIFI_DIS_LEN                                  (1U)
#define RF_FSM_IC_WIFI_DIS_MSK                                  (((1U<<RF_FSM_IC_WIFI_DIS_LEN)-1)<<RF_FSM_IC_WIFI_DIS_POS)
#define RF_FSM_IC_WIFI_DIS_UMSK                                 (~(((1U<<RF_FSM_IC_WIFI_DIS_LEN)-1)<<RF_FSM_IC_WIFI_DIS_POS))

/* 0x264 : rf_fsm_ctrl0 */
#define RF_FSM_CTRL0_OFFSET                                     (0x264)
#define RF_CH_IND_WIFI                                          RF_CH_IND_WIFI
#define RF_CH_IND_WIFI_POS                                      (0U)
#define RF_CH_IND_WIFI_LEN                                      (12U)
#define RF_CH_IND_WIFI_MSK                                      (((1U<<RF_CH_IND_WIFI_LEN)-1)<<RF_CH_IND_WIFI_POS)
#define RF_CH_IND_WIFI_UMSK                                     (~(((1U<<RF_CH_IND_WIFI_LEN)-1)<<RF_CH_IND_WIFI_POS))

/* 0x268 : rf_fsm_ctrl1 */
#define RF_FSM_CTRL1_OFFSET                                     (0x268)
#define RF_FSM_LO_TIME                                          RF_FSM_LO_TIME
#define RF_FSM_LO_TIME_POS                                      (0U)
#define RF_FSM_LO_TIME_LEN                                      (16U)
#define RF_FSM_LO_TIME_MSK                                      (((1U<<RF_FSM_LO_TIME_LEN)-1)<<RF_FSM_LO_TIME_POS)
#define RF_FSM_LO_TIME_UMSK                                     (~(((1U<<RF_FSM_LO_TIME_LEN)-1)<<RF_FSM_LO_TIME_POS))
#define RF_FSM_LO_RDY                                           RF_FSM_LO_RDY
#define RF_FSM_LO_RDY_POS                                       (16U)
#define RF_FSM_LO_RDY_LEN                                       (1U)
#define RF_FSM_LO_RDY_MSK                                       (((1U<<RF_FSM_LO_RDY_LEN)-1)<<RF_FSM_LO_RDY_POS)
#define RF_FSM_LO_RDY_UMSK                                      (~(((1U<<RF_FSM_LO_RDY_LEN)-1)<<RF_FSM_LO_RDY_POS))
#define RF_FSM_LO_RDY_RST                                       RF_FSM_LO_RDY_RST
#define RF_FSM_LO_RDY_RST_POS                                   (17U)
#define RF_FSM_LO_RDY_RST_LEN                                   (1U)
#define RF_FSM_LO_RDY_RST_MSK                                   (((1U<<RF_FSM_LO_RDY_RST_LEN)-1)<<RF_FSM_LO_RDY_RST_POS)
#define RF_FSM_LO_RDY_RST_UMSK                                  (~(((1U<<RF_FSM_LO_RDY_RST_LEN)-1)<<RF_FSM_LO_RDY_RST_POS))
#define RF_FSM_LO_RDY_4S_1                                      RF_FSM_LO_RDY_4S_1
#define RF_FSM_LO_RDY_4S_1_POS                                  (18U)
#define RF_FSM_LO_RDY_4S_1_LEN                                  (1U)
#define RF_FSM_LO_RDY_4S_1_MSK                                  (((1U<<RF_FSM_LO_RDY_4S_1_LEN)-1)<<RF_FSM_LO_RDY_4S_1_POS)
#define RF_FSM_LO_RDY_4S_1_UMSK                                 (~(((1U<<RF_FSM_LO_RDY_4S_1_LEN)-1)<<RF_FSM_LO_RDY_4S_1_POS))
#define RF_FSM_LO_RDY_SBCLR                                     RF_FSM_LO_RDY_SBCLR
#define RF_FSM_LO_RDY_SBCLR_POS                                 (19U)
#define RF_FSM_LO_RDY_SBCLR_LEN                                 (1U)
#define RF_FSM_LO_RDY_SBCLR_MSK                                 (((1U<<RF_FSM_LO_RDY_SBCLR_LEN)-1)<<RF_FSM_LO_RDY_SBCLR_POS)
#define RF_FSM_LO_RDY_SBCLR_UMSK                                (~(((1U<<RF_FSM_LO_RDY_SBCLR_LEN)-1)<<RF_FSM_LO_RDY_SBCLR_POS))
#define RF_FSM_PU_PA_DLY_N                                      RF_FSM_PU_PA_DLY_N
#define RF_FSM_PU_PA_DLY_N_POS                                  (20U)
#define RF_FSM_PU_PA_DLY_N_LEN                                  (10U)
#define RF_FSM_PU_PA_DLY_N_MSK                                  (((1U<<RF_FSM_PU_PA_DLY_N_LEN)-1)<<RF_FSM_PU_PA_DLY_N_POS)
#define RF_FSM_PU_PA_DLY_N_UMSK                                 (~(((1U<<RF_FSM_PU_PA_DLY_N_LEN)-1)<<RF_FSM_PU_PA_DLY_N_POS))

/* 0x26C : rf_fsm_ctrl2 */
#define RF_FSM_CTRL2_OFFSET                                     (0x26C)
#define RF_FSM_ST_DBG                                           RF_FSM_ST_DBG
#define RF_FSM_ST_DBG_POS                                       (0U)
#define RF_FSM_ST_DBG_LEN                                       (3U)
#define RF_FSM_ST_DBG_MSK                                       (((1U<<RF_FSM_ST_DBG_LEN)-1)<<RF_FSM_ST_DBG_POS)
#define RF_FSM_ST_DBG_UMSK                                      (~(((1U<<RF_FSM_ST_DBG_LEN)-1)<<RF_FSM_ST_DBG_POS))
#define RF_FSM_ST_DBG_EN                                        RF_FSM_ST_DBG_EN
#define RF_FSM_ST_DBG_EN_POS                                    (3U)
#define RF_FSM_ST_DBG_EN_LEN                                    (1U)
#define RF_FSM_ST_DBG_EN_MSK                                    (((1U<<RF_FSM_ST_DBG_EN_LEN)-1)<<RF_FSM_ST_DBG_EN_POS)
#define RF_FSM_ST_DBG_EN_UMSK                                   (~(((1U<<RF_FSM_ST_DBG_EN_LEN)-1)<<RF_FSM_ST_DBG_EN_POS))
#define RF_TRX_EN_BLE_4S                                        RF_TRX_EN_BLE_4S
#define RF_TRX_EN_BLE_4S_POS                                    (4U)
#define RF_TRX_EN_BLE_4S_LEN                                    (1U)
#define RF_TRX_EN_BLE_4S_MSK                                    (((1U<<RF_TRX_EN_BLE_4S_LEN)-1)<<RF_TRX_EN_BLE_4S_POS)
#define RF_TRX_EN_BLE_4S_UMSK                                   (~(((1U<<RF_TRX_EN_BLE_4S_LEN)-1)<<RF_TRX_EN_BLE_4S_POS))
#define RF_TRX_SW_BLE_4S                                        RF_TRX_SW_BLE_4S
#define RF_TRX_SW_BLE_4S_POS                                    (5U)
#define RF_TRX_SW_BLE_4S_LEN                                    (1U)
#define RF_TRX_SW_BLE_4S_MSK                                    (((1U<<RF_TRX_SW_BLE_4S_LEN)-1)<<RF_TRX_SW_BLE_4S_POS)
#define RF_TRX_SW_BLE_4S_UMSK                                   (~(((1U<<RF_TRX_SW_BLE_4S_LEN)-1)<<RF_TRX_SW_BLE_4S_POS))
#define RF_TRX_BLE_4S_EN                                        RF_TRX_BLE_4S_EN
#define RF_TRX_BLE_4S_EN_POS                                    (6U)
#define RF_TRX_BLE_4S_EN_LEN                                    (1U)
#define RF_TRX_BLE_4S_EN_MSK                                    (((1U<<RF_TRX_BLE_4S_EN_LEN)-1)<<RF_TRX_BLE_4S_EN_POS)
#define RF_TRX_BLE_4S_EN_UMSK                                   (~(((1U<<RF_TRX_BLE_4S_EN_LEN)-1)<<RF_TRX_BLE_4S_EN_POS))
#define RF_M154_RX_START_4S                                     RF_M154_RX_START_4S
#define RF_M154_RX_START_4S_POS                                 (7U)
#define RF_M154_RX_START_4S_LEN                                 (1U)
#define RF_M154_RX_START_4S_MSK                                 (((1U<<RF_M154_RX_START_4S_LEN)-1)<<RF_M154_RX_START_4S_POS)
#define RF_M154_RX_START_4S_UMSK                                (~(((1U<<RF_M154_RX_START_4S_LEN)-1)<<RF_M154_RX_START_4S_POS))
#define RF_M154_TX_START_4S                                     RF_M154_TX_START_4S
#define RF_M154_TX_START_4S_POS                                 (8U)
#define RF_M154_TX_START_4S_LEN                                 (1U)
#define RF_M154_TX_START_4S_MSK                                 (((1U<<RF_M154_TX_START_4S_LEN)-1)<<RF_M154_TX_START_4S_POS)
#define RF_M154_TX_START_4S_UMSK                                (~(((1U<<RF_M154_TX_START_4S_LEN)-1)<<RF_M154_TX_START_4S_POS))
#define RF_TRX_M154_4S_EN                                       RF_TRX_M154_4S_EN
#define RF_TRX_M154_4S_EN_POS                                   (9U)
#define RF_TRX_M154_4S_EN_LEN                                   (1U)
#define RF_TRX_M154_4S_EN_MSK                                   (((1U<<RF_TRX_M154_4S_EN_LEN)-1)<<RF_TRX_M154_4S_EN_POS)
#define RF_TRX_M154_4S_EN_UMSK                                  (~(((1U<<RF_TRX_M154_4S_EN_LEN)-1)<<RF_TRX_M154_4S_EN_POS))
#define RF_FSM_DFE_TX_DLY_N                                     RF_FSM_DFE_TX_DLY_N
#define RF_FSM_DFE_TX_DLY_N_POS                                 (10U)
#define RF_FSM_DFE_TX_DLY_N_LEN                                 (10U)
#define RF_FSM_DFE_TX_DLY_N_MSK                                 (((1U<<RF_FSM_DFE_TX_DLY_N_LEN)-1)<<RF_FSM_DFE_TX_DLY_N_POS)
#define RF_FSM_DFE_TX_DLY_N_UMSK                                (~(((1U<<RF_FSM_DFE_TX_DLY_N_LEN)-1)<<RF_FSM_DFE_TX_DLY_N_POS))
#define RF_FSM_DFE_RX_DLY_N                                     RF_FSM_DFE_RX_DLY_N
#define RF_FSM_DFE_RX_DLY_N_POS                                 (20U)
#define RF_FSM_DFE_RX_DLY_N_LEN                                 (10U)
#define RF_FSM_DFE_RX_DLY_N_MSK                                 (((1U<<RF_FSM_DFE_RX_DLY_N_LEN)-1)<<RF_FSM_DFE_RX_DLY_N_POS)
#define RF_FSM_DFE_RX_DLY_N_UMSK                                (~(((1U<<RF_FSM_DFE_RX_DLY_N_LEN)-1)<<RF_FSM_DFE_RX_DLY_N_POS))

/* 0x270 : rf_pkdet_ctrl0 */
#define RF_PKDET_CTRL0_OFFSET                                   (0x270)
#define RF_PKDET_OUT_CNT_STS                                    RF_PKDET_OUT_CNT_STS
#define RF_PKDET_OUT_CNT_STS_POS                                (0U)
#define RF_PKDET_OUT_CNT_STS_LEN                                (4U)
#define RF_PKDET_OUT_CNT_STS_MSK                                (((1U<<RF_PKDET_OUT_CNT_STS_LEN)-1)<<RF_PKDET_OUT_CNT_STS_POS)
#define RF_PKDET_OUT_CNT_STS_UMSK                               (~(((1U<<RF_PKDET_OUT_CNT_STS_LEN)-1)<<RF_PKDET_OUT_CNT_STS_POS))
#define RF_PKDET_OUT_CNT_EN                                     RF_PKDET_OUT_CNT_EN
#define RF_PKDET_OUT_CNT_EN_POS                                 (4U)
#define RF_PKDET_OUT_CNT_EN_LEN                                 (1U)
#define RF_PKDET_OUT_CNT_EN_MSK                                 (((1U<<RF_PKDET_OUT_CNT_EN_LEN)-1)<<RF_PKDET_OUT_CNT_EN_POS)
#define RF_PKDET_OUT_CNT_EN_UMSK                                (~(((1U<<RF_PKDET_OUT_CNT_EN_LEN)-1)<<RF_PKDET_OUT_CNT_EN_POS))
#define RF_PKDET_OUT_MODE                                       RF_PKDET_OUT_MODE
#define RF_PKDET_OUT_MODE_POS                                   (5U)
#define RF_PKDET_OUT_MODE_LEN                                   (1U)
#define RF_PKDET_OUT_MODE_MSK                                   (((1U<<RF_PKDET_OUT_MODE_LEN)-1)<<RF_PKDET_OUT_MODE_POS)
#define RF_PKDET_OUT_MODE_UMSK                                  (~(((1U<<RF_PKDET_OUT_MODE_LEN)-1)<<RF_PKDET_OUT_MODE_POS))

/* 0x274 : rf_fsm_ctrl3 */
#define RF_FSM_CTRL3_OFFSET                                     (0x274)
#define RF_PHY_RXMODE_4S_VAL                                    RF_PHY_RXMODE_4S_VAL
#define RF_PHY_RXMODE_4S_VAL_POS                                (0U)
#define RF_PHY_RXMODE_4S_VAL_LEN                                (3U)
#define RF_PHY_RXMODE_4S_VAL_MSK                                (((1U<<RF_PHY_RXMODE_4S_VAL_LEN)-1)<<RF_PHY_RXMODE_4S_VAL_POS)
#define RF_PHY_RXMODE_4S_VAL_UMSK                               (~(((1U<<RF_PHY_RXMODE_4S_VAL_LEN)-1)<<RF_PHY_RXMODE_4S_VAL_POS))
#define RF_PHY_RXMODE_4S_EN                                     RF_PHY_RXMODE_4S_EN
#define RF_PHY_RXMODE_4S_EN_POS                                 (3U)
#define RF_PHY_RXMODE_4S_EN_LEN                                 (1U)
#define RF_PHY_RXMODE_4S_EN_MSK                                 (((1U<<RF_PHY_RXMODE_4S_EN_LEN)-1)<<RF_PHY_RXMODE_4S_EN_POS)
#define RF_PHY_RXMODE_4S_EN_UMSK                                (~(((1U<<RF_PHY_RXMODE_4S_EN_LEN)-1)<<RF_PHY_RXMODE_4S_EN_POS))
#define RF_PHY_TXMODE_4S_VAL                                    RF_PHY_TXMODE_4S_VAL
#define RF_PHY_TXMODE_4S_VAL_POS                                (4U)
#define RF_PHY_TXMODE_4S_VAL_LEN                                (3U)
#define RF_PHY_TXMODE_4S_VAL_MSK                                (((1U<<RF_PHY_TXMODE_4S_VAL_LEN)-1)<<RF_PHY_TXMODE_4S_VAL_POS)
#define RF_PHY_TXMODE_4S_VAL_UMSK                               (~(((1U<<RF_PHY_TXMODE_4S_VAL_LEN)-1)<<RF_PHY_TXMODE_4S_VAL_POS))
#define RF_PHY_TXMODE_4S_EN                                     RF_PHY_TXMODE_4S_EN
#define RF_PHY_TXMODE_4S_EN_POS                                 (7U)
#define RF_PHY_TXMODE_4S_EN_LEN                                 (1U)
#define RF_PHY_TXMODE_4S_EN_MSK                                 (((1U<<RF_PHY_TXMODE_4S_EN_LEN)-1)<<RF_PHY_TXMODE_4S_EN_POS)
#define RF_PHY_TXMODE_4S_EN_UMSK                                (~(((1U<<RF_PHY_TXMODE_4S_EN_LEN)-1)<<RF_PHY_TXMODE_4S_EN_POS))

/* 0x278 : rf_dma_ctrl0 */
#define RF_DMA_CTRL0_OFFSET                                     (0x278)
#define RF_DMA_ADC_DONE                                         RF_DMA_ADC_DONE
#define RF_DMA_ADC_DONE_POS                                     (0U)
#define RF_DMA_ADC_DONE_LEN                                     (1U)
#define RF_DMA_ADC_DONE_MSK                                     (((1U<<RF_DMA_ADC_DONE_LEN)-1)<<RF_DMA_ADC_DONE_POS)
#define RF_DMA_ADC_DONE_UMSK                                    (~(((1U<<RF_DMA_ADC_DONE_LEN)-1)<<RF_DMA_ADC_DONE_POS))
#define RF_DMA_ADC_EN                                           RF_DMA_ADC_EN
#define RF_DMA_ADC_EN_POS                                       (1U)
#define RF_DMA_ADC_EN_LEN                                       (1U)
#define RF_DMA_ADC_EN_MSK                                       (((1U<<RF_DMA_ADC_EN_LEN)-1)<<RF_DMA_ADC_EN_POS)
#define RF_DMA_ADC_EN_UMSK                                      (~(((1U<<RF_DMA_ADC_EN_LEN)-1)<<RF_DMA_ADC_EN_POS))
#define RF_DMA_ADC_LOOP_EN                                      RF_DMA_ADC_LOOP_EN
#define RF_DMA_ADC_LOOP_EN_POS                                  (2U)
#define RF_DMA_ADC_LOOP_EN_LEN                                  (1U)
#define RF_DMA_ADC_LOOP_EN_MSK                                  (((1U<<RF_DMA_ADC_LOOP_EN_LEN)-1)<<RF_DMA_ADC_LOOP_EN_POS)
#define RF_DMA_ADC_LOOP_EN_UMSK                                 (~(((1U<<RF_DMA_ADC_LOOP_EN_LEN)-1)<<RF_DMA_ADC_LOOP_EN_POS))
#define RF_DMA_ADC_OSTD_LMT                                     RF_DMA_ADC_OSTD_LMT
#define RF_DMA_ADC_OSTD_LMT_POS                                 (3U)
#define RF_DMA_ADC_OSTD_LMT_LEN                                 (3U)
#define RF_DMA_ADC_OSTD_LMT_MSK                                 (((1U<<RF_DMA_ADC_OSTD_LMT_LEN)-1)<<RF_DMA_ADC_OSTD_LMT_POS)
#define RF_DMA_ADC_OSTD_LMT_UMSK                                (~(((1U<<RF_DMA_ADC_OSTD_LMT_LEN)-1)<<RF_DMA_ADC_OSTD_LMT_POS))
#define RF_DMA_ADC_WCMD_LEN                                     RF_DMA_ADC_WCMD_LEN
#define RF_DMA_ADC_WCMD_LEN_POS                                 (6U)
#define RF_DMA_ADC_WCMD_LEN_LEN                                 (6U)
#define RF_DMA_ADC_WCMD_LEN_MSK                                 (((1U<<RF_DMA_ADC_WCMD_LEN_LEN)-1)<<RF_DMA_ADC_WCMD_LEN_POS)
#define RF_DMA_ADC_WCMD_LEN_UMSK                                (~(((1U<<RF_DMA_ADC_WCMD_LEN_LEN)-1)<<RF_DMA_ADC_WCMD_LEN_POS))
#define RF_DMA_ADC_AXI_W_IDLE                                   RF_DMA_ADC_AXI_W_IDLE
#define RF_DMA_ADC_AXI_W_IDLE_POS                               (12U)
#define RF_DMA_ADC_AXI_W_IDLE_LEN                               (1U)
#define RF_DMA_ADC_AXI_W_IDLE_MSK                               (((1U<<RF_DMA_ADC_AXI_W_IDLE_LEN)-1)<<RF_DMA_ADC_AXI_W_IDLE_POS)
#define RF_DMA_ADC_AXI_W_IDLE_UMSK                              (~(((1U<<RF_DMA_ADC_AXI_W_IDLE_LEN)-1)<<RF_DMA_ADC_AXI_W_IDLE_POS))
#define RF_DMA_ADC_DONE_CNT                                     RF_DMA_ADC_DONE_CNT
#define RF_DMA_ADC_DONE_CNT_POS                                 (16U)
#define RF_DMA_ADC_DONE_CNT_LEN                                 (16U)
#define RF_DMA_ADC_DONE_CNT_MSK                                 (((1U<<RF_DMA_ADC_DONE_CNT_LEN)-1)<<RF_DMA_ADC_DONE_CNT_POS)
#define RF_DMA_ADC_DONE_CNT_UMSK                                (~(((1U<<RF_DMA_ADC_DONE_CNT_LEN)-1)<<RF_DMA_ADC_DONE_CNT_POS))

/* 0x27C : rf_dma_ctrl1 */
#define RF_DMA_CTRL1_OFFSET                                     (0x27C)
#define RF_DMA_ADC_ADDR_START                                   RF_DMA_ADC_ADDR_START
#define RF_DMA_ADC_ADDR_START_POS                               (0U)
#define RF_DMA_ADC_ADDR_START_LEN                               (32U)
#define RF_DMA_ADC_ADDR_START_MSK                               (((1U<<RF_DMA_ADC_ADDR_START_LEN)-1)<<RF_DMA_ADC_ADDR_START_POS)
#define RF_DMA_ADC_ADDR_START_UMSK                              (~(((1U<<RF_DMA_ADC_ADDR_START_LEN)-1)<<RF_DMA_ADC_ADDR_START_POS))

/* 0x280 : rf_dma_ctrl2 */
#define RF_DMA_CTRL2_OFFSET                                     (0x280)
#define RF_DMA_ADC_ADDR_CNT                                     RF_DMA_ADC_ADDR_CNT
#define RF_DMA_ADC_ADDR_CNT_POS                                 (0U)
#define RF_DMA_ADC_ADDR_CNT_LEN                                 (32U)
#define RF_DMA_ADC_ADDR_CNT_MSK                                 (((1U<<RF_DMA_ADC_ADDR_CNT_LEN)-1)<<RF_DMA_ADC_ADDR_CNT_POS)
#define RF_DMA_ADC_ADDR_CNT_UMSK                                (~(((1U<<RF_DMA_ADC_ADDR_CNT_LEN)-1)<<RF_DMA_ADC_ADDR_CNT_POS))

/* 0x284 : rf_dma_ctrl3 */
#define RF_DMA_CTRL3_OFFSET                                     (0x284)
#define RF_DMA_DAC_DONE                                         RF_DMA_DAC_DONE
#define RF_DMA_DAC_DONE_POS                                     (0U)
#define RF_DMA_DAC_DONE_LEN                                     (1U)
#define RF_DMA_DAC_DONE_MSK                                     (((1U<<RF_DMA_DAC_DONE_LEN)-1)<<RF_DMA_DAC_DONE_POS)
#define RF_DMA_DAC_DONE_UMSK                                    (~(((1U<<RF_DMA_DAC_DONE_LEN)-1)<<RF_DMA_DAC_DONE_POS))
#define RF_DMA_DAC_EN                                           RF_DMA_DAC_EN
#define RF_DMA_DAC_EN_POS                                       (1U)
#define RF_DMA_DAC_EN_LEN                                       (1U)
#define RF_DMA_DAC_EN_MSK                                       (((1U<<RF_DMA_DAC_EN_LEN)-1)<<RF_DMA_DAC_EN_POS)
#define RF_DMA_DAC_EN_UMSK                                      (~(((1U<<RF_DMA_DAC_EN_LEN)-1)<<RF_DMA_DAC_EN_POS))
#define RF_DMA_DAC_LOOP_EN                                      RF_DMA_DAC_LOOP_EN
#define RF_DMA_DAC_LOOP_EN_POS                                  (2U)
#define RF_DMA_DAC_LOOP_EN_LEN                                  (1U)
#define RF_DMA_DAC_LOOP_EN_MSK                                  (((1U<<RF_DMA_DAC_LOOP_EN_LEN)-1)<<RF_DMA_DAC_LOOP_EN_POS)
#define RF_DMA_DAC_LOOP_EN_UMSK                                 (~(((1U<<RF_DMA_DAC_LOOP_EN_LEN)-1)<<RF_DMA_DAC_LOOP_EN_POS))
#define RF_DMA_DAC_OSTD_LMT                                     RF_DMA_DAC_OSTD_LMT
#define RF_DMA_DAC_OSTD_LMT_POS                                 (3U)
#define RF_DMA_DAC_OSTD_LMT_LEN                                 (3U)
#define RF_DMA_DAC_OSTD_LMT_MSK                                 (((1U<<RF_DMA_DAC_OSTD_LMT_LEN)-1)<<RF_DMA_DAC_OSTD_LMT_POS)
#define RF_DMA_DAC_OSTD_LMT_UMSK                                (~(((1U<<RF_DMA_DAC_OSTD_LMT_LEN)-1)<<RF_DMA_DAC_OSTD_LMT_POS))
#define RF_DMA_DAC_RCMD_LEN                                     RF_DMA_DAC_RCMD_LEN
#define RF_DMA_DAC_RCMD_LEN_POS                                 (6U)
#define RF_DMA_DAC_RCMD_LEN_LEN                                 (6U)
#define RF_DMA_DAC_RCMD_LEN_MSK                                 (((1U<<RF_DMA_DAC_RCMD_LEN_LEN)-1)<<RF_DMA_DAC_RCMD_LEN_POS)
#define RF_DMA_DAC_RCMD_LEN_UMSK                                (~(((1U<<RF_DMA_DAC_RCMD_LEN_LEN)-1)<<RF_DMA_DAC_RCMD_LEN_POS))
#define RF_DMA_DAC_AXI_R_IDLE                                   RF_DMA_DAC_AXI_R_IDLE
#define RF_DMA_DAC_AXI_R_IDLE_POS                               (12U)
#define RF_DMA_DAC_AXI_R_IDLE_LEN                               (1U)
#define RF_DMA_DAC_AXI_R_IDLE_MSK                               (((1U<<RF_DMA_DAC_AXI_R_IDLE_LEN)-1)<<RF_DMA_DAC_AXI_R_IDLE_POS)
#define RF_DMA_DAC_AXI_R_IDLE_UMSK                              (~(((1U<<RF_DMA_DAC_AXI_R_IDLE_LEN)-1)<<RF_DMA_DAC_AXI_R_IDLE_POS))
#define RF_DMA_DAC_DONE_CNT                                     RF_DMA_DAC_DONE_CNT
#define RF_DMA_DAC_DONE_CNT_POS                                 (16U)
#define RF_DMA_DAC_DONE_CNT_LEN                                 (16U)
#define RF_DMA_DAC_DONE_CNT_MSK                                 (((1U<<RF_DMA_DAC_DONE_CNT_LEN)-1)<<RF_DMA_DAC_DONE_CNT_POS)
#define RF_DMA_DAC_DONE_CNT_UMSK                                (~(((1U<<RF_DMA_DAC_DONE_CNT_LEN)-1)<<RF_DMA_DAC_DONE_CNT_POS))

/* 0x288 : rf_dma_ctrl5 */
#define RF_DMA_CTRL5_OFFSET                                     (0x288)
#define RF_DMA_DAC_ADDR_START                                   RF_DMA_DAC_ADDR_START
#define RF_DMA_DAC_ADDR_START_POS                               (0U)
#define RF_DMA_DAC_ADDR_START_LEN                               (32U)
#define RF_DMA_DAC_ADDR_START_MSK                               (((1U<<RF_DMA_DAC_ADDR_START_LEN)-1)<<RF_DMA_DAC_ADDR_START_POS)
#define RF_DMA_DAC_ADDR_START_UMSK                              (~(((1U<<RF_DMA_DAC_ADDR_START_LEN)-1)<<RF_DMA_DAC_ADDR_START_POS))

/* 0x28C : rf_dma_ctrl6 */
#define RF_DMA_CTRL6_OFFSET                                     (0x28C)
#define RF_DMA_DAC_ADDR_CNT                                     RF_DMA_DAC_ADDR_CNT
#define RF_DMA_DAC_ADDR_CNT_POS                                 (0U)
#define RF_DMA_DAC_ADDR_CNT_LEN                                 (32U)
#define RF_DMA_DAC_ADDR_CNT_MSK                                 (((1U<<RF_DMA_DAC_ADDR_CNT_LEN)-1)<<RF_DMA_DAC_ADDR_CNT_POS)
#define RF_DMA_DAC_ADDR_CNT_UMSK                                (~(((1U<<RF_DMA_DAC_ADDR_CNT_LEN)-1)<<RF_DMA_DAC_ADDR_CNT_POS))

/* 0x290 : rf_ILA_ctrl0 */
#define RF_ILA_CTRL0_OFFSET                                     (0x290)
#define RF_CFG_ILA_EN                                           RF_CFG_ILA_EN
#define RF_CFG_ILA_EN_POS                                       (0U)
#define RF_CFG_ILA_EN_LEN                                       (1U)
#define RF_CFG_ILA_EN_MSK                                       (((1U<<RF_CFG_ILA_EN_LEN)-1)<<RF_CFG_ILA_EN_POS)
#define RF_CFG_ILA_EN_UMSK                                      (~(((1U<<RF_CFG_ILA_EN_LEN)-1)<<RF_CFG_ILA_EN_POS))
#define RF_CFG_ILA_DECI                                         RF_CFG_ILA_DECI
#define RF_CFG_ILA_DECI_POS                                     (1U)
#define RF_CFG_ILA_DECI_LEN                                     (3U)
#define RF_CFG_ILA_DECI_MSK                                     (((1U<<RF_CFG_ILA_DECI_LEN)-1)<<RF_CFG_ILA_DECI_POS)
#define RF_CFG_ILA_DECI_UMSK                                    (~(((1U<<RF_CFG_ILA_DECI_LEN)-1)<<RF_CFG_ILA_DECI_POS))
#define RF_CFG_ILA_TRIG_PH_OP                                   RF_CFG_ILA_TRIG_PH_OP
#define RF_CFG_ILA_TRIG_PH_OP_POS                               (4U)
#define RF_CFG_ILA_TRIG_PH_OP_LEN                               (4U)
#define RF_CFG_ILA_TRIG_PH_OP_MSK                               (((1U<<RF_CFG_ILA_TRIG_PH_OP_LEN)-1)<<RF_CFG_ILA_TRIG_PH_OP_POS)
#define RF_CFG_ILA_TRIG_PH_OP_UMSK                              (~(((1U<<RF_CFG_ILA_TRIG_PH_OP_LEN)-1)<<RF_CFG_ILA_TRIG_PH_OP_POS))
#define RF_CFG_ILA_TRIG_SRC_SEL1                                RF_CFG_ILA_TRIG_SRC_SEL1
#define RF_CFG_ILA_TRIG_SRC_SEL1_POS                            (8U)
#define RF_CFG_ILA_TRIG_SRC_SEL1_LEN                            (3U)
#define RF_CFG_ILA_TRIG_SRC_SEL1_MSK                            (((1U<<RF_CFG_ILA_TRIG_SRC_SEL1_LEN)-1)<<RF_CFG_ILA_TRIG_SRC_SEL1_POS)
#define RF_CFG_ILA_TRIG_SRC_SEL1_UMSK                           (~(((1U<<RF_CFG_ILA_TRIG_SRC_SEL1_LEN)-1)<<RF_CFG_ILA_TRIG_SRC_SEL1_POS))
#define RF_CFG_ILA_TRIG_SRC_SEL2                                RF_CFG_ILA_TRIG_SRC_SEL2
#define RF_CFG_ILA_TRIG_SRC_SEL2_POS                            (12U)
#define RF_CFG_ILA_TRIG_SRC_SEL2_LEN                            (3U)
#define RF_CFG_ILA_TRIG_SRC_SEL2_MSK                            (((1U<<RF_CFG_ILA_TRIG_SRC_SEL2_LEN)-1)<<RF_CFG_ILA_TRIG_SRC_SEL2_POS)
#define RF_CFG_ILA_TRIG_SRC_SEL2_UMSK                           (~(((1U<<RF_CFG_ILA_TRIG_SRC_SEL2_LEN)-1)<<RF_CFG_ILA_TRIG_SRC_SEL2_POS))
#define RF_CFG_ILA_TRIG_SRC_SEL3                                RF_CFG_ILA_TRIG_SRC_SEL3
#define RF_CFG_ILA_TRIG_SRC_SEL3_POS                            (16U)
#define RF_CFG_ILA_TRIG_SRC_SEL3_LEN                            (3U)
#define RF_CFG_ILA_TRIG_SRC_SEL3_MSK                            (((1U<<RF_CFG_ILA_TRIG_SRC_SEL3_LEN)-1)<<RF_CFG_ILA_TRIG_SRC_SEL3_POS)
#define RF_CFG_ILA_TRIG_SRC_SEL3_UMSK                           (~(((1U<<RF_CFG_ILA_TRIG_SRC_SEL3_LEN)-1)<<RF_CFG_ILA_TRIG_SRC_SEL3_POS))
#define RF_CFG_ILA_FORCE_STOP                                   RF_CFG_ILA_FORCE_STOP
#define RF_CFG_ILA_FORCE_STOP_POS                               (20U)
#define RF_CFG_ILA_FORCE_STOP_LEN                               (1U)
#define RF_CFG_ILA_FORCE_STOP_MSK                               (((1U<<RF_CFG_ILA_FORCE_STOP_LEN)-1)<<RF_CFG_ILA_FORCE_STOP_POS)
#define RF_CFG_ILA_FORCE_STOP_UMSK                              (~(((1U<<RF_CFG_ILA_FORCE_STOP_LEN)-1)<<RF_CFG_ILA_FORCE_STOP_POS))
#define RF_CFG_ILA_DATA_SRC_TO_SRAM                             RF_CFG_ILA_DATA_SRC_TO_SRAM
#define RF_CFG_ILA_DATA_SRC_TO_SRAM_POS                         (21U)
#define RF_CFG_ILA_DATA_SRC_TO_SRAM_LEN                         (1U)
#define RF_CFG_ILA_DATA_SRC_TO_SRAM_MSK                         (((1U<<RF_CFG_ILA_DATA_SRC_TO_SRAM_LEN)-1)<<RF_CFG_ILA_DATA_SRC_TO_SRAM_POS)
#define RF_CFG_ILA_DATA_SRC_TO_SRAM_UMSK                        (~(((1U<<RF_CFG_ILA_DATA_SRC_TO_SRAM_LEN)-1)<<RF_CFG_ILA_DATA_SRC_TO_SRAM_POS))
#define RF_CFG_ILA_TRIG_OP_SEL1                                 RF_CFG_ILA_TRIG_OP_SEL1
#define RF_CFG_ILA_TRIG_OP_SEL1_POS                             (22U)
#define RF_CFG_ILA_TRIG_OP_SEL1_LEN                             (2U)
#define RF_CFG_ILA_TRIG_OP_SEL1_MSK                             (((1U<<RF_CFG_ILA_TRIG_OP_SEL1_LEN)-1)<<RF_CFG_ILA_TRIG_OP_SEL1_POS)
#define RF_CFG_ILA_TRIG_OP_SEL1_UMSK                            (~(((1U<<RF_CFG_ILA_TRIG_OP_SEL1_LEN)-1)<<RF_CFG_ILA_TRIG_OP_SEL1_POS))
#define RF_CFG_ILA_TRIG_OP_SEL2                                 RF_CFG_ILA_TRIG_OP_SEL2
#define RF_CFG_ILA_TRIG_OP_SEL2_POS                             (24U)
#define RF_CFG_ILA_TRIG_OP_SEL2_LEN                             (2U)
#define RF_CFG_ILA_TRIG_OP_SEL2_MSK                             (((1U<<RF_CFG_ILA_TRIG_OP_SEL2_LEN)-1)<<RF_CFG_ILA_TRIG_OP_SEL2_POS)
#define RF_CFG_ILA_TRIG_OP_SEL2_UMSK                            (~(((1U<<RF_CFG_ILA_TRIG_OP_SEL2_LEN)-1)<<RF_CFG_ILA_TRIG_OP_SEL2_POS))
#define RF_CFG_ILA_TRIG_OP_SEL3                                 RF_CFG_ILA_TRIG_OP_SEL3
#define RF_CFG_ILA_TRIG_OP_SEL3_POS                             (26U)
#define RF_CFG_ILA_TRIG_OP_SEL3_LEN                             (2U)
#define RF_CFG_ILA_TRIG_OP_SEL3_MSK                             (((1U<<RF_CFG_ILA_TRIG_OP_SEL3_LEN)-1)<<RF_CFG_ILA_TRIG_OP_SEL3_POS)
#define RF_CFG_ILA_TRIG_OP_SEL3_UMSK                            (~(((1U<<RF_CFG_ILA_TRIG_OP_SEL3_LEN)-1)<<RF_CFG_ILA_TRIG_OP_SEL3_POS))
#define RF_CFG_ILA_DATA_SRC_SEL                                 RF_CFG_ILA_DATA_SRC_SEL
#define RF_CFG_ILA_DATA_SRC_SEL_POS                             (28U)
#define RF_CFG_ILA_DATA_SRC_SEL_LEN                             (3U)
#define RF_CFG_ILA_DATA_SRC_SEL_MSK                             (((1U<<RF_CFG_ILA_DATA_SRC_SEL_LEN)-1)<<RF_CFG_ILA_DATA_SRC_SEL_POS)
#define RF_CFG_ILA_DATA_SRC_SEL_UMSK                            (~(((1U<<RF_CFG_ILA_DATA_SRC_SEL_LEN)-1)<<RF_CFG_ILA_DATA_SRC_SEL_POS))
#define RF_ILA_DONE                                             RF_ILA_DONE
#define RF_ILA_DONE_POS                                         (31U)
#define RF_ILA_DONE_LEN                                         (1U)
#define RF_ILA_DONE_MSK                                         (((1U<<RF_ILA_DONE_LEN)-1)<<RF_ILA_DONE_POS)
#define RF_ILA_DONE_UMSK                                        (~(((1U<<RF_ILA_DONE_LEN)-1)<<RF_ILA_DONE_POS))

/* 0x294 : rf_ILA_ctrl1 */
#define RF_ILA_CTRL1_OFFSET                                     (0x294)
#define RF_CFG_ILA_TRIG_MASK_SEL1                               RF_CFG_ILA_TRIG_MASK_SEL1
#define RF_CFG_ILA_TRIG_MASK_SEL1_POS                           (0U)
#define RF_CFG_ILA_TRIG_MASK_SEL1_LEN                           (32U)
#define RF_CFG_ILA_TRIG_MASK_SEL1_MSK                           (((1U<<RF_CFG_ILA_TRIG_MASK_SEL1_LEN)-1)<<RF_CFG_ILA_TRIG_MASK_SEL1_POS)
#define RF_CFG_ILA_TRIG_MASK_SEL1_UMSK                          (~(((1U<<RF_CFG_ILA_TRIG_MASK_SEL1_LEN)-1)<<RF_CFG_ILA_TRIG_MASK_SEL1_POS))

/* 0x298 : rf_ILA_ctrl2 */
#define RF_ILA_CTRL2_OFFSET                                     (0x298)
#define RF_CFG_ILA_TRIG_MASK_SEL2                               RF_CFG_ILA_TRIG_MASK_SEL2
#define RF_CFG_ILA_TRIG_MASK_SEL2_POS                           (0U)
#define RF_CFG_ILA_TRIG_MASK_SEL2_LEN                           (32U)
#define RF_CFG_ILA_TRIG_MASK_SEL2_MSK                           (((1U<<RF_CFG_ILA_TRIG_MASK_SEL2_LEN)-1)<<RF_CFG_ILA_TRIG_MASK_SEL2_POS)
#define RF_CFG_ILA_TRIG_MASK_SEL2_UMSK                          (~(((1U<<RF_CFG_ILA_TRIG_MASK_SEL2_LEN)-1)<<RF_CFG_ILA_TRIG_MASK_SEL2_POS))

/* 0x29C : rf_ILA_ctrl3 */
#define RF_ILA_CTRL3_OFFSET                                     (0x29C)
#define RF_CFG_ILA_TRIG_MASK_SEL3                               RF_CFG_ILA_TRIG_MASK_SEL3
#define RF_CFG_ILA_TRIG_MASK_SEL3_POS                           (0U)
#define RF_CFG_ILA_TRIG_MASK_SEL3_LEN                           (32U)
#define RF_CFG_ILA_TRIG_MASK_SEL3_MSK                           (((1U<<RF_CFG_ILA_TRIG_MASK_SEL3_LEN)-1)<<RF_CFG_ILA_TRIG_MASK_SEL3_POS)
#define RF_CFG_ILA_TRIG_MASK_SEL3_UMSK                          (~(((1U<<RF_CFG_ILA_TRIG_MASK_SEL3_LEN)-1)<<RF_CFG_ILA_TRIG_MASK_SEL3_POS))

/* 0x2A0 : rf_ILA_ctrl4 */
#define RF_ILA_CTRL4_OFFSET                                     (0x2A0)
#define RF_CFG_ILA_TRIG_TARGET_SEL1                             RF_CFG_ILA_TRIG_TARGET_SEL1
#define RF_CFG_ILA_TRIG_TARGET_SEL1_POS                         (0U)
#define RF_CFG_ILA_TRIG_TARGET_SEL1_LEN                         (32U)
#define RF_CFG_ILA_TRIG_TARGET_SEL1_MSK                         (((1U<<RF_CFG_ILA_TRIG_TARGET_SEL1_LEN)-1)<<RF_CFG_ILA_TRIG_TARGET_SEL1_POS)
#define RF_CFG_ILA_TRIG_TARGET_SEL1_UMSK                        (~(((1U<<RF_CFG_ILA_TRIG_TARGET_SEL1_LEN)-1)<<RF_CFG_ILA_TRIG_TARGET_SEL1_POS))

/* 0x2A4 : rf_ILA_ctrl5 */
#define RF_ILA_CTRL5_OFFSET                                     (0x2A4)
#define RF_CFG_ILA_TRIG_TARGET_SEL2                             RF_CFG_ILA_TRIG_TARGET_SEL2
#define RF_CFG_ILA_TRIG_TARGET_SEL2_POS                         (0U)
#define RF_CFG_ILA_TRIG_TARGET_SEL2_LEN                         (32U)
#define RF_CFG_ILA_TRIG_TARGET_SEL2_MSK                         (((1U<<RF_CFG_ILA_TRIG_TARGET_SEL2_LEN)-1)<<RF_CFG_ILA_TRIG_TARGET_SEL2_POS)
#define RF_CFG_ILA_TRIG_TARGET_SEL2_UMSK                        (~(((1U<<RF_CFG_ILA_TRIG_TARGET_SEL2_LEN)-1)<<RF_CFG_ILA_TRIG_TARGET_SEL2_POS))

/* 0x2A8 : rf_ILA_ctrl6 */
#define RF_ILA_CTRL6_OFFSET                                     (0x2A8)
#define RF_CFG_ILA_TRIG_TARGET_SEL3                             RF_CFG_ILA_TRIG_TARGET_SEL3
#define RF_CFG_ILA_TRIG_TARGET_SEL3_POS                         (0U)
#define RF_CFG_ILA_TRIG_TARGET_SEL3_LEN                         (32U)
#define RF_CFG_ILA_TRIG_TARGET_SEL3_MSK                         (((1U<<RF_CFG_ILA_TRIG_TARGET_SEL3_LEN)-1)<<RF_CFG_ILA_TRIG_TARGET_SEL3_POS)
#define RF_CFG_ILA_TRIG_TARGET_SEL3_UMSK                        (~(((1U<<RF_CFG_ILA_TRIG_TARGET_SEL3_LEN)-1)<<RF_CFG_ILA_TRIG_TARGET_SEL3_POS))

/* 0x2AC : rf_ILA_ctrl7 */
#define RF_ILA_CTRL7_OFFSET                                     (0x2AC)
#define RF_CFG_ILA_END_CNT                                      RF_CFG_ILA_END_CNT
#define RF_CFG_ILA_END_CNT_POS                                  (0U)
#define RF_CFG_ILA_END_CNT_LEN                                  (32U)
#define RF_CFG_ILA_END_CNT_MSK                                  (((1U<<RF_CFG_ILA_END_CNT_LEN)-1)<<RF_CFG_ILA_END_CNT_POS)
#define RF_CFG_ILA_END_CNT_UMSK                                 (~(((1U<<RF_CFG_ILA_END_CNT_LEN)-1)<<RF_CFG_ILA_END_CNT_POS))

/* 0x2B0 : rf_ILA_ctrl8 */
#define RF_ILA_CTRL8_OFFSET                                     (0x2B0)
#define RF_SRAM_ADC_ADDR_KEEP                                   RF_SRAM_ADC_ADDR_KEEP
#define RF_SRAM_ADC_ADDR_KEEP_POS                               (0U)
#define RF_SRAM_ADC_ADDR_KEEP_LEN                               (16U)
#define RF_SRAM_ADC_ADDR_KEEP_MSK                               (((1U<<RF_SRAM_ADC_ADDR_KEEP_LEN)-1)<<RF_SRAM_ADC_ADDR_KEEP_POS)
#define RF_SRAM_ADC_ADDR_KEEP_UMSK                              (~(((1U<<RF_SRAM_ADC_ADDR_KEEP_LEN)-1)<<RF_SRAM_ADC_ADDR_KEEP_POS))

/* 0x2B4 : rf_ILA_ctrl9 */
#define RF_ILA_CTRL9_OFFSET                                     (0x2B4)
#define RF_DMA_ADC_ADDR_KEEP                                    RF_DMA_ADC_ADDR_KEEP
#define RF_DMA_ADC_ADDR_KEEP_POS                                (0U)
#define RF_DMA_ADC_ADDR_KEEP_LEN                                (32U)
#define RF_DMA_ADC_ADDR_KEEP_MSK                                (((1U<<RF_DMA_ADC_ADDR_KEEP_LEN)-1)<<RF_DMA_ADC_ADDR_KEEP_POS)
#define RF_DMA_ADC_ADDR_KEEP_UMSK                               (~(((1U<<RF_DMA_ADC_ADDR_KEEP_LEN)-1)<<RF_DMA_ADC_ADDR_KEEP_POS))

/* 0x2B8 : rf_ILA_ctrl10 */
#define RF_ILA_CTRL10_OFFSET                                    (0x2B8)
#define RF_CFG_ILA_DMA_CTRL_EN                                  RF_CFG_ILA_DMA_CTRL_EN
#define RF_CFG_ILA_DMA_CTRL_EN_POS                              (0U)
#define RF_CFG_ILA_DMA_CTRL_EN_LEN                              (1U)
#define RF_CFG_ILA_DMA_CTRL_EN_MSK                              (((1U<<RF_CFG_ILA_DMA_CTRL_EN_LEN)-1)<<RF_CFG_ILA_DMA_CTRL_EN_POS)
#define RF_CFG_ILA_DMA_CTRL_EN_UMSK                             (~(((1U<<RF_CFG_ILA_DMA_CTRL_EN_LEN)-1)<<RF_CFG_ILA_DMA_CTRL_EN_POS))
#define RF_CFG_ILA_RESERVED                                     RF_CFG_ILA_RESERVED
#define RF_CFG_ILA_RESERVED_POS                                 (1U)
#define RF_CFG_ILA_RESERVED_LEN                                 (31U)
#define RF_CFG_ILA_RESERVED_MSK                                 (((1U<<RF_CFG_ILA_RESERVED_LEN)-1)<<RF_CFG_ILA_RESERVED_POS)
#define RF_CFG_ILA_RESERVED_UMSK                                (~(((1U<<RF_CFG_ILA_RESERVED_LEN)-1)<<RF_CFG_ILA_RESERVED_POS))

/* 0x2BC : rf_timer_ctrl */
#define RF_TIMER_CTRL_OFFSET                                    (0x2BC)
#define RF_TIMER_EN                                             RF_TIMER_EN
#define RF_TIMER_EN_POS                                         (0U)
#define RF_TIMER_EN_LEN                                         (1U)
#define RF_TIMER_EN_MSK                                         (((1U<<RF_TIMER_EN_LEN)-1)<<RF_TIMER_EN_POS)
#define RF_TIMER_EN_UMSK                                        (~(((1U<<RF_TIMER_EN_LEN)-1)<<RF_TIMER_EN_POS))
#define RF_TIMER_READ                                           RF_TIMER_READ
#define RF_TIMER_READ_POS                                       (1U)
#define RF_TIMER_READ_LEN                                       (1U)
#define RF_TIMER_READ_MSK                                       (((1U<<RF_TIMER_READ_LEN)-1)<<RF_TIMER_READ_POS)
#define RF_TIMER_READ_UMSK                                      (~(((1U<<RF_TIMER_READ_LEN)-1)<<RF_TIMER_READ_POS))

/* 0x2C0 : rf_timer_value */
#define RF_TIMER_VALUE_OFFSET                                   (0x2C0)
#define RF_TIMER_KEEP                                           RF_TIMER_KEEP
#define RF_TIMER_KEEP_POS                                       (0U)
#define RF_TIMER_KEEP_LEN                                       (32U)
#define RF_TIMER_KEEP_MSK                                       (((1U<<RF_TIMER_KEEP_LEN)-1)<<RF_TIMER_KEEP_POS)
#define RF_TIMER_KEEP_UMSK                                      (~(((1U<<RF_TIMER_KEEP_LEN)-1)<<RF_TIMER_KEEP_POS))

/* 0x2C4 : fdiq_ctrl_0 */
#define RF_FDIQ_CTRL_0_OFFSET                                   (0x2C4)
#define RF_FDIQ_RSTB                                            RF_FDIQ_RSTB
#define RF_FDIQ_RSTB_POS                                        (0U)
#define RF_FDIQ_RSTB_LEN                                        (1U)
#define RF_FDIQ_RSTB_MSK                                        (((1U<<RF_FDIQ_RSTB_LEN)-1)<<RF_FDIQ_RSTB_POS)
#define RF_FDIQ_RSTB_UMSK                                       (~(((1U<<RF_FDIQ_RSTB_LEN)-1)<<RF_FDIQ_RSTB_POS))
#define RF_FDIQ_BYPASS                                          RF_FDIQ_BYPASS
#define RF_FDIQ_BYPASS_POS                                      (1U)
#define RF_FDIQ_BYPASS_LEN                                      (1U)
#define RF_FDIQ_BYPASS_MSK                                      (((1U<<RF_FDIQ_BYPASS_LEN)-1)<<RF_FDIQ_BYPASS_POS)
#define RF_FDIQ_BYPASS_UMSK                                     (~(((1U<<RF_FDIQ_BYPASS_LEN)-1)<<RF_FDIQ_BYPASS_POS))
#define RF_FDIQ_ACCEN                                           RF_FDIQ_ACCEN
#define RF_FDIQ_ACCEN_POS                                       (2U)
#define RF_FDIQ_ACCEN_LEN                                       (1U)
#define RF_FDIQ_ACCEN_MSK                                       (((1U<<RF_FDIQ_ACCEN_LEN)-1)<<RF_FDIQ_ACCEN_POS)
#define RF_FDIQ_ACCEN_UMSK                                      (~(((1U<<RF_FDIQ_ACCEN_LEN)-1)<<RF_FDIQ_ACCEN_POS))
#define RF_FDIQ_DLY                                             RF_FDIQ_DLY
#define RF_FDIQ_DLY_POS                                         (4U)
#define RF_FDIQ_DLY_LEN                                         (4U)
#define RF_FDIQ_DLY_MSK                                         (((1U<<RF_FDIQ_DLY_LEN)-1)<<RF_FDIQ_DLY_POS)
#define RF_FDIQ_DLY_UMSK                                        (~(((1U<<RF_FDIQ_DLY_LEN)-1)<<RF_FDIQ_DLY_POS))
#define RF_FDIQ_ACCUOFFSET                                      RF_FDIQ_ACCUOFFSET
#define RF_FDIQ_ACCUOFFSET_POS                                  (8U)
#define RF_FDIQ_ACCUOFFSET_LEN                                  (10U)
#define RF_FDIQ_ACCUOFFSET_MSK                                  (((1U<<RF_FDIQ_ACCUOFFSET_LEN)-1)<<RF_FDIQ_ACCUOFFSET_POS)
#define RF_FDIQ_ACCUOFFSET_UMSK                                 (~(((1U<<RF_FDIQ_ACCUOFFSET_LEN)-1)<<RF_FDIQ_ACCUOFFSET_POS))
#define RF_FDIQ_ACCULENGTH                                      RF_FDIQ_ACCULENGTH
#define RF_FDIQ_ACCULENGTH_POS                                  (20U)
#define RF_FDIQ_ACCULENGTH_LEN                                  (10U)
#define RF_FDIQ_ACCULENGTH_MSK                                  (((1U<<RF_FDIQ_ACCULENGTH_LEN)-1)<<RF_FDIQ_ACCULENGTH_POS)
#define RF_FDIQ_ACCULENGTH_UMSK                                 (~(((1U<<RF_FDIQ_ACCULENGTH_LEN)-1)<<RF_FDIQ_ACCULENGTH_POS))

/* 0x2C8 : fdiq_ctrl_1 */
#define RF_FDIQ_CTRL_1_OFFSET                                   (0x2C8)
#define RF_FDIQ_CORR                                            RF_FDIQ_CORR
#define RF_FDIQ_CORR_POS                                        (0U)
#define RF_FDIQ_CORR_LEN                                        (30U)
#define RF_FDIQ_CORR_MSK                                        (((1U<<RF_FDIQ_CORR_LEN)-1)<<RF_FDIQ_CORR_POS)
#define RF_FDIQ_CORR_UMSK                                       (~(((1U<<RF_FDIQ_CORR_LEN)-1)<<RF_FDIQ_CORR_POS))
#define RF_FDIQ_DONE                                            RF_FDIQ_DONE
#define RF_FDIQ_DONE_POS                                        (31U)
#define RF_FDIQ_DONE_LEN                                        (1U)
#define RF_FDIQ_DONE_MSK                                        (((1U<<RF_FDIQ_DONE_LEN)-1)<<RF_FDIQ_DONE_POS)
#define RF_FDIQ_DONE_UMSK                                       (~(((1U<<RF_FDIQ_DONE_LEN)-1)<<RF_FDIQ_DONE_POS))

/* 0x600 : dfe_ctrl_0 */
#define RF_DFE_CTRL_0_OFFSET                                    (0x600)
#define RF_TX_IQC_PHASE                                         RF_TX_IQC_PHASE
#define RF_TX_IQC_PHASE_POS                                     (0U)
#define RF_TX_IQC_PHASE_LEN                                     (10U)
#define RF_TX_IQC_PHASE_MSK                                     (((1U<<RF_TX_IQC_PHASE_LEN)-1)<<RF_TX_IQC_PHASE_POS)
#define RF_TX_IQC_PHASE_UMSK                                    (~(((1U<<RF_TX_IQC_PHASE_LEN)-1)<<RF_TX_IQC_PHASE_POS))
#define RF_TX_IQC_PHASE_EN                                      RF_TX_IQC_PHASE_EN
#define RF_TX_IQC_PHASE_EN_POS                                  (10U)
#define RF_TX_IQC_PHASE_EN_LEN                                  (1U)
#define RF_TX_IQC_PHASE_EN_MSK                                  (((1U<<RF_TX_IQC_PHASE_EN_LEN)-1)<<RF_TX_IQC_PHASE_EN_POS)
#define RF_TX_IQC_PHASE_EN_UMSK                                 (~(((1U<<RF_TX_IQC_PHASE_EN_LEN)-1)<<RF_TX_IQC_PHASE_EN_POS))
#define RF_TX_IQC_GAIN                                          RF_TX_IQC_GAIN
#define RF_TX_IQC_GAIN_POS                                      (12U)
#define RF_TX_IQC_GAIN_LEN                                      (11U)
#define RF_TX_IQC_GAIN_MSK                                      (((1U<<RF_TX_IQC_GAIN_LEN)-1)<<RF_TX_IQC_GAIN_POS)
#define RF_TX_IQC_GAIN_UMSK                                     (~(((1U<<RF_TX_IQC_GAIN_LEN)-1)<<RF_TX_IQC_GAIN_POS))
#define RF_TX_IQC_GAIN_EN                                       RF_TX_IQC_GAIN_EN
#define RF_TX_IQC_GAIN_EN_POS                                   (23U)
#define RF_TX_IQC_GAIN_EN_LEN                                   (1U)
#define RF_TX_IQC_GAIN_EN_MSK                                   (((1U<<RF_TX_IQC_GAIN_EN_LEN)-1)<<RF_TX_IQC_GAIN_EN_POS)
#define RF_TX_IQC_GAIN_EN_UMSK                                  (~(((1U<<RF_TX_IQC_GAIN_EN_LEN)-1)<<RF_TX_IQC_GAIN_EN_POS))
#define RF_TX2RX_LOOP_EN                                        RF_TX2RX_LOOP_EN
#define RF_TX2RX_LOOP_EN_POS                                    (24U)
#define RF_TX2RX_LOOP_EN_LEN                                    (1U)
#define RF_TX2RX_LOOP_EN_MSK                                    (((1U<<RF_TX2RX_LOOP_EN_LEN)-1)<<RF_TX2RX_LOOP_EN_POS)
#define RF_TX2RX_LOOP_EN_UMSK                                   (~(((1U<<RF_TX2RX_LOOP_EN_LEN)-1)<<RF_TX2RX_LOOP_EN_POS))
#define RF_TX2RX_LOOP_INPUT_SEL                                 RF_TX2RX_LOOP_INPUT_SEL
#define RF_TX2RX_LOOP_INPUT_SEL_POS                             (25U)
#define RF_TX2RX_LOOP_INPUT_SEL_LEN                             (1U)
#define RF_TX2RX_LOOP_INPUT_SEL_MSK                             (((1U<<RF_TX2RX_LOOP_INPUT_SEL_LEN)-1)<<RF_TX2RX_LOOP_INPUT_SEL_POS)
#define RF_TX2RX_LOOP_INPUT_SEL_UMSK                            (~(((1U<<RF_TX2RX_LOOP_INPUT_SEL_LEN)-1)<<RF_TX2RX_LOOP_INPUT_SEL_POS))

/* 0x604 : dfe_ctrl_1 */
#define RF_DFE_CTRL_1_OFFSET                                    (0x604)
#define RF_TX_DAC_OS_I                                          RF_TX_DAC_OS_I
#define RF_TX_DAC_OS_I_POS                                      (0U)
#define RF_TX_DAC_OS_I_LEN                                      (12U)
#define RF_TX_DAC_OS_I_MSK                                      (((1U<<RF_TX_DAC_OS_I_LEN)-1)<<RF_TX_DAC_OS_I_POS)
#define RF_TX_DAC_OS_I_UMSK                                     (~(((1U<<RF_TX_DAC_OS_I_LEN)-1)<<RF_TX_DAC_OS_I_POS))
#define RF_TX_DLY_TUNE_N                                        RF_TX_DLY_TUNE_N
#define RF_TX_DLY_TUNE_N_POS                                    (12U)
#define RF_TX_DLY_TUNE_N_LEN                                    (3U)
#define RF_TX_DLY_TUNE_N_MSK                                    (((1U<<RF_TX_DLY_TUNE_N_LEN)-1)<<RF_TX_DLY_TUNE_N_POS)
#define RF_TX_DLY_TUNE_N_UMSK                                   (~(((1U<<RF_TX_DLY_TUNE_N_LEN)-1)<<RF_TX_DLY_TUNE_N_POS))
#define RF_TX_RC3_CSEL                                          RF_TX_RC3_CSEL
#define RF_TX_RC3_CSEL_POS                                      (15U)
#define RF_TX_RC3_CSEL_LEN                                      (1U)
#define RF_TX_RC3_CSEL_MSK                                      (((1U<<RF_TX_RC3_CSEL_LEN)-1)<<RF_TX_RC3_CSEL_POS)
#define RF_TX_RC3_CSEL_UMSK                                     (~(((1U<<RF_TX_RC3_CSEL_LEN)-1)<<RF_TX_RC3_CSEL_POS))
#define RF_TX_DAC_OS_Q                                          RF_TX_DAC_OS_Q
#define RF_TX_DAC_OS_Q_POS                                      (16U)
#define RF_TX_DAC_OS_Q_LEN                                      (12U)
#define RF_TX_DAC_OS_Q_MSK                                      (((1U<<RF_TX_DAC_OS_Q_LEN)-1)<<RF_TX_DAC_OS_Q_POS)
#define RF_TX_DAC_OS_Q_UMSK                                     (~(((1U<<RF_TX_DAC_OS_Q_LEN)-1)<<RF_TX_DAC_OS_Q_POS))
#define RF_TX_DAC_DIT_SEL                                       RF_TX_DAC_DIT_SEL
#define RF_TX_DAC_DIT_SEL_POS                                   (28U)
#define RF_TX_DAC_DIT_SEL_LEN                                   (2U)
#define RF_TX_DAC_DIT_SEL_MSK                                   (((1U<<RF_TX_DAC_DIT_SEL_LEN)-1)<<RF_TX_DAC_DIT_SEL_POS)
#define RF_TX_DAC_DIT_SEL_UMSK                                  (~(((1U<<RF_TX_DAC_DIT_SEL_LEN)-1)<<RF_TX_DAC_DIT_SEL_POS))
#define RF_TX_DAC_DAT_FORMAT                                    RF_TX_DAC_DAT_FORMAT
#define RF_TX_DAC_DAT_FORMAT_POS                                (30U)
#define RF_TX_DAC_DAT_FORMAT_LEN                                (1U)
#define RF_TX_DAC_DAT_FORMAT_MSK                                (((1U<<RF_TX_DAC_DAT_FORMAT_LEN)-1)<<RF_TX_DAC_DAT_FORMAT_POS)
#define RF_TX_DAC_DAT_FORMAT_UMSK                               (~(((1U<<RF_TX_DAC_DAT_FORMAT_LEN)-1)<<RF_TX_DAC_DAT_FORMAT_POS))
#define RF_TX_DAC_IQ_SWAP                                       RF_TX_DAC_IQ_SWAP
#define RF_TX_DAC_IQ_SWAP_POS                                   (31U)
#define RF_TX_DAC_IQ_SWAP_LEN                                   (1U)
#define RF_TX_DAC_IQ_SWAP_MSK                                   (((1U<<RF_TX_DAC_IQ_SWAP_LEN)-1)<<RF_TX_DAC_IQ_SWAP_POS)
#define RF_TX_DAC_IQ_SWAP_UMSK                                  (~(((1U<<RF_TX_DAC_IQ_SWAP_LEN)-1)<<RF_TX_DAC_IQ_SWAP_POS))

/* 0x608 : dfe_ctrl_2 */
#define RF_DFE_CTRL_2_OFFSET                                    (0x608)
#define RF_RX_ADC_OS_I                                          RF_RX_ADC_OS_I
#define RF_RX_ADC_OS_I_POS                                      (0U)
#define RF_RX_ADC_OS_I_LEN                                      (11U)
#define RF_RX_ADC_OS_I_MSK                                      (((1U<<RF_RX_ADC_OS_I_LEN)-1)<<RF_RX_ADC_OS_I_POS)
#define RF_RX_ADC_OS_I_UMSK                                     (~(((1U<<RF_RX_ADC_OS_I_LEN)-1)<<RF_RX_ADC_OS_I_POS))
#define RF_RX_ADC_OS_Q                                          RF_RX_ADC_OS_Q
#define RF_RX_ADC_OS_Q_POS                                      (16U)
#define RF_RX_ADC_OS_Q_LEN                                      (11U)
#define RF_RX_ADC_OS_Q_MSK                                      (((1U<<RF_RX_ADC_OS_Q_LEN)-1)<<RF_RX_ADC_OS_Q_POS)
#define RF_RX_ADC_OS_Q_UMSK                                     (~(((1U<<RF_RX_ADC_OS_Q_LEN)-1)<<RF_RX_ADC_OS_Q_POS))
#define RF_RX_ADC_DCE_FLT_EN                                    RF_RX_ADC_DCE_FLT_EN
#define RF_RX_ADC_DCE_FLT_EN_POS                                (28U)
#define RF_RX_ADC_DCE_FLT_EN_LEN                                (1U)
#define RF_RX_ADC_DCE_FLT_EN_MSK                                (((1U<<RF_RX_ADC_DCE_FLT_EN_LEN)-1)<<RF_RX_ADC_DCE_FLT_EN_POS)
#define RF_RX_ADC_DCE_FLT_EN_UMSK                               (~(((1U<<RF_RX_ADC_DCE_FLT_EN_LEN)-1)<<RF_RX_ADC_DCE_FLT_EN_POS))
#define RF_RX_ADC_LOW_POW_EN                                    RF_RX_ADC_LOW_POW_EN
#define RF_RX_ADC_LOW_POW_EN_POS                                (29U)
#define RF_RX_ADC_LOW_POW_EN_LEN                                (1U)
#define RF_RX_ADC_LOW_POW_EN_MSK                                (((1U<<RF_RX_ADC_LOW_POW_EN_LEN)-1)<<RF_RX_ADC_LOW_POW_EN_POS)
#define RF_RX_ADC_LOW_POW_EN_UMSK                               (~(((1U<<RF_RX_ADC_LOW_POW_EN_LEN)-1)<<RF_RX_ADC_LOW_POW_EN_POS))
#define RF_RX_ADC_DAT_FORMAT                                    RF_RX_ADC_DAT_FORMAT
#define RF_RX_ADC_DAT_FORMAT_POS                                (30U)
#define RF_RX_ADC_DAT_FORMAT_LEN                                (1U)
#define RF_RX_ADC_DAT_FORMAT_MSK                                (((1U<<RF_RX_ADC_DAT_FORMAT_LEN)-1)<<RF_RX_ADC_DAT_FORMAT_POS)
#define RF_RX_ADC_DAT_FORMAT_UMSK                               (~(((1U<<RF_RX_ADC_DAT_FORMAT_LEN)-1)<<RF_RX_ADC_DAT_FORMAT_POS))
#define RF_RX_ADC_IQ_SWAP                                       RF_RX_ADC_IQ_SWAP
#define RF_RX_ADC_IQ_SWAP_POS                                   (31U)
#define RF_RX_ADC_IQ_SWAP_LEN                                   (1U)
#define RF_RX_ADC_IQ_SWAP_MSK                                   (((1U<<RF_RX_ADC_IQ_SWAP_LEN)-1)<<RF_RX_ADC_IQ_SWAP_POS)
#define RF_RX_ADC_IQ_SWAP_UMSK                                  (~(((1U<<RF_RX_ADC_IQ_SWAP_LEN)-1)<<RF_RX_ADC_IQ_SWAP_POS))

/* 0x60C : dfe_ctrl_3 */
#define RF_DFE_CTRL_3_OFFSET                                    (0x60C)
#define RF_RX_ADC_4S_I_VAL                                      RF_RX_ADC_4S_I_VAL
#define RF_RX_ADC_4S_I_VAL_POS                                  (0U)
#define RF_RX_ADC_4S_I_VAL_LEN                                  (11U)
#define RF_RX_ADC_4S_I_VAL_MSK                                  (((1U<<RF_RX_ADC_4S_I_VAL_LEN)-1)<<RF_RX_ADC_4S_I_VAL_POS)
#define RF_RX_ADC_4S_I_VAL_UMSK                                 (~(((1U<<RF_RX_ADC_4S_I_VAL_LEN)-1)<<RF_RX_ADC_4S_I_VAL_POS))
#define RF_RX_ADC_4S_I_EN                                       RF_RX_ADC_4S_I_EN
#define RF_RX_ADC_4S_I_EN_POS                                   (11U)
#define RF_RX_ADC_4S_I_EN_LEN                                   (1U)
#define RF_RX_ADC_4S_I_EN_MSK                                   (((1U<<RF_RX_ADC_4S_I_EN_LEN)-1)<<RF_RX_ADC_4S_I_EN_POS)
#define RF_RX_ADC_4S_I_EN_UMSK                                  (~(((1U<<RF_RX_ADC_4S_I_EN_LEN)-1)<<RF_RX_ADC_4S_I_EN_POS))
#define RF_RX_ADC_4S_Q_VAL                                      RF_RX_ADC_4S_Q_VAL
#define RF_RX_ADC_4S_Q_VAL_POS                                  (16U)
#define RF_RX_ADC_4S_Q_VAL_LEN                                  (11U)
#define RF_RX_ADC_4S_Q_VAL_MSK                                  (((1U<<RF_RX_ADC_4S_Q_VAL_LEN)-1)<<RF_RX_ADC_4S_Q_VAL_POS)
#define RF_RX_ADC_4S_Q_VAL_UMSK                                 (~(((1U<<RF_RX_ADC_4S_Q_VAL_LEN)-1)<<RF_RX_ADC_4S_Q_VAL_POS))
#define RF_RX_ADC_4S_Q_EN                                       RF_RX_ADC_4S_Q_EN
#define RF_RX_ADC_4S_Q_EN_POS                                   (27U)
#define RF_RX_ADC_4S_Q_EN_LEN                                   (1U)
#define RF_RX_ADC_4S_Q_EN_MSK                                   (((1U<<RF_RX_ADC_4S_Q_EN_LEN)-1)<<RF_RX_ADC_4S_Q_EN_POS)
#define RF_RX_ADC_4S_Q_EN_UMSK                                  (~(((1U<<RF_RX_ADC_4S_Q_EN_LEN)-1)<<RF_RX_ADC_4S_Q_EN_POS))

/* 0x610 : dfe_ctrl_4 */
#define RF_DFE_CTRL_4_OFFSET                                    (0x610)
#define RF_RX_PF_TH2                                            RF_RX_PF_TH2
#define RF_RX_PF_TH2_POS                                        (0U)
#define RF_RX_PF_TH2_LEN                                        (11U)
#define RF_RX_PF_TH2_MSK                                        (((1U<<RF_RX_PF_TH2_LEN)-1)<<RF_RX_PF_TH2_POS)
#define RF_RX_PF_TH2_UMSK                                       (~(((1U<<RF_RX_PF_TH2_LEN)-1)<<RF_RX_PF_TH2_POS))
#define RF_RX_PF_TH1                                            RF_RX_PF_TH1
#define RF_RX_PF_TH1_POS                                        (16U)
#define RF_RX_PF_TH1_LEN                                        (11U)
#define RF_RX_PF_TH1_MSK                                        (((1U<<RF_RX_PF_TH1_LEN)-1)<<RF_RX_PF_TH1_POS)
#define RF_RX_PF_TH1_UMSK                                       (~(((1U<<RF_RX_PF_TH1_LEN)-1)<<RF_RX_PF_TH1_POS))
#define RF_RX_PF_Q_EN                                           RF_RX_PF_Q_EN
#define RF_RX_PF_Q_EN_POS                                       (30U)
#define RF_RX_PF_Q_EN_LEN                                       (1U)
#define RF_RX_PF_Q_EN_MSK                                       (((1U<<RF_RX_PF_Q_EN_LEN)-1)<<RF_RX_PF_Q_EN_POS)
#define RF_RX_PF_Q_EN_UMSK                                      (~(((1U<<RF_RX_PF_Q_EN_LEN)-1)<<RF_RX_PF_Q_EN_POS))
#define RF_RX_PF_I_EN                                           RF_RX_PF_I_EN
#define RF_RX_PF_I_EN_POS                                       (31U)
#define RF_RX_PF_I_EN_LEN                                       (1U)
#define RF_RX_PF_I_EN_MSK                                       (((1U<<RF_RX_PF_I_EN_LEN)-1)<<RF_RX_PF_I_EN_POS)
#define RF_RX_PF_I_EN_UMSK                                      (~(((1U<<RF_RX_PF_I_EN_LEN)-1)<<RF_RX_PF_I_EN_POS))

/* 0x614 : dfe_ctrl_5 */
#define RF_DFE_CTRL_5_OFFSET                                    (0x614)
#define RF_RX_IQC_PHASE                                         RF_RX_IQC_PHASE
#define RF_RX_IQC_PHASE_POS                                     (0U)
#define RF_RX_IQC_PHASE_LEN                                     (10U)
#define RF_RX_IQC_PHASE_MSK                                     (((1U<<RF_RX_IQC_PHASE_LEN)-1)<<RF_RX_IQC_PHASE_POS)
#define RF_RX_IQC_PHASE_UMSK                                    (~(((1U<<RF_RX_IQC_PHASE_LEN)-1)<<RF_RX_IQC_PHASE_POS))
#define RF_RX_IQC_PHASE_EN                                      RF_RX_IQC_PHASE_EN
#define RF_RX_IQC_PHASE_EN_POS                                  (10U)
#define RF_RX_IQC_PHASE_EN_LEN                                  (1U)
#define RF_RX_IQC_PHASE_EN_MSK                                  (((1U<<RF_RX_IQC_PHASE_EN_LEN)-1)<<RF_RX_IQC_PHASE_EN_POS)
#define RF_RX_IQC_PHASE_EN_UMSK                                 (~(((1U<<RF_RX_IQC_PHASE_EN_LEN)-1)<<RF_RX_IQC_PHASE_EN_POS))
#define RF_RX_IQC_GAIN                                          RF_RX_IQC_GAIN
#define RF_RX_IQC_GAIN_POS                                      (12U)
#define RF_RX_IQC_GAIN_LEN                                      (11U)
#define RF_RX_IQC_GAIN_MSK                                      (((1U<<RF_RX_IQC_GAIN_LEN)-1)<<RF_RX_IQC_GAIN_POS)
#define RF_RX_IQC_GAIN_UMSK                                     (~(((1U<<RF_RX_IQC_GAIN_LEN)-1)<<RF_RX_IQC_GAIN_POS))
#define RF_RX_IQC_GAIN_EN                                       RF_RX_IQC_GAIN_EN
#define RF_RX_IQC_GAIN_EN_POS                                   (23U)
#define RF_RX_IQC_GAIN_EN_LEN                                   (1U)
#define RF_RX_IQC_GAIN_EN_MSK                                   (((1U<<RF_RX_IQC_GAIN_EN_LEN)-1)<<RF_RX_IQC_GAIN_EN_POS)
#define RF_RX_IQC_GAIN_EN_UMSK                                  (~(((1U<<RF_RX_IQC_GAIN_EN_LEN)-1)<<RF_RX_IQC_GAIN_EN_POS))

/* 0x618 : dfe_ctrl_6 */
#define RF_DFE_CTRL_6_OFFSET                                    (0x618)
#define RF_RX_PM_FREQSHIFT_CW                                   RF_RX_PM_FREQSHIFT_CW
#define RF_RX_PM_FREQSHIFT_CW_POS                               (0U)
#define RF_RX_PM_FREQSHIFT_CW_LEN                               (20U)
#define RF_RX_PM_FREQSHIFT_CW_MSK                               (((1U<<RF_RX_PM_FREQSHIFT_CW_LEN)-1)<<RF_RX_PM_FREQSHIFT_CW_POS)
#define RF_RX_PM_FREQSHIFT_CW_UMSK                              (~(((1U<<RF_RX_PM_FREQSHIFT_CW_LEN)-1)<<RF_RX_PM_FREQSHIFT_CW_POS))
#define RF_RX_PM_FREQSHIFT_EN                                   RF_RX_PM_FREQSHIFT_EN
#define RF_RX_PM_FREQSHIFT_EN_POS                               (20U)
#define RF_RX_PM_FREQSHIFT_EN_LEN                               (1U)
#define RF_RX_PM_FREQSHIFT_EN_MSK                               (((1U<<RF_RX_PM_FREQSHIFT_EN_LEN)-1)<<RF_RX_PM_FREQSHIFT_EN_POS)
#define RF_RX_PM_FREQSHIFT_EN_UMSK                              (~(((1U<<RF_RX_PM_FREQSHIFT_EN_LEN)-1)<<RF_RX_PM_FREQSHIFT_EN_POS))
#define RF_RX_PM_DONE                                           RF_RX_PM_DONE
#define RF_RX_PM_DONE_POS                                       (28U)
#define RF_RX_PM_DONE_LEN                                       (1U)
#define RF_RX_PM_DONE_MSK                                       (((1U<<RF_RX_PM_DONE_LEN)-1)<<RF_RX_PM_DONE_POS)
#define RF_RX_PM_DONE_UMSK                                      (~(((1U<<RF_RX_PM_DONE_LEN)-1)<<RF_RX_PM_DONE_POS))
#define RF_RX_PM_EN                                             RF_RX_PM_EN
#define RF_RX_PM_EN_POS                                         (29U)
#define RF_RX_PM_EN_LEN                                         (1U)
#define RF_RX_PM_EN_MSK                                         (((1U<<RF_RX_PM_EN_LEN)-1)<<RF_RX_PM_EN_POS)
#define RF_RX_PM_EN_UMSK                                        (~(((1U<<RF_RX_PM_EN_LEN)-1)<<RF_RX_PM_EN_POS))
#define RF_RX_PM_IN_SEL                                         RF_RX_PM_IN_SEL
#define RF_RX_PM_IN_SEL_POS                                     (30U)
#define RF_RX_PM_IN_SEL_LEN                                     (2U)
#define RF_RX_PM_IN_SEL_MSK                                     (((1U<<RF_RX_PM_IN_SEL_LEN)-1)<<RF_RX_PM_IN_SEL_POS)
#define RF_RX_PM_IN_SEL_UMSK                                    (~(((1U<<RF_RX_PM_IN_SEL_LEN)-1)<<RF_RX_PM_IN_SEL_POS))

/* 0x61C : dfe_ctrl_7 */
#define RF_DFE_CTRL_7_OFFSET                                    (0x61C)
#define RF_RX_PM_START_OFS                                      RF_RX_PM_START_OFS
#define RF_RX_PM_START_OFS_POS                                  (0U)
#define RF_RX_PM_START_OFS_LEN                                  (16U)
#define RF_RX_PM_START_OFS_MSK                                  (((1U<<RF_RX_PM_START_OFS_LEN)-1)<<RF_RX_PM_START_OFS_POS)
#define RF_RX_PM_START_OFS_UMSK                                 (~(((1U<<RF_RX_PM_START_OFS_LEN)-1)<<RF_RX_PM_START_OFS_POS))
#define RF_RX_PM_ACC_LEN                                        RF_RX_PM_ACC_LEN
#define RF_RX_PM_ACC_LEN_POS                                    (16U)
#define RF_RX_PM_ACC_LEN_LEN                                    (16U)
#define RF_RX_PM_ACC_LEN_MSK                                    (((1U<<RF_RX_PM_ACC_LEN_LEN)-1)<<RF_RX_PM_ACC_LEN_POS)
#define RF_RX_PM_ACC_LEN_UMSK                                   (~(((1U<<RF_RX_PM_ACC_LEN_LEN)-1)<<RF_RX_PM_ACC_LEN_POS))

/* 0x620 : dfe_ctrl_8 */
#define RF_DFE_CTRL_8_OFFSET                                    (0x620)
#define RF_RX_PM_IQACC_I                                        RF_RX_PM_IQACC_I
#define RF_RX_PM_IQACC_I_POS                                    (0U)
#define RF_RX_PM_IQACC_I_LEN                                    (25U)
#define RF_RX_PM_IQACC_I_MSK                                    (((1U<<RF_RX_PM_IQACC_I_LEN)-1)<<RF_RX_PM_IQACC_I_POS)
#define RF_RX_PM_IQACC_I_UMSK                                   (~(((1U<<RF_RX_PM_IQACC_I_LEN)-1)<<RF_RX_PM_IQACC_I_POS))

/* 0x624 : dfe_ctrl_9 */
#define RF_DFE_CTRL_9_OFFSET                                    (0x624)
#define RF_RX_PM_IQACC_Q                                        RF_RX_PM_IQACC_Q
#define RF_RX_PM_IQACC_Q_POS                                    (0U)
#define RF_RX_PM_IQACC_Q_LEN                                    (25U)
#define RF_RX_PM_IQACC_Q_MSK                                    (((1U<<RF_RX_PM_IQACC_Q_LEN)-1)<<RF_RX_PM_IQACC_Q_POS)
#define RF_RX_PM_IQACC_Q_UMSK                                   (~(((1U<<RF_RX_PM_IQACC_Q_LEN)-1)<<RF_RX_PM_IQACC_Q_POS))

/* 0x628 : dfe_ctrl_10 */
#define RF_DFE_CTRL_10_OFFSET                                   (0x628)
#define RF_DFE_DAC_RAW_I                                        RF_DFE_DAC_RAW_I
#define RF_DFE_DAC_RAW_I_POS                                    (0U)
#define RF_DFE_DAC_RAW_I_LEN                                    (11U)
#define RF_DFE_DAC_RAW_I_MSK                                    (((1U<<RF_DFE_DAC_RAW_I_LEN)-1)<<RF_DFE_DAC_RAW_I_POS)
#define RF_DFE_DAC_RAW_I_UMSK                                   (~(((1U<<RF_DFE_DAC_RAW_I_LEN)-1)<<RF_DFE_DAC_RAW_I_POS))
#define RF_DFE_DAC_RAW_Q                                        RF_DFE_DAC_RAW_Q
#define RF_DFE_DAC_RAW_Q_POS                                    (16U)
#define RF_DFE_DAC_RAW_Q_LEN                                    (11U)
#define RF_DFE_DAC_RAW_Q_MSK                                    (((1U<<RF_DFE_DAC_RAW_Q_LEN)-1)<<RF_DFE_DAC_RAW_Q_POS)
#define RF_DFE_DAC_RAW_Q_UMSK                                   (~(((1U<<RF_DFE_DAC_RAW_Q_LEN)-1)<<RF_DFE_DAC_RAW_Q_POS))

/* 0x62C : dfe_ctrl_11 */
#define RF_DFE_CTRL_11_OFFSET                                   (0x62C)
#define RF_DFE_ADC_RAW_I                                        RF_DFE_ADC_RAW_I
#define RF_DFE_ADC_RAW_I_POS                                    (0U)
#define RF_DFE_ADC_RAW_I_LEN                                    (11U)
#define RF_DFE_ADC_RAW_I_MSK                                    (((1U<<RF_DFE_ADC_RAW_I_LEN)-1)<<RF_DFE_ADC_RAW_I_POS)
#define RF_DFE_ADC_RAW_I_UMSK                                   (~(((1U<<RF_DFE_ADC_RAW_I_LEN)-1)<<RF_DFE_ADC_RAW_I_POS))
#define RF_DFE_ADC_RAW_Q                                        RF_DFE_ADC_RAW_Q
#define RF_DFE_ADC_RAW_Q_POS                                    (16U)
#define RF_DFE_ADC_RAW_Q_LEN                                    (11U)
#define RF_DFE_ADC_RAW_Q_MSK                                    (((1U<<RF_DFE_ADC_RAW_Q_LEN)-1)<<RF_DFE_ADC_RAW_Q_POS)
#define RF_DFE_ADC_RAW_Q_UMSK                                   (~(((1U<<RF_DFE_ADC_RAW_Q_LEN)-1)<<RF_DFE_ADC_RAW_Q_POS))

/* 0x630 : dfe_ctrl_12 */
#define RF_DFE_CTRL_12_OFFSET                                   (0x630)
#define RF_TX_CFR_TH                                            RF_TX_CFR_TH
#define RF_TX_CFR_TH_POS                                        (0U)
#define RF_TX_CFR_TH_LEN                                        (8U)
#define RF_TX_CFR_TH_MSK                                        (((1U<<RF_TX_CFR_TH_LEN)-1)<<RF_TX_CFR_TH_POS)
#define RF_TX_CFR_TH_UMSK                                       (~(((1U<<RF_TX_CFR_TH_LEN)-1)<<RF_TX_CFR_TH_POS))
#define RF_TX_CFR_MODE                                          RF_TX_CFR_MODE
#define RF_TX_CFR_MODE_POS                                      (8U)
#define RF_TX_CFR_MODE_LEN                                      (1U)
#define RF_TX_CFR_MODE_MSK                                      (((1U<<RF_TX_CFR_MODE_LEN)-1)<<RF_TX_CFR_MODE_POS)
#define RF_TX_CFR_MODE_UMSK                                     (~(((1U<<RF_TX_CFR_MODE_LEN)-1)<<RF_TX_CFR_MODE_POS))
#define RF_TX_CFR_EN                                            RF_TX_CFR_EN
#define RF_TX_CFR_EN_POS                                        (9U)
#define RF_TX_CFR_EN_LEN                                        (1U)
#define RF_TX_CFR_EN_MSK                                        (((1U<<RF_TX_CFR_EN_LEN)-1)<<RF_TX_CFR_EN_POS)
#define RF_TX_CFR_EN_UMSK                                       (~(((1U<<RF_TX_CFR_EN_LEN)-1)<<RF_TX_CFR_EN_POS))
#define RF_TX_CFR_CTRL_HW                                       RF_TX_CFR_CTRL_HW
#define RF_TX_CFR_CTRL_HW_POS                                   (10U)
#define RF_TX_CFR_CTRL_HW_LEN                                   (1U)
#define RF_TX_CFR_CTRL_HW_MSK                                   (((1U<<RF_TX_CFR_CTRL_HW_LEN)-1)<<RF_TX_CFR_CTRL_HW_POS)
#define RF_TX_CFR_CTRL_HW_UMSK                                  (~(((1U<<RF_TX_CFR_CTRL_HW_LEN)-1)<<RF_TX_CFR_CTRL_HW_POS))
#define RF_TX_CFR_EN_OFDM                                       RF_TX_CFR_EN_OFDM
#define RF_TX_CFR_EN_OFDM_POS                                   (24U)
#define RF_TX_CFR_EN_OFDM_LEN                                   (8U)
#define RF_TX_CFR_EN_OFDM_MSK                                   (((1U<<RF_TX_CFR_EN_OFDM_LEN)-1)<<RF_TX_CFR_EN_OFDM_POS)
#define RF_TX_CFR_EN_OFDM_UMSK                                  (~(((1U<<RF_TX_CFR_EN_OFDM_LEN)-1)<<RF_TX_CFR_EN_OFDM_POS))

/* 0x634 : dfe_ctrl_13 */
#define RF_DFE_CTRL_13_OFFSET                                   (0x634)
#define RF_TX_CFR_TH_MCS3                                       RF_TX_CFR_TH_MCS3
#define RF_TX_CFR_TH_MCS3_POS                                   (0U)
#define RF_TX_CFR_TH_MCS3_LEN                                   (8U)
#define RF_TX_CFR_TH_MCS3_MSK                                   (((1U<<RF_TX_CFR_TH_MCS3_LEN)-1)<<RF_TX_CFR_TH_MCS3_POS)
#define RF_TX_CFR_TH_MCS3_UMSK                                  (~(((1U<<RF_TX_CFR_TH_MCS3_LEN)-1)<<RF_TX_CFR_TH_MCS3_POS))
#define RF_TX_CFR_TH_MCS2                                       RF_TX_CFR_TH_MCS2
#define RF_TX_CFR_TH_MCS2_POS                                   (8U)
#define RF_TX_CFR_TH_MCS2_LEN                                   (8U)
#define RF_TX_CFR_TH_MCS2_MSK                                   (((1U<<RF_TX_CFR_TH_MCS2_LEN)-1)<<RF_TX_CFR_TH_MCS2_POS)
#define RF_TX_CFR_TH_MCS2_UMSK                                  (~(((1U<<RF_TX_CFR_TH_MCS2_LEN)-1)<<RF_TX_CFR_TH_MCS2_POS))
#define RF_TX_CFR_TH_MCS1                                       RF_TX_CFR_TH_MCS1
#define RF_TX_CFR_TH_MCS1_POS                                   (16U)
#define RF_TX_CFR_TH_MCS1_LEN                                   (8U)
#define RF_TX_CFR_TH_MCS1_MSK                                   (((1U<<RF_TX_CFR_TH_MCS1_LEN)-1)<<RF_TX_CFR_TH_MCS1_POS)
#define RF_TX_CFR_TH_MCS1_UMSK                                  (~(((1U<<RF_TX_CFR_TH_MCS1_LEN)-1)<<RF_TX_CFR_TH_MCS1_POS))
#define RF_TX_CFR_TH_MCS0                                       RF_TX_CFR_TH_MCS0
#define RF_TX_CFR_TH_MCS0_POS                                   (24U)
#define RF_TX_CFR_TH_MCS0_LEN                                   (8U)
#define RF_TX_CFR_TH_MCS0_MSK                                   (((1U<<RF_TX_CFR_TH_MCS0_LEN)-1)<<RF_TX_CFR_TH_MCS0_POS)
#define RF_TX_CFR_TH_MCS0_UMSK                                  (~(((1U<<RF_TX_CFR_TH_MCS0_LEN)-1)<<RF_TX_CFR_TH_MCS0_POS))

/* 0x638 : dfe_ctrl_14 */
#define RF_DFE_CTRL_14_OFFSET                                   (0x638)
#define RF_TX_CFR_TH_MCS7                                       RF_TX_CFR_TH_MCS7
#define RF_TX_CFR_TH_MCS7_POS                                   (0U)
#define RF_TX_CFR_TH_MCS7_LEN                                   (8U)
#define RF_TX_CFR_TH_MCS7_MSK                                   (((1U<<RF_TX_CFR_TH_MCS7_LEN)-1)<<RF_TX_CFR_TH_MCS7_POS)
#define RF_TX_CFR_TH_MCS7_UMSK                                  (~(((1U<<RF_TX_CFR_TH_MCS7_LEN)-1)<<RF_TX_CFR_TH_MCS7_POS))
#define RF_TX_CFR_TH_MCS6                                       RF_TX_CFR_TH_MCS6
#define RF_TX_CFR_TH_MCS6_POS                                   (8U)
#define RF_TX_CFR_TH_MCS6_LEN                                   (8U)
#define RF_TX_CFR_TH_MCS6_MSK                                   (((1U<<RF_TX_CFR_TH_MCS6_LEN)-1)<<RF_TX_CFR_TH_MCS6_POS)
#define RF_TX_CFR_TH_MCS6_UMSK                                  (~(((1U<<RF_TX_CFR_TH_MCS6_LEN)-1)<<RF_TX_CFR_TH_MCS6_POS))
#define RF_TX_CFR_TH_MCS5                                       RF_TX_CFR_TH_MCS5
#define RF_TX_CFR_TH_MCS5_POS                                   (16U)
#define RF_TX_CFR_TH_MCS5_LEN                                   (8U)
#define RF_TX_CFR_TH_MCS5_MSK                                   (((1U<<RF_TX_CFR_TH_MCS5_LEN)-1)<<RF_TX_CFR_TH_MCS5_POS)
#define RF_TX_CFR_TH_MCS5_UMSK                                  (~(((1U<<RF_TX_CFR_TH_MCS5_LEN)-1)<<RF_TX_CFR_TH_MCS5_POS))
#define RF_TX_CFR_TH_MCS4                                       RF_TX_CFR_TH_MCS4
#define RF_TX_CFR_TH_MCS4_POS                                   (24U)
#define RF_TX_CFR_TH_MCS4_LEN                                   (8U)
#define RF_TX_CFR_TH_MCS4_MSK                                   (((1U<<RF_TX_CFR_TH_MCS4_LEN)-1)<<RF_TX_CFR_TH_MCS4_POS)
#define RF_TX_CFR_TH_MCS4_UMSK                                  (~(((1U<<RF_TX_CFR_TH_MCS4_LEN)-1)<<RF_TX_CFR_TH_MCS4_POS))

/* 0x63C : dfe_ctrl_15 */
#define RF_DFE_CTRL_15_OFFSET                                   (0x63C)
#define RF_TX_DVGA_0_QDB                                        RF_TX_DVGA_0_QDB
#define RF_TX_DVGA_0_QDB_POS                                    (0U)
#define RF_TX_DVGA_0_QDB_LEN                                    (7U)
#define RF_TX_DVGA_0_QDB_MSK                                    (((1U<<RF_TX_DVGA_0_QDB_LEN)-1)<<RF_TX_DVGA_0_QDB_POS)
#define RF_TX_DVGA_0_QDB_UMSK                                   (~(((1U<<RF_TX_DVGA_0_QDB_LEN)-1)<<RF_TX_DVGA_0_QDB_POS))
#define RF_TX_DVGA_0_CTRL_HW                                    RF_TX_DVGA_0_CTRL_HW
#define RF_TX_DVGA_0_CTRL_HW_POS                                (7U)
#define RF_TX_DVGA_0_CTRL_HW_LEN                                (1U)
#define RF_TX_DVGA_0_CTRL_HW_MSK                                (((1U<<RF_TX_DVGA_0_CTRL_HW_LEN)-1)<<RF_TX_DVGA_0_CTRL_HW_POS)
#define RF_TX_DVGA_0_CTRL_HW_UMSK                               (~(((1U<<RF_TX_DVGA_0_CTRL_HW_LEN)-1)<<RF_TX_DVGA_0_CTRL_HW_POS))
#define RF_TX_DVGA_1_QDB                                        RF_TX_DVGA_1_QDB
#define RF_TX_DVGA_1_QDB_POS                                    (8U)
#define RF_TX_DVGA_1_QDB_LEN                                    (7U)
#define RF_TX_DVGA_1_QDB_MSK                                    (((1U<<RF_TX_DVGA_1_QDB_LEN)-1)<<RF_TX_DVGA_1_QDB_POS)
#define RF_TX_DVGA_1_QDB_UMSK                                   (~(((1U<<RF_TX_DVGA_1_QDB_LEN)-1)<<RF_TX_DVGA_1_QDB_POS))
#define RF_TX_DVGA_1_CTRL_HW                                    RF_TX_DVGA_1_CTRL_HW
#define RF_TX_DVGA_1_CTRL_HW_POS                                (15U)
#define RF_TX_DVGA_1_CTRL_HW_LEN                                (1U)
#define RF_TX_DVGA_1_CTRL_HW_MSK                                (((1U<<RF_TX_DVGA_1_CTRL_HW_LEN)-1)<<RF_TX_DVGA_1_CTRL_HW_POS)
#define RF_TX_DVGA_1_CTRL_HW_UMSK                               (~(((1U<<RF_TX_DVGA_1_CTRL_HW_LEN)-1)<<RF_TX_DVGA_1_CTRL_HW_POS))
#define RF_TX_DVGA_2_QDB                                        RF_TX_DVGA_2_QDB
#define RF_TX_DVGA_2_QDB_POS                                    (16U)
#define RF_TX_DVGA_2_QDB_LEN                                    (7U)
#define RF_TX_DVGA_2_QDB_MSK                                    (((1U<<RF_TX_DVGA_2_QDB_LEN)-1)<<RF_TX_DVGA_2_QDB_POS)
#define RF_TX_DVGA_2_QDB_UMSK                                   (~(((1U<<RF_TX_DVGA_2_QDB_LEN)-1)<<RF_TX_DVGA_2_QDB_POS))

/* 0x640 : dfe_ctrl_16 */
#define RF_DFE_CTRL_16_OFFSET                                   (0x640)
#define RF_TX_DFE_DLY                                           RF_TX_DFE_DLY
#define RF_TX_DFE_DLY_POS                                       (0U)
#define RF_TX_DFE_DLY_LEN                                       (9U)
#define RF_TX_DFE_DLY_MSK                                       (((1U<<RF_TX_DFE_DLY_LEN)-1)<<RF_TX_DFE_DLY_POS)
#define RF_TX_DFE_DLY_UMSK                                      (~(((1U<<RF_TX_DFE_DLY_LEN)-1)<<RF_TX_DFE_DLY_POS))

/* 0x680 : dfe_rx_notch_0 */
#define RF_DFE_RX_NOTCH_0_OFFSET                                (0x680)
#define RF_RX_NOTCH2_FNRM                                       RF_RX_NOTCH2_FNRM
#define RF_RX_NOTCH2_FNRM_POS                                   (0U)
#define RF_RX_NOTCH2_FNRM_LEN                                   (11U)
#define RF_RX_NOTCH2_FNRM_MSK                                   (((1U<<RF_RX_NOTCH2_FNRM_LEN)-1)<<RF_RX_NOTCH2_FNRM_POS)
#define RF_RX_NOTCH2_FNRM_UMSK                                  (~(((1U<<RF_RX_NOTCH2_FNRM_LEN)-1)<<RF_RX_NOTCH2_FNRM_POS))
#define RF_RX_NOTCH2_ALPHA                                      RF_RX_NOTCH2_ALPHA
#define RF_RX_NOTCH2_ALPHA_POS                                  (11U)
#define RF_RX_NOTCH2_ALPHA_LEN                                  (4U)
#define RF_RX_NOTCH2_ALPHA_MSK                                  (((1U<<RF_RX_NOTCH2_ALPHA_LEN)-1)<<RF_RX_NOTCH2_ALPHA_POS)
#define RF_RX_NOTCH2_ALPHA_UMSK                                 (~(((1U<<RF_RX_NOTCH2_ALPHA_LEN)-1)<<RF_RX_NOTCH2_ALPHA_POS))
#define RF_RX_NOTCH2_EN                                         RF_RX_NOTCH2_EN
#define RF_RX_NOTCH2_EN_POS                                     (15U)
#define RF_RX_NOTCH2_EN_LEN                                     (1U)
#define RF_RX_NOTCH2_EN_MSK                                     (((1U<<RF_RX_NOTCH2_EN_LEN)-1)<<RF_RX_NOTCH2_EN_POS)
#define RF_RX_NOTCH2_EN_UMSK                                    (~(((1U<<RF_RX_NOTCH2_EN_LEN)-1)<<RF_RX_NOTCH2_EN_POS))
#define RF_RX_NOTCH1_FNRM                                       RF_RX_NOTCH1_FNRM
#define RF_RX_NOTCH1_FNRM_POS                                   (16U)
#define RF_RX_NOTCH1_FNRM_LEN                                   (11U)
#define RF_RX_NOTCH1_FNRM_MSK                                   (((1U<<RF_RX_NOTCH1_FNRM_LEN)-1)<<RF_RX_NOTCH1_FNRM_POS)
#define RF_RX_NOTCH1_FNRM_UMSK                                  (~(((1U<<RF_RX_NOTCH1_FNRM_LEN)-1)<<RF_RX_NOTCH1_FNRM_POS))
#define RF_RX_NOTCH1_ALPHA                                      RF_RX_NOTCH1_ALPHA
#define RF_RX_NOTCH1_ALPHA_POS                                  (27U)
#define RF_RX_NOTCH1_ALPHA_LEN                                  (4U)
#define RF_RX_NOTCH1_ALPHA_MSK                                  (((1U<<RF_RX_NOTCH1_ALPHA_LEN)-1)<<RF_RX_NOTCH1_ALPHA_POS)
#define RF_RX_NOTCH1_ALPHA_UMSK                                 (~(((1U<<RF_RX_NOTCH1_ALPHA_LEN)-1)<<RF_RX_NOTCH1_ALPHA_POS))
#define RF_RX_NOTCH1_EN                                         RF_RX_NOTCH1_EN
#define RF_RX_NOTCH1_EN_POS                                     (31U)
#define RF_RX_NOTCH1_EN_LEN                                     (1U)
#define RF_RX_NOTCH1_EN_MSK                                     (((1U<<RF_RX_NOTCH1_EN_LEN)-1)<<RF_RX_NOTCH1_EN_POS)
#define RF_RX_NOTCH1_EN_UMSK                                    (~(((1U<<RF_RX_NOTCH1_EN_LEN)-1)<<RF_RX_NOTCH1_EN_POS))

/* 0x684 : dfe_rx_notch_1 */
#define RF_DFE_RX_NOTCH_1_OFFSET                                (0x684)
#define RF_RX_NOTCH_TAR_CH_0                                    RF_RX_NOTCH_TAR_CH_0
#define RF_RX_NOTCH_TAR_CH_0_POS                                (0U)
#define RF_RX_NOTCH_TAR_CH_0_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_0_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_0_LEN)-1)<<RF_RX_NOTCH_TAR_CH_0_POS)
#define RF_RX_NOTCH_TAR_CH_0_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_0_LEN)-1)<<RF_RX_NOTCH_TAR_CH_0_POS))
#define RF_RX_NOTCH_TAR_CH_1                                    RF_RX_NOTCH_TAR_CH_1
#define RF_RX_NOTCH_TAR_CH_1_POS                                (8U)
#define RF_RX_NOTCH_TAR_CH_1_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_1_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_1_LEN)-1)<<RF_RX_NOTCH_TAR_CH_1_POS)
#define RF_RX_NOTCH_TAR_CH_1_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_1_LEN)-1)<<RF_RX_NOTCH_TAR_CH_1_POS))
#define RF_RX_NOTCH_TAR_CH_2                                    RF_RX_NOTCH_TAR_CH_2
#define RF_RX_NOTCH_TAR_CH_2_POS                                (16U)
#define RF_RX_NOTCH_TAR_CH_2_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_2_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_2_LEN)-1)<<RF_RX_NOTCH_TAR_CH_2_POS)
#define RF_RX_NOTCH_TAR_CH_2_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_2_LEN)-1)<<RF_RX_NOTCH_TAR_CH_2_POS))
#define RF_RX_NOTCH_TAR_CH_3                                    RF_RX_NOTCH_TAR_CH_3
#define RF_RX_NOTCH_TAR_CH_3_POS                                (24U)
#define RF_RX_NOTCH_TAR_CH_3_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_3_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_3_LEN)-1)<<RF_RX_NOTCH_TAR_CH_3_POS)
#define RF_RX_NOTCH_TAR_CH_3_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_3_LEN)-1)<<RF_RX_NOTCH_TAR_CH_3_POS))
#define RF_RX_NOTCH_CTRL_HW                                     RF_RX_NOTCH_CTRL_HW
#define RF_RX_NOTCH_CTRL_HW_POS                                 (31U)
#define RF_RX_NOTCH_CTRL_HW_LEN                                 (1U)
#define RF_RX_NOTCH_CTRL_HW_MSK                                 (((1U<<RF_RX_NOTCH_CTRL_HW_LEN)-1)<<RF_RX_NOTCH_CTRL_HW_POS)
#define RF_RX_NOTCH_CTRL_HW_UMSK                                (~(((1U<<RF_RX_NOTCH_CTRL_HW_LEN)-1)<<RF_RX_NOTCH_CTRL_HW_POS))

/* 0x688 : dfe_rx_notch_2 */
#define RF_DFE_RX_NOTCH_2_OFFSET                                (0x688)
#define RF_RX_NOTCH_TAR_CH_4                                    RF_RX_NOTCH_TAR_CH_4
#define RF_RX_NOTCH_TAR_CH_4_POS                                (0U)
#define RF_RX_NOTCH_TAR_CH_4_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_4_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_4_LEN)-1)<<RF_RX_NOTCH_TAR_CH_4_POS)
#define RF_RX_NOTCH_TAR_CH_4_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_4_LEN)-1)<<RF_RX_NOTCH_TAR_CH_4_POS))
#define RF_RX_NOTCH_TAR_CH_5                                    RF_RX_NOTCH_TAR_CH_5
#define RF_RX_NOTCH_TAR_CH_5_POS                                (8U)
#define RF_RX_NOTCH_TAR_CH_5_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_5_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_5_LEN)-1)<<RF_RX_NOTCH_TAR_CH_5_POS)
#define RF_RX_NOTCH_TAR_CH_5_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_5_LEN)-1)<<RF_RX_NOTCH_TAR_CH_5_POS))
#define RF_RX_NOTCH_TAR_CH_6                                    RF_RX_NOTCH_TAR_CH_6
#define RF_RX_NOTCH_TAR_CH_6_POS                                (16U)
#define RF_RX_NOTCH_TAR_CH_6_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_6_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_6_LEN)-1)<<RF_RX_NOTCH_TAR_CH_6_POS)
#define RF_RX_NOTCH_TAR_CH_6_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_6_LEN)-1)<<RF_RX_NOTCH_TAR_CH_6_POS))
#define RF_RX_NOTCH_TAR_CH_7                                    RF_RX_NOTCH_TAR_CH_7
#define RF_RX_NOTCH_TAR_CH_7_POS                                (24U)
#define RF_RX_NOTCH_TAR_CH_7_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_7_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_7_LEN)-1)<<RF_RX_NOTCH_TAR_CH_7_POS)
#define RF_RX_NOTCH_TAR_CH_7_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_7_LEN)-1)<<RF_RX_NOTCH_TAR_CH_7_POS))

/* 0x68C : dfe_rx_notch_3 */
#define RF_DFE_RX_NOTCH_3_OFFSET                                (0x68C)
#define RF_RX_NOTCH_TAR_CH_8                                    RF_RX_NOTCH_TAR_CH_8
#define RF_RX_NOTCH_TAR_CH_8_POS                                (0U)
#define RF_RX_NOTCH_TAR_CH_8_LEN                                (7U)
#define RF_RX_NOTCH_TAR_CH_8_MSK                                (((1U<<RF_RX_NOTCH_TAR_CH_8_LEN)-1)<<RF_RX_NOTCH_TAR_CH_8_POS)
#define RF_RX_NOTCH_TAR_CH_8_UMSK                               (~(((1U<<RF_RX_NOTCH_TAR_CH_8_LEN)-1)<<RF_RX_NOTCH_TAR_CH_8_POS))

/* 0x690 : dfe_rx_notch_4 */
#define RF_DFE_RX_NOTCH_4_OFFSET                                (0x690)
#define RF_RX_NOTCH_PARA_0                                      RF_RX_NOTCH_PARA_0
#define RF_RX_NOTCH_PARA_0_POS                                  (0U)
#define RF_RX_NOTCH_PARA_0_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_0_MSK                                  (((1U<<RF_RX_NOTCH_PARA_0_LEN)-1)<<RF_RX_NOTCH_PARA_0_POS)
#define RF_RX_NOTCH_PARA_0_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_0_LEN)-1)<<RF_RX_NOTCH_PARA_0_POS))

/* 0x694 : dfe_rx_notch_5 */
#define RF_DFE_RX_NOTCH_5_OFFSET                                (0x694)
#define RF_RX_NOTCH_PARA_1                                      RF_RX_NOTCH_PARA_1
#define RF_RX_NOTCH_PARA_1_POS                                  (0U)
#define RF_RX_NOTCH_PARA_1_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_1_MSK                                  (((1U<<RF_RX_NOTCH_PARA_1_LEN)-1)<<RF_RX_NOTCH_PARA_1_POS)
#define RF_RX_NOTCH_PARA_1_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_1_LEN)-1)<<RF_RX_NOTCH_PARA_1_POS))

/* 0x698 : dfe_rx_notch_6 */
#define RF_DFE_RX_NOTCH_6_OFFSET                                (0x698)
#define RF_RX_NOTCH_PARA_2                                      RF_RX_NOTCH_PARA_2
#define RF_RX_NOTCH_PARA_2_POS                                  (0U)
#define RF_RX_NOTCH_PARA_2_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_2_MSK                                  (((1U<<RF_RX_NOTCH_PARA_2_LEN)-1)<<RF_RX_NOTCH_PARA_2_POS)
#define RF_RX_NOTCH_PARA_2_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_2_LEN)-1)<<RF_RX_NOTCH_PARA_2_POS))

/* 0x69C : dfe_rx_notch_7 */
#define RF_DFE_RX_NOTCH_7_OFFSET                                (0x69C)
#define RF_RX_NOTCH_PARA_3                                      RF_RX_NOTCH_PARA_3
#define RF_RX_NOTCH_PARA_3_POS                                  (0U)
#define RF_RX_NOTCH_PARA_3_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_3_MSK                                  (((1U<<RF_RX_NOTCH_PARA_3_LEN)-1)<<RF_RX_NOTCH_PARA_3_POS)
#define RF_RX_NOTCH_PARA_3_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_3_LEN)-1)<<RF_RX_NOTCH_PARA_3_POS))

/* 0x6A0 : dfe_rx_notch_8 */
#define RF_DFE_RX_NOTCH_8_OFFSET                                (0x6A0)
#define RF_RX_NOTCH_PARA_4                                      RF_RX_NOTCH_PARA_4
#define RF_RX_NOTCH_PARA_4_POS                                  (0U)
#define RF_RX_NOTCH_PARA_4_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_4_MSK                                  (((1U<<RF_RX_NOTCH_PARA_4_LEN)-1)<<RF_RX_NOTCH_PARA_4_POS)
#define RF_RX_NOTCH_PARA_4_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_4_LEN)-1)<<RF_RX_NOTCH_PARA_4_POS))

/* 0x6A4 : dfe_rx_notch_9 */
#define RF_DFE_RX_NOTCH_9_OFFSET                                (0x6A4)
#define RF_RX_NOTCH_PARA_5                                      RF_RX_NOTCH_PARA_5
#define RF_RX_NOTCH_PARA_5_POS                                  (0U)
#define RF_RX_NOTCH_PARA_5_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_5_MSK                                  (((1U<<RF_RX_NOTCH_PARA_5_LEN)-1)<<RF_RX_NOTCH_PARA_5_POS)
#define RF_RX_NOTCH_PARA_5_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_5_LEN)-1)<<RF_RX_NOTCH_PARA_5_POS))

/* 0x6A8 : dfe_rx_notch_10 */
#define RF_DFE_RX_NOTCH_10_OFFSET                               (0x6A8)
#define RF_RX_NOTCH_PARA_6                                      RF_RX_NOTCH_PARA_6
#define RF_RX_NOTCH_PARA_6_POS                                  (0U)
#define RF_RX_NOTCH_PARA_6_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_6_MSK                                  (((1U<<RF_RX_NOTCH_PARA_6_LEN)-1)<<RF_RX_NOTCH_PARA_6_POS)
#define RF_RX_NOTCH_PARA_6_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_6_LEN)-1)<<RF_RX_NOTCH_PARA_6_POS))

/* 0x6AC : dfe_rx_notch_11 */
#define RF_DFE_RX_NOTCH_11_OFFSET                               (0x6AC)
#define RF_RX_NOTCH_PARA_7                                      RF_RX_NOTCH_PARA_7
#define RF_RX_NOTCH_PARA_7_POS                                  (0U)
#define RF_RX_NOTCH_PARA_7_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_7_MSK                                  (((1U<<RF_RX_NOTCH_PARA_7_LEN)-1)<<RF_RX_NOTCH_PARA_7_POS)
#define RF_RX_NOTCH_PARA_7_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_7_LEN)-1)<<RF_RX_NOTCH_PARA_7_POS))

/* 0x6B0 : dfe_rx_notch_12 */
#define RF_DFE_RX_NOTCH_12_OFFSET                               (0x6B0)
#define RF_RX_NOTCH_PARA_8                                      RF_RX_NOTCH_PARA_8
#define RF_RX_NOTCH_PARA_8_POS                                  (0U)
#define RF_RX_NOTCH_PARA_8_LEN                                  (32U)
#define RF_RX_NOTCH_PARA_8_MSK                                  (((1U<<RF_RX_NOTCH_PARA_8_LEN)-1)<<RF_RX_NOTCH_PARA_8_POS)
#define RF_RX_NOTCH_PARA_8_UMSK                                 (~(((1U<<RF_RX_NOTCH_PARA_8_LEN)-1)<<RF_RX_NOTCH_PARA_8_POS))

/* 0x6B4 : dfe_rx_notch_13 */
#define RF_DFE_RX_NOTCH_13_OFFSET                               (0x6B4)
#define RF_RX_NOTCH2_FNRM_HW                                    RF_RX_NOTCH2_FNRM_HW
#define RF_RX_NOTCH2_FNRM_HW_POS                                (0U)
#define RF_RX_NOTCH2_FNRM_HW_LEN                                (11U)
#define RF_RX_NOTCH2_FNRM_HW_MSK                                (((1U<<RF_RX_NOTCH2_FNRM_HW_LEN)-1)<<RF_RX_NOTCH2_FNRM_HW_POS)
#define RF_RX_NOTCH2_FNRM_HW_UMSK                               (~(((1U<<RF_RX_NOTCH2_FNRM_HW_LEN)-1)<<RF_RX_NOTCH2_FNRM_HW_POS))
#define RF_RX_NOTCH2_ALPHA_HW                                   RF_RX_NOTCH2_ALPHA_HW
#define RF_RX_NOTCH2_ALPHA_HW_POS                               (11U)
#define RF_RX_NOTCH2_ALPHA_HW_LEN                               (4U)
#define RF_RX_NOTCH2_ALPHA_HW_MSK                               (((1U<<RF_RX_NOTCH2_ALPHA_HW_LEN)-1)<<RF_RX_NOTCH2_ALPHA_HW_POS)
#define RF_RX_NOTCH2_ALPHA_HW_UMSK                              (~(((1U<<RF_RX_NOTCH2_ALPHA_HW_LEN)-1)<<RF_RX_NOTCH2_ALPHA_HW_POS))
#define RF_RX_NOTCH2_EN_HW                                      RF_RX_NOTCH2_EN_HW
#define RF_RX_NOTCH2_EN_HW_POS                                  (15U)
#define RF_RX_NOTCH2_EN_HW_LEN                                  (1U)
#define RF_RX_NOTCH2_EN_HW_MSK                                  (((1U<<RF_RX_NOTCH2_EN_HW_LEN)-1)<<RF_RX_NOTCH2_EN_HW_POS)
#define RF_RX_NOTCH2_EN_HW_UMSK                                 (~(((1U<<RF_RX_NOTCH2_EN_HW_LEN)-1)<<RF_RX_NOTCH2_EN_HW_POS))
#define RF_RX_NOTCH1_FNRM_HW                                    RF_RX_NOTCH1_FNRM_HW
#define RF_RX_NOTCH1_FNRM_HW_POS                                (16U)
#define RF_RX_NOTCH1_FNRM_HW_LEN                                (11U)
#define RF_RX_NOTCH1_FNRM_HW_MSK                                (((1U<<RF_RX_NOTCH1_FNRM_HW_LEN)-1)<<RF_RX_NOTCH1_FNRM_HW_POS)
#define RF_RX_NOTCH1_FNRM_HW_UMSK                               (~(((1U<<RF_RX_NOTCH1_FNRM_HW_LEN)-1)<<RF_RX_NOTCH1_FNRM_HW_POS))
#define RF_RX_NOTCH1_ALPHA_HW                                   RF_RX_NOTCH1_ALPHA_HW
#define RF_RX_NOTCH1_ALPHA_HW_POS                               (27U)
#define RF_RX_NOTCH1_ALPHA_HW_LEN                               (4U)
#define RF_RX_NOTCH1_ALPHA_HW_MSK                               (((1U<<RF_RX_NOTCH1_ALPHA_HW_LEN)-1)<<RF_RX_NOTCH1_ALPHA_HW_POS)
#define RF_RX_NOTCH1_ALPHA_HW_UMSK                              (~(((1U<<RF_RX_NOTCH1_ALPHA_HW_LEN)-1)<<RF_RX_NOTCH1_ALPHA_HW_POS))
#define RF_RX_NOTCH1_EN_HW                                      RF_RX_NOTCH1_EN_HW
#define RF_RX_NOTCH1_EN_HW_POS                                  (31U)
#define RF_RX_NOTCH1_EN_HW_LEN                                  (1U)
#define RF_RX_NOTCH1_EN_HW_MSK                                  (((1U<<RF_RX_NOTCH1_EN_HW_LEN)-1)<<RF_RX_NOTCH1_EN_HW_POS)
#define RF_RX_NOTCH1_EN_HW_UMSK                                 (~(((1U<<RF_RX_NOTCH1_EN_HW_LEN)-1)<<RF_RX_NOTCH1_EN_HW_POS))

/* 0x700 : pwr_wifi_ctrl_0 */
#define RF_PWR_WIFI_CTRL_0_OFFSET                               (0x700)
#define RF_WIFI_NOMINAL_PWR_0                                   RF_WIFI_NOMINAL_PWR_0
#define RF_WIFI_NOMINAL_PWR_0_POS                               (0U)
#define RF_WIFI_NOMINAL_PWR_0_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_0_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_0_LEN)-1)<<RF_WIFI_NOMINAL_PWR_0_POS)
#define RF_WIFI_NOMINAL_PWR_0_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_0_LEN)-1)<<RF_WIFI_NOMINAL_PWR_0_POS))
#define RF_WIFI_NOMINAL_PWR_1                                   RF_WIFI_NOMINAL_PWR_1
#define RF_WIFI_NOMINAL_PWR_1_POS                               (6U)
#define RF_WIFI_NOMINAL_PWR_1_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_1_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_1_LEN)-1)<<RF_WIFI_NOMINAL_PWR_1_POS)
#define RF_WIFI_NOMINAL_PWR_1_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_1_LEN)-1)<<RF_WIFI_NOMINAL_PWR_1_POS))
#define RF_WIFI_NOMINAL_PWR_2                                   RF_WIFI_NOMINAL_PWR_2
#define RF_WIFI_NOMINAL_PWR_2_POS                               (12U)
#define RF_WIFI_NOMINAL_PWR_2_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_2_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_2_LEN)-1)<<RF_WIFI_NOMINAL_PWR_2_POS)
#define RF_WIFI_NOMINAL_PWR_2_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_2_LEN)-1)<<RF_WIFI_NOMINAL_PWR_2_POS))
#define RF_WIFI_NOMINAL_PWR_3                                   RF_WIFI_NOMINAL_PWR_3
#define RF_WIFI_NOMINAL_PWR_3_POS                               (18U)
#define RF_WIFI_NOMINAL_PWR_3_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_3_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_3_LEN)-1)<<RF_WIFI_NOMINAL_PWR_3_POS)
#define RF_WIFI_NOMINAL_PWR_3_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_3_LEN)-1)<<RF_WIFI_NOMINAL_PWR_3_POS))
#define RF_M154_TX_PWR                                          RF_M154_TX_PWR
#define RF_M154_TX_PWR_POS                                      (24U)
#define RF_M154_TX_PWR_LEN                                      (8U)
#define RF_M154_TX_PWR_MSK                                      (((1U<<RF_M154_TX_PWR_LEN)-1)<<RF_M154_TX_PWR_POS)
#define RF_M154_TX_PWR_UMSK                                     (~(((1U<<RF_M154_TX_PWR_LEN)-1)<<RF_M154_TX_PWR_POS))

/* 0x704 : pwr_wifi_ctrl_1 */
#define RF_PWR_WIFI_CTRL_1_OFFSET                               (0x704)
#define RF_WIFI_NOMINAL_PWR_4                                   RF_WIFI_NOMINAL_PWR_4
#define RF_WIFI_NOMINAL_PWR_4_POS                               (0U)
#define RF_WIFI_NOMINAL_PWR_4_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_4_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_4_LEN)-1)<<RF_WIFI_NOMINAL_PWR_4_POS)
#define RF_WIFI_NOMINAL_PWR_4_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_4_LEN)-1)<<RF_WIFI_NOMINAL_PWR_4_POS))
#define RF_WIFI_NOMINAL_PWR_5                                   RF_WIFI_NOMINAL_PWR_5
#define RF_WIFI_NOMINAL_PWR_5_POS                               (6U)
#define RF_WIFI_NOMINAL_PWR_5_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_5_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_5_LEN)-1)<<RF_WIFI_NOMINAL_PWR_5_POS)
#define RF_WIFI_NOMINAL_PWR_5_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_5_LEN)-1)<<RF_WIFI_NOMINAL_PWR_5_POS))
#define RF_WIFI_NOMINAL_PWR_6                                   RF_WIFI_NOMINAL_PWR_6
#define RF_WIFI_NOMINAL_PWR_6_POS                               (12U)
#define RF_WIFI_NOMINAL_PWR_6_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_6_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_6_LEN)-1)<<RF_WIFI_NOMINAL_PWR_6_POS)
#define RF_WIFI_NOMINAL_PWR_6_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_6_LEN)-1)<<RF_WIFI_NOMINAL_PWR_6_POS))
#define RF_WIFI_NOMINAL_PWR_7                                   RF_WIFI_NOMINAL_PWR_7
#define RF_WIFI_NOMINAL_PWR_7_POS                               (18U)
#define RF_WIFI_NOMINAL_PWR_7_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_7_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_7_LEN)-1)<<RF_WIFI_NOMINAL_PWR_7_POS)
#define RF_WIFI_NOMINAL_PWR_7_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_7_LEN)-1)<<RF_WIFI_NOMINAL_PWR_7_POS))
#define RF_WIFI_TX_CORR                                         RF_WIFI_TX_CORR
#define RF_WIFI_TX_CORR_POS                                     (24U)
#define RF_WIFI_TX_CORR_LEN                                     (8U)
#define RF_WIFI_TX_CORR_MSK                                     (((1U<<RF_WIFI_TX_CORR_LEN)-1)<<RF_WIFI_TX_CORR_POS)
#define RF_WIFI_TX_CORR_UMSK                                    (~(((1U<<RF_WIFI_TX_CORR_LEN)-1)<<RF_WIFI_TX_CORR_POS))

/* 0x708 : pwr_wifi_ctrl_2 */
#define RF_PWR_WIFI_CTRL_2_OFFSET                               (0x708)
#define RF_WIFI_NOMINAL_PWR_8                                   RF_WIFI_NOMINAL_PWR_8
#define RF_WIFI_NOMINAL_PWR_8_POS                               (0U)
#define RF_WIFI_NOMINAL_PWR_8_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_8_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_8_LEN)-1)<<RF_WIFI_NOMINAL_PWR_8_POS)
#define RF_WIFI_NOMINAL_PWR_8_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_8_LEN)-1)<<RF_WIFI_NOMINAL_PWR_8_POS))
#define RF_WIFI_NOMINAL_PWR_9                                   RF_WIFI_NOMINAL_PWR_9
#define RF_WIFI_NOMINAL_PWR_9_POS                               (6U)
#define RF_WIFI_NOMINAL_PWR_9_LEN                               (6U)
#define RF_WIFI_NOMINAL_PWR_9_MSK                               (((1U<<RF_WIFI_NOMINAL_PWR_9_LEN)-1)<<RF_WIFI_NOMINAL_PWR_9_POS)
#define RF_WIFI_NOMINAL_PWR_9_UMSK                              (~(((1U<<RF_WIFI_NOMINAL_PWR_9_LEN)-1)<<RF_WIFI_NOMINAL_PWR_9_POS))
#define RF_WIFI_NOMINAL_PWR_10                                  RF_WIFI_NOMINAL_PWR_10
#define RF_WIFI_NOMINAL_PWR_10_POS                              (12U)
#define RF_WIFI_NOMINAL_PWR_10_LEN                              (6U)
#define RF_WIFI_NOMINAL_PWR_10_MSK                              (((1U<<RF_WIFI_NOMINAL_PWR_10_LEN)-1)<<RF_WIFI_NOMINAL_PWR_10_POS)
#define RF_WIFI_NOMINAL_PWR_10_UMSK                             (~(((1U<<RF_WIFI_NOMINAL_PWR_10_LEN)-1)<<RF_WIFI_NOMINAL_PWR_10_POS))
#define RF_WIFI_NOMINAL_PWR_11                                  RF_WIFI_NOMINAL_PWR_11
#define RF_WIFI_NOMINAL_PWR_11_POS                              (18U)
#define RF_WIFI_NOMINAL_PWR_11_LEN                              (6U)
#define RF_WIFI_NOMINAL_PWR_11_MSK                              (((1U<<RF_WIFI_NOMINAL_PWR_11_LEN)-1)<<RF_WIFI_NOMINAL_PWR_11_POS)
#define RF_WIFI_NOMINAL_PWR_11_UMSK                             (~(((1U<<RF_WIFI_NOMINAL_PWR_11_LEN)-1)<<RF_WIFI_NOMINAL_PWR_11_POS))
#define RF_WIFI_NOMINAL_PWR_12                                  RF_WIFI_NOMINAL_PWR_12
#define RF_WIFI_NOMINAL_PWR_12_POS                              (24U)
#define RF_WIFI_NOMINAL_PWR_12_LEN                              (6U)
#define RF_WIFI_NOMINAL_PWR_12_MSK                              (((1U<<RF_WIFI_NOMINAL_PWR_12_LEN)-1)<<RF_WIFI_NOMINAL_PWR_12_POS)
#define RF_WIFI_NOMINAL_PWR_12_UMSK                             (~(((1U<<RF_WIFI_NOMINAL_PWR_12_LEN)-1)<<RF_WIFI_NOMINAL_PWR_12_POS))

/* 0x70C : pwr_wifi_tbl_0 */
#define RF_PWR_WIFI_TBL_0_OFFSET                                (0x70C)
#define RF_WIFI_IQC_GAIN_0                                      RF_WIFI_IQC_GAIN_0
#define RF_WIFI_IQC_GAIN_0_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_0_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_0_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_0_LEN)-1)<<RF_WIFI_IQC_GAIN_0_POS)
#define RF_WIFI_IQC_GAIN_0_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_0_LEN)-1)<<RF_WIFI_IQC_GAIN_0_POS))
#define RF_WIFI_DG1_QDB_0                                       RF_WIFI_DG1_QDB_0
#define RF_WIFI_DG1_QDB_0_POS                                   (11U)
#define RF_WIFI_DG1_QDB_0_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_0_MSK                                   (((1U<<RF_WIFI_DG1_QDB_0_LEN)-1)<<RF_WIFI_DG1_QDB_0_POS)
#define RF_WIFI_DG1_QDB_0_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_0_LEN)-1)<<RF_WIFI_DG1_QDB_0_POS))
#define RF_WIFI_GC_TMX_0                                        RF_WIFI_GC_TMX_0
#define RF_WIFI_GC_TMX_0_POS                                    (18U)
#define RF_WIFI_GC_TMX_0_LEN                                    (3U)
#define RF_WIFI_GC_TMX_0_MSK                                    (((1U<<RF_WIFI_GC_TMX_0_LEN)-1)<<RF_WIFI_GC_TMX_0_POS)
#define RF_WIFI_GC_TMX_0_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_0_LEN)-1)<<RF_WIFI_GC_TMX_0_POS))
#define RF_WIFI_GC_TBB_0                                        RF_WIFI_GC_TBB_0
#define RF_WIFI_GC_TBB_0_POS                                    (21U)
#define RF_WIFI_GC_TBB_0_LEN                                    (5U)
#define RF_WIFI_GC_TBB_0_MSK                                    (((1U<<RF_WIFI_GC_TBB_0_LEN)-1)<<RF_WIFI_GC_TBB_0_POS)
#define RF_WIFI_GC_TBB_0_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_0_LEN)-1)<<RF_WIFI_GC_TBB_0_POS))
#define RF_WIFI_GC_TBB_BOOST_0                                  RF_WIFI_GC_TBB_BOOST_0
#define RF_WIFI_GC_TBB_BOOST_0_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_0_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_0_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_0_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_0_POS)
#define RF_WIFI_GC_TBB_BOOST_0_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_0_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_0_POS))
#define RF_WIFI_DAC_BIAS_0                                      RF_WIFI_DAC_BIAS_0
#define RF_WIFI_DAC_BIAS_0_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_0_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_0_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_0_LEN)-1)<<RF_WIFI_DAC_BIAS_0_POS)
#define RF_WIFI_DAC_BIAS_0_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_0_LEN)-1)<<RF_WIFI_DAC_BIAS_0_POS))
#define RF_WIFI_AX_SUBMODE_0                                    RF_WIFI_AX_SUBMODE_0
#define RF_WIFI_AX_SUBMODE_0_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_0_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_0_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_0_LEN)-1)<<RF_WIFI_AX_SUBMODE_0_POS)
#define RF_WIFI_AX_SUBMODE_0_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_0_LEN)-1)<<RF_WIFI_AX_SUBMODE_0_POS))

/* 0x710 : pwr_wifi_tbl_1 */
#define RF_PWR_WIFI_TBL_1_OFFSET                                (0x710)
#define RF_WIFI_DPD_SEL_0                                       RF_WIFI_DPD_SEL_0
#define RF_WIFI_DPD_SEL_0_POS                                   (0U)
#define RF_WIFI_DPD_SEL_0_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_0_MSK                                   (((1U<<RF_WIFI_DPD_SEL_0_LEN)-1)<<RF_WIFI_DPD_SEL_0_POS)
#define RF_WIFI_DPD_SEL_0_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_0_LEN)-1)<<RF_WIFI_DPD_SEL_0_POS))
#define RF_WIFI_TOSDAC_Q_0                                      RF_WIFI_TOSDAC_Q_0
#define RF_WIFI_TOSDAC_Q_0_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_0_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_0_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_0_LEN)-1)<<RF_WIFI_TOSDAC_Q_0_POS)
#define RF_WIFI_TOSDAC_Q_0_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_0_LEN)-1)<<RF_WIFI_TOSDAC_Q_0_POS))
#define RF_WIFI_TOSDAC_I_0                                      RF_WIFI_TOSDAC_I_0
#define RF_WIFI_TOSDAC_I_0_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_0_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_0_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_0_LEN)-1)<<RF_WIFI_TOSDAC_I_0_POS)
#define RF_WIFI_TOSDAC_I_0_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_0_LEN)-1)<<RF_WIFI_TOSDAC_I_0_POS))
#define RF_WIFI_IQC_PHASE_0                                     RF_WIFI_IQC_PHASE_0
#define RF_WIFI_IQC_PHASE_0_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_0_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_0_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_0_LEN)-1)<<RF_WIFI_IQC_PHASE_0_POS)
#define RF_WIFI_IQC_PHASE_0_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_0_LEN)-1)<<RF_WIFI_IQC_PHASE_0_POS))
#define RF_WIFI_GC_PA_0                                         RF_WIFI_GC_PA_0
#define RF_WIFI_GC_PA_0_POS                                     (24U)
#define RF_WIFI_GC_PA_0_LEN                                     (3U)
#define RF_WIFI_GC_PA_0_MSK                                     (((1U<<RF_WIFI_GC_PA_0_LEN)-1)<<RF_WIFI_GC_PA_0_POS)
#define RF_WIFI_GC_PA_0_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_0_LEN)-1)<<RF_WIFI_GC_PA_0_POS))
#define RF_WIFI_PA_LP_EN_0                                      RF_WIFI_PA_LP_EN_0
#define RF_WIFI_PA_LP_EN_0_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_0_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_0_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_0_LEN)-1)<<RF_WIFI_PA_LP_EN_0_POS)
#define RF_WIFI_PA_LP_EN_0_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_0_LEN)-1)<<RF_WIFI_PA_LP_EN_0_POS))
#define RF_WIFI_DPD_EN_B_0                                      RF_WIFI_DPD_EN_B_0
#define RF_WIFI_DPD_EN_B_0_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_0_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_0_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_0_LEN)-1)<<RF_WIFI_DPD_EN_B_0_POS)
#define RF_WIFI_DPD_EN_B_0_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_0_LEN)-1)<<RF_WIFI_DPD_EN_B_0_POS))
#define RF_WIFI_DPD_EN_G_0                                      RF_WIFI_DPD_EN_G_0
#define RF_WIFI_DPD_EN_G_0_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_0_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_0_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_0_LEN)-1)<<RF_WIFI_DPD_EN_G_0_POS)
#define RF_WIFI_DPD_EN_G_0_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_0_LEN)-1)<<RF_WIFI_DPD_EN_G_0_POS))
#define RF_WIFI_DPD_EN_N_0                                      RF_WIFI_DPD_EN_N_0
#define RF_WIFI_DPD_EN_N_0_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_0_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_0_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_0_LEN)-1)<<RF_WIFI_DPD_EN_N_0_POS)
#define RF_WIFI_DPD_EN_N_0_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_0_LEN)-1)<<RF_WIFI_DPD_EN_N_0_POS))
#define RF_WIFI_DPD_EN_AX_0                                     RF_WIFI_DPD_EN_AX_0
#define RF_WIFI_DPD_EN_AX_0_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_0_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_0_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_0_LEN)-1)<<RF_WIFI_DPD_EN_AX_0_POS)
#define RF_WIFI_DPD_EN_AX_0_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_0_LEN)-1)<<RF_WIFI_DPD_EN_AX_0_POS))

/* 0x714 : pwr_wifi_tbl_2 */
#define RF_PWR_WIFI_TBL_2_OFFSET                                (0x714)
#define RF_WIFI_IQC_GAIN_1                                      RF_WIFI_IQC_GAIN_1
#define RF_WIFI_IQC_GAIN_1_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_1_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_1_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_1_LEN)-1)<<RF_WIFI_IQC_GAIN_1_POS)
#define RF_WIFI_IQC_GAIN_1_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_1_LEN)-1)<<RF_WIFI_IQC_GAIN_1_POS))
#define RF_WIFI_DG1_QDB_1                                       RF_WIFI_DG1_QDB_1
#define RF_WIFI_DG1_QDB_1_POS                                   (11U)
#define RF_WIFI_DG1_QDB_1_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_1_MSK                                   (((1U<<RF_WIFI_DG1_QDB_1_LEN)-1)<<RF_WIFI_DG1_QDB_1_POS)
#define RF_WIFI_DG1_QDB_1_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_1_LEN)-1)<<RF_WIFI_DG1_QDB_1_POS))
#define RF_WIFI_GC_TMX_1                                        RF_WIFI_GC_TMX_1
#define RF_WIFI_GC_TMX_1_POS                                    (18U)
#define RF_WIFI_GC_TMX_1_LEN                                    (3U)
#define RF_WIFI_GC_TMX_1_MSK                                    (((1U<<RF_WIFI_GC_TMX_1_LEN)-1)<<RF_WIFI_GC_TMX_1_POS)
#define RF_WIFI_GC_TMX_1_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_1_LEN)-1)<<RF_WIFI_GC_TMX_1_POS))
#define RF_WIFI_GC_TBB_1                                        RF_WIFI_GC_TBB_1
#define RF_WIFI_GC_TBB_1_POS                                    (21U)
#define RF_WIFI_GC_TBB_1_LEN                                    (5U)
#define RF_WIFI_GC_TBB_1_MSK                                    (((1U<<RF_WIFI_GC_TBB_1_LEN)-1)<<RF_WIFI_GC_TBB_1_POS)
#define RF_WIFI_GC_TBB_1_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_1_LEN)-1)<<RF_WIFI_GC_TBB_1_POS))
#define RF_WIFI_GC_TBB_BOOST_1                                  RF_WIFI_GC_TBB_BOOST_1
#define RF_WIFI_GC_TBB_BOOST_1_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_1_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_1_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_1_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_1_POS)
#define RF_WIFI_GC_TBB_BOOST_1_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_1_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_1_POS))
#define RF_WIFI_DAC_BIAS_1                                      RF_WIFI_DAC_BIAS_1
#define RF_WIFI_DAC_BIAS_1_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_1_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_1_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_1_LEN)-1)<<RF_WIFI_DAC_BIAS_1_POS)
#define RF_WIFI_DAC_BIAS_1_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_1_LEN)-1)<<RF_WIFI_DAC_BIAS_1_POS))
#define RF_WIFI_AX_SUBMODE_1                                    RF_WIFI_AX_SUBMODE_1
#define RF_WIFI_AX_SUBMODE_1_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_1_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_1_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_1_LEN)-1)<<RF_WIFI_AX_SUBMODE_1_POS)
#define RF_WIFI_AX_SUBMODE_1_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_1_LEN)-1)<<RF_WIFI_AX_SUBMODE_1_POS))

/* 0x718 : pwr_wifi_tbl_3 */
#define RF_PWR_WIFI_TBL_3_OFFSET                                (0x718)
#define RF_WIFI_DPD_SEL_1                                       RF_WIFI_DPD_SEL_1
#define RF_WIFI_DPD_SEL_1_POS                                   (0U)
#define RF_WIFI_DPD_SEL_1_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_1_MSK                                   (((1U<<RF_WIFI_DPD_SEL_1_LEN)-1)<<RF_WIFI_DPD_SEL_1_POS)
#define RF_WIFI_DPD_SEL_1_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_1_LEN)-1)<<RF_WIFI_DPD_SEL_1_POS))
#define RF_WIFI_TOSDAC_Q_1                                      RF_WIFI_TOSDAC_Q_1
#define RF_WIFI_TOSDAC_Q_1_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_1_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_1_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_1_LEN)-1)<<RF_WIFI_TOSDAC_Q_1_POS)
#define RF_WIFI_TOSDAC_Q_1_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_1_LEN)-1)<<RF_WIFI_TOSDAC_Q_1_POS))
#define RF_WIFI_TOSDAC_I_1                                      RF_WIFI_TOSDAC_I_1
#define RF_WIFI_TOSDAC_I_1_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_1_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_1_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_1_LEN)-1)<<RF_WIFI_TOSDAC_I_1_POS)
#define RF_WIFI_TOSDAC_I_1_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_1_LEN)-1)<<RF_WIFI_TOSDAC_I_1_POS))
#define RF_WIFI_IQC_PHASE_1                                     RF_WIFI_IQC_PHASE_1
#define RF_WIFI_IQC_PHASE_1_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_1_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_1_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_1_LEN)-1)<<RF_WIFI_IQC_PHASE_1_POS)
#define RF_WIFI_IQC_PHASE_1_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_1_LEN)-1)<<RF_WIFI_IQC_PHASE_1_POS))
#define RF_WIFI_GC_PA_1                                         RF_WIFI_GC_PA_1
#define RF_WIFI_GC_PA_1_POS                                     (24U)
#define RF_WIFI_GC_PA_1_LEN                                     (3U)
#define RF_WIFI_GC_PA_1_MSK                                     (((1U<<RF_WIFI_GC_PA_1_LEN)-1)<<RF_WIFI_GC_PA_1_POS)
#define RF_WIFI_GC_PA_1_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_1_LEN)-1)<<RF_WIFI_GC_PA_1_POS))
#define RF_WIFI_PA_LP_EN_1                                      RF_WIFI_PA_LP_EN_1
#define RF_WIFI_PA_LP_EN_1_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_1_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_1_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_1_LEN)-1)<<RF_WIFI_PA_LP_EN_1_POS)
#define RF_WIFI_PA_LP_EN_1_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_1_LEN)-1)<<RF_WIFI_PA_LP_EN_1_POS))
#define RF_WIFI_DPD_EN_B_1                                      RF_WIFI_DPD_EN_B_1
#define RF_WIFI_DPD_EN_B_1_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_1_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_1_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_1_LEN)-1)<<RF_WIFI_DPD_EN_B_1_POS)
#define RF_WIFI_DPD_EN_B_1_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_1_LEN)-1)<<RF_WIFI_DPD_EN_B_1_POS))
#define RF_WIFI_DPD_EN_G_1                                      RF_WIFI_DPD_EN_G_1
#define RF_WIFI_DPD_EN_G_1_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_1_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_1_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_1_LEN)-1)<<RF_WIFI_DPD_EN_G_1_POS)
#define RF_WIFI_DPD_EN_G_1_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_1_LEN)-1)<<RF_WIFI_DPD_EN_G_1_POS))
#define RF_WIFI_DPD_EN_N_1                                      RF_WIFI_DPD_EN_N_1
#define RF_WIFI_DPD_EN_N_1_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_1_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_1_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_1_LEN)-1)<<RF_WIFI_DPD_EN_N_1_POS)
#define RF_WIFI_DPD_EN_N_1_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_1_LEN)-1)<<RF_WIFI_DPD_EN_N_1_POS))
#define RF_WIFI_DPD_EN_AX_1                                     RF_WIFI_DPD_EN_AX_1
#define RF_WIFI_DPD_EN_AX_1_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_1_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_1_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_1_LEN)-1)<<RF_WIFI_DPD_EN_AX_1_POS)
#define RF_WIFI_DPD_EN_AX_1_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_1_LEN)-1)<<RF_WIFI_DPD_EN_AX_1_POS))

/* 0x71C : pwr_wifi_tbl_4 */
#define RF_PWR_WIFI_TBL_4_OFFSET                                (0x71C)
#define RF_WIFI_IQC_GAIN_2                                      RF_WIFI_IQC_GAIN_2
#define RF_WIFI_IQC_GAIN_2_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_2_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_2_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_2_LEN)-1)<<RF_WIFI_IQC_GAIN_2_POS)
#define RF_WIFI_IQC_GAIN_2_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_2_LEN)-1)<<RF_WIFI_IQC_GAIN_2_POS))
#define RF_WIFI_DG1_QDB_2                                       RF_WIFI_DG1_QDB_2
#define RF_WIFI_DG1_QDB_2_POS                                   (11U)
#define RF_WIFI_DG1_QDB_2_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_2_MSK                                   (((1U<<RF_WIFI_DG1_QDB_2_LEN)-1)<<RF_WIFI_DG1_QDB_2_POS)
#define RF_WIFI_DG1_QDB_2_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_2_LEN)-1)<<RF_WIFI_DG1_QDB_2_POS))
#define RF_WIFI_GC_TMX_2                                        RF_WIFI_GC_TMX_2
#define RF_WIFI_GC_TMX_2_POS                                    (18U)
#define RF_WIFI_GC_TMX_2_LEN                                    (3U)
#define RF_WIFI_GC_TMX_2_MSK                                    (((1U<<RF_WIFI_GC_TMX_2_LEN)-1)<<RF_WIFI_GC_TMX_2_POS)
#define RF_WIFI_GC_TMX_2_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_2_LEN)-1)<<RF_WIFI_GC_TMX_2_POS))
#define RF_WIFI_GC_TBB_2                                        RF_WIFI_GC_TBB_2
#define RF_WIFI_GC_TBB_2_POS                                    (21U)
#define RF_WIFI_GC_TBB_2_LEN                                    (5U)
#define RF_WIFI_GC_TBB_2_MSK                                    (((1U<<RF_WIFI_GC_TBB_2_LEN)-1)<<RF_WIFI_GC_TBB_2_POS)
#define RF_WIFI_GC_TBB_2_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_2_LEN)-1)<<RF_WIFI_GC_TBB_2_POS))
#define RF_WIFI_GC_TBB_BOOST_2                                  RF_WIFI_GC_TBB_BOOST_2
#define RF_WIFI_GC_TBB_BOOST_2_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_2_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_2_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_2_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_2_POS)
#define RF_WIFI_GC_TBB_BOOST_2_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_2_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_2_POS))
#define RF_WIFI_DAC_BIAS_2                                      RF_WIFI_DAC_BIAS_2
#define RF_WIFI_DAC_BIAS_2_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_2_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_2_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_2_LEN)-1)<<RF_WIFI_DAC_BIAS_2_POS)
#define RF_WIFI_DAC_BIAS_2_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_2_LEN)-1)<<RF_WIFI_DAC_BIAS_2_POS))
#define RF_WIFI_AX_SUBMODE_2                                    RF_WIFI_AX_SUBMODE_2
#define RF_WIFI_AX_SUBMODE_2_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_2_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_2_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_2_LEN)-1)<<RF_WIFI_AX_SUBMODE_2_POS)
#define RF_WIFI_AX_SUBMODE_2_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_2_LEN)-1)<<RF_WIFI_AX_SUBMODE_2_POS))

/* 0x720 : pwr_wifi_tbl_5 */
#define RF_PWR_WIFI_TBL_5_OFFSET                                (0x720)
#define RF_WIFI_DPD_SEL_2                                       RF_WIFI_DPD_SEL_2
#define RF_WIFI_DPD_SEL_2_POS                                   (0U)
#define RF_WIFI_DPD_SEL_2_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_2_MSK                                   (((1U<<RF_WIFI_DPD_SEL_2_LEN)-1)<<RF_WIFI_DPD_SEL_2_POS)
#define RF_WIFI_DPD_SEL_2_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_2_LEN)-1)<<RF_WIFI_DPD_SEL_2_POS))
#define RF_WIFI_TOSDAC_Q_2                                      RF_WIFI_TOSDAC_Q_2
#define RF_WIFI_TOSDAC_Q_2_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_2_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_2_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_2_LEN)-1)<<RF_WIFI_TOSDAC_Q_2_POS)
#define RF_WIFI_TOSDAC_Q_2_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_2_LEN)-1)<<RF_WIFI_TOSDAC_Q_2_POS))
#define RF_WIFI_TOSDAC_I_2                                      RF_WIFI_TOSDAC_I_2
#define RF_WIFI_TOSDAC_I_2_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_2_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_2_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_2_LEN)-1)<<RF_WIFI_TOSDAC_I_2_POS)
#define RF_WIFI_TOSDAC_I_2_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_2_LEN)-1)<<RF_WIFI_TOSDAC_I_2_POS))
#define RF_WIFI_IQC_PHASE_2                                     RF_WIFI_IQC_PHASE_2
#define RF_WIFI_IQC_PHASE_2_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_2_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_2_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_2_LEN)-1)<<RF_WIFI_IQC_PHASE_2_POS)
#define RF_WIFI_IQC_PHASE_2_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_2_LEN)-1)<<RF_WIFI_IQC_PHASE_2_POS))
#define RF_WIFI_GC_PA_2                                         RF_WIFI_GC_PA_2
#define RF_WIFI_GC_PA_2_POS                                     (24U)
#define RF_WIFI_GC_PA_2_LEN                                     (3U)
#define RF_WIFI_GC_PA_2_MSK                                     (((1U<<RF_WIFI_GC_PA_2_LEN)-1)<<RF_WIFI_GC_PA_2_POS)
#define RF_WIFI_GC_PA_2_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_2_LEN)-1)<<RF_WIFI_GC_PA_2_POS))
#define RF_WIFI_PA_LP_EN_2                                      RF_WIFI_PA_LP_EN_2
#define RF_WIFI_PA_LP_EN_2_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_2_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_2_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_2_LEN)-1)<<RF_WIFI_PA_LP_EN_2_POS)
#define RF_WIFI_PA_LP_EN_2_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_2_LEN)-1)<<RF_WIFI_PA_LP_EN_2_POS))
#define RF_WIFI_DPD_EN_B_2                                      RF_WIFI_DPD_EN_B_2
#define RF_WIFI_DPD_EN_B_2_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_2_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_2_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_2_LEN)-1)<<RF_WIFI_DPD_EN_B_2_POS)
#define RF_WIFI_DPD_EN_B_2_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_2_LEN)-1)<<RF_WIFI_DPD_EN_B_2_POS))
#define RF_WIFI_DPD_EN_G_2                                      RF_WIFI_DPD_EN_G_2
#define RF_WIFI_DPD_EN_G_2_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_2_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_2_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_2_LEN)-1)<<RF_WIFI_DPD_EN_G_2_POS)
#define RF_WIFI_DPD_EN_G_2_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_2_LEN)-1)<<RF_WIFI_DPD_EN_G_2_POS))
#define RF_WIFI_DPD_EN_N_2                                      RF_WIFI_DPD_EN_N_2
#define RF_WIFI_DPD_EN_N_2_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_2_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_2_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_2_LEN)-1)<<RF_WIFI_DPD_EN_N_2_POS)
#define RF_WIFI_DPD_EN_N_2_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_2_LEN)-1)<<RF_WIFI_DPD_EN_N_2_POS))
#define RF_WIFI_DPD_EN_AX_2                                     RF_WIFI_DPD_EN_AX_2
#define RF_WIFI_DPD_EN_AX_2_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_2_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_2_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_2_LEN)-1)<<RF_WIFI_DPD_EN_AX_2_POS)
#define RF_WIFI_DPD_EN_AX_2_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_2_LEN)-1)<<RF_WIFI_DPD_EN_AX_2_POS))

/* 0x724 : pwr_wifi_tbl_6 */
#define RF_PWR_WIFI_TBL_6_OFFSET                                (0x724)
#define RF_WIFI_IQC_GAIN_3                                      RF_WIFI_IQC_GAIN_3
#define RF_WIFI_IQC_GAIN_3_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_3_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_3_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_3_LEN)-1)<<RF_WIFI_IQC_GAIN_3_POS)
#define RF_WIFI_IQC_GAIN_3_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_3_LEN)-1)<<RF_WIFI_IQC_GAIN_3_POS))
#define RF_WIFI_DG1_QDB_3                                       RF_WIFI_DG1_QDB_3
#define RF_WIFI_DG1_QDB_3_POS                                   (11U)
#define RF_WIFI_DG1_QDB_3_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_3_MSK                                   (((1U<<RF_WIFI_DG1_QDB_3_LEN)-1)<<RF_WIFI_DG1_QDB_3_POS)
#define RF_WIFI_DG1_QDB_3_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_3_LEN)-1)<<RF_WIFI_DG1_QDB_3_POS))
#define RF_WIFI_GC_TMX_3                                        RF_WIFI_GC_TMX_3
#define RF_WIFI_GC_TMX_3_POS                                    (18U)
#define RF_WIFI_GC_TMX_3_LEN                                    (3U)
#define RF_WIFI_GC_TMX_3_MSK                                    (((1U<<RF_WIFI_GC_TMX_3_LEN)-1)<<RF_WIFI_GC_TMX_3_POS)
#define RF_WIFI_GC_TMX_3_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_3_LEN)-1)<<RF_WIFI_GC_TMX_3_POS))
#define RF_WIFI_GC_TBB_3                                        RF_WIFI_GC_TBB_3
#define RF_WIFI_GC_TBB_3_POS                                    (21U)
#define RF_WIFI_GC_TBB_3_LEN                                    (5U)
#define RF_WIFI_GC_TBB_3_MSK                                    (((1U<<RF_WIFI_GC_TBB_3_LEN)-1)<<RF_WIFI_GC_TBB_3_POS)
#define RF_WIFI_GC_TBB_3_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_3_LEN)-1)<<RF_WIFI_GC_TBB_3_POS))
#define RF_WIFI_GC_TBB_BOOST_3                                  RF_WIFI_GC_TBB_BOOST_3
#define RF_WIFI_GC_TBB_BOOST_3_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_3_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_3_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_3_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_3_POS)
#define RF_WIFI_GC_TBB_BOOST_3_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_3_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_3_POS))
#define RF_WIFI_DAC_BIAS_3                                      RF_WIFI_DAC_BIAS_3
#define RF_WIFI_DAC_BIAS_3_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_3_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_3_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_3_LEN)-1)<<RF_WIFI_DAC_BIAS_3_POS)
#define RF_WIFI_DAC_BIAS_3_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_3_LEN)-1)<<RF_WIFI_DAC_BIAS_3_POS))
#define RF_WIFI_AX_SUBMODE_3                                    RF_WIFI_AX_SUBMODE_3
#define RF_WIFI_AX_SUBMODE_3_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_3_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_3_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_3_LEN)-1)<<RF_WIFI_AX_SUBMODE_3_POS)
#define RF_WIFI_AX_SUBMODE_3_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_3_LEN)-1)<<RF_WIFI_AX_SUBMODE_3_POS))

/* 0x728 : pwr_wifi_tbl_7 */
#define RF_PWR_WIFI_TBL_7_OFFSET                                (0x728)
#define RF_WIFI_DPD_SEL_3                                       RF_WIFI_DPD_SEL_3
#define RF_WIFI_DPD_SEL_3_POS                                   (0U)
#define RF_WIFI_DPD_SEL_3_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_3_MSK                                   (((1U<<RF_WIFI_DPD_SEL_3_LEN)-1)<<RF_WIFI_DPD_SEL_3_POS)
#define RF_WIFI_DPD_SEL_3_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_3_LEN)-1)<<RF_WIFI_DPD_SEL_3_POS))
#define RF_WIFI_TOSDAC_Q_3                                      RF_WIFI_TOSDAC_Q_3
#define RF_WIFI_TOSDAC_Q_3_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_3_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_3_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_3_LEN)-1)<<RF_WIFI_TOSDAC_Q_3_POS)
#define RF_WIFI_TOSDAC_Q_3_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_3_LEN)-1)<<RF_WIFI_TOSDAC_Q_3_POS))
#define RF_WIFI_TOSDAC_I_3                                      RF_WIFI_TOSDAC_I_3
#define RF_WIFI_TOSDAC_I_3_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_3_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_3_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_3_LEN)-1)<<RF_WIFI_TOSDAC_I_3_POS)
#define RF_WIFI_TOSDAC_I_3_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_3_LEN)-1)<<RF_WIFI_TOSDAC_I_3_POS))
#define RF_WIFI_IQC_PHASE_3                                     RF_WIFI_IQC_PHASE_3
#define RF_WIFI_IQC_PHASE_3_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_3_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_3_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_3_LEN)-1)<<RF_WIFI_IQC_PHASE_3_POS)
#define RF_WIFI_IQC_PHASE_3_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_3_LEN)-1)<<RF_WIFI_IQC_PHASE_3_POS))
#define RF_WIFI_GC_PA_3                                         RF_WIFI_GC_PA_3
#define RF_WIFI_GC_PA_3_POS                                     (24U)
#define RF_WIFI_GC_PA_3_LEN                                     (3U)
#define RF_WIFI_GC_PA_3_MSK                                     (((1U<<RF_WIFI_GC_PA_3_LEN)-1)<<RF_WIFI_GC_PA_3_POS)
#define RF_WIFI_GC_PA_3_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_3_LEN)-1)<<RF_WIFI_GC_PA_3_POS))
#define RF_WIFI_PA_LP_EN_3                                      RF_WIFI_PA_LP_EN_3
#define RF_WIFI_PA_LP_EN_3_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_3_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_3_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_3_LEN)-1)<<RF_WIFI_PA_LP_EN_3_POS)
#define RF_WIFI_PA_LP_EN_3_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_3_LEN)-1)<<RF_WIFI_PA_LP_EN_3_POS))
#define RF_WIFI_DPD_EN_B_3                                      RF_WIFI_DPD_EN_B_3
#define RF_WIFI_DPD_EN_B_3_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_3_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_3_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_3_LEN)-1)<<RF_WIFI_DPD_EN_B_3_POS)
#define RF_WIFI_DPD_EN_B_3_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_3_LEN)-1)<<RF_WIFI_DPD_EN_B_3_POS))
#define RF_WIFI_DPD_EN_G_3                                      RF_WIFI_DPD_EN_G_3
#define RF_WIFI_DPD_EN_G_3_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_3_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_3_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_3_LEN)-1)<<RF_WIFI_DPD_EN_G_3_POS)
#define RF_WIFI_DPD_EN_G_3_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_3_LEN)-1)<<RF_WIFI_DPD_EN_G_3_POS))
#define RF_WIFI_DPD_EN_N_3                                      RF_WIFI_DPD_EN_N_3
#define RF_WIFI_DPD_EN_N_3_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_3_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_3_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_3_LEN)-1)<<RF_WIFI_DPD_EN_N_3_POS)
#define RF_WIFI_DPD_EN_N_3_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_3_LEN)-1)<<RF_WIFI_DPD_EN_N_3_POS))
#define RF_WIFI_DPD_EN_AX_3                                     RF_WIFI_DPD_EN_AX_3
#define RF_WIFI_DPD_EN_AX_3_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_3_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_3_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_3_LEN)-1)<<RF_WIFI_DPD_EN_AX_3_POS)
#define RF_WIFI_DPD_EN_AX_3_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_3_LEN)-1)<<RF_WIFI_DPD_EN_AX_3_POS))

/* 0x72C : pwr_wifi_tbl_8 */
#define RF_PWR_WIFI_TBL_8_OFFSET                                (0x72C)
#define RF_WIFI_IQC_GAIN_4                                      RF_WIFI_IQC_GAIN_4
#define RF_WIFI_IQC_GAIN_4_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_4_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_4_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_4_LEN)-1)<<RF_WIFI_IQC_GAIN_4_POS)
#define RF_WIFI_IQC_GAIN_4_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_4_LEN)-1)<<RF_WIFI_IQC_GAIN_4_POS))
#define RF_WIFI_DG1_QDB_4                                       RF_WIFI_DG1_QDB_4
#define RF_WIFI_DG1_QDB_4_POS                                   (11U)
#define RF_WIFI_DG1_QDB_4_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_4_MSK                                   (((1U<<RF_WIFI_DG1_QDB_4_LEN)-1)<<RF_WIFI_DG1_QDB_4_POS)
#define RF_WIFI_DG1_QDB_4_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_4_LEN)-1)<<RF_WIFI_DG1_QDB_4_POS))
#define RF_WIFI_GC_TMX_4                                        RF_WIFI_GC_TMX_4
#define RF_WIFI_GC_TMX_4_POS                                    (18U)
#define RF_WIFI_GC_TMX_4_LEN                                    (3U)
#define RF_WIFI_GC_TMX_4_MSK                                    (((1U<<RF_WIFI_GC_TMX_4_LEN)-1)<<RF_WIFI_GC_TMX_4_POS)
#define RF_WIFI_GC_TMX_4_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_4_LEN)-1)<<RF_WIFI_GC_TMX_4_POS))
#define RF_WIFI_GC_TBB_4                                        RF_WIFI_GC_TBB_4
#define RF_WIFI_GC_TBB_4_POS                                    (21U)
#define RF_WIFI_GC_TBB_4_LEN                                    (5U)
#define RF_WIFI_GC_TBB_4_MSK                                    (((1U<<RF_WIFI_GC_TBB_4_LEN)-1)<<RF_WIFI_GC_TBB_4_POS)
#define RF_WIFI_GC_TBB_4_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_4_LEN)-1)<<RF_WIFI_GC_TBB_4_POS))
#define RF_WIFI_GC_TBB_BOOST_4                                  RF_WIFI_GC_TBB_BOOST_4
#define RF_WIFI_GC_TBB_BOOST_4_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_4_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_4_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_4_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_4_POS)
#define RF_WIFI_GC_TBB_BOOST_4_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_4_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_4_POS))
#define RF_WIFI_DAC_BIAS_4                                      RF_WIFI_DAC_BIAS_4
#define RF_WIFI_DAC_BIAS_4_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_4_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_4_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_4_LEN)-1)<<RF_WIFI_DAC_BIAS_4_POS)
#define RF_WIFI_DAC_BIAS_4_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_4_LEN)-1)<<RF_WIFI_DAC_BIAS_4_POS))
#define RF_WIFI_AX_SUBMODE_4                                    RF_WIFI_AX_SUBMODE_4
#define RF_WIFI_AX_SUBMODE_4_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_4_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_4_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_4_LEN)-1)<<RF_WIFI_AX_SUBMODE_4_POS)
#define RF_WIFI_AX_SUBMODE_4_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_4_LEN)-1)<<RF_WIFI_AX_SUBMODE_4_POS))

/* 0x730 : pwr_wifi_tbl_9 */
#define RF_PWR_WIFI_TBL_9_OFFSET                                (0x730)
#define RF_WIFI_DPD_SEL_4                                       RF_WIFI_DPD_SEL_4
#define RF_WIFI_DPD_SEL_4_POS                                   (0U)
#define RF_WIFI_DPD_SEL_4_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_4_MSK                                   (((1U<<RF_WIFI_DPD_SEL_4_LEN)-1)<<RF_WIFI_DPD_SEL_4_POS)
#define RF_WIFI_DPD_SEL_4_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_4_LEN)-1)<<RF_WIFI_DPD_SEL_4_POS))
#define RF_WIFI_TOSDAC_Q_4                                      RF_WIFI_TOSDAC_Q_4
#define RF_WIFI_TOSDAC_Q_4_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_4_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_4_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_4_LEN)-1)<<RF_WIFI_TOSDAC_Q_4_POS)
#define RF_WIFI_TOSDAC_Q_4_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_4_LEN)-1)<<RF_WIFI_TOSDAC_Q_4_POS))
#define RF_WIFI_TOSDAC_I_4                                      RF_WIFI_TOSDAC_I_4
#define RF_WIFI_TOSDAC_I_4_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_4_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_4_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_4_LEN)-1)<<RF_WIFI_TOSDAC_I_4_POS)
#define RF_WIFI_TOSDAC_I_4_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_4_LEN)-1)<<RF_WIFI_TOSDAC_I_4_POS))
#define RF_WIFI_IQC_PHASE_4                                     RF_WIFI_IQC_PHASE_4
#define RF_WIFI_IQC_PHASE_4_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_4_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_4_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_4_LEN)-1)<<RF_WIFI_IQC_PHASE_4_POS)
#define RF_WIFI_IQC_PHASE_4_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_4_LEN)-1)<<RF_WIFI_IQC_PHASE_4_POS))
#define RF_WIFI_GC_PA_4                                         RF_WIFI_GC_PA_4
#define RF_WIFI_GC_PA_4_POS                                     (24U)
#define RF_WIFI_GC_PA_4_LEN                                     (3U)
#define RF_WIFI_GC_PA_4_MSK                                     (((1U<<RF_WIFI_GC_PA_4_LEN)-1)<<RF_WIFI_GC_PA_4_POS)
#define RF_WIFI_GC_PA_4_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_4_LEN)-1)<<RF_WIFI_GC_PA_4_POS))
#define RF_WIFI_PA_LP_EN_4                                      RF_WIFI_PA_LP_EN_4
#define RF_WIFI_PA_LP_EN_4_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_4_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_4_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_4_LEN)-1)<<RF_WIFI_PA_LP_EN_4_POS)
#define RF_WIFI_PA_LP_EN_4_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_4_LEN)-1)<<RF_WIFI_PA_LP_EN_4_POS))
#define RF_WIFI_DPD_EN_B_4                                      RF_WIFI_DPD_EN_B_4
#define RF_WIFI_DPD_EN_B_4_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_4_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_4_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_4_LEN)-1)<<RF_WIFI_DPD_EN_B_4_POS)
#define RF_WIFI_DPD_EN_B_4_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_4_LEN)-1)<<RF_WIFI_DPD_EN_B_4_POS))
#define RF_WIFI_DPD_EN_G_4                                      RF_WIFI_DPD_EN_G_4
#define RF_WIFI_DPD_EN_G_4_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_4_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_4_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_4_LEN)-1)<<RF_WIFI_DPD_EN_G_4_POS)
#define RF_WIFI_DPD_EN_G_4_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_4_LEN)-1)<<RF_WIFI_DPD_EN_G_4_POS))
#define RF_WIFI_DPD_EN_N_4                                      RF_WIFI_DPD_EN_N_4
#define RF_WIFI_DPD_EN_N_4_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_4_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_4_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_4_LEN)-1)<<RF_WIFI_DPD_EN_N_4_POS)
#define RF_WIFI_DPD_EN_N_4_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_4_LEN)-1)<<RF_WIFI_DPD_EN_N_4_POS))
#define RF_WIFI_DPD_EN_AX_4                                     RF_WIFI_DPD_EN_AX_4
#define RF_WIFI_DPD_EN_AX_4_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_4_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_4_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_4_LEN)-1)<<RF_WIFI_DPD_EN_AX_4_POS)
#define RF_WIFI_DPD_EN_AX_4_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_4_LEN)-1)<<RF_WIFI_DPD_EN_AX_4_POS))

/* 0x734 : pwr_wifi_tbl_10 */
#define RF_PWR_WIFI_TBL_10_OFFSET                               (0x734)
#define RF_WIFI_IQC_GAIN_5                                      RF_WIFI_IQC_GAIN_5
#define RF_WIFI_IQC_GAIN_5_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_5_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_5_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_5_LEN)-1)<<RF_WIFI_IQC_GAIN_5_POS)
#define RF_WIFI_IQC_GAIN_5_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_5_LEN)-1)<<RF_WIFI_IQC_GAIN_5_POS))
#define RF_WIFI_DG1_QDB_5                                       RF_WIFI_DG1_QDB_5
#define RF_WIFI_DG1_QDB_5_POS                                   (11U)
#define RF_WIFI_DG1_QDB_5_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_5_MSK                                   (((1U<<RF_WIFI_DG1_QDB_5_LEN)-1)<<RF_WIFI_DG1_QDB_5_POS)
#define RF_WIFI_DG1_QDB_5_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_5_LEN)-1)<<RF_WIFI_DG1_QDB_5_POS))
#define RF_WIFI_GC_TMX_5                                        RF_WIFI_GC_TMX_5
#define RF_WIFI_GC_TMX_5_POS                                    (18U)
#define RF_WIFI_GC_TMX_5_LEN                                    (3U)
#define RF_WIFI_GC_TMX_5_MSK                                    (((1U<<RF_WIFI_GC_TMX_5_LEN)-1)<<RF_WIFI_GC_TMX_5_POS)
#define RF_WIFI_GC_TMX_5_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_5_LEN)-1)<<RF_WIFI_GC_TMX_5_POS))
#define RF_WIFI_GC_TBB_5                                        RF_WIFI_GC_TBB_5
#define RF_WIFI_GC_TBB_5_POS                                    (21U)
#define RF_WIFI_GC_TBB_5_LEN                                    (5U)
#define RF_WIFI_GC_TBB_5_MSK                                    (((1U<<RF_WIFI_GC_TBB_5_LEN)-1)<<RF_WIFI_GC_TBB_5_POS)
#define RF_WIFI_GC_TBB_5_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_5_LEN)-1)<<RF_WIFI_GC_TBB_5_POS))
#define RF_WIFI_GC_TBB_BOOST_5                                  RF_WIFI_GC_TBB_BOOST_5
#define RF_WIFI_GC_TBB_BOOST_5_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_5_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_5_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_5_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_5_POS)
#define RF_WIFI_GC_TBB_BOOST_5_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_5_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_5_POS))
#define RF_WIFI_DAC_BIAS_5                                      RF_WIFI_DAC_BIAS_5
#define RF_WIFI_DAC_BIAS_5_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_5_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_5_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_5_LEN)-1)<<RF_WIFI_DAC_BIAS_5_POS)
#define RF_WIFI_DAC_BIAS_5_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_5_LEN)-1)<<RF_WIFI_DAC_BIAS_5_POS))
#define RF_WIFI_AX_SUBMODE_5                                    RF_WIFI_AX_SUBMODE_5
#define RF_WIFI_AX_SUBMODE_5_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_5_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_5_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_5_LEN)-1)<<RF_WIFI_AX_SUBMODE_5_POS)
#define RF_WIFI_AX_SUBMODE_5_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_5_LEN)-1)<<RF_WIFI_AX_SUBMODE_5_POS))

/* 0x738 : pwr_wifi_tbl_11 */
#define RF_PWR_WIFI_TBL_11_OFFSET                               (0x738)
#define RF_WIFI_DPD_SEL_5                                       RF_WIFI_DPD_SEL_5
#define RF_WIFI_DPD_SEL_5_POS                                   (0U)
#define RF_WIFI_DPD_SEL_5_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_5_MSK                                   (((1U<<RF_WIFI_DPD_SEL_5_LEN)-1)<<RF_WIFI_DPD_SEL_5_POS)
#define RF_WIFI_DPD_SEL_5_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_5_LEN)-1)<<RF_WIFI_DPD_SEL_5_POS))
#define RF_WIFI_TOSDAC_Q_5                                      RF_WIFI_TOSDAC_Q_5
#define RF_WIFI_TOSDAC_Q_5_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_5_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_5_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_5_LEN)-1)<<RF_WIFI_TOSDAC_Q_5_POS)
#define RF_WIFI_TOSDAC_Q_5_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_5_LEN)-1)<<RF_WIFI_TOSDAC_Q_5_POS))
#define RF_WIFI_TOSDAC_I_5                                      RF_WIFI_TOSDAC_I_5
#define RF_WIFI_TOSDAC_I_5_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_5_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_5_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_5_LEN)-1)<<RF_WIFI_TOSDAC_I_5_POS)
#define RF_WIFI_TOSDAC_I_5_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_5_LEN)-1)<<RF_WIFI_TOSDAC_I_5_POS))
#define RF_WIFI_IQC_PHASE_5                                     RF_WIFI_IQC_PHASE_5
#define RF_WIFI_IQC_PHASE_5_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_5_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_5_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_5_LEN)-1)<<RF_WIFI_IQC_PHASE_5_POS)
#define RF_WIFI_IQC_PHASE_5_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_5_LEN)-1)<<RF_WIFI_IQC_PHASE_5_POS))
#define RF_WIFI_GC_PA_5                                         RF_WIFI_GC_PA_5
#define RF_WIFI_GC_PA_5_POS                                     (24U)
#define RF_WIFI_GC_PA_5_LEN                                     (3U)
#define RF_WIFI_GC_PA_5_MSK                                     (((1U<<RF_WIFI_GC_PA_5_LEN)-1)<<RF_WIFI_GC_PA_5_POS)
#define RF_WIFI_GC_PA_5_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_5_LEN)-1)<<RF_WIFI_GC_PA_5_POS))
#define RF_WIFI_PA_LP_EN_5                                      RF_WIFI_PA_LP_EN_5
#define RF_WIFI_PA_LP_EN_5_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_5_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_5_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_5_LEN)-1)<<RF_WIFI_PA_LP_EN_5_POS)
#define RF_WIFI_PA_LP_EN_5_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_5_LEN)-1)<<RF_WIFI_PA_LP_EN_5_POS))
#define RF_WIFI_DPD_EN_B_5                                      RF_WIFI_DPD_EN_B_5
#define RF_WIFI_DPD_EN_B_5_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_5_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_5_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_5_LEN)-1)<<RF_WIFI_DPD_EN_B_5_POS)
#define RF_WIFI_DPD_EN_B_5_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_5_LEN)-1)<<RF_WIFI_DPD_EN_B_5_POS))
#define RF_WIFI_DPD_EN_G_5                                      RF_WIFI_DPD_EN_G_5
#define RF_WIFI_DPD_EN_G_5_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_5_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_5_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_5_LEN)-1)<<RF_WIFI_DPD_EN_G_5_POS)
#define RF_WIFI_DPD_EN_G_5_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_5_LEN)-1)<<RF_WIFI_DPD_EN_G_5_POS))
#define RF_WIFI_DPD_EN_N_5                                      RF_WIFI_DPD_EN_N_5
#define RF_WIFI_DPD_EN_N_5_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_5_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_5_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_5_LEN)-1)<<RF_WIFI_DPD_EN_N_5_POS)
#define RF_WIFI_DPD_EN_N_5_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_5_LEN)-1)<<RF_WIFI_DPD_EN_N_5_POS))
#define RF_WIFI_DPD_EN_AX_5                                     RF_WIFI_DPD_EN_AX_5
#define RF_WIFI_DPD_EN_AX_5_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_5_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_5_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_5_LEN)-1)<<RF_WIFI_DPD_EN_AX_5_POS)
#define RF_WIFI_DPD_EN_AX_5_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_5_LEN)-1)<<RF_WIFI_DPD_EN_AX_5_POS))

/* 0x73C : pwr_wifi_tbl_12 */
#define RF_PWR_WIFI_TBL_12_OFFSET                               (0x73C)
#define RF_WIFI_IQC_GAIN_6                                      RF_WIFI_IQC_GAIN_6
#define RF_WIFI_IQC_GAIN_6_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_6_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_6_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_6_LEN)-1)<<RF_WIFI_IQC_GAIN_6_POS)
#define RF_WIFI_IQC_GAIN_6_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_6_LEN)-1)<<RF_WIFI_IQC_GAIN_6_POS))
#define RF_WIFI_DG1_QDB_6                                       RF_WIFI_DG1_QDB_6
#define RF_WIFI_DG1_QDB_6_POS                                   (11U)
#define RF_WIFI_DG1_QDB_6_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_6_MSK                                   (((1U<<RF_WIFI_DG1_QDB_6_LEN)-1)<<RF_WIFI_DG1_QDB_6_POS)
#define RF_WIFI_DG1_QDB_6_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_6_LEN)-1)<<RF_WIFI_DG1_QDB_6_POS))
#define RF_WIFI_GC_TMX_6                                        RF_WIFI_GC_TMX_6
#define RF_WIFI_GC_TMX_6_POS                                    (18U)
#define RF_WIFI_GC_TMX_6_LEN                                    (3U)
#define RF_WIFI_GC_TMX_6_MSK                                    (((1U<<RF_WIFI_GC_TMX_6_LEN)-1)<<RF_WIFI_GC_TMX_6_POS)
#define RF_WIFI_GC_TMX_6_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_6_LEN)-1)<<RF_WIFI_GC_TMX_6_POS))
#define RF_WIFI_GC_TBB_6                                        RF_WIFI_GC_TBB_6
#define RF_WIFI_GC_TBB_6_POS                                    (21U)
#define RF_WIFI_GC_TBB_6_LEN                                    (5U)
#define RF_WIFI_GC_TBB_6_MSK                                    (((1U<<RF_WIFI_GC_TBB_6_LEN)-1)<<RF_WIFI_GC_TBB_6_POS)
#define RF_WIFI_GC_TBB_6_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_6_LEN)-1)<<RF_WIFI_GC_TBB_6_POS))
#define RF_WIFI_GC_TBB_BOOST_6                                  RF_WIFI_GC_TBB_BOOST_6
#define RF_WIFI_GC_TBB_BOOST_6_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_6_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_6_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_6_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_6_POS)
#define RF_WIFI_GC_TBB_BOOST_6_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_6_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_6_POS))
#define RF_WIFI_DAC_BIAS_6                                      RF_WIFI_DAC_BIAS_6
#define RF_WIFI_DAC_BIAS_6_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_6_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_6_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_6_LEN)-1)<<RF_WIFI_DAC_BIAS_6_POS)
#define RF_WIFI_DAC_BIAS_6_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_6_LEN)-1)<<RF_WIFI_DAC_BIAS_6_POS))
#define RF_WIFI_AX_SUBMODE_6                                    RF_WIFI_AX_SUBMODE_6
#define RF_WIFI_AX_SUBMODE_6_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_6_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_6_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_6_LEN)-1)<<RF_WIFI_AX_SUBMODE_6_POS)
#define RF_WIFI_AX_SUBMODE_6_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_6_LEN)-1)<<RF_WIFI_AX_SUBMODE_6_POS))

/* 0x740 : pwr_wifi_tbl_13 */
#define RF_PWR_WIFI_TBL_13_OFFSET                               (0x740)
#define RF_WIFI_DPD_SEL_6                                       RF_WIFI_DPD_SEL_6
#define RF_WIFI_DPD_SEL_6_POS                                   (0U)
#define RF_WIFI_DPD_SEL_6_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_6_MSK                                   (((1U<<RF_WIFI_DPD_SEL_6_LEN)-1)<<RF_WIFI_DPD_SEL_6_POS)
#define RF_WIFI_DPD_SEL_6_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_6_LEN)-1)<<RF_WIFI_DPD_SEL_6_POS))
#define RF_WIFI_TOSDAC_Q_6                                      RF_WIFI_TOSDAC_Q_6
#define RF_WIFI_TOSDAC_Q_6_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_6_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_6_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_6_LEN)-1)<<RF_WIFI_TOSDAC_Q_6_POS)
#define RF_WIFI_TOSDAC_Q_6_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_6_LEN)-1)<<RF_WIFI_TOSDAC_Q_6_POS))
#define RF_WIFI_TOSDAC_I_6                                      RF_WIFI_TOSDAC_I_6
#define RF_WIFI_TOSDAC_I_6_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_6_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_6_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_6_LEN)-1)<<RF_WIFI_TOSDAC_I_6_POS)
#define RF_WIFI_TOSDAC_I_6_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_6_LEN)-1)<<RF_WIFI_TOSDAC_I_6_POS))
#define RF_WIFI_IQC_PHASE_6                                     RF_WIFI_IQC_PHASE_6
#define RF_WIFI_IQC_PHASE_6_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_6_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_6_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_6_LEN)-1)<<RF_WIFI_IQC_PHASE_6_POS)
#define RF_WIFI_IQC_PHASE_6_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_6_LEN)-1)<<RF_WIFI_IQC_PHASE_6_POS))
#define RF_WIFI_GC_PA_6                                         RF_WIFI_GC_PA_6
#define RF_WIFI_GC_PA_6_POS                                     (24U)
#define RF_WIFI_GC_PA_6_LEN                                     (3U)
#define RF_WIFI_GC_PA_6_MSK                                     (((1U<<RF_WIFI_GC_PA_6_LEN)-1)<<RF_WIFI_GC_PA_6_POS)
#define RF_WIFI_GC_PA_6_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_6_LEN)-1)<<RF_WIFI_GC_PA_6_POS))
#define RF_WIFI_PA_LP_EN_6                                      RF_WIFI_PA_LP_EN_6
#define RF_WIFI_PA_LP_EN_6_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_6_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_6_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_6_LEN)-1)<<RF_WIFI_PA_LP_EN_6_POS)
#define RF_WIFI_PA_LP_EN_6_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_6_LEN)-1)<<RF_WIFI_PA_LP_EN_6_POS))
#define RF_WIFI_DPD_EN_B_6                                      RF_WIFI_DPD_EN_B_6
#define RF_WIFI_DPD_EN_B_6_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_6_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_6_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_6_LEN)-1)<<RF_WIFI_DPD_EN_B_6_POS)
#define RF_WIFI_DPD_EN_B_6_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_6_LEN)-1)<<RF_WIFI_DPD_EN_B_6_POS))
#define RF_WIFI_DPD_EN_G_6                                      RF_WIFI_DPD_EN_G_6
#define RF_WIFI_DPD_EN_G_6_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_6_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_6_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_6_LEN)-1)<<RF_WIFI_DPD_EN_G_6_POS)
#define RF_WIFI_DPD_EN_G_6_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_6_LEN)-1)<<RF_WIFI_DPD_EN_G_6_POS))
#define RF_WIFI_DPD_EN_N_6                                      RF_WIFI_DPD_EN_N_6
#define RF_WIFI_DPD_EN_N_6_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_6_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_6_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_6_LEN)-1)<<RF_WIFI_DPD_EN_N_6_POS)
#define RF_WIFI_DPD_EN_N_6_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_6_LEN)-1)<<RF_WIFI_DPD_EN_N_6_POS))
#define RF_WIFI_DPD_EN_AX_6                                     RF_WIFI_DPD_EN_AX_6
#define RF_WIFI_DPD_EN_AX_6_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_6_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_6_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_6_LEN)-1)<<RF_WIFI_DPD_EN_AX_6_POS)
#define RF_WIFI_DPD_EN_AX_6_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_6_LEN)-1)<<RF_WIFI_DPD_EN_AX_6_POS))

/* 0x744 : pwr_wifi_tbl_14 */
#define RF_PWR_WIFI_TBL_14_OFFSET                               (0x744)
#define RF_WIFI_IQC_GAIN_7                                      RF_WIFI_IQC_GAIN_7
#define RF_WIFI_IQC_GAIN_7_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_7_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_7_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_7_LEN)-1)<<RF_WIFI_IQC_GAIN_7_POS)
#define RF_WIFI_IQC_GAIN_7_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_7_LEN)-1)<<RF_WIFI_IQC_GAIN_7_POS))
#define RF_WIFI_DG1_QDB_7                                       RF_WIFI_DG1_QDB_7
#define RF_WIFI_DG1_QDB_7_POS                                   (11U)
#define RF_WIFI_DG1_QDB_7_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_7_MSK                                   (((1U<<RF_WIFI_DG1_QDB_7_LEN)-1)<<RF_WIFI_DG1_QDB_7_POS)
#define RF_WIFI_DG1_QDB_7_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_7_LEN)-1)<<RF_WIFI_DG1_QDB_7_POS))
#define RF_WIFI_GC_TMX_7                                        RF_WIFI_GC_TMX_7
#define RF_WIFI_GC_TMX_7_POS                                    (18U)
#define RF_WIFI_GC_TMX_7_LEN                                    (3U)
#define RF_WIFI_GC_TMX_7_MSK                                    (((1U<<RF_WIFI_GC_TMX_7_LEN)-1)<<RF_WIFI_GC_TMX_7_POS)
#define RF_WIFI_GC_TMX_7_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_7_LEN)-1)<<RF_WIFI_GC_TMX_7_POS))
#define RF_WIFI_GC_TBB_7                                        RF_WIFI_GC_TBB_7
#define RF_WIFI_GC_TBB_7_POS                                    (21U)
#define RF_WIFI_GC_TBB_7_LEN                                    (5U)
#define RF_WIFI_GC_TBB_7_MSK                                    (((1U<<RF_WIFI_GC_TBB_7_LEN)-1)<<RF_WIFI_GC_TBB_7_POS)
#define RF_WIFI_GC_TBB_7_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_7_LEN)-1)<<RF_WIFI_GC_TBB_7_POS))
#define RF_WIFI_GC_TBB_BOOST_7                                  RF_WIFI_GC_TBB_BOOST_7
#define RF_WIFI_GC_TBB_BOOST_7_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_7_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_7_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_7_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_7_POS)
#define RF_WIFI_GC_TBB_BOOST_7_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_7_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_7_POS))
#define RF_WIFI_DAC_BIAS_7                                      RF_WIFI_DAC_BIAS_7
#define RF_WIFI_DAC_BIAS_7_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_7_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_7_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_7_LEN)-1)<<RF_WIFI_DAC_BIAS_7_POS)
#define RF_WIFI_DAC_BIAS_7_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_7_LEN)-1)<<RF_WIFI_DAC_BIAS_7_POS))
#define RF_WIFI_AX_SUBMODE_7                                    RF_WIFI_AX_SUBMODE_7
#define RF_WIFI_AX_SUBMODE_7_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_7_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_7_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_7_LEN)-1)<<RF_WIFI_AX_SUBMODE_7_POS)
#define RF_WIFI_AX_SUBMODE_7_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_7_LEN)-1)<<RF_WIFI_AX_SUBMODE_7_POS))

/* 0x748 : pwr_wifi_tbl_15 */
#define RF_PWR_WIFI_TBL_15_OFFSET                               (0x748)
#define RF_WIFI_DPD_SEL_7                                       RF_WIFI_DPD_SEL_7
#define RF_WIFI_DPD_SEL_7_POS                                   (0U)
#define RF_WIFI_DPD_SEL_7_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_7_MSK                                   (((1U<<RF_WIFI_DPD_SEL_7_LEN)-1)<<RF_WIFI_DPD_SEL_7_POS)
#define RF_WIFI_DPD_SEL_7_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_7_LEN)-1)<<RF_WIFI_DPD_SEL_7_POS))
#define RF_WIFI_TOSDAC_Q_7                                      RF_WIFI_TOSDAC_Q_7
#define RF_WIFI_TOSDAC_Q_7_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_7_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_7_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_7_LEN)-1)<<RF_WIFI_TOSDAC_Q_7_POS)
#define RF_WIFI_TOSDAC_Q_7_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_7_LEN)-1)<<RF_WIFI_TOSDAC_Q_7_POS))
#define RF_WIFI_TOSDAC_I_7                                      RF_WIFI_TOSDAC_I_7
#define RF_WIFI_TOSDAC_I_7_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_7_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_7_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_7_LEN)-1)<<RF_WIFI_TOSDAC_I_7_POS)
#define RF_WIFI_TOSDAC_I_7_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_7_LEN)-1)<<RF_WIFI_TOSDAC_I_7_POS))
#define RF_WIFI_IQC_PHASE_7                                     RF_WIFI_IQC_PHASE_7
#define RF_WIFI_IQC_PHASE_7_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_7_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_7_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_7_LEN)-1)<<RF_WIFI_IQC_PHASE_7_POS)
#define RF_WIFI_IQC_PHASE_7_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_7_LEN)-1)<<RF_WIFI_IQC_PHASE_7_POS))
#define RF_WIFI_GC_PA_7                                         RF_WIFI_GC_PA_7
#define RF_WIFI_GC_PA_7_POS                                     (24U)
#define RF_WIFI_GC_PA_7_LEN                                     (3U)
#define RF_WIFI_GC_PA_7_MSK                                     (((1U<<RF_WIFI_GC_PA_7_LEN)-1)<<RF_WIFI_GC_PA_7_POS)
#define RF_WIFI_GC_PA_7_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_7_LEN)-1)<<RF_WIFI_GC_PA_7_POS))
#define RF_WIFI_PA_LP_EN_7                                      RF_WIFI_PA_LP_EN_7
#define RF_WIFI_PA_LP_EN_7_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_7_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_7_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_7_LEN)-1)<<RF_WIFI_PA_LP_EN_7_POS)
#define RF_WIFI_PA_LP_EN_7_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_7_LEN)-1)<<RF_WIFI_PA_LP_EN_7_POS))
#define RF_WIFI_DPD_EN_B_7                                      RF_WIFI_DPD_EN_B_7
#define RF_WIFI_DPD_EN_B_7_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_7_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_7_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_7_LEN)-1)<<RF_WIFI_DPD_EN_B_7_POS)
#define RF_WIFI_DPD_EN_B_7_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_7_LEN)-1)<<RF_WIFI_DPD_EN_B_7_POS))
#define RF_WIFI_DPD_EN_G_7                                      RF_WIFI_DPD_EN_G_7
#define RF_WIFI_DPD_EN_G_7_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_7_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_7_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_7_LEN)-1)<<RF_WIFI_DPD_EN_G_7_POS)
#define RF_WIFI_DPD_EN_G_7_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_7_LEN)-1)<<RF_WIFI_DPD_EN_G_7_POS))
#define RF_WIFI_DPD_EN_N_7                                      RF_WIFI_DPD_EN_N_7
#define RF_WIFI_DPD_EN_N_7_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_7_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_7_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_7_LEN)-1)<<RF_WIFI_DPD_EN_N_7_POS)
#define RF_WIFI_DPD_EN_N_7_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_7_LEN)-1)<<RF_WIFI_DPD_EN_N_7_POS))
#define RF_WIFI_DPD_EN_AX_7                                     RF_WIFI_DPD_EN_AX_7
#define RF_WIFI_DPD_EN_AX_7_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_7_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_7_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_7_LEN)-1)<<RF_WIFI_DPD_EN_AX_7_POS)
#define RF_WIFI_DPD_EN_AX_7_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_7_LEN)-1)<<RF_WIFI_DPD_EN_AX_7_POS))

/* 0x74C : pwr_wifi_tbl_16 */
#define RF_PWR_WIFI_TBL_16_OFFSET                               (0x74C)
#define RF_WIFI_IQC_GAIN_8                                      RF_WIFI_IQC_GAIN_8
#define RF_WIFI_IQC_GAIN_8_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_8_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_8_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_8_LEN)-1)<<RF_WIFI_IQC_GAIN_8_POS)
#define RF_WIFI_IQC_GAIN_8_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_8_LEN)-1)<<RF_WIFI_IQC_GAIN_8_POS))
#define RF_WIFI_DG1_QDB_8                                       RF_WIFI_DG1_QDB_8
#define RF_WIFI_DG1_QDB_8_POS                                   (11U)
#define RF_WIFI_DG1_QDB_8_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_8_MSK                                   (((1U<<RF_WIFI_DG1_QDB_8_LEN)-1)<<RF_WIFI_DG1_QDB_8_POS)
#define RF_WIFI_DG1_QDB_8_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_8_LEN)-1)<<RF_WIFI_DG1_QDB_8_POS))
#define RF_WIFI_GC_TMX_8                                        RF_WIFI_GC_TMX_8
#define RF_WIFI_GC_TMX_8_POS                                    (18U)
#define RF_WIFI_GC_TMX_8_LEN                                    (3U)
#define RF_WIFI_GC_TMX_8_MSK                                    (((1U<<RF_WIFI_GC_TMX_8_LEN)-1)<<RF_WIFI_GC_TMX_8_POS)
#define RF_WIFI_GC_TMX_8_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_8_LEN)-1)<<RF_WIFI_GC_TMX_8_POS))
#define RF_WIFI_GC_TBB_8                                        RF_WIFI_GC_TBB_8
#define RF_WIFI_GC_TBB_8_POS                                    (21U)
#define RF_WIFI_GC_TBB_8_LEN                                    (5U)
#define RF_WIFI_GC_TBB_8_MSK                                    (((1U<<RF_WIFI_GC_TBB_8_LEN)-1)<<RF_WIFI_GC_TBB_8_POS)
#define RF_WIFI_GC_TBB_8_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_8_LEN)-1)<<RF_WIFI_GC_TBB_8_POS))
#define RF_WIFI_GC_TBB_BOOST_8                                  RF_WIFI_GC_TBB_BOOST_8
#define RF_WIFI_GC_TBB_BOOST_8_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_8_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_8_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_8_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_8_POS)
#define RF_WIFI_GC_TBB_BOOST_8_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_8_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_8_POS))
#define RF_WIFI_DAC_BIAS_8                                      RF_WIFI_DAC_BIAS_8
#define RF_WIFI_DAC_BIAS_8_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_8_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_8_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_8_LEN)-1)<<RF_WIFI_DAC_BIAS_8_POS)
#define RF_WIFI_DAC_BIAS_8_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_8_LEN)-1)<<RF_WIFI_DAC_BIAS_8_POS))
#define RF_WIFI_AX_SUBMODE_8                                    RF_WIFI_AX_SUBMODE_8
#define RF_WIFI_AX_SUBMODE_8_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_8_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_8_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_8_LEN)-1)<<RF_WIFI_AX_SUBMODE_8_POS)
#define RF_WIFI_AX_SUBMODE_8_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_8_LEN)-1)<<RF_WIFI_AX_SUBMODE_8_POS))

/* 0x750 : pwr_wifi_tbl_17 */
#define RF_PWR_WIFI_TBL_17_OFFSET                               (0x750)
#define RF_WIFI_DPD_SEL_8                                       RF_WIFI_DPD_SEL_8
#define RF_WIFI_DPD_SEL_8_POS                                   (0U)
#define RF_WIFI_DPD_SEL_8_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_8_MSK                                   (((1U<<RF_WIFI_DPD_SEL_8_LEN)-1)<<RF_WIFI_DPD_SEL_8_POS)
#define RF_WIFI_DPD_SEL_8_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_8_LEN)-1)<<RF_WIFI_DPD_SEL_8_POS))
#define RF_WIFI_TOSDAC_Q_8                                      RF_WIFI_TOSDAC_Q_8
#define RF_WIFI_TOSDAC_Q_8_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_8_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_8_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_8_LEN)-1)<<RF_WIFI_TOSDAC_Q_8_POS)
#define RF_WIFI_TOSDAC_Q_8_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_8_LEN)-1)<<RF_WIFI_TOSDAC_Q_8_POS))
#define RF_WIFI_TOSDAC_I_8                                      RF_WIFI_TOSDAC_I_8
#define RF_WIFI_TOSDAC_I_8_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_8_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_8_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_8_LEN)-1)<<RF_WIFI_TOSDAC_I_8_POS)
#define RF_WIFI_TOSDAC_I_8_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_8_LEN)-1)<<RF_WIFI_TOSDAC_I_8_POS))
#define RF_WIFI_IQC_PHASE_8                                     RF_WIFI_IQC_PHASE_8
#define RF_WIFI_IQC_PHASE_8_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_8_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_8_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_8_LEN)-1)<<RF_WIFI_IQC_PHASE_8_POS)
#define RF_WIFI_IQC_PHASE_8_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_8_LEN)-1)<<RF_WIFI_IQC_PHASE_8_POS))
#define RF_WIFI_GC_PA_8                                         RF_WIFI_GC_PA_8
#define RF_WIFI_GC_PA_8_POS                                     (24U)
#define RF_WIFI_GC_PA_8_LEN                                     (3U)
#define RF_WIFI_GC_PA_8_MSK                                     (((1U<<RF_WIFI_GC_PA_8_LEN)-1)<<RF_WIFI_GC_PA_8_POS)
#define RF_WIFI_GC_PA_8_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_8_LEN)-1)<<RF_WIFI_GC_PA_8_POS))
#define RF_WIFI_PA_LP_EN_8                                      RF_WIFI_PA_LP_EN_8
#define RF_WIFI_PA_LP_EN_8_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_8_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_8_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_8_LEN)-1)<<RF_WIFI_PA_LP_EN_8_POS)
#define RF_WIFI_PA_LP_EN_8_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_8_LEN)-1)<<RF_WIFI_PA_LP_EN_8_POS))
#define RF_WIFI_DPD_EN_B_8                                      RF_WIFI_DPD_EN_B_8
#define RF_WIFI_DPD_EN_B_8_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_8_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_8_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_8_LEN)-1)<<RF_WIFI_DPD_EN_B_8_POS)
#define RF_WIFI_DPD_EN_B_8_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_8_LEN)-1)<<RF_WIFI_DPD_EN_B_8_POS))
#define RF_WIFI_DPD_EN_G_8                                      RF_WIFI_DPD_EN_G_8
#define RF_WIFI_DPD_EN_G_8_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_8_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_8_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_8_LEN)-1)<<RF_WIFI_DPD_EN_G_8_POS)
#define RF_WIFI_DPD_EN_G_8_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_8_LEN)-1)<<RF_WIFI_DPD_EN_G_8_POS))
#define RF_WIFI_DPD_EN_N_8                                      RF_WIFI_DPD_EN_N_8
#define RF_WIFI_DPD_EN_N_8_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_8_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_8_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_8_LEN)-1)<<RF_WIFI_DPD_EN_N_8_POS)
#define RF_WIFI_DPD_EN_N_8_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_8_LEN)-1)<<RF_WIFI_DPD_EN_N_8_POS))
#define RF_WIFI_DPD_EN_AX_8                                     RF_WIFI_DPD_EN_AX_8
#define RF_WIFI_DPD_EN_AX_8_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_8_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_8_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_8_LEN)-1)<<RF_WIFI_DPD_EN_AX_8_POS)
#define RF_WIFI_DPD_EN_AX_8_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_8_LEN)-1)<<RF_WIFI_DPD_EN_AX_8_POS))

/* 0x754 : pwr_wifi_tbl_18 */
#define RF_PWR_WIFI_TBL_18_OFFSET                               (0x754)
#define RF_WIFI_IQC_GAIN_9                                      RF_WIFI_IQC_GAIN_9
#define RF_WIFI_IQC_GAIN_9_POS                                  (0U)
#define RF_WIFI_IQC_GAIN_9_LEN                                  (11U)
#define RF_WIFI_IQC_GAIN_9_MSK                                  (((1U<<RF_WIFI_IQC_GAIN_9_LEN)-1)<<RF_WIFI_IQC_GAIN_9_POS)
#define RF_WIFI_IQC_GAIN_9_UMSK                                 (~(((1U<<RF_WIFI_IQC_GAIN_9_LEN)-1)<<RF_WIFI_IQC_GAIN_9_POS))
#define RF_WIFI_DG1_QDB_9                                       RF_WIFI_DG1_QDB_9
#define RF_WIFI_DG1_QDB_9_POS                                   (11U)
#define RF_WIFI_DG1_QDB_9_LEN                                   (7U)
#define RF_WIFI_DG1_QDB_9_MSK                                   (((1U<<RF_WIFI_DG1_QDB_9_LEN)-1)<<RF_WIFI_DG1_QDB_9_POS)
#define RF_WIFI_DG1_QDB_9_UMSK                                  (~(((1U<<RF_WIFI_DG1_QDB_9_LEN)-1)<<RF_WIFI_DG1_QDB_9_POS))
#define RF_WIFI_GC_TMX_9                                        RF_WIFI_GC_TMX_9
#define RF_WIFI_GC_TMX_9_POS                                    (18U)
#define RF_WIFI_GC_TMX_9_LEN                                    (3U)
#define RF_WIFI_GC_TMX_9_MSK                                    (((1U<<RF_WIFI_GC_TMX_9_LEN)-1)<<RF_WIFI_GC_TMX_9_POS)
#define RF_WIFI_GC_TMX_9_UMSK                                   (~(((1U<<RF_WIFI_GC_TMX_9_LEN)-1)<<RF_WIFI_GC_TMX_9_POS))
#define RF_WIFI_GC_TBB_9                                        RF_WIFI_GC_TBB_9
#define RF_WIFI_GC_TBB_9_POS                                    (21U)
#define RF_WIFI_GC_TBB_9_LEN                                    (5U)
#define RF_WIFI_GC_TBB_9_MSK                                    (((1U<<RF_WIFI_GC_TBB_9_LEN)-1)<<RF_WIFI_GC_TBB_9_POS)
#define RF_WIFI_GC_TBB_9_UMSK                                   (~(((1U<<RF_WIFI_GC_TBB_9_LEN)-1)<<RF_WIFI_GC_TBB_9_POS))
#define RF_WIFI_GC_TBB_BOOST_9                                  RF_WIFI_GC_TBB_BOOST_9
#define RF_WIFI_GC_TBB_BOOST_9_POS                              (26U)
#define RF_WIFI_GC_TBB_BOOST_9_LEN                              (2U)
#define RF_WIFI_GC_TBB_BOOST_9_MSK                              (((1U<<RF_WIFI_GC_TBB_BOOST_9_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_9_POS)
#define RF_WIFI_GC_TBB_BOOST_9_UMSK                             (~(((1U<<RF_WIFI_GC_TBB_BOOST_9_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_9_POS))
#define RF_WIFI_DAC_BIAS_9                                      RF_WIFI_DAC_BIAS_9
#define RF_WIFI_DAC_BIAS_9_POS                                  (28U)
#define RF_WIFI_DAC_BIAS_9_LEN                                  (2U)
#define RF_WIFI_DAC_BIAS_9_MSK                                  (((1U<<RF_WIFI_DAC_BIAS_9_LEN)-1)<<RF_WIFI_DAC_BIAS_9_POS)
#define RF_WIFI_DAC_BIAS_9_UMSK                                 (~(((1U<<RF_WIFI_DAC_BIAS_9_LEN)-1)<<RF_WIFI_DAC_BIAS_9_POS))
#define RF_WIFI_AX_SUBMODE_9                                    RF_WIFI_AX_SUBMODE_9
#define RF_WIFI_AX_SUBMODE_9_POS                                (30U)
#define RF_WIFI_AX_SUBMODE_9_LEN                                (2U)
#define RF_WIFI_AX_SUBMODE_9_MSK                                (((1U<<RF_WIFI_AX_SUBMODE_9_LEN)-1)<<RF_WIFI_AX_SUBMODE_9_POS)
#define RF_WIFI_AX_SUBMODE_9_UMSK                               (~(((1U<<RF_WIFI_AX_SUBMODE_9_LEN)-1)<<RF_WIFI_AX_SUBMODE_9_POS))

/* 0x758 : pwr_wifi_tbl_19 */
#define RF_PWR_WIFI_TBL_19_OFFSET                               (0x758)
#define RF_WIFI_DPD_SEL_9                                       RF_WIFI_DPD_SEL_9
#define RF_WIFI_DPD_SEL_9_POS                                   (0U)
#define RF_WIFI_DPD_SEL_9_LEN                                   (2U)
#define RF_WIFI_DPD_SEL_9_MSK                                   (((1U<<RF_WIFI_DPD_SEL_9_LEN)-1)<<RF_WIFI_DPD_SEL_9_POS)
#define RF_WIFI_DPD_SEL_9_UMSK                                  (~(((1U<<RF_WIFI_DPD_SEL_9_LEN)-1)<<RF_WIFI_DPD_SEL_9_POS))
#define RF_WIFI_TOSDAC_Q_9                                      RF_WIFI_TOSDAC_Q_9
#define RF_WIFI_TOSDAC_Q_9_POS                                  (2U)
#define RF_WIFI_TOSDAC_Q_9_LEN                                  (6U)
#define RF_WIFI_TOSDAC_Q_9_MSK                                  (((1U<<RF_WIFI_TOSDAC_Q_9_LEN)-1)<<RF_WIFI_TOSDAC_Q_9_POS)
#define RF_WIFI_TOSDAC_Q_9_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_Q_9_LEN)-1)<<RF_WIFI_TOSDAC_Q_9_POS))
#define RF_WIFI_TOSDAC_I_9                                      RF_WIFI_TOSDAC_I_9
#define RF_WIFI_TOSDAC_I_9_POS                                  (8U)
#define RF_WIFI_TOSDAC_I_9_LEN                                  (6U)
#define RF_WIFI_TOSDAC_I_9_MSK                                  (((1U<<RF_WIFI_TOSDAC_I_9_LEN)-1)<<RF_WIFI_TOSDAC_I_9_POS)
#define RF_WIFI_TOSDAC_I_9_UMSK                                 (~(((1U<<RF_WIFI_TOSDAC_I_9_LEN)-1)<<RF_WIFI_TOSDAC_I_9_POS))
#define RF_WIFI_IQC_PHASE_9                                     RF_WIFI_IQC_PHASE_9
#define RF_WIFI_IQC_PHASE_9_POS                                 (14U)
#define RF_WIFI_IQC_PHASE_9_LEN                                 (10U)
#define RF_WIFI_IQC_PHASE_9_MSK                                 (((1U<<RF_WIFI_IQC_PHASE_9_LEN)-1)<<RF_WIFI_IQC_PHASE_9_POS)
#define RF_WIFI_IQC_PHASE_9_UMSK                                (~(((1U<<RF_WIFI_IQC_PHASE_9_LEN)-1)<<RF_WIFI_IQC_PHASE_9_POS))
#define RF_WIFI_GC_PA_9                                         RF_WIFI_GC_PA_9
#define RF_WIFI_GC_PA_9_POS                                     (24U)
#define RF_WIFI_GC_PA_9_LEN                                     (3U)
#define RF_WIFI_GC_PA_9_MSK                                     (((1U<<RF_WIFI_GC_PA_9_LEN)-1)<<RF_WIFI_GC_PA_9_POS)
#define RF_WIFI_GC_PA_9_UMSK                                    (~(((1U<<RF_WIFI_GC_PA_9_LEN)-1)<<RF_WIFI_GC_PA_9_POS))
#define RF_WIFI_PA_LP_EN_9                                      RF_WIFI_PA_LP_EN_9
#define RF_WIFI_PA_LP_EN_9_POS                                  (27U)
#define RF_WIFI_PA_LP_EN_9_LEN                                  (1U)
#define RF_WIFI_PA_LP_EN_9_MSK                                  (((1U<<RF_WIFI_PA_LP_EN_9_LEN)-1)<<RF_WIFI_PA_LP_EN_9_POS)
#define RF_WIFI_PA_LP_EN_9_UMSK                                 (~(((1U<<RF_WIFI_PA_LP_EN_9_LEN)-1)<<RF_WIFI_PA_LP_EN_9_POS))
#define RF_WIFI_DPD_EN_B_9                                      RF_WIFI_DPD_EN_B_9
#define RF_WIFI_DPD_EN_B_9_POS                                  (28U)
#define RF_WIFI_DPD_EN_B_9_LEN                                  (1U)
#define RF_WIFI_DPD_EN_B_9_MSK                                  (((1U<<RF_WIFI_DPD_EN_B_9_LEN)-1)<<RF_WIFI_DPD_EN_B_9_POS)
#define RF_WIFI_DPD_EN_B_9_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_B_9_LEN)-1)<<RF_WIFI_DPD_EN_B_9_POS))
#define RF_WIFI_DPD_EN_G_9                                      RF_WIFI_DPD_EN_G_9
#define RF_WIFI_DPD_EN_G_9_POS                                  (29U)
#define RF_WIFI_DPD_EN_G_9_LEN                                  (1U)
#define RF_WIFI_DPD_EN_G_9_MSK                                  (((1U<<RF_WIFI_DPD_EN_G_9_LEN)-1)<<RF_WIFI_DPD_EN_G_9_POS)
#define RF_WIFI_DPD_EN_G_9_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_G_9_LEN)-1)<<RF_WIFI_DPD_EN_G_9_POS))
#define RF_WIFI_DPD_EN_N_9                                      RF_WIFI_DPD_EN_N_9
#define RF_WIFI_DPD_EN_N_9_POS                                  (30U)
#define RF_WIFI_DPD_EN_N_9_LEN                                  (1U)
#define RF_WIFI_DPD_EN_N_9_MSK                                  (((1U<<RF_WIFI_DPD_EN_N_9_LEN)-1)<<RF_WIFI_DPD_EN_N_9_POS)
#define RF_WIFI_DPD_EN_N_9_UMSK                                 (~(((1U<<RF_WIFI_DPD_EN_N_9_LEN)-1)<<RF_WIFI_DPD_EN_N_9_POS))
#define RF_WIFI_DPD_EN_AX_9                                     RF_WIFI_DPD_EN_AX_9
#define RF_WIFI_DPD_EN_AX_9_POS                                 (31U)
#define RF_WIFI_DPD_EN_AX_9_LEN                                 (1U)
#define RF_WIFI_DPD_EN_AX_9_MSK                                 (((1U<<RF_WIFI_DPD_EN_AX_9_LEN)-1)<<RF_WIFI_DPD_EN_AX_9_POS)
#define RF_WIFI_DPD_EN_AX_9_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_AX_9_LEN)-1)<<RF_WIFI_DPD_EN_AX_9_POS))

/* 0x75C : pwr_wifi_tbl_20 */
#define RF_PWR_WIFI_TBL_20_OFFSET                               (0x75C)
#define RF_WIFI_IQC_GAIN_10                                     RF_WIFI_IQC_GAIN_10
#define RF_WIFI_IQC_GAIN_10_POS                                 (0U)
#define RF_WIFI_IQC_GAIN_10_LEN                                 (11U)
#define RF_WIFI_IQC_GAIN_10_MSK                                 (((1U<<RF_WIFI_IQC_GAIN_10_LEN)-1)<<RF_WIFI_IQC_GAIN_10_POS)
#define RF_WIFI_IQC_GAIN_10_UMSK                                (~(((1U<<RF_WIFI_IQC_GAIN_10_LEN)-1)<<RF_WIFI_IQC_GAIN_10_POS))
#define RF_WIFI_DG1_QDB_10                                      RF_WIFI_DG1_QDB_10
#define RF_WIFI_DG1_QDB_10_POS                                  (11U)
#define RF_WIFI_DG1_QDB_10_LEN                                  (7U)
#define RF_WIFI_DG1_QDB_10_MSK                                  (((1U<<RF_WIFI_DG1_QDB_10_LEN)-1)<<RF_WIFI_DG1_QDB_10_POS)
#define RF_WIFI_DG1_QDB_10_UMSK                                 (~(((1U<<RF_WIFI_DG1_QDB_10_LEN)-1)<<RF_WIFI_DG1_QDB_10_POS))
#define RF_WIFI_GC_TMX_10                                       RF_WIFI_GC_TMX_10
#define RF_WIFI_GC_TMX_10_POS                                   (18U)
#define RF_WIFI_GC_TMX_10_LEN                                   (3U)
#define RF_WIFI_GC_TMX_10_MSK                                   (((1U<<RF_WIFI_GC_TMX_10_LEN)-1)<<RF_WIFI_GC_TMX_10_POS)
#define RF_WIFI_GC_TMX_10_UMSK                                  (~(((1U<<RF_WIFI_GC_TMX_10_LEN)-1)<<RF_WIFI_GC_TMX_10_POS))
#define RF_WIFI_GC_TBB_10                                       RF_WIFI_GC_TBB_10
#define RF_WIFI_GC_TBB_10_POS                                   (21U)
#define RF_WIFI_GC_TBB_10_LEN                                   (5U)
#define RF_WIFI_GC_TBB_10_MSK                                   (((1U<<RF_WIFI_GC_TBB_10_LEN)-1)<<RF_WIFI_GC_TBB_10_POS)
#define RF_WIFI_GC_TBB_10_UMSK                                  (~(((1U<<RF_WIFI_GC_TBB_10_LEN)-1)<<RF_WIFI_GC_TBB_10_POS))
#define RF_WIFI_GC_TBB_BOOST_10                                 RF_WIFI_GC_TBB_BOOST_10
#define RF_WIFI_GC_TBB_BOOST_10_POS                             (26U)
#define RF_WIFI_GC_TBB_BOOST_10_LEN                             (2U)
#define RF_WIFI_GC_TBB_BOOST_10_MSK                             (((1U<<RF_WIFI_GC_TBB_BOOST_10_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_10_POS)
#define RF_WIFI_GC_TBB_BOOST_10_UMSK                            (~(((1U<<RF_WIFI_GC_TBB_BOOST_10_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_10_POS))
#define RF_WIFI_DAC_BIAS_10                                     RF_WIFI_DAC_BIAS_10
#define RF_WIFI_DAC_BIAS_10_POS                                 (28U)
#define RF_WIFI_DAC_BIAS_10_LEN                                 (2U)
#define RF_WIFI_DAC_BIAS_10_MSK                                 (((1U<<RF_WIFI_DAC_BIAS_10_LEN)-1)<<RF_WIFI_DAC_BIAS_10_POS)
#define RF_WIFI_DAC_BIAS_10_UMSK                                (~(((1U<<RF_WIFI_DAC_BIAS_10_LEN)-1)<<RF_WIFI_DAC_BIAS_10_POS))
#define RF_WIFI_AX_SUBMODE_10                                   RF_WIFI_AX_SUBMODE_10
#define RF_WIFI_AX_SUBMODE_10_POS                               (30U)
#define RF_WIFI_AX_SUBMODE_10_LEN                               (2U)
#define RF_WIFI_AX_SUBMODE_10_MSK                               (((1U<<RF_WIFI_AX_SUBMODE_10_LEN)-1)<<RF_WIFI_AX_SUBMODE_10_POS)
#define RF_WIFI_AX_SUBMODE_10_UMSK                              (~(((1U<<RF_WIFI_AX_SUBMODE_10_LEN)-1)<<RF_WIFI_AX_SUBMODE_10_POS))

/* 0x760 : pwr_wifi_tbl_21 */
#define RF_PWR_WIFI_TBL_21_OFFSET                               (0x760)
#define RF_WIFI_DPD_SEL_10                                      RF_WIFI_DPD_SEL_10
#define RF_WIFI_DPD_SEL_10_POS                                  (0U)
#define RF_WIFI_DPD_SEL_10_LEN                                  (2U)
#define RF_WIFI_DPD_SEL_10_MSK                                  (((1U<<RF_WIFI_DPD_SEL_10_LEN)-1)<<RF_WIFI_DPD_SEL_10_POS)
#define RF_WIFI_DPD_SEL_10_UMSK                                 (~(((1U<<RF_WIFI_DPD_SEL_10_LEN)-1)<<RF_WIFI_DPD_SEL_10_POS))
#define RF_WIFI_TOSDAC_Q_10                                     RF_WIFI_TOSDAC_Q_10
#define RF_WIFI_TOSDAC_Q_10_POS                                 (2U)
#define RF_WIFI_TOSDAC_Q_10_LEN                                 (6U)
#define RF_WIFI_TOSDAC_Q_10_MSK                                 (((1U<<RF_WIFI_TOSDAC_Q_10_LEN)-1)<<RF_WIFI_TOSDAC_Q_10_POS)
#define RF_WIFI_TOSDAC_Q_10_UMSK                                (~(((1U<<RF_WIFI_TOSDAC_Q_10_LEN)-1)<<RF_WIFI_TOSDAC_Q_10_POS))
#define RF_WIFI_TOSDAC_I_10                                     RF_WIFI_TOSDAC_I_10
#define RF_WIFI_TOSDAC_I_10_POS                                 (8U)
#define RF_WIFI_TOSDAC_I_10_LEN                                 (6U)
#define RF_WIFI_TOSDAC_I_10_MSK                                 (((1U<<RF_WIFI_TOSDAC_I_10_LEN)-1)<<RF_WIFI_TOSDAC_I_10_POS)
#define RF_WIFI_TOSDAC_I_10_UMSK                                (~(((1U<<RF_WIFI_TOSDAC_I_10_LEN)-1)<<RF_WIFI_TOSDAC_I_10_POS))
#define RF_WIFI_IQC_PHASE_10                                    RF_WIFI_IQC_PHASE_10
#define RF_WIFI_IQC_PHASE_10_POS                                (14U)
#define RF_WIFI_IQC_PHASE_10_LEN                                (10U)
#define RF_WIFI_IQC_PHASE_10_MSK                                (((1U<<RF_WIFI_IQC_PHASE_10_LEN)-1)<<RF_WIFI_IQC_PHASE_10_POS)
#define RF_WIFI_IQC_PHASE_10_UMSK                               (~(((1U<<RF_WIFI_IQC_PHASE_10_LEN)-1)<<RF_WIFI_IQC_PHASE_10_POS))
#define RF_WIFI_GC_PA_10                                        RF_WIFI_GC_PA_10
#define RF_WIFI_GC_PA_10_POS                                    (24U)
#define RF_WIFI_GC_PA_10_LEN                                    (3U)
#define RF_WIFI_GC_PA_10_MSK                                    (((1U<<RF_WIFI_GC_PA_10_LEN)-1)<<RF_WIFI_GC_PA_10_POS)
#define RF_WIFI_GC_PA_10_UMSK                                   (~(((1U<<RF_WIFI_GC_PA_10_LEN)-1)<<RF_WIFI_GC_PA_10_POS))
#define RF_WIFI_PA_LP_EN_10                                     RF_WIFI_PA_LP_EN_10
#define RF_WIFI_PA_LP_EN_10_POS                                 (27U)
#define RF_WIFI_PA_LP_EN_10_LEN                                 (1U)
#define RF_WIFI_PA_LP_EN_10_MSK                                 (((1U<<RF_WIFI_PA_LP_EN_10_LEN)-1)<<RF_WIFI_PA_LP_EN_10_POS)
#define RF_WIFI_PA_LP_EN_10_UMSK                                (~(((1U<<RF_WIFI_PA_LP_EN_10_LEN)-1)<<RF_WIFI_PA_LP_EN_10_POS))
#define RF_WIFI_DPD_EN_B_10                                     RF_WIFI_DPD_EN_B_10
#define RF_WIFI_DPD_EN_B_10_POS                                 (28U)
#define RF_WIFI_DPD_EN_B_10_LEN                                 (1U)
#define RF_WIFI_DPD_EN_B_10_MSK                                 (((1U<<RF_WIFI_DPD_EN_B_10_LEN)-1)<<RF_WIFI_DPD_EN_B_10_POS)
#define RF_WIFI_DPD_EN_B_10_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_B_10_LEN)-1)<<RF_WIFI_DPD_EN_B_10_POS))
#define RF_WIFI_DPD_EN_G_10                                     RF_WIFI_DPD_EN_G_10
#define RF_WIFI_DPD_EN_G_10_POS                                 (29U)
#define RF_WIFI_DPD_EN_G_10_LEN                                 (1U)
#define RF_WIFI_DPD_EN_G_10_MSK                                 (((1U<<RF_WIFI_DPD_EN_G_10_LEN)-1)<<RF_WIFI_DPD_EN_G_10_POS)
#define RF_WIFI_DPD_EN_G_10_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_G_10_LEN)-1)<<RF_WIFI_DPD_EN_G_10_POS))
#define RF_WIFI_DPD_EN_N_10                                     RF_WIFI_DPD_EN_N_10
#define RF_WIFI_DPD_EN_N_10_POS                                 (30U)
#define RF_WIFI_DPD_EN_N_10_LEN                                 (1U)
#define RF_WIFI_DPD_EN_N_10_MSK                                 (((1U<<RF_WIFI_DPD_EN_N_10_LEN)-1)<<RF_WIFI_DPD_EN_N_10_POS)
#define RF_WIFI_DPD_EN_N_10_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_N_10_LEN)-1)<<RF_WIFI_DPD_EN_N_10_POS))
#define RF_WIFI_DPD_EN_AX_10                                    RF_WIFI_DPD_EN_AX_10
#define RF_WIFI_DPD_EN_AX_10_POS                                (31U)
#define RF_WIFI_DPD_EN_AX_10_LEN                                (1U)
#define RF_WIFI_DPD_EN_AX_10_MSK                                (((1U<<RF_WIFI_DPD_EN_AX_10_LEN)-1)<<RF_WIFI_DPD_EN_AX_10_POS)
#define RF_WIFI_DPD_EN_AX_10_UMSK                               (~(((1U<<RF_WIFI_DPD_EN_AX_10_LEN)-1)<<RF_WIFI_DPD_EN_AX_10_POS))

/* 0x764 : pwr_wifi_tbl_22 */
#define RF_PWR_WIFI_TBL_22_OFFSET                               (0x764)
#define RF_WIFI_IQC_GAIN_11                                     RF_WIFI_IQC_GAIN_11
#define RF_WIFI_IQC_GAIN_11_POS                                 (0U)
#define RF_WIFI_IQC_GAIN_11_LEN                                 (11U)
#define RF_WIFI_IQC_GAIN_11_MSK                                 (((1U<<RF_WIFI_IQC_GAIN_11_LEN)-1)<<RF_WIFI_IQC_GAIN_11_POS)
#define RF_WIFI_IQC_GAIN_11_UMSK                                (~(((1U<<RF_WIFI_IQC_GAIN_11_LEN)-1)<<RF_WIFI_IQC_GAIN_11_POS))
#define RF_WIFI_DG1_QDB_11                                      RF_WIFI_DG1_QDB_11
#define RF_WIFI_DG1_QDB_11_POS                                  (11U)
#define RF_WIFI_DG1_QDB_11_LEN                                  (7U)
#define RF_WIFI_DG1_QDB_11_MSK                                  (((1U<<RF_WIFI_DG1_QDB_11_LEN)-1)<<RF_WIFI_DG1_QDB_11_POS)
#define RF_WIFI_DG1_QDB_11_UMSK                                 (~(((1U<<RF_WIFI_DG1_QDB_11_LEN)-1)<<RF_WIFI_DG1_QDB_11_POS))
#define RF_WIFI_GC_TMX_11                                       RF_WIFI_GC_TMX_11
#define RF_WIFI_GC_TMX_11_POS                                   (18U)
#define RF_WIFI_GC_TMX_11_LEN                                   (3U)
#define RF_WIFI_GC_TMX_11_MSK                                   (((1U<<RF_WIFI_GC_TMX_11_LEN)-1)<<RF_WIFI_GC_TMX_11_POS)
#define RF_WIFI_GC_TMX_11_UMSK                                  (~(((1U<<RF_WIFI_GC_TMX_11_LEN)-1)<<RF_WIFI_GC_TMX_11_POS))
#define RF_WIFI_GC_TBB_11                                       RF_WIFI_GC_TBB_11
#define RF_WIFI_GC_TBB_11_POS                                   (21U)
#define RF_WIFI_GC_TBB_11_LEN                                   (5U)
#define RF_WIFI_GC_TBB_11_MSK                                   (((1U<<RF_WIFI_GC_TBB_11_LEN)-1)<<RF_WIFI_GC_TBB_11_POS)
#define RF_WIFI_GC_TBB_11_UMSK                                  (~(((1U<<RF_WIFI_GC_TBB_11_LEN)-1)<<RF_WIFI_GC_TBB_11_POS))
#define RF_WIFI_GC_TBB_BOOST_11                                 RF_WIFI_GC_TBB_BOOST_11
#define RF_WIFI_GC_TBB_BOOST_11_POS                             (26U)
#define RF_WIFI_GC_TBB_BOOST_11_LEN                             (2U)
#define RF_WIFI_GC_TBB_BOOST_11_MSK                             (((1U<<RF_WIFI_GC_TBB_BOOST_11_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_11_POS)
#define RF_WIFI_GC_TBB_BOOST_11_UMSK                            (~(((1U<<RF_WIFI_GC_TBB_BOOST_11_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_11_POS))
#define RF_WIFI_DAC_BIAS_11                                     RF_WIFI_DAC_BIAS_11
#define RF_WIFI_DAC_BIAS_11_POS                                 (28U)
#define RF_WIFI_DAC_BIAS_11_LEN                                 (2U)
#define RF_WIFI_DAC_BIAS_11_MSK                                 (((1U<<RF_WIFI_DAC_BIAS_11_LEN)-1)<<RF_WIFI_DAC_BIAS_11_POS)
#define RF_WIFI_DAC_BIAS_11_UMSK                                (~(((1U<<RF_WIFI_DAC_BIAS_11_LEN)-1)<<RF_WIFI_DAC_BIAS_11_POS))
#define RF_WIFI_AX_SUBMODE_11                                   RF_WIFI_AX_SUBMODE_11
#define RF_WIFI_AX_SUBMODE_11_POS                               (30U)
#define RF_WIFI_AX_SUBMODE_11_LEN                               (2U)
#define RF_WIFI_AX_SUBMODE_11_MSK                               (((1U<<RF_WIFI_AX_SUBMODE_11_LEN)-1)<<RF_WIFI_AX_SUBMODE_11_POS)
#define RF_WIFI_AX_SUBMODE_11_UMSK                              (~(((1U<<RF_WIFI_AX_SUBMODE_11_LEN)-1)<<RF_WIFI_AX_SUBMODE_11_POS))

/* 0x768 : pwr_wifi_tbl_23 */
#define RF_PWR_WIFI_TBL_23_OFFSET                               (0x768)
#define RF_WIFI_DPD_SEL_11                                      RF_WIFI_DPD_SEL_11
#define RF_WIFI_DPD_SEL_11_POS                                  (0U)
#define RF_WIFI_DPD_SEL_11_LEN                                  (2U)
#define RF_WIFI_DPD_SEL_11_MSK                                  (((1U<<RF_WIFI_DPD_SEL_11_LEN)-1)<<RF_WIFI_DPD_SEL_11_POS)
#define RF_WIFI_DPD_SEL_11_UMSK                                 (~(((1U<<RF_WIFI_DPD_SEL_11_LEN)-1)<<RF_WIFI_DPD_SEL_11_POS))
#define RF_WIFI_TOSDAC_Q_11                                     RF_WIFI_TOSDAC_Q_11
#define RF_WIFI_TOSDAC_Q_11_POS                                 (2U)
#define RF_WIFI_TOSDAC_Q_11_LEN                                 (6U)
#define RF_WIFI_TOSDAC_Q_11_MSK                                 (((1U<<RF_WIFI_TOSDAC_Q_11_LEN)-1)<<RF_WIFI_TOSDAC_Q_11_POS)
#define RF_WIFI_TOSDAC_Q_11_UMSK                                (~(((1U<<RF_WIFI_TOSDAC_Q_11_LEN)-1)<<RF_WIFI_TOSDAC_Q_11_POS))
#define RF_WIFI_TOSDAC_I_11                                     RF_WIFI_TOSDAC_I_11
#define RF_WIFI_TOSDAC_I_11_POS                                 (8U)
#define RF_WIFI_TOSDAC_I_11_LEN                                 (6U)
#define RF_WIFI_TOSDAC_I_11_MSK                                 (((1U<<RF_WIFI_TOSDAC_I_11_LEN)-1)<<RF_WIFI_TOSDAC_I_11_POS)
#define RF_WIFI_TOSDAC_I_11_UMSK                                (~(((1U<<RF_WIFI_TOSDAC_I_11_LEN)-1)<<RF_WIFI_TOSDAC_I_11_POS))
#define RF_WIFI_IQC_PHASE_11                                    RF_WIFI_IQC_PHASE_11
#define RF_WIFI_IQC_PHASE_11_POS                                (14U)
#define RF_WIFI_IQC_PHASE_11_LEN                                (10U)
#define RF_WIFI_IQC_PHASE_11_MSK                                (((1U<<RF_WIFI_IQC_PHASE_11_LEN)-1)<<RF_WIFI_IQC_PHASE_11_POS)
#define RF_WIFI_IQC_PHASE_11_UMSK                               (~(((1U<<RF_WIFI_IQC_PHASE_11_LEN)-1)<<RF_WIFI_IQC_PHASE_11_POS))
#define RF_WIFI_GC_PA_11                                        RF_WIFI_GC_PA_11
#define RF_WIFI_GC_PA_11_POS                                    (24U)
#define RF_WIFI_GC_PA_11_LEN                                    (3U)
#define RF_WIFI_GC_PA_11_MSK                                    (((1U<<RF_WIFI_GC_PA_11_LEN)-1)<<RF_WIFI_GC_PA_11_POS)
#define RF_WIFI_GC_PA_11_UMSK                                   (~(((1U<<RF_WIFI_GC_PA_11_LEN)-1)<<RF_WIFI_GC_PA_11_POS))
#define RF_WIFI_PA_LP_EN_11                                     RF_WIFI_PA_LP_EN_11
#define RF_WIFI_PA_LP_EN_11_POS                                 (27U)
#define RF_WIFI_PA_LP_EN_11_LEN                                 (1U)
#define RF_WIFI_PA_LP_EN_11_MSK                                 (((1U<<RF_WIFI_PA_LP_EN_11_LEN)-1)<<RF_WIFI_PA_LP_EN_11_POS)
#define RF_WIFI_PA_LP_EN_11_UMSK                                (~(((1U<<RF_WIFI_PA_LP_EN_11_LEN)-1)<<RF_WIFI_PA_LP_EN_11_POS))
#define RF_WIFI_DPD_EN_B_11                                     RF_WIFI_DPD_EN_B_11
#define RF_WIFI_DPD_EN_B_11_POS                                 (28U)
#define RF_WIFI_DPD_EN_B_11_LEN                                 (1U)
#define RF_WIFI_DPD_EN_B_11_MSK                                 (((1U<<RF_WIFI_DPD_EN_B_11_LEN)-1)<<RF_WIFI_DPD_EN_B_11_POS)
#define RF_WIFI_DPD_EN_B_11_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_B_11_LEN)-1)<<RF_WIFI_DPD_EN_B_11_POS))
#define RF_WIFI_DPD_EN_G_11                                     RF_WIFI_DPD_EN_G_11
#define RF_WIFI_DPD_EN_G_11_POS                                 (29U)
#define RF_WIFI_DPD_EN_G_11_LEN                                 (1U)
#define RF_WIFI_DPD_EN_G_11_MSK                                 (((1U<<RF_WIFI_DPD_EN_G_11_LEN)-1)<<RF_WIFI_DPD_EN_G_11_POS)
#define RF_WIFI_DPD_EN_G_11_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_G_11_LEN)-1)<<RF_WIFI_DPD_EN_G_11_POS))
#define RF_WIFI_DPD_EN_N_11                                     RF_WIFI_DPD_EN_N_11
#define RF_WIFI_DPD_EN_N_11_POS                                 (30U)
#define RF_WIFI_DPD_EN_N_11_LEN                                 (1U)
#define RF_WIFI_DPD_EN_N_11_MSK                                 (((1U<<RF_WIFI_DPD_EN_N_11_LEN)-1)<<RF_WIFI_DPD_EN_N_11_POS)
#define RF_WIFI_DPD_EN_N_11_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_N_11_LEN)-1)<<RF_WIFI_DPD_EN_N_11_POS))
#define RF_WIFI_DPD_EN_AX_11                                    RF_WIFI_DPD_EN_AX_11
#define RF_WIFI_DPD_EN_AX_11_POS                                (31U)
#define RF_WIFI_DPD_EN_AX_11_LEN                                (1U)
#define RF_WIFI_DPD_EN_AX_11_MSK                                (((1U<<RF_WIFI_DPD_EN_AX_11_LEN)-1)<<RF_WIFI_DPD_EN_AX_11_POS)
#define RF_WIFI_DPD_EN_AX_11_UMSK                               (~(((1U<<RF_WIFI_DPD_EN_AX_11_LEN)-1)<<RF_WIFI_DPD_EN_AX_11_POS))

/* 0x76C : pwr_wifi_tbl_24 */
#define RF_PWR_WIFI_TBL_24_OFFSET                               (0x76C)
#define RF_WIFI_IQC_GAIN_12                                     RF_WIFI_IQC_GAIN_12
#define RF_WIFI_IQC_GAIN_12_POS                                 (0U)
#define RF_WIFI_IQC_GAIN_12_LEN                                 (11U)
#define RF_WIFI_IQC_GAIN_12_MSK                                 (((1U<<RF_WIFI_IQC_GAIN_12_LEN)-1)<<RF_WIFI_IQC_GAIN_12_POS)
#define RF_WIFI_IQC_GAIN_12_UMSK                                (~(((1U<<RF_WIFI_IQC_GAIN_12_LEN)-1)<<RF_WIFI_IQC_GAIN_12_POS))
#define RF_WIFI_DG1_QDB_12                                      RF_WIFI_DG1_QDB_12
#define RF_WIFI_DG1_QDB_12_POS                                  (11U)
#define RF_WIFI_DG1_QDB_12_LEN                                  (7U)
#define RF_WIFI_DG1_QDB_12_MSK                                  (((1U<<RF_WIFI_DG1_QDB_12_LEN)-1)<<RF_WIFI_DG1_QDB_12_POS)
#define RF_WIFI_DG1_QDB_12_UMSK                                 (~(((1U<<RF_WIFI_DG1_QDB_12_LEN)-1)<<RF_WIFI_DG1_QDB_12_POS))
#define RF_WIFI_GC_TMX_12                                       RF_WIFI_GC_TMX_12
#define RF_WIFI_GC_TMX_12_POS                                   (18U)
#define RF_WIFI_GC_TMX_12_LEN                                   (3U)
#define RF_WIFI_GC_TMX_12_MSK                                   (((1U<<RF_WIFI_GC_TMX_12_LEN)-1)<<RF_WIFI_GC_TMX_12_POS)
#define RF_WIFI_GC_TMX_12_UMSK                                  (~(((1U<<RF_WIFI_GC_TMX_12_LEN)-1)<<RF_WIFI_GC_TMX_12_POS))
#define RF_WIFI_GC_TBB_12                                       RF_WIFI_GC_TBB_12
#define RF_WIFI_GC_TBB_12_POS                                   (21U)
#define RF_WIFI_GC_TBB_12_LEN                                   (5U)
#define RF_WIFI_GC_TBB_12_MSK                                   (((1U<<RF_WIFI_GC_TBB_12_LEN)-1)<<RF_WIFI_GC_TBB_12_POS)
#define RF_WIFI_GC_TBB_12_UMSK                                  (~(((1U<<RF_WIFI_GC_TBB_12_LEN)-1)<<RF_WIFI_GC_TBB_12_POS))
#define RF_WIFI_GC_TBB_BOOST_12                                 RF_WIFI_GC_TBB_BOOST_12
#define RF_WIFI_GC_TBB_BOOST_12_POS                             (26U)
#define RF_WIFI_GC_TBB_BOOST_12_LEN                             (2U)
#define RF_WIFI_GC_TBB_BOOST_12_MSK                             (((1U<<RF_WIFI_GC_TBB_BOOST_12_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_12_POS)
#define RF_WIFI_GC_TBB_BOOST_12_UMSK                            (~(((1U<<RF_WIFI_GC_TBB_BOOST_12_LEN)-1)<<RF_WIFI_GC_TBB_BOOST_12_POS))
#define RF_WIFI_DAC_BIAS_12                                     RF_WIFI_DAC_BIAS_12
#define RF_WIFI_DAC_BIAS_12_POS                                 (28U)
#define RF_WIFI_DAC_BIAS_12_LEN                                 (2U)
#define RF_WIFI_DAC_BIAS_12_MSK                                 (((1U<<RF_WIFI_DAC_BIAS_12_LEN)-1)<<RF_WIFI_DAC_BIAS_12_POS)
#define RF_WIFI_DAC_BIAS_12_UMSK                                (~(((1U<<RF_WIFI_DAC_BIAS_12_LEN)-1)<<RF_WIFI_DAC_BIAS_12_POS))
#define RF_WIFI_AX_SUBMODE_12                                   RF_WIFI_AX_SUBMODE_12
#define RF_WIFI_AX_SUBMODE_12_POS                               (30U)
#define RF_WIFI_AX_SUBMODE_12_LEN                               (2U)
#define RF_WIFI_AX_SUBMODE_12_MSK                               (((1U<<RF_WIFI_AX_SUBMODE_12_LEN)-1)<<RF_WIFI_AX_SUBMODE_12_POS)
#define RF_WIFI_AX_SUBMODE_12_UMSK                              (~(((1U<<RF_WIFI_AX_SUBMODE_12_LEN)-1)<<RF_WIFI_AX_SUBMODE_12_POS))

/* 0x770 : pwr_wifi_tbl_25 */
#define RF_PWR_WIFI_TBL_25_OFFSET                               (0x770)
#define RF_WIFI_DPD_SEL_12                                      RF_WIFI_DPD_SEL_12
#define RF_WIFI_DPD_SEL_12_POS                                  (0U)
#define RF_WIFI_DPD_SEL_12_LEN                                  (2U)
#define RF_WIFI_DPD_SEL_12_MSK                                  (((1U<<RF_WIFI_DPD_SEL_12_LEN)-1)<<RF_WIFI_DPD_SEL_12_POS)
#define RF_WIFI_DPD_SEL_12_UMSK                                 (~(((1U<<RF_WIFI_DPD_SEL_12_LEN)-1)<<RF_WIFI_DPD_SEL_12_POS))
#define RF_WIFI_TOSDAC_Q_12                                     RF_WIFI_TOSDAC_Q_12
#define RF_WIFI_TOSDAC_Q_12_POS                                 (2U)
#define RF_WIFI_TOSDAC_Q_12_LEN                                 (6U)
#define RF_WIFI_TOSDAC_Q_12_MSK                                 (((1U<<RF_WIFI_TOSDAC_Q_12_LEN)-1)<<RF_WIFI_TOSDAC_Q_12_POS)
#define RF_WIFI_TOSDAC_Q_12_UMSK                                (~(((1U<<RF_WIFI_TOSDAC_Q_12_LEN)-1)<<RF_WIFI_TOSDAC_Q_12_POS))
#define RF_WIFI_TOSDAC_I_12                                     RF_WIFI_TOSDAC_I_12
#define RF_WIFI_TOSDAC_I_12_POS                                 (8U)
#define RF_WIFI_TOSDAC_I_12_LEN                                 (6U)
#define RF_WIFI_TOSDAC_I_12_MSK                                 (((1U<<RF_WIFI_TOSDAC_I_12_LEN)-1)<<RF_WIFI_TOSDAC_I_12_POS)
#define RF_WIFI_TOSDAC_I_12_UMSK                                (~(((1U<<RF_WIFI_TOSDAC_I_12_LEN)-1)<<RF_WIFI_TOSDAC_I_12_POS))
#define RF_WIFI_IQC_PHASE_12                                    RF_WIFI_IQC_PHASE_12
#define RF_WIFI_IQC_PHASE_12_POS                                (14U)
#define RF_WIFI_IQC_PHASE_12_LEN                                (10U)
#define RF_WIFI_IQC_PHASE_12_MSK                                (((1U<<RF_WIFI_IQC_PHASE_12_LEN)-1)<<RF_WIFI_IQC_PHASE_12_POS)
#define RF_WIFI_IQC_PHASE_12_UMSK                               (~(((1U<<RF_WIFI_IQC_PHASE_12_LEN)-1)<<RF_WIFI_IQC_PHASE_12_POS))
#define RF_WIFI_GC_PA_12                                        RF_WIFI_GC_PA_12
#define RF_WIFI_GC_PA_12_POS                                    (24U)
#define RF_WIFI_GC_PA_12_LEN                                    (3U)
#define RF_WIFI_GC_PA_12_MSK                                    (((1U<<RF_WIFI_GC_PA_12_LEN)-1)<<RF_WIFI_GC_PA_12_POS)
#define RF_WIFI_GC_PA_12_UMSK                                   (~(((1U<<RF_WIFI_GC_PA_12_LEN)-1)<<RF_WIFI_GC_PA_12_POS))
#define RF_WIFI_PA_LP_EN_12                                     RF_WIFI_PA_LP_EN_12
#define RF_WIFI_PA_LP_EN_12_POS                                 (27U)
#define RF_WIFI_PA_LP_EN_12_LEN                                 (1U)
#define RF_WIFI_PA_LP_EN_12_MSK                                 (((1U<<RF_WIFI_PA_LP_EN_12_LEN)-1)<<RF_WIFI_PA_LP_EN_12_POS)
#define RF_WIFI_PA_LP_EN_12_UMSK                                (~(((1U<<RF_WIFI_PA_LP_EN_12_LEN)-1)<<RF_WIFI_PA_LP_EN_12_POS))
#define RF_WIFI_DPD_EN_B_12                                     RF_WIFI_DPD_EN_B_12
#define RF_WIFI_DPD_EN_B_12_POS                                 (28U)
#define RF_WIFI_DPD_EN_B_12_LEN                                 (1U)
#define RF_WIFI_DPD_EN_B_12_MSK                                 (((1U<<RF_WIFI_DPD_EN_B_12_LEN)-1)<<RF_WIFI_DPD_EN_B_12_POS)
#define RF_WIFI_DPD_EN_B_12_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_B_12_LEN)-1)<<RF_WIFI_DPD_EN_B_12_POS))
#define RF_WIFI_DPD_EN_G_12                                     RF_WIFI_DPD_EN_G_12
#define RF_WIFI_DPD_EN_G_12_POS                                 (29U)
#define RF_WIFI_DPD_EN_G_12_LEN                                 (1U)
#define RF_WIFI_DPD_EN_G_12_MSK                                 (((1U<<RF_WIFI_DPD_EN_G_12_LEN)-1)<<RF_WIFI_DPD_EN_G_12_POS)
#define RF_WIFI_DPD_EN_G_12_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_G_12_LEN)-1)<<RF_WIFI_DPD_EN_G_12_POS))
#define RF_WIFI_DPD_EN_N_12                                     RF_WIFI_DPD_EN_N_12
#define RF_WIFI_DPD_EN_N_12_POS                                 (30U)
#define RF_WIFI_DPD_EN_N_12_LEN                                 (1U)
#define RF_WIFI_DPD_EN_N_12_MSK                                 (((1U<<RF_WIFI_DPD_EN_N_12_LEN)-1)<<RF_WIFI_DPD_EN_N_12_POS)
#define RF_WIFI_DPD_EN_N_12_UMSK                                (~(((1U<<RF_WIFI_DPD_EN_N_12_LEN)-1)<<RF_WIFI_DPD_EN_N_12_POS))
#define RF_WIFI_DPD_EN_AX_12                                    RF_WIFI_DPD_EN_AX_12
#define RF_WIFI_DPD_EN_AX_12_POS                                (31U)
#define RF_WIFI_DPD_EN_AX_12_LEN                                (1U)
#define RF_WIFI_DPD_EN_AX_12_MSK                                (((1U<<RF_WIFI_DPD_EN_AX_12_LEN)-1)<<RF_WIFI_DPD_EN_AX_12_POS)
#define RF_WIFI_DPD_EN_AX_12_UMSK                               (~(((1U<<RF_WIFI_DPD_EN_AX_12_LEN)-1)<<RF_WIFI_DPD_EN_AX_12_POS))

/* 0x774 : pwr_bz_ctrl_0 */
#define RF_PWR_BZ_CTRL_0_OFFSET                                 (0x774)
#define RF_BZ_TX_BACKOFF_00                                     RF_BZ_TX_BACKOFF_00
#define RF_BZ_TX_BACKOFF_00_POS                                 (0U)
#define RF_BZ_TX_BACKOFF_00_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_00_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_00_LEN)-1)<<RF_BZ_TX_BACKOFF_00_POS)
#define RF_BZ_TX_BACKOFF_00_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_00_LEN)-1)<<RF_BZ_TX_BACKOFF_00_POS))
#define RF_BZ_TX_BACKOFF_01                                     RF_BZ_TX_BACKOFF_01
#define RF_BZ_TX_BACKOFF_01_POS                                 (4U)
#define RF_BZ_TX_BACKOFF_01_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_01_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_01_LEN)-1)<<RF_BZ_TX_BACKOFF_01_POS)
#define RF_BZ_TX_BACKOFF_01_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_01_LEN)-1)<<RF_BZ_TX_BACKOFF_01_POS))
#define RF_BZ_TX_BACKOFF_02                                     RF_BZ_TX_BACKOFF_02
#define RF_BZ_TX_BACKOFF_02_POS                                 (8U)
#define RF_BZ_TX_BACKOFF_02_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_02_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_02_LEN)-1)<<RF_BZ_TX_BACKOFF_02_POS)
#define RF_BZ_TX_BACKOFF_02_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_02_LEN)-1)<<RF_BZ_TX_BACKOFF_02_POS))
#define RF_BZ_TX_BACKOFF_03                                     RF_BZ_TX_BACKOFF_03
#define RF_BZ_TX_BACKOFF_03_POS                                 (12U)
#define RF_BZ_TX_BACKOFF_03_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_03_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_03_LEN)-1)<<RF_BZ_TX_BACKOFF_03_POS)
#define RF_BZ_TX_BACKOFF_03_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_03_LEN)-1)<<RF_BZ_TX_BACKOFF_03_POS))
#define RF_BZ_TX_BACKOFF_04                                     RF_BZ_TX_BACKOFF_04
#define RF_BZ_TX_BACKOFF_04_POS                                 (16U)
#define RF_BZ_TX_BACKOFF_04_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_04_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_04_LEN)-1)<<RF_BZ_TX_BACKOFF_04_POS)
#define RF_BZ_TX_BACKOFF_04_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_04_LEN)-1)<<RF_BZ_TX_BACKOFF_04_POS))
#define RF_BZ_TX_BACKOFF_05                                     RF_BZ_TX_BACKOFF_05
#define RF_BZ_TX_BACKOFF_05_POS                                 (20U)
#define RF_BZ_TX_BACKOFF_05_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_05_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_05_LEN)-1)<<RF_BZ_TX_BACKOFF_05_POS)
#define RF_BZ_TX_BACKOFF_05_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_05_LEN)-1)<<RF_BZ_TX_BACKOFF_05_POS))
#define RF_BZ_TX_BACKOFF_06                                     RF_BZ_TX_BACKOFF_06
#define RF_BZ_TX_BACKOFF_06_POS                                 (24U)
#define RF_BZ_TX_BACKOFF_06_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_06_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_06_LEN)-1)<<RF_BZ_TX_BACKOFF_06_POS)
#define RF_BZ_TX_BACKOFF_06_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_06_LEN)-1)<<RF_BZ_TX_BACKOFF_06_POS))
#define RF_BZ_TX_BACKOFF_07                                     RF_BZ_TX_BACKOFF_07
#define RF_BZ_TX_BACKOFF_07_POS                                 (28U)
#define RF_BZ_TX_BACKOFF_07_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_07_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_07_LEN)-1)<<RF_BZ_TX_BACKOFF_07_POS)
#define RF_BZ_TX_BACKOFF_07_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_07_LEN)-1)<<RF_BZ_TX_BACKOFF_07_POS))

/* 0x778 : pwr_bz_ctrl_1 */
#define RF_PWR_BZ_CTRL_1_OFFSET                                 (0x778)
#define RF_BZ_TX_BACKOFF_08                                     RF_BZ_TX_BACKOFF_08
#define RF_BZ_TX_BACKOFF_08_POS                                 (0U)
#define RF_BZ_TX_BACKOFF_08_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_08_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_08_LEN)-1)<<RF_BZ_TX_BACKOFF_08_POS)
#define RF_BZ_TX_BACKOFF_08_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_08_LEN)-1)<<RF_BZ_TX_BACKOFF_08_POS))
#define RF_BZ_TX_BACKOFF_09                                     RF_BZ_TX_BACKOFF_09
#define RF_BZ_TX_BACKOFF_09_POS                                 (4U)
#define RF_BZ_TX_BACKOFF_09_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_09_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_09_LEN)-1)<<RF_BZ_TX_BACKOFF_09_POS)
#define RF_BZ_TX_BACKOFF_09_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_09_LEN)-1)<<RF_BZ_TX_BACKOFF_09_POS))
#define RF_BZ_TX_BACKOFF_10                                     RF_BZ_TX_BACKOFF_10
#define RF_BZ_TX_BACKOFF_10_POS                                 (8U)
#define RF_BZ_TX_BACKOFF_10_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_10_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_10_LEN)-1)<<RF_BZ_TX_BACKOFF_10_POS)
#define RF_BZ_TX_BACKOFF_10_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_10_LEN)-1)<<RF_BZ_TX_BACKOFF_10_POS))
#define RF_BZ_TX_BACKOFF_11                                     RF_BZ_TX_BACKOFF_11
#define RF_BZ_TX_BACKOFF_11_POS                                 (12U)
#define RF_BZ_TX_BACKOFF_11_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_11_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_11_LEN)-1)<<RF_BZ_TX_BACKOFF_11_POS)
#define RF_BZ_TX_BACKOFF_11_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_11_LEN)-1)<<RF_BZ_TX_BACKOFF_11_POS))
#define RF_BZ_TX_BACKOFF_12                                     RF_BZ_TX_BACKOFF_12
#define RF_BZ_TX_BACKOFF_12_POS                                 (16U)
#define RF_BZ_TX_BACKOFF_12_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_12_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_12_LEN)-1)<<RF_BZ_TX_BACKOFF_12_POS)
#define RF_BZ_TX_BACKOFF_12_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_12_LEN)-1)<<RF_BZ_TX_BACKOFF_12_POS))
#define RF_BZ_TX_BACKOFF_13                                     RF_BZ_TX_BACKOFF_13
#define RF_BZ_TX_BACKOFF_13_POS                                 (20U)
#define RF_BZ_TX_BACKOFF_13_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_13_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_13_LEN)-1)<<RF_BZ_TX_BACKOFF_13_POS)
#define RF_BZ_TX_BACKOFF_13_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_13_LEN)-1)<<RF_BZ_TX_BACKOFF_13_POS))
#define RF_BZ_TX_BACKOFF_14                                     RF_BZ_TX_BACKOFF_14
#define RF_BZ_TX_BACKOFF_14_POS                                 (24U)
#define RF_BZ_TX_BACKOFF_14_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_14_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_14_LEN)-1)<<RF_BZ_TX_BACKOFF_14_POS)
#define RF_BZ_TX_BACKOFF_14_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_14_LEN)-1)<<RF_BZ_TX_BACKOFF_14_POS))
#define RF_BZ_TX_BACKOFF_15                                     RF_BZ_TX_BACKOFF_15
#define RF_BZ_TX_BACKOFF_15_POS                                 (28U)
#define RF_BZ_TX_BACKOFF_15_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_15_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_15_LEN)-1)<<RF_BZ_TX_BACKOFF_15_POS)
#define RF_BZ_TX_BACKOFF_15_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_15_LEN)-1)<<RF_BZ_TX_BACKOFF_15_POS))

/* 0x77C : pwr_bz_ctrl_2 */
#define RF_PWR_BZ_CTRL_2_OFFSET                                 (0x77C)
#define RF_BZ_TX_BACKOFF_16                                     RF_BZ_TX_BACKOFF_16
#define RF_BZ_TX_BACKOFF_16_POS                                 (0U)
#define RF_BZ_TX_BACKOFF_16_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_16_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_16_LEN)-1)<<RF_BZ_TX_BACKOFF_16_POS)
#define RF_BZ_TX_BACKOFF_16_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_16_LEN)-1)<<RF_BZ_TX_BACKOFF_16_POS))
#define RF_BZ_TX_BACKOFF_17                                     RF_BZ_TX_BACKOFF_17
#define RF_BZ_TX_BACKOFF_17_POS                                 (4U)
#define RF_BZ_TX_BACKOFF_17_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_17_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_17_LEN)-1)<<RF_BZ_TX_BACKOFF_17_POS)
#define RF_BZ_TX_BACKOFF_17_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_17_LEN)-1)<<RF_BZ_TX_BACKOFF_17_POS))
#define RF_BZ_TX_BACKOFF_18                                     RF_BZ_TX_BACKOFF_18
#define RF_BZ_TX_BACKOFF_18_POS                                 (8U)
#define RF_BZ_TX_BACKOFF_18_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_18_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_18_LEN)-1)<<RF_BZ_TX_BACKOFF_18_POS)
#define RF_BZ_TX_BACKOFF_18_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_18_LEN)-1)<<RF_BZ_TX_BACKOFF_18_POS))
#define RF_BZ_TX_BACKOFF_19                                     RF_BZ_TX_BACKOFF_19
#define RF_BZ_TX_BACKOFF_19_POS                                 (12U)
#define RF_BZ_TX_BACKOFF_19_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_19_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_19_LEN)-1)<<RF_BZ_TX_BACKOFF_19_POS)
#define RF_BZ_TX_BACKOFF_19_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_19_LEN)-1)<<RF_BZ_TX_BACKOFF_19_POS))
#define RF_BZ_TX_BACKOFF_20                                     RF_BZ_TX_BACKOFF_20
#define RF_BZ_TX_BACKOFF_20_POS                                 (16U)
#define RF_BZ_TX_BACKOFF_20_LEN                                 (4U)
#define RF_BZ_TX_BACKOFF_20_MSK                                 (((1U<<RF_BZ_TX_BACKOFF_20_LEN)-1)<<RF_BZ_TX_BACKOFF_20_POS)
#define RF_BZ_TX_BACKOFF_20_UMSK                                (~(((1U<<RF_BZ_TX_BACKOFF_20_LEN)-1)<<RF_BZ_TX_BACKOFF_20_POS))

/* 0x780 : pwr_bz_ctrl_3 */
#define RF_PWR_BZ_CTRL_3_OFFSET                                 (0x780)
#define RF_BZ_TX_CORR_0                                         RF_BZ_TX_CORR_0
#define RF_BZ_TX_CORR_0_POS                                     (0U)
#define RF_BZ_TX_CORR_0_LEN                                     (6U)
#define RF_BZ_TX_CORR_0_MSK                                     (((1U<<RF_BZ_TX_CORR_0_LEN)-1)<<RF_BZ_TX_CORR_0_POS)
#define RF_BZ_TX_CORR_0_UMSK                                    (~(((1U<<RF_BZ_TX_CORR_0_LEN)-1)<<RF_BZ_TX_CORR_0_POS))
#define RF_BZ_TX_CORR_1                                         RF_BZ_TX_CORR_1
#define RF_BZ_TX_CORR_1_POS                                     (6U)
#define RF_BZ_TX_CORR_1_LEN                                     (6U)
#define RF_BZ_TX_CORR_1_MSK                                     (((1U<<RF_BZ_TX_CORR_1_LEN)-1)<<RF_BZ_TX_CORR_1_POS)
#define RF_BZ_TX_CORR_1_UMSK                                    (~(((1U<<RF_BZ_TX_CORR_1_LEN)-1)<<RF_BZ_TX_CORR_1_POS))
#define RF_BZ_TX_CORR_2                                         RF_BZ_TX_CORR_2
#define RF_BZ_TX_CORR_2_POS                                     (12U)
#define RF_BZ_TX_CORR_2_LEN                                     (6U)
#define RF_BZ_TX_CORR_2_MSK                                     (((1U<<RF_BZ_TX_CORR_2_LEN)-1)<<RF_BZ_TX_CORR_2_POS)
#define RF_BZ_TX_CORR_2_UMSK                                    (~(((1U<<RF_BZ_TX_CORR_2_LEN)-1)<<RF_BZ_TX_CORR_2_POS))
#define RF_BZ_TX_CORR_3                                         RF_BZ_TX_CORR_3
#define RF_BZ_TX_CORR_3_POS                                     (18U)
#define RF_BZ_TX_CORR_3_LEN                                     (6U)
#define RF_BZ_TX_CORR_3_MSK                                     (((1U<<RF_BZ_TX_CORR_3_LEN)-1)<<RF_BZ_TX_CORR_3_POS)
#define RF_BZ_TX_CORR_3_UMSK                                    (~(((1U<<RF_BZ_TX_CORR_3_LEN)-1)<<RF_BZ_TX_CORR_3_POS))
#define RF_BZ_TX_CORR_4                                         RF_BZ_TX_CORR_4
#define RF_BZ_TX_CORR_4_POS                                     (24U)
#define RF_BZ_TX_CORR_4_LEN                                     (6U)
#define RF_BZ_TX_CORR_4_MSK                                     (((1U<<RF_BZ_TX_CORR_4_LEN)-1)<<RF_BZ_TX_CORR_4_POS)
#define RF_BZ_TX_CORR_4_UMSK                                    (~(((1U<<RF_BZ_TX_CORR_4_LEN)-1)<<RF_BZ_TX_CORR_4_POS))

/* 0x784 : pwr_bz_ctrl_4 */
#define RF_PWR_BZ_CTRL_4_OFFSET                                 (0x784)
#define RF_BZ_TX_TEMP_0                                         RF_BZ_TX_TEMP_0
#define RF_BZ_TX_TEMP_0_POS                                     (0U)
#define RF_BZ_TX_TEMP_0_LEN                                     (6U)
#define RF_BZ_TX_TEMP_0_MSK                                     (((1U<<RF_BZ_TX_TEMP_0_LEN)-1)<<RF_BZ_TX_TEMP_0_POS)
#define RF_BZ_TX_TEMP_0_UMSK                                    (~(((1U<<RF_BZ_TX_TEMP_0_LEN)-1)<<RF_BZ_TX_TEMP_0_POS))
#define RF_BZ_TX_TEMP_1                                         RF_BZ_TX_TEMP_1
#define RF_BZ_TX_TEMP_1_POS                                     (6U)
#define RF_BZ_TX_TEMP_1_LEN                                     (6U)
#define RF_BZ_TX_TEMP_1_MSK                                     (((1U<<RF_BZ_TX_TEMP_1_LEN)-1)<<RF_BZ_TX_TEMP_1_POS)
#define RF_BZ_TX_TEMP_1_UMSK                                    (~(((1U<<RF_BZ_TX_TEMP_1_LEN)-1)<<RF_BZ_TX_TEMP_1_POS))
#define RF_BZ_TX_TEMP_2                                         RF_BZ_TX_TEMP_2
#define RF_BZ_TX_TEMP_2_POS                                     (12U)
#define RF_BZ_TX_TEMP_2_LEN                                     (6U)
#define RF_BZ_TX_TEMP_2_MSK                                     (((1U<<RF_BZ_TX_TEMP_2_LEN)-1)<<RF_BZ_TX_TEMP_2_POS)
#define RF_BZ_TX_TEMP_2_UMSK                                    (~(((1U<<RF_BZ_TX_TEMP_2_LEN)-1)<<RF_BZ_TX_TEMP_2_POS))
#define RF_BZ_TX_TEMP_3                                         RF_BZ_TX_TEMP_3
#define RF_BZ_TX_TEMP_3_POS                                     (18U)
#define RF_BZ_TX_TEMP_3_LEN                                     (6U)
#define RF_BZ_TX_TEMP_3_MSK                                     (((1U<<RF_BZ_TX_TEMP_3_LEN)-1)<<RF_BZ_TX_TEMP_3_POS)
#define RF_BZ_TX_TEMP_3_UMSK                                    (~(((1U<<RF_BZ_TX_TEMP_3_LEN)-1)<<RF_BZ_TX_TEMP_3_POS))
#define RF_BZ_TX_TEMP_4                                         RF_BZ_TX_TEMP_4
#define RF_BZ_TX_TEMP_4_POS                                     (24U)
#define RF_BZ_TX_TEMP_4_LEN                                     (6U)
#define RF_BZ_TX_TEMP_4_MSK                                     (((1U<<RF_BZ_TX_TEMP_4_LEN)-1)<<RF_BZ_TX_TEMP_4_POS)
#define RF_BZ_TX_TEMP_4_UMSK                                    (~(((1U<<RF_BZ_TX_TEMP_4_LEN)-1)<<RF_BZ_TX_TEMP_4_POS))

/* 0x788 : pwr_bz_ctrl_5 */
#define RF_PWR_BZ_CTRL_5_OFFSET                                 (0x788)
#define RF_BZ_NOMINAL_PWR_0                                     RF_BZ_NOMINAL_PWR_0
#define RF_BZ_NOMINAL_PWR_0_POS                                 (0U)
#define RF_BZ_NOMINAL_PWR_0_LEN                                 (6U)
#define RF_BZ_NOMINAL_PWR_0_MSK                                 (((1U<<RF_BZ_NOMINAL_PWR_0_LEN)-1)<<RF_BZ_NOMINAL_PWR_0_POS)
#define RF_BZ_NOMINAL_PWR_0_UMSK                                (~(((1U<<RF_BZ_NOMINAL_PWR_0_LEN)-1)<<RF_BZ_NOMINAL_PWR_0_POS))
#define RF_BZ_NOMINAL_PWR_1                                     RF_BZ_NOMINAL_PWR_1
#define RF_BZ_NOMINAL_PWR_1_POS                                 (6U)
#define RF_BZ_NOMINAL_PWR_1_LEN                                 (6U)
#define RF_BZ_NOMINAL_PWR_1_MSK                                 (((1U<<RF_BZ_NOMINAL_PWR_1_LEN)-1)<<RF_BZ_NOMINAL_PWR_1_POS)
#define RF_BZ_NOMINAL_PWR_1_UMSK                                (~(((1U<<RF_BZ_NOMINAL_PWR_1_LEN)-1)<<RF_BZ_NOMINAL_PWR_1_POS))
#define RF_BZ_NOMINAL_PWR_2                                     RF_BZ_NOMINAL_PWR_2
#define RF_BZ_NOMINAL_PWR_2_POS                                 (12U)
#define RF_BZ_NOMINAL_PWR_2_LEN                                 (6U)
#define RF_BZ_NOMINAL_PWR_2_MSK                                 (((1U<<RF_BZ_NOMINAL_PWR_2_LEN)-1)<<RF_BZ_NOMINAL_PWR_2_POS)
#define RF_BZ_NOMINAL_PWR_2_UMSK                                (~(((1U<<RF_BZ_NOMINAL_PWR_2_LEN)-1)<<RF_BZ_NOMINAL_PWR_2_POS))
#define RF_BZ_NOMINAL_PWR_3                                     RF_BZ_NOMINAL_PWR_3
#define RF_BZ_NOMINAL_PWR_3_POS                                 (18U)
#define RF_BZ_NOMINAL_PWR_3_LEN                                 (6U)
#define RF_BZ_NOMINAL_PWR_3_MSK                                 (((1U<<RF_BZ_NOMINAL_PWR_3_LEN)-1)<<RF_BZ_NOMINAL_PWR_3_POS)
#define RF_BZ_NOMINAL_PWR_3_UMSK                                (~(((1U<<RF_BZ_NOMINAL_PWR_3_LEN)-1)<<RF_BZ_NOMINAL_PWR_3_POS))

/* 0x78C : pwr_bz_tbl_0 */
#define RF_PWR_BZ_TBL_0_OFFSET                                  (0x78C)
#define RF_BZ_IQC_GAIN_0                                        RF_BZ_IQC_GAIN_0
#define RF_BZ_IQC_GAIN_0_POS                                    (0U)
#define RF_BZ_IQC_GAIN_0_LEN                                    (11U)
#define RF_BZ_IQC_GAIN_0_MSK                                    (((1U<<RF_BZ_IQC_GAIN_0_LEN)-1)<<RF_BZ_IQC_GAIN_0_POS)
#define RF_BZ_IQC_GAIN_0_UMSK                                   (~(((1U<<RF_BZ_IQC_GAIN_0_LEN)-1)<<RF_BZ_IQC_GAIN_0_POS))
#define RF_BZ_DG1_QDB_0                                         RF_BZ_DG1_QDB_0
#define RF_BZ_DG1_QDB_0_POS                                     (11U)
#define RF_BZ_DG1_QDB_0_LEN                                     (7U)
#define RF_BZ_DG1_QDB_0_MSK                                     (((1U<<RF_BZ_DG1_QDB_0_LEN)-1)<<RF_BZ_DG1_QDB_0_POS)
#define RF_BZ_DG1_QDB_0_UMSK                                    (~(((1U<<RF_BZ_DG1_QDB_0_LEN)-1)<<RF_BZ_DG1_QDB_0_POS))
#define RF_BZ_GC_TMX_0                                          RF_BZ_GC_TMX_0
#define RF_BZ_GC_TMX_0_POS                                      (18U)
#define RF_BZ_GC_TMX_0_LEN                                      (3U)
#define RF_BZ_GC_TMX_0_MSK                                      (((1U<<RF_BZ_GC_TMX_0_LEN)-1)<<RF_BZ_GC_TMX_0_POS)
#define RF_BZ_GC_TMX_0_UMSK                                     (~(((1U<<RF_BZ_GC_TMX_0_LEN)-1)<<RF_BZ_GC_TMX_0_POS))
#define RF_BZ_GC_TBB_0                                          RF_BZ_GC_TBB_0
#define RF_BZ_GC_TBB_0_POS                                      (21U)
#define RF_BZ_GC_TBB_0_LEN                                      (5U)
#define RF_BZ_GC_TBB_0_MSK                                      (((1U<<RF_BZ_GC_TBB_0_LEN)-1)<<RF_BZ_GC_TBB_0_POS)
#define RF_BZ_GC_TBB_0_UMSK                                     (~(((1U<<RF_BZ_GC_TBB_0_LEN)-1)<<RF_BZ_GC_TBB_0_POS))
#define RF_BZ_GC_TBB_BOOST_0                                    RF_BZ_GC_TBB_BOOST_0
#define RF_BZ_GC_TBB_BOOST_0_POS                                (26U)
#define RF_BZ_GC_TBB_BOOST_0_LEN                                (2U)
#define RF_BZ_GC_TBB_BOOST_0_MSK                                (((1U<<RF_BZ_GC_TBB_BOOST_0_LEN)-1)<<RF_BZ_GC_TBB_BOOST_0_POS)
#define RF_BZ_GC_TBB_BOOST_0_UMSK                               (~(((1U<<RF_BZ_GC_TBB_BOOST_0_LEN)-1)<<RF_BZ_GC_TBB_BOOST_0_POS))
#define RF_BZ_DAC_BIAS_0                                        RF_BZ_DAC_BIAS_0
#define RF_BZ_DAC_BIAS_0_POS                                    (28U)
#define RF_BZ_DAC_BIAS_0_LEN                                    (2U)
#define RF_BZ_DAC_BIAS_0_MSK                                    (((1U<<RF_BZ_DAC_BIAS_0_LEN)-1)<<RF_BZ_DAC_BIAS_0_POS)
#define RF_BZ_DAC_BIAS_0_UMSK                                   (~(((1U<<RF_BZ_DAC_BIAS_0_LEN)-1)<<RF_BZ_DAC_BIAS_0_POS))

/* 0x790 : pwr_bz_tbl_1 */
#define RF_PWR_BZ_TBL_1_OFFSET                                  (0x790)
#define RF_BZ_DPD_SEL_0                                         RF_BZ_DPD_SEL_0
#define RF_BZ_DPD_SEL_0_POS                                     (0U)
#define RF_BZ_DPD_SEL_0_LEN                                     (2U)
#define RF_BZ_DPD_SEL_0_MSK                                     (((1U<<RF_BZ_DPD_SEL_0_LEN)-1)<<RF_BZ_DPD_SEL_0_POS)
#define RF_BZ_DPD_SEL_0_UMSK                                    (~(((1U<<RF_BZ_DPD_SEL_0_LEN)-1)<<RF_BZ_DPD_SEL_0_POS))
#define RF_BZ_TOSDAC_Q_0                                        RF_BZ_TOSDAC_Q_0
#define RF_BZ_TOSDAC_Q_0_POS                                    (2U)
#define RF_BZ_TOSDAC_Q_0_LEN                                    (6U)
#define RF_BZ_TOSDAC_Q_0_MSK                                    (((1U<<RF_BZ_TOSDAC_Q_0_LEN)-1)<<RF_BZ_TOSDAC_Q_0_POS)
#define RF_BZ_TOSDAC_Q_0_UMSK                                   (~(((1U<<RF_BZ_TOSDAC_Q_0_LEN)-1)<<RF_BZ_TOSDAC_Q_0_POS))
#define RF_BZ_TOSDAC_I_0                                        RF_BZ_TOSDAC_I_0
#define RF_BZ_TOSDAC_I_0_POS                                    (8U)
#define RF_BZ_TOSDAC_I_0_LEN                                    (6U)
#define RF_BZ_TOSDAC_I_0_MSK                                    (((1U<<RF_BZ_TOSDAC_I_0_LEN)-1)<<RF_BZ_TOSDAC_I_0_POS)
#define RF_BZ_TOSDAC_I_0_UMSK                                   (~(((1U<<RF_BZ_TOSDAC_I_0_LEN)-1)<<RF_BZ_TOSDAC_I_0_POS))
#define RF_BZ_IQC_PHASE_0                                       RF_BZ_IQC_PHASE_0
#define RF_BZ_IQC_PHASE_0_POS                                   (14U)
#define RF_BZ_IQC_PHASE_0_LEN                                   (10U)
#define RF_BZ_IQC_PHASE_0_MSK                                   (((1U<<RF_BZ_IQC_PHASE_0_LEN)-1)<<RF_BZ_IQC_PHASE_0_POS)
#define RF_BZ_IQC_PHASE_0_UMSK                                  (~(((1U<<RF_BZ_IQC_PHASE_0_LEN)-1)<<RF_BZ_IQC_PHASE_0_POS))
#define RF_BZ_GC_PA_0                                           RF_BZ_GC_PA_0
#define RF_BZ_GC_PA_0_POS                                       (24U)
#define RF_BZ_GC_PA_0_LEN                                       (3U)
#define RF_BZ_GC_PA_0_MSK                                       (((1U<<RF_BZ_GC_PA_0_LEN)-1)<<RF_BZ_GC_PA_0_POS)
#define RF_BZ_GC_PA_0_UMSK                                      (~(((1U<<RF_BZ_GC_PA_0_LEN)-1)<<RF_BZ_GC_PA_0_POS))
#define RF_BZ_PA_LP_EN_0                                        RF_BZ_PA_LP_EN_0
#define RF_BZ_PA_LP_EN_0_POS                                    (27U)
#define RF_BZ_PA_LP_EN_0_LEN                                    (1U)
#define RF_BZ_PA_LP_EN_0_MSK                                    (((1U<<RF_BZ_PA_LP_EN_0_LEN)-1)<<RF_BZ_PA_LP_EN_0_POS)
#define RF_BZ_PA_LP_EN_0_UMSK                                   (~(((1U<<RF_BZ_PA_LP_EN_0_LEN)-1)<<RF_BZ_PA_LP_EN_0_POS))
#define RF_BZ_DPD_EN_0                                          RF_BZ_DPD_EN_0
#define RF_BZ_DPD_EN_0_POS                                      (28U)
#define RF_BZ_DPD_EN_0_LEN                                      (1U)
#define RF_BZ_DPD_EN_0_MSK                                      (((1U<<RF_BZ_DPD_EN_0_LEN)-1)<<RF_BZ_DPD_EN_0_POS)
#define RF_BZ_DPD_EN_0_UMSK                                     (~(((1U<<RF_BZ_DPD_EN_0_LEN)-1)<<RF_BZ_DPD_EN_0_POS))

/* 0x794 : pwr_bz_tbl_2 */
#define RF_PWR_BZ_TBL_2_OFFSET                                  (0x794)
#define RF_BZ_IQC_GAIN_1                                        RF_BZ_IQC_GAIN_1
#define RF_BZ_IQC_GAIN_1_POS                                    (0U)
#define RF_BZ_IQC_GAIN_1_LEN                                    (11U)
#define RF_BZ_IQC_GAIN_1_MSK                                    (((1U<<RF_BZ_IQC_GAIN_1_LEN)-1)<<RF_BZ_IQC_GAIN_1_POS)
#define RF_BZ_IQC_GAIN_1_UMSK                                   (~(((1U<<RF_BZ_IQC_GAIN_1_LEN)-1)<<RF_BZ_IQC_GAIN_1_POS))
#define RF_BZ_DG1_QDB_1                                         RF_BZ_DG1_QDB_1
#define RF_BZ_DG1_QDB_1_POS                                     (11U)
#define RF_BZ_DG1_QDB_1_LEN                                     (7U)
#define RF_BZ_DG1_QDB_1_MSK                                     (((1U<<RF_BZ_DG1_QDB_1_LEN)-1)<<RF_BZ_DG1_QDB_1_POS)
#define RF_BZ_DG1_QDB_1_UMSK                                    (~(((1U<<RF_BZ_DG1_QDB_1_LEN)-1)<<RF_BZ_DG1_QDB_1_POS))
#define RF_BZ_GC_TMX_1                                          RF_BZ_GC_TMX_1
#define RF_BZ_GC_TMX_1_POS                                      (18U)
#define RF_BZ_GC_TMX_1_LEN                                      (3U)
#define RF_BZ_GC_TMX_1_MSK                                      (((1U<<RF_BZ_GC_TMX_1_LEN)-1)<<RF_BZ_GC_TMX_1_POS)
#define RF_BZ_GC_TMX_1_UMSK                                     (~(((1U<<RF_BZ_GC_TMX_1_LEN)-1)<<RF_BZ_GC_TMX_1_POS))
#define RF_BZ_GC_TBB_1                                          RF_BZ_GC_TBB_1
#define RF_BZ_GC_TBB_1_POS                                      (21U)
#define RF_BZ_GC_TBB_1_LEN                                      (5U)
#define RF_BZ_GC_TBB_1_MSK                                      (((1U<<RF_BZ_GC_TBB_1_LEN)-1)<<RF_BZ_GC_TBB_1_POS)
#define RF_BZ_GC_TBB_1_UMSK                                     (~(((1U<<RF_BZ_GC_TBB_1_LEN)-1)<<RF_BZ_GC_TBB_1_POS))
#define RF_BZ_GC_TBB_BOOST_1                                    RF_BZ_GC_TBB_BOOST_1
#define RF_BZ_GC_TBB_BOOST_1_POS                                (26U)
#define RF_BZ_GC_TBB_BOOST_1_LEN                                (2U)
#define RF_BZ_GC_TBB_BOOST_1_MSK                                (((1U<<RF_BZ_GC_TBB_BOOST_1_LEN)-1)<<RF_BZ_GC_TBB_BOOST_1_POS)
#define RF_BZ_GC_TBB_BOOST_1_UMSK                               (~(((1U<<RF_BZ_GC_TBB_BOOST_1_LEN)-1)<<RF_BZ_GC_TBB_BOOST_1_POS))
#define RF_BZ_DAC_BIAS_1                                        RF_BZ_DAC_BIAS_1
#define RF_BZ_DAC_BIAS_1_POS                                    (28U)
#define RF_BZ_DAC_BIAS_1_LEN                                    (2U)
#define RF_BZ_DAC_BIAS_1_MSK                                    (((1U<<RF_BZ_DAC_BIAS_1_LEN)-1)<<RF_BZ_DAC_BIAS_1_POS)
#define RF_BZ_DAC_BIAS_1_UMSK                                   (~(((1U<<RF_BZ_DAC_BIAS_1_LEN)-1)<<RF_BZ_DAC_BIAS_1_POS))

/* 0x798 : pwr_bz_tbl_3 */
#define RF_PWR_BZ_TBL_3_OFFSET                                  (0x798)
#define RF_BZ_DPD_SEL_1                                         RF_BZ_DPD_SEL_1
#define RF_BZ_DPD_SEL_1_POS                                     (0U)
#define RF_BZ_DPD_SEL_1_LEN                                     (2U)
#define RF_BZ_DPD_SEL_1_MSK                                     (((1U<<RF_BZ_DPD_SEL_1_LEN)-1)<<RF_BZ_DPD_SEL_1_POS)
#define RF_BZ_DPD_SEL_1_UMSK                                    (~(((1U<<RF_BZ_DPD_SEL_1_LEN)-1)<<RF_BZ_DPD_SEL_1_POS))
#define RF_BZ_TOSDAC_Q_1                                        RF_BZ_TOSDAC_Q_1
#define RF_BZ_TOSDAC_Q_1_POS                                    (2U)
#define RF_BZ_TOSDAC_Q_1_LEN                                    (6U)
#define RF_BZ_TOSDAC_Q_1_MSK                                    (((1U<<RF_BZ_TOSDAC_Q_1_LEN)-1)<<RF_BZ_TOSDAC_Q_1_POS)
#define RF_BZ_TOSDAC_Q_1_UMSK                                   (~(((1U<<RF_BZ_TOSDAC_Q_1_LEN)-1)<<RF_BZ_TOSDAC_Q_1_POS))
#define RF_BZ_TOSDAC_I_1                                        RF_BZ_TOSDAC_I_1
#define RF_BZ_TOSDAC_I_1_POS                                    (8U)
#define RF_BZ_TOSDAC_I_1_LEN                                    (6U)
#define RF_BZ_TOSDAC_I_1_MSK                                    (((1U<<RF_BZ_TOSDAC_I_1_LEN)-1)<<RF_BZ_TOSDAC_I_1_POS)
#define RF_BZ_TOSDAC_I_1_UMSK                                   (~(((1U<<RF_BZ_TOSDAC_I_1_LEN)-1)<<RF_BZ_TOSDAC_I_1_POS))
#define RF_BZ_IQC_PHASE_1                                       RF_BZ_IQC_PHASE_1
#define RF_BZ_IQC_PHASE_1_POS                                   (14U)
#define RF_BZ_IQC_PHASE_1_LEN                                   (10U)
#define RF_BZ_IQC_PHASE_1_MSK                                   (((1U<<RF_BZ_IQC_PHASE_1_LEN)-1)<<RF_BZ_IQC_PHASE_1_POS)
#define RF_BZ_IQC_PHASE_1_UMSK                                  (~(((1U<<RF_BZ_IQC_PHASE_1_LEN)-1)<<RF_BZ_IQC_PHASE_1_POS))
#define RF_BZ_GC_PA_1                                           RF_BZ_GC_PA_1
#define RF_BZ_GC_PA_1_POS                                       (24U)
#define RF_BZ_GC_PA_1_LEN                                       (3U)
#define RF_BZ_GC_PA_1_MSK                                       (((1U<<RF_BZ_GC_PA_1_LEN)-1)<<RF_BZ_GC_PA_1_POS)
#define RF_BZ_GC_PA_1_UMSK                                      (~(((1U<<RF_BZ_GC_PA_1_LEN)-1)<<RF_BZ_GC_PA_1_POS))
#define RF_BZ_PA_LP_EN_1                                        RF_BZ_PA_LP_EN_1
#define RF_BZ_PA_LP_EN_1_POS                                    (27U)
#define RF_BZ_PA_LP_EN_1_LEN                                    (1U)
#define RF_BZ_PA_LP_EN_1_MSK                                    (((1U<<RF_BZ_PA_LP_EN_1_LEN)-1)<<RF_BZ_PA_LP_EN_1_POS)
#define RF_BZ_PA_LP_EN_1_UMSK                                   (~(((1U<<RF_BZ_PA_LP_EN_1_LEN)-1)<<RF_BZ_PA_LP_EN_1_POS))
#define RF_BZ_DPD_EN_1                                          RF_BZ_DPD_EN_1
#define RF_BZ_DPD_EN_1_POS                                      (28U)
#define RF_BZ_DPD_EN_1_LEN                                      (1U)
#define RF_BZ_DPD_EN_1_MSK                                      (((1U<<RF_BZ_DPD_EN_1_LEN)-1)<<RF_BZ_DPD_EN_1_POS)
#define RF_BZ_DPD_EN_1_UMSK                                     (~(((1U<<RF_BZ_DPD_EN_1_LEN)-1)<<RF_BZ_DPD_EN_1_POS))

/* 0x79C : pwr_bz_tbl_4 */
#define RF_PWR_BZ_TBL_4_OFFSET                                  (0x79C)
#define RF_BZ_IQC_GAIN_2                                        RF_BZ_IQC_GAIN_2
#define RF_BZ_IQC_GAIN_2_POS                                    (0U)
#define RF_BZ_IQC_GAIN_2_LEN                                    (11U)
#define RF_BZ_IQC_GAIN_2_MSK                                    (((1U<<RF_BZ_IQC_GAIN_2_LEN)-1)<<RF_BZ_IQC_GAIN_2_POS)
#define RF_BZ_IQC_GAIN_2_UMSK                                   (~(((1U<<RF_BZ_IQC_GAIN_2_LEN)-1)<<RF_BZ_IQC_GAIN_2_POS))
#define RF_BZ_DG1_QDB_2                                         RF_BZ_DG1_QDB_2
#define RF_BZ_DG1_QDB_2_POS                                     (11U)
#define RF_BZ_DG1_QDB_2_LEN                                     (7U)
#define RF_BZ_DG1_QDB_2_MSK                                     (((1U<<RF_BZ_DG1_QDB_2_LEN)-1)<<RF_BZ_DG1_QDB_2_POS)
#define RF_BZ_DG1_QDB_2_UMSK                                    (~(((1U<<RF_BZ_DG1_QDB_2_LEN)-1)<<RF_BZ_DG1_QDB_2_POS))
#define RF_BZ_GC_TMX_2                                          RF_BZ_GC_TMX_2
#define RF_BZ_GC_TMX_2_POS                                      (18U)
#define RF_BZ_GC_TMX_2_LEN                                      (3U)
#define RF_BZ_GC_TMX_2_MSK                                      (((1U<<RF_BZ_GC_TMX_2_LEN)-1)<<RF_BZ_GC_TMX_2_POS)
#define RF_BZ_GC_TMX_2_UMSK                                     (~(((1U<<RF_BZ_GC_TMX_2_LEN)-1)<<RF_BZ_GC_TMX_2_POS))
#define RF_BZ_GC_TBB_2                                          RF_BZ_GC_TBB_2
#define RF_BZ_GC_TBB_2_POS                                      (21U)
#define RF_BZ_GC_TBB_2_LEN                                      (5U)
#define RF_BZ_GC_TBB_2_MSK                                      (((1U<<RF_BZ_GC_TBB_2_LEN)-1)<<RF_BZ_GC_TBB_2_POS)
#define RF_BZ_GC_TBB_2_UMSK                                     (~(((1U<<RF_BZ_GC_TBB_2_LEN)-1)<<RF_BZ_GC_TBB_2_POS))
#define RF_BZ_GC_TBB_BOOST_2                                    RF_BZ_GC_TBB_BOOST_2
#define RF_BZ_GC_TBB_BOOST_2_POS                                (26U)
#define RF_BZ_GC_TBB_BOOST_2_LEN                                (2U)
#define RF_BZ_GC_TBB_BOOST_2_MSK                                (((1U<<RF_BZ_GC_TBB_BOOST_2_LEN)-1)<<RF_BZ_GC_TBB_BOOST_2_POS)
#define RF_BZ_GC_TBB_BOOST_2_UMSK                               (~(((1U<<RF_BZ_GC_TBB_BOOST_2_LEN)-1)<<RF_BZ_GC_TBB_BOOST_2_POS))
#define RF_BZ_DAC_BIAS_2                                        RF_BZ_DAC_BIAS_2
#define RF_BZ_DAC_BIAS_2_POS                                    (28U)
#define RF_BZ_DAC_BIAS_2_LEN                                    (2U)
#define RF_BZ_DAC_BIAS_2_MSK                                    (((1U<<RF_BZ_DAC_BIAS_2_LEN)-1)<<RF_BZ_DAC_BIAS_2_POS)
#define RF_BZ_DAC_BIAS_2_UMSK                                   (~(((1U<<RF_BZ_DAC_BIAS_2_LEN)-1)<<RF_BZ_DAC_BIAS_2_POS))

/* 0x7A0 : pwr_bz_tbl_6 */
#define RF_PWR_BZ_TBL_6_OFFSET                                  (0x7A0)
#define RF_BZ_DPD_SEL_2                                         RF_BZ_DPD_SEL_2
#define RF_BZ_DPD_SEL_2_POS                                     (0U)
#define RF_BZ_DPD_SEL_2_LEN                                     (2U)
#define RF_BZ_DPD_SEL_2_MSK                                     (((1U<<RF_BZ_DPD_SEL_2_LEN)-1)<<RF_BZ_DPD_SEL_2_POS)
#define RF_BZ_DPD_SEL_2_UMSK                                    (~(((1U<<RF_BZ_DPD_SEL_2_LEN)-1)<<RF_BZ_DPD_SEL_2_POS))
#define RF_BZ_TOSDAC_Q_2                                        RF_BZ_TOSDAC_Q_2
#define RF_BZ_TOSDAC_Q_2_POS                                    (2U)
#define RF_BZ_TOSDAC_Q_2_LEN                                    (6U)
#define RF_BZ_TOSDAC_Q_2_MSK                                    (((1U<<RF_BZ_TOSDAC_Q_2_LEN)-1)<<RF_BZ_TOSDAC_Q_2_POS)
#define RF_BZ_TOSDAC_Q_2_UMSK                                   (~(((1U<<RF_BZ_TOSDAC_Q_2_LEN)-1)<<RF_BZ_TOSDAC_Q_2_POS))
#define RF_BZ_TOSDAC_I_2                                        RF_BZ_TOSDAC_I_2
#define RF_BZ_TOSDAC_I_2_POS                                    (8U)
#define RF_BZ_TOSDAC_I_2_LEN                                    (6U)
#define RF_BZ_TOSDAC_I_2_MSK                                    (((1U<<RF_BZ_TOSDAC_I_2_LEN)-1)<<RF_BZ_TOSDAC_I_2_POS)
#define RF_BZ_TOSDAC_I_2_UMSK                                   (~(((1U<<RF_BZ_TOSDAC_I_2_LEN)-1)<<RF_BZ_TOSDAC_I_2_POS))
#define RF_BZ_IQC_PHASE_2                                       RF_BZ_IQC_PHASE_2
#define RF_BZ_IQC_PHASE_2_POS                                   (14U)
#define RF_BZ_IQC_PHASE_2_LEN                                   (10U)
#define RF_BZ_IQC_PHASE_2_MSK                                   (((1U<<RF_BZ_IQC_PHASE_2_LEN)-1)<<RF_BZ_IQC_PHASE_2_POS)
#define RF_BZ_IQC_PHASE_2_UMSK                                  (~(((1U<<RF_BZ_IQC_PHASE_2_LEN)-1)<<RF_BZ_IQC_PHASE_2_POS))
#define RF_BZ_GC_PA_2                                           RF_BZ_GC_PA_2
#define RF_BZ_GC_PA_2_POS                                       (24U)
#define RF_BZ_GC_PA_2_LEN                                       (3U)
#define RF_BZ_GC_PA_2_MSK                                       (((1U<<RF_BZ_GC_PA_2_LEN)-1)<<RF_BZ_GC_PA_2_POS)
#define RF_BZ_GC_PA_2_UMSK                                      (~(((1U<<RF_BZ_GC_PA_2_LEN)-1)<<RF_BZ_GC_PA_2_POS))
#define RF_BZ_PA_LP_EN_2                                        RF_BZ_PA_LP_EN_2
#define RF_BZ_PA_LP_EN_2_POS                                    (27U)
#define RF_BZ_PA_LP_EN_2_LEN                                    (1U)
#define RF_BZ_PA_LP_EN_2_MSK                                    (((1U<<RF_BZ_PA_LP_EN_2_LEN)-1)<<RF_BZ_PA_LP_EN_2_POS)
#define RF_BZ_PA_LP_EN_2_UMSK                                   (~(((1U<<RF_BZ_PA_LP_EN_2_LEN)-1)<<RF_BZ_PA_LP_EN_2_POS))
#define RF_BZ_DPD_EN_2                                          RF_BZ_DPD_EN_2
#define RF_BZ_DPD_EN_2_POS                                      (28U)
#define RF_BZ_DPD_EN_2_LEN                                      (1U)
#define RF_BZ_DPD_EN_2_MSK                                      (((1U<<RF_BZ_DPD_EN_2_LEN)-1)<<RF_BZ_DPD_EN_2_POS)
#define RF_BZ_DPD_EN_2_UMSK                                     (~(((1U<<RF_BZ_DPD_EN_2_LEN)-1)<<RF_BZ_DPD_EN_2_POS))

/* 0x7A4 : pwr_bz_tbl_7 */
#define RF_PWR_BZ_TBL_7_OFFSET                                  (0x7A4)
#define RF_BZ_IQC_GAIN_3                                        RF_BZ_IQC_GAIN_3
#define RF_BZ_IQC_GAIN_3_POS                                    (0U)
#define RF_BZ_IQC_GAIN_3_LEN                                    (11U)
#define RF_BZ_IQC_GAIN_3_MSK                                    (((1U<<RF_BZ_IQC_GAIN_3_LEN)-1)<<RF_BZ_IQC_GAIN_3_POS)
#define RF_BZ_IQC_GAIN_3_UMSK                                   (~(((1U<<RF_BZ_IQC_GAIN_3_LEN)-1)<<RF_BZ_IQC_GAIN_3_POS))
#define RF_BZ_DG1_QDB_3                                         RF_BZ_DG1_QDB_3
#define RF_BZ_DG1_QDB_3_POS                                     (11U)
#define RF_BZ_DG1_QDB_3_LEN                                     (7U)
#define RF_BZ_DG1_QDB_3_MSK                                     (((1U<<RF_BZ_DG1_QDB_3_LEN)-1)<<RF_BZ_DG1_QDB_3_POS)
#define RF_BZ_DG1_QDB_3_UMSK                                    (~(((1U<<RF_BZ_DG1_QDB_3_LEN)-1)<<RF_BZ_DG1_QDB_3_POS))
#define RF_BZ_GC_TMX_3                                          RF_BZ_GC_TMX_3
#define RF_BZ_GC_TMX_3_POS                                      (18U)
#define RF_BZ_GC_TMX_3_LEN                                      (3U)
#define RF_BZ_GC_TMX_3_MSK                                      (((1U<<RF_BZ_GC_TMX_3_LEN)-1)<<RF_BZ_GC_TMX_3_POS)
#define RF_BZ_GC_TMX_3_UMSK                                     (~(((1U<<RF_BZ_GC_TMX_3_LEN)-1)<<RF_BZ_GC_TMX_3_POS))
#define RF_BZ_GC_TBB_3                                          RF_BZ_GC_TBB_3
#define RF_BZ_GC_TBB_3_POS                                      (21U)
#define RF_BZ_GC_TBB_3_LEN                                      (5U)
#define RF_BZ_GC_TBB_3_MSK                                      (((1U<<RF_BZ_GC_TBB_3_LEN)-1)<<RF_BZ_GC_TBB_3_POS)
#define RF_BZ_GC_TBB_3_UMSK                                     (~(((1U<<RF_BZ_GC_TBB_3_LEN)-1)<<RF_BZ_GC_TBB_3_POS))
#define RF_BZ_GC_TBB_BOOST_3                                    RF_BZ_GC_TBB_BOOST_3
#define RF_BZ_GC_TBB_BOOST_3_POS                                (26U)
#define RF_BZ_GC_TBB_BOOST_3_LEN                                (2U)
#define RF_BZ_GC_TBB_BOOST_3_MSK                                (((1U<<RF_BZ_GC_TBB_BOOST_3_LEN)-1)<<RF_BZ_GC_TBB_BOOST_3_POS)
#define RF_BZ_GC_TBB_BOOST_3_UMSK                               (~(((1U<<RF_BZ_GC_TBB_BOOST_3_LEN)-1)<<RF_BZ_GC_TBB_BOOST_3_POS))
#define RF_BZ_DAC_BIAS_3                                        RF_BZ_DAC_BIAS_3
#define RF_BZ_DAC_BIAS_3_POS                                    (28U)
#define RF_BZ_DAC_BIAS_3_LEN                                    (2U)
#define RF_BZ_DAC_BIAS_3_MSK                                    (((1U<<RF_BZ_DAC_BIAS_3_LEN)-1)<<RF_BZ_DAC_BIAS_3_POS)
#define RF_BZ_DAC_BIAS_3_UMSK                                   (~(((1U<<RF_BZ_DAC_BIAS_3_LEN)-1)<<RF_BZ_DAC_BIAS_3_POS))

/* 0x7A8 : pwr_bz_tbl_8 */
#define RF_PWR_BZ_TBL_8_OFFSET                                  (0x7A8)
#define RF_BZ_DPD_SEL_3                                         RF_BZ_DPD_SEL_3
#define RF_BZ_DPD_SEL_3_POS                                     (0U)
#define RF_BZ_DPD_SEL_3_LEN                                     (2U)
#define RF_BZ_DPD_SEL_3_MSK                                     (((1U<<RF_BZ_DPD_SEL_3_LEN)-1)<<RF_BZ_DPD_SEL_3_POS)
#define RF_BZ_DPD_SEL_3_UMSK                                    (~(((1U<<RF_BZ_DPD_SEL_3_LEN)-1)<<RF_BZ_DPD_SEL_3_POS))
#define RF_BZ_TOSDAC_Q_3                                        RF_BZ_TOSDAC_Q_3
#define RF_BZ_TOSDAC_Q_3_POS                                    (2U)
#define RF_BZ_TOSDAC_Q_3_LEN                                    (6U)
#define RF_BZ_TOSDAC_Q_3_MSK                                    (((1U<<RF_BZ_TOSDAC_Q_3_LEN)-1)<<RF_BZ_TOSDAC_Q_3_POS)
#define RF_BZ_TOSDAC_Q_3_UMSK                                   (~(((1U<<RF_BZ_TOSDAC_Q_3_LEN)-1)<<RF_BZ_TOSDAC_Q_3_POS))
#define RF_BZ_TOSDAC_I_3                                        RF_BZ_TOSDAC_I_3
#define RF_BZ_TOSDAC_I_3_POS                                    (8U)
#define RF_BZ_TOSDAC_I_3_LEN                                    (6U)
#define RF_BZ_TOSDAC_I_3_MSK                                    (((1U<<RF_BZ_TOSDAC_I_3_LEN)-1)<<RF_BZ_TOSDAC_I_3_POS)
#define RF_BZ_TOSDAC_I_3_UMSK                                   (~(((1U<<RF_BZ_TOSDAC_I_3_LEN)-1)<<RF_BZ_TOSDAC_I_3_POS))
#define RF_BZ_IQC_PHASE_3                                       RF_BZ_IQC_PHASE_3
#define RF_BZ_IQC_PHASE_3_POS                                   (14U)
#define RF_BZ_IQC_PHASE_3_LEN                                   (10U)
#define RF_BZ_IQC_PHASE_3_MSK                                   (((1U<<RF_BZ_IQC_PHASE_3_LEN)-1)<<RF_BZ_IQC_PHASE_3_POS)
#define RF_BZ_IQC_PHASE_3_UMSK                                  (~(((1U<<RF_BZ_IQC_PHASE_3_LEN)-1)<<RF_BZ_IQC_PHASE_3_POS))
#define RF_BZ_GC_PA_3                                           RF_BZ_GC_PA_3
#define RF_BZ_GC_PA_3_POS                                       (24U)
#define RF_BZ_GC_PA_3_LEN                                       (3U)
#define RF_BZ_GC_PA_3_MSK                                       (((1U<<RF_BZ_GC_PA_3_LEN)-1)<<RF_BZ_GC_PA_3_POS)
#define RF_BZ_GC_PA_3_UMSK                                      (~(((1U<<RF_BZ_GC_PA_3_LEN)-1)<<RF_BZ_GC_PA_3_POS))
#define RF_BZ_PA_LP_EN_3                                        RF_BZ_PA_LP_EN_3
#define RF_BZ_PA_LP_EN_3_POS                                    (27U)
#define RF_BZ_PA_LP_EN_3_LEN                                    (1U)
#define RF_BZ_PA_LP_EN_3_MSK                                    (((1U<<RF_BZ_PA_LP_EN_3_LEN)-1)<<RF_BZ_PA_LP_EN_3_POS)
#define RF_BZ_PA_LP_EN_3_UMSK                                   (~(((1U<<RF_BZ_PA_LP_EN_3_LEN)-1)<<RF_BZ_PA_LP_EN_3_POS))
#define RF_BZ_DPD_EN_3                                          RF_BZ_DPD_EN_3
#define RF_BZ_DPD_EN_3_POS                                      (28U)
#define RF_BZ_DPD_EN_3_LEN                                      (1U)
#define RF_BZ_DPD_EN_3_MSK                                      (((1U<<RF_BZ_DPD_EN_3_LEN)-1)<<RF_BZ_DPD_EN_3_POS)
#define RF_BZ_DPD_EN_3_UMSK                                     (~(((1U<<RF_BZ_DPD_EN_3_LEN)-1)<<RF_BZ_DPD_EN_3_POS))

/* 0x7AC : pwr_ctrl_0 */
#define RF_PWR_CTRL_0_OFFSET                                    (0x7AC)
#define RF_WIFI_TX_CORR_DSSS                                    RF_WIFI_TX_CORR_DSSS
#define RF_WIFI_TX_CORR_DSSS_POS                                (0U)
#define RF_WIFI_TX_CORR_DSSS_LEN                                (8U)
#define RF_WIFI_TX_CORR_DSSS_MSK                                (((1U<<RF_WIFI_TX_CORR_DSSS_LEN)-1)<<RF_WIFI_TX_CORR_DSSS_POS)
#define RF_WIFI_TX_CORR_DSSS_UMSK                               (~(((1U<<RF_WIFI_TX_CORR_DSSS_LEN)-1)<<RF_WIFI_TX_CORR_DSSS_POS))
#define RF_TX_PWR_STS                                           RF_TX_PWR_STS
#define RF_TX_PWR_STS_POS                                       (8U)
#define RF_TX_PWR_STS_LEN                                       (20U)
#define RF_TX_PWR_STS_MSK                                       (((1U<<RF_TX_PWR_STS_LEN)-1)<<RF_TX_PWR_STS_POS)
#define RF_TX_PWR_STS_UMSK                                      (~(((1U<<RF_TX_PWR_STS_LEN)-1)<<RF_TX_PWR_STS_POS))
#define RF_TX_REDUCE_PWR_ENA                                    RF_TX_REDUCE_PWR_ENA
#define RF_TX_REDUCE_PWR_ENA_POS                                (31U)
#define RF_TX_REDUCE_PWR_ENA_LEN                                (1U)
#define RF_TX_REDUCE_PWR_ENA_MSK                                (((1U<<RF_TX_REDUCE_PWR_ENA_LEN)-1)<<RF_TX_REDUCE_PWR_ENA_POS)
#define RF_TX_REDUCE_PWR_ENA_UMSK                               (~(((1U<<RF_TX_REDUCE_PWR_ENA_LEN)-1)<<RF_TX_REDUCE_PWR_ENA_POS))

/* 0x7B0 : pwr_ctrl_1 */
#define RF_PWR_CTRL_1_OFFSET                                    (0x7B0)
#define RF_TX_REDUCE_PWR_WIFI                                   RF_TX_REDUCE_PWR_WIFI
#define RF_TX_REDUCE_PWR_WIFI_POS                               (0U)
#define RF_TX_REDUCE_PWR_WIFI_LEN                               (8U)
#define RF_TX_REDUCE_PWR_WIFI_MSK                               (((1U<<RF_TX_REDUCE_PWR_WIFI_LEN)-1)<<RF_TX_REDUCE_PWR_WIFI_POS)
#define RF_TX_REDUCE_PWR_WIFI_UMSK                              (~(((1U<<RF_TX_REDUCE_PWR_WIFI_LEN)-1)<<RF_TX_REDUCE_PWR_WIFI_POS))
#define RF_TX_REDUCE_PWR_BT                                     RF_TX_REDUCE_PWR_BT
#define RF_TX_REDUCE_PWR_BT_POS                                 (8U)
#define RF_TX_REDUCE_PWR_BT_LEN                                 (8U)
#define RF_TX_REDUCE_PWR_BT_MSK                                 (((1U<<RF_TX_REDUCE_PWR_BT_LEN)-1)<<RF_TX_REDUCE_PWR_BT_POS)
#define RF_TX_REDUCE_PWR_BT_UMSK                                (~(((1U<<RF_TX_REDUCE_PWR_BT_LEN)-1)<<RF_TX_REDUCE_PWR_BT_POS))
#define RF_TX_REDUCE_PWR_ZB                                     RF_TX_REDUCE_PWR_ZB
#define RF_TX_REDUCE_PWR_ZB_POS                                 (16U)
#define RF_TX_REDUCE_PWR_ZB_LEN                                 (8U)
#define RF_TX_REDUCE_PWR_ZB_MSK                                 (((1U<<RF_TX_REDUCE_PWR_ZB_LEN)-1)<<RF_TX_REDUCE_PWR_ZB_POS)
#define RF_TX_REDUCE_PWR_ZB_UMSK                                (~(((1U<<RF_TX_REDUCE_PWR_ZB_LEN)-1)<<RF_TX_REDUCE_PWR_ZB_POS))
#define RF_TX_REDUCE_PWR_BLE                                    RF_TX_REDUCE_PWR_BLE
#define RF_TX_REDUCE_PWR_BLE_POS                                (24U)
#define RF_TX_REDUCE_PWR_BLE_LEN                                (8U)
#define RF_TX_REDUCE_PWR_BLE_MSK                                (((1U<<RF_TX_REDUCE_PWR_BLE_LEN)-1)<<RF_TX_REDUCE_PWR_BLE_POS)
#define RF_TX_REDUCE_PWR_BLE_UMSK                               (~(((1U<<RF_TX_REDUCE_PWR_BLE_LEN)-1)<<RF_TX_REDUCE_PWR_BLE_POS))

/* 0x7B4 : pwr_ctrl_3 */
#define RF_PWR_CTRL_3_OFFSET                                    (0x7B4)
#define RF_BT_TX_CORR_0                                         RF_BT_TX_CORR_0
#define RF_BT_TX_CORR_0_POS                                     (0U)
#define RF_BT_TX_CORR_0_LEN                                     (6U)
#define RF_BT_TX_CORR_0_MSK                                     (((1U<<RF_BT_TX_CORR_0_LEN)-1)<<RF_BT_TX_CORR_0_POS)
#define RF_BT_TX_CORR_0_UMSK                                    (~(((1U<<RF_BT_TX_CORR_0_LEN)-1)<<RF_BT_TX_CORR_0_POS))
#define RF_BT_TX_CORR_1                                         RF_BT_TX_CORR_1
#define RF_BT_TX_CORR_1_POS                                     (6U)
#define RF_BT_TX_CORR_1_LEN                                     (6U)
#define RF_BT_TX_CORR_1_MSK                                     (((1U<<RF_BT_TX_CORR_1_LEN)-1)<<RF_BT_TX_CORR_1_POS)
#define RF_BT_TX_CORR_1_UMSK                                    (~(((1U<<RF_BT_TX_CORR_1_LEN)-1)<<RF_BT_TX_CORR_1_POS))
#define RF_BT_TX_CORR_2                                         RF_BT_TX_CORR_2
#define RF_BT_TX_CORR_2_POS                                     (12U)
#define RF_BT_TX_CORR_2_LEN                                     (6U)
#define RF_BT_TX_CORR_2_MSK                                     (((1U<<RF_BT_TX_CORR_2_LEN)-1)<<RF_BT_TX_CORR_2_POS)
#define RF_BT_TX_CORR_2_UMSK                                    (~(((1U<<RF_BT_TX_CORR_2_LEN)-1)<<RF_BT_TX_CORR_2_POS))
#define RF_BT_TX_CORR_3                                         RF_BT_TX_CORR_3
#define RF_BT_TX_CORR_3_POS                                     (18U)
#define RF_BT_TX_CORR_3_LEN                                     (6U)
#define RF_BT_TX_CORR_3_MSK                                     (((1U<<RF_BT_TX_CORR_3_LEN)-1)<<RF_BT_TX_CORR_3_POS)
#define RF_BT_TX_CORR_3_UMSK                                    (~(((1U<<RF_BT_TX_CORR_3_LEN)-1)<<RF_BT_TX_CORR_3_POS))
#define RF_BT_TX_CORR_4                                         RF_BT_TX_CORR_4
#define RF_BT_TX_CORR_4_POS                                     (24U)
#define RF_BT_TX_CORR_4_LEN                                     (6U)
#define RF_BT_TX_CORR_4_MSK                                     (((1U<<RF_BT_TX_CORR_4_LEN)-1)<<RF_BT_TX_CORR_4_POS)
#define RF_BT_TX_CORR_4_UMSK                                    (~(((1U<<RF_BT_TX_CORR_4_LEN)-1)<<RF_BT_TX_CORR_4_POS))

/* 0x7B8 : pwr_ctrl_4 */
#define RF_PWR_CTRL_4_OFFSET                                    (0x7B8)
#define RF_BT_TX_TEMP_0                                         RF_BT_TX_TEMP_0
#define RF_BT_TX_TEMP_0_POS                                     (0U)
#define RF_BT_TX_TEMP_0_LEN                                     (6U)
#define RF_BT_TX_TEMP_0_MSK                                     (((1U<<RF_BT_TX_TEMP_0_LEN)-1)<<RF_BT_TX_TEMP_0_POS)
#define RF_BT_TX_TEMP_0_UMSK                                    (~(((1U<<RF_BT_TX_TEMP_0_LEN)-1)<<RF_BT_TX_TEMP_0_POS))
#define RF_BT_TX_TEMP_1                                         RF_BT_TX_TEMP_1
#define RF_BT_TX_TEMP_1_POS                                     (6U)
#define RF_BT_TX_TEMP_1_LEN                                     (6U)
#define RF_BT_TX_TEMP_1_MSK                                     (((1U<<RF_BT_TX_TEMP_1_LEN)-1)<<RF_BT_TX_TEMP_1_POS)
#define RF_BT_TX_TEMP_1_UMSK                                    (~(((1U<<RF_BT_TX_TEMP_1_LEN)-1)<<RF_BT_TX_TEMP_1_POS))
#define RF_BT_TX_TEMP_2                                         RF_BT_TX_TEMP_2
#define RF_BT_TX_TEMP_2_POS                                     (12U)
#define RF_BT_TX_TEMP_2_LEN                                     (6U)
#define RF_BT_TX_TEMP_2_MSK                                     (((1U<<RF_BT_TX_TEMP_2_LEN)-1)<<RF_BT_TX_TEMP_2_POS)
#define RF_BT_TX_TEMP_2_UMSK                                    (~(((1U<<RF_BT_TX_TEMP_2_LEN)-1)<<RF_BT_TX_TEMP_2_POS))
#define RF_BT_TX_TEMP_3                                         RF_BT_TX_TEMP_3
#define RF_BT_TX_TEMP_3_POS                                     (18U)
#define RF_BT_TX_TEMP_3_LEN                                     (6U)
#define RF_BT_TX_TEMP_3_MSK                                     (((1U<<RF_BT_TX_TEMP_3_LEN)-1)<<RF_BT_TX_TEMP_3_POS)
#define RF_BT_TX_TEMP_3_UMSK                                    (~(((1U<<RF_BT_TX_TEMP_3_LEN)-1)<<RF_BT_TX_TEMP_3_POS))
#define RF_BT_TX_TEMP_4                                         RF_BT_TX_TEMP_4
#define RF_BT_TX_TEMP_4_POS                                     (24U)
#define RF_BT_TX_TEMP_4_LEN                                     (6U)
#define RF_BT_TX_TEMP_4_MSK                                     (((1U<<RF_BT_TX_TEMP_4_LEN)-1)<<RF_BT_TX_TEMP_4_POS)
#define RF_BT_TX_TEMP_4_UMSK                                    (~(((1U<<RF_BT_TX_TEMP_4_LEN)-1)<<RF_BT_TX_TEMP_4_POS))

/* 0x7BC : pa_config_b */
#define RF_PA_CONFIG_B_OFFSET                                   (0x7BC)
#define RF_WIFI_PA_VBCAS_B                                      RF_WIFI_PA_VBCAS_B
#define RF_WIFI_PA_VBCAS_B_POS                                  (0U)
#define RF_WIFI_PA_VBCAS_B_LEN                                  (3U)
#define RF_WIFI_PA_VBCAS_B_MSK                                  (((1U<<RF_WIFI_PA_VBCAS_B_LEN)-1)<<RF_WIFI_PA_VBCAS_B_POS)
#define RF_WIFI_PA_VBCAS_B_UMSK                                 (~(((1U<<RF_WIFI_PA_VBCAS_B_LEN)-1)<<RF_WIFI_PA_VBCAS_B_POS))
#define RF_WIFI_PA_IAQ_B                                        RF_WIFI_PA_IAQ_B
#define RF_WIFI_PA_IAQ_B_POS                                    (3U)
#define RF_WIFI_PA_IAQ_B_LEN                                    (3U)
#define RF_WIFI_PA_IAQ_B_MSK                                    (((1U<<RF_WIFI_PA_IAQ_B_LEN)-1)<<RF_WIFI_PA_IAQ_B_POS)
#define RF_WIFI_PA_IAQ_B_UMSK                                   (~(((1U<<RF_WIFI_PA_IAQ_B_LEN)-1)<<RF_WIFI_PA_IAQ_B_POS))
#define RF_WIFI_PA_IET_B                                        RF_WIFI_PA_IET_B
#define RF_WIFI_PA_IET_B_POS                                    (6U)
#define RF_WIFI_PA_IET_B_LEN                                    (4U)
#define RF_WIFI_PA_IET_B_MSK                                    (((1U<<RF_WIFI_PA_IET_B_LEN)-1)<<RF_WIFI_PA_IET_B_POS)
#define RF_WIFI_PA_IET_B_UMSK                                   (~(((1U<<RF_WIFI_PA_IET_B_LEN)-1)<<RF_WIFI_PA_IET_B_POS))
#define RF_WIFI_PA_VBPMOS_IET_B                                 RF_WIFI_PA_VBPMOS_IET_B
#define RF_WIFI_PA_VBPMOS_IET_B_POS                             (10U)
#define RF_WIFI_PA_VBPMOS_IET_B_LEN                             (2U)
#define RF_WIFI_PA_VBPMOS_IET_B_MSK                             (((1U<<RF_WIFI_PA_VBPMOS_IET_B_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_B_POS)
#define RF_WIFI_PA_VBPMOS_IET_B_UMSK                            (~(((1U<<RF_WIFI_PA_VBPMOS_IET_B_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_B_POS))
#define RF_WIFI_PA_VBPMOS_B                                     RF_WIFI_PA_VBPMOS_B
#define RF_WIFI_PA_VBPMOS_B_POS                                 (12U)
#define RF_WIFI_PA_VBPMOS_B_LEN                                 (4U)
#define RF_WIFI_PA_VBPMOS_B_MSK                                 (((1U<<RF_WIFI_PA_VBPMOS_B_LEN)-1)<<RF_WIFI_PA_VBPMOS_B_POS)
#define RF_WIFI_PA_VBPMOS_B_UMSK                                (~(((1U<<RF_WIFI_PA_VBPMOS_B_LEN)-1)<<RF_WIFI_PA_VBPMOS_B_POS))
#define RF_WIFI_PA_VBCORE_B                                     RF_WIFI_PA_VBCORE_B
#define RF_WIFI_PA_VBCORE_B_POS                                 (16U)
#define RF_WIFI_PA_VBCORE_B_LEN                                 (4U)
#define RF_WIFI_PA_VBCORE_B_MSK                                 (((1U<<RF_WIFI_PA_VBCORE_B_LEN)-1)<<RF_WIFI_PA_VBCORE_B_POS)
#define RF_WIFI_PA_VBCORE_B_UMSK                                (~(((1U<<RF_WIFI_PA_VBCORE_B_LEN)-1)<<RF_WIFI_PA_VBCORE_B_POS))
#define RF_WIFI_PA_ETB_EN_B                                     RF_WIFI_PA_ETB_EN_B
#define RF_WIFI_PA_ETB_EN_B_POS                                 (20U)
#define RF_WIFI_PA_ETB_EN_B_LEN                                 (1U)
#define RF_WIFI_PA_ETB_EN_B_MSK                                 (((1U<<RF_WIFI_PA_ETB_EN_B_LEN)-1)<<RF_WIFI_PA_ETB_EN_B_POS)
#define RF_WIFI_PA_ETB_EN_B_UMSK                                (~(((1U<<RF_WIFI_PA_ETB_EN_B_LEN)-1)<<RF_WIFI_PA_ETB_EN_B_POS))
#define RF_WIFI_PA_IB_FIX_B                                     RF_WIFI_PA_IB_FIX_B
#define RF_WIFI_PA_IB_FIX_B_POS                                 (21U)
#define RF_WIFI_PA_IB_FIX_B_LEN                                 (1U)
#define RF_WIFI_PA_IB_FIX_B_MSK                                 (((1U<<RF_WIFI_PA_IB_FIX_B_LEN)-1)<<RF_WIFI_PA_IB_FIX_B_POS)
#define RF_WIFI_PA_IB_FIX_B_UMSK                                (~(((1U<<RF_WIFI_PA_IB_FIX_B_LEN)-1)<<RF_WIFI_PA_IB_FIX_B_POS))

/* 0x7C0 : pa_config_g */
#define RF_PA_CONFIG_G_OFFSET                                   (0x7C0)
#define RF_WIFI_PA_VBCAS_G                                      RF_WIFI_PA_VBCAS_G
#define RF_WIFI_PA_VBCAS_G_POS                                  (0U)
#define RF_WIFI_PA_VBCAS_G_LEN                                  (3U)
#define RF_WIFI_PA_VBCAS_G_MSK                                  (((1U<<RF_WIFI_PA_VBCAS_G_LEN)-1)<<RF_WIFI_PA_VBCAS_G_POS)
#define RF_WIFI_PA_VBCAS_G_UMSK                                 (~(((1U<<RF_WIFI_PA_VBCAS_G_LEN)-1)<<RF_WIFI_PA_VBCAS_G_POS))
#define RF_WIFI_PA_IAQ_G                                        RF_WIFI_PA_IAQ_G
#define RF_WIFI_PA_IAQ_G_POS                                    (3U)
#define RF_WIFI_PA_IAQ_G_LEN                                    (3U)
#define RF_WIFI_PA_IAQ_G_MSK                                    (((1U<<RF_WIFI_PA_IAQ_G_LEN)-1)<<RF_WIFI_PA_IAQ_G_POS)
#define RF_WIFI_PA_IAQ_G_UMSK                                   (~(((1U<<RF_WIFI_PA_IAQ_G_LEN)-1)<<RF_WIFI_PA_IAQ_G_POS))
#define RF_WIFI_PA_IET_G                                        RF_WIFI_PA_IET_G
#define RF_WIFI_PA_IET_G_POS                                    (6U)
#define RF_WIFI_PA_IET_G_LEN                                    (4U)
#define RF_WIFI_PA_IET_G_MSK                                    (((1U<<RF_WIFI_PA_IET_G_LEN)-1)<<RF_WIFI_PA_IET_G_POS)
#define RF_WIFI_PA_IET_G_UMSK                                   (~(((1U<<RF_WIFI_PA_IET_G_LEN)-1)<<RF_WIFI_PA_IET_G_POS))
#define RF_WIFI_PA_VBPMOS_IET_G                                 RF_WIFI_PA_VBPMOS_IET_G
#define RF_WIFI_PA_VBPMOS_IET_G_POS                             (10U)
#define RF_WIFI_PA_VBPMOS_IET_G_LEN                             (2U)
#define RF_WIFI_PA_VBPMOS_IET_G_MSK                             (((1U<<RF_WIFI_PA_VBPMOS_IET_G_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_G_POS)
#define RF_WIFI_PA_VBPMOS_IET_G_UMSK                            (~(((1U<<RF_WIFI_PA_VBPMOS_IET_G_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_G_POS))
#define RF_WIFI_PA_VBPMOS_G                                     RF_WIFI_PA_VBPMOS_G
#define RF_WIFI_PA_VBPMOS_G_POS                                 (12U)
#define RF_WIFI_PA_VBPMOS_G_LEN                                 (4U)
#define RF_WIFI_PA_VBPMOS_G_MSK                                 (((1U<<RF_WIFI_PA_VBPMOS_G_LEN)-1)<<RF_WIFI_PA_VBPMOS_G_POS)
#define RF_WIFI_PA_VBPMOS_G_UMSK                                (~(((1U<<RF_WIFI_PA_VBPMOS_G_LEN)-1)<<RF_WIFI_PA_VBPMOS_G_POS))
#define RF_WIFI_PA_VBCORE_G                                     RF_WIFI_PA_VBCORE_G
#define RF_WIFI_PA_VBCORE_G_POS                                 (16U)
#define RF_WIFI_PA_VBCORE_G_LEN                                 (4U)
#define RF_WIFI_PA_VBCORE_G_MSK                                 (((1U<<RF_WIFI_PA_VBCORE_G_LEN)-1)<<RF_WIFI_PA_VBCORE_G_POS)
#define RF_WIFI_PA_VBCORE_G_UMSK                                (~(((1U<<RF_WIFI_PA_VBCORE_G_LEN)-1)<<RF_WIFI_PA_VBCORE_G_POS))
#define RF_WIFI_PA_ETB_EN_G                                     RF_WIFI_PA_ETB_EN_G
#define RF_WIFI_PA_ETB_EN_G_POS                                 (20U)
#define RF_WIFI_PA_ETB_EN_G_LEN                                 (1U)
#define RF_WIFI_PA_ETB_EN_G_MSK                                 (((1U<<RF_WIFI_PA_ETB_EN_G_LEN)-1)<<RF_WIFI_PA_ETB_EN_G_POS)
#define RF_WIFI_PA_ETB_EN_G_UMSK                                (~(((1U<<RF_WIFI_PA_ETB_EN_G_LEN)-1)<<RF_WIFI_PA_ETB_EN_G_POS))
#define RF_WIFI_PA_IB_FIX_G                                     RF_WIFI_PA_IB_FIX_G
#define RF_WIFI_PA_IB_FIX_G_POS                                 (21U)
#define RF_WIFI_PA_IB_FIX_G_LEN                                 (1U)
#define RF_WIFI_PA_IB_FIX_G_MSK                                 (((1U<<RF_WIFI_PA_IB_FIX_G_LEN)-1)<<RF_WIFI_PA_IB_FIX_G_POS)
#define RF_WIFI_PA_IB_FIX_G_UMSK                                (~(((1U<<RF_WIFI_PA_IB_FIX_G_LEN)-1)<<RF_WIFI_PA_IB_FIX_G_POS))

/* 0x7C4 : pa_config_n */
#define RF_PA_CONFIG_N_OFFSET                                   (0x7C4)
#define RF_WIFI_PA_VBCAS_N                                      RF_WIFI_PA_VBCAS_N
#define RF_WIFI_PA_VBCAS_N_POS                                  (0U)
#define RF_WIFI_PA_VBCAS_N_LEN                                  (3U)
#define RF_WIFI_PA_VBCAS_N_MSK                                  (((1U<<RF_WIFI_PA_VBCAS_N_LEN)-1)<<RF_WIFI_PA_VBCAS_N_POS)
#define RF_WIFI_PA_VBCAS_N_UMSK                                 (~(((1U<<RF_WIFI_PA_VBCAS_N_LEN)-1)<<RF_WIFI_PA_VBCAS_N_POS))
#define RF_WIFI_PA_IAQ_N                                        RF_WIFI_PA_IAQ_N
#define RF_WIFI_PA_IAQ_N_POS                                    (3U)
#define RF_WIFI_PA_IAQ_N_LEN                                    (3U)
#define RF_WIFI_PA_IAQ_N_MSK                                    (((1U<<RF_WIFI_PA_IAQ_N_LEN)-1)<<RF_WIFI_PA_IAQ_N_POS)
#define RF_WIFI_PA_IAQ_N_UMSK                                   (~(((1U<<RF_WIFI_PA_IAQ_N_LEN)-1)<<RF_WIFI_PA_IAQ_N_POS))
#define RF_WIFI_PA_IET_N                                        RF_WIFI_PA_IET_N
#define RF_WIFI_PA_IET_N_POS                                    (6U)
#define RF_WIFI_PA_IET_N_LEN                                    (4U)
#define RF_WIFI_PA_IET_N_MSK                                    (((1U<<RF_WIFI_PA_IET_N_LEN)-1)<<RF_WIFI_PA_IET_N_POS)
#define RF_WIFI_PA_IET_N_UMSK                                   (~(((1U<<RF_WIFI_PA_IET_N_LEN)-1)<<RF_WIFI_PA_IET_N_POS))
#define RF_WIFI_PA_VBPMOS_IET_N                                 RF_WIFI_PA_VBPMOS_IET_N
#define RF_WIFI_PA_VBPMOS_IET_N_POS                             (10U)
#define RF_WIFI_PA_VBPMOS_IET_N_LEN                             (2U)
#define RF_WIFI_PA_VBPMOS_IET_N_MSK                             (((1U<<RF_WIFI_PA_VBPMOS_IET_N_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_N_POS)
#define RF_WIFI_PA_VBPMOS_IET_N_UMSK                            (~(((1U<<RF_WIFI_PA_VBPMOS_IET_N_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_N_POS))
#define RF_WIFI_PA_VBPMOS_N                                     RF_WIFI_PA_VBPMOS_N
#define RF_WIFI_PA_VBPMOS_N_POS                                 (12U)
#define RF_WIFI_PA_VBPMOS_N_LEN                                 (4U)
#define RF_WIFI_PA_VBPMOS_N_MSK                                 (((1U<<RF_WIFI_PA_VBPMOS_N_LEN)-1)<<RF_WIFI_PA_VBPMOS_N_POS)
#define RF_WIFI_PA_VBPMOS_N_UMSK                                (~(((1U<<RF_WIFI_PA_VBPMOS_N_LEN)-1)<<RF_WIFI_PA_VBPMOS_N_POS))
#define RF_WIFI_PA_VBCORE_N                                     RF_WIFI_PA_VBCORE_N
#define RF_WIFI_PA_VBCORE_N_POS                                 (16U)
#define RF_WIFI_PA_VBCORE_N_LEN                                 (4U)
#define RF_WIFI_PA_VBCORE_N_MSK                                 (((1U<<RF_WIFI_PA_VBCORE_N_LEN)-1)<<RF_WIFI_PA_VBCORE_N_POS)
#define RF_WIFI_PA_VBCORE_N_UMSK                                (~(((1U<<RF_WIFI_PA_VBCORE_N_LEN)-1)<<RF_WIFI_PA_VBCORE_N_POS))
#define RF_WIFI_PA_ETB_EN_N                                     RF_WIFI_PA_ETB_EN_N
#define RF_WIFI_PA_ETB_EN_N_POS                                 (20U)
#define RF_WIFI_PA_ETB_EN_N_LEN                                 (1U)
#define RF_WIFI_PA_ETB_EN_N_MSK                                 (((1U<<RF_WIFI_PA_ETB_EN_N_LEN)-1)<<RF_WIFI_PA_ETB_EN_N_POS)
#define RF_WIFI_PA_ETB_EN_N_UMSK                                (~(((1U<<RF_WIFI_PA_ETB_EN_N_LEN)-1)<<RF_WIFI_PA_ETB_EN_N_POS))
#define RF_WIFI_PA_IB_FIX_N                                     RF_WIFI_PA_IB_FIX_N
#define RF_WIFI_PA_IB_FIX_N_POS                                 (21U)
#define RF_WIFI_PA_IB_FIX_N_LEN                                 (1U)
#define RF_WIFI_PA_IB_FIX_N_MSK                                 (((1U<<RF_WIFI_PA_IB_FIX_N_LEN)-1)<<RF_WIFI_PA_IB_FIX_N_POS)
#define RF_WIFI_PA_IB_FIX_N_UMSK                                (~(((1U<<RF_WIFI_PA_IB_FIX_N_LEN)-1)<<RF_WIFI_PA_IB_FIX_N_POS))

/* 0x7C8 : pa_config_ax_sub_mode0 */
#define RF_PA_CONFIG_AX_SUB_MODE0_OFFSET                        (0x7C8)
#define RF_WIFI_PA_VBCAS_AX0                                    RF_WIFI_PA_VBCAS_AX0
#define RF_WIFI_PA_VBCAS_AX0_POS                                (0U)
#define RF_WIFI_PA_VBCAS_AX0_LEN                                (3U)
#define RF_WIFI_PA_VBCAS_AX0_MSK                                (((1U<<RF_WIFI_PA_VBCAS_AX0_LEN)-1)<<RF_WIFI_PA_VBCAS_AX0_POS)
#define RF_WIFI_PA_VBCAS_AX0_UMSK                               (~(((1U<<RF_WIFI_PA_VBCAS_AX0_LEN)-1)<<RF_WIFI_PA_VBCAS_AX0_POS))
#define RF_WIFI_PA_IAQ_AX0                                      RF_WIFI_PA_IAQ_AX0
#define RF_WIFI_PA_IAQ_AX0_POS                                  (3U)
#define RF_WIFI_PA_IAQ_AX0_LEN                                  (3U)
#define RF_WIFI_PA_IAQ_AX0_MSK                                  (((1U<<RF_WIFI_PA_IAQ_AX0_LEN)-1)<<RF_WIFI_PA_IAQ_AX0_POS)
#define RF_WIFI_PA_IAQ_AX0_UMSK                                 (~(((1U<<RF_WIFI_PA_IAQ_AX0_LEN)-1)<<RF_WIFI_PA_IAQ_AX0_POS))
#define RF_WIFI_PA_IET_AX0                                      RF_WIFI_PA_IET_AX0
#define RF_WIFI_PA_IET_AX0_POS                                  (6U)
#define RF_WIFI_PA_IET_AX0_LEN                                  (4U)
#define RF_WIFI_PA_IET_AX0_MSK                                  (((1U<<RF_WIFI_PA_IET_AX0_LEN)-1)<<RF_WIFI_PA_IET_AX0_POS)
#define RF_WIFI_PA_IET_AX0_UMSK                                 (~(((1U<<RF_WIFI_PA_IET_AX0_LEN)-1)<<RF_WIFI_PA_IET_AX0_POS))
#define RF_WIFI_PA_VBPMOS_IET_AX0                               RF_WIFI_PA_VBPMOS_IET_AX0
#define RF_WIFI_PA_VBPMOS_IET_AX0_POS                           (10U)
#define RF_WIFI_PA_VBPMOS_IET_AX0_LEN                           (2U)
#define RF_WIFI_PA_VBPMOS_IET_AX0_MSK                           (((1U<<RF_WIFI_PA_VBPMOS_IET_AX0_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_AX0_POS)
#define RF_WIFI_PA_VBPMOS_IET_AX0_UMSK                          (~(((1U<<RF_WIFI_PA_VBPMOS_IET_AX0_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_AX0_POS))
#define RF_WIFI_PA_VBPMOS_AX0                                   RF_WIFI_PA_VBPMOS_AX0
#define RF_WIFI_PA_VBPMOS_AX0_POS                               (12U)
#define RF_WIFI_PA_VBPMOS_AX0_LEN                               (4U)
#define RF_WIFI_PA_VBPMOS_AX0_MSK                               (((1U<<RF_WIFI_PA_VBPMOS_AX0_LEN)-1)<<RF_WIFI_PA_VBPMOS_AX0_POS)
#define RF_WIFI_PA_VBPMOS_AX0_UMSK                              (~(((1U<<RF_WIFI_PA_VBPMOS_AX0_LEN)-1)<<RF_WIFI_PA_VBPMOS_AX0_POS))
#define RF_WIFI_PA_VBCORE_AX0                                   RF_WIFI_PA_VBCORE_AX0
#define RF_WIFI_PA_VBCORE_AX0_POS                               (16U)
#define RF_WIFI_PA_VBCORE_AX0_LEN                               (4U)
#define RF_WIFI_PA_VBCORE_AX0_MSK                               (((1U<<RF_WIFI_PA_VBCORE_AX0_LEN)-1)<<RF_WIFI_PA_VBCORE_AX0_POS)
#define RF_WIFI_PA_VBCORE_AX0_UMSK                              (~(((1U<<RF_WIFI_PA_VBCORE_AX0_LEN)-1)<<RF_WIFI_PA_VBCORE_AX0_POS))
#define RF_WIFI_PA_ETB_EN_AX0                                   RF_WIFI_PA_ETB_EN_AX0
#define RF_WIFI_PA_ETB_EN_AX0_POS                               (20U)
#define RF_WIFI_PA_ETB_EN_AX0_LEN                               (1U)
#define RF_WIFI_PA_ETB_EN_AX0_MSK                               (((1U<<RF_WIFI_PA_ETB_EN_AX0_LEN)-1)<<RF_WIFI_PA_ETB_EN_AX0_POS)
#define RF_WIFI_PA_ETB_EN_AX0_UMSK                              (~(((1U<<RF_WIFI_PA_ETB_EN_AX0_LEN)-1)<<RF_WIFI_PA_ETB_EN_AX0_POS))
#define RF_WIFI_PA_IB_FIX_AX0                                   RF_WIFI_PA_IB_FIX_AX0
#define RF_WIFI_PA_IB_FIX_AX0_POS                               (21U)
#define RF_WIFI_PA_IB_FIX_AX0_LEN                               (1U)
#define RF_WIFI_PA_IB_FIX_AX0_MSK                               (((1U<<RF_WIFI_PA_IB_FIX_AX0_LEN)-1)<<RF_WIFI_PA_IB_FIX_AX0_POS)
#define RF_WIFI_PA_IB_FIX_AX0_UMSK                              (~(((1U<<RF_WIFI_PA_IB_FIX_AX0_LEN)-1)<<RF_WIFI_PA_IB_FIX_AX0_POS))

/* 0x7CC : pa_config_ax_sub_mode1 */
#define RF_PA_CONFIG_AX_SUB_MODE1_OFFSET                        (0x7CC)
#define RF_WIFI_PA_VBCAS_AX1                                    RF_WIFI_PA_VBCAS_AX1
#define RF_WIFI_PA_VBCAS_AX1_POS                                (0U)
#define RF_WIFI_PA_VBCAS_AX1_LEN                                (3U)
#define RF_WIFI_PA_VBCAS_AX1_MSK                                (((1U<<RF_WIFI_PA_VBCAS_AX1_LEN)-1)<<RF_WIFI_PA_VBCAS_AX1_POS)
#define RF_WIFI_PA_VBCAS_AX1_UMSK                               (~(((1U<<RF_WIFI_PA_VBCAS_AX1_LEN)-1)<<RF_WIFI_PA_VBCAS_AX1_POS))
#define RF_WIFI_PA_IAQ_AX1                                      RF_WIFI_PA_IAQ_AX1
#define RF_WIFI_PA_IAQ_AX1_POS                                  (3U)
#define RF_WIFI_PA_IAQ_AX1_LEN                                  (3U)
#define RF_WIFI_PA_IAQ_AX1_MSK                                  (((1U<<RF_WIFI_PA_IAQ_AX1_LEN)-1)<<RF_WIFI_PA_IAQ_AX1_POS)
#define RF_WIFI_PA_IAQ_AX1_UMSK                                 (~(((1U<<RF_WIFI_PA_IAQ_AX1_LEN)-1)<<RF_WIFI_PA_IAQ_AX1_POS))
#define RF_WIFI_PA_IET_AX1                                      RF_WIFI_PA_IET_AX1
#define RF_WIFI_PA_IET_AX1_POS                                  (6U)
#define RF_WIFI_PA_IET_AX1_LEN                                  (4U)
#define RF_WIFI_PA_IET_AX1_MSK                                  (((1U<<RF_WIFI_PA_IET_AX1_LEN)-1)<<RF_WIFI_PA_IET_AX1_POS)
#define RF_WIFI_PA_IET_AX1_UMSK                                 (~(((1U<<RF_WIFI_PA_IET_AX1_LEN)-1)<<RF_WIFI_PA_IET_AX1_POS))
#define RF_WIFI_PA_VBPMOS_IET_AX1                               RF_WIFI_PA_VBPMOS_IET_AX1
#define RF_WIFI_PA_VBPMOS_IET_AX1_POS                           (10U)
#define RF_WIFI_PA_VBPMOS_IET_AX1_LEN                           (2U)
#define RF_WIFI_PA_VBPMOS_IET_AX1_MSK                           (((1U<<RF_WIFI_PA_VBPMOS_IET_AX1_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_AX1_POS)
#define RF_WIFI_PA_VBPMOS_IET_AX1_UMSK                          (~(((1U<<RF_WIFI_PA_VBPMOS_IET_AX1_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_AX1_POS))
#define RF_WIFI_PA_VBPMOS_AX1                                   RF_WIFI_PA_VBPMOS_AX1
#define RF_WIFI_PA_VBPMOS_AX1_POS                               (12U)
#define RF_WIFI_PA_VBPMOS_AX1_LEN                               (4U)
#define RF_WIFI_PA_VBPMOS_AX1_MSK                               (((1U<<RF_WIFI_PA_VBPMOS_AX1_LEN)-1)<<RF_WIFI_PA_VBPMOS_AX1_POS)
#define RF_WIFI_PA_VBPMOS_AX1_UMSK                              (~(((1U<<RF_WIFI_PA_VBPMOS_AX1_LEN)-1)<<RF_WIFI_PA_VBPMOS_AX1_POS))
#define RF_WIFI_PA_VBCORE_AX1                                   RF_WIFI_PA_VBCORE_AX1
#define RF_WIFI_PA_VBCORE_AX1_POS                               (16U)
#define RF_WIFI_PA_VBCORE_AX1_LEN                               (4U)
#define RF_WIFI_PA_VBCORE_AX1_MSK                               (((1U<<RF_WIFI_PA_VBCORE_AX1_LEN)-1)<<RF_WIFI_PA_VBCORE_AX1_POS)
#define RF_WIFI_PA_VBCORE_AX1_UMSK                              (~(((1U<<RF_WIFI_PA_VBCORE_AX1_LEN)-1)<<RF_WIFI_PA_VBCORE_AX1_POS))
#define RF_WIFI_PA_ETB_EN_AX1                                   RF_WIFI_PA_ETB_EN_AX1
#define RF_WIFI_PA_ETB_EN_AX1_POS                               (20U)
#define RF_WIFI_PA_ETB_EN_AX1_LEN                               (1U)
#define RF_WIFI_PA_ETB_EN_AX1_MSK                               (((1U<<RF_WIFI_PA_ETB_EN_AX1_LEN)-1)<<RF_WIFI_PA_ETB_EN_AX1_POS)
#define RF_WIFI_PA_ETB_EN_AX1_UMSK                              (~(((1U<<RF_WIFI_PA_ETB_EN_AX1_LEN)-1)<<RF_WIFI_PA_ETB_EN_AX1_POS))
#define RF_WIFI_PA_IB_FIX_AX1                                   RF_WIFI_PA_IB_FIX_AX1
#define RF_WIFI_PA_IB_FIX_AX1_POS                               (21U)
#define RF_WIFI_PA_IB_FIX_AX1_LEN                               (1U)
#define RF_WIFI_PA_IB_FIX_AX1_MSK                               (((1U<<RF_WIFI_PA_IB_FIX_AX1_LEN)-1)<<RF_WIFI_PA_IB_FIX_AX1_POS)
#define RF_WIFI_PA_IB_FIX_AX1_UMSK                              (~(((1U<<RF_WIFI_PA_IB_FIX_AX1_LEN)-1)<<RF_WIFI_PA_IB_FIX_AX1_POS))

/* 0x7D0 : pa_config_ax_sub_mode2 */
#define RF_PA_CONFIG_AX_SUB_MODE2_OFFSET                        (0x7D0)
#define RF_WIFI_PA_VBCAS_AX2                                    RF_WIFI_PA_VBCAS_AX2
#define RF_WIFI_PA_VBCAS_AX2_POS                                (0U)
#define RF_WIFI_PA_VBCAS_AX2_LEN                                (3U)
#define RF_WIFI_PA_VBCAS_AX2_MSK                                (((1U<<RF_WIFI_PA_VBCAS_AX2_LEN)-1)<<RF_WIFI_PA_VBCAS_AX2_POS)
#define RF_WIFI_PA_VBCAS_AX2_UMSK                               (~(((1U<<RF_WIFI_PA_VBCAS_AX2_LEN)-1)<<RF_WIFI_PA_VBCAS_AX2_POS))
#define RF_WIFI_PA_IAQ_AX2                                      RF_WIFI_PA_IAQ_AX2
#define RF_WIFI_PA_IAQ_AX2_POS                                  (3U)
#define RF_WIFI_PA_IAQ_AX2_LEN                                  (3U)
#define RF_WIFI_PA_IAQ_AX2_MSK                                  (((1U<<RF_WIFI_PA_IAQ_AX2_LEN)-1)<<RF_WIFI_PA_IAQ_AX2_POS)
#define RF_WIFI_PA_IAQ_AX2_UMSK                                 (~(((1U<<RF_WIFI_PA_IAQ_AX2_LEN)-1)<<RF_WIFI_PA_IAQ_AX2_POS))
#define RF_WIFI_PA_IET_AX2                                      RF_WIFI_PA_IET_AX2
#define RF_WIFI_PA_IET_AX2_POS                                  (6U)
#define RF_WIFI_PA_IET_AX2_LEN                                  (4U)
#define RF_WIFI_PA_IET_AX2_MSK                                  (((1U<<RF_WIFI_PA_IET_AX2_LEN)-1)<<RF_WIFI_PA_IET_AX2_POS)
#define RF_WIFI_PA_IET_AX2_UMSK                                 (~(((1U<<RF_WIFI_PA_IET_AX2_LEN)-1)<<RF_WIFI_PA_IET_AX2_POS))
#define RF_WIFI_PA_VBPMOS_IET_AX2                               RF_WIFI_PA_VBPMOS_IET_AX2
#define RF_WIFI_PA_VBPMOS_IET_AX2_POS                           (10U)
#define RF_WIFI_PA_VBPMOS_IET_AX2_LEN                           (2U)
#define RF_WIFI_PA_VBPMOS_IET_AX2_MSK                           (((1U<<RF_WIFI_PA_VBPMOS_IET_AX2_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_AX2_POS)
#define RF_WIFI_PA_VBPMOS_IET_AX2_UMSK                          (~(((1U<<RF_WIFI_PA_VBPMOS_IET_AX2_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_AX2_POS))
#define RF_WIFI_PA_VBPMOS_AX2                                   RF_WIFI_PA_VBPMOS_AX2
#define RF_WIFI_PA_VBPMOS_AX2_POS                               (12U)
#define RF_WIFI_PA_VBPMOS_AX2_LEN                               (4U)
#define RF_WIFI_PA_VBPMOS_AX2_MSK                               (((1U<<RF_WIFI_PA_VBPMOS_AX2_LEN)-1)<<RF_WIFI_PA_VBPMOS_AX2_POS)
#define RF_WIFI_PA_VBPMOS_AX2_UMSK                              (~(((1U<<RF_WIFI_PA_VBPMOS_AX2_LEN)-1)<<RF_WIFI_PA_VBPMOS_AX2_POS))
#define RF_WIFI_PA_VBCORE_AX2                                   RF_WIFI_PA_VBCORE_AX2
#define RF_WIFI_PA_VBCORE_AX2_POS                               (16U)
#define RF_WIFI_PA_VBCORE_AX2_LEN                               (4U)
#define RF_WIFI_PA_VBCORE_AX2_MSK                               (((1U<<RF_WIFI_PA_VBCORE_AX2_LEN)-1)<<RF_WIFI_PA_VBCORE_AX2_POS)
#define RF_WIFI_PA_VBCORE_AX2_UMSK                              (~(((1U<<RF_WIFI_PA_VBCORE_AX2_LEN)-1)<<RF_WIFI_PA_VBCORE_AX2_POS))
#define RF_WIFI_PA_ETB_EN_AX2                                   RF_WIFI_PA_ETB_EN_AX2
#define RF_WIFI_PA_ETB_EN_AX2_POS                               (20U)
#define RF_WIFI_PA_ETB_EN_AX2_LEN                               (1U)
#define RF_WIFI_PA_ETB_EN_AX2_MSK                               (((1U<<RF_WIFI_PA_ETB_EN_AX2_LEN)-1)<<RF_WIFI_PA_ETB_EN_AX2_POS)
#define RF_WIFI_PA_ETB_EN_AX2_UMSK                              (~(((1U<<RF_WIFI_PA_ETB_EN_AX2_LEN)-1)<<RF_WIFI_PA_ETB_EN_AX2_POS))
#define RF_WIFI_PA_IB_FIX_AX2                                   RF_WIFI_PA_IB_FIX_AX2
#define RF_WIFI_PA_IB_FIX_AX2_POS                               (21U)
#define RF_WIFI_PA_IB_FIX_AX2_LEN                               (1U)
#define RF_WIFI_PA_IB_FIX_AX2_MSK                               (((1U<<RF_WIFI_PA_IB_FIX_AX2_LEN)-1)<<RF_WIFI_PA_IB_FIX_AX2_POS)
#define RF_WIFI_PA_IB_FIX_AX2_UMSK                              (~(((1U<<RF_WIFI_PA_IB_FIX_AX2_LEN)-1)<<RF_WIFI_PA_IB_FIX_AX2_POS))

/* 0x7D4 : pa_config_ax_sub_mode3 */
#define RF_PA_CONFIG_AX_SUB_MODE3_OFFSET                        (0x7D4)
#define RF_WIFI_PA_VBCAS_AX3                                    RF_WIFI_PA_VBCAS_AX3
#define RF_WIFI_PA_VBCAS_AX3_POS                                (0U)
#define RF_WIFI_PA_VBCAS_AX3_LEN                                (3U)
#define RF_WIFI_PA_VBCAS_AX3_MSK                                (((1U<<RF_WIFI_PA_VBCAS_AX3_LEN)-1)<<RF_WIFI_PA_VBCAS_AX3_POS)
#define RF_WIFI_PA_VBCAS_AX3_UMSK                               (~(((1U<<RF_WIFI_PA_VBCAS_AX3_LEN)-1)<<RF_WIFI_PA_VBCAS_AX3_POS))
#define RF_WIFI_PA_IAQ_AX3                                      RF_WIFI_PA_IAQ_AX3
#define RF_WIFI_PA_IAQ_AX3_POS                                  (3U)
#define RF_WIFI_PA_IAQ_AX3_LEN                                  (3U)
#define RF_WIFI_PA_IAQ_AX3_MSK                                  (((1U<<RF_WIFI_PA_IAQ_AX3_LEN)-1)<<RF_WIFI_PA_IAQ_AX3_POS)
#define RF_WIFI_PA_IAQ_AX3_UMSK                                 (~(((1U<<RF_WIFI_PA_IAQ_AX3_LEN)-1)<<RF_WIFI_PA_IAQ_AX3_POS))
#define RF_WIFI_PA_IET_AX3                                      RF_WIFI_PA_IET_AX3
#define RF_WIFI_PA_IET_AX3_POS                                  (6U)
#define RF_WIFI_PA_IET_AX3_LEN                                  (4U)
#define RF_WIFI_PA_IET_AX3_MSK                                  (((1U<<RF_WIFI_PA_IET_AX3_LEN)-1)<<RF_WIFI_PA_IET_AX3_POS)
#define RF_WIFI_PA_IET_AX3_UMSK                                 (~(((1U<<RF_WIFI_PA_IET_AX3_LEN)-1)<<RF_WIFI_PA_IET_AX3_POS))
#define RF_WIFI_PA_VBPMOS_IET_AX3                               RF_WIFI_PA_VBPMOS_IET_AX3
#define RF_WIFI_PA_VBPMOS_IET_AX3_POS                           (10U)
#define RF_WIFI_PA_VBPMOS_IET_AX3_LEN                           (2U)
#define RF_WIFI_PA_VBPMOS_IET_AX3_MSK                           (((1U<<RF_WIFI_PA_VBPMOS_IET_AX3_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_AX3_POS)
#define RF_WIFI_PA_VBPMOS_IET_AX3_UMSK                          (~(((1U<<RF_WIFI_PA_VBPMOS_IET_AX3_LEN)-1)<<RF_WIFI_PA_VBPMOS_IET_AX3_POS))
#define RF_WIFI_PA_VBPMOS_AX3                                   RF_WIFI_PA_VBPMOS_AX3
#define RF_WIFI_PA_VBPMOS_AX3_POS                               (12U)
#define RF_WIFI_PA_VBPMOS_AX3_LEN                               (4U)
#define RF_WIFI_PA_VBPMOS_AX3_MSK                               (((1U<<RF_WIFI_PA_VBPMOS_AX3_LEN)-1)<<RF_WIFI_PA_VBPMOS_AX3_POS)
#define RF_WIFI_PA_VBPMOS_AX3_UMSK                              (~(((1U<<RF_WIFI_PA_VBPMOS_AX3_LEN)-1)<<RF_WIFI_PA_VBPMOS_AX3_POS))
#define RF_WIFI_PA_VBCORE_AX3                                   RF_WIFI_PA_VBCORE_AX3
#define RF_WIFI_PA_VBCORE_AX3_POS                               (16U)
#define RF_WIFI_PA_VBCORE_AX3_LEN                               (4U)
#define RF_WIFI_PA_VBCORE_AX3_MSK                               (((1U<<RF_WIFI_PA_VBCORE_AX3_LEN)-1)<<RF_WIFI_PA_VBCORE_AX3_POS)
#define RF_WIFI_PA_VBCORE_AX3_UMSK                              (~(((1U<<RF_WIFI_PA_VBCORE_AX3_LEN)-1)<<RF_WIFI_PA_VBCORE_AX3_POS))
#define RF_WIFI_PA_ETB_EN_AX3                                   RF_WIFI_PA_ETB_EN_AX3
#define RF_WIFI_PA_ETB_EN_AX3_POS                               (20U)
#define RF_WIFI_PA_ETB_EN_AX3_LEN                               (1U)
#define RF_WIFI_PA_ETB_EN_AX3_MSK                               (((1U<<RF_WIFI_PA_ETB_EN_AX3_LEN)-1)<<RF_WIFI_PA_ETB_EN_AX3_POS)
#define RF_WIFI_PA_ETB_EN_AX3_UMSK                              (~(((1U<<RF_WIFI_PA_ETB_EN_AX3_LEN)-1)<<RF_WIFI_PA_ETB_EN_AX3_POS))
#define RF_WIFI_PA_IB_FIX_AX3                                   RF_WIFI_PA_IB_FIX_AX3
#define RF_WIFI_PA_IB_FIX_AX3_POS                               (21U)
#define RF_WIFI_PA_IB_FIX_AX3_LEN                               (1U)
#define RF_WIFI_PA_IB_FIX_AX3_MSK                               (((1U<<RF_WIFI_PA_IB_FIX_AX3_LEN)-1)<<RF_WIFI_PA_IB_FIX_AX3_POS)
#define RF_WIFI_PA_IB_FIX_AX3_UMSK                              (~(((1U<<RF_WIFI_PA_IB_FIX_AX3_LEN)-1)<<RF_WIFI_PA_IB_FIX_AX3_POS))

/* 0x7D8 : pa_config_bz */
#define RF_PA_CONFIG_BZ_OFFSET                                  (0x7D8)
#define RF_BZ_PA_VBCAS                                          RF_BZ_PA_VBCAS
#define RF_BZ_PA_VBCAS_POS                                      (0U)
#define RF_BZ_PA_VBCAS_LEN                                      (3U)
#define RF_BZ_PA_VBCAS_MSK                                      (((1U<<RF_BZ_PA_VBCAS_LEN)-1)<<RF_BZ_PA_VBCAS_POS)
#define RF_BZ_PA_VBCAS_UMSK                                     (~(((1U<<RF_BZ_PA_VBCAS_LEN)-1)<<RF_BZ_PA_VBCAS_POS))
#define RF_BZ_PA_IAQ                                            RF_BZ_PA_IAQ
#define RF_BZ_PA_IAQ_POS                                        (3U)
#define RF_BZ_PA_IAQ_LEN                                        (3U)
#define RF_BZ_PA_IAQ_MSK                                        (((1U<<RF_BZ_PA_IAQ_LEN)-1)<<RF_BZ_PA_IAQ_POS)
#define RF_BZ_PA_IAQ_UMSK                                       (~(((1U<<RF_BZ_PA_IAQ_LEN)-1)<<RF_BZ_PA_IAQ_POS))
#define RF_BZ_PA_IET                                            RF_BZ_PA_IET
#define RF_BZ_PA_IET_POS                                        (6U)
#define RF_BZ_PA_IET_LEN                                        (4U)
#define RF_BZ_PA_IET_MSK                                        (((1U<<RF_BZ_PA_IET_LEN)-1)<<RF_BZ_PA_IET_POS)
#define RF_BZ_PA_IET_UMSK                                       (~(((1U<<RF_BZ_PA_IET_LEN)-1)<<RF_BZ_PA_IET_POS))
#define RF_BZ_PA_VBPMOS_IET                                     RF_BZ_PA_VBPMOS_IET
#define RF_BZ_PA_VBPMOS_IET_POS                                 (10U)
#define RF_BZ_PA_VBPMOS_IET_LEN                                 (2U)
#define RF_BZ_PA_VBPMOS_IET_MSK                                 (((1U<<RF_BZ_PA_VBPMOS_IET_LEN)-1)<<RF_BZ_PA_VBPMOS_IET_POS)
#define RF_BZ_PA_VBPMOS_IET_UMSK                                (~(((1U<<RF_BZ_PA_VBPMOS_IET_LEN)-1)<<RF_BZ_PA_VBPMOS_IET_POS))
#define RF_BZ_PA_VBPMOS                                         RF_BZ_PA_VBPMOS
#define RF_BZ_PA_VBPMOS_POS                                     (12U)
#define RF_BZ_PA_VBPMOS_LEN                                     (4U)
#define RF_BZ_PA_VBPMOS_MSK                                     (((1U<<RF_BZ_PA_VBPMOS_LEN)-1)<<RF_BZ_PA_VBPMOS_POS)
#define RF_BZ_PA_VBPMOS_UMSK                                    (~(((1U<<RF_BZ_PA_VBPMOS_LEN)-1)<<RF_BZ_PA_VBPMOS_POS))
#define RF_BZ_PA_VBCORE                                         RF_BZ_PA_VBCORE
#define RF_BZ_PA_VBCORE_POS                                     (16U)
#define RF_BZ_PA_VBCORE_LEN                                     (4U)
#define RF_BZ_PA_VBCORE_MSK                                     (((1U<<RF_BZ_PA_VBCORE_LEN)-1)<<RF_BZ_PA_VBCORE_POS)
#define RF_BZ_PA_VBCORE_UMSK                                    (~(((1U<<RF_BZ_PA_VBCORE_LEN)-1)<<RF_BZ_PA_VBCORE_POS))
#define RF_BZ_PA_ETB_EN                                         RF_BZ_PA_ETB_EN
#define RF_BZ_PA_ETB_EN_POS                                     (20U)
#define RF_BZ_PA_ETB_EN_LEN                                     (1U)
#define RF_BZ_PA_ETB_EN_MSK                                     (((1U<<RF_BZ_PA_ETB_EN_LEN)-1)<<RF_BZ_PA_ETB_EN_POS)
#define RF_BZ_PA_ETB_EN_UMSK                                    (~(((1U<<RF_BZ_PA_ETB_EN_LEN)-1)<<RF_BZ_PA_ETB_EN_POS))
#define RF_BZ_PA_IB_FIX                                         RF_BZ_PA_IB_FIX
#define RF_BZ_PA_IB_FIX_POS                                     (21U)
#define RF_BZ_PA_IB_FIX_LEN                                     (1U)
#define RF_BZ_PA_IB_FIX_MSK                                     (((1U<<RF_BZ_PA_IB_FIX_LEN)-1)<<RF_BZ_PA_IB_FIX_POS)
#define RF_BZ_PA_IB_FIX_UMSK                                    (~(((1U<<RF_BZ_PA_IB_FIX_LEN)-1)<<RF_BZ_PA_IB_FIX_POS))

/* 0x7DC : tx_dpd_sram_ctrl_0 */
#define RF_TX_DPD_SRAM_CTRL_0_OFFSET                            (0x7DC)
#define RF_TX_DPD_PAGE_NUM                                      RF_TX_DPD_PAGE_NUM
#define RF_TX_DPD_PAGE_NUM_POS                                  (0U)
#define RF_TX_DPD_PAGE_NUM_LEN                                  (3U)
#define RF_TX_DPD_PAGE_NUM_MSK                                  (((1U<<RF_TX_DPD_PAGE_NUM_LEN)-1)<<RF_TX_DPD_PAGE_NUM_POS)
#define RF_TX_DPD_PAGE_NUM_UMSK                                 (~(((1U<<RF_TX_DPD_PAGE_NUM_LEN)-1)<<RF_TX_DPD_PAGE_NUM_POS))
#define RF_TX_DPD_AMP_MANU_READ_ADDR                            RF_TX_DPD_AMP_MANU_READ_ADDR
#define RF_TX_DPD_AMP_MANU_READ_ADDR_POS                        (12U)
#define RF_TX_DPD_AMP_MANU_READ_ADDR_LEN                        (8U)
#define RF_TX_DPD_AMP_MANU_READ_ADDR_MSK                        (((1U<<RF_TX_DPD_AMP_MANU_READ_ADDR_LEN)-1)<<RF_TX_DPD_AMP_MANU_READ_ADDR_POS)
#define RF_TX_DPD_AMP_MANU_READ_ADDR_UMSK                       (~(((1U<<RF_TX_DPD_AMP_MANU_READ_ADDR_LEN)-1)<<RF_TX_DPD_AMP_MANU_READ_ADDR_POS))
#define RF_TX_DPD_PHA_MANU_READ_ADDR                            RF_TX_DPD_PHA_MANU_READ_ADDR
#define RF_TX_DPD_PHA_MANU_READ_ADDR_POS                        (20U)
#define RF_TX_DPD_PHA_MANU_READ_ADDR_LEN                        (8U)
#define RF_TX_DPD_PHA_MANU_READ_ADDR_MSK                        (((1U<<RF_TX_DPD_PHA_MANU_READ_ADDR_LEN)-1)<<RF_TX_DPD_PHA_MANU_READ_ADDR_POS)
#define RF_TX_DPD_PHA_MANU_READ_ADDR_UMSK                       (~(((1U<<RF_TX_DPD_PHA_MANU_READ_ADDR_LEN)-1)<<RF_TX_DPD_PHA_MANU_READ_ADDR_POS))
#define RF_TX_DPD_AMP_MANU_READ_EN                              RF_TX_DPD_AMP_MANU_READ_EN
#define RF_TX_DPD_AMP_MANU_READ_EN_POS                          (28U)
#define RF_TX_DPD_AMP_MANU_READ_EN_LEN                          (1U)
#define RF_TX_DPD_AMP_MANU_READ_EN_MSK                          (((1U<<RF_TX_DPD_AMP_MANU_READ_EN_LEN)-1)<<RF_TX_DPD_AMP_MANU_READ_EN_POS)
#define RF_TX_DPD_AMP_MANU_READ_EN_UMSK                         (~(((1U<<RF_TX_DPD_AMP_MANU_READ_EN_LEN)-1)<<RF_TX_DPD_AMP_MANU_READ_EN_POS))
#define RF_TX_DPD_PHA_MANU_READ_EN                              RF_TX_DPD_PHA_MANU_READ_EN
#define RF_TX_DPD_PHA_MANU_READ_EN_POS                          (29U)
#define RF_TX_DPD_PHA_MANU_READ_EN_LEN                          (1U)
#define RF_TX_DPD_PHA_MANU_READ_EN_MSK                          (((1U<<RF_TX_DPD_PHA_MANU_READ_EN_LEN)-1)<<RF_TX_DPD_PHA_MANU_READ_EN_POS)
#define RF_TX_DPD_PHA_MANU_READ_EN_UMSK                         (~(((1U<<RF_TX_DPD_PHA_MANU_READ_EN_LEN)-1)<<RF_TX_DPD_PHA_MANU_READ_EN_POS))

/* 0x7E0 : tx_dpd_sram_ctrl_1 */
#define RF_TX_DPD_SRAM_CTRL_1_OFFSET                            (0x7E0)
#define RF_TX_DPD_AMP_SPRAM_RDDATA                              RF_TX_DPD_AMP_SPRAM_RDDATA
#define RF_TX_DPD_AMP_SPRAM_RDDATA_POS                          (0U)
#define RF_TX_DPD_AMP_SPRAM_RDDATA_LEN                          (24U)
#define RF_TX_DPD_AMP_SPRAM_RDDATA_MSK                          (((1U<<RF_TX_DPD_AMP_SPRAM_RDDATA_LEN)-1)<<RF_TX_DPD_AMP_SPRAM_RDDATA_POS)
#define RF_TX_DPD_AMP_SPRAM_RDDATA_UMSK                         (~(((1U<<RF_TX_DPD_AMP_SPRAM_RDDATA_LEN)-1)<<RF_TX_DPD_AMP_SPRAM_RDDATA_POS))

/* 0x7E4 : tx_dpd_sram_ctrl_2 */
#define RF_TX_DPD_SRAM_CTRL_2_OFFSET                            (0x7E4)
#define RF_TX_DPD_PHA_SPRAM_RDDATA                              RF_TX_DPD_PHA_SPRAM_RDDATA
#define RF_TX_DPD_PHA_SPRAM_RDDATA_POS                          (0U)
#define RF_TX_DPD_PHA_SPRAM_RDDATA_LEN                          (28U)
#define RF_TX_DPD_PHA_SPRAM_RDDATA_MSK                          (((1U<<RF_TX_DPD_PHA_SPRAM_RDDATA_LEN)-1)<<RF_TX_DPD_PHA_SPRAM_RDDATA_POS)
#define RF_TX_DPD_PHA_SPRAM_RDDATA_UMSK                         (~(((1U<<RF_TX_DPD_PHA_SPRAM_RDDATA_LEN)-1)<<RF_TX_DPD_PHA_SPRAM_RDDATA_POS))

/* 0x800 : tx_dpd_ctrl_0 */
#define RF_TX_DPD_CTRL_0_OFFSET                                 (0x800)
#define RF_TX_DPD_EN                                            RF_TX_DPD_EN
#define RF_TX_DPD_EN_POS                                        (0U)
#define RF_TX_DPD_EN_LEN                                        (1U)
#define RF_TX_DPD_EN_MSK                                        (((1U<<RF_TX_DPD_EN_LEN)-1)<<RF_TX_DPD_EN_POS)
#define RF_TX_DPD_EN_UMSK                                       (~(((1U<<RF_TX_DPD_EN_LEN)-1)<<RF_TX_DPD_EN_POS))
#define RF_TX_DPD_AMON                                          RF_TX_DPD_AMON
#define RF_TX_DPD_AMON_POS                                      (1U)
#define RF_TX_DPD_AMON_LEN                                      (1U)
#define RF_TX_DPD_AMON_MSK                                      (((1U<<RF_TX_DPD_AMON_LEN)-1)<<RF_TX_DPD_AMON_POS)
#define RF_TX_DPD_AMON_UMSK                                     (~(((1U<<RF_TX_DPD_AMON_LEN)-1)<<RF_TX_DPD_AMON_POS))
#define RF_TX_DPD_PMON                                          RF_TX_DPD_PMON
#define RF_TX_DPD_PMON_POS                                      (2U)
#define RF_TX_DPD_PMON_LEN                                      (1U)
#define RF_TX_DPD_PMON_MSK                                      (((1U<<RF_TX_DPD_PMON_LEN)-1)<<RF_TX_DPD_PMON_POS)
#define RF_TX_DPD_PMON_UMSK                                     (~(((1U<<RF_TX_DPD_PMON_LEN)-1)<<RF_TX_DPD_PMON_POS))
#define RF_TX_DPD_IN_SCAL                                       RF_TX_DPD_IN_SCAL
#define RF_TX_DPD_IN_SCAL_POS                                   (3U)
#define RF_TX_DPD_IN_SCAL_LEN                                   (6U)
#define RF_TX_DPD_IN_SCAL_MSK                                   (((1U<<RF_TX_DPD_IN_SCAL_LEN)-1)<<RF_TX_DPD_IN_SCAL_POS)
#define RF_TX_DPD_IN_SCAL_UMSK                                  (~(((1U<<RF_TX_DPD_IN_SCAL_LEN)-1)<<RF_TX_DPD_IN_SCAL_POS))
#define RF_TX_DPD_IN_SCAL_EN                                    RF_TX_DPD_IN_SCAL_EN
#define RF_TX_DPD_IN_SCAL_EN_POS                                (9U)
#define RF_TX_DPD_IN_SCAL_EN_LEN                                (1U)
#define RF_TX_DPD_IN_SCAL_EN_MSK                                (((1U<<RF_TX_DPD_IN_SCAL_EN_LEN)-1)<<RF_TX_DPD_IN_SCAL_EN_POS)
#define RF_TX_DPD_IN_SCAL_EN_UMSK                               (~(((1U<<RF_TX_DPD_IN_SCAL_EN_LEN)-1)<<RF_TX_DPD_IN_SCAL_EN_POS))
#define RF_TX_DPD_LUT_P_MODE                                    RF_TX_DPD_LUT_P_MODE
#define RF_TX_DPD_LUT_P_MODE_POS                                (10U)
#define RF_TX_DPD_LUT_P_MODE_LEN                                (1U)
#define RF_TX_DPD_LUT_P_MODE_MSK                                (((1U<<RF_TX_DPD_LUT_P_MODE_LEN)-1)<<RF_TX_DPD_LUT_P_MODE_POS)
#define RF_TX_DPD_LUT_P_MODE_UMSK                               (~(((1U<<RF_TX_DPD_LUT_P_MODE_LEN)-1)<<RF_TX_DPD_LUT_P_MODE_POS))
#define RF_TX_DPD_LUT_X_MODE                                    RF_TX_DPD_LUT_X_MODE
#define RF_TX_DPD_LUT_X_MODE_POS                                (11U)
#define RF_TX_DPD_LUT_X_MODE_LEN                                (1U)
#define RF_TX_DPD_LUT_X_MODE_MSK                                (((1U<<RF_TX_DPD_LUT_X_MODE_LEN)-1)<<RF_TX_DPD_LUT_X_MODE_POS)
#define RF_TX_DPD_LUT_X_MODE_UMSK                               (~(((1U<<RF_TX_DPD_LUT_X_MODE_LEN)-1)<<RF_TX_DPD_LUT_X_MODE_POS))
#define RF_TX_DPD_LUT_MODE                                      RF_TX_DPD_LUT_MODE
#define RF_TX_DPD_LUT_MODE_POS                                  (12U)
#define RF_TX_DPD_LUT_MODE_LEN                                  (1U)
#define RF_TX_DPD_LUT_MODE_MSK                                  (((1U<<RF_TX_DPD_LUT_MODE_LEN)-1)<<RF_TX_DPD_LUT_MODE_POS)
#define RF_TX_DPD_LUT_MODE_UMSK                                 (~(((1U<<RF_TX_DPD_LUT_MODE_LEN)-1)<<RF_TX_DPD_LUT_MODE_POS))
#define RF_TX_DPD_CTRL_HW                                       RF_TX_DPD_CTRL_HW
#define RF_TX_DPD_CTRL_HW_POS                                   (13U)
#define RF_TX_DPD_CTRL_HW_LEN                                   (1U)
#define RF_TX_DPD_CTRL_HW_MSK                                   (((1U<<RF_TX_DPD_CTRL_HW_LEN)-1)<<RF_TX_DPD_CTRL_HW_POS)
#define RF_TX_DPD_CTRL_HW_UMSK                                  (~(((1U<<RF_TX_DPD_CTRL_HW_LEN)-1)<<RF_TX_DPD_CTRL_HW_POS))
#define RF_TX_DPD_LP_CTRL_ON                                    RF_TX_DPD_LP_CTRL_ON
#define RF_TX_DPD_LP_CTRL_ON_POS                                (14U)
#define RF_TX_DPD_LP_CTRL_ON_LEN                                (1U)
#define RF_TX_DPD_LP_CTRL_ON_MSK                                (((1U<<RF_TX_DPD_LP_CTRL_ON_LEN)-1)<<RF_TX_DPD_LP_CTRL_ON_POS)
#define RF_TX_DPD_LP_CTRL_ON_UMSK                               (~(((1U<<RF_TX_DPD_LP_CTRL_ON_LEN)-1)<<RF_TX_DPD_LP_CTRL_ON_POS))
#define RF_TX_DPD_LP_TH                                         RF_TX_DPD_LP_TH
#define RF_TX_DPD_LP_TH_POS                                     (15U)
#define RF_TX_DPD_LP_TH_LEN                                     (6U)
#define RF_TX_DPD_LP_TH_MSK                                     (((1U<<RF_TX_DPD_LP_TH_LEN)-1)<<RF_TX_DPD_LP_TH_POS)
#define RF_TX_DPD_LP_TH_UMSK                                    (~(((1U<<RF_TX_DPD_LP_TH_LEN)-1)<<RF_TX_DPD_LP_TH_POS))
#define RF_TX_DPD_COMP_MODE                                     RF_TX_DPD_COMP_MODE
#define RF_TX_DPD_COMP_MODE_POS                                 (21U)
#define RF_TX_DPD_COMP_MODE_LEN                                 (2U)
#define RF_TX_DPD_COMP_MODE_MSK                                 (((1U<<RF_TX_DPD_COMP_MODE_LEN)-1)<<RF_TX_DPD_COMP_MODE_POS)
#define RF_TX_DPD_COMP_MODE_UMSK                                (~(((1U<<RF_TX_DPD_COMP_MODE_LEN)-1)<<RF_TX_DPD_COMP_MODE_POS))

/* 0x804 : tx_dpd_ctrl_1 */
#define RF_TX_DPD_CTRL_1_OFFSET                                 (0x804)
#define RF_TX_DPD_EN_OFDM                                       RF_TX_DPD_EN_OFDM
#define RF_TX_DPD_EN_OFDM_POS                                   (0U)
#define RF_TX_DPD_EN_OFDM_LEN                                   (8U)
#define RF_TX_DPD_EN_OFDM_MSK                                   (((1U<<RF_TX_DPD_EN_OFDM_LEN)-1)<<RF_TX_DPD_EN_OFDM_POS)
#define RF_TX_DPD_EN_OFDM_UMSK                                  (~(((1U<<RF_TX_DPD_EN_OFDM_LEN)-1)<<RF_TX_DPD_EN_OFDM_POS))
#define RF_TX_DPD_EN_DSSS                                       RF_TX_DPD_EN_DSSS
#define RF_TX_DPD_EN_DSSS_POS                                   (8U)
#define RF_TX_DPD_EN_DSSS_LEN                                   (4U)
#define RF_TX_DPD_EN_DSSS_MSK                                   (((1U<<RF_TX_DPD_EN_DSSS_LEN)-1)<<RF_TX_DPD_EN_DSSS_POS)
#define RF_TX_DPD_EN_DSSS_UMSK                                  (~(((1U<<RF_TX_DPD_EN_DSSS_LEN)-1)<<RF_TX_DPD_EN_DSSS_POS))

/* 0xA10 : rx_dpd_ctrl_0 */
#define RF_RX_DPD_CTRL_0_OFFSET                                 (0xA10)
#define RF_RX_DPD_DONE                                          RF_RX_DPD_DONE
#define RF_RX_DPD_DONE_POS                                      (0U)
#define RF_RX_DPD_DONE_LEN                                      (1U)
#define RF_RX_DPD_DONE_MSK                                      (((1U<<RF_RX_DPD_DONE_LEN)-1)<<RF_RX_DPD_DONE_POS)
#define RF_RX_DPD_DONE_UMSK                                     (~(((1U<<RF_RX_DPD_DONE_LEN)-1)<<RF_RX_DPD_DONE_POS))
#define RF_RX_DPD_I_PATH_ONLY                                   RF_RX_DPD_I_PATH_ONLY
#define RF_RX_DPD_I_PATH_ONLY_POS                               (1U)
#define RF_RX_DPD_I_PATH_ONLY_LEN                               (1U)
#define RF_RX_DPD_I_PATH_ONLY_MSK                               (((1U<<RF_RX_DPD_I_PATH_ONLY_LEN)-1)<<RF_RX_DPD_I_PATH_ONLY_POS)
#define RF_RX_DPD_I_PATH_ONLY_UMSK                              (~(((1U<<RF_RX_DPD_I_PATH_ONLY_LEN)-1)<<RF_RX_DPD_I_PATH_ONLY_POS))
#define RF_RX_DPD_BYPASS_FILTER                                 RF_RX_DPD_BYPASS_FILTER
#define RF_RX_DPD_BYPASS_FILTER_POS                             (2U)
#define RF_RX_DPD_BYPASS_FILTER_LEN                             (1U)
#define RF_RX_DPD_BYPASS_FILTER_MSK                             (((1U<<RF_RX_DPD_BYPASS_FILTER_LEN)-1)<<RF_RX_DPD_BYPASS_FILTER_POS)
#define RF_RX_DPD_BYPASS_FILTER_UMSK                            (~(((1U<<RF_RX_DPD_BYPASS_FILTER_LEN)-1)<<RF_RX_DPD_BYPASS_FILTER_POS))
#define RF_RX_DPD_WAIT_TIME_US                                  RF_RX_DPD_WAIT_TIME_US
#define RF_RX_DPD_WAIT_TIME_US_POS                              (3U)
#define RF_RX_DPD_WAIT_TIME_US_LEN                              (5U)
#define RF_RX_DPD_WAIT_TIME_US_MSK                              (((1U<<RF_RX_DPD_WAIT_TIME_US_LEN)-1)<<RF_RX_DPD_WAIT_TIME_US_POS)
#define RF_RX_DPD_WAIT_TIME_US_UMSK                             (~(((1U<<RF_RX_DPD_WAIT_TIME_US_LEN)-1)<<RF_RX_DPD_WAIT_TIME_US_POS))
#define RF_RX_DPD_FC_MHZ                                        RF_RX_DPD_FC_MHZ
#define RF_RX_DPD_FC_MHZ_POS                                    (8U)
#define RF_RX_DPD_FC_MHZ_LEN                                    (5U)
#define RF_RX_DPD_FC_MHZ_MSK                                    (((1U<<RF_RX_DPD_FC_MHZ_LEN)-1)<<RF_RX_DPD_FC_MHZ_POS)
#define RF_RX_DPD_FC_MHZ_UMSK                                   (~(((1U<<RF_RX_DPD_FC_MHZ_LEN)-1)<<RF_RX_DPD_FC_MHZ_POS))
#define RF_RX_DPD_DGC_GAIN_DB                                   RF_RX_DPD_DGC_GAIN_DB
#define RF_RX_DPD_DGC_GAIN_DB_POS                               (13U)
#define RF_RX_DPD_DGC_GAIN_DB_LEN                               (5U)
#define RF_RX_DPD_DGC_GAIN_DB_MSK                               (((1U<<RF_RX_DPD_DGC_GAIN_DB_LEN)-1)<<RF_RX_DPD_DGC_GAIN_DB_POS)
#define RF_RX_DPD_DGC_GAIN_DB_UMSK                              (~(((1U<<RF_RX_DPD_DGC_GAIN_DB_LEN)-1)<<RF_RX_DPD_DGC_GAIN_DB_POS))
#define RF_RX_DPD_DGC_SHIFT                                     RF_RX_DPD_DGC_SHIFT
#define RF_RX_DPD_DGC_SHIFT_POS                                 (18U)
#define RF_RX_DPD_DGC_SHIFT_LEN                                 (3U)
#define RF_RX_DPD_DGC_SHIFT_MSK                                 (((1U<<RF_RX_DPD_DGC_SHIFT_LEN)-1)<<RF_RX_DPD_DGC_SHIFT_POS)
#define RF_RX_DPD_DGC_SHIFT_UMSK                                (~(((1U<<RF_RX_DPD_DGC_SHIFT_LEN)-1)<<RF_RX_DPD_DGC_SHIFT_POS))
#define RF_RX_DPD_N_PERIOD                                      RF_RX_DPD_N_PERIOD
#define RF_RX_DPD_N_PERIOD_POS                                  (21U)
#define RF_RX_DPD_N_PERIOD_LEN                                  (3U)
#define RF_RX_DPD_N_PERIOD_MSK                                  (((1U<<RF_RX_DPD_N_PERIOD_LEN)-1)<<RF_RX_DPD_N_PERIOD_POS)
#define RF_RX_DPD_N_PERIOD_UMSK                                 (~(((1U<<RF_RX_DPD_N_PERIOD_LEN)-1)<<RF_RX_DPD_N_PERIOD_POS))
#define RF_RX_DPD_EN                                            RF_RX_DPD_EN
#define RF_RX_DPD_EN_POS                                        (24U)
#define RF_RX_DPD_EN_LEN                                        (1U)
#define RF_RX_DPD_EN_MSK                                        (((1U<<RF_RX_DPD_EN_LEN)-1)<<RF_RX_DPD_EN_POS)
#define RF_RX_DPD_EN_UMSK                                       (~(((1U<<RF_RX_DPD_EN_LEN)-1)<<RF_RX_DPD_EN_POS))
#define RF_RX_DPD_SEL                                           RF_RX_DPD_SEL
#define RF_RX_DPD_SEL_POS                                       (25U)
#define RF_RX_DPD_SEL_LEN                                       (1U)
#define RF_RX_DPD_SEL_MSK                                       (((1U<<RF_RX_DPD_SEL_LEN)-1)<<RF_RX_DPD_SEL_POS)
#define RF_RX_DPD_SEL_UMSK                                      (~(((1U<<RF_RX_DPD_SEL_LEN)-1)<<RF_RX_DPD_SEL_POS))
#define RF_RX_DPD_SRAM_EN                                       RF_RX_DPD_SRAM_EN
#define RF_RX_DPD_SRAM_EN_POS                                   (26U)
#define RF_RX_DPD_SRAM_EN_LEN                                   (1U)
#define RF_RX_DPD_SRAM_EN_MSK                                   (((1U<<RF_RX_DPD_SRAM_EN_LEN)-1)<<RF_RX_DPD_SRAM_EN_POS)
#define RF_RX_DPD_SRAM_EN_UMSK                                  (~(((1U<<RF_RX_DPD_SRAM_EN_LEN)-1)<<RF_RX_DPD_SRAM_EN_POS))
#define RF_RX_DPD_SW_RESET                                      RF_RX_DPD_SW_RESET
#define RF_RX_DPD_SW_RESET_POS                                  (27U)
#define RF_RX_DPD_SW_RESET_LEN                                  (1U)
#define RF_RX_DPD_SW_RESET_MSK                                  (((1U<<RF_RX_DPD_SW_RESET_LEN)-1)<<RF_RX_DPD_SW_RESET_POS)
#define RF_RX_DPD_SW_RESET_UMSK                                 (~(((1U<<RF_RX_DPD_SW_RESET_LEN)-1)<<RF_RX_DPD_SW_RESET_POS))
#define RF_SRAM_CLK_4S_EN                                       RF_SRAM_CLK_4S_EN
#define RF_SRAM_CLK_4S_EN_POS                                   (31U)
#define RF_SRAM_CLK_4S_EN_LEN                                   (1U)
#define RF_SRAM_CLK_4S_EN_MSK                                   (((1U<<RF_SRAM_CLK_4S_EN_LEN)-1)<<RF_SRAM_CLK_4S_EN_POS)
#define RF_SRAM_CLK_4S_EN_UMSK                                  (~(((1U<<RF_SRAM_CLK_4S_EN_LEN)-1)<<RF_SRAM_CLK_4S_EN_POS))

/* 0xA14 : rx_dpd_ctrl_1 */
#define RF_RX_DPD_CTRL_1_OFFSET                                 (0xA14)
#define RF_RX_DPD_SAMPLE_OFFSET                                 RF_RX_DPD_SAMPLE_OFFSET
#define RF_RX_DPD_SAMPLE_OFFSET_POS                             (0U)
#define RF_RX_DPD_SAMPLE_OFFSET_LEN                             (8U)
#define RF_RX_DPD_SAMPLE_OFFSET_MSK                             (((1U<<RF_RX_DPD_SAMPLE_OFFSET_LEN)-1)<<RF_RX_DPD_SAMPLE_OFFSET_POS)
#define RF_RX_DPD_SAMPLE_OFFSET_UMSK                            (~(((1U<<RF_RX_DPD_SAMPLE_OFFSET_LEN)-1)<<RF_RX_DPD_SAMPLE_OFFSET_POS))
#define RF_RX_DPD_SYNC_MF_OUT_MAG_TH                            RF_RX_DPD_SYNC_MF_OUT_MAG_TH
#define RF_RX_DPD_SYNC_MF_OUT_MAG_TH_POS                        (8U)
#define RF_RX_DPD_SYNC_MF_OUT_MAG_TH_LEN                        (2U)
#define RF_RX_DPD_SYNC_MF_OUT_MAG_TH_MSK                        (((1U<<RF_RX_DPD_SYNC_MF_OUT_MAG_TH_LEN)-1)<<RF_RX_DPD_SYNC_MF_OUT_MAG_TH_POS)
#define RF_RX_DPD_SYNC_MF_OUT_MAG_TH_UMSK                       (~(((1U<<RF_RX_DPD_SYNC_MF_OUT_MAG_TH_LEN)-1)<<RF_RX_DPD_SYNC_MF_OUT_MAG_TH_POS))
#define RF_RX_DPD_SYNC_MF_IN_MAG_TH                             RF_RX_DPD_SYNC_MF_IN_MAG_TH
#define RF_RX_DPD_SYNC_MF_IN_MAG_TH_POS                         (10U)
#define RF_RX_DPD_SYNC_MF_IN_MAG_TH_LEN                         (2U)
#define RF_RX_DPD_SYNC_MF_IN_MAG_TH_MSK                         (((1U<<RF_RX_DPD_SYNC_MF_IN_MAG_TH_LEN)-1)<<RF_RX_DPD_SYNC_MF_IN_MAG_TH_POS)
#define RF_RX_DPD_SYNC_MF_IN_MAG_TH_UMSK                        (~(((1U<<RF_RX_DPD_SYNC_MF_IN_MAG_TH_LEN)-1)<<RF_RX_DPD_SYNC_MF_IN_MAG_TH_POS))
#define RF_RX_DPD_USE_SYNC_MF                                   RF_RX_DPD_USE_SYNC_MF
#define RF_RX_DPD_USE_SYNC_MF_POS                               (12U)
#define RF_RX_DPD_USE_SYNC_MF_LEN                               (1U)
#define RF_RX_DPD_USE_SYNC_MF_MSK                               (((1U<<RF_RX_DPD_USE_SYNC_MF_LEN)-1)<<RF_RX_DPD_USE_SYNC_MF_POS)
#define RF_RX_DPD_USE_SYNC_MF_UMSK                              (~(((1U<<RF_RX_DPD_USE_SYNC_MF_LEN)-1)<<RF_RX_DPD_USE_SYNC_MF_POS))
#define RF_RX_DPD_DLY_TH                                        RF_RX_DPD_DLY_TH
#define RF_RX_DPD_DLY_TH_POS                                    (16U)
#define RF_RX_DPD_DLY_TH_LEN                                    (10U)
#define RF_RX_DPD_DLY_TH_MSK                                    (((1U<<RF_RX_DPD_DLY_TH_LEN)-1)<<RF_RX_DPD_DLY_TH_POS)
#define RF_RX_DPD_DLY_TH_UMSK                                   (~(((1U<<RF_RX_DPD_DLY_TH_LEN)-1)<<RF_RX_DPD_DLY_TH_POS))


struct  rf_reg {
    /* 0x0 : Silicon revision */
    union {
        struct {
            uint32_t rf_id                          :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t fw_rev                         :  8; /* [15: 8],          r,        0x0 */
            uint32_t hw_rev                         :  8; /* [23:16],          r,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_rev;

    /* 0x4 : Digital Control */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t rf_fsm_ctrl_en                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t rf_fsm_t2r_cal_mode            :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t rf_fsm_state                   :  3; /* [ 6: 4],          r,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rf_rc_state_dbg                :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t rf_rc_state_dbg_en             :  1; /* [   11],        r/w,        0x0 */
            uint32_t rf_fsm_st_int_sel              :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rf_fsm_st_int                  :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t rf_fsm_st_int_clr              :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t rf_fsm_st_int_set              :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t rf_rc_state_value              :  3; /* [30:28],          r,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl_hw;

    /* 0x8 : rfsm status reg */
    union {
        struct {
            uint32_t rf_fsm_sw_st                   :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t rf_fsm_sw_st_vld               :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t full_cal_en                    :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t inc_cal_timeout                :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t lo_unlocked                    :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl_sw;

    /* 0xC : Control logic switch */
    union {
        struct {
            uint32_t rf_reserved3                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rfctrl_hw_en;

    /* 0x10 : temp_comp */
    union {
        struct {
            uint32_t const_acal                     :  8; /* [ 7: 0],        r/w,       0x1a */
            uint32_t const_fcal                     :  8; /* [15: 8],        r/w,       0x74 */
            uint32_t temp_comp_en                   :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } temp_comp;

    /* 0x14 : rfcal_status */
    union {
        struct {
            uint32_t rcal_status                    :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t adc_oscal_status               :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t fcal_status                    :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t acal_status                    :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t inc_fcal_status                :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t inc_acal_status                :  2; /* [11:10],        r/w,        0x0 */
            uint32_t clkpll_cal_status              :  2; /* [13:12],        r/w,        0x0 */
            uint32_t ros_status                     :  2; /* [15:14],        r/w,        0x0 */
            uint32_t tos_status                     :  2; /* [17:16],        r/w,        0x0 */
            uint32_t rccal_status                   :  2; /* [19:18],        r/w,        0x0 */
            uint32_t lo_leakcal_status              :  2; /* [21:20],        r/w,        0x0 */
            uint32_t tiqcal_status_resv             :  2; /* [23:22],        r/w,        0x0 */
            uint32_t riqcal_status_resv             :  2; /* [25:24],        r/w,        0x0 */
            uint32_t pwdet_cal_status               :  2; /* [27:26],        r/w,        0x0 */
            uint32_t tenscal_status                 :  2; /* [29:28],        r/w,        0x0 */
            uint32_t dpd_status                     :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rfcal_status;

    /* 0x18 : rfcal_status2 */
    union {
        struct {
            uint32_t dl_rfcal_table_status          :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rfcal_status2;

    /* 0x1C : Calibration mode register */
    union {
        struct {
            uint32_t rcal_en_resv                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t adc_oscal_en                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t dl_rfcal_table_en              :  1; /* [    2],        r/w,        0x0 */
            uint32_t fcal_en                        :  1; /* [    3],        r/w,        0x0 */
            uint32_t acal_en                        :  1; /* [    4],        r/w,        0x0 */
            uint32_t fcal_inc_en                    :  1; /* [    5],        r/w,        0x1 */
            uint32_t acal_inc_en                    :  1; /* [    6],        r/w,        0x1 */
            uint32_t roscal_inc_en                  :  1; /* [    7],        r/w,        0x1 */
            uint32_t clkpll_cal_en                  :  1; /* [    8],        r/w,        0x0 */
            uint32_t roscal_en                      :  1; /* [    9],        r/w,        0x0 */
            uint32_t toscal_en                      :  1; /* [   10],        r/w,        0x0 */
            uint32_t rccal_en                       :  1; /* [   11],        r/w,        0x0 */
            uint32_t lo_leakcal_en                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t tiqcal_en                      :  1; /* [   13],        r/w,        0x0 */
            uint32_t riqcal_en                      :  1; /* [   14],        r/w,        0x0 */
            uint32_t pwdet_cal_en                   :  1; /* [   15],        r/w,        0x0 */
            uint32_t tsencal_en                     :  1; /* [   16],        r/w,        0x0 */
            uint32_t dpd_en                         :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rfcal_ctrlen;

    /* 0x20 : rf calibration state enabl in full cal list */
    union {
        struct {
            uint32_t rcal_sten_resv                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t adc_oscal_sten                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t dl_rfcal_table_sten            :  1; /* [    2],        r/w,        0x1 */
            uint32_t fcal_sten                      :  1; /* [    3],        r/w,        0x1 */
            uint32_t acal_sten                      :  1; /* [    4],        r/w,        0x1 */
            uint32_t inc_fcal_sten                  :  1; /* [    5],        r/w,        0x1 */
            uint32_t inc_acal_sten                  :  1; /* [    6],        r/w,        0x1 */
            uint32_t clkpll_cal_sten                :  1; /* [    7],        r/w,        0x1 */
            uint32_t roscal_sten                    :  1; /* [    8],        r/w,        0x1 */
            uint32_t toscal_sten_resv               :  1; /* [    9],        r/w,        0x0 */
            uint32_t rccal_sten                     :  1; /* [   10],        r/w,        0x1 */
            uint32_t lo_leakcal_sten                :  1; /* [   11],        r/w,        0x1 */
            uint32_t tiqcal_sten                    :  1; /* [   12],        r/w,        0x1 */
            uint32_t riqcal_sten                    :  1; /* [   13],        r/w,        0x0 */
            uint32_t pwdet_cal_sten                 :  1; /* [   14],        r/w,        0x1 */
            uint32_t tsencal_sten                   :  1; /* [   15],        r/w,        0x1 */
            uint32_t dpd_sten                       :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_29                 : 13; /* [29:17],       rsvd,        0x0 */
            uint32_t rfcal_level                    :  2; /* [31:30],        r/w,        0x2 */
        }BF;
        uint32_t WORD;
    } rfcal_stateen;

    /* 0x24 : SARADC Control Registers */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } saradc_resv;

    /* 0x28 : ZRF Control register 0 */
    union {
        struct {
            uint32_t aupll_sdm_rst_dly              :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t lo_sdm_rst_dly                 :  2; /* [ 3: 2],        r/w,        0x3 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t ppu_lead                       :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t pud_vco_dly                    :  2; /* [11:10],        r/w,        0x1 */
            uint32_t pud_iref_dly                   :  2; /* [13:12],        r/w,        0x1 */
            uint32_t pud_pa_dly                     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t pud_vbuf_fbdv_dly              :  2; /* [17:16],        r/w,        0x1 */
            uint32_t pud_vbuf_lodist_dly            :  2; /* [19:18],        r/w,        0x1 */
            uint32_t reserved_20_26                 :  7; /* [26:20],       rsvd,        0x0 */
            uint32_t mbg_trim                       :  2; /* [28:27],        r/w,        0x2 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_base_ctrl1;

    /* 0x2C : ZRF Control register 0 */
    union {
        struct {
            uint32_t pu_ctrl_hw                     :  1; /* [    0],        r/w,        0x1 */
            uint32_t rx_gain_ctrl_hw                :  1; /* [    1],        r/w,        0x1 */
            uint32_t tx_gain_ctrl_hw                :  1; /* [    2],        r/w,        0x1 */
            uint32_t lna_ctrl_hw                    :  1; /* [    3],        r/w,        0x1 */
            uint32_t rbb_bw_ctrl_hw                 :  1; /* [    4],        r/w,        0x1 */
            uint32_t rxcal_ctrl_hw                  :  1; /* [    5],        r/w,        0x1 */
            uint32_t lo_ctrl_hw                     :  1; /* [    6],        r/w,        0x1 */
            uint32_t inc_acal_ctrl_en_hw            :  1; /* [    7],        r/w,        0x1 */
            uint32_t inc_fcal_ctrl_en_hw            :  1; /* [    8],        r/w,        0x1 */
            uint32_t sdm_ctrl_hw                    :  1; /* [    9],        r/w,        0x1 */
            uint32_t rbb_pkdet_en_ctrl_hw           :  1; /* [   10],        r/w,        0x1 */
            uint32_t rbb_pkdet_out_rstn_ctrl_hw     :  1; /* [   11],        r/w,        0x1 */
            uint32_t adda_ctrl_hw                   :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t txcal_ctrl_hw                  :  1; /* [   14],        r/w,        0x1 */
            uint32_t reserved_15_31                 : 17; /* [31:15],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_base_ctrl2;

    /* 0x30 : pucr1 */
    union {
        struct {
            uint32_t pu_sfreg                       :  1; /* [    0],        r/w,        0x0 */
            uint32_t pu_rcal                        :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t pu_lna                         :  1; /* [    8],        r/w,        0x0 */
            uint32_t pu_rmxgm                       :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_rmx                         :  1; /* [   10],        r/w,        0x0 */
            uint32_t pu_rbb                         :  1; /* [   11],        r/w,        0x0 */
            uint32_t pu_adda_ldo                    :  1; /* [   12],        r/w,        0x0 */
            uint32_t adc_clk_en                     :  1; /* [   13],        r/w,        0x0 */
            uint32_t pu_adc                         :  1; /* [   14],        r/w,        0x0 */
            uint32_t pu_op_atest                    :  1; /* [   15],        r/w,        0x0 */
            uint32_t pu_pa                          :  1; /* [   16],        r/w,        0x0 */
            uint32_t pu_tmx                         :  1; /* [   17],        r/w,        0x0 */
            uint32_t pu_tbb                         :  1; /* [   18],        r/w,        0x0 */
            uint32_t pu_dac                         :  1; /* [   19],        r/w,        0x0 */
            uint32_t pu_vco                         :  1; /* [   20],        r/w,        0x0 */
            uint32_t pu_fbdv                        :  1; /* [   21],        r/w,        0x0 */
            uint32_t pu_pfd                         :  1; /* [   22],        r/w,        0x0 */
            uint32_t pu_osmx                        :  1; /* [   23],        r/w,        0x0 */
            uint32_t pu_rxbuf                       :  1; /* [   24],        r/w,        0x0 */
            uint32_t pu_txbuf                       :  1; /* [   25],        r/w,        0x0 */
            uint32_t trsw_en                        :  2; /* [27:26],        r/w,        0x3 */
            uint32_t pu_pkdet                       :  1; /* [   28],        r/w,        0x0 */
            uint32_t pu_rosdac                      :  1; /* [   29],        r/w,        0x0 */
            uint32_t pu_pwrmx                       :  1; /* [   30],        r/w,        0x0 */
            uint32_t pu_tosdac                      :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr1;

    /* 0x34 : read only from hardware logic */
    union {
        struct {
            uint32_t pu_sfreg_hw                    :  1; /* [    0],          r,        0x1 */
            uint32_t pu_rcal_hw                     :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t pu_lna_hw                      :  1; /* [    8],          r,        0x1 */
            uint32_t pu_rmxgm_hw                    :  1; /* [    9],          r,        0x1 */
            uint32_t pu_rmx_hw                      :  1; /* [   10],          r,        0x1 */
            uint32_t pu_rbb_hw                      :  1; /* [   11],          r,        0x1 */
            uint32_t pu_adda_ldo_hw                 :  1; /* [   12],          r,        0x1 */
            uint32_t adc_clk_en_hw                  :  1; /* [   13],          r,        0x1 */
            uint32_t pu_adc_hw                      :  1; /* [   14],          r,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pu_pa_hw                       :  1; /* [   16],          r,        0x1 */
            uint32_t pu_tmx_hw                      :  1; /* [   17],          r,        0x1 */
            uint32_t pu_tbb_hw                      :  1; /* [   18],          r,        0x1 */
            uint32_t pu_dac_hw                      :  1; /* [   19],          r,        0x1 */
            uint32_t pu_vco_hw                      :  1; /* [   20],          r,        0x1 */
            uint32_t pu_fbdv_hw                     :  1; /* [   21],          r,        0x1 */
            uint32_t pu_pfd_hw                      :  1; /* [   22],          r,        0x1 */
            uint32_t pu_osmx_hw                     :  1; /* [   23],          r,        0x1 */
            uint32_t pu_rxbuf_hw                    :  1; /* [   24],          r,        0x1 */
            uint32_t pu_txbuf_hw                    :  1; /* [   25],          r,        0x1 */
            uint32_t trsw_en_hw                     :  2; /* [27:26],          r,        0x3 */
            uint32_t pu_pkdet_hw                    :  1; /* [   28],          r,        0x1 */
            uint32_t pu_rosdac_hw                   :  1; /* [   29],          r,        0x1 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t pu_tosdac_hw                   :  1; /* [   31],          r,        0x1 */
        }BF;
        uint32_t WORD;
    } pucr1_hw;

    /* 0x38 : pucr2 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr2;

    /* 0x3C : pucr2_hw */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr2_hw;

    /* 0x40 : ppu_ctrl_hw */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t ppu_lna_hw                     :  1; /* [    8],          r,        0x1 */
            uint32_t ppu_rmxgm_hw                   :  1; /* [    9],          r,        0x1 */
            uint32_t reserved_10                    :  1; /* [   10],       rsvd,        0x0 */
            uint32_t ppu_rbb_hw                     :  1; /* [   11],          r,        0x1 */
            uint32_t reserved_12_19                 :  8; /* [19:12],       rsvd,        0x0 */
            uint32_t ppu_vco_hw                     :  1; /* [   20],          r,        0x1 */
            uint32_t ppu_fbdv_hw                    :  1; /* [   21],          r,        0x1 */
            uint32_t ppu_pfd_hw                     :  1; /* [   22],          r,        0x1 */
            uint32_t ppu_osmx_hw                    :  1; /* [   23],          r,        0x1 */
            uint32_t ppu_rxbuf_hw                   :  1; /* [   24],          r,        0x1 */
            uint32_t ppu_txbuf_hw                   :  1; /* [   25],          r,        0x1 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ppu_ctrl_hw;

    /* 0x44 : pud_ctrl_hw */
    union {
        struct {
            uint32_t reserved_0_19                  : 20; /* [19: 0],       rsvd,        0x0 */
            uint32_t pud_vco_hw                     :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pud_ctrl_hw;

    /* 0x48 : gain control1 */
    union {
        struct {
            uint32_t gc_lna                         :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t gc_rmxgm                       :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t rmxgm_ratt                     :  3; /* [ 7: 5],        r/w,        0x0 */
            uint32_t gc_rbb1                        :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gc_rbb2                        :  3; /* [14:12],        r/w,        0x6 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gc_tmx                         :  3; /* [18:16],        r/w,        0x2 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t gc_tbb                         :  5; /* [24:20],        r/w,        0x8 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t gc_tbb_boost                   :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } trx_gain1;

    /* 0x4C : trx gain hardware readback */
    union {
        struct {
            uint32_t gc_lna_hw                      :  3; /* [ 2: 0],          r,        0x7 */
            uint32_t gc_rmxgm_hw                    :  2; /* [ 4: 3],          r,        0x1 */
            uint32_t rmxgm_ratt_hw                  :  3; /* [ 7: 5],          r,        0x0 */
            uint32_t gc_rbb1_hw                     :  2; /* [ 9: 8],          r,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gc_rbb2_hw                     :  3; /* [14:12],          r,        0x6 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gc_tmx_hw                      :  3; /* [18:16],          r,        0x8 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t gc_tbb_hw                      :  5; /* [24:20],          r,        0x3 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t gc_tbb_boost_hw                :  2; /* [29:28],          r,        0x0 */
            uint32_t dac_bias_sel_hw                :  2; /* [31:30],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } trx_gain_hw;

    /* 0x50 : dc test register */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t dc_tp_33_en                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t dc_tp_25_en                    :  1; /* [    2],        r/w,        0x0 */
            uint32_t dc_tp_18_en                    :  1; /* [    3],        r/w,        0x0 */
            uint32_t dc_tp_15_en                    :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_11                  :  7; /* [11: 5],       rsvd,        0x0 */
            uint32_t ten_rrf_0                      :  1; /* [   12],        r/w,        0x0 */
            uint32_t ten_rrf_1                      :  1; /* [   13],        r/w,        0x0 */
            uint32_t ten_pa                         :  1; /* [   14],        r/w,        0x0 */
            uint32_t ten_tmx                        :  1; /* [   15],        r/w,        0x0 */
            uint32_t ten_tia                        :  1; /* [   16],        r/w,        0x0 */
            uint32_t ten_bq                         :  1; /* [   17],        r/w,        0x0 */
            uint32_t ten_atest                      :  1; /* [   18],        r/w,        0x0 */
            uint32_t ten_tbb                        :  1; /* [   19],        r/w,        0x0 */
            uint32_t ten_adc                        :  1; /* [   20],        r/w,        0x0 */
            uint32_t ten_dac_i                      :  1; /* [   21],        r/w,        0x0 */
            uint32_t ten_dac_q                      :  1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ten_vco_0                      :  1; /* [   24],        r/w,        0x0 */
            uint32_t ten_vco_1                      :  1; /* [   25],        r/w,        0x0 */
            uint32_t ten_pfdcp                      :  1; /* [   26],        r/w,        0x0 */
            uint32_t ten_lf                         :  1; /* [   27],        r/w,        0x0 */
            uint32_t ten_lodist                     :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ten_dc;

    /* 0x54 : digital test register */
    union {
        struct {
            uint32_t reserved_0_5                   :  6; /* [ 5: 0],       rsvd,        0x0 */
            uint32_t dten_lo_fsdm                   :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t dten_lo_fref                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t dtest_pull_down_rf             :  1; /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_22                 : 13; /* [22:10],       rsvd,        0x0 */
            uint32_t rf_dtest_en                    :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ten_dig;

    /* 0x58 : ac test register */
    union {
        struct {
            uint32_t atest_op_cc                    :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t atest_dac_en                   :  1; /* [    4],        r/w,        0x0 */
            uint32_t atest_in_trx_sw                :  1; /* [    5],        r/w,        0x0 */
            uint32_t atest_in_en                    :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t atest_gain_r9                  :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t atest_gain_r8                  :  2; /* [11:10],        r/w,        0x0 */
            uint32_t atest_gain_r7                  :  2; /* [13:12],        r/w,        0x0 */
            uint32_t atest_gain_r6                  :  2; /* [15:14],        r/w,        0x0 */
            uint32_t atest_gain_r5                  :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t atest_out_en_q                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t atest_out_en_i                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t atest_in_en_q                  :  1; /* [   22],        r/w,        0x0 */
            uint32_t atest_in_en_i                  :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ten_ac;

    /* 0x5C : RX normal bias mode registers */
    union {
        struct {
            uint32_t vg11_sel                       :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t vg13_sel                       :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t rcal_icx_code                  :  6; /* [ 9: 4],        r/w,       0x20 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t rcal_icx_out_en                :  1; /* [   12],        r/w,        0x0 */
            uint32_t rcal_icx_tswitch_en            :  1; /* [   13],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rcal_iptat_code                :  5; /* [20:16],        r/w,       0x10 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t rcal_iptat_out_en              :  1; /* [   24],        r/w,        0x0 */
            uint32_t rcal_iptat_tswitch_en          :  1; /* [   25],        r/w,        0x1 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cip_0;

    /* 0x60 : pa0 */
    union {
        struct {
            uint32_t pa_pwrmx_osdac_bm              :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa0;

    /* 0x64 : pa1 */
    union {
        struct {
            uint32_t reserved_0_16                  : 17; /* [16: 0],       rsvd,        0x0 */
            uint32_t pa_lz_bias_en                  :  1; /* [   17],        r/w,        0x1 */
            uint32_t pa_pwrmx_osdac                 :  5; /* [22:18],        r/w,        0x8 */
            uint32_t pa_pwrmx_dac_pn_switch         :  1; /* [   23],        r/w,        0x0 */
            uint32_t pa_pwrmx_bm                    :  3; /* [26:24],        r/w,        0x3 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t pa_att_gc                      :  4; /* [31:28],        r/w,        0x7 */
        }BF;
        uint32_t WORD;
    } pa1;

    /* 0x68 : RX normal bias mode registers */
    union {
        struct {
            uint32_t pa_vbcas                       :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t pa_iaq                         :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t pa_iet                         :  4; /* [11: 8],        r/w,        0x3 */
            uint32_t pa_vbpmos_iet                  :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t pa_vbpmos                      :  4; /* [19:16],        r/w,        0x1 */
            uint32_t pa_vbcore                      :  4; /* [23:20],        r/w,        0x6 */
            uint32_t pa_etb_en                      :  1; /* [   24],        r/w,        0x1 */
            uint32_t pa_lp_en                       :  1; /* [   25],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t pa_ib_fix                      :  1; /* [   28],        r/w,        0x0 */
            uint32_t gc_pa                          :  3; /* [31:29],        r/w,        0x7 */
        }BF;
        uint32_t WORD;
    } pa2;

    /* 0x6C : pa3 */
    union {
        struct {
            uint32_t pa_vbcas_hw                    :  3; /* [ 2: 0],          r,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t pa_iaq_hw                      :  3; /* [ 6: 4],          r,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t pa_iet_hw                      :  4; /* [11: 8],          r,        0x3 */
            uint32_t pa_vbpmos_iet_hw               :  2; /* [13:12],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t pa_vbpmos_hw                   :  4; /* [19:16],          r,        0x1 */
            uint32_t pa_vbcore_hw                   :  4; /* [23:20],          r,        0x6 */
            uint32_t pa_etb_en_hw                   :  1; /* [   24],          r,        0x1 */
            uint32_t pa_lp_en_hw                    :  1; /* [   25],          r,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t pa_ib_fix_hw                   :  1; /* [   28],          r,        0x0 */
            uint32_t gc_pa_hw                       :  3; /* [31:29],          r,        0x7 */
        }BF;
        uint32_t WORD;
    } pa3;

    /* 0x70 : tmx */
    union {
        struct {
            uint32_t tmx_cs                         :  4; /* [ 3: 0],        r/w,        0x7 */
            uint32_t tmx_bm_sw                      :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tmx_bm_cas                     :  3; /* [10: 8],        r/w,        0x6 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t tmx_bm_cas_bulk                :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tx_tsense_en                   :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t tmx_r_att                      :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tmx;

    /* 0x74 : tbb */
    union {
        struct {
            uint32_t tbb_bm_sf                      :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t tbb_bm_cg                      :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t tbb_vcm                        :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t tbb_cflt                       :  2; /* [13:12],        r/w,        0x1 */
            uint32_t tbb_iq_bias_short              :  1; /* [   14],        r/w,        0x0 */
            uint32_t tbb_atest_out_en               :  1; /* [   15],        r/w,        0x0 */
            uint32_t tbb_tosdac_q                   :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t tbb_tosdac_i                   :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tbb;

    /* 0x78 : lna */
    union {
        struct {
            uint32_t lna_bm                         :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t lna_bm_hw                      :  4; /* [ 7: 4],          r,        0x6 */
            uint32_t lna_load_csw                   :  4; /* [11: 8],        r/w,        0x6 */
            uint32_t lna_load_csw_hw                :  4; /* [15:12],          r,        0x6 */
            uint32_t lna_rfb_match                  :  3; /* [18:16],        r/w,        0x4 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t lna_cap_lg                     :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lna_lg_gsel                    :  3; /* [26:24],        r/w,        0x1 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t lna_cmatch_preind              :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lna;

    /* 0x7C : rmxgm */
    union {
        struct {
            uint32_t rmx_bm                         :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t rmxgm_bm                       :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rmxgm_10m_mode_en              :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t pu_rrf_avdd15                  :  1; /* [   12],        r/w,        0x1 */
            uint32_t pu_rrfldo                      :  1; /* [   13],        r/w,        0x0 */
            uint32_t rrfldo_bypass                  :  1; /* [   14],        r/w,        0x0 */
            uint32_t rrfldo_pulldown_en             :  1; /* [   15],        r/w,        0x0 */
            uint32_t rrfldo_vout_sel                :  3; /* [18:16],        r/w,        0x2 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rmxgm;

    /* 0x80 : rbb1 */
    union {
        struct {
            uint32_t rosdac_q                       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rosdac_i                       :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rosdac_q_hw                    :  6; /* [21:16],          r,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rosdac_i_hw                    :  6; /* [29:24],          r,       0x20 */
            uint32_t rbb_mfb_r2_bypass              :  1; /* [   30],        r/w,        0x0 */
            uint32_t rosdac_range                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb1;

    /* 0x84 : rbb2 */
    union {
        struct {
            uint32_t rbb_cap2_fc_q                  :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rbb_cap2_fc_i                  :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_q                  :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_i                  :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb2;

    /* 0x88 : rbb3 */
    union {
        struct {
            uint32_t rbb_mfb_deq                    :  4; /* [ 3: 0],        r/w,        0x7 */
            uint32_t rbb_startup_sel                :  1; /* [    4],        r/w,        0x0 */
            uint32_t rbb_bt_fif_tune                :  2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rbb_deq                        :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t rbb_bm_op                      :  3; /* [14:12],        r/w,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rbb_vcm                        :  2; /* [17:16],        r/w,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t rbb_bq_iqbias_short            :  1; /* [   20],        r/w,        0x1 */
            uint32_t rbb_tia_iqbias_short           :  1; /* [   21],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_bw                         :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t rxiqcal_en                     :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t pwr_det_en                     :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb3;

    /* 0x8C : rbb4 */
    union {
        struct {
            uint32_t rbb_pkdet_vth                  :  4; /* [ 3: 0],        r/w,        0x5 */
            uint32_t rbb_pkdet_out_rstn             :  1; /* [    4],        r/w,        0x0 */
            uint32_t rbb_pkdet_en                   :  1; /* [    5],        r/w,        0x0 */
            uint32_t rbb_mfb_c1_fc                  :  6; /* [11: 6],        r/w,       0x1a */
            uint32_t rbb_pkdet_out_rstn_hw          :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rbb_pkdet_en_hw                :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pkdet_out_raw                  :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t pkdet_out_latch                :  1; /* [   24],          r,        0x0 */
            uint32_t rbb_bt_mode_hw                 :  1; /* [   25],          r,        0x0 */
            uint32_t rbb_bt_mode                    :  1; /* [   26],        r/w,        0x0 */
            uint32_t rbb_cap1_deq_i                 :  1; /* [   27],        r/w,        0x0 */
            uint32_t rbb_cap1_deq_q                 :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb4;

    /* 0x90 : adda1 */
    union {
        struct {
            uint32_t dac_dvdd_sel                   :  3; /* [ 2: 0],        r/w,        0x2 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t dac_bias_sel                   :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t dac_clk_sel                    :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t dac_rccalsel                   :  1; /* [   12],        r/w,        0x0 */
            uint32_t dac_clk_sync_inv               :  1; /* [   13],        r/w,        0x1 */
            uint32_t dac_rccal_iqswap               :  1; /* [   14],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t adda_ldo_byps                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adda_ldo_dvdd_sel              :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t adda_ldo_dvdd_sel_hw           :  3; /* [26:24],          r,        0x4 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t dac_vlow_sel                   :  2; /* [29:28],        r/w,        0x1 */
            uint32_t dac_nsink_sel                  :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } adda1;

    /* 0x94 : adda2 */
    union {
        struct {
            uint32_t adc_vref_sel                   :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adc_dly_ctl                    :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t adc_dvdd_sel                   :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adc_sar_ascal_en               :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t adc_gt_rm                      :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adc_dclk_inv                   :  1; /* [   20],        r/w,        0x0 */
            uint32_t adc_dclk_sel                   :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adc_clk_inv                    :  1; /* [   24],        r/w,        0x0 */
            uint32_t adc_vref_comp_en               :  1; /* [   25],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t adc_clk_div_sel                :  1; /* [   28],        r/w,        0x1 */
            uint32_t adc_ti_en                      :  1; /* [   29],        r/w,        0x1 */
            uint32_t adc_vbuf_sel                   :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } adda2;

    /* 0x98 : adda3 */
    union {
        struct {
            uint32_t adc_dout_i                     : 11; /* [10: 0],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t adc_dout_q                     : 11; /* [26:16],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adda3;

    /* 0x9c  reserved */
    uint8_t RESERVED0x9c[4];

    /* 0xA0 : vco1 */
    union {
        struct {
            uint32_t lo_vco_freq_cw                 :  8; /* [ 7: 0],        r/w,       0x80 */
            uint32_t lo_vco_freq_cw_hw              :  8; /* [15: 8],          r,       0x80 */
            uint32_t lo_vco_idac_cw                 :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_idac_cw_hw              :  6; /* [29:24],          r,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } vco1;

    /* 0xA4 : vco2 */
    union {
        struct {
            uint32_t lo_vco_vbias_cw                :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t lo_vco_idac_boot               :  1; /* [    4],        r/w,        0x0 */
            uint32_t lo_vco_short_vbias_filter      :  1; /* [    5],        r/w,        0x0 */
            uint32_t lo_vco_short_idac_filter       :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t acal_vref_cw                   :  4; /* [11: 8],        r/w,        0xa */
            uint32_t acal_vco_ud                    :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t acal_inc_en_hw                 :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_28                 : 12; /* [28:17],       rsvd,        0x0 */
            uint32_t lo_vco_ldo_bypass              :  1; /* [   29],        r/w,        0x1 */
            uint32_t lo_vco_ldo_sel                 :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } vco2;

    /* 0xA8 : vco3 */
    union {
        struct {
            uint32_t fcal_div                       : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t fcal_cnt_op                    : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } vco3;

    /* 0xAC : vco4 */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t fcal_cnt_start                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t fcal_inc_en_hw                 :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t fcal_inc_large_range           :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t fcal_cnt_rdy                   :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t fcal_inc_vctrl_ud              :  2; /* [25:24],          r,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } vco4;

    /* 0xB0 : pfdcp */
    union {
        struct {
            uint32_t lo_cp_sel                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t lo_cp_sel_hw                   :  1; /* [    1],          r,        0x1 */
            uint32_t lo_cp_lp_mode_en               :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t lo_cp_offset_n_sel             :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t lo_cp_offset_p_sel             :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t lo_cp_startup_en               :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t lo_cp_ota_en                   :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t lo_cp_opamp_en                 :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t lo_cp_hiz                      :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_pfd_rvdd_boost              :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t lo_pfd_rst_csd                 :  1; /* [   28],        r/w,        0x0 */
            uint32_t lo_pfd_rst_csd_hw              :  1; /* [   29],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pfdcp;

    /* 0xB4 : lo */
    union {
        struct {
            uint32_t lo_lf_rz_hw                    :  3; /* [ 2: 0],          r,        0x2 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t lo_lf_r4_hw                    :  2; /* [ 5: 4],          r,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_lf_cz_hw                    :  2; /* [ 9: 8],          r,        0x2 */
            uint32_t reserved_10                    :  1; /* [   10],       rsvd,        0x0 */
            uint32_t lo_lf_rz                       :  3; /* [13:11],        r/w,        0x2 */
            uint32_t lo_lf_cz                       :  2; /* [15:14],        r/w,        0x2 */
            uint32_t lo_lf_r4                       :  2; /* [17:16],        r/w,        0x2 */
            uint32_t lo_lf_r4_short                 :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t lo_slipped_dn                  :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_slipped_up                  :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo;

    /* 0xB8 : fbdv */
    union {
        struct {
            uint32_t lo_fbdv_halfstep_en_hw         :  1; /* [    0],          r,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t lo_fbdv_halfstep_en            :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_fbdv_sel_sample_clk         :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lo_fbdv_sel_fb_clk             :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lo_fbdv_rst                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t lo_fbdv_rst_hw                 :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t pu_vbuf_fbdv                   :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t pu_vbuf_fbdv_hw                :  1; /* [   28],          r,        0x1 */
            uint32_t ppu_vbuf_fbdv_hw               :  1; /* [   29],          r,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } fbdv;

    /* 0xBC : lodist */
    union {
        struct {
            uint32_t lo_osmx_xgm_boost              :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t lo_osmx_en_xgm                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_osmx_fix_cap                :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t lo_osmx_vbuf_stre              :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t lo_osmx_capbank_bias           :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t lo_osmx_cap                    :  4; /* [23:20],        r/w,        0x8 */
            uint32_t lo_lodist_txbuf_stre           :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_26                 :  2; /* [26:25],       rsvd,        0x0 */
            uint32_t lo_lodist_rxbuf_stre           :  1; /* [   27],        r/w,        0x1 */
            uint32_t lo_lodist_dpd_en               :  1; /* [   28],        r/w,        0x0 */
            uint32_t pu_vbuf_lodist                 :  1; /* [   29],        r/w,        0x1 */
            uint32_t pu_vbuf_lodist_hw              :  1; /* [   30],          r,        0x1 */
            uint32_t ppu_vbuf_lodist_hw             :  1; /* [   31],          r,        0x1 */
        }BF;
        uint32_t WORD;
    } lodist;

    /* 0xC0 : sdm1 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_hw           :  2; /* [ 1: 0],          r,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t lo_sdm_bypass_hw               :  1; /* [    4],          r,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_sdm_dither_sel              :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lo_sdm_bypass                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t lo_sdm_rstb                    :  1; /* [   16],        r/w,        0x1 */
            uint32_t lo_sdm_rstb_hw                 :  1; /* [   17],          r,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t lo_sdm_flag                    :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sdm1;

    /* 0xC4 : sdm2 */
    union {
        struct {
            uint32_t lo_sdmin                       : 30; /* [29: 0],        r/w, 0x1501c71c */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sdm2;

    /* 0xC8 : sdm3 */
    union {
        struct {
            uint32_t lo_sdmin_hw                    : 30; /* [29: 0],          r,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sdm3;

    /* 0xCC : refbuf */
    union {
        struct {
            uint32_t reserved_0_20                  : 21; /* [20: 0],       rsvd,        0x0 */
            uint32_t lo_refbuf_loclk_en             :  1; /* [   21],        r/w,        0x1 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } refbuf;

    /* 0xD0 : glb_misc */
    union {
        struct {
            uint32_t wifipll_sdm_bypass_rx_bz       :  1; /* [    0],        r/w,        0x0 */
            uint32_t wifipll_sdm_bypass_tx_bz       :  1; /* [    1],        r/w,        0x0 */
            uint32_t wifipll_sdm_bypass_rx_wf       :  1; /* [    2],        r/w,        0x0 */
            uint32_t wifipll_sdm_bypass_tx_wf       :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } glb_misc;

    /* 0xD4 : RX normal bias mode registers */
    union {
        struct {
            uint32_t pa_imix_bias_sel               :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t pa_icx_scale                   :  4; /* [ 7: 4],        r/w,        0x9 */
            uint32_t pa_iptat_scale                 :  4; /* [11: 8],        r/w,        0x6 */
            uint32_t dac_imix_bias_sel              :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t dac_icx_scale                  :  4; /* [19:16],        r/w,        0x9 */
            uint32_t dac_iptat_scale                :  4; /* [23:20],        r/w,        0x6 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cip_1;

    /* 0xd8  reserved */
    uint8_t RESERVED0xd8[20];

    /* 0xEC : rf_resv_reg_0 */
    union {
        struct {
            uint32_t rf_reserved0                   : 32; /* [31: 0],        r/w, 0xffff0000 */
        }BF;
        uint32_t WORD;
    } rf_resv_reg_0;

    /* 0xF0 : rf_resv_reg_1 */
    union {
        struct {
            uint32_t rf_reserved1                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_resv_reg_1;

    /* 0xF4 : rf_resv_reg_2 */
    union {
        struct {
            uint32_t rf_reserved2                   : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_resv_reg_2;

    /* 0xf8  reserved */
    uint8_t RESERVED0xf8[8];

    /* 0x100 : rrf_gain_index1 */
    union {
        struct {
            uint32_t gain_ctrl0_gc_rmxgm_ratt       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t gain_ctrl0_gc_rmxgm            :  2; /* [ 4: 3],        r/w,        0x1 */
            uint32_t gain_ctrl0_gc_lna              :  3; /* [ 7: 5],        r/w,        0x0 */
            uint32_t gain_ctrl1_gc_rmxgm_ratt       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t gain_ctrl1_gc_rmxgm            :  2; /* [12:11],        r/w,        0x1 */
            uint32_t gain_ctrl1_gc_lna              :  3; /* [15:13],        r/w,        0x1 */
            uint32_t gain_ctrl2_gc_rmxgm_ratt       :  3; /* [18:16],        r/w,        0x0 */
            uint32_t gain_ctrl2_gc_rmxgm            :  2; /* [20:19],        r/w,        0x1 */
            uint32_t gain_ctrl2_gc_lna              :  3; /* [23:21],        r/w,        0x2 */
            uint32_t gain_ctrl3_gc_rmxgm_ratt       :  3; /* [26:24],        r/w,        0x0 */
            uint32_t gain_ctrl3_gc_rmxgm            :  2; /* [28:27],        r/w,        0x1 */
            uint32_t gain_ctrl3_gc_lna              :  3; /* [31:29],        r/w,        0x3 */
        }BF;
        uint32_t WORD;
    } rrf_gain_index1;

    /* 0x104 : rrf_gain_index2 */
    union {
        struct {
            uint32_t gain_ctrl4_gc_rmxgm_ratt       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t gain_ctrl4_gc_rmxgm            :  2; /* [ 4: 3],        r/w,        0x1 */
            uint32_t gain_ctrl4_gc_lna              :  3; /* [ 7: 5],        r/w,        0x4 */
            uint32_t gain_ctrl5_gc_rmxgm_ratt       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t gain_ctrl5_gc_rmxgm            :  2; /* [12:11],        r/w,        0x1 */
            uint32_t gain_ctrl5_gc_lna              :  3; /* [15:13],        r/w,        0x5 */
            uint32_t gain_ctrl6_gc_rmxgm_ratt       :  3; /* [18:16],        r/w,        0x0 */
            uint32_t gain_ctrl6_gc_rmxgm            :  2; /* [20:19],        r/w,        0x1 */
            uint32_t gain_ctrl6_gc_lna              :  3; /* [23:21],        r/w,        0x6 */
            uint32_t gain_ctrl7_gc_rmxgm_ratt       :  3; /* [26:24],        r/w,        0x0 */
            uint32_t gain_ctrl7_gc_rmxgm            :  2; /* [28:27],        r/w,        0x1 */
            uint32_t gain_ctrl7_gc_lna              :  3; /* [31:29],        r/w,        0x7 */
        }BF;
        uint32_t WORD;
    } rrf_gain_index2;

    /* 0x108 : rrf_gain_index3 */
    union {
        struct {
            uint32_t gain_ctrl8_gc_rmxgm_ratt       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t gain_ctrl8_gc_rmxgm            :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t gain_ctrl8_gc_lna              :  3; /* [ 7: 5],        r/w,        0x7 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rrf_gain_index3;

    /* 0x10C : lna_ctrl_hw_mux */
    union {
        struct {
            uint32_t lna_bm_hg                      :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t lna_bm_lg                      :  4; /* [ 7: 4],        r/w,        0x6 */
            uint32_t lna_load_csw_hg                :  4; /* [11: 8],        r/w,        0x6 */
            uint32_t lna_load_csw_lg                :  4; /* [15:12],        r/w,        0x6 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lna_ctrl_hw_mux;

    /* 0x110 : rbb_gain_index1 */
    union {
        struct {
            uint32_t gain_ctrl0_gc_rbb1             :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl0_gc_rbb2             :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t gain_ctrl1_gc_rbb1             :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gain_ctrl1_gc_rbb2             :  3; /* [14:12],        r/w,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gain_ctrl2_gc_rbb1             :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t gain_ctrl2_gc_rbb2             :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t gain_ctrl3_gc_rbb1             :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t gain_ctrl3_gc_rbb2             :  3; /* [30:28],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index1;

    /* 0x114 : rbb_gain_index2 */
    union {
        struct {
            uint32_t gain_ctrl4_gc_rbb1             :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl4_gc_rbb2             :  3; /* [ 6: 4],        r/w,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t gain_ctrl5_gc_rbb1             :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gain_ctrl5_gc_rbb2             :  3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gain_ctrl6_gc_rbb1             :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t gain_ctrl6_gc_rbb2             :  3; /* [22:20],        r/w,        0x3 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t gain_ctrl7_gc_rbb1             :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t gain_ctrl7_gc_rbb2             :  3; /* [30:28],        r/w,        0x4 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index2;

    /* 0x118 : rbb_gain_index3 */
    union {
        struct {
            uint32_t gain_ctrl8_gc_rbb1             :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl8_gc_rbb2             :  3; /* [ 6: 4],        r/w,        0x5 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t gain_ctrl9_gc_rbb1             :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gain_ctrl9_gc_rbb2             :  3; /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gain_ctrl10_gc_rbb1            :  2; /* [17:16],        r/w,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t gain_ctrl10_gc_rbb2            :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t gain_ctrl11_gc_rbb1            :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t gain_ctrl11_gc_rbb2            :  3; /* [30:28],        r/w,        0x5 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index3;

    /* 0x11C : rbb_gain_index4 */
    union {
        struct {
            uint32_t gain_ctrl12_gc_rbb1            :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl12_gc_rbb2            :  3; /* [ 6: 4],        r/w,        0x6 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t gain_ctrl13_gc_rbb1            :  2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gain_ctrl13_gc_rbb2            :  3; /* [14:12],        r/w,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gain_ctrl14_gc_rbb1            :  2; /* [17:16],        r/w,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t gain_ctrl14_gc_rbb2            :  3; /* [22:20],        r/w,        0x5 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t gain_ctrl15_gc_rbb1            :  2; /* [25:24],        r/w,        0x3 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t gain_ctrl15_gc_rbb2            :  3; /* [30:28],        r/w,        0x6 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index4;

    /* 0x120 : rbb_gain_index5 */
    union {
        struct {
            uint32_t gain_ctrl16_gc_rbb1            :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl16_gc_rbb2            :  3; /* [ 6: 4],        r/w,        0x7 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index5;

    /* 0x124  reserved */
    uint8_t RESERVED0x124[16];

    /* 0x134 : adda_reg_ctrl_hw */
    union {
        struct {
            uint32_t adda_ldo_dvdd_sel_rx           :  3; /* [ 2: 0],        r/w,        0x2 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t adda_ldo_dvdd_sel_tx           :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adda_reg_ctrl_hw;

    /* 0x138 : lo_reg_ctrl_hw1 */
    union {
        struct {
            uint32_t lo_fbdv_halfstep_en_rx         :  1; /* [    0],        r/w,        0x0 */
            uint32_t lo_fbdv_halfstep_en_tx         :  1; /* [    1],        r/w,        0x1 */
            uint32_t lo_cp_sel_rx                   :  1; /* [    2],        r/w,        0x1 */
            uint32_t lo_cp_sel_tx                   :  1; /* [    3],        r/w,        0x1 */
            uint32_t lo_lf_cz_rx                    :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_lf_cz_tx                    :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lo_lf_rz_rx                    :  3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t lo_lf_rz_tx                    :  3; /* [18:16],        r/w,        0x2 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t lo_lf_r4_rx                    :  2; /* [21:20],        r/w,        0x2 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_lf_r4_tx                    :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_reg_ctrl_hw1;

    /* 0x13C : lo_cal_ctrl_hw1 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2404            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2404            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2408            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2408            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw1;

    /* 0x140 : lo_cal_ctrl_hw2 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2412            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2412            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2416            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2416            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw2;

    /* 0x144 : lo_cal_ctrl_hw3 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2420            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2420            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2424            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2424            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw3;

    /* 0x148 : lo_cal_ctrl_hw4 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2428            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2428            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2432            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2432            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw4;

    /* 0x14C : lo_cal_ctrl_hw5 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2436            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2436            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2440            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2440            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw5;

    /* 0x150 : lo_cal_ctrl_hw6 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2444            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2444            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2448            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2448            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw6;

    /* 0x154 : lo_cal_ctrl_hw7 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2452            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2452            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2456            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2456            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw7;

    /* 0x158 : lo_cal_ctrl_hw8 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2460            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2460            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2464            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2464            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw8;

    /* 0x15C : lo_cal_ctrl_hw9 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2468            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2468            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2472            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2472            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw9;

    /* 0x160 : lo_cal_ctrl_hw10 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2476            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2476            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2480            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2480            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw10;

    /* 0x164 : lo_cal_ctrl_hw11 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2484            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2484            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw11;

    /* 0x168 : rosdac_ctrl_hw1 */
    union {
        struct {
            uint32_t rosdac_i_gc0                   :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rosdac_q_gc0                   :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rosdac_i_gc1                   :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rosdac_q_gc1                   :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rosdac_ctrl_hw1;

    /* 0x16C : rosdac_ctrl_hw2 */
    union {
        struct {
            uint32_t rosdac_i_gc2                   :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rosdac_q_gc2                   :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rosdac_i_gc3                   :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rosdac_q_gc3                   :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rosdac_ctrl_hw2;

    /* 0x170 : rxiq_ctrl_hw1 */
    union {
        struct {
            uint32_t rx_iq_phase_comp_gc0           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_iq_gain_comp_gc0            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxiq_ctrl_hw1;

    /* 0x174 : rxiq_ctrl_hw2 */
    union {
        struct {
            uint32_t rx_iq_phase_comp_gc1           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_iq_gain_comp_gc1            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxiq_ctrl_hw2;

    /* 0x178 : rxiq_ctrl_hw3 */
    union {
        struct {
            uint32_t rx_iq_phase_comp_gc2           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_iq_gain_comp_gc2            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxiq_ctrl_hw3;

    /* 0x17C : rxiq_ctrl_hw4 */
    union {
        struct {
            uint32_t rx_iq_phase_comp_gc3           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_iq_gain_comp_gc3            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxiq_ctrl_hw4;

    /* 0x180 : tosdac_ctrl_hw1 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tosdac_ctrl_hw1;

    /* 0x184 : tosdac_ctrl_hw2 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tosdac_ctrl_hw2;

    /* 0x188 : tosdac_ctrl_hw3 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tosdac_ctrl_hw3;

    /* 0x18C : tosdac_ctrl_hw4 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tosdac_ctrl_hw4;

    /* 0x190 : tx_iq_gain_hw0 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw0;

    /* 0x194 : tx_iq_gain_hw1 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw1;

    /* 0x198 : tx_iq_gain_hw2 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw2;

    /* 0x19C : tx_iq_gain_hw3 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw3;

    /* 0x1A0 : tx_iq_gain_hw4 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw4;

    /* 0x1A4 : tx_iq_gain_hw5 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw5;

    /* 0x1A8 : tx_iq_gain_hw6 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw6;

    /* 0x1AC : tx_iq_gain_hw7 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw7;

    /* 0x1B0 : lo_sdm_ctrl_hw1 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_wlan_2412    :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2417    :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2422    :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2427    :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2432    :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2437    :  2; /* [11:10],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2442    :  2; /* [13:12],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2447    :  2; /* [15:14],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2452    :  2; /* [17:16],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2457    :  2; /* [19:18],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2462    :  2; /* [21:20],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2467    :  2; /* [23:22],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2472    :  2; /* [25:24],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2484    :  2; /* [27:26],        r/w,        0x1 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw1;

    /* 0x1B4 : lo_sdm_ctrl_hw2 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_ble_2402     :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2404     :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2406     :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2408     :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2410     :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2412     :  2; /* [11:10],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2414     :  2; /* [13:12],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2416     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2418     :  2; /* [17:16],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2420     :  2; /* [19:18],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2422     :  2; /* [21:20],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2424     :  2; /* [23:22],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2426     :  2; /* [25:24],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2428     :  2; /* [27:26],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2430     :  2; /* [29:28],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2432     :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw2;

    /* 0x1B8 : lo_sdm_ctrl_hw3 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_ble_2434     :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2436     :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2438     :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2440     :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2442     :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2444     :  2; /* [11:10],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2446     :  2; /* [13:12],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2448     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2450     :  2; /* [17:16],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2452     :  2; /* [19:18],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2454     :  2; /* [21:20],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2456     :  2; /* [23:22],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2458     :  2; /* [25:24],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2460     :  2; /* [27:26],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2462     :  2; /* [29:28],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2464     :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw3;

    /* 0x1BC : lo_sdm_ctrl_hw4 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_ble_2466     :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2468     :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2470     :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2472     :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2474     :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2476     :  2; /* [11:10],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2478     :  2; /* [13:12],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2480     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_tx       :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw4;

    /* 0x1C0 : lo_sdm_ctrl_hw5 */
    union {
        struct {
            uint32_t lo_center_freq_mhz             : 12; /* [11: 0],        r/w,      0x97e */
            uint32_t lo_sdm_bypass_mode             :  6; /* [17:12],        r/w,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw5;

    /* 0x1C4 : lo_sdm_ctrl_hw6 */
    union {
        struct {
            uint32_t lo_sdmin_center                : 29; /* [28: 0],        r/w, 0x14400000 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw6;

    /* 0x1C8 : lo_sdm_ctrl_hw7 */
    union {
        struct {
            uint32_t lo_sdmin_1m                    : 20; /* [19: 0],        r/w,    0x22222 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw7;

    /* 0x1CC : lo_sdm_ctrl_hw8 */
    union {
        struct {
            uint32_t lo_sdmin_if                    : 20; /* [19: 0],        r/w,    0x2aaab */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw8;

    /* 0x1D0 : rbb_bw_ctrl_hw */
    union {
        struct {
            uint32_t rbb_bt_mode_ble                :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_bw_ctrl_hw;

    /* 0x1d4  reserved */
    uint8_t RESERVED0x1d4[52];

    /* 0x208 : rf_top_ctrl */
    union {
        struct {
            uint32_t rfckg_clk_phy_en               :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_top_ctrl;

    /* 0x20C : singen_ctrl0 */
    union {
        struct {
            uint32_t singen_inc_step1               : 10; /* [ 9: 0],        r/w,        0x1 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t singen_inc_step0               : 10; /* [25:16],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t singen_unsign_en               :  1; /* [   28],        r/w,        0x0 */
            uint32_t singen_clkdiv_n                :  2; /* [30:29],        r/w,        0x0 */
            uint32_t singen_en                      :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl0;

    /* 0x210 : singen_ctrl1 */
    union {
        struct {
            uint32_t singen_clkdiv_q                : 10; /* [ 9: 0],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t singen_mode_q                  :  4; /* [15:12],        r/w,        0x0 */
            uint32_t singen_clkdiv_i                : 10; /* [25:16],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t singen_mode_i                  :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl1;

    /* 0x214 : singen_ctrl2 */
    union {
        struct {
            uint32_t singen_gain_i                  : 11; /* [10: 0],        r/w,      0x3ff */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t singen_start_addr1_i           : 10; /* [21:12],        r/w,        0x0 */
            uint32_t singen_start_addr0_i           : 10; /* [31:22],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl2;

    /* 0x218 : singen_ctrl3 */
    union {
        struct {
            uint32_t singen_gain_q                  : 11; /* [10: 0],        r/w,      0x3ff */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t singen_start_addr1_q           : 10; /* [21:12],        r/w,        0x0 */
            uint32_t singen_start_addr0_q           : 10; /* [31:22],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl3;

    /* 0x21C : singen_ctrl4 */
    union {
        struct {
            uint32_t singen_fix_q                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t singen_fix_en_q                :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t singen_fix_i                   : 12; /* [27:16],        r/w,        0x0 */
            uint32_t singen_fix_en_i                :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl4;

    /* 0x220 : rfif_dfe_ctrl0 */
    union {
        struct {
            uint32_t rfckg_rxclk_4s_on              :  1; /* [    0],        r/w,        0x0 */
            uint32_t rfckg_txclk_4s_on              :  1; /* [    1],        r/w,        0x0 */
            uint32_t rfckg_adc_afifo_inv            :  1; /* [    2],        r/w,        0x0 */
            uint32_t rfckg_adc_clkout_sel           :  1; /* [    3],        r/w,        0x0 */
            uint32_t rfckg_dac_afifo_inv            :  1; /* [    4],        r/w,        0x1 */
            uint32_t rx_dfe_en_4s                   :  1; /* [    5],        r/w,        0x0 */
            uint32_t rx_dfe_en_4s_en                :  1; /* [    6],        r/w,        0x0 */
            uint32_t tx_dfe_en_4s                   :  1; /* [    7],        r/w,        0x0 */
            uint32_t tx_dfe_en_4s_en                :  1; /* [    8],        r/w,        0x0 */
            uint32_t rx_test_sel                    :  2; /* [10: 9],        r/w,        0x0 */
            uint32_t tx_test_sel                    :  2; /* [12:11],        r/w,        0x0 */
            uint32_t pad_adc_clkout_inv_en          :  1; /* [   13],        r/w,        0x1 */
            uint32_t pad_dac_clkout_inv_en          :  1; /* [   14],        r/w,        0x0 */
            uint32_t rf_ch_ind_ble_4s               :  7; /* [21:15],        r/w,        0x0 */
            uint32_t rf_ch_ind_ble_4s_en            :  1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t wifibw                         :  1; /* [   24],        r/w,        0x0 */
            uint32_t wifimode_4s_en                 :  1; /* [   25],        r/w,        0x0 */
            uint32_t bbmode_4s                      :  1; /* [   26],        r/w,        0x0 */
            uint32_t bbmode_4s_en                   :  1; /* [   27],        r/w,        0x0 */
            uint32_t test_sel                       :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rfif_dfe_ctrl0;

    /* 0x224 : rfif_test_read */
    union {
        struct {
            uint32_t test_read                      : 32; /* [31: 0],          r,     0x1111 */
        }BF;
        uint32_t WORD;
    } rfif_test_read;

    /* 0x228 : rfif_dig_ctrl */
    union {
        struct {
            uint32_t test_from_pad_en               :  1; /* [    0],        r/w,        0x0 */
            uint32_t test_gc_from_pad_en            :  1; /* [    1],        r/w,        0x0 */
            uint32_t rfckg_rxclk_div2_mode          :  1; /* [    2],        r/w,        0x0 */
            uint32_t rfif_int_lo_unlocked_mask      :  1; /* [    3],        r/w,        0x1 */
            uint32_t wifimode_4s                    : 11; /* [14: 4],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rfif_ppud_cnt2                 :  9; /* [24:16],        r/w,       0x9a */
            uint32_t rfif_ppud_cnt1                 :  5; /* [29:25],        r/w,        0xf */
            uint32_t rfif_ppud_manaual_en           :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rfif_dig_ctrl;

    /* 0x22C : rf_data_temp_0 */
    union {
        struct {
            uint32_t rf_data_temp_0                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_data_temp_0;

    /* 0x230 : rf_data_temp_1 */
    union {
        struct {
            uint32_t rf_data_temp_1                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_data_temp_1;

    /* 0x234 : rf_data_temp_2 */
    union {
        struct {
            uint32_t rf_data_temp_2                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_data_temp_2;

    /* 0x238 : rf_data_temp_3 */
    union {
        struct {
            uint32_t rf_data_temp_3                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_data_temp_3;

    /* 0x23C : rf_sram_ctrl0 */
    union {
        struct {
            uint32_t rf_sram_link_dly               : 16; /* [15: 0],        r/w,       0x20 */
            uint32_t rf_sram_link_mode              :  2; /* [17:16],        r/w,        0x0 */
            uint32_t rf_sram_swap                   :  1; /* [   18],        r/w,        0x0 */
            uint32_t rf_sram_ext_clr                :  1; /* [   19],        r/w,        0x0 */
            uint32_t rf_sram_ext_set                :  1; /* [   20],        r/w,        0x0 */
            uint32_t rf_sram_ext_src                :  2; /* [22:21],        r/w,        0x0 */
            uint32_t tx_test_2rx_sel                :  2; /* [24:23],        r/w,        0x0 */
            uint32_t tx_test_2rx_en                 :  1; /* [   25],        r/w,        0x0 */
            uint32_t reserved_26_30                 :  5; /* [30:26],       rsvd,        0x0 */
            uint32_t rf_sram_sel_dma                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl0;

    /* 0x240 : rf_sram_ctrl1 */
    union {
        struct {
            uint32_t rf_sram_adc_done               :  1; /* [    0],          r,        0x0 */
            uint32_t rf_sram_adc_en                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t rf_sram_adc_loop_en            :  1; /* [    2],        r/w,        0x0 */
            uint32_t rf_sram_adc_sts_clr            :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_15                  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t rf_sram_adc_done_cnt           : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl1;

    /* 0x244 : rf_sram_ctrl2 */
    union {
        struct {
            uint32_t rf_sram_adc_addr_end           : 16; /* [15: 0],        r/w,      0xfff */
            uint32_t rf_sram_adc_addr_start         : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl2;

    /* 0x248 : rf_sram_ctrl3 */
    union {
        struct {
            uint32_t rf_sram_adc_sts                : 32; /* [31: 0],          r, 0x50000000 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl3;

    /* 0x24C : rf_sram_ctrl4 */
    union {
        struct {
            uint32_t rf_sram_dac_done               :  1; /* [    0],          r,        0x0 */
            uint32_t rf_sram_dac_en                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t rf_sram_dac_loop_en            :  1; /* [    2],        r/w,        0x0 */
            uint32_t rf_sram_dac_sts_clr            :  1; /* [    3],        r/w,        0x0 */
            uint32_t rf_sram_dac_done_cnt_limit     : 12; /* [15: 4],        r/w,        0x0 */
            uint32_t rf_sram_dac_done_cnt           : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl4;

    /* 0x250 : rf_sram_ctrl5 */
    union {
        struct {
            uint32_t rf_sram_dac_addr_end           : 16; /* [15: 0],        r/w,      0xfff */
            uint32_t rf_sram_dac_addr_start         : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl5;

    /* 0x254 : rf_sram_ctrl6 */
    union {
        struct {
            uint32_t rf_sram_dac_sts                : 32; /* [31: 0],          r, 0x50000000 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl6;

    /* 0x258 : rf_ical_ctrl0 */
    union {
        struct {
            uint32_t rf_ical_r_cnt_n                : 10; /* [ 9: 0],        r/w,       0x50 */
            uint32_t rf_ical_a_cnt_n                : 10; /* [19:10],        r/w,       0xa0 */
            uint32_t rf_ical_f_cnt_n                : 10; /* [29:20],        r/w,       0x50 */
            uint32_t rf_ical_a_ud_inv_en            :  1; /* [   30],        r/w,        0x0 */
            uint32_t rf_ical_f_ud_inv_en            :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ical_ctrl0;

    /* 0x25C : rf_ical_ctrl1 */
    union {
        struct {
            uint32_t rf_ical_r_avg_n                :  5; /* [ 4: 0],        r/w,       0x1f */
            uint32_t reserved_5_9                   :  5; /* [ 9: 5],       rsvd,        0x0 */
            uint32_t rf_ical_r_os_q                 : 11; /* [20:10],          r,      0x400 */
            uint32_t rf_ical_r_os_i                 : 11; /* [31:21],          r,      0x400 */
        }BF;
        uint32_t WORD;
    } rf_ical_ctrl1;

    /* 0x260 : rf_ical_ctrl2 */
    union {
        struct {
            uint32_t rf_ical_period_n               : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_28                 : 13; /* [28:16],       rsvd,        0x0 */
            uint32_t rf_fsm_ic_blerx_dis            :  1; /* [   29],        r/w,        0x0 */
            uint32_t rf_fsm_ic_bletx_dis            :  1; /* [   30],        r/w,        0x0 */
            uint32_t rf_fsm_ic_wifi_dis             :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ical_ctrl2;

    /* 0x264 : rf_fsm_ctrl0 */
    union {
        struct {
            uint32_t rf_ch_ind_wifi                 : 12; /* [11: 0],        r/w,      0x99c */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl0;

    /* 0x268 : rf_fsm_ctrl1 */
    union {
        struct {
            uint32_t rf_fsm_lo_time                 : 16; /* [15: 0],        r/w,      0xc80 */
            uint32_t rf_fsm_lo_rdy                  :  1; /* [   16],          r,        0x0 */
            uint32_t rf_fsm_lo_rdy_rst              :  1; /* [   17],        r/w,        0x0 */
            uint32_t rf_fsm_lo_rdy_4s_1             :  1; /* [   18],        r/w,        0x0 */
            uint32_t rf_fsm_lo_rdy_sbclr            :  1; /* [   19],        r/w,        0x0 */
            uint32_t rf_fsm_pu_pa_dly_n             : 10; /* [29:20],        r/w,       0x50 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl1;

    /* 0x26C : rf_fsm_ctrl2 */
    union {
        struct {
            uint32_t rf_fsm_st_dbg                  :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t rf_fsm_st_dbg_en               :  1; /* [    3],        r/w,        0x0 */
            uint32_t rf_trx_en_ble_4s               :  1; /* [    4],        r/w,        0x0 */
            uint32_t rf_trx_sw_ble_4s               :  1; /* [    5],        r/w,        0x0 */
            uint32_t rf_trx_ble_4s_en               :  1; /* [    6],        r/w,        0x0 */
            uint32_t rf_m154_rx_start_4s            :  1; /* [    7],        r/w,        0x0 */
            uint32_t rf_m154_tx_start_4s            :  1; /* [    8],        r/w,        0x0 */
            uint32_t rf_trx_m154_4s_en              :  1; /* [    9],        r/w,        0x0 */
            uint32_t rf_fsm_dfe_tx_dly_n            : 10; /* [19:10],        r/w,       0xa0 */
            uint32_t rf_fsm_dfe_rx_dly_n            : 10; /* [29:20],        r/w,       0xa0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl2;

    /* 0x270 : rf_pkdet_ctrl0 */
    union {
        struct {
            uint32_t pkdet_out_cnt_sts              :  4; /* [ 3: 0],          r,        0x0 */
            uint32_t pkdet_out_cnt_en               :  1; /* [    4],        r/w,        0x0 */
            uint32_t pkdet_out_mode                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_pkdet_ctrl0;

    /* 0x274 : rf_fsm_ctrl3 */
    union {
        struct {
            uint32_t phy_rxmode_4s_val              :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t phy_rxmode_4s_en               :  1; /* [    3],        r/w,        0x0 */
            uint32_t phy_txmode_4s_val              :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t phy_txmode_4s_en               :  1; /* [    7],        r/w,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl3;

    /* 0x278 : rf_dma_ctrl0 */
    union {
        struct {
            uint32_t rf_dma_adc_done                :  1; /* [    0],          r,        0x0 */
            uint32_t rf_dma_adc_en                  :  1; /* [    1],        r/w,        0x0 */
            uint32_t rf_dma_adc_loop_en             :  1; /* [    2],        r/w,        0x0 */
            uint32_t rf_dma_adc_ostd_lmt            :  3; /* [ 5: 3],        r/w,        0x7 */
            uint32_t rf_dma_adc_wcmd_len            :  6; /* [11: 6],        r/w,       0x1f */
            uint32_t rf_dma_adc_axi_w_idle          :  1; /* [   12],          r,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rf_dma_adc_done_cnt            : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_dma_ctrl0;

    /* 0x27C : rf_dma_ctrl1 */
    union {
        struct {
            uint32_t rf_dma_adc_addr_start          : 32; /* [31: 0],        r/w, 0x50000100 */
        }BF;
        uint32_t WORD;
    } rf_dma_ctrl1;

    /* 0x280 : rf_dma_ctrl2 */
    union {
        struct {
            uint32_t rf_dma_adc_addr_cnt            : 32; /* [31: 0],        r/w,      0xf00 */
        }BF;
        uint32_t WORD;
    } rf_dma_ctrl2;

    /* 0x284 : rf_dma_ctrl3 */
    union {
        struct {
            uint32_t rf_dma_dac_done                :  1; /* [    0],          r,        0x0 */
            uint32_t rf_dma_dac_en                  :  1; /* [    1],        r/w,        0x0 */
            uint32_t rf_dma_dac_loop_en             :  1; /* [    2],        r/w,        0x0 */
            uint32_t rf_dma_dac_ostd_lmt            :  3; /* [ 5: 3],        r/w,        0x7 */
            uint32_t rf_dma_dac_rcmd_len            :  6; /* [11: 6],        r/w,       0x1f */
            uint32_t rf_dma_dac_axi_r_idle          :  1; /* [   12],          r,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rf_dma_dac_done_cnt            : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_dma_ctrl3;

    /* 0x288 : rf_dma_ctrl5 */
    union {
        struct {
            uint32_t rf_dma_dac_addr_start          : 32; /* [31: 0],        r/w, 0x50000100 */
        }BF;
        uint32_t WORD;
    } rf_dma_ctrl5;

    /* 0x28C : rf_dma_ctrl6 */
    union {
        struct {
            uint32_t rf_dma_dac_addr_cnt            : 32; /* [31: 0],        r/w,      0xf00 */
        }BF;
        uint32_t WORD;
    } rf_dma_ctrl6;

    /* 0x290 : rf_ILA_ctrl0 */
    union {
        struct {
            uint32_t cfg_ILA_en                     :  1; /* [    0],        r/w,        0x0 */
            uint32_t cfg_ILA_deci                   :  3; /* [ 3: 1],        r/w,        0x0 */
            uint32_t cfg_ILA_trig_ph_op             :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cfg_ILA_trig_src_sel1          :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t cfg_ILA_trig_src_sel2          :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t cfg_ILA_trig_src_sel3          :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t cfg_ILA_force_stop             :  1; /* [   20],        w1p,        0x0 */
            uint32_t cfg_ILA_data_src_to_sram       :  1; /* [   21],        r/w,        0x0 */
            uint32_t cfg_ILA_trig_op_sel1           :  2; /* [23:22],        r/w,        0x0 */
            uint32_t cfg_ILA_trig_op_sel2           :  2; /* [25:24],        r/w,        0x0 */
            uint32_t cfg_ILA_trig_op_sel3           :  2; /* [27:26],        r/w,        0x0 */
            uint32_t cfg_ILA_data_src_sel           :  3; /* [30:28],        r/w,        0x0 */
            uint32_t ILA_done                       :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl0;

    /* 0x294 : rf_ILA_ctrl1 */
    union {
        struct {
            uint32_t cfg_ILA_trig_mask_sel1         : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl1;

    /* 0x298 : rf_ILA_ctrl2 */
    union {
        struct {
            uint32_t cfg_ILA_trig_mask_sel2         : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl2;

    /* 0x29C : rf_ILA_ctrl3 */
    union {
        struct {
            uint32_t cfg_ILA_trig_mask_sel3         : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl3;

    /* 0x2A0 : rf_ILA_ctrl4 */
    union {
        struct {
            uint32_t cfg_ILA_trig_target_sel1       : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl4;

    /* 0x2A4 : rf_ILA_ctrl5 */
    union {
        struct {
            uint32_t cfg_ILA_trig_target_sel2       : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl5;

    /* 0x2A8 : rf_ILA_ctrl6 */
    union {
        struct {
            uint32_t cfg_ILA_trig_target_sel3       : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl6;

    /* 0x2AC : rf_ILA_ctrl7 */
    union {
        struct {
            uint32_t cfg_ILA_end_cnt                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl7;

    /* 0x2B0 : rf_ILA_ctrl8 */
    union {
        struct {
            uint32_t rf_sram_adc_addr_keep          : 16; /* [15: 0],          r,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl8;

    /* 0x2B4 : rf_ILA_ctrl9 */
    union {
        struct {
            uint32_t rf_dma_adc_addr_keep           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl9;

    /* 0x2B8 : rf_ILA_ctrl10 */
    union {
        struct {
            uint32_t cfg_ILA_dma_ctrl_en            :  1; /* [    0],        r/w,        0x0 */
            uint32_t cfg_ILA_reserved               : 31; /* [31: 1],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ILA_ctrl10;

    /* 0x2BC : rf_timer_ctrl */
    union {
        struct {
            uint32_t rf_timer_en                    :  1; /* [    0],        r/w,        0x0 */
            uint32_t rf_timer_read                  :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_timer_ctrl;

    /* 0x2C0 : rf_timer_value */
    union {
        struct {
            uint32_t rf_timer_keep                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_timer_value;

    /* 0x2C4 : fdiq_ctrl_0 */
    union {
        struct {
            uint32_t fdiq_rstb                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t fdiq_bypass                    :  1; /* [    1],        r/w,        0x1 */
            uint32_t fdiq_accen                     :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t fdiq_dly                       :  4; /* [ 7: 4],        r/w,        0x6 */
            uint32_t fdiq_accuOffset                : 10; /* [17: 8],        r/w,        0xa */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t fdiq_accuLength                : 10; /* [29:20],        r/w,       0xa0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } fdiq_ctrl_0;

    /* 0x2C8 : fdiq_ctrl_1 */
    union {
        struct {
            uint32_t fdiq_corr                      : 30; /* [29: 0],          r,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t fdiq_done                      :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } fdiq_ctrl_1;

    /* 0x2cc  reserved */
    uint8_t RESERVED0x2cc[820];

    /* 0x600 : dfe_ctrl_0 */
    union {
        struct {
            uint32_t tx_iqc_phase                   : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t tx_iqc_phase_en                :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t tx_iqc_gain                    : 11; /* [22:12],        r/w,      0x400 */
            uint32_t tx_iqc_gain_en                 :  1; /* [   23],        r/w,        0x1 */
            uint32_t tx2rx_loop_en                  :  1; /* [   24],        r/w,        0x0 */
            uint32_t tx2rx_loop_input_sel           :  1; /* [   25],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_0;

    /* 0x604 : dfe_ctrl_1 */
    union {
        struct {
            uint32_t tx_dac_os_i                    : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t tx_dly_tune_n                  :  3; /* [14:12],        r/w,        0x0 */
            uint32_t tx_rc3_csel                    :  1; /* [   15],        r/w,        0x0 */
            uint32_t tx_dac_os_q                    : 12; /* [27:16],        r/w,        0x0 */
            uint32_t tx_dac_dit_sel                 :  2; /* [29:28],        r/w,        0x0 */
            uint32_t tx_dac_dat_format              :  1; /* [   30],        r/w,        0x0 */
            uint32_t tx_dac_iq_swap                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_1;

    /* 0x608 : dfe_ctrl_2 */
    union {
        struct {
            uint32_t rx_adc_os_i                    : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t rx_adc_os_q                    : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t rx_adc_dce_flt_en              :  1; /* [   28],        r/w,        0x1 */
            uint32_t rx_adc_low_pow_en              :  1; /* [   29],        r/w,        0x0 */
            uint32_t rx_adc_dat_format              :  1; /* [   30],        r/w,        0x0 */
            uint32_t rx_adc_iq_swap                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_2;

    /* 0x60C : dfe_ctrl_3 */
    union {
        struct {
            uint32_t rx_adc_4s_i_val                : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t rx_adc_4s_i_en                 :  1; /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rx_adc_4s_q_val                : 11; /* [26:16],        r/w,        0x0 */
            uint32_t rx_adc_4s_q_en                 :  1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_3;

    /* 0x610 : dfe_ctrl_4 */
    union {
        struct {
            uint32_t rx_pf_th2                      : 11; /* [10: 0],        r/w,        0xf */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t rx_pf_th1                      : 11; /* [26:16],        r/w,       0x7f */
            uint32_t reserved_27_29                 :  3; /* [29:27],       rsvd,        0x0 */
            uint32_t rx_pf_q_en                     :  1; /* [   30],        r/w,        0x0 */
            uint32_t rx_pf_i_en                     :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_4;

    /* 0x614 : dfe_ctrl_5 */
    union {
        struct {
            uint32_t rx_iqc_phase                   : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t rx_iqc_phase_en                :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t rx_iqc_gain                    : 11; /* [22:12],        r/w,      0x400 */
            uint32_t rx_iqc_gain_en                 :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_5;

    /* 0x618 : dfe_ctrl_6 */
    union {
        struct {
            uint32_t rx_pm_freqshift_cw             : 20; /* [19: 0],        r/w,        0x0 */
            uint32_t rx_pm_freqshift_en             :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_27                 :  7; /* [27:21],       rsvd,        0x0 */
            uint32_t rx_pm_done                     :  1; /* [   28],          r,        0x0 */
            uint32_t rx_pm_en                       :  1; /* [   29],        r/w,        0x0 */
            uint32_t rx_pm_in_sel                   :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_6;

    /* 0x61C : dfe_ctrl_7 */
    union {
        struct {
            uint32_t rx_pm_start_ofs                : 16; /* [15: 0],        r/w,       0x50 */
            uint32_t rx_pm_acc_len                  : 16; /* [31:16],        r/w,      0x400 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_7;

    /* 0x620 : dfe_ctrl_8 */
    union {
        struct {
            uint32_t rx_pm_iqacc_i                  : 25; /* [24: 0],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_8;

    /* 0x624 : dfe_ctrl_9 */
    union {
        struct {
            uint32_t rx_pm_iqacc_q                  : 25; /* [24: 0],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_9;

    /* 0x628 : dfe_ctrl_10 */
    union {
        struct {
            uint32_t dfe_dac_raw_i                  : 11; /* [10: 0],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t dfe_dac_raw_q                  : 11; /* [26:16],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_10;

    /* 0x62C : dfe_ctrl_11 */
    union {
        struct {
            uint32_t dfe_adc_raw_i                  : 11; /* [10: 0],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t dfe_adc_raw_q                  : 11; /* [26:16],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_11;

    /* 0x630 : dfe_ctrl_12 */
    union {
        struct {
            uint32_t tx_cfr_th                      :  8; /* [ 7: 0],        r/w,       0x80 */
            uint32_t tx_cfr_mode                    :  1; /* [    8],        r/w,        0x0 */
            uint32_t tx_cfr_en                      :  1; /* [    9],        r/w,        0x0 */
            uint32_t tx_cfr_ctrl_hw                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_23                 : 13; /* [23:11],       rsvd,        0x0 */
            uint32_t tx_cfr_en_ofdm                 :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_12;

    /* 0x634 : dfe_ctrl_13 */
    union {
        struct {
            uint32_t tx_cfr_th_mcs3                 :  8; /* [ 7: 0],        r/w,       0xff */
            uint32_t tx_cfr_th_mcs2                 :  8; /* [15: 8],        r/w,       0xff */
            uint32_t tx_cfr_th_mcs1                 :  8; /* [23:16],        r/w,       0xff */
            uint32_t tx_cfr_th_mcs0                 :  8; /* [31:24],        r/w,       0xff */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_13;

    /* 0x638 : dfe_ctrl_14 */
    union {
        struct {
            uint32_t tx_cfr_th_mcs7                 :  8; /* [ 7: 0],        r/w,       0xff */
            uint32_t tx_cfr_th_mcs6                 :  8; /* [15: 8],        r/w,       0xff */
            uint32_t tx_cfr_th_mcs5                 :  8; /* [23:16],        r/w,       0xff */
            uint32_t tx_cfr_th_mcs4                 :  8; /* [31:24],        r/w,       0xff */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_14;

    /* 0x63C : dfe_ctrl_15 */
    union {
        struct {
            uint32_t tx_dvga_0_qdb                  :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t tx_dvga_0_ctrl_hw              :  1; /* [    7],        r/w,        0x1 */
            uint32_t tx_dvga_1_qdb                  :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t tx_dvga_1_ctrl_hw              :  1; /* [   15],        r/w,        0x1 */
            uint32_t tx_dvga_2_qdb                  :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_15;

    /* 0x640 : dfe_ctrl_16 */
    union {
        struct {
            uint32_t tx_dfe_dly                     :  9; /* [ 8: 0],        r/w,       0x6b */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_16;

    /* 0x644  reserved */
    uint8_t RESERVED0x644[60];

    /* 0x680 : dfe_rx_notch_0 */
    union {
        struct {
            uint32_t rx_notch2_fnrm                 : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t rx_notch2_alpha                :  4; /* [14:11],        r/w,        0x0 */
            uint32_t rx_notch2_en                   :  1; /* [   15],        r/w,        0x0 */
            uint32_t rx_notch1_fnrm                 : 11; /* [26:16],        r/w,        0x0 */
            uint32_t rx_notch1_alpha                :  4; /* [30:27],        r/w,        0x0 */
            uint32_t rx_notch1_en                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_0;

    /* 0x684 : dfe_rx_notch_1 */
    union {
        struct {
            uint32_t rx_notch_tar_ch_0              :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_1              :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_2              :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_3              :  7; /* [30:24],        r/w,        0x0 */
            uint32_t rx_notch_ctrl_hw               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_1;

    /* 0x688 : dfe_rx_notch_2 */
    union {
        struct {
            uint32_t rx_notch_tar_ch_4              :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_5              :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_6              :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_7              :  7; /* [30:24],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_2;

    /* 0x68C : dfe_rx_notch_3 */
    union {
        struct {
            uint32_t rx_notch_tar_ch_8              :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_3;

    /* 0x690 : dfe_rx_notch_4 */
    union {
        struct {
            uint32_t rx_notch_para_0                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_4;

    /* 0x694 : dfe_rx_notch_5 */
    union {
        struct {
            uint32_t rx_notch_para_1                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_5;

    /* 0x698 : dfe_rx_notch_6 */
    union {
        struct {
            uint32_t rx_notch_para_2                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_6;

    /* 0x69C : dfe_rx_notch_7 */
    union {
        struct {
            uint32_t rx_notch_para_3                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_7;

    /* 0x6A0 : dfe_rx_notch_8 */
    union {
        struct {
            uint32_t rx_notch_para_4                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_8;

    /* 0x6A4 : dfe_rx_notch_9 */
    union {
        struct {
            uint32_t rx_notch_para_5                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_9;

    /* 0x6A8 : dfe_rx_notch_10 */
    union {
        struct {
            uint32_t rx_notch_para_6                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_10;

    /* 0x6AC : dfe_rx_notch_11 */
    union {
        struct {
            uint32_t rx_notch_para_7                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_11;

    /* 0x6B0 : dfe_rx_notch_12 */
    union {
        struct {
            uint32_t rx_notch_para_8                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_12;

    /* 0x6B4 : dfe_rx_notch_13 */
    union {
        struct {
            uint32_t rx_notch2_fnrm_hw              : 11; /* [10: 0],          r,        0x0 */
            uint32_t rx_notch2_alpha_hw             :  4; /* [14:11],          r,        0x0 */
            uint32_t rx_notch2_en_hw                :  1; /* [   15],          r,        0x0 */
            uint32_t rx_notch1_fnrm_hw              : 11; /* [26:16],          r,        0x0 */
            uint32_t rx_notch1_alpha_hw             :  4; /* [30:27],          r,        0x0 */
            uint32_t rx_notch1_en_hw                :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_rx_notch_13;

    /* 0x6b8  reserved */
    uint8_t RESERVED0x6b8[72];

    /* 0x700 : pwr_wifi_ctrl_0 */
    union {
        struct {
            uint32_t wifi_nominal_pwr_0             :  6; /* [ 5: 0],        r/w,       0x18 */
            uint32_t wifi_nominal_pwr_1             :  6; /* [11: 6],        r/w,       0x16 */
            uint32_t wifi_nominal_pwr_2             :  6; /* [17:12],        r/w,       0x14 */
            uint32_t wifi_nominal_pwr_3             :  6; /* [23:18],        r/w,       0x10 */
            uint32_t m154_tx_pwr                    :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_ctrl_0;

    /* 0x704 : pwr_wifi_ctrl_1 */
    union {
        struct {
            uint32_t wifi_nominal_pwr_4             :  6; /* [ 5: 0],        r/w,        0xc */
            uint32_t wifi_nominal_pwr_5             :  6; /* [11: 6],        r/w,        0x8 */
            uint32_t wifi_nominal_pwr_6             :  6; /* [17:12],        r/w,        0x4 */
            uint32_t wifi_nominal_pwr_7             :  6; /* [23:18],        r/w,        0x0 */
            uint32_t wifi_tx_corr                   :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_ctrl_1;

    /* 0x708 : pwr_wifi_ctrl_2 */
    union {
        struct {
            uint32_t wifi_nominal_pwr_8             :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t wifi_nominal_pwr_9             :  6; /* [11: 6],        r/w,        0x0 */
            uint32_t wifi_nominal_pwr_10            :  6; /* [17:12],        r/w,        0x0 */
            uint32_t wifi_nominal_pwr_11            :  6; /* [23:18],        r/w,        0x0 */
            uint32_t wifi_nominal_pwr_12            :  6; /* [29:24],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_ctrl_2;

    /* 0x70C : pwr_wifi_tbl_0 */
    union {
        struct {
            uint32_t wifi_iqc_gain_0                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_0                 :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_0                  :  3; /* [20:18],        r/w,        0x7 */
            uint32_t wifi_gc_tbb_0                  :  5; /* [25:21],        r/w,       0x19 */
            uint32_t wifi_gc_tbb_boost_0            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_0                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_0              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_0;

    /* 0x710 : pwr_wifi_tbl_1 */
    union {
        struct {
            uint32_t wifi_dpd_sel_0                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_0                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_0                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_0               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_0                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_0                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_0                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_0                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_0                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_0               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_1;

    /* 0x714 : pwr_wifi_tbl_2 */
    union {
        struct {
            uint32_t wifi_iqc_gain_1                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_1                 :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_1                  :  3; /* [20:18],        r/w,        0x7 */
            uint32_t wifi_gc_tbb_1                  :  5; /* [25:21],        r/w,       0x14 */
            uint32_t wifi_gc_tbb_boost_1            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_1                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_1              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_2;

    /* 0x718 : pwr_wifi_tbl_3 */
    union {
        struct {
            uint32_t wifi_dpd_sel_1                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_1                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_1                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_1               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_1                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_1                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_1                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_1                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_1                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_1               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_3;

    /* 0x71C : pwr_wifi_tbl_4 */
    union {
        struct {
            uint32_t wifi_iqc_gain_2                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_2                 :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_2                  :  3; /* [20:18],        r/w,        0x7 */
            uint32_t wifi_gc_tbb_2                  :  5; /* [25:21],        r/w,        0xe */
            uint32_t wifi_gc_tbb_boost_2            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_2                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_2              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_4;

    /* 0x720 : pwr_wifi_tbl_5 */
    union {
        struct {
            uint32_t wifi_dpd_sel_2                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_2                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_2                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_2               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_2                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_2                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_2                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_2                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_2                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_2               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_5;

    /* 0x724 : pwr_wifi_tbl_6 */
    union {
        struct {
            uint32_t wifi_iqc_gain_3                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_3                 :  7; /* [17:11],        r/w,       0x7c */
            uint32_t wifi_gc_tmx_3                  :  3; /* [20:18],        r/w,        0x7 */
            uint32_t wifi_gc_tbb_3                  :  5; /* [25:21],        r/w,        0x8 */
            uint32_t wifi_gc_tbb_boost_3            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_3                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_3              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_6;

    /* 0x728 : pwr_wifi_tbl_7 */
    union {
        struct {
            uint32_t wifi_dpd_sel_3                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_3                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_3                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_3               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_3                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_3                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_3                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_3                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_3                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_3               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_7;

    /* 0x72C : pwr_wifi_tbl_8 */
    union {
        struct {
            uint32_t wifi_iqc_gain_4                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_4                 :  7; /* [17:11],        r/w,        0x4 */
            uint32_t wifi_gc_tmx_4                  :  3; /* [20:18],        r/w,        0x6 */
            uint32_t wifi_gc_tbb_4                  :  5; /* [25:21],        r/w,        0x5 */
            uint32_t wifi_gc_tbb_boost_4            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_4                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_4              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_8;

    /* 0x730 : pwr_wifi_tbl_9 */
    union {
        struct {
            uint32_t wifi_dpd_sel_4                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_4                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_4                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_4               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_4                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_4                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_4                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_4                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_4                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_4               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_9;

    /* 0x734 : pwr_wifi_tbl_10 */
    union {
        struct {
            uint32_t wifi_iqc_gain_5                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_5                 :  7; /* [17:11],        r/w,       0x70 */
            uint32_t wifi_gc_tmx_5                  :  3; /* [20:18],        r/w,        0x5 */
            uint32_t wifi_gc_tbb_5                  :  5; /* [25:21],        r/w,        0x4 */
            uint32_t wifi_gc_tbb_boost_5            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_5                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_5              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_10;

    /* 0x738 : pwr_wifi_tbl_11 */
    union {
        struct {
            uint32_t wifi_dpd_sel_5                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_5                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_5                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_5               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_5                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_5                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_5                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_5                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_5                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_5               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_11;

    /* 0x73C : pwr_wifi_tbl_12 */
    union {
        struct {
            uint32_t wifi_iqc_gain_6                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_6                 :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_6                  :  3; /* [20:18],        r/w,        0x3 */
            uint32_t wifi_gc_tbb_6                  :  5; /* [25:21],        r/w,        0x4 */
            uint32_t wifi_gc_tbb_boost_6            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_6                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_6              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_12;

    /* 0x740 : pwr_wifi_tbl_13 */
    union {
        struct {
            uint32_t wifi_dpd_sel_6                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_6                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_6                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_6               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_6                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_6                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_6                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_6                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_6                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_6               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_13;

    /* 0x744 : pwr_wifi_tbl_14 */
    union {
        struct {
            uint32_t wifi_iqc_gain_7                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_7                 :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_7                  :  3; /* [20:18],        r/w,        0x2 */
            uint32_t wifi_gc_tbb_7                  :  5; /* [25:21],        r/w,        0x4 */
            uint32_t wifi_gc_tbb_boost_7            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_7                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_7              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_14;

    /* 0x748 : pwr_wifi_tbl_15 */
    union {
        struct {
            uint32_t wifi_dpd_sel_7                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_7                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_7                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_7               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_7                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_7                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_7                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_7                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_7                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_7               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_15;

    /* 0x74C : pwr_wifi_tbl_16 */
    union {
        struct {
            uint32_t wifi_iqc_gain_8                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_8                 :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_8                  :  3; /* [20:18],        r/w,        0x2 */
            uint32_t wifi_gc_tbb_8                  :  5; /* [25:21],        r/w,        0x4 */
            uint32_t wifi_gc_tbb_boost_8            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_8                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_8              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_16;

    /* 0x750 : pwr_wifi_tbl_17 */
    union {
        struct {
            uint32_t wifi_dpd_sel_8                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_8                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_8                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_8               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_8                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_8                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_8                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_8                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_8                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_8               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_17;

    /* 0x754 : pwr_wifi_tbl_18 */
    union {
        struct {
            uint32_t wifi_iqc_gain_9                : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_9                 :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_9                  :  3; /* [20:18],        r/w,        0x2 */
            uint32_t wifi_gc_tbb_9                  :  5; /* [25:21],        r/w,        0x4 */
            uint32_t wifi_gc_tbb_boost_9            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_9                :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_9              :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_18;

    /* 0x758 : pwr_wifi_tbl_19 */
    union {
        struct {
            uint32_t wifi_dpd_sel_9                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_9                :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_9                :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_9               : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_9                   :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_9                :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_9                :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_9                :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_9                :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_9               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_19;

    /* 0x75C : pwr_wifi_tbl_20 */
    union {
        struct {
            uint32_t wifi_iqc_gain_10               : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_10                :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_10                 :  3; /* [20:18],        r/w,        0x2 */
            uint32_t wifi_gc_tbb_10                 :  5; /* [25:21],        r/w,        0x4 */
            uint32_t wifi_gc_tbb_boost_10           :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_10               :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_10             :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_20;

    /* 0x760 : pwr_wifi_tbl_21 */
    union {
        struct {
            uint32_t wifi_dpd_sel_10                :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_10               :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_10               :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_10              : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_10                  :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_10               :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_10               :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_10               :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_10               :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_10              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_21;

    /* 0x764 : pwr_wifi_tbl_22 */
    union {
        struct {
            uint32_t wifi_iqc_gain_11               : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_11                :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_11                 :  3; /* [20:18],        r/w,        0x2 */
            uint32_t wifi_gc_tbb_11                 :  5; /* [25:21],        r/w,        0x4 */
            uint32_t wifi_gc_tbb_boost_11           :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_11               :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_11             :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_22;

    /* 0x768 : pwr_wifi_tbl_23 */
    union {
        struct {
            uint32_t wifi_dpd_sel_11                :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_11               :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_11               :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_11              : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_11                  :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_11               :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_11               :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_11               :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_11               :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_11              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_23;

    /* 0x76C : pwr_wifi_tbl_24 */
    union {
        struct {
            uint32_t wifi_iqc_gain_12               : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t wifi_dg1_qdb_12                :  7; /* [17:11],        r/w,        0x0 */
            uint32_t wifi_gc_tmx_12                 :  3; /* [20:18],        r/w,        0x2 */
            uint32_t wifi_gc_tbb_12                 :  5; /* [25:21],        r/w,        0x4 */
            uint32_t wifi_gc_tbb_boost_12           :  2; /* [27:26],        r/w,        0x0 */
            uint32_t wifi_dac_bias_12               :  2; /* [29:28],        r/w,        0x1 */
            uint32_t wifi_ax_submode_12             :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_24;

    /* 0x770 : pwr_wifi_tbl_25 */
    union {
        struct {
            uint32_t wifi_dpd_sel_12                :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifi_tosdac_q_12               :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t wifi_tosdac_i_12               :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t wifi_iqc_phase_12              : 10; /* [23:14],        r/w,        0x0 */
            uint32_t wifi_gc_pa_12                  :  3; /* [26:24],        r/w,        0x0 */
            uint32_t wifi_pa_lp_en_12               :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifi_dpd_en_b_12               :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifi_dpd_en_g_12               :  1; /* [   29],        r/w,        0x0 */
            uint32_t wifi_dpd_en_n_12               :  1; /* [   30],        r/w,        0x0 */
            uint32_t wifi_dpd_en_ax_12              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_wifi_tbl_25;

    /* 0x774 : pwr_bz_ctrl_0 */
    union {
        struct {
            uint32_t bz_tx_backoff_00               :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t bz_tx_backoff_01               :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t bz_tx_backoff_02               :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t bz_tx_backoff_03               :  4; /* [15:12],        r/w,        0x0 */
            uint32_t bz_tx_backoff_04               :  4; /* [19:16],        r/w,        0x0 */
            uint32_t bz_tx_backoff_05               :  4; /* [23:20],        r/w,        0x0 */
            uint32_t bz_tx_backoff_06               :  4; /* [27:24],        r/w,        0x0 */
            uint32_t bz_tx_backoff_07               :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_ctrl_0;

    /* 0x778 : pwr_bz_ctrl_1 */
    union {
        struct {
            uint32_t bz_tx_backoff_08               :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t bz_tx_backoff_09               :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t bz_tx_backoff_10               :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t bz_tx_backoff_11               :  4; /* [15:12],        r/w,        0x0 */
            uint32_t bz_tx_backoff_12               :  4; /* [19:16],        r/w,        0x0 */
            uint32_t bz_tx_backoff_13               :  4; /* [23:20],        r/w,        0x0 */
            uint32_t bz_tx_backoff_14               :  4; /* [27:24],        r/w,        0x0 */
            uint32_t bz_tx_backoff_15               :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_ctrl_1;

    /* 0x77C : pwr_bz_ctrl_2 */
    union {
        struct {
            uint32_t bz_tx_backoff_16               :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t bz_tx_backoff_17               :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t bz_tx_backoff_18               :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t bz_tx_backoff_19               :  4; /* [15:12],        r/w,        0x0 */
            uint32_t bz_tx_backoff_20               :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_ctrl_2;

    /* 0x780 : pwr_bz_ctrl_3 */
    union {
        struct {
            uint32_t bz_tx_corr_0                   :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t bz_tx_corr_1                   :  6; /* [11: 6],        r/w,        0x0 */
            uint32_t bz_tx_corr_2                   :  6; /* [17:12],        r/w,        0x0 */
            uint32_t bz_tx_corr_3                   :  6; /* [23:18],        r/w,        0x0 */
            uint32_t bz_tx_corr_4                   :  6; /* [29:24],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_ctrl_3;

    /* 0x784 : pwr_bz_ctrl_4 */
    union {
        struct {
            uint32_t bz_tx_temp_0                   :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t bz_tx_temp_1                   :  6; /* [11: 6],        r/w,        0x0 */
            uint32_t bz_tx_temp_2                   :  6; /* [17:12],        r/w,        0x0 */
            uint32_t bz_tx_temp_3                   :  6; /* [23:18],        r/w,        0x0 */
            uint32_t bz_tx_temp_4                   :  6; /* [29:24],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_ctrl_4;

    /* 0x788 : pwr_bz_ctrl_5 */
    union {
        struct {
            uint32_t bz_nominal_pwr_0               :  6; /* [ 5: 0],        r/w,        0xf */
            uint32_t bz_nominal_pwr_1               :  6; /* [11: 6],        r/w,        0xc */
            uint32_t bz_nominal_pwr_2               :  6; /* [17:12],        r/w,        0xa */
            uint32_t bz_nominal_pwr_3               :  6; /* [23:18],        r/w,        0x7 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_ctrl_5;

    /* 0x78C : pwr_bz_tbl_0 */
    union {
        struct {
            uint32_t bz_iqc_gain_0                  : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t bz_dg1_qdb_0                   :  7; /* [17:11],        r/w,        0x0 */
            uint32_t bz_gc_tmx_0                    :  3; /* [20:18],        r/w,        0x7 */
            uint32_t bz_gc_tbb_0                    :  5; /* [25:21],        r/w,        0x6 */
            uint32_t bz_gc_tbb_boost_0              :  2; /* [27:26],        r/w,        0x0 */
            uint32_t bz_dac_bias_0                  :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_tbl_0;

    /* 0x790 : pwr_bz_tbl_1 */
    union {
        struct {
            uint32_t bz_dpd_sel_0                   :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t bz_tosdac_q_0                  :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t bz_tosdac_i_0                  :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t bz_iqc_phase_0                 : 10; /* [23:14],        r/w,        0x0 */
            uint32_t bz_gc_pa_0                     :  3; /* [26:24],        r/w,        0x0 */
            uint32_t bz_pa_lp_en_0                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t bz_dpd_en_0                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_tbl_1;

    /* 0x794 : pwr_bz_tbl_2 */
    union {
        struct {
            uint32_t bz_iqc_gain_1                  : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t bz_dg1_qdb_1                   :  7; /* [17:11],        r/w,        0x0 */
            uint32_t bz_gc_tmx_1                    :  3; /* [20:18],        r/w,        0x7 */
            uint32_t bz_gc_tbb_1                    :  5; /* [25:21],        r/w,        0x4 */
            uint32_t bz_gc_tbb_boost_1              :  2; /* [27:26],        r/w,        0x0 */
            uint32_t bz_dac_bias_1                  :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_tbl_2;

    /* 0x798 : pwr_bz_tbl_3 */
    union {
        struct {
            uint32_t bz_dpd_sel_1                   :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t bz_tosdac_q_1                  :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t bz_tosdac_i_1                  :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t bz_iqc_phase_1                 : 10; /* [23:14],        r/w,        0x0 */
            uint32_t bz_gc_pa_1                     :  3; /* [26:24],        r/w,        0x0 */
            uint32_t bz_pa_lp_en_1                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t bz_dpd_en_1                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_tbl_3;

    /* 0x79C : pwr_bz_tbl_4 */
    union {
        struct {
            uint32_t bz_iqc_gain_2                  : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t bz_dg1_qdb_2                   :  7; /* [17:11],        r/w,        0x0 */
            uint32_t bz_gc_tmx_2                    :  3; /* [20:18],        r/w,        0x7 */
            uint32_t bz_gc_tbb_2                    :  5; /* [25:21],        r/w,        0x3 */
            uint32_t bz_gc_tbb_boost_2              :  2; /* [27:26],        r/w,        0x0 */
            uint32_t bz_dac_bias_2                  :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_tbl_4;

    /* 0x7A0 : pwr_bz_tbl_6 */
    union {
        struct {
            uint32_t bz_dpd_sel_2                   :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t bz_tosdac_q_2                  :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t bz_tosdac_i_2                  :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t bz_iqc_phase_2                 : 10; /* [23:14],        r/w,        0x0 */
            uint32_t bz_gc_pa_2                     :  3; /* [26:24],        r/w,        0x0 */
            uint32_t bz_pa_lp_en_2                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t bz_dpd_en_2                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_tbl_6;

    /* 0x7A4 : pwr_bz_tbl_7 */
    union {
        struct {
            uint32_t bz_iqc_gain_3                  : 11; /* [10: 0],        r/w,      0x400 */
            uint32_t bz_dg1_qdb_3                   :  7; /* [17:11],        r/w,        0x0 */
            uint32_t bz_gc_tmx_3                    :  3; /* [20:18],        r/w,        0x7 */
            uint32_t bz_gc_tbb_3                    :  5; /* [25:21],        r/w,        0x2 */
            uint32_t bz_gc_tbb_boost_3              :  2; /* [27:26],        r/w,        0x0 */
            uint32_t bz_dac_bias_3                  :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_tbl_7;

    /* 0x7A8 : pwr_bz_tbl_8 */
    union {
        struct {
            uint32_t bz_dpd_sel_3                   :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t bz_tosdac_q_3                  :  6; /* [ 7: 2],        r/w,       0x20 */
            uint32_t bz_tosdac_i_3                  :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t bz_iqc_phase_3                 : 10; /* [23:14],        r/w,        0x0 */
            uint32_t bz_gc_pa_3                     :  3; /* [26:24],        r/w,        0x0 */
            uint32_t bz_pa_lp_en_3                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t bz_dpd_en_3                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_bz_tbl_8;

    /* 0x7AC : pwr_ctrl_0 */
    union {
        struct {
            uint32_t wifi_tx_corr_dsss              :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t rf_tx_pwr_sts                  : 20; /* [27: 8],          r,    0x70000 */
            uint32_t reserved_28_30                 :  3; /* [30:28],       rsvd,        0x0 */
            uint32_t rf_tx_reduce_pwr_ena           :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_ctrl_0;

    /* 0x7B0 : pwr_ctrl_1 */
    union {
        struct {
            uint32_t rf_tx_reduce_pwr_wifi          :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t rf_tx_reduce_pwr_bt            :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t rf_tx_reduce_pwr_zb            :  8; /* [23:16],        r/w,        0x3 */
            uint32_t rf_tx_reduce_pwr_ble           :  8; /* [31:24],        r/w,        0x4 */
        }BF;
        uint32_t WORD;
    } pwr_ctrl_1;

    /* 0x7B4 : pwr_ctrl_3 */
    union {
        struct {
            uint32_t bt_tx_corr_0                   :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t bt_tx_corr_1                   :  6; /* [11: 6],        r/w,        0x0 */
            uint32_t bt_tx_corr_2                   :  6; /* [17:12],        r/w,        0x0 */
            uint32_t bt_tx_corr_3                   :  6; /* [23:18],        r/w,        0x0 */
            uint32_t bt_tx_corr_4                   :  6; /* [29:24],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_ctrl_3;

    /* 0x7B8 : pwr_ctrl_4 */
    union {
        struct {
            uint32_t bt_tx_temp_0                   :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t bt_tx_temp_1                   :  6; /* [11: 6],        r/w,        0x0 */
            uint32_t bt_tx_temp_2                   :  6; /* [17:12],        r/w,        0x0 */
            uint32_t bt_tx_temp_3                   :  6; /* [23:18],        r/w,        0x0 */
            uint32_t bt_tx_temp_4                   :  6; /* [29:24],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwr_ctrl_4;

    /* 0x7BC : pa_config_b */
    union {
        struct {
            uint32_t wifi_pa_vbcas_b                :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t wifi_pa_iaq_b                  :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t wifi_pa_iet_b                  :  4; /* [ 9: 6],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_iet_b           :  2; /* [11:10],        r/w,        0x0 */
            uint32_t wifi_pa_vbpmos_b               :  4; /* [15:12],        r/w,        0x1 */
            uint32_t wifi_pa_vbcore_b               :  4; /* [19:16],        r/w,        0x6 */
            uint32_t wifi_pa_etb_en_b               :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifi_pa_ib_fix_b               :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_config_b;

    /* 0x7C0 : pa_config_g */
    union {
        struct {
            uint32_t wifi_pa_vbcas_g                :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t wifi_pa_iaq_g                  :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t wifi_pa_iet_g                  :  4; /* [ 9: 6],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_iet_g           :  2; /* [11:10],        r/w,        0x0 */
            uint32_t wifi_pa_vbpmos_g               :  4; /* [15:12],        r/w,        0x1 */
            uint32_t wifi_pa_vbcore_g               :  4; /* [19:16],        r/w,        0x6 */
            uint32_t wifi_pa_etb_en_g               :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifi_pa_ib_fix_g               :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_config_g;

    /* 0x7C4 : pa_config_n */
    union {
        struct {
            uint32_t wifi_pa_vbcas_n                :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t wifi_pa_iaq_n                  :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t wifi_pa_iet_n                  :  4; /* [ 9: 6],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_iet_n           :  2; /* [11:10],        r/w,        0x0 */
            uint32_t wifi_pa_vbpmos_n               :  4; /* [15:12],        r/w,        0x1 */
            uint32_t wifi_pa_vbcore_n               :  4; /* [19:16],        r/w,        0x6 */
            uint32_t wifi_pa_etb_en_n               :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifi_pa_ib_fix_n               :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_config_n;

    /* 0x7C8 : pa_config_ax_sub_mode0 */
    union {
        struct {
            uint32_t wifi_pa_vbcas_ax0              :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t wifi_pa_iaq_ax0                :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t wifi_pa_iet_ax0                :  4; /* [ 9: 6],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_iet_ax0         :  2; /* [11:10],        r/w,        0x0 */
            uint32_t wifi_pa_vbpmos_ax0             :  4; /* [15:12],        r/w,        0x1 */
            uint32_t wifi_pa_vbcore_ax0             :  4; /* [19:16],        r/w,        0x6 */
            uint32_t wifi_pa_etb_en_ax0             :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifi_pa_ib_fix_ax0             :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_config_ax_sub_mode0;

    /* 0x7CC : pa_config_ax_sub_mode1 */
    union {
        struct {
            uint32_t wifi_pa_vbcas_ax1              :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t wifi_pa_iaq_ax1                :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t wifi_pa_iet_ax1                :  4; /* [ 9: 6],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_iet_ax1         :  2; /* [11:10],        r/w,        0x2 */
            uint32_t wifi_pa_vbpmos_ax1             :  4; /* [15:12],        r/w,        0x0 */
            uint32_t wifi_pa_vbcore_ax1             :  4; /* [19:16],        r/w,        0xa */
            uint32_t wifi_pa_etb_en_ax1             :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifi_pa_ib_fix_ax1             :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_config_ax_sub_mode1;

    /* 0x7D0 : pa_config_ax_sub_mode2 */
    union {
        struct {
            uint32_t wifi_pa_vbcas_ax2              :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t wifi_pa_iaq_ax2                :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t wifi_pa_iet_ax2                :  4; /* [ 9: 6],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_iet_ax2         :  2; /* [11:10],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_ax2             :  4; /* [15:12],        r/w,        0x0 */
            uint32_t wifi_pa_vbcore_ax2             :  4; /* [19:16],        r/w,        0xa */
            uint32_t wifi_pa_etb_en_ax2             :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifi_pa_ib_fix_ax2             :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_config_ax_sub_mode2;

    /* 0x7D4 : pa_config_ax_sub_mode3 */
    union {
        struct {
            uint32_t wifi_pa_vbcas_ax3              :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t wifi_pa_iaq_ax3                :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t wifi_pa_iet_ax3                :  4; /* [ 9: 6],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_iet_ax3         :  2; /* [11:10],        r/w,        0x3 */
            uint32_t wifi_pa_vbpmos_ax3             :  4; /* [15:12],        r/w,        0x0 */
            uint32_t wifi_pa_vbcore_ax3             :  4; /* [19:16],        r/w,        0x6 */
            uint32_t wifi_pa_etb_en_ax3             :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifi_pa_ib_fix_ax3             :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_config_ax_sub_mode3;

    /* 0x7D8 : pa_config_bz */
    union {
        struct {
            uint32_t bz_pa_vbcas                    :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t bz_pa_iaq                      :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t bz_pa_iet                      :  4; /* [ 9: 6],        r/w,        0x3 */
            uint32_t bz_pa_vbpmos_iet               :  2; /* [11:10],        r/w,        0x0 */
            uint32_t bz_pa_vbpmos                   :  4; /* [15:12],        r/w,        0x1 */
            uint32_t bz_pa_vbcore                   :  4; /* [19:16],        r/w,        0x5 */
            uint32_t bz_pa_etb_en                   :  1; /* [   20],        r/w,        0x0 */
            uint32_t bz_pa_ib_fix                   :  1; /* [   21],        r/w,        0x1 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_config_bz;

    /* 0x7DC : tx_dpd_sram_ctrl_0 */
    union {
        struct {
            uint32_t tx_dpd_page_num                :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3_11                  :  9; /* [11: 3],       rsvd,        0x0 */
            uint32_t tx_dpd_amp_manu_read_addr      :  8; /* [19:12],        r/w,        0x0 */
            uint32_t tx_dpd_pha_manu_read_addr      :  8; /* [27:20],        r/w,        0x0 */
            uint32_t tx_dpd_amp_manu_read_en        :  1; /* [   28],        r/w,        0x0 */
            uint32_t tx_dpd_pha_manu_read_en        :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_dpd_sram_ctrl_0;

    /* 0x7E0 : tx_dpd_sram_ctrl_1 */
    union {
        struct {
            uint32_t tx_dpd_amp_spram_rddata        : 24; /* [23: 0],          r,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_dpd_sram_ctrl_1;

    /* 0x7E4 : tx_dpd_sram_ctrl_2 */
    union {
        struct {
            uint32_t tx_dpd_pha_spram_rddata        : 28; /* [27: 0],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_dpd_sram_ctrl_2;

    /* 0x7e8  reserved */
    uint8_t RESERVED0x7e8[24];

    /* 0x800 : tx_dpd_ctrl_0 */
    union {
        struct {
            uint32_t tx_dpd_en                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t tx_dpd_amon                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t tx_dpd_pmon                    :  1; /* [    2],        r/w,        0x0 */
            uint32_t tx_dpd_in_scal                 :  6; /* [ 8: 3],        r/w,        0x0 */
            uint32_t tx_dpd_in_scal_en              :  1; /* [    9],        r/w,        0x0 */
            uint32_t tx_dpd_lut_p_mode              :  1; /* [   10],        r/w,        0x0 */
            uint32_t tx_dpd_lut_x_mode              :  1; /* [   11],        r/w,        0x0 */
            uint32_t tx_dpd_lut_mode                :  1; /* [   12],        r/w,        0x0 */
            uint32_t tx_dpd_ctrl_hw                 :  1; /* [   13],        r/w,        0x0 */
            uint32_t tx_dpd_lp_ctrl_on              :  1; /* [   14],        r/w,        0x0 */
            uint32_t tx_dpd_lp_th                   :  6; /* [20:15],        r/w,       0x12 */
            uint32_t tx_dpd_comp_mode               :  2; /* [22:21],        r/w,        0x3 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_dpd_ctrl_0;

    /* 0x804 : tx_dpd_ctrl_1 */
    union {
        struct {
            uint32_t tx_dpd_en_ofdm                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t tx_dpd_en_dsss                 :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_dpd_ctrl_1;

    /* 0x808  reserved */
    uint8_t RESERVED0x808[520];

    /* 0xA10 : rx_dpd_ctrl_0 */
    union {
        struct {
            uint32_t rx_dpd_done                    :  1; /* [    0],          r,        0x0 */
            uint32_t rx_dpd_i_path_only             :  1; /* [    1],        r/w,        0x0 */
            uint32_t rx_dpd_bypass_filter           :  1; /* [    2],        r/w,        0x0 */
            uint32_t rx_dpd_wait_time_us            :  5; /* [ 7: 3],        r/w,        0x0 */
            uint32_t rx_dpd_fc_mhz                  :  5; /* [12: 8],        r/w,       0x10 */
            uint32_t rx_dpd_dgc_gain_dB             :  5; /* [17:13],        r/w,        0x0 */
            uint32_t rx_dpd_dgc_shift               :  3; /* [20:18],        r/w,        0x0 */
            uint32_t rx_dpd_n_period                :  3; /* [23:21],        r/w,        0x1 */
            uint32_t rx_dpd_en                      :  1; /* [   24],        r/w,        0x0 */
            uint32_t rx_dpd_sel                     :  1; /* [   25],        r/w,        0x1 */
            uint32_t rx_dpd_sram_en                 :  1; /* [   26],        r/w,        0x1 */
            uint32_t rx_dpd_sw_reset                :  1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_30                 :  3; /* [30:28],       rsvd,        0x0 */
            uint32_t sram_clk_4s_en                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_dpd_ctrl_0;

    /* 0xA14 : rx_dpd_ctrl_1 */
    union {
        struct {
            uint32_t rx_dpd_sample_offset           :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t rx_dpd_sync_mf_out_mag_th      :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t rx_dpd_sync_mf_in_mag_th       :  2; /* [11:10],        r/w,        0x0 */
            uint32_t rx_dpd_use_sync_mf             :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rx_dpd_dly_th                  : 10; /* [25:16],        r/w,        0x7 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_dpd_ctrl_1;

};

typedef volatile struct rf_reg rf_reg_t;


#endif  /* __RF_REG_H__ */
