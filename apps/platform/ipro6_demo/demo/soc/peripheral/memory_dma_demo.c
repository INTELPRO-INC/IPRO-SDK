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
#include <string.h>

#include "shell.h"
#include <FreeRTOS.h>
#include <hal_mtimer.h>
#include <task.h>
#include <hal_dma.h>

#define DRAM_ADDR         (0x1E000000)
#define DRAM_SIZE         (1 * 1024 * 1024)

#define DMA_TRANSFER_BYTE 4000

static uint32_t memoryAddr[] = { DRAM_ADDR };
static uint32_t memorySize[] = { DRAM_SIZE };
static uint32_t errorFlag = 0;

static int test_stop = 1;
static void cpu_rwtest_proc(void *arg)
{
    uint32_t i,j;
    uint64_t start_us;

    memoryAddr[0] = (uint32_t)malloc(DRAM_SIZE);
    while (!test_stop) {

        printf("CPU byte read/write:\r\n");
        start_us = mtimer_get_time_us();
        /* Write 0 */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i];j++){
                ((uint8_t*)(memoryAddr[i]))[j] = 0;
            }
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE(DRAM_ADDR, DRAM_SIZE);
        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i];j++){
                if(((uint8_t*)(memoryAddr[i]))[j] != 0){
                    printf("Error! Addr: 0x%08lx is 0x%02x, expect 0\r\n",memoryAddr[i]+j,((uint8_t*)(memoryAddr[i]))[j]);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error, takes: %llu us\r\n", mtimer_get_time_us() - start_us);
            start_us = mtimer_get_time_us();
        }else{
            errorFlag = 0;
        }

        /* Write value */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i];j++){
                ((uint8_t*)(memoryAddr[i]))[j] = j&0xff;
            }
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE(DRAM_ADDR, DRAM_SIZE);

        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i];j++){
                if(((uint8_t*)(memoryAddr[i]))[j] != (j&0xff)){
                    printf("Error! Addr: 0x%08lx is 0x%02x, expect 0x%02lx\r\n",memoryAddr[i]+j,((uint8_t*)(memoryAddr[i]))[j],j&0xff);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error, takes: %llu us\r\n", mtimer_get_time_us() - start_us);
            start_us = mtimer_get_time_us();
        }else{
            errorFlag = 0;
        }

        printf("CPU half word read/write:\r\n");

        /* Write 0 */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/2;j++){
                ((uint16_t*)(memoryAddr[i]))[j] = 0;
            }
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE(DRAM_ADDR, DRAM_SIZE);

        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/2;j++){
                if(((uint16_t*)(memoryAddr[i]))[j] != 0){
                    printf("Error! Addr: 0x%08lx is 0x%04x, expect 0\r\n",memoryAddr[i]+j*2,((uint16_t*)(memoryAddr[i]))[j]);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error, takes: %llu us\r\n", mtimer_get_time_us() - start_us);
            start_us = mtimer_get_time_us();
        }else{
            errorFlag = 0;
        }

        /* Write value */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/2;j++){
                ((uint16_t*)(memoryAddr[i]))[j] = j*0x21;
            }
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE(DRAM_ADDR, DRAM_SIZE);

        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/2;j++){
                if(((uint16_t*)(memoryAddr[i]))[j] != (j*0x21&0xffff)){
                    printf("Error! Addr: 0x%08lx is 0x%04x, expect 0x%04lx\r\n",memoryAddr[i]+j*2,((uint16_t*)(memoryAddr[i]))[j],j*0x21&0xffff);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error, takes: %llu us\r\n", mtimer_get_time_us() - start_us);
            start_us = mtimer_get_time_us();
        }else{
            errorFlag = 0;
        }

        printf("CPU word read/write:\r\n");

        /* Write 0 */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/4;j++){
                ((uint32_t*)(memoryAddr[i]))[j] = 0;
            }
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE(DRAM_ADDR, DRAM_SIZE);

        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/4;j++){
                if(((uint32_t*)(memoryAddr[i]))[j] != 0){
                    printf("Error! Addr: 0x%08lx is 0x%08lx, expect 0\r\n",memoryAddr[i]+j*4,((uint32_t*)(memoryAddr[i]))[j]);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error, takes: %llu us\r\n", mtimer_get_time_us() - start_us);
            start_us = mtimer_get_time_us();
        }else{
            errorFlag = 0;
        }

        /* Write value */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/4;j++){
                ((uint32_t*)(memoryAddr[i]))[j] = j*0x231;
            }
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE(DRAM_ADDR, DRAM_SIZE);

        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/4;j++){
                if(((uint32_t*)(memoryAddr[i]))[j] != j*0x231){
                    printf("Error! Addr: 0x%08lx is 0x%08lx, expect 0x%08lx\r\n",memoryAddr[i]+j*4,((uint32_t*)(memoryAddr[i]))[j],j*0x231);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error, takes: %llu us\r\n", mtimer_get_time_us() - start_us);
            start_us = mtimer_get_time_us();
        }else{
            errorFlag = 0;
        }
    }
    free((void *)memoryAddr[0]);
    vTaskDelete(NULL);
}

