__generate_library()

__component_add_include_dirs(.)

list(APPEND ADD_SRCS  xz_crc32.c xz_dec_lzma2.c xz_dec_stream.c xz_decompress.c xz_port.c)
__component_add_sources(${ADD_SRCS})
