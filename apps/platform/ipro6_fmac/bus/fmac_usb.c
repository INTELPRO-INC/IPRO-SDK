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

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <usbd_core.h>

#include "drv_glb.h"
#include "drv_pds.h"

#include "ipro_irq.h"
#include "ipro_vendor_ids.h"
#include "fmac_dl.h"
#include "fmac_usb.h"

#define IN_EP       0x81
#define OUT_EP      0x02
#define MPS         512
/* OUT buffers; with one, nothing is armed while a request is held */
#ifndef FMAC_USB_NBUF
#define FMAC_USB_NBUF 2
#endif
#define NBUF        FMAC_USB_NBUF
/* Largest IN transfer */
#ifndef FMAC_USB_IN_MAX
#define FMAC_USB_IN_MAX FMAC_DL_BLK
#endif
#define IN_LEN      FMAC_USB_IN_MAX
#define PID_OFS     10
#define wC(ch)      ch, 0
/* Bus reset seen but no SET_CONFIGURATION this long: attach again */
#define ENUM_TMO_MS 1000u
/* Detach time of a retry */
#define RETRY_DETACH_MS 1000u
#define CFG_LEN     (9 + 9 + 7 + 7)

/* drv_usb_cherryusb.c */
extern uint8_t g_setup_buffer[8];
extern void USBD_IRQHandler(void);

static uint8_t s_desc[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, IPRO_USB_VID,
                               FMAC_USB_PID, 0x0100, 1),
    USB_CONFIG_DESCRIPTOR_INIT(CFG_LEN, 0x01, 0x01,
                               USB_CONFIG_BUS_POWERED, 250),
    0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0x02,
    0x07, 0x05, IN_EP, 0x02, MPS & 0xFF, MPS >> 8, 0x00,
    0x07, 0x05, OUT_EP, 0x02, MPS & 0xFF, MPS >> 8, 0x00,
    USB_LANGID_INIT(1033),
    0x1C, USB_DESCRIPTOR_TYPE_STRING,
    wC('I'), wC('N'), wC('T'), wC('E'), wC('L'), wC('P'), wC('R'),
    wC('O'), wC(' '), wC('I'), wC('N'), wC('C'), wC('.'),
    0x18, USB_DESCRIPTOR_TYPE_STRING,
    wC('I'), wC('P'), wC('R'), wC('O'), wC('6'), wC('S'), wC(' '),
    wC('F'), wC('M'), wC('A'), wC('C'),
    0x0A, USB_DESCRIPTOR_TYPE_STRING,
    wC('0'), wC('0'), wC('0'), wC('0'),
    0x0A, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x40, 0x01, 0x00,
    0x00
};

#ifdef FMAC_BUS_POOL
#include "fmac_bus_pool.h"
#define s_buf   (fmac_dnld_pool.usb)
#define s_in    (fmac_upld_pool.usb)
#else
static USB_NOCACHE_RAM_SECTION uint8_t
    s_buf[NBUF][FMAC_DL_PKT_MAX] __attribute__((aligned(32)));
/* + 4: a whole-packet transfer gets 4 zero bytes, so it ends short */
static USB_NOCACHE_RAM_SECTION uint8_t
    s_in[IN_LEN + 4] __attribute__((aligned(32)));
#endif
static volatile uint8_t s_ring[NBUF];
static volatile uint32_t s_head, s_tail;
static volatile uint32_t s_len[NBUF];
/* OUT buffer armed: index, or -1 */
static volatile int s_armed = -1;
/* Bit i: buffer i is free for the OUT endpoint */
static volatile uint32_t s_free;
#ifndef FMAC_USB_IN_NQ
#define FMAC_USB_IN_NQ 1
#endif
/* The IN buffer is split into INQ buffers, each IB_LEN bytes plus 4 pad
 * bytes: the next one starts from the completion of the previous one */
#define INQ         FMAC_USB_IN_NQ
#define IB_LEN      ((IN_LEN + 4) / INQ - 4)
/* Bit i: IN buffer i is queued or in flight; s_iq holds them in order */
static volatile uint32_t s_ibusy;
static volatile uint8_t s_iq[INQ];
static volatile uint32_t s_iq_head, s_iq_tail;
static uint32_t s_ilen[INQ];
static volatile int s_configured;
static volatile int s_reset_seen;
static int s_inited;
/* 0 detached, 1 waiting to attach, 2 attached */
static int s_state;
static TickType_t s_at;
static uint32_t s_retries;
static volatile uint32_t s_evt[USBD_EVENT_UNKNOWN + 1];
static void (*s_notify)(void);

