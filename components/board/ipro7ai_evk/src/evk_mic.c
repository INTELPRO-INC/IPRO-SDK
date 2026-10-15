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

/**
 * @file evk_mic.c
 * @brief PDM microphone capture through the AUADC. See evk_mic.h.
 *
 * Lifted from apps/platform/ipro7ai_evk_demo/src/audio.c (the bring-up tool
 * that found the AON pad ownership problem) and trimmed to the capture path.
 */

#include "evk_mic.h"
#include "evk_pcm.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <stream_buffer.h>

#include "hal_auadc.h"
#include "drv_auadc_v1.h"   /* AUADC_SetVolume: the integer half of the gain API */
#include "drv_hbn.h"
#include "hbn_reg.h"
#include "compiler/common.h"   /* ATTR_NOCACHE_NOINIT_RAM_SECTION */
#include <system_soc.h>        /* L1C_DCACHE_INVALID_RANGE */
#include "ipro_log.h"

#define LOG_TAG "EVK_MIC"

#define FRAME_BYTES        (EVK_MIC_FRAME_SAMPLES * (uint32_t)sizeof(int16_t))
#define QUEUE_FRAMES       8      /* 160 ms of slack for a late reader */

/* The mic needs a few frames to settle after its clock starts; the first
 * ones are garbage. Same allowance the SDK's PDM demo makes. */
#define STARTUP_DISCARD    5

/*
 * DMA double buffer. MUST live in non-cached OCRAM: the AUADC DMA cannot be
 * pointed at PSRAM, and with CONFIG_USE_PSRAM the heap can hand out PSRAM.
 */
static int16_t ATTR_NOCACHE_NOINIT_RAM_SECTION
    __attribute__((aligned(32))) s_dma_raw[EVK_MIC_FRAME_SAMPLES * 2];

static StreamBufferHandle_t s_pcm;
static volatile bool        s_on;
static volatile uint32_t    s_discard;
static bool                 s_hw_ready;
static int                  s_gain_db = EVK_MIC_DIGITAL_GAIN_DB;
static evk_mic_stats_t      s_stats;

/* ---------------------------------------------------------------------------
 * ISR: one completed DMA frame
 * -------------------------------------------------------------------------*/

static void pdm_frame_isr(int buf_idx)
{
    BaseType_t woken = pdFALSE;
    int16_t   *src   = &s_dma_raw[buf_idx * EVK_MIC_FRAME_SAMPLES];

    /* Counted before any other check: when nothing records, "is the AUADC
     * producing frames at all?" is the first question. */
    s_stats.isr_frames++;

    if (!s_on || !s_pcm) {
        return;
    }
    L1C_DCACHE_INVALID_RANGE(src, FRAME_BYTES);

    if (s_discard) {
        s_discard--;
        s_stats.discarded++;
        return;
    }
    if (xStreamBufferSendFromISR(s_pcm, src, FRAME_BYTES, &woken) == FRAME_BYTES) {
        s_stats.delivered++;
    } else {
        s_stats.dropped++;
    }
    portYIELD_FROM_ISR(woken);
}

/* ---------------------------------------------------------------------------
 * Pads
 * -------------------------------------------------------------------------*/

/*
 * Hand GPIO18/GPIO19 back to the digital pad mux.
 *
 * The AON block's per-pad "I own this pin" bits are split across TWO
 * registers: HBN_PAD_CTRL_0[27:20] for the low eight AON pads and
 * HBN_PAD_CTRL_2[11:10] for the top two. On this board PAD_CTRL_0's field
 * reads 0 (looks like "AON owns nothing") while PAD_CTRL_2's reads 0b11 with
 * its ie_smt_msb field at 0 - the mic pins are claimed by the always-on block
 * with their input receivers switched off, and read 0 no matter what.
 *
 * The same pins are the 32.768 kHz crystal pads. HBN_Power_Off_Xtal_32K()
 * clears the PU bits but never sets XTAL32K_HIZ_EN, so the pads can stay
 * attached to the oscillator; select RC32K and tri-state them explicitly.
 */
static void claim_pads(void)
{
    uint32_t p2_before = readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET);
    uint32_t p2 = p2_before;

    p2 &= HBN_REG_EN_AON_CTRL_GPIO_MSB_UMSK;   /* give the top two pads to GLB */
    p2 |= HBN_REG_AON_GPIO_IE_SMT_MSB_MSK;     /* input receivers on           */
    writel(p2, HBN_BASE + HBN_PAD_CTRL_2_OFFSET);

    uint32_t p0 = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    p0 &= HBN_REG_EN_AON_CTRL_GPIO_7_0_UMSK;
    p0 |= HBN_REG_AON_GPIO_IE_SMT_MSK;
    writel(p0, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);

    uint32_t x = readl(HBN_BASE + HBN_XTAL32K_OFFSET);
    if (!(x & HBN_XTAL32K_HIZ_EN_MSK)) {
        HBN_32K_Sel(HBN_32K_RC);
        x &= HBN_PU_XTAL32K_UMSK;
        x &= HBN_PU_XTAL32K_BUF_UMSK;
        x |= HBN_XTAL32K_HIZ_EN_MSK;
        writel(x, HBN_BASE + HBN_XTAL32K_OFFSET);
    }
    IPRO_LOGI(LOG_TAG, "AON pads released: HBN_PAD_CTRL_2 0x%08lX -> 0x%08lX",
              (unsigned long)p2_before,
              (unsigned long)readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET));
}

