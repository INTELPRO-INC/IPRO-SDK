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

#ifndef  __SDH_REG_H__
#define  __SDH_REG_H__

#include "ipro7.h"

/* 0x00 : System Address Low Register */
#define SDH_SD_SYS_ADDR_LOW_OFFSET                              (0x00)
#define SDH_DMA_ADDR_L                                          SDH_DMA_ADDR_L
#define SDH_DMA_ADDR_L_POS                                      (0U)
#define SDH_DMA_ADDR_L_LEN                                      (16U)
#define SDH_DMA_ADDR_L_MSK                                      (((1U<<SDH_DMA_ADDR_L_LEN)-1)<<SDH_DMA_ADDR_L_POS)
#define SDH_DMA_ADDR_L_UMSK                                     (~(((1U<<SDH_DMA_ADDR_L_LEN)-1)<<SDH_DMA_ADDR_L_POS))

/* 0x02 : System Address High Register */
#define SDH_SD_SYS_ADDR_HIGH_OFFSET                             (0x02)
#define SDH_DMA_ADDR_H                                          SDH_DMA_ADDR_H
#define SDH_DMA_ADDR_H_POS                                      (0U)
#define SDH_DMA_ADDR_H_LEN                                      (16U)
#define SDH_DMA_ADDR_H_MSK                                      (((1U<<SDH_DMA_ADDR_H_LEN)-1)<<SDH_DMA_ADDR_H_POS)
#define SDH_DMA_ADDR_H_UMSK                                     (~(((1U<<SDH_DMA_ADDR_H_LEN)-1)<<SDH_DMA_ADDR_H_POS))

/* 0x04 : Block Size Register */
#define SDH_SD_BLOCK_SIZE_OFFSET                                (0x04)
#define SDH_BLOCK_SIZE                                          SDH_BLOCK_SIZE
#define SDH_BLOCK_SIZE_POS                                      (0U)
#define SDH_BLOCK_SIZE_LEN                                      (12U)
#define SDH_BLOCK_SIZE_MSK                                      (((1U<<SDH_BLOCK_SIZE_LEN)-1)<<SDH_BLOCK_SIZE_POS)
#define SDH_BLOCK_SIZE_UMSK                                     (~(((1U<<SDH_BLOCK_SIZE_LEN)-1)<<SDH_BLOCK_SIZE_POS))
#define SDH_HOST_DMA_BDRY                                       SDH_HOST_DMA_BDRY
#define SDH_HOST_DMA_BDRY_POS                                   (12U)
#define SDH_HOST_DMA_BDRY_LEN                                   (3U)
#define SDH_HOST_DMA_BDRY_MSK                                   (((1U<<SDH_HOST_DMA_BDRY_LEN)-1)<<SDH_HOST_DMA_BDRY_POS)
#define SDH_HOST_DMA_BDRY_UMSK                                  (~(((1U<<SDH_HOST_DMA_BDRY_LEN)-1)<<SDH_HOST_DMA_BDRY_POS))

/* 0x06 : Block Count Register */
#define SDH_SD_BLOCK_COUNT_OFFSET                               (0x06)
#define SDH_BLOCK_COUNT                                         SDH_BLOCK_COUNT
#define SDH_BLOCK_COUNT_POS                                     (0U)
#define SDH_BLOCK_COUNT_LEN                                     (16U)
#define SDH_BLOCK_COUNT_MSK                                     (((1U<<SDH_BLOCK_COUNT_LEN)-1)<<SDH_BLOCK_COUNT_POS)
#define SDH_BLOCK_COUNT_UMSK                                    (~(((1U<<SDH_BLOCK_COUNT_LEN)-1)<<SDH_BLOCK_COUNT_POS))

/* 0x08 : Argument Low Register */
#define SDH_SD_ARG_LOW_OFFSET                                   (0x08)
#define SDH_ARG_L                                               SDH_ARG_L
#define SDH_ARG_L_POS                                           (0U)
#define SDH_ARG_L_LEN                                           (16U)
#define SDH_ARG_L_MSK                                           (((1U<<SDH_ARG_L_LEN)-1)<<SDH_ARG_L_POS)
#define SDH_ARG_L_UMSK                                          (~(((1U<<SDH_ARG_L_LEN)-1)<<SDH_ARG_L_POS))

/* 0x0A : Argument High Register */
#define SDH_SD_ARG_HIGH_OFFSET                                  (0x0A)
#define SDH_ARG_H                                               SDH_ARG_H
#define SDH_ARG_H_POS                                           (0U)
#define SDH_ARG_H_LEN                                           (16U)
#define SDH_ARG_H_MSK                                           (((1U<<SDH_ARG_H_LEN)-1)<<SDH_ARG_H_POS)
#define SDH_ARG_H_UMSK                                          (~(((1U<<SDH_ARG_H_LEN)-1)<<SDH_ARG_H_POS))

/* 0x0C : Transfer Mode Register */
#define SDH_SD_TRANSFER_MODE_OFFSET                             (0x0C)
#define SDH_DMA_EN                                              SDH_DMA_EN
#define SDH_DMA_EN_POS                                          (0U)
#define SDH_DMA_EN_LEN                                          (1U)
#define SDH_DMA_EN_MSK                                          (((1U<<SDH_DMA_EN_LEN)-1)<<SDH_DMA_EN_POS)
#define SDH_DMA_EN_UMSK                                         (~(((1U<<SDH_DMA_EN_LEN)-1)<<SDH_DMA_EN_POS))
#define SDH_BLK_CNT_EN                                          SDH_BLK_CNT_EN
#define SDH_BLK_CNT_EN_POS                                      (1U)
#define SDH_BLK_CNT_EN_LEN                                      (1U)
#define SDH_BLK_CNT_EN_MSK                                      (((1U<<SDH_BLK_CNT_EN_LEN)-1)<<SDH_BLK_CNT_EN_POS)
#define SDH_BLK_CNT_EN_UMSK                                     (~(((1U<<SDH_BLK_CNT_EN_LEN)-1)<<SDH_BLK_CNT_EN_POS))
#define SDH_AUTO_CMD_EN                                         SDH_AUTO_CMD_EN
#define SDH_AUTO_CMD_EN_POS                                     (2U)
#define SDH_AUTO_CMD_EN_LEN                                     (2U)
#define SDH_AUTO_CMD_EN_MSK                                     (((1U<<SDH_AUTO_CMD_EN_LEN)-1)<<SDH_AUTO_CMD_EN_POS)
#define SDH_AUTO_CMD_EN_UMSK                                    (~(((1U<<SDH_AUTO_CMD_EN_LEN)-1)<<SDH_AUTO_CMD_EN_POS))
#define SDH_TO_HOST_DIR                                         SDH_TO_HOST_DIR
#define SDH_TO_HOST_DIR_POS                                     (4U)
#define SDH_TO_HOST_DIR_LEN                                     (1U)
#define SDH_TO_HOST_DIR_MSK                                     (((1U<<SDH_TO_HOST_DIR_LEN)-1)<<SDH_TO_HOST_DIR_POS)
#define SDH_TO_HOST_DIR_UMSK                                    (~(((1U<<SDH_TO_HOST_DIR_LEN)-1)<<SDH_TO_HOST_DIR_POS))
#define SDH_MULTI_BLK_SEL                                       SDH_MULTI_BLK_SEL
#define SDH_MULTI_BLK_SEL_POS                                   (5U)
#define SDH_MULTI_BLK_SEL_LEN                                   (1U)
#define SDH_MULTI_BLK_SEL_MSK                                   (((1U<<SDH_MULTI_BLK_SEL_LEN)-1)<<SDH_MULTI_BLK_SEL_POS)
#define SDH_MULTI_BLK_SEL_UMSK                                  (~(((1U<<SDH_MULTI_BLK_SEL_LEN)-1)<<SDH_MULTI_BLK_SEL_POS))
#define SDH_CMD23_EMMC_MODE                                     SDH_CMD23_EMMC_MODE
#define SDH_CMD23_EMMC_MODE_POS                                 (10U)
#define SDH_CMD23_EMMC_MODE_LEN                                 (1U)
#define SDH_CMD23_EMMC_MODE_MSK                                 (((1U<<SDH_CMD23_EMMC_MODE_LEN)-1)<<SDH_CMD23_EMMC_MODE_POS)
#define SDH_CMD23_EMMC_MODE_UMSK                                (~(((1U<<SDH_CMD23_EMMC_MODE_LEN)-1)<<SDH_CMD23_EMMC_MODE_POS))
#define SDH_STREAM_MODE_EN                                      SDH_STREAM_MODE_EN
#define SDH_STREAM_MODE_EN_POS                                  (11U)
#define SDH_STREAM_MODE_EN_LEN                                  (1U)
#define SDH_STREAM_MODE_EN_MSK                                  (((1U<<SDH_STREAM_MODE_EN_LEN)-1)<<SDH_STREAM_MODE_EN_POS)
#define SDH_STREAM_MODE_EN_UMSK                                 (~(((1U<<SDH_STREAM_MODE_EN_LEN)-1)<<SDH_STREAM_MODE_EN_POS))
#define SDH_SPI_MODE_EN                                         SDH_SPI_MODE_EN
#define SDH_SPI_MODE_EN_POS                                     (12U)
#define SDH_SPI_MODE_EN_LEN                                     (1U)
#define SDH_SPI_MODE_EN_MSK                                     (((1U<<SDH_SPI_MODE_EN_LEN)-1)<<SDH_SPI_MODE_EN_POS)
#define SDH_SPI_MODE_EN_UMSK                                    (~(((1U<<SDH_SPI_MODE_EN_LEN)-1)<<SDH_SPI_MODE_EN_POS))
#define SDH_BOOT_ACK_EN                                         SDH_BOOT_ACK_EN
#define SDH_BOOT_ACK_EN_POS                                     (13U)
#define SDH_BOOT_ACK_EN_LEN                                     (1U)
#define SDH_BOOT_ACK_EN_MSK                                     (((1U<<SDH_BOOT_ACK_EN_LEN)-1)<<SDH_BOOT_ACK_EN_POS)
#define SDH_BOOT_ACK_EN_UMSK                                    (~(((1U<<SDH_BOOT_ACK_EN_LEN)-1)<<SDH_BOOT_ACK_EN_POS))
#define SDH_ALT_BOOT_OPREATION                                  SDH_ALT_BOOT_OPREATION
#define SDH_ALT_BOOT_OPREATION_POS                              (14U)
#define SDH_ALT_BOOT_OPREATION_LEN                              (1U)
#define SDH_ALT_BOOT_OPREATION_MSK                              (((1U<<SDH_ALT_BOOT_OPREATION_LEN)-1)<<SDH_ALT_BOOT_OPREATION_POS)
#define SDH_ALT_BOOT_OPREATION_UMSK                             (~(((1U<<SDH_ALT_BOOT_OPREATION_LEN)-1)<<SDH_ALT_BOOT_OPREATION_POS))
#define SDH_BOOT_OPREATION                                      SDH_BOOT_OPREATION
#define SDH_BOOT_OPREATION_POS                                  (15U)
#define SDH_BOOT_OPREATION_LEN                                  (1U)
#define SDH_BOOT_OPREATION_MSK                                  (((1U<<SDH_BOOT_OPREATION_LEN)-1)<<SDH_BOOT_OPREATION_POS)
#define SDH_BOOT_OPREATION_UMSK                                 (~(((1U<<SDH_BOOT_OPREATION_LEN)-1)<<SDH_BOOT_OPREATION_POS))

/* 0x0E : Command Register */
#define SDH_SD_CMD_OFFSET                                       (0x0E)
#define SDH_RESP_TYPE                                           SDH_RESP_TYPE
#define SDH_RESP_TYPE_POS                                       (0U)
#define SDH_RESP_TYPE_LEN                                       (2U)
#define SDH_RESP_TYPE_MSK                                       (((1U<<SDH_RESP_TYPE_LEN)-1)<<SDH_RESP_TYPE_POS)
#define SDH_RESP_TYPE_UMSK                                      (~(((1U<<SDH_RESP_TYPE_LEN)-1)<<SDH_RESP_TYPE_POS))
#define SDH_CMD_CRC_CHK_EN                                      SDH_CMD_CRC_CHK_EN
#define SDH_CMD_CRC_CHK_EN_POS                                  (3U)
#define SDH_CMD_CRC_CHK_EN_LEN                                  (1U)
#define SDH_CMD_CRC_CHK_EN_MSK                                  (((1U<<SDH_CMD_CRC_CHK_EN_LEN)-1)<<SDH_CMD_CRC_CHK_EN_POS)
#define SDH_CMD_CRC_CHK_EN_UMSK                                 (~(((1U<<SDH_CMD_CRC_CHK_EN_LEN)-1)<<SDH_CMD_CRC_CHK_EN_POS))
#define SDH_CMD_INDEX_CHK_EN                                    SDH_CMD_INDEX_CHK_EN
#define SDH_CMD_INDEX_CHK_EN_POS                                (4U)
#define SDH_CMD_INDEX_CHK_EN_LEN                                (1U)
#define SDH_CMD_INDEX_CHK_EN_MSK                                (((1U<<SDH_CMD_INDEX_CHK_EN_LEN)-1)<<SDH_CMD_INDEX_CHK_EN_POS)
#define SDH_CMD_INDEX_CHK_EN_UMSK                               (~(((1U<<SDH_CMD_INDEX_CHK_EN_LEN)-1)<<SDH_CMD_INDEX_CHK_EN_POS))
#define SDH_DATA_PRESENT                                        SDH_DATA_PRESENT
#define SDH_DATA_PRESENT_POS                                    (5U)
#define SDH_DATA_PRESENT_LEN                                    (1U)
#define SDH_DATA_PRESENT_MSK                                    (((1U<<SDH_DATA_PRESENT_LEN)-1)<<SDH_DATA_PRESENT_POS)
#define SDH_DATA_PRESENT_UMSK                                   (~(((1U<<SDH_DATA_PRESENT_LEN)-1)<<SDH_DATA_PRESENT_POS))
#define SDH_CMD_TYPE                                            SDH_CMD_TYPE
#define SDH_CMD_TYPE_POS                                        (6U)
#define SDH_CMD_TYPE_LEN                                        (2U)
#define SDH_CMD_TYPE_MSK                                        (((1U<<SDH_CMD_TYPE_LEN)-1)<<SDH_CMD_TYPE_POS)
#define SDH_CMD_TYPE_UMSK                                       (~(((1U<<SDH_CMD_TYPE_LEN)-1)<<SDH_CMD_TYPE_POS))
#define SDH_CMD_INDEX                                           SDH_CMD_INDEX
#define SDH_CMD_INDEX_POS                                       (8U)
#define SDH_CMD_INDEX_LEN                                       (6U)
#define SDH_CMD_INDEX_MSK                                       (((1U<<SDH_CMD_INDEX_LEN)-1)<<SDH_CMD_INDEX_POS)
#define SDH_CMD_INDEX_UMSK                                      (~(((1U<<SDH_CMD_INDEX_LEN)-1)<<SDH_CMD_INDEX_POS))

/* 0x10 : Response Register 0 */
#define SDH_SD_RESP_0_OFFSET                                    (0x10)
#define SDH_RESP0                                               SDH_RESP0
#define SDH_RESP0_POS                                           (0U)
#define SDH_RESP0_LEN                                           (16U)
#define SDH_RESP0_MSK                                           (((1U<<SDH_RESP0_LEN)-1)<<SDH_RESP0_POS)
#define SDH_RESP0_UMSK                                          (~(((1U<<SDH_RESP0_LEN)-1)<<SDH_RESP0_POS))

/* 0x12 : Response Register 1 */
#define SDH_SD_RESP_1_OFFSET                                    (0x12)
#define SDH_RESP1                                               SDH_RESP1
#define SDH_RESP1_POS                                           (0U)
#define SDH_RESP1_LEN                                           (16U)
#define SDH_RESP1_MSK                                           (((1U<<SDH_RESP1_LEN)-1)<<SDH_RESP1_POS)
#define SDH_RESP1_UMSK                                          (~(((1U<<SDH_RESP1_LEN)-1)<<SDH_RESP1_POS))

/* 0x14 : Response Register 2 */
#define SDH_SD_RESP_2_OFFSET                                    (0x14)
#define SDH_RESP2                                               SDH_RESP2
#define SDH_RESP2_POS                                           (0U)
#define SDH_RESP2_LEN                                           (16U)
#define SDH_RESP2_MSK                                           (((1U<<SDH_RESP2_LEN)-1)<<SDH_RESP2_POS)
#define SDH_RESP2_UMSK                                          (~(((1U<<SDH_RESP2_LEN)-1)<<SDH_RESP2_POS))

/* 0x16 : Response Register 3 */
#define SDH_SD_RESP_3_OFFSET                                    (0x16)
#define SDH_RESP3                                               SDH_RESP3
#define SDH_RESP3_POS                                           (0U)
#define SDH_RESP3_LEN                                           (16U)
#define SDH_RESP3_MSK                                           (((1U<<SDH_RESP3_LEN)-1)<<SDH_RESP3_POS)
#define SDH_RESP3_UMSK                                          (~(((1U<<SDH_RESP3_LEN)-1)<<SDH_RESP3_POS))

/* 0x18 : Response Register 4 */
#define SDH_SD_RESP_4_OFFSET                                    (0x18)
#define SDH_RESP4                                               SDH_RESP4
#define SDH_RESP4_POS                                           (0U)
#define SDH_RESP4_LEN                                           (16U)
#define SDH_RESP4_MSK                                           (((1U<<SDH_RESP4_LEN)-1)<<SDH_RESP4_POS)
#define SDH_RESP4_UMSK                                          (~(((1U<<SDH_RESP4_LEN)-1)<<SDH_RESP4_POS))

/* 0x1A : Response Register 5 */
#define SDH_SD_RESP_5_OFFSET                                    (0x1A)
#define SDH_RESP5                                               SDH_RESP5
#define SDH_RESP5_POS                                           (0U)
#define SDH_RESP5_LEN                                           (16U)
#define SDH_RESP5_MSK                                           (((1U<<SDH_RESP5_LEN)-1)<<SDH_RESP5_POS)
#define SDH_RESP5_UMSK                                          (~(((1U<<SDH_RESP5_LEN)-1)<<SDH_RESP5_POS))

/* 0x1C : Response Register 6 */
#define SDH_SD_RESP_6_OFFSET                                    (0x1C)
#define SDH_RESP6                                               SDH_RESP6
#define SDH_RESP6_POS                                           (0U)
#define SDH_RESP6_LEN                                           (16U)
#define SDH_RESP6_MSK                                           (((1U<<SDH_RESP6_LEN)-1)<<SDH_RESP6_POS)
#define SDH_RESP6_UMSK                                          (~(((1U<<SDH_RESP6_LEN)-1)<<SDH_RESP6_POS))

/* 0x1E : Response Register 7 */
#define SDH_SD_RESP_7_OFFSET                                    (0x1E)
#define SDH_RESP7                                               SDH_RESP7
#define SDH_RESP7_POS                                           (0U)
#define SDH_RESP7_LEN                                           (16U)
#define SDH_RESP7_MSK                                           (((1U<<SDH_RESP7_LEN)-1)<<SDH_RESP7_POS)
#define SDH_RESP7_UMSK                                          (~(((1U<<SDH_RESP7_LEN)-1)<<SDH_RESP7_POS))

/* 0x20 : Buffer Data Port 0 Register */
#define SDH_SD_BUFFER_DATA_PORT_0_OFFSET                        (0x20)
#define SDH_SD_BUFFER_DATA_PORT_0                               SDH_SD_BUFFER_DATA_PORT_0
#define SDH_SD_BUFFER_DATA_PORT_0_POS                           (0U)
#define SDH_SD_BUFFER_DATA_PORT_0_LEN                           (16U)
#define SDH_SD_BUFFER_DATA_PORT_0_MSK                           (((1U<<SDH_SD_BUFFER_DATA_PORT_0_LEN)-1)<<SDH_SD_BUFFER_DATA_PORT_0_POS)
#define SDH_SD_BUFFER_DATA_PORT_0_UMSK                          (~(((1U<<SDH_SD_BUFFER_DATA_PORT_0_LEN)-1)<<SDH_SD_BUFFER_DATA_PORT_0_POS))

/* 0x22 : Buffer Data Port 1 Register */
#define SDH_SD_BUFFER_DATA_PORT_1_OFFSET                        (0x22)
#define SDH_SD_BUFFER_DATA_PORT_1                               SDH_SD_BUFFER_DATA_PORT_1
#define SDH_SD_BUFFER_DATA_PORT_1_POS                           (0U)
#define SDH_SD_BUFFER_DATA_PORT_1_LEN                           (16U)
#define SDH_SD_BUFFER_DATA_PORT_1_MSK                           (((1U<<SDH_SD_BUFFER_DATA_PORT_1_LEN)-1)<<SDH_SD_BUFFER_DATA_PORT_1_POS)
#define SDH_SD_BUFFER_DATA_PORT_1_UMSK                          (~(((1U<<SDH_SD_BUFFER_DATA_PORT_1_LEN)-1)<<SDH_SD_BUFFER_DATA_PORT_1_POS))

/* 0x24 : Present State Register 1 */
#define SDH_SD_PRESENT_STATE_1_OFFSET                           (0x24)
#define SDH_CMD_INHIBIT_CMD                                     SDH_CMD_INHIBIT_CMD
#define SDH_CMD_INHIBIT_CMD_POS                                 (0U)
#define SDH_CMD_INHIBIT_CMD_LEN                                 (1U)
#define SDH_CMD_INHIBIT_CMD_MSK                                 (((1U<<SDH_CMD_INHIBIT_CMD_LEN)-1)<<SDH_CMD_INHIBIT_CMD_POS)
#define SDH_CMD_INHIBIT_CMD_UMSK                                (~(((1U<<SDH_CMD_INHIBIT_CMD_LEN)-1)<<SDH_CMD_INHIBIT_CMD_POS))
#define SDH_CMD_INHIBIT_DAT                                     SDH_CMD_INHIBIT_DAT
#define SDH_CMD_INHIBIT_DAT_POS                                 (1U)
#define SDH_CMD_INHIBIT_DAT_LEN                                 (1U)
#define SDH_CMD_INHIBIT_DAT_MSK                                 (((1U<<SDH_CMD_INHIBIT_DAT_LEN)-1)<<SDH_CMD_INHIBIT_DAT_POS)
#define SDH_CMD_INHIBIT_DAT_UMSK                                (~(((1U<<SDH_CMD_INHIBIT_DAT_LEN)-1)<<SDH_CMD_INHIBIT_DAT_POS))
#define SDH_DAT_ACTIVE                                          SDH_DAT_ACTIVE
#define SDH_DAT_ACTIVE_POS                                      (2U)
#define SDH_DAT_ACTIVE_LEN                                      (1U)
#define SDH_DAT_ACTIVE_MSK                                      (((1U<<SDH_DAT_ACTIVE_LEN)-1)<<SDH_DAT_ACTIVE_POS)
#define SDH_DAT_ACTIVE_UMSK                                     (~(((1U<<SDH_DAT_ACTIVE_LEN)-1)<<SDH_DAT_ACTIVE_POS))
#define SDH_RETUNING_REQ                                        SDH_RETUNING_REQ
#define SDH_RETUNING_REQ_POS                                    (3U)
#define SDH_RETUNING_REQ_LEN                                    (1U)
#define SDH_RETUNING_REQ_MSK                                    (((1U<<SDH_RETUNING_REQ_LEN)-1)<<SDH_RETUNING_REQ_POS)
#define SDH_RETUNING_REQ_UMSK                                   (~(((1U<<SDH_RETUNING_REQ_LEN)-1)<<SDH_RETUNING_REQ_POS))
#define SDH_TX_ACTIVE                                           SDH_TX_ACTIVE
#define SDH_TX_ACTIVE_POS                                       (8U)
#define SDH_TX_ACTIVE_LEN                                       (1U)
#define SDH_TX_ACTIVE_MSK                                       (((1U<<SDH_TX_ACTIVE_LEN)-1)<<SDH_TX_ACTIVE_POS)
#define SDH_TX_ACTIVE_UMSK                                      (~(((1U<<SDH_TX_ACTIVE_LEN)-1)<<SDH_TX_ACTIVE_POS))
#define SDH_RX_ACTIVE                                           SDH_RX_ACTIVE
#define SDH_RX_ACTIVE_POS                                       (9U)
#define SDH_RX_ACTIVE_LEN                                       (1U)
#define SDH_RX_ACTIVE_MSK                                       (((1U<<SDH_RX_ACTIVE_LEN)-1)<<SDH_RX_ACTIVE_POS)
#define SDH_RX_ACTIVE_UMSK                                      (~(((1U<<SDH_RX_ACTIVE_LEN)-1)<<SDH_RX_ACTIVE_POS))
#define SDH_BUFFER_WR_EN                                        SDH_BUFFER_WR_EN
#define SDH_BUFFER_WR_EN_POS                                    (10U)
#define SDH_BUFFER_WR_EN_LEN                                    (1U)
#define SDH_BUFFER_WR_EN_MSK                                    (((1U<<SDH_BUFFER_WR_EN_LEN)-1)<<SDH_BUFFER_WR_EN_POS)
#define SDH_BUFFER_WR_EN_UMSK                                   (~(((1U<<SDH_BUFFER_WR_EN_LEN)-1)<<SDH_BUFFER_WR_EN_POS))
#define SDH_BUFFER_RD_EN                                        SDH_BUFFER_RD_EN
#define SDH_BUFFER_RD_EN_POS                                    (11U)
#define SDH_BUFFER_RD_EN_LEN                                    (1U)
#define SDH_BUFFER_RD_EN_MSK                                    (((1U<<SDH_BUFFER_RD_EN_LEN)-1)<<SDH_BUFFER_RD_EN_POS)
#define SDH_BUFFER_RD_EN_UMSK                                   (~(((1U<<SDH_BUFFER_RD_EN_LEN)-1)<<SDH_BUFFER_RD_EN_POS))

