# WiFi/BT coexistence (PTA arbiter).
#
# Ported from bl_iot_sdk components/network/wifi_bt_coex. Same shape: public
# API + per-chip register backend + shared timing context + console command.
# It replaces three independent sets of hardcoded PTA magic numbers that used
# to live in macsw's hal_machw.c, the wifi6 CLI, and the 802.15.4 HAL.

## Public include paths exported to project level
__component_add_include_dirs(include)

__generate_library()

__component_add_sources(
    src/ipro_coex.c
    src/ipro_coex_ctx.c
    src/ipro_coex_cli.c
)

if(CONFIG_IPRO6)
    __component_add_sources(src/ipro_coex_impl_ipro6.c)
endif()

# Nothing outside the module references the CLI translation unit, so without
# this the linker never pulls it out of the archive and the `coex` command
# silently disappears from the shell. FSymTab entries only survive if the
# object makes it into the link.
__component_whole_archive()
