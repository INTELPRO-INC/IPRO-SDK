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

#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <hal_spi.h>
#include <hal_dma.h>
#include <camera.h>
#include <sccb.h>
#include "hal_mjpeg.h"

#define SPI_CAM_YUV_BUF_NUM     (2)
#define SPI_CAM_FRAME_BUFFER

#ifdef SPI_CAM_FRAME_BUFFER
#define MJENC_HEIGHT_UNIT (240)
#define SPICAM_DATA_BLOCK_CNT   (240/MJENC_HEIGHT_UNIT)
#define SPICAM_YUV_BLOCK_SIZE   (240*240*2)
#else
#define MJENC_HEIGHT_UNIT (8)
#define SPICAM_DATA_BLOCK_CNT   (240/MJENC_HEIGHT_UNIT)
#define SPICAM_YUV_BLOCK_SIZE   (240*8*2)
#endif

enum {
    SPICAM_STATE_WAIT_FRAME_HEADER = 0,
    SPICAM_STATE_WAIT_FRAME_DATA_BLOCK,
    SPICAM_STATE_WAIT_FRAME_DATA_BLOCK_END = SPICAM_DATA_BLOCK_CNT,
    SPICAM_STATE_MAX,
};

typedef struct {
    uint8_t frameHdrBuf[9];
    uint32_t frameBufAddr[SPI_CAM_YUV_BUF_NUM];
    uint32_t frameBufLen;
    uint32_t dummyBuf[4];
} spi_cam_dma_buf_info_t;

typedef struct{
    uint32_t len;
    uint8_t *data;
    uint16_t times;
    uint8_t block_index;
    uint8_t use_index;
}queue_data_t;


static uint8_t gSPICamState = SPICAM_STATE_WAIT_FRAME_HEADER;
static spi_cam_dma_buf_info_t spiCamBuf;
static xQueueHandle cam_data_queue;
static int gCurBlockProcessed = 0;
static uint32_t gCamFrameNum = 0;
static uint32_t gPrevTimeMS = 0;
static int gCurFrmHdrIdx = 0;
static SemaphoreHandle_t gJencNotify;

#define MAX_LLI_FRAME_SIZE  (4064)
#ifdef SPI_CAM_FRAME_BUFFER
static void spi_dma_cb(void *dev, uint32_t flag)
{
    queue_data_t queue_data;
    uint32_t tick_now;

    switch (gSPICamState) {
        case SPICAM_STATE_WAIT_FRAME_HEADER:
        {
            MInvalDCacheRange((uint32_t)spiCamBuf.frameHdrBuf, 9);
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n", spiCamBuf.frameHdrBuf[0], spiCamBuf.frameHdrBuf[1],
                spiCamBuf.frameHdrBuf[2], spiCamBuf.frameHdrBuf[3],
                spiCamBuf.frameHdrBuf[4], spiCamBuf.frameHdrBuf[5], spiCamBuf.frameHdrBuf[6], spiCamBuf.frameHdrBuf[7], spiCamBuf.frameHdrBuf[8]);
            uint32_t pkt_start = *((uint32_t *) spiCamBuf.frameHdrBuf);
            if (unlikely(pkt_start != 0x01FFFFFF)) {
                printf("Invalid packet %02x %02x %02x %02x\r\n", spiCamBuf.frameHdrBuf[0], spiCamBuf.frameHdrBuf[1],
                    spiCamBuf.frameHdrBuf[2], spiCamBuf.frameHdrBuf[3]);
                while(1);
            } else {
                tick_now = xTaskGetTickCountFromISR();
                gSPICamState = SPICAM_STATE_WAIT_FRAME_DATA_BLOCK;
                gCurBlockProcessed = 0;
                gCamFrameNum++;
                printf("[FPS: %lu] Valid frame no %lu sync start, image: %d x %d\t\n", 1000 / (tick_now - gPrevTimeMS), gCamFrameNum,
                        (spiCamBuf.frameHdrBuf[6] << 8) | spiCamBuf.frameHdrBuf[5],
                        (spiCamBuf.frameHdrBuf[8] << 8) | spiCamBuf.frameHdrBuf[7]);
                gPrevTimeMS = tick_now;
            }
            break;
        }
        case SPICAM_STATE_WAIT_FRAME_DATA_BLOCK:
            gCurBlockProcessed++;
            if (gCurBlockProcessed == SPICAM_STATE_WAIT_FRAME_DATA_BLOCK_END)
                gSPICamState = SPICAM_STATE_WAIT_FRAME_HEADER;
            queue_data.block_index = gCurFrmHdrIdx;
            queue_data.len = SPICAM_YUV_BLOCK_SIZE;
//            printf("spiCamBuf.frameBufAddr[%d] = 0x%lx, queue_data.data = %p\r\n", gCurFrmHdrIdx, spiCamBuf.frameBufAddr[gCurFrmHdrIdx], queue_data.data);
            if (unlikely(xQueueSendFromISR(cam_data_queue, &queue_data, NULL) == pdFALSE)) {
                printf("YUV to MJPEG buffer full!\r\n");
            }
            gCurFrmHdrIdx = (gCurFrmHdrIdx + 1) % SPI_CAM_YUV_BUF_NUM;
            break;
        default:
            break;
    }
}

