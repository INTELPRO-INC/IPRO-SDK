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



#include "drv_clock.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_pds.h"
#include "drv_glb_gpio.h"

#define GLB_GPIO_INT0_NUM           (GLB_GPIO_PIN_MAX)
#define GLB_GPIO_INT0_CLEAR_TIMEOUT (32)

#define GLB_GPIO_TIMEOUT_COUNT (160 * 1000)

#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *ATTR_TCM_CONST_SECTION glbGpioInt0CbfArra[GLB_GPIO_INT0_NUM] = { NULL };

static intCallback_Type *ATTR_TCM_CONST_SECTION glbGpioFifoCbfArra[GLB_GPIO_FIFO_INT_ALL] = { NULL };
#endif

int ATTR_TCM_SECTION GLB_GPIO_Pad_LeadOut_Sts(uint8_t gpioPin)
{
    uint8_t package_type;

    package_type = GLB_Get_Package_Type();

    if ( GLB_PACKAGE_TYPE_QFN56 == package_type ){
        return SET;
    }

    /* */
    if ((( gpioPin >= GLB_GPIO_PIN_0) && (gpioPin < GLB_GPIO_PIN_4))
    || (( gpioPin > GLB_GPIO_PIN_9) && (gpioPin < GLB_GPIO_PIN_18))
    || (( gpioPin > GLB_GPIO_PIN_19) && (gpioPin < GLB_GPIO_PIN_23))
    || (( gpioPin > GLB_GPIO_PIN_26) && (gpioPin < GLB_GPIO_PIN_31))) {
        return SET;
    }

    return RESET;
}

int ATTR_TCM_SECTION GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg)
{
    uint8_t gpioPin = cfg->gpioPin;
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    if (RESET == GLB_GPIO_Pad_LeadOut_Sts(gpioPin)) {
        return ERROR;
    }

    /* drive strength(drive) = 0  <=>  8.0mA  @ 3.3V */
    /* drive strength(drive) = 1  <=>  9.6mA  @ 3.3V */
    /* drive strength(drive) = 2  <=>  11.2mA @ 3.3V */
    /* drive strength(drive) = 3  <=>  12.8mA @ 3.3V */

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    /* Disable output anyway*/
    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_OE);
    writel(tmpVal, gpioCfgAddress);

    /* input/output, pull up/down, drive, smt, function */
    tmpVal = readl(gpioCfgAddress);

    if (cfg->gpioMode != GPIO_MODE_ANALOG) {
        /* not analog mode */

        if (cfg->gpioMode == GPIO_MODE_OUTPUT) {
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_IE);
            tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_OE);
        } else {
            tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_IE);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_OE);
        }

        if (cfg->pullType == GPIO_PULL_UP) {
            tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_PU);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PD);
        } else if (cfg->pullType == GPIO_PULL_DOWN) {
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PU);
            tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_PD);
        } else {
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PU);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PD);
        }
    } else {
        /* analog mode */

        /* clear ie && oe */
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_IE);
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_OE);

        /* clear pu && pd */
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PU);
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PD);
    }

    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_DRV, cfg->drive);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_SMT, cfg->smtCtrl);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_FUNC_SEL, cfg->gpioFun);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_MODE, cfg->outputMode);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int GLB_GPIO_Func_Init(uint8_t gpioFun, uint8_t *pinList, uint8_t cnt)
{
    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = (uint8_t)gpioFun,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive = 1,
        .smtCtrl = 1
    };

    if (gpioFun == GPIO_FUN_ANALOG) {
        gpioCfg.gpioMode = GPIO_MODE_ANALOG;
    }

    for (uint8_t i = 0; i < cnt; i++) {
        gpioCfg.gpioPin = pinList[i];
        GLB_GPIO_Init(&gpioCfg);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_GPIO_Input_Enable(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_IE);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_Embedded_Flash_Pad_Enable(uint8_t swapIo2Cs)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;
    uint8_t i;

    /* all 6 embedded flash pads (GPIO35-40) need IE enabled */
    for (i = 35; i <= 40; i++) {
        gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (i << 2);
        tmpVal = readl(gpioCfgAddress);
        tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_IE);
        writel(tmpVal, gpioCfgAddress);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_GPIO_Input_Disable(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_IE);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_GPIO_Output_Enable(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    if (RESET == GLB_GPIO_Pad_LeadOut_Sts(gpioPin)) {
        return ERROR;
    }

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_OE);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_GPIO_Output_Disable(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_OE);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_GPIO_Set_HZ(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    if (RESET == GLB_GPIO_Pad_LeadOut_Sts(gpioPin)) {
        return ERROR;
    }

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    /* Disable output anyway*/
    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_OE);
    writel(tmpVal, gpioCfgAddress);

    /* ie=0, oe=0, drive=0, smt=0, pu=1 (pull up), pd=0, func=swgpio */
    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_IE);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_OE);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_DRV, 0);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_SMT, 0);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_PU); /* pull up */
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PD);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_FUNC_SEL, 0xB);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

