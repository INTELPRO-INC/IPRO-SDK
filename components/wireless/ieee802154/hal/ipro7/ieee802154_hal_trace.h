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
 * @file ieee802154_hal_trace.h
 * @brief Frame trace of the IPRO7 802.15.4 HAL: every frame this radio
 *        receives or transmits (data/command frames after security, its own
 *        Enh-ACKs, the ACKs it receives), with the HAL's microsecond
 *        timestamp of the end of the SFD.
 *
 * A debugging aid for protocol verification without an external sniffer:
 * the records are dumped over the console and turned into a pcap on the host.
 * Built with CONFIG_IEEE802154_FRAME_TRACE (FreeRTOS) or
 * CONFIG_IEEE802154_IPRO_FRAME_TRACE (Zephyr); recording is off until
 * enabled at run time. Without the option the hooks compile to nothing.
 */
#ifndef IEEE802154_HAL_TRACE_H_
#define IEEE802154_HAL_TRACE_H_

#include <stdbool.h>
#include <stdint.h>

#if !defined(__ZEPHYR__)
#include <generated/autoconf.h>
#endif

#if defined(CONFIG_IEEE802154_FRAME_TRACE) || defined(CONFIG_IEEE802154_IPRO_FRAME_TRACE)
#define IPRO154_FRAME_TRACE 1
#else
#define IPRO154_FRAME_TRACE 0
#endif

/* record kinds (one character in the dump) */
#define HAL_TR_RX     'R' /* frame received, FCS good */
#define HAL_TR_RX_BAD 'X' /* frame received, FCS bad */
#define HAL_TR_TX     'T' /* data / command frame sent */
#define HAL_TR_ACK_TX 'A' /* Enh-ACK sent by this radio */
#define HAL_TR_ACK_RX 'K' /* ACK received for a frame of ours */

#if IPRO154_FRAME_TRACE
/* MAC interrupt or task context. psdu without FCS; sfd_us on the HAL clock. */
void hal_trace_add(char kind, const uint8_t *psdu, uint8_t len, uint32_t sfd_us, int8_t rssi);
/* A frame handed to the MAC (task): recorded at every TX end (the MAC's own
 * retries included), at tx_end_us (the TX_END event, calibrated) minus its
 * air time, until hal_trace_tx_drop() - the TX's final outcome. */
void hal_trace_tx_prepare(const uint8_t *psdu, uint8_t len);
void hal_trace_tx_done(uint32_t tx_end_us);
void hal_trace_tx_drop(void);
#else
/* not built in: the arguments (register reads in the MAC interrupt) are not evaluated */
#define hal_trace_add(kind, psdu, len, sfd_us, rssi) ((void)0)
#define hal_trace_tx_prepare(psdu, len)              ((void)0)
#define hal_trace_tx_done(tx_end_us)                 ((void)0)
#define hal_trace_tx_drop()                          ((void)0)
#endif

/* Where the TR lines go: one whole line per call ("\r\n" included), from the
 * caller of dump / pump. NULL (the default) prints with printf. */
typedef void (*hal_trace_emit_t)(const char *line);
void ieee802154_hal_trace_set_emit(hal_trace_emit_t fn);

/* Console API (always present; a note when the trace is not built in).
 * on: start/stop recording; dump: print and remove the records
 * ("TR <kind> <t_us> <ch> <rssi> <hex>" per frame, then "TR end ..."). */
void ieee802154_hal_trace_enable(bool on);
void ieee802154_hal_trace_clear(void);
void ieee802154_hal_trace_dump(void);
/* Streaming: print up to max waiting records (0 = all) without the end
 * line; "TR lost <n>" when records were overwritten. Returns the count. */
unsigned ieee802154_hal_trace_pump(unsigned max);

#endif /* IEEE802154_HAL_TRACE_H_ */
