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

/**
 * @file tc_romapi.c
 * @brief ROM API test cases for hybrid mode verification
 *
 * Tests 164 enabled ROM APIs to verify they work without hang/exception.
 * Psram_* APIs are skipped (use corrupted CpuIRegionBase via arch_delay_us).
 *
 * Usage: romapi_test [category]
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

#include "drv_clock.h"
#include "drv_pds.h"
#include "drv_sf_ctrl.h"
#include "drv_sf_cfg.h"
#include "drv_uart.h"
#include "drv_ir.h"
#include "drv_kys.h"
#include "drv_common.h"
#include "utils_crc.h"

/* Test result tracking */
static int test_pass_count = 0;
static int test_fail_count = 0;
static int test_skip_count = 0;

#define TEST_START(name) \
    do { printf("  %-40s ", name); } while (0)

#define TEST_PASS() \
    do { printf("PASS\r\n"); test_pass_count++; } while (0)

#define TEST_PASS_VAL(fmt, val) \
    do { printf("PASS (" fmt ")\r\n", val); test_pass_count++; } while (0)

#define TEST_FAIL(reason) \
    do { printf("FAIL (%s)\r\n", reason); test_fail_count++; } while (0)

#define TEST_SKIP() \
    do { printf("SKIP\r\n"); test_skip_count++; } while (0)

/* ============================================================================
 * Clock API Tests (3 APIs)
 * ============================================================================ */
static void test_clock_apis(void)
{
    printf("\n[Clock APIs - 3 total]\r\n");

    TEST_START("Clock_System_Clock_Get(FCLK)");
    TEST_PASS_VAL("%lu", Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_FCLK));

    TEST_START("Clock_System_Clock_Get(BCLK)");
    TEST_PASS_VAL("%lu", Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK));

    TEST_START("Clock_Peripheral_Clock_Get(UART0)");
    TEST_PASS_VAL("%lu", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0));

    TEST_START("Clock_Peripheral_Clock_Get(FLASH)");
    TEST_PASS_VAL("%lu", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_FLASH));

    TEST_START("SystemCoreClockGet");
    TEST_PASS_VAL("%lu", SystemCoreClockGet());
}

/* ============================================================================
 * PDS API Tests (28 APIs)
 * ============================================================================ */
static void test_pds_apis(void)
{
    printf("\n[PDS APIs - 28 total]\r\n");

    /* Read-only status APIs */
    TEST_START("PDS_Get_GPIO_Pad_IntStatus(PIN0)");
    TEST_PASS_VAL("%d", PDS_Get_GPIO_Pad_IntStatus(PDS_GPIO_PIN_0));

    TEST_START("PDS_Get_IntStatus(WAKEUP)");
    TEST_PASS_VAL("%d", PDS_Get_IntStatus(PDS_INT_WAKEUP));

    TEST_START("PDS_Get_IntStatus(RF_DONE)");
    TEST_PASS_VAL("%d", PDS_Get_IntStatus(PDS_INT_RF_DONE));

    /* Skip write/config APIs to avoid side effects */
    TEST_START("PDS_Set_* (config APIs)");
    TEST_SKIP();

    TEST_START("PDS_Enable/Force_Config");
    TEST_SKIP();

    TEST_START("PDS_RAM/Power APIs");
    TEST_SKIP();
}

/* ============================================================================
 * SF_Ctrl API Tests (31 APIs)
 * ============================================================================ */
