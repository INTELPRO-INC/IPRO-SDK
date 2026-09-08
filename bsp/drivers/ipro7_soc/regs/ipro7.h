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

#ifndef __IPRO7_H__
#define __IPRO7_H__

// CPU Features
#ifndef UNDEF_CFG_TMR_PRIVATE
#define CFG_TMR_PRIVATE
#endif
#ifndef UNDEF_CFG_HAS_CLIC
#define CFG_HAS_CLIC
#endif

// External Interrupt Number
#define CFG_IRQ_NUM                 64

#ifndef UNDEF_CFG_HAS_PMP
//#define CFG_HAS_PMP
#endif
//#define CFG_PMP_ENTRY_NUM           8

#define CFG_HAS_STACK_CHECK
//#define CFG_HAS_NICE
//#define CFG_HAS_VNICE

#define CFG_HAS_ICACHE
#ifndef CONFIG_DCACHE_DISABLE
#define CFG_HAS_DCACHE
#endif
#define CFG_HAS_IOCC

#define IRQ_NUM_BASE 19

typedef enum {
    /* =======================================  Core Specific Interrupt Numbers  ======================================== */
    Reserved0_IRQn            =   0,              /*!<  Internal reserved */
    Reserved1_IRQn            =   1,              /*!<  Internal reserved */
    Reserved2_IRQn            =   2,              /*!<  Internal reserved */
    CLIC_INT_SFT              =   3,              /*!< CPU Software interrupt */
    SysTimerSW_IRQn           =   3,              /*!<  System Timer SW interrupt */
    Reserved3_IRQn            =   4,              /*!<  Internal reserved */
    Reserved4_IRQn            =   5,              /*!<  Internal reserved */
    Reserved5_IRQn            =   6,              /*!<  Internal reserved */
    CLIC_INT_TMR              =   7,              /*!< CPU Timer interrupt */
    SysTimer_IRQn             =   7,              /*!<  System Timer Interrupt */
    Reserved6_IRQn            =   8,              /*!<  Internal reserved */
    Reserved7_IRQn            =   9,              /*!<  Internal reserved */
    Reserved8_IRQn            =  10,              /*!<  Internal reserved */
    Reserved9_IRQn            =  11,              /*!<  Internal reserved */
    Reserved10_IRQn           =  12,              /*!<  Internal reserved */
    Reserved11_IRQn           =  13,              /*!<  Internal reserved */
    Reserved12_IRQn           =  14,              /*!<  Internal reserved */
    Reserved13_IRQn           =  15,              /*!<  Internal reserved */
    Reserved14_IRQn           =  16,              /*!<  Internal reserved */
    BusError_IRQn             =  17,              /*!<  Bus Error interrupt */
    PerfMon_IRQn              =  18,              /*!<  Performance Monitor */

    /******  SoC Interrupt Numbers **********************************************************************/
    BUS_ERR_IRQn = IRQ_NUM_BASE + 0,
    OSD_BLEND_IRQn = IRQ_NUM_BASE + 1,
    D2XC_IRQn = IRQ_NUM_BASE + 2,
    DTSRC_IRQn = IRQ_NUM_BASE + 3,
    IPROAI_IRQn = IRQ_NUM_BASE + 4,
    RF_TOP_INT0_IRQn = IRQ_NUM_BASE + 5,
    RF_TOP_INT1_IRQn = IRQ_NUM_BASE + 6,
    D2XA_IRQn = IRQ_NUM_BASE + 7,
    D2XB_IRQn = IRQ_NUM_BASE + 8,
    SEC_GMAC_IRQn = IRQ_NUM_BASE + 9,
    SEC_CDET_IRQn = IRQ_NUM_BASE + 10,
    SEC_PKA_IRQn = IRQ_NUM_BASE + 11,
    SEC_TRNG_IRQn = IRQ_NUM_BASE + 12,
    SEC_AES_IRQn = IRQ_NUM_BASE + 13,
    SEC_SHA_IRQn = IRQ_NUM_BASE + 14,
    DMA0_ALL_IRQn = IRQ_NUM_BASE + 15,
    PIO_INT0_IRQn = IRQ_NUM_BASE + 16,
    PIO_INT1_IRQn = IRQ_NUM_BASE + 17,
    SOLO_IRQn = IRQ_NUM_BASE + 18,
    IRTX_IRQn = IRQ_NUM_BASE + 19,
    LDO14_OCP_IRQn = IRQ_NUM_BASE + 20,
    DCDC_OCP_IRQn = IRQ_NUM_BASE + 21,
    JPEG_IRQn = IRQ_NUM_BASE + 22,
    MJDEC_IRQn = JPEG_IRQn,
    SF_CTRL_IRQn = IRQ_NUM_BASE + 23,
    SCALER_HYDRE_IRQn = IRQ_NUM_BASE + 24,
    GPADC_DMA_IRQn = IRQ_NUM_BASE + 25,
    EFUSE_IRQn = IRQ_NUM_BASE + 26,
    SPI0_IRQn = IRQ_NUM_BASE + 27,
    USB_IRQn = IRQ_NUM_BASE + 28,
#ifdef RFDTBT2
    UART0_IRQn = IRQ_NUM_BASE + 30,
    UART1_IRQn = IRQ_NUM_BASE + 29,
#else
    UART0_IRQn = IRQ_NUM_BASE + 29,
    UART1_IRQn = IRQ_NUM_BASE + 30,
#endif
    CAN_IRQn = IRQ_NUM_BASE + 31,
    I2C0_IRQn = IRQ_NUM_BASE + 32,
    XTAL_CNT_32K_DONE = IRQ_NUM_BASE + 33,
    PWM_IRQn = IRQ_NUM_BASE + 34,
    SDH_IRQn = IRQ_NUM_BASE + 35,
    TIMER0_CH0_IRQn = IRQ_NUM_BASE + 36,
    TIMER0_CH1_IRQn = IRQ_NUM_BASE + 37,
    TIMER0_WDT_IRQn = IRQ_NUM_BASE + 38,
    KYS_IRQn = IRQ_NUM_BASE + 39,
    I2S0_IRQn = IRQ_NUM_BASE + 40,
    QDEC0_IRQn = IRQ_NUM_BASE + 41,
    QDEC1_IRQn = IRQ_NUM_BASE + 42,
    QDEC2_IRQn = IRQ_NUM_BASE + 43,
    GPIO_INT0_IRQn = IRQ_NUM_BASE + 44,
    EMAC_IRQn = IRQ_NUM_BASE + 45,
    PMAC_IRQn = IRQ_NUM_BASE + 46,
    M154_REQ_ENH_ACK_IRQn = IRQ_NUM_BASE + 47,
    M154_IRQn = IRQ_NUM_BASE + 48,
    M154_AES_IRQn = IRQ_NUM_BASE + 49,
    PDS_WAKEUP_IRQn = IRQ_NUM_BASE + 50,
    HBN_OUT0_IRQn = IRQ_NUM_BASE + 51,
    HBN_OUT1_IRQn = IRQ_NUM_BASE + 52,
    BOR_IRQn = IRQ_NUM_BASE + 53,
    SPI2_IRQn = IRQ_NUM_BASE + 54,
    BZ_PHY_IRQn = IRQ_NUM_BASE + 55,
    BLE_IRQn = IRQ_NUM_BASE + 56,
    ISP_AE_IRQn = IRQ_NUM_BASE + 57,
    ISP_AE_HIST_IRQn = IRQ_NUM_BASE + 58,
    ISP_AWB3_IRQn = IRQ_NUM_BASE + 59,
    ISP_WDR_IRQn = IRQ_NUM_BASE + 60,
    ISP_MM_SEOF0_IRQn = IRQ_NUM_BASE + 61,
    ISP_MM_SEOF1_IRQn = IRQ_NUM_BASE + 62,
    ISP_I2C1_IRQn = IRQ_NUM_BASE + 63,
    IRQn_LAST = IRQ_NUM_BASE + 64,
} IRQn_Type;

