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
 * @file uvc.c
 * @brief Minimal USB UVC (Video Class) device for the IPRO7 UVC camera.
 *
 * Streams hardware-encoded MJPEG frames from the ISP (uvc_isp.c) to a USB
 * host over an isochronous endpoint. There is no shell mode switch: the
 * stream is always enabled and starts as soon as the host opens it.
 *
 * Default format: MJPEG 1280x720 @ 30fps (SC101IOT native).
 */

#include <generated/autoconf.h>
#include "ipro_vendor_ids.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drv_glb.h"
#include "usbd_core.h"
#include "usbd_video.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hal_common.h"
#include "hal_dma.h"
#include "semphr.h"
#include "compiler/common.h"
#include "uvc_isp.h"

#define VIDEO_IN_EP 0x81

/* Full-Speed iso payload per 1 ms frame (~1 MB/s ceiling). If the host never
 * selects alt 1, check the macOS UVCAssistant log for a failed GET_CUR(PROBE)
 * before blaming bandwidth: that was the EP0 OUT_DONE handling in
 * drv_usb_v1.c, not the iso size or a hub. */
#define MAX_PAYLOAD_SIZE  1020
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))

/* UVC preview (MJPEG) resolution advertised to the host. QVGA 320x240 because the
 * 640x384 gesture model's 5 MB NPU activation buffer + 0.98 MB input buffer leave
 * too little PSRAM for a VGA ch0 ping-pong (see uvc_isp.c). Detection still runs at
 * full 640x384; only this preview is smaller. Restore 640x480 once the model's
 * activation buffer is <= ~4.4 MB (compile with -u 32). The size is set once in
 * uvc_isp.h (UVC_WIDTH/UVC_HEIGHT/UVC_RES_MODE, 720p on the Reef build). */
#define WIDTH  (unsigned int)(UVC_WIDTH)
#define HEIGHT (unsigned int)(UVC_HEIGHT)

#define CAM_FPS        (30)
#define INTERVAL       (unsigned long)(10000000 / CAM_FPS)
#define MIN_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS)
#define MAX_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS)
#define MAX_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 2)

/* A second, smaller frame for a smooth stream: the Full-Speed iso link
 * carries ~1 MB/s, and a 720p MJPEG frame of a busy scene is 60-75 KB even
 * at q50 (~15 fps), where 640x360 is ~18 KB and keeps 30 fps. The host
 * picks one; uvc_isp.c follows the committed frame index. */
#if defined(UVC_ALT_WIDTH)
#define UVC_NUM_FRAMES     2
#define ALT_MIN_BIT_RATE   (unsigned long)(UVC_ALT_WIDTH * UVC_ALT_HEIGHT * 16 * CAM_FPS)
#define ALT_MAX_BIT_RATE   ALT_MIN_BIT_RATE
#define ALT_MAX_FRAME_SIZE (unsigned long)(UVC_ALT_WIDTH * UVC_ALT_HEIGHT * 2)
#else
#define UVC_NUM_FRAMES     1
#endif
#define FRAME_DESC_SIZ     30

#define USB_VIDEO_DESC_SIZ (unsigned long)(9 + 8 + 9 + 13 + 18 + 9 + 12 + 9 + 14 + 11 + FRAME_DESC_SIZ * UVC_NUM_FRAMES + 9 + 7)
#define VC_TERMINAL_SIZ    (unsigned int)(13 + 18 + 12 + 9)
#define VS_HEADER_SIZ      (unsigned int)(13 + 1 + 11 + FRAME_DESC_SIZ * UVC_NUM_FRAMES)

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0701
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/* Packet staging buffer — holds exactly ONE MJPEG frame (+~1.6% UVC-header
 * overhead) before the iso push. At q50 VGA a frame is ~20-35KB, so 256KB is
 * ample; uvc_frame_fill() drops any frame that would not fit (no corruption).
 * Kept at 256KB so the two full-frame VGA ch0 ping-pong buffers fit in PSRAM
 * alongside the ~6MB resident AI model. */
#define UVC_PACKET_BUF_SIZE (256 * 1024)

/* ============================================================================
 * USB Video Descriptor
 * ============================================================================ */

