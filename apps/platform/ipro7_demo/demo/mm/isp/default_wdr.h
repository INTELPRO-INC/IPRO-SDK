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


#ifndef _DEFAULT_WDR_H_
#define _DEFAULT_WDR_H_

/****************************************************************************
*                         Public Constant Definition                        *
****************************************************************************/
#define CMODEL_WDR    {\
        /*cs_en*/\
        1, \
        /*hist_roi_en[0]*/\
        1, \
        /*hist_roi_weight[0]*/\
        1, \
        /*hist_x_min[0]*/\
        0, \
        /*hist_x_max[0]*/\
        1279, \
        /*hist_y_min[0]*/\
        0, \
        /*hist_y_max[0]*/\
        719, \
        /*curve_en*/\
        1, \
        /*curve_weight*/\
        16, \
        /*enh_en*/\
        1, \
        /*enh_weight*/\
        16, \
        /*enh_y_offset*/\
        260, \
        /*enh_y_gain*/\
        5030, \
        /*enh_c_gain*/\
        5815, \
}

#define CMODE_WDR_CURVE_TABLE {\
        {\
            0, \
            16, \
            32, \
            48, \
            64, \
            80, \
            96, \
            112, \
            129, \
            147, \
            166, \
            185, \
            202, \
            218, \
            231, \
            243, \
            256, \
            271, \
            286, \
            303, \
            320, \
            338, \
            355, \
            373, \
            390, \
            407, \
            423, \
            441, \
            459, \
            478, \
            497, \
            513, \
            528, \
            543, \
            558, \
            576, \
            594, \
            612, \
            630, \
            647, \
            662, \
            675, \
            687, \
            698, \
            711, \
            728, \
            747, \
            765, \
            782, \
            799, \
            815, \
            830, \
            845, \
            860, \
            875, \
            890, \
            905, \
            918, \
            930, \
            943, \
            954, \
            966, \
            981, \
            1002, \
        },\
}

#define CMODEL_WDR_CS_TYPE    {\
        /*cs_gain*/\
        4195, \
        /*cs_weight*/\
        8, \
        /*cs_grey_th*/\
        48, \
}

#endif /*_DEFAULT_WDR_H_*/

