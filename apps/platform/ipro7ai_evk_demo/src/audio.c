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
 * @file audio.c
 * @brief Speaker + PDM microphone bring-up. See audio.h and audio_port.h.
 */

#include "audio.h"
#include "audio_port.h"
#include "st7789p3.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include <semphr.h>
#include <stream_buffer.h>

#include "audio_output.h"
#include "hal_auadc.h"
#include "hal_gpio.h"
#include "hal_mtimer.h"
#include "hal_dac.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_hbn.h"
#include "hbn_reg.h"
#include "glb_reg.h"
#include "compiler/common.h"   /* ATTR_NOCACHE_NOINIT_RAM_SECTION */
#include <system_soc.h>          /* L1C_DCACHE_INVALID_RANGE */
#include "ipro_log.h"

#define LOG_TAG "AUDIO"

#define AUDIO_TASK_STACK   1536
#define AUDIO_TASK_PRIO    (configMAX_PRIORITIES - 4)

#define TONE_LUT_SIZE      256
#define TONE_CHUNK         320
#define REC_MAX_SECONDS    10
#define CAPTURE_BUF_FRAMES 8
#define FRAME_BYTES        (AUDIO_FRAME_SAMP * (uint32_t)sizeof(int16_t))

/* The mic needs a few frames to settle after its clock starts; the first ones
 * are garbage. Same allowance the SDK's PDM demo makes. */
#define STARTUP_DISCARD    5

/*
 * Acoustic measurement.
 *
 * MEAS_RATE is the nominal AUADC setting; the real rate is a divided-down
 * 2.032 MHz and the driver's enum does not describe it honestly (slot 3 is
 * named AUADC_CLK_RESVERSE1 and only a comment claims 32K). Nothing here
 * assumes it: a pilot tone of known frequency is played first, and the host
 * derives the true capture rate from where that pilot lands.
 */
#define MEAS_RATE          32000    /* nominal setting; the real rate is ~47.6 kHz */

/*
 * Buffer sizing uses the worst case the AUADC can actually deliver, not the
 * nominal setting. The first run sized it at 32000 and overran by 322 frames:
 * slot 3 really runs near 47.6 kHz, so 10 s of it does not fit a 10 s buffer
 * built from the nominal figure.
 */
#define MEAS_RATE_MAX      48000u
#define MEAS_PILOT_HZ      1000u    /* known reference for rate calibration */
#define MEAS_PILOT_SEC     1u

/* Consecutive 100 ms read timeouts before a capture loop gives up and reports.
 * Without this a silent microphone makes `mic rec` spin forever, which looks
 * exactly like "the command did nothing". */
#define CAPTURE_STALL_LIMIT 20

/*
 * -60 dB, not -20. A phone's volume slider at its lowest setting still sends a
 * nonzero VCP level, and 20 dB of travel left that audibly loud. 60 dB is what
 * a volume control actually needs.
 */
#define GAIN_DB_MIN        (-60)
#define GAIN_DB_MAX        48

/* ---------------------------------------------------------------------------
 * State
 * -------------------------------------------------------------------------*/

static volatile audio_mode_t s_mode;
static volatile bool         s_stop_req;
static TaskHandle_t          s_task;
static audio_output_t       *s_out;

static int      s_gain_db     = SPK_MAKEUP_GAIN_DB;
static int32_t  s_gain_q15    = 32768;
static bool     s_muted;
static int      s_mic_gain_db = MIC_DIGITAL_GAIN_DB;
static int      s_spk_on;

static int16_t *s_rec;               /* PSRAM capture buffer */
static uint32_t s_rec_cap;           /* capacity in samples  */
static uint32_t s_rec_len;           /* valid samples        */
static uint32_t s_rec_rate = AUDIO_RATE;  /* what the buffer holds */

static int16_t  s_lut[TONE_LUT_SIZE];
static int      s_lut_ready;

static int16_t  s_frame[AUDIO_FRAME_SAMP];
static int16_t  s_work[AUDIO_FRAME_SAMP];

/* Parameters handed to the worker task. */
static uint32_t s_arg_freq, s_arg_sec, s_arg_amp;

/* Level accumulation. */
static uint32_t s_peak;
static uint64_t s_sumsq;
static uint32_t s_nsamp;
static audio_level_t s_level;

/* ---------------------------------------------------------------------------
 * Level metering
 * -------------------------------------------------------------------------*/

static void level_reset(void)
{
    s_peak = 0;
    s_sumsq = 0;
    s_nsamp = 0;
    memset(&s_level, 0, sizeof(s_level));
    s_level.peak_dbfs = -120.0f;
    s_level.rms_dbfs  = -120.0f;
}

static void level_add(const int16_t *pcm, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        int32_t v = pcm[i];
        uint32_t a = (uint32_t)((v < 0) ? -v : v);
        if (a > s_peak) {
            s_peak = a;
        }
        if (a >= 32767u) {
            s_level.clipped++;
        }
        s_sumsq += (uint64_t)((int64_t)v * v);
    }
    s_nsamp += n;
    s_level.frames++;
}

static float to_dbfs(float v)
{
    if (v < 1.0f) {
        return -120.0f;
    }
    return 20.0f * log10f(v / 32768.0f);
}

static void level_finish(void)
{
    s_level.peak_dbfs = to_dbfs((float)s_peak);
    if (s_nsamp) {
        s_level.rms_dbfs = to_dbfs(sqrtf((float)((double)s_sumsq / s_nsamp)));
    }
}

void audio_get_level(audio_level_t *out)
{
    if (out) {
        level_finish();
        *out = s_level;
    }
}

/* ---------------------------------------------------------------------------
 * Gain
 * -------------------------------------------------------------------------*/

static void gain_recalc(void)
{
    /*
     * Table, not powf().
     *
     * This runs on the shell task, and while LE Audio is playing the LC3
     * decoder is doing heavy float work on another task. The FreeRTOS port
     * here does not save f0-f31 across a context switch, so two tasks using
     * the FPU corrupt each other silently - the exact hazard this app's README
     * warns about, which I then walked into by leaving a powf() on the volume
     * path. Integer lookup keeps this path away from the FPU entirely.
     */
    static const int32_t q15[GAIN_DB_MAX - GAIN_DB_MIN + 1] = {
    33, 37, 41, 46, 52, 58,
    65, 73, 82, 92, 104, 116,
    130, 146, 164, 184, 207, 232,
    260, 292, 328, 368, 413, 463,
    519, 583, 654, 734, 823, 924,
    1036, 1163, 1305, 1464, 1642, 1843,
    2068, 2320, 2603, 2920, 3277, 3677,
    4125, 4629, 5193, 5827, 6538, 7336,
    8231, 9235, 10362, 11627, 13045, 14637,
    16423, 18427, 20675, 23198, 26029, 29205,
    32768, 36766, 41252, 46286, 51934, 58271,
    65381, 73358, 82309, 92353, 103622, 116265,
    130452, 146369, 164229, 184268, 206752, 231980,
    260285, 292045, 327680, 367663, 412525, 462860,
    519338, 582707, 653808, 733584, 823095, 923528,
    1036215, 1162653, 1304518, 1463693, 1642290, 1842680,
    2067521, 2319797, 2602855, 2920451, 3276800, 3676630,
    4125247, 4628603, 5193378, 5827066, 6538076, 7335841,
    8230949
    };

    s_gain_q15 = q15[s_gain_db - GAIN_DB_MIN];
}

static void gain_apply(const int16_t *src, int16_t *dst, uint32_t n)
{
    if (s_muted) {
        memset(dst, 0, n * sizeof(int16_t));
        return;
    }

    for (uint32_t i = 0; i < n; i++) {
        /*
         * 64-bit intermediate. The scale is Q15 so it can still resolve -60 dB
         * - Q8 quantised anything below about -48 dB straight to silence - and
         * at +48 dB the factor is 8.2 million, which overflows a 32-bit product.
         */
        int32_t v = (int32_t)(((int64_t)src[i] * s_gain_q15) >> 15);
        if (v > 32767) {
            v = 32767;
            s_level.clipped++;
        } else if (v < -32768) {
            v = -32768;
            s_level.clipped++;
        }
        dst[i] = (int16_t)v;
    }
}

