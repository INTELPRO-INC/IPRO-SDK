/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * uart_ipro async API (DMA) on the console UART. The host side of the RX
 * tests is scripted: when the board prints "RX ARMED" the host sends
 * "hello-async\n", when it prints "RX2 ARMED" it sends "abcdefghijkl".
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

static const struct device *const uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

static K_SEM_DEFINE(tx_sem, 0, 1);
static K_SEM_DEFINE(rx_sem, 0, 4);
static K_SEM_DEFINE(rx_dis_sem, 0, 1);
static struct uart_event last_tx;
static uint8_t rx_data[128];
static size_t rx_total;
static int buf_requests, buf_released, tx_aborted;
static uint8_t rx_buf_a[8], rx_buf_b[8], rx_buf_big[64];
static bool give_second_buffer;

static void cb(const struct device *dev, struct uart_event *evt, void *user)
{
	ARG_UNUSED(dev); ARG_UNUSED(user);
	switch (evt->type) {
	case UART_TX_DONE:
		last_tx = *evt;
		k_sem_give(&tx_sem);
		break;
	case UART_TX_ABORTED:
		last_tx = *evt;
		tx_aborted++;
		k_sem_give(&tx_sem);
		break;
	case UART_RX_RDY:
		if (rx_total + evt->data.rx.len <= sizeof(rx_data)) {
			memcpy(rx_data + rx_total, evt->data.rx.buf + evt->data.rx.offset, evt->data.rx.len);
		}
		rx_total += evt->data.rx.len;
		k_sem_give(&rx_sem);
		break;
	case UART_RX_BUF_REQUEST:
		buf_requests++;
		if (give_second_buffer) {
			give_second_buffer = false;
			(void)uart_rx_buf_rsp(dev, rx_buf_b, sizeof(rx_buf_b));
		}
		break;
	case UART_RX_BUF_RELEASED:
		buf_released++;
		break;
	case UART_RX_DISABLED:
		k_sem_give(&rx_dis_sem);
		break;
	default:
		break;
	}
}

static void *setup(void)
{
	zassert_true(device_is_ready(uart));
	zassert_ok(uart_callback_set(uart, cb, NULL));
	return NULL;
}

ZTEST(ipro_uart_async, test_a_tx)
{
	static uint8_t msg[200];
	int64_t t0;

	for (int i = 0; i < sizeof(msg) - 2; i++) {
		msg[i] = 'A' + (i % 26);
	}
	msg[sizeof(msg) - 2] = '\r';
	msg[sizeof(msg) - 1] = '\n';
	k_sem_reset(&tx_sem);
	t0 = k_uptime_get();
	zassert_ok(uart_tx(uart, msg, sizeof(msg), 100000));
	zassert_equal(uart_tx(uart, msg, sizeof(msg), 100000), -EBUSY, "second tx must be refused");
	zassert_ok(k_sem_take(&tx_sem, K_MSEC(200)), "no TX_DONE");
	zassert_equal(last_tx.type, UART_TX_DONE);
	zassert_equal(last_tx.data.tx.len, sizeof(msg));
	/* 200 bytes at 2 Mbaud = 1 ms */
	zassert_true(k_uptime_get() - t0 < 20, "TX took too long");
	k_msleep(5);
	printk("\nasync tx: 200 bytes done\n");
}

ZTEST(ipro_uart_async, test_b_tx_abort)
{
	static uint8_t big[4000];

	memset(big, '.', sizeof(big));
	k_sem_reset(&tx_sem);
	tx_aborted = 0;
	zassert_ok(uart_tx(uart, big, sizeof(big), SYS_FOREVER_US));
	k_busy_wait(2000); /* ~400 bytes out at 2 Mbaud */
	zassert_ok(uart_tx_abort(uart));
	zassert_ok(k_sem_take(&tx_sem, K_MSEC(100)), "no TX_ABORTED");
	zassert_equal(tx_aborted, 1);
	zassert_true(last_tx.data.tx.len > 0 && last_tx.data.tx.len < sizeof(big),
		     "aborted after %u of %u", (unsigned)last_tx.data.tx.len, (unsigned)sizeof(big));
	zassert_equal(uart_tx_abort(uart), -EFAULT, "nothing to abort");
	k_msleep(20); /* drain the FIFO before printing */
	printk("\nasync tx abort: %u of %u sent\n", (unsigned)last_tx.data.tx.len, (unsigned)sizeof(big));

	/* Timeout path: a huge transfer with a short timeout aborts by itself. */
	k_sem_reset(&tx_sem);
	zassert_ok(uart_tx(uart, big, sizeof(big), 1000));
	zassert_ok(k_sem_take(&tx_sem, K_MSEC(100)), "no timeout abort");
	zassert_equal(last_tx.type, UART_TX_ABORTED);
	k_msleep(20);
	printk("\n");
}

