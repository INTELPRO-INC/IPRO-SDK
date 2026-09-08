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
 * @file composite_uav_ncm_template.c
 * @brief UAC (mic-only) + CDC-NCM composite USB device
 *
 * Composite device with microphone input (AUADC -> USB) and CDC-NCM
 * network adapter using IAD (Interface Association Descriptors).
 *
 * Endpoint allocation:
 *   EP1 IN  (0x81) - UAC mic    (isoc, DMA0_CH1)
 *   EP2 IN  (0x82) - NCM notify (interrupt)
 *   EP3 OUT (0x03) - NCM data   (bulk)
 *   EP4 IN  (0x84) - NCM data   (bulk)
 *
 * Interface layout (4 of 8 max):
 *   0: Audio Control          (IAD 0-1)
 *   1: Audio Streaming IN     (mic)
 *   2: CDC NCM Communication  (IAD 2-3)
 *   3: CDC NCM Data
 */

#include <string.h>
#include "ipro_vendor_ids.h"
#include <generated/autoconf.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#include <stream_buffer.h>

#include "shell.h"
#include "hal_common.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_mtimer.h"
#include "hal_auadc.h"
#include "drv_glb.h"
#include "compiler/common.h"

#include "usbd_core.h"
#include "usbd_audio.h"
#include "usbd_cdc_ncm.h"
#include "usbd_event_common.h"

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "netif/etharp.h"

/* ================================================================== */
/* USB descriptor constants                                           */
/* ================================================================== */
#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0701
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/* --- Endpoint addresses --- */
#define AUDIO_IN_EP    0x81  /* EP1 IN  - mic isoc */
#define NCM_INT_EP     0x82  /* EP2 IN  - NCM notify interrupt */
#define NCM_OUT_EP     0x03  /* EP3 OUT - NCM bulk data */
#define NCM_IN_EP      0x84  /* EP4 IN  - NCM bulk data */

/* --- Audio config (mic only) --- */
#define AUDIO_IN_FU_ID 0x02

#define AUDIO_MIC_FREQ                16000U
#define AUDIO_MIC_FRAME_SIZE_BYTE     2u
#define AUDIO_MIC_RESOLUTION_BIT      16u

/* 16bit(2 Bytes) stereo(2ch) per 1ms frame */
#define AUDIO_IN_PACKET \
    ((uint32_t)((AUDIO_MIC_FREQ * AUDIO_MIC_FRAME_SIZE_BYTE * 2) / 1000))

#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

/* --- NCM config --- */
#define NCM_MPS 64

/* ================================================================== */
/* Descriptor size calculations                                       */
/* ================================================================== */

/* AC header total: header(1 streaming) + mic IT + mic FU + streaming OT */
#define AUDIO_AC_SIZ (AUDIO_SIZEOF_AC_HEADER_DESC(1) +          \
                      AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                      AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                      AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

/* Audio function: IAD + AC intf + AC header + terminals + 1x AS (mic) */
#define AUDIO_FUNC_DESC_SIZ (AUDIO_AC_DESCRIPTOR_INIT_LEN(1) +         \
                             AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                             AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                             AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC +    \
                             AUDIO_AS_DESCRIPTOR_INIT_LEN(1))

/* NCM function: IAD(8) + Comm intf(9) + CDC Header(5) + CDC Union(5) +
 *   CDC ETH(13) + CDC NCM(6) + INT EP(7) + Data intf alt0(9) +
 *   Data intf alt1(9) + OUT EP(7) + IN EP(7) = 85 */
#define NCM_FUNC_DESC_SIZ 85

#define USB_COMPOSITE_CONFIG_DESC_SIZ \
    (9 + AUDIO_FUNC_DESC_SIZ + NCM_FUNC_DESC_SIZ)

