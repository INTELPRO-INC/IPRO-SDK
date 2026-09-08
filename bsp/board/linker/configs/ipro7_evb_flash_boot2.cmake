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

# IPRO7 EVB Flash + Boot2 Configuration
# All data in PSRAM — boot2 has already initialized PSRAM before FW starts.
# "OCRAM" region is mapped to high PSRAM to eliminate physical OCRAM usage.

set(FLASH_ORIGIN "0x13000000")
set(FLASH_SIZE   "1M")
set(PSRAM_ORIGIN "0x1e000000")
set(PSRAM_SIZE   "7M")

# Map "OCRAM" to top of PSRAM (7M offset) — boot2 already initialized PSRAM
# This puts stack, tcm_code, noinit_data, ocram_data in PSRAM physical memory
set(OCRAM_ORIGIN "0x1e700000")
set(OCRAM_SIZE   "256K")

set(HBNRAM_ORIGIN "0x20090000")
set(HBNRAM_SIZE  "4K")

set(RAM_REGION   "psram")
set(STACK_SIZE   "2K")
set(HEAP_SIZE    "2K")
set(EM_SIZE      "0K")

# No nocache offset — "OCRAM" is actually PSRAM, non-cacheable alias not applicable
set(NOCACHE_ADDR_OFFSET "0")
