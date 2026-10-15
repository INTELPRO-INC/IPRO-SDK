# IPRO7AI EVK board library: ST7789P3 panel + row-compositing gfx (always),
# integer PCM math (always; the resampler is components/audio's audio_asrc),
# SC101IOT camera -> ISP -> preview/NPU (CONFIG_EVK_CAMERA), PDM microphone
# and DAC speaker (CONFIG_EVK_AUDIO), micro-SD + FatFS (CONFIG_EVK_SDCARD),
# shell commands (CONFIG_EVK_SHELL).
#
# The shell commands live only in their FSymTab section and are never
# referenced, so the archive is force-linked whole when they are on.

__generate_library()

__component_add_include_dirs(include)

__component_add_sources(src/evk.c)
__component_add_sources(src/st7789p3.c)
__component_add_sources(src/font5x7.c)
__component_add_sources(src/evk_gfx.c)
__component_add_sources(src/evk_pcm.c)

if(CONFIG_EVK_CAMERA)
    if(NOT CONFIG_MM_SUPPORT OR NOT CONFIG_COMPONENTS_SENSOR_ENABLE)
        message(FATAL_ERROR "CONFIG_EVK_CAMERA needs CONFIG_MM_SUPPORT=y and CONFIG_COMPONENTS_SENSOR_ENABLE=y")
    endif()
    __component_add_sources(src/evk_cam.c)
endif()

if(CONFIG_EVK_AUDIO)
    if(NOT CONFIG_AUDIO_OUTPUT)
        message(FATAL_ERROR "CONFIG_EVK_AUDIO needs CONFIG_AUDIO_OUTPUT=y (components/audio DAC sink)")
    endif()
    __component_add_sources(src/evk_mic.c)
    __component_add_sources(src/evk_spk.c)
endif()

if(CONFIG_EVK_SDCARD)
    if(NOT CONFIG_COMPONENTS_FATFS_ENABLE OR NOT CONFIG_EVFS_USE_FATFS)
        message(FATAL_ERROR "CONFIG_EVK_SDCARD needs CONFIG_EVFS=y, CONFIG_EVFS_USE_FATFS=y and CONFIG_COMPONENTS_FATFS_ENABLE=y")
    endif()
    __component_add_sources(src/evk_sd.c)
endif()

if(CONFIG_EVK_SHELL)
    __component_add_sources(src/evk_shell.c)
    # SHELL_CMD_EXPORT_ALIAS casts int(*)(int,char**) to the shell's generic
    # syscall_func; that is the shell's contract.
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/evk_shell.c
        PROPERTIES COMPILE_OPTIONS -Wno-cast-function-type)
    __component_whole_archive()
endif()
