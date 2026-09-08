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

#include "drv_glb.h"
#include "drv_hbn.h"

#define GLB_GPIO_Get_Reg(pin)       (glb_gpio_reg_t *)(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + (pin / 2) * 4)
#define GLB_GPIO_INT0_NUM           (32)
#define GLB_GPIO_INT0_CLEAR_TIMEOUT (32)

int ATTR_TCM_SECTION GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg)
{
    uint8_t gpioPin = cfg->gpioPin;
    uint32_t *pOut;
    uint32_t pos;
    uint32_t tmpOut;
    uint32_t tmpVal;

    /* drive strength(drive) = 0  <=>  8.0mA  @ 3.3V */
    /* drive strength(drive) = 1  <=>  9.6mA  @ 3.3V */
    /* drive strength(drive) = 2  <=>  11.2mA @ 3.3V */
    /* drive strength(drive) = 3  <=>  12.8mA @ 3.3V */

    pOut = (uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL35_OFFSET - ((gpioPin >> 5) << 2));
    pos = gpioPin % 32;
    tmpOut = *pOut;

    /* Disable output anyway*/
    tmpOut &= (~(1 << pos));
    *pOut = tmpOut;

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + gpioPin / 2 * 4);
    if (gpioPin % 2 == 0) {
        if (cfg->gpioMode != GPIO_MODE_ANALOG) {
            /* not analog mode */

            /* Set input or output */
            if (cfg->gpioMode == GPIO_MODE_OUTPUT) {
                tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_IE);
                tmpOut |= (1 << pos);
            } else {
                tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_IE);
            }

            /* Set pull up or down */
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PU);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PD);
            if (cfg->pullType == GPIO_PULL_UP) {
                tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_PU);
            } else if (cfg->pullType == GPIO_PULL_DOWN) {
                tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_PD);
            }
        } else {
            /* analog mode */

            /* clear ie && oe */
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_IE);
            tmpOut &= ~(1 << pos);

            /* clear pu && pd */
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PU);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_PD);
        }

        /* set drive && smt && func */
        tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_DRV, cfg->drive);
        tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_SMT, cfg->smtCtrl);
        tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_FUNC_SEL, cfg->gpioFun);
    } else {
        if (cfg->gpioMode != GPIO_MODE_ANALOG) {
            /* not analog mode */

            /* Set input or output */
            if (cfg->gpioMode == GPIO_MODE_OUTPUT) {
                tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_1_IE);
                tmpOut |= (1 << pos);
            } else {
                tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_1_IE);
            }

            /* Set pull up or down */
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_1_PU);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_1_PD);
            if (cfg->pullType == GPIO_PULL_UP) {
                tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_1_PU);
            } else if (cfg->pullType == GPIO_PULL_DOWN) {
                tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_1_PD);
            }
        } else {
            /* analog mode */

            /* clear ie && oe */
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_1_IE);
            tmpOut &= ~(1 << pos);

            /* clear pu && pd */
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_1_PU);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_1_PD);
        }

        /* set drive && smt && func */
        tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_1_DRV, cfg->drive);
        tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_1_SMT, cfg->smtCtrl);
        tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_1_FUNC_SEL, cfg->gpioFun);
    }

     writel(tmpVal, GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + gpioPin / 2 * 4);

    *pOut = tmpOut;

    return SUCCESS;
}

int GLB_GPIO_Func_Init(GLB_GPIO_FUNC_Type gpioFun, uint8_t *pinList, uint8_t cnt)
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

int GLB_GPIO_OUTPUT_Mode_Set(int gpioPin, GLB_GPIO_OUTPUT_MODE_Type mode)
{
    uint32_t tmpVal;
    uint32_t pinOffset;

    pinOffset = (gpioPin >> 1) << 2;
    tmpVal = *(uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + pinOffset);
    if (gpioPin % 2 == 0) {
        tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_0_MODE, mode);
    } else {
        tmpVal = reg_set_bits(tmpVal, GLB_REG_GPIO_1_MODE, mode);
    }
    *(uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + pinOffset) = tmpVal;

    return SUCCESS;
}

