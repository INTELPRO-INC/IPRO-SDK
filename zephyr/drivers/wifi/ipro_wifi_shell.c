/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * `wifi_ipro` shell: a peek at the MAC's rate-control state, useful when
 * the link is slower than the RSSI suggests.
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <stdlib.h>
#include <string.h>
#include "export_macsw.h"
#include "wifi_mgmr_ext.h"
#include "net_al_zephyr.h"
#include <zephyr/net/net_pkt.h>
#include <zephyr/net/net_if.h>
#include "drv_clock.h"
#ifdef CONFIG_WIFI_IPRO_TCP_TRACE
#include "ipro_wifi_tcptrace.h"
#endif
#include <zephyr/sys/sys_heap.h>
#include "hal_flash.h"
/* SoC header first, or core_feature_cache.h makes the cache helpers no-ops */
#include "ipro_soc_regs.h"
#include "core_feature_cache.h"

static const char *const format_str[] = {"NON-HT", "NON-HT-DUP", "HT-MF", "HT-GF",
					 "VHT", "HE-SU", "HE-MU", "HE-ER", "HE-TB"};

static void print_rate(const struct shell *sh, uint32_t cfg)
{
	uint8_t ft = export_rc_get_format_mod(cfg);

	shell_fprintf(sh, SHELL_NORMAL, "%-10s mcs %2u nss %u bw %u gi %u",
		      ft < ARRAY_SIZE(format_str) ? format_str[ft] : "?",
		      export_rc_get_mcs_index(cfg), export_rc_get_nss(cfg) + 1,
		      export_rc_get_bw(cfg), export_rc_get_sgi(cfg));
}

static int cmd_rc(const struct shell *sh, size_t argc, char **argv)
{
	struct macsw_rc_snapshot snap;
	uint16_t sta = argc > 1 ? (uint16_t)strtoul(argv[1], NULL, 0) : 0;

	if (macsw_rc_snapshot_get(sta, &snap) != 0) {
		shell_error(sh, "sta %u: no rate control state", sta);
		return -ENOENT;
	}
	shell_print(sh, "sta %u peer %02x:%02x:%02x:%02x:%02x:%02x mode %u format %u bw_max %u",
		    sta, snap.peer_addr[0], snap.peer_addr[1], snap.peer_addr[2],
		    snap.peer_addr[3], snap.peer_addr[4], snap.peer_addr[5], snap.mode,
		    snap.format_mod, snap.bw_max);
	shell_print(sh, "HW policy table (in use now):");
	for (int i = 0; i < MACSW_RC_STEPS; i++) {
		shell_fprintf(sh, SHELL_NORMAL, "  step%d %08x ", i, snap.hw_step[i]);
		print_rate(sh, snap.hw_step[i]);
		shell_print(sh, " retries %u", (snap.hw_step[i] >> 29) & 0x7);
	}
	shell_print(sh, " #  rate                                   prob   ok(  tot) skipped");
	for (int i = 0; i < snap.no_samples; i++) {
		unsigned int eprob = ((snap.sample[i].probability * 1000) >> 16) + 1;

		shell_fprintf(sh, SHELL_NORMAL, "%2d  ", i);
		print_rate(sh, snap.sample[i].rate_config);
		shell_print(sh, "  %c%c%c %3u.%u %5u(%5u) %u",
			    snap.retry_step_idx[0] == i ? 'T' : ' ',
			    snap.retry_step_idx[1] == i ? 't' : ' ',
			    snap.retry_step_idx[2] == i ? 'P' : ' ', eprob / 10, eprob % 10,
			    snap.sample[i].success, snap.sample[i].attempts,
			    snap.sample[i].sample_skipped);
	}
	shell_print(sh, "MPDUs %u AMPDUs %u avg len %u.%u amsdu %u", snap.ampdu_len,
		    snap.ampdu_packets, (unsigned int)(snap.avg_ampdu_len >> 16),
		    (unsigned int)(((snap.avg_ampdu_len * 10) >> 16) % 10), snap.curr_amsdu_len);
	return 0;
}

static int cmd_txtrace(const struct shell *sh, size_t argc, char **argv)
{
	extern void fhost_tx_trace_print(void);
	extern void fhost_tx_trace_reset(void);
	extern volatile uint8_t g_txprof_en;

	ARG_UNUSED(sh);
	if (argc > 1) {
		g_txprof_en = 1;
		fhost_tx_trace_reset();
	} else {
		fhost_tx_trace_print();
	}
	return 0;
}

/* The SDK's own per-frame TX profilers (fhost -> macif -> txu -> txl -> cfm). */
static int cmd_txprof(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint8_t g_txprof_en, g_lmac_prof_en;
	extern void fhost_tx_trace_print(void), fhost_tx_trace_reset(void);
	extern void macif_tx_evt_prof_print(void), macif_tx_evt_prof_reset(void);
	extern void txu_cntrl_push_prof_print(void), txu_cntrl_push_prof_reset(void);
	extern void txl_push_prof_print(void), txl_push_prof_reset(void);
	extern void txl_cfm_prof_print(void), txl_cfm_prof_reset(void);

	ARG_UNUSED(sh);
	if (argc > 1) {
		g_txprof_en = 1;
		g_lmac_prof_en = 1;
		fhost_tx_trace_reset();
		macif_tx_evt_prof_reset();
		txu_cntrl_push_prof_reset();
		txl_push_prof_reset();
		txl_cfm_prof_reset();
		return 0;
	}
	fhost_tx_trace_print();
	macif_tx_evt_prof_print();
	txu_cntrl_push_prof_print();
	txl_push_prof_print();
	txl_cfm_prof_print();
	return 0;
}

