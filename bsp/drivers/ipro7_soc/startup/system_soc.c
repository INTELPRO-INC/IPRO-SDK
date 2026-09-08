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

#include "ipro7.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_aon.h"
#include "hal_boot2.h"
#include "hal_sys.h"
#include "compiler/common.h"

#ifdef CONFIG_EARLY_PRINT
#include "early_print.h"
#endif

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
ATTR_OCRAM_SECTION volatile unsigned long CpuIRegionBase = 0xFFFFFFFF;
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
    /* DCDC voltage set moved to board_power_init() — applied AFTER the
     * VDD14 bond option is enabled and DCDC is powered up, so the rail
     * transition sequence matches hardware expectations. */
    /* init bor for all platform */
    system_bor_init();
    /* global IRQ enable */
    __enable_irq();

}

void System_Post_Init(void)
{
    writel(0xffffffff, GLB_BASE + GLB_UART_SIG_SEL_0_OFFSET);
    //PDS_Trim_RC32M(); // TODO: ipro7
    HBN_Trim_RC32K();
}


// NOTE: FALLBACK_DEFAULT_ECLIC_BASE/FALLBACK_DEFAULT_SYSTIMER_BASE macros are removed
// No longer support for cpu without iregion feature

#define CLINT_MSIP(base, hartid)    (*(volatile uint32_t *)((uintptr_t)((base) + ((hartid) * 4))))

void _premain_init(void)
{
    //volatile unsigned long mcfginfo = __RV_CSR_READ(CSR_MCFG_INFO);

#ifdef CONFIG_EARLY_PRINT
    early_print_init();
    early_puts("[EP] _premain_init\n");
    early_flush();
#endif

#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1)
    if (ICachePresent()) { // Check whether icache real present or not
        EnableICache();
    }
#endif
    /* Do fence and fence.i to make sure previous ilm/dlm/icache/dcache control done */
    __RWMB();
    __FENCE_I();

    // IREGION INFO MUST BE AFTER L1/L2 Cache enabled and SMP enabled if SMP present
    CpuIRegionBase = (__RV_CSR_READ(CSR_MIRGB_INFO) >> 10) << 10;

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

#ifdef CONFIG_EARLY_PRINT
    early_puts("[EP] efuse_init\n");
    early_flush();
#endif
    hal_boot2_init_efuse();

    /* Detect reset reason here in SystemInit so main() sees a valid value
     * when it prints "[SYS] Reset reason: ...".  Includes AON WDT status
     * check (used as pure_dcdc wakeup-hang safety net). */
    hal_rst_reason_detect();

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)
    /*
     * PSRAM HW init + section loading already done in startup.S
     * (via psram_early_hw_init or boot2). Just set LP framework
     * DQS config and availability flag.
     */
    {
        extern void hal_psram_set_dqs_cfg(uint16_t cfg);
        extern unsigned int hal_boot2_get_psram_dqs_cfg(void);
        extern uint32_t __psram_available;
        extern volatile uint16_t g_psram_early_dqs_cfg;

        /* Prefer the window psram_early_hw_init measured on this chip at
         * this boot over the efuse trim: on parts with psram_trim
         * unprogrammed the efuse reads 0xffff, and the LP wake path would
         * rebuild PSRAM at delay-table index 15 instead of the working
         * centre — PSRAM then returns garbage after the first PDS wake.
         * Falls back to efuse on the boot2 hand-off path, where no sweep
         * ran here. */
        uint16_t dqs = g_psram_early_dqs_cfg;
        if (dqs == 0) {
            dqs = (uint16_t)hal_boot2_get_psram_dqs_cfg();
        }
        hal_psram_set_dqs_cfg(dqs);
        __psram_available = 1;
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
