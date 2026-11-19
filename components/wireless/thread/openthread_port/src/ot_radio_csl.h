/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/**
 * @file ot_radio_csl.h
 * @brief CSL receiver support of the IPRO7 OpenThread radio port (ot_radio_csl.c).
 */
#ifndef OT_RADIO_CSL_H_
#define OT_RADIO_CSL_H_

#include <stdbool.h>
#include <stdint.h>
#include <openthread/platform/radio.h>

/* Register the Enh-ACK security hook with the HAL (radio enable). */
void ot_radio_csl_init(void);

/* This device's extended address, over-the-air order (CCM* nonce). */
void ot_radio_csl_set_ext_addr(const uint8_t ext_addr[8]);

/* Fill the CSL IE and secure the HAL's transmit copy psdu (len without the
 * FCS) of aFrame. Returns 0, or a negative error when securing failed. The
 * CSL phase is for a typical CSMA delay: frames with a CSL IE go through
 * ot_radio_csl_prepare_exact() instead. */
int ot_radio_csl_tx_prepare(otRadioFrame *aFrame, uint8_t *psdu, uint8_t len);

/* A frame with a CSL IE, to be sent at an exact time (the phase must be
 * right for the moment it is sent, Thread 1.4.1 §3.2.6.3.5). */
bool ot_radio_csl_needs_exact_time(const uint8_t *psdu, uint8_t len);

/* ieee802154_hal_transmit_exact() prepare callback (ctx = the otRadioFrame):
 * CSL IE for the given SFD time (HAL clock) and security. */
int ot_radio_csl_prepare_exact(uint8_t *psdu, uint8_t len, uint64_t sfd_hal_us, void *ctx);

uint32_t ot_radio_csl_tx_sec_failures(void);

/* The receiver came on (otPlatRadioReceive): lead-time stats of CSL windows. */
void ot_radio_csl_note_rx_on(bool deferred);
void ot_radio_csl_dump(bool clear);
/** The next CSL sample time OpenThread announced (its us clock); false with CSL off. */
bool ot_radio_csl_next_sample_us(uint32_t *out);
/* A received frame (task context), for the CSL arrival statistics. */
void ot_radio_csl_note_rx_frame(const uint8_t *psdu, uint8_t len, uint64_t sfd_time);

#endif /* OT_RADIO_CSL_H_ */
