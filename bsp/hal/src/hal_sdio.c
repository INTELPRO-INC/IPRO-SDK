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
 * SDIO host stack on top of hal_sdh / drv_sdh.
 *
 * Implements SDIO Simplified Specification CMD0/3/5/7/52/53, CCCR/FBR/CIS
 * parsing, multi-function management, and DAT1-line interrupt dispatch.
 *
 * Layered above drv_sdh.c's SDH_SendCommand() which already supports R4/R5
 * response types and ADMA2 transfers — we only add the SDIO IO semantics
 * (argument encoding, function ID, fixed-vs-incrementing addressing).
 */

#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "hal_sdio.h"
#include "drv_sdh.h"
#include "drv_glb.h"
#include "system_soc.h"
#include "ipro7.h"
#include "compiler/common.h"   /* ATTR_DTCM_SECTION */

#define SDIO_LOG(...)               printf("[sdio] " __VA_ARGS__)
#define SDIO_ERR(...)               printf("[sdio] err: " __VA_ARGS__)

#define SDIO_DEFAULT_INIT_CLOCK_HZ  400000U     /* identification mode */
#define SDIO_DEFAULT_FAST_CLOCK_HZ  25000000U   /* default speed */
#define SDIO_HIGH_SPEED_CLOCK_HZ    50000000U
#define SDIO_OCR_VOLTAGE_MASK       0x00FF8000U /* 3.0V/3.3V window */
#define SDIO_OCR_NIO_FUNCS_MASK     0x70000000U
#define SDIO_OCR_NIO_FUNCS_SHIFT    28U
#define SDIO_OCR_MEMORY_PRESENT     0x08000000U
#define SDIO_OCR_C_FLAG             0x80000000U /* card ready */
#define SDIO_CMD8_CHECK_PATTERN     0xAAU
#define SDIO_CMD8_VHS_27_36V        0x100U
#define SDIO_CIS_AREA_BASE          0x00001000U
#define SDIO_CIS_AREA_END           0x00017FFFU

/* CMD52 argument bits (per Simplified Spec) */
#define SDIO_CMD52_RW_FLAG          (1U << 31)
#define SDIO_CMD52_FUNC_SHIFT       28
#define SDIO_CMD52_RAW              (1U << 27)
#define SDIO_CMD52_REG_ADDR_SHIFT   9
#define SDIO_CMD52_REG_ADDR_MASK    0x1FFFFU
#define SDIO_CMD52_DATA_MASK        0xFFU

/* R5 response (8-bit response flags + read data byte) */
#define SDIO_R5_COM_CRC_ERROR       (1U << 15)
#define SDIO_R5_ILLEGAL_COMMAND     (1U << 14)
#define SDIO_R5_IO_CURRENT_STATE_M  (3U << 12)
#define SDIO_R5_ERROR               (1U << 11)
#define SDIO_R5_FUNCTION_NUMBER     (1U << 9)
#define SDIO_R5_OUT_OF_RANGE        (1U << 8)
#define SDIO_R5_DATA_MASK           0xFFU
#define SDIO_R5_ERROR_FLAGS \
    (SDIO_R5_COM_CRC_ERROR | SDIO_R5_ILLEGAL_COMMAND | SDIO_R5_ERROR | \
     SDIO_R5_FUNCTION_NUMBER | SDIO_R5_OUT_OF_RANGE)

/* ADMA2 descriptor pool — caller buffers must be 4-byte aligned.
 * The descriptor table MUST live in TCM (.tcm_data), not cached OCRAM/.bss:
 * the table is filled by the CPU and then fetched by the SDH ADMA master with
 * NO cache maintenance on it. In cached .bss the ADMA reads a stale descriptor
 * and parks the write data engine at ST_FDS (CMD53 write clocks no data,
 * wedges 209) — while reads happen to tolerate it. The in-tree SD-card path
 * (hal_sdh.c adma2Entries) already uses ATTR_DTCM_SECTION; match it. */
#define SDIO_ADMA2_ENTRIES          8
static ATTR_DTCM_SECTION __attribute__((aligned(64)))
SDH_ADMA2_Desc_Type s_adma_table[SDIO_ADMA2_ENTRIES];

/* Interrupts we care about during a CMD52/CMD53 transfer. */
#define SDIO_CMD_INT_MASK \
    (SDH_INT_CMD_COMPLETED | SDH_INT_CMD_ERRORS)
#define SDIO_DATA_INT_MASK \
    (SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS | SDH_INT_DMA_ERROR | \
     SDH_INT_DMA_COMPLETED)
#define SDIO_CARD_INT_MASK          SDH_INT_CARD

/* IRQ strategy (Phase A — narrow ISR for SDH_INT_CARD only).
 *
 * IPRO7 SDH peripheral has a CARD_INT bit (bit 8 of SDH_SD_NORMAL_INT_STATUS)
 * that signals DAT1-line interrupts from the SDIO device. SDH_IRQn (ECLIC
 * slot IRQ_NUM_BASE+35) is wired in HW but the global SDIO_SDCARD_INT_MODE
 * macro in hal_sdh.h that hooks ALL SDH events (CMD/data complete, errors,
 * insertion/removal) into one shared ISR is intentionally left disabled —
 * the SDH CMD/data path stays in polling mode (working and stable).
 *
 * We install a narrow ISR here that owns ONLY the SDH_INT_CARD bit:
 *   - sdio_host_init       — create sem, register sdio_sdh_isr at SDH_IRQn,
 *                            enable CPU-side IRQ. INT_SOURCE for CARD stays
 *                            disabled until the device-side function is
 *                            configured (sdio_enable_irq).
 *   - sdio_enable_irq      — enables INT_STATUS + INT_SOURCE for SDH_INT_CARD
 *                            and writes CCCR INT_ENABLE on the device side.
 *   - sdio_sdh_isr         — when CARD bit fires, clears it at the SDH and
 *                            xSemaphoreGiveFromISR's host->card_int_sem.
 *   - sdio_wait_irq        — takes the sem, then reads CCCR INT_PENDING via
 *                            CMD52 and dispatches to per-function handlers.
 *   - sdio_poll_irq        — legacy non-blocking path retained for callers
 *                            that don't want to block; it also drains
 *                            card_int_sem with zero timeout to stay coherent.
 */

/* ------------------------------------------------------------------ */
/* Single-host singleton.                                              */
/*                                                                     */
/* IPRO7 exposes one SDH controller — we keep an internal pointer to   */
/* the active host so the ISR can find it without per-IRQ user data.   */
/* ------------------------------------------------------------------ */
static SDIO_Host_Type *s_active_host = NULL;