int GLB_GPIO_INPUT_Enable(int gpioPin)
{
    uint32_t tmpVal;
    uint32_t pinOffset;
    uint8_t realPin;

    realPin = gpioPin;
    pinOffset = (realPin >> 1) << 2;
    tmpVal = *(uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + pinOffset);
    if (realPin % 2 == 0) {
        /* [0] is ie */
        tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_0_IE);
    } else {
        /* [16] is ie */
        tmpVal = reg_set_bit(tmpVal, GLB_REG_GPIO_1_IE);
    }
    *(uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + pinOffset) = tmpVal;

    return SUCCESS;
}

int GLB_GPIO_INPUT_Disable(int gpioPin)
{
    uint32_t tmpVal;
    uint32_t pinOffset;
    uint8_t realPin;

    realPin = gpioPin;
    pinOffset = (realPin >> 1) << 2;
    tmpVal = readl(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + pinOffset);
    if (realPin % 2 == 0) {
        /* [0] is ie */
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_0_IE);
    } else {
        /* [16] is ie */
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_GPIO_1_IE);
    }
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + pinOffset);
    return SUCCESS;
}

int GLB_GPIO_OUTPUT_Enable(int gpioPin)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFGCTL35_OFFSET - ((gpioPin >> 5) << 2));
    tmpVal = tmpVal | (1 << (gpioPin % 32));
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFGCTL35_OFFSET - ((gpioPin >> 5) << 2));

    return SUCCESS;
}

int GLB_GPIO_OUTPUT_Disable(int gpioPin)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFGCTL35_OFFSET - ((gpioPin >> 5) << 2));
    tmpVal = tmpVal & ~(1 << (gpioPin % 32));
    writel(tmpVal, GLB_BASE + GLB_GPIO_CFGCTL35_OFFSET - ((gpioPin >> 5) << 2));

    return SUCCESS;
}

ATTR_TCM_SECTION
int GLB_GPIO_Set_HZ(int gpioPin)
{
    uint32_t *pOut;
    uint32_t pos;
    uint32_t tmpOut;
    uint32_t tmpVal;
    uint8_t realPin;
#if 0
    uint32_t aonPadIeSmt;
    /* always on pads IE control (in HBN) */
    if (gpioPin >= 9 && gpioPin <= 13) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        aonPadIeSmt = reg_get_bits(tmpVal, HBN_REG_AON_PAD_IE_SMT);
        aonPadIeSmt &= ~(1 << (gpioPin - 9));
        tmpVal = reg_set_bits(tmpVal, HBN_REG_AON_PAD_IE_SMT, aonPadIeSmt);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    }
#endif
    realPin = gpioPin;

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + realPin / 2 * 4);

    /* pu=0, pd=0, ie=0 */
    if (realPin % 2 == 0) {
        tmpVal = (tmpVal & 0xffffff00);
    } else {
        tmpVal = (tmpVal & 0xff00ffff);
    }

    /* func_sel=swgpio */
    if (gpioPin % 2 == 0) {
        tmpVal = (tmpVal & 0xffff00ff);
        tmpVal |= 0x0B00;
    } else {
        tmpVal = (tmpVal & 0x00ffffff);
        tmpVal |= (0x0B00 << 16);
    }

    writel(tmpVal, GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + realPin / 2 * 4);

    pOut = (uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL35_OFFSET - ((gpioPin >> 5) << 2));
    pos = gpioPin % 32;
    tmpOut = *pOut;

    /* Disable output anyway*/
    tmpOut &= (~(1 << pos));
    *pOut = tmpOut;

    return SUCCESS;
}

ATTR_TCM_SECTION
int GLB_Set_Flash_Pad_HZ(void)
{
    for (int gpio = 44; gpio < 50; gpio++) {
        GLB_GPIO_Set_HZ(gpio);
    }
    return SUCCESS;
}

