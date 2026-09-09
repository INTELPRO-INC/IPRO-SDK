/**
  ******************************************************************************
  * @file    rf_fsm_reg.h
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
#ifndef  __RF_FSM_REG_H__
#define  __RF_FSM_REG_H__

#include <stdint.h>

/* 0x500 : rf_fsm_0 */
#define RF_FSM_0_OFFSET                                         (0x500)
#define RF_FSM_VECTOR_00                                        RF_FSM_VECTOR_00
#define RF_FSM_VECTOR_00_POS                                    (0U)
#define RF_FSM_VECTOR_00_LEN                                    (1U)
#define RF_FSM_VECTOR_00_MSK                                    (((1U<<RF_FSM_VECTOR_00_LEN)-1)<<RF_FSM_VECTOR_00_POS)
#define RF_FSM_VECTOR_00_UMSK                                   (~(((1U<<RF_FSM_VECTOR_00_LEN)-1)<<RF_FSM_VECTOR_00_POS))
#define RF_FSM_VECTOR_01                                        RF_FSM_VECTOR_01
#define RF_FSM_VECTOR_01_POS                                    (1U)
#define RF_FSM_VECTOR_01_LEN                                    (1U)
#define RF_FSM_VECTOR_01_MSK                                    (((1U<<RF_FSM_VECTOR_01_LEN)-1)<<RF_FSM_VECTOR_01_POS)
#define RF_FSM_VECTOR_01_UMSK                                   (~(((1U<<RF_FSM_VECTOR_01_LEN)-1)<<RF_FSM_VECTOR_01_POS))
#define RF_FSM_VECTOR_02                                        RF_FSM_VECTOR_02
#define RF_FSM_VECTOR_02_POS                                    (2U)
#define RF_FSM_VECTOR_02_LEN                                    (1U)
#define RF_FSM_VECTOR_02_MSK                                    (((1U<<RF_FSM_VECTOR_02_LEN)-1)<<RF_FSM_VECTOR_02_POS)
#define RF_FSM_VECTOR_02_UMSK                                   (~(((1U<<RF_FSM_VECTOR_02_LEN)-1)<<RF_FSM_VECTOR_02_POS))
#define RF_FSM_VECTOR_03                                        RF_FSM_VECTOR_03
#define RF_FSM_VECTOR_03_POS                                    (3U)
#define RF_FSM_VECTOR_03_LEN                                    (1U)
#define RF_FSM_VECTOR_03_MSK                                    (((1U<<RF_FSM_VECTOR_03_LEN)-1)<<RF_FSM_VECTOR_03_POS)
#define RF_FSM_VECTOR_03_UMSK                                   (~(((1U<<RF_FSM_VECTOR_03_LEN)-1)<<RF_FSM_VECTOR_03_POS))
#define RF_FSM_VECTOR_04                                        RF_FSM_VECTOR_04
#define RF_FSM_VECTOR_04_POS                                    (4U)
#define RF_FSM_VECTOR_04_LEN                                    (1U)
#define RF_FSM_VECTOR_04_MSK                                    (((1U<<RF_FSM_VECTOR_04_LEN)-1)<<RF_FSM_VECTOR_04_POS)
#define RF_FSM_VECTOR_04_UMSK                                   (~(((1U<<RF_FSM_VECTOR_04_LEN)-1)<<RF_FSM_VECTOR_04_POS))
#define RF_FSM_VECTOR_05                                        RF_FSM_VECTOR_05
#define RF_FSM_VECTOR_05_POS                                    (5U)
#define RF_FSM_VECTOR_05_LEN                                    (1U)
#define RF_FSM_VECTOR_05_MSK                                    (((1U<<RF_FSM_VECTOR_05_LEN)-1)<<RF_FSM_VECTOR_05_POS)
#define RF_FSM_VECTOR_05_UMSK                                   (~(((1U<<RF_FSM_VECTOR_05_LEN)-1)<<RF_FSM_VECTOR_05_POS))
#define RF_FSM_VECTOR_06                                        RF_FSM_VECTOR_06
#define RF_FSM_VECTOR_06_POS                                    (6U)
#define RF_FSM_VECTOR_06_LEN                                    (1U)
#define RF_FSM_VECTOR_06_MSK                                    (((1U<<RF_FSM_VECTOR_06_LEN)-1)<<RF_FSM_VECTOR_06_POS)
#define RF_FSM_VECTOR_06_UMSK                                   (~(((1U<<RF_FSM_VECTOR_06_LEN)-1)<<RF_FSM_VECTOR_06_POS))
#define RF_FSM_VECTOR_07                                        RF_FSM_VECTOR_07
#define RF_FSM_VECTOR_07_POS                                    (7U)
#define RF_FSM_VECTOR_07_LEN                                    (1U)
#define RF_FSM_VECTOR_07_MSK                                    (((1U<<RF_FSM_VECTOR_07_LEN)-1)<<RF_FSM_VECTOR_07_POS)
#define RF_FSM_VECTOR_07_UMSK                                   (~(((1U<<RF_FSM_VECTOR_07_LEN)-1)<<RF_FSM_VECTOR_07_POS))
#define RF_FSM_VECTOR_08                                        RF_FSM_VECTOR_08
#define RF_FSM_VECTOR_08_POS                                    (8U)
#define RF_FSM_VECTOR_08_LEN                                    (1U)
#define RF_FSM_VECTOR_08_MSK                                    (((1U<<RF_FSM_VECTOR_08_LEN)-1)<<RF_FSM_VECTOR_08_POS)
#define RF_FSM_VECTOR_08_UMSK                                   (~(((1U<<RF_FSM_VECTOR_08_LEN)-1)<<RF_FSM_VECTOR_08_POS))
#define RF_FSM_VECTOR_09                                        RF_FSM_VECTOR_09
#define RF_FSM_VECTOR_09_POS                                    (9U)
#define RF_FSM_VECTOR_09_LEN                                    (1U)
#define RF_FSM_VECTOR_09_MSK                                    (((1U<<RF_FSM_VECTOR_09_LEN)-1)<<RF_FSM_VECTOR_09_POS)
#define RF_FSM_VECTOR_09_UMSK                                   (~(((1U<<RF_FSM_VECTOR_09_LEN)-1)<<RF_FSM_VECTOR_09_POS))
#define RF_FSM_VECTOR_10                                        RF_FSM_VECTOR_10
#define RF_FSM_VECTOR_10_POS                                    (10U)
#define RF_FSM_VECTOR_10_LEN                                    (1U)
#define RF_FSM_VECTOR_10_MSK                                    (((1U<<RF_FSM_VECTOR_10_LEN)-1)<<RF_FSM_VECTOR_10_POS)
#define RF_FSM_VECTOR_10_UMSK                                   (~(((1U<<RF_FSM_VECTOR_10_LEN)-1)<<RF_FSM_VECTOR_10_POS))
#define RF_FSM_VECTOR_11                                        RF_FSM_VECTOR_11
#define RF_FSM_VECTOR_11_POS                                    (11U)
#define RF_FSM_VECTOR_11_LEN                                    (1U)
#define RF_FSM_VECTOR_11_MSK                                    (((1U<<RF_FSM_VECTOR_11_LEN)-1)<<RF_FSM_VECTOR_11_POS)
#define RF_FSM_VECTOR_11_UMSK                                   (~(((1U<<RF_FSM_VECTOR_11_LEN)-1)<<RF_FSM_VECTOR_11_POS))
#define RF_FSM_VECTOR_12                                        RF_FSM_VECTOR_12
#define RF_FSM_VECTOR_12_POS                                    (12U)
#define RF_FSM_VECTOR_12_LEN                                    (1U)
#define RF_FSM_VECTOR_12_MSK                                    (((1U<<RF_FSM_VECTOR_12_LEN)-1)<<RF_FSM_VECTOR_12_POS)
#define RF_FSM_VECTOR_12_UMSK                                   (~(((1U<<RF_FSM_VECTOR_12_LEN)-1)<<RF_FSM_VECTOR_12_POS))
#define RF_FSM_VECTOR_13                                        RF_FSM_VECTOR_13
#define RF_FSM_VECTOR_13_POS                                    (13U)
#define RF_FSM_VECTOR_13_LEN                                    (1U)
#define RF_FSM_VECTOR_13_MSK                                    (((1U<<RF_FSM_VECTOR_13_LEN)-1)<<RF_FSM_VECTOR_13_POS)
#define RF_FSM_VECTOR_13_UMSK                                   (~(((1U<<RF_FSM_VECTOR_13_LEN)-1)<<RF_FSM_VECTOR_13_POS))
#define RF_FSM_VECTOR_14                                        RF_FSM_VECTOR_14
#define RF_FSM_VECTOR_14_POS                                    (14U)
#define RF_FSM_VECTOR_14_LEN                                    (1U)
#define RF_FSM_VECTOR_14_MSK                                    (((1U<<RF_FSM_VECTOR_14_LEN)-1)<<RF_FSM_VECTOR_14_POS)
#define RF_FSM_VECTOR_14_UMSK                                   (~(((1U<<RF_FSM_VECTOR_14_LEN)-1)<<RF_FSM_VECTOR_14_POS))
#define RF_FSM_VECTOR_15                                        RF_FSM_VECTOR_15
#define RF_FSM_VECTOR_15_POS                                    (15U)
#define RF_FSM_VECTOR_15_LEN                                    (1U)
#define RF_FSM_VECTOR_15_MSK                                    (((1U<<RF_FSM_VECTOR_15_LEN)-1)<<RF_FSM_VECTOR_15_POS)
#define RF_FSM_VECTOR_15_UMSK                                   (~(((1U<<RF_FSM_VECTOR_15_LEN)-1)<<RF_FSM_VECTOR_15_POS))
#define RF_FSM_VECTOR_16                                        RF_FSM_VECTOR_16
#define RF_FSM_VECTOR_16_POS                                    (16U)
#define RF_FSM_VECTOR_16_LEN                                    (1U)
#define RF_FSM_VECTOR_16_MSK                                    (((1U<<RF_FSM_VECTOR_16_LEN)-1)<<RF_FSM_VECTOR_16_POS)
#define RF_FSM_VECTOR_16_UMSK                                   (~(((1U<<RF_FSM_VECTOR_16_LEN)-1)<<RF_FSM_VECTOR_16_POS))
#define RF_FSM_VECTOR_17                                        RF_FSM_VECTOR_17
#define RF_FSM_VECTOR_17_POS                                    (17U)
#define RF_FSM_VECTOR_17_LEN                                    (1U)
#define RF_FSM_VECTOR_17_MSK                                    (((1U<<RF_FSM_VECTOR_17_LEN)-1)<<RF_FSM_VECTOR_17_POS)
#define RF_FSM_VECTOR_17_UMSK                                   (~(((1U<<RF_FSM_VECTOR_17_LEN)-1)<<RF_FSM_VECTOR_17_POS))
#define RF_FSM_VECTOR_18                                        RF_FSM_VECTOR_18
#define RF_FSM_VECTOR_18_POS                                    (18U)
#define RF_FSM_VECTOR_18_LEN                                    (1U)
#define RF_FSM_VECTOR_18_MSK                                    (((1U<<RF_FSM_VECTOR_18_LEN)-1)<<RF_FSM_VECTOR_18_POS)
#define RF_FSM_VECTOR_18_UMSK                                   (~(((1U<<RF_FSM_VECTOR_18_LEN)-1)<<RF_FSM_VECTOR_18_POS))
#define RF_FSM_VECTOR_19                                        RF_FSM_VECTOR_19
#define RF_FSM_VECTOR_19_POS                                    (19U)
#define RF_FSM_VECTOR_19_LEN                                    (1U)
#define RF_FSM_VECTOR_19_MSK                                    (((1U<<RF_FSM_VECTOR_19_LEN)-1)<<RF_FSM_VECTOR_19_POS)
#define RF_FSM_VECTOR_19_UMSK                                   (~(((1U<<RF_FSM_VECTOR_19_LEN)-1)<<RF_FSM_VECTOR_19_POS))
#define RF_FSM_VECTOR_20                                        RF_FSM_VECTOR_20
#define RF_FSM_VECTOR_20_POS                                    (20U)
#define RF_FSM_VECTOR_20_LEN                                    (1U)
#define RF_FSM_VECTOR_20_MSK                                    (((1U<<RF_FSM_VECTOR_20_LEN)-1)<<RF_FSM_VECTOR_20_POS)
#define RF_FSM_VECTOR_20_UMSK                                   (~(((1U<<RF_FSM_VECTOR_20_LEN)-1)<<RF_FSM_VECTOR_20_POS))
#define RF_FSM_VECTOR_21                                        RF_FSM_VECTOR_21
#define RF_FSM_VECTOR_21_POS                                    (21U)
#define RF_FSM_VECTOR_21_LEN                                    (1U)
#define RF_FSM_VECTOR_21_MSK                                    (((1U<<RF_FSM_VECTOR_21_LEN)-1)<<RF_FSM_VECTOR_21_POS)
#define RF_FSM_VECTOR_21_UMSK                                   (~(((1U<<RF_FSM_VECTOR_21_LEN)-1)<<RF_FSM_VECTOR_21_POS))
#define RF_FSM_VECTOR_22                                        RF_FSM_VECTOR_22
#define RF_FSM_VECTOR_22_POS                                    (22U)
#define RF_FSM_VECTOR_22_LEN                                    (1U)
#define RF_FSM_VECTOR_22_MSK                                    (((1U<<RF_FSM_VECTOR_22_LEN)-1)<<RF_FSM_VECTOR_22_POS)
#define RF_FSM_VECTOR_22_UMSK                                   (~(((1U<<RF_FSM_VECTOR_22_LEN)-1)<<RF_FSM_VECTOR_22_POS))
#define RF_FSM_VECTOR_23                                        RF_FSM_VECTOR_23
#define RF_FSM_VECTOR_23_POS                                    (23U)
#define RF_FSM_VECTOR_23_LEN                                    (1U)
#define RF_FSM_VECTOR_23_MSK                                    (((1U<<RF_FSM_VECTOR_23_LEN)-1)<<RF_FSM_VECTOR_23_POS)
#define RF_FSM_VECTOR_23_UMSK                                   (~(((1U<<RF_FSM_VECTOR_23_LEN)-1)<<RF_FSM_VECTOR_23_POS))

