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

/*
 * OTA staging over UART zmodem.
 *
 * `ota_zmodem` receives an OTA package (tools/create_ota.py: 512B header +
 * XZ-compressed whole_img) from the host over the console pins (GPIO28/29) via
 * the zmodem component, streams it to FW-B (ota_common), validates it, then
 * flips the FW partition's activeIndex and reboots so boot2 decompresses
 * FW-B -> FW-A. The transport-agnostic core (FW-B sink, SHA256 validate, PT
 * apply) lives in ota_common; this file is only the UART/zmodem transport.
 *
 * UART handoff: the transfer runs on UART1 @2M routed onto the same console
 * pads via the IPRO6 UART signal mux (GPIO28->SIG4, GPIO29->SIG5). UART0
 * (shell/log @115200) is left untouched; its pads are re-pointed at UART1 for
 * the duration and restored afterwards (success OR failure).
 */
#include <generated/autoconf.h>

#if defined(CONFIG_COMPONENTS_ZMODEM_ENABLE)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "shell.h"
#include "hal_uart.h"
#include "hal_flash.h"
#include "hal_sys.h"
#include "drv_glb.h"
#include "drv_uart.h"
#include "zmodem.h"
#include "ota_common.h"

/* GPIO28 -> UART sig 4, GPIO29 -> UART sig 5 (sig = gpio % 8). See board.c. */
#define OTA_ZM_SIG_TX       GLB_UART_SIG_4
#define OTA_ZM_SIG_RX       GLB_UART_SIG_5

#define OTA_ZM_BAUD         2000000
#define OTA_ZM_TIMEOUT_MS   30000

/* zmodem subpacket buffer. Sized for large ZCRCQ subpackets (fewer ACK
 * round-trips -> higher throughput on a high-latency USB-serial bridge). Must
 * exceed the host sender's subpacket; 16 KB is the swept throughput knee
 * (~111 KB/s @2M); tools/ipro_sz.py defaults to a matching 16384-byte subpacket. */
#define OTA_ZM_DATA_BUF     16896

static uint8_t *s_data_buf;      /* malloc'd per command */

/* ---- UART pad handoff (console pins between UART0 and UART1) ---- */

static void uart_tx_drain(int uart_id)
{
    /* Wait until the TX shift register is idle, so re-pointing the pad away
     * from this UART doesn't truncate the last byte. Bounded spin. */
    uint32_t spin = 0;
    while (UART_GetTxBusBusyStatus(uart_id) && ++spin < 2000000)
        ;
}

static void console_pads_to_uart1(void)
{
    uart_tx_drain(UART0_INDEX);
    GLB_UART_Fun_Sel_Swap(OTA_ZM_SIG_TX, GLB_UART_SIG_FUN_UART1_TXD);
    GLB_UART_Fun_Sel_Swap(OTA_ZM_SIG_RX, GLB_UART_SIG_FUN_UART1_RXD);
}

static void console_pads_to_uart0(void)
{
    uart_tx_drain(UART1_INDEX);
    GLB_UART_Fun_Sel_Swap(OTA_ZM_SIG_TX, GLB_UART_SIG_FUN_UART0_TXD);
    GLB_UART_Fun_Sel_Swap(OTA_ZM_SIG_RX, GLB_UART_SIG_FUN_UART0_RXD);
}

/* ---- zmodem receive state machine (silent: pads are on UART1, no printf) ----
 * Reuses the component's protocol primitives; data goes to ota_sink(). Returns
 * 0 on a completed transfer (ZFIN), negative otherwise. Our zm_send (UART)
 * never fails, so header replies aren't checked. */
