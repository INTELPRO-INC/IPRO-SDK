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
 * drv_pio - low-level IPRO7 PIO peripheral driver (RP2040-compatible).
 * See inc/drv_pio.h. Validated by tests/drv_pio (host register model).
 */
#include "drv_pio.h"

/* ---- MMIO accessors ------------------------------------------------------
 * Self-contained (no SDK readl/writel dependency), mirroring ivf_replay. On
 * target these are volatile loads/stores at PIO_BASE+off; under -DDRV_PIO_HOSTTEST
 * they call a software register model so the driver logic runs on the host.
 */
#ifdef DRV_PIO_HOSTTEST
uint32_t drv_pio_test_mmio_read(uint32_t addr);
void     drv_pio_test_mmio_write(uint32_t addr, uint32_t val);
static inline uint32_t pio_rd(uint32_t off) { return drv_pio_test_mmio_read(PIO_BASE + off); }
static inline void     pio_wr(uint32_t off, uint32_t val) { drv_pio_test_mmio_write(PIO_BASE + off, val); }
#else
static inline uint32_t pio_rd(uint32_t off) { return *(volatile uint32_t *)(uintptr_t)(PIO_BASE + off); }
static inline void     pio_wr(uint32_t off, uint32_t val) { *(volatile uint32_t *)(uintptr_t)(PIO_BASE + off) = val; }
#endif

/*
 * SM0 and SM1 have identical bitfield layouts; only the register-block base
 * differs by a fixed stride. So one set of PIO_CR_SM0_* / PIO_ST_SM0_* field
 * constants serves both SMs, and the SM index only picks the block offset.
 * This removes all SM0-vs-SM1 special-casing.
 */
#define SM_STRIDE             0x18u
#define SM_REG(sm, sm0_off)   ((sm0_off) + (uint32_t)(sm) * SM_STRIDE)

/* pack value into a named field (POS/MSK from pio_reg.h); MSK truncates, so a
 * threshold of 32 into a 5-bit field naturally encodes as 0 (hw convention). */
#define FIELD(name, val)      (((uint32_t)(val) << (name##_POS)) & (name##_MSK))
#define GETF(word, name)      (((word) & (name##_MSK)) >> (name##_POS))

void pio_load_program(const uint16_t *ops, uint8_t n, uint8_t origin)
{
    for (uint8_t i = 0; i < n; i++)
        pio_wr(PIO_INSTR_MEM0_OFFSET + ((uint32_t)origin + i) * 4u, ops[i]);
}

void pio_sm_set_pins(uint8_t sm, uint8_t out_base, uint8_t out_cnt, uint8_t in_base,
                     uint8_t in_cnt, uint8_t ss_base, uint8_t ss_cnt)
{
    (void)in_cnt;   /* PIO has no IN_COUNT field: the `in` instruction carries the
                       bit count. in_cnt documents intent at the HAL call site. */
    uint32_t w = FIELD(PIO_CR_SM0_OUT_BASE, out_base)
               | FIELD(PIO_CR_SM0_OUT_COUNT, out_cnt)
               | FIELD(PIO_CR_SM0_IN_BASE, in_base)
               | FIELD(PIO_CR_SM0_SIDESET_BASE, ss_base)
               | FIELD(PIO_CR_SM0_SIDESET_COUNT, ss_cnt);
    pio_wr(SM_REG(sm, PIO_SM0_PINCTRL_OFFSET), w);
}

void pio_sm_set_shift(uint8_t sm, bool out_shdir, bool autopull, uint8_t pull_thr,
                      bool in_shdir, bool autopush, uint8_t push_thr)
{
    uint32_t w = FIELD(PIO_CR_SM0_OUT_SHIFTDIR, out_shdir)
               | FIELD(PIO_CR_SM0_AUTOPULL, autopull)
               | FIELD(PIO_CR_SM0_PULL_THRESH, pull_thr)
               | FIELD(PIO_CR_SM0_IN_SHIFTDIR, in_shdir)
               | FIELD(PIO_CR_SM0_AUTOPUSH, autopush)
               | FIELD(PIO_CR_SM0_PUSH_THRESH, push_thr);
    pio_wr(SM_REG(sm, PIO_SM0_SHIFTCTRL_OFFSET), w);
}

