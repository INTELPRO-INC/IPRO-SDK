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
#include "shell.h"
/* Filesystem - EVFS POSIX API */
#include "evfs.h"
#include "evfs_posix.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_event_common.h"
#include <compiler/common.h>
#include "system_soc.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stream_buffer.h>
#include <string.h>
#include "drv_glb.h"

#ifdef CONFIG_PIRCAM_TOOL_UART
#include "helper_socket.h"
#endif

//#define CDC_THROUGHPUT_TEST_SUPPORT

#ifdef CONFIG_PIRCAM_TOOL_UART
/*!< endpoint address (0x82/0x03/0x84 to coexist with UVC on 0x81) */
#define CDC_IN_EP  0x82
#define CDC_OUT_EP 0x03
#define CDC_INT_EP 0x84
#else
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83
#endif

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0701
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

/*!< global descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
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
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
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
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

/* PSRAM buffers with cache-line alignment (32 bytes) for DMA coherency */
static uint8_t read_buffer[2048] ATTR_EALIGN(32) ATTR_PSRAM_BSS_SECTION;
static uint8_t write_buffer[2048] ATTR_EALIGN(32) ATTR_PSRAM_BSS_SECTION;
#ifdef CONFIG_PIRCAM_TOOL_UART
static uint8_t iproc_tx_buffer[512] ATTR_EALIGN(32) ATTR_PSRAM_BSS_SECTION;
static uint8_t iproc_deferred_buf[512];
static volatile uint32_t iproc_deferred_len = 0;
static TaskHandle_t iproc_task_handle = NULL;

static void iproc_task(void *arg)
{
    (void)arg;
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (iproc_deferred_len > 0) {
            iproc_process_bulk(iproc_deferred_buf, iproc_deferred_len);
            iproc_deferred_len = 0;
        }
    }
}
#endif

static volatile bool ep_tx_busy_flag = false;

/* CDC write function registered as IPROC transport */
#ifdef CONFIG_PIRCAM_TOOL_UART
static int cdc_iproc_write(const void *buf, uint32_t len)
{
    extern volatile uint8_t dtr_enable;
    if (!dtr_enable)
        return len;  /* host not listening, silently discard */
    if (len == 0 || len > sizeof(iproc_tx_buffer))
        return -1;
    memcpy(iproc_tx_buffer, buf, len);
    L1C_DCACHE_CLEAN_RANGE(iproc_tx_buffer, len);
    printf("[CDC] TX start len=%lu ep_tx_busy=%d\n", (unsigned long)len, (int)ep_tx_busy_flag);
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, iproc_tx_buffer, len);
    uint32_t wait_ms = 0;
    while (ep_tx_busy_flag) {
        vTaskDelay(pdMS_TO_TICKS(1));
        wait_ms++;
        if (wait_ms >= 2000) {
            printf("[CDC] TX timeout! ep_tx_busy stuck\n");
            ep_tx_busy_flag = false;
            return -1;
        }
    }
    printf("[CDC] TX done wait=%lums\n", (unsigned long)wait_ms);
    return (int)len;
}
#endif
volatile static int gCDCEchoTest = 0;

/* CDC file receive state (used by bulk_out callback) */
static StreamBufferHandle_t s_cdc_rx_stream;
static volatile int s_cdc_rx_active;
#ifdef CDC_THROUGHPUT_TEST_SUPPORT
USB_MEM_ALIGNX static uint8_t gTPBuffer[120*1024];
#endif
#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

static void cdc_event_handler(uint8_t event)
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
            /* setup first out ep read transfer */
            usbd_ep_start_read(CDC_OUT_EP, read_buffer, 2048);
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

