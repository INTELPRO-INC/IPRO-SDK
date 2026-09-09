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

#ifndef  __DMA_CTRL_REG_H__
#define  __DMA_CTRL_REG_H__

#include "ipro6.h"

/* 0x0 : DMA_IntStatus */
#define DMA_CTRL_DMA_INTSTATUS_OFFSET                           (0x0)
#define DMA_CTRL_INTSTATUS                                      DMA_CTRL_INTSTATUS
#define DMA_CTRL_INTSTATUS_POS                                  (0U)
#define DMA_CTRL_INTSTATUS_LEN                                  (8U)
#define DMA_CTRL_INTSTATUS_MSK                                  (((1ULL<<DMA_CTRL_INTSTATUS_LEN)-1)<<DMA_CTRL_INTSTATUS_POS)
#define DMA_CTRL_INTSTATUS_UMSK                                 (~(((1ULL<<DMA_CTRL_INTSTATUS_LEN)-1)<<DMA_CTRL_INTSTATUS_POS))

/* 0x4 : DMA_IntTCStatus */
#define DMA_CTRL_DMA_INTTCSTATUS_OFFSET                         (0x4)
#define DMA_CTRL_INTTCSTATUS                                    DMA_CTRL_INTTCSTATUS
#define DMA_CTRL_INTTCSTATUS_POS                                (0U)
#define DMA_CTRL_INTTCSTATUS_LEN                                (8U)
#define DMA_CTRL_INTTCSTATUS_MSK                                (((1ULL<<DMA_CTRL_INTTCSTATUS_LEN)-1)<<DMA_CTRL_INTTCSTATUS_POS)
#define DMA_CTRL_INTTCSTATUS_UMSK                               (~(((1ULL<<DMA_CTRL_INTTCSTATUS_LEN)-1)<<DMA_CTRL_INTTCSTATUS_POS))

/* 0x8 : DMA_IntTCClear */
#define DMA_CTRL_DMA_INTTCCLEAR_OFFSET                          (0x8)
#define DMA_CTRL_INTTCCLEAR                                     DMA_CTRL_INTTCCLEAR
#define DMA_CTRL_INTTCCLEAR_POS                                 (0U)
#define DMA_CTRL_INTTCCLEAR_LEN                                 (8U)
#define DMA_CTRL_INTTCCLEAR_MSK                                 (((1ULL<<DMA_CTRL_INTTCCLEAR_LEN)-1)<<DMA_CTRL_INTTCCLEAR_POS)
#define DMA_CTRL_INTTCCLEAR_UMSK                                (~(((1ULL<<DMA_CTRL_INTTCCLEAR_LEN)-1)<<DMA_CTRL_INTTCCLEAR_POS))

/* 0xC : DMA_IntErrorStatus */
#define DMA_CTRL_DMA_INTERRORSTATUS_OFFSET                      (0xC)
#define DMA_CTRL_INTERRORSTATUS                                 DMA_CTRL_INTERRORSTATUS
#define DMA_CTRL_INTERRORSTATUS_POS                             (0U)
#define DMA_CTRL_INTERRORSTATUS_LEN                             (8U)
#define DMA_CTRL_INTERRORSTATUS_MSK                             (((1ULL<<DMA_CTRL_INTERRORSTATUS_LEN)-1)<<DMA_CTRL_INTERRORSTATUS_POS)
#define DMA_CTRL_INTERRORSTATUS_UMSK                            (~(((1ULL<<DMA_CTRL_INTERRORSTATUS_LEN)-1)<<DMA_CTRL_INTERRORSTATUS_POS))

/* 0x10 : DMA_IntErrClr */
#define DMA_CTRL_DMA_INTERRCLR_OFFSET                           (0x10)
#define DMA_CTRL_INTERRCLR                                      DMA_CTRL_INTERRCLR
#define DMA_CTRL_INTERRCLR_POS                                  (0U)
#define DMA_CTRL_INTERRCLR_LEN                                  (8U)
#define DMA_CTRL_INTERRCLR_MSK                                  (((1ULL<<DMA_CTRL_INTERRCLR_LEN)-1)<<DMA_CTRL_INTERRCLR_POS)
#define DMA_CTRL_INTERRCLR_UMSK                                 (~(((1ULL<<DMA_CTRL_INTERRCLR_LEN)-1)<<DMA_CTRL_INTERRCLR_POS))

/* 0x14 : DMA_RawIntTCStatus */
#define DMA_CTRL_DMA_RAWINTTCSTATUS_OFFSET                      (0x14)
#define DMA_CTRL_RAWINTTCSTATUS                                 DMA_CTRL_RAWINTTCSTATUS
#define DMA_CTRL_RAWINTTCSTATUS_POS                             (0U)
#define DMA_CTRL_RAWINTTCSTATUS_LEN                             (8U)
#define DMA_CTRL_RAWINTTCSTATUS_MSK                             (((1ULL<<DMA_CTRL_RAWINTTCSTATUS_LEN)-1)<<DMA_CTRL_RAWINTTCSTATUS_POS)
#define DMA_CTRL_RAWINTTCSTATUS_UMSK                            (~(((1ULL<<DMA_CTRL_RAWINTTCSTATUS_LEN)-1)<<DMA_CTRL_RAWINTTCSTATUS_POS))

/* 0x18 : DMA_RawIntErrorStatus */
#define DMA_CTRL_DMA_RAWINTERRORSTATUS_OFFSET                   (0x18)
#define DMA_CTRL_RAWINTERRORSTATUS                              DMA_CTRL_RAWINTERRORSTATUS
#define DMA_CTRL_RAWINTERRORSTATUS_POS                          (0U)
#define DMA_CTRL_RAWINTERRORSTATUS_LEN                          (8U)
#define DMA_CTRL_RAWINTERRORSTATUS_MSK                          (((1ULL<<DMA_CTRL_RAWINTERRORSTATUS_LEN)-1)<<DMA_CTRL_RAWINTERRORSTATUS_POS)
#define DMA_CTRL_RAWINTERRORSTATUS_UMSK                         (~(((1ULL<<DMA_CTRL_RAWINTERRORSTATUS_LEN)-1)<<DMA_CTRL_RAWINTERRORSTATUS_POS))

/* 0x1C : DMA_EnbldChns */
#define DMA_CTRL_DMA_ENBLDCHNS_OFFSET                           (0x1C)
#define DMA_CTRL_ENABLEDCHANNELS                                DMA_CTRL_ENABLEDCHANNELS
#define DMA_CTRL_ENABLEDCHANNELS_POS                            (0U)
#define DMA_CTRL_ENABLEDCHANNELS_LEN                            (8U)
#define DMA_CTRL_ENABLEDCHANNELS_MSK                            (((1ULL<<DMA_CTRL_ENABLEDCHANNELS_LEN)-1)<<DMA_CTRL_ENABLEDCHANNELS_POS)
#define DMA_CTRL_ENABLEDCHANNELS_UMSK                           (~(((1ULL<<DMA_CTRL_ENABLEDCHANNELS_LEN)-1)<<DMA_CTRL_ENABLEDCHANNELS_POS))

/* 0x20 : DMA_SoftBReq */
#define DMA_CTRL_DMA_SOFTBREQ_OFFSET                            (0x20)
#define DMA_CTRL_SOFTBREQ                                       DMA_CTRL_SOFTBREQ
#define DMA_CTRL_SOFTBREQ_POS                                   (0U)
#define DMA_CTRL_SOFTBREQ_LEN                                   (32U)
#define DMA_CTRL_SOFTBREQ_MSK                                   (((1ULL<<DMA_CTRL_SOFTBREQ_LEN)-1)<<DMA_CTRL_SOFTBREQ_POS)
#define DMA_CTRL_SOFTBREQ_UMSK                                  (~(((1ULL<<DMA_CTRL_SOFTBREQ_LEN)-1)<<DMA_CTRL_SOFTBREQ_POS))

/* 0x24 : DMA_SoftSReq */
#define DMA_CTRL_DMA_SOFTSREQ_OFFSET                            (0x24)
#define DMA_CTRL_SOFTSREQ                                       DMA_CTRL_SOFTSREQ
#define DMA_CTRL_SOFTSREQ_POS                                   (0U)
#define DMA_CTRL_SOFTSREQ_LEN                                   (32U)
#define DMA_CTRL_SOFTSREQ_MSK                                   (((1ULL<<DMA_CTRL_SOFTSREQ_LEN)-1)<<DMA_CTRL_SOFTSREQ_POS)
#define DMA_CTRL_SOFTSREQ_UMSK                                  (~(((1ULL<<DMA_CTRL_SOFTSREQ_LEN)-1)<<DMA_CTRL_SOFTSREQ_POS))

/* 0x28 : DMA_SoftLBReq */
#define DMA_CTRL_DMA_SOFTLBREQ_OFFSET                           (0x28)
#define DMA_CTRL_SOFTLBREQ                                      DMA_CTRL_SOFTLBREQ
#define DMA_CTRL_SOFTLBREQ_POS                                  (0U)
#define DMA_CTRL_SOFTLBREQ_LEN                                  (32U)
#define DMA_CTRL_SOFTLBREQ_MSK                                  (((1ULL<<DMA_CTRL_SOFTLBREQ_LEN)-1)<<DMA_CTRL_SOFTLBREQ_POS)
#define DMA_CTRL_SOFTLBREQ_UMSK                                 (~(((1ULL<<DMA_CTRL_SOFTLBREQ_LEN)-1)<<DMA_CTRL_SOFTLBREQ_POS))

/* 0x2C : DMA_SoftLSReq */
#define DMA_CTRL_DMA_SOFTLSREQ_OFFSET                           (0x2C)
#define DMA_CTRL_SOFTLSREQ                                      DMA_CTRL_SOFTLSREQ
#define DMA_CTRL_SOFTLSREQ_POS                                  (0U)
#define DMA_CTRL_SOFTLSREQ_LEN                                  (32U)
#define DMA_CTRL_SOFTLSREQ_MSK                                  (((1ULL<<DMA_CTRL_SOFTLSREQ_LEN)-1)<<DMA_CTRL_SOFTLSREQ_POS)
#define DMA_CTRL_SOFTLSREQ_UMSK                                 (~(((1ULL<<DMA_CTRL_SOFTLSREQ_LEN)-1)<<DMA_CTRL_SOFTLSREQ_POS))

/* 0x30 : DMA_Top_Config */
#define DMA_CTRL_DMA_TOP_CONFIG_OFFSET                          (0x30)
#define DMA_CTRL_E                                              DMA_CTRL_E
#define DMA_CTRL_E_POS                                          (0U)
#define DMA_CTRL_E_LEN                                          (1U)
#define DMA_CTRL_E_MSK                                          (((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS)
#define DMA_CTRL_E_UMSK                                         (~(((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS))
#define DMA_CTRL_M                                              DMA_CTRL_M
#define DMA_CTRL_M_POS                                          (1U)
#define DMA_CTRL_M_LEN                                          (1U)
#define DMA_CTRL_M_MSK                                          (((1ULL<<DMA_CTRL_M_LEN)-1)<<DMA_CTRL_M_POS)
#define DMA_CTRL_M_UMSK                                         (~(((1ULL<<DMA_CTRL_M_LEN)-1)<<DMA_CTRL_M_POS))