/* A-MPDU close policy: how long an open aggregate waits for more subframes. */
/* Flat TX power for every rate (bench A/B against AP-side saturation). */
static int cmd_txpwr(const struct shell *sh, size_t argc, char **argv)
{
	rf_pwr_table_t t;

	if (wifi_mgmr_tpc_pwr_get(&t) != 0) {
		shell_error(sh, "tpc get failed");
		return -EIO;
	}
	if (argc > 1) {
		int8_t dbm = (int8_t)strtol(argv[1], NULL, 0);

		memset(&t, dbm, sizeof(t));
		if (wifi_mgmr_tpc_pwr_set(&t) != 0) {
			shell_error(sh, "tpc set failed");
			return -EIO;
		}
	}
	shell_print(sh, "11b %d 11g %d ht20 %d he20[0] %d he20[7] %d dBm", t.pwr_11b[0],
		    t.pwr_11g[0], t.pwr_11n_ht20[0], t.pwr_11ax_he20[0], t.pwr_11ax_he20[7]);
	return 0;
}

/* RX AGC ladder hold-off (beacons between gain-state transitions). */
static int cmd_pdg(const struct shell *sh, size_t argc, char **argv)
{
	extern uint8_t bba_pd_dwell_bcn;
	extern uint32_t bba_pd_held;

	if (argc > 1) {
		bba_pd_dwell_bcn = (uint8_t)strtoul(argv[1], NULL, 0);
	}
	shell_print(sh, "pdg dwell %u beacons, held %u", bba_pd_dwell_bcn, bba_pd_held);
	return 0;
}

/* MAC HW error / self-reset counters (hal_machw.c, dbg_assert.c, txl_cntrl.c). */
static int cmd_macstat(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint32_t macsw_hw_err_total, macsw_hw_err_rx_fifo_overflow,
		macsw_hw_err_mac_phyif_overflow, macsw_hw_err_mac_phyif_under_run,
		macsw_hw_err_phy_err, macsw_hw_err_rx_hdr_dma_dead, macsw_hw_err_rx_pld_dma_dead,
		pm48_phyif_und_reset_fired, macsw_mac_reset_armed;

	ARG_UNUSED(argc); ARG_UNUSED(argv);
	shell_print(sh, "hw_err total %u rxfifo_ovf %u phyif_ovf %u phyif_und %u phy_err %u "
		    "rxhdr_dma_dead %u rxpld_dma_dead %u", macsw_hw_err_total,
		    macsw_hw_err_rx_fifo_overflow, macsw_hw_err_mac_phyif_overflow,
		    macsw_hw_err_mac_phyif_under_run, macsw_hw_err_phy_err,
		    macsw_hw_err_rx_hdr_dma_dead, macsw_hw_err_rx_pld_dma_dead);
	{
		extern uint32_t ipro_wifi_cfm_watchdog_resets;

		extern uint32_t ipro_zif_rx_ok, ipro_zif_rx_nopkt, ipro_zif_rx_err;

		shell_print(sh, "driver cfm-watchdog resets %u", ipro_wifi_cfm_watchdog_resets);
		shell_print(sh, "driver rx: to stack %u, no net_pkt %u, stack refused %u", ipro_zif_rx_ok,
			    ipro_zif_rx_nopkt, ipro_zif_rx_err);
	}
	shell_print(sh, "resets: pm48 underrun-storm %u, assert-armed %u, pm45 no-advance BE %u "
		    "BK %u VI %u VO %u, backstop BE %u", pm48_phyif_und_reset_fired,
		    macsw_mac_reset_armed, fhost_statistic.pm45_assert_rec_fired[1],
		    fhost_statistic.pm45_assert_rec_fired[0], fhost_statistic.pm45_assert_rec_fired[2],
		    fhost_statistic.pm45_assert_rec_fired[3], fhost_statistic.timer_backstop_arm[1]);
	return 0;
}

/* The micro-benchmark keeps 2 KB of code and 3 KB of buffers in RAM: bench
 * builds only (CONFIG_WIFI_IPRO_SHELL_BENCH). */
#ifdef CONFIG_WIFI_IPRO_SHELL_BENCH
static uint64_t ipro_zprof_cyc(void)
{
	k_thread_runtime_stats_t st;

	k_thread_runtime_stats_get(k_current_get(), &st);
	return st.execution_cycles;
}


/* ---- CPU / code-fetch micro-benchmark ------------------------------------ */
static inline uint32_t bench_cyc(void)
{
	uint32_t c;

	__asm__ volatile("csrr %0, mcycle" : "=r"(c));
	return c;
}

#define COLD_FN(pfx, n) \
	__attribute__((noinline)) uint32_t pfx##_f##n(uint32_t x) \
	{ x ^= x << 13; x ^= x >> 17; x ^= x << 5; x += n; x ^= x << 13; x ^= x >> 17; \
	  x ^= x << 5; x += n * 3; x ^= x << 13; x ^= x >> 17; x ^= x << 5; return x + n; }
#define COLD_SET(pfx, attr) \
	attr COLD_FN(pfx, 0) attr COLD_FN(pfx, 1) attr COLD_FN(pfx, 2) attr COLD_FN(pfx, 3) \
	attr COLD_FN(pfx, 4) attr COLD_FN(pfx, 5) attr COLD_FN(pfx, 6) attr COLD_FN(pfx, 7) \
	attr COLD_FN(pfx, 8) attr COLD_FN(pfx, 9) attr COLD_FN(pfx, 10) attr COLD_FN(pfx, 11) \
	attr COLD_FN(pfx, 12) attr COLD_FN(pfx, 13) attr COLD_FN(pfx, 14) attr COLD_FN(pfx, 15) \
	attr COLD_FN(pfx, 16) attr COLD_FN(pfx, 17) attr COLD_FN(pfx, 18) attr COLD_FN(pfx, 19) \
	attr COLD_FN(pfx, 20) attr COLD_FN(pfx, 21) attr COLD_FN(pfx, 22) attr COLD_FN(pfx, 23) \
	attr COLD_FN(pfx, 24) attr COLD_FN(pfx, 25) attr COLD_FN(pfx, 26) attr COLD_FN(pfx, 27) \
	attr COLD_FN(pfx, 28) attr COLD_FN(pfx, 29) attr COLD_FN(pfx, 30) attr COLD_FN(pfx, 31)