typedef enum EXCn {
    InsUnalign_EXCn          =   0,              /*!<  Instruction address misaligned */
    InsAccFault_EXCn         =   1,              /*!<  Instruction access fault */
    IlleIns_EXCn             =   2,              /*!<  Illegal instruction */
    Break_EXCn               =   3,              /*!<  Beakpoint */
    LdAddrUnalign_EXCn       =   4,              /*!<  Load address misaligned */
    LdFault_EXCn             =   5,              /*!<  Load access fault */
    StAddrUnalign_EXCn       =   6,              /*!<  Store or AMO address misaligned */
    StAccessFault_EXCn       =   7,              /*!<  Store or AMO access fault */
    UmodeEcall_EXCn          =   8,              /*!<  Environment call from User mode */
    SmodeEcall_EXCn          =   9,              /*!<  Environment call from S-mode */
    MmodeEcall_EXCn          =  11,              /*!<  Environment call from Machine mode */
    InsPageFault_EXCn        =  12,              /*!<  Instruction page fault */
    LdPageFault_EXCn         =  13,              /*!<  Load page fault */
    StPageFault_EXCn         =  15,              /*!<  Store or AMO page fault */
    NMI_EXCn                 =  0xfff,           /*!<  NMI interrupt */
} EXCn_Type;

