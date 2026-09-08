# SPDX-FileCopyrightText: 2025 IPRO Technology Co., Ltd.
# SPDX-License-Identifier: Apache-2.0
#
# IPRO OS Abstraction Layer (OSAL)
# Provides unified API for OS primitives, abstracting FreeRTOS.

__generate_library(0)

__component_add_include_dirs(include)

__component_add_sources(
    ipro_osal_freertos.c
)
