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

#include "drv_glb.h"
#include "drv_gpio.h"
#include "ipro6.h"
#include "ipro6_sdio3.h"
#include "ipro_irq.h"
#include "compiler/common.h"
#include "hal_mtimer.h"
#include "fmac_dl.h"
#include "fmac_smid.h"

/* Download buffers, and the largest upload (whole blocks) */
#ifndef FMAC_SMID_NBUF
#define FMAC_SMID_NBUF 2
#endif
#ifndef FMAC_SMID_UPLD_MAX
#define FMAC_SMID_UPLD_MAX FMAC_DL_BLK
#endif
#define NBUF        FMAC_SMID_NBUF
#define UPLD_LEN    FMAC_SMID_UPLD_MAX
_Static_assert(UPLD_LEN % FMAC_DL_BLK == 0, "upload is whole blocks");
/* An upload pending this long with a zero read count has lost its
 * doorbell write; it is written again after the zero held DB_HOLD_US */
#define DB_LOST_US  5000u
#define DB_HOLD_US  1000u

static ipro6_smid_dev_t s_dev = { .reg_base = SMID_BASE,
                                  .irq_num = SMID_IRQn };
#ifdef FMAC_BUS_POOL
#include "fmac_bus_pool.h"
#define s_buf   (fmac_dnld_pool.smid)
#define s_upld  (fmac_upld_pool.smid)
#else
static ATTR_NOCACHE_RAM_SECTION uint8_t
    s_buf[NBUF][FMAC_DL_PKT_MAX] __attribute__((aligned(32)));
static ATTR_NOCACHE_RAM_SECTION uint8_t
    s_upld[UPLD_LEN] __attribute__((aligned(32)));
#endif
#ifndef FMAC_SMID_UPLD_NQ
#define FMAC_SMID_UPLD_NQ 1
#endif
/* The upload buffer is split into NQ buffers queued to the SMID back to
 * back: one is read by the host while the next one waits */
#define NQ          FMAC_SMID_UPLD_NQ
#define UB_LEN      (UPLD_LEN / NQ)
_Static_assert(UB_LEN % FMAC_DL_BLK == 0, "upload buffer is whole blocks");
static ipro6_sdio3_trans_desc_t s_desc[NBUF];
static ipro6_sdio3_trans_desc_t s_udesc[NQ];
/* Bit i: upload buffer i is queued; s_uq holds them in push order */
static volatile uint32_t s_ubusy;
static volatile uint8_t s_uq[NQ];
static volatile uint32_t s_uq_head, s_uq_tail;
static uint32_t s_ulen[NQ];
static void (*s_notify)(void);
static volatile uint8_t s_ring[NBUF];
static volatile uint32_t s_head, s_tail;
static volatile uint32_t s_len[NBUF];
static volatile uint32_t s_n_dnld, s_n_upld, s_n_other;
static volatile int s_last_evt = -1;
static uint64_t s_push_us;
/* The queue head became readable at this time */
static uint64_t s_head_us;
static uint64_t s_zero_us;
/* Upload timing, us: queue head readable to its completion, and a
 * completion to the next push into an empty queue; completions */
static uint64_t s_cpl_us;
static uint32_t s_t_read_us, s_t_turn_us, s_t_n;
static uint32_t s_rearms;

static int buf_index(const void *p)
{
    for (int i = 0; i < NBUF; i++)
        if (p == s_buf[i])
            return i;
    return -1;
}

static uint8_t *ubuf(int i)
{
    return s_upld + i * UB_LEN;
}

static int ubuf_index(const void *p)
{
    for (int i = 0; i < NQ; i++)
        if (p == ubuf(i))
            return i;
    return -1;
}

static int ubuf_free(void)
{
    for (int i = 0; i < NQ; i++)
        if (!(s_ubusy & (1u << i)))
            return i;
    return -1;
}

