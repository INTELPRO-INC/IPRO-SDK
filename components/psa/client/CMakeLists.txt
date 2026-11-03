# NSPE PSA client library (CONFIG_PSA_CLIENT): psa/client.h over the
# U-mode ecall wrapper psa_ipro/ecall.h, PSA Crypto on the client-mode
# mbedtls headers (no mbedtls code), ITS, attestation, platform and
# firmware-update staging.
__generate_library()

# Exported: psa/*.h, the client-mode mbedtls headers and their config
__component_add_include_dirs(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR})
if(CONFIG_TF_PSA_CRYPTO)
    __component_add_include_dirs(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../crypto/tfpsa/tf-psa-crypto/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../crypto/tfpsa/tf-psa-crypto/drivers/builtin/include)
    __component_add_compile_definitions(
        TF_PSA_CRYPTO_CONFIG_FILE=\"psa_client_tfpsa_config.h\")
elseif(CONFIG_MBEDTLS_V3)
    __component_add_include_dirs(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../crypto/mbedtls3/mbedtls/include)
    __component_add_compile_definitions(
        MBEDTLS_CONFIG_FILE=\"psa_client_config3.h\")
else()
    __component_add_include_dirs(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../crypto/mbedtls/mbedtls/include)
    __component_add_compile_definitions(
        MBEDTLS_CONFIG_FILE=\"psa_client_config.h\")
endif()

__component_add_sources(
    psa_attest_client.c
    psa_client.c
    psa_crypto_client.c
    psa_crypto_client_asym.c
    psa_crypto_client_pake.c
    psa_crypto_client_sym.c
    psa_fwu_client.c
    psa_its_client.c
    psa_platform_client.c
)

# BSP drivers of the NSPE (drv_usb_v1.c, drv_dma.c) register and
# enable IRQs through the vIRQ table, not the M-mode ECLIC
if(CONFIG_PSA_NSPE_DMA_USB)
    __component_add_compile_definitions(IPRO_HAL_OS_IRQ_SEAM=1)
endif()
