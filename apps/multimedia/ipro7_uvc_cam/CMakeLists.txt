# IPRO7 UVC Camera Application CMakeLists.txt

__component_add_include_dirs(.)

target_compile_options(app PRIVATE -Werror)

# ISP / DVP / MJPEG pipeline + SC101IOT sensor
# The ISP algorithms (AE/AWB/tone mapping/defog) come from the isp_algorithm
# component, which components/CMakeLists.txt builds whenever CONFIG_MM_SUPPORT
# is set - as ipro7_demo uses it. Do not list its sources here: the SDK
# package ships that component as libisp_algorithm.a without its .c files.
if (CONFIG_MM_SUPPORT)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/uvc_isp.c
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
        ${CMAKE_CURRENT_SOURCE_DIR}/uvc_iq_sc101iot.c
        ${CMAKE_SOURCE_DIR}/components/sensor/sc101iot.c
    )

    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/components/isp_algorithm/fast/include
        ${CMAKE_SOURCE_DIR}/components/isp_iq_param/include
        ${CMAKE_SOURCE_DIR}/components/isp_firmware/include
        ${CMAKE_SOURCE_DIR}/bsp/hal/inc
    )
endif()

# USB UVC streaming (USB Video Class)
if (CONFIG_CHERRYUSB_DEVICE_VIDEO)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/uvc.c
    )
endif()

# AI on the NPU. The `ai` component itself is compiled automatically when
# CONFIG_AI_SUPPORT=y; here we add the app glue and the AI headers.
#   default         YOLOv8 person + pet boxes (uvc_ai.c, EVB)
#   CONFIG_UVC_RPS  rock-paper-scissors game drawn into the stream (uvc_rps.c,
#                   IPRO7AI Reef). The classifier, the game and the model are
#                   the EVK app's (apps/ai/ipro7ai_evk_rps) - one copy, two boards.
if (CONFIG_AI_SUPPORT)
    if (CONFIG_UVC_RPS)
        set(RPS_APP_DIR ${CMAKE_SOURCE_DIR}/apps/ai/ipro7ai_evk_rps)
        set(EVK_BOARD_DIR ${CMAKE_SOURCE_DIR}/components/board/ipro7ai_evk)
        target_sources(app PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/uvc_rps.c
            ${RPS_APP_DIR}/src/rps_model.c
            ${RPS_APP_DIR}/src/rps_game.c
            ${EVK_BOARD_DIR}/src/font5x7.c
        )
        target_include_directories(app PRIVATE
            ${RPS_APP_DIR}/src
            ${EVK_BOARD_DIR}/include
        )
        # Model linked into the image (CONFIG_RPS_EMBED_MODEL), as in the EVK
        # app: one file to flash. See that app's CMakeLists.txt for why both
        # properties need TARGET_DIRECTORY app.
        if (CONFIG_RPS_EMBED_MODEL)
            target_sources(app PRIVATE ${RPS_APP_DIR}/src/model_blob.S)
            set_property(SOURCE ${RPS_APP_DIR}/src/model_blob.S TARGET_DIRECTORY app
                         APPEND PROPERTY COMPILE_OPTIONS -I${RPS_APP_DIR}/model)
            set_property(SOURCE ${RPS_APP_DIR}/src/model_blob.S TARGET_DIRECTORY app
                         APPEND PROPERTY OBJECT_DEPENDS ${RPS_APP_DIR}/model/rps_v3.inm)
        endif()
    else()
        target_sources(app PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/uvc_ai.c
        )
    endif()
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/components/ai/src/core/include
        ${CMAKE_SOURCE_DIR}/components/ai/src/inst/include
        ${CMAKE_SOURCE_DIR}/components/ai/src/inst/include/detect
    )
endif()

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro7_uvc_cam)
