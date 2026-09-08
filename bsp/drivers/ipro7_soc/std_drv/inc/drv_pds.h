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

#ifndef __DRV_PDS_H__
#define __DRV_PDS_H__

#include "pds_reg.h"
#include "drv_ef_ctrl.h"
#include "drv_aon.h"
#include "drv_hbn.h"
#include "drv_sflash.h"
#include "drv_common.h"

typedef struct
{
    uint8_t pd_en;
    uint8_t pu_en;
    uint8_t ie_en;
    uint8_t oe_en;
} PDS_GPIO_Cfg_Type;

typedef struct
{
    uint32_t PDS_RAM_CFG_0KB_16KB_CPU_RAM_RET1N  : 1;  /*!< [0] 0~16KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_16KB_32KB_CPU_RAM_RET1N : 1;  /*!< [1] 16~32KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_32KB_48KB_CPU_RAM_RET1N : 1;  /*!< [2] 32~48KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_48KB_64KB_CPU_RAM_RET1N : 1;  /*!< [3] 48~64KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_64KB_80KB_CPU_RAM_RET1N : 1;  /*!< [4] 64~80KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_RSV_5_7                 : 3;  /*!< [7:5]reserve */
    uint32_t PDS_RAM_CFG_0KB_16KB_CPU_RAM_RET2N  : 1;  /*!< [4] 0~16KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_16KB_32KB_CPU_RAM_RET2N : 1;  /*!< [5] 16~32KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_32KB_48KB_CPU_RAM_RET2N : 1;  /*!< [6] 32~48KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_48KB_64KB_CPU_RAM_RET2N : 1;  /*!< [7] 48~64KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_64KB_80KB_CPU_RAM_RET2N : 1;  /*!< [12] 64~80KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_RSV_13_15               : 3;  /*!< [15:13]reserve */
    uint32_t PDS_RAM_CFG_0KB_16KB_CPU_RAM_PGEN   : 1;  /*!< [8] 0~16KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_16KB_32KB_CPU_RAM_PGEN  : 1;  /*!< [9] 16~32KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_32KB_48KB_CPU_RAM_PGEN  : 1;  /*!< [10] 32~48KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_48KB_64KB_CPU_RAM_PGEN  : 1;  /*!< [11] 48~64KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_64KB_80KB_CPU_RAM_PGEN  : 1;  /*!< [20] 64~80KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_RSV_21_31               : 11; /*!< [31:21]reserve */
} PDS_RAM_CFG_Type;

typedef struct
{
    uint32_t pdsStart                : 1; /*!< [0]PDS Start */
    uint32_t sleepForever            : 1; /*!< [1]PDS sleep forever */
    uint32_t xtalForceOff            : 1; /*!< [2]Power off xtal force */
    uint32_t waitRC32mRdy   : 1; /*!< [3]wait XTAL Ready during before PDS Interrupt */
    uint32_t avdd14Off      : 1; /*!< [4]power down avdd14 during PDS */
    uint32_t bgSysOff                : 1; /*!< [5]power down bg_sys during PDS */
    uint32_t glbRstProtect  : 1; /*!< [6]avoid glb_reg reset by any reset */
    uint32_t puFlash                 : 1; /*!< [7]turn off Flash Power During PDS */
    uint32_t clkOff                  : 1; /*!< [8]gate clock during PDS (each pwr domain has its own control) */
    uint32_t memStby                 : 1; /*!< [9]mem_stby during PDS (each power domain can has its own control) */
    uint32_t swPuFlash               : 1; /*!< [10]SW Turn on Flash */
    uint32_t isolation               : 1; /*!< [11]Isolation during PDS (each power domain can has its own control) */
    uint32_t waitXtalRdy             : 1; /*!< [12]wait XTAL Ready during before PDS Interrupt */
    uint32_t pdsPwrOff               : 1; /*!< [13]Power off during PDS (each power domain can has its own control) */
    uint32_t xtalOff                 : 1; /*!< [14]xtal power down during PDS */
    uint32_t socEnbForceOn           : 1; /*!< [15]pds_soc_enb always active */
    uint32_t pdsRstSocEn             : 1; /*!< [16]pds_rst controlled by PDS */
    uint32_t pdsRC32mOn              : 1; /*!< [17]RC32M always on or RC32M on/off controlled by PDS state */
    uint32_t pdsLdoVselEn            : 1; /*!< [18]PDS "SLEEP" control LDO voltage enable */
    uint32_t rsv19          : 1; /*!< [19]Reserved */
    uint32_t xtalCntRC32kEn : 1; /*!< [20]HW Auto count RC32K to be XTAL Counter after PDS */
    uint32_t cpu0WfiMask             : 1; /*!< [21]pds start condition mask np_wfi */
    uint32_t ldo11Off                : 1; /*!< [22]power down ldo11 during PDS */
    uint32_t pdsCtlRfSel    : 1; /*!< [23]PDS control RF on/off */
    uint32_t xtalDegEn      : 1; /*!< [24]Enable Digital Deg */
    uint32_t bleWakeupReqEn : 1; /*!< [25]PDS auto send ble_wakeup_req to sleep_timer when exit PDS */
    uint32_t swGpioIsoMod   : 1; /*!< [26]SW force keep GPIO */
    uint32_t pdsClkOff      : 1; /*!< [27]Turn off PDS Clock , Set this bit to 0 to active PDS */
    uint32_t pdsGpioKeep    : 3; /*!< [30:28]enable or disable keep function */
    uint32_t pdsGpioIsoMod  : 1; /*!< [31]HW Keep GPIO at PDS Mode  */
} PDS_CTL_Type;

