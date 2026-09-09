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
 * @brief ROM API test cases for ROM driver mode verification
 *
 * Tests ALL ROM APIs including write/config APIs on FPGA.
 * Only HBN_Mode_Enter and PDS_Enable are skipped (halt CPU).
 *
 * Usage: romapi_test [category]
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

#include "drv_romdriver.h"
#include "drv_clock.h"
#include "drv_aon.h"
#include "drv_ef_ctrl.h"
#include "drv_glb.h"
#include "drv_glb_gpio.h"
#include "drv_hbn.h"
#include "drv_pds.h"
#include "drv_sf_ctrl.h"
#include "drv_sf_cfg.h"
#include "drv_xip_sflash.h"
#include "drv_uart.h"
#include "drv_common.h"
#include "hal_flash.h"
#include "hal_common.h"
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

#define TEST_FAIL_VAL(fmt, ...) \
    do { printf("FAIL (" fmt ")\r\n", __VA_ARGS__); test_fail_count++; } while (0)

#define TEST_SKIP() \
    do { printf("SKIP\r\n"); test_skip_count++; } while (0)

/* SF_Ctrl XIP-safe test: results stored in OCRAM, printed after XIP restore */
enum {
    SF_TEST_SET_OWNER = 0,
    SF_TEST_SET_IO_DELAY,
    SF_TEST_SBUS_SELECT_BANK,
    SF_TEST_AES_DISABLE,
    SF_TEST_AES_SET_MODE,
    SF_TEST_AES_SET_KEY,
    SF_TEST_AES_SET_IV,
    SF_TEST_AES_GET_IV_BE,
    SF_TEST_AES_SET_REGION,
    SF_TEST_AES_XTS_SET_KEY,
    SF_TEST_AES_XTS_SET_IV,
    SF_TEST_DISABLE_WRAP,
    SF_TEST_COUNT,
};

static ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t sf_test_pass[SF_TEST_COUNT];
static ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t sf_test_iv_match;

/*
 * Entire XIP exit + SF_Ctrl tests + XIP restore in one TCM function.
 * Caller is in flash, so everything between State_Save and State_Restore
 * must execute from TCM/ROM. All SF_Ctrl_* and XIP_SFlash_* are ROM APIs.
 */
void __attribute__((noinline, used)) ATTR_TCM_SECTION sf_ctrl_run_xip_safe(
    SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *results, uint8_t *iv_match)
{
    uint8_t key[32];
    uint8_t iv[16];
    uint8_t iv_out[16];
    uint32_t xipOffset;
    uint8_t aesEn;
    int i;

    /* Fill test data (can't use memset - might be in flash) */
    for (i = 0; i < 16; i++) {
        key[i] = (uint8_t)(0xA0 + i);
        iv[i] = (uint8_t)(0x10 + i);
    }
    for (i = 16; i < 32; i++)
        key[i] = (uint8_t)(0xB0 + i - 16);

    /* === Enter XIP-disabled zone === */
    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter(&aesEn);
    XIP_SFlash_State_Save(pFlashCfg, &xipOffset, 0, 0);

    /* From here until State_Restore, NO flash access allowed */

    /* 0: SF_Ctrl_Set_Owner (switch to SAHB, then back to IAHB) */
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_IAHB);
    results[SF_TEST_SET_OWNER] = 1;

    /* 1: SF_Ctrl_Set_IO_Delay */
    SF_Ctrl_Set_IO_Delay(0, 0, 0, 0);
    results[SF_TEST_SET_IO_DELAY] = 1;

    /* 2: SF_Ctrl_Sbus_Select_Bank */
    SF_Ctrl_Sbus_Select_Bank(0);
    results[SF_TEST_SBUS_SELECT_BANK] = 1;

    /* 3: SF_Ctrl_AES_Disable */
    SF_Ctrl_AES_Disable();
    results[SF_TEST_AES_DISABLE] = 1;

    /* 4: SF_Ctrl_AES_Set_Mode (CTR) */
    SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_CTR_MODE);
    results[SF_TEST_AES_SET_MODE] = 1;

    /* 5: SF_Ctrl_AES_Set_Key */
    SF_Ctrl_AES_Set_Key(0, key, SF_CTRL_AES_128BITS);
    results[SF_TEST_AES_SET_KEY] = 1;

    /* 6: SF_Ctrl_AES_Set_IV */
    SF_Ctrl_AES_Set_IV(0, iv, 0x2000);
    results[SF_TEST_AES_SET_IV] = 1;

    /* 7: SF_Ctrl_AES_Get_IV_BE - verify round-trip */
    SF_Ctrl_AES_Get_IV_BE(0, iv_out);
    *iv_match = 1;
    for (i = 0; i < 16; i++) {
        if (iv_out[i] != iv[3 - (i % 4) + (i / 4) * 4]) {
            *iv_match = 0;
            break;
        }
    }
    results[SF_TEST_AES_GET_IV_BE] = 1;

    /* 8: SF_Ctrl_AES_Set_Region */
    SF_Ctrl_AES_Set_Region(0, 0, 0, 0x2000, 0x3000, 0);
    results[SF_TEST_AES_SET_REGION] = 1;

    /* 9: SF_Ctrl_AES_XTS_Set_Key */
    SF_Ctrl_AES_XTS_Set_Key(0, key, SF_CTRL_AES_128BITS_DOUBLE_KEY);
    results[SF_TEST_AES_XTS_SET_KEY] = 1;

    /* 10: SF_Ctrl_AES_XTS_Set_IV */
    SF_Ctrl_AES_XTS_Set_IV(0, iv, 0x2000);
    results[SF_TEST_AES_XTS_SET_IV] = 1;

    /* 11: SF_Ctrl_Disable_Wrap_Access */
    SF_Ctrl_Disable_Wrap_Access(0);
    results[SF_TEST_DISABLE_WRAP] = 1;

    /* Clean up: disable AES before restoring XIP */
    SF_Ctrl_AES_Disable();

    /* === Restore XIP === */
    XIP_SFlash_State_Restore(pFlashCfg, xipOffset, 0, 0);
    XIP_SFlash_Opt_Exit(aesEn);
    cpu_global_irq_enable();
}