#define COLD_CALL(pfx, x) \
	(pfx##_f0(x) + pfx##_f1(x) + pfx##_f2(x) + pfx##_f3(x) + pfx##_f4(x) + pfx##_f5(x) + \
	 pfx##_f6(x) + pfx##_f7(x) + pfx##_f8(x) + pfx##_f9(x) + pfx##_f10(x) + pfx##_f11(x) + \
	 pfx##_f12(x) + pfx##_f13(x) + pfx##_f14(x) + pfx##_f15(x) + pfx##_f16(x) + pfx##_f17(x) + \
	 pfx##_f18(x) + pfx##_f19(x) + pfx##_f20(x) + pfx##_f21(x) + pfx##_f22(x) + pfx##_f23(x) + \
	 pfx##_f24(x) + pfx##_f25(x) + pfx##_f26(x) + pfx##_f27(x) + pfx##_f28(x) + pfx##_f29(x) + \
	 pfx##_f30(x) + pfx##_f31(x))

COLD_SET(flashc, )
COLD_SET(fl0, )
COLD_SET(fl1, )
COLD_SET(fl2, )
COLD_SET(fl3, )
COLD_SET(fl4, )
COLD_SET(fl5, )
COLD_SET(fl6, )
COLD_SET(fl7, )
COLD_SET(ramc, __attribute__((section(".tcm_code.bench"))))

static __attribute__((noinline)) uint32_t hot_loop(uint32_t x, int n)
{
	for (int i = 0; i < n; i++) {
		x ^= x << 13; x ^= x >> 17; x ^= x << 5;
	}
	return x;
}
static __attribute__((noinline, section(".tcm_code.bench"))) uint32_t hot_loop_ram(uint32_t x, int n)
{
	for (int i = 0; i < n; i++) {
		x ^= x << 13; x ^= x >> 17; x ^= x << 5;
	}
	return x;
}

static int cmd_cpubench(const struct shell *sh, size_t argc, char **argv)
{
	volatile uint32_t sink = 1;
	uint32_t c0, c1;
	static uint8_t big[1536];

	ARG_UNUSED(argc); ARG_UNUSED(argv);
	c0 = bench_cyc(); sink = hot_loop(sink, 100000); c1 = bench_cyc();
	shell_print(sh, "hot loop flash : %u cycles (expect ~500k)", c1 - c0);
	c0 = bench_cyc(); sink = hot_loop_ram(sink, 100000); c1 = bench_cyc();
	shell_print(sh, "hot loop ram   : %u cycles", c1 - c0);
	for (int pass = 0; pass < 2; pass++) {
		c0 = bench_cyc();
		for (int i = 0; i < 200; i++) {
			sink = COLD_CALL(flashc, sink);
		}
		c1 = bench_cyc();
		shell_print(sh, "32-fn chain flash x200 (pass %d): %u cycles/call", pass, (c1 - c0) / 6400);
		c0 = bench_cyc();
		for (int i = 0; i < 200; i++) {
			sink = COLD_CALL(ramc, sink);
		}
		c1 = bench_cyc();
		shell_print(sh, "32-fn chain ram   x200 (pass %d): %u cycles/call", pass, (c1 - c0) / 6400);
	}
	for (int pass = 0; pass < 2; pass++) {
		c0 = bench_cyc();
		for (int i = 0; i < 80; i++) {
			sink = COLD_CALL(fl0, sink) + COLD_CALL(fl1, sink);
		}
		c1 = bench_cyc();
		shell_print(sh, "64-fn chain flash (~8 KB) (pass %d): %u cycles/call", pass, (c1 - c0) / 5120);
		c0 = bench_cyc();
		for (int i = 0; i < 40; i++) {
			sink = COLD_CALL(fl0, sink) + COLD_CALL(fl1, sink) + COLD_CALL(fl2, sink) + COLD_CALL(fl3, sink);
		}
		c1 = bench_cyc();
		shell_print(sh, "128-fn chain flash (~16 KB) (pass %d): %u cycles/call", pass, (c1 - c0) / 5120);
	}
	for (int pass = 0; pass < 2; pass++) {
		c0 = bench_cyc();
		for (int i = 0; i < 20; i++) {
			sink = COLD_CALL(fl0, sink) + COLD_CALL(fl1, sink) + COLD_CALL(fl2, sink) + COLD_CALL(fl3, sink) + COLD_CALL(fl4, sink) + COLD_CALL(fl5, sink) + COLD_CALL(fl6, sink) + COLD_CALL(fl7, sink);
		}
		c1 = bench_cyc();
		shell_print(sh, "256-fn chain flash (~32 KB) x20 (pass %d): %u cycles/call", pass, (c1 - c0) / 5120);
	}
	{
		uint8_t *cfg = NULL; uint32_t len = 0;

		shell_print(sh, "flash clock %u Hz, mcu %u Hz", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_FLASH),
			    Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK));
		if (flash_get_cfg(&cfg, &len) == 0 && cfg != NULL) {
			shell_print(sh, "flash cfg (%u B) ioMode 0x%02x clkDelay %u clkInvert 0x%02x cReadSupport %u",
				    len, cfg[0], cfg[2], cfg[3], cfg[19]);
		}
	}
	{
		extern struct k_heap _system_heap;
		struct sys_memory_stats st;

		sys_heap_runtime_stats_get(&_system_heap.heap, &st);
		shell_print(sh, "kernel heap: free %u B, allocated %u B, max %u B", st.free_bytes,
			    st.allocated_bytes, st.max_allocated_bytes);
	}
	c0 = bench_cyc(); memset(big, (int)sink, sizeof(big)); c1 = bench_cyc();
	shell_print(sh, "memset 1.5 KB ocram: %u cycles", c1 - c0);
	c0 = bench_cyc();
	for (int i = 0; i < 100; i++) {
		(void)ipro_zprof_cyc();
	}
	c1 = bench_cyc();
	shell_print(sh, "k_thread_runtime_stats_get: %u cycles each", (c1 - c0) / 100);
	c0 = bench_cyc();
	for (int i = 0; i < 100; i++) {
		unsigned int key = irq_lock(); irq_unlock(key);
	}
	c1 = bench_cyc();
	shell_print(sh, "irq_lock/unlock: %u cycles each", (c1 - c0) / 100);
	{
		struct net_if *iface = net_if_get_first_wifi();
		struct net_pkt *pkt;
		uint32_t a = 0, w = 0, r = 0, u = 0;
		extern uint8_t __wifi_bss_start[];
		static uint8_t src[1470 + 2]; /* + 2: the both+2 copy below */

		for (int i = 0; i < 50; i++) {
			c0 = bench_cyc();
			pkt = net_pkt_alloc_with_buffer(iface, 1470, AF_INET, IPPROTO_UDP, K_NO_WAIT);
			c1 = bench_cyc(); a += c1 - c0;
			if (pkt == NULL) { shell_print(sh, "pkt alloc failed"); break; }
			c0 = bench_cyc(); net_pkt_write(pkt, src, 1470); c1 = bench_cyc(); w += c1 - c0;
			net_pkt_cursor_init(pkt);
			c0 = bench_cyc(); net_pkt_read(pkt, big, 1470); c1 = bench_cyc(); r += c1 - c0;
			c0 = bench_cyc(); net_pkt_unref(pkt); c1 = bench_cyc(); u += c1 - c0;
		}
		shell_print(sh, "net_pkt 1470B: alloc %u, write %u, read %u, unref %u cycles", a / 50, w / 50, r / 50, u / 50);
		c0 = bench_cyc(); memcpy(big, src, 1470); c1 = bench_cyc();
		shell_print(sh, "memcpy 1470B ocram->ocram: %u cycles", c1 - c0);
		c0 = bench_cyc(); memcpy(big + 2, src, 1470); c1 = bench_cyc();
		shell_print(sh, "memcpy 1470B dst+2: %u cycles", c1 - c0);
		c0 = bench_cyc(); memcpy(big + 2, src + 2, 1470); c1 = bench_cyc();
		shell_print(sh, "memcpy 1470B both+2: %u cycles", c1 - c0);
		{
			volatile uint32_t *mis = (volatile uint32_t *)(big + 2);
			uint32_t v;

			c0 = bench_cyc();
			for (int i = 0; i < 300; i++) {
				v = mis[i];
				mis[i] = v + 1;
			}
			c1 = bench_cyc();
			shell_print(sh, "misaligned word load/store x300: %u cycles (HW unaligned ok)", c1 - c0);
		}
		c0 = bench_cyc(); memcpy(__wifi_bss_start + 4096, src, 1470); c1 = bench_cyc();
		shell_print(sh, "memcpy 1470B ocram->wram(uncached): %u cycles", c1 - c0);
		c0 = bench_cyc();
		for (int i = 0; i < 1470; i += 4) {
			*(volatile uint32_t *)(__wifi_bss_start + 4096 + i) = i;
		}
		c1 = bench_cyc();
		shell_print(sh, "word stores 1470B -> wram: %u cycles", c1 - c0);
		memset(big, 1, 1470);
		c0 = bench_cyc(); MFlushDCacheRange((uintptr_t)big, 1470); c1 = bench_cyc();
		shell_print(sh, "dcache flush 1470B (dirty): %u cycles", c1 - c0);
		c0 = bench_cyc(); MFlushDCacheRange((uintptr_t)big, 1470); c1 = bench_cyc();
		shell_print(sh, "dcache flush 1470B (clean): %u cycles", c1 - c0);
	}
	return 0;
}
#else
static int cmd_cpubench(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	shell_print(sh, "build with CONFIG_WIFI_IPRO_SHELL_BENCH=y");
	return -ENOTSUP;
}
#endif /* CONFIG_WIFI_IPRO_SHELL_BENCH */

