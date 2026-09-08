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
# BLE Stack Options - options.cmake
# ============================================================================
# Compile definitions for btblecontroller + blestack
# Most definitions are handled by ble_common.cmake based on variant selection
# ============================================================================

# ============================================================================
# Controller Variant Selection (from Kconfig)
# ============================================================================
if(DEFINED CONFIG_BTBLECONTROLLER_LIB)
    set(PRIV_CONFIG_GEN_BLE ${CONFIG_BTBLECONTROLLER_LIB})
endif()

# ============================================================================
# GATT Services
# ============================================================================
if(CONFIG_BT_BAS_SERVER)
    __component_add_compile_definitions(-DCONFIG_BT_BAS_SERVER)
endif()

if(CONFIG_BT_DIS_SERVER)
    __component_add_compile_definitions(-DCONFIG_BT_DIS_SERVER)
endif()

if(CONFIG_BT_SPP_SERVER)
    __component_add_compile_definitions(-DCONFIG_BT_SPP_SERVER)
endif()

if(CONFIG_HOGP_SERVER)
    __component_add_compile_definitions(-DCONFIG_HOGP_SERVER)
endif()

if(CONFIG_BT_OAD_SERVER)
    __component_add_compile_definitions(-DCONFIG_BT_OAD_SERVER)
endif()

if(CONFIG_BT_OAD_CLIENT)
    __component_add_compile_definitions(-DCONFIG_BT_OAD_CLIENT)
endif()

if(CONFIG_BLE_TP_SERVER)
    __component_add_compile_definitions(-DCONFIG_BLE_TP_SERVER)
endif()

# ============================================================================
# Debug Options
# ============================================================================
if(CONFIG_BT_DEBUG)
    __component_add_compile_definitions(-DCONFIG_BT_DEBUG)
endif()

if(CONFIG_BT_DEBUG_MONITOR)
    __component_add_compile_definitions(-DCONFIG_BT_DEBUG_MONITOR)
endif()

# ============================================================================
# BR/EDR Classic Bluetooth Profile Options (IPRO6)
# ============================================================================
if(CONFIG_BT_BREDR)
    __component_add_compile_definitions(-DCONFIG_BT_BREDR)
endif()

if(CONFIG_BT_A2DP)
    __component_add_compile_definitions(-DCONFIG_BT_A2DP)
endif()

if(CONFIG_BT_AVRCP)
    __component_add_compile_definitions(-DCONFIG_BT_AVRCP)
endif()

if(CONFIG_BT_HFP)
    __component_add_compile_definitions(-DCONFIG_BT_HFP)
endif()

if(CONFIG_BT_SPP)
    __component_add_compile_definitions(-DCONFIG_BT_SPP)
endif()

# ============================================================================
# Note: Most compile definitions are now set in ble_common.cmake
# based on the selected variant (m0s1, m1s1, m2s1, m4s1, m8s1)
# ============================================================================
