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

#include <generated/autoconf.h>
#include "ipro_vendor_ids.h"
#include "drv_glb.h"
#include "usbd_core.h"
#include "usbd_video.h"
#include "pic_data.h"
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"
#include "hal_common.h"

#include "hal_dma.h"
#include "semphr.h"
#include "usbd_event_common.h"
#include "compiler/common.h"

#define VIDEO_IN_EP 0x81

static SemaphoreHandle_t dma_complete_sem = NULL;
static uint32_t current_transfer_len = 0;
static struct usbd_interface intf0;
static struct usbd_interface intf1;

ATTR_PSRAM_SECTION USB_MEM_ALIGNX uint8_t packet_buffer[10 * 1024];

#ifdef CONFIG_MM_SUPPORT
#ifdef CONFIG_LINK_FLASH
static uint8_t *mm_packet_buffer = 0;
static uint32_t mm_packet_buffer_size = 256*1024;
#else
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t mm_packet_buffer[256 * 1024];
#endif
#endif

#define MAX_PAYLOAD_SIZE  1020
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))

#define WIDTH  (unsigned int)(640)
#define HEIGHT (unsigned int)(480)

#define CAM_FPS        (30)
#define INTERVAL       (unsigned long)(10000000 / CAM_FPS)
#define MIN_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS) //16 bit
#define MAX_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS)
#define MAX_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 2)

#define USB_VIDEO_DESC_SIZ (unsigned long)(9 +  \
                                           8 +  \
                                           9 +  \
                                           13 + \
                                           18 + \
                                           9 +  \
                                           12 + \
                                           9 +  \
                                           14 + \
                                           11 + \
                                           30 + \
                                           9 +  \
                                           7)

#define VC_TERMINAL_SIZ (unsigned int)(13 + 18 + 12 + 9)
#define VS_HEADER_SIZ   (unsigned int)(13 + 1 + 11 + 30)

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

const uint8_t video_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    VIDEO_VC_DESCRIPTOR_INIT(0x00, 0, 0x0100, VC_TERMINAL_SIZ, 48000000, 0x02),
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x00),
    VIDEO_VS_HEADER_DESCRIPTOR_INIT(0x01, VS_HEADER_SIZ, VIDEO_IN_EP, 0x00),
    VIDEO_VS_FORMAT_MJPEG_DESCRIPTOR_INIT(0x01, 0x01),
    VIDEO_VS_FRAME_MJPEG_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, MAX_FRAME_SIZE, DBVAL(INTERVAL), 0x01, DBVAL(INTERVAL)),
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x01, 0x01),
    /* 1.2.2.2 Standard VideoStream Isochronous Video Data Endpoint Descriptor */
    0x07,                         /* bLength */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: ENDPOINT */
    0x81,                         /* bEndpointAddress: IN endpoint 2 */
    0x01,                         /* bmAttributes: Isochronous transfer type. Asynchronous synchronization type. */
    WBVAL(VIDEO_PACKET_SIZE),     /* wMaxPacketSize */
    0x01,                         /* bInterval: One frame interval */

    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
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
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'U', 0x00,                  /* wcChar10 */
    'V', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '1', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '3', 0x00,                  /* wcChar5 */
    '1', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '0', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

