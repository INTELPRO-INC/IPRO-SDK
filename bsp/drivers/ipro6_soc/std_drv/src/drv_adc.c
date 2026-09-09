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



#include "drv_adc.h"
#include "drv_ef_ctrl.h"
#include "drv_glb.h"

#undef MSG
#define MSG(...)
#define AON_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }
#define ADC_RESTART_DUMMY_WAIT arch_delay_us(100)

static intCallback_Type *adcIntCbfArra[ADC_INT_ALL] = { NULL };
static ADC_Gain_Coeff_Type adcGainCoeffCal = {
    .adcGainCoeffEnable = DISABLE,
    .adcgainCoeffVal = 0,
    .coe = 1,
};

void ADC_Reset(void)
{
    uint32_t regCmd;

    /* reset ADC */
    regCmd = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    writel(reg_set_bit(regCmd, AON_GPADC_SOFT_RST), AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    AON_CLK_SET_DUMMY_WAIT;
    writel(reg_clr_bit(regCmd, AON_GPADC_SOFT_RST), AON_BASE + AON_GPADC_REG_CMD_OFFSET);
}

void ADC_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_GLOBAL_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CMD_OFFSET);
}

void ADC_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_GLOBAL_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CMD_OFFSET);
}

void ADC_Init(ADC_CFG_Type *cfg)
{
    uint32_t regCfg1;
    uint32_t regCfg2;
    uint32_t regCalib;
    uint32_t regCmd;

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_GPIP);

    /* config 1 */
    regCfg1 = readl(AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);
    regCfg1 = reg_set_bits(regCfg1, AON_GPADC_V18_SEL, cfg->v18Sel);
    regCfg1 = reg_set_bits(regCfg1, AON_GPADC_V11_SEL, cfg->v11Sel);
    regCfg1 = reg_clr_bit(regCfg1, AON_GPADC_DITHER_EN);
    regCfg1 = reg_clr_bit(regCfg1, AON_GPADC_SCAN_EN);
    regCfg1 = reg_set_bits(regCfg1, AON_GPADC_SCAN_LENGTH, 0);
    regCfg1 = reg_set_bits(regCfg1, AON_GPADC_CLK_DIV_RATIO, cfg->clkDiv);
    regCfg1 = reg_clr_bit(regCfg1, AON_GPADC_CLK_ANA_INV);
    regCfg1 = reg_set_bits(regCfg1, AON_GPADC_CAL_OS_EN, cfg->offsetCalibEn);
    regCfg1 = reg_set_bits(regCfg1, AON_GPADC_RES_SEL, cfg->resWidth);
    writel(regCfg1, AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);
    AON_CLK_SET_DUMMY_WAIT;

    /* config 2 */
    regCfg2 = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_DLY_SEL, 0x02);
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_PGA1_GAIN, cfg->gain1);
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_PGA2_GAIN, cfg->gain2);
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_BIAS_SEL, cfg->biasSel);
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_CHOP_MODE, cfg->chopMode);
    /* pga_vcmi_en is for mic */
    regCfg2 = reg_clr_bit(regCfg2, AON_GPADC_PGA_VCMI_EN);

    if ((cfg->gain1 != ADC_PGA_GAIN_NONE) || (cfg->gain2 != ADC_PGA_GAIN_NONE)) {
        regCfg2 = reg_set_bit(regCfg2, AON_GPADC_PGA_EN);
    } else {
        regCfg2 = reg_clr_bit(regCfg2, AON_GPADC_PGA_EN);
    }

    /* pga_os_cal is for mic */
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_PGA_OS_CAL, 8);
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_PGA_VCM, cfg->vcm);
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_VREF_SEL, cfg->vref);
    regCfg2 = reg_set_bits(regCfg2, AON_GPADC_DIFF_MODE, cfg->inputMode);

    writel(regCfg2, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);

    /* mic2 diff enable */
    regCmd = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    regCmd = reg_set_bit(regCmd, AON_GPADC_MIC2_DIFF);
    if (cfg->inputMode == ADC_INPUT_SINGLE_END) {
        regCmd = reg_set_bit(regCmd, AON_GPADC_NEG_GND);
    } else {
        regCmd = reg_clr_bit(regCmd, AON_GPADC_NEG_GND);
    }
    writel(regCmd, AON_BASE + AON_GPADC_REG_CMD_OFFSET);

    /* calibration offset */
    regCalib = readl(AON_BASE + AON_GPADC_REG_DEFINE_OFFSET);
    regCalib = reg_set_bits(regCalib, AON_GPADC_OS_CAL_DATA, cfg->offsetCalibVal);
    writel(regCalib, AON_BASE + AON_GPADC_REG_DEFINE_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(GPADC_DMA_IRQn, GPADC_DMA_IRQHandler);
#endif

    ADC_Gain_Trim();
}

