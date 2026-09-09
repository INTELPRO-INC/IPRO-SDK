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

/* Per-app lwIP overrides for the native-WiFi build.
 *
 * Included (opt-in, override style) from
 * components/wireless/wifi6/wifi6_lwip_adapter/include/lwipopts.h BEFORE its
 * #ifndef defaults: anything defined here wins, anything left out falls back
 * to the adapter values. Structural options tied to the fhost datapath
 * (custom pbufs, PBUF_POOL_SIZE, TX headroom, MEM_SIZE=LWIP_HEAP_SIZE,
 * checksum hook) stay adapter-owned and cannot be overridden here.
 *
 * NOT the same file as lwipopts_app.h — that one belongs to the lwip-port
 * shim (whole-file replacement) and is only read by non-WiFi builds.
 *
 * Note (2026-07-10 investigation): the WiFi TCP TX in-flight window is
 * bounded by LWIP_HEAP_SIZE (wifi6/CMakeLists.txt), NOT by TCP_SND_BUF —
 * raise the heap first if TX throughput is the goal. TCP_WND (RX window)
 * defaults to MAC_RXQ_DEPTH(12)*MSS and IS a real per-app knob here.
 */
#ifndef LWIPOPTS_WIFI_APP_H_
#define LWIPOPTS_WIFI_APP_H_

/* Live-allocation tracing for the lwIP heap (debug instrument, 2026-07-30).
 *
 * Four hypotheses about who holds the ~3 KB/standby-cycle leak were each
 * eliminated by a different measurement -- pcb unsent/unacked kept by a graceful
 * close (SO_LINGER made no difference once it was VERIFIED armed with -dM),
 * references held by the TX path (net_buf_tx_balance: d_out/d_tx = 0.40, only
 * +0.7 frames/cycle), release waiting on a TX confirm (zero reclaim across a 60 s
 * fully-awake window), and powering down with the TX path busy (pdsgate: 1% of
 * entries, no correlation). Knowing which CALL SITES grow mem.used is not the
 * same as knowing which BLOCKS are still live at the end of a cycle, and only
 * the latter answers the question. So record (ptr, size, caller) per live
 * allocation and dump what survived, aggregated by allocating PC.
 *
 * Costs ~6 KB of RAM and a few instructions per malloc/free. Debug only.
 */
/* Live-allocation tracer: DEFAULT OFF. 512 slots plus per-alloc bookkeeping;
 * turn it on only to answer "which blocks are still held, and who took them". */
#define LWIP_MEM_TRACE                  0


#endif /* LWIPOPTS_WIFI_APP_H_ */
