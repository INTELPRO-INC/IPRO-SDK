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

/**
 * @file random.c
 * @brief OpenThread random/entropy platform implementation for IPRO7
 * 
 * Implements OpenThread's random number API using IPRO7's hardware TRNG
 * (True Random Number Generator) for cryptographic-quality randomness.
 */

#include <stdint.h>
#include <openthread/platform/entropy.h>
#include <openthread/error.h>
#include "hal_sec_trng.h"
#include "ipro_log.h"

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    int ret;
    
    if (aOutput == NULL || aOutputLength == 0)
    {
        return OT_ERROR_INVALID_ARGS;
    }
    
    // Use IPRO7's hardware TRNG to get cryptographic random bytes
    // hal_rand_stream is the correct API for reading random data
    ret = hal_rand_stream(aOutput, aOutputLength);
    
    if (ret != 0)
    {
        IPRO_LOGE("OT-Random", "TRNG read failed: %d", ret);
        return OT_ERROR_FAILED;
    }
    
    return OT_ERROR_NONE;
}

uint32_t otPlatRandomGet(void)
{
    uint32_t value = 0;
    
    // Get 4 bytes of randomness
    otError error = otPlatEntropyGet((uint8_t *)&value, sizeof(value));
    
    if (error != OT_ERROR_NONE)
    {
        // Fallback: use a simple counter if TRNG fails
        // This should never happen in normal operation
        static uint32_t sFallbackCounter = 0;
        value = ++sFallbackCounter;
        IPRO_LOGW("OT-Random", "Using fallback counter: %u", value);
    }
    
    return value;
}

uint16_t otPlatRandomGetUint16InRange(uint16_t aMin, uint16_t aMax)
{
    uint32_t range;
    uint32_t value;
    
    if (aMin >= aMax)
    {
        return aMin;
    }
    
    range = aMax - aMin;
    value = otPlatRandomGet();
    
    // Simple modulo for range - good enough for OpenThread's needs
    // (OpenThread doesn't require perfect uniform distribution)
    return (uint16_t)(aMin + (value % range));
}

uint32_t otPlatRandomGetUint32InRange(uint32_t aMin, uint32_t aMax)
{
    uint64_t range;
    uint64_t value;
    
    if (aMin >= aMax)
    {
        return aMin;
    }
    
    range = (uint64_t)aMax - aMin;
    value = otPlatRandomGet();
    
    // Simple modulo for range
    return (uint32_t)(aMin + (value % range));
}
