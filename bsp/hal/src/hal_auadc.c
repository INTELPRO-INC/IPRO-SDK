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

#include <stdlib.h>
#include <string.h>
#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#endif
#include "hal_auadc.h"
#include "drv_auadc_v1.h"
#include "hal_dma.h"
#include "drv_common.h"
#include "hal_gpio.h"
#include "system_soc.h"
#if defined(IPRO7)
#include "drv_hbn.h"
#include "hbn_reg.h"
#endif


static int8_t audio_dma_ch = -1;
static uint16_t pcm_frame_size;
static int16_t *pcm_frame_buf[2];
static hal_auadc_callback_t pcm_frame_event;
static int pcm_frame_idx;
static dma_device_t *audio_dma_dev = NULL;
#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
static dma_ch_handle_t audio_dma_handle = NULL;
#endif


static void audio_dma_callback(void *p_arg, uint32_t flag)
{
    int idx = pcm_frame_idx;

    pcm_frame_idx = !pcm_frame_idx;
    
    if (pcm_frame_event != NULL) {
        pcm_frame_event(idx);
    }
}


static void audio_amic_gpio_init(hal_auadc_amic_cfg_t *cfg)
{
#ifdef IPRO7
    uint8_t audioPinList[] = {4, 5, 6, 7, 10, 11, 12, 13};
#else
    #error("Unsupported platform");
#endif
    GLB_GPIO_Cfg_Type gpioCfg;

    gpioCfg.gpioFun = GPIO_FUN_ANALOG;
    gpioCfg.gpioMode = GPIO_MODE_ANALOG;
    gpioCfg.pullType = GPIO_PULL_NONE;
    gpioCfg.drive = 0;
    gpioCfg.smtCtrl = 0;

    gpioCfg.gpioPin = audioPinList[cfg->amic_pos_ch];
    GLB_GPIO_Init(&gpioCfg);

    if (!cfg->amic_single_ended) {
        gpioCfg.gpioPin = audioPinList[cfg->amic_neg_ch];
        GLB_GPIO_Init(&gpioCfg);
    }
}

#if defined(IPRO7)
/* The two XTAL32K pads, which are also the always-on block's two "MSB" GPIO
 * pads. Only they need the treatment below. */
#define AUADC_PDM_XTAL32K_PIN_A   18U
#define AUADC_PDM_XTAL32K_PIN_B   19U

static bool pdm_pin_is_xtal32k_pad(uint8_t pin)
{
    return (pin == AUADC_PDM_XTAL32K_PIN_A) || (pin == AUADC_PDM_XTAL32K_PIN_B);
}

/*
 * Take GPIO18/19 back from the always-on block before muxing them for PDM.
 *
 * On IPRO7 those two pins are the 32 kHz crystal pads, and the AON block owns
 * them by default. Its per-pad ownership bits are split across two registers:
 *
 *   HBN_PAD_CTRL_0 [27:20]  reg_en_aon_ctrl_gpio_7_0  (the low 8 AON pads)
 *   HBN_PAD_CTRL_2 [11:10]  reg_en_aon_ctrl_gpio_msb  (these two)
 *
 * PAD_CTRL_0's field reads 0 on the IPRO7AI EVK, which looks like "AON owns
 * nothing" if that is the only register checked - while PAD_CTRL_2's reads
 * 0b11 with its ie_smt_msb field [9:8] at 0: the pads belong to AON and their
 * input receivers are off. A pin in that state reads 0 whatever is on it and
 * will not even follow its own pull-up, so a PDM microphone on GPIO19 is
 * silent and every recording is a flat line at -84 dBFS. Found the hard way
 * on ipro7ai_evk_demo, whose `mic probe` measured exactly that.
 *
 * The crystal oscillator shares the pads. HBN_Power_Off_Xtal_32K() clears the
 * power bits but never sets XTAL32K_HIZ_EN, which is what actually detaches
 * the pads; and a BLE controller probing for a 32K crystal at boot re-powers
 * them (HBN_Power_On_Xtal_32K clears HIZ_EN) after the application has set
 * its pins up. That is why this runs from hal_auadc_start() as well as from
 * init - it is idempotent, and each capture then begins with the pads in a
 * known state regardless of what ran in between. RC32K is selected first so
 * nothing is left leaning on the crystal when it is tri-stated.
 *
 * This lives in the HAL rather than in each app because the HAL is what muxes
 * the pins; an app that asks for PDM on 18/19 has said everything it needs
 * to.
 */
