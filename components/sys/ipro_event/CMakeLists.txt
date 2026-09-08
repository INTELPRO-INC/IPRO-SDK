# SPDX-FileCopyrightText: 2025 IPRO Technology Co., Ltd.
# SPDX-License-Identifier: Apache-2.0
#
# IPRO Event Loop Library

if(CONFIG_IPRO_EVENT)

__generate_library()

__component_add_include_dirs(include)

__component_add_sources(
    src/ipro_event.c
)

__component_add_compile_definitions(-DCONFIG_IPRO_EVENT)

endif()
