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

#ifndef  __MJCOMB_CSR_REG_H__
#define  __MJCOMB_CSR_REG_H__

#include "ipro7.h"

/* 0x0 : share_csr_0 */
#define MJCOMB_CSR_SHARE_CSR_0_OFFSET                           (0x0)
#define MJCOMB_CSR_REG_MJCOMB_MODE                              MJCOMB_CSR_REG_MJCOMB_MODE
#define MJCOMB_CSR_REG_MJCOMB_MODE_POS                          (0U)
#define MJCOMB_CSR_REG_MJCOMB_MODE_LEN                          (1U)
#define MJCOMB_CSR_REG_MJCOMB_MODE_MSK                          (((1LL<<MJCOMB_CSR_REG_MJCOMB_MODE_LEN)-1)<<MJCOMB_CSR_REG_MJCOMB_MODE_POS)
#define MJCOMB_CSR_REG_MJCOMB_MODE_UMSK                         (~(((1LL<<MJCOMB_CSR_REG_MJCOMB_MODE_LEN)-1)<<MJCOMB_CSR_REG_MJCOMB_MODE_POS))
#define MJCOMB_CSR_REG_MJCOMB_ENABLE                            MJCOMB_CSR_REG_MJCOMB_ENABLE
#define MJCOMB_CSR_REG_MJCOMB_ENABLE_POS                        (1U)
#define MJCOMB_CSR_REG_MJCOMB_ENABLE_LEN                        (1U)
#define MJCOMB_CSR_REG_MJCOMB_ENABLE_MSK                        (((1LL<<MJCOMB_CSR_REG_MJCOMB_ENABLE_LEN)-1)<<MJCOMB_CSR_REG_MJCOMB_ENABLE_POS)
#define MJCOMB_CSR_REG_MJCOMB_ENABLE_UMSK                       (~(((1LL<<MJCOMB_CSR_REG_MJCOMB_ENABLE_LEN)-1)<<MJCOMB_CSR_REG_MJCOMB_ENABLE_POS))
#define MJCOMB_CSR_REG_ORDER_U_EVEN                             MJCOMB_CSR_REG_ORDER_U_EVEN
#define MJCOMB_CSR_REG_ORDER_U_EVEN_POS                         (2U)
#define MJCOMB_CSR_REG_ORDER_U_EVEN_LEN                         (1U)
#define MJCOMB_CSR_REG_ORDER_U_EVEN_MSK                         (((1LL<<MJCOMB_CSR_REG_ORDER_U_EVEN_LEN)-1)<<MJCOMB_CSR_REG_ORDER_U_EVEN_POS)
#define MJCOMB_CSR_REG_ORDER_U_EVEN_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ORDER_U_EVEN_LEN)-1)<<MJCOMB_CSR_REG_ORDER_U_EVEN_POS))
#define MJCOMB_CSR_REG_SWAP_MODE                                MJCOMB_CSR_REG_SWAP_MODE
#define MJCOMB_CSR_REG_SWAP_MODE_POS                            (3U)
#define MJCOMB_CSR_REG_SWAP_MODE_LEN                            (1U)
#define MJCOMB_CSR_REG_SWAP_MODE_MSK                            (((1LL<<MJCOMB_CSR_REG_SWAP_MODE_LEN)-1)<<MJCOMB_CSR_REG_SWAP_MODE_POS)
#define MJCOMB_CSR_REG_SWAP_MODE_UMSK                           (~(((1LL<<MJCOMB_CSR_REG_SWAP_MODE_LEN)-1)<<MJCOMB_CSR_REG_SWAP_MODE_POS))
#define MJCOMB_CSR_REG_LAST_HF_WBLK_DMY                         MJCOMB_CSR_REG_LAST_HF_WBLK_DMY
#define MJCOMB_CSR_REG_LAST_HF_WBLK_DMY_POS                     (4U)
#define MJCOMB_CSR_REG_LAST_HF_WBLK_DMY_LEN                     (1U)
#define MJCOMB_CSR_REG_LAST_HF_WBLK_DMY_MSK                     (((1LL<<MJCOMB_CSR_REG_LAST_HF_WBLK_DMY_LEN)-1)<<MJCOMB_CSR_REG_LAST_HF_WBLK_DMY_POS)
#define MJCOMB_CSR_REG_LAST_HF_WBLK_DMY_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_LAST_HF_WBLK_DMY_LEN)-1)<<MJCOMB_CSR_REG_LAST_HF_WBLK_DMY_POS))
#define MJCOMB_CSR_REG_LAST_HF_HBLK_DMY                         MJCOMB_CSR_REG_LAST_HF_HBLK_DMY
#define MJCOMB_CSR_REG_LAST_HF_HBLK_DMY_POS                     (5U)
#define MJCOMB_CSR_REG_LAST_HF_HBLK_DMY_LEN                     (1U)
#define MJCOMB_CSR_REG_LAST_HF_HBLK_DMY_MSK                     (((1LL<<MJCOMB_CSR_REG_LAST_HF_HBLK_DMY_LEN)-1)<<MJCOMB_CSR_REG_LAST_HF_HBLK_DMY_POS)
#define MJCOMB_CSR_REG_LAST_HF_HBLK_DMY_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_LAST_HF_HBLK_DMY_LEN)-1)<<MJCOMB_CSR_REG_LAST_HF_HBLK_DMY_POS))
#define MJCOMB_CSR_REG_W_XLEN                                   MJCOMB_CSR_REG_W_XLEN
#define MJCOMB_CSR_REG_W_XLEN_POS                               (8U)
#define MJCOMB_CSR_REG_W_XLEN_LEN                               (3U)
#define MJCOMB_CSR_REG_W_XLEN_MSK                               (((1LL<<MJCOMB_CSR_REG_W_XLEN_LEN)-1)<<MJCOMB_CSR_REG_W_XLEN_POS)
#define MJCOMB_CSR_REG_W_XLEN_UMSK                              (~(((1LL<<MJCOMB_CSR_REG_W_XLEN_LEN)-1)<<MJCOMB_CSR_REG_W_XLEN_POS))
#define MJCOMB_CSR_REG_YUV_MODE                                 MJCOMB_CSR_REG_YUV_MODE
#define MJCOMB_CSR_REG_YUV_MODE_POS                             (12U)
#define MJCOMB_CSR_REG_YUV_MODE_LEN                             (3U)
#define MJCOMB_CSR_REG_YUV_MODE_MSK                             (((1LL<<MJCOMB_CSR_REG_YUV_MODE_LEN)-1)<<MJCOMB_CSR_REG_YUV_MODE_POS)
#define MJCOMB_CSR_REG_YUV_MODE_UMSK                            (~(((1LL<<MJCOMB_CSR_REG_YUV_MODE_LEN)-1)<<MJCOMB_CSR_REG_YUV_MODE_POS))
#define MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT                       MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT
#define MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT_POS                   (16U)
#define MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT_LEN                   (5U)
#define MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT_MSK                   (((1LL<<MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT_LEN)-1)<<MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT_POS)
#define MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT_UMSK                  (~(((1LL<<MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT_LEN)-1)<<MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT_POS))

/* 0x08 : share_csr_1 */
#define MJCOMB_CSR_SHARE_CSR_1_OFFSET                           (0x08)
#define MJCOMB_CSR_REG_YY_ADDR_START                            MJCOMB_CSR_REG_YY_ADDR_START
#define MJCOMB_CSR_REG_YY_ADDR_START_POS                        (0U)
#define MJCOMB_CSR_REG_YY_ADDR_START_LEN                        (32U)
#define MJCOMB_CSR_REG_YY_ADDR_START_MSK                        (((1LL<<MJCOMB_CSR_REG_YY_ADDR_START_LEN)-1)<<MJCOMB_CSR_REG_YY_ADDR_START_POS)
#define MJCOMB_CSR_REG_YY_ADDR_START_UMSK                       (~(((1LL<<MJCOMB_CSR_REG_YY_ADDR_START_LEN)-1)<<MJCOMB_CSR_REG_YY_ADDR_START_POS))

/* 0x0C : share_csr_2 */
#define MJCOMB_CSR_SHARE_CSR_2_OFFSET                           (0x0C)
#define MJCOMB_CSR_REG_UV_ADDR_START                            MJCOMB_CSR_REG_UV_ADDR_START
#define MJCOMB_CSR_REG_UV_ADDR_START_POS                        (0U)
#define MJCOMB_CSR_REG_UV_ADDR_START_LEN                        (32U)
#define MJCOMB_CSR_REG_UV_ADDR_START_MSK                        (((1LL<<MJCOMB_CSR_REG_UV_ADDR_START_LEN)-1)<<MJCOMB_CSR_REG_UV_ADDR_START_POS)
#define MJCOMB_CSR_REG_UV_ADDR_START_UMSK                       (~(((1LL<<MJCOMB_CSR_REG_UV_ADDR_START_LEN)-1)<<MJCOMB_CSR_REG_UV_ADDR_START_POS))

/* 0x10 : share_csr_3 */
#define MJCOMB_CSR_SHARE_CSR_3_OFFSET                           (0x10)
#define MJCOMB_CSR_REG_FRAME_WBLK                               MJCOMB_CSR_REG_FRAME_WBLK
#define MJCOMB_CSR_REG_FRAME_WBLK_POS                           (0U)
#define MJCOMB_CSR_REG_FRAME_WBLK_LEN                           (12U)
#define MJCOMB_CSR_REG_FRAME_WBLK_MSK                           (((1LL<<MJCOMB_CSR_REG_FRAME_WBLK_LEN)-1)<<MJCOMB_CSR_REG_FRAME_WBLK_POS)
#define MJCOMB_CSR_REG_FRAME_WBLK_UMSK                          (~(((1LL<<MJCOMB_CSR_REG_FRAME_WBLK_LEN)-1)<<MJCOMB_CSR_REG_FRAME_WBLK_POS))
#define MJCOMB_CSR_REG_FRAME_HBLK                               MJCOMB_CSR_REG_FRAME_HBLK
#define MJCOMB_CSR_REG_FRAME_HBLK_POS                           (16U)
#define MJCOMB_CSR_REG_FRAME_HBLK_LEN                           (12U)
#define MJCOMB_CSR_REG_FRAME_HBLK_MSK                           (((1LL<<MJCOMB_CSR_REG_FRAME_HBLK_LEN)-1)<<MJCOMB_CSR_REG_FRAME_HBLK_POS)
#define MJCOMB_CSR_REG_FRAME_HBLK_UMSK                          (~(((1LL<<MJCOMB_CSR_REG_FRAME_HBLK_LEN)-1)<<MJCOMB_CSR_REG_FRAME_HBLK_POS))

/* 0x14 : share_csr_4 */
#define MJCOMB_CSR_SHARE_CSR_4_OFFSET                           (0x14)
#define MJCOMB_CSR_REG_DEC_JP_PUSH                              MJCOMB_CSR_REG_DEC_JP_PUSH
#define MJCOMB_CSR_REG_DEC_JP_PUSH_POS                          (0U)
#define MJCOMB_CSR_REG_DEC_JP_PUSH_LEN                          (1U)
#define MJCOMB_CSR_REG_DEC_JP_PUSH_MSK                          (((1LL<<MJCOMB_CSR_REG_DEC_JP_PUSH_LEN)-1)<<MJCOMB_CSR_REG_DEC_JP_PUSH_POS)
#define MJCOMB_CSR_REG_DEC_JP_PUSH_UMSK                         (~(((1LL<<MJCOMB_CSR_REG_DEC_JP_PUSH_LEN)-1)<<MJCOMB_CSR_REG_DEC_JP_PUSH_POS))
#define MJCOMB_CSR_REG_JP_ADDR                                  MJCOMB_CSR_REG_JP_ADDR
#define MJCOMB_CSR_REG_JP_ADDR_POS                              (2U)
#define MJCOMB_CSR_REG_JP_ADDR_LEN                              (30U)
#define MJCOMB_CSR_REG_JP_ADDR_MSK                              (((1LL<<MJCOMB_CSR_REG_JP_ADDR_LEN)-1)<<MJCOMB_CSR_REG_JP_ADDR_POS)
#define MJCOMB_CSR_REG_JP_ADDR_UMSK                             (~(((1LL<<MJCOMB_CSR_REG_JP_ADDR_LEN)-1)<<MJCOMB_CSR_REG_JP_ADDR_POS))

/* 0x18 : share_csr_5 */
#define MJCOMB_CSR_SHARE_CSR_5_OFFSET                           (0x18)
#define MJCOMB_CSR_REG_HEAD_BYTE                                MJCOMB_CSR_REG_HEAD_BYTE
#define MJCOMB_CSR_REG_HEAD_BYTE_POS                            (0U)
#define MJCOMB_CSR_REG_HEAD_BYTE_LEN                            (16U)
#define MJCOMB_CSR_REG_HEAD_BYTE_MSK                            (((1LL<<MJCOMB_CSR_REG_HEAD_BYTE_LEN)-1)<<MJCOMB_CSR_REG_HEAD_BYTE_POS)
#define MJCOMB_CSR_REG_HEAD_BYTE_UMSK                           (~(((1LL<<MJCOMB_CSR_REG_HEAD_BYTE_LEN)-1)<<MJCOMB_CSR_REG_HEAD_BYTE_POS))
#define MJCOMB_CSR_REG_ENC_TAIL_EXP                             MJCOMB_CSR_REG_ENC_TAIL_EXP
#define MJCOMB_CSR_REG_ENC_TAIL_EXP_POS                         (16U)
#define MJCOMB_CSR_REG_ENC_TAIL_EXP_LEN                         (1U)
#define MJCOMB_CSR_REG_ENC_TAIL_EXP_MSK                         (((1LL<<MJCOMB_CSR_REG_ENC_TAIL_EXP_LEN)-1)<<MJCOMB_CSR_REG_ENC_TAIL_EXP_POS)
#define MJCOMB_CSR_REG_ENC_TAIL_EXP_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ENC_TAIL_EXP_LEN)-1)<<MJCOMB_CSR_REG_ENC_TAIL_EXP_POS))
#define MJCOMB_CSR_REG_DEC_HDER_SKIP                            MJCOMB_CSR_REG_DEC_HDER_SKIP
#define MJCOMB_CSR_REG_DEC_HDER_SKIP_POS                        (17U)
#define MJCOMB_CSR_REG_DEC_HDER_SKIP_LEN                        (1U)
#define MJCOMB_CSR_REG_DEC_HDER_SKIP_MSK                        (((1LL<<MJCOMB_CSR_REG_DEC_HDER_SKIP_LEN)-1)<<MJCOMB_CSR_REG_DEC_HDER_SKIP_POS)
#define MJCOMB_CSR_REG_DEC_HDER_SKIP_UMSK                       (~(((1LL<<MJCOMB_CSR_REG_DEC_HDER_SKIP_LEN)-1)<<MJCOMB_CSR_REG_DEC_HDER_SKIP_POS))

/* 0x7C : share_csr_e */
#define MJCOMB_CSR_SHARE_CSR_E_OFFSET                           (0x7C)
#define MJCOMB_CSR_DUMMY_REG                                    MJCOMB_CSR_DUMMY_REG
#define MJCOMB_CSR_DUMMY_REG_POS                                (0U)
#define MJCOMB_CSR_DUMMY_REG_LEN                                (32U)
#define MJCOMB_CSR_DUMMY_REG_MSK                                (((1LL<<MJCOMB_CSR_DUMMY_REG_LEN)-1)<<MJCOMB_CSR_DUMMY_REG_POS)
#define MJCOMB_CSR_DUMMY_REG_UMSK                               (~(((1LL<<MJCOMB_CSR_DUMMY_REG_LEN)-1)<<MJCOMB_CSR_DUMMY_REG_POS))

/* 0x80 : jenc_csr_0 */
#define MJCOMB_CSR_JENC_CSR_0_OFFSET                            (0x80)
#define MJCOMB_CSR_REG_ENC_BIT_ORDER                            MJCOMB_CSR_REG_ENC_BIT_ORDER
#define MJCOMB_CSR_REG_ENC_BIT_ORDER_POS                        (0U)
#define MJCOMB_CSR_REG_ENC_BIT_ORDER_LEN                        (1U)
#define MJCOMB_CSR_REG_ENC_BIT_ORDER_MSK                        (((1LL<<MJCOMB_CSR_REG_ENC_BIT_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_BIT_ORDER_POS)
#define MJCOMB_CSR_REG_ENC_BIT_ORDER_UMSK                       (~(((1LL<<MJCOMB_CSR_REG_ENC_BIT_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_BIT_ORDER_POS))
#define MJCOMB_CSR_REG_ENC_HW_MODE_SWEN                         MJCOMB_CSR_REG_ENC_HW_MODE_SWEN
#define MJCOMB_CSR_REG_ENC_HW_MODE_SWEN_POS                     (1U)
#define MJCOMB_CSR_REG_ENC_HW_MODE_SWEN_LEN                     (1U)
#define MJCOMB_CSR_REG_ENC_HW_MODE_SWEN_MSK                     (((1LL<<MJCOMB_CSR_REG_ENC_HW_MODE_SWEN_LEN)-1)<<MJCOMB_CSR_REG_ENC_HW_MODE_SWEN_POS)
#define MJCOMB_CSR_REG_ENC_HW_MODE_SWEN_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_ENC_HW_MODE_SWEN_LEN)-1)<<MJCOMB_CSR_REG_ENC_HW_MODE_SWEN_POS))
#define MJCOMB_CSR_REG_ENC_REFLECT_DMY                          MJCOMB_CSR_REG_ENC_REFLECT_DMY
#define MJCOMB_CSR_REG_ENC_REFLECT_DMY_POS                      (2U)
#define MJCOMB_CSR_REG_ENC_REFLECT_DMY_LEN                      (1U)
#define MJCOMB_CSR_REG_ENC_REFLECT_DMY_MSK                      (((1LL<<MJCOMB_CSR_REG_ENC_REFLECT_DMY_LEN)-1)<<MJCOMB_CSR_REG_ENC_REFLECT_DMY_POS)
#define MJCOMB_CSR_REG_ENC_REFLECT_DMY_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_ENC_REFLECT_DMY_LEN)-1)<<MJCOMB_CSR_REG_ENC_REFLECT_DMY_POS))
#define MJCOMB_CSR_REG_ENC_READ_FWRAP                           MJCOMB_CSR_REG_ENC_READ_FWRAP
#define MJCOMB_CSR_REG_ENC_READ_FWRAP_POS                       (3U)
#define MJCOMB_CSR_REG_ENC_READ_FWRAP_LEN                       (1U)
#define MJCOMB_CSR_REG_ENC_READ_FWRAP_MSK                       (((1LL<<MJCOMB_CSR_REG_ENC_READ_FWRAP_LEN)-1)<<MJCOMB_CSR_REG_ENC_READ_FWRAP_POS)
#define MJCOMB_CSR_REG_ENC_READ_FWRAP_UMSK                      (~(((1LL<<MJCOMB_CSR_REG_ENC_READ_FWRAP_LEN)-1)<<MJCOMB_CSR_REG_ENC_READ_FWRAP_POS))
#define MJCOMB_CSR_REG_ENC_W_LOOP_MODE                          MJCOMB_CSR_REG_ENC_W_LOOP_MODE
#define MJCOMB_CSR_REG_ENC_W_LOOP_MODE_POS                      (4U)
#define MJCOMB_CSR_REG_ENC_W_LOOP_MODE_LEN                      (1U)
#define MJCOMB_CSR_REG_ENC_W_LOOP_MODE_MSK                      (((1LL<<MJCOMB_CSR_REG_ENC_W_LOOP_MODE_LEN)-1)<<MJCOMB_CSR_REG_ENC_W_LOOP_MODE_POS)
#define MJCOMB_CSR_REG_ENC_W_LOOP_MODE_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_ENC_W_LOOP_MODE_LEN)-1)<<MJCOMB_CSR_REG_ENC_W_LOOP_MODE_POS))
//removed //#define MJCOMB_CSR_REG_HW_KICK_MODE                             MJCOMB_CSR_REG_HW_KICK_MODE
//removed //#define MJCOMB_CSR_REG_HW_KICK_MODE_POS                         (5U)
//removed //#define MJCOMB_CSR_REG_HW_KICK_MODE_LEN                         (1U)
//removed //#define MJCOMB_CSR_REG_HW_KICK_MODE_MSK                         (((1LL<<MJCOMB_CSR_REG_HW_KICK_MODE_LEN)-1)<<MJCOMB_CSR_REG_HW_KICK_MODE_POS)
//removed //#define MJCOMB_CSR_REG_HW_KICK_MODE_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_HW_KICK_MODE_LEN)-1)<<MJCOMB_CSR_REG_HW_KICK_MODE_POS))
//removed //#define MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK                        MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK
//removed //#define MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK_POS                    (20U)
//removed //#define MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK_LEN                    (4U)
//removed //#define MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK_MSK                    (((1LL<<MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK_LEN)-1)<<MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK_POS)
//removed //#define MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK_UMSK                   (~(((1LL<<MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK_LEN)-1)<<MJCOMB_CSR_REG_ENC_ID_LATCH_HBLK_POS))
#define MJCOMB_CSR_REG_ENC_HW_FRAME                             MJCOMB_CSR_REG_ENC_HW_FRAME
#define MJCOMB_CSR_REG_ENC_HW_FRAME_POS                         (24U)
#define MJCOMB_CSR_REG_ENC_HW_FRAME_LEN                         (6U)
#define MJCOMB_CSR_REG_ENC_HW_FRAME_MSK                         (((1LL<<MJCOMB_CSR_REG_ENC_HW_FRAME_LEN)-1)<<MJCOMB_CSR_REG_ENC_HW_FRAME_POS)
#define MJCOMB_CSR_REG_ENC_HW_FRAME_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ENC_HW_FRAME_LEN)-1)<<MJCOMB_CSR_REG_ENC_HW_FRAME_POS))
#define MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK                       MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK
#define MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK_POS                   (31U)
#define MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK_LEN                   (1U)
#define MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK_MSK                   (((1LL<<MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK_LEN)-1)<<MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK_POS)
#define MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK_UMSK                  (~(((1LL<<MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK_LEN)-1)<<MJCOMB_CSR_REG_ENC_FRAME_DONE_MSK_POS))

/* 0x84 : jenc_csr_1 */
#define MJCOMB_CSR_JENC_CSR_1_OFFSET                            (0x84)
#define MJCOMB_CSR_REG_ENC_SW_FRAME                             MJCOMB_CSR_REG_ENC_SW_FRAME
#define MJCOMB_CSR_REG_ENC_SW_FRAME_POS                         (0U)
#define MJCOMB_CSR_REG_ENC_SW_FRAME_LEN                         (5U)
#define MJCOMB_CSR_REG_ENC_SW_FRAME_MSK                         (((1LL<<MJCOMB_CSR_REG_ENC_SW_FRAME_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_FRAME_POS)
#define MJCOMB_CSR_REG_ENC_SW_FRAME_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ENC_SW_FRAME_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_FRAME_POS))
#define MJCOMB_CSR_REG_ENC_SW_KICK                              MJCOMB_CSR_REG_ENC_SW_KICK
#define MJCOMB_CSR_REG_ENC_SW_KICK_POS                          (6U)
#define MJCOMB_CSR_REG_ENC_SW_KICK_LEN                          (1U)
#define MJCOMB_CSR_REG_ENC_SW_KICK_MSK                          (((1LL<<MJCOMB_CSR_REG_ENC_SW_KICK_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_KICK_POS)
#define MJCOMB_CSR_REG_ENC_SW_KICK_UMSK                         (~(((1LL<<MJCOMB_CSR_REG_ENC_SW_KICK_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_KICK_POS))
#define MJCOMB_CSR_REG_ENC_SW_KICK_MODE                         MJCOMB_CSR_REG_ENC_SW_KICK_MODE
#define MJCOMB_CSR_REG_ENC_SW_KICK_MODE_POS                     (7U)
#define MJCOMB_CSR_REG_ENC_SW_KICK_MODE_LEN                     (1U)
#define MJCOMB_CSR_REG_ENC_SW_KICK_MODE_MSK                     (((1LL<<MJCOMB_CSR_REG_ENC_SW_KICK_MODE_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_KICK_MODE_POS)
#define MJCOMB_CSR_REG_ENC_SW_KICK_MODE_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_ENC_SW_KICK_MODE_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_KICK_MODE_POS))
#define MJCOMB_CSR_REG_ENC_SW_MODE                              MJCOMB_CSR_REG_ENC_SW_MODE
#define MJCOMB_CSR_REG_ENC_SW_MODE_POS                          (8U)
#define MJCOMB_CSR_REG_ENC_SW_MODE_LEN                          (1U)
#define MJCOMB_CSR_REG_ENC_SW_MODE_MSK                          (((1LL<<MJCOMB_CSR_REG_ENC_SW_MODE_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_MODE_POS)
#define MJCOMB_CSR_REG_ENC_SW_MODE_UMSK                         (~(((1LL<<MJCOMB_CSR_REG_ENC_SW_MODE_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_MODE_POS))
#define MJCOMB_CSR_REG_ENC_SW_RUN                               MJCOMB_CSR_REG_ENC_SW_RUN
#define MJCOMB_CSR_REG_ENC_SW_RUN_POS                           (9U)
#define MJCOMB_CSR_REG_ENC_SW_RUN_LEN                           (1U)
#define MJCOMB_CSR_REG_ENC_SW_RUN_MSK                           (((1LL<<MJCOMB_CSR_REG_ENC_SW_RUN_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_RUN_POS)
#define MJCOMB_CSR_REG_ENC_SW_RUN_UMSK                          (~(((1LL<<MJCOMB_CSR_REG_ENC_SW_RUN_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_RUN_POS))
#define MJCOMB_CSR_REG_ENC_SW_ABORT                             MJCOMB_CSR_REG_ENC_SW_ABORT
#define MJCOMB_CSR_REG_ENC_SW_ABORT_POS                         (10U)
#define MJCOMB_CSR_REG_ENC_SW_ABORT_LEN                         (1U)
#define MJCOMB_CSR_REG_ENC_SW_ABORT_MSK                         (((1LL<<MJCOMB_CSR_REG_ENC_SW_ABORT_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_ABORT_POS)
#define MJCOMB_CSR_REG_ENC_SW_ABORT_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ENC_SW_ABORT_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_ABORT_POS))
//removed //#define MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL                       MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL
//removed //#define MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL_POS                   (10U)
//removed //#define MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL_LEN                   (3U)
//removed //#define MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL_MSK                   (((1LL<<MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL_LEN)-1)<<MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL_POS)
//removed //#define MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL_UMSK                  (~(((1LL<<MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL_LEN)-1)<<MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL_POS))
//removed //#define MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL                       MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL
//removed //#define MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL_POS                   (13U)
//removed //#define MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL_LEN                   (3U)
//removed //#define MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL_MSK                   (((1LL<<MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL_LEN)-1)<<MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL_POS)
//removed //#define MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL_UMSK                  (~(((1LL<<MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL_LEN)-1)<<MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL_POS))
#define MJCOMB_CSR_REG_ENC_WAIT_CYCLE                           MJCOMB_CSR_REG_ENC_WAIT_CYCLE
#define MJCOMB_CSR_REG_ENC_WAIT_CYCLE_POS                       (16U)
#define MJCOMB_CSR_REG_ENC_WAIT_CYCLE_LEN                       (16U)
#define MJCOMB_CSR_REG_ENC_WAIT_CYCLE_MSK                       (((1LL<<MJCOMB_CSR_REG_ENC_WAIT_CYCLE_LEN)-1)<<MJCOMB_CSR_REG_ENC_WAIT_CYCLE_POS)
#define MJCOMB_CSR_REG_ENC_WAIT_CYCLE_UMSK                      (~(((1LL<<MJCOMB_CSR_REG_ENC_WAIT_CYCLE_LEN)-1)<<MJCOMB_CSR_REG_ENC_WAIT_CYCLE_POS))

/* 0x88 : jenc_csr_2 */
#define MJCOMB_CSR_JENC_CSR_2_OFFSET                            (0x88)
#define MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT                       MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT
#define MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT_POS                   (0U)
#define MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT_LEN                   (32U)
#define MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT_MSK                   (((1LL<<MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT_LEN)-1)<<MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT_POS)
#define MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT_UMSK                  (~(((1LL<<MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT_LEN)-1)<<MJCOMB_CSR_REG_ENC_JSTR_BURST_CNT_POS))

/* 0x8C : jenc_csr_3 */
#define MJCOMB_CSR_JENC_CSR_3_OFFSET                            (0x8C)
#define MJCOMB_CSR_REG_ENC_YY_MEM_HBLK                          MJCOMB_CSR_REG_ENC_YY_MEM_HBLK
#define MJCOMB_CSR_REG_ENC_YY_MEM_HBLK_POS                      (0U)
#define MJCOMB_CSR_REG_ENC_YY_MEM_HBLK_LEN                      (13U)
#define MJCOMB_CSR_REG_ENC_YY_MEM_HBLK_MSK                      (((1LL<<MJCOMB_CSR_REG_ENC_YY_MEM_HBLK_LEN)-1)<<MJCOMB_CSR_REG_ENC_YY_MEM_HBLK_POS)
#define MJCOMB_CSR_REG_ENC_YY_MEM_HBLK_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_ENC_YY_MEM_HBLK_LEN)-1)<<MJCOMB_CSR_REG_ENC_YY_MEM_HBLK_POS))
#define MJCOMB_CSR_REG_ENC_UV_MEM_HBLK                          MJCOMB_CSR_REG_ENC_UV_MEM_HBLK
#define MJCOMB_CSR_REG_ENC_UV_MEM_HBLK_POS                      (16U)
#define MJCOMB_CSR_REG_ENC_UV_MEM_HBLK_LEN                      (13U)
#define MJCOMB_CSR_REG_ENC_UV_MEM_HBLK_MSK                      (((1LL<<MJCOMB_CSR_REG_ENC_UV_MEM_HBLK_LEN)-1)<<MJCOMB_CSR_REG_ENC_UV_MEM_HBLK_POS)
#define MJCOMB_CSR_REG_ENC_UV_MEM_HBLK_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_ENC_UV_MEM_HBLK_LEN)-1)<<MJCOMB_CSR_REG_ENC_UV_MEM_HBLK_POS))