/* =========================================================================================================================== */
/* ================                           Processor and Core Section                                      ================ */
/* =========================================================================================================================== */
/*!< Set to 0, 1, or 2, 0 not present, 1 single floating point unit present, 2 double floating point unit present */
#if !defined(__riscv_flen)
#define __FPU_PRESENT             0
#elif __riscv_flen == 32
#define __FPU_PRESENT             1
#else
#define __FPU_PRESENT             2
#endif

#if defined(__riscv_bitmanip)
#define __BITMANIP_PRESENT        1                     /*!< Set to 1 if Bitmainpulation extension is present */
#else
#define __BITMANIP_PRESENT        0                     /*!< Set to 1 if Bitmainpulation extension is present */
#endif
#if defined(__riscv_dsp)
#define __DSP_PRESENT             1                     /*!< Set to 1 if Partial SIMD(DSP) extension is present */
#else
#define __DSP_PRESENT             0                     /*!< Set to 1 if Partial SIMD(DSP) extension is present */
#endif
#if defined(__riscv_vector)
#define __VECTOR_PRESENT          1                     /*!< Set to 1 if Vector extension is present */
#else
#define __VECTOR_PRESENT          0                     /*!< Set to 1 if Vector extension is present */
#endif

// CPU IREGION Base Address
// To set IREGION base, just define macro CFG_IREGION_BASE_ADDR in cpufeature.h
#ifndef CFG_IREGION_BASE_ADDR
// it is defined in system_evalsoc.c, you should not use this variable CpuIRegionBase
// SystemIRegionInfo variable in previous release is removed, you should avoid to use it
// you should use macro __IREGION_BASEADDR defined in evalsoc.h
extern volatile unsigned long CpuIRegionBase;
#define CPU_IREGION_BASE            CpuIRegionBase
#else
#define CPU_IREGION_BASE            CFG_IREGION_BASE_ADDR
#endif
#define __IREGION_BASEADDR          (CPU_IREGION_BASE)

// ECLIC Configuration
// To enable ECLIC, just define macro CFG_HAS_CLIC/CFG_CLICINTCTLBITS/CFG_IRQ_NUM in cpufeature.h
#ifdef CFG_HAS_CLIC
#define __ECLIC_PRESENT             1
#ifdef CFG_CLICINTCTLBITS
#define __ECLIC_INTCTLBITS          CFG_CLICINTCTLBITS
#endif
#define __ECLIC_INTNUM              (CFG_IRQ_NUM + IRQ_NUM_BASE)
#else
#define __ECLIC_PRESENT             0
#endif
#define __ECLIC_BASEADDR            (__IREGION_BASEADDR + IREGION_ECLIC_OFS)

// PLIC Configuration
// To enable PLIC, just define macro CFG_HAS_PLIC/CFG_IRQ_NUM in cpufeature.h
#ifdef CFG_HAS_PLIC
#define __PLIC_PRESENT              1
#define __PLIC_INTNUM               (CFG_IRQ_NUM + 1)
#else
#define __PLIC_PRESENT              0
#endif
#define __PLIC_BASEADDR             (__IREGION_BASEADDR + IREGION_PLIC_OFS)

