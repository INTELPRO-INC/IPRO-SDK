/**
 * @file reef_jpeg.c
 * @brief One-shot JPEG encode of a camera frame (see reef_jpeg.h)
 *
 * Copyright (c) 2026 INTELPRO
 */

#include "reef_jpeg.h"

#include <stdbool.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "hal_mjpeg.h"
#include <system_soc.h>
#include "ipro_log.h"

#define LOG_TAG             "JPEG"

#define JPEG_OUT_BUF_SIZE   (192u * 1024u)    /* 640x360 at Q50 is ~25-40 KB */
#define JPEG_TIMEOUT_MS     1000u
#define JPEG_HEADER_MAX     800u

static SemaphoreHandle_t s_done;
static uint8_t  *s_out;
static uint8_t   s_header[JPEG_HEADER_MAX];
static uint16_t  s_w, s_h;
static bool      s_ready, s_first;

static void jpeg_isr_cb(uint32_t event)
{
    BaseType_t woken = pdFALSE;

    if (event == MJENC_HAL_INT_NORMAL) {
        xSemaphoreGiveFromISR(s_done, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

int reef_jpeg_init(uint16_t width, uint16_t height, uint8_t quality)
{
    struct mjpeg_hal_cfg_t cfg = {0};
    uint32_t header_len;

    if (s_ready) {
        if (width == s_w && height == s_h) {
            return 0;
        }
        reef_jpeg_deinit();
    }
    if (width == 0u || height == 0u || (width % 16u) != 0u || (height % 8u) != 0u) {
        IPRO_LOGE(LOG_TAG, "%ux%u is not a whole number of 16x8 MCUs", width, height);
        return -1;
    }
    if (s_done == NULL) {
        s_done = xSemaphoreCreateBinary();
        if (s_done == NULL) {
            return -1;
        }
    }
    if (s_out == NULL) {
        s_out = pvPortMalloc(JPEG_OUT_BUF_SIZE);
        if (s_out == NULL) {
            IPRO_LOGE(LOG_TAG, "no %u KB output buffer", (unsigned)(JPEG_OUT_BUF_SIZE / 1024u));
            return -1;
        }
    }

    cfg.format = MJPEG_HAL_FORMAT_YUV422_YUYV;
    cfg.quality = quality;
    cfg.rows = height;                     /* whole frame per encode */
    cfg.resolution_x = width;
    cfg.resolution_y = height;
    cfg.input_bufaddr0 = 0;                /* set per encode */
    cfg.input_bufaddr1 = 0;
    cfg.output_bufaddr = (uint32_t)(uintptr_t)s_out;
    cfg.output_bufsize = JPEG_OUT_BUF_SIZE;
    cfg.output_swap_mode_en = 0;           /* frame mode */
    cfg.hw_mode_swen_en = 0;               /* SW mode, no camera sync */
    cfg.callback = jpeg_isr_cb;
    if (mjpeg_init(&cfg) != 0) {
        IPRO_LOGE(LOG_TAG, "mjpeg_init failed");
        return -1;
    }
    header_len = mjpeg_headerCreate(YUV_MODE_422, quality, width, height, s_header);
    mjpeg_fill_header_tail(s_header, header_len);

    s_w = width;
    s_h = height;
    s_first = true;
    s_ready = true;
    IPRO_LOGI(LOG_TAG, "encoder ready: %ux%u YUYV, quality %u", width, height, quality);
    return 0;
}

void reef_jpeg_deinit(void)
{
    if (s_ready) {
        mjpeg_stop();
        mjpeg_deinit();
        s_ready = false;
    }
}

int32_t reef_jpeg_encode(uint32_t yuv_addr, uint8_t *out, uint32_t out_max)
{
    uint32_t addr = 0, len = 0;
    int32_t ret;

    if (!s_ready || yuv_addr == 0u || out == NULL) {
        return -1;
    }

    /* The ISP's DMA wrote the frame behind the cache. */
    L1C_DCACHE_INVALID_RANGE((uintptr_t)yuv_addr, (uint32_t)s_w * s_h * 2u);
    mjpeg_set_input_addr(MJPEG_HAL_SET_ADDR_Y, yuv_addr);
    mjpeg_set_input_addr(MJPEG_HAL_SET_ADDR_UV, yuv_addr + (uint32_t)s_w * s_h);

    (void)xSemaphoreTake(s_done, 0);
    if (s_first) {
        mjpeg_set_mode(1, 1, 1);           /* SW, frame mode, one frame */
        mjpeg_start(1);
        s_first = false;
    } else {
        mjpeg_sw_run_next();
    }
    if (xSemaphoreTake(s_done, pdMS_TO_TICKS(JPEG_TIMEOUT_MS)) != pdTRUE) {
        IPRO_LOGE(LOG_TAG, "encode timeout");
        return -2;
    }
    if (mjpeg_get_one_frame(&addr, &len) != 0 || len == 0u) {
        return -3;
    }
    if (len > out_max) {
        ret = -4;
    } else {
        L1C_DCACHE_INVALID_RANGE((uintptr_t)addr, len);
        memcpy(out, (const void *)(uintptr_t)addr, len);
        ret = (int32_t)len;
    }
    mjpeg_drop_one_frame();
    return ret;
}