/* 0x26 : Present State Register 2 */
#define SDH_SD_PRESENT_STATE_2_OFFSET                           (0x26)
#define SDH_CARD_INSERTED                                       SDH_CARD_INSERTED
#define SDH_CARD_INSERTED_POS                                   (0U)
#define SDH_CARD_INSERTED_LEN                                   (1U)
#define SDH_CARD_INSERTED_MSK                                   (((1U<<SDH_CARD_INSERTED_LEN)-1)<<SDH_CARD_INSERTED_POS)
#define SDH_CARD_INSERTED_UMSK                                  (~(((1U<<SDH_CARD_INSERTED_LEN)-1)<<SDH_CARD_INSERTED_POS))
#define SDH_CARD_STABLE                                         SDH_CARD_STABLE
#define SDH_CARD_STABLE_POS                                     (1U)
#define SDH_CARD_STABLE_LEN                                     (1U)
#define SDH_CARD_STABLE_MSK                                     (((1U<<SDH_CARD_STABLE_LEN)-1)<<SDH_CARD_STABLE_POS)
#define SDH_CARD_STABLE_UMSK                                    (~(((1U<<SDH_CARD_STABLE_LEN)-1)<<SDH_CARD_STABLE_POS))
#define SDH_CARD_DET                                            SDH_CARD_DET
#define SDH_CARD_DET_POS                                        (2U)
#define SDH_CARD_DET_LEN                                        (1U)
#define SDH_CARD_DET_MSK                                        (((1U<<SDH_CARD_DET_LEN)-1)<<SDH_CARD_DET_POS)
#define SDH_CARD_DET_UMSK                                       (~(((1U<<SDH_CARD_DET_LEN)-1)<<SDH_CARD_DET_POS))
#define SDH_WRITE_PROT                                          SDH_WRITE_PROT
#define SDH_WRITE_PROT_POS                                      (3U)
#define SDH_WRITE_PROT_LEN                                      (1U)
#define SDH_WRITE_PROT_MSK                                      (((1U<<SDH_WRITE_PROT_LEN)-1)<<SDH_WRITE_PROT_POS)
#define SDH_WRITE_PROT_UMSK                                     (~(((1U<<SDH_WRITE_PROT_LEN)-1)<<SDH_WRITE_PROT_POS))
#define SDH_DAT_LEVEL                                           SDH_DAT_LEVEL
#define SDH_DAT_LEVEL_POS                                       (4U)
#define SDH_DAT_LEVEL_LEN                                       (4U)
#define SDH_DAT_LEVEL_MSK                                       (((1U<<SDH_DAT_LEVEL_LEN)-1)<<SDH_DAT_LEVEL_POS)
#define SDH_DAT_LEVEL_UMSK                                      (~(((1U<<SDH_DAT_LEVEL_LEN)-1)<<SDH_DAT_LEVEL_POS))
#define SDH_CMD_LEVEL                                           SDH_CMD_LEVEL
#define SDH_CMD_LEVEL_POS                                       (8U)
#define SDH_CMD_LEVEL_LEN                                       (1U)
#define SDH_CMD_LEVEL_MSK                                       (((1U<<SDH_CMD_LEVEL_LEN)-1)<<SDH_CMD_LEVEL_POS)
#define SDH_CMD_LEVEL_UMSK                                      (~(((1U<<SDH_CMD_LEVEL_LEN)-1)<<SDH_CMD_LEVEL_POS))

/* 0x28 : Host Control Register */
#define SDH_SD_HOST_CTRL_OFFSET                                 (0x28)
#define SDH_LED_CTRL                                            SDH_LED_CTRL
#define SDH_LED_CTRL_POS                                        (0U)
#define SDH_LED_CTRL_LEN                                        (1U)
#define SDH_LED_CTRL_MSK                                        (((1U<<SDH_LED_CTRL_LEN)-1)<<SDH_LED_CTRL_POS)
#define SDH_LED_CTRL_UMSK                                       (~(((1U<<SDH_LED_CTRL_LEN)-1)<<SDH_LED_CTRL_POS))
#define SDH_DATA_WIDTH                                          SDH_DATA_WIDTH
#define SDH_DATA_WIDTH_POS                                      (1U)
#define SDH_DATA_WIDTH_LEN                                      (1U)
#define SDH_DATA_WIDTH_MSK                                      (((1U<<SDH_DATA_WIDTH_LEN)-1)<<SDH_DATA_WIDTH_POS)
#define SDH_DATA_WIDTH_UMSK                                     (~(((1U<<SDH_DATA_WIDTH_LEN)-1)<<SDH_DATA_WIDTH_POS))
#define SDH_HI_SPEED_EN                                         SDH_HI_SPEED_EN
#define SDH_HI_SPEED_EN_POS                                     (2U)
#define SDH_HI_SPEED_EN_LEN                                     (1U)
#define SDH_HI_SPEED_EN_MSK                                     (((1U<<SDH_HI_SPEED_EN_LEN)-1)<<SDH_HI_SPEED_EN_POS)
#define SDH_HI_SPEED_EN_UMSK                                    (~(((1U<<SDH_HI_SPEED_EN_LEN)-1)<<SDH_HI_SPEED_EN_POS))
#define SDH_DMA_SEL                                             SDH_DMA_SEL
#define SDH_DMA_SEL_POS                                         (3U)
#define SDH_DMA_SEL_LEN                                         (2U)
#define SDH_DMA_SEL_MSK                                         (((1U<<SDH_DMA_SEL_LEN)-1)<<SDH_DMA_SEL_POS)
#define SDH_DMA_SEL_UMSK                                        (~(((1U<<SDH_DMA_SEL_LEN)-1)<<SDH_DMA_SEL_POS))
#define SDH_EX_DATA_WIDTH                                       SDH_EX_DATA_WIDTH
#define SDH_EX_DATA_WIDTH_POS                                   (5U)
#define SDH_EX_DATA_WIDTH_LEN                                   (1U)
#define SDH_EX_DATA_WIDTH_MSK                                   (((1U<<SDH_EX_DATA_WIDTH_LEN)-1)<<SDH_EX_DATA_WIDTH_POS)
#define SDH_EX_DATA_WIDTH_UMSK                                  (~(((1U<<SDH_EX_DATA_WIDTH_LEN)-1)<<SDH_EX_DATA_WIDTH_POS))
#define SDH_CARD_DET_L                                          SDH_CARD_DET_L
#define SDH_CARD_DET_L_POS                                      (6U)
#define SDH_CARD_DET_L_LEN                                      (1U)
#define SDH_CARD_DET_L_MSK                                      (((1U<<SDH_CARD_DET_L_LEN)-1)<<SDH_CARD_DET_L_POS)
#define SDH_CARD_DET_L_UMSK                                     (~(((1U<<SDH_CARD_DET_L_LEN)-1)<<SDH_CARD_DET_L_POS))
#define SDH_CARD_DET_S                                          SDH_CARD_DET_S
#define SDH_CARD_DET_S_POS                                      (7U)
#define SDH_CARD_DET_S_LEN                                      (1U)
#define SDH_CARD_DET_S_MSK                                      (((1U<<SDH_CARD_DET_S_LEN)-1)<<SDH_CARD_DET_S_POS)
#define SDH_CARD_DET_S_UMSK                                     (~(((1U<<SDH_CARD_DET_S_LEN)-1)<<SDH_CARD_DET_S_POS))
#define SDH_SD_BUS_POWER                                        SDH_SD_BUS_POWER
#define SDH_SD_BUS_POWER_POS                                    (8U)
#define SDH_SD_BUS_POWER_LEN                                    (1U)
#define SDH_SD_BUS_POWER_MSK                                    (((1U<<SDH_SD_BUS_POWER_LEN)-1)<<SDH_SD_BUS_POWER_POS)
#define SDH_SD_BUS_POWER_UMSK                                   (~(((1U<<SDH_SD_BUS_POWER_LEN)-1)<<SDH_SD_BUS_POWER_POS))
#define SDH_SD_BUS_VLT                                          SDH_SD_BUS_VLT
#define SDH_SD_BUS_VLT_POS                                      (9U)
#define SDH_SD_BUS_VLT_LEN                                      (3U)
#define SDH_SD_BUS_VLT_MSK                                      (((1U<<SDH_SD_BUS_VLT_LEN)-1)<<SDH_SD_BUS_VLT_POS)
#define SDH_SD_BUS_VLT_UMSK                                     (~(((1U<<SDH_SD_BUS_VLT_LEN)-1)<<SDH_SD_BUS_VLT_POS))
#define SDH_SMIH_OD_PP                                          SDH_SMIH_OD_PP
#define SDH_SMIH_OD_PP_POS                                      (12U)
#define SDH_SMIH_OD_PP_LEN                                      (1U)
#define SDH_SMIH_OD_PP_MSK                                      (((1U<<SDH_SMIH_OD_PP_LEN)-1)<<SDH_SMIH_OD_PP_POS)
#define SDH_SMIH_OD_PP_UMSK                                     (~(((1U<<SDH_SMIH_OD_PP_LEN)-1)<<SDH_SMIH_OD_PP_POS))
#define SDH_SMIH_RST_N                                          SDH_SMIH_RST_N
#define SDH_SMIH_RST_N_POS                                      (13U)
#define SDH_SMIH_RST_N_LEN                                      (1U)
#define SDH_SMIH_RST_N_MSK                                      (((1U<<SDH_SMIH_RST_N_LEN)-1)<<SDH_SMIH_RST_N_POS)
#define SDH_SMIH_RST_N_UMSK                                     (~(((1U<<SDH_SMIH_RST_N_LEN)-1)<<SDH_SMIH_RST_N_POS))

/* 0x2A : Block Gap Control Register */
#define SDH_SD_BLOCK_GAP_CTRL_OFFSET                            (0x2A)
#define SDH_STOP_AT_BLOCK_GAP_REQ                               SDH_STOP_AT_BLOCK_GAP_REQ
#define SDH_STOP_AT_BLOCK_GAP_REQ_POS                           (0U)
#define SDH_STOP_AT_BLOCK_GAP_REQ_LEN                           (1U)
#define SDH_STOP_AT_BLOCK_GAP_REQ_MSK                           (((1U<<SDH_STOP_AT_BLOCK_GAP_REQ_LEN)-1)<<SDH_STOP_AT_BLOCK_GAP_REQ_POS)
#define SDH_STOP_AT_BLOCK_GAP_REQ_UMSK                          (~(((1U<<SDH_STOP_AT_BLOCK_GAP_REQ_LEN)-1)<<SDH_STOP_AT_BLOCK_GAP_REQ_POS))
#define SDH_CONT_REQ                                            SDH_CONT_REQ
#define SDH_CONT_REQ_POS                                        (1U)
#define SDH_CONT_REQ_LEN                                        (1U)
#define SDH_CONT_REQ_MSK                                        (((1U<<SDH_CONT_REQ_LEN)-1)<<SDH_CONT_REQ_POS)
#define SDH_CONT_REQ_UMSK                                       (~(((1U<<SDH_CONT_REQ_LEN)-1)<<SDH_CONT_REQ_POS))
#define SDH_RD_WAIT_CTL                                         SDH_RD_WAIT_CTL
#define SDH_RD_WAIT_CTL_POS                                     (2U)
#define SDH_RD_WAIT_CTL_LEN                                     (1U)
#define SDH_RD_WAIT_CTL_MSK                                     (((1U<<SDH_RD_WAIT_CTL_LEN)-1)<<SDH_RD_WAIT_CTL_POS)
#define SDH_RD_WAIT_CTL_UMSK                                    (~(((1U<<SDH_RD_WAIT_CTL_LEN)-1)<<SDH_RD_WAIT_CTL_POS))
#define SDH_INT_BLK_GAP                                         SDH_INT_BLK_GAP
#define SDH_INT_BLK_GAP_POS                                     (3U)
#define SDH_INT_BLK_GAP_LEN                                     (1U)
#define SDH_INT_BLK_GAP_MSK                                     (((1U<<SDH_INT_BLK_GAP_LEN)-1)<<SDH_INT_BLK_GAP_POS)
#define SDH_INT_BLK_GAP_UMSK                                    (~(((1U<<SDH_INT_BLK_GAP_LEN)-1)<<SDH_INT_BLK_GAP_POS))
#define SDH_W_CARD_INT                                          SDH_W_CARD_INT
#define SDH_W_CARD_INT_POS                                      (8U)
#define SDH_W_CARD_INT_LEN                                      (1U)
#define SDH_W_CARD_INT_MSK                                      (((1U<<SDH_W_CARD_INT_LEN)-1)<<SDH_W_CARD_INT_POS)
#define SDH_W_CARD_INT_UMSK                                     (~(((1U<<SDH_W_CARD_INT_LEN)-1)<<SDH_W_CARD_INT_POS))
#define SDH_W_INSERTION                                         SDH_W_INSERTION
#define SDH_W_INSERTION_POS                                     (9U)
#define SDH_W_INSERTION_LEN                                     (1U)
#define SDH_W_INSERTION_MSK                                     (((1U<<SDH_W_INSERTION_LEN)-1)<<SDH_W_INSERTION_POS)
#define SDH_W_INSERTION_UMSK                                    (~(((1U<<SDH_W_INSERTION_LEN)-1)<<SDH_W_INSERTION_POS))
#define SDH_W_REMOVAL                                           SDH_W_REMOVAL
#define SDH_W_REMOVAL_POS                                       (10U)
#define SDH_W_REMOVAL_LEN                                       (1U)
#define SDH_W_REMOVAL_MSK                                       (((1U<<SDH_W_REMOVAL_LEN)-1)<<SDH_W_REMOVAL_POS)
#define SDH_W_REMOVAL_UMSK                                      (~(((1U<<SDH_W_REMOVAL_LEN)-1)<<SDH_W_REMOVAL_POS))

/* 0x2C : Clock Control Register */
#define SDH_SD_CLOCK_CTRL_OFFSET                                (0x2C)
#define SDH_INT_CLK_EN                                          SDH_INT_CLK_EN
#define SDH_INT_CLK_EN_POS                                      (0U)
#define SDH_INT_CLK_EN_LEN                                      (1U)
#define SDH_INT_CLK_EN_MSK                                      (((1U<<SDH_INT_CLK_EN_LEN)-1)<<SDH_INT_CLK_EN_POS)
#define SDH_INT_CLK_EN_UMSK                                     (~(((1U<<SDH_INT_CLK_EN_LEN)-1)<<SDH_INT_CLK_EN_POS))
#define SDH_INT_CLK_STABLE                                      SDH_INT_CLK_STABLE
#define SDH_INT_CLK_STABLE_POS                                  (1U)
#define SDH_INT_CLK_STABLE_LEN                                  (1U)
#define SDH_INT_CLK_STABLE_MSK                                  (((1U<<SDH_INT_CLK_STABLE_LEN)-1)<<SDH_INT_CLK_STABLE_POS)
#define SDH_INT_CLK_STABLE_UMSK                                 (~(((1U<<SDH_INT_CLK_STABLE_LEN)-1)<<SDH_INT_CLK_STABLE_POS))
#define SDH_SD_CLK_EN                                           SDH_SD_CLK_EN
#define SDH_SD_CLK_EN_POS                                       (2U)
#define SDH_SD_CLK_EN_LEN                                       (1U)
#define SDH_SD_CLK_EN_MSK                                       (((1U<<SDH_SD_CLK_EN_LEN)-1)<<SDH_SD_CLK_EN_POS)
#define SDH_SD_CLK_EN_UMSK                                      (~(((1U<<SDH_SD_CLK_EN_LEN)-1)<<SDH_SD_CLK_EN_POS))
#define SDH_CLK_GEN_SEL                                         SDH_CLK_GEN_SEL
#define SDH_CLK_GEN_SEL_POS                                     (5U)
#define SDH_CLK_GEN_SEL_LEN                                     (1U)
#define SDH_CLK_GEN_SEL_MSK                                     (((1U<<SDH_CLK_GEN_SEL_LEN)-1)<<SDH_CLK_GEN_SEL_POS)
#define SDH_CLK_GEN_SEL_UMSK                                    (~(((1U<<SDH_CLK_GEN_SEL_LEN)-1)<<SDH_CLK_GEN_SEL_POS))
#define SDH_SD_FREQ_SEL_HI                                      SDH_SD_FREQ_SEL_HI
#define SDH_SD_FREQ_SEL_HI_POS                                  (6U)
#define SDH_SD_FREQ_SEL_HI_LEN                                  (2U)
#define SDH_SD_FREQ_SEL_HI_MSK                                  (((1U<<SDH_SD_FREQ_SEL_HI_LEN)-1)<<SDH_SD_FREQ_SEL_HI_POS)
#define SDH_SD_FREQ_SEL_HI_UMSK                                 (~(((1U<<SDH_SD_FREQ_SEL_HI_LEN)-1)<<SDH_SD_FREQ_SEL_HI_POS))
#define SDH_SD_FREQ_SEL_LO                                      SDH_SD_FREQ_SEL_LO
#define SDH_SD_FREQ_SEL_LO_POS                                  (8U)
#define SDH_SD_FREQ_SEL_LO_LEN                                  (8U)
#define SDH_SD_FREQ_SEL_LO_MSK                                  (((1U<<SDH_SD_FREQ_SEL_LO_LEN)-1)<<SDH_SD_FREQ_SEL_LO_POS)
#define SDH_SD_FREQ_SEL_LO_UMSK                                 (~(((1U<<SDH_SD_FREQ_SEL_LO_LEN)-1)<<SDH_SD_FREQ_SEL_LO_POS))

/* 0x2E : Timeout Control/Software Reset Register */
#define SDH_SD_TIMEOUT_CTRL_SW_RESET_OFFSET                     (0x2E)
#define SDH_TIMEOUT_VALUE                                       SDH_TIMEOUT_VALUE
#define SDH_TIMEOUT_VALUE_POS                                   (0U)
#define SDH_TIMEOUT_VALUE_LEN                                   (4U)
#define SDH_TIMEOUT_VALUE_MSK                                   (((1U<<SDH_TIMEOUT_VALUE_LEN)-1)<<SDH_TIMEOUT_VALUE_POS)
#define SDH_TIMEOUT_VALUE_UMSK                                  (~(((1U<<SDH_TIMEOUT_VALUE_LEN)-1)<<SDH_TIMEOUT_VALUE_POS))
#define SDH_SW_RST_ALL                                          SDH_SW_RST_ALL
#define SDH_SW_RST_ALL_POS                                      (8U)
#define SDH_SW_RST_ALL_LEN                                      (1U)
#define SDH_SW_RST_ALL_MSK                                      (((1U<<SDH_SW_RST_ALL_LEN)-1)<<SDH_SW_RST_ALL_POS)
#define SDH_SW_RST_ALL_UMSK                                     (~(((1U<<SDH_SW_RST_ALL_LEN)-1)<<SDH_SW_RST_ALL_POS))
#define SDH_SW_RST_CMD                                          SDH_SW_RST_CMD
#define SDH_SW_RST_CMD_POS                                      (9U)
#define SDH_SW_RST_CMD_LEN                                      (1U)
#define SDH_SW_RST_CMD_MSK                                      (((1U<<SDH_SW_RST_CMD_LEN)-1)<<SDH_SW_RST_CMD_POS)
#define SDH_SW_RST_CMD_UMSK                                     (~(((1U<<SDH_SW_RST_CMD_LEN)-1)<<SDH_SW_RST_CMD_POS))
#define SDH_SW_RST_DAT                                          SDH_SW_RST_DAT
#define SDH_SW_RST_DAT_POS                                      (10U)
#define SDH_SW_RST_DAT_LEN                                      (1U)
#define SDH_SW_RST_DAT_MSK                                      (((1U<<SDH_SW_RST_DAT_LEN)-1)<<SDH_SW_RST_DAT_POS)
#define SDH_SW_RST_DAT_UMSK                                     (~(((1U<<SDH_SW_RST_DAT_LEN)-1)<<SDH_SW_RST_DAT_POS))