/* 0x504 : sb_rf_fsm_0 */
#define RF_FSM_SB_RF_FSM_0_OFFSET                               (0x504)
#define RF_FSM_SB_VECTOR_00                                     RF_FSM_SB_VECTOR_00
#define RF_FSM_SB_VECTOR_00_POS                                 (0U)
#define RF_FSM_SB_VECTOR_00_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_00_MSK                                 (((1U<<RF_FSM_SB_VECTOR_00_LEN)-1)<<RF_FSM_SB_VECTOR_00_POS)
#define RF_FSM_SB_VECTOR_00_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_00_LEN)-1)<<RF_FSM_SB_VECTOR_00_POS))
#define RF_FSM_SB_VECTOR_01                                     RF_FSM_SB_VECTOR_01
#define RF_FSM_SB_VECTOR_01_POS                                 (1U)
#define RF_FSM_SB_VECTOR_01_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_01_MSK                                 (((1U<<RF_FSM_SB_VECTOR_01_LEN)-1)<<RF_FSM_SB_VECTOR_01_POS)
#define RF_FSM_SB_VECTOR_01_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_01_LEN)-1)<<RF_FSM_SB_VECTOR_01_POS))
#define RF_FSM_SB_VECTOR_02                                     RF_FSM_SB_VECTOR_02
#define RF_FSM_SB_VECTOR_02_POS                                 (2U)
#define RF_FSM_SB_VECTOR_02_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_02_MSK                                 (((1U<<RF_FSM_SB_VECTOR_02_LEN)-1)<<RF_FSM_SB_VECTOR_02_POS)
#define RF_FSM_SB_VECTOR_02_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_02_LEN)-1)<<RF_FSM_SB_VECTOR_02_POS))
#define RF_FSM_SB_VECTOR_03                                     RF_FSM_SB_VECTOR_03
#define RF_FSM_SB_VECTOR_03_POS                                 (3U)
#define RF_FSM_SB_VECTOR_03_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_03_MSK                                 (((1U<<RF_FSM_SB_VECTOR_03_LEN)-1)<<RF_FSM_SB_VECTOR_03_POS)
#define RF_FSM_SB_VECTOR_03_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_03_LEN)-1)<<RF_FSM_SB_VECTOR_03_POS))
#define RF_FSM_SB_VECTOR_04                                     RF_FSM_SB_VECTOR_04
#define RF_FSM_SB_VECTOR_04_POS                                 (4U)
#define RF_FSM_SB_VECTOR_04_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_04_MSK                                 (((1U<<RF_FSM_SB_VECTOR_04_LEN)-1)<<RF_FSM_SB_VECTOR_04_POS)
#define RF_FSM_SB_VECTOR_04_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_04_LEN)-1)<<RF_FSM_SB_VECTOR_04_POS))
#define RF_FSM_SB_VECTOR_05                                     RF_FSM_SB_VECTOR_05
#define RF_FSM_SB_VECTOR_05_POS                                 (5U)
#define RF_FSM_SB_VECTOR_05_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_05_MSK                                 (((1U<<RF_FSM_SB_VECTOR_05_LEN)-1)<<RF_FSM_SB_VECTOR_05_POS)
#define RF_FSM_SB_VECTOR_05_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_05_LEN)-1)<<RF_FSM_SB_VECTOR_05_POS))
#define RF_FSM_SB_VECTOR_06                                     RF_FSM_SB_VECTOR_06
#define RF_FSM_SB_VECTOR_06_POS                                 (6U)
#define RF_FSM_SB_VECTOR_06_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_06_MSK                                 (((1U<<RF_FSM_SB_VECTOR_06_LEN)-1)<<RF_FSM_SB_VECTOR_06_POS)
#define RF_FSM_SB_VECTOR_06_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_06_LEN)-1)<<RF_FSM_SB_VECTOR_06_POS))
#define RF_FSM_SB_VECTOR_07                                     RF_FSM_SB_VECTOR_07
#define RF_FSM_SB_VECTOR_07_POS                                 (7U)
#define RF_FSM_SB_VECTOR_07_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_07_MSK                                 (((1U<<RF_FSM_SB_VECTOR_07_LEN)-1)<<RF_FSM_SB_VECTOR_07_POS)
#define RF_FSM_SB_VECTOR_07_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_07_LEN)-1)<<RF_FSM_SB_VECTOR_07_POS))
#define RF_FSM_SB_VECTOR_08                                     RF_FSM_SB_VECTOR_08
#define RF_FSM_SB_VECTOR_08_POS                                 (8U)
#define RF_FSM_SB_VECTOR_08_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_08_MSK                                 (((1U<<RF_FSM_SB_VECTOR_08_LEN)-1)<<RF_FSM_SB_VECTOR_08_POS)
#define RF_FSM_SB_VECTOR_08_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_08_LEN)-1)<<RF_FSM_SB_VECTOR_08_POS))
#define RF_FSM_SB_VECTOR_09                                     RF_FSM_SB_VECTOR_09
#define RF_FSM_SB_VECTOR_09_POS                                 (9U)
#define RF_FSM_SB_VECTOR_09_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_09_MSK                                 (((1U<<RF_FSM_SB_VECTOR_09_LEN)-1)<<RF_FSM_SB_VECTOR_09_POS)
#define RF_FSM_SB_VECTOR_09_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_09_LEN)-1)<<RF_FSM_SB_VECTOR_09_POS))
#define RF_FSM_SB_VECTOR_10                                     RF_FSM_SB_VECTOR_10
#define RF_FSM_SB_VECTOR_10_POS                                 (10U)
#define RF_FSM_SB_VECTOR_10_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_10_MSK                                 (((1U<<RF_FSM_SB_VECTOR_10_LEN)-1)<<RF_FSM_SB_VECTOR_10_POS)
#define RF_FSM_SB_VECTOR_10_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_10_LEN)-1)<<RF_FSM_SB_VECTOR_10_POS))
#define RF_FSM_SB_VECTOR_11                                     RF_FSM_SB_VECTOR_11
#define RF_FSM_SB_VECTOR_11_POS                                 (11U)
#define RF_FSM_SB_VECTOR_11_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_11_MSK                                 (((1U<<RF_FSM_SB_VECTOR_11_LEN)-1)<<RF_FSM_SB_VECTOR_11_POS)
#define RF_FSM_SB_VECTOR_11_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_11_LEN)-1)<<RF_FSM_SB_VECTOR_11_POS))
#define RF_FSM_SB_VECTOR_12                                     RF_FSM_SB_VECTOR_12
#define RF_FSM_SB_VECTOR_12_POS                                 (12U)
#define RF_FSM_SB_VECTOR_12_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_12_MSK                                 (((1U<<RF_FSM_SB_VECTOR_12_LEN)-1)<<RF_FSM_SB_VECTOR_12_POS)
#define RF_FSM_SB_VECTOR_12_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_12_LEN)-1)<<RF_FSM_SB_VECTOR_12_POS))
#define RF_FSM_SB_VECTOR_13                                     RF_FSM_SB_VECTOR_13
#define RF_FSM_SB_VECTOR_13_POS                                 (13U)
#define RF_FSM_SB_VECTOR_13_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_13_MSK                                 (((1U<<RF_FSM_SB_VECTOR_13_LEN)-1)<<RF_FSM_SB_VECTOR_13_POS)
#define RF_FSM_SB_VECTOR_13_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_13_LEN)-1)<<RF_FSM_SB_VECTOR_13_POS))
#define RF_FSM_SB_VECTOR_14                                     RF_FSM_SB_VECTOR_14
#define RF_FSM_SB_VECTOR_14_POS                                 (14U)
#define RF_FSM_SB_VECTOR_14_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_14_MSK                                 (((1U<<RF_FSM_SB_VECTOR_14_LEN)-1)<<RF_FSM_SB_VECTOR_14_POS)
#define RF_FSM_SB_VECTOR_14_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_14_LEN)-1)<<RF_FSM_SB_VECTOR_14_POS))
#define RF_FSM_SB_VECTOR_15                                     RF_FSM_SB_VECTOR_15
#define RF_FSM_SB_VECTOR_15_POS                                 (15U)
#define RF_FSM_SB_VECTOR_15_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_15_MSK                                 (((1U<<RF_FSM_SB_VECTOR_15_LEN)-1)<<RF_FSM_SB_VECTOR_15_POS)
#define RF_FSM_SB_VECTOR_15_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_15_LEN)-1)<<RF_FSM_SB_VECTOR_15_POS))
#define RF_FSM_SB_VECTOR_16                                     RF_FSM_SB_VECTOR_16
#define RF_FSM_SB_VECTOR_16_POS                                 (16U)
#define RF_FSM_SB_VECTOR_16_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_16_MSK                                 (((1U<<RF_FSM_SB_VECTOR_16_LEN)-1)<<RF_FSM_SB_VECTOR_16_POS)
#define RF_FSM_SB_VECTOR_16_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_16_LEN)-1)<<RF_FSM_SB_VECTOR_16_POS))
#define RF_FSM_SB_VECTOR_17                                     RF_FSM_SB_VECTOR_17
#define RF_FSM_SB_VECTOR_17_POS                                 (17U)
#define RF_FSM_SB_VECTOR_17_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_17_MSK                                 (((1U<<RF_FSM_SB_VECTOR_17_LEN)-1)<<RF_FSM_SB_VECTOR_17_POS)
#define RF_FSM_SB_VECTOR_17_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_17_LEN)-1)<<RF_FSM_SB_VECTOR_17_POS))
#define RF_FSM_SB_VECTOR_18                                     RF_FSM_SB_VECTOR_18
#define RF_FSM_SB_VECTOR_18_POS                                 (18U)
#define RF_FSM_SB_VECTOR_18_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_18_MSK                                 (((1U<<RF_FSM_SB_VECTOR_18_LEN)-1)<<RF_FSM_SB_VECTOR_18_POS)
#define RF_FSM_SB_VECTOR_18_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_18_LEN)-1)<<RF_FSM_SB_VECTOR_18_POS))
#define RF_FSM_SB_VECTOR_19                                     RF_FSM_SB_VECTOR_19
#define RF_FSM_SB_VECTOR_19_POS                                 (19U)
#define RF_FSM_SB_VECTOR_19_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_19_MSK                                 (((1U<<RF_FSM_SB_VECTOR_19_LEN)-1)<<RF_FSM_SB_VECTOR_19_POS)
#define RF_FSM_SB_VECTOR_19_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_19_LEN)-1)<<RF_FSM_SB_VECTOR_19_POS))
#define RF_FSM_SB_VECTOR_20                                     RF_FSM_SB_VECTOR_20
#define RF_FSM_SB_VECTOR_20_POS                                 (20U)
#define RF_FSM_SB_VECTOR_20_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_20_MSK                                 (((1U<<RF_FSM_SB_VECTOR_20_LEN)-1)<<RF_FSM_SB_VECTOR_20_POS)
#define RF_FSM_SB_VECTOR_20_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_20_LEN)-1)<<RF_FSM_SB_VECTOR_20_POS))
#define RF_FSM_SB_VECTOR_21                                     RF_FSM_SB_VECTOR_21
#define RF_FSM_SB_VECTOR_21_POS                                 (21U)
#define RF_FSM_SB_VECTOR_21_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_21_MSK                                 (((1U<<RF_FSM_SB_VECTOR_21_LEN)-1)<<RF_FSM_SB_VECTOR_21_POS)
#define RF_FSM_SB_VECTOR_21_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_21_LEN)-1)<<RF_FSM_SB_VECTOR_21_POS))
#define RF_FSM_SB_VECTOR_22                                     RF_FSM_SB_VECTOR_22
#define RF_FSM_SB_VECTOR_22_POS                                 (22U)
#define RF_FSM_SB_VECTOR_22_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_22_MSK                                 (((1U<<RF_FSM_SB_VECTOR_22_LEN)-1)<<RF_FSM_SB_VECTOR_22_POS)
#define RF_FSM_SB_VECTOR_22_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_22_LEN)-1)<<RF_FSM_SB_VECTOR_22_POS))
#define RF_FSM_SB_VECTOR_23                                     RF_FSM_SB_VECTOR_23
#define RF_FSM_SB_VECTOR_23_POS                                 (23U)
#define RF_FSM_SB_VECTOR_23_LEN                                 (1U)
#define RF_FSM_SB_VECTOR_23_MSK                                 (((1U<<RF_FSM_SB_VECTOR_23_LEN)-1)<<RF_FSM_SB_VECTOR_23_POS)
#define RF_FSM_SB_VECTOR_23_UMSK                                (~(((1U<<RF_FSM_SB_VECTOR_23_LEN)-1)<<RF_FSM_SB_VECTOR_23_POS))

