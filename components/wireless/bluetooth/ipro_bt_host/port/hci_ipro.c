/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#undef STRINGIFY
#undef likely
#undef unlikely

#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/device.h>
#include <zephyr/drivers/bluetooth.h>
#include <zephyr/kernel.h>
#include <zephyr/net_buf.h>

#include "ipro_log.h"

#define LOG_TAG "BT_HCI"
#include <zephyr/sys/byteorder.h>

#include "ipro_ble_ctlr_hcitl.h"
#include "hci_driver_ipro_ble_ctlr.h"

#define H4_CMD 0x01
#define H4_ACL 0x02
#define H4_SCO 0x03
#define H4_EVT 0x04
#define H4_ISO 0x05

static struct k_sem rx_sem;
static struct k_sem tx_lock;
static bt_hci_recv_t host_recv;
static bool hci_opened;
static ipro_bt_hci_sco_rx_cb_t sco_rx_cb;
static void *sco_rx_user_data;

volatile uint32_t ipro_bt_host_h4_acl_rx_count;
volatile uint32_t ipro_bt_host_h4_acl_rx_bytes;
volatile uint32_t ipro_bt_host_h4_acl_rx_oversize_count;
volatile uint16_t ipro_bt_host_h4_acl_rx_last_len;
volatile uint16_t ipro_bt_host_h4_acl_rx_max_len;
volatile uint32_t ipro_bt_host_h4_sco_rx_count;
volatile uint32_t ipro_bt_host_h4_sco_rx_bytes;
volatile uint32_t ipro_bt_host_h4_sco_rx_no_callback_count;
volatile uint32_t ipro_bt_host_h4_sco_tx_count;
volatile uint32_t ipro_bt_host_h4_sco_tx_bytes;

#if defined(CONFIG_IPRO_BT_HOST_HCI_TRACE)
static const char *hci_opcode_name(uint16_t opcode)
{
	switch (opcode) {
	case BT_HCI_OP_LE_SET_RANDOM_ADDRESS:
		return "LE_SET_RANDOM_ADDRESS";
	case BT_HCI_OP_LE_SET_ADV_PARAM:
		return "LE_SET_ADV_PARAM";
	case BT_HCI_OP_LE_SET_ADV_DATA:
		return "LE_SET_ADV_DATA";
	case BT_HCI_OP_LE_SET_ADV_ENABLE:
		return "LE_SET_ADV_ENABLE";
	case BT_HCI_OP_LE_CLEAR_FAL:
		return "LE_CLEAR_FAL";
	case BT_HCI_OP_LE_ADD_DEV_TO_FAL:
		return "LE_ADD_DEV_TO_FAL";
	case BT_HCI_OP_LE_REM_DEV_FROM_FAL:
		return "LE_REM_DEV_FROM_FAL";
	case BT_HCI_OP_LE_ENCRYPT:
		return "LE_ENCRYPT";
	case BT_HCI_OP_LE_START_ENCRYPTION:
		return "LE_START_ENC";
	case BT_HCI_OP_LE_LTK_REQ_REPLY:
		return "LE_LTK_REPLY";
	case BT_HCI_OP_LE_LTK_REQ_NEG_REPLY:
		return "LE_LTK_NEG";
	case BT_HCI_OP_LE_P256_PUBLIC_KEY:
		return "LE_P256_PUBLIC_KEY";
	case BT_HCI_OP_LE_GENERATE_DHKEY:
		return "LE_GENERATE_DHKEY";
	case BT_HCI_OP_LE_GENERATE_DHKEY_V2:
		return "LE_GENERATE_DHKEY_V2";
	case BT_HCI_OP_LE_ADD_DEV_TO_RL:
		return "LE_ADD_DEV_TO_RL";
	case BT_HCI_OP_LE_REM_DEV_FROM_RL:
		return "LE_REM_DEV_FROM_RL";
	case BT_HCI_OP_LE_CLEAR_RL:
		return "LE_CLEAR_RL";
	case BT_HCI_OP_LE_READ_PEER_RPA:
		return "LE_READ_PEER_RPA";
	case BT_HCI_OP_LE_READ_LOCAL_RPA:
		return "LE_READ_LOCAL_RPA";
	case BT_HCI_OP_LE_SET_ADDR_RES_ENABLE:
		return "LE_SET_ADDR_RES_ENABLE";
	case BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR:
		return "LE_SET_ADV_SET_RANDOM_ADDR";
	case BT_HCI_OP_LE_SET_EXT_ADV_PARAM:
		return "LE_SET_EXT_ADV_PARAM";
	case BT_HCI_OP_LE_SET_EXT_ADV_PARAM_V2:
		return "LE_SET_EXT_ADV_PARAM_V2";
	case BT_HCI_OP_LE_SET_EXT_ADV_DATA:
		return "LE_SET_EXT_ADV_DATA";
	case BT_HCI_OP_LE_SET_EXT_ADV_ENABLE:
		return "LE_SET_EXT_ADV_ENABLE";
	case BT_HCI_OP_LE_EXT_CREATE_CONN:
		return "LE_EXT_CREATE_CONN";
	case BT_HCI_OP_LE_EXT_CREATE_CONN_V2:
		return "LE_EXT_CREATE_CONN_V2";
	case BT_HCI_OP_LE_READ_BUFFER_SIZE_V2:
		return "LE_READ_BUFFER_SIZE_V2";
	case BT_HCI_OP_LE_READ_REMOTE_FEATURES:
		return "LE_READ_REMOTE_FEATURES";
	case BT_HCI_OP_READ_REMOTE_VERSION_INFO:
		return "READ_REMOTE_VERSION_INFO";
	case BT_HCI_OP_LE_SET_CIG_PARAMS:
		return "LE_SET_CIG_PARAMS";
	case BT_HCI_OP_LE_SET_CIG_PARAMS_TEST:
		return "LE_SET_CIG_PARAMS_TEST";
	case BT_HCI_OP_LE_CREATE_CIS:
		return "LE_CREATE_CIS";
	case BT_HCI_OP_LE_ACCEPT_CIS:
		return "LE_ACCEPT_CIS";
	case BT_HCI_OP_LE_REJECT_CIS:
		return "LE_REJECT_CIS";
	case BT_HCI_OP_LE_SETUP_ISO_PATH:
		return "LE_SETUP_ISO_PATH";
	case BT_HCI_OP_LE_REMOVE_ISO_PATH:
		return "LE_REMOVE_ISO_PATH";
	case BT_HCI_OP_LE_READ_ISO_LINK_QUALITY:
		return "LE_READ_ISO_LINK_QUALITY";
	case BT_HCI_OP_LE_SET_PRIVACY_MODE:
		return "LE_SET_PRIVACY_MODE";
	default:
		return NULL;
	}
}

