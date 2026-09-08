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
 * hal_pio_sdh.c - PIO-based 4-bit SD host.
 *
 * CMD transport. IPRO7's PIO diverges from RP2040 in ways that break the classic
 * rp2040_sdio cmd_clk program (all probed on silicon 2026-07-02):
 *   - OUT datapath: OSR shifts LEFT (hardwired) but the delivered bits tap the
 *     LSB end, so multi-out-per-word streams emit garbage. We avoid OUT entirely
 *     and drive CMD with `mov pins, osr` after a fresh `pull` (one word per bit).
 *   - INPUT_SYNC_BYPASS is broken: bypass=1 disconnects the input (reads 0).
 *     Keep the 2-FF synchronizer active (adds 2 SM cycles of input latency).
 *   - `jmp x--` loop counts match RP2040; STATUS (TX<n) and side-set are fine.
 *
 * The bit cell is 4 SM cycles (CLK low 2, high 2 -> SD_CLK = 24MHz/clkdiv), one
 * FIFO word per bit, framing on the CPU. TX and RX use SEPARATE cells swapped
 * per phase (pio_sdh_load_cell), because on this silicon running `mov pins` on
 * the shared bidirectional CMD pin during reception perturbs the sampled value:
 *   TX cell  (k_cmdbit_prog): pull / mov pins,osr / nop side1 / in - drives CMD
 *   RX cell  (k_rxbit_prog):  pull / nop / nop side1 / in         - never drives
 * Sampling phase: CLK rises at cyc2; `in` runs at cyc3 and, through the 2-FF
 * sync, latches the pad as of cyc1 (settled low phase) - the response bit the
 * card drove on the previous falling edge, aligned with the LA's rising-edge
 * sample. TX feeds 48 words MSB-first (discarding echoes); RX releases CMD
 * (pindir=in), clocks dummy words, and scans for the start bit. CLK is gated
 * (pull stalls, CLK low) whenever the FIFO is empty - SD allows a stopped clock.
 * CRC7/CRC16/command framing live in hal_pio_sdh_crc.c (host-unit-tested).
 *
 * HW-verified 2026-07-02: full enumeration (CMD0/8/ACMD41/CMD2/3/9/7/ACMD6) of a
 * 64GB SDXC card in 4-bit mode; all R1 CRC7 checks pass.
 */
#include "hal_pio_sdh.h"
#include "hal_pio_sdh_crc.h"

#include <stdio.h>
#include <generated/autoconf.h>
#include "drv_pio.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_common.h"     /* ATTR_NOCACHE_RAM_SECTION */
#include "glb_reg.h"
#include "ipro7.h"
#include "hal_dma.h"
#include "drv_dma.h"        /* DMA_Channel_Update_{Src,Dst}Memcfg (R6 fast re-arm) */

#define ARRAY_LEN(a)   ((uint8_t)(sizeof(a) / sizeof((a)[0])))

/* PIO SM clock = 96 MHz (GLB pre-divider left at 0). The cmd/clk program spends
 * 4 SM cycles per SD clock bit, so SD_CLK = 24 MHz / clkdiv_int. */
#define SDH_PIO_BIT_HZ        24000000U   /* 96 MHz / 4 cycles-per-bit */
#define SDH_INIT_HZ           400000U
/* Default transfer clock after enumeration. Full rate (clkdiv=1) is REQUIRED
 * for reliable 4-bit reads on this bench, not just for speed: at any slower
 * divider the long CLK phases leave a window where a dense-data all-4-DAT
 * simultaneous rise couples an extra clock edge into the card (deterministic
 * dropped nibble, e.g. FAT-VBR block 3). At 24 MHz the CLK high pulse (~21 ns,
 * same geometry as the HW SDH's 25 MHz) closes that window - verified by a
 * 100k/400k/3M/12M/24M sweep (all fail but 24M) and a 24M stress pass. */
#define SDH_XFER_HZ           24000000U
/* Response-wait spin budget (~100 ms at 400 kHz worst case incl. card NCR). */
#define SDH_CMD_TIMEOUT_SPINS 2000000U

/* Program layout in the 32-word shared instruction memory:
 *   cmd bit-cell resident at origin 0 (4 words);
 *   data programs load at origin 18 in later phases. */
#define CMD_CLK_ORIGIN        0
#define CMD_CLK_WRAP_TOP      3
#define DATA_ORIGIN           18

/* ---- PIO program: JTAG-style CMD bit cell (see file header) -------------- */

/* Every op here is silicon-probe-verified: pull block, mov (LSB tap), in
 * pins (left shift, autopush), side-set. Autopull is avoided entirely - the
 * explicit `pull block` both paces the bit stream and gates CLK low while the
 * FIFO is empty. 4 SM cycles per bit (1 + 1 + 1 + 1).
 *
 * Sampling phase: the input passes a 2-FF synchronizer (INPUT_SYNC_BYPASS is
 * broken on this silicon, so it cannot be turned off), i.e. `in` sees the pad
 * value from 2 SM cycles earlier. CLK rises at cyc2 (nop side 1); `in` runs at
 * cyc3 and therefore latches the pad as it was at cyc1 - the settled low phase,
 * which is exactly the response bit the card drove on the previous falling edge
 * (the same instant the LA samples on the rising edge). TX is unchanged: `mov`
 * drives CMD at cyc1 (CLK low), one cycle of setup before the cyc2 rising edge.
 * (An earlier 3-word cell sampled at cyc2 -> read the pad at cyc0, right on the
 * card's falling-edge transition, so every response bit came back as noise and
 * cmd_rx_bits never found the start bit even though the card was replying.) */
static const uint16_t k_cmdbit_prog[] = {
    0x80A0,  /* 0: pull block   side 0   CLK low, wait for/load a bit word     */
    0xA007,  /* 1: mov pins,osr side 0   CLK low, drive CMD = word bit0        */
    0xB042,  /* 2: nop          side 1   CLK high (rising edge)                */
    0x5001,  /* 3: in pins,1    side 1   CLK high, sample CMD (pad @ cyc1)     */
};

/* RX phase cell: identical CLK/timing to k_cmdbit_prog but with NO `mov pins`,
 * so CMD is never driven while the card is replying. On this silicon, running
 * `mov pins` on the shared bidirectional CMD pin during reception perturbs the
 * sampled value (the response came back as noise); a sample-only cell fixes it.
 * The two cells are swapped in per phase - both are 4 words at CMD_CLK_ORIGIN. */
static const uint16_t k_rxbit_prog[] = {
    0x80A0,  /* 0: pull block   side 0   CLK low, pace on dummy word           */
    0xA042,  /* 1: nop          side 0   CLK low                               */
    0xB042,  /* 2: nop          side 1   CLK high (rising edge)                */
    0x5001,  /* 3: in pins,1    side 1   CLK high, sample CMD (pad @ cyc1)     */
};

/* DATA phase cell: samples 4 bits (the DAT3..DAT0 nibble) from IN_BASE=pin_d0.
 * Never drives; one nibble per word (autopush threshold 4, DMA repacks wider).
 *
 * Cell geometry (SI-tuned for the back-to-back DMA-fed stream): the card flips
 * its DAT outputs ~5-14 ns after each rising edge, and a many-line
 * simultaneous flip (FAT-table content = a 0x0->0x7 three-line rise every 4
 * bytes) rings the bus hard enough to double-clock the card IF the NEXT
 * rising edge arrives while the ring is still live. The CPU-paced path passes
 * because its inter-nibble gap is ~1 us; a 2-cycle low (21 ns, plain 4-cycle
 * cell at clkdiv=1) fails on dense content, and moving the flip into the low
 * phase (1-cycle-high cell) is WORSE (a positive glitch on a low CLK fakes a
 * rising edge - even single-event blocks fail deterministically). So: keep
 * the flip inside the CLK-high phase and stretch the LOW phase - s_dat_gap
 * extra cycles (runtime `pio_sdcard gap <n>`, split across the two low-phase
 * instructions' delay fields, max 30) push the next rising edge away from the
 * flip. DUT gap sweeps (FDEBUG shows no FIFO stall at any gap - purely SI
 * settle time): FAT-table content (0x0->0x7 three-line rise every 4 bytes)
 * fails at 0/2/7 and passes at 15; alternating 00/FF content (0x0->0xF FULL
 * four-line flip every byte, the write-test pattern) was marginal at 15 and
 * clean at 30 during the P4 sweep. The threshold is CONTENT- and BENCH-
 * dependent. RE-MEASURED 2026-07-03 (card/jumpers reseated since P4): this
 * bench now reads the 00/FF worst-case cleanly down to gap=0 (soak 6/6), and
 * throughput plateaus at gap<=5 - below that the per-block software cost
 * (scan/arm/finalize), not the wire, limits read, so gap=0 is no faster than
 * gap=5 and has zero SI margin. Default 5 = full read speed with margin:
 * fatfs read 1246 (gap 30) -> 3500 KiB/s (gap 5), data verified. Raise via
 * `pio_sdcard gap <n>` if a different bench/card shows dropped nibbles. */
#ifndef CONFIG_HAL_PIO_SDH_DAT_GAP
#define CONFIG_HAL_PIO_SDH_DAT_GAP 0
#endif
#ifndef CONFIG_HAL_PIO_SDH_PAD_DRIVE
#define CONFIG_HAL_PIO_SDH_PAD_DRIVE 3
#endif
/* Pad configuration for all six pins, sticky across init/hw_setup. Boot
 * defaults from Kconfig; runtime override via hal_pio_sdh_set_pad_cfg
 * (`pio_sdcard pad`). pull_up=0 is for SD adapters with EXTERNAL pull-ups
 * (single defined pull instead of internal+external in parallel). */
static uint8_t s_pad_drive = CONFIG_HAL_PIO_SDH_PAD_DRIVE;
#ifdef CONFIG_HAL_PIO_SDH_PULL_NONE
static uint8_t s_pad_pull_up = 0;
#else
static uint8_t s_pad_pull_up = 1;
#endif
static uint8_t s_pad_smt = 1;
/* extra CLK-low cycles between nibbles (read); boot default from Kconfig
 * (CONFIG_HAL_PIO_SDH_DAT_GAP, default 0 = full 24 MHz wire, clean under
 * HS+rising). Runtime override via `pio_sdcard gap`. A reboot restores this. */
static uint8_t s_dat_gap = CONFIG_HAL_PIO_SDH_DAT_GAP;
/* Write-direction inter-nibble gap. The read gap law is card-driven; the write
 * aggressor is the HOST pad (different physics), so it gets its own knob.
 * Default 0: earlier bench "SSO write failures" that motivated a large gap were
 * traced to the READBACK read-SI (content-dependent CRC16 on the read path),
 * NOT the write - the card accepted every write (CRC status 0x2, TRAN) at all
 * gaps. The HS + rising-latch read fix repaired read-SI, so the write gap no
 * longer masks anything. With the packed expand (SD-phase-bound, not expand-
 * bound), the wire time dominates, and dropping the gap is the throughput lever:
 * gap 4->0 raises fspeed write ~4.0 -> ~6.4 MB/s (+59%) with the same block, and
 * the SSO-worst `pio_sdcard write` pattern verifies 26/26 at gap 0. The base
 * loop still holds one CLK-low `nop` between the OUT and the rising edge, so gap
 * 0 is not zero DAT settling. Raise via `pio_sdcard wgap` if a bench/card ever
 * shows a rejected write (rc=-3) or a readback mismatch. */
static uint8_t s_wr_gap = 0;
/* HS-mode read timing: 0 = DS (sample the settled CLK-low phase = the nibble the
 * card launched on the previous falling edge); 1 = rising latch (sample the
 * CLK-high phase = the nibble launched on the rising edge, per SD High-Speed
 * mode after a CMD6 switch). Set by `hal_pio_sdh_switch_hs` / `pio_sdcard rising`. */
static uint8_t s_rising;

/* ---- DMA data path (P4b) --------------------------------------------------
 * PIO per-SM FIFO data registers + DMA mux request ids (same map hal_pio_cci
 * uses): TXF0 @+0x10 / RXF0 @+0x20, stride 4; pio_tx[sm]=20+sm, pio_rx[sm]=16+sm. */
#define SDH_PIO_TXF(sm)       (PIO_BASE + 0x10u + (uint32_t)(sm) * 4u)
#define SDH_PIO_RXF(sm)       (PIO_BASE + 0x20u + (uint32_t)(sm) * 4u)
#define SDH_PIO_TX_REQ(sm)    (20u + (uint32_t)(sm))
#define SDH_PIO_RX_REQ(sm)    (16u + (uint32_t)(sm))

/* One block = 1024 data nibbles + 16 CRC nibbles. The DMA phase packs
 * SDH_PACK_NIBS nibbles per RX word (autopush threshold 4*N); TX still paces
 * one word per nibble (the cell's `pull` gates one CLK cycle each).
 * N=8 (threshold 32) is AVOIDED: 32 encodes as 0 in the 5-bit PUSH_THRESH
 * field, and 0-encoded thresholds are exactly where this silicon's shifter
 * already proved broken on the pull side (autopull thr=1 delivers 0). */
#define SDH_PACK_NIBS         4u
#define SDH_BLK_NIBS          1040u
#define SDH_BLK_RXWORDS       (SDH_BLK_NIBS / SDH_PACK_NIBS)

/* 1-bit DMA capture: the proven k_rxbit_prog cell (`in pins,1`, one DAT0 bit per
 * clock) packed 16 bits per RX word (autopush 16 - same safe threshold as 4-bit;
 * 32 is avoided per the note above). 512 bytes = 4096 data bits = 256 words, + 1
 * word (16 bits) for the CRC = 257 RX words (fits s_raw[260]). TX paces one clock
 * per bit = 257*16 = 4112 clocks; that exceeds the DMA's 4095 transferLength, so
 * the pace is fed in two segments (the SM cleanly stalls on `pull` between them -
 * legal clock gating). */
#define SDH_BLK_1B_DATAW      256u                        /* 4096 data bits / 16  */
#define SDH_BLK_1B_RXWORDS    (SDH_BLK_1B_DATAW + 1u)     /* + CRC word = 257     */
#define SDH_BLK_1B_CLKS       (SDH_BLK_1B_RXWORDS * 16u)  /* 4112 pace clocks     */
#define SDH_BLK_1B_TXSEG      2048u                        /* seg1 (<4095), word-aligned */

/* Write DMA feed: one nibble per word (the cell pulls one word per CLK, the
 * OUT shifter being broken). A block expands to 4 N_WR idle + 1 start + 1024
 * data + 16 CRC + 3 end/idle words. The 3 trailing idle (vs the minimum 1)
 * pad the total to 1048 = a multiple of 4, so the INCR4 burst DMA feed has no
 * partial-burst tail; extra idle-high nibbles before the token are harmless. */
#define SDH_WR_WORDS          (4u + 1u + 1024u + 16u + 3u)
#define SDH_WR_BURST          DMA_BURST_INCR4   /* 1048 % 4 == 0 */

/* W3 SM-autonomous write: one program drives a COUNTED nibble stream (jmp x--),
 * then reads the CRC status token and waits out programming busy on the SM
 * (jmp pin loops on DAT0) - so the CPU is free to expand the next block during
 * the whole SD phase (feed + token + busy). The DATA phase uses `out pins,4`
 * with AUTOPULL (thresh 32): 8 wire nibbles PACK into one 32-bit feed word and
 * the SM auto-refills the OSR from the FIFO every 8 outs. Packing is the whole
 * point of the layout: the block expand was measured STORE-bound (~45 us of
 * 1024 nocache word stores) + LOAD-bound (~25 us of 512 byte reads), NOT
 * CRC-bound. Packing 8/word cuts the feed to 1(count)+135 = 136 words and lets
 * the expand pack whole 4-byte words with ONE SWAR nibble-swap (128 word loads
 * + 128 word stores instead of 512 byte loads + 1024 nibble stores), so expand
 * drops ~86 -> ~26 us and the write becomes wire-bound (~read speed). The 31
 * idle prefix nibbles (vs the SD-minimum 2) pad the layout so the 1024 data
 * nibbles start word-aligned (4 prefix words) and the total is 136 (% 4 for the
 * INCR4 burst); the idle-high prefix precedes the start bit so it cannot collide
 * with the card. jmp-pin/wait + out-multi + autopull all proven on this silicon
 * (see hal_pio_sdh_dbg_opcode_probe2 / _outpack_probe). */
