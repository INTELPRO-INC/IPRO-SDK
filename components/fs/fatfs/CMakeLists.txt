__generate_library()

__component_add_include_dirs(. port)

__component_add_sources(
	diskio.c
	ff.c
	ffsystem.c
    ffunicode.c
    port/fatfs.c
)

# SDH SD card support - only on chips with SDH peripheral (IPRO7, IPRO6S)
if(CONFIG_CHIP STREQUAL "IPRO7" OR CONFIG_CHIP STREQUAL "IPRO6S")
    __component_add_sources(port/fatfs_sdh_sdcard.c)
    __component_add_compile_definitions(-DCONFIG_FATFS_SDH_SDCARD)
endif()

# PIO-based software SD host (A/B counterpart to the hardware SDH port)
if(CONFIG_HAL_PIO_SDH)
    __component_add_sources(port/fatfs_pio_sdh_sdcard.c)
endif()

# EVFS mode uses ipro_osal for mutex instead of aos_compat
if(CONFIG_EVFS AND CONFIG_EVFS_USE_FATFS)
    __component_add_private_include_dirs(${IPRO_SDK_BASE}/components/os/ipro_osal/include)
endif()