void ADC_Channel_Config(uint8_t posCh, uint8_t negCh, int contEn)
{
    uint32_t regCmd;
    uint32_t regCfg1;

    /* set channel */
    regCmd = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    regCmd = reg_set_bits(regCmd, AON_GPADC_POS_SEL, posCh);
    regCmd = reg_set_bits(regCmd, AON_GPADC_NEG_SEL, negCh);
    writel(regCmd, AON_BASE + AON_GPADC_REG_CMD_OFFSET);

    /* set continuous mode */
    regCfg1 = readl(AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);
    regCfg1 = reg_set_bits(regCfg1, AON_GPADC_CONT_CONV_EN, contEn);
    regCfg1 = reg_clr_bit(regCfg1, AON_GPADC_SCAN_EN);
    writel(regCfg1, AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);
}

void ADC_Scan_Channel_Config(uint8_t posChList[], uint8_t negChList[], uint8_t scanLength, int contEn)
{
    uint32_t tmpVal, i;
    uint32_t dealLen;

    /* Deal with the first 6 */
    dealLen = 6;

    if (scanLength < dealLen) {
        dealLen = scanLength;
    }

    /* Set first 6 scan channels */
    tmpVal = readl(AON_BASE + AON_GPADC_REG_SCN_POS1_OFFSET);

    for (i = 0; i < dealLen; i++) {
        tmpVal = tmpVal & (~(0x1F << (i * 5)));
        tmpVal |= (posChList[i] << (i * 5));
    }

    writel(tmpVal, AON_BASE + AON_GPADC_REG_SCN_POS1_OFFSET);

    tmpVal = readl(AON_BASE + AON_GPADC_REG_SCN_NEG1_OFFSET);

    for (i = 0; i < dealLen; i++) {
        tmpVal = tmpVal & (~(0x1F << (i * 5)));
        tmpVal |= (negChList[i] << (i * 5));
    }

    writel(tmpVal, AON_BASE + AON_GPADC_REG_SCN_NEG1_OFFSET);

    /* Set the left channels */
    if (scanLength > dealLen) {
        tmpVal = readl(AON_BASE + AON_GPADC_REG_SCN_POS2_OFFSET);

        for (i = 0; i < scanLength - dealLen; i++) {
            tmpVal = tmpVal & (~(0x1F << (i * 5)));
            tmpVal |= (posChList[i + dealLen] << (i * 5));
        }

        writel(tmpVal, AON_BASE + AON_GPADC_REG_SCN_POS2_OFFSET);

        tmpVal = readl(AON_BASE + AON_GPADC_REG_SCN_NEG2_OFFSET);

        for (i = 0; i < scanLength - dealLen; i++) {
            tmpVal = tmpVal & (~(0x1F << (i * 5)));
            tmpVal |= (negChList[i + dealLen] << (i * 5));
        }

        writel(tmpVal, AON_BASE + AON_GPADC_REG_SCN_NEG2_OFFSET);
    }

    /* Scan mode */
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_SCAN_LENGTH, scanLength - 1);
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_CONT_CONV_EN, contEn);
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_CLK_ANA_INV);
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_SCAN_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);
}

void ADC_Start(void)
{
    uint32_t regCmd;

    /* disable convert start */
    regCmd = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    regCmd = reg_clr_bit(regCmd, AON_GPADC_CONV_START);
    writel(regCmd, AON_BASE + AON_GPADC_REG_CMD_OFFSET);

    ADC_RESTART_DUMMY_WAIT;

    /* enable convert start */
    regCmd = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    regCmd = reg_set_bit(regCmd, AON_GPADC_CONV_START);
    writel(regCmd, AON_BASE + AON_GPADC_REG_CMD_OFFSET);
}

