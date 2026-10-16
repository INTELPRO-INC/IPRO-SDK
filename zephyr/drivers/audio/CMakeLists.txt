# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources(dmic_ipro.c)
zephyr_library_include_directories(${CMAKE_CURRENT_LIST_DIR}/../../include/ipro_freertos_shim)
