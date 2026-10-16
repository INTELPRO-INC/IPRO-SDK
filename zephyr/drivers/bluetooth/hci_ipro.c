/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Zephyr HCI driver for the IPRO7 BLE controller.
 *
 * This is the Zephyr-native form of the SDK's
 * components/wireless/bluetooth/ipro_bt_host/port/hci_ipro.c (which already
 * targets the Zephyr v4.4 host API but runs on FreeRTOS): the H4 byte stream
 * is read from / written to the controller's in-memory transport (hcitl)
 * through the ipro_bt_ctlr_* facade; packet framing and the buffer rules are
 * unchanged.
 *
 *   open()  -> start the controller thread + HCI RX thread
 *   send()  -> serialised H4 write into the controller FIFO
 *   RX      -> thread blocks on the controller's rx-notify, reassembles
 *              EVT/ACL/ISO packets into net_bufs, hands them to the host
 */

#define DT_DRV_COMPAT intelpro_ipro_bt_hci

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/bluetooth.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/net_buf.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include <ipro_zephyr/ipro_bt_ctlr.h>
#include <zephyr/dt-bindings/clock/ipro7-clock.h>

LOG_MODULE_REGISTER(bt_hci_ipro, CONFIG_BT_HCI_DRIVER_LOG_LEVEL);

#define H4_CMD 0x01
#define H4_ACL 0x02
#define H4_SCO 0x03
#define H4_EVT 0x04
#define H4_ISO 0x05

struct hci_ipro_data {
	bt_hci_recv_t recv;
	struct k_sem rx_sem;
	struct k_sem tx_lock;
	struct k_thread rx_thread;
	K_KERNEL_STACK_MEMBER(rx_stack, CONFIG_BT_IPRO_HCI_RX_STACK_SIZE);
	bool opened;
};

static void controller_rx_notify(void *ctx)
{
	struct hci_ipro_data *data = ctx;

	k_sem_give(&data->rx_sem);
}

static void rx_exact(struct hci_ipro_data *data, uint8_t *dst, uint16_t len)
{
	uint16_t offset = 0;

	while (offset < len) {
		uint32_t count = ipro_bt_ctlr_hci_read(dst + offset, len - offset);

		if (count == 0U) {
			(void)k_sem_take(&data->rx_sem, K_FOREVER);
		} else {
			offset = (uint16_t)(offset + count);
		}
	}
}

static void rx_discard(struct hci_ipro_data *data, uint16_t len)
{
	uint8_t scratch[16];

	while (len != 0U) {
		uint16_t count = len > sizeof(scratch) ? sizeof(scratch) : len;

		rx_exact(data, scratch, count);
		len = (uint16_t)(len - count);
	}
}