typedef struct
{
    uint32_t rsv0_7          : 8;  /*!< [7:0]reserve */
    uint32_t forceCpuPdsRst  : 1;  /*!< [8]manual force NP pds reset */
    uint32_t rsv9            : 1;  /*!< [9]reserve */
    uint32_t forceBzPdsRst   : 1;  /*!< [10]manual force BZ pds reset */
    uint32_t rsv11_15        : 5;  /*!< [15:11]reserve */
    uint32_t forceCpuGateClk : 1;  /*!< [16]manual force NP clock gated */
    uint32_t rsv17           : 1;  /*!< [17]reserve */
    uint32_t forceBzGateClk  : 1;  /*!< [18]manual force BZ clock gated */
    uint32_t rsv19_31        : 13; /*!< [31:19]reserve */
} PDS_CTL2_Type;

typedef struct
{
    uint32_t rsv0             : 1; /*!< [0]reserve */
    uint32_t forceMiscPwrOff  : 1; /*!< [1]manual force MISC pwr_off */
    uint32_t rsv2_6           : 5;  /*!< [6:2]reserve */
    uint32_t forceMiscPdsRst  : 1; /*!< [7]manual force MISC pds_rst */
    uint32_t rsv8_12          : 5;  /*!< [12:8]reserve */
    uint32_t forceMiscGateClk : 1; /*!< [13]manual force MISC gate_clk */
    uint32_t rsv14_26         : 13; /*!< [26:14]reserve */
    uint32_t BzIsoEn          : 1; /*!< [27]isolate BZ at PDS sleep */
    uint32_t BleIsoEn         : 1; /*!< [28]isolate BZ_BLE at PDS sleep */
    uint32_t rsv29            : 1; /*!< [29]reserve */
    uint32_t MiscIsoEn        : 1; /*!< [30]make misc isolated at PDS Sleep state */
    uint32_t rsv31            : 1; /*!< [31]reserve */
} PDS_CTL3_Type;

typedef struct
{
    uint32_t rsv0        : 1; /*!< [0]reserve */
    uint32_t cpuRst        : 1; /*!< [1] */
    uint32_t rsv2        : 1; /*!< [2]reserve */
    uint32_t cpuGateClk    : 1; /*!< [3] */
    uint32_t rsv4_12     : 9; /*!< [12:4]reserve */
    uint32_t BzRst         : 1; /*!< [13] */
    uint32_t rsv14       : 1; /*!< [14]reserve */
    uint32_t BzGateClk     : 1; /*!< [15] */
    /* The BZ_BLE domain — the BLE IP core itself — is controlled separately
     * from BZ (the PHY). These four bits were declared reserved, so they kept
     * their reset default of 1 and the core was power-gated and reset on every
     * PDS sleep: DEEPSLCNTL read back as zero afterwards, which meant the
     * deep-sleep enable written on the way in had been erased and the IP never
     * actually slept. */
    uint32_t BlePwrOff     : 1; /*!< [16]bz_ble power off at PDS sleep */
    uint32_t BleRst        : 1; /*!< [17]bz_ble reset at PDS sleep */
    uint32_t BleMemStby    : 1; /*!< [18]bz_ble RAM retention at PDS sleep */
    uint32_t BleGateClk    : 1; /*!< [19]bz_ble clock gated at PDS sleep */
    uint32_t rsv20_23    : 4; /*!< [23:20]reserve */
    uint32_t MiscPwrOff    : 1; /*!< [24] */
    uint32_t MiscRst       : 1; /*!< [25] */
    uint32_t rsv26       : 1; /*!< [26]reserve */
    uint32_t MiscGateClk   : 1; /*!< [27] */
    uint32_t rsv28_31    : 4; /*!< [30:28]reserve */
} PDS_CTL4_Type;

