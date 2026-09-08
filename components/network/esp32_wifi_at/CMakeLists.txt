__generate_library()

__component_add_include_dirs(include)

__component_add_sources(
    src/esp32_wifi_at.c
    src/esp32_wifi_hw.c
)

# Force whole-archive linking so SHELL_CMD_EXPORT_ALIAS entries are kept
__component_whole_archive()
