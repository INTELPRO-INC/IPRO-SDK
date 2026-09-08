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

#include "hal_adc.h"
#include "hal_clock.h"
#include "hal_dma.h"
#include "drv_glb.h"
#include "drv_dma.h"
#include "drv_adc.h"
#include "adc_config.h"
#include "system_soc.h"

static void ADC_IRQ(void);

static adc_device_t adc_devices[ADC_MAX_INDEX] = {
    ADC0_CONFIG,
};

adc_device_t* hal_adc_getDevice(int index)
{
    return &adc_devices[index];
}

void hal_adc_setConfig(adc_device_t *dev)
{
    memcpy(&adc_devices[dev->id], dev, sizeof(adc_device_t));
}

uint8_t hal_adc_check_channel_status(uint8_t *pos_list, uint8_t *neg_list, uint16_t channelNum)
{
    uint16_t i = 0;

    uint8_t channel_io_reference_table[] = {
        GLB_GPIO_PIN_8,  /* CH0 IO */
        GLB_GPIO_PIN_15, /* CH1 IO */
        GLB_GPIO_PIN_17, /* CH2 IO */
        GLB_GPIO_PIN_11, /* CH3 IO */
        GLB_GPIO_PIN_12, /* CH4 IO */
        GLB_GPIO_PIN_14, /* CH5 IO */
        GLB_GPIO_PIN_7,  /* CH6 IO */
        GLB_GPIO_PIN_9,  /* CH7 IO */
        GLB_GPIO_PIN_18, /* CH8 IO */
        GLB_GPIO_PIN_19, /* CH9 IO */
        GLB_GPIO_PIN_20, /* CH10 IO */
        GLB_GPIO_PIN_21, /* CH11 IO */
    };

    for (i = 0; i < channelNum; i++) {
        if (pos_list[i] > ADC_CHANNEL11) {
            continue;
        }

        if (GLB_GPIO_Get_Fun(channel_io_reference_table[pos_list[i]]) != GPIO_FUN_ANALOG) {
            return -1;
        }
    }

    return 0;
}

int hal_adc_init(int adc_id)
{
    ADC_CFG_Type adc_cfg = { 0 };
    ADC_FIFO_Cfg_Type adc_fifo_cfg = { 0 };
    adc_device_t *adc_device = &adc_devices[adc_id];

    CPU_Interrupt_Disable(GPADC_DMA_IRQn);
    ADC_IntMask(ADC_INT_ALL, MASK);

    GLB_Set_ADC_CLK(1, GLB_ADC_CLK_XCLK, 0);

    adc_device->rx_dma = -1;
    adc_device->id = adc_id;

    adc_cfg.clkDiv = adc_device->clk_div;
    adc_cfg.vref = adc_device->vref;
    adc_cfg.resWidth = adc_device->data_width;
    adc_cfg.inputMode = adc_device->differential_mode;

    adc_cfg.v18Sel = ADC_V18_SELECT;
    adc_cfg.v11Sel = ADC_V11_SELECT;
    adc_cfg.gain1 = ADC_PGA_GAIN1;
    adc_cfg.gain2 = ADC_PGA_GAIN2;
    adc_cfg.chopMode = ADC_CHOP_MODE;
    adc_cfg.biasSel = ADC_BIAS_SELECT;
    adc_cfg.vcm = ADC_PGA_VCM;
    adc_cfg.offsetCalibEn = ADC_OFFSET_CALIB_EN;
    adc_cfg.offsetCalibVal = ADC_OFFSER_CALIB_VAL;

    adc_fifo_cfg.dmaEn = DISABLE;
    adc_fifo_cfg.fifoThreshold = adc_device->fifo_threshold;

    if ((adc_device->caps & COMMON_CAPS_INT_TX) || (adc_device->caps & COMMON_CAPS_INT_RX)) {
        Interrupt_Handler_Register(GPADC_DMA_IRQn, ADC_IRQ);
        CPU_Interrupt_Enable(GPADC_DMA_IRQn);
    }

    if (adc_device->caps & COMMON_CAPS_DMA_RX) {
        adc_fifo_cfg.dmaEn = ENABLE;
    }

    ADC_Disable();
    ADC_Enable();

    ADC_Reset();

    ADC_Init(&adc_cfg);

    ADC_FIFO_Cfg(&adc_fifo_cfg);
    ADC_IntClr(ADC_INT_ADC_READY);
    return 0;
}

int hal_adc_deinit(int adc_id)
{
    //adc_device_t *adc_device = &adc_devices[adc_id];

    ADC_IntMask(ADC_INT_ALL, MASK);
    ADC_Stop();
    ADC_Disable();
    ADC_Vbat_Disable();
    ADC_Reset();

    return 0;
}