// CPU System Timer Configuration
// To enable CPU System Timer, just define macro CFG_TMR_PRIVATE in cpufeature.h
#if defined(CFG_TMR_PRIVATE) || defined(CFG_TMR_BASE_ADDR)
#define __SYSTIMER_PRESENT          1
#else
#define __SYSTIMER_PRESENT          0
#endif
#ifdef CFG_TMR_BASE_ADDR    // Maybe using timer out of cpu for evalsoc
#define __SYSTIMER_BASEADDR         (CFG_TMR_BASE_ADDR)
#else
#define __SYSTIMER_BASEADDR         (__IREGION_BASEADDR + IREGION_TIMER_OFS)
#endif
#define __CLINT_TIMER_BASEADDR      (__SYSTIMER_BASEADDR + 0x1000)

// CIDU Configuration
// To enable CIDU, just define macro CFG_HAS_IDU in cpufeature.h
#ifdef CFG_HAS_IDU
#define __CIDU_PRESENT              1
#else
#define __CIDU_PRESENT              0
#endif

#define __CIDU_BASEADDR             (__IREGION_BASEADDR + IREGION_IDU_OFS)

// SMP & CC Configuration
// To enable SMP & CC, just define macro CFG_HAS_SMP in cpufeature.h
#ifdef CFG_HAS_SMP
#define __SMPCC_PRESENT             1
#else
#define __SMPCC_PRESENT             0
#endif

#define __SMPCC_BASEADDR            (__IREGION_BASEADDR + IREGION_SMP_OFS)

// PMP Configuration
// To enable PMP, just define macro CFG_HAS_PMP/__PMP_ENTRY_NUM in cpufeature.h
#ifdef CFG_HAS_PMP
#define __PMP_PRESENT               1
#define __PMP_ENTRY_NUM             CFG_PMP_ENTRY_NUM
#else
#define __PMP_PRESENT               0
#define __PMP_ENTRY_NUM             0
#endif

// TEE/sPMP Configuration
// To enable TEE, just define macro CFG_HAS_TEE in cpufeature.h
// TEE required PMP, please also make sure CFG_HAS_PMP defined
#ifdef CFG_HAS_TEE
#define __TEE_PRESENT               1
#define __SPMP_PRESENT              1
#define __SPMP_ENTRY_NUM            CFG_PMP_ENTRY_NUM
#ifdef CFG_HAS_SMPU
#define __SMPU_PRESENT              1
#define __SMPU_ENTRY_NUM            __SPMP_ENTRY_NUM
#endif
#else
#define __TEE_PRESENT               0
#define __SPMP_PRESENT              0
#define __SMPU_PRESENT              0
#define __SPMP_ENTRY_NUM            0
#endif

// ICache Configuration
// To enable ICACHE, just define macro CFG_HAS_ICACHE in cpufeature.h
#ifdef CFG_HAS_ICACHE
#define __ICACHE_PRESENT            1
#else
#define __ICACHE_PRESENT            0
#endif

// DCache Configuration
// To enable DCACHE, just define macro CFG_HAS_DCACHE in cpufeature.h
#ifdef CFG_HAS_DCACHE
#define __DCACHE_PRESENT            1
#else
#define __DCACHE_PRESENT            0
#endif

// CCM Configuration
// To enable CCM, just define macro CFG_HAS_IOCC in cpufeature.h
#ifdef CFG_HAS_IOCC
#define __CCM_PRESENT               1
#else
#define __CCM_PRESENT               0
#endif

// NICE Configuration
// To enable NICE, just define macro CFG_HAS_NICE in cpufeature.h
#ifdef CFG_HAS_NICE
#define __NICE_PRESENT              1
#else
#define __NICE_PRESENT              0
#endif

// VNICE Configuration
// To enable Vector NICE, just define macro CFG_HAS_VNICE in cpufeature.h
#ifdef CFG_HAS_VNICE
#define __VNICE_PRESENT             1
#else
#define __VNICE_PRESENT             0
#endif

#ifndef __INC_INTRINSIC_API
#define __INC_INTRINSIC_API         0                     /*!< Set to 1 if intrinsic api header files need to be included */
#endif

#define __Vendor_SysTickConfig      0                     /*!< Set to 1 if different SysTick Config is used */
#define __Vendor_EXCEPTION          0                     /*!< Set to 1 if vendor exception hander is present */