/*
 * Same gain, exposed so other sinks - the LE Audio decoder - go through one
 * dB mapping and one clip counter instead of growing a second one that drifts.
 */
void audio_gain_apply(const int16_t *src, int16_t *dst, uint32_t n)
{
    gain_apply(src, dst, n);
}

int audio_set_gain(int db)
{
    if (db < GAIN_DB_MIN || db > GAIN_DB_MAX) {
        return AUDIO_ERR_PARAM;
    }
    s_gain_db = db;
    gain_recalc();
    return AUDIO_OK;
}

int audio_get_gain(void) { return s_gain_db; }

void audio_set_mute(int on) { s_muted = (on != 0); }
int  audio_get_mute(void)   { return s_muted ? 1 : 0; }

int audio_set_mic_gain(int db)
{
    if (db < 0 || db > 18) {
        return AUDIO_ERR_PARAM;
    }
    s_mic_gain_db = db;
    return AUDIO_OK;
}

int audio_get_mic_gain(void) { return s_mic_gain_db; }


/* ---------------------------------------------------------------------------
 * Amplifier enable
 * -------------------------------------------------------------------------*/

void audio_spk_enable(int on)
{
    uint8_t pin[1] = { SPK_PIN_EN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(SPK_PIN_EN, GPIO_OUTPUT_PP_MODE);
    gpio_write(SPK_PIN_EN, (uint32_t)(on ? SPK_EN_ACTIVE : !SPK_EN_ACTIVE));
    s_spk_on = on ? 1 : 0;
}

int audio_spk_is_enabled(void) { return s_spk_on; }

/* ---------------------------------------------------------------------------
 * DAC sink
 *
 * audio_output_dac.c drives the enable GPIO for the IPRO7 EVB's LM4871, whose
 * SHUTDOWN input is active LOW: it writes 0 on start and 1 on stop. This
 * board's TPA2037D1 EN is active HIGH, so both writes are backwards and the
 * config struct has no polarity field. Re-assert the pin either side of the
 * component call - see audio_port.h.
 * -------------------------------------------------------------------------*/

/*
 * Open and close the DAC plainly, and live with the pop.
 *
 * Two attempts at removing it both made playback worse, and playback matters
 * more than a click at the edges:
 *
 *   1. Muting the amplifier around the DAC's steps needed a settle delay for
 *      the DC-blocking cap. That delay sits between audio_output_start() and
 *      the producer's first write, and it changes where the producer stands
 *      relative to the DMA - close enough to being lapped that stale segments
 *      get replayed.
 *   2. Leaving the DAC running and flushing the ring to silence on release
 *      hands the next user a FULL ring. A full ring means the producer blocks
 *      on its first write with no lead over the DMA at all, which is the same
 *      failure by a different route.
 *
 * The ring's initial state is load-bearing: audio_output_create() pre-fills it
 * with silence and leaves every buf_ready[] slot clear, so the producer can get
 * four segments ahead before it ever blocks. Anything that consumes that lead
 * before the first write shows up as rough audio.
 *
 * A real fix belongs in audio_output_dac.c - ramping the DAC to mid-scale
 * before the DMA starts, so there is no step to mute around - not here.
 */
audio_output_t *audio_dac_open_ex(uint32_t rate, uint8_t ring_count,
                                  uint16_t ring_ms)
{
    audio_output_cfg_t cfg = {
        .type           = AUDIO_OUTPUT_DAC,
        .mode           = AUDIO_OUTPUT_MODE_STREAMING,
        .sample_rate    = rate,
        .channels       = 1,
        .ring_buf_count = ring_count,
        .ring_buf_ms    = ring_ms,
        .dac_spk_gpio   = SPK_PIN_EN,
    };
    audio_output_t *out = audio_output_create(&cfg);

    if (!out) {
        IPRO_LOGE(LOG_TAG, "audio_output_create failed");
        return NULL;
    }
    if (audio_output_start(out) != 0) {
        IPRO_LOGE(LOG_TAG, "audio_output_start failed");
        audio_output_destroy(out);
        return NULL;
    }

    audio_spk_enable(1);      /* undo the component's active-low unmute */
    return out;
}

void audio_dac_close(audio_output_t *out)
{
    if (!out) {
        return;
    }
    audio_output_stop(out);
    audio_spk_enable(0);      /* stop() drove EN high, which is ON here */
    audio_output_destroy(out);
}

/*
 * 4x20 ms suits this app's own paths, where frames are generated locally and
 * arrive on time. A network sink needs a deeper ring - see bt_audio_out.c.
 */
audio_output_t *audio_dac_open(uint32_t rate)
{
    return audio_dac_open_ex(rate, 4, 20);
}


/* ---------------------------------------------------------------------------
 * Capture
 * -------------------------------------------------------------------------*/

/*
 * DMA double buffers. These MUST live in non-cached OCRAM: the AUADC DMA
 * cannot be pointed at PSRAM, and this app has PSRAM enabled, so an ordinary
 * heap allocation is not safe here. The SDK's own PDM demo carries the same
 * declaration with the same warning.
 */
static int16_t ATTR_NOCACHE_NOINIT_RAM_SECTION
    __attribute__((aligned(32))) s_dma_raw[AUDIO_FRAME_SAMP * 2];

static StreamBufferHandle_t s_pcm;
static volatile bool     s_cap_on;
static volatile uint32_t s_discard;
static volatile uint32_t s_isr_frames;    /* counted even while idle */
static volatile uint32_t s_isr_dropped;
static bool              s_pdm_ready;
static uint32_t          s_pdm_rate = AUDIO_RATE;   /* nominal rate now in HW */

/*
 * Measurement capture goes straight from the ISR into the PSRAM buffer.
 *
 * The obvious structure - one loop that reads a mic frame and writes a sweep
 * frame - deadlocks the two rates against each other: dac_write() blocks until
 * the DMA has room and capture_frame() blocks up to 100 ms waiting for a frame,
 * and the DAC (31985 Hz) and the AUADC (a divided 2.032 MHz) are not the same
 * clock, so one side always starves. Writing from the ISR decouples them
 * completely: the task is paced by the DAC alone, the mic fills PSRAM on its
 * own clock, and the pilot tone reconciles the two time bases afterwards.
 */
static int               rec_alloc(void);
static volatile bool     s_meas_direct;
static uint32_t          s_meas_f1 = 50u;
static uint32_t          s_meas_f2 = 15000u;
static uint32_t          s_meas_dropped;
static int               s_pdm_channel = MIC_PDM_CHANNEL;

/*
 * ISR: one completed DMA frame. s_isr_frames is incremented before any other
 * check, so it answers the only question that matters when nothing is
 * recorded - is the AUADC producing frames at all? Zero means clock, pins or
 * rails; non-zero with an empty stream means the plumbing above it.
 */
static void pdm_frame_isr(int buf_idx)
{
    BaseType_t woken = pdFALSE;
    int16_t   *src   = &s_dma_raw[buf_idx * AUDIO_FRAME_SAMP];

    s_isr_frames++;

    if (!s_cap_on || !s_pcm) {
        portYIELD_FROM_ISR(woken);
        return;
    }
    L1C_DCACHE_INVALID_RANGE(src, FRAME_BYTES);

    if (s_discard) {
        s_discard--;
        portYIELD_FROM_ISR(woken);
        return;
    }
    if (s_meas_direct) {
        /* Single producer here, single consumer once the run ends. */
        if (s_rec && s_rec_len + AUDIO_FRAME_SAMP <= s_rec_cap) {
            memcpy(&s_rec[s_rec_len], src, FRAME_BYTES);
            s_rec_len += AUDIO_FRAME_SAMP;
        } else {
            s_meas_dropped++;
        }
        portYIELD_FROM_ISR(woken);
        return;
    }
    if (xStreamBufferSendFromISR(s_pcm, src, FRAME_BYTES, &woken) != FRAME_BYTES) {
        s_isr_dropped++;
    }
    portYIELD_FROM_ISR(woken);
}

/*
 * Configure the PDM hardware exactly once for the life of the app.
 * hal_auadc_pdm_init() must not be called a second time - the SDK's PDM demo
 * states plainly that "reinit breaks DMA on subsequent calls", so repeated
 * record/loop/scope cycles are gated with start/stop instead. That also means
 * the DMA channel is never released, which is deliberate.
 */
/*
 * Hand GPIO18/GPIO19 back to the digital pad mux.
 *
 * These two pins are the 32.768 kHz crystal pads (XTAL32K_IN / XTAL32K_OUT) as
 * well as AON GPIOs, and out of reset they belong to the oscillator, not to
 * the GLB function mux. Muxing them to GPIO_FUN_PDM therefore does nothing
 * until the analogue block lets go - the AUADC gets clocked, its DMA is armed,
 * and not one frame ever arrives.
 *
 * The board init does call HBN_Power_Off_Xtal_32K(), but that function only
 * clears PU_XTAL32K and PU_XTAL32K_BUF. It never sets XTAL32K_HIZ_EN, which is
 * the bit that actually tri-states the pads - note that its counterpart
 * HBN_Power_On_Xtal_32K() explicitly *clears* HIZ_EN on the way in and nothing
 * puts it back. So after a normal boot the oscillator is unpowered but the
 * pads are still attached to it.
 *
 * Setting HIZ_EN here is the missing half. RC32K is selected first so nothing
 * in the system is still leaning on the crystal when it goes away.
 */
static void pdm_claim_pads(void)
{
    uint32_t p2_before, p2_after, x_before;

    /*
     * Hand GPIO18/19 back to the GLB pad mux.
     *
     * The AON block's per-pad "I own this pin" bits are split across TWO
     * registers, which is what made this hard to see:
     *
     *   HBN_PAD_CTRL_0 [27:20]  reg_en_aon_ctrl_gpio_7_0   (low 8 AON pads)
     *   HBN_PAD_CTRL_2 [11:10]  reg_en_aon_ctrl_gpio_msb   (top 2 AON pads)
     *
     * On this board PAD_CTRL_0's field reads 0x00 - which looks like "AON owns
     * nothing" if you stop there - while PAD_CTRL_2's reads 0b11. Those top
     * two pads are claimed by the always-on block, and PAD_CTRL_2's matching
     * ie_smt_msb field [9:8] is 0, so their input receivers are switched off.
     * A pin in that state reads 0 no matter what, and will not even follow its
     * own internal pull-up - exactly what `mic probe` measured on GPIO19.
     *
     * Clearing en_aon_ctrl_msb returns the pads to GLB. ie_smt_msb is set as
     * well so the input path is alive even if the AON block still latches it.
     */
    p2_before = readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET);
    p2_after  = p2_before;
    p2_after &= HBN_REG_EN_AON_CTRL_GPIO_MSB_UMSK;   /* give the pads to GLB */
    p2_after |= HBN_REG_AON_GPIO_IE_SMT_MSB_MSK;     /* input receiver on     */
    writel(p2_after, HBN_BASE + HBN_PAD_CTRL_2_OFFSET);

    /* Same for the low 8 AON pads, in case a different board build claims
     * GPIO18/19 through that field instead. */
    {
        uint32_t p0 = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
        p0 &= HBN_REG_EN_AON_CTRL_GPIO_7_0_UMSK;
        p0 |= HBN_REG_AON_GPIO_IE_SMT_MSK;
        writel(p0, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    }

    /* The 32 kHz crystal shares these pins. Already released on this board,
     * but make it explicit so a build that boots with XTAL32K running still
     * works: select RC32K, then tri-state the crystal pads. */
    x_before = readl(HBN_BASE + HBN_XTAL32K_OFFSET);
    if (!(x_before & HBN_XTAL32K_HIZ_EN_MSK)) {
        uint32_t x = x_before;
        HBN_32K_Sel(HBN_32K_RC);
        x &= HBN_PU_XTAL32K_UMSK;
        x &= HBN_PU_XTAL32K_BUF_UMSK;
        x |= HBN_XTAL32K_HIZ_EN_MSK;
        writel(x, HBN_BASE + HBN_XTAL32K_OFFSET);
    }

    IPRO_LOGI(LOG_TAG, "pads claimed: HBN_PAD_CTRL_2 0x%08X -> 0x%08X "
                       "(en_aon_ctrl_msb %u -> %u, ie_smt_msb %u -> %u)",
              (unsigned)p2_before, (unsigned)readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET),
              (unsigned)((p2_before >> 10) & 0x3u),
              (unsigned)((readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET) >> 10) & 0x3u),
              (unsigned)((p2_before >> 8) & 0x3u),
              (unsigned)((readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET) >> 8) & 0x3u));
}

