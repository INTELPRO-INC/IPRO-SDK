# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
#
# SDK sources the public IPRO-SDK package ships as Zephyr-built archives
# instead of source: the IPRO7 RF driver (components/wireless/rf/ipro7_rf) and
# the IEEE 802.15.4 MAC (components/wireless/ieee802154). The internal tree
# compiles them into a "<name>" Zephyr library; tools/release builds that
# library once per enum representation and ships it as
#
#   <component>/lib/zephyr/lib<name>.a               default enums
#   <component>/lib/zephyr/lib<name>_short_enums.a   -fshort-enums
#
# The image is built with -fshort-enums when the BLE controller is linked (see
# the Bluetooth section of ../CMakeLists.txt) and enums cross these archives'
# interfaces, so the archive must match the image: a mismatch links cleanly
# and goes wrong at run time. IPRO_ZEPHYR_SHORT_ENUMS=ON forces the
# short-enum representation without the controller; tools/release uses it to
# build the second archive.

if(CONFIG_IPRO_BT_CTLR_PREBUILT OR IPRO_ZEPHYR_SHORT_ENUMS)
  set(IPRO_CORE_ARCHIVE_SUFFIX _short_enums)
else()
  set(IPRO_CORE_ARCHIVE_SUFFIX "")
endif()
if(IPRO_ZEPHYR_SHORT_ENUMS AND NOT CONFIG_IPRO_BT_CTLR_PREBUILT)
  zephyr_compile_options(-fshort-enums)
endif()

# ipro_core_sources_present(<out-var> <source>...)
# Sets <out-var> to TRUE when every source exists (the internal tree).
function(ipro_core_sources_present out)
  foreach(src ${ARGN})
    if(NOT EXISTS ${src})
      set(${out} FALSE PARENT_SCOPE)
      return()
    endif()
  endforeach()
  set(${out} TRUE PARENT_SCOPE)
endfunction()

# ipro_core_link_prebuilt(<name> <component dir>)
# Links the shipped archive whole: the source build is a Zephyr library, which
# Zephyr links whole too, so the image gets the same objects (shell commands
# that nothing references included).
#
# Flag and path go in ONE -Wl, item. Given as three items, CMake keeps the
# flags where they are but moves the archive path to the end of the link line
# (and drops a repeated -Wl,--whole-archive pair), so the archive ends up
# outside the whole-archive span and after the archives that need it.
function(ipro_core_link_prebuilt name dir)
  set(archive ${dir}/lib/zephyr/lib${name}${IPRO_CORE_ARCHIVE_SUFFIX}.a)
  if(NOT EXISTS ${archive})
    message(FATAL_ERROR "${name}: neither its sources nor ${archive} exist")
  endif()
  if(archive MATCHES ",")
    message(FATAL_ERROR "${name}: the archive path must not contain a comma: ${archive}")
  endif()
  zephyr_link_libraries("-Wl,--whole-archive,${archive},--no-whole-archive")
  message(STATUS "[ipro] ${name}: ${archive}")
endfunction()
