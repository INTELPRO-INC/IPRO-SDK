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

#include <FreeRTOS.h>
#include "ipro_vendor_ids.h"
#include <task.h>
#include <semphr.h>
#include <stream_buffer.h>
#include "shell.h"
#include <hal_dma.h>
#include <hal_dma.h>
#include <hal_dac.h>
#include <hal_gpio.h>
#include <hal_mtimer.h>
#include <hal_auadc.h>
#include "usbd_core.h"
#include "usbd_audio.h"
#include "usbd_event_common.h"

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0701
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

#define AUDIO_IN_EP  0x81
#define AUDIO_OUT_EP 0x02

#define AUDIO_IN_FU_ID  0x02
#define AUDIO_OUT_FU_ID 0x05

/* AUDIO Class Config */
#define AUDIO_SPEAKER_FREQ            16000U
#define AUDIO_SPEAKER_FRAME_SIZE_BYTE 2u
#define AUDIO_SPEAKER_RESOLUTION_BIT  16u
#define AUDIO_MIC_FREQ                16000U
#define AUDIO_MIC_FRAME_SIZE_BYTE     2u
#define AUDIO_MIC_RESOLUTION_BIT      16u

#define AUDIO_SAMPLE_FREQ(frq) (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define AUDIO_OUT_PACKET ((uint32_t)((AUDIO_SPEAKER_FREQ * AUDIO_SPEAKER_FRAME_SIZE_BYTE * 2) / 1000))
/* 16bit(2 Bytes) 双声道(Stereo:2) */
#define AUDIO_IN_PACKET ((uint32_t)((AUDIO_MIC_FREQ * AUDIO_MIC_FRAME_SIZE_BYTE * 2) / 1000))

#define USB_AUDIO_CONFIG_DESC_SIZ (unsigned long)(9 +                                       \
                                                  AUDIO_AC_DESCRIPTOR_INIT_LEN(2) +         \
                                                  AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                                                  AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                                                  AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC +    \
                                                  AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                                                  AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                                                  AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC +    \
                                                  AUDIO_AS_DESCRIPTOR_INIT_LEN(1) +         \
                                                  AUDIO_AS_DESCRIPTOR_INIT_LEN(1))

#define AUDIO_AC_SIZ (AUDIO_SIZEOF_AC_HEADER_DESC(2) +          \
                      AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                      AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                      AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC +    \
                      AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                      AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                      AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

const uint8_t audio_v1_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, 0x00, 0x01, 0x02),
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC, 0x02, 0x0003),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(0x02, 0x01, 0x01, 0x03, 0x00, 0x00),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING, 0x02),
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_TERMINAL_STREAMING, 0x02, 0x0003),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(0x05, 0x04, 0x01, 0x03, 0x00, 0x00),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x06, AUDIO_OUTTERM_SPEAKER, 0x05),
    AUDIO_AS_DESCRIPTOR_INIT(0x01, 0x04, 0x02, AUDIO_SPEAKER_FRAME_SIZE_BYTE, AUDIO_SPEAKER_RESOLUTION_BIT, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET,
                             EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(AUDIO_SPEAKER_FREQ)),
    AUDIO_AS_DESCRIPTOR_INIT(0x02, 0x03, 0x02, AUDIO_MIC_FRAME_SIZE_BYTE, AUDIO_MIC_RESOLUTION_BIT, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET,
                             EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(AUDIO_MIC_FREQ)),
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
    'A', 0x00,                  /* wcChar11 */
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
    '1', 0x00,                  /* wcChar9 */
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

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t read_buffer[AUDIO_OUT_PACKET];

static volatile bool tx_flag = 0;
static volatile bool rx_flag = 0;
static volatile bool ep_tx_busy_flag = false;
static SemaphoreHandle_t dma_complete_sem = NULL;
static uint32_t current_transfer_len = 0;