/* 0x30 : Normal Interrupt Status Register */
#define SDH_SD_NORMAL_INT_STATUS_OFFSET                         (0x30)
#define SDH_CMD_COMPLETE                                        SDH_CMD_COMPLETE
#define SDH_CMD_COMPLETE_POS                                    (0U)
#define SDH_CMD_COMPLETE_LEN                                    (1U)
#define SDH_CMD_COMPLETE_MSK                                    (((1U<<SDH_CMD_COMPLETE_LEN)-1)<<SDH_CMD_COMPLETE_POS)
#define SDH_CMD_COMPLETE_UMSK                                   (~(((1U<<SDH_CMD_COMPLETE_LEN)-1)<<SDH_CMD_COMPLETE_POS))
#define SDH_XFER_COMPLETE                                       SDH_XFER_COMPLETE
#define SDH_XFER_COMPLETE_POS                                   (1U)
#define SDH_XFER_COMPLETE_LEN                                   (1U)
#define SDH_XFER_COMPLETE_MSK                                   (((1U<<SDH_XFER_COMPLETE_LEN)-1)<<SDH_XFER_COMPLETE_POS)
#define SDH_XFER_COMPLETE_UMSK                                  (~(((1U<<SDH_XFER_COMPLETE_LEN)-1)<<SDH_XFER_COMPLETE_POS))
#define SDH_BLOCK_GAP_EVT                                       SDH_BLOCK_GAP_EVT
#define SDH_BLOCK_GAP_EVT_POS                                   (2U)
#define SDH_BLOCK_GAP_EVT_LEN                                   (1U)
#define SDH_BLOCK_GAP_EVT_MSK                                   (((1U<<SDH_BLOCK_GAP_EVT_LEN)-1)<<SDH_BLOCK_GAP_EVT_POS)
#define SDH_BLOCK_GAP_EVT_UMSK                                  (~(((1U<<SDH_BLOCK_GAP_EVT_LEN)-1)<<SDH_BLOCK_GAP_EVT_POS))
#define SDH_DMA_INT                                             SDH_DMA_INT
#define SDH_DMA_INT_POS                                         (3U)
#define SDH_DMA_INT_LEN                                         (1U)
#define SDH_DMA_INT_MSK                                         (((1U<<SDH_DMA_INT_LEN)-1)<<SDH_DMA_INT_POS)
#define SDH_DMA_INT_UMSK                                        (~(((1U<<SDH_DMA_INT_LEN)-1)<<SDH_DMA_INT_POS))
#define SDH_TX_RDY                                              SDH_TX_RDY
#define SDH_TX_RDY_POS                                          (4U)
#define SDH_TX_RDY_LEN                                          (1U)
#define SDH_TX_RDY_MSK                                          (((1U<<SDH_TX_RDY_LEN)-1)<<SDH_TX_RDY_POS)
#define SDH_TX_RDY_UMSK                                         (~(((1U<<SDH_TX_RDY_LEN)-1)<<SDH_TX_RDY_POS))
#define SDH_RX_RDY                                              SDH_RX_RDY
#define SDH_RX_RDY_POS                                          (5U)
#define SDH_RX_RDY_LEN                                          (1U)
#define SDH_RX_RDY_MSK                                          (((1U<<SDH_RX_RDY_LEN)-1)<<SDH_RX_RDY_POS)
#define SDH_RX_RDY_UMSK                                         (~(((1U<<SDH_RX_RDY_LEN)-1)<<SDH_RX_RDY_POS))
#define SDH_CARD_INS_INT                                        SDH_CARD_INS_INT
#define SDH_CARD_INS_INT_POS                                    (6U)
#define SDH_CARD_INS_INT_LEN                                    (1U)
#define SDH_CARD_INS_INT_MSK                                    (((1U<<SDH_CARD_INS_INT_LEN)-1)<<SDH_CARD_INS_INT_POS)
#define SDH_CARD_INS_INT_UMSK                                   (~(((1U<<SDH_CARD_INS_INT_LEN)-1)<<SDH_CARD_INS_INT_POS))
#define SDH_CARD_REM_INT                                        SDH_CARD_REM_INT
#define SDH_CARD_REM_INT_POS                                    (7U)
#define SDH_CARD_REM_INT_LEN                                    (1U)
#define SDH_CARD_REM_INT_MSK                                    (((1U<<SDH_CARD_REM_INT_LEN)-1)<<SDH_CARD_REM_INT_POS)
#define SDH_CARD_REM_INT_UMSK                                   (~(((1U<<SDH_CARD_REM_INT_LEN)-1)<<SDH_CARD_REM_INT_POS))
#define SDH_CARD_INT                                            SDH_CARD_INT
#define SDH_CARD_INT_POS                                        (8U)
#define SDH_CARD_INT_LEN                                        (1U)
#define SDH_CARD_INT_MSK                                        (((1U<<SDH_CARD_INT_LEN)-1)<<SDH_CARD_INT_POS)
#define SDH_CARD_INT_UMSK                                       (~(((1U<<SDH_CARD_INT_LEN)-1)<<SDH_CARD_INT_POS))
#define SDH_INT_A                                               SDH_INT_A
#define SDH_INT_A_POS                                           (9U)
#define SDH_INT_A_LEN                                           (1U)
#define SDH_INT_A_MSK                                           (((1U<<SDH_INT_A_LEN)-1)<<SDH_INT_A_POS)
#define SDH_INT_A_UMSK                                          (~(((1U<<SDH_INT_A_LEN)-1)<<SDH_INT_A_POS))
#define SDH_INT_B                                               SDH_INT_B
#define SDH_INT_B_POS                                           (10U)
#define SDH_INT_B_LEN                                           (1U)
#define SDH_INT_B_MSK                                           (((1U<<SDH_INT_B_LEN)-1)<<SDH_INT_B_POS)
#define SDH_INT_B_UMSK                                          (~(((1U<<SDH_INT_B_LEN)-1)<<SDH_INT_B_POS))
#define SDH_INT_C                                               SDH_INT_C
#define SDH_INT_C_POS                                           (11U)
#define SDH_INT_C_LEN                                           (1U)
#define SDH_INT_C_MSK                                           (((1U<<SDH_INT_C_LEN)-1)<<SDH_INT_C_POS)
#define SDH_INT_C_UMSK                                          (~(((1U<<SDH_INT_C_LEN)-1)<<SDH_INT_C_POS))
#define SDH_RETUNING_INT                                        SDH_RETUNING_INT
#define SDH_RETUNING_INT_POS                                    (12U)
#define SDH_RETUNING_INT_LEN                                    (1U)
#define SDH_RETUNING_INT_MSK                                    (((1U<<SDH_RETUNING_INT_LEN)-1)<<SDH_RETUNING_INT_POS)
#define SDH_RETUNING_INT_UMSK                                   (~(((1U<<SDH_RETUNING_INT_LEN)-1)<<SDH_RETUNING_INT_POS))
#define SDH_BOOT_COMPLETE                                       SDH_BOOT_COMPLETE
#define SDH_BOOT_COMPLETE_POS                                   (13U)
#define SDH_BOOT_COMPLETE_LEN                                   (1U)
#define SDH_BOOT_COMPLETE_MSK                                   (((1U<<SDH_BOOT_COMPLETE_LEN)-1)<<SDH_BOOT_COMPLETE_POS)
#define SDH_BOOT_COMPLETE_UMSK                                  (~(((1U<<SDH_BOOT_COMPLETE_LEN)-1)<<SDH_BOOT_COMPLETE_POS))
#define SDH_BOOT_DONE                                           SDH_BOOT_DONE
#define SDH_BOOT_DONE_POS                                       (14U)
#define SDH_BOOT_DONE_LEN                                       (1U)
#define SDH_BOOT_DONE_MSK                                       (((1U<<SDH_BOOT_DONE_LEN)-1)<<SDH_BOOT_DONE_POS)
#define SDH_BOOT_DONE_UMSK                                      (~(((1U<<SDH_BOOT_DONE_LEN)-1)<<SDH_BOOT_DONE_POS))
#define SDH_ERR_INT                                             SDH_ERR_INT
#define SDH_ERR_INT_POS                                         (15U)
#define SDH_ERR_INT_LEN                                         (1U)
#define SDH_ERR_INT_MSK                                         (((1U<<SDH_ERR_INT_LEN)-1)<<SDH_ERR_INT_POS)
#define SDH_ERR_INT_UMSK                                        (~(((1U<<SDH_ERR_INT_LEN)-1)<<SDH_ERR_INT_POS))

/* 0x32 : Error Interrupt Status Register */
#define SDH_SD_ERROR_INT_STATUS_OFFSET                          (0x32)
#define SDH_CMD_TIMEOUT_ERR                                     SDH_CMD_TIMEOUT_ERR
#define SDH_CMD_TIMEOUT_ERR_POS                                 (0U)
#define SDH_CMD_TIMEOUT_ERR_LEN                                 (1U)
#define SDH_CMD_TIMEOUT_ERR_MSK                                 (((1U<<SDH_CMD_TIMEOUT_ERR_LEN)-1)<<SDH_CMD_TIMEOUT_ERR_POS)
#define SDH_CMD_TIMEOUT_ERR_UMSK                                (~(((1U<<SDH_CMD_TIMEOUT_ERR_LEN)-1)<<SDH_CMD_TIMEOUT_ERR_POS))
#define SDH_CMD_CRC_ERR                                         SDH_CMD_CRC_ERR
#define SDH_CMD_CRC_ERR_POS                                     (1U)
#define SDH_CMD_CRC_ERR_LEN                                     (1U)
#define SDH_CMD_CRC_ERR_MSK                                     (((1U<<SDH_CMD_CRC_ERR_LEN)-1)<<SDH_CMD_CRC_ERR_POS)
#define SDH_CMD_CRC_ERR_UMSK                                    (~(((1U<<SDH_CMD_CRC_ERR_LEN)-1)<<SDH_CMD_CRC_ERR_POS))
#define SDH_CMD_END_BIT_ERR                                     SDH_CMD_END_BIT_ERR
#define SDH_CMD_END_BIT_ERR_POS                                 (2U)
#define SDH_CMD_END_BIT_ERR_LEN                                 (1U)
#define SDH_CMD_END_BIT_ERR_MSK                                 (((1U<<SDH_CMD_END_BIT_ERR_LEN)-1)<<SDH_CMD_END_BIT_ERR_POS)
#define SDH_CMD_END_BIT_ERR_UMSK                                (~(((1U<<SDH_CMD_END_BIT_ERR_LEN)-1)<<SDH_CMD_END_BIT_ERR_POS))
#define SDH_CMD_INDEX_ERR                                       SDH_CMD_INDEX_ERR
#define SDH_CMD_INDEX_ERR_POS                                   (3U)
#define SDH_CMD_INDEX_ERR_LEN                                   (1U)
#define SDH_CMD_INDEX_ERR_MSK                                   (((1U<<SDH_CMD_INDEX_ERR_LEN)-1)<<SDH_CMD_INDEX_ERR_POS)
#define SDH_CMD_INDEX_ERR_UMSK                                  (~(((1U<<SDH_CMD_INDEX_ERR_LEN)-1)<<SDH_CMD_INDEX_ERR_POS))
#define SDH_DATA_TIMEOUT_ERR                                    SDH_DATA_TIMEOUT_ERR
#define SDH_DATA_TIMEOUT_ERR_POS                                (4U)
#define SDH_DATA_TIMEOUT_ERR_LEN                                (1U)
#define SDH_DATA_TIMEOUT_ERR_MSK                                (((1U<<SDH_DATA_TIMEOUT_ERR_LEN)-1)<<SDH_DATA_TIMEOUT_ERR_POS)
#define SDH_DATA_TIMEOUT_ERR_UMSK                               (~(((1U<<SDH_DATA_TIMEOUT_ERR_LEN)-1)<<SDH_DATA_TIMEOUT_ERR_POS))
#define SDH_RD_DATA_CRC_ERR                                     SDH_RD_DATA_CRC_ERR
#define SDH_RD_DATA_CRC_ERR_POS                                 (5U)
#define SDH_RD_DATA_CRC_ERR_LEN                                 (1U)
#define SDH_RD_DATA_CRC_ERR_MSK                                 (((1U<<SDH_RD_DATA_CRC_ERR_LEN)-1)<<SDH_RD_DATA_CRC_ERR_POS)
#define SDH_RD_DATA_CRC_ERR_UMSK                                (~(((1U<<SDH_RD_DATA_CRC_ERR_LEN)-1)<<SDH_RD_DATA_CRC_ERR_POS))
#define SDH_RD_DATA_END_BIT_ERR                                 SDH_RD_DATA_END_BIT_ERR
#define SDH_RD_DATA_END_BIT_ERR_POS                             (6U)
#define SDH_RD_DATA_END_BIT_ERR_LEN                             (1U)
#define SDH_RD_DATA_END_BIT_ERR_MSK                             (((1U<<SDH_RD_DATA_END_BIT_ERR_LEN)-1)<<SDH_RD_DATA_END_BIT_ERR_POS)
#define SDH_RD_DATA_END_BIT_ERR_UMSK                            (~(((1U<<SDH_RD_DATA_END_BIT_ERR_LEN)-1)<<SDH_RD_DATA_END_BIT_ERR_POS))
#define SDH_CUR_LIMIT_ERR                                       SDH_CUR_LIMIT_ERR
#define SDH_CUR_LIMIT_ERR_POS                                   (7U)
#define SDH_CUR_LIMIT_ERR_LEN                                   (1U)
#define SDH_CUR_LIMIT_ERR_MSK                                   (((1U<<SDH_CUR_LIMIT_ERR_LEN)-1)<<SDH_CUR_LIMIT_ERR_POS)
#define SDH_CUR_LIMIT_ERR_UMSK                                  (~(((1U<<SDH_CUR_LIMIT_ERR_LEN)-1)<<SDH_CUR_LIMIT_ERR_POS))
#define SDH_AUTO_CMD12_ERR                                      SDH_AUTO_CMD12_ERR
#define SDH_AUTO_CMD12_ERR_POS                                  (8U)
#define SDH_AUTO_CMD12_ERR_LEN                                  (1U)
#define SDH_AUTO_CMD12_ERR_MSK                                  (((1U<<SDH_AUTO_CMD12_ERR_LEN)-1)<<SDH_AUTO_CMD12_ERR_POS)
#define SDH_AUTO_CMD12_ERR_UMSK                                 (~(((1U<<SDH_AUTO_CMD12_ERR_LEN)-1)<<SDH_AUTO_CMD12_ERR_POS))
#define SDH_ADMA_ERR                                            SDH_ADMA_ERR
#define SDH_ADMA_ERR_POS                                        (9U)
#define SDH_ADMA_ERR_LEN                                        (1U)
#define SDH_ADMA_ERR_MSK                                        (((1U<<SDH_ADMA_ERR_LEN)-1)<<SDH_ADMA_ERR_POS)
#define SDH_ADMA_ERR_UMSK                                       (~(((1U<<SDH_ADMA_ERR_LEN)-1)<<SDH_ADMA_ERR_POS))
#define SDH_TUNE_ERR                                            SDH_TUNE_ERR
#define SDH_TUNE_ERR_POS                                        (10U)
#define SDH_TUNE_ERR_LEN                                        (1U)
#define SDH_TUNE_ERR_MSK                                        (((1U<<SDH_TUNE_ERR_LEN)-1)<<SDH_TUNE_ERR_POS)
#define SDH_TUNE_ERR_UMSK                                       (~(((1U<<SDH_TUNE_ERR_LEN)-1)<<SDH_TUNE_ERR_POS))
#define SDH_VENDOR_SPECIFIC_ERR                                 SDH_VENDOR_SPECIFIC_ERR
#define SDH_VENDOR_SPECIFIC_ERR_POS                             (12U)
#define SDH_VENDOR_SPECIFIC_ERR_LEN                             (4U)
#define SDH_VENDOR_SPECIFIC_ERR_MSK                             (((1U<<SDH_VENDOR_SPECIFIC_ERR_LEN)-1)<<SDH_VENDOR_SPECIFIC_ERR_POS)
#define SDH_VENDOR_SPECIFIC_ERR_UMSK                            (~(((1U<<SDH_VENDOR_SPECIFIC_ERR_LEN)-1)<<SDH_VENDOR_SPECIFIC_ERR_POS))

/* 0x34 : Normal Interrupt Status Enable Register */
#define SDH_SD_NORMAL_INT_STATUS_EN_OFFSET                      (0x34)
#define SDH_CMD_COMPLETE_EN                                     SDH_CMD_COMPLETE_EN
#define SDH_CMD_COMPLETE_EN_POS                                 (0U)
#define SDH_CMD_COMPLETE_EN_LEN                                 (1U)
#define SDH_CMD_COMPLETE_EN_MSK                                 (((1U<<SDH_CMD_COMPLETE_EN_LEN)-1)<<SDH_CMD_COMPLETE_EN_POS)
#define SDH_CMD_COMPLETE_EN_UMSK                                (~(((1U<<SDH_CMD_COMPLETE_EN_LEN)-1)<<SDH_CMD_COMPLETE_EN_POS))
#define SDH_XFER_COMPLETE_EN                                    SDH_XFER_COMPLETE_EN
#define SDH_XFER_COMPLETE_EN_POS                                (1U)
#define SDH_XFER_COMPLETE_EN_LEN                                (1U)
#define SDH_XFER_COMPLETE_EN_MSK                                (((1U<<SDH_XFER_COMPLETE_EN_LEN)-1)<<SDH_XFER_COMPLETE_EN_POS)
#define SDH_XFER_COMPLETE_EN_UMSK                               (~(((1U<<SDH_XFER_COMPLETE_EN_LEN)-1)<<SDH_XFER_COMPLETE_EN_POS))
#define SDH_BLOCK_GAP_EVT_EN                                    SDH_BLOCK_GAP_EVT_EN
#define SDH_BLOCK_GAP_EVT_EN_POS                                (2U)
#define SDH_BLOCK_GAP_EVT_EN_LEN                                (1U)
#define SDH_BLOCK_GAP_EVT_EN_MSK                                (((1U<<SDH_BLOCK_GAP_EVT_EN_LEN)-1)<<SDH_BLOCK_GAP_EVT_EN_POS)
#define SDH_BLOCK_GAP_EVT_EN_UMSK                               (~(((1U<<SDH_BLOCK_GAP_EVT_EN_LEN)-1)<<SDH_BLOCK_GAP_EVT_EN_POS))
#define SDH_DMA_INT_EN                                          SDH_DMA_INT_EN
#define SDH_DMA_INT_EN_POS                                      (3U)
#define SDH_DMA_INT_EN_LEN                                      (1U)
#define SDH_DMA_INT_EN_MSK                                      (((1U<<SDH_DMA_INT_EN_LEN)-1)<<SDH_DMA_INT_EN_POS)
#define SDH_DMA_INT_EN_UMSK                                     (~(((1U<<SDH_DMA_INT_EN_LEN)-1)<<SDH_DMA_INT_EN_POS))
#define SDH_TX_RDY_EN                                           SDH_TX_RDY_EN
#define SDH_TX_RDY_EN_POS                                       (4U)
#define SDH_TX_RDY_EN_LEN                                       (1U)
#define SDH_TX_RDY_EN_MSK                                       (((1U<<SDH_TX_RDY_EN_LEN)-1)<<SDH_TX_RDY_EN_POS)
#define SDH_TX_RDY_EN_UMSK                                      (~(((1U<<SDH_TX_RDY_EN_LEN)-1)<<SDH_TX_RDY_EN_POS))
#define SDH_RD_RDY_EN                                           SDH_RD_RDY_EN
#define SDH_RD_RDY_EN_POS                                       (5U)
#define SDH_RD_RDY_EN_LEN                                       (1U)
#define SDH_RD_RDY_EN_MSK                                       (((1U<<SDH_RD_RDY_EN_LEN)-1)<<SDH_RD_RDY_EN_POS)
#define SDH_RD_RDY_EN_UMSK                                      (~(((1U<<SDH_RD_RDY_EN_LEN)-1)<<SDH_RD_RDY_EN_POS))
#define SDH_CARD_INS_EN                                         SDH_CARD_INS_EN
#define SDH_CARD_INS_EN_POS                                     (6U)
#define SDH_CARD_INS_EN_LEN                                     (1U)
#define SDH_CARD_INS_EN_MSK                                     (((1U<<SDH_CARD_INS_EN_LEN)-1)<<SDH_CARD_INS_EN_POS)
#define SDH_CARD_INS_EN_UMSK                                    (~(((1U<<SDH_CARD_INS_EN_LEN)-1)<<SDH_CARD_INS_EN_POS))
#define SDH_CARD_REM_EN                                         SDH_CARD_REM_EN
#define SDH_CARD_REM_EN_POS                                     (7U)
#define SDH_CARD_REM_EN_LEN                                     (1U)
#define SDH_CARD_REM_EN_MSK                                     (((1U<<SDH_CARD_REM_EN_LEN)-1)<<SDH_CARD_REM_EN_POS)
#define SDH_CARD_REM_EN_UMSK                                    (~(((1U<<SDH_CARD_REM_EN_LEN)-1)<<SDH_CARD_REM_EN_POS))
#define SDH_CARD_INT_EN                                         SDH_CARD_INT_EN
#define SDH_CARD_INT_EN_POS                                     (8U)
#define SDH_CARD_INT_EN_LEN                                     (1U)
#define SDH_CARD_INT_EN_MSK                                     (((1U<<SDH_CARD_INT_EN_LEN)-1)<<SDH_CARD_INT_EN_POS)
#define SDH_CARD_INT_EN_UMSK                                    (~(((1U<<SDH_CARD_INT_EN_LEN)-1)<<SDH_CARD_INT_EN_POS))
#define SDH_INT_A_INT_EN                                        SDH_INT_A_INT_EN
#define SDH_INT_A_INT_EN_POS                                    (9U)
#define SDH_INT_A_INT_EN_LEN                                    (1U)
#define SDH_INT_A_INT_EN_MSK                                    (((1U<<SDH_INT_A_INT_EN_LEN)-1)<<SDH_INT_A_INT_EN_POS)
#define SDH_INT_A_INT_EN_UMSK                                   (~(((1U<<SDH_INT_A_INT_EN_LEN)-1)<<SDH_INT_A_INT_EN_POS))
#define SDH_INT_B_INT_EN                                        SDH_INT_B_INT_EN
#define SDH_INT_B_INT_EN_POS                                    (10U)
#define SDH_INT_B_INT_EN_LEN                                    (1U)
#define SDH_INT_B_INT_EN_MSK                                    (((1U<<SDH_INT_B_INT_EN_LEN)-1)<<SDH_INT_B_INT_EN_POS)
#define SDH_INT_B_INT_EN_UMSK                                   (~(((1U<<SDH_INT_B_INT_EN_LEN)-1)<<SDH_INT_B_INT_EN_POS))
#define SDH_INT_C_INT_EN                                        SDH_INT_C_INT_EN
#define SDH_INT_C_INT_EN_POS                                    (11U)
#define SDH_INT_C_INT_EN_LEN                                    (1U)
#define SDH_INT_C_INT_EN_MSK                                    (((1U<<SDH_INT_C_INT_EN_LEN)-1)<<SDH_INT_C_INT_EN_POS)
#define SDH_INT_C_INT_EN_UMSK                                   (~(((1U<<SDH_INT_C_INT_EN_LEN)-1)<<SDH_INT_C_INT_EN_POS))
#define SDH_RETUNE_INT_EN                                       SDH_RETUNE_INT_EN
#define SDH_RETUNE_INT_EN_POS                                   (12U)
#define SDH_RETUNE_INT_EN_LEN                                   (1U)
#define SDH_RETUNE_INT_EN_MSK                                   (((1U<<SDH_RETUNE_INT_EN_LEN)-1)<<SDH_RETUNE_INT_EN_POS)
#define SDH_RETUNE_INT_EN_UMSK                                  (~(((1U<<SDH_RETUNE_INT_EN_LEN)-1)<<SDH_RETUNE_INT_EN_POS))
#define SDH_BOOT_ACK_COMPLETE_EN                                SDH_BOOT_ACK_COMPLETE_EN
#define SDH_BOOT_ACK_COMPLETE_EN_POS                            (13U)
#define SDH_BOOT_ACK_COMPLETE_EN_LEN                            (1U)
#define SDH_BOOT_ACK_COMPLETE_EN_MSK                            (((1U<<SDH_BOOT_ACK_COMPLETE_EN_LEN)-1)<<SDH_BOOT_ACK_COMPLETE_EN_POS)
#define SDH_BOOT_ACK_COMPLETE_EN_UMSK                           (~(((1U<<SDH_BOOT_ACK_COMPLETE_EN_LEN)-1)<<SDH_BOOT_ACK_COMPLETE_EN_POS))
#define SDH_BOOT_DONE_INT_STATUS_EN                             SDH_BOOT_DONE_INT_STATUS_EN
#define SDH_BOOT_DONE_INT_STATUS_EN_POS                         (14U)
#define SDH_BOOT_DONE_INT_STATUS_EN_LEN                         (1U)
#define SDH_BOOT_DONE_INT_STATUS_EN_MSK                         (((1U<<SDH_BOOT_DONE_INT_STATUS_EN_LEN)-1)<<SDH_BOOT_DONE_INT_STATUS_EN_POS)
#define SDH_BOOT_DONE_INT_STATUS_EN_UMSK                        (~(((1U<<SDH_BOOT_DONE_INT_STATUS_EN_LEN)-1)<<SDH_BOOT_DONE_INT_STATUS_EN_POS))
#define SDH_FIX_TO_ZERO                                         SDH_FIX_TO_ZERO
#define SDH_FIX_TO_ZERO_POS                                     (15U)
#define SDH_FIX_TO_ZERO_LEN                                     (1U)
#define SDH_FIX_TO_ZERO_MSK                                     (((1U<<SDH_FIX_TO_ZERO_LEN)-1)<<SDH_FIX_TO_ZERO_POS)
#define SDH_FIX_TO_ZERO_UMSK                                    (~(((1U<<SDH_FIX_TO_ZERO_LEN)-1)<<SDH_FIX_TO_ZERO_POS))

