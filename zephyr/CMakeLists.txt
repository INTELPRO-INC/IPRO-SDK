# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
#
# Zephyr module entry point for the IPRO SDK.
#
#   ipro_hal        the SDK HAL compiled in place from bsp/ (no copies)
#   drivers/        Zephyr driver adapters on top of that HAL
#
# The HAL is only built when a driver or the SoC selects CONFIG_IPRO_HAL, so
# a Zephyr application that does not target an IPRO SoC pays nothing.

set(IPRO_SDK_ROOT ${ZEPHYR_IPRO_SDK_MODULE_DIR})
set(IPRO_BSP      ${IPRO_SDK_ROOT}/bsp)
if(CONFIG_SOC_SERIES_IPRO6S)
  # IPRO6S is the IPRO6 die: bsp/drivers/ipro6_soc, CONFIG_IPRO6 in the SDK.
  set(IPRO_SOC_DRV  ${IPRO_BSP}/drivers/ipro6_soc)
  set(IPRO_CHIP     IPRO6)
  set(IPRO_CHIP_LC  ipro6)
else()
  set(IPRO_SOC_DRV  ${IPRO_BSP}/drivers/ipro7_soc)
  set(IPRO_CHIP     IPRO7)
  set(IPRO_CHIP_LC  ipro7)
endif()

# Warnings the SDK sources trip under Zephyr's stricter -Wall/-Wextra set
# (twister builds with warnings-as-errors). Applied to every SDK-owned
# library; the Zephyr-side drivers under drivers/ compile clean without it.
# -fno-jump-tables is mandatory, not a warning knob: ATTR_TCM_SECTION flash
# routines (SFlash_Read, SFlash_Set_IDbus_Cfg, ...) run from OCRAM with XIP
# paused, and a switch() jump table would sit in .rodata in flash - the SDK
# sets the same flag in cmake/compiler_flags.cmake. Observed as a silent hang
# in the first flash_read() on the EVK before this was added.
#
# -fmacro-prefix-map keeps the build machine's path out of __FILE__ (asserts,
# logs), which tools/release ships inside the prebuilt core archives; the
# debug info, stripped from those archives, keeps full paths for debugging.
set(IPRO_SDK_WARNING_OPTS
  -fmacro-prefix-map=${IPRO_SDK_ROOT}/=
  -fno-jump-tables
  -Wno-format
  -Wno-format-security
  -Wno-pointer-arith
  -Wno-unused-but-set-variable
  -Wno-unused-variable
  -Wno-unused-function
  -Wno-pointer-sign
  -Wno-sign-compare
  -Wno-implicit-fallthrough
  -Wno-missing-field-initializers
  -Wno-double-promotion
)