static const char *smp_code_name(uint8_t code)
{
	switch (code) {
	case 0x01:
		return "PAIRING_REQ";
	case 0x02:
		return "PAIRING_RSP";
	case 0x03:
		return "PAIRING_CONFIRM";
	case 0x04:
		return "PAIRING_RANDOM";
	case 0x05:
		return "PAIRING_FAILED";
	case 0x06:
		return "ENCRYPT_INFO";
	case 0x07:
		return "CENTRAL_IDENT";
	case 0x08:
		return "IDENT_INFO";
	case 0x09:
		return "IDENT_ADDR";
	case 0x0a:
		return "SIGNING_INFO";
	case 0x0b:
		return "SECURITY_REQ";
	case 0x0c:
		return "PUBLIC_KEY";
	case 0x0d:
		return "DHKEY_CHECK";
	case 0x0e:
		return "KEYPRESS";
	case 0x0f:
		return "ID_ADDR_INFO";
	default:
		return "UNKNOWN";
	}
}

/* Longest dump any trace site asks for (LE_CIS_ESTABLISHED). */
#define TRACE_HEX_MAX     64U
#define TRACE_HEX_BUF_SZ  (TRACE_HEX_MAX * 2U + 3U)   /* hex + ".." + NUL */

/*
 * Format up to `max` bytes as hex into `out`.
 *
 * The old code printf'd the prefix, then the bytes, then the newline, so a log
 * line from another task could land in the middle of a packet dump. Building
 * the whole line first and handing it to one IPRO_LOGD keeps a trace line
 * atomic, and lets the level filter drop it before any formatting cost.
 *
 * Callers size their buffer from TRACE_HEX_BUF_SZ: this runs on the
 * ipro_hci_rx task, whose stack is 2 KB, so nothing bigger belongs here.
 */
