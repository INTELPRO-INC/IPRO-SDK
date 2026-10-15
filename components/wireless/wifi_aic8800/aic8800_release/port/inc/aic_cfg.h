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

#ifndef _AIC_CFG_H_
#define _AIC_CFG_H_

/* ============================================================================
 * AIC8800 统一配置头文件 — Host 与 Platform 层共用
 *
 * 用法:
 *   1. 功能裁剪: 取消下方 AIC_DISABLE_* 宏的注释即可禁用对应功能
 *   2. 其他宏: 均为驱动默认配置，通常无需修改
 *   3. 源文件裁剪: 配合 aic_cfg.mk 中的 Make 开关一同使用
 *
 * 注意: AP 与 STA 为基础功能，不可裁剪
 * ============================================================================
 */

/* ============================================================================
 * 功能裁剪 — RTOS 空间敏感场景
 * 在 aic_cfg.mk 中通过 AIC_FEATURE_P2P / AIC_FEATURE_BT 控制，
 * Makefile 自动映射为 -D 编译宏传入此处
 * ============================================================================ */


/* ============================================================================
 * 芯片型号
 *
 * 由 aic_cfg.mk 通过 -D 标志控制，此处仅作为默认值兖底。
 * AIC_CHIP_MODEL 在 aic_cfg.mk 中设置：D80 / D80N / D80N_U02C / DW / DC / DL(DC变体)
 * ============================================================================ */
#ifndef CONFIG_AIC8800D80
#ifndef CONFIG_AIC8800DW
#ifndef CONFIG_AIC8800DC
#ifndef CONFIG_AIC8800D80N
#ifndef CONFIG_AIC8800D80X2
// 若 Makefile 未指定，默认启用 D80
#define CONFIG_AIC8800D80
#endif
#endif
#endif
#endif
#endif


/* ============================================================================
 * 硬件配置
 * ============================================================================ */
// USE_5G 仅 CONFIG_AIC8800D80 / CONFIG_AIC8800D80N / CONFIG_AIC8800D80X2 芯片支持，由 aic_cfg.mk 通过 -DUSE_5G 控制
#if (defined(CONFIG_AIC8800D80) || defined(CONFIG_AIC8800D80N) || defined(CONFIG_AIC8800D80X2)) && !defined(USE_5G)
#define USE_5G
#endif
/* ---- Bus type selection (Plan B) --------------------------------------- *
 * AIC_BUS_TYPE selects the active transport bus:
 *   SDIO (default) → CONFIG_SDIO_SUPPORT
 *   USB            → CONFIG_USB_SUPPORT + CONFIG_AICWF_BUS_USB
 * Set via aic_cfg.mk (AIC_BUS_TYPE := SDIO|USB) or -DAIC_BUS_USB / -DAIC_BUS_SDIO.
 * When USB is selected, do NOT auto-define CONFIG_SDIO_SUPPORT.
 */
#if !defined(CONFIG_SDIO_SUPPORT) && !defined(CONFIG_USB_SUPPORT)
  #if defined(AIC_BUS_USB)
    #define CONFIG_USB_SUPPORT
    #ifndef CONFIG_AICWF_BUS_USB
    #define CONFIG_AICWF_BUS_USB
    #endif
  #else
    #define CONFIG_SDIO_SUPPORT
  #endif
#endif
#ifndef CONFIG_PMIC_SETTING
#define CONFIG_PMIC_SETTING
#endif
#ifndef CONFIG_LOAD_USERCONFIG
#define CONFIG_LOAD_USERCONFIG
#endif
#ifndef CONFIG_DPD
#define CONFIG_DPD
#endif
#ifndef CONFIG_FORCE_DPD_CALIB
#define CONFIG_FORCE_DPD_CALIB
#endif
#ifndef CONFIG_SDIO_BUS_PWRCTRL
#define CONFIG_SDIO_BUS_PWRCTRL
#endif
#ifndef CONFIG_SDIO_BUS_PWRCTRL_DYN
#define CONFIG_SDIO_BUS_PWRCTRL_DYN
#endif


/* ============================================================================
 * 固件加载
 *
 * CONFIG_LOAD_FW:
 *   0 = filemode   从文件系统加载
 *   1 = flashmode  从 Flash 加载
 *   2 = innermode  内嵌固件数组
 *
 * 注意: CONFIG_LOAD_FW 属于 platform 层固件来源决策，由 platform_config.mk
 * 通过 -DCONFIG_LOAD_FW=n 传入；host 层不依赖、也不在此提供默认值。
 * ============================================================================ */