static int pdm_hw_once(uint32_t rate)
{
    hal_auadc_pdm_cfg_t cfg;

    /* Re-init when the rate changes; the HAL reuses its DMA channel. */
    if (s_pdm_ready && s_pdm_rate == rate) {
        hal_auadc_set_digital_gain((float)s_mic_gain_db);
        return AUDIO_OK;
    }
    if (!s_pcm) {
        s_pcm = xStreamBufferCreate(FRAME_BYTES * CAPTURE_BUF_FRAMES,
                                    FRAME_BYTES);
        if (!s_pcm) {
            IPRO_LOGE(LOG_TAG, "PCM stream buffer alloc failed");
            return AUDIO_ERR_NOMEM;
        }
    }

    /* Must happen before hal_auadc_pdm_init() muxes the pins. */
    pdm_claim_pads();

    memset(&cfg, 0, sizeof(cfg));
    cfg.pdm_clk_pin      = MIC_PIN_CLK;
    cfg.pdm_in_pin       = MIC_PIN_DAT;
    cfg.pdm_ch           = (uint8_t)s_pdm_channel;
    cfg.pcm_frame_size   = FRAME_BYTES;
    cfg.pcm_frame_buf[0] = &s_dma_raw[0];
    cfg.pcm_frame_buf[1] = &s_dma_raw[AUDIO_FRAME_SAMP];
    cfg.pcm_frame_event  = pdm_frame_isr;
    cfg.sample_rate      = rate;

    if (hal_auadc_pdm_init(&cfg) != 0) {
        IPRO_LOGE(LOG_TAG, "hal_auadc_pdm_init failed");
        return AUDIO_ERR_HW;
    }
    hal_auadc_set_digital_gain((float)s_mic_gain_db);
    s_pdm_ready = true;
    s_pdm_rate  = rate;

    IPRO_LOGI(LOG_TAG, "PDM ready: CLK=GPIO%d DIN=GPIO%d %s slot, gain %+d dB, "
                       "%u Hz nominal (~%u Hz real), DMA buf %p",
              MIC_PIN_CLK, MIC_PIN_DAT, s_pdm_channel ? "right" : "left",
              s_mic_gain_db, (unsigned)rate,
              (unsigned)hal_auadc_real_rate(rate), (void *)s_dma_raw);
    return AUDIO_OK;
}

static int capture_open_rate(uint32_t rate)
{
    int rc = pdm_hw_once(rate);

    if (rc != AUDIO_OK) {
        return rc;
    }
    xStreamBufferReset(s_pcm);
    s_discard     = STARTUP_DISCARD;
    s_isr_dropped = 0;
    s_cap_on      = true;

    if (hal_auadc_start() != 0) {
        s_cap_on = false;
        IPRO_LOGE(LOG_TAG, "hal_auadc_start failed");
        return AUDIO_ERR_HW;
    }
    return AUDIO_OK;
}

