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

#ifndef __DRV_MJCOMB_H__
#define __DRV_MJCOMB_H__

#include <stdint.h>
//#include "mjcomb_csr_reg.h"

#ifndef CHECK_PARAM
#define CHECK_PARAM(x, ret) \
	do { \
		if (!(x)) { \
			return ret; \
		}\
	} while (0)
#endif


typedef enum {
    MJCOMB_AXI_XLEN_SINGLE = 0,
    MJCOMB_AXI_XLEN_INCR4  = 1,
    MJCOMB_AXI_XLEN_INCR8  = 2,
    MJCOMB_AXI_XLEN_INCR16 = 3,
    //rsvd.//MJCOMB_AXI_XLEN_INCR32 = 5,
    //rsvd.//MJCOMB_AXI_XLEN_INCR64 = 6,
} MJCOMB_AXI_XLEN_E;

#define MJCOMBO_AXI_BURST_INCR16_ALIGNMENT_MASK     ( 0x7F )


typedef enum {
    MJCOMBO_HW_STATE_UNINIT         = 0x00,
    MJCOMBO_HW_STATE_ENC_INIT       = 0x11,
    MJCOMBO_HW_STATE_ENC_RUNNING    = 0x12,
    MJCOMBO_HW_STATE_ENC_IDLE       = 0x13,
    MJCOMBO_HW_STATE_DEC_INIT       = 0x21,
    MJCOMBO_HW_STATE_DEC_RUNNING    = 0x22,
    MJCOMBO_HW_STATE_DEC_IDLE       = 0x23,
} MJCOMBO_HW_STATE;

#define IS_MJCOMBO_HW_STATE_FOR_ENC(state) ((state&0xF0) == 0x10)
#define IS_MJCOMBO_HW_STATE_FOR_DEC(state) ((state&0xF0) == 0x20)


extern volatile MJCOMBO_HW_STATE g_nMJComboHWState;


#endif /* __DRV_MJCOMB_H__ */

