/*
 * PIO-driven 3-wire 9-bit SPI for EL036MF1. See epd_pio.h for why.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "epd_pio.h"

#include <stdio.h>
#include <string.h>

#include "compiler/common.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_pio.h"
#include "glb_reg.h"
#include "hal_dma.h"
#include "drv_dma.h"

/* --- Program -------------------------------------------------------------
 *
 *   0: pull block                 wait for a frame -- CSB is still HIGH here
 *   1: set  pins, 0    side 0     CSB low, SCL low          -- frame opens
 *   2: set  x, 8       side 0     9 bits to send
 *   3: out  pins, 1    side 0     present a bit, SCL low
 *   4: jmp  x--, 3     side 1     SCL high -- the panel samples here
 *   5: set  pins, 1    side 0     CSB high                  -- frame closes
 *   (wrap 5 -> 0)
 *
 * The blocking pull has to come before CSB is asserted, not after. Opening the
 * frame first and letting autopull stall mid-word leaves the state machine
 * parked at `out` with CSB already low -- which is where it sits for as long as
 * the FIFO stays empty, i.e. after every command and at the end of every
 * transfer. The panel then sees CSB permanently asserted and the per-byte
 * framing it needs is gone. Observed on a logic analyser as CSB dropping once
 * and never coming back up.
 *
 * Autopull is therefore off; `pull` overwrites the OSR wholesale, so the 23
 * unused bits of the previous word are discarded for free.
 *
 * `jmp x--` tests x before decrementing, so x = 8 runs the body nine times:
 * one D/C bit plus eight data bits.
 *
 * Instruction encoding is the standard PIO layout, the same one hal_pio_cci.c
 * hand-assembles: [15:13] opcode, [12] side-set (one side-set pin, SIDE_EN
 * off), [11:8] delay, [7:5] destination, [4:0] operand. PULL is 0x8080 with
 * bit5 = block.
 *
 * Cost is 22 cycles per byte. CSB stays high for the pull cycle plus however
 * long the FIFO makes it wait -- at 8 MHz that floor is 125 ns against the
 * 40 ns the panel asks for (Application Note Table 3, tCHW).
 */
#define OP_PULL_BLOCK 0x80A0u                 /* pull block         */
#define OP_SET_PINS_0 0xE000u                 /* set pins,0  side 0 */
/* set x,N-1 -- the bit-loop counter. 9 bits for 3-wire (D/C travels in the
 * frame), 8 for 4-wire (D/C is a separate line held across the transfer). */
#define OP_SET_X(n) (0xE000u | (1u << 5) | ((n) - 1u))
#define OP_OUT_PINS_1 (0x6000u | 1u)          /* out pins,1  side 0 */
#define OP_JMP_XDEC_3                                                          \
  (0x0000u | (1u << 12) | (2u << 5) | 3u) /* jmp x--,3 side 1 */
#define OP_SET_PINS_1 (0xE000u | 1u)      /* set pins,1  side 0 */

#define PROG_LEN 6u
#define PROG_ORIGIN 0u
#define PROG_WRAP_TOP 5u

/* pull + CSB low + counter + 2 per bit + CSB high */
#define CYCLES_PER_BYTE(n) (4u + 2u * (n))

/* One frame per FIFO word, in the top nine bits: `out` shifts left, MSB first.
 * Packing frames back to back would be 3.5x denser but would need all 120000
 * bytes bit-packed up front -- 135 KB, which this no-PSRAM build cannot spare.
 * At 4 MHz the state machine consumes a word every ~2.75 us; 1.5 MB/s of DMA
 * traffic is not worth optimising away. */
/* The frame sits in the top s_bits of the word: `out` shifts left, MSB first. */
static uint8_t s_bits = 9;
#define FRAME_SHIFT (32u - s_bits)

#define PIO_SM 0u
#define PIO_BASE_ADDR 0x3001D000u
#define PIO_TXF(sm) (PIO_BASE_ADDR + 0x10u + (uint32_t)(sm) * 4u)
#define PIO_TX_REQ(sm) (20u + (uint32_t)(sm))