/* 0x508 : lo_rf_fsm_0 */
#define RF_FSM_LO_RF_FSM_0_OFFSET                               (0x508)
#define RF_FSM_LO_VECTOR_00                                     RF_FSM_LO_VECTOR_00
#define RF_FSM_LO_VECTOR_00_POS                                 (0U)
#define RF_FSM_LO_VECTOR_00_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_00_MSK                                 (((1U<<RF_FSM_LO_VECTOR_00_LEN)-1)<<RF_FSM_LO_VECTOR_00_POS)
#define RF_FSM_LO_VECTOR_00_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_00_LEN)-1)<<RF_FSM_LO_VECTOR_00_POS))
#define RF_FSM_LO_VECTOR_01                                     RF_FSM_LO_VECTOR_01
#define RF_FSM_LO_VECTOR_01_POS                                 (1U)
#define RF_FSM_LO_VECTOR_01_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_01_MSK                                 (((1U<<RF_FSM_LO_VECTOR_01_LEN)-1)<<RF_FSM_LO_VECTOR_01_POS)
#define RF_FSM_LO_VECTOR_01_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_01_LEN)-1)<<RF_FSM_LO_VECTOR_01_POS))
#define RF_FSM_LO_VECTOR_02                                     RF_FSM_LO_VECTOR_02
#define RF_FSM_LO_VECTOR_02_POS                                 (2U)
#define RF_FSM_LO_VECTOR_02_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_02_MSK                                 (((1U<<RF_FSM_LO_VECTOR_02_LEN)-1)<<RF_FSM_LO_VECTOR_02_POS)
#define RF_FSM_LO_VECTOR_02_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_02_LEN)-1)<<RF_FSM_LO_VECTOR_02_POS))
#define RF_FSM_LO_VECTOR_03                                     RF_FSM_LO_VECTOR_03
#define RF_FSM_LO_VECTOR_03_POS                                 (3U)
#define RF_FSM_LO_VECTOR_03_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_03_MSK                                 (((1U<<RF_FSM_LO_VECTOR_03_LEN)-1)<<RF_FSM_LO_VECTOR_03_POS)
#define RF_FSM_LO_VECTOR_03_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_03_LEN)-1)<<RF_FSM_LO_VECTOR_03_POS))
#define RF_FSM_LO_VECTOR_04                                     RF_FSM_LO_VECTOR_04
#define RF_FSM_LO_VECTOR_04_POS                                 (4U)
#define RF_FSM_LO_VECTOR_04_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_04_MSK                                 (((1U<<RF_FSM_LO_VECTOR_04_LEN)-1)<<RF_FSM_LO_VECTOR_04_POS)
#define RF_FSM_LO_VECTOR_04_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_04_LEN)-1)<<RF_FSM_LO_VECTOR_04_POS))
#define RF_FSM_LO_VECTOR_05                                     RF_FSM_LO_VECTOR_05
#define RF_FSM_LO_VECTOR_05_POS                                 (5U)
#define RF_FSM_LO_VECTOR_05_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_05_MSK                                 (((1U<<RF_FSM_LO_VECTOR_05_LEN)-1)<<RF_FSM_LO_VECTOR_05_POS)
#define RF_FSM_LO_VECTOR_05_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_05_LEN)-1)<<RF_FSM_LO_VECTOR_05_POS))
#define RF_FSM_LO_VECTOR_06                                     RF_FSM_LO_VECTOR_06
#define RF_FSM_LO_VECTOR_06_POS                                 (6U)
#define RF_FSM_LO_VECTOR_06_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_06_MSK                                 (((1U<<RF_FSM_LO_VECTOR_06_LEN)-1)<<RF_FSM_LO_VECTOR_06_POS)
#define RF_FSM_LO_VECTOR_06_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_06_LEN)-1)<<RF_FSM_LO_VECTOR_06_POS))
#define RF_FSM_LO_VECTOR_07                                     RF_FSM_LO_VECTOR_07
#define RF_FSM_LO_VECTOR_07_POS                                 (7U)
#define RF_FSM_LO_VECTOR_07_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_07_MSK                                 (((1U<<RF_FSM_LO_VECTOR_07_LEN)-1)<<RF_FSM_LO_VECTOR_07_POS)
#define RF_FSM_LO_VECTOR_07_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_07_LEN)-1)<<RF_FSM_LO_VECTOR_07_POS))
#define RF_FSM_LO_VECTOR_08                                     RF_FSM_LO_VECTOR_08
#define RF_FSM_LO_VECTOR_08_POS                                 (8U)
#define RF_FSM_LO_VECTOR_08_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_08_MSK                                 (((1U<<RF_FSM_LO_VECTOR_08_LEN)-1)<<RF_FSM_LO_VECTOR_08_POS)
#define RF_FSM_LO_VECTOR_08_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_08_LEN)-1)<<RF_FSM_LO_VECTOR_08_POS))
#define RF_FSM_LO_VECTOR_09                                     RF_FSM_LO_VECTOR_09
#define RF_FSM_LO_VECTOR_09_POS                                 (9U)
#define RF_FSM_LO_VECTOR_09_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_09_MSK                                 (((1U<<RF_FSM_LO_VECTOR_09_LEN)-1)<<RF_FSM_LO_VECTOR_09_POS)
#define RF_FSM_LO_VECTOR_09_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_09_LEN)-1)<<RF_FSM_LO_VECTOR_09_POS))
#define RF_FSM_LO_VECTOR_10                                     RF_FSM_LO_VECTOR_10
#define RF_FSM_LO_VECTOR_10_POS                                 (10U)
#define RF_FSM_LO_VECTOR_10_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_10_MSK                                 (((1U<<RF_FSM_LO_VECTOR_10_LEN)-1)<<RF_FSM_LO_VECTOR_10_POS)
#define RF_FSM_LO_VECTOR_10_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_10_LEN)-1)<<RF_FSM_LO_VECTOR_10_POS))
#define RF_FSM_LO_VECTOR_11                                     RF_FSM_LO_VECTOR_11
#define RF_FSM_LO_VECTOR_11_POS                                 (11U)
#define RF_FSM_LO_VECTOR_11_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_11_MSK                                 (((1U<<RF_FSM_LO_VECTOR_11_LEN)-1)<<RF_FSM_LO_VECTOR_11_POS)
#define RF_FSM_LO_VECTOR_11_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_11_LEN)-1)<<RF_FSM_LO_VECTOR_11_POS))
#define RF_FSM_LO_VECTOR_12                                     RF_FSM_LO_VECTOR_12
#define RF_FSM_LO_VECTOR_12_POS                                 (12U)
#define RF_FSM_LO_VECTOR_12_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_12_MSK                                 (((1U<<RF_FSM_LO_VECTOR_12_LEN)-1)<<RF_FSM_LO_VECTOR_12_POS)
#define RF_FSM_LO_VECTOR_12_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_12_LEN)-1)<<RF_FSM_LO_VECTOR_12_POS))
#define RF_FSM_LO_VECTOR_13                                     RF_FSM_LO_VECTOR_13
#define RF_FSM_LO_VECTOR_13_POS                                 (13U)
#define RF_FSM_LO_VECTOR_13_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_13_MSK                                 (((1U<<RF_FSM_LO_VECTOR_13_LEN)-1)<<RF_FSM_LO_VECTOR_13_POS)
#define RF_FSM_LO_VECTOR_13_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_13_LEN)-1)<<RF_FSM_LO_VECTOR_13_POS))
#define RF_FSM_LO_VECTOR_14                                     RF_FSM_LO_VECTOR_14
#define RF_FSM_LO_VECTOR_14_POS                                 (14U)
#define RF_FSM_LO_VECTOR_14_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_14_MSK                                 (((1U<<RF_FSM_LO_VECTOR_14_LEN)-1)<<RF_FSM_LO_VECTOR_14_POS)
#define RF_FSM_LO_VECTOR_14_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_14_LEN)-1)<<RF_FSM_LO_VECTOR_14_POS))
#define RF_FSM_LO_VECTOR_15                                     RF_FSM_LO_VECTOR_15
#define RF_FSM_LO_VECTOR_15_POS                                 (15U)
#define RF_FSM_LO_VECTOR_15_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_15_MSK                                 (((1U<<RF_FSM_LO_VECTOR_15_LEN)-1)<<RF_FSM_LO_VECTOR_15_POS)
#define RF_FSM_LO_VECTOR_15_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_15_LEN)-1)<<RF_FSM_LO_VECTOR_15_POS))
#define RF_FSM_LO_VECTOR_16                                     RF_FSM_LO_VECTOR_16
#define RF_FSM_LO_VECTOR_16_POS                                 (16U)
#define RF_FSM_LO_VECTOR_16_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_16_MSK                                 (((1U<<RF_FSM_LO_VECTOR_16_LEN)-1)<<RF_FSM_LO_VECTOR_16_POS)
#define RF_FSM_LO_VECTOR_16_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_16_LEN)-1)<<RF_FSM_LO_VECTOR_16_POS))
#define RF_FSM_LO_VECTOR_17                                     RF_FSM_LO_VECTOR_17
#define RF_FSM_LO_VECTOR_17_POS                                 (17U)
#define RF_FSM_LO_VECTOR_17_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_17_MSK                                 (((1U<<RF_FSM_LO_VECTOR_17_LEN)-1)<<RF_FSM_LO_VECTOR_17_POS)
#define RF_FSM_LO_VECTOR_17_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_17_LEN)-1)<<RF_FSM_LO_VECTOR_17_POS))
#define RF_FSM_LO_VECTOR_18                                     RF_FSM_LO_VECTOR_18
#define RF_FSM_LO_VECTOR_18_POS                                 (18U)
#define RF_FSM_LO_VECTOR_18_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_18_MSK                                 (((1U<<RF_FSM_LO_VECTOR_18_LEN)-1)<<RF_FSM_LO_VECTOR_18_POS)
#define RF_FSM_LO_VECTOR_18_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_18_LEN)-1)<<RF_FSM_LO_VECTOR_18_POS))
#define RF_FSM_LO_VECTOR_19                                     RF_FSM_LO_VECTOR_19
#define RF_FSM_LO_VECTOR_19_POS                                 (19U)
#define RF_FSM_LO_VECTOR_19_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_19_MSK                                 (((1U<<RF_FSM_LO_VECTOR_19_LEN)-1)<<RF_FSM_LO_VECTOR_19_POS)
#define RF_FSM_LO_VECTOR_19_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_19_LEN)-1)<<RF_FSM_LO_VECTOR_19_POS))
#define RF_FSM_LO_VECTOR_20                                     RF_FSM_LO_VECTOR_20
#define RF_FSM_LO_VECTOR_20_POS                                 (20U)
#define RF_FSM_LO_VECTOR_20_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_20_MSK                                 (((1U<<RF_FSM_LO_VECTOR_20_LEN)-1)<<RF_FSM_LO_VECTOR_20_POS)
#define RF_FSM_LO_VECTOR_20_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_20_LEN)-1)<<RF_FSM_LO_VECTOR_20_POS))
#define RF_FSM_LO_VECTOR_21                                     RF_FSM_LO_VECTOR_21
#define RF_FSM_LO_VECTOR_21_POS                                 (21U)
#define RF_FSM_LO_VECTOR_21_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_21_MSK                                 (((1U<<RF_FSM_LO_VECTOR_21_LEN)-1)<<RF_FSM_LO_VECTOR_21_POS)
#define RF_FSM_LO_VECTOR_21_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_21_LEN)-1)<<RF_FSM_LO_VECTOR_21_POS))
#define RF_FSM_LO_VECTOR_22                                     RF_FSM_LO_VECTOR_22
#define RF_FSM_LO_VECTOR_22_POS                                 (22U)
#define RF_FSM_LO_VECTOR_22_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_22_MSK                                 (((1U<<RF_FSM_LO_VECTOR_22_LEN)-1)<<RF_FSM_LO_VECTOR_22_POS)
#define RF_FSM_LO_VECTOR_22_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_22_LEN)-1)<<RF_FSM_LO_VECTOR_22_POS))
#define RF_FSM_LO_VECTOR_23                                     RF_FSM_LO_VECTOR_23
#define RF_FSM_LO_VECTOR_23_POS                                 (23U)
#define RF_FSM_LO_VECTOR_23_LEN                                 (1U)
#define RF_FSM_LO_VECTOR_23_MSK                                 (((1U<<RF_FSM_LO_VECTOR_23_LEN)-1)<<RF_FSM_LO_VECTOR_23_POS)
#define RF_FSM_LO_VECTOR_23_UMSK                                (~(((1U<<RF_FSM_LO_VECTOR_23_LEN)-1)<<RF_FSM_LO_VECTOR_23_POS))

