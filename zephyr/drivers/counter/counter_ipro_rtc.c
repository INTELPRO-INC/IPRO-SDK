/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * HBN RTC as a Zephyr counter: a free-running 40-bit counter on the 32 kHz
 * low-power clock (RC32K or XTAL32K, whichever the SoC init selected) with
 * one compare register routed to the HBN_OUT0 interrupt line. The counter
 * keeps running through PDS/HBN sleep, so it is the timebase a low-power
 * application can trust across sleep; it is also what the SDK's LP
 * framework programs for its wake-up, so only one owner at a time.
 *
 * The RC32K is trimmed from eFuse only (about 5 % off on the bench EVB); the
 * driver measures the actual rate against the 32 MHz crystal at init
 * (CONFIG_COUNTER_IPRO_RTC_MEASURE_MS) and reports that as the counter
 * frequency, so tick <-> time conversions are right without the BLE
 * controller's 1 s RC32K calibration. Boards with XTAL32K measure 32768.
 *
 * Exposed as a 32-bit counter (the low word of the 40-bit value): alarms
 * are programmed against the full 40-bit compare so a wrap of the low word
 * is handled; top value is fixed at UINT32_MAX; the counter cannot be
 * stopped (the SoC keeps it enabled) so counter_stop() is -ENOTSUP.
 */

#define DT_DRV_COMPAT intelpro_ipro_rtc

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/irq.h>
#include <zephyr/spinlock.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "drv_hbn.h"

#define RTC_IPRO_MASK40 ((1ULL << 40) - 1)

struct rtc_ipro_config {
	struct counter_config_info info;
	uint32_t irq;
};

struct rtc_ipro_data {
	struct k_spinlock lock;
	uint32_t freq;
	counter_alarm_callback_t alarm_cb;
	void *alarm_data;
	bool alarm_armed;
};

static uint64_t rtc_ipro_read40(void)
{
	uint32_t lo, hi;

	(void)HBN_Get_RTC_Timer_Val(&lo, &hi);
	return (((uint64_t)hi << 32) | lo) & RTC_IPRO_MASK40;
}

static int rtc_ipro_start(const struct device *dev)
{
	ARG_UNUSED(dev);
	return HBN_Enable_RTC_Counter() == 0 ? 0 : -EIO;
}

static int rtc_ipro_stop(const struct device *dev)
{
	ARG_UNUSED(dev);
	/* The RTC is the SoC's sleep timebase; it is never stopped. */
	return -ENOTSUP;
}

static int rtc_ipro_get_value(const struct device *dev, uint32_t *ticks)
{
	ARG_UNUSED(dev);
	*ticks = (uint32_t)rtc_ipro_read40();
	return 0;
}

#ifdef CONFIG_COUNTER_64BITS_TICKS
static int rtc_ipro_get_value_64(const struct device *dev, uint64_t *ticks)
{
	ARG_UNUSED(dev);
	*ticks = rtc_ipro_read40();
	return 0;
}
#endif

static void rtc_ipro_disarm(void)
{
	(void)HBN_Disable_RTC_Compare();
	(void)HBN_Clear_RTC_INT();
	(void)HBN_Clear_IRQ(HBN_INT_RTC);
}

static int rtc_ipro_set_alarm(const struct device *dev, uint8_t chan,
			      const struct counter_alarm_cfg *alarm_cfg)
{
	struct rtc_ipro_data *data = dev->data;
	uint64_t now, target;
	bool late = false;

	if (chan != 0U || alarm_cfg == NULL || alarm_cfg->callback == NULL) {
		return -EINVAL;
	}

	k_spinlock_key_t key = k_spin_lock(&data->lock);

	if (data->alarm_armed) {
		k_spin_unlock(&data->lock, key);
		return -EBUSY;
	}

	now = rtc_ipro_read40();
	if (alarm_cfg->flags & COUNTER_ALARM_CFG_ABSOLUTE) {
		/* Absolute in the 32-bit view: the next 40-bit instant whose low
		 * word equals the requested tick. */
		target = (now & ~0xFFFFFFFFULL) | alarm_cfg->ticks;
		if (target <= now) {
			if (alarm_cfg->flags & COUNTER_ALARM_CFG_EXPIRE_WHEN_LATE) {
				late = true;
			} else {
				target += 1ULL << 32;
			}
		}
	} else {
		target = now + alarm_cfg->ticks;
	}
	target &= RTC_IPRO_MASK40;

	data->alarm_cb = alarm_cfg->callback;
	data->alarm_data = alarm_cfg->user_data;
	data->alarm_armed = true;

	if (late) {
		k_spin_unlock(&data->lock, key);
		data->alarm_armed = false;
		alarm_cfg->callback(dev, 0, (uint32_t)now, alarm_cfg->user_data);
		return 0;
	}

	(void)HBN_Clear_IRQ(HBN_INT_RTC);
	(void)HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, (uint32_t)target, (uint32_t)(target >> 32),
				HBN_RTC_COMP_BIT0_39);
	k_spin_unlock(&data->lock, key);
	return 0;
}

static int rtc_ipro_cancel_alarm(const struct device *dev, uint8_t chan)
{
	struct rtc_ipro_data *data = dev->data;

	if (chan != 0U) {
		return -EINVAL;
	}
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	rtc_ipro_disarm();
	data->alarm_armed = false;
	data->alarm_cb = NULL;
	k_spin_unlock(&data->lock, key);
	return 0;
}

static int rtc_ipro_set_top(const struct device *dev, const struct counter_top_cfg *cfg)
{
	ARG_UNUSED(dev);
	/* Free-running hardware counter: only the full 32-bit range. */
	if (cfg->ticks == UINT32_MAX && cfg->callback == NULL) {
		return 0;
	}
	return -ENOTSUP;
}

