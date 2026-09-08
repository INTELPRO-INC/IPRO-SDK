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

#include "hal_dma.h"
#include "drv_dma.h"
#include "drv_glb.h"
#include "compiler/common.h"

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#endif

static void DMA0_IRQHandler(void);

static ATTR_OCRAM_SECTION dma_device_t dma_devices[DMA_ID_MAX][DMA_MAX_INDEX] = { {
    DMA0_CH0_CONFIG,
    DMA0_CH1_CONFIG,
    DMA0_CH2_CONFIG,
    DMA0_CH3_CONFIG,
    DMA0_CH4_CONFIG,
    DMA0_CH5_CONFIG,
    DMA0_CH6_CONFIG,
    DMA0_CH7_CONFIG,
} };

/* Controller-init idempotency flag. Tracks DMA_Enable + IRQ bring-up
 * per controller. The legacy dma_init() path still does its own
 * disable/enable cycle for backward compatibility, so this flag only
 * prevents duplicate IRQ registration. */
static uint8_t s_controller_initted[DMA_ID_MAX];

dma_device_t *dma_getDevice(int dma_id, int dma_ch)
{
    return &dma_devices[dma_id][dma_ch];
}

void dma_setConfig(dma_device_t *dma_device)
{
    memcpy(&dma_devices[dma_device->id][dma_device->ch], dma_device, sizeof(dma_device_t));
}

void dma_controller_init(int dma_id)
{
    if (s_controller_initted[dma_id]) {
        return;
    }

    DMA_Enable(dma_id);
    Interrupt_Handler_Register(DMA0_ALL_IRQn, DMA0_IRQHandler);
    CPU_Interrupt_Enable(DMA0_ALL_IRQn);

    s_controller_initted[dma_id] = 1;
}

int dma_channel_attach(int dma_id, int dma_ch)
{
    dma_device_t *dma_device = &dma_devices[dma_id][dma_ch];
    DMA_Channel_Cfg_Type chCfg = { 0 };

    dma_controller_init(dma_id);

    /* Per-channel DMA clock. CH4..CH7 are clock-gated at reset; the
     * legacy dma_init() happened to work on CH0..CH3 only because
     * their clocks are enabled by default. Verified via
     * apps/test/dma_mux_probe 2026-04-20 — see
     * claude_code_todo/dma_mux_matrix.md. */
#if defined(CONFIG_IPRO6)
    /* ipro6 GLB takes a uint8_t channel index (GLB_DMA0_CLK_CHx), not the
     * ipro7 GLB_DMA_CLK_ID_Type enum. Pure chip-symbol difference. */
    GLB_Set_DMA_CLK(1, (uint8_t)(GLB_DMA0_CLK_CH0 + dma_ch));
#else
    GLB_Set_DMA_CLK(1, (GLB_DMA_CLK_ID_Type)(GLB_DMA_CLK_DMA0_CH0 + dma_ch));
#endif

    /* Mask this channel's interrupts during reconfiguration, but do
     * NOT touch the controller-wide enable or the CPU IRQ line —
     * other channels may be mid-transfer. */
    DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);
    DMA_Channel_Disable(dma_id, dma_ch);

    chCfg.ch = dma_device->ch;
    chCfg.dir = dma_device->direction;
    chCfg.srcPeriph = dma_device->src_req;
    chCfg.dstPeriph = dma_device->dst_req;
    chCfg.srcAddrInc = dma_device->src_addr_inc;
    chCfg.destAddrInc = dma_device->dst_addr_inc;
    chCfg.srcBurstSize = dma_device->src_burst_size;
    chCfg.dstBurstSize = dma_device->dst_burst_size;
    chCfg.srcTransfWidth = dma_device->src_width;
    chCfg.dstTransfWidth = dma_device->dst_width;
    DMA_Channel_Init(dma_id, &chCfg);

    return 0;
}