/* Counters the bench patch in the Zephyr tree bumps (zephyr/bench/); weak
 * zeros otherwise. */
__weak uint32_t ipro_npkt_big_write, ipro_npkt_big_read;

/* Stage marks along zsock_sendto() (bench patch in the Zephyr tree): mcycle
 * deltas; the minimum over many frames is the uninterrupted cost. */
static uint32_t zp_acc[16], zp_min[16], zp_last, zp_n;
void ipro_zp_mark(int i)
{
	uint32_t now;

	__asm__ volatile("csrr %0, mcycle" : "=r"(now));
	if (i > 0) {
		uint32_t d = now - zp_last;

		zp_acc[i] += d;
		if (zp_min[i] == 0 || d < zp_min[i]) {
			zp_min[i] = d;
		}
	} else {
		zp_n++;
	}
	zp_last = now;
}

/* Sender-thread CPU split per frame inside the driver. */
static int cmd_txsplit(const struct shell *sh, size_t argc, char **argv)
{
	struct ipro_tx_prof *p = &ipro_tx_prof;
	uint32_t n = p->n ? p->n : 1;
	uint32_t hz = sys_clock_hw_cycles_per_sec();

	extern volatile uint64_t ipro_wifi_isr_cycles;
	extern volatile uint32_t ipro_wifi_isr_count;

	if (argc > 1) {
		memset(p, 0, sizeof(*p));
		ipro_wifi_isr_cycles = 0;
		ipro_wifi_isr_count = 0;
		{
			extern volatile uint32_t intc_irq_cycles[], intc_irq_count[];

			extern volatile uint32_t intc_resume_cycles, intc_resume_slow;

			intc_resume_cycles = 0;
			intc_resume_slow = 0;
			for (int i = 0; i < 64; i++) {
				intc_irq_cycles[i] = 0;
				intc_irq_count[i] = 0;
			}
		}
		return 0;
	}
	{
		extern volatile uint32_t intc_irq_cycles[], intc_irq_count[];

		for (int i = 0; i < 64; i++) {
			if (intc_irq_count[i]) {
				shell_print(sh, "  irq src %d: %u calls, %u us total, %u us/frame", i,
					    intc_irq_count[i], intc_irq_cycles[i] / 240,
					    intc_irq_cycles[i] / 240 / n);
			}
		}
	}
	{
		extern volatile uint32_t intc_resume_cycles, intc_resume_slow;

		shell_print(sh, "  task resume from ISR: %u us total, %u us/frame, %u calls > 4 us",
			    intc_resume_cycles / 240, intc_resume_cycles / 240 / n, intc_resume_slow);
	}
#ifdef CONFIG_IPRO_WIFI_TX_PROFILE
	{
		struct ipro_memcpy_site { uint32_t ra, cycles, bytes, calls, min, unaligned; };
		extern struct ipro_memcpy_site ipro_memcpy_sites[48];

		for (int i = 0; i < 48; i++) {
			if (ipro_memcpy_sites[i].ra && ipro_memcpy_sites[i].calls > 50) {
				shell_print(sh, "  memcpy from %p: %u calls %u B %u us min %u cyc unaligned %u",
					    (void *)ipro_memcpy_sites[i].ra, ipro_memcpy_sites[i].calls,
					    ipro_memcpy_sites[i].bytes, ipro_memcpy_sites[i].cycles / 240,
					    ipro_memcpy_sites[i].min, ipro_memcpy_sites[i].unaligned);
			}
		}
		memset(ipro_memcpy_sites, 0, sizeof(struct ipro_memcpy_site) * 48);
	}
#endif
	if (zp_n) {
		static const char *const nm[16] = {"", "sock->ctx", "pkt alloc", "udp/ip hdr", "->send_data",
						   "->net_if_tx", "->eth_send", "arp+ethhdr", "driver", "->exit",
						   " drv gate", " drv alloc", " drv copy", " drv fhost", "  fh lock", "  fh req"};
		shell_print(sh, "sendto stages (%u): min / mean us per frame", zp_n);
		for (int i = 1; i < 16; i++) {
			shell_print(sh, "  %-12s %3u.%u / %u", nm[i], zp_min[i] / 240, (zp_min[i] % 240) * 10 / 240,
				    zp_acc[i] / 240 / zp_n);
		}
		memset(zp_acc, 0, sizeof(zp_acc));
		memset(zp_min, 0, sizeof(zp_min));
		zp_n = 0;
	}
	{
		extern uint32_t ipro_tx_zc_hit, ipro_tx_zc_miss, ipro_npkt_big_write, ipro_npkt_big_read;

		extern uint32_t ipro_zc_miss_nofrag, ipro_zc_miss_multi, ipro_zc_frag0_len, ipro_zc_frag1_len, ipro_zc_miss_len;

		shell_print(sh, "zero-copy hit %u miss %u (nofrag %u multi %u frag0 %u frag1 %u lenmismatch 0x%08x); net_pkt big write %u read %u",
			    ipro_tx_zc_hit, ipro_tx_zc_miss, ipro_zc_miss_nofrag, ipro_zc_miss_multi, ipro_zc_frag0_len,
			    ipro_zc_frag1_len, ipro_zc_miss_len, ipro_npkt_big_write, ipro_npkt_big_read);
		ipro_tx_zc_hit = ipro_tx_zc_miss = ipro_npkt_big_write = ipro_npkt_big_read = 0;
	}
	shell_print(sh, "MAC ISR: %u calls, %u us total, %u us per frame",
		    ipro_wifi_isr_count, (uint32_t)(ipro_wifi_isr_cycles / 240),
		    (uint32_t)(ipro_wifi_isr_cycles / 240 / n));
	shell_print(sh, "%u frames: thread-cycles/frame wait %u alloc %u copy %u fhost_tx_start %u "
		    "| blocked %u us/frame", p->n, (uint32_t)(p->wait / n), (uint32_t)(p->alloc / n),
		    (uint32_t)(p->copy / n), (uint32_t)(p->fhost / n),
		    (uint32_t)(p->wait_wall / n * 1000000ULL / hz));
	return 0;
}