static void audio_pdm_reclaim_xtal32k_pads(const hal_auadc_pdm_cfg_t *cfg)
{
    uint32_t v;

    if (!pdm_pin_is_xtal32k_pad(cfg->pdm_clk_pin) &&
        !pdm_pin_is_xtal32k_pad(cfg->pdm_in_pin)) {
        return;
    }

    /* Hand the two MSB pads to GLB and switch their input receivers on. */
    v = readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET);
    v &= HBN_REG_EN_AON_CTRL_GPIO_MSB_UMSK;
    v |= HBN_REG_AON_GPIO_IE_SMT_MSB_MSK;
    writel(v, HBN_BASE + HBN_PAD_CTRL_2_OFFSET);

    /* And the low eight, should a board variant route them through there. */
    v = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    v &= HBN_REG_EN_AON_CTRL_GPIO_7_0_UMSK;
    v |= HBN_REG_AON_GPIO_IE_SMT_MSK;
    writel(v, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);

    /* Detach the crystal oscillator from the pads. */
    v = readl(HBN_BASE + HBN_XTAL32K_OFFSET);
    if ((v & HBN_XTAL32K_HIZ_EN_MSK) == 0U) {
        /* Only switch the 32K source if something is still on the crystal.
         * HBN_32K_Sel() is a ROM call that re-applies the efuse RC32K trim on
         * IPRO7 (see hal_pm.c), which would throw away the runtime RC32K
         * calibration the BLE controller did at boot - a few hundred ppm,
         * more than a connected sleep can absorb. On a board that is already
         * on RC32K, which is every board with a microphone on these pins,
         * there is nothing to select. */
        uint32_t glb = readl(HBN_BASE + HBN_GLB_OFFSET);

        if (((glb & HBN_F32K_SEL_MSK) >> HBN_F32K_SEL_POS) != (uint32_t)HBN_32K_RC) {
            HBN_32K_Sel(HBN_32K_RC);
        }
        v &= HBN_PU_XTAL32K_UMSK;
        v &= HBN_PU_XTAL32K_BUF_UMSK;
        v |= HBN_XTAL32K_HIZ_EN_MSK;
        writel(v, HBN_BASE + HBN_XTAL32K_OFFSET);
    }
}
#else
static void audio_pdm_reclaim_xtal32k_pads(const hal_auadc_pdm_cfg_t *cfg)
{
    (void)cfg;
}
#endif

static hal_auadc_pdm_cfg_t s_pdm_cfg;   /* last PDM configuration, for start() */

static void audio_pdm_gpio_init(hal_auadc_pdm_cfg_t *cfg)
{
    GLB_GPIO_Cfg_Type gpioCfg;

    /* Must precede GLB_GPIO_Init(): the mux is GLB's, and GLB does not have
     * the pins until AON lets go of them. */
    audio_pdm_reclaim_xtal32k_pads(cfg);

    gpioCfg.gpioFun = GPIO_FUN_PDM;
    gpioCfg.gpioMode = GPIO_MODE_AF;
    gpioCfg.pullType = GPIO_PULL_NONE;
    gpioCfg.drive = 1;
    gpioCfg.smtCtrl = 1;

    gpioCfg.gpioPin = cfg->pdm_clk_pin;
    GLB_GPIO_Init(&gpioCfg);

    gpioCfg.gpioPin = cfg->pdm_in_pin;
    GLB_GPIO_Init(&gpioCfg);
}


