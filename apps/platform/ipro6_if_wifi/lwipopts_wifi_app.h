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

/* Per-app lwIP overrides for ipro6_if_wifi (SMID SDIO-WiFi bridge).
 *
 * THIS is the file the WiFi build reads. Included (opt-in, override style) from
 * components/wireless/wifi6/wifi6_lwip_adapter/include/lwipopts.h BEFORE its
 * #ifndef defaults: anything defined here wins, anything left out falls back to
 * the adapter value.
 *
 * NOT the same file as lwipopts_app.h. That one is the lwip-port shim's hook
 * (whole-file-replacement semantics) and it is INERT for this app: the include
 * path resolves "lwipopts.h" to wifi6_lwip_adapter/include/lwipopts.h, so
 * lwip-port/config/lwipopts.h — where the lwipopts_app.h hook lives — is never
 * read here. This app shipped only lwipopts_app.h until 2026-07-28, i.e. it had
 * NO working per-app lwIP config at all, and edits to it changed nothing.
 *
 * Verified 2026-07-28 that this hook really fires: a probe build defining
 * MEMP_NUM_NETCONN 2 shrank memp_memory_NETCONN_base from 0x383 to 0x73 in the
 * ELF. (The app directory was already on the include path — no build-system
 * change was needed, only the correct filename.)
 *
 * OVERRIDABLE here (the adapter guards these with #ifndef):
 *   TCP_MSS, TCP_WND, TCP_SND_BUF, TCP_QUEUE_OOSEQ, TCP_RCV_SCALE,
 *   LWIP_WND_SCALE, TCPIP_MBOX_SIZE, IP_DEFAULT_TTL, IP_REASS_MAX_PBUFS,
 *   MEMP_NUM_{PBUF,NETBUF,NETCONN,TCP_SEG,UDP_PCB,REASSDATA},
 *   DEFAULT_{TCP,UDP,RAW}_RECVMBOX_SIZE, DEFAULT_ACCEPTMBOX_SIZE,
 *   DEFAULT_THREAD_{PRIO,STACKSIZE}
 *
 * NOT overridable — the adapter owns these and its definition wins (an attempt
 * here only earns a macro-redefined warning):
 *   MEM_SIZE (= LWIP_HEAP_SIZE, set by -D in components/wireless/wifi6/
 *   CMakeLists.txt — currently 4096), PBUF_POOL_SIZE (0),
 *   PBUF_LINK_ENCAPSULATION_HLEN (452), LWIP_SUPPORT_CUSTOM_PBUF.
 *   To change the lwIP heap, edit LWIP_HEAP_SIZE, not this file.
 *
 * Sizing note for this app specifically: the bridge is proven never to enter
 * lwIP for bridged data (tcp stats all zero after 766k frames); lwIP here
 * serves only the device's own STA management path (DHCP). So the lwIP knobs
 * are NOT a throughput lever for the SDIO datapath — do not tune them expecting
 * one. They are only worth touching to reclaim ram_wifi, and the whole lwIP
 * footprint there is ~8.7KB, so the ceiling is small.
 */
#ifndef LWIPOPTS_WIFI_APP_H_
#define LWIPOPTS_WIFI_APP_H_

/* No overrides — adapter defaults apply. This file exists so that per-app lwIP
 * tuning has a place that actually takes effect; add overrides above the
 * #endif and verify the effect in the ELF, not just in the source. */

#endif /* LWIPOPTS_WIFI_APP_H_ */
