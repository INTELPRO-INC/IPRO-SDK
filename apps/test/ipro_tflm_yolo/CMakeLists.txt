__component_add_include_dirs(.)
__component_add_include_dirs(./src)

target_sources(app PRIVATE
    src/yolo_runner.cc
    src/model_yolov8.S
)

# .incbin resolves "yolov8_person_pet.tflite" relative to the assembler's
# include path, not the source file.
set_property(SOURCE src/model_yolov8.S APPEND PROPERTY COMPILE_OPTIONS
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
setup_project(ipro_tflm_yolo)
