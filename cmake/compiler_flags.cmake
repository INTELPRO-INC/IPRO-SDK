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

# C++ standard for the whole tree. Components that need a newer one (TFLM
# is C++17) set IPRO_CXX_STD before this file is included; everything else
# keeps the historical c++11 so no existing component changes behaviour.
if(NOT DEFINED IPRO_CXX_STD)
  if(CONFIG_COMPONENTS_TFLM_ENABLE)
    set(IPRO_CXX_STD c++17)
  else()
    set(IPRO_CXX_STD c++11)
  endif()
endif()

__component_add_compile_options(
-fno-jump-tables
-fno-common
-fno-builtin-printf
-fno-builtin-puts
-fno-builtin-putchar
-fms-extensions
-ffunction-sections
-fdata-sections
-fmessage-length=0
-Wall
-Wchar-subscripts
-Wformat
-Wundef
-Wuninitialized
-Winit-self
-Wignored-qualifiers
-fstrict-volatile-bitfields
-fshort-enums
-MMD
$<$<COMPILE_LANGUAGE:C>:-std=gnu99>
$<$<COMPILE_LANGUAGE:CXX>:-std=${IPRO_CXX_STD}>
$<$<COMPILE_LANGUAGE:CXX>:-nostdlib>
$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

__component_add_link_options(
-Wl,--cref
-Wl,--gc-sections
-Wl,--print-memory-usage
-nostartfiles
-fms-extensions
-ffunction-sections
-fdata-sections
--specs=nano.specs
)

__component_add_link_libraries(c m)
__component_add_compile_options_ifdef(CONFIG_DEBUG -g3)
__component_add_compile_options(-Os)

# Frame pointer support for better debugging (controlled by Kconfig)
__component_add_compile_options_ifdef(CONFIG_ENABLE_FRAME_POINTER -fno-omit-frame-pointer)

# Single-precision float: treat unsuffixed float literals as float, not double
__component_add_compile_options_ifdef(CONFIG_SINGLE_PRECISION_FLOAT -fsingle-precision-constant)

