/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 TIMER channel as a Zephyr counter (SDK LL drv_timer.c).
 *
 * Comparators 0 and 1 are the two alarm channels; comparator 2 holds the
 * top value and, in preload mode, reloads the counter with 0 when reached
 * (TIMER_PRELOAD_TRIG_COMP2), giving wrap-around at top. The channel clock
 * is programmed by the clock-control driver (XTAL/32 = 1 MHz).
 */

#define DT_DRV_COMPAT intelpro_ipro_counter

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/irq.h>
#include <zephyr/spinlock.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "drv_timer.h"
#include "drv_clock.h"

#define COUNTER_IPRO_ALARMS 2
#define COUNTER_IPRO_TOP_COMP TIMER_COMP_ID_2

struct counter_ipro_config {
	struct counter_config_info info;
	TIMER_Chan_Type ch;
	uint32_t irq;
	uint32_t irq_prio;
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
};

struct counter_ipro_data {
	struct k_spinlock lock;
	counter_alarm_callback_t alarm_cb[COUNTER_IPRO_ALARMS];
	void *alarm_data[COUNTER_IPRO_ALARMS];
	counter_top_callback_t top_cb;
	void *top_data;
	uint32_t top;
};

/*
 * The LL counter read (TCVWR request + immediate read-back) returns the value
 * latched by the PREVIOUS request at 192 MHz: measured on the EVK, consecutive
 * reads lag one call behind. TCVSYNn is the live, synchronised counter
 * (100 ms busy-wait read as 100002 ticks at 1 MHz), so read that instead.
 */
static inline uint32_t counter_ipro_read(TIMER_Chan_Type ch)
{
	return *(volatile uint32_t *)(TIMER_BASE + TIMER_TCVSYN2_OFFSET + 4 * ch);
}

static int counter_ipro_start(const struct device *dev)
{
	const struct counter_ipro_config *cfg = dev->config;

	TIMER_Enable(TIMER0_ID, cfg->ch);
	return 0;
}

static int counter_ipro_stop(const struct device *dev)
{
	const struct counter_ipro_config *cfg = dev->config;

	TIMER_Disable(TIMER0_ID, cfg->ch);
	return 0;
}

static int counter_ipro_get_value(const struct device *dev, uint32_t *ticks)
{
	const struct counter_ipro_config *cfg = dev->config;

	*ticks = counter_ipro_read(cfg->ch);
	return 0;
}

static int counter_ipro_set_alarm(const struct device *dev, uint8_t chan,
				  const struct counter_alarm_cfg *alarm_cfg)
{
	const struct counter_ipro_config *cfg = dev->config;
	struct counter_ipro_data *data = dev->data;
	uint32_t ticks = alarm_cfg->ticks;

	if (chan >= COUNTER_IPRO_ALARMS) {
		return -EINVAL;
	}
	if (ticks > data->top) {
		return -EINVAL;
	}

	k_spinlock_key_t key = k_spin_lock(&data->lock);

	if (data->alarm_cb[chan] != NULL) {
		k_spin_unlock(&data->lock, key);
		return -EBUSY;
	}

	if (!(alarm_cfg->flags & COUNTER_ALARM_CFG_ABSOLUTE)) {
		uint32_t now = counter_ipro_read(cfg->ch);

		ticks = (now + ticks) % (data->top + 1);
	}

	data->alarm_cb[chan] = alarm_cfg->callback;
	data->alarm_data[chan] = alarm_cfg->user_data;
	TIMER_SetCompValue(TIMER0_ID, cfg->ch, (TIMER_Comp_ID_Type)chan, ticks);
	TIMER_ClearIntStatus(TIMER0_ID, cfg->ch, (TIMER_Comp_ID_Type)chan);
	TIMER_IntMask(TIMER0_ID, cfg->ch, (TIMER_INT_Type)chan, UNMASK);

	k_spin_unlock(&data->lock, key);
	return 0;
}