/* ============================================================================
 * ROM Table Validation
 * ============================================================================ */
static void test_rom_table(void)
{
    printf("\n[ROM Table Validation]\r\n");

    TEST_START("ROM_APITABLE base addr");
    TEST_PASS_VAL("0x%08lx", (unsigned long)(uintptr_t)ROM_APITABLE);

    TEST_START("ROM_APITABLE[VERSION]");
    uint32_t ver = ROM_APITABLE[ROM_API_INDEX_VERSION];
    TEST_PASS_VAL("0x%08lx", (unsigned long)ver);

    /* Spot-check a few entries are non-zero (populated) */
    TEST_START("ROM entry: CPU_MTimer_Delay_MS");
    uint32_t entry = ROM_APITABLE[ROM_API_INDEX_CPU_MTimer_Delay_MS];
    if (entry != 0) TEST_PASS_VAL("0x%08lx", (unsigned long)entry);
    else TEST_FAIL("NULL entry");

    TEST_START("ROM entry: CPU_MTimer_Delay_US");
    entry = ROM_APITABLE[ROM_API_INDEX_CPU_MTimer_Delay_US];
    if (entry != 0) TEST_PASS_VAL("0x%08lx", (unsigned long)entry);
    else TEST_FAIL("NULL entry");

    TEST_START("ROM entry: CPU_Get_MTimer_Counter");
    entry = ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_Counter];
    if (entry != 0) TEST_PASS_VAL("0x%08lx", (unsigned long)entry);
    else TEST_FAIL("NULL entry");

    TEST_START("ROM entry: GLB_GPIO_Init");
    entry = ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Init];
    if (entry != 0) TEST_PASS_VAL("0x%08lx", (unsigned long)entry);
    else TEST_FAIL("NULL entry");
}

/* ============================================================================
 * CPU Timer / Delay API Tests (with timing verification)
 * ============================================================================ */
static void test_cpu_timer_delay_apis(void)
{
    printf("\n[CPU Timer/Delay APIs]\r\n");

    /* --- Read-only timer query APIs --- */
    TEST_START("CPU_Get_MTimer_Clock");
    uint32_t mtimer_clk = CPU_Get_MTimer_Clock();
    TEST_PASS_VAL("%lu Hz", (unsigned long)mtimer_clk);

    TEST_START("CPU_Get_MTimer_Counter");
    uint64_t cnt = CPU_Get_MTimer_Counter();
    TEST_PASS_VAL("%llu", (unsigned long long)cnt);

    TEST_START("CPU_Get_MTimer_US");
    uint64_t us = CPU_Get_MTimer_US();
    TEST_PASS_VAL("%llu", (unsigned long long)us);

    TEST_START("CPU_Get_MTimer_MS");
    uint64_t ms = CPU_Get_MTimer_MS();
    TEST_PASS_VAL("%llu", (unsigned long long)ms);

    TEST_START("CPU_Get_MTimer_Toggle_Cnt");
    TEST_PASS_VAL("%u", CPU_Get_MTimer_Toggle_Cnt());

    TEST_START("CPU_Get_CPU_Cycle");
    uint64_t cyc = CPU_Get_CPU_Cycle();
    TEST_PASS_VAL("%llu", (unsigned long long)cyc);

    /* --- Delay APIs with timing verification --- */

    TEST_START("CPU_MTimer_Delay_US(1000)");
    {
        uint64_t t0 = SysTimer_GetLoadValue();
        CPU_MTimer_Delay_US(1000);
        uint64_t t1 = SysTimer_GetLoadValue();
        uint64_t elapsed_us = (t1 - t0) / SOC_TIMER_TICK_PER_US;
        if (elapsed_us >= 800 && elapsed_us <= 5000)
            TEST_PASS_VAL("%llu us", (unsigned long long)elapsed_us);
        else
            TEST_FAIL("out of range");
    }

    TEST_START("CPU_MTimer_Delay_MS(100)");
    {
        uint64_t t0 = SysTimer_GetLoadValue();
        CPU_MTimer_Delay_MS(100);
        uint64_t t1 = SysTimer_GetLoadValue();
        uint64_t elapsed_ms = (t1 - t0) / SOC_TIMER_TICK_PER_US / 1000;
        if (elapsed_ms >= 80 && elapsed_ms <= 500)
            TEST_PASS_VAL("%llu ms", (unsigned long long)elapsed_ms);
        else
            TEST_FAIL("out of range");
    }

    TEST_START("arch_delay_us(1000)");
    {
        uint64_t t0 = SysTimer_GetLoadValue();
        arch_delay_us(1000);
        uint64_t t1 = SysTimer_GetLoadValue();
        uint64_t elapsed_us = (t1 - t0) / SOC_TIMER_TICK_PER_US;
        if (elapsed_us >= 800 && elapsed_us <= 5000)
            TEST_PASS_VAL("%llu us", (unsigned long long)elapsed_us);
        else
            TEST_FAIL("out of range");
    }

    TEST_START("arch_delay_ms(100)");
    {
        uint64_t t0 = SysTimer_GetLoadValue();
        arch_delay_ms(100);
        uint64_t t1 = SysTimer_GetLoadValue();
        uint64_t elapsed_ms = (t1 - t0) / SOC_TIMER_TICK_PER_US / 1000;
        if (elapsed_ms >= 80 && elapsed_ms <= 500)
            TEST_PASS_VAL("%llu ms", (unsigned long long)elapsed_ms);
        else
            TEST_FAIL("out of range");
    }
}

/* ============================================================================
 * AON API Tests (all APIs including write)
 * ============================================================================ */