/* Data-phase wait demotion (SDH_WaitDataHook in drv_sdh.c). IPRO7's SDH
 * interrupt does not work, so a CMD53's ~300 us data phase is polled; polled
 * at the SDIO reader's top priority it starved every task below it (the
 * SDIO-WiFi host threw ~60% of its received UDP datagrams away at the socket
 * queue). The task that registers here has that poll run at IDLE+1 instead —
 * after the command is on the bus, so the transfer overlaps the work of the
 * tasks that preempt the poll — and its base priority restored afterwards.
 * The registrant supplies its base priority because the wait runs under
 * host->mutex, where the current priority may be inherited and this kernel
 * has no uxTaskBasePriorityGet. Any other caller keeps the plain poll. */
static TaskHandle_t s_demote_task = NULL;
static UBaseType_t  s_demote_base = 0;

void sdio_set_wait_demotion(void *task, unsigned base_prio)
{
    s_demote_task = (TaskHandle_t)task;
    s_demote_base = (UBaseType_t)base_prio;
}

int SDH_WaitDataHook(SD_ID_Type sdId, uint32_t timeout_ms)
{
    const uint32_t done = SDH_INT_DATA_COMPLETED | SDH_INT_DATA_ERRORS |
                          SDH_INT_DMA_ERROR | SDH_INT_TUNE_ERROR;
    if (s_demote_task == NULL || xTaskGetCurrentTaskHandle() != s_demote_task) {
        return -1;
    }
    TickType_t t0 = xTaskGetTickCount();
    int rc = 0;
    vTaskPrioritySet(NULL, tskIDLE_PRIORITY + 1);
    while (!(SDH_GetIntStatus(sdId) & done)) {
        if ((xTaskGetTickCount() - t0) > pdMS_TO_TICKS(timeout_ms)) {
            rc = -1;
            break;
        }
    }
    /* Rewrites the base only; an inherited priority (a TX writer queued on
     * host->mutex meanwhile) stays until the outer release drops it. */
    vTaskPrioritySet(NULL, s_demote_base);
    return rc;
}

/* Forward declarations. */
static SDIO_Status_Type sdio_cmd52_raw(SDIO_Host_Type *host,
                                       uint8_t func, bool write, bool raw,
                                       uint32_t addr, uint8_t in_val,
                                       uint8_t *out_val);
static SDIO_Status_Type sdio_cmd53_xfer(SDIO_Host_Type *host,
                                        uint8_t func, bool write,
                                        bool block_mode, bool fixed_addr,
                                        uint32_t addr, uint8_t *buf,
                                        uint16_t count, uint16_t blk_size);
static SDIO_Status_Type sdio_check_r5(uint32_t resp, uint8_t *data);
static void sdio_sdh_isr(void);

/* ================================================================== */
/* Initialization / enumeration                                        */
/* ================================================================== */

static void sdh_phase_cache_invalidate(void);

/* SDH controller HW bring-up in 1-bit, ID-mode clock. Shared by sdio_host_init()
 * and sdio_host_reinit() — the latter re-runs ONLY this HW sequence so repeated
 * re-enumeration (e.g. phase calibration) doesn't recreate RTOS objects.
 * Mirror the SDH_Init sequence used by the SD-card path (hal_sdh.c)
 * — the order matters: peripheral SW reset first, then ungate clock,
 * then controller SW reset, then Ctrl_Init, then Set_Timeout (the
 * cfg.dataTimeout field is NOT applied by SDH_Ctrl_Init, only by
 * SDH_Set_Timeout), then Powon last. The previous order (Powon
 * before Reset) was a SW_RST_ALL-clears-SD_BUS_POWER trap and the
 * missing Set_Timeout left the data-timeout counter at 0x0000 so
 * the hardware never recovered a wedged data phase. */
static void sdh_host_hw_bringup(SD_ID_Type sd_id)
{
    SDH_Cfg_Type cfg = {
        .speed              = SDH_SPEED_MODE_SDR12,
        .vlot18Enable       = 0,
        .highSpeed          = 0,
        .dataWidth          = SDH_DATA_BUS_WIDTH_1BIT,
        .volt               = SDH_VOLTAGE_3P3V,
        .srcClock           = 96000000U,
        .busClock           = SDIO_DEFAULT_INIT_CLOCK_HZ,
        .dataTimeout        = 0xE,
        .readWatermarkLevel = 0x80,
        .writeWatermarkLevel= 0x80,
        .readBurstLen       = 0x8,
        .writeBurstLen      = 0x8,
    };
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDH);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);
    SDH_Reset(sd_id);
    SDH_Ctrl_Init(&cfg, sd_id);
    SDH_Set_Timeout(0x0e, sd_id);
    /* Clear residual UHS state in Host Control 2 (offset 0x3E). The
     * dump from a wedged CMD53 showed V18_EN=1 even though our cfg
     * passes vlot18Enable=0 — SDH_Ctrl_Init only writes HOST_CTRL_2
     * when vlot18Enable is non-zero, so a pre-set V18_EN survives.
     * If V18_EN sticks while the bus is wired for 3.3V, the data-line
     * PHY won't toggle and the host hangs in TX_ACTIVE forever. */
    writew(0, (uint32_t)SDH_BASE + SDH_HOST_CTRL_2_OFFSET);
    SDH_Powon(sd_id);
    /* HOST_CTRL_2 was just zeroed — any cached sampling-phase state is stale. */
    sdh_phase_cache_invalidate();
}