/* Forward declaration */
static void cdc_rx_feed(const uint8_t *data, uint32_t len);

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    /* invalidate cache after DMA write (ensure CPU sees DMA's data) */
    L1C_DCACHE_INVALID_RANGE(read_buffer, sizeof(read_buffer));

    /* Route to file receive if active */
    if (s_cdc_rx_active && nbytes > 0) {
        cdc_rx_feed(read_buffer, nbytes);
        usbd_ep_start_read(CDC_OUT_EP, read_buffer, 2048);
        return;
    }

    /* IPROC protocol: detect prefix and defer to task context */
	#ifdef CONFIG_PIRCAM_TOOL_UART
    if (nbytes >= 5 && memcmp(read_buffer, "IPROC", 5) == 0) {
        uint32_t proc_len = (nbytes < sizeof(iproc_deferred_buf)) ? nbytes : sizeof(iproc_deferred_buf);
        memcpy(iproc_deferred_buf, read_buffer, proc_len);
        iproc_deferred_len = proc_len;

        /* re-arm OUT endpoint */
        usbd_ep_start_read(CDC_OUT_EP, read_buffer, 2048);

        /* wake up iproc task (ISR-safe) */
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(iproc_task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        return;
    }
	#endif

#ifndef CDC_THROUGHPUT_TEST_SUPPORT
    USB_LOG_RAW("actual out len:%ld\r\n", nbytes);
#endif
    // for (int i = 0; i < 100; i++) {
    //     printf("%02x ", read_buffer[i]);
    // }
    // printf("\r\n");
#ifdef CDC_ECHO_SUPPORT
    if (gCDCEchoTest) {
        USB_LOG_RAW("actual out len:%ld\r\n", nbytes);
        for (int i = 0; i < 4; i++) {
            printf("%02x ", read_buffer[i]);
        }
        printf("\r\n");
        if (nbytes > 0)
            read_buffer[0]+=1;
        if (nbytes > 1)
            read_buffer[1]+=2;
        if (nbytes > 2)
            read_buffer[2]+=3;
        if (nbytes > 3)
            read_buffer[3]+=4;
        /* flush cache before DMA read */
        L1C_DCACHE_CLEAN_RANGE(read_buffer, nbytes);
        usbd_ep_start_write(CDC_IN_EP, read_buffer, nbytes);
    }
#endif
    /* setup next out ep read transfer */
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, 2048);
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
#ifndef CDC_THROUGHPUT_TEST_SUPPORT
    USB_LOG_RAW("actual in len:%ld\r\n", nbytes);
#endif

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(CDC_IN_EP, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
    }
}

/*!< endpoint call back */
static struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

static struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;

/* Setup CDC-ACM interfaces/endpoints only (caller manages usbd_initialize) */
void cdc_acm_setup(void)
{
    const uint8_t data[10] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };

    memcpy(&write_buffer[0], data, 10);
    memset(&write_buffer[10], 'a', 2038);
    L1C_DCACHE_CLEAN_RANGE(write_buffer, sizeof(write_buffer));

    usbd_desc_register(cdc_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);
    usbd_event_register_callback(cdc_event_handler);

#ifdef CONFIG_PIRCAM_TOOL_UART
    iproc_set_write_fn(cdc_iproc_write);
    if (!iproc_task_handle)
        xTaskCreate(iproc_task, "iproc", 2048, NULL, 10, &iproc_task_handle);
    printf("CDC-ACM setup done, IPROC over CDC\r\n");
#endif
}

/* Add CDC interfaces/endpoints only (for composite — no descriptor, no init) */
void cdc_acm_add_interfaces(void)
{
    const uint8_t data[10] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };

    memcpy(&write_buffer[0], data, 10);
    memset(&write_buffer[10], 'a', 2038);
    L1C_DCACHE_CLEAN_RANGE(write_buffer, sizeof(write_buffer));

    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);

#ifdef CONFIG_PIRCAM_TOOL_UART
    iproc_set_write_fn(cdc_iproc_write);
    if (!iproc_task_handle)
        xTaskCreate(iproc_task, "iproc", 2048, NULL, 10, &iproc_task_handle);
#endif
}

/* Arm first CDC OUT read (called by composite event handler on CONFIGURED) */
void cdc_acm_on_configured(void)
{
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, 2048);
}

static void cdc_acm_init_task(void *arg)
{
    usbd_deinitialize();
    cdc_acm_setup();
    usbd_initialize();
    printf("CDC-ACM ready (standalone)\r\n");
    vTaskDelete(NULL);
}

