__generate_library()

__component_add_include_dirs(include)
if (NOT CONFIG_BOOTROM)
__component_add_sources(platform_common.c)
endif()
__component_add_sources(cpp_new.cpp)
__component_add_sources(misc.c)
#__component_add_sources(drv_device.c)
__component_add_sources(drv_mmheap.c)
__component_add_sources(ring_buffer.c)

#__component_add_compile_definitions_ifndef(BOOTROM -DCONFIG_FLASH_INIT)