static void uvc_event_handler(uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

static volatile bool tx_flag = 0;
static volatile bool iso_tx_busy = false;

void usbd_video_open(uint8_t intf)
{
    tx_flag = 1;
    USB_LOG_RAW("OPEN\r\n");
    iso_tx_busy = false;
}
void usbd_video_close(uint8_t intf)
{
    USB_LOG_RAW("CLOSE\r\n");
    tx_flag = 0;
    iso_tx_busy = false;
}

void usbd_video_iso_callback(uint8_t ep, uint32_t nbytes)
{
    //USB_LOG_RAW("actual in len:%ld\r\n", nbytes);
    iso_tx_busy = false;
}

static struct usbd_endpoint video_in_ep = {
    .ep_cb = usbd_video_iso_callback,
    .ep_addr = VIDEO_IN_EP
};

static void dma_usb_callback(void *dev, uint32_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (event == HAL_DMA_INT_COMPLETE) {
        /* Give semaphore from ISR */
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
    /* Create semaphore for DMA completion */
    if (dma_complete_sem == NULL) {
        dma_complete_sem = xSemaphoreCreateBinary();
    }

    /* Configure DMA0_CH1 for USB EP1 TX using dma_setConfig */
    dma_setConfig(&usb_dma_dev);
    dma_init(DMA0_ID, DMA0_CH1_INDEX);

    /* Enable DMA interrupt */
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_CALLBACK, dma_usb_callback);

    /* Enable DMA TX for EP1 in USB controller */
    usbd_ep_enable_dma_tx(VIDEO_IN_EP);
}

static int dma_usb_write(const uint8_t *data, uint32_t len)
{
    int ret;

    current_transfer_len = len;

    /* Clean cache for DMA */
    L1C_DCACHE_CLEAN_RANGE((unsigned long)data, len);

    /* Reload and start DMA */
    ret = dma_reload(DMA0_ID, DMA0_CH1_INDEX, (uint32_t)(uintptr_t)data, usbd_ep_get_tx_fifo_addr(VIDEO_IN_EP), len, NULL);
    if (ret != 0)
        return ret;

    dma_channel_start(DMA0_ID, DMA0_CH1_INDEX);

    /* Set EP1 Ready to trigger USB transmission (which pulls from DMA) */
    usbd_ep_trigger_ready(VIDEO_IN_EP);

    /* Wait for DMA completion via semaphore */
    if (xSemaphoreTake(dma_complete_sem, portMAX_DELAY) == pdTRUE) {
        /* Notify stack - Note: Isoc (UVC) endpoints don't generate EP1_DONE interrupt,
         * so we must manually call the completion handler */
        usbd_event_ep_in_complete_handler(VIDEO_IN_EP | 0x80, current_transfer_len);
        return 0;
    } else {
        /* Timeout */
        return -1;
    }
}

void vdo_start_proc(void *arg)
{
    uint32_t out_len;
    //int ret;

    dma_usb_init();

    memset(packet_buffer, 0, 10 * 1024);

    while (1) {
        if (tx_flag) {
            usbd_video_mjpeg_payload_fill((uint8_t *)jpeg_data, sizeof(jpeg_data), packet_buffer, &out_len);
            L1C_DCACHE_CLEAN_RANGE((unsigned long)packet_buffer, out_len);
            iso_tx_busy = true;

            // ret = usbd_ep_start_write(VIDEO_IN_EP, packet_buffer, out_len);
            dma_usb_write(packet_buffer, out_len);

            //printf("packet_buffer: %p, out_len = %ld, ret = %d\r\n", packet_buffer, out_len, ret);

            while (iso_tx_busy) {
                if (tx_flag == 0) {
                    break;
                }
            }
        }
    }
    vTaskDelete(NULL);
}

static struct usbd_interface intf0;
static struct usbd_interface intf1;

int cmd_vdo_init(int argc, char **argv)
{
    usbd_deinitialize();

    usbd_desc_register(video_descriptor);
    usbd_add_interface(usbd_video_init_intf(&intf0, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_interface(usbd_video_init_intf(&intf1, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_endpoint(&video_in_ep);
    usbd_event_register_callback(uvc_event_handler);

    usbd_initialize();
    xTaskCreate(vdo_start_proc, "uvc", 1024, NULL, 10, NULL);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vdo_init, usbd_vdo_init, USBD MJPEG Test);

#ifdef CONFIG_MM_SUPPORT
void stream_send_jpg(uint32_t addr, uint32_t size)
{
    uint32_t out_len;

    if (tx_flag && !iso_tx_busy) {
        L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)addr, (unsigned long)size);
        usbd_video_mjpeg_payload_fill((uint8_t *)addr, size, mm_packet_buffer, &out_len);
        iso_tx_busy = true;

        dma_usb_write(mm_packet_buffer, out_len);
    }
}

void stream_uvc_init(void)
{
    usbd_desc_register(video_descriptor);
    usbd_add_interface(usbd_video_init_intf(&intf0, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_interface(usbd_video_init_intf(&intf1, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_endpoint(&video_in_ep);

    usbd_initialize();

    dma_usb_init();

#ifdef CONFIG_LINK_FLASH // in flash, bss section in ocram and this buffer would overflow, use malloc
    mm_packet_buffer = pvPortMalloc(mm_packet_buffer_size);
#endif
    memset(mm_packet_buffer, 0, 256 * 1024);
}
#endif
