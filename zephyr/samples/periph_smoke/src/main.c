/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Exercises every IPRO7 Zephyr driver once and prints PASS/FAIL per block.
 * It is a build-coverage and bench bring-up tool, not a unit test.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/drivers/entropy.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/dt-bindings/pwm/pwm.h>
#include <string.h>

#define RESULT(name, ok) printk("[%s] %s\n", (ok) ? "PASS" : "FAIL", name)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct pwm_dt_spec pwm_led = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

static void test_gpio(void)
{
	bool ok = gpio_is_ready_dt(&led) && gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) == 0;

	for (int i = 0; ok && i < 4; i++) {
		ok = gpio_pin_toggle_dt(&led) == 0;
		k_msleep(50);
	}
	RESULT("gpio", ok);
}

static volatile int uart_rx_chars;

static void uart_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	ARG_UNUSED(user_data);
	if (!uart_irq_update(dev)) {
		return;
	}
	while (uart_irq_rx_ready(dev) && uart_fifo_read(dev, &c, 1) == 1) {
		uart_rx_chars++;
		uart_poll_out(dev, c);
	}
}

static void test_uart_irq(void)
{
	const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	bool ok = device_is_ready(uart);

#if DT_HAS_CHOSEN(zephyr_shell_uart) && defined(CONFIG_SHELL_BACKEND_SERIAL)
	if (uart == DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart))) {
		/* The shell owns this UART's interrupt callback; taking it over
		 * would leave the console without a command line. The shell
		 * itself is the interrupt-driven RX/TX test here: type "help". */
		RESULT("uart-irq (shell backend owns the console, type 'help')", ok);
		return;
	}
#endif
	if (ok) {
		uart_irq_callback_set(uart, uart_cb);
		uart_irq_rx_enable(uart);
	}
	RESULT("uart-irq (echo armed, type to test)", ok);
}

static void test_spi(void)
{
	const struct device *spi = DEVICE_DT_GET(DT_NODELABEL(spi0));
	struct spi_config cfg = {
		.frequency = 1000000,
		.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	};
	uint8_t tx[8] = {0xA5, 0x5A, 1, 2, 3, 4, 5, 6};
	uint8_t rx[8] = {0};
	const struct spi_buf txb = {.buf = tx, .len = sizeof(tx)};
	const struct spi_buf rxb = {.buf = rx, .len = sizeof(rx)};
	const struct spi_buf_set txs = {.buffers = &txb, .count = 1};
	const struct spi_buf_set rxs = {.buffers = &rxb, .count = 1};
	bool ok = device_is_ready(spi) && spi_transceive(spi, &cfg, &txs, &rxs) == 0;

	printk("spi rx: %02x %02x %02x ... (%s)\n", rx[0], rx[1], rx[2],
	       memcmp(tx, rx, sizeof(tx)) == 0 ? "loopback match" : "no loopback jumper");
	RESULT("spi transceive", ok);
}

static void test_i2c(void)
{
	const struct device *i2c = DEVICE_DT_GET(DT_NODELABEL(i2c0));
	bool ok = device_is_ready(i2c);
	int found = 0;

	for (uint16_t addr = 0x08; ok && addr < 0x78; addr++) {
		uint8_t dummy;
		struct i2c_msg msg = {.buf = &dummy, .len = 0, .flags = I2C_MSG_WRITE | I2C_MSG_STOP};

		if (i2c_transfer(i2c, &msg, 1, addr) == 0) {
			printk("i2c device at 0x%02x\n", addr);
			found++;
		}
	}
	printk("i2c scan: %d device(s)\n", found);
	RESULT("i2c scan", ok);
}

static void test_pwm(void)
{
	bool ok = pwm_is_ready_dt(&pwm_led);

	for (int i = 0; ok && i <= 4; i++) {
		ok = pwm_set_pulse_dt(&pwm_led, pwm_led.period * i / 4) == 0;
		k_msleep(100);
	}
	RESULT("pwm", ok);
}

static void test_flash(void)
{
	const struct device *flash = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));
	off_t off = DT_REG_ADDR(DT_NODELABEL(storage_partition));
	uint8_t wr[16], rd[16];
	bool ok = device_is_ready(flash);

	for (int i = 0; i < 16; i++) {
		wr[i] = (uint8_t)(0x10 + i);
	}
	ok = ok && flash_erase(flash, off, 4096) == 0;
	ok = ok && flash_read(flash, off, rd, sizeof(rd)) == 0 && rd[0] == 0xff;
	ok = ok && flash_write(flash, off, wr, sizeof(wr)) == 0;
	ok = ok && flash_read(flash, off, rd, sizeof(rd)) == 0 && memcmp(wr, rd, sizeof(rd)) == 0;
	RESULT("flash erase/write/read @storage", ok);
}