void ADC_Stop(void)
{
    uint32_t regCmd;

    /* disable convert start */
    regCmd = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    regCmd = reg_clr_bit(regCmd, AON_GPADC_CONV_START);
    writel(regCmd, AON_BASE + AON_GPADC_REG_CMD_OFFSET);
}

void ADC_FIFO_Cfg(ADC_FIFO_Cfg_Type *fifoCfg)
{
    uint32_t tmpVal;

    /* Check the parameters */

    /*
     *  DMA enable : ,When the fifo data is exceeded to fifoThreshold DMA request will occur
     *  DMA disable : fifoThreshold determine how many data will raise FIFO ready interrupt
     */

    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPADC_FIFO_THL, fifoCfg->fifoThreshold);

    /* Enable DMA */
    tmpVal = reg_set_bits(tmpVal, GPIP_GPADC_DMA_EN, fifoCfg->dmaEn);

    writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    /* clear fifo by SET GPIP_GPADC_FIFO_CLR bit*/
    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_CLR);
    writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
}

uint8_t ADC_Get_FIFO_Count(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    return reg_get_bits(tmpVal, GPIP_GPADC_FIFO_DATA_COUNT);
}

int ADC_FIFO_Is_Full(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    if (reg_is_bit_set(tmpVal, GPIP_GPADC_FIFO_FULL)) {
        return SET;
    } else {
        return RESET;
    }
}

int ADC_FIFO_Is_Empty(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    if (reg_is_bit_set(tmpVal, GPIP_GPADC_FIFO_NE)) {
        return RESET;
    } else {
        return SET;
    }
}

uint32_t ADC_Read_FIFO(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPADC_DMA_RDATA_OFFSET);

    return (tmpVal);
}

void ADC_Parse_Result(uint32_t *orgVal, uint32_t len, ADC_Result_Type *result)
{
    uint8_t neg = 0;
    uint32_t tmpVal1 = 0, tmpVal2 = 0;
    ADC_Data_Width_Type dataType;
    ADC_SIG_INPUT_Type sigType;
    uint32_t conv_result = 0;
    float ref = 2.0;
    uint32_t i = 0;

    float coe = 1.0;

    if (adcGainCoeffCal.adcGainCoeffEnable) {
        coe = adcGainCoeffCal.coe;
    }

    tmpVal1 = readl(AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);
    tmpVal2 = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    dataType = reg_get_bits(tmpVal1, AON_GPADC_RES_SEL);
    sigType = reg_get_bits(tmpVal2, AON_GPADC_DIFF_MODE);

    if (reg_get_bits(tmpVal2, AON_GPADC_VREF_SEL) == ADC_VREF_3P2V) {
        ref = 3.2;
    }

    if (sigType == ADC_INPUT_SINGLE_END) {
        for (i = 0; i < len; i++) {
            result[i].posChan = orgVal[i] >> 21;
            result[i].negChan = -1;

            if (dataType == ADC_DATA_WIDTH_12) {
                conv_result = (unsigned int)(((orgVal[i] & 0xffff) >> 4) / coe);
                if (conv_result > 4095) {
                    conv_result = 4095;
                }
                result[i].value = conv_result;
                result[i].volt = result[i].value / 4096.0 * ref;
            } else if ((dataType == ADC_DATA_WIDTH_14_WITH_16_AVERAGE) ||
                       (dataType == ADC_DATA_WIDTH_14_WITH_64_AVERAGE)) {
                conv_result = (unsigned int)(((orgVal[i] & 0xffff) >> 2) / coe);
                if (conv_result > 16383) {
                    conv_result = 16383;
                }
                result[i].value = conv_result;
                result[i].volt = result[i].value / 16384.0 * ref;
            } else if ((dataType == ADC_DATA_WIDTH_16_WITH_128_AVERAGE) ||
                       (dataType == ADC_DATA_WIDTH_16_WITH_256_AVERAGE)) {
                conv_result = (unsigned int)((orgVal[i] & 0xffff) / coe);
                if (conv_result > 65535) {
                    conv_result = 65535;
                }
                result[i].value = conv_result;
                result[i].volt = result[i].value / 65536.0 * ref;
            }
        }
    } else {
        for (i = 0; i < len; i++) {
            neg = 0;
            result[i].posChan = orgVal[i] >> 21;
            result[i].negChan = (orgVal[i] >> 16) & 0x1F;

            if (orgVal[i] & 0x8000) {
                orgVal[i] = ~orgVal[i];
                orgVal[i] += 1;
                neg = 1;
            }

            if (dataType == ADC_DATA_WIDTH_12) {
                conv_result = (unsigned int)(((orgVal[i] & 0xffff) >> 4) / coe);
                if (conv_result > 2047) {
                    conv_result = 2047;
                }
                result[i].value = conv_result;
                result[i].volt = result[i].value / 2048.0 * ref;
            } else if ((dataType == ADC_DATA_WIDTH_14_WITH_16_AVERAGE) ||
                       (dataType == ADC_DATA_WIDTH_14_WITH_64_AVERAGE)) {
                conv_result = (unsigned int)(((orgVal[i] & 0xffff) >> 2) / coe);
                if (conv_result > 8191) {
                    conv_result = 8191;
                }
                result[i].value = conv_result;
                result[i].volt = result[i].value / 8192.0 * ref;
            } else if ((dataType == ADC_DATA_WIDTH_16_WITH_128_AVERAGE) ||
                       (dataType == ADC_DATA_WIDTH_16_WITH_256_AVERAGE)) {
                conv_result = (unsigned int)((orgVal[i] & 0xffff) / coe);
                if (conv_result > 32767) {
                    conv_result = 32767;
                }
                result[i].value = conv_result;
                result[i].volt = result[i].value / 32768.0 * ref;
            }

            if (neg) {
                result[i].volt = -result[i].volt;
            }
        }
    }
}

