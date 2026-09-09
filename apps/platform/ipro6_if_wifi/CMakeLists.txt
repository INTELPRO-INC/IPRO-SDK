# IPRO6 SDIO-WiFi device application.
# Dedicated, stripped build: SDIO-WiFi (SMID transport) + WiFi6 + low-power only.
# Cloned from ipro6_demo with all unrelated peripheral/test/benchmark demos removed.

# Use generated linker script (IPRO6 uses IPRO7's BSP)
set(CONFIG_USE_GENERATED_LINKER_SCRIPT ON)

__component_add_include_dirs(.)
__component_add_include_dirs(./include)

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
                )
            endif()
        endif()

    endif() # CONFIG_DEMO_PERIPHERAL

endif() # CONFIG_DEMO_SOC AND CONFIG_SHELL

# =============================================================================
# Standalone lp_fw Stage-1 image (IPRO6 low-power demo, two-build incbin)
# =============================================================================
# Build the minimal lp_fw image as its own bin, then embed it via .incbin
# (demo/soc/lowpower/lpfw_blob.S). lp_demo.c memcpy's it to the reserved OCRAM
# slot (0x11048000) at boot. Gated on the low-power demo so a normal build does
# not pull in the separate lp_fw sub-build.
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
setup_project(ipro6_if_wifi)
