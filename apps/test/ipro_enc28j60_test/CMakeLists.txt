# ENC28J60 SPI Ethernet bring-up test (IPRO7 Arduino board)

__component_add_include_dirs(.)
__component_add_include_dirs(src)

# Shared USB CDC console (+ cherryusb usb_config.h) for the Arduino board.
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/apps/turnkey/common/include)

# NOTE: main.c is registered ONLY via __set_main_file() below, so it compiles
# into the ELF target alone. Listing it here too would double-define main()
# once the app archive is force-linked with --whole-archive.
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/enc28j60.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/enc28j60_cli.c
    ${CMAKE_SOURCE_DIR}/apps/turnkey/common/usb_console.c
)

target_compile_options(app PRIVATE -Wall -Wextra)

# USB identity — distinct PID/strings so this enumerates as its own device.
target_compile_definitions(app PRIVATE
    USB_CONSOLE_PID=0x7012
    USB_CONSOLE_PRODUCT="IPRO ENC28J60 Test"
    USB_CONSOLE_SERIAL="ENC-0001"
)

# Force-link the whole app archive so the shell command in enc28j60_cli.c
# (registered only via its FSymTab section, not referenced by main) survives.
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)

__set_main_file(main.c)
setup_project(ipro_enc28j60_test)

# Redirect newlib printf / puts / putchar (and therefore IPRO_LOG) from the
# debug UART to the USB CDC console (usb_console.c defines the __wrap_*
# targets). USB-only console — no parallel UART output.
target_link_options(ipro_enc28j60_test_IPRO7.elf PRIVATE
    -Wl,--wrap=printf
    -Wl,--wrap=puts
    -Wl,--wrap=putchar
)
