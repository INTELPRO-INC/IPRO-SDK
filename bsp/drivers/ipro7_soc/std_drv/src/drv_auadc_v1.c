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



#include "drv_auadc_v1.h"

void AUADC_Init(AUADC_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    /* set fir mode , select one order or two order fir filter*/
    tmpVal = readl(AUD_BASE + AUD_PDM_ADC_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_FIR_MODE, cfg->firMode);
    writel(tmpVal, AUD_BASE + AUD_PDM_ADC_0_OFFSET);

    /*set adc source */
    tmpVal = readl(AUD_BASE + AUD_PDM_DAC_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_SRC, cfg->source);
    writel(tmpVal, AUD_BASE + AUD_PDM_DAC_0_OFFSET);

    /*set pdm channel left or right*/
    if (cfg->source != AUADC_SOURCE_ANALOG) {
        tmpVal = readl(AUD_BASE + AUD_PDM_PDM_0_OFFSET);
        tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_PDM_SEL, cfg->pdmChannel);
        tmpVal = reg_set_bits(tmpVal, AUD_PDM_0_EN, 1);
        writel(tmpVal, AUD_BASE + AUD_PDM_PDM_0_OFFSET);
    }
    /* Set Clock */
    tmpVal = readl(AUD_BASE + AUDPDM_TOP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_RATE, cfg->clk);
    tmpVal = reg_set_bits(tmpVal, AUD_PDM_ITF_INV_SEL, cfg->pdmItfInvEnable);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_ITF_INV_SEL, cfg->adcItfInvEnable);
    tmpVal = reg_set_bits(tmpVal, AUDIO_CKG_EN, cfg->auadcClkEnable);

    writel(tmpVal, AUD_BASE + AUDPDM_TOP_OFFSET);
}

void AUADC_FifoInit(AUADC_FifoCfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);

    tmpVal = reg_set_bits(tmpVal, AUD_RX_DATA_RES, cfg->resolution);
    tmpVal = reg_set_bits(tmpVal, AUD_RX_DATA_MODE, cfg->ailgnMode);
    tmpVal = reg_set_bits(tmpVal, AUD_RX_DRQ_CNT, cfg->dmaThresholdMode);
    tmpVal = reg_set_bits(tmpVal, AUD_RX_TRG_LEVEL, cfg->FifoIntThreshold);
    tmpVal = reg_set_bits(tmpVal, AUD_RX_DRQ_EN, cfg->dmaEn);

    writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);

    /* Set dma interface */
    tmpVal = readl(AUD_BASE + AUDPDM_ITF_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_ITF_EN, cfg->dmaEn);
    writel(tmpVal, AUD_BASE + AUDPDM_ITF_OFFSET);
}

void AUADC_FifoClear(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_RX_FIFO_FLUSH, 1);
    writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
}

void AUADC_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDPDM_ITF_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_EN, 1);
    writel(tmpVal, AUD_BASE + AUDPDM_ITF_OFFSET);
}

void AUADC_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDPDM_ITF_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_EN, 0);
    writel(tmpVal, AUD_BASE + AUDPDM_ITF_OFFSET);
}

void AUADC_Start(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_RX_CH_EN, 1);
    writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
}

void AUADC_Stop(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_RX_CH_EN, 0);
    writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
}

void AUADC_HPFConfig(uint8_t k1_enable, uint8_t k1, uint8_t k2_enable, uint8_t k2)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUD_PDM_ADC_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_K1, k1);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_K1_EN, k1_enable);
    /* k2 and k2_enable used to be written to each other's fields: the enable
     * flag landed in the 4-bit K2 coefficient [8:5] and the coefficient in
     * the 1-bit K2_EN [9]. Harmless while every caller passed k2 = 0, but any
     * non-zero k2 placed the pole far from where it was asked for and cut
     * into the voice band instead of below it. */
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_K2, k2);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_K2_EN, k2_enable);

    writel(tmpVal, AUD_BASE + AUD_PDM_ADC_1_OFFSET);
}

void AUADC_SetVolume(uint32_t volume)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUD_PDM_ADC_S0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_S0_VOLUME, volume);
    writel(tmpVal, AUD_BASE + AUD_PDM_ADC_S0_OFFSET);
}

