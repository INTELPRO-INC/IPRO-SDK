/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Zephyr v4.4 host HCI transport over the AIC8800 D80 SDIO mailbox.
 *
 * The AIC combo chip carries a BR/EDR + LE controller next to its Wi-Fi
 * FullMAC, and the vendor release (components/wireless/wifi_aic8800, package
 * aic8800_release_d80_20261002) exposes it as a public transport:
 *
 *     aic_btsdio_init()                               aic_hci_mailbox.h
 *     aic_hci_transport_attach(cb, ctx, FLOW_HOST)    aic_hci_transport.h
 *     aic_hci_transport_send(type, packet, len, ...)
 *
 * Command / Event / ACL only, with the H4 type passed beside the packet rather
 * than in front of it. The vendor's own BLE demo (aic_btsdio.c, which earlier
 * packages made us delete from the archive so we could own aic_btsdio_recv) is
 * gone; the driver owns the mailbox and we are a client of it.
 *
 * Three contracts from the vendor header matter here and are enforced below:
 *
 *   - Callbacks run on the transport worker and must not block. We copy each
 *     packet into a slot ring and let a dedicated RX task hand it to the host,
 *     the same shape hci_ipro.c uses for the on-chip controller.
 *   - The effective ACL data MTU is AIC_HCI_MAX_ACL_DATA (1019), but the
 *     Controller reports 1021 in Read Buffer Size and the driver refuses to
 *     fragment silently. The host adapter has to clamp, so this file rewrites
 *     that event on its way through (see clamp_buffer_size()).
 *   - There is no SCO/eSCO transport at all, so Read Buffer Size is also
 *     rewritten to advertise zero SCO buffers and ipro_bt_hci_sco_send()
 *     returns -ENOTSUP.
 *
 * A fault callback means the Controller must be re-initialised for real before
 * anything re-attaches; the module's reset is shared with Wi-Fi, so that is the
 * application's call. We latch it, stop sending, and say so loudly.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/device.h>
#include <zephyr/drivers/bluetooth.h>
#include <zephyr/kernel.h>
#include <zephyr/net_buf.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>

#include "aic_hci_mailbox.h"
#include "aic_hci_transport.h"

#include "hci_driver_ipro_ble_ctlr.h"
#include "hci_event_bounds.h"
#include "ipro_log.h"

#define LOG_TAG "hci_aic"

#define H4_CMD 0x01
#define H4_ACL 0x02
#define H4_SCO 0x03
#define H4_EVT 0x04

/* Slot payload: H4 type byte + the largest packet the transport accepts. */
#define AIC_HCI_PKT_MAX      (1U + AIC_HCI_MAX_PACKET)
#define AIC_HCI_RX_SLOTS     8U
/* host_recv() runs the host's priority-event handling (command complete,
 * NOCP -> TX completion) synchronously on this task: 2 KB left 186 B on Reef
 * before any BR/EDR connection existed. */
#define AIC_HCI_RX_STACK     (4096U / sizeof(StackType_t))
#define AIC_HCI_RX_PRIO      6U
/* How long a send may wait for room in the transport's 16-deep TX queue.
 * Long enough to ride out one SDIO burst, short enough that a wedged
 * Controller surfaces as a host command timeout rather than a stuck task. */
#define AIC_HCI_SEND_TIMEOUT_MS 1000U

/* ---- RX slot ring: transport worker -> HCI RX task ---------------------- */
struct aic_hci_slot {
	uint16_t len;			/* bytes used in data[], including type */
	uint8_t data[AIC_HCI_PKT_MAX];
};

static struct aic_hci_slot rx_slots[AIC_HCI_RX_SLOTS];
static volatile uint8_t rx_head; /* next slot the transport worker fills */
static volatile uint8_t rx_tail; /* next slot the RX task drains */
static SemaphoreHandle_t rx_sem;
static StaticSemaphore_t rx_sem_storage;

static bt_hci_recv_t host_recv;
static bool hci_opened;
static bool hci_attached;
static volatile bool hci_faulted;
static uint32_t tx_token;

