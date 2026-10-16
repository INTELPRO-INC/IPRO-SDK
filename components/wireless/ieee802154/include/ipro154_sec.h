/*
 * Copyright (c) 2026 IntelPro
 * SPDX-License-Identifier: Apache-2.0
 *
 * IEEE 802.15.4 transmit security (CCM*) and CSL IE filling for the IPRO
 * radio, shared by the Zephyr driver and the FreeRTOS OpenThread port. The
 * radio side secures frames (OpenThread radio capability TRANSMIT_SEC)
 * because the CSL IE phase is written at transmit time and the Enh-ACKs
 * secured from the MAC interrupt share the frame counter with them.
 * Software AES-128, no heap: callable from the MAC interrupt.
 */
#ifndef IPRO154_SEC_H_
#define IPRO154_SEC_H_

#include <stdint.h>
#include <stdbool.h>

#define IPRO154_SEC_MAX_KEYS 3
#define IPRO154_SEC_NONCE_LEN 13
#define IPRO154_CSL_US_PER_TEN_SYMBOLS 160u

#define IPRO154_SEC_RK_WORDS 44

struct ipro154_sec_key {
	bool valid;
	uint8_t key_index;   /* Key ID mode 1 */
	uint32_t rk[IPRO154_SEC_RK_WORDS]; /* AES-128 expanded round keys, big-endian words */
};

struct ipro154_sec {
	struct ipro154_sec_key keys[IPRO154_SEC_MAX_KEYS];
	uint32_t frame_counter;
	uint32_t fc_lowered, fc_lowered_by; /* stack set the counter below the value in use */
	uint8_t current_key_index; /* our outgoing key index (Key ID Mode 1), 0 = not set */
	uint32_t csl_period;       /* 10-symbol units, 0 = CSL off */
	uint64_t csl_rx_time_us;   /* expected sample time on the radio clock */
	/* last frame secured here, for `ipro154 stats` / offline verification */
	uint8_t last_psdu[127];
	uint8_t last_len;
	uint8_t last_nonce[IPRO154_SEC_NONCE_LEN];
	/* plaintext of the last secured frame, so a retransmission of a
	 * CSL-IE frame can get a fresh phase and be re-secured with the
	 * same frame counter (what the stack expects of the radio) */
	uint8_t plain_psdu[127];
	uint8_t plain_len;
	bool plain_valid;
};

/* What the transmit path needs to know about a frame's MHR. */
struct ipro154_frame_info {
	bool sec_enabled;
	uint8_t sec_level;
	uint8_t key_id_mode;
	uint8_t key_index;
	uint8_t mic_len;
	bool fc_suppressed;   /* 2015 frame counter suppression (not used by Thread) */
	uint8_t aux_off;      /* auxiliary security header offset */
	uint8_t payload_off;  /* first byte of the (private) MAC payload */
	int16_t csl_ie_off;   /* offset of the CSL IE content, -1 if none */
};

int ipro154_frame_parse(const uint8_t *psdu, uint8_t len, struct ipro154_frame_info *fi);

/* Critical section around the shared frame counter (task vs MAC interrupt),
 * provided by the port. */
unsigned int ipro154_sec_lock(void);
void ipro154_sec_unlock(unsigned int key);

/* Expand a key-ID-mode-1 key into k (runs the AES self-test once: -EIO when
 * it fails, k left invalid). Build all keys this way outside any lock, then
 * install them with ipro154_sec_keys_commit(). */
int ipro154_sec_key_make(struct ipro154_sec_key *k, uint8_t key_index, const uint8_t key[16]);
/* Replace the key table atomically against the MAC interrupt; current_index
 * is the stack's current outgoing key index (the Enh-ACKs use it). */
void ipro154_sec_keys_commit(struct ipro154_sec *s, const struct ipro154_sec_key keys[IPRO154_SEC_MAX_KEYS],
			     uint8_t current_index);
void ipro154_sec_set_frame_counter(struct ipro154_sec *s, uint32_t fc, bool only_if_larger);

/* Write the frame counter (unless the header is already final) and secure the
 * frame in place. ext_addr is this device's extended address as given to
 * the address filter (over-the-air order, least significant byte first). */
int ipro154_sec_secure(struct ipro154_sec *s, uint8_t *psdu, uint8_t len,
		       const struct ipro154_frame_info *fi, const uint8_t ext_addr[8], bool hdr_rdy);

/* Secure a software Enh-ACK in place (from the MAC interrupt): ack holds the
 * MHR with the aux security header and IEs, len bytes, no MIC yet. Sets the
 * key index to our current outgoing one (Thread 1.4.1 §3.2.7.5 / 802.15.4-2015
 * table row 7.3.3: not the acked frame's), assigns our next frame counter and
 * appends the MIC-32; returns the new length. */
int ipro154_sec_secure_enh_ack(struct ipro154_sec *s, uint8_t *ack, uint8_t len, uint8_t max,
			       const uint8_t ext_addr[8]);

bool ipro154_sec_is_retransmission(const struct ipro154_sec *s, const uint8_t *psdu, uint8_t len,
				   const struct ipro154_frame_info *fi);

/* Fill phase/period of the CSL IE for a frame whose SFD ends at sfd_end_us. */
void ipro154_csl_ie_fill(uint8_t *psdu, const struct ipro154_frame_info *fi,
			 const struct ipro154_sec *s, uint64_t sfd_end_us);

#endif /* IPRO154_SEC_H_ */