if(CONFIG_IPRO_HAL)

  zephyr_library_named(ipro_hal)

  # Public include paths: every driver adapter (and applications that want
  # the SDK HAL directly) sees the same headers the FreeRTOS SDK uses.
  # The shim directory MUST come first: it provides board_config.h and
  # generated/autoconf.h in the form the HAL expects under Zephyr.
  zephyr_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include/ipro_hal_shim
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${IPRO_BSP}/hal/inc
    ${IPRO_BSP}/hal/default_config
    # soc_common/inc must precede std_drv/inc: both carry a drv_psram.h and
    # the SDK build resolves the soc_common (X8 PSRAM controller) one.
    ${IPRO_BSP}/drivers/soc_common/inc
    ${IPRO_SOC_DRV}
    ${IPRO_SOC_DRV}/std_drv/inc
    ${IPRO_SOC_DRV}/regs
    ${IPRO_SOC_DRV}/startup
    ${IPRO_SOC_DRV}/risc-v/Core/Include
    ${IPRO_BSP}/common/platform_utils/include
    ${IPRO_SDK_ROOT}/components/utils/include
    ${IPRO_SDK_ROOT}/components/softcrc
  )

  # Chip selector the LL layer tests as a bare macro (bsp CMake adds
  # -D${CONFIG_CHIP}); AON WDT API is present on the IPRO7 std_drv.
  zephyr_compile_definitions(${IPRO_CHIP})
  if(CONFIG_SOC_SERIES_IPRO6S)
    # What bsp/drivers/ipro6_soc/CMakeLists.txt adds for every IPRO6 build:
    # HAL-driver mode (no LL-internal ISR registration), the ECLIC line
    # count and the fixed IREGION base the SDK uses on this die.
    zephyr_compile_definitions(IPRO_USE_HAL_DRIVER ARCH_RISCV CONFIG_IRQ_NUM=80
                               CFG_IREGION_BASE_ADDR=0x400000)
    # CFG_HAS_ICACHE/DCACHE/IOCC come from ipro6.h itself (it derives
    # __ICACHE_PRESENT/__DCACHE_PRESENT/__CCM_PRESENT from them); passing
    # them on the command line as well only produces -Wredefined errors.
  endif()
  if(CONFIG_AON_WDT_SUPPORT)
    zephyr_compile_definitions(CONFIG_AON_WDT_SUPPORT=1)
  endif()
  if(CONFIG_IPRO_FLASH_CFG_FROM_BOOTROM)
    zephyr_compile_definitions(CONFIG_FLASH_CFG_FROM_BOOTROM=1)
  endif()

  # ---- Register / LL layer (bsp/drivers/ipro7_soc/std_drv) ---------------
  set(IPRO_LL ${IPRO_SOC_DRV}/std_drv/src)
  zephyr_library_sources(
    ${IPRO_LL}/drv_common.c
    ${IPRO_LL}/drv_ef_cfg.c
    ${IPRO_LL}/drv_timer.c
    ${IPRO_LL}/drv_sec_eng.c
    ${IPRO_LL}/drv_sec_dbg.c
  )
  if(CONFIG_SOC_SERIES_IPRO6S)
    # IPRO6 production configuration (CONFIG_USE_ROM_DRIVERS in the SDK):
    # GLB/GPIO/AON/HBN/PDS/EF_CTRL/SF_CTRL/SF_CFG/SFLASH/XIP_SFLASH come from
    # the boot ROM through the weak wrappers in drv_romapi.c, with the
    # per-API strong overrides of drv_romapi_patch.c (force-pulled by the
    # anchor). The ROM copies are XIP-safe; the source copies call
    # flash-resident helpers (Utils_Soft_CRC32) with XIP paused and hang.
    # (drv_uart.c / drv_clock.c are ROM + patch as well.)
    zephyr_library_sources(
      ${IPRO_LL}/drv_romapi.c
      ${IPRO_LL}/drv_romapi_patch.c
      ${IPRO_LL}/drv_tzc_sec.c
    )
    zephyr_link_libraries(-Wl,-u,__drv_romapi_patch_anchor)
  else()
    zephyr_library_sources(
      ${IPRO_LL}/drv_clock.c
      ${IPRO_LL}/drv_uart.c
      ${IPRO_LL}/drv_glb.c
      ${IPRO_LL}/drv_hbn.c
      ${IPRO_LL}/drv_aon.c
      ${IPRO_LL}/drv_pds.c
      ${IPRO_LL}/drv_ef_ctrl.c
      ${IPRO_LL}/drv_sf_ctrl.c
      ${IPRO_LL}/drv_sf_cfg.c
      ${IPRO_LL}/drv_sflash.c
      ${IPRO_LL}/drv_xip_sflash.c
      ${IPRO_LL}/drv_gpio.c
      ${IPRO_LL}/drv_sf_cfg_ext.c
      ${IPRO_LL}/drv_sflash_ext.c
      ${IPRO_LL}/drv_xip_sflash_ext.c
      ${IPRO_LL}/drv_mcu_misc.c
      ${IPRO_LL}/drv_tzc.c
    )
  endif()
  zephyr_library_sources_ifdef(CONFIG_SPI_IPRO      ${IPRO_LL}/drv_spi.c)
  zephyr_library_sources_ifdef(CONFIG_I2C_IPRO      ${IPRO_LL}/drv_i2c.c)
  zephyr_library_sources_ifdef(CONFIG_PWM_IPRO      ${IPRO_LL}/drv_pwm.c)
  zephyr_library_sources_ifdef(CONFIG_DMA_IPRO      ${IPRO_LL}/drv_dma.c)
  zephyr_library_sources_ifdef(CONFIG_ADC_IPRO      ${IPRO_LL}/drv_adc.c)
  zephyr_library_sources_ifdef(CONFIG_DAC_IPRO      ${IPRO_LL}/drv_dac.c)
  zephyr_library_sources_ifdef(CONFIG_I2S_IPRO      ${IPRO_LL}/drv_i2s.c)
  zephyr_library_sources_ifdef(CONFIG_AUDIO_DMIC_IPRO ${IPRO_LL}/drv_auadc_v1.c)
  zephyr_library_sources_ifdef(CONFIG_IPRO_HAL_USE_PSRAM ${IPRO_BSP}/drivers/soc_common/src/drv_psram.c)

  # ---- Startup helpers reused verbatim ------------------------------------
  # boot2_param.c : boot2 -> application hand-off (partition table, flash cfg)
  # clock_boost.c : XIP-resident PLL bring-up used from soc_prep_hook()
  # NOT compiled: startup_ipro7.S / system_soc.c / interrupt.c / section_copy.c
  # (Zephyr owns reset, mtvec, the ISR table and the data copy).
  zephyr_library_sources(${IPRO_SOC_DRV}/startup/boot2_param.c)
  zephyr_library_sources_ifdef(CONFIG_SOC_SERIES_IPRO7 ${IPRO_SOC_DRV}/startup/clock_boost.c)
  # psram_init.c owns __psram_available; the training itself is
  # psram_early_hw_init.c, called from soc_prep_hook().
  zephyr_library_sources_ifdef(CONFIG_IPRO_HAL_USE_PSRAM
    ${IPRO_SOC_DRV}/startup/psram_init.c
    ${IPRO_SOC_DRV}/startup/psram_early_hw_init.c
  )
  if(CONFIG_IPRO_HAL_USE_PSRAM)
    zephyr_library_include_directories(${IPRO_BSP}/hal/lp_framework/ipro7)
  endif()

  # ---- HAL device layer (bsp/hal/src) --------------------------------------
  set(IPRO_HAL_SRC ${IPRO_BSP}/hal/src)
  zephyr_library_sources(
    ${IPRO_HAL_SRC}/hal_common.c
    ${IPRO_HAL_SRC}/hal_sys.c
    ${IPRO_HAL_SRC}/hal_boot2_${IPRO_CHIP_LC}.c
    ${IPRO_HAL_SRC}/hal_flash.c
    ${IPRO_HAL_SRC}/hal_gpio.c
    ${IPRO_HAL_SRC}/hal_uart.c
    ${IPRO_HAL_SRC}/hal_mtimer.c
    ${IPRO_HAL_SRC}/hal_timer.c
    ${IPRO_HAL_SRC}/hal_wdt.c
    ${IPRO_HAL_SRC}/hal_aon_wdt.c
    ${IPRO_HAL_SRC}/hal_rtc.c
    ${IPRO_HAL_SRC}/hal_ptmgr.c
    ${IPRO_HAL_SRC}/hal_sec_common.c
    ${IPRO_HAL_SRC}/hal_sec_trng.c
    ${IPRO_SDK_ROOT}/components/utils/src/utils_crc.c
    ${IPRO_SDK_ROOT}/components/softcrc/softcrc.c
  )
  zephyr_library_sources_ifdef(CONFIG_SPI_IPRO      ${IPRO_HAL_SRC}/hal_spi.c)
  zephyr_library_sources_ifdef(CONFIG_I2C_IPRO      ${IPRO_HAL_SRC}/hal_i2c.c)
  zephyr_library_sources_ifdef(CONFIG_PWM_IPRO      ${IPRO_HAL_SRC}/hal_pwm.c)
  zephyr_library_sources_ifdef(CONFIG_DMA_IPRO      ${IPRO_HAL_SRC}/hal_dma.c)
  zephyr_library_sources_ifdef(CONFIG_ADC_IPRO      ${IPRO_HAL_SRC}/hal_adc.c)
  zephyr_library_sources_ifdef(CONFIG_DAC_IPRO      ${IPRO_HAL_SRC}/hal_dac.c)
  zephyr_library_sources_ifdef(CONFIG_I2S_IPRO      ${IPRO_HAL_SRC}/hal_i2s.c)
  zephyr_library_sources_ifdef(CONFIG_AUDIO_DMIC_IPRO ${IPRO_HAL_SRC}/hal_auadc.c)
  zephyr_library_sources_ifdef(CONFIG_IPRO_HAL_USE_PSRAM ${IPRO_HAL_SRC}/hal_psram.c)

  # ---- camera / ISP pipeline (CONFIG_IPRO_CAMERA) --------------------------
  # hal_isp over the MM-domain LL (DVP_AS, ISP, scaler, DVP2AXI, OSD, MM
  # misc), the SDK sensor registry (components/sensor) and an SCCB layer on
  # the Zephyr I2C API. The sensor descriptors live in .camera_desc (linker
  # snippet ipro7_camera_desc.ld); the ISP firmware's 3A (img_proc) is not
  # built - exposure/white balance stay at the sensor's power-on values.
  if(CONFIG_IPRO_CAMERA)
    zephyr_library_sources(
      ${IPRO_HAL_SRC}/hal_isp.c
      ${IPRO_LL}/drv_isp.c
      ${IPRO_LL}/drv_isp_wdr.c
      ${IPRO_LL}/drv_dvp2axi.c
      ${IPRO_LL}/drv_scaler_hydre.c
      ${IPRO_LL}/drv_tsrc.c
      ${IPRO_LL}/drv_osd_blend.c
      ${IPRO_LL}/drv_osd_draw.c
      ${IPRO_LL}/drv_mm_misc.c
      ${IPRO_SDK_ROOT}/components/sensor/camera.c
      ${IPRO_SDK_ROOT}/components/sensor/sensor_utils.c
      ${IPRO_SDK_ROOT}/components/sensor/sc101iot.c
      ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_sccb_glue.c
    )
    zephyr_include_directories(${IPRO_SDK_ROOT}/components/sensor)
    zephyr_linker_sources(ROM_SECTIONS ${CMAKE_CURRENT_LIST_DIR}/soc/intelpro/ipro7/ipro7_camera_desc.ld)
    zephyr_link_libraries(-Wl,-u,__rt_camera_sc101iot_desc)
  endif()

  # ---- bsp/common ----------------------------------------------------------
  zephyr_library_sources_ifdef(CONFIG_IPRO_HAL_PLATFORM_UTILS
    ${IPRO_BSP}/common/platform_utils/misc.c
    ${IPRO_BSP}/common/platform_utils/ring_buffer.c
  )

  # ---- OS glue: the seams where the HAL meets the kernel -------------------
  zephyr_library_sources(
    ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_irq_glue.c
    ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_platform_glue.c
  )
  # Facades for HAL entry points whose names clash with Zephyr driver APIs
  # (see include/ipro_zephyr/ipro_hal_glue.h).
  if(CONFIG_SOC_SERIES_IPRO6S)
    zephyr_library_sources_ifdef(CONFIG_GPIO_IPRO    ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_gpio_glue_ipro6s.c)
  else()
    zephyr_library_sources_ifdef(CONFIG_GPIO_IPRO    ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_gpio_glue.c)
  endif()
  zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_IPRO ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_flash_glue.c)
  zephyr_library_sources_ifdef(CONFIG_WDT_IPRO       ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_wdt_glue.c)
  zephyr_library_sources_ifdef(CONFIG_DMA_IPRO       ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_dma_glue.c)
  zephyr_library_sources_ifdef(CONFIG_IPRO_HAL_SDK_LOG
    ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_log_glue.c
  )

  # The HAL was written for the SDK's -Wall set; Zephyr adds a few more.
  # Keep the SDK sources warning-clean without patching them for style.
  zephyr_library_compile_options(${IPRO_SDK_WARNING_OPTS})