#define DMA_CH 0u

#define PIO_SRC_HZ 96000000u /* hal_pio_cci.h and hal_pio_sdh.c agree */

/* DMA reads this directly, so it must not sit behind the data cache. */
#define CHUNK_WORDS 1024u
static ATTR_NOCACHE_RAM_SECTION uint32_t s_chunk[CHUNK_WORDS];

static bool s_ready;
static uint32_t s_actual_hz;
static uint16_t s_div_int;
static uint8_t s_div_frac;
static uint8_t s_scl, s_sda, s_csb;

/* The PIO block comes up held in reset with its clock gated. Without this the
 * state machine never advances and the pads, once muxed to PIO, simply sit at
 * zero -- on a logic analyser that looks like CSB being asserted and never
 * released, not like a dead peripheral. Same sequence as
 * hal_pio_cmsis_dap.c's hal_dap_glb_clock().
 *
 * sel = 0, div = 0 is the 96 MHz source every in-tree PIO user picks
 * (hal_pio_sdh.c derives its 24 MHz bit clock from it). */
static inline uint32_t glb_rd(uint32_t off) {
  return *(volatile uint32_t *)(uintptr_t)(GLB_BASE + off);
}

static inline void glb_wr(uint32_t off, uint32_t v) {
  *(volatile uint32_t *)(uintptr_t)(GLB_BASE + off) = v;
}

static void pio_block_clock_on(void) {
  uint32_t v;
  v = glb_rd(GLB_SWRST_CFG1_OFFSET);
  glb_wr(GLB_SWRST_CFG1_OFFSET, v | GLB_SWRST_PIO_MSK);
  v = glb_rd(GLB_SWRST_CFG1_OFFSET);
  glb_wr(GLB_SWRST_CFG1_OFFSET, v & ~GLB_SWRST_PIO_MSK);

  v = glb_rd(GLB_CGEN_CFG0_OFFSET);
  glb_wr(GLB_CGEN_CFG0_OFFSET, v | GLB_CGEN_PIO_MSK);

  v = glb_rd(GLB_CLK_CFG6_OFFSET);
  v &= ~(GLB_PIO_CLK_SEL_MSK | GLB_PIO_CLK_DIV_MSK);
  v |= GLB_PIO_CLK_EN_MSK;
  glb_wr(GLB_CLK_CFG6_OFFSET, v);
}

static void pin_to(uint8_t pin, uint32_t fun) {
  GLB_GPIO_Cfg_Type c = {
      .gpioPin = pin,
      .gpioFun = fun,
      .gpioMode = GPIO_MODE_OUTPUT,
      .pullType = GPIO_PULL_NONE,
      .drive = 1,
      .smtCtrl = 1,
  };
  GLB_GPIO_Init(&c);
}

static void dma_setup(void) {
  dma_device_t tx = {
      .id = DMA0_ID,
      .ch = DMA_CH,
      .direction = DMA_MEMORY_TO_PERIPH,
      .src_req = DMA_REQUEST_NONE,
      .dst_req = PIO_TX_REQ(PIO_SM),
      .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
      .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
      /* Paired with the SM's TX DREQ threshold below: the request gates on
       * four free slots, so the burst must move exactly four words. This is
       * the combination hal_pio_sdh.c uses -- the only other PIO+DMA user in
       * the tree, and the one that works. */
      .src_burst_size = DMA_BURST_INCR4,
      .dst_burst_size = DMA_BURST_INCR4,
      .src_width = DMA_TRANSFER_WIDTH_32BIT,
      .dst_width = DMA_TRANSFER_WIDTH_32BIT,
      .transfer_mode = DMA_LLI_ONCE_MODE,
      .lli_cfg = NULL,
      .callback = NULL,
  };
  GLB_Set_DMA_CLK(1, (GLB_DMA_CLK_ID_Type)(GLB_DMA_CLK_DMA0_CH0 + DMA_CH));
  dma_setConfig(&tx);
  dma_init(DMA0_ID, DMA_CH);
  /* TX DREQ threshold. Left at its reset value of 0 the request never fires
   * and the channel stays armed forever -- the transfer simply never starts,
   * which reads as a DMA timeout with no other symptom. pio_sm_set_shift()
   * zeroes this field, so it has to be set after that call, not before. */
  pio_sm_set_fifo_thr(PIO_SM, 4u, 0u);
  pio_sm_dma_enable(PIO_SM, true, false);
}

