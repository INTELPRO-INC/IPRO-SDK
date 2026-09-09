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

#ifndef  __RF_EPA_REG_H__
#define  __RF_EPA_REG_H__

#include "ipro6.h"

/* 0x580 : rf_epa_0 */
#define RF_EPA_0_OFFSET                                         (0x580)
#define RF_EPA_VECTOR_00                                        RF_EPA_VECTOR_00
#define RF_EPA_VECTOR_00_POS                                    (0U)
#define RF_EPA_VECTOR_00_LEN                                    (1U)
#define RF_EPA_VECTOR_00_MSK                                    (((1ULL<<RF_EPA_VECTOR_00_LEN)-1)<<RF_EPA_VECTOR_00_POS)
#define RF_EPA_VECTOR_00_UMSK                                   (~(((1ULL<<RF_EPA_VECTOR_00_LEN)-1)<<RF_EPA_VECTOR_00_POS))
#define RF_EPA_VECTOR_01                                        RF_EPA_VECTOR_01
#define RF_EPA_VECTOR_01_POS                                    (1U)
#define RF_EPA_VECTOR_01_LEN                                    (1U)
#define RF_EPA_VECTOR_01_MSK                                    (((1ULL<<RF_EPA_VECTOR_01_LEN)-1)<<RF_EPA_VECTOR_01_POS)
#define RF_EPA_VECTOR_01_UMSK                                   (~(((1ULL<<RF_EPA_VECTOR_01_LEN)-1)<<RF_EPA_VECTOR_01_POS))
#define RF_EPA_VECTOR_02                                        RF_EPA_VECTOR_02
#define RF_EPA_VECTOR_02_POS                                    (2U)
#define RF_EPA_VECTOR_02_LEN                                    (1U)
#define RF_EPA_VECTOR_02_MSK                                    (((1ULL<<RF_EPA_VECTOR_02_LEN)-1)<<RF_EPA_VECTOR_02_POS)
#define RF_EPA_VECTOR_02_UMSK                                   (~(((1ULL<<RF_EPA_VECTOR_02_LEN)-1)<<RF_EPA_VECTOR_02_POS))
#define RF_EPA_VECTOR_03                                        RF_EPA_VECTOR_03
#define RF_EPA_VECTOR_03_POS                                    (3U)
#define RF_EPA_VECTOR_03_LEN                                    (1U)
#define RF_EPA_VECTOR_03_MSK                                    (((1ULL<<RF_EPA_VECTOR_03_LEN)-1)<<RF_EPA_VECTOR_03_POS)
#define RF_EPA_VECTOR_03_UMSK                                   (~(((1ULL<<RF_EPA_VECTOR_03_LEN)-1)<<RF_EPA_VECTOR_03_POS))

/* 0x584 : rx2on_rf_epa_0 */
#define RF_EPA_RX2ON_RF_EPA_0_OFFSET                            (0x584)
#define RF_EPA_RX2ON_VECTOR_00                                  RF_EPA_RX2ON_VECTOR_00
#define RF_EPA_RX2ON_VECTOR_00_POS                              (0U)
#define RF_EPA_RX2ON_VECTOR_00_LEN                              (1U)
#define RF_EPA_RX2ON_VECTOR_00_MSK                              (((1ULL<<RF_EPA_RX2ON_VECTOR_00_LEN)-1)<<RF_EPA_RX2ON_VECTOR_00_POS)
#define RF_EPA_RX2ON_VECTOR_00_UMSK                             (~(((1ULL<<RF_EPA_RX2ON_VECTOR_00_LEN)-1)<<RF_EPA_RX2ON_VECTOR_00_POS))
#define RF_EPA_RX2ON_VECTOR_01                                  RF_EPA_RX2ON_VECTOR_01
#define RF_EPA_RX2ON_VECTOR_01_POS                              (1U)
#define RF_EPA_RX2ON_VECTOR_01_LEN                              (1U)
#define RF_EPA_RX2ON_VECTOR_01_MSK                              (((1ULL<<RF_EPA_RX2ON_VECTOR_01_LEN)-1)<<RF_EPA_RX2ON_VECTOR_01_POS)
#define RF_EPA_RX2ON_VECTOR_01_UMSK                             (~(((1ULL<<RF_EPA_RX2ON_VECTOR_01_LEN)-1)<<RF_EPA_RX2ON_VECTOR_01_POS))
#define RF_EPA_RX2ON_VECTOR_02                                  RF_EPA_RX2ON_VECTOR_02
#define RF_EPA_RX2ON_VECTOR_02_POS                              (2U)
#define RF_EPA_RX2ON_VECTOR_02_LEN                              (1U)
#define RF_EPA_RX2ON_VECTOR_02_MSK                              (((1ULL<<RF_EPA_RX2ON_VECTOR_02_LEN)-1)<<RF_EPA_RX2ON_VECTOR_02_POS)
#define RF_EPA_RX2ON_VECTOR_02_UMSK                             (~(((1ULL<<RF_EPA_RX2ON_VECTOR_02_LEN)-1)<<RF_EPA_RX2ON_VECTOR_02_POS))
#define RF_EPA_RX2ON_VECTOR_03                                  RF_EPA_RX2ON_VECTOR_03
#define RF_EPA_RX2ON_VECTOR_03_POS                              (3U)
#define RF_EPA_RX2ON_VECTOR_03_LEN                              (1U)
#define RF_EPA_RX2ON_VECTOR_03_MSK                              (((1ULL<<RF_EPA_RX2ON_VECTOR_03_LEN)-1)<<RF_EPA_RX2ON_VECTOR_03_POS)
#define RF_EPA_RX2ON_VECTOR_03_UMSK                             (~(((1ULL<<RF_EPA_RX2ON_VECTOR_03_LEN)-1)<<RF_EPA_RX2ON_VECTOR_03_POS))