/* 0x36 : Error Interrupt Status Enable Register */
#define SDH_SD_ERROR_INT_STATUS_EN_OFFSET                       (0x36)
#define SDH_CMD_TIMEOUT_ERR_EN                                  SDH_CMD_TIMEOUT_ERR_EN
#define SDH_CMD_TIMEOUT_ERR_EN_POS                              (0U)
#define SDH_CMD_TIMEOUT_ERR_EN_LEN                              (1U)
#define SDH_CMD_TIMEOUT_ERR_EN_MSK                              (((1U<<SDH_CMD_TIMEOUT_ERR_EN_LEN)-1)<<SDH_CMD_TIMEOUT_ERR_EN_POS)
#define SDH_CMD_TIMEOUT_ERR_EN_UMSK                             (~(((1U<<SDH_CMD_TIMEOUT_ERR_EN_LEN)-1)<<SDH_CMD_TIMEOUT_ERR_EN_POS))
#define SDH_CMD_CRC_ERR_EN                                      SDH_CMD_CRC_ERR_EN
#define SDH_CMD_CRC_ERR_EN_POS                                  (1U)
#define SDH_CMD_CRC_ERR_EN_LEN                                  (1U)
#define SDH_CMD_CRC_ERR_EN_MSK                                  (((1U<<SDH_CMD_CRC_ERR_EN_LEN)-1)<<SDH_CMD_CRC_ERR_EN_POS)
#define SDH_CMD_CRC_ERR_EN_UMSK                                 (~(((1U<<SDH_CMD_CRC_ERR_EN_LEN)-1)<<SDH_CMD_CRC_ERR_EN_POS))
#define SDH_CMD_END_BIT_ERR_EN                                  SDH_CMD_END_BIT_ERR_EN
#define SDH_CMD_END_BIT_ERR_EN_POS                              (2U)
#define SDH_CMD_END_BIT_ERR_EN_LEN                              (1U)
#define SDH_CMD_END_BIT_ERR_EN_MSK                              (((1U<<SDH_CMD_END_BIT_ERR_EN_LEN)-1)<<SDH_CMD_END_BIT_ERR_EN_POS)
#define SDH_CMD_END_BIT_ERR_EN_UMSK                             (~(((1U<<SDH_CMD_END_BIT_ERR_EN_LEN)-1)<<SDH_CMD_END_BIT_ERR_EN_POS))
#define SDH_CMD_INDEX_ERR_EN                                    SDH_CMD_INDEX_ERR_EN
#define SDH_CMD_INDEX_ERR_EN_POS                                (3U)
#define SDH_CMD_INDEX_ERR_EN_LEN                                (1U)
#define SDH_CMD_INDEX_ERR_EN_MSK                                (((1U<<SDH_CMD_INDEX_ERR_EN_LEN)-1)<<SDH_CMD_INDEX_ERR_EN_POS)
#define SDH_CMD_INDEX_ERR_EN_UMSK                               (~(((1U<<SDH_CMD_INDEX_ERR_EN_LEN)-1)<<SDH_CMD_INDEX_ERR_EN_POS))
#define SDH_DATA_TIMEOUT_ERR_EN                                 SDH_DATA_TIMEOUT_ERR_EN
#define SDH_DATA_TIMEOUT_ERR_EN_POS                             (4U)
#define SDH_DATA_TIMEOUT_ERR_EN_LEN                             (1U)
#define SDH_DATA_TIMEOUT_ERR_EN_MSK                             (((1U<<SDH_DATA_TIMEOUT_ERR_EN_LEN)-1)<<SDH_DATA_TIMEOUT_ERR_EN_POS)
#define SDH_DATA_TIMEOUT_ERR_EN_UMSK                            (~(((1U<<SDH_DATA_TIMEOUT_ERR_EN_LEN)-1)<<SDH_DATA_TIMEOUT_ERR_EN_POS))
#define SDH_RD_DATA_CRC_ERR_EN                                  SDH_RD_DATA_CRC_ERR_EN
#define SDH_RD_DATA_CRC_ERR_EN_POS                              (5U)
#define SDH_RD_DATA_CRC_ERR_EN_LEN                              (1U)
#define SDH_RD_DATA_CRC_ERR_EN_MSK                              (((1U<<SDH_RD_DATA_CRC_ERR_EN_LEN)-1)<<SDH_RD_DATA_CRC_ERR_EN_POS)
#define SDH_RD_DATA_CRC_ERR_EN_UMSK                             (~(((1U<<SDH_RD_DATA_CRC_ERR_EN_LEN)-1)<<SDH_RD_DATA_CRC_ERR_EN_POS))
#define SDH_RD_DATA_END_BIT_ERR_EN                              SDH_RD_DATA_END_BIT_ERR_EN
#define SDH_RD_DATA_END_BIT_ERR_EN_POS                          (6U)
#define SDH_RD_DATA_END_BIT_ERR_EN_LEN                          (1U)
#define SDH_RD_DATA_END_BIT_ERR_EN_MSK                          (((1U<<SDH_RD_DATA_END_BIT_ERR_EN_LEN)-1)<<SDH_RD_DATA_END_BIT_ERR_EN_POS)
#define SDH_RD_DATA_END_BIT_ERR_EN_UMSK                         (~(((1U<<SDH_RD_DATA_END_BIT_ERR_EN_LEN)-1)<<SDH_RD_DATA_END_BIT_ERR_EN_POS))
#define SDH_CUR_LIM_ERR_EN                                      SDH_CUR_LIM_ERR_EN
#define SDH_CUR_LIM_ERR_EN_POS                                  (7U)
#define SDH_CUR_LIM_ERR_EN_LEN                                  (1U)
#define SDH_CUR_LIM_ERR_EN_MSK                                  (((1U<<SDH_CUR_LIM_ERR_EN_LEN)-1)<<SDH_CUR_LIM_ERR_EN_POS)
#define SDH_CUR_LIM_ERR_EN_UMSK                                 (~(((1U<<SDH_CUR_LIM_ERR_EN_LEN)-1)<<SDH_CUR_LIM_ERR_EN_POS))
#define SDH_AUTO_CMD12_ERR_EN                                   SDH_AUTO_CMD12_ERR_EN
#define SDH_AUTO_CMD12_ERR_EN_POS                               (8U)
#define SDH_AUTO_CMD12_ERR_EN_LEN                               (1U)
#define SDH_AUTO_CMD12_ERR_EN_MSK                               (((1U<<SDH_AUTO_CMD12_ERR_EN_LEN)-1)<<SDH_AUTO_CMD12_ERR_EN_POS)
#define SDH_AUTO_CMD12_ERR_EN_UMSK                              (~(((1U<<SDH_AUTO_CMD12_ERR_EN_LEN)-1)<<SDH_AUTO_CMD12_ERR_EN_POS))
#define SDH_ADMA_ERR_EN                                         SDH_ADMA_ERR_EN
#define SDH_ADMA_ERR_EN_POS                                     (9U)
#define SDH_ADMA_ERR_EN_LEN                                     (1U)
#define SDH_ADMA_ERR_EN_MSK                                     (((1U<<SDH_ADMA_ERR_EN_LEN)-1)<<SDH_ADMA_ERR_EN_POS)
#define SDH_ADMA_ERR_EN_UMSK                                    (~(((1U<<SDH_ADMA_ERR_EN_LEN)-1)<<SDH_ADMA_ERR_EN_POS))
#define SDH_TUNING_ERR_EN                                       SDH_TUNING_ERR_EN
#define SDH_TUNING_ERR_EN_POS                                   (10U)
#define SDH_TUNING_ERR_EN_LEN                                   (1U)
#define SDH_TUNING_ERR_EN_MSK                                   (((1U<<SDH_TUNING_ERR_EN_LEN)-1)<<SDH_TUNING_ERR_EN_POS)
#define SDH_TUNING_ERR_EN_UMSK                                  (~(((1U<<SDH_TUNING_ERR_EN_LEN)-1)<<SDH_TUNING_ERR_EN_POS))
#define SDH_VENDOR_SPECIFIC_ERR_EN                              SDH_VENDOR_SPECIFIC_ERR_EN
#define SDH_VENDOR_SPECIFIC_ERR_EN_POS                          (12U)
#define SDH_VENDOR_SPECIFIC_ERR_EN_LEN                          (4U)
#define SDH_VENDOR_SPECIFIC_ERR_EN_MSK                          (((1U<<SDH_VENDOR_SPECIFIC_ERR_EN_LEN)-1)<<SDH_VENDOR_SPECIFIC_ERR_EN_POS)
#define SDH_VENDOR_SPECIFIC_ERR_EN_UMSK                         (~(((1U<<SDH_VENDOR_SPECIFIC_ERR_EN_LEN)-1)<<SDH_VENDOR_SPECIFIC_ERR_EN_POS))

/* 0x38 : Normal Interrupt Status Interrupt Enable Register */
#define SDH_SD_NORMAL_INT_STATUS_INT_EN_OFFSET                  (0x38)
#define SDH_CMD_COMPLETE_INT_EN                                 SDH_CMD_COMPLETE_INT_EN
#define SDH_CMD_COMPLETE_INT_EN_POS                             (0U)
#define SDH_CMD_COMPLETE_INT_EN_LEN                             (1U)
#define SDH_CMD_COMPLETE_INT_EN_MSK                             (((1U<<SDH_CMD_COMPLETE_INT_EN_LEN)-1)<<SDH_CMD_COMPLETE_INT_EN_POS)
#define SDH_CMD_COMPLETE_INT_EN_UMSK                            (~(((1U<<SDH_CMD_COMPLETE_INT_EN_LEN)-1)<<SDH_CMD_COMPLETE_INT_EN_POS))
#define SDH_XFER_COMPLETE_INT_EN                                SDH_XFER_COMPLETE_INT_EN
#define SDH_XFER_COMPLETE_INT_EN_POS                            (1U)
#define SDH_XFER_COMPLETE_INT_EN_LEN                            (1U)
#define SDH_XFER_COMPLETE_INT_EN_MSK                            (((1U<<SDH_XFER_COMPLETE_INT_EN_LEN)-1)<<SDH_XFER_COMPLETE_INT_EN_POS)
#define SDH_XFER_COMPLETE_INT_EN_UMSK                           (~(((1U<<SDH_XFER_COMPLETE_INT_EN_LEN)-1)<<SDH_XFER_COMPLETE_INT_EN_POS))
#define SDH_BLOCK_GAP_EVT_INT_EN                                SDH_BLOCK_GAP_EVT_INT_EN
#define SDH_BLOCK_GAP_EVT_INT_EN_POS                            (2U)
#define SDH_BLOCK_GAP_EVT_INT_EN_LEN                            (1U)
#define SDH_BLOCK_GAP_EVT_INT_EN_MSK                            (((1U<<SDH_BLOCK_GAP_EVT_INT_EN_LEN)-1)<<SDH_BLOCK_GAP_EVT_INT_EN_POS)
#define SDH_BLOCK_GAP_EVT_INT_EN_UMSK                           (~(((1U<<SDH_BLOCK_GAP_EVT_INT_EN_LEN)-1)<<SDH_BLOCK_GAP_EVT_INT_EN_POS))
#define SDH_DMA_INT_INT_EN                                      SDH_DMA_INT_INT_EN
#define SDH_DMA_INT_INT_EN_POS                                  (3U)
#define SDH_DMA_INT_INT_EN_LEN                                  (1U)
#define SDH_DMA_INT_INT_EN_MSK                                  (((1U<<SDH_DMA_INT_INT_EN_LEN)-1)<<SDH_DMA_INT_INT_EN_POS)
#define SDH_DMA_INT_INT_EN_UMSK                                 (~(((1U<<SDH_DMA_INT_INT_EN_LEN)-1)<<SDH_DMA_INT_INT_EN_POS))
#define SDH_TX_RDY_INT_EN                                       SDH_TX_RDY_INT_EN
#define SDH_TX_RDY_INT_EN_POS                                   (4U)
#define SDH_TX_RDY_INT_EN_LEN                                   (1U)
#define SDH_TX_RDY_INT_EN_MSK                                   (((1U<<SDH_TX_RDY_INT_EN_LEN)-1)<<SDH_TX_RDY_INT_EN_POS)
#define SDH_TX_RDY_INT_EN_UMSK                                  (~(((1U<<SDH_TX_RDY_INT_EN_LEN)-1)<<SDH_TX_RDY_INT_EN_POS))
#define SDH_RX_RDY_INT_EN                                       SDH_RX_RDY_INT_EN
#define SDH_RX_RDY_INT_EN_POS                                   (5U)
#define SDH_RX_RDY_INT_EN_LEN                                   (1U)
#define SDH_RX_RDY_INT_EN_MSK                                   (((1U<<SDH_RX_RDY_INT_EN_LEN)-1)<<SDH_RX_RDY_INT_EN_POS)
#define SDH_RX_RDY_INT_EN_UMSK                                  (~(((1U<<SDH_RX_RDY_INT_EN_LEN)-1)<<SDH_RX_RDY_INT_EN_POS))
#define SDH_CARD_INS_INT_EN                                     SDH_CARD_INS_INT_EN
#define SDH_CARD_INS_INT_EN_POS                                 (6U)
#define SDH_CARD_INS_INT_EN_LEN                                 (1U)
#define SDH_CARD_INS_INT_EN_MSK                                 (((1U<<SDH_CARD_INS_INT_EN_LEN)-1)<<SDH_CARD_INS_INT_EN_POS)
#define SDH_CARD_INS_INT_EN_UMSK                                (~(((1U<<SDH_CARD_INS_INT_EN_LEN)-1)<<SDH_CARD_INS_INT_EN_POS))
#define SDH_CARD_REM_INT_EN                                     SDH_CARD_REM_INT_EN
#define SDH_CARD_REM_INT_EN_POS                                 (7U)
#define SDH_CARD_REM_INT_EN_LEN                                 (1U)
#define SDH_CARD_REM_INT_EN_MSK                                 (((1U<<SDH_CARD_REM_INT_EN_LEN)-1)<<SDH_CARD_REM_INT_EN_POS)
#define SDH_CARD_REM_INT_EN_UMSK                                (~(((1U<<SDH_CARD_REM_INT_EN_LEN)-1)<<SDH_CARD_REM_INT_EN_POS))
#define SDH_CARD_INT_INT_EN                                     SDH_CARD_INT_INT_EN
#define SDH_CARD_INT_INT_EN_POS                                 (8U)
#define SDH_CARD_INT_INT_EN_LEN                                 (1U)
#define SDH_CARD_INT_INT_EN_MSK                                 (((1U<<SDH_CARD_INT_INT_EN_LEN)-1)<<SDH_CARD_INT_INT_EN_POS)
#define SDH_CARD_INT_INT_EN_UMSK                                (~(((1U<<SDH_CARD_INT_INT_EN_LEN)-1)<<SDH_CARD_INT_INT_EN_POS))
#define SDH_INT_A_INT_INT_EN                                    SDH_INT_A_INT_INT_EN
#define SDH_INT_A_INT_INT_EN_POS                                (9U)
#define SDH_INT_A_INT_INT_EN_LEN                                (1U)
#define SDH_INT_A_INT_INT_EN_MSK                                (((1U<<SDH_INT_A_INT_INT_EN_LEN)-1)<<SDH_INT_A_INT_INT_EN_POS)
#define SDH_INT_A_INT_INT_EN_UMSK                               (~(((1U<<SDH_INT_A_INT_INT_EN_LEN)-1)<<SDH_INT_A_INT_INT_EN_POS))
#define SDH_INT_B_INT_INT_EN                                    SDH_INT_B_INT_INT_EN
#define SDH_INT_B_INT_INT_EN_POS                                (10U)
#define SDH_INT_B_INT_INT_EN_LEN                                (1U)
#define SDH_INT_B_INT_INT_EN_MSK                                (((1U<<SDH_INT_B_INT_INT_EN_LEN)-1)<<SDH_INT_B_INT_INT_EN_POS)
#define SDH_INT_B_INT_INT_EN_UMSK                               (~(((1U<<SDH_INT_B_INT_INT_EN_LEN)-1)<<SDH_INT_B_INT_INT_EN_POS))
#define SDH_INT_C_INT_INT_EN                                    SDH_INT_C_INT_INT_EN
#define SDH_INT_C_INT_INT_EN_POS                                (11U)
#define SDH_INT_C_INT_INT_EN_LEN                                (1U)
#define SDH_INT_C_INT_INT_EN_MSK                                (((1U<<SDH_INT_C_INT_INT_EN_LEN)-1)<<SDH_INT_C_INT_INT_EN_POS)
#define SDH_INT_C_INT_INT_EN_UMSK                               (~(((1U<<SDH_INT_C_INT_INT_EN_LEN)-1)<<SDH_INT_C_INT_INT_EN_POS))
#define SDH_RETUNE_INT_INT_EN                                   SDH_RETUNE_INT_INT_EN
#define SDH_RETUNE_INT_INT_EN_POS                               (12U)
#define SDH_RETUNE_INT_INT_EN_LEN                               (1U)
#define SDH_RETUNE_INT_INT_EN_MSK                               (((1U<<SDH_RETUNE_INT_INT_EN_LEN)-1)<<SDH_RETUNE_INT_INT_EN_POS)
#define SDH_RETUNE_INT_INT_EN_UMSK                              (~(((1U<<SDH_RETUNE_INT_INT_EN_LEN)-1)<<SDH_RETUNE_INT_INT_EN_POS))
#define SDH_BOOT_ACK_COMPLETE_EN                                SDH_BOOT_ACK_COMPLETE_EN
#define SDH_BOOT_ACK_COMPLETE_EN_POS                            (13U)
#define SDH_BOOT_ACK_COMPLETE_EN_LEN                            (1U)
#define SDH_BOOT_ACK_COMPLETE_EN_MSK                            (((1U<<SDH_BOOT_ACK_COMPLETE_EN_LEN)-1)<<SDH_BOOT_ACK_COMPLETE_EN_POS)
#define SDH_BOOT_ACK_COMPLETE_EN_UMSK                           (~(((1U<<SDH_BOOT_ACK_COMPLETE_EN_LEN)-1)<<SDH_BOOT_ACK_COMPLETE_EN_POS))
#define SDH_BOOT_DONE_INT_EN                                    SDH_BOOT_DONE_INT_EN
#define SDH_BOOT_DONE_INT_EN_POS                                (14U)
#define SDH_BOOT_DONE_INT_EN_LEN                                (1U)
#define SDH_BOOT_DONE_INT_EN_MSK                                (((1U<<SDH_BOOT_DONE_INT_EN_LEN)-1)<<SDH_BOOT_DONE_INT_EN_POS)
#define SDH_BOOT_DONE_INT_EN_UMSK                               (~(((1U<<SDH_BOOT_DONE_INT_EN_LEN)-1)<<SDH_BOOT_DONE_INT_EN_POS))
#define SDH_FIX_TO_ZERO                                         SDH_FIX_TO_ZERO
#define SDH_FIX_TO_ZERO_POS                                     (15U)
#define SDH_FIX_TO_ZERO_LEN                                     (1U)
#define SDH_FIX_TO_ZERO_MSK                                     (((1U<<SDH_FIX_TO_ZERO_LEN)-1)<<SDH_FIX_TO_ZERO_POS)
#define SDH_FIX_TO_ZERO_UMSK                                    (~(((1U<<SDH_FIX_TO_ZERO_LEN)-1)<<SDH_FIX_TO_ZERO_POS))

/* 0x3A : Error Interrupt Status Interrupt Enable Register */
#define SDH_SD_ERROR_INT_STATUS_INT_EN_OFFSET                   (0x3A)
#define SDH_CMD_TIMEOUT_ERR_INT_EN                              SDH_CMD_TIMEOUT_ERR_INT_EN
#define SDH_CMD_TIMEOUT_ERR_INT_EN_POS                          (0U)
#define SDH_CMD_TIMEOUT_ERR_INT_EN_LEN                          (1U)
#define SDH_CMD_TIMEOUT_ERR_INT_EN_MSK                          (((1U<<SDH_CMD_TIMEOUT_ERR_INT_EN_LEN)-1)<<SDH_CMD_TIMEOUT_ERR_INT_EN_POS)
#define SDH_CMD_TIMEOUT_ERR_INT_EN_UMSK                         (~(((1U<<SDH_CMD_TIMEOUT_ERR_INT_EN_LEN)-1)<<SDH_CMD_TIMEOUT_ERR_INT_EN_POS))
#define SDH_CMD_CRC_ERR_INT_EN                                  SDH_CMD_CRC_ERR_INT_EN
#define SDH_CMD_CRC_ERR_INT_EN_POS                              (1U)
#define SDH_CMD_CRC_ERR_INT_EN_LEN                              (1U)
#define SDH_CMD_CRC_ERR_INT_EN_MSK                              (((1U<<SDH_CMD_CRC_ERR_INT_EN_LEN)-1)<<SDH_CMD_CRC_ERR_INT_EN_POS)
#define SDH_CMD_CRC_ERR_INT_EN_UMSK                             (~(((1U<<SDH_CMD_CRC_ERR_INT_EN_LEN)-1)<<SDH_CMD_CRC_ERR_INT_EN_POS))
#define SDH_CMD_END_BIT_ERR_INT_EN                              SDH_CMD_END_BIT_ERR_INT_EN
#define SDH_CMD_END_BIT_ERR_INT_EN_POS                          (2U)
#define SDH_CMD_END_BIT_ERR_INT_EN_LEN                          (1U)
#define SDH_CMD_END_BIT_ERR_INT_EN_MSK                          (((1U<<SDH_CMD_END_BIT_ERR_INT_EN_LEN)-1)<<SDH_CMD_END_BIT_ERR_INT_EN_POS)
#define SDH_CMD_END_BIT_ERR_INT_EN_UMSK                         (~(((1U<<SDH_CMD_END_BIT_ERR_INT_EN_LEN)-1)<<SDH_CMD_END_BIT_ERR_INT_EN_POS))
#define SDH_CMD_INDEX_ERR_INT_EN                                SDH_CMD_INDEX_ERR_INT_EN
#define SDH_CMD_INDEX_ERR_INT_EN_POS                            (3U)
#define SDH_CMD_INDEX_ERR_INT_EN_LEN                            (1U)
#define SDH_CMD_INDEX_ERR_INT_EN_MSK                            (((1U<<SDH_CMD_INDEX_ERR_INT_EN_LEN)-1)<<SDH_CMD_INDEX_ERR_INT_EN_POS)
#define SDH_CMD_INDEX_ERR_INT_EN_UMSK                           (~(((1U<<SDH_CMD_INDEX_ERR_INT_EN_LEN)-1)<<SDH_CMD_INDEX_ERR_INT_EN_POS))
#define SDH_DATA_TIMEOUT_ERR_INT_EN                             SDH_DATA_TIMEOUT_ERR_INT_EN
#define SDH_DATA_TIMEOUT_ERR_INT_EN_POS                         (4U)
#define SDH_DATA_TIMEOUT_ERR_INT_EN_LEN                         (1U)
#define SDH_DATA_TIMEOUT_ERR_INT_EN_MSK                         (((1U<<SDH_DATA_TIMEOUT_ERR_INT_EN_LEN)-1)<<SDH_DATA_TIMEOUT_ERR_INT_EN_POS)
#define SDH_DATA_TIMEOUT_ERR_INT_EN_UMSK                        (~(((1U<<SDH_DATA_TIMEOUT_ERR_INT_EN_LEN)-1)<<SDH_DATA_TIMEOUT_ERR_INT_EN_POS))
#define SDH_RD_DATA_CRC_ERR_INT_EN                              SDH_RD_DATA_CRC_ERR_INT_EN
#define SDH_RD_DATA_CRC_ERR_INT_EN_POS                          (5U)
#define SDH_RD_DATA_CRC_ERR_INT_EN_LEN                          (1U)
#define SDH_RD_DATA_CRC_ERR_INT_EN_MSK                          (((1U<<SDH_RD_DATA_CRC_ERR_INT_EN_LEN)-1)<<SDH_RD_DATA_CRC_ERR_INT_EN_POS)
#define SDH_RD_DATA_CRC_ERR_INT_EN_UMSK                         (~(((1U<<SDH_RD_DATA_CRC_ERR_INT_EN_LEN)-1)<<SDH_RD_DATA_CRC_ERR_INT_EN_POS))
#define SDH_RD_DATA_END_BIT_ERR_INT_EN                          SDH_RD_DATA_END_BIT_ERR_INT_EN
#define SDH_RD_DATA_END_BIT_ERR_INT_EN_POS                      (6U)
#define SDH_RD_DATA_END_BIT_ERR_INT_EN_LEN                      (1U)
#define SDH_RD_DATA_END_BIT_ERR_INT_EN_MSK                      (((1U<<SDH_RD_DATA_END_BIT_ERR_INT_EN_LEN)-1)<<SDH_RD_DATA_END_BIT_ERR_INT_EN_POS)
#define SDH_RD_DATA_END_BIT_ERR_INT_EN_UMSK                     (~(((1U<<SDH_RD_DATA_END_BIT_ERR_INT_EN_LEN)-1)<<SDH_RD_DATA_END_BIT_ERR_INT_EN_POS))
#define SDH_CUR_LIM_ERR_INT_EN                                  SDH_CUR_LIM_ERR_INT_EN
#define SDH_CUR_LIM_ERR_INT_EN_POS                              (7U)
#define SDH_CUR_LIM_ERR_INT_EN_LEN                              (1U)
#define SDH_CUR_LIM_ERR_INT_EN_MSK                              (((1U<<SDH_CUR_LIM_ERR_INT_EN_LEN)-1)<<SDH_CUR_LIM_ERR_INT_EN_POS)
#define SDH_CUR_LIM_ERR_INT_EN_UMSK                             (~(((1U<<SDH_CUR_LIM_ERR_INT_EN_LEN)-1)<<SDH_CUR_LIM_ERR_INT_EN_POS))
#define SDH_AUTO_CMD12_ERR_INT_EN                               SDH_AUTO_CMD12_ERR_INT_EN
#define SDH_AUTO_CMD12_ERR_INT_EN_POS                           (8U)
#define SDH_AUTO_CMD12_ERR_INT_EN_LEN                           (1U)
#define SDH_AUTO_CMD12_ERR_INT_EN_MSK                           (((1U<<SDH_AUTO_CMD12_ERR_INT_EN_LEN)-1)<<SDH_AUTO_CMD12_ERR_INT_EN_POS)
#define SDH_AUTO_CMD12_ERR_INT_EN_UMSK                          (~(((1U<<SDH_AUTO_CMD12_ERR_INT_EN_LEN)-1)<<SDH_AUTO_CMD12_ERR_INT_EN_POS))
#define SDH_ADMA_ERR_INT_EN                                     SDH_ADMA_ERR_INT_EN
#define SDH_ADMA_ERR_INT_EN_POS                                 (9U)
#define SDH_ADMA_ERR_INT_EN_LEN                                 (1U)
#define SDH_ADMA_ERR_INT_EN_MSK                                 (((1U<<SDH_ADMA_ERR_INT_EN_LEN)-1)<<SDH_ADMA_ERR_INT_EN_POS)
#define SDH_ADMA_ERR_INT_EN_UMSK                                (~(((1U<<SDH_ADMA_ERR_INT_EN_LEN)-1)<<SDH_ADMA_ERR_INT_EN_POS))
#define SDH_TUNE_ERR_INT_EN                                     SDH_TUNE_ERR_INT_EN
#define SDH_TUNE_ERR_INT_EN_POS                                 (10U)
#define SDH_TUNE_ERR_INT_EN_LEN                                 (1U)
#define SDH_TUNE_ERR_INT_EN_MSK                                 (((1U<<SDH_TUNE_ERR_INT_EN_LEN)-1)<<SDH_TUNE_ERR_INT_EN_POS)
#define SDH_TUNE_ERR_INT_EN_UMSK                                (~(((1U<<SDH_TUNE_ERR_INT_EN_LEN)-1)<<SDH_TUNE_ERR_INT_EN_POS))
#define SDH_VENDOR_SPECIFIC_ERR_INT_EN                          SDH_VENDOR_SPECIFIC_ERR_INT_EN
#define SDH_VENDOR_SPECIFIC_ERR_INT_EN_POS                      (12U)
#define SDH_VENDOR_SPECIFIC_ERR_INT_EN_LEN                      (4U)
#define SDH_VENDOR_SPECIFIC_ERR_INT_EN_MSK                      (((1U<<SDH_VENDOR_SPECIFIC_ERR_INT_EN_LEN)-1)<<SDH_VENDOR_SPECIFIC_ERR_INT_EN_POS)
#define SDH_VENDOR_SPECIFIC_ERR_INT_EN_UMSK                     (~(((1U<<SDH_VENDOR_SPECIFIC_ERR_INT_EN_LEN)-1)<<SDH_VENDOR_SPECIFIC_ERR_INT_EN_POS))