int epd_pio_init(uint8_t scl, uint8_t sda, uint8_t csb, uint32_t hz,
                 uint8_t bits) {
  if (bits != 8 && bits != 9) {
    return -1;
  }
  s_bits = bits;

  if (hz == 0u) {
    return -1;
  }

  s_scl = scl;
  s_sda = sda;
  s_csb = csb;

  pio_block_clock_on();

  const uint16_t prog[PROG_LEN] = {
      OP_PULL_BLOCK, OP_SET_PINS_0, OP_SET_X(s_bits),
      OP_OUT_PINS_1, OP_JMP_XDEC_3, OP_SET_PINS_1,
  };

  pio_sm_enable(PIO_SM, false);
  pio_load_program(prog, PROG_LEN, PROG_ORIGIN);

  /* Two instructions per bit, so the divider targets twice the SCL rate.
   * 64-bit intermediate: PIO_SRC_HZ * 256 is 2.46e10 and wraps a uint32_t,
   * which silently yields a divider of ~1.5 instead of 12. */
  uint32_t div256 =
      (uint32_t)(((uint64_t)PIO_SRC_HZ * 256u) / (2u * (uint64_t)hz));
  if (div256 < 256u) {
    div256 = 256u; /* divider of 1 is the floor */
  }
  uint16_t intg = (uint16_t)(div256 >> 8);
  uint8_t frac = (uint8_t)(div256 & 0xFFu);
  s_div_int = intg;
  s_div_frac = frac;
  s_actual_hz =
      (uint32_t)(((uint64_t)PIO_SRC_HZ * 256u) / (2u * (uint64_t)div256));

  /* out = SDA (1 pin), no input group, side-set = SCL (1 pin). */
  pio_sm_set_pins(PIO_SM, sda, 1, 0, 0, scl, 1);
  /* SET group is CSB alone. RMW, so it must follow pio_sm_set_pins. */
  pio_sm_set_set_base(PIO_SM, csb, 1);
  /* Shift left => MSB first, which is what the panel expects. Autopull off:
   * the program pulls explicitly so that it blocks with CSB high rather than
   * with CSB low. Nothing is read back. */
  pio_sm_set_shift(PIO_SM, false, false, 32, false, false, 32);
  pio_sm_set_clkdiv(PIO_SM, intg, frac);
  pio_sm_set_wrap(PIO_SM, PROG_WRAP_TOP, PROG_ORIGIN, false);

  /* A PIO pad only drives once its pindir says output, regardless of pinmux. */
  pio_sm_set_pindirs(PIO_SM, scl, 1, 1u);
  pio_sm_set_pindirs(PIO_SM, sda, 1, 1u);
  pio_sm_set_pindirs(PIO_SM, csb, 1, 1u);

  pin_to(scl, GPIO_FUN_PIO);
  pin_to(sda, GPIO_FUN_PIO);
  pin_to(csb, GPIO_FUN_PIO);

  pio_sm_restart(PIO_SM);
  /* Park CSB high before the first frame; until instruction 5 has run once
   * the pad would otherwise come up at whatever the reset value is. */
  pio_sm_exec(PIO_SM, OP_SET_PINS_1);
  pio_sm_enable(PIO_SM, true);
  dma_setup();

  s_ready = true;
  return 0;
}