void AUADC_IntMask(AUADC_INT_Type intType, int intMask)
{
    uint32_t tmpVal = 0;

    switch (intType) {
        case AUADC_INT_RX_FIFO_THR:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
            if (intMask) {
                tmpVal = reg_clr_bit(tmpVal, AUD_RXA_INT_EN);
            } else {
                tmpVal = reg_set_bit(tmpVal, AUD_RXA_INT_EN);
            }
            writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
            break;
        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
            if (intMask) {
                tmpVal = reg_clr_bit(tmpVal, AUD_RXO_INT_EN);
            } else {
                tmpVal = reg_set_bit(tmpVal, AUD_RXO_INT_EN);
            }
            writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
            break;
        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
            if (intMask) {
                tmpVal = reg_clr_bit(tmpVal, AUD_RXU_INT_EN);
            } else {
                tmpVal = reg_set_bit(tmpVal, AUD_RXU_INT_EN);
            }
            writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
            break;

        case AUADC_INT_NUM_ALL:
            if (intMask) {
                tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
                tmpVal = reg_clr_bit(tmpVal, AUD_RXA_INT_EN);
                tmpVal = reg_clr_bit(tmpVal, AUD_RXU_INT_EN);
                tmpVal = reg_clr_bit(tmpVal, AUD_RXO_INT_EN);
                writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);

            } else {
                tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
                tmpVal = reg_set_bit(tmpVal, AUD_RXA_INT_EN);
                tmpVal = reg_set_bit(tmpVal, AUD_RXU_INT_EN);
                tmpVal = reg_set_bit(tmpVal, AUD_RXO_INT_EN);
                writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_CTRL_OFFSET);
            }

        default:
            break;
    }
}

void AUADC_IntClear(AUADC_INT_Type intType)
{
    uint32_t tmpVal = 0;

    switch (intType) {
        case AUADC_INT_RX_FIFO_THR:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            tmpVal = reg_set_bit(tmpVal, AUD_RXA_INT);
            writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            break;

        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            tmpVal = reg_set_bit(tmpVal, AUD_RXO_INT);
            writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            break;

        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            tmpVal = reg_set_bit(tmpVal, AUD_RXU_INT);
            writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            break;

        case AUADC_INT_NUM_ALL:

            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            tmpVal = reg_set_bit(tmpVal, AUD_RXO_INT);
            tmpVal = reg_set_bit(tmpVal, AUD_RXU_INT);
            tmpVal = reg_set_bit(tmpVal, AUD_RXA_INT);
            writel(tmpVal, AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            break;

        default:
            break;
    }
}

int AUADC_GetIntStatus(AUADC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    int rlt = RESET;

    switch (intType) {
        case AUADC_INT_RX_FIFO_THR:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            rlt = reg_is_bit_set(tmpVal, AUD_RXA_INT);
            break;
        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            rlt = reg_is_bit_set(tmpVal, AUD_RXO_INT);
            break;
        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
            rlt = reg_is_bit_set(tmpVal, AUD_RXU_INT);
            break;

        default:
            break;
    }

    return rlt;
}

void AUADC_SetPDMChannel(AUADC_PDM_Channel_Type pdmChannel)
{
    uint32_t tmpVal;
    tmpVal = readl(AUD_BASE + AUD_PDM_PDM_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_PDM_0_EN, 0);
    writel(tmpVal, AUD_BASE + AUD_PDM_PDM_0_OFFSET);

    tmpVal = readl(AUD_BASE + AUD_PDM_PDM_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUD_ADC_0_PDM_SEL, pdmChannel);
    tmpVal = reg_set_bits(tmpVal, AUD_PDM_0_EN, 1);
    writel(tmpVal, AUD_BASE + AUD_PDM_PDM_0_OFFSET);
}

uint32_t AUADC_GetFifoCount(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_STATUS_OFFSET);
    tmpVal = reg_get_bits(tmpVal, AUD_RXA_CNT);

    return tmpVal;
}

