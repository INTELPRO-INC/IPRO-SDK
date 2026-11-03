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
 * HUK provisioning image (CONFIG_PSA_HUK_PROVISION): one boot draws
 * 128 bits from the SEC_ENG TRNG, burns eFuse key slot 4 and its write
 * and read locks after the host echoes the printed plan digest. Only
 * public values reach the console: chip id, digest, eFuse words outside
 * slots 2 and 4, check names and KCV = AES-128(HUK, 0^128).
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

enum hukp_result {
    HUKP_DONE,          /* slot 4 and both locks burned and verified */
    HUKP_REFUSED,       /* nothing burned */
    HUKP_FAIL,          /* a burn ran and a check after it failed */
};

/* One provisioning run: [HUKP] lines, every secret buffer wiped. */
enum hukp_result huk_prov_run(void);

/* huk_prov_run, then wfi forever. */
void huk_prov_main(void) __attribute__((noreturn));

/* Registers huk_prov.c reads or writes through hukp_reg_* */
enum hukp_reg {
    HUKP_TRNG_CTRL_0,   /* SEC_ENG 0x200, ht_error bit 4 */
    HUKP_TRNG_TEST,     /* SEC_ENG 0x228 */
    HUKP_TRNG_CTRL_3,   /* SEC_ENG 0x234, cp_ratio / rct / apt */
    HUKP_TRNG_ROSC_C1,  /* SEC_ENG 0x250 */
    HUKP_SE_PROT_RD,    /* SEC_ENG 0xF00, se_dbg_dis bit 31 */
    HUKP_EF_SW_CFG_0,   /* EF_CTRL 0x818, ef_sw_se_dbg_dis bit 22 */
};

/*
 * Register layer: huk_prov_hw.c on the chip, fakes in the host test.
 * eFuse words are R0 word indices 0..31 (byte offset / 4).
 */
int hukp_ef_ready(void);
void hukp_ef_load(void);
uint32_t hukp_ef_get(uint32_t word);
void hukp_ef_set(uint32_t word, uint32_t v);
/* program R0 from the shadow, wait for idle; -1 after about 1 s busy */
int hukp_ef_program(void);
uint32_t hukp_reg_read(enum hukp_reg r);
void hukp_reg_write(enum hukp_reg r, uint32_t v);
/* en 1: enable and wait for the instantiate; -1 on a timeout */
int hukp_trng_enable(int en);
/* one 256-bit generation; dout is cleared after the read */
int hukp_trng_generate(uint32_t out[8]);
uint32_t hukp_tzc_sboot_done(void);
/* tzc_sboot_done = 0xF by read-modify-write */
void hukp_tzc_close(void);
uint32_t hukp_bclk_mhz(void);
/* one console byte, -1 when none is waiting */
int hukp_uart_getc(void);
void hukp_delay_ms(uint32_t ms);
/* eFuse chip id; -1 when its zero-bit parity does not match */
int hukp_chip_id(uint8_t id[8]);
const uint8_t *hukp_image(size_t *len);
void hukp_irq_off(void);

#ifdef SPM_HOST
/* the module's secret-holding state, for the wipe checks */
const void *huk_prov_state(size_t *len);
#endif
