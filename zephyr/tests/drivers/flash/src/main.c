/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * flash_ipro on the storage partition: page layout, range/alignment checks,
 * erase -> 0xFF, byte-granular write (write-block-size 1), cross-page write,
 * and that writes never touch the running image.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <string.h>

static const struct device *const flash = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));
#define STORAGE_OFF  DT_REG_ADDR(DT_NODELABEL(storage_partition))
#define STORAGE_SIZE DT_REG_SIZE(DT_NODELABEL(storage_partition))
#define PAGE 4096

ZTEST(ipro_flash, test_ready_and_layout)
{
	struct flash_pages_info info;
	const struct flash_parameters *p = flash_get_parameters(flash);

	zassert_true(device_is_ready(flash));
	zassert_equal(p->write_block_size, 1);
	zassert_equal(p->erase_value, 0xff);
	zassert_ok(flash_get_page_info_by_offs(flash, STORAGE_OFF, &info));
	zassert_equal(info.size, PAGE);
	zassert_equal(info.start_offset, STORAGE_OFF);
	zassert_equal(flash_get_page_count(flash), DT_REG_SIZE(DT_NODELABEL(flash0)) / PAGE);
}

ZTEST(ipro_flash, test_range_and_alignment)
{
	uint8_t b[4];
	size_t total = DT_REG_SIZE(DT_NODELABEL(flash0));

	zassert_equal(flash_read(flash, total, b, 1), -EINVAL, "past end");
	zassert_equal(flash_read(flash, total - 2, b, 4), -EINVAL, "straddles end");
	zassert_equal(flash_erase(flash, STORAGE_OFF + 1, PAGE), -EINVAL, "unaligned erase");
	zassert_equal(flash_erase(flash, STORAGE_OFF, PAGE - 1), -EINVAL, "partial page");
	zassert_ok(flash_read(flash, STORAGE_OFF, b, 0), "zero-length read is a no-op");
}

ZTEST(ipro_flash, test_erase_write_read)
{
	static uint8_t buf[PAGE + 64];
	uint8_t back[64];

	zassert_ok(flash_erase(flash, STORAGE_OFF, 2 * PAGE));
	zassert_ok(flash_read(flash, STORAGE_OFF, back, sizeof(back)));
	for (int i = 0; i < 64; i++) {
		zassert_equal(back[i], 0xff, "erased byte %d = %02x", i, back[i]);
	}

	for (size_t i = 0; i < sizeof(buf); i++) {
		buf[i] = (uint8_t)(i * 31 + 7);
	}
	/* byte-granular and unaligned start, crossing the page boundary */
	zassert_ok(flash_write(flash, STORAGE_OFF + 3, buf, sizeof(buf)));
	for (size_t off = 0; off < sizeof(buf); off += 64) {
		size_t n = MIN(64, sizeof(buf) - off);

		zassert_ok(flash_read(flash, STORAGE_OFF + 3 + off, back, n));
		zassert_mem_equal(back, buf + off, n, "mismatch at +%zu", off);
	}
	zassert_ok(flash_erase(flash, STORAGE_OFF, 2 * PAGE));
}

ZTEST(ipro_flash, test_flash_map_area)
{
	const struct flash_area *fa;
	uint8_t b[16];

	zassert_ok(flash_area_open(PARTITION_ID(storage_partition), &fa));
	zassert_equal(fa->fa_off, STORAGE_OFF);
	zassert_equal(fa->fa_size, STORAGE_SIZE);
	zassert_ok(flash_area_read(fa, 0, b, sizeof(b)));
	flash_area_close(fa);
}

ZTEST(ipro_flash, test_running_image_is_readable)
{
	uint32_t first;

	/* image-0 begins with the entry vector; it must read back as code,
	 * i.e. not erased flash */
	zassert_ok(flash_read(flash, DT_REG_ADDR(DT_NODELABEL(slot0_partition)), &first, 4));
	zassert_not_equal(first, 0xffffffff);
}

ZTEST_SUITE(ipro_flash, NULL, NULL, NULL, NULL, NULL);