uint8_t ATTR_TCM_SECTION GLB_GPIO_Get_Fun(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);
    tmpVal = readl(gpioCfgAddress);

    return reg_get_bits(tmpVal, GLB_REG_GPIO_0_FUNC_SEL);
}

uint32_t GLB_GPIO_Read(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);
    tmpVal = readl(gpioCfgAddress);

    return reg_get_bits(tmpVal, GLB_REG_GPIO_0_I) ? SET : RESET;
}

int GLB_GPIO_Write(uint8_t gpioPin, uint32_t val)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    if (RESET == GLB_GPIO_Pad_LeadOut_Sts(gpioPin)) {
        return ERROR;
    }

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);
    tmpVal = readl(gpioCfgAddress);

    if (val) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_O);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_O);
    }

    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int GLB_GPIO_Set(uint8_t gpioPin)
{
    if (RESET == GLB_GPIO_Pad_LeadOut_Sts(gpioPin)) {
        return ERROR;
    }

    if (gpioPin < GLB_GPIO_PIN_32) {
        writel(1 << gpioPin, GLB_BASE + GLB_GPIO_CFG138_OFFSET);
    } else {
        writel(1 << (gpioPin - GLB_GPIO_PIN_32), GLB_BASE + GLB_GPIO_CFG139_OFFSET);
    }

    return SUCCESS;
}

int GLB_GPIO_Clr(uint8_t gpioPin)
{
    if (gpioPin < GLB_GPIO_PIN_32) {
        writel(1 << gpioPin, GLB_BASE + GLB_GPIO_CFG140_OFFSET);
    } else {
        writel(1 << (gpioPin - GLB_GPIO_PIN_32), GLB_BASE + GLB_GPIO_CFG141_OFFSET);
    }

    return SUCCESS;
}

int GLB_GPIO_Toggle(int gpioPin)
{
    int val = GLB_GPIO_Read(gpioPin);
    GLB_GPIO_Write(gpioPin, !val);

    return SUCCESS;
}

int GLB_GPIO_Int_Init(GLB_GPIO_INT_Cfg_Type *intCfg)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;
    uint32_t gpioPin = intCfg->gpioPin;

    if (RESET == GLB_GPIO_Pad_LeadOut_Sts(gpioPin)) {
        return ERROR;
    }

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);
    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_INT_MODE_SET, intCfg->trig);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_INT_MASK, intCfg->intMask);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int GLB_Set_GPIO_IntMod(int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod, GLB_GPIO_INT_TRIG_Type intTrgMod)
{
    GLB_GPIO_INT_Cfg_Type intCfg;

    intCfg.gpioPin = gpioPin;
    intCfg.intMask = UNMASK;
    intCfg.trig = intTrgMod | (intCtlMod << 3);

    GLB_GPIO_Int_Init(&intCfg);

    return SUCCESS;
}

int GLB_GPIO_IntMask(uint8_t gpioPin, int intMask)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);
    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_INT_MASK, intMask);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int GLB_Get_GPIO_IntStatus(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    return reg_get_bits(readl(gpioCfgAddress), GLB_GPIO_0_INT_STAT) ? SET : RESET;
}

int GLB_Clr_GPIO_IntStatus(uint8_t gpioPin)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    /* clr=1 */
    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_INT_CLR);
    writel(tmpVal, gpioCfgAddress);

    /* clr=0 */
    tmpVal = readl(gpioCfgAddress);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_INT_CLR);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

int GLB_GPIO_IntClear(int gpioPin, int intClear)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;

    gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin << 2);

    /* clr=1 */
    tmpVal = readl(gpioCfgAddress);
    if (intClear == SET)
        tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_INT_CLR);
    else
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_INT_CLR);
    writel(tmpVal, gpioCfgAddress);

    return SUCCESS;
}

#ifndef IPRO_USE_HAL_DRIVER
void GPIO_FIFO_IRQHandler(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFG143_OFFSET);

    if (reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_END_INT) && !reg_is_bit_set(tmpVal, GLB_CR_GPIO_TX_END_MASK)) {
        if (glbGpioFifoCbfArra[GLB_GPIO_FIFO_INT_END] != NULL) {
            glbGpioFifoCbfArra[GLB_GPIO_FIFO_INT_END]();
        }
    }

    if (reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_FER_INT) && !reg_is_bit_set(tmpVal, GLB_CR_GPIO_TX_FER_MASK)) {
        if (glbGpioFifoCbfArra[GLB_GPIO_FIFO_INT_FER] != NULL) {
            glbGpioFifoCbfArra[GLB_GPIO_FIFO_INT_FER]();
        }
    }

    if (reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_FIFO_INT) && !reg_is_bit_set(tmpVal, GLB_CR_GPIO_TX_FIFO_MASK)) {
        if (glbGpioFifoCbfArra[GLB_GPIO_FIFO_INT_FIFO] != NULL) {
            glbGpioFifoCbfArra[GLB_GPIO_FIFO_INT_FIFO]();
        }
    }
}
#endif

