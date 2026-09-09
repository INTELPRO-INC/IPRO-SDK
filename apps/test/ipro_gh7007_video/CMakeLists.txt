# SD-card video player on the GH7007-01 5" SPI LCD (IPRO7 Arduino board)
#
# Pipeline: FATFS on PIO-SDH (CLK=G22 CMD=G23 D0-3=G24-27) reads pre-converted
# .ghv frames (wire-format RGB565, see tools/video_convert.py) into PSRAM;
# each frame streams to the panel via the proven 48 MHz sync row path.

__component_add_include_dirs(.)
__component_add_include_dirs(src)

# Shared USB CDC console (+ cherryusb usb_config.h) for the Arduino board.
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/apps/turnkey/common/include)

target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gh7007.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gh7007_patterns.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gh7007_cli.c
    ${CMAKE_SOURCE_DIR}/apps/turnkey/common/usb_console.c
)

target_compile_options(app PRIVATE -Wall -Wextra)

# USB identity — distinct PID/strings so this enumerates as its own device.
target_compile_definitions(app PRIVATE
    USB_CONSOLE_PID=0x7015
    USB_CONSOLE_PRODUCT="IPRO GH7007 Video Player"
    USB_CONSOLE_SERIAL="GHVID-1"
)

# Force-link the whole app archive so shell commands registered only via
# FSymTab sections (lcd, video) survive.
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)

__set_main_file(main.c)
setup_project(ipro_gh7007_video)

# Redirect newlib printf / puts / putchar (and therefore IPRO_LOG) from the
# debug UART to the USB CDC console (usb_console.c defines the __wrap_* targets).
target_link_options(ipro_gh7007_video_IPRO7.elf PRIVATE
    -Wl,--wrap=printf
    -Wl,--wrap=puts
    -Wl,--wrap=putchar
)
