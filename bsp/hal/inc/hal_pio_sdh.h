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
 * hal_pio_sdh.h - PIO-based 4-bit SD host (A/B against the hardware SDH).
 *
 * Built on the IPRO7 PIO state machines (GPIO22-27 by default):
 *   FSM(cmd) generates CLK (side-set) and shifts the 48-bit command out /
 *   response in; FSM(dat) shifts the 4-bit DATA lines, synchronized to CLK.
 * Independent of the hardware SDH (GPIO28-33), which is left untouched so the
 * two backends can be compared block-for-block.
 */
#ifndef HAL_PIO_SDH_H
#define HAL_PIO_SDH_H

#include <stdint.h>

/* Default pin map: CLK=22, CMD=23, D0=24, D1=25, D2=26, D3=27.
 * Constraints: D0..D3 must be 4 contiguous ascending GPIOs, and
 * pin_clk must equal pin_d0 - 2 (relative "wait pin" clock sync). */
typedef struct {
    uint8_t pin_clk;        /* default 22; MUST equal pin_d0 - 2 */
    uint8_t pin_cmd;        /* default 23 */
    uint8_t pin_d0;         /* default 24; D1..D3 = d0+1..d0+3 */
    uint8_t sm_cmd;         /* PIO state machine for CLK/CMD, default 0 */
    uint8_t sm_dat;         /* PIO state machine for DATA, default 1 */
    uint8_t use_dma;        /* 1 = DMA data path, 0 = CPU-paced FIFO */
    uint8_t dma_tx_ch;      /* DMA0 channel pacing the TX FIFO (use_dma) */
    uint8_t dma_rx_ch;      /* DMA0 channel draining the RX FIFO (use_dma) */
    uint8_t pio_clk_div;    /* GLB PIO pre-divider */
    uint8_t pio_clk_sel;    /* GLB PIO clock source select */
} hal_pio_sdh_cfg_t;

typedef struct {
    uint32_t block_count;   /* total 512-byte sectors */
    uint32_t block_size;    /* always 512 */
    uint8_t  access_mode;   /* 0 = byte addressing (SDSC), 1 = sector (SDHC/SDXC) */
    uint8_t  bus_width;     /* 1 or 4 */
    uint8_t  cid[16];
    uint8_t  csd[16];
} hal_pio_sdh_card_t;

/* All functions return 0 on success, negative on error. */
int  hal_pio_sdh_init(const hal_pio_sdh_cfg_t *cfg, hal_pio_sdh_card_t *out);

/* --- Bring-up seams (P2); also used internally by init in later phases. --- */
/* GLB clock + pinmux + load the cmd/clk program on SM(cmd) and start it. Once
 * running the CLK free-runs on pin_clk (the wait-for-command loop toggles it). */
int  hal_pio_sdh_hw_setup(const hal_pio_sdh_cfg_t *cfg);
/* Send one 48-bit command. resp_bits = 48 (R1/R3/R6/R7) or 0 (no response).
 * On success returns 0 and, for a 48-bit response, fills raw[0]=first FIFO word
 * and raw[1]=second. Returns negative on response timeout. (R2/136-bit later.) */
int  hal_pio_sdh_send_cmd(uint8_t cmd, uint32_t arg, uint8_t resp_bits, uint32_t raw[2]);
/* Run n clock cycles with CMD released (>=74 required after card power-up). */
void hal_pio_sdh_clock_burst(uint32_t n);
/* Raw PIO register peek (bring-up debug only). off = byte offset from PIO base. */
uint32_t hal_pio_sdh_reg_read(uint32_t off);
int  hal_pio_sdh_read_blocks(uint8_t *buf, uint32_t lba, uint32_t count);
int  hal_pio_sdh_write_blocks(const uint8_t *buf, uint32_t lba, uint32_t count);
int  hal_pio_sdh_set_clock(uint32_t hz);
/* FatFS diskio port (components/fs/fatfs/port/fatfs_pio_sdh_sdcard.c, built
 * when HAL_PIO_SDH is enabled): registers this host on the DEV_SD physical
 * drive - the same slot as the hardware SDH port, since the A/B bench has
 * the card in exactly one socket at a time. */
void fatfs_pio_sdh_driver_register(const hal_pio_sdh_cfg_t *cfg);
/* DBG: runtime CLK pad drive-strength override (0..3); needs hw_setup first. */
int  hal_pio_sdh_set_clk_drive(uint8_t drive);
/* Pad config for ALL six pins: drive 0-3, internal pull-up on/off, schmitt.
 * Sticky - stored and re-applied on every later init/hw_setup; also applied
 * immediately if the hw is already set up. pull_up=0 is for benches whose SD
 * adapter provides external pull-up resistors (single defined pull). */
int  hal_pio_sdh_set_pad_cfg(uint8_t drive, uint8_t pull_up, uint8_t smt);
/* Switch DATA bus width (1 or 4) via ACMD6; needs an enumerated card. */
int  hal_pio_sdh_set_bus_width(uint8_t width);
/* CMD6 SWITCH_FUNC -> High-Speed access mode. Reads the 512-bit switch status
 * (64 bytes into *status, nullable). Returns 0 (HS granted), -1 (cmd/read fail),
 * -2 (card did not grant HS). Card must be enumerated + 4-bit. */