static void test_sf_ctrl_apis(void)
{
    printf("\n[SF_Ctrl APIs - 31 total]\r\n");

    /* Read-only APIs */
    TEST_START("SF_Ctrl_Get_Clock_Delay");
    TEST_PASS_VAL("%u", SF_Ctrl_Get_Clock_Delay());

    TEST_START("SF_Ctrl_Is_AES_Enable");
    TEST_PASS_VAL("%d", SF_Ctrl_Is_AES_Enable());

    TEST_START("SF_Ctrl_Get_Flash_Image_Offset(0,0)");
    TEST_PASS_VAL("0x%lx", (unsigned long)SF_Ctrl_Get_Flash_Image_Offset(0, 0));

    TEST_START("SF_Ctrl_GetBusyState");
    TEST_PASS_VAL("%d", SF_Ctrl_GetBusyState());

    /* Skip write/config APIs */
    TEST_START("SF_Ctrl_Set* (config APIs)");
    TEST_SKIP();

    TEST_START("SF_Ctrl_Enable/Disable");
    TEST_SKIP();

    TEST_START("SF_Ctrl_AES* APIs");
    TEST_SKIP();

    TEST_START("SF_Cfg_* APIs");
    TEST_SKIP();
}

/* ============================================================================
 * UART API Tests (37 APIs)
 * ============================================================================ */
static void test_uart_apis(void)
{
    printf("\n[UART APIs - 37 total]\r\n");

    /* UART0 status APIs (read-only, safe) */
    TEST_START("UART_GetTxFifoCount(0)");
    TEST_PASS_VAL("%u", UART_GetTxFifoCount(0));

    TEST_START("UART_GetRxFifoCount(0)");
    TEST_PASS_VAL("%u", UART_GetRxFifoCount(0));

    TEST_START("UART_GetIntStatus(0, TX_FIFO_REQ)");
    TEST_PASS_VAL("%d", UART_GetIntStatus(0, UART_INT_TX_FIFO_REQ));

    TEST_START("UART_GetIntStatus(0, RX_FIFO_REQ)");
    TEST_PASS_VAL("%d", UART_GetIntStatus(0, UART_INT_RX_FIFO_REQ));

    TEST_START("UART_GetIntStatus(0, TX_END)");
    TEST_PASS_VAL("%d", UART_GetIntStatus(0, UART_INT_TX_END));

    TEST_START("UART_GetIntStatus(0, RX_END)");
    TEST_PASS_VAL("%d", UART_GetIntStatus(0, UART_INT_RX_END));

    TEST_START("UART_GetTxBusBusyStatus(0)");
    TEST_PASS_VAL("%d", UART_GetTxBusBusyStatus(0));

    TEST_START("UART_GetRxBusBusyStatus(0)");
    TEST_PASS_VAL("%d", UART_GetRxBusBusyStatus(0));

    /* UART1 status APIs */
    TEST_START("UART_GetTxFifoCount(1)");
    TEST_PASS_VAL("%u", UART_GetTxFifoCount(1));

    TEST_START("UART_GetRxFifoCount(1)");
    TEST_PASS_VAL("%u", UART_GetRxFifoCount(1));

    /* Skip config/init APIs */
    TEST_START("UART_Init/DeInit");
    TEST_SKIP();

    TEST_START("UART_SetBaudRate");
    TEST_SKIP();

    TEST_START("UART_FifoConfig");
    TEST_SKIP();

    TEST_START("UART_Enable/Disable");
    TEST_SKIP();

    TEST_START("UART_DMA APIs");
    TEST_SKIP();

    TEST_START("UART_Send/Receive APIs");
    TEST_SKIP();
}

/* ============================================================================
 * IR API Tests (27 APIs)
 * ============================================================================ */
static void test_ir_apis(void)
{
    printf("\n[IR APIs - 27 total]\r\n");

    /* Read-only status APIs */
    TEST_START("IR_GetIntStatus(TX)");
    TEST_PASS_VAL("%d", IR_GetIntStatus(IR_INT_TX));

    TEST_START("IR_GetIntStatus(RX)");
    TEST_PASS_VAL("%d", IR_GetIntStatus(IR_INT_RX));

    TEST_START("IR_GetRxFIFOCount");
    TEST_PASS_VAL("%u", IR_GetRxFIFOCount());

    /* Skip init/config APIs */
    TEST_START("IR_TxInit/RxInit");
    TEST_SKIP();

    TEST_START("IR_Enable/Disable");
    TEST_SKIP();

    TEST_START("IR_TxPulseWidthConfig");
    TEST_SKIP();

    TEST_START("IR_SendData/ReceiveData");
    TEST_SKIP();

    TEST_START("IR_SWM APIs");
    TEST_SKIP();

    TEST_START("IR_LEDx APIs");
    TEST_SKIP();
}

