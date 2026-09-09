/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_CHANNEL_MAP_H_
#define IPRO_BT_HOST_CHANNEL_MAP_H_

#include <stdint.h>

struct bt_conn;

/* Read the Controller's current five-octet data-channel map for an LE ACL. */
int ipro_bt_conn_le_read_channel_map(struct bt_conn *conn, uint8_t map[5]);

#endif
