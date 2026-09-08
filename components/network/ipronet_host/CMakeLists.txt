# ipronet_host: FreeRTOS host side of the ipronet SDIO-WiFi bridge (IPRO7 SDH
# host <-> IPRO6S ipro6_if_wifi device, SMID wire protocol). Public API is
# ipronet_host.h; ipronet_host_diag.h is bench access with no API promise.

__generate_library()

__component_add_include_dirs(.)

__component_add_sources(ipronet_host.c)
__component_add_sources(ipronet_sdio.c)
__component_add_sources(ipronet_ctrl.c)
__component_add_sources(ipronet_netif.c)