/* Set the FIFO DREQ thresholds (this fork's SHIFTCTRL addition): a nonzero
 * TXF_THR/RXF_THR makes the per-SM DMA request assert only once the FIFO has
 * that many free/filled entries, so a burst-sized DMA transfer can be granted
 * atomically. RMW because pio_sm_set_shift writes the whole word (zeroing these
 * fields), so call this AFTER it. */
void pio_sm_set_fifo_thr(uint8_t sm, uint8_t txf_thr, uint8_t rxf_thr)
{
    uint32_t w = pio_rd(SM_REG(sm, PIO_SM0_SHIFTCTRL_OFFSET));
    w &= (PIO_CR_SM0_TXF_THR_UMSK & PIO_CR_SM0_RXF_THR_UMSK);
    w |= FIELD(PIO_CR_SM0_TXF_THR, txf_thr) | FIELD(PIO_CR_SM0_RXF_THR, rxf_thr);
    pio_wr(SM_REG(sm, PIO_SM0_SHIFTCTRL_OFFSET), w);
}

void pio_sm_set_clkdiv(uint8_t sm, uint16_t intg, uint8_t frac)
{
    uint32_t w = FIELD(PIO_CR_SM0_INT, intg) | FIELD(PIO_CR_SM0_FRAC, frac);
    pio_wr(SM_REG(sm, PIO_SM0_CLKDIV_OFFSET), w);
}

void pio_sm_set_wrap(uint8_t sm, uint8_t top, uint8_t bottom, bool ss_en)
{
    uint32_t w = FIELD(PIO_CR_SM0_WRAP_TOP, top)
               | FIELD(PIO_CR_SM0_WRAP_BOTTOM, bottom)
               | FIELD(PIO_CR_SM0_SIDE_EN, ss_en);
    /* SIDE_PINDIR left 0: side-set drives pin values, not directions. */
    pio_wr(SM_REG(sm, PIO_SM0_EXECCTRL_OFFSET), w);
}

void pio_sm_enable(uint8_t sm, bool en)
{
    uint32_t bit = 1u << (PIO_CR_SM_EN_POS + sm);
    uint32_t v = pio_rd(PIO_CTRL_PIO_OFFSET);
    v = en ? (v | bit) : (v & ~bit);
    pio_wr(PIO_CTRL_PIO_OFFSET, v);
}

void pio_sm_restart(uint8_t sm)
{
    /* SM_RESET is a write-1 pulse (hw self-clears); RMW-OR preserves SM_EN. */
    uint32_t v = pio_rd(PIO_CTRL_PIO_OFFSET);
    pio_wr(PIO_CTRL_PIO_OFFSET, v | (1u << (PIO_CR_SM_RESET_POS + sm)));
}

void pio_sm_exec(uint8_t sm, uint16_t op)
{
    pio_wr(SM_REG(sm, PIO_SM0_INSTR_OFFSET), FIELD(PIO_CR_SM0_INSTR, op));
}

void pio_sm_set_pindirs(uint8_t sm, uint8_t base, uint8_t count, uint32_t dirs)
{
    /* point the SET group at [base, count], exec `set pindirs, dirs`, restore. */
    uint32_t off = SM_REG(sm, PIO_SM0_PINCTRL_OFFSET);
    uint32_t saved = pio_rd(off);
    uint32_t tmp = (saved & ~(PIO_CR_SM0_SET_BASE_MSK | PIO_CR_SM0_SET_COUNT_MSK))
                 | FIELD(PIO_CR_SM0_SET_BASE, base)
                 | FIELD(PIO_CR_SM0_SET_COUNT, count);
    pio_wr(off, tmp);
    /* SET pindirs, dirs:  opcode 0xE000 | (dest=pindirs=0b100)<<5 | (dirs & 0x1F) */
    pio_sm_exec(sm, (uint16_t)(0xE000u | (0x4u << 5) | (dirs & 0x1Fu)));
    pio_wr(off, saved);
}

