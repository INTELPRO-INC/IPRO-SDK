# sdio_host_probe: shared IPRO7 SDIO host bring-up layer for module probe apps
# (ipro_sdio_probe / ipro_sdio_host_probe / ipro_atbm_probe). Public API is
# sdio_probe.h. The BUS half owns an SDIO_Host_Type and must stay off for
# modules whose vendor driver calls sdio_host_init() itself.

__generate_library()

__component_add_include_dirs(.)

__component_add_sources(sdio_probe_board.c)
__component_add_sources_ifdef(CONFIG_SDIO_HOST_PROBE_BUS   sdio_probe_bus.c)
__component_add_sources_ifdef(CONFIG_SDIO_HOST_PROBE_BUS   sdio_probe_dump.c)
__component_add_sources_ifdef(CONFIG_SDIO_HOST_PROBE_SHELL sdio_probe_shell.c)
