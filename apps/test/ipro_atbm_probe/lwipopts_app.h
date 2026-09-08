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

/*
 * Per-app lwIP configuration for ipro_atbm_probe.
 *
 * Picked up by components/network/lwip/lwip/lwip-port/config/lwipopts.h via
 * __has_include("lwipopts_app.h") and REPLACES the SDK default entirely.
 * Snapshot of the SDK default config (2026-07-06, stock queue depths).
 * Tune lwIP memory usage / behavior for this app HERE -- do not edit the
 * shared default under components/network/lwip.
 */
#ifndef __LWIPOPTS_APP_H__
#define __LWIPOPTS_APP_H__

#include "stdbool.h"
#include "sys/time.h"

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    1

#define LWIP_NETIF_HOSTNAME     1
#define ETHARP_TRUST_IP_MAC     0
#define IP_REASSEMBLY           1
#define IP_FRAG                 1
#define ARP_QUEUEING            0
#define LWIP_NETIF_API          1

#define LWIP_MDNS_RESPONDER     1
#define LWIP_IGMP               1

#define LWIP_NUM_NETIF_CLIENT_DATA      1

#define LWIP_ALTCP                      1

/* TLS over altcp (mbedtls) — opt-in per app via CONFIG_LWIP_ALTCP_TLS so apps
 * that don't need it (most) don't pull the altcp_tls_mbedtls port. The G-TAG
 * gateway enables it for MQTT-over-TLS to the broker. */
#ifdef CONFIG_LWIP_ALTCP_TLS
#define LWIP_ALTCP_TLS                  1
#define LWIP_ALTCP_TLS_MBEDTLS          1
#else
#define LWIP_ALTCP_TLS                  0
#endif

/* MQTT client buffer sizes (lwIP defaults 128/256 too small for rlink v2 topics).
 * MQTT_VAR_HEADER_BUFFER_LEN is selectable via Kconfig
 * (CONFIG_MQTT_VAR_HEADER_BUFFER_LEN); projects that don't set it fall back
 * to 1024 here so the build still succeeds. */
#ifdef CONFIG_MQTT_VAR_HEADER_BUFFER_LEN
#define MQTT_VAR_HEADER_BUFFER_LEN      CONFIG_MQTT_VAR_HEADER_BUFFER_LEN
#else
#define MQTT_VAR_HEADER_BUFFER_LEN      1024
#endif
/* App-tunable via CONFIG_LWIP_MQTT_OUTPUT_RINGBUF (e.g. the G-TAG gateway
 * mirrors 1568B binary waveforms); other projects keep the 1024 default. */
#ifdef CONFIG_LWIP_MQTT_OUTPUT_RINGBUF
#define MQTT_OUTPUT_RINGBUF_SIZE        CONFIG_LWIP_MQTT_OUTPUT_RINGBUF
#else
#define MQTT_OUTPUT_RINGBUF_SIZE        1024
#endif

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  0

#define LWIP_TIMEVAL_PRIVATE    0

#define LWIP_HAVE_LOOPIF           1
#define LWIP_NETIF_LOOPBACK        1
#define LWIP_LOOPBACK_MAX_PBUFS    8

/**
 * LWIP_TCPIP_CORE_LOCKING_INPUT: when LWIP_TCPIP_CORE_LOCKING is enabled,
 * this lets tcpip_input() grab the mutex for input packets as well,
 * instead of allocating a message and passing it to tcpip_thread.
 *
 * ATTENTION: this does not work when tcpip_input() is called from
 * interrupt context!
 */
/* Disable inline TCP processing in the caller's context.  With =1 the
 * NCM RX task processes TCP/UDP inside tcpip_input() while holding the
 * core mutex, which delays NTB buffer release and causes ~33 % NAK rate
 * (rx_stall) on sustained FS-USB traffic.  With =0 tcpip_input() posts
 * a message to the TCPIP thread and returns immediately, so the NCM RX
 * task frees the RX slot in microseconds instead of milliseconds. */
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0

/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE                (60*1024)

#define MEMP_NUM_NETBUF               32

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           100
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
//#define MEMP_NUM_UDP_PCB        6
#define MEMP_NUM_UDP_PCB        20

/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        20
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 20
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG        100

/* NUM of sys_timeout pool*/
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 8 + 3)

#define MEMP_NUM_NETCONN    (MEMP_NUM_TCP_PCB + MEMP_NUM_UDP_PCB + MEMP_NUM_TCP_PCB_LISTEN)

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#if !defined PBUF_POOL_SIZE
#if defined(CFG_ETHERNET_ENABLE)
/* 12 was far too small — USB NCM double-buffer holds up to 12
 * datagrams (6 per NTB × 2 slots), leaving zero free pbufs for
 * TCP retransmit queues and out-of-order segments.  48 gives
 * comfortable headroom for sustained full-speed USB traffic. */