#define SDH_AUTO_DRIVE_NIBS   (31u + 1u + 1024u + 16u + 1u)      /* 1073 wire nibs */
#define SDH_AUTO_PACK_WORDS   ((SDH_AUTO_DRIVE_NIBS + 7u) / 8u)  /* 135 packed     */
#define SDH_AUTO_WORDS        (1u + SDH_AUTO_PACK_WORDS)         /* 136, %4 == 0   */

static hal_pio_sdh_cfg_t s_cfg;
static uint32_t s_rca;
static uint8_t  s_access_mode;       /* 1 = sector addressing (SDHC/SDXC)      */
static uint8_t  s_bus_width = 4;     /* active DATA width: 4 (ACMD6=2) or 1    */
static uint8_t  s_dma_ready;         /* DMA channels configured by hw_setup    */
static uint8_t  s_wauto = 1;         /* SM-autonomous write path (W3): default   */
static uint32_t s_auto_status;       /* last CRC status token (0x2 = accepted)  */
static uint8_t  s_rd_cmd23;          /* DBG: last multi-read used CMD23 bound    */
static uint8_t  s_recover = 1;       /* DBG: 0 = disable abort recovery (stress) */
static uint8_t  s_rauto = 1;         /* self-clocked 4-bit read loop (R3): default */
static uint8_t  s_read_ncyc = 4u;    /* read loop SM cycles/nibble: 4=24MHz(default),
                                      * 3=32MHz, 2=48MHz (faster CLK, less SI margin) */
static uint8_t  s_rhunt = 1u;        /* hardware `jmp pin` start-bit hunt (rising 4-cyc
                                      * only): default. SM self-clocks the ~20 idle
                                      * recovery nibbles at wire speed instead of the
                                      * CPU polling one nibble per MMIO round-trip
                                      * (+17-24% read). `rhunt 0` = CPU-scan fallback. */
static uint8_t  s_hunt_active;       /* effective hunt state (rhunt AND rising AND 4-cyc);
                                      * set by setup, read by arm so the arm path matches
                                      * the loaded program - rcyc 2/3 and non-HS (DS)
                                      * cards force the CPU scan even when s_rhunt=1. */

/* Ping-pong raw capture buffers + the TX pacing word. DMA needs coherent
 * memory: .nocache_ram is the uncached OCRAM alias (NOLOAD - s_pace_word gets
 * its value in pio_sdh_dma_setup, not from an initializer). The ping-pong pair
 * exists for THROUGHPUT (CPU unpacks/CRCs block N while DMA captures N+1);
 * correctness never needs it - CLK is TX-word paced, so a stalled drain just
 * freezes the bus, it cannot drop data. */
static ATTR_NOCACHE_RAM_SECTION uint32_t s_raw[2][SDH_BLK_RXWORDS];
static ATTR_NOCACHE_RAM_SECTION uint32_t s_pace_word;
/* Write-path DMA feed buffers (nibble-per-word, expanded on the CPU then
 * streamed by the TX DMA). Nocache OCRAM: measured equivalent to cached-OCRAM
 * +clean (the feed is limited by the INCR1 per-word DMA request rate ~8M
 * words/s, not buffer access latency), and a plain .bss copy in PSRAM is worse
 * (slow DMA reads starve the SM). Ping-pong pair: the CPU expands block N+1
 * into the idle buffer while the DMA feeds block N from the other, hiding the
 * ~82 us expand under the ~127 us feed. ~8.2 KB. */
static ATTR_NOCACHE_RAM_SECTION uint32_t s_wexp[2][SDH_WR_WORDS];
/* TX channel config mode: the read pace stream is src-fixed, the write feed is
 * src-incrementing. DMA_Channel_Update_SrcMemcfg keeps SI, so the channel must
 * be re-primed with a full dma_reload when switching read<->write. */
static uint8_t s_tx_mode;            /* 0 = read pace, 1 = write feed */
static uint16_t s_clkdiv_int = 60;   /* default 400 kHz (24 MHz / 60) */
static uint8_t  s_clkdiv_frac;

/* Read-path phase profiling (R2): SysTimer runs at SOC_TIMER_TICK_PER_US (4)
 * ticks/us. The DMA read path splits into scan (start-bit hunt) / arm (DMA
 * reload+enable) / wait (block clocks in) / finalize (unpack+CRC16); summing
 * ticks per phase across a whole read attributes the per-block fixed software
 * cost that dominates once the wire is fast. A few timer reads per block is
 * negligible next to a ~100 us block, so accumulation is always on; the totals
 * are dumped on demand via hal_pio_sdh_prof_get(). */
static struct {
    uint64_t scan, arm, wait, finalize;   /* summed SysTimer ticks */
    uint32_t blocks;
} s_prof;

/* Write-path phase profiling (W1): expand (CPU nibble layout + CRC16) / feed
 * (DMA-fed data phase, wire + drain) / tokenbusy (CRC status token + card
 * program busy). Attributes the DMA-write per-block cost the same way s_prof
 * does for reads. */
static struct {
    uint64_t expand, feed, tokenbusy;     /* summed SysTimer ticks */
    uint64_t tb_cell;                     /* of tokenbusy: setup_dat0_rx switch */
    uint32_t busyclks;                    /* actual card busy-release poll count */
    uint32_t blocks;
} s_wprof;

/* Defined below the read path (write path helpers); the CMD18 STOP busy-wait
 * reuses them. */
static void pio_sdh_setup_dat0_rx(void);
static int  dat0_wait_busy_release(void);
static inline uint32_t resp_payload(const uint32_t raw[2]);

/* ---- GLB clock + pinmux (mirror hal_pio_cmsis_dap) ------------------------ */
static inline uint32_t glb_rd(uint32_t off) { return *(volatile uint32_t *)(uintptr_t)(GLB_BASE + off); }
static inline void     glb_wr(uint32_t off, uint32_t v) { *(volatile uint32_t *)(uintptr_t)(GLB_BASE + off) = v; }

static void pio_sdh_glb_clock(const hal_pio_sdh_cfg_t *cfg)
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

static void pin_func(uint8_t pin, uint32_t fun, uint32_t mode, uint32_t pull)
{
    /* drive default 3 (max): holds CLK firmly against glitches coupled from
     * the DAT lines switching together on dense 4-bit data (caused a card
     * double-clock -> dropped nibble on block reads). s_pad_* are the sticky
     * runtime/Kconfig pad settings (hal_pio_sdh_set_pad_cfg). */
    GLB_GPIO_Cfg_Type c = { .gpioPin = pin, .gpioFun = fun, .gpioMode = mode,
        .pullType = pull, .drive = s_pad_drive, .smtCtrl = s_pad_smt };
    GLB_GPIO_Init(&c);
}

static void pio_sdh_pinmux(const hal_pio_sdh_cfg_t *c)
{
    uint32_t pull = s_pad_pull_up ? GPIO_PULL_UP : GPIO_PULL_NONE;
    /* CLK is always driven; CMD/D0-D3 are bidirectional: GLB mode INPUT keeps
     * the input buffer on while the PIO pindir controls the drive (probed OK). */
    pin_func(c->pin_clk, GPIO_FUN_PIO, GPIO_MODE_OUTPUT, pull);   /* side-set */
    pin_func(c->pin_cmd, GPIO_FUN_PIO, GPIO_MODE_INPUT,  pull);
    pin_func(c->pin_d0,  GPIO_FUN_PIO, GPIO_MODE_INPUT,  pull);   /* DAT0/busy */
    pin_func((uint8_t)(c->pin_d0 + 1), GPIO_FUN_PIO, GPIO_MODE_INPUT, pull);
    pin_func((uint8_t)(c->pin_d0 + 2), GPIO_FUN_PIO, GPIO_MODE_INPUT, pull);
    pin_func((uint8_t)(c->pin_d0 + 3), GPIO_FUN_PIO, GPIO_MODE_INPUT, pull);
}

static void set_clkdiv_for(uint32_t hz)
{
    uint32_t div = hz ? (SDH_PIO_BIT_HZ / hz) : (SDH_PIO_BIT_HZ / SDH_INIT_HZ);
    if (div < 1u)      div = 1u;
    if (div > 0xFFFFu) div = 0xFFFFu;
    s_clkdiv_int = (uint16_t)div;
    s_clkdiv_frac = 0;
}

/* Configure SM(cmd) for the CMD bit cell: CLK on side-set, CMD on out+in. */
static void pio_sdh_setup_cmd_sm(const hal_pio_sdh_cfg_t *c)
{
    uint8_t sm = c->sm_cmd;
    pio_load_program(k_cmdbit_prog, ARRAY_LEN(k_cmdbit_prog), CMD_CLK_ORIGIN);
    pio_sm_set_pins(sm, c->pin_cmd, 1, c->pin_cmd, 1, c->pin_clk, 1);
    pio_sm_set_set_base(sm, c->pin_cmd, 1);
    /* One FIFO word per bit both ways: explicit pull (autopull OFF - both its
     * threshold edge cases and the OUT shifter are broken on this silicon) and
     * autopush per sampled bit (in LEFT: bit lands at ISR[0], JTAG-proven). */
    pio_sm_set_shift(sm, false, false, 32, false, true, 1);
    pio_sm_set_clkdiv(sm, s_clkdiv_int, s_clkdiv_frac);
    pio_sm_set_wrap(sm, CMD_CLK_WRAP_TOP, CMD_CLK_ORIGIN, false);
    pio_sm_set_pindirs(sm, c->pin_clk, 1, 1);    /* CLK driven                 */
    pio_sm_set_pindirs(sm, c->pin_cmd, 1, 0);    /* CMD released until TX      */
    pio_sm_exec(sm, (uint16_t)(0x0000u | CMD_CLK_ORIGIN)); /* PC = origin      */
}

/* Swap the per-phase bit cell into the shared imem window and re-point the PC.
 * TX uses the drive cell (k_cmdbit_prog), RX uses the sample-only cell
 * (k_rxbit_prog); both are 4 words at CMD_CLK_ORIGIN. Safe to call while the SM
 * is enabled - it is stalled on `pull` (empty FIFO) at every phase boundary. */
static void pio_sdh_load_cell(uint8_t sm, const uint16_t *prog, uint8_t len)
{
    pio_load_program(prog, len, CMD_CLK_ORIGIN);
    pio_sm_set_wrap(sm, CMD_CLK_WRAP_TOP, CMD_CLK_ORIGIN, false);
    pio_sm_exec(sm, (uint16_t)(0x0000u | CMD_CLK_ORIGIN));
}

/* One-shot config of the TX (pace) / RX (capture) DMA channels for the data
 * phase. Mirrors hal_pio_cci's proven setup, but the TX source never
 * increments - it re-reads s_pace_word (all-1s dummy) once per nibble.
 * dma_controller_init + dma_channel_attach (not legacy dma_init) so arming
 * here cannot abort another peripheral's in-flight transfer. */
static void pio_sdh_dma_setup(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    dma_device_t txdev = {
        .id = DMA0_ID, .ch = s_cfg.dma_tx_ch, .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE, .dst_req = SDH_PIO_TX_REQ(sm),
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE, .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_size = DMA_BURST_INCR1, .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_32BIT, .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .transfer_mode = DMA_LLI_ONCE_MODE, .lli_cfg = NULL, .callback = NULL,
    };
    dma_device_t rxdev = {
        .id = DMA0_ID, .ch = s_cfg.dma_rx_ch, .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = SDH_PIO_RX_REQ(sm), .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE, .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR1, .dst_burst_size = DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_32BIT, .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .transfer_mode = DMA_LLI_ONCE_MODE, .lli_cfg = NULL, .callback = NULL,
    };
    s_pace_word = 0xFFFFFFFFu;             /* .nocache_ram is NOLOAD */
    dma_controller_init(DMA0_ID);
    GLB_Set_DMA_CLK(1, (GLB_DMA_CLK_ID_Type)(GLB_DMA_CLK_DMA0_CH0 + s_cfg.dma_tx_ch));
    GLB_Set_DMA_CLK(1, (GLB_DMA_CLK_ID_Type)(GLB_DMA_CLK_DMA0_CH0 + s_cfg.dma_rx_ch));
    dma_setConfig(&txdev);
    dma_channel_attach(DMA0_ID, s_cfg.dma_tx_ch);
    dma_setConfig(&rxdev);
    dma_channel_attach(DMA0_ID, s_cfg.dma_rx_ch);
    /* R6: prime each channel's CONTROL register (widths/burst/SI/DI) + addrs +
     * LLI=0 ONCE with a full dma_reload. The per-block re-arm in dma_capture_arm
     * then only rewrites the changing fields (RX dst ping-pong pointer + the
     * auto-decremented TransferSize) via DMA_Channel_Update_{Dst,Src}Memcfg -
     * no realloc / LLI rebuild / dcache clean (that path cost ~94 us/block). */
    dma_reload(DMA0_ID, s_cfg.dma_rx_ch, SDH_PIO_RXF(sm),
               (uint32_t)(uintptr_t)s_raw[0], SDH_BLK_RXWORDS * 4u, NULL);
    dma_reload(DMA0_ID, s_cfg.dma_tx_ch, (uint32_t)(uintptr_t)&s_pace_word,
               SDH_PIO_TXF(sm), SDH_BLK_NIBS * 4u, NULL);
    /* NOTE: pio_sm_dma_enable is NOT turned on here. The req-ack gates must
     * only be open while an armed DMA channel is listening: CPU FIFO traffic
     * with the gates open desyncs the handshake and every later DMA transfer
     * waits forever (hal_pio_scan learned the same - it scopes the gates to
     * each shift). Gates open in dma_capture_arm, close in dma_capture_wait. */
    s_tx_mode = 0;                         /* primed as read pace (src-fixed) */
    s_dma_ready = 1;
}

/* Re-prime the TX channel for the read pace stream (src-fixed s_pace_word) or
 * the write feed (src-incrementing s_wexp). DMA_Channel_Update_SrcMemcfg keeps
 * the SI bit, so the CONTROL register must be rebuilt with a full dma_reload
 * whenever the source direction flips - cheap because it runs once per read/
 * write operation, not per block. No-op when already in the requested mode. */
static void dma_tx_ensure_mode(uint8_t wr)
{
    if (s_dma_ready && s_tx_mode == wr) return;
    uint8_t sm = s_cfg.sm_cmd;
    dma_device_t tx = {
        .id = DMA0_ID, .ch = s_cfg.dma_tx_ch, .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE, .dst_req = SDH_PIO_TX_REQ(sm),
        .src_addr_inc = wr ? DMA_ADDR_INCREMENT_ENABLE : DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        /* Write feed uses an INCR4 burst (4 words per DMA grant) to beat the
         * ~8M words/s INCR1 per-word request rate; the read pace stays INCR1. */
        .src_burst_size = wr ? SDH_WR_BURST : DMA_BURST_INCR1,
        .dst_burst_size = wr ? SDH_WR_BURST : DMA_BURST_INCR1,
        .src_width = DMA_TRANSFER_WIDTH_32BIT, .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .transfer_mode = DMA_LLI_ONCE_MODE, .lli_cfg = NULL, .callback = NULL,
    };
    dma_setConfig(&tx);
    dma_channel_attach(DMA0_ID, s_cfg.dma_tx_ch);
    if (wr)
        dma_reload(DMA0_ID, s_cfg.dma_tx_ch, (uint32_t)(uintptr_t)s_wexp,
                   SDH_PIO_TXF(sm), SDH_WR_WORDS * 4u, NULL);
    else
        dma_reload(DMA0_ID, s_cfg.dma_tx_ch, (uint32_t)(uintptr_t)&s_pace_word,
                   SDH_PIO_TXF(sm), SDH_BLK_NIBS * 4u, NULL);
    s_tx_mode = wr;
}

int hal_pio_sdh_hw_setup(const hal_pio_sdh_cfg_t *cfg)
{
    if (!cfg) return -1;
    s_cfg = *cfg;
    set_clkdiv_for(SDH_INIT_HZ);
    pio_sdh_glb_clock(cfg);
    pio_sdh_pinmux(cfg);
    pio_sdh_setup_cmd_sm(cfg);
    /* NOTE: input_sync_bypass is left OFF - bypass is broken on this silicon
     * (bypassed inputs read constant 0; probed 2026-07-02). */
    pio_sm_restart(cfg->sm_cmd);
    pio_sm_enable(cfg->sm_cmd, true);
    s_dma_ready = 0;                   /* stale from a prior use_dma=1 setup */
    if (cfg->use_dma) pio_sdh_dma_setup();
    return 0;
}

