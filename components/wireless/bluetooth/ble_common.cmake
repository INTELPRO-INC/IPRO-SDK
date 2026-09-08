#======================================================================
# Copyright (c) 2026 IntelPro Technology Co., Ltd.
# All rights reserved.
#
# This software and associated documentation files (the "Software") are
# proprietary and confidential to IntelPro Technology Co., Ltd..
#
# Access to and use of this Software is limited exclusively to authorized 
# customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
# valid license or non-disclosure agreement.
#
# Unauthorized copying, modification, distribution, or reverse engineering 
# of this Software, via any medium, is strictly prohibited without 
# prior written permission from IntelPro Technology Co., Ltd..
#
# SPDX-License-Identifier: Proprietary
#======================================================================

# ============================================================================
# BLE Common Configuration - ble_common.cmake
# ============================================================================
# Central configuration file for BLE controller variants and compile options.
# Based on bluetooth_bl/ble_common.cmake pattern.
#
# Usage:
#   1. Set PRIV_CONFIG_GEN_BLE to select variant (e.g., "link1", "link2", "link8")
#   2. Set CHIP to select platform (e.g., "ipro7", "ipro6")
#   3. Include this file in your CMakeLists.txt
#
#   Legacy mXsY names (m0s1, m1s1, m2s1, etc.) are supported with warnings
#
# Variant Naming Convention (linkN):
#   linkN[_suffix] - N indicates simultaneous connection count (unified pool)
#
#   IMPORTANT: The controller uses a unified connection pool architecture.
#              CONFIG_BT_CONN = total simultaneous connections (any role combination).
#              Each connection slot can be used as Central OR Peripheral role.
#
#   - link1_periph: 1 connection, peripheral-only (no central role)
#   - link1_periph_scan: 1 connection, peripheral + observer (scanner)
#   - link1: 1 connection, all roles (can be central OR peripheral) [default]
#   - link2: 2 connections, all roles (any mix: 2C+0P, 1C+1P, 0C+2P)
#   - link4: 4 connections, all roles (any central/peripheral combination)
#   - link8: 8 connections, all roles (any central/peripheral combination)
#   - uarthci: UART HCI transport mode (no host)
#
#   DEPRECATED: Old mXsY names (m0s1, m0s1s, m1s1, m2s1, m4s1, m8s1) are supported
#               with automatic mapping to new linkN names. Update your config to use
#               the new naming convention. See backward compatibility mapping below.
# ============================================================================

# ============================================================================
# Default PRIV_CONFIG_GEN_BLE Selection
# ============================================================================
# PRIV_CONFIG_GEN_BLE should be defined in variant directory's CMakeLists.txt
# or via CONFIG_BTBLECONTROLLER_LIB from Kconfig

if(NOT DEFINED PRIV_CONFIG_GEN_BLE)
    if(CONFIG_BT_CTLR_IPRO)
        if(DEFINED CONFIG_IPRO_BLE_CTLR_LIB)
            set(PRIV_CONFIG_GEN_BLE ${CONFIG_IPRO_BLE_CTLR_LIB})
        else()
            set(PRIV_CONFIG_GEN_BLE "link1_periph")
        endif()
    elseif(DEFINED CONFIG_BTBLECONTROLLER_LIB)
        set(PRIV_CONFIG_GEN_BLE ${CONFIG_BTBLECONTROLLER_LIB})
    else()
        # Default to link1 (1 connection, all roles)
        set(PRIV_CONFIG_GEN_BLE "link1")
    endif()
endif()

# ============================================================================
# Backward Compatibility: Map old mXsY names to new linkN names
# ============================================================================
if(DEFINED PRIV_CONFIG_GEN_BLE)
    # Extract base variant (before any _suffix like _leaudio, _53, _54)
    string(REGEX MATCH "^[^_]+" BASE_VARIANT "${PRIV_CONFIG_GEN_BLE}")
    string(REPLACE "${BASE_VARIANT}" "" SUFFIX_PART "${PRIV_CONFIG_GEN_BLE}")

    # Map old names to new names for backward compatibility
    set(VARIANT_MAPPED FALSE)
    if("${BASE_VARIANT}" STREQUAL "m0s1")
        set(PRIV_CONFIG_GEN_BLE "link1_periph${SUFFIX_PART}")
        set(VARIANT_MAPPED TRUE)
        message(WARNING "[BLE] Deprecated variant 'm0s1' mapped to 'link1_periph'. Please update your configuration to use the new linkN naming convention.")
    elseif("${BASE_VARIANT}" STREQUAL "m0s1s")
        set(PRIV_CONFIG_GEN_BLE "link1_periph_scan${SUFFIX_PART}")
        set(VARIANT_MAPPED TRUE)
        message(WARNING "[BLE] Deprecated variant 'm0s1s' mapped to 'link1_periph_scan'. Please update your configuration to use the new linkN naming convention.")
    elseif("${BASE_VARIANT}" STREQUAL "m1s1")
        set(PRIV_CONFIG_GEN_BLE "link1${SUFFIX_PART}")
        set(VARIANT_MAPPED TRUE)
        message(WARNING "[BLE] Deprecated variant 'm1s1' mapped to 'link1'. Please update your configuration to use the new linkN naming convention.")
    elseif("${BASE_VARIANT}" STREQUAL "m2s1")
        set(PRIV_CONFIG_GEN_BLE "link2${SUFFIX_PART}")
        set(VARIANT_MAPPED TRUE)
        message(WARNING "[BLE] Deprecated variant 'm2s1' mapped to 'link2'. Please update your configuration to use the new linkN naming convention.")
    elseif("${BASE_VARIANT}" STREQUAL "m4s1")
        set(PRIV_CONFIG_GEN_BLE "link4${SUFFIX_PART}")
        set(VARIANT_MAPPED TRUE)
        message(WARNING "[BLE] Deprecated variant 'm4s1' mapped to 'link4'. Please update your configuration to use the new linkN naming convention.")
    elseif("${BASE_VARIANT}" STREQUAL "m8s1")
        set(PRIV_CONFIG_GEN_BLE "link8${SUFFIX_PART}")
        set(VARIANT_MAPPED TRUE)
        message(WARNING "[BLE] Deprecated variant 'm8s1' mapped to 'link8'. Please update your configuration to use the new linkN naming convention.")
    elseif("${BASE_VARIANT}" STREQUAL "m0s1p")
        set(PRIV_CONFIG_GEN_BLE "link1_periph_pds${SUFFIX_PART}")
        set(VARIANT_MAPPED TRUE)
        message(WARNING "[BLE] Deprecated variant 'm0s1p' (IPRO6LE) mapped to 'link1_periph_pds'. Please update your configuration to use the new linkN naming convention.")
    elseif("${BASE_VARIANT}" STREQUAL "m1s1p")
        set(PRIV_CONFIG_GEN_BLE "link1_pds${SUFFIX_PART}")
        set(VARIANT_MAPPED TRUE)
        message(WARNING "[BLE] Deprecated variant 'm1s1p' (IPRO6LE) mapped to 'link1_pds'. Please update your configuration to use the new linkN naming convention.")
    endif()

    if(VARIANT_MAPPED)
        message(STATUS "[BLE] Variant remapped: ${BASE_VARIANT} -> ${PRIV_CONFIG_GEN_BLE}")
    endif()
