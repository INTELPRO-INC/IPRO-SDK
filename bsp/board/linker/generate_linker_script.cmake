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

# Generate Linker Script from Configuration
# This CMake module provides functions to generate linker scripts
# based on board and build type configurations

# Usage:
#   include(${CMAKE_CURRENT_SOURCE_DIR}/bsp/board/linker/generate_linker_script.cmake)
#   generate_linker_script(
#       BOARD ipro7_evb
#       BUILD_TYPE ram
#       OUTPUT_DIR ${CMAKE_BINARY_DIR}/linker
#   )

function(generate_linker_script)
    set(options "")
    set(oneValueArgs BOARD BUILD_TYPE OUTPUT_DIR)
    set(multiValueArgs "")

    cmake_parse_arguments(GEN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Validate required arguments
    if(NOT GEN_BOARD)
        message(FATAL_ERROR "BOARD argument is required")
    endif()

    if(NOT GEN_BUILD_TYPE)
        message(FATAL_ERROR "BUILD_TYPE argument is required")
    endif()

    if(NOT GEN_OUTPUT_DIR)
        set(GEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/linker)
    endif()

    # Construct configuration file path with fallback mechanism
    # Use CMAKE_SOURCE_DIR to get absolute path to linker directory
    set(LINKER_SCRIPT_DIR "${CMAKE_SOURCE_DIR}/bsp/board/linker")
    set(OUTPUT_FILE "${GEN_OUTPUT_DIR}/${GEN_BOARD}_${GEN_BUILD_TYPE}.ld")

    # Extract chip name from board name (e.g., ipro7_evb -> ipro7)
    string(REGEX REPLACE "^([^_]+)_.*" "\\1" CHIP_NAME "${GEN_BOARD}")

    # Try to find configuration file with fallback:
    # 1. First try: <chip>_<board>_<type>.cmake (e.g., ipro7_evb_ram.cmake)
    # 2. Fallback: <chip>_<type>.cmake (e.g., ipro7_ram.cmake)
    set(SPECIFIC_CONFIG "${LINKER_SCRIPT_DIR}/configs/${GEN_BOARD}_${GEN_BUILD_TYPE}.cmake")
    set(GENERIC_CONFIG "${LINKER_SCRIPT_DIR}/configs/${CHIP_NAME}_${GEN_BUILD_TYPE}.cmake")

    if(EXISTS ${SPECIFIC_CONFIG})
        set(CONFIG_FILE ${SPECIFIC_CONFIG})
        set(CONFIG_TYPE "board-specific")
    elseif(EXISTS ${GENERIC_CONFIG})
        set(CONFIG_FILE ${GENERIC_CONFIG})
        set(CONFIG_TYPE "chip-generic")
    else()
        message(FATAL_ERROR "Configuration file not found:\n"
                "  Tried: ${SPECIFIC_CONFIG}\n"
                "  Tried: ${GENERIC_CONFIG}")
    endif()

    # Create output directory if it doesn't exist
    file(MAKE_DIRECTORY ${GEN_OUTPUT_DIR})

    # Load board configuration
    include(${CONFIG_FILE})

    # IPRO6 controller applications normally reserve 32 KiB of the shared
    # WRAM/EM macro. Keep that platform default, but allow the BTDM validation
    # application to qualify the hardware-supported 64 KiB split.
    if(DEFINED CONFIG_BTDM_EM_SIZE_64K AND CONFIG_BTDM_EM_SIZE_64K)
        set(IPRO6_CONTROLLER_EM_SIZE "64K")
    else()
        set(IPRO6_CONTROLLER_EM_SIZE "32K")
    endif()

    # Determine template file path
    # Config files can optionally specify LINKER_TEMPLATE variable
    # Default: ${CHIP_NAME}_common.ld.in
    if(NOT DEFINED LINKER_TEMPLATE)
        set(LINKER_TEMPLATE "${CHIP_NAME}_common.ld.in")
    endif()

    # Look for template in chip-specific directory
    set(TEMPLATE_FILE "${LINKER_SCRIPT_DIR}/${CHIP_NAME}/${LINKER_TEMPLATE}")

    # Check if template file exists
    if(NOT EXISTS ${TEMPLATE_FILE})
        message(FATAL_ERROR "Template file not found: ${TEMPLATE_FILE}\n"
                "  Chip: ${CHIP_NAME}\n"
                "  Template: ${LINKER_TEMPLATE}\n"
                "  Expected path: ${TEMPLATE_FILE}")
    endif()

    # Handle PSRAM configuration
    # Two combinations:
    # 1. CONFIG_USE_PSRAM=ON  -> PSRAM + OCRAM (RAM points to PSRAM for default data/bss)
    # 2. CONFIG_USE_PSRAM=OFF -> OCRAM only (RAM points to OCRAM)
    if(DEFINED CONFIG_USE_PSRAM AND CONFIG_USE_PSRAM)
        # Combination 1: PSRAM + OCRAM
        # Default .data/.bss go to PSRAM, explicit .ocram_* go to OCRAM
        set(PSRAM_MEMORY_REGION "  psram     (rwxa) : ORIGIN = ${PSRAM_ORIGIN}, LENGTH = ${PSRAM_SIZE}")

        # Override RAM_REGION to point to PSRAM for default data/bss
        set(RAM_REGION "psram")

        # Add PSRAM code section - executable code in PSRAM (backward compat)
        set(PSRAM_CODE_SECTION "
  /* PSRAM code section - executable code in PSRAM */
  .text_psram : ALIGN(8)
  {
    PROVIDE( _text_psram_lma = LOADADDR(.text_psram) );
    PROVIDE( _text_psram_vma = ADDR(.text_psram) );
    *(.psram_code)
    *(.psram_code.*)
    . = ALIGN(8);
    PROVIDE( _etext_psram = . );
  } >psram AT>ROM
")

        # Add PSRAM data section - initialized data in PSRAM (backward compat)
        set(PSRAM_DATA_SECTION "
  /* PSRAM data section - initialized data in PSRAM (backward compat) */
  .data_psram : ALIGN(8)
  {
    PROVIDE( _data_psram_lma = LOADADDR(.data_psram) );
    PROVIDE( _data_psram_vma = ADDR(.data_psram) );
    *(.psram_data)
    *(.psram_data.*)
    . = ALIGN(8);
    PROVIDE( _edata_psram = . );
  } >psram AT>ROM
")

        # Add PSRAM bss section - uninitialized data in PSRAM (backward compat)
        set(PSRAM_BSS_SECTION "
  /* PSRAM bss section - uninitialized data in PSRAM (backward compat) */
  .bss_psram (NOLOAD) : ALIGN(8)
  {
    PROVIDE( _bss_psram_start = . );
    *(.psram_bss)
    *(.psram_bss.*)
    *(.lwip_mem*)
    . = ALIGN(8);
    PROVIDE( _bss_psram_end = . );
  } >psram
")

        # PSRAM heap starts after every reserved PSRAM section.  When
        # CONFIG_NVDS_IN_PSRAM=y, .nvds is intentionally placed after
        # .bss_psram in section order; anchoring the heap on _bss_psram_end
        # would let heap_5 allocate over the NVDS mirror and corrupt it with
        # stack canaries before the BLE controller walks the tags.
        set(PSRAM_PROVIDE_START "  PROVIDE( __heap_psram_start = MAX(_bss_psram_end, __nvds_end) );")
        set(PSRAM_PROVIDE_SIZE  "  PROVIDE( __heap_psram_size = ORIGIN(psram) + LENGTH(psram) - __heap_psram_start );")
        set(PSRAM_PROVIDE_END   "  PROVIDE( __heap_psram_end = __heap_psram_start + __heap_psram_size );")

        # Add OCRAM data section - initialized data in fast OCRAM (new)
        # Includes .sdata for GP-relative addressing (GP must be valid before PSRAM init)
        set(OCRAM_DATA_SECTION "
  /* OCRAM data section - initialized data in fast OCRAM */
  /* Includes .sdata because GP-relative addressing must work before PSRAM init */
  .data_ocram : ALIGN(8)
  {
    PROVIDE( _data_ocram_lma = LOADADDR(.data_ocram) );
    PROVIDE( _data_ocram_vma = ADDR(.data_ocram) );
    *(.ocram_data)
    *(.ocram_data.*)
    . = ALIGN(8);
    /* Global pointer for small data access - must be in OCRAM for early boot */
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.* .sdata*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
    PROVIDE( _edata_ocram = . );
  } >ocram AT>ROM
")

        # Add OCRAM bss section - uninitialized data in fast OCRAM (new)
        # Includes .sbss for GP-relative addressing (GP must be valid before PSRAM init)
        set(_BTCTRL2_PDS_RETAIN_BSS "")
        if(CONFIG_BTCTRL2_PDS_SNAPSHOT_EXPERIMENTAL)
            set(_BTCTRL2_PDS_RETAIN_BSS "
    /* btctrl2 PDS31 experiment: controller software state is resumed after
     * wake and therefore must not land above the front-64K retention limit. */
    *libbtctrl2.a:(.bss .bss.* .sbss .sbss.*)")
        endif()
        set(OCRAM_BSS_SECTION "
  /* OCRAM bss section - uninitialized data in fast OCRAM */
  /* Includes .sbss because GP-relative addressing must work before PSRAM init */
  .bss_ocram (NOLOAD) : ALIGN(8)
  {
    PROVIDE( _bss_ocram_start = . );
    *(.ocram_bss)
    *(.ocram_bss.*)
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    . = ALIGN(8);
    PROVIDE( _bss_ocram_end = . );
  } >ocram
")
        # Track end of OCRAM data sections - before switching to PSRAM
        set(OCRAM_DATA_END_MARKER "
  /* Track OCRAM position before default data/bss go to PSRAM */
  __ocram_data_end = .;
")
    else()
        # Combination 2: OCRAM only
        set(PSRAM_MEMORY_REGION "  /* PSRAM disabled - CONFIG_USE_PSRAM not set */")

        # When PSRAM is disabled, still provide sections for code that uses PSRAM attributes
        # These go to RAM (OCRAM) instead
        set(PSRAM_CODE_SECTION "
  /* PSRAM code section - placed in RAM when PSRAM disabled */
  .text_psram : ALIGN(8)
  {
    PROVIDE( _text_psram_lma = LOADADDR(.text_psram) );
    PROVIDE( _text_psram_vma = ADDR(.text_psram) );
    *(.psram_code)
    *(.psram_code.*)
    . = ALIGN(8);
    PROVIDE( _etext_psram = . );
  } >RAM AT>ROM
")

        set(PSRAM_DATA_SECTION "
  /* PSRAM data section - placed in RAM when PSRAM disabled */
  .data_psram : ALIGN(8)
  {
    PROVIDE( _data_psram_lma = LOADADDR(.data_psram) );
    PROVIDE( _data_psram_vma = ADDR(.data_psram) );
    *(.psram_data)
    *(.psram_data.*)
    . = ALIGN(8);
    PROVIDE( _edata_psram = . );
  } >RAM AT>ROM
")

        set(PSRAM_BSS_SECTION "
  /* PSRAM bss section - placed in RAM when PSRAM disabled.
   * No explicit address: .stack is emitted before this and IS explicitly addressed
   * (__stack_start_addr), so the >RAM counter has already been advanced past both the
   * physical OCRAM occupied by the noncacheable sections -- whose VMA is in the 0x21...
   * alias and so does not advance that counter on its own -- AND past the stack.
   * Hard-coding __ocram_cacheable_end + __nocache_total_size here instead would drag the
   * location counter back to the front of the stack, and __ocram_heap_start (computed
   * from '.' further down) would hand heap_5 a region that overlaps live stack. */
  .bss_psram (NOLOAD) : ALIGN(8)
  {
    PROVIDE( _bss_psram_start = . );
    *(.psram_bss)
    *(.psram_bss.*)
    *(.lwip_mem*)
    . = ALIGN(8);
    PROVIDE( _bss_psram_end = . );
  } >RAM
")
        set(PSRAM_PROVIDE_START "")
        set(PSRAM_PROVIDE_SIZE "")
        set(PSRAM_PROVIDE_END   "")
        
        # OCRAM sections - even when PSRAM disabled, startup code needs these symbols
        # `.data_ocram` holds the ECLIC vector_table (ATTR_OCRAM_SECTION).
        # In split mode the default RAM alias is ocram_noret (wiped on
        # PDS); the vector_table MUST land in retention so post-wake IRQ
        # dispatch finds valid handler pointers — otherwise SysTimer
        # interrupts dispatch through NULL, scheduler tick stops, and
        # FreeRTOS timeouts never fire (observed 2026-05-20: OT task
        # hangs forever inside ulTaskNotifyTake post-PDS).
        if(ENABLE_OCRAM_NORET)
            set(_OCRAM_DATA_REGION "ocram")
        else()
            set(_OCRAM_DATA_REGION "RAM")
        endif()
        set(OCRAM_DATA_SECTION "
  /* OCRAM data section - initialized data in OCRAM */
  .data_ocram : ALIGN(8)
  {
    PROVIDE( _data_ocram_lma = LOADADDR(.data_ocram) );
    PROVIDE( _data_ocram_vma = ADDR(.data_ocram) );
    *(.ocram_data)
    *(.ocram_data.*)
    . = ALIGN(8);
    PROVIDE( _edata_ocram = . );
  } >${_OCRAM_DATA_REGION} AT>ROM
")
        # Pick the memory region for .ocram_bss explicitly: it must land
        # in the retention banks (0-3, 0x11010000-0x11020000) which the
        # PDS_RAM_CFG bits cover. With CONFIG_OCRAM_NORET_AS_RAM the
        # default RAM alias points at ocram_noret (non-retention), so
        # `> RAM` would defeat the whole purpose. Override to `> ocram`
        # whenever ocram_noret is split out as a separate region.
        if(ENABLE_OCRAM_NORET)
            set(_OCRAM_BSS_REGION "ocram")
        else()
            set(_OCRAM_BSS_REGION "RAM")
        endif()
        set(_BTCTRL2_PDS_RETAIN_BSS "")
        if(CONFIG_BTCTRL2_PDS_SNAPSHOT_EXPERIMENTAL)
            set(_BTCTRL2_PDS_RETAIN_BSS "
    /* btctrl2 PDS31 experiment: retain all controller software state. */
    *libbtctrl2.a:(.bss .bss.* .sbss .sbss.*)
    *libshell.a:(.bss .bss.* .sbss .sbss.*)")
        endif()
        set(OCRAM_BSS_SECTION "
  /* OCRAM bss section — pinned to the retention region.
   *
   * Under CONFIG_OCRAM_NORET_SPLIT the first 64 KB of OCRAM (banks 0-3)
   * is retained across PDS31 via PDS_RAM_CFG bits 0-3; the remaining
   * 192 KB (ocram_noret) is power-gated. Symbols emitted into
   * .ocram_bss via __attribute__((section(\".ocram_bss\"))) MUST go
   * into the retained region.
   *
   * The whole-archive `*libfreertos.a:` glob pulls FreeRTOS's internal
   * state (pxCurrentTCB, xReadyTasksLists, xDelayedTaskList, timer
   * task list, event group, queue registry, …) into retention without
   * having to source-modify tasks.c / queue.c / timers.c. After every
   * PDS31 wake the scheduler dereferences pxCurrentTCB to resume the
   * pre-sleep task; if that pointer or the lists it walks are wiped,
   * post-wake context switch jumps into garbage and the CPU faults
   * (observed 2026-05-20 with stack=ocram_noret splits). */
  .bss_ocram (NOLOAD) : ALIGN(8)
  {
    PROVIDE( _bss_ocram_start = . );
    *(.ocram_bss)
    *(.ocram_bss.*)
    /* Matter PDS experiment (2026-06-01): the CHIP-bundled OpenThread
     * instance + OT heap (ot::gInstanceRaw / ot::sHeapRaw) live in the CHIP
     * prebuilt's default .bss, so without this they land in ocram_noret and
     * are wiped on every PDS31 wake — corrupting OT MAC/MLE state (observed
     * as the sed_fastpoll snapshot pan/EUI flapping). Pull just these two
     * per-object sections (`-fdata-sections` in the CHIP .a) into retention.
     * Harmless for non-Matter apps: the sections don't exist → no match. */
    *(.bss._ZN2ot12gInstanceRawE)
    *(.bss._ZN2ot8sHeapRawE)
    *libfreertos.a:(.bss .bss.* .sbss .sbss.*)
    /* HAL + radio driver state — tx_in_flight, lmac154 monitoring
     * counters, hal_state.config (cached OT-level config), g_rtc_*
     * (used by hal_lp_enter result fill), wakeup hook table, flash
     * config, etc. All of these are written pre-PDS and read post-PDS
     * by code resuming on the same logical task. */
    *libhal.a:(.bss .bss.* .sbss .sbss.*)
    *libieee802154.a:(.bss .bss.* .sbss .sbss.*)
    *libIPRO7_soc.a:(.bss .bss.* .sbss .sbss.*)
    *libipro7_rf.a:(.bss .bss.* .sbss .sbss.*)
    ${_BTCTRL2_PDS_RETAIN_BSS}
    /* App-level statics (s_ot_instance, s_thread_active, s_fp_started
     * — written during init / OT callbacks and read by sed_fastpoll
     * after PDS wake). */
    *libapp.a:(.bss .bss.* .sbss .sbss.*)
    /* HW AES / GCM handles (g_aes_handle, g_gcm_aes_handle) — opaque
     * device-state structures the SEC AES driver dereferences on every
     * call. Wiping them post-PDS makes mbedtls_ccm_setkey stall inside
     * the hardware setup path the next time the engine builds a Data
     * Request frame. Only the hw-accel mbedtls port objects come in
     * here; the bulky SW AES tables (RT0-3, FT0-3) are excluded when
     * CONFIG_MBEDTLS_AES_USE_HW is on. */
    *aes_alt.c.o(.bss .bss.* .sbss .sbss.*)
    *gcm_alt.c.o(.bss .bss.* .sbss .sbss.*)
    *ccm_alt.c.o(.bss .bss.* .sbss .sbss.*)
    . = ALIGN(8);
    PROVIDE( _bss_ocram_end = . );
  } >${_OCRAM_BSS_REGION}
")
        # Second retention region — last 16 KB of OCRAM (0x1104C000 ~
        # 0x11050000). Hardware retains this bank across PDS31 alongside
        # the first 64 KB. Used here for the OpenThread runtime state
        # which is too big to fit in the front 64 KB alongside FreeRTOS /
        # HAL / .tcmcode.
        if(ENABLE_OCRAM_HIGH)
            set(OCRAM_HIGH_MEMORY_REGION
                "  ocram_high (wxa!r) : ORIGIN = ${OCRAM_HIGH_ORIGIN}, LENGTH = ${OCRAM_HIGH_SIZE}")
            set(OCRAM_HIGH_BSS_SECTION "
  /* OCRAM HIGH bss section — second retention region (last 16 KB).
   * OpenThread runtime state pinned here: gInstanceRaw plus
   * libopenthread-mtd / libopenthread_port .bss .sbss are too large
   * (~10 KB combined) for the front 64 KB region but easily fit in
   * the back 16 KB. Linker keeps them out of ocram_noret which gets
   * wiped on every PDS wake. */
  .bss_ocram_high (NOLOAD) : ALIGN(8)
  {
    PROVIDE( _bss_ocram_high_start = . );
    *(.ocram_bss_high)
    *(.ocram_bss_high.*)
    *libopenthread-mtd.a:(.bss .bss.* .sbss .sbss.*)
    *libopenthread_port.a:(.bss .bss.* .sbss .sbss.*)
    . = ALIGN(8);
    PROVIDE( _bss_ocram_high_end = . );
  } >ocram_high
")
        else()
            set(OCRAM_HIGH_MEMORY_REGION "  /* ocram_high disabled */")
            set(OCRAM_HIGH_BSS_SECTION "")
        endif()
        set(OCRAM_DATA_END_MARKER "")
    endif()

    # Handle FPGA RAM as Heap configuration (IPRO6 specific)
    # When CONFIG_FPGA_RAM_AS_HEAP is enabled, the flash region (which is actually RAM
    # in FPGA environment) can provide additional heap space after code/data sections.
    if(DEFINED CONFIG_FPGA_RAM_AS_HEAP AND CONFIG_FPGA_RAM_AS_HEAP)
        set(FPGA_HEAP_SECTION "
  /* FPGA RAM Heap - remaining flash region after code/data */
  .fpga_heap (NOLOAD) : ALIGN(8)
  {
    PROVIDE( __heap_flash_start = . );
  } > flash

  PROVIDE( __heap_flash_size = ORIGIN(flash) + LENGTH(flash) - __heap_flash_start );
  PROVIDE( __heap_flash_end = __heap_flash_start + __heap_flash_size );
")
    else()
        set(FPGA_HEAP_SECTION "  /* FPGA RAM as Heap disabled */")
    endif()

    # Handle ocram_noret configuration — non-retention OCRAM region
    # Enabled when: (1) AI=y, MM=y, EM_SIZE>0, or (2) HAL_LP=y without PSRAM
    if(ENABLE_OCRAM_NORET)
        set(OCRAM_NORET_MEMORY_REGION "  ocram_noret (wxa!r) : ORIGIN = ${OCRAM_NORET_ORIGIN}, LENGTH = ${OCRAM_NORET_SIZE}")
        set(OCRAM_NORET_SECTION "
  /* OCRAM non-retention section - NOT preserved during PDS sleep */
  /* Use ATTR_OCRAM_NORET for buffers that don't need LP retention */
  .ocram_noret (NOLOAD) : ALIGN(8)
  {
    PROVIDE( __ocram_noret_start = . );
    *(.ocram_noret)
    *(.ocram_noret.*)
    *(.ocram_noret_noinit)
    *(.ocram_noret_noinit.*)
    . = ALIGN(8);
    PROVIDE( __ocram_noret_end = . );
  } >ocram_noret

  /* Heap in remaining ocram_noret space (non-retention, lost during PDS).
     DIRECT assignment, NOT PROVIDE: freertos_app_hooks.c references these via
     WEAK externs, and a weak undefined reference does NOT trigger a PROVIDE in
     GNU ld — so PROVIDE leaves the symbols at 0, the `__heap_noret_size > 0`
     guard fails, and the entire noret heap region is silently dropped (only
     the 16 KB retention region survives → large mallocs OOM at boot). Defining
     them unconditionally makes the weak externs resolve to the real values. */
  __heap_noret_start = __ocram_noret_end ;
  __heap_noret_size = ORIGIN(ocram_noret) + LENGTH(ocram_noret) - __heap_noret_start ;
  /* End as a real ADDRESS (not an ABS size value): freertos_app_hooks.c reads
   * the heap size as (end - start) from address symbols, because reading the
   * ABS *size* symbols (__heap_*_size) gets GP-relative-relaxed by ld and
   * truncates (R_RISCV_GPREL_I against an *ABS* symbol). */
  __heap_noret_end = ORIGIN(ocram_noret) + LENGTH(ocram_noret) ;
")
    else()
        set(OCRAM_NORET_MEMORY_REGION "  /* ocram_noret disabled */")
        set(OCRAM_NORET_SECTION "  /* ocram_noret section disabled */")
    endif()

    # Handle em_tcm_ret configuration
    # When ENABLE_EM_TCM_RET=ON (AI_SUPPORT=y, MM_SUPPORT=y, EM_SIZE=0):
    #   - em_tcm_ret region at 0x1102C000, 16KB
    #   - Split tcmcode: LP+Flash stay in ocram, others move to em_tcm_ret
    # When ENABLE_EM_TCM_RET=OFF:
    #   - All tcmcode stays in ocram (original behavior)
    if(ENABLE_EM_TCM_RET)
        set(EM_TCM_RET_MEMORY_REGION "  em_tcm_ret (wxa!r) : ORIGIN = ${EM_TCM_RET_ORIGIN}, LENGTH = ${EM_TCM_RET_SIZE}")

        # tcmcode section: only essential LP wakeup code in fast OCRAM
        # Keep minimal: hal_lp.c.o (LP framework) and hal_lp_entry.S.o (entry point)
        # Use RAM which is ocram (when PSRAM disabled) or ocram explicit (when PSRAM enabled)
        if(DEFINED CONFIG_USE_PSRAM AND CONFIG_USE_PSRAM)
            set(TCMCODE_SECTION "
  .tcmcode : ALIGN(8)
  {
    /* Essential LP wakeup code - must stay in fast OCRAM */
    *hal_lp.c.o(.tcm_code.*)
    *hal_lp.c.o(.tcm_const.*)
    *hal_lp_entry.S.o(.tcm_code.*)
    . = ALIGN(8);
  } >ocram AT>ROM

  /* Writable TCM data, kept in its own section so startup can copy it with
   * its own symbols and so the executable and writable parts of TCM are not
   * one blob.
   *
   * The section start was page-aligned at one point, to stop ld merging
   * .tcmcode and .tcmdata into a single RWE LOAD segment. That spent up to
   * 4 KB of OCRAM positioning a few hundred bytes and pushed ipro7_demo past
   * the end of its region - its AI+MM configuration gets only the 64 KB of
   * banks 0-3. A merged RWE segment has no runtime meaning here: the image is
   * objcopy'd to a raw binary, with no loader or MMU to enforce W^X. */
  .tcmdata : ALIGN(8)
  {
    /* TCM data - DMA descriptors, buffers requiring fast OCRAM access */
    *(.tcm_data*)
    . = ALIGN(8);
  } >ocram AT>ROM

  /* Track OCRAM position after tcm code/data (for PSRAM mode) */
  __ocram_tcmcode_end = .;
")
        else()
            set(TCMCODE_SECTION "
  .tcmcode : ALIGN(8)
  {
    /* Essential LP wakeup code - must stay in fast OCRAM */
    *hal_lp.c.o(.tcm_code.*)
    *hal_lp.c.o(.tcm_const.*)
    *hal_lp_entry.S.o(.tcm_code.*)
    . = ALIGN(8);
  } >RAM AT>ROM

  /* Writable TCM data, kept in its own section so startup can copy it with
   * its own symbols and so the executable and writable parts of TCM are not
   * one blob.
   *
   * The section start was page-aligned at one point, to stop ld merging
   * .tcmcode and .tcmdata into a single RWE LOAD segment. That spent up to
   * 4 KB of OCRAM positioning a few hundred bytes and pushed ipro7_demo past
   * the end of its region - its AI+MM configuration gets only the 64 KB of
   * banks 0-3. A merged RWE segment has no runtime meaning here: the image is
   * objcopy'd to a raw binary, with no loader or MMU to enforce W^X. */
  .tcmdata : ALIGN(8)
  {
    /* TCM data - DMA descriptors, buffers requiring fast OCRAM access */
    *(.tcm_data*)
    . = ALIGN(8);
  } >RAM AT>ROM
")
        endif()

        # em_tcm_ret section: all other TCM code moved here to maximize OCRAM savings
        set(EM_TCM_RET_SECTION "
  /* em_tcm_ret section - TCM code moved here to save OCRAM */
  .em_tcm_ret : ALIGN(8)
  {
    PROVIDE( __em_tcm_ret_start = . );
    /* LP main, Flash, UART, clock code - less critical for wakeup latency */
    *hal_lpfw_main.c.o(.tcm_code.*)
    *hal_lpfw_main.c.o(.tcm_const.*)
    *hal_flash.c.o(.tcm_code.*)
    *hal_flash.c.o(.tcm_const.*)
    *lp_uart.c.o(.tcm_code.*)
    *lp_uart.c.o(.tcm_const.*)
    *(.sclock_rlt_code.*)
    *(.sclock_rlt_const.*)
    /* All other tcm code */
    *(EXCLUDE_FILE(*hal_lp.c.o *hal_lp_entry.S.o) .tcm_code.*)
    *(EXCLUDE_FILE(*hal_lp.c.o *hal_lp_entry.S.o) .tcm_const.*)
    . = ALIGN(8);
    PROVIDE( __em_tcm_ret_end = . );
  } >em_tcm_ret AT>ROM
")

        set(EM_TCM_RET_PROVIDES "
  PROVIDE( _em_tcm_ret_lma = LOADADDR(.em_tcm_ret) );
  PROVIDE( _em_tcm_ret = ADDR(.em_tcm_ret) );
  PROVIDE( _em_tcm_ret_size = __em_tcm_ret_end - __em_tcm_ret_start );
")
    else()
        set(EM_TCM_RET_MEMORY_REGION "  /* em_tcm_ret disabled */")

        # Original tcmcode section: all tcm code.
        # `.tcm_code` contains the LP framework entry/restore routines
        # (hal_lp_enter, hal_lp_restore_cpu_regs, hal_lpfw_main, lp_uart,
        # …) — code that MUST survive PDS31 wake because the chip
        # resumes execution into it. When CONFIG_OCRAM_NORET_AS_RAM=y
        # the default RAM alias points at ocram_noret (wiped on PDS),
        # so we pin tcmcode to `ocram` (the retention region) instead.
        if(DEFINED CONFIG_USE_PSRAM AND CONFIG_USE_PSRAM)
            set(_TCMCODE_REGION "ocram")
        elseif(ENABLE_OCRAM_NORET)
            set(_TCMCODE_REGION "ocram")
        else()
            set(_TCMCODE_REGION "RAM")
        endif()
        set(TCMCODE_SECTION "
  .tcmcode : ALIGN(8)
  {
    *(.tcm_code.*)
    *(.tcm_const.*)
    *(.sclock_rlt_code.*)
    *(.sclock_rlt_const.*)
    . = ALIGN(8);
  } >${_TCMCODE_REGION} AT>ROM

  /* Writable TCM data, kept in its own section so startup can copy it with
   * its own symbols and so the executable and writable parts of TCM are not
   * one blob.
   *
   * The section start was page-aligned at one point, to stop ld merging
   * .tcmcode and .tcmdata into a single RWE LOAD segment. That spent up to
   * 4 KB of OCRAM positioning a few hundred bytes and pushed ipro7_demo past
   * the end of its region - its AI+MM configuration gets only the 64 KB of
   * banks 0-3. A merged RWE segment has no runtime meaning here: the image is
   * objcopy'd to a raw binary, with no loader or MMU to enforce W^X. */
  .tcmdata : ALIGN(8)
  {
    *(.tcm_data*)
    . = ALIGN(8);
  } >${_TCMCODE_REGION} AT>ROM
")
        if(_TCMCODE_REGION STREQUAL "ocram")
            set(TCMCODE_SECTION "${TCMCODE_SECTION}
  /* Track OCRAM position after tcm code/data (for retention layout) */
  __ocram_tcmcode_end = .;
")
        endif()

        set(EM_TCM_RET_SECTION "")
        # Provide dummy symbols for startup code (LMA=VMA=0 skips copy)
        set(EM_TCM_RET_PROVIDES "
  PROVIDE( _em_tcm_ret_lma = 0 );
  PROVIDE( _em_tcm_ret = 0 );
  PROVIDE( __em_tcm_ret_end = 0 );
")
    endif()

    # Handle NOINIT_DATA, NOCACHE, and STACK sections
    # Different behavior based on whether PSRAM is enabled
    if(DEFINED CONFIG_USE_PSRAM AND CONFIG_USE_PSRAM)
        # PSRAM enabled: OCRAM for fast sections, PSRAM for default data/bss
        set(NOINIT_DATA_SECTION "
  /* No-init data section - not cleared during startup */
  /* Used for data that must survive BSS clear (e.g., boot2 parameters) */
  /* Always in OCRAM for fast access and persistence */
  .noinit_data __ocram_data_end (NOLOAD) : ALIGN(8)
  {
    __noinit_data_start = .;
    *(.noinit_data)
    *(.noinit_data.*)
    . = ALIGN(8);
    __noinit_data_end = .;
  } >ocram
")
        set(NOCACHE_SECTIONS "
  /* Record end of cacheable OCRAM region for nocache section placement */
  __ocram_cacheable_end = __noinit_data_end;

  /* Non-cacheable RAM section (initialized data) */
  /* VMA = OCRAM position + offset, always in OCRAM for DMA */
  .nocache_ram __ocram_cacheable_end + __NOCACHE_ADDR_OFFSET (NOLOAD) : ALIGN(8)
  {
    __nocache_ram_start = .;
    *(.nocache_ram)
    *(.nocache_ram.*)
    *(.noncacheable)
    *(.noncacheable.*)
    . = ALIGN(8);
    __nocache_ram_end = .;
  }

  /* Non-cacheable RAM section (uninitialized, no need to clear) */
  /* Continue from previous nocache section (already in non-cacheable space) */
  .nocache_noinit_ram __nocache_ram_end (NOLOAD) : ALIGN(8)
  {
    __nocache_noinit_start = .;
    *(.nocache_noinit_ram)
    *(.nocache_noinit_ram.*)
    . = ALIGN(8);
    __nocache_noinit_end = .;
  }

  /* Calculate total nocache size (in cacheable address space units) */
  __nocache_total_size = (__nocache_noinit_end - __NOCACHE_ADDR_OFFSET) - __ocram_cacheable_end;

  /* Calculate where stack should start (after nocache region in physical OCRAM).
   * Align the symbol itself, not only the .stack section body: otherwise ld moves
   * the section start forward and reports '.stack changed by <n>' while
   * __stack_start_addr still names the pre-aligned address. */
  __stack_start_addr = ALIGN(__ocram_cacheable_end + __nocache_total_size, 16);
")
        set(STACK_SECTION "
  /* Stack section - always in OCRAM for fast access */
  /* Address calculated after nocache region in OCRAM */
  .stack __stack_start_addr (NOLOAD) :
  {
    . = ALIGN(16);
    PROVIDE( __StackLimit = . );
    PROVIDE( __StackBottom = . );
    . += __TOT_STACK_SIZE;
    . = ALIGN(16);
    PROVIDE( __StackTop = . );
    PROVIDE( _sp = . );
  } >ocram
")
    else()
        # PSRAM disabled: original behavior - all in RAM (OCRAM)
        set(NOINIT_DATA_SECTION "
  /* No-init data section - not cleared during startup */
  /* Used for data that must survive BSS clear (e.g., boot2 parameters) */
  .noinit_data (NOLOAD) : ALIGN(8)
  {
    __noinit_data_start = .;
    *(.noinit_data)
    *(.noinit_data.*)
    . = ALIGN(8);
    __noinit_data_end = .;
  } >RAM
")
        set(NOCACHE_SECTIONS "
  /* Record end of cacheable region for nocache section placement */
  __ocram_cacheable_end = .;

  /* Non-cacheable RAM section (initialized data) */
  /* VMA = current position + offset */
  .nocache_ram . + __NOCACHE_ADDR_OFFSET (NOLOAD) : ALIGN(8)
  {
    __nocache_ram_start = .;
    *(.nocache_ram)
    *(.nocache_ram.*)
    *(.noncacheable)
    *(.noncacheable.*)
    . = ALIGN(8);
    __nocache_ram_end = .;
  }

  /* Non-cacheable RAM section (uninitialized, no need to clear) */
  /* Continue from previous nocache section (already in non-cacheable space) */
  .nocache_noinit_ram __nocache_ram_end (NOLOAD) : ALIGN(8)
  {
    __nocache_noinit_start = .;
    *(.nocache_noinit_ram)
    *(.nocache_noinit_ram.*)
    . = ALIGN(8);
    __nocache_noinit_end = .;
  }

  /* Calculate total nocache size (in cacheable address space units) */
  __nocache_total_size = (__nocache_noinit_end - __NOCACHE_ADDR_OFFSET) - __ocram_cacheable_end;

  /* Where the stack must start to clear the nocache region. The .nocache_* sections
   * above carry a VMA in the +__NOCACHE_ADDR_OFFSET alias, which is OUTSIDE the RAM
   * MEMORY region, so ld never advances the RAM region counter for them -- a following
   * '.stack ... >RAM' with no explicit address lands right back on __ocram_cacheable_end
   * and the stack then shares physical OCRAM with every DMA buffer in .nocache_ram.
   * With no nocache users __nocache_total_size is 0 and this is the old address exactly. */
  __stack_start_addr = ALIGN(__ocram_cacheable_end + __nocache_total_size, 16);

")
        set(STACK_SECTION "
  /* Stack section - explicitly past the nocache region (see __stack_start_addr).
   * It must NOT be left to the >RAM region counter: the nocache sections have an
   * aliased VMA outside the RAM region, so that counter does not account for them. */
  .stack __stack_start_addr (NOLOAD) : ALIGN(16)
  {
    PROVIDE( __StackLimit = . );
    PROVIDE( __StackBottom = . );
    . += __TOT_STACK_SIZE;
    . = ALIGN(16);
    PROVIDE( __StackTop = . );
    PROVIDE( _sp = . );
  } >RAM
")
    endif()

    # Handle NVDS configuration
    # Support Kconfig-style configuration:
    # - CONFIG_ENABLE_NVDS=y          → Enable NVDS
    # - CONFIG_ENABLE_NVDS=n          → Disable NVDS
    # - # CONFIG_ENABLE_NVDS is not set → Disable NVDS (Kconfig comment style)
    # - Not defined at all            → Use config file default

    if(DEFINED CONFIG_ENABLE_NVDS)
        # User explicitly set NVDS preference in proj.conf
        # CONFIG_ENABLE_NVDS can be: 1 (enabled), 0 (disabled), "y", "n"
        if(CONFIG_ENABLE_NVDS)
            # If evaluates to true (1, "y", non-zero, non-empty) → enable
            set(ENABLE_NVDS ON)
        else()
            # If evaluates to false (0, "n", "", empty) → disable
            set(ENABLE_NVDS OFF)
        endif()
    endif()
    # If CONFIG_ENABLE_NVDS not defined, ENABLE_NVDS keeps the value from config file
    # If ENABLE_NVDS is still ON, generate NVDS section
    if(ENABLE_NVDS)
        if(DEFINED CONFIG_NVDS_SIZE AND NOT "${CONFIG_NVDS_SIZE}" STREQUAL "")
            set(NVDS_SIZE "${CONFIG_NVDS_SIZE}")
            string(REGEX REPLACE "^\"(.*)\"$" "\\1" NVDS_SIZE "${NVDS_SIZE}")
        endif()
        if(NOT DEFINED NVDS_SIZE)
            # Default 1 KB.  The RW BLE controller only stores a handful of
            # small tags (BD_ADDRESS, LPCLK_DRIFT, ACTCLK_DRIFT, SCA,
            # OSC/RM_WAKEUP_TIME, SLEEP_ENABLE …), so 1 KB is sufficient for
            # the generated default set.  Override via CONFIG_NVDS_SIZE if a
            # board/app genuinely needs more.
            set(NVDS_SIZE "1K")
        endif()
        
        # Determine NVDS memory region
        if(CONFIG_NVDS_IN_PSRAM AND CONFIG_USE_PSRAM)
            set(NVDS_REGION "psram")
            set(NVDS_COMMENT "PSRAM (heap starts after MAX(_bss_psram_end, __nvds_end))")
        else()
            set(NVDS_REGION "ocram")
            set(NVDS_COMMENT "OCRAM (default)")
        endif()
        
        set(NVDS_SECTION "
  /* NVDS section - ${NVDS_SIZE} reserved from ${NVDS_COMMENT} */
  .nvds (NOLOAD) : ALIGN(4K)
  {
    PROVIDE( __nvds_start = . );
    . += ${NVDS_SIZE};
    PROVIDE( __nvds_end = . );
    PROVIDE( __nvds_size = __nvds_end - __nvds_start );
  } >${NVDS_REGION}
")
        set(NVDS_PROVIDES "
  /* NVDS information */
  PROVIDE( _ld_nvds_addr = __nvds_start );
  PROVIDE( _ld_nvds_size = __nvds_size );
")
    else()
        set(NVDS_SECTION "
  /* NVDS section disabled - CONFIG_ENABLE_NVDS not set */
  PROVIDE( __nvds_start = 0 );
  PROVIDE( __nvds_end = 0 );
  PROVIDE( __nvds_size = 0 );
")
        set(NVDS_PROVIDES "
  /* NVDS information */
  PROVIDE( _ld_nvds_addr = __nvds_start );
  PROVIDE( _ld_nvds_size = __nvds_size );
")
    endif()

    # Generate linker script using configure_file
    configure_file(
        ${TEMPLATE_FILE}
        ${OUTPUT_FILE}
        @ONLY
    )

    message(STATUS "Generated linker script: ${OUTPUT_FILE}")
    message(STATUS "  - Board: ${GEN_BOARD}")
    message(STATUS "  - Build Type: ${GEN_BUILD_TYPE}")
    message(STATUS "  - Config: ${CONFIG_TYPE} (${CHIP_NAME})")
    message(STATUS "  - Template: ${LINKER_TEMPLATE}")
    message(STATUS "  - Flash Origin: ${FLASH_ORIGIN}")
    message(STATUS "  - RAM Region: ${RAM_REGION}")
    message(STATUS "  - NVDS Enabled: ${ENABLE_NVDS}")

    # Export the output file path to parent scope
    set(GENERATED_LINKER_SCRIPT ${OUTPUT_FILE} PARENT_SCOPE)

    # Export ENABLE_NVDS status to parent scope for compile definitions
    set(LINKER_NVDS_ENABLED ${ENABLE_NVDS} PARENT_SCOPE)

endfunction()

# Convenience function to auto-detect and generate linker script
# based on Kconfig or CMake variables
function(auto_generate_linker_script)
    # Try to determine board from various sources
    if(DEFINED CONFIG_BOARD)
        set(BOARD_NAME ${CONFIG_BOARD})
    elseif(DEFINED BOARD)
        set(BOARD_NAME ${BOARD})
    else()
        message(WARNING "Board not specified, defaulting to ipro7_evb")
        set(BOARD_NAME "ipro7_evb")
    endif()

    # Try to determine build type
    if(DEFINED CONFIG_BOOT_FROM_RAM AND CONFIG_BOOT_FROM_RAM)
        set(BUILD_TYPE "ram")
    elseif(DEFINED BOOT_FROM_RAM AND BOOT_FROM_RAM)
        set(BUILD_TYPE "ram")
    else()
        set(BUILD_TYPE "flash")
    endif()

    # Generate the linker script
    generate_linker_script(
        BOARD ${BOARD_NAME}
        BUILD_TYPE ${BUILD_TYPE}
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/linker
    )

    # Export to parent scope
    set(GENERATED_LINKER_SCRIPT ${GENERATED_LINKER_SCRIPT} PARENT_SCOPE)

endfunction()
