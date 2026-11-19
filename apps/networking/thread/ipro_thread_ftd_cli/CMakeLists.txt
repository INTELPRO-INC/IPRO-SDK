# IPRO Thread FTD CLI Application
# Full Thread Device with CLI interface for OpenThread

__component_add_include_dirs(.)
__component_add_include_dirs(./include)

# Add OpenThread includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/thread/openthread/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/thread/openthread/src/core)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/thread/openthread/examples/platforms)

# Add OpenThread port includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/thread/openthread_port/include)

# Add IPRO Log System includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/utils/include)

# Add CLI system includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/sys/cli/include)

# Add VFS includes
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/fs/vfs/include)

# Add AOS compatibility includes (provides yloop stubs)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/sys/aos_compat/include)

# Shared default dataset helper (factory-default Thread network params).
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/apps/networking/thread/common/include)

# Add application source files
# Note: main.c is NOT added to app library - it's only compiled for the ELF target
# via __set_main_file() to avoid duplicate symbol errors with --whole-archive
target_sources(app PRIVATE
    thread_ftd_app.c
    thread_ftd_cli.c
    udp_load.c
    udp_load_wire.c
    ${CMAKE_SOURCE_DIR}/apps/networking/thread/common/src/thread_default_dataset.c
)

# OpenThread integration is handled automatically by the thread component
# when CONFIG_THREAD_ENABLE=y is set in proj.conf
# IPRO Log System integration is handled by CONFIG_IPRO_LOG_ENABLE=y

# Set compiler options
target_compile_options(app PRIVATE -Werror)

# Set main file and project setup
set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
# Force whole-archive linking for app to include shell commands
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(main.c)
setup_project(ipro_thread_ftd_cli)
