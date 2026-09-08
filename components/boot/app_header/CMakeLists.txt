__generate_library()

__component_add_include_dirs(.)
__component_add_sources(app_header_verify.c)

# Default: pull CRC32 from softcrc. Boot2 builds can define
# BOOT_APP_HEADER_STANDALONE_CRC to drop the softcrc dependency.
if(CONFIG_BOOT_APP_HEADER_STANDALONE_CRC)
    __component_add_private_compile_definitions(APP_HEADER_STANDALONE_CRC)
else()
    __component_add_private_compile_definitions(APP_HEADER_USE_SOFTCRC)
endif()
