/*
 * Copyright (C) 2018-2024 AICSemi Ltd.
 *
 * All Rights Reserved
 *
 * Unified net_buf_tx_t definition.
 *
 * This is the SINGLE canonical definition of net_buf_tx_t.
 *
 * It lives in a standalone, minimal-dependency header so that:
 *   - netif_port.h   (port/inc/)  can reference net_buf_tx_t / net_if_t
 *   - netif_def.h    (drv_api/inc/) can alias it to TCPIP_PACKET_INFO_T
 *
 * None of these three files need circular-include guards anymore —
 * they simply #include "aic_netbuf.h" unconditionally.
 *
 * The struct layout is pointer-compatible with TCPIP_PACKET_INFO_T
 * (defined in netif_def.h for internal use).
 */

#ifndef _AIC_NETBUF_H_
#define _AIC_NETBUF_H_

#include "aic_porting_types.h"

typedef void *net_if_t;  /* opaque network interface handle */

typedef struct net_buf_tx {
    uint8_t *data_ptr;
    uint32_t data_len;
    uint32_t pkt_type;
    uint32_t net_id;
    void    *frag_next;  /* [PLATFORM] nocopy TX (NET_IF_UAP_NCPYBUF=1): pbuf 链指针，
                            透传到 SDIO 聚合 scatter copy；copy 模式恒为 NULL */
} net_buf_tx_t;

#endif /* _AIC_NETBUF_H_ */
