# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers/adc)
zephyr_library_sources(adc_ipro.c)