#define LLI_BUFF_SIZE       2048
volatile int dma_done = 0;
static void dma_copy_callback(void *dev, uint32_t flag)
{
    if (flag == HAL_DMA_INT_COMPLETE)
        dma_done = 1;
}
#if 0
static void DMA_memset(hosal_dma_chan_t dma_chan, uint32_t dstAddr, uint8_t val, uint32_t length)
{
    uint32_t i;
    uint32_t dstAddrTmp = dstAddr;
    DMA_LLI_Ctrl_Type *pstlli;
    uint32_t *llibuf;
    DMA_LLI_Cfg_Type llicfg;
    uint32_t count;
    int32_t remainder;

    count = length / LLI_BUFF_SIZE;
    remainder = length % LLI_BUFF_SIZE;

    if (remainder != 0) {
        count = count + 1;
    }

    pstlli = pvPortMalloc(sizeof(DMA_LLI_Ctrl_Type) * count);
    if (NULL == pstlli) {
        printf("malloc lli failed. \r\n");
        return;
    }

    llicfg.dir = DMA_TRNS_M2M;
    llicfg.srcPeriph = DMA_REQ_NONE;
    llicfg.dstPeriph = DMA_REQ_NONE;
    DMA_Channel_Disable(DMA0_ID, dma_chan);

    struct DMA_Control_Reg dma_ctrl_reg={
        .TransferSize=0,
        .SBSize=DMA_BURST_SIZE_4,
        .dst_min_mode=DISABLE,
        .DBSize=DMA_BURST_SIZE_4,
        .dst_add_mode=DISABLE,
        .SWidth=DMA_TRNS_WIDTH_8BITS,
        .DWidth=DMA_TRNS_WIDTH_8BITS,
        .fix_cnt=0,
        .SI=DMA_MINC_DISABLE,
        .DI=DMA_MINC_ENABLE,
        .I=0,
    };

    for (i = 0; i < count; i++) {
        if (remainder == 0) {
            dma_ctrl_reg.TransferSize = LLI_BUFF_SIZE;
        } else {
            if (i == count - 1)
                dma_ctrl_reg.TransferSize = remainder;
            else
                dma_ctrl_reg.TransferSize = LLI_BUFF_SIZE;
        }
        pstlli[i].srcDmaAddr = &val;
        pstlli[i].destDmaAddr = dstAddr + i*LLI_BUFF_SIZE;
        pstlli[i].dmaCtrl = dma_ctrl_reg;
        if (i > 0)
            pstlli[i-1].nextLLI = (uint32_t)&pstlli[i];
        pstlli[i].nextLLI = 0;
    }
    pstlli[count-1].dmaCtrl.I = 1;

    flush_dcache_range((unsigned long long)pstlli, sizeof(DMA_LLI_Ctrl_Type) * count);

    DMA_LLI_Init(DMA0_ID, dma_chan, &llicfg);
    DMA_LLI_Update(DMA0_ID, dma_chan, (uint32_t)&(pstlli[0]));
    dma_done = 0;

    hosal_dma_chan_start(HOSAL_DMA0_ID, dma_chan);

    while (!dma_done) {
    }

    hosal_dma_chan_stop(HOSAL_DMA0_ID, dma_chan);

    vPortFree(pstlli);
}
#endif

