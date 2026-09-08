# IPRO Low Power Test (No BLE, No PSRAM)

__component_add_include_dirs(.)

# IPRO Log System
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# Application sources
target_sources(app PRIVATE
    src/lp_test.c
    src/lp_test_cli.c
)

target_compile_options(app PRIVATE -Werror)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_lp_test)