int GLB_GPIO_INT0_IRQHandler_Install(void)
{
#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(GPIO_INT0_IRQn, GPIO_INT0_IRQHandler);
#endif

    return SUCCESS;
}

int GLB_GPIO_FIFO_IRQHandler_Install(void)
{
#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(GPIO_DMA_IRQn, GPIO_FIFO_IRQHandler);
#endif

    return SUCCESS;
}

#ifndef IPRO_USE_HAL_DRIVER
int GLB_GPIO_INT0_Callback_Install(uint8_t gpioPin, intCallback_Type *cbFun)
{
    if (gpioPin < GLB_GPIO_PIN_MAX) {
        glbGpioInt0CbfArra[gpioPin] = cbFun;
    }

    return SUCCESS;
}
#endif
#ifndef IPRO_USE_HAL_DRIVER
int GLB_GPIO_Fifo_Callback_Install(uint8_t intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    glbGpioFifoCbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void GPIO_INT0_IRQHandler(void)
{
    uint8_t gpioPin;
    uint32_t timeOut = 0;

    for (gpioPin = GLB_GPIO_PIN_0; gpioPin < GLB_GPIO_PIN_MAX; gpioPin++) {
        if (SET == GLB_Get_GPIO_IntStatus(gpioPin)) {
            GLB_Clr_GPIO_IntStatus(gpioPin);

            /* timeout check */
            timeOut = GLB_GPIO_INT0_CLEAR_TIMEOUT;

            do {
                timeOut--;
            } while ((SET == GLB_Get_GPIO_IntStatus(gpioPin)) && timeOut);

            if (!timeOut) {
                //MSG("WARNING: Clear GPIO interrupt status fail.\r\n");
            }

            if (glbGpioInt0CbfArra[gpioPin] != NULL) {
                /* Call the callback function */
                glbGpioInt0CbfArra[gpioPin]();
            }
        }
    }
}
#endif

int GLB_GPIO_Fifo_Init(GLB_GPIO_FIFO_CFG_Type *cfg)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFG143_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_GPIO_DMA_TX_EN, cfg->fifoDmaEnable);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_GPIO_TX_FIFO_TH, cfg->fifoDmaThreshold);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_GPIO_DMA_OUT_SEL_LATCH, cfg->latch);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_GPIO_DMA_PARK_VALUE, cfg->idle);
    tmpVal = reg_set_bit(tmpVal, GLB_GPIO_TX_FIFO_CLR);
    tmpVal = reg_clr_bit(tmpVal, GLB_GPIO_TX_END_CLR);
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFG143_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFG142_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_CODE0_HIGH_TIME, cfg->code0FirstTime);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_CODE1_HIGH_TIME, cfg->code1FirstTime);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_CODE_TOTAL_TIME, cfg->codeTotalTime);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_INVERT_CODE0_HIGH, cfg->code0Phase);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_INVERT_CODE1_HIGH, cfg->code1Phase);
    tmpVal = reg_set_bit(tmpVal, GLB_CR_GPIO_TX_EN);
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFG142_OFFSET);

    GLB_GPIO_FIFO_IRQHandler_Install();

    return SUCCESS;
}

int GLB_GPIO_Fifo_Push(uint16_t *data, uint16_t len)
{
    uint32_t txLen = 0;
    uint32_t timeoutCnt = GLB_GPIO_TIMEOUT_COUNT;

    while (txLen < len) {
        if (GLB_GPIO_Fifo_GetCount() > 0) {
            writel(data[txLen++], GLB_BASE + GLB_GPIO_CFG144_OFFSET);
            timeoutCnt = GLB_GPIO_TIMEOUT_COUNT;
        } else {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        }
    }

    return SUCCESS;
}

uint32_t GLB_GPIO_Fifo_GetCount(void)
{
    return reg_get_bits(readl(GLB_BASE + GLB_GPIO_CFG143_OFFSET), GLB_GPIO_TX_FIFO_CNT);
}

int GLB_GPIO_Fifo_Clear(void)
{
    writel(reg_set_bit(readl(GLB_BASE + GLB_GPIO_CFG143_OFFSET), GLB_GPIO_TX_FIFO_CLR), GLB_BASE + GLB_GPIO_CFG143_OFFSET);

    return SUCCESS;
}

