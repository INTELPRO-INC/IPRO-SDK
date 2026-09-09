# NMSIS-NN - Nuclei Neural Network Kernel Library for RISC-V
# https://github.com/Nuclei-Software/NMSIS
# Upstream: dc728b988b6562868d311a2e14b5d9f8ebbd26b0 (2026-06-04)
# License: Apache 2.0
#
# DSP acceleration is selected automatically from -march: the upstream header
# riscv_nn_math_types.h keys off __riscv_dsp / __riscv_xxldsp{,n1x,n2x,n3x},
# which the vendor toolchain predefines. No -DRISCV_MATH_DSP is needed.
#
# Headers are vendored under Include/nmsis_nn/ on purpose. components/ai ships
# its own trimmed riscv_nn_types.h whose nmsis_nn_conv_params carries an extra
# filter_offset field, and __component_add_include_dirs() is INTERFACE-scoped
# on the shared __component_lib target - a flat Include/ here would put two
# different riscv_nn_types.h on one global search path and silently misalign
# struct fields. Consumers include "nmsis_nn/riscv_nnfunctions.h".

__generate_library(1)

# Public: parent dir only, so the prefix "nmsis_nn/" is mandatory for consumers.
__component_add_include_dirs(Include)

# Private: flat dir for upstream's own #include "riscv_nnfunctions.h", plus the
# NMSIS Core already shipped in the BSP (verified byte-identical codegen to the
# upstream Core 1.6.0 headers, so no second copy is vendored).
__component_add_private_include_dirs(Include/nmsis_nn)
if(CONFIG_IPRO7)
    __component_add_private_include_dirs(${IPRO_SDK_BASE}/bsp/drivers/ipro7_soc/risc-v/Core/Include)
elseif(CONFIG_IPRO6)
    __component_add_private_include_dirs(${IPRO_SDK_BASE}/bsp/drivers/ipro6_soc/risc-v/Core/Include)
endif()

__component_add_private_compile_options(-O2)
__component_add_private_compile_options(-ffunction-sections)
__component_add_private_compile_options(-fdata-sections)
__component_add_private_compile_options(-Wno-unused-parameter)
__component_add_private_compile_options(-Wno-sign-compare)

file(GLOB_RECURSE NMSIS_NN_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} Source/*.c)
__component_add_sources(${NMSIS_NN_SRCS})
