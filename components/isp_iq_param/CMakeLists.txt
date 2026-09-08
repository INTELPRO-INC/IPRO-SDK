__generate_library()

__component_add_sources(sc101iot/img_param_sc101iot.c)

# Force the IQ descriptor out of the archive.
#
# The tables register themselves into the .img_param_desc linker section, and
# the linker script KEEPs that section - but KEEP only protects sections in
# objects that were already pulled in. Nothing references sc101iot_img_param by
# name, so the archive member is never extracted and the section comes out
# EMPTY: _img_param_desc_start == _img_param_desc_end. img_param_load() then
# walks a zero-length list, imgproc_bind_sensor_model() fails, and the ISP runs
# with no IQ parameters at all - "cannot match any IQ param to sensor model
# SC101IOT", followed by get_ae_metering_tbl spraying "invalid param idx".
#
# This is the same idiom components/sensor/CMakeLists.txt already uses for its
# camera descriptors. Apps that appeared to work (ipro7_uvc_cam,
# ipro_embed_common) each carry their own copy of the IQ table as an app source
# file, which links because app objects are not archive members - so this gap
# only bites apps that rely on the shared component.
__component_add_link_libraries(-usc101iot_img_param)

# Private includes (needed for compilation only)
__component_add_private_include_dirs(
    ${CMAKE_SOURCE_DIR}/components/isp_firmware/include
)
