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

#ifndef __IPRO6_H__
#define __IPRO6_H__

/**
 * @file ipro6.h
 * @brief IPRO6 Chip Definition Header
 *
 * IPRO6 is a hybrid chip:
 * - CPU: Same as IPRO7 (RISC-V RV32IMAFC with custom extensions)
 * - SoC/Peripherals: IPRO6 specific (memory map, registers, IRQs)
 * - No PSRAM support
 */

/*============================================================================
 * IPRO7 CPU Features Configuration (same CPU as IPRO7)
 *============================================================================*/
#ifndef UNDEF_CFG_TMR_PRIVATE
#define CFG_TMR_PRIVATE
#endif
#ifndef UNDEF_CFG_HAS_CLIC
#define CFG_HAS_CLIC
#endif

// External Interrupt Number
#define CFG_IRQ_NUM                 80

#define CFG_HAS_STACK_CHECK

#define CFG_HAS_ICACHE
#ifndef CONFIG_DCACHE_DISABLE
#define CFG_HAS_DCACHE
#endif
#define CFG_HAS_IOCC

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
#define __BITMANIP_PRESENT        1
#else
#define __BITMANIP_PRESENT        0
#endif
#if defined(__riscv_dsp)
#define __DSP_PRESENT             1
#else
#define __DSP_PRESENT             0
#endif
#if defined(__riscv_vector)
#define __VECTOR_PRESENT          1
#else
#define __VECTOR_PRESENT          0
#endif

// CPU IREGION Base Address
#ifndef CFG_IREGION_BASE_ADDR
extern volatile unsigned long CpuIRegionBase;
#define CPU_IREGION_BASE            CpuIRegionBase
#else
#define CPU_IREGION_BASE            CFG_IREGION_BASE_ADDR
#endif
#define __IREGION_BASEADDR          (CPU_IREGION_BASE)

// ECLIC Configuration
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
#ifdef CFG_HAS_PLIC
#define __PLIC_PRESENT              1
#define __PLIC_INTNUM               (CFG_IRQ_NUM + 1)
#else
#define __PLIC_PRESENT              0
#endif
#define __PLIC_BASEADDR             (__IREGION_BASEADDR + IREGION_PLIC_OFS)

// CPU System Timer Configuration
#if defined(CFG_TMR_PRIVATE) || defined(CFG_TMR_BASE_ADDR)
#define __SYSTIMER_PRESENT          1
#else
#define __SYSTIMER_PRESENT          0
#endif
#ifdef CFG_TMR_BASE_ADDR
#define __SYSTIMER_BASEADDR         (CFG_TMR_BASE_ADDR)
#else
#define __SYSTIMER_BASEADDR         (__IREGION_BASEADDR + IREGION_TIMER_OFS)
#endif
#define __CLINT_TIMER_BASEADDR      (__SYSTIMER_BASEADDR + 0x1000)

// CIDU Configuration
#ifdef CFG_HAS_IDU
#define __CIDU_PRESENT              1
#else
#define __CIDU_PRESENT              0
#endif
#define __CIDU_BASEADDR             (__IREGION_BASEADDR + IREGION_IDU_OFS)

// SMP & CC Configuration
#ifdef CFG_HAS_SMP
#define __SMPCC_PRESENT             1
#else
#define __SMPCC_PRESENT             0
#endif
#define __SMPCC_BASEADDR            (__IREGION_BASEADDR + IREGION_SMP_OFS)

// PMP Configuration
#ifdef CFG_HAS_PMP
#define __PMP_PRESENT               1
#define __PMP_ENTRY_NUM             CFG_PMP_ENTRY_NUM
#else
#define __PMP_PRESENT               0
#define __PMP_ENTRY_NUM             0
#endif

// TEE/sPMP Configuration
#ifdef CFG_HAS_TEE
#define __TEE_PRESENT               1
#define __SPMP_PRESENT              1
#define __SPMP_ENTRY_NUM            CFG_PMP_ENTRY_NUM
#else
#define __TEE_PRESENT               0
#define __SPMP_PRESENT              0
#define __SPMP_ENTRY_NUM            0
#endif

// ICache Configuration
#ifdef CFG_HAS_ICACHE
#define __ICACHE_PRESENT            1
#else
#define __ICACHE_PRESENT            0
#endif

