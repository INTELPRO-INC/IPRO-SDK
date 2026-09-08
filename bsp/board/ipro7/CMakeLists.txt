__component_add_include_dirs(.)

# Determine board type for linker script generation
set(BOARD_TYPE "")
if (DEFINED CONFIG_FPGA AND CONFIG_FPGA)
    set(BOARD_TYPE "fpga")
endif()
if ((DEFINED CONFIG_EVB AND CONFIG_EVB) OR (DEFINED CONFIG_DVK AND CONFIG_DVK))
    set(BOARD_TYPE "evb")
endif()
# The IPRO7AI EVK is a standard flash + PSRAM IPRO7 part, so it shares the EVB
# memory layout and therefore the EVB linker scripts.
if (DEFINED CONFIG_IPRO7AI_EVK AND CONFIG_IPRO7AI_EVK)
    set(BOARD_TYPE "evb")
endif()

target_sources(app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/board.c")

# Linker script generation
# Priority:
# 1. App-local linker script specified by CONFIG_LINKER_SCRIPT (Kconfig) or APP_LINKER_SCRIPT (CMake)
# 2. App-local default: apps/<project>/<project>.ld
# 3. Generated linker script (from cmake config)
# 4. Static board linker script (legacy)
#
# To use a specific linker script, set in app's .config:
#   CONFIG_LINKER_SCRIPT="custom_64k.ld"
# Or in app's CMakeLists.txt:
#   set(APP_LINKER_SCRIPT "custom_64k.ld")

# Check for app-local linker script
# Support for nested app directories (e.g., apps/bluetooth/ble/ipro_ble_remote)
if(DEFINED CONFIG_APP_CATEGORY AND NOT CONFIG_APP_CATEGORY STREQUAL "")
    set(APP_DIR "${CMAKE_SOURCE_DIR}/apps/${CONFIG_APP_CATEGORY}/${CONFIG_BUILD_PORJECT}")
else()
    set(APP_DIR "${CMAKE_SOURCE_DIR}/apps/${CONFIG_BUILD_PORJECT}")
endif()

if(DEFINED CONFIG_LINKER_SCRIPT AND NOT CONFIG_LINKER_SCRIPT STREQUAL "")
    # Option 1: Linker script specified via Kconfig
    set(APP_LINKER_SCRIPT "${APP_DIR}/${CONFIG_LINKER_SCRIPT}")
elseif(DEFINED APP_LINKER_SCRIPT AND NOT APP_LINKER_SCRIPT STREQUAL "")
    # Option 2: Linker script specified via CMake variable (relative to app dir)
    if(NOT IS_ABSOLUTE "${APP_LINKER_SCRIPT}")
        set(APP_LINKER_SCRIPT "${APP_DIR}/${APP_LINKER_SCRIPT}")
    endif()
else()
    # Option 3: Default app-local linker script
    set(APP_LINKER_SCRIPT "${APP_DIR}/${CONFIG_BUILD_PORJECT}.ld")
endif()

if(EXISTS "${APP_LINKER_SCRIPT}")
    set(TARGET_LINKER_SCRIPT "${APP_LINKER_SCRIPT}")
    message(STATUS "Using app-local linker script: ${TARGET_LINKER_SCRIPT}")
endif()

if(NOT DEFINED TARGET_LINKER_SCRIPT)
    # Determine build type
    if(CONFIG_LINK_FLASH)
        set(BUILD_TYPE "flash")
    else()
        if(CONFIG_USE_PSRAM)
            set(BUILD_TYPE "psram")
        else()
            set(BUILD_TYPE "ram")
        endif()
    endif()

    # Determine board name (ipro7_evb or ipro7_fpga)
    set(BOARD_NAME "ipro7_${BOARD_TYPE}")

    # Option 1: Use generated linker script (recommended)
    # Default to ON if CONFIG_USE_GENERATED_LINKER_SCRIPT is not defined or if it's enabled
    if(NOT DEFINED CONFIG_USE_GENERATED_LINKER_SCRIPT OR CONFIG_USE_GENERATED_LINKER_SCRIPT)
        include(${CMAKE_SOURCE_DIR}/bsp/board/linker/generate_linker_script.cmake)

        generate_linker_script(
            BOARD ${BOARD_NAME}
            BUILD_TYPE ${BUILD_TYPE}
            OUTPUT_DIR ${CMAKE_BINARY_DIR}/linker
        )

        set(LINKER_PATH "${GENERATED_LINKER_SCRIPT}")
        message(STATUS "Using generated linker script: ${LINKER_PATH}")

        # Note: CONFIG_ENABLE_NVDS is now available from autoconf.h
        # No need to manually define it here
    else()
        # Option 2: Use static linker script (backward compatibility)
        set(LINKER_FILE "ipro7_${BUILD_TYPE}.ld")
        set(LINKER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${BOARD_TYPE}/${LINKER_FILE}")
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