endif() # CONFIG_IPRO_HAL

# ---- Wireless: RF front-end + IEEE 802.15.4 (components/wireless) --------
# The SDK's RF and 802.15.4 drivers ("core" libraries) build from source here
# and link as prebuilt archives in the public package, where their sources
# are not shipped; the Zephyr-side glue always builds from source.
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ipro_core_prebuilt.cmake)

if(CONFIG_IPRO_WIRELESS_RF)
  set(IPRO_RF ${IPRO_SDK_ROOT}/components/wireless/rf/ipro7_rf)
  set(IPRO_RF_PRIVATE_INC ${IPRO_RF}/src
    ${CMAKE_CURRENT_LIST_DIR}/include/ipro_hal_shim_wireless)
  zephyr_include_directories(${IPRO_RF}/api ${IPRO_RF}/reg ${IPRO_RF}/inc)

  set(IPRO_RF_CORE_SOURCES
    ${IPRO_RF}/src/ipro7_phy.c
    ${IPRO_RF}/src/ipro7_rf_private.c
    ${IPRO_RF}/src/ipro7_cli.c
  )
  ipro_core_sources_present(IPRO_RF_CORE_FROM_SOURCE ${IPRO_RF_CORE_SOURCES})
  if(IPRO_RF_CORE_FROM_SOURCE)
    zephyr_library_named(ipro_rf_core)
    zephyr_library_sources(${IPRO_RF_CORE_SOURCES})
    zephyr_library_include_directories(${IPRO_RF_PRIVATE_INC})
    zephyr_library_compile_options(${IPRO_SDK_WARNING_OPTS})
    zephyr_library_link_libraries(ipro_hal)
  else()
    ipro_core_link_prebuilt(ipro_rf_core ${IPRO_RF})
  endif()

  zephyr_library_named(ipro_rf)
  zephyr_library_sources(${IPRO_BSP}/hal/src/hal_wireless.c)
  zephyr_library_include_directories(${IPRO_RF_PRIVATE_INC})
  zephyr_library_compile_options(${IPRO_SDK_WARNING_OPTS})
  zephyr_library_link_libraries(ipro_hal)
