/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK side of the 802.15.4 facade: the only translation unit that includes
 * both the SDK radio driver header and the RF front-end API.
 *
 * Bring-up mirrors the proven sequence of the apps/networking/thread apps: RF mode
 * ZB-only and zeroed ZB power offsets BEFORE ieee802154_init() (the RFPHY
 * init inside lmac154 configures the RX demodulator from the current mode),
 * hardware auto-ACK on, CCA mode 3 (carrier AND energy - ED-only reports
 * false busy on this silicon), then rf_optimize_by_channel() and
 * rf_set_tx_power_zb() once the RFPHY is up.
 */

#include "ieee802154_hal_trace.h"
#include <ipro_zephyr/ipro_hal_154.h>
#include <string.h>

#include "ieee802154.h"
#include "ieee802154_hal.h"
#include "lmac154.h"

/* components/wireless/rf/ipro7_rf */
extern void rf_set_bz_mode(uint8_t mode);
extern void rf_set_pwr_offset_zb(int8_t tx_pwr_os[16]);
extern void rf_optimize_by_channel(uint32_t channel_mhz);
extern uint8_t rf_set_tx_power_zb(int32_t target_power);

#define RF_BZ_MODE_ZB_ONLY 1
#define CHANNEL_TO_MHZ(ch) (2405U + 5U * ((uint32_t)(ch) - 11U))

static ipro154_event_cb_t user_cb;
static void *user_ctx;
static uint8_t cur_channel;
static int8_t cur_tx_power;

static void sdk_event(ieee802154_event_type_t event, ieee802154_frame_t *frame, void *ctx)
{
	struct ipro154_rx_frame rx;

	(void)ctx;
	if (user_cb == NULL) {
		return;
	}

	switch (event) {
	case IEEE802154_EVENT_TX_DONE:
		/* For an acknowledged unicast the HAL hands us the ACK MPDU
		 * (lmac154_ackFrameEvent): its Frame Pending bit is what a
		 * sleepy child polls for, so pass it up with the event. */
		if (frame != NULL && frame->length >= 3) {
			rx.psdu = frame->psdu;
			rx.len = frame->length;
			rx.rssi = frame->info.rx.rssi;
			rx.lqi = frame->info.rx.lqi;
			rx.timestamp_us = frame->info.rx.timestamp; /* 0 = unknown */
			rx.ack_fpb = frame->info.rx.frame_pending;
			user_cb(IPRO154_EVT_TX_DONE, &rx, user_ctx);
		} else {
			user_cb(IPRO154_EVT_TX_DONE, NULL, user_ctx);
		}
		break;
	case IEEE802154_EVENT_TX_NO_ACK:
		user_cb(IPRO154_EVT_TX_NO_ACK, NULL, user_ctx);
		break;
	case IEEE802154_EVENT_TX_CHANNEL_BUSY:
		user_cb(IPRO154_EVT_TX_CHANNEL_BUSY, NULL, user_ctx);
		break;
	case IEEE802154_EVENT_RX_DONE:
		if (frame == NULL) {
			return;
		}
		rx.psdu = frame->psdu;
		rx.len = frame->length;
		rx.rssi = frame->info.rx.rssi;
		rx.lqi = frame->info.rx.lqi;
		rx.timestamp_us = frame->info.rx.timestamp;
		rx.ack_fpb = frame->info.rx.frame_pending;
		user_cb(IPRO154_EVT_RX_DONE, &rx, user_ctx);
		break;
	default:
		break;
	}
}

