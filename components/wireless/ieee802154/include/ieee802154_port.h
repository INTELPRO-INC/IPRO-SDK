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
 * @file ieee802154_port.h
 * @brief Portability macros for the IEEE 802.15.4 driver
 *
 * Lets opt-in low-power apps place hot-path functions in fast OCRAM
 * (TCM) so they don't suffer I-cache refills on every PDS wake.
 */

#ifndef __IEEE802154_PORT_H__
#define __IEEE802154_PORT_H__

/* The Zephyr port defines IEEE802154_HOT_SECTION for this library in
 * zephyr/CMakeLists.txt (.ramfunc), so no Zephyr header is included here:
 * <zephyr/toolchain.h> redefines likely/unlikely/STRINGIFY from the SDK's
 * compiler/gcc.h and riscv_bits.h. */
#ifndef IEEE802154_HOT_SECTION
#include <generated/autoconf.h>

#ifdef CONFIG_IEEE802154_LP_HOT_PATH_TCM
#include "compiler/common.h"
/**
 * Hot-path annotation — places the function in .tcmcode (OCRAM) so it
 * runs at SRAM speed without depending on the I-cache (which is flushed
 * on PDS31 wake). Enable by setting CONFIG_IEEE802154_LP_HOT_PATH_TCM=y
 * in the app .config. Costs roughly 5-8 KB of retention OCRAM.
 *
 * Only the Enh-ACK path belongs here (MAC interrupt -> prearm -> ACK
 * trigger, against aTurnaroundTime): init / configuration code stays in
 * flash.  .tcmcode's size moves every OCRAM data section after it, and on
 * the IPRO7 EVB a 2 KB shift of that data brought back PDS31 no-wakes
 * under a parent-loss stress (mechanism unknown, bench 2026-09-28): keep
 * the TCM code size balanced, and re-run that stress when it changes.
 */
#define IEEE802154_HOT_SECTION ATTR_TCM_SECTION
#else
#define IEEE802154_HOT_SECTION
#endif
#endif /* IEEE802154_HOT_SECTION */

/* A static inline helper of the hot path takes the section only: an inline
 * function cannot also be noinline, which the Zephyr port's
 * IEEE802154_HOT_SECTION is (zephyr/CMakeLists.txt). */
#ifndef IEEE802154_HOT_INLINE
#define IEEE802154_HOT_INLINE IEEE802154_HOT_SECTION
#endif

#endif /* __IEEE802154_PORT_H__ */
