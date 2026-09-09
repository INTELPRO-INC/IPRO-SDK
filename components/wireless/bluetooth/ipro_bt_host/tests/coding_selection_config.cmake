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
set(CONFIG_BT_EXT_ADV_CODING_SELECTION ON)
set(CONFIG_BT_EXT_ADV OFF)
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/resolve_config.cmake")
if(NOT CONFIG_BT_EXT_ADV)
    message(FATAL_ERROR "Coding selection must resolve extended advertising")
endif()
if(NOT CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE EQUAL 255)
    message(FATAL_ERROR "Coding selection must resolve extended event capacity")
endif()
message(STATUS "Coding selection dependencies PASS")
set(CONFIG_BT_EXT_ADV_CODING_SELECTION OFF)
set(CONFIG_BT_EXT_ADV OFF)
unset(CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE)
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/resolve_config.cmake")
if(CONFIG_BT_EXT_ADV)
    message(FATAL_ERROR "Disabled coding selection enabled extended advertising")
endif()
if(NOT CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE EQUAL 43)
    message(FATAL_ERROR "Disabled feature changed legacy event capacity")
endif()
message(STATUS "Coding selection disabled defaults PASS")
