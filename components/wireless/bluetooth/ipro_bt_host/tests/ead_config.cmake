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
set(CONFIG_BT_EAD ON)
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/resolve_config.cmake")
if(NOT CONFIG_BT_HOST_CCM)
    message(FATAL_ERROR "EAD must select BT_HOST_CCM")
endif()
message(STATUS "EAD CCM dependency PASS")