static int cmd_wedge(const struct shell *sh, size_t argc, char **argv)
{
	extern void macsw_tx_dma_dead_snapshot_print(void);

	ARG_UNUSED(sh); ARG_UNUSED(argc); ARG_UNUSED(argv);
	macsw_tx_dma_dead_snapshot_print();
	return 0;
}

static int cmd_macreset(const struct shell *sh, size_t argc, char **argv)
{
	extern void dbg_mac_reset_request(void);

	ARG_UNUSED(sh); ARG_UNUSED(argc); ARG_UNUSED(argv);
	dbg_mac_reset_request();
	return 0;
}

static int cmd_txdiag(const struct shell *sh, size_t argc, char **argv)
{
	extern void macsw_txl_diag_print(void), fhost_tx_diag_print(void), macif_diag_print(void);
	extern volatile uint32_t txl_agg_hold_timer_fired, txl_agg_hold_timer_cancel;

	ARG_UNUSED(argc); ARG_UNUSED(argv);
	macsw_txl_diag_print();
	fhost_tx_diag_print();
	macif_diag_print();
	shell_print(sh, "driver inflight %ld / window %d; agg closes timer %u natural %u",
		    (long)ipro_tx_inflight_get(), ipro_tx_window_get(), txl_agg_hold_timer_fired,
		    txl_agg_hold_timer_cancel);
	return 0;
}

/* WiFi-task kernel events: CPU per event handler. */
static int cmd_evtprof(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint32_t ke_evt_prof_cyc[], ke_evt_prof_cnt[];
	extern void *ke_evt_prof_func(int i);

	for (int i = 0; i < 24; i++) {
		if (argc > 1) {
			ke_evt_prof_cyc[i] = 0;
			ke_evt_prof_cnt[i] = 0;
		} else if (ke_evt_prof_cnt[i]) {
			shell_print(sh, "evt %2d: %6u calls %8u us  %u us/call  handler %p", i,
				    ke_evt_prof_cnt[i], ke_evt_prof_cyc[i] / 240,
				    ke_evt_prof_cyc[i] / 240 / ke_evt_prof_cnt[i], ke_evt_prof_func(i));
		}
	}
	return 0;
}

/* Poor man's PC sampler: from the 1 kHz tick (a k_timer callback runs in the
 * timer ISR) print the interrupted PC (mepc). The host bins them by symbol. */
