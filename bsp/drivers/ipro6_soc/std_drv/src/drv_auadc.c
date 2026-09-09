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



#include "drv_auadc.h"

static intCallback_Type *auadcIntCbfArra[AUADC_INT_NUM_ALL] = { NULL };

void AUADC_Init(AUADC_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    /* set fir mode , select one order or two order fir filter */
    tmpVal = readl(AUADC_BASE + AUADC_PDM_ADC_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_FIR_MODE, cfg->firMode);
    writel(tmpVal, AUADC_BASE + AUADC_PDM_ADC_0_OFFSET);

    if (cfg->source == AUADC_SOURCE_ANALOG) {
        /*set adc source analog */
        tmpVal = readl(AUADC_BASE + AUADC_PDM_DAC_0_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, AUADC_ADC_0_SRC);
        writel(tmpVal, AUADC_BASE + AUADC_PDM_DAC_0_OFFSET);
        /* disable pdm */
        tmpVal = readl(AUADC_BASE + AUADC_PDM_PDM_0_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, AUADC_PDM_0_EN);
        writel(tmpVal, AUADC_BASE + AUADC_PDM_PDM_0_OFFSET);

    } else {
        /*set adc source pdm */
        tmpVal = readl(AUADC_BASE + AUADC_PDM_DAC_0_OFFSET);
        tmpVal = reg_set_bit(tmpVal, AUADC_ADC_0_SRC);
        writel(tmpVal, AUADC_BASE + AUADC_PDM_DAC_0_OFFSET);
        /* set pdm channel */
        tmpVal = readl(AUADC_BASE + AUADC_PDM_PDM_0_OFFSET);
        tmpVal = reg_set_bit(tmpVal, AUADC_PDM_0_EN);
        if (cfg->source == AUADC_SOURCE_PDM_LEFT) {
            tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_PDM_SEL, 0);
        } else {
            tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_PDM_SEL, 1);
        }
        writel(tmpVal, AUADC_BASE + AUADC_PDM_PDM_0_OFFSET);
    }

    /* Set Clock */
    tmpVal = readl(AUADC_BASE + AUADC_AUDPDM_TOP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_RATE, cfg->clk);
    tmpVal = reg_set_bits(tmpVal, AUADC_PDM_ITF_INV_SEL, cfg->pdmItfInvEnable);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_ITF_INV_SEL, cfg->adcItfInvEnable);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDIO_CKG_EN, cfg->auadcClkEnable);
    writel(tmpVal, AUADC_BASE + AUADC_AUDPDM_TOP_OFFSET);

    /* To avoid excessive PDM frequency, the OSR was reduced from 128 to 64 */
    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_CMD_OFFSET);
    if (cfg->source != AUADC_SOURCE_ANALOG && cfg->clk >= AUADC_CLK_32K_HZ) {
        tmpVal = reg_set_bit(tmpVal, AUADC_AUDADC_AUDIO_OSR_SEL);
    } else {
        tmpVal = reg_clr_bit(tmpVal, AUADC_AUDADC_AUDIO_OSR_SEL);
    }
    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_CMD_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(AUPDM_IRQn, AUADC_IRQHandler);
#endif
}

void AUADC_FifoInit(AUADC_FifoCfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);

    tmpVal = reg_set_bits(tmpVal, AUADC_RX_DATA_RES, cfg->resolution);
    tmpVal = reg_set_bits(tmpVal, AUADC_RX_DATA_MODE, cfg->ailgnMode);
    tmpVal = reg_set_bits(tmpVal, AUADC_RX_DRQ_CNT, cfg->dmaThresholdMode);
    tmpVal = reg_set_bits(tmpVal, AUADC_RX_TRG_LEVEL, cfg->FifoIntThreshold);
    tmpVal = reg_set_bits(tmpVal, AUADC_RX_DRQ_EN, cfg->dmaEn);

    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);

    /* Set dma interface */
    tmpVal = readl(AUADC_BASE + AUADC_AUDPDM_ITF_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_ITF_EN, cfg->dmaEn);
    writel(tmpVal, AUADC_BASE + AUADC_AUDPDM_ITF_OFFSET);
}

void AUADC_FifoClear(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_RX_FIFO_FLUSH, 1);
    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
}

void AUADC_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_AUDPDM_ITF_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_EN, 1);
    writel(tmpVal, AUADC_BASE + AUADC_AUDPDM_ITF_OFFSET);
}