void epd_pio_deinit(void) {
  if (!s_ready) {
    return;
  }
  epd_pio_drain();
  pio_sm_enable(PIO_SM, false);
  /* Back to plain GPIO so the bit-bang path can drive these again. */
  pin_to(s_scl, GPIO_FUN_GPIO);
  pin_to(s_sda, GPIO_FUN_GPIO);
  pin_to(s_csb, GPIO_FUN_GPIO);
  s_ready = false;
}

/* Someone else has taken the pads back (gpio_setup() re-muxes all six to plain
 * GPIO). Stop the state machine and forget the claim so the next transfer
 * re-arms; do NOT touch the pins here, they already belong to the caller. */
void epd_pio_release_claim(void) {
  if (!s_ready) {
    return;
  }
  pio_sm_enable(PIO_SM, false);
  s_ready = false;
}

bool epd_pio_ready(void) { return s_ready; }
uint32_t epd_pio_actual_hz(void) { return s_actual_hz; }

void epd_pio_divider(uint16_t *intg, uint8_t *frac) {
  if (intg)
    *intg = s_div_int;
  if (frac)
    *frac = s_div_frac;
}

/* Bulk data is always D/C = 1. In 3-wire that flag is the frame's top bit; in
 * 4-wire the frame is just the byte and the caller holds the DC line high. */
static inline uint32_t data_frame(uint8_t val) {
  return (s_bits == 9) ? (0x100u | (uint32_t)val) : (uint32_t)val;
}

void epd_pio_write_byte(uint8_t val, bool is_data) {
  if (!s_ready) {
    return;
  }
  /* 4-wire puts D/C on its own pin, which the caller drives before getting
   * here; only the 9-bit frame carries it as data. */
  uint32_t frame = (s_bits == 9)
                       ? ((is_data ? 0x100u : 0x000u) | (uint32_t)val)
                       : (uint32_t)val;
  while (pio_txlevel(PIO_SM) >= PIO_FIFO_DEPTH) {
  }
  pio_sm_put(PIO_SM, frame << FRAME_SHIFT);
}

/* Feed the FIFO from the CPU, one frame per FIFO slot.
 *
 * Deliberately does NOT read s_chunk. That buffer lives in .nocache_ram because
 * DMA has to see it, and every uncached word the CPU reads back costs far more
 * than the pio_sm_put() itself -- with the DMA path unavailable there is no
 * reason to pay it. Callers hand over either a constant or their own (cached)
 * buffer instead.
 *
 * The FIFO level is read once per batch rather than once per word; a word costs
 * the state machine 22 cycles (2.75 us at 4 MHz), so per-word polling would
 * spend most of the budget on the poll.
 */
#define FIFO_BATCH(GET_WORD)                                                   \
  do {                                                                         \
    uint32_t i = 0;                                                            \
    while (i < n) {                                                            \
      uint32_t level = pio_txlevel(PIO_SM);                                    \
      if (level >= PIO_FIFO_DEPTH) {                                           \
        continue;                                                              \
      }                                                                        \
      uint32_t room = PIO_FIFO_DEPTH - level;                                  \
      if (room > n - i) {                                                      \
        room = n - i;                                                          \
      }                                                                        \
      while (room--) {                                                         \
        pio_sm_put(PIO_SM, (GET_WORD));                                        \
        i++;                                                                   \
      }                                                                        \
    }                                                                          \
  } while (0)

static void feed_const(uint32_t word, uint32_t n) { FIFO_BATCH(word); }

static void feed_bytes(const uint8_t *buf, uint32_t n) {
  FIFO_BATCH(data_frame(buf[i]) << FRAME_SHIFT);
}

/* Push one prepared chunk by DMA. Returns false if it did not complete.
 *
 * A full chunk is 1024 frames at ~2.75 us each, so ~2.8 ms. The guard is set an
 * order of magnitude above that: long enough that a working transfer never
 * trips it, short enough that a broken one reports instead of appearing to
 * hang. The earlier 40M-iteration guard took long enough per chunk that a
 * failing fill looked indistinguishable from a lock-up. */