static uint32_t pcs_left;
static void pcs_tick(struct k_timer *t)
{
	uint32_t pc;

	ARG_UNUSED(t);
	__asm__ volatile("csrr %0, mepc" : "=r"(pc));
	printk("P%x\n", pc);
	if (--pcs_left == 0) {
		k_timer_stop(t);
	}
}
K_TIMER_DEFINE(pcs_timer, pcs_tick, NULL);

static int cmd_pcsample(const struct shell *sh, size_t argc, char **argv)
{
	pcs_left = argc > 1 ? strtoul(argv[1], NULL, 0) : 5000;
	shell_print(sh, "sampling %u ticks", pcs_left);
	k_timer_start(&pcs_timer, K_MSEC(1), K_MSEC(1));
	return 0;
}

static int cmd_batch(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint8_t fhost_tx_sched_batch_en;

	if (argc > 1) {
		fhost_tx_sched_batch_en = (uint8_t)strtoul(argv[1], NULL, 0);
	}
	shell_print(sh, "deferred MAC kicks: %u", fhost_tx_sched_batch_en);
	return 0;
}

/* Change a thread's priority by name (bench: WiFi task vs sender ordering). */
struct prio_find { const char *name; k_tid_t tid; };
static void prio_find_cb(const struct k_thread *t, void *user)
{
	struct prio_find *f = user;
	const char *n = k_thread_name_get((k_tid_t)t);

	if (n != NULL && strcmp(n, f->name) == 0) {
		f->tid = (k_tid_t)t;
	}
}
static int cmd_tprio(const struct shell *sh, size_t argc, char **argv)
{
	struct prio_find f = {.name = argv[1], .tid = NULL};

	k_thread_foreach(prio_find_cb, &f);
	if (f.tid == NULL) {
		shell_error(sh, "no thread %s", argv[1]);
		return -ENOENT;
	}
	if (argc > 2) {
		k_thread_priority_set(f.tid, atoi(argv[2]));
	}
	shell_print(sh, "%s priority %d", argv[1], k_thread_priority_get(f.tid));
	return 0;
}

/* fhost A-MSDU batching knobs (SDK globals, default off). */
static int cmd_amsdu(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint8_t fhost_amsdu_enabled, fhost_amsdu_max_batch_run;
	extern volatile uint32_t fhost_amsdu_payl_max, fhost_amsdu_committed, fhost_amsdu_size_hist[];

	if (argc > 1) {
		fhost_amsdu_enabled = (uint8_t)strtoul(argv[1], NULL, 0);
	}
	if (argc > 2) {
		fhost_amsdu_max_batch_run = (uint8_t)strtoul(argv[2], NULL, 0);
	}
	if (argc > 3) {
		fhost_amsdu_payl_max = strtoul(argv[3], NULL, 0);
	}
	shell_print(sh, "amsdu enabled %u max_batch %u payl_max %u committed %u sizes 1:%u 2:%u 3:%u 4:%u",
		    fhost_amsdu_enabled, fhost_amsdu_max_batch_run, fhost_amsdu_payl_max,
		    fhost_amsdu_committed, fhost_amsdu_size_hist[1], fhost_amsdu_size_hist[2],
		    fhost_amsdu_size_hist[3], fhost_amsdu_size_hist[4]);
	return 0;
}

static int cmd_csum(const struct shell *sh, size_t argc, char **argv)
{
	extern uint8_t ipro_wifi_csum_offload;

	if (argc > 1) {
		ipro_wifi_csum_offload = (uint8_t)strtoul(argv[1], NULL, 0);
	}
	shell_print(sh, "UDP TX checksum offload: %u", ipro_wifi_csum_offload);
	return 0;
}

/* MAC A-MPDU builder knobs (SDK globals). */
static int cmd_agg(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint32_t g_ampdu_max_cnt, txl_agg_hold_us, txl_agg_hold_skip_n, txl_agg_pipeline_max,
		txl_agg_hold_min_cnt, txl_agg_hold_only_busy;
	extern volatile uint8_t txl_agg_hold_max_cnt;
	extern volatile uint32_t txl_agg_hold_closed_full, txl_agg_hold_closed_age, txl_agg_hold_timer_fired,
		txl_agg_hold_skipped;

	if (argc > 1) {
		g_ampdu_max_cnt = strtoul(argv[1], NULL, 0);
	}
	if (argc > 2) {
		txl_agg_hold_us = strtoul(argv[2], NULL, 0);
	}
	if (argc > 3) {
		txl_agg_hold_skip_n = strtoul(argv[3], NULL, 0);
	}
	if (argc > 4) {
		txl_agg_pipeline_max = strtoul(argv[4], NULL, 0);
	}
	if (argc > 5) {
		txl_agg_hold_max_cnt = (uint8_t)strtoul(argv[5], NULL, 0);
	}
	if (argc > 6) {
		txl_agg_hold_min_cnt = strtoul(argv[6], NULL, 0);
	}
	if (argc > 7) {
		txl_agg_hold_only_busy = strtoul(argv[7], NULL, 0);
	}
	shell_print(sh, "ampdu_max_cnt %u hold_us %u skip_n %u pipeline %u hold_max_cnt %u min_cnt %u only_busy %u | closes: full %u age %u timer %u skipped %u",
		    g_ampdu_max_cnt, txl_agg_hold_us, txl_agg_hold_skip_n, txl_agg_pipeline_max, txl_agg_hold_max_cnt,
		    txl_agg_hold_min_cnt, txl_agg_hold_only_busy,
		    txl_agg_hold_closed_full, txl_agg_hold_closed_age, txl_agg_hold_timer_fired, txl_agg_hold_skipped);
	return 0;
}

static int cmd_txop(const struct shell *sh, size_t argc, char **argv)
{
	extern uint32_t macsw_ampdu_max_dur_set(uint8_t ac, uint32_t val);
	uint32_t v = argc > 1 ? strtoul(argv[1], NULL, 0) : 0;

	shell_print(sh, "BE A-MPDU max duration: %u x 32 us", macsw_ampdu_max_dur_set(1, v));
	return 0;
}