void AUADC_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_AUDPDM_ITF_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_EN, 0);
    writel(tmpVal, AUADC_BASE + AUADC_AUDPDM_ITF_OFFSET);
}

void AUADC_Start(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_RX_CH_EN, 1);
    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
}

void AUADC_Stop(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_RX_CH_EN, 0);
    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
}

void AUADC_HPFConfig(uint8_t k1_enable, uint8_t k1, uint8_t k2_enable, uint8_t k2)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_PDM_ADC_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_K1, k1);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_K1_EN, k1_enable);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_K2, k2_enable);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_0_K2_EN, k2);

    writel(tmpVal, AUADC_BASE + AUADC_PDM_ADC_1_OFFSET);
}

void AUADC_SetVolume(uint32_t volume)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_PDM_ADC_S0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_ADC_S0_VOLUME, volume);
    writel(tmpVal, AUADC_BASE + AUADC_PDM_ADC_S0_OFFSET);
}

void AUADC_IntMask(AUADC_INT_Type intType, int intMask)
{
    uint32_t tmpVal = 0;

    switch (intType) {
        case AUADC_INT_RX_FIFO_THR:
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
            if (intMask) {
                tmpVal = reg_clr_bit(tmpVal, AUADC_RXA_INT_EN);
            } else {
                tmpVal = reg_set_bit(tmpVal, AUADC_RXA_INT_EN);
            }
            writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
            break;
        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
            if (intMask) {
                tmpVal = reg_clr_bit(tmpVal, AUADC_RXO_INT_EN);
            } else {
                tmpVal = reg_set_bit(tmpVal, AUADC_RXO_INT_EN);
            }
            writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
            break;
        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
            if (intMask) {
                tmpVal = reg_clr_bit(tmpVal, AUADC_RXU_INT_EN);
            } else {
                tmpVal = reg_set_bit(tmpVal, AUADC_RXU_INT_EN);
            }
            writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
            break;

        case AUADC_INT_NUM_ALL:
            if (intMask) {
                tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
                tmpVal = reg_clr_bit(tmpVal, AUADC_RXA_INT_EN);
                tmpVal = reg_clr_bit(tmpVal, AUADC_RXU_INT_EN);
                tmpVal = reg_clr_bit(tmpVal, AUADC_RXO_INT_EN);
                writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);

            } else {
                tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
                tmpVal = reg_set_bit(tmpVal, AUADC_RXA_INT_EN);
                tmpVal = reg_set_bit(tmpVal, AUADC_RXU_INT_EN);
                tmpVal = reg_set_bit(tmpVal, AUADC_RXO_INT_EN);
                writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_CTRL_OFFSET);
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
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            tmpVal = reg_set_bit(tmpVal, AUADC_RXA_INT);
            writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            break;

        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            tmpVal = reg_set_bit(tmpVal, AUADC_RXO_INT);
            writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            break;

        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            tmpVal = reg_set_bit(tmpVal, AUADC_RXU_INT);
            writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            break;

        case AUADC_INT_NUM_ALL:

            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            tmpVal = reg_set_bit(tmpVal, AUADC_RXO_INT);
            tmpVal = reg_set_bit(tmpVal, AUADC_RXU_INT);
            tmpVal = reg_set_bit(tmpVal, AUADC_RXA_INT);
            writel(tmpVal, AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            break;

        default:
            break;
    }
}

int AUADC_Int_Callback_Install(AUADC_INT_Type intType, intCallback_Type *cbFun)
{

    auadcIntCbfArra[intType] = cbFun;

    return SUCCESS;
}

int AUADC_GetIntStatus(AUADC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    int rlt = RESET;

    switch (intType) {
        case AUADC_INT_RX_FIFO_THR:
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            rlt = reg_is_bit_set(tmpVal, AUADC_RXA_INT);
            break;
        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            rlt = reg_is_bit_set(tmpVal, AUADC_RXO_INT);
            break;
        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
            rlt = reg_is_bit_set(tmpVal, AUADC_RXU_INT);
            break;

        default:
            break;
    }

    return rlt;
}

uint32_t AUADC_GetFifoCount(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_STATUS_OFFSET);
    tmpVal = reg_get_bits(tmpVal, AUADC_RXA_CNT);

    return tmpVal;
}