int  hal_pio_sdh_switch_hs(uint8_t *status);
/* DBG: force the read sample edge (0 = DS low-phase, 1 = HS rising-latch). */
void hal_pio_sdh_dbg_set_rising(uint8_t on);
/* DBG: extra CLK-low cycles between DATA nibbles (SI gap sweep, 0..30). */
int  hal_pio_sdh_set_dat_gap(uint8_t cycles);
/* DBG: same, for the write data phase (host-pad SSO sweep, 0..30). */
int  hal_pio_sdh_set_wr_gap(uint8_t cycles);
/* Select the SM-autonomous write path (W3: feed+token+busy on the SM). The feed
 * is packed 8 nibbles/word (out pins,4 + autopull); with the default wgap 0 the
 * write is wire-bound. *last_status (nullable) = last CRC status token (0x2 =
 * accepted). */
int  hal_pio_sdh_set_wauto(uint8_t on, uint32_t *last_status);
/* DBG: 1 if the last multi-block read used a CMD23-bounded transfer, 0 if it
 * fell back to open-ended CMD18 + CMD12. */
uint8_t hal_pio_sdh_dbg_last_read_cmd23(void);
/* DBG: enable(1)/disable(0) the aborted-transfer stop recovery (stress the raw
 * datapath - a failing transfer then wedges instead of recovering). */
void hal_pio_sdh_dbg_set_recover(uint8_t on);
/* DBG (R3 probe): 1 = 4-bit DMA reads use the self-clocked read loop (CLK does
 * not stall on CPU finalize, so the ping-pong overlaps); 0 = TX-paced default. */
void hal_pio_sdh_dbg_set_rauto(uint8_t on);
/* Read-loop SM cycles per nibble: 4 = 24 MHz CLK (default/solid), 3 = 32 MHz
 * (+33%), 2 = 48 MHz (~2x wire, SI-marginal on jumper wiring). Any other value
 * -> 4. Mainly helps raw reads (FS path is finalize/scan-bound). Shell:
 * `pio_sdcard rcyc <2|3|4>`. */
void hal_pio_sdh_dbg_set_read2c(uint8_t ncyc);
/* Hardware `jmp pin` start-bit hunt for the rising 4-cyc self-clocked read
 * (1 = default: SM self-clocks the idle recovery nibbles between blocks instead
 * of the CPU polling one per MMIO round-trip, +17-24% read; 0 = CPU-scan
 * fallback). Recovers the ~12 us/block CPU scan cost. Shell:
 * `pio_sdcard rhunt <0|1>`. */
void hal_pio_sdh_dbg_set_rhunt(uint8_t on);
/* CMD13 SEND_STATUS. *status = 32-bit R1 card status, *state = CURRENT_STATE
 * [12:9] (4=tran,5=data,6=rcv,7=prg). Returns 0 or -1 (no card / no response).
 * Used by the transfer-stop recovery to tell a stranded card from an
 * already-stopped one, and by the shell status/wdiag probes. */
int hal_pio_sdh_card_status(uint32_t *status, uint32_t *state);

/* Single-block (CMD24) write settle stats since the last call (then cleared).
 * still_prg > 0 means the DAT0 busy sniff reported "done" while the card was
 * in fact still programming - the collision that breaks metadata-heavy
 * (small-file) workloads. timeouts = card never came back ready (real fault).
 * settle_us = total time the settle cost, to weigh it against its benefit. */
void hal_pio_sdh_wr1_diag_get(uint32_t *total, uint32_t *still_prg,
                              uint32_t *timeouts, uint32_t *settle_us);

/* Runtime A/B switch for that settle (default on): lets a test compare with
 * and without it on the SAME card in the SAME state. */
void    hal_pio_sdh_set_wr1_settle(uint8_t on);
uint8_t hal_pio_sdh_get_wr1_settle(void);
/* DBG: DMA read-path per-phase averages in ns (scan/arm/wait/finalize) since
 * the previous call, plus the sampled block count. NULL any unwanted output. */
void hal_pio_sdh_prof_get(uint32_t *scan_ns, uint32_t *arm_ns, uint32_t *wait_ns,
                          uint32_t *fin_ns, uint32_t *blocks);
/* DBG: DMA write-path per-phase averages in ns (expand/feed/tokenbusy), plus
 * the tokenbusy cell-switch sub-cost and the mean card busy-release clock count
 * (near 0 = card not the bottleneck). NULL any unwanted output. */
void hal_pio_sdh_wprof_get(uint32_t *exp_ns, uint32_t *feed_ns, uint32_t *tb_ns,
                           uint32_t *cell_ns, uint32_t *busyclks, uint32_t *blocks);
void hal_pio_sdh_deinit(void);

#endif /* HAL_PIO_SDH_H */
