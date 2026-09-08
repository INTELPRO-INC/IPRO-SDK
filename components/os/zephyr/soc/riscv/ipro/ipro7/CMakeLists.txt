# Copyright (c) 2025 IPRO
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(.)

# SOC specific initialization
zephyr_library_sources(soc.c)
zephyr_library_sources(board_config.c)

# IPRO7-specific startup code is now integrated into arch/riscv/core/reset.S
# with CONFIG_SOC_IPRO7 guards. This includes:
# - mmisc_ctl CSR initialization (NMI, BPU)
# - mtvec early exception handler
# - OPI unlock (0x30000070)
# - I-Cache enable
# - mcycle/minstret counter enable

# Determine SDK root - try environment variable first, then relative path
if(DEFINED ENV{IPRO_SDK_BASE})
    set(IPRO_SDK_ROOT "$ENV{IPRO_SDK_BASE}")
    message(STATUS "Using IPRO_SDK_BASE from environment: ${IPRO_SDK_ROOT}")
elseif(DEFINED IPRO_SDK_BASE)
    set(IPRO_SDK_ROOT "${IPRO_SDK_BASE}")
    message(STATUS "Using IPRO_SDK_BASE from CMake: ${IPRO_SDK_ROOT}")
else()
    # Fallback: calculate from west workspace (parent of zephyr)
    get_filename_component(ZEPHYR_WORKSPACE "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
    set(IPRO_SDK_ROOT "${ZEPHYR_WORKSPACE}")
    message(STATUS "IPRO_SDK_BASE not set, calculated from workspace: ${IPRO_SDK_ROOT}")
endif()

message(STATUS "IPRO SDK Root: ${IPRO_SDK_ROOT}")

# Include IPRO7 SOC BSP headers
set(IPRO_BSP_DIR "${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/risc-v/Core/Include")
if(EXISTS ${IPRO_BSP_DIR})
    zephyr_include_directories(${IPRO_BSP_DIR})
    message(STATUS "Added IPRO7 BSP include directory: ${IPRO_BSP_DIR}")
else()
    message(WARNING "IPRO7 BSP directory not found: ${IPRO_BSP_DIR}")
endif()

# Include IPRO7 startup and system files
set(IPRO_STARTUP_DIR "${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/startup")
if(EXISTS ${IPRO_STARTUP_DIR})
    # Add system initialization (contains CpuIRegionBase and other globals)
    zephyr_library_sources(${IPRO_STARTUP_DIR}/system_soc.c)
    
    # Add minimal required BSP driver source files for GPIO and other hardware
    # These are needed by Zephyr device drivers but don't depend on FreeRTOS
    zephyr_library_sources(${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/std_drv/src/drv_glb.c)
    zephyr_library_sources(${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/std_drv/src/drv_gpio.c)
    zephyr_library_sources(${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/std_drv/src/drv_hbn.c)
    zephyr_library_sources(${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/std_drv/src/drv_common.c)
    zephyr_library_sources(${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/std_drv/src/drv_clock.c)
    zephyr_library_sources(${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/std_drv/src/drv_ef_ctrl.c)
    # Note: drv_romapi.c and drv_romapi_patch.c have too many dependencies (utils_crc.h etc.)
    # Required functions (EF_Ctrl_*) are provided as stubs in board_config.c
    
    message(STATUS "IPRO7 startup directory found: ${IPRO_STARTUP_DIR}")
    message(STATUS "Note: Using Zephyr native startup for SOC initialization")
else()
    message(WARNING "IPRO7 startup directory not found: ${IPRO_STARTUP_DIR}")
endif()

# Note: We do NOT include platform_common.c, hal_boot2_ipro7.c, or board.c directly
# because they have FreeRTOS-specific dependencies (generated/autoconf.h, peripheral_config.h, etc.)
# Instead, we implement the essential initialization (GPIO pin mux, UART setup) directly in soc.c

# Add board-specific initialization includes for reference (but don't compile)
set(BOARD_CONFIG_DIR "${IPRO_SDK_ROOT}/bsp/board/ipro7")
if(EXISTS ${BOARD_CONFIG_DIR})
    zephyr_include_directories(${BOARD_CONFIG_DIR})
    message(STATUS "Added IPRO7 board include directory (SDK reference only)")
endif()

# Add platform-specific initialization includes for reference (but don't compile)
set(PLATFORM_UTILS_DIR "${IPRO_SDK_ROOT}/bsp/common/platform_utils")
if(EXISTS ${PLATFORM_UTILS_DIR}/include)
    zephyr_include_directories(${PLATFORM_UTILS_DIR}/include)
    message(STATUS "Added platform include directory (SDK reference only)")
endif()

# Add IPRO7 BSP include directories
target_include_directories(zephyr_interface
  INTERFACE
    ${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/risc-v/Core/Include
    ${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/startup
    ${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/hal_drv/inc
    ${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/std_drv/inc
    ${IPRO_SDK_ROOT}/bsp/drivers/ipro7_soc/regs
    ${IPRO_SDK_ROOT}/bsp/drivers/soc_common/inc
    ${IPRO_SDK_ROOT}/bsp/hal/inc
    ${IPRO_SDK_ROOT}/bsp/common/platform_utils/include
)

# Force 32-bit IPRO7 architecture settings
zephyr_compile_options(-march=rv32imafc_zba_zbb_zbc_zbs_xxldsp -mabi=ilp32f)

# Force 32-bit architecture using preprocessor macros
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -UCONFIG_64BIT -DCONFIG_64BIT=0")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -UCONFIG_64BIT -DCONFIG_64BIT=0")

# Link correct 32-bit libgcc directly
zephyr_link_libraries(/opt/toolchain/riscv_ipro7/lib/gcc/riscv64-unknown-elf/13.1.1/rv32imafc_zba_zbb_zbc_zbs_xxldsp/ilp32f/libgcc.a)

# Add IPRO7 DTS bindings
list(APPEND DTS_ROOT "${IPRO_SDK_ROOT}/components/os/zephyr")
