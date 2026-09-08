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

/* hal_pio_cci.c - CCI master on a dedicated PIO state machine. See hal_pio_cci.h.
 *
 * The CCI bit-cell is identical to the cmsis-dap JTAG cell (out {DI,CS} with CLK
 * low; raise CLK + sample DO on the high phase; lower CLK). It runs on a separate
 * SM and separate pins, so it coexists with JTAG (SM0) with no pin/mux switching.
 */
#include "hal_pio_cci.h"
#include "drv_pio.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "hal_dma.h"
#include "ipro7.h"      /* HBN_BASE */
#include "hbn_reg.h"    /* HBN_PAD_CTRL_2_OFFSET, HBN_REG_EN_AON_CTRL_GPIO_MSB_* */

#define CCI_PIO_DELAY  3u   /* per-phase side-set delay; >= input 2-FF sync */

/* DMA-fed PIO so the CLK never pauses mid-frame. CPU-fed FIFO underruns paused the
 * clock at the data-phase 16-bit halfword boundary, corrupting bits 16/17 of the
 * word; the DMA paces the FIFO in hardware. DMA0 channels 0/1 are free in this app
 * (only ch2 = UART1 RX is used; JTAG/SM0 is CPU-fed). */
#define CCI_DMA_TX_CH      0u
#define CCI_DMA_RX_CH      1u
/* PIO per-SM FIFO data registers: PIO_BASE 0x3001D000, TXF0 @0x10 / RXF0 @0x20,
 * stride 4 per SM (regs/pio_reg.h). IPRO7 DMA mux request IDs: pio_tx[sm]=20+sm,
 * pio_rx[sm]=16+sm (drv_pio.h). */
#define CCI_PIO_TXF(sm)    (0x3001D000u + 0x10u + (uint32_t)(sm) * 4u)
#define CCI_PIO_RXF(sm)    (0x3001D000u + 0x20u + (uint32_t)(sm) * 4u)
#define CCI_PIO_TX_REQ(sm) (20u + (uint32_t)(sm))
#define CCI_PIO_RX_REQ(sm) (16u + (uint32_t)(sm))

static hal_pio_cci_cfg_t s_cfg;

/* 3-instruction cell (same encoding as hal_pio_cmsis_dap_build_jtag_prog):
 *   out pins,2 side 0 [d] -> CLK low, drive DI(bit0)/CS(bit1)
 *   in  pins,1 side 1 [d] -> CLK high (rising edge), sample DO on the high phase
 *   nop        side 0 [d] -> CLK low (idle low between CPU-paced words) */
static uint8_t build_cci_prog(uint16_t prog[8], uint8_t *wrap_top)
{
    const uint16_t d = CCI_PIO_DELAY;
    prog[0] = 0x6000 | (0u << 12) | (d << 8) | 2u;   /* out pins,2 side 0 [d] */
    prog[1] = 0x4000 | (1u << 12) | (d << 8) | 1u;   /* in  pins,1 side 1 [d] */
    prog[2] = 0xA042 | (0u << 12) | (d << 8);        /* nop        side 0 [d] */
    *wrap_top = 2;
    return 3;
}

/* GPIO18 (xtal32k_in) / GPIO19 (xtal32k_out) come out of reset under AON HW control
 * (HBN_PAD_CTRL_2.reg_en_aon_ctrl_gpio_msb bits[11:10] = 2'h3, bit10=GPIO18/bit11=GPIO19).
 * Under AON control a pad can be READ but not DRIVEN. So the release is DIRECTION-SPECIFIC:
 *   - OUTPUT (CLK/CS/DI side): CLEAR the AON bit -> hand the pad to normal PIO drive.
 *   - INPUT  (DO side):        KEEP the AON bit SET -> the pad stays readable by the PIO.
 *     Clearing it for an input hands the pad to normal control, whose input buffer is NOT
 *     enabled here, so the PIO then reads a constant (pull-up) instead of the pad -- that
 *     is the "CCI DO reads all-0xFF on GPIO19" bug. (HW: GPIO18 as CCI DO input worked
 *     under the AON default; GPIO19 as CCI DO read all-1 the moment its AON bit was cleared.)
 * The probe never uses a 32k crystal, so either state is safe. */
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

/* Configure the two one-shot DMA channels that feed/drain this SM's FIFOs. Static
 * config (direction/req/width) is set here once; per-shift only the addresses+size
 * are reloaded. */