/*!< Set this timer hartid if you have only 1 hart in your cpu, and you know the timer hartid, just set it */
//#define __SYSTIMER_HARTID           0


#ifndef __HARTID_OFFSET
/**
 * If your hart index is different to your hartid, you must define this __HARTID_OFFSET macro.
 * For example, if your cpu has 4 harts, and hartid start from 3, so the __HARTID_OFFSET should set to 3.
 * Which means hartid 3-6 means hart index 0-3, this is useful for the timer software interrupt and timer interrupt trigger register location
 */
#define __HARTID_OFFSET             0
#endif

/* Define boot hart id */
#ifndef BOOT_HARTID
/**
 * Choosen boot hart id in current cluster when in soc system, need to align with the value defined in startup_<Device>.S,
 * should start from 0, taken the mhartid bit 0-7 value
 */
#define BOOT_HARTID                 0
#endif

/** @} */ /* End of group Configuration_of_NMSIS */

/* ========================================  Start of section using anonymous unions  ======================================== */

#define RTC_FREQ                  (2000000UL)
#define SOC_TIMER_FREQ            (RTC_FREQ << 1)
// The TIMER frequency is just the RTC frequency, sampled at both rising and falling edge
// RTC_FREQ = 2M, so mtimer tick 1 us = 4T (2000000/1000000*2)
#define SOC_TIMER_TICK_PER_US     (4UL) 

#define DRV_FLASH_XIP_BASE        (0x13000000)
#define DRV_FLASH_XIP_END         (DRV_FLASH_XIP_BASE + 16 * 1024 * 1024)
#define DRV_FLASH_XIP_REMAP0_BASE (0x23000000)
#define DRV_FLASH_XIP_REMAP0_END  (DRV_FLASH_XIP_REMAP0_BASE + 16 * 1024 * 1024)

#define DRV_PSRAM_XIP_BASE        (0x14000000)
#define DRV_PSRAM_XIP_END         (DRV_PSRAM_XIP_BASE + 16 * 1024 * 1024)
#define DRV_PSRAM_XIP_REMAP0_BASE (0x24000000)
#define DRV_PSRAM_XIP_REMAP0_END  (DRV_PSRAM_XIP_REMAP0_BASE + 16 * 1024 * 1024)

#define DRV_TCM_BASE              (0x11010000)
#define DRV_TCM_END               (DRV_TCM_BASE + 256*1024)
#define DRV_TCM_REMAP0_BASE       (0x21010000)
#define DRV_TCM_REMAP0_END        (DRV_TCM_REMAP0_BASE + 256*1024)