// DCache Configuration
#ifdef CFG_HAS_DCACHE
#define __DCACHE_PRESENT            1
#else
#define __DCACHE_PRESENT            0
#endif

// CCM Configuration
#ifdef CFG_HAS_IOCC
#define __CCM_PRESENT               1
#else
#define __CCM_PRESENT               0
#endif

// NICE Configuration
#ifdef CFG_HAS_NICE
#define __NICE_PRESENT              1
#else
#define __NICE_PRESENT              0
#endif

// VNICE Configuration
#ifdef CFG_HAS_VNICE
#define __VNICE_PRESENT             1
#else
#define __VNICE_PRESENT             0
#endif

#ifndef __INC_INTRINSIC_API
#define __INC_INTRINSIC_API         0
#endif
#define __Vendor_SysTickConfig      0
#define __Vendor_EXCEPTION          0

#ifndef __HARTID_OFFSET
#define __HARTID_OFFSET             0
#endif

#ifndef BOOT_HARTID
#define BOOT_HARTID                 0
#endif

/*============================================================================
 * IPRO6 SoC Core ID Definitions
 *============================================================================*/
#define CORE_ID_ADDRESS (0xF0000000)
#define CORE_ID_M0      (0xE9070000)
#define CORE_ID_D0      (0xDEAD5500)
#define CORE_ID_LP      (0xDEADE902)

#define CORE_M0_JTAG_TCK_PIN  (GLB_GPIO_PIN_27)
#define CORE_M0_JTAG_TMS_PIN  (GLB_GPIO_PIN_28)
#define CORE_M0_JTAG_TCK_FUNC (GPIO_FUN_M_CJTAG)
#define CORE_M0_JTAG_TMS_FUNC (GPIO_FUN_M_CJTAG)

/* IRQ_NUM_BASE: SoC IRQs start after CPU internal IRQs (same as IPRO7) */
#define IRQ_NUM_BASE 19