int hal_pio_sdh_set_clock(uint32_t hz)
{
    set_clkdiv_for(hz);
    pio_sm_set_clkdiv(s_cfg.sm_cmd, s_clkdiv_int, s_clkdiv_frac);
    return 0;
}

/* DBG: set the inter-nibble CLK-low gap (extra SM cycles, 0..30) for the
 * 4-bit DATA cell. Takes effect on the next data phase (setup_dat_sm). */
int hal_pio_sdh_set_dat_gap(uint8_t cycles)
{
    if (cycles > 30u) return -1;
    s_dat_gap = cycles;
    return 0;
}

/* DBG: set the write-direction inter-nibble CLK-low gap (0..30). Takes effect
 * on the next write data phase (setup_dat_tx_sm). Sweep this if a DMA-rate
 * write ever shows host-pad SSO corruption. */
int hal_pio_sdh_set_wr_gap(uint8_t cycles)
{
    if (cycles > 30u) return -1;
    s_wr_gap = cycles;
    return 0;
}

/* Select the SM-autonomous write path (W3: feed+token+busy on the SM, CPU
 * expands the next block during the whole SD phase). *last_status (nullable)
 * returns the last CRC status token (0x2 = accepted). NOTE: this path needs a
 * small write gap (`pio_sdcard wgap 4`+) for SI - back-to-back autonomous
 * blocks corrupt the wire at gap 0 (the CPU path's per-block token/busy time
 * happens to provide that settle). */
int hal_pio_sdh_set_wauto(uint8_t on, uint32_t *last_status)
{
    s_wauto = on ? 1u : 0u;
    if (last_status) *last_status = s_auto_status;
    return 0;
}

/* DBG: did the last multi-block read use a CMD23-bounded transfer (1) or fall
 * back to open-ended CMD18 + CMD12 (0)? Lets a stress test confirm the card
 * actually accepted CMD23 before crediting/blaming the bounded-read path. */
uint8_t hal_pio_sdh_dbg_last_read_cmd23(void)
{
    return s_rd_cmd23;
}

/* DBG: enable(1)/disable(0) the aborted-transfer stop recovery in xfer_stop.
 * Disable to stress the RAW datapath - a failing transfer then wedges instead of
 * being nursed back to TRAN, so a setting is only "stable" if it never fails. */
void hal_pio_sdh_dbg_set_recover(uint8_t on)
{
    s_recover = on ? 1u : 0u;
}

/* 1 (default) = route 4-bit DMA reads through the self-clocked read loop
 * (read_blocks_read_auto); 0 = the TX-paced read_blocks_dma. The self-clocked
 * loop is +17% (the TX pace's INCR1 req-ack throttled the CLK below the SM's
 * native rate). A/B via `pio_sdcard rauto <0|1>` + fspeed. */
void hal_pio_sdh_dbg_set_rauto(uint8_t on)
{
    s_rauto = on ? 1u : 0u;
}

/* Read-loop SM cycles per nibble -> SD CLK on this 96 MHz PIO:
 *   4 = 24 MHz (default, SOLID), 3 = 32 MHz, 2 = 48 MHz.
 * The extra cycles beyond 2 are SI settle margin, not a functional need (min=2).
 * Measured on this bench: raw read throughput PLATEAUS at ~32 MHz (3-cyc and
 * 2-cyc tie at ~9.5 MB/s, +15% over 4-cyc) - the non-wire per-block cost caps it,
 * so 48 MHz buys nothing over 32 MHz but loses margin. BOTH 3-cyc and 2-cyc are
 * SI-MARGINAL on jumper wiring (intermittent CRC/data errors, content- and
 * length-dependent); neither is reliable here - they need a controlled-impedance
 * PCB. And the FS read path is finalize/scan-bound, so a faster wire barely
 * moves FS throughput anyway. Any value other than 2 or 3 selects the solid
 * 4-cycle default. Runtime: `pio_sdcard rcyc <2|3|4>`. */
void hal_pio_sdh_dbg_set_read2c(uint8_t ncyc)
{
    s_read_ncyc = (ncyc == 2u || ncyc == 3u) ? ncyc : 4u;
}

/* Hardware `jmp pin` start-bit hunt (rising 4-cyc self-clocked read only). The
 * CPU scan clocks the ~20 idle recovery nibbles between blocks one-at-a-time via
 * MMIO round-trips (~577 ns/nibble, ~12 us/block - the dominant non-wire read
 * cost); this hands that hunt to the SM, which clocks them at wire speed
 * (~42 ns/nibble) and falls into the read loop the instant DAT0 drops. Correct
 * only if the scan->read-loop handoff stays nibble-aligned - verify with the
 * byte-verify oracle (rloop crc count / fspeed). Runtime: `pio_sdcard rhunt
 * <0|1>`; ignored unless the rising 4-cyc path is active. */
void hal_pio_sdh_dbg_set_rhunt(uint8_t on)
{
    s_rhunt = on ? 1u : 0u;
}

/* CMD13 SEND_STATUS on the enumerated card. *status = the 32-bit R1 card status
 * register (error flags in the high bits); *state = CURRENT_STATE field [12:9]
 * (4=tran, 5=data, 6=rcv, 7=prg, 8=dis). Returns 0, or -1 if no card / no
 * response / CRC7 fail (a stranded card that stops answering CMD13 is itself a
 * signal). Read-only - does not clear or move the card state. Used by xfer_stop
 * to distinguish a stranded card from an already-stopped one. */
int hal_pio_sdh_card_status(uint32_t *status, uint32_t *state)
{
    uint32_t raw[2];
    if (!s_rca) return -1;
    pio_sdh_setup_cmd_sm(&s_cfg);
    if (hal_pio_sdh_send_cmd(13, s_rca << 16, 48, raw)) return -1;
    if (sdio_check_r1_crc(raw[0], raw[1])) return -1;
    uint32_t r1 = resp_payload(raw);
    if (status) *status = r1;
    if (state) *state = (r1 >> 9) & 0xFu;
    return 0;
}

/* ---- Single-block (CMD24) write settle ---------------------------------
 * dat0_wait_busy_release() sniffs DAT0 for only 8 clocks to decide whether the
 * card asserted programming busy; if it samples the window late it concludes
 * "no busy" and returns immediately, letting the NEXT command go out while the
 * card is still programming (the "a read right after a CMD24 write fails"
 * fault noted in hal_pio_sdh_write_blocks). Multi-block writes are covered by
 * xfer_stop()'s CMD12 + CMD13 state check; single-block writes had no such
 * handshake. CMD13 is legal while the card is programming, so polling it until
 * READY_FOR_DATA + TRAN is the spec-correct settle. Metadata-heavy workloads
 * (FAT/bitmap/dir updates from many small files) are almost all single-block,
 * which is why they fail where a large sequential write passes. */
static uint32_t s_wr1_total;        /* single-block writes issued            */
static uint32_t s_wr1_still_prg;    /* ...still programming after DAT0 said done */
static uint32_t s_wr1_ready_to;     /* ...ready-poll timed out (real fault)  */
static uint64_t s_wr1_settle_tk;    /* total ticks spent in the settle       */
static uint8_t  s_wr1_settle = 1u;  /* runtime A/B switch (default on)       */

static int wait_ready_tran(uint32_t timeout_ms)
{
    uint64_t t0 = SysTimer_GetLoadValue();
    uint64_t limit = (uint64_t)timeout_ms * 1000ull * SOC_TIMER_TICK_PER_US;
    uint32_t polls = 0;
    int rc = -1;
    for (;;) {
        uint32_t st = 0, state = 0;
        if (hal_pio_sdh_card_status(&st, &state) == 0 &&
            (st & (1u << 8)) != 0u && state == 4u) {     /* READY_FOR_DATA + tran */
            if (polls) s_wr1_still_prg++;
            rc = 0;
            break;
        }
        polls++;
        if (SysTimer_GetLoadValue() - t0 > limit) { s_wr1_ready_to++; break; }
    }
    s_wr1_settle_tk += SysTimer_GetLoadValue() - t0;
    return rc;
}

/* Enable/disable the post-single-block-write CMD13 settle at runtime, so a
 * test can A/B it on the SAME card in the SAME state. Default on. */
void hal_pio_sdh_set_wr1_settle(uint8_t on)
{
    s_wr1_settle = on ? 1u : 0u;
}

uint8_t hal_pio_sdh_get_wr1_settle(void)
{
    return s_wr1_settle;
}

/* Single-block write settle stats since the last call (then cleared). Lets a
 * test prove whether the DAT0 busy sniff is missing the card's busy window,
 * and what the settle actually costs. */
void hal_pio_sdh_wr1_diag_get(uint32_t *total, uint32_t *still_prg,
                              uint32_t *timeouts, uint32_t *settle_us)
{
    if (total)     *total     = s_wr1_total;
    if (still_prg) *still_prg = s_wr1_still_prg;
    if (timeouts)  *timeouts  = s_wr1_ready_to;
    if (settle_us) *settle_us = (uint32_t)(s_wr1_settle_tk / SOC_TIMER_TICK_PER_US);
    s_wr1_total = s_wr1_still_prg = s_wr1_ready_to = 0;
    s_wr1_settle_tk = 0;
}

/* DBG: re-init the CLK pad with a different drive strength at runtime (SI
 * experiment - a hard-driven CLK edge rings at the card and can be seen as an
 * extra clock; a weaker driver doubles as source termination). */
int hal_pio_sdh_set_clk_drive(uint8_t drive)
{
    if (!s_cfg.pin_clk) return -1;               /* hw_setup not run yet */
    GLB_GPIO_Cfg_Type c = { .gpioPin = s_cfg.pin_clk, .gpioFun = GPIO_FUN_PIO,
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = s_pad_pull_up ? GPIO_PULL_UP : GPIO_PULL_NONE,
        .drive = (drive > 3u) ? 3u : drive, .smtCtrl = s_pad_smt };
    GLB_GPIO_Init(&c);
    return 0;
}

/* Reconfigure ALL six pads (drive 0-3, internal pull on/off, schmitt) and
 * make the setting sticky: pio_sdh_pinmux reads s_pad_* on every later
 * init/hw_setup. Applied immediately when the hw is already up. pull_up=0
 * pairs with SD adapters that carry external pull-up resistors. */
int hal_pio_sdh_set_pad_cfg(uint8_t drive, uint8_t pull_up, uint8_t smt)
{
    s_pad_drive = (drive > 3u) ? 3u : drive;
    s_pad_pull_up = pull_up ? 1u : 0u;
    s_pad_smt = smt ? 1u : 0u;
    if (s_cfg.pin_clk) pio_sdh_pinmux(&s_cfg);   /* live re-apply */
    return 0;
}

/* Run n clock cycles with CMD released (dummy high bits). SD cards need at
 * least 74 clocks after power-up before the first command. */
void hal_pio_sdh_clock_burst(uint32_t n)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t fed = 0, drained = 0, guard = 0;
    pio_sm_set_pindirs(sm, s_cfg.pin_cmd, 1, 0);
    while (drained < n) {
        while (fed < n && pio_txlevel(sm) < PIO_FIFO_DEPTH) {
            pio_sm_put(sm, 1u);
            fed++;
        }
        while (drained < n && pio_rxlevel(sm) > 0u) {
            (void)pio_sm_get(sm);
            drained++;
        }
        if (++guard > SDH_CMD_TIMEOUT_SPINS) return;
    }
}

/* TX one 48-bit frame (one FIFO word per bit, MSB first), draining echoes. */
static int cmd_tx_frame(uint64_t frame)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t fed = 0, drained = 0, guard = 0;

    while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);
    pio_sdh_load_cell(sm, k_cmdbit_prog, ARRAY_LEN(k_cmdbit_prog));  /* drive cell */
    pio_sm_set_pindirs(sm, s_cfg.pin_cmd, 1, 1);
    while (drained < 48u) {
        while (fed < 48u && pio_txlevel(sm) < PIO_FIFO_DEPTH) {
            pio_sm_put(sm, (uint32_t)((frame >> (47u - fed)) & 1u));
            fed++;
        }
        while (drained < 48u && pio_rxlevel(sm) > 0u) {
            (void)pio_sm_get(sm);
            drained++;
        }
        if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
    }
    pio_sm_set_pindirs(sm, s_cfg.pin_cmd, 1, 0);   /* release for the response */
    return 0;
}

/* Clock dummy bits and scan the sampled CMD line: wait for the start bit,
 * then collect nbits response bits (start bit included as bits[0]=0).
 *
 * STRICT one-word-in-flight: the clock stops dead on the response's last bit.
 * The earlier pipelined version (FIFO topped up to depth 8) overran the
 * response by up to ~8 clocks; the card counts those toward N_AC/N_WR, and a
 * cache-fast CMD17 right after a write legally starts its read DATA (N_AC
 * min = 2 clocks) inside the overrun - the start bit and first nibbles were
 * clocked away and drained as garbage before the DATA phase even began
 * (deterministic mid-pattern capture, rx CRC = idle 0xffff). Zero overrun
 * also kills the stale-RX-word class of bugs at the source. */
static int cmd_rx_bits(uint8_t *bits, uint32_t nbits)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t max_dummy = 192u + nbits;
    uint32_t sent = 0, got = 0, guard = 0;
    int started = 0;

    /* CMD is released (cmd_tx_frame left pindir=in); switch to the sample-only
     * cell so the shared pin is never re-driven while the card replies. */
    pio_sdh_load_cell(sm, k_rxbit_prog, ARRAY_LEN(k_rxbit_prog));
    while (got < nbits) {
        if (sent++ >= max_dummy) return -1;        /* response never came     */
        pio_sm_put(sm, 1u);
        guard = 0;
        while (pio_rxlevel(sm) == 0u) {
            if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
        }
        uint32_t b = pio_sm_get(sm) & 1u;
        if (!started) {
            if (b == 0u) {
                started = 1;
                bits[got++] = 0;
            }
        } else {
            bits[got++] = (uint8_t)b;
        }
    }
    return 0;
}

int hal_pio_sdh_send_cmd(uint8_t cmd, uint32_t arg, uint8_t resp_bits, uint32_t raw[2])
{
    uint32_t w0, w1;
    sdio_make_cmd_words(cmd, arg, resp_bits, &w0, &w1);
    uint64_t frame = ((uint64_t)(w0 & 0xFFFFFFu) << 24) | ((w1 >> 8) & 0xFFFFFFu);

    /* N_RC/N_CR: >= 8 clocks between the previous command/response and this
     * command's start bit. The response reader is strictly zero-overrun, so
     * the gap must be provided explicitly (it used to ride on the overrun). */
    hal_pio_sdh_clock_burst(8);
    if (cmd_tx_frame(frame)) return -1;
    if (!resp_bits) return 0;
    if (resp_bits != 48u) return -1;   /* R2 goes through send_cmd_r2 */

    uint8_t bits[48];
    if (cmd_rx_bits(bits, 48u)) return -1;

    uint32_t r0 = 0, r1 = 0;
    for (int i = 0; i < 32; i++) r0 = (r0 << 1) | bits[i];
    for (int i = 32; i < 48; i++) r1 = (r1 << 1) | bits[i];
    raw[0] = r0;
    raw[1] = r1;
    return 0;
}

/* 136-bit R2 (CID/CSD): start+dir+6 reserved bits, then the 128-bit register
 * (whose last byte is its own CRC7+end). out[16] = the 128 bits, MSB first. */
static int send_cmd_r2(uint8_t cmd, uint32_t arg, uint8_t out[16])
{
    uint32_t w0, w1;
    sdio_make_cmd_words(cmd, arg, 136, &w0, &w1);
    uint64_t frame = ((uint64_t)(w0 & 0xFFFFFFu) << 24) | ((w1 >> 8) & 0xFFFFFFu);

    hal_pio_sdh_clock_burst(8);                        /* N_RC/N_CR gap */
    if (cmd_tx_frame(frame)) return -1;

    uint8_t bits[136];
    if (cmd_rx_bits(bits, 136u)) return -1;
    for (int i = 0; i < 16; i++) {
        uint8_t v = 0;
        for (int b = 0; b < 8; b++) v = (uint8_t)((v << 1) | bits[8 + i * 8 + b]);
        out[i] = v;
    }
    return 0;
}

/* ---- SD enumeration (P3) -------------------------------------------------- */

