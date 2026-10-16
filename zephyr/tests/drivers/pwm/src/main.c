/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * pwm_ipro: the PWM block has one 16-bit period shared by its four channels.
 * Tests the period bookkeeping (second channel must match, may change once
 * the first is released), range checks, and cycles-per-sec reporting.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/pwm.h>

static const struct device *const pwm = DEVICE_DT_GET(DT_NODELABEL(pwm0));

ZTEST(ipro_pwm, test_ready)
{
	zassert_true(device_is_ready(pwm));
}

ZTEST(ipro_pwm, test_cycles_per_sec)
{
	uint64_t hz = 0;

	zassert_ok(pwm_get_cycles_per_sec(pwm, 0, &hz));
	zassert_true(hz > 0 && hz <= 80000000ULL, "clock %llu Hz", hz);
	zassert_equal(pwm_get_cycles_per_sec(pwm, 4, &hz), -EINVAL);
}

ZTEST(ipro_pwm, test_range_checks)
{
	zassert_equal(pwm_set_cycles(pwm, 4, 100, 50, 0), -EINVAL, "channel");
	zassert_equal(pwm_set_cycles(pwm, 0, 0, 0, 0), -ENOTSUP, "zero period");
	zassert_equal(pwm_set_cycles(pwm, 0, 0x10000, 1, 0), -ENOTSUP, "17-bit period");
	zassert_equal(pwm_set_cycles(pwm, 0, 100, 101, 0), -EINVAL, "pulse > period");
}

ZTEST(ipro_pwm, test_shared_period)
{
	zassert_ok(pwm_set_cycles(pwm, 0, 1000, 250, 0));
	zassert_ok(pwm_set_cycles(pwm, 1, 1000, 500, 0), "same period on 2nd channel");
	zassert_equal(pwm_set_cycles(pwm, 1, 2000, 500, 0), -EBUSY,
		      "different period while channel 0 is active");
	/* pulse 0 releases a channel; then the period may change */
	zassert_ok(pwm_set_cycles(pwm, 0, 1000, 0, 0));
	zassert_ok(pwm_set_cycles(pwm, 1, 1000, 0, 0));
	zassert_ok(pwm_set_cycles(pwm, 1, 2000, 100, PWM_POLARITY_INVERTED));
	zassert_ok(pwm_set_cycles(pwm, 1, 2000, 0, 0));
}

ZTEST(ipro_pwm, test_dt_spec_sweep)
{
#if DT_NODE_HAS_STATUS(DT_ALIAS(pwm_led0), okay)
	const struct pwm_dt_spec led = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

	zassert_true(pwm_is_ready_dt(&led));
	for (int i = 0; i <= 4; i++) {
		zassert_ok(pwm_set_pulse_dt(&led, led.period * i / 4));
		k_msleep(50);
	}
	zassert_ok(pwm_set_pulse_dt(&led, 0));
#else
	ztest_test_skip();
#endif
}

ZTEST_SUITE(ipro_pwm, NULL, NULL, NULL, NULL, NULL);