typedef enum {
    /* =======================================  Core Specific Interrupt Numbers  ======================================== */
    /* Same as IPRO7 CPU internal IRQ layout */
    Reserved0_IRQn            =   0,              /*!<  Internal reserved */
    Reserved1_IRQn            =   1,              /*!<  Internal reserved */
    Reserved2_IRQn            =   2,              /*!<  Internal reserved */
    CLIC_INT_SFT              =   3,              /*!<  CPU Software interrupt */
    SysTimerSW_IRQn           =   3,              /*!<  System Timer SW interrupt */
    Reserved3_IRQn            =   4,              /*!<  Internal reserved */
    Reserved4_IRQn            =   5,              /*!<  Internal reserved */
    Reserved5_IRQn            =   6,              /*!<  Internal reserved */
    CLIC_INT_TMR              =   7,              /*!<  CPU Timer interrupt */
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

    /* IPRO6 SoC IRQ Mapping (IRQ_NUM_BASE + offset) */
    BMX_MCU_BUS_ERR_IRQn = IRQ_NUM_BASE + 0,                    /*!< [0] bus_err Interrupt                                             */
    DBI_IRQn = IRQ_NUM_BASE + 1,                                /*!< [1] DBI Interrupt                                                 */
    QDID_IRQn = IRQ_NUM_BASE + 2,                               /*!< [2] QDID Interrupt                                                */
    SDU_SOFT_RST_IRQn = IRQ_NUM_BASE + 3,                       /*!< [3] SDU software reset Interrupt                                  */
    /* IRQ 4 Reserved */
    RF_TOP_INT0_IRQn = IRQ_NUM_BASE + 5,                        /*!< [5] rf_top[0] Interrupt                                           */
    RF_TOP_INT1_IRQn = IRQ_NUM_BASE + 6,                        /*!< [6] rf_top[1] Interrupt                                           */
    SMID_IRQn = IRQ_NUM_BASE + 7,                               /*!< [7] SMID Interrupt                                                */
    WIFI_TBTT_SLEEP_IRQn = IRQ_NUM_BASE + 8,                    /*!< [8] wifi_tbtt_sleep Interrupt                                     */
    SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn = IRQ_NUM_BASE + 9,  /*!< [9] sec non-secure Interrupt                                      */
    SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn = IRQ_NUM_BASE + 10, /*!< [10] sec secure Interrupt                                         */
    SEC_ENG_ID1_CDET_IRQn = IRQ_NUM_BASE + 11,                  /*!< [11] cdet non-secure Interrupt                                    */
    SEC_ENG_ID0_CDET_IRQn = IRQ_NUM_BASE + 12,                  /*!< [12] cdet secure Interrupt                                        */
    SF_CTRL_ID1_IRQn = IRQ_NUM_BASE + 13,                       /*!< [13] sf non-secure Interrupt                                      */
    SF_CTRL_ID0_IRQn = IRQ_NUM_BASE + 14,                       /*!< [14] sf secure Interrupt                                          */
    DMA0_ALL_IRQn = IRQ_NUM_BASE + 15,                          /*!< [15] DMA Interrupt                                                */
    DVP2BUS_INT0_IRQn = IRQ_NUM_BASE + 16,                      /*!< [16] dvp2axiA Interrupt                                           */
    DVP2BUS_INT1_IRQn = IRQ_NUM_BASE + 17,                      /*!< [17] dvp2axiB Interrupt                                           */
    JCOMB_IRQn = IRQ_NUM_BASE + 18,                             /*!< [18] jcomb Interrupt                                              */
    WIFI_TBTT_WAKEUP_IRQn = IRQ_NUM_BASE + 19,                  /*!< [19] wifi_tbtt_wakeup Interrupt                                   */
    IRRX_IRQn = IRQ_NUM_BASE + 20,                              /*!< [20] irrx Interrupt                                               */
    USB_IRQn = IRQ_NUM_BASE + 21,                               /*!< [21] USB Interrupt                                                */
    SPI2DVP_IRQn = IRQ_NUM_BASE + 22,                           /*!< [22] spi2dvp Interrupt                                            */
    /* IRQ 23 Reserved (1'b0) */
    EMAC_IRQn = IRQ_NUM_BASE + 24,                              /*!< [24] EMAC Interrupt                                               */
    PMAC_IRQn = EMAC_IRQn,
    GPADC_DMA_IRQn = IRQ_NUM_BASE + 25,                         /*!< [25] gpadc_dma Interrupt                                          */
    EFUSE_IRQn = IRQ_NUM_BASE + 26,                             /*!< [26] efuse_int Interrupt                                          */
    SPI0_IRQn = IRQ_NUM_BASE + 27,                              /*!< [27] spi_0 Interrupt                                              */
    UART0_IRQn = IRQ_NUM_BASE + 28,                             /*!< [28] uart_0 Interrupt                                             */
    UART1_IRQn = IRQ_NUM_BASE + 29,                             /*!< [29] uart_1 Interrupt                                             */
    SPI1_IRQn = IRQ_NUM_BASE + 30,                              /*!< [30] spi_1 Interrupt                                              */
    GPIO_DMA_IRQn = IRQ_NUM_BASE + 31,                          /*!< [31] gpio_dma Interrupt                                           */
    I2C0_IRQn = IRQ_NUM_BASE + 32,                              /*!< [32] i2c_0 Interrupt                                              */
    PWM_IRQn = IRQ_NUM_BASE + 33,                               /*!< [33] PWM Interrupt                                                */
    /* IRQ 34-35 Reserved (1'b0) */
    TIMER0_CH0_IRQn = IRQ_NUM_BASE + 36,                        /*!< [36] timer_2 Interrupt                                            */
    TIMER0_CH1_IRQn = IRQ_NUM_BASE + 37,                        /*!< [37] timer_3 Interrupt                                            */
    TIMER0_WDT_IRQn = IRQ_NUM_BASE + 38,                        /*!< [38] WDT Interrupt                                                */
    I2C1_IRQn = IRQ_NUM_BASE + 39,                              /*!< [39] i2c_1 Interrupt                                              */
    I2S_IRQn = IRQ_NUM_BASE + 40,                               /*!< [40] i2s_0 Interrupt                                              */
    ANA_OCP_OUT_TO_CPU_0_IRQn = IRQ_NUM_BASE + 41,              /*!< [41] ana_ocp_out_to_cpu[0] Interrupt                              */
    ANA_OCP_OUT_TO_CPU_1_IRQn = IRQ_NUM_BASE + 42,              /*!< [42] ana_ocp_out_to_cpu[1] Interrupt                              */
    XTAL_RDY_SCAN_IRQn = IRQ_NUM_BASE + 43,                     /*!< [43] xtal_rdy_scan Interrupt                                      */
    GPIO_INT0_IRQn = IRQ_NUM_BASE + 44,                         /*!< [44] gpio_irq Interrupt                                           */
    DM_IRQn = IRQ_NUM_BASE + 45,                                /*!< [45] dm_irq Interrupt                                             */
    BT_IRQn = IRQ_NUM_BASE + 46,                                /*!< [46] bt_irq Interrupt                                             */
    M154_REQ_ACK_IRQn = IRQ_NUM_BASE + 47,                      /*!< [47] m154_req_enh_ack_int Interrupt                               */
    M154_INT_IRQn = IRQ_NUM_BASE + 48,                          /*!< [48] m154_int Interrupt                                           */
    M154_AES_IRQn = IRQ_NUM_BASE + 49,                          /*!< [49] m154_aes_int Interrupt                                       */
    PDS_WAKEUP_IRQn = IRQ_NUM_BASE + 50,                        /*!< [50] pds_int Interrupt                                            */
    HBN_OUT0_IRQn = IRQ_NUM_BASE + 51,                          /*!< [51] hbn_irq_out[0] Interrupt                                     */
    HBN_OUT1_IRQn = IRQ_NUM_BASE + 52,                          /*!< [52] hbn_irq_out[1] Interrupt                                     */
    BOD_IRQn = IRQ_NUM_BASE + 53,                               /*!< [53] bod_out Interrupt                                            */
    WIFI_IRQn = IRQ_NUM_BASE + 54,                              /*!< [54] wifi_to_cpu_irq Interrupt                                    */
    BZ_PHY_INT_IRQn = IRQ_NUM_BASE + 55,                        /*!< [55] bz_phy_int Interrupt                                         */
    BLE_IRQn = IRQ_NUM_BASE + 56,                               /*!< [56] ble_int Interrupt                                            */
    MAC_INT_TIMER_IRQn = IRQ_NUM_BASE + 57,                     /*!< [57] proc_irq[0] Interrupt                                        */
    MAC_INT_MISC_IRQn = IRQ_NUM_BASE + 58,                      /*!< [58] proc_irq[1] Interrupt                                        */
    MAC_INT_RX_TRIGGER_IRQn = IRQ_NUM_BASE + 59,                /*!< [59] proc_irq[2] Interrupt                                        */
    MAC_INT_TX_TRIGGER_IRQn = IRQ_NUM_BASE + 60,                /*!< [60] proc_irq[3] Interrupt                                        */
    MAC_INT_GEN_IRQn = IRQ_NUM_BASE + 61,                       /*!< [61] proc_irq[4] Interrupt                                        */
    MAC_INT_PROT_TRIGGER_IRQn = IRQ_NUM_BASE + 62,              /*!< [62] proc_irq[5] Interrupt                                        */
    WIFI_IPC_IRQn = IRQ_NUM_BASE + 63,                          /*!< [63] host_irq Interrupt                                           */
    IRQn_LAST,
} IRQn_Type;