/* 32-bit payload of a 48-bit response (status/OCR/RCA field). */
static inline uint32_t resp_payload(const uint32_t raw[2])
{
    return ((raw[0] & 0x00FFFFFFu) << 8) | ((raw[1] >> 8) & 0xFFu);
}

static int acmd(uint8_t cmd, uint32_t arg, uint32_t rca, uint32_t raw[2])
{
    uint32_t r[2];
    if (hal_pio_sdh_send_cmd(55, rca << 16, 48, r)) return -1;
    if (sdio_check_r1_crc(r[0], r[1])) return -1;
    return hal_pio_sdh_send_cmd(cmd, arg, 48, raw);
}

/* Capacity from CSD v1/v2 (csd[0] holds bits 127:120). */
static void csd_decode(const uint8_t csd[16], hal_pio_sdh_card_t *card)
{
    uint8_t ver = (uint8_t)(csd[0] >> 6);
    if (ver == 1) {                      /* CSD v2.0: SDHC/SDXC, sector mode */
        uint32_t c_size = (((uint32_t)csd[7] & 0x3Fu) << 16) |
                          ((uint32_t)csd[8] << 8) | csd[9];
        card->block_count = (c_size + 1u) * 1024u;
        card->access_mode = 1;
    } else {                             /* CSD v1.0: SDSC, byte mode */
        uint32_t c_size = (((uint32_t)csd[6] & 0x03u) << 10) |
                          ((uint32_t)csd[7] << 2) | (csd[8] >> 6);
        uint32_t mult = (uint32_t)(((csd[9] & 0x03u) << 1) | (csd[10] >> 7));
        uint32_t read_bl_len = (uint32_t)(csd[5] & 0x0Fu);
        /* blocks = (c_size+1) * 2^(mult+2) * 2^read_bl_len / 512 */
        card->block_count = (c_size + 1u) << (mult + 2u + read_bl_len - 9u);
        card->access_mode = 0;
    }
    card->block_size = 512;
}

int hal_pio_sdh_init(const hal_pio_sdh_cfg_t *cfg, hal_pio_sdh_card_t *out)
{
    uint32_t raw[2];
    hal_pio_sdh_card_t card = { 0 };
    int rc = hal_pio_sdh_hw_setup(cfg);
    if (rc) return rc;
    s_rising = 0u;               /* re-enumeration resets the card to DS timing */

    hal_pio_sdh_clock_burst(96);                       /* >=74 power-up clocks */
    hal_pio_sdh_send_cmd(0, 0, 0, raw);                /* CMD0 GO_IDLE         */
    hal_pio_sdh_clock_burst(16);

    /* CMD8: mandatory for SD 2.0+; echo check pattern. */
    int v2 = 0;
    if (hal_pio_sdh_send_cmd(8, 0x000001AAu, 48, raw) == 0 &&
        sdio_check_r1_crc(raw[0], raw[1]) == 0 &&
        (resp_payload(raw) & 0xFFFu) == 0x1AAu) {
        v2 = 1;
    }

    /* ACMD41 until the card leaves busy (OCR[31] set). R3 has no CRC. */
    uint32_t ocr = 0;
    int ready = 0;
    for (int tries = 0; tries < 1000; tries++) {
        uint32_t arg = 0x00FF8000u | (v2 ? 0x40000000u : 0u);
        if (acmd(41, arg, 0, raw) == 0) {
            ocr = resp_payload(raw);
            if (ocr & 0x80000000u) {
                ready = 1;
                break;
            }
        }
        hal_pio_sdh_clock_burst(400);                  /* ~1 ms @400 kHz       */
    }
    if (!ready) return -2;

    if (send_cmd_r2(2, 0, card.cid)) return -3;        /* CMD2 ALL_SEND_CID    */

    if (hal_pio_sdh_send_cmd(3, 0, 48, raw)) return -4;/* CMD3 -> RCA          */
    if (sdio_check_r1_crc(raw[0], raw[1])) return -4;
    uint32_t rca = resp_payload(raw) >> 16;

    if (send_cmd_r2(9, rca << 16, card.csd)) return -5;/* CMD9 SEND_CSD        */
    csd_decode(card.csd, &card);
    if (ocr & 0x40000000u) card.access_mode = 1;       /* CCS confirms sector  */

    if (hal_pio_sdh_send_cmd(7, rca << 16, 48, raw)) return -6;  /* select     */
    if (sdio_check_r1_crc(raw[0], raw[1])) return -6;
    hal_pio_sdh_clock_burst(2000);                     /* R1b busy allowance   */

    if (acmd(6, 2, rca, raw)) return -7;               /* ACMD6: 4-bit bus     */
    if (sdio_check_r1_crc(raw[0], raw[1])) return -7;
    card.bus_width = 4;

    (void)hal_pio_sdh_switch_hs(NULL);                 /* best-effort HS+rising  */
    s_bus_width = 4;

    s_rca = rca;
    s_access_mode = card.access_mode;
    if (out) *out = card;
    hal_pio_sdh_set_clock(SDH_XFER_HZ);                /* 400 kHz was enum-only */
    return 0;
}

/* ---- Block read (P4): CPU-paced 4-bit DATA reception ---------------------- */

/* Point the SM at the DATA lines: IN_BASE=pin_d0, sample-only DATA cell.
 * 4-bit: one nibble per word (in pins,4 / autopush 4). 1-bit: one DAT0 bit per
 * word (the CMD-phase k_rxbit_prog `in pins,1` cell reused with IN_BASE moved
 * to pin_d0 / autopush 1). CLK side-set and pindirs are unchanged (CLK stays
 * driven; DAT0-3 are inputs). */
static void pio_sdh_setup_dat_sm(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    pio_sm_set_pins(sm, s_cfg.pin_cmd, 1, s_cfg.pin_d0, 4, s_cfg.pin_clk, 1);
    if (s_bus_width == 1u) {
        pio_sm_set_shift(sm, false, false, 32, false, true, 1); /* autopush /bit */
        pio_sdh_load_cell(sm, k_rxbit_prog, ARRAY_LEN(k_rxbit_prog));
    } else {
        /* Build the DATA cell with the current inter-nibble gap (see the
         * geometry note above s_dat_gap): delay fields on the two low-phase
         * instructions, 15 max each. */
        uint16_t g1 = (s_dat_gap > 15u) ? 15u : s_dat_gap;
        uint16_t g0 = (s_dat_gap > 15u) ? (uint16_t)(s_dat_gap - 15u) : 0u;
        uint16_t cell[4] = {
            (uint16_t)(0x80A0 | (g0 << 8)), /* pull block side 0 [g0] CLK low  */
            (uint16_t)(0xA042 | (g1 << 8)), /* nop        side 0 [g1] CLK low  */
            0xB042,                         /* nop        side 1  CLK high     */
            0x5004,                         /* in pins,4  side 1  pad @ low    */
        };
        pio_sm_set_shift(sm, false, false, 32, false, true, 4); /* autopush /nib */
        pio_sdh_load_cell(sm, cell, 4);
    }
    /* One divider for the whole bus: the DATA phase inherits whatever
     * hal_pio_sdh_set_clock last set (400 kHz default from hw_setup). Above the
     * CPU's per-nibble drain cost the TX FIFO under-runs and CLK gates low
     * between nibbles (stutter); that is benign - the bus is fully static and
     * the strict one-word-in-flight path behaved byte-identically. At clkdiv=1
     * each granted nibble still gets a full-rate 2-SM-cycle (~21 ns) CLK high
     * pulse, i.e. HW-SDH-like 24 MHz edge geometry. (DMA lifts the CPU ceiling
     * later.) */
    pio_sm_set_clkdiv(sm, s_clkdiv_int, s_clkdiv_frac);
    pio_sm_exec(sm, 0xA0C3);                                  /* mov isr, null    */
}

/* CRC16-CCITT (poly 0x1021, seed 0) - one bit, MSB-first; the SD data-line CRC. */
static inline uint16_t crc16_bit(uint16_t crc, uint32_t bit)
{
    uint32_t msb = (crc >> 15) & 1u;
    crc = (uint16_t)(crc << 1);
    if (msb ^ (bit & 1u)) crc ^= 0x1021u;
    return crc;
}

/* Byte-wise CRC16-CCITT (poly 0x1021, seed 0, MSB-first) via a 256-entry table
 * built once. Same result as feeding each byte MSB-first through crc16_bit, but
 * 8x fewer ops - the 1-bit DMA finalize CRC over 512 bytes was serial per-bit
 * and the read throughput bottleneck. */
static uint16_t s_crc16_tab[256];
static uint8_t  s_crc16_tab_ready;
static uint16_t crc16_bytes(const uint8_t *p, uint32_t n)
{
    if (!s_crc16_tab_ready) {
        for (uint32_t i = 0; i < 256u; i++) {
            uint16_t c = (uint16_t)(i << 8);
            for (int k = 0; k < 8; k++)
                c = (uint16_t)((c & 0x8000u) ? (c << 1) ^ 0x1021u : (c << 1));
            s_crc16_tab[i] = c;
        }
        s_crc16_tab_ready = 1u;
    }
    uint16_t crc = 0;
    for (uint32_t i = 0; i < n; i++)
        crc = (uint16_t)((crc << 8) ^ s_crc16_tab[(crc >> 8) ^ p[i]]);
    return crc;
}

/* All four DATA-line CRC16s over one 512-byte block, packed as the 16 wire
 * nibbles MSB-first (nibble j = bits[63-4j .. 60-4j]). Fast path is the
 * carlk3 branchless sdio_crc16_4bit straight over the byte buffer (it
 * byte-swaps internally; host-tested nibble-equivalent to the bitwise
 * reference, 202 blocks incl. all-FF/all-00). The bitwise per-line fallback
 * covers non-32-bit-aligned caller buffers (RISC-V alignment). */
static uint64_t block_crc16(const uint8_t *buf)
{
    if (((uintptr_t)buf & 3u) == 0u)
        return sdio_crc16_4bit((const uint32_t *)(const void *)buf, 128u);

    uint16_t crc[4] = { 0, 0, 0, 0 };
    for (uint32_t i = 0; i < 1024u; i++) {
        uint32_t nib = (uint32_t)(buf[i >> 1] >> ((i & 1u) ? 0u : 4u)) & 0xFu;
        for (int L = 0; L < 4; L++)
            crc[L] = crc16_bit(crc[L], nib >> L);
    }
    uint64_t c = 0;
    for (uint32_t j = 0; j < 16u; j++) {
        uint32_t nib = 0;
        for (int L = 0; L < 4; L++)
            nib |= (uint32_t)(((crc[L] >> (15u - j)) & 1u) << L);
        c = (c << 4) | nib;
    }
    return c;
}

/* Capture one 512-byte block on DAT3..0: scan for the start bit, then capture
 * 1024 data nibbles + 16 CRC nibbles, verifying each line's CRC16.
 *
 * The clock is kept FREE-RUNNING for the whole block: the TX FIFO is topped up
 * every iteration so the cmd/clk SM never stalls on `pull` (which would drop CLK
 * low for a CPU-latency-long, variable time between nibbles). An earlier strict
 * one-word-in-flight version stuttered the clock once per nibble; on dense 4-bit
 * data (many DAT lines switching together) that reproducibly cost one dropped
 * nibble mid-block (block CRC16 caught it - block 3, a FAT VBR, always failed
 * with the 0x55AA signature shifted one nibble). A uniform continuous clock
 * removes the stall, and is the shape the DMA path needs anyway.
 *
 * RX is drained in order; an RX-full stall only pauses CLK high (no data loss),
 * so the sole thing to avoid is TX under-run, which the top-up prevents. Feeding
 * stops the moment the block is complete to bound over-clocking into the next
 * block's start bit on a CMD18 stream. The DAT SM must already be set up
 * (pio_sdh_setup_dat_sm); this does NOT touch the SM config, so consecutive
 * CMD18 blocks stay in sync. */
static int capture_block(uint8_t *buf)
{
    uint8_t sm = s_cfg.sm_cmd;
    enum { NDATA = 1024u, NCRC = 16u, NTOTAL = NDATA + NCRC };
    uint64_t rx = 0;                      /* received CRC16 nibbles, MSB 1st */
    uint32_t idx = 0, guard = 0;
    int started = 0, armed = 0;
    uint8_t hi = 0;

    while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);

    for (;;) {
        /* Keep CLK free-running: refill TX while nibbles remain, stop once done. */
        if (idx < NTOTAL && pio_txlevel(sm) < PIO_FIFO_DEPTH)
            pio_sm_put(sm, 0xFu);

        if (pio_rxlevel(sm) == 0u) {
            if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
            continue;
        }
        guard = 0;
        uint32_t nib = pio_sm_get(sm) & 0xFu;

        if (!started) {
            /* Arm on an idle-high sample first (see dma_scan_start_bit), then
             * the next DAT0-low is the start bit. */
            if ((nib & 0x1u) != 0u) armed = 1;
            else if (armed) started = 1;
            continue;                                  /* drop idle + start    */
        }

        if (idx < NDATA) {
            if ((idx & 1u) == 0u) hi = (uint8_t)nib;   /* high nibble          */
            else buf[idx >> 1] = (uint8_t)((hi << 4) | nib);
        } else {
            rx = (rx << 4) | nib;                      /* CRC nibble, MSB 1st  */
        }
        if (++idx == NTOTAL) break;
    }

    return (block_crc16(buf) == rx) ? 0 : -2;
}

/* 1-bit (SSO-probe / fallback) capture: same free-running-clock scheme as
 * capture_block but every RX word is a single DAT0 bit. 4096 data bits + 16
 * CRC bits, single-line CRC16 over the serial stream. Only DAT0 ever toggles,
 * so a pass here where 4-bit fails isolates the multi-line simultaneous-
 * switching event as the trigger. */
static int capture_block_1bit(uint8_t *buf)
{
    uint8_t sm = s_cfg.sm_cmd;
    enum { NDATA = 4096u, NCRC = 16u, NTOTAL = NDATA + NCRC };
    uint16_t crc = 0, rxcrc = 0;
    uint32_t idx = 0, guard = 0;
    int started = 0, armed = 0;
    uint8_t byte = 0;

    while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);

    for (;;) {
        if (idx < NTOTAL && pio_txlevel(sm) < PIO_FIFO_DEPTH)
            pio_sm_put(sm, 0xFu);

        if (pio_rxlevel(sm) == 0u) {
            if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
            continue;
        }
        guard = 0;
        uint32_t bit = pio_sm_get(sm) & 0x1u;

        if (!started) {
            if (bit != 0u) armed = 1;                  /* idle first, then...  */
            else if (armed) started = 1;               /* start bit (DAT0 low) */
            continue;
        }

        if (idx < NDATA) {
            byte = (uint8_t)((byte << 1) | bit);       /* MSB-first             */
            if ((idx & 7u) == 7u) buf[idx >> 3] = byte;
            crc = crc16_bit(crc, bit);
        } else {
            rxcrc |= (uint16_t)(bit << (15u - (idx - NDATA)));
        }
        if (++idx == NTOTAL) break;
    }

    return (crc == rxcrc) ? 0 : -2;
}

/* ---- DMA capture (P4b): scan -> arm -> overlap -> wait -> finalize --------- */

/* Scan for the block start bit with strict one-word-in-flight pacing so both
 * FIFOs and the ISR are guaranteed empty the instant it is found - the clean
 * switch point from the nibble-granular scan (autopush 4) to the packed DMA
 * phase (autopush 32, word-aligned data). */
static int dma_scan_start_bit(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t guard = 0;
    int armed = 0;
    /* Flush stale RX words (CMD-phase response overrun) BEFORE scanning: a
     * leftover word here both fakes a start bit and offsets the RX DMA's word
     * count against the TX pace count - the whole capture shifts and the
     * residue compounds into the next block (rx CRC reads as idle 0xffff). */
    while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);
    for (;;) {
        pio_sm_put(sm, 0xFu);
        while (pio_rxlevel(sm) == 0u) {
            if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
        }
        guard = 0;
        /* Require one idle-high sample before accepting a low as the start
         * bit: right after a write's programming-busy the card can still be
         * releasing DAT0, and a not-yet-idle low here started the capture a
         * few nibbles early (read tail ran into idle, rx CRC = 0xffff). */
        if ((pio_sm_get(sm) & 0x1u) == 0u) {
            if (armed) return 0;                       /* start bit (DAT0 low) */
        } else {
            armed = 1;
        }
    }
}