/* 0x50C : rx_rf_fsm_0 */
#define RF_FSM_RX_RF_FSM_0_OFFSET                               (0x50C)
#define RF_FSM_RX_VECTOR_00                                     RF_FSM_RX_VECTOR_00
#define RF_FSM_RX_VECTOR_00_POS                                 (0U)
#define RF_FSM_RX_VECTOR_00_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_00_MSK                                 (((1U<<RF_FSM_RX_VECTOR_00_LEN)-1)<<RF_FSM_RX_VECTOR_00_POS)
#define RF_FSM_RX_VECTOR_00_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_00_LEN)-1)<<RF_FSM_RX_VECTOR_00_POS))
#define RF_FSM_RX_VECTOR_01                                     RF_FSM_RX_VECTOR_01
#define RF_FSM_RX_VECTOR_01_POS                                 (1U)
#define RF_FSM_RX_VECTOR_01_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_01_MSK                                 (((1U<<RF_FSM_RX_VECTOR_01_LEN)-1)<<RF_FSM_RX_VECTOR_01_POS)
#define RF_FSM_RX_VECTOR_01_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_01_LEN)-1)<<RF_FSM_RX_VECTOR_01_POS))
#define RF_FSM_RX_VECTOR_02                                     RF_FSM_RX_VECTOR_02
#define RF_FSM_RX_VECTOR_02_POS                                 (2U)
#define RF_FSM_RX_VECTOR_02_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_02_MSK                                 (((1U<<RF_FSM_RX_VECTOR_02_LEN)-1)<<RF_FSM_RX_VECTOR_02_POS)
#define RF_FSM_RX_VECTOR_02_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_02_LEN)-1)<<RF_FSM_RX_VECTOR_02_POS))
#define RF_FSM_RX_VECTOR_03                                     RF_FSM_RX_VECTOR_03
#define RF_FSM_RX_VECTOR_03_POS                                 (3U)
#define RF_FSM_RX_VECTOR_03_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_03_MSK                                 (((1U<<RF_FSM_RX_VECTOR_03_LEN)-1)<<RF_FSM_RX_VECTOR_03_POS)
#define RF_FSM_RX_VECTOR_03_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_03_LEN)-1)<<RF_FSM_RX_VECTOR_03_POS))
#define RF_FSM_RX_VECTOR_04                                     RF_FSM_RX_VECTOR_04
#define RF_FSM_RX_VECTOR_04_POS                                 (4U)
#define RF_FSM_RX_VECTOR_04_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_04_MSK                                 (((1U<<RF_FSM_RX_VECTOR_04_LEN)-1)<<RF_FSM_RX_VECTOR_04_POS)
#define RF_FSM_RX_VECTOR_04_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_04_LEN)-1)<<RF_FSM_RX_VECTOR_04_POS))
#define RF_FSM_RX_VECTOR_05                                     RF_FSM_RX_VECTOR_05
#define RF_FSM_RX_VECTOR_05_POS                                 (5U)
#define RF_FSM_RX_VECTOR_05_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_05_MSK                                 (((1U<<RF_FSM_RX_VECTOR_05_LEN)-1)<<RF_FSM_RX_VECTOR_05_POS)
#define RF_FSM_RX_VECTOR_05_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_05_LEN)-1)<<RF_FSM_RX_VECTOR_05_POS))
#define RF_FSM_RX_VECTOR_06                                     RF_FSM_RX_VECTOR_06
#define RF_FSM_RX_VECTOR_06_POS                                 (6U)
#define RF_FSM_RX_VECTOR_06_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_06_MSK                                 (((1U<<RF_FSM_RX_VECTOR_06_LEN)-1)<<RF_FSM_RX_VECTOR_06_POS)
#define RF_FSM_RX_VECTOR_06_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_06_LEN)-1)<<RF_FSM_RX_VECTOR_06_POS))
#define RF_FSM_RX_VECTOR_07                                     RF_FSM_RX_VECTOR_07
#define RF_FSM_RX_VECTOR_07_POS                                 (7U)
#define RF_FSM_RX_VECTOR_07_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_07_MSK                                 (((1U<<RF_FSM_RX_VECTOR_07_LEN)-1)<<RF_FSM_RX_VECTOR_07_POS)
#define RF_FSM_RX_VECTOR_07_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_07_LEN)-1)<<RF_FSM_RX_VECTOR_07_POS))
#define RF_FSM_RX_VECTOR_08                                     RF_FSM_RX_VECTOR_08
#define RF_FSM_RX_VECTOR_08_POS                                 (8U)
#define RF_FSM_RX_VECTOR_08_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_08_MSK                                 (((1U<<RF_FSM_RX_VECTOR_08_LEN)-1)<<RF_FSM_RX_VECTOR_08_POS)
#define RF_FSM_RX_VECTOR_08_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_08_LEN)-1)<<RF_FSM_RX_VECTOR_08_POS))
#define RF_FSM_RX_VECTOR_09                                     RF_FSM_RX_VECTOR_09
#define RF_FSM_RX_VECTOR_09_POS                                 (9U)
#define RF_FSM_RX_VECTOR_09_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_09_MSK                                 (((1U<<RF_FSM_RX_VECTOR_09_LEN)-1)<<RF_FSM_RX_VECTOR_09_POS)
#define RF_FSM_RX_VECTOR_09_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_09_LEN)-1)<<RF_FSM_RX_VECTOR_09_POS))
#define RF_FSM_RX_VECTOR_10                                     RF_FSM_RX_VECTOR_10
#define RF_FSM_RX_VECTOR_10_POS                                 (10U)
#define RF_FSM_RX_VECTOR_10_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_10_MSK                                 (((1U<<RF_FSM_RX_VECTOR_10_LEN)-1)<<RF_FSM_RX_VECTOR_10_POS)
#define RF_FSM_RX_VECTOR_10_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_10_LEN)-1)<<RF_FSM_RX_VECTOR_10_POS))
#define RF_FSM_RX_VECTOR_11                                     RF_FSM_RX_VECTOR_11
#define RF_FSM_RX_VECTOR_11_POS                                 (11U)
#define RF_FSM_RX_VECTOR_11_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_11_MSK                                 (((1U<<RF_FSM_RX_VECTOR_11_LEN)-1)<<RF_FSM_RX_VECTOR_11_POS)
#define RF_FSM_RX_VECTOR_11_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_11_LEN)-1)<<RF_FSM_RX_VECTOR_11_POS))
#define RF_FSM_RX_VECTOR_12                                     RF_FSM_RX_VECTOR_12
#define RF_FSM_RX_VECTOR_12_POS                                 (12U)
#define RF_FSM_RX_VECTOR_12_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_12_MSK                                 (((1U<<RF_FSM_RX_VECTOR_12_LEN)-1)<<RF_FSM_RX_VECTOR_12_POS)
#define RF_FSM_RX_VECTOR_12_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_12_LEN)-1)<<RF_FSM_RX_VECTOR_12_POS))
#define RF_FSM_RX_VECTOR_13                                     RF_FSM_RX_VECTOR_13
#define RF_FSM_RX_VECTOR_13_POS                                 (13U)
#define RF_FSM_RX_VECTOR_13_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_13_MSK                                 (((1U<<RF_FSM_RX_VECTOR_13_LEN)-1)<<RF_FSM_RX_VECTOR_13_POS)
#define RF_FSM_RX_VECTOR_13_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_13_LEN)-1)<<RF_FSM_RX_VECTOR_13_POS))
#define RF_FSM_RX_VECTOR_14                                     RF_FSM_RX_VECTOR_14
#define RF_FSM_RX_VECTOR_14_POS                                 (14U)
#define RF_FSM_RX_VECTOR_14_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_14_MSK                                 (((1U<<RF_FSM_RX_VECTOR_14_LEN)-1)<<RF_FSM_RX_VECTOR_14_POS)
#define RF_FSM_RX_VECTOR_14_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_14_LEN)-1)<<RF_FSM_RX_VECTOR_14_POS))
#define RF_FSM_RX_VECTOR_15                                     RF_FSM_RX_VECTOR_15
#define RF_FSM_RX_VECTOR_15_POS                                 (15U)
#define RF_FSM_RX_VECTOR_15_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_15_MSK                                 (((1U<<RF_FSM_RX_VECTOR_15_LEN)-1)<<RF_FSM_RX_VECTOR_15_POS)
#define RF_FSM_RX_VECTOR_15_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_15_LEN)-1)<<RF_FSM_RX_VECTOR_15_POS))
#define RF_FSM_RX_VECTOR_16                                     RF_FSM_RX_VECTOR_16
#define RF_FSM_RX_VECTOR_16_POS                                 (16U)
#define RF_FSM_RX_VECTOR_16_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_16_MSK                                 (((1U<<RF_FSM_RX_VECTOR_16_LEN)-1)<<RF_FSM_RX_VECTOR_16_POS)
#define RF_FSM_RX_VECTOR_16_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_16_LEN)-1)<<RF_FSM_RX_VECTOR_16_POS))
#define RF_FSM_RX_VECTOR_17                                     RF_FSM_RX_VECTOR_17
#define RF_FSM_RX_VECTOR_17_POS                                 (17U)
#define RF_FSM_RX_VECTOR_17_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_17_MSK                                 (((1U<<RF_FSM_RX_VECTOR_17_LEN)-1)<<RF_FSM_RX_VECTOR_17_POS)
#define RF_FSM_RX_VECTOR_17_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_17_LEN)-1)<<RF_FSM_RX_VECTOR_17_POS))
#define RF_FSM_RX_VECTOR_18                                     RF_FSM_RX_VECTOR_18
#define RF_FSM_RX_VECTOR_18_POS                                 (18U)
#define RF_FSM_RX_VECTOR_18_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_18_MSK                                 (((1U<<RF_FSM_RX_VECTOR_18_LEN)-1)<<RF_FSM_RX_VECTOR_18_POS)
#define RF_FSM_RX_VECTOR_18_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_18_LEN)-1)<<RF_FSM_RX_VECTOR_18_POS))
#define RF_FSM_RX_VECTOR_19                                     RF_FSM_RX_VECTOR_19
#define RF_FSM_RX_VECTOR_19_POS                                 (19U)
#define RF_FSM_RX_VECTOR_19_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_19_MSK                                 (((1U<<RF_FSM_RX_VECTOR_19_LEN)-1)<<RF_FSM_RX_VECTOR_19_POS)
#define RF_FSM_RX_VECTOR_19_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_19_LEN)-1)<<RF_FSM_RX_VECTOR_19_POS))
#define RF_FSM_RX_VECTOR_20                                     RF_FSM_RX_VECTOR_20
#define RF_FSM_RX_VECTOR_20_POS                                 (20U)
#define RF_FSM_RX_VECTOR_20_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_20_MSK                                 (((1U<<RF_FSM_RX_VECTOR_20_LEN)-1)<<RF_FSM_RX_VECTOR_20_POS)
#define RF_FSM_RX_VECTOR_20_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_20_LEN)-1)<<RF_FSM_RX_VECTOR_20_POS))
#define RF_FSM_RX_VECTOR_21                                     RF_FSM_RX_VECTOR_21
#define RF_FSM_RX_VECTOR_21_POS                                 (21U)
#define RF_FSM_RX_VECTOR_21_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_21_MSK                                 (((1U<<RF_FSM_RX_VECTOR_21_LEN)-1)<<RF_FSM_RX_VECTOR_21_POS)
#define RF_FSM_RX_VECTOR_21_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_21_LEN)-1)<<RF_FSM_RX_VECTOR_21_POS))
#define RF_FSM_RX_VECTOR_22                                     RF_FSM_RX_VECTOR_22
#define RF_FSM_RX_VECTOR_22_POS                                 (22U)
#define RF_FSM_RX_VECTOR_22_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_22_MSK                                 (((1U<<RF_FSM_RX_VECTOR_22_LEN)-1)<<RF_FSM_RX_VECTOR_22_POS)
#define RF_FSM_RX_VECTOR_22_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_22_LEN)-1)<<RF_FSM_RX_VECTOR_22_POS))
#define RF_FSM_RX_VECTOR_23                                     RF_FSM_RX_VECTOR_23
#define RF_FSM_RX_VECTOR_23_POS                                 (23U)
#define RF_FSM_RX_VECTOR_23_LEN                                 (1U)
#define RF_FSM_RX_VECTOR_23_MSK                                 (((1U<<RF_FSM_RX_VECTOR_23_LEN)-1)<<RF_FSM_RX_VECTOR_23_POS)
#define RF_FSM_RX_VECTOR_23_UMSK                                (~(((1U<<RF_FSM_RX_VECTOR_23_LEN)-1)<<RF_FSM_RX_VECTOR_23_POS))

