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

# IPRO6 RAM Configuration
# RAM-based boot configuration for IPRO6 platform (FPGA debug)
#
# IPRO6 uses:
# - IPRO7's CPU (toolchain, startup code)
# - IPRO6 specific memory layout (no PSRAM)
#
# RAM boot mode: LMA = VMA for data sections, so OpenOCD can load
# them directly to RAM without startup code copying from Flash.

set(LINKER_TEMPLATE "ipro6_common.ld.in")

# RAM boot mode - data sections have LMA = VMA (no AT > flash)
set(DATA_LMA_REGION "")

# Flash region
set(FLASH_ORIGIN    "0x12000000")
set(FLASH_SIZE      "2M")

# TCM RAM region (main RAM + nocache share same physical OCRAM)
# Non-cacheable access via 0x21xxxxxx alias (offset 0x10000000)
# Physical OCRAM is 320K. With CONFIG_WIFI_LP_APP_SUPPORT the top 40K
# (0x11046000-0x11050000) is reserved for the standalone lp_fw image (OCRAM is
# retained across PDS31 by HW default) so ram_tcm shrinks to 280K; otherwise it
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
# Canonical HBN-RAM base is 0x20090000 (same as IPRO7 and the vendor ipro6 boot2):
# the bootrom jumps here directly on PDS wake (target taken from HBN_RSV1), so the
# wakeup code MUST be linked at this exact VMA. Must match HBN_RAM_BASE in ipro6.h.
set(HBNRAM_ORIGIN   "0x20090000")
set(HBNRAM_SIZE     "4K")

# Stack and heap
set(STACK_SIZE      "2K")
set(HEAP_SIZE       "2K")  # Not directly used, heap is dynamic

# EM size for BLE controller (0K if not using BLE)
set(EM_SIZE         "0K")

# NVDS not used in IPRO6
set(ENABLE_NVDS     OFF)