typedef struct
{
    PDS_CTL_Type pdsCtl;   /*!< PDS_CTL configuration */
    PDS_CTL2_Type pdsCtl2; /*!< PDS_CTL2 configuration */
    PDS_CTL3_Type pdsCtl3; /*!< PDS_CTL3 configuration */
    PDS_CTL4_Type pdsCtl4; /*!< PDS_CTL4 configuration */
} PDS_DEFAULT_LV_CFG_Type;

typedef enum {
    PDS_INT_WAKEUP = 0,    /*!< PDS wakeup interrupt(assert bit while wakeup, include PDS_Timer/...) */
    PDS_INT_RF_DONE = 2, /*!< PDS RF done interrupt */
    PDS_INT_MAX,           /*!< PDS int max number */
} PDS_INT_Type;

typedef enum {
    PDS_WAKEUP_SRC_HBN_IRQ_OUT = 0, /*!< wakeup trigger by hbn_irq_out[1:0] */
    PDS_WAKEUP_SRC_KYD_WAKEUP,      /*!< wakeup trigger by pds_kyd_wakeup */
    PDS_WAKEUP_SRC_GPIO_IRQ,        /*!< wakeup trigger by gpio_irq */
    PDS_WAKEUP_SRC_PDS_IO_INT,      /*!< wakeup trigger by irrx_int */
    PDS_WAKEUP_SRC_BLE_SLP_IRQ,     /*!< wakeup trigger by ble_slp_irq */
    PDS_WAKEUP_SRC_KYS_INT,         /*!< wakeup trigger by kys_int */
    PDS_WAKEUP_SRC_WDG_TIMEOUT,     /*!< wakeup trigger by pds_watchdog_timeout event */
} PDS_WAKEUP_SRC_Type;

typedef enum {
    PDS_GPIO_PIN_0 = 0,
    PDS_GPIO_PIN_1,
    PDS_GPIO_PIN_2,
    PDS_GPIO_PIN_3,
    PDS_GPIO_PIN_4,
    PDS_GPIO_PIN_5,
    PDS_GPIO_PIN_6,
    PDS_GPIO_PIN_7,
    PDS_GPIO_PIN_8,
    PDS_GPIO_PIN_9,
    PDS_GPIO_PIN_20,
    PDS_GPIO_PIN_21,
    PDS_GPIO_PIN_22,
    PDS_GPIO_PIN_23,
    PDS_GPIO_PIN_24,
    PDS_GPIO_PIN_25,
    PDS_GPIO_PIN_26,
    PDS_GPIO_PIN_27,
    PDS_GPIO_PIN_28,
    PDS_GPIO_PIN_29,
    PDS_GPIO_PIN_30,
    PDS_GPIO_PIN_31,
    PDS_GPIO_PIN_32,
    PDS_GPIO_PIN_33,
    PDS_GPIO_PIN_34,
    PDS_GPIO_PIN_35,
    PDS_GPIO_PIN_36,
    PDS_GPIO_PIN_37,
    PDS_GPIO_PIN_38,
    PDS_GPIO_PIN_39,
    PDS_GPIO_PIN_40,
    PDS_GPIO_PIN_41,
    PDS_GPIO_PIN_MAX,
} PDS_GPIO_Type;

typedef enum {
    PDS_GPIO_GROUP_SET_GPIO0_GPIO3,   /*!< gpio int set 0, gpio0  - gpio3  */
    PDS_GPIO_GROUP_SET_GPIO4_GPIO7,   /*!< gpio int set 1, goio4  - gpio7  */
    PDS_GPIO_GROUP_SET_GPIO8_GPIO9,   /*!< gpio int set 2, gpio8  - gpio9  */
    PDS_GPIO_GROUP_SET_GPIO20_GPIO23, /*!< gpio int set 3, gpio20 - gpio23 */
    PDS_GPIO_GROUP_SET_GPIO24_GPIO27, /*!< gpio int set 4, gpio24 - gpio27 */
    PDS_GPIO_GROUP_SET_GPIO28_GPIO31, /*!< gpio int set 5, gpio28 - gpio31 */
    PDS_GPIO_GROUP_SET_GPIO32_GPIO35, /*!< gpio int set 6, gpio32 - gpio35 */
    PDS_GPIO_GROUP_SET_GPIO36_GPIO39, /*!< gpio int set 7, gpio36 - gpio39 */
    PDS_GPIO_GROUP_SET_GPIO40_GPIO43, /*!< gpio int set 8, gpio40 - gpio43 */
} PDS_GPIO_GROUP_SET_Type;

