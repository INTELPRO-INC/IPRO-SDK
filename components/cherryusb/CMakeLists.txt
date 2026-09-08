__generate_library()

__component_add_include_dirs(common)
__component_add_include_dirs(osal)
__component_add_include_dirs(core)
__component_add_include_dirs(class/cdc)
__component_add_include_dirs(class/hid)
__component_add_include_dirs(class/msc)
__component_add_include_dirs(class/audio)
__component_add_include_dirs(class/video)
__component_add_include_dirs(class/hub)
__component_add_include_dirs(class/wireless)

__component_add_compile_definitions(-DCONFIG_CHERRYUSB)

if(CONFIG_CHERRYUSB_DEVICE)
__component_add_sources(core/usbd_core.c)

if(CONFIG_USB_HS AND CONFIG_CHIP STREQUAL "IPRO7")
message(FATAL_ERROR "IPRO7 USB do not support high speed")
endif()

if(CONFIG_CHERRYUSB_DEVICE_CDC)
__component_add_sources(class/cdc/usbd_cdc.c)
endif()
if(CONFIG_CHERRYUSB_DEVICE_HID)
__component_add_sources(class/hid/usbd_hid.c)
endif()
if(CONFIG_CHERRYUSB_DEVICE_MSC)
__component_add_sources(class/msc/usbd_msc.c)
endif()
if(CONFIG_CHERRYUSB_DEVICE_AUDIO)
__component_add_sources(class/audio/usbd_audio.c)
endif()
if(CONFIG_CHERRYUSB_DEVICE_VIDEO)
__component_add_sources(class/video/usbd_video.c)
endif()
if(CONFIG_CHERRYUSB_DEVICE_CDC_ECM)
__component_add_sources(class/cdc/usbd_cdc_ecm.c)
endif()
if(CONFIG_CHERRYUSB_DEVICE_CDC_NCM)
__component_add_sources(class/cdc/usbd_cdc_ncm.c)
endif()
if(CONFIG_CHERRYUSB_DEVICE_RNDIS)
__component_add_sources(class/wireless/usbd_rndis.c)
endif()

elseif(CONFIG_CHERRYUSB_HOST)
__component_add_sources(core/usbh_core.c)
__component_add_sources(port/ehci/usb_hc_ehci.c)
__component_add_sources(osal/usb_osal_freertos.c)
__component_add_sources(class/cdc/usbh_cdc_acm.c)
__component_add_sources(class/msc/usbh_msc.c)
__component_add_sources(class/hid/usbh_hid.c)
__component_add_sources(class/hub/usbh_hub.c)
__component_add_sources(class/video/usbh_video.c)
__component_add_sources(class/audio/usbh_audio.c)

__component_add_link_libraries(
-ucdc_acm_class_driver 
-uhid_class_driver 
-umsc_class_driver 
-uvideo_ctrl_class_driver 
-uaudio_ctrl_class_driver)

endif()
