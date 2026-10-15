__component_add_include_dirs(.)
__component_add_include_dirs(./include)
__component_add_include_dirs(./include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/core/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/ai/src/inst/include)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/fs/fatfs)
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/fs/fatfs/port)

# CDC 資料路徑 —— 只有 .config 開了 CherryUSB 才編。
if(CONFIG_CHERRYUSB_DEVICE_CDC)
    target_sources(app PRIVATE src/cdc_link.c)
endif()

# CDC 資料路徑 —— 只有 .config 開了 CherryUSB 才編。
if(CONFIG_CHERRYUSB_DEVICE_CDC)
    target_sources(app PRIVATE src/cdc_link.c)
endif()

target_compile_options(app PRIVATE -Werror)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
__set_main_file(main.c)
setup_project(ipro_ai_link)
