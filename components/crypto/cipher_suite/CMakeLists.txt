__generate_library()

################# Add global include #################
__component_add_include_dirs(include)
#######################################################

############## Add current dir source files ###########
__component_add_sources(src/ipro_crypt.c)
__component_add_sources(src/ipro_dsa.c)
__component_add_sources(src/ipro_ecdsa.c)
__component_add_sources(src/ipro_hash.c)
__component_add_sources(src/ipro_rsa.c)


