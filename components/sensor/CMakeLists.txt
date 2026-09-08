__generate_library()

__component_add_include_dirs(.)
__component_add_sources(camera.c)
__component_add_sources(sensor_utils.c)
__component_add_sources(sc101iot.c)
__component_add_sources(sccb.c)
#__component_add_sources(bf30a2.c)
__component_add_sources(gc6133.c)
__component_add_sources(gc032a.c)

__component_add_link_libraries(
-u__rt_camera_gc6133_desc
-u__rt_camera_sc101iot_desc
-u__rt_camera_gc032a_desc
-u_camera_desc_start
-u_camera_desc_end)
