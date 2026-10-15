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

#ifndef _AIC_COMPILER_H_
#define _AIC_COMPILER_H_

#ifndef __INLINE

#define __INLINE     //inline
#endif /* _COMPILER_H_ */

#ifndef __STATIC_INLINE
#define __STATIC_INLINE   static inline
#endif /* __STATIC_INLINE */


#if defined(__CC_ARM)
#pragma anon_unions
#endif

#if defined ( __ICCARM__ ) /*!< IAR Compiler */ 
#define DECLARE_ALIGNED_64() _Pragma("data_alignment=64") 
#define DECLARE_ALIGNED_32() _Pragma("data_alignment=32") 
#else
#define __ALIGN__(n)
#endif


#define dbg_vsnprintf vsnprintf
#define dbg_vsprintf  vsprintf
#endif /* _AIC_COMPILER_H_ */
