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

#include "usb.h"
#include "ipro_vendor_ids.h"
#include "txrx.h"
#include "usb_cdc.h"
#include "usbwifi.h"
#include "utils.h"

#include <assert.h>
#include <stdio.h>
#include <usbd_core.h>
#include <usbd_cdc.h>
#include <utils_list.h>

#if defined(IPRO6LE)
#include "ipro6le.h"
extern void Interrupt_Handler_Register(IRQn_Type irq, void (*interruptFun)(void));
extern void CPU_Interrupt_Enable(uint32_t irq_num);
#endif

/********************************************************************************
 *  Imports
 ********************************************************************************/
#if defined(IPRO6LE)
extern void USBD_IRQHandler(void);
#else
extern void USBD_IRQHandler(int irq, void *arg);
#endif

/********************************************************************************
 *  Private definations
 ********************************************************************************/
#define USBD_VID        IPRO_USB_VID
#define USBD_PID        0x616F
#define USBD_BCD_DEVICE 0x0100
#define USBD_MAX_POWER  255
#define USBD_LANGID_STRING 1033

#define WIFI_IN_EP  0x81
#define WIFI_OUT_EP 0x02
#define CDC_IN_EP   0x83
#define CDC_OUT_EP  0x04
#define CDC_INT_EP  0x85

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

#if defined(IPRO6)
/* IPRO6 USB has 4 FIFO blocks = exactly one double-buffered bulk pair, so only
 * the WiFi IN/OUT endpoints fit. Drop the CDC-ACM side-channel: its bulk
 * endpoints would steal the WiFi OUT FIFO pair, and single-buffering the WiFi
 * bulk underruns/NAKs (see drv_usb_cherryusb.c usbd_ep_open). */
#define USB_CONFIG_SIZE  (9 + 8 + 9 + 7 + 7)
#define USBWIFI_NUM_INTF 0x01
#else
#define USB_CONFIG_SIZE  (9 + 8 + 9 + 7 + 7 + CDC_ACM_DESCRIPTOR_LEN)
#define USBWIFI_NUM_INTF 0x03
#endif

#define wC(ch) ch, 0

static const uint8_t usb_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, USBD_BCD_DEVICE, 1),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, USBWIFI_NUM_INTF, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    // IAD
    0x08, USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION, 0x00, 0x01, 0xff, 0xff, 0xff, 0x00,

    // Interface Descriptor
    0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0x02,
    // Endpoint 1 Descriptor
    0x07, 0x05, WIFI_IN_EP, 0x02, 0x00, 0x02, 0x00,
    // Endpoint 2 Descriptor
    0x07, 0x05, WIFI_OUT_EP, 0x02, 0x00, 0x02, 0x00,

#if !defined(IPRO6)
    CDC_ACM_DESCRIPTOR_INIT(0x01, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
#endif
    // string0 descriptor
    USB_LANGID_INIT(USBD_LANGID_STRING),
    // string1 descriptor
    0x1a,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    wC('B'), wC('o'), wC('u'), wC('f'), wC('f'), wC('a'), wC('l'), wC('o'), wC(' '), wC('L'), wC('a'), wC('b'),
    // string2 descriptor
    0x12,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    wC('I'), wC('o'), wC('T'), wC(' '), wC('W'), wC('i'), wC('F'), wC('i'),
    // string3 descriptor
    0x0a,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    wC('0'), wC('0'), wC('0'), wC('0'),
    // device qualifier descriptor
    0x0a, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, 0x00, 0x02, 0x02, 0x02, 0x01, 0x40, 0x01, 0x00,

    0x00
};

static void usb_worker(void *pvParameters)
{
    while (1) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        usbwifi_usb_data_process();
    }
}

static inline void attach_bulk_out_buf(struct usbwifi_tx_env_tag *tx_env, tx_buf *buf)
{
    void *b = TX_BUF_GET_USB_BUF(buf);
    assert((uintptr_t)b % CONFIG_USB_ALIGN_SIZE == 0);
    tx_env->curr_attached = buf;
    usbd_ep_start_read(WIFI_OUT_EP, b, 2048);
}

static void alloc_attach_bulk_out_buf(struct usbwifi_tx_env_tag *tx_env)
{
    tx_buf *buf = usbwifi_tx_buf_alloc(tx_env, true);
    if (buf) {
        attach_bulk_out_buf(tx_env, buf);
    }
}

bool usb_try_reattach_bulk_out_buf(struct usbwifi_tx_env_tag *tx_env, tx_buf *buf)
{
    if (tx_env->curr_attached) {
        return false;
    }
    usbwifi_tx_buf_reset(buf);
    attach_bulk_out_buf(tx_env, buf);
    return true;
}

static void usbd_configure_done_impl(void)
{
    struct usbwifi_tx_env_tag *tx_env = &g_usbwifi.tx_env;

    printf("[USB] configure done\r\n");
    /* setup first out ep read transfer */
    if (tx_env->curr_attached == NULL) {
        alloc_attach_bulk_out_buf(tx_env);
    } else {
        attach_bulk_out_buf(tx_env, tx_env->curr_attached);
    }

    usbwifi_configured();
}

