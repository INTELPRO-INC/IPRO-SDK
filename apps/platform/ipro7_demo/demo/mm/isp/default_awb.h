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


#ifndef _DEFAULT_AWB_H_
#define _DEFAULT_AWB_H_

/****************************************************************************
*                         Public Constant Definition                        *
****************************************************************************/
#define CMODEL_AWB    {\
                /*stat_x_min*/ \
                0, \
                /*stat_x_max*/ \
                1279, \
                /*stat_y_min*/ \
                0, \
                /*stat_y_max*/ \
                719, \
                /*r_ratio_ofst*/ \
                230, \
                /*b_ratio_ofst*/ \
                300, \
                /*r_ratio_mult*/ \
                175, \
                /*b_ratio_mult*/ \
                115, \
                /*ratio_sum_min*/ \
                192, \
                /*ratio_sum_max*/ \
                288, \
                /*g_divisor*/ \
                0, \
                /*weight_ratio_en*/ \
                1, \
                /*weight_ratio_gain*/ \
                16, \
                { \
                    { \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        8, \
                        16, \
                        16, \
                        8, \
                    }, \
                    { \
                        0, \
                        0, \
                        0, \
                        0, \
                        8, \
                        48, \
                        96, \
                        48, \
                        16, \
                    }, \
                    { \
                        0, \
                        0, \
                        0, \
                        8, \
                        96, \
                        192, \
                        64, \
                        16, \
                        0, \
                    }, \
                    { \
                        0, \
                        0, \
                        0, \
                        96, \
                        255, \
                        48, \
                        8, \
                        0, \
                        0, \
                    }, \
                    { \
                        0, \
                        0, \
                        48, \
                        160, \
                        160, \
                        0, \
                        0, \
                        0, \
                        0, \
                    }, \
                    { \
                        0, \
                        16, \
                        96, \
                        64, \
                        64, \
                        0, \
                        0, \
                        0, \
                        0, \
                    }, \
                    { \
                        8, \
                        48, \
                        48, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                    }, \
                    { \
                        16, \
                        48, \
                        16, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                    }, \
                    { \
                        8, \
                        16, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                    }, \
                }, \
                /*y_r_coeff*/ \
                77, \
                /*y_g_coeff*/ \
                150, \
                /*y_b_coeff*/ \
                29, \
                /*weight_y_use_g*/ \
                1, \
                /*weight_y_en*/ \
                1, \
                /*weight_y_gain*/ \
                16, \
                { \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                    { \
                        0, \
                        32, \
                        64, \
                        96, \
                        128, \
                        160, \
                        192, \
                        224, \
                        255, \
                    }, \
                }, \
                /*count_zero_weight*/ \
                0, \
                /*ink_type*/ \
                0, \
                /*ink_gain*/ \
                1, \
}

#define CMODEL_AWB_EN   (\
    /*stat_en*/ \
    1 \
)

#endif /*_DEFAULT_AWB_H_*/