volatile uint32_t ipro_bt_host_h4_evt_oversize_count;
volatile uint32_t ipro_bt_host_h4_acl_rx_count;
volatile uint32_t ipro_bt_host_h4_acl_rx_bytes;
volatile uint32_t ipro_bt_host_h4_acl_rx_oversize_count;
volatile uint32_t ipro_bt_host_h4_sco_rx_count;
volatile uint32_t ipro_bt_host_h4_sco_rx_bytes;
volatile uint32_t ipro_bt_host_h4_sco_rx_no_callback_count;
volatile uint32_t ipro_bt_host_h4_sco_tx_count;
volatile uint32_t ipro_bt_host_h4_sco_tx_bytes;
volatile uint32_t ipro_bt_host_aic_rx_drop_count;    /* ring full */
volatile uint32_t ipro_bt_host_aic_rx_bad_count;     /* malformed packet */
volatile uint32_t ipro_bt_host_aic_tx_fail_count;
volatile uint32_t ipro_bt_host_aic_fault_count;
/* Host-flow-control accounting: ACL packets handed to the transport versus
 * completions the Controller reports back (Number Of Completed Packets). If
 * the second stops tracking the first, the host runs out of ACL credits and
 * every L2CAP channel stalls. */
volatile uint32_t ipro_bt_host_aic_acl_tx_count;
volatile uint32_t ipro_bt_host_aic_nocp_events;
volatile uint32_t ipro_bt_host_aic_nocp_packets;
/*
 * Frame capture for bring-up (bt_trace in the A2DP app): the next N ACL frames
 * in either direction are recorded into a RAM ring - time, direction, L2CAP CID
 * and the first payload bytes - and printed later from the shell. Nothing is
 * printed on the RX task or the TX processor while capturing, so the timing
 * being observed is not changed by observing it (the host's own debug level
 * floods a 115200 console, and a log line on the TX processor once overflowed
 * its stack).
 */
#define AIC_TRACE_RECS  256U
#define AIC_TRACE_BYTES 12U

struct aic_trace_rec {
	uint32_t t_ms;
	uint16_t len;		/* ACL data length */
	uint16_t cid;
	uint8_t dir;		/* 'R' / 'T' */
	uint8_t b[AIC_TRACE_BYTES];	/* L2CAP payload */
};

static struct aic_trace_rec trace_ring[AIC_TRACE_RECS];
static volatile uint32_t trace_left;
static volatile uint32_t trace_count;

static void trace_frame(char dir, const uint8_t *acl, uint16_t len)
{
	struct aic_trace_rec *r;
	uint32_t idx;

	if (trace_left == 0U || len < 8U) {
		return;
	}
	taskENTER_CRITICAL();
	if (trace_left == 0U) {
		taskEXIT_CRITICAL();
		return;
	}
	trace_left--;
	idx = trace_count++ % AIC_TRACE_RECS;
	taskEXIT_CRITICAL();

	r = &trace_ring[idx];
	r->t_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
	r->dir = (uint8_t)dir;
	r->len = (uint16_t)(len - 4U);
	/* Continuation fragments carry no L2CAP header. */
	r->cid = (((sys_get_le16(acl) >> 12) & 0x3U) == 0x1U) ? 0xffffU
							      : sys_get_le16(&acl[6]);
	memset(r->b, 0, sizeof(r->b));
	memcpy(r->b, &acl[8], MIN((uint16_t)(len - 8U), (uint16_t)AIC_TRACE_BYTES));
}

void ipro_bt_hci_aic_trace_arm(uint32_t frames)
{
	taskENTER_CRITICAL();
	trace_count = 0U;
	trace_left = frames;
	taskEXIT_CRITICAL();
}

