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

# IPRO7 RAM Configuration
# RAM-based boot configuration with NVDS support
# Applies to all IPRO7 boards (EVB, FPGA, etc.)

set(FLASH_ORIGIN "0x11010000")
set(FLASH_SIZE   "128K")
set(PSRAM_ORIGIN "0x1e400000")
set(PSRAM_SIZE   "8M")
set(OCRAM_ORIGIN "0x11030000")
set(OCRAM_SIZE   "128K")
set(HBNRAM_ORIGIN "0x20090000")
set(HBNRAM_SIZE  "4K")

set(RAM_REGION   "ocram")
set(STACK_SIZE   "2K")
set(HEAP_SIZE    "2K")
set(EM_SIZE      "16K")

# NVDS configuration
# Enable NVDS by default for RAM builds
# Can be overridden by CONFIG_ENABLE_NVDS in proj.conf
set(ENABLE_NVDS  ON)
set(NVDS_SIZE    "1M")
