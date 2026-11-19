# IPRO6S Classic Bluetooth example: Serial Port Profile (SPP) echo server.

__component_add_include_dirs(.)

target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_classic_common.c
)

target_compile_options(app PRIVATE -Werror)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
# Whole-archive link keeps the shell commands the app registers.
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro6s_bt_spp)
