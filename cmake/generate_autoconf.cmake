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

# Generate autoconf.h from .config file
# This provides a Linux kernel-style autoconf.h header for C code

function(generate_autoconf_header)
    # Determine config file path
    if(DEFINED CUSTOM_CONFIG_DIR)
        set(CONFIG_DIR "${CUSTOM_CONFIG_DIR}")
    else()
        set(CONFIG_DIR "${CMAKE_SOURCE_DIR}")
    endif()

    # Require .config file (Linux kernel standard)
    if(EXISTS "${CONFIG_DIR}/.config")
        set(CONFIG_FILE "${CONFIG_DIR}/.config")
        set(CONFIG_SOURCE ".config")
    else()
        message(FATAL_ERROR "No .config file found in ${CONFIG_DIR}.")
    endif()

    # Output path
    set(AUTOCONF_H "${CMAKE_BINARY_DIR}/include/generated/autoconf.h")
    set(AUTOCONF_DIR "${CMAKE_BINARY_DIR}/include/generated")

    # Create output directory
    file(MAKE_DIRECTORY "${AUTOCONF_DIR}")

    # Read config file
    file(READ "${CONFIG_FILE}" config_contents)
    string(REPLACE "\n" ";" config_lines "${config_contents}")

    # Generate header content
    set(header_content "/*\n")
    set(header_content "${header_content} * Auto-generated file - DO NOT EDIT\n")
    set(header_content "${header_content} * Generated from: ${CONFIG_SOURCE}\n")
    set(header_content "${header_content} * Generator: cmake/generate_autoconf.cmake\n")
    string(TIMESTAMP current_time "%Y-%m-%d %H:%M:%S")
    set(header_content "${header_content} * Generated at: ${current_time}\n")
    set(header_content "${header_content} */\n\n")
    set(header_content "${header_content}#ifndef __AUTOCONF_H__\n")
    set(header_content "${header_content}#define __AUTOCONF_H__\n\n")

    # Process each line
    foreach(line IN LISTS config_lines)
        string(STRIP "${line}" line)

        # Skip empty lines
        if(line STREQUAL "")
            continue()
        endif()

        # Convert comment lines to C-style comments (but not "# CONFIG_XXX is not set")
        if(line MATCHES "^#.*" AND NOT line MATCHES "^# CONFIG_[A-Za-z0-9_]+ is not set")
            string(REGEX REPLACE "^#" "/*" comment_line "${line}")
            set(header_content "${header_content}${comment_line} */\n")
            continue()
        endif()

        # Handle "# CONFIG_XXX is not set"
        if(line MATCHES "^# CONFIG_[A-Za-z0-9_]+ is not set")
            string(REGEX REPLACE "^# (CONFIG_[A-Za-z0-9_]+) is not set" "\\1" var_name "${line}")
            set(header_content "${header_content}/* ${var_name} is not set */\n")
            set(header_content "${header_content}#undef ${var_name}\n")
            # Set CMake variable to 0 for disabled options (needed for linker script generation)
            set(${var_name} 0 PARENT_SCOPE)
            continue()
        endif()

        # Handle CONFIG_XXX=y
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=y$")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=y$" "\\1" var_name "${line}")
            set(header_content "${header_content}#define ${var_name} 1\n")
            set(${var_name} 1 PARENT_SCOPE)
            continue()
        endif()

        # Handle CONFIG_XXX=n
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=n$")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=n$" "\\1" var_name "${line}")
            set(header_content "${header_content}/* ${var_name}=n */\n")
            set(header_content "${header_content}#undef ${var_name}\n")
            # Set CMake variable to 0 for disabled options (needed for linker script generation)
            set(${var_name} 0 PARENT_SCOPE)
            continue()
        endif()

        # Handle CONFIG_XXX="string"
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=\".*\"$")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=\"(.*)\"$" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=\"(.*)\"$" "\\2" var_value "${line}")
            set(header_content "${header_content}#define ${var_name} \"${var_value}\"\n")
            set(${var_name} "${var_value}" PARENT_SCOPE)
            continue()
        endif()

        # Handle CONFIG_XXX=number
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=[0-9]+$")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([0-9]+)$" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([0-9]+)$" "\\2" var_value "${line}")
            set(header_content "${header_content}#define ${var_name} ${var_value}\n")
            set(${var_name} ${var_value} PARENT_SCOPE)
            continue()
        endif()

        # Handle CONFIG_XXX=identifier (catch-all for other values)
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=[A-Za-z0-9_]+$")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([A-Za-z0-9_]+)$" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([A-Za-z0-9_]+)$" "\\2" var_value "${line}")
            set(header_content "${header_content}#define ${var_name} ${var_value}\n")
            set(${var_name} "${var_value}" PARENT_SCOPE)
            continue()
        endif()
    endforeach()

    # Close header guard
    set(header_content "${header_content}\n#endif /* __AUTOCONF_H__ */\n")

    # Write header file
    file(WRITE "${AUTOCONF_H}" "${header_content}")
    message(STATUS "Generated autoconf header: ${AUTOCONF_H}")
    message(STATUS "  Source: ${CONFIG_FILE}")

    # Export include directory to parent scope
    set(AUTOCONF_INCLUDE_DIR "${CMAKE_BINARY_DIR}/include" PARENT_SCOPE)

    # Make this header a dependency for reconfiguration
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CONFIG_FILE}")
endfunction()
