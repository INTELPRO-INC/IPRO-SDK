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

#ifndef __IPROAI_DEF_INTERNAL_H__
#define __IPROAI_DEF_INTERNAL_H__

#define IPROAI_PROFILING
#define IPROAI_SHOW_NETWORK

/* NPU 每層停止 - 用於 EVA 驗證每一層 */
//#define IPROAI_NPU_HALT_EACH_LAYER

#ifdef CONFIG_AI_DEBUG
#define IPROAI_NPU_OPS_DEBUG
#define IPROAI_SHOW_INST
//#define IPROAI_MEMALLOC_DEBUG
//#define EVA_EACH_CPU_LAYER
#endif

#ifdef IPROAI_PROFILING
#define IPROAI_PROFILING_EACH_LAYER
#endif


#endif /* __IPROAI_DEF_INTERNAL_H__ */