/* 0x588 : rxon_rf_epa_0 */
#define RF_EPA_RXON_RF_EPA_0_OFFSET                             (0x588)
#define RF_EPA_RXON_VECTOR_00                                   RF_EPA_RXON_VECTOR_00
#define RF_EPA_RXON_VECTOR_00_POS                               (0U)
#define RF_EPA_RXON_VECTOR_00_LEN                               (1U)
#define RF_EPA_RXON_VECTOR_00_MSK                               (((1ULL<<RF_EPA_RXON_VECTOR_00_LEN)-1)<<RF_EPA_RXON_VECTOR_00_POS)
#define RF_EPA_RXON_VECTOR_00_UMSK                              (~(((1ULL<<RF_EPA_RXON_VECTOR_00_LEN)-1)<<RF_EPA_RXON_VECTOR_00_POS))
#define RF_EPA_RXON_VECTOR_01                                   RF_EPA_RXON_VECTOR_01
#define RF_EPA_RXON_VECTOR_01_POS                               (1U)
#define RF_EPA_RXON_VECTOR_01_LEN                               (1U)
#define RF_EPA_RXON_VECTOR_01_MSK                               (((1ULL<<RF_EPA_RXON_VECTOR_01_LEN)-1)<<RF_EPA_RXON_VECTOR_01_POS)
#define RF_EPA_RXON_VECTOR_01_UMSK                              (~(((1ULL<<RF_EPA_RXON_VECTOR_01_LEN)-1)<<RF_EPA_RXON_VECTOR_01_POS))
#define RF_EPA_RXON_VECTOR_02                                   RF_EPA_RXON_VECTOR_02
#define RF_EPA_RXON_VECTOR_02_POS                               (2U)
#define RF_EPA_RXON_VECTOR_02_LEN                               (1U)
#define RF_EPA_RXON_VECTOR_02_MSK                               (((1ULL<<RF_EPA_RXON_VECTOR_02_LEN)-1)<<RF_EPA_RXON_VECTOR_02_POS)
#define RF_EPA_RXON_VECTOR_02_UMSK                              (~(((1ULL<<RF_EPA_RXON_VECTOR_02_LEN)-1)<<RF_EPA_RXON_VECTOR_02_POS))
#define RF_EPA_RXON_VECTOR_03                                   RF_EPA_RXON_VECTOR_03
#define RF_EPA_RXON_VECTOR_03_POS                               (3U)
#define RF_EPA_RXON_VECTOR_03_LEN                               (1U)
#define RF_EPA_RXON_VECTOR_03_MSK                               (((1ULL<<RF_EPA_RXON_VECTOR_03_LEN)-1)<<RF_EPA_RXON_VECTOR_03_POS)
#define RF_EPA_RXON_VECTOR_03_UMSK                              (~(((1ULL<<RF_EPA_RXON_VECTOR_03_LEN)-1)<<RF_EPA_RXON_VECTOR_03_POS))