int dma_init(int dma_id, int dma_ch)
{
#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    /* Allocator build: non-disruptive per-channel attach, no global
     * controller reset. Safe to call while other channels are live. */
    return dma_channel_attach(dma_id, dma_ch);
#else
    /* Legacy path: global DMA disable/enable on every channel init.
     * Preserved bit-for-bit for apps not yet migrated to the
     * allocator. Aborts any in-flight DMA on other channels — this
     * is the bug that blocks duplex audio. */
    dma_device_t *dma_device = &dma_devices[dma_id][dma_ch];
    DMA_Channel_Cfg_Type chCfg = { 0 };

    DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);
    CPU_Interrupt_Disable(DMA0_ALL_IRQn);

    DMA_Disable(dma_id);

    DMA_Channel_Disable(dma_id, dma_ch);

    chCfg.ch = dma_device->ch;
    chCfg.dir = dma_device->direction;
    chCfg.srcPeriph = dma_device->src_req;
    chCfg.dstPeriph = dma_device->dst_req;
    chCfg.srcAddrInc = dma_device->src_addr_inc;
    chCfg.destAddrInc = dma_device->dst_addr_inc;
    chCfg.srcBurstSize = dma_device->src_burst_size;
    chCfg.dstBurstSize = dma_device->dst_burst_size;
    chCfg.srcTransfWidth = dma_device->src_width;
    chCfg.dstTransfWidth = dma_device->dst_width;
    DMA_Channel_Init(dma_id, &chCfg);

    DMA_Enable(dma_id);

    Interrupt_Handler_Register(DMA0_ALL_IRQn, DMA0_IRQHandler);
    /* Enable dma interrupt*/
    CPU_Interrupt_Enable(DMA0_ALL_IRQn);

    return 0;
#endif
}

int dma_ioctl(int dma_id, int dma_ch, int cmd, void *args)
{
    dma_device_t *dma_device = &dma_devices[dma_id][dma_ch];
    switch (cmd) {
        case DMA_CTRL_SET_INT:
            /* Dma interrupt configuration */
            DMA_IntMask(dma_id, dma_ch, DMA_INT_TCOMPLETED, UNMASK);
            DMA_IntMask(dma_id, dma_ch, DMA_INT_ERR, UNMASK);
            break;

        case DMA_CTRL_CLEAR_INT:
            /* Dma interrupt configuration */
            DMA_IntMask(dma_id, dma_ch, DMA_INT_TCOMPLETED, MASK);
            DMA_IntMask(dma_id, dma_ch, DMA_INT_ERR, MASK);
            break;

        case DMA_CTRL_CHANNEL_LLI_UPDATE:
            DMA_LLI_Update(dma_id, dma_ch, (uint32_t)args);
            break;

        case DMA_CTRL_CHANNEL_TRANS_PARAM_UPDATE: {
            dma_trans_param_t *param = (dma_trans_param_t *)args;
            if (param->map & DMA_PARAM_UPDATE_SRC_WIDTH)
                dma_device->src_width = param->src_width;
            if (param->map & DMA_PARAM_UPDATE_SRC_BURST)
                dma_device->src_burst_size = param->src_burst;
            if (param->map & DMA_PARAM_UPDATE_DST_WIDTH)
                dma_device->dst_width = param->dst_width;
            if (param->map & DMA_PARAM_UPDATE_DST_BURST)
                dma_device->dst_burst_size = param->dst_burst;
            if (param->map & DMA_PARAM_UPDATE_SRC_ADDR_INC)
                dma_device->src_addr_inc = param->src_addr_inc;
            if (param->map & DMA_PARAM_UPDATE_DST_ADDR_INC)
                dma_device->dst_addr_inc = param->dst_addr_inc;
            break;
        }
        case DMA_CTRL_CHANNEL_GET_STATUS:
            return DMA_Channel_Is_Busy(dma_id, dma_ch);

        case DMA_CTRL_CHANNEL_START:
            DMA_Channel_Enable(dma_id, dma_ch);
            break;

        case DMA_CTRL_CHANNEL_STOP:
            DMA_Channel_Disable(dma_id, dma_ch);
            break;

        case DMA_CTRL_SET_CALLBACK:
            dma_device->callback = args;
            break;

        default:
            break;
    }

    return 0;
}

