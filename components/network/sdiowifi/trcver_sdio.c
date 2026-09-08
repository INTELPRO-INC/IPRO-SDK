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

#include <sdiowifi_securec_wrap.h>
#include <stdint.h>
#if defined(IPRO6LE)
#include "drv_sdu.h"
#include "drv_gpio.h"
#include "drv_glb.h"
#include "drv_glb_gpio.h"
#elif defined(CFG_IPRO_WIFI)
#include <bl616_sdu.h>
#include <bl616_gpio.h>
#include <bl616_glb_gpio.h>
#else
#include <bl602_glb.h>
#include <bl602.h>
#include <bl602_sdu.h>
#include <platform_device.h>
#endif
#include <sdiowifi_irq_compat.h>
#include <sdiowifi_utils.h>
#include "trcver_sdio.h"
#include "net_wifi_transceiver.h"
#include "rx_profile.h"
#include "sdiowifi_mgmr.h"
#include "sdiowifi_mgmr_type.h"
//#include <bl_timer.h>

#include <sdiowifi_platform_adapt.h>

#include <sdio_port.h>

#define SDU_INT_HOST        0
#define SDU_SEND_TIME_OUT   500

static bool sdu_software_reset = false;
static ipro_sdio_read_cb_t sd_read_callback = NULL;
static sdiowifi_task_t sdu_irq_handle = NULL;
static int send_timeout = 0;
static int twin_send_timeout __attribute__((unused)) = 0;

/* Bug B dive (PM13 2026-05-29): count sdu_send_data busy-wait timeouts — the
 * upload backpressure event (host too slow reading RdBitMap) that, with no
 * queue/retry upstream, becomes a dropped ACK (see sdio_upld_fail). */
volatile uint32_t sdio_upld_timeout = 0;

/* PM13i/Task 4-C download-drain stall diag (read/reset via tx_diag). Pins WHY
 * sdu_receive_data stops advancing curr_dnld_port during a UDP-TX wedge:
 *   break_a = stopped because WrBitMap bit SET on curr_dnld_port (no data ->
 *             host/device port pointers DESYNCED, or host stopped writing)
 *   break_b = stopped because rx_buf NOT attached on curr_dnld_port (the
 *             plan's hypothesis: a buffer was consumed but never re-attached) */
volatile uint32_t sdu_dnld_calls        = 0;  /* DnLdOvr drain invocations */
volatile uint32_t sdu_dnld_processed    = 0;  /* ports drained + advanced */
volatile uint32_t sdu_dnld_crc_err      = 0;  /* CMD53 download CRC-error breaks */
volatile uint32_t sdu_dnld_break_a      = 0;  /* break: WrBitMap set (no data) */
volatile uint32_t sdu_dnld_break_b      = 0;  /* break: rx_buf not attached */
volatile uint8_t  sdu_dnld_last_port    = 0;  /* curr_dnld_port at last drain exit */
volatile uint16_t sdu_dnld_last_attached= 0;  /* sdu_rx_buf_attached[WIFI] snapshot */
volatile uint16_t sdu_dnld_last_wrbitmap= 0;  /* WrBitMap snapshot at drain exit */

/* PM13n 2026-05-30: pin the UDP-TX lost-DnLdOvr-wakeup leak point.
 *   sdu_irq_window_lost = a DnLdOvr asserted in the read->clear window of
 *       sdu_interrupt_handle (value=CardIntStatus; CardIntStatus=~value): the
 *       clear can drop it -> the drain for that host write never runs.
 * Live accessors below let tx_diag scan WrBitMap vs curr_dnld_port at the
 * wedge: if the drain is parked on a FREE port while an attached port still
 * holds data, that data was written but its wakeup was lost (stranded). */
volatile uint32_t sdu_irq_window_lost   = 0;
/* PM13o: # times the drain skipped a free-port gap to reach a stranded port
 * (drain-by-occupancy fix firing). >0 = drift happened and was self-healed;
 * with the fix, occ_attached in tx_diag's sdu_live should stay ~0. */