/* 0x58C : tx2on_rf_epa_0 */
#define RF_EPA_TX2ON_RF_EPA_0_OFFSET                            (0x58C)
#define RF_EPA_TX2ON_VECTOR_00                                  RF_EPA_TX2ON_VECTOR_00
#define RF_EPA_TX2ON_VECTOR_00_POS                              (0U)
#define RF_EPA_TX2ON_VECTOR_00_LEN                              (1U)
#define RF_EPA_TX2ON_VECTOR_00_MSK                              (((1ULL<<RF_EPA_TX2ON_VECTOR_00_LEN)-1)<<RF_EPA_TX2ON_VECTOR_00_POS)
#define RF_EPA_TX2ON_VECTOR_00_UMSK                             (~(((1ULL<<RF_EPA_TX2ON_VECTOR_00_LEN)-1)<<RF_EPA_TX2ON_VECTOR_00_POS))
#define RF_EPA_TX2ON_VECTOR_01                                  RF_EPA_TX2ON_VECTOR_01
#define RF_EPA_TX2ON_VECTOR_01_POS                              (1U)
#define RF_EPA_TX2ON_VECTOR_01_LEN                              (1U)
#define RF_EPA_TX2ON_VECTOR_01_MSK                              (((1ULL<<RF_EPA_TX2ON_VECTOR_01_LEN)-1)<<RF_EPA_TX2ON_VECTOR_01_POS)
#define RF_EPA_TX2ON_VECTOR_01_UMSK                             (~(((1ULL<<RF_EPA_TX2ON_VECTOR_01_LEN)-1)<<RF_EPA_TX2ON_VECTOR_01_POS))
#define RF_EPA_TX2ON_VECTOR_02                                  RF_EPA_TX2ON_VECTOR_02
#define RF_EPA_TX2ON_VECTOR_02_POS                              (2U)
#define RF_EPA_TX2ON_VECTOR_02_LEN                              (1U)
#define RF_EPA_TX2ON_VECTOR_02_MSK                              (((1ULL<<RF_EPA_TX2ON_VECTOR_02_LEN)-1)<<RF_EPA_TX2ON_VECTOR_02_POS)
#define RF_EPA_TX2ON_VECTOR_02_UMSK                             (~(((1ULL<<RF_EPA_TX2ON_VECTOR_02_LEN)-1)<<RF_EPA_TX2ON_VECTOR_02_POS))
#define RF_EPA_TX2ON_VECTOR_03                                  RF_EPA_TX2ON_VECTOR_03
#define RF_EPA_TX2ON_VECTOR_03_POS                              (3U)
#define RF_EPA_TX2ON_VECTOR_03_LEN                              (1U)
#define RF_EPA_TX2ON_VECTOR_03_MSK                              (((1ULL<<RF_EPA_TX2ON_VECTOR_03_LEN)-1)<<RF_EPA_TX2ON_VECTOR_03_POS)
#define RF_EPA_TX2ON_VECTOR_03_UMSK                             (~(((1ULL<<RF_EPA_TX2ON_VECTOR_03_LEN)-1)<<RF_EPA_TX2ON_VECTOR_03_POS))

/* 0x590 : tx2paon_rf_epa_0 */
#define RF_EPA_TX2PAON_RF_EPA_0_OFFSET                          (0x590)
#define RF_EPA_TX2PAON_VECTOR_00                                RF_EPA_TX2PAON_VECTOR_00
#define RF_EPA_TX2PAON_VECTOR_00_POS                            (0U)
#define RF_EPA_TX2PAON_VECTOR_00_LEN                            (1U)
#define RF_EPA_TX2PAON_VECTOR_00_MSK                            (((1ULL<<RF_EPA_TX2PAON_VECTOR_00_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_00_POS)
#define RF_EPA_TX2PAON_VECTOR_00_UMSK                           (~(((1ULL<<RF_EPA_TX2PAON_VECTOR_00_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_00_POS))
#define RF_EPA_TX2PAON_VECTOR_01                                RF_EPA_TX2PAON_VECTOR_01
#define RF_EPA_TX2PAON_VECTOR_01_POS                            (1U)
#define RF_EPA_TX2PAON_VECTOR_01_LEN                            (1U)
#define RF_EPA_TX2PAON_VECTOR_01_MSK                            (((1ULL<<RF_EPA_TX2PAON_VECTOR_01_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_01_POS)
#define RF_EPA_TX2PAON_VECTOR_01_UMSK                           (~(((1ULL<<RF_EPA_TX2PAON_VECTOR_01_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_01_POS))
#define RF_EPA_TX2PAON_VECTOR_02                                RF_EPA_TX2PAON_VECTOR_02
#define RF_EPA_TX2PAON_VECTOR_02_POS                            (2U)
#define RF_EPA_TX2PAON_VECTOR_02_LEN                            (1U)
#define RF_EPA_TX2PAON_VECTOR_02_MSK                            (((1ULL<<RF_EPA_TX2PAON_VECTOR_02_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_02_POS)
#define RF_EPA_TX2PAON_VECTOR_02_UMSK                           (~(((1ULL<<RF_EPA_TX2PAON_VECTOR_02_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_02_POS))
#define RF_EPA_TX2PAON_VECTOR_03                                RF_EPA_TX2PAON_VECTOR_03
#define RF_EPA_TX2PAON_VECTOR_03_POS                            (3U)
#define RF_EPA_TX2PAON_VECTOR_03_LEN                            (1U)
#define RF_EPA_TX2PAON_VECTOR_03_MSK                            (((1ULL<<RF_EPA_TX2PAON_VECTOR_03_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_03_POS)
#define RF_EPA_TX2PAON_VECTOR_03_UMSK                           (~(((1ULL<<RF_EPA_TX2PAON_VECTOR_03_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_03_POS))