endif()

# ============================================================================
# Parse variant name and compose if Kconfig booleans are set
# ============================================================================
# If user sets boolean flags in Kconfig, compose the variant name (only if not already present)
if(CONFIG_BTBLECONTROLLER_LE_AUDIO AND NOT "${PRIV_CONFIG_GEN_BLE}" MATCHES "_leaudio")
    set(PRIV_CONFIG_GEN_BLE "${PRIV_CONFIG_GEN_BLE}_leaudio")
endif()
# Don't append _53 if _54 is present (54 implies 53)
if(CONFIG_BTBLECONTROLLER_BT_53 AND NOT "${PRIV_CONFIG_GEN_BLE}" MATCHES "_53" AND NOT "${PRIV_CONFIG_GEN_BLE}" MATCHES "_54")
    set(PRIV_CONFIG_GEN_BLE "${PRIV_CONFIG_GEN_BLE}_53")
endif()
if(CONFIG_BTBLECONTROLLER_BT_54 AND NOT "${PRIV_CONFIG_GEN_BLE}" MATCHES "_54")
    set(PRIV_CONFIG_GEN_BLE "${PRIV_CONFIG_GEN_BLE}_54")
endif()

# Parse the variant string to detect features (supports direct LIB specification)
if("${PRIV_CONFIG_GEN_BLE}" MATCHES "_leaudio")
    set(CONFIG_BTBLECONTROLLER_LE_AUDIO 1)
endif()
if("${PRIV_CONFIG_GEN_BLE}" MATCHES "_53")
    set(CONFIG_BTBLECONTROLLER_BT_53 1)
endif()
if("${PRIV_CONFIG_GEN_BLE}" MATCHES "_54")
    set(CONFIG_BTBLECONTROLLER_BT_54 1)
    set(CONFIG_BTBLECONTROLLER_BT_53 1)  # 5.4 implies 5.3
endif()

# ============================================================================
# Default CHIP Selection
# ============================================================================
if(NOT DEFINED CHIP)
    if(CONFIG_IPRO7)
        set(CHIP "ipro7")
    elseif(CONFIG_IPRO6)
        set(CHIP "ipro6")
    elseif(CONFIG_IPRO6LE)
        set(CHIP "ipro6le")
    else()
        set(CHIP "ipro7")  # Default to IPRO7
    endif()
endif()

message(STATUS "[BLE] Platform: ${CHIP}, Variant: ${PRIV_CONFIG_GEN_BLE}")

# ============================================================================
# Controller Configuration Flags (set by variant selection)
# ============================================================================
# These flags are configured by variant and should not be manually changed.
#
# Key Architecture Concepts:
# - CONFIG_BT_CONN: Total simultaneous connections (unified pool)
# - Connection pool is role-agnostic: each slot can be Central OR Peripheral
# - CONFIG_BT_CENTRAL/PERIPHERAL: Enable/disable role support (not connection limits)
# - Example: CONFIG_BT_CONN=8 supports any mix (8C+0P, 4C+4P, 0C+8P, etc.)
#
# Flags:
# - CONFIG_BT_CONN: Total connection pool size
# - CONFIG_BT_ALLROLES: Enable all roles (shorthand for central+peripheral+observer+broadcaster)
# - CONFIG_BT_CENTRAL: Enable central (master) role
# - CONFIG_BT_PERIPHERAL: Enable peripheral (slave) role
# - CONFIG_BT_OBSERVER: Enable observer (scanner) role
# - CONFIG_BT_BROADCASTER: Enable broadcaster (advertiser) role
# ============================================================================

# ============================================================================
# IPRO7 Platform Variant Configurations
# ============================================================================
if("${CHIP}" STREQUAL "ipro7")
    # Default IPRO7 features
    set(CONFIG_BT 0)
    set(CONFIG_BT_BREDR 0)
    set(CONFIG_EM_SIZE 32)
    set(CONFIG_BLE_PDS 0)
# ============================================================================
# Controller CFG_* 定義的分派
# ============================================================================
#
# 🔴 本檔的 CFG_* 是為 **btblecontroller** 準備的，而且是 public 定義
# （會傳播到每一個元件），但它在 controller 選擇**之前**就被 include。
#
# ipro_ble_controller 自帶一整套 CFG_*（見 ipro_ble_controller/CMakeLists.txt，照抄原廠
# sourcelist.txt / SCompile 的推導），兩套會直接打架。實測衝突：
#
#     -DCFG_ACT=5 -DCFG_ACT=7   兩個都下，後者勝 → ACT 不是 ipro_ble_controller 要的值
#     -DCFG_ISO_CON=8 -DCFG_CIS -DCFG_BIS
#         → BLE_ISO_PRESENT=1 → rwip.c 去 include 不存在的 dma.h
#     -DCFG_ALLROLES -DCFG_CENTRAL
#         → 把 link1_periph（單連線、周邊）的範圍整個推翻
#
# 所以 CFG_* 走這個 macro，ipro_ble_controller 選中時一律不發。
# 非 CFG_ 的定義（CONFIG_BT_* / IPRO7 / ARCH_RISCV …）是 host 與平台共用的，
# 照常發送，不受影響。
# host（blestack）自己也引用了幾個 CFG_*，那些必須照常發送 ——
# 它們描述的是 host/controller **共用**的尺寸與功能，不是 controller 私有組態。
# 清單來自 grep blestack/src 的實際引用，不是猜的。
set(__BLE_HOST_CFG
    CFG_CON                     # → host config.h 的 CONFIG_BT_MAX_CONN
    CFG_BLE_TX_BUFF_DATA
    CFG_NVDS
    CFG_BLE_PDS
    CFG_BLE_STACK_DBG_PRINT
    CFG_BT_RESET
    CFG_IOT_SDK
)