static int capture_open(void)
{
    return capture_open_rate(AUDIO_RATE);
}

static void capture_close(void)
{
    if (!s_cap_on) {
        return;
    }
    s_cap_on = false;
    hal_auadc_stop();
}

/* Pull one frame; returns samples read (0 on timeout). */
static uint32_t capture_frame(int16_t *dst)
{
    size_t got = xStreamBufferReceive(s_pcm, dst, FRAME_BYTES,
                                      pdMS_TO_TICKS(100));
    if (got == 0) {
        s_level.overruns++;
    }
    return (uint32_t)(got / sizeof(int16_t));
}

/*
 * Track consecutive read timeouts and give up with a diagnosis instead of
 * spinning forever. Returns true when the caller should abandon the loop.
 */
static bool capture_stalled(uint32_t *misses)
{
    if (++(*misses) < CAPTURE_STALL_LIMIT) {
        return false;
    }
    if (s_isr_frames == 0u) {
        IPRO_LOGE(LOG_TAG, "no PDM frames at all - the AUADC completion ISR "
                           "never fired, so nothing is clocking the mic. "
                           "Check the 1.8 V mic rail and that GPIO%d/GPIO%d "
                           "really carry CLK/DATA.",
                  MIC_PIN_CLK, MIC_PIN_DAT);
    } else {
        IPRO_LOGE(LOG_TAG, "AUADC produced %u frames but none reached the "
                           "reader (%u dropped) - a buffering problem, not "
                           "the microphone.",
                  (unsigned)s_isr_frames, (unsigned)s_isr_dropped);
    }
    return true;
}

uint32_t audio_isr_frames(void)  { return s_isr_frames; }
uint32_t audio_isr_dropped(void) { return s_isr_dropped; }

/* ---------------------------------------------------------------------------
 * Worker bodies
 * -------------------------------------------------------------------------*/

static void lut_build(void)
{
    if (s_lut_ready) {
        return;
    }
    for (int i = 0; i < TONE_LUT_SIZE; i++) {
        s_lut[i] = (int16_t)(32767.0f *
                             sinf(2.0f * 3.14159265f * i / TONE_LUT_SIZE));
    }
    s_lut_ready = 1;
}

static void do_tone(void)
{
    static int16_t chunk[TONE_CHUNK];
    uint32_t total = s_arg_sec * AUDIO_TONE_RATE;
    uint32_t done  = 0;
    uint32_t phase = 0;
    uint32_t step;
    int32_t  amp_q15;

    lut_build();
    step    = (uint32_t)(((uint64_t)s_arg_freq * TONE_LUT_SIZE * 65536u)
                         / AUDIO_TONE_RATE);
    amp_q15 = (int32_t)((s_arg_amp > 100u ? 100u : s_arg_amp) * 32768u / 100u);

    s_out = audio_dac_open(AUDIO_TONE_RATE);
    if (!s_out) {
        return;
    }

    while (done < total && !s_stop_req) {
        uint32_t n = (total - done < TONE_CHUNK) ? (total - done) : TONE_CHUNK;
        for (uint32_t i = 0; i < n; i++) {
            chunk[i] = (int16_t)(((int32_t)s_lut[(phase >> 16) & (TONE_LUT_SIZE - 1)]
                                  * amp_q15) >> 15);
            phase += step;
        }
        level_add(chunk, n);
        audio_output_write(s_out, chunk, n);
        done += n;
    }

    audio_dac_close(s_out);
    s_out = NULL;
}

/*
 * Interpolated LUT lookup. do_tone() indexes s_lut directly, which leaves the
 * phase quantised to 1/256 of a cycle and spurs about 42 dB down - audible as
 * nothing, but the same order as the response ripple we are trying to measure.
 * Using the low 16 bits as a fraction puts the spurs near -70 dB instead.
 */
static inline int32_t meas_sine(uint32_t phase)
{
    uint32_t idx  = (phase >> 16) & (TONE_LUT_SIZE - 1u);
    int32_t  frac = (int32_t)(phase & 0xFFFFu);
    int32_t  a    = s_lut[idx];
    int32_t  b    = s_lut[(idx + 1u) & (TONE_LUT_SIZE - 1u)];
    return a + (((b - a) * frac) >> 16);
}

/*
 * Duplex acoustic measurement.
 *
 * Plays MEAS_PILOT_SEC of MEAS_PILOT_HZ followed by a logarithmic sweep out of
 * the DAC at its native rate - opening the output at AUDIO_TONE_RATE keeps the
 * resampler out of the path, so what is measured is the DAC, amplifier and
 * speaker rather than our own interpolation filter. The mic records the whole
 * thing into PSRAM from the ISR.
 *
 * The pilot is not decoration. It fixes the recording's absolute time base,
 * which is needed because (a) the AUADC's real rate is unknown, and (b) there
 * is a fixed but unmeasured delay through the DAC ring and the air. Both fall
 * out of where the pilot lands and how long it lasts.
 */
static void do_measure(void)
{
    static int16_t chunk[TONE_CHUNK];

    const uint32_t fs      = AUDIO_TONE_RATE;
    const uint32_t pilot_n = MEAS_PILOT_SEC * fs;
    const uint32_t sweep_n = s_arg_sec * fs;
    const uint32_t f1      = s_meas_f1;
    const uint32_t f2      = s_meas_f2;

    if (f1 == 0u || f2 <= f1 || f2 > fs / 2u) {
        IPRO_LOGE(LOG_TAG, "measure: bad range %u..%u Hz (Nyquist %u)",
                  (unsigned)f1, (unsigned)f2, (unsigned)(fs / 2u));
        return;
    }
    if (rec_alloc() != AUDIO_OK) {
        return;
    }

    lut_build();
    const int32_t amp_q15 =
        (int32_t)((s_arg_amp > 100u ? 100u : s_arg_amp) * 32768u / 100u);

    /* Capture first so the mic is settled before the pilot starts. */
    s_rec_len      = 0;
    s_meas_dropped = 0;
    TickType_t t0  = xTaskGetTickCount();
    if (capture_open_rate(MEAS_RATE) != AUDIO_OK) {
        return;
    }
    s_out = audio_dac_open(fs);
    if (!s_out) {
        capture_close();
        return;
    }
    s_rec_rate    = hal_auadc_real_rate(MEAS_RATE);
    s_meas_direct = true;

    IPRO_LOGI(LOG_TAG, "measure: %u s pilot @ %u Hz, then %u s sweep %u->%u Hz",
              (unsigned)MEAS_PILOT_SEC, (unsigned)MEAS_PILOT_HZ,
              (unsigned)s_arg_sec, (unsigned)f1, (unsigned)f2);

    /* --- pilot ----------------------------------------------------------- */
    uint32_t phase = 0;
    const uint32_t pilot_step =
        (uint32_t)(((uint64_t)MEAS_PILOT_HZ * TONE_LUT_SIZE * 65536u) / fs);
    for (uint32_t done = 0; done < pilot_n && !s_stop_req; ) {
        uint32_t n = (pilot_n - done < TONE_CHUNK) ? (pilot_n - done) : TONE_CHUNK;
        for (uint32_t i = 0; i < n; i++) {
            chunk[i] = (int16_t)((meas_sine(phase) * amp_q15) >> 15);
            phase += pilot_step;
        }
        level_add(chunk, n);
        audio_output_write(s_out, chunk, n);
        done += n;
    }

    /* --- sweep ------------------------------------------------------------ */
    /*
     * The phase increment is recomputed once per chunk and interpolated within
     * it. Evaluating pow() per sample is not an option: double maths is
     * soft-float on rv32imafc, and at 32 kHz it cannot keep the DAC fed.
     * Per chunk the frequency moves under 1%, so the error against the true
     * exponential is around 1e-5.
     */
    const double ratio = (double)f2 / (double)f1;
    const double inv_n = 1.0 / (double)(sweep_n > 1u ? sweep_n - 1u : 1u);
    const double kstep = (double)TONE_LUT_SIZE * 65536.0 / (double)fs;
    for (uint32_t done = 0; done < sweep_n && !s_stop_req; ) {
        uint32_t n = (sweep_n - done < TONE_CHUNK) ? (sweep_n - done) : TONE_CHUNK;

        uint32_t d0 = (uint32_t)((double)f1 * pow(ratio, (double)done * inv_n) * kstep);
        uint32_t d1 = (uint32_t)((double)f1 * pow(ratio, (double)(done + n) * inv_n) * kstep);

        for (uint32_t i = 0; i < n; i++) {
            uint32_t step = d0 + (uint32_t)(((int64_t)(d1 - d0) * (int64_t)i) / (int64_t)n);
            chunk[i] = (int16_t)((meas_sine(phase) * amp_q15) >> 15);
            phase += step;
        }
        level_add(chunk, n);
        audio_output_write(s_out, chunk, n);
        done += n;
    }

    s_meas_direct = false;
    audio_dac_close(s_out);
    s_out = NULL;
    capture_close();

    /*
     * Wall-clock rate estimate. Independent of the driver's nominal figure and
     * available immediately, so a rate surprise shows up here rather than as a
     * silently stretched frequency axis on the host.
     */
    uint32_t ms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
    uint32_t got = s_rec_len + s_meas_dropped * AUDIO_FRAME_SAMP;
    if (ms) {
        s_rec_rate = (got * 1000u) / ms;
    }
    IPRO_LOGI(LOG_TAG, "measure: captured %u samples in %u ms -> %u Hz measured "
                       "(table said %u), %u dropped",
              (unsigned)s_rec_len, (unsigned)ms, (unsigned)s_rec_rate,
              (unsigned)hal_auadc_real_rate(MEAS_RATE),
              (unsigned)s_meas_dropped);
    if (s_meas_dropped) {
        IPRO_LOGE(LOG_TAG, "measure: buffer overran - result is NOT valid");
    }
}

