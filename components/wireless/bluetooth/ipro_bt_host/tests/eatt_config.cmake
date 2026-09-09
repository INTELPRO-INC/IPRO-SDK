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
if(DEFINED CAPACITY)
    set(CONFIG_BT_EATT ON)
    set(CONFIG_BT_EATT_MAX ${CAPACITY})
    include("${CMAKE_CURRENT_LIST_DIR}/../cmake/resolve_config.cmake")
    foreach(dep CONFIG_BT_SMP CONFIG_BT_L2CAP_ECRED CONFIG_BT_L2CAP_DYNAMIC_CHANNEL CONFIG_BT_GATT_READ_MULT_VAR_LEN)
        if(NOT ${dep})
            message(FATAL_ERROR "Missing ${dep}")
        endif()
    endforeach()
    return()
endif()
foreach(capacity 1 3 8 0 9)
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DCAPACITY=${capacity}" -P "${CMAKE_CURRENT_LIST_FILE}"
                    RESULT_VARIABLE result ERROR_VARIABLE error)
    if(capacity LESS 1 OR capacity GREATER 8)
        if(result EQUAL 0 OR NOT error MATCHES "EATT_MAX must be in 1..8")
            message(FATAL_ERROR "Invalid EATT capacity ${capacity} not rejected: ${error}")
        endif()
    elseif(NOT result EQUAL 0)
        message(FATAL_ERROR "Valid EATT capacity ${capacity} failed: ${error}")
    endif()
endforeach()
message(STATUS "EATT dependencies and capacity boundaries PASS")
