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

#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define IPRO_WIFI_SCAN_CAPACITY 64u
#define IPRO_WIFI_SCAN_ENTRY_SIZE 42u
#define IPRO_WIFI_SCAN_SNAPSHOT_SIZE 2692u
/* Byte-only entry ABI. SSID is length-delimited (not a C string); zero length
 * denotes a hidden SSID. IPRO6S only reports 2.4 GHz channels 1..14 here.
 * auth is an SDK classification, NOT the Matter security bitmap:
 * 0=open, 1=WEP, 2=WPA-PSK, 3=WPA2-PSK, 4=WPA/WPA2-PSK,
 * 5=enterprise, 6=WPA3-SAE, 7=WPA2-PSK/WPA3-SAE, 255=unknown.
 * A discovered network need not be supported by the connect/profile APIs. */
typedef struct {
    uint8_t ssid[32], bssid[6];
    uint8_t ssid_length, channel;
    int8_t rssi;
    uint8_t auth;
} ipro_wifi_scan_entry;

typedef struct {
    uint32_t count;
    ipro_wifi_scan_entry entries[IPRO_WIFI_SCAN_CAPACITY];
} ipro_wifi_scan_snapshot;

/* Read-only cache snapshot, NOT scan initiation or completion. A caller must
 * correlate its own successful scan completion before using this for a Matter
 * ScanNetworks response; the cache can contain unexpired older scan results.
 * Requires running task + CHIP stack lock, after Wi-Fi manager initialization;
 * manager reinitialization/deinitialization must not overlap this call.
 * Never wait while holding OT/lwIP locks. Cache mutex is acquired with zero wait.
 * Return 0=success (possibly empty), 1=busy, -1=invalid/unavailable.
 * No allocations/callbacks or borrowed pointers; copy completes under the same
 * mutex used by SDK cache writers. Expired entries and invalid SSID lengths or
 * channels are omitted; this is not full BSSID/security validation.
 * Valid-size output is cleared on failure; size mismatch is left untouched.
 * Caller supplies writable, non-overlapping storage of exactly the ABI size. */
int ipro_wifi_scan_cache_read(ipro_wifi_scan_snapshot *out, uint32_t size);

#define IPRO_WIFI_SCAN_SUCCESS 0u
#define IPRO_WIFI_SCAN_FAILED 1u
#define IPRO_WIFI_SCAN_TIMEOUT 2u
/* Task-only, thread-safe single-operation mailbox. Copies SSID (0..32 bytes,
 * zero length means wildcard); no caller buffer is retained. timeout_ms is
 * 1..60000. Input and output storage must not overlap. Returns 0=worker queued,
 * 1=busy, -1=invalid/closed/create failure. Queued does NOT mean scan accepted.
 * IDs never repeat before reboot; use only this module for nonzero SDK IDs.
 * Poll take: 0=one result consumed, 1=pending, -1=invalid/already consumed.
 * Timeout is observed by take or completion, not an independent timer callback.
 * A timeout does not cancel: new starts stay busy until worker/firmware drains.
 * Uncertain admission also retains pending state until its callback arrives;
 * if it never arrives, no drain success is claimed. No task is forcibly freed.
 * Requires stable initialized Wi-Fi manager; no VIF reset/reinit while pending.
 * This does not fence other scan/connect writers or freeze the cache snapshot.
 * Close is terminal and discards results; returns 1 until this module's work
 * drains, 0 when drained. It does NOT drain all SDK scans or stop radio traffic. */
int ipro_wifi_scan_start(const uint8_t *ssid, uint32_t length, uint32_t timeout_ms, uint32_t *operation);
int ipro_wifi_scan_take(uint32_t operation, uint32_t *result);
/* Atomically consume status plus observations belonging to this request, not
 * the manager cache. Same return codes/context as take(). Output must have exact
 * ABI size and not overlap result. Valid-sized output is cleared on every call;
 * only successful scan completion exposes entries. BSSID duplicates are updated,
 * at most 64 unique BSSIDs retained (later new ones dropped). Results freeze on
 * completion and cannot mix with subsequent/legacy scans. Taking via either API
 * consumes the same result; caller must copy before starting another operation.
 * Freshness assumes firmware result-before-CFM ordering and no VIF reset. */
int ipro_wifi_scan_take_results(uint32_t operation, uint32_t *result,
                                ipro_wifi_scan_snapshot *out, uint32_t size);
int ipro_wifi_scan_close(void);
/* Nonterminal fail-safe handoff, task-only. Discard the current result and
 * skip queued workers that have not begun. Running/submitted/uncertain commands
 * keep their buffers until worker + matching completion drain. Poll 1=busy,
 * 0=drained, -1=invalid context/terminally closed. No forced cancellation and
 * timeout is NOT drainage. Caller must serialize this with ALL module starts
 * and takes until handoff finishes; this is not an admission lease or a fence
 * for legacy scans, CONNECT, DHCP, or other Wi-Fi writers. New starts can resume
 * after 0. Never use this to reopen terminal close. */
int ipro_wifi_scan_quiesce(void);

#ifdef __cplusplus
}
#endif