/* 0x34 : DMA_Sync */
#define DMA_CTRL_DMA_SYNC_OFFSET                                (0x34)
#define DMA_CTRL_DMA_SYNC                                       DMA_CTRL_DMA_SYNC
#define DMA_CTRL_DMA_SYNC_POS                                   (0U)
#define DMA_CTRL_DMA_SYNC_LEN                                   (32U)
#define DMA_CTRL_DMA_SYNC_MSK                                   (((1ULL<<DMA_CTRL_DMA_SYNC_LEN)-1)<<DMA_CTRL_DMA_SYNC_POS)
#define DMA_CTRL_DMA_SYNC_UMSK                                  (~(((1ULL<<DMA_CTRL_DMA_SYNC_LEN)-1)<<DMA_CTRL_DMA_SYNC_POS))

/* 0x100 : DMA_C0SrcAddr */
#define DMA_CTRL_DMA_C0SRCADDR_OFFSET                           (0x100)
#define DMA_CTRL_SRCADDR                                        DMA_CTRL_SRCADDR
#define DMA_CTRL_SRCADDR_POS                                    (0U)
#define DMA_CTRL_SRCADDR_LEN                                    (32U)
#define DMA_CTRL_SRCADDR_MSK                                    (((1ULL<<DMA_CTRL_SRCADDR_LEN)-1)<<DMA_CTRL_SRCADDR_POS)
#define DMA_CTRL_SRCADDR_UMSK                                   (~(((1ULL<<DMA_CTRL_SRCADDR_LEN)-1)<<DMA_CTRL_SRCADDR_POS))

/* 0x104 : DMA_C0DstAddr */
#define DMA_CTRL_DMA_C0DSTADDR_OFFSET                           (0x104)
#define DMA_CTRL_DSTADDR                                        DMA_CTRL_DSTADDR
#define DMA_CTRL_DSTADDR_POS                                    (0U)
#define DMA_CTRL_DSTADDR_LEN                                    (32U)
#define DMA_CTRL_DSTADDR_MSK                                    (((1ULL<<DMA_CTRL_DSTADDR_LEN)-1)<<DMA_CTRL_DSTADDR_POS)
#define DMA_CTRL_DSTADDR_UMSK                                   (~(((1ULL<<DMA_CTRL_DSTADDR_LEN)-1)<<DMA_CTRL_DSTADDR_POS))

/* 0x108 : DMA_C0LLI */
#define DMA_CTRL_DMA_C0LLI_OFFSET                               (0x108)
#define DMA_CTRL_LLI                                            DMA_CTRL_LLI
#define DMA_CTRL_LLI_POS                                        (0U)
#define DMA_CTRL_LLI_LEN                                        (32U)
#define DMA_CTRL_LLI_MSK                                        (((1ULL<<DMA_CTRL_LLI_LEN)-1)<<DMA_CTRL_LLI_POS)
#define DMA_CTRL_LLI_UMSK                                       (~(((1ULL<<DMA_CTRL_LLI_LEN)-1)<<DMA_CTRL_LLI_POS))

/* 0x10C : DMA_C0Control */
#define DMA_CTRL_DMA_C0CONTROL_OFFSET                           (0x10C)
#define DMA_CTRL_TRANSFERSIZE                                   DMA_CTRL_TRANSFERSIZE
#define DMA_CTRL_TRANSFERSIZE_POS                               (0U)
#define DMA_CTRL_TRANSFERSIZE_LEN                               (12U)
#define DMA_CTRL_TRANSFERSIZE_MSK                               (((1ULL<<DMA_CTRL_TRANSFERSIZE_LEN)-1)<<DMA_CTRL_TRANSFERSIZE_POS)
#define DMA_CTRL_TRANSFERSIZE_UMSK                              (~(((1ULL<<DMA_CTRL_TRANSFERSIZE_LEN)-1)<<DMA_CTRL_TRANSFERSIZE_POS))
#define DMA_CTRL_SBSIZE                                         DMA_CTRL_SBSIZE
#define DMA_CTRL_SBSIZE_POS                                     (12U)
#define DMA_CTRL_SBSIZE_LEN                                     (2U)
#define DMA_CTRL_SBSIZE_MSK                                     (((1ULL<<DMA_CTRL_SBSIZE_LEN)-1)<<DMA_CTRL_SBSIZE_POS)
#define DMA_CTRL_SBSIZE_UMSK                                    (~(((1ULL<<DMA_CTRL_SBSIZE_LEN)-1)<<DMA_CTRL_SBSIZE_POS))
#define DMA_CTRL_DST_MIN_MODE                                   DMA_CTRL_DST_MIN_MODE
#define DMA_CTRL_DST_MIN_MODE_POS                               (14U)
#define DMA_CTRL_DST_MIN_MODE_LEN                               (1U)
#define DMA_CTRL_DST_MIN_MODE_MSK                               (((1ULL<<DMA_CTRL_DST_MIN_MODE_LEN)-1)<<DMA_CTRL_DST_MIN_MODE_POS)
#define DMA_CTRL_DST_MIN_MODE_UMSK                              (~(((1ULL<<DMA_CTRL_DST_MIN_MODE_LEN)-1)<<DMA_CTRL_DST_MIN_MODE_POS))
#define DMA_CTRL_DBSIZE                                         DMA_CTRL_DBSIZE
#define DMA_CTRL_DBSIZE_POS                                     (15U)
#define DMA_CTRL_DBSIZE_LEN                                     (2U)
#define DMA_CTRL_DBSIZE_MSK                                     (((1ULL<<DMA_CTRL_DBSIZE_LEN)-1)<<DMA_CTRL_DBSIZE_POS)
#define DMA_CTRL_DBSIZE_UMSK                                    (~(((1ULL<<DMA_CTRL_DBSIZE_LEN)-1)<<DMA_CTRL_DBSIZE_POS))
#define DMA_CTRL_DST_ADD_MODE                                   DMA_CTRL_DST_ADD_MODE
#define DMA_CTRL_DST_ADD_MODE_POS                               (17U)
#define DMA_CTRL_DST_ADD_MODE_LEN                               (1U)
#define DMA_CTRL_DST_ADD_MODE_MSK                               (((1ULL<<DMA_CTRL_DST_ADD_MODE_LEN)-1)<<DMA_CTRL_DST_ADD_MODE_POS)
#define DMA_CTRL_DST_ADD_MODE_UMSK                              (~(((1ULL<<DMA_CTRL_DST_ADD_MODE_LEN)-1)<<DMA_CTRL_DST_ADD_MODE_POS))
#define DMA_CTRL_SWIDTH                                         DMA_CTRL_SWIDTH
#define DMA_CTRL_SWIDTH_POS                                     (18U)
#define DMA_CTRL_SWIDTH_LEN                                     (2U)
#define DMA_CTRL_SWIDTH_MSK                                     (((1ULL<<DMA_CTRL_SWIDTH_LEN)-1)<<DMA_CTRL_SWIDTH_POS)
#define DMA_CTRL_SWIDTH_UMSK                                    (~(((1ULL<<DMA_CTRL_SWIDTH_LEN)-1)<<DMA_CTRL_SWIDTH_POS))
#define DMA_CTRL_DWIDTH                                         DMA_CTRL_DWIDTH
#define DMA_CTRL_DWIDTH_POS                                     (21U)
#define DMA_CTRL_DWIDTH_LEN                                     (2U)
#define DMA_CTRL_DWIDTH_MSK                                     (((1ULL<<DMA_CTRL_DWIDTH_LEN)-1)<<DMA_CTRL_DWIDTH_POS)
#define DMA_CTRL_DWIDTH_UMSK                                    (~(((1ULL<<DMA_CTRL_DWIDTH_LEN)-1)<<DMA_CTRL_DWIDTH_POS))
#define DMA_CTRL_FIX_CNT                                        DMA_CTRL_FIX_CNT
#define DMA_CTRL_FIX_CNT_POS                                    (23U)
#define DMA_CTRL_FIX_CNT_LEN                                    (3U)
#define DMA_CTRL_FIX_CNT_MSK                                    (((1ULL<<DMA_CTRL_FIX_CNT_LEN)-1)<<DMA_CTRL_FIX_CNT_POS)
#define DMA_CTRL_FIX_CNT_UMSK                                   (~(((1ULL<<DMA_CTRL_FIX_CNT_LEN)-1)<<DMA_CTRL_FIX_CNT_POS))
#define DMA_CTRL_SI                                             DMA_CTRL_SI
#define DMA_CTRL_SI_POS                                         (26U)
#define DMA_CTRL_SI_LEN                                         (1U)
#define DMA_CTRL_SI_MSK                                         (((1ULL<<DMA_CTRL_SI_LEN)-1)<<DMA_CTRL_SI_POS)
#define DMA_CTRL_SI_UMSK                                        (~(((1ULL<<DMA_CTRL_SI_LEN)-1)<<DMA_CTRL_SI_POS))
#define DMA_CTRL_DI                                             DMA_CTRL_DI
#define DMA_CTRL_DI_POS                                         (27U)
#define DMA_CTRL_DI_LEN                                         (1U)
#define DMA_CTRL_DI_MSK                                         (((1ULL<<DMA_CTRL_DI_LEN)-1)<<DMA_CTRL_DI_POS)
#define DMA_CTRL_DI_UMSK                                        (~(((1ULL<<DMA_CTRL_DI_LEN)-1)<<DMA_CTRL_DI_POS))
#define DMA_CTRL_PROT                                           DMA_CTRL_PROT
#define DMA_CTRL_PROT_POS                                       (28U)
#define DMA_CTRL_PROT_LEN                                       (3U)
#define DMA_CTRL_PROT_MSK                                       (((1ULL<<DMA_CTRL_PROT_LEN)-1)<<DMA_CTRL_PROT_POS)
#define DMA_CTRL_PROT_UMSK                                      (~(((1ULL<<DMA_CTRL_PROT_LEN)-1)<<DMA_CTRL_PROT_POS))
#define DMA_CTRL_I                                              DMA_CTRL_I
#define DMA_CTRL_I_POS                                          (31U)
#define DMA_CTRL_I_LEN                                          (1U)
#define DMA_CTRL_I_MSK                                          (((1ULL<<DMA_CTRL_I_LEN)-1)<<DMA_CTRL_I_POS)
#define DMA_CTRL_I_UMSK                                         (~(((1ULL<<DMA_CTRL_I_LEN)-1)<<DMA_CTRL_I_POS))

