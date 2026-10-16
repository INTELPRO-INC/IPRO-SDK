/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Facade over the SDK unified IEEE 802.15.4 driver
 * (components/wireless/ieee802154). The SDK header and Zephyr's
 * <zephyr/net/ieee802154_radio.h> both define ieee802154_init() and
 * ieee802154_event_cb_t, so the Zephyr radio driver talks to the SDK through
 * these ipro154_* functions only (zephyr/hal_glue/ipro_ieee802154_glue.c).
 */

#ifndef IPRO_ZEPHYR_IPRO_HAL_154_H_
#define IPRO_ZEPHYR_IPRO_HAL_154_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPRO154_MAX_PSDU 127
#define IPRO154_FCS_LEN  2

enum ipro154_event {
	IPRO154_EVT_TX_DONE,
	IPRO154_EVT_TX_NO_ACK,
	IPRO154_EVT_TX_CHANNEL_BUSY,
	IPRO154_EVT_RX_DONE,
};

struct ipro154_rx_frame {
	const uint8_t *psdu;
	uint8_t len;
	int8_t rssi;
	uint8_t lqi;
	uint32_t timestamp_us;
	bool ack_fpb;
};

struct ipro154_tx_frame {
	const uint8_t *psdu;   /* MHR + payload, without FCS */
	uint8_t len;
	uint8_t channel;
	int8_t tx_power;
	bool ack_request;
	bool csma_ca;
	uint8_t max_csma_backoffs;
	uint8_t max_frame_retries;
};

struct ipro154_init_cfg {
	uint8_t channel;
	int8_t tx_power;
	int8_t cca_threshold;
	const uint8_t *ext_addr; /* 8 bytes, same order as ieee802154_set_ext_addr() (as printed) */
};

/* Called from the radio ISR. rx is NULL for TX events, except TX_DONE of an
 * acknowledged frame, where it carries the received ACK (for its Frame
 * Pending bit). */
typedef void (*ipro154_event_cb_t)(enum ipro154_event evt, const struct ipro154_rx_frame *rx,
				   void *ctx);

int ipro154_init(const struct ipro154_init_cfg *cfg, ipro154_event_cb_t cb, void *ctx);
int ipro154_enable(void);
int ipro154_disable(void);
int ipro154_set_channel(uint8_t channel);
int ipro154_set_tx_power(int8_t dbm);
int ipro154_set_pan_id(uint16_t pan_id);
int ipro154_set_short_addr(uint16_t addr);
int ipro154_set_ext_addr_le(const uint8_t addr[8]);
int ipro154_set_promiscuous(bool enable);
int ipro154_set_frame_pending(const uint8_t *addr, bool extended, bool pending);
int ipro154_clear_frame_pending(void); /* drop every source-match entry */
void ipro154_set_csl_ie(uint16_t period_10sym, uint32_t sample_time_us); /* CSL IE content for our Enh-ACKs */
/* Security for the software Enh-ACK (called from the MAC interrupt): fill the
 * frame counter and append the MIC; return the new length, <0 to send it
 * unsecured. */
void ipro154_set_enh_ack_secure(int (*fn)(uint8_t *ack, uint8_t len, uint8_t max));
void ipro154_dbg_dump(void);           /* HAL counters to stdout (shell context) */
void ipro154_set_enh_ack_prearm(uint8_t mode);      /* Enh-ACK prepared during RX: 0 off, 1 measure, 2 on */
void ipro154_cfo_dump(bool clear);                  /* per-sender frequency offset / SFD corr / RSSI */
uint32_t ipro154_xtal_capcode(int in, int out);     /* RF crystal capcode in<<8|out; in < 0 reads */
void ipro154_trace(int op);                        /* frame trace: 1 on, 0 off, 2 clear, 3 dump */
unsigned ipro154_trace_pump(unsigned max);         /* frame trace: print up to max waiting records */
void ipro154_trace_emit(void (*fn)(const char *line)); /* frame trace: line output (NULL = printf) */
bool ipro154_crc_report(int on);                    /* CRC-failed frames counted by the HAL (on 1/off 0/query -1) */
void ipro154_set_enh_ack_prearm_lead(uint16_t us);  /* timed-trigger lead for it, us (default 128) */
bool ipro154_rx_busy(void);            /* a frame or our Enh-ACK for it is on the air */
uint64_t ipro154_last_tx_end_us(void); /* MAC timestamp of the last TX end (symbol clock) */
int ipro154_transmit(const struct ipro154_tx_frame *tx);
bool ipro154_cca_clear(int8_t threshold_dbm);
int ipro154_energy_detect(uint8_t channel, uint16_t duration_ms, int8_t *energy_dbm);
uint64_t ipro154_timestamp_us(void); /* MAC symbol-counter clock, 16 us resolution */

/* Timed transmit at target_us on the ipro154_timestamp_us() clock, no CSMA.
 * -2 when the target is closer than IPRO154_TXTIME_GUARD_US. */
#define IPRO154_TXTIME_GUARD_US 200
int ipro154_transmit_at(const struct ipro154_tx_frame *tx, uint64_t target_us);
/* Transmit at a time fixed before the frame is final (a CSL IE frame), CSMA-CA
 * done by the HAL: prepare() writes and secures the HAL's copy of the frame for
 * its SFD time (symbol clock, us). 0 = sent, -2 = channel busy, -1 = error. */
int ipro154_transmit_exact(const struct ipro154_tx_frame *tx,
			   int (*prepare)(uint8_t *psdu, uint8_t len, uint64_t sfd_sym_us, void *ctx), void *ctx);
/* 8-byte MAC from the efuse; -1 if unprogrammed. */
int ipro154_read_eui64(uint8_t eui64[8]);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_ZEPHYR_IPRO_HAL_154_H_ */