static void trace_hex_str(char *out, size_t out_sz, const uint8_t *data,
			  uint16_t len, uint16_t max)
{
	static const char digits[] = "0123456789abcdef";
	uint16_t count = len < max ? len : max;
	size_t pos = 0;

	if (out == NULL || out_sz == 0U) {
		return;
	}
	for (uint16_t i = 0; i < count && (pos + 2U) < out_sz; i++) {
		out[pos++] = digits[data[i] >> 4];
		out[pos++] = digits[data[i] & 0x0fU];
	}
	if (len > count && (pos + 2U) < out_sz) {
		out[pos++] = '.';
		out[pos++] = '.';
	}
	out[pos] = '\0';
}

static const char *l2cap_cid_name(uint16_t cid)
{
	switch (cid) {
	case 0x0004:
		return "ATT";
	case 0x0005:
		return "LE_SIG";
	case 0x0006:
		return "SMP";
	default:
		return NULL;
	}
}

static const char *att_code_name(uint8_t code)
{
	switch (code) {
	case 0x02: return "MTU_REQ";
	case 0x03: return "MTU_RSP";
	case 0x04: return "FIND_INFO_REQ";
	case 0x05: return "FIND_INFO_RSP";
	case 0x06: return "FIND_TYPE_REQ";
	case 0x07: return "FIND_TYPE_RSP";
	case 0x08: return "READ_TYPE_REQ";
	case 0x09: return "READ_TYPE_RSP";
	case 0x0a: return "READ_REQ";
	case 0x0b: return "READ_RSP";
	case 0x10: return "READ_BY_GROUP_REQ";
	case 0x11: return "READ_BY_GROUP_RSP";
	case 0x12: return "WRITE_REQ";
	case 0x13: return "WRITE_RSP";
	case 0x1b: return "HANDLE_NTF";
	case 0x52: return "WRITE_CMD";
	default: return "-";
	}
}

static void trace_acl_l2cap(const char *dir, const uint8_t *data, uint16_t len,
			    bool has_h4)
{
	uint16_t off = has_h4 ? 1U : 0U;
	uint16_t handle_pb;
	uint16_t acl_len;
	uint16_t l2_len;
	uint16_t cid;
	uint8_t code;
	const char *cid_name;

	if (len < (uint16_t)(off + 9U)) {
		return;
	}

	handle_pb = sys_get_le16(&data[off]);
	acl_len = sys_get_le16(&data[off + 2U]);
	l2_len = sys_get_le16(&data[off + 4U]);
	cid = sys_get_le16(&data[off + 6U]);
	cid_name = l2cap_cid_name(cid);
	if (cid_name == NULL) {
		return;
	}

	code = data[off + 8U];
	{
		char hex[TRACE_HEX_BUF_SZ];

		trace_hex_str(hex, sizeof(hex), &data[off + 8U], (uint16_t)(len - off - 8U), 40U);
		IPRO_LOGD(LOG_TAG, "%s ACL_%s handle=0x%04x pb=%u acl_len=%u l2_len=%u code=0x%02x(%s) data=%s", dir, cid_name, handle_pb & 0x0fffU, (handle_pb >> 12U) & 0x3U, acl_len, l2_len, code, cid == 0x0004U ? att_code_name(code) : (cid == 0x0006U ? smp_code_name(code) : "-"), hex);
	}
}

static void trace_cmd(const uint8_t *data, uint16_t len)
{
	uint16_t opcode;
	const char *name;

	if (len < 4U || data[0] != H4_CMD) {
		return;
	}

	opcode = sys_get_le16(&data[1]);
	name = hci_opcode_name(opcode);
	if (name == NULL) {
		return;
	}

	{
		char hex[TRACE_HEX_BUF_SZ];

		trace_hex_str(hex, sizeof(hex), &data[4], data[3], 48U);
		IPRO_LOGD(LOG_TAG, "TX CMD opcode=0x%04x(%s) plen=%u data=%s", opcode, name, data[3], hex);
	}
}

