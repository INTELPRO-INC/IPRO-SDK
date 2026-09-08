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
 * Per-app lwIP config for ipro7_sdio_wifi (SDIO-bridge host). Picked up by
 * components/network/lwip/.../config/lwipopts.h via __has_include(), replacing
 * the SDK default. This is the SDK default body + a CONFIG_USE_PSRAM block at
 * the end that enlarges the lwIP buffers (they live in PSRAM via .lwip_mem) to
 * lift sustained TCP RX (5.7 -> ~9 Mbps, no stalls). See
 * project_ipronet_phase_d_blocker.
 */
#ifndef __LWIPOPTS_APP_H__
#define __LWIPOPTS_APP_H__

#include "stdbool.h"
#include "sys/time.h"

/* Low-latency TCP loss recovery for the SDIO WiFi bridge. The link has a small
 * RTT (~9 ms) but ~2% baseline packet loss (WiFi). Stock lwIP recovers each loss
 * via a coarse RTO (clamped "min wake time >= 1s" in tcp_slowtmr, and it halves
 * cwnd on every rexmit) — with a small cwnd, <3 dup-acks are produced so fast-
 * retransmit never engages, so TCP TX collapses to ~0.3 Mbps while UDP TX is
 * 16 Mbps on the SAME egress (proving the bottleneck is the TCP feedback loop,
 * not the device). This fork's _LP_TCP_OPTIMIZE: rexmit RTO = 1 slow-tick (no
 * 1s clamp) AND does NOT cut cwnd/ssthresh on rexmit, so TCP rides through the
 * random loss. Needs TCP_MAXRTX/TCP_SYNMAXRTX <= 48 (defaults 12/6, ok). */
#define _LP_TCP_OPTIMIZE        1

/* The fork also armed fast retransmit on the FIRST dup-ACK, on the premise that
 * this path never reorders so a dup-ACK can only mean loss. Measurement says
 * otherwise: the 1-tick RTO fires below the link's RTT jitter (6.7 ms min,
 * 217 ms max) and resends the whole unacked window, so most dup-ACKs report a
 * DUPLICATE rather than a gap. Each one then halved cwnd, and the two fed each
 * other — 1610 retransmissions against 2 genuinely lost segments, cwnd pinned at
 * its 2 MSS floor (median 1 MSS in flight against a 64 KB window), 50% of the
 * run stalled. Back to the RFC 5681 value. LAB02, 4 alternating pairs, no
 * overlap: 2.23 -> 5.34 Mbps; UDP on the same egress does 11.4, so the amplifier
 * was the gap, not the device. Raising the RTO floor instead (tcprto 6) is a
 * NULL on its own — it slows the source but leaves the amplifier armed. Both
 * are runtime-tunable for A/B via the `tcprto` console command. */
#define LP_TCP_DUPACK_THRESH    3

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
#define LWIP_ALTCP_TLS                  0

/* MQTT client buffer sizes (lwIP defaults 128/256 too small for rlink v2 topics).
 * MQTT_VAR_HEADER_BUFFER_LEN is selectable via Kconfig
 * (CONFIG_MQTT_VAR_HEADER_BUFFER_LEN); projects that don't set it fall back
 * to 1024 here so the build still succeeds. */
#ifdef CONFIG_MQTT_VAR_HEADER_BUFFER_LEN
#define MQTT_VAR_HEADER_BUFFER_LEN      CONFIG_MQTT_VAR_HEADER_BUFFER_LEN
#else
#define MQTT_VAR_HEADER_BUFFER_LEN      1024
#endif
#define MQTT_OUTPUT_RINGBUF_SIZE        1024

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
/* SDIO bridge: keep protocol processing off the high-priority rx_task. ZCEN3
 * delivers several frames per CMD53; processing them inline delayed the next
 * doorbell drain long enough to empty the device's 8-buffer upload pool. The
 * rx_task now posts pbufs here and yields after a bounded drain budget, so SDIO
 * and lwIP run as a pipeline. A 30-Mbps UDP RX run exhausted the 128-entry
 * input-message pool before the mailbox itself filled, so keep both depths at
 * 512. These pools live in PSRAM in this build; the extra depth moves the burst
 * out of the device's eight-buffer upload queue without consuming OCRAM.
 *
 * [inherited USB-NCM note, does NOT apply to SDIO] On USB CDC-NCM, =1 made the
 * NCM RX task hold the core mutex through TCP/UDP processing, delaying NTB
 * buffer release and causing ~33% NAK (rx_stall) on FS-USB. The SDIO path has
 * no such buffer-release coupling. */
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
#define MEMP_NUM_TCPIP_MSG_INPKT        (512)

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

