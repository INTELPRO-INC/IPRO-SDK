# ipro_epd_test — bring-up app for GDEY0213F52 (2.13" BWRY) via DESPI-C02

__component_add_include_dirs(.)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

target_sources(app PRIVATE
    src/epd_gdey0213f52.c
    src/pricetag_render.c
)

target_compile_options(app PRIVATE -Werror)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_epd_test)