typedef enum {
    PDS_RST_EVENT_BUS_RST,        /*!< hreset_n (Bus Reset) */
    PDS_RST_EVENT_HBN_PWR_ON_RST, /*!< pwr_rst_n (hbn power on reset) */
    PDS_RST_EVENT_PDS_RST,        /*!< pds_rst_n (pds reset) */
    PDS_RST_EVENT_MAX,            /*!< PDS rst event max number */
} PDS_RST_EVENT_Type;

typedef enum {
    PDS_RF_STS_OFF = 0,        /*!< 4'b0000 */
    PDS_RF_STS_PU_MBG = 1,     /*!< 4'b0001 */
    PDS_RF_STS_PU_LDO15RF = 3, /*!< 4'b0011 */
    PDS_RF_STS_PU_SFREG = 7,   /*!< 4'b0111 */
    PDS_RF_STS_BZ_EN_AON = 15, /*!< 4'b1111 */
} PDS_RF_STS_Type;

typedef enum {
    PDS_STS_IDLE = 0,            /*!< 4'b0000 */
    PDS_STS_ECG = 8,             /*!< 4'b1000 */
    PDS_STS_ERST = 12,           /*!< 4'b1100 */
    PDS_STS_EISO = 15,           /*!< 4'b1111 */
    PDS_STS_POFF = 7,            /*!< 4'b0111 */
    PDS_STS_PRE_BGON = 3,        /*!< 4'b0011 */
    PDS_STS_PRE_BGON1 = 1,       /*!< 4'b0001 */
    PDS_STS_BGON = 5,            /*!< 4'b0101 */
    PDS_STS_CLK_SW_32M = 4,      /*!< 4'b0100 */
    PDS_STS_PON_DCDC = 6,        /*!< 4'b0110 */
    PDS_STS_PON_LDO11_MISC = 14, /*!< 4'b1110 */
    PDS_STS_PON = 10,            /*!< 4'b1010 */
    PDS_STS_DISO = 2,            /*!< 4'b0010 */
    PDS_STS_DCG = 13,            /*!< 4'b1101 */
    PDS_STS_DRST = 11,           /*!< 4'b1011 */
    PDS_STS_WAIT_EFUSE = 9,      /*!< 4'b1001 */
} PDS_STS_Type;

typedef enum {
    PDS_AON_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE,  /*!< PDS level 0-7 mode HBN GPIO INT trigger type: sync falling edge trigger */
    PDS_AON_GPIO_INT_TRIGGER_SYNC_RISING_EDGE,   /*!< PDS level 0-7 mode HBN GPIO INT trigger type: sync rising edge trigger */
    PDS_AON_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL,     /*!< PDS level 0-7 mode HBN GPIO INT trigger type: sync low level trigger */
    PDS_AON_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL,    /*!< PDS level 0-7 mode HBN GPIO INT trigger type: sync high level trigger */
    PDS_AON_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE, /*!< PDS level 0-7 mode HBN GPIO INT trigger type: async falling edge trigger */
    PDS_AON_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE,  /*!< PDS level 0-7 mode HBN GPIO INT trigger type: async rising edge trigger */
    PDS_AON_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL,    /*!< PDS level 0-7 mode HBN GPIO INT trigger type: async low level trigger */
    PDS_AON_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL,   /*!< PDS level 0-7 mode HBN GPIO INT trigger type: async high level trigger */
} PDS_AON_GPIO_INT_Trigger_Type;

