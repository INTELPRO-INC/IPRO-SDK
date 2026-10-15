if(NOT CONFIG_RTSP_SERVER)
    return()
endif()

__generate_library(1)  # allow warnings: the sources have sockaddr-cast and similar warnings

# GCC 14 promotes several warnings to errors by default; these sources
# were written against older GCC. Demote these back to warnings for this
# component only.
target_compile_options(${CURRENT_STATIC_LIBRARY} PRIVATE
    -Wno-error=incompatible-pointer-types
    -Wno-error=int-conversion
    -Wno-error=implicit-function-declaration
    -Wno-error=implicit-int
)

__component_add_include_dirs(
    inc
)

if(CONFIG_RTSP_TEST_SOURCE)
    __component_add_include_dirs(test_source)
endif()

if(CONFIG_RTSP_ISP_SOURCE)
    __component_add_include_dirs(isp_source)
    __component_add_private_include_dirs(
        ${CMAKE_SOURCE_DIR}/bsp/hal/inc
        ${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/std_drv/inc
        ${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/regs
        ${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/risc-v/Core/Include
        ${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/startup
        ${CMAKE_SOURCE_DIR}/bsp/board/ipro7
        ${CMAKE_SOURCE_DIR}/bsp/common/platform_utils/include
        ${CMAKE_SOURCE_DIR}/components/sensor
        ${CMAKE_SOURCE_DIR}/components/isp_firmware/include
    )
endif()

# RTSP server sources (Apache-2.0, see LICENSE-APACHE-2.0.txt).
__component_add_sources(
    src/delay_task.c
    src/jpeg_parser.c
    src/librtspsrv.c
    src/log.c
    src/rtcp.c
    src/rtp.c
    src/rtsp_method.c
    src/rtsp_parser.c
    src/rtsp_sess.c
    src/rtsp_srv.c
    src/sd_handler.c
    src/util.c
)

if(CONFIG_RTSP_TEST_SOURCE)
    __component_add_sources(
        test_source/rtsp_test_source.c
    )
endif()

if(CONFIG_RTSP_ISP_SOURCE)
    __component_add_sources(
        isp_source/rtsp_isp_source.c
    )
endif()

if(CONFIG_RTSP_SHELL)
    __component_add_sources(
        shell/rtsp_shell.c
    )
endif()

# Force whole-archive linking so SHELL_CMD_EXPORT_ALIAS entries are kept
__component_whole_archive()