uint32_t AUADC_GetRawData(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_PDM_DAC_0_OFFSET);
    tmpVal = reg_get_bits(tmpVal, AUADC_ADC_0_SRC);

    if (tmpVal) {
        /* pdm interface */
        tmpVal = readl(AUADC_BASE + AUADC_AUDADC_RX_FIFO_DATA_OFFSET);
    } else {
        /*analog interface */
        tmpVal = readl(AUADC_BASE + AUADC_AUDADC_DATA_OFFSET);
        tmpVal = reg_get_bits(tmpVal, AUADC_AUDADC_RAW_DATA);
    }

    return tmpVal;
}

int AUADC_FifoDataReady(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_DATA_OFFSET);
    tmpVal = reg_get_bits(tmpVal, AUADC_AUDADC_DATA_RDY);

    return tmpVal;
}

void AUADC_ADC_Config(AUADC_ADC_AnalogCfg_Type *adc_cfg)
{
    uint32_t tmpVal = 0;
    uint8_t ch_en = 0;

    if (adc_cfg == NULL) {
        return;
    }

    /* audadc_ana_cfg1 */
    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_ANA_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_SEL_EDGE, adc_cfg->adc_edge_mode);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_CKB_EN, adc_cfg->adc_clock_phase_invert_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_LP_EN, adc_cfg->adc_clock_phase_invert_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_ICTRL_PGA_MIC, adc_cfg->pga_opmic_bias_cur);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_ICTRL_PGA_AAF, adc_cfg->pga_opaaf_bias_cur);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_RHPAS_SEL, adc_cfg->pga_hf_res);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_CHOP_CFG, adc_cfg->pga_chopper);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_CHOP_EN, adc_cfg->pga_chopper_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_CHOP_FREQ, adc_cfg->pga_chopper_freq);
    // tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_CHOP_CKSEL, adc_cfg->pga_chopper_clk_source);
    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_ANA_CFG1_OFFSET);

    /* audadc_ana_cfg2 */
    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_ANA_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_SDM_LP_EN, adc_cfg->sdm_lowpower_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_ICTRL_ADC, adc_cfg->sdm_bias_cur);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_NCTRL_ADC1, adc_cfg->sdm_i_first_num);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_NCTRL_ADC2, adc_cfg->sdm_i_sec_num);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_DEM_EN, adc_cfg->dem_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_QUAN_GAIN, adc_cfg->sdm_qg);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_DITHER_ENA, adc_cfg->sdm_dither_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_DITHER_SEL, adc_cfg->sdm_dither_level);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_DITHER_ORDER, adc_cfg->sdm_dither_order);
    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_ANA_CFG2_OFFSET);

    /* audadc_cmd */
    tmpVal = readl(AUADC_BASE + AUADC_AUDADC_CMD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_PU, adc_cfg->pga_circuit_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_SDM_PU, adc_cfg->sdm_circuit_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_CONV, 0);
    if (adc_cfg->pga_positive_en) {
        ch_en |= (0x01 << 1);
    }
    if (adc_cfg->pga_negative_en) {
        ch_en |= (0x01 << 0);
    }
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_CHANNEL_EN, ch_en);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_CHANNEL_SELP, adc_cfg->pga_posi_ch);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_CHANNEL_SELN, adc_cfg->pga_nega_ch);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_MODE, adc_cfg->pga_coupled_mode);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_PGA_GAIN, adc_cfg->pga_gain);
    if (adc_cfg->adc_mode == AUADC_ADC_FILT_MODE_AUDIO) {
        // tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_AUDIO_FILTER_EN, 1);
        tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_MEAS_FILTER_EN, 0);
    } else if (adc_cfg->adc_mode == AUADC_ADC_FILT_MODE_MEASURE) {
        // tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_AUDIO_FILTER_EN, 0);
        tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_MEAS_FILTER_EN, 1);
    }
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_MEAS_FILTER_TYPE, adc_cfg->measure_filter_mode);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_MEAS_ODR_SEL, adc_cfg->measure_rate);
    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_CMD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AUADC_AUDADC_CONV, 1);
    writel(tmpVal, AUADC_BASE + AUADC_AUDADC_CMD_OFFSET);
}

#ifndef IPRO_USE_HAL_DRIVER
void AUADC_IRQHandler(void)
{
    uint8_t intIndex = 0;

    for (intIndex = 0; intIndex < AUADC_INT_NUM_ALL; intIndex++) {
        if (AUADC_GetIntStatus(intIndex) == SET) {
            if (auadcIntCbfArra[intIndex] != NULL) {
                auadcIntCbfArra[intIndex]();
            }
            AUADC_IntClear(intIndex);
        }
    }
}
#endif