endif()

if(CONFIG_IPRO_WIRELESS_IEEE802154)
  set(IPRO_154 ${IPRO_SDK_ROOT}/components/wireless/ieee802154)
  zephyr_include_directories(${IPRO_154}/include ${IPRO_154}/hal/ipro7 ${IPRO_154}/reg/ipro7)
  set(IPRO_154_PRIVATE_INC
    ${IPRO_154}/lmac/include
    ${IPRO_154}/lmac
    ${IPRO_BSP}/hal/lp_framework/ipro7
    ${CMAKE_CURRENT_LIST_DIR}/include/ipro_hal_shim_wireless)
  set(IPRO_154_DEFS
    LMAC154_PLAT_H="lmac154_ipro7.h"
    LMAC154_DEP_H="lmac154_ipro7_imp.h"
    # The SDK driver's entry point has the same name as Zephyr's L2
    # ieee802154_init(iface). Rename it consistently (declaration in the SDK
    # header + definition + the facade's call) inside these libraries only;
    # the FreeRTOS SDK keeps the original symbol.
    ieee802154_init=ipro_sdk_ieee802154_init
  )
  # The Enh-ACK path (built and secured in the MAC interrupt against
  # aTurnaroundTime) runs from RAM: from flash XIP a short frame's late
  # Enh-ACK started ~350 us after it. Same attribute as Zephyr's __ramfunc,
  # handed to ieee802154_port.h's #ifndef guard: that header must not include
  # <zephyr/toolchain.h> (its likely/unlikely/STRINGIFY clash with the SDK's).
  if(CONFIG_XIP AND CONFIG_ARCH_HAS_RAMFUNC_SUPPORT)
    list(APPEND IPRO_154_DEFS
      "IEEE802154_HOT_SECTION=__attribute__((noinline,section(\".ramfunc\")))"
      "IEEE802154_HOT_INLINE=__attribute__((section(\".ramfunc\")))")
  else()
    list(APPEND IPRO_154_DEFS IEEE802154_HOT_SECTION= IEEE802154_HOT_INLINE=)
  endif()

  set(IPRO_154_CORE_SOURCES
    ${IPRO_154}/src/ieee802154.c
    ${IPRO_154}/src/ieee802154_frame.c
    ${IPRO_154}/src/ieee802154_mac.c
    ${IPRO_154}/src/ipro154_sec.c
    ${IPRO_154}/lmac/lmac154.c
    ${IPRO_154}/lmac/zb_timer.c
    ${IPRO_154}/lmac/lmac154_fpt.c
    ${IPRO_154}/lmac/lmac154_fpt_mode.c
    ${IPRO_154}/hal/ipro7/ieee802154_hal_lmac154.c
    ${IPRO_154}/hal/ipro7/ieee802154_hal_trace.c
  )
  ipro_core_sources_present(IPRO_154_CORE_FROM_SOURCE ${IPRO_154_CORE_SOURCES})
  if(IPRO_154_CORE_FROM_SOURCE)
    zephyr_library_named(ipro_ieee802154_core)
    zephyr_library_sources(${IPRO_154_CORE_SOURCES})
    zephyr_library_include_directories(${IPRO_154_PRIVATE_INC})
    zephyr_library_compile_definitions(${IPRO_154_DEFS})
    zephyr_library_compile_options(${IPRO_SDK_WARNING_OPTS})
    zephyr_library_link_libraries(ipro_hal ipro_rf)
  else()
    # The archive is built for an XIP image with RAM functions (the Enh-ACK
    # path above sits in .ramfunc); both IPRO7 boards are.
    if(NOT (CONFIG_XIP AND CONFIG_ARCH_HAS_RAMFUNC_SUPPORT))
      message(FATAL_ERROR "the prebuilt ipro_ieee802154_core needs CONFIG_XIP and RAM function support")
    endif()
    ipro_core_link_prebuilt(ipro_ieee802154_core ${IPRO_154})
  endif()

  zephyr_library_named(ipro_ieee802154)
  zephyr_library_sources(${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_ieee802154_glue.c)
  zephyr_library_include_directories(${IPRO_154_PRIVATE_INC})
  zephyr_library_compile_definitions(${IPRO_154_DEFS})
  zephyr_library_compile_options(${IPRO_SDK_WARNING_OPTS})
  zephyr_library_link_libraries(ipro_hal ipro_rf)
endif()

# ---- Bluetooth: IPRO7 BLE controller (prebuilt archive or source build) --
if(CONFIG_IPRO_BT_CTLR_PREBUILT)
  set(IPRO_BLE_CTLR ${IPRO_SDK_ROOT}/components/wireless/bluetooth/ipro_ble_controller)

  # The controller's own build description (variant/feature -> archive name,
  # global defines, and for a source build the patched source list, include
  # directories and CFG_* defines) is the same file the SDK component uses.
  # It reads the SDK's CONFIG_IPRO_BLE_CTLR_* names; map the Zephyr ones.
  set(CONFIG_IPRO_BLE_CTLR_LIB ${CONFIG_BT_IPRO_CTLR_LIB})
  set(CONFIG_IPRO_BLE_CTLR_FEATURE ${CONFIG_BT_IPRO_CTLR_FEATURE})
  set(CONFIG_IPRO_BLE_CTLR_CFG_DBG ${CONFIG_BT_IPRO_CTLR_CFG_DBG})
  set(CONFIG_EM_SIZE ${CONFIG_IPRO7_EM_SIZE_KB})
  set(CONFIG_IPRO_BLE_CTLR_NATIVE_SLEEP_DEFAULT n)
  if(CONFIG_BT_IPRO_CTLR_SOURCE_BUILD)
    set(CONFIG_IPRO_BLE_CTLR_FORCE_SOURCE_BUILD y)
    set(IPRO_BLE_CTLR_WANT_SOURCES ON)
  endif()
  # The description checks the FreeRTOS host's CONFIG_BT_CONN (a count) against
  # the variant; Zephyr's CONFIG_BT_CONN is a bool, so hide it and check
  # CONFIG_BT_MAX_CONN ourselves.
  set(_zephyr_bt_conn ${CONFIG_BT_CONN})
  unset(CONFIG_BT_CONN)
  set(IPRO_BLE_CTLR_PATCHED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ipro_ble_ctlr_patched)
  include(${IPRO_BLE_CTLR}/cmake/ipro7_build.cmake)
  set(CONFIG_BT_CONN ${_zephyr_bt_conn})
  if(DEFINED CONFIG_BT_MAX_CONN AND DEFINED IPRO_BLE_CTLR_CON
     AND NOT CONFIG_BT_MAX_CONN EQUAL IPRO_BLE_CTLR_CON)
    message(FATAL_ERROR "CONFIG_BT_MAX_CONN=${CONFIG_BT_MAX_CONN} does not match the ${CONFIG_BT_IPRO_CTLR_LIB} controller (${IPRO_BLE_CTLR_CON} connections)")
  endif()

  if(CONFIG_BT_IPRO_CTLR_FEATURE STREQUAL "bt54_iso2" AND CONFIG_IPRO7_EM_SIZE_KB LESS 32)
    message(FATAL_ERROR "bt54_iso2 controller needs CONFIG_IPRO7_EM_SIZE_KB >= 32")
  endif()
  if(CONFIG_IPRO7_EM_SIZE_KB EQUAL 0)
    message(FATAL_ERROR "The BLE controller needs Exchange Memory: set CONFIG_IPRO7_EM_SIZE_KB (16 for bt52)")
  endif()

  # The archive was built by the SDK with -fshort-enums; every enum that
  # crosses the archive boundary must have the same representation, so the
  # whole Zephyr image adopts it when the controller is linked. The global
  # defines (trace depth, PDS capability, 32K policy) shape the controller
  # headers the glue and the HCI driver include.
  zephyr_compile_options(-fshort-enums)
  zephyr_compile_definitions(${IPRO_BLE_CTLR_GLOBAL_DEFS})

  zephyr_library_named(ipro_bt_ctlr)
  zephyr_include_directories(
    ${IPRO_BLE_CTLR}/include
    ${IPRO_BLE_CTLR}/hcitl
    ${IPRO_BLE_CTLR}/upstream/src/modules/rwip/api
    ${IPRO_BLE_CTLR}/upstream/src/modules/common/api
    ${IPRO_BLE_CTLR}/upstream/src/ip/hci/api
    ${IPRO_BLE_CTLR}/plf/ipro7/arch/main
    ${IPRO_BLE_CTLR}/plf/ipro7/nvds
    ${IPRO_BLE_CTLR}/plf/ipro7/lowpower
    ${IPRO_BSP}/hal/lp_framework/ipro7
    ${CMAKE_CURRENT_LIST_DIR}/include/ipro_freertos_shim
  )
  zephyr_library_sources(${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_bt_ctlr_glue.c)
  zephyr_library_link_libraries(ipro_hal ipro_rf)

  if(CONFIG_BT_IPRO_CTLR_SOURCE_BUILD)
    # Same sources, patches, includes and CFG_* defines as the SDK component
    # (and the prebuilt archives), compiled here. upstream/ is vendor code
    # built without -Werror, as in the SDK.
    zephyr_library_sources(${IPRO_BLE_CTLR_SRC_FINAL})
    zephyr_library_include_directories(${IPRO_BLE_CTLR_PUBLIC_INC} ${IPRO_BLE_CTLR_PRIVATE_INC})
    zephyr_library_compile_definitions(${IPRO_BLE_CTLR_DEFS})
    zephyr_library_compile_options(${IPRO_BLE_CTLR_COMPILE_OPTIONS})
    zephyr_library_compile_options(${IPRO_SDK_WARNING_OPTS} -Wno-error -Wno-unused-parameter
      -Wno-sign-compare -Wno-implicit-fallthrough -Wno-missing-field-initializers)
    message(STATUS "[ipro] BLE controller: source build (${IPRO_BLE_CTLR_LIBRARY_NAME})")
  else()
    if(NOT EXISTS ${IPRO_BLE_CTLR_PREBUILT})
      message(FATAL_ERROR "IPRO BLE controller archive not found: ${IPRO_BLE_CTLR_PREBUILT} (set CONFIG_BT_IPRO_CTLR_SOURCE_BUILD=y to build it)")
    endif()
    if(IPRO_BLE_CTLR_PREBUILT MATCHES ",")
      message(FATAL_ERROR "The BLE controller archive path must not contain a comma: ${IPRO_BLE_CTLR_PREBUILT}")
    endif()
    zephyr_library_sources(${IPRO_BLE_CTLR}/prebuilt_anchor.c)
    # Linked whole, like the source build (a Zephyr library, which Zephyr
    # links whole), so both builds load the same objects; --gc-sections then
    # drops what nothing references.
    #
    # Flag and path go in ONE -Wl, item. Given as three items, CMake keeps the
    # flags where they are but moves the archive path to the end of the link
    # line, so the pair ends up empty and the archive is linked as a plain
    # library: only the members some symbol reference reaches are loaded.
    zephyr_link_libraries("-Wl,--whole-archive,${IPRO_BLE_CTLR_PREBUILT},--no-whole-archive")
    message(STATUS "[ipro] BLE controller: ${IPRO_BLE_CTLR_PREBUILT}")
  endif()
  # The FreeRTOS linker symbol _sp the archives reference (see the file).
  zephyr_linker_sources(SECTIONS ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_bt_ctlr_sp.ld)
endif()

# ---- Wi-Fi 6 (IPRO6S): SDK components built in place ----------------------
if(CONFIG_IPRO_WIFI)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/ipro_sdk_components.cmake)
  # The SDK CMake files test the FreeRTOS-side configuration names.
  set(CONFIG_IPRO6 y)
  set(CONFIG_WIFI6 y)
  set(CONFIG_IPRO_EVENT n)
  set(CONFIG_MACSW_FORCE_SOURCE_BUILD y)
  # fhost's boot default for the 40 MHz permission (CFG_HT40): the AP and
  # the modem capability still decide the width.
  if(CONFIG_IPRO_WIFI_HT40)
    set(CONFIG_WIFI_HT40_ENABLE y)
  endif()
  # The supplicant's includes.h takes <sys/socket.h> alone on lwIP builds;
  # the shim in include/ipro_hal_shim_wireless/sys serves it from Zephyr's
  # IP headers.
  set(CONFIG_LWIP y)
  zephyr_compile_definitions(CONFIG_LWIP)
  # Public headers every consumer of the stack needs (the glue, the driver).
  zephyr_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/subsys/net/ipro_wifi/include   # net_def.h before the SDK's
    ${IPRO_SDK_ROOT}/components/wireless/wifi6/wifi6_lwip_adapter/include  # net_al.h
    ${IPRO_SDK_ROOT}/components/wireless/wifi6/ipro_os_adapter/include
    ${IPRO_SDK_ROOT}/components/wireless/wifi6/ipro_fhost/include
    ${IPRO_SDK_ROOT}/components/wireless/macsw/include
    ${CMAKE_CURRENT_LIST_DIR}/include/ipro_freertos_shim
    ${CMAKE_CURRENT_LIST_DIR}/include/ipro_hal_shim_wireless)
  # RF parameter TLVs (flash "rftlv" area) and the mfg media (efuse/flash MAC)
  # the RF driver and the platform glue read, from the SoC driver tree.
  zephyr_include_directories(${IPRO_SOC_DRV}/rfparam/inc)
  ipro_sdk_add_component(components/wireless/rf/ipro6_rf)
  ipro_sdk_add_component(components/wireless/ipro_coex)
  ipro_sdk_add_component(components/wireless/macsw)
  ipro_sdk_add_component(components/wireless/wifi6)
  if(CONFIG_IPRO_WIFI_HOT_CODE)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/ipro_wifi_hot.cmake)
  endif()
  ipro_sdk_link_components()

  zephyr_library_named(ipro_wifi_glue)
  zephyr_library_sources(
    ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_wifi_rtos_al.c
    ${CMAKE_CURRENT_LIST_DIR}/hal_glue/ipro_wifi_platform.c
    # (from the SoC driver tree; they need the RF component's headers)
    ${IPRO_SOC_DRV}/rfparam/src/rfparam.c
    ${IPRO_SOC_DRV}/rfparam/src/rfparam_adapter.c
    ${IPRO_SOC_DRV}/rfparam/src/rfparam_rftlv.c
    ${IPRO_SOC_DRV}/std_drv/src/drv_mfg_media.c
    ${IPRO_SOC_DRV}/std_drv/src/drv_mfg_efuse.c
    ${IPRO_SOC_DRV}/std_drv/src/drv_mfg_flash.c
  )
  zephyr_library_compile_options(${IPRO_SDK_WARNING_OPTS} -Wno-error -Wno-incompatible-pointer-types)
  zephyr_library_link_libraries(ipro_hal __component_lib)
  add_subdirectory(subsys/net/ipro_wifi)
  zephyr_linker_sources(SECTIONS ${CMAKE_CURRENT_LIST_DIR}/soc/intelpro/ipro6s/ipro6s_wifi.ld)
endif()

add_subdirectory(drivers)
add_subdirectory_ifdef(CONFIG_IPRO_SHELL_CMDS subsys/shell)

if(CONFIG_IPRO_WIFI)
  # MAC RX buffer count (WRAM); fhost_rx_config.h only provides a default.
  zephyr_compile_definitions(FHOST_RX_BUF_CNT=${CONFIG_IPRO_WIFI_FHOST_RX_BUF_CNT})
  # MAC RX buffer 1 (WRAM); rxl_hwdesc.c only provides a default.
  zephyr_compile_definitions(RXL_BUFFER1_BYTES=${CONFIG_IPRO_WIFI_RXL_BUF1_BYTES})
  # The SDK's Zephyr TCP patch (zephyr/patches.yml) is worth ~25 % of a TCP
  # download over Wi-Fi; say so when the tree lacks it.
  file(STRINGS ${ZEPHYR_BASE}/subsys/net/ip/tcp_private.h _ipro_tcp_ack_patch REGEX "rx_unacked")
  if(NOT _ipro_tcp_ack_patch)
    message(STATUS "IPRO Wi-Fi: Zephyr without the SDK TCP ACK patch (west patch apply)")
  endif()
endif()