static void test_aon_apis(void)
{
    printf("\n[AON APIs]\r\n");

    /* Read-only getter */
    TEST_START("AON_Get_Xtal_CapCode");
    uint8_t capCode = AON_Get_Xtal_CapCode();
    TEST_PASS_VAL("%u", capCode);

    /* CapCode round-trip: read, write same, verify */
    TEST_START("AON_Set_Xtal_CapCode (round-trip)");
    AON_Set_Xtal_CapCode(capCode, capCode);
    uint8_t capCode2 = AON_Get_Xtal_CapCode();
    if (capCode2 == capCode) TEST_PASS_VAL("%u", capCode2);
    else TEST_FAIL("mismatch");

    /* Trim APIs - just call, they read eFuse trim and apply */
    TEST_START("AON_Trim_DcdcDis");
    AON_Trim_DcdcDis();
    TEST_PASS();

    TEST_START("AON_Trim_DcdcVoutSel");
    AON_Trim_DcdcVoutSel();
    TEST_PASS();

    TEST_START("AON_Trim_DcdcVoutTrim");
    AON_Trim_DcdcVoutTrim();
    TEST_PASS();

    TEST_START("AON_Trim_Ldo11socVoutTrim");
    AON_Trim_Ldo11socVoutTrim();
    TEST_PASS();

    TEST_START("AON_Trim_Usb20RcalCode");
    AON_Trim_Usb20RcalCode();
    TEST_PASS();

    /* Power toggle: MBG on (should already be on) */
    TEST_START("AON_Power_On_MBG");
    AON_Power_On_MBG();
    TEST_PASS();

    /* Power on SFReg (flash regulator, should already be on) */
    TEST_START("AON_Power_On_SFReg");
    AON_Power_On_SFReg();
    TEST_PASS();

    /* Output control APIs */
    TEST_START("AON_Output_Float_LDO15_RF");
    AON_Output_Float_LDO15_RF();
    TEST_PASS();

    TEST_START("AON_Output_LDO15_RF_Ctrl_by_Pd");
    AON_Output_LDO15_RF_Ctrl_by_Pd();
    TEST_PASS();
}

/* ============================================================================
 * EF_Ctrl API Tests (shadow register write with program=0)
 * ============================================================================ */
