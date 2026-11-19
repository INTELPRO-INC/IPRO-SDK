/*
 *  Copyright (c) 2025, INTELPRO Inc.
 *  All rights reserved.
 *
 *  OpenThread configuration for IPRO7 platform
 *  Configuration is controlled via Kconfig in application proj.conf
 */

#ifndef OPENTHREAD_CORE_IPRO7_CONFIG_H_
#define OPENTHREAD_CORE_IPRO7_CONFIG_H_

/**
 * Include autoconf to get Kconfig settings
 */
#ifdef CONFIG_AUTOCONF_INCLUDED
#include "ipro_autoconf.h"
#endif

/**
 * Platform identification
 */
#define OPENTHREAD_CONFIG_PLATFORM_INFO "IPRO7"

/**
 * Logging Configuration
 * Controlled by CONFIG_OPENTHREAD_LOG_LEVEL in proj.conf
 */
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED

#ifdef CONFIG_OPENTHREAD_LOG_LEVEL
#define OPENTHREAD_CONFIG_LOG_LEVEL CONFIG_OPENTHREAD_LOG_LEVEL
#else
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_WARN  // Default
#endif

/* Runtime-adjustable log level: OPENTHREAD_CONFIG_LOG_LEVEL above is the
 * compiled-in MAXIMUM; the live level is set with otLoggingSetLevel() and the
 * OT CLI `log level <n>`. Apps default it to NONE at startup so the console is
 * quiet until logging is explicitly raised. */
#define OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE 1

/**
 * FreeRTOS Integration
 */
#define OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT 1

/**
 * Memory Configuration
 */
#ifdef CONFIG_OPENTHREAD_NUM_MESSAGE_BUFFERS
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS CONFIG_OPENTHREAD_NUM_MESSAGE_BUFFERS
#else
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS 44
#endif

#ifdef CONFIG_OPENTHREAD_MESSAGE_BUFFER_SIZE
#define OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE CONFIG_OPENTHREAD_MESSAGE_BUFFER_SIZE
#else
#define OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE 128
#endif

/* 6LoWPAN fragment-to-fragment reassembly timeout. Thread requires 2-4 s
 * (spec 1.4.1 3.3, Table 3-6 row 5.3); OT's default is 2. The embedding
 * video stream apps set 1 in their .config: a single lost fragment otherwise
 * pins ~15 message buffers for the full timeout, and at 5-7 fps of
 * 14-fragment frames that cascades into ReassemblyTimeout drops. */
#ifdef CONFIG_OPENTHREAD_6LOWPAN_REASSEMBLY_TIMEOUT
#define OPENTHREAD_CONFIG_6LOWPAN_REASSEMBLY_TIMEOUT CONFIG_OPENTHREAD_6LOWPAN_REASSEMBLY_TIMEOUT
#else
#define OPENTHREAD_CONFIG_6LOWPAN_REASSEMBLY_TIMEOUT 2
#endif

/* Largest IPv6 datagram. Thread's MTU is 1280 (spec 1.4.1 3.3, Table 3-6
 * row 4), OT's default. The embedding video stream apps set 1700 in their
 * .config: a frame is a 1648 B datagram (1600 B embedding + 8 UDP + 40 IPv6),
 * and with IP6 fragmentation disabled Ip6::Header::IsValid() rejects anything
 * above the cap on both the sender and the receiver (<= kMaxAssembledDatagram
 * = 2000). The Wi-Fi border router keeps 1700 as well: a backbone multicast
 * of up to 1280 B goes into the mesh inside an IPv6-in-IPv6 MPL tunnel
 * (+48 B), which a 1280 cap would drop. */
#ifdef CONFIG_OPENTHREAD_IP6_MAX_DATAGRAM_LENGTH
#define OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH CONFIG_OPENTHREAD_IP6_MAX_DATAGRAM_LENGTH
#elif defined(CONFIG_THREAD_WIFI_BORDER_ROUTER)
#define OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH 1700
#else
#define OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH 1280
#endif

#define OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE 1

/**
 * Radio Configuration  
 */
#ifdef CONFIG_OPENTHREAD_RADIO_COEX_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE 1
#else
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE 0
#endif

/**
 * CLI Configuration
 */
