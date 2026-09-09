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

cmake_minimum_required(VERSION 3.20)
set(CONFIG_BT_GATT_SECURITY_LEVELS ON)
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/resolve_config.cmake")
if(NOT CONFIG_BT_GATT_GAP_SERVICE OR NOT CONFIG_BT_GATT_GAP_SVC_VALIDATE)
    message(FATAL_ERROR "Security levels must resolve GAP service and validation")
endif()
unset(CONFIG_BT_GATT_SECURITY_LEVELS)
unset(CONFIG_BT_GATT_GAP_SERVICE)
unset(CONFIG_BT_GATT_GAP_SVC_VALIDATE)
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/resolve_config.cmake")
if(CONFIG_BT_GATT_GAP_SERVICE OR CONFIG_BT_GATT_GAP_SVC_VALIDATE)
    message(FATAL_ERROR "Disabled Security Levels must not add the GAP service")
endif()
message(STATUS "GATT Security Levels dependencies PASS")
