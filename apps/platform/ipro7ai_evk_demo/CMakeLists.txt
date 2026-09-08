# IPRO7AI EVK board demo: ST7789P3 display, SC101IOT camera, DAC speaker,
# PDM microphone and the internal supply-rail ADC.

__component_add_include_dirs(.)
__component_add_include_dirs(src)

# main.c is registered only via __set_main_file() below so it compiles into the
# ELF target alone - listing it here as well would double-define main() once the
# app archive is force-linked with --whole-archive.
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/st7789p3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/st7789p3_patterns.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/st7789p3_cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cam.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cam_cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vbat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vbus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdcard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vidplay.c
)

# The BLE files only ever build with a host to build against. bt.c and bt_cli.c
# guard themselves on CONFIG_BLUETOOTH_LE_ENABLE, but bt_shell.c includes
# <zephyr/bluetooth/*.h> at the top with no guard and bt_audio_out.c pulls in
# liblc3 and the whole ISO rate matcher for a stream that cannot exist - so the
# switch belongs here, where it also keeps the objects out of the archive.
if(CONFIG_BLUETOOTH_LE_ENABLE)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bt_audio_out.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bt_shell.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bt_cli.c
    )
endif()

# CherryUSB calls usbd_event_handler() by fixed name and components/audio's UAC
# backend registers through it, but the component cannot define the dispatcher
# itself: apps that drive USB directly define usbd_event_handler() themselves
# and would collide.  Every USB app in the tree carries this same shim.
if(CONFIG_CHERRYUSB_DEVICE_AUDIO)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd_event_common.c
    )
endif()

# liblc3 is an always-built independent component with no Kconfig switch, so
# only its headers need adding here; the archive is on the link line already.
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/3rdparty/liblc3/include)

# Read the autoplay switch straight out of .config.
#
# The generated autoconf.h only carries symbols the Kconfig tree declares, so an
# app-private option added to .config never reaches the compiler - both builds
# came out identical the first time this was tried. Reading the file here keeps
# the switch where the other build settings live without needing a Kconfig entry.
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/.config _evk_cfg)
if(_evk_cfg MATCHES "\nCONFIG_EVK_VIDEO_AUTOPLAY=y")
    target_compile_definitions(app PRIVATE CONFIG_EVK_VIDEO_AUTOPLAY=1)
    message(STATUS "[evk] SD card autoplay: ON")
else()
    message(STATUS "[evk] SD card autoplay: off (`vid auto on` starts it)")
endif()

# Which clip autoplay shows. Unset walks the whole list, largest first.
if(_evk_cfg MATCHES "\nCONFIG_EVK_VIDEO_CLIP=([0-9]+)")
    target_compile_definitions(app PRIVATE CONFIG_EVK_VIDEO_CLIP=${CMAKE_MATCH_1})
    message(STATUS "[evk] autoplay clip: ${CMAKE_MATCH_1}.ipv on repeat")
else()
    message(STATUS "[evk] autoplay clip: all of them, largest first")
endif()

if(_evk_cfg MATCHES "\nCONFIG_EVK_VIDEO_MUTE=y")
    target_compile_definitions(app PRIVATE CONFIG_EVK_VIDEO_MUTE=1)
    message(STATUS "[evk] video sound: muted at boot (`vid mute off` unmutes)")
else()
    message(STATUS "[evk] video sound: on")
endif()

if(_evk_cfg MATCHES "\nCONFIG_EVK_WAIT_VBUS=y")
    target_compile_definitions(app PRIVATE CONFIG_EVK_WAIT_VBUS=1)
    message(STATUS "[evk] hold demo until USB-C is powered: ON")
else()
    message(STATUS "[evk] hold demo until USB-C is powered: off")
endif()

target_compile_options(app PRIVATE -Wall -Wextra)

# The `lcd` shell command is reachable only through its FSymTab section, never
# referenced from main, so the app archive must be force-linked whole.
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)

__set_main_file(main.c)
setup_project(ipro7ai_evk_demo)
