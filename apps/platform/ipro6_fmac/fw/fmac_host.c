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
 * FMAC host link: keeps the SDIO or USB session the loader left, sends
 * READY, serves the download protocol INFO / REBOOT, and moves KE
 * messages between the host and macsw (macif_bus.c) as FMAC_T_MSG frames.
 */
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "hal_sys.h"
#include "ipro_irq.h"
#include "fmac_dl.h"
#include "fmac_host.h"
#include "fmac_bus_pool.h"
#include "compiler/common.h"
#include "fmac_smid.h"
#include "fmac_usb.h"
#include "fmac_wire.h"
#include "macif_bus.h"

/* FW -> host frames waiting for the bus, bytes */
#define RING_BYTES      4096u
/* Ring record type: the rest of the ring is free, continue at its start */
#define RING_WRAP       0xFFFFu
#define BUS_TASK_PRIO   10
#ifdef CONFIG_IPRO6_FMAC_AP
#define BUS_STACK_WORDS 512
#else
#define BUS_STACK_WORDS 768
#endif
#define DIAG_MS         5000u
/* USB REBOOT: stay on the bus this long after the ack */
#define USB_REBOOT_DELAY_MS 500u
/* USB REBOOT: off the bus this long before the reset */
#define USB_REBOOT_DETACH_MS 200u

typedef struct {
    int (*next)(uint8_t **pkt, uint32_t *len);
    void (*release)(uint8_t *pkt);
    int (*send)(const void *msg, uint32_t len);
    int (*tx_idle)(void);
    void (*poll)(void);
    uint32_t (*dnld_slots)(void);
    uint8_t *(*upld_buf)(uint32_t *max);
    int (*tx_done)(void);
    const char *name;
} bus_t;

static void usb_poll(void)
{
    fmac_usb_poll();
}

static const bus_t s_sdio = { fmac_smid_next, fmac_smid_release,
                              fmac_smid_send, fmac_smid_tx_idle,
                              fmac_smid_poll, fmac_smid_dnld_slots,
                              fmac_smid_upld_buf, fmac_smid_tx_done, "SDIO" };
static const bus_t s_usb = { fmac_usb_next, fmac_usb_release,
                             fmac_usb_send, fmac_usb_tx_idle,
                             usb_poll, fmac_usb_dnld_slots,
                             fmac_usb_upld_buf, fmac_usb_tx_done, "USB" };
static const bus_t *s_bus;
/* Frames ready for the bus, fmac_hdr_t + body, 4-byte aligned. The
 * producers write at s_head under the IRQ lock, the bus task reads at
 * s_tail; s_head never reaches s_tail from behind */
static uint8_t s_ring[RING_BYTES] __attribute__((aligned(4)));
static volatile uint32_t s_head, s_tail;
static TaskHandle_t s_task;
/* Bus task loop count and the step it is in, for the monitor task */
static volatile uint32_t s_loops, s_where;
static StackType_t s_stack[BUS_STACK_WORDS];
static StaticTask_t s_tcb;
ATTR_NOCACHE_RAM_SECTION fmac_dnld_pool_t fmac_dnld_pool __attribute__((aligned(32)));
ATTR_NOCACHE_RAM_SECTION fmac_upld_pool_t fmac_upld_pool __attribute__((aligned(32)));
_Static_assert(FMAC_SMID_UPLD_MAX <= FMAC_UPLD_MAX &&
               FMAC_USB_IN_MAX + 4 <= FMAC_UPLD_MAX, "upload fits a host buffer");

/* Host transfers handled, and the count the host last heard */
static uint16_t s_dnld_done, s_dnld_told;
/* TX slots freed since boot, reported or not; host frames dropped
 * without a slot and not yet reported as freed */
static uint16_t s_tx_freed, s_tx_drop_untold;

/* TX slots freed since the last call; a dropped host frame counts */
static uint16_t tx_freed_take(void)
{
    uint16_t n = (uint16_t)macif_bus_tx_freed_take() + s_tx_drop_untold;

    s_tx_drop_untold = 0;
    s_tx_freed += n;
    return n;
}

static struct {
    uint32_t rx_msg, rx_tx, tx_drop, rx_bad, rx_push_fail, rx_dl, rx_spin;
    uint32_t tx_msg, tx_rx, tx_xfer, tx_long, ring_full;
} s_cnt, s_cnt_shown;

