# IPRO7AI Reef: image Teach, voiceprints and Sensor Teach in one firmware, the
# only Edge AI firmware since 2026-11-01. image/, voice/ and sensor/ are the
# three tools' sources; src/ adds the mode store and the USB forwarding, and
# gives each tool its slot of the mfg partition (src/reef_mode.h has the map;
# partition_reef_teach.toml the partitions). image/ also holds the camera
# pipeline (uvc.c, uvc_isp.c, uvc_draw.c) that apps/multimedia/ipro7_uvc_cam
# builds as a plain USB camera.
set(IMG_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/image)
set(VOICE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/voice)
set(SENSOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sensor)
set(EVK_BOARD_DIR ${CMAKE_SOURCE_DIR}/components/board/ipro7ai_evk)

__component_add_include_dirs(.)
__component_add_include_dirs(src)
__component_add_include_dirs(${VOICE_DIR})
__component_add_include_dirs(${SENSOR_DIR})
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/core/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/inst/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/audio/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)
target_include_directories(app PRIVATE
    ${IMG_DIR}
    ${EVK_BOARD_DIR}/include
    ${CMAKE_SOURCE_DIR}/components/isp_algorithm/fast/include
    ${CMAKE_SOURCE_DIR}/components/isp_iq_param/include
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/include
    ${CMAKE_SOURCE_DIR}/bsp/hal/inc
    ${CMAKE_SOURCE_DIR}/components/ai/src/inst/include/detect
)

target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/reef_mode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/reef_apps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/reef_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/reef_multi.c
    # voiceprints (sensor/rgb_led.c is the same file: one copy is built)
    ${VOICE_DIR}/rgb_led.c
    ${VOICE_DIR}/spk_model.c
    ${VOICE_DIR}/voice.c
    ${VOICE_DIR}/voice_link.c
    ${VOICE_DIR}/voice_cli.c
    # Sensor Teach
    ${SENSOR_DIR}/sensor_app.c
    ${SENSOR_DIR}/sensor_cli.c
    ${SENSOR_DIR}/sensor_link.c
    ${SENSOR_DIR}/sensor_model.c
    ${SENSOR_DIR}/sensor_sim.c
    ${SENSOR_DIR}/sensor_src.c
    # image Teach: camera -> ISP -> MJPEG -> UVC + NPU
    ${IMG_DIR}/uvc.c
    ${IMG_DIR}/uvc_isp.c
    ${IMG_DIR}/uvc_iq_sc101iot.c
    ${IMG_DIR}/uvc_teach.c
    ${IMG_DIR}/teach_model.c
    ${IMG_DIR}/uvc_teach_link.c
    ${IMG_DIR}/uvc_draw.c
    ${EVK_BOARD_DIR}/src/font5x7.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/firmware_cs.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/firmware_ct.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/firmware_defog.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/firmware_gamma.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/firmware_tonemapping.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/firmware_yuvadj.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/firmware_yuvctrl.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/hal_isp_module.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/img_param.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/img_param_parse.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/img_proc.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/img_utils.c
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/src/isp_comm_sns.c
    ${CMAKE_SOURCE_DIR}/components/sensor/sc101iot.c
)

# mfg slots (offsets from the partition's start; src/reef_mode.h)
target_compile_definitions(app PRIVATE
    REEF_TEACH_COMBINED
    REEF_MODE_OFFSET=0x1000u
    VOICE_MFG_OFFSET=0x2000u    VOICE_MFG_SIZE=0x70000u
    TEACH_MFG_OFFSET=0x72000u   TEACH_MFG_SIZE=0x80000u
    SENSOR_MFG_OFFSET=0xF2000u  SENSOR_MFG_SIZE=0x40000u
)
target_compile_options(app PRIVATE -Wall -Wno-cast-function-type)
# the camera side as in ipro7_uvc_cam (CONFIG_SINGLE_PRECISION_FLOAT there)
set_source_files_properties(
    ${IMG_DIR}/uvc.c ${IMG_DIR}/uvc_isp.c ${IMG_DIR}/uvc_iq_sc101iot.c ${IMG_DIR}/uvc_teach.c
    ${IMG_DIR}/teach_model.c ${IMG_DIR}/uvc_teach_link.c ${IMG_DIR}/uvc_draw.c
    TARGET_DIRECTORY app PROPERTIES COMPILE_OPTIONS -fsingle-precision-constant)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_reef_teach)