void ipro_bt_hci_aic_trace_dump(void)
{
	uint32_t n = MIN(trace_count, AIC_TRACE_RECS);
	uint32_t first = trace_count - n;

	printf("%lu frames captured (%lu still armed)\r\n",
	       (unsigned long)trace_count, (unsigned long)trace_left);
	for (uint32_t i = 0U; i < n; i++) {
		const struct aic_trace_rec *r = &trace_ring[(first + i) % AIC_TRACE_RECS];

		printf("%6lu %c cid=%04x len=%4u |", (unsigned long)r->t_ms, r->dir,
		       r->cid, r->len);
		for (uint32_t j = 0U; j < AIC_TRACE_BYTES; j++) {
			printf(" %02x", r->b[j]);
		}
		printf("\r\n");
	}
}
volatile int ipro_bt_host_aic_fault_status;
/* Controller's own view, read back for bt_stat. */
uint8_t aic_hci_lmp_features[8];
uint16_t aic_hci_acl_max_len;

static ipro_bt_hci_sco_rx_cb_t sco_rx_cb;
static void *sco_rx_user_data;

/* ---- transport callbacks (transport worker context) --------------------- */

static void aic_rx_cb(void *ctx, uint8_t type, const uint8_t *packet, uint16_t len)
{
	uint8_t head = rx_head;
	uint8_t next = (uint8_t)((head + 1U) % AIC_HCI_RX_SLOTS);

	(void)ctx;
	if (!hci_opened || packet == NULL || len == 0U ||
	    len > AIC_HCI_PKT_MAX - 1U) {
		ipro_bt_host_aic_rx_bad_count++;
		return;
	}
	if (next == rx_tail) {
		ipro_bt_host_aic_rx_drop_count++;
		return;
	}
	/* Re-assemble the H4 framing the dispatch below (and hci_ipro.c) expects.
	 * The borrowed bytes must not outlive this callback, so copy now. */
	rx_slots[head].data[0] = type;
	memcpy(&rx_slots[head].data[1], packet, len);
	rx_slots[head].len = (uint16_t)(len + 1U);
	rx_head = next;
	(void)xSemaphoreGive(rx_sem);
}

static void aic_tx_done_cb(void *ctx, uint32_t token, int status)
{
	(void)ctx;
	if (status != 0) {
		ipro_bt_host_aic_tx_fail_count++;
		IPRO_LOGW(LOG_TAG, "mailbox submission failed token=%lu status=%d",
			  (unsigned long)token, status);
	}
}

static void aic_fault_cb(void *ctx, int status)
{
	(void)ctx;
	ipro_bt_host_aic_fault_count++;
	ipro_bt_host_aic_fault_status = status;
	if (!hci_faulted) {
		hci_faulted = true;
		IPRO_LOGE(LOG_TAG,
			  "transport FAULT status=%d - the Controller must be "
			  "re-initialised (power-cycle the module and re-run "
			  "aic_start) before Bluetooth can be used again",
			  status);
	}
}

static const struct aic_hci_callbacks aic_hci_cb = {
	.rx = aic_rx_cb,
	.tx_done = aic_tx_done_cb,
	.fault = aic_fault_cb,
};

/* ---- RX dispatch (HCI RX task context) ---------------------------------- */

static bool event_buffer_fits(struct net_buf *buf, const struct bt_hci_evt_hdr *hdr)
{
	if (ipro_hci_event_fits(net_buf_tailroom(buf), hdr->len)) {
		return true;
	}
	ipro_bt_host_h4_evt_oversize_count++;
	if (ipro_bt_host_h4_evt_oversize_count == 1U) {
		IPRO_LOGW(LOG_TAG, "DROP oversized HCI event 0x%02x len=%u room=%u",
			  hdr->evt, hdr->len, (unsigned)net_buf_tailroom(buf));
	}
	net_buf_unref(buf);
	return false;
}

/*
 * Read Buffer Size carries what the host will believe for the rest of the
 * session, and on this transport the Controller's answer is wrong twice over:
 * it reports an ACL MTU of 1021 while the mailbox envelope leaves room for
 * 1019, and it reports SCO buffers that no SCO path exists for. Patch both
 * before the host sees them - the alternative is the host fragmenting to 1021
 * and every such packet being rejected by aic_hci_transport_send().
 * `params` points at the command-complete return parameters (status first).
 */