static const fmac_handoff_t *handoff(void)
{
    const fmac_handoff_t *ho = (const fmac_handoff_t *)FMAC_HANDOFF_ADDR;

    if (ho->magic != FMAC_HANDOFF_MAGIC || ho->ver != FMAC_HANDOFF_VER)
        return NULL;
    return ho;
}

uint32_t fmac_rftlv_addr(void)
{
    const fmac_handoff_t *ho = handoff();

    if (!ho || !ho->rftlv_len || ho->rftlv_len > FMAC_RFTLV_MAX ||
        fmac_crc32(0, ho->rftlv, ho->rftlv_len) != ho->rftlv_crc)
        return 0;
    return (uint32_t)(uintptr_t)ho->rftlv;
}

static void fill_id(fmac_dl_ack_t *ack)
{
    ack->value = FMAC_STAGE_FW;
    ack->info[0] = FMAC_DL_PROTO_VER | (FMAC_FW_VER << 16);
    ack->info[1] = FMAC_DL_WIN_BASE;
    ack->info[2] = FMAC_DL_WIN_SIZE;
    ack->info[3] = FMAC_FW_VER;
}

static void send_ready(void)
{
    fmac_dl_ack_t ack;

    memset(&ack, 0, sizeof(ack));
    ack.magic = FMAC_DL_ACK_MAGIC;
    ack.type = FMAC_DL_READY;
    fill_id(&ack);
    s_bus->send(&ack, sizeof(ack));
}

static void dl_request(const fmac_dl_req_t *r, uint32_t len)
{
    fmac_dl_ack_t ack;
    int reboot = 0;

    memset(&ack, 0, sizeof(ack));
    ack.magic = FMAC_DL_ACK_MAGIC;
    if (len < FMAC_DL_HDR_LEN) {
        ack.status = FMAC_DL_E_HDR;
    } else {
        ack.type = r->type;
        ack.seq = r->seq;
        if (r->type == FMAC_DL_INFO)
            fill_id(&ack);
        else if (r->type == FMAC_DL_REBOOT)
            reboot = 1;
        else
            ack.status = FMAC_DL_E_TYPE;
    }
    s_bus->send(&ack, sizeof(ack));
    if (reboot) {
        for (int i = 0; i < 100000 && !s_bus->tx_done(); i++)
            s_bus->poll();
        printf("[FMACFW] reboot\r\n");
        /* USB: stays on the bus while the host finishes unbinding, then
         * leaves it before the reset */
        if (s_bus == &s_usb) {
            vTaskDelay(pdMS_TO_TICKS(USB_REBOOT_DELAY_MS));
            fmac_usb_detach();
            vTaskDelay(pdMS_TO_TICKS(USB_REBOOT_DETACH_MS));
        }
        hal_reboot_media();
    }
}

/* Copies one frame into the ring; 0 when it has no room */
static int ring_put(uint16_t type, const void *body, uint32_t len)
{
    uint32_t need = FMAC_ALIGN4(FMAC_HDR_LEN + len);
    uintptr_t flag = ipro_irq_save();
    uint32_t head = s_head, tail = s_tail;
    fmac_hdr_t *h;

    if (head >= tail && RING_BYTES - head < need + (tail == 0 ? 4u : 0u)) {
        /* No room up to the end: wrap when the start has room */
        if (tail <= need + 4u) {
            ipro_irq_restore(flag);
            return 0;
        }
        if (head < RING_BYTES)
            ((fmac_hdr_t *)(s_ring + head))->type = RING_WRAP;
        head = 0;
    }
    if (head < tail && tail - head < need + 4u) {
        ipro_irq_restore(flag);
        return 0;
    }
    h = (fmac_hdr_t *)(s_ring + head);
    h->len = (uint16_t)len;
    h->type = type;
    h->queue = 0;
    h->rsv = 0;
    memcpy(h + 1, body, len);
    head += need;
    s_head = head == RING_BYTES ? 0 : head;
    ipro_irq_restore(flag);
    return 1;
}

