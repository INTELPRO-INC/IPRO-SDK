# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers/spi)
zephyr_library_sources(spi_ipro.c)
