# IPRO LE Audio Headset Application
# Uses standard Zephyr Bluetooth Audio API (not RivieraWaves Component Framework)

# Include directories
__component_add_include_dirs(.)
__component_add_include_dirs(./src)
__component_add_include_dirs(./include)

# IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# ============================================================================
# Zephyr Bluetooth Stack Includes (Same as ipro_ble_remote)
# ============================================================================

# New controller + Zephyr 4.4 host port.  LE Audio host/profile sources are
# imported by ipro_bt_host when CONFIG_BT_AUDIO is selected.  The optional
# CONFIG_IPRO_LE_AUDIO_NEW_HOST_SMOKE mode intentionally skips profile/audio
# registration and validates only BLE init, SMP callbacks and LE Audio UUID
# advertising.
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_ble_controller/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/port/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/upstream/zephyr/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/bluetooth/ipro_bt_host/upstream/zephyr/subsys/bluetooth/host)

# Add VFS includes (for file system operations)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/fs/vfs/include)

# ============================================================================
# Application Source Files (Refactored for Zephyr API)
# ============================================================================
target_sources(app PRIVATE
    # Core application files
    src/ble_init.c          # NEW: BLE stack initialization (Zephyr API)
)

if(NOT CONFIG_IPRO_LE_AUDIO_NEW_HOST_SMOKE)
    target_sources(app PRIVATE
        src/ble_main.c          # Main LE Audio profile logic
        src/bap_server.c        # BAP Unicast Server implementation
        src/vcp_server.c        # VCP Volume Control implementation
        src/cap_acceptor.c      # CAP Common Audio Profile
        src/app_pcm_monitor.c   # PCM waveform monitoring
        src/iso_content_validator.c # LC3 payload integrity validation
    )
endif()

# Audio output: DAC (speaker) or UAC (USB capture device)
# Conditionally compiled to save RAM when audio output not needed
if (CONFIG_IPRO_LE_AUDIO_NEW_HOST_SMOKE)
    message(STATUS "[ipro_le_audio_headset] New-host smoke: LE Audio profile/audio sources disabled")
elseif(CONFIG_AUDIO_OUTPUT_UAC)
    target_sources(app PRIVATE
        src/app_audio.c     # LC3 decoder
        src/app_uac.c
        src/usbd_event_common.c
    )
elseif(CONFIG_BSP_USING_DAC)
    target_sources(app PRIVATE
        src/app_audio.c     # LC3 decoder
        src/app_dac.c
    )
else()
    # Audio output disabled - saves ~100 KB RAM (s_audio, s_dac, s_resampler buffers)
    message(STATUS "[ipro_le_audio_headset] Audio output disabled - saving RAM")
endif()

if((CONFIG_AUDIO_OUTPUT_UAC OR CONFIG_BSP_USING_DAC) AND NOT CONFIG_IPRO_LE_AUDIO_NEW_HOST_SMOKE)
    target_sources(app PRIVATE
        src/app_shell_commands.c # Shell commands for audio/debug output
    )
endif()

# Set compiler options
target_compile_options(app PRIVATE -Werror)

# LC3 Codec includes (now via independent 3rdparty component)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/3rdparty/liblc3/include)

# ============================================================================
# Dependencies and Component Configuration
# ============================================================================

# Add IPRO Log System includes (for IPRO_LOG macros)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# Add VFS includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/fs/vfs/include)

# ============================================================================
# Final Setup
# ============================================================================

# Compiler options - disable -Werror for this project due to complex stack headers
target_compile_options(app PRIVATE -Wno-error)

# Set main file and project setup
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(src/main.c)

# Apply audio output definitions to ALL sources (including main.c)
if (CONFIG_AUDIO_OUTPUT_UAC)
    target_compile_definitions(app PRIVATE CONFIG_AUDIO_OUTPUT_UAC)
elseif(CONFIG_BSP_USING_DAC)
    target_compile_definitions(app PRIVATE CONFIG_BSP_USING_DAC)
endif()

setup_project(ipro_le_audio_headset)