/* Find the start bit, then hand the whole block to hardware: RX DMA drains 130
 * packed words into raw, TX DMA feeds 1040 pacing words from the never-
 * incrementing s_pace_word. Returns with the transfer IN FLIGHT. */
static int dma_capture_arm(uint32_t *raw)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint64_t t0 = SysTimer_GetLoadValue();
    int rc = dma_scan_start_bit();
    uint64_t t1 = SysTimer_GetLoadValue();
    s_prof.scan += t1 - t0;
    if (rc) return rc;
    pio_sm_set_shift(sm, false, false, 32, false, true, 4u * SDH_PACK_NIBS);
    /* R6: fast re-arm - the channels were fully configured once in
     * pio_sdh_dma_setup, so only reload the RX dst (ping-pong) + both
     * TransferSizes (auto-decremented to 0 by the last transfer). SI/DI/width/
     * burst/LLI in CONTROL are preserved. TransferSize is in transfer units
     * (32-bit words), not bytes. RX src (RXF) and TX dst (TXF) don't increment,
     * so they survive from the priming reload. */
    DMA_Channel_Update_DstMemcfg(DMA0_ID, s_cfg.dma_rx_ch,
                                 (uint32_t)(uintptr_t)raw, SDH_BLK_RXWORDS);
    DMA_Channel_Update_SrcMemcfg(DMA0_ID, s_cfg.dma_tx_ch,
                                 (uint32_t)(uintptr_t)&s_pace_word, SDH_BLK_NIBS);
    dma_channel_start(DMA0_ID, s_cfg.dma_rx_ch);   /* RX armed before TX clocks */
    dma_channel_start(DMA0_ID, s_cfg.dma_tx_ch);
    /* Open the req-ack gates ONLY now that both channels are armed and
     * listening (see pio_sdh_dma_setup note); the SM is stalled on `pull`
     * with both FIFOs empty, so the first request goes straight to the DMA. */
    pio_sm_dma_enable(sm, true, true);
    s_prof.arm += SysTimer_GetLoadValue() - t1;
    return 0;
}

static int dma_capture_wait(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t guard = 8000000u;                     /* block @24MHz is ~43 us    */
    while (dma_channel_check_busy(DMA0_ID, s_cfg.dma_rx_ch) && --guard) { }
    while (dma_channel_check_busy(DMA0_ID, s_cfg.dma_tx_ch) && guard) { }
    /* hand the FIFOs back to the CPU (scan/CMD phases) and drop back to one
     * nibble per word (SM is stalled on `pull` with both FIFOs empty, same
     * safe point as load_cell) */
    pio_sm_dma_enable(sm, false, false);
    pio_sm_set_shift(sm, false, false, 32, false, true, 4);
    if (!guard) {
        dma_channel_stop(DMA0_ID, s_cfg.dma_rx_ch);
        dma_channel_stop(DMA0_ID, s_cfg.dma_tx_ch);
        return -1;
    }
    return 0;
}

/* Nibble i (0..1039) from the packed raw capture: SDH_PACK_NIBS MSB-first
 * nibbles per word, valid bits right-aligned (IN shifts LEFT into the LSB). */
static inline uint32_t raw_nib(const uint32_t *raw, uint32_t i)
{
    uint32_t sh = 4u * (SDH_PACK_NIBS - 1u - (i % SDH_PACK_NIBS));
    return (raw[i / SDH_PACK_NIBS] >> sh) & 0xFu;
}

/* Unpack one DMA-captured block into buf and verify the CRC16s. Pure CPU
 * work - this is the stage the ping-pong overlaps with the next block's
 * capture, and (as the throughput bottleneck) it uses the fast block_crc16
 * path rather than per-nibble bitwise CRC. */
static int block_finalize_dma(const uint32_t *raw, uint8_t *buf)
{
    /* Each raw word carries SDH_PACK_NIBS MSB-first nibbles right-aligned =
     * SDH_PACK_NIBS/2 big-endian bytes in its low half. */
    enum { WBYTES = SDH_PACK_NIBS / 2u, NDATAW = 512u / WBYTES };
    for (uint32_t w = 0; w < NDATAW; w++) {
        uint32_t v = raw[w];
        for (uint32_t b = 0; b < WBYTES; b++)
            buf[w * WBYTES + b] = (uint8_t)(v >> (8u * (WBYTES - 1u - b)));
    }

    uint64_t rx = 0;
    for (uint32_t j = 0; j < 16u; j++)                        /* CRC16 nibbles  */
        rx = (rx << 4) | raw_nib(raw, 1024u + j);
    return (block_crc16(buf) == rx) ? 0 : -2;
}

/* Ping-pong block pipeline: while DMA captures block i into s_raw[i&1], the
 * CPU unpacks + CRC-checks block i-1 from the other buffer. */
static int read_blocks_dma(uint8_t *buf, uint32_t count)
{
    int rc = 0;
    uint64_t t;
    dma_tx_ensure_mode(0);          /* pace stream (a prior write leaves src-inc) */
    for (uint32_t i = 0; i < count; i++) {
        rc = dma_capture_arm(s_raw[i & 1u]);
        if (rc) break;
        if (i) {
            t = SysTimer_GetLoadValue();
            int frc = block_finalize_dma(s_raw[(i - 1u) & 1u], buf + (i - 1u) * 512u);
            s_prof.finalize += SysTimer_GetLoadValue() - t;
            s_prof.blocks++;
            if (frc) {
                (void)dma_capture_wait();          /* quiesce, then bail       */
                return frc;
            }
        }
        t = SysTimer_GetLoadValue();
        rc = dma_capture_wait();
        s_prof.wait += SysTimer_GetLoadValue() - t;
        if (rc) break;
    }
    if (rc == 0 && count) {
        t = SysTimer_GetLoadValue();
        rc = block_finalize_dma(s_raw[(count - 1u) & 1u], buf + (count - 1u) * 512u);
        s_prof.finalize += SysTimer_GetLoadValue() - t;
        s_prof.blocks++;
    }
    return rc;
}

/* ---- R3: self-clocked 4-bit read loop (default) --------------------------- */

/* Load the self-clocked read program at CMD_CLK_ORIGIN: the pull-gated scan
 * cell (words 0-3, driven CPU-paced by dma_scan_start_bit) PLUS a counted read
 * loop (words 4-10) that clocks SDH_BLK_NIBS nibbles autonomously via side-set
 * + `jmp x--`. Unlike the TX-paced cell (whose `pull` stalls the CLK when the
 * TX DMA is starved by CPU bus traffic), this loop self-clocks, so the wire
 * keeps running while the CPU finalizes the previous block => the ping-pong can
 * finally overlap. RX DMA drains as usual; NO TX pace channel. The scan cell
 * keeps wrap 0-3; the loop lives above it and exits only via its own jmps. */
static void pio_sdh_setup_dat_read_auto_sm(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint16_t g1 = (s_dat_gap > 15u) ? 15u : s_dat_gap;
    uint16_t g0 = (s_dat_gap > 15u) ? (uint16_t)(s_dat_gap - 15u) : 0u;
    /* DS: CLK low(0,1) high(2,rising)(3,in) - `in` reads the settled low phase.
     * Rising latch (HS): CLK high(0,rising)(1) low(2,in)(3) - `in` reads the
     * high phase = the nibble the card launched on the rising edge. Same 4-cyc
     * cell, sample point shifted half a clock; scan cell + read loop both flip. */
    const uint16_t prog_ds[] = {
        (uint16_t)(0x80A0 | (g0 << 8)),  /* 0 pull block   side0[g0] scan pace     */
        (uint16_t)(0xA042 | (g1 << 8)),  /* 1 nop          side0[g1]                */
        0xB042,                          /* 2 nop          side1                    */
        0x5004,                          /* 3 in pins,4    side1  scan sample       */
        0x80A0,                          /* 4 pull block   side0  count word        */
        0x6020,                          /* 5 out x,32     side0  X = nibbles-1      */
        (uint16_t)(0xA042 | (g0 << 8)),  /* 6 nop          side0[g0] loopA CLK low   */
        0xB042,                          /* 7 nop          side1  CLK high (rising)  */
        0x5004,                          /* 8 in pins,4    side1  sample nibble      */
        (uint16_t)(0x0046 | (g1 << 8)),  /* 9 jmp x-- 6    side0[g1] loop            */
        0x000A,                          /* 10 jmp 10      side0  halt (CLK low)     */
    };
    const uint16_t prog_rising[] = {
        (uint16_t)(0x90A0 | (g0 << 8)),  /* 0 pull block   side1[g0] CLK high (rising)*/
        0xB042,                          /* 1 nop          side1  CLK high            */
        0x4004,                          /* 2 in pins,4    side0  sample (word0 high)  */
        (uint16_t)(0xA042 | (g1 << 8)),  /* 3 nop          side0[g1] CLK low           */
        0x80A0,                          /* 4 pull block   side0  count word          */
        0x6020,                          /* 5 out x,32     side0  X = nibbles-1        */
        (uint16_t)(0xB042 | (g0 << 8)),  /* 6 nop          side1[g0] loopA CLK high    */
        0xB042,                          /* 7 nop          side1  CLK high             */
        0x4004,                          /* 8 in pins,4    side0  sample (word6 high)   */
        (uint16_t)(0x0046 | (g1 << 8)),  /* 9 jmp x-- 6    side0[g1] loop CLK low       */
        0x000A,                          /* 10 jmp 10      side0  halt                  */
    };
    /* EXPERIMENT: shorter read loops -> faster CLK (24 MHz at 4 cyc is default).
     * Scan cell (rising, words 0-3) + count (4-5) shared; only the loop shrinks.
     * The 2-FF input sync latency samples the CLK-low (most-settled) phase in all
     * variants. 2-cyc has NO settle margin (48 MHz, SI-marginal); 3-cyc keeps one
     * CLK-low settle `nop` (32 MHz). */
    const uint16_t prog_2c[] = {                     /* 2 cyc/nibble -> 48 MHz     */
        (uint16_t)(0x90A0 | (g0 << 8)),  /* 0 pull block   side1[g0] scan (rising)   */
        0xB042,                          /* 1 nop          side1                      */
        0x4004,                          /* 2 in pins,4    side0  scan sample          */
        (uint16_t)(0xA042 | (g1 << 8)),  /* 3 nop          side0[g1]                   */
        0x80A0,                          /* 4 pull block   side0  count word           */
        0x6020,                          /* 5 out x,32     side0  X = nibbles-1         */
        0x4004,                          /* 6 in pins,4    side0  LOOP CLK low, sample  */
        0x1046,                          /* 7 jmp x-- 6    side1  CLK high (rising)     */
        0x0008,                          /* 8 jmp 8        side0  halt                  */
    };
    const uint16_t prog_3c[] = {                     /* 3 cyc/nibble -> 32 MHz     */
        (uint16_t)(0x90A0 | (g0 << 8)),  /* 0 pull block   side1[g0] scan (rising)   */
        0xB042,                          /* 1 nop          side1                      */
        0x4004,                          /* 2 in pins,4    side0  scan sample          */
        (uint16_t)(0xA042 | (g1 << 8)),  /* 3 nop          side0[g1]                   */
        0x80A0,                          /* 4 pull block   side0  count word           */
        0x6020,                          /* 5 out x,32     side0  X = nibbles-1         */
        0x4004,                          /* 6 in pins,4    side0  LOOP CLK low, sample  */
        0xA042,                          /* 7 nop          side0  CLK low settle        */
        0x1046,                          /* 8 jmp x-- 6    side1  CLK high (rising)     */
        0x0009,                          /* 9 jmp 9        side0  halt                  */
    };
    /* Hardware start-hunt (s_rhunt, rising 4-cyc only): words 0-3 replace the
     * CPU-paced pull-gated scan with a self-clocked `jmp pin` loop. The SM
     * clocks the ~20 idle recovery nibbles at wire speed (0.84 us) and falls
     * into the read loop the instant DAT0 goes low, instead of the CPU polling
     * one nibble per MMIO round-trip (~12 us). Words 4-10 are byte-identical to
     * prog_rising, so the proven read-loop alignment is untouched; only the
     * scan-cell -> read-loop handoff phase is new. */
    const uint16_t prog_rising_hunt[] = {
        (uint16_t)(0xB042 | (g0 << 8)),  /* 0 nop        side1[g0] hunt CLK high(rising)*/
        0xB042,                          /* 1 nop        side1  CLK high               */
        0xA042,                          /* 2 nop        side0  CLK low settle          */
        (uint16_t)(0x00C0 | (g1 << 8)),  /* 3 jmp pin 0  side0[g1] DAT0 high->hunt,low->fall*/
        0x80A0,                          /* 4 pull block side0  count word             */
        0x6020,                          /* 5 out x,32   side0  X = nibbles-1           */
        (uint16_t)(0xB042 | (g0 << 8)),  /* 6 nop        side1[g0] loopA CLK high        */
        0xB042,                          /* 7 nop        side1  CLK high                */
        0x4004,                          /* 8 in pins,4  side0  sample                  */
        (uint16_t)(0x0046 | (g1 << 8)),  /* 9 jmp x-- 6  side0[g1] loop CLK low          */
        0x000A,                          /* 10 jmp 10    side0  halt                    */
    };
    const uint16_t *prog; uint32_t prog_len;
    uint8_t hunt = (s_rhunt && s_rising && s_read_ncyc == 4u) ? 1u : 0u;
    s_hunt_active = hunt;   /* arm path must match the program loaded here */
    if (hunt)                   { prog = prog_rising_hunt; prog_len = ARRAY_LEN(prog_rising_hunt); }
    else if (s_read_ncyc == 2u) { prog = prog_2c; prog_len = ARRAY_LEN(prog_2c); }
    else if (s_read_ncyc == 3u) { prog = prog_3c; prog_len = ARRAY_LEN(prog_3c); }
    else { prog = s_rising ? prog_rising : prog_ds; prog_len = ARRAY_LEN(prog_ds); }
    pio_sm_set_pins(sm, s_cfg.pin_cmd, 1, s_cfg.pin_d0, 4, s_cfg.pin_clk, 1);
    pio_sm_set_shift(sm, false, false, 32, false, true, 4); /* autopush /nib (scan) */
    pio_sm_set_clkdiv(sm, s_clkdiv_int, s_clkdiv_frac);
    pio_load_program(prog, prog_len, CMD_CLK_ORIGIN);
    if (hunt) {
        /* The hunt must FALL THROUGH word 3 (jmp pin not-taken = start bit) into
         * the read loop; the scan-cell wrap 0-3 would instead trap PC back at
         * word 0. Widen the wrap to the halt word so only explicit jmps redirect
         * PC, and point `jmp pin` at DAT0. */
        pio_sm_set_wrap(sm, (uint8_t)(prog_len - 1u), CMD_CLK_ORIGIN, false);
        pio_sm_set_jmp_pin(sm, s_cfg.pin_d0);
    } else {
        pio_sm_set_wrap(sm, CMD_CLK_WRAP_TOP, CMD_CLK_ORIGIN, false); /* scan wraps 0-3 */
    }
    pio_sm_exec(sm, 0xA0C3);                                  /* mov isr, null      */
    pio_sm_exec(sm, (uint16_t)(0x0000u | CMD_CLK_ORIGIN));    /* PC = 0 (scan cell) */
}

/* Arm one self-clocked block: point the SM back at the scan cell, CPU-scan the
 * start bit, then switch to packed autopush, redirect to the read loop, hand it
 * the nibble count, and open only the RX gate. Returns with the block self-
 * clocking IN FLIGHT so the CPU can finalize the previous block. */
