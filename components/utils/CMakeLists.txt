add_subdirectory(getopt)

__generate_library()

################# Add global include #################
__component_add_include_dirs(include)
#######################################################

############## Add current dir source files ###########
if (CONFIG_FREERTOS_SUPPORT)
__component_add_sources(src/utils_rbtree.c)
endif()
__component_add_sources(src/utils_crc.c)
__component_add_sources(src/utils_string.c)
__component_add_sources(src/utils_base64.c)
__component_add_sources(src/utils_list.c)
__component_add_sources(src/utils_hex.c)
__component_add_sources(src/utils_sha256.c)

# Add IPRO Log System
if (CONFIG_IPRO_LOG_ENABLE)
__component_add_sources(src/ipro_log.c)
endif()