/* Exception Code Definitions (same as IPRO7) */
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

/* ========================================  Timer Configuration  ======================================== */

#define RTC_FREQ                  (2000000UL)
#define SOC_TIMER_FREQ            (1000000UL)
// mtime_toggle_cnt=32 divides XCLK(RC32M=32MHz) to 1MHz
// SOC_TIMER_TICK_PER_US = 1MHz / 1000000 = 1
#define SOC_TIMER_TICK_PER_US     (1UL)

/* IPRO6 Memory Map
 * OCRAM: 0x21000000, 320KB (non-cacheable)
 *        0x11000000, 320KB (cacheable remap)
 * WRAM:  0x21050000, 160KB (non-cacheable)
 *        0x11050000, 160KB (cacheable remap)
 * XIP:   0x30000000, 16MB  (non-cacheable)
 *        0x10000000, 16MB  (cacheable remap)
 */
#define IPRO6_OCRAM_BASE                    (0x21000000)
#define IPRO6_OCRAM_END                     (0x21000000 + 320 * 1024)
#define IPRO6_OCRAM_CACHEABLE_BASE          (0x11000000)
#define IPRO6_OCRAM_CACHEABLE_END           (0x11000000 + 320 * 1024)

#define IPRO6_WRAM_BASE                     (0x21050000)
#define IPRO6_WRAM_END                      (0x21050000 + 160 * 1024)
#define IPRO6_WRAM_CACHEABLE_BASE           (0x11050000)
#define IPRO6_WRAM_CACHEABLE_END            (0x11050000 + 160 * 1024)

