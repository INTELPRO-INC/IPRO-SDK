# SPDX-FileCopyrightText: 2026 IPRO Technology Co., Ltd.
# SPDX-License-Identifier: Apache-2.0
#
# IPRO power management

if(CONFIG_IPRO_PM)

__generate_library()

__component_add_include_dirs(include)

__component_add_sources(
    src/pm_core.c
    src/pm_freertos.c
    src/pm_arbiter.c
)

# raw per-app .config builds do not resolve Kconfig dependencies
if(CONFIG_IPRO_PM_BLE AND CONFIG_IPRO7 AND CONFIG_BT_CTLR_IPRO)
    __component_add_sources(src/pm_ble_ipro7.c)
    __component_add_private_compile_definitions(-DIPRO_PM_HAS_BLE=1)
endif()

if(CONFIG_IPRO7 AND (CONFIG_IPRO_PM_TIMEBASE OR CONFIG_IPRO_PM_IDLE_PDS))
    __component_add_sources(src/pm_lpclk.c)
endif()
if(CONFIG_IPRO7 AND CONFIG_IPRO_PM_IDLE_PDS)
    __component_add_sources(src/pm_exec_idle.c)
endif()

if(CONFIG_IPRO_PM_SHELL)
    __component_add_sources(src/pm_shell.c)
endif()

endif()
