/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * i2c_ipro: speed/mode validation, bounded NACK handling (a transfer to an
 * unpopulated address must fail within the timeout, not hang), and a bus
 * scan report.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/i2c.h>

static const struct device *const i2c = DEVICE_DT_GET(DT_NODELABEL(i2c0));

ZTEST(ipro_i2c, test_ready)
{
	zassert_true(device_is_ready(i2c));
}

ZTEST(ipro_i2c, test_configure)
{
	zassert_ok(i2c_configure(i2c, I2C_MODE_CONTROLLER | I2C_SPEED_SET(I2C_SPEED_STANDARD)));
	zassert_ok(i2c_configure(i2c, I2C_MODE_CONTROLLER | I2C_SPEED_SET(I2C_SPEED_FAST)));
	zassert_equal(i2c_configure(i2c, I2C_MODE_CONTROLLER | I2C_SPEED_SET(I2C_SPEED_FAST_PLUS)),
		      -ENOTSUP);
	zassert_equal(i2c_configure(i2c, I2C_SPEED_SET(I2C_SPEED_STANDARD)), -ENOTSUP,
		      "target mode is not implemented");
	zassert_ok(i2c_configure(i2c, I2C_MODE_CONTROLLER | I2C_SPEED_SET(I2C_SPEED_FAST)));
}

ZTEST(ipro_i2c, test_ten_bit_address_rejected)
{
	uint8_t b = 0;
	struct i2c_msg m = {.buf = &b, .len = 1, .flags = I2C_MSG_WRITE | I2C_MSG_STOP};

	zassert_equal(i2c_transfer(i2c, &m, 1, 0x1ff), -ENOTSUP);
}

ZTEST(ipro_i2c, test_nack_is_bounded)
{
	uint8_t b = 0;
	struct i2c_msg m = {.buf = &b, .len = 1, .flags = I2C_MSG_READ | I2C_MSG_STOP};
	int64_t t0 = k_uptime_get();
	/* 0x7e is reserved (10-bit prefix) so nothing legitimate answers. */
	int rc = i2c_transfer(i2c, &m, 1, 0x7e);
	int64_t dt = k_uptime_get() - t0;

	zassert_not_equal(rc, 0, "read from an empty address must fail");
	zassert_true(dt < 2 * (CONFIG_I2C_IPRO_TIMEOUT_US / 1000) + 50,
		     "NACK took %lld ms, timeout is %u us", dt, CONFIG_I2C_IPRO_TIMEOUT_US);
}

ZTEST(ipro_i2c, test_scan_report)
{
	int found = 0;

	for (uint16_t addr = 0x08; addr < 0x78; addr++) {
		uint8_t d;
		struct i2c_msg m = {.buf = &d, .len = 0, .flags = I2C_MSG_WRITE | I2C_MSG_STOP};

		if (i2c_transfer(i2c, &m, 1, addr) == 0) {
			TC_PRINT("device at 0x%02x\n", addr);
			found++;
		}
	}
	TC_PRINT("scan: %d device(s)\n", found);
}

ZTEST_SUITE(ipro_i2c, NULL, NULL, NULL, NULL, NULL);
