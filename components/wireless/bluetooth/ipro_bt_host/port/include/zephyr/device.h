/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_DEVICE_H_
#define IPRO_BT_HOST_ZEPHYR_DEVICE_H_

#include <stdbool.h>

/* Minimal device model used at the IPRO HCI boundary. Device discovery is
 * explicit on this SoC; no Zephyr devicetree or init-section machinery is
 * involved. */
struct device {
	const char *name;
	const void *config;
	const void *api;
	void *data;
};

extern const struct device ipro_bt_hci_device;

#define DEVICE_API_GET(subsys, dev) \
	((const struct subsys##_driver_api *)(dev)->api)

static inline bool device_is_ready(const struct device *dev)
{
	return dev != NULL && dev->api != NULL;
}

#endif /* IPRO_BT_HOST_ZEPHYR_DEVICE_H_ */