/* 0x510 : tx_rf_fsm_0 */
#define RF_FSM_TX_RF_FSM_0_OFFSET                               (0x510)
#define RF_FSM_TX_VECTOR_00                                     RF_FSM_TX_VECTOR_00
#define RF_FSM_TX_VECTOR_00_POS                                 (0U)
#define RF_FSM_TX_VECTOR_00_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_00_MSK                                 (((1U<<RF_FSM_TX_VECTOR_00_LEN)-1)<<RF_FSM_TX_VECTOR_00_POS)
#define RF_FSM_TX_VECTOR_00_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_00_LEN)-1)<<RF_FSM_TX_VECTOR_00_POS))
#define RF_FSM_TX_VECTOR_01                                     RF_FSM_TX_VECTOR_01
#define RF_FSM_TX_VECTOR_01_POS                                 (1U)
#define RF_FSM_TX_VECTOR_01_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_01_MSK                                 (((1U<<RF_FSM_TX_VECTOR_01_LEN)-1)<<RF_FSM_TX_VECTOR_01_POS)
#define RF_FSM_TX_VECTOR_01_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_01_LEN)-1)<<RF_FSM_TX_VECTOR_01_POS))
#define RF_FSM_TX_VECTOR_02                                     RF_FSM_TX_VECTOR_02
#define RF_FSM_TX_VECTOR_02_POS                                 (2U)
#define RF_FSM_TX_VECTOR_02_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_02_MSK                                 (((1U<<RF_FSM_TX_VECTOR_02_LEN)-1)<<RF_FSM_TX_VECTOR_02_POS)
#define RF_FSM_TX_VECTOR_02_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_02_LEN)-1)<<RF_FSM_TX_VECTOR_02_POS))
#define RF_FSM_TX_VECTOR_03                                     RF_FSM_TX_VECTOR_03
#define RF_FSM_TX_VECTOR_03_POS                                 (3U)
#define RF_FSM_TX_VECTOR_03_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_03_MSK                                 (((1U<<RF_FSM_TX_VECTOR_03_LEN)-1)<<RF_FSM_TX_VECTOR_03_POS)
#define RF_FSM_TX_VECTOR_03_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_03_LEN)-1)<<RF_FSM_TX_VECTOR_03_POS))
#define RF_FSM_TX_VECTOR_04                                     RF_FSM_TX_VECTOR_04
#define RF_FSM_TX_VECTOR_04_POS                                 (4U)
#define RF_FSM_TX_VECTOR_04_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_04_MSK                                 (((1U<<RF_FSM_TX_VECTOR_04_LEN)-1)<<RF_FSM_TX_VECTOR_04_POS)
#define RF_FSM_TX_VECTOR_04_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_04_LEN)-1)<<RF_FSM_TX_VECTOR_04_POS))
#define RF_FSM_TX_VECTOR_05                                     RF_FSM_TX_VECTOR_05
#define RF_FSM_TX_VECTOR_05_POS                                 (5U)
#define RF_FSM_TX_VECTOR_05_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_05_MSK                                 (((1U<<RF_FSM_TX_VECTOR_05_LEN)-1)<<RF_FSM_TX_VECTOR_05_POS)
#define RF_FSM_TX_VECTOR_05_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_05_LEN)-1)<<RF_FSM_TX_VECTOR_05_POS))
#define RF_FSM_TX_VECTOR_06                                     RF_FSM_TX_VECTOR_06
#define RF_FSM_TX_VECTOR_06_POS                                 (6U)
#define RF_FSM_TX_VECTOR_06_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_06_MSK                                 (((1U<<RF_FSM_TX_VECTOR_06_LEN)-1)<<RF_FSM_TX_VECTOR_06_POS)
#define RF_FSM_TX_VECTOR_06_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_06_LEN)-1)<<RF_FSM_TX_VECTOR_06_POS))
#define RF_FSM_TX_VECTOR_07                                     RF_FSM_TX_VECTOR_07
#define RF_FSM_TX_VECTOR_07_POS                                 (7U)
#define RF_FSM_TX_VECTOR_07_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_07_MSK                                 (((1U<<RF_FSM_TX_VECTOR_07_LEN)-1)<<RF_FSM_TX_VECTOR_07_POS)
#define RF_FSM_TX_VECTOR_07_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_07_LEN)-1)<<RF_FSM_TX_VECTOR_07_POS))
#define RF_FSM_TX_VECTOR_08                                     RF_FSM_TX_VECTOR_08
#define RF_FSM_TX_VECTOR_08_POS                                 (8U)
#define RF_FSM_TX_VECTOR_08_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_08_MSK                                 (((1U<<RF_FSM_TX_VECTOR_08_LEN)-1)<<RF_FSM_TX_VECTOR_08_POS)
#define RF_FSM_TX_VECTOR_08_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_08_LEN)-1)<<RF_FSM_TX_VECTOR_08_POS))
#define RF_FSM_TX_VECTOR_09                                     RF_FSM_TX_VECTOR_09
#define RF_FSM_TX_VECTOR_09_POS                                 (9U)
#define RF_FSM_TX_VECTOR_09_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_09_MSK                                 (((1U<<RF_FSM_TX_VECTOR_09_LEN)-1)<<RF_FSM_TX_VECTOR_09_POS)
#define RF_FSM_TX_VECTOR_09_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_09_LEN)-1)<<RF_FSM_TX_VECTOR_09_POS))
#define RF_FSM_TX_VECTOR_10                                     RF_FSM_TX_VECTOR_10
#define RF_FSM_TX_VECTOR_10_POS                                 (10U)
#define RF_FSM_TX_VECTOR_10_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_10_MSK                                 (((1U<<RF_FSM_TX_VECTOR_10_LEN)-1)<<RF_FSM_TX_VECTOR_10_POS)
#define RF_FSM_TX_VECTOR_10_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_10_LEN)-1)<<RF_FSM_TX_VECTOR_10_POS))
#define RF_FSM_TX_VECTOR_11                                     RF_FSM_TX_VECTOR_11
#define RF_FSM_TX_VECTOR_11_POS                                 (11U)
#define RF_FSM_TX_VECTOR_11_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_11_MSK                                 (((1U<<RF_FSM_TX_VECTOR_11_LEN)-1)<<RF_FSM_TX_VECTOR_11_POS)
#define RF_FSM_TX_VECTOR_11_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_11_LEN)-1)<<RF_FSM_TX_VECTOR_11_POS))
#define RF_FSM_TX_VECTOR_12                                     RF_FSM_TX_VECTOR_12
#define RF_FSM_TX_VECTOR_12_POS                                 (12U)
#define RF_FSM_TX_VECTOR_12_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_12_MSK                                 (((1U<<RF_FSM_TX_VECTOR_12_LEN)-1)<<RF_FSM_TX_VECTOR_12_POS)
#define RF_FSM_TX_VECTOR_12_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_12_LEN)-1)<<RF_FSM_TX_VECTOR_12_POS))
#define RF_FSM_TX_VECTOR_13                                     RF_FSM_TX_VECTOR_13
#define RF_FSM_TX_VECTOR_13_POS                                 (13U)
#define RF_FSM_TX_VECTOR_13_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_13_MSK                                 (((1U<<RF_FSM_TX_VECTOR_13_LEN)-1)<<RF_FSM_TX_VECTOR_13_POS)
#define RF_FSM_TX_VECTOR_13_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_13_LEN)-1)<<RF_FSM_TX_VECTOR_13_POS))
#define RF_FSM_TX_VECTOR_14                                     RF_FSM_TX_VECTOR_14
#define RF_FSM_TX_VECTOR_14_POS                                 (14U)
#define RF_FSM_TX_VECTOR_14_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_14_MSK                                 (((1U<<RF_FSM_TX_VECTOR_14_LEN)-1)<<RF_FSM_TX_VECTOR_14_POS)
#define RF_FSM_TX_VECTOR_14_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_14_LEN)-1)<<RF_FSM_TX_VECTOR_14_POS))
#define RF_FSM_TX_VECTOR_15                                     RF_FSM_TX_VECTOR_15
#define RF_FSM_TX_VECTOR_15_POS                                 (15U)
#define RF_FSM_TX_VECTOR_15_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_15_MSK                                 (((1U<<RF_FSM_TX_VECTOR_15_LEN)-1)<<RF_FSM_TX_VECTOR_15_POS)
#define RF_FSM_TX_VECTOR_15_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_15_LEN)-1)<<RF_FSM_TX_VECTOR_15_POS))
#define RF_FSM_TX_VECTOR_16                                     RF_FSM_TX_VECTOR_16
#define RF_FSM_TX_VECTOR_16_POS                                 (16U)
#define RF_FSM_TX_VECTOR_16_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_16_MSK                                 (((1U<<RF_FSM_TX_VECTOR_16_LEN)-1)<<RF_FSM_TX_VECTOR_16_POS)
#define RF_FSM_TX_VECTOR_16_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_16_LEN)-1)<<RF_FSM_TX_VECTOR_16_POS))
#define RF_FSM_TX_VECTOR_17                                     RF_FSM_TX_VECTOR_17
#define RF_FSM_TX_VECTOR_17_POS                                 (17U)
#define RF_FSM_TX_VECTOR_17_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_17_MSK                                 (((1U<<RF_FSM_TX_VECTOR_17_LEN)-1)<<RF_FSM_TX_VECTOR_17_POS)
#define RF_FSM_TX_VECTOR_17_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_17_LEN)-1)<<RF_FSM_TX_VECTOR_17_POS))
#define RF_FSM_TX_VECTOR_18                                     RF_FSM_TX_VECTOR_18
#define RF_FSM_TX_VECTOR_18_POS                                 (18U)
#define RF_FSM_TX_VECTOR_18_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_18_MSK                                 (((1U<<RF_FSM_TX_VECTOR_18_LEN)-1)<<RF_FSM_TX_VECTOR_18_POS)
#define RF_FSM_TX_VECTOR_18_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_18_LEN)-1)<<RF_FSM_TX_VECTOR_18_POS))
#define RF_FSM_TX_VECTOR_19                                     RF_FSM_TX_VECTOR_19
#define RF_FSM_TX_VECTOR_19_POS                                 (19U)
#define RF_FSM_TX_VECTOR_19_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_19_MSK                                 (((1U<<RF_FSM_TX_VECTOR_19_LEN)-1)<<RF_FSM_TX_VECTOR_19_POS)
#define RF_FSM_TX_VECTOR_19_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_19_LEN)-1)<<RF_FSM_TX_VECTOR_19_POS))
#define RF_FSM_TX_VECTOR_20                                     RF_FSM_TX_VECTOR_20
#define RF_FSM_TX_VECTOR_20_POS                                 (20U)
#define RF_FSM_TX_VECTOR_20_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_20_MSK                                 (((1U<<RF_FSM_TX_VECTOR_20_LEN)-1)<<RF_FSM_TX_VECTOR_20_POS)
#define RF_FSM_TX_VECTOR_20_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_20_LEN)-1)<<RF_FSM_TX_VECTOR_20_POS))
#define RF_FSM_TX_VECTOR_21                                     RF_FSM_TX_VECTOR_21
#define RF_FSM_TX_VECTOR_21_POS                                 (21U)
#define RF_FSM_TX_VECTOR_21_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_21_MSK                                 (((1U<<RF_FSM_TX_VECTOR_21_LEN)-1)<<RF_FSM_TX_VECTOR_21_POS)
#define RF_FSM_TX_VECTOR_21_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_21_LEN)-1)<<RF_FSM_TX_VECTOR_21_POS))
#define RF_FSM_TX_VECTOR_22                                     RF_FSM_TX_VECTOR_22
#define RF_FSM_TX_VECTOR_22_POS                                 (22U)
#define RF_FSM_TX_VECTOR_22_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_22_MSK                                 (((1U<<RF_FSM_TX_VECTOR_22_LEN)-1)<<RF_FSM_TX_VECTOR_22_POS)
#define RF_FSM_TX_VECTOR_22_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_22_LEN)-1)<<RF_FSM_TX_VECTOR_22_POS))
#define RF_FSM_TX_VECTOR_23                                     RF_FSM_TX_VECTOR_23
#define RF_FSM_TX_VECTOR_23_POS                                 (23U)
#define RF_FSM_TX_VECTOR_23_LEN                                 (1U)
#define RF_FSM_TX_VECTOR_23_MSK                                 (((1U<<RF_FSM_TX_VECTOR_23_LEN)-1)<<RF_FSM_TX_VECTOR_23_POS)
#define RF_FSM_TX_VECTOR_23_UMSK                                (~(((1U<<RF_FSM_TX_VECTOR_23_LEN)-1)<<RF_FSM_TX_VECTOR_23_POS))