/* 0x110 : DMA_C0Config */
#define DMA_CTRL_DMA_C0CONFIG_OFFSET                            (0x110)
#define DMA_CTRL_E                                              DMA_CTRL_E
#define DMA_CTRL_E_POS                                          (0U)
#define DMA_CTRL_E_LEN                                          (1U)
#define DMA_CTRL_E_MSK                                          (((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS)
#define DMA_CTRL_E_UMSK                                         (~(((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS))
#define DMA_CTRL_SRCPERIPHERAL                                  DMA_CTRL_SRCPERIPHERAL
#define DMA_CTRL_SRCPERIPHERAL_POS                              (1U)
#define DMA_CTRL_SRCPERIPHERAL_LEN                              (5U)
#define DMA_CTRL_SRCPERIPHERAL_MSK                              (((1ULL<<DMA_CTRL_SRCPERIPHERAL_LEN)-1)<<DMA_CTRL_SRCPERIPHERAL_POS)
#define DMA_CTRL_SRCPERIPHERAL_UMSK                             (~(((1ULL<<DMA_CTRL_SRCPERIPHERAL_LEN)-1)<<DMA_CTRL_SRCPERIPHERAL_POS))
#define DMA_CTRL_DSTPERIPHERAL                                  DMA_CTRL_DSTPERIPHERAL
#define DMA_CTRL_DSTPERIPHERAL_POS                              (6U)
#define DMA_CTRL_DSTPERIPHERAL_LEN                              (5U)
#define DMA_CTRL_DSTPERIPHERAL_MSK                              (((1ULL<<DMA_CTRL_DSTPERIPHERAL_LEN)-1)<<DMA_CTRL_DSTPERIPHERAL_POS)
#define DMA_CTRL_DSTPERIPHERAL_UMSK                             (~(((1ULL<<DMA_CTRL_DSTPERIPHERAL_LEN)-1)<<DMA_CTRL_DSTPERIPHERAL_POS))
#define DMA_CTRL_FLOWCNTRL                                      DMA_CTRL_FLOWCNTRL
#define DMA_CTRL_FLOWCNTRL_POS                                  (11U)
#define DMA_CTRL_FLOWCNTRL_LEN                                  (3U)
#define DMA_CTRL_FLOWCNTRL_MSK                                  (((1ULL<<DMA_CTRL_FLOWCNTRL_LEN)-1)<<DMA_CTRL_FLOWCNTRL_POS)
#define DMA_CTRL_FLOWCNTRL_UMSK                                 (~(((1ULL<<DMA_CTRL_FLOWCNTRL_LEN)-1)<<DMA_CTRL_FLOWCNTRL_POS))
#define DMA_CTRL_IE                                             DMA_CTRL_IE
#define DMA_CTRL_IE_POS                                         (14U)
#define DMA_CTRL_IE_LEN                                         (1U)
#define DMA_CTRL_IE_MSK                                         (((1ULL<<DMA_CTRL_IE_LEN)-1)<<DMA_CTRL_IE_POS)
#define DMA_CTRL_IE_UMSK                                        (~(((1ULL<<DMA_CTRL_IE_LEN)-1)<<DMA_CTRL_IE_POS))
#define DMA_CTRL_ITC                                            DMA_CTRL_ITC
#define DMA_CTRL_ITC_POS                                        (15U)
#define DMA_CTRL_ITC_LEN                                        (1U)
#define DMA_CTRL_ITC_MSK                                        (((1ULL<<DMA_CTRL_ITC_LEN)-1)<<DMA_CTRL_ITC_POS)
#define DMA_CTRL_ITC_UMSK                                       (~(((1ULL<<DMA_CTRL_ITC_LEN)-1)<<DMA_CTRL_ITC_POS))
#define DMA_CTRL_L                                              DMA_CTRL_L
#define DMA_CTRL_L_POS                                          (16U)
#define DMA_CTRL_L_LEN                                          (1U)
#define DMA_CTRL_L_MSK                                          (((1ULL<<DMA_CTRL_L_LEN)-1)<<DMA_CTRL_L_POS)
#define DMA_CTRL_L_UMSK                                         (~(((1ULL<<DMA_CTRL_L_LEN)-1)<<DMA_CTRL_L_POS))
#define DMA_CTRL_A                                              DMA_CTRL_A
#define DMA_CTRL_A_POS                                          (17U)
#define DMA_CTRL_A_LEN                                          (1U)
#define DMA_CTRL_A_MSK                                          (((1ULL<<DMA_CTRL_A_LEN)-1)<<DMA_CTRL_A_POS)
#define DMA_CTRL_A_UMSK                                         (~(((1ULL<<DMA_CTRL_A_LEN)-1)<<DMA_CTRL_A_POS))
#define DMA_CTRL_H                                              DMA_CTRL_H
#define DMA_CTRL_H_POS                                          (18U)
#define DMA_CTRL_H_LEN                                          (1U)
#define DMA_CTRL_H_MSK                                          (((1ULL<<DMA_CTRL_H_LEN)-1)<<DMA_CTRL_H_POS)
#define DMA_CTRL_H_UMSK                                         (~(((1ULL<<DMA_CTRL_H_LEN)-1)<<DMA_CTRL_H_POS))
#define DMA_CTRL_LLICOUNTER                                     DMA_CTRL_LLICOUNTER
#define DMA_CTRL_LLICOUNTER_POS                                 (20U)
#define DMA_CTRL_LLICOUNTER_LEN                                 (10U)
#define DMA_CTRL_LLICOUNTER_MSK                                 (((1ULL<<DMA_CTRL_LLICOUNTER_LEN)-1)<<DMA_CTRL_LLICOUNTER_POS)
#define DMA_CTRL_LLICOUNTER_UMSK                                (~(((1ULL<<DMA_CTRL_LLICOUNTER_LEN)-1)<<DMA_CTRL_LLICOUNTER_POS))

/* 0x11C : DMA_C0RSVD */
#define DMA_CTRL_DMA_C0RSVD_OFFSET                              (0x11C)
#define DMA_CTRL_DSTREMNSGLE                                    DMA_CTRL_DSTREMNSGLE
#define DMA_CTRL_DSTREMNSGLE_POS                                (3U)
#define DMA_CTRL_DSTREMNSGLE_LEN                                (1U)
#define DMA_CTRL_DSTREMNSGLE_MSK                                (((1ULL<<DMA_CTRL_DSTREMNSGLE_LEN)-1)<<DMA_CTRL_DSTREMNSGLE_POS)
#define DMA_CTRL_DSTREMNSGLE_UMSK                               (~(((1ULL<<DMA_CTRL_DSTREMNSGLE_LEN)-1)<<DMA_CTRL_DSTREMNSGLE_POS))
#define DMA_CTRL_SRCREMNSGLE                                    DMA_CTRL_SRCREMNSGLE
#define DMA_CTRL_SRCREMNSGLE_POS                                (4U)
#define DMA_CTRL_SRCREMNSGLE_LEN                                (1U)
#define DMA_CTRL_SRCREMNSGLE_MSK                                (((1ULL<<DMA_CTRL_SRCREMNSGLE_LEN)-1)<<DMA_CTRL_SRCREMNSGLE_POS)
#define DMA_CTRL_SRCREMNSGLE_UMSK                               (~(((1ULL<<DMA_CTRL_SRCREMNSGLE_LEN)-1)<<DMA_CTRL_SRCREMNSGLE_POS))

/* 0x200 : DMA_C1SrcAddr */
#define DMA_CTRL_DMA_C1SRCADDR_OFFSET                           (0x200)
#define DMA_CTRL_SRCADDR                                        DMA_CTRL_SRCADDR
#define DMA_CTRL_SRCADDR_POS                                    (0U)
#define DMA_CTRL_SRCADDR_LEN                                    (32U)
#define DMA_CTRL_SRCADDR_MSK                                    (((1ULL<<DMA_CTRL_SRCADDR_LEN)-1)<<DMA_CTRL_SRCADDR_POS)
#define DMA_CTRL_SRCADDR_UMSK                                   (~(((1ULL<<DMA_CTRL_SRCADDR_LEN)-1)<<DMA_CTRL_SRCADDR_POS))

/* 0x204 : DMA_C1DstAddr */
#define DMA_CTRL_DMA_C1DSTADDR_OFFSET                           (0x204)
#define DMA_CTRL_DSTADDR                                        DMA_CTRL_DSTADDR
#define DMA_CTRL_DSTADDR_POS                                    (0U)
#define DMA_CTRL_DSTADDR_LEN                                    (32U)
#define DMA_CTRL_DSTADDR_MSK                                    (((1ULL<<DMA_CTRL_DSTADDR_LEN)-1)<<DMA_CTRL_DSTADDR_POS)
#define DMA_CTRL_DSTADDR_UMSK                                   (~(((1ULL<<DMA_CTRL_DSTADDR_LEN)-1)<<DMA_CTRL_DSTADDR_POS))

/* 0x208 : DMA_C1LLI */
#define DMA_CTRL_DMA_C1LLI_OFFSET                               (0x208)
#define DMA_CTRL_LLI                                            DMA_CTRL_LLI
#define DMA_CTRL_LLI_POS                                        (0U)
#define DMA_CTRL_LLI_LEN                                        (32U)
#define DMA_CTRL_LLI_MSK                                        (((1ULL<<DMA_CTRL_LLI_LEN)-1)<<DMA_CTRL_LLI_POS)
#define DMA_CTRL_LLI_UMSK                                       (~(((1ULL<<DMA_CTRL_LLI_LEN)-1)<<DMA_CTRL_LLI_POS))