int cmd_cdc_acm_init(int argc, char **argv)
{
    xTaskCreate(cdc_acm_init_task, "cdc_init", 2048, NULL, 10, NULL);
    printf("CDC init started in background\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_acm_init, usbd_cdc_init, USBD CDC Init);

/* reboot_to_usb_dl [5|inf]: request a LONGER boot2 USB-DL window than the 500ms
 * default that every boot already offers. Use it for manual flashing when you
 * can't catch the short default window. no-arg / "inf" = wait forever, "5" = 5s.
 * The request is encoded in HBN_RSV3 (survives reset): [31:16] = magic "US"
 * (0x5553), [15:8] = timeout code (0x02=5s, 0xFF=inf); [7:0] = BROM XTAL byte.
 * Same encoding as the flash tool's bootheader patch (GLB_RSV1). */
int cmd_reboot_to_usb_dl(int argc, char **argv)
{
    uint8_t code = 0xFF;  /* default: wait for the host indefinitely */
    const char *name = "inf";

    if (argc >= 2 && strcmp(argv[1], "5") == 0) {
        code = 0x02;  /* 5 s */
        name = "5s";
    }

    uint32_t v = *(volatile uint32_t *)0x3008F10C;
    *(volatile uint32_t *)0x3008F10C =
        (0x5553u << 16) | ((uint32_t)code << 8) | (v & 0x000000FFu);
    printf("Rebooting to boot2 USB download (timeout=%s)...\r\n", name);
    GLB_SW_System_Reset();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_reboot_to_usb_dl, reboot_to_usb_dl, Reboot into boot2 USB download [5|inf]);

/* reboot_nousb: reboot with NO boot2 USB-DL window. Sets the SKIP magic so
 * boot2 returns before usb_init() — no USB hardware init / enumeration — and
 * boots the application straight away. (The framework `reboot` does a bare
 * reset, which with this boot2 takes the 500ms default window when USB is
 * plugged; this is the explicit "skip USB" path.) */
int cmd_reboot_nousb(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint32_t v = *(volatile uint32_t *)0x3008F10C;
    /* magic "US" + SKIP code (0x00), preserving [7:0] BROM XTAL byte */
    *(volatile uint32_t *)0x3008F10C = (0x5553u << 16) | (v & 0x000000FFu);
    printf("Rebooting (skip boot2 USB window)...\r\n");
    GLB_SW_System_Reset();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_reboot_nousb, reboot_nousb, Reboot skipping boot2 USB window);

volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    if (dtr) {
        dtr_enable = 1;
    } else {
        dtr_enable = 0;
    }
}

void cdc_acm_data_send_with_dtr_test(void)
{
    if (dtr_enable) {
        /* flush cache before DMA read */
        L1C_DCACHE_CLEAN_RANGE(write_buffer, sizeof(write_buffer));
        ep_tx_busy_flag = true;
        usbd_ep_start_write(CDC_IN_EP, write_buffer, 2048);
        while (ep_tx_busy_flag) {
        }
    }
}

__attribute__((unused)) static int cmd_cdc_dtr_test(int argc, char **argv)
{
    /* flush cache before DMA read */
    L1C_DCACHE_CLEAN_RANGE(write_buffer, sizeof(write_buffer));
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, write_buffer, 2048);
    while (ep_tx_busy_flag) {
    }

    return 0;
}

__attribute__((unused)) int cmd_cdc_acm_echo_test(int argc, char **argv)
{
    if (argc == 2)
        gCDCEchoTest = atoi(argv[1]);
    printf("gCDCEchoTest = %d\r\n", gCDCEchoTest);

    return 0;
}

__attribute__((unused)) int cmd_cdc_acm_write_test(int argc, char **argv)
{
#ifdef CDC_THROUGHPUT_TEST_SUPPORT
    memset(gTPBuffer, 0xa5, sizeof(gTPBuffer));
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, gTPBuffer, 4);
    while (ep_tx_busy_flag);
    printf("Write Start\r\n");
    int write_len = 0;
    while (write_len < 120*1024*1024) {
        //printf("write cdc %d\r\n", write_len);
        ep_tx_busy_flag = true;
        usbd_ep_start_write(CDC_IN_EP, gTPBuffer, 120*1024);
        while (ep_tx_busy_flag);
        write_len += 120*1024;
    }
    printf("Write Finished\r\n");
#endif

    return 0;
}

#define CDC_STREAM_CHUNK    4096

static void cdc_send_buf(const uint8_t *buf, uint32_t len)
{
    L1C_DCACHE_CLEAN_RANGE(buf, len);
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, buf, len);
    while (ep_tx_busy_flag);
}

