# USB CDC-ACM debug console (shared ipro6/ipro7). See usb_console.h.
if(CONFIG_DEBUG_CONSOLE_USB)

__generate_library()

__component_add_include_dirs(include)

__component_add_sources(usb_console.c)

# Strong usbd_event_handler dispatcher -- omit when the app already links one
# (usb_demo's usbd_event_common.c or the OTA shim) to avoid a duplicate symbol.
if(CONFIG_DEBUG_CONSOLE_USB_OWN_DISPATCHER)
    __component_add_sources(usb_console_event.c)
endif()

endif()