int ADC_IntGetMask(ADC_INT_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    switch (intType) {
        case ADC_INT_POS_SATURATION:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            return reg_is_bit_set(tmpVal, AON_GPADC_POS_SATUR_MASK);
            break;

        case ADC_INT_NEG_SATURATION:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            return reg_is_bit_set(tmpVal, AON_GPADC_NEG_SATUR_MASK);
            break;

        case ADC_INT_FIFO_UNDERRUN:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            return reg_is_bit_set(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_MASK);
            break;

        case ADC_INT_FIFO_OVERRUN:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            return reg_is_bit_set(tmpVal, GPIP_GPADC_FIFO_OVERRUN_MASK);
            break;

        case ADC_INT_ADC_READY:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            return reg_is_bit_set(tmpVal, GPIP_GPADC_RDY_MASK);
            break;

        default:
            break;
    }
    return 0;
}

void ADC_IntMask(ADC_INT_Type intType, int intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */

    switch (intType) {
        case ADC_INT_POS_SATURATION:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, AON_GPADC_POS_SATUR_MASK);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_set_bit(tmpVal, AON_GPADC_POS_SATUR_MASK);
            }

            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            break;

        case ADC_INT_NEG_SATURATION:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, AON_GPADC_NEG_SATUR_MASK);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_set_bit(tmpVal, AON_GPADC_NEG_SATUR_MASK);
            }

            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            break;

        case ADC_INT_FIFO_UNDERRUN:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_MASK);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_MASK);
            }

            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            break;

        case ADC_INT_FIFO_OVERRUN:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_MASK);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_MASK);
            }

            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            break;

        case ADC_INT_ADC_READY:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_RDY_MASK);
            } else {
                /* Disable this interrupt */
                tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_RDY_MASK);
            }

            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            break;

        case ADC_INT_FIFO_READY:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_RDY);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
           
            tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_RDY);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            /*Manual reset*/
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_RDY);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            break;

        case ADC_INT_ALL:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
                tmpVal = reg_clr_bit(tmpVal, AON_GPADC_POS_SATUR_MASK);
                tmpVal = reg_clr_bit(tmpVal, AON_GPADC_NEG_SATUR_MASK);
                writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

                tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
                tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_MASK);
                tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_MASK);
                tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_RDY_MASK);
                writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            } else {
                /* Disable this interrupt */
                tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
                tmpVal = reg_set_bit(tmpVal, AON_GPADC_POS_SATUR_MASK);
                tmpVal = reg_set_bit(tmpVal, AON_GPADC_NEG_SATUR_MASK);
                writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

                tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
                tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_MASK);
                tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_MASK);
                tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_RDY_MASK);
                writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            }

            break;

        default:
            break;
    }
}