static const uint8_t video_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    VIDEO_VC_DESCRIPTOR_INIT(0x00, 0, 0x0100, VC_TERMINAL_SIZ, 48000000, 0x02),
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x00),
    VIDEO_VS_HEADER_DESCRIPTOR_INIT(0x01, VS_HEADER_SIZ, VIDEO_IN_EP, 0x00),
    VIDEO_VS_FORMAT_MJPEG_DESCRIPTOR_INIT(0x01, UVC_NUM_FRAMES),
    VIDEO_VS_FRAME_MJPEG_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, MAX_FRAME_SIZE, DBVAL(INTERVAL), 0x01, DBVAL(INTERVAL)),
#if UVC_NUM_FRAMES > 1
    VIDEO_VS_FRAME_MJPEG_DESCRIPTOR_INIT(0x02, UVC_ALT_WIDTH, UVC_ALT_HEIGHT, ALT_MIN_BIT_RATE, ALT_MAX_BIT_RATE, ALT_MAX_FRAME_SIZE, DBVAL(INTERVAL), 0x01, DBVAL(INTERVAL)),
#endif
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x01, 0x01),
    /* Isochronous Video Data Endpoint */
    0x07,                         /* bLength */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType */
    0x81,                         /* bEndpointAddress: IN endpoint 1 */
    0x01,                         /* bmAttributes: Isochronous */
    WBVAL(VIDEO_PACKET_SIZE),     /* wMaxPacketSize */
    0x01,                         /* bInterval */

    /* String 0: Language ID */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* String 1: Manufacturer - "CherryUSB" */
    0x1C,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'I', 0x00,                  /* wcChar0 */
    'N', 0x00,                  /* wcChar1 */
    'T', 0x00,                  /* wcChar2 */
    'E', 0x00,                  /* wcChar3 */
    'L', 0x00,                  /* wcChar4 */
    'P', 0x00,                  /* wcChar5 */
    'R', 0x00,                  /* wcChar6 */
    'O', 0x00,                  /* wcChar7 */
    ' ', 0x00,                  /* wcChar8 */
    'I', 0x00,                  /* wcChar9 */
    'N', 0x00,                  /* wcChar10 */
    'C', 0x00,                  /* wcChar11 */
    '.', 0x00,                  /* wcChar12 */
    /* String 2: Product - "IPRO7 UVC Cam" (13 chars) */
    0x1C,                       /* bLength = 2 + 13*2 */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'I', 0x00, 'P', 0x00, 'R', 0x00, 'O', 0x00,
    '7', 0x00, ' ', 0x00, 'U', 0x00, 'V', 0x00,
    'C', 0x00, ' ', 0x00, 'C', 0x00, 'a', 0x00,
    'm', 0x00,
    /* String 3: Serial */
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00, '0', 0x00, '2', 0x00, '5', 0x00,
    '0', 0x00, '1', 0x00, '0', 0x00, '1', 0x00,
    '0', 0x00, '0', 0x00,
    /* Terminator */
    0x00
};

/* ============================================================================
 * USB Event Handler (called by CherryUSB stack)
 * ============================================================================ */

void usbd_event_handler(uint8_t event)
{
    (void)event;
    /* No special handling needed for UVC-only mode */
}

/* ============================================================================
 * UVC Callbacks
 * ============================================================================ */

static volatile bool tx_flag = 0;
static volatile bool iso_tx_busy = false;

void usbd_video_open(uint8_t intf)
{
    (void)intf;
    tx_flag = 1;
    iso_tx_busy = false;
}

void usbd_video_close(uint8_t intf)
{
    (void)intf;
    tx_flag = 0;
    iso_tx_busy = false;
}

void usbd_video_iso_callback(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    (void)nbytes;
    iso_tx_busy = false;
}

static struct usbd_endpoint video_in_ep = {
    .ep_cb = usbd_video_iso_callback,
    .ep_addr = VIDEO_IN_EP
};

/* ============================================================================
 * DMA for USB EP1 TX
 * ============================================================================ */

static SemaphoreHandle_t dma_complete_sem = NULL;
static uint32_t current_transfer_len = 0;

