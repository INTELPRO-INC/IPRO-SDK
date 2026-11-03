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
#include "shell.h"
#include <hal_spi.h>
#include <hal_dma.h>
#include "FreeRTOS.h"
#include "task.h"

enum {
    SPI_TESTCASE_MASTER_SEND = 0,
    SPI_TESTCASE_MASTER_RECV,
    SPI_TESTCASE_MASTER_SEND_DMA,
    SPI_TESTCASE_MASTER_RECV_DMA,
    SPI_TESTCASE_SLAVE_SEND,
    SPI_TESTCASE_SLAVE_RECV,
    SPI_TESTCASE_SLAVE_SEND_DMA,
    SPI_TESTCASE_SLAVE_RECV_DMA,
    SPI_TESTCASE_NUM,
    SPI_TESTCASE_REGRESSION,
    SPI_TESTCASE_SPI1_MASTER_SEND,
    SPI_TESTCASE_SPI0_MASTER_SEND_AND_SPI1_SLAVE_RECV,
    SPI_TESTCASE_SPI1_SLAVE_SEND_AND_SPI0_MASTER_RECV

};


#define SPI_TEST_DATA_LEN   (8)
uint8_t send_data[SPI_TEST_DATA_LEN];
uint8_t recv_data[SPI_TEST_DATA_LEN];
static volatile int gSPIDmaDone = 0;
static void dump_buffer_u8(uint8_t *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        if (i && (i % 32 == 0))
            printf("\r\n");
        printf("%02x ",ptr[i]);
    }
    printf("\r\n");
}

static void spi_irq_cb(void *arg, uint32_t event)
{
    printf("SPI IRQ raised\r\n");
//    printf("spi send:\r\n");
//    dump_buffer_u8(send_data, SPI_TEST_DATA_LEN);

//    printf("\r\nspi recv:\r\n");
//    dump_buffer_u8(recv_data, SPI_TEST_DATA_LEN);
}

static void spi_dma_cb(void *dev, uint32_t flag)
{
    printf("spi send:\r\n");
    dump_buffer_u8(send_data, SPI_TEST_DATA_LEN);

    printf("\r\nspi recv:\r\n");
    dump_buffer_u8(recv_data, SPI_TEST_DATA_LEN);
    gSPIDmaDone = 1;
}