void ADC_IntClr(ADC_INT_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    switch (intType) {
        case ADC_INT_POS_SATURATION:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, AON_GPADC_POS_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            tmpVal = reg_set_bit(tmpVal, AON_GPADC_POS_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            /*Manual reset*/
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, AON_GPADC_POS_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            break;

        case ADC_INT_NEG_SATURATION:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, AON_GPADC_NEG_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            tmpVal = reg_set_bit(tmpVal, AON_GPADC_NEG_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            /*Manual reset*/
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, AON_GPADC_NEG_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            break;

        case ADC_INT_FIFO_UNDERRUN:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            /*Manual reset*/
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            break;

        case ADC_INT_FIFO_OVERRUN:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            /*Manual reset*/
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            break;

        case ADC_INT_ADC_READY:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_RDY_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_RDY_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            /*Manual reset*/
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_RDY_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            break;

        case ADC_INT_ALL:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, AON_GPADC_POS_SATUR_CLR);
            tmpVal = reg_clr_bit(tmpVal, AON_GPADC_NEG_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            tmpVal = reg_set_bit(tmpVal, AON_GPADC_POS_SATUR_CLR);
            tmpVal = reg_set_bit(tmpVal, AON_GPADC_NEG_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            /*Manual reset*/
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, AON_GPADC_POS_SATUR_CLR);
            tmpVal = reg_clr_bit(tmpVal, AON_GPADC_NEG_SATUR_CLR);
            writel(tmpVal, AON_BASE + AON_GPADC_REG_ISR_OFFSET);

            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_CLR);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_CLR);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_RDY_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_CLR);
            tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_CLR);
            tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_RDY_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            /*Manual reset*/
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_UNDERRUN_CLR);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_FIFO_OVERRUN_CLR);
            tmpVal = reg_clr_bit(tmpVal, GPIP_GPADC_RDY_CLR);
            writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

            break;

        default:
            break;
    }
}

int ADC_GetIntStatus(ADC_INT_Type intType)
{
    uint32_t tmpVal;
    int bitStatus = RESET;

    /* Check the parameters */

    switch (intType) {
        case ADC_INT_POS_SATURATION:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            bitStatus = (reg_is_bit_set(tmpVal, AON_GPADC_POS_SATUR)) ? SET : RESET;
            break;

        case ADC_INT_NEG_SATURATION:
            tmpVal = readl(AON_BASE + AON_GPADC_REG_ISR_OFFSET);
            bitStatus = (reg_is_bit_set(tmpVal, AON_GPADC_NEG_SATUR)) ? SET : RESET;
            break;

        case ADC_INT_FIFO_UNDERRUN:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            bitStatus = (reg_is_bit_set(tmpVal, GPIP_GPADC_FIFO_UNDERRUN)) ? SET : RESET;
            break;

        case ADC_INT_FIFO_OVERRUN:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            bitStatus = (reg_is_bit_set(tmpVal, GPIP_GPADC_FIFO_OVERRUN)) ? SET : RESET;
            break;

        case ADC_INT_ADC_READY:
            tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            bitStatus = (reg_is_bit_set(tmpVal, GPIP_GPADC_RDY)) ? SET : RESET;
            break;

        case ADC_INT_ALL:
            break;

        default:
            break;
    }

    return bitStatus;
}

void ADC_Int_Callback_Install(ADC_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    adcIntCbfArra[intType] = cbFun;
}