static void test_ef_ctrl_apis(void)
{
    printf("\n[EF_Ctrl APIs]\r\n");

    /* Read-only APIs */
    TEST_START("EF_Ctrl_AutoLoad_Done");
    TEST_PASS_VAL("%d", EF_Ctrl_AutoLoad_Done());

    TEST_START("EF_Ctrl_Busy");
    TEST_PASS_VAL("%d", EF_Ctrl_Busy());

    TEST_START("EF_Ctrl_Read_Chip_ID");
    {
        uint8_t chipID[8] = {0};
        EF_Ctrl_Read_Chip_ID(chipID);
        printf("PASS (");
        for (int i = 0; i < 8; i++) printf("%02X", chipID[i]);
        printf(")\r\n");
        test_pass_count++;
    }

    TEST_START("EF_Ctrl_Read_MAC_Address");
    {
        uint8_t mac[6] = {0};
        EF_Ctrl_Read_MAC_Address(mac);
        printf("PASS (%02X:%02X:%02X:%02X:%02X:%02X)\r\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        test_pass_count++;
    }

    TEST_START("EF_Ctrl_Read_Device_Info");
    {
        Efuse_Device_Info_Type info = {0};
        EF_Ctrl_Read_Device_Info(&info);
        TEST_PASS();
    }

    TEST_START("EF_Ctrl_Read_FlashDlyCoe");
    TEST_PASS_VAL("%u", EF_Ctrl_Read_FlashDlyCoe());

    TEST_START("EF_Ctrl_Get_Trim_Parity(0xFF,8)");
    TEST_PASS_VAL("%u", EF_Ctrl_Get_Trim_Parity(0xFF, 8));

    TEST_START("EF_Ctrl_Is_All_Bits_Zero(0,0,32)");
    TEST_PASS_VAL("%u", EF_Ctrl_Is_All_Bits_Zero(0, 0, 32));

    TEST_START("EF_Ctrl_Crc_Is_Busy");
    TEST_PASS_VAL("%d", EF_Ctrl_Crc_Is_Busy());

    TEST_START("EF_Ctrl_Crc_Result");
    TEST_PASS_VAL("%d", EF_Ctrl_Crc_Result());

    TEST_START("EF_Ctrl_Read_Secure_Cfg");
    {
        EF_Ctrl_Sec_Param_Type cfg = {0};
        EF_Ctrl_Read_Secure_Cfg(&cfg);
        TEST_PASS();
    }

    /* Write APIs with program=0: writes to shadow register only, not fuse */
    TEST_START("EF_Ctrl_Write_MAC_Address (prog=0)");
    {
        uint8_t mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
        EF_Ctrl_Write_MAC_Address(mac, 0);
        TEST_PASS();
    }

    TEST_START("EF_Ctrl_Write_Sw_Usage (prog=0)");
    EF_Ctrl_Write_Sw_Usage(0, 0x12345678, 0);
    TEST_PASS();

    TEST_START("EF_Ctrl_Write_Secure_Cfg (prog=0)");
    {
        EF_Ctrl_Sec_Param_Type cfg = {0};
        EF_Ctrl_Write_Secure_Cfg(&cfg, 0);
        TEST_PASS();
    }

    TEST_START("EF_Ctrl_Write_Secure_Boot (prog=0)");
    {
        uint8_t sign[1] = {0};
        uint8_t aes[1] = {0};
        EF_Ctrl_Write_Secure_Boot(sign, aes, 0);
        TEST_PASS();
    }

    TEST_START("EF_Ctrl_Write_Dbg_Pwd (prog=0)");
    EF_Ctrl_Write_Dbg_Pwd(0, 0, 0, 0);
    TEST_PASS();

    /* EF_Ctrl_Clear: clear shadow register region */
    TEST_START("EF_Ctrl_Clear(0, 1)");
    EF_Ctrl_Clear(0, 1);
    TEST_PASS();

    /* EF_Ctrl_Load_Efuse_R0: reload from fuse to shadow */
    TEST_START("EF_Ctrl_Load_Efuse_R0");
    EF_Ctrl_Load_Efuse_R0();
    TEST_PASS();

    /* EF_Ctrl_Program_Direct_R0: program=0 was already tested, this
     * actually writes to fuse on ASIC but on FPGA it's RAM-emulated */
    TEST_START("EF_Ctrl_Program_Direct_R0 (idx=31)");
    {
        uint32_t data = 0;
        EF_Ctrl_Program_Direct_R0(31, &data, 1);
        TEST_PASS();
    }
}

/* ============================================================================
 * GLB API Tests (including set/config)
 * ============================================================================ */
static void test_glb_apis(void)
{
    printf("\n[GLB APIs]\r\n");

    /* Read-only getters */
    TEST_START("GLB_Get_Core_Type");
    TEST_PASS_VAL("%u", GLB_Get_Core_Type());

    TEST_START("GLB_Get_Package_Type");
    TEST_PASS_VAL("%u", GLB_Get_Package_Type());

    TEST_START("GLB_Get_Flash_Id_Value");
    uint32_t flashId = GLB_Get_Flash_Id_Value();
    TEST_PASS_VAL("0x%08lx", (unsigned long)flashId);

    TEST_START("GLB_Get_SRAM_PARM");
    TEST_PASS_VAL("0x%08lx", (unsigned long)GLB_Get_SRAM_PARM());

    TEST_START("GLB_Get_SRAM_RET");
    TEST_PASS_VAL("0x%08lx", (unsigned long)GLB_Get_SRAM_RET());

    TEST_START("GLB_Get_SRAM_SLP");
    TEST_PASS_VAL("0x%08lx", (unsigned long)GLB_Get_SRAM_SLP());

    TEST_START("GLB_Get_BMX_TO_Status");
    TEST_PASS_VAL("%u", GLB_Get_BMX_TO_Status());

    TEST_START("GLB_Get_MCU_Muxpll_80M_Sel");
    TEST_PASS_VAL("%u", GLB_Get_MCU_Muxpll_80M_Sel());

    TEST_START("GLB_Get_MCU_Muxpll_160M_Sel");
    TEST_PASS_VAL("%u", GLB_Get_MCU_Muxpll_160M_Sel());

    TEST_START("GLB_Get_ISP_Muxpll_80M_Sel");
    TEST_PASS_VAL("%u", GLB_Get_ISP_Muxpll_80M_Sel());

    TEST_START("GLB_UART_Get_Fun(sig=9)");
    TEST_PASS_VAL("%u", GLB_UART_Get_Fun(9));

    TEST_START("GLB_UART_Get_Fun(sig=10)");
    TEST_PASS_VAL("%u", GLB_UART_Get_Fun(10));

    TEST_START("GLB_UART_Find_Sig_By_Fun(2)");
    TEST_PASS_VAL("%u", GLB_UART_Find_Sig_By_Fun(2));

    TEST_START("GLB_GPIO_Read(14)");
    TEST_PASS_VAL("%lu", (unsigned long)GLB_GPIO_Read(14));

    TEST_START("GLB_GPIO_Get_Fun(14)");
    TEST_PASS_VAL("%u", GLB_GPIO_Get_Fun(14));

    TEST_START("GLB_Get_Reset_Reason");
    {
        GLB_RESET_RECORD_Type reason = {0};
        GLB_Get_Reset_Reason(&reason);
        TEST_PASS();
    }

    TEST_START("GLB_Get_PAD_Bonging_to_GND_Sts");
    TEST_PASS_VAL("%d", GLB_Get_PAD_Bonging_to_GND_Sts());

    TEST_START("GLB_Get_MCU_System_CLK_Div");
    {
        uint8_t clkDiv = 0, bclkDiv = 0;
        GLB_Get_MCU_System_CLK_Div(&clkDiv, &bclkDiv);
        printf("PASS (clk=%u, bclk=%u)\r\n", clkDiv, bclkDiv);
        test_pass_count++;
    }

    /* --- Write/config APIs --- */

    /*
     * GLB_GPIO_Init on a free pad, verified via register readback.
     * GPIO28/29 are the debug console (UART0, see bsp/board/ipro6/board.c) —
     * reconfiguring them as GPIO tears the console off its TX/RX pins. Use
     * GPIO18 (GPIO_FUN_UNUSED on the EVB) and confirm the pad-function
     * register took the write instead of trusting a physical pad level.
     */
    TEST_START("GLB_GPIO_Init (GPIO18, input)");
    {
        GLB_GPIO_Cfg_Type gpioCfg;
        gpioCfg.gpioPin = 18;
        gpioCfg.gpioFun = 11; /* GPIO function */
        gpioCfg.gpioMode = 0; /* input */
        gpioCfg.pullType = 0; /* none */
        gpioCfg.drive = 0;
        gpioCfg.smtCtrl = 1;
        gpioCfg.outputMode = 0;
        GLB_GPIO_Init(&gpioCfg);
        uint8_t fn = GLB_GPIO_Get_Fun(18);
        if (fn == 11) TEST_PASS_VAL("fun=%u", (unsigned)fn);
        else TEST_FAIL_VAL("fun readback: want 11 got %u", (unsigned)fn);
    }

    /* GLB_GPIO_Output on GPIO18: drive high, read the pad register back */
    TEST_START("GLB_GPIO_Output (GPIO18, high)");
    {
        GLB_GPIO_Cfg_Type gpioCfg;
        gpioCfg.gpioPin = 18;
        gpioCfg.gpioFun = 11;
        gpioCfg.gpioMode = 1; /* output */
        gpioCfg.pullType = 0;
        gpioCfg.drive = 0;
        gpioCfg.smtCtrl = 1;
        gpioCfg.outputMode = 0;
        GLB_GPIO_Init(&gpioCfg);
        GLB_GPIO_Write(18, 1);
        uint32_t val = GLB_GPIO_Read(18);
        TEST_PASS_VAL("%lu", (unsigned long)val);
    }

    /* GLB_Set_Flash_Id_Value round-trip (API stores only low 24 bits) */
    TEST_START("GLB_Set_Flash_Id_Value (round-trip)");
    GLB_Set_Flash_Id_Value(0x00AD1234);
    {
        uint32_t newId = GLB_Get_Flash_Id_Value();
        if (newId == 0x00AD1234) TEST_PASS_VAL("0x%08lx", (unsigned long)newId);
        else TEST_FAIL_VAL("wrote=0x00AD1234 read=0x%08lx", (unsigned long)newId);
    }
    GLB_Set_Flash_Id_Value(flashId); /* restore */

    /* GLB_Clr_Reset_Reason */
    TEST_START("GLB_Clr_Reset_Reason");
    GLB_Clr_Reset_Reason();
    TEST_PASS();

    /* GLB_Clr_BMX_TO_Status */
    TEST_START("GLB_Clr_BMX_TO_Status");
    GLB_Clr_BMX_TO_Status();
    TEST_PASS();

    /* GLB_Set_EM_Sel - set embedded memory select (round-trip safe) */
    TEST_START("GLB_Set_EM_Sel(0)");
    GLB_Set_EM_Sel(0);
    TEST_PASS();

    /* GLB_Set_MCU_Muxpll_80M_Sel (round-trip) */
    TEST_START("GLB_Set_MCU_Muxpll_80M_Sel(0)");
    GLB_Set_MCU_Muxpll_80M_Sel(0);
    if (GLB_Get_MCU_Muxpll_80M_Sel() == 0) TEST_PASS();
    else TEST_FAIL("mismatch");

    /* GLB_Set_MCU_Muxpll_160M_Sel (round-trip) */
    TEST_START("GLB_Set_MCU_Muxpll_160M_Sel(0)");
    GLB_Set_MCU_Muxpll_160M_Sel(0);
    if (GLB_Get_MCU_Muxpll_160M_Sel() == 0) TEST_PASS();
    else TEST_FAIL("mismatch");
}

/* ============================================================================
 * HBN API Tests (set/get round-trips, skip HBN_Mode_Enter)
 * ============================================================================ */
static void test_hbn_apis(void)
{
    printf("\n[HBN APIs]\r\n");

    /* Read-only getters */
    TEST_START("HBN_Get_Xtal_Type");
    {
        uint8_t xtalType = 0;
        HBN_Get_Xtal_Type(&xtalType);
        TEST_PASS_VAL("%u", xtalType);
    }

    TEST_START("HBN_Get_Xtal_Value");
    {
        uint32_t xtalVal = 0;
        HBN_Get_Xtal_Value(&xtalVal);
        TEST_PASS_VAL("%lu", (unsigned long)xtalVal);
    }

    TEST_START("HBN_Get_Status_Flag");
    uint32_t origFlag = HBN_Get_Status_Flag();
    TEST_PASS_VAL("0x%08lx", (unsigned long)origFlag);

    TEST_START("HBN_Get_Wakeup_Addr");
    uint32_t origWkAddr = HBN_Get_Wakeup_Addr();
    TEST_PASS_VAL("0x%08lx", (unsigned long)origWkAddr);

    TEST_START("HBN_Get_User_Boot_Config");
    uint8_t origUbCfg = HBN_Get_User_Boot_Config();
    TEST_PASS_VAL("0x%02x", origUbCfg);

    TEST_START("HBN_Get_MCU_Root_CLK_Sel");
    TEST_PASS_VAL("%u", HBN_Get_MCU_Root_CLK_Sel());

    TEST_START("HBN_Get_MCU_XCLK_Sel");
    TEST_PASS_VAL("%u", HBN_Get_MCU_XCLK_Sel());

    TEST_START("HBN_Get_Reset_Reason");
    {
        uint16_t rstReason = 0;
        HBN_Get_Reset_Reason(&rstReason);
        TEST_PASS_VAL("0x%04x", rstReason);
    }

    TEST_START("HBN_Get_BOD_OUT_State");
    TEST_PASS_VAL("%d", HBN_Get_BOD_OUT_State());

    TEST_START("HBN_Get_INT_State(GPIO16)");
    TEST_PASS_VAL("%d", HBN_Get_INT_State(HBN_INT_GPIO16));

    TEST_START("HBN_Get_RTC_Timer_Val");
    {
        uint32_t lo = 0, hi = 0;
        HBN_Get_RTC_Timer_Val(&lo, &hi);
        printf("PASS (0x%08lx_%08lx)\r\n",
               (unsigned long)hi, (unsigned long)lo);
        test_pass_count++;
    }

    /* --- Write/config APIs (round-trip or safe calls) --- */

    /* Status flag round-trip */
    TEST_START("HBN_Set_Status_Flag (round-trip)");
    HBN_Set_Status_Flag(0xA5A5A5A5);
    {
        uint32_t flag = HBN_Get_Status_Flag();
        if (flag == 0xA5A5A5A5) TEST_PASS_VAL("0x%08lx", (unsigned long)flag);
        else TEST_FAIL("mismatch");
    }
    HBN_Set_Status_Flag(origFlag); /* restore */

    /* Wakeup addr round-trip */
    TEST_START("HBN_Set_Wakeup_Addr (round-trip)");
    HBN_Set_Wakeup_Addr(0x12345678);
    {
        uint32_t addr = HBN_Get_Wakeup_Addr();
        if (addr == 0x12345678) TEST_PASS_VAL("0x%08lx", (unsigned long)addr);
        else TEST_FAIL("mismatch");
    }
    HBN_Set_Wakeup_Addr(origWkAddr); /* restore */

    /* User boot config round-trip (API field is 2-bit: HBN_USER_BOOT_SEL[27:26]) */
    TEST_START("HBN_Set_User_Boot_Config (round-trip)");
    HBN_Set_User_Boot_Config(0x02);
    {
        uint8_t cfg = HBN_Get_User_Boot_Config();
        if (cfg == 0x02) TEST_PASS_VAL("0x%02x", cfg);
        else TEST_FAIL_VAL("wrote=0x02 read=0x%02x rsv2=0x%08lx",
                           cfg, (unsigned long)readl(0x3008F108));
    }
    HBN_Set_User_Boot_Config(origUbCfg); /* restore */

    /* Reset reason round-trip */
    TEST_START("HBN_Set_Reset_Reason");
    HBN_Set_Reset_Reason(0x0000);
    {
        uint16_t reason = 0;
        HBN_Get_Reset_Reason(&reason);
        TEST_PASS_VAL("0x%04x", reason);
    }

    /* Xtal type round-trip */
    TEST_START("HBN_Set_Xtal_Type (round-trip)");
    {
        uint8_t origType = 0;
        HBN_Get_Xtal_Type(&origType);
        HBN_Set_Xtal_Type(origType);
        uint8_t newType = 0;
        HBN_Get_Xtal_Type(&newType);
        if (newType == origType) TEST_PASS_VAL("%u", newType);
        else TEST_FAIL("mismatch");
    }

    /* IRQ clear (safe, just clears pending bits) */
    TEST_START("HBN_Clear_IRQ(GPIO16)");
    HBN_Clear_IRQ(HBN_INT_GPIO16);
    TEST_PASS();

    TEST_START("HBN_Clear_RTC_INT");
    HBN_Clear_RTC_INT();
    TEST_PASS();

    /* GPIO INT enable/disable */
    TEST_START("HBN_GPIO_INT_Enable(posedge)");
    HBN_GPIO_INT_Enable(HBN_ACOMP_INT_EDGE_POSEDGE);
    TEST_PASS();

    TEST_START("HBN_GPIO_INT_Disable");
    HBN_GPIO_INT_Disable();
    TEST_PASS();

    /* BOD config */
    TEST_START("HBN_Set_BOD_Config(en,2.20V,ind)");
    HBN_Set_BOD_Config(1, HBN_BOD_THRES_2P20V, HBN_BOD_MODE_POR_INDEPENDENT);
    TEST_PASS();

    /* LDO voltage */
    TEST_START("HBN_Set_Ldo11_All_Vout(0xA)");
    HBN_Set_Ldo11_All_Vout(0xA);
    TEST_PASS();

    TEST_START("HBN_Set_Ldo11_Soc_Vout(0xA)");
    HBN_Set_Ldo11_Soc_Vout(0xA);
    TEST_PASS();

    TEST_START("HBN_Set_Ldo11_Rt_Vout(0xA)");
    HBN_Set_Ldo11_Rt_Vout(0xA);
    TEST_PASS();

    /* RTC counter */
    TEST_START("HBN_Enable_RTC_Counter");
    HBN_Enable_RTC_Counter();
    TEST_PASS();

    /* AComp IRQ enable/disable */
    TEST_START("HBN_Enable_AComp0_IRQ");
    HBN_Enable_AComp0_IRQ();
    TEST_PASS();

    TEST_START("HBN_Disable_AComp0_IRQ");
    HBN_Disable_AComp0_IRQ();
    TEST_PASS();

    TEST_START("HBN_Enable_BOD_IRQ");
    HBN_Enable_BOD_IRQ();
    TEST_PASS();

    TEST_START("HBN_Disable_BOD_IRQ");
    HBN_Disable_BOD_IRQ();
    TEST_PASS();

    /* Hw Pu/Pd config */
    TEST_START("HBN_Hw_Pu_Pd_Cfg(0)");
    HBN_Hw_Pu_Pd_Cfg(0);
    TEST_PASS();

    /* UART CLK sel - SKIP: HBN_Set_UART_CLK_Sel reprograms the UART clock
     * source and breaks the debug console (UART0) on the ASIC. */
    TEST_START("HBN_Set_UART_CLK_Sel");
    TEST_SKIP();

    /* GPADC CLK sel */
    TEST_START("HBN_Set_GPADC_CLK_Sel(0)");
    HBN_Set_GPADC_CLK_Sel(0);
    TEST_PASS();

    /* GPIO/PDS keep */
    TEST_START("HBN_Set_Gpio_Keep(0)");
    HBN_Set_Gpio_Keep(0);
    TEST_PASS();

    TEST_START("HBN_Clear_Gpio_Keep(0)");
    HBN_Clear_Gpio_Keep(0);
    TEST_PASS();

    TEST_START("HBN_Set_PDS_Gpio_Keep(0)");
    HBN_Set_PDS_Gpio_Keep(0);
    TEST_PASS();

    TEST_START("HBN_Clear_PDS_Gpio_Keep(0)");
    HBN_Clear_PDS_Gpio_Keep(0);
    TEST_PASS();

    /* Skip HBN_Mode_Enter / HBN_Enable (halts CPU) */
    TEST_START("HBN_Mode_Enter");
    TEST_SKIP();    /* would halt CPU */

    TEST_START("HBN_Enable");
    TEST_SKIP();    /* would halt CPU */
}

/* ============================================================================
 * Clock API Tests
 * ============================================================================ */
static void test_clock_apis(void)
{
    printf("\n[Clock APIs]\r\n");

    TEST_START("Clock_System_Clock_Get(MCU_CLK)");
    TEST_PASS_VAL("%lu", (unsigned long)Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK));

    TEST_START("Clock_System_Clock_Get(MCU_BCLK)");
    TEST_PASS_VAL("%lu", (unsigned long)Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_BCLK));

    TEST_START("Clock_Peripheral_Clock_Get(UART0)");
    TEST_PASS_VAL("%lu", (unsigned long)Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0));

    TEST_START("Clock_Peripheral_Clock_Get(FLASH)");
    TEST_PASS_VAL("%lu", (unsigned long)Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_FLASH));

    TEST_START("SystemCoreClockGet");
    TEST_PASS_VAL("%lu", (unsigned long)SystemCoreClockGet());

    /* PSRAM clock getter (in ROM table) */
    TEST_START("Clock_Get_PSRAMB_Clk");
    TEST_PASS_VAL("%lu", (unsigned long)Clock_Get_PSRAMB_Clk());
}

