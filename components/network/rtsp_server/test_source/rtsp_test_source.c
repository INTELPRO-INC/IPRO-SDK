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
 * Phase 4 canned-frame pull source for the RTSP server.
 *
 * Feeds pre-encoded MJPEG frames (128x96, ~1.6 KB each, 4-frame cycle)
 * from rtsp_test_frames.h through the RTSP library's get_frm_t callback
 * installed via set_strm_cb(). Pacing is enforced locally because the
 * RTSP library polls at ~125 fps on success; we return 0 (retry) until
 * 1/CONFIG_RTSP_DEFAULT_VIDEO_FPS seconds have elapsed.
 *
 * Phase 5 replaces this with an ISP JPEG-done hook using the same
 * get_frm_t signature.
 */

#include <stdint.h>
#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"

#include "librtspsrv.h"
#include "rtsp_test_source.h"
#include "rtsp_test_frames.h"

static TickType_t s_last_tick;
static uint32_t   s_rtp_ts;
static uint32_t   s_idx;

static int get_frm_cb(struct strm_info *si, struct frm_info *fi)
{
    (void)si;

    TickType_t now = xTaskGetTickCount();
    TickType_t min_gap = pdMS_TO_TICKS(1000 / CONFIG_RTSP_DEFAULT_VIDEO_FPS);

    if (s_last_tick != 0 && (TickType_t)(now - s_last_tick) < min_gap) {
        /* Not time for next frame yet — library retries in 1ms. */
        return 0;
    }
    s_last_tick = now;

    uint32_t slot = s_idx % MJPEG_FRAME_COUNT;
    fi->frm_buf   = (char *)mjpeg_frames[slot];
    fi->frm_sz    = (unsigned int)mjpeg_frames_len[slot];
    fi->frm_type  = FRM_TYPE_M;     /* MJPEG */
    fi->timestamp = s_rtp_ts;

    /* librtspsrv re-scales frm_info->timestamp by *90 in send_mjpeg_frm
     * to produce 90 kHz RTP units, so caller passes ms — not 90 kHz
     * units. Earlier value (90000/fps) double-scaled to 18 s/frame and
     * froze decoders. */
    s_rtp_ts += (1000U / CONFIG_RTSP_DEFAULT_VIDEO_FPS);
    s_idx++;

    return (int)fi->frm_sz;
}

void rtsp_test_source_install(void)
{
    s_last_tick = 0;
    s_rtp_ts = 0;
    s_idx = 0;
    set_strm_cb(get_frm_cb);
}

void rtsp_test_source_uninstall(void)
{
    set_strm_cb(NULL);
}