/* 0x20C : DMA_C1Control */
#define DMA_CTRL_DMA_C1CONTROL_OFFSET                           (0x20C)
#define DMA_CTRL_TRANSFERSIZE                                   DMA_CTRL_TRANSFERSIZE
#define DMA_CTRL_TRANSFERSIZE_POS                               (0U)
#define DMA_CTRL_TRANSFERSIZE_LEN                               (12U)
#define DMA_CTRL_TRANSFERSIZE_MSK                               (((1ULL<<DMA_CTRL_TRANSFERSIZE_LEN)-1)<<DMA_CTRL_TRANSFERSIZE_POS)
#define DMA_CTRL_TRANSFERSIZE_UMSK                              (~(((1ULL<<DMA_CTRL_TRANSFERSIZE_LEN)-1)<<DMA_CTRL_TRANSFERSIZE_POS))
#define DMA_CTRL_SBSIZE                                         DMA_CTRL_SBSIZE
#define DMA_CTRL_SBSIZE_POS                                     (12U)
#define DMA_CTRL_SBSIZE_LEN                                     (2U)
#define DMA_CTRL_SBSIZE_MSK                                     (((1ULL<<DMA_CTRL_SBSIZE_LEN)-1)<<DMA_CTRL_SBSIZE_POS)
#define DMA_CTRL_SBSIZE_UMSK                                    (~(((1ULL<<DMA_CTRL_SBSIZE_LEN)-1)<<DMA_CTRL_SBSIZE_POS))
#define DMA_CTRL_DST_MIN_MODE                                   DMA_CTRL_DST_MIN_MODE
#define DMA_CTRL_DST_MIN_MODE_POS                               (14U)
#define DMA_CTRL_DST_MIN_MODE_LEN                               (1U)
#define DMA_CTRL_DST_MIN_MODE_MSK                               (((1ULL<<DMA_CTRL_DST_MIN_MODE_LEN)-1)<<DMA_CTRL_DST_MIN_MODE_POS)
#define DMA_CTRL_DST_MIN_MODE_UMSK                              (~(((1ULL<<DMA_CTRL_DST_MIN_MODE_LEN)-1)<<DMA_CTRL_DST_MIN_MODE_POS))
#define DMA_CTRL_DBSIZE                                         DMA_CTRL_DBSIZE
#define DMA_CTRL_DBSIZE_POS                                     (15U)
#define DMA_CTRL_DBSIZE_LEN                                     (2U)
#define DMA_CTRL_DBSIZE_MSK                                     (((1ULL<<DMA_CTRL_DBSIZE_LEN)-1)<<DMA_CTRL_DBSIZE_POS)
#define DMA_CTRL_DBSIZE_UMSK                                    (~(((1ULL<<DMA_CTRL_DBSIZE_LEN)-1)<<DMA_CTRL_DBSIZE_POS))
#define DMA_CTRL_DST_ADD_MODE                                   DMA_CTRL_DST_ADD_MODE
#define DMA_CTRL_DST_ADD_MODE_POS                               (17U)
#define DMA_CTRL_DST_ADD_MODE_LEN                               (1U)
#define DMA_CTRL_DST_ADD_MODE_MSK                               (((1ULL<<DMA_CTRL_DST_ADD_MODE_LEN)-1)<<DMA_CTRL_DST_ADD_MODE_POS)
#define DMA_CTRL_DST_ADD_MODE_UMSK                              (~(((1ULL<<DMA_CTRL_DST_ADD_MODE_LEN)-1)<<DMA_CTRL_DST_ADD_MODE_POS))
#define DMA_CTRL_SWIDTH                                         DMA_CTRL_SWIDTH
#define DMA_CTRL_SWIDTH_POS                                     (18U)
#define DMA_CTRL_SWIDTH_LEN                                     (2U)
#define DMA_CTRL_SWIDTH_MSK                                     (((1ULL<<DMA_CTRL_SWIDTH_LEN)-1)<<DMA_CTRL_SWIDTH_POS)
#define DMA_CTRL_SWIDTH_UMSK                                    (~(((1ULL<<DMA_CTRL_SWIDTH_LEN)-1)<<DMA_CTRL_SWIDTH_POS))
#define DMA_CTRL_DWIDTH                                         DMA_CTRL_DWIDTH
#define DMA_CTRL_DWIDTH_POS                                     (21U)
#define DMA_CTRL_DWIDTH_LEN                                     (2U)
#define DMA_CTRL_DWIDTH_MSK                                     (((1ULL<<DMA_CTRL_DWIDTH_LEN)-1)<<DMA_CTRL_DWIDTH_POS)
#define DMA_CTRL_DWIDTH_UMSK                                    (~(((1ULL<<DMA_CTRL_DWIDTH_LEN)-1)<<DMA_CTRL_DWIDTH_POS))
#define DMA_CTRL_FIX_CNT                                        DMA_CTRL_FIX_CNT
#define DMA_CTRL_FIX_CNT_POS                                    (23U)
#define DMA_CTRL_FIX_CNT_LEN                                    (3U)
#define DMA_CTRL_FIX_CNT_MSK                                    (((1ULL<<DMA_CTRL_FIX_CNT_LEN)-1)<<DMA_CTRL_FIX_CNT_POS)
#define DMA_CTRL_FIX_CNT_UMSK                                   (~(((1ULL<<DMA_CTRL_FIX_CNT_LEN)-1)<<DMA_CTRL_FIX_CNT_POS))
#define DMA_CTRL_SI                                             DMA_CTRL_SI
#define DMA_CTRL_SI_POS                                         (26U)
#define DMA_CTRL_SI_LEN                                         (1U)
#define DMA_CTRL_SI_MSK                                         (((1ULL<<DMA_CTRL_SI_LEN)-1)<<DMA_CTRL_SI_POS)
#define DMA_CTRL_SI_UMSK                                        (~(((1ULL<<DMA_CTRL_SI_LEN)-1)<<DMA_CTRL_SI_POS))
#define DMA_CTRL_DI                                             DMA_CTRL_DI
#define DMA_CTRL_DI_POS                                         (27U)
#define DMA_CTRL_DI_LEN                                         (1U)
#define DMA_CTRL_DI_MSK                                         (((1ULL<<DMA_CTRL_DI_LEN)-1)<<DMA_CTRL_DI_POS)
#define DMA_CTRL_DI_UMSK                                        (~(((1ULL<<DMA_CTRL_DI_LEN)-1)<<DMA_CTRL_DI_POS))
#define DMA_CTRL_PROT                                           DMA_CTRL_PROT
#define DMA_CTRL_PROT_POS                                       (28U)
#define DMA_CTRL_PROT_LEN                                       (3U)
#define DMA_CTRL_PROT_MSK                                       (((1ULL<<DMA_CTRL_PROT_LEN)-1)<<DMA_CTRL_PROT_POS)
#define DMA_CTRL_PROT_UMSK                                      (~(((1ULL<<DMA_CTRL_PROT_LEN)-1)<<DMA_CTRL_PROT_POS))
#define DMA_CTRL_I                                              DMA_CTRL_I
#define DMA_CTRL_I_POS                                          (31U)
#define DMA_CTRL_I_LEN                                          (1U)
#define DMA_CTRL_I_MSK                                          (((1ULL<<DMA_CTRL_I_LEN)-1)<<DMA_CTRL_I_POS)
#define DMA_CTRL_I_UMSK                                         (~(((1ULL<<DMA_CTRL_I_LEN)-1)<<DMA_CTRL_I_POS))

/* 0x210 : DMA_C1Config */
#define DMA_CTRL_DMA_C1CONFIG_OFFSET                            (0x210)
#define DMA_CTRL_E                                              DMA_CTRL_E
#define DMA_CTRL_E_POS                                          (0U)
#define DMA_CTRL_E_LEN                                          (1U)
#define DMA_CTRL_E_MSK                                          (((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS)
#define DMA_CTRL_E_UMSK                                         (~(((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS))
#define DMA_CTRL_SRCPERIPHERAL                                  DMA_CTRL_SRCPERIPHERAL
#define DMA_CTRL_SRCPERIPHERAL_POS                              (1U)
#define DMA_CTRL_SRCPERIPHERAL_LEN                              (5U)
#define DMA_CTRL_SRCPERIPHERAL_MSK                              (((1ULL<<DMA_CTRL_SRCPERIPHERAL_LEN)-1)<<DMA_CTRL_SRCPERIPHERAL_POS)
#define DMA_CTRL_SRCPERIPHERAL_UMSK                             (~(((1ULL<<DMA_CTRL_SRCPERIPHERAL_LEN)-1)<<DMA_CTRL_SRCPERIPHERAL_POS))
#define DMA_CTRL_DSTPERIPHERAL                                  DMA_CTRL_DSTPERIPHERAL
#define DMA_CTRL_DSTPERIPHERAL_POS                              (6U)
#define DMA_CTRL_DSTPERIPHERAL_LEN                              (5U)
#define DMA_CTRL_DSTPERIPHERAL_MSK                              (((1ULL<<DMA_CTRL_DSTPERIPHERAL_LEN)-1)<<DMA_CTRL_DSTPERIPHERAL_POS)
#define DMA_CTRL_DSTPERIPHERAL_UMSK                             (~(((1ULL<<DMA_CTRL_DSTPERIPHERAL_LEN)-1)<<DMA_CTRL_DSTPERIPHERAL_POS))
#define DMA_CTRL_FLOWCNTRL                                      DMA_CTRL_FLOWCNTRL
#define DMA_CTRL_FLOWCNTRL_POS                                  (11U)
#define DMA_CTRL_FLOWCNTRL_LEN                                  (3U)
#define DMA_CTRL_FLOWCNTRL_MSK                                  (((1ULL<<DMA_CTRL_FLOWCNTRL_LEN)-1)<<DMA_CTRL_FLOWCNTRL_POS)
#define DMA_CTRL_FLOWCNTRL_UMSK                                 (~(((1ULL<<DMA_CTRL_FLOWCNTRL_LEN)-1)<<DMA_CTRL_FLOWCNTRL_POS))
#define DMA_CTRL_IE                                             DMA_CTRL_IE
#define DMA_CTRL_IE_POS                                         (14U)
#define DMA_CTRL_IE_LEN                                         (1U)
#define DMA_CTRL_IE_MSK                                         (((1ULL<<DMA_CTRL_IE_LEN)-1)<<DMA_CTRL_IE_POS)
#define DMA_CTRL_IE_UMSK                                        (~(((1ULL<<DMA_CTRL_IE_LEN)-1)<<DMA_CTRL_IE_POS))
#define DMA_CTRL_ITC                                            DMA_CTRL_ITC
#define DMA_CTRL_ITC_POS                                        (15U)
#define DMA_CTRL_ITC_LEN                                        (1U)
#define DMA_CTRL_ITC_MSK                                        (((1ULL<<DMA_CTRL_ITC_LEN)-1)<<DMA_CTRL_ITC_POS)
#define DMA_CTRL_ITC_UMSK                                       (~(((1ULL<<DMA_CTRL_ITC_LEN)-1)<<DMA_CTRL_ITC_POS))
#define DMA_CTRL_L                                              DMA_CTRL_L
#define DMA_CTRL_L_POS                                          (16U)
#define DMA_CTRL_L_LEN                                          (1U)
#define DMA_CTRL_L_MSK                                          (((1ULL<<DMA_CTRL_L_LEN)-1)<<DMA_CTRL_L_POS)
#define DMA_CTRL_L_UMSK                                         (~(((1ULL<<DMA_CTRL_L_LEN)-1)<<DMA_CTRL_L_POS))
#define DMA_CTRL_A                                              DMA_CTRL_A
#define DMA_CTRL_A_POS                                          (17U)
#define DMA_CTRL_A_LEN                                          (1U)
#define DMA_CTRL_A_MSK                                          (((1ULL<<DMA_CTRL_A_LEN)-1)<<DMA_CTRL_A_POS)
#define DMA_CTRL_A_UMSK                                         (~(((1ULL<<DMA_CTRL_A_LEN)-1)<<DMA_CTRL_A_POS))
#define DMA_CTRL_H                                              DMA_CTRL_H
#define DMA_CTRL_H_POS                                          (18U)
#define DMA_CTRL_H_LEN                                          (1U)
#define DMA_CTRL_H_MSK                                          (((1ULL<<DMA_CTRL_H_LEN)-1)<<DMA_CTRL_H_POS)
#define DMA_CTRL_H_UMSK                                         (~(((1ULL<<DMA_CTRL_H_LEN)-1)<<DMA_CTRL_H_POS))

/* 0x21C : DMA_C1RSVD */
#define DMA_CTRL_DMA_C1RSVD_OFFSET                              (0x21C)
#define DMA_CTRL_DSTREMNSGLE                                    DMA_CTRL_DSTREMNSGLE
#define DMA_CTRL_DSTREMNSGLE_POS                                (3U)
#define DMA_CTRL_DSTREMNSGLE_LEN                                (1U)
#define DMA_CTRL_DSTREMNSGLE_MSK                                (((1ULL<<DMA_CTRL_DSTREMNSGLE_LEN)-1)<<DMA_CTRL_DSTREMNSGLE_POS)
#define DMA_CTRL_DSTREMNSGLE_UMSK                               (~(((1ULL<<DMA_CTRL_DSTREMNSGLE_LEN)-1)<<DMA_CTRL_DSTREMNSGLE_POS))
#define DMA_CTRL_SRCREMNSGLE                                    DMA_CTRL_SRCREMNSGLE
#define DMA_CTRL_SRCREMNSGLE_POS                                (4U)
#define DMA_CTRL_SRCREMNSGLE_LEN                                (1U)
#define DMA_CTRL_SRCREMNSGLE_MSK                                (((1ULL<<DMA_CTRL_SRCREMNSGLE_LEN)-1)<<DMA_CTRL_SRCREMNSGLE_POS)
#define DMA_CTRL_SRCREMNSGLE_UMSK                               (~(((1ULL<<DMA_CTRL_SRCREMNSGLE_LEN)-1)<<DMA_CTRL_SRCREMNSGLE_POS))