/* Efuse MAC; a blank efuse gets a random locally administered one */
static void info_mac(uint8_t mac[6])
{
    extern int mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload);
    extern int Sec_Eng_Trng_Enable(void);
    extern int Sec_Eng_Trng_Get_Random(uint8_t *data, uint32_t len);
    static uint8_t s_mac[6];
    static int s_have;

    if (!s_have) {
        if (mfg_media_read_macaddr_with_lock(s_mac, 1) != 0) {
            Sec_Eng_Trng_Enable();
            Sec_Eng_Trng_Get_Random(s_mac, 6);
            s_mac[0] = (s_mac[0] & 0xfe) | 0x02;
        }
        s_have = 1;
    }
    memcpy(mac, s_mac, 6);
}

static void send_info(void)
{
    fmac_info_t info;

    memset(&info, 0, sizeof(info));
    info_mac(info.mac);
    info.tx_slots = (uint16_t)macif_bus_tx_slots();
    info.fw_ver = FMAC_FW_VER | FMAC_FW_CAPS;
    info.dnld_slots = (uint16_t)s_bus->dnld_slots();
    /* + 1: the INFO request is handled once this returns */
    info.dnld_done = s_dnld_done + 1;
    tx_freed_take();
    info.tx_taken = (uint16_t)(s_cnt.rx_tx + s_cnt.tx_drop);
    info.tx_freed = s_tx_freed;
    if (!ring_put(FMAC_T_INFO, &info, sizeof(info)))
        s_cnt.ring_full++;
}

static void rx_frames(const uint8_t *pkt, uint32_t len)
{
    uint32_t off = 0;

    while (off + FMAC_HDR_LEN <= len) {
        const fmac_hdr_t *h = (const fmac_hdr_t *)(pkt + off);

        if (h->type == FMAC_T_END)
            break;
        if (h->len > len - off - FMAC_HDR_LEN) {
            s_cnt.rx_bad++;
            break;
        }
        if (h->type == FMAC_T_MSG) {
            s_where = 10;
            s_cnt.rx_msg++;
            if (macif_bus_msg_push(pkt + off + FMAC_HDR_LEN, h->len))
                s_cnt.rx_push_fail++;
        } else if (h->type == FMAC_T_INFO) {
            s_where = 11;
            send_info();
        } else if (h->type == FMAC_T_TX && h->len > sizeof(fmac_tx_hdr_t)) {
            const uint8_t *body = pkt + off + FMAC_HDR_LEN;

            s_where = 12;
            if (macif_bus_tx_push(body, body + sizeof(fmac_tx_hdr_t),
                                  h->len - sizeof(fmac_tx_hdr_t))) {
                s_cnt.tx_drop++;
                s_tx_drop_untold++;
            } else {
                s_cnt.rx_tx++;
            }
#ifdef CONFIG_IPRO6_FMAC_AP
        } else if (h->type == FMAC_T_MGMT && h->len > sizeof(fmac_mgmt_hdr_t)) {
            s_where = 13;
            if (macif_bus_mgmt_push(pkt + off + FMAC_HDR_LEN, h->len)) {
                s_cnt.tx_drop++;
                s_tx_drop_untold++;
            } else {
                s_cnt.rx_tx++;
            }
#endif
        } else {
            s_cnt.rx_bad++;
        }
        off += FMAC_ALIGN4(FMAC_HDR_LEN + h->len);
    }
}

static int rx_one(void)
{
    uint8_t *pkt;
    uint32_t len;

    if (!s_bus->next(&pkt, &len))
        return 0;
    if (len >= 4 && *(const uint32_t *)pkt == FMAC_DL_REQ_MAGIC) {
        s_cnt.rx_dl++;
        /* REBOOT does not return */
        dl_request((const fmac_dl_req_t *)pkt, len);
    } else {
        rx_frames(pkt, len);
    }
    s_bus->release(pkt);
    s_dnld_done++;
    return 1;
}

