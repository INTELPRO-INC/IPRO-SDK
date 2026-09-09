# Use generated linker script (IPRO6 uses IPRO7's BSP)
set(CONFIG_USE_GENERATED_LINKER_SCRIPT ON)

if(CONFIG_MM_SUPPORT)
    message("MM Support is enabled")
endif()

__component_add_include_dirs(.)
__component_add_include_dirs(./include)

# Add semihosting include path if fileio enabled
if(CONFIG_SEMIHOSTING_FILEIO)
    __component_add_include_dirs(${IPRO_SDK_BASE}/components/libc/semihosting)
endif()

target_compile_options(app PRIVATE -Werror)

# =============================================================================
# SOC Demos (requires CONFIG_DEMO_SOC and CONFIG_SHELL)
# =============================================================================
if (CONFIG_DEMO_SOC AND CONFIG_SHELL)

    # -------------------------------------------------------------------------
    # Peripheral Demos
    # -------------------------------------------------------------------------
    if (CONFIG_DEMO_PERIPHERAL)

        # Basic peripherals: timer, WDT, UART, SD card
        if (CONFIG_DEMO_PERIPHERAL_BASIC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/peripheral_demo.c
            )
        endif()

        # OTA staging (shared core at apps/platform/common/ota): FW-B sink +
        # HW-SHA256 validate + PT apply, over UART zmodem and/or USB CDC.
        # ipro6_demo already provides usbd_event_handler via usb_demo, so it does
        # NOT compile the shared ota_usb_event.c (that would duplicate the symbol).
        set(OTA_DIR ${IPRO_SDK_BASE}/apps/platform/common/ota)
        if (CONFIG_COMPONENTS_ZMODEM_ENABLE OR (CONFIG_DEMO_USB AND CONFIG_COMPONENTS_CHERRYUSB_ENABLE))
            __component_add_include_dirs(${OTA_DIR})
            target_sources(app PRIVATE ${OTA_DIR}/ota_common.c)
        endif()
        if (CONFIG_COMPONENTS_ZMODEM_ENABLE)
            target_sources(app PRIVATE ${OTA_DIR}/ota_zmodem.c)
        endif()
        if (CONFIG_DEMO_USB AND CONFIG_COMPONENTS_CHERRYUSB_ENABLE)
            target_sources(app PRIVATE ${OTA_DIR}/ota_usb.c)
        endif()

        # ROM API test suite (disabled by default)
        if (CONFIG_DEMO_ROMAPI_TEST)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/tc_romapi.c
            )
        endif()

        # CPU benchmark: memory, DMA, cache
        if (CONFIG_DEMO_CPU_BENCH)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/cpu_demo.c
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/memory_dma_demo.c
            )

            # Cache test (optional, adds ~40KB BSS)
            if (CONFIG_DEMO_CACHE_TEST)
                target_sources(app PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/cache_test.c
                )
            endif()

            # Dhrystone benchmark (optional, adds ~10KB BSS)
            if (CONFIG_DEMO_DHRYSTONE)
                file(GLOB DHRYSTONE_SRC_FILES
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/dhrystone/*.c
                )
                target_sources(app PRIVATE ${DHRYSTONE_SRC_FILES})
            endif()
        endif()

        # PWM demo
        if (CONFIG_DEMO_PWM)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/tc_pwm.c
            )
        endif()

        # SPI and Flash demo
        if (CONFIG_DEMO_SPI)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/tc_spi.c
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/tc_sflash.c
            )
        endif()

        # I2S audio interface demo
        if (CONFIG_DEMO_I2S)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/i2s_demo.c
            )
        endif()

        # LCD display demo
        if (CONFIG_DEMO_LCD)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/lcd_demo.c
            )
            file(GLOB_RECURSE LCD_DRIVER_SRC_FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/lcd/*.c
            )
            target_sources(app PRIVATE ${LCD_DRIVER_SRC_FILES})
        endif()

        # Ethernet MAC demo
        if (CONFIG_DEMO_EMAC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/emac_demo.c
            )
        endif()

        # Low power demo
        if (CONFIG_DEMO_LOWPOWER)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/lowpower_demo.c
            )
            # IPRO6 PDS/HBN/DTIM app flow + `lp` shell command (staged lp_fw image).
            # The lp_fw blob two-build is wired below (also IPRO6-gated).
            if (CONFIG_IPRO6)
                __component_add_include_dirs(demo/soc/lowpower)
                target_sources(app PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/lowpower/lp_demo.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/lowpower/lp_demo_cli.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/lowpower/lp_tcpdtim.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/lowpower/lp_tickless.c
                )
            endif()
        endif()

        # SPI camera demo
        if (CONFIG_DEMO_SPICAM)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/spi_cam.c
            )
        endif()

    endif() # CONFIG_DEMO_PERIPHERAL

    # -------------------------------------------------------------------------
    # Audio Demos
    # -------------------------------------------------------------------------
    if (CONFIG_DEMO_AUDIO)

        # Audio ADC demo
        if (CONFIG_DEMO_ADC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/audio/adc_demo.c
            )
        endif()

        # Audio DAC demo
        if (CONFIG_DEMO_DAC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/audio/dac_demo.c
            )
        endif()

    endif() # CONFIG_DEMO_AUDIO

    # -------------------------------------------------------------------------
    # Security Demos
    # -------------------------------------------------------------------------
    if (CONFIG_DEMO_SECURITY)
        file(GLOB SEC_DEMO_SRC_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/security/*.c
        )
        # Exclude NIST test unless explicitly enabled (requires PSRAM)
        list(FILTER SEC_DEMO_SRC_FILES EXCLUDE REGEX "trng_nist_test\\.c$")
        target_sources(app PRIVATE ${SEC_DEMO_SRC_FILES})

        # NIST TRNG test suite (optional, requires PSRAM for large buffer)
        # Test source is provided by nist_sp800_22 component in test/ directory
        if (CONFIG_DEMO_TRNG_NIST)
            target_sources(app PRIVATE
                ${NIST_SP800_22_TEST_DIR}/trng_nist_test.c
            )
        endif()
    endif()

    # -------------------------------------------------------------------------
    # DSP Demos
    # -------------------------------------------------------------------------
    if (CONFIG_DEMO_DSP)
        file(GLOB DSP_DEMO_SRC_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/dsp/*.c
        )
        target_sources(app PRIVATE ${DSP_DEMO_SRC_FILES})
    endif()

    # -------------------------------------------------------------------------
    # Semihosting Demos
    # -------------------------------------------------------------------------
    if (CONFIG_DEMO_SEMIHOSTING)
        file(GLOB SEMIHOST_DEMO_SRC_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/semihosting/*.c
        )
        target_sources(app PRIVATE ${SEMIHOST_DEMO_SRC_FILES})
    endif()

    # -------------------------------------------------------------------------
    # Filesystem Demos
    # -------------------------------------------------------------------------
    if (CONFIG_DEMO_FS)
        file(GLOB_RECURSE FS_DEMO_SRC_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/fs/*.c
        )
        target_sources(app PRIVATE ${FS_DEMO_SRC_FILES})
    endif()

    # -------------------------------------------------------------------------
    # System Demos
    # -------------------------------------------------------------------------
    if (CONFIG_DEMO_SYS)
        file(GLOB_RECURSE SYS_DEMO_SRC_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/sys/*.c
        )
        target_sources(app PRIVATE ${SYS_DEMO_SRC_FILES})
    endif()

    # -------------------------------------------------------------------------
    # USB Demos
    # -------------------------------------------------------------------------
    if (CONFIG_DEMO_USB AND CONFIG_COMPONENTS_CHERRYUSB_ENABLE)
        if (CONFIG_CHERRYUSB_DEVICE_CDC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/cdc_acm_template.c
            )
        endif()
        if (CONFIG_CHERRYUSB_DEVICE_VIDEO)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/video_static_mjpeg_template.c
            )
        endif()
        if (CONFIG_CHERRYUSB_DEVICE_AUDIO)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/uac_v1_template.c
            )
        endif()
        if (CONFIG_CHERRYUSB_DEVICE_VENDOR_TP)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/usb_tp_test_template.c
            )
        endif()
    endif()

endif() # CONFIG_DEMO_SOC AND CONFIG_SHELL

# USB event common (always needed when CherryUSB is enabled)
if (CONFIG_COMPONENTS_CHERRYUSB_ENABLE)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/usbd_event_common.c
    )
endif()

# =============================================================================
# MM Demo (Multimedia)
# =============================================================================
if (CONFIG_MM_SUPPORT)
    file(GLOB_RECURSE MM_DEMO_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/demo/mm/*.c)
    target_sources(app PRIVATE ${MM_DEMO_SRC_FILES})
endif()
# =============================================================================
# Standalone lp_fw Stage-1 image (IPRO6 low-power demo, two-build incbin)
# =============================================================================
# Build the minimal lp_fw image as its own bin, then embed it via .incbin
# (demo/soc/lowpower/lpfw_blob.S). lp_demo.c memcpy's it to the reserved OCRAM
# slot (0x11048000) at boot. Gated on the low-power demo so a normal demo build
# does not pull in the separate lp_fw sub-build.
if (CONFIG_DEMO_LOWPOWER AND CONFIG_IPRO6)
    set(LPFW_DIR ${CMAKE_SOURCE_DIR}/apps/platform/ipro6_lpfw)
    set(LPFW_BIN ${LPFW_DIR}/build/ipro6_lpfw.bin)

    add_custom_command(
        OUTPUT  ${LPFW_BIN}
        COMMAND make -C ${LPFW_DIR} CROSS=${CROSS_COMPILE}
        DEPENDS ${LPFW_DIR}/lp_fw.c
                ${LPFW_DIR}/startup.S
                ${LPFW_DIR}/ipro6_lpfw.ld
                ${LPFW_DIR}/Makefile
                ${CMAKE_SOURCE_DIR}/bsp/hal/lp_framework/ipro6/lp_fw.h
        COMMENT "Building standalone lp_fw Stage-1 image -> ${LPFW_BIN}"
        VERBATIM
    )
    add_custom_target(ipro6_lpfw_image DEPENDS ${LPFW_BIN})

    # Embed the bin via a dedicated static lib defined in THIS directory so the
    # .incbin source's OBJECT_DEPENDS (re-assemble whenever the bin changes) is
    # honored. configure_file bakes the absolute bin path into the .incbin at
    # configure time (so the generated source exists for add_library).
    set(LPFW_BLOB_S ${CMAKE_CURRENT_BINARY_DIR}/lpfw_blob.S)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/lowpower/lpfw_blob.S.in ${LPFW_BLOB_S} @ONLY)
    add_library(lpfw_blob STATIC ${LPFW_BLOB_S})
    set_source_files_properties(${LPFW_BLOB_S} PROPERTIES OBJECT_DEPENDS "${LPFW_BIN}")
    add_dependencies(lpfw_blob ipro6_lpfw_image)
    set_property(GLOBAL APPEND PROPERTY SDK_LIBS lpfw_blob)
    set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS lpfw_blob)
endif()

# =============================================================================
# Build Configuration
# =============================================================================

# Note: main.c is NOT added to app library - it's only compiled for the ELF target
# via __set_main_file() to avoid duplicate symbol errors with --whole-archive
if (CONFIG_WIFI6 OR CONFIG_BLUETOOTH_LE_ENABLE)
    target_sources(app PRIVATE wl_board_cfg.c)
endif()

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
# Force whole-archive linking for app to include shell commands from demo files
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro6_demo)