uint32_t AUADC_GetRawData(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUD_PDM_DAC_0_OFFSET);
    tmpVal = reg_get_bits(tmpVal, AUD_ADC_0_SRC);

    if (tmpVal) {
        /* pdm interface */
        tmpVal = readl(AUD_BASE + AUDADC_RX_FIFO_DATA_OFFSET);
    } else {
        /*analog interface */
        tmpVal = readl(AUD_BASE + AUDADC_DATA_OFFSET);
        tmpVal = reg_get_bits(tmpVal, AUDADC_RAW_DATA);
    }

    return tmpVal;
}

int AUADC_FifoDataReady(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDADC_DATA_OFFSET);
    tmpVal = reg_get_bits(tmpVal, AUDADC_DATA_RDY);

    return tmpVal;
}

void AUADC_Set_OSR(AUADC_ADC_AudioMode_OSR_Type osr)
{
    uint32_t tmpVal = 0;
    /* audadc_cmd */
    tmpVal = readl(AUD_BASE + AUDADC_CMD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUDADC_AUDIO_OSR_SEL, osr);
    writel(tmpVal, AUD_BASE + AUDADC_CMD_OFFSET);
}

void AUADC_SetAnalogGain(uint8_t gain)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDADC_CMD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_GAIN, gain);
    writel(tmpVal, AUD_BASE + AUDADC_CMD_OFFSET);
}

uint8_t AUADC_GetAnalogGain(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUDADC_CMD_OFFSET);
    tmpVal = reg_get_bits(tmpVal, AUDADC_PGA_GAIN);

    return (uint8_t)tmpVal;
}

uint32_t AUADC_GetVolume(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUD_BASE + AUD_PDM_ADC_S0_OFFSET);
    tmpVal = reg_get_bits(tmpVal, AUD_ADC_S0_VOLUME);

    return tmpVal;
}

/* ========== Precision ADC (Measure Mode) ========== */

static int32_t auadc_sign_extend_24(uint32_t raw)
{
    if (raw & 0x800000) {
        return (int32_t)(raw | 0xFF000000);
    }
    return (int32_t)raw;
}

void AUADC_SoftReset(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AUD_BASE + AUDADC_DATA_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUDADC_SOFT_RST, 1);
    writel(tmpVal, AUD_BASE + AUDADC_DATA_OFFSET);

    tmpVal = reg_set_bits(tmpVal, AUDADC_SOFT_RST, 0);
    writel(tmpVal, AUD_BASE + AUDADC_DATA_OFFSET);
}

void AUADC_PADC_Init(AUADC_PADC_Cfg_Type *cfg)
{
    AUADC_ADC_AnalogCfg_Type adc_cfg = {0};

    if (cfg == NULL) {
        return;
    }

    /* Analog front-end defaults for measurement */
    adc_cfg.adc_edge_mode = AUADC_ADC_PGA_CLK_FALLING_SENT;
    adc_cfg.adc_clock_phase_invert_en = DISABLE;
    adc_cfg.pga_opmic_bias_cur = AUADC_ADC_PGA_BIAS_6_UA;
    adc_cfg.pga_opaaf_bias_cur = AUADC_ADC_PGA_BIAS_6_UA;
    adc_cfg.pga_chopper = AUADC_ADC_PGA_OPMIC_ON_OPAAF_ON;
    adc_cfg.pga_hf_res = AUADC_ADC_PGA_HF_480K;
    adc_cfg.pga_chopper_en = ENABLE;
    adc_cfg.pga_chopper_freq = AUADC_ADC_PGA_CHOPPER_FREQ_64K;

    adc_cfg.sdm_lowpower_en = DISABLE;
    adc_cfg.sdm_bias_cur = AUADC_ADC_SDM_BIAS_6_UA;
    adc_cfg.sdm_i_first_num = AUADC_ADC_SDM_I_FIRST_NUM_48;
    adc_cfg.sdm_i_sec_num = AUADC_ADC_SDM_I_SEC_NUM_36;
    adc_cfg.dem_en = ENABLE;
    adc_cfg.sdm_qg = AUADC_ADC_SDM_QG_DIV14;
    adc_cfg.sdm_dither_en = ENABLE;
    adc_cfg.sdm_dither_level = AUADC_ADC_SDM_DL_LSB_DIV15;
    adc_cfg.sdm_dither_order = AUADC_ADC_SDM_ORDER_1;

    /* Power up PGA + SDM */
    adc_cfg.pga_circuit_en = ENABLE;
    adc_cfg.sdm_circuit_en = ENABLE;

    /* Channel selection from user config */
    adc_cfg.pga_positive_en = ENABLE;
    adc_cfg.pga_posi_ch = cfg->posi_ch;
    adc_cfg.pga_coupled_mode = cfg->pga_mode;
    adc_cfg.pga_gain = cfg->pga_gain;

    if (cfg->pga_mode == AUADC_ADC_PGA_MODE_DC_DIFF) {
        adc_cfg.pga_negative_en = ENABLE;
        adc_cfg.pga_nega_ch = cfg->nega_ch;
    } else {
        adc_cfg.pga_negative_en = DISABLE;
    }

    /* Measure mode */
    adc_cfg.adc_mode = AUADC_ADC_FILT_MODE_MEASURE;
    adc_cfg.measure_filter_mode = cfg->filter;
    adc_cfg.measure_rate = cfg->rate;
    adc_cfg.audio_osr = AUADC_ADC_Audio_OSR_128;

    AUADC_SoftReset();
    AUADC_ADC_Config(&adc_cfg);
}

