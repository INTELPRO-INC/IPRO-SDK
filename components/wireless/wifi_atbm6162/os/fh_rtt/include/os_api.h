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

#include <stdio.h>
#include "stddef.h"
#define __INLINE inline
#define iot_printf rt_kprintf
#define  atbm_random()   rand()

#define  rcu_read_lock()
#define  rcu_read_unlock()
#define  TargetUsb_lmac_start()

atbm_uint32 atbm_os_random();
#define ZEROSIZE 0
//#ifndef PACK_STRUCT_BEGIN
//#define PACK_STRUCT_BEGIN
//#endif /* PACK_STRUCT_BEGIN */
  
//#ifndef PACK_STRUCT_END
//#define PACK_STRUCT_END
//#endif /* PACK_STRUCT_END */

