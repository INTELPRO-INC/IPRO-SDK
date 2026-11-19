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

/* Compile with the real SDK/OT adapter flags, not a standalone mock profile. */
#include <FreeRTOS.h>
#include <task.h>
#include <stddef.h>
#include <openthread-core-config.h>
#include <openthread/error.h>
#include <openthread/instance.h>
#include <mbedtls/build_info.h>
#include <ieee802154.h>
#include <ipro/MatterPackageAbi.h>
#include <ipro/MatterPackageIdentity.h>

_Static_assert(CONFIG_MATTER_VENDOR_ID == IPRO_MATTER_IDENTITY_VENDOR_ID, "Matter identity mismatch: VENDOR_ID");
_Static_assert(CONFIG_MATTER_PRODUCT_ID == IPRO_MATTER_IDENTITY_PRODUCT_ID, "Matter identity mismatch: PRODUCT_ID");
_Static_assert(CONFIG_MATTER_DEVICE_TYPE == IPRO_MATTER_IDENTITY_DEVICE_TYPE, "Matter identity mismatch: DEVICE_TYPE");

/* Compare against compiler-emitted archive values, not manifest assertions. */
#define IPRO_ABI_CHECK(expression, field) \
    _Static_assert((expression) == IPRO_MATTER_ABI_##field, "Matter archive ABI mismatch: " #field)
IPRO_ABI_CHECK(sizeof(void *), POINTER_BYTES);
IPRO_ABI_CHECK(_Alignof(void *), POINTER_ALIGN);
IPRO_ABI_CHECK(sizeof(otError), ENUM_BYTES);
IPRO_ABI_CHECK(sizeof(BaseType_t), BASE_BYTES);
IPRO_ABI_CHECK(sizeof(UBaseType_t), UBASE_BYTES);
IPRO_ABI_CHECK(sizeof(TickType_t), TICK_BYTES);
IPRO_ABI_CHECK(_Alignof(TickType_t), TICK_ALIGN);
IPRO_ABI_CHECK(sizeof(StackType_t), STACK_BYTES);
IPRO_ABI_CHECK(sizeof(StaticTask_t), STATIC_TASK_BYTES);
IPRO_ABI_CHECK(_Alignof(StaticTask_t), STATIC_TASK_ALIGN);
IPRO_ABI_CHECK(sizeof(StaticQueue_t), STATIC_QUEUE_BYTES);
IPRO_ABI_CHECK(_Alignof(StaticQueue_t), STATIC_QUEUE_ALIGN);
IPRO_ABI_CHECK(sizeof(StaticEventGroup_t), STATIC_EVENT_BYTES);
IPRO_ABI_CHECK(_Alignof(StaticEventGroup_t), STATIC_EVENT_ALIGN);
IPRO_ABI_CHECK(sizeof(StaticStreamBuffer_t), STATIC_STREAM_BYTES);
IPRO_ABI_CHECK(_Alignof(StaticStreamBuffer_t), STATIC_STREAM_ALIGN);
IPRO_ABI_CHECK(configTICK_RATE_HZ, TICK_HZ);
IPRO_ABI_CHECK(configMAX_PRIORITIES, MAX_PRIORITIES);
IPRO_ABI_CHECK(portBYTE_ALIGNMENT, PORT_ALIGNMENT);
IPRO_ABI_CHECK(OPENTHREAD_API_VERSION, OT_API);
IPRO_ABI_CHECK(OPENTHREAD_CONFIG_THREAD_VERSION, THREAD_VERSION);
IPRO_ABI_CHECK(configSUPPORT_STATIC_ALLOCATION, STATIC_ALLOCATION);
IPRO_ABI_CHECK(configSUPPORT_DYNAMIC_ALLOCATION, DYNAMIC_ALLOCATION);
IPRO_ABI_CHECK(sizeof(TimeOut_t), TIMEOUT_BYTES);
IPRO_ABI_CHECK(_Alignof(TimeOut_t), TIMEOUT_ALIGN);
IPRO_ABI_CHECK(offsetof(TimeOut_t, xOverflowCount), TIMEOUT_OVERFLOW_OFFSET);
IPRO_ABI_CHECK(offsetof(TimeOut_t, xTimeOnEntering), TIMEOUT_TICK_OFFSET);
IPRO_ABI_CHECK(configTICK_TYPE_WIDTH_IN_BITS, TICK_CONFIG_WIDTH);
#undef IPRO_ABI_CHECK

_Static_assert(sizeof(void *) == 4, "Matter package requires RV32 pointers");
_Static_assert(sizeof(TickType_t) == 8, "Matter package requires 64-bit FreeRTOS ticks");
_Static_assert(configTICK_RATE_HZ == 1000, "Matter package requires 1000 Hz ticks");
_Static_assert(configMAX_PRIORITIES == 32, "Matter package RTOS priorities differ");
_Static_assert(sizeof(otError) == 4, "OpenThread package enum ABI mismatch");
_Static_assert(sizeof(ieee802154_state_t) == 1, "Radio driver state ABI must be fixed width");
_Static_assert(sizeof(ieee802154_cca_mode_t) == 1, "Radio driver configuration ABI must be fixed width");
_Static_assert(OPENTHREAD_API_VERSION == 548, "Wrong OpenThread headers");
_Static_assert(OPENTHREAD_FTD == 0 && OPENTHREAD_MTD == 1 && OPENTHREAD_RADIO == 0, "Wrong Thread role");
_Static_assert(OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_4, "Wrong Thread version");
_Static_assert(OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE == 1, "Matter requires SRP client");
_Static_assert(OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE == 1, "Matter requires DNS client");
_Static_assert(OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE == 1, "SDK owns the shared heap");
_Static_assert(OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS_MANAGEMENT == 0, "Do not replace Matter crypto allocators");
_Static_assert(MBEDTLS_VERSION_NUMBER == 0x03060000, "Wrong mbedTLS headers");
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ || !defined(__riscv_float_abi_single)
#error "Matter package requires little-endian RV32 single-float ABI"
#endif
#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) || defined(MBEDTLS_ECDSA_SIGN_ALT)
#error "Legacy hardware-crypto shims do not match this package"
#endif
