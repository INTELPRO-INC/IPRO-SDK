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

#include "hal_common.h"
#include "drv_aon.h"
#include "drv_ef_ctrl.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_l1c.h"
#include "drv_sec_eng.h"

#if defined(CONFIG_IPRO7) || defined(CONFIG_IPRO6)
extern void board_init(void);
#endif

//#define ENABLE_IRQ_NESTING_CHECK
void ATTR_TCM_SECTION cpu_global_irq_enable(void)
{
#ifdef ENABLE_IRQ_NESTING_CHECK
    if (__RV_CSR_READ(CSR_MSTATUS) & MSTATUS_MIE)
        while(1);
#endif
    __enable_irq();
}

void ATTR_TCM_SECTION cpu_global_irq_disable(void)
{
#ifdef ENABLE_IRQ_NESTING_CHECK
    if (!(__RV_CSR_READ(CSR_MSTATUS) & MSTATUS_MIE))
        while(1);
#endif
    __disable_irq();
}

void hal_por_reset(void)
{
    GLB_SW_POR_Reset();
}

void hal_system_reset(void)
{
    GLB_SW_System_Reset();
}

void hal_cpu_reset(void)
{
    GLB_SW_CPU_Reset();
}

void hal_reboot_config(hal_reboot_cfg_t rbot)
{
    (void)rbot;
}

void hal_get_chip_id(uint8_t chip_id[8])
{
    EF_Ctrl_Read_MAC_Address(chip_id);
}

void hal_enter_usb_iap(void)
{
    HBN_Set_Status_Flag(0x00425355); //"\0BSU"

    arch_delay_ms(1000);
    GLB_SW_POR_Reset();
}

void ATTR_TCM_SECTION hal_jump2app(uint32_t flash_offset)
{
    /*flash_offset from 48K to 3.98M*/
    if ((flash_offset >= 0xc000) && (flash_offset < (0x400000 - 20 * 1024))) {
        void (*app_main)(void) = (void (*)(void))0x23000000;
        SF_Ctrl_Set_Flash_Image_Offset(flash_offset, 0, 0);
//        L1C_Cache_Flush_Ext();
        app_main();
    } else {
        while(1)
        {}
    }
}

int hal_get_trng_seed(void)
{
    uint32_t seed[8];

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SEC);

    Sec_Eng_Trng_Enable();
    Sec_Eng_Trng_Read((uint8_t *)seed);
    Sec_Eng_Trng_Disable();

    GLB_PER_Clock_Gate(GLB_AHB_CLOCK_SEC);

    return seed[0];
}

void hal_system_init(void)
{
#if defined(CONFIG_IPRO7) || defined(CONFIG_IPRO6)
    /* IPRO7/IPRO6: board_init is called here (not in startup.S) */
    board_init();
#endif
    /* IPRO6LE: board_init is already called from startup.S */
}

int hal_is_cacheable_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;
    
    if ((a & 0xF0000000UL) == 0x10000000UL)
        return true;
    else
        return false;

}

void* hal_get_noncacheable_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    /* Already non-cacheable (0x2xxxxxxx or 0x3xxxxxxx), return as-is */
    if (!hal_is_cacheable_addr(addr)) {
        return (void *)addr;
    }

    /* Convert cacheable 0x1xxxxxxx to non-cacheable 0x2xxxxxxx */
    if ((a & 0xF0000000UL) == 0x10000000UL) {
        return (void *)((a & ~0xF0000000UL) | 0x20000000UL);
    }

    return NULL;
}

