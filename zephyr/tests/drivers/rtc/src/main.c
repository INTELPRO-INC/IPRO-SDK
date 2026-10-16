/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * counter_ipro_rtc (HBN RTC, 32 kHz): rate against the kernel clock,
 * monotonic 64-bit read, relative / absolute / late alarms, cancel, and the
 * documented -ENOTSUP paths (stop, top value).
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/counter.h>

static const struct device *const rtc = DEVICE_DT_GET(DT_NODELABEL(rtc0));
static K_SEM_DEFINE(alarm_sem, 0, 1);
static uint32_t alarm_ticks;
static int64_t alarm_uptime;

static void on_alarm(const struct device *dev, uint8_t chan, uint32_t ticks, void *ud)
{
	ARG_UNUSED(dev); ARG_UNUSED(chan); ARG_UNUSED(ud);
	alarm_ticks = ticks;
	alarm_uptime = k_uptime_get();
	k_sem_give(&alarm_sem);
}

ZTEST(ipro_rtc, test_rate)
{
	uint32_t a, b;
	uint64_t a64, b64;

	uint32_t f = counter_get_frequency(rtc);

	zassert_true(device_is_ready(rtc));
	/* Measured at init: RC32K within 10 % of nominal, XTAL32K exact. */
	zassert_true(f >= 29500 && f <= 36000, "RTC frequency %u", f);
	zassert_equal(counter_get_num_of_channels(rtc), 1);
	zassert_true(counter_is_counting_up(rtc));
	zassert_ok(counter_start(rtc));

	zassert_ok(counter_get_value_64(rtc, &a64));
	zassert_ok(counter_get_value(rtc, &a));
	k_busy_wait(100000);
	zassert_ok(counter_get_value(rtc, &b));
	zassert_ok(counter_get_value_64(rtc, &b64));

	uint32_t d = b - a;

	/* 100 ms at the measured rate, +-1 % (busy-wait runs on the crystal). */
	uint32_t expect = f / 10;

	zassert_true(d >= expect - expect / 100 && d <= expect + expect / 100,
		     "100 ms busy wait measured as %u RTC ticks, expected %u", d, expect);
	zassert_true(b64 > a64 && (b64 - a64) < 4000, "64-bit read not monotonic");
	zassert_equal((uint32_t)b64 >= b, true, "64/32-bit views disagree");
}

ZTEST(ipro_rtc, test_relative_alarm)
{
	struct counter_alarm_cfg cfg = {.callback = on_alarm, .ticks = counter_us_to_ticks(rtc, 50000)};
	int64_t t0 = k_uptime_get();

	k_sem_reset(&alarm_sem);
	zassert_ok(counter_set_channel_alarm(rtc, 0, &cfg));
	zassert_equal(counter_set_channel_alarm(rtc, 0, &cfg), -EBUSY, "second alarm must be refused");
	zassert_ok(k_sem_take(&alarm_sem, K_MSEC(200)), "alarm did not fire");

	int64_t dt = alarm_uptime - t0;

	zassert_true(dt >= 45 && dt <= 70, "50 ms alarm fired after %lld ms", dt);
	/* After the callback the channel is free again. */
	zassert_ok(counter_set_channel_alarm(rtc, 0, &cfg));
	zassert_ok(counter_cancel_channel_alarm(rtc, 0));
}

ZTEST(ipro_rtc, test_absolute_alarm)
{
	uint32_t now;
	struct counter_alarm_cfg cfg = {.callback = on_alarm, .flags = COUNTER_ALARM_CFG_ABSOLUTE};

	zassert_ok(counter_get_value(rtc, &now));
	cfg.ticks = now + counter_us_to_ticks(rtc, 100000);
	k_sem_reset(&alarm_sem);
	zassert_ok(counter_set_channel_alarm(rtc, 0, &cfg));
	zassert_ok(k_sem_take(&alarm_sem, K_MSEC(300)), "absolute alarm did not fire");
	zassert_true(alarm_ticks >= cfg.ticks && alarm_ticks - cfg.ticks < 64,
		     "fired at %u for target %u", alarm_ticks, cfg.ticks);

	/* A target already in the past with EXPIRE_WHEN_LATE fires at once. */
	zassert_ok(counter_get_value(rtc, &now));
	cfg.ticks = now - 1000;
	cfg.flags = COUNTER_ALARM_CFG_ABSOLUTE | COUNTER_ALARM_CFG_EXPIRE_WHEN_LATE;
	k_sem_reset(&alarm_sem);
	zassert_ok(counter_set_channel_alarm(rtc, 0, &cfg));
	zassert_ok(k_sem_take(&alarm_sem, K_MSEC(20)), "late alarm did not expire immediately");
}

ZTEST(ipro_rtc, test_cancel)
{
	struct counter_alarm_cfg cfg = {.callback = on_alarm, .ticks = counter_us_to_ticks(rtc, 50000)};

	k_sem_reset(&alarm_sem);
	zassert_ok(counter_set_channel_alarm(rtc, 0, &cfg));
	zassert_ok(counter_cancel_channel_alarm(rtc, 0));
	zassert_equal(k_sem_take(&alarm_sem, K_MSEC(120)), -EAGAIN, "cancelled alarm fired");
	zassert_equal(counter_get_pending_int(rtc), 0);
}

ZTEST(ipro_rtc, test_unsupported)
{
	struct counter_top_cfg top = {.ticks = 1000};

	zassert_equal(counter_stop(rtc), -ENOTSUP);
	zassert_equal(counter_set_top_value(rtc, &top), -ENOTSUP);
	zassert_equal(counter_get_top_value(rtc), UINT32_MAX);
	zassert_equal(counter_set_channel_alarm(rtc, 1, NULL), -ENOTSUP, "API rejects channel 1");
}

ZTEST_SUITE(ipro_rtc, NULL, NULL, NULL, NULL, NULL);