macro(__ble_ctlr_cfg __def)
    string(REGEX REPLACE "^-D([A-Za-z_0-9]+).*$" "\\1" __cfg_name "${__def}")
    list(FIND __BLE_HOST_CFG ${__cfg_name} __cfg_idx)
    if(NOT CONFIG_BTDM_ENABLE AND
       ((NOT CONFIG_BT_CTLR_IPRO) OR (NOT __cfg_idx EQUAL -1)))
        __component_add_compile_definitions(${__def})
    endif()
endmacro()

    __ble_ctlr_cfg(-DCFG_DBG)

    # ------------------------------------------------------------------
    # Variant: link1_periph - Peripheral only (minimal)
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "link1_periph")
        set(CONFIG_BT_CONN 1)
        set(CONFIG_BLE_TX_BUFF_DATA 2)
        set(CONFIG_BLE_ACT_MAX 5)
        set(CONFIG_BT_ALLROLES 0)
        set(CONFIG_BT_CENTRAL 0)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 0)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_SMP 0)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE 0)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 1)
        set(CONFIG_ADV_EXTENSION 0)
        set(CONFIG_CIS 0)
        set(CONFIG_BIS 0)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 0)
    endif()

    # ------------------------------------------------------------------
    # Variant: link1_periph_scan - Peripheral + Observer (scanner)
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "link1_periph_scan")
        set(CONFIG_BT_CONN 1)
        set(CONFIG_BLE_TX_BUFF_DATA 2)
        set(CONFIG_BLE_ACT_MAX 5)
        set(CONFIG_BT_ALLROLES 0)
        set(CONFIG_BT_CENTRAL 0)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_SMP 0)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE 0)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 1)
        set(CONFIG_ADV_EXTENSION 0)
        set(CONFIG_CIS 0)
        set(CONFIG_BIS 0)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 0)
    endif()

    # ------------------------------------------------------------------
    # Variant: link1 - 1 connection, all roles (default)
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "link1")
        set(CONFIG_BT_CONN 1)
        set(CONFIG_BLE_TX_BUFF_DATA 2)
        set(CONFIG_BLE_ACT_MAX 7)
        set(CONFIG_BT_ALLROLES 1)
        set(CONFIG_BT_CENTRAL 1)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE 0)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
        set(CONFIG_ADV_EXTENSION 1)
        set(CONFIG_CIS 1)
        set(CONFIG_BIS 1)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 1)
    endif()

    # ------------------------------------------------------------------
    # Variant: link2 - 2 connections, all roles
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "link2")
        set(CONFIG_BT_CONN 2)
        set(CONFIG_BLE_TX_BUFF_DATA 4)
        set(CONFIG_BLE_ACT_MAX 7)
        set(CONFIG_BT_ALLROLES 1)
        set(CONFIG_BT_CENTRAL 1)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
        set(CONFIG_ADV_EXTENSION 1)
        set(CONFIG_CIS 1)
        set(CONFIG_BIS 1)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 1)
    endif()

    # ------------------------------------------------------------------
    # Variant: link4 - 4 connections, all roles
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "link4")
        set(CONFIG_BT_CONN 4)
        set(CONFIG_BLE_TX_BUFF_DATA 6)
        set(CONFIG_BLE_ACT_MAX 8)
        set(CONFIG_BT_ALLROLES 1)
        set(CONFIG_BT_CENTRAL 1)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
        set(CONFIG_ADV_EXTENSION 1)
        set(CONFIG_CIS 1)
        set(CONFIG_BIS 1)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 1)
    endif()

    # ------------------------------------------------------------------
    # Variant: link8 - 8 connections, all roles (any central/peripheral mix)
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "link8")
        set(CONFIG_BT_CONN 8)
        set(CONFIG_BLE_TX_BUFF_DATA 8)
        set(CONFIG_BLE_ACT_MAX 10)
        set(CONFIG_EM_SIZE 64)
        set(CONFIG_BT_ALLROLES 1)
        set(CONFIG_BT_CENTRAL 1)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
        set(CONFIG_ADV_EXTENSION 1)
        set(CONFIG_CIS 1)
        set(CONFIG_BIS 1)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 1)
    endif()

    # ------------------------------------------------------------------
    # Variant: le_audio - LE Audio optimized (legacy, use BTBLECONTROLLER_LE_AUDIO instead)
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "le_audio")
        set(CONFIG_BT_CONN 2)
        set(CONFIG_BLE_TX_BUFF_DATA 4)
        set(CONFIG_BLE_ACT_MAX 8)
        set(CONFIG_BT_ALLROLES 1)
        set(CONFIG_BT_CENTRAL 1)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
        set(CONFIG_ADV_EXTENSION 1)
        set(CONFIG_CIS 1)
        set(CONFIG_BIS 1)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 1)
        set(CONFIG_CFG_GAF 1)
    endif()

    # ------------------------------------------------------------------
    # Variant: uarthci - UART HCI transport (no host)
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "uarthci")
        set(CONFIG_BLE_HOST_DISABLE 1)
        set(CONFIG_BT_CONN 2)
        set(CONFIG_BLE_TX_BUFF_DATA 4)
        set(CONFIG_BLE_ACT_MAX 7)
        set(CONFIG_EM_SIZE 64)
        set(CONFIG_BT_ALLROLES 1)
        set(CONFIG_BT_CENTRAL 1)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
        set(CONFIG_ADV_EXTENSION 1)
        set(CONFIG_CIS 1)
        set(CONFIG_BIS 1)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 1)
    endif()

    # ------------------------------------------------------------------
    # Variant: default - Full featured (same as m1s1)
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "default")
        set(CONFIG_BT_CONN 2)
        set(CONFIG_BLE_TX_BUFF_DATA 4)
        set(CONFIG_BLE_ACT_MAX 7)
        set(CONFIG_BT_ALLROLES 1)
        set(CONFIG_BT_CENTRAL 1)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
        set(CONFIG_ADV_EXTENSION 1)
        set(CONFIG_CIS 1)
        set(CONFIG_BIS 1)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 1)
    endif()

    # ------------------------------------------------------------------
    # Variant: mfg - Manufacturing test mode
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "mfg")
        set(CONFIG_EM_SIZE 32)
        set(CONFIG_BLE_MFG 1)
        set(CONFIG_BLE_MFG_HCI_CMD 1)
        set(CONFIG_BT_CONN 1)
        set(CONFIG_BLE_TX_BUFF_DATA 2)
        set(CONFIG_BLE_ACT_MAX 5)
        set(CONFIG_BT_ALLROLES 0)
        set(CONFIG_BT_CENTRAL 0)
        set(CONFIG_BT_PERIPHERAL 0)
        set(CONFIG_BT_OBSERVER 0)
        set(CONFIG_BT_BROADCASTER 0)
        set(CONFIG_ADV_EXTENSION 0)
        set(CONFIG_CIS 0)
        set(CONFIG_BIS 0)
        set(CONFIG_CTE 0)
        set(CONFIG_LE_PWR_CTRL 0)
    endif()
