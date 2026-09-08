__generate_library()

__component_add_include_dirs(.)

# Add semihosting include path and source if enabled
if(CONFIG_SEMIHOSTING_PRINTF)
    __component_add_include_dirs(../semihosting)
    __component_add_sources(../semihosting/semihost_printf.c)
endif()

if(CONFIG_SEMIHOSTING_FILEIO)
    __component_add_include_dirs(../semihosting)
    __component_add_sources(../semihosting/semihost_fileio.c)
endif()

#__component_add_sources(syscalls.c assert.c debug.c)
__component_add_sources(debug.c)
__component_add_sources_ifdef(CONFIG_FREERTOS_SUPPORT syscalls.c)

# Boot2 (no RTOS) has no float format strings; disable float path to save ~2KB flash
__component_add_compile_definitions_ifdef(CONFIG_NO_RTOS DISABLE_PRINT_FLOAT)

__component_add_link_libraries(
-unewlibc_syscall_init
-unewlibc_debug_init
)