int ipro154_init(const struct ipro154_init_cfg *cfg, ipro154_event_cb_t cb, void *ctx)
{
	ieee802154_config_t rcfg = {
		.channel = cfg->channel,
		.tx_power = cfg->tx_power,
		.pan_id = 0xFFFF,
		.short_addr = 0xFFFF,
		.promiscuous = false,
		.auto_ack = true,
		.cca_mode = IEEE802154_CCA_MODE_CARRIER_AND_ENERGY,
		.cca_threshold = cfg->cca_threshold,
	};
	int8_t zero_offsets[16] = {0};
	ieee802154_error_t err;

	if (cfg->ext_addr != NULL) {
		memcpy(rcfg.ext_addr.m8, cfg->ext_addr, 8);
	}
	user_cb = cb;
	user_ctx = ctx;
	cur_channel = cfg->channel;
	cur_tx_power = cfg->tx_power;

	rf_set_bz_mode(RF_BZ_MODE_ZB_ONLY);
	rf_set_pwr_offset_zb(zero_offsets);

	err = ieee802154_init(&rcfg);
	if (err != IEEE802154_OK && err != IEEE802154_ERROR_ALREADY_INITIALIZED) {
		return -(int)err;
	}
	rf_optimize_by_channel(CHANNEL_TO_MHZ(cfg->channel));
	(void)rf_set_tx_power_zb(cfg->tx_power);

	ieee802154_register_callback(sdk_event, NULL);
	return 0;
}

int ipro154_enable(void)
{
	if (ieee802154_enable() != IEEE802154_OK) {
		return -1;
	}
	return ieee802154_receive() == IEEE802154_OK ? 0 : -1;
}

int ipro154_disable(void)
{
	return ieee802154_disable() == IEEE802154_OK ? 0 : -1;
}

int ipro154_set_channel(uint8_t channel)
{
	if (ieee802154_set_channel(channel) != IEEE802154_OK) {
		return -1;
	}
	rf_optimize_by_channel(CHANNEL_TO_MHZ(channel));
	(void)rf_set_tx_power_zb(cur_tx_power);
	cur_channel = channel;
	return 0;
}

int ipro154_set_tx_power(int8_t dbm)
{
	if (ieee802154_set_tx_power(dbm) != IEEE802154_OK) {
		return -1;
	}
	(void)rf_set_tx_power_zb(dbm);
	cur_tx_power = dbm;
	return 0;
}

int ipro154_set_pan_id(uint16_t pan_id)
{
	return ieee802154_set_pan_id(pan_id) == IEEE802154_OK ? 0 : -1;
}

int ipro154_set_short_addr(uint16_t addr)
{
	return ieee802154_set_short_addr(addr) == IEEE802154_OK ? 0 : -1;
}

int ipro154_set_ext_addr_le(const uint8_t addr[8])
{
	ieee802154_ext_addr_t ext;

	memcpy(ext.m8, addr, 8);
	return ieee802154_set_ext_addr(&ext) == IEEE802154_OK ? 0 : -1;
}

int ipro154_set_promiscuous(bool enable)
{
	return ieee802154_set_promiscuous(enable) == IEEE802154_OK ? 0 : -1;
}

int ipro154_set_frame_pending(const uint8_t *addr, bool extended, bool pending)
{
	return ieee802154_set_frame_pending(addr,
					    extended ? IEEE802154_ADDR_MODE_EXTENDED
						     : IEEE802154_ADDR_MODE_SHORT,
					    pending) == IEEE802154_OK
		       ? 0
		       : -1;
}

extern void ieee802154_hal_dbg_dump(void); /* HAL debug counters, not in ieee802154_hal.h */

uint64_t ipro154_last_tx_end_us(void)
{
	/* register counts microseconds; the helper shifts by 4 once too often */
	return lmac154_getEventTimeUs(LMAC154_EVENT_TIME_TX_END) >> 4;
}

extern void ieee802154_hal_set_enh_ack_secure(int (*fn)(uint8_t *ack, uint8_t len, uint8_t max));
void ipro154_set_enh_ack_secure(int (*fn)(uint8_t *ack, uint8_t len, uint8_t max))
{
	ieee802154_hal_set_enh_ack_secure(fn);
}

extern void ieee802154_hal_set_csl_ie(uint16_t period_10sym, uint32_t sample_time_us);
void ipro154_set_csl_ie(uint16_t period_10sym, uint32_t sample_time_us)
{
	ieee802154_hal_set_csl_ie(period_10sym, sample_time_us);
}