/* 0x300 : DMA_C2SrcAddr */
#define DMA_CTRL_DMA_C2SRCADDR_OFFSET                           (0x300)
#define DMA_CTRL_SRCADDR                                        DMA_CTRL_SRCADDR
#define DMA_CTRL_SRCADDR_POS                                    (0U)
#define DMA_CTRL_SRCADDR_LEN                                    (32U)
#define DMA_CTRL_SRCADDR_MSK                                    (((1ULL<<DMA_CTRL_SRCADDR_LEN)-1)<<DMA_CTRL_SRCADDR_POS)
#define DMA_CTRL_SRCADDR_UMSK                                   (~(((1ULL<<DMA_CTRL_SRCADDR_LEN)-1)<<DMA_CTRL_SRCADDR_POS))

/* 0x304 : DMA_C2DstAddr */
#define DMA_CTRL_DMA_C2DSTADDR_OFFSET                           (0x304)
#define DMA_CTRL_DSTADDR                                        DMA_CTRL_DSTADDR
#define DMA_CTRL_DSTADDR_POS                                    (0U)
#define DMA_CTRL_DSTADDR_LEN                                    (32U)
#define DMA_CTRL_DSTADDR_MSK                                    (((1ULL<<DMA_CTRL_DSTADDR_LEN)-1)<<DMA_CTRL_DSTADDR_POS)
#define DMA_CTRL_DSTADDR_UMSK                                   (~(((1ULL<<DMA_CTRL_DSTADDR_LEN)-1)<<DMA_CTRL_DSTADDR_POS))

/* 0x308 : DMA_C2LLI */
#define DMA_CTRL_DMA_C2LLI_OFFSET                               (0x308)
#define DMA_CTRL_LLI                                            DMA_CTRL_LLI
#define DMA_CTRL_LLI_POS                                        (0U)
#define DMA_CTRL_LLI_LEN                                        (32U)
#define DMA_CTRL_LLI_MSK                                        (((1ULL<<DMA_CTRL_LLI_LEN)-1)<<DMA_CTRL_LLI_POS)
#define DMA_CTRL_LLI_UMSK                                       (~(((1ULL<<DMA_CTRL_LLI_LEN)-1)<<DMA_CTRL_LLI_POS))

/* 0x30C : DMA_C2Control */
#define DMA_CTRL_DMA_C2CONTROL_OFFSET                           (0x30C)
#define DMA_CTRL_TRANSFERSIZE                                   DMA_CTRL_TRANSFERSIZE
#define DMA_CTRL_TRANSFERSIZE_POS                               (0U)
#define DMA_CTRL_TRANSFERSIZE_LEN                               (12U)
#define DMA_CTRL_TRANSFERSIZE_MSK                               (((1ULL<<DMA_CTRL_TRANSFERSIZE_LEN)-1)<<DMA_CTRL_TRANSFERSIZE_POS)
#define DMA_CTRL_TRANSFERSIZE_UMSK                              (~(((1ULL<<DMA_CTRL_TRANSFERSIZE_LEN)-1)<<DMA_CTRL_TRANSFERSIZE_POS))
#define DMA_CTRL_SBSIZE                                         DMA_CTRL_SBSIZE
#define DMA_CTRL_SBSIZE_POS                                     (12U)
#define DMA_CTRL_SBSIZE_LEN                                     (2U)
#define DMA_CTRL_SBSIZE_MSK                                     (((1ULL<<DMA_CTRL_SBSIZE_LEN)-1)<<DMA_CTRL_SBSIZE_POS)
#define DMA_CTRL_SBSIZE_UMSK                                    (~(((1ULL<<DMA_CTRL_SBSIZE_LEN)-1)<<DMA_CTRL_SBSIZE_POS))
#define DMA_CTRL_DST_MIN_MODE                                   DMA_CTRL_DST_MIN_MODE
#define DMA_CTRL_DST_MIN_MODE_POS                               (14U)
#define DMA_CTRL_DST_MIN_MODE_LEN                               (1U)
#define DMA_CTRL_DST_MIN_MODE_MSK                               (((1ULL<<DMA_CTRL_DST_MIN_MODE_LEN)-1)<<DMA_CTRL_DST_MIN_MODE_POS)
#define DMA_CTRL_DST_MIN_MODE_UMSK                              (~(((1ULL<<DMA_CTRL_DST_MIN_MODE_LEN)-1)<<DMA_CTRL_DST_MIN_MODE_POS))
#define DMA_CTRL_DBSIZE                                         DMA_CTRL_DBSIZE
#define DMA_CTRL_DBSIZE_POS                                     (15U)
#define DMA_CTRL_DBSIZE_LEN                                     (2U)
#define DMA_CTRL_DBSIZE_MSK                                     (((1ULL<<DMA_CTRL_DBSIZE_LEN)-1)<<DMA_CTRL_DBSIZE_POS)
#define DMA_CTRL_DBSIZE_UMSK                                    (~(((1ULL<<DMA_CTRL_DBSIZE_LEN)-1)<<DMA_CTRL_DBSIZE_POS))
#define DMA_CTRL_DST_ADD_MODE                                   DMA_CTRL_DST_ADD_MODE
#define DMA_CTRL_DST_ADD_MODE_POS                               (17U)
#define DMA_CTRL_DST_ADD_MODE_LEN                               (1U)
#define DMA_CTRL_DST_ADD_MODE_MSK                               (((1ULL<<DMA_CTRL_DST_ADD_MODE_LEN)-1)<<DMA_CTRL_DST_ADD_MODE_POS)
#define DMA_CTRL_DST_ADD_MODE_UMSK                              (~(((1ULL<<DMA_CTRL_DST_ADD_MODE_LEN)-1)<<DMA_CTRL_DST_ADD_MODE_POS))
#define DMA_CTRL_SWIDTH                                         DMA_CTRL_SWIDTH
#define DMA_CTRL_SWIDTH_POS                                     (18U)
#define DMA_CTRL_SWIDTH_LEN                                     (2U)
#define DMA_CTRL_SWIDTH_MSK                                     (((1ULL<<DMA_CTRL_SWIDTH_LEN)-1)<<DMA_CTRL_SWIDTH_POS)
#define DMA_CTRL_SWIDTH_UMSK                                    (~(((1ULL<<DMA_CTRL_SWIDTH_LEN)-1)<<DMA_CTRL_SWIDTH_POS))
#define DMA_CTRL_DWIDTH                                         DMA_CTRL_DWIDTH
#define DMA_CTRL_DWIDTH_POS                                     (21U)
#define DMA_CTRL_DWIDTH_LEN                                     (2U)
#define DMA_CTRL_DWIDTH_MSK                                     (((1ULL<<DMA_CTRL_DWIDTH_LEN)-1)<<DMA_CTRL_DWIDTH_POS)
#define DMA_CTRL_DWIDTH_UMSK                                    (~(((1ULL<<DMA_CTRL_DWIDTH_LEN)-1)<<DMA_CTRL_DWIDTH_POS))
#define DMA_CTRL_FIX_CNT                                        DMA_CTRL_FIX_CNT
#define DMA_CTRL_FIX_CNT_POS                                    (23U)
#define DMA_CTRL_FIX_CNT_LEN                                    (3U)
#define DMA_CTRL_FIX_CNT_MSK                                    (((1ULL<<DMA_CTRL_FIX_CNT_LEN)-1)<<DMA_CTRL_FIX_CNT_POS)
#define DMA_CTRL_FIX_CNT_UMSK                                   (~(((1ULL<<DMA_CTRL_FIX_CNT_LEN)-1)<<DMA_CTRL_FIX_CNT_POS))
#define DMA_CTRL_SI                                             DMA_CTRL_SI
#define DMA_CTRL_SI_POS                                         (26U)
#define DMA_CTRL_SI_LEN                                         (1U)
#define DMA_CTRL_SI_MSK                                         (((1ULL<<DMA_CTRL_SI_LEN)-1)<<DMA_CTRL_SI_POS)
#define DMA_CTRL_SI_UMSK                                        (~(((1ULL<<DMA_CTRL_SI_LEN)-1)<<DMA_CTRL_SI_POS))
#define DMA_CTRL_DI                                             DMA_CTRL_DI
#define DMA_CTRL_DI_POS                                         (27U)
#define DMA_CTRL_DI_LEN                                         (1U)
#define DMA_CTRL_DI_MSK                                         (((1ULL<<DMA_CTRL_DI_LEN)-1)<<DMA_CTRL_DI_POS)
#define DMA_CTRL_DI_UMSK                                        (~(((1ULL<<DMA_CTRL_DI_LEN)-1)<<DMA_CTRL_DI_POS))
#define DMA_CTRL_PROT                                           DMA_CTRL_PROT
#define DMA_CTRL_PROT_POS                                       (28U)
#define DMA_CTRL_PROT_LEN                                       (3U)
#define DMA_CTRL_PROT_MSK                                       (((1ULL<<DMA_CTRL_PROT_LEN)-1)<<DMA_CTRL_PROT_POS)
#define DMA_CTRL_PROT_UMSK                                      (~(((1ULL<<DMA_CTRL_PROT_LEN)-1)<<DMA_CTRL_PROT_POS))
#define DMA_CTRL_I                                              DMA_CTRL_I
#define DMA_CTRL_I_POS                                          (31U)
#define DMA_CTRL_I_LEN                                          (1U)
#define DMA_CTRL_I_MSK                                          (((1ULL<<DMA_CTRL_I_LEN)-1)<<DMA_CTRL_I_POS)
#define DMA_CTRL_I_UMSK                                         (~(((1ULL<<DMA_CTRL_I_LEN)-1)<<DMA_CTRL_I_POS))

