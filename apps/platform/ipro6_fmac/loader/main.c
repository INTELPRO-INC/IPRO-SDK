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
 * IPRO6S FMAC loader. Runs from XIP with its RAM in WRAM, enumerates
 * SDIO as class 0xA6 and USB as 0x3896:0x1062, takes the FMAC FW into
 * OCRAM over the download protocol (fmac_dl.h) on either bus and jumps
 * to it. Both sessions stay up across the jump. The protocol itself is
 * fmac_dl_loader.c; this file is the SDK platform it runs on.
 */
#include "hal_wdt.h"
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ipro6.h"
#include "hbn_reg.h"
#include "drv_hbn.h"
#include "hal_hbnram_mgnt.h"
#include "hal_sys.h"
#include "ipro_vendor_ids.h"
#include "platform_common.h"
#include "fmac_dl_loader.h"
#include "fmac_smid.h"
#include "fmac_usb.h"

#define LOADER_VER          1u

/* bsp/board/ipro6/board.c: HBN RAM override of the 32 kHz source */
#define CLK32K_OVR_MAGIC    0xC32B0000u
#define CLK32K_OVR_XTAL     1u
#define CLK32K_OVR_RC       2u

/* Console line time before the FW re-inits the UART */
#define UART_DRAIN_MS       5u
/* USB attach this long after the loader starts */
#define USB_ATTACH_DELAY_MS 2000u
/* USB diagnostics period while attached and not configured */
#define USB_DIAG_MS         2000u

void setup_heap(void);

extern const uint8_t _ld_symbol_rftlv_address[];

static const fmac_dl_bus_t s_bus[] = {
    { FMAC_BUS_SDIO, fmac_smid_next, fmac_smid_release, fmac_smid_send,
      fmac_smid_tx_idle },
    { FMAC_BUS_USB, fmac_usb_next, fmac_usb_release, fmac_usb_send,
      fmac_usb_tx_idle },
};

static void handoff_fill(uint32_t bus)
{
    fmac_handoff_t *ho = (fmac_handoff_t *)FMAC_HANDOFF_ADDR;
    const uint32_t *tlv = (const uint32_t *)_ld_symbol_rftlv_address;

    memset(ho, 0, sizeof(*ho));
    ho->magic = FMAC_HANDOFF_MAGIC;
    ho->ver = FMAC_HANDOFF_VER;
    ho->size = sizeof(*ho);
    ho->bus = bus;
    ho->loader_ver = LOADER_VER;
    ho->sdio_blk = fmac_smid_blk();
    /* Slot head IPRF PARA: iprocube patched the board TLV after it */
    if (tlv[0] == 0x46525049u && tlv[1] == 0x41524150u) {
        memcpy(ho->rftlv, tlv, FMAC_RFTLV_MAX);
        ho->rftlv_len = FMAC_RFTLV_MAX;
        ho->rftlv_crc = fmac_crc32(0, ho->rftlv, FMAC_RFTLV_MAX);
    }
}

static void __attribute__((noreturn)) jump(uint32_t entry, uint32_t bus)
{
    handoff_fill(bus);
    fmac_smid_quiesce();
    __disable_irq();
    for (uint32_t i = 0; i < __ECLIC_INTNUM; i++) {
        ECLIC_DisableIRQ((IRQn_Type)i);
        ECLIC_ClearPendingIRQ((IRQn_Type)i);
    }
    MFlushInvalDCache();
    MInvalICache();
    __FENCE_I();
    ((void (*)(void))(uintptr_t)entry)();
    for (;;)
        ;
}

/* Lets the FW keep the 32 kHz source this boot resolved, without its
 * 3 s crystal probe */
static void clk32k_pin(void)
{
    uint32_t sel = (readl(HBN_BASE + HBN_GLB_OFFSET) & HBN_F32K_SEL_MSK) >>
                   HBN_F32K_SEL_POS;

    *(volatile uint32_t *)HAL_HBNRAM_CLK32K_OVR_ADDR =
        CLK32K_OVR_MAGIC |
        (sel == HBN_32K_XTAL ? CLK32K_OVR_XTAL : CLK32K_OVR_RC);
}

static uint32_t plat_now_ms(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

static void plat_log(const fmac_dl_bus_t *b, const fmac_dl_ack_t *ack)
{
    printf("[FMACLD] bus %lu type %u seq %lu status %u crc %08lx rearm %lu\r\n",
           (unsigned long)b->id, (unsigned)ack->type,
           (unsigned long)ack->seq, (unsigned)ack->status,
           (unsigned long)ack->value, (unsigned long)fmac_smid_rearms());
}

static void plat_reboot(void)
{
    hal_reboot_media();
}

static void plat_jump(uint32_t entry, uint32_t bus)
{
    printf("[FMACLD] jump 0x%08lx\r\n", (unsigned long)entry);
    vTaskDelay(pdMS_TO_TICKS(UART_DRAIN_MS));
    jump(entry, bus);
}

static const fmac_dl_plat_t s_plat = {
    .loader_ver = LOADER_VER,
    .now_ms = plat_now_ms,
    .poll = fmac_smid_poll,
    .log = plat_log,
    .reboot = plat_reboot,
    .jump = plat_jump,
};

static void loader_task(void *arg)
{
    TickType_t diag_at = xTaskGetTickCount() + pdMS_TO_TICKS(USB_ATTACH_DELAY_MS);

    (void)arg;
    fmac_dl_loader_init(&s_plat);
    fmac_usb_init(FMAC_USB_PID, USB_ATTACH_DELAY_MS);
    fmac_smid_init(0);
    printf("[FMACLD] v%u up: SDIO FBR1 0x%03x, USB %04x:%04x in %u ms\r\n",
           (unsigned)LOADER_VER, (unsigned)FMAC_SMID_FBR1_IDENT,
           (unsigned)IPRO_USB_VID, (unsigned)FMAC_USB_PID,
           (unsigned)USB_ATTACH_DELAY_MS);
    TickType_t last_pkt = 0;
    int quiet_diag = 0;

    for (;;) {
        int idle = 1;

        if (last_pkt && !quiet_diag &&
            (int32_t)(xTaskGetTickCount() - last_pkt) >= (int32_t)pdMS_TO_TICKS(1500)) {
            fmac_smid_diag("quiet");
            quiet_diag = 1;
        }
        fmac_usb_poll();
        if ((int32_t)(xTaskGetTickCount() - diag_at) >= 0) {
            diag_at += pdMS_TO_TICKS(USB_DIAG_MS);
            fmac_usb_diag("FMACLD");
        }

        fmac_smid_poll();
        for (unsigned i = 0; i < sizeof(s_bus) / sizeof(s_bus[0]); i++) {
            if (fmac_dl_loader_serve(&s_bus[i])) {
                last_pkt = xTaskGetTickCount();
                quiet_diag = 0;
                idle = 0;
            }
        }
        if (idle)
            taskYIELD();
    }
}

static StackType_t s_stack[1024];
static StaticTask_t s_task;

int main(void)
{
    /* A watchdog left by the previous image keeps running across its
     * reset */
    wdt_disable();
    platform_init(0);
    clk32k_pin();
    setup_heap();
    xTaskCreateStatic(loader_task, "fmacld", sizeof(s_stack) / 4, NULL, 10,
                      s_stack, &s_task);
    vTaskStartScheduler();
    return 0;
}