static void do_record(void)
{
    uint32_t want = s_arg_sec * AUDIO_RATE;

    if (want > s_rec_cap) {
        want = s_rec_cap;
    }
    if (capture_open() != AUDIO_OK) {
        return;
    }

    s_rec_len = 0;
    uint32_t misses = 0;
    while (s_rec_len < want && !s_stop_req) {
        uint32_t n = capture_frame(s_frame);
        if (!n) {
            if (capture_stalled(&misses)) {
                break;
            }
            continue;
        }
        misses = 0;
        if (s_rec_len + n > want) {
            n = want - s_rec_len;
        }
        memcpy(&s_rec[s_rec_len], s_frame, n * sizeof(int16_t));
        s_rec_len += n;
        level_add(s_frame, n);
    }

    capture_close();
    IPRO_LOGI(LOG_TAG, "recorded %u samples (%u ms)",
              (unsigned)s_rec_len, (unsigned)(s_rec_len * 1000u / AUDIO_RATE));
}

static void do_play(void)
{
    uint32_t done = 0;

    if (!s_rec_len) {
        IPRO_LOGE(LOG_TAG, "nothing recorded - run `mic rec` first");
        return;
    }

    s_out = audio_dac_open(s_rec_rate);
    if (!s_out) {
        return;
    }

    while (done < s_rec_len && !s_stop_req) {
        uint32_t n = s_rec_len - done;
        if (n > AUDIO_FRAME_SAMP) {
            n = AUDIO_FRAME_SAMP;
        }
        gain_apply(&s_rec[done], s_work, n);
        level_add(s_work, n);
        audio_output_write(s_out, s_work, n);
        done += n;
    }

    audio_dac_close(s_out);
    s_out = NULL;
}

static void do_loop(void)
{
    uint32_t deadline = s_arg_sec ? (s_arg_sec * 1000u / 20u) : 0xFFFFFFFFu;
    uint32_t frames   = 0;

    if (capture_open() != AUDIO_OK) {
        return;
    }
    s_out = audio_dac_open(s_rec_rate);
    if (!s_out) {
        capture_close();
        return;
    }

    uint32_t misses = 0;
    while (frames < deadline && !s_stop_req) {
        uint32_t n = capture_frame(s_frame);
        if (!n) {
            if (capture_stalled(&misses)) {
                break;
            }
            continue;
        }
        misses = 0;
        level_add(s_frame, n);
        gain_apply(s_frame, s_work, n);
        audio_output_write(s_out, s_work, n);
        frames++;
    }

    audio_dac_close(s_out);
    s_out = NULL;
    capture_close();
}

static void do_mic_only(void)
{
    if (capture_open() != AUDIO_OK) {
        return;
    }
    uint32_t misses = 0;
    while (!s_stop_req) {
        uint32_t n = capture_frame(s_frame);
        if (n) {
            misses = 0;
            level_add(s_frame, n);
        } else if (capture_stalled(&misses)) {
            break;
        }
    }
    capture_close();
}

/*
 * Scrolling waveform: one panel column per 20 ms frame, so a 320-column
 * landscape screen holds 6.4 s of history. Each frame costs two fill_rects,
 * which is cheap enough to run alongside capture.
 */
static void do_scope(void)
{
    uint16_t w, h, mid;
    uint16_t x = 0;
    uint32_t deadline = s_arg_sec ? (s_arg_sec * 1000u / 20u) : 0xFFFFFFFFu;
    uint32_t frames = 0;
    uint32_t misses = 0;

    if (!st7789p3_is_ready()) {
        IPRO_LOGE(LOG_TAG, "panel not initialised - run `lcd init` first");
        return;
    }
    if (capture_open() != AUDIO_OK) {
        return;
    }

    st7789p3_set_dir(1, 0);
    st7789p3_fill_screen(ST_BLACK);
    w   = st7789p3_width();
    h   = st7789p3_height();
    mid = (uint16_t)(h / 2u);
    st7789p3_fill_rect(0, mid, (uint16_t)(w - 1u), mid, ST_GRAY);

    while (frames < deadline && !s_stop_req) {
        uint32_t n = capture_frame(s_frame);
        int32_t  pk = 0;
        uint16_t half;

        if (!n) {
            if (capture_stalled(&misses)) {
                break;
            }
            continue;
        }
        misses = 0;
        level_add(s_frame, n);

        for (uint32_t i = 0; i < n; i++) {
            int32_t a = s_frame[i] < 0 ? -s_frame[i] : s_frame[i];
            if (a > pk) {
                pk = a;
            }
        }
        /* Scale peak to half the screen height. */
        half = (uint16_t)(((uint32_t)pk * (mid - 1u)) / 32768u);

        st7789p3_bus_lock();
        st7789p3_fill_rect(x, 0, x, (uint16_t)(h - 1u), ST_BLACK);
        st7789p3_fill_rect(x, mid, x, mid, ST_GRAY);
        if (half) {
            st7789p3_fill_rect(x, (uint16_t)(mid - half), x,
                               (uint16_t)(mid + half),
                               (pk >= 32767) ? ST_RED : ST_GREEN);
        }
        st7789p3_bus_unlock();

        x = (uint16_t)((x + 1u) % w);
        frames++;
    }

    capture_close();
}

/* ---------------------------------------------------------------------------
 * Worker task
 * -------------------------------------------------------------------------*/

static void audio_task(void *arg)
{
    (void)arg;

    level_reset();
    switch (s_mode) {
        case AUDIO_MODE_TONE:  do_tone();     break;
        case AUDIO_MODE_REC:   do_record();   break;
        case AUDIO_MODE_PLAY:  do_play();     break;
        case AUDIO_MODE_LOOP:  do_loop();     break;
        case AUDIO_MODE_SCOPE: do_scope();    break;
        case AUDIO_MODE_MIC:   do_mic_only(); break;
        case AUDIO_MODE_MEASURE: do_measure(); break;
        default:                              break;
    }
    level_finish();

    s_mode  = AUDIO_MODE_IDLE;
    s_task  = NULL;
    vTaskDelete(NULL);
}

