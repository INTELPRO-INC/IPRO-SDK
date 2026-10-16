/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Zephyr Wi-Fi station interface on the SDK Wi-Fi 6 stack (CONFIG_IPRO_WIFI):
 * Ethernet L2 over the fhost data path (subsys/net/ipro_wifi), management
 * (scan / connect / disconnect / status) through the SDK's Wi-Fi manager.
 */
#define DT_DRV_COMPAT intelpro_ipro_wifi

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <ipro_zephyr/ipro_wifi.h>
#include "net_al_zephyr.h"
#include "wifi_mgmr_ext.h"
#ifdef CONFIG_WIFI_IPRO_TCP_TRACE
#include "ipro_wifi_tcptrace.h"
#endif

LOG_MODULE_REGISTER(wifi_ipro, CONFIG_WIFI_LOG_LEVEL);

#define STA_VIF 0

struct ipro_wifi_data {
	struct net_if *iface;
	uint8_t mac[6];
	scan_result_cb_t scan_cb;
	bool connected;
	struct k_work scan_done_work;
	struct k_work_delayable connect_result_work;
	int connect_status;
	char ssid[33];
};

static struct ipro_wifi_data wifi_data;

extern int platform_get_mac(uint8_t *mac);

/* ---- events from the stack ---------------------------------------------- */

static void scan_item(void *env, void *arg, wifi_mgmr_scan_item_t *ap)
{
	struct ipro_wifi_data *data = env;
	struct wifi_scan_result res = {0};

	ARG_UNUSED(arg);
	if (data->scan_cb == NULL) {
		return;
	}
	res.ssid_length = MIN(ap->ssid_len, WIFI_SSID_MAX_LEN);
	memcpy(res.ssid, ap->ssid, res.ssid_length);
	res.channel = ap->channel;
	res.band = WIFI_FREQ_BAND_2_4_GHZ;
	res.rssi = ap->rssi;
	res.mac_length = 6;
	memcpy(res.mac, ap->bssid, 6);
	switch (ap->auth) {
	case WIFI_EVENT_BEACON_IND_AUTH_OPEN:
		res.security = WIFI_SECURITY_TYPE_NONE;
		break;
	case WIFI_EVENT_BEACON_IND_AUTH_WEP:
		res.security = WIFI_SECURITY_TYPE_WEP;
		break;
	case WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE:
	case WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE:
		res.security = WIFI_SECURITY_TYPE_SAE;
		break;
	default:
		res.security = WIFI_SECURITY_TYPE_PSK;
		break;
	}
	data->scan_cb(data->iface, 0, &res);
}

static void scan_done_fn(struct k_work *work)
{
	struct ipro_wifi_data *data = CONTAINER_OF(work, struct ipro_wifi_data, scan_done_work);

	wifi_mgmr_scan_ap_all(data, NULL, scan_item);
	if (data->scan_cb != NULL) {
		scan_result_cb_t cb = data->scan_cb;

		data->scan_cb = NULL;
		cb(data->iface, 0, NULL);
	}
}

static void connect_result_fn(struct k_work *work)
{
	struct k_work_delayable *dw = k_work_delayable_from_work(work);
	struct ipro_wifi_data *data = CONTAINER_OF(dw, struct ipro_wifi_data, connect_result_work);

	if (data->connect_status == WIFI_STATUS_CONN_SUCCESS && !IS_ENABLED(CONFIG_IPRO_WIFI_STA_PS)) {
		/* The MAC associates in 802.11 power save: the AP then holds our
		 * downlink until the next beacon/DTIM, which shows up as 50-400 ms
		 * RTT and loss above a few Mbps. Leave PS unless asked for. */
		if (wifi_mgmr_sta_ps_exit() != 0) {
			LOG_WRN("could not leave power save");
		}
	}
	wifi_mgmt_raise_connect_result_event(data->iface, data->connect_status);
}

