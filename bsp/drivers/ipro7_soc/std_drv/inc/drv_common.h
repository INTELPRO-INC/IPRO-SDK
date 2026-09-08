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

#ifndef __DRV_COMMON_H_
#define __DRV_COMMON_H_

#include "misc.h"
#include "platform_common.h"
#include "system_soc.h"

#ifndef __NOP
#define __NOP() __ASM volatile("nop") /* This implementation generates debug information */
#endif
#ifndef __WFI
#define __WFI() __ASM volatile("wfi") /* This implementation generates debug information */
#endif
#ifndef __WFE
#define __WFE() __ASM volatile("wfe") /* This implementation generates debug information */
#endif
#ifndef __SEV
#define __SEV() __ASM volatile("sev") /* This implementation generates debug information */
#endif
#ifndef __set_MSP
#define __set_MSP(msp) __ASM volatile("add sp, x0, %0" ::"r"(msp))
#endif

void arch_delay_us(uint32_t cnt);
void arch_delay_ms(uint32_t cnt);
#endif // __DRV_COMMON_H_