#ifndef IPRO_USE_HAL_DRIVER
void GPADC_DMA_IRQHandler(void)
{
    if (ADC_IntGetMask(ADC_INT_POS_SATURATION) == UNMASK && ADC_GetIntStatus(ADC_INT_POS_SATURATION) == SET) {
        ADC_IntClr(ADC_INT_POS_SATURATION);

        if (adcIntCbfArra[ADC_INT_POS_SATURATION] != NULL) {
            adcIntCbfArra[ADC_INT_POS_SATURATION]();
        }
    }

    if (ADC_IntGetMask(ADC_INT_NEG_SATURATION) == UNMASK && ADC_GetIntStatus(ADC_INT_NEG_SATURATION) == SET) {
        ADC_IntClr(ADC_INT_NEG_SATURATION);

        if (adcIntCbfArra[ADC_INT_NEG_SATURATION] != NULL) {
            adcIntCbfArra[ADC_INT_NEG_SATURATION]();
        }
    }

    if (ADC_IntGetMask(ADC_INT_FIFO_UNDERRUN) == UNMASK && ADC_GetIntStatus(ADC_INT_FIFO_UNDERRUN) == SET) {
        ADC_IntClr(ADC_INT_FIFO_UNDERRUN);

        if (adcIntCbfArra[ADC_INT_FIFO_UNDERRUN] != NULL) {
            adcIntCbfArra[ADC_INT_FIFO_UNDERRUN]();
        }
    }

    if (ADC_IntGetMask(ADC_INT_FIFO_OVERRUN) == UNMASK && ADC_GetIntStatus(ADC_INT_FIFO_OVERRUN) == SET) {
        ADC_IntClr(ADC_INT_FIFO_OVERRUN);

        if (adcIntCbfArra[ADC_INT_FIFO_OVERRUN] != NULL) {
            adcIntCbfArra[ADC_INT_FIFO_OVERRUN]();
        }
    }

    if (ADC_IntGetMask(ADC_INT_ADC_READY) == UNMASK && ADC_GetIntStatus(ADC_INT_ADC_READY) == SET) {
        ADC_IntClr(ADC_INT_ADC_READY);

        if (adcIntCbfArra[ADC_INT_ADC_READY] != NULL) {
            adcIntCbfArra[ADC_INT_ADC_READY]();
        }
    }
}
#endif

void ADC_Vbat_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_VBAT_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
}

void ADC_Vbat_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_VBAT_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
}

void ADC_Tsen_Init(ADC_TSEN_MOD_Type tsenMod)
{
    uint32_t tmpVal;

    /* config gpadc_reg_cmd */
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    /* enable sensor dc test mux*/
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_SEN_TEST_EN);
    /*selected sen output current channel*/
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_SEN_SEL, 0);
    /* enable chip sensor*/
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_CHIP_SEN_PU);
    /*dwa_en */
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_DWA_EN, 1);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CMD_OFFSET);

    /* config 2 */
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    /*tsvbe low=0*/
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_TSVBE_LOW);
    /*dly_sel=2*/
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_DLY_SEL, 2);
    /*test_sel=0*/
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_TEST_SEL, 0);
    /*test_en=0*/
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_TEST_EN);
    /*ts_en*/
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_TS_EN);
    /*select tsen ext or inner*/
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_TSEXT_SEL, tsenMod);
    /*chopper mode = 1*/
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_CHOP_MODE, 1);

    tmpVal = reg_set_bits(tmpVal, AON_GPADC_PGA_VCM, 1);
    /*pga vcmi enable*/
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_PGA_VCMI_EN);
    /*0:512uS;1:16mS;2:32mS;3:64mS*/
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_PGA_OS_CAL, 0);

    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);

    /* config 3 */
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);
    /* set gpadc_dither_en */
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_DITHER_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG1_OFFSET);

    /* set 4000F90C[19](gpadc_mic2_diff) = 1
     * debug advise form Ran
     * 2020.08.26
     */
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_GPADC_MIC2_DIFF, 1);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CMD_OFFSET);
}

void ADC_Tsen_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_TS_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
}

void ADC_Tsen_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_TS_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
}

void ADC_FIFO_Clear(void)
{
    uint32_t tmpVal;

    /* clear fifo by SET GPIP_GPADC_FIFO_CLR bit*/
    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_CLR);
    writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
}