#define GLB_BASE         ((uint32_t)0x30000000)
#define RF_BASE          ((uint32_t)0x30001000)
#define BZ_PHY_BASE      ((uint32_t)0x30002000)
#define BZ_PHY_AGC_BASE  ((uint32_t)0x30003000)
#define GPIP_BASE        ((uint32_t)0x30004000) /*!< AUX module base address */
#define SEC_DBG_BASE     ((uint32_t)0x30005000) /*!< Security Debug module base address */
#define TZC_SEC_BASE     ((uint32_t)0x30006000) /*!< Trustzone control security base address */
#define TZC_NSEC_BASE    ((uint32_t)0x30007000) /*!< Trustzone control none-security base address */
#define EF_DATA_BASE     ((uint32_t)0x30008000)
#define EF_CTRL_BASE     ((uint32_t)0x30008000)
#define CCI_BASE         ((uint32_t)0x30009000)
#define MCU_MISC_BASE    ((uint32_t)0x3000A000)
#ifdef RFDTBT2
#define UART0_BASE       ((uint32_t)0x3000C000)
#define UART1_BASE       ((uint32_t)0x3000B000)
#else
#define UART0_BASE       ((uint32_t)0x3000B000)
#define UART1_BASE       ((uint32_t)0x3000C000)
#endif
#define SPI_BASE         ((uint32_t)0x3000D000)
#define I2C_BASE         ((uint32_t)0x3000E000)
#define PWM_BASE         ((uint32_t)0x3000F000)
#define TIMER_BASE       ((uint32_t)0x30010000)
#define IR_BASE          ((uint32_t)0x30011000)
#define CKS_BASE         ((uint32_t)0x30012000)
#define QDEC0_BASE       ((uint32_t)0x30013000)
#define QDEC1_BASE       ((uint32_t)0x30014000)
#define QDEC2_BASE       ((uint32_t)0x30015000)
#define KYS_BASE         ((uint32_t)0x30016000)
#define I2S_BASE         ((uint32_t)0x30017000)
#define AUD_BASE         ((uint32_t)0x30018000)
#define I2C1_BASE        ((uint32_t)0x30019000)
#define PSRAM_CTRL_BASE  ((uint32_t)0x3001B000)
#define CAN_BASE         ((uint32_t)0x3001C000)
#define PIO_BASE         ((uint32_t)0x3001D000)
#define SPI1_BASE        ((uint32_t)0x3001E000)
#define MM_MISC_BASE     ((uint32_t)0x30040000)
#define D2XA_BASE        ((uint32_t)0x30041000)
#define D2XB_BASE        ((uint32_t)0x30042000)
#define D2XC_BASE        ((uint32_t)0x30043000)
#define SCALERDA_BASE    ((uint32_t)0x30045000)
#define OSD_BLEND_BASE   ((uint32_t)0x30046000)
#define OSD_DRAW_BASE    ((uint32_t)0x30047000)
#define ISP_BASE         ((uint32_t)0x30048000)
#define MJPEG_BASE       ((uint32_t)0x30049000)
#define IPROAI_BASE      ((uint32_t)0x3004A000)
#define AE_AVG_BASE      ((uint32_t)0x3004B000)
#define AE_HIST_BASE     ((uint32_t)0x3004C000)
#define DTSRCT_BASE      ((uint32_t)0x3004D000)
#define WDR_BASE         ((uint32_t)0x3004E000)
#define GAMMA_BASE       ((uint32_t)0x3004F000)
#define SEC_ENG_BASE     ((uint32_t)0x30080000) /*!< Security Engine module base address */
#define SF_CTRL_BASE     ((uint32_t)0x30081000)
#define SF_CTRL_BUF_BASE ((uint32_t)0x30081600)
#define DMA_BASE         ((uint32_t)0x30082000)
#define EMAC_BASE        ((uint32_t)0x30083000)
#define USB_BASE         ((uint32_t)0x30084000)
#define SDH_BASE         ((uint32_t)0x30085000)
#define PDS_BASE         ((uint32_t)0x3008E000) /*!< Power down sleep module base address */
#define HBN_BASE         ((uint32_t)0x3008F000) /*!< Hibernate module base address */
#define AON_BASE         ((uint32_t)0x3008F000) /*!< Always on module base address */
#define MAC154_BASE      ((uint32_t)0x3C000000) /*!< MAC154 module base address */

#define HBN_RAM_BASE ((uint32_t)0x20090000)

typedef enum {
    GLB_AHB_MCU_SW_M154 = 4,
    GLB_AHB_MCU_SW_BLE = 8,
    GLB_AHB_MCU_SW_I2C1 = 16,
    GLB_AHB_MCU_SW_GLB = 32,
    GLB_AHB_MCU_SW_MIX = 33,
    GLB_AHB_MCU_SW_GPIP = 34,
    GLB_AHB_MCU_SW_SEC_DBG = 35,
    GLB_AHB_MCU_SW_SEC_ENG = 36,
    GLB_AHB_MCU_SW_TZ1 = 37,
    GLB_AHB_MCU_SW_TZ2 = 38,
    GLB_AHB_MCU_SW_EFUSE = 39,
    GLB_AHB_MCU_SW_CCI = 40,
    GLB_AHB_MCU_SW_L1C = 41,
    GLB_AHB_MCU_SW_SF = 42,
    GLB_AHB_MCU_SW_DMA = 43,
    GLB_AHB_MCU_SW_PDS = 44,
    GLB_AHB_MCU_SW_UART0 = 45,
    GLB_AHB_MCU_SW_SPI = 46,
    GLB_AHB_MCU_SW_I2C = 47,
    GLB_AHB_MCU_SW_PWM = 48,
    GLB_AHB_MCU_SW_TIMER = 49,
    GLB_AHB_MCU_SW_IR_REMOTE = 50,
    GLB_AHB_MCU_SW_CHECKSUM = 51,
    GLB_AHB_MCU_SW_KYS = 52,
    GLB_AHB_MCU_SW_AUSOLO = 53,
    GLB_AHB_MCU_SW_PSRAM = 54,
    GLB_AHB_MCU_SW_QEDC = 55,
    GLB_AHB_MCU_SW_PIO = 56,
    GLB_AHB_MCU_SW_MM = 57,
    GLB_AHB_MCU_SW_USB = 58,
    GLB_AHB_MCU_SW_EMAC = 59,
    GLB_AHB_MCU_SW_UART1 = 60,
    GLB_AHB_MCU_SW_CAN = 61,
    GLB_AHB_MCU_SW_I2S = 62,
    GLB_AHB_MCU_SW_SPI2 = 63,
    GLB_AHB_MCU_SW_PWRON_RST = 64,
    GLB_AHB_MCU_SW_CPU_RESET = 65,
    GLB_AHB_MCU_SW_SYS_RESET = 66,
    GLB_AHB_MCU_SW_SDH = 72,
} DRV_AHB_MCU_SW_Type;