static int audio_launch(audio_mode_t mode)
{
    audio_stop();

    s_stop_req = false;
    s_mode = mode;
    if (xTaskCreate(audio_task, "audio", AUDIO_TASK_STACK, NULL,
                    AUDIO_TASK_PRIO, &s_task) != pdPASS) {
        s_mode = AUDIO_MODE_IDLE;
        return AUDIO_ERR_NOMEM;
    }
    return AUDIO_OK;
}

void audio_stop(void)
{
    if (!s_task) {
        return;
    }
    s_stop_req = true;
    for (int i = 0; i < 200 && s_task; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    audio_spk_enable(0);
}

audio_mode_t audio_get_mode(void)      { return s_mode; }
uint32_t     audio_get_rate(void)      { return AUDIO_RATE; }
uint32_t     audio_recorded_samples(void) { return s_rec_len; }

/* ---------------------------------------------------------------------------
 * Public entry points
 * -------------------------------------------------------------------------*/

int audio_tone(uint32_t freq_hz, uint32_t seconds, uint32_t amp_pct)
{
    if (freq_hz == 0u || freq_hz > AUDIO_TONE_RATE / 2u || seconds == 0u) {
        return AUDIO_ERR_PARAM;
    }
    gain_recalc();
    s_arg_freq = freq_hz;
    s_arg_sec  = seconds;
    s_arg_amp  = amp_pct ? amp_pct : 30u;
    return audio_launch(AUDIO_MODE_TONE);
}

int audio_dac_dc(int code)
{
    dac_device_t dev = {
        .id          = DAC0_INDEX,
        .caps        = 0,
        .sample_freq = DAC_SAMPLE_FREQ_32KHZ,
        .channels    = DAC_CHANNEL_0,
        .vref        = DAC_VREF_INTERNAL,
        .tx_dma      = -1,
    };
    static int16_t dc[64];

    if (code < 0 || code > 1023) {
        return AUDIO_ERR_PARAM;
    }
    audio_stop();

    /* Map the 10-bit DAC code onto the int16 range the HAL expects. */
    for (unsigned i = 0; i < sizeof(dc) / sizeof(dc[0]); i++) {
        dc[i] = (int16_t)((code - 512) << 6);
    }

    hal_dac_setConfig(&dev);
    hal_dac_init(DAC0_INDEX);
    hal_dac_write(DAC0_INDEX, DAC_CHANNEL_0, dc, sizeof(dc));

    IPRO_LOGI(LOG_TAG, "DAC code %d held on GPIO%d "
                       "(amplifier left OFF on purpose - measure before C19)",
              code, SPK_PIN_DAC);
    return AUDIO_OK;
}

static int rec_alloc(void)
{
    if (s_rec) {
        return AUDIO_OK;
    }
    /* Sized for the fastest rate any mode uses, not just AUDIO_RATE. */
    s_rec_cap = MEAS_RATE_MAX * REC_MAX_SECONDS;
    s_rec = pvPortMalloc(s_rec_cap * sizeof(int16_t));
    if (!s_rec) {
        s_rec_cap = 0;
        IPRO_LOGE(LOG_TAG, "record buffer alloc failed (%u samples)",
                  (unsigned)(MEAS_RATE_MAX * REC_MAX_SECONDS));
        return AUDIO_ERR_NOMEM;
    }
    return AUDIO_OK;
}

/*
 * Lend the PSRAM record buffer to another producer.
 *
 * Exists so decoded LE Audio can be captured and then replayed locally through
 * `mic play`. That replay uses the same DAC path a synthesised tone does, and
 * a tone is known to come out clean - so if the captured audio replays cleanly
 * it says the decoder output was fine all along and the fault is in real-time
 * delivery, and if it replays rough the fault is upstream of the DAC. There is
 * no other way to tell those two apart by ear.
 */
int16_t *audio_capture_buffer(uint32_t *cap_samples)
{
    s_rec_rate = AUDIO_TONE_RATE;   /* a borrowed buffer is at the DAC rate */

    if (rec_alloc() != AUDIO_OK) {
        return NULL;
    }
    if (cap_samples) {
        *cap_samples = s_rec_cap;
    }
    s_rec_len = 0;
    return s_rec;
}

void audio_capture_commit(uint32_t samples)
{
    s_rec_len = (samples > s_rec_cap) ? s_rec_cap : samples;
}

int audio_measure(uint32_t seconds, uint32_t f1, uint32_t f2, uint32_t amp_pct)
{
    /* Pilot + sweep must both fit the PSRAM buffer. */
    if (seconds == 0u || seconds + MEAS_PILOT_SEC > REC_MAX_SECONDS) {
        return AUDIO_ERR_PARAM;
    }
    if (f1 == 0u || f2 <= f1 || f2 > AUDIO_TONE_RATE / 2u) {
        return AUDIO_ERR_PARAM;
    }
    if (rec_alloc() != AUDIO_OK) {
        return AUDIO_ERR_NOMEM;
    }
    s_arg_sec = seconds;
    s_arg_amp = amp_pct ? amp_pct : 60u;
    s_meas_f1 = f1;
    s_meas_f2 = f2;
    return audio_launch(AUDIO_MODE_MEASURE);
}

/*
 * Base64 the recording to the console.
 *
 * Same framing as the headset's pcmcap: 57 raw bytes per line so each line
 * decodes on its own, and a hex line index so a line lost to interleaved
 * output becomes a known hole the receiver can zero-fill in place instead of
 * shifting everything after it. Pacing is derived from the console baud rate -
 * yielding a fixed tick per line pushes far faster than the wire can carry and
 * the UART silently drops the excess.
 */
#define B64_LINE_MS  ((77u * 10u * 1000u) / CONFIG_PLATFORM_UART_BAUDRATE + 1u)

int audio_dump_b64(void)
{
    static const char B64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if (!s_rec || s_rec_len == 0u) {
        printf("nothing recorded\n");
        return AUDIO_ERR_STATE;
    }

    const uint8_t *p     = (const uint8_t *)s_rec;
    uint32_t       bytes = s_rec_len * sizeof(int16_t);

    printf("\n---PCMCAP-BEGIN rate=%lu samples=%lu bits=16 ch=1 lines=%lu---\n",
           (unsigned long)s_rec_rate, (unsigned long)s_rec_len,
           (unsigned long)((bytes + 56u) / 57u));

    char     line[81];
    uint32_t col = 0, line_idx = 0;
    for (uint32_t i = 0; i < bytes; i += 3u) {
        uint32_t v = (uint32_t)p[i] << 16;
        uint32_t n = bytes - i;
        if (n > 1u) v |= (uint32_t)p[i + 1] << 8;
        if (n > 2u) v |= (uint32_t)p[i + 2];

        line[col++] = B64[(v >> 18) & 0x3f];
        line[col++] = B64[(v >> 12) & 0x3f];
        line[col++] = (n > 1u) ? B64[(v >> 6) & 0x3f] : '=';
        line[col++] = (n > 2u) ? B64[v & 0x3f]        : '=';

        if (col >= 76u) {
            line[col] = 0;
            printf("%04lx:%s\n", (unsigned long)line_idx++, line);
            col = 0;
            vTaskDelay(pdMS_TO_TICKS(B64_LINE_MS));
        }
    }
    if (col) { line[col] = 0; printf("%04lx:%s\n", (unsigned long)line_idx++, line); }

    printf("---PCMCAP-END---\n");
    return AUDIO_OK;
}

int audio_record(uint32_t seconds)
{
    if (seconds == 0u || seconds > REC_MAX_SECONDS) {
        return AUDIO_ERR_PARAM;
    }
    if (rec_alloc() != AUDIO_OK) {
        return AUDIO_ERR_NOMEM;
    }
    s_arg_sec = seconds;
    return audio_launch(AUDIO_MODE_REC);
}

int audio_play(void)
{
    if (!s_rec || !s_rec_len) {
        return AUDIO_ERR_STATE;
    }
    gain_recalc();
    return audio_launch(AUDIO_MODE_PLAY);
}

int audio_play_gain(int db)
{
    if (audio_set_gain(db) != AUDIO_OK) {
        return AUDIO_ERR_PARAM;
    }
    return audio_play();
}

/*
 * Derive the make-up gain from the take that was actually recorded, rather
 * than assuming how loud the source was. 6 dB is held back because the peak
 * is measured per sample: resampling to the DAC rate interpolates, and the
 * reconstructed waveform can overshoot the highest sample it was built from.
 */
int audio_play_auto(void)
{
    int head = audio_headroom_db();
    int db;

    if (!s_rec || !s_rec_len) {
        return AUDIO_ERR_STATE;
    }
    db = head - 6;
    if (db < GAIN_DB_MIN) {
        db = GAIN_DB_MIN;
    } else if (db > GAIN_DB_MAX) {
        db = GAIN_DB_MAX;
    }
    IPRO_LOGI(LOG_TAG, "auto make-up: recorded peak leaves %d dB of headroom, "
                       "playing at %+d dB (6 dB held back for resampler "
                       "overshoot)", head, db);
    return audio_play_gain(db);
}

int audio_loop(uint32_t seconds)
{
    gain_recalc();
    s_arg_sec = seconds;
    return audio_launch(AUDIO_MODE_LOOP);
}

int audio_scope(uint32_t seconds)
{
    s_arg_sec = seconds;
    return audio_launch(AUDIO_MODE_SCOPE);
}

int audio_mic_start(void)
{
    return audio_launch(AUDIO_MODE_MIC);
}

/* ---------------------------------------------------------------------------
 * Hardware probe
 *
 * Everything above depends on the AUADC actually routing PDM to GPIO18/19,
 * which nothing in this SDK documents: drv_gpio.h's per-pin function tables
 * have no entry for function 5 (PDM) on ANY pin - not even GPIO0, which the
 * SDK's own PDM demo drives successfully. So the pin table cannot confirm or
 * deny GPIO18/19, and the only honest way to separate "the microphone and its
 * wiring are fine, the AUADC just is not reaching these pins" from "the mic is
 * not responding at all" is to bypass the AUADC entirely.
 *
 * This clocks GPIO18 by hand and samples GPIO19, i.e. a software PDM master.
 * A live PDM mic answers a clock in the 1-3.25 MHz range with a 1-bit
 * sigma-delta stream that toggles constantly even in silence. A dead rail,
 * a wrong pin or a mic held in reset gives a line stuck at one level.
 *
 * The achieved clock is measured and reported, because a negative result is
 * only meaningful if the clock was fast enough to bring the mic out of its
 * low-power mode.
 * -------------------------------------------------------------------------*/

int audio_mic_probe(uint32_t *out_clk_hz, uint32_t *out_ones,
                    uint32_t *out_edges, uint32_t *out_samples)
{
    const uint32_t samples = 20000u;
    uint8_t  dat_pin[1] = { MIC_PIN_DAT };
    uint32_t ones = 0, edges = 0;
    int      prev = -1;
    int      rd_pu, rd_pd;
    uint64_t t0, t1;

    audio_stop();
    if (s_cap_on) {
        capture_close();
    }

    /*
     * Step 1 - is the DATA pin's input path even alive?
     *
     * A rock-steady 0 on a PDM data line has two completely different causes:
     * the microphone is not driving it, or this pin's input buffer is off and
     * gpio_read() returns 0 whatever happens. Toggling the internal pull-up
     * and pull-down settles that without driving the pin against the mic - if
     * the pin follows them, the receiver works and the line is really idle.
     */
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, dat_pin, 1);
    gpio_set_mode(MIC_PIN_DAT, GPIO_INPUT_PP_MODE);
    arch_delay_us(200);
    rd_pu = gpio_read(MIC_PIN_DAT);
    gpio_set_mode(MIC_PIN_DAT, GPIO_INPUT_PD_MODE);
    arch_delay_us(200);
    rd_pd = gpio_read(MIC_PIN_DAT);
    IPRO_LOGI(LOG_TAG, "GPIO%d before claiming pads: pull-up %d, pull-down %d",
              MIC_PIN_DAT, rd_pu, rd_pd);

    /* Take the pins back from the always-on block, then repeat the test. The
     * difference between the two readings is the proof of who owned them. */
    pdm_claim_pads();

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, dat_pin, 1);
    gpio_set_mode(MIC_PIN_DAT, GPIO_INPUT_PP_MODE);
    arch_delay_us(200);
    rd_pu = gpio_read(MIC_PIN_DAT);
    gpio_set_mode(MIC_PIN_DAT, GPIO_INPUT_PD_MODE);
    arch_delay_us(200);
    rd_pd = gpio_read(MIC_PIN_DAT);
    IPRO_LOGI(LOG_TAG, "GPIO%d after  claiming pads: pull-up %d, pull-down %d",
              MIC_PIN_DAT, rd_pu, rd_pd);

    if (rd_pu == 1 && rd_pd == 0) {
        IPRO_LOGI(LOG_TAG, "  -> receiver works and the line follows its own "
                           "pulls, so the pad is ours now. A flat DATA line "
                           "from here on means the mic is not driving it.");
    } else if (rd_pu == 0 && rd_pd == 0) {
        IPRO_LOGE(LOG_TAG, "  -> STILL will not follow its own pull-up. The "
                           "pad is held by something this driver has not "
                           "found, or DATA is shorted low on the board.");
    } else if (rd_pu == 1 && rd_pd == 1) {
        IPRO_LOGE(LOG_TAG, "  -> DATA is held hard HIGH by something.");
    }

    /*
     * Step 2 - let the AUADC generate the clock, and watch DATA as a GPIO.
     *
     * Bit-banging the clock from software only reached ~900 kHz, which is
     * BELOW the 1.0 MHz an IM69D129 needs for normal mode - in sleep mode its
     * DATA output is high-Z, so a silent bit-banged result proves nothing.
     * The AUADC clocks PDM at 2.032 MHz, comfortably in spec, so drive the
     * clock with the real hardware and sample DATA asynchronously. Any live
     * PDM stream shows transitions regardless of sampling phase, which also
     * sidesteps the question of whether this mic drives on the rising or the
     * falling edge (SELECT is grounded, but vendors disagree on what that
     * means).
     */
    if (pdm_hw_once(s_pdm_rate) != AUDIO_OK) {
        return AUDIO_ERR_HW;
    }
    s_cap_on = false;                       /* clock only, discard frames */
    if (hal_auadc_start() != 0) {
        IPRO_LOGE(LOG_TAG, "hal_auadc_start failed during probe");
        return AUDIO_ERR_HW;
    }

    /* Take DATA back as a GPIO input; CLK stays with the AUADC. */
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, dat_pin, 1);
    gpio_set_mode(MIC_PIN_DAT, GPIO_INPUT_MODE);
    arch_delay_ms(5);                       /* let the mic leave standby */

    t0 = mtimer_get_time_us();
    for (uint32_t i = 0; i < samples; i++) {
        int bit = gpio_read(MIC_PIN_DAT);
        if (bit) {
            ones++;
        }
        if (prev >= 0 && bit != prev) {
            edges++;
        }
        prev = bit;
    }
    t1 = mtimer_get_time_us();

    hal_auadc_stop();

    /* Hand DATA back to the AUADC so normal capture still works afterwards. */
    {
        GLB_GPIO_Cfg_Type cfg = {
            .gpioPin  = MIC_PIN_DAT,
            .gpioFun  = GPIO_FUN_PDM,
            .gpioMode = GPIO_MODE_AF,
            .pullType = GPIO_PULL_NONE,
            .drive    = 1,
            .smtCtrl  = 1,
        };
        GLB_GPIO_Init(&cfg);
    }

    if (out_clk_hz) {
        uint64_t us = (t1 > t0) ? (t1 - t0) : 1u;
        *out_clk_hz = (uint32_t)(((uint64_t)samples * 1000000u) / us);
    }
    if (out_ones)    { *out_ones = ones; }
    if (out_edges)   { *out_edges = edges; }
    if (out_samples) { *out_samples = samples; }
    return AUDIO_OK;
}