/* 0x310 : DMA_C2Config */
#define DMA_CTRL_DMA_C2CONFIG_OFFSET                            (0x310)
#define DMA_CTRL_E                                              DMA_CTRL_E
#define DMA_CTRL_E_POS                                          (0U)
#define DMA_CTRL_E_LEN                                          (1U)
#define DMA_CTRL_E_MSK                                          (((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS)
#define DMA_CTRL_E_UMSK                                         (~(((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS))
#define DMA_CTRL_SRCPERIPHERAL                                  DMA_CTRL_SRCPERIPHERAL
#define DMA_CTRL_SRCPERIPHERAL_POS                              (1U)
#define DMA_CTRL_SRCPERIPHERAL_LEN                              (5U)
#define DMA_CTRL_SRCPERIPHERAL_MSK                              (((1ULL<<DMA_CTRL_SRCPERIPHERAL_LEN)-1)<<DMA_CTRL_SRCPERIPHERAL_POS)
#define DMA_CTRL_SRCPERIPHERAL_UMSK                             (~(((1ULL<<DMA_CTRL_SRCPERIPHERAL_LEN)-1)<<DMA_CTRL_SRCPERIPHERAL_POS))
#define DMA_CTRL_DSTPERIPHERAL                                  DMA_CTRL_DSTPERIPHERAL
#define DMA_CTRL_DSTPERIPHERAL_POS                              (6U)
#define DMA_CTRL_DSTPERIPHERAL_LEN                              (5U)
#define DMA_CTRL_DSTPERIPHERAL_MSK                              (((1ULL<<DMA_CTRL_DSTPERIPHERAL_LEN)-1)<<DMA_CTRL_DSTPERIPHERAL_POS)
#define DMA_CTRL_DSTPERIPHERAL_UMSK                             (~(((1ULL<<DMA_CTRL_DSTPERIPHERAL_LEN)-1)<<DMA_CTRL_DSTPERIPHERAL_POS))
#define DMA_CTRL_FLOWCNTRL                                      DMA_CTRL_FLOWCNTRL
#define DMA_CTRL_FLOWCNTRL_POS                                  (11U)
#define DMA_CTRL_FLOWCNTRL_LEN                                  (3U)
#define DMA_CTRL_FLOWCNTRL_MSK                                  (((1ULL<<DMA_CTRL_FLOWCNTRL_LEN)-1)<<DMA_CTRL_FLOWCNTRL_POS)
#define DMA_CTRL_FLOWCNTRL_UMSK                                 (~(((1ULL<<DMA_CTRL_FLOWCNTRL_LEN)-1)<<DMA_CTRL_FLOWCNTRL_POS))
#define DMA_CTRL_IE                                             DMA_CTRL_IE
#define DMA_CTRL_IE_POS                                         (14U)
#define DMA_CTRL_IE_LEN                                         (1U)
#define DMA_CTRL_IE_MSK                                         (((1ULL<<DMA_CTRL_IE_LEN)-1)<<DMA_CTRL_IE_POS)
#define DMA_CTRL_IE_UMSK                                        (~(((1ULL<<DMA_CTRL_IE_LEN)-1)<<DMA_CTRL_IE_POS))
#define DMA_CTRL_ITC                                            DMA_CTRL_ITC
#define DMA_CTRL_ITC_POS                                        (15U)
#define DMA_CTRL_ITC_LEN                                        (1U)
#define DMA_CTRL_ITC_MSK                                        (((1ULL<<DMA_CTRL_ITC_LEN)-1)<<DMA_CTRL_ITC_POS)
#define DMA_CTRL_ITC_UMSK                                       (~(((1ULL<<DMA_CTRL_ITC_LEN)-1)<<DMA_CTRL_ITC_POS))
#define DMA_CTRL_L                                              DMA_CTRL_L
#define DMA_CTRL_L_POS                                          (16U)
#define DMA_CTRL_L_LEN                                          (1U)
#define DMA_CTRL_L_MSK                                          (((1ULL<<DMA_CTRL_L_LEN)-1)<<DMA_CTRL_L_POS)
#define DMA_CTRL_L_UMSK                                         (~(((1ULL<<DMA_CTRL_L_LEN)-1)<<DMA_CTRL_L_POS))
#define DMA_CTRL_A                                              DMA_CTRL_A
#define DMA_CTRL_A_POS                                          (17U)
#define DMA_CTRL_A_LEN                                          (1U)
#define DMA_CTRL_A_MSK                                          (((1ULL<<DMA_CTRL_A_LEN)-1)<<DMA_CTRL_A_POS)
#define DMA_CTRL_A_UMSK                                         (~(((1ULL<<DMA_CTRL_A_LEN)-1)<<DMA_CTRL_A_POS))
#define DMA_CTRL_H                                              DMA_CTRL_H
#define DMA_CTRL_H_POS                                          (18U)
#define DMA_CTRL_H_LEN                                          (1U)
#define DMA_CTRL_H_MSK                                          (((1ULL<<DMA_CTRL_H_LEN)-1)<<DMA_CTRL_H_POS)
#define DMA_CTRL_H_UMSK                                         (~(((1ULL<<DMA_CTRL_H_LEN)-1)<<DMA_CTRL_H_POS))

/* 0x31C : DMA_C2RSVD */
#define DMA_CTRL_DMA_C2RSVD_OFFSET                              (0x31C)
#define DMA_CTRL_DSTREMNSGLE                                    DMA_CTRL_DSTREMNSGLE
#define DMA_CTRL_DSTREMNSGLE_POS                                (3U)
#define DMA_CTRL_DSTREMNSGLE_LEN                                (1U)
#define DMA_CTRL_DSTREMNSGLE_MSK                                (((1ULL<<DMA_CTRL_DSTREMNSGLE_LEN)-1)<<DMA_CTRL_DSTREMNSGLE_POS)
#define DMA_CTRL_DSTREMNSGLE_UMSK                               (~(((1ULL<<DMA_CTRL_DSTREMNSGLE_LEN)-1)<<DMA_CTRL_DSTREMNSGLE_POS))
#define DMA_CTRL_SRCREMNSGLE                                    DMA_CTRL_SRCREMNSGLE
#define DMA_CTRL_SRCREMNSGLE_POS                                (4U)
#define DMA_CTRL_SRCREMNSGLE_LEN                                (1U)
#define DMA_CTRL_SRCREMNSGLE_MSK                                (((1ULL<<DMA_CTRL_SRCREMNSGLE_LEN)-1)<<DMA_CTRL_SRCREMNSGLE_POS)
#define DMA_CTRL_SRCREMNSGLE_UMSK                               (~(((1ULL<<DMA_CTRL_SRCREMNSGLE_LEN)-1)<<DMA_CTRL_SRCREMNSGLE_POS))

/* 0x400 : DMA_C3SrcAddr */
#define DMA_CTRL_DMA_C3SRCADDR_OFFSET                           (0x400)
#define DMA_CTRL_SRCADDR                                        DMA_CTRL_SRCADDR
#define DMA_CTRL_SRCADDR_POS                                    (0U)
#define DMA_CTRL_SRCADDR_LEN                                    (32U)
#define DMA_CTRL_SRCADDR_MSK                                    (((1ULL<<DMA_CTRL_SRCADDR_LEN)-1)<<DMA_CTRL_SRCADDR_POS)
#define DMA_CTRL_SRCADDR_UMSK                                   (~(((1ULL<<DMA_CTRL_SRCADDR_LEN)-1)<<DMA_CTRL_SRCADDR_POS))

/* 0x404 : DMA_C3DstAddr */
#define DMA_CTRL_DMA_C3DSTADDR_OFFSET                           (0x404)
#define DMA_CTRL_DSTADDR                                        DMA_CTRL_DSTADDR
#define DMA_CTRL_DSTADDR_POS                                    (0U)
#define DMA_CTRL_DSTADDR_LEN                                    (32U)
#define DMA_CTRL_DSTADDR_MSK                                    (((1ULL<<DMA_CTRL_DSTADDR_LEN)-1)<<DMA_CTRL_DSTADDR_POS)
#define DMA_CTRL_DSTADDR_UMSK                                   (~(((1ULL<<DMA_CTRL_DSTADDR_LEN)-1)<<DMA_CTRL_DSTADDR_POS))

/* 0x408 : DMA_C3LLI */
#define DMA_CTRL_DMA_C3LLI_OFFSET                               (0x408)
#define DMA_CTRL_LLI                                            DMA_CTRL_LLI
#define DMA_CTRL_LLI_POS                                        (0U)
#define DMA_CTRL_LLI_LEN                                        (32U)
#define DMA_CTRL_LLI_MSK                                        (((1ULL<<DMA_CTRL_LLI_LEN)-1)<<DMA_CTRL_LLI_POS)
#define DMA_CTRL_LLI_UMSK                                       (~(((1ULL<<DMA_CTRL_LLI_LEN)-1)<<DMA_CTRL_LLI_POS))

/* 0x40C : DMA_C3Control */
#define DMA_CTRL_DMA_C3CONTROL_OFFSET                           (0x40C)
#define DMA_CTRL_TRANSFERSIZE                                   DMA_CTRL_TRANSFERSIZE
#define DMA_CTRL_TRANSFERSIZE_POS                               (0U)
#define DMA_CTRL_TRANSFERSIZE_LEN                               (12U)
#define DMA_CTRL_TRANSFERSIZE_MSK                               (((1ULL<<DMA_CTRL_TRANSFERSIZE_LEN)-1)<<DMA_CTRL_TRANSFERSIZE_POS)
#define DMA_CTRL_TRANSFERSIZE_UMSK                              (~(((1ULL<<DMA_CTRL_TRANSFERSIZE_LEN)-1)<<DMA_CTRL_TRANSFERSIZE_POS))
#define DMA_CTRL_SBSIZE                                         DMA_CTRL_SBSIZE
#define DMA_CTRL_SBSIZE_POS                                     (12U)
#define DMA_CTRL_SBSIZE_LEN                                     (2U)
#define DMA_CTRL_SBSIZE_MSK                                     (((1ULL<<DMA_CTRL_SBSIZE_LEN)-1)<<DMA_CTRL_SBSIZE_POS)
#define DMA_CTRL_SBSIZE_UMSK                                    (~(((1ULL<<DMA_CTRL_SBSIZE_LEN)-1)<<DMA_CTRL_SBSIZE_POS))
#define DMA_CTRL_DST_MIN_MODE                                   DMA_CTRL_DST_MIN_MODE
#define DMA_CTRL_DST_MIN_MODE_POS                               (14U)
#define DMA_CTRL_DST_MIN_MODE_LEN                               (1U)
#define DMA_CTRL_DST_MIN_MODE_MSK                               (((1ULL<<DMA_CTRL_DST_MIN_MODE_LEN)-1)<<DMA_CTRL_DST_MIN_MODE_POS)
#define DMA_CTRL_DST_MIN_MODE_UMSK                              (~(((1ULL<<DMA_CTRL_DST_MIN_MODE_LEN)-1)<<DMA_CTRL_DST_MIN_MODE_POS))
#define DMA_CTRL_DBSIZE                                         DMA_CTRL_DBSIZE
#define DMA_CTRL_DBSIZE_POS                                     (15U)
#define DMA_CTRL_DBSIZE_LEN                                     (2U)
#define DMA_CTRL_DBSIZE_MSK                                     (((1ULL<<DMA_CTRL_DBSIZE_LEN)-1)<<DMA_CTRL_DBSIZE_POS)
#define DMA_CTRL_DBSIZE_UMSK                                    (~(((1ULL<<DMA_CTRL_DBSIZE_LEN)-1)<<DMA_CTRL_DBSIZE_POS))
#define DMA_CTRL_DST_ADD_MODE                                   DMA_CTRL_DST_ADD_MODE
#define DMA_CTRL_DST_ADD_MODE_POS                               (17U)
#define DMA_CTRL_DST_ADD_MODE_LEN                               (1U)
#define DMA_CTRL_DST_ADD_MODE_MSK                               (((1ULL<<DMA_CTRL_DST_ADD_MODE_LEN)-1)<<DMA_CTRL_DST_ADD_MODE_POS)
#define DMA_CTRL_DST_ADD_MODE_UMSK                              (~(((1ULL<<DMA_CTRL_DST_ADD_MODE_LEN)-1)<<DMA_CTRL_DST_ADD_MODE_POS))
#define DMA_CTRL_SWIDTH                                         DMA_CTRL_SWIDTH
#define DMA_CTRL_SWIDTH_POS                                     (18U)
#define DMA_CTRL_SWIDTH_LEN                                     (2U)
#define DMA_CTRL_SWIDTH_MSK                                     (((1ULL<<DMA_CTRL_SWIDTH_LEN)-1)<<DMA_CTRL_SWIDTH_POS)
#define DMA_CTRL_SWIDTH_UMSK                                    (~(((1ULL<<DMA_CTRL_SWIDTH_LEN)-1)<<DMA_CTRL_SWIDTH_POS))
#define DMA_CTRL_DWIDTH                                         DMA_CTRL_DWIDTH
#define DMA_CTRL_DWIDTH_POS                                     (21U)
#define DMA_CTRL_DWIDTH_LEN                                     (2U)
#define DMA_CTRL_DWIDTH_MSK                                     (((1ULL<<DMA_CTRL_DWIDTH_LEN)-1)<<DMA_CTRL_DWIDTH_POS)
#define DMA_CTRL_DWIDTH_UMSK                                    (~(((1ULL<<DMA_CTRL_DWIDTH_LEN)-1)<<DMA_CTRL_DWIDTH_POS))
#define DMA_CTRL_FIX_CNT                                        DMA_CTRL_FIX_CNT
#define DMA_CTRL_FIX_CNT_POS                                    (23U)
#define DMA_CTRL_FIX_CNT_LEN                                    (3U)
#define DMA_CTRL_FIX_CNT_MSK                                    (((1ULL<<DMA_CTRL_FIX_CNT_LEN)-1)<<DMA_CTRL_FIX_CNT_POS)
#define DMA_CTRL_FIX_CNT_UMSK                                   (~(((1ULL<<DMA_CTRL_FIX_CNT_LEN)-1)<<DMA_CTRL_FIX_CNT_POS))
#define DMA_CTRL_SI                                             DMA_CTRL_SI
#define DMA_CTRL_SI_POS                                         (26U)
#define DMA_CTRL_SI_LEN                                         (1U)
#define DMA_CTRL_SI_MSK                                         (((1ULL<<DMA_CTRL_SI_LEN)-1)<<DMA_CTRL_SI_POS)
#define DMA_CTRL_SI_UMSK                                        (~(((1ULL<<DMA_CTRL_SI_LEN)-1)<<DMA_CTRL_SI_POS))
#define DMA_CTRL_DI                                             DMA_CTRL_DI
#define DMA_CTRL_DI_POS                                         (27U)
#define DMA_CTRL_DI_LEN                                         (1U)
#define DMA_CTRL_DI_MSK                                         (((1ULL<<DMA_CTRL_DI_LEN)-1)<<DMA_CTRL_DI_POS)
#define DMA_CTRL_DI_UMSK                                        (~(((1ULL<<DMA_CTRL_DI_LEN)-1)<<DMA_CTRL_DI_POS))
#define DMA_CTRL_PROT                                           DMA_CTRL_PROT
#define DMA_CTRL_PROT_POS                                       (28U)
#define DMA_CTRL_PROT_LEN                                       (3U)
#define DMA_CTRL_PROT_MSK                                       (((1ULL<<DMA_CTRL_PROT_LEN)-1)<<DMA_CTRL_PROT_POS)
#define DMA_CTRL_PROT_UMSK                                      (~(((1ULL<<DMA_CTRL_PROT_LEN)-1)<<DMA_CTRL_PROT_POS))
#define DMA_CTRL_I                                              DMA_CTRL_I
#define DMA_CTRL_I_POS                                          (31U)
#define DMA_CTRL_I_LEN                                          (1U)
#define DMA_CTRL_I_MSK                                          (((1ULL<<DMA_CTRL_I_LEN)-1)<<DMA_CTRL_I_POS)
#define DMA_CTRL_I_UMSK                                         (~(((1ULL<<DMA_CTRL_I_LEN)-1)<<DMA_CTRL_I_POS))

