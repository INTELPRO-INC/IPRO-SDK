# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
#
# Zephyr's samples/bluetooth/bap_broadcast_source built for the IPRO7 boards
# with the ISO (bt54_iso2) controller and LC3 enabled. The upstream sample's
# Kconfig only lets ENABLE_LC3 be set on a list of vendor SoCs; this wrapper
# reuses its sources with a Kconfig that allows any FPU-equipped SoC.
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(bap_broadcast_source_ipro)

set(UPSTREAM ${ZEPHYR_BASE}/samples/bluetooth/bap_broadcast_source)
target_sources(app PRIVATE ${UPSTREAM}/src/main.c)
if(CONFIG_BAP_BROADCAST_HIGH_RELIABILITY)
  set_source_files_properties(${UPSTREAM}/src/main.c PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/preset_high_reliability.h")
endif()
