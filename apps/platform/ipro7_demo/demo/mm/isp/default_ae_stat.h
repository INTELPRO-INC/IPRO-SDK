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


#ifndef _DEFAULT_AE_STAT_H_
#define _DEFAULT_AE_STAT_H_

/****************************************************************************
*                         Public Constant Definition                        *
****************************************************************************/
#define CMODEL_AE_STAT    {\
                /*roi_start_x*/ \
                4, \
                /*roi_start_y*/ \
                0, \
                /*roi_width*/ \
                1272, \
                /*roi_height*/ \
                720, \
                /*image_width*/ \
                1280, \
                /*image_height*/ \
                720, \
                /*grid_width*/ \
                106, \
                /*grid_height*/ \
                90, \
                /*grid_x_max*/ \
                12, \
                /*grid_y_max*/ \
                8, \
}

#define CMODE_AE_SUM_COEFF {\
                /*ysum_coeff_r*/ \
                77, \
                /*ysum_coeff_g*/ \
                150, \
                /*ysum_coeff_b*/ \
                29, \
}

#define CMODEL_AE_STAT_EN    (\
        /*sta_enable*/\
        1 \
)

#endif /*_DEFAULT_AE_STAT_H_*/

