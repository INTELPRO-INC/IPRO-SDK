__generate_library()

__component_add_include_dirs(include)

# Our headers (crc32.h, crc16.h, ...) collide by name with other components'
# (e.g. wpa_supplicant's src/utils/crc32.h) on the shared global include path.
# Add include/ as PRIVATE too so our own sources resolve their own headers
# first (target-private dirs are searched before linked INTERFACE dirs).
__component_add_private_include_dirs(include)

__component_add_sources(
	src/crc16.c
	src/crc32.c
	src/zheaders.c
	src/znumbers.c
	src/zserial.c
	src/port.c
)

#__component_add_compile_definitions(-DZDEBUG -DZTRACE)
