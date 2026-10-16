/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Bring-up shell for the IPRO7 802.15.4 radio: use `ieee802154 set_chan`,
 * `ieee802154 set_pan_id`, `ieee802154 set_short_addr`, `net iface` and
 * `net ping` between two boards.
 */

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	struct net_if *iface = net_if_get_first_by_type(&NET_L2_GET_NAME(IEEE802154));

	printk("IPRO7 802.15.4 shell; iface %p %s\n", iface,
	       iface != NULL ? (net_if_is_up(iface) ? "up" : "down") : "missing");
	return 0;
}