static void clamp_buffer_size(uint8_t *params, uint16_t params_len)
{
	uint16_t reported;

	if (params_len < sizeof(struct bt_hci_rp_read_buffer_size) ||
	    params[0] != 0U) {
		return;
	}
	reported = sys_get_le16(&params[1]);
	aic_hci_acl_max_len = reported;
	if (reported > AIC_HCI_MAX_ACL_DATA) {
		sys_put_le16(AIC_HCI_MAX_ACL_DATA, &params[1]);
		IPRO_LOGI(LOG_TAG, "ACL MTU %u -> %u (mailbox envelope)",
			  reported, (unsigned)AIC_HCI_MAX_ACL_DATA);
	}
	if (params[3] != 0U || sys_get_le16(&params[6]) != 0U) {
		IPRO_LOGI(LOG_TAG, "hiding %u SCO buffers of %u bytes (no SCO transport)",
			  sys_get_le16(&params[6]), params[3]);
		params[3] = 0U;			/* sco_max_len */
		sys_put_le16(0U, &params[6]);	/* sco_max_num */
	}
}

static void rx_event(const struct device *dev, const uint8_t *pkt, uint16_t len)
{
	struct bt_hci_evt_hdr hdr;
	struct net_buf *buf;
	bool discardable = false;

	if (len < sizeof(hdr)) {
		ipro_bt_host_aic_rx_bad_count++;
		return;
	}
	memcpy(&hdr, pkt, sizeof(hdr));
	if (hdr.len != len - sizeof(hdr)) {
		ipro_bt_host_aic_rx_bad_count++;
		return;
	}
	if (hdr.evt == BT_HCI_EVT_LE_META_EVENT && hdr.len != 0U) {
		uint8_t subevent = pkt[sizeof(hdr)];

		discardable = subevent == BT_HCI_EVT_LE_ADVERTISING_REPORT ||
			      subevent == BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT;
	}
	buf = bt_buf_get_evt(hdr.evt, discardable, discardable ? K_NO_WAIT : K_FOREVER);
	if (buf == NULL || !event_buffer_fits(buf, &hdr)) {
		return;
	}
	/* bt_buf_get_evt() already placed the H4 type byte at data[0]. */
	net_buf_add_mem(buf, pkt, len);

	/* Zephyr's host tracks one outstanding command; keep the controller's
	 * credit count at 1 like the on-chip transport does. */
	if (hdr.evt == BT_HCI_EVT_CMD_COMPLETE && hdr.len >= 1U) {
		buf->data[1U + sizeof(hdr)] = 1U;
		if (hdr.len >= 3U) {
			/* In buf: type, evt hdr, ncmd, opcode, then parameters. */
			uint8_t *params = &buf->data[1U + sizeof(hdr) + 1U + 2U];
			uint16_t opcode = sys_get_le16(&pkt[sizeof(hdr) + 1U]);
			const uint8_t *rp = &pkt[sizeof(hdr) + 3U];
			uint16_t rplen = (uint16_t)(hdr.len - 3U);

			if (opcode == BT_HCI_OP_READ_BUFFER_SIZE) {
				clamp_buffer_size(params, rplen);
			} else if (opcode == BT_HCI_OP_READ_LOCAL_FEATURES && rplen >= 9U) {
				memcpy(aic_hci_lmp_features, rp + 1, 8);
				IPRO_LOGI(LOG_TAG, "LMP features %02x %02x %02x %02x %02x %02x %02x %02x (BR/EDR %s)",
					  rp[1], rp[2], rp[3], rp[4], rp[5], rp[6], rp[7], rp[8],
					  (rp[5] & 0x20U) ? "NOT supported" : "supported");
			} else if (opcode == BT_HCI_OP_READ_LOCAL_VERSION_INFO && rplen >= 9U) {
				IPRO_LOGI(LOG_TAG, "HCI v%u rev 0x%04x, LMP v%u subver 0x%04x, manufacturer 0x%04x",
					  rp[1], sys_get_le16(rp + 2), rp[4], sys_get_le16(rp + 7),
					  sys_get_le16(rp + 5));
			}
		}
	} else if (hdr.evt == BT_HCI_EVT_CMD_STATUS && hdr.len >= 2U) {
		buf->data[1U + sizeof(hdr) + 1U] = 1U;
	} else if (hdr.evt == BT_HCI_EVT_NUM_COMPLETED_PACKETS && hdr.len >= 1U) {
		uint8_t handles = pkt[sizeof(hdr)];

		ipro_bt_host_aic_nocp_events++;
		for (uint8_t i = 0U; i < handles &&
		     sizeof(hdr) + 1U + 4U * (i + 1U) <= len; i++) {
			ipro_bt_host_aic_nocp_packets +=
				sys_get_le16(&pkt[sizeof(hdr) + 1U + 4U * i + 2U]);
		}
	}
	(void)host_recv(dev, buf);
}