static void trace_event(const struct net_buf *buf)
{
	const uint8_t *data = buf->data;
	uint16_t len = buf->len;
	uint16_t off = 0U;
	uint8_t evt;
	uint8_t plen;

	if (len >= 3U && data[0] == H4_EVT) {
		off = 1U;
	}
	if (len < (uint16_t)(off + 2U)) {
		return;
	}

	evt = data[off];
	plen = data[off + 1U];
	if (evt == BT_HCI_EVT_CMD_COMPLETE && len >= (uint16_t)(off + 6U)) {
		uint16_t opcode = sys_get_le16(&data[off + 3U]);
		const char *name = hci_opcode_name(opcode);
		if (name != NULL) {
			{
				char hex[TRACE_HEX_BUF_SZ];

				trace_hex_str(hex, sizeof(hex), &data[off + 5U], (uint16_t)(len - off - 5U), 48U);
				IPRO_LOGD(LOG_TAG, "RX EVT CMD_COMPLETE opcode=0x%04x(%s) status=0x%02x plen=%u data=%s", opcode, name, data[off + 5U], plen, hex);
			}
		}
	} else if (evt == BT_HCI_EVT_CMD_STATUS && len >= (uint16_t)(off + 6U)) {
		uint16_t opcode = sys_get_le16(&data[off + 4U]);
		const char *name = hci_opcode_name(opcode);
		if (name != NULL) {
			IPRO_LOGD(LOG_TAG, "RX EVT CMD_STATUS opcode=0x%04x(%s) status=0x%02x plen=%u", opcode, name, data[off + 2U], plen);
		}
	} else if (evt == BT_HCI_EVT_ENCRYPT_CHANGE && len >= (uint16_t)(off + 6U)) {
		IPRO_LOGD(LOG_TAG, "RX EVT ENC_CHANGE status=0x%02x handle=0x%04x encrypt=0x%02x", data[off + 2U], sys_get_le16(&data[off + 3U]), data[off + 5U]);
	} else if (evt == 0x59U) {
		/* Encryption Change V2 (Core 5.3) is not named by the imported
		 * Zephyr 4.4 HCI header yet.  Keep the raw event visible while the
		 * IPRO6S dual-mode controller/Host contract is qualified. */
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 16U);
			IPRO_LOGD(LOG_TAG, "RX EVT ENC_CHANGE_V2 data=%s", hex);
		}
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 3U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_LTK_REQUEST) {
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 24U);
			IPRO_LOGD(LOG_TAG, "RX EVT LE_LTK_REQUEST data=%s", hex);
		}
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 3U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_P256_PUBLIC_KEY_COMPLETE) {
		IPRO_LOGD(LOG_TAG, "RX EVT LE_P256_PUBLIC_KEY_COMPLETE status=0x%02x", len >= (uint16_t)(off + 4U) ? data[off + 3U] : 0xffU);
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 3U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_GENERATE_DHKEY_COMPLETE) {
		IPRO_LOGD(LOG_TAG, "RX EVT LE_GENERATE_DHKEY_COMPLETE status=0x%02x", len >= (uint16_t)(off + 4U) ? data[off + 3U] : 0xffU);
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 4U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_CONN_COMPLETE) {
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 48U);
			IPRO_LOGD(LOG_TAG, "RX EVT LE_CONN_COMPLETE status=0x%02x data=%s", data[off + 3U], hex);
		}
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 4U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_ENH_CONN_COMPLETE) {
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 48U);
			IPRO_LOGD(LOG_TAG, "RX EVT LE_ENH_CONN_COMPLETE status=0x%02x data=%s", data[off + 3U], hex);
		}
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 4U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_ENH_CONN_COMPLETE_V2) {
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 48U);
			IPRO_LOGD(LOG_TAG, "RX EVT LE_ENH_CONN_COMPLETE_V2 status=0x%02x data=%s", data[off + 3U], hex);
		}
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 4U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_ADV_SET_TERMINATED) {
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 48U);
			IPRO_LOGD(LOG_TAG, "RX EVT LE_ADV_SET_TERMINATED status=0x%02x data=%s", data[off + 3U], hex);
		}
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 4U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_CIS_REQ) {
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 48U);
			IPRO_LOGD(LOG_TAG, "RX EVT LE_CIS_REQ status=0x%02x data=%s", data[off + 3U], hex);
		}
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 4U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_CIS_ESTABLISHED) {
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 64U);
			IPRO_LOGD(LOG_TAG, "RX EVT LE_CIS_ESTABLISHED status=0x%02x data=%s", data[off + 3U], hex);
		}
	} else if (evt == BT_HCI_EVT_LE_META_EVENT && len >= (uint16_t)(off + 4U) &&
		   data[off + 2U] == BT_HCI_EVT_LE_CIS_ESTABLISHED_V2) {
		{
			char hex[TRACE_HEX_BUF_SZ];

			trace_hex_str(hex, sizeof(hex), &data[off + 2U], (uint16_t)(len - off - 2U), 64U);
			IPRO_LOGD(LOG_TAG, "RX EVT LE_CIS_ESTABLISHED_V2 status=0x%02x data=%s", data[off + 3U], hex);
		}
	} else if (evt == BT_HCI_EVT_DISCONN_COMPLETE && len >= (uint16_t)(off + 5U)) {
		IPRO_LOGD(LOG_TAG, "RX EVT DISCONN_COMPLETE status=0x%02x handle=0x%04x reason=0x%02x", data[off + 2U], sys_get_le16(&data[off + 3U]), data[off + 5U]);
	}
}
#else
static inline void trace_acl_l2cap(const char *dir, const uint8_t *data,
				   uint16_t len, bool has_h4)
{
	(void)dir;
	(void)data;
	(void)len;
	(void)has_h4;
}