#define PBUF_POOL_SIZE          48
#elif defined(CFG_USE_WIFI_BR)
#define PBUF_POOL_SIZE          16
#else
#define PBUF_POOL_SIZE          200
#endif
#endif

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#if defined(CFG_ETHERNET_ENABLE)
#define PBUF_POOL_BUFSIZE       1600
#else
#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE       760
#endif
#endif /* CFG_ETHERNET_ENABLE */


/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define IP_DEFAULT_TTL          64

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP timer interval (ms).  Default 250 ms causes up to 250 ms delayed-ACK
 * latency which cripples throughput on slow links (e.g. FS USB NCM).
 * 25 ms keeps delayed-ACK latency reasonable without excessive CPU load. */
#define TCP_TMR_INTERVAL        25

/* TCP Maximum segment size. */
#define TCP_MSS                 (1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */
//#define TCP_MSS                 (1500 - 80)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */
//#define TCP_MSS                 (800 - 40 - 80 + 8)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */


/* TCP sender buffer space (bytes). */
#ifdef CFG_ETHERNET_ENABLE
#define TCP_SND_BUF             (11*TCP_MSS)
#else
#define TCP_SND_BUF             (12*TCP_MSS)
#endif

/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */

#define TCP_SND_QUEUELEN        ((2 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))

/* With CORE_LOCKING_INPUT=0, every incoming packet posts a message to
 * the TCPIP thread.  At ~240 NTBs/sec the pool must absorb bursts
 * before the TCPIP thread (higher prio) drains them. */
#define MEMP_NUM_TCPIP_MSG_INPKT        (64)

/**
 * TCP_SNDQUEUELOWAT: TCP writable bufs (pbuf count). This must be less
 * than TCP_SND_QUEUELEN. If the number of pbufs queued on a pcb drops below
 * this number, select returns writable (combined with TCP_SNDLOWAT).
 */
#define TCP_SNDQUEUELOWAT               ((TCP_SND_QUEUELEN)/2)

/* TCP receive window. */
#ifdef CFG_ETHERNET_ENABLE
/* 6 segments was too small for FS USB NCM — the 2 ms ACK round-trip
 * (flush-timer + USB TX) limited throughput to ~4 MB/s.  20 segments
 * keeps the pipe full at 12 Mbps while staying within pbuf budget. */
#define TCP_WND                 (20*TCP_MSS)
#else
#define TCP_WND                 (30*TCP_MSS)
#endif

/* Memory alignment is defined in arch.h */

/**
 * TCP_WND_UPDATE_THRESHOLD: difference in window to trigger an
 * explicit window update
 */
#define TCP_WND_UPDATE_THRESHOLD   LWIP_MIN((TCP_WND / 2), (TCP_MSS * 16))

/**
 * By default, TCP socket/netconn close waits 20 seconds max to send the FIN
 */
#define LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT 5000

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#define LWIP_SO_SNDTIMEO                1
/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO                1


/* ---------- ICMP options ---------- */
#define LWIP_ICMP                       1


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#define LWIP_DHCP               1

#ifdef CONFIG_LWIP_DHCP_DOES_ARP_CHECK
#define DHCP_DOES_ARP_CHECK             1
#else
#define DHCP_DOES_ARP_CHECK             0
#endif

/* ---------- UDP options ---------- */
#define LWIP_UDP                1


/* ---------- Statistics options ---------- */
#define LWIP_STATS 1
#ifdef OPENTHREAD_BORDER_ROUTER
#define LWIP_ERRNO_STDINCLUDE 1
#else
#define LWIP_PROVIDE_ERRNO 1
#endif

/* ---------- link callback options ---------- */
/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK        1

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

#define LWIP_CHECKSUM_ON_COPY            1
#ifdef CFG_ETHERNET_ENABLE
#define LWIP_NETIF_TX_SINGLE_PBUF    0
#else
#define LWIP_NETIF_TX_SINGLE_PBUF    1
#endif /* CFG_ETHERNET_ENABLE */

#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
  /* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               1
#endif


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
#define LWIP_CHKSUM_ALGORITHM 3

/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    1

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     1

/*
   -----------------------------------
   ---------- DEBUG options ----------
   -----------------------------------
*/

//#define LWIP_DEBUG                      0

/*
   ---------------------------------
   ---------- OS options ----------
   ---------------------------------
*/