static int spi_regression(void)
{
    int fail = 0;
    spi_device_t spi_dev = {
        .caps = 0,
        .id = SPI0_INDEX,
        .clk = 1000000,
        .mode = SPI_MASTER_MODE,
        .direction = SPI_MSB_BYTE0_DIRECTION_FIRST,
        .clk_polaraity = SPI_POLARITY_LOW,
        .clk_phase = SPI_PHASE_1EDGE,
        .datasize = SPI_DATASIZE_8BIT,
        .fifo_threshold = 0,
        .pin_swap_enable = 0, /*swap mosi and miso*/
        .delitch_cnt = 0,
        .callback = spi_irq_cb,
    };
    dma_device_t dma_dev = {
        .id = DMA0_ID,
        .ch = DMA0_CH0_INDEX,
        .transfer_mode = DMA_LLI_ONCE_MODE,
        .direction = DMA_MEMORY_TO_MEMORY,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR1,
        .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_8BIT,
        .dst_width = DMA_TRANSFER_WIDTH_8BIT,
        .lli_cfg = NULL,
        .callback = spi_dma_cb,
    };

    printf("SPI regression\r\n");

    arch_delay_ms(1000);

    printf("1. master send by dma\r\n");

    /* config spi */
    spi_dev.mode = SPI_MASTER_MODE;
    spi_dev.caps = COMMON_CAPS_DMA_TX;
    spi_setConfig(&spi_dev);
    spi_init(SPI0_INDEX);
    spi_enable(SPI0_INDEX, 1);

    /* config dma */
    dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    dma_dev.src_req = DMA_REQUEST_NONE;
    dma_dev.dst_req = DMA_REQUEST_SPI0_TX;
    dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
    dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
    dma_setConfig(&dma_dev);
    dma_init(DMA0_ID, dma_dev.ch);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
    L1C_DCACHE_CLEAN_RANGE((unsigned long)send_data, SPI_TEST_DATA_LEN);
    dma_reload(DMA0_ID, DMA0_CH0_INDEX, (uint32_t)send_data, (uint32_t)DMA_ADDR_SPI_TDR, SPI_TEST_DATA_LEN, NULL);
    dma_channel_start(DMA0_ID, DMA0_CH0_INDEX);

    while(gSPIDmaDone == 0);
    gSPIDmaDone = 0;
    spi_deinit(SPI0_INDEX);

    arch_delay_ms(1000);
    memset(recv_data, 0, sizeof(recv_data));

    printf("2. master recv by polling\r\n");
    spi_dev.mode = SPI_MASTER_MODE;
    spi_dev.caps = 0;
    spi_setConfig(&spi_dev);
    spi_init(SPI0_INDEX);
    spi_enable(SPI0_INDEX, 1);
    spi_receive(SPI0_INDEX, recv_data, SPI_TEST_DATA_LEN);
    dump_buffer_u8(recv_data, SPI_TEST_DATA_LEN);

    for (int i = 0; i < SPI_TEST_DATA_LEN; i++) {
        if (recv_data[i] != (send_data[i] * 2)) {
            printf("recv[%d](0x%02x) != send[%d](0x%02x)\r\n", i, recv_data[i], i, send_data[i]);
            fail = 1;
            break;
        }
        send_data[i] = recv_data[i] / 2;
    }
    spi_deinit(SPI0_INDEX);

    printf("3. slave send by polling\r\n");
    spi_dev.mode = SPI_SLAVE_MODE;
    spi_dev.caps = 0;
    spi_setConfig(&spi_dev);
    spi_init(SPI0_INDEX);
    spi_enable(SPI0_INDEX, 1);
    spi_transmit(SPI0_INDEX, send_data, SPI_TEST_DATA_LEN);
    dump_buffer_u8(send_data, SPI_TEST_DATA_LEN);
    spi_deinit(SPI0_INDEX);

    printf("4. slave recv by dma\r\n");
    /* config dma */
    dma_dev.direction = DMA_PERIPH_TO_MEMORY;
    dma_dev.src_req = DMA_REQUEST_SPI0_RX;
    dma_dev.dst_req = DMA_REQUEST_NONE;
    dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
    dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
    dma_setConfig(&dma_dev);
    dma_init(DMA0_ID, dma_dev.ch);
    dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
    L1C_DCACHE_INVALID_RANGE((unsigned long)recv_data, SPI_TEST_DATA_LEN);
    dma_reload(DMA0_ID, DMA0_CH0_INDEX, (uint32_t)DMA_ADDR_SPI_RDR, (uint32_t)recv_data, SPI_TEST_DATA_LEN, NULL);
    dma_channel_start(DMA0_ID, DMA0_CH0_INDEX);

    spi_dev.mode = SPI_SLAVE_MODE;
    spi_dev.caps = COMMON_CAPS_DMA_RX;
    spi_setConfig(&spi_dev);
    spi_init(SPI0_INDEX);
    spi_enable(SPI0_INDEX, 1);

    while(gSPIDmaDone == 0);
    gSPIDmaDone = 0;

    dump_buffer_u8(recv_data, SPI_TEST_DATA_LEN);

    for (int i = 0; i < SPI_TEST_DATA_LEN; i++) {
        if (recv_data[i] != (send_data[i] * 2)) {
            printf("recv[%d](0x%02x) != send[%d](0x%02x)\r\n", i, recv_data[i], i, send_data[i]);
            fail = 1;
            break;
        }
    }
    spi_deinit(SPI0_INDEX);

    return fail;
}

    spi_device_t spi_dev = {
        .caps = 0,
        .id = SPI0_INDEX,
        .clk = 1000000,
        .mode = SPI_MASTER_MODE,
        .direction = SPI_MSB_BYTE0_DIRECTION_FIRST,
        .clk_polaraity = SPI_POLARITY_LOW,
        .clk_phase = SPI_PHASE_1EDGE,
        .datasize = SPI_DATASIZE_8BIT,
        .fifo_threshold = 0,
        .pin_swap_enable = 0, /*swap mosi and miso*/
        .delitch_cnt = 0,
        .callback = spi_irq_cb,
    };
    spi_device_t spi_dev_1 = {
        .caps = 0,
        .id = SPI1_INDEX,
        .clk = 1000000,
        .mode = SPI_SLAVE_MODE,
        .direction = SPI_MSB_BYTE0_DIRECTION_FIRST,
        .clk_polaraity = SPI_POLARITY_LOW,
        .clk_phase = SPI_PHASE_1EDGE,
        .datasize = SPI_DATASIZE_8BIT,
        .fifo_threshold = 0,
        .pin_swap_enable = 0, /*swap mosi and miso*/
        .delitch_cnt = 0,
        .callback = spi_irq_cb,
    };