/* Audio frame size: 16kHz * 1ms = 16 samples per channel */
#define AUDIO_SAMPLES_PER_MS  16
#define MONO_FRAME_SIZE_BYTES (AUDIO_SAMPLES_PER_MS * 2)  /* 16 samples * 2 bytes = 32 bytes */
#define STEREO_FRAME_SIZE_BYTES (AUDIO_IN_PACKET)          /* 64 bytes for 2 channels */

/* ADC frame size from HAL (20ms = 320 samples) */
#define ADC_FRAME_SIZE_SAMPLES (320)
#define ADC_FRAME_SIZE_BYTES (ADC_FRAME_SIZE_SAMPLES * 2)

/* Stream buffer for mono audio data (buffer size = 100ms worth of data) */
#define STREAM_BUFFER_SIZE (MONO_FRAME_SIZE_BYTES * 100)
static StreamBufferHandle_t audio_stream_buffer = NULL;

/* Ping-pong buffers for ADC */
static int16_t *pcm_buf[2] = {NULL, NULL};

/* Temporary buffers for USB transmission */
static int16_t mono_frame[AUDIO_SAMPLES_PER_MS];    /* 1ms mono data */
static int16_t stereo_frame[AUDIO_SAMPLES_PER_MS * 2]; /* 1ms stereo data (interleaved) */

/* Audio source selection */
enum {
    UAC_SRC_AMIC,
    UAC_SRC_PDM,
};
static int uac_audio_src = UAC_SRC_AMIC;

static void uac_event_handler(uint8_t event)
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

void usbd_audio_open(uint8_t intf)
{
    if (intf == 1) {
        rx_flag = 1;
        /* setup first out ep read transfer */
        usbd_ep_start_read(AUDIO_OUT_EP, read_buffer, AUDIO_OUT_PACKET);
        printf("OPEN1\r\n");
    } else {
        tx_flag = 1;
        printf("OPEN2\r\n");
        hal_auadc_start();
    }
}

void usbd_audio_close(uint8_t intf)
{
    if (intf == 1) {
        rx_flag = 1;
        printf("CLOSE1\r\n");
    } else {
        tx_flag = 0;
        printf("CLOSE2\r\n");
        hal_auadc_stop();
    }
}

void usbd_audio_out_callback(uint8_t ep, uint32_t nbytes)
{
    //USB_LOG_RAW("actual out len:%lu\r\n", (unsigned long)nbytes);
    usbd_ep_start_read(AUDIO_OUT_EP, read_buffer, AUDIO_OUT_PACKET);
}

void usbd_audio_in_callback(uint8_t ep, uint32_t nbytes)
{
    //USB_LOG_RAW("actual in len:%lu\r\n", (unsigned long)nbytes);
    ep_tx_busy_flag = false;
}

static struct usbd_endpoint audio_in_ep = {
    .ep_cb = usbd_audio_in_callback,
    .ep_addr = AUDIO_IN_EP
};

static struct usbd_endpoint audio_out_ep = {
    .ep_cb = usbd_audio_out_callback,
    .ep_addr = AUDIO_OUT_EP
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static struct usbd_interface intf2;

struct audio_entity_info audio_entity_table[] = {
    { .bEntityId = AUDIO_IN_FU_ID,
      .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
      .ep = AUDIO_IN_EP },
    { .bEntityId = AUDIO_OUT_FU_ID,
      .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
      .ep = AUDIO_OUT_EP },
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
    usbd_ep_enable_dma_tx(AUDIO_IN_EP);
}

static int dma_usb_write(const uint8_t *data, uint32_t len)
{
    int ret;

    current_transfer_len = len;

    /* Clean cache for DMA */
    L1C_DCACHE_CLEAN_RANGE((unsigned long)data, len);

    /* Reload and start DMA */
    ret = dma_reload(DMA0_ID, DMA0_CH1_INDEX, (uint32_t)(uintptr_t)data, usbd_ep_get_tx_fifo_addr(AUDIO_IN_EP), len, NULL);
    if (ret != 0)
        return ret;

    dma_channel_start(DMA0_ID, DMA0_CH1_INDEX);

    /* Set EP1 Ready to trigger USB transmission (which pulls from DMA) */
    usbd_ep_trigger_ready(AUDIO_IN_EP);

    /* Wait for DMA completion via semaphore */
    if (xSemaphoreTake(dma_complete_sem, portMAX_DELAY) == pdTRUE) {
        /* Notify stack - Note: Isoc (UVC) endpoints don't generate EP1_DONE interrupt,
         * so we must manually call the completion handler */
        usbd_event_ep_in_complete_handler(AUDIO_IN_EP | 0x80, current_transfer_len);
        return 0;
    } else {
        /* Timeout */
        return -1;
    }
}



/* Audio callback from ADC - called from ISR context */
static void audio_callback(int buf_idx)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (audio_stream_buffer == NULL) {
        return;
    }

