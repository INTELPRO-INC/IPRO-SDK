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

/* hal_pio_cmsis_dap.c - see inc/hal_pio_cmsis_dap.h. */
#include "hal_pio_cmsis_dap.h"
#ifdef HAL_PIO_CMSIS_DAP_HOSTTEST
/* Host build: forward-declare the drv_pio API we call, without the target reg
 * map (drv_pio.h -> pio_reg.h -> ipro7.h pulls RISC-V-only headers). */
#include <stdbool.h>
void     pio_load_program(const uint16_t *ops, uint8_t n, uint8_t origin);
void     pio_sm_set_pins(uint8_t sm, uint8_t out_base, uint8_t out_cnt,
                         uint8_t in_base, uint8_t in_cnt,
                         uint8_t ss_base, uint8_t ss_cnt);
void     pio_sm_set_shift(uint8_t sm, bool out_shdir, bool autopull, uint8_t pull_thr,
                          bool in_shdir,  bool autopush, uint8_t push_thr);
void     pio_sm_set_clkdiv(uint8_t sm, uint16_t intg, uint8_t frac);
void     pio_sm_set_wrap(uint8_t sm, uint8_t top, uint8_t bottom, bool ss_en);
void     pio_sm_enable(uint8_t sm, bool en);
void     pio_sm_set_pindirs(uint8_t sm, uint8_t base, uint8_t count, uint32_t dirs);
void     pio_sm_put(uint8_t sm, uint32_t w);
uint32_t pio_sm_get(uint8_t sm);
uint8_t  pio_txlevel(uint8_t sm);
uint8_t  pio_rxlevel(uint8_t sm);
#define  PIO_FIFO_DEPTH  8
#else
#include "drv_pio.h"
#endif /* HAL_PIO_CMSIS_DAP_HOSTTEST */
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
#include <generated/autoconf.h>
#include "drv_glb.h"
#include "drv_gpio.h"
#include "hal_gpio.h"      /* gpio_set_mode / gpio_write / gpio_read (nRESET/nTRST) */
#include "glb_reg.h"
#include "ipro7.h"         /* HBN_BASE */
#include "hbn_reg.h"       /* HBN_PAD_CTRL_2_OFFSET, HBN_REG_EN_AON_CTRL_GPIO_MSB_* */
#endif

static hal_pio_cmsis_dap_cfg_t s_cfg;
static uint8_t s_pin_state;

/* PIO base clock and SM cycles consumed per TCK by the loaded program.
 * The JTAG cell is 3 instructions, each (1 + d) cycles where d is the side-set
 * delay in build_jtag_prog, so cycles/TCK = 3*(1+d). With d=3 that is 12.
 * set_clock() divides by this; keep it in sync if the program changes. */
#define DAP_PIO_BASE_HZ          96000000U
#define DAP_PIO_CYCLES_PER_TCK   12U

/* DAP_SWJ_Pins bit positions handled here (cold path). */
#define DAP_SWJ_PIN_NTRST        0x20U
#define DAP_SWJ_PIN_NRESET       0x80U

/* ---- JTAG PIO program (host-testable, pure) ------------------------------
 * out group = {TDI@bit0, TMS@bit1} (2 pins from pin_tdi), side-set = TCK.
 * Per TCK: drive TDI/TMS with TCK low; raise TCK (target samples on rising);
 * lower TCK and sample TDO. The sample edge is the first-light tuning knob
 * (same practice as hal_pio_scan's LA-proven pre-edge sample). */
uint8_t hal_pio_cmsis_dap_build_jtag_prog(uint16_t prog[8], uint8_t *wrap_top)
{
    /* opcodes: OUT=0x6000 IN=0x4000; field [12:8] = side-set+delay. With
     * SIDESET_COUNT=1 and SIDE_EN=0 (mandatory side-set), bit[12] = TCK level
     * and bits[11:8] = delay. 3-instruction wrap loop that idles TCK LOW (standard
     * JTAG; matches the FT2232H golden waveform):
     *   out pins,2 side 0 [d] -> TCK LOW, drive TDI(bit0)/TMS(bit1); setup = (1+d) cyc
     *   in  pins,1 side 1 [d] -> TCK HIGH (rising edge: target latches TDI/TMS),
     *                            sample TDO (stable since the previous falling edge)
     *   nop        side 0 [d] -> TCK LOW (falling edge: target updates TDO)
     * The autopull stall sits on instr 0 (out); the instruction before it in wrap
     * order is the nop (side 0), so TCK idles LOW between CPU-paced words -- not
     * HIGH as the old 2-instr loop did. nop = mov y,y = 0xA042 (RP2040-compatible).
     * d = per-phase delay; >= the input 2-FF sync so the sampled TDO is settled. */
    const uint16_t d = 3u;
    prog[0] = 0x6000 | (0u << 12) | (d << 8) | 2u;   /* out pins,2 side 0 [d] : TCK low, drive */
    prog[1] = 0x4000 | (1u << 12) | (d << 8) | 1u;   /* in  pins,1 side 1 [d] : TCK high, sample TDO */
    prog[2] = 0xA042 | (0u << 12) | (d << 8);        /* nop        side 0 [d] : TCK low (idle low) */
    *wrap_top = 2;
    return 3;
}