SDIO_Status_Type sdio_host_init(SDIO_Host_Type *host, SD_ID_Type sd_id)
{
    if (host == NULL) {
        return Status_SDIO_InvalidArg;
    }
    memset(host, 0, sizeof(*host));
    host->sd_id      = sd_id;
    host->bus_width  = SDIO_BUS_WIDTH_1BIT;
    host->mutex      = (void *)xSemaphoreCreateRecursiveMutex();
    if (host->mutex == NULL) {
        return Status_SDIO_InvalidArg;
    }
    host->card_int_sem = (void *)xSemaphoreCreateBinary();
    if (host->card_int_sem == NULL) {
        return Status_SDIO_InvalidArg;
    }

    sdh_host_hw_bringup(sd_id);

    /* INT_STATUS for SDH_INT_CARD is enabled so SDH_GetIntStatus()
     * latches the device's DAT1 pulse; INT_SOURCE (which actually fires
     * the ECLIC line) is deferred to sdio_enable_irq() — we don't want
     * IRQs to fire before the device side is configured. */
    s_active_host = host;
    SDH_EnableIntStatus(SDH_INT_CARD, sd_id);

    Interrupt_Handler_Register(SDH_IRQn, sdio_sdh_isr);
    CPU_Interrupt_Enable(SDH_IRQn);

    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_host_reset(SDIO_Host_Type *host)
{
    if (host == NULL) return Status_SDIO_InvalidArg;
    SDH_Reset(host->sd_id);
    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_claim_host(SDIO_Host_Type *host)
{
    if (host == NULL || host->mutex == NULL) return Status_SDIO_InvalidArg;
    xSemaphoreTakeRecursive((SemaphoreHandle_t)host->mutex, portMAX_DELAY);
    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_release_host(SDIO_Host_Type *host)
{
    if (host == NULL || host->mutex == NULL) return Status_SDIO_InvalidArg;
    xSemaphoreGiveRecursive((SemaphoreHandle_t)host->mutex);
    return Status_SDIO_Success;
}

/* CMD0 — GO_IDLE. No response. */
static SDIO_Status_Type sdio_cmd0(SDIO_Host_Type *host)
{
    SDH_CMD_Cfg_Type cmd = {
        .index    = 0,
        .argument = 0,
        .type     = SDH_CMD_NORMAL,
        .respType = SDH_RESP_NONE,
        .flag     = 0,
    };
    SDH_SendCommand(&cmd, host->sd_id);
    if (SDH_WaitCommandDone(&cmd, host->sd_id) != SDH_STAT_SUCCESS) {
        return Status_SDIO_CmdTimeout;
    }
    return Status_SDIO_Success;
}

/* CMD8 — SEND_IF_COND. Used to advertise host voltage to v2.x cards.
 * R7 response. Pure-SDIO cards may NOT respond — we tolerate timeout. */
static SDIO_Status_Type sdio_cmd8(SDIO_Host_Type *host)
{
    SDH_CMD_Cfg_Type cmd = {
        .index    = 8,
        .argument = SDIO_CMD8_VHS_27_36V | SDIO_CMD8_CHECK_PATTERN,
        .type     = SDH_CMD_NORMAL,
        .respType = SDH_RESP_R7,
        .flag     = 0,
    };
    SDH_SendCommand(&cmd, host->sd_id);
    (void)SDH_WaitCommandDone(&cmd, host->sd_id);
    return Status_SDIO_Success;  /* timeout OK for SDIO-only cards */
}

/* CMD5 — IO_SEND_OP_COND. R4 response (OCR + nf + memPresent + Cflag). */
static SDIO_Status_Type sdio_cmd5(SDIO_Host_Type *host, uint32_t ocr_arg,
                                  uint32_t *out_resp)
{
    SDH_CMD_Cfg_Type cmd = {
        .index    = 5,
        .argument = ocr_arg,
        .type     = SDH_CMD_NORMAL,
        .respType = SDH_RESP_R4,
        .flag     = 0,
    };
    SDH_SendCommand(&cmd, host->sd_id);
    if (SDH_WaitCommandDone(&cmd, host->sd_id) != SDH_STAT_SUCCESS) {
        return Status_SDIO_CmdTimeout;
    }
    if (out_resp) *out_resp = cmd.response[0];
    return Status_SDIO_Success;
}

/* CMD3 — SEND_RELATIVE_ADDR. R6 (RCA in upper 16 bits). */
static SDIO_Status_Type sdio_cmd3(SDIO_Host_Type *host, uint16_t *out_rca)
{
    SDH_CMD_Cfg_Type cmd = {
        .index    = 3,
        .argument = 0,
        .type     = SDH_CMD_NORMAL,
        .respType = SDH_RESP_R6,
        .flag     = 0,
    };
    SDH_SendCommand(&cmd, host->sd_id);
    if (SDH_WaitCommandDone(&cmd, host->sd_id) != SDH_STAT_SUCCESS) {
        return Status_SDIO_CmdTimeout;
    }
    *out_rca = (uint16_t)(cmd.response[0] >> 16);
    return Status_SDIO_Success;
}

/* CMD7 — SELECT_CARD. R1b. */
static SDIO_Status_Type sdio_cmd7(SDIO_Host_Type *host, uint16_t rca)
{
    SDH_CMD_Cfg_Type cmd = {
        .index    = 7,
        .argument = ((uint32_t)rca) << 16,
        .type     = SDH_CMD_NORMAL,
        .respType = SDH_RESP_R1B,
        .flag     = 0,
    };
    SDH_SendCommand(&cmd, host->sd_id);
    if (SDH_WaitCommandDone(&cmd, host->sd_id) != SDH_STAT_SUCCESS) {
        return Status_SDIO_CmdTimeout;
    }
    return Status_SDIO_Success;
}

/* Walk a CIS tuple chain. Caller-provided callback returns non-zero to stop. */
SDIO_Status_Type sdio_walk_cis(SDIO_Host_Type *host, uint8_t func_num,
                               uint32_t start_ptr,
                               SDIO_CIS_Tuple_CB cb, void *arg)
{
    /* CIS tuple link byte is 8-bit (max 255), so a 255-byte body
     * captures any tuple end-to-end without truncation. */
    uint32_t addr = start_ptr;
    uint8_t  body[255];
    SDIO_Status_Type st;

    if (addr < SDIO_CIS_AREA_BASE || addr > SDIO_CIS_AREA_END) {
        return Status_SDIO_InvalidArg;
    }
    while (addr <= SDIO_CIS_AREA_END) {
        uint8_t code = 0, link = 0;
        st = sdio_readb(host, SDIO_FUNC_0, addr++, &code);
        if (st != Status_SDIO_Success) return st;
        if (code == SDIO_CISTPL_END) break;
        if (code == SDIO_CISTPL_NULL) continue;
        st = sdio_readb(host, SDIO_FUNC_0, addr++, &link);
        if (st != Status_SDIO_Success) return st;
        for (uint8_t i = 0; i < link; i++) {
            st = sdio_readb(host, SDIO_FUNC_0, addr + i, &body[i]);
            if (st != Status_SDIO_Success) return st;
        }
        if (cb && cb(code, link, body, link, arg) != 0) break;
        addr += link;
        (void)func_num;  /* func currently unused; CIS lives in F0 space */
    }
    return Status_SDIO_Success;
}

/* CIS callback: extract MANFID + FUNCID + CISTPL_FUNCE block size. */
struct cis_collect {
    SDIO_Func_Info_Type *finfo;
    uint8_t             want_func_class;
};

static int cis_collect_cb(uint8_t code, uint8_t link,
                          const uint8_t *body, uint8_t body_len, void *arg)
{
    struct cis_collect *c = (struct cis_collect *)arg;
    (void)link;
    switch (code) {
    case SDIO_CISTPL_MANFID:
        if (body_len >= 4) {
            c->finfo->manf_id =
                (uint16_t)(body[0] | (body[1] << 8));
            c->finfo->prod_id =
                (uint16_t)(body[2] | (body[3] << 8));
        }
        break;
    case SDIO_CISTPL_FUNCID:
        if (body_len >= 1) c->finfo->func_class = body[0];
        break;
    case SDIO_CISTPL_FUNCE:
        /* type 1 (per-function) carries max block size at body[12..13] */
        if (body_len >= 14 && body[0] == 0x01) {
            c->finfo->max_blk_size =
                (uint16_t)(body[12] | (body[13] << 8));
        }
        break;
    default: break;
    }
    return 0;
}

SDIO_Status_Type sdio_card_enumerate(SDIO_Host_Type *host)
{
    SDIO_Status_Type st;
    uint32_t r4 = 0;
    uint32_t cccr_cis_lo = 0, cccr_cis_mid = 0, cccr_cis_hi = 0;

    if (host == NULL) return Status_SDIO_InvalidArg;

    /* Step 1: idle the bus */
    st = sdio_cmd0(host);
    if (st != Status_SDIO_Success) return st;

    /* Step 2: optional CMD8 (SD v2 voltage advertise — SDIO ignores) */
    (void)sdio_cmd8(host);

    /* Step 3: CMD5 OCR negotiation. First call with arg=0 reads OCR;
     * second call asserts our voltage window and waits for C flag. */
    st = sdio_cmd5(host, 0, &r4);
    if (st != Status_SDIO_Success) return st;
    if ((r4 & SDIO_OCR_NIO_FUNCS_MASK) == 0) {
        /* No IO functions reported — not an SDIO card. */
        return Status_SDIO_NotSDIOCard;
    }
    host->ocr = r4 & SDIO_OCR_VOLTAGE_MASK;
    host->num_funcs =
        (uint8_t)((r4 & SDIO_OCR_NIO_FUNCS_MASK) >> SDIO_OCR_NIO_FUNCS_SHIFT);

    for (int retry = 0; retry < 100; retry++) {
        st = sdio_cmd5(host, host->ocr, &r4);
        if (st != Status_SDIO_Success) return st;
        if (r4 & SDIO_OCR_C_FLAG) break;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if ((r4 & SDIO_OCR_C_FLAG) == 0) {
        return Status_SDIO_CmdTimeout;
    }

    /* Step 4: CMD3 — get RCA */
    st = sdio_cmd3(host, &host->rca);
    if (st != Status_SDIO_Success) return st;

    /* Step 5: CMD7 — select card by RCA. From here all CMD52/53 work. */
    st = sdio_cmd7(host, host->rca);
    if (st != Status_SDIO_Success) return st;

    /* Step 6: read common CIS pointer from CCCR (3 bytes LE) */
    {
        uint8_t b0 = 0, b1 = 0, b2 = 0;
        st = sdio_readb(host, SDIO_FUNC_0,
                        SDIO_CCCR_COMMON_CIS_PTR + 0, &b0);
        if (st != Status_SDIO_Success) return st;
        st = sdio_readb(host, SDIO_FUNC_0,
                        SDIO_CCCR_COMMON_CIS_PTR + 1, &b1);
        if (st != Status_SDIO_Success) return st;
        st = sdio_readb(host, SDIO_FUNC_0,
                        SDIO_CCCR_COMMON_CIS_PTR + 2, &b2);
        if (st != Status_SDIO_Success) return st;
        cccr_cis_lo = b0; cccr_cis_mid = b1; cccr_cis_hi = b2;
        host->common_cis_ptr =
            cccr_cis_lo | (cccr_cis_mid << 8) | (cccr_cis_hi << 16);
    }

    /* Step 7: per-function FBR CIS pointer + max block size */
    for (uint8_t f = 1; f <= host->num_funcs && f <= SDIO_MAX_FUNCS; f++) {
        SDIO_Func_Info_Type *fi = &host->funcs[f];
        uint32_t fbr = SDIO_FBR_BASE(f);
        uint8_t  b0 = 0, b1 = 0, b2 = 0;

        fi->func_num = f;
        (void)sdio_readb(host, SDIO_FUNC_0, fbr + SDIO_FBR_CIS_PTR_OFFSET + 0, &b0);
        (void)sdio_readb(host, SDIO_FUNC_0, fbr + SDIO_FBR_CIS_PTR_OFFSET + 1, &b1);
        (void)sdio_readb(host, SDIO_FUNC_0, fbr + SDIO_FBR_CIS_PTR_OFFSET + 2, &b2);
        fi->cis_ptr = b0 | (b1 << 8) | (b2 << 16);

        /* Walk CIS to fill manf/prod/class/max_blk. */
        struct cis_collect cc = { .finfo = fi };
        if (fi->cis_ptr) {
            (void)sdio_walk_cis(host, f, fi->cis_ptr, cis_collect_cb, &cc);
        }
    }

    SDIO_LOG("enumerated rca=0x%04X funcs=%u manf=0x%04X prod=0x%04X\r\n",
             host->rca, host->num_funcs,
             host->funcs[1].manf_id, host->funcs[1].prod_id);
    return Status_SDIO_Success;
}

/* CCCR bus-interface-ctrl byte (width bits masked off), cached for sdh_set_width_fast.
 * 0xff = uninitialised; invalidated whenever sdio_set_bus_width reprograms CCCR_IF. */
static uint8_t s_cccr_if_base = 0xffu;

SDIO_Status_Type sdio_set_bus_width(SDIO_Host_Type *host, uint8_t width)
{
    SDIO_Status_Type st;
    uint8_t bic = 0;

    if (host == NULL) return Status_SDIO_InvalidArg;
    if (width != SDIO_BUS_WIDTH_1BIT && width != SDIO_BUS_WIDTH_4BIT) {
        return Status_SDIO_InvalidArg;
    }
    st = sdio_readb(host, SDIO_FUNC_0, SDIO_CCCR_BUS_INTERFACE_CTRL, &bic);
    if (st != Status_SDIO_Success) return st;
    bic = (bic & ~0x03U) | width;
    st = sdio_writeb(host, SDIO_FUNC_0, SDIO_CCCR_BUS_INTERFACE_CTRL, bic);
    if (st != Status_SDIO_Success) return Status_SDIO_BusWidthReject;

    /* Update SDH controller side. High Speed (50 MHz @ 3.3V) is selected by
     * HI_SPEED_EN (cfg.highSpeed), NOT by a UHS mode: SDR50/SDR104/DDR50 are
     * UHS-I modes that require 1.8V signalling. At 3.3V the UHS-mode field
     * stays SDR25 and the 50 MHz step is purely HI_SPEED_EN + busClock. */
    SDH_Cfg_Type cfg = {
        .speed = SDH_SPEED_MODE_SDR25,
        .highSpeed = host->high_speed ? 1 : 0,
        .dataWidth = (width == SDIO_BUS_WIDTH_4BIT)
                        ? SDH_DATA_BUS_WIDTH_4BITS
                        : SDH_DATA_BUS_WIDTH_1BIT,
        .volt = SDH_VOLTAGE_3P3V,
        .srcClock = 96000000U,
        .busClock = host->high_speed ? SDIO_HIGH_SPEED_CLOCK_HZ
                                      : SDIO_DEFAULT_FAST_CLOCK_HZ,
        .dataTimeout = 0xE,
        .readWatermarkLevel  = 0x80,
        .writeWatermarkLevel = 0x80,
        .readBurstLen  = 0x8,
        .writeBurstLen = 0x8,
    };
    SDH_Ctrl_Init(&cfg, host->sd_id);
    host->bus_width = width;
    s_cccr_if_base = 0xffu;   /* CCCR_IF was just reprogrammed — refresh the fast-switch cache */
    /* SDH_Ctrl_Init may touch HOST_CTRL_2 (V18 path) — treat cached phase as stale. */
    sdh_phase_cache_invalidate();
    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_set_high_speed(SDIO_Host_Type *host, bool enable)
{
    SDIO_Status_Type st;
    uint8_t hs = 0;

    if (host == NULL) return Status_SDIO_InvalidArg;
    st = sdio_readb(host, SDIO_FUNC_0, SDIO_CCCR_HIGHSPEED, &hs);
    if (st != Status_SDIO_Success) return st;
    if (enable) {
        if ((hs & 0x01) == 0) {
            /* Card does not support HS. */
            return Status_SDIO_HighSpeedReject;
        }
        hs |= 0x02;     /* EHS bit */
    } else {
        hs &= ~0x02U;
    }
    st = sdio_writeb(host, SDIO_FUNC_0, SDIO_CCCR_HIGHSPEED, hs);
    if (st != Status_SDIO_Success) return st;
    host->high_speed = enable ? 1 : 0;
    return sdio_set_clock(host,
                          enable ? SDIO_HIGH_SPEED_CLOCK_HZ
                                 : SDIO_DEFAULT_FAST_CLOCK_HZ);
}

SDIO_Status_Type sdio_set_clock(SDIO_Host_Type *host, uint32_t hz)
{
    if (host == NULL) return Status_SDIO_InvalidArg;
    SDH_SetSdClock(96000000U, hz, host->sd_id);
    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_set_block_size(SDIO_Host_Type *host, uint8_t func_num,
                                     uint16_t block_size)
{
    SDIO_Status_Type st;
    uint32_t addr;

    if (host == NULL || func_num > host->num_funcs) {
        return Status_SDIO_InvalidArg;
    }
    addr = (func_num == 0) ? SDIO_CCCR_FN0_BLOCK_SIZE
                           : SDIO_FBR_BASE(func_num) +
                             SDIO_FBR_BLOCK_SIZE_OFFSET;

    st = sdio_writeb(host, SDIO_FUNC_0, addr,     (uint8_t)(block_size & 0xFF));
    if (st != Status_SDIO_Success) return Status_SDIO_BlockSizeReject;
    st = sdio_writeb(host, SDIO_FUNC_0, addr + 1, (uint8_t)((block_size >> 8) & 0xFF));
    if (st != Status_SDIO_Success) return Status_SDIO_BlockSizeReject;

    if (func_num <= SDIO_MAX_FUNCS) {
        host->funcs[func_num].cur_blk_size = block_size;
    }
    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_enable_func(SDIO_Host_Type *host, uint8_t func_num)
{
    SDIO_Status_Type st;
    uint8_t ioe = 0, ior = 0;

    if (host == NULL || func_num == 0 || func_num > host->num_funcs) {
        return Status_SDIO_InvalidArg;
    }
    st = sdio_readb(host, SDIO_FUNC_0, SDIO_CCCR_IO_ENABLE, &ioe);
    if (st != Status_SDIO_Success) return st;
    ioe |= (1U << func_num);
    st = sdio_writeb(host, SDIO_FUNC_0, SDIO_CCCR_IO_ENABLE, ioe);
    if (st != Status_SDIO_Success) return Status_SDIO_FuncEnableFail;

    /* Poll IO Ready. */
    for (int retry = 0; retry < 200; retry++) {
        (void)sdio_readb(host, SDIO_FUNC_0, SDIO_CCCR_IO_READY, &ior);
        if (ior & (1U << func_num)) {
            host->funcs[func_num].enabled = 1;
            return Status_SDIO_Success;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return Status_SDIO_FuncEnableFail;
}

SDIO_Status_Type sdio_disable_func(SDIO_Host_Type *host, uint8_t func_num)
{
    uint8_t ioe = 0;
    SDIO_Status_Type st;

    if (host == NULL || func_num == 0) return Status_SDIO_InvalidArg;
    st = sdio_readb(host, SDIO_FUNC_0, SDIO_CCCR_IO_ENABLE, &ioe);
    if (st != Status_SDIO_Success) return st;
    ioe &= ~(1U << func_num);
    st = sdio_writeb(host, SDIO_FUNC_0, SDIO_CCCR_IO_ENABLE, ioe);
    if (st == Status_SDIO_Success && func_num <= SDIO_MAX_FUNCS) {
        host->funcs[func_num].enabled = 0;
    }
    return st;
}

/* ================================================================== */
/* CMD52 — IO_RW_DIRECT                                                */
/* ================================================================== */

static SDIO_Status_Type sdio_check_r5(uint32_t resp, uint8_t *data)
{
    if (resp & SDIO_R5_ERROR_FLAGS) {
        return Status_SDIO_R5Error;
    }
    if (data) *data = (uint8_t)(resp & SDIO_R5_DATA_MASK);
    return Status_SDIO_Success;
}

static SDIO_Status_Type sdio_cmd52_raw(SDIO_Host_Type *host,
                                       uint8_t func, bool write, bool raw,
                                       uint32_t addr, uint8_t in_val,
                                       uint8_t *out_val)
{
    SDH_CMD_Cfg_Type cmd;
    uint32_t arg = 0;

    if (host == NULL) return Status_SDIO_InvalidArg;
    if (func > SDIO_MAX_FUNCS) return Status_SDIO_InvalidArg;
    if ((addr & ~SDIO_CMD52_REG_ADDR_MASK) != 0) return Status_SDIO_InvalidArg;

    if (write) arg |= SDIO_CMD52_RW_FLAG;
    arg |= ((uint32_t)func) << SDIO_CMD52_FUNC_SHIFT;
    if (raw) arg |= SDIO_CMD52_RAW;
    arg |= (addr & SDIO_CMD52_REG_ADDR_MASK) << SDIO_CMD52_REG_ADDR_SHIFT;
    if (write) arg |= (in_val & SDIO_CMD52_DATA_MASK);

    memset(&cmd, 0, sizeof(cmd));
    cmd.index    = 52;
    cmd.argument = arg;
    cmd.type     = SDH_CMD_NORMAL;
    cmd.respType = SDH_RESP_R5;

    xSemaphoreTakeRecursive((SemaphoreHandle_t)host->mutex, portMAX_DELAY);
    SDH_SendCommand(&cmd, host->sd_id);
    SDH_Stat_Type sdh_st = SDH_WaitCommandDone(&cmd, host->sd_id);
    xSemaphoreGiveRecursive((SemaphoreHandle_t)host->mutex);

    if (sdh_st != SDH_STAT_SUCCESS) return Status_SDIO_CmdTimeout;
    return sdio_check_r5(cmd.response[0], out_val);
}

SDIO_Status_Type sdio_readb(SDIO_Host_Type *host, uint8_t func_num,
                            uint32_t addr, uint8_t *out_val)
{
    if (out_val == NULL) return Status_SDIO_InvalidArg;
    return sdio_cmd52_raw(host, func_num, false, false, addr, 0, out_val);
}

SDIO_Status_Type sdio_writeb(SDIO_Host_Type *host, uint8_t func_num,
                             uint32_t addr, uint8_t val)
{
    return sdio_cmd52_raw(host, func_num, true, false, addr, val, NULL);
}

SDIO_Status_Type sdio_writeb_readback(SDIO_Host_Type *host, uint8_t func_num,
                                      uint32_t addr, uint8_t val,
                                      uint8_t *out_old)
{
    return sdio_cmd52_raw(host, func_num, true, true, addr, val, out_old);
}

/* ================================================================== */
/* CMD53 — IO_RW_EXTENDED                                              */
/* ================================================================== */

/* When set, route SMALL CMD53 transfers (<= SDH_SMALL_PIO_MAX bytes) through PIO
 * (dmaCfg=NULL -> SDH polls the buffer data port) instead of ADMA2. A tiny 4-byte
 * ADMA descriptor can fail to engage the SDH data engine at HS/4-bit (data phase
 * never goes active -> DataTimeout) while large block transfers work; PIO has no
 * such descriptor. Bulk transfers stay on ADMA. Default off (no behaviour change). */
int g_sdh_small_pio = 0;
#define SDH_SMALL_PIO_MAX 64u
void sdio_set_small_pio(int en) { g_sdh_small_pio = en ? 1 : 0; }

/* Accept READ data even when the SDH flags DATA_CRC (but NOT on data-timeout). On the
 * 4-bit flywire the SDH clocks one extra nibble in the read data phase: the data body is
 * captured correct but the CRC window is off -> a FALSE CRC error on good data. Setting
 * enableIgnoreError lets the verified-correct data through; callers re-verify (read-twice). */
int g_sdh_ignore_rd_crc = 0;
void sdio_set_ignore_rd_crc(int en) { g_sdh_ignore_rd_crc = en ? 1 : 0; }

/* Route every READ at 1-bit (clean, no 4-bit false-CRC) while WRITES stay at the configured
 * width (e.g. 4-bit for TX throughput). The switch is lightweight (card CCCR + host data-width
 * bit, no clock/Ctrl_Init) and is done INSIDE the per-transfer mutex in sdio_cmd53_xfer, so it
 * is atomic against concurrent bh/TX threads — unlike a caller-side width flip. */
int g_sdh_reads_1bit = 0;
void sdio_set_reads_1bit(int en) { g_sdh_reads_1bit = en ? 1 : 0; }

static void sdh_set_width_fast(SDIO_Host_Type *host, uint8_t width)
{
    if (s_cccr_if_base == 0xffu) {
        uint8_t bic = 0;
        if (sdio_readb(host, SDIO_FUNC_0, SDIO_CCCR_BUS_INTERFACE_CTRL, &bic) == Status_SDIO_Success) {
            s_cccr_if_base = (uint8_t)(bic & ~0x03u);
        } else {
            s_cccr_if_base = 0;
        }
    }
    (void)sdio_writeb(host, SDIO_FUNC_0, SDIO_CCCR_BUS_INTERFACE_CTRL,
                      (uint8_t)(s_cccr_if_base | (width & 0x03u)));
    uint16_t hc = readw((uint32_t)SDH_BASE + SDH_SD_HOST_CTRL_OFFSET);
    if (width == SDIO_BUS_WIDTH_4BIT) {
        hc |= (uint16_t)(1u << SDH_DATA_WIDTH_POS);
    } else {
        hc &= (uint16_t)~(1u << SDH_DATA_WIDTH_POS);
    }
    writew(hc, (uint32_t)SDH_BASE + SDH_SD_HOST_CTRL_OFFSET);
    host->bus_width = width;
}

/* Select the io_dly-DELAYED sampling clock for reads (HOST_CTRL_2 bit7 SAMPLING_CLK_SEL).
 * Default SDH init clears HOST_CTRL_2=0, so reads sample on the FIXED clock and the GLB
 * smih input-delay taps have no effect. Setting bit7 makes the SDH sample with the
 * delayed clock so those taps actually shift the read capture point. */
void sdio_set_sampling_clk_sel(int en)
{
    uint16_t v = readw((uint32_t)SDH_BASE + SDH_HOST_CTRL_2_OFFSET);
    if (en) {
        v |= (1u << 7);
    } else {
        v &= (uint16_t)~(1u << 7);
    }
    writew(v, (uint32_t)SDH_BASE + SDH_HOST_CTRL_2_OFFSET);
}

/* ---- Per-transfer-direction sampling phase ------------------------------------
 * The read-optimal and write-optimal sampling phases differ on marginal wiring:
 * a shifted read-capture phase (GLB smih io_dly taps + SAMPLING_CLK_SEL) recovers
 * 4-bit reads, but the same shift makes the host mis-sample the card's post-WRITE
 * CRC-status token on DAT0 -> false DATA_TIMEOUT. Both settings are global, so the
 * only way to serve both directions is to switch phase per transfer, keyed on
 * direction, inside sdio_cmd53_xfer's mutex (same spot as the reads-1bit width
 * switch). A last-applied cache makes consecutive same-direction transfers cost
 * one compare and zero register writes. Default off: zero behavior change. */
static int g_sdh_dir_phase_en = 0;
static SDIO_Phase_Type g_sdh_rd_phase, g_sdh_wr_phase;
static uint32_t s_phase_last_iodly = 0xffffffffu;   /* invalid = force re-apply */
static int      s_phase_last_samp  = -1;

static void sdh_phase_cache_invalidate(void)
{
    s_phase_last_iodly = 0xffffffffu;
    s_phase_last_samp  = -1;
}

static void sdh_apply_phase(const SDIO_Phase_Type *p)
{
    if (p->io_dly != s_phase_last_iodly) {
        *(volatile uint32_t *)((uint32_t)GLB_BASE + GLB_SMIH_0_IO_DLY_0_OFFSET) = p->io_dly;
        s_phase_last_iodly = p->io_dly;
    }
    if ((int)p->samp_sel != s_phase_last_samp) {
        sdio_set_sampling_clk_sel(p->samp_sel);
        s_phase_last_samp = (int)p->samp_sel;
    }
}

void sdio_set_dir_phase(const SDIO_Phase_Type *rd_phase,
                        const SDIO_Phase_Type *wr_phase, int enable)
{
    if (enable && rd_phase != NULL && wr_phase != NULL) {
        g_sdh_rd_phase = *rd_phase;
        g_sdh_wr_phase = *wr_phase;
        g_sdh_dir_phase_en = 1;
    } else {
        g_sdh_dir_phase_en = 0;
    }
    /* Force re-apply on the next transfer (or leave HW as-is when disabling —
     * the caller is responsible for restoring its preferred global phase). */
    sdh_phase_cache_invalidate();
}

SDIO_Status_Type sdio_host_reinit(SDIO_Host_Type *host)
{
    if (host == NULL || host->mutex == NULL) {
        return Status_SDIO_InvalidArg;   /* must have been sdio_host_init'ed once */
    }
    xSemaphoreTakeRecursive((SemaphoreHandle_t)host->mutex, portMAX_DELAY);
    sdh_host_hw_bringup(host->sd_id);
    /* Enumeration state is void after the controller reset. */
    host->rca         = 0;
    host->num_funcs   = 0;
    host->common_cis_ptr = 0;
    memset(host->funcs, 0, sizeof(host->funcs));
    host->bus_width   = SDIO_BUS_WIDTH_1BIT;
    host->high_speed  = 0;               /* the bring-up above programmed highSpeed = 0 */
    s_cccr_if_base    = 0xffu;
    SDH_EnableIntStatus(SDH_INT_CARD, host->sd_id);
    xSemaphoreGiveRecursive((SemaphoreHandle_t)host->mutex);
    return Status_SDIO_Success;
}

static SDIO_Status_Type sdio_cmd53_xfer(SDIO_Host_Type *host,
                                        uint8_t func, bool write,
                                        bool block_mode, bool fixed_addr,
                                        uint32_t addr, uint8_t *buf,
                                        uint16_t count, uint16_t blk_size)
{
    SDH_CMD_Cfg_Type cmd;
    SDH_Data_Cfg_Type data;
    SDH_DMA_Cfg_Type  dma;
    SDH_Trans_Cfg_Type trans;
    uint32_t arg = 0;
    uint32_t total_bytes;

    if (host == NULL || buf == NULL) return Status_SDIO_InvalidArg;
    if (func > SDIO_MAX_FUNCS) return Status_SDIO_InvalidArg;
    if ((((uintptr_t)buf) & 0x3) != 0) return Status_SDIO_BufferAlignment;
    if (count > SDIO_CMD53_BYTE_BLOCK_COUNT_MAX) return Status_SDIO_InvalidArg;
    if (block_mode && blk_size == 0) return Status_SDIO_InvalidArg;
    if ((addr & ~SDIO_CMD52_REG_ADDR_MASK) != 0) return Status_SDIO_InvalidArg;

    if (write)             arg |= SDIO_CMD53_RW_FLAG;
    arg |= ((uint32_t)func) << SDIO_CMD53_FUNC_SHIFT;
    if (block_mode)        arg |= SDIO_CMD53_BLOCK_MODE;
    if (!fixed_addr)       arg |= SDIO_CMD53_OP_INC_ADDR;
    arg |= (addr & SDIO_CMD52_REG_ADDR_MASK) << SDIO_CMD53_REG_ADDR_SHIFT;
    /* Byte mode: count==0 means 512 bytes; we keep the natural mapping. */
    arg |= (count & SDIO_CMD53_BYTE_BLOCK_COUNT_MAX);

    total_bytes = block_mode ? ((uint32_t)count * blk_size) : count;
    if (block_mode && count == 0) total_bytes = blk_size;
    if (!block_mode && count == 0) total_bytes = 512;

    memset(&cmd, 0, sizeof(cmd));
    memset(&data, 0, sizeof(data));
    memset(&dma, 0, sizeof(dma));
    cmd.index    = 53;
    cmd.argument = arg;
    cmd.type     = SDH_CMD_NORMAL;
    cmd.respType = SDH_RESP_R5;

    data.blockSize  = block_mode ? blk_size : total_bytes;
    data.blockCount = block_mode ? (count == 0 ? 1 : count) : 1;
    if (write) {
        data.txData     = (const uint32_t *)buf;
        data.txDataLen  = total_bytes;
    } else {
        data.rxData     = (uint32_t *)buf;
        data.rxDataLen  = total_bytes;
    }
    data.dataType = SDH_TRANS_DATA_NORMAL;
    data.enableIgnoreError = (!write) && g_sdh_ignore_rd_crc;

    dma.dmaMode    = SDH_DMA_MODE_ADMA2;
    dma.admaEntries = (uint32_t *)s_adma_table;
    dma.maxEntries = SDIO_ADMA2_ENTRIES;

    trans.cmdCfg  = &cmd;
    trans.dataCfg = &data;

    /* ADMA2 fetches from / writes to physical RAM. Cached buffers in OCRAM
     * (s_tx_buf at 0x110xxxxx in ipro6le_probe) need an explicit cache
     * sync: clean before write so DMA sees CPU stores; clean+invalidate
     * around read so the post-DMA CPU load picks up RAM, not stale cache.
     * Without this, the IPRO6LE SDIO target sees a 2048-byte block of zeros
     * (the pre-build memset state in RAM) and never asserts data-done. */
    if (write) {
        L1C_DCACHE_CLEAN_RANGE((uintptr_t)buf, total_bytes);
    } else {
        L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)buf, total_bytes);
    }

    /* Small transfers: optionally use PIO (dmaCfg=NULL) — a 4-byte ADMA descriptor can
     * fail to engage the data engine at HS/4-bit. Bulk stays on ADMA. */
    bool use_pio = (g_sdh_small_pio != 0) && (total_bytes <= SDH_SMALL_PIO_MAX);

    xSemaphoreTakeRecursive((SemaphoreHandle_t)host->mutex, portMAX_DELAY);
    /* Direction-keyed sampling phase (see sdio_set_dir_phase). Applied before the
     * transfer, under the mutex, bus idle. No post-transfer restore: the last-applied
     * cache flips it on the next opposite-direction transfer instead. */
    if (g_sdh_dir_phase_en) {
        sdh_apply_phase(write ? &g_sdh_wr_phase : &g_sdh_rd_phase);
    }
    /* Reads at 1-bit (clean) while keeping the configured width for writes — atomic here
     * under the mutex so concurrent threads can't transfer at the wrong width. */
    int restore_width = 0;
    uint8_t saved_width = host->bus_width;
    if (!write && g_sdh_reads_1bit && host->bus_width != SDIO_BUS_WIDTH_1BIT) {
        sdh_set_width_fast(host, SDIO_BUS_WIDTH_1BIT);
        restore_width = 1;
    }
    SDH_Stat_Type sdh_st = SDH_TransferBlocking(use_pio ? NULL : &dma, &trans, host->sd_id);
    if (restore_width) {
        sdh_set_width_fast(host, saved_width);
    }
    xSemaphoreGiveRecursive((SemaphoreHandle_t)host->mutex);

    if (!write && sdh_st == SDH_STAT_SUCCESS) {
        L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)buf, total_bytes);
    }

    if (sdh_st == SDH_STAT_SUCCESS) {
        return sdio_check_r5(cmd.response[0], NULL);
    }
    if (sdh_st == SDH_STAT_DMA_ADDR_NOT_ALIGN) {
        return Status_SDIO_BufferAlignment;
    }
    return Status_SDIO_DataTimeout;
}

SDIO_Status_Type sdio_read_bytes(SDIO_Host_Type *host, uint8_t func_num,
                                 uint32_t addr, uint8_t *buf,
                                 uint16_t count, bool fixed_addr)
{
    return sdio_cmd53_xfer(host, func_num, false, false, fixed_addr,
                           addr, buf, count, 0);
}

SDIO_Status_Type sdio_write_bytes(SDIO_Host_Type *host, uint8_t func_num,
                                  uint32_t addr, const uint8_t *buf,
                                  uint16_t count, bool fixed_addr)
{
    return sdio_cmd53_xfer(host, func_num, true, false, fixed_addr,
                           addr, (uint8_t *)buf, count, 0);
}

SDIO_Status_Type sdio_read_blocks(SDIO_Host_Type *host, uint8_t func_num,
                                  uint32_t addr, uint8_t *buf,
                                  uint16_t count, bool fixed_addr)
{
    if (func_num > SDIO_MAX_FUNCS) return Status_SDIO_InvalidArg;
    uint16_t blk = host->funcs[func_num].cur_blk_size;
    if (blk == 0) return Status_SDIO_InvalidArg;
    return sdio_cmd53_xfer(host, func_num, false, true, fixed_addr,
                           addr, buf, count, blk);
}

SDIO_Status_Type sdio_write_blocks(SDIO_Host_Type *host, uint8_t func_num,
                                   uint32_t addr, const uint8_t *buf,
                                   uint16_t count, bool fixed_addr)
{
    if (func_num > SDIO_MAX_FUNCS) return Status_SDIO_InvalidArg;
    uint16_t blk = host->funcs[func_num].cur_blk_size;
    if (blk == 0) return Status_SDIO_InvalidArg;
    return sdio_cmd53_xfer(host, func_num, true, true, fixed_addr,
                           addr, (uint8_t *)buf, count, blk);
}

/* ================================================================== */
/* IRQ                                                                  */
/* ================================================================== */

SDIO_Status_Type sdio_claim_irq(SDIO_Host_Type *host, uint8_t func_num,
                                SDIO_IRQ_Handler handler, void *arg)
{
    if (host == NULL || func_num == 0 || func_num > SDIO_MAX_FUNCS) {
        return Status_SDIO_InvalidArg;
    }
    host->irq_handlers[func_num] = handler;
    host->irq_args[func_num]     = arg;
    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_release_irq(SDIO_Host_Type *host, uint8_t func_num)
{
    if (host == NULL || func_num == 0 || func_num > SDIO_MAX_FUNCS) {
        return Status_SDIO_InvalidArg;
    }
    host->irq_handlers[func_num] = NULL;
    host->irq_args[func_num]     = NULL;
    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_enable_irq(SDIO_Host_Type *host)
{
    SDIO_Status_Type st;
    uint8_t ie = 1;     /* Master IENM bit */

    if (host == NULL) return Status_SDIO_InvalidArg;
    for (uint8_t f = 1; f <= host->num_funcs && f <= SDIO_MAX_FUNCS; f++) {
        if (host->irq_handlers[f]) ie |= (1U << f);
    }
    st = sdio_writeb(host, SDIO_FUNC_0, SDIO_CCCR_INT_ENABLE, ie);
    if (st != Status_SDIO_Success) return st;
    SDH_EnableIntStatus(SDIO_CARD_INT_MASK, host->sd_id);
    SDH_EnableIntSource(SDIO_CARD_INT_MASK);
    return Status_SDIO_Success;
}

SDIO_Status_Type sdio_disable_irq(SDIO_Host_Type *host)
{
    if (host == NULL) return Status_SDIO_InvalidArg;
    SDH_DisableIntSource(SDIO_CARD_INT_MASK);
    SDH_DisableIntStatus(SDIO_CARD_INT_MASK, host->sd_id);
    return sdio_writeb(host, SDIO_FUNC_0, SDIO_CCCR_INT_ENABLE, 0);
}

/*
 * Common dispatch — read CCCR INT_PENDING via CMD52, invoke registered
 * per-function handlers. Caller is responsible for the "what triggered me"
 * gate (sem-take for IRQ-driven, SDH_GetIntStatus check for polled).
 *
 * The device-side cause is cleared by the per-function handler (typically
 * by writing the function's own INT register).
 */
static SDIO_Status_Type sdio_dispatch_funcs(SDIO_Host_Type *host)
{
    uint8_t pending = 0;
    SDIO_Status_Type st =
        sdio_readb(host, SDIO_FUNC_0, SDIO_CCCR_INT_PENDING, &pending);
    if (st != Status_SDIO_Success) return st;

    for (uint8_t f = 1; f <= SDIO_MAX_FUNCS; f++) {
        if ((pending & (1U << f)) && host->irq_handlers[f]) {
            host->irq_handlers[f](f, host->irq_args[f]);
        }
    }
    return Status_SDIO_Success;
}

/*
 * SDH ECLIC ISR — owns ONLY the SDH_INT_CARD bit. Other SDH events
 * (CMD/data complete, errors, insertion/removal) are left in polling
 * mode by hal_sdh.c since SDIO_SDCARD_INT_MODE is 0.
 */
static void sdio_sdh_isr(void)
{
    BaseType_t woken = pdFALSE;
    SDIO_Host_Type *host = s_active_host;
    if (host == NULL) return;

    uint32_t intFlag = SDH_GetIntStatus(host->sd_id);
    if (intFlag & SDIO_CARD_INT_MASK) {
        /* Clear at the SDH; the device-side cause is cleared later by the
         * dispatched handler. Do NOT disable INT_SOURCE here — that would
         * lose the next IRQ until the consumer re-enables it. */
        SDH_ClearIntStatus(SDIO_CARD_INT_MASK, host->sd_id);
        xSemaphoreGiveFromISR((SemaphoreHandle_t)host->card_int_sem, &woken);
    }
    portYIELD_FROM_ISR(woken);
}

/* Legacy non-blocking polled dispatch. Drains card_int_sem so the IRQ-driven
 * and polled callers don't double-fire on the same DAT1 pulse. */
SDIO_Status_Type sdio_poll_irq(SDIO_Host_Type *host)
{
    if (host == NULL) return Status_SDIO_InvalidArg;

    BaseType_t took = xSemaphoreTake((SemaphoreHandle_t)host->card_int_sem, 0);
    if (took != pdTRUE) {
        /* No sem pending — fall back to direct status read for callers that
         * use polled-only mode (sdio_enable_irq never called → no ISR). */
        uint32_t intFlag = SDH_GetIntStatus(host->sd_id);
        if ((intFlag & SDIO_CARD_INT_MASK) == 0) {
            return Status_SDIO_Success;
        }
        SDH_ClearIntStatus(SDIO_CARD_INT_MASK, host->sd_id);
    }
    return sdio_dispatch_funcs(host);
}

/* Blocking dispatch driven by the SDH card-int ISR semaphore. */
SDIO_Status_Type sdio_wait_irq(SDIO_Host_Type *host, uint32_t ticks)
{
    if (host == NULL) return Status_SDIO_InvalidArg;

    if (xSemaphoreTake((SemaphoreHandle_t)host->card_int_sem, ticks) != pdTRUE) {
        return Status_SDIO_CmdTimeout;
    }
    return sdio_dispatch_funcs(host);
}