/* 0x594 : txpaon_rf_epa_0 */
#define RF_EPA_TXPAON_RF_EPA_0_OFFSET                           (0x594)
#define RF_EPA_TXPAON_VECTOR_00                                 RF_EPA_TXPAON_VECTOR_00
#define RF_EPA_TXPAON_VECTOR_00_POS                             (0U)
#define RF_EPA_TXPAON_VECTOR_00_LEN                             (1U)
#define RF_EPA_TXPAON_VECTOR_00_MSK                             (((1ULL<<RF_EPA_TXPAON_VECTOR_00_LEN)-1)<<RF_EPA_TXPAON_VECTOR_00_POS)
#define RF_EPA_TXPAON_VECTOR_00_UMSK                            (~(((1ULL<<RF_EPA_TXPAON_VECTOR_00_LEN)-1)<<RF_EPA_TXPAON_VECTOR_00_POS))
#define RF_EPA_TXPAON_VECTOR_01                                 RF_EPA_TXPAON_VECTOR_01
#define RF_EPA_TXPAON_VECTOR_01_POS                             (1U)
#define RF_EPA_TXPAON_VECTOR_01_LEN                             (1U)
#define RF_EPA_TXPAON_VECTOR_01_MSK                             (((1ULL<<RF_EPA_TXPAON_VECTOR_01_LEN)-1)<<RF_EPA_TXPAON_VECTOR_01_POS)
#define RF_EPA_TXPAON_VECTOR_01_UMSK                            (~(((1ULL<<RF_EPA_TXPAON_VECTOR_01_LEN)-1)<<RF_EPA_TXPAON_VECTOR_01_POS))
#define RF_EPA_TXPAON_VECTOR_02                                 RF_EPA_TXPAON_VECTOR_02
#define RF_EPA_TXPAON_VECTOR_02_POS                             (2U)
#define RF_EPA_TXPAON_VECTOR_02_LEN                             (1U)
#define RF_EPA_TXPAON_VECTOR_02_MSK                             (((1ULL<<RF_EPA_TXPAON_VECTOR_02_LEN)-1)<<RF_EPA_TXPAON_VECTOR_02_POS)
#define RF_EPA_TXPAON_VECTOR_02_UMSK                            (~(((1ULL<<RF_EPA_TXPAON_VECTOR_02_LEN)-1)<<RF_EPA_TXPAON_VECTOR_02_POS))
#define RF_EPA_TXPAON_VECTOR_03                                 RF_EPA_TXPAON_VECTOR_03
#define RF_EPA_TXPAON_VECTOR_03_POS                             (3U)
#define RF_EPA_TXPAON_VECTOR_03_LEN                             (1U)
#define RF_EPA_TXPAON_VECTOR_03_MSK                             (((1ULL<<RF_EPA_TXPAON_VECTOR_03_LEN)-1)<<RF_EPA_TXPAON_VECTOR_03_POS)
#define RF_EPA_TXPAON_VECTOR_03_UMSK                            (~(((1ULL<<RF_EPA_TXPAON_VECTOR_03_LEN)-1)<<RF_EPA_TXPAON_VECTOR_03_POS))

/* 0x5c0 : rf_epa_0_bz */
#define RF_EPA_0_BZ_OFFSET                                      (0x5c0)
#define RF_EPA_VECTOR_00_BZ                                     RF_EPA_VECTOR_00_BZ
#define RF_EPA_VECTOR_00_BZ_POS                                 (0U)
#define RF_EPA_VECTOR_00_BZ_LEN                                 (1U)
#define RF_EPA_VECTOR_00_BZ_MSK                                 (((1ULL<<RF_EPA_VECTOR_00_BZ_LEN)-1)<<RF_EPA_VECTOR_00_BZ_POS)
#define RF_EPA_VECTOR_00_BZ_UMSK                                (~(((1ULL<<RF_EPA_VECTOR_00_BZ_LEN)-1)<<RF_EPA_VECTOR_00_BZ_POS))
#define RF_EPA_VECTOR_01_BZ                                     RF_EPA_VECTOR_01_BZ
#define RF_EPA_VECTOR_01_BZ_POS                                 (1U)
#define RF_EPA_VECTOR_01_BZ_LEN                                 (1U)
#define RF_EPA_VECTOR_01_BZ_MSK                                 (((1ULL<<RF_EPA_VECTOR_01_BZ_LEN)-1)<<RF_EPA_VECTOR_01_BZ_POS)
#define RF_EPA_VECTOR_01_BZ_UMSK                                (~(((1ULL<<RF_EPA_VECTOR_01_BZ_LEN)-1)<<RF_EPA_VECTOR_01_BZ_POS))
#define RF_EPA_VECTOR_02_BZ                                     RF_EPA_VECTOR_02_BZ
#define RF_EPA_VECTOR_02_BZ_POS                                 (2U)
#define RF_EPA_VECTOR_02_BZ_LEN                                 (1U)
#define RF_EPA_VECTOR_02_BZ_MSK                                 (((1ULL<<RF_EPA_VECTOR_02_BZ_LEN)-1)<<RF_EPA_VECTOR_02_BZ_POS)
#define RF_EPA_VECTOR_02_BZ_UMSK                                (~(((1ULL<<RF_EPA_VECTOR_02_BZ_LEN)-1)<<RF_EPA_VECTOR_02_BZ_POS))
#define RF_EPA_VECTOR_03_BZ                                     RF_EPA_VECTOR_03_BZ
#define RF_EPA_VECTOR_03_BZ_POS                                 (3U)
#define RF_EPA_VECTOR_03_BZ_LEN                                 (1U)
#define RF_EPA_VECTOR_03_BZ_MSK                                 (((1ULL<<RF_EPA_VECTOR_03_BZ_LEN)-1)<<RF_EPA_VECTOR_03_BZ_POS)
#define RF_EPA_VECTOR_03_BZ_UMSK                                (~(((1ULL<<RF_EPA_VECTOR_03_BZ_LEN)-1)<<RF_EPA_VECTOR_03_BZ_POS))