static void stack_event(int code, int arg, void *user)
{
	struct ipro_wifi_data *data = user;

	ARG_UNUSED(arg);
	switch (code) {
	case CODE_WIFI_ON_SCAN_DONE:
		k_work_submit(&data->scan_done_work);
		break;
	case CODE_WIFI_ON_CONNECTED:
		LOG_INF("connected");
		data->connected = true;
		data->connect_status = WIFI_STATUS_CONN_SUCCESS;
		k_work_schedule(&data->connect_result_work, K_NO_WAIT);
		break;
	case CODE_WIFI_ON_GOT_IP:
		LOG_INF("got IP");
		break;
	case CODE_WIFI_ON_DISCONNECT:
		LOG_INF("disconnected");
		if (data->connected) {
			data->connected = false;
			wifi_mgmt_raise_disconnect_result_event(data->iface, 0);
		} else {
			data->connect_status = WIFI_STATUS_CONN_FAIL;
			k_work_schedule(&data->connect_result_work, K_NO_WAIT);
		}
		break;
	default:
		break;
	}
}

/* ---- wifi_mgmt ------------------------------------------------------------ */

static int ipro_wifi_scan(const struct device *dev, struct wifi_scan_params *params,
			  scan_result_cb_t cb)
{
	struct ipro_wifi_data *data = dev->data;
	wifi_mgmr_scan_params_t p = {0};
	int err;

	ARG_UNUSED(params);
	if (data->scan_cb != NULL) {
		return -EBUSY;
	}
	err = ipro_wifi_start();
	if (err != 0) {
		return err;
	}
	data->scan_cb = cb;
	err = wifi_mgmr_sta_scan(&p);
	if (err != 0) {
		data->scan_cb = NULL;
		return -EIO;
	}
	return 0;
}

static int ipro_wifi_connect(const struct device *dev, struct wifi_connect_req_params *params)
{
	struct ipro_wifi_data *data = dev->data;
	char key[65] = {0};
	int err;

	err = ipro_wifi_start();
	if (err != 0) {
		return err;
	}
	if (params->ssid_length == 0 || params->ssid_length > 32) {
		return -EINVAL;
	}
	memcpy(data->ssid, params->ssid, params->ssid_length);
	data->ssid[params->ssid_length] = '\0';
	if (params->security != WIFI_SECURITY_TYPE_NONE) {
		if (params->psk_length == 0 || params->psk_length > 64) {
			return -EINVAL;
		}
		memcpy(key, params->psk, params->psk_length);
	}
	LOG_INF("connecting to \"%s\"", data->ssid);
	err = wifi_sta_connect(data->ssid, key[0] != '\0' ? key : NULL, NULL, NULL, 0, 0, 0, 1);
	if (err != 0) {
		LOG_ERR("wifi_sta_connect: %d", err);
		return -EIO;
	}
	return 0;
}

static int ipro_wifi_disconnect(const struct device *dev)
{
	ARG_UNUSED(dev);
	return wifi_sta_disconnect() == 0 ? 0 : -EIO;
}

static int ipro_wifi_status(const struct device *dev, struct wifi_iface_status *status)
{
	struct ipro_wifi_data *data = dev->data;
	int rssi = 0, channel = 0;

	memset(status, 0, sizeof(*status));
	status->iface_mode = WIFI_MODE_INFRA;
	status->band = WIFI_FREQ_BAND_2_4_GHZ;
	status->link_mode = WIFI_6;
	if (!data->connected) {
		status->state = WIFI_STATE_DISCONNECTED;
		return 0;
	}
	status->state = WIFI_STATE_COMPLETED;
	(void)wifi_mgmr_sta_rssi_get(&rssi);
	(void)wifi_mgmr_sta_channel_get(&channel);
	status->rssi = rssi;
	status->channel = channel;
	status->ssid_len = strlen(data->ssid);
	memcpy(status->ssid, data->ssid, status->ssid_len);
	status->security = WIFI_SECURITY_TYPE_PSK;
	return 0;
}

