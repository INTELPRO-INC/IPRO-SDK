/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

/* IPRO7 U-mode NSPE target, after tgt_dev_apis_tfm_an521/nspe/
 * pal_config.h; the suite and build macros come from CMakeLists.txt.
 */

#ifndef _PAL_CONFIG_H_
#define _PAL_CONFIG_H_

#include "pal_crypto_config.h"
#include "pal_attestation_config.h"
#include "pal_storage_config.h"

/* UART0, the NSPE console; output goes through putchar() */
#define UART_NUM                               1

#define UART_0_BASE                            0x3000B000
#define UART_0_SIZE                            0xFFF
#define UART_0_INTR_ID                         0xFF
#define UART_0_PERMISSION                      TYPE_READ_WRITE

/* No watchdog: WATCHDOG_AVAILABLE stays undefined */
#define WATCHDOG_NUM                           0

/* 1 KB NVMEM: a RAM array of pal_driver_intf.c, offsets from 0 */
#define NVMEM_NUM                              1

/* Attestation PAL: IAK public key import with psa_key_attributes_t */
#define CRYPTO_VERSION_BETA3

/* UART base address assigned */
#define PLATFORM_UART_BASE UART_0_BASE

/* Watchdog timeouts read by val_peripherals.c: all 0 */
#define PLATFORM_WD_BASE                        0x0
#define PLATFORM_WD_NUM_OF_TICK_PER_MICRO_SEC   0x0
#define PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_LOW    0x0
#define PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_MEDIUM 0x0
#define PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_HIGH   0x0
#define PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_CRYPTO 0x0

/* Non-volatile memory base address assigned */
#define PLATFORM_NVM_BASE 0

/* Fills the 1 KB RAM NVMEM with 0xFF: the next val_entry() starts
 * from the first test with zeroed counters */
void pal_nvmem_reset(void);

/* PSA API header of the suite being built */
#ifdef CRYPTO
#include "psa/crypto.h"

/* PSA Crypto API 1.1 identifiers the 1.0.0 test list names that the
 * mbedtls 2.28 headers lack; values as in the 1.1 specification */
#ifndef PSA_KEY_DERIVATION_INPUT_PASSWORD
#define PSA_KEY_DERIVATION_INPUT_PASSWORD ((psa_key_derivation_step_t)0x0102)
#endif
#ifndef PSA_KEY_DERIVATION_INPUT_OTHER_SECRET
#define PSA_KEY_DERIVATION_INPUT_OTHER_SECRET \
    ((psa_key_derivation_step_t)0x0103)
#endif
#ifndef PSA_KEY_DERIVATION_INPUT_COST
#define PSA_KEY_DERIVATION_INPUT_COST ((psa_key_derivation_step_t)0x0205)
#endif
#ifndef PSA_ALG_IS_PBKDF2_HMAC
#define PSA_ALG_IS_PBKDF2_HMAC(alg) \
    (((alg) & ~PSA_ALG_HASH_MASK) == ((psa_algorithm_t)0x08800100))
#endif
#ifndef PSA_ALG_GET_HASH
#define PSA_ALG_GET_HASH(alg) \
    (((alg) & PSA_ALG_HASH_MASK) == 0 ? ((psa_algorithm_t)0) : \
     (PSA_ALG_CATEGORY_HASH | ((alg) & PSA_ALG_HASH_MASK)))
#endif
#endif

#ifdef INTERNAL_TRUSTED_STORAGE
#include "psa/internal_trusted_storage.h"
#endif

#ifdef INITIAL_ATTESTATION
#include "psa/initial_attestation.h"
#endif

#endif /* _PAL_CONFIG_H_ */