static int counter_ipro_cancel_alarm(const struct device *dev, uint8_t chan)
{
	const struct counter_ipro_config *cfg = dev->config;
	struct counter_ipro_data *data = dev->data;

	if (chan >= COUNTER_IPRO_ALARMS) {
		return -EINVAL;
	}

	k_spinlock_key_t key = k_spin_lock(&data->lock);

	TIMER_IntMask(TIMER0_ID, cfg->ch, (TIMER_INT_Type)chan, MASK);
	TIMER_ClearIntStatus(TIMER0_ID, cfg->ch, (TIMER_Comp_ID_Type)chan);
	data->alarm_cb[chan] = NULL;
	k_spin_unlock(&data->lock, key);
	return 0;
}

static int counter_ipro_set_top_value(const struct device *dev, const struct counter_top_cfg *top_cfg)
{
	const struct counter_ipro_config *cfg = dev->config;
	struct counter_ipro_data *data = dev->data;

	for (int i = 0; i < COUNTER_IPRO_ALARMS; i++) {
		if (data->alarm_cb[i] != NULL) {
			return -EBUSY;
		}
	}
	if (top_cfg->ticks == 0) {
		return -EINVAL;
	}

	k_spinlock_key_t key = k_spin_lock(&data->lock);

	data->top = top_cfg->ticks;
	data->top_cb = top_cfg->callback;
	data->top_data = top_cfg->user_data;

	TIMER_SetCompValue(TIMER0_ID, cfg->ch, COUNTER_IPRO_TOP_COMP, top_cfg->ticks);
	TIMER_ClearIntStatus(TIMER0_ID, cfg->ch, COUNTER_IPRO_TOP_COMP);
	TIMER_IntMask(TIMER0_ID, cfg->ch, TIMER_INT_COMP_2, top_cfg->callback ? UNMASK : MASK);

	if (!(top_cfg->flags & COUNTER_TOP_CFG_DONT_RESET)) {
		/* Re-arm from zero: the preload path restarts the count. */
		TIMER_Disable(TIMER0_ID, cfg->ch);
		TIMER_SetPreloadValue(TIMER0_ID, cfg->ch, 0);
		TIMER_Enable(TIMER0_ID, cfg->ch);
	} else if (counter_ipro_read(cfg->ch) >= top_cfg->ticks) {
		k_spin_unlock(&data->lock, key);
		return -ETIME;
	}

	k_spin_unlock(&data->lock, key);
	return 0;
}

static uint32_t counter_ipro_get_pending_int(const struct device *dev)
{
	const struct counter_ipro_config *cfg = dev->config;

	return TIMER_GetMatchStatus(TIMER0_ID, cfg->ch, TIMER_COMP_ID_0) ||
	       TIMER_GetMatchStatus(TIMER0_ID, cfg->ch, TIMER_COMP_ID_1) ||
	       TIMER_GetMatchStatus(TIMER0_ID, cfg->ch, TIMER_COMP_ID_2);
}

static uint32_t counter_ipro_get_top_value(const struct device *dev)
{
	struct counter_ipro_data *data = dev->data;

	return data->top;
}

static uint32_t counter_ipro_get_freq(const struct device *dev)
{
	const struct counter_ipro_config *cfg = dev->config;
	uint32_t rate = 0;

	/* The clock-control driver knows the channel clock it programmed
	 * (XTAL / (div + 1)); the IPRO6 LL has no TIMER entry in
	 * Clock_Peripheral_Clock_Get(). */
	if (clock_control_get_rate(cfg->clock_dev, cfg->clock_id, &rate) != 0) {
		return 0;
	}
	return rate;
}

static void counter_ipro_isr(const void *arg)
{
	const struct device *dev = arg;
	const struct counter_ipro_config *cfg = dev->config;
	struct counter_ipro_data *data = dev->data;

	for (int i = 0; i < COUNTER_IPRO_ALARMS; i++) {
		if (TIMER_GetMatchStatus(TIMER0_ID, cfg->ch, (TIMER_Comp_ID_Type)i)) {
			counter_alarm_callback_t cb = data->alarm_cb[i];

			TIMER_IntMask(TIMER0_ID, cfg->ch, (TIMER_INT_Type)i, MASK);
			TIMER_ClearIntStatus(TIMER0_ID, cfg->ch, (TIMER_Comp_ID_Type)i);
			data->alarm_cb[i] = NULL;
			if (cb != NULL) {
				cb(dev, i, counter_ipro_read(cfg->ch), data->alarm_data[i]);
			}
		}
	}
	if (TIMER_GetMatchStatus(TIMER0_ID, cfg->ch, COUNTER_IPRO_TOP_COMP)) {
		TIMER_ClearIntStatus(TIMER0_ID, cfg->ch, COUNTER_IPRO_TOP_COMP);
		if (data->top_cb != NULL) {
			data->top_cb(dev, data->top_data);
		}
	}
}