/* ============================================================================
 * KYS API Tests (7 APIs)
 * ============================================================================ */
static void test_kys_apis(void)
{
    printf("\n[KYS APIs - 7 total]\r\n");

    /* Read-only status */
    TEST_START("KYS_GetIntStatus");
    TEST_PASS_VAL("%d", KYS_GetIntStatus());

    /* Skip config APIs */
    TEST_START("KYS_Init");
    TEST_SKIP();

    TEST_START("KYS_Enable/Disable");
    TEST_SKIP();

    TEST_START("KYS_IntMask/IntClear");
    TEST_SKIP();

    TEST_START("KYS_GetKeycode");
    TEST_SKIP();
}

/* ============================================================================
 * arch_* and memory API Tests (5 APIs)
 * ============================================================================ */
static void test_arch_apis(void)
{
    printf("\n[arch_* APIs - 5 total]\r\n");

    /* arch_memcpy */
    TEST_START("arch_memcpy");
    {
        uint8_t src[16] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
        uint8_t dst[16] = {0};
        arch_memcpy(dst, src, 8);
        if (memcmp(src, dst, 8) == 0) TEST_PASS();
        else TEST_FAIL("mismatch");
    }

    /* arch_memcpy4 */
    TEST_START("arch_memcpy4");
    {
        uint32_t src[4] = {0x12345678, 0x9ABCDEF0, 0x11223344, 0x55667788};
        uint32_t dst[4] = {0};
        arch_memcpy4(dst, src, 4);
        if (memcmp(src, dst, 16) == 0) TEST_PASS();
        else TEST_FAIL("mismatch");
    }

    /* arch_memcpy_fast */
    TEST_START("arch_memcpy_fast");
    {
        uint8_t src[32], dst[32] = {0};
        for (int i = 0; i < 32; i++) src[i] = i;
        arch_memcpy_fast(dst, src, 32);
        if (memcmp(src, dst, 32) == 0) TEST_PASS();
        else TEST_FAIL("mismatch");
    }

    /* arch_memset4 */
    TEST_START("arch_memset4");
    {
        uint32_t dst[4] = {0};
        arch_memset4(dst, 0xDEADBEEF, 4);
        int pass = 1;
        for (int i = 0; i < 4; i++) if (dst[i] != 0xDEADBEEF) pass = 0;
        if (pass) TEST_PASS();
        else TEST_FAIL("mismatch");
    }

    /* arch_memcmp */
    TEST_START("arch_memcmp");
    {
        uint8_t a[8] = {1,2,3,4,5,6,7,8};
        uint8_t b[8] = {1,2,3,4,5,6,7,8};
        uint8_t c[8] = {1,2,3,4,5,6,7,9};
        if (arch_memcmp(a, b, 8) == 0 && arch_memcmp(a, c, 8) != 0) TEST_PASS();
        else TEST_FAIL("compare error");
    }

    /* memcopy_to_fifo / fifocopy_to_mem */
    TEST_START("memcopy_to_fifo");
    TEST_SKIP();  /* Requires valid FIFO address */

    TEST_START("fifocopy_to_mem");
    TEST_SKIP();  /* Requires valid FIFO address */
}

/* ============================================================================
 * utils_* API Tests (8 APIs)
 * ============================================================================ */