static void DMA_Copy_Memory(uint8_t dma_chan, uint32_t srcAddr,uint32_t dstAddr,uint32_t length,uint8_t width,uint8_t burst)
{
    dma_trans_param_t trans_param;

    dma_done = 0;
    trans_param.src_burst = burst;
    trans_param.dst_burst = burst;
    trans_param.src_width = width;
    trans_param.dst_width = width;
    trans_param.map = DMA_PARAM_UPDATE_WIDTH | DMA_PARAM_UPDATE_BURST;
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_CHANNEL_TRANS_PARAM_UPDATE, (void *)&trans_param);
    dma_reload(DMA0_ID, DMA0_CH0_INDEX, srcAddr, dstAddr, length, NULL);
    MFlushDCacheRange(srcAddr, length);
    MInvalDCacheRange(dstAddr, length);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_CHANNEL_START, NULL);
    while (dma_done == 0);

}

static void dmatest_proc(void *arg)
{

    int i, j;

    dma_device_t dma_dev = {
        .id = DMA0_ID,
        .ch = DMA0_CH0_INDEX,
        .transfer_mode = DMA_LLI_ONCE_MODE,
        .direction = DMA_MEMORY_TO_MEMORY,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR4,
        .dst_burst_size = DMA_BURST_INCR4,
        .src_width = DMA_TRANSFER_WIDTH_32BIT,
        .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .lli_cfg = NULL,
        .callback = dma_copy_callback
    };

    dma_setConfig(&dma_dev);
    dma_init(DMA0_ID, dma_dev.ch);

    while(1) {
        dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);

        /* Clear memory */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=memorySize[i]/2;j<memorySize[i];j++){
                ((uint8_t*)(memoryAddr[i]))[j] = 0;
            }
        }

        /* Gen memory */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/2;j++){
                ((uint8_t*)(memoryAddr[i]))[j] = rand() & 0xFF;
            }
        }


        printf("DMA byte read/write:\r\n");
        /* DMA copy */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++) {
            DMA_Copy_Memory(DMA0_CH0_INDEX, memoryAddr[i],memoryAddr[i]+memorySize[i]/2,memorySize[i]/2,DMA_TRANSFER_WIDTH_8BIT,DMA_BURST_INCR16);
        }

        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/2;j++){
                if(((uint8_t*)(memoryAddr[i]))[j] != ((uint8_t*)(memoryAddr[i]+memorySize[i]/2))[j]){
                    printf("Error! Addr: 0x%08lx is 0x%02x, expect 0x%02x\n",memoryAddr[i]+memorySize[i]/2+j, \
                    ((uint8_t*)(memoryAddr[i]+memorySize[i]/2))[j],((uint8_t*)(memoryAddr[i]))[j]);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error\n");
        }else{
            errorFlag = 0;
        }

        printf("DMA half word read/write:\r\n");

        /* Clear memory */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++) {
            for(j=memorySize[i]/2;j<memorySize[i];j++){
                ((uint8_t*)(memoryAddr[i]))[j] = 0;
            }
        }

        /* DMA copy */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++) {
            DMA_Copy_Memory(DMA0_CH0_INDEX, memoryAddr[i],memoryAddr[i]+memorySize[i]/2,memorySize[i]/2, DMA_TRANSFER_WIDTH_16BIT, DMA_BURST_INCR8);
        }

        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/4;j++){
                if(((uint16_t*)(memoryAddr[i]))[j] != ((uint16_t*)(memoryAddr[i]+memorySize[i]/2))[j]){
                    printf("Error! Addr: 0x%08lx is 0x%04x, expect 0x%04x\n",memoryAddr[i]+memorySize[i]/2+j*2, \
                    ((uint16_t*)(memoryAddr[i]+memorySize[i]/2))[j],((uint16_t*)(memoryAddr[i]))[j]);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error\n");
        }else{
            errorFlag = 0;
        }

        printf("DMA word read/write:\r\n");

        /* Clear memory */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=memorySize[i]/2;j<memorySize[i];j++){
                ((uint8_t*)(memoryAddr[i]))[j] = 0;
            }
        }

        /* DMA copy */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++) {
            DMA_Copy_Memory(DMA0_CH0_INDEX, memoryAddr[i],memoryAddr[i]+memorySize[i]/2,memorySize[i]/2, DMA_TRANSFER_WIDTH_32BIT, DMA_BURST_INCR4);
        }

        /* Check */
        for(i=0;i<sizeof(memoryAddr)/sizeof(memoryAddr[0]);i++){
            for(j=0;j<memorySize[i]/8;j++){
                if(((uint32_t*)(memoryAddr[i]))[j] != ((uint32_t*)(memoryAddr[i]+memorySize[i]/2))[j]){
                    printf("Error! Addr: 0x%08lx is 0x%08lx, expect 0x%08lx\n",memoryAddr[i]+memorySize[i]/2+j*4, \
                    ((uint32_t*)(memoryAddr[i]+memorySize[i]/2))[j],((uint32_t*)(memoryAddr[i]))[j]);
                    errorFlag = 1;
                    while(1){}
                }
            }
        }

        if(errorFlag == 0){
            printf("no error\n");
        }else{
            errorFlag = 0;
        }

        printf("end\r\n");
        dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_CHANNEL_STOP, NULL);
        dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_CLEAR_INT, NULL);
    }
    vTaskDelete(NULL);
}

