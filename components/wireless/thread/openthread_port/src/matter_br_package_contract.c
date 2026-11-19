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

/* Compiled with the real SDK FTD/Spinel flags, never the MED enum ABI. */
#include <FreeRTOS.h>
#include <task.h>
#include <stddef.h>
#include <openthread-core-config.h>
#include <openthread/error.h>
#include <openthread/instance.h>
#include <ipro_matter_br_thread.h>
#include <mbedtls/build_info.h>
#include <lwip/pbuf.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <MatterPackageAbi.h>
#include <MatterPackageIdentity.h>

#define ABI_CHECK(expression, field) \
    _Static_assert((expression) == IPRO_MATTER_ABI_##field, "Matter BR archive ABI mismatch: " #field)
ABI_CHECK(3, SCHEMA);
ABI_CHECK(sizeof(void *), POINTER_BYTES);
ABI_CHECK(_Alignof(void *), POINTER_ALIGN);
ABI_CHECK(sizeof(otError), ENUM_BYTES);
ABI_CHECK(sizeof(BaseType_t), BASE_BYTES);
ABI_CHECK(sizeof(UBaseType_t), UBASE_BYTES);
ABI_CHECK(sizeof(TickType_t), TICK_BYTES);
ABI_CHECK(_Alignof(TickType_t), TICK_ALIGN);
ABI_CHECK(sizeof(StackType_t), STACK_BYTES);
ABI_CHECK(sizeof(StaticTask_t), STATIC_TASK_BYTES);
ABI_CHECK(_Alignof(StaticTask_t), STATIC_TASK_ALIGN);
ABI_CHECK(sizeof(StaticQueue_t), STATIC_QUEUE_BYTES);
ABI_CHECK(_Alignof(StaticQueue_t), STATIC_QUEUE_ALIGN);
ABI_CHECK(sizeof(StaticEventGroup_t), STATIC_EVENT_BYTES);
ABI_CHECK(_Alignof(StaticEventGroup_t), STATIC_EVENT_ALIGN);
ABI_CHECK(sizeof(StaticStreamBuffer_t), STATIC_STREAM_BYTES);
ABI_CHECK(_Alignof(StaticStreamBuffer_t), STATIC_STREAM_ALIGN);
ABI_CHECK(configTICK_RATE_HZ, TICK_HZ);
ABI_CHECK(configMAX_PRIORITIES, MAX_PRIORITIES);
ABI_CHECK(portBYTE_ALIGNMENT, PORT_ALIGNMENT);
ABI_CHECK(OPENTHREAD_API_VERSION, OT_API);
ABI_CHECK(OPENTHREAD_CONFIG_THREAD_VERSION, THREAD_VERSION);
ABI_CHECK(configSUPPORT_STATIC_ALLOCATION, STATIC_ALLOCATION);
ABI_CHECK(configSUPPORT_DYNAMIC_ALLOCATION, DYNAMIC_ALLOCATION);
ABI_CHECK(sizeof(TimeOut_t), TIMEOUT_BYTES);
ABI_CHECK(_Alignof(TimeOut_t), TIMEOUT_ALIGN);
ABI_CHECK(offsetof(TimeOut_t, xOverflowCount), TIMEOUT_OVERFLOW_OFFSET);
ABI_CHECK(offsetof(TimeOut_t, xTimeOnEntering), TIMEOUT_TICK_OFFSET);
ABI_CHECK(configTICK_TYPE_WIDTH_IN_BITS, TICK_CONFIG_WIDTH);
ABI_CHECK(OPENTHREAD_FTD, OT_FTD);
ABI_CHECK(OPENTHREAD_MTD, OT_MTD);
ABI_CHECK(OPENTHREAD_RADIO, OT_RADIO);
ABI_CHECK(sizeof(ipro_matter_br_thread_state), THREAD_STATE_BYTES);
ABI_CHECK(_Alignof(ipro_matter_br_thread_state), THREAD_STATE_ALIGN);
ABI_CHECK(offsetof(ipro_matter_br_thread_state, service_name), THREAD_STATE_SERVICE_NAME_OFFSET);
ABI_CHECK(sizeof(((ipro_matter_br_thread_state *)0)->service_name), THREAD_STATE_SERVICE_NAME_CAPACITY);
ABI_CHECK(sizeof(otOperationalDatasetTlvs), DATASET_TLVS_BYTES);
ABI_CHECK(_Alignof(otOperationalDatasetTlvs), DATASET_TLVS_ALIGN);
ABI_CHECK(offsetof(otOperationalDatasetTlvs, mLength), DATASET_TLVS_LENGTH_OFFSET);
ABI_CHECK(MBEDTLS_VERSION_NUMBER, MBEDTLS_VERSION);
ABI_CHECK(sizeof(struct pbuf), PBUF_BYTES);
ABI_CHECK(_Alignof(struct pbuf), PBUF_ALIGN);
ABI_CHECK(sizeof(ip_addr_t), IP_ADDR_BYTES);
ABI_CHECK(_Alignof(ip_addr_t), IP_ADDR_ALIGN);
ABI_CHECK(sizeof(struct netif), NETIF_BYTES);
ABI_CHECK(_Alignof(struct netif), NETIF_ALIGN);
#undef ABI_CHECK
_Static_assert(CONFIG_MATTER_VENDOR_ID == IPRO_MATTER_IDENTITY_VENDOR_ID, "BR vendor mismatch");
_Static_assert(CONFIG_MATTER_PRODUCT_ID == IPRO_MATTER_IDENTITY_PRODUCT_ID, "BR product mismatch");
_Static_assert(CONFIG_MATTER_DEVICE_TYPE == IPRO_MATTER_IDENTITY_DEVICE_TYPE, "BR device type mismatch");
_Static_assert(OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS_MANAGEMENT == 0, "Shared crypto allocator ownership");
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ || !defined(__riscv_float_abi_single)
#error "Matter BR requires little-endian RV32 single-float ABI"
#endif
#if defined(IPRO_CRYPT_HARDWARE) || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) || defined(MBEDTLS_ECDSA_SIGN_ALT)
#error "Legacy crypto shims do not match the BR package"
#endif