/* 0x5C4 : rx2on_rf_epa_0_bz */
#define RF_EPA_RX2ON_RF_EPA_0_BZ_OFFSET                         (0x5C4)
#define RF_EPA_RX2ON_VECTOR_00_BZ                               RF_EPA_RX2ON_VECTOR_00_BZ
#define RF_EPA_RX2ON_VECTOR_00_BZ_POS                           (0U)
#define RF_EPA_RX2ON_VECTOR_00_BZ_LEN                           (1U)
#define RF_EPA_RX2ON_VECTOR_00_BZ_MSK                           (((1ULL<<RF_EPA_RX2ON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_RX2ON_VECTOR_00_BZ_POS)
#define RF_EPA_RX2ON_VECTOR_00_BZ_UMSK                          (~(((1ULL<<RF_EPA_RX2ON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_RX2ON_VECTOR_00_BZ_POS))
#define RF_EPA_RX2ON_VECTOR_01_BZ                               RF_EPA_RX2ON_VECTOR_01_BZ
#define RF_EPA_RX2ON_VECTOR_01_BZ_POS                           (1U)
#define RF_EPA_RX2ON_VECTOR_01_BZ_LEN                           (1U)
#define RF_EPA_RX2ON_VECTOR_01_BZ_MSK                           (((1ULL<<RF_EPA_RX2ON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_RX2ON_VECTOR_01_BZ_POS)
#define RF_EPA_RX2ON_VECTOR_01_BZ_UMSK                          (~(((1ULL<<RF_EPA_RX2ON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_RX2ON_VECTOR_01_BZ_POS))
#define RF_EPA_RX2ON_VECTOR_02_BZ                               RF_EPA_RX2ON_VECTOR_02_BZ
#define RF_EPA_RX2ON_VECTOR_02_BZ_POS                           (2U)
#define RF_EPA_RX2ON_VECTOR_02_BZ_LEN                           (1U)
#define RF_EPA_RX2ON_VECTOR_02_BZ_MSK                           (((1ULL<<RF_EPA_RX2ON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_RX2ON_VECTOR_02_BZ_POS)
#define RF_EPA_RX2ON_VECTOR_02_BZ_UMSK                          (~(((1ULL<<RF_EPA_RX2ON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_RX2ON_VECTOR_02_BZ_POS))
#define RF_EPA_RX2ON_VECTOR_03_BZ                               RF_EPA_RX2ON_VECTOR_03_BZ
#define RF_EPA_RX2ON_VECTOR_03_BZ_POS                           (3U)
#define RF_EPA_RX2ON_VECTOR_03_BZ_LEN                           (1U)
#define RF_EPA_RX2ON_VECTOR_03_BZ_MSK                           (((1ULL<<RF_EPA_RX2ON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_RX2ON_VECTOR_03_BZ_POS)
#define RF_EPA_RX2ON_VECTOR_03_BZ_UMSK                          (~(((1ULL<<RF_EPA_RX2ON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_RX2ON_VECTOR_03_BZ_POS))

/* 0x5C8 : rxon_rf_epa_0_bz */
#define RF_EPA_RXON_RF_EPA_0_BZ_OFFSET                          (0x5C8)
#define RF_EPA_RXON_VECTOR_00_BZ                                RF_EPA_RXON_VECTOR_00_BZ
#define RF_EPA_RXON_VECTOR_00_BZ_POS                            (0U)
#define RF_EPA_RXON_VECTOR_00_BZ_LEN                            (1U)
#define RF_EPA_RXON_VECTOR_00_BZ_MSK                            (((1ULL<<RF_EPA_RXON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_RXON_VECTOR_00_BZ_POS)
#define RF_EPA_RXON_VECTOR_00_BZ_UMSK                           (~(((1ULL<<RF_EPA_RXON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_RXON_VECTOR_00_BZ_POS))
#define RF_EPA_RXON_VECTOR_01_BZ                                RF_EPA_RXON_VECTOR_01_BZ
#define RF_EPA_RXON_VECTOR_01_BZ_POS                            (1U)
#define RF_EPA_RXON_VECTOR_01_BZ_LEN                            (1U)
#define RF_EPA_RXON_VECTOR_01_BZ_MSK                            (((1ULL<<RF_EPA_RXON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_RXON_VECTOR_01_BZ_POS)
#define RF_EPA_RXON_VECTOR_01_BZ_UMSK                           (~(((1ULL<<RF_EPA_RXON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_RXON_VECTOR_01_BZ_POS))
#define RF_EPA_RXON_VECTOR_02_BZ                                RF_EPA_RXON_VECTOR_02_BZ
#define RF_EPA_RXON_VECTOR_02_BZ_POS                            (2U)
#define RF_EPA_RXON_VECTOR_02_BZ_LEN                            (1U)
#define RF_EPA_RXON_VECTOR_02_BZ_MSK                            (((1ULL<<RF_EPA_RXON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_RXON_VECTOR_02_BZ_POS)
#define RF_EPA_RXON_VECTOR_02_BZ_UMSK                           (~(((1ULL<<RF_EPA_RXON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_RXON_VECTOR_02_BZ_POS))
#define RF_EPA_RXON_VECTOR_03_BZ                                RF_EPA_RXON_VECTOR_03_BZ
#define RF_EPA_RXON_VECTOR_03_BZ_POS                            (3U)
#define RF_EPA_RXON_VECTOR_03_BZ_LEN                            (1U)
#define RF_EPA_RXON_VECTOR_03_BZ_MSK                            (((1ULL<<RF_EPA_RXON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_RXON_VECTOR_03_BZ_POS)
#define RF_EPA_RXON_VECTOR_03_BZ_UMSK                           (~(((1ULL<<RF_EPA_RXON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_RXON_VECTOR_03_BZ_POS))

