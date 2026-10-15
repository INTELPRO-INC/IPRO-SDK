# IPRO7AI EVK launcher: camera preview, NPU gesture navigation, microphone
# record / speaker playback and a status page - the four board peripherals in
# one image, driven through components/board/ipro7ai_evk.

__component_add_include_dirs(.)
__component_add_include_dirs(src)

# main.c is registered only via __set_main_file() below so it compiles into the
# ELF target alone - listing it here as well would double-define main() once the
# app archive is force-linked with --whole-archive.
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/npu_share.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gesture.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/softmax_q.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/detect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/input.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/voice.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/page_camera.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/page_detect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/page_voice.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/page_about.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_cli.c
)

target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/components/ai/src/core/include
    ${CMAKE_SOURCE_DIR}/components/ai/src/inst/include
)

# components/ai's YOLOv8 decoder prints three lines per detection at its
# default debug level - ~10 lines/s on this app's console. Off.
__component_add_compile_definitions(YOLOV8_DEBUG_LEVEL=0)

target_compile_options(app PRIVATE -Wall -Wextra)
# SHELL_CMD_EXPORT_ALIAS casts int(*)(int,char**) to the shell's generic
# syscall_func; that is the shell's contract, not something this file can fix.
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/app_cli.c
    PROPERTIES COMPILE_OPTIONS -Wno-cast-function-type)

# The shell command is reachable only through its FSymTab section, never
# referenced from main, so the app archive must be force-linked whole.
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)

__set_main_file(main.c)
setup_project(ipro7ai_evk_launcher)
