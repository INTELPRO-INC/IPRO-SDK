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
__component_add_sources(src/utils_list.c)
__component_add_sources(src/utils_hex.c)
# Base64 and SHA-256 carry a third-party copyright line and no licence text;
# a package that leaves them out (the Edge AI SDK: nothing there links them)
# still builds.
foreach(src src/utils_base64.c src/utils_sha256.c)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${src})
        __component_add_sources(${src})
    endif()
endforeach()

# Add IPRO Log System
if (CONFIG_IPRO_LOG_ENABLE)
__component_add_sources(src/ipro_log.c)
endif()
