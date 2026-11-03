# ipro_sec: SEC_ENG PSA transparent accelerator driver of the SPE
# (CONFIG_PSA_SEC_ENG_ACCEL), on the SEC_ENG HAL v2. The driver
# wrappers of components/crypto/{mbedtls3,tfpsa} call it.
__generate_library()
__component_add_include_dirs(.)
__component_add_sources(
    ipro_sec_aead.c
    ipro_sec_cipher.c
    ipro_sec_ecc.c
    ipro_sec_entropy.c
    ipro_sec_hash.c
)