/* 0x3C : Auto CMD12 Error Status */
#define SDH_SD_AUTO_CMD12_ERROR_STATUS_OFFSET                   (0x3C)
#define SDH_AUTO_CMD12_NOT_EXE                                  SDH_AUTO_CMD12_NOT_EXE
#define SDH_AUTO_CMD12_NOT_EXE_POS                              (0U)
#define SDH_AUTO_CMD12_NOT_EXE_LEN                              (1U)
#define SDH_AUTO_CMD12_NOT_EXE_MSK                              (((1U<<SDH_AUTO_CMD12_NOT_EXE_LEN)-1)<<SDH_AUTO_CMD12_NOT_EXE_POS)
#define SDH_AUTO_CMD12_NOT_EXE_UMSK                             (~(((1U<<SDH_AUTO_CMD12_NOT_EXE_LEN)-1)<<SDH_AUTO_CMD12_NOT_EXE_POS))
#define SDH_AUTO_CMD_TIMEOUT_ERR                                SDH_AUTO_CMD_TIMEOUT_ERR
#define SDH_AUTO_CMD_TIMEOUT_ERR_POS                            (1U)
#define SDH_AUTO_CMD_TIMEOUT_ERR_LEN                            (1U)
#define SDH_AUTO_CMD_TIMEOUT_ERR_MSK                            (((1U<<SDH_AUTO_CMD_TIMEOUT_ERR_LEN)-1)<<SDH_AUTO_CMD_TIMEOUT_ERR_POS)
#define SDH_AUTO_CMD_TIMEOUT_ERR_UMSK                           (~(((1U<<SDH_AUTO_CMD_TIMEOUT_ERR_LEN)-1)<<SDH_AUTO_CMD_TIMEOUT_ERR_POS))
#define SDH_AUTO_CMD_CRC_ERR                                    SDH_AUTO_CMD_CRC_ERR
#define SDH_AUTO_CMD_CRC_ERR_POS                                (2U)
#define SDH_AUTO_CMD_CRC_ERR_LEN                                (1U)
#define SDH_AUTO_CMD_CRC_ERR_MSK                                (((1U<<SDH_AUTO_CMD_CRC_ERR_LEN)-1)<<SDH_AUTO_CMD_CRC_ERR_POS)
#define SDH_AUTO_CMD_CRC_ERR_UMSK                               (~(((1U<<SDH_AUTO_CMD_CRC_ERR_LEN)-1)<<SDH_AUTO_CMD_CRC_ERR_POS))
#define SDH_AUTO_CMD_END_BIT_ERR                                SDH_AUTO_CMD_END_BIT_ERR
#define SDH_AUTO_CMD_END_BIT_ERR_POS                            (3U)
#define SDH_AUTO_CMD_END_BIT_ERR_LEN                            (1U)
#define SDH_AUTO_CMD_END_BIT_ERR_MSK                            (((1U<<SDH_AUTO_CMD_END_BIT_ERR_LEN)-1)<<SDH_AUTO_CMD_END_BIT_ERR_POS)
#define SDH_AUTO_CMD_END_BIT_ERR_UMSK                           (~(((1U<<SDH_AUTO_CMD_END_BIT_ERR_LEN)-1)<<SDH_AUTO_CMD_END_BIT_ERR_POS))
#define SDH_AUTO_CMD_INDEX_ERR                                  SDH_AUTO_CMD_INDEX_ERR
#define SDH_AUTO_CMD_INDEX_ERR_POS                              (4U)
#define SDH_AUTO_CMD_INDEX_ERR_LEN                              (1U)
#define SDH_AUTO_CMD_INDEX_ERR_MSK                              (((1U<<SDH_AUTO_CMD_INDEX_ERR_LEN)-1)<<SDH_AUTO_CMD_INDEX_ERR_POS)
#define SDH_AUTO_CMD_INDEX_ERR_UMSK                             (~(((1U<<SDH_AUTO_CMD_INDEX_ERR_LEN)-1)<<SDH_AUTO_CMD_INDEX_ERR_POS))
#define SDH_CMD_NOT_ISSUED                                      SDH_CMD_NOT_ISSUED
#define SDH_CMD_NOT_ISSUED_POS                                  (7U)
#define SDH_CMD_NOT_ISSUED_LEN                                  (1U)
#define SDH_CMD_NOT_ISSUED_MSK                                  (((1U<<SDH_CMD_NOT_ISSUED_LEN)-1)<<SDH_CMD_NOT_ISSUED_POS)
#define SDH_CMD_NOT_ISSUED_UMSK                                 (~(((1U<<SDH_CMD_NOT_ISSUED_LEN)-1)<<SDH_CMD_NOT_ISSUED_POS))

/* 0x3E : Host Control 2 */
#define SDH_HOST_CTRL_2_OFFSET                                  (0x3E)
#define SDH_UHS_MODE_SEL                                        SDH_UHS_MODE_SEL
#define SDH_UHS_MODE_SEL_POS                                    (0U)
#define SDH_UHS_MODE_SEL_LEN                                    (3U)
#define SDH_UHS_MODE_SEL_MSK                                    (((1U<<SDH_UHS_MODE_SEL_LEN)-1)<<SDH_UHS_MODE_SEL_POS)
#define SDH_UHS_MODE_SEL_UMSK                                   (~(((1U<<SDH_UHS_MODE_SEL_LEN)-1)<<SDH_UHS_MODE_SEL_POS))
#define SDH_SDH_V18_EN                                          SDH_SDH_V18_EN
#define SDH_SDH_V18_EN_POS                                      (3U)
#define SDH_SDH_V18_EN_LEN                                      (1U)
#define SDH_SDH_V18_EN_MSK                                      (((1U<<SDH_SDH_V18_EN_LEN)-1)<<SDH_SDH_V18_EN_POS)
#define SDH_SDH_V18_EN_UMSK                                     (~(((1U<<SDH_SDH_V18_EN_LEN)-1)<<SDH_SDH_V18_EN_POS))
#define SDH_DRV_STRENGTH_SEL                                    SDH_DRV_STRENGTH_SEL
#define SDH_DRV_STRENGTH_SEL_POS                                (4U)
#define SDH_DRV_STRENGTH_SEL_LEN                                (2U)
#define SDH_DRV_STRENGTH_SEL_MSK                                (((1U<<SDH_DRV_STRENGTH_SEL_LEN)-1)<<SDH_DRV_STRENGTH_SEL_POS)
#define SDH_DRV_STRENGTH_SEL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_SEL_LEN)-1)<<SDH_DRV_STRENGTH_SEL_POS))
#define SDH_EXE_TUNING                                          SDH_EXE_TUNING
#define SDH_EXE_TUNING_POS                                      (6U)
#define SDH_EXE_TUNING_LEN                                      (1U)
#define SDH_EXE_TUNING_MSK                                      (((1U<<SDH_EXE_TUNING_LEN)-1)<<SDH_EXE_TUNING_POS)
#define SDH_EXE_TUNING_UMSK                                     (~(((1U<<SDH_EXE_TUNING_LEN)-1)<<SDH_EXE_TUNING_POS))
#define SDH_SAMPLING_CLK_SEL                                    SDH_SAMPLING_CLK_SEL
#define SDH_SAMPLING_CLK_SEL_POS                                (7U)
#define SDH_SAMPLING_CLK_SEL_LEN                                (1U)
#define SDH_SAMPLING_CLK_SEL_MSK                                (((1U<<SDH_SAMPLING_CLK_SEL_LEN)-1)<<SDH_SAMPLING_CLK_SEL_POS)
#define SDH_SAMPLING_CLK_SEL_UMSK                               (~(((1U<<SDH_SAMPLING_CLK_SEL_LEN)-1)<<SDH_SAMPLING_CLK_SEL_POS))
#define SDH_CONSECUTIVE_SAMPLING                                SDH_CONSECUTIVE_SAMPLING
#define SDH_CONSECUTIVE_SAMPLING_POS                            (8U)
#define SDH_CONSECUTIVE_SAMPLING_LEN                            (6U)
#define SDH_CONSECUTIVE_SAMPLING_MSK                            (((1U<<SDH_CONSECUTIVE_SAMPLING_LEN)-1)<<SDH_CONSECUTIVE_SAMPLING_POS)
#define SDH_CONSECUTIVE_SAMPLING_UMSK                           (~(((1U<<SDH_CONSECUTIVE_SAMPLING_LEN)-1)<<SDH_CONSECUTIVE_SAMPLING_POS))
#define SDH_ASYNC_INT_EN                                        SDH_ASYNC_INT_EN
#define SDH_ASYNC_INT_EN_POS                                    (14U)
#define SDH_ASYNC_INT_EN_LEN                                    (1U)
#define SDH_ASYNC_INT_EN_MSK                                    (((1U<<SDH_ASYNC_INT_EN_LEN)-1)<<SDH_ASYNC_INT_EN_POS)
#define SDH_ASYNC_INT_EN_UMSK                                   (~(((1U<<SDH_ASYNC_INT_EN_LEN)-1)<<SDH_ASYNC_INT_EN_POS))
#define SDH_PRE_VAL_EN                                          SDH_PRE_VAL_EN
#define SDH_PRE_VAL_EN_POS                                      (15U)
#define SDH_PRE_VAL_EN_LEN                                      (1U)
#define SDH_PRE_VAL_EN_MSK                                      (((1U<<SDH_PRE_VAL_EN_LEN)-1)<<SDH_PRE_VAL_EN_POS)
#define SDH_PRE_VAL_EN_UMSK                                     (~(((1U<<SDH_PRE_VAL_EN_LEN)-1)<<SDH_PRE_VAL_EN_POS))

/* 0x40 : Capabilities Register 1 */
#define SDH_SD_CAPABILITIES_1_OFFSET                            (0x40)
#define SDH_TIMEOUT_FREQ                                        SDH_TIMEOUT_FREQ
#define SDH_TIMEOUT_FREQ_POS                                    (0U)
#define SDH_TIMEOUT_FREQ_LEN                                    (6U)
#define SDH_TIMEOUT_FREQ_MSK                                    (((1U<<SDH_TIMEOUT_FREQ_LEN)-1)<<SDH_TIMEOUT_FREQ_POS)
#define SDH_TIMEOUT_FREQ_UMSK                                   (~(((1U<<SDH_TIMEOUT_FREQ_LEN)-1)<<SDH_TIMEOUT_FREQ_POS))
#define SDH_TIMEOUT_UNIT                                        SDH_TIMEOUT_UNIT
#define SDH_TIMEOUT_UNIT_POS                                    (7U)
#define SDH_TIMEOUT_UNIT_LEN                                    (1U)
#define SDH_TIMEOUT_UNIT_MSK                                    (((1U<<SDH_TIMEOUT_UNIT_LEN)-1)<<SDH_TIMEOUT_UNIT_POS)
#define SDH_TIMEOUT_UNIT_UMSK                                   (~(((1U<<SDH_TIMEOUT_UNIT_LEN)-1)<<SDH_TIMEOUT_UNIT_POS))
#define SDH_BASE_FREQ                                           SDH_BASE_FREQ
#define SDH_BASE_FREQ_POS                                       (8U)
#define SDH_BASE_FREQ_LEN                                       (8U)
#define SDH_BASE_FREQ_MSK                                       (((1U<<SDH_BASE_FREQ_LEN)-1)<<SDH_BASE_FREQ_POS)
#define SDH_BASE_FREQ_UMSK                                      (~(((1U<<SDH_BASE_FREQ_LEN)-1)<<SDH_BASE_FREQ_POS))

/* 0x42 : Capabilities Register 2 */
#define SDH_SD_CAPABILITIES_2_OFFSET                            (0x42)
#define SDH_MAX_BLK_LEN                                         SDH_MAX_BLK_LEN
#define SDH_MAX_BLK_LEN_POS                                     (0U)
#define SDH_MAX_BLK_LEN_LEN                                     (2U)
#define SDH_MAX_BLK_LEN_MSK                                     (((1U<<SDH_MAX_BLK_LEN_LEN)-1)<<SDH_MAX_BLK_LEN_POS)
#define SDH_MAX_BLK_LEN_UMSK                                    (~(((1U<<SDH_MAX_BLK_LEN_LEN)-1)<<SDH_MAX_BLK_LEN_POS))
#define SDH_EMBEDDED_DEVICE_SUPPORT                             SDH_EMBEDDED_DEVICE_SUPPORT
#define SDH_EMBEDDED_DEVICE_SUPPORT_POS                         (2U)
#define SDH_EMBEDDED_DEVICE_SUPPORT_LEN                         (1U)
#define SDH_EMBEDDED_DEVICE_SUPPORT_MSK                         (((1U<<SDH_EMBEDDED_DEVICE_SUPPORT_LEN)-1)<<SDH_EMBEDDED_DEVICE_SUPPORT_POS)
#define SDH_EMBEDDED_DEVICE_SUPPORT_UMSK                        (~(((1U<<SDH_EMBEDDED_DEVICE_SUPPORT_LEN)-1)<<SDH_EMBEDDED_DEVICE_SUPPORT_POS))
#define SDH_ADMA2_SUPPORT                                       SDH_ADMA2_SUPPORT
#define SDH_ADMA2_SUPPORT_POS                                   (3U)
#define SDH_ADMA2_SUPPORT_LEN                                   (1U)
#define SDH_ADMA2_SUPPORT_MSK                                   (((1U<<SDH_ADMA2_SUPPORT_LEN)-1)<<SDH_ADMA2_SUPPORT_POS)
#define SDH_ADMA2_SUPPORT_UMSK                                  (~(((1U<<SDH_ADMA2_SUPPORT_LEN)-1)<<SDH_ADMA2_SUPPORT_POS))
#define SDH_HI_SPEED_SUPPORT                                    SDH_HI_SPEED_SUPPORT
#define SDH_HI_SPEED_SUPPORT_POS                                (5U)
#define SDH_HI_SPEED_SUPPORT_LEN                                (1U)
#define SDH_HI_SPEED_SUPPORT_MSK                                (((1U<<SDH_HI_SPEED_SUPPORT_LEN)-1)<<SDH_HI_SPEED_SUPPORT_POS)
#define SDH_HI_SPEED_SUPPORT_UMSK                               (~(((1U<<SDH_HI_SPEED_SUPPORT_LEN)-1)<<SDH_HI_SPEED_SUPPORT_POS))
#define SDH_SDMA_SUPPORT                                        SDH_SDMA_SUPPORT
#define SDH_SDMA_SUPPORT_POS                                    (6U)
#define SDH_SDMA_SUPPORT_LEN                                    (1U)
#define SDH_SDMA_SUPPORT_MSK                                    (((1U<<SDH_SDMA_SUPPORT_LEN)-1)<<SDH_SDMA_SUPPORT_POS)
#define SDH_SDMA_SUPPORT_UMSK                                   (~(((1U<<SDH_SDMA_SUPPORT_LEN)-1)<<SDH_SDMA_SUPPORT_POS))
#define SDH_SUS_RES_SUPPORT                                     SDH_SUS_RES_SUPPORT
#define SDH_SUS_RES_SUPPORT_POS                                 (7U)
#define SDH_SUS_RES_SUPPORT_LEN                                 (1U)
#define SDH_SUS_RES_SUPPORT_MSK                                 (((1U<<SDH_SUS_RES_SUPPORT_LEN)-1)<<SDH_SUS_RES_SUPPORT_POS)
#define SDH_SUS_RES_SUPPORT_UMSK                                (~(((1U<<SDH_SUS_RES_SUPPORT_LEN)-1)<<SDH_SUS_RES_SUPPORT_POS))
#define SDH_VLG_33_SUPPORT                                      SDH_VLG_33_SUPPORT
#define SDH_VLG_33_SUPPORT_POS                                  (8U)
#define SDH_VLG_33_SUPPORT_LEN                                  (1U)
#define SDH_VLG_33_SUPPORT_MSK                                  (((1U<<SDH_VLG_33_SUPPORT_LEN)-1)<<SDH_VLG_33_SUPPORT_POS)
#define SDH_VLG_33_SUPPORT_UMSK                                 (~(((1U<<SDH_VLG_33_SUPPORT_LEN)-1)<<SDH_VLG_33_SUPPORT_POS))
#define SDH_VLG_30_SUPPORT                                      SDH_VLG_30_SUPPORT
#define SDH_VLG_30_SUPPORT_POS                                  (9U)
#define SDH_VLG_30_SUPPORT_LEN                                  (1U)
#define SDH_VLG_30_SUPPORT_MSK                                  (((1U<<SDH_VLG_30_SUPPORT_LEN)-1)<<SDH_VLG_30_SUPPORT_POS)
#define SDH_VLG_30_SUPPORT_UMSK                                 (~(((1U<<SDH_VLG_30_SUPPORT_LEN)-1)<<SDH_VLG_30_SUPPORT_POS))
#define SDH_VLG_18_SUPPORT                                      SDH_VLG_18_SUPPORT
#define SDH_VLG_18_SUPPORT_POS                                  (10U)
#define SDH_VLG_18_SUPPORT_LEN                                  (1U)
#define SDH_VLG_18_SUPPORT_MSK                                  (((1U<<SDH_VLG_18_SUPPORT_LEN)-1)<<SDH_VLG_18_SUPPORT_POS)
#define SDH_VLG_18_SUPPORT_UMSK                                 (~(((1U<<SDH_VLG_18_SUPPORT_LEN)-1)<<SDH_VLG_18_SUPPORT_POS))
#define SDH_VLG_12_SUPPORT                                      SDH_VLG_12_SUPPORT
#define SDH_VLG_12_SUPPORT_POS                                  (11U)
#define SDH_VLG_12_SUPPORT_LEN                                  (1U)
#define SDH_VLG_12_SUPPORT_MSK                                  (((1U<<SDH_VLG_12_SUPPORT_LEN)-1)<<SDH_VLG_12_SUPPORT_POS)
#define SDH_VLG_12_SUPPORT_UMSK                                 (~(((1U<<SDH_VLG_12_SUPPORT_LEN)-1)<<SDH_VLG_12_SUPPORT_POS))
#define SDH_SYS_BUS_64_SUPPORT                                  SDH_SYS_BUS_64_SUPPORT
#define SDH_SYS_BUS_64_SUPPORT_POS                              (12U)
#define SDH_SYS_BUS_64_SUPPORT_LEN                              (1U)
#define SDH_SYS_BUS_64_SUPPORT_MSK                              (((1U<<SDH_SYS_BUS_64_SUPPORT_LEN)-1)<<SDH_SYS_BUS_64_SUPPORT_POS)
#define SDH_SYS_BUS_64_SUPPORT_UMSK                             (~(((1U<<SDH_SYS_BUS_64_SUPPORT_LEN)-1)<<SDH_SYS_BUS_64_SUPPORT_POS))
#define SDH_ASYNC_INT_SUPPORT                                   SDH_ASYNC_INT_SUPPORT
#define SDH_ASYNC_INT_SUPPORT_POS                               (13U)
#define SDH_ASYNC_INT_SUPPORT_LEN                               (1U)
#define SDH_ASYNC_INT_SUPPORT_MSK                               (((1U<<SDH_ASYNC_INT_SUPPORT_LEN)-1)<<SDH_ASYNC_INT_SUPPORT_POS)
#define SDH_ASYNC_INT_SUPPORT_UMSK                              (~(((1U<<SDH_ASYNC_INT_SUPPORT_LEN)-1)<<SDH_ASYNC_INT_SUPPORT_POS))
#define SDH_CFG_SLOT_TYPE                                       SDH_CFG_SLOT_TYPE
#define SDH_CFG_SLOT_TYPE_POS                                   (14U)
#define SDH_CFG_SLOT_TYPE_LEN                                   (2U)
#define SDH_CFG_SLOT_TYPE_MSK                                   (((1U<<SDH_CFG_SLOT_TYPE_LEN)-1)<<SDH_CFG_SLOT_TYPE_POS)
#define SDH_CFG_SLOT_TYPE_UMSK                                  (~(((1U<<SDH_CFG_SLOT_TYPE_LEN)-1)<<SDH_CFG_SLOT_TYPE_POS))

/* 0x44 : Capabilities Register 3 */
#define SDH_SD_CAPABILITIES_3_OFFSET                            (0x44)
#define SDH_SDR50_SUPPORT                                       SDH_SDR50_SUPPORT
#define SDH_SDR50_SUPPORT_POS                                   (0U)
#define SDH_SDR50_SUPPORT_LEN                                   (1U)
#define SDH_SDR50_SUPPORT_MSK                                   (((1U<<SDH_SDR50_SUPPORT_LEN)-1)<<SDH_SDR50_SUPPORT_POS)
#define SDH_SDR50_SUPPORT_UMSK                                  (~(((1U<<SDH_SDR50_SUPPORT_LEN)-1)<<SDH_SDR50_SUPPORT_POS))
#define SDH_SDR104_SUPPORT                                      SDH_SDR104_SUPPORT
#define SDH_SDR104_SUPPORT_POS                                  (1U)
#define SDH_SDR104_SUPPORT_LEN                                  (1U)
#define SDH_SDR104_SUPPORT_MSK                                  (((1U<<SDH_SDR104_SUPPORT_LEN)-1)<<SDH_SDR104_SUPPORT_POS)
#define SDH_SDR104_SUPPORT_UMSK                                 (~(((1U<<SDH_SDR104_SUPPORT_LEN)-1)<<SDH_SDR104_SUPPORT_POS))
#define SDH_DDR50_SUPPORT                                       SDH_DDR50_SUPPORT
#define SDH_DDR50_SUPPORT_POS                                   (2U)
#define SDH_DDR50_SUPPORT_LEN                                   (1U)
#define SDH_DDR50_SUPPORT_MSK                                   (((1U<<SDH_DDR50_SUPPORT_LEN)-1)<<SDH_DDR50_SUPPORT_POS)
#define SDH_DDR50_SUPPORT_UMSK                                  (~(((1U<<SDH_DDR50_SUPPORT_LEN)-1)<<SDH_DDR50_SUPPORT_POS))
#define SDH_HS200_SUPPORT                                       SDH_HS200_SUPPORT
#define SDH_HS200_SUPPORT_POS                                   (3U)
#define SDH_HS200_SUPPORT_LEN                                   (1U)
#define SDH_HS200_SUPPORT_MSK                                   (((1U<<SDH_HS200_SUPPORT_LEN)-1)<<SDH_HS200_SUPPORT_POS)
#define SDH_HS200_SUPPORT_UMSK                                  (~(((1U<<SDH_HS200_SUPPORT_LEN)-1)<<SDH_HS200_SUPPORT_POS))
#define SDH_DRV_TYPE_A                                          SDH_DRV_TYPE_A
#define SDH_DRV_TYPE_A_POS                                      (4U)
#define SDH_DRV_TYPE_A_LEN                                      (1U)
#define SDH_DRV_TYPE_A_MSK                                      (((1U<<SDH_DRV_TYPE_A_LEN)-1)<<SDH_DRV_TYPE_A_POS)
#define SDH_DRV_TYPE_A_UMSK                                     (~(((1U<<SDH_DRV_TYPE_A_LEN)-1)<<SDH_DRV_TYPE_A_POS))
#define SDH_DRV_TYPE_C                                          SDH_DRV_TYPE_C
#define SDH_DRV_TYPE_C_POS                                      (5U)
#define SDH_DRV_TYPE_C_LEN                                      (1U)
#define SDH_DRV_TYPE_C_MSK                                      (((1U<<SDH_DRV_TYPE_C_LEN)-1)<<SDH_DRV_TYPE_C_POS)
#define SDH_DRV_TYPE_C_UMSK                                     (~(((1U<<SDH_DRV_TYPE_C_LEN)-1)<<SDH_DRV_TYPE_C_POS))
#define SDH_DRV_TYPE_D                                          SDH_DRV_TYPE_D
#define SDH_DRV_TYPE_D_POS                                      (6U)
#define SDH_DRV_TYPE_D_LEN                                      (1U)
#define SDH_DRV_TYPE_D_MSK                                      (((1U<<SDH_DRV_TYPE_D_LEN)-1)<<SDH_DRV_TYPE_D_POS)
#define SDH_DRV_TYPE_D_UMSK                                     (~(((1U<<SDH_DRV_TYPE_D_LEN)-1)<<SDH_DRV_TYPE_D_POS))
#define SDH_TMR_RETUNE                                          SDH_TMR_RETUNE
#define SDH_TMR_RETUNE_POS                                      (8U)
#define SDH_TMR_RETUNE_LEN                                      (4U)
#define SDH_TMR_RETUNE_MSK                                      (((1U<<SDH_TMR_RETUNE_LEN)-1)<<SDH_TMR_RETUNE_POS)
#define SDH_TMR_RETUNE_UMSK                                     (~(((1U<<SDH_TMR_RETUNE_LEN)-1)<<SDH_TMR_RETUNE_POS))
#define SDH_SDR50_TUNE                                          SDH_SDR50_TUNE
#define SDH_SDR50_TUNE_POS                                      (13U)
#define SDH_SDR50_TUNE_LEN                                      (1U)
#define SDH_SDR50_TUNE_MSK                                      (((1U<<SDH_SDR50_TUNE_LEN)-1)<<SDH_SDR50_TUNE_POS)
#define SDH_SDR50_TUNE_UMSK                                     (~(((1U<<SDH_SDR50_TUNE_LEN)-1)<<SDH_SDR50_TUNE_POS))
#define SDH_RETUNE_MODES                                        SDH_RETUNE_MODES
#define SDH_RETUNE_MODES_POS                                    (14U)
#define SDH_RETUNE_MODES_LEN                                    (2U)
#define SDH_RETUNE_MODES_MSK                                    (((1U<<SDH_RETUNE_MODES_LEN)-1)<<SDH_RETUNE_MODES_POS)
#define SDH_RETUNE_MODES_UMSK                                   (~(((1U<<SDH_RETUNE_MODES_LEN)-1)<<SDH_RETUNE_MODES_POS))