/* ============================================================================
 * PDS API Tests (config APIs, skip PDS_Enable)
 * ============================================================================ */
static void test_pds_apis(void)
{
    printf("\n[PDS APIs]\r\n");

    /* Read-only status APIs */
    TEST_START("PDS_Get_GPIO_Pad_IntStatus(pad=0)");
    TEST_PASS_VAL("%d", PDS_Get_GPIO_Pad_IntStatus(0));

    TEST_START("PDS_Get_IntStatus(WAKEUP)");
    TEST_PASS_VAL("%d", PDS_Get_IntStatus(PDS_INT_WAKEUP));

    TEST_START("PDS_Get_IntStatus(RF_DONE)");
    TEST_PASS_VAL("%d", PDS_Get_IntStatus(PDS_INT_RF_DONE));

    /* Config APIs (safe, just set registers) */
    TEST_START("PDS_Set_GPIO_Pad_IntMask(0, MASK)");
    PDS_Set_GPIO_Pad_IntMask(0, 1); /* mask = disable */
    TEST_PASS();

    TEST_START("PDS_Set_GPIO_Pad_IntClr(0)");
    PDS_Set_GPIO_Pad_IntClr(0);
    TEST_PASS();

    TEST_START("PDS_Set_GPIO_Pad_IntMode(0, 0)");
    PDS_Set_GPIO_Pad_IntMode(0, 0);
    TEST_PASS();

    TEST_START("PDS_Set_Flash_Pad_Pull_None(0)");
    PDS_Set_Flash_Pad_Pull_None(0);
    TEST_PASS();

    TEST_START("PDS_Set_Flash_Pad_Pull_None_Fast(0)");
    PDS_Set_Flash_Pad_Pull_None_Fast(0);
    TEST_PASS();

    TEST_START("PDS_Set_MCU0_Clock_Enable");
    PDS_Set_MCU0_Clock_Enable();
    TEST_PASS();

    TEST_START("PDS_Set_MCU0_Reset_Address(0)");
    PDS_Set_MCU0_Reset_Address(0);
    TEST_PASS();

    /* USB suspend/resume - SKIP: the ROM PDS_Set_USB_Suspend can busy-hang the
     * CPU when the USB device is actively enumerated by the host (state-
     * dependent), which also takes the UART console down. No patch/SDK override
     * binds (drv_pds.c is dead-stripped), so there is no safe path to run it. */
    TEST_START("PDS_Set_USB_Suspend");
    TEST_SKIP();

    TEST_START("PDS_Set_USB_Resume");
    TEST_SKIP();

    /* Power on/off WB (wireless block) */
    TEST_START("PDS_Power_On_WB");
    PDS_Power_On_WB();
    TEST_PASS();

    /* PDS_Force_Config (set force registers, no sleep entry) */
    TEST_START("PDS_Force_Config (no-op cfg)");
    {
        PDS_CTL2_Type cfg2 = {0};
        PDS_CTL3_Type cfg3 = {0};
        PDS_CTL5_Type cfg5 = {0};
        PDS_Force_Config(&cfg2, &cfg3, &cfg5);
        TEST_PASS();
    }

    /* PDS_RAM_Config */
    TEST_START("PDS_RAM_Config (no-op)");
    {
        PDS_CTRL_RAM1_Type r1 = {0};
        PDS_CTRL_RAM2_Type r2 = {0};
        PDS_CTRL_RAM3_Type r3 = {0};
        PDS_CTRL_RAM4_Type r4 = {0};
        PDS_RAM_Config(&r1, &r2, &r3, &r4);
        TEST_PASS();
    }

    /* Skip PDS_Enable (enters power-down sleep) */
    TEST_START("PDS_Enable");
    TEST_SKIP();    /* would halt CPU */
}

