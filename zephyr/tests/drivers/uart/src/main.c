/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * uart_ipro on the console UART: runtime configure/config_get round trip,
 * rejected formats, poll-out, and the interrupt-driven TX path.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/uart.h>

static const struct device *const uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

ZTEST(ipro_uart, test_ready)
{
	zassert_true(device_is_ready(uart));
}

ZTEST(ipro_uart, test_config_roundtrip)
{
	struct uart_config cfg, back;

	zassert_ok(uart_config_get(uart, &cfg));
	zassert_equal(cfg.baudrate, DT_PROP(DT_CHOSEN(zephyr_console), current_speed));
	zassert_equal(cfg.data_bits, UART_CFG_DATA_BITS_8);
	zassert_equal(cfg.parity, UART_CFG_PARITY_NONE);

	/* re-apply the same configuration: must succeed and read back equal */
	zassert_ok(uart_configure(uart, &cfg));
	zassert_ok(uart_config_get(uart, &back));
	zassert_equal(back.baudrate, cfg.baudrate);
	zassert_equal(back.stop_bits, cfg.stop_bits);
	zassert_equal(back.flow_ctrl, cfg.flow_ctrl);
}

ZTEST(ipro_uart, test_rejected_formats)
{
	struct uart_config cfg;

	zassert_ok(uart_config_get(uart, &cfg));

	cfg.data_bits = UART_CFG_DATA_BITS_9;
	zassert_equal(uart_configure(uart, &cfg), -ENOTSUP);
	zassert_ok(uart_config_get(uart, &cfg));

	cfg.parity = UART_CFG_PARITY_MARK;
	zassert_equal(uart_configure(uart, &cfg), -ENOTSUP);
	zassert_ok(uart_config_get(uart, &cfg));

	cfg.flow_ctrl = UART_CFG_FLOW_CTRL_DTR_DSR;
	zassert_equal(uart_configure(uart, &cfg), -ENOTSUP);
}

ZTEST(ipro_uart, test_poll_out)
{
	const char msg[] = "\r\n[ipro_uart poll_out ok]\r\n";

	for (size_t i = 0; i < sizeof(msg) - 1; i++) {
		uart_poll_out(uart, msg[i]);
	}
}

static const char irq_msg[] = "[ipro_uart irq tx ok]\r\n";
static volatile size_t irq_pos;
static K_SEM_DEFINE(tx_done, 0, 1);

static void tx_isr(const struct device *dev, void *ud)
{
	ARG_UNUSED(ud);
	if (!uart_irq_update(dev)) {
		return;
	}
	while (uart_irq_tx_ready(dev) && irq_pos < sizeof(irq_msg) - 1) {
		irq_pos += uart_fifo_fill(dev, (const uint8_t *)irq_msg + irq_pos,
					  sizeof(irq_msg) - 1 - irq_pos);
	}
	if (irq_pos >= sizeof(irq_msg) - 1) {
		uart_irq_tx_disable(dev);
		k_sem_give(&tx_done);
	}
}

ZTEST(ipro_uart, test_irq_tx)
{
	irq_pos = 0;
	uart_irq_callback_user_data_set(uart, tx_isr, NULL);
	uart_irq_tx_enable(uart);
	zassert_ok(k_sem_take(&tx_done, K_MSEC(500)), "TX interrupt never drained the message");
	zassert_equal(irq_pos, sizeof(irq_msg) - 1);
	k_msleep(20);
}

ZTEST_SUITE(ipro_uart, NULL, NULL, NULL, NULL, NULL);
