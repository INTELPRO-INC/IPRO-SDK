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

# IPRO7 Flash Configuration
# Flash-based boot configuration without NVDS
# Applies to all IPRO7 boards (EVB, FPGA, etc.)

set(FLASH_ORIGIN "0x13000000")

# Flash size: default 1M (matches on-chip flash on most IPRO7 boards).
# Opt into larger sizes for boards with SF3 external flash (efuse flash_cfg=2):
#   CONFIG_IPRO7_FLASH_2M=y → 2M
#   CONFIG_IPRO7_FLASH_4M=y → 4M
#   CONFIG_IPRO7_FLASH_8M=y → 8M
# Keeping the default at 1M prevents apps from silently growing past the
# physical capacity of mainstream boards. Largest selector wins if multiple set.
if(CONFIG_IPRO7_FLASH_8M)
    set(FLASH_SIZE "8M")
    message(STATUS "IPRO7 flash: 8M (CONFIG_IPRO7_FLASH_8M=y, SF3 external flash)")
elseif(CONFIG_IPRO7_FLASH_4M)
    set(FLASH_SIZE "4M")
    message(STATUS "IPRO7 flash: 4M (CONFIG_IPRO7_FLASH_4M=y, SF3 external flash)")
elseif(CONFIG_IPRO7_FLASH_2M)
    set(FLASH_SIZE "2M")
    message(STATUS "IPRO7 flash: 2M (CONFIG_IPRO7_FLASH_2M=y, SF3 external flash)")
elseif(CONFIG_IPRO7_FLASH_1M)
    set(FLASH_SIZE "1M")
    message(STATUS "IPRO7 flash: 1M (CONFIG_IPRO7_FLASH_1M=y, on-chip)")
else()
    set(FLASH_SIZE "1M")
    message(STATUS "IPRO7 flash: 1M (default, on-chip)")
endif()

set(PSRAM_ORIGIN "0x1e000000")
set(PSRAM_SIZE   "8M")

# Memory Layout based on AI/MM support:
# 1. AI=y, MM=y, EM_SIZE=0:
#     OCRAM: 0x11010000, 64KB  (banks 0-3, ends exactly at the NPU sram4
#            remap base 0x11020000 — same layout as ipro7_uvc_cam's .ld)
#     em_tcm_ret: 0x1102C000, 16KB (tcmcode split)
# 2. AI=y, MM=y, EM_SIZE>0:
#     OCRAM: 0x11010000, 48KB
#     ocram_noret: 0x11020000, 64KB-EM_SIZE
#     (no em_tcm_ret, all tcmcode in ocram)
# 3. AI=n:
#     OCRAM: 0x11010000, 256KB-EM_SIZE
if(CONFIG_AI_SUPPORT AND CONFIG_MM_SUPPORT)
    set(OCRAM_ORIGIN "0x11010000")

    if(NOT DEFINED CONFIG_EM_SIZE OR CONFIG_EM_SIZE EQUAL 0)
        # Case 1: AI=y, MM=y, EM_SIZE=0
        #
        # OCRAM uses the full 64 KB of banks 0-3 (0x11010000-0x11020000).
        # The NPU sram4 remap (cr_blai_en=1) and em_tcm_ret both live at or
        # above 0x11020000, so banks 0-3 are free for the linker — the old
        # 48 KB left a 16 KB hole (0x1101C000-0x11020000) unused, which made
        # heavier no-PSRAM-retention demos (e.g. ipro7_demo: ~24 KB explicit
        # .ocram_bss + stack + tcm) overflow the region. 64 KB matches the
        # proven ipro7_uvc_cam custom .ld layout.
        set(OCRAM_SIZE "64K")

        # Check if user wants to disable em_tcm_ret (merge TCM code into OCRAM)
        if(CONFIG_DISABLE_EM_TCM_RET)
            set(ENABLE_EM_TCM_RET OFF)
            set(ENABLE_OCRAM_NORET OFF)
            message(STATUS "AI+MM (EM=0): OCRAM=64KB, em_tcm_ret=DISABLED (TCM merged into OCRAM)")
        else()
            # 0x1102C000..0x11030000 is 16 KB, which is not enough for every
            # AI+MM demo: ipro7_demo's .tcmcode alone is 19 KB and overflowed
            # by 2.6 KB. The region runs to the end of OCRAM at 0x11050000,
            # and the NPU's sram4 remap sits above that, so extending it here
            # takes space nothing else in this layout can reach - OCRAM proper
            # is capped at banks 0-3 regardless.
            set(EM_TCM_RET_ORIGIN "0x1102C000")
            sdk_ifndef(CONFIG_EM_TCM_RET_SIZE_KB 32)
            set(EM_TCM_RET_SIZE "${CONFIG_EM_TCM_RET_SIZE_KB}K")
            set(ENABLE_EM_TCM_RET ON)
            set(ENABLE_OCRAM_NORET OFF)
            message(STATUS "AI+MM (EM=0): OCRAM=64KB, em_tcm_ret=${CONFIG_EM_TCM_RET_SIZE_KB}KB")
        endif()
    else()
        # Case 2: AI=y, MM=y, EM_SIZE>0
        set(OCRAM_SIZE "48K")
        set(ENABLE_EM_TCM_RET OFF)
        set(ENABLE_OCRAM_NORET OFF)
        set(OCRAM_NORET_ORIGIN "0x11020000")
        math(EXPR OCRAM_NORET_SIZE_KB "64 - ${CONFIG_EM_SIZE}")
        set(OCRAM_NORET_SIZE "${OCRAM_NORET_SIZE_KB}K")
        message(STATUS "AI+MM (EM=${CONFIG_EM_SIZE}K): OCRAM=48KB, ocram_noret=${OCRAM_NORET_SIZE_KB}KB")
    endif()