/* TDI-only variant for the tms_gpio (non-adjacent) profile: identical 3-instruction
 * cell but `out pins,1` drives TDI alone -- TMS is a plain GPIO the CPU holds for the
 * run, so it is not in any PIO group. Same cycles/TCK (12), so the clock math holds. */
uint8_t hal_pio_cmsis_dap_build_jtag_prog_tdi(uint16_t prog[8], uint8_t *wrap_top)
{
    const uint16_t d = 3u;
    prog[0] = 0x6000 | (0u << 12) | (d << 8) | 1u;   /* out pins,1 side 0 [d] : TCK low, drive TDI */
    prog[1] = 0x4000 | (1u << 12) | (d << 8) | 1u;   /* in  pins,1 side 1 [d] : TCK high, sample TDO */
    prog[2] = 0xA042 | (0u << 12) | (d << 8);        /* nop        side 0 [d] : TCK low (idle low) */
    *wrap_top = 2;
    return 3;
}

#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
/* ---- target-only: GLB clock + pinmux (mirror hal_pio_scan) ---------------- */
static inline uint32_t glb_rd(uint32_t off) { return *(volatile uint32_t *)(uintptr_t)(GLB_BASE + off); }
static inline void     glb_wr(uint32_t off, uint32_t v) { *(volatile uint32_t *)(uintptr_t)(GLB_BASE + off) = v; }

static void hal_dap_glb_clock(const hal_pio_cmsis_dap_cfg_t *cfg)
{
    uint32_t v;
    v = glb_rd(GLB_SWRST_CFG1_OFFSET); glb_wr(GLB_SWRST_CFG1_OFFSET, v | GLB_SWRST_PIO_MSK);
    v = glb_rd(GLB_SWRST_CFG1_OFFSET); glb_wr(GLB_SWRST_CFG1_OFFSET, v & ~GLB_SWRST_PIO_MSK);
    v = glb_rd(GLB_CGEN_CFG0_OFFSET);  glb_wr(GLB_CGEN_CFG0_OFFSET,  v | GLB_CGEN_PIO_MSK);
    v = glb_rd(GLB_CLK_CFG6_OFFSET);
    v &= ~(GLB_PIO_CLK_SEL_MSK | GLB_PIO_CLK_DIV_MSK);
    v |= GLB_PIO_CLK_EN_MSK;
    v |= ((uint32_t)cfg->pio_clk_sel << GLB_PIO_CLK_SEL_POS) & GLB_PIO_CLK_SEL_MSK;
    v |= ((uint32_t)cfg->pio_clk_div << GLB_PIO_CLK_DIV_POS) & GLB_PIO_CLK_DIV_MSK;
    glb_wr(GLB_CLK_CFG6_OFFSET, v);
}

/* GPIO18 (xtal32k_in) / GPIO19 (xtal32k_out) reset under AON HW control
 * (HBN_PAD_CTRL_2.reg_en_aon_ctrl_gpio_msb bits[11:10]=2'h3, bit10=GPIO18/bit11=GPIO19).
 * Under AON control a pad can be READ but not DRIVEN, so the release is DIRECTION-SPECIFIC:
 * an OUTPUT (UNO TMS18/TCK19, UNO2 TCK18) CLEARS its AON bit to be driven; an INPUT
 * (UNO2 TDO19) KEEPS the bit SET so the PIO can still read it -- clearing it for an input
 * hands the pad to normal control whose input buffer is not enabled here, so the PIO would
 * read a constant (pull-up). Mirrors hal_pio_cci.c. Probe never uses a 32k crystal. */