/* 0x5CC : tx2on_rf_epa_0_bz */
#define RF_EPA_TX2ON_RF_EPA_0_BZ_OFFSET                         (0x5CC)
#define RF_EPA_TX2ON_VECTOR_00_BZ                               RF_EPA_TX2ON_VECTOR_00_BZ
#define RF_EPA_TX2ON_VECTOR_00_BZ_POS                           (0U)
#define RF_EPA_TX2ON_VECTOR_00_BZ_LEN                           (1U)
#define RF_EPA_TX2ON_VECTOR_00_BZ_MSK                           (((1ULL<<RF_EPA_TX2ON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_TX2ON_VECTOR_00_BZ_POS)
#define RF_EPA_TX2ON_VECTOR_00_BZ_UMSK                          (~(((1ULL<<RF_EPA_TX2ON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_TX2ON_VECTOR_00_BZ_POS))
#define RF_EPA_TX2ON_VECTOR_01_BZ                               RF_EPA_TX2ON_VECTOR_01_BZ
#define RF_EPA_TX2ON_VECTOR_01_BZ_POS                           (1U)
#define RF_EPA_TX2ON_VECTOR_01_BZ_LEN                           (1U)
#define RF_EPA_TX2ON_VECTOR_01_BZ_MSK                           (((1ULL<<RF_EPA_TX2ON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_TX2ON_VECTOR_01_BZ_POS)
#define RF_EPA_TX2ON_VECTOR_01_BZ_UMSK                          (~(((1ULL<<RF_EPA_TX2ON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_TX2ON_VECTOR_01_BZ_POS))
#define RF_EPA_TX2ON_VECTOR_02_BZ                               RF_EPA_TX2ON_VECTOR_02_BZ
#define RF_EPA_TX2ON_VECTOR_02_BZ_POS                           (2U)
#define RF_EPA_TX2ON_VECTOR_02_BZ_LEN                           (1U)
#define RF_EPA_TX2ON_VECTOR_02_BZ_MSK                           (((1ULL<<RF_EPA_TX2ON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_TX2ON_VECTOR_02_BZ_POS)
#define RF_EPA_TX2ON_VECTOR_02_BZ_UMSK                          (~(((1ULL<<RF_EPA_TX2ON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_TX2ON_VECTOR_02_BZ_POS))
#define RF_EPA_TX2ON_VECTOR_03_BZ                               RF_EPA_TX2ON_VECTOR_03_BZ
#define RF_EPA_TX2ON_VECTOR_03_BZ_POS                           (3U)
#define RF_EPA_TX2ON_VECTOR_03_BZ_LEN                           (1U)
#define RF_EPA_TX2ON_VECTOR_03_BZ_MSK                           (((1ULL<<RF_EPA_TX2ON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_TX2ON_VECTOR_03_BZ_POS)
#define RF_EPA_TX2ON_VECTOR_03_BZ_UMSK                          (~(((1ULL<<RF_EPA_TX2ON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_TX2ON_VECTOR_03_BZ_POS))