/* IRQ context */
static void smid_cb(void *arg, int event, void *data)
{
    ipro6_sdio3_trans_desc_t *d = data;

    (void)arg;
    s_last_evt = event;
    if (event == IPRO6_SDIO3_IRQ_EVENT_DNLD_CPL)
        s_n_dnld++;
    else if (event == IPRO6_SDIO3_IRQ_EVENT_UPLD_CPL)
        s_n_upld++;
    else
        s_n_other++;
    if (event == IPRO6_SDIO3_IRQ_EVENT_DNLD_CPL) {
        int i = buf_index(d->buff);

        if (i >= 0) {
            s_len[i] = d->data_len;
            s_ring[s_head % NBUF] = (uint8_t)i;
            s_head++;
        }
    } else if (event == IPRO6_SDIO3_IRQ_EVENT_UPLD_CPL ||
               event == IPRO6_SDIO3_IRQ_EVENT_SOFT_RESET ||
               event == IPRO6_SDIO3_IRQ_EVENT_FUNC1_RESET) {
        if (event == IPRO6_SDIO3_IRQ_EVENT_UPLD_CPL) {
            int i = ubuf_index(d->buff);

            if (i >= 0 && (s_ubusy & (1u << i))) {
                s_cpl_us = mtimer_get_time_us();
                s_t_read_us += (uint32_t)(s_cpl_us - s_head_us);
                s_t_n++;
                s_head_us = s_cpl_us;
                s_ubusy &= ~(1u << i);
                s_uq_tail++;
            }
        } else {
            s_ubusy = 0;
            s_uq_tail = s_uq_head;
        }
    }
    if (s_notify)
        s_notify();
}

void fmac_smid_set_notify(void (*fn)(void))
{
    s_notify = fn;
}

static void post(int i)
{
    s_desc[i].func = 1;
    s_desc[i].flags = 0;
    s_desc[i].data_len = FMAC_DL_PKT_MAX;
    s_desc[i].buff = s_buf[i];
    ipro6_sdio3_dnld_push(&s_dev, &s_desc[i]);
}

void fmac_smid_init(int takeover)
{
    static ipro6_sdio3_config_s cfg = {
        .func_num = 1, .func1_if_code = 0,
        .cap_flag = 0, .ocr = 0x00FF8000,
        .func1_dnld_size_max = FMAC_DL_PKT_MAX, .func2_dnld_size_max = 0,
    };

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU);
    if (!takeover) {
        GLB_GPIO_Cfg_Type g = {
            .drive = 1, .smtCtrl = 1, .gpioMode = GPIO_MODE_AF,
            .pullType = GPIO_PULL_UP, .gpioFun = GPIO_FUN_SDIO,
        };

        for (int p = 10; p < 16; p++) {
            g.gpioPin = (uint8_t)p;
            GLB_GPIO_Init(&g);
        }
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDU);
        for (volatile int i = 0; i < 10000; i++)
            __asm__ volatile("nop");
    }
    ipro6_sdio3_irq_attach(&s_dev, smid_cb, NULL);
    ipro6_sdio3_init(&s_dev, &cfg);
    if (takeover) {
        ipro6_sdio3_reset_resync(&s_dev);
    } else {
        uint32_t v = getreg32(SMID_BASE + IPRO6_SDIO3_FBR1_CONTROL_REGISTER_OFFSET);

        putreg32((v & ~0xFFFu) | FMAC_SMID_FBR1_IDENT,
                 SMID_BASE + IPRO6_SDIO3_FBR1_CONTROL_REGISTER_OFFSET);
    }
    s_head = s_tail = 0;
    s_ubusy = 0;
    s_uq_tail = s_uq_head;
    ipro6_sdio3_feature_control(&s_dev, IPRO6_SDIO3_CMD_INIT_READY, 1);
    ipro6_sdio3_feature_control(&s_dev, IPRO6_SDIO3_CMD_SET_FUNC_CARD_READY,
                                1);
    for (int i = 0; i < NBUF; i++)
        post(i);
}

int fmac_smid_next(uint8_t **pkt, uint32_t *len)
{
    if (s_tail == s_head)
        return 0;
    int i = s_ring[s_tail % NBUF];
    s_tail++;
    *pkt = s_buf[i];
    *len = s_len[i];
    return 1;
}

uint8_t *fmac_smid_upld_buf(uint32_t *max)
{
    int i = ubuf_free();

    *max = UB_LEN;
    return i >= 0 ? ubuf(i) : NULL;
}

uint32_t fmac_smid_dnld_slots(void)
{
    return NBUF;
}

void fmac_smid_release(uint8_t *pkt)
{
    int i = buf_index(pkt);

    if (i >= 0)
        post(i);
}