endif()

# ============================================================================
# Feature Overlay Pass (applied after base variant selection)
# ============================================================================
# These overlays apply additional settings based on Kconfig boolean flags,
# regardless of which base variant was selected.

if(CONFIG_BTBLECONTROLLER_LE_AUDIO)
    set(CONFIG_CFG_GAF 1)
    set(CONFIG_CIS 1)
    set(CONFIG_BIS 1)
    set(CONFIG_ADV_EXTENSION 1)
    set(CONFIG_CFG_PER_ADV 1)
    set(CONFIG_CFG_PAST 1)
    message(STATUS "[BLE] LE Audio controller overlay applied (GAF/CIS/BIS/PER_ADV/PAST)")
endif()

if(CONFIG_BTBLECONTROLLER_BT_53)
    set(CONFIG_CFG_BT_53 1)
    set(CONFIG_CFG_LE_SUBRATING 1)
    set(CONFIG_CFG_CSSA 1)
    message(STATUS "[BLE] BLE 5.3 controller overlay applied (LE_SUBRATING/CSSA)")
endif()

if(CONFIG_BTBLECONTROLLER_BT_54)
    set(CONFIG_CFG_BT_54 1)
    set(CONFIG_CFG_BT_53 1)
    set(CONFIG_CFG_PAWR 1)
    set(CONFIG_CFG_PER_ADV 1)
    set(CONFIG_CFG_LE_SUBRATING 1)
    set(CONFIG_CFG_CSSA 1)
    message(STATUS "[BLE] BLE 5.4 controller overlay applied (PAWR/PER_ADV/LE_SUBRATING/CSSA)")
endif()