static int cmd_memory_test(int argc, char **argv)
{
    if (test_stop) {
        test_stop = 0;
        xTaskCreate(cpu_rwtest_proc, "cpu_test", 2048, NULL, 10, NULL);
    } else {
        test_stop = 1;
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_memory_test, mem_rw_test, mem rw test);

static int cmd_dma_test(int argc, char **argv)
{
    if (test_stop) {
        test_stop = 0;
        xTaskCreate(dmatest_proc, "dma_test", 2048, NULL, 10, NULL);
    } else {
        test_stop = 1;
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dma_test, mem_dma_test, mem dma Test);

static int cmd_ddr_test(int argc, char **argv)
{
    volatile uint32_t val = 0xa5a5a5a5;
    volatile uint32_t *addr = (uint32_t *)malloc(6*1024*1024);
    volatile uint32_t *start, *end;
    start = addr;
    end = (uint32_t*)((uint32_t)addr+6*1024*1024);
    printf("start ddr test %p - %p, size: %lu MB \r\n", start, end, ((uint32_t)end - (uint32_t)start) >> 20);

    //writel(1, 0x40000070);
    for (int i = 0; i < 100; i++) {
        val = 0xa5a5a5a5;
        while(val > 0) {
            printf("[%d] write test\r\n", i);
            start = addr;
            while (start < end) {
                *start = val;
                start++;
            }
            printf("[%d] readback test\r\n", i);
            start = addr;
            while (start < end) {
                if (*start != val) {
                    printf("addr %p = 0x%lx, expect 0x%lx\r\n", start, *start, val);
                    while(1);
                }
                start++;
            }
            printf("[%d] test val 0x%lx pass\r\n", i, val);
            val >>= 1;
        }
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ddr_test, ddr_test, ddr);
