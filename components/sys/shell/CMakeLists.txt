# Shell component CMakeLists.txt
# Based on RT-Thread FinSH shell (Apache-2.0 License)

if(CONFIG_SHELL)

__generate_library()

__component_add_include_dirs(.)

__component_add_sources(
    shell.c
    ring_buffer.c
    shell_utils.c
)

if(CONFIG_FREERTOS)
    __component_add_sources(shell_freertos.c)
endif()

# Filesystem commands (ls, cat, hexdump, pwd, cd, mkdir, rm) - requires EVFS and IPRO_OSAL
if(CONFIG_EVFS AND CONFIG_EVFS_POSIX AND CONFIG_IPRO_OSAL)
    __component_add_sources(shell_vfs.c)
    __component_add_private_include_dirs(${IPRO_SDK_BASE}/components/os/ipro_osal/include)
endif()

# Command Scheduler - requires ipro_event
if(CONFIG_CMD_SCHEDULER AND CONFIG_IPRO_EVENT)
    __component_add_sources(cmd_scheduler/cmd_scheduler.c)
    __component_add_include_dirs(cmd_scheduler)
endif()

# PHY/RF shell commands (IPRO6 only, not IPRO6LE) - requires ipro6_rf driver
if((CONFIG_WIFI6 OR CONFIG_PHY_CLI) AND CONFIG_IPRO6 AND NOT CONFIG_IPRO6LE)
    __component_add_sources(${IPRO_SDK_BASE}/components/wireless/rf/ipro6_rf/cli/shell_phy.c)
    __component_add_include_dirs(${IPRO_SDK_BASE}/components/wireless/rf/ipro6_rf/cli)
endif()

__component_add_compile_definitions(-DCONFIG_SHELL)

if(CONFIG_SHELL_EXEC_THREAD)
    __component_add_compile_definitions(-DCONFIG_SHELL_EXEC_THREAD=1)
endif()

# Force whole-archive linking to include shell commands
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS shell)

endif()