volatile uint32_t sdu_dnld_resync       = 0;

/* TX-aggregation (multi-frame per download port): total frames parsed across
 * all ports, and malformed/overrun breaks. agg_frames / sdu_dnld_processed ~=
 * average packed-frame count per port (N). Read/reset via tx_diag. */
volatile uint32_t sdu_dnld_agg_frames   = 0;
volatile uint32_t sdu_dnld_agg_ovf      = 0;

#define SDIO_TX_WDT_TIMEOUT     (450)

#define SDU_WORKER_EV_NONE      (0)
#define SDU_WORKER_EV_RESET     (1 << 0)
#define SDU_WORKER_EV_TX_WDT    (1 << 1)
static long sdu_worker_ev;

/* Check whether rx buf has been attached to one port by corresponding bit */
static volatile uint16_t sdu_rx_buf_attached[NUM_FUNC];

void sdu_interrupt_entry(int irq, void *arg);
void sdu_soft_reset_interrupt_entry(int irq, void *arg);
void sdu_interrupt_task(void *pvParameters);

static void sdu_attach_rx_bufs(uint8_t fn_num, uint8_t port_num)
{
    sdio_attach_rxbuf(fn_num, port_num);
    sdu_rx_buf_attached[fn_num] |= (1 << port_num);

#if SDU_INT_HOST
    /* Generate host sdio interrupt */
    sdio_GEN_CARD2HOST_INT(fn_num,(SDIO_CCR_CS_ReadCISRdy | SDIO_CCR_CS_DnLdRdy  | SDIO_CCR_CS_IORdy));
#endif
}

typedef void (*sdio_tx_cfm_cb_t)(int idx, void *arg);

struct sdio_tx_desc {
    bool used;
    bool is_cmd; // true: cmd, false: frame
    sdio_tx_cfm_cb_t cb;
    void *cb_arg;
    int cb_arg_idx;
    struct sdio_tx_desc *twin;
    uint8_t ref;
    sdiowifi_tick_t tick_attached;
};

static struct sdio_tx_desc sdio_tx_descs[16] = { 0 };
static size_t sdio_tx_attached = 0;
static sdiowifi_timer_handle_t sdio_tx_wdt_timer;

#pragma pack(push, 1)
struct sdio_top_msg {
    uint8_t type_lsb;
    uint8_t type_msb;
    uint8_t len_lsb;            // length of pld, excluding padding
    uint8_t len_msb;
    uint8_t pld_off;
    uint8_t is_amsdu;
    uint8_t has_twin;
    uint8_t subtype_lsb;
    uint8_t subtype_msb;
    uint8_t first_part_len_lsb; // length of pld, excluding padding
    uint8_t first_part_len_msb;
    uint8_t _pad0[1];
    uint8_t pld_w_pad[];
};
#pragma pack(pop)

/* GPIO IRQ side-channel: drive a clean rising edge on this pin at every upload
 * data-ready so the IPRO7 host can IRQ-wake its rx_task instead of polling the
 * RD bitmap (the in-band SDIO UpLdRdy never reaches the host SDH ECLIC). Wired:
 * device GPIO0 (this output) -> host GPIO0 (input + sync-rising IRQ). */
#define SDU_DATA_READY_GPIO_PIN   0

static uint8_t curr_upld_port = 0;
static uint8_t curr_dnld_port = 0;
static uint16_t rd_bitmap_old = 0;

void sdu_software_reset_set(int enable)
{
    sdu_software_reset = !!enable;
}

void notify_sdu_worker(int event, int isr)
{
    if (!sdu_irq_handle) {
        HR_LOGD("%s(%d): sdu worker task is not found\r\n", __func__, __LINE__);
        return;
    }
    if (event) {
        sdu_worker_ev |= event;
    }
    sdiowifi_task_notify(sdu_irq_handle, isr);
}

static void sdio_tx_timer_cb(sdiowifi_timer_handle_t timer, void *arg)
{
    notify_sdu_worker(SDU_WORKER_EV_TX_WDT, 0);
}