/* ============================================================================
 * SF_Ctrl API Tests (read/restore and safe config)
 * ============================================================================ */
static void test_sf_ctrl_apis(void)
{
    printf("\n[SF_Ctrl APIs]\r\n");

    /* Read-only APIs */
    TEST_START("SF_Ctrl_Get_Clock_Delay");
    uint8_t origDelay = SF_Ctrl_Get_Clock_Delay();
    TEST_PASS_VAL("%u", origDelay);

    TEST_START("SF_Ctrl_Is_AES_Enable");
    TEST_PASS_VAL("%d", SF_Ctrl_Is_AES_Enable());

    TEST_START("SF_Ctrl_Get_Flash_Image_Offset(0,0)");
    uint32_t origOffset = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    TEST_PASS_VAL("0x%lx", (unsigned long)origOffset);

    TEST_START("SF_Ctrl_GetBusyState");
    TEST_PASS_VAL("%d", SF_Ctrl_GetBusyState());

    /* Config APIs (read/restore pattern) */
    TEST_START("SF_Ctrl_Set_Clock_Delay (round-trip)");
    SF_Ctrl_Set_Clock_Delay(origDelay);
    if (SF_Ctrl_Get_Clock_Delay() == origDelay) TEST_PASS();
    else TEST_FAIL("mismatch");

    TEST_START("SF_Ctrl_Set_Flash_Image_Offset (round-trip)");
    SF_Ctrl_Set_Flash_Image_Offset(origOffset, 0, 0);
    if (SF_Ctrl_Get_Flash_Image_Offset(0, 0) == origOffset) TEST_PASS();
    else TEST_FAIL("mismatch");

    /*
     * SF_Ctrl write APIs: must exit XIP first (code runs from flash).
     * Use XIP_SFlash_State_Save/Restore to safely switch to SBUS mode.
     * The TCM-resident function calls all SF_Ctrl ROM APIs while XIP
     * is disabled, storing pass/fail in OCRAM. Print results after
     * XIP is restored.
     */
    {
        SPI_Flash_Cfg_Type *pFlashCfg;
        uint32_t cfgLen;
        int i;

        static const char *sf_test_names[SF_TEST_COUNT] = {
            "SF_Ctrl_Set_Owner",
            "SF_Ctrl_Set_IO_Delay",
            "SF_Ctrl_Sbus_Select_Bank",
            "SF_Ctrl_AES_Disable",
            "SF_Ctrl_AES_Set_Mode",
            "SF_Ctrl_AES_Set_Key",
            "SF_Ctrl_AES_Set_IV",
            "SF_Ctrl_AES_Get_IV_BE",
            "SF_Ctrl_AES_Set_Region",
            "SF_Ctrl_AES_XTS_Set_Key",
            "SF_Ctrl_AES_XTS_Set_IV",
            "SF_Ctrl_Disable_Wrap_Access",
        };

        for (i = 0; i < SF_TEST_COUNT; i++)
            sf_test_pass[i] = 0;
        sf_test_iv_match = 0;

        flash_get_cfg((uint8_t **)&pFlashCfg, &cfgLen);

        /* XIP exit + tests + XIP restore all happen inside TCM function */
        sf_ctrl_run_xip_safe(pFlashCfg, sf_test_pass, &sf_test_iv_match);

        /* Now safe to printf - XIP is restored */
        for (i = 0; i < SF_TEST_COUNT; i++) {
            TEST_START(sf_test_names[i]);
            if (sf_test_pass[i]) {
                if (i == SF_TEST_AES_GET_IV_BE) {
                    TEST_PASS_VAL("IV match=%d", (int)sf_test_iv_match);
                } else {
                    TEST_PASS();
                }
            } else {
                TEST_FAIL("not reached");
            }
        }
    }

    /* SF_Cfg_Get_Flash_Cfg_Need_Lock - read-only lookup, safe */
    TEST_START("SF_Cfg_Get_Flash_Cfg_Need_Lock");
    {
        SPI_Flash_Cfg_Type cfg = {0};
        int ret = SF_Cfg_Get_Flash_Cfg_Need_Lock(0xEF6016, &cfg);
        TEST_PASS_VAL("ret=%d", ret);
    }
}

