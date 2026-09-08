# aos_compat removed - use ipro_osal instead
__component_add_subdirectory_ifdef(CONFIG_IPRO_EVENT ipro_event)
# CLI component removed - use Shell instead (CONFIG_SHELL=y)
__component_add_subdirectory_ifdef(CONFIG_SHELL shell)
__component_add_subdirectory_ifdef(CONFIG_DEBUG_CONSOLE_USB usb_console)