static void rx_event(const struct device *dev)
{
	struct hci_ipro_data *data = dev->data;
	struct bt_hci_evt_hdr hdr;
	struct net_buf *buf;
	bool discardable = false;

	rx_exact(data, (uint8_t *)&hdr, sizeof(hdr));

	if (hdr.evt == BT_HCI_EVT_LE_META_EVENT && hdr.len != 0U) {
		uint8_t subevent;

		rx_exact(data, &subevent, 1);
		discardable = subevent == BT_HCI_EVT_LE_ADVERTISING_REPORT ||
			      subevent == BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT;
		buf = bt_buf_get_evt(hdr.evt, discardable, discardable ? K_NO_WAIT : K_FOREVER);
		if (buf == NULL || net_buf_tailroom(buf) < sizeof(hdr) + hdr.len) {
			static uint32_t dropped;

			if (buf != NULL) {
				/* Too big for the pool: see BT_BUF_EVT_DISCARDABLE_SIZE. */
				if ((dropped++ % 100U) == 0U) {
					LOG_WRN("LE event 0x%02x len %u dropped (%u so far): raise "
						"BT_BUF_EVT_DISCARDABLE_SIZE", subevent, hdr.len,
						dropped);
				}
				net_buf_unref(buf);
			}
			rx_discard(data, (uint16_t)(hdr.len - 1U));
			return;
		}
		net_buf_add_mem(buf, &hdr, sizeof(hdr));
		net_buf_add_u8(buf, subevent);
		if (hdr.len > 1U) {
			rx_exact(data, net_buf_add(buf, hdr.len - 1U), hdr.len - 1U);
		}
	} else {
		buf = bt_buf_get_evt(hdr.evt, false, K_FOREVER);
		if (buf == NULL || net_buf_tailroom(buf) < sizeof(hdr) + hdr.len) {
			if (buf != NULL) {
				net_buf_unref(buf);
			}
			rx_discard(data, hdr.len);
			return;
		}
		net_buf_add_mem(buf, &hdr, sizeof(hdr));
		if (hdr.len != 0U) {
			rx_exact(data, net_buf_add(buf, hdr.len), hdr.len);
		}
	}

	/*
	 * The RW controller advertises several command credits; Zephyr's host
	 * tracks one synchronous command at a time, so clamp Num_HCI_Command_
	 * Packets to 1 in Command Complete / Command Status (same as the SDK
	 * port). bt_buf_get_evt() keeps the H4 type byte at data[0].
	 */
	if (hdr.evt == BT_HCI_EVT_CMD_COMPLETE && hdr.len >= 1U) {
		buf->data[1U + sizeof(hdr)] = 1U;
	} else if (hdr.evt == BT_HCI_EVT_CMD_STATUS && hdr.len >= 2U) {
		buf->data[1U + sizeof(hdr) + 1U] = 1U;
	}

	(void)data->recv(dev, buf);

	if (hdr.evt == BT_HCI_EVT_CMD_COMPLETE || hdr.evt == BT_HCI_EVT_CMD_STATUS) {
		/*
		 * Scheduling boundary a real UART transport has between a
		 * synchronous command event and whatever the controller sends
		 * next. This controller answers some commands with the follow-up
		 * event almost at once (LE Create BIG -> LE BIG Complete within a
		 * millisecond); without the gap the host's RX work processes that
		 * event before the command's caller has run its own
		 * post-processing (bt_iso: "invalid transition", channels left in
		 * "connecting"). Sleep AFTER handing the command event over, so
		 * the waiting caller gets to run first.
		 */
		k_sleep(K_MSEC(2));
	}
}

static void rx_acl(const struct device *dev)
{
	struct hci_ipro_data *data = dev->data;
	struct bt_hci_acl_hdr hdr;
	struct net_buf *buf;
	uint16_t len;

	rx_exact(data, (uint8_t *)&hdr, sizeof(hdr));
	len = sys_le16_to_cpu(hdr.len);

	buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_FOREVER);
	if (buf == NULL || net_buf_tailroom(buf) < sizeof(hdr) + len) {
		if (buf != NULL) {
			net_buf_unref(buf);
		}
		rx_discard(data, len);
		return;
	}
	net_buf_add_mem(buf, &hdr, sizeof(hdr));
	if (len != 0U) {
		rx_exact(data, net_buf_add(buf, len), len);
	}
	(void)data->recv(dev, buf);
}

static void rx_sco(const struct device *dev)
{
	struct hci_ipro_data *data = dev->data;
	struct bt_hci_sco_hdr hdr;

	/* SCO (BR/EDR voice) is not consumed by the LE host: keep the stream aligned. */
	rx_exact(data, (uint8_t *)&hdr, sizeof(hdr));
	rx_discard(data, hdr.len);
}

static void rx_iso(const struct device *dev)
{
	struct hci_ipro_data *data = dev->data;
	struct bt_hci_iso_hdr hdr;
	uint16_t len;

	rx_exact(data, (uint8_t *)&hdr, sizeof(hdr));
	len = bt_iso_hdr_len(sys_le16_to_cpu(hdr.len));

#if defined(CONFIG_BT_ISO_RX)
	struct net_buf *buf = bt_buf_get_rx(BT_BUF_ISO_IN, K_NO_WAIT);

	if (buf == NULL || net_buf_tailroom(buf) < sizeof(hdr) + len) {
		if (buf != NULL) {
			net_buf_unref(buf);
		}
		rx_discard(data, len);
		return;
	}
	net_buf_add_mem(buf, &hdr, sizeof(hdr));
	if (len != 0U) {
		rx_exact(data, net_buf_add(buf, len), len);
	}
	(void)data->recv(dev, buf);
#else
	rx_discard(data, len);
#endif
}