int cmd_cdc_acm_send_file(int argc, char **argv)
{
    int fd;
    char *path;
    struct stat st;
    uint32_t file_size, path_len, trans_type = 0;

    if (argc < 2) {
        printf("Usage: usbd_cdc_sendfile <file_path> [max_bytes]\r\n");
        return 0;
    }

    path = argv[1];

    if (stat(path, &st) != 0) {
        printf("stat %s fail\r\n", path);
        return 0;
    }
    file_size = st.st_size;

    /* Optional: limit transfer size */
    if (argc >= 3) {
        uint32_t limit = (uint32_t)strtoul(argv[2], NULL, 0);
        if (limit > 0 && limit < file_size) {
            file_size = limit;
        }
    }

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("open %s fail\r\n", path);
        return 0;
    }

    printf("Sending %s (%lu bytes) via CDC streaming\r\n",
           path, (unsigned long)file_size);

    /* Header: type(4) + size(4) + pathlen(4) + path */
    cdc_send_buf((uint8_t *)&trans_type, 4);
    cdc_send_buf((uint8_t *)&file_size, 4);
    path_len = strlen(path);
    cdc_send_buf((uint8_t *)&path_len, 4);
    cdc_send_buf((uint8_t *)path, path_len);

    /* Stream file data in chunks */
    uint8_t *chunk_buf = malloc(CDC_STREAM_CHUNK);
    if (!chunk_buf) {
        printf("chunk alloc fail\r\n");
        close(fd);
        return 0;
    }

    uint32_t sent = 0;
    while (sent < file_size) {
        uint32_t want = file_size - sent;
        if (want > CDC_STREAM_CHUNK) want = CDC_STREAM_CHUNK;

        int n = read(fd, chunk_buf, want);
        if (n <= 0) {
            printf("read error at %lu\r\n", (unsigned long)sent);
            break;
        }

        cdc_send_buf(chunk_buf, n);
        sent += n;

        if (sent % (256 * 1024) < (uint32_t)n) {
            printf("  %lu / %lu KB\r\n",
                   (unsigned long)(sent / 1024),
                   (unsigned long)(file_size / 1024));
        }
    }

    free(chunk_buf);
    close(fd);

    printf("Send done: %lu bytes\r\n", (unsigned long)sent);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_acm_send_file, usbd_cdc_sendfile, USBD CDC Send File [max_bytes]);