static void config_xtal32k_pad(uint8_t pin, int as_output)
{
    uint32_t bit;
    if (pin == 18u)      bit = 1u << HBN_REG_EN_AON_CTRL_GPIO_MSB_POS;        /* GPIO18 = bit10 */
    else if (pin == 19u) bit = 1u << (HBN_REG_EN_AON_CTRL_GPIO_MSB_POS + 1u); /* GPIO19 = bit11 */
    else return;
    volatile uint32_t *r = (volatile uint32_t *)(uintptr_t)(HBN_BASE + HBN_PAD_CTRL_2_OFFSET);
    if (as_output) *r &= ~bit;   /* output: leave AON control -> PIO can drive the pad */
    else           *r |=  bit;   /* input:  keep AON control  -> PIO can still read the pad */
}

static void pin_func(uint8_t pin, uint32_t fun, uint32_t mode, uint32_t pull)
{
    config_xtal32k_pad(pin, mode == GPIO_MODE_OUTPUT);   /* 18/19: output clears AON bit, input keeps it */
    GLB_GPIO_Cfg_Type c = { .gpioPin = pin, .gpioFun = fun, .gpioMode = mode,
        .pullType = pull, .drive = 1, .smtCtrl = 1 };
    GLB_GPIO_Init(&c);
}

/* Open-drain emulation for the active-low reset lines. IPRO7 GPIO has no native
 * open-drain mode, so: assert (low) = drive output 0; release (high) = Hi-Z input
 * + internal pull-up. An EXTERNAL pull-up on the line is still recommended (a
 * stronger/faster rise, and a defined high while the probe is unpowered).
 * NOTE: the mode ids here are the hal_gpio set ("PP" = Pull-up), GPIO_OUTPUT_MODE
 * / GPIO_INPUT_PP_MODE -- NOT the drv-level GPIO_MODE_* that pin_func/GLB take. */
static void od_reset_set(uint8_t pin, uint32_t high)
{
    if (pin == 0xFF) return;
    if (high) {
        gpio_set_mode(pin, GPIO_INPUT_PP_MODE);   /* release: Hi-Z + pull-up -> high */
    } else {
        gpio_set_mode(pin, GPIO_OUTPUT_MODE);     /* assert: actively drive low */
        gpio_write(pin, 0);
    }
}

static void hal_dap_pinmux(const hal_pio_cmsis_dap_cfg_t *cfg)
{
    pin_func(cfg->pin_tdi, GPIO_FUN_PIO, GPIO_MODE_OUTPUT, GPIO_PULL_NONE);
    if (cfg->tms_gpio) {
        /* TMS driven by the CPU as a plain GPIO (quasi-static per sequence), not a
         * PIO out-group pad. Idle low; JTAG_Sequence/SWJ_Sequence set it before use. */
        gpio_set_mode(cfg->pin_tms, GPIO_OUTPUT_MODE);
        gpio_write(cfg->pin_tms, 0);
    } else {
        pin_func(cfg->pin_tms, GPIO_FUN_PIO, GPIO_MODE_OUTPUT, GPIO_PULL_NONE);
    }
    pin_func(cfg->pin_tck, GPIO_FUN_PIO, GPIO_MODE_OUTPUT, GPIO_PULL_NONE);  /* side-set */
    /* TDO: PIO input with PULL-UP so it idles high (matches FT2232H) and is never
     * driven by us; the explicit input pindir is set in hal_dap_setup_sm. */
    pin_func(cfg->pin_tdo, GPIO_FUN_PIO, GPIO_MODE_INPUT, GPIO_PULL_UP);
    od_reset_set(cfg->pin_nreset, 1);   /* nSRST released (open-drain high) */
    od_reset_set(cfg->pin_ntrst,  1);   /* nTRST released (open-drain high) */
}
#endif /* !HAL_PIO_CMSIS_DAP_HOSTTEST */

/* Set the JTAG PIO pin directions: out-group {TMS,TDI} + TCK as outputs, TDO as
 * input. Factored out of setup so repin() can restore them after the CCI master
 * (REPIN time-mux) flipped a shared pad's direction (e.g. CCI_CLK out vs JTAG TDO
 * in). pio_sm_set_pindirs works on a stopped SM, so repin calls it before re-enable. */
static void hal_dap_set_pindirs(const hal_pio_cmsis_dap_cfg_t *cfg)
{
    if (cfg->tms_gpio) {
        pio_sm_set_pindirs(cfg->pio_sm, cfg->pin_tdi, 1, 0x1U);   /* TDI-only out-group */
    } else {
        uint8_t out_base = cfg->out_swap ? cfg->pin_tms : cfg->pin_tdi;
        pio_sm_set_pindirs(cfg->pio_sm, out_base, 2, 0x3U);       /* out-group {TMS,TDI} */
    }
    pio_sm_set_pindirs(cfg->pio_sm, cfg->pin_tck, 1, 1);  /* TCK output */
    pio_sm_set_pindirs(cfg->pio_sm, cfg->pin_tdo, 1, 0);  /* TDO input (never driven) */
}