static void rx_acl(const struct device *dev, const uint8_t *pkt, uint16_t len)
{
	struct bt_hci_acl_hdr hdr;
	struct net_buf *buf;
	uint16_t dlen;

	if (len < sizeof(hdr)) {
		ipro_bt_host_aic_rx_bad_count++;
		return;
	}
	memcpy(&hdr, pkt, sizeof(hdr));
	dlen = sys_le16_to_cpu(hdr.len);
	if (dlen != len - sizeof(hdr)) {
		ipro_bt_host_aic_rx_bad_count++;
		return;
	}
	buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_FOREVER);
	if (buf == NULL || net_buf_tailroom(buf) < len) {
		ipro_bt_host_h4_acl_rx_oversize_count++;
		if (buf != NULL) {
			net_buf_unref(buf);
		}
		return;
	}
	trace_frame('R', pkt, len);
	net_buf_add_mem(buf, pkt, len);
	ipro_bt_host_h4_acl_rx_count++;
	ipro_bt_host_h4_acl_rx_bytes += dlen;
	(void)host_recv(dev, buf);
}

static void rx_thread(void *arg)
{
	const struct device *dev = arg;

	for (;;) {
		(void)xSemaphoreTake(rx_sem, portMAX_DELAY);
		while (rx_tail != rx_head) {
			struct aic_hci_slot *slot = &rx_slots[rx_tail];
			const uint8_t *pkt = slot->data + 1;
			uint16_t len = (uint16_t)(slot->len - 1U);

			switch (slot->data[0]) {
			case H4_EVT:
				rx_event(dev, pkt, len);
				break;
			case H4_ACL:
				rx_acl(dev, pkt, len);
				break;
			default:
				/* The transport delivers COMMAND/EVENT/ACL only. */
				ipro_bt_host_aic_rx_bad_count++;
				break;
			}
			rx_tail = (uint8_t)((rx_tail + 1U) % AIC_HCI_RX_SLOTS);
		}
	}
}

/* ---- bt_hci_driver_api --------------------------------------------------- */

static int aic_hci_open(const struct device *dev, bt_hci_recv_t recv)
{
	int rc;

	if (aic_btsdio_ready() == 0) {
		/* aic_btsdio_init() needs the Wi-Fi driver up: it is the SDIO
		 * probe that loads the chip's BT patch tables. */
		rc = aic_btsdio_init();
		if (rc != 0) {
			IPRO_LOGE(LOG_TAG, "aic_btsdio_init failed rc=%d "
					   "(run aic_start first)", rc);
			return -ENODEV;
		}
	}

	host_recv = recv;
	rx_head = 0U;
	rx_tail = 0U;
	if (rx_sem == NULL) {
		rx_sem = xSemaphoreCreateCountingStatic(AIC_HCI_RX_SLOTS, 0, &rx_sem_storage);
		if (xTaskCreate(rx_thread, "aic_hci_rx", AIC_HCI_RX_STACK, (void *)dev,
				AIC_HCI_RX_PRIO, NULL) != pdPASS) {
			return -ENOMEM;
		}
	}
	/* Mark open before attaching: the first event can arrive inside attach. */
	hci_opened = true;
	hci_faulted = false;
	rc = aic_hci_transport_attach(&aic_hci_cb, NULL, AIC_HCI_FLOW_HOST);
	if (rc != 0) {
		hci_opened = false;
		IPRO_LOGE(LOG_TAG, "aic_hci_transport_attach failed rc=%d", rc);
		return rc == AIC_HCI_BUSY ? -EBUSY : -EIO;
	}
	hci_attached = true;
	IPRO_LOGI(LOG_TAG, "HCI transport: AIC8800 SDIO mailbox, host flow control, "
			   "ACL MTU %u", (unsigned)AIC_HCI_MAX_ACL_DATA);
	return 0;
}