static int cmd_ps(const struct shell *sh, size_t argc, char **argv)
{
	int on = argc > 1 ? atoi(argv[1]) : -1;
	int r = on == 1 ? wifi_mgmr_sta_ps_enter() : on == 0 ? wifi_mgmr_sta_ps_exit() : 0;

	shell_print(sh, "station power save %s: %d", on == 1 ? "on" : on == 0 ? "off" : "(no change)", r);
	return 0;
}

/* Software-DMA copy variants (dma.c): dmacpy [unlocked 0|1] [cached_src 0|1] */
static int cmd_dmacpy(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint8_t g_dma_push_unlocked, g_dma_push_cached_src;

	if (argc > 1) {
		g_dma_push_unlocked = (uint8_t)strtoul(argv[1], NULL, 0);
	}
	if (argc > 2) {
		g_dma_push_cached_src = (uint8_t)strtoul(argv[2], NULL, 0);
	}
	shell_print(sh, "dma_push copy: unlocked %u cached_src %u", g_dma_push_unlocked,
		    g_dma_push_cached_src);
	return 0;
}

#ifdef CONFIG_TIMESLICING
/* Scheduler time slice in ms (0 = off): every context switch re-arms it. */
static int cmd_slice(const struct shell *sh, size_t argc, char **argv)
{
	int ms = argc > 1 ? atoi(argv[1]) : CONFIG_TIMESLICE_SIZE;

	k_sched_time_slice_set(ms, CONFIG_TIMESLICE_PRIORITY);
	shell_print(sh, "time slice %d ms", ms);
	return 0;
}
#endif

/* Cost of reading 1.5 KB of WRAM (RX buffer 1) uncached vs through its cached
 * view after an invalidate, and whether both views return the same bytes. */
static int cmd_wramtest(const struct shell *sh, size_t argc, char **argv)
{
	extern uint8_t __wifi_bss_start[];
	const uint32_t len = 1536;
	uint8_t *dst = malloc(2 * len);
	uintptr_t nc = (uintptr_t)__wifi_bss_start;
	uintptr_t ca = nc - 0x10000000UL;
	uint32_t t0, t1, t2, t3, t4;
	unsigned int key;

/* the memory clobber keeps the compiler from merging or dropping the copies */
#define WRAMTEST_CYC(v) __asm__ volatile("csrr %0, mcycle" : "=r"(v) : : "memory")
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	if (dst == NULL) {
		return -ENOMEM;
	}
	key = irq_lock();
	WRAMTEST_CYC(t0);
	memcpy(dst, (const void *)nc, len);
	WRAMTEST_CYC(t1);
	MInvalDCacheRange(ca, len);
	WRAMTEST_CYC(t2);
	memcpy(dst + len, (const void *)ca, len);
	WRAMTEST_CYC(t3);
	memcpy(dst + len, (const void *)ca, len); /* warm: already in the cache */
	WRAMTEST_CYC(t4);
	irq_unlock(key);
	shell_print(sh, "WRAM %p (cached view %p), %u B: uncached %u cyc, invalidate %u cyc, cached cold %u cyc, "
		    "warm %u cyc, same bytes %s", (void *)nc, (void *)ca, len, t1 - t0, t2 - t1, t3 - t2, t4 - t3,
		    memcmp(dst, dst + len, len) == 0 ? "yes" : "NO");
	free(dst);
	return 0;
}

/* MAC TX confirmations per access category (txl_cfm.c): how many MPDUs went
 * out, how many were acknowledged, and why the rest were given up. */
static int cmd_txstat(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint32_t macsw_cfm_total[], macsw_cfm_ok[], macsw_cfm_retry_limit[],
		macsw_cfm_lifetime[], macsw_cfm_nostatus[], macsw_cfm_retries[8];
	static const char *const ac_name[] = {"BK", "BE", "VI", "VO", "BCN"};

	for (int ac = 0; ac < NX_TXQ_CNT && ac < ARRAY_SIZE(ac_name); ac++) {
		if (argc > 1 && argv[1][0] == 'r') {
			macsw_cfm_total[ac] = macsw_cfm_ok[ac] = macsw_cfm_retry_limit[ac] = 0;
			macsw_cfm_lifetime[ac] = macsw_cfm_nostatus[ac] = 0;
		} else if (macsw_cfm_total[ac]) {
			shell_print(sh, "%s: %u MPDUs, acked %u, retry limit %u, lifetime %u, no status %u",
				    ac_name[ac], macsw_cfm_total[ac], macsw_cfm_ok[ac],
				    macsw_cfm_retry_limit[ac], macsw_cfm_lifetime[ac], macsw_cfm_nostatus[ac]);
		}
	}
	if (argc > 1 && argv[1][0] == 'r') {
		memset((void *)macsw_cfm_retries, 0, sizeof(uint32_t) * 8);
		return 0;
	}
	shell_print(sh, "retries: %u %u %u %u %u %u %u %u+", macsw_cfm_retries[0], macsw_cfm_retries[1],
		    macsw_cfm_retries[2], macsw_cfm_retries[3], macsw_cfm_retries[4], macsw_cfm_retries[5],
		    macsw_cfm_retries[6], macsw_cfm_retries[7]);
	return 0;
}

