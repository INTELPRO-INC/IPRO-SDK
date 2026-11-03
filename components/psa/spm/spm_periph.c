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
 * NSPE peripheral table: UART0; with CONFIG_PSA_NSPE_DMA_USB DMA0 and
 * USB (ID1 masters under spm_tzc.c) and the USB GLB register grants.
 */
#include "spm_periph.h"

#include <stddef.h>

#include "psa_ipro/memmap.h"
#include "spm_log.h"
#include "system_soc.h"

#if defined(CONFIG_PSA_NSPE_DMA_USB) && defined(CONFIG_PSA_SPE) && \
    !defined(CONFIG_PSA_TZC_ISOLATION)
#error "CONFIG_PSA_NSPE_DMA_USB needs CONFIG_PSA_TZC_ISOLATION"
#endif

#ifdef CONFIG_PSA_NSPE_DMA_USB
#define SPM_GLB_BASE         0x30000000u
/* GLB usb_xcvr, usb_xcvr_config: the bits drv_usb_v1.c writes */
#define SPM_GLB_USB_XCVR     (SPM_GLB_BASE + 0x228u)
#define SPM_GLB_USB_XCVR_WM  0x00F11770u
#define SPM_GLB_USB_CFG      (SPM_GLB_BASE + 0x22Cu)
#define SPM_GLB_USB_CFG_WM   0x77777FFFu
/* GPIO_CFGCTL4 = pins 8 and 9, func_sel fixed to 10 (analog) */
#define SPM_GLB_GPIO_8_9     (SPM_GLB_BASE + 0x110u)
#define SPM_GLB_GPIO_FUNC    0x1F001F00u
#define SPM_GLB_GPIO_ANALOG  0x0A000A00u
/* CFGCTL35 bits 8, 9 = OE of pins 8 and 9 */
#define SPM_GLB_GPIO_OE      (SPM_GLB_BASE + 0x190u)
#define SPM_GLB_GPIO_OE_WM   0x00000300u

/* GLB_CGEN_CFG0 bit 2 = GLB_AHB_CLOCK_DMA_0 */
#define SPM_GLB_CGEN0        (SPM_GLB_BASE + 0x30u)
#define SPM_GLB_CGEN0_DMA0   0x4u

#define SPM_DMA_REG(off)     (*(volatile uint32_t *)(uintptr_t)( \
                                  PSA_NSPE_DMA_BASE + (off)))
#define SPM_DMA_INT_TC_CLR   0x08u
#define SPM_DMA_INT_ERR_CLR  0x10u
#define SPM_DMA_ENBLD_CHNS   0x1Cu
#define SPM_DMA_CH_CONFIG(ch) (0x110u + (ch) * 0x100u)
#define SPM_DMA_CHANNELS     4u
#define SPM_DMA_ALL_CH       0x0Fu
#define SPM_DMA_E            0x1u
/* reads of ENBLD_CHNS for the channels to stop, else spm_fatal */
#define SPM_DMA_HALT_POLLS   1000u
/* usb_xcvr PU_USB, USB_ENUM: off detaches the device from the host */
#define SPM_GLB_USB_PU_ENUM  0x00900000u
#define SPM_USB_CONFIG       PSA_NSPE_USB_BASE
#define SPM_USB_EN           0x1u

static void spm_dma_reset(void)
{
    *(volatile uint32_t *)(uintptr_t)SPM_GLB_CGEN0 |= SPM_GLB_CGEN0_DMA0;
    for (uint32_t ch = 0; ch < SPM_DMA_CHANNELS; ch++) {
        SPM_DMA_REG(SPM_DMA_CH_CONFIG(ch)) &= ~SPM_DMA_E;
    }
    for (uint32_t i = 0; SPM_DMA_REG(SPM_DMA_ENBLD_CHNS) & SPM_DMA_ALL_CH;
         i++) {
        if (i == SPM_DMA_HALT_POLLS) {
            SPM_LOG("DMA0 channels %lx still on",
                    (unsigned long)SPM_DMA_REG(SPM_DMA_ENBLD_CHNS));
            spm_fatal();
        }
    }
    SPM_DMA_REG(SPM_DMA_INT_TC_CLR) = SPM_DMA_ALL_CH;
    SPM_DMA_REG(SPM_DMA_INT_ERR_CLR) = SPM_DMA_ALL_CH;
}

