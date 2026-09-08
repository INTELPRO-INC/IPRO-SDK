/**
 * @file uac_mic.c
 * @brief USB Audio Class microphone — the board enumerates as a 16 kHz
 *        mic so a PC can record the PDM capture directly.
 *
 * Ported from apps/platform/ipro7_demo/demo/soc/usb_demo/uac_v1_template.c.
 * The descriptor block is kept byte-identical to that proven template; only
 * the audio source, the DMA channel and the shell surface are adapted:
 *
 *   - Source: the template drove its own hal_auadc_*_init(). This app already
 *     configures PDM once at boot and re-initialising breaks the shared DMA,
 *     so the stream buffer is instead fed from main.c's existing capture ISR
 *     via uac_mic_feed_frame_isr() — the same hook pattern as spk_ai and
 *     dac_test.
 *   - DMA: the template hardcoded DMA0_CH1, which now collides with the DAC.
 *     With CONFIG_DMA_CHANNEL_ALLOCATOR the channel is requested at runtime.
 *
 * The speaker (OUT) half of the template is left in place so the descriptors
 * stay untouched; incoming packets are read into a scratch buffer and
 * discarded. Wiring it to the DAC would be a separate change.
 *
 * Shell: uac on | off | stat
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <FreeRTOS.h>
#include "ipro_vendor_ids.h"
#include <task.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <stdbool.h>
#include <string.h>
#include "shell.h"
#include <hal_dma.h>
#include <hal_gpio.h>
#include <hal_auadc.h>
#include "usbd_core.h"
#include "usbd_audio.h"
#include "usbd_event_common.h"
#include "ipro_log.h"

/* Implemented in bsp/drivers/ipro7_soc/std_drv/src/drv_usb_v1.c but not
 * exposed in a header — a plain register read of the VBUS detect bit. */
extern bool drv_usb_vbus_present(void);

#define TAG "UAC_MIC"

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0702    /* 0x0701 is the ipro7_demo template */
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

/* Capture frame size produced by main.c's PDM ISR (20 ms = 320 samples) */
#define ADC_FRAME_SIZE_SAMPLES (320)
#define ADC_FRAME_SIZE_BYTES (ADC_FRAME_SIZE_SAMPLES * 2)

/* Stream buffer for mono audio data (buffer size = 100ms worth of data) */
#define STREAM_BUFFER_SIZE (MONO_FRAME_SIZE_BYTES * 100)
static StreamBufferHandle_t audio_stream_buffer = NULL;

/* Temporary buffers for USB transmission */
static int16_t mono_frame[AUDIO_SAMPLES_PER_MS];    /* 1ms mono data */
static int16_t stereo_frame[AUDIO_SAMPLES_PER_MS * 2]; /* 1ms stereo data (interleaved) */

static volatile bool     uac_enabled;      /* shell `uac on` latched */
static volatile uint32_t uac_overflow;     /* frames dropped: USB not draining */
static volatile uint32_t uac_frames;       /* 1 ms packets shipped */

/* From main.c — the shared PDM capture this app owns */
extern bool pdm_is_recording(void);
extern bool pdm_is_streaming(void);
extern int  pdm_get_gain(void);

/* Bring-up instrument: the template left every case empty, which makes a
 * non-enumerating device indistinguishable from a disconnected cable. Each
 * event tells you exactly how far the host got:
 *   (no events)  -> no VBUS / no data lines / D+ pull-up not seen
 *   RESET only   -> host sees the device but the descriptor fetch failed
 *   CONFIGURED   -> enumeration complete; the mic should now be selectable
 */
static volatile uint32_t uac_ev_reset, uac_ev_configured;

static void uac_event_handler(uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            uac_ev_reset++;
            IPRO_LOGI(TAG, "USB event: RESET (#%lu)",
                      (unsigned long)uac_ev_reset);
            break;
        case USBD_EVENT_CONNECTED:
            IPRO_LOGI(TAG, "USB event: CONNECTED");
            break;
        case USBD_EVENT_DISCONNECTED:
            IPRO_LOGI(TAG, "USB event: DISCONNECTED");
            break;
        case USBD_EVENT_RESUME:
            IPRO_LOGI(TAG, "USB event: RESUME");
            break;
        case USBD_EVENT_SUSPEND:
            IPRO_LOGI(TAG, "USB event: SUSPEND");
            break;
        case USBD_EVENT_CONFIGURED:
            uac_ev_configured++;
            IPRO_LOGI(TAG, "USB event: CONFIGURED (#%lu) — enumerated OK",
                      (unsigned long)uac_ev_configured);
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            IPRO_LOGD(TAG, "USB event: %u", event);
            break;
    }
}

