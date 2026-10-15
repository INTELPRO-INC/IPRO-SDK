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

#ifndef IPRO_BR_MEMP_TCM_H
#define IPRO_BR_MEMP_TCM_H

#include "lwip/opt.h"

/* Applied to memp.c only. The IPRO6 linker moves the MLD membership pool
 * to TCM; other pools remain in WRAM. No packet payloads may move to TCM:
 * the Wi-Fi6 port supplies RX custom pbufs, TX uses mem.c's WRAM heap.
 * Preserve every pool count; this is placement, not a capacity reduction. */
#if PBUF_POOL_SIZE != 0 || MEMP_USE_CUSTOM_POOLS || MEM_USE_POOLS || MEMP_MEM_MALLOC
#error "BR BLE TCM placement requires metadata-only static lwIP pools"
#endif
#undef LWIP_DECLARE_MEMORY_ALIGNED
#define LWIP_DECLARE_MEMORY_ALIGNED(name, size) \
    u8_t name[LWIP_MEM_ALIGN_BUFFER(size)] \
    __attribute__((section(".lwip_mem." #name), aligned(MEM_ALIGNMENT)))

#endif