int cmd_cdc_acm_send_mem(int argc, char **argv)
{
    uint32_t start_addr, end_addr, mem_size, trans_type = 1;
    uint8_t *mem_ptr;

    if (argc != 3) {
        printf("Usage: usbd_cdc_sendmem <start_addr> <end_addr>\r\n");
        printf("  Addresses can be in hex (0x...) or decimal format\r\n");
        return 0;
    }

    /* Parse start and end addresses, auto-detect base (hex with 0x or decimal) */
    start_addr = strtoul(argv[1], NULL, 0);
    end_addr = strtoul(argv[2], NULL, 0);

    if (end_addr <= start_addr) {
        printf("Error: end address must be greater than start address\r\n");
        return 0;
    }

    mem_size = end_addr - start_addr;
    mem_ptr = (uint8_t *)start_addr;

    printf("Sending memory from 0x%08lx to 0x%08lx (%lu bytes)\r\n",
           start_addr, end_addr, mem_size);

    /* Transfer Type (4B)*/
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, (uint8_t *)&trans_type, 4);
    while (ep_tx_busy_flag);

    /* Data Size (4B)*/
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, (uint8_t *)&mem_size, 4);
    while (ep_tx_busy_flag);

    /* Start Address (4B)*/
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, (uint8_t *)&start_addr, 4);
    while (ep_tx_busy_flag);

    printf("Sending memory data...\r\n");
    /* Data (Variable)*/
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, mem_ptr, mem_size);
    while (ep_tx_busy_flag);

    printf("Memory send completed\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_acm_send_mem, usbd_cdc_sendmem, USBD CDC Send Memory);

/* ---------------------------------------------------------------------------
 * CDC File Receive (Host → Device SD Card)
 *
 * Protocol: Host sends [magic:4B='WRIT'][size:4B][pathlen:4B][path][data]
 * Device receives via bulk_out callback and writes to SD card.
 * ------------------------------------------------------------------------- */

/* Receive task: reads from stream buffer, parses protocol, writes file */
static void cdc_rx_file_task(void *arg)
{
    (void)arg;
    uint8_t hdr[12];
    size_t n;

    printf("[CDC_RX] Waiting for file from host...\n");

    while (s_cdc_rx_active) {
        /* Read header: magic(4) + size(4) + pathlen(4) */
        n = xStreamBufferReceive(s_cdc_rx_stream, hdr, 12, pdMS_TO_TICKS(500));
        if (n < 12) continue;

        if (memcmp(hdr, "WRIT", 4) != 0) {
            printf("[CDC_RX] Unknown magic: %02x%02x%02x%02x\n",
                   hdr[0], hdr[1], hdr[2], hdr[3]);
            continue;
        }

        uint32_t file_size, path_len;
        memcpy(&file_size, &hdr[4], 4);
        memcpy(&path_len, &hdr[8], 4);

        if (path_len > 256 || path_len == 0) {
            printf("[CDC_RX] Invalid path_len: %lu\n", (unsigned long)path_len);
            continue;
        }

        /* Read path */
        char path[257];
        n = xStreamBufferReceive(s_cdc_rx_stream, path, path_len, pdMS_TO_TICKS(2000));
        if (n < path_len) {
            printf("[CDC_RX] Path read timeout\n");
            continue;
        }
        path[path_len] = '\0';

        printf("[CDC_RX] Receiving %s (%lu bytes)\n", path, (unsigned long)file_size);

        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            printf("[CDC_RX] Cannot create %s\n", path);
            /* Drain remaining data */
            uint32_t drain = file_size;
            uint8_t tmp[512];
            while (drain > 0) {
                uint32_t chunk = (drain > 512) ? 512 : drain;
                n = xStreamBufferReceive(s_cdc_rx_stream, tmp, chunk, pdMS_TO_TICKS(2000));
                if (n == 0) break;
                drain -= n;
            }
            continue;
        }

        /* Receive file data */
        uint32_t received = 0;
        uint8_t *buf = malloc(4096);
        if (!buf) {
            printf("[CDC_RX] Alloc failed\n");
            close(fd);
            continue;
        }

        while (received < file_size) {
            uint32_t want = file_size - received;
            if (want > 4096) want = 4096;
            n = xStreamBufferReceive(s_cdc_rx_stream, buf, want, pdMS_TO_TICKS(5000));
            if (n == 0) {
                printf("[CDC_RX] Timeout at %lu/%lu\n",
                       (unsigned long)received, (unsigned long)file_size);
                break;
            }
            write(fd, buf, n);
            received += n;
            if (received % (64 * 1024) < n) {
                printf("[CDC_RX] %lu/%lu KB\n",
                       (unsigned long)(received / 1024),
                       (unsigned long)(file_size / 1024));
            }
        }

        close(fd);
        free(buf);

        if (received == file_size) {
            printf("[CDC_RX] OK: %s (%lu bytes)\n", path, (unsigned long)file_size);
            /* Send confirmation to host */
            const char *ok_msg = "OK\n";
            ep_tx_busy_flag = true;
            L1C_DCACHE_CLEAN_RANGE((uintptr_t)ok_msg, 3);
            usbd_ep_start_write(CDC_IN_EP, (uint8_t *)ok_msg, 3);
            while (ep_tx_busy_flag);
        } else {
            printf("[CDC_RX] FAIL: %lu/%lu bytes\n",
                   (unsigned long)received, (unsigned long)file_size);
        }
    }

    vTaskDelete(NULL);
}

/* Modified bulk_out: route to stream buffer when rx active */
static void cdc_rx_feed(const uint8_t *data, uint32_t len)
{
    if (s_cdc_rx_stream && s_cdc_rx_active) {
        xStreamBufferSend(s_cdc_rx_stream, data, len, pdMS_TO_TICKS(100));
    }
}

static int cmd_cdc_rx_start(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (s_cdc_rx_active) {
        printf("CDC RX already active\n");
        return 0;
    }

    if (!s_cdc_rx_stream) {
        s_cdc_rx_stream = xStreamBufferCreate(32768, 1);
        if (!s_cdc_rx_stream) {
            printf("Stream buffer alloc failed\n");
            return -1;
        }
    }

    s_cdc_rx_active = 1;
    xTaskCreate(cdc_rx_file_task, "cdc_rx", 2048, NULL,
                configMAX_PRIORITIES - 4, NULL);

    printf("CDC file receive mode enabled. Send file from host with:\n");
    printf("  python3 tools/sdcard_transfer.py write <CDC_PORT> <LOCAL> <REMOTE>\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_rx_start, usbd_cdc_recv, Enable CDC file receive from host);
