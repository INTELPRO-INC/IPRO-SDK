# IPRO7 Board Definition
# Copyright (c) 2025 IPRO
# SPDX-License-Identifier: Apache-2.0

# Set IPRO toolchain if available
if(DEFINED ENV{IPRO_TOOLCHAIN_PATH})
    set(CROSS_COMPILE $ENV{IPRO_TOOLCHAIN_PATH}/bin/riscv64-unknown-elf-)
    set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
    set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
    set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc)
endif()

board_runner_args(openocd --config=${BOARD_DIR}/support/openocd.cfg)

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