/* Hex dump of the recording, to tell "no frames arrived" from "frames arrived
 * but every sample is zero" - two very different faults that both present as
 * "no audio". */
int audio_dump_samples(uint32_t offset, uint32_t n)
{
    if (!s_rec || !s_rec_len) {
        return AUDIO_ERR_STATE;
    }
    if (offset >= s_rec_len) {
        offset = 0;
    }
    if (n == 0u || n > 32u) {
        n = 16u;
    }
    if (offset + n > s_rec_len) {
        n = s_rec_len - offset;
    }

    for (uint32_t i = 0; i < n; i += 8u) {
        uint32_t m = (n - i < 8u) ? (n - i) : 8u;
        char line[128];
        int  p = 0;
        for (uint32_t j = 0; j < m; j++) {
            p += snprintf(&line[p], sizeof(line) - (size_t)p, " %6d",
                          s_rec[offset + i + j]);
        }
        IPRO_LOGI(LOG_TAG, "[%5u]%s", (unsigned)(offset + i), line);
    }
    return AUDIO_OK;
}

int audio_set_channel(int right)
{
    if (s_pdm_ready) {
        return AUDIO_ERR_STATE;
    }
    s_pdm_channel = right ? 1 : 0;
    return AUDIO_OK;
}

int audio_get_channel(void) { return s_pdm_channel; }