int dma_close(uint32_t dma_id, int dma_ch)
{
    DMA_Channel_Cfg_Type chCfg = { 0 };
    dma_device_t *dma_device = &dma_devices[dma_id][dma_ch];
    if(dma_device->lli_cfg != NULL)
    {
        free(dma_device->lli_cfg);
        dma_device->lli_cfg = NULL;
    }
    DMA_Channel_Disable(dma_id, dma_ch);
    DMA_Channel_Init(dma_id, &chCfg);
    return 0;
}

int dma_reload(int dma_id, int dma_ch, uint32_t src_addr, uint32_t dst_addr, uint32_t transfer_size, dma_lli_ctrl_t **lliCfg)
{
    uint32_t malloc_count;
    uint32_t remain_len;
    uint32_t actual_transfer_len = 0;
    uint8_t transferSizeShift = 0;
    dma_control_data_t dma_ctrl_cfg;
    dma_device_t *dma_device = &dma_devices[dma_id][dma_ch];

    DMA_Channel_Disable(dma_id, dma_ch);

    if (transfer_size == 0) {
        return 0;
    }

    switch (dma_device->src_width) {
        case DMA_TRANSFER_WIDTH_8BIT:
            actual_transfer_len = transfer_size;
            transferSizeShift = 0;
            break;
        case DMA_TRANSFER_WIDTH_16BIT:
            if (transfer_size % 2) {
                return -1;
            }
            actual_transfer_len = transfer_size >> 1;
            transferSizeShift = 1;
            break;
        case DMA_TRANSFER_WIDTH_32BIT:
            if (transfer_size % 4) {
                return -1;
            }

            actual_transfer_len = transfer_size >> 2;
            transferSizeShift = 2;
            break;

        default:
            return -3;
            break;
    }

    memset(&dma_ctrl_cfg, 0, sizeof(dma_ctrl_cfg));
    dma_ctrl_cfg.bits.SBSize = dma_device->src_burst_size;
    dma_ctrl_cfg.bits.DBSize = dma_device->dst_burst_size;
    dma_ctrl_cfg.bits.SWidth = dma_device->src_width;
    dma_ctrl_cfg.bits.DWidth = dma_device->dst_width;
    dma_ctrl_cfg.bits.SI = dma_device->src_addr_inc;
    dma_ctrl_cfg.bits.DI = dma_device->dst_addr_inc;

    malloc_count = actual_transfer_len / DMA_MAX_TRANSFER_PER_LLI;
    remain_len = actual_transfer_len % DMA_MAX_TRANSFER_PER_LLI;

    if (remain_len) {
        malloc_count++;
    }

    if (dma_device->transfer_mode == DMA_LLI_PINGPONG_MODE)
        malloc_count *= 2;
    else if (dma_device->transfer_mode == DMA_LLI_CYCLE_N_MODE && dma_device->lli_count >= 2)
        malloc_count *= dma_device->lli_count;

    dma_device->lli_cfg = (dma_lli_ctrl_t *)realloc(dma_device->lli_cfg, sizeof(dma_lli_ctrl_t) * malloc_count);

    if (dma_device->lli_cfg) {
        dma_ctrl_cfg.bits.TransferSize = DMA_MAX_TRANSFER_PER_LLI;
        /*transfer_size will be integer multiple of DMA_MAX_TRANSFER_PER_LLI*n or DMA_MAX_TRANSFER_PER_LLI*2*n or DMA_MAX_TRANSFER_PER_LLI*4*n,(n>0) */
        for (uint32_t i = 0; i < malloc_count; i++) {
            dma_device->lli_cfg[i].src_addr = src_addr;
            dma_device->lli_cfg[i].dst_addr = dst_addr;
            dma_device->lli_cfg[i].nextlli = 0;
            dma_ctrl_cfg.bits.I = 0;
            dma_ctrl_cfg.bits.TransferSize = DMA_MAX_TRANSFER_PER_LLI;
            // Set interrupt at boundary of each buffer segment
            if (dma_device->transfer_mode == DMA_LLI_PINGPONG_MODE && (i == (malloc_count / 2 - 1))) {
                dma_ctrl_cfg.bits.I = 1;
                if (remain_len) {
                    dma_ctrl_cfg.bits.TransferSize = remain_len;
                } else {
                    dma_ctrl_cfg.bits.TransferSize = DMA_MAX_TRANSFER_PER_LLI;
                }
            }
            // N-buffer mode: interrupt at each 1/N boundary (except last, handled below)
            if (dma_device->transfer_mode == DMA_LLI_CYCLE_N_MODE && dma_device->lli_count >= 2) {
                uint32_t segment_size = malloc_count / dma_device->lli_count;
                // Set IRQ at end of each segment (except last one which is handled below)
                for (uint8_t n = 1; n < dma_device->lli_count; n++) {
                    if (i == (n * segment_size - 1)) {
                        dma_ctrl_cfg.bits.I = 1;
                        if (remain_len) {
                            dma_ctrl_cfg.bits.TransferSize = remain_len;
                        } else {
                            dma_ctrl_cfg.bits.TransferSize = DMA_MAX_TRANSFER_PER_LLI;
                        }
                        break;
                    }
                }
            }
            if (i == malloc_count - 1) {
                dma_ctrl_cfg.bits.I = 1;
                if (remain_len) {
                    dma_ctrl_cfg.bits.TransferSize = remain_len;
                } else {
                    dma_ctrl_cfg.bits.TransferSize = DMA_MAX_TRANSFER_PER_LLI;
                }

                // Form circular LLI chain for cycle, pingpong, and N-buffer modes
                if (dma_device->transfer_mode == DMA_LLI_CYCLE_MODE ||
                    dma_device->transfer_mode == DMA_LLI_PINGPONG_MODE ||
                    dma_device->transfer_mode == DMA_LLI_CYCLE_N_MODE) {
                    dma_device->lli_cfg[i].nextlli = (uint32_t)&dma_device->lli_cfg[0];
                }
            }

            if (dma_ctrl_cfg.bits.SI) {
                src_addr += (dma_ctrl_cfg.bits.TransferSize << transferSizeShift);
            }

            if (dma_ctrl_cfg.bits.DI) {
                dst_addr += (dma_ctrl_cfg.bits.TransferSize << transferSizeShift);
            }

            if (i) {
                dma_device->lli_cfg[i - 1].nextlli = (uint32_t)&dma_device->lli_cfg[i];
            }

            dma_device->lli_cfg[i].cfg = dma_ctrl_cfg;
        }
        L1C_DCACHE_CLEAN_RANGE((uint32_t)dma_device->lli_cfg, sizeof(dma_lli_ctrl_t) * malloc_count);
        DMA_LLI_Update(dma_id, dma_ch, (uint32_t)dma_device->lli_cfg);
    } else {
        return -2;
    }
    if (lliCfg) {
        *lliCfg = dma_device->lli_cfg;
        dma_device->lli_cfg = NULL;
    }

    return 0;
}