#ifndef CONFIG_DOWNLOAD_FW
#define CONFIG_DOWNLOAD_FW
#endif


/* ============================================================================
 * 协议栈 / 软件配置
 * ============================================================================ */
#ifndef CONFIG_RWNX_FULLMAC
#define CONFIG_RWNX_FULLMAC
#endif
#ifndef CONFIG_LWIP
#define CONFIG_LWIP
#endif
#ifndef CONFIG_REORD_FORWARD_LIST
#define CONFIG_REORD_FORWARD_LIST
#endif
#ifndef CONFIG_MBEDTLS
#define CONFIG_MBEDTLS
#endif
#ifndef CONFIG_MBEDTLS_V3
#define CONFIG_MBEDTLS_V3
#endif
#ifndef CONFIG_DRIVER_EDCA
#define CONFIG_DRIVER_EDCA
#endif
#ifndef CONFIG_SOCK_SENDTO
#define CONFIG_SOCK_SENDTO
#endif
#ifndef CONFIG_SOCK_RTOS_AL
#define CONFIG_SOCK_RTOS_AL
#endif
#ifndef CONFIG_RTOS_RESLIST
#define CONFIG_RTOS_RESLIST
#endif
#ifndef SDIO_FC_NARB
#define SDIO_FC_NARB
#endif
//#define CONFIG_FILTER_TCP_ACK


/* ============================================================================
 * 调试
 * ============================================================================ */
#ifndef AIC_LOG_DEBUG_ON
#define AIC_LOG_DEBUG_ON
#endif

/* Compile-time minimum log severity (default: all levels).
 * Reduce ROM by setting a higher threshold in CFLAGS:
 *   -DCONFIG_LOG_LEVEL=3   → strip INF+VRB format strings & code
 *   -DCONFIG_LOG_LEVEL=2   → strip WRN+INF+VRB
 * Values: 1=CRT 2=ERR 3=WRN 4=INF 5=VRB
 *
 * Compile-time module filter (default: all modules).
 * Lop off unused modules in CFLAGS:
 *   -DCONFIG_LOG_MODULES=0x00000C00   → FHOST+FMAC only (bits 10,11) */
#ifndef CONFIG_DRIVER_ORM
#define CONFIG_DRIVER_ORM
#endif
#ifndef CONFIG_RTOS_TRACE
#define CONFIG_RTOS_TRACE        /* 当前代码无引用，预留 */
#endif


/* ============================================================================
 * 平台适配
 * ============================================================================ */
#ifndef PLATFORM_NO_FILE
#define PLATFORM_NO_FILE         /* RTOS/裸机环境无 stdio FILE */
#endif


/* ============================================================================
 * 基础功能 (AP / STA — 不可裁剪)
 * ============================================================================ */
#ifndef CONFIG_STATION
#define CONFIG_STATION
#endif
#ifndef CONFIG_SOFTAP
#define CONFIG_SOFTAP
#endif
#ifndef CONFIG_OFFCHANNEL
#define CONFIG_OFFCHANNEL        /* 底层 MAC 功能，AP/STA 均可能用到 */
#endif


/* ============================================================================
 * 可选功能 (P2P / BT — 由 aic_cfg.mk 的 AIC_FEATURE_P2P / AIC_FEATURE_BT 统一控制)
 * ============================================================================ */
/* P2P */
#ifdef AIC_FEATURE_P2P
#ifndef CONFIG_P2P_SUPPORT
#define CONFIG_P2P_SUPPORT
#endif
/*
 * 注意: build_config.h 检测 CONFIG_P2P_SUPPORT 后链式定义:
 *   CONFIG_P2P            → wpa_supplicant P2P 核心
 *   CONFIG_WIFI_DISPLAY   → Wi-Fi Display
 *   CONFIG_GAS            → Generic Advertisement Service
 * rwnx_config.h 检测 CONFIG_P2P_SUPPORT 后定义:
 *   NX_P2P               → LMAC P2P 支持
 *   CFG_P2P              → FMAC P2P 支持
 */
#endif /* AIC_FEATURE_P2P */

/* 蓝牙 */
#ifdef AIC_FEATURE_BT
#ifndef CONFIG_BT_SUPPORT
#define CONFIG_BT_SUPPORT
#endif
#if defined(CONFIG_SDIO_SUPPORT) && !defined(AIC_BT_BUS_USB)
#ifndef CONFIG_SDIO_BT
#define CONFIG_SDIO_BT
#endif
#endif
#endif /* AIC_FEATURE_BT */


#endif /* _AIC_CFG_H_ */
