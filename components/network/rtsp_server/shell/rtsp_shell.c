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
 * Phase 4 dev shell commands for the RTSP server. SHELL_EXEC_THREAD=y
 * runs each command in its own task, so these commands only touch
 * state that is either local-stack or serialized via the RTSP
 * library's own locking.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librtspsrv.h"
#ifdef CONFIG_RTSP_TEST_SOURCE
#include "rtsp_test_source.h"
#endif
#ifdef CONFIG_RTSP_ISP_SOURCE
#include "rtsp_isp_source.h"
#endif
#include "shell.h"

/* Library quirk: rtsp_set_*Fmt takes RTSP_*FMT_* enum but
 * rtsp_get_*Fmt returns the RTP payload type (26=JPEG, 96=H264,
 * 0=PCMU, 8=PCMA, 97=OPUS, etc.). Decode the RTP PT here. */
static const char *video_fmt_name(int rtp_pt)
{
    switch (rtp_pt) {
    case 26: return "mjpeg";
    case 96: return "h264";
    default: return "?";
    }
}

static const char *audio_fmt_name(int rtp_pt)
{
    switch (rtp_pt) {
    case 0:  return "pcmu";
    case 8:  return "pcma";
    case 97: return "opus";
    case 98: return "aac";
    default: return "?";
    }
}

static int audio_fmt_from_name(const char *s)
{
    if (!strcmp(s, "aac"))  return RTSP_AUDIOFMT_AAC;
    if (!strcmp(s, "pcma")) return RTSP_AUDIOFMT_PCMA;
    if (!strcmp(s, "pcmu")) return RTSP_AUDIOFMT_PCMU;
    if (!strcmp(s, "opus")) return RTSP_AUDIOFMT_OPUS;
    return -1;
}

/* rtsp_init — start listener, set MJPEG + default fps. */
static int cmd_rtsp_init(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = rtsp_init_lib();
    rtsp_set_video_en(1);
    rtsp_set_videoFmt(RTSP_VIDEOFMT_MJPEG);
    rtsp_set_video_fps(CONFIG_RTSP_DEFAULT_VIDEO_FPS);
    printf("rtsp_init: rc=%d port=%d fps=%d video=%s\r\n",
           rc, CONFIG_RTSP_LISTEN_PORT,
           CONFIG_RTSP_DEFAULT_VIDEO_FPS,
           video_fmt_name(RTSP_VIDEOFMT_MJPEG));
    return rc;
}

static int cmd_rtsp_deinit(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = rtsp_deinit_lib();
    printf("rtsp_deinit: rc=%d\r\n", rc);
    return rc;
}

static int cmd_rtsp_stats(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("rtsp_stats:\r\n");
    printf("  video_en  = %d (fmt=%s)\r\n",
           rtsp_get_video_en(), video_fmt_name(rtsp_get_videoFmt()));
    printf("  audio_en  = %d (fmt=%s)\r\n",
           rtsp_get_audio_en(), audio_fmt_name(rtsp_get_audioFmt()));
    printf("  video_fps = %d\r\n", rtsp_get_video_fps());
    return 0;
}

#ifdef CONFIG_RTSP_ISP_SOURCE
/* rtsp_isp_src on|off — install/uninstall the real ISP+MJPEG pull source.
 * Installing fails if rtsp_test_src is currently installed (only one
 * get_frm_t cb at a time); the user must turn the other off first. */
static int cmd_rtsp_isp_src(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: rtsp_isp_src on|off|stats\r\n");
        return -1;
    }
    if (!strcmp(argv[1], "on")) {
        int rc = rtsp_isp_source_install();
        printf("rtsp_isp_src: install rc=%d\r\n", rc);
        return rc;
    }
    if (!strcmp(argv[1], "off")) {
        rtsp_isp_source_uninstall();
        printf("rtsp_isp_src: removed\r\n");
        return 0;
    }
    if (!strcmp(argv[1], "stats")) {
        rtsp_isp_source_stats();
        return 0;
    }
    printf("bad arg: %s\r\n", argv[1]);
    return -1;
}
#endif

#ifdef CONFIG_RTSP_TEST_SOURCE
/* rtsp_test_src on|off — install/uninstall canned-frame pull callback. */
static int cmd_rtsp_test_src(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: rtsp_test_src on|off\r\n");
        return -1;
    }
    if (!strcmp(argv[1], "on")) {
        rtsp_test_source_install();
        printf("rtsp_test_src: installed\r\n");
        return 0;
    }
    if (!strcmp(argv[1], "off")) {
        rtsp_test_source_uninstall();
        printf("rtsp_test_src: removed\r\n");
        return 0;
    }
    printf("bad arg: %s\r\n", argv[1]);
    return -1;
}
#endif

/* rtsp_audio_en on|off [codec=pcmu|pcma|aac|opus]
 *   When on, SDP advertises audio track. Phase 4 stubs RTP audio push. */
static int cmd_rtsp_audio_en(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: rtsp_audio_en on|off [pcmu|pcma|aac|opus]\r\n");
        return -1;
    }
    if (!strcmp(argv[1], "off")) {
        rtsp_set_audio_en(0);
        printf("rtsp_audio_en: off\r\n");
        return 0;
    }
    if (!strcmp(argv[1], "on")) {
        int fmt = RTSP_AUDIOFMT_PCMU;
        if (argc > 2) {
            fmt = audio_fmt_from_name(argv[2]);
            if (fmt < 0) {
                printf("bad codec: %s\r\n", argv[2]);
                return -1;
            }
        }
        rtsp_set_audioFmt(fmt);
        rtsp_set_audio_en(1);
        printf("rtsp_audio_en: on fmt=%s\r\n", audio_fmt_name(fmt));
        return 0;
    }
    printf("bad arg: %s\r\n", argv[1]);
    return -1;
}

SHELL_CMD_EXPORT_ALIAS(cmd_rtsp_init,    rtsp_init,    Start RTSP server + MJPEG defaults);
SHELL_CMD_EXPORT_ALIAS(cmd_rtsp_deinit,  rtsp_deinit,  Stop RTSP server);
SHELL_CMD_EXPORT_ALIAS(cmd_rtsp_stats,   rtsp_stats,   Print RTSP server config snapshot);
#ifdef CONFIG_RTSP_TEST_SOURCE
SHELL_CMD_EXPORT_ALIAS(cmd_rtsp_test_src, rtsp_test_src, Install/remove canned-frame pull source);
#endif
#ifdef CONFIG_RTSP_ISP_SOURCE
SHELL_CMD_EXPORT_ALIAS(cmd_rtsp_isp_src, rtsp_isp_src, Install/remove ISP+MJPEG pull source);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_rtsp_audio_en, rtsp_audio_en, Toggle audio SDP track + codec);