void dma_channel_isr(uint32_t dma_id, int dma_ch)
{
    dma_device_t *dma_device = &dma_devices[dma_id][dma_ch];

    if ((DMA_GetIntStatus(dma_id) & (1 << dma_ch)) != 0) {
        DMA_ClrIntStatus(dma_id, dma_ch);
        if (dma_device->callback)
            dma_device->callback(dma_device, HAL_DMA_INT_COMPLETE);
    }

    if ((DMA_GetErrStatus(dma_id) & (1 << dma_ch)) != 0) {
        DMA_ClrErrStatus(dma_id, dma_ch);
        if (dma_device->callback)
            dma_device->callback(dma_device, HAL_DMA_INT_ERR);
    }
}

void DMA0_IRQHandler(void)
{
    for (uint8_t i = 0; i < DMA_MAX_INDEX; i++) {
        dma_channel_isr(DMA0_ID, i);
    }
}

/* ===================================================================
 *                    Runtime DMA channel allocator
 * ===================================================================
 *
 * Gated on CONFIG_DMA_CHANNEL_ALLOCATOR (bsp/Kconfig). See
 * claude_code_todo/dma_channel_allocator_design.md for the full design.
 *
 * Lifecycle:
 *   dma_request_channel(req, dir, "owner")
 *       - Finds a free slot, marks in_use, records owner/req.
 *       - Installs dma_device_t with direction + request routing.
 *       - Calls dma_channel_attach() (clock + channel regs; no
 *         global DMA_Disable).
 *       - Returns an opaque handle.
 *
 *   dma_handle_to_ch(h) -> raw channel index for passing through
 *       dma_reload / dma_channel_start during migration.
 *
 *   dma_release_channel(h)
 *       - Refcount--. On zero: stop, close (frees lli_cfg), return
 *         slot to free pool.
 *
 * Invariants:
 *   - Slot `in_use == true` implies this slot is allocator-owned.
 *     Raw-API callers MUST NOT dma_close() such a slot; the allocator
 *     owns lli_cfg lifecycle.
 *   - Mutex protects table updates; API is task-context only.
 * ================================================================= */

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR

/* One opaque handle per slot, indexed by (dma_id, ch). Callers only
 * see a pointer; they cannot modify internals. */
struct dma_channel_handle {
    uint8_t  dma_id;
    uint8_t  dma_ch;
};

struct dma_channel_slot {
    uint8_t      in_use;
    uint32_t     req;       /* peripheral request ID */
    uint8_t      dir;       /* M2M / M2P / P2M */
    uint16_t     refcount;
    const char  *owner;
    struct dma_channel_handle handle;
};

static struct dma_channel_slot s_alloc_table[DMA_ID_MAX][DMA_MAX_INDEX];
static SemaphoreHandle_t       s_alloc_mutex;
static uint8_t                 s_alloc_initted;

static void alloc_lazy_init(void)
{
    if (s_alloc_initted) {
        return;
    }
    /* One-shot mutex creation. Safe under the default FreeRTOS
     * scheduler-not-yet-started boot path because pvPortMalloc is
     * available from the very first heap_init. */
    if (!s_alloc_mutex) {
        s_alloc_mutex = xSemaphoreCreateMutex();
    }
    for (int id = 0; id < DMA_ID_MAX; id++) {
        for (int ch = 0; ch < DMA_MAX_INDEX; ch++) {
            s_alloc_table[id][ch].handle.dma_id = (uint8_t)id;
            s_alloc_table[id][ch].handle.dma_ch = (uint8_t)ch;
        }
    }
    s_alloc_initted = 1;
}

static void alloc_lock(void)
{
    if (s_alloc_mutex) {
        xSemaphoreTake(s_alloc_mutex, portMAX_DELAY);
    }
}

static void alloc_unlock(void)
{
    if (s_alloc_mutex) {
        xSemaphoreGive(s_alloc_mutex);
    }
}

/* Install a minimal dma_device_t from request/direction. The caller
 * of dma_request_channel is expected to customise the rest via
 * dma_setConfig + dma_ioctl afterwards (width, burst, callback, ...).
 * We fill only the channel + direction + request routing here so the
 * attach call has valid values. */
static void seed_device(int dma_id, int dma_ch, uint32_t req, uint8_t dir)
{
    dma_device_t *d = &dma_devices[dma_id][dma_ch];
    d->id = (uint8_t)dma_id;
    d->ch = (uint8_t)dma_ch;
    d->direction = dir;

    /* Peripheral request routing follows direction:
     *   M2P -> dst_req = req, src_req = NONE
     *   P2M -> src_req = req, dst_req = NONE
     *   M2M -> both NONE
     */
    if (dir == DMA_MEMORY_TO_PERIPH) {
        d->src_req = DMA_REQUEST_NONE;
        d->dst_req = req;
    } else if (dir == DMA_PERIPH_TO_MEMORY) {
        d->src_req = req;
        d->dst_req = DMA_REQUEST_NONE;
    } else {
        d->src_req = DMA_REQUEST_NONE;
        d->dst_req = DMA_REQUEST_NONE;
    }
}