int GLB_GPIO_Fifo_IntMask(uint8_t intType, int intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFG143_OFFSET);

    /* Mask or unmask certain or all interrupt */
    if (MASK == intMask) {
        switch (intType) {
            case GLB_GPIO_FIFO_INT_FER:
                tmpVal = reg_set_bit(tmpVal, GLB_CR_GPIO_TX_FER_MASK);
                break;

            case GLB_GPIO_FIFO_INT_FIFO:
                tmpVal = reg_set_bit(tmpVal, GLB_CR_GPIO_TX_FIFO_MASK);
                break;

            case GLB_GPIO_FIFO_INT_END:
                tmpVal = reg_set_bit(tmpVal, GLB_CR_GPIO_TX_END_MASK);
                break;

            case GLB_GPIO_FIFO_INT_ALL:
                tmpVal = reg_set_bit(tmpVal, GLB_CR_GPIO_TX_END_MASK);
                tmpVal = reg_set_bit(tmpVal, GLB_CR_GPIO_TX_FIFO_MASK);
                tmpVal = reg_set_bit(tmpVal, GLB_CR_GPIO_TX_FER_MASK);
                break;

            default:
                break;
        }
    } else {
        switch (intType) {
            case GLB_GPIO_FIFO_INT_FER:
                tmpVal = reg_clr_bit(tmpVal, GLB_CR_GPIO_TX_FER_MASK);
                break;

            case GLB_GPIO_FIFO_INT_FIFO:
                tmpVal = reg_clr_bit(tmpVal, GLB_CR_GPIO_TX_FIFO_MASK);
                break;

            case GLB_GPIO_FIFO_INT_END:
                tmpVal = reg_clr_bit(tmpVal, GLB_CR_GPIO_TX_END_MASK);
                break;

            case GLB_GPIO_FIFO_INT_ALL:
                tmpVal = reg_clr_bit(tmpVal, GLB_CR_GPIO_TX_END_MASK);
                tmpVal = reg_clr_bit(tmpVal, GLB_CR_GPIO_TX_FIFO_MASK);
                tmpVal = reg_clr_bit(tmpVal, GLB_CR_GPIO_TX_FER_MASK);
                break;

            default:
                break;
        }
    }

    /* Write back */
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFG143_OFFSET);

    return SUCCESS;
}

int GLB_GPIO_Fifo_IntClear(uint8_t intType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFG143_OFFSET);

    /* Clear certain or all interrupt */
    switch (intType) {
        case GLB_GPIO_FIFO_INT_FER:
            tmpVal = reg_set_bit(tmpVal, GLB_GPIO_TX_FIFO_CLR);
            break;

        case GLB_GPIO_FIFO_INT_FIFO:
            //tmpVal = reg_set_bit(tmpVal, GLB_GPIO_TX_FIFO_CLR);
            break;

        case GLB_GPIO_FIFO_INT_END:
            tmpVal = reg_set_bit(tmpVal, GLB_GPIO_TX_END_CLR);
            break;

        case GLB_GPIO_FIFO_INT_ALL:
            tmpVal = reg_set_bit(tmpVal, GLB_GPIO_TX_FIFO_CLR);
            tmpVal = reg_set_bit(tmpVal, GLB_GPIO_TX_END_CLR);
            break;

        default:
            break;
    }

    /* Write back */
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFG143_OFFSET);

    return SUCCESS;
}

int GLB_GPIO_Fifo_GetIntStatus(uint8_t intType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    /* Get certain or all interrupt status */
    tmpVal = readl(GLB_BASE + GLB_GPIO_CFG143_OFFSET);

    if (GLB_GPIO_FIFO_INT_ALL == intType) {
        if (reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_END_INT) ||
            reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_FIFO_INT) ||
            reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_FER_INT)) {
            return SET;
        } else {
            return RESET;
        }
    } else {
        switch (intType) {
            case GLB_GPIO_FIFO_INT_FER:
                return reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_FER_INT);

            case GLB_GPIO_FIFO_INT_FIFO:
                return reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_FIFO_INT);

            case GLB_GPIO_FIFO_INT_END:
                return reg_is_bit_set(tmpVal, GLB_R_GPIO_TX_END_INT);

            default:
                return RESET;
        }
    }
}
int GLB_GPIO_Fifo_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFG142_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_CR_GPIO_TX_EN);
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFG142_OFFSET);

    return SUCCESS;
}
int GLB_GPIO_Fifo_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFG142_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_CR_GPIO_TX_EN);
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFG142_OFFSET);

    return SUCCESS;
}

