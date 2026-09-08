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

# Configure for 32-bit RISC-V with FPU (as required by toolchain)
set(CMAKE_C_FLAGS "-march=rv32imafc -mabi=ilp32f" CACHE STRING "")
set(CMAKE_CXX_FLAGS "-march=rv32imafc -mabi=ilp32f" CACHE STRING "")
set(CMAKE_ASM_FLAGS "-march=rv32imafc -mabi=ilp32f" CACHE STRING "")

# Point to correct 32-bit libraries with FPU
set(CMAKE_EXE_LINKER_FLAGS "-L/opt/toolchain/riscv_ipro7/lib/gcc/riscv64-unknown-elf/13.1.1/rv32imafc/ilp32f -L/opt/toolchain/riscv_ipro7/riscv64-unknown-elf/lib/rv32imafc/ilp32f -march=rv32imafc -mabi=ilp32f" CACHE STRING "")

board_runner_args(openocd --config=${BOARD_DIR}/support/openocd.cfg)

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