/* 0x90 : jenc_csr_4 */
#define MJCOMB_CSR_JENC_CSR_4_OFFSET                            (0x90)
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_EN                        MJCOMB_CSR_REG_ENC_INT_NORMAL_EN
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_EN_POS                    (0U)
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_EN_LEN                    (1U)
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_EN_MSK                    (((1LL<<MJCOMB_CSR_REG_ENC_INT_NORMAL_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_NORMAL_EN_POS)
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_EN_UMSK                   (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_NORMAL_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_NORMAL_EN_POS))
#define MJCOMB_CSR_REG_ENC_INT_CAM_EN                           MJCOMB_CSR_REG_ENC_INT_CAM_EN
#define MJCOMB_CSR_REG_ENC_INT_CAM_EN_POS                       (1U)
#define MJCOMB_CSR_REG_ENC_INT_CAM_EN_LEN                       (1U)
#define MJCOMB_CSR_REG_ENC_INT_CAM_EN_MSK                       (((1LL<<MJCOMB_CSR_REG_ENC_INT_CAM_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_CAM_EN_POS)
#define MJCOMB_CSR_REG_ENC_INT_CAM_EN_UMSK                      (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_CAM_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_CAM_EN_POS))
#define MJCOMB_CSR_REG_ENC_INT_MEM_EN                           MJCOMB_CSR_REG_ENC_INT_MEM_EN
#define MJCOMB_CSR_REG_ENC_INT_MEM_EN_POS                       (2U)
#define MJCOMB_CSR_REG_ENC_INT_MEM_EN_LEN                       (1U)
#define MJCOMB_CSR_REG_ENC_INT_MEM_EN_MSK                       (((1LL<<MJCOMB_CSR_REG_ENC_INT_MEM_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_MEM_EN_POS)
#define MJCOMB_CSR_REG_ENC_INT_MEM_EN_UMSK                      (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_MEM_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_MEM_EN_POS))
#define MJCOMB_CSR_REG_ENC_INT_FRAME_EN                         MJCOMB_CSR_REG_ENC_INT_FRAME_EN
#define MJCOMB_CSR_REG_ENC_INT_FRAME_EN_POS                     (3U)
#define MJCOMB_CSR_REG_ENC_INT_FRAME_EN_LEN                     (1U)
#define MJCOMB_CSR_REG_ENC_INT_FRAME_EN_MSK                     (((1LL<<MJCOMB_CSR_REG_ENC_INT_FRAME_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_FRAME_EN_POS)
#define MJCOMB_CSR_REG_ENC_INT_FRAME_EN_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_FRAME_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_FRAME_EN_POS))
#define MJCOMB_CSR_REG_ENC_INT_IDLE_EN                          MJCOMB_CSR_REG_ENC_INT_IDLE_EN
#define MJCOMB_CSR_REG_ENC_INT_IDLE_EN_POS                      (4U)
#define MJCOMB_CSR_REG_ENC_INT_IDLE_EN_LEN                      (1U)
#define MJCOMB_CSR_REG_ENC_INT_IDLE_EN_MSK                      (((1LL<<MJCOMB_CSR_REG_ENC_INT_IDLE_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_IDLE_EN_POS)
#define MJCOMB_CSR_REG_ENC_INT_IDLE_EN_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_IDLE_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_IDLE_EN_POS))
#define MJCOMB_CSR_REG_ENC_INT_SWAP_EN                          MJCOMB_CSR_REG_ENC_INT_SWAP_EN
#define MJCOMB_CSR_REG_ENC_INT_SWAP_EN_POS                      (5U)
#define MJCOMB_CSR_REG_ENC_INT_SWAP_EN_LEN                      (1U)
#define MJCOMB_CSR_REG_ENC_INT_SWAP_EN_MSK                      (((1LL<<MJCOMB_CSR_REG_ENC_INT_SWAP_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_SWAP_EN_POS)
#define MJCOMB_CSR_REG_ENC_INT_SWAP_EN_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_SWAP_EN_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_SWAP_EN_POS))

/* 0x94 : jenc_csr_5 */
#define MJCOMB_CSR_JENC_CSR_5_OFFSET                            (0x94)
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR                       MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR_POS                   (0U)
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR_LEN                   (1U)
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR_MSK                   (((1LL<<MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR_POS)
#define MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR_UMSK                  (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_NORMAL_CLR_POS))
#define MJCOMB_CSR_REG_ENC_INT_CAM_CLR                          MJCOMB_CSR_REG_ENC_INT_CAM_CLR
#define MJCOMB_CSR_REG_ENC_INT_CAM_CLR_POS                      (1U)
#define MJCOMB_CSR_REG_ENC_INT_CAM_CLR_LEN                      (1U)
#define MJCOMB_CSR_REG_ENC_INT_CAM_CLR_MSK                      (((1LL<<MJCOMB_CSR_REG_ENC_INT_CAM_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_CAM_CLR_POS)
#define MJCOMB_CSR_REG_ENC_INT_CAM_CLR_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_CAM_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_CAM_CLR_POS))
#define MJCOMB_CSR_REG_ENC_INT_MEM_CLR                          MJCOMB_CSR_REG_ENC_INT_MEM_CLR
#define MJCOMB_CSR_REG_ENC_INT_MEM_CLR_POS                      (2U)
#define MJCOMB_CSR_REG_ENC_INT_MEM_CLR_LEN                      (1U)
#define MJCOMB_CSR_REG_ENC_INT_MEM_CLR_MSK                      (((1LL<<MJCOMB_CSR_REG_ENC_INT_MEM_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_MEM_CLR_POS)
#define MJCOMB_CSR_REG_ENC_INT_MEM_CLR_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_MEM_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_MEM_CLR_POS))
#define MJCOMB_CSR_REG_ENC_INT_FRAME_CLR                        MJCOMB_CSR_REG_ENC_INT_FRAME_CLR
#define MJCOMB_CSR_REG_ENC_INT_FRAME_CLR_POS                    (3U)
#define MJCOMB_CSR_REG_ENC_INT_FRAME_CLR_LEN                    (1U)
#define MJCOMB_CSR_REG_ENC_INT_FRAME_CLR_MSK                    (((1LL<<MJCOMB_CSR_REG_ENC_INT_FRAME_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_FRAME_CLR_POS)
#define MJCOMB_CSR_REG_ENC_INT_FRAME_CLR_UMSK                   (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_FRAME_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_FRAME_CLR_POS))
#define MJCOMB_CSR_REG_ENC_INT_IDLE_CLR                         MJCOMB_CSR_REG_ENC_INT_IDLE_CLR
#define MJCOMB_CSR_REG_ENC_INT_IDLE_CLR_POS                     (4U)
#define MJCOMB_CSR_REG_ENC_INT_IDLE_CLR_LEN                     (1U)
#define MJCOMB_CSR_REG_ENC_INT_IDLE_CLR_MSK                     (((1LL<<MJCOMB_CSR_REG_ENC_INT_IDLE_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_IDLE_CLR_POS)
#define MJCOMB_CSR_REG_ENC_INT_IDLE_CLR_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_IDLE_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_IDLE_CLR_POS))
#define MJCOMB_CSR_REG_ENC_INT_SWAP_CLR                         MJCOMB_CSR_REG_ENC_INT_SWAP_CLR
#define MJCOMB_CSR_REG_ENC_INT_SWAP_CLR_POS                     (5U)
#define MJCOMB_CSR_REG_ENC_INT_SWAP_CLR_LEN                     (1U)
#define MJCOMB_CSR_REG_ENC_INT_SWAP_CLR_MSK                     (((1LL<<MJCOMB_CSR_REG_ENC_INT_SWAP_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_SWAP_CLR_POS)
#define MJCOMB_CSR_REG_ENC_INT_SWAP_CLR_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_ENC_INT_SWAP_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_INT_SWAP_CLR_POS))
#define MJCOMB_CSR_REG_ENC_W_SWAP_CLR                           MJCOMB_CSR_REG_ENC_W_SWAP_CLR
#define MJCOMB_CSR_REG_ENC_W_SWAP_CLR_POS                       (6U)
#define MJCOMB_CSR_REG_ENC_W_SWAP_CLR_LEN                       (1U)
#define MJCOMB_CSR_REG_ENC_W_SWAP_CLR_MSK                       (((1LL<<MJCOMB_CSR_REG_ENC_W_SWAP_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_W_SWAP_CLR_POS)
#define MJCOMB_CSR_REG_ENC_W_SWAP_CLR_UMSK                      (~(((1LL<<MJCOMB_CSR_REG_ENC_W_SWAP_CLR_LEN)-1)<<MJCOMB_CSR_REG_ENC_W_SWAP_CLR_POS))
#define MJCOMB_CSR_REG_ENC_RFIFO_POP                            MJCOMB_CSR_REG_ENC_RFIFO_POP
#define MJCOMB_CSR_REG_ENC_RFIFO_POP_POS                        (7U)
#define MJCOMB_CSR_REG_ENC_RFIFO_POP_LEN                        (1U)
#define MJCOMB_CSR_REG_ENC_RFIFO_POP_MSK                        (((1LL<<MJCOMB_CSR_REG_ENC_RFIFO_POP_LEN)-1)<<MJCOMB_CSR_REG_ENC_RFIFO_POP_POS)
#define MJCOMB_CSR_REG_ENC_RFIFO_POP_UMSK                       (~(((1LL<<MJCOMB_CSR_REG_ENC_RFIFO_POP_LEN)-1)<<MJCOMB_CSR_REG_ENC_RFIFO_POP_POS))
#define MJCOMB_CSR_STS_ENC_NORMAL_INT                           MJCOMB_CSR_STS_ENC_NORMAL_INT
#define MJCOMB_CSR_STS_ENC_NORMAL_INT_POS                       (8U)
#define MJCOMB_CSR_STS_ENC_NORMAL_INT_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_NORMAL_INT_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_NORMAL_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_NORMAL_INT_POS)
#define MJCOMB_CSR_STS_ENC_NORMAL_INT_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_NORMAL_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_NORMAL_INT_POS))
#define MJCOMB_CSR_STS_ENC_CAM_INT                              MJCOMB_CSR_STS_ENC_CAM_INT
#define MJCOMB_CSR_STS_ENC_CAM_INT_POS                          (9U)
#define MJCOMB_CSR_STS_ENC_CAM_INT_LEN                          (1U)
#define MJCOMB_CSR_STS_ENC_CAM_INT_MSK                          (((1LL<<MJCOMB_CSR_STS_ENC_CAM_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_CAM_INT_POS)
#define MJCOMB_CSR_STS_ENC_CAM_INT_UMSK                         (~(((1LL<<MJCOMB_CSR_STS_ENC_CAM_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_CAM_INT_POS))
#define MJCOMB_CSR_STS_ENC_MEM_INT                              MJCOMB_CSR_STS_ENC_MEM_INT
#define MJCOMB_CSR_STS_ENC_MEM_INT_POS                          (10U)
#define MJCOMB_CSR_STS_ENC_MEM_INT_LEN                          (1U)
#define MJCOMB_CSR_STS_ENC_MEM_INT_MSK                          (((1LL<<MJCOMB_CSR_STS_ENC_MEM_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_MEM_INT_POS)
#define MJCOMB_CSR_STS_ENC_MEM_INT_UMSK                         (~(((1LL<<MJCOMB_CSR_STS_ENC_MEM_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_MEM_INT_POS))
#define MJCOMB_CSR_STS_ENC_FRAME_INT                            MJCOMB_CSR_STS_ENC_FRAME_INT
#define MJCOMB_CSR_STS_ENC_FRAME_INT_POS                        (11U)
#define MJCOMB_CSR_STS_ENC_FRAME_INT_LEN                        (1U)
#define MJCOMB_CSR_STS_ENC_FRAME_INT_MSK                        (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_INT_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_INT_UMSK                       (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_INT_POS))
#define MJCOMB_CSR_STS_ENC_IDLE_INT                             MJCOMB_CSR_STS_ENC_IDLE_INT
#define MJCOMB_CSR_STS_ENC_IDLE_INT_POS                         (12U)
#define MJCOMB_CSR_STS_ENC_IDLE_INT_LEN                         (1U)
#define MJCOMB_CSR_STS_ENC_IDLE_INT_MSK                         (((1LL<<MJCOMB_CSR_STS_ENC_IDLE_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_IDLE_INT_POS)
#define MJCOMB_CSR_STS_ENC_IDLE_INT_UMSK                        (~(((1LL<<MJCOMB_CSR_STS_ENC_IDLE_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_IDLE_INT_POS))
#define MJCOMB_CSR_STS_ENC_SWAP_INT                             MJCOMB_CSR_STS_ENC_SWAP_INT
#define MJCOMB_CSR_STS_ENC_SWAP_INT_POS                         (13U)
#define MJCOMB_CSR_STS_ENC_SWAP_INT_LEN                         (1U)
#define MJCOMB_CSR_STS_ENC_SWAP_INT_MSK                         (((1LL<<MJCOMB_CSR_STS_ENC_SWAP_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP_INT_POS)
#define MJCOMB_CSR_STS_ENC_SWAP_INT_UMSK                        (~(((1LL<<MJCOMB_CSR_STS_ENC_SWAP_INT_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP_INT_POS))
//removed //#define MJCOMB_CSR_REG_HW_KICK                                  MJCOMB_CSR_REG_HW_KICK
//removed //#define MJCOMB_CSR_REG_HW_KICK_POS                              (14U)
//removed //#define MJCOMB_CSR_REG_HW_KICK_LEN                              (1U)
//removed //#define MJCOMB_CSR_REG_HW_KICK_UMSK                             (~(((1LL<<MJCOMB_CSR_REG_HW_KICK_LEN)-1)<<MJCOMB_CSR_REG_HW_KICK_POS))
//removed //#define MJCOMB_CSR_REG_HW_KICK_MSK                              (((1LL<<MJCOMB_CSR_REG_HW_KICK_LEN)-1)<<MJCOMB_CSR_REG_HW_KICK_POS)
#define MJCOMB_CSR_STS_ENC_MJPEG_IDLE                           MJCOMB_CSR_STS_ENC_MJPEG_IDLE
#define MJCOMB_CSR_STS_ENC_MJPEG_IDLE_POS                       (16U)
#define MJCOMB_CSR_STS_ENC_MJPEG_IDLE_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_MJPEG_IDLE_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_IDLE_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_IDLE_POS)
#define MJCOMB_CSR_STS_ENC_MJPEG_IDLE_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_IDLE_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_IDLE_POS))
#define MJCOMB_CSR_STS_ENC_MJPEG_FUNC                           MJCOMB_CSR_STS_ENC_MJPEG_FUNC
#define MJCOMB_CSR_STS_ENC_MJPEG_FUNC_POS                       (17U)
#define MJCOMB_CSR_STS_ENC_MJPEG_FUNC_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_MJPEG_FUNC_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_FUNC_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_FUNC_POS)
#define MJCOMB_CSR_STS_ENC_MJPEG_FUNC_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_FUNC_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_FUNC_POS))
#define MJCOMB_CSR_STS_ENC_MJPEG_WAIT                           MJCOMB_CSR_STS_ENC_MJPEG_WAIT
#define MJCOMB_CSR_STS_ENC_MJPEG_WAIT_POS                       (18U)
#define MJCOMB_CSR_STS_ENC_MJPEG_WAIT_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_MJPEG_WAIT_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_WAIT_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_WAIT_POS)
#define MJCOMB_CSR_STS_ENC_MJPEG_WAIT_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_WAIT_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_WAIT_POS))
#define MJCOMB_CSR_STS_ENC_MJPEG_FLSH                           MJCOMB_CSR_STS_ENC_MJPEG_FLSH
#define MJCOMB_CSR_STS_ENC_MJPEG_FLSH_POS                       (19U)
#define MJCOMB_CSR_STS_ENC_MJPEG_FLSH_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_MJPEG_FLSH_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_FLSH_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_FLSH_POS)
#define MJCOMB_CSR_STS_ENC_MJPEG_FLSH_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_FLSH_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_FLSH_POS))
#define MJCOMB_CSR_STS_ENC_MJPEG_MANS                           MJCOMB_CSR_STS_ENC_MJPEG_MANS
#define MJCOMB_CSR_STS_ENC_MJPEG_MANS_POS                       (20U)
#define MJCOMB_CSR_STS_ENC_MJPEG_MANS_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_MJPEG_MANS_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_MANS_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_MANS_POS)
#define MJCOMB_CSR_STS_ENC_MJPEG_MANS_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_MANS_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_MANS_POS))
#define MJCOMB_CSR_STS_ENC_MJPEG_MANF                           MJCOMB_CSR_STS_ENC_MJPEG_MANF
#define MJCOMB_CSR_STS_ENC_MJPEG_MANF_POS                       (21U)
#define MJCOMB_CSR_STS_ENC_MJPEG_MANF_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_MJPEG_MANF_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_MANF_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_MANF_POS)
#define MJCOMB_CSR_STS_ENC_MJPEG_MANF_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_MJPEG_MANF_LEN)-1)<<MJCOMB_CSR_STS_ENC_MJPEG_MANF_POS))
#define MJCOMB_CSR_STS_ENC_AXI_READ_IDLE                        MJCOMB_CSR_STS_ENC_AXI_READ_IDLE
#define MJCOMB_CSR_STS_ENC_AXI_READ_IDLE_POS                    (22U)
#define MJCOMB_CSR_STS_ENC_AXI_READ_IDLE_LEN                    (1U)
#define MJCOMB_CSR_STS_ENC_AXI_READ_IDLE_MSK                    (((1LL<<MJCOMB_CSR_STS_ENC_AXI_READ_IDLE_LEN)-1)<<MJCOMB_CSR_STS_ENC_AXI_READ_IDLE_POS)
#define MJCOMB_CSR_STS_ENC_AXI_READ_IDLE_UMSK                   (~(((1LL<<MJCOMB_CSR_STS_ENC_AXI_READ_IDLE_LEN)-1)<<MJCOMB_CSR_STS_ENC_AXI_READ_IDLE_POS))
#define MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE                       MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE
#define MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE_POS                   (23U)
#define MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE_LEN                   (1U)
#define MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE_MSK                   (((1LL<<MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE_LEN)-1)<<MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE_POS)
#define MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE_UMSK                  (~(((1LL<<MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE_LEN)-1)<<MJCOMB_CSR_STS_ENC_AXI_WRITE_IDLE_POS))
#define MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT                      MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT
#define MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT_POS                  (24U)
#define MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT_LEN                  (5U)
#define MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT_MSK                  (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT_UMSK                 (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT_POS))

/* 0x98 : jenc_csr_6 */
#define MJCOMB_CSR_JENC_CSR_6_OFFSET                            (0x98)
#define MJCOMB_CSR_REG_ENC_Y0_ORDER                             MJCOMB_CSR_REG_ENC_Y0_ORDER
#define MJCOMB_CSR_REG_ENC_Y0_ORDER_POS                         (24U)
#define MJCOMB_CSR_REG_ENC_Y0_ORDER_LEN                         (2U)
#define MJCOMB_CSR_REG_ENC_Y0_ORDER_MSK                         (((1LL<<MJCOMB_CSR_REG_ENC_Y0_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_Y0_ORDER_POS)
#define MJCOMB_CSR_REG_ENC_Y0_ORDER_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ENC_Y0_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_Y0_ORDER_POS))
#define MJCOMB_CSR_REG_ENC_U0_ORDER                             MJCOMB_CSR_REG_ENC_U0_ORDER
#define MJCOMB_CSR_REG_ENC_U0_ORDER_POS                         (26U)
#define MJCOMB_CSR_REG_ENC_U0_ORDER_LEN                         (2U)
#define MJCOMB_CSR_REG_ENC_U0_ORDER_MSK                         (((1LL<<MJCOMB_CSR_REG_ENC_U0_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_U0_ORDER_POS)
#define MJCOMB_CSR_REG_ENC_U0_ORDER_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ENC_U0_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_U0_ORDER_POS))
#define MJCOMB_CSR_REG_ENC_Y1_ORDER                             MJCOMB_CSR_REG_ENC_Y1_ORDER
#define MJCOMB_CSR_REG_ENC_Y1_ORDER_POS                         (28U)
#define MJCOMB_CSR_REG_ENC_Y1_ORDER_LEN                         (2U)
#define MJCOMB_CSR_REG_ENC_Y1_ORDER_MSK                         (((1LL<<MJCOMB_CSR_REG_ENC_Y1_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_Y1_ORDER_POS)
#define MJCOMB_CSR_REG_ENC_Y1_ORDER_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ENC_Y1_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_Y1_ORDER_POS))
#define MJCOMB_CSR_REG_ENC_V0_ORDER                             MJCOMB_CSR_REG_ENC_V0_ORDER
#define MJCOMB_CSR_REG_ENC_V0_ORDER_POS                         (30U)
#define MJCOMB_CSR_REG_ENC_V0_ORDER_LEN                         (2U)
#define MJCOMB_CSR_REG_ENC_V0_ORDER_MSK                         (((1LL<<MJCOMB_CSR_REG_ENC_V0_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_V0_ORDER_POS)
#define MJCOMB_CSR_REG_ENC_V0_ORDER_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_ENC_V0_ORDER_LEN)-1)<<MJCOMB_CSR_REG_ENC_V0_ORDER_POS))

/* 0x9C : jenc_csr_7 */
#define MJCOMB_CSR_JENC_CSR_7_OFFSET                            (0x9C)
#define MJCOMB_CSR_STS_ENC_SWAP0_FULL                           MJCOMB_CSR_STS_ENC_SWAP0_FULL
#define MJCOMB_CSR_STS_ENC_SWAP0_FULL_POS                       (8U)
#define MJCOMB_CSR_STS_ENC_SWAP0_FULL_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_SWAP0_FULL_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_SWAP0_FULL_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP0_FULL_POS)
#define MJCOMB_CSR_STS_ENC_SWAP0_FULL_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_SWAP0_FULL_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP0_FULL_POS))
#define MJCOMB_CSR_STS_ENC_SWAP1_FULL                           MJCOMB_CSR_STS_ENC_SWAP1_FULL
#define MJCOMB_CSR_STS_ENC_SWAP1_FULL_POS                       (9U)
#define MJCOMB_CSR_STS_ENC_SWAP1_FULL_LEN                       (1U)
#define MJCOMB_CSR_STS_ENC_SWAP1_FULL_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_SWAP1_FULL_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP1_FULL_POS)
#define MJCOMB_CSR_STS_ENC_SWAP1_FULL_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_SWAP1_FULL_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP1_FULL_POS))
#define MJCOMB_CSR_STS_ENC_READ_SWAP_IDX                        MJCOMB_CSR_STS_ENC_READ_SWAP_IDX
#define MJCOMB_CSR_STS_ENC_READ_SWAP_IDX_POS                    (10U)
#define MJCOMB_CSR_STS_ENC_READ_SWAP_IDX_LEN                    (1U)
#define MJCOMB_CSR_STS_ENC_READ_SWAP_IDX_MSK                    (((1LL<<MJCOMB_CSR_STS_ENC_READ_SWAP_IDX_LEN)-1)<<MJCOMB_CSR_STS_ENC_READ_SWAP_IDX_POS)
#define MJCOMB_CSR_STS_ENC_READ_SWAP_IDX_UMSK                   (~(((1LL<<MJCOMB_CSR_STS_ENC_READ_SWAP_IDX_LEN)-1)<<MJCOMB_CSR_STS_ENC_READ_SWAP_IDX_POS))
#define MJCOMB_CSR_STS_ENC_SWAP_FSTART                          MJCOMB_CSR_STS_ENC_SWAP_FSTART
#define MJCOMB_CSR_STS_ENC_SWAP_FSTART_POS                      (11U)
#define MJCOMB_CSR_STS_ENC_SWAP_FSTART_LEN                      (1U)
#define MJCOMB_CSR_STS_ENC_SWAP_FSTART_MSK                      (((1LL<<MJCOMB_CSR_STS_ENC_SWAP_FSTART_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP_FSTART_POS)
#define MJCOMB_CSR_STS_ENC_SWAP_FSTART_UMSK                     (~(((1LL<<MJCOMB_CSR_STS_ENC_SWAP_FSTART_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP_FSTART_POS))
#define MJCOMB_CSR_STS_ENC_SWAP_FEND                            MJCOMB_CSR_STS_ENC_SWAP_FEND
#define MJCOMB_CSR_STS_ENC_SWAP_FEND_POS                        (12U)
#define MJCOMB_CSR_STS_ENC_SWAP_FEND_LEN                        (1U)
#define MJCOMB_CSR_STS_ENC_SWAP_FEND_MSK                        (((1LL<<MJCOMB_CSR_STS_ENC_SWAP_FEND_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP_FEND_POS)
#define MJCOMB_CSR_STS_ENC_SWAP_FEND_UMSK                       (~(((1LL<<MJCOMB_CSR_STS_ENC_SWAP_FEND_LEN)-1)<<MJCOMB_CSR_STS_ENC_SWAP_FEND_POS))

/* 0xA0 : jenc_csr_8 */
#define MJCOMB_CSR_JENC_CSR_8_OFFSET                            (0xA0)
#define MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT               MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT
#define MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT_POS           (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT_LEN           (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT_MSK           (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT_UMSK          (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_SWAP_END_BIT_CNT_POS))

/* 0xA4 : jenc_csr_9 */
#define MJCOMB_CSR_JENC_CSR_9_OFFSET                            (0xA4)
#define MJCOMB_CSR_REG_ENC_SW_KICK_HBLK                         MJCOMB_CSR_REG_ENC_SW_KICK_HBLK
#define MJCOMB_CSR_REG_ENC_SW_KICK_HBLK_POS                     (0U)
#define MJCOMB_CSR_REG_ENC_SW_KICK_HBLK_LEN                     (13U)
#define MJCOMB_CSR_REG_ENC_SW_KICK_HBLK_MSK                     (((1LL<<MJCOMB_CSR_REG_ENC_SW_KICK_HBLK_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_KICK_HBLK_POS)
#define MJCOMB_CSR_REG_ENC_SW_KICK_HBLK_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_ENC_SW_KICK_HBLK_LEN)-1)<<MJCOMB_CSR_REG_ENC_SW_KICK_HBLK_POS))

/* 0xA8 : jenc_csr_10 */
#define MJCOMB_CSR_JENC_CSR_10_OFFSET                           (0xA8)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0                   MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0_POS               (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0_LEN               (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0_MSK               (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0_UMSK              (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_0_POS))

/* 0xAC : jenc_csr_11 */
#define MJCOMB_CSR_JENC_CSR_11_OFFSET                           (0xAC)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0                      MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0_POS                  (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0_LEN                  (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0_MSK                  (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0_UMSK                 (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_0_POS))

/* 0xB0 : jenc_csr_12 */
#define MJCOMB_CSR_JENC_CSR_12_OFFSET                           (0xB0)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1                   MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1_POS               (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1_LEN               (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1_MSK               (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1_UMSK              (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_1_POS))

/* 0xB4 : jenc_csr_13 */
#define MJCOMB_CSR_JENC_CSR_13_OFFSET                           (0xB4)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1                      MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1_POS                  (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1_LEN                  (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1_MSK                  (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1_UMSK                 (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_1_POS))

/* 0xB8 : jenc_csr_14 */
#define MJCOMB_CSR_JENC_CSR_14_OFFSET                           (0xB8)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2                   MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2_POS               (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2_LEN               (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2_MSK               (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2_UMSK              (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_2_POS))

/* 0xBC : jenc_csr_15 */
#define MJCOMB_CSR_JENC_CSR_15_OFFSET                           (0xBC)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2                      MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2_POS                  (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2_LEN                  (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2_MSK                  (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2_UMSK                 (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_2_POS))

/* 0xC0 : jenc_csr_16 */
#define MJCOMB_CSR_JENC_CSR_16_OFFSET                           (0xC0)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3                   MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3_POS               (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3_LEN               (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3_MSK               (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3_UMSK              (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_START_ADDR_3_POS))

/* 0xC4 : jenc_csr_17 */
#define MJCOMB_CSR_JENC_CSR_17_OFFSET                           (0xC4)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3                      MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3_POS                  (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3_LEN                  (32U)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3_MSK                  (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3_UMSK                 (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_BIT_CNT_3_POS))

/* 0xC8 : jenc_csr_18 */
#define MJCOMB_CSR_JENC_CSR_18_OFFSET                           (0xC8)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_0                           MJCOMB_CSR_STS_ENC_FRAME_ID_0
#define MJCOMB_CSR_STS_ENC_FRAME_ID_0_POS                       (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_0_LEN                       (16U)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_0_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_ID_0_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_ID_0_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_0_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_ID_0_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_ID_0_POS))
#define MJCOMB_CSR_STS_ENC_FRAME_ID_1                           MJCOMB_CSR_STS_ENC_FRAME_ID_1
#define MJCOMB_CSR_STS_ENC_FRAME_ID_1_POS                       (16U)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_1_LEN                       (16U)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_1_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_ID_1_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_ID_1_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_1_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_ID_1_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_ID_1_POS))

/* 0xCC : jenc_csr_19 */
#define MJCOMB_CSR_JENC_CSR_19_OFFSET                           (0xCC)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_2                           MJCOMB_CSR_STS_ENC_FRAME_ID_2
#define MJCOMB_CSR_STS_ENC_FRAME_ID_2_POS                       (0U)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_2_LEN                       (16U)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_2_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_ID_2_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_ID_2_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_2_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_ID_2_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_ID_2_POS))
#define MJCOMB_CSR_STS_ENC_FRAME_ID_3                           MJCOMB_CSR_STS_ENC_FRAME_ID_3
#define MJCOMB_CSR_STS_ENC_FRAME_ID_3_POS                       (16U)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_3_LEN                       (16U)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_3_MSK                       (((1LL<<MJCOMB_CSR_STS_ENC_FRAME_ID_3_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_ID_3_POS)
#define MJCOMB_CSR_STS_ENC_FRAME_ID_3_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_ENC_FRAME_ID_3_LEN)-1)<<MJCOMB_CSR_STS_ENC_FRAME_ID_3_POS))

/* 0x100 : jdec_csr_0 */
#define MJCOMB_CSR_JDEC_CSR_0_OFFSET                            (0x100)
#define MJCOMB_CSR_REG_DEC_JSTR_HW_EN                           MJCOMB_CSR_REG_DEC_JSTR_HW_EN
#define MJCOMB_CSR_REG_DEC_JSTR_HW_EN_POS                       (7U)
#define MJCOMB_CSR_REG_DEC_JSTR_HW_EN_LEN                       (1U)
#define MJCOMB_CSR_REG_DEC_JSTR_HW_EN_MSK                       (((1LL<<MJCOMB_CSR_REG_DEC_JSTR_HW_EN_LEN)-1)<<MJCOMB_CSR_REG_DEC_JSTR_HW_EN_POS)
#define MJCOMB_CSR_REG_DEC_JSTR_HW_EN_UMSK                      (~(((1LL<<MJCOMB_CSR_REG_DEC_JSTR_HW_EN_LEN)-1)<<MJCOMB_CSR_REG_DEC_JSTR_HW_EN_POS))
#define MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY                      MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY
#define MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY_POS                  (11U)
#define MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY_LEN                  (1U)
#define MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY_MSK                  (((1LL<<MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY_LEN)-1)<<MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY_POS)
#define MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY_UMSK                 (~(((1LL<<MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY_LEN)-1)<<MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY_POS))
#define MJCOMB_CSR_REG_DEC_R_XLEN                               MJCOMB_CSR_REG_DEC_R_XLEN
#define MJCOMB_CSR_REG_DEC_R_XLEN_POS                           (12U)
#define MJCOMB_CSR_REG_DEC_R_XLEN_LEN                           (3U)
#define MJCOMB_CSR_REG_DEC_R_XLEN_MSK                           (((1LL<<MJCOMB_CSR_REG_DEC_R_XLEN_LEN)-1)<<MJCOMB_CSR_REG_DEC_R_XLEN_POS)
#define MJCOMB_CSR_REG_DEC_R_XLEN_UMSK                          (~(((1LL<<MJCOMB_CSR_REG_DEC_R_XLEN_LEN)-1)<<MJCOMB_CSR_REG_DEC_R_XLEN_POS))
#define MJCOMB_CSR_REG_DEC_BLK_MODE_EN                          MJCOMB_CSR_REG_DEC_BLK_MODE_EN
#define MJCOMB_CSR_REG_DEC_BLK_MODE_EN_POS                      (31U)
#define MJCOMB_CSR_REG_DEC_BLK_MODE_EN_LEN                      (1U)
#define MJCOMB_CSR_REG_DEC_BLK_MODE_EN_MSK                      (((1LL<<MJCOMB_CSR_REG_DEC_BLK_MODE_EN_LEN)-1)<<MJCOMB_CSR_REG_DEC_BLK_MODE_EN_POS)
#define MJCOMB_CSR_REG_DEC_BLK_MODE_EN_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_DEC_BLK_MODE_EN_LEN)-1)<<MJCOMB_CSR_REG_DEC_BLK_MODE_EN_POS))

/* 0x104 : jdec_csr_1 */
#define MJCOMB_CSR_JDEC_CSR_1_OFFSET                            (0x104)
#define MJCOMB_CSR_REG_DEC_INT_NORMAL_EN                        MJCOMB_CSR_REG_DEC_INT_NORMAL_EN
#define MJCOMB_CSR_REG_DEC_INT_NORMAL_EN_POS                    (0U)
#define MJCOMB_CSR_REG_DEC_INT_NORMAL_EN_LEN                    (1U)
#define MJCOMB_CSR_REG_DEC_INT_NORMAL_EN_MSK                    (((1LL<<MJCOMB_CSR_REG_DEC_INT_NORMAL_EN_LEN)-1)<<MJCOMB_CSR_REG_DEC_INT_NORMAL_EN_POS)
#define MJCOMB_CSR_REG_DEC_INT_NORMAL_EN_UMSK                   (~(((1LL<<MJCOMB_CSR_REG_DEC_INT_NORMAL_EN_LEN)-1)<<MJCOMB_CSR_REG_DEC_INT_NORMAL_EN_POS))
#define MJCOMB_CSR_REG_DEC_INT_IDLE_EN                          MJCOMB_CSR_REG_DEC_INT_IDLE_EN
#define MJCOMB_CSR_REG_DEC_INT_IDLE_EN_POS                      (1U)
#define MJCOMB_CSR_REG_DEC_INT_IDLE_EN_LEN                      (1U)
#define MJCOMB_CSR_REG_DEC_INT_IDLE_EN_MSK                      (((1LL<<MJCOMB_CSR_REG_DEC_INT_IDLE_EN_LEN)-1)<<MJCOMB_CSR_REG_DEC_INT_IDLE_EN_POS)
#define MJCOMB_CSR_REG_DEC_INT_IDLE_EN_UMSK                     (~(((1LL<<MJCOMB_CSR_REG_DEC_INT_IDLE_EN_LEN)-1)<<MJCOMB_CSR_REG_DEC_INT_IDLE_EN_POS))
#define MJCOMB_CSR_STS_DEC_NORMAL_INT                           MJCOMB_CSR_STS_DEC_NORMAL_INT
#define MJCOMB_CSR_STS_DEC_NORMAL_INT_POS                       (4U)
#define MJCOMB_CSR_STS_DEC_NORMAL_INT_LEN                       (1U)
#define MJCOMB_CSR_STS_DEC_NORMAL_INT_MSK                       (((1LL<<MJCOMB_CSR_STS_DEC_NORMAL_INT_LEN)-1)<<MJCOMB_CSR_STS_DEC_NORMAL_INT_POS)
#define MJCOMB_CSR_STS_DEC_NORMAL_INT_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_DEC_NORMAL_INT_LEN)-1)<<MJCOMB_CSR_STS_DEC_NORMAL_INT_POS))
#define MJCOMB_CSR_STS_DEC_IDLE_INT                             MJCOMB_CSR_STS_DEC_IDLE_INT
#define MJCOMB_CSR_STS_DEC_IDLE_INT_POS                         (5U)
#define MJCOMB_CSR_STS_DEC_IDLE_INT_LEN                         (1U)
#define MJCOMB_CSR_STS_DEC_IDLE_INT_MSK                         (((1LL<<MJCOMB_CSR_STS_DEC_IDLE_INT_LEN)-1)<<MJCOMB_CSR_STS_DEC_IDLE_INT_POS)
#define MJCOMB_CSR_STS_DEC_IDLE_INT_UMSK                        (~(((1LL<<MJCOMB_CSR_STS_DEC_IDLE_INT_LEN)-1)<<MJCOMB_CSR_STS_DEC_IDLE_INT_POS))
#define MJCOMB_CSR_STS_DEC_MJPEG_IDLE                           MJCOMB_CSR_STS_DEC_MJPEG_IDLE
#define MJCOMB_CSR_STS_DEC_MJPEG_IDLE_POS                       (8U)
#define MJCOMB_CSR_STS_DEC_MJPEG_IDLE_LEN                       (1U)
#define MJCOMB_CSR_STS_DEC_MJPEG_IDLE_MSK                       (((1LL<<MJCOMB_CSR_STS_DEC_MJPEG_IDLE_LEN)-1)<<MJCOMB_CSR_STS_DEC_MJPEG_IDLE_POS)
#define MJCOMB_CSR_STS_DEC_MJPEG_IDLE_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_DEC_MJPEG_IDLE_LEN)-1)<<MJCOMB_CSR_STS_DEC_MJPEG_IDLE_POS))
#define MJCOMB_CSR_STS_DEC_MJPEG_FUNC                           MJCOMB_CSR_STS_DEC_MJPEG_FUNC
#define MJCOMB_CSR_STS_DEC_MJPEG_FUNC_POS                       (9U)
#define MJCOMB_CSR_STS_DEC_MJPEG_FUNC_LEN                       (1U)
#define MJCOMB_CSR_STS_DEC_MJPEG_FUNC_MSK                       (((1LL<<MJCOMB_CSR_STS_DEC_MJPEG_FUNC_LEN)-1)<<MJCOMB_CSR_STS_DEC_MJPEG_FUNC_POS)
#define MJCOMB_CSR_STS_DEC_MJPEG_FUNC_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_DEC_MJPEG_FUNC_LEN)-1)<<MJCOMB_CSR_STS_DEC_MJPEG_FUNC_POS))
#define MJCOMB_CSR_STS_DEC_MJPEG_FLSH                           MJCOMB_CSR_STS_DEC_MJPEG_FLSH
#define MJCOMB_CSR_STS_DEC_MJPEG_FLSH_POS                       (11U)
#define MJCOMB_CSR_STS_DEC_MJPEG_FLSH_LEN                       (1U)
#define MJCOMB_CSR_STS_DEC_MJPEG_FLSH_MSK                       (((1LL<<MJCOMB_CSR_STS_DEC_MJPEG_FLSH_LEN)-1)<<MJCOMB_CSR_STS_DEC_MJPEG_FLSH_POS)
#define MJCOMB_CSR_STS_DEC_MJPEG_FLSH_UMSK                      (~(((1LL<<MJCOMB_CSR_STS_DEC_MJPEG_FLSH_LEN)-1)<<MJCOMB_CSR_STS_DEC_MJPEG_FLSH_POS))
#define MJCOMB_CSR_STS_DEC_AXI_READ_IDLE                        MJCOMB_CSR_STS_DEC_AXI_READ_IDLE
#define MJCOMB_CSR_STS_DEC_AXI_READ_IDLE_POS                    (14U)
#define MJCOMB_CSR_STS_DEC_AXI_READ_IDLE_LEN                    (1U)
#define MJCOMB_CSR_STS_DEC_AXI_READ_IDLE_MSK                    (((1LL<<MJCOMB_CSR_STS_DEC_AXI_READ_IDLE_LEN)-1)<<MJCOMB_CSR_STS_DEC_AXI_READ_IDLE_POS)
#define MJCOMB_CSR_STS_DEC_AXI_READ_IDLE_UMSK                   (~(((1LL<<MJCOMB_CSR_STS_DEC_AXI_READ_IDLE_LEN)-1)<<MJCOMB_CSR_STS_DEC_AXI_READ_IDLE_POS))
#define MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE                       MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE
#define MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE_POS                   (15U)
#define MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE_LEN                   (1U)
#define MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE_MSK                   (((1LL<<MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE_LEN)-1)<<MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE_POS)
#define MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE_UMSK                  (~(((1LL<<MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE_LEN)-1)<<MJCOMB_CSR_STS_DEC_AXI_WRITE_IDLE_POS))
#define MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT                   MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT
#define MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT_POS               (16U)
#define MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT_LEN               (5U)
#define MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT_MSK               (((1LL<<MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT_LEN)-1)<<MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT_POS)
#define MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT_UMSK              (~(((1LL<<MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT_LEN)-1)<<MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT_POS))
#define MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT                      MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT
#define MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT_POS                  (24U)
#define MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT_LEN                  (8U)
#define MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT_MSK                  (((1LL<<MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT_LEN)-1)<<MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT_POS)
#define MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT_UMSK                 (~(((1LL<<MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT_LEN)-1)<<MJCOMB_CSR_STS_DEC_FRAME_VALID_CNT_POS))

/* 0x108 : jdec_csr_2 */
#define MJCOMB_CSR_JDEC_CSR_2_OFFSET                            (0x108)
#define MJCOMB_CSR_REG_DEC_INT_CLR                              MJCOMB_CSR_REG_DEC_INT_CLR
#define MJCOMB_CSR_REG_DEC_INT_CLR_POS                          (0U)
#define MJCOMB_CSR_REG_DEC_INT_CLR_LEN                          (1U)
#define MJCOMB_CSR_REG_DEC_INT_CLR_MSK                          (((1LL<<MJCOMB_CSR_REG_DEC_INT_CLR_LEN)-1)<<MJCOMB_CSR_REG_DEC_INT_CLR_POS)
#define MJCOMB_CSR_REG_DEC_INT_CLR_UMSK                         (~(((1LL<<MJCOMB_CSR_REG_DEC_INT_CLR_LEN)-1)<<MJCOMB_CSR_REG_DEC_INT_CLR_POS))
#define MJCOMB_CSR_REG_DEC_YUV_FRAME_POP                        MJCOMB_CSR_REG_DEC_YUV_FRAME_POP
#define MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_POS                    (1U)
#define MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_LEN                    (1U)
#define MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_MSK                    (((1LL<<MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_LEN)-1)<<MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_POS)
#define MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_UMSK                   (~(((1LL<<MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_LEN)-1)<<MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_POS))
#define MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE                        MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE
#define MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE_POS                    (2U)
#define MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE_LEN                    (1U)
#define MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE_MSK                    (((1LL<<MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE_LEN)-1)<<MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE_POS)
#define MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE_UMSK                   (~(((1LL<<MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE_LEN)-1)<<MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE_POS))
#define MJCOMB_CSR_STS_DEC_HUF_CAL_DONE                         MJCOMB_CSR_STS_DEC_HUF_CAL_DONE
#define MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_POS                     (18U)
#define MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_LEN                     (1U)
#define MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_MSK                     (((1LL<<MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_LEN)-1)<<MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_POS)
#define MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_UMSK                    (~(((1LL<<MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_LEN)-1)<<MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_POS))

/* 0x10C : jdec_csr_3 */
#define MJCOMB_CSR_JDEC_CSR_3_OFFSET                            (0x10C)
#define MJCOMB_CSR_STS_DEC_JP_FRAME_CNT                         MJCOMB_CSR_STS_DEC_JP_FRAME_CNT
#define MJCOMB_CSR_STS_DEC_JP_FRAME_CNT_POS                     (0U)
#define MJCOMB_CSR_STS_DEC_JP_FRAME_CNT_LEN                     (3U)
#define MJCOMB_CSR_STS_DEC_JP_FRAME_CNT_MSK                     (((1LL<<MJCOMB_CSR_STS_DEC_JP_FRAME_CNT_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_FRAME_CNT_POS)
#define MJCOMB_CSR_STS_DEC_JP_FRAME_CNT_UMSK                    (~(((1LL<<MJCOMB_CSR_STS_DEC_JP_FRAME_CNT_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_FRAME_CNT_POS))

/* 0x110 : jdec_csr_4 */
#define MJCOMB_CSR_JDEC_CSR_4_OFFSET                            (0x110)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_0                            MJCOMB_CSR_STS_DEC_JP_ADDR_0
#define MJCOMB_CSR_STS_DEC_JP_ADDR_0_POS                        (0U)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_0_LEN                        (32U)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_0_MSK                        (((1LL<<MJCOMB_CSR_STS_DEC_JP_ADDR_0_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_ADDR_0_POS)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_0_UMSK                       (~(((1LL<<MJCOMB_CSR_STS_DEC_JP_ADDR_0_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_ADDR_0_POS))

/* 0x114 : jdec_csr_5 */
#define MJCOMB_CSR_JDEC_CSR_5_OFFSET                            (0x114)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_1                            MJCOMB_CSR_STS_DEC_JP_ADDR_1
#define MJCOMB_CSR_STS_DEC_JP_ADDR_1_POS                        (0U)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_1_LEN                        (32U)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_1_MSK                        (((1LL<<MJCOMB_CSR_STS_DEC_JP_ADDR_1_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_ADDR_1_POS)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_1_UMSK                       (~(((1LL<<MJCOMB_CSR_STS_DEC_JP_ADDR_1_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_ADDR_1_POS))

/* 0x118 : jdec_csr_6 */
#define MJCOMB_CSR_JDEC_CSR_6_OFFSET                            (0x118)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_2                            MJCOMB_CSR_STS_DEC_JP_ADDR_2
#define MJCOMB_CSR_STS_DEC_JP_ADDR_2_POS                        (0U)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_2_LEN                        (32U)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_2_MSK                        (((1LL<<MJCOMB_CSR_STS_DEC_JP_ADDR_2_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_ADDR_2_POS)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_2_UMSK                       (~(((1LL<<MJCOMB_CSR_STS_DEC_JP_ADDR_2_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_ADDR_2_POS))

/* 0x11C : jdec_csr_7 */
#define MJCOMB_CSR_JDEC_CSR_7_OFFSET                            (0x11C)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_3                            MJCOMB_CSR_STS_DEC_JP_ADDR_3
#define MJCOMB_CSR_STS_DEC_JP_ADDR_3_POS                        (0U)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_3_LEN                        (32U)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_3_MSK                        (((1LL<<MJCOMB_CSR_STS_DEC_JP_ADDR_3_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_ADDR_3_POS)
#define MJCOMB_CSR_STS_DEC_JP_ADDR_3_UMSK                       (~(((1LL<<MJCOMB_CSR_STS_DEC_JP_ADDR_3_LEN)-1)<<MJCOMB_CSR_STS_DEC_JP_ADDR_3_POS))

/* 0x200 : jdec_yy_dc_cfg0 */
#define MJCOMB_CSR_JDEC_YY_DC_CFG0_OFFSET                       (0x200)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_01                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_01
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_01_POS                     (0U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_01_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_01_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_01_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_01_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_01_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_01_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_01_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_02                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_02
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_02_POS                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_02_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_02_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_02_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_02_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_02_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_02_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_02_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_03                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_03
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_03_POS                     (8U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_03_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_03_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_03_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_03_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_03_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_03_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_03_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_04                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_04
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_04_POS                     (12U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_04_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_04_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_04_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_04_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_04_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_04_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_04_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_05                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_05
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_05_POS                     (16U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_05_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_05_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_05_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_05_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_05_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_05_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_05_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_06                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_06
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_06_POS                     (20U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_06_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_06_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_06_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_06_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_06_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_06_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_06_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_07                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_07
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_07_POS                     (24U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_07_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_07_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_07_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_07_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_07_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_07_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_07_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_08                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_08
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_08_POS                     (28U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_08_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_08_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_08_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_08_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_08_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_08_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_08_POS))

/* 0x204 : jdec_yy_dc_cfg1 */
#define MJCOMB_CSR_JDEC_YY_DC_CFG1_OFFSET                       (0x204)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_09                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_09
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_09_POS                     (0U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_09_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_09_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_09_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_09_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_09_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_09_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_09_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A_POS                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0A_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B_POS                     (8U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0B_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C_POS                     (12U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0C_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D_POS                     (16U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0D_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E_POS                     (20U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0E_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F_POS                     (24U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_0F_POS))
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_10                         MJCOMB_CSR_REG_YY_DC_LEN_CNT_10
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_10_POS                     (28U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_10_LEN                     (4U)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_10_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_10_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_10_POS)
#define MJCOMB_CSR_REG_YY_DC_LEN_CNT_10_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_10_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_LEN_CNT_10_POS))

/* 0x208 : jdec_yy_dc_cfg2 */
#define MJCOMB_CSR_JDEC_YY_DC_CFG2_OFFSET                       (0x208)
#define MJCOMB_CSR_REG_YY_DC_VAL_00                             MJCOMB_CSR_REG_YY_DC_VAL_00
#define MJCOMB_CSR_REG_YY_DC_VAL_00_POS                         (0U)
#define MJCOMB_CSR_REG_YY_DC_VAL_00_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_00_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_00_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_00_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_00_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_00_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_00_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_01                             MJCOMB_CSR_REG_YY_DC_VAL_01
#define MJCOMB_CSR_REG_YY_DC_VAL_01_POS                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_01_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_01_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_01_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_01_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_01_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_01_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_01_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_02                             MJCOMB_CSR_REG_YY_DC_VAL_02
#define MJCOMB_CSR_REG_YY_DC_VAL_02_POS                         (8U)
#define MJCOMB_CSR_REG_YY_DC_VAL_02_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_02_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_02_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_02_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_02_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_02_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_02_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_03                             MJCOMB_CSR_REG_YY_DC_VAL_03
#define MJCOMB_CSR_REG_YY_DC_VAL_03_POS                         (12U)
#define MJCOMB_CSR_REG_YY_DC_VAL_03_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_03_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_03_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_03_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_03_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_03_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_03_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_04                             MJCOMB_CSR_REG_YY_DC_VAL_04
#define MJCOMB_CSR_REG_YY_DC_VAL_04_POS                         (16U)
#define MJCOMB_CSR_REG_YY_DC_VAL_04_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_04_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_04_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_04_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_04_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_04_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_04_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_05                             MJCOMB_CSR_REG_YY_DC_VAL_05
#define MJCOMB_CSR_REG_YY_DC_VAL_05_POS                         (20U)
#define MJCOMB_CSR_REG_YY_DC_VAL_05_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_05_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_05_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_05_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_05_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_05_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_05_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_06                             MJCOMB_CSR_REG_YY_DC_VAL_06
#define MJCOMB_CSR_REG_YY_DC_VAL_06_POS                         (24U)
#define MJCOMB_CSR_REG_YY_DC_VAL_06_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_06_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_06_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_06_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_06_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_06_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_06_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_07                             MJCOMB_CSR_REG_YY_DC_VAL_07
#define MJCOMB_CSR_REG_YY_DC_VAL_07_POS                         (28U)
#define MJCOMB_CSR_REG_YY_DC_VAL_07_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_07_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_07_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_07_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_07_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_07_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_07_POS))

/* 0x20C : jdec_yy_dc_cfg3 */
#define MJCOMB_CSR_JDEC_YY_DC_CFG3_OFFSET                       (0x20C)
#define MJCOMB_CSR_REG_YY_DC_VAL_08                             MJCOMB_CSR_REG_YY_DC_VAL_08
#define MJCOMB_CSR_REG_YY_DC_VAL_08_POS                         (0U)
#define MJCOMB_CSR_REG_YY_DC_VAL_08_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_08_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_08_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_08_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_08_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_08_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_08_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_09                             MJCOMB_CSR_REG_YY_DC_VAL_09
#define MJCOMB_CSR_REG_YY_DC_VAL_09_POS                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_09_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_09_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_09_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_09_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_09_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_09_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_09_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_0A                             MJCOMB_CSR_REG_YY_DC_VAL_0A
#define MJCOMB_CSR_REG_YY_DC_VAL_0A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_DC_VAL_0A_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_0A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_0A_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_0A_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_0A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_0A_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_0A_POS))
#define MJCOMB_CSR_REG_YY_DC_VAL_0B                             MJCOMB_CSR_REG_YY_DC_VAL_0B
#define MJCOMB_CSR_REG_YY_DC_VAL_0B_POS                         (12U)
#define MJCOMB_CSR_REG_YY_DC_VAL_0B_LEN                         (4U)
#define MJCOMB_CSR_REG_YY_DC_VAL_0B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_0B_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_0B_POS)
#define MJCOMB_CSR_REG_YY_DC_VAL_0B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_DC_VAL_0B_LEN)-1)<<MJCOMB_CSR_REG_YY_DC_VAL_0B_POS))

/* 0x220 : jdec_yy_ac_cfg0 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG0_OFFSET                       (0x220)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_01                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_01
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_01_POS                     (0U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_01_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_01_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_01_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_01_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_01_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_01_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_01_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_02                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_02
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_02_POS                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_02_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_02_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_02_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_02_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_02_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_02_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_02_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_03                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_03
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_03_POS                     (16U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_03_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_03_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_03_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_03_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_03_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_03_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_03_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_04                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_04
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_04_POS                     (24U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_04_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_04_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_04_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_04_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_04_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_04_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_04_POS))

/* 0x224 : jdec_yy_ac_cfg1 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG1_OFFSET                       (0x224)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_05                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_05
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_05_POS                     (0U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_05_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_05_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_05_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_05_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_05_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_05_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_05_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_06                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_06
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_06_POS                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_06_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_06_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_06_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_06_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_06_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_06_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_06_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_07                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_07
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_07_POS                     (16U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_07_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_07_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_07_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_07_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_07_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_07_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_07_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_08                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_08
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_08_POS                     (24U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_08_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_08_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_08_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_08_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_08_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_08_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_08_POS))

/* 0x228 : jdec_yy_ac_cfg2 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG2_OFFSET                       (0x228)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_09                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_09
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_09_POS                     (0U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_09_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_09_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_09_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_09_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_09_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_09_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_09_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A_POS                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0A_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B_POS                     (16U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0B_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C_POS                     (24U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0C_POS))

/* 0x22C : jdec_yy_ac_cfg3 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG3_OFFSET                       (0x22C)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D_POS                     (0U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0D_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E_POS                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0E_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F_POS                     (16U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_0F_POS))
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_10                         MJCOMB_CSR_REG_YY_AC_LEN_CNT_10
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_10_POS                     (24U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_10_LEN                     (8U)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_10_MSK                     (((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_10_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_10_POS)
#define MJCOMB_CSR_REG_YY_AC_LEN_CNT_10_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_10_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_LEN_CNT_10_POS))

/* 0x230 : jdec_yy_ac_cfg4 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG4_OFFSET                       (0x230)
#define MJCOMB_CSR_REG_YY_AC_VAL_01                             MJCOMB_CSR_REG_YY_AC_VAL_01
#define MJCOMB_CSR_REG_YY_AC_VAL_01_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_01_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_01_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_01_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_01_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_01_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_01_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_01_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_02                             MJCOMB_CSR_REG_YY_AC_VAL_02
#define MJCOMB_CSR_REG_YY_AC_VAL_02_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_02_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_02_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_02_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_02_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_02_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_02_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_02_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_03                             MJCOMB_CSR_REG_YY_AC_VAL_03
#define MJCOMB_CSR_REG_YY_AC_VAL_03_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_03_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_03_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_03_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_03_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_03_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_03_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_03_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_04                             MJCOMB_CSR_REG_YY_AC_VAL_04
#define MJCOMB_CSR_REG_YY_AC_VAL_04_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_04_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_04_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_04_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_04_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_04_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_04_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_04_POS))

/* 0x234 : jdec_yy_ac_cfg5 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG5_OFFSET                       (0x234)
#define MJCOMB_CSR_REG_YY_AC_VAL_05                             MJCOMB_CSR_REG_YY_AC_VAL_05
#define MJCOMB_CSR_REG_YY_AC_VAL_05_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_05_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_05_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_05_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_05_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_05_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_05_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_05_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_06                             MJCOMB_CSR_REG_YY_AC_VAL_06
#define MJCOMB_CSR_REG_YY_AC_VAL_06_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_06_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_06_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_06_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_06_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_06_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_06_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_06_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_07                             MJCOMB_CSR_REG_YY_AC_VAL_07
#define MJCOMB_CSR_REG_YY_AC_VAL_07_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_07_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_07_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_07_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_07_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_07_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_07_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_07_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_08                             MJCOMB_CSR_REG_YY_AC_VAL_08
#define MJCOMB_CSR_REG_YY_AC_VAL_08_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_08_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_08_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_08_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_08_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_08_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_08_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_08_POS))

/* 0x238 : jdec_yy_ac_cfg6 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG6_OFFSET                       (0x238)
#define MJCOMB_CSR_REG_YY_AC_VAL_09                             MJCOMB_CSR_REG_YY_AC_VAL_09
#define MJCOMB_CSR_REG_YY_AC_VAL_09_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_09_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_09_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_09_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_09_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_09_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_09_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_09_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_0A                             MJCOMB_CSR_REG_YY_AC_VAL_0A
#define MJCOMB_CSR_REG_YY_AC_VAL_0A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_0A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_0B                             MJCOMB_CSR_REG_YY_AC_VAL_0B
#define MJCOMB_CSR_REG_YY_AC_VAL_0B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_0B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_0C                             MJCOMB_CSR_REG_YY_AC_VAL_0C
#define MJCOMB_CSR_REG_YY_AC_VAL_0C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_0C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0C_POS))

/* 0x23C : jdec_yy_ac_cfg7 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG7_OFFSET                       (0x23C)
#define MJCOMB_CSR_REG_YY_AC_VAL_0D                             MJCOMB_CSR_REG_YY_AC_VAL_0D
#define MJCOMB_CSR_REG_YY_AC_VAL_0D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_0D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_0E                             MJCOMB_CSR_REG_YY_AC_VAL_0E
#define MJCOMB_CSR_REG_YY_AC_VAL_0E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_0E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_0F                             MJCOMB_CSR_REG_YY_AC_VAL_0F
#define MJCOMB_CSR_REG_YY_AC_VAL_0F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_0F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_0F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_0F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_0F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_10                             MJCOMB_CSR_REG_YY_AC_VAL_10
#define MJCOMB_CSR_REG_YY_AC_VAL_10_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_10_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_10_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_10_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_10_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_10_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_10_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_10_POS))

/* 0x240 : jdec_yy_ac_cfg8 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG8_OFFSET                       (0x240)
#define MJCOMB_CSR_REG_YY_AC_VAL_11                             MJCOMB_CSR_REG_YY_AC_VAL_11
#define MJCOMB_CSR_REG_YY_AC_VAL_11_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_11_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_11_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_11_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_11_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_11_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_11_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_11_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_12                             MJCOMB_CSR_REG_YY_AC_VAL_12
#define MJCOMB_CSR_REG_YY_AC_VAL_12_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_12_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_12_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_12_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_12_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_12_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_12_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_12_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_13                             MJCOMB_CSR_REG_YY_AC_VAL_13
#define MJCOMB_CSR_REG_YY_AC_VAL_13_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_13_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_13_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_13_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_13_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_13_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_13_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_13_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_14                             MJCOMB_CSR_REG_YY_AC_VAL_14
#define MJCOMB_CSR_REG_YY_AC_VAL_14_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_14_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_14_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_14_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_14_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_14_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_14_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_14_POS))

/* 0x244 : jdec_yy_ac_cfg9 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG9_OFFSET                       (0x244)
#define MJCOMB_CSR_REG_YY_AC_VAL_15                             MJCOMB_CSR_REG_YY_AC_VAL_15
#define MJCOMB_CSR_REG_YY_AC_VAL_15_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_15_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_15_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_15_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_15_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_15_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_15_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_15_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_16                             MJCOMB_CSR_REG_YY_AC_VAL_16
#define MJCOMB_CSR_REG_YY_AC_VAL_16_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_16_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_16_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_16_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_16_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_16_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_16_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_16_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_17                             MJCOMB_CSR_REG_YY_AC_VAL_17
#define MJCOMB_CSR_REG_YY_AC_VAL_17_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_17_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_17_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_17_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_17_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_17_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_17_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_17_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_18                             MJCOMB_CSR_REG_YY_AC_VAL_18
#define MJCOMB_CSR_REG_YY_AC_VAL_18_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_18_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_18_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_18_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_18_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_18_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_18_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_18_POS))

/* 0x248 : jdec_yy_ac_cfgA */
#define MJCOMB_CSR_JDEC_YY_AC_CFGA_OFFSET                       (0x248)
#define MJCOMB_CSR_REG_YY_AC_VAL_19                             MJCOMB_CSR_REG_YY_AC_VAL_19
#define MJCOMB_CSR_REG_YY_AC_VAL_19_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_19_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_19_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_19_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_19_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_19_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_19_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_19_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_1A                             MJCOMB_CSR_REG_YY_AC_VAL_1A
#define MJCOMB_CSR_REG_YY_AC_VAL_1A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_1A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_1B                             MJCOMB_CSR_REG_YY_AC_VAL_1B
#define MJCOMB_CSR_REG_YY_AC_VAL_1B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_1B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_1C                             MJCOMB_CSR_REG_YY_AC_VAL_1C
#define MJCOMB_CSR_REG_YY_AC_VAL_1C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_1C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1C_POS))

/* 0x24C : jdec_yy_ac_cfgB */
#define MJCOMB_CSR_JDEC_YY_AC_CFGB_OFFSET                       (0x24C)
#define MJCOMB_CSR_REG_YY_AC_VAL_1D                             MJCOMB_CSR_REG_YY_AC_VAL_1D
#define MJCOMB_CSR_REG_YY_AC_VAL_1D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_1D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_1E                             MJCOMB_CSR_REG_YY_AC_VAL_1E
#define MJCOMB_CSR_REG_YY_AC_VAL_1E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_1E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_1F                             MJCOMB_CSR_REG_YY_AC_VAL_1F
#define MJCOMB_CSR_REG_YY_AC_VAL_1F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_1F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_1F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_1F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_1F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_20                             MJCOMB_CSR_REG_YY_AC_VAL_20
#define MJCOMB_CSR_REG_YY_AC_VAL_20_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_20_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_20_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_20_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_20_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_20_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_20_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_20_POS))

/* 0x250 : jdec_yy_ac_cfgC */
#define MJCOMB_CSR_JDEC_YY_AC_CFGC_OFFSET                       (0x250)
#define MJCOMB_CSR_REG_YY_AC_VAL_21                             MJCOMB_CSR_REG_YY_AC_VAL_21
#define MJCOMB_CSR_REG_YY_AC_VAL_21_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_21_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_21_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_21_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_21_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_21_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_21_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_21_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_22                             MJCOMB_CSR_REG_YY_AC_VAL_22
#define MJCOMB_CSR_REG_YY_AC_VAL_22_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_22_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_22_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_22_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_22_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_22_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_22_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_22_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_23                             MJCOMB_CSR_REG_YY_AC_VAL_23
#define MJCOMB_CSR_REG_YY_AC_VAL_23_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_23_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_23_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_23_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_23_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_23_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_23_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_23_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_24                             MJCOMB_CSR_REG_YY_AC_VAL_24
#define MJCOMB_CSR_REG_YY_AC_VAL_24_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_24_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_24_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_24_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_24_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_24_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_24_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_24_POS))

/* 0x254 : jdec_yy_ac_cfgD */
#define MJCOMB_CSR_JDEC_YY_AC_CFGD_OFFSET                       (0x254)
#define MJCOMB_CSR_REG_YY_AC_VAL_25                             MJCOMB_CSR_REG_YY_AC_VAL_25
#define MJCOMB_CSR_REG_YY_AC_VAL_25_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_25_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_25_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_25_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_25_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_25_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_25_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_25_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_26                             MJCOMB_CSR_REG_YY_AC_VAL_26
#define MJCOMB_CSR_REG_YY_AC_VAL_26_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_26_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_26_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_26_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_26_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_26_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_26_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_26_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_27                             MJCOMB_CSR_REG_YY_AC_VAL_27
#define MJCOMB_CSR_REG_YY_AC_VAL_27_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_27_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_27_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_27_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_27_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_27_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_27_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_27_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_28                             MJCOMB_CSR_REG_YY_AC_VAL_28
#define MJCOMB_CSR_REG_YY_AC_VAL_28_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_28_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_28_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_28_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_28_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_28_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_28_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_28_POS))

/* 0x258 : jdec_yy_ac_cfgE */
#define MJCOMB_CSR_JDEC_YY_AC_CFGE_OFFSET                       (0x258)
#define MJCOMB_CSR_REG_YY_AC_VAL_29                             MJCOMB_CSR_REG_YY_AC_VAL_29
#define MJCOMB_CSR_REG_YY_AC_VAL_29_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_29_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_29_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_29_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_29_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_29_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_29_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_29_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_2A                             MJCOMB_CSR_REG_YY_AC_VAL_2A
#define MJCOMB_CSR_REG_YY_AC_VAL_2A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_2A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_2B                             MJCOMB_CSR_REG_YY_AC_VAL_2B
#define MJCOMB_CSR_REG_YY_AC_VAL_2B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_2B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_2C                             MJCOMB_CSR_REG_YY_AC_VAL_2C
#define MJCOMB_CSR_REG_YY_AC_VAL_2C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_2C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2C_POS))

/* 0x25C : jdec_yy_ac_cfgF */
#define MJCOMB_CSR_JDEC_YY_AC_CFGF_OFFSET                       (0x25C)
#define MJCOMB_CSR_REG_YY_AC_VAL_2D                             MJCOMB_CSR_REG_YY_AC_VAL_2D
#define MJCOMB_CSR_REG_YY_AC_VAL_2D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_2D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_2E                             MJCOMB_CSR_REG_YY_AC_VAL_2E
#define MJCOMB_CSR_REG_YY_AC_VAL_2E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_2E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_2F                             MJCOMB_CSR_REG_YY_AC_VAL_2F
#define MJCOMB_CSR_REG_YY_AC_VAL_2F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_2F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_2F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_2F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_2F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_30                             MJCOMB_CSR_REG_YY_AC_VAL_30
#define MJCOMB_CSR_REG_YY_AC_VAL_30_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_30_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_30_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_30_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_30_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_30_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_30_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_30_POS))

/* 0x260 : jdec_yy_ac_cfg10 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG10_OFFSET                      (0x260)
#define MJCOMB_CSR_REG_YY_AC_VAL_31                             MJCOMB_CSR_REG_YY_AC_VAL_31
#define MJCOMB_CSR_REG_YY_AC_VAL_31_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_31_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_31_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_31_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_31_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_31_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_31_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_31_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_32                             MJCOMB_CSR_REG_YY_AC_VAL_32
#define MJCOMB_CSR_REG_YY_AC_VAL_32_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_32_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_32_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_32_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_32_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_32_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_32_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_32_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_33                             MJCOMB_CSR_REG_YY_AC_VAL_33
#define MJCOMB_CSR_REG_YY_AC_VAL_33_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_33_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_33_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_33_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_33_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_33_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_33_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_33_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_34                             MJCOMB_CSR_REG_YY_AC_VAL_34
#define MJCOMB_CSR_REG_YY_AC_VAL_34_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_34_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_34_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_34_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_34_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_34_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_34_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_34_POS))

/* 0x264 : jdec_yy_ac_cfg11 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG11_OFFSET                      (0x264)
#define MJCOMB_CSR_REG_YY_AC_VAL_35                             MJCOMB_CSR_REG_YY_AC_VAL_35
#define MJCOMB_CSR_REG_YY_AC_VAL_35_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_35_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_35_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_35_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_35_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_35_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_35_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_35_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_36                             MJCOMB_CSR_REG_YY_AC_VAL_36
#define MJCOMB_CSR_REG_YY_AC_VAL_36_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_36_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_36_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_36_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_36_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_36_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_36_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_36_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_37                             MJCOMB_CSR_REG_YY_AC_VAL_37
#define MJCOMB_CSR_REG_YY_AC_VAL_37_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_37_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_37_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_37_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_37_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_37_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_37_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_37_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_38                             MJCOMB_CSR_REG_YY_AC_VAL_38
#define MJCOMB_CSR_REG_YY_AC_VAL_38_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_38_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_38_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_38_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_38_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_38_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_38_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_38_POS))

/* 0x268 : jdec_yy_ac_cfg12 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG12_OFFSET                      (0x268)
#define MJCOMB_CSR_REG_YY_AC_VAL_39                             MJCOMB_CSR_REG_YY_AC_VAL_39
#define MJCOMB_CSR_REG_YY_AC_VAL_39_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_39_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_39_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_39_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_39_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_39_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_39_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_39_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_3A                             MJCOMB_CSR_REG_YY_AC_VAL_3A
#define MJCOMB_CSR_REG_YY_AC_VAL_3A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_3A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_3B                             MJCOMB_CSR_REG_YY_AC_VAL_3B
#define MJCOMB_CSR_REG_YY_AC_VAL_3B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_3B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_3C                             MJCOMB_CSR_REG_YY_AC_VAL_3C
#define MJCOMB_CSR_REG_YY_AC_VAL_3C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_3C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3C_POS))

/* 0x26C : jdec_yy_ac_cfg13 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG13_OFFSET                      (0x26C)
#define MJCOMB_CSR_REG_YY_AC_VAL_3D                             MJCOMB_CSR_REG_YY_AC_VAL_3D
#define MJCOMB_CSR_REG_YY_AC_VAL_3D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_3D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_3E                             MJCOMB_CSR_REG_YY_AC_VAL_3E
#define MJCOMB_CSR_REG_YY_AC_VAL_3E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_3E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_3F                             MJCOMB_CSR_REG_YY_AC_VAL_3F
#define MJCOMB_CSR_REG_YY_AC_VAL_3F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_3F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_3F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_3F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_3F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_40                             MJCOMB_CSR_REG_YY_AC_VAL_40
#define MJCOMB_CSR_REG_YY_AC_VAL_40_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_40_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_40_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_40_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_40_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_40_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_40_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_40_POS))

/* 0x270 : jdec_yy_ac_cfg14 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG14_OFFSET                      (0x270)
#define MJCOMB_CSR_REG_YY_AC_VAL_41                             MJCOMB_CSR_REG_YY_AC_VAL_41
#define MJCOMB_CSR_REG_YY_AC_VAL_41_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_41_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_41_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_41_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_41_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_41_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_41_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_41_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_42                             MJCOMB_CSR_REG_YY_AC_VAL_42
#define MJCOMB_CSR_REG_YY_AC_VAL_42_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_42_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_42_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_42_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_42_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_42_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_42_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_42_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_43                             MJCOMB_CSR_REG_YY_AC_VAL_43
#define MJCOMB_CSR_REG_YY_AC_VAL_43_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_43_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_43_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_43_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_43_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_43_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_43_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_43_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_44                             MJCOMB_CSR_REG_YY_AC_VAL_44
#define MJCOMB_CSR_REG_YY_AC_VAL_44_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_44_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_44_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_44_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_44_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_44_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_44_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_44_POS))

/* 0x274 : jdec_yy_ac_cfg15 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG15_OFFSET                      (0x274)
#define MJCOMB_CSR_REG_YY_AC_VAL_45                             MJCOMB_CSR_REG_YY_AC_VAL_45
#define MJCOMB_CSR_REG_YY_AC_VAL_45_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_45_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_45_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_45_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_45_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_45_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_45_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_45_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_46                             MJCOMB_CSR_REG_YY_AC_VAL_46
#define MJCOMB_CSR_REG_YY_AC_VAL_46_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_46_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_46_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_46_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_46_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_46_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_46_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_46_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_47                             MJCOMB_CSR_REG_YY_AC_VAL_47
#define MJCOMB_CSR_REG_YY_AC_VAL_47_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_47_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_47_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_47_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_47_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_47_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_47_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_47_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_48                             MJCOMB_CSR_REG_YY_AC_VAL_48
#define MJCOMB_CSR_REG_YY_AC_VAL_48_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_48_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_48_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_48_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_48_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_48_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_48_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_48_POS))

/* 0x278 : jdec_yy_ac_cfg16 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG16_OFFSET                      (0x278)
#define MJCOMB_CSR_REG_YY_AC_VAL_49                             MJCOMB_CSR_REG_YY_AC_VAL_49
#define MJCOMB_CSR_REG_YY_AC_VAL_49_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_49_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_49_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_49_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_49_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_49_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_49_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_49_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_4A                             MJCOMB_CSR_REG_YY_AC_VAL_4A
#define MJCOMB_CSR_REG_YY_AC_VAL_4A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_4A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_4B                             MJCOMB_CSR_REG_YY_AC_VAL_4B
#define MJCOMB_CSR_REG_YY_AC_VAL_4B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_4B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_4C                             MJCOMB_CSR_REG_YY_AC_VAL_4C
#define MJCOMB_CSR_REG_YY_AC_VAL_4C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_4C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4C_POS))

/* 0x27C : jdec_yy_ac_cfg17 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG17_OFFSET                      (0x27C)
#define MJCOMB_CSR_REG_YY_AC_VAL_4D                             MJCOMB_CSR_REG_YY_AC_VAL_4D
#define MJCOMB_CSR_REG_YY_AC_VAL_4D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_4D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_4E                             MJCOMB_CSR_REG_YY_AC_VAL_4E
#define MJCOMB_CSR_REG_YY_AC_VAL_4E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_4E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_4F                             MJCOMB_CSR_REG_YY_AC_VAL_4F
#define MJCOMB_CSR_REG_YY_AC_VAL_4F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_4F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_4F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_4F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_4F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_50                             MJCOMB_CSR_REG_YY_AC_VAL_50
#define MJCOMB_CSR_REG_YY_AC_VAL_50_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_50_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_50_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_50_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_50_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_50_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_50_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_50_POS))

/* 0x280 : jdec_yy_ac_cfg18 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG18_OFFSET                      (0x280)
#define MJCOMB_CSR_REG_YY_AC_VAL_51                             MJCOMB_CSR_REG_YY_AC_VAL_51
#define MJCOMB_CSR_REG_YY_AC_VAL_51_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_51_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_51_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_51_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_51_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_51_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_51_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_51_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_52                             MJCOMB_CSR_REG_YY_AC_VAL_52
#define MJCOMB_CSR_REG_YY_AC_VAL_52_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_52_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_52_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_52_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_52_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_52_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_52_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_52_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_53                             MJCOMB_CSR_REG_YY_AC_VAL_53
#define MJCOMB_CSR_REG_YY_AC_VAL_53_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_53_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_53_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_53_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_53_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_53_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_53_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_53_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_54                             MJCOMB_CSR_REG_YY_AC_VAL_54
#define MJCOMB_CSR_REG_YY_AC_VAL_54_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_54_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_54_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_54_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_54_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_54_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_54_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_54_POS))

/* 0x284 : jdec_yy_ac_cfg19 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG19_OFFSET                      (0x284)
#define MJCOMB_CSR_REG_YY_AC_VAL_55                             MJCOMB_CSR_REG_YY_AC_VAL_55
#define MJCOMB_CSR_REG_YY_AC_VAL_55_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_55_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_55_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_55_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_55_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_55_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_55_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_55_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_56                             MJCOMB_CSR_REG_YY_AC_VAL_56
#define MJCOMB_CSR_REG_YY_AC_VAL_56_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_56_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_56_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_56_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_56_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_56_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_56_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_56_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_57                             MJCOMB_CSR_REG_YY_AC_VAL_57
#define MJCOMB_CSR_REG_YY_AC_VAL_57_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_57_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_57_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_57_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_57_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_57_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_57_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_57_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_58                             MJCOMB_CSR_REG_YY_AC_VAL_58
#define MJCOMB_CSR_REG_YY_AC_VAL_58_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_58_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_58_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_58_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_58_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_58_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_58_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_58_POS))

/* 0x288 : jdec_yy_ac_cfg1A */
#define MJCOMB_CSR_JDEC_YY_AC_CFG1A_OFFSET                      (0x288)
#define MJCOMB_CSR_REG_YY_AC_VAL_59                             MJCOMB_CSR_REG_YY_AC_VAL_59
#define MJCOMB_CSR_REG_YY_AC_VAL_59_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_59_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_59_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_59_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_59_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_59_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_59_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_59_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_5A                             MJCOMB_CSR_REG_YY_AC_VAL_5A
#define MJCOMB_CSR_REG_YY_AC_VAL_5A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_5A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_5B                             MJCOMB_CSR_REG_YY_AC_VAL_5B
#define MJCOMB_CSR_REG_YY_AC_VAL_5B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_5B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_5C                             MJCOMB_CSR_REG_YY_AC_VAL_5C
#define MJCOMB_CSR_REG_YY_AC_VAL_5C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_5C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5C_POS))

/* 0x28C : jdec_yy_ac_cfg1B */
#define MJCOMB_CSR_JDEC_YY_AC_CFG1B_OFFSET                      (0x28C)
#define MJCOMB_CSR_REG_YY_AC_VAL_5D                             MJCOMB_CSR_REG_YY_AC_VAL_5D
#define MJCOMB_CSR_REG_YY_AC_VAL_5D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_5D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_5E                             MJCOMB_CSR_REG_YY_AC_VAL_5E
#define MJCOMB_CSR_REG_YY_AC_VAL_5E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_5E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_5F                             MJCOMB_CSR_REG_YY_AC_VAL_5F
#define MJCOMB_CSR_REG_YY_AC_VAL_5F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_5F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_5F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_5F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_5F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_60                             MJCOMB_CSR_REG_YY_AC_VAL_60
#define MJCOMB_CSR_REG_YY_AC_VAL_60_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_60_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_60_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_60_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_60_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_60_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_60_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_60_POS))

/* 0x290 : jdec_yy_ac_cfg1C */
#define MJCOMB_CSR_JDEC_YY_AC_CFG1C_OFFSET                      (0x290)
#define MJCOMB_CSR_REG_YY_AC_VAL_61                             MJCOMB_CSR_REG_YY_AC_VAL_61
#define MJCOMB_CSR_REG_YY_AC_VAL_61_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_61_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_61_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_61_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_61_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_61_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_61_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_61_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_62                             MJCOMB_CSR_REG_YY_AC_VAL_62
#define MJCOMB_CSR_REG_YY_AC_VAL_62_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_62_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_62_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_62_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_62_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_62_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_62_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_62_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_63                             MJCOMB_CSR_REG_YY_AC_VAL_63
#define MJCOMB_CSR_REG_YY_AC_VAL_63_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_63_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_63_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_63_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_63_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_63_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_63_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_63_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_64                             MJCOMB_CSR_REG_YY_AC_VAL_64
#define MJCOMB_CSR_REG_YY_AC_VAL_64_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_64_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_64_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_64_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_64_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_64_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_64_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_64_POS))

/* 0x294 : jdec_yy_ac_cfg1D */
#define MJCOMB_CSR_JDEC_YY_AC_CFG1D_OFFSET                      (0x294)
#define MJCOMB_CSR_REG_YY_AC_VAL_65                             MJCOMB_CSR_REG_YY_AC_VAL_65
#define MJCOMB_CSR_REG_YY_AC_VAL_65_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_65_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_65_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_65_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_65_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_65_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_65_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_65_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_66                             MJCOMB_CSR_REG_YY_AC_VAL_66
#define MJCOMB_CSR_REG_YY_AC_VAL_66_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_66_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_66_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_66_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_66_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_66_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_66_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_66_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_67                             MJCOMB_CSR_REG_YY_AC_VAL_67
#define MJCOMB_CSR_REG_YY_AC_VAL_67_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_67_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_67_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_67_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_67_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_67_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_67_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_67_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_68                             MJCOMB_CSR_REG_YY_AC_VAL_68
#define MJCOMB_CSR_REG_YY_AC_VAL_68_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_68_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_68_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_68_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_68_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_68_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_68_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_68_POS))

/* 0x298 : jdec_yy_ac_cfg1E */
#define MJCOMB_CSR_JDEC_YY_AC_CFG1E_OFFSET                      (0x298)
#define MJCOMB_CSR_REG_YY_AC_VAL_69                             MJCOMB_CSR_REG_YY_AC_VAL_69
#define MJCOMB_CSR_REG_YY_AC_VAL_69_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_69_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_69_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_69_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_69_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_69_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_69_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_69_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_6A                             MJCOMB_CSR_REG_YY_AC_VAL_6A
#define MJCOMB_CSR_REG_YY_AC_VAL_6A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_6A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_6B                             MJCOMB_CSR_REG_YY_AC_VAL_6B
#define MJCOMB_CSR_REG_YY_AC_VAL_6B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_6B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_6C                             MJCOMB_CSR_REG_YY_AC_VAL_6C
#define MJCOMB_CSR_REG_YY_AC_VAL_6C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_6C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6C_POS))

/* 0x29C : jdec_yy_ac_cfg1F */
#define MJCOMB_CSR_JDEC_YY_AC_CFG1F_OFFSET                      (0x29C)
#define MJCOMB_CSR_REG_YY_AC_VAL_6D                             MJCOMB_CSR_REG_YY_AC_VAL_6D
#define MJCOMB_CSR_REG_YY_AC_VAL_6D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_6D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_6E                             MJCOMB_CSR_REG_YY_AC_VAL_6E
#define MJCOMB_CSR_REG_YY_AC_VAL_6E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_6E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_6F                             MJCOMB_CSR_REG_YY_AC_VAL_6F
#define MJCOMB_CSR_REG_YY_AC_VAL_6F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_6F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_6F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_6F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_6F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_70                             MJCOMB_CSR_REG_YY_AC_VAL_70
#define MJCOMB_CSR_REG_YY_AC_VAL_70_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_70_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_70_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_70_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_70_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_70_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_70_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_70_POS))

/* 0x2A0 : jdec_yy_ac_cfg20 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG20_OFFSET                      (0x2A0)
#define MJCOMB_CSR_REG_YY_AC_VAL_71                             MJCOMB_CSR_REG_YY_AC_VAL_71
#define MJCOMB_CSR_REG_YY_AC_VAL_71_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_71_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_71_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_71_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_71_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_71_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_71_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_71_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_72                             MJCOMB_CSR_REG_YY_AC_VAL_72
#define MJCOMB_CSR_REG_YY_AC_VAL_72_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_72_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_72_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_72_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_72_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_72_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_72_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_72_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_73                             MJCOMB_CSR_REG_YY_AC_VAL_73
#define MJCOMB_CSR_REG_YY_AC_VAL_73_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_73_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_73_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_73_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_73_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_73_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_73_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_73_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_74                             MJCOMB_CSR_REG_YY_AC_VAL_74
#define MJCOMB_CSR_REG_YY_AC_VAL_74_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_74_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_74_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_74_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_74_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_74_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_74_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_74_POS))

/* 0x2A4 : jdec_yy_ac_cfg21 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG21_OFFSET                      (0x2A4)
#define MJCOMB_CSR_REG_YY_AC_VAL_75                             MJCOMB_CSR_REG_YY_AC_VAL_75
#define MJCOMB_CSR_REG_YY_AC_VAL_75_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_75_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_75_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_75_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_75_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_75_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_75_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_75_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_76                             MJCOMB_CSR_REG_YY_AC_VAL_76
#define MJCOMB_CSR_REG_YY_AC_VAL_76_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_76_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_76_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_76_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_76_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_76_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_76_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_76_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_77                             MJCOMB_CSR_REG_YY_AC_VAL_77
#define MJCOMB_CSR_REG_YY_AC_VAL_77_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_77_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_77_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_77_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_77_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_77_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_77_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_77_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_78                             MJCOMB_CSR_REG_YY_AC_VAL_78
#define MJCOMB_CSR_REG_YY_AC_VAL_78_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_78_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_78_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_78_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_78_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_78_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_78_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_78_POS))

/* 0x2A8 : jdec_yy_ac_cfg22 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG22_OFFSET                      (0x2A8)
#define MJCOMB_CSR_REG_YY_AC_VAL_79                             MJCOMB_CSR_REG_YY_AC_VAL_79
#define MJCOMB_CSR_REG_YY_AC_VAL_79_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_79_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_79_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_79_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_79_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_79_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_79_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_79_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_7A                             MJCOMB_CSR_REG_YY_AC_VAL_7A
#define MJCOMB_CSR_REG_YY_AC_VAL_7A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_7A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_7B                             MJCOMB_CSR_REG_YY_AC_VAL_7B
#define MJCOMB_CSR_REG_YY_AC_VAL_7B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_7B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_7C                             MJCOMB_CSR_REG_YY_AC_VAL_7C
#define MJCOMB_CSR_REG_YY_AC_VAL_7C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_7C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7C_POS))

/* 0x2AC : jdec_yy_ac_cfg23 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG23_OFFSET                      (0x2AC)
#define MJCOMB_CSR_REG_YY_AC_VAL_7D                             MJCOMB_CSR_REG_YY_AC_VAL_7D
#define MJCOMB_CSR_REG_YY_AC_VAL_7D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_7D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_7E                             MJCOMB_CSR_REG_YY_AC_VAL_7E
#define MJCOMB_CSR_REG_YY_AC_VAL_7E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_7E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_7F                             MJCOMB_CSR_REG_YY_AC_VAL_7F
#define MJCOMB_CSR_REG_YY_AC_VAL_7F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_7F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_7F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_7F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_7F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_80                             MJCOMB_CSR_REG_YY_AC_VAL_80
#define MJCOMB_CSR_REG_YY_AC_VAL_80_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_80_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_80_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_80_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_80_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_80_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_80_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_80_POS))

/* 0x2B0 : jdec_yy_ac_cfg24 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG24_OFFSET                      (0x2B0)
#define MJCOMB_CSR_REG_YY_AC_VAL_81                             MJCOMB_CSR_REG_YY_AC_VAL_81
#define MJCOMB_CSR_REG_YY_AC_VAL_81_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_81_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_81_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_81_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_81_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_81_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_81_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_81_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_82                             MJCOMB_CSR_REG_YY_AC_VAL_82
#define MJCOMB_CSR_REG_YY_AC_VAL_82_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_82_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_82_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_82_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_82_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_82_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_82_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_82_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_83                             MJCOMB_CSR_REG_YY_AC_VAL_83
#define MJCOMB_CSR_REG_YY_AC_VAL_83_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_83_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_83_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_83_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_83_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_83_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_83_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_83_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_84                             MJCOMB_CSR_REG_YY_AC_VAL_84
#define MJCOMB_CSR_REG_YY_AC_VAL_84_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_84_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_84_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_84_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_84_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_84_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_84_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_84_POS))

/* 0x2B4 : jdec_yy_ac_cfg25 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG25_OFFSET                      (0x2B4)
#define MJCOMB_CSR_REG_YY_AC_VAL_85                             MJCOMB_CSR_REG_YY_AC_VAL_85
#define MJCOMB_CSR_REG_YY_AC_VAL_85_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_85_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_85_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_85_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_85_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_85_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_85_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_85_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_86                             MJCOMB_CSR_REG_YY_AC_VAL_86
#define MJCOMB_CSR_REG_YY_AC_VAL_86_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_86_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_86_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_86_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_86_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_86_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_86_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_86_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_87                             MJCOMB_CSR_REG_YY_AC_VAL_87
#define MJCOMB_CSR_REG_YY_AC_VAL_87_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_87_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_87_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_87_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_87_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_87_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_87_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_87_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_88                             MJCOMB_CSR_REG_YY_AC_VAL_88
#define MJCOMB_CSR_REG_YY_AC_VAL_88_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_88_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_88_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_88_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_88_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_88_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_88_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_88_POS))

/* 0x2B8 : jdec_yy_ac_cfg26 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG26_OFFSET                      (0x2B8)
#define MJCOMB_CSR_REG_YY_AC_VAL_89                             MJCOMB_CSR_REG_YY_AC_VAL_89
#define MJCOMB_CSR_REG_YY_AC_VAL_89_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_89_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_89_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_89_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_89_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_89_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_89_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_89_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_8A                             MJCOMB_CSR_REG_YY_AC_VAL_8A
#define MJCOMB_CSR_REG_YY_AC_VAL_8A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_8A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_8B                             MJCOMB_CSR_REG_YY_AC_VAL_8B
#define MJCOMB_CSR_REG_YY_AC_VAL_8B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_8B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_8C                             MJCOMB_CSR_REG_YY_AC_VAL_8C
#define MJCOMB_CSR_REG_YY_AC_VAL_8C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_8C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8C_POS))

/* 0x2BC : jdec_yy_ac_cfg27 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG27_OFFSET                      (0x2BC)
#define MJCOMB_CSR_REG_YY_AC_VAL_8D                             MJCOMB_CSR_REG_YY_AC_VAL_8D
#define MJCOMB_CSR_REG_YY_AC_VAL_8D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_8D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_8E                             MJCOMB_CSR_REG_YY_AC_VAL_8E
#define MJCOMB_CSR_REG_YY_AC_VAL_8E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_8E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_8F                             MJCOMB_CSR_REG_YY_AC_VAL_8F
#define MJCOMB_CSR_REG_YY_AC_VAL_8F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_8F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_8F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_8F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_8F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_90                             MJCOMB_CSR_REG_YY_AC_VAL_90
#define MJCOMB_CSR_REG_YY_AC_VAL_90_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_90_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_90_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_90_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_90_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_90_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_90_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_90_POS))

/* 0x2C0 : jdec_yy_ac_cfg28 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG28_OFFSET                      (0x2C0)
#define MJCOMB_CSR_REG_YY_AC_VAL_91                             MJCOMB_CSR_REG_YY_AC_VAL_91
#define MJCOMB_CSR_REG_YY_AC_VAL_91_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_91_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_91_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_91_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_91_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_91_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_91_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_91_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_92                             MJCOMB_CSR_REG_YY_AC_VAL_92
#define MJCOMB_CSR_REG_YY_AC_VAL_92_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_92_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_92_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_92_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_92_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_92_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_92_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_92_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_93                             MJCOMB_CSR_REG_YY_AC_VAL_93
#define MJCOMB_CSR_REG_YY_AC_VAL_93_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_93_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_93_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_93_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_93_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_93_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_93_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_93_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_94                             MJCOMB_CSR_REG_YY_AC_VAL_94
#define MJCOMB_CSR_REG_YY_AC_VAL_94_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_94_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_94_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_94_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_94_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_94_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_94_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_94_POS))

/* 0x2C4 : jdec_yy_ac_cfg29 */
#define MJCOMB_CSR_JDEC_YY_AC_CFG29_OFFSET                      (0x2C4)
#define MJCOMB_CSR_REG_YY_AC_VAL_95                             MJCOMB_CSR_REG_YY_AC_VAL_95
#define MJCOMB_CSR_REG_YY_AC_VAL_95_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_95_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_95_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_95_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_95_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_95_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_95_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_95_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_96                             MJCOMB_CSR_REG_YY_AC_VAL_96
#define MJCOMB_CSR_REG_YY_AC_VAL_96_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_96_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_96_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_96_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_96_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_96_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_96_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_96_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_97                             MJCOMB_CSR_REG_YY_AC_VAL_97
#define MJCOMB_CSR_REG_YY_AC_VAL_97_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_97_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_97_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_97_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_97_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_97_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_97_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_97_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_98                             MJCOMB_CSR_REG_YY_AC_VAL_98
#define MJCOMB_CSR_REG_YY_AC_VAL_98_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_98_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_98_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_98_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_98_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_98_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_98_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_98_POS))

/* 0x2C8 : jdec_yy_ac_cfg2A */
#define MJCOMB_CSR_JDEC_YY_AC_CFG2A_OFFSET                      (0x2C8)
#define MJCOMB_CSR_REG_YY_AC_VAL_99                             MJCOMB_CSR_REG_YY_AC_VAL_99
#define MJCOMB_CSR_REG_YY_AC_VAL_99_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_99_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_99_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_99_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_99_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_99_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_99_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_99_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_9A                             MJCOMB_CSR_REG_YY_AC_VAL_9A
#define MJCOMB_CSR_REG_YY_AC_VAL_9A_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9A_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9A_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9A_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_9A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9A_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9A_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_9B                             MJCOMB_CSR_REG_YY_AC_VAL_9B
#define MJCOMB_CSR_REG_YY_AC_VAL_9B_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9B_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9B_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9B_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_9B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9B_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9B_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_9C                             MJCOMB_CSR_REG_YY_AC_VAL_9C
#define MJCOMB_CSR_REG_YY_AC_VAL_9C_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9C_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9C_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9C_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_9C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9C_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9C_POS))

/* 0x2CC : jdec_yy_ac_cfg2B */
#define MJCOMB_CSR_JDEC_YY_AC_CFG2B_OFFSET                      (0x2CC)
#define MJCOMB_CSR_REG_YY_AC_VAL_9D                             MJCOMB_CSR_REG_YY_AC_VAL_9D
#define MJCOMB_CSR_REG_YY_AC_VAL_9D_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9D_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9D_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9D_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_9D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9D_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9D_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_9E                             MJCOMB_CSR_REG_YY_AC_VAL_9E
#define MJCOMB_CSR_REG_YY_AC_VAL_9E_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9E_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9E_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9E_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_9E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9E_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9E_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_9F                             MJCOMB_CSR_REG_YY_AC_VAL_9F
#define MJCOMB_CSR_REG_YY_AC_VAL_9F_POS                         (16U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9F_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_9F_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9F_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_9F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_9F_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_9F_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_A0                             MJCOMB_CSR_REG_YY_AC_VAL_A0
#define MJCOMB_CSR_REG_YY_AC_VAL_A0_POS                         (24U)
#define MJCOMB_CSR_REG_YY_AC_VAL_A0_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_A0_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_A0_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_A0_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_A0_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_A0_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_A0_POS))

/* 0x2D0 : jdec_yy_ac_cfg2C */
#define MJCOMB_CSR_JDEC_YY_AC_CFG2C_OFFSET                      (0x2D0)
#define MJCOMB_CSR_REG_YY_AC_VAL_A1                             MJCOMB_CSR_REG_YY_AC_VAL_A1
#define MJCOMB_CSR_REG_YY_AC_VAL_A1_POS                         (0U)
#define MJCOMB_CSR_REG_YY_AC_VAL_A1_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_A1_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_A1_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_A1_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_A1_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_A1_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_A1_POS))
#define MJCOMB_CSR_REG_YY_AC_VAL_A2                             MJCOMB_CSR_REG_YY_AC_VAL_A2
#define MJCOMB_CSR_REG_YY_AC_VAL_A2_POS                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_A2_LEN                         (8U)
#define MJCOMB_CSR_REG_YY_AC_VAL_A2_MSK                         (((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_A2_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_A2_POS)
#define MJCOMB_CSR_REG_YY_AC_VAL_A2_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_YY_AC_VAL_A2_LEN)-1)<<MJCOMB_CSR_REG_YY_AC_VAL_A2_POS))

/* 0x300 : jdec_uv_dc_cfg0 */
#define MJCOMB_CSR_JDEC_UV_DC_CFG0_OFFSET                       (0x300)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_01                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_01
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_01_POS                     (0U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_01_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_01_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_01_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_01_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_01_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_01_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_01_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_02                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_02
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_02_POS                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_02_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_02_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_02_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_02_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_02_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_02_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_02_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_03                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_03
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_03_POS                     (8U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_03_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_03_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_03_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_03_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_03_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_03_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_03_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_04                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_04
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_04_POS                     (12U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_04_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_04_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_04_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_04_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_04_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_04_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_04_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_05                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_05
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_05_POS                     (16U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_05_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_05_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_05_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_05_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_05_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_05_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_05_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_06                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_06
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_06_POS                     (20U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_06_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_06_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_06_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_06_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_06_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_06_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_06_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_07                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_07
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_07_POS                     (24U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_07_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_07_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_07_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_07_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_07_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_07_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_07_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_08                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_08
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_08_POS                     (28U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_08_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_08_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_08_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_08_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_08_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_08_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_08_POS))

/* 0x304 : jdec_uv_dc_cfg1 */
#define MJCOMB_CSR_JDEC_UV_DC_CFG1_OFFSET                       (0x304)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_09                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_09
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_09_POS                     (0U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_09_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_09_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_09_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_09_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_09_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_09_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_09_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A_POS                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0A_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B_POS                     (8U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0B_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C_POS                     (12U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0C_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D_POS                     (16U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0D_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E_POS                     (20U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0E_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F_POS                     (24U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_0F_POS))
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_10                         MJCOMB_CSR_REG_UV_DC_LEN_CNT_10
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_10_POS                     (28U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_10_LEN                     (4U)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_10_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_10_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_10_POS)
#define MJCOMB_CSR_REG_UV_DC_LEN_CNT_10_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_10_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_LEN_CNT_10_POS))

/* 0x308 : jdec_uv_dc_cfg2 */
#define MJCOMB_CSR_JDEC_UV_DC_CFG2_OFFSET                       (0x308)
#define MJCOMB_CSR_REG_UV_DC_VAL_00                             MJCOMB_CSR_REG_UV_DC_VAL_00
#define MJCOMB_CSR_REG_UV_DC_VAL_00_POS                         (0U)
#define MJCOMB_CSR_REG_UV_DC_VAL_00_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_00_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_00_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_00_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_00_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_00_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_00_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_01                             MJCOMB_CSR_REG_UV_DC_VAL_01
#define MJCOMB_CSR_REG_UV_DC_VAL_01_POS                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_01_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_01_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_01_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_01_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_01_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_01_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_01_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_02                             MJCOMB_CSR_REG_UV_DC_VAL_02
#define MJCOMB_CSR_REG_UV_DC_VAL_02_POS                         (8U)
#define MJCOMB_CSR_REG_UV_DC_VAL_02_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_02_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_02_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_02_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_02_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_02_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_02_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_03                             MJCOMB_CSR_REG_UV_DC_VAL_03
#define MJCOMB_CSR_REG_UV_DC_VAL_03_POS                         (12U)
#define MJCOMB_CSR_REG_UV_DC_VAL_03_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_03_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_03_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_03_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_03_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_03_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_03_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_04                             MJCOMB_CSR_REG_UV_DC_VAL_04
#define MJCOMB_CSR_REG_UV_DC_VAL_04_POS                         (16U)
#define MJCOMB_CSR_REG_UV_DC_VAL_04_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_04_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_04_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_04_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_04_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_04_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_04_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_05                             MJCOMB_CSR_REG_UV_DC_VAL_05
#define MJCOMB_CSR_REG_UV_DC_VAL_05_POS                         (20U)
#define MJCOMB_CSR_REG_UV_DC_VAL_05_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_05_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_05_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_05_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_05_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_05_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_05_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_06                             MJCOMB_CSR_REG_UV_DC_VAL_06
#define MJCOMB_CSR_REG_UV_DC_VAL_06_POS                         (24U)
#define MJCOMB_CSR_REG_UV_DC_VAL_06_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_06_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_06_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_06_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_06_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_06_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_06_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_07                             MJCOMB_CSR_REG_UV_DC_VAL_07
#define MJCOMB_CSR_REG_UV_DC_VAL_07_POS                         (28U)
#define MJCOMB_CSR_REG_UV_DC_VAL_07_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_07_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_07_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_07_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_07_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_07_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_07_POS))

/* 0x30C : jdec_uv_dc_cfg3 */
#define MJCOMB_CSR_JDEC_UV_DC_CFG3_OFFSET                       (0x30C)
#define MJCOMB_CSR_REG_UV_DC_VAL_08                             MJCOMB_CSR_REG_UV_DC_VAL_08
#define MJCOMB_CSR_REG_UV_DC_VAL_08_POS                         (0U)
#define MJCOMB_CSR_REG_UV_DC_VAL_08_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_08_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_08_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_08_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_08_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_08_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_08_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_09                             MJCOMB_CSR_REG_UV_DC_VAL_09
#define MJCOMB_CSR_REG_UV_DC_VAL_09_POS                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_09_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_09_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_09_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_09_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_09_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_09_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_09_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_0A                             MJCOMB_CSR_REG_UV_DC_VAL_0A
#define MJCOMB_CSR_REG_UV_DC_VAL_0A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_DC_VAL_0A_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_0A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_0A_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_0A_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_0A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_0A_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_0A_POS))
#define MJCOMB_CSR_REG_UV_DC_VAL_0B                             MJCOMB_CSR_REG_UV_DC_VAL_0B
#define MJCOMB_CSR_REG_UV_DC_VAL_0B_POS                         (12U)
#define MJCOMB_CSR_REG_UV_DC_VAL_0B_LEN                         (4U)
#define MJCOMB_CSR_REG_UV_DC_VAL_0B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_0B_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_0B_POS)
#define MJCOMB_CSR_REG_UV_DC_VAL_0B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_DC_VAL_0B_LEN)-1)<<MJCOMB_CSR_REG_UV_DC_VAL_0B_POS))

/* 0x320 : jdec_uv_ac_cfg0 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG0_OFFSET                       (0x320)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_01                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_01
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_01_POS                     (0U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_01_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_01_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_01_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_01_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_01_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_01_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_01_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_02                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_02
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_02_POS                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_02_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_02_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_02_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_02_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_02_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_02_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_02_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_03                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_03
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_03_POS                     (16U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_03_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_03_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_03_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_03_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_03_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_03_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_03_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_04                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_04
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_04_POS                     (24U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_04_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_04_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_04_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_04_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_04_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_04_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_04_POS))

/* 0x324 : jdec_uv_ac_cfg1 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG1_OFFSET                       (0x324)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_05                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_05
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_05_POS                     (0U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_05_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_05_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_05_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_05_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_05_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_05_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_05_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_06                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_06
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_06_POS                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_06_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_06_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_06_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_06_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_06_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_06_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_06_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_07                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_07
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_07_POS                     (16U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_07_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_07_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_07_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_07_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_07_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_07_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_07_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_08                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_08
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_08_POS                     (24U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_08_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_08_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_08_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_08_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_08_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_08_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_08_POS))

/* 0x328 : jdec_uv_ac_cfg2 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG2_OFFSET                       (0x328)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_09                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_09
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_09_POS                     (0U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_09_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_09_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_09_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_09_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_09_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_09_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_09_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A_POS                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0A_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B_POS                     (16U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0B_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C_POS                     (24U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0C_POS))

/* 0x32C : jdec_uv_ac_cfg3 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG3_OFFSET                       (0x32C)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D_POS                     (0U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0D_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E_POS                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0E_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F_POS                     (16U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_0F_POS))
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_10                         MJCOMB_CSR_REG_UV_AC_LEN_CNT_10
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_10_POS                     (24U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_10_LEN                     (8U)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_10_MSK                     (((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_10_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_10_POS)
#define MJCOMB_CSR_REG_UV_AC_LEN_CNT_10_UMSK                    (~(((1LL<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_10_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_LEN_CNT_10_POS))

/* 0x330 : jdec_uv_ac_cfg4 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG4_OFFSET                       (0x330)
#define MJCOMB_CSR_REG_UV_AC_VAL_01                             MJCOMB_CSR_REG_UV_AC_VAL_01
#define MJCOMB_CSR_REG_UV_AC_VAL_01_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_01_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_01_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_01_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_01_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_01_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_01_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_01_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_02                             MJCOMB_CSR_REG_UV_AC_VAL_02
#define MJCOMB_CSR_REG_UV_AC_VAL_02_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_02_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_02_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_02_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_02_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_02_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_02_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_02_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_03                             MJCOMB_CSR_REG_UV_AC_VAL_03
#define MJCOMB_CSR_REG_UV_AC_VAL_03_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_03_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_03_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_03_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_03_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_03_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_03_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_03_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_04                             MJCOMB_CSR_REG_UV_AC_VAL_04
#define MJCOMB_CSR_REG_UV_AC_VAL_04_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_04_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_04_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_04_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_04_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_04_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_04_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_04_POS))

/* 0x334 : jdec_uv_ac_cfg5 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG5_OFFSET                       (0x334)
#define MJCOMB_CSR_REG_UV_AC_VAL_05                             MJCOMB_CSR_REG_UV_AC_VAL_05
#define MJCOMB_CSR_REG_UV_AC_VAL_05_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_05_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_05_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_05_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_05_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_05_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_05_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_05_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_06                             MJCOMB_CSR_REG_UV_AC_VAL_06
#define MJCOMB_CSR_REG_UV_AC_VAL_06_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_06_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_06_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_06_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_06_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_06_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_06_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_06_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_07                             MJCOMB_CSR_REG_UV_AC_VAL_07
#define MJCOMB_CSR_REG_UV_AC_VAL_07_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_07_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_07_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_07_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_07_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_07_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_07_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_07_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_08                             MJCOMB_CSR_REG_UV_AC_VAL_08
#define MJCOMB_CSR_REG_UV_AC_VAL_08_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_08_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_08_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_08_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_08_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_08_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_08_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_08_POS))

/* 0x338 : jdec_uv_ac_cfg6 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG6_OFFSET                       (0x338)
#define MJCOMB_CSR_REG_UV_AC_VAL_09                             MJCOMB_CSR_REG_UV_AC_VAL_09
#define MJCOMB_CSR_REG_UV_AC_VAL_09_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_09_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_09_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_09_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_09_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_09_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_09_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_09_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_0A                             MJCOMB_CSR_REG_UV_AC_VAL_0A
#define MJCOMB_CSR_REG_UV_AC_VAL_0A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_0A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_0B                             MJCOMB_CSR_REG_UV_AC_VAL_0B
#define MJCOMB_CSR_REG_UV_AC_VAL_0B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_0B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_0C                             MJCOMB_CSR_REG_UV_AC_VAL_0C
#define MJCOMB_CSR_REG_UV_AC_VAL_0C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_0C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0C_POS))

/* 0x33C : jdec_uv_ac_cfg7 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG7_OFFSET                       (0x33C)
#define MJCOMB_CSR_REG_UV_AC_VAL_0D                             MJCOMB_CSR_REG_UV_AC_VAL_0D
#define MJCOMB_CSR_REG_UV_AC_VAL_0D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_0D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_0E                             MJCOMB_CSR_REG_UV_AC_VAL_0E
#define MJCOMB_CSR_REG_UV_AC_VAL_0E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_0E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_0F                             MJCOMB_CSR_REG_UV_AC_VAL_0F
#define MJCOMB_CSR_REG_UV_AC_VAL_0F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_0F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_0F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_0F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_0F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_10                             MJCOMB_CSR_REG_UV_AC_VAL_10
#define MJCOMB_CSR_REG_UV_AC_VAL_10_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_10_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_10_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_10_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_10_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_10_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_10_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_10_POS))

/* 0x340 : jdec_uv_ac_cfg8 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG8_OFFSET                       (0x340)
#define MJCOMB_CSR_REG_UV_AC_VAL_11                             MJCOMB_CSR_REG_UV_AC_VAL_11
#define MJCOMB_CSR_REG_UV_AC_VAL_11_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_11_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_11_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_11_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_11_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_11_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_11_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_11_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_12                             MJCOMB_CSR_REG_UV_AC_VAL_12
#define MJCOMB_CSR_REG_UV_AC_VAL_12_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_12_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_12_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_12_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_12_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_12_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_12_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_12_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_13                             MJCOMB_CSR_REG_UV_AC_VAL_13
#define MJCOMB_CSR_REG_UV_AC_VAL_13_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_13_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_13_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_13_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_13_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_13_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_13_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_13_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_14                             MJCOMB_CSR_REG_UV_AC_VAL_14
#define MJCOMB_CSR_REG_UV_AC_VAL_14_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_14_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_14_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_14_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_14_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_14_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_14_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_14_POS))

/* 0x344 : jdec_uv_ac_cfg9 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG9_OFFSET                       (0x344)
#define MJCOMB_CSR_REG_UV_AC_VAL_15                             MJCOMB_CSR_REG_UV_AC_VAL_15
#define MJCOMB_CSR_REG_UV_AC_VAL_15_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_15_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_15_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_15_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_15_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_15_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_15_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_15_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_16                             MJCOMB_CSR_REG_UV_AC_VAL_16
#define MJCOMB_CSR_REG_UV_AC_VAL_16_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_16_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_16_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_16_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_16_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_16_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_16_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_16_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_17                             MJCOMB_CSR_REG_UV_AC_VAL_17
#define MJCOMB_CSR_REG_UV_AC_VAL_17_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_17_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_17_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_17_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_17_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_17_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_17_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_17_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_18                             MJCOMB_CSR_REG_UV_AC_VAL_18
#define MJCOMB_CSR_REG_UV_AC_VAL_18_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_18_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_18_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_18_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_18_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_18_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_18_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_18_POS))

/* 0x348 : jdec_uv_ac_cfgA */
#define MJCOMB_CSR_JDEC_UV_AC_CFGA_OFFSET                       (0x348)
#define MJCOMB_CSR_REG_UV_AC_VAL_19                             MJCOMB_CSR_REG_UV_AC_VAL_19
#define MJCOMB_CSR_REG_UV_AC_VAL_19_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_19_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_19_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_19_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_19_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_19_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_19_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_19_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_1A                             MJCOMB_CSR_REG_UV_AC_VAL_1A
#define MJCOMB_CSR_REG_UV_AC_VAL_1A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_1A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_1B                             MJCOMB_CSR_REG_UV_AC_VAL_1B
#define MJCOMB_CSR_REG_UV_AC_VAL_1B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_1B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_1C                             MJCOMB_CSR_REG_UV_AC_VAL_1C
#define MJCOMB_CSR_REG_UV_AC_VAL_1C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_1C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1C_POS))

/* 0x34C : jdec_uv_ac_cfgB */
#define MJCOMB_CSR_JDEC_UV_AC_CFGB_OFFSET                       (0x34C)
#define MJCOMB_CSR_REG_UV_AC_VAL_1D                             MJCOMB_CSR_REG_UV_AC_VAL_1D
#define MJCOMB_CSR_REG_UV_AC_VAL_1D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_1D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_1E                             MJCOMB_CSR_REG_UV_AC_VAL_1E
#define MJCOMB_CSR_REG_UV_AC_VAL_1E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_1E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_1F                             MJCOMB_CSR_REG_UV_AC_VAL_1F
#define MJCOMB_CSR_REG_UV_AC_VAL_1F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_1F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_1F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_1F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_1F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_20                             MJCOMB_CSR_REG_UV_AC_VAL_20
#define MJCOMB_CSR_REG_UV_AC_VAL_20_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_20_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_20_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_20_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_20_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_20_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_20_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_20_POS))

/* 0x350 : jdec_uv_ac_cfgC */
#define MJCOMB_CSR_JDEC_UV_AC_CFGC_OFFSET                       (0x350)
#define MJCOMB_CSR_REG_UV_AC_VAL_21                             MJCOMB_CSR_REG_UV_AC_VAL_21
#define MJCOMB_CSR_REG_UV_AC_VAL_21_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_21_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_21_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_21_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_21_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_21_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_21_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_21_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_22                             MJCOMB_CSR_REG_UV_AC_VAL_22
#define MJCOMB_CSR_REG_UV_AC_VAL_22_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_22_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_22_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_22_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_22_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_22_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_22_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_22_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_23                             MJCOMB_CSR_REG_UV_AC_VAL_23
#define MJCOMB_CSR_REG_UV_AC_VAL_23_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_23_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_23_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_23_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_23_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_23_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_23_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_23_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_24                             MJCOMB_CSR_REG_UV_AC_VAL_24
#define MJCOMB_CSR_REG_UV_AC_VAL_24_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_24_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_24_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_24_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_24_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_24_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_24_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_24_POS))

/* 0x354 : jdec_uv_ac_cfgD */
#define MJCOMB_CSR_JDEC_UV_AC_CFGD_OFFSET                       (0x354)
#define MJCOMB_CSR_REG_UV_AC_VAL_25                             MJCOMB_CSR_REG_UV_AC_VAL_25
#define MJCOMB_CSR_REG_UV_AC_VAL_25_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_25_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_25_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_25_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_25_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_25_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_25_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_25_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_26                             MJCOMB_CSR_REG_UV_AC_VAL_26
#define MJCOMB_CSR_REG_UV_AC_VAL_26_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_26_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_26_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_26_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_26_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_26_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_26_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_26_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_27                             MJCOMB_CSR_REG_UV_AC_VAL_27
#define MJCOMB_CSR_REG_UV_AC_VAL_27_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_27_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_27_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_27_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_27_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_27_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_27_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_27_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_28                             MJCOMB_CSR_REG_UV_AC_VAL_28
#define MJCOMB_CSR_REG_UV_AC_VAL_28_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_28_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_28_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_28_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_28_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_28_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_28_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_28_POS))

/* 0x358 : jdec_uv_ac_cfgE */
#define MJCOMB_CSR_JDEC_UV_AC_CFGE_OFFSET                       (0x358)
#define MJCOMB_CSR_REG_UV_AC_VAL_29                             MJCOMB_CSR_REG_UV_AC_VAL_29
#define MJCOMB_CSR_REG_UV_AC_VAL_29_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_29_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_29_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_29_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_29_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_29_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_29_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_29_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_2A                             MJCOMB_CSR_REG_UV_AC_VAL_2A
#define MJCOMB_CSR_REG_UV_AC_VAL_2A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_2A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_2B                             MJCOMB_CSR_REG_UV_AC_VAL_2B
#define MJCOMB_CSR_REG_UV_AC_VAL_2B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_2B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_2C                             MJCOMB_CSR_REG_UV_AC_VAL_2C
#define MJCOMB_CSR_REG_UV_AC_VAL_2C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_2C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2C_POS))

/* 0x35C : jdec_uv_ac_cfgF */
#define MJCOMB_CSR_JDEC_UV_AC_CFGF_OFFSET                       (0x35C)
#define MJCOMB_CSR_REG_UV_AC_VAL_2D                             MJCOMB_CSR_REG_UV_AC_VAL_2D
#define MJCOMB_CSR_REG_UV_AC_VAL_2D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_2D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_2E                             MJCOMB_CSR_REG_UV_AC_VAL_2E
#define MJCOMB_CSR_REG_UV_AC_VAL_2E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_2E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_2F                             MJCOMB_CSR_REG_UV_AC_VAL_2F
#define MJCOMB_CSR_REG_UV_AC_VAL_2F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_2F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_2F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_2F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_2F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_30                             MJCOMB_CSR_REG_UV_AC_VAL_30
#define MJCOMB_CSR_REG_UV_AC_VAL_30_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_30_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_30_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_30_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_30_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_30_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_30_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_30_POS))

/* 0x360 : jdec_uv_ac_cfg10 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG10_OFFSET                      (0x360)
#define MJCOMB_CSR_REG_UV_AC_VAL_31                             MJCOMB_CSR_REG_UV_AC_VAL_31
#define MJCOMB_CSR_REG_UV_AC_VAL_31_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_31_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_31_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_31_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_31_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_31_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_31_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_31_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_32                             MJCOMB_CSR_REG_UV_AC_VAL_32
#define MJCOMB_CSR_REG_UV_AC_VAL_32_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_32_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_32_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_32_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_32_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_32_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_32_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_32_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_33                             MJCOMB_CSR_REG_UV_AC_VAL_33
#define MJCOMB_CSR_REG_UV_AC_VAL_33_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_33_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_33_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_33_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_33_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_33_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_33_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_33_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_34                             MJCOMB_CSR_REG_UV_AC_VAL_34
#define MJCOMB_CSR_REG_UV_AC_VAL_34_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_34_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_34_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_34_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_34_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_34_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_34_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_34_POS))

/* 0x364 : jdec_uv_ac_cfg11 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG11_OFFSET                      (0x364)
#define MJCOMB_CSR_REG_UV_AC_VAL_35                             MJCOMB_CSR_REG_UV_AC_VAL_35
#define MJCOMB_CSR_REG_UV_AC_VAL_35_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_35_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_35_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_35_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_35_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_35_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_35_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_35_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_36                             MJCOMB_CSR_REG_UV_AC_VAL_36
#define MJCOMB_CSR_REG_UV_AC_VAL_36_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_36_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_36_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_36_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_36_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_36_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_36_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_36_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_37                             MJCOMB_CSR_REG_UV_AC_VAL_37
#define MJCOMB_CSR_REG_UV_AC_VAL_37_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_37_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_37_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_37_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_37_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_37_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_37_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_37_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_38                             MJCOMB_CSR_REG_UV_AC_VAL_38
#define MJCOMB_CSR_REG_UV_AC_VAL_38_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_38_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_38_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_38_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_38_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_38_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_38_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_38_POS))

/* 0x368 : jdec_uv_ac_cfg12 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG12_OFFSET                      (0x368)
#define MJCOMB_CSR_REG_UV_AC_VAL_39                             MJCOMB_CSR_REG_UV_AC_VAL_39
#define MJCOMB_CSR_REG_UV_AC_VAL_39_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_39_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_39_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_39_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_39_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_39_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_39_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_39_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_3A                             MJCOMB_CSR_REG_UV_AC_VAL_3A
#define MJCOMB_CSR_REG_UV_AC_VAL_3A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_3A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_3B                             MJCOMB_CSR_REG_UV_AC_VAL_3B
#define MJCOMB_CSR_REG_UV_AC_VAL_3B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_3B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_3C                             MJCOMB_CSR_REG_UV_AC_VAL_3C
#define MJCOMB_CSR_REG_UV_AC_VAL_3C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_3C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3C_POS))

/* 0x36C : jdec_uv_ac_cfg13 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG13_OFFSET                      (0x36C)
#define MJCOMB_CSR_REG_UV_AC_VAL_3D                             MJCOMB_CSR_REG_UV_AC_VAL_3D
#define MJCOMB_CSR_REG_UV_AC_VAL_3D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_3D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_3E                             MJCOMB_CSR_REG_UV_AC_VAL_3E
#define MJCOMB_CSR_REG_UV_AC_VAL_3E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_3E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_3F                             MJCOMB_CSR_REG_UV_AC_VAL_3F
#define MJCOMB_CSR_REG_UV_AC_VAL_3F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_3F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_3F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_3F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_3F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_40                             MJCOMB_CSR_REG_UV_AC_VAL_40
#define MJCOMB_CSR_REG_UV_AC_VAL_40_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_40_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_40_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_40_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_40_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_40_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_40_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_40_POS))

/* 0x370 : jdec_uv_ac_cfg14 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG14_OFFSET                      (0x370)
#define MJCOMB_CSR_REG_UV_AC_VAL_41                             MJCOMB_CSR_REG_UV_AC_VAL_41
#define MJCOMB_CSR_REG_UV_AC_VAL_41_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_41_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_41_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_41_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_41_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_41_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_41_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_41_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_42                             MJCOMB_CSR_REG_UV_AC_VAL_42
#define MJCOMB_CSR_REG_UV_AC_VAL_42_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_42_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_42_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_42_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_42_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_42_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_42_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_42_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_43                             MJCOMB_CSR_REG_UV_AC_VAL_43
#define MJCOMB_CSR_REG_UV_AC_VAL_43_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_43_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_43_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_43_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_43_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_43_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_43_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_43_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_44                             MJCOMB_CSR_REG_UV_AC_VAL_44
#define MJCOMB_CSR_REG_UV_AC_VAL_44_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_44_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_44_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_44_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_44_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_44_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_44_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_44_POS))

/* 0x374 : jdec_uv_ac_cfg15 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG15_OFFSET                      (0x374)
#define MJCOMB_CSR_REG_UV_AC_VAL_45                             MJCOMB_CSR_REG_UV_AC_VAL_45
#define MJCOMB_CSR_REG_UV_AC_VAL_45_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_45_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_45_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_45_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_45_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_45_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_45_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_45_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_46                             MJCOMB_CSR_REG_UV_AC_VAL_46
#define MJCOMB_CSR_REG_UV_AC_VAL_46_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_46_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_46_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_46_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_46_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_46_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_46_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_46_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_47                             MJCOMB_CSR_REG_UV_AC_VAL_47
#define MJCOMB_CSR_REG_UV_AC_VAL_47_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_47_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_47_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_47_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_47_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_47_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_47_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_47_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_48                             MJCOMB_CSR_REG_UV_AC_VAL_48
#define MJCOMB_CSR_REG_UV_AC_VAL_48_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_48_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_48_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_48_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_48_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_48_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_48_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_48_POS))

/* 0x378 : jdec_uv_ac_cfg16 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG16_OFFSET                      (0x378)
#define MJCOMB_CSR_REG_UV_AC_VAL_49                             MJCOMB_CSR_REG_UV_AC_VAL_49
#define MJCOMB_CSR_REG_UV_AC_VAL_49_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_49_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_49_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_49_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_49_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_49_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_49_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_49_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_4A                             MJCOMB_CSR_REG_UV_AC_VAL_4A
#define MJCOMB_CSR_REG_UV_AC_VAL_4A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_4A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_4B                             MJCOMB_CSR_REG_UV_AC_VAL_4B
#define MJCOMB_CSR_REG_UV_AC_VAL_4B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_4B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_4C                             MJCOMB_CSR_REG_UV_AC_VAL_4C
#define MJCOMB_CSR_REG_UV_AC_VAL_4C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_4C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4C_POS))

/* 0x37C : jdec_uv_ac_cfg17 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG17_OFFSET                      (0x37C)
#define MJCOMB_CSR_REG_UV_AC_VAL_4D                             MJCOMB_CSR_REG_UV_AC_VAL_4D
#define MJCOMB_CSR_REG_UV_AC_VAL_4D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_4D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_4E                             MJCOMB_CSR_REG_UV_AC_VAL_4E
#define MJCOMB_CSR_REG_UV_AC_VAL_4E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_4E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_4F                             MJCOMB_CSR_REG_UV_AC_VAL_4F
#define MJCOMB_CSR_REG_UV_AC_VAL_4F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_4F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_4F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_4F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_4F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_50                             MJCOMB_CSR_REG_UV_AC_VAL_50
#define MJCOMB_CSR_REG_UV_AC_VAL_50_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_50_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_50_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_50_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_50_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_50_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_50_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_50_POS))

/* 0x380 : jdec_uv_ac_cfg18 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG18_OFFSET                      (0x380)
#define MJCOMB_CSR_REG_UV_AC_VAL_51                             MJCOMB_CSR_REG_UV_AC_VAL_51
#define MJCOMB_CSR_REG_UV_AC_VAL_51_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_51_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_51_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_51_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_51_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_51_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_51_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_51_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_52                             MJCOMB_CSR_REG_UV_AC_VAL_52
#define MJCOMB_CSR_REG_UV_AC_VAL_52_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_52_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_52_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_52_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_52_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_52_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_52_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_52_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_53                             MJCOMB_CSR_REG_UV_AC_VAL_53
#define MJCOMB_CSR_REG_UV_AC_VAL_53_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_53_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_53_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_53_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_53_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_53_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_53_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_53_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_54                             MJCOMB_CSR_REG_UV_AC_VAL_54
#define MJCOMB_CSR_REG_UV_AC_VAL_54_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_54_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_54_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_54_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_54_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_54_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_54_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_54_POS))

/* 0x384 : jdec_uv_ac_cfg19 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG19_OFFSET                      (0x384)
#define MJCOMB_CSR_REG_UV_AC_VAL_55                             MJCOMB_CSR_REG_UV_AC_VAL_55
#define MJCOMB_CSR_REG_UV_AC_VAL_55_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_55_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_55_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_55_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_55_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_55_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_55_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_55_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_56                             MJCOMB_CSR_REG_UV_AC_VAL_56
#define MJCOMB_CSR_REG_UV_AC_VAL_56_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_56_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_56_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_56_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_56_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_56_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_56_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_56_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_57                             MJCOMB_CSR_REG_UV_AC_VAL_57
#define MJCOMB_CSR_REG_UV_AC_VAL_57_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_57_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_57_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_57_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_57_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_57_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_57_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_57_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_58                             MJCOMB_CSR_REG_UV_AC_VAL_58
#define MJCOMB_CSR_REG_UV_AC_VAL_58_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_58_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_58_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_58_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_58_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_58_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_58_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_58_POS))

/* 0x388 : jdec_uv_ac_cfg1A */
#define MJCOMB_CSR_JDEC_UV_AC_CFG1A_OFFSET                      (0x388)
#define MJCOMB_CSR_REG_UV_AC_VAL_59                             MJCOMB_CSR_REG_UV_AC_VAL_59
#define MJCOMB_CSR_REG_UV_AC_VAL_59_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_59_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_59_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_59_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_59_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_59_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_59_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_59_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_5A                             MJCOMB_CSR_REG_UV_AC_VAL_5A
#define MJCOMB_CSR_REG_UV_AC_VAL_5A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_5A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_5B                             MJCOMB_CSR_REG_UV_AC_VAL_5B
#define MJCOMB_CSR_REG_UV_AC_VAL_5B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_5B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_5C                             MJCOMB_CSR_REG_UV_AC_VAL_5C
#define MJCOMB_CSR_REG_UV_AC_VAL_5C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_5C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5C_POS))

/* 0x38C : jdec_uv_ac_cfg1B */
#define MJCOMB_CSR_JDEC_UV_AC_CFG1B_OFFSET                      (0x38C)
#define MJCOMB_CSR_REG_UV_AC_VAL_5D                             MJCOMB_CSR_REG_UV_AC_VAL_5D
#define MJCOMB_CSR_REG_UV_AC_VAL_5D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_5D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_5E                             MJCOMB_CSR_REG_UV_AC_VAL_5E
#define MJCOMB_CSR_REG_UV_AC_VAL_5E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_5E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_5F                             MJCOMB_CSR_REG_UV_AC_VAL_5F
#define MJCOMB_CSR_REG_UV_AC_VAL_5F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_5F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_5F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_5F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_5F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_60                             MJCOMB_CSR_REG_UV_AC_VAL_60
#define MJCOMB_CSR_REG_UV_AC_VAL_60_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_60_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_60_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_60_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_60_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_60_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_60_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_60_POS))

/* 0x390 : jdec_uv_ac_cfg1C */
#define MJCOMB_CSR_JDEC_UV_AC_CFG1C_OFFSET                      (0x390)
#define MJCOMB_CSR_REG_UV_AC_VAL_61                             MJCOMB_CSR_REG_UV_AC_VAL_61
#define MJCOMB_CSR_REG_UV_AC_VAL_61_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_61_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_61_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_61_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_61_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_61_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_61_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_61_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_62                             MJCOMB_CSR_REG_UV_AC_VAL_62
#define MJCOMB_CSR_REG_UV_AC_VAL_62_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_62_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_62_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_62_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_62_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_62_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_62_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_62_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_63                             MJCOMB_CSR_REG_UV_AC_VAL_63
#define MJCOMB_CSR_REG_UV_AC_VAL_63_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_63_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_63_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_63_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_63_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_63_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_63_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_63_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_64                             MJCOMB_CSR_REG_UV_AC_VAL_64
#define MJCOMB_CSR_REG_UV_AC_VAL_64_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_64_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_64_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_64_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_64_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_64_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_64_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_64_POS))

/* 0x394 : jdec_uv_ac_cfg1D */
#define MJCOMB_CSR_JDEC_UV_AC_CFG1D_OFFSET                      (0x394)
#define MJCOMB_CSR_REG_UV_AC_VAL_65                             MJCOMB_CSR_REG_UV_AC_VAL_65
#define MJCOMB_CSR_REG_UV_AC_VAL_65_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_65_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_65_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_65_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_65_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_65_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_65_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_65_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_66                             MJCOMB_CSR_REG_UV_AC_VAL_66
#define MJCOMB_CSR_REG_UV_AC_VAL_66_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_66_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_66_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_66_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_66_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_66_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_66_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_66_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_67                             MJCOMB_CSR_REG_UV_AC_VAL_67
#define MJCOMB_CSR_REG_UV_AC_VAL_67_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_67_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_67_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_67_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_67_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_67_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_67_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_67_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_68                             MJCOMB_CSR_REG_UV_AC_VAL_68
#define MJCOMB_CSR_REG_UV_AC_VAL_68_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_68_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_68_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_68_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_68_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_68_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_68_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_68_POS))

/* 0x398 : jdec_uv_ac_cfg1E */
#define MJCOMB_CSR_JDEC_UV_AC_CFG1E_OFFSET                      (0x398)
#define MJCOMB_CSR_REG_UV_AC_VAL_69                             MJCOMB_CSR_REG_UV_AC_VAL_69
#define MJCOMB_CSR_REG_UV_AC_VAL_69_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_69_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_69_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_69_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_69_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_69_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_69_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_69_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_6A                             MJCOMB_CSR_REG_UV_AC_VAL_6A
#define MJCOMB_CSR_REG_UV_AC_VAL_6A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_6A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_6B                             MJCOMB_CSR_REG_UV_AC_VAL_6B
#define MJCOMB_CSR_REG_UV_AC_VAL_6B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_6B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_6C                             MJCOMB_CSR_REG_UV_AC_VAL_6C
#define MJCOMB_CSR_REG_UV_AC_VAL_6C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_6C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6C_POS))

/* 0x39C : jdec_uv_ac_cfg1F */
#define MJCOMB_CSR_JDEC_UV_AC_CFG1F_OFFSET                      (0x39C)
#define MJCOMB_CSR_REG_UV_AC_VAL_6D                             MJCOMB_CSR_REG_UV_AC_VAL_6D
#define MJCOMB_CSR_REG_UV_AC_VAL_6D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_6D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_6E                             MJCOMB_CSR_REG_UV_AC_VAL_6E
#define MJCOMB_CSR_REG_UV_AC_VAL_6E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_6E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_6F                             MJCOMB_CSR_REG_UV_AC_VAL_6F
#define MJCOMB_CSR_REG_UV_AC_VAL_6F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_6F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_6F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_6F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_6F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_70                             MJCOMB_CSR_REG_UV_AC_VAL_70
#define MJCOMB_CSR_REG_UV_AC_VAL_70_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_70_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_70_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_70_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_70_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_70_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_70_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_70_POS))

/* 0x3A0 : jdec_uv_ac_cfg20 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG20_OFFSET                      (0x3A0)
#define MJCOMB_CSR_REG_UV_AC_VAL_71                             MJCOMB_CSR_REG_UV_AC_VAL_71
#define MJCOMB_CSR_REG_UV_AC_VAL_71_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_71_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_71_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_71_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_71_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_71_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_71_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_71_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_72                             MJCOMB_CSR_REG_UV_AC_VAL_72
#define MJCOMB_CSR_REG_UV_AC_VAL_72_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_72_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_72_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_72_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_72_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_72_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_72_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_72_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_73                             MJCOMB_CSR_REG_UV_AC_VAL_73
#define MJCOMB_CSR_REG_UV_AC_VAL_73_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_73_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_73_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_73_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_73_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_73_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_73_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_73_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_74                             MJCOMB_CSR_REG_UV_AC_VAL_74
#define MJCOMB_CSR_REG_UV_AC_VAL_74_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_74_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_74_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_74_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_74_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_74_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_74_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_74_POS))

/* 0x3A4 : jdec_uv_ac_cfg21 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG21_OFFSET                      (0x3A4)
#define MJCOMB_CSR_REG_UV_AC_VAL_75                             MJCOMB_CSR_REG_UV_AC_VAL_75
#define MJCOMB_CSR_REG_UV_AC_VAL_75_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_75_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_75_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_75_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_75_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_75_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_75_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_75_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_76                             MJCOMB_CSR_REG_UV_AC_VAL_76
#define MJCOMB_CSR_REG_UV_AC_VAL_76_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_76_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_76_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_76_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_76_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_76_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_76_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_76_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_77                             MJCOMB_CSR_REG_UV_AC_VAL_77
#define MJCOMB_CSR_REG_UV_AC_VAL_77_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_77_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_77_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_77_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_77_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_77_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_77_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_77_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_78                             MJCOMB_CSR_REG_UV_AC_VAL_78
#define MJCOMB_CSR_REG_UV_AC_VAL_78_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_78_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_78_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_78_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_78_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_78_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_78_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_78_POS))

/* 0x3A8 : jdec_uv_ac_cfg22 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG22_OFFSET                      (0x3A8)
#define MJCOMB_CSR_REG_UV_AC_VAL_79                             MJCOMB_CSR_REG_UV_AC_VAL_79
#define MJCOMB_CSR_REG_UV_AC_VAL_79_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_79_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_79_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_79_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_79_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_79_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_79_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_79_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_7A                             MJCOMB_CSR_REG_UV_AC_VAL_7A
#define MJCOMB_CSR_REG_UV_AC_VAL_7A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_7A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_7B                             MJCOMB_CSR_REG_UV_AC_VAL_7B
#define MJCOMB_CSR_REG_UV_AC_VAL_7B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_7B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_7C                             MJCOMB_CSR_REG_UV_AC_VAL_7C
#define MJCOMB_CSR_REG_UV_AC_VAL_7C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_7C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7C_POS))

/* 0x3AC : jdec_uv_ac_cfg23 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG23_OFFSET                      (0x3AC)
#define MJCOMB_CSR_REG_UV_AC_VAL_7D                             MJCOMB_CSR_REG_UV_AC_VAL_7D
#define MJCOMB_CSR_REG_UV_AC_VAL_7D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_7D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_7E                             MJCOMB_CSR_REG_UV_AC_VAL_7E
#define MJCOMB_CSR_REG_UV_AC_VAL_7E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_7E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_7F                             MJCOMB_CSR_REG_UV_AC_VAL_7F
#define MJCOMB_CSR_REG_UV_AC_VAL_7F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_7F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_7F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_7F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_7F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_80                             MJCOMB_CSR_REG_UV_AC_VAL_80
#define MJCOMB_CSR_REG_UV_AC_VAL_80_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_80_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_80_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_80_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_80_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_80_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_80_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_80_POS))

/* 0x3B0 : jdec_uv_ac_cfg24 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG24_OFFSET                      (0x3B0)
#define MJCOMB_CSR_REG_UV_AC_VAL_81                             MJCOMB_CSR_REG_UV_AC_VAL_81
#define MJCOMB_CSR_REG_UV_AC_VAL_81_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_81_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_81_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_81_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_81_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_81_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_81_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_81_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_82                             MJCOMB_CSR_REG_UV_AC_VAL_82
#define MJCOMB_CSR_REG_UV_AC_VAL_82_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_82_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_82_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_82_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_82_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_82_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_82_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_82_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_83                             MJCOMB_CSR_REG_UV_AC_VAL_83
#define MJCOMB_CSR_REG_UV_AC_VAL_83_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_83_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_83_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_83_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_83_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_83_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_83_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_83_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_84                             MJCOMB_CSR_REG_UV_AC_VAL_84
#define MJCOMB_CSR_REG_UV_AC_VAL_84_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_84_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_84_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_84_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_84_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_84_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_84_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_84_POS))

/* 0x3B4 : jdec_uv_ac_cfg25 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG25_OFFSET                      (0x3B4)
#define MJCOMB_CSR_REG_UV_AC_VAL_85                             MJCOMB_CSR_REG_UV_AC_VAL_85
#define MJCOMB_CSR_REG_UV_AC_VAL_85_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_85_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_85_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_85_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_85_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_85_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_85_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_85_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_86                             MJCOMB_CSR_REG_UV_AC_VAL_86
#define MJCOMB_CSR_REG_UV_AC_VAL_86_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_86_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_86_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_86_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_86_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_86_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_86_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_86_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_87                             MJCOMB_CSR_REG_UV_AC_VAL_87
#define MJCOMB_CSR_REG_UV_AC_VAL_87_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_87_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_87_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_87_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_87_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_87_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_87_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_87_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_88                             MJCOMB_CSR_REG_UV_AC_VAL_88
#define MJCOMB_CSR_REG_UV_AC_VAL_88_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_88_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_88_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_88_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_88_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_88_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_88_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_88_POS))

/* 0x3B8 : jdec_uv_ac_cfg26 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG26_OFFSET                      (0x3B8)
#define MJCOMB_CSR_REG_UV_AC_VAL_89                             MJCOMB_CSR_REG_UV_AC_VAL_89
#define MJCOMB_CSR_REG_UV_AC_VAL_89_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_89_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_89_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_89_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_89_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_89_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_89_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_89_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_8A                             MJCOMB_CSR_REG_UV_AC_VAL_8A
#define MJCOMB_CSR_REG_UV_AC_VAL_8A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_8A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_8B                             MJCOMB_CSR_REG_UV_AC_VAL_8B
#define MJCOMB_CSR_REG_UV_AC_VAL_8B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_8B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_8C                             MJCOMB_CSR_REG_UV_AC_VAL_8C
#define MJCOMB_CSR_REG_UV_AC_VAL_8C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_8C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8C_POS))

/* 0x3BC : jdec_uv_ac_cfg27 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG27_OFFSET                      (0x3BC)
#define MJCOMB_CSR_REG_UV_AC_VAL_8D                             MJCOMB_CSR_REG_UV_AC_VAL_8D
#define MJCOMB_CSR_REG_UV_AC_VAL_8D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_8D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_8E                             MJCOMB_CSR_REG_UV_AC_VAL_8E
#define MJCOMB_CSR_REG_UV_AC_VAL_8E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_8E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_8F                             MJCOMB_CSR_REG_UV_AC_VAL_8F
#define MJCOMB_CSR_REG_UV_AC_VAL_8F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_8F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_8F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_8F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_8F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_90                             MJCOMB_CSR_REG_UV_AC_VAL_90
#define MJCOMB_CSR_REG_UV_AC_VAL_90_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_90_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_90_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_90_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_90_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_90_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_90_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_90_POS))

/* 0x3C0 : jdec_uv_ac_cfg28 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG28_OFFSET                      (0x3C0)
#define MJCOMB_CSR_REG_UV_AC_VAL_91                             MJCOMB_CSR_REG_UV_AC_VAL_91
#define MJCOMB_CSR_REG_UV_AC_VAL_91_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_91_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_91_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_91_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_91_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_91_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_91_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_91_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_92                             MJCOMB_CSR_REG_UV_AC_VAL_92
#define MJCOMB_CSR_REG_UV_AC_VAL_92_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_92_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_92_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_92_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_92_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_92_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_92_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_92_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_93                             MJCOMB_CSR_REG_UV_AC_VAL_93
#define MJCOMB_CSR_REG_UV_AC_VAL_93_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_93_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_93_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_93_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_93_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_93_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_93_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_93_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_94                             MJCOMB_CSR_REG_UV_AC_VAL_94
#define MJCOMB_CSR_REG_UV_AC_VAL_94_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_94_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_94_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_94_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_94_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_94_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_94_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_94_POS))

/* 0x3C4 : jdec_uv_ac_cfg29 */
#define MJCOMB_CSR_JDEC_UV_AC_CFG29_OFFSET                      (0x3C4)
#define MJCOMB_CSR_REG_UV_AC_VAL_95                             MJCOMB_CSR_REG_UV_AC_VAL_95
#define MJCOMB_CSR_REG_UV_AC_VAL_95_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_95_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_95_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_95_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_95_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_95_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_95_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_95_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_96                             MJCOMB_CSR_REG_UV_AC_VAL_96
#define MJCOMB_CSR_REG_UV_AC_VAL_96_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_96_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_96_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_96_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_96_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_96_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_96_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_96_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_97                             MJCOMB_CSR_REG_UV_AC_VAL_97
#define MJCOMB_CSR_REG_UV_AC_VAL_97_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_97_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_97_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_97_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_97_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_97_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_97_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_97_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_98                             MJCOMB_CSR_REG_UV_AC_VAL_98
#define MJCOMB_CSR_REG_UV_AC_VAL_98_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_98_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_98_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_98_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_98_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_98_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_98_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_98_POS))

/* 0x3C8 : jdec_uv_ac_cfg2A */
#define MJCOMB_CSR_JDEC_UV_AC_CFG2A_OFFSET                      (0x3C8)
#define MJCOMB_CSR_REG_UV_AC_VAL_99                             MJCOMB_CSR_REG_UV_AC_VAL_99
#define MJCOMB_CSR_REG_UV_AC_VAL_99_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_99_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_99_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_99_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_99_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_99_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_99_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_99_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_9A                             MJCOMB_CSR_REG_UV_AC_VAL_9A
#define MJCOMB_CSR_REG_UV_AC_VAL_9A_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9A_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9A_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9A_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_9A_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9A_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9A_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_9B                             MJCOMB_CSR_REG_UV_AC_VAL_9B
#define MJCOMB_CSR_REG_UV_AC_VAL_9B_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9B_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9B_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9B_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_9B_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9B_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9B_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_9C                             MJCOMB_CSR_REG_UV_AC_VAL_9C
#define MJCOMB_CSR_REG_UV_AC_VAL_9C_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9C_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9C_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9C_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_9C_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9C_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9C_POS))

/* 0x3CC : jdec_uv_ac_cfg2B */
#define MJCOMB_CSR_JDEC_UV_AC_CFG2B_OFFSET                      (0x3CC)
#define MJCOMB_CSR_REG_UV_AC_VAL_9D                             MJCOMB_CSR_REG_UV_AC_VAL_9D
#define MJCOMB_CSR_REG_UV_AC_VAL_9D_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9D_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9D_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9D_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_9D_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9D_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9D_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_9E                             MJCOMB_CSR_REG_UV_AC_VAL_9E
#define MJCOMB_CSR_REG_UV_AC_VAL_9E_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9E_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9E_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9E_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_9E_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9E_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9E_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_9F                             MJCOMB_CSR_REG_UV_AC_VAL_9F
#define MJCOMB_CSR_REG_UV_AC_VAL_9F_POS                         (16U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9F_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_9F_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9F_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_9F_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_9F_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_9F_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_A0                             MJCOMB_CSR_REG_UV_AC_VAL_A0
#define MJCOMB_CSR_REG_UV_AC_VAL_A0_POS                         (24U)
#define MJCOMB_CSR_REG_UV_AC_VAL_A0_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_A0_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_A0_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_A0_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_A0_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_A0_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_A0_POS))

/* 0x3D0 : jdec_uv_ac_cfg2C */
#define MJCOMB_CSR_JDEC_UV_AC_CFG2C_OFFSET                      (0x3D0)
#define MJCOMB_CSR_REG_UV_AC_VAL_A1                             MJCOMB_CSR_REG_UV_AC_VAL_A1
#define MJCOMB_CSR_REG_UV_AC_VAL_A1_POS                         (0U)
#define MJCOMB_CSR_REG_UV_AC_VAL_A1_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_A1_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_A1_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_A1_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_A1_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_A1_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_A1_POS))
#define MJCOMB_CSR_REG_UV_AC_VAL_A2                             MJCOMB_CSR_REG_UV_AC_VAL_A2
#define MJCOMB_CSR_REG_UV_AC_VAL_A2_POS                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_A2_LEN                         (8U)
#define MJCOMB_CSR_REG_UV_AC_VAL_A2_MSK                         (((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_A2_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_A2_POS)
#define MJCOMB_CSR_REG_UV_AC_VAL_A2_UMSK                        (~(((1LL<<MJCOMB_CSR_REG_UV_AC_VAL_A2_LEN)-1)<<MJCOMB_CSR_REG_UV_AC_VAL_A2_POS))


struct  mjcomb_csr_reg {
    /* 0x0 : share_csr_0 */
    union {
        struct {
            uint32_t reg_mjcomb_mode                :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_mjcomb_enable              :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_order_u_even               :  1; /* [    2],        r/w,        0x1 */
            uint32_t reg_swap_mode                  :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_last_hf_wblk_dmy           :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_last_hf_hblk_dmy           :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_w_xlen                     :  3; /* [10: 8],        r/w,        0x3 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t reg_yuv_mode                   :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t reg_frame_cnt_trgr_int         :  5; /* [20:16],        r/w,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } share_csr_0;

    /* 0x4  reserved */
    uint8_t RESERVED0x4[4];

    /* 0x08 : share_csr_1 */
    union {
        struct {
            uint32_t reg_yy_addr_start              : 32; /* [31: 0],        r/w, 0x80000000 */
        }BF;
        uint32_t WORD;
    } share_csr_1;

    /* 0x0C : share_csr_2 */
    union {
        struct {
            uint32_t reg_uv_addr_start              : 32; /* [31: 0],        r/w, 0x80000000 */
        }BF;
        uint32_t WORD;
    } share_csr_2;

    /* 0x10 : share_csr_3 */
    union {
        struct {
            uint32_t reg_frame_wblk                 : 12; /* [11: 0],        r/w,        0xf */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t reg_frame_hblk                 : 12; /* [27:16],        r/w,       0x14 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } share_csr_3;

    /* 0x14 : share_csr_4 */
    union {
        struct {
            uint32_t reg_dec_jp_push                :  1; /* [    0],        w1p,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t reg_jp_addr                    : 30; /* [31: 2],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } share_csr_4;

    /* 0x18 : share_csr_5 */
    union {
        struct {
            uint32_t reg_head_byte                  : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reg_enc_tail_exp               :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_dec_hder_skip              :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } share_csr_5;

    /* 0x1c  reserved */
    uint8_t RESERVED0x1c[96];

    /* 0x7C : share_csr_e */
    union {
        struct {
            uint32_t dummy_reg                      : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } share_csr_e;

    /* 0x80 : jenc_csr_0 */
    union {
        struct {
            uint32_t reg_enc_bit_order              :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_enc_hw_mode_swen           :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_enc_reflect_dmy            :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_enc_read_fwrap             :  1; /* [    3],        r/w,        0x1 */
            uint32_t reg_enc_w_loop_mode            :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_19                  : 15; /* [19: 5],       rsvd,        0x0 */
            uint32_t reg_enc_id_latch_hblk          :  4; /* [23:20],        r/w,        0x1 */
            uint32_t reg_enc_hw_frame               :  6; /* [29:24],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t reg_enc_frame_done_msk         :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_0;

    /* 0x84 : jenc_csr_1 */
    union {
        struct {
            uint32_t reg_enc_sw_frame               :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t reserved_5                     :  1; /* [    5],       rsvd,        0x0 */
            uint32_t reg_enc_sw_kick                :  1; /* [    6],        w1p,        0x0 */
            uint32_t reg_enc_sw_kick_mode           :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_enc_sw_mode                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_enc_sw_run                 :  1; /* [    9],        r/w,        0x0 */
            uint32_t reg_enc_yy_dvp2axi_sel         :  3; /* [12:10],        r/w,        0x0 */
            uint32_t reg_enc_uv_dvp2axi_sel         :  3; /* [15:13],        r/w,        0x1 */
            uint32_t reg_enc_wait_cycle             : 16; /* [31:16],        r/w,      0x100 */
        }BF;
        uint32_t WORD;
    } jenc_csr_1;

    /* 0x88 : jenc_csr_2 */
    union {
        struct {
            uint32_t reg_enc_jstr_burst_cnt         : 32; /* [31: 0],        r/w,     0x4000 */
        }BF;
        uint32_t WORD;
    } jenc_csr_2;

    /* 0x8C : jenc_csr_3 */
    union {
        struct {
            uint32_t reg_enc_yy_mem_hblk            : 13; /* [12: 0],        r/w,        0x2 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t reg_enc_uv_mem_hblk            : 13; /* [28:16],        r/w,        0x2 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_3;

    /* 0x90 : jenc_csr_4 */
    union {
        struct {
            uint32_t reg_enc_int_normal_en          :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_enc_int_cam_en             :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_enc_int_mem_en             :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_enc_int_frame_en           :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_enc_int_idle_en            :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_enc_int_swap_en            :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_4;

    /* 0x94 : jenc_csr_5 */
    union {
        struct {
            uint32_t reg_enc_int_normal_clr         :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_enc_int_cam_clr            :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_enc_int_mem_clr            :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_enc_int_frame_clr          :  1; /* [    3],        w1p,        0x0 */
            uint32_t reg_enc_int_idle_clr           :  1; /* [    4],        w1p,        0x0 */
            uint32_t reg_enc_int_swap_clr           :  1; /* [    5],        w1p,        0x0 */
            uint32_t reg_enc_w_swap_clr             :  1; /* [    6],        w1p,        0x0 */
            uint32_t reg_enc_rfifo_pop              :  1; /* [    7],        w1p,        0x0 */
            uint32_t sts_enc_normal_int             :  1; /* [    8],          r,        0x0 */
            uint32_t sts_enc_cam_int                :  1; /* [    9],          r,        0x0 */
            uint32_t sts_enc_mem_int                :  1; /* [   10],          r,        0x0 */
            uint32_t sts_enc_frame_int              :  1; /* [   11],          r,        0x0 */
            uint32_t sts_enc_idle_int               :  1; /* [   12],          r,        0x0 */
            uint32_t sts_enc_swap_int               :  1; /* [   13],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t sts_enc_mjpeg_idle             :  1; /* [   16],          r,        0x1 */
            uint32_t sts_enc_mjpeg_func             :  1; /* [   17],          r,        0x0 */
            uint32_t sts_enc_mjpeg_wait             :  1; /* [   18],          r,        0x0 */
            uint32_t sts_enc_mjpeg_flsh             :  1; /* [   19],          r,        0x0 */
            uint32_t sts_enc_mjpeg_mans             :  1; /* [   20],          r,        0x0 */
            uint32_t sts_enc_mjpeg_manf             :  1; /* [   21],          r,        0x0 */
            uint32_t sts_enc_axi_read_idle          :  1; /* [   22],          r,        0x0 */
            uint32_t sts_enc_axi_write_idle         :  1; /* [   23],          r,        0x0 */
            uint32_t sts_enc_frame_valid_cnt        :  5; /* [28:24],          r,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_5;

    /* 0x98 : jenc_csr_6 */
    union {
        struct {
            uint32_t reserved_0_23                  : 24; /* [23: 0],       rsvd,        0x0 */
            uint32_t reg_enc_y0_order               :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reg_enc_u0_order               :  2; /* [27:26],        r/w,        0x1 */
            uint32_t reg_enc_y1_order               :  2; /* [29:28],        r/w,        0x2 */
            uint32_t reg_enc_v0_order               :  2; /* [31:30],        r/w,        0x3 */
        }BF;
        uint32_t WORD;
    } jenc_csr_6;

    /* 0x9C : jenc_csr_7 */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t sts_enc_swap0_full             :  1; /* [    8],          r,        0x0 */
            uint32_t sts_enc_swap1_full             :  1; /* [    9],          r,        0x0 */
            uint32_t sts_enc_read_swap_idx          :  1; /* [   10],          r,        0x0 */
            uint32_t sts_enc_swap_fstart            :  1; /* [   11],          r,        0x0 */
            uint32_t sts_enc_swap_fend              :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_7;

    /* 0xA0 : jenc_csr_8 */
    union {
        struct {
            uint32_t sts_enc_frame_swap_end_bit_cnt : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_8;

    /* 0xA4 : jenc_csr_9 */
    union {
        struct {
            uint32_t reg_enc_sw_kick_hblk           : 13; /* [12: 0],        r/w,        0x2 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_9;

    /* 0xA8 : jenc_csr_10 */
    union {
        struct {
            uint32_t sts_enc_frame_start_addr_0     : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_10;

    /* 0xAC : jenc_csr_11 */
    union {
        struct {
            uint32_t sts_enc_frame_bit_cnt_0        : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_11;

    /* 0xB0 : jenc_csr_12 */
    union {
        struct {
            uint32_t sts_enc_frame_start_addr_1     : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_12;

    /* 0xB4 : jenc_csr_13 */
    union {
        struct {
            uint32_t sts_enc_frame_bit_cnt_1        : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_13;

    /* 0xB8 : jenc_csr_14 */
    union {
        struct {
            uint32_t sts_enc_frame_start_addr_2     : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_14;

    /* 0xBC : jenc_csr_15 */
    union {
        struct {
            uint32_t sts_enc_frame_bit_cnt_2        : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_15;

    /* 0xC0 : jenc_csr_16 */
    union {
        struct {
            uint32_t sts_enc_frame_start_addr_3     : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_16;

    /* 0xC4 : jenc_csr_17 */
    union {
        struct {
            uint32_t sts_enc_frame_bit_cnt_3        : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_17;

    /* 0xC8 : jenc_csr_18 */
    union {
        struct {
            uint32_t sts_enc_frame_id_0             : 16; /* [15: 0],          r,        0x0 */
            uint32_t sts_enc_frame_id_1             : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_18;

    /* 0xCC : jenc_csr_19 */
    union {
        struct {
            uint32_t sts_enc_frame_id_2             : 16; /* [15: 0],          r,        0x0 */
            uint32_t sts_enc_frame_id_3             : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jenc_csr_19;

    /* 0xd0  reserved */
    uint8_t RESERVED0xd0[48];

    /* 0x100 : jdec_csr_0 */
    union {
        struct {
            uint32_t reserved_0_6                   :  7; /* [ 6: 0],       rsvd,        0x0 */
            uint32_t reg_dec_jstr_hw_en             :  1; /* [    7],        r/w,        0x0 */
            uint32_t reserved_8_10                  :  3; /* [10: 8],       rsvd,        0x0 */
            uint32_t reg_dec_last_hf_blk_dmy        :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_dec_r_xlen                 :  3; /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15_30                 : 16; /* [30:15],       rsvd,        0x0 */
            uint32_t reg_dec_blk_mode_en            :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_csr_0;

    /* 0x104 : jdec_csr_1 */
    union {
        struct {
            uint32_t reg_dec_int_normal_en          :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_dec_int_idle_en            :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t sts_dec_normal_int             :  1; /* [    4],          r,        0x0 */
            uint32_t sts_dec_idle_int               :  1; /* [    5],          r,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t sts_dec_mjpeg_idle             :  1; /* [    8],          r,        0x1 */
            uint32_t sts_dec_mjpeg_func             :  1; /* [    9],          r,        0x0 */
            uint32_t reserved_10                    :  1; /* [   10],       rsvd,        0x0 */
            uint32_t sts_dec_mjpeg_flsh             :  1; /* [   11],          r,        0x0 */
            uint32_t reserved_12_13                 :  2; /* [13:12],       rsvd,        0x0 */
            uint32_t sts_dec_axi_read_idle          :  1; /* [   14],          r,        0x0 */
            uint32_t sts_dec_axi_write_idle         :  1; /* [   15],          r,        0x0 */
            uint32_t reg_dec_frame_cnt_trgr_int     :  5; /* [20:16],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t sts_dec_frame_valid_cnt        :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_csr_1;

    /* 0x108 : jdec_csr_2 */
    union {
        struct {
            uint32_t reg_dec_int_clr                :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_dec_yuv_frame_pop          :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_dec_huf_cal_pulse          :  1; /* [    2],        w1p,        0x0 */
            uint32_t reserved_3_17                  : 15; /* [17: 3],       rsvd,        0x0 */
            uint32_t sts_dec_huf_cal_done           :  1; /* [   18],          r,        0x1 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_csr_2;

    /* 0x10C : jdec_csr_3 */
    union {
        struct {
            uint32_t sts_dec_jp_frame_cnt           :  3; /* [ 2: 0],          r,        0x0 */
            uint32_t reserved_3_31                  : 29; /* [31: 3],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_csr_3;

    /* 0x110 : jdec_csr_4 */
    union {
        struct {
            uint32_t sts_dec_jp_addr_0              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_csr_4;

    /* 0x114 : jdec_csr_5 */
    union {
        struct {
            uint32_t sts_dec_jp_addr_1              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_csr_5;

    /* 0x118 : jdec_csr_6 */
    union {
        struct {
            uint32_t sts_dec_jp_addr_2              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_csr_6;

    /* 0x11C : jdec_csr_7 */
    union {
        struct {
            uint32_t sts_dec_jp_addr_3              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_csr_7;

    /* 0x120  reserved */
    uint8_t RESERVED0x120[224];

    /* 0x200 : jdec_yy_dc_cfg0 */
    union {
        struct {
            uint32_t reg_yy_dc_len_cnt_01           :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_yy_dc_len_cnt_02           :  4; /* [ 7: 4],        r/w,        0x1 */
            uint32_t reg_yy_dc_len_cnt_03           :  4; /* [11: 8],        r/w,        0x5 */
            uint32_t reg_yy_dc_len_cnt_04           :  4; /* [15:12],        r/w,        0x1 */
            uint32_t reg_yy_dc_len_cnt_05           :  4; /* [19:16],        r/w,        0x1 */
            uint32_t reg_yy_dc_len_cnt_06           :  4; /* [23:20],        r/w,        0x1 */
            uint32_t reg_yy_dc_len_cnt_07           :  4; /* [27:24],        r/w,        0x1 */
            uint32_t reg_yy_dc_len_cnt_08           :  4; /* [31:28],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } jdec_yy_dc_cfg0;

    /* 0x204 : jdec_yy_dc_cfg1 */
    union {
        struct {
            uint32_t reg_yy_dc_len_cnt_09           :  4; /* [ 3: 0],        r/w,        0x1 */
            uint32_t reg_yy_dc_len_cnt_0A           :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_yy_dc_len_cnt_0B           :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_yy_dc_len_cnt_0C           :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reg_yy_dc_len_cnt_0D           :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reg_yy_dc_len_cnt_0E           :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reg_yy_dc_len_cnt_0F           :  4; /* [27:24],        r/w,        0x0 */
            uint32_t reg_yy_dc_len_cnt_10           :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_yy_dc_cfg1;

    /* 0x208 : jdec_yy_dc_cfg2 */
    union {
        struct {
            uint32_t reg_yy_dc_val_00               :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_yy_dc_val_01               :  4; /* [ 7: 4],        r/w,        0x1 */
            uint32_t reg_yy_dc_val_02               :  4; /* [11: 8],        r/w,        0x2 */
            uint32_t reg_yy_dc_val_03               :  4; /* [15:12],        r/w,        0x3 */
            uint32_t reg_yy_dc_val_04               :  4; /* [19:16],        r/w,        0x4 */
            uint32_t reg_yy_dc_val_05               :  4; /* [23:20],        r/w,        0x5 */
            uint32_t reg_yy_dc_val_06               :  4; /* [27:24],        r/w,        0x6 */
            uint32_t reg_yy_dc_val_07               :  4; /* [31:28],        r/w,        0x7 */
        }BF;
        uint32_t WORD;
    } jdec_yy_dc_cfg2;

    /* 0x20C : jdec_yy_dc_cfg3 */
    union {
        struct {
            uint32_t reg_yy_dc_val_08               :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t reg_yy_dc_val_09               :  4; /* [ 7: 4],        r/w,        0x9 */
            uint32_t reg_yy_dc_val_0A               :  4; /* [11: 8],        r/w,        0xa */
            uint32_t reg_yy_dc_val_0B               :  4; /* [15:12],        r/w,        0xb */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_yy_dc_cfg3;

    /* 0x210  reserved */
    uint8_t RESERVED0x210[16];

    /* 0x220 : jdec_yy_ac_cfg0 */
    union {
        struct {
            uint32_t reg_yy_ac_len_cnt_01           :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_yy_ac_len_cnt_02           :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t reg_yy_ac_len_cnt_03           :  8; /* [23:16],        r/w,        0x1 */
            uint32_t reg_yy_ac_len_cnt_04           :  8; /* [31:24],        r/w,        0x3 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg0;

    /* 0x224 : jdec_yy_ac_cfg1 */
    union {
        struct {
            uint32_t reg_yy_ac_len_cnt_05           :  8; /* [ 7: 0],        r/w,        0x3 */
            uint32_t reg_yy_ac_len_cnt_06           :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t reg_yy_ac_len_cnt_07           :  8; /* [23:16],        r/w,        0x4 */
            uint32_t reg_yy_ac_len_cnt_08           :  8; /* [31:24],        r/w,        0x3 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg1;

    /* 0x228 : jdec_yy_ac_cfg2 */
    union {
        struct {
            uint32_t reg_yy_ac_len_cnt_09           :  8; /* [ 7: 0],        r/w,        0x5 */
            uint32_t reg_yy_ac_len_cnt_0A           :  8; /* [15: 8],        r/w,        0x5 */
            uint32_t reg_yy_ac_len_cnt_0B           :  8; /* [23:16],        r/w,        0x4 */
            uint32_t reg_yy_ac_len_cnt_0C           :  8; /* [31:24],        r/w,        0x4 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg2;

    /* 0x22C : jdec_yy_ac_cfg3 */
    union {
        struct {
            uint32_t reg_yy_ac_len_cnt_0D           :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_yy_ac_len_cnt_0E           :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_yy_ac_len_cnt_0F           :  8; /* [23:16],        r/w,        0x1 */
            uint32_t reg_yy_ac_len_cnt_10           :  8; /* [31:24],        r/w,       0x7d */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg3;

    /* 0x230 : jdec_yy_ac_cfg4 */
    union {
        struct {
            uint32_t reg_yy_ac_val_01               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t reg_yy_ac_val_02               :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t reg_yy_ac_val_03               :  8; /* [23:16],        r/w,        0x3 */
            uint32_t reg_yy_ac_val_04               :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg4;

    /* 0x234 : jdec_yy_ac_cfg5 */
    union {
        struct {
            uint32_t reg_yy_ac_val_05               :  8; /* [ 7: 0],        r/w,        0x4 */
            uint32_t reg_yy_ac_val_06               :  8; /* [15: 8],        r/w,       0x11 */
            uint32_t reg_yy_ac_val_07               :  8; /* [23:16],        r/w,        0x5 */
            uint32_t reg_yy_ac_val_08               :  8; /* [31:24],        r/w,       0x12 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg5;

    /* 0x238 : jdec_yy_ac_cfg6 */
    union {
        struct {
            uint32_t reg_yy_ac_val_09               :  8; /* [ 7: 0],        r/w,       0x21 */
            uint32_t reg_yy_ac_val_0A               :  8; /* [15: 8],        r/w,       0x31 */
            uint32_t reg_yy_ac_val_0B               :  8; /* [23:16],        r/w,       0x41 */
            uint32_t reg_yy_ac_val_0C               :  8; /* [31:24],        r/w,        0x6 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg6;

    /* 0x23C : jdec_yy_ac_cfg7 */
    union {
        struct {
            uint32_t reg_yy_ac_val_0D               :  8; /* [ 7: 0],        r/w,       0x13 */
            uint32_t reg_yy_ac_val_0E               :  8; /* [15: 8],        r/w,       0x51 */
            uint32_t reg_yy_ac_val_0F               :  8; /* [23:16],        r/w,       0x61 */
            uint32_t reg_yy_ac_val_10               :  8; /* [31:24],        r/w,        0x7 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg7;

    /* 0x240 : jdec_yy_ac_cfg8 */
    union {
        struct {
            uint32_t reg_yy_ac_val_11               :  8; /* [ 7: 0],        r/w,       0x22 */
            uint32_t reg_yy_ac_val_12               :  8; /* [15: 8],        r/w,       0x71 */
            uint32_t reg_yy_ac_val_13               :  8; /* [23:16],        r/w,       0x14 */
            uint32_t reg_yy_ac_val_14               :  8; /* [31:24],        r/w,       0x32 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg8;

    /* 0x244 : jdec_yy_ac_cfg9 */
    union {
        struct {
            uint32_t reg_yy_ac_val_15               :  8; /* [ 7: 0],        r/w,       0x81 */
            uint32_t reg_yy_ac_val_16               :  8; /* [15: 8],        r/w,       0x91 */
            uint32_t reg_yy_ac_val_17               :  8; /* [23:16],        r/w,       0xa1 */
            uint32_t reg_yy_ac_val_18               :  8; /* [31:24],        r/w,        0x8 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg9;

    /* 0x248 : jdec_yy_ac_cfgA */
    union {
        struct {
            uint32_t reg_yy_ac_val_19               :  8; /* [ 7: 0],        r/w,       0x23 */
            uint32_t reg_yy_ac_val_1A               :  8; /* [15: 8],        r/w,       0x42 */
            uint32_t reg_yy_ac_val_1B               :  8; /* [23:16],        r/w,       0xb1 */
            uint32_t reg_yy_ac_val_1C               :  8; /* [31:24],        r/w,       0xc1 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfgA;

    /* 0x24C : jdec_yy_ac_cfgB */
    union {
        struct {
            uint32_t reg_yy_ac_val_1D               :  8; /* [ 7: 0],        r/w,       0x15 */
            uint32_t reg_yy_ac_val_1E               :  8; /* [15: 8],        r/w,       0x52 */
            uint32_t reg_yy_ac_val_1F               :  8; /* [23:16],        r/w,       0xd1 */
            uint32_t reg_yy_ac_val_20               :  8; /* [31:24],        r/w,       0xf0 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfgB;

    /* 0x250 : jdec_yy_ac_cfgC */
    union {
        struct {
            uint32_t reg_yy_ac_val_21               :  8; /* [ 7: 0],        r/w,       0x24 */
            uint32_t reg_yy_ac_val_22               :  8; /* [15: 8],        r/w,       0x33 */
            uint32_t reg_yy_ac_val_23               :  8; /* [23:16],        r/w,       0x62 */
            uint32_t reg_yy_ac_val_24               :  8; /* [31:24],        r/w,       0x72 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfgC;

    /* 0x254 : jdec_yy_ac_cfgD */
    union {
        struct {
            uint32_t reg_yy_ac_val_25               :  8; /* [ 7: 0],        r/w,       0x82 */
            uint32_t reg_yy_ac_val_26               :  8; /* [15: 8],        r/w,        0x9 */
            uint32_t reg_yy_ac_val_27               :  8; /* [23:16],        r/w,        0xa */
            uint32_t reg_yy_ac_val_28               :  8; /* [31:24],        r/w,       0x16 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfgD;

    /* 0x258 : jdec_yy_ac_cfgE */
    union {
        struct {
            uint32_t reg_yy_ac_val_29               :  8; /* [ 7: 0],        r/w,       0x17 */
            uint32_t reg_yy_ac_val_2A               :  8; /* [15: 8],        r/w,       0x18 */
            uint32_t reg_yy_ac_val_2B               :  8; /* [23:16],        r/w,       0x19 */
            uint32_t reg_yy_ac_val_2C               :  8; /* [31:24],        r/w,       0x1a */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfgE;

    /* 0x25C : jdec_yy_ac_cfgF */
    union {
        struct {
            uint32_t reg_yy_ac_val_2D               :  8; /* [ 7: 0],        r/w,       0x25 */
            uint32_t reg_yy_ac_val_2E               :  8; /* [15: 8],        r/w,       0x26 */
            uint32_t reg_yy_ac_val_2F               :  8; /* [23:16],        r/w,       0x27 */
            uint32_t reg_yy_ac_val_30               :  8; /* [31:24],        r/w,       0x28 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfgF;

    /* 0x260 : jdec_yy_ac_cfg10 */
    union {
        struct {
            uint32_t reg_yy_ac_val_31               :  8; /* [ 7: 0],        r/w,       0x29 */
            uint32_t reg_yy_ac_val_32               :  8; /* [15: 8],        r/w,       0x2a */
            uint32_t reg_yy_ac_val_33               :  8; /* [23:16],        r/w,       0x34 */
            uint32_t reg_yy_ac_val_34               :  8; /* [31:24],        r/w,       0x35 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg10;

    /* 0x264 : jdec_yy_ac_cfg11 */
    union {
        struct {
            uint32_t reg_yy_ac_val_35               :  8; /* [ 7: 0],        r/w,       0x36 */
            uint32_t reg_yy_ac_val_36               :  8; /* [15: 8],        r/w,       0x37 */
            uint32_t reg_yy_ac_val_37               :  8; /* [23:16],        r/w,       0x38 */
            uint32_t reg_yy_ac_val_38               :  8; /* [31:24],        r/w,       0x39 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg11;

    /* 0x268 : jdec_yy_ac_cfg12 */
    union {
        struct {
            uint32_t reg_yy_ac_val_39               :  8; /* [ 7: 0],        r/w,       0x3a */
            uint32_t reg_yy_ac_val_3A               :  8; /* [15: 8],        r/w,       0x43 */
            uint32_t reg_yy_ac_val_3B               :  8; /* [23:16],        r/w,       0x44 */
            uint32_t reg_yy_ac_val_3C               :  8; /* [31:24],        r/w,       0x45 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg12;

    /* 0x26C : jdec_yy_ac_cfg13 */
    union {
        struct {
            uint32_t reg_yy_ac_val_3D               :  8; /* [ 7: 0],        r/w,       0x46 */
            uint32_t reg_yy_ac_val_3E               :  8; /* [15: 8],        r/w,       0x47 */
            uint32_t reg_yy_ac_val_3F               :  8; /* [23:16],        r/w,       0x48 */
            uint32_t reg_yy_ac_val_40               :  8; /* [31:24],        r/w,       0x49 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg13;

    /* 0x270 : jdec_yy_ac_cfg14 */
    union {
        struct {
            uint32_t reg_yy_ac_val_41               :  8; /* [ 7: 0],        r/w,       0x4a */
            uint32_t reg_yy_ac_val_42               :  8; /* [15: 8],        r/w,       0x53 */
            uint32_t reg_yy_ac_val_43               :  8; /* [23:16],        r/w,       0x54 */
            uint32_t reg_yy_ac_val_44               :  8; /* [31:24],        r/w,       0x55 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg14;

    /* 0x274 : jdec_yy_ac_cfg15 */
    union {
        struct {
            uint32_t reg_yy_ac_val_45               :  8; /* [ 7: 0],        r/w,       0x56 */
            uint32_t reg_yy_ac_val_46               :  8; /* [15: 8],        r/w,       0x57 */
            uint32_t reg_yy_ac_val_47               :  8; /* [23:16],        r/w,       0x58 */
            uint32_t reg_yy_ac_val_48               :  8; /* [31:24],        r/w,       0x59 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg15;

    /* 0x278 : jdec_yy_ac_cfg16 */
    union {
        struct {
            uint32_t reg_yy_ac_val_49               :  8; /* [ 7: 0],        r/w,       0x5a */
            uint32_t reg_yy_ac_val_4A               :  8; /* [15: 8],        r/w,       0x63 */
            uint32_t reg_yy_ac_val_4B               :  8; /* [23:16],        r/w,       0x64 */
            uint32_t reg_yy_ac_val_4C               :  8; /* [31:24],        r/w,       0x65 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg16;

    /* 0x27C : jdec_yy_ac_cfg17 */
    union {
        struct {
            uint32_t reg_yy_ac_val_4D               :  8; /* [ 7: 0],        r/w,       0x66 */
            uint32_t reg_yy_ac_val_4E               :  8; /* [15: 8],        r/w,       0x67 */
            uint32_t reg_yy_ac_val_4F               :  8; /* [23:16],        r/w,       0x68 */
            uint32_t reg_yy_ac_val_50               :  8; /* [31:24],        r/w,       0x69 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg17;

    /* 0x280 : jdec_yy_ac_cfg18 */
    union {
        struct {
            uint32_t reg_yy_ac_val_51               :  8; /* [ 7: 0],        r/w,       0x6a */
            uint32_t reg_yy_ac_val_52               :  8; /* [15: 8],        r/w,       0x73 */
            uint32_t reg_yy_ac_val_53               :  8; /* [23:16],        r/w,       0x74 */
            uint32_t reg_yy_ac_val_54               :  8; /* [31:24],        r/w,       0x75 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg18;

    /* 0x284 : jdec_yy_ac_cfg19 */
    union {
        struct {
            uint32_t reg_yy_ac_val_55               :  8; /* [ 7: 0],        r/w,       0x76 */
            uint32_t reg_yy_ac_val_56               :  8; /* [15: 8],        r/w,       0x77 */
            uint32_t reg_yy_ac_val_57               :  8; /* [23:16],        r/w,       0x78 */
            uint32_t reg_yy_ac_val_58               :  8; /* [31:24],        r/w,       0x79 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg19;

    /* 0x288 : jdec_yy_ac_cfg1A */
    union {
        struct {
            uint32_t reg_yy_ac_val_59               :  8; /* [ 7: 0],        r/w,       0x7a */
            uint32_t reg_yy_ac_val_5A               :  8; /* [15: 8],        r/w,       0x83 */
            uint32_t reg_yy_ac_val_5B               :  8; /* [23:16],        r/w,       0x84 */
            uint32_t reg_yy_ac_val_5C               :  8; /* [31:24],        r/w,       0x85 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg1A;

    /* 0x28C : jdec_yy_ac_cfg1B */
    union {
        struct {
            uint32_t reg_yy_ac_val_5D               :  8; /* [ 7: 0],        r/w,       0x86 */
            uint32_t reg_yy_ac_val_5E               :  8; /* [15: 8],        r/w,       0x87 */
            uint32_t reg_yy_ac_val_5F               :  8; /* [23:16],        r/w,       0x88 */
            uint32_t reg_yy_ac_val_60               :  8; /* [31:24],        r/w,       0x89 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg1B;

    /* 0x290 : jdec_yy_ac_cfg1C */
    union {
        struct {
            uint32_t reg_yy_ac_val_61               :  8; /* [ 7: 0],        r/w,       0x8a */
            uint32_t reg_yy_ac_val_62               :  8; /* [15: 8],        r/w,       0x92 */
            uint32_t reg_yy_ac_val_63               :  8; /* [23:16],        r/w,       0x93 */
            uint32_t reg_yy_ac_val_64               :  8; /* [31:24],        r/w,       0x94 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg1C;

    /* 0x294 : jdec_yy_ac_cfg1D */
    union {
        struct {
            uint32_t reg_yy_ac_val_65               :  8; /* [ 7: 0],        r/w,       0x95 */
            uint32_t reg_yy_ac_val_66               :  8; /* [15: 8],        r/w,       0x96 */
            uint32_t reg_yy_ac_val_67               :  8; /* [23:16],        r/w,       0x97 */
            uint32_t reg_yy_ac_val_68               :  8; /* [31:24],        r/w,       0x98 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg1D;

    /* 0x298 : jdec_yy_ac_cfg1E */
    union {
        struct {
            uint32_t reg_yy_ac_val_69               :  8; /* [ 7: 0],        r/w,       0x99 */
            uint32_t reg_yy_ac_val_6A               :  8; /* [15: 8],        r/w,       0x9a */
            uint32_t reg_yy_ac_val_6B               :  8; /* [23:16],        r/w,       0xa2 */
            uint32_t reg_yy_ac_val_6C               :  8; /* [31:24],        r/w,       0xa3 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg1E;

    /* 0x29C : jdec_yy_ac_cfg1F */
    union {
        struct {
            uint32_t reg_yy_ac_val_6D               :  8; /* [ 7: 0],        r/w,       0xa4 */
            uint32_t reg_yy_ac_val_6E               :  8; /* [15: 8],        r/w,       0xa5 */
            uint32_t reg_yy_ac_val_6F               :  8; /* [23:16],        r/w,       0xa6 */
            uint32_t reg_yy_ac_val_70               :  8; /* [31:24],        r/w,       0xa7 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg1F;

    /* 0x2A0 : jdec_yy_ac_cfg20 */
    union {
        struct {
            uint32_t reg_yy_ac_val_71               :  8; /* [ 7: 0],        r/w,       0xa8 */
            uint32_t reg_yy_ac_val_72               :  8; /* [15: 8],        r/w,       0xa9 */
            uint32_t reg_yy_ac_val_73               :  8; /* [23:16],        r/w,       0xaa */
            uint32_t reg_yy_ac_val_74               :  8; /* [31:24],        r/w,       0xb2 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg20;

    /* 0x2A4 : jdec_yy_ac_cfg21 */
    union {
        struct {
            uint32_t reg_yy_ac_val_75               :  8; /* [ 7: 0],        r/w,       0xb3 */
            uint32_t reg_yy_ac_val_76               :  8; /* [15: 8],        r/w,       0xb4 */
            uint32_t reg_yy_ac_val_77               :  8; /* [23:16],        r/w,       0xb5 */
            uint32_t reg_yy_ac_val_78               :  8; /* [31:24],        r/w,       0xb6 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg21;

    /* 0x2A8 : jdec_yy_ac_cfg22 */
    union {
        struct {
            uint32_t reg_yy_ac_val_79               :  8; /* [ 7: 0],        r/w,       0xb7 */
            uint32_t reg_yy_ac_val_7A               :  8; /* [15: 8],        r/w,       0xb8 */
            uint32_t reg_yy_ac_val_7B               :  8; /* [23:16],        r/w,       0xb9 */
            uint32_t reg_yy_ac_val_7C               :  8; /* [31:24],        r/w,       0xba */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg22;

    /* 0x2AC : jdec_yy_ac_cfg23 */
    union {
        struct {
            uint32_t reg_yy_ac_val_7D               :  8; /* [ 7: 0],        r/w,       0xc2 */
            uint32_t reg_yy_ac_val_7E               :  8; /* [15: 8],        r/w,       0xc3 */
            uint32_t reg_yy_ac_val_7F               :  8; /* [23:16],        r/w,       0xc4 */
            uint32_t reg_yy_ac_val_80               :  8; /* [31:24],        r/w,       0xc5 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg23;

    /* 0x2B0 : jdec_yy_ac_cfg24 */
    union {
        struct {
            uint32_t reg_yy_ac_val_81               :  8; /* [ 7: 0],        r/w,       0xc6 */
            uint32_t reg_yy_ac_val_82               :  8; /* [15: 8],        r/w,       0xc7 */
            uint32_t reg_yy_ac_val_83               :  8; /* [23:16],        r/w,       0xc8 */
            uint32_t reg_yy_ac_val_84               :  8; /* [31:24],        r/w,       0xc9 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg24;

    /* 0x2B4 : jdec_yy_ac_cfg25 */
    union {
        struct {
            uint32_t reg_yy_ac_val_85               :  8; /* [ 7: 0],        r/w,       0xca */
            uint32_t reg_yy_ac_val_86               :  8; /* [15: 8],        r/w,       0xd2 */
            uint32_t reg_yy_ac_val_87               :  8; /* [23:16],        r/w,       0xd3 */
            uint32_t reg_yy_ac_val_88               :  8; /* [31:24],        r/w,       0xd4 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg25;

    /* 0x2B8 : jdec_yy_ac_cfg26 */
    union {
        struct {
            uint32_t reg_yy_ac_val_89               :  8; /* [ 7: 0],        r/w,       0xd5 */
            uint32_t reg_yy_ac_val_8A               :  8; /* [15: 8],        r/w,       0xd6 */
            uint32_t reg_yy_ac_val_8B               :  8; /* [23:16],        r/w,       0xd7 */
            uint32_t reg_yy_ac_val_8C               :  8; /* [31:24],        r/w,       0xd8 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg26;

    /* 0x2BC : jdec_yy_ac_cfg27 */
    union {
        struct {
            uint32_t reg_yy_ac_val_8D               :  8; /* [ 7: 0],        r/w,       0xd9 */
            uint32_t reg_yy_ac_val_8E               :  8; /* [15: 8],        r/w,       0xda */
            uint32_t reg_yy_ac_val_8F               :  8; /* [23:16],        r/w,       0xe1 */
            uint32_t reg_yy_ac_val_90               :  8; /* [31:24],        r/w,       0xe2 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg27;

    /* 0x2C0 : jdec_yy_ac_cfg28 */
    union {
        struct {
            uint32_t reg_yy_ac_val_91               :  8; /* [ 7: 0],        r/w,       0xe3 */
            uint32_t reg_yy_ac_val_92               :  8; /* [15: 8],        r/w,       0xe4 */
            uint32_t reg_yy_ac_val_93               :  8; /* [23:16],        r/w,       0xe5 */
            uint32_t reg_yy_ac_val_94               :  8; /* [31:24],        r/w,       0xe6 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg28;

    /* 0x2C4 : jdec_yy_ac_cfg29 */
    union {
        struct {
            uint32_t reg_yy_ac_val_95               :  8; /* [ 7: 0],        r/w,       0xe7 */
            uint32_t reg_yy_ac_val_96               :  8; /* [15: 8],        r/w,       0xe8 */
            uint32_t reg_yy_ac_val_97               :  8; /* [23:16],        r/w,       0xe9 */
            uint32_t reg_yy_ac_val_98               :  8; /* [31:24],        r/w,       0xea */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg29;

    /* 0x2C8 : jdec_yy_ac_cfg2A */
    union {
        struct {
            uint32_t reg_yy_ac_val_99               :  8; /* [ 7: 0],        r/w,       0xf1 */
            uint32_t reg_yy_ac_val_9A               :  8; /* [15: 8],        r/w,       0xf2 */
            uint32_t reg_yy_ac_val_9B               :  8; /* [23:16],        r/w,       0xf3 */
            uint32_t reg_yy_ac_val_9C               :  8; /* [31:24],        r/w,       0xf4 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg2A;

    /* 0x2CC : jdec_yy_ac_cfg2B */
    union {
        struct {
            uint32_t reg_yy_ac_val_9D               :  8; /* [ 7: 0],        r/w,       0xf5 */
            uint32_t reg_yy_ac_val_9E               :  8; /* [15: 8],        r/w,       0xf6 */
            uint32_t reg_yy_ac_val_9F               :  8; /* [23:16],        r/w,       0xf7 */
            uint32_t reg_yy_ac_val_A0               :  8; /* [31:24],        r/w,       0xf8 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg2B;

    /* 0x2D0 : jdec_yy_ac_cfg2C */
    union {
        struct {
            uint32_t reg_yy_ac_val_A1               :  8; /* [ 7: 0],        r/w,       0xf9 */
            uint32_t reg_yy_ac_val_A2               :  8; /* [15: 8],        r/w,       0xfa */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_yy_ac_cfg2C;

    /* 0x2d4  reserved */
    uint8_t RESERVED0x2d4[44];

    /* 0x300 : jdec_uv_dc_cfg0 */
    union {
        struct {
            uint32_t reg_uv_dc_len_cnt_01           :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_uv_dc_len_cnt_02           :  4; /* [ 7: 4],        r/w,        0x3 */
            uint32_t reg_uv_dc_len_cnt_03           :  4; /* [11: 8],        r/w,        0x1 */
            uint32_t reg_uv_dc_len_cnt_04           :  4; /* [15:12],        r/w,        0x1 */
            uint32_t reg_uv_dc_len_cnt_05           :  4; /* [19:16],        r/w,        0x1 */
            uint32_t reg_uv_dc_len_cnt_06           :  4; /* [23:20],        r/w,        0x1 */
            uint32_t reg_uv_dc_len_cnt_07           :  4; /* [27:24],        r/w,        0x1 */
            uint32_t reg_uv_dc_len_cnt_08           :  4; /* [31:28],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } jdec_uv_dc_cfg0;

    /* 0x304 : jdec_uv_dc_cfg1 */
    union {
        struct {
            uint32_t reg_uv_dc_len_cnt_09           :  4; /* [ 3: 0],        r/w,        0x1 */
            uint32_t reg_uv_dc_len_cnt_0A           :  4; /* [ 7: 4],        r/w,        0x1 */
            uint32_t reg_uv_dc_len_cnt_0B           :  4; /* [11: 8],        r/w,        0x1 */
            uint32_t reg_uv_dc_len_cnt_0C           :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reg_uv_dc_len_cnt_0D           :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reg_uv_dc_len_cnt_0E           :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reg_uv_dc_len_cnt_0F           :  4; /* [27:24],        r/w,        0x0 */
            uint32_t reg_uv_dc_len_cnt_10           :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_uv_dc_cfg1;

    /* 0x308 : jdec_uv_dc_cfg2 */
    union {
        struct {
            uint32_t reg_uv_dc_val_00               :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_uv_dc_val_01               :  4; /* [ 7: 4],        r/w,        0x1 */
            uint32_t reg_uv_dc_val_02               :  4; /* [11: 8],        r/w,        0x2 */
            uint32_t reg_uv_dc_val_03               :  4; /* [15:12],        r/w,        0x3 */
            uint32_t reg_uv_dc_val_04               :  4; /* [19:16],        r/w,        0x4 */
            uint32_t reg_uv_dc_val_05               :  4; /* [23:20],        r/w,        0x5 */
            uint32_t reg_uv_dc_val_06               :  4; /* [27:24],        r/w,        0x6 */
            uint32_t reg_uv_dc_val_07               :  4; /* [31:28],        r/w,        0x7 */
        }BF;
        uint32_t WORD;
    } jdec_uv_dc_cfg2;

    /* 0x30C : jdec_uv_dc_cfg3 */
    union {
        struct {
            uint32_t reg_uv_dc_val_08               :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t reg_uv_dc_val_09               :  4; /* [ 7: 4],        r/w,        0x9 */
            uint32_t reg_uv_dc_val_0A               :  4; /* [11: 8],        r/w,        0xa */
            uint32_t reg_uv_dc_val_0B               :  4; /* [15:12],        r/w,        0xb */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_uv_dc_cfg3;

    /* 0x310  reserved */
    uint8_t RESERVED0x310[16];

    /* 0x320 : jdec_uv_ac_cfg0 */
    union {
        struct {
            uint32_t reg_uv_ac_len_cnt_01           :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_uv_ac_len_cnt_02           :  8; /* [15: 8],        r/w,        0x2 */
            uint32_t reg_uv_ac_len_cnt_03           :  8; /* [23:16],        r/w,        0x1 */
            uint32_t reg_uv_ac_len_cnt_04           :  8; /* [31:24],        r/w,        0x2 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg0;

    /* 0x324 : jdec_uv_ac_cfg1 */
    union {
        struct {
            uint32_t reg_uv_ac_len_cnt_05           :  8; /* [ 7: 0],        r/w,        0x4 */
            uint32_t reg_uv_ac_len_cnt_06           :  8; /* [15: 8],        r/w,        0x4 */
            uint32_t reg_uv_ac_len_cnt_07           :  8; /* [23:16],        r/w,        0x3 */
            uint32_t reg_uv_ac_len_cnt_08           :  8; /* [31:24],        r/w,        0x4 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg1;

    /* 0x328 : jdec_uv_ac_cfg2 */
    union {
        struct {
            uint32_t reg_uv_ac_len_cnt_09           :  8; /* [ 7: 0],        r/w,        0x7 */
            uint32_t reg_uv_ac_len_cnt_0A           :  8; /* [15: 8],        r/w,        0x5 */
            uint32_t reg_uv_ac_len_cnt_0B           :  8; /* [23:16],        r/w,        0x4 */
            uint32_t reg_uv_ac_len_cnt_0C           :  8; /* [31:24],        r/w,        0x4 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg2;

    /* 0x32C : jdec_uv_ac_cfg3 */
    union {
        struct {
            uint32_t reg_uv_ac_len_cnt_0D           :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_uv_ac_len_cnt_0E           :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t reg_uv_ac_len_cnt_0F           :  8; /* [23:16],        r/w,        0x2 */
            uint32_t reg_uv_ac_len_cnt_10           :  8; /* [31:24],        r/w,       0x77 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg3;

    /* 0x330 : jdec_uv_ac_cfg4 */
    union {
        struct {
            uint32_t reg_uv_ac_val_01               :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_uv_ac_val_02               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t reg_uv_ac_val_03               :  8; /* [23:16],        r/w,        0x2 */
            uint32_t reg_uv_ac_val_04               :  8; /* [31:24],        r/w,        0x3 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg4;

    /* 0x334 : jdec_uv_ac_cfg5 */
    union {
        struct {
            uint32_t reg_uv_ac_val_05               :  8; /* [ 7: 0],        r/w,       0x11 */
            uint32_t reg_uv_ac_val_06               :  8; /* [15: 8],        r/w,        0x4 */
            uint32_t reg_uv_ac_val_07               :  8; /* [23:16],        r/w,        0x5 */
            uint32_t reg_uv_ac_val_08               :  8; /* [31:24],        r/w,       0x21 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg5;

    /* 0x338 : jdec_uv_ac_cfg6 */
    union {
        struct {
            uint32_t reg_uv_ac_val_09               :  8; /* [ 7: 0],        r/w,       0x31 */
            uint32_t reg_uv_ac_val_0A               :  8; /* [15: 8],        r/w,        0x6 */
            uint32_t reg_uv_ac_val_0B               :  8; /* [23:16],        r/w,       0x12 */
            uint32_t reg_uv_ac_val_0C               :  8; /* [31:24],        r/w,       0x41 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg6;

    /* 0x33C : jdec_uv_ac_cfg7 */
    union {
        struct {
            uint32_t reg_uv_ac_val_0D               :  8; /* [ 7: 0],        r/w,       0x51 */
            uint32_t reg_uv_ac_val_0E               :  8; /* [15: 8],        r/w,        0x7 */
            uint32_t reg_uv_ac_val_0F               :  8; /* [23:16],        r/w,       0x61 */
            uint32_t reg_uv_ac_val_10               :  8; /* [31:24],        r/w,       0x71 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg7;

    /* 0x340 : jdec_uv_ac_cfg8 */
    union {
        struct {
            uint32_t reg_uv_ac_val_11               :  8; /* [ 7: 0],        r/w,       0x13 */
            uint32_t reg_uv_ac_val_12               :  8; /* [15: 8],        r/w,       0x22 */
            uint32_t reg_uv_ac_val_13               :  8; /* [23:16],        r/w,       0x32 */
            uint32_t reg_uv_ac_val_14               :  8; /* [31:24],        r/w,       0x81 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg8;

    /* 0x344 : jdec_uv_ac_cfg9 */
    union {
        struct {
            uint32_t reg_uv_ac_val_15               :  8; /* [ 7: 0],        r/w,        0x8 */
            uint32_t reg_uv_ac_val_16               :  8; /* [15: 8],        r/w,       0x14 */
            uint32_t reg_uv_ac_val_17               :  8; /* [23:16],        r/w,       0x42 */
            uint32_t reg_uv_ac_val_18               :  8; /* [31:24],        r/w,       0x91 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg9;

    /* 0x348 : jdec_uv_ac_cfgA */
    union {
        struct {
            uint32_t reg_uv_ac_val_19               :  8; /* [ 7: 0],        r/w,       0xa1 */
            uint32_t reg_uv_ac_val_1A               :  8; /* [15: 8],        r/w,       0xb1 */
            uint32_t reg_uv_ac_val_1B               :  8; /* [23:16],        r/w,       0xc1 */
            uint32_t reg_uv_ac_val_1C               :  8; /* [31:24],        r/w,        0x9 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfgA;

    /* 0x34C : jdec_uv_ac_cfgB */
    union {
        struct {
            uint32_t reg_uv_ac_val_1D               :  8; /* [ 7: 0],        r/w,       0x23 */
            uint32_t reg_uv_ac_val_1E               :  8; /* [15: 8],        r/w,       0x33 */
            uint32_t reg_uv_ac_val_1F               :  8; /* [23:16],        r/w,       0x52 */
            uint32_t reg_uv_ac_val_20               :  8; /* [31:24],        r/w,       0xf0 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfgB;

    /* 0x350 : jdec_uv_ac_cfgC */
    union {
        struct {
            uint32_t reg_uv_ac_val_21               :  8; /* [ 7: 0],        r/w,       0x15 */
            uint32_t reg_uv_ac_val_22               :  8; /* [15: 8],        r/w,       0x62 */
            uint32_t reg_uv_ac_val_23               :  8; /* [23:16],        r/w,       0x72 */
            uint32_t reg_uv_ac_val_24               :  8; /* [31:24],        r/w,       0xd1 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfgC;

    /* 0x354 : jdec_uv_ac_cfgD */
    union {
        struct {
            uint32_t reg_uv_ac_val_25               :  8; /* [ 7: 0],        r/w,        0xa */
            uint32_t reg_uv_ac_val_26               :  8; /* [15: 8],        r/w,       0x16 */
            uint32_t reg_uv_ac_val_27               :  8; /* [23:16],        r/w,       0x24 */
            uint32_t reg_uv_ac_val_28               :  8; /* [31:24],        r/w,       0x34 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfgD;

    /* 0x358 : jdec_uv_ac_cfgE */
    union {
        struct {
            uint32_t reg_uv_ac_val_29               :  8; /* [ 7: 0],        r/w,       0xe1 */
            uint32_t reg_uv_ac_val_2A               :  8; /* [15: 8],        r/w,       0x25 */
            uint32_t reg_uv_ac_val_2B               :  8; /* [23:16],        r/w,       0xf1 */
            uint32_t reg_uv_ac_val_2C               :  8; /* [31:24],        r/w,       0x17 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfgE;

    /* 0x35C : jdec_uv_ac_cfgF */
    union {
        struct {
            uint32_t reg_uv_ac_val_2D               :  8; /* [ 7: 0],        r/w,       0x18 */
            uint32_t reg_uv_ac_val_2E               :  8; /* [15: 8],        r/w,       0x19 */
            uint32_t reg_uv_ac_val_2F               :  8; /* [23:16],        r/w,       0x1a */
            uint32_t reg_uv_ac_val_30               :  8; /* [31:24],        r/w,       0x26 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfgF;

    /* 0x360 : jdec_uv_ac_cfg10 */
    union {
        struct {
            uint32_t reg_uv_ac_val_31               :  8; /* [ 7: 0],        r/w,       0x27 */
            uint32_t reg_uv_ac_val_32               :  8; /* [15: 8],        r/w,       0x28 */
            uint32_t reg_uv_ac_val_33               :  8; /* [23:16],        r/w,       0x29 */
            uint32_t reg_uv_ac_val_34               :  8; /* [31:24],        r/w,       0x2a */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg10;

    /* 0x364 : jdec_uv_ac_cfg11 */
    union {
        struct {
            uint32_t reg_uv_ac_val_35               :  8; /* [ 7: 0],        r/w,       0x35 */
            uint32_t reg_uv_ac_val_36               :  8; /* [15: 8],        r/w,       0x36 */
            uint32_t reg_uv_ac_val_37               :  8; /* [23:16],        r/w,       0x37 */
            uint32_t reg_uv_ac_val_38               :  8; /* [31:24],        r/w,       0x38 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg11;

    /* 0x368 : jdec_uv_ac_cfg12 */
    union {
        struct {
            uint32_t reg_uv_ac_val_39               :  8; /* [ 7: 0],        r/w,       0x39 */
            uint32_t reg_uv_ac_val_3A               :  8; /* [15: 8],        r/w,       0x3a */
            uint32_t reg_uv_ac_val_3B               :  8; /* [23:16],        r/w,       0x43 */
            uint32_t reg_uv_ac_val_3C               :  8; /* [31:24],        r/w,       0x44 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg12;

    /* 0x36C : jdec_uv_ac_cfg13 */
    union {
        struct {
            uint32_t reg_uv_ac_val_3D               :  8; /* [ 7: 0],        r/w,       0x45 */
            uint32_t reg_uv_ac_val_3E               :  8; /* [15: 8],        r/w,       0x46 */
            uint32_t reg_uv_ac_val_3F               :  8; /* [23:16],        r/w,       0x47 */
            uint32_t reg_uv_ac_val_40               :  8; /* [31:24],        r/w,       0x48 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg13;

    /* 0x370 : jdec_uv_ac_cfg14 */
    union {
        struct {
            uint32_t reg_uv_ac_val_41               :  8; /* [ 7: 0],        r/w,       0x49 */
            uint32_t reg_uv_ac_val_42               :  8; /* [15: 8],        r/w,       0x4a */
            uint32_t reg_uv_ac_val_43               :  8; /* [23:16],        r/w,       0x53 */
            uint32_t reg_uv_ac_val_44               :  8; /* [31:24],        r/w,       0x54 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg14;

    /* 0x374 : jdec_uv_ac_cfg15 */
    union {
        struct {
            uint32_t reg_uv_ac_val_45               :  8; /* [ 7: 0],        r/w,       0x55 */
            uint32_t reg_uv_ac_val_46               :  8; /* [15: 8],        r/w,       0x56 */
            uint32_t reg_uv_ac_val_47               :  8; /* [23:16],        r/w,       0x57 */
            uint32_t reg_uv_ac_val_48               :  8; /* [31:24],        r/w,       0x58 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg15;

    /* 0x378 : jdec_uv_ac_cfg16 */
    union {
        struct {
            uint32_t reg_uv_ac_val_49               :  8; /* [ 7: 0],        r/w,       0x59 */
            uint32_t reg_uv_ac_val_4A               :  8; /* [15: 8],        r/w,       0x5a */
            uint32_t reg_uv_ac_val_4B               :  8; /* [23:16],        r/w,       0x63 */
            uint32_t reg_uv_ac_val_4C               :  8; /* [31:24],        r/w,       0x64 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg16;

    /* 0x37C : jdec_uv_ac_cfg17 */
    union {
        struct {
            uint32_t reg_uv_ac_val_4D               :  8; /* [ 7: 0],        r/w,       0x65 */
            uint32_t reg_uv_ac_val_4E               :  8; /* [15: 8],        r/w,       0x66 */
            uint32_t reg_uv_ac_val_4F               :  8; /* [23:16],        r/w,       0x67 */
            uint32_t reg_uv_ac_val_50               :  8; /* [31:24],        r/w,       0x68 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg17;

    /* 0x380 : jdec_uv_ac_cfg18 */
    union {
        struct {
            uint32_t reg_uv_ac_val_51               :  8; /* [ 7: 0],        r/w,       0x69 */
            uint32_t reg_uv_ac_val_52               :  8; /* [15: 8],        r/w,       0x6a */
            uint32_t reg_uv_ac_val_53               :  8; /* [23:16],        r/w,       0x73 */
            uint32_t reg_uv_ac_val_54               :  8; /* [31:24],        r/w,       0x74 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg18;

    /* 0x384 : jdec_uv_ac_cfg19 */
    union {
        struct {
            uint32_t reg_uv_ac_val_55               :  8; /* [ 7: 0],        r/w,       0x75 */
            uint32_t reg_uv_ac_val_56               :  8; /* [15: 8],        r/w,       0x76 */
            uint32_t reg_uv_ac_val_57               :  8; /* [23:16],        r/w,       0x77 */
            uint32_t reg_uv_ac_val_58               :  8; /* [31:24],        r/w,       0x78 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg19;

    /* 0x388 : jdec_uv_ac_cfg1A */
    union {
        struct {
            uint32_t reg_uv_ac_val_59               :  8; /* [ 7: 0],        r/w,       0x79 */
            uint32_t reg_uv_ac_val_5A               :  8; /* [15: 8],        r/w,       0x7a */
            uint32_t reg_uv_ac_val_5B               :  8; /* [23:16],        r/w,       0x82 */
            uint32_t reg_uv_ac_val_5C               :  8; /* [31:24],        r/w,       0x83 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg1A;

    /* 0x38C : jdec_uv_ac_cfg1B */
    union {
        struct {
            uint32_t reg_uv_ac_val_5D               :  8; /* [ 7: 0],        r/w,       0x84 */
            uint32_t reg_uv_ac_val_5E               :  8; /* [15: 8],        r/w,       0x85 */
            uint32_t reg_uv_ac_val_5F               :  8; /* [23:16],        r/w,       0x86 */
            uint32_t reg_uv_ac_val_60               :  8; /* [31:24],        r/w,       0x87 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg1B;

    /* 0x390 : jdec_uv_ac_cfg1C */
    union {
        struct {
            uint32_t reg_uv_ac_val_61               :  8; /* [ 7: 0],        r/w,       0x88 */
            uint32_t reg_uv_ac_val_62               :  8; /* [15: 8],        r/w,       0x89 */
            uint32_t reg_uv_ac_val_63               :  8; /* [23:16],        r/w,       0x8a */
            uint32_t reg_uv_ac_val_64               :  8; /* [31:24],        r/w,       0x92 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg1C;

    /* 0x394 : jdec_uv_ac_cfg1D */
    union {
        struct {
            uint32_t reg_uv_ac_val_65               :  8; /* [ 7: 0],        r/w,       0x93 */
            uint32_t reg_uv_ac_val_66               :  8; /* [15: 8],        r/w,       0x94 */
            uint32_t reg_uv_ac_val_67               :  8; /* [23:16],        r/w,       0x95 */
            uint32_t reg_uv_ac_val_68               :  8; /* [31:24],        r/w,       0x96 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg1D;

    /* 0x398 : jdec_uv_ac_cfg1E */
    union {
        struct {
            uint32_t reg_uv_ac_val_69               :  8; /* [ 7: 0],        r/w,       0x97 */
            uint32_t reg_uv_ac_val_6A               :  8; /* [15: 8],        r/w,       0x98 */
            uint32_t reg_uv_ac_val_6B               :  8; /* [23:16],        r/w,       0x99 */
            uint32_t reg_uv_ac_val_6C               :  8; /* [31:24],        r/w,       0x9a */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg1E;

    /* 0x39C : jdec_uv_ac_cfg1F */
    union {
        struct {
            uint32_t reg_uv_ac_val_6D               :  8; /* [ 7: 0],        r/w,       0xa2 */
            uint32_t reg_uv_ac_val_6E               :  8; /* [15: 8],        r/w,       0xa3 */
            uint32_t reg_uv_ac_val_6F               :  8; /* [23:16],        r/w,       0xa4 */
            uint32_t reg_uv_ac_val_70               :  8; /* [31:24],        r/w,       0xa5 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg1F;

    /* 0x3A0 : jdec_uv_ac_cfg20 */
    union {
        struct {
            uint32_t reg_uv_ac_val_71               :  8; /* [ 7: 0],        r/w,       0xa6 */
            uint32_t reg_uv_ac_val_72               :  8; /* [15: 8],        r/w,       0xa7 */
            uint32_t reg_uv_ac_val_73               :  8; /* [23:16],        r/w,       0xa8 */
            uint32_t reg_uv_ac_val_74               :  8; /* [31:24],        r/w,       0xa9 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg20;

    /* 0x3A4 : jdec_uv_ac_cfg21 */
    union {
        struct {
            uint32_t reg_uv_ac_val_75               :  8; /* [ 7: 0],        r/w,       0xaa */
            uint32_t reg_uv_ac_val_76               :  8; /* [15: 8],        r/w,       0xb2 */
            uint32_t reg_uv_ac_val_77               :  8; /* [23:16],        r/w,       0xb3 */
            uint32_t reg_uv_ac_val_78               :  8; /* [31:24],        r/w,       0xb4 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg21;

    /* 0x3A8 : jdec_uv_ac_cfg22 */
    union {
        struct {
            uint32_t reg_uv_ac_val_79               :  8; /* [ 7: 0],        r/w,       0xb5 */
            uint32_t reg_uv_ac_val_7A               :  8; /* [15: 8],        r/w,       0xb6 */
            uint32_t reg_uv_ac_val_7B               :  8; /* [23:16],        r/w,       0xb7 */
            uint32_t reg_uv_ac_val_7C               :  8; /* [31:24],        r/w,       0xb8 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg22;

    /* 0x3AC : jdec_uv_ac_cfg23 */
    union {
        struct {
            uint32_t reg_uv_ac_val_7D               :  8; /* [ 7: 0],        r/w,       0xb9 */
            uint32_t reg_uv_ac_val_7E               :  8; /* [15: 8],        r/w,       0xba */
            uint32_t reg_uv_ac_val_7F               :  8; /* [23:16],        r/w,       0xc2 */
            uint32_t reg_uv_ac_val_80               :  8; /* [31:24],        r/w,       0xc3 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg23;

    /* 0x3B0 : jdec_uv_ac_cfg24 */
    union {
        struct {
            uint32_t reg_uv_ac_val_81               :  8; /* [ 7: 0],        r/w,       0xc4 */
            uint32_t reg_uv_ac_val_82               :  8; /* [15: 8],        r/w,       0xc5 */
            uint32_t reg_uv_ac_val_83               :  8; /* [23:16],        r/w,       0xc6 */
            uint32_t reg_uv_ac_val_84               :  8; /* [31:24],        r/w,       0xc7 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg24;

    /* 0x3B4 : jdec_uv_ac_cfg25 */
    union {
        struct {
            uint32_t reg_uv_ac_val_85               :  8; /* [ 7: 0],        r/w,       0xc8 */
            uint32_t reg_uv_ac_val_86               :  8; /* [15: 8],        r/w,       0xc9 */
            uint32_t reg_uv_ac_val_87               :  8; /* [23:16],        r/w,       0xca */
            uint32_t reg_uv_ac_val_88               :  8; /* [31:24],        r/w,       0xd2 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg25;

    /* 0x3B8 : jdec_uv_ac_cfg26 */
    union {
        struct {
            uint32_t reg_uv_ac_val_89               :  8; /* [ 7: 0],        r/w,       0xd3 */
            uint32_t reg_uv_ac_val_8A               :  8; /* [15: 8],        r/w,       0xd4 */
            uint32_t reg_uv_ac_val_8B               :  8; /* [23:16],        r/w,       0xd5 */
            uint32_t reg_uv_ac_val_8C               :  8; /* [31:24],        r/w,       0xd6 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg26;

    /* 0x3BC : jdec_uv_ac_cfg27 */
    union {
        struct {
            uint32_t reg_uv_ac_val_8D               :  8; /* [ 7: 0],        r/w,       0xd7 */
            uint32_t reg_uv_ac_val_8E               :  8; /* [15: 8],        r/w,       0xd8 */
            uint32_t reg_uv_ac_val_8F               :  8; /* [23:16],        r/w,       0xd9 */
            uint32_t reg_uv_ac_val_90               :  8; /* [31:24],        r/w,       0xda */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg27;

    /* 0x3C0 : jdec_uv_ac_cfg28 */
    union {
        struct {
            uint32_t reg_uv_ac_val_91               :  8; /* [ 7: 0],        r/w,       0xe2 */
            uint32_t reg_uv_ac_val_92               :  8; /* [15: 8],        r/w,       0xe3 */
            uint32_t reg_uv_ac_val_93               :  8; /* [23:16],        r/w,       0xe4 */
            uint32_t reg_uv_ac_val_94               :  8; /* [31:24],        r/w,       0xe5 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg28;

    /* 0x3C4 : jdec_uv_ac_cfg29 */
    union {
        struct {
            uint32_t reg_uv_ac_val_95               :  8; /* [ 7: 0],        r/w,       0xe6 */
            uint32_t reg_uv_ac_val_96               :  8; /* [15: 8],        r/w,       0xe7 */
            uint32_t reg_uv_ac_val_97               :  8; /* [23:16],        r/w,       0xe8 */
            uint32_t reg_uv_ac_val_98               :  8; /* [31:24],        r/w,       0xe9 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg29;

    /* 0x3C8 : jdec_uv_ac_cfg2A */
    union {
        struct {
            uint32_t reg_uv_ac_val_99               :  8; /* [ 7: 0],        r/w,       0xea */
            uint32_t reg_uv_ac_val_9A               :  8; /* [15: 8],        r/w,       0xf2 */
            uint32_t reg_uv_ac_val_9B               :  8; /* [23:16],        r/w,       0xf3 */
            uint32_t reg_uv_ac_val_9C               :  8; /* [31:24],        r/w,       0xf4 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg2A;

    /* 0x3CC : jdec_uv_ac_cfg2B */
    union {
        struct {
            uint32_t reg_uv_ac_val_9D               :  8; /* [ 7: 0],        r/w,       0xf5 */
            uint32_t reg_uv_ac_val_9E               :  8; /* [15: 8],        r/w,       0xf6 */
            uint32_t reg_uv_ac_val_9F               :  8; /* [23:16],        r/w,       0xf7 */
            uint32_t reg_uv_ac_val_A0               :  8; /* [31:24],        r/w,       0xf8 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg2B;

    /* 0x3D0 : jdec_uv_ac_cfg2C */
    union {
        struct {
            uint32_t reg_uv_ac_val_A1               :  8; /* [ 7: 0],        r/w,       0xf9 */
            uint32_t reg_uv_ac_val_A2               :  8; /* [15: 8],        r/w,       0xfa */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } jdec_uv_ac_cfg2C;

};

typedef volatile struct mjcomb_csr_reg mjcomb_csr_reg_t;


#endif  /* __MJCOMB_CSR_REG_H__ */