/* 0x5D0 : tx2paon_rf_epa_0_bz */
#define RF_EPA_TX2PAON_RF_EPA_0_BZ_OFFSET                       (0x5D0)
#define RF_EPA_TX2PAON_VECTOR_00_BZ                             RF_EPA_TX2PAON_VECTOR_00_BZ
#define RF_EPA_TX2PAON_VECTOR_00_BZ_POS                         (0U)
#define RF_EPA_TX2PAON_VECTOR_00_BZ_LEN                         (1U)
#define RF_EPA_TX2PAON_VECTOR_00_BZ_MSK                         (((1ULL<<RF_EPA_TX2PAON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_00_BZ_POS)
#define RF_EPA_TX2PAON_VECTOR_00_BZ_UMSK                        (~(((1ULL<<RF_EPA_TX2PAON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_00_BZ_POS))
#define RF_EPA_TX2PAON_VECTOR_01_BZ                             RF_EPA_TX2PAON_VECTOR_01_BZ
#define RF_EPA_TX2PAON_VECTOR_01_BZ_POS                         (1U)
#define RF_EPA_TX2PAON_VECTOR_01_BZ_LEN                         (1U)
#define RF_EPA_TX2PAON_VECTOR_01_BZ_MSK                         (((1ULL<<RF_EPA_TX2PAON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_01_BZ_POS)
#define RF_EPA_TX2PAON_VECTOR_01_BZ_UMSK                        (~(((1ULL<<RF_EPA_TX2PAON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_01_BZ_POS))
#define RF_EPA_TX2PAON_VECTOR_02_BZ                             RF_EPA_TX2PAON_VECTOR_02_BZ
#define RF_EPA_TX2PAON_VECTOR_02_BZ_POS                         (2U)
#define RF_EPA_TX2PAON_VECTOR_02_BZ_LEN                         (1U)
#define RF_EPA_TX2PAON_VECTOR_02_BZ_MSK                         (((1ULL<<RF_EPA_TX2PAON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_02_BZ_POS)
#define RF_EPA_TX2PAON_VECTOR_02_BZ_UMSK                        (~(((1ULL<<RF_EPA_TX2PAON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_02_BZ_POS))
#define RF_EPA_TX2PAON_VECTOR_03_BZ                             RF_EPA_TX2PAON_VECTOR_03_BZ
#define RF_EPA_TX2PAON_VECTOR_03_BZ_POS                         (3U)
#define RF_EPA_TX2PAON_VECTOR_03_BZ_LEN                         (1U)
#define RF_EPA_TX2PAON_VECTOR_03_BZ_MSK                         (((1ULL<<RF_EPA_TX2PAON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_03_BZ_POS)
#define RF_EPA_TX2PAON_VECTOR_03_BZ_UMSK                        (~(((1ULL<<RF_EPA_TX2PAON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_TX2PAON_VECTOR_03_BZ_POS))

/* 0x5D4 : txpaon_rf_epa_0_bz */
#define RF_EPA_TXPAON_RF_EPA_0_BZ_OFFSET                        (0x5D4)
#define RF_EPA_TXPAON_VECTOR_00_BZ                              RF_EPA_TXPAON_VECTOR_00_BZ
#define RF_EPA_TXPAON_VECTOR_00_BZ_POS                          (0U)
#define RF_EPA_TXPAON_VECTOR_00_BZ_LEN                          (1U)
#define RF_EPA_TXPAON_VECTOR_00_BZ_MSK                          (((1ULL<<RF_EPA_TXPAON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_TXPAON_VECTOR_00_BZ_POS)
#define RF_EPA_TXPAON_VECTOR_00_BZ_UMSK                         (~(((1ULL<<RF_EPA_TXPAON_VECTOR_00_BZ_LEN)-1)<<RF_EPA_TXPAON_VECTOR_00_BZ_POS))
#define RF_EPA_TXPAON_VECTOR_01_BZ                              RF_EPA_TXPAON_VECTOR_01_BZ
#define RF_EPA_TXPAON_VECTOR_01_BZ_POS                          (1U)
#define RF_EPA_TXPAON_VECTOR_01_BZ_LEN                          (1U)
#define RF_EPA_TXPAON_VECTOR_01_BZ_MSK                          (((1ULL<<RF_EPA_TXPAON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_TXPAON_VECTOR_01_BZ_POS)
#define RF_EPA_TXPAON_VECTOR_01_BZ_UMSK                         (~(((1ULL<<RF_EPA_TXPAON_VECTOR_01_BZ_LEN)-1)<<RF_EPA_TXPAON_VECTOR_01_BZ_POS))
#define RF_EPA_TXPAON_VECTOR_02_BZ                              RF_EPA_TXPAON_VECTOR_02_BZ
#define RF_EPA_TXPAON_VECTOR_02_BZ_POS                          (2U)
#define RF_EPA_TXPAON_VECTOR_02_BZ_LEN                          (1U)
#define RF_EPA_TXPAON_VECTOR_02_BZ_MSK                          (((1ULL<<RF_EPA_TXPAON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_TXPAON_VECTOR_02_BZ_POS)
#define RF_EPA_TXPAON_VECTOR_02_BZ_UMSK                         (~(((1ULL<<RF_EPA_TXPAON_VECTOR_02_BZ_LEN)-1)<<RF_EPA_TXPAON_VECTOR_02_BZ_POS))
#define RF_EPA_TXPAON_VECTOR_03_BZ                              RF_EPA_TXPAON_VECTOR_03_BZ
#define RF_EPA_TXPAON_VECTOR_03_BZ_POS                          (3U)
#define RF_EPA_TXPAON_VECTOR_03_BZ_LEN                          (1U)
#define RF_EPA_TXPAON_VECTOR_03_BZ_MSK                          (((1ULL<<RF_EPA_TXPAON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_TXPAON_VECTOR_03_BZ_POS)
#define RF_EPA_TXPAON_VECTOR_03_BZ_UMSK                         (~(((1ULL<<RF_EPA_TXPAON_VECTOR_03_BZ_LEN)-1)<<RF_EPA_TXPAON_VECTOR_03_BZ_POS))


