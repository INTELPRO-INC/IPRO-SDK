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

#ifndef TRCVER_SDIO_H_7YSRGJ3T
#define TRCVER_SDIO_H_7YSRGJ3T

#include <stdbool.h>
#include <stdint.h>
#include <lwip/pbuf.h>
#include "sdiowifi_utils.h"

#define LP_FW_START_ADDR    0x23026800 // XXX duplicated
#define LP_FW_MAX_SIZE      (37 * 1024)

#ifndef BL_SDIO_RX_MAX_PORT_NUM
#define SDIO_RX_MAX_PORT_NUM   8
#else
#define SDIO_RX_MAX_PORT_NUM BL_SDIO_RX_MAX_PORT_NUM
#endif

#define SDIO_RX_BUF_SIZE     2048
#define SDIO_TX_BUF_SIZE     2048

/* TX aggregation (host->device download): the host packs up to ~4 frames
 * [2B body_len][body]... terminated by a 0x0000 len sentinel into ONE CMD53
 * write per download port. The IPRO6LE download-ring port buffer must hold
 * the largest aggregated write. Kept SEPARATE from SDIO_RX_BUF_SIZE (which
 * still feeds the LP_FW SW_TX_BUF math below + the CFG_IPRO_WIFI in-LP_FW ring)
 * so enlarging the IPRO6LE download ring does NOT shrink the upload
 * SW_TX_BUF region into negative size. MUST match the host IPRO6LE_AGG_TX_SIZE.
 * 6144 = up to ~4 full-MTU frames/port — the avgN sweet spot: aggregation is
 * CMD53-overhead-bound only up to avgN ~3.5 (then air/device-drain bound, proven
 * by the flush-timeout sweep: avgN 3.5->8 left UDP TX flat at ~11). So a 4-frame
 * buffer captures the gain (UDP TX ~5->11) with no benefit from going larger.
 * Ring lives in ram_nocache (the uncached 0x2-alias of OCRAM); 8 x 6144 = 48KB
 * fits the 64KB window with margin. (The window is expandable past 64KB into the
 * cached-0x6 ram_tcm's physical RAM if ever needed — DMA-verified — but avgN is
 * traffic-bound so a bigger ring does not help throughput.) */
#define SDIO_DNLD_AGG_BUF_SIZE  6144
#define SDIO_RX_BUF_START    SDM_ALIGN_HI(LP_FW_START_ADDR, 32)
#define SDIO_RX_BUF_SIZE_TOT (SDIO_RX_MAX_PORT_NUM * SDIO_RX_BUF_SIZE)
#define SDIO_RX_BUF_END      (SDIO_RX_BUF_START + SDIO_RX_BUF_SIZE_TOT)
#define SW_TX_BUF_SIZE       (LP_FW_MAX_SIZE - SDIO_RX_BUF_SIZE_TOT - 128)
#define SW_TX_BUF_START      SDM_ALIGN_HI(SDIO_RX_BUF_END, 32)

typedef void (*ipro_sdio_read_cb_t)(void *cb_arg, const void *data_ptr, uint16_t data_len);

// init
int ipro_sdu_init(void);

// read register
int ipro_sdio_read_cb_register(void *env, ipro_sdio_read_cb_t cb, void *cb_arg);

// ctrl
int32_t ipro_sdio_handshake(void);
void ipro_sdio_tx_timer_stop(void);

// write
/* data_len is len(V), excluding len(TL) */
int ipro_sdio_write_pbuf(void *env, uint16_t type, uint16_t subtype, struct pbuf *p, bool is_amsdu, void *cb, void *cb_arg);
int ipro_sdio_write_cmd(void *env, uint16_t type, uint16_t subtype, const void *headroom, uint16_t headroom_len, const void *tailroom, uint16_t tailroom_len);

#endif /* end of include guard: TRCVER_SDIO_H_7YSRGJ3T */
