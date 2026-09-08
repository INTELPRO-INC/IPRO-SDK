__generate_library()

################# Add global include #################
__component_add_include_dirs(include)
#######################################################

############## Add source files ###########
# Resampler is always built (small, used by output backends and app code)
__component_add_sources(src/audio_resampler.c)

# Speaker correction is a property of the board's speaker, not of the output
# framework: apps with their own DAC driver (the LE Audio headset) use it too,
# so it is built unconditionally alongside the resampler.
__component_add_sources(src/audio_speaker_eq.c)
__component_add_sources(src/audio_bass_boost.c)

if(CONFIG_AUDIO_CAPTURE)
    __component_add_sources(src/audio_capture.c)
endif()

if(CONFIG_AUDIO_OUTPUT)
    __component_add_sources(src/audio_output.c)
    __component_add_sources(src/audio_output_dac.c)
    __component_add_sources(src/audio_output_dump.c)
    # UAC backend pulls in CherryUSB headers — only build when USB audio is on.
    # Apps that implement their own UAC device set AUDIO_OUTPUT_UAC_EXCLUDE to
    # avoid colliding on the class's fixed-name usbd_audio_* callbacks.
    if(CONFIG_CHERRYUSB_DEVICE_AUDIO AND NOT CONFIG_AUDIO_OUTPUT_UAC_EXCLUDE)
        __component_add_sources(src/audio_output_uac.c)
    endif()
    # I2S backend pulls in hal_i2s — only build when I2S peripheral is enabled
    if(CONFIG_BSP_USING_I2S0)
        __component_add_sources(src/audio_output_i2s.c)
    endif()
endif()

# MP3 decoder is opt-in (pulls in minimp3)
if(CONFIG_DEMO_MP3)
    __component_add_sources(src/mp3_decoder.c)
endif()
