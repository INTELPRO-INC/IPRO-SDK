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
 * ATBM6162 (Altobeam OCEANUS) Wi-Fi 6 — public bring-up API for IPRO7.
 *
 * This is the ONLY header the application should include. It exposes no vendor
 * types, so the driver's unprefixed typedefs (u8, min, bool ...) stay contained
 * inside the component. See .claude/plans/atbm6162-integration.md.
 */
#ifndef ATBM6162_H
#define ATBM6162_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Phase-0/1 chip bring-up over SDIO.
 *
 * Runs the vendor register flow: SDIO host init -> card enumerate -> wlan-core
 * probe (firmware download + chip handshake). The board layer must have set up
 * the SDH pinmux and sequenced ATBM power/reset before calling this.
 *
 * @return 0 on success, <0 on failure (e.g. no card, enumerate/handshake error).
 */
int atbm6162_bringup(void);

/* ---- Phase 2: STA mode ----------------------------------------------------- */

/** Bring the interface up in STA mode (call after atbm6162_bringup()). 0 = ok. */
int atbm6162_sta_start(void);

/** Active scan; fills `buf` (size bytes) with the vendor-formatted AP list. */
int atbm6162_scan(char *buf, unsigned size);

/** Join an OPEN (unencrypted) AP by SSID. 0 = join request accepted. */
int atbm6162_connect_open(const char *ssid);

/** Join a WPA2-PSK (AES) AP by SSID + passphrase. 0 = join request accepted. */
int atbm6162_connect_wpa2(const char *ssid, const char *pass);

/** 1 = associated to an AP, 0 = not. */
int atbm6162_is_connected(void);

/** Enable(1)/disable(0) WiFi power-save. Off = low latency/reliable, higher power. 0 = ok. */
int atbm6162_set_powersave(int enable);

/** Set the SDH 4-bit read-capture phase taps (sweep tool); re-run bring-up to apply.
 *  inv=0/1 (clock-edge invert), clkdly=0..3 (capture clock delay), datdly=0..3 (DAT delay). */
void atbm6162_set_rdphase(int inv, int clkdly, int datdly);

/** Enable(1)/disable(0) the OCRAM bounce buffer for small SDIO transfers (default on).
 *  A/B test for the SDH<->PSRAM 4-bit DataTimeout. Re-run bring-up to apply. */
void atbm6162_set_bounce(int enable);

/** Enable(1)/disable(0) routing small register transfers via CMD52 (no data phase).
 *  Bypasses the short-CMD53 4-bit DataTimeout. Re-run bring-up to apply. */
void atbm6162_set_cmd52(int enable);

/** Enable(1)/disable(0) routing small CMD53 transfers via PIO instead of ADMA.
 *  Tests whether tiny ADMA descriptors are what fail to engage at 4-bit. Re-run bring-up. */
void atbm6162_set_pio(int enable);

/** Enable(1)/disable(0) SDH High-Speed read sampling. DS(0) removes the HS half-clock
 *  sample shift that can make short 4-bit blocks fail CRC. Re-run bring-up to apply. */
void atbm6162_set_hs(int enable);

/** Set the 4-bit bus clock in Hz (default 24000000). Lower = wider read sample margin
 *  for a marginal short-block CRC. Re-run bring-up to apply. */
void atbm6162_set_clk(unsigned hz);

/** Enable(1)/disable(0) doing small register transfers at 1-bit while bulk data stays
 *  4-bit. Sidesteps the post-download short-4-bit-read CRC. Re-run bring-up to apply. */
void atbm6162_set_rem1bit(int enable);

/** Enable(1)/disable(0) the delayed sampling clock (HOST_CTRL_2 bit7) so the read-capture
 *  taps actually shift the read sample point. Re-run bring-up to apply. */
void atbm6162_set_samp(int enable);

/** Enable(1)/disable(0) CRC diagnostic: on a 4-bit read CRC error, re-read at 1-bit and
 *  compare — distinguishes real data corruption (SI) from a false CRC flag. */
void atbm6162_set_crcdiag(int enable);

/** Enable(1)/disable(0) accepting read data despite a (false) SDH DATA_CRC flag.
 *  Workaround for the 4-bit extra-nibble data-phase bug. Re-run bring-up to apply. */
void atbm6162_set_ignorecrc(int enable);

/** Enable(1)/disable(0) all reads at 1-bit (clean), writes stay 4-bit — thread-safe under
 *  the SDIO lock. The robust way to run RX/registers reliably with 4-bit TX. Re-run bring-up. */
void atbm6162_set_reads1bit(int enable);

/** A/B: clear BLK_CNT_EN for single-block transfers (4-bit read CRC test). */
void atbm6162_set_noblkcnt(int enable);

/** Enable(1)/disable(0) native-4-bit read-phase auto-calibration (io_dly tap sweep on
 *  first CRC-fail; requires reads-1bit OFF). Re-run bring-up to apply. */
void atbm6162_set_rdcal(int enable);

/** Enable(1)/disable(0) read-twice-verify for register reads (also enables ignore-CRC).
 *  Rejects the rare one-nibble slip so 4-bit register reads are reliable. */
void atbm6162_set_rdverify(int enable);

/** Per-direction sampling phase split for native full 4-bit: READ transfers sample at
 *  {inv,clkdly,datdly}+delayed-clock, WRITE transfers at the proven default phase —
 *  switched per transfer inside the SDIO lock. Forces reads-1bit OFF. en=0 restores
 *  the legacy global phase. Survives re-probe. */
void atbm6162_set_phasesplit(int enable, int inv, int clkdly, int datdly);

/** Per-direction phase calibration sweep: for each candidate read-phase, power-cycle the
 *  chip (via the app-provided callback), re-init the host, enumerate, and measure reads
 *  (ground-truth compare) and writes (scratch write + 1-bit readback verify) in isolation.
 *  Prints a matrix + recommendation. k = iterations per candidate (0 = default 20).
 *  Run BEFORE atbm_probe; power-cycles the chip, so re-probe afterwards. Returns 0. */
int atbm6162_physweep(unsigned k, void (*chip_power_cycle)(void));

/** Native 4-bit READ stress test: hammer AHB reg `addr` (0=default 0xab0016c) `n` times
 *  (0=default 1000) at the current width/clock/tap and report the DATA_CRC failure rate +
 *  value mismatches. Call after atbm6162_bringup(); set reads-1bit OFF to measure native 4-bit. */
void atbm6162_rdtest(unsigned n, unsigned addr);

/** Fill `buf` with the DHCP-assigned IPv4 address string. 0 = ok, <0 = no IP yet. */
int atbm6162_get_ip(char *buf, unsigned size);

#ifdef __cplusplus
}
#endif

#endif /* ATBM6162_H */
