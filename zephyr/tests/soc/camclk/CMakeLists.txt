# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ipro_camclk_probe)
target_sources(app PRIVATE src/main.c ${ZEPHYR_IPRO_SDK_MODULE_DIR}/bsp/drivers/ipro7_soc/std_drv/src/drv_mm_misc.c)
target_compile_options(app PRIVATE -Wno-error -w)