#define IPRO6_FLASH_XIP_BASE                (0x30000000)
#define IPRO6_FLASH_XIP_END                 (0x30000000 + 16 * 1024 * 1024)
#define IPRO6_FLASH_XIP_CACHEABLE_BASE      (0x10000000)
#define IPRO6_FLASH_XIP_CACHEABLE_END       (0x10000000 + 16 * 1024 * 1024)

#define IPRO6_ALLRAM_BASE                   (0x21000000)
#define IPRO6_ALLRAM_END                    (0x21000000 + 320 * 1024 + 160 * 1024)
#define IPRO6_ALLRAM_CACHEABLE_BASE         (0x11000000)
#define IPRO6_ALLRAM_CACHEABLE_END          (0x11000000 + 320 * 1024 + 160 * 1024)

/* IPRO6 peripherals base address */
#define GLB_BASE         ((uint32_t)0x30000000)
#define RF_BASE          ((uint32_t)0x30001000)
#define MIX_BASE         ((uint32_t)0x30001000)
#define GPIP_BASE        ((uint32_t)0x30002000)
#define PHY_BASE         ((uint32_t)0x30003000)
#define AGC_BASE         ((uint32_t)0x30004000)
#define SEC_DBG_BASE     ((uint32_t)0x30005000)
#define DBG_BASE         ((uint32_t)0x30005000)
#define TZ1_BASE         ((uint32_t)0x30006000)
#define TZC_SEC_BASE     ((uint32_t)0x30006000)
#define TZ2_BASE         ((uint32_t)0x30007000)
#define TZC_NSEC_BASE    ((uint32_t)0x30007000)
#define CCI_BASE         ((uint32_t)0x30008000)
#define MCU_MISC_BASE    ((uint32_t)0x30009000)
#define L1C_BASE         ((uint32_t)0x30009000)
#define UART0_BASE       ((uint32_t)0x3000A000)
#define UART1_BASE       ((uint32_t)0x3000B000)
#define EFUSE_BASE       ((uint32_t)0x3000C000)
#define EF_DATA_BASE     ((uint32_t)0x3000C000)
#define EF_CTRL_BASE     ((uint32_t)0x3000C000)
#define SPI_BASE         ((uint32_t)0x3000D000)
#define I2C0_BASE        ((uint32_t)0x3000E000)
#define I2C1_BASE        ((uint32_t)0x3000F000)
#define PWM_BASE         ((uint32_t)0x30010000)
#define TIMER_BASE       ((uint32_t)0x30011000)
#define CKS_BASE         ((uint32_t)0x30012000)
#define I2S_BASE         ((uint32_t)0x30013000)
#define USB_PHY_BASE     ((uint32_t)0x30014000)
#define QDID_BASE        ((uint32_t)0x30015000)
#define DBI_BASE         ((uint32_t)0x30016000)
#define MM_MISC_BASE     ((uint32_t)0x30017000)
#define DVP2AXI0_BASE    ((uint32_t)0x30018000)
#define DVP2AXI1_BASE    ((uint32_t)0x30019000)
#define MJPEG_BASE       ((uint32_t)0x3001A000)

#define SEC_ENG_BASE     ((uint32_t)0x30080000)
#define DMA_BASE         ((uint32_t)0x30081000)
#define QSPI_BASE        ((uint32_t)0x30082000)
#define SF_CTRL_BASE     ((uint32_t)0x30082000)
#define SF_CTRL_BUF_BASE ((uint32_t)0x30082600)
#define EMAC_BASE        ((uint32_t)0x30083000)
#define SDU_BASE         ((uint32_t)0x30084000)
/* SMID = SDIO 3.0 / SD3.0 / eMMC device controller. It occupies the
 * SDU_BASE window; base 0x30084000 CONFIRMED on ipro6 silicon (P0 readback). */
#define SMID_BASE        SDU_BASE
#define USB_BASE         ((uint32_t)0x30085000)