#define TCPIP_THREAD_NAME              "TCP/IP"
#ifdef CFG_ETHERNET_ENABLE
#define TCPIP_THREAD_STACKSIZE          1536
#elif defined(CFG_SDIOWIFI)
#define TCPIP_THREAD_STACKSIZE          512
#elif defined(CFG_NETBUS_WIFI_ENABLE)
#define TCPIP_THREAD_STACKSIZE          1536
#elif defined(CFG_TCPIP_STACKSIZE)
#define TCPIP_THREAD_STACKSIZE          CFG_TCPIP_STACKSIZE
#else
#define TCPIP_THREAD_STACKSIZE          4000
#endif /* CFG_ETHERNET_ENABLE */
#define TCPIP_MBOX_SIZE                 50
#define DEFAULT_UDP_RECVMBOX_SIZE       50
#define DEFAULT_TCP_RECVMBOX_SIZE       50
#define DEFAULT_RAW_RECVMBOX_SIZE       50
#define DEFAULT_ACCEPTMBOX_SIZE         50
#define DEFAULT_THREAD_STACKSIZE        500
#define TCPIP_THREAD_PRIO               (configMAX_PRIORITIES - 2)

#define LWIP_COMPAT_MUTEX               0
#define LWIP_TCPIP_CORE_LOCKING         1
#define LWIP_SOCKET_SET_ERRNO           1
#define SO_REUSE                        1
#define LWIP_TCP_KEEPALIVE              1

/*Enable Status callback and link callback*/
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_LINK_CALLBACK        1
/*Enable dns*/
#define LWIP_DNS                        1
#define LWIP_DNS_SECURE                 0
#define LWIP_DNS_SERVER                 0

#define MEMP_MEM_MALLOC                 0
#define LWIP_SUPPORT_CUSTOM_PBUF        1

#define PBUF_LINK_ENCAPSULATION_HLEN    48u

#define LWIP_RAW                        1

#define LWIP_DEBUG                      1

#ifdef MATTER_SUPPORT
#include "lwip/arch.h"
#include <lwip/mem.h>
#define LWIP_PBUF_CUSTOM_DATA           mem_size_t pool;
#endif

/*
   ---------------------------------
   ---------- SNTP options ----------
   ---------------------------------
*/
#include <time.h>
#define SNTP_SERVER_DNS             1
#define SNTP_MAX_SERVERS            3
#define SNTP_UPDATE_DELAY           3600000  /* 1 hour */
#define SNTP_SET_SYSTEM_TIME(sec)   do { \
    struct timeval tv = { .tv_sec = (sec), .tv_usec = 0 }; \
    settimeofday(&tv, NULL); \
} while(0)

/*
   ---------------------------------
   ---------- MISC. options ----------
   ---------------------------------
*/

#if defined(__cplusplus)
extern "C" int hal_rand(void);
extern "C" int * __errno(void);
#else
extern int * __errno(void);
#endif

#define errno (*__errno())

/**
 * LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS==1: randomize the local port for the first
 * local TCP/UDP pcb (default==0). This can prevent creating predictable port
 * numbers after booting a device.
 */

#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS 1
#define LWIP_RAND() ((u32_t)rand())

#ifdef LWIP_NETCONN_DUPLEX_SWITCH
#define LWIP_NETCONN_FULLDUPLEX 1
#define LWIP_NETCONN_SEM_PER_THREAD     1

void *sys_thread_sem_get(void);
void sys_thread_sem_init(void);
void sys_thread_sem_deinit(void);
#define LWIP_NETCONN_THREAD_SEM_GET() sys_thread_sem_get()
#define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_thread_sem_init()
#define LWIP_NETCONN_THREAD_SEM_FREE() sys_thread_sem_deinit()
#endif

/* ------------------------------------------------------------------ */
/* PPP over Serial (PPPoS) — enabled when LTE modem is configured     */
/* ------------------------------------------------------------------ */
#ifdef CONFIG_LTE_MODEM

#define PPP_SUPPORT             1
#define PPPOS_SUPPORT           1
#define PPP_INPROC_IRQ_SAFE     0   /* use pppos_input_tcpip() from task context */

/* sys_jiffies not in FreeRTOS port — alias to sys_now */
#define sys_jiffies             sys_now

/* Authentication */
#define PAP_SUPPORT             1
#define CHAP_SUPPORT            0
#define EAP_SUPPORT             0
#define MSCHAP_SUPPORT          0

/* Compression — disable for simplicity and RAM savings */
#define VJ_SUPPORT              0
#define CCP_SUPPORT             0
#define MPPE_SUPPORT            0

/* PPP options */
#define PPP_MAXIDLEFLAG         100
#define PPP_MRU                 1500
#define PPP_DEFMRU              1500

/* Use pppapi (threaded) */
#define LWIP_PPP_API            1

#endif /* CONFIG_LTE_MODEM */

#endif /* __LWIPOPTS_APP_H__ */
