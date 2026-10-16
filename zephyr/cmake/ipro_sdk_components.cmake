# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
#
# Build SDK components with their own CMakeLists.txt inside a Zephyr build.
#
# The SDK's component files only use the __component_* macros from
# <sdk>/cmake/extension.cmake: each component becomes a STATIC library that
# links the INTERFACE target `__component_lib`, which carries every public
# include directory and compile definition the components publish for each
# other. Here `__component_lib` is bound to Zephyr's `zephyr_interface`, so
# the components compile with the Zephyr flags/autoconf and see the SDK HAL
# headers the ipro_hal library exposes, and ipro_sdk_link_components() hands
# the resulting archives to the Zephyr link.
#
#   ipro_sdk_add_component(<dir relative to the SDK root>)
#   ipro_sdk_link_components()          # once, after the last add
#
# CONFIG_* variables the SDK CMake files test (CONFIG_WIFI6, CONFIG_IPRO6,
# ...) are ordinary CMake variables here; set them before adding.

if(NOT TARGET __component_lib)
  add_library(__component_lib INTERFACE)
  target_link_libraries(__component_lib INTERFACE zephyr_interface)
endif()
# extension.cmake's __generate_library() adds -Werror unless this is unset.
set(CONFIG_BUILD_ERROR_ON_WARN n)
# Tells the few SDK CMake files that must differ under Zephyr (no lwIP, no
# SDK shell, the RTOS abstraction comes from hal_glue/).
set(IPRO_SDK_UNDER_ZEPHYR ON)
include(${IPRO_SDK_ROOT}/cmake/extension.cmake)

# Override: each SDK component becomes a Zephyr library (linked whole-archive
# with the kernel, ordered after the generated headers) instead of a plain
# static library. The rest of extension.cmake works unchanged on
# CURRENT_STATIC_LIBRARY.
macro(__generate_library)
  if(${ARGC} GREATER_EQUAL 2)
    set(__component_lib_name ${ARGV1})
  else()
    get_filename_component(__component_lib_name ${CMAKE_CURRENT_LIST_DIR} NAME)
  endif()
  zephyr_library_named(${__component_lib_name})
  set(CURRENT_STATIC_LIBRARY ${__component_lib_name})
  set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${__component_lib_name})
  target_link_libraries(${__component_lib_name} PUBLIC __component_lib)
  target_compile_options(${__component_lib_name} PRIVATE ${IPRO_SDK_WARNING_OPTS} -Wno-error
    -Wno-unused-parameter -Wno-undef -Wno-shadow -Wno-cpp -Wno-implicit-function-declaration
    -fasm)  # the MAC software uses the asm keyword (Zephyr compiles strict -std=c17)
endmacro()

function(ipro_sdk_add_component rel)
  add_subdirectory(${IPRO_SDK_ROOT}/${rel} ${CMAKE_CURRENT_BINARY_DIR}/sdk/${rel})
endfunction()

function(ipro_sdk_link_components)
  get_property(libs GLOBAL PROPERTY SDK_LIBS)
  message(STATUS "[ipro] SDK components: ${libs}")
  # Per-source options come last on the compile line, after the options
  # zephyr_interface hands every target (-Wall, -Wdouble-promotion, and
  # -Werror when twister builds with warnings-as-errors). Options on the
  # target itself come before those and lose, so the SDK's warning
  # relaxations are pinned here, where they win.
  # The MAC/RF software is only validated at -Os (the SDK's own level): built
  # at -O2 the Wi-Fi TX path stalls under load and association becomes flaky.
  # Zephyr appends its global -O flag after the target options too.
  set(warn_opts ${IPRO_SDK_WARNING_OPTS} -Wno-error)
  foreach(lib ${libs})
    get_target_property(srcs ${lib} SOURCES)
    if(NOT srcs)
      continue()
    endif()
    set(os_srcs ${srcs})
    if(CONFIG_IPRO_WIFI_HOT_CODE_O2 AND IPRO_WIFI_HOT_SDK_SOURCES)
      # The RAM-resident hot path is built at the image's -O2.
      list(REMOVE_ITEM os_srcs ${IPRO_WIFI_HOT_SDK_SOURCES})
    endif()
    set_source_files_properties(${srcs} TARGET_DIRECTORY ${lib}
      PROPERTIES COMPILE_OPTIONS "${warn_opts}")
    if(CONFIG_SPEED_OPTIMIZATIONS AND os_srcs)
      set_source_files_properties(${os_srcs} TARGET_DIRECTORY ${lib}
        PROPERTIES COMPILE_OPTIONS "${warn_opts};-Os")
    endif()
  endforeach()
endfunction()