/* 0x46 : Capabilities Register 4 */
#define SDH_SD_CAPABILITIES_4_OFFSET                            (0x46)
#define SDH_CLK_MULTIPLIER                                      SDH_CLK_MULTIPLIER
#define SDH_CLK_MULTIPLIER_POS                                  (0U)
#define SDH_CLK_MULTIPLIER_LEN                                  (8U)
#define SDH_CLK_MULTIPLIER_MSK                                  (((1U<<SDH_CLK_MULTIPLIER_LEN)-1)<<SDH_CLK_MULTIPLIER_POS)
#define SDH_CLK_MULTIPLIER_UMSK                                 (~(((1U<<SDH_CLK_MULTIPLIER_LEN)-1)<<SDH_CLK_MULTIPLIER_POS))

/* 0x48 : Maximum Current Register 1 */
#define SDH_SD_MAX_CURRENT_1_OFFSET                             (0x48)
#define SDH_MAX_CUR_33                                          SDH_MAX_CUR_33
#define SDH_MAX_CUR_33_POS                                      (0U)
#define SDH_MAX_CUR_33_LEN                                      (8U)
#define SDH_MAX_CUR_33_MSK                                      (((1U<<SDH_MAX_CUR_33_LEN)-1)<<SDH_MAX_CUR_33_POS)
#define SDH_MAX_CUR_33_UMSK                                     (~(((1U<<SDH_MAX_CUR_33_LEN)-1)<<SDH_MAX_CUR_33_POS))
#define SDH_MAX_CUR_30                                          SDH_MAX_CUR_30
#define SDH_MAX_CUR_30_POS                                      (8U)
#define SDH_MAX_CUR_30_LEN                                      (8U)
#define SDH_MAX_CUR_30_MSK                                      (((1U<<SDH_MAX_CUR_30_LEN)-1)<<SDH_MAX_CUR_30_POS)
#define SDH_MAX_CUR_30_UMSK                                     (~(((1U<<SDH_MAX_CUR_30_LEN)-1)<<SDH_MAX_CUR_30_POS))

/* 0x4A : Maximum Current Register 2 */
#define SDH_SD_MAX_CURRENT_2_OFFSET                             (0x4A)
#define SDH_MAX_CUR_18                                          SDH_MAX_CUR_18
#define SDH_MAX_CUR_18_POS                                      (0U)
#define SDH_MAX_CUR_18_LEN                                      (8U)
#define SDH_MAX_CUR_18_MSK                                      (((1U<<SDH_MAX_CUR_18_LEN)-1)<<SDH_MAX_CUR_18_POS)
#define SDH_MAX_CUR_18_UMSK                                     (~(((1U<<SDH_MAX_CUR_18_LEN)-1)<<SDH_MAX_CUR_18_POS))

/* 0x4C : Maximum Current Register 3 */
#define SDH_SD_MAX_CURRENT_3_OFFSET                             (0x4C)

/* 0x4E : Maximum Current Register 4 */
#define SDH_SD_MAX_CURRENT_4_OFFSET                             (0x4E)

/* 0x50 : Force Event Auto cmd12 Error Register */
#define SDH_SD_FORCE_EVENT_AUTO_CMD12_ERROR_OFFSET              (0x50)
#define SDH_F_ACMD12_NEXE_ERR                                   SDH_F_ACMD12_NEXE_ERR
#define SDH_F_ACMD12_NEXE_ERR_POS                               (0U)
#define SDH_F_ACMD12_NEXE_ERR_LEN                               (1U)
#define SDH_F_ACMD12_NEXE_ERR_MSK                               (((1U<<SDH_F_ACMD12_NEXE_ERR_LEN)-1)<<SDH_F_ACMD12_NEXE_ERR_POS)
#define SDH_F_ACMD12_NEXE_ERR_UMSK                              (~(((1U<<SDH_F_ACMD12_NEXE_ERR_LEN)-1)<<SDH_F_ACMD12_NEXE_ERR_POS))
#define SDH_F_ACMD_TO_ERR                                       SDH_F_ACMD_TO_ERR
#define SDH_F_ACMD_TO_ERR_POS                                   (1U)
#define SDH_F_ACMD_TO_ERR_LEN                                   (1U)
#define SDH_F_ACMD_TO_ERR_MSK                                   (((1U<<SDH_F_ACMD_TO_ERR_LEN)-1)<<SDH_F_ACMD_TO_ERR_POS)
#define SDH_F_ACMD_TO_ERR_UMSK                                  (~(((1U<<SDH_F_ACMD_TO_ERR_LEN)-1)<<SDH_F_ACMD_TO_ERR_POS))
#define SDH_F_ACMD_CRC_ERR                                      SDH_F_ACMD_CRC_ERR
#define SDH_F_ACMD_CRC_ERR_POS                                  (2U)
#define SDH_F_ACMD_CRC_ERR_LEN                                  (1U)
#define SDH_F_ACMD_CRC_ERR_MSK                                  (((1U<<SDH_F_ACMD_CRC_ERR_LEN)-1)<<SDH_F_ACMD_CRC_ERR_POS)
#define SDH_F_ACMD_CRC_ERR_UMSK                                 (~(((1U<<SDH_F_ACMD_CRC_ERR_LEN)-1)<<SDH_F_ACMD_CRC_ERR_POS))
#define SDH_F_ACMD_EBIT_ERR                                     SDH_F_ACMD_EBIT_ERR
#define SDH_F_ACMD_EBIT_ERR_POS                                 (3U)
#define SDH_F_ACMD_EBIT_ERR_LEN                                 (1U)
#define SDH_F_ACMD_EBIT_ERR_MSK                                 (((1U<<SDH_F_ACMD_EBIT_ERR_LEN)-1)<<SDH_F_ACMD_EBIT_ERR_POS)
#define SDH_F_ACMD_EBIT_ERR_UMSK                                (~(((1U<<SDH_F_ACMD_EBIT_ERR_LEN)-1)<<SDH_F_ACMD_EBIT_ERR_POS))
#define SDH_F_ACMD_INDEX_ERR                                    SDH_F_ACMD_INDEX_ERR
#define SDH_F_ACMD_INDEX_ERR_POS                                (4U)
#define SDH_F_ACMD_INDEX_ERR_LEN                                (1U)
#define SDH_F_ACMD_INDEX_ERR_MSK                                (((1U<<SDH_F_ACMD_INDEX_ERR_LEN)-1)<<SDH_F_ACMD_INDEX_ERR_POS)
#define SDH_F_ACMD_INDEX_ERR_UMSK                               (~(((1U<<SDH_F_ACMD_INDEX_ERR_LEN)-1)<<SDH_F_ACMD_INDEX_ERR_POS))
#define SDH_F_ACMD12_ISSUE_ERR                                  SDH_F_ACMD12_ISSUE_ERR
#define SDH_F_ACMD12_ISSUE_ERR_POS                              (7U)
#define SDH_F_ACMD12_ISSUE_ERR_LEN                              (1U)
#define SDH_F_ACMD12_ISSUE_ERR_MSK                              (((1U<<SDH_F_ACMD12_ISSUE_ERR_LEN)-1)<<SDH_F_ACMD12_ISSUE_ERR_POS)
#define SDH_F_ACMD12_ISSUE_ERR_UMSK                             (~(((1U<<SDH_F_ACMD12_ISSUE_ERR_LEN)-1)<<SDH_F_ACMD12_ISSUE_ERR_POS))

/* 0x52 : Force Event for Error Status Register */
#define SDH_SD_FORCE_EVENT_FOR_ERROR_STATUS_OFFSET              (0x52)
#define SDH_F_CMD_TO_ERR                                        SDH_F_CMD_TO_ERR
#define SDH_F_CMD_TO_ERR_POS                                    (0U)
#define SDH_F_CMD_TO_ERR_LEN                                    (1U)
#define SDH_F_CMD_TO_ERR_MSK                                    (((1U<<SDH_F_CMD_TO_ERR_LEN)-1)<<SDH_F_CMD_TO_ERR_POS)
#define SDH_F_CMD_TO_ERR_UMSK                                   (~(((1U<<SDH_F_CMD_TO_ERR_LEN)-1)<<SDH_F_CMD_TO_ERR_POS))
#define SDH_F_CMD_CRC_ERR                                       SDH_F_CMD_CRC_ERR
#define SDH_F_CMD_CRC_ERR_POS                                   (1U)
#define SDH_F_CMD_CRC_ERR_LEN                                   (1U)
#define SDH_F_CMD_CRC_ERR_MSK                                   (((1U<<SDH_F_CMD_CRC_ERR_LEN)-1)<<SDH_F_CMD_CRC_ERR_POS)
#define SDH_F_CMD_CRC_ERR_UMSK                                  (~(((1U<<SDH_F_CMD_CRC_ERR_LEN)-1)<<SDH_F_CMD_CRC_ERR_POS))
#define SDH_F_CMD_END_BIT_ERR                                   SDH_F_CMD_END_BIT_ERR
#define SDH_F_CMD_END_BIT_ERR_POS                               (2U)
#define SDH_F_CMD_END_BIT_ERR_LEN                               (1U)
#define SDH_F_CMD_END_BIT_ERR_MSK                               (((1U<<SDH_F_CMD_END_BIT_ERR_LEN)-1)<<SDH_F_CMD_END_BIT_ERR_POS)
#define SDH_F_CMD_END_BIT_ERR_UMSK                              (~(((1U<<SDH_F_CMD_END_BIT_ERR_LEN)-1)<<SDH_F_CMD_END_BIT_ERR_POS))
#define SDH_F_CMD_INDEX_ERR                                     SDH_F_CMD_INDEX_ERR
#define SDH_F_CMD_INDEX_ERR_POS                                 (3U)
#define SDH_F_CMD_INDEX_ERR_LEN                                 (1U)
#define SDH_F_CMD_INDEX_ERR_MSK                                 (((1U<<SDH_F_CMD_INDEX_ERR_LEN)-1)<<SDH_F_CMD_INDEX_ERR_POS)
#define SDH_F_CMD_INDEX_ERR_UMSK                                (~(((1U<<SDH_F_CMD_INDEX_ERR_LEN)-1)<<SDH_F_CMD_INDEX_ERR_POS))
#define SDH_F_DAT_TO_ERR                                        SDH_F_DAT_TO_ERR
#define SDH_F_DAT_TO_ERR_POS                                    (4U)
#define SDH_F_DAT_TO_ERR_LEN                                    (1U)
#define SDH_F_DAT_TO_ERR_MSK                                    (((1U<<SDH_F_DAT_TO_ERR_LEN)-1)<<SDH_F_DAT_TO_ERR_POS)
#define SDH_F_DAT_TO_ERR_UMSK                                   (~(((1U<<SDH_F_DAT_TO_ERR_LEN)-1)<<SDH_F_DAT_TO_ERR_POS))
#define SDH_F_DAT_CRC_ERR                                       SDH_F_DAT_CRC_ERR
#define SDH_F_DAT_CRC_ERR_POS                                   (5U)
#define SDH_F_DAT_CRC_ERR_LEN                                   (1U)
#define SDH_F_DAT_CRC_ERR_MSK                                   (((1U<<SDH_F_DAT_CRC_ERR_LEN)-1)<<SDH_F_DAT_CRC_ERR_POS)
#define SDH_F_DAT_CRC_ERR_UMSK                                  (~(((1U<<SDH_F_DAT_CRC_ERR_LEN)-1)<<SDH_F_DAT_CRC_ERR_POS))
#define SDH_F_DAT_END_BIT_ERR                                   SDH_F_DAT_END_BIT_ERR
#define SDH_F_DAT_END_BIT_ERR_POS                               (6U)
#define SDH_F_DAT_END_BIT_ERR_LEN                               (1U)
#define SDH_F_DAT_END_BIT_ERR_MSK                               (((1U<<SDH_F_DAT_END_BIT_ERR_LEN)-1)<<SDH_F_DAT_END_BIT_ERR_POS)
#define SDH_F_DAT_END_BIT_ERR_UMSK                              (~(((1U<<SDH_F_DAT_END_BIT_ERR_LEN)-1)<<SDH_F_DAT_END_BIT_ERR_POS))
#define SDH_F_CURRENT_ERR                                       SDH_F_CURRENT_ERR
#define SDH_F_CURRENT_ERR_POS                                   (7U)
#define SDH_F_CURRENT_ERR_LEN                                   (1U)
#define SDH_F_CURRENT_ERR_MSK                                   (((1U<<SDH_F_CURRENT_ERR_LEN)-1)<<SDH_F_CURRENT_ERR_POS)
#define SDH_F_CURRENT_ERR_UMSK                                  (~(((1U<<SDH_F_CURRENT_ERR_LEN)-1)<<SDH_F_CURRENT_ERR_POS))
#define SDH_F_ACMD12_ERR                                        SDH_F_ACMD12_ERR
#define SDH_F_ACMD12_ERR_POS                                    (8U)
#define SDH_F_ACMD12_ERR_LEN                                    (1U)
#define SDH_F_ACMD12_ERR_MSK                                    (((1U<<SDH_F_ACMD12_ERR_LEN)-1)<<SDH_F_ACMD12_ERR_POS)
#define SDH_F_ACMD12_ERR_UMSK                                   (~(((1U<<SDH_F_ACMD12_ERR_LEN)-1)<<SDH_F_ACMD12_ERR_POS))
#define SDH_F_ADMA_ERR                                          SDH_F_ADMA_ERR
#define SDH_F_ADMA_ERR_POS                                      (9U)
#define SDH_F_ADMA_ERR_LEN                                      (1U)
#define SDH_F_ADMA_ERR_MSK                                      (((1U<<SDH_F_ADMA_ERR_LEN)-1)<<SDH_F_ADMA_ERR_POS)
#define SDH_F_ADMA_ERR_UMSK                                     (~(((1U<<SDH_F_ADMA_ERR_LEN)-1)<<SDH_F_ADMA_ERR_POS))
#define SDH_F_VENDOR_SPECIFIC_ERR                               SDH_F_VENDOR_SPECIFIC_ERR
#define SDH_F_VENDOR_SPECIFIC_ERR_POS                           (12U)
#define SDH_F_VENDOR_SPECIFIC_ERR_LEN                           (4U)
#define SDH_F_VENDOR_SPECIFIC_ERR_MSK                           (((1U<<SDH_F_VENDOR_SPECIFIC_ERR_LEN)-1)<<SDH_F_VENDOR_SPECIFIC_ERR_POS)
#define SDH_F_VENDOR_SPECIFIC_ERR_UMSK                          (~(((1U<<SDH_F_VENDOR_SPECIFIC_ERR_LEN)-1)<<SDH_F_VENDOR_SPECIFIC_ERR_POS))

/* 0x54 : ADMA Error Status Register */
#define SDH_SD_ADMA_ERROR_STATUS_OFFSET                         (0x54)
#define SDH_ADMA_STATE                                          SDH_ADMA_STATE
#define SDH_ADMA_STATE_POS                                      (0U)
#define SDH_ADMA_STATE_LEN                                      (2U)
#define SDH_ADMA_STATE_MSK                                      (((1U<<SDH_ADMA_STATE_LEN)-1)<<SDH_ADMA_STATE_POS)
#define SDH_ADMA_STATE_UMSK                                     (~(((1U<<SDH_ADMA_STATE_LEN)-1)<<SDH_ADMA_STATE_POS))
#define SDH_ADMA_LEN_ERR                                        SDH_ADMA_LEN_ERR
#define SDH_ADMA_LEN_ERR_POS                                    (2U)
#define SDH_ADMA_LEN_ERR_LEN                                    (1U)
#define SDH_ADMA_LEN_ERR_MSK                                    (((1U<<SDH_ADMA_LEN_ERR_LEN)-1)<<SDH_ADMA_LEN_ERR_POS)
#define SDH_ADMA_LEN_ERR_UMSK                                   (~(((1U<<SDH_ADMA_LEN_ERR_LEN)-1)<<SDH_ADMA_LEN_ERR_POS))

/* 0x58 : ADMA System Address Register 1 */
#define SDH_SD_ADMA_SYS_ADDR_1_OFFSET                           (0x58)
#define SDH_ADMA_SYS_ADDR                                       SDH_ADMA_SYS_ADDR
#define SDH_ADMA_SYS_ADDR_POS                                   (0U)
#define SDH_ADMA_SYS_ADDR_LEN                                   (16U)
#define SDH_ADMA_SYS_ADDR_MSK                                   (((1U<<SDH_ADMA_SYS_ADDR_LEN)-1)<<SDH_ADMA_SYS_ADDR_POS)
#define SDH_ADMA_SYS_ADDR_UMSK                                  (~(((1U<<SDH_ADMA_SYS_ADDR_LEN)-1)<<SDH_ADMA_SYS_ADDR_POS))

/* 0x5A : ADMA System Address Register 2 */
#define SDH_SD_ADMA_SYS_ADDR_2_OFFSET                           (0x5A)
#define SDH_ADMA_SYS_ADDR                                       SDH_ADMA_SYS_ADDR
#define SDH_ADMA_SYS_ADDR_POS                                   (0U)
#define SDH_ADMA_SYS_ADDR_LEN                                   (16U)
#define SDH_ADMA_SYS_ADDR_MSK                                   (((1U<<SDH_ADMA_SYS_ADDR_LEN)-1)<<SDH_ADMA_SYS_ADDR_POS)
#define SDH_ADMA_SYS_ADDR_UMSK                                  (~(((1U<<SDH_ADMA_SYS_ADDR_LEN)-1)<<SDH_ADMA_SYS_ADDR_POS))

/* 0x5C : ADMA System Address Register 3 */
#define SDH_SD_ADMA_SYS_ADDR_3_OFFSET                           (0x5C)

/* 0x5E : ADMA System Address Register 4 */
#define SDH_SD_ADMA_SYS_ADDR_4_OFFSET                           (0x5E)

/* 0x60 : Preset Value Register for Initialization */
#define SDH_PRESET_VALUE_FOR_INIT_OFFSET                        (0x60)
#define SDH_SDCLK_FREQ_SEL_VAL                                  SDH_SDCLK_FREQ_SEL_VAL
#define SDH_SDCLK_FREQ_SEL_VAL_POS                              (0U)
#define SDH_SDCLK_FREQ_SEL_VAL_LEN                              (10U)
#define SDH_SDCLK_FREQ_SEL_VAL_MSK                              (((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS)
#define SDH_SDCLK_FREQ_SEL_VAL_UMSK                             (~(((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS))
#define SDH_CLKGEN_SEL_VAL                                      SDH_CLKGEN_SEL_VAL
#define SDH_CLKGEN_SEL_VAL_POS                                  (10U)
#define SDH_CLKGEN_SEL_VAL_LEN                                  (1U)
#define SDH_CLKGEN_SEL_VAL_MSK                                  (((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS)
#define SDH_CLKGEN_SEL_VAL_UMSK                                 (~(((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS))
#define SDH_DRV_STRENGTH_VAL                                    SDH_DRV_STRENGTH_VAL
#define SDH_DRV_STRENGTH_VAL_POS                                (14U)
#define SDH_DRV_STRENGTH_VAL_LEN                                (2U)
#define SDH_DRV_STRENGTH_VAL_MSK                                (((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS)
#define SDH_DRV_STRENGTH_VAL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS))

/* 0x62 : Preset Value Register for Default Speed */
#define SDH_PRESET_VALUE_FOR_DS_OFFSET                          (0x62)
#define SDH_SDCLK_FREQ_SEL_VAL                                  SDH_SDCLK_FREQ_SEL_VAL
#define SDH_SDCLK_FREQ_SEL_VAL_POS                              (0U)
#define SDH_SDCLK_FREQ_SEL_VAL_LEN                              (10U)
#define SDH_SDCLK_FREQ_SEL_VAL_MSK                              (((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS)
#define SDH_SDCLK_FREQ_SEL_VAL_UMSK                             (~(((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS))
#define SDH_CLKGEN_SEL_VAL                                      SDH_CLKGEN_SEL_VAL
#define SDH_CLKGEN_SEL_VAL_POS                                  (10U)
#define SDH_CLKGEN_SEL_VAL_LEN                                  (1U)
#define SDH_CLKGEN_SEL_VAL_MSK                                  (((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS)
#define SDH_CLKGEN_SEL_VAL_UMSK                                 (~(((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS))
#define SDH_DRV_STRENGTH_VAL                                    SDH_DRV_STRENGTH_VAL
#define SDH_DRV_STRENGTH_VAL_POS                                (14U)
#define SDH_DRV_STRENGTH_VAL_LEN                                (2U)
#define SDH_DRV_STRENGTH_VAL_MSK                                (((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS)
#define SDH_DRV_STRENGTH_VAL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS))

/* 0x64 : Preset Value Register for High Speed */
#define SDH_PRESET_VALUE_FOR_HS_OFFSET                          (0x64)
#define SDH_SDCLK_FREQ_SEL_VAL                                  SDH_SDCLK_FREQ_SEL_VAL
#define SDH_SDCLK_FREQ_SEL_VAL_POS                              (0U)
#define SDH_SDCLK_FREQ_SEL_VAL_LEN                              (10U)
#define SDH_SDCLK_FREQ_SEL_VAL_MSK                              (((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS)
#define SDH_SDCLK_FREQ_SEL_VAL_UMSK                             (~(((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS))
#define SDH_CLKGEN_SEL_VAL                                      SDH_CLKGEN_SEL_VAL
#define SDH_CLKGEN_SEL_VAL_POS                                  (10U)
#define SDH_CLKGEN_SEL_VAL_LEN                                  (1U)
#define SDH_CLKGEN_SEL_VAL_MSK                                  (((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS)
#define SDH_CLKGEN_SEL_VAL_UMSK                                 (~(((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS))
#define SDH_DRV_STRENGTH_VAL                                    SDH_DRV_STRENGTH_VAL
#define SDH_DRV_STRENGTH_VAL_POS                                (14U)
#define SDH_DRV_STRENGTH_VAL_LEN                                (2U)
#define SDH_DRV_STRENGTH_VAL_MSK                                (((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS)
#define SDH_DRV_STRENGTH_VAL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS))

