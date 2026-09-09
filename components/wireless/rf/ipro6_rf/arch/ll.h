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

#ifndef __LL_H__
#define __LL_H__

#ifndef __INLINE
#define __INLINE static __attribute__((__always_inline__)) inline
#endif
#ifndef   __ASM
#define __ASM                                  __asm
#endif

#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif

__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_irq__(int do_enable)
{
  if (do_enable) {
    __ASM volatile ("csrsi mstatus, 8");
  }
}

__attribute__( ( always_inline ) ) __STATIC_INLINE int __disable_irq__(void)
{
  register unsigned int mstatus = 0;

  __ASM volatile("csrrci %0, mstatus, 8" : "=r"(mstatus));

  if (mstatus & 8) {
    return 1; // privious mie is enable
  } else {
    return 0;
  }
}

#define GLOBAL_INT_DISABLE()  if (1) { \
  int ____prev_mie____ = __disable_irq__();

#define GLOBAL_INT_RESTORE() \
  __enable_irq__(____prev_mie____); \
}

#endif