static void start_tx_wdt(void)
{
    if (!(sdiowifi_timer_is_valid(sdio_tx_wdt_timer))) {
        sdiowifi_timer_start(sdio_tx_wdt_timer);
    }
}

void ipro_sdio_tx_timer_stop(void)
{
    if (sdiowifi_timer_is_valid(sdio_tx_wdt_timer)) {
        sdiowifi_timer_stop(sdio_tx_wdt_timer);
    }
}

void trcver_sdio_stats_display(void)
{
    HR_LOGI("rd:0x%08lX, wr:0x%08lX\r\n", SdioFuncReg[0]->RdBitMap, SdioFuncReg[0]->WrBitMap);
}

int __attribute__((section(".tcm_code"))) sdu_send_data(void *data, uint32_t len, bool is_cmd, sdio_tx_cfm_cb_t cb, void *cb_arg)
{
    const uint8_t fn = 0;
    uint64_t count_time;
    struct sdio_tx_desc *desc;

    (void)len;

    count_time = CPU_Get_MTimer_US();
    while ((SdioFuncReg[fn]->RdBitMap & (1<< curr_upld_port))){
        if((CPU_Get_MTimer_US() - count_time) / 1000 >
           (send_timeout ? 10 : SDU_SEND_TIME_OUT)){
            send_timeout = 1;
            sdio_upld_timeout++;
            HR_LOGD("sdu send timeout\r\n");
            return IPRO_MSG_ERR_TIMEOUT;
        } else if (send_timeout) {
            /* after once timeout maybe next use delay for other thread scheduler and reduce timeout val*/
            sdiowifi_delay_ms(1);
        }
    }
    send_timeout = 0;

    desc = &sdio_tx_descs[curr_upld_port];
    if (desc->used) {
        HR_LOGD("sdio tx desc used!\r\n");
        return IPRO_MSG_ERR_DESC_USED;
    }

    sdiowifi_enter_critical();
    desc->used = true;
    desc->is_cmd = is_cmd;
    desc->cb = cb;
    desc->cb_arg = cb_arg;
    desc->cb_arg_idx = curr_upld_port;
    desc->ref = 1;
    desc->twin = NULL;
    SdioFuncReg[fn]->RdIdx = curr_upld_port;
    SdioFuncReg[fn]->RdLen[curr_upld_port] = SDIO_TX_BUF_SIZE;
    SdioFuncReg[fn]->SqReadBase = (uint32_t)data;
    SdioFuncReg[fn]->RdBitMap = 1 << curr_upld_port;
    rd_bitmap_old = rd_bitmap_old | (1 << curr_upld_port);
    /* HR_LOGD("sdu_send_data %lu port %u, old %x\r\n", len, curr_upld_port, rd_bitmap_old); */
    /* GPIO data-ready edge: high -> (assert in-band UpLdRdy) -> low. The host
     * GPIO0 sync-rising IRQ fires on the leading edge and wakes its rx_task
     * immediately. Returning low re-arms for the next frame's rising edge.
     * Runs inside the SDU critical section — two register writes, negligible. */
    GLB_GPIO_Write(SDU_DATA_READY_GPIO_PIN, 1);
    sdio_GEN_CARD2HOST_INT(0, SDIO_CCR_CS_UpLdRdy);
    GLB_GPIO_Write(SDU_DATA_READY_GPIO_PIN, 0);
    curr_upld_port = (curr_upld_port + 1) % 16;
    desc->tick_attached = sdiowifi_tick_now(0);
    ++sdio_tx_attached;
    sdiowifi_exit_critical(0);

    start_tx_wdt();

    return 0;
}

