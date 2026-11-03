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
 * SPE firmware-update staging service (PSA_HANDLE_FWU): the NSPE
 * writes an OTA package into FW-B, the SPE checks its header format
 * and writes the pending partition table that boot2 installs from.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"

/* OTA package header of ipro_boot2 (main.c, boot2_secure.h) */
#define FWU_HDR_SIZE      512u
#define FWU_HDR_MAGIC     "IPRO_OTA_Ver1.0"
#define FWU_HDR_TYPE_OFF  16u
#define FWU_HDR_TYPE      "XZ  "
#define FWU_HDR_LEN_OFF   20u
#define FWU_HDR_SVN_OFF   24u
#define FWU_HDR_SVN_LEGACY "\x01\x02\x03\x04\x05\x06\x07\x08"
#define FWU_HDR_CHECK     32u
/* boot2 OTA result word (HBN RAM sysinfo word 6), magic "OR" */
#define FWU_OTA_RES_MAGIC 0x4F520000u
#define FWU_OTA_INSTALLED 0u
#define FWU_SECTOR        4096u

/*
 * Takes boot2's OTA result of this boot (the word is cleared) and,
 * after an install, the SVN of the package in FW-B.
 */
void svc_fwu_init(void);

/* Reset hook: the next package starts at offset 0 */
void svc_fwu_client_reset(void);

/* spm_service_fn of PSA_HANDLE_FWU, psa_ipro/fwu_rpc.h */
psa_status_t svc_fwu_call(int32_t type, const psa_invec *in,
                          size_t in_len, psa_outvec *out, size_t out_len);
