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
 * ipronet_host_diag.h - bench and A/B access into ipronet_host.
 *
 * NOT part of the customer API: no compatibility promise, may change without
 * notice. The example app apps/platform/ipro7_sdio_wifi builds its ipro6s_*
 * shell commands on this.
 */
#ifndef IPRONET_HOST_DIAG_H
#define IPRONET_HOST_DIAG_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_sdio.h"
#include "ipronet_host.h"      /* ipronet_callbacks_t */

/* SMID wire facts the raw probes need (see ipronet_sdio.c for the story). */
#define IPRONET_FUNC_DATA         SDIO_FUNC_1
#define IPRONET_FUNC_BLOCK_SIZE   512U
#define IPRONET_DOORBELL_LO       0x00U
#define IPRONET_DOORBELL_HI       0x01U
#define IPRONET_FIFO_ADDR         0x00000000U
#define IPRONET_RX_BUF_SIZE       (4U * 2048U)   /* ZC3: up to four 2048 B strides */

#pragma pack(push, 1)
struct ipronet_top_msg {               /* 12-byte SMID upload header (Linux sdio_msgs.h) */
    uint8_t type_lsb, type_msb;
    uint8_t len_lsb, len_msb;
    uint8_t pld_off;
    uint8_t is_amsdu;
    uint8_t has_twin;
    uint8_t subtype_lsb, subtype_msb;
    uint8_t first_part_len_lsb, first_part_len_msb;
    uint8_t _pad0[1];
    uint8_t pld_w_pad[];
};
#pragma pack(pop)

typedef struct {
    uint32_t writes;          /* CMD53 writes (single + agg) */
    uint32_t agg_frames;      /* frames delivered across writes */
    uint32_t cmd_frames;      /* type 0xEF01 */
    uint32_t eth_frames;      /* type 0xABCD */
    uint64_t bytes;           /* on-wire, padded blocks */
    uint32_t errors;
    uint32_t agg_flushes;
    uint32_t agg_to_flush;    /* timeout-triggered flushes */
    uint64_t write_us;        /* summed CMD53 write latency */
    uint32_t write_max_us;
    uint32_t agg_max;         /* current cap; 0 = bypass */
    uint32_t zc2_nslot;       /* 0 = stride packing off */
    uint32_t zc3_uslots;      /* 0 = legacy upload wire */
    uint32_t pace_kbps;
    uint32_t pace_drops;
} ipronet_tx_stats_t;

typedef struct {
    uint32_t doorbell_polls;
    uint32_t doorbell_zero;
    uint32_t transfers;       /* CMD53 upload reads */
    uint64_t bytes;
    uint32_t pdus;            /* valid headers */
    uint32_t data_frames;
    uint32_t amsdu_drop;
    uint32_t bad_type;
    uint32_t bad_subtype;
    uint32_t desync;
    uint32_t unaligned_cnt;
    uint32_t pool_drop;       /* PBUF_POOL exhausted (netif) */
    uint32_t mbox_drop;       /* tcpip_input mbox full (netif) */
    uint16_t last_pdu_len;
    uint16_t last_pdu_subtype;
    uint32_t girq_count;      /* GPIO0 data-ready ISR hits */
} ipronet_rx_stats_t;

typedef struct {
    uint8_t  enabled;
    uint8_t  armed;           /* credentials stored and not user_down */
    char     ssid[33];
    uint32_t attempts;
    uint32_t restores;
} ipronet_reconn_stats_t;

/* ---- handles / state ---------------------------------------------------- */
SDIO_Host_Type *ipronet_diag_host(void);          /* NULL before sdio_init */
bool ipronet_diag_rx_task_running(void);

/* The transport's RX transfer buffer (IPRONET_RX_BUF_SIZE). Lent to bench code
 * that drives the doorbell itself - only valid while rx_task is stopped, which
 * ipronet_diag_rx_task_running() reports. Borrowing it keeps the flood test
 * from costing a second PSRAM buffer of its own. */
uint8_t *ipronet_diag_rx_buf(void);
void ipronet_diag_rx_profile(int reset);          /* rx_task stage census */
int  ipronet_diag_rx_xfer_lowprio(int on);        /* CMD53 wait at IDLE+1: 1/0, -1 = query */
void ipronet_diag_tx_stats(ipronet_tx_stats_t *out);
void ipronet_diag_rx_stats(ipronet_rx_stats_t *out);
void ipronet_diag_reconn_stats(ipronet_reconn_stats_t *out);
int  ipronet_diag_ap_mac(uint8_t mac[6]);         /* AP MAC from GET_MAC */

/* ---- the individual bring-up steps (ipro6s_init/enum/setup/... commands) */
/* ipronet_start() registers the app's callbacks itself; a step-by-step
 * bring-up has to, or the device's events (CONNECTED / IP / scan) go unheard. */
void ipronet_diag_set_callbacks(const ipronet_callbacks_t *cb);   /* NULL = none */
int  ipronet_diag_sdio_init(void);                /* pinmux + sdio_host_init; idempotent */
int  ipronet_diag_sdio_enum(void);                /* prints the CIS/FBR report */
int  ipronet_diag_sdio_setup(uint32_t bus_hz);    /* 25000000 or 50000000 */
int  ipronet_diag_rx_start(void);
int  ipronet_diag_send_hello(void);
int  ipronet_diag_get_mac(void);                  /* blocks <= 1 s */
int  ipronet_diag_zcen(int gen);                  /* 1, 2 or 3 */
int  ipronet_diag_netif_up(void);
const char *ipronet_diag_netif_ip_str(void);      /* what iproeth0 holds; static buffer */
int  ipronet_diag_link_status(void);              /* sends STA_GET_LINK_STATUS */

/* ---- A/B knobs ---------------------------------------------------------- */
int  ipronet_diag_set_aggmax(uint32_t n);         /* 0 = bypass aggregation */
int  ipronet_diag_girq(int on);                   /* GPIO0 data-ready IRQ wake on/off */
int  ipronet_diag_set_pace(uint32_t kbps);
int  ipronet_diag_set_reconnect(int en);           /* the HOST's reconnect worker */
int  ipronet_diag_device_autoconnect(int en);      /* the DEVICE's own autoconnect (RNM) */
void ipronet_diag_last_ack(uint16_t *cmd, uint16_t *status);   /* last device ACK seen */

/* ---- raw data path for tx_ramp / flood-style tests ---------------------- */
int  ipronet_diag_eth_tx(const uint8_t *frame, uint16_t len);   /* the netif linkoutput path */
void ipronet_diag_tx_agg_flush(void);                           /* push a partial batch */

/* ---- device rail before start() (ipro6_rst / ipro6_dl / ipro6_pwr) ------ */
void ipronet_sdio_power_configure(int8_t gpio, uint8_t active_low);   /* rail pin for device_power() */

#endif /* IPRONET_HOST_DIAG_H */
