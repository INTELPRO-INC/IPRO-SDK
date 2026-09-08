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


#ifndef _DEFAULT_COLOR_TRANS_H_
#define _DEFAULT_COLOR_TRANS_H_

/****************************************************************************
*                         Public Constant Definition                        *
****************************************************************************/
#define CMODEL_COLOR_TRANS_PRE_OFFSET    {\
        { \
            /*pre_offset_0*/ \
            0, \
            /*pre_offset_1*/ \
            -2048, \
            /*pre_offset_2*/ \
            -2048, \
        }, \
}

#define CMODEL_COLOR_TRANS_POST_OFFSET    {\
        { \
            /*post_offset_0*/ \
            0, \
            /*post_offset_1*/ \
            0, \
            /*post_offset_2*/ \
            0, \
        }, \
}

#define CMODEL_COLOR_TRANS_MATRIX    {\
        { \
            /*matrix_00*/ \
            512, \
            /*matrix_01*/ \
            0, \
            /*matrix_02*/ \
            718, \
            /*matrix_10*/ \
            512, \
            /*matrix_11*/ \
            -176, \
            /*matrix_12*/ \
            -366, \
            /*matrix_20*/ \
            512, \
            /*matrix_21*/ \
            907, \
            /*matrix_22*/ \
            0, \
        }, \
}

#endif /*_DEFAULT_COLOR_TRANS_H_*/

