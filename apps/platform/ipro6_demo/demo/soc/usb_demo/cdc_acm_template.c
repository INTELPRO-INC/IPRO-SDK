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
#include "FreeRTOS.h"
#include "task.h"
#include "drv_glb.h"

//#define CDC_THROUGHPUT_TEST_SUPPORT
/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0601
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

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t read_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t write_buffer[2048];
/* Staging buffer for transfer headers: USB DMA can only read DMA-coherent
 * (non-cacheable) memory, so small headers built on the stack must be copied
 * here before being sent. */
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t hdr_buffer[16];

static volatile bool ep_tx_busy_flag = false;

volatile static int gCDCEchoTest = 0;
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

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
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

int cmd_cdc_acm_init(int argc, char **argv)
{
    const uint8_t data[10] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };

    memcpy(&write_buffer[0], data, 10);
    memset(&write_buffer[10], 'a', 2038);

    usbd_deinitialize();
    usbd_desc_register(cdc_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);
    usbd_event_register_callback(cdc_event_handler);
    usbd_initialize();
    printf("cdc_acm_demo done\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_acm_init, usbd_cdc_init, USBD CDC Init);

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
        ep_tx_busy_flag = true;
        usbd_ep_start_write(CDC_IN_EP, write_buffer, 2048);
        while (ep_tx_busy_flag) {
        }
    }
}

__attribute__((unused)) static int cmd_cdc_dtr_test(int argc, char **argv)
{
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

/* Wait (bounded) for the in-flight CDC IN transfer to finish. The bulk-in
 * callback clears ep_tx_busy_flag; if the host never drains, give up after
 * timeout_ms instead of spinning forever. Returns 0 on completion, -1 on timeout. */
static int cdc_acm_wait_tx(uint32_t timeout_ms)
{
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeout_ms);

    while (ep_tx_busy_flag) {
        if ((int32_t)(xTaskGetTickCount() - deadline) >= 0) {
            ep_tx_busy_flag = false;
            return -1;
        }
    }

    return 0;
}

/* Send a buffer that may live in cacheable memory over the CDC IN endpoint by
 * staging it through the non-cacheable write_buffer in chunks (the USB vDMA can
 * only read DMA-coherent memory). Returns 0 on success, -1 on transfer timeout. */
static int cdc_acm_send(const uint8_t *src, uint32_t len)
{
    uint32_t off = 0;

    while (off < len) {
        uint32_t chunk = len - off;

        if (chunk > sizeof(write_buffer)) {
            chunk = sizeof(write_buffer);
        }
        memcpy(write_buffer, src + off, chunk);
        ep_tx_busy_flag = true;
        usbd_ep_start_write(CDC_IN_EP, write_buffer, chunk);
        if (cdc_acm_wait_tx(3000) != 0) {
            return -1;
        }
        off += chunk;
    }

    return 0;
}

int cmd_cdc_acm_send_file(int argc, char **argv)
{
    void *buf_addr = NULL;
    int fd;
    char *path;
    struct stat st;
    uint32_t file_size, path_len, trans_type = 0;

    if (argc != 2) {
        printf("Usage: cdc_acm_send_file <file_path>\r\n");
        return 0;
    }

    path = argv[1];

    if (stat(path, &st) != 0) {
        printf("stat %s fail\r\n", path);
        return 0;
    }
    file_size = st.st_size;
    buf_addr = malloc(file_size);

    printf("Opening file %s\r\n", path);
    fd = open(path, O_RDONLY);
    if (read(fd, buf_addr, file_size) != file_size) {
        printf("read fail\r\n");
    } else {
        /* Header: type(4) + size(4) + path_len(4), staged in non-cacheable memory. */
        path_len = strlen(path);
        memcpy(&hdr_buffer[0], &trans_type, 4);
        memcpy(&hdr_buffer[4], &file_size, 4);
        memcpy(&hdr_buffer[8], &path_len, 4);
        ep_tx_busy_flag = true;
        usbd_ep_start_write(CDC_IN_EP, hdr_buffer, 12);
        if (cdc_acm_wait_tx(3000) != 0) {
            printf("USB CDC TX timeout\r\n");
        } else {
            /* path (argv) and the file data (heap) are cacheable; cdc_acm_send
             * stages them through non-cacheable memory for the USB vDMA. */
            printf("Write Start\r\n");
            if (cdc_acm_send((const uint8_t *)path, path_len) != 0 ||
                cdc_acm_send(buf_addr, file_size) != 0) {
                printf("USB CDC TX timeout\r\n");
            } else {
                printf("Write Finished\r\n");
            }
        }
    }
    free(buf_addr);
    close(fd);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_acm_send_file, usbd_cdc_sendfile, USBD CDC Send File);

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

    /* Header: type(4) + size(4) + start_addr(4), staged in non-cacheable memory. */
    memcpy(&hdr_buffer[0], &trans_type, 4);
    memcpy(&hdr_buffer[4], &mem_size, 4);
    memcpy(&hdr_buffer[8], &start_addr, 4);
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, hdr_buffer, 12);
    if (cdc_acm_wait_tx(3000) != 0) {
        printf("USB CDC TX timeout\r\n");
        return 0;
    }

    printf("Sending memory data...\r\n");
    /* cdc_acm_send stages the region through non-cacheable memory, so any source
     * (including cacheable RAM, not just XIP flash) is sent correctly. */
    if (cdc_acm_send(mem_ptr, mem_size) != 0) {
        printf("USB CDC TX timeout\r\n");
        return 0;
    }

    printf("Memory send completed\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_acm_send_mem, usbd_cdc_sendmem, USBD CDC Send Memory);

/* reboot_to_usb_dl [5|inf]: request a LONGER boot2 USB-DL window than the 500ms
 * default that every boot already offers. Use it for manual flashing when you
 * can't catch the short default window. no-arg / "inf" = wait forever, "5" = 5s.
 * The request lives in HBN_RSV3 (survives reset): [31:16] = magic "US" (0x5553),
 * [15:8] = timeout code (0x02=5s, 0xFF=inf); [7:0] is the BROM XTAL byte. */
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