typedef struct
{
    uint8_t pdsLevel;                                 /*!< PDS level */
    uint8_t turnOffXtal32m;                           /*!< Wheather turn off XTAL32M */
    uint8_t turnOffDll;                               /*!< Wheather turn off DLL */
    uint8_t turnOffRF;                                /*!< Wheather turn off RF */
    uint8_t useXtal32k;                               /*!< Wheather use xtal 32K as 32K clock source,otherwise use rc32k */
    uint8_t pdsAonGpioWakeupSrc;                      /*!< PDS level 0-7,31 mode aon GPIO Wakeup source(HBN wakeup pin) */
    PDS_AON_GPIO_INT_Trigger_Type pdsAonGpioTrigType; /*!< PDS level 0-7,31 mode aon GPIO Triger type(HBN wakeup pin) */
    uint8_t powerDownFlash;                           /*!< Whether power down flash */
    uint8_t pdsHoldGpio;                              /*!< Whether pds hold gpio level */
    uint8_t turnOffFlashPad;                          /*!< Whether turn off flash pad(GPIO17-GPIO22, GPIO23-GPIO28) */
    uint8_t flashPinCfg;                              /*!< 0 ext flash 23-28, 1 internal flash 23-28, 2 internal flash 23-28, 3 ext flash 17-22 */
    uint8_t turnoffPLL;                               /*!< Whether trun off PLL */
    uint8_t xtalType;                                 /*!< XTal type, used when user choose turn off PLL, PDS will turn on when exit PDS mode */
    uint8_t flashContRead;                            /*!< Whether enable flash continue read */
    uint32_t sleepTime;                               /*!< PDS sleep time */
    SPI_Flash_Cfg_Type *flashCfg;                     /*!< Flash config pointer, used when power down flash */
    HBN_LDO_LEVEL_Type ldoLevel;                      /*!< LDO level */
    void (*preCbFun)(void);                           /*!< Pre callback function */
    void (*postCbFun)(void);                          /*!< Post callback function */
} PDS_APP_CFG_Type;

typedef struct
{
    int mbgPower;      /*!< PDS auto [31] MBG power */
    int ldo18rfPower;  /*!< PDS auto [30] LDO18RF power */
    int sfregPower;    /*!< PDS auto [29] SF_REG power */
    int pllPower;      /*!< PDS auto [28] PLL power */
    int cpu0Power;     /*!< PDS auto [19] NP power */
    int rc32mPower;    /*!< PDS auto [17] RC32M power */
    int xtalPower;     /*!< PDS auto [14] XTAL power */
    int allPower;      /*!< PDS auto [13] all power */
    int isoPower;      /*!< PDS auto [11] ISO power */
    int bzPower;       /*!< PDS auto [10] BZ power */
    int sramDisStanby; /*!< PDS auto [9] SRAM memory stanby disable */
    int cgPower;       /*!< PDS auto [8] CG power */
    int cpu1Power;     /*!< PDS auto [7] AP power */
    int usbPower;      /*!< PDS auto [3] USB power */
} PDS_AUTO_POWER_DOWN_CFG_Type;

typedef struct
{
    uint32_t vddcoreVol;             /*!< PDS auto [27:24] VDDCORE voltage, reference 0x4001F80C[27:24], recommended 0xA */
    int vddcoreVolEn;        /*!< PDS auto [18] VDDCORE voltage enable bit */
    int cpu0NotNeedWFI;      /*!< PDS auto [21] NP not need WFI to get in PDS mode */
    int cpu1NotNeedWFI;      /*!< PDS auto [20] AP not need WFI to get in PDS mode */
    int busReset;            /*!< PDS auto [16] bus reset bit, reset after wake up from PDS mode */
    int disIrqWakeUp;        /*!< PDS auto [15] disable IRQ request to wake up from PDS mode, except PDS counter IRQ */
    int powerOffXtalForever; /*!< PDS auto [2] power off xtal after get in PDS mode, and never power on xtal after wake up */
    int sleepForever;        /*!< PDS auto [1] sleep forever after get in PDS mode, need reset system to restart */
} PDS_AUTO_NORMAL_CFG_Type;

typedef enum {
    PDS_FORCE_NP,  /*!< PDS force NP */
    PDS_FORCE_RSV, /*!< rsv */
    PDS_FORCE_BZ,  /*!< PDS force BZ */
} PDS_FORCE_Type;