static inline void trace_cmd(const uint8_t *data, uint16_t len)
{
	(void)data;
	(void)len;
}

static inline void trace_event(const struct net_buf *buf)
{
	(void)buf;
}
#endif

static void controller_rx_notify(void)
{
	k_sem_give(&rx_sem);
}

static void rx_exact(uint8_t *dst, uint16_t len)
{
	uint16_t offset = 0;

	while (offset < len) {
		uint32_t count = ipro_ble_ctlr_hcitl_host_read(dst + offset,
							      len - offset);
		if (count == 0U) {
			(void)k_sem_take(&rx_sem, K_FOREVER);
		} else {
			offset = (uint16_t)(offset + count);
		}
	}
}

static void rx_discard(uint16_t len)
{
	uint8_t scratch[16];

	while (len != 0U) {
		uint16_t count = len > sizeof(scratch) ? sizeof(scratch) : len;
		rx_exact(scratch, count);
		len = (uint16_t)(len - count);
	}
}

static void rx_event(const struct device *dev)
{
	struct bt_hci_evt_hdr hdr;
	struct net_buf *buf;
	bool discardable = false;

	rx_exact((uint8_t *)&hdr, sizeof(hdr));
	if (hdr.evt == BT_HCI_EVT_LE_META_EVENT && hdr.len != 0U) {
		uint8_t subevent;
		rx_exact(&subevent, 1);
		discardable = subevent == BT_HCI_EVT_LE_ADVERTISING_REPORT ||
			subevent == BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT;
		buf = bt_buf_get_evt(hdr.evt, discardable,
				     discardable ? K_NO_WAIT : K_FOREVER);
		if (buf == NULL) {
			rx_discard((uint16_t)(hdr.len - 1U));
			return;
		}
		net_buf_add_mem(buf, &hdr, sizeof(hdr));
		net_buf_add_u8(buf, subevent);
		if (hdr.len > 1U) {
			rx_exact(net_buf_add(buf, hdr.len - 1U), hdr.len - 1U);
		}
	} else {
		buf = bt_buf_get_evt(hdr.evt, false, K_FOREVER);
		if (buf == NULL) {
			rx_discard(hdr.len);
			return;
		}
		net_buf_add_mem(buf, &hdr, sizeof(hdr));
		if (hdr.len != 0U) {
			rx_exact(net_buf_add(buf, hdr.len), hdr.len);
		}
	}
	/* bt_buf_get_evt() keeps the H4 type byte at buf->data[0]. The RW
	 * controller advertises several command credits; Zephyr's Host tracks
	 * one synchronous sent_cmd buffer, so exposing more than one lets
	 * initialization commands overwrite that slot before their completions
	 * arrive. Keep the byte-stream link serialized at this adapter boundary.
	 */
	if (hdr.evt == BT_HCI_EVT_CMD_COMPLETE && hdr.len >= 1U) {
		buf->data[1U + sizeof(hdr)] = 1U;
	} else if (hdr.evt == BT_HCI_EVT_CMD_STATUS && hdr.len >= 2U) {
		buf->data[1U + sizeof(hdr) + 1U] = 1U;
	}
	if ((hdr.evt == BT_HCI_EVT_CMD_COMPLETE) ||
	    (hdr.evt == BT_HCI_EVT_CMD_STATUS)) {
		/* Preserve the scheduling boundary that an external UART HCI
		 * transport naturally has between assembling a synchronous event
		 * and handing it to the Host RX workqueue. */
		vTaskDelay(1U);
	}
	trace_event(buf);
	(void)host_recv(dev, buf);
}