static void audio_amic_adc_init(hal_auadc_amic_cfg_t *cfg)
{
    AUADC_ADC_AnalogCfg_Type auadc_adc_cfg = {
        .adc_edge_mode = AUADC_ADC_PGA_CLK_FALLING_SENT,
        .adc_clock_phase_invert_en = DISABLE,
        .pga_opmic_bias_cur = AUADC_ADC_PGA_BIAS_4_UA,
        .pga_opaaf_bias_cur = AUADC_ADC_PGA_BIAS_4_UA,
        .pga_chopper = AUADC_ADC_PGA_OPMIC_ON_OPAAF_ON,
        .pga_hf_res = AUADC_ADC_PGA_HF_480K,
        .pga_chopper_en = ENABLE,
        .pga_chopper_freq = AUADC_ADC_PGA_CHOPPER_FREQ_128K,

        .sdm_lowpower_en = DISABLE,
        .sdm_bias_cur = AUADC_ADC_SDM_BIAS_5_UA,
        .sdm_i_first_num = AUADC_ADC_SDM_I_FIRST_NUM_48,
        .sdm_i_sec_num = AUADC_ADC_SDM_I_SEC_NUM_24,
        .dem_en = ENABLE,
        .sdm_qg = AUADC_ADC_SDM_QG_DIV12,
        .sdm_dither_en = ENABLE,
        .sdm_dither_level = AUADC_ADC_SDM_DL_LSB_DIV7_5,
        .sdm_dither_order = AUADC_ADC_SDM_ORDER_0,

        .pga_circuit_en = ENABLE,
        .sdm_circuit_en = ENABLE,
        .pga_positive_en = ENABLE,
        .pga_negative_en = ENABLE,
        .pga_posi_ch = cfg->amic_pos_ch,
        .pga_nega_ch = cfg->amic_neg_ch,
        .pga_coupled_mode = cfg->amic_single_ended,
        .pga_gain = AUADC_ADC_PGA_GAIN_42DB,
        .adc_mode = AUADC_ADC_FILT_MODE_AUDIO,
        .audio_osr = AUADC_ADC_Audio_OSR_128,
        .measure_filter_mode = AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
        .measure_rate = AUADC_ADC_MEASURE_RATE_SPS_2_5,
    };

    AUADC_ADC_Config(&auadc_adc_cfg);
}

/*
 * Nominal -> real rate. Measured, not derived.
 *
 * The audio clock is 2.032 MHz and the AUADC divides it down, so none of the
 * nominal names are exact: the "16 kHz" setting is really 2032000/128 = 15875
 * Hz, which audio_port.h has carried as a comment for a while.
 *
 * Slot 3 is worse than inexact. The driver enum names it AUADC_CLK_RESVERSE1
 * and comments it as 32K; on an IPRO7AI EVK it clocks out 423040 samples in
 * 8.913 s, i.e. ~47.5 kHz - three times the 16 kHz setting (15875 x 3 =
 * 47625), not twice. The comment is wrong. Treat the figures here as a
 * starting point for buffer sizing only; anything needing an absolute time
 * base must calibrate against a known tone.
 */
uint32_t hal_auadc_real_rate(uint32_t nominal_rate)
{
    switch (nominal_rate) {
        case 8000:  return 7937u;
        case 32000: return 47625u;   /* slot 3: measured, despite the name */
        case 48000: return 48000u;   /* slot 4: not characterised */
        default:    return 15875u;
    }
}

/*
 * The enum in drv_auadc_v1.h names slot 3 AUADC_CLK_RESVERSE1 while commenting
 * it as 32K. AUADC_CLK_32K_HZ does not exist - the IS_AUADC_CLOCK_TYPE macro
 * in the same header references it (and AUADC_CLK_96K_HZ) and would not
 * compile if anything expanded it. Use the slot that is actually defined.
 */
static uint32_t auadc_clk_sel(uint32_t nominal_rate)
{
    switch (nominal_rate) {
        case 8000:  return AUADC_CLK_8K_HZ;
        case 32000: return AUADC_CLK_RESVERSE1;
        case 48000: return AUADC_CLK_48K_HZ;
        default:    return AUADC_CLK_16K_HZ;
    }
}

static void audio_auadc_init(int auadc_src, void *cfg, uint32_t rate)
{
    AUADC_Cfg_Type auadc_cfg = {
        auadc_clk_sel(rate),
        ENABLE,
        DISABLE,
        DISABLE,
        DISABLE,
        auadc_src,
        AUADC_PDM_LEFT,
    };

    AUADC_FifoCfg_Type auadc_fifo_cfg = {
        AUADC_RES_16_BITS,
        AUADC_FIFO_AILGN_MSB_AT_BIT15,
        0,
        AUADC_DRQ_EQUEL_TO_IRQ,
        ENABLE,
    };

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUIDO);
    GLB_Set_AUDIO_CLK(0, 1, GLB_AUDIO_CLK_SRC_2P032M, 4);

    AUADC_Disable();
    AUADC_Enable();

    if (auadc_src == AUADC_SOURCE_ANALOG) {
        audio_amic_adc_init((hal_auadc_amic_cfg_t *)cfg);
    } else {
        if (auadc_src == AUADC_SOURCE_PDM_CH_LEFT)
            auadc_cfg.pdmChannel = AUADC_PDM_LEFT;
        else if (auadc_src == AUADC_SOURCE_PDM_CH_RIGHT)
            auadc_cfg.pdmChannel = AUADC_PDM_RIGHT;
        auadc_cfg.source = AUADC_SOURCE_PDM;
    }

    AUADC_Init(&auadc_cfg);
    AUADC_FifoInit(&auadc_fifo_cfg);
}

