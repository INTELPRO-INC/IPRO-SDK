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

# IPRO6 Flash Configuration
# Flash-based boot configuration for IPRO6 platform
#
# IPRO6 uses:
# - IPRO7's CPU (toolchain, startup code)
# - IPRO6 specific memory layout (no PSRAM)
#
# Flash boot mode: Data sections have LMA in Flash, VMA in RAM.
# Startup code copies data from Flash to RAM before main().

set(LINKER_TEMPLATE "ipro6_common.ld.in")

# Flash boot - data sections have LMA in flash, VMA in RAM
# Startup code copies data from Flash to RAM before main()
set(DATA_LMA_REGION "AT > flash")

# Flash region
set(FLASH_ORIGIN    "0x13000000")
set(FLASH_SIZE      "2M")

# TCM RAM region (main RAM + nocache share same physical OCRAM)
# Non-cacheable access via 0x21xxxxxx alias (offset 0x10000000)
# Physical OCRAM is 320K. With CONFIG_WIFI_LP_APP_SUPPORT the top 40K
# (0x11046000-0x11050000) is reserved for the standalone lp_fw image, which must
# live in OCRAM (retained across PDS31 by the HW default) rather than WRAM (WiFi
# power domain, gated off during PDS), so ram_tcm shrinks to 280K; otherwise it
# keeps the full 320K. MUST match ipro6_lpfw.ld ORIGIN/LENGTH and LPFW_RUN_ADDR.
set(RAM_TCM_ORIGIN  "0x11000000")
if(CONFIG_WIFI_LP_APP_SUPPORT)
    set(RAM_TCM_SIZE    "280K")
else()
    set(RAM_TCM_SIZE    "320K")
endif()

# WiFi RAM region
set(RAM_WIFI_ORIGIN "0x21050000")
set(RAM_WIFI_SIZE   "160K")

# HBN RAM region (hibernate retention).
# Must match HBN_RAM_BASE in ipro6.h so the .hbn wakeup code links/executes and
# is addressed consistently with the C code that copies and jumps to it. The
# bootrom jumps here directly on PDS wake (from HBN_RSV1), so the wakeup code
# MUST be linked at this same VMA the silicon/bootrom expects (0x20090000).
set(HBNRAM_ORIGIN   "0x20090000")
set(HBNRAM_SIZE     "4K")

# Stack and heap
set(STACK_SIZE      "2K")
set(HEAP_SIZE       "2K")

# EM size for BLE controller (0K if not using BLE)
set(EM_SIZE         "0K")

# NVDS not used in IPRO6
set(ENABLE_NVDS     OFF)