void pio_sm_set_set_base(uint8_t sm, uint8_t base, uint8_t count)
{
    uint32_t off = SM_REG(sm, PIO_SM0_PINCTRL_OFFSET);
    uint32_t w = pio_rd(off) & ~(PIO_CR_SM0_SET_BASE_MSK | PIO_CR_SM0_SET_COUNT_MSK);
    w |= FIELD(PIO_CR_SM0_SET_BASE, base) | FIELD(PIO_CR_SM0_SET_COUNT, count);
    pio_wr(off, w);
}

void pio_sm_set_jmp_pin(uint8_t sm, uint8_t pin)
{
    uint32_t off = SM_REG(sm, PIO_SM0_EXECCTRL_OFFSET);
    uint32_t w = pio_rd(off) & ~PIO_CR_SM0_JMP_PIN_MSK;
    w |= FIELD(PIO_CR_SM0_JMP_PIN, pin);
    pio_wr(off, w);
}

void pio_sm_set_mov_status(uint8_t sm, bool rx_not_tx, uint8_t n)
{
    uint32_t off = SM_REG(sm, PIO_SM0_EXECCTRL_OFFSET);
    uint32_t w = pio_rd(off) & ~(PIO_CR_SM0_STATUS_SEL_MSK | PIO_CR_SM0_STATUS_N_MSK);
    w |= FIELD(PIO_CR_SM0_STATUS_SEL, rx_not_tx ? 1u : 0u)
       | FIELD(PIO_CR_SM0_STATUS_N, n);
    pio_wr(off, w);
}

void pio_sm_dma_enable(uint8_t sm, bool tx_en, bool rx_en)
{
    /* per-SM bits packed [SM0_TX, SM0_RX, SM1_TX, SM1_RX] = 2 bits/SM. RMW keeps
     * the other SM's gate. */
    uint32_t tx_bit = 1u << (PIO_CR_SM0_DMA_TX_EN_POS + (uint32_t)sm * 2u);
    uint32_t rx_bit = 1u << (PIO_CR_SM0_DMA_RX_EN_POS + (uint32_t)sm * 2u);
    uint32_t v = pio_rd(PIO_DMA_CONFIG_OFFSET) & ~(tx_bit | rx_bit);
    if (tx_en) v |= tx_bit;
    if (rx_en) v |= rx_bit;
    pio_wr(PIO_DMA_CONFIG_OFFSET, v);
}

void pio_sm_put(uint8_t sm, uint32_t w)
{
    pio_wr(PIO_TXF0_OFFSET + (uint32_t)sm * 4u, w);
}

uint32_t pio_sm_get(uint8_t sm)
{
    return pio_rd(PIO_RXF0_OFFSET + (uint32_t)sm * 4u);
}

uint8_t pio_txlevel(uint8_t sm)
{
    /* FLEVEL packs [SM0_TX, SM0_RX, SM1_TX, SM1_RX] in 4-bit fields, 8 bits/SM. */
    uint32_t fl = pio_rd(PIO_FLEVEL_OFFSET);
    return (uint8_t)((fl >> (PIO_ST_SM0_TXLEVEL_POS + (uint32_t)sm * 8u)) & 0xFu);
}

uint8_t pio_rxlevel(uint8_t sm)
{
    uint32_t fl = pio_rd(PIO_FLEVEL_OFFSET);
    return (uint8_t)((fl >> (PIO_ST_SM0_RXLEVEL_POS + (uint32_t)sm * 8u)) & 0xFu);
}

void pio_input_sync_bypass(uint32_t mask)
{
    pio_wr(PIO_INPUT_SYNC_BYPASS_OFFSET, mask);
}

void pio_cfginfo(uint8_t *sm_count, uint8_t *fifo_depth, uint8_t *imem_size)
{
    uint32_t ci = pio_rd(PIO_DBG_CFGINFO_OFFSET);
    if (sm_count)   *sm_count   = (uint8_t)GETF(ci, PIO_ST_SM_COUNT);
    if (fifo_depth) *fifo_depth = (uint8_t)GETF(ci, PIO_ST_FIFO_DEPTH);
    if (imem_size)  *imem_size  = (uint8_t)GETF(ci, PIO_ST_IMEM_SIZE);
}