    /* Invalidate cache for DMA buffer */
    L1C_DCACHE_INVALID_RANGE(pcm_buf[buf_idx], ADC_FRAME_SIZE_BYTES);

    /* Push mono audio data to stream buffer (non-blocking from ISR) */
    size_t bytes_sent = xStreamBufferSendFromISR(
        audio_stream_buffer,
        pcm_buf[buf_idx],
        ADC_FRAME_SIZE_BYTES,
        &xHigherPriorityTaskWoken
    );

    /* Check if buffer is full (optional warning) */
    if (bytes_sent < ADC_FRAME_SIZE_BYTES) {
        /* Stream buffer is full, data lost */
        static uint32_t overflow_count = 0;
        overflow_count++;
        (void)overflow_count;  /* Suppress unused warning */
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void _auadc_amic_init(int gain)
{
    hal_auadc_amic_cfg_t cfg;

    /* Allocate ping-pong buffers for ADC (20ms frames) */
    if (pcm_buf[0] == NULL) {
        pcm_buf[0] = (int16_t *)malloc(ADC_FRAME_SIZE_BYTES * 2);
        if (pcm_buf[0] == NULL) {
            printf("Failed to allocate ADC buffers\r\n");
            return;
        }
        pcm_buf[1] = pcm_buf[0] + ADC_FRAME_SIZE_SAMPLES;
        printf("ADC buffers allocated: %p, %p\r\n", pcm_buf[0], pcm_buf[1]);
    }

    /* Configure ADC for mono microphone input */
    cfg.amic_single_ended = 1;
    cfg.amic_pos_ch = 4;
    cfg.amic_neg_ch = 7;
    cfg.pcm_frame_size = ADC_FRAME_SIZE_BYTES;
    cfg.pcm_frame_buf[0] = pcm_buf[0];
    cfg.pcm_frame_buf[1] = pcm_buf[1];
    cfg.pcm_frame_event = audio_callback;

    hal_auadc_set_analog_gain(gain);
    printf("ADC gain: %d\r\n", gain);

    hal_auadc_amic_init(&cfg);
}

#define PDM_CLK_PIN  12
#define PDM_DATA_PIN 11

static void _auadc_pdm_init(int gain)
{
    hal_auadc_pdm_cfg_t cfg;

    if (pcm_buf[0] == NULL) {
        pcm_buf[0] = (int16_t *)malloc(ADC_FRAME_SIZE_BYTES * 2);
        if (pcm_buf[0] == NULL) {
            printf("Failed to allocate PDM buffers\r\n");
            return;
        }
        pcm_buf[1] = pcm_buf[0] + ADC_FRAME_SIZE_SAMPLES;
        printf("PDM buffers allocated: %p, %p\r\n", pcm_buf[0], pcm_buf[1]);
    }

    cfg.pdm_clk_pin = PDM_CLK_PIN;
    cfg.pdm_in_pin = PDM_DATA_PIN;
    cfg.pdm_ch = HAL_AUADC_PDM_CH_LEFT;
    cfg.pcm_frame_size = ADC_FRAME_SIZE_BYTES;
    cfg.pcm_frame_buf[0] = pcm_buf[0];
    cfg.pcm_frame_buf[1] = pcm_buf[1];
    cfg.pcm_frame_event = audio_callback;

    hal_auadc_set_digital_gain(gain);
    printf("PDM digital gain: %d dB\r\n", gain);

    hal_auadc_pdm_init(&cfg);
}

static void uac_start_proc(void *arg)
{
    size_t bytes_received;

    /* Create stream buffer for audio data */
    audio_stream_buffer = xStreamBufferCreate(STREAM_BUFFER_SIZE, MONO_FRAME_SIZE_BYTES);
    if (audio_stream_buffer == NULL) {
        printf("Failed to create audio stream buffer\r\n");
        vTaskDelete(NULL);
        return;
    }
    printf("Audio stream buffer created (size: %d bytes)\r\n", STREAM_BUFFER_SIZE);

    /* Initialize audio source and DMA */
    if (uac_audio_src == UAC_SRC_PDM) {
        _auadc_pdm_init(0);
    } else {
        _auadc_amic_init(42);
    }
    dma_usb_init();

    printf("UAC streaming started\r\n");

    while (1) {
        if (tx_flag) {
            /* Read 1ms of mono audio data from stream buffer (32 bytes) */
            bytes_received = xStreamBufferReceive(
                audio_stream_buffer,
                mono_frame,
                MONO_FRAME_SIZE_BYTES,
                portMAX_DELAY  /* Wait indefinitely for data */
            );

            if (bytes_received == MONO_FRAME_SIZE_BYTES) {
                /* Convert mono to stereo (interleaved: L R L R ...) */
                for (int i = 0; i < AUDIO_SAMPLES_PER_MS; i++) {
                    stereo_frame[i * 2 + 0] = mono_frame[i];  /* Left channel */
                    stereo_frame[i * 2 + 1] = mono_frame[i];  /* Right channel (duplicate) */
                }

                /* Send stereo data via USB (64 bytes) */
                ep_tx_busy_flag = true;
                dma_usb_write((uint8_t*)stereo_frame, STEREO_FRAME_SIZE_BYTES);

                /* Wait for transmission to complete */
                while (ep_tx_busy_flag) {
                    if (tx_flag == false) {
                        break;
                    }
                }
            }
        } else {
            /* USB not active, wait a bit */
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    vTaskDelete(NULL);
}

int cmd_uac_init(int argc, char **argv)
{
    usbd_deinitialize();
    usbd_desc_register(audio_v1_descriptor);
    usbd_add_interface(usbd_audio_init_intf(&intf0, 0x0100, audio_entity_table, 2));
    usbd_add_interface(usbd_audio_init_intf(&intf1, 0x0100, audio_entity_table, 2));
    usbd_add_interface(usbd_audio_init_intf(&intf2, 0x0100, audio_entity_table, 2));
    usbd_add_endpoint(&audio_in_ep);
    usbd_add_endpoint(&audio_out_ep);
    usbd_event_register_callback(uac_event_handler);

    usbd_initialize();

    xTaskCreate(uac_start_proc, "uac", 1024, NULL, 10, NULL);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_uac_init, usbd_uac_init, USBD Audio Test);

int cmd_uac_pdm_init(int argc, char **argv)
{
    uac_audio_src = UAC_SRC_PDM;

    usbd_deinitialize();
    usbd_desc_register(audio_v1_descriptor);
    usbd_add_interface(usbd_audio_init_intf(&intf0, 0x0100, audio_entity_table, 2));
    usbd_add_interface(usbd_audio_init_intf(&intf1, 0x0100, audio_entity_table, 2));
    usbd_add_interface(usbd_audio_init_intf(&intf2, 0x0100, audio_entity_table, 2));
    usbd_add_endpoint(&audio_in_ep);
    usbd_add_endpoint(&audio_out_ep);
    usbd_event_register_callback(uac_event_handler);

    usbd_initialize();

    xTaskCreate(uac_start_proc, "uac", 1024, NULL, 10, NULL);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_uac_pdm_init, usbd_uac_pdm_init, USBD Audio PDM DMIC Test);