static bool send_chunk_dma(uint32_t words) {
  /* INCR4 bursts move four words at a time; a length that is not a multiple of
   * four cannot be expressed. Every size this driver actually uses happens to
   * qualify (1024 per chunk, 120000 and 20000 both leaving multiple-of-four
   * remainders), but a future change to CHUNK_WORDS or the frame size would
   * otherwise break DMA silently. */
  if (words % 4u) {
    return false;
  }

  /* Program the channel registers directly instead of going through
   * dma_reload(). That helper builds an LLI descriptor chain with realloc(),
   * so the descriptors live in the ordinary cached heap -- and the DMA engine
   * fetches them itself. The CPU's writes can still be sitting in cache when
   * it does, leaving the channel armed from a stale descriptor and never
   * completing, which is exactly the "armed but busy never clears" symptom.
   * hal_pio_sdh.c, the one PIO+DMA user in the tree that works, takes this
   * same direct route. Length is in transfers, not bytes. */
  DMA_Channel_Update_SrcMemcfg(DMA0_ID, DMA_CH, (uint32_t)(uintptr_t)s_chunk,
                               words);
  DMA_Channel_Update_DstMemcfg(DMA0_ID, DMA_CH, PIO_TXF(PIO_SM), words);
  DMA_Channel_Enable(DMA0_ID, DMA_CH);

  /* A chunk is 1024 frames at ~2.75 us, so ~2.8 ms of wire time. This guard is
   * about 20 ms -- comfortably above that, and small enough that the one probe
   * made per boot is not noticeable. */
  uint32_t guard = 100000u;
  while (dma_channel_check_busy(DMA0_ID, DMA_CH) && --guard) {
  }
  return guard != 0u;
}

/* Once DMA has failed there is no reason to keep retrying it every chunk. */
static bool s_dma_broken;

static bool dma_usable(void) { return !s_dma_broken; }

static void note_dma_failure(void) {
  s_dma_broken = true;
  printf("[epd_pio] DMA did not complete - falling back to CPU-fed FIFO\r\n");
}

bool epd_pio_dma_ok(void) { return !s_dma_broken; }

void epd_pio_write_data(const uint8_t *buf, uint32_t n) {
  if (!s_ready || buf == NULL) {
    return;
  }
  while (n) {
    uint32_t k = (n > CHUNK_WORDS) ? CHUNK_WORDS : n;
    if (dma_usable()) {
      for (uint32_t i = 0; i < k; i++) {
        s_chunk[i] = data_frame(buf[i]) << FRAME_SHIFT;
      }
      if (!send_chunk_dma(k)) {
        note_dma_failure();
        feed_bytes(buf, k);
      }
    } else {
      feed_bytes(buf, k);
    }
    buf += k;
    n -= k;
  }
}

void epd_pio_fill(uint8_t val, uint32_t n) {
  if (!s_ready) {
    return;
  }
  const uint32_t w = data_frame(val) << FRAME_SHIFT;

  if (!dma_usable()) {
    feed_const(w, n); /* no buffer, no uncached reads, no chunking */
    return;
  }

  for (uint32_t i = 0; i < CHUNK_WORDS; i++) {
    s_chunk[i] = w; /* constant, so fill once and replay */
  }
  while (n) {
    uint32_t k = (n > CHUNK_WORDS) ? CHUNK_WORDS : n;
    if (!send_chunk_dma(k)) {
      note_dma_failure();
      feed_const(w, n); /* finish the rest on the CPU */
      return;
    }
    n -= k;
  }
}

void epd_pio_drain(void) {
  if (!s_ready) {
    return;
  }
  uint32_t guard = 40000000u;
  while (pio_txlevel(PIO_SM) && --guard) {
  }
  /* The FIFO being empty only means the last word was taken, not clocked.
   * 21 cycles at the divided rate covers the frame still in flight. */
  for (volatile uint32_t i = 0; i < 2000u; i++) {
  }
}