int GLB_Set_Psram_Pad_HZ(void)
{
    for (int gpio = 50; gpio < 62; gpio++) {
        GLB_GPIO_Set_HZ(gpio);
    }
    return SUCCESS;
}

uint8_t GLB_GPIO_Get_Fun(int gpioPin)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_GPIO_CFGCTL0_OFFSET + gpioPin / 2 * 4);

    if (gpioPin % 2 == 0) {
        return reg_get_bits(tmpVal, GLB_REG_GPIO_0_FUNC_SEL);
    } else {
        return reg_get_bits(tmpVal, GLB_REG_GPIO_1_FUNC_SEL);
    }
}

int GLB_GPIO_Write(int gpioPin, uint32_t val)
{
    uint32_t pos = gpioPin % 32;
    uint32_t tmpOut;

    if (gpioPin >= 32)
        pos = (gpioPin % 32) + 12;

    tmpOut = readl(GLB_BASE + GLB_GPIO_CFGCTL33_OFFSET + ((gpioPin >> 5) << 2));
    if (val > 0) {
        tmpOut |= (1 << pos);
    } else {
        tmpOut &= (~(1 << pos));
    }
    writel(tmpOut, GLB_BASE + GLB_GPIO_CFGCTL33_OFFSET + ((gpioPin >> 5) << 2));

    return SUCCESS;
}

int GLB_GPIO_Toggle(int gpioPin)
{
    uint32_t pos = gpioPin % 32;
    uint32_t tmpOut;

    if (gpioPin >= 32)
        pos = (gpioPin % 32) + 12;

    tmpOut = readl(GLB_BASE + GLB_GPIO_CFGCTL33_OFFSET + ((gpioPin >> 5) << 2));
    tmpOut ^= (1 << pos);
    writel(tmpOut, GLB_BASE + GLB_GPIO_CFGCTL33_OFFSET + ((gpioPin >> 5) << 2));

    return SUCCESS;
}


uint32_t GLB_GPIO_Read(int gpioPin)
{
    uint32_t val = readl(GLB_BASE + GLB_GPIO_CFGCTL31_OFFSET + ((gpioPin >> 5) << 2));
    uint32_t pos = gpioPin % 32;

    if (val & (1 << pos)) {
        return 1;
    } else {
        return 0;
    }
}

int GLB_GPIO_IntMask(int gpioPin, int intMask)
{
    uint32_t tmpVal;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MASK1_OFFSET);
        if (intMask == MASK) {
            tmpVal = tmpVal | (1 << gpioPin);
        } else {
            tmpVal = tmpVal & ~(1 << gpioPin);
        }
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT_MASK1_OFFSET);
    } else if (gpioPin <= 43) {
        /* GPIO32 ~ GPIO43 — per vault glb.md: MASK lives in INT_MASK2
         * @ 0x1A4 bits[11:0] for INT0 line. Default value is 0
         * (UNMASKED) unlike MASK1 whose default is 1, so failing to
         * actually write here would leave the pin permanently active. */
        uint32_t bit = gpioPin - 32;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MASK2_OFFSET);
        if (intMask == MASK) {
            tmpVal = tmpVal | (1u << bit);
        } else {
            tmpVal = tmpVal & ~(1u << bit);
        }
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT_MASK2_OFFSET);
    }

    return SUCCESS;
}

int GLB_GPIO_IntClear(int gpioPin, int intClear)
{
    uint32_t tmpVal;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_CLR1_OFFSET);
        if (intClear == SET) {
            tmpVal = tmpVal | (1 << gpioPin);
        } else {
            tmpVal = tmpVal & ~(1 << gpioPin);
        }
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT_CLR1_OFFSET);
    } else if (gpioPin <= 43) {
        /* GPIO32 ~ GPIO43 — per vault glb.md: CLR bits live inside
         * INT_MASK3 @ 0x1AC bits[23:12] (w1p). Lower 12 bits in the
         * same register duplicate the mask. Read-modify-write so
         * touching CLR doesn't disturb that mask. */
        uint32_t bit = 12 + (gpioPin - 32);
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MASK3_OFFSET);
        if (intClear == SET) {
            tmpVal = tmpVal | (1u << bit);
        } else {
            tmpVal = tmpVal & ~(1u << bit);
        }
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT_MASK3_OFFSET);
    }

    return SUCCESS;
}

