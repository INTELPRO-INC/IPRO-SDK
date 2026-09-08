__generate_library(0)

__component_add_sources(src/easyflash.c)
__component_add_sources(src/ef_env.c)
__component_add_sources(src/ef_port.c)
__component_add_sources(src/ef_utils.c)

# CLI commands (optional)
__component_add_sources_ifdef(CONFIG_EASYFLASH4_CLI src/easyflash_cli.c)

__component_add_include_dirs(inc)

# Force whole-archive linking to include static CLI commands
if(CONFIG_EASYFLASH4_CLI)
    __component_whole_archive()
endif()
