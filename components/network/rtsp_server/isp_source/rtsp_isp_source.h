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

/*
 * Phase 5 ISP -> MJPEG pull source for the RTSP server.
 *
 * Boots the IPRO7 sensor + ISP pipeline (Ch0 YUV ping-pong in PSRAM),
 * encodes one JPEG per RTSP pull via MJPEG SW frame mode, and wires
 * the result through the RTSP library's get_frm_t callback installed by
 * set_strm_cb().  Pacing tracks the configured RTSP fps; the encoder
 * runs only when a new ISP frame is available since the previous pull.
 *
 * Pipeline diagram:
 *   sensor -> ISP -> Ch0 (YUV422 ping-pong, freerun src=30 dst=fps)
 *                       |
 *                       v
 *                    MJPEG SW frame encode (on get_frm_cb)
 *                       |
 *                       v
 *                    JPEG buf -> RTSP -> RTP/JPEG (PT 26)
 *
 * Mutually exclusive with rtsp_test_source: install one or the other.
 */

#ifndef __RTSP_ISP_SOURCE_H__
#define __RTSP_ISP_SOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Bring up the sensor + ISP + MJPEG and register the get_frm_t callback.
 * Idempotent.  Returns 0 on success, negative on failure.
 */
int rtsp_isp_source_install(void);

/*
 * Tear down the pipeline and clear the callback.
 */
void rtsp_isp_source_uninstall(void);

/*
 * Print debug counters (encode count, encode failures, ISP frame count,
 * convergence state).  Useful from a shell command.
 */
void rtsp_isp_source_stats(void);

#ifdef __cplusplus
}
#endif

#endif /* __RTSP_ISP_SOURCE_H__ */
