# ipro7ai_sensor_tag_demo: the sensors of the IPRO7AI Sensor Tag board, streamed over the
# board's own USB as `name:value` lines (IPRO Workbench plots them and shows
# them on its Sensor Dashboard; see dashboard.yaml and README.md).

__component_add_include_dirs(.)
__component_add_include_dirs(./drivers/include)

# Shared USB CDC console (+ cherryusb usb_config.h).
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/apps/turnkey/common/include)

target_sources(app PRIVATE
    tag_board.c
    drivers/ism330bx.c
    drivers/spi_bitbang.c
    drivers/cw2015.c
    ${CMAKE_SOURCE_DIR}/apps/turnkey/common/usb_console.c
)

# USB identity: its own product id and name, so it is told from other firmware
# on the same board.
target_compile_definitions(app PRIVATE
    USB_CONSOLE_PID=0x7018
    USB_CONSOLE_PRODUCT="IPRO7AI Sensor Tag Demo"
    USB_CONSOLE_SERIAL="TAGDEMO-1"
)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro7ai_sensor_tag_demo)

# printf / puts / putchar (and so IPRO_LOG) go to the USB CDC console:
# usb_console.c defines the __wrap_* targets.
target_link_options(ipro7ai_sensor_tag_demo_IPRO7.elf PRIVATE
    -Wl,--wrap=printf
    -Wl,--wrap=puts
    -Wl,--wrap=putchar
)