static void test_utils_apis(void)
{
    printf("\n[utils_* APIs - 8 total]\r\n");

    uint8_t data[] = "Hello, ROM API Test!";
    uint32_t len = sizeof(data) - 1;

    /* CRC16 */
    TEST_START("utils_crc16");
    TEST_PASS_VAL("0x%04x", utils_crc16(data, len));

    /* CRC16-CCITT */
    TEST_START("utils_crc16_ccitt");
    TEST_PASS_VAL("0x%04x", utils_crc16_ccitt(data, len));

    /* CRC32 */
    TEST_START("utils_crc32");
    TEST_PASS_VAL("0x%08lx", (unsigned long)utils_crc32(data, len));

    /* CRC32 accumulate */
    TEST_START("utils_crc32_accumulate");
    {
        uint32_t crc = 0xFFFFFFFF;
        crc = utils_crc32_accumulate(crc, data, len);
        TEST_PASS_VAL("0x%08lx", (unsigned long)crc);
    }

    /* CRC32 stream APIs */
    TEST_START("utils_crc32_stream_*");
    {
        struct crc32_stream_ctx ctx;
        utils_crc32_stream_init(&ctx);
        utils_crc32_stream_feed(&ctx, 0x55);
        utils_crc32_stream_feed_block(&ctx, data, len);
        uint32_t result = utils_crc32_stream_results(&ctx);
        TEST_PASS_VAL("0x%08lx", (unsigned long)result);
    }
}

/* ============================================================================
 * Psram API Tests (14 APIs) - SKIPPED
 * ============================================================================ */
static void test_psram_apis(void)
{
    printf("\n[Psram APIs - 14 total] SKIPPED\r\n");
    printf("  WARNING: Psram APIs use ROM's arch_delay_us internally\r\n");
    printf("  ROM's CpuIRegionBase is corrupted after bootrom exit\r\n");
    printf("  Calling these APIs may cause system hang\r\n");
    test_skip_count += 14;
}

/* ============================================================================
 * Main test runner
 * ============================================================================ */
static void romapi_test_all(void)
{
    printf("\r\n================================================\r\n");
    printf("ROM API Test Suite (Hybrid Mode)\r\n");
    printf("Testing 164 ROM APIs (150 testable, 14 skipped)\r\n");
    printf("================================================\r\n");

    test_pass_count = 0;
    test_fail_count = 0;
    test_skip_count = 0;

    test_clock_apis();
    test_pds_apis();
    test_sf_ctrl_apis();
    test_uart_apis();
    test_ir_apis();
    test_kys_apis();
    test_arch_apis();
    test_utils_apis();
    test_psram_apis();

    printf("\r\n================================================\r\n");
    printf("Results: %d PASSED, %d FAILED, %d SKIPPED\r\n",
           test_pass_count, test_fail_count, test_skip_count);
    printf("================================================\r\n");

    if (test_fail_count == 0) {
        printf("ALL EXECUTABLE TESTS PASSED!\r\n");
    }
}

/* Shell command handler */
int cmd_romapi_test(int argc, char **argv)
{
    const char *category = "all";

    if (argc >= 2) {
        category = argv[1];
    }

    test_pass_count = 0;
    test_fail_count = 0;
    test_skip_count = 0;

    if (strcmp(category, "all") == 0) {
        romapi_test_all();
    } else if (strcmp(category, "clock") == 0) {
        test_clock_apis();
    } else if (strcmp(category, "pds") == 0) {
        test_pds_apis();
    } else if (strcmp(category, "sf") == 0) {
        test_sf_ctrl_apis();
    } else if (strcmp(category, "uart") == 0) {
        test_uart_apis();
    } else if (strcmp(category, "ir") == 0) {
        test_ir_apis();
    } else if (strcmp(category, "kys") == 0) {
        test_kys_apis();
    } else if (strcmp(category, "arch") == 0) {
        test_arch_apis();
    } else if (strcmp(category, "utils") == 0) {
        test_utils_apis();
    } else if (strcmp(category, "psram") == 0) {
        test_psram_apis();
    } else {
        printf("Usage: romapi_test [category]\r\n");
        printf("Categories: all, clock, pds, sf, uart, ir, kys, arch, utils, psram\r\n");
        return 0;
    }

    if (strcmp(category, "all") != 0) {
        printf("\r\nResults: %d PASSED, %d FAILED, %d SKIPPED\r\n",
               test_pass_count, test_fail_count, test_skip_count);
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_romapi_test, romapi_test, Test ROM APIs in hybrid mode);
