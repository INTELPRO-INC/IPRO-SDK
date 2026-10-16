# Agora Module - AOSL (OS Abstraction Layer) + RTC SDK
#
# AOSL: Platform abstraction (threads, memory, networking) compiled from source.
# SDK:  Agora RTC prebuilt library for real-time audio/video communication.

__generate_library(1)

# ============================================================================
# AOSL (Advanced Operating System Layer)
# ============================================================================
set(CONFIG_PLATFORM "ipro7")
set(AOSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/aosl)
set(AOSL_DECLARE_PROJECT OFF CACHE BOOL "Declare as Standalone Project" FORCE)

# The AOSL core is a git submodule: https://github.com/AgoraIO-Community/aosl
# at commit d825607 (2026-02-05), the version the prebuilt RTC library was
# built against; later AOSL versions change the API headers and are untested
# with it. The IPRO7 port the SDK builds is port/ipro7, not the copy under
# aosl/platform/src/ipro7: that copy predates the SDK's fixes.
set(AOSL_PINNED_COMMIT d8256077cef8a9ae57ad2ec2f2df87f3b6f0d0eb)
if(NOT EXISTS ${AOSL_DIR}/cmake/build.cmake)
    message(FATAL_ERROR
        "AOSL not found in ${AOSL_DIR}. It comes from "
        "https://github.com/AgoraIO-Community/aosl at commit d825607.\n"
        "In a git checkout of the SDK:\n"
        "  git submodule update --init components/3rdparty/agora/aosl\n"
        "From a release archive:\n"
        "  git clone https://github.com/AgoraIO-Community/aosl ${AOSL_DIR}\n"
        "  git -C ${AOSL_DIR} checkout d825607")
endif()
execute_process(COMMAND git -C ${AOSL_DIR} rev-parse HEAD
    OUTPUT_VARIABLE AOSL_HEAD OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET RESULT_VARIABLE AOSL_HEAD_RESULT)
if(AOSL_HEAD_RESULT EQUAL 0 AND NOT AOSL_HEAD STREQUAL AOSL_PINNED_COMMIT)
    message(WARNING
        "AOSL in ${AOSL_DIR} is at ${AOSL_HEAD}; the Agora RTC library was "
        "built against d825607 (git -C ${AOSL_DIR} checkout d825607).")
endif()

# Include AOSL build configuration (populates AOSL_ADD_* variables)
include(${AOSL_DIR}/cmake/build.cmake)

# Swap the submodule's platform/src/ipro7 for the SDK's port/ipro7
set(AOSL_UPSTREAM_PORT "${AOSL_DIR}/platform/src/${CONFIG_PLATFORM}/")
set(AOSL_SDK_PORT ${CMAKE_CURRENT_SOURCE_DIR}/port/${CONFIG_PLATFORM})
foreach(var AOSL_ADD_SOURCES AOSL_ADD_INCLUDES_PUBLIC AOSL_ADD_INCLUDES_PRIVATE)
    set(kept)
    foreach(item ${${var}})
        string(FIND "${item}" "${AOSL_UPSTREAM_PORT}" pos)
        if(NOT pos EQUAL 0)
            list(APPEND kept "${item}")
        endif()
    endforeach()
    set(${var} ${kept})
endforeach()
file(GLOB AOSL_SDK_PORT_SRCS ${AOSL_SDK_PORT}/*.c)
list(APPEND AOSL_ADD_SOURCES ${AOSL_SDK_PORT_SRCS})
list(APPEND AOSL_ADD_INCLUDES_PUBLIC ${AOSL_SDK_PORT}/config)

# Public includes
if(AOSL_ADD_INCLUDES_PUBLIC)
    foreach(inc_dir ${AOSL_ADD_INCLUDES_PUBLIC})
        __component_add_include_dirs(${inc_dir})
    endforeach()
endif()

# Private includes
if(DEFINED AOSL_ADD_INCLUDES_PRIVATE AND AOSL_ADD_INCLUDES_PRIVATE)
    list(LENGTH AOSL_ADD_INCLUDES_PRIVATE inc_count)
    if(inc_count GREATER 0)
        target_include_directories(${CURRENT_STATIC_LIBRARY} PRIVATE ${AOSL_ADD_INCLUDES_PRIVATE})
    endif()
endif()

# Sources
if(DEFINED AOSL_ADD_SOURCES AND AOSL_ADD_SOURCES)
    list(LENGTH AOSL_ADD_SOURCES src_count)
    if(src_count GREATER 0)
        __component_add_sources(${AOSL_ADD_SOURCES})
    endif()
endif()

# Compiler options
if(DEFINED AOSL_ADD_OPTIONS AND AOSL_ADD_OPTIONS)
    list(LENGTH AOSL_ADD_OPTIONS opt_count)
    if(opt_count GREATER 0)
        target_compile_options(${CURRENT_STATIC_LIBRARY} PRIVATE ${AOSL_ADD_OPTIONS})
    endif()
endif()

# Compiler definitions
if(DEFINED AOSL_ADD_DEFINITIONS AND AOSL_ADD_DEFINITIONS)
    foreach(def ${AOSL_ADD_DEFINITIONS})
        string(REGEX REPLACE "^-D" "" clean_def "${def}")
        if(clean_def)
            target_compile_definitions(${CURRENT_STATIC_LIBRARY} PRIVATE "${clean_def}")
        endif()
    endforeach()
endif()

# ============================================================================
# Agora RTC SDK (prebuilt)
# ============================================================================
__component_add_include_dirs(sdk/include)

set(AGORA_RTC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/riscv64/libagora-rtc-sdk.a)
if(NOT EXISTS ${AGORA_RTC_LIB})
    message(FATAL_ERROR "[agora] Prebuilt library not found: ${AGORA_RTC_LIB}")
endif()

set(AGORA_RTC_SDK_LIB agora_rtc_sdk)
add_library(${AGORA_RTC_SDK_LIB} STATIC IMPORTED GLOBAL)
set_target_properties(${AGORA_RTC_SDK_LIB} PROPERTIES
    IMPORTED_LOCATION ${AGORA_RTC_LIB})
set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${AGORA_RTC_SDK_LIB})
target_link_libraries(${AGORA_RTC_SDK_LIB} INTERFACE __component_lib)

# The prebuilt Agora SDK uses soft-float operations; link libgcc to
# supply soft-float builtins (__floatunsisf, __mulsf3, etc.) when
# the application is built with hardware float (ilp32f).
target_link_libraries(${AGORA_RTC_SDK_LIB} INTERFACE gcc)