static void sdu_receive_data(uint8_t int_status)
{
    /* The port number for host to download packets*/
    uint8_t fn;
    /* uint8_t value; */
    uint8_t CRCError;
    uint8_t *recv_buf=NULL;

    for (fn = 0; fn < NUM_FUNC; fn++)
    {
        switch(fn)
        {
            case FUNC_WIFI:
            {
                /* value = SdioFuncReg[fn]->CardIntStatus; */
                /* Read the CRC error for the CMD 53 write*/
                CRCError = SdioFuncReg[fn]->HostTransferStatus;

                if (int_status & SDIO_CCR_CIC_DnLdOvr)
                {
                    sdu_dnld_calls++;   /* Task 4-C diag: DnLdOvr drain invocations */
                    /* Clear Interrupt Bit */
                    //SdioFuncReg[fn]->CardIntStatus = ~value|SDIO_CCR_CIC_PwrUp;
                    /* SdioFuncReg[fn]->CardIntStatus = value & 0xfe; */
                    if(CRCError & SDIO_CCR_HOST_INT_DnLdCRC_err)
                    {
                        sdu_dnld_crc_err++;
                        break;
                    }
                    while (1) {
                        uint16_t wrbm = SdioFuncReg[fn]->WrBitMap;
                        int has_data = !(wrbm & (1 << curr_dnld_port));
                        int attached = (sdu_rx_buf_attached[fn] & (1 << curr_dnld_port)) ? 1 : 0;

                        /* PM13o drain-by-occupancy fix: the old code BROKE the
                         * moment curr_dnld_port was not drainable. Under
                         * host/device pointer drift the host-written (occupied)
                         * ports can be SEPARATED from curr_dnld_port by free-
                         * port gaps; a strict in-order break-on-first-free then
                         * strands every port beyond the gap forever (the rx_buf
                         * stays attached, the WrBitMap bit stays clear) and the
                         * ring's free slots leak away until a full wedge.
                         * Instead, when curr_dnld_port is not drainable, scan
                         * the whole ring for the next (has_data && attached)
                         * port and resync curr_dnld_port over the gap; only
                         * break when NOTHING is drainable. The fast path
                         * (curr_dnld_port has data) is unchanged. */
                        if (!(has_data && attached)) {
                            int found = -1;
                            for (int i = 1; i < SDIO_RX_MAX_PORT_NUM; i++) {
                                uint8_t p = curr_dnld_port + i;
                                if (p >= SDIO_RX_MAX_PORT_NUM)
                                    p -= SDIO_RX_MAX_PORT_NUM;
                                if (!(wrbm & (1 << p)) &&
                                    (sdu_rx_buf_attached[fn] & (1 << p))) {
                                    found = p;
                                    break;
                                }
                            }
                            if (found < 0) {
                                /* nothing drainable anywhere — really idle */
                                if (has_data && !attached) sdu_dnld_break_b++;
                                else                       sdu_dnld_break_a++;
                                sdu_dnld_last_port     = curr_dnld_port;
                                sdu_dnld_last_attached = sdu_rx_buf_attached[fn];
                                sdu_dnld_last_wrbitmap = wrbm;
                                break;
                            }
                            /* skip the free-port gap, drain the stranded port */
                            curr_dnld_port = (uint8_t)found;
                            sdu_dnld_resync++;
                        }

                        recv_buf = (uint8_t *)sdio_get_rxbuf(curr_dnld_port);
                        /*clear current download port attach flag*/
                        sdu_rx_buf_attached[fn] &= ~(1 << curr_dnld_port);///clear attached flag

                        /* TX-aggregation parse: the host packs one or more
                         * frames [2B body_len][body]... terminated by a 0x0000
                         * len sentinel, all in ONE CMD53 write to this port.
                         * Drain every frame until the sentinel or the buffer
                         * end. Backward compatible: a single non-aggregated
                         * frame is followed by the host-written 0x0000 -> exactly
                         * one iteration. (The loop reads `len` only at frame
                         * boundaries, advancing off by 2+len, so body bytes are
                         * never mis-parsed as a length.) */
                        {
                            uint32_t off = 0;
                            while (off + 2U <= SDIO_DNLD_AGG_BUF_SIZE) {
                                uint16_t len = recv_buf[off] | (recv_buf[off + 1] << 8);
                                if (len == 0) break;                 /* sentinel */
                                if (off + 2U + len > SDIO_DNLD_AGG_BUF_SIZE) {
                                    sdu_dnld_agg_ovf++;              /* malformed / overrun */
                                    break;
                                }
                                if (sd_read_callback != NULL) {
                                    rxprof_t0(len);
                                    sd_read_callback(NULL, recv_buf + off + 2, len);
                                }
                                off += 2U + len;
                                sdu_dnld_agg_frames++;
                            }
                        }

                        sdu_attach_rx_bufs(fn, curr_dnld_port);
                        /*move on to next port*/
                        curr_dnld_port++;
                        if (curr_dnld_port == SDIO_RX_MAX_PORT_NUM) {
                            curr_dnld_port = 0;
                        }
                        sdu_dnld_processed++;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

/* PM13n live diag accessors (curr_dnld_port + sdu_rx_buf_attached are file
 * static; WrBitMap is a HW reg). Called from the shell task during tx_diag. */
uint16_t sdu_dbg_live_wrbm(void) { return SdioFuncReg[FUNC_WIFI]->WrBitMap; }
uint8_t  sdu_dbg_curr_dnld(void) { return curr_dnld_port; }
uint16_t sdu_dbg_attached(void)  { return sdu_rx_buf_attached[FUNC_WIFI]; }

int ipro_sdu_init(void)
{
    HR_LOGI("ipro_sdu_init\r\n");
    sdio_gpio_init();

    /* Data-ready side-channel: GPIO0 push-pull output, idle low. Configured
     * once here; sdu_send_data() pulses it. Mirrors the (commented) original
     * GLB_GPIO_Write(PIN_3,...) data-ready pulse in sdio_port.c gen_card2host_int(). */
    {
        GLB_GPIO_Cfg_Type dr_cfg = {
            .gpioPin  = SDU_DATA_READY_GPIO_PIN,
            .gpioFun  = GPIO_FUN_GPIO,
            .gpioMode = GPIO_MODE_OUTPUT,
            .pullType = GPIO_PULL_NONE,
            .drive    = 1,
            .smtCtrl  = 0,
        };
        GLB_GPIO_Init(&dr_cfg);
        GLB_GPIO_Write(SDU_DATA_READY_GPIO_PIN, 0);   /* idle low — arm for rising edge */
    }

    GLB_Config_SDIO_Host_Reset_System(0);
    GLB_Config_SDIO_Host_Reset_SDU(1);
    GLB_Config_SDIO_Host_Interrupt_CPU(1);

    sdiowifi_irq_attach(SDU_SOFT_RST_IRQn, sdu_soft_reset_interrupt_entry, NULL);
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR |= 0x2;   /* rising edge trigger */
    sdiowifi_irq_enable(SDU_SOFT_RST_IRQn);

    sdiowifi_task_create(sdu_interrupt_task, "sdu_worker", 0, 512, NULL, 31, &sdu_irq_handle);
    sdio_tx_wdt_timer = sdiowifi_timer_create(sdio_tx_timer_cb, NULL, SDIO_TX_WDT_TIMEOUT, 1, 0);

    sdio_glb_reset();

    sdiowifi_irq_attach(SDIO_IRQn, sdu_interrupt_entry, NULL);
    sdiowifi_irq_enable(SDIO_IRQn);

    return 0;
}

int32_t ipro_sdio_handshake(void)
{
    sdio_host_check_init();

    for (int i = 0; i < SDIO_RX_MAX_PORT_NUM; ++i) {
        sdu_attach_rx_bufs(FUNC_WIFI, i);
    }
    return 0;
}

int ipro_sdio_read_cb_register(void *env, ipro_sdio_read_cb_t cb, void *cb_arg)
{
    sd_read_callback = cb;

    return 0;
}

#define LSB(x) ((x) & 0xff)
#define SLSB(x) (((x) >> 8) & 0xff)
#define OFFSET_OF(mem, st) ((uintptr_t)&(((st *)0)->mem))

struct sd_cmd_alloc_header {
    uint8_t guard_[16];
    void *orig;
    uint8_t data[];
};

static void sd_cmd_cfm_cb(int idx, void *arg)
{
#ifdef CFG_IPRO_WIFI
    if (arg) {
        struct sd_cmd_alloc_header *hdr;
        hdr = (void *)((uint8_t *)arg - sizeof(*hdr));
        vPortFree(hdr->orig);
    }
#else
    vPortFree(arg);
#endif
}

static void *sd_cmd_buf_alloc()
{
    void *ret;
#ifdef CFG_IPRO_WIFI
    size_t alloc_sz = sizeof(struct sd_cmd_alloc_header) + SDIO_TX_BUF_SIZE + 32;
    ret = pvPortMalloc(alloc_sz);
    if (ret) {
        void *cpy = ret;
        struct sd_cmd_alloc_header *hdr;
        L1C_DCache_Clean_Invalid_By_Addr((uintptr_t)ret, alloc_sz);
        hdr = (void *)SDIO_MAWOC(ret);
        hdr->orig = cpy;
        ret = hdr->data;
    }
#else
    ret = pvPortMalloc(SDIO_TX_BUF_SIZE);
#endif
    return ret;
}

int ipro_sdio_write_cmd(void *env, uint16_t type, uint16_t subtype, const void *headroom, uint16_t headroom_len, const void *tailroom, uint16_t tailroom_len)
{
    const uint8_t *p1 = (const uint8_t *)headroom;
    size_t p1_len = (size_t)headroom_len;
    const uint8_t *p2 = (const uint8_t *)tailroom;
    size_t p2_len = (size_t)tailroom_len;

    int ret;
    struct sdio_top_msg *msg;
    void *p_buf;
    size_t buf_size = SDIO_TX_BUF_SIZE;
    size_t len = p1_len + p2_len;

    if (sizeof(struct sdio_top_msg) + len > SDIO_TX_BUF_SIZE) {
        return -1;
    }

    p_buf = sd_cmd_buf_alloc();
    if (!p_buf) {
        return -1;
    }

    msg = (struct sdio_top_msg *)p_buf;
    msg->type_lsb = LSB(type);
    msg->type_msb = SLSB(type);
    msg->len_lsb = LSB(len);
    msg->len_msb = SLSB(len);
    msg->pld_off = OFFSET_OF(pld_w_pad, struct sdio_top_msg);
    msg->subtype_lsb = LSB(subtype);
    msg->subtype_msb = SLSB(subtype);
    msg->is_amsdu = false;
    msg->has_twin = 0;

    p_buf += msg->pld_off;
    buf_size -= msg->pld_off;

    MEMCPY_SAFE((uint8_t *)p_buf, buf_size, p1, p1_len);
    if (p2) {
        p_buf += p1_len;
        buf_size -= p1_len;
        MEMCPY_SAFE((uint8_t *)p_buf, buf_size, p2, p2_len);
    }

    ret = sdu_send_data(msg, len, false, sd_cmd_cfm_cb, msg);
    if (ret) {
        sd_cmd_cfm_cb(-1, msg);
    }
    return ret;
}

int ipro_sdio_write_pbuf(void *env, uint16_t type, uint16_t subtype, struct pbuf *p, bool is_amsdu, void *cb, void *cb_arg)
{
    struct sdio_top_msg *msg;
    void *data_ptr = p->payload;
    uint16_t data_len = p->tot_len;
    struct pbuf *next;

    msg = (struct sdio_top_msg *)SDM_ALIGN_LO(data_ptr - sizeof(*msg), 4);

    msg->type_lsb = LSB(type);
    msg->type_msb = SLSB(type);
    msg->len_lsb = LSB(data_len);
    msg->len_msb = SLSB(data_len);
    msg->pld_off = data_ptr - (void *)msg;
    msg->is_amsdu = is_amsdu;
    msg->has_twin = 0;
    msg->subtype_lsb = LSB(subtype);
    msg->subtype_msb = SLSB(subtype);
    msg->first_part_len_lsb = LSB(p->len);
    msg->first_part_len_msb = SLSB(p->len);

    next = p->next;
    if (next) {
        HR_LOGE("BL616 this should not happen, this is an A-AMSDU with two pbds\r\n");
        return IPRO_MSG_ERR_TIMEOUT;
    } else {
        return sdu_send_data(msg, data_len, false, cb, cb_arg);
    }
}

#define BIT_ISSET(val, bitn) ((val) & (1 << (bitn)))

// Function to handle SDIO interrupts
static void __attribute__((section(".tcm_code"))) sdu_interrupt_handle(void)
{
    uint8_t value;
    uint16_t rd_bitmap;
    uint16_t rd_bitmap_diff;
    struct sdio_tx_desc *desc;

    // Read the value of SDIO interrupt status register
    value = SdioFuncReg[0]->CardIntStatus;
    // Clear the interrupt status and set pwrup flag
    SdioFuncReg[0]->CardIntStatus = ~value | SDIO_CCR_CIC_PwrUp;

    /* PM13n diag: a DnLdOvr that asserted in the read->clear window above is at
     * risk of being dropped by the ~value clear. If a DnLdOvr is pending NOW
     * but was NOT in the value we just serviced, it raced the clear. */
    {
        uint8_t after = SdioFuncReg[0]->CardIntStatus;
        if ((after & SDIO_CCR_CIC_DnLdOvr) && !(value & SDIO_CCR_CIC_DnLdOvr))
            sdu_irq_window_lost++;
    }

    // If it's a download data over interrupt, process the download data
    if (value & SDIO_CCR_CIC_DnLdOvr) {
        sdu_receive_data(value);
    }

    // If it's an upload data over interrupt, process the upload data
    if (value & SDIO_CCR_CIC_UpLdOvr) {
        // Read the read data bitmap and calculate the difference
        rd_bitmap = SdioFuncReg[0]->RdBitMap;
        rd_bitmap_diff = rd_bitmap ^ rd_bitmap_old;

        // If there's a difference in the read data bitmap, process the corresponding upload data
        if (rd_bitmap_diff) {
            for (int i = 0; i < 16; ++i) {
                // Check if the current bit is set
                if (!(BIT_ISSET(rd_bitmap_diff, i)) || !(BIT_ISSET(rd_bitmap_old, i))) {
                    continue;
                }

                // Check if the current descriptor is available and not processed
                desc = &sdio_tx_descs[i];
                if (!(desc->used) || !(desc->cb) || !(desc->twin == NULL)) {
                    continue;
                }

                // If the reference count is zero, process the descriptor
                desc->ref--;
                if (desc->ref != 0) {
                    continue;
                }

                // Call the callback function to process the descriptor and mark it as unused
                desc->cb(i, desc->cb_arg);
                sdiowifi_enter_critical();
                desc->used = false;
                --sdio_tx_attached;
                sdiowifi_exit_critical(0);
            }
        } else {
            // If there's no difference in the read data bitmap, log the event
            HR_LOGD("sdu_interrupt_handle rd_bitmap_diff:%d\r\n", rd_bitmap_diff);
        }
        // Update the old read data bitmap
        rd_bitmap_old = rd_bitmap;
    }
}

static void sdio_tx_timedout()
{
    // TODO take your action here
    /* sdiowifi_mgmr_reinit(false); */
}

static void check_tx_timeout(void)
{
    int i;
    sdiowifi_tick_t now = sdiowifi_tick_now(0);
    size_t timedout = 0;

    if (sdio_tx_attached == 0) {
        return;
    }
    for (i = 0; i < sizeof(sdio_tx_descs) / sizeof(sdio_tx_descs[0]); ++i) {
        struct sdio_tx_desc *d = &sdio_tx_descs[i];
        if (!d->used) {
            continue;
        }
        if (!(now - d->tick_attached > sdiowifi_ms2ticks(SDIO_TX_WDT_TIMEOUT))) {
            continue;
        }

        if (d->cb) {
            d->cb(i, d->cb_arg);
        }
        sdiowifi_enter_critical();
        d->used = false;
        --sdio_tx_attached;
        sdiowifi_exit_critical(0);
        ++timedout;
        // XXX twin not considered
    }

    if (timedout) {
        sdio_tx_timedout();
    }
}

static void ipro_sdu_reinit(void)
{
    sdiowifi_irq_disable(SDIO_IRQn);

    // sdio bit_map flag reset
    curr_upld_port = 0;
    curr_dnld_port = 0;
    rd_bitmap_old = 0;

    // tx buffer reset
    for (int i = 0; i < sizeof(sdio_tx_descs) / sizeof(sdio_tx_descs[0]); ++i) {
        struct sdio_tx_desc *desc = &sdio_tx_descs[i];
        if (desc->used) {
            desc->cb(i, desc->cb_arg);
        }
    }
    MEMSET_SAFE(sdio_tx_descs,  sizeof(sdio_tx_descs),  0,  sizeof(sdio_tx_descs));
    sdiowifi_enter_critical();
    sdio_tx_attached = 0;
    sdiowifi_exit_critical(0);

    if (sdu_software_reset) {
        HR_LOGD("SDU Software Reset\r\n");
        sdio_glb_reset();
        sdu_software_reset = false;
    }

    GLB_Config_SDIO_Host_Reset_System(0);
    GLB_Config_SDIO_Host_Reset_SDU(1);
    GLB_Config_SDIO_Host_Interrupt_CPU(1);

    sdiowifi_irq_attach(SDU_SOFT_RST_IRQn, sdu_soft_reset_interrupt_entry, NULL);
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR |= 0x2;   /* rising edge trigger */
    sdiowifi_irq_enable(SDU_SOFT_RST_IRQn);

    sdio_gpio_init();

    ipro_sdio_handshake();

    sdiowifi_irq_enable(SDIO_IRQn);
}

static int sdio_software_reset(void)
{
    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;

    HR_LOGI("sdio_software_reset\r\n");

    if (!sdm->init) {
        HR_LOGW("%s(%d): sdm is not initialized\r\n", __func__, __LINE__);
        return -1;
    }
    ipro_net_wifi_trcver_set_present(&sdm->trcver, false);
    ipro_sdu_reinit();
    ipro_net_wifi_trcver_reinit(&sdm->trcver);
    sdiowifi_tx_buf_update_credit(&sdm->tx_desc);
    return 0;
}

void __attribute__((section(".tcm_code"))) sdu_interrupt_entry(int irq, void *arg)
{
    sdiowifi_irq_disable(SDIO_IRQn);
    notify_sdu_worker(SDU_WORKER_EV_NONE, 1);
}

void sdu_soft_reset_interrupt_entry(int irq, void *arg)
{
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].IP = 0;
    sdiowifi_irq_disable(SDIO_IRQn);

    sdu_software_reset_set(false);
    HR_LOGW("recv reset signal from isr, -> set SDU_WORKER_EV_RESET\r\n");
    notify_sdu_worker(SDU_WORKER_EV_RESET, 1);
}

void __attribute__((section(".tcm_code"))) sdu_interrupt_task(void *pvParameters)
{
    sdu_irq_handle = sdiowifi_get_task_handle();

    while(1) {
        sdiowifi_task_wait_notification(SDIOWIFI_WAIT_FOREVER);

        sdu_interrupt_handle();

        if (sdu_worker_ev & SDU_WORKER_EV_RESET) {
            sdu_worker_ev &= ~SDU_WORKER_EV_RESET;
            HR_LOGI("SDU_WORKER_EV_RESET\r\n");
            sdio_software_reset();
        }

        if (sdu_worker_ev & SDU_WORKER_EV_TX_WDT) {
            sdu_worker_ev &= ~SDU_WORKER_EV_TX_WDT;
            HR_LOGI("SDU_WORKER_EV_TX_WDT\r\n");
            check_tx_timeout();
        }

        if (sdio_tx_attached == 0) {
#ifdef SDIOWIFI_HEARTBEAT
#else
            HR_LOGD("sdio_tx_attached is zero.\r\n");
#endif
            ipro_sdio_tx_timer_stop();
        }

        sdiowifi_irq_enable(SDIO_IRQn);
    }
}