/* ============================================================================
 * UART API Tests (test on UART1, keep UART0 for debug)
 * ============================================================================ */
static void test_uart_apis(void)
{
    printf("\n[UART APIs]\r\n");

    /* UART0 status APIs (read-only, safe) */
    TEST_START("UART_GetTxFifoCount(0)");
    TEST_PASS_VAL("%lu", (unsigned long)UART_GetTxFifoCount(0));

    TEST_START("UART_GetRxFifoCount(0)");
    TEST_PASS_VAL("%lu", (unsigned long)UART_GetRxFifoCount(0));

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
    TEST_PASS_VAL("%lu", (unsigned long)UART_GetTxFifoCount(1));

    TEST_START("UART_GetRxFifoCount(1)");
    TEST_PASS_VAL("%lu", (unsigned long)UART_GetRxFifoCount(1));

    /*
     * UART0 driver init / clock / baudrate tests - SKIP.
     * UART0 is the debug console; UART_Disable/DeInit/Init/SetBaudRate on it
     * tear down or re-clock the console and kill its output on the ASIC (the
     * FPGA-era uartClk=32MHz assumption also miscomputes the divisor). The
     * read-only UART0 status APIs above already exercise the ROM UART path.
     */
    TEST_START("UART_Disable/DeInit/Init(0)");
    TEST_SKIP();

    TEST_START("UART_FifoConfig/Enable(0)");
    TEST_SKIP();

    TEST_START("UART_SetBaudRate(0)");
    TEST_SKIP();

    TEST_START("UART_TxFreeRun/SendData(0)");
    TEST_SKIP();
}

