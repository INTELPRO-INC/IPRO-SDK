/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * <sys/socket.h> for the SDK wpa_supplicant under Zephyr. Its includes.h
 * takes this header on CONFIG_LWIP builds (lwIP ships a sys/socket.h);
 * only the address-family constants, byte-order helpers and inet_ntop()
 * are referenced by the sources built here, all served by Zephyr's IP
 * header (CONFIG_NETWORKING).
 */
#ifndef IPRO_ZEPHYR_SUPPLICANT_SYS_SOCKET_H_
#define IPRO_ZEPHYR_SUPPLICANT_SYS_SOCKET_H_

#include <zephyr/net/net_ip.h>
/* (not <zephyr/net/ethernet.h>: it pulls net_if.h, whose net_if_get_name()
 * and net_l2_send() clash with the SDK's net_al.h in the fhost sources) */
#ifndef ETH_P_802_3
#define ETH_P_802_3 0x0001
#endif
#ifndef ETHERTYPE_IP
#define ETHERTYPE_IP 0x0800
#endif

/* inet_ntoa() as the supplicant's ip_addr.c uses it (one static buffer) */
static inline const char *ipro_shim_inet_ntoa(struct in_addr a)
{
	static char buf[16];

	return net_addr_ntop(AF_INET, &a, buf, sizeof(buf)) != NULL ? buf : "";
}
#ifndef inet_ntoa
#define inet_ntoa(a) ipro_shim_inet_ntoa(a)
#endif

#ifndef inet_ntop
#define inet_ntop(family, src, dst, size) net_addr_ntop((family), (src), (dst), (size))
#endif

#endif