static void dma_usb_callback(void *dev, uint32_t event)
{
    (void)dev;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (event == HAL_DMA_INT_COMPLETE) {
        xSemaphoreGiveFromISR(dma_complete_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static dma_device_t usb_dma_dev = {
    .id = DMA0_ID,
    .ch = DMA0_CH1_INDEX,
    .transfer_mode = DMA_LLI_ONCE_MODE,
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_USB_EP1,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR16,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_8BIT,
    .dst_width = DMA_TRANSFER_WIDTH_8BIT,
    .lli_cfg = NULL,
    .callback = dma_usb_callback,
};

static void dma_usb_init(void)
{
    if (dma_complete_sem == NULL)
        dma_complete_sem = xSemaphoreCreateBinary();

    dma_setConfig(&usb_dma_dev);
    dma_init(DMA0_ID, DMA0_CH1_INDEX);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_CALLBACK, dma_usb_callback);
    usbd_ep_enable_dma_tx(VIDEO_IN_EP);
}

static int dma_usb_write(const uint8_t *data, uint32_t len)
{
    int ret;

    current_transfer_len = len;
    L1C_DCACHE_CLEAN_RANGE((unsigned long)data, len);

    ret = dma_reload(DMA0_ID, DMA0_CH1_INDEX,
                     (uint32_t)(uintptr_t)data,
                     usbd_ep_get_tx_fifo_addr(VIDEO_IN_EP),
                     len, NULL);
    if (ret != 0)
        return ret;

    dma_channel_start(DMA0_ID, DMA0_CH1_INDEX);
    usbd_ep_trigger_ready(VIDEO_IN_EP);

    if (xSemaphoreTake(dma_complete_sem, portMAX_DELAY) == pdTRUE) {
        usbd_event_ep_in_complete_handler(VIDEO_IN_EP | 0x80, current_transfer_len);
        return 0;
    }

    return -1;
}

/* ============================================================================
 * UVC Public API
 * ============================================================================ */

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static uint8_t *uvc_packet_buffer = NULL;

/* 1-based frame descriptor index the host committed (1 = WIDTH x HEIGHT). */
uint8_t uvc_frame_index(void)
{
    return usbd_video_get_frame_index();
}

void uvc_init(void)
{
    usbd_video_set_num_frames(UVC_NUM_FRAMES);
    usbd_desc_register(video_descriptor);
    usbd_add_interface(usbd_video_init_intf(&intf0, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_interface(usbd_video_init_intf(&intf1, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_endpoint(&video_in_ep);

    usbd_initialize();
    dma_usb_init();

    if (uvc_packet_buffer == NULL) {
        uvc_packet_buffer = pvPortMalloc(UVC_PACKET_BUF_SIZE);
        if (uvc_packet_buffer)
            memset(uvc_packet_buffer, 0, UVC_PACKET_BUF_SIZE);
        else
            printf("[UVC] ERROR: packet buffer malloc failed (%d bytes)\r\n", UVC_PACKET_BUF_SIZE);
    }
}

static uint32_t staged_len = 0;

/*
 * Stage one MJPEG frame: pack the UVC payloads into uvc_packet_buffer. Once
 * this returns 1 the *source* frame has been fully copied out and can be
 * released (so the MJPEG ring can be drained and the encoder keeps running)
 * before the blocking USB push. Returns 0 (nothing staged) if the host has
 * not opened the stream or a transfer is still in flight.
 */
int uvc_frame_fill(uint32_t addr, uint32_t size)
{
    if (!tx_flag || iso_tx_busy || !uvc_packet_buffer)
        return 0;

    /* payload_fill adds ~1.2% UVC-header overhead; drop frames that would not
     * fit the staging buffer (can happen at very high quality / busy scenes)
     * rather than overflowing it. */
    if (size + (size / 64) + 1024 > UVC_PACKET_BUF_SIZE) {
        printf("[UVC] frame too large (%lu B), dropped\r\n", (unsigned long)size);
        return 0;
    }

    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)addr, (unsigned long)size);
    usbd_video_mjpeg_payload_fill((uint8_t *)addr, size, uvc_packet_buffer, &staged_len);
    return 1;
}

/*
 * Push the staged frame over the iso endpoint. Blocks until the DMA completes,
 * but the MJPEG encoder runs in parallel because the ring frame was already
 * released after uvc_frame_fill(). This overlap is what lifts the rate from
 * ~15fps (serialized encode+send) toward the Full-Speed ceiling.
 */
void uvc_frame_flush(void)
{
    iso_tx_busy = true;
    dma_usb_write(uvc_packet_buffer, staged_len);
}