int fmac_smid_send(const void *msg, uint32_t len)
{
    int i = ubuf_index(msg);
    uint8_t *p;
    uintptr_t flag;

    if (len > UB_LEN)
        return -1;
    if (i < 0) {
        for (uint32_t spins = 0; (i = ubuf_free()) < 0; spins++) {
            fmac_smid_poll();
            if (spins == 2000000u)
                fmac_smid_diag("upld busy");
        }
        memcpy(ubuf(i), msg, len);
    }
    p = ubuf(i);
    s_ulen[i] = (len + FMAC_DL_BLK - 1) / FMAC_DL_BLK * FMAC_DL_BLK;
    if (!s_ulen[i])
        s_ulen[i] = FMAC_DL_BLK;
    memset(p + len, 0, s_ulen[i] - len);
    s_udesc[i].func = 1;
    s_udesc[i].flags = 0;
    s_udesc[i].data_len = s_ulen[i];
    s_udesc[i].buff = p;
    s_push_us = mtimer_get_time_us();
    flag = ipro_irq_save();
    if (!s_ubusy) {
        s_head_us = s_push_us;
        if (s_cpl_us)
            s_t_turn_us += (uint32_t)(s_push_us - s_cpl_us);
    }
    s_ubusy |= 1u << i;
    s_uq[s_uq_head % NQ] = (uint8_t)i;
    s_uq_head++;
    ipro_irq_restore(flag);
    s_zero_us = 0;
    return ipro6_sdio3_upld_push(&s_dev, &s_udesc[i]);
}

static uint32_t read_count(void)
{
    return getreg32(SMID_BASE + IPRO6_SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET) & 0xFFFFu;
}

/* Re-writes the read-count doorbell of the pending upload when it reads
 * zero with no upload transfer running */
static void doorbell_watch(void)
{
    uint64_t now;

    if (!s_ubusy || read_count() ||
        ipro6_sdio3_tran_sta() == IPRO6_SDIO3_TRAN_STA_BUSY_UPLD) {
        s_zero_us = 0;
        return;
    }
    now = mtimer_get_time_us();
    if (now - s_push_us < DB_LOST_US)
        return;
    if (!s_zero_us) {
        s_zero_us = now;
        return;
    }
    if (now - s_zero_us < DB_HOLD_US)
        return;
    uintptr_t flag = ipro_irq_save();
    if (s_ubusy && !read_count() &&
        ipro6_sdio3_tran_sta() != IPRO6_SDIO3_TRAN_STA_BUSY_UPLD) {
        putreg16(s_ulen[s_uq[s_uq_tail % NQ]],
                 SMID_BASE + IPRO6_SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
        s_rearms++;
    }
    ipro_irq_restore(flag);
    s_zero_us = 0;
}

uint32_t fmac_smid_rearms(void)
{
    return s_rearms;
}

int fmac_smid_tx_idle(void)
{
    return ubuf_free() >= 0;
}

int fmac_smid_tx_done(void)
{
    return !s_ubusy;
}

void fmac_smid_poll(void)
{
    ipro6_sdio3_keep_ready(&s_dev);
    doorbell_watch();
}

void fmac_smid_quiesce(void)
{
    ipro_irq_disable(s_dev.irq_num);
}

void fmac_smid_timing(uint32_t *read_us, uint32_t *turn_us, uint32_t *n)
{
    *read_us = s_t_read_us;
    *turn_us = s_t_turn_us;
    *n = s_t_n;
    s_t_read_us = s_t_turn_us = s_t_n = 0;
}

void fmac_smid_diag(const char *why)
{
    printf("[FMACSMID] %s: dnld %lu upld %lu other %lu last %d busy %d "
           "rearm %lu head %lu tail %lu tran %u rdcnt %04lx int %08lx\r\n", why,
           (unsigned long)s_n_dnld, (unsigned long)s_n_upld,
           (unsigned long)s_n_other, s_last_evt, (int)s_ubusy,
           (unsigned long)s_rearms,
           (unsigned long)s_head, (unsigned long)s_tail,
           (unsigned)ipro6_sdio3_tran_sta(),
           (unsigned long)(getreg32(SMID_BASE + IPRO6_SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET) & 0xFFFFu),
           (unsigned long)getreg32(SMID_BASE + IPRO6_SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET));
}

uint32_t fmac_smid_blk(void)
{
    return (uint32_t)ipro6_sdio3_feature_control(
        &s_dev, IPRO6_SDIO3_CMD_GET_CARD_STA, 0);
}
