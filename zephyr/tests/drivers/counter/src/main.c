/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * counter_ipro (TIMER0 channel at 1 MHz): frequency, monotonic count,
 * relative and absolute alarms, alarm slot bookkeeping, top value.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/counter.h>

static const struct device *const cnt = DEVICE_DT_GET(DT_NODELABEL(counter0));
static K_SEM_DEFINE(alarm_sem, 0, 1);
static uint32_t alarm_ticks;

static void on_alarm(const struct device *dev, uint8_t chan, uint32_t ticks, void *ud)
{
	ARG_UNUSED(dev); ARG_UNUSED(chan); ARG_UNUSED(ud);
	alarm_ticks = ticks;
	k_sem_give(&alarm_sem);
}

ZTEST(ipro_counter, test_freq_and_running)
{
	uint32_t a, b;

	zassert_true(device_is_ready(cnt));
	zassert_equal(counter_get_frequency(cnt), 1000000, "TIMER0 runs from XTAL/32... = 1 MHz");
	zassert_equal(counter_get_num_of_channels(cnt), 2);
	zassert_ok(counter_start(cnt));
	zassert_ok(counter_get_value(cnt, &a));
	k_busy_wait(1000);
	zassert_ok(counter_get_value(cnt, &b));
	uint32_t d = counter_ticks_to_us(cnt, b - a);

	zassert_true(d >= 900 && d <= 1500, "1 ms busy wait measured as %u us", d);
}

ZTEST(ipro_counter, test_relative_alarm)
{
	struct counter_alarm_cfg cfg = {.callback = on_alarm, .ticks = counter_us_to_ticks(cnt, 5000)};

	k_sem_reset(&alarm_sem);
	zassert_ok(counter_set_channel_alarm(cnt, 0, &cfg));
	zassert_equal(counter_set_channel_alarm(cnt, 0, &cfg), -EBUSY, "slot in use");
	zassert_ok(k_sem_take(&alarm_sem, K_MSEC(100)), "alarm did not fire");
	zassert_ok(counter_cancel_channel_alarm(cnt, 0));
}

ZTEST(ipro_counter, test_absolute_alarm_and_range)
{
	uint32_t now;
	struct counter_alarm_cfg cfg = {.callback = on_alarm, .flags = COUNTER_ALARM_CFG_ABSOLUTE};

	zassert_ok(counter_get_value(cnt, &now));
	cfg.ticks = now + counter_us_to_ticks(cnt, 3000);
	k_sem_reset(&alarm_sem);
	zassert_ok(counter_set_channel_alarm(cnt, 1, &cfg));
	zassert_ok(k_sem_take(&alarm_sem, K_MSEC(100)));
	zassert_ok(counter_cancel_channel_alarm(cnt, 1));

	/* counter.h rejects an unknown channel itself, before the driver */
	zassert_equal(counter_set_channel_alarm(cnt, 2, &cfg), -ENOTSUP, "only two alarms");
	/* top is UINT32_MAX (max_top_value): "beyond top" is unrepresentable
	 * on a 32-bit tick, so the -EINVAL path is covered by test_top_value */
}

ZTEST(ipro_counter, test_top_value)
{
	struct counter_top_cfg top = {.ticks = 0};

	zassert_equal(counter_set_top_value(cnt, &top), -EINVAL);
	top.ticks = 1000000;
	zassert_ok(counter_set_top_value(cnt, &top));
	zassert_equal(counter_get_top_value(cnt), 1000000);
	top.ticks = counter_get_max_top_value(cnt);
	zassert_ok(counter_set_top_value(cnt, &top));
	zassert_ok(counter_stop(cnt));
}

static void *counter_suite_setup(void)
{
	/* ztest runs cases alphabetically; the alarms need a running counter. */
	counter_start(cnt);
	return NULL;
}

ZTEST_SUITE(ipro_counter, NULL, counter_suite_setup, NULL, NULL, NULL);