extern bool ieee802154_hal_rx_busy(void);
bool ipro154_rx_busy(void)
{
	return ieee802154_hal_rx_busy();
}

void ipro154_dbg_dump(void)
{
	ieee802154_hal_dbg_dump();
}

extern void ieee802154_hal_set_enh_ack_prearm(uint8_t mode);
void ipro154_set_enh_ack_prearm(uint8_t mode)
{
	ieee802154_hal_set_enh_ack_prearm(mode);
}

extern void ieee802154_hal_set_enh_ack_prearm_lead(uint16_t us);
void ipro154_cfo_dump(bool clear)
{
	extern void ieee802154_hal_cfo_dump(bool clear);

	ieee802154_hal_cfo_dump(clear);
}

/* RF crystal capcode (AON_XTAL_CFG2: in [15:8], out [7:0]); in < 0 only reads */
uint32_t ipro154_xtal_capcode(int in, int out)
{
	extern int AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut);

	if (in >= 0) {
		(void)AON_Set_Xtal_CapCode((uint8_t)in, (uint8_t)(out >= 0 ? out : in));
	}
	return *(volatile uint32_t *)(0x3008F000u + 0x888u) & 0xffffu;
}

void ipro154_trace(int op)
{
	switch (op) {
	case 0: ieee802154_hal_trace_enable(false); break;
	case 1: ieee802154_hal_trace_enable(true); break;
	case 2: ieee802154_hal_trace_clear(); break;
	default: ieee802154_hal_trace_dump(); break;
	}
}

unsigned ipro154_trace_pump(unsigned max)
{
	return ieee802154_hal_trace_pump(max);
}

void ipro154_trace_emit(void (*fn)(const char *line))
{
	ieee802154_hal_trace_set_emit(fn);
}

bool ipro154_crc_report(int on)
{
	uint32_t p = (uint32_t)lmac154_getRxAcceptPolicy();

	if (on >= 0) {
		p = on ? (p | LMAC154_RX_ACCEPT_CRC_ERR) : (p & ~(uint32_t)LMAC154_RX_ACCEPT_CRC_ERR);
		lmac154_setRxAcceptPolicy((lmac154_rx_accept_policy_t)p);
	}
	return (p & LMAC154_RX_ACCEPT_CRC_ERR) != 0u;
}

void ipro154_set_enh_ack_prearm_lead(uint16_t us)
{
	ieee802154_hal_set_enh_ack_prearm_lead(us);
}

int ipro154_clear_frame_pending(void)
{
	return ieee802154_clear_frame_pending() == IEEE802154_OK ? 0 : -1;
}

static int ipro154_fill_frame(ieee802154_frame_t *frame, const struct ipro154_tx_frame *tx)
{
	if (tx->len > IPRO154_MAX_PSDU - IPRO154_FCS_LEN) {
		return -1;
	}
	memset(frame, 0, sizeof(*frame));
	memcpy(frame->psdu, tx->psdu, tx->len);
	/* ieee802154_frame_t.length on TX is the MPDU *without* the FCS: the
	 * HAL hands it straight to lmac154_triggerTxDelay() and the MAC
	 * appends the FCS itself (ieee802154_hal_lmac154.c). Adding the two
	 * FCS bytes here put two zero bytes on the air in front of the real
	 * FCS: beacons survived that (scans worked) but every MLE/UDP frame
	 * failed the peer's UDP checksum and Thread attach never completed.
	 * On RX the HAL appends the FCS, so received lengths do include it
	 * (CONFIG_IEEE802154_L2_PKT_INCL_FCS=y). */
	frame->length = tx->len;
	frame->info.tx.channel = tx->channel;
	frame->info.tx.tx_power = tx->tx_power;
	frame->info.tx.ack_request = tx->ack_request;
	frame->info.tx.csma_ca_enabled = tx->csma_ca;
	frame->info.tx.max_csma_backoffs = tx->max_csma_backoffs;
	frame->info.tx.max_frame_retries = tx->max_frame_retries;
	return 0;
}