/* Appends received frames to the transfer buf at off while they fit */
static uint32_t rx_pack(uint8_t *buf, uint32_t max, uint32_t off)
{
    fmac_rx_hdr_t rh;
    const uint8_t *frame;
    uint32_t len, id;

    while (macif_bus_rx_peek(&rh, &frame, &len, &id)) {
        uint32_t body = sizeof(rh) + len;
        uint32_t need = FMAC_ALIGN4(FMAC_HDR_LEN + body);
        fmac_hdr_t *h = (fmac_hdr_t *)(buf + off);

        if (off + need > max)
            break;
        h->len = (uint16_t)body;
        h->type = FMAC_T_RX;
        h->queue = 0;
        h->rsv = 0;
        memcpy(buf + off + FMAC_HDR_LEN, &rh, sizeof(rh));
        memcpy(buf + off + FMAC_HDR_LEN + sizeof(rh), frame, len);
        off += need;
        macif_bus_rx_done(id);
        s_cnt.tx_rx++;
    }
    return off;
}

/* Packs ring frames and RX frames straight into the bus upload buffer */
static int tx_xfer(void)
{
    uint32_t off = 0, max;
    fmac_txcfm_t cfm = { 0, 0 };
    uint8_t *buf;

    if (!s_bus->tx_idle())
        return 0;
    buf = s_bus->upld_buf(&max);
    cfm.freed = tx_freed_take();
    if (cfm.freed && !ring_put(FMAC_T_TXCFM, &cfm, sizeof(cfm)))
        s_cnt.ring_full++;
    if (s_dnld_told != s_dnld_done) {
        fmac_flow_t fl = { s_dnld_done, (uint16_t)s_bus->dnld_slots() };

        if (ring_put(FMAC_T_FLOW, &fl, sizeof(fl)))
            s_dnld_told = fl.dnld_done;
    }

    while (s_tail != s_head) {
        const fmac_hdr_t *h = (const fmac_hdr_t *)(s_ring + s_tail);
        uint32_t need;

        if (h->type == RING_WRAP) {
            s_tail = 0;
            continue;
        }
        need = FMAC_ALIGN4(FMAC_HDR_LEN + h->len);
        if (off + need > max)
            break;
        memcpy(buf + off, h, need);
        off += need;
        s_tail = s_tail + need == RING_BYTES ? 0 : s_tail + need;
        s_cnt.tx_msg++;
    }
    s_where = 20;
    off = rx_pack(buf, max, off);
    if (!off)
        return 0;
    if (off + FMAC_HDR_LEN <= max) {
        memset(buf + off, 0, FMAC_HDR_LEN);
        off += FMAC_HDR_LEN;
    }
    s_where = 21;
    s_bus->send(buf, off);
    s_cnt.tx_xfer++;
    return 1;
}

/* CPU share of a few tasks since the last call, percent */
static void cpu_diag(void)
{
    static const char *const names[] = { "WiFi task", "fmachost", "IDLE",
                                         "Tmr Svc", "fmacmon" };
    static uint32_t prev[5], prev_total;
    uint32_t total, d_total, pct[5];

    __asm__ volatile("csrr %0, mcycle" : "=r"(total));
    d_total = total - prev_total;
    prev_total = total;
    for (int i = 0; i < 5; i++) {
        TaskHandle_t t = xTaskGetHandle(names[i]);
        TaskStatus_t st;
        uint32_t d;

        pct[i] = 0;
        if (!t)
            continue;
        vTaskGetInfo(t, &st, pdFALSE, eInvalid);
        d = st.ulRunTimeCounter - prev[i];
        prev[i] = st.ulRunTimeCounter;
        pct[i] = d_total ? (uint32_t)((uint64_t)d * 100 / d_total) : 0;
    }
    printf("[FMACFW] cpu%%: wifi %lu bus %lu idle %lu tmr %lu mon %lu\r\n",
           (unsigned long)pct[0], (unsigned long)pct[1], (unsigned long)pct[2],
           (unsigned long)pct[3], (unsigned long)pct[4]);
}

