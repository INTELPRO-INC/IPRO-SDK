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

#ifndef __RTSP_TEST_SOURCE_H__
#define __RTSP_TEST_SOURCE_H__

/* Phase 4 canned-frame pull source. Install registers a get_frm_t
 * callback via set_strm_cb(); frames come from rtsp_test_frames.h
 * (128x96 MJPEG, 4-frame cycle). */

#ifdef __cplusplus
extern "C" {
#endif

void rtsp_test_source_install(void);
void rtsp_test_source_uninstall(void);

#ifdef __cplusplus
}
#endif

#endif /* __RTSP_TEST_SOURCE_H__ */
