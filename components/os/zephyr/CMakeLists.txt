# Zephyr RTOS Integration for IPRO SDK
# This file integrates Zephyr RTOS into the IPRO SDK build system

cmake_minimum_required(VERSION 3.20.0)

# Check if Zephyr is enabled
if(NOT CONFIG_ZEPHYR)
    return()
endif()

# Set Zephyr configuration variables
set(ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR})

# Define the Zephyr board configuration for IPRO hardware
if(NOT DEFINED BOARD)
    set(BOARD ipro7)
endif()

# Set Zephyr base directory - you might need to adjust this path
if(NOT DEFINED ZEPHYR_BASE)
    # Try to find Zephyr in common locations
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/zephyr-sdk)
        set(ZEPHYR_BASE ${CMAKE_CURRENT_SOURCE_DIR}/zephyr-sdk)
    elseif(EXISTS $ENV{ZEPHYR_BASE})
        set(ZEPHYR_BASE $ENV{ZEPHYR_BASE})
    else()
        message(WARNING "Zephyr RTOS not found. Please set ZEPHYR_BASE environment variable or install Zephyr SDK")
        return()
    endif()
endif()

# Include Zephyr's CMake boilerplate
if(EXISTS ${ZEPHYR_BASE}/cmake/app/boilerplate.cmake)
    include(${ZEPHYR_BASE}/cmake/app/boilerplate.cmake)
elseif(EXISTS ${ZEPHYR_BASE}/share/zephyr-package/cmake/ZephyrConfig.cmake)
    find_package(Zephyr REQUIRED HINTS ${ZEPHYR_BASE})
else()
    message(FATAL_ERROR "Cannot find Zephyr CMake configuration files")
endif()

# Create a library for Zephyr integration
add_library(zephyr_integration INTERFACE)

# Add Zephyr-specific include directories
target_include_directories(zephyr_integration INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/boards
    ${CMAKE_CURRENT_SOURCE_DIR}/dts
)

# Add Zephyr-specific compile definitions
target_compile_definitions(zephyr_integration INTERFACE
    CONFIG_ZEPHYR=1
    CONFIG_RTOS_ZEPHYR=1
)

# Link with Zephyr if the target is available
if(TARGET zephyr_interface)
    target_link_libraries(zephyr_integration INTERFACE zephyr_interface)
endif()

# Add source files for IPRO-specific Zephyr adaptation
target_sources(zephyr_integration INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ipro_zephyr_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ipro_zephyr_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ipro_zephyr_gpio.c
)

# Create an alias for backward compatibility
add_library(ipro::os::zephyr ALIAS zephyr_integration)

# Add IPRO7 drivers if SOC is IPRO
if(CONFIG_SOC_FAMILY_IPRO)
  add_subdirectory(drivers)
endif()

message(STATUS "Zephyr RTOS support enabled for IPRO SDK")
