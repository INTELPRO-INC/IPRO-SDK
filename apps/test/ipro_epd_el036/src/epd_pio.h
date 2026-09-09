/*
 * 3-wire 9-bit SPI for EL036MF1, driven by the IPRO7 PIO.
 *
 * The panel's native mode needs a 9-bit frame (a D/C flag ahead of each byte)
 * and CSB toggled once per byte. The SPI controller can do neither: its frame
 * sizes are 8/16/24/32 and its chip-select is per-transfer. Bit-banging can, but
 * costs the CPU ~2.5 us a byte and caps out around 450 kHz.
 *
 * The PIO does both in a five-instruction program, at a clock set by a divider
 * rather than by loop timing, and its FIFO can be fed by DMA -- so a full-frame
 * write costs the CPU nothing per byte.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EPD_PIO_H
#define EPD_PIO_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Claim the pins and start the state machine. Returns 0 on success.
 *
 * scl/sda/csb must be plain GPIO numbers; unlike the SPI controller the PIO
 * places no modulo-4 constraint on them. csb has to sit within 5 pins of itself
 * (it is the only member of the SET group), which is always true.
 *
 * hz is the SCL rate. The panel allows 20 MHz for writes and 6.67 MHz for
 * reads; E Ink recommends 5 MHz in practice.
 *
 * bits is the frame width: 9 for 3-wire (the D/C flag rides in the frame) or
 * 8 for 4-wire (D/C is a separate line the caller drives). Anything else is
 * rejected.
 */
int  epd_pio_init(uint8_t scl, uint8_t sda, uint8_t csb, uint32_t hz,
                  uint8_t bits);

/* Hand the pins back so the bit-bang path can drive them again. */
void epd_pio_deinit(void);

bool epd_pio_ready(void);

/* Drop the claim on the pads without driving them -- for when something else
 * has already re-muxed them (see gpio_setup). The next transfer re-arms. */
void epd_pio_release_claim(void);

/* One 9-bit frame. is_data selects the D/C flag. Blocks until the FIFO accepts
 * it, not until it has been clocked out -- use epd_pio_drain() for that. */
void epd_pio_write_byte(uint8_t val, bool is_data);

/* n data bytes, DMA-fed. Every byte carries D/C = 1. */
void epd_pio_write_data(const uint8_t *buf, uint32_t n);

/* n copies of one data byte, DMA-fed, with no caller-side frame buffer. */
void epd_pio_fill(uint8_t val, uint32_t n);

/* Wait for the state machine to finish what it has been given. */
void epd_pio_drain(void);

/* Actual SCL rate after the divider truncates, in Hz. 0 before init. */
uint32_t epd_pio_actual_hz(void);

/* CLKDIV as programmed, so a measured rate can be checked against the assumed
 * 96 MHz source rather than guessed at. */
void epd_pio_divider(uint16_t *intg, uint8_t *frac);

/* False once a DMA chunk has failed to complete and the CPU-fed path took over.
 * The wire timing is the same either way; only throughput and CPU load differ. */
bool epd_pio_dma_ok(void);

#endif /* EPD_PIO_H */