static int dma_read_auto_arm(uint32_t *raw)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint64_t t0 = SysTimer_GetLoadValue();
    if (s_hunt_active) {   /* NOT s_rhunt: only when setup loaded the hunt program
                            * (rhunt AND rising AND 4-cyc). rcyc 2/3 or a non-HS card
                            * load a scan-cell program and must take the CPU-scan arm. */
        /* Hardware start-hunt: no CPU per-nibble scan. Flush any stale RX (CMD
         * response residue), clear the ISR, arm the packed RX DMA + count, then
         * release the SM at the hunt cell - it self-clocks the idle recovery
         * nibbles and falls into the read loop the instant DAT0 goes low. */
        while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);
        pio_sm_exec(sm, 0xA0C3);                             /* mov isr, null      */
        pio_sm_set_shift(sm, false, false, 32, false, true, 4u * SDH_PACK_NIBS);
        DMA_Channel_Update_DstMemcfg(DMA0_ID, s_cfg.dma_rx_ch,
                                     (uint32_t)(uintptr_t)raw, SDH_BLK_RXWORDS);
        dma_channel_start(DMA0_ID, s_cfg.dma_rx_ch);
        pio_sm_dma_enable(sm, false, true);                  /* RX gate only       */
        pio_sm_put(sm, SDH_BLK_NIBS - 1u);                   /* X: read-loop count */
        pio_sm_exec(sm, (uint16_t)(0x0000u | CMD_CLK_ORIGIN)); /* PC = 0 (hunt)    */
        s_prof.scan += SysTimer_GetLoadValue() - t0;         /* setup only (tiny)  */
        return 0;
    }
    pio_sm_exec(sm, (uint16_t)(0x0000u | CMD_CLK_ORIGIN));    /* PC = 0 (scan cell) */
    pio_sm_set_shift(sm, false, false, 32, false, true, 4);   /* scan autopush /nib */
    int rc = dma_scan_start_bit();
    uint64_t t1 = SysTimer_GetLoadValue();
    s_prof.scan += t1 - t0;
    if (rc) return rc;
    pio_sm_set_shift(sm, false, false, 32, false, true, 4u * SDH_PACK_NIBS);
    pio_sm_exec(sm, (uint16_t)(0x0000u | 4u));               /* PC = 4 (read loop) */
    DMA_Channel_Update_DstMemcfg(DMA0_ID, s_cfg.dma_rx_ch,
                                 (uint32_t)(uintptr_t)raw, SDH_BLK_RXWORDS);
    dma_channel_start(DMA0_ID, s_cfg.dma_rx_ch);
    pio_sm_dma_enable(sm, false, true);                      /* RX gate only       */
    pio_sm_put(sm, SDH_BLK_NIBS - 1u);                       /* X: self-clock count */
    s_prof.arm += SysTimer_GetLoadValue() - t1;
    return 0;
}

/* Wait for the self-clocked block's RX DMA to drain, then close the gate and
 * hand the FIFO back to the CPU (scan phase of the next block). */
static int dma_read_auto_wait(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t guard = 8000000u;
    while (dma_channel_check_busy(DMA0_ID, s_cfg.dma_rx_ch) && --guard) { }
    pio_sm_dma_enable(sm, false, false);
    pio_sm_set_shift(sm, false, false, 32, false, true, 4);
    if (!guard) { dma_channel_stop(DMA0_ID, s_cfg.dma_rx_ch); return -1; }
    return 0;
}

/* R3 probe pipeline: same ping-pong shape as read_blocks_dma but each block is
 * self-clocked, so the CPU finalize of block i-1 overlaps block i's wire. */
static int read_blocks_read_auto(uint8_t *buf, uint32_t count)
{
    int rc = 0;
    uint64_t t;
    pio_sdh_setup_dat_read_auto_sm();
    for (uint32_t i = 0; i < count; i++) {
        rc = dma_read_auto_arm(s_raw[i & 1u]);
        if (rc) break;
        if (i) {
            t = SysTimer_GetLoadValue();
            int frc = block_finalize_dma(s_raw[(i - 1u) & 1u], buf + (i - 1u) * 512u);
            s_prof.finalize += SysTimer_GetLoadValue() - t;
            s_prof.blocks++;
            if (frc) { (void)dma_read_auto_wait(); return frc; }
        }
        t = SysTimer_GetLoadValue();
        rc = dma_read_auto_wait();
        s_prof.wait += SysTimer_GetLoadValue() - t;
        if (rc) break;
    }
    if (rc == 0 && count) {
        t = SysTimer_GetLoadValue();
        rc = block_finalize_dma(s_raw[(count - 1u) & 1u], buf + (count - 1u) * 512u);
        s_prof.finalize += SysTimer_GetLoadValue() - t;
        s_prof.blocks++;
    }
    return rc;
}

/* Unpack a 1-bit DMA capture (256 words of 16 MSB-first DAT0 bits = 2 big-endian
 * bytes each) into buf, then verify the single-line CRC16 (word 256, MSB-first
 * in [15:0]) with the byte-table CRC (the per-bit CRC was the throughput
 * bottleneck; the card computes the same CRC16-CCITT over the serial DAT0). */
static int block_finalize_1bit(const uint32_t *raw, uint8_t *buf)
{
    for (uint32_t w = 0; w < SDH_BLK_1B_DATAW; w++) {
        uint32_t v = raw[w];                             /* IN LEFT: first bit @15 */
        buf[2u * w]      = (uint8_t)(v >> 8);            /* MSB-first byte pair    */
        buf[2u * w + 1u] = (uint8_t)v;
    }
    uint16_t rxcrc = (uint16_t)(raw[SDH_BLK_1B_DATAW] & 0xFFFFu);
    return (crc16_bytes(buf, 512u) == rxcrc) ? 0 : -2;
}

/* Capture ONE 1-bit block by DMA: scan the start bit, switch the packed cell to
 * autopush 16, then drain SDH_BLK_1B_RXWORDS RX words while pacing SDH_BLK_1B_CLKS
 * CLKs in TWO TX segments (the 4095 transferLength cap). The SM stalls on `pull`
 * between segments (a benign mid-block clock pause; the card holds DAT). The RX
 * transfer spans both segments - it drains whenever the SM produces. */
static int dma_capture_1bit(uint32_t *raw)
{
    uint8_t sm = s_cfg.sm_cmd;
    if (dma_scan_start_bit()) return -1;
    pio_sm_set_shift(sm, false, false, 32, false, true, 16u);      /* pack /16 bits */
    DMA_Channel_Update_DstMemcfg(DMA0_ID, s_cfg.dma_rx_ch,
                                 (uint32_t)(uintptr_t)raw, SDH_BLK_1B_RXWORDS);
    dma_channel_start(DMA0_ID, s_cfg.dma_rx_ch);                  /* RX armed first */
    DMA_Channel_Update_SrcMemcfg(DMA0_ID, s_cfg.dma_tx_ch,
                                 (uint32_t)(uintptr_t)&s_pace_word, SDH_BLK_1B_TXSEG);
    dma_channel_start(DMA0_ID, s_cfg.dma_tx_ch);                  /* pace seg 1     */
    pio_sm_dma_enable(sm, true, true);
    uint32_t guard = 8000000u;
    while (dma_channel_check_busy(DMA0_ID, s_cfg.dma_tx_ch) && --guard) { }
    if (guard) {                                                  /* pace seg 2     */
        DMA_Channel_Update_SrcMemcfg(DMA0_ID, s_cfg.dma_tx_ch,
                                     (uint32_t)(uintptr_t)&s_pace_word,
                                     SDH_BLK_1B_CLKS - SDH_BLK_1B_TXSEG);
        dma_channel_start(DMA0_ID, s_cfg.dma_tx_ch);             /* clock resumes  */
        while (dma_channel_check_busy(DMA0_ID, s_cfg.dma_rx_ch) && --guard) { }
        while (dma_channel_check_busy(DMA0_ID, s_cfg.dma_tx_ch) && guard) { }
    }
    pio_sm_dma_enable(sm, false, false);
    pio_sm_set_shift(sm, false, false, 32, false, true, 1u);      /* restore /1 bit */
    if (!guard) {
        dma_channel_stop(DMA0_ID, s_cfg.dma_rx_ch);
        dma_channel_stop(DMA0_ID, s_cfg.dma_tx_ch);
        return -1;
    }
    return 0;
}

/* 1-bit multi-block read via DMA: capture then finalize each block. Reliable
 * where 4-bit fails (single DAT line = no 4-line SSO double-clock) at ~1/1.8 the
 * 4-bit wire rate, and ~10x the CPU 1-bit path (which round-trips per bit). No
 * ping-pong yet (finalize is cheap vs capture). */
static int read_blocks_dma_1bit(uint8_t *buf, uint32_t count)
{
    int rc = 0;
    dma_tx_ensure_mode(0);          /* pace stream (a prior write leaves src-inc) */
    for (uint32_t i = 0; i < count; i++) {
        rc = dma_capture_1bit(s_raw[0]);
        if (rc) break;
        rc = block_finalize_1bit(s_raw[0], buf + i * 512u);
        if (rc) break;
    }
    return rc;
}

/* Switch the card+host DATA width at runtime (card must be enumerated). ACMD6
 * arg 2 = 4-bit, 0 = 1-bit; reads dispatch on the stored width afterwards. */
int hal_pio_sdh_set_bus_width(uint8_t width)
{
    uint32_t raw[2];
    if (width != 1u && width != 4u) return -1;
    if (!s_rca) return -1;                             /* no card enumerated    */
    if (acmd(6, (width == 4u) ? 2u : 0u, s_rca, raw)) return -2;
    if (sdio_check_r1_crc(raw[0], raw[1])) return -2;
    s_bus_width = width;
    return 0;
}

/* CMD6 SWITCH_FUNC -> set High-Speed access mode (group 1 = function 1). The
 * card answers R1 on CMD then streams a 512-bit switch-status block on the DAT
 * lines (which must be clocked out for the switch to take effect); the switch
 * is active 8 clocks after. status[16] low nibble = the group-1 function
 * selection (1 = HS granted, 0xF = not supported). The 64 status bytes are
 * returned in *status (nullable). Read in the current (DS) timing - the switch
 * only changes the card's read-data edge AFTER this block. Card must be
 * enumerated + in 4-bit mode; caller then switches the host to rising-edge
 * latch (s_rising). Returns 0 (HS granted), -1 (cmd/read fail), -2 (not HS). */
int hal_pio_sdh_switch_hs(uint8_t *status)
{
    uint32_t raw[2];
    uint8_t sm = s_cfg.sm_cmd;
    uint8_t st[64];
    for (unsigned i = 0; i < 64u; i++) st[i] = 0;
    if (hal_pio_sdh_send_cmd(6, 0x80FFFFF1u, 48, raw)) return -1;   /* set grp1=HS */
    if (sdio_check_r1_crc(raw[0], raw[1])) return -1;
    pio_sdh_setup_dat_sm();
    {
        enum { NDATA = 128u, NTOTAL = NDATA + 16u };   /* 64 data bytes + CRC nibs */
        uint32_t idx = 0, guard = 0; int started = 0, armed = 0; uint8_t hi = 0;
        while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);
        for (;;) {
            if (idx < NTOTAL && pio_txlevel(sm) < PIO_FIFO_DEPTH) pio_sm_put(sm, 0xFu);
            if (pio_rxlevel(sm) == 0u) {
                if (++guard > SDH_CMD_TIMEOUT_SPINS) { pio_sdh_setup_cmd_sm(&s_cfg); return -1; }
                continue;
            }
            guard = 0;
            uint32_t nib = pio_sm_get(sm) & 0xFu;
            if (!started) { if (nib & 1u) armed = 1; else if (armed) started = 1; continue; }
            if (idx < NDATA) {
                if (!(idx & 1u)) hi = (uint8_t)nib;
                else st[idx >> 1] = (uint8_t)((hi << 4) | nib);
            }
            if (++idx == NTOTAL) break;
        }
    }
    pio_sdh_setup_cmd_sm(&s_cfg);
    hal_pio_sdh_clock_burst(8);                        /* switch takes effect +8clk */
    if (status) for (unsigned i = 0; i < 64u; i++) status[i] = st[i];
    if ((st[16] & 0x0Fu) != 1u) return -2;
    s_rising = 1u;                    /* HS granted -> host samples rising-edge data */
    return 0;
}

/* DBG: force the read sample edge (0 = DS low-phase, 1 = HS rising-latch) for
 * A/B without a CMD6 switch. */
void hal_pio_sdh_dbg_set_rising(uint8_t on)
{
    s_rising = on ? 1u : 0u;
}

/* End-of-multi-block stop/recovery, shared by the read and write paths. Issues
 * CMD12 STOP_TRANSMISSION when the transfer was open-ended (card streams until
 * stopped) OR it aborted (rc != 0) and the card is NOT already back in TRAN -
 * i.e. genuinely stranded mid-stream. Checking CMD13 first avoids a stray
 * STOP-in-TRAN (which sets ILLEGAL_COMMAND and can itself wedge the next
 * command) when a CMD23-bounded transfer's CRC error landed on its LAST block,
 * so the card had already auto-stopped. Then the R1b/programming busy is polled
 * out on DAT0 (exits in ~12 clocks when no busy appears) and returned so the
 * write path can treat a real programming-busy timeout as its own error. Leaves
 * the CMD SM up. */
static int xfer_stop(int bounded, int rc)
{
    uint32_t raw[2];
    pio_sdh_setup_cmd_sm(&s_cfg);
    if (!bounded) {
        hal_pio_sdh_send_cmd(12, 0, 48, raw);            /* open-ended: always stop */
    } else if (rc != 0 && s_recover) {
        /* Bounded + aborted: stop ONLY if the card is genuinely stranded (not
         * already auto-stopped in TRAN - a stray STOP-in-TRAN just sets
         * ILLEGAL_COMMAND). Retry a few times: on a marginal bus the CMD12 or
         * its response can itself be SI-corrupted (the same wire fault that
         * failed the transfer), so one STOP may not land. */
        for (int a = 0; a < 3; a++) {
            uint32_t st, state;
            if (hal_pio_sdh_card_status(&st, &state) == 0 && state == 4u)
                break;                                   /* back in TRAN         */
            pio_sdh_setup_cmd_sm(&s_cfg);
            hal_pio_sdh_send_cmd(12, 0, 48, raw);        /* STOP (R1b)           */
        }
        pio_sdh_setup_cmd_sm(&s_cfg);
    }
    pio_sdh_setup_dat0_rx();
    int brc = dat0_wait_busy_release();
    pio_sdh_setup_cmd_sm(&s_cfg);
    return brc;
}

int hal_pio_sdh_read_blocks(uint8_t *buf, uint32_t lba, uint32_t count)
{
    uint32_t raw[2];
    if (!buf || !count) return -1;
    uint32_t arg = s_access_mode ? lba : (lba * 512u);   /* byte vs sector addr */
    int dma4 = (s_dma_ready && s_bus_width == 4u);
    int dma1 = (s_dma_ready && s_bus_width == 1u);       /* 1-bit now has DMA too */

    if (count == 1u) {                                   /* CMD17 single block  */
        if (hal_pio_sdh_send_cmd(17, arg, 48, raw)) return -1;
        if (sdio_check_r1_crc(raw[0], raw[1])) return -1;
        pio_sdh_setup_dat_sm();
        int rc = (dma4 && s_rauto) ? read_blocks_read_auto(buf, 1u)
               : dma4 ? read_blocks_dma(buf, 1u)
               : dma1 ? read_blocks_dma_1bit(buf, 1u)
               : (s_bus_width == 1u) ? capture_block_1bit(buf)
                                     : capture_block(buf);
        pio_sdh_setup_cmd_sm(&s_cfg);
        return rc;
    }

    /* CMD23 SET_BLOCK_COUNT (best effort): a bounded CMD18 makes the card
     * auto-stop after exactly `count` blocks. Open-ended CMD18 instead keeps the
     * card streaming until CMD12, so when read_blocks_dma stops clocking after
     * `count` the card is abandoned MID-block and only CMD12 rescues it - a race
     * that can progressively wedge marginal cards under sustained read load.
     * Bounded read removes that race entirely. Fall back to open-ended if the
     * card rejects CMD23 (R1 illegal-command bit 22). */
    s_rd_cmd23 = (uint8_t)(hal_pio_sdh_send_cmd(23, count, 48, raw) == 0 &&
                           sdio_check_r1_crc(raw[0], raw[1]) == 0 &&
                           (raw[0] & (1u << 22)) == 0u);

    /* CMD18 READ_MULTIPLE_BLOCK: the card streams consecutive blocks (each with
     * its own start bit + CRC). Bounded (CMD23) => auto-stops after `count`;
     * open-ended => runs until CMD12. Set the DAT SM up once for the stream. */
    if (hal_pio_sdh_send_cmd(18, arg, 48, raw)) return -1;
    if (sdio_check_r1_crc(raw[0], raw[1])) return -1;
    pio_sdh_setup_dat_sm();
    int rc = 0;
    if (dma4 && s_rauto) {
        rc = read_blocks_read_auto(buf, count);
    } else if (dma4) {
        rc = read_blocks_dma(buf, count);
    } else if (dma1) {
        rc = read_blocks_dma_1bit(buf, count);
    } else {
        for (uint32_t i = 0; i < count; i++) {
            rc = (s_bus_width == 1u) ? capture_block_1bit(buf + i * 512u)
                                     : capture_block(buf + i * 512u);
            if (rc) break;
        }
    }
    pio_sdh_setup_cmd_sm(&s_cfg);                        /* back to CMD SM      */
    xfer_stop((int)s_rd_cmd23, rc);
    return rc;
}