/* ================================================================== */
/* Composite USB descriptor                                           */
/* ================================================================== */
static const uint8_t composite_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01,
                               USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_COMPOSITE_CONFIG_DESC_SIZ, 0x04, 0x01,
                               USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /* ============================================================== */
    /* UAC: Audio Control (intf 0) + Streaming IN (intf 1, mic only)  */
    /* ============================================================== */
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, 0x00, 0x01),
    /* Mic path: IT(mic) -> FU -> OT(streaming) */
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC, 0x02, 0x0003),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(0x02, 0x01, 0x01, 0x03, 0x00, 0x00),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING, 0x02),
    /* AS IN (mic): intf 1, linked to OT 0x03 */
    AUDIO_AS_DESCRIPTOR_INIT(0x01, 0x03, 0x02,
                             AUDIO_MIC_FRAME_SIZE_BYTE,
                             AUDIO_MIC_RESOLUTION_BIT,
                             AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET,
                             EP_INTERVAL,
                             AUDIO_SAMPLE_FREQ_3B(AUDIO_MIC_FREQ)),

    /* ============================================================== */
    /* CDC NCM: Communication (intf 2) + Data (intf 3)                */
    /* ============================================================== */
    /* --- IAD (8 bytes) --- */
    0x08,
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,
    0x02,                                       /* bFirstInterface */
    0x02,                                       /* bInterfaceCount */
    USB_DEVICE_CLASS_CDC,                       /* bFunctionClass */
    CDC_NETWORK_CONTROL_MODEL,                  /* bFunctionSubClass */
    CDC_COMMON_PROTOCOL_NONE,                   /* bFunctionProtocol */
    0x00,                                       /* iFunction */
    /* --- Communication Interface (intf 2, 1 EP) --- */
    0x09,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    0x02, 0x00, 0x01,                           /* intf 2, alt 0, 1 EP */
    USB_DEVICE_CLASS_CDC,
    CDC_NETWORK_CONTROL_MODEL,
    CDC_COMMON_PROTOCOL_NONE,
    0x00,                                       /* iInterface */
    /* --- CDC Header --- */
    0x05, CDC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, WBVAL(CDC_V1_10),
    /* --- CDC Union --- */
    0x05, CDC_CS_INTERFACE, CDC_FUNC_DESC_UNION, 0x02, 0x03,
    /* --- CDC Ethernet Networking --- */
    0x0D, CDC_CS_INTERFACE, CDC_FUNC_DESC_ETHERNET_NETWORKING,
    0x04,                                       /* iMACAddress = string 4 */
    DBVAL_BE(0x00000000),                       /* bmEthernetStatistics */
    WBVAL(CONFIG_CDC_NCM_ETH_MAX_SEGSZE),       /* wMaxSegmentSize */
    WBVAL(0x0000),                              /* wNumberMCFilters */
    0x00,                                       /* bNumberPowerFilters */
    /* --- CDC NCM Functional --- */
    0x06, CDC_CS_INTERFACE, CDC_FUNC_DESC_NCM, 0x00, 0x01, 0x00,
    /* --- Interrupt IN EP (EP2 IN) --- */
    0x07, USB_DESCRIPTOR_TYPE_ENDPOINT,
    NCM_INT_EP, 0x03, WBVAL(16), 0x10,
    /* --- Data Interface alt 0 (intf 3, 0 EP, inactive) --- */
    0x09, USB_DESCRIPTOR_TYPE_INTERFACE,
    0x03, 0x00, 0x00,                           /* intf 3, alt 0, 0 EP */
    CDC_DATA_INTERFACE_CLASS, 0x00, 0x01,       /* NTB protocol */
    0x00,
    /* --- Data Interface alt 1 (intf 3, 2 EP, active) --- */
    0x09, USB_DESCRIPTOR_TYPE_INTERFACE,
    0x03, 0x01, 0x02,                           /* intf 3, alt 1, 2 EP */
    CDC_DATA_INTERFACE_CLASS, 0x00, 0x01,       /* NTB protocol */
    0x00,
    /* --- Bulk OUT EP (EP3 OUT) --- */
    0x07, USB_DESCRIPTOR_TYPE_ENDPOINT,
    NCM_OUT_EP, 0x02, WBVAL(NCM_MPS), 0x00,
    /* --- Bulk IN EP (EP4 IN) --- */
    0x07, USB_DESCRIPTOR_TYPE_ENDPOINT,
    NCM_IN_EP, 0x02, WBVAL(NCM_MPS), 0x00,

    /* ============================================================== */
    /* String descriptors                                             */
    /* ============================================================== */
    /* String 0: Language ID */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* String 1: Manufacturer */
    0x14, USB_DESCRIPTOR_TYPE_STRING,
    'C', 0x00, 'h', 0x00, 'e', 0x00, 'r', 0x00,
    'r', 0x00, 'y', 0x00, 'U', 0x00, 'S', 0x00,
    'B', 0x00,
    /* String 2: Product "UAC+NCM DEMO" */
    0x1A, USB_DESCRIPTOR_TYPE_STRING,
    'U', 0x00, 'A', 0x00, 'C', 0x00, '+', 0x00,
    'N', 0x00, 'C', 0x00, 'M', 0x00, ' ', 0x00,
    'D', 0x00, 'E', 0x00, 'M', 0x00, 'O', 0x00,
    /* String 3: Serial Number */
    0x16, USB_DESCRIPTOR_TYPE_STRING,
    '2', 0x00, '0', 0x00, '2', 0x00, '5', 0x00,
    '0', 0x00, '1', 0x00, '0', 0x00, '1', 0x00,
    '0', 0x00, '0', 0x00,
    /* String 4: MAC address "C07415A13D7E" (OUI C0:74:15, host adapter) */
    0x1A, USB_DESCRIPTOR_TYPE_STRING,
    'C', 0x00, '0', 0x00, '7', 0x00, '4', 0x00,
    '1', 0x00, '5', 0x00, 'A', 0x00, '1', 0x00,
    '3', 0x00, 'D', 0x00, '7', 0x00, 'E', 0x00,