int ipro154_transmit(const struct ipro154_tx_frame *tx)
{
	static ieee802154_frame_t frame;

	if (ipro154_fill_frame(&frame, tx) != 0) {
		return -1;
	}
	return ieee802154_transmit(&frame) == IEEE802154_OK ? 0 : -1;
}

int ipro154_transmit_exact(const struct ipro154_tx_frame *tx,
			   int (*prepare)(uint8_t *psdu, uint8_t len, uint64_t sfd_sym_us, void *ctx), void *ctx)
{
	static ieee802154_frame_t frame;
	ieee802154_error_t err;

	if (ipro154_fill_frame(&frame, tx) != 0) {
		return -1;
	}
	err = ieee802154_hal_transmit_exact(&frame, prepare, ctx);
	return err == IEEE802154_OK ? 0 : err == IEEE802154_ERROR_CHANNEL_BUSY ? -2 : -1;
}

int ipro154_transmit_at(const struct ipro154_tx_frame *tx, uint64_t target_us)
{
	static ieee802154_frame_t frame;
	uint64_t now = ipro154_timestamp_us();

	if (ipro154_fill_frame(&frame, tx) != 0) {
		return -1;
	}
	if (target_us <= now + IPRO154_TXTIME_GUARD_US) {
		return -2; /* too late to arm the MAC's timed trigger */
	}
	return ieee802154_transmit_delay(&frame, 0, (uint32_t)(target_us - now)) == IEEE802154_OK ? 0 : -1;
}

bool ipro154_cca_clear(int8_t threshold_dbm)
{
	return ieee802154_hal_cca(IEEE802154_CCA_MODE_CARRIER_AND_ENERGY, threshold_dbm);
}

int ipro154_energy_detect(uint8_t channel, uint16_t duration_ms, int8_t *energy_dbm)
{
	return ieee802154_energy_detect(channel, duration_ms, energy_dbm) == IEEE802154_OK ? 0 : -1;
}

uint64_t ipro154_timestamp_us(void)
{
	/* MAC symbol counter x 16 us: the clock the RX event timestamps and the
	 * timed-TX trigger use, so net_pkt timestamps, otPlatRadioGetNow() and
	 * TXTIME requests all agree. (ieee802154_get_timestamp() is the kernel
	 * uptime on Zephyr - a different clock.) */
	return ieee802154_hal_get_timestamp();
}

extern int EF_Ctrl_Read_MAC_Address(uint8_t mac[8]);
/* Only present when the Wi-Fi/mfg library is in the image. */
__attribute__((weak)) int mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload)
{
	(void)mac; (void)reload;
	return -1;
}

static bool mac_usable(const uint8_t *m, int n)
{
	bool zero = true, ones = true;

	for (int i = 0; i < n; i++) {
		zero = zero && m[i] == 0;
		ones = ones && m[i] == 0xff;
	}
	return !zero && !ones;
}

/* EUI-64 for 802.15.4: the board's MAC from the mfg partition (what Wi-Fi
 * uses) expanded with FF:FE, else the efuse MAC slot, else -1. */
int ipro154_read_eui64(uint8_t eui64[8])
{
	uint8_t m[8];

	if (mfg_media_read_macaddr_with_lock(m, 1) == 0 && mac_usable(m, 6)) {
		eui64[0] = m[0]; eui64[1] = m[1]; eui64[2] = m[2];
		eui64[3] = 0xff; eui64[4] = 0xfe;
		eui64[5] = m[3]; eui64[6] = m[4]; eui64[7] = m[5];
		return 0;
	}
	if (EF_Ctrl_Read_MAC_Address(m) == 0 && mac_usable(m, 8) && mac_usable(m + 3, 5)) {
		memcpy(eui64, m, 8);
		return 0;
	}
	return -1;
}