/* 0x66 : Preset Value Register for SDR12 */
#define SDH_PRESET_VALUE_FOR_SDR12_OFFSET                       (0x66)
#define SDH_SDCLK_FREQ_SEL_VAL                                  SDH_SDCLK_FREQ_SEL_VAL
#define SDH_SDCLK_FREQ_SEL_VAL_POS                              (0U)
#define SDH_SDCLK_FREQ_SEL_VAL_LEN                              (10U)
#define SDH_SDCLK_FREQ_SEL_VAL_MSK                              (((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS)
#define SDH_SDCLK_FREQ_SEL_VAL_UMSK                             (~(((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS))
#define SDH_CLKGEN_SEL_VAL                                      SDH_CLKGEN_SEL_VAL
#define SDH_CLKGEN_SEL_VAL_POS                                  (10U)
#define SDH_CLKGEN_SEL_VAL_LEN                                  (1U)
#define SDH_CLKGEN_SEL_VAL_MSK                                  (((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS)
#define SDH_CLKGEN_SEL_VAL_UMSK                                 (~(((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS))
#define SDH_DRV_STRENGTH_VAL                                    SDH_DRV_STRENGTH_VAL
#define SDH_DRV_STRENGTH_VAL_POS                                (14U)
#define SDH_DRV_STRENGTH_VAL_LEN                                (2U)
#define SDH_DRV_STRENGTH_VAL_MSK                                (((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS)
#define SDH_DRV_STRENGTH_VAL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS))

/* 0x68 : Preset Value Register for SDR25 */
#define SDH_PRESET_VALUE_FOR_SDR25_OFFSET                       (0x68)
#define SDH_SDCLK_FREQ_SEL_VAL                                  SDH_SDCLK_FREQ_SEL_VAL
#define SDH_SDCLK_FREQ_SEL_VAL_POS                              (0U)
#define SDH_SDCLK_FREQ_SEL_VAL_LEN                              (10U)
#define SDH_SDCLK_FREQ_SEL_VAL_MSK                              (((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS)
#define SDH_SDCLK_FREQ_SEL_VAL_UMSK                             (~(((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS))
#define SDH_CLKGEN_SEL_VAL                                      SDH_CLKGEN_SEL_VAL
#define SDH_CLKGEN_SEL_VAL_POS                                  (10U)
#define SDH_CLKGEN_SEL_VAL_LEN                                  (1U)
#define SDH_CLKGEN_SEL_VAL_MSK                                  (((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS)
#define SDH_CLKGEN_SEL_VAL_UMSK                                 (~(((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS))
#define SDH_DRV_STRENGTH_VAL                                    SDH_DRV_STRENGTH_VAL
#define SDH_DRV_STRENGTH_VAL_POS                                (14U)
#define SDH_DRV_STRENGTH_VAL_LEN                                (2U)
#define SDH_DRV_STRENGTH_VAL_MSK                                (((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS)
#define SDH_DRV_STRENGTH_VAL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS))

/* 0x6a : Preset Value Register for SDR50 */
#define SDH_PRESET_VALUE_FOR_SDR50_OFFSET                       (0x6a)
#define SDH_SDCLK_FREQ_SEL_VAL                                  SDH_SDCLK_FREQ_SEL_VAL
#define SDH_SDCLK_FREQ_SEL_VAL_POS                              (0U)
#define SDH_SDCLK_FREQ_SEL_VAL_LEN                              (10U)
#define SDH_SDCLK_FREQ_SEL_VAL_MSK                              (((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS)
#define SDH_SDCLK_FREQ_SEL_VAL_UMSK                             (~(((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS))
#define SDH_CLKGEN_SEL_VAL                                      SDH_CLKGEN_SEL_VAL
#define SDH_CLKGEN_SEL_VAL_POS                                  (10U)
#define SDH_CLKGEN_SEL_VAL_LEN                                  (1U)
#define SDH_CLKGEN_SEL_VAL_MSK                                  (((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS)
#define SDH_CLKGEN_SEL_VAL_UMSK                                 (~(((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS))
#define SDH_DRV_STRENGTH_VAL                                    SDH_DRV_STRENGTH_VAL
#define SDH_DRV_STRENGTH_VAL_POS                                (14U)
#define SDH_DRV_STRENGTH_VAL_LEN                                (2U)
#define SDH_DRV_STRENGTH_VAL_MSK                                (((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS)
#define SDH_DRV_STRENGTH_VAL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS))

/* 0x6c : Preset Value Register for SDR104 */
#define SDH_PRESET_VALUE_FOR_SDR104_OFFSET                      (0x6c)
#define SDH_SDCLK_FREQ_SEL_VAL                                  SDH_SDCLK_FREQ_SEL_VAL
#define SDH_SDCLK_FREQ_SEL_VAL_POS                              (0U)
#define SDH_SDCLK_FREQ_SEL_VAL_LEN                              (10U)
#define SDH_SDCLK_FREQ_SEL_VAL_MSK                              (((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS)
#define SDH_SDCLK_FREQ_SEL_VAL_UMSK                             (~(((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS))
#define SDH_CLKGEN_SEL_VAL                                      SDH_CLKGEN_SEL_VAL
#define SDH_CLKGEN_SEL_VAL_POS                                  (10U)
#define SDH_CLKGEN_SEL_VAL_LEN                                  (1U)
#define SDH_CLKGEN_SEL_VAL_MSK                                  (((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS)
#define SDH_CLKGEN_SEL_VAL_UMSK                                 (~(((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS))
#define SDH_DRV_STRENGTH_VAL                                    SDH_DRV_STRENGTH_VAL
#define SDH_DRV_STRENGTH_VAL_POS                                (14U)
#define SDH_DRV_STRENGTH_VAL_LEN                                (2U)
#define SDH_DRV_STRENGTH_VAL_MSK                                (((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS)
#define SDH_DRV_STRENGTH_VAL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS))

/* 0x6e : Preset Value Register for DDR50 */
#define SDH_PRESET_VALUE_FOR_DDR50_OFFSET                       (0x6e)
#define SDH_SDCLK_FREQ_SEL_VAL                                  SDH_SDCLK_FREQ_SEL_VAL
#define SDH_SDCLK_FREQ_SEL_VAL_POS                              (0U)
#define SDH_SDCLK_FREQ_SEL_VAL_LEN                              (10U)
#define SDH_SDCLK_FREQ_SEL_VAL_MSK                              (((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS)
#define SDH_SDCLK_FREQ_SEL_VAL_UMSK                             (~(((1U<<SDH_SDCLK_FREQ_SEL_VAL_LEN)-1)<<SDH_SDCLK_FREQ_SEL_VAL_POS))
#define SDH_CLKGEN_SEL_VAL                                      SDH_CLKGEN_SEL_VAL
#define SDH_CLKGEN_SEL_VAL_POS                                  (10U)
#define SDH_CLKGEN_SEL_VAL_LEN                                  (1U)
#define SDH_CLKGEN_SEL_VAL_MSK                                  (((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS)
#define SDH_CLKGEN_SEL_VAL_UMSK                                 (~(((1U<<SDH_CLKGEN_SEL_VAL_LEN)-1)<<SDH_CLKGEN_SEL_VAL_POS))
#define SDH_DRV_STRENGTH_VAL                                    SDH_DRV_STRENGTH_VAL
#define SDH_DRV_STRENGTH_VAL_POS                                (14U)
#define SDH_DRV_STRENGTH_VAL_LEN                                (2U)
#define SDH_DRV_STRENGTH_VAL_MSK                                (((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS)
#define SDH_DRV_STRENGTH_VAL_UMSK                               (~(((1U<<SDH_DRV_STRENGTH_VAL_LEN)-1)<<SDH_DRV_STRENGTH_VAL_POS))

/* 0xe0 : Shared Bus Control Register */
#define SDH_SHARED_BUS_CTRL_OFFSET                              (0xe0)
#define SDH_NUM_CLK_PINS                                        SDH_NUM_CLK_PINS
#define SDH_NUM_CLK_PINS_POS                                    (0U)
#define SDH_NUM_CLK_PINS_LEN                                    (3U)
#define SDH_NUM_CLK_PINS_MSK                                    (((1U<<SDH_NUM_CLK_PINS_LEN)-1)<<SDH_NUM_CLK_PINS_POS)
#define SDH_NUM_CLK_PINS_UMSK                                   (~(((1U<<SDH_NUM_CLK_PINS_LEN)-1)<<SDH_NUM_CLK_PINS_POS))
#define SDH_NUM_INT_PINS                                        SDH_NUM_INT_PINS
#define SDH_NUM_INT_PINS_POS                                    (4U)
#define SDH_NUM_INT_PINS_LEN                                    (2U)
#define SDH_NUM_INT_PINS_MSK                                    (((1U<<SDH_NUM_INT_PINS_LEN)-1)<<SDH_NUM_INT_PINS_POS)
#define SDH_NUM_INT_PINS_UMSK                                   (~(((1U<<SDH_NUM_INT_PINS_LEN)-1)<<SDH_NUM_INT_PINS_POS))
#define SDH_BUS_WIDTH_PRESET                                    SDH_BUS_WIDTH_PRESET
#define SDH_BUS_WIDTH_PRESET_POS                                (8U)
#define SDH_BUS_WIDTH_PRESET_LEN                                (7U)
#define SDH_BUS_WIDTH_PRESET_MSK                                (((1U<<SDH_BUS_WIDTH_PRESET_LEN)-1)<<SDH_BUS_WIDTH_PRESET_POS)
#define SDH_BUS_WIDTH_PRESET_UMSK                               (~(((1U<<SDH_BUS_WIDTH_PRESET_LEN)-1)<<SDH_BUS_WIDTH_PRESET_POS))
#define SDH_CLK_PIN_SEL                                         SDH_CLK_PIN_SEL
#define SDH_CLK_PIN_SEL_POS                                     (16U)
#define SDH_CLK_PIN_SEL_LEN                                     (3U)
#define SDH_CLK_PIN_SEL_MSK                                     (((1U<<SDH_CLK_PIN_SEL_LEN)-1)<<SDH_CLK_PIN_SEL_POS)
#define SDH_CLK_PIN_SEL_UMSK                                    (~(((1U<<SDH_CLK_PIN_SEL_LEN)-1)<<SDH_CLK_PIN_SEL_POS))
#define SDH_INT_PIN_SEL                                         SDH_INT_PIN_SEL
#define SDH_INT_PIN_SEL_POS                                     (20U)
#define SDH_INT_PIN_SEL_LEN                                     (3U)
#define SDH_INT_PIN_SEL_MSK                                     (((1U<<SDH_INT_PIN_SEL_LEN)-1)<<SDH_INT_PIN_SEL_POS)
#define SDH_INT_PIN_SEL_UMSK                                    (~(((1U<<SDH_INT_PIN_SEL_LEN)-1)<<SDH_INT_PIN_SEL_POS))
#define SDH_BEND_PWR_CTRL                                       SDH_BEND_PWR_CTRL
#define SDH_BEND_PWR_CTRL_POS                                   (24U)
#define SDH_BEND_PWR_CTRL_LEN                                   (7U)
#define SDH_BEND_PWR_CTRL_MSK                                   (((1U<<SDH_BEND_PWR_CTRL_LEN)-1)<<SDH_BEND_PWR_CTRL_POS)
#define SDH_BEND_PWR_CTRL_UMSK                                  (~(((1U<<SDH_BEND_PWR_CTRL_LEN)-1)<<SDH_BEND_PWR_CTRL_POS))

/* 0xEC : SD_AHB_BURST_SIZE */
#define SDH_SD_AHB_BURST_SIZE_OFFSET                            (0xEC)
#define SDH_AHB_BURST_SIZE                                      SDH_AHB_BURST_SIZE
#define SDH_AHB_BURST_SIZE_POS                                  (0U)
#define SDH_AHB_BURST_SIZE_LEN                                  (7U)
#define SDH_AHB_BURST_SIZE_MSK                                  (((1U<<SDH_AHB_BURST_SIZE_LEN)-1)<<SDH_AHB_BURST_SIZE_POS)
#define SDH_AHB_BURST_SIZE_UMSK                                 (~(((1U<<SDH_AHB_BURST_SIZE_LEN)-1)<<SDH_AHB_BURST_SIZE_POS))

/* 0xFC : Slot Interrupt Status Register */
#define SDH_SD_SLOT_INT_STATUS_OFFSET                           (0xFC)
#define SDH_SLOT_INT                                            SDH_SLOT_INT
#define SDH_SLOT_INT_POS                                        (0U)
#define SDH_SLOT_INT_LEN                                        (8U)
#define SDH_SLOT_INT_MSK                                        (((1U<<SDH_SLOT_INT_LEN)-1)<<SDH_SLOT_INT_POS)
#define SDH_SLOT_INT_UMSK                                       (~(((1U<<SDH_SLOT_INT_LEN)-1)<<SDH_SLOT_INT_POS))

/* 0xFE : Host Control Version Register */
#define SDH_SD_HOST_CTRL_VER_OFFSET                             (0xFE)
#define SDH_SPECIFICATION_VER_NUM                               SDH_SPECIFICATION_VER_NUM
#define SDH_SPECIFICATION_VER_NUM_POS                           (0U)
#define SDH_SPECIFICATION_VER_NUM_LEN                           (8U)
#define SDH_SPECIFICATION_VER_NUM_MSK                           (((1U<<SDH_SPECIFICATION_VER_NUM_LEN)-1)<<SDH_SPECIFICATION_VER_NUM_POS)
#define SDH_SPECIFICATION_VER_NUM_UMSK                          (~(((1U<<SDH_SPECIFICATION_VER_NUM_LEN)-1)<<SDH_SPECIFICATION_VER_NUM_POS))
#define SDH_VENDOR_VER                                          SDH_VENDOR_VER
#define SDH_VENDOR_VER_POS                                      (8U)
#define SDH_VENDOR_VER_LEN                                      (8U)
#define SDH_VENDOR_VER_MSK                                      (((1U<<SDH_VENDOR_VER_LEN)-1)<<SDH_VENDOR_VER_POS)
#define SDH_VENDOR_VER_UMSK                                     (~(((1U<<SDH_VENDOR_VER_LEN)-1)<<SDH_VENDOR_VER_POS))


struct  sdh_reg {
    /* 0x00 : System Address Low Register */
    union {
        struct {
            uint16_t DMA_ADDR_L                     : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_SYS_ADDR_LOW;

    /* 0x02 : System Address High Register */
    union {
        struct {
            uint16_t DMA_ADDR_H                     : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_SYS_ADDR_HIGH;

    /* 0x04 : Block Size Register */
    union {
        struct {
            uint16_t BLOCK_SIZE                     : 12; /* [11: 0],         RW,        0x0 */
            uint16_t HOST_DMA_BDRY                  :  3; /* [14:12],         RW,        0x0 */
            uint16_t Reserved                       :  1; /* [   15],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_BLOCK_SIZE;

    /* 0x06 : Block Count Register */
    union {
        struct {
            uint16_t BLOCK_COUNT                    : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_BLOCK_COUNT;

    /* 0x08 : Argument Low Register */
    union {
        struct {
            uint16_t ARG_L                          : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ARG_LOW;

    /* 0x0A : Argument High Register */
    union {
        struct {
            uint16_t ARG_H                          : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ARG_HIGH;

    /* 0x0C : Transfer Mode Register */
    union {
        struct {
            uint16_t DMA_EN                         :  1; /* [    0],         RW,        0x0 */
            uint16_t BLK_CNT_EN                     :  1; /* [    1],         RW,        0x0 */
            uint16_t AUTO_CMD_EN                    :  2; /* [ 3: 2],       RSVD,        0x0 */
            uint16_t TO_HOST_DIR                    :  1; /* [    4],         RW,        0x0 */
            uint16_t MULTI_BLK_SEL                  :  1; /* [    5],         RW,        0x0 */
            uint16_t Reserved_10_6                  :  5; /* [10: 6],       RSVD,        0x0 */
            uint16_t STREAM_MODE_EN                 :  1; /* [   11],         RW,        0x0 */
            uint16_t SPI_MODE_EN                    :  1; /* [   12],         RW,        0x0 */
            uint16_t BOOT_ACK_EN                    :  1; /* [   13],         RW,        0x0 */
            uint16_t ALT_BOOT_OPREATION             :  1; /* [   14],         RW,        0x0 */
            uint16_t BOOT_OPREATION                 :  1; /* [   15],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_TRANSFER_MODE;

    /* 0x0E : Command Register */
    union {
        struct {
            uint16_t RESP_TYPE                      :  2; /* [ 1: 0],         RW,        0x0 */
            uint16_t Reserved_2                     :  1; /* [    2],       RSVD,        0x0 */
            uint16_t CMD_CRC_CHK_EN                 :  1; /* [    3],         RW,        0x0 */
            uint16_t CMD_INDEX_CHK_EN               :  1; /* [    4],         RW,        0x0 */
            uint16_t DATA_PRESENT                   :  1; /* [    5],         RW,        0x0 */
            uint16_t CMD_TYPE                       :  2; /* [ 7: 6],         RW,        0x0 */
            uint16_t CMD_INDEX                      :  6; /* [13: 8],         RW,        0x0 */
            uint16_t Reserved_15_14                 :  2; /* [15:14],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_CMD;

    /* 0x10 : Response Register 0 */
    union {
        struct {
            uint16_t RESP0                          : 16; /* [15: 0],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_RESP_0;

    /* 0x12 : Response Register 1 */
    union {
        struct {
            uint16_t RESP1                          : 16; /* [15: 0],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_RESP_1;

    /* 0x14 : Response Register 2 */
    union {
        struct {
            uint16_t RESP2                          : 16; /* [15: 0],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_RESP_2;

    /* 0x16 : Response Register 3 */
    union {
        struct {
            uint16_t RESP3                          : 16; /* [15: 0],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_RESP_3;

    /* 0x18 : Response Register 4 */
    union {
        struct {
            uint16_t RESP4                          : 16; /* [15: 0],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_RESP_4;

    /* 0x1A : Response Register 5 */
    union {
        struct {
            uint16_t RESP5                          : 16; /* [15: 0],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_RESP_5;

    /* 0x1C : Response Register 6 */
    union {
        struct {
            uint16_t RESP6                          : 16; /* [15: 0],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_RESP_6;

    /* 0x1E : Response Register 7 */
    union {
        struct {
            uint16_t RESP7                          : 16; /* [15: 0],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_RESP_7;

    /* 0x20 : Buffer Data Port 0 Register */
    union {
        struct {
            uint16_t SD_BUFFER_DATA_PORT_0          : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_BUFFER_DATA_PORT_0;

    /* 0x22 : Buffer Data Port 1 Register */
    union {
        struct {
            uint16_t SD_BUFFER_DATA_PORT_1          : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_BUFFER_DATA_PORT_1;