/* 0x514 : t2r_iros_rf_fsm_0 */
#define RF_FSM_T2R_IROS_RF_FSM_0_OFFSET                         (0x514)
#define RF_FSM_T2R_IROS_VECTOR_00                               RF_FSM_T2R_IROS_VECTOR_00
#define RF_FSM_T2R_IROS_VECTOR_00_POS                           (0U)
#define RF_FSM_T2R_IROS_VECTOR_00_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_00_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_00_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_00_POS)
#define RF_FSM_T2R_IROS_VECTOR_00_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_00_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_00_POS))
#define RF_FSM_T2R_IROS_VECTOR_01                               RF_FSM_T2R_IROS_VECTOR_01
#define RF_FSM_T2R_IROS_VECTOR_01_POS                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_01_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_01_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_01_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_01_POS)
#define RF_FSM_T2R_IROS_VECTOR_01_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_01_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_01_POS))
#define RF_FSM_T2R_IROS_VECTOR_02                               RF_FSM_T2R_IROS_VECTOR_02
#define RF_FSM_T2R_IROS_VECTOR_02_POS                           (2U)
#define RF_FSM_T2R_IROS_VECTOR_02_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_02_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_02_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_02_POS)
#define RF_FSM_T2R_IROS_VECTOR_02_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_02_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_02_POS))
#define RF_FSM_T2R_IROS_VECTOR_03                               RF_FSM_T2R_IROS_VECTOR_03
#define RF_FSM_T2R_IROS_VECTOR_03_POS                           (3U)
#define RF_FSM_T2R_IROS_VECTOR_03_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_03_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_03_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_03_POS)
#define RF_FSM_T2R_IROS_VECTOR_03_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_03_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_03_POS))
#define RF_FSM_T2R_IROS_VECTOR_04                               RF_FSM_T2R_IROS_VECTOR_04
#define RF_FSM_T2R_IROS_VECTOR_04_POS                           (4U)
#define RF_FSM_T2R_IROS_VECTOR_04_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_04_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_04_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_04_POS)
#define RF_FSM_T2R_IROS_VECTOR_04_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_04_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_04_POS))
#define RF_FSM_T2R_IROS_VECTOR_05                               RF_FSM_T2R_IROS_VECTOR_05
#define RF_FSM_T2R_IROS_VECTOR_05_POS                           (5U)
#define RF_FSM_T2R_IROS_VECTOR_05_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_05_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_05_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_05_POS)
#define RF_FSM_T2R_IROS_VECTOR_05_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_05_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_05_POS))
#define RF_FSM_T2R_IROS_VECTOR_06                               RF_FSM_T2R_IROS_VECTOR_06
#define RF_FSM_T2R_IROS_VECTOR_06_POS                           (6U)
#define RF_FSM_T2R_IROS_VECTOR_06_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_06_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_06_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_06_POS)
#define RF_FSM_T2R_IROS_VECTOR_06_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_06_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_06_POS))
#define RF_FSM_T2R_IROS_VECTOR_07                               RF_FSM_T2R_IROS_VECTOR_07
#define RF_FSM_T2R_IROS_VECTOR_07_POS                           (7U)
#define RF_FSM_T2R_IROS_VECTOR_07_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_07_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_07_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_07_POS)
#define RF_FSM_T2R_IROS_VECTOR_07_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_07_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_07_POS))
#define RF_FSM_T2R_IROS_VECTOR_08                               RF_FSM_T2R_IROS_VECTOR_08
#define RF_FSM_T2R_IROS_VECTOR_08_POS                           (8U)
#define RF_FSM_T2R_IROS_VECTOR_08_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_08_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_08_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_08_POS)
#define RF_FSM_T2R_IROS_VECTOR_08_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_08_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_08_POS))
#define RF_FSM_T2R_IROS_VECTOR_09                               RF_FSM_T2R_IROS_VECTOR_09
#define RF_FSM_T2R_IROS_VECTOR_09_POS                           (9U)
#define RF_FSM_T2R_IROS_VECTOR_09_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_09_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_09_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_09_POS)
#define RF_FSM_T2R_IROS_VECTOR_09_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_09_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_09_POS))
#define RF_FSM_T2R_IROS_VECTOR_10                               RF_FSM_T2R_IROS_VECTOR_10
#define RF_FSM_T2R_IROS_VECTOR_10_POS                           (10U)
#define RF_FSM_T2R_IROS_VECTOR_10_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_10_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_10_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_10_POS)
#define RF_FSM_T2R_IROS_VECTOR_10_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_10_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_10_POS))
#define RF_FSM_T2R_IROS_VECTOR_11                               RF_FSM_T2R_IROS_VECTOR_11
#define RF_FSM_T2R_IROS_VECTOR_11_POS                           (11U)
#define RF_FSM_T2R_IROS_VECTOR_11_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_11_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_11_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_11_POS)
#define RF_FSM_T2R_IROS_VECTOR_11_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_11_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_11_POS))
#define RF_FSM_T2R_IROS_VECTOR_12                               RF_FSM_T2R_IROS_VECTOR_12
#define RF_FSM_T2R_IROS_VECTOR_12_POS                           (12U)
#define RF_FSM_T2R_IROS_VECTOR_12_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_12_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_12_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_12_POS)
#define RF_FSM_T2R_IROS_VECTOR_12_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_12_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_12_POS))
#define RF_FSM_T2R_IROS_VECTOR_13                               RF_FSM_T2R_IROS_VECTOR_13
#define RF_FSM_T2R_IROS_VECTOR_13_POS                           (13U)
#define RF_FSM_T2R_IROS_VECTOR_13_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_13_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_13_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_13_POS)
#define RF_FSM_T2R_IROS_VECTOR_13_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_13_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_13_POS))
#define RF_FSM_T2R_IROS_VECTOR_14                               RF_FSM_T2R_IROS_VECTOR_14
#define RF_FSM_T2R_IROS_VECTOR_14_POS                           (14U)
#define RF_FSM_T2R_IROS_VECTOR_14_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_14_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_14_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_14_POS)
#define RF_FSM_T2R_IROS_VECTOR_14_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_14_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_14_POS))
#define RF_FSM_T2R_IROS_VECTOR_15                               RF_FSM_T2R_IROS_VECTOR_15
#define RF_FSM_T2R_IROS_VECTOR_15_POS                           (15U)
#define RF_FSM_T2R_IROS_VECTOR_15_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_15_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_15_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_15_POS)
#define RF_FSM_T2R_IROS_VECTOR_15_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_15_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_15_POS))
#define RF_FSM_T2R_IROS_VECTOR_16                               RF_FSM_T2R_IROS_VECTOR_16
#define RF_FSM_T2R_IROS_VECTOR_16_POS                           (16U)
#define RF_FSM_T2R_IROS_VECTOR_16_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_16_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_16_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_16_POS)
#define RF_FSM_T2R_IROS_VECTOR_16_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_16_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_16_POS))
#define RF_FSM_T2R_IROS_VECTOR_17                               RF_FSM_T2R_IROS_VECTOR_17
#define RF_FSM_T2R_IROS_VECTOR_17_POS                           (17U)
#define RF_FSM_T2R_IROS_VECTOR_17_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_17_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_17_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_17_POS)
#define RF_FSM_T2R_IROS_VECTOR_17_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_17_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_17_POS))
#define RF_FSM_T2R_IROS_VECTOR_18                               RF_FSM_T2R_IROS_VECTOR_18
#define RF_FSM_T2R_IROS_VECTOR_18_POS                           (18U)
#define RF_FSM_T2R_IROS_VECTOR_18_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_18_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_18_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_18_POS)
#define RF_FSM_T2R_IROS_VECTOR_18_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_18_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_18_POS))
#define RF_FSM_T2R_IROS_VECTOR_19                               RF_FSM_T2R_IROS_VECTOR_19
#define RF_FSM_T2R_IROS_VECTOR_19_POS                           (19U)
#define RF_FSM_T2R_IROS_VECTOR_19_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_19_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_19_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_19_POS)
#define RF_FSM_T2R_IROS_VECTOR_19_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_19_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_19_POS))
#define RF_FSM_T2R_IROS_VECTOR_20                               RF_FSM_T2R_IROS_VECTOR_20
#define RF_FSM_T2R_IROS_VECTOR_20_POS                           (20U)
#define RF_FSM_T2R_IROS_VECTOR_20_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_20_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_20_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_20_POS)
#define RF_FSM_T2R_IROS_VECTOR_20_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_20_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_20_POS))
#define RF_FSM_T2R_IROS_VECTOR_21                               RF_FSM_T2R_IROS_VECTOR_21
#define RF_FSM_T2R_IROS_VECTOR_21_POS                           (21U)
#define RF_FSM_T2R_IROS_VECTOR_21_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_21_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_21_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_21_POS)
#define RF_FSM_T2R_IROS_VECTOR_21_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_21_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_21_POS))
#define RF_FSM_T2R_IROS_VECTOR_22                               RF_FSM_T2R_IROS_VECTOR_22
#define RF_FSM_T2R_IROS_VECTOR_22_POS                           (22U)
#define RF_FSM_T2R_IROS_VECTOR_22_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_22_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_22_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_22_POS)
#define RF_FSM_T2R_IROS_VECTOR_22_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_22_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_22_POS))
#define RF_FSM_T2R_IROS_VECTOR_23                               RF_FSM_T2R_IROS_VECTOR_23
#define RF_FSM_T2R_IROS_VECTOR_23_POS                           (23U)
#define RF_FSM_T2R_IROS_VECTOR_23_LEN                           (1U)
#define RF_FSM_T2R_IROS_VECTOR_23_MSK                           (((1U<<RF_FSM_T2R_IROS_VECTOR_23_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_23_POS)
#define RF_FSM_T2R_IROS_VECTOR_23_UMSK                          (~(((1U<<RF_FSM_T2R_IROS_VECTOR_23_LEN)-1)<<RF_FSM_T2R_IROS_VECTOR_23_POS))