/* ---------------------------------------------------------------------------
 * Pad ownership dump
 *
 * The pull-up test showed GPIO19 refusing to follow its own pull-up, which
 * means something is holding it with a far lower impedance than the ~40k
 * internal pull. gpio_set_mode() and GLB_GPIO_Init() are both correct on
 * inspection - GLB_GPIO_Init() even clears the output enable first - so the
 * next question is whether the GLB config is the config that actually wins.
 *
 * GPIO18/19 are AON pads. HBN_PAD_CTRL_0.reg_en_aon_ctrl_gpio_7_0 hands a pad
 * to the always-on block, and from then on HBN_PAD_CTRL_1's own oe/o/pu/pd
 * fields drive it and the GLB settings are ignored. If boot2 or the ROM left
 * that enabled with output low, the pin is driven low and nothing done through
 * the GLB registers can move it.
 *
 * This dumps both sides rather than guessing which one is in charge.
 * -------------------------------------------------------------------------*/
void audio_write_pad_ctrl2(uint32_t val)
{
    uint32_t before = readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET);

    writel(val, HBN_BASE + HBN_PAD_CTRL_2_OFFSET);
    IPRO_LOGI(LOG_TAG, "HBN_PAD_CTRL_2 0x%08X -> wrote 0x%08X -> reads 0x%08X",
              (unsigned)before, (unsigned)val,
              (unsigned)readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET));
}

void audio_dump_pads(void)
{
    uint32_t glb_cfg18 = readl(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + (18 / 2) * 4);
    uint32_t glb_cfg19 = readl(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + (19 / 2) * 4);
    uint32_t glb_oe    = readl(GLB_BASE + GLB_GPIO_CFGCTL35_OFFSET);
    uint32_t hbn_p0    = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    uint32_t hbn_p1    = readl(HBN_BASE + HBN_PAD_CTRL_1_OFFSET);
    uint32_t hbn_p2    = readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET);
    uint32_t xtal32k   = readl(HBN_BASE + HBN_XTAL32K_OFFSET);

    /* GPIO18 is the even half of its word, GPIO19 the odd half. */
    IPRO_LOGI(LOG_TAG, "GLB cfg GPIO18 word=0x%08X func=%u ie=%u pu=%u pd=%u",
              (unsigned)glb_cfg18,
              (unsigned)((glb_cfg18 >> 8) & 0x1Fu),
              (unsigned)((glb_cfg18 >> 0) & 1u),
              (unsigned)((glb_cfg18 >> 4) & 1u),
              (unsigned)((glb_cfg18 >> 5) & 1u));
    IPRO_LOGI(LOG_TAG, "GLB cfg GPIO19 word=0x%08X func=%u ie=%u pu=%u pd=%u",
              (unsigned)glb_cfg19,
              (unsigned)((glb_cfg19 >> 24) & 0x1Fu),
              (unsigned)((glb_cfg19 >> 16) & 1u),
              (unsigned)((glb_cfg19 >> 20) & 1u),
              (unsigned)((glb_cfg19 >> 21) & 1u));
    IPRO_LOGI(LOG_TAG, "GLB oe  0x%08X -> GPIO18 oe=%u, GPIO19 oe=%u",
              (unsigned)glb_oe,
              (unsigned)((glb_oe >> 18) & 1u),
              (unsigned)((glb_oe >> 19) & 1u));

    IPRO_LOGI(LOG_TAG, "HBN_PAD_CTRL_0 0x%08X ie_smt=0x%02X en_aon_ctrl=0x%02X "
                       "sel_aon_o=%u iso=%u",
              (unsigned)hbn_p0,
              (unsigned)(hbn_p0 & 0xFFu),
              (unsigned)((hbn_p0 >> 20) & 0xFFu),
              (unsigned)((hbn_p0 >> 19) & 1u),
              (unsigned)((hbn_p0 >> 31) & 1u));
    IPRO_LOGI(LOG_TAG, "HBN_PAD_CTRL_1 0x%08X aon oe=0x%02X pd=0x%02X "
                       "pu=0x%02X o=0x%02X",
              (unsigned)hbn_p1,
              (unsigned)(hbn_p1 & 0xFFu),
              (unsigned)((hbn_p1 >> 8) & 0xFFu),
              (unsigned)((hbn_p1 >> 16) & 0x7Fu),
              (unsigned)((hbn_p1 >> 24) & 0xFFu));
    IPRO_LOGI(LOG_TAG, "HBN_PAD_CTRL_2 0x%08X   HBN_XTAL32K 0x%08X hiz=%u "
                       "pu_xtal=%u pu_buf=%u",
              (unsigned)hbn_p2, (unsigned)xtal32k,
              (unsigned)(xtal32k & 1u),
              (unsigned)((xtal32k >> 19) & 1u),
              (unsigned)((xtal32k >> 18) & 1u));

    if ((hbn_p0 >> 20) & 0xFFu) {
        IPRO_LOGE(LOG_TAG, "en_aon_ctrl is NON-ZERO: the always-on block owns "
                           "at least one AON pad, and its oe/o fields above "
                           "override anything written through GLB.");
    }
}

/*
 * What the AUADC hardware reports back, as opposed to what we asked for.
 * hal_auadc_set_digital_gain() is called after hal_auadc_pdm_init(), because
 * the init path re-runs AUADC_Init(); reading the value back is the only way
 * to know the write actually stuck.
 */
float audio_get_mic_gain_actual(void)
{
    return s_pdm_ready ? hal_auadc_get_digital_gain() : 0.0f;
}

/* How much make-up gain the recorded peak can still take before clipping. */
int audio_headroom_db(void)
{
    float pk;

    level_finish();
    pk = s_level.peak_dbfs;
    if (pk <= -120.0f) {
        return 0;
    }
    return (int)(-pk);
}
