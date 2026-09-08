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
#include <FreeRTOS.h>
#include <task.h>
#include <hal_flash.h>
#include <hal_mtimer.h>

enum {
    SF_TESTCASE_GETID = 0,
    SF_TESTCASE_NOR_ERASE,
    SF_TESTCASE_NOR_READ_WRITE,
    SF_TESTCASE_XIP_READ,
    SF_TESTCASE_NUM,
};

#define FLASH_TEST_LEN  (64*1024)
#define FLASH_XIP_START_ADDR    (0x13000000)
static void sf_ut(int testcase)
{
    int result = 0;
    uint32_t val = 0;
    uint8_t *src_buffer = (uint8_t *)malloc(FLASH_TEST_LEN);
    uint8_t *dst_buffer = (uint8_t *)malloc(FLASH_TEST_LEN);

    switch (testcase) {
    case SF_TESTCASE_GETID:
        flash_read_jedec_id((uint8_t *)&val);
        printf("SF Get JedecID: 0x%lx\r\n", val);
        break;
    case SF_TESTCASE_NOR_ERASE:
        printf("SPI Nor Flash Erase... ");
        flash_erase(0xC0000, FLASH_TEST_LEN);
        flash_read(0xC0000, dst_buffer, FLASH_TEST_LEN);
        for (int i =0; i < FLASH_TEST_LEN; i++) {
            if (dst_buffer[i] != 0xff) {
                result = 1;
                break;
            }
        }
        if (result)
            printf("Fail\r\n");
        else
            printf("Pass\r\n");
        break;
    case SF_TESTCASE_NOR_READ_WRITE:
        printf("SPI Nor Flash Read/Write (src: %p) ", src_buffer);
        for (int i = 0; i < FLASH_TEST_LEN; i++)
            src_buffer[i] = rand() & 0xFF;
        memset(dst_buffer, 0, FLASH_TEST_LEN);
        flash_erase(0xC0000, FLASH_TEST_LEN);
        flash_write(0xC0000, src_buffer, FLASH_TEST_LEN);
        flash_read(0xC0000, dst_buffer, FLASH_TEST_LEN);
        for (int i =0; i < FLASH_TEST_LEN; i++) {
            if (src_buffer[i] != dst_buffer[i]) {
                result = 1;
                break;
            }
        }
        if (result)
            printf("Fail\r\n");
        else
            printf("Pass\r\n");
        break;
    case SF_TESTCASE_XIP_READ:
    {
        printf("SPI Nor Flash XIP Read (src: %p) ", src_buffer);
        for (int i = 0; i < FLASH_TEST_LEN; i++)
            src_buffer[i] = rand() & 0xFF;
        memset(dst_buffer, 0, FLASH_TEST_LEN);
        flash_erase(0xC0000, FLASH_TEST_LEN);
        flash_write(0xC0000, src_buffer, FLASH_TEST_LEN);

        L1C_DCACHE_INVALID_RANGE(FLASH_XIP_START_ADDR, FLASH_TEST_LEN);
        uint64_t start_tick = mtimer_get_time_us();
        start_tick = mtimer_get_time_us();

        memcpy(dst_buffer, (void *)FLASH_XIP_START_ADDR, FLASH_TEST_LEN);
        uint64_t end_tick = mtimer_get_time_us();
        printf("XIP read throughput: %f Mbps\r\n", (float)FLASH_TEST_LEN*8 / (end_tick - start_tick));
        if (memcmp(dst_buffer, src_buffer, FLASH_TEST_LEN)) {
            printf("Buffer not match!!\r\n");
            break;
        }
        printf("XIP Read check pass\r\n");
        break;
    }
    default:
        printf("Unknown sf testcase\r\n");
        break;
    }
    free(src_buffer);
    free(dst_buffer);
}

static void sflash_test_proc(void *arg)
{
    flash_init();

    printf("start sf test\r\n");
    for (int i = 0; i < SF_TESTCASE_NUM; i++) {
        sf_ut(i);
        arch_delay_ms(100);
    }

    printf("sf testcase done\r\n");
    vTaskDelete(NULL);
}

int cmd_sflash_test(int argc, char **argv)
{
    printf("argc = %d\r\n", argc);
    if (argc == 2) {
        sf_ut(atoi(argv[1]));
    } else {
        xTaskCreate(sflash_test_proc ,"sf_test", 4096, NULL, 10, NULL);
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_sflash_test, flash_test, Flash test);

int cmd_flash_read_reg(int argc, char **argv)
{
    uint8_t cmd;
    uint8_t reglen;
    if (argc == 3) {
        cmd = strtoul(argv[1], NULL, 16);
        reglen = atoi(argv[2]);
        printf("flash read reg cmd 0x%x = 0x%lx\r\n", cmd, flash_readReg(cmd, reglen));
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_flash_read_reg, flash_readReg, Flash read reg);

int cmd_flash_write_reg(int argc, char **argv)
{
    uint8_t cmd, reglen;
    uint32_t val;
    if (argc == 4) {
        cmd = strtoul(argv[1], NULL, 16);
        val = strtoul(argv[2], NULL, 16);
        reglen = atoi(argv[2]);

        printf("flash write reg 0x%x = 0x%lx\r\n", cmd, val);
        flash_writeReg(cmd, val, reglen);
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_flash_write_reg, flash_writeReg, Flash write_reg);