int hal_adc_ioctl(int adc_id, int cmd, void *args)
{
    adc_device_t *adc_device = &adc_devices[adc_id];
    adc_channel_cfg_t *adc_channel_cfg = (adc_channel_cfg_t *)args;
    uint8_t rlt = 0;

    switch (cmd) {
        case ADC_CTRL_SET_INT: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while ((2 <= offset) && (offset < 6)) {
                if ((uint32_t)args & (1 << offset)) {
                    ADC_IntMask(offset, UNMASK);
                }
                offset++;
            }
            CPU_Interrupt_Enable(GPADC_DMA_IRQn);

            break;
        }

        case ADC_CTRL_CLEAR_INT: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while ((2 <= offset) && (offset < 6)) {
                if ((uint32_t)args & (1 << offset)) {
                    ADC_IntMask(offset, UNMASK);
                }
                offset++;
            }
            CPU_Interrupt_Disable(GPADC_DMA_IRQn);

            break;
        }

        case ADC_CTRL_CHANNEL_CONFIG:
            if (adc_channel_cfg->num == 1) {
                ADC_Channel_Config(adc_channel_cfg->pos_channel[0], adc_channel_cfg->neg_channel[0], adc_device->continuous_conv_mode);
                rlt = hal_adc_check_channel_status(adc_channel_cfg->pos_channel, adc_channel_cfg->neg_channel, 1);
            } else {
                ADC_Scan_Channel_Config(adc_channel_cfg->pos_channel, adc_channel_cfg->neg_channel, adc_channel_cfg->num, adc_device->continuous_conv_mode);
                rlt = hal_adc_check_channel_status(adc_channel_cfg->pos_channel, adc_channel_cfg->neg_channel, adc_channel_cfg->num);
            }

            break;

        case ADC_CTRL_CHANNEL_START:
            ADC_Start();
            break;

        case ADC_CTRL_CHANNEL_STOP:
            ADC_Stop();
            break;
        
        case ADC_CTRL_VBAT_ON:
            ADC_Vbat_Enable();
            break;

        case ADC_CTRL_VBAT_OFF:
            ADC_Vbat_Disable();
            break;

        case ADC_CTRL_TSEN_ON:
            ADC_Tsen_Init(ADC_TSEN_MOD_INTERNAL_DIODE);
            break;

        case ADC_CTRL_TSEN_OFF:
            break;
        case ADC_CTRL_ATTACH_RX_DMA:
            adc_device->rx_dma = (int) args;
            break;
        case ADC_CTRL_DATA_PARSE: {
            adc_data_parse_t *parse = (adc_data_parse_t *)args;
            ADC_Parse_Result(parse->input, parse->num, (ADC_Result_Type *)parse->output);
        } break;
        case ADC_CTRL_ADD_INT_CB:
            adc_device->callback = args;
        default:
            break;
    }

    return rlt;
}

int hal_adc_read(int adc_id, void *buffer, uint32_t size)
{
    adc_device_t *adc_device = &adc_devices[adc_id];
    uint32_t adc_fifo_val[32];
    int ret = -1;

    if (adc_device->caps & COMMON_CAPS_DMA_RX) {
        int dma_ch = adc_device->rx_dma;
        if (dma_ch < 0)
            return -1;

        ret = dma_reload(DMA0_ID, dma_ch, (uint32_t)DMA_ADDR_ADC_RDR, (uint32_t)buffer, size, NULL);
        dma_channel_start(DMA0_ID, dma_ch);

        return ret;
    } else {
        if (size > 32)
            return -1;
        while (ADC_Get_FIFO_Count() < size) {
        }

        for (uint32_t i = 0; i < size; i++) {
            adc_fifo_val[i] = ADC_Read_FIFO();
        }
        adc_channel_val_t *adc_parse_val = (adc_channel_val_t *)buffer;
        ADC_Parse_Result(adc_fifo_val, size, (ADC_Result_Type *)adc_parse_val);
        return size;
    }
    return ret;
}

int hal_adc_trim_tsen(uint16_t *tsen_offset)
{
    return ADC_Trim_TSEN(tsen_offset);
}

float hal_adc_get_tsen(uint16_t tsen_offset)
{
    return TSEN_Get_Temp(tsen_offset);
}

void hal_adc_isr(int adc_id)
{
    adc_device_t *adc_device = &adc_devices[adc_id];

    if (ADC_GetIntStatus(ADC_INT_POS_SATURATION) == SET && ADC_IntGetMask(ADC_INT_POS_SATURATION) == UNMASK) {
        ADC_IntClr(ADC_INT_POS_SATURATION);
    }

    if (ADC_GetIntStatus(ADC_INT_NEG_SATURATION) == SET && ADC_IntGetMask(ADC_INT_NEG_SATURATION) == UNMASK) {
        ADC_IntClr(ADC_INT_NEG_SATURATION);
    }

    if (ADC_GetIntStatus(ADC_INT_FIFO_UNDERRUN) == SET && ADC_IntGetMask(ADC_INT_FIFO_UNDERRUN) == UNMASK) {
        ADC_IntClr(ADC_INT_FIFO_UNDERRUN);
        if (adc_device->callback)
            adc_device->callback(adc_device, NULL, 0, ADC_EVENT_UNDERRUN);
    }

    if (ADC_GetIntStatus(ADC_INT_FIFO_OVERRUN) == SET && ADC_IntGetMask(ADC_INT_FIFO_OVERRUN) == UNMASK) {
        ADC_IntClr(ADC_INT_FIFO_OVERRUN);
        if (adc_device->callback)
            adc_device->callback(adc_device, NULL, 0, ADC_EVENT_OVERRUN);
    }

    if (ADC_GetIntStatus(ADC_INT_FIFO_READY) == SET && ADC_IntGetMask(ADC_INT_FIFO_READY) == UNMASK) {
        //ADC_IntClr(ADC_INT_FIFO_READY);
        uint32_t adc_count = ADC_Get_FIFO_Count();
        uint32_t adc_fifo_val[32];
        adc_channel_val_t adc_parse_val[32];
        for (uint32_t i = 0; i < adc_count; i++) {
            adc_fifo_val[i] = ADC_Read_FIFO();
        }
        ADC_Parse_Result(adc_fifo_val, adc_count, (ADC_Result_Type *)adc_parse_val);
        if (adc_device->callback)
            adc_device->callback(adc_device, (void *)adc_parse_val, adc_count, ADC_EVENT_FIFO);
    }
}

void ADC_IRQ(void)
{
    hal_adc_isr(ADC0_INDEX);
}