#ifdef CONFIG_OPENTHREAD_CLI_MAX_LINE_LENGTH
#define OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH CONFIG_OPENTHREAD_CLI_MAX_LINE_LENGTH
#else
#define OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH 256
#endif

/**
 * Network Configuration
 * Note: These may be defined by CMake via ot_option(), so use #ifndef guards
 */
#ifndef OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
#ifdef CONFIG_OPENTHREAD_IP6_SLAAC_ENABLE
#define OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE 1
#else
#define OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE 0  // Disabled by default to save memory
#endif
#endif

#ifndef OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
#ifdef CONFIG_OPENTHREAD_TMF_NETDATA_SERVICE_ENABLE
#define OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE 1
#else
#define OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE 1  // Enabled by default
#endif
#endif

/**
 * CSL (Coordinated Sampled Listening)
 * Note: These may be defined by CMake via ot_option(), so use #ifndef guards
 */
#ifndef OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
#ifdef CONFIG_OPENTHREAD_MAC_CSL_RECEIVER_ENABLE
#define OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE 1
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 1
#else
#define OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE 0
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 0
#endif
#endif

/**
 * CLI Features
 */
#ifdef CONFIG_THREAD_RCP_SOFTWARE_TIMING
#undef OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 1
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE 1
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE 1
#endif

#ifdef CONFIG_OPENTHREAD_CLI_TCP_ENABLE
#define OPENTHREAD_CONFIG_CLI_TCP_ENABLE 1
#else
#define OPENTHREAD_CONFIG_CLI_TCP_ENABLE 0
#endif

/**
 * Security Features
 * Note: These may be defined by CMake via ot_option(), so use #ifndef guards
 */
#ifndef OPENTHREAD_CONFIG_ECDSA_ENABLE
#ifdef CONFIG_OPENTHREAD_ECDSA_ENABLE
#define OPENTHREAD_CONFIG_ECDSA_ENABLE 1
#else
#define OPENTHREAD_CONFIG_ECDSA_ENABLE 0  // Disabled by default to save ~15KB
#endif
#endif

#ifndef OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#ifdef CONFIG_OPENTHREAD_SRP_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE 1
#else
#define OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE 0  // Disabled by default to save ~20KB
#endif
#endif

/**
 * MLE Configuration
 */
#ifdef CONFIG_OPENTHREAD_MLE_MAX_CHILDREN
#define OPENTHREAD_CONFIG_MLE_MAX_CHILDREN CONFIG_OPENTHREAD_MLE_MAX_CHILDREN
#else
#define OPENTHREAD_CONFIG_MLE_MAX_CHILDREN 16
#endif

#ifdef CONFIG_OPENTHREAD_MLE_IP_ADDRS_PER_CHILD
#define OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD CONFIG_OPENTHREAD_MLE_IP_ADDRS_PER_CHILD
#else
#define OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD 4
#endif

/* Attach backoff cap (OT default 20 min). A sleepy device that lost its
 * parent keeps its radio off between attempts, so a long cap means it can
 * stay detached long after the network is back. Apps that must reattach
 * quickly (a battery-powered sleepy device, for one) set these; others keep
 * OT defaults. */
#ifdef CONFIG_OPENTHREAD_ATTACH_BACKOFF_MAX_MS
#define OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MAXIMUM_INTERVAL CONFIG_OPENTHREAD_ATTACH_BACKOFF_MAX_MS
#endif
#ifdef CONFIG_OPENTHREAD_ATTACH_BACKOFF_JITTER_MS
#define OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_JITTER_INTERVAL CONFIG_OPENTHREAD_ATTACH_BACKOFF_JITTER_MS
#endif

/**
 * MAC Configuration
 * Note: These may be defined by CMake via ot_option(), so use #ifndef guards
 */
#ifndef OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
#ifdef CONFIG_OPENTHREAD_MAC_FILTER_ENABLE
#define OPENTHREAD_CONFIG_MAC_FILTER_ENABLE 1
#else
#define OPENTHREAD_CONFIG_MAC_FILTER_ENABLE 0
#endif
#endif

/* MAC retries in OpenThread (random delay before each retry) instead of the
 * lmac154 hardware retry loop, unless an app opts back into hardware retries
 * (Kconfig OPENTHREAD_MAC_HW_RETRANSMIT). */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
#ifdef CONFIG_OPENTHREAD_MAC_HW_RETRANSMIT
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE 0
#else
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE 1
#endif
#endif

