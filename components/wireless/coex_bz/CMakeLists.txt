# BLE / 802.15.4 PTA arbiter on IPRO7 (coex_bz.h).
if(CONFIG_COEX_BZ)
__generate_library()
__component_add_include_dirs(include)
__component_add_sources(src/coex_bz.c)
# the shell command is the only reference to parts of this object
__component_whole_archive()
endif()