static void rx_acl(const struct device *dev)
{
	struct bt_hci_acl_hdr hdr;
	struct net_buf *buf;
	uint16_t len;

	rx_exact((uint8_t *)&hdr, sizeof(hdr));
	len = sys_le16_to_cpu(hdr.len);
	ipro_bt_host_h4_acl_rx_last_len = len;
	if (len > ipro_bt_host_h4_acl_rx_max_len) {
		ipro_bt_host_h4_acl_rx_max_len = len;
	}
	buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_FOREVER);
	if (buf == NULL || net_buf_tailroom(buf) < (sizeof(hdr) + len)) {
		ipro_bt_host_h4_acl_rx_oversize_count++;
		if (buf != NULL) {
			net_buf_unref(buf);
		}
		rx_discard(len);
		return;
	}
	net_buf_add_mem(buf, &hdr, sizeof(hdr));
	if (len != 0U) {
		rx_exact(net_buf_add(buf, len), len);
	}
	ipro_bt_host_h4_acl_rx_count++;
	ipro_bt_host_h4_acl_rx_bytes += len;
	trace_acl_l2cap("RX", buf->data, buf->len, false);
	trace_acl_l2cap("RX", buf->data, buf->len, true);
	(void)host_recv(dev, buf);
}

static void rx_sco(void)
{
    struct bt_hci_sco_hdr hdr;
    uint8_t payload[UINT8_MAX];
    uint16_t handle_flags;
    uint16_t handle;
    uint8_t packet_status;

    rx_exact((uint8_t *)&hdr, sizeof(hdr));
    if (hdr.len != 0U) {
        rx_exact(payload, hdr.len);
    }

    handle_flags = sys_le16_to_cpu(hdr.handle);
    handle = handle_flags & 0x0fffU;
    packet_status = (uint8_t)((handle_flags >> 12U) & 0x03U);
    ipro_bt_host_h4_sco_rx_count++;
    ipro_bt_host_h4_sco_rx_bytes += hdr.len;

    if (sco_rx_cb != NULL) {
        sco_rx_cb(handle, packet_status, payload, hdr.len,
                  sco_rx_user_data);
    } else {
        ipro_bt_host_h4_sco_rx_no_callback_count++;
    }
}

static void rx_iso(const struct device *dev)
{
	struct bt_hci_iso_hdr hdr;
	uint16_t len;

	rx_exact((uint8_t *)&hdr, sizeof(hdr));
	len = bt_iso_hdr_len(sys_le16_to_cpu(hdr.len));

#if defined(CONFIG_BT_ISO_RX)
	struct net_buf *buf;
#if defined(CONFIG_IPRO_BT_HOST_HCI_TRACE)
	static uint32_t iso_rx_cnt;
	static uint32_t iso_drop_cnt;
#endif

	/* ISO media packets are loss-tolerant, but blocking the single HCI RX
	 * adapter thread can stall following control events and ACL traffic.
	 * If the Host ISO pool is exhausted, drop this ISO packet and keep the
	 * byte stream synchronized; use HCI_TRACE to size the pool if drops occur.
	 */
	buf = bt_buf_get_rx(BT_BUF_ISO_IN, K_NO_WAIT);
	if (buf == NULL || net_buf_tailroom(buf) < (sizeof(hdr) + len)) {
		if (buf != NULL) {
			net_buf_unref(buf);
		}
#if defined(CONFIG_IPRO_BT_HOST_HCI_TRACE)
		iso_drop_cnt++;
		if ((iso_drop_cnt <= 5U) || ((iso_drop_cnt % 100U) == 0U)) {
			uint16_t handle = sys_le16_to_cpu(hdr.handle);
			uint16_t flags_len = sys_le16_to_cpu(hdr.len);

			IPRO_LOGD(LOG_TAG, "DROP ISO #%lu handle=0x%04x pb=%u ts=%u len=%u raw_len=0x%04x", (unsigned long)iso_drop_cnt, handle & 0x0fffU, (handle >> 12U) & 0x3U, (handle >> 14U) & 0x1U, len, flags_len);
		}
#endif
		rx_discard(len);
		return;
	}

	net_buf_add_mem(buf, &hdr, sizeof(hdr));
	if (len != 0U) {
		rx_exact(net_buf_add(buf, len), len);
	}
#if defined(CONFIG_IPRO_BT_HOST_HCI_TRACE)
	iso_rx_cnt++;
	if ((iso_rx_cnt <= 5U) || ((iso_rx_cnt % 500U) == 0U)) {
		uint16_t handle = sys_le16_to_cpu(hdr.handle);
		uint16_t flags_len = sys_le16_to_cpu(hdr.len);

		IPRO_LOGD(LOG_TAG, "RX ISO #%lu handle=0x%04x pb=%u ts=%u len=%u raw_len=0x%04x", (unsigned long)iso_rx_cnt, handle & 0x0fffU, (handle >> 12U) & 0x3U, (handle >> 14U) & 0x1U, len, flags_len);
	}
#endif
	(void)host_recv(dev, buf);
#else
	(void)dev;
	rx_discard(len);
#endif
}