int GLB_Get_GPIO_IntStatus(int gpioPin)
{
    uint32_t tmpVal = 0;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_STAT1_OFFSET);
        return (tmpVal & (1 << gpioPin)) ? SET : RESET;
    } else if (gpioPin <= 43) {
        /* GPIO32 ~ GPIO43 — status lives in CFGCTL32 @ 0x184 bits
         * [23:12]; there is no dedicated INT_STAT2 register. */
        tmpVal = readl(GLB_BASE + GLB_GPIO_CFGCTL32_OFFSET);
        return (tmpVal & (1u << (12 + gpioPin - 32))) ? SET : RESET;
    }

    return RESET;
}

int GLB_Set_GPIO_IntMod(int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod, GLB_GPIO_INT_TRIG_Type intTrgMod)
{
    uint32_t tmpVal;
    uint32_t tmpGpioPin;

    if (gpioPin < GLB_GPIO_PIN_10) {
        /* GPIO0 ~ GPIO9 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MODE_SET1_OFFSET);
        tmpGpioPin = gpioPin;
        tmpVal = (tmpVal & ~(0x7 << (3 * tmpGpioPin))) | (((intCtlMod << 2) | intTrgMod) << (3 * tmpGpioPin));
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT_MODE_SET1_OFFSET);
    } else if (gpioPin < GLB_GPIO_PIN_20) {
        /* GPIO10 ~ GPIO19 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MODE_SET2_OFFSET);
        tmpGpioPin = gpioPin - GLB_GPIO_PIN_10;
        tmpVal = (tmpVal & ~(0x7 << (3 * tmpGpioPin))) | (((intCtlMod << 2) | intTrgMod) << (3 * tmpGpioPin));
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT_MODE_SET2_OFFSET);
    } else if (gpioPin < GLB_GPIO_PIN_30) {
        /* GPIO20 ~ GPIO29 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MODE_SET3_OFFSET);
        tmpGpioPin = gpioPin - GLB_GPIO_PIN_20;
        tmpVal = (tmpVal & ~(0x7 << (3 * tmpGpioPin))) | (((intCtlMod << 2) | intTrgMod) << (3 * tmpGpioPin));
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT_MODE_SET3_OFFSET);
    } else if (gpioPin < 32) {
        /* GPIO30 ~ GPIO31 not recommend */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MODE_SET4_OFFSET);
        tmpGpioPin = gpioPin - GLB_GPIO_PIN_30;
        tmpVal = (tmpVal & ~(0x7 << (3 * tmpGpioPin))) | (((intCtlMod << 2) | intTrgMod) << (3 * tmpGpioPin));
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT_MODE_SET4_OFFSET);
    } else if (gpioPin <= 43) {
        /* GPIO32 ~ GPIO43 — per vault glb.md, MODE_SET5 @ 0x1C4 covers
         * pins 32-39 and MODE_SET6 @ 0x1C8 covers pins 40-43. Fields are
         * 4 bits wide per pin: [3]=reserved, [2]=ctl (SYNC/ASYNC),
         * [1:0]=trig. Unlike the legacy <32 path which packs fields
         * 3-bits-per-pin, the upper-bank encoding matches the register
         * header. */
        uint32_t offset_reg = (gpioPin <= 39) ? GLB_GPIO_INT_MODE_SET5_OFFSET
                                              : GLB_GPIO_INT_MODE_SET6_OFFSET;
        uint32_t base_pin   = (gpioPin <= 39) ? 32 : 40;
        uint32_t bit        = (gpioPin - base_pin) * 4;
        uint32_t field      = ((intCtlMod & 0x1) << 2) | (intTrgMod & 0x3);
        tmpVal = readl(GLB_BASE + offset_reg);
        tmpVal = (tmpVal & ~(0xFu << bit)) | (field << bit);
        writel(tmpVal, GLB_BASE + offset_reg);
    }

    return SUCCESS;
}