void fmac_usb_set_notify(void (*fn)(void))
{
    s_notify = fn;
}

/* Arms the OUT endpoint with a free buffer when none is armed */
static void arm(void)
{
    uintptr_t flag = ipro_irq_save();

    if (s_configured && s_armed < 0 && s_free) {
        int i = __builtin_ctz(s_free);

        s_free &= ~(1u << i);
        s_armed = i;
        usbd_ep_start_read(OUT_EP, s_buf[i], FMAC_DL_PKT_MAX);
    }
    ipro_irq_restore(flag);
}

/* IRQ context */
static void out_cb(uint8_t ep, uint32_t nbytes)
{
    int i = s_armed;

    (void)ep;
    s_armed = -1;
    if (i >= 0) {
        s_len[i] = nbytes;
        s_ring[s_head % NBUF] = (uint8_t)i;
        s_head++;
    }
    arm();
    if (s_notify)
        s_notify();
}

static uint8_t *ibuf(int i)
{
    return s_in + i * (IB_LEN + 4);
}

static int ibuf_index(const void *p)
{
    for (int i = 0; i < INQ; i++)
        if (p == ibuf(i))
            return i;
    return -1;
}

static int ibuf_free(void)
{
    for (int i = 0; i < INQ; i++)
        if (!(s_ibusy & (1u << i)))
            return i;
    return -1;
}

/* IRQ context */
static void in_cb(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    (void)nbytes;
    if (s_iq_tail != s_iq_head) {
        s_ibusy &= ~(1u << s_iq[s_iq_tail % INQ]);
        s_iq_tail++;
        if (s_iq_tail != s_iq_head) {
            int j = s_iq[s_iq_tail % INQ];

            usbd_ep_start_write(IN_EP, ibuf(j), s_ilen[j]);
        }
    }
    if (s_notify)
        s_notify();
}

static struct usbd_endpoint s_out_ep = { .ep_addr = OUT_EP, .ep_cb = out_cb };
static struct usbd_endpoint s_in_ep = { .ep_addr = IN_EP, .ep_cb = in_cb };

void usbd_event_handler(uint8_t event)
{
    if (event <= USBD_EVENT_UNKNOWN)
        s_evt[event]++;
    switch (event) {
    case USBD_EVENT_RESET:
        s_reset_seen = 1;
        __attribute__((fallthrough));
    case USBD_EVENT_DISCONNECTED:
        s_configured = 0;
        s_armed = -1;
        s_free = (1u << NBUF) - 1u;
        s_head = s_tail = 0;
        s_ibusy = 0;
        s_iq_tail = s_iq_head;
        break;
    case USBD_EVENT_CONFIGURED:
        s_configured = 1;
        arm();
        break;
    default:
        break;
    }
}

/* PHY off and a GLB reset of the USB block, then attach after
 * detach_ms from fmac_usb_poll() */
static void detach_reset(uint32_t detach_ms)
{
    PDS_Turn_Off_USB();
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_USB);
    s_state = 1;
    s_at = xTaskGetTickCount() + pdMS_TO_TICKS(detach_ms);
}

void fmac_usb_detach(void)
{
    CPU_Interrupt_Disable(USB_IRQn);
    s_configured = 0;
    PDS_Turn_Off_USB();
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_USB);
}

static void register_desc(uint16_t pid)
{
    s_desc[PID_OFS] = pid & 0xFF;
    s_desc[PID_OFS + 1] = pid >> 8;
    s_free = (1u << NBUF) - 1u;
    usbd_desc_register(s_desc);
    usbd_add_endpoint(&s_out_ep);
    usbd_add_endpoint(&s_in_ep);
}

void fmac_usb_init(uint16_t pid, uint32_t detach_ms)
{
    register_desc(pid);
    detach_reset(detach_ms);
}