ZTEST(ipro_uart_async, test_c_rx)
{
	rx_total = 0;
	buf_requests = 0;
	buf_released = 0;
	k_sem_reset(&rx_sem);
	k_sem_reset(&rx_dis_sem);
	zassert_ok(uart_rx_enable(uart, rx_buf_big, sizeof(rx_buf_big), 10000));
	zassert_equal(uart_rx_enable(uart, rx_buf_big, sizeof(rx_buf_big), 10000), -EBUSY);
	zassert_equal(buf_requests, 1, "BUF_REQUEST expected right after enable");
	printk("RX ARMED\n");
	/* Host sends "hello-async\n" (12 bytes); the RTO interrupt reports it. */
	zassert_ok(k_sem_take(&rx_sem, K_SECONDS(15)), "no RX_RDY (host did not send?)");
	k_msleep(20);
	while (k_sem_take(&rx_sem, K_NO_WAIT) == 0) {
	}
	rx_data[MIN(rx_total, sizeof(rx_data) - 1)] = 0;
	printk("rx got %u bytes: '%s'\n", (unsigned)rx_total, rx_data);
	zassert_equal(rx_total, 12, "expected 12 bytes, got %u", (unsigned)rx_total);
	zassert_mem_equal(rx_data, "hello-async\n", 12);

	zassert_ok(uart_rx_disable(uart));
	zassert_ok(k_sem_take(&rx_dis_sem, K_MSEC(100)), "no RX_DISABLED");
	zassert_equal(buf_released, 1);
	zassert_equal(uart_rx_disable(uart), -EFAULT);
}

ZTEST(ipro_uart_async, test_d_rx_buffer_switch)
{
	rx_total = 0;
	buf_requests = 0;
	buf_released = 0;
	give_second_buffer = true;
	k_sem_reset(&rx_sem);
	k_sem_reset(&rx_dis_sem);
	zassert_ok(uart_rx_enable(uart, rx_buf_a, sizeof(rx_buf_a), 10000));
	printk("RX2 ARMED\n");
	/* Host sends "abcdefghijkl" (12 bytes): 8 fill buffer A (DMA complete ->
	 * RDY + RELEASED + switch), 4 land in B (RTO -> RDY). */
	zassert_ok(k_sem_take(&rx_sem, K_SECONDS(15)), "no RX_RDY");
	k_msleep(50);
	while (k_sem_take(&rx_sem, K_NO_WAIT) == 0) {
	}
	rx_data[MIN(rx_total, sizeof(rx_data) - 1)] = 0;
	printk("rx2 got %u bytes: '%s' (requests %d, released %d)\n", (unsigned)rx_total, rx_data,
	       buf_requests, buf_released);
	zassert_equal(rx_total, 12, "expected 12 bytes, got %u", (unsigned)rx_total);
	zassert_mem_equal(rx_data, "abcdefghijkl", 12);
	zassert_equal(buf_released, 1, "buffer A must have been released");
	zassert_equal(buf_requests, 2, "a request per buffer");

	zassert_ok(uart_rx_disable(uart));
	zassert_ok(k_sem_take(&rx_dis_sem, K_MSEC(100)));
	zassert_equal(buf_released, 2, "buffer B released on disable");
}

ZTEST_SUITE(ipro_uart_async, NULL, setup, NULL, NULL, NULL);