static K_SEM_DEFINE(dma_done, 0, 1);
static int dma_status;

static void dma_cb(const struct device *dev, void *user_data, uint32_t channel, int status)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);
	ARG_UNUSED(channel);
	dma_status = status;
	k_sem_give(&dma_done);
}

static void test_dma(void)
{
	const struct device *dma = DEVICE_DT_GET(DT_NODELABEL(dma0));
	static uint32_t src[64] __aligned(32);
	static uint32_t dst[64] __aligned(32);
	struct dma_block_config blk = {
		.source_address = (uint32_t)src,
		.dest_address = (uint32_t)dst,
		.block_size = sizeof(src),
	};
	struct dma_config cfg = {
		.channel_direction = MEMORY_TO_MEMORY,
		.source_data_size = 4,
		.dest_data_size = 4,
		.source_burst_length = 16,
		.dest_burst_length = 16,
		.block_count = 1,
		.head_block = &blk,
		.dma_callback = dma_cb,
	};
	bool ok = device_is_ready(dma);

	for (int i = 0; i < 64; i++) {
		src[i] = 0x1000 + i;
		dst[i] = 0;
	}
	int ch = ok ? dma_request_channel(dma, NULL) : -1;

	ok = ok && ch >= 0 && dma_config(dma, ch, &cfg) == 0 && dma_start(dma, ch) == 0;
	ok = ok && k_sem_take(&dma_done, K_MSEC(200)) == 0 && dma_status == DMA_STATUS_COMPLETE;
	ok = ok && memcmp(src, dst, sizeof(src)) == 0;
	if (ch >= 0) {
		dma_release_channel(dma, ch);
	}
	RESULT("dma mem2mem", ok);
}

static K_SEM_DEFINE(alarm_sem, 0, 1);

static void alarm_cb(const struct device *dev, uint8_t chan, uint32_t ticks, void *user_data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(chan);
	ARG_UNUSED(ticks);
	ARG_UNUSED(user_data);
	k_sem_give(&alarm_sem);
}

static void test_counter(void)
{
	const struct device *cnt = DEVICE_DT_GET(DT_NODELABEL(counter0));
	struct counter_alarm_cfg acfg = {
		.callback = alarm_cb,
		.ticks = counter_us_to_ticks(cnt, 20000),
	};
	bool ok = device_is_ready(cnt) && counter_start(cnt) == 0 &&
		  counter_set_channel_alarm(cnt, 0, &acfg) == 0;

	ok = ok && k_sem_take(&alarm_sem, K_MSEC(200)) == 0;
	RESULT("counter alarm (20 ms)", ok);
}

static void test_entropy(void)
{
	const struct device *rng = DEVICE_DT_GET(DT_NODELABEL(trng));
	uint8_t buf[16] = {0};
	bool ok = device_is_ready(rng) && entropy_get_entropy(rng, buf, sizeof(buf)) == 0;
	int zeros = 0;

	for (int i = 0; i < 16; i++) {
		zeros += buf[i] == 0;
	}
	printk("trng: %02x%02x%02x%02x...\n", buf[0], buf[1], buf[2], buf[3]);
	RESULT("entropy", ok && zeros < 8);
}

static void test_watchdog(void)
{
	const struct device *wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));
	struct wdt_timeout_cfg cfg = {
		.window = {.min = 0, .max = 2000},
		.flags = WDT_FLAG_RESET_SOC,
	};
	bool ok = device_is_ready(wdt) && wdt_install_timeout(wdt, &cfg) >= 0 &&
		  wdt_setup(wdt, 0) == 0;

	for (int i = 0; ok && i < 5; i++) {
		k_msleep(500);
		wdt_feed(wdt, 0);
	}
	ok = ok && wdt_disable(wdt) == 0;
	RESULT("watchdog feed/disable", ok);
}

int main(void)
{
	printk("\n=== IPRO7 Zephyr peripheral smoke test ===\n");
	test_gpio();
	test_uart_irq();
	test_pwm();
	test_spi();
	test_i2c();
	test_flash();
	test_dma();
	test_counter();
	test_entropy();
	test_watchdog();
	printk("=== done ===\n");
	return 0;
}
