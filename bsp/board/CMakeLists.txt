if(NOT DEFINED CONFIG_CHIP)
  message(FATAL_ERROR "CONFIG_CHIP is not defined")
endif()

string(TOLOWER "${CONFIG_CHIP}" CONFIG_CHIP_LOWER)
set(CHIP_DIR "")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_CHIP}/CMakeLists.txt")
  set(CHIP_DIR "${CONFIG_CHIP}")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_CHIP_LOWER}/CMakeLists.txt")
  set(CHIP_DIR "${CONFIG_CHIP_LOWER}")
else()
  message(FATAL_ERROR "'${CONFIG_CHIP}' is not a valid chip name (no CMakeLists.txt found)")
endif()

add_subdirectory("${CHIP_DIR}")