else()
    # Case 3: AI=n (or MM=n)
    set(OCRAM_ORIGIN "0x11010000")
    set(ENABLE_EM_TCM_RET OFF)
    set(ENABLE_OCRAM_NORET OFF)

    if(DEFINED CONFIG_EM_SIZE AND CONFIG_EM_SIZE GREATER 0)
        math(EXPR OCRAM_SIZE_KB "256 - ${CONFIG_EM_SIZE}")
        set(OCRAM_SIZE "${OCRAM_SIZE_KB}K")
        message(STATUS "AI disabled: OCRAM=${OCRAM_SIZE_KB}KB (256-${CONFIG_EM_SIZE})")
    else()
        set(OCRAM_SIZE "256K")
        message(STATUS "AI disabled: OCRAM=256KB")
    endif()

    # Low-power split: 64 KB retention + 192 KB non-retention.
    #
    # Only banks 0-3 of OCRAM have HW retention bits in PDS_RAM_CFG
    # (0x3000008c) on this IC. Splitting the linker layout into a
    # retention-only RAM region + an explicit `ocram_noret` region lets
    # small LP apps (sed_lp, ble_lp) keep their tasks/stacks alive with
    # PDS_RAM_CFG retention bits set, without needing `misc_keep=true`.
    #
    # OPT-IN with `CONFIG_OCRAM_NORET_SPLIT=y`. Apps larger than 64 KB
    # OCRAM (Thread MTD with OT message buffers / mbedTLS tables / OT
    # task stack) need the full 256 KB as a single region and rely on
    # `pm_pds_set_misc_keep(true)` for retention — leave the flag unset
    # there so the linker treats OCRAM as one 256 KB block.
    # NOTE: the split is a pure memory LAYOUT (front-64K + middle + EM-at-top).
    # It does NOT require the low-power framework — CONFIG_HAL_LP only matters
    # at runtime (PDS_RAM_CFG retention bits) for apps that actually sleep. A
    # no-PSRAM app that just needs the bulk of OCRAM as usable RAM (e.g. a
    # Matter MED that never sleeps) opts in with CONFIG_OCRAM_NORET_SPLIT alone.
    # (Coupling it to HAL_LP forced apps to pull in the LP framework, whose
    # app-side init hangs the prebuilt non-LP BLE controller.)
    if(NOT CONFIG_USE_PSRAM AND CONFIG_OCRAM_NORET_SPLIT)
        # IPRO7 retains TWO regions across PDS31:
        #   - Front 64 KB (banks 0-3, controlled by PDS_RAM_CFG bits 0-3)
        #     0x11010000 ~ 0x11020000
        #   - Back 16 KB (banks 15)
        #     0x1104C000 ~ 0x11050000
        # Middle 176 KB is power-gated and wiped every wake. Split the
        # linker layout to expose both retention regions so apps can
        # spread retention-critical .bss across them.
        set(OCRAM_SIZE "64K")
        set(ENABLE_OCRAM_NORET ON)
        set(OCRAM_NORET_ORIGIN "0x11020000")
        # EM (BLE Exchange Memory) is carved from the TOP of OCRAM downward.
        # For EM<=16K it lands EXACTLY in the back-16 KB retention region
        # (0x1104C000~0x11050000) — i.e. EM *is* ocram_high. So the middle
        # 176 KB (0x11020000~0x1104C000) is fully available as ocram_noret;
        # EM does NOT shrink it. Only EM>16K intrudes into the middle, costing
        # (EM-16)K of noret. (The previous `176 - EM` double-counted: it both
        # carved a 16K gap out of the middle AND exposed the real EM region as
        # ocram_high, wasting 16 KB of RAM.)
        if(DEFINED CONFIG_EM_SIZE AND CONFIG_EM_SIZE GREATER 16)
            math(EXPR OCRAM_NORET_SIZE_KB "192 - ${CONFIG_EM_SIZE}")
        else()
            set(OCRAM_NORET_SIZE_KB "176")
        endif()
        set(OCRAM_NORET_SIZE "${OCRAM_NORET_SIZE_KB}K")
        if(DEFINED CONFIG_EM_SIZE AND CONFIG_EM_SIZE GREATER 0)
            # The back-16 KB is the EM (hardware-managed). Do NOT expose it as a
            # software-usable retention region, or app .bss_ocram_high would be
            # clobbered by the BLE controller.
            set(ENABLE_OCRAM_HIGH OFF)
            message(STATUS "LP no-PSRAM split: ocram=64KB(ret) + ocram_noret=${OCRAM_NORET_SIZE_KB}KB + EM=${CONFIG_EM_SIZE}KB(back, ocram_high)")
        else()
            # EM=0: the back-16 KB is a second software retention region.
            set(ENABLE_OCRAM_HIGH ON)
            set(OCRAM_HIGH_ORIGIN "0x1104C000")
            set(OCRAM_HIGH_SIZE "16K")
            message(STATUS "LP no-PSRAM split: ocram=64KB(ret) + ocram_noret=${OCRAM_NORET_SIZE_KB}KB + ocram_high=16KB(ret)")
        endif()
    endif()