static void rx_thread(void *p1, void *p2, void *p3)
{
	const struct device *dev = p1;
	struct hci_ipro_data *data = dev->data;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (;;) {
		uint8_t type;

		rx_exact(data, &type, 1);
		switch (type) {
		case H4_EVT:
			rx_event(dev);
			break;
		case H4_ACL:
			rx_acl(dev);
			break;
		case H4_SCO:
			rx_sco(dev);
			break;
		case H4_ISO:
			rx_iso(dev);
			break;
		default:
			LOG_ERR("unknown H4 type 0x%02x (stream desync)", type);
			break;
		}
	}
}

static int hci_ipro_open(const struct device *dev, bt_hci_recv_t recv)
{
	struct hci_ipro_data *data = dev->data;
	int ret;

	if (data->opened) {
		return -EALREADY;
	}
	data->recv = recv;
	k_sem_init(&data->rx_sem, 0, K_SEM_MAX_LIMIT);
	k_sem_init(&data->tx_lock, 1, 1);

	/*
	 * Ungate the BLE core's AHB clock (GLB CGEN, the "clocks" phandle of
	 * this node). The SDK board init does it for every wireless block;
	 * the prebuilt controller does not. Without it the controller's HCI
	 * path still answers (it is host-side buffered) but the core never
	 * runs: no advertising on air, no BLE interrupt. Measured on the EVK
	 * against the FreeRTOS reference (CGEN_CFG2 0x11 vs 0x00). The
	 * 802.15.4 block shares the 2.4 GHz PHY, so bring it up as well.
	 */
	{
		const struct device *clk = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(0));

		if (device_is_ready(clk)) {
			(void)clock_control_on(clk, (clock_control_subsys_t)DT_INST_CLOCKS_CELL(0, id));
			(void)clock_control_on(clk, (clock_control_subsys_t)IPRO7_CLK_M154);
		}
	}

	ret = ipro_bt_ctlr_start(CONFIG_BT_IPRO_CTLR_TASK_PRIO, controller_rx_notify, data);
	if (ret != 0) {
		LOG_ERR("controller start failed (%d)", ret);
		return ret;
	}

	k_thread_create(&data->rx_thread, data->rx_stack, K_KERNEL_STACK_SIZEOF(data->rx_stack),
			rx_thread, (void *)dev, NULL, NULL, K_PRIO_COOP(CONFIG_BT_IPRO_HCI_RX_PRIO),
			0, K_NO_WAIT);
	k_thread_name_set(&data->rx_thread, "ipro_hci_rx");
	data->opened = true;
	return 0;
}

static int hci_ipro_send(const struct device *dev, struct net_buf *buf)
{
	struct hci_ipro_data *data = dev->data;
	uint32_t offset = 0;

	(void)k_sem_take(&data->tx_lock, K_FOREVER);
	while (offset < buf->len) {
		uint32_t count = ipro_bt_ctlr_hci_write(buf->data + offset, buf->len - offset);

		if (count == 0U) {
			k_yield();
		} else {
			offset += count;
		}
	}
	k_sem_give(&data->tx_lock);
	net_buf_unref(buf);
	return 0;
}

static DEVICE_API(bt_hci, hci_ipro_api) = {
	.open = hci_ipro_open,
	.send = hci_ipro_send,
};

static struct hci_ipro_data hci_ipro_data_0;

DEVICE_DT_INST_DEFINE(0, NULL, NULL, &hci_ipro_data_0, NULL, POST_KERNEL,
		      CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &hci_ipro_api);
