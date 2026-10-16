/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 flash driver: Zephyr flash_driver_api -> ipro_hal_flash_* facade
 * -> SDK hal_flash.c.
 *
 * Offsets are PHYSICAL flash offsets (0 = start of the chip), exactly what
 * the SDK's flash_read/flash_write/flash_erase take. The flash node in the
 * device tree therefore describes the physical part (0 .. size), while the
 * firmware itself executes through the XIP window at 0x13000000 that boot2
 * mapped onto the active FW partition.
 *
 * XIP safety: hal_flash.c's write/erase/read paths are ATTR_TCM_SECTION
 * (placed in OCRAM by the SoC linker snippet), pause XIP with
 * XIP_SFlash_Opt_Enter() and run with interrupts globally disabled, so no
 * code is fetched from flash while the controller is busy. Zephyr threads
 * are serialised with a mutex on top.
 */

#define DT_DRV_COMPAT intelpro_ipro_flash_controller

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/logging/log.h>
#include <ipro_zephyr/ipro_hal_glue.h>

LOG_MODULE_REGISTER(flash_ipro, CONFIG_FLASH_LOG_LEVEL);

#define FLASH_NODE       DT_INST_CHILD(0, flash_0)
#define FLASH_IPRO_SIZE  DT_REG_SIZE(FLASH_NODE)
#define FLASH_IPRO_ERASE DT_PROP(FLASH_NODE, erase_block_size)
#define FLASH_IPRO_WRITE DT_PROP(FLASH_NODE, write_block_size)

struct flash_ipro_data {
	struct k_mutex lock;
	uint32_t size;
};

static const struct flash_parameters flash_ipro_parameters = {
	.write_block_size = FLASH_IPRO_WRITE,
	.erase_value = 0xff,
};

static bool flash_ipro_range_ok(const struct device *dev, off_t offset, size_t len)
{
	struct flash_ipro_data *data = dev->data;

	return offset >= 0 && len <= data->size && (size_t)offset <= data->size - len;
}

static int flash_ipro_read(const struct device *dev, off_t offset, void *buf, size_t len)
{
	struct flash_ipro_data *data = dev->data;
	int ret;

	if (len == 0) {
		return 0;
	}
	if (!flash_ipro_range_ok(dev, offset, len)) {
		return -EINVAL;
	}
	k_mutex_lock(&data->lock, K_FOREVER);
	ret = ipro_hal_flash_read((uint32_t)offset, buf, len) == 0 ? 0 : -EIO;
	k_mutex_unlock(&data->lock);
	return ret;
}

static int flash_ipro_write(const struct device *dev, off_t offset, const void *buf, size_t len)
{
	struct flash_ipro_data *data = dev->data;
	int ret;

	if (len == 0) {
		return 0;
	}
	if (!flash_ipro_range_ok(dev, offset, len)) {
		return -EINVAL;
	}
	k_mutex_lock(&data->lock, K_FOREVER);
	ret = ipro_hal_flash_write((uint32_t)offset, buf, len) == 0 ? 0 : -EIO;
	k_mutex_unlock(&data->lock);
	return ret;
}

static int flash_ipro_erase(const struct device *dev, off_t offset, size_t len)
{
	struct flash_ipro_data *data = dev->data;
	int ret;

	if (len == 0) {
		return 0;
	}
	if (!flash_ipro_range_ok(dev, offset, len) || (offset % FLASH_IPRO_ERASE) != 0 ||
	    (len % FLASH_IPRO_ERASE) != 0) {
		return -EINVAL;
	}
	k_mutex_lock(&data->lock, K_FOREVER);
	/* One sector per HAL call: hal_flash erases the whole range with
	 * interrupts disabled, and a slot-sized erase (hundreds of sectors,
	 * seconds) starves the BLE controller and the UART. Per sector the
	 * blackout is one sector-erase time. */
	ret = 0;
	for (size_t done = 0; done < len && ret == 0; done += FLASH_IPRO_ERASE) {
		ret = ipro_hal_flash_erase((uint32_t)offset + done, FLASH_IPRO_ERASE) == 0 ? 0 : -EIO;
		if (done + FLASH_IPRO_ERASE < len) {
			k_yield();
		}
	}
	k_mutex_unlock(&data->lock);
	return ret;
}

static const struct flash_parameters *flash_ipro_get_parameters(const struct device *dev)
{
	ARG_UNUSED(dev);
	return &flash_ipro_parameters;
}

#ifdef CONFIG_FLASH_PAGE_LAYOUT
static struct flash_pages_layout flash_ipro_layout = {
	.pages_count = FLASH_IPRO_SIZE / FLASH_IPRO_ERASE,
	.pages_size = FLASH_IPRO_ERASE,
};

static void flash_ipro_page_layout(const struct device *dev, const struct flash_pages_layout **layout,
				   size_t *layout_size)
{
	struct flash_ipro_data *data = dev->data;

	flash_ipro_layout.pages_count = data->size / FLASH_IPRO_ERASE;
	*layout = &flash_ipro_layout;
	*layout_size = 1;
}
#endif

static int flash_ipro_init(const struct device *dev)
{
	struct flash_ipro_data *data = dev->data;
	uint32_t detected;

	k_mutex_init(&data->lock);

	if (ipro_hal_flash_init() != 0) {
		LOG_WRN("boot2 parameters unavailable, flash config from detection");
	}

	detected = ipro_hal_flash_size();
	data->size = FLASH_IPRO_SIZE;
	if (detected != 0 && detected < FLASH_IPRO_SIZE) {
		LOG_WRN("device tree says %u KB, JEDEC says %u KB; using JEDEC",
			FLASH_IPRO_SIZE / 1024, detected / 1024);
		data->size = detected;
	}
	return 0;
}

static DEVICE_API(flash, flash_ipro_api) = {
	.read = flash_ipro_read,
	.write = flash_ipro_write,
	.erase = flash_ipro_erase,
	.get_parameters = flash_ipro_get_parameters,
#ifdef CONFIG_FLASH_PAGE_LAYOUT
	.page_layout = flash_ipro_page_layout,
#endif
};

static struct flash_ipro_data flash_ipro_data_0;

DEVICE_DT_INST_DEFINE(0, flash_ipro_init, NULL, &flash_ipro_data_0, NULL, POST_KERNEL,
		      CONFIG_FLASH_INIT_PRIORITY, &flash_ipro_api);