/* 0x518 : r2t_rf_fsm_0 */
#define RF_FSM_R2T_RF_FSM_0_OFFSET                              (0x518)
#define RF_FSM_R2T_VECTOR_00                                    RF_FSM_R2T_VECTOR_00
#define RF_FSM_R2T_VECTOR_00_POS                                (0U)
#define RF_FSM_R2T_VECTOR_00_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_00_MSK                                (((1U<<RF_FSM_R2T_VECTOR_00_LEN)-1)<<RF_FSM_R2T_VECTOR_00_POS)
#define RF_FSM_R2T_VECTOR_00_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_00_LEN)-1)<<RF_FSM_R2T_VECTOR_00_POS))
#define RF_FSM_R2T_VECTOR_01                                    RF_FSM_R2T_VECTOR_01
#define RF_FSM_R2T_VECTOR_01_POS                                (1U)
#define RF_FSM_R2T_VECTOR_01_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_01_MSK                                (((1U<<RF_FSM_R2T_VECTOR_01_LEN)-1)<<RF_FSM_R2T_VECTOR_01_POS)
#define RF_FSM_R2T_VECTOR_01_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_01_LEN)-1)<<RF_FSM_R2T_VECTOR_01_POS))
#define RF_FSM_R2T_VECTOR_02                                    RF_FSM_R2T_VECTOR_02
#define RF_FSM_R2T_VECTOR_02_POS                                (2U)
#define RF_FSM_R2T_VECTOR_02_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_02_MSK                                (((1U<<RF_FSM_R2T_VECTOR_02_LEN)-1)<<RF_FSM_R2T_VECTOR_02_POS)
#define RF_FSM_R2T_VECTOR_02_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_02_LEN)-1)<<RF_FSM_R2T_VECTOR_02_POS))
#define RF_FSM_R2T_VECTOR_03                                    RF_FSM_R2T_VECTOR_03
#define RF_FSM_R2T_VECTOR_03_POS                                (3U)
#define RF_FSM_R2T_VECTOR_03_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_03_MSK                                (((1U<<RF_FSM_R2T_VECTOR_03_LEN)-1)<<RF_FSM_R2T_VECTOR_03_POS)
#define RF_FSM_R2T_VECTOR_03_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_03_LEN)-1)<<RF_FSM_R2T_VECTOR_03_POS))
#define RF_FSM_R2T_VECTOR_04                                    RF_FSM_R2T_VECTOR_04
#define RF_FSM_R2T_VECTOR_04_POS                                (4U)
#define RF_FSM_R2T_VECTOR_04_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_04_MSK                                (((1U<<RF_FSM_R2T_VECTOR_04_LEN)-1)<<RF_FSM_R2T_VECTOR_04_POS)
#define RF_FSM_R2T_VECTOR_04_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_04_LEN)-1)<<RF_FSM_R2T_VECTOR_04_POS))
#define RF_FSM_R2T_VECTOR_05                                    RF_FSM_R2T_VECTOR_05
#define RF_FSM_R2T_VECTOR_05_POS                                (5U)
#define RF_FSM_R2T_VECTOR_05_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_05_MSK                                (((1U<<RF_FSM_R2T_VECTOR_05_LEN)-1)<<RF_FSM_R2T_VECTOR_05_POS)
#define RF_FSM_R2T_VECTOR_05_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_05_LEN)-1)<<RF_FSM_R2T_VECTOR_05_POS))
#define RF_FSM_R2T_VECTOR_06                                    RF_FSM_R2T_VECTOR_06
#define RF_FSM_R2T_VECTOR_06_POS                                (6U)
#define RF_FSM_R2T_VECTOR_06_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_06_MSK                                (((1U<<RF_FSM_R2T_VECTOR_06_LEN)-1)<<RF_FSM_R2T_VECTOR_06_POS)
#define RF_FSM_R2T_VECTOR_06_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_06_LEN)-1)<<RF_FSM_R2T_VECTOR_06_POS))
#define RF_FSM_R2T_VECTOR_07                                    RF_FSM_R2T_VECTOR_07
#define RF_FSM_R2T_VECTOR_07_POS                                (7U)
#define RF_FSM_R2T_VECTOR_07_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_07_MSK                                (((1U<<RF_FSM_R2T_VECTOR_07_LEN)-1)<<RF_FSM_R2T_VECTOR_07_POS)
#define RF_FSM_R2T_VECTOR_07_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_07_LEN)-1)<<RF_FSM_R2T_VECTOR_07_POS))
#define RF_FSM_R2T_VECTOR_08                                    RF_FSM_R2T_VECTOR_08
#define RF_FSM_R2T_VECTOR_08_POS                                (8U)
#define RF_FSM_R2T_VECTOR_08_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_08_MSK                                (((1U<<RF_FSM_R2T_VECTOR_08_LEN)-1)<<RF_FSM_R2T_VECTOR_08_POS)
#define RF_FSM_R2T_VECTOR_08_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_08_LEN)-1)<<RF_FSM_R2T_VECTOR_08_POS))
#define RF_FSM_R2T_VECTOR_09                                    RF_FSM_R2T_VECTOR_09
#define RF_FSM_R2T_VECTOR_09_POS                                (9U)
#define RF_FSM_R2T_VECTOR_09_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_09_MSK                                (((1U<<RF_FSM_R2T_VECTOR_09_LEN)-1)<<RF_FSM_R2T_VECTOR_09_POS)
#define RF_FSM_R2T_VECTOR_09_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_09_LEN)-1)<<RF_FSM_R2T_VECTOR_09_POS))
#define RF_FSM_R2T_VECTOR_10                                    RF_FSM_R2T_VECTOR_10
#define RF_FSM_R2T_VECTOR_10_POS                                (10U)
#define RF_FSM_R2T_VECTOR_10_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_10_MSK                                (((1U<<RF_FSM_R2T_VECTOR_10_LEN)-1)<<RF_FSM_R2T_VECTOR_10_POS)
#define RF_FSM_R2T_VECTOR_10_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_10_LEN)-1)<<RF_FSM_R2T_VECTOR_10_POS))
#define RF_FSM_R2T_VECTOR_11                                    RF_FSM_R2T_VECTOR_11
#define RF_FSM_R2T_VECTOR_11_POS                                (11U)
#define RF_FSM_R2T_VECTOR_11_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_11_MSK                                (((1U<<RF_FSM_R2T_VECTOR_11_LEN)-1)<<RF_FSM_R2T_VECTOR_11_POS)
#define RF_FSM_R2T_VECTOR_11_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_11_LEN)-1)<<RF_FSM_R2T_VECTOR_11_POS))
#define RF_FSM_R2T_VECTOR_12                                    RF_FSM_R2T_VECTOR_12
#define RF_FSM_R2T_VECTOR_12_POS                                (12U)
#define RF_FSM_R2T_VECTOR_12_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_12_MSK                                (((1U<<RF_FSM_R2T_VECTOR_12_LEN)-1)<<RF_FSM_R2T_VECTOR_12_POS)
#define RF_FSM_R2T_VECTOR_12_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_12_LEN)-1)<<RF_FSM_R2T_VECTOR_12_POS))
#define RF_FSM_R2T_VECTOR_13                                    RF_FSM_R2T_VECTOR_13
#define RF_FSM_R2T_VECTOR_13_POS                                (13U)
#define RF_FSM_R2T_VECTOR_13_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_13_MSK                                (((1U<<RF_FSM_R2T_VECTOR_13_LEN)-1)<<RF_FSM_R2T_VECTOR_13_POS)
#define RF_FSM_R2T_VECTOR_13_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_13_LEN)-1)<<RF_FSM_R2T_VECTOR_13_POS))
#define RF_FSM_R2T_VECTOR_14                                    RF_FSM_R2T_VECTOR_14
#define RF_FSM_R2T_VECTOR_14_POS                                (14U)
#define RF_FSM_R2T_VECTOR_14_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_14_MSK                                (((1U<<RF_FSM_R2T_VECTOR_14_LEN)-1)<<RF_FSM_R2T_VECTOR_14_POS)
#define RF_FSM_R2T_VECTOR_14_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_14_LEN)-1)<<RF_FSM_R2T_VECTOR_14_POS))
#define RF_FSM_R2T_VECTOR_15                                    RF_FSM_R2T_VECTOR_15
#define RF_FSM_R2T_VECTOR_15_POS                                (15U)
#define RF_FSM_R2T_VECTOR_15_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_15_MSK                                (((1U<<RF_FSM_R2T_VECTOR_15_LEN)-1)<<RF_FSM_R2T_VECTOR_15_POS)
#define RF_FSM_R2T_VECTOR_15_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_15_LEN)-1)<<RF_FSM_R2T_VECTOR_15_POS))
#define RF_FSM_R2T_VECTOR_16                                    RF_FSM_R2T_VECTOR_16
#define RF_FSM_R2T_VECTOR_16_POS                                (16U)
#define RF_FSM_R2T_VECTOR_16_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_16_MSK                                (((1U<<RF_FSM_R2T_VECTOR_16_LEN)-1)<<RF_FSM_R2T_VECTOR_16_POS)
#define RF_FSM_R2T_VECTOR_16_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_16_LEN)-1)<<RF_FSM_R2T_VECTOR_16_POS))
#define RF_FSM_R2T_VECTOR_17                                    RF_FSM_R2T_VECTOR_17
#define RF_FSM_R2T_VECTOR_17_POS                                (17U)
#define RF_FSM_R2T_VECTOR_17_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_17_MSK                                (((1U<<RF_FSM_R2T_VECTOR_17_LEN)-1)<<RF_FSM_R2T_VECTOR_17_POS)
#define RF_FSM_R2T_VECTOR_17_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_17_LEN)-1)<<RF_FSM_R2T_VECTOR_17_POS))
#define RF_FSM_R2T_VECTOR_18                                    RF_FSM_R2T_VECTOR_18
#define RF_FSM_R2T_VECTOR_18_POS                                (18U)
#define RF_FSM_R2T_VECTOR_18_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_18_MSK                                (((1U<<RF_FSM_R2T_VECTOR_18_LEN)-1)<<RF_FSM_R2T_VECTOR_18_POS)
#define RF_FSM_R2T_VECTOR_18_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_18_LEN)-1)<<RF_FSM_R2T_VECTOR_18_POS))
#define RF_FSM_R2T_VECTOR_19                                    RF_FSM_R2T_VECTOR_19
#define RF_FSM_R2T_VECTOR_19_POS                                (19U)
#define RF_FSM_R2T_VECTOR_19_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_19_MSK                                (((1U<<RF_FSM_R2T_VECTOR_19_LEN)-1)<<RF_FSM_R2T_VECTOR_19_POS)
#define RF_FSM_R2T_VECTOR_19_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_19_LEN)-1)<<RF_FSM_R2T_VECTOR_19_POS))
#define RF_FSM_R2T_VECTOR_20                                    RF_FSM_R2T_VECTOR_20
#define RF_FSM_R2T_VECTOR_20_POS                                (20U)
#define RF_FSM_R2T_VECTOR_20_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_20_MSK                                (((1U<<RF_FSM_R2T_VECTOR_20_LEN)-1)<<RF_FSM_R2T_VECTOR_20_POS)
#define RF_FSM_R2T_VECTOR_20_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_20_LEN)-1)<<RF_FSM_R2T_VECTOR_20_POS))
#define RF_FSM_R2T_VECTOR_21                                    RF_FSM_R2T_VECTOR_21
#define RF_FSM_R2T_VECTOR_21_POS                                (21U)
#define RF_FSM_R2T_VECTOR_21_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_21_MSK                                (((1U<<RF_FSM_R2T_VECTOR_21_LEN)-1)<<RF_FSM_R2T_VECTOR_21_POS)
#define RF_FSM_R2T_VECTOR_21_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_21_LEN)-1)<<RF_FSM_R2T_VECTOR_21_POS))
#define RF_FSM_R2T_VECTOR_22                                    RF_FSM_R2T_VECTOR_22
#define RF_FSM_R2T_VECTOR_22_POS                                (22U)
#define RF_FSM_R2T_VECTOR_22_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_22_MSK                                (((1U<<RF_FSM_R2T_VECTOR_22_LEN)-1)<<RF_FSM_R2T_VECTOR_22_POS)
#define RF_FSM_R2T_VECTOR_22_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_22_LEN)-1)<<RF_FSM_R2T_VECTOR_22_POS))
#define RF_FSM_R2T_VECTOR_23                                    RF_FSM_R2T_VECTOR_23
#define RF_FSM_R2T_VECTOR_23_POS                                (23U)
#define RF_FSM_R2T_VECTOR_23_LEN                                (1U)
#define RF_FSM_R2T_VECTOR_23_MSK                                (((1U<<RF_FSM_R2T_VECTOR_23_LEN)-1)<<RF_FSM_R2T_VECTOR_23_POS)
#define RF_FSM_R2T_VECTOR_23_UMSK                               (~(((1U<<RF_FSM_R2T_VECTOR_23_LEN)-1)<<RF_FSM_R2T_VECTOR_23_POS))


