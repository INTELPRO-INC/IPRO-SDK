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


#ifndef _DEFAULT_YUVADJ_H_
#define _DEFAULT_YUVADJ_H_

/****************************************************************************
*                         Public Constant Definition                        *
****************************************************************************/
#define CMODEL_YUVADJ    {\
                /*y_luma*/\
                0, \
                /*y_mul0*/\
                862, \
                /*y_mul1*/\
                896, \
                /*y_min*/\
                16, \
                /*y_max*/\
                235, \
                /*uv_mul0*/\
                903, \
                /*uv_mul1*/\
                896, \
                /*uv_min*/\
                16, \
                /*uv_max*/\
                240, \
}

#define CMODEL_YUVADJ_EN    (\
        /*eb*/\
        1 \
)

#endif /*_DEFAULT_YUVADJ_H_*/