dma_ch_handle_t dma_request_channel(uint32_t req, uint8_t dir,
                                    const char *owner)
{
    alloc_lazy_init();
    alloc_lock();

    /* Same-owner same-req reuse (refcount). Unless req is NONE
     * (M2M) where sharing makes no sense — each M2M caller needs its
     * own channel to avoid interleaving. */
    if (req != DMA_REQUEST_NONE) {
        for (int id = 0; id < DMA_ID_MAX; id++) {
            for (int ch = 0; ch < DMA_MAX_INDEX; ch++) {
                struct dma_channel_slot *s = &s_alloc_table[id][ch];
                if (s->in_use && s->req == req) {
                    if (s->owner == owner ||
                        (s->owner && owner && strcmp(s->owner, owner) == 0)) {
                        s->refcount++;
                        alloc_unlock();
                        return &s->handle;
                    }
                    printf("dma_alloc: req 0x%x already owned by '%s', "
                           "reject request from '%s'\n",
                           (unsigned)req, s->owner ? s->owner : "?",
                           owner ? owner : "?");
                    alloc_unlock();
                    return NULL;
                }
            }
        }
    }

    /* Allocate a fresh slot. Policy: walk DMA0 CH0..CH7 in order,
     * take the first free. */
    for (int ch = 0; ch < DMA_MAX_INDEX; ch++) {
        struct dma_channel_slot *s = &s_alloc_table[DMA0_ID][ch];
        if (!s->in_use) {
            s->in_use = 1;
            s->req = req;
            s->dir = dir;
            s->refcount = 1;
            s->owner = owner;

            seed_device(DMA0_ID, ch, req, dir);
            dma_channel_attach(DMA0_ID, ch);

            alloc_unlock();
            return &s->handle;
        }
    }

    printf("dma_alloc: no free channel for req 0x%x owner '%s'\n",
           (unsigned)req, owner ? owner : "?");
    alloc_unlock();
    return NULL;
}

void dma_release_channel(dma_ch_handle_t h)
{
    if (!h) return;

    alloc_lock();

    struct dma_channel_slot *s =
        &s_alloc_table[h->dma_id][h->dma_ch];

    if (!s->in_use) {
        /* Double release — warn but don't crash. */
        printf("dma_alloc: release of unowned ch%d\n", h->dma_ch);
        alloc_unlock();
        return;
    }

    if (s->refcount > 1) {
        s->refcount--;
        alloc_unlock();
        return;
    }

    /* Final release: tear down hardware, free the LLI chain. */
    DMA_Channel_Disable(h->dma_id, h->dma_ch);
    DMA_IntMask(h->dma_id, h->dma_ch, DMA_INT_ALL, MASK);

    dma_device_t *d = &dma_devices[h->dma_id][h->dma_ch];
    if (d->lli_cfg) {
        free(d->lli_cfg);
        d->lli_cfg = NULL;
    }
    d->callback = NULL;

    s->in_use = 0;
    s->req = DMA_REQUEST_NONE;
    s->refcount = 0;
    s->owner = NULL;

    alloc_unlock();
}

int dma_handle_to_ch(dma_ch_handle_t h)
{
    return h ? h->dma_ch : -1;
}

int dma_channel_is_allocator_owned(int dma_id, int dma_ch)
{
    if (!s_alloc_initted) return 0;
    if ((unsigned)dma_id >= DMA_ID_MAX) return 0;
    if ((unsigned)dma_ch >= DMA_MAX_INDEX) return 0;
    return s_alloc_table[dma_id][dma_ch].in_use ? 1 : 0;
}

void dma_dump_allocation(void)
{
    if (!s_alloc_initted) {
        printf("dma_alloc: allocator not initialised\n");
        return;
    }

    alloc_lock();
    printf("ch | in_use | owner          | req    | dir | refcnt\n");
    for (int id = 0; id < DMA_ID_MAX; id++) {
        for (int ch = 0; ch < DMA_MAX_INDEX; ch++) {
            struct dma_channel_slot *s = &s_alloc_table[id][ch];
            const char *dirstr =
                s->dir == DMA_MEMORY_TO_PERIPH ? "M2P" :
                s->dir == DMA_PERIPH_TO_MEMORY ? "P2M" :
                s->dir == DMA_MEMORY_TO_MEMORY ? "M2M" : "P2P";
            printf("%d  | %-6s | %-14s | 0x%04x | %s | %u\n",
                   ch, s->in_use ? "yes" : "no",
                   s->owner ? s->owner : "-",
                   (unsigned)s->req, dirstr,
                   (unsigned)s->refcount);
        }
    }
    alloc_unlock();
}

#endif /* CONFIG_DMA_CHANNEL_ALLOCATOR */