struct  rf_epa_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[1408];

    /* 0x580 : rf_epa_0 */
    union {
        struct {
            uint32_t vector_00                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t vector_01                      :  1; /* [    1],        r/w,        0x0 */
            uint32_t vector_02                      :  1; /* [    2],        r/w,        0x0 */
            uint32_t vector_03                      :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_epa_0;

    /* 0x584 : rx2on_rf_epa_0 */
    union {
        struct {
            uint32_t rx2on_vector_00                :  1; /* [    0],        r/w,        0x0 */
            uint32_t rx2on_vector_01                :  1; /* [    1],        r/w,        0x1 */
            uint32_t rx2on_vector_02                :  1; /* [    2],        r/w,        0x0 */
            uint32_t rx2on_vector_03                :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rx2on_rf_epa_0;

    /* 0x588 : rxon_rf_epa_0 */
    union {
        struct {
            uint32_t rxon_vector_00                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t rxon_vector_01                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t rxon_vector_02                 :  1; /* [    2],        r/w,        0x0 */
            uint32_t rxon_vector_03                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxon_rf_epa_0;

    /* 0x58C : tx2on_rf_epa_0 */
    union {
        struct {
            uint32_t tx2on_vector_00                :  1; /* [    0],        r/w,        0x0 */
            uint32_t tx2on_vector_01                :  1; /* [    1],        r/w,        0x1 */
            uint32_t tx2on_vector_02                :  1; /* [    2],        r/w,        0x0 */
            uint32_t tx2on_vector_03                :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx2on_rf_epa_0;

    /* 0x590 : tx2paon_rf_epa_0 */
    union {
        struct {
            uint32_t tx2paon_vector_00              :  1; /* [    0],        r/w,        0x1 */
            uint32_t tx2paon_vector_01              :  1; /* [    1],        r/w,        0x1 */
            uint32_t tx2paon_vector_02              :  1; /* [    2],        r/w,        0x0 */
            uint32_t tx2paon_vector_03              :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx2paon_rf_epa_0;

    /* 0x594 : txpaon_rf_epa_0 */
    union {
        struct {
            uint32_t txpaon_vector_00               :  1; /* [    0],        r/w,        0x1 */
            uint32_t txpaon_vector_01               :  1; /* [    1],        r/w,        0x1 */
            uint32_t txpaon_vector_02               :  1; /* [    2],        r/w,        0x0 */
            uint32_t txpaon_vector_03               :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } txpaon_rf_epa_0;

    /* 0x598  reserved */
    uint8_t RESERVED0x598[40];

    /* 0x5c0 : rf_epa_0_bz */
    union {
        struct {
            uint32_t vector_00_bz                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t vector_01_bz                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t vector_02_bz                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t vector_03_bz                   :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_epa_0_bz;

    /* 0x5C4 : rx2on_rf_epa_0_bz */
    union {
        struct {
            uint32_t rx2on_vector_00_bz             :  1; /* [    0],        r/w,        0x0 */
            uint32_t rx2on_vector_01_bz             :  1; /* [    1],        r/w,        0x1 */
            uint32_t rx2on_vector_02_bz             :  1; /* [    2],        r/w,        0x0 */
            uint32_t rx2on_vector_03_bz             :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rx2on_rf_epa_0_bz;

    /* 0x5C8 : rxon_rf_epa_0_bz */
    union {
        struct {
            uint32_t rxon_vector_00_bz              :  1; /* [    0],        r/w,        0x0 */
            uint32_t rxon_vector_01_bz              :  1; /* [    1],        r/w,        0x1 */
            uint32_t rxon_vector_02_bz              :  1; /* [    2],        r/w,        0x0 */
            uint32_t rxon_vector_03_bz              :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxon_rf_epa_0_bz;

    /* 0x5CC : tx2on_rf_epa_0_bz */
    union {
        struct {
            uint32_t tx2on_vector_00_bz             :  1; /* [    0],        r/w,        0x0 */
            uint32_t tx2on_vector_01_bz             :  1; /* [    1],        r/w,        0x1 */
            uint32_t tx2on_vector_02_bz             :  1; /* [    2],        r/w,        0x0 */
            uint32_t tx2on_vector_03_bz             :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx2on_rf_epa_0_bz;

    /* 0x5D0 : tx2paon_rf_epa_0_bz */
    union {
        struct {
            uint32_t tx2paon_vector_00_bz           :  1; /* [    0],        r/w,        0x1 */
            uint32_t tx2paon_vector_01_bz           :  1; /* [    1],        r/w,        0x1 */
            uint32_t tx2paon_vector_02_bz           :  1; /* [    2],        r/w,        0x0 */
            uint32_t tx2paon_vector_03_bz           :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx2paon_rf_epa_0_bz;

    /* 0x5D4 : txpaon_rf_epa_0_bz */
    union {
        struct {
            uint32_t txpaon_vector_00_bz            :  1; /* [    0],        r/w,        0x1 */
            uint32_t txpaon_vector_01_bz            :  1; /* [    1],        r/w,        0x1 */
            uint32_t txpaon_vector_02_bz            :  1; /* [    2],        r/w,        0x0 */
            uint32_t txpaon_vector_03_bz            :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } txpaon_rf_epa_0_bz;

};

typedef volatile struct rf_epa_reg rf_epa_reg_t;


#endif  /* __RF_EPA_REG_H__ */
