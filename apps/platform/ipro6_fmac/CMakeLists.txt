# IPRO6S FullMAC: one app, two images picked by the defconfig.
#   defconfig_ipro6_fmac_loader: loader, XIP code with RAM in WRAM.
#     Enumerates SDIO and USB, takes the FW into OCRAM and jumps to it.
#   defconfig_ipro6_fmac (CONFIG_IPRO6_FMAC): FullMAC firmware,
#     OCRAM-linked, downloaded by the host and started by the loader.
# Host API headers (fmac_dl.h, fmac_wire.h, fmac_msg.h): beside macif_bus.c
# dl/: loader side of fmac_dl.h, without RTOS or SDK use
# bus/: SDIO and USB device transports, loader and FW
# loader/, fw/: the two images; tools/: host-side image tools
__component_add_include_dirs(${CMAKE_SOURCE_DIR}/components/wireless/macsw/modules/macif/src)
__component_add_include_dirs(dl)
__component_add_include_dirs(bus)
target_compile_options(app PRIVATE -Werror)

if(CONFIG_IPRO6_FMAC)
    __component_add_include_dirs(fw)
    __component_add_include_dirs(fw/fhost_shim)

    # One bus at a time: SDIO and USB share fmac_bus_pool. Host -> FW
    # transfers up to FMAC_XFER_MAX, FW -> host up to FMAC_UPLD_MAX (fmac_wire.h)
    target_compile_definitions(app PRIVATE FMAC_BUS_POOL=1
        FMAC_SMID_NBUF=4 FMAC_USB_NBUF=4
        FMAC_SMID_UPLD_MAX=9216 FMAC_SMID_UPLD_NQ=2 FMAC_USB_IN_MAX=9212
        FMAC_USB_IN_NQ=2)
    target_sources(app PRIVATE
        fw/app_support.c
        fw/fmac_fhost.c
        fw/fmac_host.c
        fw/fmac_platform.c
    )
    set(FMAC_IMAGE ipro6_fmac_fw)
    set(FMAC_MAIN fw/main.c)
else()
    __component_add_include_dirs(loader)
    target_compile_options(app PRIVATE -DFMAC_USB_NBUF=1)
    target_sources(app PRIVATE
        loader/app_support.c
        dl/fmac_dl_loader.c
    )
    set(FMAC_IMAGE ipro6_fmac_loader)
    set(FMAC_MAIN loader/main.c)
endif()

target_sources(app PRIVATE
    dl/fmac_crc32.c
    bus/fmac_smid.c
    bus/fmac_usb.c
)

set_property(GLOBAL APPEND PROPERTY SDK_LIBS app)
set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS app)
__set_main_file(${FMAC_MAIN})
setup_project(${FMAC_IMAGE})

if(CONFIG_IPRO6_FMAC)
    add_custom_command(TARGET ${proj_name}.elf POST_BUILD
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tools/mkimg.py
                $<TARGET_FILE:${proj_name}.elf> ${build_dir}/ipro6s_fmac.img
        VERBATIM)
endif()
