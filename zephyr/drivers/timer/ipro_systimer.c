/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Zephyr system timer on the IPRO7's Nuclei private systimer.
 *
 * The algorithm is Zephyr's drivers/timer/riscv_machine_timer.c (64-bit
 * free-running mtime, single mtimecmp, tickless). The register base comes
 * from ipro_systimer_base() (MIRGB_INFO + 0x30000) instead of the device
 * tree, and the Nuclei MTIMECTL register is used to make sure the counter
 * is running. mtime ticks at SYS_CLOCK_HW_CYCLES_PER_SEC (4 MHz).
 */

#define DT_DRV_COMPAT intelpro_ipro_systimer

#include <limits.h>
#include <zephyr/init.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys_clock.h>
#include <zephyr/irq.h>
#include <soc.h>

#define TIMER_IRQN     DT_INST_IRQN(0)
#define TIMER_IRQ_PRIO DT_INST_IRQ(0, priority)

#define MTIMER_OFS     0x000
#define MTIMERCMP_OFS  0x008
#define MTIMECTL_OFS   0xFF8
#define MSIP_OFS       0xFFC
#define MTIMECTL_TIMESTOP BIT(0)

#define CYC_PER_TICK ((uint32_t)(sys_clock_hw_cycles_per_sec() / CONFIG_SYS_CLOCK_TICKS_PER_SEC))

/* Same bounds reasoning as riscv_machine_timer.c */
#define cycle_diff_t   unsigned long
#define CYCLE_DIFF_MAX (~(cycle_diff_t)0)
#define CYCLES_MAX_1   ((uint64_t)INT32_MAX * (uint64_t)CYC_PER_TICK)
#define CYCLES_MAX_2   ((uint64_t)CYCLE_DIFF_MAX)
#define CYCLES_MAX_3   MIN(CYCLES_MAX_1, CYCLES_MAX_2)
#define CYCLES_MAX_4   (CYCLES_MAX_3 / 2 + CYCLES_MAX_3 / 4)
#define CYCLES_MAX     (CYCLES_MAX_4 + LSB_GET(CYCLES_MAX_4))

static uint64_t last_count;
static uint64_t last_ticks;
static uint32_t last_elapsed;
static uintptr_t timer_base;

#if defined(CONFIG_TEST)
const int32_t z_sys_timer_irq_for_test = TIMER_IRQN;
#endif

static void set_mtimecmp(uint64_t time)
{
	volatile uint32_t *r = (volatile uint32_t *)(timer_base + MTIMERCMP_OFS);

	/* Not latched: park the high word first so no spurious match fires. */
	r[1] = 0xffffffff;
	r[0] = (uint32_t)time;
	r[1] = (uint32_t)(time >> 32);
}

static uint64_t mtime(void)
{
	volatile uint32_t *r = (volatile uint32_t *)(timer_base + MTIMER_OFS);
	uint32_t lo, hi;

	do {
		hi = r[1];
		lo = r[0];
	} while (r[1] != hi);

	return (((uint64_t)hi) << 32) | lo;
}

static void timer_isr(const void *arg)
{
	ARG_UNUSED(arg);

	/* Kernel-owned timer lock (v4.4 sys_clock_lock/announce_locked model). */
	k_spinlock_key_t key = sys_clock_lock();
	uint64_t now = mtime();
	uint64_t dcycles = now - last_count;
	uint32_t dticks = (cycle_diff_t)dcycles / CYC_PER_TICK;

	last_count += (cycle_diff_t)dticks * CYC_PER_TICK;
	last_ticks += dticks;
	last_elapsed = 0;

	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		set_mtimecmp(last_count + CYC_PER_TICK);
	}

	sys_clock_announce_locked(dticks, key);
}

void sys_clock_set_timeout(int32_t ticks, bool idle)
{
	ARG_UNUSED(idle);

	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		return;
	}

	/* Called with the kernel timer lock held. */
	uint64_t cyc;

	if (ticks == K_TICKS_FOREVER) {
		cyc = last_count + CYCLES_MAX;
	} else {
		cyc = (last_ticks + last_elapsed + ticks) * CYC_PER_TICK;
		if ((cyc - last_count) > CYCLES_MAX) {
			cyc = last_count + CYCLES_MAX;
		}
	}
	set_mtimecmp(cyc);
}

uint32_t sys_clock_elapsed(void)
{
	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		return 0;
	}

	uint64_t now = mtime();
	uint64_t dcycles = now - last_count;
	uint32_t dticks = (cycle_diff_t)dcycles / CYC_PER_TICK;

	last_elapsed = dticks;
	return dticks;
}

uint32_t sys_clock_cycle_get_32(void)
{
	return (uint32_t)mtime();
}

uint64_t sys_clock_cycle_get_64(void)
{
	return mtime();
}

void sys_clock_disable(void)
{
	irq_disable(TIMER_IRQN);
}

/*
 * Re-arm the tick one period ahead of "now". Used by the SDK's BLE
 * controller (through the FreeRTOS shim's vPortSetupTimerInterrupt) after a
 * recovery path that may have left mtimecmp in the past.
 */
void ipro_systimer_rearm(void)
{
	k_spinlock_key_t key = sys_clock_lock();

	set_mtimecmp(mtime() + CYC_PER_TICK);
	sys_clock_announce_locked(0, key);
}

static int sys_clock_driver_init(void)
{
	timer_base = ipro_systimer_base();
	__ASSERT(timer_base != 0, "IREGION base not discovered before systimer init");

	/* Make sure the counter runs (SDK: SysTimer_Start()). */
	volatile uint32_t *ctl = (volatile uint32_t *)(timer_base + MTIMECTL_OFS);

	*ctl &= ~MTIMECTL_TIMESTOP;
	/* No stray software interrupt. */
	*(volatile uint32_t *)(timer_base + MSIP_OFS) = 0;

	IRQ_CONNECT(TIMER_IRQN, TIMER_IRQ_PRIO, timer_isr, NULL, 0);
	last_ticks = mtime() / CYC_PER_TICK;
	last_count = last_ticks * CYC_PER_TICK;
	set_mtimecmp(last_count + CYC_PER_TICK);
	irq_enable(TIMER_IRQN);
	return 0;
}

SYS_INIT(sys_clock_driver_init, PRE_KERNEL_2, CONFIG_SYSTEM_CLOCK_INIT_PRIORITY);