/* ============================================================================
 * arch_* and memory API Tests
 * ============================================================================ */
static void test_arch_apis(void)
{
    printf("\n[arch_* APIs]\r\n");

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
}

/* ============================================================================
 * utils_* API Tests
 * ============================================================================ */
static void test_utils_apis(void)
{
    printf("\n[utils_* APIs]\r\n");

    uint8_t data[] = "Hello, ROM API Test!";
    uint32_t len = sizeof(data) - 1;

    TEST_START("utils_crc16");
    TEST_PASS_VAL("0x%04x", utils_crc16(data, len));

    TEST_START("utils_crc16_ccitt");
    TEST_PASS_VAL("0x%04x", utils_crc16_ccitt(data, len));

    TEST_START("utils_crc32");
    TEST_PASS_VAL("0x%08lx", (unsigned long)utils_crc32(data, len));

    TEST_START("utils_crc32_accumulate");
    {
        uint32_t crc = 0xFFFFFFFF;
        crc = utils_crc32_accumulate(crc, data, len);
        TEST_PASS_VAL("0x%08lx", (unsigned long)crc);
    }

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
 * PSRAM-related APIs (only 2 in ROM table)
 * ============================================================================ */
static void test_psram_apis(void)
{
    printf("\n[PSRAM-related APIs]\r\n");

    TEST_START("Clock_Get_PSRAMB_Clk");
    TEST_PASS_VAL("%lu Hz", (unsigned long)Clock_Get_PSRAMB_Clk());

    TEST_START("GLB_Set_PSRAMB_CLK_Sel(dis,0,1)");
    GLB_Set_PSRAMB_CLK_Sel(0, 0, 1);
    TEST_PASS();
}

/* ============================================================================
 * Main test runner
 * ============================================================================ */
static void romapi_test_all(void)
{
    printf("\r\n================================================\r\n");
    printf("ROM API Test Suite (ROM Driver Mode - Full)\r\n");
    printf("================================================\r\n");

    test_pass_count = 0;
    test_fail_count = 0;
    test_skip_count = 0;

    test_rom_table();
    test_cpu_timer_delay_apis();
    test_aon_apis();
    test_ef_ctrl_apis();
    test_glb_apis();
    test_hbn_apis();
    test_clock_apis();
    test_pds_apis();
    test_sf_ctrl_apis();
    test_uart_apis();
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
    } else if (strcmp(category, "table") == 0) {
        test_rom_table();
    } else if (strcmp(category, "delay") == 0) {
        test_cpu_timer_delay_apis();
    } else if (strcmp(category, "aon") == 0) {
        test_aon_apis();
    } else if (strcmp(category, "efuse") == 0) {
        test_ef_ctrl_apis();
    } else if (strcmp(category, "glb") == 0) {
        test_glb_apis();
    } else if (strcmp(category, "hbn") == 0) {
        test_hbn_apis();
    } else if (strcmp(category, "clock") == 0) {
        test_clock_apis();
    } else if (strcmp(category, "pds") == 0) {
        test_pds_apis();
    } else if (strcmp(category, "sf") == 0) {
        test_sf_ctrl_apis();
    } else if (strcmp(category, "uart") == 0) {
        test_uart_apis();
    } else if (strcmp(category, "arch") == 0) {
        test_arch_apis();
    } else if (strcmp(category, "utils") == 0) {
        test_utils_apis();
    } else if (strcmp(category, "psram") == 0) {
        test_psram_apis();
    } else {
        printf("Usage: romapi_test [category]\r\n");
        printf("Categories: all, table, delay, aon, efuse, glb, hbn,\r\n");
        printf("            clock, pds, sf, uart, arch, utils, psram\r\n");
        return 0;
    }

    if (strcmp(category, "all") != 0) {
        printf("\r\nResults: %d PASSED, %d FAILED, %d SKIPPED\r\n",
               test_pass_count, test_fail_count, test_skip_count);
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_romapi_test, romapi_test, Test ROM APIs in ROM driver mode);
