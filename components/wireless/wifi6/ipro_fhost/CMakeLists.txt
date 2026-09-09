__generate_library(1)

__component_add_compile_options(-Wno-undef)
# Force inline for static inline functions to avoid undefined reference errors
__component_add_compile_options(-finline-functions)

# LP beacon-RX firmware (CONFIG_PHY_LPFW): see src/fhost_lpfw_stub.c — the real
# sources need the full UMAC task enum and the archive is never linked; only the
# include dirs must survive for wifi6_lwip_adapter's lwipopts.h.
if(CONFIG_PHY_LPFW)
__component_add_sources(src/fhost_lpfw_stub.c)
else()
__component_add_sources(src/fhost.c)
__component_add_sources(src/fhost_config.c)
__component_add_sources(src/fhost_cntrl.c)
__component_add_sources(src/fhost_rx.c)
__component_add_sources(src/fhost_tx.c)
__component_add_sources(src/fhost_wpa.c)
__component_add_sources(src/application/fhost_connect.c)
__component_add_sources(src/application/wifi_mgmr_cli.c)
__component_add_sources(src/application/wifi_mgmr_ext.c)
__component_add_sources(src/application/wifi_mgmr_pmk.c)
__component_add_sources(src/application/wifi_mgmr.c)
endif()

__component_add_include_dirs(include)
__component_add_private_include_dirs(src)
__component_add_private_include_dirs(src/application)
