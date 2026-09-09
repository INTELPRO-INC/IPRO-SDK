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

# IPRO6LE Flash Configuration
# Flash-based boot configuration for IPRO6LE platform
# Applies to all IPRO6LE boards

set(FLASH_ORIGIN "0x13000000")
set(FLASH_SIZE   "4M")
set(PSRAM_ORIGIN "0x1e400000")
set(PSRAM_SIZE   "16M")
set(OCRAM_ORIGIN "0x11010000")
set(OCRAM_SIZE   "256K")
set(HBNRAM_ORIGIN "0x20010000")
set(HBNRAM_SIZE  "4K")

set(RAM_REGION   "psram")
set(STACK_SIZE   "4K")
set(HEAP_SIZE    "2K")
set(EM_SIZE      "16K")

# NVDS configuration
# Enable NVDS by default for all builds
# Can be overridden by CONFIG_ENABLE_NVDS in proj.conf
set(ENABLE_NVDS  ON)
set(NVDS_SIZE    "1M")