static void diag(void)
{
    uint32_t t[9];

    if (!memcmp(&s_cnt, &s_cnt_shown, sizeof(s_cnt)))
        return;
    s_cnt_shown = s_cnt;
    macif_bus_mib_diag();
    cpu_diag();
    if (s_bus == &s_sdio) {
        uint32_t rd, turn, n;

        fmac_smid_timing(&rd, &turn, &n);
        if (n)
            printf("[FMACFW] upld %lu: read %lu us turn %lu us avg\r\n",
                   (unsigned long)n, (unsigned long)(rd / n),
                   (unsigned long)(turn / n));
    }
    macif_bus_rx_diag(t);
    printf("[FMACFW] rx pool full %lu free_min %lu held_max %lu free %lu amsdu chk %lu off %lu\r\n",
           (unsigned long)t[0], (unsigned long)t[1], (unsigned long)t[2],
           (unsigned long)t[3], (unsigned long)t[4], (unsigned long)t[5]);
    macif_bus_tx_diag(t);
    printf("[FMACFW] tx pushed %lu cfm %lu noack %lu retry_drop %lu free %lu"
           " | ac bk %lx be %lx vi %lx vo %lx\r\n",
           (unsigned long)t[0], (unsigned long)t[1], (unsigned long)t[8],
           (unsigned long)t[2], (unsigned long)t[3], (unsigned long)t[4],
           (unsigned long)t[5], (unsigned long)t[6], (unsigned long)t[7]);
    printf("[FMACFW] msg rx %lu tx %lu, frames rx %lu tx %lu drop %lu, in %lu "
           "xfers | dl %lu bad %lu push_fail %lu long %lu ring_full %lu spin %lu\r\n",
           (unsigned long)s_cnt.rx_msg, (unsigned long)s_cnt.tx_msg,
           (unsigned long)s_cnt.tx_rx, (unsigned long)s_cnt.rx_tx,
           (unsigned long)s_cnt.tx_drop,
           (unsigned long)s_cnt.tx_xfer,
           (unsigned long)s_cnt.rx_dl, (unsigned long)s_cnt.rx_bad,
           (unsigned long)s_cnt.rx_push_fail, (unsigned long)s_cnt.tx_long,
           (unsigned long)s_cnt.ring_full, (unsigned long)s_cnt.rx_spin);
}

static StackType_t s_mon_stack[256];
static StaticTask_t s_mon_tcb;

/* Saved PC and RA of a task that is not running (port frame: PC at 0) */
static void mon_task_pc(const char *name)
{
    TaskHandle_t t = xTaskGetHandle(name);
    const uint32_t *sp;

    if (!t)
        return;
    sp = *(uint32_t *const *)t;
    printf("[FMACFW]   %s: state %d prio %lu pc %08lx ra %08lx\r\n", name,
           (int)eTaskGetState(t), (unsigned long)uxTaskPriorityGet(t),
           (unsigned long)sp[0], (unsigned long)sp[1]);
}

/* Free stack words of a task, or -1 */
static long stack_free(const char *name)
{
    TaskHandle_t t = xTaskGetHandle(name);

    return t ? (long)uxTaskGetStackHighWaterMark(t) : -1;
}

static void mem_diag(void)
{
    printf("[FMACFW] heap free %lu min %lu | stack free words: idle %ld tmr %ld wifi %ld bus %ld fw %ld\r\n",
           (unsigned long)xPortGetFreeHeapSize(),
           (unsigned long)xPortGetMinimumEverFreeHeapSize(),
           stack_free("IDLE"), stack_free("Tmr Svc"), stack_free("WiFi task"),
           stack_free("fmachost"), stack_free("fw"));
}

static void mon_task(void *arg)
{
    uint32_t seen = 0, ticks = 0;

    (void)arg;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (++ticks % 30 == 0)
            mem_diag();
        if (s_loops != seen) {
            seen = s_loops;
            continue;
        }
        printf("[FMACFW] bus task stalled: loops %lu where %lu state %d "
               "stack_free %lu tx_idle %d\r\n",
               (unsigned long)s_loops, (unsigned long)s_where,
               (int)eTaskGetState(s_task),
               (unsigned long)uxTaskGetStackHighWaterMark(s_task),
               s_bus->tx_idle());
        if (s_bus == &s_sdio)
            fmac_smid_diag("stall");
        mon_task_pc("fmachost");
        mon_task_pc("WiFi task");
        mon_task_pc("Tmr Svc");
        mon_task_pc("IDLE");
    }
}

/* The upload has been pending since this tick, or 0 */
static TickType_t s_busy_since;