GLB_GPIO_INT_CONTROL_Type GLB_Get_GPIO_IntCtlMod(int gpioPin)
{
    uint32_t tmpVal;
    uint32_t bitVal;

    if (gpioPin < GLB_GPIO_PIN_10) {
        /* GPIO0 - GPIO9 */
        bitVal = gpioPin - 0;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MODE_SET1_OFFSET);
        tmpVal = (tmpVal & (0x7 << (bitVal * 3))) >> (bitVal * 3);
        return (tmpVal >> 2) ? GLB_GPIO_INT_CONTROL_ASYNC : GLB_GPIO_INT_CONTROL_SYNC;
    } else if ((gpioPin > GLB_GPIO_PIN_9) && (gpioPin < GLB_GPIO_PIN_20)) {
        /* GPIO10 - GPIO19 */
        bitVal = gpioPin - 10;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MODE_SET2_OFFSET);
        tmpVal = (tmpVal & (0x7 << (bitVal * 3))) >> (bitVal * 3);
        return (tmpVal >> 2) ? GLB_GPIO_INT_CONTROL_ASYNC : GLB_GPIO_INT_CONTROL_SYNC;
    } else if ((gpioPin > GLB_GPIO_PIN_19) && (gpioPin < GLB_GPIO_PIN_30)) {
        /* GPIO20 - GPIO29 */
        bitVal = gpioPin - 20;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MODE_SET3_OFFSET);
        tmpVal = (tmpVal & (0x7 << (bitVal * 3))) >> (bitVal * 3);
        return (tmpVal >> 2) ? GLB_GPIO_INT_CONTROL_ASYNC : GLB_GPIO_INT_CONTROL_SYNC;
    } else {
        /* GPIO30 ~ GPIO31 not recommend */
        bitVal = gpioPin - 30;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT_MODE_SET4_OFFSET);
        tmpVal = (tmpVal & (0x7 << (bitVal * 3))) >> (bitVal * 3);
        return (tmpVal >> 2) ? GLB_GPIO_INT_CONTROL_ASYNC : GLB_GPIO_INT_CONTROL_SYNC;
    }
}

int GLB_GPIO_Int2Mask(int gpioPin, int intMask)
{
#if 0 // TODO: ipro
    uint32_t tmpVal;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MASK1_OFFSET);
        if (intMask == MASK) {
            tmpVal = tmpVal | (1 << gpioPin);
        } else {
            tmpVal = tmpVal & ~(1 << gpioPin);
        }
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT2_MASK1_OFFSET);
    }
#endif
    return SUCCESS;
}

int GLB_GPIO_Int2Clear(int gpioPin, int intClear)
{
#if 0 // TODO: ipro
    uint32_t tmpVal;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_CLR1_OFFSET);
        if (intClear == SET) {
            tmpVal = tmpVal | (1 << gpioPin);
        } else {
            tmpVal = tmpVal & ~(1 << gpioPin);
        }
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT2_CLR1_OFFSET);
    }
#endif
    return SUCCESS;
}

int GLB_Get_GPIO_Int2Status(int gpioPin)
{
#if 0 // TODO: ipro
    uint32_t tmpVal = 0;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_STAT1_OFFSET);
    }

    return (tmpVal & (1 << gpioPin)) ? SET : RESET;
#else
    return 0;
#endif
}