static int zmodem_receive(void)
{
    uint8_t  rzr_buf[4];
    uint16_t result, count;
    ZHDR hdr;

    /* lrzsz `sz` auto-starts the remote receiver by sending "rz\r" first */
    if (zm_await("rz\r", (char *)rzr_buf, sizeof(rzr_buf)) != OK)
        return -1;

    while (true) {
        result = zm_await_header(&hdr);
        if (result == CANCELLED || result == CLOSED)
            return -2;                  /* remote cancelled, or idle timeout */
        if (IS_ERROR(result)) {         /* noise / bad CRC -> ask to resend  */
            zm_send_pos_hdr(ZNAK, ota_recv_len());
            continue;
        }

        switch (hdr.type) {
        case ZRQINIT:
            zm_send_flags_hdr(ZRINIT, CANOVIO | CANFC32, 0, 0, 0);
            continue;

        case ZSINIT:
            zm_send_pos_hdr(ZACK, ota_recv_len());
            continue;

        case ZFILE:
            /* filename subpacket -- we get size from the OTA header, ignore */
            count  = OTA_ZM_DATA_BUF;
            result = zm_read_data_block(s_data_buf, &count);
            if (result == CANCELLED || result == CLOSED)
                return -2;
            if (!IS_ERROR(result))
                zm_send_pos_hdr(ZRPOS, ota_recv_len());
            continue;

        case ZDATA:
            while (true) {
                count  = OTA_ZM_DATA_BUF;
                result = zm_read_data_block(s_data_buf, &count);
                if (result == CANCELLED || result == CLOSED)
                    return -2;
                if (IS_ERROR(result)) {
                    /* bad subpacket: ask sender to resume from our position */
                    zm_send_pos_hdr(ZRPOS, ota_recv_len());
                    break;
                }
                /* count includes a trailing framing byte -> data is count-1 */
                if (count > 0 && ota_sink(s_data_buf, count - 1) != 0)
                    return -4;

                if (result == GOT_CRCE) {
                    break;                  /* end of frame, header follows  */
                } else if (result == GOT_CRCG) {
                    continue;               /* streaming, more data follows  */
                } else if (result == GOT_CRCQ || result == GOT_CRCW) {
                    zm_send_pos_hdr(ZACK, ota_recv_len());   /* ACK subpacket */
                    if (result == GOT_CRCW)
                        break;
                }
            }
            continue;

        case ZEOF:
            zm_send_flags_hdr(ZRINIT, CANOVIO | CANFC32, 0, 0, 0);
            continue;

        case ZFIN:
            zm_send_pos_hdr(ZFIN, 0);
            return 0;                       /* transfer complete             */

        default:
            continue;                       /* ignore unknown header types   */
        }
    }
}

int cmd_ota_zmodem(int argc, char **argv)
{
    uint32_t fw_b_addr = 0, fw_b_max = 0;
    bool reboot = false;
    int rc = -1;

    /* `ota_zmodem noapply`: receive + validate only (for throughput tuning) --
     * leave FW-B staged but do not flip activeIndex / reboot. */
    bool noapply = (argc >= 2 && strcmp(argv[1], "noapply") == 0);

    if (ota_get_fw_b(&fw_b_addr, &fw_b_max) != 0) {
        printf("ota_zmodem: no valid partition table / FW entry\r\n");
        return -1;
    }

    s_data_buf = malloc(OTA_ZM_DATA_BUF);
    if (!s_data_buf || ota_sink_begin(fw_b_addr, fw_b_max) != 0) {
        printf("ota_zmodem: out of memory\r\n");
        goto cleanup;
    }

    printf("ota_zmodem: erasing FW-B @ 0x%lx (max 0x%lx)...\r\n",
           (unsigned long)fw_b_addr, (unsigned long)fw_b_max);
    if (flash_erase(fw_b_addr, fw_b_max) != SUCCESS) {
        printf("ota_zmodem: FW-B erase failed\r\n");
        goto cleanup;
    }

    /* Bring up UART1 @2M (pads still on UART0 for now). */
    uart_set_baudrate(UART1_INDEX, OTA_ZM_BAUD);
    uart_init(UART1_INDEX, 0);
    zm_port_set_uart(UART1_INDEX);
    zm_port_set_timeout_ms(OTA_ZM_TIMEOUT_MS);

    printf("ota_zmodem: switch your terminal to %d 8N1 and send the .ota "
           "package with sz now (%ds timeout)...\r\n",
           OTA_ZM_BAUD, OTA_ZM_TIMEOUT_MS / 1000);

    /* Hand the console pads to UART1 @2M, run zmodem, then ALWAYS hand them
     * back to UART0 @115200 -- success or failure. No printf in between. */
    console_pads_to_uart1();
    rc = zmodem_receive();
    if (rc == 0)
        rc = ota_sink_flush();
    console_pads_to_uart0();

    if (rc != 0) {
        printf("\r\nota_zmodem: transfer failed (rc=%d%s) -- FW-A untouched\r\n",
               rc, ota_overflowed() ? ", package > FW-B slot" : "");
        goto cleanup;
    }

    rc = ota_validate();
    if (rc != 0) {
        printf("ota_zmodem: received %lu bytes but package invalid (rc=%d) -- "
               "FW-A untouched\r\n", (unsigned long)ota_recv_len(), rc);
        goto cleanup;
    }

    printf("ota_zmodem: received %lu bytes, package OK (SHA verified).\r\n",
           (unsigned long)ota_recv_len());

    if (noapply) {
        printf("ota_zmodem: noapply -- FW-B staged, activeIndex NOT flipped\r\n");
        goto cleanup;
    }

    if (ota_mark_fwb_active() != 0) {
        printf("ota_zmodem: partition activeIndex update failed\r\n");
        rc = -1;
        goto cleanup;
    }
    reboot = true;

cleanup:
    free(s_data_buf);
    s_data_buf = NULL;
    ota_sink_end();

    if (reboot) {
        printf("ota_zmodem: rebooting to apply OTA...\r\n");
        hal_reboot();
    }
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ota_zmodem, ota_zmodem,
                       receive OTA package via zmodem on UART1 @2M and apply);

#endif /* CONFIG_COMPONENTS_ZMODEM_ENABLE */
