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
 * SPE PSA Crypto service (PSA_HANDLE_CRYPTO) over the mbedtls PSA
 * core. Keys the SPE creates itself must be volatile: the client
 * owns its volatile keys and every persistent id of the user range.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"

/* Heap hook (spm_heap) and psa_crypto_init(); its status */
psa_status_t svc_crypto_init(void);

/* Reset hook: frees every op slot, destroys client volatile keys */
void svc_crypto_client_reset(void);

/* spm_service_fn of PSA_HANDLE_CRYPTO, ids of psa_ipro/crypto_rpc.h */
psa_status_t svc_crypto_call(int32_t type, const psa_invec *in,
                             size_t in_len, psa_outvec *out,
                             size_t out_len);
