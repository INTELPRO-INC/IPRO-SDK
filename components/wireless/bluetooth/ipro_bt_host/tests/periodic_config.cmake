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
set(CONFIG_BT_PER_ADV_RSP ON)
set(CONFIG_BT_PER_ADV_SYNC_RSP ON)
set(CONFIG_BT_PER_ADV_SYNC_BUF_SIZE 512)
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/resolve_config.cmake")
if(NOT CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE EQUAL 255)
    message(FATAL_ERROR "Extended advertising needs full HCI event capacity")
endif()
foreach(symbol CONFIG_BT_EXT_ADV CONFIG_BT_PER_ADV CONFIG_BT_PER_ADV_SYNC
               CONFIG_BT_BROADCASTER CONFIG_BT_OBSERVER)
    if(NOT ${symbol})
        message(FATAL_ERROR "PAwR missing dependency: ${symbol}")
    endif()
endforeach()
if(NOT CONFIG_BT_PER_ADV_SYNC_BUF_SIZE EQUAL 512)
    message(FATAL_ERROR "Periodic reassembly capacity was overwritten")
endif()
file(READ "${CMAKE_CURRENT_LIST_DIR}/../CMakeLists.txt" build_source)
string(REGEX MATCHALL "-DCONFIG_BT_PER_ADV_SYNC_BUF_SIZE=[^ \n\r]+" definitions "${build_source}")
list(LENGTH definitions count)
if(NOT count EQUAL 1)
    message(FATAL_ERROR "Periodic buffer must have one compile definition, found ${count}")
endif()
if(NOT definitions STREQUAL "-DCONFIG_BT_PER_ADV_SYNC_BUF_SIZE=\${CONFIG_BT_PER_ADV_SYNC_BUF_SIZE}")
    message(FATAL_ERROR "Periodic buffer compile definition must use resolved config")
endif()
message(STATUS "PAwR dependencies and periodic buffer configuration PASS")

# Reproduce the SDK autoconf #undef overriding command-line dependencies.
if(NOT DEFINED TEST_BINARY_DIR)
    message(FATAL_ERROR "Pass -DTEST_BINARY_DIR=<temporary directory>")
endif()
file(MAKE_DIRECTORY "${TEST_BINARY_DIR}/generated")
configure_file("${CMAKE_CURRENT_LIST_DIR}/../cmake/periodic_config.h.in"
               "${TEST_BINARY_DIR}/ipro_bt_host_periodic_config.h")
file(WRITE "${TEST_BINARY_DIR}/generated/autoconf.h"
    "#undef CONFIG_BT_EXT_ADV\n#undef CONFIG_BT_PER_ADV\n#undef CONFIG_BT_PER_ADV_SYNC\n#undef CONFIG_BT_BROADCASTER\n#undef CONFIG_BT_OBSERVER\n")
file(WRITE "${TEST_BINARY_DIR}/periodic_config_test.c"
    "#include <zephyr/autoconf.h>\n#if !CONFIG_BT_EXT_ADV || !CONFIG_BT_PER_ADV || !CONFIG_BT_PER_ADV_SYNC || !CONFIG_BT_BROADCASTER || !CONFIG_BT_OBSERVER\n#error Missing resolved periodic dependency\n#endif\nint main(void) { return 0; }\n")
execute_process(COMMAND cc -Werror -Wundef -fsyntax-only
    "-I${TEST_BINARY_DIR}" "-I${CMAKE_CURRENT_LIST_DIR}/../port/include"
    "${TEST_BINARY_DIR}/periodic_config_test.c" RESULT_VARIABLE result)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Resolved dependencies lost after application autoconf")
endif()
message(STATUS "Application autoconf explicit-undef regression PASS")
foreach(symbol CONFIG_BT_EXT_ADV CONFIG_BT_PER_ADV CONFIG_BT_PER_ADV_SYNC
               CONFIG_BT_BROADCASTER CONFIG_BT_OBSERVER)
    set(${symbol} OFF)
endforeach()
configure_file("${CMAKE_CURRENT_LIST_DIR}/../cmake/periodic_config.h.in"
               "${TEST_BINARY_DIR}/ipro_bt_host_periodic_config.h")
file(WRITE "${TEST_BINARY_DIR}/periodic_config_test.c"
    "#include <zephyr/autoconf.h>\n#if defined(CONFIG_BT_EXT_ADV) || defined(CONFIG_BT_PER_ADV) || defined(CONFIG_BT_PER_ADV_SYNC) || defined(CONFIG_BT_BROADCASTER) || defined(CONFIG_BT_OBSERVER)\n#error Disabled dependencies unexpectedly enabled\n#endif\nint main(void) { return 0; }\n")
execute_process(COMMAND cc -Werror -Wundef -fsyntax-only
    "-I${TEST_BINARY_DIR}" "-I${CMAKE_CURRENT_LIST_DIR}/../port/include"
    "${TEST_BINARY_DIR}/periodic_config_test.c" RESULT_VARIABLE result)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Disabled configuration was not preserved")
endif()
message(STATUS "Disabled periodic configuration PASS")