static void hal_dap_setup_sm(const hal_pio_cmsis_dap_cfg_t *cfg)
{
    uint16_t prog[8]; uint8_t wrap_top;
    /* out-group base: normally TDI (bit0=TDI, bit1=TMS). With out_swap the order is
     * reversed -- base = TMS (bit0=TMS, bit1=TDI) -- so the lower of the {TMS,TDI}
     * pair is the base. The caller's encoder packs the matching bit order. The
     * tms_gpio cell instead drives TDI alone (`out pins,1`, 1 bit/word); TMS is a
     * CPU-held GPIO, so the out-group is 1 pin and no adjacency is needed. */
    uint8_t n, out_base, out_cnt, pull_thr;
    if (cfg->tms_gpio) {
        n = hal_pio_cmsis_dap_build_jtag_prog_tdi(prog, &wrap_top);
        out_base = cfg->pin_tdi; out_cnt = 1; pull_thr = 1;
    } else {
        n = hal_pio_cmsis_dap_build_jtag_prog(prog, &wrap_top);
        out_base = cfg->out_swap ? cfg->pin_tms : cfg->pin_tdi; out_cnt = 2; pull_thr = 2;
    }
    pio_load_program(prog, n, 0);
    pio_sm_set_pins(cfg->pio_sm, out_base, out_cnt, cfg->pin_tdo, 1, cfg->pin_tck, 1);
    /* OUT_SHIFTDIR = right: `out pins,2` consumes OSR LSB-first -> TDI=OSR[0],
     * TMS=OSR[1] (matches the tx-word packing). IN_SHIFTDIR MUST be LEFT (false):
     * `in pins,1` then shifts the single TDO bit into ISR bit 0, which is what
     * hal_pio_cmsis_dap_shift() reads back as `r & 1`. With IN_SHIFTDIR=right the
     * bit lands in ISR bit 31 instead, so `r & 1` is always 0 and every scan reads
     * "all zeroes" even though the target drives valid TDO on the wire (LA-confirmed
     * 2026-06-11: TDO had real IDCODE data, firmware read it as 0). The host stub
     * ignores in_shdir, so host-TDD cannot catch this -- it is a silicon contract. */
    pio_sm_set_shift(cfg->pio_sm, true, true, pull_thr, false, true, 1);
    pio_sm_set_clkdiv(cfg->pio_sm, cfg->clkdiv_int, cfg->clkdiv_frac);
    pio_sm_set_wrap(cfg->pio_sm, wrap_top, 0, false);
    /* Pin directions (out-group + TCK out, TDO in) -- factored so repin() can restore
     * them; out_base above already accounts for out_swap. */
    hal_dap_set_pindirs(cfg);
}

int hal_pio_cmsis_dap_init(const hal_pio_cmsis_dap_cfg_t *cfg)
{
    if (!cfg) return -1;
    s_cfg = *cfg;
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
    hal_dap_glb_clock(cfg);
#endif
    hal_dap_setup_sm(cfg);
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
    hal_dap_pinmux(cfg);
#endif
    return 0;
}

/* SWRST + re-enable the shared PIO block clock, wiping ALL SM state (programs, pindirs,
 * FIFOs). The REPIN time-mux calls this before re-initialising the active mode's SM so a
 * disabled SM cannot leave a stale pin-direction claim on a shared pad. Uses the cfg the
 * last init saved. */
void hal_pio_cmsis_dap_pio_reclock(void)
{
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
    hal_dap_glb_clock(&s_cfg);
#endif
}

int hal_pio_cmsis_dap_set_port(hal_dap_port_t port)
{
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
    pio_sm_enable(s_cfg.pio_sm, port != HAL_DAP_PORT_OFF);
#else
    (void)port;
#endif
    return 0;
}

uint32_t hal_pio_cmsis_dap_set_clock(uint32_t hz)
{
    /* TCK rate = PIO base / (CYCLES_PER_TCK * clkdiv). */
    uint32_t base = DAP_PIO_BASE_HZ / DAP_PIO_CYCLES_PER_TCK;
    uint32_t div  = hz ? (base / hz) : 96U;
    if (div < 1U) div = 1U;
    s_cfg.clkdiv_int = (uint16_t)div;
    s_cfg.clkdiv_frac = 0;
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
    pio_sm_set_clkdiv(s_cfg.pio_sm, s_cfg.clkdiv_int, 0);
#endif
    return base / div;
}