#define PDS_BASE         ((uint32_t)0x3008E000)
#define HBN_BASE         ((uint32_t)0x3008F000)
#define AON_BASE         ((uint32_t)0x3008F000)
/* HBN RAM at 0x20090000 (peripherals stay at 0x3xxx; HBN RAM is the 0x2xxx view,
 * same split as IPRO7). This is the address the BROM jumps to on PDS wake
 * (ipro_bootrom_hbn_jump -> HBN_RSV1), and the address the vendor ipro6 boot2
 * links .hbn at — the wake firmware must be linked AND executed here so the
 * bootrom-direct jump and PC-relative refs resolve correctly. */
#define HBN_RAM_BASE     ((uint32_t)0x20090000)

/* Memory regions */
#define ROM_BASE         ((uint32_t)0x30FE0000)
#define OCRAM_BASE       ((uint32_t)0x31000000)
#define WRAM_BASE        ((uint32_t)0x31050000)

/* Wireless subsystem */
#define WIFI_TOP_BASE    ((uint32_t)0x34800000)
#define BTDM_BASE        ((uint32_t)0x38000000)
#define EM1_BASE         ((uint32_t)0x38010000)
#define M154_BASE        ((uint32_t)0x3C000000)
#define M154_AES_BASE    ((uint32_t)0x3C000800)

typedef enum {
    IPRO_AHB_MASTER_CPU = 0x00,
    IPRO_AHB_MASTER_SDU = 0x01,
    IPRO_AHB_MASTER_SEC = 0x02,
    IPRO_AHB_MASTER_DMA = 0x03,
    IPRO_AHB_MASTER_CCI = 0x04,
    IPRO_AHB_MASTER_WIFI_PLATFORM = 0x05,
    IPRO_AHB_MASTER_WIFI_MAC_PHY = 0x06,
    IPRO_AHB_MASTER_WIFI_PHY = 0x07,
    IPRO_AHB_MASTER_MAX = 0x08,
} IPRO_AHB_Master_Type;

typedef enum {
    IPRO_AHB_SLAVE1_GLB = 0x00,    //cgen rsvd
    IPRO_AHB_SLAVE1_RF_TOP = 0x01, //swrst mix
    IPRO_AHB_SLAVE1_GPIP = 0x02,
    IPRO_AHB_SLAVE1_SEC_DBG = 0x03,
    IPRO_AHB_SLAVE1_SEC_ENG = 0x04,
    IPRO_AHB_SLAVE1_TZ = 0x05,    //swrst tz1,cgen tz1+tz2
    IPRO_AHB_SLAVE1_RSVD6 = 0x06, //swrst tz2,
    IPRO_AHB_SLAVE1_EF_CTRL = 0x07,
    IPRO_AHB_SLAVE1_CCI = 0x08, //cgen rsvd
    IPRO_AHB_SLAVE1_L1C = 0x09, //cgen rsvd
    IPRO_AHB_SLAVE1_RSVD10 = 0x0A,
    IPRO_AHB_SLAVE1_SF_CTRL = 0x0B,
    IPRO_AHB_SLAVE1_DMA = 0x0C,
    IPRO_AHB_SLAVE1_SDU = 0x0D, //cgen rsvd
    IPRO_AHB_SLAVE1_PDS = 0x0E, //cgen rsvd
    IPRO_AHB_SLAVE1_RSVD15 = 0x0F,
    IPRO_AHB_SLAVE1_UART0 = 0x10,
    IPRO_AHB_SLAVE1_UART1 = 0x11,
    IPRO_AHB_SLAVE1_SPI = 0x12,
    IPRO_AHB_SLAVE1_I2C = 0x13,
    IPRO_AHB_SLAVE1_PWM = 0x14,
    IPRO_AHB_SLAVE1_TIMER = 0x15,
    IPRO_AHB_SLAVE1_IRR = 0x16,
    IPRO_AHB_SLAVE1_CKS = 0x17,
    IPRO_AHB_SLAVE1_QDEC = 0x18,
    IPRO_AHB_SLAVE1_KYS = 0x19,
    IPRO_AHB_SLAVE1_UART2 = 0x1A,
    IPRO_AHB_SLAVE1_RSVD27 = 0x1B,
    IPRO_AHB_SLAVE1_RSVD28 = 0x1C,
    IPRO_AHB_SLAVE1_RSVD29 = 0x1D,
    IPRO_AHB_SLAVE1_RSVD30 = 0x1E,
    IPRO_AHB_SLAVE1_RSVD31 = 0x1F,
    IPRO_AHB_SLAVE1_MAX = 0x20,
    IPRO_AHB_SLAVE1_GPADC = 0x21, /* not used for cgen and swrst */
    IPRO_AHB_SLAVE1_GPDAC = 0x22, /* not used for cgen and swrst */
    IPRO_AHB_SLAVE1_I2S = 0x23,   /* not used for cgen and swrst */
    IPRO_AHB_SLAVE1_CAM = 0x24,   /* not used for cgen and swrst */
} IPRO_AHB_Slave1_Type;