    /* 0x24 : Present State Register 1 */
    union {
        struct {
            uint16_t CMD_INHIBIT_CMD                :  1; /* [    0],        ROC,        0x0 */
            uint16_t CMD_INHIBIT_DAT                :  1; /* [    1],        ROC,        0x0 */
            uint16_t DAT_ACTIVE                     :  1; /* [    2],        ROC,        0x0 */
            uint16_t RETUNING_REQ                   :  1; /* [    3],        ROC,        0x0 */
            uint16_t Reserved_7_4                   :  4; /* [ 7: 4],       RSVD,        0x0 */
            uint16_t TX_ACTIVE                      :  1; /* [    8],        ROC,        0x0 */
            uint16_t RX_ACTIVE                      :  1; /* [    9],        ROC,        0x0 */
            uint16_t BUFFER_WR_EN                   :  1; /* [   10],        ROC,        0x0 */
            uint16_t BUFFER_RD_EN                   :  1; /* [   11],        ROC,        0x0 */
            uint16_t Reserved_15_12                 :  4; /* [15:12],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_PRESENT_STATE_1;

    /* 0x26 : Present State Register 2 */
    union {
        struct {
            uint16_t CARD_INSERTED                  :  1; /* [    0],         RO,        0x0 */
            uint16_t CARD_STABLE                    :  1; /* [    1],         RO,        0x0 */
            uint16_t CARD_DET                       :  1; /* [    2],         RO,        0x0 */
            uint16_t WRITE_PROT                     :  1; /* [    3],         RO,        0x0 */
            uint16_t DAT_LEVEL                      :  4; /* [ 7: 4],         RO,        0x0 */
            uint16_t CMD_LEVEL                      :  1; /* [    8],         RO,        0x0 */
            uint16_t Reserved_15_9                  :  7; /* [15: 9],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_PRESENT_STATE_2;

    /* 0x28 : Host Control Register */
    union {
        struct {
            uint16_t LED_CTRL                       :  1; /* [    0],         RW,        0x0 */
            uint16_t DATA_WIDTH                     :  1; /* [    1],         RW,        0x0 */
            uint16_t HI_SPEED_EN                    :  1; /* [    2],         RW,        0x0 */
            uint16_t DMA_SEL                        :  2; /* [ 4: 3],         RW,        0x0 */
            uint16_t EX_DATA_WIDTH                  :  1; /* [    5],         RW,        0x0 */
            uint16_t CARD_DET_L                     :  1; /* [    6],         RW,        0x0 */
            uint16_t CARD_DET_S                     :  1; /* [    7],         RW,        0x0 */
            uint16_t SD_BUS_POWER                   :  1; /* [    8],         RW,        0x0 */
            uint16_t SD_BUS_VLT                     :  3; /* [11: 9],         RW,        0x0 */
            uint16_t SMIH_OD_PP                     :  1; /* [   12],         RW,        0x0 */
            uint16_t SMIH_RST_N                     :  1; /* [   13],         RW,        0x0 */
            uint16_t Reserved_15_14                 :  2; /* [15:14],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_HOST_CTRL;

    /* 0x2A : Block Gap Control Register */
    union {
        struct {
            uint16_t STOP_AT_BLOCK_GAP_REQ          :  1; /* [    0],         RW,        0x0 */
            uint16_t CONT_REQ                       :  1; /* [    1],         RW,        0x0 */
            uint16_t RD_WAIT_CTL                    :  1; /* [    2],         RW,        0x0 */
            uint16_t INT_BLK_GAP                    :  1; /* [    3],         RW,        0x0 */
            uint16_t Reserved_7_4                   :  4; /* [ 7: 4],       RSVD,        0x0 */
            uint16_t W_CARD_INT                     :  1; /* [    8],         RW,        0x0 */
            uint16_t W_INSERTION                    :  1; /* [    9],         RW,        0x0 */
            uint16_t W_REMOVAL                      :  1; /* [   10],         RW,        0x0 */
            uint16_t Reserved_15_11                 :  5; /* [15:11],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_BLOCK_GAP_CTRL;

    /* 0x2C : Clock Control Register */
    union {
        struct {
            uint16_t INT_CLK_EN                     :  1; /* [    0],         RW,        0x0 */
            uint16_t INT_CLK_STABLE                 :  1; /* [    1],        ROC,        0x0 */
            uint16_t SD_CLK_EN                      :  1; /* [    2],         RW,        0x0 */
            uint16_t Reserved_4_3                   :  2; /* [ 4: 3],       RSVD,        0x0 */
            uint16_t CLK_GEN_SEL                    :  1; /* [    5],     RW/ROC,        0x0 */
            uint16_t SD_FREQ_SEL_HI                 :  2; /* [ 7: 6],         RW,        0x0 */
            uint16_t SD_FREQ_SEL_LO                 :  8; /* [15: 8],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_CLOCK_CTRL;

    /* 0x2E : Timeout Control/Software Reset Register */
    union {
        struct {
            uint16_t TIMEOUT_VALUE                  :  4; /* [ 3: 0],         RW,        0x0 */
            uint16_t Reserved_7_4                   :  4; /* [ 7: 4],       RSVD,        0x0 */
            uint16_t SW_RST_ALL                     :  1; /* [    8],       RWAC,        0x0 */
            uint16_t SW_RST_CMD                     :  1; /* [    9],       RWAC,        0x0 */
            uint16_t SW_RST_DAT                     :  1; /* [   10],       RWAC,        0x0 */
            uint16_t Reserved_15_11                 :  5; /* [15:11],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_TIMEOUT_CTRL_SW_RESET;

    /* 0x30 : Normal Interrupt Status Register */
    union {
        struct {
            uint16_t CMD_COMPLETE                   :  1; /* [    0],       RW1C,        0x0 */
            uint16_t XFER_COMPLETE                  :  1; /* [    1],       RW1C,        0x0 */
            uint16_t BLOCK_GAP_EVT                  :  1; /* [    2],       RW1C,        0x0 */
            uint16_t DMA_INT                        :  1; /* [    3],       RW1C,        0x0 */
            uint16_t TX_RDY                         :  1; /* [    4],       RW1C,        0x0 */
            uint16_t RX_RDY                         :  1; /* [    5],       RW1C,        0x0 */
            uint16_t CARD_INS_INT                   :  1; /* [    6],       RW1C,        0x0 */
            uint16_t CARD_REM_INT                   :  1; /* [    7],       RW1C,        0x0 */
            uint16_t CARD_INT                       :  1; /* [    8],        ROC,        0x0 */
            uint16_t INT_A                          :  1; /* [    9],        ROC,        0x0 */
            uint16_t INT_B                          :  1; /* [   10],        ROC,        0x0 */
            uint16_t INT_C                          :  1; /* [   11],        ROC,        0x0 */
            uint16_t RETUNING_INT                   :  1; /* [   12],        ROC,        0x0 */
            uint16_t BOOT_COMPLETE                  :  1; /* [   13],       RW1C,        0x0 */
            uint16_t BOOT_DONE                      :  1; /* [   14],       RW1C,        0x0 */
            uint16_t ERR_INT                        :  1; /* [   15],        ROC,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_NORMAL_INT_STATUS;

    /* 0x32 : Error Interrupt Status Register */
    union {
        struct {
            uint16_t CMD_TIMEOUT_ERR                :  1; /* [    0],       RW1C,        0x0 */
            uint16_t CMD_CRC_ERR                    :  1; /* [    1],       RW1C,        0x0 */
            uint16_t CMD_END_BIT_ERR                :  1; /* [    2],       RW1C,        0x0 */
            uint16_t CMD_INDEX_ERR                  :  1; /* [    3],       RW1C,        0x0 */
            uint16_t DATA_TIMEOUT_ERR               :  1; /* [    4],       RW1C,        0x0 */
            uint16_t RD_DATA_CRC_ERR                :  1; /* [    5],        ROC,        0x0 */
            uint16_t RD_DATA_END_BIT_ERR            :  1; /* [    6],        ROC,        0x0 */
            uint16_t CUR_LIMIT_ERR                  :  1; /* [    7],        ROC,        0x0 */
            uint16_t AUTO_CMD12_ERR                 :  1; /* [    8],        ROC,        0x0 */
            uint16_t ADMA_ERR                       :  1; /* [    9],        ROC,        0x0 */
            uint16_t TUNE_ERR                       :  1; /* [   10],       RW1C,        0x0 */
            uint16_t Reserved                       :  1; /* [   11],       RSVD,        0x0 */
            uint16_t VENDOR_SPECIFIC_ERR            :  4; /* [15:12],       RW1C,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ERROR_INT_STATUS;

    /* 0x34 : Normal Interrupt Status Enable Register */
    union {
        struct {
            uint16_t CMD_COMPLETE_EN                :  1; /* [    0],         RW,        0x0 */
            uint16_t XFER_COMPLETE_EN               :  1; /* [    1],         RW,        0x0 */
            uint16_t BLOCK_GAP_EVT_EN               :  1; /* [    2],         RW,        0x0 */
            uint16_t DMA_INT_EN                     :  1; /* [    3],         RW,        0x0 */
            uint16_t TX_RDY_EN                      :  1; /* [    4],         RW,        0x0 */
            uint16_t RD_RDY_EN                      :  1; /* [    5],         RW,        0x0 */
            uint16_t CARD_INS_EN                    :  1; /* [    6],         RW,        0x0 */
            uint16_t CARD_REM_EN                    :  1; /* [    7],         RW,        0x0 */
            uint16_t CARD_INT_EN                    :  1; /* [    8],         RW,        0x0 */
            uint16_t INT_A_INT_EN                   :  1; /* [    9],         RW,        0x0 */
            uint16_t INT_B_INT_EN                   :  1; /* [   10],         RW,        0x0 */
            uint16_t INT_C_INT_EN                   :  1; /* [   11],         RW,        0x0 */
            uint16_t RETUNE_INT_EN                  :  1; /* [   12],         RW,        0x0 */
            uint16_t BOOT_ACK_COMPLETE_EN           :  1; /* [   13],         RW,        0x0 */
            uint16_t BOOT_DONE_INT_STATUS_EN        :  1; /* [   14],         RW,        0x0 */
            uint16_t FIX_TO_ZERO                    :  1; /* [   15],         RO,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_NORMAL_INT_STATUS_EN;

    /* 0x36 : Error Interrupt Status Enable Register */
    union {
        struct {
            uint16_t CMD_TIMEOUT_ERR_EN             :  1; /* [    0],         RW,        0x0 */
            uint16_t CMD_CRC_ERR_EN                 :  1; /* [    1],         RW,        0x0 */
            uint16_t CMD_END_BIT_ERR_EN             :  1; /* [    2],         RW,        0x0 */
            uint16_t CMD_INDEX_ERR_EN               :  1; /* [    3],         RW,        0x0 */
            uint16_t DATA_TIMEOUT_ERR_EN            :  1; /* [    4],         RW,        0x0 */
            uint16_t RD_DATA_CRC_ERR_EN             :  1; /* [    5],         RW,        0x0 */
            uint16_t RD_DATA_END_BIT_ERR_EN         :  1; /* [    6],         RW,        0x0 */
            uint16_t CUR_LIM_ERR_EN                 :  1; /* [    7],         RW,        0x0 */
            uint16_t AUTO_CMD12_ERR_EN              :  1; /* [    8],         RW,        0x0 */
            uint16_t ADMA_ERR_EN                    :  1; /* [    9],         RW,        0x0 */
            uint16_t TUNING_ERR_EN                  :  1; /* [   10],         RW,        0x0 */
            uint16_t Reserved                       :  1; /* [   11],       RSVD,        0x0 */
            uint16_t VENDOR_SPECIFIC_ERR_EN         :  4; /* [15:12],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ERROR_INT_STATUS_EN;

    /* 0x38 : Normal Interrupt Status Interrupt Enable Register */
    union {
        struct {
            uint16_t CMD_COMPLETE_INT_EN            :  1; /* [    0],         RW,        0x0 */
            uint16_t XFER_COMPLETE_INT_EN           :  1; /* [    1],         RW,        0x0 */
            uint16_t BLOCK_GAP_EVT_INT_EN           :  1; /* [    2],         RW,        0x0 */
            uint16_t DMA_INT_INT_EN                 :  1; /* [    3],         RW,        0x0 */
            uint16_t TX_RDY_INT_EN                  :  1; /* [    4],         RW,        0x0 */
            uint16_t RX_RDY_INT_EN                  :  1; /* [    5],         RW,        0x0 */
            uint16_t CARD_INS_INT_EN                :  1; /* [    6],         RW,        0x0 */
            uint16_t CARD_REM_INT_EN                :  1; /* [    7],         RW,        0x0 */
            uint16_t CARD_INT_INT_EN                :  1; /* [    8],         RW,        0x0 */
            uint16_t INT_A_INT_INT_EN               :  1; /* [    9],         RW,        0x0 */
            uint16_t INT_B_INT_INT_EN               :  1; /* [   10],         RW,        0x0 */
            uint16_t INT_C_INT_INT_EN               :  1; /* [   11],         RW,        0x0 */
            uint16_t RETUNE_INT_INT_EN              :  1; /* [   12],         RW,        0x0 */
            uint16_t BOOT_ACK_COMPLETE_EN           :  1; /* [   13],         RO,        0x0 */
            uint16_t BOOT_DONE_INT_EN               :  1; /* [   14],         RO,        0x0 */
            uint16_t FIX_TO_ZERO                    :  1; /* [   15],         RO,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_NORMAL_INT_STATUS_INT_EN;

    /* 0x3A : Error Interrupt Status Interrupt Enable Register */
    union {
        struct {
            uint16_t CMD_TIMEOUT_ERR_INT_EN         :  1; /* [    0],         RW,        0x0 */
            uint16_t CMD_CRC_ERR_INT_EN             :  1; /* [    1],         RW,        0x0 */
            uint16_t CMD_END_BIT_ERR_INT_EN         :  1; /* [    2],         RW,        0x0 */
            uint16_t CMD_INDEX_ERR_INT_EN           :  1; /* [    3],         RW,        0x0 */
            uint16_t DATA_TIMEOUT_ERR_INT_EN        :  1; /* [    4],         RW,        0x0 */
            uint16_t RD_DATA_CRC_ERR_INT_EN         :  1; /* [    5],         RW,        0x0 */
            uint16_t RD_DATA_END_BIT_ERR_INT_EN     :  1; /* [    6],         RW,        0x0 */
            uint16_t CUR_LIM_ERR_INT_EN             :  1; /* [    7],         RW,        0x0 */
            uint16_t AUTO_CMD12_ERR_INT_EN          :  1; /* [    8],         RW,        0x0 */
            uint16_t ADMA_ERR_INT_EN                :  1; /* [    9],         RW,        0x0 */
            uint16_t TUNE_ERR_INT_EN                :  1; /* [   10],         RW,        0x0 */
            uint16_t Reserved                       :  1; /* [   11],       RSVD,        0x0 */
            uint16_t VENDOR_SPECIFIC_ERR_INT_EN     :  4; /* [15:12],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ERROR_INT_STATUS_INT_EN;

    /* 0x3C : Auto CMD12 Error Status */
    union {
        struct {
            uint16_t AUTO_CMD12_NOT_EXE             :  1; /* [    0],        ROC,        0x0 */
            uint16_t AUTO_CMD_TIMEOUT_ERR           :  1; /* [    1],        ROC,        0x0 */
            uint16_t AUTO_CMD_CRC_ERR               :  1; /* [    2],        ROC,        0x0 */
            uint16_t AUTO_CMD_END_BIT_ERR           :  1; /* [    3],        ROC,        0x0 */
            uint16_t AUTO_CMD_INDEX_ERR             :  1; /* [    4],        ROC,        0x0 */
            uint16_t Reserved_6_5                   :  2; /* [ 6: 5],       RSVD,        0x0 */
            uint16_t CMD_NOT_ISSUED                 :  1; /* [    7],        ROC,        0x0 */
            uint16_t Reserved_15_8                  :  8; /* [15: 8],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_AUTO_CMD12_ERROR_STATUS;

    /* 0x3E : Host Control 2 */
    union {
        struct {
            uint16_t UHS_MODE_SEL                   :  3; /* [ 2: 0],         RW,        0x0 */
            uint16_t SDH_V18_EN                     :  1; /* [    3],         RW,        0x0 */
            uint16_t DRV_STRENGTH_SEL               :  2; /* [ 5: 4],         RW,        0x0 */
            uint16_t EXE_TUNING                     :  1; /* [    6],         RW,        0x0 */
            uint16_t SAMPLING_CLK_SEL               :  1; /* [    7],         RW,        0x0 */
            uint16_t CONSECUTIVE_SAMPLING           :  6; /* [13: 8],         RW,        0x0 */
            uint16_t ASYNC_INT_EN                   :  1; /* [   14],         RW,        0x0 */
            uint16_t PRE_VAL_EN                     :  1; /* [   15],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } HOST_CTRL_2;

    /* 0x40 : Capabilities Register 1 */
    union {
        struct {
            uint16_t TIMEOUT_FREQ                   :  6; /* [ 5: 0],     HwInit,       0x32 */
            uint16_t Reserved                       :  1; /* [    6],       RSVD,        0x1 */
            uint16_t TIMEOUT_UNIT                   :  1; /* [    7],     HwInit,        0x1 */
            uint16_t BASE_FREQ                      :  8; /* [15: 8],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_CAPABILITIES_1;

    /* 0x42 : Capabilities Register 2 */
    union {
        struct {
            uint16_t MAX_BLK_LEN                    :  2; /* [ 1: 0],     HwInit,        0x0 */
            uint16_t EMBEDDED_DEVICE_SUPPORT        :  1; /* [    2],     HwInit,        0x1 */
            uint16_t ADMA2_SUPPORT                  :  1; /* [    3],     HwInit,        0x1 */
            uint16_t Reserved                       :  1; /* [    4],       RSVD,        0x0 */
            uint16_t HI_SPEED_SUPPORT               :  1; /* [    5],     HwInit,        0x1 */
            uint16_t SDMA_SUPPORT                   :  1; /* [    6],     HwInit,        0x1 */
            uint16_t SUS_RES_SUPPORT                :  1; /* [    7],     HwInit,        0x0 */
            uint16_t VLG_33_SUPPORT                 :  1; /* [    8],     HwInit,        0x1 */
            uint16_t VLG_30_SUPPORT                 :  1; /* [    9],     HwInit,        0x1 */
            uint16_t VLG_18_SUPPORT                 :  1; /* [   10],     HwInit,        0x1 */
            uint16_t VLG_12_SUPPORT                 :  1; /* [   11],     HwInit,        0x1 */
            uint16_t SYS_BUS_64_SUPPORT             :  1; /* [   12],     HwInit,        0x1 */
            uint16_t ASYNC_INT_SUPPORT              :  1; /* [   13],     HwInit,        0x0 */
            uint16_t CFG_SLOT_TYPE                  :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_CAPABILITIES_2;

    /* 0x44 : Capabilities Register 3 */
    union {
        struct {
            uint16_t SDR50_SUPPORT                  :  1; /* [    0],     HwInit,        0x1 */
            uint16_t SDR104_SUPPORT                 :  1; /* [    1],     HwInit,        0x1 */
            uint16_t DDR50_SUPPORT                  :  1; /* [    2],     HwInit,        0x1 */
            uint16_t HS200_SUPPORT                  :  1; /* [    3],     HwInit,        0x0 */
            uint16_t DRV_TYPE_A                     :  1; /* [    4],     HwInit,        0x1 */
            uint16_t DRV_TYPE_C                     :  1; /* [    5],     HwInit,        0x1 */
            uint16_t DRV_TYPE_D                     :  1; /* [    6],     HwInit,        0x1 */
            uint16_t Reserved_7                     :  1; /* [    7],       RSVD,        0x0 */
            uint16_t TMR_RETUNE                     :  4; /* [11: 8],     HwInit,        0x0 */
            uint16_t Reserved_12                    :  1; /* [   12],       RSVD,        0x0 */
            uint16_t SDR50_TUNE                     :  1; /* [   13],     HwInit,        0x0 */
            uint16_t RETUNE_MODES                   :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_CAPABILITIES_3;

    /* 0x46 : Capabilities Register 4 */
    union {
        struct {
            uint16_t CLK_MULTIPLIER                 :  8; /* [ 7: 0],     HwInit,        0x0 */
            uint16_t Reserved                       :  8; /* [15: 8],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_CAPABILITIES_4;

    /* 0x48 : Maximum Current Register 1 */
    union {
        struct {
            uint16_t MAX_CUR_33                     :  8; /* [ 7: 0],     HwInit,        0x0 */
            uint16_t MAX_CUR_30                     :  8; /* [15: 8],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_MAX_CURRENT_1;

    /* 0x4A : Maximum Current Register 2 */
    union {
        struct {
            uint16_t MAX_CUR_18                     :  8; /* [ 7: 0],     HwInit,        0x0 */
            uint16_t Reserved                       :  8; /* [15: 8],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_MAX_CURRENT_2;

    /* 0x4C : Maximum Current Register 3 */
    union {
        struct {
            uint16_t Reserved                       : 16; /* [15: 0],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_MAX_CURRENT_3;

    /* 0x4E : Maximum Current Register 4 */
    union {
        struct {
            uint16_t Reserved                       : 16; /* [15: 0],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_MAX_CURRENT_4;

    /* 0x50 : Force Event Auto cmd12 Error Register */
    union {
        struct {
            uint16_t F_ACMD12_NEXE_ERR              :  1; /* [    0],         WO,        0x0 */
            uint16_t F_ACMD_TO_ERR                  :  1; /* [    1],         WO,        0x0 */
            uint16_t F_ACMD_CRC_ERR                 :  1; /* [    2],         WO,        0x0 */
            uint16_t F_ACMD_EBIT_ERR                :  1; /* [    3],         WO,        0x0 */
            uint16_t F_ACMD_INDEX_ERR               :  1; /* [    4],         WO,        0x0 */
            uint16_t Reserved_6_5                   :  2; /* [ 6: 5],       RSVD,        0x0 */
            uint16_t F_ACMD12_ISSUE_ERR             :  1; /* [    7],         WO,        0x0 */
            uint16_t Reserved_15_8                  :  8; /* [15: 8],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_FORCE_EVENT_AUTO_CMD12_ERROR;

    /* 0x52 : Force Event for Error Status Register */
    union {
        struct {
            uint16_t F_CMD_TO_ERR                   :  1; /* [    0],         WO,        0x0 */
            uint16_t F_CMD_CRC_ERR                  :  1; /* [    1],         WO,        0x0 */
            uint16_t F_CMD_END_BIT_ERR              :  1; /* [    2],         WO,        0x0 */
            uint16_t F_CMD_INDEX_ERR                :  1; /* [    3],         WO,        0x0 */
            uint16_t F_DAT_TO_ERR                   :  1; /* [    4],         WO,        0x0 */
            uint16_t F_DAT_CRC_ERR                  :  1; /* [    5],         WO,        0x0 */
            uint16_t F_DAT_END_BIT_ERR              :  1; /* [    6],         WO,        0x0 */
            uint16_t F_CURRENT_ERR                  :  1; /* [    7],         WO,        0x0 */
            uint16_t F_ACMD12_ERR                   :  1; /* [    8],         WO,        0x0 */
            uint16_t F_ADMA_ERR                     :  1; /* [    9],         WO,        0x0 */
            uint16_t Reserved                       :  2; /* [11:10],       RSVD,        0x0 */
            uint16_t F_VENDOR_SPECIFIC_ERR          :  4; /* [15:12],         WO,        0x1 */
        }BF;
        uint16_t SHORT;
    } SD_FORCE_EVENT_FOR_ERROR_STATUS;

    /* 0x54 : ADMA Error Status Register */
    union {
        struct {
            uint16_t ADMA_STATE                     :  2; /* [ 1: 0],         RW,        0x0 */
            uint16_t ADMA_LEN_ERR                   :  1; /* [    2],         RW,        0x0 */
            uint16_t Reserved                       :  5; /* [ 7: 3],       RSVD,        0x0 */
            uint16_t reserved_8_15                  :  8; /* [15: 8],       rsvd,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ADMA_ERROR_STATUS;

    /* 0x58  reserved */
    uint8_t RESERVED0x58[2];

    /* 0x58 : ADMA System Address Register 1 */
    union {
        struct {
            uint16_t ADMA_SYS_ADDR                  : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ADMA_SYS_ADDR_1;

    /* 0x5A : ADMA System Address Register 2 */
    union {
        struct {
            uint16_t ADMA_SYS_ADDR                  : 16; /* [15: 0],         RW,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ADMA_SYS_ADDR_2;

    /* 0x5C : ADMA System Address Register 3 */
    union {
        struct {
            uint16_t Reserved                       : 16; /* [15: 0],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ADMA_SYS_ADDR_3;

    /* 0x5E : ADMA System Address Register 4 */
    union {
        struct {
            uint16_t Reserved                       : 16; /* [15: 0],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_ADMA_SYS_ADDR_4;

    /* 0x60 : Preset Value Register for Initialization */
    union {
        struct {
            uint16_t SDCLK_FREQ_SEL_VAL             : 10; /* [ 9: 0],     HwInit,        0x0 */
            uint16_t CLKGEN_SEL_VAL                 :  1; /* [   10],     HwInit,        0x0 */
            uint16_t Reserved                       :  3; /* [13:11],       RSVD,        0x0 */
            uint16_t DRV_STRENGTH_VAL               :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } PRESET_VALUE_FOR_INIT;

    /* 0x62 : Preset Value Register for Default Speed */
    union {
        struct {
            uint16_t SDCLK_FREQ_SEL_VAL             : 10; /* [ 9: 0],     HwInit,        0x0 */
            uint16_t CLKGEN_SEL_VAL                 :  1; /* [   10],     HwInit,        0x0 */
            uint16_t Reserved                       :  3; /* [13:11],       RSVD,        0x0 */
            uint16_t DRV_STRENGTH_VAL               :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } PRESET_VALUE_FOR_DS;

    /* 0x64 : Preset Value Register for High Speed */
    union {
        struct {
            uint16_t SDCLK_FREQ_SEL_VAL             : 10; /* [ 9: 0],     HwInit,        0x0 */
            uint16_t CLKGEN_SEL_VAL                 :  1; /* [   10],     HwInit,        0x0 */
            uint16_t Reserved                       :  3; /* [13:11],       RSVD,        0x0 */
            uint16_t DRV_STRENGTH_VAL               :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } PRESET_VALUE_FOR_HS;

    /* 0x66 : Preset Value Register for SDR12 */
    union {
        struct {
            uint16_t SDCLK_FREQ_SEL_VAL             : 10; /* [ 9: 0],     HwInit,        0x0 */
            uint16_t CLKGEN_SEL_VAL                 :  1; /* [   10],     HwInit,        0x0 */
            uint16_t Reserved                       :  3; /* [13:11],       RSVD,        0x0 */
            uint16_t DRV_STRENGTH_VAL               :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } PRESET_VALUE_FOR_SDR12;

    /* 0x68 : Preset Value Register for SDR25 */
    union {
        struct {
            uint16_t SDCLK_FREQ_SEL_VAL             : 10; /* [ 9: 0],     HwInit,        0x0 */
            uint16_t CLKGEN_SEL_VAL                 :  1; /* [   10],     HwInit,        0x0 */
            uint16_t Reserved                       :  3; /* [13:11],       RSVD,        0x0 */
            uint16_t DRV_STRENGTH_VAL               :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } PRESET_VALUE_FOR_SDR25;

    /* 0x6a : Preset Value Register for SDR50 */
    union {
        struct {
            uint16_t SDCLK_FREQ_SEL_VAL             : 10; /* [ 9: 0],     HwInit,        0x0 */
            uint16_t CLKGEN_SEL_VAL                 :  1; /* [   10],     HwInit,        0x0 */
            uint16_t Reserved                       :  3; /* [13:11],       RSVD,        0x0 */
            uint16_t DRV_STRENGTH_VAL               :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } PRESET_VALUE_FOR_SDR50;

    /* 0x6c : Preset Value Register for SDR104 */
    union {
        struct {
            uint16_t SDCLK_FREQ_SEL_VAL             : 10; /* [ 9: 0],     HwInit,        0x0 */
            uint16_t CLKGEN_SEL_VAL                 :  1; /* [   10],     HwInit,        0x0 */
            uint16_t Reserved                       :  3; /* [13:11],       RSVD,        0x0 */
            uint16_t DRV_STRENGTH_VAL               :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } PRESET_VALUE_FOR_SDR104;

    /* 0x6e : Preset Value Register for DDR50 */
    union {
        struct {
            uint16_t SDCLK_FREQ_SEL_VAL             : 10; /* [ 9: 0],     HwInit,        0x0 */
            uint16_t CLKGEN_SEL_VAL                 :  1; /* [   10],     HwInit,        0x0 */
            uint16_t Reserved                       :  3; /* [13:11],       RSVD,        0x0 */
            uint16_t DRV_STRENGTH_VAL               :  2; /* [15:14],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } PRESET_VALUE_FOR_DDR50;

    /* 0x70  reserved */
    uint8_t RESERVED0x70[112];

    /* 0xe0 : Shared Bus Control Register */
    union {
        struct {
            uint32_t NUM_CLK_PINS                   :  3; /* [ 2: 0],     HwInit,        0x0 */
            uint32_t Reserved_3                     :  1; /* [    3],       RSVD,        0x0 */
            uint32_t NUM_INT_PINS                   :  2; /* [ 5: 4],     HwInit,        0x0 */
            uint32_t Reserved_7_6                   :  2; /* [ 7: 6],       RSVD,        0x0 */
            uint32_t BUS_WIDTH_PRESET               :  7; /* [14: 8],     HwInit,        0x0 */
            uint32_t Reserved_15                    :  1; /* [   15],       RSVD,        0x0 */
            uint32_t CLK_PIN_SEL                    :  3; /* [18:16],         RW,        0x0 */
            uint32_t Reserved_19                    :  1; /* [   19],       RSVD,        0x0 */
            uint32_t INT_PIN_SEL                    :  3; /* [22:20],         RW,        0x0 */
            uint32_t Reserved_23                    :  1; /* [   23],       RSVD,        0x0 */
            uint32_t BEND_PWR_CTRL                  :  7; /* [30:24],         RW,        0x0 */
            uint32_t Reserved_31                    :  1; /* [   31],       RSVD,        0x0 */
        }BF;
        uint32_t WORD;
    } SHARED_BUS_CTRL;

    /* 0xe4  reserved */
    uint8_t RESERVED0xe4[8];

    /* 0xEC : SD_AHB_BURST_SIZE */
    union {
        struct {
            uint16_t AHB_BURST_SIZE                 :  7; /* [ 6: 0],         RW,        0x7 */
            uint16_t Reserved_15_7                  :  9; /* [15: 7],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_AHB_BURST_SIZE;

    /* 0xee  reserved */
    uint8_t RESERVED0xee[14];

    /* 0xFC : Slot Interrupt Status Register */
    union {
        struct {
            uint16_t SLOT_INT                       :  8; /* [ 7: 0],        ROC,        0x0 */
            uint16_t Reserved_15_8                  :  8; /* [15: 8],       RSVD,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_SLOT_INT_STATUS;

    /* 0xFE : Host Control Version Register */
    union {
        struct {
            uint16_t SPECIFICATION_VER_NUM          :  8; /* [ 7: 0],     HwInit,        0x0 */
            uint16_t VENDOR_VER                     :  8; /* [15: 8],     HwInit,        0x0 */
        }BF;
        uint16_t SHORT;
    } SD_HOST_CTRL_VER;

};

typedef volatile struct sdh_reg sdh_reg_t;


#endif  /* __SDH_REG_H__ */