static int counter_ipro_init(const struct device *dev)
{
	const struct counter_ipro_config *cfg = dev->config;
	struct counter_ipro_data *data = dev->data;

	(void)clock_control_on(cfg->clock_dev, cfg->clock_id);

	TIMER_Disable(TIMER0_ID, cfg->ch);
	TIMER_IntMask(TIMER0_ID, cfg->ch, TIMER_INT_ALL, MASK);
	TIMER_SetCountMode(TIMER0_ID, cfg->ch, TIMER_COUNT_PRELOAD);
	TIMER_SetPreloadSrc(TIMER0_ID, cfg->ch, TIMER_PRELOAD_TRIG_COMP2);
	TIMER_SetPreloadValue(TIMER0_ID, cfg->ch, 0);
	data->top = cfg->info.max_top_value;
	TIMER_SetCompValue(TIMER0_ID, cfg->ch, TIMER_COMP_ID_0, 0xFFFFFFFF);
	TIMER_SetCompValue(TIMER0_ID, cfg->ch, TIMER_COMP_ID_1, 0xFFFFFFFF);
	TIMER_SetCompValue(TIMER0_ID, cfg->ch, COUNTER_IPRO_TOP_COMP, data->top);
	TIMER_ClearIntStatus(TIMER0_ID, cfg->ch, TIMER_COMP_ID_0);
	TIMER_ClearIntStatus(TIMER0_ID, cfg->ch, TIMER_COMP_ID_1);
	TIMER_ClearIntStatus(TIMER0_ID, cfg->ch, COUNTER_IPRO_TOP_COMP);

	irq_connect_dynamic(cfg->irq, cfg->irq_prio, counter_ipro_isr, dev, 0);
	irq_enable(cfg->irq);
	return 0;
}

static DEVICE_API(counter, counter_ipro_api) = {
	.start = counter_ipro_start,
	.stop = counter_ipro_stop,
	.get_value = counter_ipro_get_value,
	.set_alarm = counter_ipro_set_alarm,
	.cancel_alarm = counter_ipro_cancel_alarm,
	.set_top_value = counter_ipro_set_top_value,
	.get_pending_int = counter_ipro_get_pending_int,
	.get_top_value = counter_ipro_get_top_value,
	.get_freq = counter_ipro_get_freq,
};

#define COUNTER_IPRO_PARENT(n) DT_INST_PARENT(n)

#define COUNTER_IPRO_INIT(n)                                                                       \
	static const struct counter_ipro_config counter_ipro_cfg_##n = {                           \
		.info = {                                                                          \
			.max_top_value = UINT32_MAX,                                               \
			.flags = COUNTER_CONFIG_INFO_COUNT_UP,                                     \
			.channels = COUNTER_IPRO_ALARMS,                                           \
		},                                                                                 \
		.ch = (TIMER_Chan_Type)DT_INST_PROP(n, channel),                                   \
		.irq = DT_IRQ_BY_IDX(COUNTER_IPRO_PARENT(n), DT_INST_PROP(n, channel), irq),       \
		.irq_prio = DT_IRQ_BY_IDX(COUNTER_IPRO_PARENT(n), DT_INST_PROP(n, channel), priority), \
		.clock_dev = DEVICE_DT_GET(DT_CLOCKS_CTLR(COUNTER_IPRO_PARENT(n))),                \
		.clock_id = (clock_control_subsys_t)DT_CLOCKS_CELL(COUNTER_IPRO_PARENT(n), id),    \
	};                                                                                         \
	static struct counter_ipro_data counter_ipro_data_##n;                                     \
	DEVICE_DT_INST_DEFINE(n, counter_ipro_init, NULL, &counter_ipro_data_##n,                  \
			      &counter_ipro_cfg_##n, POST_KERNEL, CONFIG_COUNTER_INIT_PRIORITY,      \
			      &counter_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(COUNTER_IPRO_INIT)
