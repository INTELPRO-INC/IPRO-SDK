# IPRO7AI EVK person detection: SC101IOT camera -> ISP -> NPU (YOLOv8
# person_pet_detect) with the live preview and boxes on the ST7789P3 LCD.

__component_add_include_dirs(.)
__component_add_include_dirs(src)

# main.c is registered only via __set_main_file() below so it compiles into the
# ELF target alone - listing it here as well would double-define main() once the
# app archive is force-linked with --whole-archive.
# Panel, camera pipeline and font come from components/board/ipro7ai_evk
# (CONFIG_COMPONENTS_IPRO7AI_EVK_ENABLE + CONFIG_EVK_CAMERA in .config).
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/detect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_cli.c
)

# The `ai` component is compiled by CONFIG_AI_SUPPORT; its public headers are
# already on the global include path, but the detector post-processors live one
# level down.
target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/components/ai/src/core/include
    ${CMAKE_SOURCE_DIR}/components/ai/src/inst/include
)

target_compile_options(app PRIVATE -Wall -Wextra)
# SHELL_CMD_EXPORT_ALIAS casts int(*)(int,char**) to the shell's generic
# syscall_func; that is the shell's contract, not something this file can fix.
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/app_cli.c
    PROPERTIES COMPILE_OPTIONS -Wno-cast-function-type)

# The shell commands are reachable only through their FSymTab section, never
# referenced from main, so the app archive must be force-linked whole.
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)

__set_main_file(main.c)
setup_project(ipro7ai_evk_person_detect)