# ============================================================================
# IPRO6LE Platform Variant Configurations (Low Energy variant)
# ============================================================================
if("${CHIP}" STREQUAL "ipro6le")
    # Default IPRO6LE features - optimized for low power
    set(CONFIG_BT 0)
    set(CONFIG_BT_BREDR 0)
    set(CONFIG_EM_SIZE 8)
    set(CONFIG_BLE_PDS 1)
    set(CONFIG_ADV_EXTENSION 0)
    set(CONFIG_CIS 0)
    set(CONFIG_BIS 0)
    set(CONFIG_CTE 0)
    set(CONFIG_LE_PWR_CTRL 0)
    set(CONFIG_LONG_RANG 0)
    set(CONFIG_CLK_ACC 0)
    set(CONFIG_LE_PING 0)

    # ------------------------------------------------------------------
    # Variant: link1_periph_pds - Peripheral only with PDS
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "link1_periph_pds")
        set(CONFIG_BLE_PDS 1)
        set(CONFIG_BT_CONN 1)
        set(CONFIG_BLE_TX_BUFF_DATA 2)
        set(CONFIG_BLE_ACT_MAX 5)
        set(CONFIG_BT_ALLROLES 0)
        set(CONFIG_BT_CENTRAL 0)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 0)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
    endif()

    # ------------------------------------------------------------------
    # Variant: link1_pds - All roles with PDS
    # ------------------------------------------------------------------
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "link1_pds")
        set(CONFIG_BLE_PDS 1)
        set(CONFIG_BT_CONN 1)
        set(CONFIG_BLE_TX_BUFF_DATA 2)
        set(CONFIG_BLE_ACT_MAX 5)
        set(CONFIG_BT_ALLROLES 1)
        set(CONFIG_BT_CENTRAL 1)
        set(CONFIG_BT_PERIPHERAL 1)
        set(CONFIG_BT_OBSERVER 1)
        set(CONFIG_BT_BROADCASTER 1)
        set(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
        set(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
    endif()
endif()

# IPRO6S does not use the IPRO7 variant table above, but its unified controller
# still shares this file with the host.  Define the same CFG dispatcher when
# the platform-specific block did not do so.  Controller-private btblecontroller
# flags must not leak into either unified backend; the small allowlist remains
# public because the host consumes those sizes/policies too.
if(NOT COMMAND __ble_ctlr_cfg)
    set(__BLE_HOST_CFG
        CFG_CON
        CFG_BLE_TX_BUFF_DATA
        CFG_NVDS
        CFG_BLE_PDS
        CFG_BLE_STACK_DBG_PRINT
        CFG_BT_RESET
        CFG_IOT_SDK
    )

    macro(__ble_ctlr_cfg __def)
        string(REGEX REPLACE "^-D([A-Za-z_0-9]+).*$" "\\1" __cfg_name "${__def}")
        list(FIND __BLE_HOST_CFG ${__cfg_name} __cfg_idx)
        if(NOT CONFIG_BTDM_ENABLE AND
           ((NOT CONFIG_BT_CTLR_IPRO) OR (NOT __cfg_idx EQUAL -1)))
            __component_add_compile_definitions(${__def})
        endif()
    endmacro()
endif()


# ============================================================================
# Default Variable Initialization
# ============================================================================
# Set defaults for any variables not set by variant configuration

sdk_ifndef(CONFIG_BT 0)
sdk_ifndef(CONFIG_BT_BREDR 0)
sdk_ifndef(CONFIG_BLE 1)
sdk_ifndef(CONFIG_BT_CONN 1)
sdk_ifndef(CONFIG_BLE_TX_BUFF_DATA 2)
sdk_ifndef(CONFIG_BLE_ACT_MAX 7)
sdk_ifndef(CONFIG_EM_SIZE 32)

sdk_ifndef(CONFIG_BT_ALLROLES 1)
sdk_ifndef(CONFIG_BT_CENTRAL 1)
sdk_ifndef(CONFIG_BT_PERIPHERAL 1)
sdk_ifndef(CONFIG_BT_OBSERVER 1)
sdk_ifndef(CONFIG_BT_BROADCASTER 1)

sdk_ifndef(CONFIG_BT_SETTINGS 0)
sdk_ifndef(CONFIG_BLE_TP_SERVER 0)
sdk_ifndef(CONFIG_BLE_MULTI_ADV 0)
sdk_ifndef(CONFIG_BT_STACK_CLI 1)
sdk_ifndef(CONFIG_BLE_STACK_DBG_PRINT 1)
sdk_ifndef(CONFIG_BT_GATT_CLIENT 1)
sdk_ifndef(CONFIG_BT_DATA_LEN_UPDATE 0)

sdk_ifndef(CONFIG_DISABLE_BT_SMP 0)
sdk_ifndef(CONFIG_DISABLE_BT_HOST_PRIVACY 1)
sdk_ifndef(CONFIG_DISABLE_BT_ECC 0)
sdk_ifndef(CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE 0)
sdk_ifndef(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE 0)
sdk_ifndef(CONFIG_DISABLE_BLE_CONTROLLER_SEC_CON 0)

sdk_ifndef(CONFIG_ADV_EXTENSION 1)
sdk_ifndef(CONFIG_CIS 1)
sdk_ifndef(CONFIG_BIS 1)
sdk_ifndef(CONFIG_LONG_RANG 1)
sdk_ifndef(CONFIG_LE_PWR_CTRL 1)
sdk_ifndef(CONFIG_CTE 0)
sdk_ifndef(CONFIG_PHY_UPDATE 1)
sdk_ifndef(CONFIG_CLK_ACC 1)
sdk_ifndef(CONFIG_LE_PING 1)

sdk_ifndef(CONFIG_BT_MESH 0)
sdk_ifndef(CONFIG_BT_MESH_MODEL 0)
sdk_ifndef(CONFIG_BLE_HOST_DISABLE 0)
sdk_ifndef(CONFIG_HW_SEC_ENG_DISABLE 0)
sdk_ifndef(CONFIG_BLE_PDS 0)
sdk_ifndef(CONFIG_EM_HEAP_DISABLE 0)

# Host activity count (used by HOST_ACTIVITY_MAX in rwip_config.h)
# Default: connections + 2 for scan/adv activities
if(NOT DEFINED CONFIG_CFG_HL_ACT)
    math(EXPR CONFIG_CFG_HL_ACT "${CONFIG_BT_CONN} + 2")
endif()

# Resolving Address List size (used by BLE_RAL_MAX in rwip_config.h)
sdk_ifndef(CONFIG_CFG_RAL 3)

# ============================================================================
# FreeRTOS Integration
# ============================================================================
if(NOT DEFINED CONFIG_FREERTOS_DISABLE)
    __ble_ctlr_cfg(-DCFG_FREERTOS)
    # CFG_RTOS is checked by ble_prot_os.h to include FreeRTOS headers
    __ble_ctlr_cfg(-DCFG_RTOS)
    # CFG_OS is used for OS-aware code paths in controller
    __ble_ctlr_cfg(-DCFG_OS=1)
endif()

# ============================================================================
# Core BLE Compile Definitions
# ============================================================================
__ble_ctlr_cfg(-DCFG_BLE_ENABLE)
__ble_ctlr_cfg(-DCFG_BLE)
__component_add_compile_definitions(-DARCH_RISCV)

# IPRO BLE flag - enables on-chip HCI code paths in blestack
__component_add_compile_definitions(-DIPRO_BLE)

# Controller Embedded Definition (required for EMB_PRESENT = 1)
__ble_ctlr_cfg(-DCFG_BLE_EMB)

# BLE Role Definitions (CFG_* for controller internal use)
if(CONFIG_BT_CENTRAL)
    __ble_ctlr_cfg(-DCFG_CENTRAL)
endif()
if(CONFIG_BT_PERIPHERAL)
    __ble_ctlr_cfg(-DCFG_PERIPHERAL)
endif()
if(CONFIG_BT_OBSERVER)
    __ble_ctlr_cfg(-DCFG_OBSERVER)
endif()
if(CONFIG_BT_BROADCASTER)
    __ble_ctlr_cfg(-DCFG_BROADCASTER)
endif()
if(CONFIG_BT_ALLROLES)
    __ble_ctlr_cfg(-DCFG_ALLROLES)
endif()

# Platform-specific definitions
if("${CHIP}" STREQUAL "ipro7")
    __component_add_compile_definitions(-DIPRO7)
    __ble_ctlr_cfg(-DCFG_IPRO7)
elseif("${CHIP}" STREQUAL "ipro6")
    __component_add_compile_definitions(-DIPRO6)
    __ble_ctlr_cfg(-DCFG_IPRO6)
    __component_add_compile_definitions(-DBL616)
    __component_add_compile_definitions(-DBFLB_BLE)
elseif("${CHIP}" STREQUAL "ipro6le")
    __component_add_compile_definitions(-DIPRO6LE)
    __ble_ctlr_cfg(-DCFG_IPRO6LE)
endif()

# Allow per-app override of TX buffer count (with CONFIG_BLE_FORCE_SOURCE_BUILD)
if(DEFINED CONFIG_APP_BLE_TX_BUFF_DATA)
    set(CONFIG_BLE_TX_BUFF_DATA ${CONFIG_APP_BLE_TX_BUFF_DATA})
endif()

# Connection configuration
__ble_ctlr_cfg(-DCFG_CON=${CONFIG_BT_CONN})
__ble_ctlr_cfg(-DCFG_BLE_TX_BUFF_DATA=${CONFIG_BLE_TX_BUFF_DATA})
__ble_ctlr_cfg(-DCFG_ACT=${CONFIG_BLE_ACT_MAX})
__ble_ctlr_cfg(-DCFG_HL_ACT=${CONFIG_CFG_HL_ACT})
__ble_ctlr_cfg(-DCFG_RAL=${CONFIG_CFG_RAL})

# ============================================================================
# BLE Role Configuration
# ============================================================================
if(CONFIG_BT_ALLROLES)
    __component_add_compile_definitions(
        -DCONFIG_BT_ALLROLES
        -DCONFIG_BT_CENTRAL
        -DCONFIG_BT_OBSERVER
        -DCONFIG_BT_PERIPHERAL
        -DCONFIG_BT_BROADCASTER
    )
else()
    if(CONFIG_BT_CENTRAL)
        __component_add_compile_definitions(-DCONFIG_BT_CENTRAL)
    endif()
    if(CONFIG_BT_OBSERVER)
        __component_add_compile_definitions(-DCONFIG_BT_OBSERVER)
    endif()
    if(CONFIG_BT_PERIPHERAL)
        __component_add_compile_definitions(-DCONFIG_BT_PERIPHERAL)
    endif()
    if(CONFIG_BT_BROADCASTER)
        __component_add_compile_definitions(-DCONFIG_BT_BROADCASTER)
    endif()
endif()

# ============================================================================
# Feature Enable/Disable Definitions
# ============================================================================
# Note: CONFIG_BT_CONN is defined in autoconf.h from Kconfig, no need to redefine

if(CONFIG_BLE_PDS)
    __ble_ctlr_cfg(-DCFG_BLE_PDS)
    __component_add_compile_definitions(-DCONFIG_HW_SEC_ENG_DISABLE)
endif()

if(CONFIG_EM_HEAP_DISABLE)
    __ble_ctlr_cfg(-DCFG_EM_HEAP_DISABLE)
endif()

if(CONFIG_BLE_HOST_DISABLE)
    __component_add_compile_definitions(-DCONFIG_BLE_HOST_DISABLE)
endif()

# ============================================================================
# HCI Transport Layer Configuration
# ============================================================================
# HCI TL is needed in two scenarios:
# 1. Controller-only mode (CONFIG_BLE_HOST_DISABLE) - external host via UART/USB
# 2. On-chip HCI with blestack - internal communication
#
# For on-chip HCI: HCI_TL_SUPPORT=1 with HOST_PRESENT=0 uses simplified macros
# that don't require RivieraWaves host headers (hl_hci.h enum values).

if(CONFIG_BLE_HOST_DISABLE)
    # Controller-only mode: Enable HCI Transport Layer for external host
    __ble_ctlr_cfg(-DCFG_HCITL)
    __ble_ctlr_cfg(-DCFG_H4TL)

    message(STATUS "[BLE] HCI Transport Layer enabled (controller-only mode)")

    # UART Transport
    if(CONFIG_BLE_HCI_UART)
        __component_add_compile_definitions(-DCONFIG_BLE_HCI_UART)
        message(STATUS "[BLE] HCI Transport: UART")

        # Optional: Low-power UART HCI
        if(CONFIG_LP_HCIUART)
            __component_add_compile_definitions(-DCONFIG_LP_HCIUART)
        endif()
    endif()

    # USB CDC Transport
    if(CONFIG_BLE_HCI_USB_CDC OR CONFIG_APP_USB_DEVICE_CDC)
        __component_add_compile_definitions(-DCONFIG_BLE_HCI_USB_CDC)
        message(STATUS "[BLE] HCI Transport: USB CDC")
    endif()
else()
    # On-chip HCI mode: Enable HCI TL for communication with blestack
    # Use CFG_ONCHIP_HCI instead of CFG_BLE_HOST to avoid including
    # RivieraWaves host headers (rwhl_config.h, hl_hci.h)
    __ble_ctlr_cfg(-DCFG_ONCHIP_HCI)
    __ble_ctlr_cfg(-DCFG_HCITL)

    message(STATUS "[BLE] On-chip HCI mode enabled (CFG_ONCHIP_HCI + CFG_HCITL)")
endif()

# Advanced features
if(CONFIG_ADV_EXTENSION)
    __ble_ctlr_cfg(-DCFG_ADV_EXT)
endif()

if(CONFIG_CIS)
    __ble_ctlr_cfg(-DCFG_CIS)
    # CFG_ISO_CON required for BLE_ISO_PRESENT in controller (enables CIS/BIS feature bits)
    __ble_ctlr_cfg(-DCFG_ISO_CON=8)
    # CFG_ISOOHCI enables ISO data path over HCI transport (required for on-chip HCI)
    __ble_ctlr_cfg(-DCFG_ISOOHCI)
endif()

if(CONFIG_BIS)
    __ble_ctlr_cfg(-DCFG_BIS)
endif()

if(CONFIG_CTE)
    __ble_ctlr_cfg(-DCFG_CTE)
endif()

if(CONFIG_LE_PWR_CTRL OR CONFIG_CFG_LE_PWR_CTRL)
    __ble_ctlr_cfg(-DCFG_LE_PWR_CTRL)
endif()

# Advertising Features
if(CONFIG_CFG_PER_ADV)
    __ble_ctlr_cfg(-DCFG_PER_ADV)
endif()

if(CONFIG_CFG_PAWR)
    __ble_ctlr_cfg(-DCFG_PAWR)
endif()

if(CONFIG_CFG_PAST)
    __ble_ctlr_cfg(-DCFG_PAST)
endif()

if(CONFIG_CFG_CSSA)
    __ble_ctlr_cfg(-DCFG_CSSA)
endif()

if(CONFIG_CFG_ADV_CH_IDX)
    __ble_ctlr_cfg(-DCFG_ADV_CH_IDX)
endif()

# Connection Features
if(CONFIG_CFG_LE_SUBRATING)
    __ble_ctlr_cfg(-DCFG_LE_SUBRATING)
endif()

# GAF (Generic Audio Framework) Support
if(CONFIG_CFG_GAF)
    __ble_ctlr_cfg(-DCFG_GAF)
endif()

# Bluetooth Version Support
if(CONFIG_CFG_BT_53)
    __ble_ctlr_cfg(-DCFG_BT_53)
endif()

if(CONFIG_CFG_BT_54)
    __ble_ctlr_cfg(-DCFG_BT_54)
endif()

# ============================================================================
# BLE Stack Options
# ============================================================================
__component_add_compile_definitions(-DCONFIG_BT_HCI_VS_EVT_USER)
__component_add_compile_definitions(-DCONFIG_BT_ASSERT)

if(NOT DEFINED CONFIG_SIMPLE_MASTER OR NOT CONFIG_SIMPLE_MASTER)
    __component_add_compile_definitions(
        -DCONFIG_BT_GATT_DIS_PNP
        -DCONFIG_BT_GATT_DIS_SERIAL_NUMBER
        -DCONFIG_BT_GATT_DIS_FW_REV
        -DCONFIG_BT_GATT_DIS_HW_REV
        -DCONFIG_BT_GATT_DIS_SW_REV
        -DCONFIG_BT_GATT_DYNAMIC_DB
        -DCONFIG_BT_GATT_SERVICE_CHANGED
    )
endif()

if(CONFIG_BT_SETTINGS)
    __component_add_compile_definitions(-DCONFIG_BT_SETTINGS)
endif()

if(CONFIG_BT_GATT_CLIENT)
    __component_add_compile_definitions(-DCONFIG_BT_GATT_CLIENT)
endif()

if(CONFIG_BT_STACK_CLI)
    __component_add_compile_definitions(-DCONFIG_BT_STACK_CLI)
endif()

if(CONFIG_BLE_STACK_DBG_PRINT)
    __component_add_compile_definitions(-DCONFIG_BLE_STACK_DBG_PRINT)
endif()

# ============================================================================
# Security Manager Configuration
# ============================================================================
if(NOT CONFIG_DISABLE_BT_SMP)
    # Set the CMake variable as well as the macro. ipro_bt_host picks
    # smp.c + keys.c over smp_null.c from the variable, while the Host sources
    # gate their call sites on the macro. Defining only the macro leaves the
    # two describing different images: hci_core.c, id.c and conn.c compile
    # their bt_smp_*/bt_keys_* calls while the link supplies the null
    # implementation, and the build dies on undefined references. That stayed
    # hidden while smp.c and keys.c were compiled unconditionally.
    set(CONFIG_BT_SMP 1)
    __component_add_compile_definitions(-DCONFIG_BT_SMP)
    __component_add_compile_definitions(-DCONFIG_BT_SIGNING)
    __component_add_compile_definitions(-DCONFIG_BT_SMP_SC_PAIR_ONLY)
    __component_add_compile_definitions(-DCONFIG_BT_BONDABLE)

    # Enable ECC support for LE Secure Connections (LESC)
    # CONFIG_BT_ECC enables smp_public_key/smp_dhkey_check handlers in smp.c
    __component_add_compile_definitions(-DCONFIG_BT_ECC)
    # Emulate the two asynchronous ECC HCI commands in the host.  The IPRO7
    # controller implementation performs remote-point validation synchronously
    # in its controller task, which can starve radio/HCI processing for tens of
    # seconds.  TinyCrypt runs the same validated P-256 operation in a dedicated
    # host task and preserves the standard HCI command/event behaviour.
    __component_add_compile_definitions(-DCONFIG_BT_TINYCRYPT_ECC)
endif()

# ECC P256 Support (required for Secure Connections)
# This enables ECC_MULT_ALGO_TYPE definition in rwip_config.h
__ble_ctlr_cfg(-DCFG_ECC_P256_SUPPORT)

# ECC algorithm type: 16-bit for smaller memory footprint, 32-bit for speed
# Default to 32-bit for IPRO7
if(NOT CONFIG_ECC_16_BITS_ALGO)
    # 32-bit ECC algorithm (default, faster)
else()
    __ble_ctlr_cfg(-DCFG_ECC_16_BITS_ALGO)
endif()

if(CONFIG_DISABLE_BT_HOST_PRIVACY)
    __component_add_compile_definitions(-DCONFIG_DISABLE_BT_HOST_PRIVACY)
endif()

# ============================================================================
# BLE Mesh Configuration
# ============================================================================
if(CONFIG_BT_MESH)
    __component_add_compile_definitions(-DCONFIG_BT_MESH)

    sdk_ifndef(CONFIG_BT_MESH_CLI 1)
    sdk_ifndef(CONFIG_BT_MESH_PB_ADV 1)
    sdk_ifndef(CONFIG_BT_MESH_RELAY 1)
    sdk_ifndef(CONFIG_BT_MESH_PB_GATT 1)
    sdk_ifndef(CONFIG_BT_MESH_FRIEND 1)
    sdk_ifndef(CONFIG_BT_MESH_LOW_POWER 1)
    sdk_ifndef(CONFIG_BT_MESH_PROXY 1)
    sdk_ifndef(CONFIG_BT_MESH_GATT_PROXY 1)
    sdk_ifndef(CONFIG_BT_MESH_PROVISIONER 0)

    if(CONFIG_BT_MESH_CLI)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_CLI)
    endif()
    if(CONFIG_BT_MESH_PB_ADV)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_PB_ADV)
    endif()
    if(CONFIG_BT_MESH_RELAY)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_RELAY)
    endif()
    if(CONFIG_BT_MESH_PB_GATT)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_PB_GATT)
    endif()
    if(CONFIG_BT_MESH_FRIEND)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_FRIEND)
    endif()
    if(CONFIG_BT_MESH_LOW_POWER)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_LOW_POWER)
    endif()
    if(CONFIG_BT_MESH_PROXY)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_PROXY)
    endif()
    if(CONFIG_BT_MESH_GATT_PROXY)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_GATT_PROXY)
    endif()
    if(CONFIG_BT_MESH_PROVISIONER)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_PROVISIONER)
        sdk_ifndef(CONFIG_BT_MESH_CDB 1)
        if(CONFIG_BT_MESH_CDB)
            __component_add_compile_definitions(-DCONFIG_BT_MESH_CDB)
        endif()
    endif()

    if(CONFIG_BT_MESH_MODEL)
        __component_add_compile_definitions(-DCONFIG_BT_MESH_MODEL)
        sdk_ifndef(CONFIG_BT_MESH_MODEL_GEN_SRV 1)
        sdk_ifndef(CONFIG_BT_MESH_MODEL_GEN_CLI 1)
        sdk_ifndef(CONFIG_BT_MESH_MODEL_LIGHT_SRV 1)
        sdk_ifndef(CONFIG_BT_MESH_MODEL_LIGHT_CLI 1)
    endif()