/* Cap destination-unreachable replies at 100/s (0 would be stock lwIP).
 *
 * Without a cap, every datagram that arrives for a closed port costs one frame
 * transmitted from the tcpip thread. Measured on this bench 2026-09-03: a
 * 15 s window of 7755 unmatched datagrams (only ~4.7 Mbps inbound) produced
 * 7755 ICMP transmits and drove PBUF_POOL's high-water to 243 of 256, while a
 * cell delivering 33826 datagrams to an OPEN socket -- 4x the traffic --
 * peaked at 4. The RX pool is not short of buffers; it was being emptied by
 * the host's own reply storm. 100/s still answers a legitimate prober
 * immediately. Raw data: scratch/ipronet_host_baseline/orphan_ab_1.json. */
#define ICMP_DEST_UNREACH_RATE_LIMIT    100


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
#define TCPIP_MBOX_SIZE                 512
#define DEFAULT_UDP_RECVMBOX_SIZE       50
#define DEFAULT_TCP_RECVMBOX_SIZE       50
#define DEFAULT_RAW_RECVMBOX_SIZE       50
#define DEFAULT_ACCEPTMBOX_SIZE         50
#define DEFAULT_THREAD_STACKSIZE        500
/* Bug B fix H (PM13 2026-05-29): drop the tcpip thread one priority below
 * rx_task (configMAX-2) so RX-upload draining outranks TX-download. Root cause:
 * rx_task and tcpip were both configMAX-2, so during a TCP-TX download burst the
 * tcpip thread monopolised the SDIO bus (sdio_claim_host) and rx_task could not
 * read the device's upload(ACK) ports in time -> device sdu_send_data busy-wait
 * timeout -> dropped ACK (9% measured) -> host TCP RTO/cwnd-cut -> TX collapse.
 * Reading ACKs first is what lets TCP TX keep flowing, so RX-over-TX helps TX. */
#define TCPIP_THREAD_PRIO               (configMAX_PRIORITIES - 3)

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


/* ---- SDIO-bridge throughput: larger lwIP buffers, placed in PSRAM via the
 *      .lwip_mem section (only when CONFIG_USE_PSRAM; OCRAM builds unchanged) ---- */
#if defined(CONFIG_USE_PSRAM) && (CONFIG_USE_PSRAM == 1)
#undef  MEM_SIZE
#define MEM_SIZE            (1024 * 1024)
#undef  PBUF_POOL_SIZE
#define PBUF_POOL_SIZE      256
#undef  MEMP_NUM_PBUF
#define MEMP_NUM_PBUF       512
#undef  MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG    512
/* Offer RFC 7323 window scaling so the Linux peer is not forced to advertise
 * a 64-KB window. LAB02 receiver capture showed that cap in every unscaled
 * connection; at 12 Mbps it was reached during normal RTT excursions. Doubling
 * the PSRAM-backed send/receive windows lifted paired TCP TX without changing
 * the socket API, and peers that omit the option retain the legacy behavior. */
#undef  LWIP_WND_SCALE
#define LWIP_WND_SCALE      1
#undef  TCP_RCV_SCALE
#define TCP_RCV_SCALE       1
#undef  TCP_WND
#define TCP_WND             (88 * TCP_MSS)
#undef  TCP_SND_BUF
#define TCP_SND_BUF         (88 * TCP_MSS)
#undef  TCP_SNDLOWAT
#define TCP_SNDLOWAT        (22 * TCP_MSS)
#undef  TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN    ((2 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS))
#endif

#endif /* __LWIPOPTS_APP_H__ */