struct  rf_fsm_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[1280];

    /* 0x500 : rf_fsm_0 */
    union {
        struct {
            uint32_t vector_00                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t vector_01                      :  1; /* [    1],        r/w,        0x0 */
            uint32_t vector_02                      :  1; /* [    2],        r/w,        0x0 */
            uint32_t vector_03                      :  1; /* [    3],        r/w,        0x0 */
            uint32_t vector_04                      :  1; /* [    4],        r/w,        0x0 */
            uint32_t vector_05                      :  1; /* [    5],        r/w,        0x0 */
            uint32_t vector_06                      :  1; /* [    6],        r/w,        0x0 */
            uint32_t vector_07                      :  1; /* [    7],        r/w,        0x0 */
            uint32_t vector_08                      :  1; /* [    8],        r/w,        0x1 */
            uint32_t vector_09                      :  1; /* [    9],        r/w,        0x0 */
            uint32_t vector_10                      :  1; /* [   10],        r/w,        0x0 */
            uint32_t vector_11                      :  1; /* [   11],        r/w,        0x0 */
            uint32_t vector_12                      :  1; /* [   12],        r/w,        0x0 */
            uint32_t vector_13                      :  1; /* [   13],        r/w,        0x0 */
            uint32_t vector_14                      :  1; /* [   14],        r/w,        0x0 */
            uint32_t vector_15                      :  1; /* [   15],        r/w,        0x0 */
            uint32_t vector_16                      :  1; /* [   16],        r/w,        0x0 */
            uint32_t vector_17                      :  1; /* [   17],        r/w,        0x0 */
            uint32_t vector_18                      :  1; /* [   18],        r/w,        0x0 */
            uint32_t vector_19                      :  1; /* [   19],        r/w,        0x0 */
            uint32_t vector_20                      :  1; /* [   20],        r/w,        0x0 */
            uint32_t vector_21                      :  1; /* [   21],        r/w,        0x0 */
            uint32_t vector_22                      :  1; /* [   22],        r/w,        0x0 */
            uint32_t vector_23                      :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_0;

    /* 0x504 : sb_rf_fsm_0 */
    union {
        struct {
            uint32_t sb_vector_00                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t sb_vector_01                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t sb_vector_02                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t sb_vector_03                   :  1; /* [    3],        r/w,        0x0 */
            uint32_t sb_vector_04                   :  1; /* [    4],        r/w,        0x0 */
            uint32_t sb_vector_05                   :  1; /* [    5],        r/w,        0x0 */
            uint32_t sb_vector_06                   :  1; /* [    6],        r/w,        0x0 */
            uint32_t sb_vector_07                   :  1; /* [    7],        r/w,        0x0 */
            uint32_t sb_vector_08                   :  1; /* [    8],        r/w,        0x1 */
            uint32_t sb_vector_09                   :  1; /* [    9],        r/w,        0x1 */
            uint32_t sb_vector_10                   :  1; /* [   10],        r/w,        0x0 */
            uint32_t sb_vector_11                   :  1; /* [   11],        r/w,        0x0 */
            uint32_t sb_vector_12                   :  1; /* [   12],        r/w,        0x0 */
            uint32_t sb_vector_13                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t sb_vector_14                   :  1; /* [   14],        r/w,        0x0 */
            uint32_t sb_vector_15                   :  1; /* [   15],        r/w,        0x0 */
            uint32_t sb_vector_16                   :  1; /* [   16],        r/w,        0x0 */
            uint32_t sb_vector_17                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t sb_vector_18                   :  1; /* [   18],        r/w,        0x1 */
            uint32_t sb_vector_19                   :  1; /* [   19],        r/w,        0x1 */
            uint32_t sb_vector_20                   :  1; /* [   20],        r/w,        0x0 */
            uint32_t sb_vector_21                   :  1; /* [   21],        r/w,        0x1 */
            uint32_t sb_vector_22                   :  1; /* [   22],        r/w,        0x0 */
            uint32_t sb_vector_23                   :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sb_rf_fsm_0;

    /* 0x508 : lo_rf_fsm_0 */
    union {
        struct {
            uint32_t lo_vector_00                   :  1; /* [    0],        r/w,        0x1 */
            uint32_t lo_vector_01                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t lo_vector_02                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t lo_vector_03                   :  1; /* [    3],        r/w,        0x1 */
            uint32_t lo_vector_04                   :  1; /* [    4],        r/w,        0x1 */
            uint32_t lo_vector_05                   :  1; /* [    5],        r/w,        0x1 */
            uint32_t lo_vector_06                   :  1; /* [    6],        r/w,        0x1 */
            uint32_t lo_vector_07                   :  1; /* [    7],        r/w,        0x0 */
            uint32_t lo_vector_08                   :  1; /* [    8],        r/w,        0x1 */
            uint32_t lo_vector_09                   :  1; /* [    9],        r/w,        0x1 */
            uint32_t lo_vector_10                   :  1; /* [   10],        r/w,        0x0 */
            uint32_t lo_vector_11                   :  1; /* [   11],        r/w,        0x0 */
            uint32_t lo_vector_12                   :  1; /* [   12],        r/w,        0x0 */
            uint32_t lo_vector_13                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t lo_vector_14                   :  1; /* [   14],        r/w,        0x0 */
            uint32_t lo_vector_15                   :  1; /* [   15],        r/w,        0x0 */
            uint32_t lo_vector_16                   :  1; /* [   16],        r/w,        0x0 */
            uint32_t lo_vector_17                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t lo_vector_18                   :  1; /* [   18],        r/w,        0x1 */
            uint32_t lo_vector_19                   :  1; /* [   19],        r/w,        0x1 */
            uint32_t lo_vector_20                   :  1; /* [   20],        r/w,        0x1 */
            uint32_t lo_vector_21                   :  1; /* [   21],        r/w,        0x1 */
            uint32_t lo_vector_22                   :  1; /* [   22],        r/w,        0x1 */
            uint32_t lo_vector_23                   :  1; /* [   23],        r/w,        0x1 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_rf_fsm_0;

    /* 0x50C : rx_rf_fsm_0 */
    union {
        struct {
            uint32_t rx_vector_00                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t rx_vector_01                   :  1; /* [    1],        r/w,        0x1 */
            uint32_t rx_vector_02                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t rx_vector_03                   :  1; /* [    3],        r/w,        0x0 */
            uint32_t rx_vector_04                   :  1; /* [    4],        r/w,        0x0 */
            uint32_t rx_vector_05                   :  1; /* [    5],        r/w,        0x0 */
            uint32_t rx_vector_06                   :  1; /* [    6],        r/w,        0x0 */
            uint32_t rx_vector_07                   :  1; /* [    7],        r/w,        0x0 */
            uint32_t rx_vector_08                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t rx_vector_09                   :  1; /* [    9],        r/w,        0x0 */
            uint32_t rx_vector_10                   :  1; /* [   10],        r/w,        0x1 */
            uint32_t rx_vector_11                   :  1; /* [   11],        r/w,        0x1 */
            uint32_t rx_vector_12                   :  1; /* [   12],        r/w,        0x1 */
            uint32_t rx_vector_13                   :  1; /* [   13],        r/w,        0x1 */
            uint32_t rx_vector_14                   :  1; /* [   14],        r/w,        0x1 */
            uint32_t rx_vector_15                   :  1; /* [   15],        r/w,        0x1 */
            uint32_t rx_vector_16                   :  1; /* [   16],        r/w,        0x1 */
            uint32_t rx_vector_17                   :  1; /* [   17],        r/w,        0x1 */
            uint32_t rx_vector_18                   :  1; /* [   18],        r/w,        0x1 */
            uint32_t rx_vector_19                   :  1; /* [   19],        r/w,        0x1 */
            uint32_t rx_vector_20                   :  1; /* [   20],        r/w,        0x1 */
            uint32_t rx_vector_21                   :  1; /* [   21],        r/w,        0x1 */
            uint32_t rx_vector_22                   :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_vector_23                   :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_rf_fsm_0;

    /* 0x510 : tx_rf_fsm_0 */
    union {
        struct {
            uint32_t tx_vector_00                   :  1; /* [    0],        r/w,        0x1 */
            uint32_t tx_vector_01                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t tx_vector_02                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t tx_vector_03                   :  1; /* [    3],        r/w,        0x1 */
            uint32_t tx_vector_04                   :  1; /* [    4],        r/w,        0x1 */
            uint32_t tx_vector_05                   :  1; /* [    5],        r/w,        0x1 */
            uint32_t tx_vector_06                   :  1; /* [    6],        r/w,        0x1 */
            uint32_t tx_vector_07                   :  1; /* [    7],        r/w,        0x1 */
            uint32_t tx_vector_08                   :  1; /* [    8],        r/w,        0x1 */
            uint32_t tx_vector_09                   :  1; /* [    9],        r/w,        0x1 */
            uint32_t tx_vector_10                   :  1; /* [   10],        r/w,        0x0 */
            uint32_t tx_vector_11                   :  1; /* [   11],        r/w,        0x0 */
            uint32_t tx_vector_12                   :  1; /* [   12],        r/w,        0x0 */
            uint32_t tx_vector_13                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t tx_vector_14                   :  1; /* [   14],        r/w,        0x0 */
            uint32_t tx_vector_15                   :  1; /* [   15],        r/w,        0x0 */
            uint32_t tx_vector_16                   :  1; /* [   16],        r/w,        0x0 */
            uint32_t tx_vector_17                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t tx_vector_18                   :  1; /* [   18],        r/w,        0x1 */
            uint32_t tx_vector_19                   :  1; /* [   19],        r/w,        0x1 */
            uint32_t tx_vector_20                   :  1; /* [   20],        r/w,        0x1 */
            uint32_t tx_vector_21                   :  1; /* [   21],        r/w,        0x1 */
            uint32_t tx_vector_22                   :  1; /* [   22],        r/w,        0x1 */
            uint32_t tx_vector_23                   :  1; /* [   23],        r/w,        0x1 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_rf_fsm_0;

    /* 0x514 : t2r_iros_rf_fsm_0 */
    union {
        struct {
            uint32_t t2r_iros_vector_00             :  1; /* [    0],        r/w,        0x0 */
            uint32_t t2r_iros_vector_01             :  1; /* [    1],        r/w,        0x1 */
            uint32_t t2r_iros_vector_02             :  1; /* [    2],        r/w,        0x0 */
            uint32_t t2r_iros_vector_03             :  1; /* [    3],        r/w,        0x0 */
            uint32_t t2r_iros_vector_04             :  1; /* [    4],        r/w,        0x0 */
            uint32_t t2r_iros_vector_05             :  1; /* [    5],        r/w,        0x0 */
            uint32_t t2r_iros_vector_06             :  1; /* [    6],        r/w,        0x0 */
            uint32_t t2r_iros_vector_07             :  1; /* [    7],        r/w,        0x0 */
            uint32_t t2r_iros_vector_08             :  1; /* [    8],        r/w,        0x1 */
            uint32_t t2r_iros_vector_09             :  1; /* [    9],        r/w,        0x1 */
            uint32_t t2r_iros_vector_10             :  1; /* [   10],        r/w,        0x1 */
            uint32_t t2r_iros_vector_11             :  1; /* [   11],        r/w,        0x1 */
            uint32_t t2r_iros_vector_12             :  1; /* [   12],        r/w,        0x1 */
            uint32_t t2r_iros_vector_13             :  1; /* [   13],        r/w,        0x1 */
            uint32_t t2r_iros_vector_14             :  1; /* [   14],        r/w,        0x1 */
            uint32_t t2r_iros_vector_15             :  1; /* [   15],        r/w,        0x1 */
            uint32_t t2r_iros_vector_16             :  1; /* [   16],        r/w,        0x1 */
            uint32_t t2r_iros_vector_17             :  1; /* [   17],        r/w,        0x0 */
            uint32_t t2r_iros_vector_18             :  1; /* [   18],        r/w,        0x1 */
            uint32_t t2r_iros_vector_19             :  1; /* [   19],        r/w,        0x1 */
            uint32_t t2r_iros_vector_20             :  1; /* [   20],        r/w,        0x1 */
            uint32_t t2r_iros_vector_21             :  1; /* [   21],        r/w,        0x1 */
            uint32_t t2r_iros_vector_22             :  1; /* [   22],        r/w,        0x0 */
            uint32_t t2r_iros_vector_23             :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } t2r_iros_rf_fsm_0;

    /* 0x518 : r2t_rf_fsm_0 */
    union {
        struct {
            uint32_t r2t_vector_00                  :  1; /* [    0],        r/w,        0x1 */
            uint32_t r2t_vector_01                  :  1; /* [    1],        r/w,        0x1 */
            uint32_t r2t_vector_02                  :  1; /* [    2],        r/w,        0x0 */
            uint32_t r2t_vector_03                  :  1; /* [    3],        r/w,        0x1 */
            uint32_t r2t_vector_04                  :  1; /* [    4],        r/w,        0x1 */
            uint32_t r2t_vector_05                  :  1; /* [    5],        r/w,        0x1 */
            uint32_t r2t_vector_06                  :  1; /* [    6],        r/w,        0x1 */
            uint32_t r2t_vector_07                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t r2t_vector_08                  :  1; /* [    8],        r/w,        0x1 */
            uint32_t r2t_vector_09                  :  1; /* [    9],        r/w,        0x1 */
            uint32_t r2t_vector_10                  :  1; /* [   10],        r/w,        0x0 */
            uint32_t r2t_vector_11                  :  1; /* [   11],        r/w,        0x0 */
            uint32_t r2t_vector_12                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t r2t_vector_13                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t r2t_vector_14                  :  1; /* [   14],        r/w,        0x0 */
            uint32_t r2t_vector_15                  :  1; /* [   15],        r/w,        0x0 */
            uint32_t r2t_vector_16                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t r2t_vector_17                  :  1; /* [   17],        r/w,        0x0 */
            uint32_t r2t_vector_18                  :  1; /* [   18],        r/w,        0x1 */
            uint32_t r2t_vector_19                  :  1; /* [   19],        r/w,        0x1 */
            uint32_t r2t_vector_20                  :  1; /* [   20],        r/w,        0x1 */
            uint32_t r2t_vector_21                  :  1; /* [   21],        r/w,        0x1 */
            uint32_t r2t_vector_22                  :  1; /* [   22],        r/w,        0x1 */
            uint32_t r2t_vector_23                  :  1; /* [   23],        r/w,        0x1 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } r2t_rf_fsm_0;

};

typedef volatile struct rf_fsm_reg rf_fsm_reg_t;


#endif  /* __RF_FSM_REG_H__ */