endif()

# ============================================================================
# Manufacturing Test Mode
# ============================================================================
if(CONFIG_BLE_MFG)
    __component_add_compile_definitions(-DCONFIG_BLE_MFG)
    if(CONFIG_BLE_MFG_HCI_CMD)
        __component_add_compile_definitions(-DCONFIG_BLE_MFG_HCI_CMD)
    endif()
endif()

# ============================================================================
# Memory Configuration
# ============================================================================
if(CONFIG_EM_SIZE GREATER 32)
    __ble_ctlr_cfg(-DCFG_EM_SIZE=${CONFIG_EM_SIZE})
endif()

message(STATUS "[BLE] CONFIG_BT_CONN=${CONFIG_BT_CONN}, CONFIG_BLE_TX_BUFF_DATA=${CONFIG_BLE_TX_BUFF_DATA}")
message(STATUS "[BLE] Roles: CENTRAL=${CONFIG_BT_CENTRAL}, PERIPHERAL=${CONFIG_BT_PERIPHERAL}, OBSERVER=${CONFIG_BT_OBSERVER}, BROADCASTER=${CONFIG_BT_BROADCASTER}")

# ============================================================================
# LE Audio Configuration (BLE 5.2+)
# ============================================================================
if(CONFIG_BT_AUDIO)
    __component_add_compile_definitions(-DCONFIG_BT_AUDIO)
    __component_add_compile_definitions(-DCONFIG_BT_ZEPHYR_COMPAT)
    message(STATUS "[BLE] LE Audio enabled")

    # Basic Audio Profile (BAP)
    if(CONFIG_BT_BAP_UNICAST)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_UNICAST)
    endif()
    if(CONFIG_BT_BAP_UNICAST_SERVER)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_UNICAST_SERVER)
    endif()
    if(CONFIG_BT_BAP_UNICAST_CLIENT)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_UNICAST_CLIENT)
    endif()

    # Audio Stream Control Service (ASCS)
    if(CONFIG_BT_ASCS)
        __component_add_compile_definitions(-DCONFIG_BT_ASCS)
    endif()

    # Published Audio Capabilities Service (PACS)
    if(CONFIG_BT_PACS)
        __component_add_compile_definitions(-DCONFIG_BT_PACS)
    endif()
    if(CONFIG_BT_BAP_BROADCAST_SOURCE)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_BROADCAST_SOURCE)
    endif()
    if(CONFIG_BT_BAP_BROADCAST_SINK)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_BROADCAST_SINK)
    endif()
    if(CONFIG_BT_BAP_SCAN_DELEGATOR)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_SCAN_DELEGATOR)
    endif()
    if(CONFIG_BT_BAP_BROADCAST_ASSISTANT)
        __component_add_compile_definitions(-DCONFIG_BT_BAP_BROADCAST_ASSISTANT)
    endif()

    # ISO Channels
    if(CONFIG_BT_ISO)
        __component_add_compile_definitions(-DCONFIG_BT_ISO)
    endif()
    if(CONFIG_BT_ISO_TX)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_TX)
    endif()
    if(CONFIG_BT_ISO_RX)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_RX)
    endif()
    if(CONFIG_BT_ISO_BROADCASTER)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_BROADCASTER)
    endif()
    if(CONFIG_BT_ISO_SYNC_RECEIVER)
        __component_add_compile_definitions(-DCONFIG_BT_ISO_SYNC_RECEIVER)
    endif()
    if(CONFIG_BT_AUDIO_TX)
        __component_add_compile_definitions(-DCONFIG_BT_AUDIO_TX)
    endif()
    if(CONFIG_BT_AUDIO_RX)
        __component_add_compile_definitions(-DCONFIG_BT_AUDIO_RX)
    endif()

    # Volume Control Profile (VCP)
    if(CONFIG_BT_VCP_VOL_REND)
        __component_add_compile_definitions(-DCONFIG_BT_VCP_VOL_REND)
    endif()
    if(CONFIG_BT_VCP_VOL_CTLR)
        __component_add_compile_definitions(-DCONFIG_BT_VCP_VOL_CTLR)
    endif()

    # Microphone Control Profile (MICP)
    if(CONFIG_BT_MICP_MIC_DEV)
        __component_add_compile_definitions(-DCONFIG_BT_MICP_MIC_DEV)
    endif()
    if(CONFIG_BT_MICP_MIC_CTLR)
        __component_add_compile_definitions(-DCONFIG_BT_MICP_MIC_CTLR)
    endif()

    # Coordinated Set Identification Profile (CSIP)
    if(CONFIG_BT_CSIP_SET_MEMBER)
        __component_add_compile_definitions(-DCONFIG_BT_CSIP_SET_MEMBER)
    endif()
    if(CONFIG_BT_CSIP_SET_COORDINATOR)
        __component_add_compile_definitions(-DCONFIG_BT_CSIP_SET_COORDINATOR)
    endif()

    # Common Audio Profile (CAP)
    if(CONFIG_BT_CAP_ACCEPTOR)
        __component_add_compile_definitions(-DCONFIG_BT_CAP_ACCEPTOR)
    endif()
    if(CONFIG_BT_CAP_INITIATOR)
        __component_add_compile_definitions(-DCONFIG_BT_CAP_INITIATOR)
    endif()
    if(CONFIG_BT_CAP_COMMANDER)
        __component_add_compile_definitions(-DCONFIG_BT_CAP_COMMANDER)
    endif()

    # Media Control (MCS/MCP)
    if(CONFIG_BT_MCS)
        __component_add_compile_definitions(-DCONFIG_BT_MCS)
    endif()
    if(CONFIG_BT_MCC)
        __component_add_compile_definitions(-DCONFIG_BT_MCC)
    endif()

    # Telephone Bearer Service (TBS/CCP)
    if(CONFIG_BT_TBS)
        __component_add_compile_definitions(-DCONFIG_BT_TBS)
    endif()
    if(CONFIG_BT_TBS_CLIENT)
        __component_add_compile_definitions(-DCONFIG_BT_TBS_CLIENT)
    endif()

    # Hearing Access Service (HAS)
    if(CONFIG_BT_HAS)
        __component_add_compile_definitions(-DCONFIG_BT_HAS)
    endif()
    if(CONFIG_BT_HAS_CLIENT)
        __component_add_compile_definitions(-DCONFIG_BT_HAS_CLIENT)
    endif()

    # Telephony and Media Audio Profile (TMAP)
    if(CONFIG_BT_TMAP)
        __component_add_compile_definitions(-DCONFIG_BT_TMAP)
    endif()
endif()

# Zephyr compatibility layer (can be enabled without full LE Audio)
if(CONFIG_BT_ZEPHYR_COMPAT)
    __component_add_compile_definitions(-DCONFIG_BT_ZEPHYR_COMPAT)
endif()

# ============================================================================
# BLE 5.4 Features (PAwR / ESL)
# ============================================================================
if(CONFIG_BT_PER_ADV_RSP)
    __component_add_compile_definitions(-DCONFIG_BT_PER_ADV_RSP)
    message(STATUS "[BLE] Periodic Advertising with Responses (PAwR) enabled")
endif()

if(CONFIG_BT_PER_ADV_SYNC_RSP)
    __component_add_compile_definitions(-DCONFIG_BT_PER_ADV_SYNC_RSP)
endif()

if(CONFIG_BT_ESL)
    __component_add_compile_definitions(-DCONFIG_BT_ESL)
    message(STATUS "[BLE] Electronic Shelf Label (ESL) enabled")
endif()
