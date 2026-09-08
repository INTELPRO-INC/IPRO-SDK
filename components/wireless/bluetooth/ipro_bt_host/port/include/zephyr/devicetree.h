/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_DEVICETREE_H_
#define IPRO_BT_HOST_ZEPHYR_DEVICETREE_H_

/* IPRO selects its on-chip HCI transport through Kconfig/CMake. Map Zephyr's
 * mandatory chosen HCI node onto the explicitly exported IPRO device. */
#define DT_HAS_CHOSEN(prop) 1
#define DT_CHOSEN(prop) ipro_bt_hci
#define DEVICE_DT_GET(node_id) (&ipro_bt_hci_device)
#define DT_PROP_OR(node_id, prop, default_value) (default_value)
#define DT_ENUM_IDX_OR(node_id, prop, default_value) (default_value)

#endif /* IPRO_BT_HOST_ZEPHYR_DEVICETREE_H_ */