static void audio_dma_init(void)
{
    audio_dma_dev = dma_getDevice(DMA0_ID, audio_dma_ch);
    if (audio_dma_dev == NULL) {
        return;
    }

    audio_dma_dev->id = DMA0_ID;
    audio_dma_dev->ch = audio_dma_ch;
    audio_dma_dev->direction = DMA_PERIPH_TO_MEMORY;
    audio_dma_dev->src_req = DMA_REQUEST_AUD_ADC;
    audio_dma_dev->dst_req = DMA_REQUEST_NONE;
    audio_dma_dev->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    audio_dma_dev->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    audio_dma_dev->src_burst_size = DMA_BURST_INCR1;
    audio_dma_dev->dst_burst_size = DMA_BURST_INCR1;
    audio_dma_dev->src_width = DMA_TRANSFER_WIDTH_16BIT;
    audio_dma_dev->dst_width = DMA_TRANSFER_WIDTH_16BIT;
    audio_dma_dev->transfer_mode = DMA_LLI_PINGPONG_MODE;
    audio_dma_dev->lli_cfg = NULL;
    audio_dma_dev->callback = audio_dma_callback;

    dma_setConfig(audio_dma_dev);
    dma_init(audio_dma_dev->id, audio_dma_ch);

    /* Enable DMA interrupt */
    dma_ioctl(audio_dma_dev->id, audio_dma_dev->ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(audio_dma_dev->id, audio_dma_dev->ch, DMA_CTRL_SET_CALLBACK, audio_dma_callback);
}


int hal_auadc_amic_init(hal_auadc_amic_cfg_t *cfg)
{
    if (cfg == NULL) {
        return -1;
    }

    if (cfg->amic_single_ended > 1) {
        return -1;
    }

    if (cfg->amic_pos_ch > 7) {
        return -1;
    }

    if (!cfg->amic_single_ended) {
        if (cfg->amic_neg_ch > 7) {
            return -1;
        }
    }

    if (cfg->pcm_frame_size < 1 || cfg->pcm_frame_size > 4095) {
        return -1;
    }

    if (cfg->pcm_frame_buf[0] == NULL) {
        return -1;
    }

    if (cfg->pcm_frame_buf[1] == NULL) {
        return -1;
    }

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    if (audio_dma_handle == NULL) {
        audio_dma_handle = dma_request_channel(DMA_REQUEST_AUD_ADC,
                                               DMA_PERIPH_TO_MEMORY,
                                               "auadc");
        if (!audio_dma_handle) {
            return -1;
        }
        audio_dma_ch = (int8_t)dma_handle_to_ch(audio_dma_handle);
    }
#else
    if (audio_dma_ch < 0) {
        audio_dma_ch = DMA0_CH0_INDEX;
    }
#endif

    audio_amic_gpio_init(cfg);
    audio_auadc_init(AUADC_SOURCE_ANALOG, cfg, 0);
    audio_dma_init();

    pcm_frame_size = cfg->pcm_frame_size;
    pcm_frame_buf[0] = cfg->pcm_frame_buf[0];
    pcm_frame_buf[1] = cfg->pcm_frame_buf[1];
    pcm_frame_event = cfg->pcm_frame_event;

    return 0;
}

int hal_auadc_pdm_init(hal_auadc_pdm_cfg_t *cfg)
{
    if (cfg == NULL) {
        return -1;
    }
    s_pdm_cfg = *cfg;

    if (cfg->pcm_frame_size < 1 || cfg->pcm_frame_size > 4095) {
        return -1;
    }

    if (cfg->pcm_frame_buf[0] == NULL) {
        return -1;
    }

    if (cfg->pcm_frame_buf[1] == NULL) {
        return -1;
    }

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    if (audio_dma_handle == NULL) {
        audio_dma_handle = dma_request_channel(DMA_REQUEST_AUD_ADC,
                                               DMA_PERIPH_TO_MEMORY,
                                               "auadc");
        if (!audio_dma_handle) {
            return -1;
        }
        audio_dma_ch = (int8_t)dma_handle_to_ch(audio_dma_handle);
    }
#else
    if (audio_dma_ch < 0) {
        audio_dma_ch = DMA0_CH0_INDEX;
    }
#endif

    audio_pdm_gpio_init(cfg);
    if (cfg->pdm_ch == HAL_AUADC_PDM_CH_LEFT) {
        audio_auadc_init(AUADC_SOURCE_PDM_CH_LEFT, NULL, cfg->sample_rate);
    } else {
        audio_auadc_init(AUADC_SOURCE_PDM_CH_RIGHT, NULL, cfg->sample_rate);
    }
    audio_dma_init();

    pcm_frame_size = cfg->pcm_frame_size;
    pcm_frame_buf[0] = cfg->pcm_frame_buf[0];
    pcm_frame_buf[1] = cfg->pcm_frame_buf[1];
    pcm_frame_event = cfg->pcm_frame_event;

    return 0;
}

int hal_auadc_start(void)
{
    if (audio_dma_ch < 0) {
        return -1;
    }

    /* See audio_pdm_reclaim_xtal32k_pads(): a 32K-crystal probe between init
     * and this start would otherwise have taken the pins back. Harmless when
     * the pins are not 18/19, or when nothing intervened. */
    audio_pdm_reclaim_xtal32k_pads(&s_pdm_cfg);

    pcm_frame_idx = 0;

    dma_reload(DMA0_ID, audio_dma_ch, DMA_ADDR_AUD_RDR, (uint32_t)pcm_frame_buf[0], pcm_frame_size, NULL);
    dma_channel_start(DMA0_ID, audio_dma_ch);

    AUADC_Start();

    return 0;
}

int hal_auadc_stop(void)
{
    if (audio_dma_ch < 0) {
        return -1;
    }

    dma_channel_stop(DMA0_ID, audio_dma_ch);

    AUADC_Stop();
    AUADC_FifoClear();

    return 0;
}

void hal_auadc_deinit(void)
{
    if (audio_dma_ch < 0) {
        return;
    }

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    if (audio_dma_handle) {
        dma_release_channel(audio_dma_handle);
        audio_dma_handle = NULL;
    }
#endif
    audio_dma_ch = -1;
    audio_dma_dev = NULL;
    pcm_frame_event = NULL;
}

void hal_auadc_set_high_pass_filter(int k1, int k2)
{
    uint8_t k1_en = (k1 != 0) ? 1 : 0;
    uint8_t k2_en = (k2 != 0) ? 1 : 0;

    AUADC_HPFConfig(k1_en, (uint8_t)k1, k2_en, (uint8_t)k2);
}

void hal_auadc_set_analog_gain(int gain_db)
{
    uint8_t pga_gain;

    if (gain_db < 6) {
        gain_db = 6;
    }

    if (gain_db > 42) {
        gain_db = 42;
    }

    pga_gain = (uint8_t)(gain_db / 3);
    AUADC_SetAnalogGain(pga_gain);
}

void hal_auadc_set_digital_gain(float gain_db)
{
    uint32_t volume;

    if (gain_db < -95.5f) {
        gain_db = -95.5f;
    }

    if (gain_db > 18.0f) {
        gain_db = 18.0f;
    }

    volume = (uint32_t)((int)(gain_db * 2) & 0x1FF);
    AUADC_SetVolume(volume);
}

int hal_auadc_get_analog_gain(void)
{
    uint8_t pga_gain;

    pga_gain = AUADC_GetAnalogGain();

    if (pga_gain < 2) {
        pga_gain = 2;
    }

    if (pga_gain > 14) {
        pga_gain = 14;
    }

    return pga_gain * 3;
}

float hal_auadc_get_digital_gain(void)
{
    uint32_t volume;
    int vol_signed;

    volume = AUADC_GetVolume();
    vol_signed = (int)(volume & 0x1FF);

    if (vol_signed >> 8) {
        vol_signed -= 512;
    }

    return (float)vol_signed / 2.0f;
}