#ifdef CONFIG_USB_HS
    /* Device qualifier descriptor */
    0x0A,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00, 0x02,
    0x00, 0x00, 0x00,
    0x40,
    0x01,
    0x00,
#endif
    /* Terminator */
    0x00
};

/* ================================================================== */
/* UAC runtime (mic only)                                             */
/* ================================================================== */
static volatile bool uac_tx_flag;
static volatile bool uac_ep_tx_busy;
static SemaphoreHandle_t uac_dma_sem;
static uint32_t uac_dma_len;

#define AUDIO_SAMPLES_PER_MS    16
#define MONO_FRAME_SIZE_BYTES   (AUDIO_SAMPLES_PER_MS * 2)
#define STEREO_FRAME_SIZE_BYTES (AUDIO_IN_PACKET)

#define ADC_FRAME_SIZE_SAMPLES  320
#define ADC_FRAME_SIZE_BYTES    (ADC_FRAME_SIZE_SAMPLES * 2)

#define UAC_STREAM_BUFFER_SIZE  (MONO_FRAME_SIZE_BYTES * 100)
static StreamBufferHandle_t uac_stream_buffer;

static int16_t *uac_pcm_buf[2];
static int16_t uac_mono_frame[AUDIO_SAMPLES_PER_MS];
static int16_t uac_stereo_frame[AUDIO_SAMPLES_PER_MS * 2];