void ADC_PGA_Config(uint8_t pga_vcmi_enable, uint8_t pga_os_cal)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);

    if (pga_vcmi_enable) {
        tmpVal = reg_set_bit(tmpVal, AON_GPADC_PGA_VCMI_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, AON_GPADC_PGA_VCMI_EN);
    }

    tmpVal = reg_set_bits(tmpVal, AON_GPADC_PGA_OS_CAL, pga_os_cal);

    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
}

uint32_t TSEN_Get_V_Error(void)
{
    uint32_t v0 = 0, v1 = 0;
    uint32_t v_error = 0;
    uint32_t regVal = 0;
    ADC_Result_Type result = { 0 };
    uint32_t tmpVal;
    uint8_t gainCalEnabled = 0;

    /* clear fifo by SET GPIP_GPADC_FIFO_CLR bit*/
    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_CLR);
    writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    ADC_SET_TSVBE_LOW();

    ADC_Start();

    while (ADC_Get_FIFO_Count() == 0)
        ;

    regVal = ADC_Read_FIFO();
    gainCalEnabled = adcGainCoeffCal.adcGainCoeffEnable;
    adcGainCoeffCal.adcGainCoeffEnable = 0;
    ADC_Parse_Result(&regVal, 1, &result);
    adcGainCoeffCal.adcGainCoeffEnable = gainCalEnabled;
    v0 = result.value;

    /* clear fifo by SET GPIP_GPADC_FIFO_CLR bit*/
    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_CLR);
    writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    ADC_SET_TSVBE_HIGH();

    ADC_Start();

    while (ADC_Get_FIFO_Count() == 0)
        ;

    regVal = ADC_Read_FIFO();
    gainCalEnabled = adcGainCoeffCal.adcGainCoeffEnable;
    adcGainCoeffCal.adcGainCoeffEnable = 0;
    ADC_Parse_Result(&regVal, 1, &result);
    adcGainCoeffCal.adcGainCoeffEnable = gainCalEnabled;
    v1 = result.value;

    v_error = v0 - v1;

    return v_error;
}

int ATTR_CLOCK_SECTION ADC_Trim_TSEN(uint16_t *tsen_offset)
{
    Efuse_TSEN_Refcode_Corner_Type trim;

    EF_Ctrl_Read_TSEN_Trim(&trim);
    if (trim.tsenRefcodeCornerEn) {
        if (trim.tsenRefcodeCornerParity == EF_Ctrl_Get_Trim_Parity(trim.tsenRefcodeCorner, 12)) {
            *tsen_offset = trim.tsenRefcodeCorner;

            return SUCCESS;
        }
    }

    return ERROR;
}

void ADC_SET_TSVBE_LOW(void)
{
    uint32_t tmpVal;
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_TSVBE_LOW);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
}

void ADC_SET_TSVBE_HIGH(void)
{
    uint32_t tmpVal;
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_TSVBE_LOW);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);
}

float TSEN_Get_Temp(uint32_t tsen_offset)
{
    uint32_t v0 = 0, v1 = 0;
    float temp = 0;
    uint32_t regVal = 0;
    ADC_Result_Type result = { 0 };
    uint32_t tmpVal;
    uint8_t gainCalEnabled = 0;

    /* clear fifo by SET GPIP_GPADC_FIFO_CLR bit*/
    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_CLR);
    writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    ADC_SET_TSVBE_LOW();

    ADC_Start();

    while (ADC_Get_FIFO_Count() == 0)
        ;

    regVal = ADC_Read_FIFO();

    gainCalEnabled = adcGainCoeffCal.adcGainCoeffEnable;
    adcGainCoeffCal.adcGainCoeffEnable = 0;
    ADC_Parse_Result(&regVal, 1, &result);
    adcGainCoeffCal.adcGainCoeffEnable = gainCalEnabled;
    v0 = result.value;

    /* clear fifo by SET GPIP_GPADC_FIFO_CLR bit*/
    tmpVal = readl(GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPADC_FIFO_CLR);
    writel(tmpVal, GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    ADC_SET_TSVBE_HIGH();

    ADC_Start();

    while (ADC_Get_FIFO_Count() == 0)
        ;

    regVal = ADC_Read_FIFO();
    gainCalEnabled = adcGainCoeffCal.adcGainCoeffEnable;
    adcGainCoeffCal.adcGainCoeffEnable = 0;
    ADC_Parse_Result(&regVal, 1, &result);
    adcGainCoeffCal.adcGainCoeffEnable = gainCalEnabled;
    v1 = result.value;

    if (v0 > v1) {
        temp = (((float)v0 - (float)v1) - (float)tsen_offset) / 7.753;
    } else {
        temp = (((float)v1 - (float)v0) - (float)tsen_offset) / 7.753;
    }

    return temp;
}