static int cmd_rxstat(const struct shell *sh, size_t argc, char **argv)
{
	extern volatile uint32_t macsw_rxl_del_nolen, macsw_rxl_del_bcn_same, macsw_rxl_del_bfmer,
		macsw_rxl_del_datamgt, macsw_rxl_del_other, macsw_rxl_up_bcn_new, macsw_rxl_up_datamgt,
		macsw_rxl_up_nosucc, macsw_rxl_up_nokey, macsw_rxl_up_badsta, macsw_rxl_up_novif;
	extern uint32_t ipro_zif_rx_ok, ipro_zif_rx_nopkt, ipro_zif_rx_err;
	extern uint32_t fhost_rx_free_bufs(void);
	extern volatile uint32_t macsw_rxl_desc_ptr_bad, macsw_rxl_desc_pattern_bad,
		macsw_rxl_embedded_ptr_bad, macsw_rxl_embed_gaveup, macsw_rxl_buf1_hwm;
	extern volatile uint32_t macsw_rxu_drop_old, macsw_rxu_drop_dup, macsw_rxu_drop_pn,
		macsw_rxu_drop_noba, macsw_rxu_skip_flush, macsw_rxu_skip_timeout;

	shell_print(sh, "rxl up: data/mgmt %u bcn_new %u | up fail: nosucc %u nokey %u badsta %u novif %u",
		    macsw_rxl_up_datamgt, macsw_rxl_up_bcn_new, macsw_rxl_up_nosucc, macsw_rxl_up_nokey,
		    macsw_rxl_up_badsta, macsw_rxl_up_novif);
	shell_print(sh, "rxl del: nolen %u bcn_same %u bfmer %u datamgt %u other %u", macsw_rxl_del_nolen,
		    macsw_rxl_del_bcn_same, macsw_rxl_del_bfmer, macsw_rxl_del_datamgt, macsw_rxl_del_other);
	shell_print(sh, "fhost rx free bufs %u | driver rx: to stack %u, no net_pkt %u, refused %u",
		    fhost_rx_free_bufs(), ipro_zif_rx_ok, ipro_zif_rx_nopkt, ipro_zif_rx_err);
	shell_print(sh, "rx ring 1: peak %u of %u B | resync: ptr %u pattern %u embedded %u gave up %u",
		    macsw_rxl_buf1_hwm, CONFIG_IPRO_WIFI_RXL_BUF1_BYTES, macsw_rxl_desc_ptr_bad,
		    macsw_rxl_desc_pattern_bad, macsw_rxl_embedded_ptr_bad, macsw_rxl_embed_gaveup);
	shell_print(sh, "rxu drop: old %u dup %u pn %u no-BA %u | reorder gave up on SNs: flush %u timeout %u",
		    macsw_rxu_drop_old, macsw_rxu_drop_dup, macsw_rxu_drop_pn, macsw_rxu_drop_noba,
		    macsw_rxu_skip_flush, macsw_rxu_skip_timeout);
	if (argc > 1 && argv[1][0] == 'r') {
		macsw_rxl_buf1_hwm = 0;
	}
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_wifi_ipro,
	SHELL_CMD_ARG(rxstat, NULL, "MAC/fhost/driver RX counters [r: reset the ring peak]", cmd_rxstat, 1, 1),
	SHELL_CMD_ARG(txstat, NULL, "MAC TX confirmations per AC [r: reset]", cmd_txstat, 1, 1),
	SHELL_CMD_ARG(dmacpy, NULL, "software-DMA copy [unlocked 0|1] [cached_src 0|1]", cmd_dmacpy, 1, 2),
#ifdef CONFIG_TIMESLICING
	SHELL_CMD_ARG(slice, NULL, "scheduler time slice [ms, 0 = off]", cmd_slice, 1, 1),
#endif
	SHELL_CMD_ARG(wramtest, NULL, "WRAM read cost, uncached vs cached view", cmd_wramtest, 1, 0),
	SHELL_CMD_ARG(ps, NULL, "station 802.11 power save [0|1]", cmd_ps, 1, 1),
	SHELL_CMD_ARG(txop, NULL, "BE A-MPDU duration cap [32us units]", cmd_txop, 1, 1),
	SHELL_CMD_ARG(agg, NULL, "A-MPDU knobs [max_cnt] [hold_us] [skip_n] [pipeline] [hold_max] [min_cnt] [only_busy]", cmd_agg, 1, 7),
	SHELL_CMD_ARG(csum, NULL, "UDP TX checksum offload [0|1]", cmd_csum, 1, 1),
	SHELL_CMD_ARG(amsdu, NULL, "A-MSDU batching [en] [max_batch] [payl_max]", cmd_amsdu, 1, 3),
	SHELL_CMD_ARG(tprio, NULL, "thread priority <name> [prio]", cmd_tprio, 2, 1),
	SHELL_CMD_ARG(batch, NULL, "deferred MAC kicks per burst [0|1]", cmd_batch, 1, 1),
	SHELL_CMD_ARG(pcsample, NULL, "print interrupted PC every tick [count]", cmd_pcsample, 1, 1),
	SHELL_CMD_ARG(evtprof, NULL, "WiFi task event CPU profile [r]", cmd_evtprof, 1, 1),
	SHELL_CMD_ARG(txdiag, NULL, "TX pipeline state (txl / fhost / macif / driver)", cmd_txdiag, 1, 0),
	SHELL_CMD_ARG(macreset, NULL, "force the MAC self-reset path (KE_EVT_RESET)", cmd_macreset, 1, 0),
	SHELL_CMD_ARG(wedge, NULL, "MAC TX wedge snapshot", cmd_wedge, 1, 0),
	SHELL_CMD_ARG(cpubench, NULL, "flash vs RAM code fetch micro-benchmark", cmd_cpubench, 1, 0),
	SHELL_CMD_ARG(txsplit, NULL, "driver TX CPU split [r]", cmd_txsplit, 1, 1),
	SHELL_CMD_ARG(macstat, NULL, "MAC HW error / reset counters", cmd_macstat, 1, 0),
	SHELL_CMD_ARG(pdg, NULL, "RX gain-ladder hold-off [beacons]", cmd_pdg, 1, 1),
	SHELL_CMD_ARG(txpwr, NULL, "TX power table [flat dBm]", cmd_txpwr, 1, 1),
	SHELL_CMD_ARG(txprof, NULL, "SDK TX profilers: txprof r (arm+reset), txprof (print)", cmd_txprof, 1, 1),
	SHELL_CMD_ARG(txtrace, NULL, "fhost TX timing trace [reset]", cmd_txtrace, 1, 1),
	SHELL_CMD_ARG(rc, NULL, "Rate-control state [sta_idx]", cmd_rc, 1, 1),
#ifdef CONFIG_WIFI_IPRO_TCP_TRACE
	SHELL_CMD_ARG(tcptrace, NULL, "TCP timing trace: tcptrace arm [tx], then tcptrace (dump)",
		      ipro_tcptrace_cmd, 1, 2),
#endif
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(wifi_ipro, &sub_wifi_ipro, "IPRO Wi-Fi driver commands", NULL);
