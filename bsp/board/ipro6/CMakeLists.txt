__component_add_include_dirs(.)

string(TOLOWER "${CONFIG_BOARD}" CONFIG_BOARD_LOWER)

set(BOARD_DIR "")

if (DEFINED CONFIG_FPGA AND CONFIG_FPGA)
    set(BOARD_DIR "fpga")
endif()

if (DEFINED CONFIG_EVB AND CONFIG_EVB)
    set(BOARD_DIR "evb")
endif()

__component_add_include_dirs("${BOARD_DIR}")

target_sources(app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/board.c")

# Linker script selection for IPRO6
# Priority:
# 1. App-local linker script specified by CONFIG_LINKER_SCRIPT (Kconfig)
# 2. Generated linker script (from cmake config)
# 3. Static board linker script (legacy)
#
# To use a specific linker script, set in app's .config:
#   CONFIG_LINKER_SCRIPT="ipro6_boot2_flash.ld"

# Resolve app directory (support nested app dirs)
if(DEFINED CONFIG_APP_CATEGORY AND NOT CONFIG_APP_CATEGORY STREQUAL "")
    set(APP_DIR "${CMAKE_SOURCE_DIR}/apps/${CONFIG_APP_CATEGORY}/${CONFIG_BUILD_PORJECT}")
else()
    set(APP_DIR "${CMAKE_SOURCE_DIR}/apps/${CONFIG_BUILD_PORJECT}")
endif()

# Check for app-local linker script via Kconfig
if(DEFINED CONFIG_LINKER_SCRIPT AND NOT CONFIG_LINKER_SCRIPT STREQUAL "")
    set(APP_LINKER_SCRIPT "${APP_DIR}/${CONFIG_LINKER_SCRIPT}")
    if(EXISTS "${APP_LINKER_SCRIPT}")
        set(TARGET_LINKER_SCRIPT "${APP_LINKER_SCRIPT}")
        message(STATUS "Using app-local linker script: ${TARGET_LINKER_SCRIPT}")
    else()
        message(FATAL_ERROR "App linker script not found: ${APP_LINKER_SCRIPT}")
    endif()
endif()

if(NOT DEFINED TARGET_LINKER_SCRIPT)
    # Determine build type
    if(CONFIG_LINK_FLASH)
        set(BUILD_TYPE "flash")
    else()
        set(BUILD_TYPE "ram")
    endif()

    set(BOARD_NAME "ipro6_${BOARD_DIR}")

    # Use generated linker script
    if(NOT DEFINED CONFIG_USE_GENERATED_LINKER_SCRIPT OR CONFIG_USE_GENERATED_LINKER_SCRIPT)
        include(${CMAKE_SOURCE_DIR}/bsp/board/linker/generate_linker_script.cmake)

        generate_linker_script(
            BOARD ${BOARD_NAME}
            BUILD_TYPE ${BUILD_TYPE}
            OUTPUT_DIR ${CMAKE_BINARY_DIR}/linker
        )

        set(LINKER_PATH "${GENERATED_LINKER_SCRIPT}")
        message(STATUS "Using generated linker script: ${LINKER_PATH}")
    else()
        # Option 2: Use static linker script (backward compatibility)
        set(LINKER_FILE "ipro6_${BUILD_TYPE}.ld")
        set(LINKER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${BOARD_DIR}/${LINKER_FILE}")
        message(STATUS "Using static linker script: ${LINKER_PATH}")
    endif()

    __set_linker_script("${LINKER_PATH}")
else()
    if(EXISTS "${TARGET_LINKER_SCRIPT}")
        __set_linker_script("${TARGET_LINKER_SCRIPT}")
    else()
        message(FATAL_ERROR "Cannot find linker script: ${TARGET_LINKER_SCRIPT}")
    endif()
endif()
