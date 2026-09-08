#======================================================================
# Copyright (c) 2026 IntelPro Technology Co., Ltd.
# All rights reserved.
#
# This software and associated documentation files (the "Software") are
# proprietary and confidential to IntelPro Technology Co., Ltd..
#
# Access to and use of this Software is limited exclusively to authorized 
# customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
# valid license or non-disclosure agreement.
#
# Unauthorized copying, modification, distribution, or reverse engineering 
# of this Software, via any medium, is strictly prohibited without 
# prior written permission from IntelPro Technology Co., Ltd..
#
# SPDX-License-Identifier: Proprietary
#======================================================================

macro(__generate_library)
  if(${ARGC} GREATER_EQUAL 2)
    set(__component_lib ${ARGV1})
  else()
    get_filename_component(__component_lib ${CMAKE_CURRENT_LIST_DIR} NAME)
  endif()

  if("${CONFIG_BUILD_ERROR_ON_WARN}" STREQUAL "y")
    if(${ARGC} GREATER_EQUAL 1)
      set(__LIB_ALLOW_WARNING ${ARGV0})
    else()
      set(__LIB_ALLOW_WARNING 0)
    endif()
  else()
      set(__LIB_ALLOW_WARNING 1)
  endif()
  message(STATUS "[register library : ${__component_lib}], path:${CMAKE_CURRENT_LIST_DIR}, allow warning: ${__LIB_ALLOW_WARNING}")

  set(CURRENT_STATIC_LIBRARY ${__component_lib})
  add_library(${__component_lib} STATIC)
  set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${__component_lib})
  target_link_libraries(${__component_lib} PUBLIC __component_lib)
  if (NOT ${__LIB_ALLOW_WARNING})
    target_compile_options(${CURRENT_STATIC_LIBRARY} PRIVATE -Werror)
  endif()
endmacro()

# Mark current library for whole-archive linking
# Use this for libraries with static CLI commands or other symbols that need forced inclusion
function(__component_whole_archive)
  set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS ${CURRENT_STATIC_LIBRARY})
endfunction()

function(__component_add_sources)
  foreach(arg ${ARGV})
    if(IS_DIRECTORY ${arg})
    message(FATAL_ERROR "__component_add_sources() was called on a directory")
    endif()

    if(IS_ABSOLUTE ${arg})
    set(path ${arg})
    else()
    set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()
    target_sources(${CURRENT_STATIC_LIBRARY} PRIVATE ${path})
  endforeach()
endfunction()

function(__component_add_sources_ifdef feature)
  if(${${feature}})
  __component_add_sources(${ARGN})
  endif()
endfunction()

function(__component_add_include_dirs)
  foreach(arg ${ARGV})
    if(IS_ABSOLUTE ${arg})
      set(path ${arg})
    else()
      set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()
    target_include_directories(__component_lib INTERFACE ${path})
  endforeach()
endfunction()

function(__component_add_private_include_dirs)
  foreach(arg ${ARGV})
    if(IS_ABSOLUTE ${arg})
      set(path ${arg})
    else()
      set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()
    target_include_directories(${CURRENT_STATIC_LIBRARY} PRIVATE ${path})
  endforeach()
endfunction()

function(__component_add_include_dirs_ifdef feature)
  if(${${feature}})
  __component_add_include_dirs(${ARGN})
  endif()
endfunction()

function(__component_add_private_include_dirs_ifdef feature)
  if(${${feature}})
  __component_add_private_include_dirs(${ARGN})
  endif()
endfunction()

function(__component_add_compile_definitions)
  target_compile_definitions(__component_lib INTERFACE ${ARGV})
endfunction()

function(__component_add_private_compile_definitions)
  target_compile_definitions(${CURRENT_STATIC_LIBRARY} PRIVATE ${ARGV})
endfunction()

function(__component_add_compile_definitions_ifdef feature)
  if(${${feature}})
  __component_add_compile_definitions(${ARGN})
  endif()
endfunction()

function(__component_add_compile_definitions_ifndef feature)
  if(NOT ${${feature}})
  __component_add_compile_definitions(${ARGN})
  endif()
endfunction()

function(__component_add_private_compile_definitions_ifdef feature)
  if(${${feature}})
    __component_add_private_compile_definitions(${ARGN})
  endif()
endfunction()

function(__component_add_compile_options)
  target_compile_options(__component_lib INTERFACE ${ARGV})