/*
 * AUADC digital gain in whole dB, integer only.
 *
 * hal_auadc_set_digital_gain() takes a float and does float compares and a
 * float multiply on the way to the register - fine at boot, not from a task
 * that runs beside the NPU task on a port that does not save FPU context.
 * The register is 0.5 dB steps, 9-bit two's complement, so whole dB is
 * simply db * 2 masked to 9 bits (what the HAL computes for integral input).
 */
static void set_digital_gain_db(int db)
{
    if (db < -95) db = -95;
    if (db > EVK_MIC_DIGITAL_GAIN_DB) db = EVK_MIC_DIGITAL_GAIN_DB;
    AUADC_SetVolume((uint32_t)((db * 2) & 0x1FF));
}

/* ---------------------------------------------------------------------------
 * Hardware, configured once
 * -------------------------------------------------------------------------*/

static int hw_once(void)
{
    hal_auadc_pdm_cfg_t cfg;

    if (s_hw_ready) {
        return EVK_MIC_OK;
    }
    if (!s_pcm) {
        s_pcm = xStreamBufferCreate(FRAME_BYTES * QUEUE_FRAMES, FRAME_BYTES);
        if (!s_pcm) {
            IPRO_LOGE(LOG_TAG, "stream buffer alloc failed");
            return EVK_MIC_ERR_NOMEM;
        }
    }

    claim_pads();                     /* before hal_auadc_pdm_init() muxes the pins */

    memset(&cfg, 0, sizeof(cfg));
    cfg.pdm_clk_pin      = EVK_MIC_PIN_CLK;
    cfg.pdm_in_pin       = EVK_MIC_PIN_DAT;
    cfg.pdm_ch           = EVK_MIC_PDM_CHANNEL;
    cfg.pcm_frame_size   = FRAME_BYTES;
    cfg.pcm_frame_buf[0] = &s_dma_raw[0];
    cfg.pcm_frame_buf[1] = &s_dma_raw[EVK_MIC_FRAME_SAMPLES];
    cfg.pcm_frame_event  = pdm_frame_isr;
    cfg.sample_rate      = EVK_MIC_RATE_HZ;

    if (hal_auadc_pdm_init(&cfg) != 0) {
        IPRO_LOGE(LOG_TAG, "hal_auadc_pdm_init failed");
        return EVK_MIC_ERR_HW;
    }
    set_digital_gain_db(s_gain_db);
    s_hw_ready = true;

    IPRO_LOGI(LOG_TAG, "PDM ready: CLK=GPIO%d DIN=GPIO%d %s slot, gain %+d dB, "
                       "%u Hz nominal (~%u Hz real), DMA %p",
              EVK_MIC_PIN_CLK, EVK_MIC_PIN_DAT,
              EVK_MIC_PDM_CHANNEL ? "right" : "left", s_gain_db,
              (unsigned)EVK_MIC_RATE_HZ,
              (unsigned)hal_auadc_real_rate(EVK_MIC_RATE_HZ), (void *)s_dma_raw);
    return EVK_MIC_OK;
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

int evk_mic_start(void)
{
    int rc;

    if (s_on) {
        return EVK_MIC_OK;
    }
    rc = hw_once();
    if (rc != EVK_MIC_OK) {
        return rc;
    }
    xStreamBufferReset(s_pcm);
    s_discard = STARTUP_DISCARD;
    s_on      = true;
    if (hal_auadc_start() != 0) {
        s_on = false;
        IPRO_LOGE(LOG_TAG, "hal_auadc_start failed");
        return EVK_MIC_ERR_HW;
    }
    return EVK_MIC_OK;
}

void evk_mic_stop(void)
{
    if (!s_on) {
        return;
    }
    s_on = false;
    hal_auadc_stop();
}

bool evk_mic_running(void)
{
    return s_on;
}

uint32_t evk_mic_read(int16_t *dst, uint32_t timeout_ms)
{
    if (!s_pcm || !dst) {
        return 0;
    }
    size_t got = xStreamBufferReceive(s_pcm, dst, FRAME_BYTES, pdMS_TO_TICKS(timeout_ms));
    return (uint32_t)(got / sizeof(int16_t));
}

void evk_mic_flush(void)
{
    if (s_pcm) {
        xStreamBufferReset(s_pcm);
    }
}

int evk_mic_set_gain_db(int db)
{
    if (db < 0 || db > EVK_MIC_DIGITAL_GAIN_DB) {
        return EVK_MIC_ERR_PARAM;
    }
    s_gain_db = db;
    if (s_hw_ready) {
        set_digital_gain_db(db);
    }
    return EVK_MIC_OK;
}

int evk_mic_get_gain_db(void)
{
    return s_gain_db;
}

uint32_t evk_mic_real_rate(void)
{
    return hal_auadc_real_rate(EVK_MIC_RATE_HZ);
}

void evk_mic_get_stats(evk_mic_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}

void evk_mic_level(const int16_t *pcm, uint32_t n, uint16_t *peak, uint16_t *rms)
{
    evk_pcm_level(pcm, n, peak, rms);
}

int evk_mic_dbfs_x10(uint32_t v)
{
    return evk_pcm_dbfs_x10(v);
}
