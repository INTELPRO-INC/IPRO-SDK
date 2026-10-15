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

/* ipronet_ota_proto.h - OTA protocol, no RTOS. Not an API. */
#ifndef IPRONET_OTA_PROTO_H
#define IPRONET_OTA_PROTO_H

#include <stdint.h>
#include "ipronet_host.h"
#include "ipronet_ota.h"
#include "ipronet_ctrl.h"

#define IPRONET_OTA_HDR_SIZE        512U
#define IPRONET_OTA_TRIES           3
#define IPRONET_OTA_START_MS        2000U
#define IPRONET_OTA_PROGRAM_MS      2000U
#define IPRONET_OTA_COMMIT_MS       5000U
#define IPRONET_OTA_ABORT_MS        500U
#define IPRONET_OTA_COMMIT_ASSUMED  1     /* no ack, device silent */

/* One OTA body out, the reply body back; *status = ack hdr.status.
 * Returns 0, IPRONET_ERR_OTA_TIMEOUT or another IPRONET_ERR_*. */
typedef int (*ipronet_ota_req_fn)(void *ctx, const uint8_t *body,
                                  uint16_t len, uint8_t *rsp,
                                  uint16_t *rsp_len, uint16_t *status,
                                  uint32_t timeout_ms);
/* 1 when the device still answers control requests. */
typedef int (*ipronet_ota_probe_fn)(void *ctx);

int ipronet_ota_check_header(const uint8_t *hdr, uint32_t size);
/* START, then the package; hdr = its first 512 B, already read. */
int ipronet_ota_xfer(const ipronet_ota_src_t *src, const uint8_t *hdr,
                     ipronet_ota_req_fn req, void *ctx,
                     const ipronet_ota_opts_t *opts,
                     ipronet_ota_report_t *rep);
/* IPRONET_OK, IPRONET_OTA_COMMIT_ASSUMED or IPRONET_ERR_*. */
int ipronet_ota_commit(ipronet_ota_req_fn req, ipronet_ota_probe_fn probe,
                       void *ctx, ipronet_ota_report_t *rep);
void ipronet_ota_abort(ipronet_ota_req_fn req, void *ctx);
/* [7:0] device FW entry activeIndex (0 = installed, 1 = still pending),
 * [15:8] boot2's reason for not installing (IPRONET_OTA_E_BOOT2 + n). */
int ipronet_ota_query_pending(ipronet_ota_req_fn req, void *ctx,
                              uint32_t *pending, ipronet_ota_report_t *rep);

#endif /* IPRONET_OTA_PROTO_H */
