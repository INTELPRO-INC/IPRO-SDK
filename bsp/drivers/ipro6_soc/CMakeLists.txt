# IPRO6 SoC Driver
#
# IPRO6 is a hybrid chip:
# - CPU: Same as IPRO7 (RISC-V RV32IMAFC with custom extensions)
# - SoC/Peripherals: Same as IPRO6LE (drivers, registers, memory map)
# - No PSRAM support

__generate_library()

# IPRO7 CPU architecture flags
# Use detected arch settings from toolchain.cmake
SET(MARCH "${IPRO_MARCH}")
SET(MABI "${IPRO_MABI}")
SET(MARCH_EXT "${IPRO_MARCH_EXT_DSP}")

# Include directories (IPRO6LE's SoC registers + IPRO7's CPU core)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/inc)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/rfparam/inc)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/regs)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/startup)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/risc-v/Core/Include)
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/DSP/include)
__component_add_private_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/DSP/privateinclude)

# Startup and ROM driver table
if(CONFIG_BOOTROM)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_romdriver.c)
else()
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/startup/GCC/intexc_ipro6.S)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/startup/GCC/startup_ipro6.S)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/startup/start_load.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/startup/system_soc.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/startup/boot2_param.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/startup/interrupt.c)
endif()
# IPRO6 does not have PSRAM

# IPRO6LE Standard Drivers (IPRO6LE's SoC peripherals)
# When CONFIG_USE_ROM_DRIVERS is enabled, use ROM driver wrappers instead of full implementations
if(CONFIG_USE_ROM_DRIVERS)
    # ROM driver wrappers (weak) supply every SoC-peripheral API from ROM.
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_romapi.c)
    # Per-API strong overrides for the functions whose ROM copy is buggy. The
    # wrappers above are weak, so each strong def here wins; the anchor force-
    # pulls this TU (a weak-only archive member is not auto-extracted).
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_romapi_patch.c)
    __component_add_link_libraries(-u__drv_romapi_patch_anchor)

    # Only genuinely-non-ROM helpers stay as SDK sources (drv_is_cache_addr,
    # drv_get_no_cache_addr, check_failed).
    #
    # The whole-file SDK fallbacks that used to sit here (glb, glb_gpio, aon,
    # hbn, ef_ctrl, sflash, xip_sflash, pds, sf_ctrl, sf_cfg) are covered
    # per-API by drv_romapi_patch.c. Linking the whole files only pulled dead
    # code (all gc-stripped to 0 bytes) and, now that the patch overrides are
    # strong, would risk a multiple-definition clash if ever extracted. A ROM
    # target links ROM wrappers + the specific override APIs, nothing else.
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_common.c)
else()
    # Full SDK driver implementations
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_clock.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_aon.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_ef_ctrl.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_glb.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_glb_gpio.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_hbn.c)
    # drv_l1c.c removed - L1C_* functions are now macros in drv_l1c.h
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_sflash.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_sf_cfg.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_sf_ctrl.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_uart.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_xip_sflash.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_common.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_pds.c)
endif()

# Drivers always needed (not in ROM or have additional functionality)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_common_s.c)
# SMID (PE-SMID SD3.0/SDIO3.0 device controller) driver
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/ipro6_sdio3.c)
# drv_auadc.c removed - AUADC not available on IPRO6
# drv_aupwm.c removed - AUPWM not available on IPRO6
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_cam.c)
# drv_iso11898.c removed - ISO11898 not available on IPRO6
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_dbi.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_dbi_qspi.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_dma.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_ef_cfg.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_emac.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_i2c.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_i2s.c)
# drv_ir.c removed - IR not available on IPRO6
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_pwm.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_spi.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_sec_dbg.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_sec_eng.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_tzc_sec.c)
__component_add_sources_ifdef(CONFIG_COMPONENTS_CHERRYUSB_ENABLE ${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_usb.c)
__component_add_sources_ifdef(CONFIG_COMPONENTS_CHERRYUSB_ENABLE ${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_usb_cherryusb.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_adc.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_dac.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_acomp.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_timer.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_mfg_media.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_mfg_efuse.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_mfg_flash.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_mjdec.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_dvp2axi.c)
__component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/drv_mm_misc.c)

# RF Parameter (IPRO6LE compatible) - only needed for WiFi/BLE
if(CONFIG_WIFI6 OR CONFIG_BLUETOOTH_LE_ENABLE)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/rfparam/src/rfparam.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/rfparam/src/rfparam_adapter.c)
    __component_add_sources(${CMAKE_CURRENT_SOURCE_DIR}/rfparam/src/rfparam_rftlv.c)
    __component_add_compile_definitions(-DRFPARAM_IPRO6LE)
endif()

# DSP library (same as IPRO7 - shared CPU architecture)
__component_add_static_library(${CMAKE_CURRENT_SOURCE_DIR}/DSP/lib/GCC/libnmsis_dsp_${IPRO_MARCH}${IPRO_MARCH_EXT_DSP_LIB}.a)

# IPRO7 CPU compile/link options
__component_add_compile_options(-march=${MARCH}${MARCH_EXT} -mabi=${MABI})
__component_add_link_options(-march=${MARCH}${MARCH_EXT} -mabi=${MABI})
if(NOT "${IPRO_MTUNE_FLAG}" STREQUAL "")
    __component_add_compile_options(${IPRO_MTUNE_FLAG})
    __component_add_link_options(${IPRO_MTUNE_FLAG})
endif()

# Suppress warnings for IPRO6LE driver compatibility
__component_add_private_compile_options(-Wno-error=pointer-to-int-cast -Wno-error=int-to-pointer-cast)
__component_add_private_compile_options(-Wno-error=unused-function -Wno-error=int-conversion)
__component_add_private_compile_options(-Wno-error=incompatible-pointer-types -Wno-error=format=)

# Chip identification
__component_add_compile_definitions(-DIPRO_USE_HAL_DRIVER -DARCH_RISCV)
__component_add_compile_definitions(-DCONFIG_IRQ_NUM=80)
__component_add_compile_definitions(-DIPRO6)

# WL_API_RMEM_EN=1: the WiFi/RF driver stores its cal (wl_rmem_t) at the caller-
# supplied RMEM address (WL_API_RMEM_ADDR = HAL_HBNRAM_WLCAL_ADDR, 0x20090B00,
# retained HBN RAM) instead of a plain .bss global. This is required for the lp_fw
# warm-restore: the full firmware's RF cal must survive PDS in WLCAL so the wake
# path can restore it (wl_lp_init reads from that address). Mirrors ipro6le_soc.
__component_add_compile_definitions(-DWL_API_RMEM_EN=1)

# IREGION base address (hardcoded to avoid runtime CSR read)
__component_add_compile_definitions(-DCFG_IREGION_BASE_ADDR=0x400000)

# Force romDriverTable symbol to be linked (only needed for bootrom builds)
if(CONFIG_BOOTROM)
    __component_add_link_libraries(-uromDriverTable)
endif()
