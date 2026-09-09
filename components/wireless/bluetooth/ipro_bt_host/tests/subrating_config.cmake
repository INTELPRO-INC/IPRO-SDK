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
if(DEFINED ROLE)
    set(CONFIG_BT_SUBRATING ON)
    if(ROLE STREQUAL "central")
        set(CONFIG_BT_CENTRAL ON)
    elseif(ROLE STREQUAL "peripheral")
        set(CONFIG_BT_PERIPHERAL ON)
    endif()
    include("${CMAKE_CURRENT_LIST_DIR}/../cmake/resolve_config.cmake")
    return()
endif()

foreach(role central peripheral none)
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DROLE=${role}" -P "${CMAKE_CURRENT_LIST_FILE}"
                    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(role STREQUAL "none")
        if(result EQUAL 0 OR NOT error MATCHES "SUBRATING needs CENTRAL or PERIPHERAL")
            message(FATAL_ERROR "Subrating without a connection role was not rejected: ${error}")
        endif()
    elseif(NOT result EQUAL 0)
        message(FATAL_ERROR "Valid ${role} configuration failed: ${error}")
    endif()
endforeach()
message(STATUS "Subrating config: central/peripheral accepted, missing role rejected")
