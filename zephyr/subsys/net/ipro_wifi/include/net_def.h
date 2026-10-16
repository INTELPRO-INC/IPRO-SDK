/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * net_def.h for the SDK fhost under Zephyr: the network-stack types behind
 * the opaque net_al_if_t / net_al_rx_t / net_al_tx_t handles. The fhost
 * only forwards them; their layout lives in net_al_zephyr.c.
 */
#ifndef __NET_DEF_H__
#define __NET_DEF_H__

struct ipro_net_if;
struct ipro_txbuf;

typedef struct ipro_net_if inet_if_t;
typedef void inet_buf_rx_t;
typedef struct ipro_txbuf inet_buf_tx_t;

/// Maximum size of a interface name (including null character)
#define NET_AL_MAX_IFNAME 4

#endif /* __NET_DEF_H__ */