static void stuck_check(void)
{
    TickType_t now = xTaskGetTickCount();

    if (s_bus->tx_done()) {
        s_busy_since = 0;
        return;
    }
    if (!s_busy_since) {
        s_busy_since = now;
        return;
    }
    if (now - s_busy_since >= pdMS_TO_TICKS(1000)) {
        s_busy_since = now;
        if (s_bus == &s_sdio)
            fmac_smid_diag("upld stuck");
        else
            printf("[FMACFW] USB IN stuck\r\n");
    }
}

static void bus_task(void *arg)
{
    TickType_t last = xTaskGetTickCount();

    (void)arg;
    send_ready();
    printf("[FMACFW] v%u on %s, READY sent\r\n", (unsigned)FMAC_FW_VER,
           s_bus->name);
    for (;;) {
        int busy;

        s_loops++;
        s_where = 1;
        s_bus->poll();
        s_where = 2;
        /* Hand every filled download buffer back before sending: the host
         * may only write into a posted one */
        busy = 0;
        for (uint32_t n = 0; rx_one(); n++) {
            busy = 1;
            if (n >= s_bus->dnld_slots()) {
                s_cnt.rx_spin++;
                break;
            }
        }
        s_where = 3;
        busy |= tx_xfer();
        s_where = 4;
        stuck_check();
        if (xTaskGetTickCount() - last >= pdMS_TO_TICKS(DIAG_MS)) {
            last = xTaskGetTickCount();
            diag();
        }
        s_where = 5;
        if (!busy)
            ulTaskNotifyTake(pdTRUE, 1);
    }
}

void fmac_host_msg_fwd(void *msg, const void *body, uint32_t len, bool isr)
{
    if (len > FMAC_BODY_MAX)
        s_cnt.tx_long++;
    else if (!ring_put(FMAC_T_MSG, body, len))
        s_cnt.ring_full++;
    macif_bus_msg_done(msg);
    if (isr) {
        BaseType_t woken = pdFALSE;

        vTaskNotifyGiveFromISR(s_task, &woken);
        portYIELD_FROM_ISR(woken);
    } else {
        xTaskNotifyGive(s_task);
    }
}

/* IRQ context: a transfer landed or completed */
static void bus_notify(void)
{
    BaseType_t woken = pdFALSE;

    if (!s_task)
        return;
    vTaskNotifyGiveFromISR(s_task, &woken);
    portYIELD_FROM_ISR(woken);
}

int fmac_host_init(void)
{
    const fmac_handoff_t *ho = handoff();

    if (ho && ho->bus == FMAC_BUS_USB) {
        s_bus = &s_usb;
        fmac_usb_takeover(FMAC_USB_PID);
    } else {
        /* Ungates the SMID clock board_init gated and keeps the session */
        s_bus = &s_sdio;
        fmac_smid_init(1);
    }
    printf("[FMACFW] handoff %s, loader v%lu, rftlv %s\r\n",
           ho ? "ok" : "none", ho ? (unsigned long)ho->loader_ver : 0ul,
           fmac_rftlv_addr() ? "ok" : "none");
    s_task = xTaskCreateStatic(bus_task, "fmachost", BUS_STACK_WORDS, NULL,
                               BUS_TASK_PRIO, s_stack, &s_tcb);
    fmac_smid_set_notify(bus_notify);
    fmac_usb_set_notify(bus_notify);
    xTaskCreateStatic(mon_task, "fmacmon", 256, NULL, 26, s_mon_stack,
                      &s_mon_tcb);
    return 0;
}

/* Data path hooks of macif_bus.c */

void fmac_host_rx_ready(bool isr)
{
    if (isr) {
        BaseType_t woken = pdFALSE;

        vTaskNotifyGiveFromISR(s_task, &woken);
        portYIELD_FROM_ISR(woken);
    } else {
        xTaskNotifyGive(s_task);
    }
}

void fmac_host_tx_cfm_ready(bool isr)
{
    fmac_host_rx_ready(isr);
}

#ifdef CONFIG_IPRO6_FMAC_AP
void fmac_host_mgmt_cfm(uint32_t cookie, bool acked, bool isr)
{
    fmac_mgmt_cfm_t c = { cookie, acked, { 0 } };

    if (!ring_put(FMAC_T_MGMT_CFM, &c, sizeof(c)))
        s_cnt.ring_full++;
    fmac_host_rx_ready(isr);
}
#endif