/* ---- Ethernet interface --------------------------------------------------- */

static int ipro_wifi_send(const struct device *dev, struct net_pkt *pkt)
{
	ARG_UNUSED(dev);
#ifdef CONFIG_WIFI_IPRO_TCP_TRACE
	ipro_tcptrace_tx(pkt);
#endif
	return ipro_net_if_send(STA_VIF, pkt);
}

static void ipro_wifi_iface_init(struct net_if *iface)
{
	const struct device *dev = net_if_get_device(iface);
	struct ipro_wifi_data *data = dev->data;
	struct ethernet_context *eth_ctx = net_if_l2_data(iface);

	eth_ctx->eth_if_type = L2_ETH_IF_TYPE_WIFI;
	data->iface = iface;
	(void)platform_get_mac(data->mac);
	net_if_set_link_addr(iface, data->mac, sizeof(data->mac), NET_LINK_ETHERNET);
	ethernet_init(iface);
	net_if_dormant_on(iface); /* until the stack reports a link */
	ipro_net_if_bind(STA_VIF, iface);
	ipro_wifi_set_event_listener(stack_event, data);
	k_work_init(&data->scan_done_work, scan_done_fn);
	k_work_init_delayable(&data->connect_result_work, connect_result_fn);
}

#ifdef CONFIG_IPRO_WIFI_TX_CSUM_OFFLOAD
/* The UDP checksum is optional in IPv4 (0 = none); the driver fills in the
 * IPv4 header checksum itself (net_al_zephyr.c), so Zephyr can skip both
 * and save a pass over the payload per datagram. TCP stays in software. */
static int ipro_wifi_get_config(const struct device *dev, enum ethernet_config_type type,
				struct ethernet_config *config)
{
	ARG_UNUSED(dev);
	if (type == ETHERNET_CONFIG_TYPE_TX_CHECKSUM_SUPPORT) {
		config->chksum_support = ETHERNET_CHECKSUM_SUPPORT_IPV4_HEADER |
					 ETHERNET_CHECKSUM_SUPPORT_UDP;
		return 0;
	}
	return -ENOTSUP;
}
#endif

/* Runtime A/B switch for the bench (wifi_ipro csum). */
uint8_t ipro_wifi_csum_offload = IS_ENABLED(CONFIG_IPRO_WIFI_TX_CSUM_OFFLOAD);

static enum ethernet_hw_caps ipro_wifi_caps(const struct device *dev)
{
	ARG_UNUSED(dev);
	return ipro_wifi_csum_offload ? ETHERNET_HW_TX_CHKSUM_OFFLOAD : 0;
}

static const struct wifi_mgmt_ops ipro_wifi_mgmt = {
	.scan = ipro_wifi_scan,
	.connect = ipro_wifi_connect,
	.disconnect = ipro_wifi_disconnect,
	.iface_status = ipro_wifi_status,
};

static const struct net_wifi_mgmt_offload ipro_wifi_api = {
	.wifi_iface.iface_api.init = ipro_wifi_iface_init,
	.wifi_iface.get_capabilities = ipro_wifi_caps,
#ifdef CONFIG_IPRO_WIFI_TX_CSUM_OFFLOAD
	.wifi_iface.get_config = ipro_wifi_get_config,
#endif
	.wifi_iface.send = ipro_wifi_send,
	.wifi_mgmt_api = &ipro_wifi_mgmt,
};

static int ipro_wifi_init(const struct device *dev)
{
	ARG_UNUSED(dev);
	return 0;
}

NET_DEVICE_DT_INST_DEFINE(0, ipro_wifi_init, NULL, &wifi_data, NULL,
			  CONFIG_WIFI_IPRO_INIT_PRIORITY, &ipro_wifi_api, ETHERNET_L2,
			  NET_L2_GET_CTX_TYPE(ETHERNET_L2), NET_ETH_MTU);