#ifdef CONFIG_OPENTHREAD_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
#define OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE 1
#else
#define OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE 0
#endif

/**
 * Crypto Configuration
 */
#define OPENTHREAD_CONFIG_CRYPTO_LIB OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS

/**
 * Application Layer Features
 * Note: These may be defined by CMake via ot_option(), so use #ifndef guards
 */
#ifndef OPENTHREAD_CONFIG_COAP_API_ENABLE
#ifdef CONFIG_OPENTHREAD_COAP_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_API_ENABLE 1
#else
#define OPENTHREAD_CONFIG_COAP_API_ENABLE 0
#endif
#endif

#ifndef OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#ifdef CONFIG_OPENTHREAD_COAP_SECURE_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE 1
#else
#define OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE 0
#endif
#endif

/* ipro_embed_stream: CoAP Observe (RFC 7641) for the streaming control plane
 * (GET-Observe subscription on /embed). No Kconfig wires this, so define it
 * directly (gated by core CoAP). Block-wise (RFC 7959) and coaps/DTLS are
 * deliberately NOT enabled — the data plane stays raw-UDP single-frame chunks. */
#if OPENTHREAD_CONFIG_COAP_API_ENABLE
#ifndef OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE 1
#endif
#endif

#ifdef CONFIG_THREAD_WIFI_BORDER_ROUTER
#define OPENTHREAD_CONFIG_BORDER_ROUTING_MANAGED_EXTRA_RA_OPTIONS_ENABLE 1
#define OPENTHREAD_CONFIG_MULTICAST_DNS_PUBLIC_API_ENABLE 1
#define OPENTHREAD_CONFIG_BORDER_AGENT_MESHCOP_SERVICE_BASE_NAME "IPRO6S Thread Border Router"
#endif

#ifndef OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
#ifdef CONFIG_OPENTHREAD_BORDER_AGENT_ENABLE
#define OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE 1
#else
#define OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE 0
#endif
#endif

#ifndef OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
#ifdef CONFIG_OPENTHREAD_SNTP_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE 1
#else
#define OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE 0
#endif
#endif

#ifdef CONFIG_OPENTHREAD_UDP_FORWARD_ENABLE
#define OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE 1
#else
#define OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE 0
#endif

#ifdef CONFIG_OPENTHREAD_TIME_SYNC_ENABLE
#define OPENTHREAD_CONFIG_TIME_SYNC_ENABLE 1
#else
#define OPENTHREAD_CONFIG_TIME_SYNC_ENABLE 0
#endif

/**
 * Commissioner and Joiner
 * Note: These may be defined by CMake via ot_option(), so use #ifndef guards
 */
#ifndef OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
#ifdef CONFIG_OPENTHREAD_COMMISSIONER
#define OPENTHREAD_CONFIG_COMMISSIONER_ENABLE 1
#else
#define OPENTHREAD_CONFIG_COMMISSIONER_ENABLE 0
#endif
#endif

#ifndef OPENTHREAD_CONFIG_JOINER_ENABLE
#ifdef CONFIG_OPENTHREAD_JOINER
#define OPENTHREAD_CONFIG_JOINER_ENABLE 1
#else
#define OPENTHREAD_CONFIG_JOINER_ENABLE 0
#endif
#endif

/**
 * Network Data "Service" TLV. Lets end devices discover a gateway's RLOC via a
 * registered service (otServerAddService / otNetDataGetNextService) instead of
 * assuming gateway == Leader (which breaks when another FTD wins leadership,
 * e.g. after a gateway reboot). Off by default upstream.
 */
#ifndef OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
#define OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE 1
#endif

/**
 * DHCP6 and DNS
 * Note: These may be defined by CMake via ot_option(), so use #ifndef guards
 */
#ifndef OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE
#ifdef CONFIG_OPENTHREAD_DHCP6_CLIENT
#define OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE 1
#else
#define OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE 0
#endif
#endif

#ifndef OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
#ifdef CONFIG_OPENTHREAD_DHCP6_SERVER
#define OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE 1
#else
#define OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE 0
#endif
#endif

#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
#ifdef CONFIG_OPENTHREAD_DNS_CLIENT
#define OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE 1
#else
#define OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE 0
#endif
#endif

#endif /* OPENTHREAD_CORE_IPRO7_CONFIG_H_ */