static void spm_usb_reset(void)
{
    *(volatile uint32_t *)(uintptr_t)SPM_USB_CONFIG &= ~SPM_USB_EN;
    *(volatile uint32_t *)(uintptr_t)SPM_GLB_USB_XCVR &=
        ~SPM_GLB_USB_PU_ENUM;
}
#endif

static const struct spm_periph spm_periphs[] = {
    { PSA_NSPE_UART_BASE, PSA_NSPE_UART_SIZE, UART0_IRQn, NULL },
#ifdef CONFIG_PSA_NSPE_DMA_USB
    { PSA_NSPE_DMA_BASE, PSA_NSPE_DMA_SIZE, DMA0_ALL_IRQn,
      spm_dma_reset },
    { PSA_NSPE_USB_BASE, PSA_NSPE_USB_SIZE, USB_IRQn, spm_usb_reset },
#endif
};

#ifdef CONFIG_PSA_NSPE_DMA_USB
static const struct spm_reg_grant spm_reg_grants[] = {
    { SPM_GLB_USB_XCVR, SPM_GLB_USB_XCVR_WM, 0, 0 },
    { SPM_GLB_USB_CFG, SPM_GLB_USB_CFG_WM, 0, 0 },
    { SPM_GLB_GPIO_8_9, ~SPM_GLB_GPIO_FUNC, SPM_GLB_GPIO_FUNC,
      SPM_GLB_GPIO_ANALOG },
    { SPM_GLB_GPIO_OE, SPM_GLB_GPIO_OE_WM, 0, 0 },
};

_Static_assert(sizeof(spm_reg_grants) / sizeof(spm_reg_grants[0]) ==
                   SPM_REG_GRANT_COUNT,
               "SPM_REG_GRANT_COUNT");
_Static_assert(DMA_BASE == PSA_NSPE_DMA_BASE &&
                   USB_BASE == PSA_NSPE_USB_BASE,
               "PSA_NSPE_DMA_BASE / USB_BASE are not this build's");
#endif

_Static_assert(sizeof(spm_periphs) / sizeof(spm_periphs[0]) ==
                   SPM_PERIPH_COUNT,
               "SPM_PERIPH_COUNT");
_Static_assert(UART0_BASE == PSA_NSPE_UART_BASE,
               "PSA_NSPE_UART_BASE is not the UART0 of this build");

const struct spm_periph *spm_periph_at(uint32_t i)
{
    return i < SPM_PERIPH_COUNT ? &spm_periphs[i] : NULL;
}

int spm_periph_irq_ok(uint32_t irq)
{
    for (uint32_t i = 0; i < SPM_PERIPH_COUNT; i++) {
        if (spm_periphs[i].irq == irq) {
            return 1;
        }
    }
    return 0;
}

void spm_periph_reset(void)
{
    for (uint32_t i = 0; i < SPM_PERIPH_COUNT; i++) {
        if (spm_periphs[i].reset != NULL) {
            spm_periphs[i].reset();
        }
    }
}

const struct spm_reg_grant *spm_reg_grant_find(uint32_t addr)
{
#ifdef CONFIG_PSA_NSPE_DMA_USB
    for (uint32_t i = 0; i < SPM_REG_GRANT_COUNT; i++) {
        if (spm_reg_grants[i].addr == addr) {
            return &spm_reg_grants[i];
        }
    }
#else
    (void)addr;
#endif
    return NULL;
}
