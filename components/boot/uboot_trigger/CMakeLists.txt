__generate_library()

__component_add_include_dirs(.)
__component_add_sources(uboot_trigger.c)

# When CONFIG_BOOT_UBOOT_TRIGGER_AUTO_CDC_HOOKS=y the component installs
# non-weak usbd_cdc_acm_set_{line_coding,dtr} overrides. These must land
# in the final ELF even if no app code references them directly.
if (CONFIG_BOOT_UBOOT_TRIGGER_AUTO_CDC_HOOKS)
    __component_add_private_compile_definitions(CONFIG_BOOT_UBOOT_TRIGGER_AUTO_CDC_HOOKS)
    set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS uboot_trigger)
endif()