/* ---- Block write (P5): CPU-paced 4-bit DATA transmission ------------------ */

/* DATA TX cell (write path): drive DAT3..0 from word bits [3:0] with a fresh
 * `pull` per nibble (`mov pins,osr` - the OUT/autopull shifter is broken on
 * this silicon; the mov-after-pull tap is the CMD-cell-proven path). The
 * nibble is driven at cyc1 (CLK low), one setup cycle before the cyc2 rising
 * edge where the card samples. CPU-paced feeding stalls at `pull` with CLK
 * low for ~us between nibbles, which keeps the host-driven DAT flip far from
 * the next rising edge - the same SI settle law as the read path, satisfied
 * for free (no DMA write path yet, and the write aggressor is the HOST pad,
 * untested at back-to-back rates). */
/* Built at load time with s_wr_gap on the two CLK-low instructions (delay
 * fields, 15 max each), mirroring the read DATA cell. gap=0 = the proven
 * 4-cycle base; a nonzero gap stretches the CLK-low phase to back off the
 * host-driven DAT flip from the sampling rising edge if a DMA-rate write ever
 * shows SSO. See k_datbit_prog for the identical construction. */

/* Point the SM at the DATA lines as OUTPUTS (OUT group = D0..D3).
 * The CMD line is actively driven HIGH for the whole data phase: left on its
 * weak pull-up, the four simultaneously-switching host-driven DAT lines
 * couple enough onto CMD to fake a start bit - the card then decodes a
 * phantom command (CMD13 afterwards shows COM_CRC_ERROR, state back to tran)
 * and silently abandons the write (no CRC token ever). A stiff driven-high
 * CMD closes that path. The card never drives CMD during the data phase, so
 * there is no contention; pio_sdh_setup_cmd_sm releases it again. */
static void pio_sdh_setup_dat_tx_sm(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    pio_sm_set_pins(sm, s_cfg.pin_d0, 4, s_cfg.pin_d0, 4, s_cfg.pin_clk, 1);
    pio_sm_set_shift(sm, false, false, 32, false, true, 1);
    pio_sm_set_clkdiv(sm, s_clkdiv_int, s_clkdiv_frac);
    uint16_t g1 = (s_wr_gap > 15u) ? 15u : s_wr_gap;
    uint16_t g0 = (s_wr_gap > 15u) ? (uint16_t)(s_wr_gap - 15u) : 0u;
    uint16_t cell[4] = {
        (uint16_t)(0x80A0 | (g0 << 8)), /* pull block   side 0 [g0] CLK low    */
        (uint16_t)(0xA007 | (g1 << 8)), /* mov pins,osr side 0 [g1] CLK low    */
        0xB042,                         /* nop          side 1  CLK high       */
        0xB042,                         /* nop          side 1  CLK high       */
    };
    pio_sdh_load_cell(sm, cell, 4);
    pio_sm_set_set_base(sm, s_cfg.pin_cmd, 1);
    pio_sm_exec(sm, 0xE001);                             /* set pins,1: CMD=1 */
    pio_sm_set_pindirs(sm, s_cfg.pin_cmd, 1, 1);         /* drive CMD high    */
    pio_sm_set_pindirs(sm, s_cfg.pin_d0, 4, 0xFu);       /* drive DAT0-3      */
    pio_sm_exec(sm, 0xA0C3);                             /* mov isr, null     */
}

/* Release DAT0-3 and switch to the sample-only 1-bit cell on DAT0 for the
 * CRC status token and the card's programming-busy signal. */
static void pio_sdh_setup_dat0_rx(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    pio_sm_set_pindirs(sm, s_cfg.pin_d0, 4, 0x0u);       /* release DAT0-3    */
    pio_sm_set_pins(sm, s_cfg.pin_cmd, 1, s_cfg.pin_d0, 4, s_cfg.pin_clk, 1);
    pio_sm_set_shift(sm, false, false, 32, false, true, 1);
    pio_sm_set_clkdiv(sm, s_clkdiv_int, s_clkdiv_frac);
    pio_sdh_load_cell(sm, k_rxbit_prog, ARRAY_LEN(k_rxbit_prog));
    pio_sm_exec(sm, 0xA0C3);                             /* mov isr, null     */
    /* Flush stale RX words (CMD-response overrun) - they would be read as
     * bogus CRC-status token bits (the same lesson as the read-path scan). */
    while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);
}

/* Feed one nibble word into the TX FIFO (bounded wait for space). */
static int dat_feed(uint32_t nib)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t guard = 0;
    while (pio_txlevel(sm) >= PIO_FIFO_DEPTH) {
        if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
    }
    pio_sm_put(sm, nib);
    return 0;
}

/* Clock one cycle and sample DAT0 through the sample-only cell. */
static int dat0_clock_bit(uint32_t *bit)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t guard = 0;
    pio_sm_put(sm, 0xFu);
    while (pio_rxlevel(sm) == 0u) {
        if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
    }
    *bit = pio_sm_get(sm) & 0x1u;
    return 0;
}

/* Wait out the card's busy on DAT0 (requires the dat0 rx cell): busy appears
 * within ~2 clocks of the trigger (CRC token end / CMD12 R1b) - wait for it
 * FIRST (exiting on a pre-busy high sample lets the next transfer collide
 * with the card's busy window), then for its release, then a few trailing
 * clocks so the card fully lets go of DAT. Polls are CPU-paced clocks (~us);
 * spec allows up to 250 ms of busy. */
static int dat0_wait_busy_release(void)
{
    uint32_t bit, guard;
    int busy_seen = 0;
    for (uint32_t k = 0; k < 8u; k++) {
        if (dat0_clock_bit(&bit)) return -1;
        if (bit == 0u) { busy_seen = 1; break; }
    }
    if (busy_seen) {
        guard = 0;
        do {
            if (dat0_clock_bit(&bit)) return -1;
            if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
        } while (bit == 0u);
        s_wprof.busyclks += guard;         /* actual card busy duration (clocks) */
    }
    for (uint32_t k = 0; k < 4u; k++) {
        if (dat0_clock_bit(&bit)) return -1;
    }
    return 0;
}

/* After a data phase, switch to the DAT0 rx cell, read the CRC status token
 * (start bit then 3 bits: 010 accepted, 101 CRC error, 110 write error) and
 * wait out programming busy. Returns 0, -1 timeout, -3 rejected, -4 wr error. */
static int write_token_and_busy(void)
{
    uint64_t tc = SysTimer_GetLoadValue();
    pio_sdh_setup_dat0_rx();
    s_wprof.tb_cell += SysTimer_GetLoadValue() - tc;
    uint32_t bit, st = 0, scan = 0;
    for (;;) {
        if (dat0_clock_bit(&bit)) return -1;
        if (bit == 0u) break;
        if (++scan > 100u) return -1;                    /* token never came  */
    }
    for (int i = 0; i < 3; i++) {
        if (dat0_clock_bit(&bit)) return -1;
        st = (st << 1) | bit;
    }
    if (dat0_wait_busy_release()) return -1;
    if (st != 0x2u) return (st == 0x5u) ? -3 : -4;
    return 0;
}

/* Expand a 512-byte block into the nibble-per-word DMA feed buffer: 4 N_WR
 * idle-high + start(0) + 1024 data nibbles (MSB first) + 16 CRC16 nibbles +
 * end(0xF). Each word's low nibble is what `mov pins,osr` drives onto DAT3..0. */
static void expand_block_wr(const uint8_t *buf, uint32_t *dst)
{
    uint64_t crc = block_crc16(buf);       /* 16 wire nibbles, MSB first */
    uint32_t k = 0;
    for (int i = 0; i < 4; i++) dst[k++] = 0xFu;         /* N_WR idle-high    */
    dst[k++] = 0x0u;                                      /* start bit         */
    for (uint32_t i = 0; i < 512u; i++) {
        dst[k++] = (uint32_t)buf[i] >> 4;
        dst[k++] = (uint32_t)buf[i] & 0xFu;
    }
    for (uint32_t j = 0; j < 16u; j++)                   /* CRC nibble, MSB 1st */
        dst[k++] = (uint32_t)(crc >> (60u - 4u * j)) & 0xFu;
    dst[k++] = 0xFu;                                      /* end bit           */
    dst[k++] = 0xFu;                                      /* idle pad (burst)  */
    dst[k++] = 0xFu;                                      /* idle pad (burst)  */
}

/* ---- W3: SM-autonomous write (data feed + token + busy on the SM) ---------- */

/* Expand a block for the autonomous program, PACKED 8 wire nibbles per 32-bit
 * feed word (the SM drives them with `out pins,4` + autopull(32), RIGHT shift so
 * the first wire nibble is bits [3:0]). Layout (SDH_AUTO_WORDS = 136 words):
 *   [0]      = jmp-x-- preload (SDH_AUTO_DRIVE_NIBS - 1)
 *   [1..4]   = 31 idle-high + start bit (32 nibbles, word-aligned prefix)
 *   [5..132] = 1024 data nibbles = 128 words, one per 4 source bytes
 *   [133,134]= 16 CRC nibbles
 *   [135]    = end bit (only nibble 0 driven)
 *
 * The data path is the throughput lever. For each 4-byte-aligned source word
 * b0..b3 the wire order is b0.hi,b0.lo,b1.hi,b1.lo,... ; under RIGHT-shift
 * `out pins,4` that means feed byte-lane L must hold nibbleswap(bL). A single
 * SWAR op swaps the two nibbles in ALL four byte lanes at once, so one 32-bit
 * load + one op + one store emits 8 wire nibbles - vs the old 4 byte loads + 8
 * nibble stores. Expand goes from ~86 us (store+load bound) to ~26 us. */
static void expand_block_wr_auto(const uint8_t *buf, uint32_t *dst)
{
    uint64_t crc = block_crc16(buf);       /* 16 wire nibbles, MSB first */
    const uint32_t *bw = (const uint32_t *)(const void *)buf;  /* 4-byte aligned */

    dst[0] = SDH_AUTO_DRIVE_NIBS - 1u;                   /* X: jmp x-- runs NIBS  */
    dst[1] = 0xFFFFFFFFu;                                /* 8 idle-high           */
    dst[2] = 0xFFFFFFFFu;                                /* 8 idle-high           */
    dst[3] = 0xFFFFFFFFu;                                /* 8 idle-high           */
    dst[4] = 0x0FFFFFFFu;                                /* 7 idle + start (n31=0)*/
    for (uint32_t i = 0; i < 128u; i++) {                /* 512 data bytes -> 128 */
        uint32_t w = bw[i];
        dst[5u + i] = ((w << 4) & 0xF0F0F0F0u) | ((w >> 4) & 0x0F0F0F0Fu);
    }
    /* CRC: nibble j (MSB first) -> feed word bits [4*(j%8)] (RIGHT-shift order). */
    uint32_t c0 = 0u, c1 = 0u;
    for (uint32_t j = 0; j < 8u; j++) {
        c0 |= ((uint32_t)(crc >> (60u - 4u * j)) & 0xFu) << (4u * j);
        c1 |= ((uint32_t)(crc >> (28u - 4u * j)) & 0xFu) << (4u * j);
    }
    dst[133] = c0;                                       /* CRC nibbles 0..7      */
    dst[134] = c1;                                       /* CRC nibbles 8..15     */
    dst[135] = 0xFu;                                     /* end bit (nibble 0)    */
}

/* Build + load the autonomous write program at CMD_CLK_ORIGIN and leave DAT0-3
 * driven (data phase). side 0 = CLK low, side 1 = CLK high (SIDESET_COUNT=1).
 * The program: pull count -> X; counted data loop (drive 1 nibble/iter); release
 * DAT; hunt the token start bit and read 3 status bits (4-cyc rx-cell timing,
 * sample at CLK-high); push status; wait busy assert then release (clocked
 * jmp-pin loops on DAT0); push done sentinel; halt. */
static void pio_sdh_setup_dat_auto_sm(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint8_t d0 = s_cfg.pin_d0;
    uint16_t g1 = (s_wr_gap > 15u) ? 15u : s_wr_gap;
    uint16_t g0 = (s_wr_gap > 15u) ? (uint16_t)(s_wr_gap - 15u) : 0u;
    const uint16_t prog[] = {
        0x6020,                          /* 0  out x,32     side0   X = nibs-1 (autopull) */
        (uint16_t)(0x6004 | (g0 << 8)),  /* 1  out pins,4   side0[g0] drive nibble (autopull) */
        (uint16_t)(0xA042 | (g1 << 8)),  /* 2  nop          side0[g1] CLK low stretch  */
        0xB042,                          /* 3  nop          side1   CLK high (edge)     */
        0x1041,                          /* 4  jmp x-- 1    side1   loop / fall          */
        0xE080,                          /* 5  set pindirs,0 side0  DAT -> input          */
        0xA042,                          /* 6  nop          side0                          */
        0xA042,                          /* 7  nop          side0                          */
        0xB042,                          /* 8  nop          side1   CLK high               */
        0x10C6,                          /* 9  jmp pin 6    side1   hunt token start       */
        0xA042, 0xA042, 0xB042, 0x5001,  /* 10-13 status A (4-cyc, in pins,1 side1)        */
        0xA042, 0xA042, 0xB042, 0x5001,  /* 14-17 status B                                */
        0xA042, 0xA042, 0xB042, 0x5001,  /* 18-21 status C                                */
        0x9020,                          /* 22 push         side1   3-bit status           */
        0xB042,                          /* 23 nop          side1   (CLK toggles: the       */
        0x00D7,                          /* 24 jmp pin 23   side0    card needs clock to     */
        0xB042,                          /* 25 nop          side1    release busy)           */
        0x00DC,                          /* 26 jmp pin 28   side0   busy released(high)?    */
        0x1019,                          /* 27 jmp 25       side1   loop (keep clocking)    */
        0x9020,                          /* 28 push         side1   done sentinel            */
        0x101D,                          /* 29 jmp 29       side1   halt                     */
    };
    pio_sm_set_pins(sm, d0, 4, d0, 4, s_cfg.pin_clk, 1); /* OUT/IN base=d0, SIDESET=clk */
    pio_sm_set_shift(sm, true, true, 32, false, false, 32); /* out RIGHT + autopull(32) */
    pio_sm_set_clkdiv(sm, s_clkdiv_int, s_clkdiv_frac);
    pio_load_program(prog, ARRAY_LEN(prog), CMD_CLK_ORIGIN);
    pio_sm_set_wrap(sm, CMD_CLK_ORIGIN + 29, CMD_CLK_ORIGIN, false); /* linear (own jmps+halt) */
    pio_sm_set_jmp_pin(sm, d0);                          /* AFTER set_wrap (it clobbers)*/
    /* SSO guard: drive CMD high through the data phase (host DAT flips couple a
     * phantom start bit onto the weak-pull-up CMD otherwise). */
    pio_sm_set_set_base(sm, s_cfg.pin_cmd, 1);
    pio_sm_exec(sm, 0xE001);                             /* set pins,1 (CMD=1)         */
    pio_sm_set_pindirs(sm, s_cfg.pin_cmd, 1, 1);         /* drive CMD high             */
    pio_sm_set_set_base(sm, d0, 4);                      /* `set pindirs` acts on DAT   */
    pio_sm_set_pindirs(sm, d0, 4, 0xFu);                /* drive DAT0-3               */
    pio_sm_exec(sm, 0xA0C3);                             /* mov isr, null              */
    pio_sm_set_fifo_thr(sm, 4u, 0u);                     /* INCR4 burst gate (once)    */
}

/* Arm the autonomous program for the block in s_wexp[idx] and start the DMA
 * feed. The program + all config are loaded ONCE by pio_sdh_setup_dat_auto_sm()
 * before the loop, so per-block arm is only: re-drive DAT0-3 (the previous
 * block's `set pindirs,0` released them), restart, redirect-after-enable
 * (silicon rule), and re-point the DMA source. Returns with the feed IN FLIGHT
 * so the CPU can expand the next block. */
