if (CONFIG_APP_USB_DEVICE_MSC)
    message("USB Device MSC is enabled")
endif()

if(CONFIG_APP_SPI_SENSOR)
    message("SPI Sensor is enabled")
endif()

if(CONFIG_MM_SUPPORT)
    message("MM Support is enabled")
endif()

__component_add_include_dirs(.)
__component_add_include_dirs(./include)
__component_add_include_dirs(./config)

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
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/clock_tree_dump.c
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
                # Override optimization for dhrystone files only
                # COMPILE_FLAGS appends after target flags; last -O wins in GCC
                set_source_files_properties(${DHRYSTONE_SRC_FILES} PROPERTIES COMPILE_FLAGS "-O2")
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

        # PIO-based SD host demo (A/B vs hardware SDH)
        if (CONFIG_DEMO_PIO_SDH)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/pio_sdcard_cmd.c
            )
        endif()

        # I2S audio interface demo
        if (CONFIG_DEMO_I2S)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/i2s_demo.c
            )
        endif()

        # I2S DMIC record test
        if (CONFIG_DEMO_I2S_DMIC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/i2s_dmic_demo.c
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
        endif()

        # ROM API test
        if (CONFIG_DEMO_ROMAPI)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/tc_romapi.c
            )
        endif()

        # SPI camera demo
        if (CONFIG_DEMO_SPICAM)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/spi_cam.c
            )
        endif()

        # Generic ADC voltage demo (moved out of audio/ directory)
        if (CONFIG_DEMO_PERIPHERAL_ADC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/adc_demo.c
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/peripheral/tsen_demo.c
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
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/audio/padc_demo.c
            )
        endif()

        # Audio DAC demo
        if (CONFIG_DEMO_DAC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/audio/dac_demo.c
            )
        endif()

        # Simultaneous DAC + AMIC duplex loopback test
        if (CONFIG_DEMO_DAC AND CONFIG_DEMO_ADC)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/audio/duplex_demo.c
            )
        endif()

        # Audio transfer over ESP32-AT WiFi (upload captured PCM to host)
        if (CONFIG_DEMO_ESP32_AT AND (CONFIG_DEMO_DAC OR CONFIG_DEMO_ADC))
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/audio/audio_net_demo.c
            )
        endif()

        # MP3 decode and playback demo
        if (CONFIG_DEMO_MP3)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/audio/mp3_demo.c
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
            # Known-answer test vs sts-2.1.2 (embeds 128KB reference data)
            if (CONFIG_NIST_KAT)
                target_sources(app PRIVATE
                    ${NIST_SP800_22_TEST_DIR}/trng_kat_test.c
                    ${NIST_SP800_22_TEST_DIR}/nist_kat_data.c
                )
            endif()
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
        # CDC ACM is always compiled (no callback conflicts)
        target_sources(app PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/cdc_acm_template.c
        )

        if (CONFIG_CHERRYUSB_DEVICE_AUDIO AND CONFIG_CHERRYUSB_DEVICE_CDC_NCM)
            # Composite mode: UAC+NCM in one file (replaces individual uac/ncm
            # templates to avoid duplicate CherryUSB callback symbols)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/composite_uav_ncm_template.c
            )
            # Video template compiled separately (no callback conflicts)
            if (CONFIG_CHERRYUSB_DEVICE_VIDEO)
                target_sources(app PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/video_static_mjpeg_template.c
                )
            endif()
        else()
            # Individual mode: compile templates separately
            if (CONFIG_CHERRYUSB_DEVICE_VIDEO)
                target_sources(app PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/video_static_mjpeg_template.c
                )
            endif()
            if (CONFIG_CHERRYUSB_DEVICE_AUDIO AND NOT CONFIG_DEMO_MP3)
                target_sources(app PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/uac_v1_template.c
                )
            endif()
            if (CONFIG_CHERRYUSB_DEVICE_CDC_NCM)
                target_sources(app PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/ncm_template.c
                )
            endif()
        endif()

        if (CONFIG_CHERRYUSB_DEVICE_CDC_ECM)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/cdc_ecm_template.c
            )
        endif()
        if (CONFIG_CHERRYUSB_DEVICE_RNDIS)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/usb_demo/rndis_template.c
            )
        endif()
    endif()

    # -------------------------------------------------------------------------
    # SoC Info / eFuse CLI (always on when DEMO_SOC + SHELL enabled)
    # -------------------------------------------------------------------------
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/soc/efuse_cli.c
    )

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
    # Files with unguarded refs to AI / LCD / SPICAM symbols. Exclude unless
    # the matching feature is enabled — keeps a minimal MM build (e.g. RTSP
    # ISP source) link-clean.
    if (NOT CONFIG_AI_SUPPORT)
        list(FILTER MM_DEMO_SRC_FILES EXCLUDE REGEX "/uvc_ai_demo\\.c$")
        list(FILTER MM_DEMO_SRC_FILES EXCLUDE REGEX "/lcd_ai_demo\\.c$")
        list(FILTER MM_DEMO_SRC_FILES EXCLUDE REGEX "/stream_control\\.c$")
    endif()
    if (NOT CONFIG_DEMO_SPICAM)
        list(FILTER MM_DEMO_SRC_FILES EXCLUDE REGEX "/mm_demo\\.c$")
    endif()
    target_sources(app PRIVATE ${MM_DEMO_SRC_FILES})

    # ISP components are now built as separate libraries via __generate_library()
    # Sources are handled by components/isp_firmware/CMakeLists.txt
    # and components/isp_algorithm/CMakeLists.txt
    # App still needs includes for demo code
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/components/isp_firmware/include
        ${CMAKE_SOURCE_DIR}/components/isp_algorithm/fast/include
    )

    file(GLOB_RECURSE ISP_IP_PARAM_FILES
        ${CMAKE_SOURCE_DIR}/components/isp_iq_param/*.c
    )
    target_sources(app PRIVATE ${ISP_IP_PARAM_FILES})

    file(GLOB_RECURSE ISP_SENSOR_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../components/sensor/*.c
    )
    target_sources(app PRIVATE ${ISP_SENSOR_FILES})
endif()

# =============================================================================
# AI Demo (requires shell)
# =============================================================================
if (CONFIG_AI_SUPPORT AND CONFIG_SHELL)
    message("AI Support is enabled")
    file(GLOB_RECURSE AI_DEMO_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/demo/ai/*.c)
    target_sources(app PRIVATE ${AI_DEMO_SRC_FILES})
    # Add AI demo header directory for external modules
    __component_add_include_dirs(./demo/ai)
endif()

# =============================================================================
# ESP32-AT Network Application Demo
# =============================================================================
if (CONFIG_DEMO_ESP32_AT)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/esp32_at/demo_esp32_at.c
    )
endif()

# =============================================================================
# Command Scheduler CLI (app-specific command sequences)
# =============================================================================
if (CONFIG_CMD_SCHEDULER)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cmd_scheduler_cli.c
    )
endif()

# =============================================================================
# BLE Demo (requires CONFIG_BLUETOOTH_LE_ENABLE)
# =============================================================================
if (CONFIG_BLUETOOTH_LE_ENABLE)
    message("BLE Support is enabled")

    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/bluetooth/ble_demo.c
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/bluetooth/ble_demo_app.c
    )
    __component_add_include_dirs(./demo/bluetooth)

    # ============================================================================
    # Zephyr Bluetooth Stack Includes (Same as ipro_ble_remote)
    # ============================================================================

    # Add BLE controller includes
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/btble_inc)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/rwip/api)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/common/api)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/ke/api)

    # Add platform/compiler includes
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch/compiler)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/btblecontroller/plf/ipro7/src/arch/compiler/riscvgcc)

    # Add Zephyr blestack includes
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/include)
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/blestack/src/host)

    # Add IPRO Log System includes
    __component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)
endif()

# =============================================================================
# Build Configuration
# =============================================================================

# Note: main.c is NOT added to app library - it's only compiled for the ELF target
# via __set_main_file() to avoid duplicate symbol errors with --whole-archive

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
# Force whole-archive linking for app to include shell commands from demo files
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro7_demo)
