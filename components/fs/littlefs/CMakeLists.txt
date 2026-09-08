__generate_library(0)

__component_add_sources(littlefs/lfs_util.c)
__component_add_sources(littlefs/lfs.c)
__component_add_sources(port/lfs_xip_flash.c)

# CLI commands (optional, requires shell)
if(CONFIG_LITTLEFS_CLI AND CONFIG_SHELL)
    __component_add_sources(port/lfs_cli.c)
endif()

__component_add_include_dirs(littlefs)
__component_add_include_dirs(port)

# Use autoconf.h for all CONFIG_* definitions
# No manual compile definitions needed - autoconf.h is included globally

if(CONFIG_FREERTOS)
    __component_add_compile_definitions(-DLFS_THREADSAFE)
endif()

# Force whole-archive linking to include shell commands
if(CONFIG_LITTLEFS_CLI AND CONFIG_SHELL)
    __component_whole_archive()
endif()