endif()

set(HBNRAM_ORIGIN "0x20090000")
set(HBNRAM_SIZE  "4K")

# Default RAM region.
# - "ocram" (64 KB retention): small LP apps where all .bss should
#   survive PDS31 (sed_lp, ble_lp).
# - "ocram_noret" (192 KB non-retention): larger apps where the bulk
#   of .bss / .data doesn't need retention; critical retention items
#   are individually pinned via __attribute__((section(".ocram_bss"))).
#   Opt in with CONFIG_OCRAM_NORET_AS_RAM=y (requires
#   CONFIG_OCRAM_NORET_SPLIT=y).
if(CONFIG_OCRAM_NORET_SPLIT AND CONFIG_OCRAM_NORET_AS_RAM)
    set(RAM_REGION   "ocram_noret")
    message(STATUS "RAM region: ocram_noret (192 KB, non-retention bulk)")
else()
    set(RAM_REGION   "ocram")
endif()
# Main (pre-scheduler / MSP) stack. Default 2K, but some init paths that run
# in main() before the scheduler need much more — e.g. ble_controller_init ->
# rwip_init -> rwip_driver_init needs ~16K (see arch_main.c). On a no-PSRAM
# build the heap sits immediately after .stack in OCRAM, so a main-stack
# overflow silently corrupts the heap free-list (next malloc hangs). Apps can
# raise it with CONFIG_MAIN_STACK_SIZE_KB.
if(DEFINED CONFIG_MAIN_STACK_SIZE_KB AND CONFIG_MAIN_STACK_SIZE_KB GREATER 0)
    set(STACK_SIZE "${CONFIG_MAIN_STACK_SIZE_KB}K")
else()
    set(STACK_SIZE "2K")
endif()
set(HEAP_SIZE    "2K")

# EM_SIZE for linker script (__EM_SIZE symbol)
# Should match CONFIG_EM_SIZE from proj.conf for hardware configuration
if(DEFINED CONFIG_EM_SIZE)
    set(EM_SIZE "${CONFIG_EM_SIZE}K")
else()
    # Default: 0KB (no EM for non-BLE applications)
    set(EM_SIZE "0K")
endif()

# NVDS configuration
# Enable NVDS by default for all builds
# Can be overridden by CONFIG_ENABLE_NVDS in proj.conf
# RAM MODE: 16KB allocated from OCRAM
# set(ENABLE_NVDS  ON)
# set(NVDS_SIZE    "16K")