/* Host opened/closed a streaming interface. intf 1 = speaker (OUT, discarded),
 * anything else = mic (IN). The mic side owns hal_auadc start/stop while it is
 * open, so refuse if a shell capture already has it. */
void usbd_audio_open(uint8_t intf)
{
    if (intf == 1) {
        rx_flag = 1;
        /* setup first out ep read transfer */
        usbd_ep_start_read(AUDIO_OUT_EP, read_buffer, AUDIO_OUT_PACKET);
        return;
    }

    if (pdm_is_recording() || pdm_is_streaming()) {
        IPRO_LOGW(TAG, "host opened the mic while a capture is running — "
                       "stop pdm_rec/pdm_stream first");
        return;
    }

    uac_frames = 0;
    uac_overflow = 0;
    xStreamBufferReset(audio_stream_buffer);

    hal_auadc_set_digital_gain(pdm_get_gain());
    uac_enabled = true;
    tx_flag = 1;
    hal_auadc_start();
    IPRO_LOGI(TAG, "host opened mic — streaming at %d dB", pdm_get_gain());
}

void usbd_audio_close(uint8_t intf)
{
    if (intf == 1) {
        rx_flag = 0;
        return;
    }

    tx_flag = 0;
    uac_enabled = false;
    hal_auadc_stop();
    IPRO_LOGI(TAG, "host closed mic — %lu packets, %lu overflow",
              (unsigned long)uac_frames, (unsigned long)uac_overflow);
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

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
static dma_ch_handle_t usb_dma_handle;
#endif
static int usb_dma_ch = DMA0_CH1_INDEX;

static void dma_usb_init(void)
{
    /* Create semaphore for DMA completion */
    if (dma_complete_sem == NULL) {
        dma_complete_sem = xSemaphoreCreateBinary();
    }

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    /* The template hardcoded CH1, which this app now hands to the DAC.
     * Request a channel instead so AUADC capture, DAC playback and the UAC
     * endpoint can be live at the same time. */
    if (usb_dma_handle == NULL) {
        usb_dma_handle = dma_request_channel(DMA_REQUEST_USB_EP1,
                                             DMA_MEMORY_TO_PERIPH, "uac");
        if (usb_dma_handle == NULL) {
            IPRO_LOGE(TAG, "DMA channel request failed");
            return;
        }
        usb_dma_ch = dma_handle_to_ch(usb_dma_handle);
    }
#endif
    usb_dma_dev.ch = (uint8_t)usb_dma_ch;

    dma_setConfig(&usb_dma_dev);
    dma_init(DMA0_ID, usb_dma_ch);

    /* Enable DMA interrupt */
    dma_ioctl(DMA0_ID, usb_dma_ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, usb_dma_ch, DMA_CTRL_SET_CALLBACK, dma_usb_callback);

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
    ret = dma_reload(DMA0_ID, usb_dma_ch, (uint32_t)(uintptr_t)data, usbd_ep_get_tx_fifo_addr(AUDIO_IN_EP), len, NULL);
    if (ret != 0)
        return ret;

    dma_channel_start(DMA0_ID, usb_dma_ch);

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



/* Fed from main.c's PDM DMA ISR for every captured 20 ms frame. The capture
 * hardware is initialised once at boot and shared, so this path only copies —
 * it never touches hal_auadc configuration. */
void uac_mic_feed_frame_isr(const int16_t *pcm, uint32_t bytes,
                            BaseType_t *woken)
{
    if (!uac_enabled || audio_stream_buffer == NULL) {
        return;
    }

    size_t sent = xStreamBufferSendFromISR(audio_stream_buffer, pcm, bytes,
                                           woken);
    if (sent < bytes) {
        uac_overflow++;
    }
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

    /* No capture bring-up here: main.c owns the single hal_auadc instance
     * and re-initialising it would break the shared DMA. Frames arrive via
     * uac_mic_feed_frame_isr(); this task only reshapes and ships them. */
    dma_usb_init();

    IPRO_LOGI(TAG, "UAC mic ready — 16 kHz mono, pdm_gain %d dB", pdm_get_gain());

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
                uac_frames++;

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

static bool uac_started;

static void uac_usb_bring_up(void)
{
    usbd_deinitialize();
    usbd_desc_register(audio_v1_descriptor);
    usbd_add_interface(usbd_audio_init_intf(&intf0, 0x0100, audio_entity_table, 2));
    usbd_add_interface(usbd_audio_init_intf(&intf1, 0x0100, audio_entity_table, 2));
    usbd_add_interface(usbd_audio_init_intf(&intf2, 0x0100, audio_entity_table, 2));
    usbd_add_endpoint(&audio_in_ep);
    usbd_add_endpoint(&audio_out_ep);
    usbd_event_register_callback(uac_event_handler);

    int rc = usbd_initialize();
    printf("uac: usbd_initialize -> %d, VBUS %s\r\n",
           rc, drv_usb_vbus_present() ? "present" : "ABSENT");
    if (!drv_usb_vbus_present()) {
        printf("uac: no VBUS — check the cable is a data cable and that the\r\n"
               "     port is powered; the host cannot see us without it\r\n");
    }

    xTaskCreate(uac_start_proc, "uac", 1024, NULL, 10, NULL);
}

static int cmd_uac(int argc, char **argv)
{
    if (argc < 2) {
        printf("uac: USB %s, host %s, %lu packets, %lu overflow\r\n",
               uac_started ? "up" : "down",
               tx_flag ? "streaming" : "idle",
               (unsigned long)uac_frames, (unsigned long)uac_overflow);
        printf("Usage: uac on | off | stat\r\n");
        printf("  Enumerates as a 16 kHz mono microphone. Record it with any\r\n");
        printf("  host audio tool; pdm_gain still sets the capture level.\r\n");
        return 0;
    }

    if (strcmp(argv[1], "on") == 0) {
        if (uac_started) {
            printf("uac: already up\r\n");
            return 0;
        }
        if (audio_stream_buffer == NULL) {
            audio_stream_buffer = xStreamBufferCreate(STREAM_BUFFER_SIZE,
                                                      MONO_FRAME_SIZE_BYTES);
            if (audio_stream_buffer == NULL) {
                printf("uac: stream buffer alloc failed\r\n");
                return -1;
            }
        }
        uac_usb_bring_up();
        uac_started = true;
        printf("uac: up — plug USB into a host and select the mic\r\n");
        return 0;
    }

    if (strcmp(argv[1], "off") == 0) {
        if (!uac_started) {
            printf("uac: not up\r\n");
            return 0;
        }
        uac_enabled = false;
        tx_flag = 0;
        hal_auadc_stop();
        usbd_deinitialize();
        uac_started = false;
        printf("uac: down\r\n");
        return 0;
    }

    if (strcmp(argv[1], "stat") == 0) {
        printf("uac: USB %s, host %s\r\n",
               uac_started ? "up" : "down",
               tx_flag ? "streaming" : "idle");
        /* The VBUS status bit only means anything once usb_dc_init() has
         * brought the PHY up, so do not print a reading that would look
         * authoritative while USB is still down. */
        if (uac_started) {
            printf("  VBUS %s   resets %lu   configured %lu\r\n",
                   drv_usb_vbus_present() ? "present" : "ABSENT",
                   (unsigned long)uac_ev_reset,
                   (unsigned long)uac_ev_configured);
        } else {
            printf("  VBUS unknown (PHY not initialised) — run `uac on` first\r\n");
        }
        printf("  packets %lu (1 ms each = %lu.%02lu s)\r\n",
               (unsigned long)uac_frames,
               (unsigned long)(uac_frames / 1000),
               (unsigned long)((uac_frames % 1000) / 10));
        printf("  overflow %lu frames (capture outran USB)\r\n",
               (unsigned long)uac_overflow);
        printf("  pdm_gain %d dB\r\n", pdm_get_gain());
        return 0;
    }

    printf("uac: unknown option\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_uac, uac, USB Audio mic: uac on|off|stat);