static void wifi_bulk_out(uint8_t ep, uint32_t nbytes)
{
    usbwifi_t *uw = &g_usbwifi;
    struct usbwifi_tx_env_tag *tx_env = &uw->tx_env;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    assert(tx_env->curr_attached != NULL);
    utils_list_push_back(&tx_env->ready_list, tx_env->curr_attached);

    tx_env->curr_attached = NULL;
    alloc_attach_bulk_out_buf(tx_env);

    vTaskNotifyGiveFromISR(uw->usb_worker_task, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

extern void dsd_usb_drained_inc(void);
extern void dsd_usb_complete_inc(void);
extern uint32_t inline_hal_machw_time(void);

static void wifi_bulk_in(uint8_t ep, uint32_t nbytes)
{
    usbwifi_t *uw = &g_usbwifi;
    struct usbwifi_rx_env_tag *env = &uw->rx_env;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        usbd_ep_start_write(WIFI_IN_EP, NULL, 0);
    } else {
        dsd_usb_complete_inc();
        usb_data_emb_t *ud = (usb_data_emb_t *)env->curr_attached;
        assert(ud != NULL);
        /* Counter J: timestamp at completion ISR. ud lives in uncached
         * MAWOC ram_wifi so write is visible immediately to usb_worker
         * task on the same core. */
        {
            uint32_t now = inline_hal_machw_time();
            ud->complete_us = now ? now : 1;
        }
        struct utils_list_hdr *hdr = (struct utils_list_hdr *)&ud->list_hdr;
        utils_list_push_back(&env->pendding_free_list, hdr);

        hdr = utils_list_pop_front(&env->pendding_tx_list);
        if (hdr) {
            dsd_usb_drained_inc();
            ud = container_of((uint32_t *)hdr, usb_data_emb_t, list_hdr);
            usb_bulk_in(ud, ud->tx_len);
        } else {
            env->curr_attached = NULL;
        }

        vTaskNotifyGiveFromISR(uw->usb_worker_task, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void usb_bulk_in(const void *data, size_t len)
{
    struct usbwifi_rx_env_tag *env = &g_usbwifi.rx_env;

    if (len % CDC_MAX_MPS == 0) {
        ++len;
    }
    env->curr_attached = data;
    usbd_ep_start_write(WIFI_IN_EP, data, len);
}

void usb_cdc_bulk_out(struct usbwifi_cdc_env_tag *env, void *data, size_t len)
{
    assert((uintptr_t)data % CONFIG_USB_ALIGN_SIZE == 0);
    env->bulk_out_buf_attached = data;
    usbd_ep_start_read(CDC_OUT_EP, data, len);
}

void usb_cdc_bulk_in(struct usbwifi_cdc_env_tag *env, const void *data, size_t len)
{
    assert((uintptr_t)data % CONFIG_USB_ALIGN_SIZE == 0);
    env->bulk_in_buf_attached = data;
    usbd_ep_start_write(CDC_IN_EP, data, len);
}

#if !defined(IPRO6)   /* CDC-ACM side-channel dropped on IPRO6 (no spare USB FIFO) */
static void cdc_bulk_out(uint8_t ep, uint32_t nbytes)
{
    struct usbwifi_cdc_env_tag *env = &g_usbwifi.cdc_env;

    env->bulk_out_buf_attached = NULL;

    if (env->bulk_out_cb) {
        env->bulk_out_cb(env->bulk_out_cb_arg, nbytes);
    }
}

static void cdc_bulk_in(uint8_t ep, uint32_t nbytes)
{
    struct usbwifi_cdc_env_tag *env = &g_usbwifi.cdc_env;

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        usbd_ep_start_write(WIFI_IN_EP, NULL, 0);
    } else {
        if (env->bulk_in_cb) {
            env->bulk_in_cb(env->bulk_in_cb_arg, nbytes);
        }
    }
}
#endif

void usbd_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        usbd_configure_done_impl();
        break;
    case USBD_EVENT_SUSPEND:
    case USBD_EVENT_DISCONNECTED:
        usbwifi_disconnected();
        break;
    default:
        break;
    }
}

static struct usbd_endpoint wifi_out_ep = {
    .ep_addr = WIFI_OUT_EP,
    .ep_cb = wifi_bulk_out
};
static struct usbd_endpoint wifi_in_ep = {
    .ep_addr = WIFI_IN_EP,
    .ep_cb = wifi_bulk_in
};

#if !defined(IPRO6)   /* CDC-ACM side-channel dropped on IPRO6 (no spare USB FIFO) */
static struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = cdc_bulk_out
};
static struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = cdc_bulk_in
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;
#endif

int usbwifi_usb_init()
{
    usbwifi_t *uw = &g_usbwifi;

#if defined(IPRO6)
    /* IPRO6: the CherryUSB device port registers + enables USB_IRQn inside
     * usb_dc_init() (called by usbd_initialize() below); do not double-register. */
#elif defined(IPRO6LE)
    Interrupt_Handler_Register(USB_IRQn, USBD_IRQHandler);
    CPU_Interrupt_Enable(USB_IRQn);
#else
    bl_irq_register(37, USBD_IRQHandler);
#endif

    xTaskCreate(usb_worker, "usb_worker", 512, NULL, 31, &uw->usb_worker_task);

    usbd_desc_register(usb_descriptor);

#if !defined(IPRO6)
    /* CDC-ACM side-channel: dropped on IPRO6 (only 4 USB FIFO blocks = one bulk
     * pair, reserved for the WiFi endpoints). */
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);
#endif

    usbd_add_endpoint(&wifi_out_ep);
    usbd_add_endpoint(&wifi_in_ep);

    usbd_initialize();

    return 0;
}
