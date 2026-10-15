# IPRO7AI EVK rock-paper-scissors: SC101IOT camera -> ISP -> NPU classifier
# (rock/paper/scissors/none) with the game on the ST7789P3 LCD.

__component_add_include_dirs(.)
__component_add_include_dirs(src)

# main.c is registered only via __set_main_file() below so it compiles into the
# ELF target alone - listing it here as well would double-define main() once the
# app archive is force-linked with --whole-archive.
# Panel, camera pipeline and font come from components/board/ipro7ai_evk
# (CONFIG_COMPONENTS_IPRO7AI_EVK_ENABLE + CONFIG_EVK_CAMERA in .config).
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rps_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rps_game.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rps_ui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_cli.c
)

# CONFIG_RPS_EMBED_MODEL links model/rps_v3.inm into the image instead of
# reading it from the mfg partition (src/model_blob.S).
if(CONFIG_RPS_EMBED_MODEL)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/model_blob.S)
    # TARGET_DIRECTORY on both: `app` is defined at the top level, and a plain
    # set_property(SOURCE ...) here only writes this directory's scope, where
    # nothing reads it.
    # .incbin resolves its path against the assembler include path.
    set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/model_blob.S TARGET_DIRECTORY app
                 APPEND PROPERTY COMPILE_OPTIONS -I${CMAKE_CURRENT_SOURCE_DIR}/model)
    # The assembler emits no dependency information for .incbin, so a retrained
    # model would otherwise leave the stale one linked in.
    set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/model_blob.S TARGET_DIRECTORY app
                 APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/model/rps_v3.inm)
endif()

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
setup_project(ipro7ai_evk_rps)
