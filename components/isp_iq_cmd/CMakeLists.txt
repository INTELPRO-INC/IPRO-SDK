
__generate_library()

# Public include directory (exported to consumers)
__component_add_include_dirs(include)

__component_add_sources(
    src/isp_cmd_parse.c
    src/helper_socket.c
)

# Private includes (needed for compilation only)
__component_add_private_include_dirs(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/components/sys/cli/include
	${CMAKE_SOURCE_DIR}/components/isp_firmware/include
	${CMAKE_SOURCE_DIR}/components/os/freertos/Source/include
	${CMAKE_SOURCE_DIR}/components/os/freertos/Source/portable_ipro7
    ${CMAKE_SOURCE_DIR}/bsp/hal/inc
    ${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/std_drv/inc
    ${CMAKE_SOURCE_DIR}/bsp/common/platform_utils/include
    ${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/startup    
    ${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/regs
	${CMAKE_SOURCE_DIR}/bsp/drivers/ipro7_soc/risc-v/Core/Include
	${CMAKE_SOURCE_DIR}/bsp/drivers/soc_common/inc
	${CMAKE_SOURCE_DIR}/bsp/board/ipro7
	${CMAKE_SOURCE_DIR}/bsp/board/ipro7/evb
	${CMAKE_SOURCE_DIR}/components/sensor
)

__component_add_private_compile_definitions(CONFIG_IPRO7)