int32_t AUADC_PADC_Read(uint32_t timeout_us)
{
    uint32_t tmpVal;
    uint32_t elapsed = 0;

    while (elapsed < timeout_us) {
        tmpVal = readl(AUD_BASE + AUDADC_DATA_OFFSET);
        if (reg_get_bits(tmpVal, AUDADC_DATA_RDY)) {
            return auadc_sign_extend_24(reg_get_bits(tmpVal, AUDADC_RAW_DATA));
        }
        arch_delay_us(1);
        elapsed++;
    }

    return AUADC_PADC_TIMEOUT;
}

int AUADC_PADC_ReadNonBlocking(int32_t *result)
{
    uint32_t tmpVal;

    tmpVal = readl(AUD_BASE + AUDADC_DATA_OFFSET);
    if (!reg_get_bits(tmpVal, AUDADC_DATA_RDY)) {
        return -1;
    }

    *result = auadc_sign_extend_24(reg_get_bits(tmpVal, AUDADC_RAW_DATA));
    return 0;
}

/* ========== Full Analog Config ========== */

void AUADC_ADC_Config(AUADC_ADC_AnalogCfg_Type *adc_cfg)
{
    uint32_t tmpVal = 0;
    uint32_t tmpVal2 = 0;
    uint8_t ch_en = 0;

    if(adc_cfg == NULL){
        return;
    }

    /* audadc_ana_cfg1 */
    tmpVal = readl(AUD_BASE + AUDADC_ANA_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUDADC_SEL_EDGE, adc_cfg->adc_edge_mode);
    tmpVal = reg_set_bits(tmpVal, AUDADC_CKB_EN, adc_cfg->adc_clock_phase_invert_en);
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_LP_EN, adc_cfg->adc_clock_phase_invert_en);
    if(adc_cfg->pga_opmic_bias_cur < AUADC_ADC_PGA_BIAS_7_UA){
        tmpVal = reg_set_bits(tmpVal, AUDADC_ICTRL_PGA_MIC, adc_cfg->pga_opmic_bias_cur);
    }else{
        tmpVal = reg_set_bits(tmpVal, AUDADC_ICTRL_PGA_MIC, adc_cfg->pga_opmic_bias_cur - AUADC_ADC_PGA_BIAS_7_UA);
        tmpVal2 = readl(AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
        /* AUADC_AUDADC_RESERVED2 removed in new reg, skip this setting */
        writel(tmpVal2, AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
    }
    if(adc_cfg->pga_opaaf_bias_cur < AUADC_ADC_PGA_BIAS_7_UA){
        tmpVal = reg_set_bits(tmpVal, AUDADC_ICTRL_PGA_AAF, adc_cfg->pga_opaaf_bias_cur);
    }else{
        tmpVal = reg_set_bits(tmpVal, AUDADC_ICTRL_PGA_AAF, adc_cfg->pga_opaaf_bias_cur - AUADC_ADC_PGA_BIAS_7_UA);
        tmpVal2 = readl(AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
        /* AUADC_AUDADC_RESERVED1 removed in new reg, skip this setting */
        writel(tmpVal2, AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
    }
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_RHPAS_SEL, adc_cfg->pga_hf_res);
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_CHOP_CFG, adc_cfg->pga_chopper);
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_CHOP_EN, adc_cfg->pga_chopper_en);
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_CHOP_FREQ, adc_cfg->pga_chopper_freq);
    // tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_CHOP_CKSEL, adc_cfg->pga_chopper_clk_source);
    writel(tmpVal, AUD_BASE + AUDADC_ANA_CFG1_OFFSET);

    /* audadc_ana_cfg2 */
    tmpVal = readl(AUD_BASE + AUDADC_ANA_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUDADC_SDM_LP_EN, adc_cfg->sdm_lowpower_en);
    tmpVal = reg_set_bits(tmpVal, AUDADC_ICTRL_ADC, adc_cfg->sdm_bias_cur);
    tmpVal = reg_set_bits(tmpVal, AUDADC_NCTRL_ADC1, adc_cfg->sdm_i_first_num);
    tmpVal = reg_set_bits(tmpVal, AUDADC_NCTRL_ADC2, adc_cfg->sdm_i_sec_num);
    tmpVal = reg_set_bits(tmpVal, AUDADC_DEM_EN, adc_cfg->dem_en);
    tmpVal = reg_set_bits(tmpVal, AUDADC_QUAN_GAIN, adc_cfg->sdm_qg);
    tmpVal = reg_set_bits(tmpVal, AUDADC_DITHER_ENA, adc_cfg->sdm_dither_en);
    tmpVal = reg_set_bits(tmpVal, AUDADC_DITHER_SEL, adc_cfg->sdm_dither_level);
    tmpVal = reg_set_bits(tmpVal, AUDADC_DITHER_ORDER, adc_cfg->sdm_dither_order);
    writel(tmpVal, AUD_BASE + AUDADC_ANA_CFG2_OFFSET);

    /* audadc_cmd */
    tmpVal = readl(AUD_BASE + AUDADC_CMD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_PU, adc_cfg->pga_circuit_en);
    tmpVal = reg_set_bits(tmpVal, AUDADC_SDM_PU, adc_cfg->sdm_circuit_en);
    tmpVal = reg_set_bits(tmpVal, AUDADC_CONV, 0);
    if(adc_cfg->pga_positive_en) {
        ch_en |= (0x01 << 1);
    }
    if(adc_cfg->pga_negative_en) {
        ch_en |= (0x01 << 0);
    }
    tmpVal = reg_set_bits(tmpVal, AUDADC_CHANNEL_EN, ch_en);
    tmpVal = reg_set_bits(tmpVal, AUDADC_CHANNEL_SELP, adc_cfg->pga_posi_ch);
    tmpVal = reg_set_bits(tmpVal, AUDADC_CHANNEL_SELN, adc_cfg->pga_nega_ch);
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_MODE, adc_cfg->pga_coupled_mode);
    tmpVal = reg_set_bits(tmpVal, AUDADC_PGA_GAIN, adc_cfg->pga_gain);
    if(adc_cfg->adc_mode == AUADC_ADC_FILT_MODE_AUDIO) {
        tmpVal = reg_set_bits(tmpVal, AUDADC_MEAS_FILTER_EN, 0);
    } else if(adc_cfg->adc_mode == AUADC_ADC_FILT_MODE_MEASURE) {
        tmpVal = reg_set_bits(tmpVal, AUDADC_MEAS_FILTER_EN, 1);
    }
    tmpVal = reg_set_bits(tmpVal, AUDADC_AUDIO_OSR_SEL, adc_cfg->audio_osr);
    tmpVal = reg_set_bits(tmpVal, AUDADC_MEAS_FILTER_TYPE, adc_cfg->measure_filter_mode);
    tmpVal = reg_set_bits(tmpVal, AUDADC_MEAS_ODR_SEL, adc_cfg->measure_rate);
    writel(tmpVal, AUD_BASE + AUDADC_CMD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUDADC_CONV, 1);
    writel(tmpVal, AUD_BASE + AUDADC_CMD_OFFSET);
}