void fmac_usb_takeover(uint16_t pid)
{
    struct usbd_endpoint_cfg out = { .ep_addr = OUT_EP,
                                     .ep_type = USB_ENDPOINT_TYPE_BULK,
                                     .ep_mps = MPS };
    struct usbd_endpoint_cfg in = { .ep_addr = IN_EP,
                                    .ep_type = USB_ENDPOINT_TYPE_BULK,
                                    .ep_mps = MPS };

    register_desc(pid);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU | GLB_AHB_CLOCK_USB11);
    usbd_ep_open(&out);
    usbd_ep_open(&in);
    Interrupt_Handler_Register(USB_IRQn, USBD_IRQHandler);
    s_inited = 1;
    s_state = 2;
    s_configured = 1;
    arm();
    CPU_Interrupt_Enable(USB_IRQn);
}

void fmac_usb_poll(void)
{
    TickType_t now = xTaskGetTickCount();

    if (s_state == 1 && (int32_t)(now - s_at) >= 0) {
        s_reset_seen = 0;
        usbd_initialize();
        s_inited = 1;
        s_state = 2;
    } else if (s_state == 2 && !s_configured) {
        if (!s_reset_seen) {
            s_at = now;
        } else if ((int32_t)(now - s_at) >= (int32_t)pdMS_TO_TICKS(ENUM_TMO_MS)) {
            s_retries++;
            usbd_deinitialize();
            s_inited = 0;
            detach_reset(RETRY_DETACH_MS);
        }
    }
}



int fmac_usb_next(uint8_t **pkt, uint32_t *len)
{
    if (s_tail == s_head)
        return 0;
    int i = s_ring[s_tail % NBUF];
    s_tail++;
    *pkt = s_buf[i];
    *len = s_len[i];
    return 1;
}

uint8_t *fmac_usb_upld_buf(uint32_t *max)
{
    int i = ibuf_free();

    *max = IB_LEN;
    return i >= 0 ? ibuf(i) : NULL;
}

uint32_t fmac_usb_dnld_slots(void)
{
    return NBUF;
}

void fmac_usb_release(uint8_t *pkt)
{
    for (int i = 0; i < NBUF; i++) {
        if (pkt == s_buf[i]) {
            uintptr_t flag = ipro_irq_save();

            s_free |= 1u << i;
            ipro_irq_restore(flag);
        }
    }
    arm();
}

int fmac_usb_send(const void *msg, uint32_t len)
{
    int i = ibuf_index(msg), start;
    uintptr_t flag;

    if (len > IB_LEN || !s_configured)
        return -1;
    if (i < 0) {
        while ((i = ibuf_free()) < 0 && s_configured)
            ;
        if (i < 0)
            return -1;
        memcpy(ibuf(i), msg, len);
    }
    if (len % MPS == 0) {
        memset(ibuf(i) + len, 0, 4);
        len += 4;
    }
    s_ilen[i] = len;
    flag = ipro_irq_save();
    s_ibusy |= 1u << i;
    s_iq[s_iq_head % INQ] = (uint8_t)i;
    s_iq_head++;
    start = s_iq_head - s_iq_tail == 1;
    ipro_irq_restore(flag);
    return start ? usbd_ep_start_write(IN_EP, ibuf(i), len) : 0;
}

int fmac_usb_tx_idle(void)
{
    return ibuf_free() >= 0;
}

int fmac_usb_tx_done(void)
{
    return !s_ibusy;
}

void fmac_usb_diag(const char *tag)
{
    static const uint16_t ofs[] = {
        0x080, 0x0C0, 0x100, 0x104, 0x120, 0x130, 0x134,
        0x13C, 0x140, 0x144, 0x14C, 0x300, 0x308, 0x310, 0x330,
    };

    if (!s_inited || s_configured || !s_reset_seen)
        return;
    printf("[%s] usb retries %lu reset %lu conn %lu disc %lu conf %lu regs",
           tag, (unsigned long)s_retries,
           (unsigned long)s_evt[USBD_EVENT_RESET],
           (unsigned long)s_evt[USBD_EVENT_CONNECTED],
           (unsigned long)s_evt[USBD_EVENT_DISCONNECTED],
           (unsigned long)s_evt[USBD_EVENT_CONFIGURED]);
    for (unsigned i = 0; i < sizeof(ofs) / sizeof(ofs[0]); i++)
        printf(" %03x=%08lx", ofs[i],
               (unsigned long)*(volatile uint32_t *)(0x30085000u + ofs[i]));
    printf(" setup");
    for (unsigned i = 0; i < sizeof(g_setup_buffer); i++)
        printf(" %02x", g_setup_buffer[i]);
    memset((void *)g_setup_buffer, 0, sizeof(g_setup_buffer));
    printf("\r\n");
}