void spi0_master_thread_tx(void *arg) {
    uint8_t send_buf[SPI_TEST_DATA_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
    spi_dev.mode = SPI_MASTER_MODE;
    spi_dev.caps = 0;
    spi_setConfig(&spi_dev);
    spi_init(SPI0_INDEX);
    spi_enable(SPI0_INDEX, 1);
    int j = 3;
    while (j) {

        vTaskDelay(100);
        spi_transmit(SPI0_INDEX, send_buf, SPI_TEST_DATA_LEN);
        printf("[Master] TX done.\r\n");

        j--;
        vTaskDelay(100);
    }
}

void spi0_master_thread_rx(void *arg) {
    uint8_t recv_buf[SPI_TEST_DATA_LEN];
    spi_dev.mode = SPI_MASTER_MODE;
    spi_dev.caps = 0;
    spi_setConfig(&spi_dev);
    spi_init(SPI0_INDEX);
    spi_enable(SPI0_INDEX, 1);
    int i = 3;
    while (i) {

        printf("[Master] RX...\r\n");
        spi_receive(SPI0_INDEX, recv_buf, SPI_TEST_DATA_LEN);
        printf("[Master] RX: ");
        dump_buffer_u8(recv_buf, SPI_TEST_DATA_LEN);
        i--;
        vTaskDelay(1000);
    }
}

void spi1_slave_thread_rx(void *arg) {
    uint8_t recv_buf[SPI_TEST_DATA_LEN];
    spi_dev_1.mode = SPI_SLAVE_MODE;
    spi_dev_1.caps = 0;
    spi_setConfig(&spi_dev_1);
    spi_init(SPI1_INDEX);
    //int i = 3;

    spi_enable(SPI1_INDEX, 1);
    while (1) {

        printf("[Slave] Waiting for master...\r\n");
        // MOSI
        spi_receive(SPI1_INDEX, recv_buf, SPI_TEST_DATA_LEN);
        printf("[Slave] RX done:\r\n");
        dump_buffer_u8(recv_buf, SPI_TEST_DATA_LEN);
        vTaskDelay(10);

    }
}

void spi1_slave_thread_tx(void *arg) {
    uint8_t send_buf[SPI_TEST_DATA_LEN] = {8, 7, 6, 5, 4, 3, 2, 1};
    spi_dev_1.mode = SPI_SLAVE_MODE;
    spi_dev_1.caps = 0;
    spi_setConfig(&spi_dev_1);
    spi_init(SPI1_INDEX);
    int i = 3;
    spi_enable(SPI1_INDEX, 1);
    while (i) {

        printf("[Slave] Prepare TX FIFO for next Master...\r\n");
        spi_transmit(SPI1_INDEX, send_buf, SPI_TEST_DATA_LEN);
        printf("[Slave] Waiting for Master clock...\r\n");
        vTaskDelay(1000);

        i--;
    }
}

static void spi_ut(int testcase) {

    dma_device_t dma_dev = {
        .id = DMA0_ID,
        .ch = DMA0_CH0_INDEX,
        .transfer_mode = DMA_LLI_ONCE_MODE,
        .direction = DMA_MEMORY_TO_MEMORY,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR1,
        .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_8BIT,
        .dst_width = DMA_TRANSFER_WIDTH_8BIT,
        .lli_cfg = NULL,
        .callback = spi_dma_cb,
    };

    int start = rand();
    /* prepara send buf */
    for (int i = 0; i < sizeof(send_data)/sizeof(send_data[0]); i++) {
        send_data[i] = start + i + 2;
    }
    gSPIDmaDone = 0;
    memset(recv_data, 0, SPI_TEST_DATA_LEN);
    printf("Test case %d, ", testcase);
    switch ((testcase)) {
    case SPI_TESTCASE_MASTER_SEND:
        printf("SPI master send\r\n");
        spi_dev.mode = SPI_MASTER_MODE;
        spi_dev.caps = 0;
        spi_setConfig(&spi_dev);
        spi_init(SPI0_INDEX);
        spi_enable(SPI0_INDEX, 1);
        spi_transmit(SPI0_INDEX, send_data, SPI_TEST_DATA_LEN);
        dump_buffer_u8(send_data, SPI_TEST_DATA_LEN);
        break;
    case SPI_TESTCASE_MASTER_RECV:
        printf("SPI master recv\r\n");
        spi_dev.mode = SPI_MASTER_MODE;
        spi_dev.caps = 0;
        spi_setConfig(&spi_dev);
        spi_init(SPI0_INDEX);
        spi_enable(SPI0_INDEX, 1);
        spi_receive(SPI0_INDEX, recv_data, SPI_TEST_DATA_LEN);
        dump_buffer_u8(recv_data, SPI_TEST_DATA_LEN);
        break;
    case SPI_TESTCASE_MASTER_SEND_DMA:
        printf("SPI master send by dma\r\n");

        /* config spi */
        spi_dev.mode = SPI_MASTER_MODE;
        spi_dev.caps = COMMON_CAPS_DMA_TX;
        spi_setConfig(&spi_dev);
        spi_init(SPI0_INDEX);
        spi_enable(SPI0_INDEX, 1);

        /* config dma */
        dma_dev.direction = DMA_MEMORY_TO_PERIPH;
        dma_dev.src_req = DMA_REQUEST_NONE;
        dma_dev.dst_req = DMA_REQUEST_SPI0_TX;
        dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
        dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
        dma_setConfig(&dma_dev);
        dma_init(DMA0_ID, dma_dev.ch);
        dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
        L1C_DCACHE_CLEAN_RANGE((unsigned long)send_data, SPI_TEST_DATA_LEN);
        dma_reload(DMA0_ID, DMA0_CH0_INDEX, (uint32_t)send_data, (uint32_t)DMA_ADDR_SPI_TDR, SPI_TEST_DATA_LEN, NULL);
        dma_channel_start(DMA0_ID, DMA0_CH0_INDEX);

        while(gSPIDmaDone == 0);
        break;
    case SPI_TESTCASE_MASTER_RECV_DMA:
        printf("SPI master recv by dma\r\n");
        /* config dma */
        dma_dev.direction = DMA_PERIPH_TO_MEMORY;
        dma_dev.src_req = DMA_REQUEST_SPI0_RX;
        dma_dev.dst_req = DMA_REQUEST_NONE;
        dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
        dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
        dma_setConfig(&dma_dev);
        dma_init(DMA0_ID, dma_dev.ch);
        dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
        L1C_DCACHE_INVALID_RANGE((unsigned long)recv_data, SPI_TEST_DATA_LEN);
        dma_reload(DMA0_ID, DMA0_CH0_INDEX, (uint32_t)DMA_ADDR_SPI_RDR, (uint32_t)recv_data, SPI_TEST_DATA_LEN, NULL);
        dma_channel_start(DMA0_ID, DMA0_CH0_INDEX);

        spi_dev.mode = SPI_MASTER_MODE;
        spi_dev.caps = COMMON_CAPS_DMA_RX;
        spi_setConfig(&spi_dev);
        spi_init(SPI0_INDEX);
        spi_enable(SPI0_INDEX, 1);
        memset(send_data, 0xFF, SPI_TEST_DATA_LEN);
        spi_transmit(SPI0_INDEX, send_data, SPI_TEST_DATA_LEN);

        while(gSPIDmaDone == 0);

        break;
    case SPI_TESTCASE_SLAVE_SEND:
        printf("SPI slave send (SPI_MASTER_8B_RX)\r\n");
        spi_dev.mode = SPI_SLAVE_MODE;
        spi_dev.caps = 0;
        spi_setConfig(&spi_dev);
        spi_init(SPI0_INDEX);
        spi_enable(SPI0_INDEX, 1);
        spi_transmit(SPI0_INDEX, send_data, SPI_TEST_DATA_LEN);
        dump_buffer_u8(send_data, SPI_TEST_DATA_LEN);
        break;
    case SPI_TESTCASE_SLAVE_RECV:
        printf("SPI slave recv (SPI_MASTER_8B_TX)\r\n");
        spi_dev.mode = SPI_SLAVE_MODE;
        spi_dev.caps = 0;
        spi_setConfig(&spi_dev);
        spi_init(SPI0_INDEX);
        spi_enable(SPI0_INDEX, 1);
        spi_receive(SPI0_INDEX, recv_data, SPI_TEST_DATA_LEN);
        dump_buffer_u8(recv_data, SPI_TEST_DATA_LEN);

        break;
    case SPI_TESTCASE_SLAVE_SEND_DMA:
        printf("SPI slave send by dma (SPI_MASTER_8B_RX)\r\n");
        /* config spi */
        spi_dev.mode = SPI_SLAVE_MODE;
        spi_dev.caps = COMMON_CAPS_DMA_TX;
        spi_setConfig(&spi_dev);
        spi_init(SPI0_INDEX);
        spi_enable(SPI0_INDEX, 1);

        /* config dma */
        dma_dev.direction = DMA_MEMORY_TO_PERIPH;
        dma_dev.src_req = DMA_REQUEST_NONE;
        dma_dev.dst_req = DMA_REQUEST_SPI0_TX;
        dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
        dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
        dma_setConfig(&dma_dev);
        dma_init(DMA0_ID, dma_dev.ch);
        dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
        L1C_DCACHE_CLEAN_RANGE((unsigned long)send_data, SPI_TEST_DATA_LEN);
        dma_reload(DMA0_ID, DMA0_CH0_INDEX, (uint32_t)send_data, (uint32_t)DMA_ADDR_SPI_TDR, SPI_TEST_DATA_LEN, NULL);
        dma_channel_start(DMA0_ID, DMA0_CH0_INDEX);

        while(gSPIDmaDone == 0);
        break;
    case SPI_TESTCASE_SLAVE_RECV_DMA:
        printf("SPI slave recv by dma (SPI_MASTER_8B_TX)\r\n");
        /* config dma */
        dma_dev.direction = DMA_PERIPH_TO_MEMORY;
        dma_dev.src_req = DMA_REQUEST_SPI0_RX;
        dma_dev.dst_req = DMA_REQUEST_NONE;
        dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
        dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
        dma_setConfig(&dma_dev);
        dma_init(DMA0_ID, dma_dev.ch);
        dma_ioctl(DMA0_ID, DMA0_CH0_INDEX, DMA_CTRL_SET_INT, NULL);
        L1C_DCACHE_INVALID_RANGE((unsigned long)recv_data, SPI_TEST_DATA_LEN);
        dma_reload(DMA0_ID, DMA0_CH0_INDEX, (uint32_t)DMA_ADDR_SPI_RDR, (uint32_t)recv_data, SPI_TEST_DATA_LEN, NULL);
        dma_channel_start(DMA0_ID, DMA0_CH0_INDEX);

        spi_dev.mode = SPI_SLAVE_MODE;
        spi_dev.caps = COMMON_CAPS_DMA_RX;
        spi_setConfig(&spi_dev);
        spi_init(SPI0_INDEX);
        spi_enable(SPI0_INDEX, 1);

        while(gSPIDmaDone == 0);

        break;
    case SPI_TESTCASE_REGRESSION:
        if (spi_regression() == 0)
            printf("Pass\r\n");
        else
            printf("Fail\r\n");
        break;
    case SPI_TESTCASE_SPI1_MASTER_SEND:
        printf("SPI 1 master send\r\n");
        spi_dev_1.mode = SPI_MASTER_MODE;
        spi_dev_1.caps = 0;
        spi_setConfig(&spi_dev_1);
        spi_init(SPI1_INDEX);
        spi_enable(SPI1_INDEX, 1);
        spi_transmit(SPI1_INDEX, send_data, SPI_TEST_DATA_LEN);
        dump_buffer_u8(send_data, SPI_TEST_DATA_LEN);
        break;
    case SPI_TESTCASE_SPI0_MASTER_SEND_AND_SPI1_SLAVE_RECV:
        xTaskCreate(spi1_slave_thread_rx, "SPI1_Slave_rx", 1024, NULL, 3, NULL);
        vTaskDelay(10);
        xTaskCreate(spi0_master_thread_tx, "SPI0_Master_tx", 1024, NULL, 3, NULL);
        break;

    case SPI_TESTCASE_SPI1_SLAVE_SEND_AND_SPI0_MASTER_RECV:
        xTaskCreate(spi1_slave_thread_tx, "SPI0_Master_tx", 1024, NULL, 3, NULL);
        vTaskDelay(10);
        xTaskCreate(spi0_master_thread_rx, "SPI1_Slave_rx", 1024, NULL, 3, NULL);

        break;

    default:
        printf("Unknown case\r\n");
        break;
    }
    spi_deinit(SPI0_INDEX);
}

static int cmd_spi_test(int argc, char **argv)
{
    int tc = 0;
    if (argc == 2)
        tc = atoi(argv[1]);
    spi_ut(tc);
    printf("spi testcase done\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_spi_test, spi_test, SPI Test);
