/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * LCD helpers over the Zephyr display API (ST7789P3 through spi_ipro +
 * mipi-dbi-spi on the EVK). Row-buffered writes: the panel is 240x320
 * RGB565, a full frame is 150 KB, so patterns are streamed a strip at a
 * time from a 4 KB scratch buffer.
 */
#include "demo.h"
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/byteorder.h>
#include <string.h>

static const struct device *const disp = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static const struct gpio_dt_spec bl = GPIO_DT_SPEC_GET(DT_NODELABEL(lcd_backlight), gpios);
static struct display_capabilities caps;
#define STRIP_PIXELS 4096 /* 8 KB: 12 rows of a 320-wide frame per SPI write */
static uint16_t strip[STRIP_PIXELS];
static bool big_endian_pixels;

int lcd_init(void)
{
	if (!device_is_ready(disp)) {
		return -ENODEV;
	}
	display_get_capabilities(disp, &caps);
	/* PIXEL_FORMAT_RGB_565 is big-endian on the wire; RGB_565X little-endian. */
	big_endian_pixels = (caps.current_pixel_format & PIXEL_FORMAT_RGB_565X) == 0;
	(void)display_blanking_off(disp);
	(void)lcd_backlight(true);
	return 0;
}

uint16_t lcd_width(void) { return caps.x_resolution; }
uint16_t lcd_height(void) { return caps.y_resolution; }

int lcd_backlight(bool on)
{
	if (!gpio_is_ready_dt(&bl)) {
		return -ENODEV;
	}
	return gpio_pin_configure_dt(&bl, on ? GPIO_OUTPUT_ACTIVE : GPIO_OUTPUT_INACTIVE);
}

static inline uint16_t wire(uint16_t c)
{
	return big_endian_pixels ? sys_cpu_to_be16(c) : c;
}

int lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	uint16_t rows = STRIP_PIXELS / w;
	struct display_buffer_descriptor d = {.width = w, .pitch = w, .frame_incomplete = true};
	uint16_t c = wire(color);

	if (w == 0 || rows == 0) {
		return -EINVAL;
	}
	for (int i = 0; i < w * rows; i++) {
		strip[i] = c;
	}
	for (uint16_t yy = 0; yy < h; yy += rows) {
		uint16_t n = MIN(rows, h - yy);

		d.height = n;
		d.buf_size = n * w * 2;
		d.frame_incomplete = (yy + n) < h;
		int r = display_write(disp, x, y + yy, &d, strip);

		if (r != 0) {
			return r;
		}
	}
	return 0;
}

int lcd_fill(uint16_t color)
{
	return lcd_fill_rect(0, 0, caps.x_resolution, caps.y_resolution, color);
}

int lcd_bars(void)
{
	static const uint16_t cols[8] = {C_WHITE, C_YELLOW, C_CYAN, C_GREEN,
					 C_MAGENTA, C_RED, C_BLUE, C_BLACK};
	uint16_t bw = caps.x_resolution / 8;

	for (int i = 0; i < 8; i++) {
		int r = lcd_fill_rect(i * bw, 0, (i == 7) ? caps.x_resolution - 7 * bw : bw,
				      caps.y_resolution, cols[i]);
		if (r != 0) {
			return r;
		}
	}
	return 0;
}

int lcd_checker(uint16_t cell)
{
	uint16_t w = caps.x_resolution;
	struct display_buffer_descriptor d = {.width = w, .pitch = w, .height = 1, .buf_size = w * 2};

	if (cell == 0) {
		cell = 16;
	}
	for (uint16_t y = 0; y < caps.y_resolution; y++) {
		for (uint16_t x = 0; x < w; x++) {
			strip[x] = wire((((x / cell) + (y / cell)) & 1) ? C_WHITE : C_BLACK);
		}
		d.frame_incomplete = (y + 1) < caps.y_resolution;
		int r = display_write(disp, 0, y, &d, strip);

		if (r != 0) {
			return r;
		}
	}
	return 0;
}

int lcd_gradient(void)
{
	uint16_t w = caps.x_resolution;
	struct display_buffer_descriptor d = {.width = w, .pitch = w, .height = 1, .buf_size = w * 2};

	for (uint16_t y = 0; y < caps.y_resolution; y++) {
		uint8_t g = (uint8_t)(y * 255 / (caps.y_resolution - 1));

		for (uint16_t x = 0; x < w; x++) {
			uint8_t rr = (uint8_t)(x * 255 / (w - 1));

			strip[x] = wire(RGB565(rr, g, 255 - rr));
		}
		d.frame_incomplete = (y + 1) < caps.y_resolution;
		int r = display_write(disp, 0, y, &d, strip);

		if (r != 0) {
			return r;
		}
	}
	return 0;
}

int lcd_blit(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *rgb565)
{
	uint16_t rows = STRIP_PIXELS / w;
	struct display_buffer_descriptor d = {.width = w, .pitch = w};

	if (w == 0 || rows == 0) {
		return -EINVAL;
	}
	for (uint16_t yy = 0; yy < h; yy += rows) {
		uint16_t n = MIN(rows, h - yy);
		const uint16_t *src = rgb565 + (size_t)yy * w;

		for (size_t i = 0; i < (size_t)n * w; i++) {
			strip[i] = wire(src[i]);
		}
		d.height = n;
		d.buf_size = n * w * 2;
		d.frame_incomplete = (yy + n) < h;
		int r = display_write(disp, x, y + yy, &d, strip);

		if (r != 0) {
			return r;
		}
	}
	return 0;
}

int lcd_level_bar(uint16_t permille, uint16_t color)
{
	uint16_t h = 12, y = caps.y_resolution - h;
	uint16_t lit = (uint32_t)caps.x_resolution * MIN(permille, 1000) / 1000;
	int r = lcd_fill_rect(0, y, lit ? lit : 1, h, lit ? color : C_BLACK);

	if (r == 0 && lit < caps.x_resolution) {
		r = lcd_fill_rect(lit, y, caps.x_resolution - lit, h, C_BLACK);
	}
	return r;
}

/* Raw strip write: pixels already in the panel's wire byte order. */
int lcd_write_raw(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *wire_pixels,
		  bool last)
{
	struct display_buffer_descriptor d = {
		.width = w, .pitch = w, .height = h, .buf_size = (uint32_t)w * h * 2,
		.frame_incomplete = !last,
	};
	return display_write(disp, x, y, &d, wire_pixels);
}

bool lcd_big_endian(void)
{
	return big_endian_pixels;
}