static uint32_t rtc_ipro_get_top(const struct device *dev)
{
	ARG_UNUSED(dev);
	return UINT32_MAX;
}

static uint32_t rtc_ipro_get_pending(const struct device *dev)
{
	ARG_UNUSED(dev);
	return HBN_Get_INT_State(HBN_INT_RTC) ? 1U : 0U;
}

static uint32_t rtc_ipro_get_freq(const struct device *dev)
{
	const struct rtc_ipro_data *data = dev->data;

	return data->freq;
}

/* Count RTC ticks over a busy-wait on the crystal-derived cycle counter. */
static uint32_t rtc_ipro_measure_freq(uint32_t nominal)
{
	uint64_t t0, t1;
	uint32_t c0, c1, elapsed_us, freq;

	if (CONFIG_COUNTER_IPRO_RTC_MEASURE_MS == 0) {
		return nominal;
	}
	t0 = rtc_ipro_read40();
	c0 = k_cycle_get_32();
	k_busy_wait(CONFIG_COUNTER_IPRO_RTC_MEASURE_MS * 1000U);
	t1 = rtc_ipro_read40();
	c1 = k_cycle_get_32();
	elapsed_us = k_cyc_to_us_floor32(c1 - c0);
	if (elapsed_us == 0U || t1 <= t0) {
		return nominal;
	}
	freq = (uint32_t)(((t1 - t0) * 1000000ULL + elapsed_us / 2U) / elapsed_us);
	/* Sanity: an RC32K within +-10 % or a crystal; otherwise keep nominal. */
	if (freq < nominal - nominal / 10U || freq > nominal + nominal / 10U) {
		return nominal;
	}
	return freq;
}

static void rtc_ipro_isr(const void *arg)
{
	const struct device *dev = arg;
	struct rtc_ipro_data *data = dev->data;
	counter_alarm_callback_t cb;
	void *user;

	if (!HBN_Get_INT_State(HBN_INT_RTC)) {
		/*
		 * Another HBN_OUT0 source (wake-pin GPIO 8-14/22/30/31, BOR,
		 * ACOMP) is pending. Nothing in this port consumes those, and the
		 * line is level-sensitive: leaving the status set re-enters this
		 * ISR forever (seen on ipro7ai_evk 2026-09-21, where the LCD /
		 * PWM pads sit on wake pins). Acknowledge everything but the RTC.
		 */
		uint32_t pending = sys_read32(HBN_BASE + HBN_IRQ_STAT_OFFSET) &
				   ~BIT(HBN_INT_RTC);

		for (int i = 0; i < 32 && pending != 0U; i++) {
			if (pending & BIT(i)) {
				(void)HBN_Clear_IRQ((HBN_INT_Type)i);
				pending &= ~BIT(i);
			}
		}
		return;
	}

	k_spinlock_key_t key = k_spin_lock(&data->lock);

	rtc_ipro_disarm();
	cb = data->alarm_cb;
	user = data->alarm_data;
	data->alarm_cb = NULL;
	data->alarm_armed = false;
	k_spin_unlock(&data->lock, key);

	if (cb != NULL) {
		cb(dev, 0, (uint32_t)rtc_ipro_read40(), user);
	}
}

static int rtc_ipro_init(const struct device *dev)
{
	const struct rtc_ipro_config *cfg = dev->config;

	struct rtc_ipro_data *data = dev->data;

	rtc_ipro_disarm();
	/* HBN_OUT0 is shared with the wake-pin / BOR / ACOMP sources: mask the
	 * wake pins (bit set = masked) and drop anything already pending so
	 * enabling the line only ever fires for the RTC compare. */
	(void)HBN_Pin_WakeUp_Mask(0x7FF);
	(void)HBN_Clear_All_IRQ();
	(void)HBN_Enable_RTC_Counter();
	data->freq = rtc_ipro_measure_freq(cfg->info.freq);
	/* One HBN block, one RTC: static connect on the single instance. */
	IRQ_CONNECT(DT_INST_IRQN(0), DT_INST_IRQ(0, priority), rtc_ipro_isr,
		    DEVICE_DT_INST_GET(0), 0);
	irq_enable(cfg->irq);
	return 0;
}

static DEVICE_API(counter, rtc_ipro_api) = {
	.start = rtc_ipro_start,
	.stop = rtc_ipro_stop,
	.get_value = rtc_ipro_get_value,
#ifdef CONFIG_COUNTER_64BITS_TICKS
	.get_value_64 = rtc_ipro_get_value_64,
#endif
	.set_alarm = rtc_ipro_set_alarm,
	.cancel_alarm = rtc_ipro_cancel_alarm,
	.set_top_value = rtc_ipro_set_top,
	.get_pending_int = rtc_ipro_get_pending,
	.get_top_value = rtc_ipro_get_top,
	.get_freq = rtc_ipro_get_freq,
};

#define RTC_IPRO_INIT(n)                                                                           \
	static const struct rtc_ipro_config rtc_ipro_cfg_##n = {                                   \
		.info = {.max_top_value = UINT32_MAX,                                              \
			 .freq = DT_INST_PROP_OR(n, clock_frequency, 32768),                       \
			 .flags = COUNTER_CONFIG_INFO_COUNT_UP,                                    \
			 .channels = 1},                                                           \
		.irq = DT_INST_IRQN(n),                                                            \
	};                                                                                         \
	static struct rtc_ipro_data rtc_ipro_data_##n;                                             \
	DEVICE_DT_INST_DEFINE(n, rtc_ipro_init, NULL, &rtc_ipro_data_##n, &rtc_ipro_cfg_##n,      \
			      POST_KERNEL, CONFIG_COUNTER_INIT_PRIORITY, &rtc_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(RTC_IPRO_INIT)