typedef enum {
    PDS_GPIO_INT_SYNC_FALLING_EDGE = 0,        /*!< GPIO interrupt sync mode, GPIO negedge pulse trigger interrupt */
    PDS_GPIO_INT_SYNC_RISING_EDGE = 1,         /*!< GPIO interrupt sync mode, GPIO posedge pulse trigger interrupt */
    PDS_GPIO_INT_SYNC_LOW_LEVEL = 2,           /*!< GPIO interrupt sync mode, GPIO negedge level trigger interrupt */
    PDS_GPIO_INT_SYNC_HIGH_LEVEL = 3,          /*!< GPIO interrupt sync mode, GPIO posedge level trigger interrupt */
    PDS_GPIO_INT_SYNC_RISING_FALLING_EDGE = 4, /*!< GPIO interrupt sync mode, GPIO posedge negedge pulse trigger interrupt */
    PDS_GPIO_INT_ASYNC_FALLING_EDGE = 8,       /*!< GPIO interrupt async mode, GPIO negedge pulse trigger interrupt */
    PDS_GPIO_INT_ASYNC_RISING_EDGE = 9,        /*!< GPIO interrupt async mode, GPIO posedge pulse trigger interrupt */
    PDS_GPIO_INT_ASYNC_LOW_LEVEL = 10,         /*!< GPIO interrupt async mode, GPIO negedge level trigger interrupt */
    PDS_GPIO_INT_ASYNC_HIGH_LEVEL = 11,        /*!< GPIO interrupt async mode, GPIO posedge level trigger interrupt */
} PDS_GPIO_INT_TRIG_Type;

#define PDS_LDO_MIN_PU_CNT        (25) /* LDO need 25 cycles to power up */
#define PDS_WARMUP_CNT            (38) /* LDO hw warmup compensation latency cycles */
#define PDS_WARMUP_LATENCY_CNT    (38) /* LDO hw warmup compensation latency cycles */
#define PDS_FORCE_PWR_OFF_OFFSET  (0)
#define PDS_FORCE_ISO_EN_OFFSET   (4)
#define PDS_FORCE_PDS_RST_OFFSET  (8)
#define PDS_FORCE_MEM_STBY_OFFSET (12)
#define PDS_FORCE_GATE_CLK_OFFSET (16)

#define PDS_GPIO_IS_GROUP_ID(pin) (pin >> 2)

#define PDS_AON_WAKEUP_GPIO_ALL   (0x1E)

int PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_Type pin, PDS_GPIO_Cfg_Type *cfg);
int PDS_GPIO_Write(PDS_GPIO_GROUP_SET_Type grp, uint32_t val);
int PDS_Set_GPIO_Pad_IntMask(int pin, int intMask);
int PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_GROUP_SET_Type grp, PDS_GPIO_INT_TRIG_Type trig);
int PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_GROUP_SET_Type grp);
int PDS_Set_All_GPIO_IntClear(void);
int PDS_Get_GPIO_Pad_IntStatus(PDS_GPIO_Type pin);
int PDS_Set_Flash_Pad_Pull_None(SF_Ctrl_Pin_Select pinCfg);
int PDS_Set_Flash_Pad_Pull_None_Fast(SF_Ctrl_Pin_Select pinCfg);
int PDS_Disable_GPIO_Keep(void);
int PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt);
int PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3);
int PDS_RAM_Config(PDS_RAM_CFG_Type *ramCfg);
int PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt);

int PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_Type intType, int enable);
int PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_Type intType);
int PDS_IntMask(PDS_INT_Type intType, int intMask);
int PDS_Get_IntStatus(PDS_INT_Type intType);
int PDS_IntClear(void);
int PDS_Get_PdsPllStstus(void);
int PDS_Get_PdsRfStstus(void);
int PDS_Get_PdsStstus(void);
int PDS_Wakeup_Src_En_DisableAll(void);
int PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_Type intType, int enable);
int PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_Type intType);
int PDS_Clear_Reset_Event(void);
int PDS_Get_Reset_Event(PDS_RST_EVENT_Type event);

int PDS_WAKEUP_IRQHandler_Install(void);
void PDS_Auto_Enable(PDS_AUTO_POWER_DOWN_CFG_Type *powerCfg, PDS_AUTO_NORMAL_CFG_Type *normalCfg, int enable);
void PDS_Manual_Force_Turn_Off(PDS_FORCE_Type domain);
void PDS_Manual_Force_Turn_On(PDS_FORCE_Type domain);
int PDS_Set_KYD_Matrix_Size(uint8_t col_size, uint8_t row_size);
int PDS_Set_KYD_Col_Value(uint8_t val);
int PDS_Set_KYD_Row_Pull(uint8_t en);
int PDS_Set_KYD_Wakeup_En(uint8_t en);
int PDS_Clear_PSRAM_Pad_Cfg(void);

#endif /* __DRV_PDS_H__ */
