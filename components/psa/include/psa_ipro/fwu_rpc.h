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
 * PSA_HANDLE_FWU RPC of svc_fwu.c and the NSPE client: psa_call type
 * = fwu_fn; req and status vectors are exactly their struct size.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa_ipro/abi.h"

enum fwu_fn {
    FWU_FN_WRITE = 1,       /* in [req, data], out [] */
    FWU_FN_INSTALL,         /* in [req], out [] */
    FWU_FN_QUERY,           /* in [], out [status] */
};

/* Data bytes of one WRITE */
#define FWU_WRITE_MAX 2048u

/*
 * WRITE: offset of the data in FW-B, len = data length. INSTALL:
 * offset 0, len = package bytes.
 */
struct fwu_req {
    uint32_t offset;
    uint32_t len;
};

enum fwu_state {
    FWU_STATE_IDLE,         /* nothing written since boot */
    FWU_STATE_WRITING,      /* staged_len bytes written from offset 0 */
    FWU_STATE_PENDING,      /* the table boot2 reads has FW-B pending */
};

#define FWU_SVN_NONE 0xFFFFFFFFu
#define FWU_OTA_NONE 0xFFFFFFFFu

struct fwu_status {
    uint32_t state;         /* enum fwu_state */
    uint32_t fw_active;     /* FW activeindex in the table boot2 reads */
    uint32_t pt_active;     /* that table: 0 PT0, 1 PT1 */
    uint32_t pt_age;        /* its age */
    uint32_t staged_len;    /* bytes written from offset 0 */
    uint32_t slot_size;     /* FW-B bytes */
    uint32_t staged_svn;    /* SVN of the header in FW-B, or NONE */
    uint32_t svn_floor;     /* eFuse anti-rollback counter */
    uint32_t running_svn;   /* SVN boot2 installed this boot, or NONE */
    uint32_t last_ota;      /* boot2 OTA result code at init, or NONE */
};

_Static_assert(sizeof(struct fwu_req) == 8, "fwu_req size");
_Static_assert(sizeof(struct fwu_status) == 40, "fwu_status size");
