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

/**
 * @file uvc_isp.h
 * @brief Minimal ISP/DVP/MJPEG control for the IPRO7 UVC camera.
 *
 * Drives the SC101IOT DVP sensor through the ISP into the hardware MJPEG
 * encoder, then hands each encoded frame to the UVC layer (uvc.c). Continuous
 * streaming only — no PIR / AI / single-shot capture.
 */

#ifndef __UVC_ISP_H__
#define __UVC_ISP_H__

#include <stdint.h>
#include <stdbool.h>
#include <generated/autoconf.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ISP_RES_MODE_VGA,   /*!< 640x480   */
    ISP_RES_MODE_QVGA,  /*!< 320x240   */
    ISP_RES_MODE_720P,  /*!< 1280x720  (SC101IOT native) */
    ISP_RES_MODE_1080P, /*!< 1920x1080 (upscaled)        */
    ISP_RES_MODE_360P,  /*!< 640x360   */
    ISP_RES_MODE_180P,  /*!< 320x180   */
    ISP_RES_MODE_MAX,
} isp_res_mode_e;

/*
 * Preview size: advertised by uvc.c, produced by ISP ch0 + MJPEG, selected in
 * main.c. QVGA on the EVB build (the 640x384 AI model leaves too little PSRAM
 * for more, see uvc.c); the IPRO7AI Reef + OV5640 build has no AI and streams
 * the sensor's native 1280x720.
 *
 * The Reef rock-paper-scissors build (CONFIG_UVC_RPS) streams 640x360 only:
 * the game is drawn into every frame by the CPU (mirror + bars + text), which
 * held a 1280x720 encode to ~11 fps; at 640x360 it encodes 30 fps, which
 * Full-Speed USB also carries.
 */
#if defined(CONFIG_IPRO7AI_REEF) && defined(CONFIG_UVC_RPS)
#define UVC_RES_MODE ISP_RES_MODE_360P
#define UVC_WIDTH    640u
#define UVC_HEIGHT   360u
#elif defined(CONFIG_IPRO7AI_REEF)
#define UVC_RES_MODE ISP_RES_MODE_720P
#define UVC_WIDTH    1280u
#define UVC_HEIGHT   720u
/* second UVC frame (uvc.c): 640x360, for 30 fps over Full-Speed USB */
#define UVC_ALT_WIDTH     640u
#define UVC_ALT_HEIGHT    360u
#define UVC_ALT_RES_MODE  ISP_RES_MODE_360P
#else
#define UVC_RES_MODE ISP_RES_MODE_QVGA
#define UVC_WIDTH    320u
#define UVC_HEIGHT   240u
#endif

/**
 * @brief Select output resolution / color before uvc_isp_init().
 *        Must match the UVC descriptor in uvc.c: pass UVC_RES_MODE.
 */
void uvc_isp_set_param(isp_res_mode_e res_mode, bool is_color);

/**
 * @brief Bring up sensor + ISP + MJPEG and start the continuous stream task.
 *        The encoded frames are pushed to the host via uvc_send_jpg().
 */
void uvc_isp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __UVC_ISP_H__ */