/* 0x410 : DMA_C3Config */
#define DMA_CTRL_DMA_C3CONFIG_OFFSET                            (0x410)
#define DMA_CTRL_E                                              DMA_CTRL_E
#define DMA_CTRL_E_POS                                          (0U)
#define DMA_CTRL_E_LEN                                          (1U)
#define DMA_CTRL_E_MSK                                          (((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS)
#define DMA_CTRL_E_UMSK                                         (~(((1ULL<<DMA_CTRL_E_LEN)-1)<<DMA_CTRL_E_POS))
#define DMA_CTRL_SRCPERIPHERAL                                  DMA_CTRL_SRCPERIPHERAL
#define DMA_CTRL_SRCPERIPHERAL_POS                              (1U)
#define DMA_CTRL_SRCPERIPHERAL_LEN                              (5U)
#define DMA_CTRL_SRCPERIPHERAL_MSK                              (((1ULL<<DMA_CTRL_SRCPERIPHERAL_LEN)-1)<<DMA_CTRL_SRCPERIPHERAL_POS)
#define DMA_CTRL_SRCPERIPHERAL_UMSK                             (~(((1ULL<<DMA_CTRL_SRCPERIPHERAL_LEN)-1)<<DMA_CTRL_SRCPERIPHERAL_POS))
#define DMA_CTRL_DSTPERIPHERAL                                  DMA_CTRL_DSTPERIPHERAL
#define DMA_CTRL_DSTPERIPHERAL_POS                              (6U)
#define DMA_CTRL_DSTPERIPHERAL_LEN                              (5U)
#define DMA_CTRL_DSTPERIPHERAL_MSK                              (((1ULL<<DMA_CTRL_DSTPERIPHERAL_LEN)-1)<<DMA_CTRL_DSTPERIPHERAL_POS)
#define DMA_CTRL_DSTPERIPHERAL_UMSK                             (~(((1ULL<<DMA_CTRL_DSTPERIPHERAL_LEN)-1)<<DMA_CTRL_DSTPERIPHERAL_POS))
#define DMA_CTRL_FLOWCNTRL                                      DMA_CTRL_FLOWCNTRL
#define DMA_CTRL_FLOWCNTRL_POS                                  (11U)
#define DMA_CTRL_FLOWCNTRL_LEN                                  (3U)
#define DMA_CTRL_FLOWCNTRL_MSK                                  (((1ULL<<DMA_CTRL_FLOWCNTRL_LEN)-1)<<DMA_CTRL_FLOWCNTRL_POS)
#define DMA_CTRL_FLOWCNTRL_UMSK                                 (~(((1ULL<<DMA_CTRL_FLOWCNTRL_LEN)-1)<<DMA_CTRL_FLOWCNTRL_POS))
#define DMA_CTRL_IE                                             DMA_CTRL_IE
#define DMA_CTRL_IE_POS                                         (14U)
#define DMA_CTRL_IE_LEN                                         (1U)
#define DMA_CTRL_IE_MSK                                         (((1ULL<<DMA_CTRL_IE_LEN)-1)<<DMA_CTRL_IE_POS)
#define DMA_CTRL_IE_UMSK                                        (~(((1ULL<<DMA_CTRL_IE_LEN)-1)<<DMA_CTRL_IE_POS))
#define DMA_CTRL_ITC                                            DMA_CTRL_ITC
#define DMA_CTRL_ITC_POS                                        (15U)
#define DMA_CTRL_ITC_LEN                                        (1U)
#define DMA_CTRL_ITC_MSK                                        (((1ULL<<DMA_CTRL_ITC_LEN)-1)<<DMA_CTRL_ITC_POS)
#define DMA_CTRL_ITC_UMSK                                       (~(((1ULL<<DMA_CTRL_ITC_LEN)-1)<<DMA_CTRL_ITC_POS))
#define DMA_CTRL_L                                              DMA_CTRL_L
#define DMA_CTRL_L_POS                                          (16U)
#define DMA_CTRL_L_LEN                                          (1U)
#define DMA_CTRL_L_MSK                                          (((1ULL<<DMA_CTRL_L_LEN)-1)<<DMA_CTRL_L_POS)
#define DMA_CTRL_L_UMSK                                         (~(((1ULL<<DMA_CTRL_L_LEN)-1)<<DMA_CTRL_L_POS))
#define DMA_CTRL_A                                              DMA_CTRL_A
#define DMA_CTRL_A_POS                                          (17U)
#define DMA_CTRL_A_LEN                                          (1U)
#define DMA_CTRL_A_MSK                                          (((1ULL<<DMA_CTRL_A_LEN)-1)<<DMA_CTRL_A_POS)
#define DMA_CTRL_A_UMSK                                         (~(((1ULL<<DMA_CTRL_A_LEN)-1)<<DMA_CTRL_A_POS))
#define DMA_CTRL_H                                              DMA_CTRL_H
#define DMA_CTRL_H_POS                                          (18U)
#define DMA_CTRL_H_LEN                                          (1U)
#define DMA_CTRL_H_MSK                                          (((1ULL<<DMA_CTRL_H_LEN)-1)<<DMA_CTRL_H_POS)
#define DMA_CTRL_H_UMSK                                         (~(((1ULL<<DMA_CTRL_H_LEN)-1)<<DMA_CTRL_H_POS))

/* 0x41C : DMA_C3RSVD */
#define DMA_CTRL_DMA_C3RSVD_OFFSET                              (0x41C)
#define DMA_CTRL_DSTREMNSGLE                                    DMA_CTRL_DSTREMNSGLE
#define DMA_CTRL_DSTREMNSGLE_POS                                (3U)
#define DMA_CTRL_DSTREMNSGLE_LEN                                (1U)
#define DMA_CTRL_DSTREMNSGLE_MSK                                (((1ULL<<DMA_CTRL_DSTREMNSGLE_LEN)-1)<<DMA_CTRL_DSTREMNSGLE_POS)
#define DMA_CTRL_DSTREMNSGLE_UMSK                               (~(((1ULL<<DMA_CTRL_DSTREMNSGLE_LEN)-1)<<DMA_CTRL_DSTREMNSGLE_POS))
#define DMA_CTRL_SRCREMNSGLE                                    DMA_CTRL_SRCREMNSGLE
#define DMA_CTRL_SRCREMNSGLE_POS                                (4U)
#define DMA_CTRL_SRCREMNSGLE_LEN                                (1U)
#define DMA_CTRL_SRCREMNSGLE_MSK                                (((1ULL<<DMA_CTRL_SRCREMNSGLE_LEN)-1)<<DMA_CTRL_SRCREMNSGLE_POS)
#define DMA_CTRL_SRCREMNSGLE_UMSK                               (~(((1ULL<<DMA_CTRL_SRCREMNSGLE_LEN)-1)<<DMA_CTRL_SRCREMNSGLE_POS))


struct  dma_ctrl_reg {
    /* 0x0 : DMA_IntStatus */
    union {
        struct {
            uint32_t IntStatus                      :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_IntStatus;

    /* 0x4 : DMA_IntTCStatus */
    union {
        struct {
            uint32_t IntTCStatus                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_IntTCStatus;

    /* 0x8 : DMA_IntTCClear */
    union {
        struct {
            uint32_t IntTCClear                     :  8; /* [ 7: 0],          w,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_IntTCClear;

    /* 0xC : DMA_IntErrorStatus */
    union {
        struct {
            uint32_t IntErrorStatus                 :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_IntErrorStatus;

    /* 0x10 : DMA_IntErrClr */
    union {
        struct {
            uint32_t IntErrClr                      :  8; /* [ 7: 0],          w,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_IntErrClr;

    /* 0x14 : DMA_RawIntTCStatus */
    union {
        struct {
            uint32_t RawIntTCStatus                 :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_RawIntTCStatus;

    /* 0x18 : DMA_RawIntErrorStatus */
    union {
        struct {
            uint32_t RawIntErrorStatus              :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_RawIntErrorStatus;

    /* 0x1C : DMA_EnbldChns */
    union {
        struct {
            uint32_t EnabledChannels                :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_EnbldChns;

    /* 0x20 : DMA_SoftBReq */
    union {
        struct {
            uint32_t SoftBReq                       : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_SoftBReq;

    /* 0x24 : DMA_SoftSReq */
    union {
        struct {
            uint32_t SoftSReq                       : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_SoftSReq;

    /* 0x28 : DMA_SoftLBReq */
    union {
        struct {
            uint32_t SoftLBReq                      : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_SoftLBReq;

