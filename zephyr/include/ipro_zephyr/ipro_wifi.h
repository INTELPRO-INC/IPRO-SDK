/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Zephyr-side entry points of the SDK Wi-Fi 6 stack (CONFIG_IPRO_WIFI):
 * what the glue exports to the network driver and to bring-up samples.
 */
#ifndef IPRO_ZEPHYR_IPRO_WIFI_H
#define IPRO_ZEPHYR_IPRO_WIFI_H

#include <stdint.h>
#include <stdbool.h>

/* Stack events (the SDK's CODE_WIFI_ON_* codes from wifi_mgmr_ext.h) are
 * delivered to one listener, from the Wi-Fi threads. */
typedef void (*ipro_wifi_event_cb_t)(int code, int arg, void *user);
void ipro_wifi_set_event_listener(ipro_wifi_event_cb_t cb, void *user);

/* Start the MAC firmware task and the Wi-Fi manager; returns once the
 * manager is up (CODE_WIFI_ON_MGMR_DONE) or -ETIMEDOUT. */
int ipro_wifi_start(void);
bool ipro_wifi_started(void);

#endif
