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

#ifndef ATBM_OS_SPINLOCK_H
#define ATBM_OS_SPINLOCK_H
#include "atbm_type.h"
#include "rtdef.h"

typedef   atbm_uint8 atbm_spinlock_t;
/*spin lock*/
#define atbm_spin_lock_init(x)
#define atbm_spin_lock(x) rt_enter_critical()
#define atbm_spin_unlock(x) rt_exit_critical()
#define atbm_spin_lock_irqsave(x,f) do {*(f) = rt_hw_interrupt_disable();}while(0)//rt_enter_critical()
#define atbm_spin_unlock_irqrestore(x,f) rt_hw_interrupt_enable(f)//rt_exit_critical()
#define atbm_spin_lock_bh(x) rt_enter_critical()
#define atbm_spin_unlock_bh(x) rt_exit_critical()

#define atbm_disable_irq() rt_hw_interrupt_disable()
#define atbm_enable_irq(f) rt_hw_interrupt_enable(f)
#endif /* ATBM_OS_SPINLOCK_H */

