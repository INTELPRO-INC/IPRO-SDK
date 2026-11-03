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

#pragma once
#include <stdint.h>

#ifndef PSA_SUCCESS
typedef int32_t psa_status_t;
#define PSA_SUCCESS ((psa_status_t)0)
#endif

#ifndef PSA_ERROR_PROGRAMMER_ERROR
#define PSA_ERROR_PROGRAMMER_ERROR ((psa_status_t)-129)
#endif
#ifndef PSA_ERROR_CONNECTION_REFUSED
#define PSA_ERROR_CONNECTION_REFUSED ((psa_status_t)-130)
#endif
#ifndef PSA_ERROR_CONNECTION_BUSY
#define PSA_ERROR_CONNECTION_BUSY ((psa_status_t)-131)
#endif
#ifndef PSA_ERROR_GENERIC_ERROR
#define PSA_ERROR_GENERIC_ERROR ((psa_status_t)-132)
#endif
#ifndef PSA_ERROR_NOT_PERMITTED
#define PSA_ERROR_NOT_PERMITTED ((psa_status_t)-133)
#endif
#ifndef PSA_ERROR_NOT_SUPPORTED
#define PSA_ERROR_NOT_SUPPORTED ((psa_status_t)-134)
#endif
#ifndef PSA_ERROR_INVALID_ARGUMENT
#define PSA_ERROR_INVALID_ARGUMENT ((psa_status_t)-135)
#endif
#ifndef PSA_ERROR_INVALID_HANDLE
#define PSA_ERROR_INVALID_HANDLE ((psa_status_t)-136)
#endif
#ifndef PSA_ERROR_BAD_STATE
#define PSA_ERROR_BAD_STATE ((psa_status_t)-137)
#endif
#ifndef PSA_ERROR_BUFFER_TOO_SMALL
#define PSA_ERROR_BUFFER_TOO_SMALL ((psa_status_t)-138)
#endif
#ifndef PSA_ERROR_ALREADY_EXISTS
#define PSA_ERROR_ALREADY_EXISTS ((psa_status_t)-139)
#endif
#ifndef PSA_ERROR_DOES_NOT_EXIST
#define PSA_ERROR_DOES_NOT_EXIST ((psa_status_t)-140)
#endif
#ifndef PSA_ERROR_INSUFFICIENT_MEMORY
#define PSA_ERROR_INSUFFICIENT_MEMORY ((psa_status_t)-141)
#endif
#ifndef PSA_ERROR_INSUFFICIENT_STORAGE
#define PSA_ERROR_INSUFFICIENT_STORAGE ((psa_status_t)-142)
#endif
#ifndef PSA_ERROR_INSUFFICIENT_DATA
#define PSA_ERROR_INSUFFICIENT_DATA ((psa_status_t)-143)
#endif
#ifndef PSA_ERROR_SERVICE_FAILURE
#define PSA_ERROR_SERVICE_FAILURE ((psa_status_t)-144)
#endif
#ifndef PSA_ERROR_COMMUNICATION_FAILURE
#define PSA_ERROR_COMMUNICATION_FAILURE ((psa_status_t)-145)
#endif
#ifndef PSA_ERROR_STORAGE_FAILURE
#define PSA_ERROR_STORAGE_FAILURE ((psa_status_t)-146)
#endif
#ifndef PSA_ERROR_HARDWARE_FAILURE
#define PSA_ERROR_HARDWARE_FAILURE ((psa_status_t)-147)
#endif
#ifndef PSA_ERROR_INVALID_SIGNATURE
#define PSA_ERROR_INVALID_SIGNATURE ((psa_status_t)-149)
#endif
#ifndef PSA_ERROR_DATA_CORRUPT
#define PSA_ERROR_DATA_CORRUPT ((psa_status_t)-152)
#endif
#ifndef PSA_ERROR_DATA_INVALID
#define PSA_ERROR_DATA_INVALID ((psa_status_t)-153)
#endif
