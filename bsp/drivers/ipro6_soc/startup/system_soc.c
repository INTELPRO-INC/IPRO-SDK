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

#include "ipro6.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_aon.h"
#include "hal_boot2.h"
#include "hal_sys.h"

#if 0
#include "drv_usb.h"
void USB_DoNothing_IRQHandler(void)
{
    /* clear all USB int sts */
    USB_Clr_IntStatus(32);
}
#endif

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define SYSTEM_CLOCK (32000000UL)

/*----------------------------------------------------------------------------
  Vector Table
 *----------------------------------------------------------------------------*/
#define VECT_TAB_OFFSET 0x00 /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x200. */

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
#ifndef CFG_IREGION_BASE_ADDR
volatile unsigned long CpuIRegionBase = 0xFFFFFFFF;
#endif

void system_bor_init(void)
{
    HBN_BOR_CFG_Type borCfg;
    borCfg.enableBor = 1;
    borCfg.enableBorInt = 0;
    borCfg.borThreshold = 1;
    borCfg.enablePorInBor = 1;
    HBN_Set_BOR_Cfg(&borCfg);
}

void SystemInit(void)
{
    int i = 0;

    /* global IRQ disable */
    __disable_irq();

    /* Detect reset reason early (before clearing interrupts) */
    hal_rst_reason_detect();

    /* CLear all interrupt */
    for(i=0;i<IRQn_LAST;i++) {
        __ECLIC_ClearPendingIRQ(i);
        __ECLIC_DisableIRQ(i);
    }

#if 0 // TODO: ipro7 TBC
    /* Restore default setting*/
    /* GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_NONE); */
    tmpVal = readl(GLB_BASE + GLB_PARM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_UART_SWAP_SET, UART_SIG_SWAP_NONE);
    writel(tmpVal, GLB_BASE + GLB_PARM_OFFSET);
#endif

//    Interrupt_Handler_Register(USB_IRQn, USB_DoNothing_IRQHandler);
#if 0 // TODO: ipro7 TBC
    /* dcdc 1.8v -> 1.5v */
    AON_Set_DCDC14_Top_0(0xC, 0x3);
#endif
    /* init bor for all platform */
    system_bor_init();
    /* global IRQ enable */
    __enable_irq();

}

void System_Post_Init(void)
{
    /* Initialize UART signal selection registers */
    writel(0xffffffff, GLB_BASE + GLB_UART_CFG1_OFFSET);
    //PDS_Trim_RC32M(); // TODO: ipro7
    HBN_Trim_RC32K();
}


// NOTE: FALLBACK_DEFAULT_ECLIC_BASE/FALLBACK_DEFAULT_SYSTIMER_BASE macros are removed
// No longer support for cpu without iregion feature

#define CLINT_MSIP(base, hartid)    (*(volatile uint32_t *)((uintptr_t)((base) + ((hartid) * 4))))

void _premain_init(void)
{
    //volatile unsigned long mcfginfo = __RV_CSR_READ(CSR_MCFG_INFO);

#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1)
    if (ICachePresent()) { // Check whether icache real present or not
        EnableICache();
    }
#endif

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1)
#ifndef BOOTROM
    if (DCachePresent()) { // Check whether dcache real present or not
        EnableDCache();
    }
#endif
#endif

    /* Do fence and fence.i to make sure previous ilm/dlm/icache/dcache control done */
    __RWMB();
    __FENCE_I();

    // IREGION INFO MUST BE AFTER L1/L2 Cache enabled and SMP enabled if SMP present
#ifndef CFG_IREGION_BASE_ADDR
    CpuIRegionBase = (__RV_CSR_READ(CSR_MIRGB_INFO) >> 10) << 10;
#endif

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
    // NOTE: CFG_HAS_SMODE and CFG_HAS_UMODE are defined in auto generated cpufeature.h if present in cpu
#if defined(CFG_HAS_SMODE) || defined(CFG_HAS_UMODE)
    EnableSUCCM();
#endif
#endif
    // TODO implement get_cpu_freq function to get real cpu clock freq in HZ or directly give the real cpu HZ
    // TODO you can directly give the correct cpu frequency here, if you know it without call get_cpu_freq function
//        SystemCoreClock = get_cpu_freq();
    /* Initialize exception default handlers */
    Exception_Init();
    /* Interrupt initialization */
    System_Interrupt_Init();

    hal_boot2_init_efuse();

    /* Do fence and fence.i to make sure previous ilm/dlm/icache/dcache control done */
    __RWMB();
    __FENCE_I();


#ifdef RUNMODE_CONTROL
    printf("Current RUNMODE=%s, ilm:%d, dlm %d, icache %d, dcache %d, ccm %d\r\n", \
        RUNMODE_STRING, RUNMODE_ILM_EN, RUNMODE_DLM_EN, \
        RUNMODE_IC_EN, RUNMODE_DC_EN, RUNMODE_CCM_EN);
    // ILM and DLM need to be present
    if (mcfginfo & 0x180 == 0x180) {
        printf("CSR: MILM_CTL 0x%x, MDLM_CTL 0x%x\r\n", \
            __RV_CSR_READ(CSR_MILM_CTL), __RV_CSR_READ(CSR_MDLM_CTL));
    }
    // I/D cache need to be present
    if (mcfginfo & 0x600) {
        printf("CSR: MCACHE_CTL 0x%x\r\n", __RV_CSR_READ(CSR_MCACHE_CTL));
    }
    printf("CSR: MMISC_CTL 0x%x\r\n", __RV_CSR_READ(CSR_MMISC_CTL));
#endif
}

void _postmain_fini(int status)
{
}

void _init(void)
{
    /* Don't put any code here, please use _premain_init now */
}

void _fini(void)
{
    /* Don't put any code here, please use _postmain_fini now */
}