static void rx_thread(void *arg)
{
	const struct device *dev = arg;

	for (;;) {
		uint8_t type;
		rx_exact(&type, 1);
		if (type == H4_EVT) {
			rx_event(dev);
		} else if (type == H4_ACL) {
			rx_acl(dev);
		} else if (type == H4_SCO) {
			rx_sco();
		} else if (type == H4_ISO) {
			rx_iso(dev);
		} else {
			/* ISO is enabled with the LE Audio profile after its pools are
			 * selected. Unknown packet types indicate stream desynchronization. */
			(void)type;
		}
	}
}

static int ipro_hci_open(const struct device *dev, bt_hci_recv_t recv)
{
	host_recv = recv;
	(void)k_sem_init(&rx_sem, 0, UINT16_MAX);
	(void)k_sem_init(&tx_lock, 1, 1);
	ipro_ble_ctlr_hcitl_host_set_rx_notify(controller_rx_notify);
	if (xTaskCreate(rx_thread, "ipro_hci_rx", 2048 / sizeof(StackType_t),
			(void *)dev, 6, NULL) != pdPASS) {
		return -ENOMEM;
	}
	hci_opened = true;
	return 0;
}

static void h4_write_all(const uint8_t *data, uint32_t len)
{
	uint32_t offset = 0;

	while (offset < len) {
		uint32_t count = ipro_ble_ctlr_hcitl_host_write(data + offset,
							       len - offset);
		if (count == 0U) {
			taskYIELD();
		} else {
			offset += count;
		}
	}
}

static int ipro_hci_send(const struct device *dev, struct net_buf *buf)
{
	(void)dev;
	trace_cmd(buf->data, buf->len);
	trace_acl_l2cap("TX", buf->data, buf->len, true);
	(void)k_sem_take(&tx_lock, K_FOREVER);
	h4_write_all(buf->data, buf->len);
	k_sem_give(&tx_lock);
	net_buf_unref(buf);
	return 0;
}

static const struct bt_hci_driver_api ipro_hci_api = {
	.open = ipro_hci_open,
	.send = ipro_hci_send,
};

const struct device ipro_bt_hci_device = {
	.name = "ipro_ble_controller",
	.api = &ipro_hci_api,
};

int hci_driver_ipro_ble_ctlr_init(void)
{
	return 0;
}

int ipro_bt_hci_sco_rx_register(ipro_bt_hci_sco_rx_cb_t callback,
				void *user_data)
{
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
	sco_rx_user_data = user_data;
	sco_rx_cb = callback;
	return 0;
#else
	(void)callback;
	(void)user_data;
	return -ENOTSUP;
#endif
}

int ipro_bt_hci_sco_send(uint16_t handle, uint8_t packet_status,
			 const uint8_t *payload, uint8_t len)
{
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
	uint8_t type = H4_SCO;
	struct bt_hci_sco_hdr hdr;

	if (!hci_opened) {
		return -EAGAIN;
	}
	if (handle > 0x0fffU || packet_status > 0x03U ||
	    (payload == NULL && len != 0U)) {
		return -EINVAL;
	}

	hdr.handle = sys_cpu_to_le16(handle | ((uint16_t)packet_status << 12U));
	hdr.len = len;
	(void)k_sem_take(&tx_lock, K_FOREVER);
	h4_write_all(&type, sizeof(type));
	h4_write_all((const uint8_t *)&hdr, sizeof(hdr));
	if (len != 0U) {
		h4_write_all(payload, len);
	}
	k_sem_give(&tx_lock);
	ipro_bt_host_h4_sco_tx_count++;
	ipro_bt_host_h4_sco_tx_bytes += len;
	return 0;
#else
	(void)handle;
	(void)packet_status;
	(void)payload;
	(void)len;
	return -ENOTSUP;
#endif
}
