# JPEG -> RGB565 / RGB888 sample (software decoder) - see main.c and jdec_rgb.h.
# The decoder (TJpgDec R0.03) is built from tjpgd/, so the app needs nothing
# outside the SDK release packages.
__component_add_include_dirs(.)
__component_add_include_dirs(./tjpgd)

target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/jdec_rgb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tjpgd/tjpgd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_images.c
)

# Corrupt data can overflow the decoder's integer IDCT; -fwrapv makes that a
# defined wrap-around (garbage pixels) instead of undefined behaviour.
target_compile_options(app PRIVATE -fwrapv)

# Decoding runs about 1.9x faster at -O2 than at the SDK's -Os. Setting it on
# two sources of the app target needs CMake 3.18; older CMake keeps -Os.
if(NOT CMAKE_VERSION VERSION_LESS 3.18)
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/jdec_rgb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tjpgd/tjpgd.c
        TARGET_DIRECTORY app
        PROPERTIES COMPILE_OPTIONS -O2
    )
endif()

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)

__set_main_file(main.c)
setup_project(ipro7_jpeg_decode)