static void arm_auto(uint32_t idx)
{
    uint8_t sm = s_cfg.sm_cmd;
    pio_sm_set_pindirs(sm, s_cfg.pin_d0, 4, 0xFu);       /* re-drive DAT0-3     */
    pio_sm_restart(sm);
    while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);
    pio_sm_enable(sm, true);
    pio_sm_exec(sm, (uint16_t)(0x0000u | CMD_CLK_ORIGIN)); /* PC=0 while running */
    DMA_Channel_Update_SrcMemcfg(DMA0_ID, s_cfg.dma_tx_ch,
                                 (uint32_t)(uintptr_t)s_wexp[idx], SDH_AUTO_WORDS);
    dma_channel_start(DMA0_ID, s_cfg.dma_tx_ch);
    pio_sm_dma_enable(sm, true, false);                  /* TX gate only        */
}

/* Wait for the autonomous program to push the status word then the done
 * sentinel (block fully written + programmed). Returns 0 / -1 timeout and the
 * 3-bit CRC status in *status (0x2 = accepted). */
static int wait_auto_done(uint32_t *status)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t guard = 8000000u;
    while (pio_rxlevel(sm) < 2u && --guard) { }
    pio_sm_dma_enable(sm, false, false);                 /* close before CPU FIFO */
    if (!guard) {
        pio_sm_enable(sm, false);
        dma_channel_stop(DMA0_ID, s_cfg.dma_tx_ch);
        return -1;
    }
    uint32_t st = pio_sm_get(sm) & 0x7u;                 /* CRC status token    */
    (void)pio_sm_get(sm);                                /* done sentinel       */
    while (pio_rxlevel(sm) > 0u) (void)pio_sm_get(sm);
    pio_sm_enable(sm, false);
    *status = st;
    return 0;
}

/* SM-autonomous multi-block write: the SM does feed + token + busy for block i
 * while the CPU expands block i+1 into the other ping-pong buffer. Because the
 * ~87 us CPU expand exceeds the ~46 us SD phase, throughput becomes expand-
 * bound (vs the old CPU-active token/busy that serialised ~42 us per block). */
static int write_blocks_auto(const uint8_t *buf, uint32_t count)
{
    uint64_t t0 = SysTimer_GetLoadValue();
    expand_block_wr_auto(buf, s_wexp[0]);
    s_wprof.expand += SysTimer_GetLoadValue() - t0;

    /* Load the program + all config ONCE; arm_auto() reuses it per block (the
     * imem + PINCTRL/EXECCTRL/SHIFTCTRL survive restart/enable), so per-block
     * overhead drops to a pindir re-drive + restart + redirect + DMA re-point. */
    pio_sdh_setup_dat_auto_sm();

    int rc = 0;
    for (uint32_t i = 0; i < count; i++) {
        uint64_t ta = SysTimer_GetLoadValue();
        arm_auto(i & 1u);
        uint64_t te = SysTimer_GetLoadValue();
        if (i + 1u < count) {                            /* overlap next expand */
            expand_block_wr_auto(buf + (i + 1u) * 512u, s_wexp[(i + 1u) & 1u]);
            s_wprof.expand += SysTimer_GetLoadValue() - te;
        }
        uint32_t st = 0;
        uint64_t tw = SysTimer_GetLoadValue();
        rc = wait_auto_done(&st);
        s_wprof.tokenbusy += SysTimer_GetLoadValue() - tw;
        s_wprof.feed += te - ta;
        s_wprof.blocks++;
        s_auto_status = st;
        if (rc) break;
        if (st != 0x2u) { rc = (st == 0x5u) ? -3 : -4; break; }
    }
    /* wait_auto_done() left the SM DISABLED; the rest of the driver expects the
     * CMD SM running (the normal flow never disables it), so the next command
     * would run on a dead SM. Restore it to the enabled CMD state here, then
     * clock the bus so the card fully returns to TRAN before the next command
     * (the autonomous path is much faster than the CPU path, so the next CMD
     * can otherwise arrive before the card is ready - the first command after
     * a multi-block autonomous write was seen to fail). */
    pio_sdh_setup_cmd_sm(&s_cfg);
    pio_sm_restart(s_cfg.sm_cmd);
    pio_sm_enable(s_cfg.sm_cmd, true);
    hal_pio_sdh_clock_burst(80);
    return rc;
}

/* Arm the TX DMA feed for a block already expanded into s_wexp[idx]. Returns
 * with the transfer IN FLIGHT (gate open); the CPU is then free to expand the
 * next block into the other ping-pong buffer while this one clocks out. The
 * cell is byte-identical to the CPU path (mov-after-pull, OUT quirk untouched);
 * only the feed source changes. Gate scoping mirrors the read path: the TX
 * req-ack gate opens only once the channel is armed. */
static void write_feed_arm(uint32_t idx)
{
    uint8_t sm = s_cfg.sm_cmd;
    pio_sdh_setup_dat_tx_sm();
    /* Gate the TX DREQ to >=4 free FIFO entries so the INCR4 burst is granted
     * atomically (pio_sm_set_shift in setup zeroed this). FIFO is 8 deep. */
    pio_sm_set_fifo_thr(sm, 4u, 0u);
    DMA_Channel_Update_SrcMemcfg(DMA0_ID, s_cfg.dma_tx_ch,
                                 (uint32_t)(uintptr_t)s_wexp[idx], SDH_WR_WORDS);
    dma_channel_start(DMA0_ID, s_cfg.dma_tx_ch);
    pio_sm_dma_enable(sm, true, false);                  /* TX gate only      */
}

/* Wait out the armed feed and drain the FIFO (closing the gate before any CPU
 * FIFO traffic). Does NOT read the token - the caller does that after, so the
 * expand of the next block can overlap this wait. */
static int write_feed_wait(void)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint32_t guard = 8000000u;
    while (dma_channel_check_busy(DMA0_ID, s_cfg.dma_tx_ch) && --guard) { }
    pio_sm_dma_enable(sm, false, false);                 /* close before CPU  */
    if (!guard) { dma_channel_stop(DMA0_ID, s_cfg.dma_tx_ch); return -1; }

    /* Drain: the tail nibbles may still be clocking out after the transfer
     * count hits zero. */
    guard = 0;
    while (pio_txlevel(sm) > 0u) {
        if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
    }
    for (volatile int d = 0; d < 64; d++) { }
    return 0;
}

/* DMA-fed write of `count` blocks with expand/feed ping-pong: expand block i+1
 * into the idle buffer while the DMA feeds block i, hiding the CPU expand under
 * the wire. Caller has already issued CMD24/25 and (multi-block) sends CMD12 +
 * final busy after. Breaks the ~1 MB/s CPU-per-nibble MMIO ceiling. */
static int write_blocks_dma(const uint8_t *buf, uint32_t count)
{
    uint64_t t0 = SysTimer_GetLoadValue();
    expand_block_wr(buf, s_wexp[0]);
    s_wprof.expand += SysTimer_GetLoadValue() - t0;

    int rc = 0;
    for (uint32_t i = 0; i < count; i++) {
        uint64_t tf = SysTimer_GetLoadValue();
        write_feed_arm(i & 1u);
        uint64_t ta = SysTimer_GetLoadValue();
        if (i + 1u < count) {                            /* overlap next expand */
            expand_block_wr(buf + (i + 1u) * 512u, s_wexp[(i + 1u) & 1u]);
            s_wprof.expand += SysTimer_GetLoadValue() - ta;
        }
        uint64_t tw = SysTimer_GetLoadValue();
        rc = write_feed_wait();
        uint64_t tk = SysTimer_GetLoadValue();
        s_wprof.feed += (ta - tf) + (tk - tw);           /* arm + wait/drain    */
        if (rc) break;
        rc = write_token_and_busy();
        s_wprof.tokenbusy += SysTimer_GetLoadValue() - tk;
        s_wprof.blocks++;
        if (rc) break;
    }
    return rc;
}

/* One block's write data phase: start nibble + 1024 data nibbles + 16 CRC16
 * nibbles + end nibble, then read the card's CRC status token on DAT0 and
 * wait out programming busy. Returns 0, -1 timeout, -3 card rejected CRC,
 * -4 card write error. */
static int write_block_data(const uint8_t *buf)
{
    uint8_t sm = s_cfg.sm_cmd;
    uint64_t crc = block_crc16(buf);      /* 16 wire nibbles, MSB first */

    pio_sdh_setup_dat_tx_sm();
    /* N_WR: >= 2 clocks between the command's R1 end bit and the data start
     * bit (the response reader is strictly zero-overrun now, so these must be
     * provided explicitly). Driven-high nibbles = bus idle. */
    for (int k = 0; k < 4; k++) {
        if (dat_feed(0xFu)) return -1;
    }
    if (dat_feed(0x0u)) return -1;                       /* start nibble      */
    for (uint32_t i = 0; i < 512u; i++) {
        if (dat_feed((uint32_t)buf[i] >> 4)) return -1;
        if (dat_feed((uint32_t)buf[i] & 0xFu)) return -1;
    }
    for (uint32_t j = 0; j < 16u; j++) {                 /* CRC nibble, MSB 1st */
        if (dat_feed((uint32_t)(crc >> (60u - 4u * j)) & 0xFu)) return -1;
    }
    if (dat_feed(0xFu)) return -1;                       /* end nibble        */

    /* Drain: FIFO empty, then a beat for the SM to clock the last nibble out
     * before the bus is released. */
    uint32_t guard = 0;
    while (pio_txlevel(sm) > 0u) {
        if (++guard > SDH_CMD_TIMEOUT_SPINS) return -1;
    }
    for (volatile int d = 0; d < 64; d++) { }

    return write_token_and_busy();
}

int hal_pio_sdh_write_blocks(const uint8_t *buf, uint32_t lba, uint32_t count)
{
    uint32_t raw[2];
    if (!buf || !count) return -1;
    if (s_bus_width != 4u) return -1;                    /* 4-bit only        */
    uint32_t arg = s_access_mode ? lba : (lba * 512u);
    int dma = (s_dma_ready != 0);                        /* DMA-fed data phase */
    if (dma) dma_tx_ensure_mode(1);                      /* TX src-incrementing */

    if (count == 1u) {                                   /* CMD24 single      */
        if (hal_pio_sdh_send_cmd(24, arg, 48, raw)) return -1;
        if (sdio_check_r1_crc(raw[0], raw[1])) return -1;
        int rc = dma ? ((s_wauto && s_dma_ready) ? write_blocks_auto(buf, 1u)
                                                 : write_blocks_dma(buf, 1u))
                     : write_block_data(buf);
        /* The autonomous SM busy-loop exits on release but has no trailing
         * settle clocks, so the card can still be finishing when the next
         * command goes out (a read right after a CMD24 write then fails). The
         * CPU path settles inside write_token_and_busy; give the autonomous
         * path the same via a bounded busy poll (no busy left = 8+4 clocks). */
        if (s_wauto && s_dma_ready) {
            pio_sdh_setup_dat0_rx();
            (void)dat0_wait_busy_release();
        }
        pio_sdh_setup_cmd_sm(&s_cfg);
        /* The DAT0 sniff can miss the busy window entirely (8-clock sample) and
         * report done while the card is still programming - the next command
         * then collides. Confirm on CMD13 that the card is really back in TRAN
         * before returning. See wait_ready_tran(). */
        s_wr1_total++;
        if (s_wr1_settle && !rc && wait_ready_tran(500u)) rc = -1;
        return rc;
    }

    /* CMD23 SET_BLOCK_COUNT (best effort): a bounded CMD25 lets the card
     * pipeline its programming instead of a per-block flush (the measured
     * ~52us/block busy). If the card rejects it (R1 illegal-command bit 22),
     * fall back to open-ended CMD25 + CMD12. */
    /* Autonomous path: force open-ended CMD25 + explicit CMD12 stop. The
     * CMD23-bounded auto-stop appears to leave the card in a state where the
     * next chunk's CMD25 is not answered (consecutive-write failure); an
     * explicit CMD12 gives a clean TRAN transition between chunks. */
    int use_cmd23 = !s_wauto &&
                    (hal_pio_sdh_send_cmd(23, count, 48, raw) == 0 &&
                     sdio_check_r1_crc(raw[0], raw[1]) == 0 &&
                     (raw[0] & (1u << 22)) == 0u);

    /* CMD25 WRITE_MULTIPLE_BLOCK: per-block start/CRC/token/busy. The DMA path
     * pipelines expand vs feed internally. */
    if (hal_pio_sdh_send_cmd(25, arg, 48, raw)) return -1;
    if (sdio_check_r1_crc(raw[0], raw[1])) return -1;
    int rc = 0;
    if (dma) {
        rc = (s_wauto && s_dma_ready) ? write_blocks_auto(buf, count)
                                      : write_blocks_dma(buf, count);
    } else {
        for (uint32_t i = 0; i < count; i++) {
            rc = write_block_data(buf + i * 512u);
            if (rc) break;
        }
    }
    pio_sdh_setup_cmd_sm(&s_cfg);
    /* Stop + programming-busy wait, shared with the read path. CMD12 fires for
     * an open-ended write, or when a CMD23-bounded write aborted and the card is
     * still stranded mid-write (a stray STOP-in-TRAN on a last-block reject is
     * avoided by the CMD13 state check - that stray STOP is what hung a wgap=0
     * format and wedged the card). */
    int brc = xfer_stop((int)use_cmd23, rc);
    /* The SM-autonomous path already waits out each block's programming busy
     * (including the last), so this final poll is redundant; a timeout on it
     * (card already idle) must not fail an otherwise-good write. */
    if (s_wauto) brc = 0;
    return rc ? rc : brc;
}

void hal_pio_sdh_deinit(void)
{
    pio_sm_enable(s_cfg.sm_cmd, false);
}

uint32_t hal_pio_sdh_reg_read(uint32_t off)
{
    return *(volatile uint32_t *)(uintptr_t)(PIO_BASE + off);
}

/* Read-path phase profiling (R2). Returns per-phase averages in nanoseconds
 * (SysTimer = 4 ticks/us -> 250 ns/tick) plus the sampled block count, then
 * clears the accumulators so each call reports since the previous one. Pass
 * NULL for any output not wanted. */
void hal_pio_sdh_prof_get(uint32_t *scan_ns, uint32_t *arm_ns, uint32_t *wait_ns,
                          uint32_t *fin_ns, uint32_t *blocks)
{
    uint32_t n = s_prof.blocks ? s_prof.blocks : 1u;
    if (scan_ns) *scan_ns = (uint32_t)(s_prof.scan     * 1000u / SOC_TIMER_TICK_PER_US / n);
    if (arm_ns)  *arm_ns  = (uint32_t)(s_prof.arm      * 1000u / SOC_TIMER_TICK_PER_US / n);
    if (wait_ns) *wait_ns = (uint32_t)(s_prof.wait     * 1000u / SOC_TIMER_TICK_PER_US / n);
    if (fin_ns)  *fin_ns  = (uint32_t)(s_prof.finalize * 1000u / SOC_TIMER_TICK_PER_US / n);
    if (blocks)  *blocks  = s_prof.blocks;
    s_prof.scan = s_prof.arm = s_prof.wait = s_prof.finalize = 0;
    s_prof.blocks = 0;
}

/* Write-path phase profiling (W1). Per-block averages in ns (expand / feed /
 * tokenbusy) since the previous call, then clears. NULL any unwanted output. */
void hal_pio_sdh_wprof_get(uint32_t *exp_ns, uint32_t *feed_ns, uint32_t *tb_ns,
                           uint32_t *cell_ns, uint32_t *busyclks, uint32_t *blocks)
{
    uint32_t n = s_wprof.blocks ? s_wprof.blocks : 1u;
    if (exp_ns)   *exp_ns   = (uint32_t)(s_wprof.expand    * 1000u / SOC_TIMER_TICK_PER_US / n);
    if (feed_ns)  *feed_ns  = (uint32_t)(s_wprof.feed      * 1000u / SOC_TIMER_TICK_PER_US / n);
    if (tb_ns)    *tb_ns    = (uint32_t)(s_wprof.tokenbusy * 1000u / SOC_TIMER_TICK_PER_US / n);
    if (cell_ns)  *cell_ns  = (uint32_t)(s_wprof.tb_cell   * 1000u / SOC_TIMER_TICK_PER_US / n);
    if (busyclks) *busyclks = s_wprof.busyclks / n;
    if (blocks)   *blocks   = s_wprof.blocks;
    s_wprof.expand = s_wprof.feed = s_wprof.tokenbusy = s_wprof.tb_cell = 0;
    s_wprof.busyclks = 0;
    s_wprof.blocks = 0;
}