static int spi_cam_lli_list_init(dma_lli_ctrl_t **pprxlli, spi_cam_dma_buf_info_t *rx_buf_info)
{
    uint32_t lli_idx = 0;
    uint8_t lli_buffer_idx = 0;
    dma_control_data_t dmactrl;

    memset(&dmactrl, 0, sizeof(dmactrl));

    dmactrl.bits.SBSize = DMA_BURST_INCR1;
    dmactrl.bits.DBSize = DMA_BURST_INCR16;
    dmactrl.bits.SWidth = DMA_TRANSFER_WIDTH_8BIT;
    dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_8BIT;
    dmactrl.bits.Prot = 0;
    dmactrl.bits.SLargerD = 0;
    dmactrl.bits.dst_add_mode = DISABLE;
    dmactrl.bits.dst_min_mode = DISABLE;
    dmactrl.bits.fix_cnt = 0;

    *pprxlli = (dma_lli_ctrl_t *)pvPortMalloc(sizeof(dma_lli_ctrl_t) * (1 + 5 + 240*2 + 5 + 1)*2); // Frame Header(1) + Frame Data(3*16)*15 + Frame End(1)
    //*pprxlli = (dma_lli_ctrl_t *)0x21010000;
    //*pprxlli = pvPortMalloc(sizeof(DMA_LLI_Ctrl_Type) * 40); // Frame Header(1) + Frame Data(3*16)*15 + Frame End(1)
    if (*pprxlli == NULL) {
        printf("malloc spi_cam lli failed. \r\n");
        return -1;
    }

    for (int i = 0; i < 2; i++) {
        // Frame Header
        dmactrl.bits.TransferSize = 9; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
        dmactrl.bits.I = 1;
        dmactrl.bits.DBSize = DMA_BURST_INCR16;
        dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_8BIT;
        dmactrl.bits.dst_add_mode = DISABLE;
        dmactrl.bits.dst_min_mode = DISABLE;
        dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
        dmactrl.bits.DI = DMA_ADDR_INCREMENT_ENABLE;
        (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
        (*pprxlli)[lli_idx].dst_addr = (uint32_t)rx_buf_info->frameHdrBuf;
        (*pprxlli)[lli_idx].cfg = dmactrl;
        (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
        lli_idx++;

        // line 1 - 40
        int remain_size = (6+6+240*2)*40;
        while (remain_size > 0) {
            if (remain_size > MAX_LLI_FRAME_SIZE)
                dmactrl.bits.TransferSize = MAX_LLI_FRAME_SIZE; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
            else
                dmactrl.bits.TransferSize = remain_size; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
            remain_size -= dmactrl.bits.TransferSize;
            dmactrl.bits.I = 0;
            dmactrl.bits.DBSize = DMA_BURST_INCR4;
            dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
            dmactrl.bits.dst_add_mode = DISABLE;
            dmactrl.bits.dst_min_mode = DISABLE;
            dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
            (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
            (*pprxlli)[lli_idx].dst_addr = (uint32_t)&rx_buf_info->dummyBuf;
            (*pprxlli)[lli_idx].cfg = dmactrl;
            (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
            lli_idx++;
        }

        // line 41 - 280
        // Frame Data (8 line x 30 = 240 in height)
        // get 240 pixel from 320 for width
        for (int h = 0; h < 240; h++) {
            dmactrl.bits.TransferSize = 12;
            dmactrl.bits.I = 0;
            dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DBSize = DMA_BURST_INCR4;
            dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
            dmactrl.bits.dst_add_mode = DISABLE;
            dmactrl.bits.dst_min_mode = DISABLE;

            (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
            (*pprxlli)[lli_idx].dst_addr = (uint32_t)&rx_buf_info->dummyBuf;
            (*pprxlli)[lli_idx].cfg = dmactrl;
            (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
            lli_idx++;

            // Active Data first 240 pixel yuv422 = 480 bytes
            dmactrl.bits.TransferSize = 480;
            if (h == 239) // Raise Interrupt when 240*16 frame data is received
                dmactrl.bits.I = 1;
            else
                dmactrl.bits.I = 0;
            dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DI = DMA_ADDR_INCREMENT_ENABLE;
            dmactrl.bits.DBSize = DMA_BURST_INCR4;
            dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
            dmactrl.bits.dst_add_mode = DISABLE;
            dmactrl.bits.dst_min_mode = DISABLE;

            (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
            (*pprxlli)[lli_idx].dst_addr = rx_buf_info->frameBufAddr[lli_buffer_idx] + h*480;
            (*pprxlli)[lli_idx].cfg = dmactrl;
            (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
            lli_idx++;
        }

        // line 281 - 320
        remain_size = (6+6+240*2)*40;
        while (remain_size > 0) {
            if (remain_size > MAX_LLI_FRAME_SIZE)
                dmactrl.bits.TransferSize = MAX_LLI_FRAME_SIZE; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
            else
                dmactrl.bits.TransferSize = remain_size; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
            remain_size -= dmactrl.bits.TransferSize;
            dmactrl.bits.I = 0;
            dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DBSize = DMA_BURST_INCR4;
            dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
            dmactrl.bits.dst_add_mode = DISABLE;
            dmactrl.bits.dst_min_mode = DISABLE;

            (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
            (*pprxlli)[lli_idx].dst_addr = (uint32_t)&rx_buf_info->dummyBuf;
            (*pprxlli)[lli_idx].cfg = dmactrl;
            (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
            lli_idx++;
        }


        // Frame End
        dmactrl.bits.TransferSize = 4; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
        dmactrl.bits.I = 0;
        dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
        dmactrl.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
        dmactrl.bits.DBSize = DMA_BURST_INCR16;
        dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
        dmactrl.bits.dst_add_mode = DISABLE;
        dmactrl.bits.dst_min_mode = DISABLE;

        (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
        (*pprxlli)[lli_idx].dst_addr = (uint32_t)&rx_buf_info->dummyBuf;
        (*pprxlli)[lli_idx].cfg = dmactrl;
        if (i == 0)
            (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
        else
            (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[0];
        lli_idx++;

        lli_buffer_idx = lli_buffer_idx + 1;
    }

    MFlushDCacheRange((uint32_t)*pprxlli, sizeof(dma_lli_ctrl_t) * (1 + 5 + 240*2 + 5 + 1)*2);
    return 0;
}
#else // #ifdef SPI_CAM_FRAME_BUFFER
static void spi_dma_cb(void *dev, uint32_t flag)
{
    uint32_t tick_now;

    switch (gSPICamState) {
        case SPICAM_STATE_WAIT_FRAME_HEADER:
        {
            MInvalDCacheRange((uint32_t)spiCamBuf.frameHdrBuf, 9);
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n", spiCamBuf.frameHdrBuf[0], spiCamBuf.frameHdrBuf[1], 
                spiCamBuf.frameHdrBuf[2], spiCamBuf.frameHdrBuf[3]
                , spiCamBuf.frameHdrBuf[4], spiCamBuf.frameHdrBuf[5], spiCamBuf.frameHdrBuf[6], spiCamBuf.frameHdrBuf[7], spiCamBuf.frameHdrBuf[8]);
            uint32_t pkt_start = *((uint32_t *) spiCamBuf.frameHdrBuf);
            if (unlikely(pkt_start != 0x01FFFFFF)) {
                printf("Invalid packet %02x %02x %02x %02x\r\n", spiCamBuf.frameHdrBuf[0], spiCamBuf.frameHdrBuf[1],
                    spiCamBuf.frameHdrBuf[2], spiCamBuf.frameHdrBuf[3]);
                while(1);
            } else {
                tick_now = xTaskGetTickCountFromISR();
                gSPICamState = SPICAM_STATE_WAIT_FRAME_DATA_BLOCK;
                gCurBlockProcessed = 0;
                gCamFrameNum++;
                printf("[FPS: %lu] Valid frame no %lu sync start, image: %d x %d\t\n", 1000 / (tick_now - gPrevTimeMS), gCamFrameNum, 
                        (spiCamBuf.frameHdrBuf[6] << 8) | spiCamBuf.frameHdrBuf[5], 
                        (spiCamBuf.frameHdrBuf[8] << 8) | spiCamBuf.frameHdrBuf[7]);
                gPrevTimeMS = tick_now;
            }
            break;
        }
        case SPICAM_STATE_WAIT_FRAME_DATA_BLOCK:
            gCurBlockProcessed++;
            if (gCurBlockProcessed == SPICAM_STATE_WAIT_FRAME_DATA_BLOCK_END)
                gSPICamState = SPICAM_STATE_WAIT_FRAME_HEADER;
            break;
        default:
            break;
    }
}

static int spi_cam_lli_list_init(dma_lli_ctrl_t **pprxlli, spi_cam_dma_buf_info_t *rx_buf_info)
{
    uint32_t lli_idx = 0;
    uint8_t lli_buffer_idx = 0;
    dma_control_data_t dmactrl;

    memset(&dmactrl, 0, sizeof(dmactrl));

    dmactrl.bits.SBSize = DMA_BURST_INCR1;
    dmactrl.bits.DBSize = DMA_BURST_INCR16;
    dmactrl.bits.SWidth = DMA_TRANSFER_WIDTH_8BIT;
    dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_8BIT;
    dmactrl.bits.Prot = 0;
    dmactrl.bits.SLargerD = 0;
    dmactrl.bits.dst_add_mode = DISABLE;
    dmactrl.bits.dst_min_mode = DISABLE;
    dmactrl.bits.fix_cnt = 0;

    //*pprxlli = (dma_lli_ctrl_t *)pvPortMalloc(sizeof(dma_lli_ctrl_t) * (1 + 5 + 240*2 + 5 + 1)); // Frame Header(1) + Frame Data(3*16)*15 + Frame End(1)
    *pprxlli = (dma_lli_ctrl_t *)0x21010000;
    //*pprxlli = pvPortMalloc(sizeof(DMA_LLI_Ctrl_Type) * 40); // Frame Header(1) + Frame Data(3*16)*15 + Frame End(1)
    if (*pprxlli == NULL) {
        printf("malloc spi_cam lli failed. \r\n");
        return -1;
    }

    // Frame Header
    dmactrl.bits.TransferSize = 9; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
    dmactrl.bits.I = 1;
    dmactrl.bits.DBSize = DMA_BURST_INCR16;
    dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_8BIT;
    dmactrl.bits.dst_add_mode = DISABLE;
    dmactrl.bits.dst_min_mode = DISABLE;
    dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
    dmactrl.bits.DI = DMA_ADDR_INCREMENT_ENABLE;
    (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
    (*pprxlli)[lli_idx].dst_addr = (uint32_t)rx_buf_info->frameHdrBuf;
    (*pprxlli)[lli_idx].cfg = dmactrl;
    (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
    lli_idx++;
#if 0 // test for frame header only
    int remain_size = (6+6+240*2)*320 + 4;
    while (remain_size > 0) {
        if (remain_size > 4095)
            dmactrl.TransferSize = 4095; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
        else
            dmactrl.TransferSize = remain_size; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
        remain_size -= dmactrl.TransferSize;
        dmactrl.I = 0;
        dmactrl.SI = DMA_ADDR_INCREMENT_DISABLE;
        dmactrl.DI = DMA_ADDR_INCREMENT_DISABLE;
        (*pprxlli)[lli_idx].srcDmaAddr = (uint32_t)DMA_ADDR_SPI_RDR;
        (*pprxlli)[lli_idx].destDmaAddr = (uint32_t)&rx_buf_info->dummyBuf;
        (*pprxlli)[lli_idx].dmaCtrl = dmactrl;
        (*pprxlli)[lli_idx].nextLLI = (uint32_t)&(*pprxlli)[lli_idx+1];
        lli_idx++;
    }
    (*pprxlli)[lli_idx - 1].nextLLI = (uint32_t)&(*pprxlli)[0];
    printf("lli_idx = %ld\r\n", lli_idx);
#else

    // line 1 - 40
    int remain_size = (6+6+240*2)*40;
    while (remain_size > 0) {
        if (remain_size > MAX_LLI_FRAME_SIZE)
            dmactrl.bits.TransferSize = MAX_LLI_FRAME_SIZE; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
        else
            dmactrl.bits.TransferSize = remain_size; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
        remain_size -= dmactrl.bits.TransferSize;
        dmactrl.bits.I = 0;
        dmactrl.bits.DBSize = DMA_BURST_INCR4;
        dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
        dmactrl.bits.dst_add_mode = DISABLE;
        dmactrl.bits.dst_min_mode = DISABLE;
        dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
        dmactrl.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
        (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
        (*pprxlli)[lli_idx].dst_addr = (uint32_t)&rx_buf_info->dummyBuf;
        (*pprxlli)[lli_idx].cfg = dmactrl;
        (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
        lli_idx++;
    }
    
    // line 41 - 280
    // Frame Data (8 line x 30 = 240 in height)
    // get 240 pixel from 320 for width
    for (int h_block = 0; h_block < 30; h_block++) {
        for (int h = 0; h < 8; h++) {
            // Sync Code[23:0] 8�耠40 Data_size[7:0] Data_size[15:8]
            dmactrl.bits.TransferSize = 12;
            dmactrl.bits.I = 0;
            dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DBSize = DMA_BURST_INCR4;
            dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
            dmactrl.bits.dst_add_mode = DISABLE;
            dmactrl.bits.dst_min_mode = DISABLE;

            (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
            (*pprxlli)[lli_idx].dst_addr = (uint32_t)&rx_buf_info->dummyBuf;
            (*pprxlli)[lli_idx].cfg = dmactrl;
            (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
            lli_idx++;

            // Active Data first 240 pixel yuv422 = 480 bytes
            dmactrl.bits.TransferSize = 480; // Sync Code[23:0] 8�耠40 Data_size[7:0] Data_size[15:8]
            if (h == 7) // Raise Interrupt when 240*16 frame data is received
                dmactrl.bits.I = 1;
            else
                dmactrl.bits.I = 0;
            dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
            dmactrl.bits.DI = DMA_ADDR_INCREMENT_ENABLE;
            dmactrl.bits.DBSize = DMA_BURST_INCR4;
            dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
            dmactrl.bits.dst_add_mode = DISABLE;
            dmactrl.bits.dst_min_mode = DISABLE;

            (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
            (*pprxlli)[lli_idx].dst_addr = rx_buf_info->frameBufAddr[lli_buffer_idx] + h*480;
            (*pprxlli)[lli_idx].cfg = dmactrl;
            (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
            lli_idx++;
        }
        lli_buffer_idx = (lli_buffer_idx + 1) % SPI_CAM_YUV_BUF_NUM;
    }

    // line 281 - 320
    remain_size = (6+6+240*2)*40;
    while (remain_size > 0) {
        if (remain_size > MAX_LLI_FRAME_SIZE)
            dmactrl.bits.TransferSize = MAX_LLI_FRAME_SIZE; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
        else
            dmactrl.bits.TransferSize = remain_size; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
        remain_size -= dmactrl.bits.TransferSize;
        dmactrl.bits.I = 0;
        dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
        dmactrl.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
        dmactrl.bits.DBSize = DMA_BURST_INCR4;
        dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
        dmactrl.bits.dst_add_mode = DISABLE;
        dmactrl.bits.dst_min_mode = DISABLE;

        (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
        (*pprxlli)[lli_idx].dst_addr = (uint32_t)&rx_buf_info->dummyBuf;
        (*pprxlli)[lli_idx].cfg = dmactrl;
        (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[lli_idx+1];
        lli_idx++;
    }


    // Frame End
    dmactrl.bits.TransferSize = 4; //Sync Code[23:0] 8�耠01 Data_id[7:0] Image_width[7:0] Image_width[15:8] Image_height[7:0] Image_height[15:8]
    dmactrl.bits.I = 0;
    dmactrl.bits.SI = DMA_ADDR_INCREMENT_DISABLE;
    dmactrl.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
    dmactrl.bits.DBSize = DMA_BURST_INCR16;
    dmactrl.bits.DWidth = DMA_TRANSFER_WIDTH_32BIT;
    dmactrl.bits.dst_add_mode = DISABLE;
    dmactrl.bits.dst_min_mode = DISABLE;

    (*pprxlli)[lli_idx].src_addr = (uint32_t)DMA_ADDR_SPI_RDR;
    (*pprxlli)[lli_idx].dst_addr = (uint32_t)&rx_buf_info->dummyBuf;
    (*pprxlli)[lli_idx].cfg = dmactrl;
    (*pprxlli)[lli_idx].nextlli = (uint32_t)&(*pprxlli)[0];
    lli_idx++;
#endif
    MFlushDCacheRange((uint32_t)*pprxlli, sizeof(dma_lli_ctrl_t) * (1 + 5 + 240*2 + 5 + 1));
    return 0;
}
#endif

static void mjpeg_enc_event_cb(uint32_t event)
{
    if (event == MJENC_HAL_INT_NORMAL)
        xSemaphoreGiveFromISR(gJencNotify, NULL);
    else
        printf("Unknown jenc event %lu\r\n", event);
}

static void spi_cam_proc(void *argv)
{
    dma_lli_ctrl_t *spi_cam_lli;
    queue_data_t queue_data;
    struct mjpeg_hal_cfg_t mjpeg_cfg;
    uint32_t jenc_size, jenc_addr;
    static uint8_t jpg_head_buf[800] = { 0 };

    spi_device_t spi_dev = {
        .caps = 0,
        .id = SPI0_INDEX,
        .clk = 24000000,
        .mode = SPI_MASTER_MODE,
        .direction = SPI_LSB_BYTE0_DIRECTION_FIRST,
        .clk_polaraity = SPI_POLARITY_HIGH,
        .clk_phase = SPI_PHASE_1EDGE,
        .datasize = SPI_DATASIZE_8BIT,
        .fifo_threshold = 0,
        .pin_swap_enable = 0, /*swap mosi and miso*/
        .ignore_cs = 1,
        .delitch_cnt = 0,
        .callback = NULL,
    };
    dma_device_t dma_dev = {
        .id = DMA0_ID,
        .ch = DMA0_CH0_INDEX,
        .transfer_mode = DMA_LLI_ONCE_MODE,
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_SPI0_RX,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR1,
        .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_8BIT,
        .dst_width = DMA_TRANSFER_WIDTH_8BIT,
        .lli_cfg = NULL,
        .callback = spi_dma_cb,
    };
    memset(&spiCamBuf, 0, sizeof(spiCamBuf));
    spiCamBuf.frameBufLen = SPICAM_YUV_BLOCK_SIZE;
    spiCamBuf.dummyBuf[0] = 0xFFFFFFFF;
    spiCamBuf.dummyBuf[1] = 0xFFFFFFFF;
    spiCamBuf.dummyBuf[2] = 0xFFFFFFFF;
    spiCamBuf.dummyBuf[3] = 0xFFFFFFFF;
    cam_data_queue = xQueueCreate(4, sizeof(queue_data_t));

    for (int i = 0; i < SPI_CAM_YUV_BUF_NUM; i++) {
        spiCamBuf.frameBufAddr[i] = (uint32_t)pvPortMalloc(spiCamBuf.frameBufLen + 15) & 0xFFFFFFF0;
        printf("spiCamBuf.frameBufAddr[%d] = 0x%lx\r\n", i, spiCamBuf.frameBufAddr[i]);
        if (spiCamBuf.frameBufAddr[i] == 0) {
            printf("Cannot allocate frame buffer %d\r\n", i);
            configASSERT(0);
        }
    }

    spi_cam_lli_list_init(&spi_cam_lli, &spiCamBuf);
    dma_setConfig(&dma_dev);
    dma_init(DMA0_ID, dma_dev.ch);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_CHANNEL_LLI_UPDATE, (void *)spi_cam_lli);
    dma_channel_start(DMA0_ID, DMA0_CH0_INDEX);

    //TODO: enable_24mhz

    SCCB_Init();
    printf("camera module init!!!\r\n");
    const rt_camera_desc *desc = camera_module_init();
    if (NULL == desc) {
        vTaskDelete(NULL);
    }

    jenc_size = 512*1024;
    jenc_addr = (uint32_t) pvPortMalloc( jenc_size );
    if (!jenc_addr) {
        printf("%s[%d] error: memory malloc failed! (size: %lu)\r\n", __func__, __LINE__, jenc_size);
        vTaskDelete(NULL);
    }

    mjpeg_cfg.format = MJPEG_HAL_FORMAT_YUV422_UYVY;
    mjpeg_cfg.quality = MJPEG_DEFAULT_QUALITY;
    mjpeg_cfg.rows = 240;
    mjpeg_cfg.resolution_x = 240;
    mjpeg_cfg.resolution_y = 240;
    mjpeg_cfg.input_bufaddr0 = 0;
    mjpeg_cfg.input_bufaddr1 = 0;
    mjpeg_cfg.output_bufaddr = jenc_addr;
    mjpeg_cfg.output_bufsize = jenc_size;
    mjpeg_cfg.input_yy_table = NULL;
    mjpeg_cfg.input_uv_table = NULL;
    mjpeg_cfg.output_swap_mode_en = 0;    /*enable encode output buffer swap mode, 0 frame mode, 1 swap mode.*/
    mjpeg_cfg.callback = mjpeg_enc_event_cb;

    mjpeg_init(&mjpeg_cfg);
    gJencNotify = xSemaphoreCreateCounting(1, 0);

    uint32_t jpg_head_len = mjpeg_headerCreate(YUV_MODE_422, MJPEG_DEFAULT_QUALITY, 240, 240, jpg_head_buf);
    mjpeg_fill_header_tail(jpg_head_buf, jpg_head_len);


    spi_dev.mode = SPI_SLAVE_MODE;
    spi_dev.caps = COMMON_CAPS_DMA_RX;
    spi_setConfig(&spi_dev);
    spi_init(SPI0_INDEX);
    spi_enable(SPI0_INDEX, 1);

    camera_module_start(desc);

    uint8_t *spiCamYuv = pvPortMalloc(240*240*2);
    int count = 0;
    uint32_t yuv_linebuffer_addr = 0;
    uint32_t jpg_addr = 0, jpg_len = 0, jpg_sec_half_addr = 0, jpg_sec_half_len = 0, jpg_total_len=0;

    while(1) {
        xQueueReceive(cam_data_queue, &queue_data, portMAX_DELAY);
        yuv_linebuffer_addr = spiCamBuf.frameBufAddr[queue_data.block_index];
        printf("yuv_linebuffer_addr = 0x%lx, %d\r\n", yuv_linebuffer_addr, queue_data.block_index);
        mjpeg_set_input_addr(MJPEG_HAL_SET_ADDR_Y, yuv_linebuffer_addr);
        mjpeg_set_mode(1, 1, 1);
        mjpeg_start(1);
        xSemaphoreTake(gJencNotify, portMAX_DELAY);
#if 1
        if (count++ == 30) {
            mjpeg_get_one_frame(&jpg_addr, &jpg_len, &jpg_sec_half_addr, &jpg_sec_half_len);
            MInvalDCacheRange(jpg_addr, jpg_len);
            memcpy(spiCamYuv, (void *)jpg_addr, jpg_len);
            if(jpg_sec_half_len != 0)
            {
                MInvalDCacheRange(jpg_sec_half_addr, jpg_sec_half_len);
                memcpy((void *)((uint32_t)spiCamYuv+jpg_len), (void *)jpg_sec_half_addr, jpg_sec_half_len);
            }

            jpg_total_len = jpg_len + jpg_sec_half_len;
            printf("dump binary memory test.jpg 0x%lx 0x%lx\r\n", (uint32_t)spiCamYuv, (uint32_t)spiCamYuv + jpg_total_len);
        }
#else
        printf("mjenc done\r\n");
#endif
    };

    vTaskDelete(NULL);
}

void cmd_spi_cam(char *buf, int len, int argc, char **argv)
{
    xTaskCreate(spi_cam_proc, "spi_cam", 1024, NULL, 10, NULL);
}


