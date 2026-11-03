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

/*
 * Register layer of huk_prov.c on IPRO7: eFuse shadow and program
 * through drv_ef_ctrl, the SEC_ENG HAL v2 TRNG, TZC, BCLK, console RX.
 */
#include <stddef.h>
#include <stdint.h>

#include "compiler/common.h"
#include "drv_clock.h"
#include "drv_ef_ctrl.h"
#include "drv_tzc.h"
#include "hal_sec_trng_v2.h"
#include "hal_uart.h"
#include "huk_prov.h"
#include "psa_ipro/memmap.h"
#include "system_soc.h"

/* LMA end of the last loaded section, ipro7_psa_spe.ld */
extern const uint8_t __spe_image_end[];
int board_get_debug_uart_index(void);

static const uint8_t hukp_trng_reg[] = {
    [HUKP_TRNG_CTRL_0] = HAL_SEC_TRNG_V2_CTRL_0,
    [HUKP_TRNG_TEST] = HAL_SEC_TRNG_V2_TEST,
    [HUKP_TRNG_CTRL_3] = HAL_SEC_TRNG_V2_CTRL_3,
    [HUKP_TRNG_ROSC_C1] = HAL_SEC_TRNG_V2_ROSC_C1,
    [HUKP_SE_PROT_RD] = HAL_SEC_TRNG_V2_PROT_RD,
};
#define HUKP_EF_SW_CFG_0_ADDR (EF_CTRL_BASE + EF_CTRL_EF_SW_CFG_0_OFFSET)

int hukp_ef_ready(void)
{
    return EF_Ctrl_AutoLoad_Done() == SET && EF_Ctrl_Busy() == RESET;
}

void hukp_ef_load(void)
{
    EF_Ctrl_Load_Efuse_R0();
}

uint32_t hukp_ef_get(uint32_t word)
{
    return getreg32(EF_DATA_BASE + 4u * word);
}

void hukp_ef_set(uint32_t word, uint32_t v)
{
    EF_Ctrl_Sw_AHB_Clk_0();
    putreg32(v, EF_DATA_BASE + 4u * word);
}

/* busy polls of hukp_ef_program, 100 us apart */
#define HUKP_PROGRAM_POLLS  10000u

int hukp_ef_program(void)
{
    EF_Ctrl_Program_Efuse_0();
    for (uint32_t i = 0; i < HUKP_PROGRAM_POLLS; i++) {
        arch_delay_us(100);
        if (EF_Ctrl_Busy() == RESET) {
            return 0;
        }
    }
    return -1;
}

uint32_t hukp_reg_read(enum hukp_reg r)
{
    if (r == HUKP_EF_SW_CFG_0) {
        return getreg32(HUKP_EF_SW_CFG_0_ADDR);
    }
    return hal_sec_trng_v2_reg_read(hukp_trng_reg[r]);
}

void hukp_reg_write(enum hukp_reg r, uint32_t v)
{
    if (r == HUKP_EF_SW_CFG_0) {
        putreg32(v, HUKP_EF_SW_CFG_0_ADDR);
        return;
    }
    hal_sec_trng_v2_reg_write(hukp_trng_reg[r], v);
}

int hukp_trng_enable(int en)
{
    return hal_sec_trng_v2_enable(en) == 0 ? 0 : -1;
}

int hukp_trng_generate(uint32_t out[8])
{
    return hal_sec_trng_v2_generate((uint8_t *)out) == 0 ? 0 : -1;
}

uint32_t hukp_tzc_sboot_done(void)
{
    return (getreg32(TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET) &
            TZC_SEC_TZC_SBOOT_DONE_MSK) >> TZC_SEC_TZC_SBOOT_DONE_POS;
}

void hukp_tzc_close(void)
{
    uint32_t a = TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET;

    putreg32((getreg32(a) & ~TZC_SEC_TZC_SBOOT_DONE_MSK) |
             TZC_SEC_TZC_SBOOT_DONE_MSK, a);
}

uint32_t hukp_bclk_mhz(void)
{
    return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK) / 1000000u;
}

int hukp_uart_getc(void)
{
    uint8_t c;

    return uart_read_available(board_get_debug_uart_index(), &c, 1) == 1
               ? c : -1;
}

void hukp_delay_ms(uint32_t ms)
{
    arch_delay_ms(ms);
}

int hukp_chip_id(uint8_t id[8])
{
    return EF_Ctrl_Read_Chip_ID(id) == SUCCESS ? 0 : -1;
}

const uint8_t *hukp_image(size_t *len)
{
    *len = (size_t)((uintptr_t)__spe_image_end - PSA_SPE_FLASH_BASE);
    return (const uint8_t *)(uintptr_t)PSA_SPE_FLASH_BASE;
}

void hukp_irq_off(void)
{
    __disable_irq();
}
