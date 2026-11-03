# SPM of the M-mode PSA-RoT (CONFIG_PSA_SPE): trap entry, PMP/sPMP,
# peripheral table, virtual IRQ, ecall dispatch, iovec bounce, NSPE
# launch, Crypto, ITS, attestation, platform, firmware-update staging
# services, HUK and KDF.
__generate_library()
__component_add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/../include)
__component_add_include_dirs(.)

__component_add_sources(
    eat.c
    huk.c
    its_lfs.c
    its_mbedtls_adapter.c
    its_store.c
    kdf.c
    spm_copy.c
    spm_entry.S
    spm_harden.c
    spm_heap.c
    spm_nspe.c
    spm_periph.c
    spm_pmp.c
    spm_range.c
    spm_services.c
    spm_stack.c
    spm_trap.c
    spm_virq.c
    svc_attest.c
    svc_crypto.c
    svc_crypto_family.c
    svc_fwu.c
    svc_its.c
    svc_platform.c
)

# HUK provisioning image only (defconfig_ipro7_hukprov)
if(CONFIG_PSA_HUK_PROVISION)
    __component_add_sources(huk_prov.c huk_prov_hw.c)
endif()

if(CONFIG_PSA_TZC_ISOLATION)
    __component_add_sources(spm_tzc.c)
endif()

# NMSIS PMP accessors (core_feature_pmp.h) for spm_pmp.c
__component_add_private_compile_definitions(CFG_HAS_PMP CFG_PMP_ENTRY_NUM=8)