typedef enum {
    IPRO_AHB_SLAVE2_RSVD0 = 0x00,
    IPRO_AHB_SLAVE2_RSVD1 = 0x01,
    IPRO_AHB_SLAVE2_RSVD2 = 0x02,
    IPRO_AHB_SLAVE2_RSVD3 = 0x03,
    IPRO_AHB_SLAVE2_WIFI = 0x04,
    IPRO_AHB_SLAVE2_RSVD5 = 0x05,
    IPRO_AHB_SLAVE2_RSVD6 = 0x06,
    IPRO_AHB_SLAVE2_RSVD7 = 0x07,
    IPRO_AHB_SLAVE2_BT_BLE = 0x08,
    IPRO_AHB_SLAVE2_M154 = 0x09,
    IPRO_AHB_SLAVE2_BT_BLE2 = 0x0A,
    IPRO_AHB_SLAVE2_M1542 = 0x0B,
    IPRO_AHB_SLAVE2_RSVD12 = 0x0C,
    IPRO_AHB_SLAVE2_RSVD13 = 0x0D,
    IPRO_AHB_SLAVE2_RSVD14 = 0x0E,
    IPRO_AHB_SLAVE2_RSVD15 = 0x0F,
    IPRO_AHB_SLAVE2_EXT_EMI_MISC = 0x10,
    IPRO_AHB_SLAVE2_EXT_PSRAM0_CTRL = 0x11,
    IPRO_AHB_SLAVE2_EXT_PSRAM1_CTRL = 0x12,
    IPRO_AHB_SLAVE2_EXT_USB = 0x13,
    IPRO_AHB_SLAVE2_EXT_MIX2 = 0x14,
    IPRO_AHB_SLAVE2_EXT_AUDIO = 0x15,
    IPRO_AHB_SLAVE2_EXT_SDH = 0x16,
    IPRO_AHB_SLAVE2_EXT_EMAC = 0x17,
    IPRO_AHB_SLAVE2_EXT_DMA2 = 0x18,
    IPRO_AHB_SLAVE2_EXT_RSVD25 = 0x19,
    IPRO_AHB_SLAVE2_EXT_RSVD26 = 0x1A,
    IPRO_AHB_SLAVE2_EXT_RSVD27 = 0x1B,
    IPRO_AHB_SLAVE2_MAX = 0x1C,
} IPRO_AHB_Slave2_Type;

typedef enum {
    IPRO_AHB_SEC_ENG_AES0 = 0,
    IPRO_AHB_SEC_ENG_AES1,
    IPRO_AHB_SEC_ENG_SHA0,
    IPRO_AHB_SEC_ENG_SHA1,
} IPRO_AHB_Sec_Eng_Type;

typedef enum {
    IPRO_AHB_DMA0_CH0 = 0,
    IPRO_AHB_DMA0_CH1,
    IPRO_AHB_DMA0_CH2,
    IPRO_AHB_DMA0_CH3,
    IPRO_AHB_DMA0_CH4,
    IPRO_AHB_DMA0_CH5,
    IPRO_AHB_DMA0_CH6,
    IPRO_AHB_DMA0_CH7,
} IPRO_AHB_DMA0_CHNL_Type;

#ifndef NULL
#define NULL ((void*)0)
#endif

#include <stdint.h>

/* IPRO7 RISC-V CPU Core includes (same CPU as IPRO7) */
#include "core_compatiable.h"
#include "core_feature_base.h"
#include "core_feature_cache.h"
#include "core_feature_eclic.h"
#include "core_feature_timer.h"
#include "riscv_encoding.h"

#endif // __IPRO6_H__
