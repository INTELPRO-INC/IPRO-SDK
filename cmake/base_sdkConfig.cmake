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

if(NOT DEFINED ENV{IPRO_SDK_BASE})
    message( "please set IPRO_SDK_BASE in your system environment")
endif()

if(MINGW OR CYGWIN OR WIN32)
SET(CMAKE_SYSTEM_NAME Generic)
elseif(UNIX)
SET(CMAKE_SYSTEM_NAME Linux)
elseif(APPLE)
SET(CMAKE_SYSTEM_NAME Darwin)
endif()
SET(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR RISCV)

set(IPRO_SDK_BASE $ENV{IPRO_SDK_BASE})

set(build_dir ${CMAKE_CURRENT_BINARY_DIR}/build_out)
set(PROJECT_SOURCE_DIR ${IPRO_SDK_BASE})
set(PROJECT_BINARY_DIR ${build_dir})
set(EXECUTABLE_OUTPUT_PATH ${build_dir})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(__component_lib INTERFACE)
add_library(app STATIC)
target_link_libraries(app __component_lib)

include(${IPRO_SDK_BASE}/cmake/toolchain.cmake)
include(${IPRO_SDK_BASE}/cmake/extension.cmake)
include(${IPRO_SDK_BASE}/cmake/generate_autoconf.cmake)

sdk_ifndef(CUSTOM_CONFIG_DIR ".config")

# Generate autoconf.h from configuration file
# This provides Linux kernel-style <generated/autoconf.h> header
generate_autoconf_header()

# Add autoconf include directory globally
include_directories(${AUTOCONF_INCLUDE_DIR})

# Parse configuration file (require .config - Linux kernel standard)
# IMPORTANT: Must be done BEFORE compiler_flags.cmake so that CONFIG_* variables
# are available for conditional compile options (e.g., CONFIG_ENABLE_FRAME_POINTER)
if(EXISTS "${CUSTOM_CONFIG_DIR}/.config")
    config_parse(${CUSTOM_CONFIG_DIR}/.config)
else()
    message(FATAL_ERROR "No .config file found in ${CUSTOM_CONFIG_DIR}.")
endif()

# Add chip-specific definition (IPRO6LE/IPRO6/IPRO7)
# For IPRO6: both IPRO7 (from toolchain.cmake) and IPRO6 are defined
# Code should check IPRO6 first since it's more specific
add_definitions(-D${CONFIG_CHIP})

# Include compiler flags AFTER config_parse so CONFIG_* variables are available
include(${IPRO_SDK_BASE}/cmake/compiler_flags.cmake)

# enable_language(C CXX ASM)

# add_subdirectory(${IPRO_SDK_BASE} ${build_dir})