typedef enum {
    GLB_AHB_CLOCK_MCU = 0,
    GLB_AHB_CLOCK_SEC,
    GLB_AHB_CLOCK_DMA_0,
    GLB_AHB_CLOCK_CCI,
    GLB_AHB_CLOCK_GPIP,
    GLB_AHB_CLOCK_SEC_DBG,
    GLB_AHB_CLOCK_SEC_ENG,
    GLB_AHB_CLOCK_TZ1,
    GLB_AHB_CLOCK_EF_CTRL,
    GLB_AHB_CLOCK_SF_CTRL,
    GLB_AHB_CLOCK_SPI1,
    GLB_AHB_CLOCK_UART0,
    GLB_AHB_CLOCK_SPI0,
    GLB_AHB_CLOCK_I2C0,
    GLB_AHB_CLOCK_PWM,
    GLB_AHB_CLOCK_TIMER,
    GLB_AHB_CLOCK_IRR,
    GLB_AHB_CLOCK_CHECKSUM,
    GLB_AHB_CLOCK_KYS,
    GLB_AHB_CLOCK_AUIDO,
    GLB_AHB_CLOCK_MM,
    GLB_AHB_CLOCK_UART1,
    GLB_AHB_CLOCK_CAN,
    GLB_AHB_CLOCK_I2S,
    GLB_AHB_CLOCK_QDEC0,
    GLB_AHB_CLOCK_QDEC1,
    GLB_AHB_CLOCK_QDEC2,
    GLB_AHB_CLOCK_PIO,
    GLB_AHB_CLOCK_USB,
    GLB_AHB_CLOCK_PKA,
    GLB_AHB_CLOCK_EMAC,
    GLB_AHB_CLOCK_SDH,
    GLB_AHB_CLOCK_I2C1 = 32+14,
    GLB_AHB_CLOCK_M154 = 64,
    GLB_AHB_CLOCK_BLE = 64+4,
} GLB_AHB_CLOCK_Type;

typedef enum {
    DRV_AHB_SEC_ENG_AES0 = 0,
    DRV_AHB_SEC_ENG_AES1,
    DRV_AHB_SEC_ENG_SHA0,
    DRV_AHB_SEC_ENG_SHA1,
} DRV_AHB_Sec_Eng_Type;

typedef enum {
    DRV_AHB_DMA0_CH0 = 0,
    DRV_AHB_DMA0_CH1,
    DRV_AHB_DMA0_CH2,
    DRV_AHB_DMA0_CH3,
    DRV_AHB_DMA0_CH4,
    DRV_AHB_DMA0_CH5,
    DRV_AHB_DMA0_CH6,
    DRV_AHB_DMA0_CH7,
} DRV_AHB_DMA0_CHNL_Type;


#ifndef NULL
#define NULL ((void*)0) /**< This define statement sets NULL as a preprocessor alias for (void*)0 */
#endif

#include <stdint.h>
#include "core_compatiable.h"
#include "core_feature_base.h"
#include "core_feature_cache.h"
#include "core_feature_eclic.h"
#include "core_feature_timer.h"
#include "riscv_encoding.h"

#endif