endfunction()

function(__component_add_private_compile_options)
  target_compile_options(${CURRENT_STATIC_LIBRARY} PRIVATE ${ARGV})
endfunction()

function(__component_add_compile_options_ifdef feature)
  if(${${feature}})
  __component_add_compile_options(${ARGN})
  endif()
endfunction()

function(__component_add_private_compile_options_ifdef feature)
  if(${${feature}})
  __component_add_private_compile_options(${ARGN})
  endif()
endfunction()

function(__component_add_link_options)
  target_link_options(__component_lib INTERFACE ${ARGV})
endfunction()

function(__component_add_private_link_options)
  target_link_options(${CURRENT_STATIC_LIBRARY} PRIVATE ${ARGV})
endfunction()

function(__component_add_link_options_ifdef feature)
  if(${${feature}})
  __component_add_link_options(${ARGN})
  endif()
endfunction()

function(__component_add_private_link_options_ifdef feature)
  if(${${feature}})
    __component_add_private_link_options(${ARGN})
  endif()
endfunction()

function(__component_add_link_libraries)
  target_link_libraries(__component_lib INTERFACE ${ARGV})
endfunction()

function(__component_add_link_libraries_ifdef feature)
  if(${${feature}})
  __component_add_link_libraries(${ARGN})
  endif()
endfunction()

function(__component_add_subdirectory_ifdef feature dir)
  if(${${feature}})
    add_subdirectory(${dir})
  endif()
endfunction()

function(__component_add_static_library)
  foreach(arg ${ARGV})
  if(IS_DIRECTORY ${arg})
  message(FATAL_ERROR "__component_add_static_library() was called on a directory")
  endif()

  if(IS_ABSOLUTE ${arg})
  set(path ${arg})
  else()
  set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
  endif()
  get_filename_component(__component_lib ${path} NAME_WE)
  message(STATUS "[register extern library : ${__component_lib}], path:${CMAKE_CURRENT_LIST_DIR}")
  set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${path})
  endforeach()
endfunction()

function(__component_add_static_library_ifdef feature)
  if(${${feature}})
    __component_add_static_library(${ARGN})
  endif()
endfunction()

macro(__component_default_def define val)
  if(NOT DEFINED ${define})
    set(${define} ${val})
  endif()
endmacro()

function(__set_linker_script ld)
  if(IS_ABSOLUTE ${ld})
  set(path ${ld})
  else()
  set(path ${CMAKE_CURRENT_SOURCE_DIR}/${ld})
  endif()
  set_property(GLOBAL PROPERTY LINKER_SCRIPT ${path})
  message(STATUS "Linker Script: ${path}")
endfunction()

macro(__set_main_file)
    if(IS_ABSOLUTE ${ARGV0})
    set(path ${ARGV0})
    else()
    set(path ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0})
    endif()
  set(CURRENT_MAIN_FILE ${path})
endmacro()

