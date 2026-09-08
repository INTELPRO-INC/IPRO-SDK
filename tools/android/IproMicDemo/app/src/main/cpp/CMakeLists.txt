cmake_minimum_required(VERSION 3.22.1)
project("lc3decoder" C)

# LC3 library sources
set(LC3_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblc3/src)
add_library(lc3 STATIC
    ${LC3_SRC_DIR}/lc3.c
    ${LC3_SRC_DIR}/tables.c
    ${LC3_SRC_DIR}/mdct.c
    ${LC3_SRC_DIR}/spec.c
    ${LC3_SRC_DIR}/bwdet.c
    ${LC3_SRC_DIR}/sns.c
    ${LC3_SRC_DIR}/attdet.c
    ${LC3_SRC_DIR}/energy.c
    ${LC3_SRC_DIR}/bits.c
    ${LC3_SRC_DIR}/tns.c
    ${LC3_SRC_DIR}/ltpf.c
    ${LC3_SRC_DIR}/plc.c
)
target_include_directories(lc3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/liblc3/include
    ${CMAKE_CURRENT_SOURCE_DIR}/liblc3/src
)

# JNI bridge
add_library(lc3decoder SHARED lc3_jni.c)
target_link_libraries(lc3decoder lc3 log)

# 16KB page size alignment (required for Android 15+ / Google Play)
target_link_options(lc3decoder PRIVATE "-Wl,-z,max-page-size=16384")
target_include_directories(lc3decoder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/liblc3/include
)