static void cci_dma_setup(uint8_t sm)
{
    dma_device_t txdev = {
        .id = DMA0_ID, .ch = CCI_DMA_TX_CH, .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE, .dst_req = CCI_PIO_TX_REQ(sm),
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE, .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_size = DMA_BURST_INCR1, .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_32BIT, .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .transfer_mode = DMA_LLI_ONCE_MODE, .lli_cfg = NULL, .callback = NULL,
    };
    dma_device_t rxdev = {
        .id = DMA0_ID, .ch = CCI_DMA_RX_CH, .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = CCI_PIO_RX_REQ(sm), .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE, .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR1, .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_32BIT, .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .transfer_mode = DMA_LLI_ONCE_MODE, .lli_cfg = NULL, .callback = NULL,
    };
    GLB_Set_DMA_CLK(1, (GLB_DMA_CLK_ID_Type)(GLB_DMA_CLK_DMA0_CH0 + CCI_DMA_TX_CH));
    GLB_Set_DMA_CLK(1, (GLB_DMA_CLK_ID_Type)(GLB_DMA_CLK_DMA0_CH0 + CCI_DMA_RX_CH));
    /* attach, NOT dma_init(): without CONFIG_DMA_CHANNEL_ALLOCATOR the legacy
     * dma_init() does a controller-wide DMA_Disable(), aborting every OTHER
     * channel mid-transfer. This runs on every JTAG<->CCI mode switch on a co-pin
     * profile, so with dma_init() each switch silently killed the UART bridge's
     * cyclic RX DMA (CH2) and the DUT console went dead. dma_channel_attach()
     * touches only this channel; the controller comes up once, idempotently. */
    dma_setConfig(&txdev);
    dma_channel_attach(DMA0_ID, CCI_DMA_TX_CH);
    dma_setConfig(&rxdev);
    dma_channel_attach(DMA0_ID, CCI_DMA_RX_CH);
    pio_sm_dma_enable(sm, true, true);   /* gate the SM's tx/rx DMA req-ack lines */
}

int hal_pio_cci_init(const hal_pio_cci_cfg_t *cfg)
{
    if (!cfg) return -1;
    s_cfg = *cfg;

    uint16_t prog[8]; uint8_t wrap_top;
    uint8_t n = build_cci_prog(prog, &wrap_top);
    pio_load_program(prog, n, 0);   /* shared with JTAG (identical cell) at origin 0 */

    /* out-group is 2 contiguous pads from out_base; in {DO} from pin_do; side-set CLK.
     * out_base = lower of {DI,CS}: normally DI (CS=DI+1), or CS when out_swap (DI=CS+1).
     * OUT_SHIFTDIR=right -> OSR LSB-first: bit0=out_base, bit1=out_base+1 (cci_xfer.c's
     * W() packs to match). IN_SHIFTDIR=left + autopush thr 1 -> DO in ISR bit0 (r & 1). */
    uint8_t out_base = cfg->out_swap ? cfg->pin_cs : cfg->pin_di;
    pio_sm_set_pins(cfg->pio_sm, out_base, 2, cfg->pin_do, 1, cfg->pin_clk, 1);
    pio_sm_set_shift(cfg->pio_sm, true, true, 2, false, true, 1);
    pio_sm_set_clkdiv(cfg->pio_sm, cfg->clkdiv_int, cfg->clkdiv_frac);
    pio_sm_set_wrap(cfg->pio_sm, wrap_top, 0, false);
    pio_sm_set_pindirs(cfg->pio_sm, out_base, 2, 0x3U);      /* out-group {DI,CS} outputs */
    pio_sm_set_pindirs(cfg->pio_sm, cfg->pin_clk, 1, 1);     /* CLK output */
    pio_sm_set_pindirs(cfg->pio_sm, cfg->pin_do, 1, 0);      /* DO input */

    /* pinmux: CLK/DI/CS -> PIO output; DO -> PIO input with pull-up (idle high,
     * the slave drives it low while CLK is high and releases it on CLK low). */
    pin_func(cfg->pin_clk, GPIO_FUN_PIO, GPIO_MODE_OUTPUT, GPIO_PULL_NONE);
    pin_func(cfg->pin_di,  GPIO_FUN_PIO, GPIO_MODE_OUTPUT, GPIO_PULL_NONE);
    pin_func(cfg->pin_cs,  GPIO_FUN_PIO, GPIO_MODE_OUTPUT, GPIO_PULL_NONE);
    pin_func(cfg->pin_do,  GPIO_FUN_PIO, GPIO_MODE_INPUT,  GPIO_PULL_UP);

    pio_sm_enable(cfg->pio_sm, true);
    cci_dma_setup(cfg->pio_sm);
    return 0;
}

/* Shift n cells via DMA so the SM is fed/drained at hardware speed and the CLK
 * never pauses mid-frame (a CPU-fed FIFO underrun pausing the clock at the data
 * phase corrupted bits 16/17 of the word). The RX channel must always run (the cell
 * `in` pushes one word per cell -- an undrained RX FIFO stalls the SM), so a write
 * passes a scratch buffer as rx. tx and rx MUST be non-cacheable (DMA-coherent);
 * the caller (cci_xfer) places them in the USB no-cache section. */
void hal_pio_cci_shift(const uint32_t *tx, uint32_t n, uint32_t *rx)
{
    uint8_t sm = s_cfg.pio_sm;
    if (n == 0u || rx == 0)
        return;
    dma_reload(DMA0_ID, CCI_DMA_RX_CH, CCI_PIO_RXF(sm), (uint32_t)(uintptr_t)rx, n * 4u, NULL);
    dma_reload(DMA0_ID, CCI_DMA_TX_CH, (uint32_t)(uintptr_t)tx, CCI_PIO_TXF(sm), n * 4u, NULL);
    dma_channel_start(DMA0_ID, CCI_DMA_RX_CH);   /* arm RX before TX feeds the clock */
    dma_channel_start(DMA0_ID, CCI_DMA_TX_CH);
    uint32_t guard = 4000000u;                   /* ~ bounded so a stall can't wedge the task */
    while (dma_channel_check_busy(DMA0_ID, CCI_DMA_RX_CH) && --guard) { }
    while (dma_channel_check_busy(DMA0_ID, CCI_DMA_TX_CH) && guard) { }
}