macro(setup_project name)

  set(proj_name ${name}_${CONFIG_CHIP})

  set(HEX_FILE ${build_dir}/${proj_name}.hex)
  set(BIN_FILE ${build_dir}/${proj_name}.bin)
  set(MAP_FILE ${build_dir}/${proj_name}.map)
  set(ASM_FILE ${build_dir}/${proj_name}.asm)

  add_executable(${proj_name}.elf ${CURRENT_MAIN_FILE})
  target_link_libraries(${proj_name}.elf __component_lib)
  get_property(LINKER_SCRIPT_PROPERTY GLOBAL PROPERTY LINKER_SCRIPT)
  if(EXISTS ${LINKER_SCRIPT_PROPERTY})
    set_target_properties(${proj_name}.elf PROPERTIES LINK_FLAGS "-T${LINKER_SCRIPT_PROPERTY} -Wl,-Map=${MAP_FILE}")
    set_target_properties(${proj_name}.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT_PROPERTY})
  endif()

  get_property(SDK_LIBS_PROPERTY GLOBAL PROPERTY SDK_LIBS)
  get_property(SDK_WHOLE_ARCHIVE_LIBS_PROPERTY GLOBAL PROPERTY SDK_WHOLE_ARCHIVE_LIBS)

  # Build link command with whole-archive libraries
  if(SDK_WHOLE_ARCHIVE_LIBS_PROPERTY)
    target_link_libraries(${proj_name}.elf
      -Wl,--whole-archive ${SDK_WHOLE_ARCHIVE_LIBS_PROPERTY} -Wl,--no-whole-archive
      -Wl,--start-group ${SDK_LIBS_PROPERTY} app -Wl,--end-group)
  else()
    target_link_libraries(${proj_name}.elf -Wl,--start-group ${SDK_LIBS_PROPERTY} app -Wl,--end-group)
  endif()

  if(OUTPUT_DIR)
  add_custom_command(TARGET ${proj_name}.elf POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${proj_name}.elf> ${BIN_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${proj_name}.elf> ${OUTPUT_DIR}/${name}/${proj_name}.elf
  COMMAND ${CMAKE_COMMAND} -E copy ${MAP_FILE} ${OUTPUT_DIR}/${name}/${proj_name}.map
  COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} ${OUTPUT_DIR}/${name}/${proj_name}.bin
  COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} ${OUTPUT_DIR}/project.bin
  COMMENT "Generate ${BIN_FILE}\r\n"
  )
  if(CONFIG_GENERATE_ASM)
  add_custom_command(TARGET ${proj_name}.elf POST_BUILD
  COMMAND ${CMAKE_OBJDUMP} -d -S $<TARGET_FILE:${proj_name}.elf> >${ASM_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy ${ASM_FILE} ${OUTPUT_DIR}/${name}/${proj_name}.asm
  COMMENT "Generate ${ASM_FILE}\r\n"
  )
  endif()

  else()
  add_custom_command(TARGET ${proj_name}.elf POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${proj_name}.elf> ${BIN_FILE}
  COMMENT "Generate ${BIN_FILE}\r\n"
  )
  if(CONFIG_GENERATE_ASM)
  add_custom_command(TARGET ${proj_name}.elf POST_BUILD
  COMMAND ${CMAKE_OBJDUMP} -d -S $<TARGET_FILE:${proj_name}.elf> >${ASM_FILE}
  COMMENT "Generate ${ASM_FILE}\r\n"
  )
  endif()
  endif()
endmacro()
macro(sdk_ifndef define val)
  if(NOT DEFINED ${define})
    set(${define} ${val})
  endif()
endmacro()

function(config_parse config_file)
    file(READ "${config_file}" file_contents)
    string(REPLACE "\n" ";" file_lines "${file_contents}")

    foreach(line IN LISTS file_lines)
        string(STRIP "${line}" line)
        if(line MATCHES "^#.*" OR line STREQUAL "")
            continue()
        endif()
        if(line MATCHES "^# CONFIG_[A-Za-z0-9_]+ is not set")
            string(REGEX REPLACE "^# (CONFIG_[A-Za-z0-9_]+) is not set" "\\1" var_name "${line}")
            # Set CMake variable to 0 for disabled options (needed for linker script generation)
            set(${var_name} 0 PARENT_SCOPE)
            # DO NOT define the macro in C code - leave it undefined
            # This matches the old behavior and allows BLE stack to use defined() checks correctly
            #__component_add_compile_options(-D${var_name}=0)
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=y")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=y" "\\1" var_name "${line}")
            set(${var_name} 1 PARENT_SCOPE)
            __component_add_compile_options(-D${var_name}=1)
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=\".*\"")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=\"(.*)\"" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=\"(.*)\"" "\\2" var_value "${line}")
            set(${var_name} "${var_value}" PARENT_SCOPE)
            __component_add_compile_options(-D${var_name}="${var_value}")
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=n$")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=n$" "\\1" var_name "${line}")
            # Set CMake variable to 0 for disabled options (needed for linker script generation)
            set(${var_name} 0 PARENT_SCOPE)
            # DO NOT define the macro in C code - leave it undefined
            # This matches the old behavior and allows BLE stack to use defined() checks correctly
            #__component_add_compile_options(-D${var_name}=0)
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=[0-9]+")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([0-9]+)" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([0-9]+)" "\\2" var_value "${line}")
            set(${var_name} "${var_value}" PARENT_SCOPE)
            __component_add_compile_options(-D${var_name}=${var_value})
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=[A-Za-z0-9_]+")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([A-Za-z0-9_]+)" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([A-Za-z0-9_]+)" "\\2" var_value "${line}")
            set(${var_name} "${var_value}" PARENT_SCOPE)
            __component_add_compile_options(-D${var_name}=${var_value})
            continue()
        endif()
    endforeach()
endfunction()