    /* 0x2C : DMA_SoftLSReq */
    union {
        struct {
            uint32_t SoftLSReq                      : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_SoftLSReq;

    /* 0x30 : DMA_Top_Config */
    union {
        struct {
            uint32_t E                              :  1; /* [    0],        r/w,        0x0 */
            uint32_t M                              :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_Top_Config;

    /* 0x34 : DMA_Sync */
    union {
        struct {
            uint32_t DMA_Sync                       : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_Sync;

    /* 0x38  reserved */
    uint8_t RESERVED0x38[200];

    /* 0x100 : DMA_C0SrcAddr */
    union {
        struct {
            uint32_t SrcAddr                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C0SrcAddr;

    /* 0x104 : DMA_C0DstAddr */
    union {
        struct {
            uint32_t DstAddr                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C0DstAddr;

    /* 0x108 : DMA_C0LLI */
    union {
        struct {
            uint32_t LLI                            : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C0LLI;

    /* 0x10C : DMA_C0Control */
    union {
        struct {
            uint32_t TransferSize                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t SBSize                         :  2; /* [13:12],        r/w,        0x1 */
            uint32_t dst_min_mode                   :  1; /* [   14],        r/w,        0x0 */
            uint32_t DBSize                         :  2; /* [16:15],        r/w,        0x1 */
            uint32_t dst_add_mode                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t SWidth                         :  2; /* [19:18],        r/w,        0x2 */
            uint32_t reserved_20                    :  1; /* [   20],       rsvd,        0x0 */
            uint32_t DWidth                         :  2; /* [22:21],        r/w,        0x2 */
            uint32_t fix_cnt                        :  3; /* [25:23],        r/w,        0x0 */
            uint32_t SI                             :  1; /* [   26],        r/w,        0x1 */
            uint32_t DI                             :  1; /* [   27],        r/w,        0x1 */
            uint32_t Prot                           :  3; /* [30:28],        r/w,        0x0 */
            uint32_t I                              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C0Control;

    /* 0x110 : DMA_C0Config */
    union {
        struct {
            uint32_t E                              :  1; /* [    0],        r/w,        0x0 */
            uint32_t SrcPeripheral                  :  5; /* [ 5: 1],        r/w,        0x0 */
            uint32_t DstPeripheral                  :  5; /* [10: 6],        r/w,        0x0 */
            uint32_t FlowCntrl                      :  3; /* [13:11],        r/w,        0x0 */
            uint32_t IE                             :  1; /* [   14],        r/w,        0x0 */
            uint32_t ITC                            :  1; /* [   15],        r/w,        0x0 */
            uint32_t L                              :  1; /* [   16],        r/w,        0x0 */
            uint32_t A                              :  1; /* [   17],          r,        0x0 */
            uint32_t H                              :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t LLICounter                     : 10; /* [29:20],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C0Config;

    /* 0x114  reserved */
    uint8_t RESERVED0x114[8];

    /* 0x11C : DMA_C0RSVD */
    union {
        struct {
            uint32_t reserved_0_2                   :  3; /* [ 2: 0],       rsvd,        0x0 */
            uint32_t DstRemnSgle                    :  1; /* [    3],        r/w,        0x0 */
            uint32_t SrcRemnSgle                    :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C0RSVD;

    /* 0x120  reserved */
    uint8_t RESERVED0x120[224];

    /* 0x200 : DMA_C1SrcAddr */
    union {
        struct {
            uint32_t SrcAddr                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C1SrcAddr;

    /* 0x204 : DMA_C1DstAddr */
    union {
        struct {
            uint32_t DstAddr                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C1DstAddr;

    /* 0x208 : DMA_C1LLI */
    union {
        struct {
            uint32_t LLI                            : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C1LLI;

    /* 0x20C : DMA_C1Control */
    union {
        struct {
            uint32_t TransferSize                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t SBSize                         :  2; /* [13:12],        r/w,        0x1 */
            uint32_t dst_min_mode                   :  1; /* [   14],        r/w,        0x0 */
            uint32_t DBSize                         :  2; /* [16:15],        r/w,        0x1 */
            uint32_t dst_add_mode                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t SWidth                         :  2; /* [19:18],        r/w,        0x2 */
            uint32_t reserved_20                    :  1; /* [   20],       rsvd,        0x0 */
            uint32_t DWidth                         :  2; /* [22:21],        r/w,        0x2 */
            uint32_t fix_cnt                        :  3; /* [25:23],        r/w,        0x0 */
            uint32_t SI                             :  1; /* [   26],        r/w,        0x1 */
            uint32_t DI                             :  1; /* [   27],        r/w,        0x1 */
            uint32_t Prot                           :  3; /* [30:28],        r/w,        0x0 */
            uint32_t I                              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C1Control;

    /* 0x210 : DMA_C1Config */
    union {
        struct {
            uint32_t E                              :  1; /* [    0],        r/w,        0x0 */
            uint32_t SrcPeripheral                  :  5; /* [ 5: 1],        r/w,        0x0 */
            uint32_t DstPeripheral                  :  5; /* [10: 6],        r/w,        0x0 */
            uint32_t FlowCntrl                      :  3; /* [13:11],        r/w,        0x0 */
            uint32_t IE                             :  1; /* [   14],        r/w,        0x0 */
            uint32_t ITC                            :  1; /* [   15],        r/w,        0x0 */
            uint32_t L                              :  1; /* [   16],        r/w,        0x0 */
            uint32_t A                              :  1; /* [   17],          r,        0x0 */
            uint32_t H                              :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C1Config;

    /* 0x214  reserved */
    uint8_t RESERVED0x214[8];

    /* 0x21C : DMA_C1RSVD */
    union {
        struct {
            uint32_t reserved_0_2                   :  3; /* [ 2: 0],       rsvd,        0x0 */
            uint32_t DstRemnSgle                    :  1; /* [    3],        r/w,        0x0 */
            uint32_t SrcRemnSgle                    :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C1RSVD;

    /* 0x220  reserved */
    uint8_t RESERVED0x220[224];

    /* 0x300 : DMA_C2SrcAddr */
    union {
        struct {
            uint32_t SrcAddr                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C2SrcAddr;

    /* 0x304 : DMA_C2DstAddr */
    union {
        struct {
            uint32_t DstAddr                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C2DstAddr;

    /* 0x308 : DMA_C2LLI */
    union {
        struct {
            uint32_t LLI                            : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C2LLI;

    /* 0x30C : DMA_C2Control */
    union {
        struct {
            uint32_t TransferSize                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t SBSize                         :  2; /* [13:12],        r/w,        0x1 */
            uint32_t dst_min_mode                   :  1; /* [   14],        r/w,        0x0 */
            uint32_t DBSize                         :  2; /* [16:15],        r/w,        0x1 */
            uint32_t dst_add_mode                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t SWidth                         :  2; /* [19:18],        r/w,        0x2 */
            uint32_t reserved_20                    :  1; /* [   20],       rsvd,        0x0 */
            uint32_t DWidth                         :  2; /* [22:21],        r/w,        0x2 */
            uint32_t fix_cnt                        :  3; /* [25:23],        r/w,        0x0 */
            uint32_t SI                             :  1; /* [   26],        r/w,        0x1 */
            uint32_t DI                             :  1; /* [   27],        r/w,        0x1 */
            uint32_t Prot                           :  3; /* [30:28],        r/w,        0x0 */
            uint32_t I                              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C2Control;

    /* 0x310 : DMA_C2Config */
    union {
        struct {
            uint32_t E                              :  1; /* [    0],        r/w,        0x0 */
            uint32_t SrcPeripheral                  :  5; /* [ 5: 1],        r/w,        0x0 */
            uint32_t DstPeripheral                  :  5; /* [10: 6],        r/w,        0x0 */
            uint32_t FlowCntrl                      :  3; /* [13:11],        r/w,        0x0 */
            uint32_t IE                             :  1; /* [   14],        r/w,        0x0 */
            uint32_t ITC                            :  1; /* [   15],        r/w,        0x0 */
            uint32_t L                              :  1; /* [   16],        r/w,        0x0 */
            uint32_t A                              :  1; /* [   17],          r,        0x0 */
            uint32_t H                              :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C2Config;

    /* 0x314  reserved */
    uint8_t RESERVED0x314[8];

    /* 0x31C : DMA_C2RSVD */
    union {
        struct {
            uint32_t reserved_0_2                   :  3; /* [ 2: 0],       rsvd,        0x0 */
            uint32_t DstRemnSgle                    :  1; /* [    3],        r/w,        0x0 */
            uint32_t SrcRemnSgle                    :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C2RSVD;

    /* 0x320  reserved */
    uint8_t RESERVED0x320[224];

    /* 0x400 : DMA_C3SrcAddr */
    union {
        struct {
            uint32_t SrcAddr                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C3SrcAddr;

    /* 0x404 : DMA_C3DstAddr */
    union {
        struct {
            uint32_t DstAddr                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C3DstAddr;

    /* 0x408 : DMA_C3LLI */
    union {
        struct {
            uint32_t LLI                            : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C3LLI;

    /* 0x40C : DMA_C3Control */
    union {
        struct {
            uint32_t TransferSize                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t SBSize                         :  2; /* [13:12],        r/w,        0x1 */
            uint32_t dst_min_mode                   :  1; /* [   14],        r/w,        0x0 */
            uint32_t DBSize                         :  2; /* [16:15],        r/w,        0x1 */
            uint32_t dst_add_mode                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t SWidth                         :  2; /* [19:18],        r/w,        0x2 */
            uint32_t reserved_20                    :  1; /* [   20],       rsvd,        0x0 */
            uint32_t DWidth                         :  2; /* [22:21],        r/w,        0x2 */
            uint32_t fix_cnt                        :  3; /* [25:23],        r/w,        0x0 */
            uint32_t SI                             :  1; /* [   26],        r/w,        0x1 */
            uint32_t DI                             :  1; /* [   27],        r/w,        0x1 */
            uint32_t Prot                           :  3; /* [30:28],        r/w,        0x0 */
            uint32_t I                              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C3Control;

    /* 0x410 : DMA_C3Config */
    union {
        struct {
            uint32_t E                              :  1; /* [    0],        r/w,        0x0 */
            uint32_t SrcPeripheral                  :  5; /* [ 5: 1],        r/w,        0x0 */
            uint32_t DstPeripheral                  :  5; /* [10: 6],        r/w,        0x0 */
            uint32_t FlowCntrl                      :  3; /* [13:11],        r/w,        0x0 */
            uint32_t IE                             :  1; /* [   14],        r/w,        0x0 */
            uint32_t ITC                            :  1; /* [   15],        r/w,        0x0 */
            uint32_t L                              :  1; /* [   16],        r/w,        0x0 */
            uint32_t A                              :  1; /* [   17],          r,        0x0 */
            uint32_t H                              :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C3Config;

    /* 0x414  reserved */
    uint8_t RESERVED0x414[8];

    /* 0x41C : DMA_C3RSVD */
    union {
        struct {
            uint32_t reserved_0_2                   :  3; /* [ 2: 0],       rsvd,        0x0 */
            uint32_t DstRemnSgle                    :  1; /* [    3],        r/w,        0x0 */
            uint32_t SrcRemnSgle                    :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_C3RSVD;

};

typedef volatile struct dma_ctrl_reg dma_ctrl_reg_t;


#endif  /* __DMA_CTRL_REG_H__ */