/* PRECONDITION: the SM must be running (call set_port(JTAG/SWD) first).
 *
 * drv pio_sm_put/get are RAW FIFO register accessors -- they do NOT block on
 * TXFULL/RXEMPTY. The loaded program emits exactly one RX word per TX word, but
 * the SM needs ~12 PIO cycles to autopull a cell, clock it, and autopush the TDO
 * bit. A naive put();get(); per word reads the RX FIFO BEFORE that word exists,
 * returning stale/garbage (LA-confirmed: the wire carries the correct IDCODE,
 * but the racy read yielded non-deterministic, speed-dependent garbage). Mirror
 * hal_pio_scan's proven level-checked producer/consumer: feed TX while the FIFO
 * has room, and consume RX only once FLEVEL shows a word is present. RX is always
 * drained (even when rx==NULL) so the autopush never stalls the SM. */
void hal_pio_cmsis_dap_shift(const uint32_t *tx, uint32_t n, uint32_t *rx)
{
    uint32_t ti = 0, ri = 0;
    while (ri < n) {
        while (ti < n && pio_txlevel(s_cfg.pio_sm) < PIO_FIFO_DEPTH)
            pio_sm_put(s_cfg.pio_sm, tx[ti++]);
        while (ri < n && pio_rxlevel(s_cfg.pio_sm) > 0u) {
            uint32_t r = pio_sm_get(s_cfg.pio_sm);
            if (rx) rx[ri] = r & 1U;
            ri++;
        }
    }
}

/* tms_gpio cell: hold the CPU-owned TMS pin. Quasi-static -- the target latches TMS
 * on each TCK rising edge, and TMS is set here well before the SM emits any edge, so
 * the level is stable for the whole constant-TMS run (JTAG) or the single TCK (SWJ). */
void hal_pio_cmsis_dap_set_tms(uint32_t level)
{
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
    if (s_cfg.tms_gpio && s_cfg.pin_tms != 0xFF)
        gpio_write(s_cfg.pin_tms, level ? 1u : 0u);
#else
    (void)level;
#endif
}

void hal_pio_cmsis_dap_set_pins(uint8_t value, uint8_t select)
{
    s_pin_state = (s_pin_state & ~select) | (value & select);
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
    /* open-drain: DAP pin level 1 -> release (Hi-Z + pull-up), 0 -> drive low */
    if (select & DAP_SWJ_PIN_NRESET) od_reset_set(s_cfg.pin_nreset, value & DAP_SWJ_PIN_NRESET);
    if (select & DAP_SWJ_PIN_NTRST)  od_reset_set(s_cfg.pin_ntrst,  value & DAP_SWJ_PIN_NTRST);
#endif
}

uint8_t hal_pio_cmsis_dap_get_pins(void)
{
#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
    if (s_cfg.pin_nreset != 0xFF) {
        if (gpio_read(s_cfg.pin_nreset)) s_pin_state |= DAP_SWJ_PIN_NRESET;
        else s_pin_state &= ~DAP_SWJ_PIN_NRESET;
    }
    if (s_cfg.pin_ntrst != 0xFF) {
        if (gpio_read(s_cfg.pin_ntrst)) s_pin_state |= DAP_SWJ_PIN_NTRST;
        else s_pin_state &= ~DAP_SWJ_PIN_NTRST;
    }
#endif
    return s_pin_state;
}

void hal_pio_cmsis_dap_deinit(void) { hal_pio_cmsis_dap_set_port(HAL_DAP_PORT_OFF); }

#ifndef HAL_PIO_CMSIS_DAP_HOSTTEST
void hal_pio_cmsis_dap_board_init(void)
{
    hal_pio_cmsis_dap_cfg_t cfg = {
        .pin_tck = CONFIG_CMSIS_DAP_PIN_TCK, .pin_tms = CONFIG_CMSIS_DAP_PIN_TMS,
        .pin_tdi = CONFIG_CMSIS_DAP_PIN_TDI, .pin_tdo = CONFIG_CMSIS_DAP_PIN_TDO,
        .pin_nreset = CONFIG_CMSIS_DAP_PIN_NRESET, .pin_ntrst = CONFIG_CMSIS_DAP_PIN_NTRST,
        .pio_sm = 0, .pio_clk_div = 0, .pio_clk_sel = 0,
        .clkdiv_int = 96, .clkdiv_frac = 0,   /* conservative; set_clock() retunes from DAP_SWJ_Clock */
    };
    hal_pio_cmsis_dap_init(&cfg);
}
#endif
