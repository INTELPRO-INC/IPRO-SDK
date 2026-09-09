__component_add_include_dirs(.)
__component_add_include_dirs(./src)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/core/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/inst/include)

target_sources(app PRIVATE src/model_inm.S)

# .incbin resolves the path against the assembler's include path.
set_property(SOURCE src/model_inm.S APPEND PROPERTY COMPILE_OPTIONS
             -I${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_options(app PRIVATE -Werror)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
__set_main_file(main.c)
setup_project(ipro_npu_yolo)
