__component_add_include_dirs(.)
__component_add_include_dirs(./src)

target_sources(app PRIVATE
    src/model_zoo.cc
    src/models.S
)

# .incbin resolves its paths against the assembler include path.
set_property(SOURCE src/models.S APPEND PROPERTY COMPILE_OPTIONS
             -I${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_options(app PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    -DTF_LITE_STATIC_MEMORY
)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
__set_main_file(main.c)
setup_project(ipro_tflm_test)
