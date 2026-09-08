# ipro_pio_sdh_test — dedicated PIO SDH (GPIO22-27) + FatFS validation/demo.
# pio_sdcard (PIO host) with the HW-SDH sdcard/sdh_speed_test/format_sd
# commands (GPIO28-33) as the on-board A/B reference. Console + shell + log
# run over USB CDC (apps/turnkey/common/usb_console.c). See README.md.

__component_add_include_dirs(.)

# Shared USB CDC console (+ cherryusb usb_config.h).
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/apps/turnkey/common/include)

target_sources(app PRIVATE
    src/pio_sdcard_cmd.c
    src/fatfs_pio_mmc_port.c
    src/hw_sdcard_cmd.c
)

# USB CDC console — only when CherryUSB is enabled in .config. Flip the
# CherryUSB/BOOT block in .config off to get a UART-console build (used to
# isolate USB runtime interference from the PIO SDH timing).
if(CONFIG_COMPONENTS_CHERRYUSB_ENABLE)
    target_sources(app PRIVATE
        ${CMAKE_SOURCE_DIR}/apps/turnkey/common/usb_console.c
    )
endif()

# USB identity — distinct PID/strings so this enumerates as its own device.
target_compile_definitions(app PRIVATE
    USB_CONSOLE_PID=0x7014
    USB_CONSOLE_PRODUCT="IPRO PIO SDH Test"
    USB_CONSOLE_SERIAL="PIOSDH-1"
)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_pio_sdh_test)

# Redirect newlib printf / puts / putchar (and therefore IPRO_LOG) from the
# debug UART to the USB CDC console (usb_console.c defines the __wrap_* targets).
if(CONFIG_COMPONENTS_CHERRYUSB_ENABLE)
    target_link_options(ipro_pio_sdh_test_IPRO7.elf PRIVATE
        -Wl,--wrap=printf
        -Wl,--wrap=puts
        -Wl,--wrap=putchar
    )
endif()