/* --- UAC DMA (CH1 -> EP1) --- */
static void uac_dma_callback(void *dev, uint32_t event)
{
    BaseType_t woken = pdFALSE;

    if (event == HAL_DMA_INT_COMPLETE) {
        xSemaphoreGiveFromISR(uac_dma_sem, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

static dma_device_t uac_dma_dev = {
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
    .callback = uac_dma_callback,
};

static void uac_dma_init(void)
{
    if (uac_dma_sem == NULL)
        uac_dma_sem = xSemaphoreCreateBinary();

    dma_setConfig(&uac_dma_dev);
    dma_init(DMA0_ID, DMA0_CH1_INDEX);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_CALLBACK, uac_dma_callback);
    usbd_ep_enable_dma_tx(AUDIO_IN_EP);
}

static int uac_dma_write(const uint8_t *data, uint32_t len)
{
    int ret;

    uac_dma_len = len;
    L1C_DCACHE_CLEAN_RANGE((unsigned long)data, len);

    ret = dma_reload(DMA0_ID, DMA0_CH1_INDEX,
                     (uint32_t)(uintptr_t)data,
                     usbd_ep_get_tx_fifo_addr(AUDIO_IN_EP), len, NULL);
    if (ret != 0)
        return ret;

    dma_channel_start(DMA0_ID, DMA0_CH1_INDEX);
    usbd_ep_trigger_ready(AUDIO_IN_EP);

    if (xSemaphoreTake(uac_dma_sem, portMAX_DELAY) == pdTRUE) {
        usbd_event_ep_in_complete_handler(AUDIO_IN_EP | 0x80, uac_dma_len);
        return 0;
    }
    return -1;
}

/* --- UAC ADC callback (ISR) --- */
static void uac_audio_callback(int buf_idx)
{
    BaseType_t woken = pdFALSE;

    if (uac_stream_buffer == NULL)
        return;

    L1C_DCACHE_INVALID_RANGE(uac_pcm_buf[buf_idx], ADC_FRAME_SIZE_BYTES);

    xStreamBufferSendFromISR(uac_stream_buffer,
                             uac_pcm_buf[buf_idx],
                             ADC_FRAME_SIZE_BYTES, &woken);
    portYIELD_FROM_ISR(woken);
}

static void uac_auadc_init(int gain)
{
    hal_auadc_amic_cfg_t cfg;

    if (uac_pcm_buf[0] == NULL) {
        uac_pcm_buf[0] = (int16_t *)malloc(ADC_FRAME_SIZE_BYTES * 2);
        if (uac_pcm_buf[0] == NULL) {
            printf("[UAC] failed to allocate ADC buffers\r\n");
            return;
        }
        uac_pcm_buf[1] = uac_pcm_buf[0] + ADC_FRAME_SIZE_SAMPLES;
    }

    cfg.amic_single_ended = 1;
    cfg.amic_pos_ch = 4;
    cfg.amic_neg_ch = 7;
    cfg.pcm_frame_size = ADC_FRAME_SIZE_BYTES;
    cfg.pcm_frame_buf[0] = uac_pcm_buf[0];
    cfg.pcm_frame_buf[1] = uac_pcm_buf[1];
    cfg.pcm_frame_event = uac_audio_callback;

    hal_auadc_set_analog_gain(gain);
    hal_auadc_amic_init(&cfg);
}

/* --- UAC CherryUSB callbacks --- */
void usbd_audio_open(uint8_t intf)
{
    if (intf == 1) {
        uac_tx_flag = 1;
        printf("[UAC] mic open\r\n");
        hal_auadc_start();
    }
}

void usbd_audio_close(uint8_t intf)
{
    if (intf == 1) {
        uac_tx_flag = 0;
        printf("[UAC] mic close\r\n");
        hal_auadc_stop();
    }
}

void usbd_audio_in_callback(uint8_t ep, uint32_t nbytes)
{
    uac_ep_tx_busy = false;
}

static struct usbd_endpoint uac_in_ep = {
    .ep_cb = usbd_audio_in_callback,
    .ep_addr = AUDIO_IN_EP
};

struct audio_entity_info audio_entity_table[] = {
    { .bEntityId = AUDIO_IN_FU_ID,
      .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
      .ep = AUDIO_IN_EP },
};

/* --- UAC task --- */
static void uac_start_proc(void *arg)
{
    size_t bytes_received;

    uac_stream_buffer = xStreamBufferCreate(UAC_STREAM_BUFFER_SIZE,
                                            MONO_FRAME_SIZE_BYTES);
    if (uac_stream_buffer == NULL) {
        printf("[UAC] failed to create stream buffer\r\n");
        vTaskDelete(NULL);
        return;
    }

    uac_auadc_init(42);
    uac_dma_init();
    printf("[UAC] streaming ready\r\n");

    while (1) {
        if (uac_tx_flag) {
            bytes_received = xStreamBufferReceive(uac_stream_buffer,
                                                  uac_mono_frame,
                                                  MONO_FRAME_SIZE_BYTES,
                                                  portMAX_DELAY);
            if (bytes_received == MONO_FRAME_SIZE_BYTES) {
                for (int i = 0; i < AUDIO_SAMPLES_PER_MS; i++) {
                    uac_stereo_frame[i * 2 + 0] = uac_mono_frame[i];
                    uac_stereo_frame[i * 2 + 1] = uac_mono_frame[i];
                }
                uac_ep_tx_busy = true;
                uac_dma_write((uint8_t *)uac_stereo_frame,
                              STEREO_FRAME_SIZE_BYTES);
                while (uac_ep_tx_busy) {
                    if (!uac_tx_flag)
                        break;
                }
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    vTaskDelete(NULL);
}

/* ================================================================== */
/* NCM runtime                                                        */
/* ================================================================== */
#define NCM_IFNAME0 'n'
#define NCM_IFNAME1 'c'
#define NCM_RX_TASK_STACK_SIZE 1024
#define NCM_RX_TASK_PRIO       28

/* Device MAC must differ from iMACAddress (host adapter MAC C0:74:15:A1:3D:7E).
 * Using same MAC causes NDIS to drop frames as loopback. */
static uint8_t ncm_mac[6] = { 0xC0, 0x74, 0x15, 0xB2, 0x4E, 0x8F };
static struct netif ncm_netif;
static SemaphoreHandle_t ncm_rx_sem;
static StackType_t ncm_rx_stack[NCM_RX_TASK_STACK_SIZE];
static StaticTask_t ncm_rx_handle;

static volatile bool ncm_connected;
static volatile bool ncm_link_notified;
static volatile bool ncm_disconnect_pending;
static volatile bool ncm_data_intf_ready;

static TimerHandle_t ncm_tx_flush_timer;

static void ncm_tx_flush_callback(TimerHandle_t timer)
{
    taskENTER_CRITICAL();
    int ret = usbd_cdc_ncm_tx_flush();
    taskEXIT_CRITICAL();

    /* If USB IN was busy, the ACK stays in the aggregation buffer.
     * Restart the one-shot timer to retry — without this, window
     * update ACKs can be stuck indefinitely. */
    if (ret == -USB_ERR_BUSY)
        xTimerReset(timer, 0);
}

/* --- NCM lwIP netif --- */
static volatile uint32_t ncm_tx_call_cnt;
static volatile uint32_t ncm_tx_busy_cnt;
static volatile uint32_t ncm_tx_err_cnt;

static err_t ncm_linkoutput(struct netif *netif, struct pbuf *p)
{
    (void)netif;
    int ret;

    ncm_tx_call_cnt++;

    taskENTER_CRITICAL();
    ret = usbd_cdc_ncm_eth_tx(p);
    taskEXIT_CRITICAL();

    if (ret == 0) {
        if (ncm_tx_flush_timer)
            xTimerReset(ncm_tx_flush_timer, 0);
        return ERR_OK;
    }
    if (ret == -USB_ERR_BUSY) {
        ncm_tx_busy_cnt++;
        return ERR_WOULDBLOCK;
    }
    ncm_tx_err_cnt++;
    return ERR_IF;
}

static void ncm_rx_task(void *arg)
{
    struct netif *nif = (struct netif *)arg;
    struct pbuf *p;
    bool dhcp_started = false;
    bool ip_printed = false;
    int dhcp_wait = 0;
    int arp_retry = 0;
    ip4_addr_t static_gw;

    IP4_ADDR(&static_gw, 192, 168, 137, 1);

    for (;;) {
        xSemaphoreTake(ncm_rx_sem, pdMS_TO_TICKS(2));

        if (ncm_data_intf_ready && !ncm_link_notified) {
            int ret = usbd_cdc_ncm_send_connect_status(true);
            if (ret < 0) {
                /* INT IN EP not ready — retry next iteration */
                continue;
            }

            ncm_link_notified = true;
            ncm_data_intf_ready = false;

            if (dhcp_started) {
                dhcp_stop(nif);
                netif_set_link_down(nif);
            }
            netif_set_link_up(nif);
            vTaskDelay(pdMS_TO_TICKS(100));
            dhcp_start(nif);
            dhcp_started = true;
            ip_printed = false;
            arp_retry = 0;
            printf("[NCM] link up, DHCP started\r\n");
        }

        /* Prevent tcpip_thread (higher prio) from preempting during
         * datagram extraction.  Without this, every nif->input() post
         * causes a context switch to tcpip_thread and back, holding
         * the double-buffer slot ~6x longer than necessary. */
        taskENTER_CRITICAL();
        while ((p = usbd_cdc_ncm_eth_rx()) != NULL) {
            if (nif->input(p, nif) != ERR_OK)
                pbuf_free(p);
        }

        /* Flush pending TX (e.g. TCP ACKs) immediately after processing
         * RX data.  Without this, ACKs sit in the aggregation buffer
         * until the 1 ms flush timer fires, adding up to 1 ms per
         * round-trip and starving TCP throughput on FS USB. */
        usbd_cdc_ncm_tx_flush();
        taskEXIT_CRITICAL();

        /* Flush TX again OUTSIDE the critical section.  The flush
         * inside only catches data from *previous* iterations;
         * tcpip_thread (prio 30) preempts right after taskEXIT and
         * generates TCP ACKs into the aggregation buffer.  This
         * second flush sends those ACKs immediately instead of
         * waiting for the 1 ms timer, cutting ACK latency in half
         * and keeping the TCP window open. */
        taskENTER_CRITICAL();
        usbd_cdc_ncm_tx_flush();
        taskEXIT_CRITICAL();

        /* Recover stale TX: if EP IN is halted by host (e.g. ep:84
         * clear halt) the bulk_in callback never fires, leaving
         * g_ncm_tx_data_length > 0 forever.  All subsequent flushes
         * return -USB_ERR_BUSY, dropping TCP ACKs and ARP replies.
         * 50 ms timeout matches miniclaw's proven configuration. */
        usbd_cdc_ncm_tx_recover_stale(50);

        /* RX stale recovery: detect "lost" state where EP_RDY=0,
         * both slots free, not stalled.  Check every 500 iterations
         * (~1s) to avoid overhead.  Re-arm the endpoint to resume
         * receiving data. */
        {
            extern uint8_t usbd_cdc_ncm_get_rx_stalled(void);
            extern void usbd_cdc_ncm_recover_stale_rx(void);
            static uint32_t rx_watchdog_cnt;
            static uint32_t rx_watchdog_hit;
            if (++rx_watchdog_cnt >= 500) {
                rx_watchdog_cnt = 0;
                uint8_t stalled = usbd_cdc_ncm_get_rx_stalled();
                int ep_rdy = usbd_ep_is_ready(NCM_OUT_EP);
                if (ncm_link_notified && !stalled && ep_rdy == 0) {
                    rx_watchdog_hit++;
                    printf("[WD] EP OUT lost! stall=%u rdy=%d hit=%lu\r\n",
                           stalled, ep_rdy, rx_watchdog_hit);
                    usbd_cdc_ncm_recover_stale_rx();
                }
            }
        }

        if (dhcp_started && !ip_printed &&
            !ip4_addr_isany_val(*netif_ip4_addr(nif))) {
            printf("[NCM] IP: %s\r\n", ip4addr_ntoa(netif_ip4_addr(nif)));
            printf("[NCM] GW: %s\r\n", ip4addr_ntoa(netif_ip4_gw(nif)));
            ip_printed = true;
        }

        if (dhcp_started && !ip_printed) {
            dhcp_wait++;
            if (dhcp_wait > 2500) { /* 2500 * 2ms = 5s */
                ip4_addr_t ip, mask;
                IP4_ADDR(&ip, 192, 168, 137, 2);
                IP4_ADDR(&mask, 255, 255, 255, 0);
                dhcp_stop(nif);
                netif_set_addr(nif, &ip, &mask, &static_gw);
                ip_addr_t dns1, dns2;
                IP_ADDR4(&dns1, 192, 168, 137, 1);
                IP_ADDR4(&dns2, 8, 8, 8, 8);
                dns_setserver(0, &dns1);
                dns_setserver(1, &dns2);
                printf("[NCM] DHCP timeout, static: 192.168.137.2\r\n");
                arp_retry = 1;
                ip_printed = true;
                dhcp_started = false;
            }
        }

        /* Non-blocking ARP retry: send every 500th loop iteration (1s)
         * for up to 5 attempts. RX keeps running between retries. */
        if (arp_retry > 0) {
            if ((arp_retry % 500) == 1) { /* fire at iter 1, 501, 1001, ... */
                struct eth_addr *eth_ret;
                const ip4_addr_t *ip_ret;

                if (etharp_find_addr(nif, &static_gw,
                                     &eth_ret, &ip_ret) >= 0) {
                    printf("[NCM] gateway ARP resolved\r\n");
                    arp_retry = 0;
                } else {
                    etharp_gratuitous(nif);
                    etharp_request(nif, &static_gw);
                }
            }
            if (arp_retry > 2500) /* 2500 * 2ms = 5s total */
                arp_retry = 0;
            else if (arp_retry > 0)
                arp_retry++;
        }

        if (ncm_disconnect_pending) {
            ncm_disconnect_pending = false;
            if (dhcp_started) {
                dhcp_stop(nif);
                dhcp_started = false;
            }
            netif_set_link_down(nif);
            ip_printed = false;
            arp_retry = 0;
            printf("[NCM] link down\r\n");
        }
    }
}

static err_t ncm_netif_init(struct netif *netif)
{
    netif->name[0] = NCM_IFNAME0;
    netif->name[1] = NCM_IFNAME1;
    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, ncm_mac, 6);
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    netif->output = etharp_output;
    netif->linkoutput = ncm_linkoutput;

    ncm_rx_sem = xSemaphoreCreateBinary();
    if (ncm_rx_sem == NULL)
        return ERR_MEM;

    xTaskCreateStatic(ncm_rx_task, "ncm_rx", NCM_RX_TASK_STACK_SIZE,
                      netif, NCM_RX_TASK_PRIO, ncm_rx_stack, &ncm_rx_handle);
    return ERR_OK;
}

static void ncm_tcpip_init_done(void *arg)
{
    struct netif *nif = (struct netif *)arg;
    ip4_addr_t zero;

    ip4_addr_set_zero(&zero);
    netif_add(nif, &zero, &zero, &zero, NULL, ncm_netif_init, tcpip_input);
    netif_set_default(nif);
    netif_set_up(nif);
    printf("[NCM] netif up\r\n");
}

/* --- NCM CherryUSB callbacks --- */
void usbd_cdc_ncm_data_recv_done(uint8_t *buf, uint32_t len)
{
    (void)buf;
    (void)len;
    BaseType_t woken = pdFALSE;

    if (ncm_rx_sem)
        xSemaphoreGiveFromISR(ncm_rx_sem, &woken);
    portYIELD_FROM_ISR(woken);
}

void usbd_cdc_ncm_data_send_done(void)
{
}

void usbd_cdc_ncm_set_interface_done(void)
{
    ncm_data_intf_ready = true;
}

/* ================================================================== */
/* Composite event handler                                            */
/* ================================================================== */
static void composite_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        ncm_connected = true;
        ncm_link_notified = false;
        break;
    case USBD_EVENT_DISCONNECTED:
        ncm_connected = false;
        ncm_data_intf_ready = false;
        ncm_disconnect_pending = true;
        break;
    default:
        break;
    }
}

/* ================================================================== */
/* Shell command                                                      */
/* ================================================================== */
static struct usbd_interface intf0; /* Audio Control */
static struct usbd_interface intf1; /* Audio Streaming IN (mic) */
static struct usbd_interface intf2; /* NCM Communication */
static struct usbd_interface intf3; /* NCM Data */

static int cmd_composite_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    usbd_deinitialize();

    usbd_desc_register(composite_descriptor);

    /* Audio: 2 interfaces (AC + AS mic) */
    usbd_add_interface(usbd_audio_init_intf(&intf0, 0x0100,
                                            audio_entity_table, 1));
    usbd_add_interface(usbd_audio_init_intf(&intf1, 0x0100,
                                            audio_entity_table, 1));
    /* NCM: 2 interfaces */
    usbd_add_interface(usbd_cdc_ncm_init_intf(&intf2, NCM_INT_EP,
                                               NCM_OUT_EP, NCM_IN_EP));
    usbd_add_interface(usbd_cdc_ncm_init_intf(&intf3, NCM_INT_EP,
                                               NCM_OUT_EP, NCM_IN_EP));

    /* Register mic endpoint (NCM registers its own internally) */
    usbd_add_endpoint(&uac_in_ep);

    usbd_event_register_callback(composite_event_handler);
    usbd_initialize();

    ncm_tx_flush_timer = xTimerCreate("ncm_flush", pdMS_TO_TICKS(1),
                                       pdFALSE, NULL,
                                       ncm_tx_flush_callback);

    /* Start lwIP + NCM */
    tcpip_init(ncm_tcpip_init_done, &ncm_netif);

    /* Start UAC task */
    xTaskCreate(uac_start_proc, "uac", 1024, NULL, 10, NULL);

    printf("[COMPOSITE] UAC(mic)+NCM initialized\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_composite_init, usbd_composite_init,
                       UAC mic + NCM composite USB device);

/* --- NCM diagnostic counters --- */
extern volatile uint32_t ncm_diag_rx_ntb;
extern volatile uint32_t ncm_diag_rx_restart;
extern volatile uint32_t ncm_diag_rx_dgrams;
extern volatile uint32_t ncm_diag_tx_flush;
extern volatile uint32_t ncm_diag_tx_done;
extern volatile uint32_t ncm_diag_rx_stall;
extern volatile uint32_t ncm_diag_set_intf;
extern volatile uint32_t ncm_diag_rx_eth_call;
extern volatile uint32_t ncm_diag_rx_eth_ok;
extern volatile uint32_t ncm_diag_rx_rearm;
extern volatile uint32_t ncm_diag_rx_eth_hit;
extern volatile uint32_t ncm_diag_rx_pbuf_fail;
extern volatile uint32_t ncm_diag_rx_isr_rearm;
extern volatile uint32_t ncm_diag_max_stall_ticks;
extern volatile uint32_t ncm_diag_tx_stale_recover;
extern volatile uint32_t ncm_diag_tx_zlp;
extern volatile uint32_t ncm_diag_tx_last_nbytes;
extern volatile uint32_t ncm_diag_tx_last_len;

static int cmd_ncm_diag(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "reset") == 0) {
        extern void usbd_cdc_ncm_reset_diag(void);
        usbd_cdc_ncm_reset_diag();
        usbd_reset_irq_stats();
        ncm_tx_call_cnt = 0;
        ncm_tx_busy_cnt = 0;
        ncm_tx_err_cnt = 0;
        printf("[NCM DIAG] counters reset\r\n");
        return 0;
    }
    printf("[NCM DIAG] set_intf=%lu rx_ntb=%lu rx_drop=%lu rx_dgrams=%lu "
           "rx_stall=%lu tx_flush=%lu tx_done=%lu\r\n",
           (unsigned long)ncm_diag_set_intf,
           (unsigned long)ncm_diag_rx_ntb,
           (unsigned long)ncm_diag_rx_restart,
           (unsigned long)ncm_diag_rx_dgrams,
           (unsigned long)ncm_diag_rx_stall,
           (unsigned long)ncm_diag_tx_flush,
           (unsigned long)ncm_diag_tx_done);
    printf("[NCM DIAG] eth_call=%lu hit=%lu ok=%lu pbuf_fail=%lu "
           "rearm=%lu isr_rearm=%lu\r\n",
           (unsigned long)ncm_diag_rx_eth_call,
           (unsigned long)ncm_diag_rx_eth_hit,
           (unsigned long)ncm_diag_rx_eth_ok,
           (unsigned long)ncm_diag_rx_pbuf_fail,
           (unsigned long)ncm_diag_rx_rearm,
           (unsigned long)ncm_diag_rx_isr_rearm);
    printf("[NCM TX] linkout_call=%lu busy=%lu err=%lu\r\n",
           (unsigned long)ncm_tx_call_cnt,
           (unsigned long)ncm_tx_busy_cnt,
           (unsigned long)ncm_tx_err_cnt);

    /* EP3 OUT hardware status (post-mortem when link is dead) */
    extern uint8_t usbd_cdc_ncm_get_rx_stalled(void);
    extern uint8_t usbd_cdc_ncm_get_rx_fill_idx(void);
    extern void usbd_cdc_ncm_get_slot_info(uint8_t slot, uint16_t *dgram_count);
    usb_ep_out_status_t ep_st;
    usbd_get_ep_out_status(3, &ep_st);
    uint16_t slot0_cnt, slot1_cnt;
    usbd_cdc_ncm_get_slot_info(0, &slot0_cnt);
    usbd_cdc_ncm_get_slot_info(1, &slot1_cnt);
    printf("[EP3 OUT] cfg=0x%08lx fifo_rx=%u en=%u stall=%u "
           "buf=%p len=%lu actual=%lu\r\n",
           (unsigned long)ep_st.ep_cfg_reg,
           (unsigned)ep_st.fifo_rx_cnt,
           (unsigned)ep_st.ep_enabled,
           (unsigned)ep_st.ep_stalled,
           ep_st.xfer_buf,
           (unsigned long)ep_st.xfer_len,
           (unsigned long)ep_st.actual_xfer_len);
    printf("[NCM RX] ep_stalled=%u fill_idx=%u slot0_cnt=%u slot1_cnt=%u "
           "max_stall=%lums\r\n",
           (unsigned)usbd_cdc_ncm_get_rx_stalled(),
           (unsigned)usbd_cdc_ncm_get_rx_fill_idx(),
           (unsigned)slot0_cnt, (unsigned)slot1_cnt,
           (unsigned long)ncm_diag_max_stall_ticks);
    usb_irq_stats_t s;
    usbd_get_irq_stats(&s);
    printf("[ISR] total=%lu reset=%lu vbus=%lu ep0s=%lu ep0i=%lu ep0o=%lu "
           "rend=%lu ep0_bto=%lu setup_fail=%lu\r\n",
           (unsigned long)s.total,
           (unsigned long)s.reset,
           (unsigned long)s.vbus_tgl,
           (unsigned long)s.ep0_setup,
           (unsigned long)s.ep0_in,
           (unsigned long)s.ep0_out,
           (unsigned long)s.rend,
           (unsigned long)s.ep0_busy_timeout,
           (unsigned long)s.setup_fail);
    printf("[ISR OUT] entry=%lu busy=%lu rxz=%lu(still=%lu rec=%lu) "
           "short=%lu xfz=%lu more=%lu ep3=%lu\r\n",
           (unsigned long)s.out_entry,
           (unsigned long)s.out_busy_timeout,
           (unsigned long)s.out_rxcount_zero,
           (unsigned long)s.out_rxz_still,
           (unsigned long)s.out_rxz_recovered,
           (unsigned long)s.out_short_pkt,
           (unsigned long)s.out_xferlen_zero,
           (unsigned long)s.out_more_data,
           (unsigned long)s.ep_done[2]);

    /* EP4 IN (NCM TX) hardware status */
    usb_ep_in_status_t ep4_st;
    usbd_get_ep_in_status(4, &ep4_st);
    printf("[EP4 IN] cfg=0x%08lx fifo_tx=%u en=%u stall=%u "
           "buf=%p len=%lu actual=%lu\r\n",
           (unsigned long)ep4_st.ep_cfg_reg,
           (unsigned)ep4_st.fifo_tx_cnt,
           (unsigned)ep4_st.ep_enabled,
           (unsigned)ep4_st.ep_stalled,
           ep4_st.xfer_buf,
           (unsigned long)ep4_st.xfer_len,
           (unsigned long)ep4_st.actual_xfer_len);

    /* Per-EP done counts and NCM TX data length */
    extern uint32_t usbd_cdc_ncm_get_tx_data_length(void);
    printf("[ISR EP] ep3=%lu ep4=%lu tx_data_len=%lu\r\n",
           (unsigned long)s.ep_done[2],
           (unsigned long)s.ep_done[3],
           (unsigned long)usbd_cdc_ncm_get_tx_data_length());
    printf("[NCM TX2] stale_rec=%lu zlp=%lu last_nb=%lu last_len=%lu\r\n",
           (unsigned long)ncm_diag_tx_stale_recover,
           (unsigned long)ncm_diag_tx_zlp,
           (unsigned long)ncm_diag_tx_last_nbytes,
           (unsigned long)ncm_diag_tx_last_len);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ncm_diag, ncm_diag, NCM diagnostic counters);
