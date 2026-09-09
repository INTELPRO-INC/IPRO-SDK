# ipro_epd_el036 — bring-up app for E Ink Spectra 6 EL036MF1 (3.6" 400x600)

__component_add_include_dirs(.)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

target_sources(app PRIVATE
    src/epd_el036.c
    src/epd_pio.c
    # The PIO driver is only added to the SDK build behind CONFIG_HAL_PIO_SDH,
    # which would pull in the whole PIO SD host with it. Compile it here so the
    # app stays self-contained and no SDK file needs patching.
    ${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/std_drv/src/drv_pio.c
)

target_compile_options(app PRIVATE -Werror)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_epd_el036)