int ADC_Mic_Init(ADC_MIC_Type *adc_mic_config)
{
    uint32_t tmpVal1 = 0, tmpVal2 = 0;

    tmpVal2 = readl(AON_BASE + AON_GPADC_REG_CONFIG2_OFFSET);

    tmpVal1 = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    tmpVal1 = reg_set_bits(tmpVal1, AON_GPADC_MICBOOST_32DB_EN, adc_mic_config->micboostDb);
    tmpVal1 = reg_set_bits(tmpVal1, AON_GPADC_MIC_PGA2_GAIN, adc_mic_config->micPga2Gain);
    tmpVal1 = reg_set_bits(tmpVal1, AON_GPADC_MIC1_DIFF, adc_mic_config->mic1Mode);
    tmpVal1 = reg_set_bits(tmpVal1, AON_GPADC_MIC2_DIFF, adc_mic_config->mic2Mode);
    tmpVal1 = reg_set_bits(tmpVal1, AON_GPADC_DWA_EN, adc_mic_config->dwaEn);
    tmpVal1 = reg_set_bits(tmpVal1, AON_GPADC_BYP_MICBOOST, adc_mic_config->micboostBypassEn);

    if (reg_is_bit_set(tmpVal2, AON_GPADC_PGA_EN) && adc_mic_config->micPgaEn == ENABLE) {
        /* 0x4000F914[13] and 0x4000F90c[15] Cannot be both Enable*/
        return ERROR;
    } else {
        tmpVal1 = reg_set_bits(tmpVal1, AON_GPADC_MICPGA_EN, adc_mic_config->micPgaEn);
    }

    tmpVal1 = reg_set_bits(tmpVal1, AON_GPADC_MICBIAS_EN, adc_mic_config->micBiasEn);

    writel(tmpVal1, AON_BASE + AON_GPADC_REG_CMD_OFFSET);

    return SUCCESS;
}

void ADC_MIC_Bias_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_MICBIAS_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CMD_OFFSET);
}

void ADC_MIC_Bias_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_MICBIAS_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CMD_OFFSET);
}

int ATTR_CLOCK_SECTION ADC_Gain_Trim(void)
{
    Efuse_ADC_Gain_Coeff_Type trim;
    uint32_t tmp;

    EF_Ctrl_Read_ADC_Gain_Trim(&trim);

    if (trim.adcGainCoeffEn) {
        if (trim.adcGainCoeffParity == EF_Ctrl_Get_Trim_Parity(trim.adcGainCoeff, 12)) {
            adcGainCoeffCal.adcGainCoeffEnable = ENABLE;
            adcGainCoeffCal.adcgainCoeffVal = trim.adcGainCoeff;
            tmp = adcGainCoeffCal.adcgainCoeffVal;

            if (tmp & 0x800) {
                tmp = ~tmp;
                tmp += 1;
                tmp = tmp & 0xfff;
                //printf("val==%08x\r\n",(unsigned int)tmp);
                adcGainCoeffCal.coe = (1.0 + ((float)tmp / 2048.0));
                //printf("coe==%0f\r\n",adcGainCoeffCal.coe);
            } else {
                adcGainCoeffCal.coe = (1.0 - ((float)tmp / 2048.0));
                //printf("coe==%0f\r\n",adcGainCoeffCal.coe);
            }

            return SUCCESS;
        }
    }

    return ERROR;
}