int GLB_Set_GPIO_Int2Mod(int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod, GLB_GPIO_INT_TRIG_Type intTrgMod)
{
#if 0 // TODO: ipro
    uint32_t tmpVal;
    uint32_t tmpGpioPin;
    if (gpioPin < GLB_GPIO_PIN_10) {
        /* GPIO0 ~ GPIO9 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MODE_SET1_OFFSET);
        tmpGpioPin = gpioPin;
        tmpVal = (tmpVal & ~(0x7 << (3 * tmpGpioPin))) | (((intCtlMod << 2) | intTrgMod) << (3 * tmpGpioPin));
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT2_MODE_SET1_OFFSET);
    } else if (gpioPin < GLB_GPIO_PIN_20) {
        /* GPIO10 ~ GPIO19 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MODE_SET2_OFFSET);
        tmpGpioPin = gpioPin - GLB_GPIO_PIN_10;
        tmpVal = (tmpVal & ~(0x7 << (3 * tmpGpioPin))) | (((intCtlMod << 2) | intTrgMod) << (3 * tmpGpioPin));
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT2_MODE_SET2_OFFSET);
    } else if (gpioPin < GLB_GPIO_PIN_30) {
        /* GPIO20 ~ GPIO29 */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MODE_SET3_OFFSET);
        tmpGpioPin = gpioPin - GLB_GPIO_PIN_20;
        tmpVal = (tmpVal & ~(0x7 << (3 * tmpGpioPin))) | (((intCtlMod << 2) | intTrgMod) << (3 * tmpGpioPin));
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT2_MODE_SET3_OFFSET);
    } else {
        /* GPIO30 ~ GPIO31 not recommend */
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MODE_SET4_OFFSET);
        tmpGpioPin = gpioPin - GLB_GPIO_PIN_30;
        tmpVal = (tmpVal & ~(0x7 << (3 * tmpGpioPin))) | (((intCtlMod << 2) | intTrgMod) << (3 * tmpGpioPin));
        writel(tmpVal, GLB_BASE + GLB_GPIO_INT2_MODE_SET4_OFFSET);
    }
#endif
    return SUCCESS;
}

GLB_GPIO_INT_CONTROL_Type GLB_Get_GPIO_Int2CtlMod(int gpioPin)
{
#if 0 // TODO: ipro
    uint32_t tmpVal;
    uint32_t bitVal;

    if (gpioPin < GLB_GPIO_PIN_10) {
        /* GPIO0 - GPIO9 */
        bitVal = gpioPin - 0;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MODE_SET1_OFFSET);
        tmpVal = (tmpVal & (0x7 << (bitVal * 3))) >> (bitVal * 3);
        return (tmpVal >> 2) ? GLB_GPIO_INT_CONTROL_ASYNC : GLB_GPIO_INT_CONTROL_SYNC;
    } else if ((gpioPin > GLB_GPIO_PIN_9) && (gpioPin < GLB_GPIO_PIN_20)) {
        /* GPIO10 - GPIO19 */
        bitVal = gpioPin - 10;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MODE_SET2_OFFSET);
        tmpVal = (tmpVal & (0x7 << (bitVal * 3))) >> (bitVal * 3);
        return (tmpVal >> 2) ? GLB_GPIO_INT_CONTROL_ASYNC : GLB_GPIO_INT_CONTROL_SYNC;
    } else if ((gpioPin > GLB_GPIO_PIN_19) && (gpioPin < GLB_GPIO_PIN_30)) {
        /* GPIO20 - GPIO29 */
        bitVal = gpioPin - 20;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MODE_SET3_OFFSET);
        tmpVal = (tmpVal & (0x7 << (bitVal * 3))) >> (bitVal * 3);
        return (tmpVal >> 2) ? GLB_GPIO_INT_CONTROL_ASYNC : GLB_GPIO_INT_CONTROL_SYNC;
    } else {
        /* GPIO30 ~ GPIO31 not recommend */
        bitVal = gpioPin - 30;
        tmpVal = readl(GLB_BASE + GLB_GPIO_INT2_MODE_SET4_OFFSET);
        tmpVal = (tmpVal & (0x7 << (bitVal * 3))) >> (bitVal * 3);
        return (tmpVal >> 2) ? GLB_GPIO_INT_CONTROL_ASYNC : GLB_GPIO_INT_CONTROL_SYNC;
    }
#else
    return 0;
#endif
}