static int aic_hci_close(const struct device *dev)
{
	int rc;

	(void)dev;
	if (!hci_attached) {
		return 0;
	}
	rc = aic_hci_transport_detach();
	if (rc != 0) {
		/* BUSY means the Controller still has links or outstanding
		 * commands; the host has to tear those down first. */
		IPRO_LOGW(LOG_TAG, "detach refused rc=%d", rc);
		return rc == AIC_HCI_BUSY ? -EBUSY : -EIO;
	}
	hci_attached = false;
	hci_opened = false;
	return 0;
}

static int aic_hci_send(const struct device *dev, struct net_buf *buf)
{
	uint16_t len;
	uint8_t type;
	int rc;

	(void)dev;
	if (!hci_opened || hci_faulted || buf->len < 2U ||
	    buf->len > AIC_HCI_PKT_MAX) {
		net_buf_unref(buf);
		return hci_faulted ? -EIO : -EINVAL;
	}
	/* buf->data[0] is the H4 type; this transport takes it separately. */
	type = buf->data[0];
	len = (uint16_t)(buf->len - 1U);
	if (type == H4_ACL) {
		trace_frame('T', &buf->data[1], len);
	}
	rc = aic_hci_transport_send(type, &buf->data[1], len, ++tx_token,
				    AIC_HCI_SEND_TIMEOUT_MS);
	net_buf_unref(buf);
	if (rc == 0 && type == H4_ACL) {
		ipro_bt_host_aic_acl_tx_count++;
	}
	if (rc != 0) {
		ipro_bt_host_aic_tx_fail_count++;
		IPRO_LOGW(LOG_TAG, "send type=%u len=%u rc=%d", type, len, rc);
		switch (rc) {
		case AIC_HCI_INVALID:
			return -EINVAL;
		case AIC_HCI_BUSY:
		case AIC_HCI_OVERFLOW:
			return -ENOBUFS;
		case AIC_HCI_TIMEOUT:
			return -ETIMEDOUT;
		default:
			return -EIO;
		}
	}
	return 0;
}

static const struct bt_hci_driver_api aic_hci_api = {
	.open = aic_hci_open,
	.close = aic_hci_close,
	.send = aic_hci_send,
};

const struct device ipro_bt_hci_device = {
	.name = "aic8800_sdio_hci",
	.api = &aic_hci_api,
};

/* Same entry point applications call before bt_enable() on the on-chip path. */
bool ipro_bt_hci_aic_opened(void)
{
	return hci_opened && !hci_faulted;
}

void ipro_bt_hci_aic_stats(struct aic_hci_stats *out)
{
	if (out != NULL) {
		aic_hci_transport_stats(out);
	}
}

int hci_driver_ipro_ble_ctlr_init(void)
{
#if defined(CONFIG_BT_GATT_GAP_SERVICE)
	extern void ipro_bt_gatt_gap_service_link_anchor(void);
	ipro_bt_gatt_gap_service_link_anchor();
#endif
	return 0;
}

/*
 * SCO: the mailbox transport carries Command/Event/ACL only, and Read Buffer
 * Size is rewritten above so the host never allocates SCO buffers. These two
 * stay because the host port's callback tables reference them.
 */
int ipro_bt_hci_sco_rx_register(ipro_bt_hci_sco_rx_cb_t callback, void *user_data)
{
	sco_rx_user_data = user_data;
	sco_rx_cb = callback;
	return -ENOTSUP;
}

int ipro_bt_hci_sco_send(uint16_t handle, uint8_t packet_status,
			 const uint8_t *payload, uint8_t len)
{
	(void)handle;
	(void)packet_status;
	(void)payload;
	(void)len;
	return -ENOTSUP;
}
