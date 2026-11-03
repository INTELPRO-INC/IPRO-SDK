/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/*
 * SPE platform service and the identity of this boot. The eFuse and
 * SPE flash reads are host-test stand-ins under SPM_HOST.
 */
#include <string.h>

#include "huk.h"
#include "kdf.h"
#include "mbedtls/platform_util.h"
#include "spm_sha256.h"
#include "psa/crypto.h"
#include "psa_ipro/memmap.h"
#include "psa_ipro/platform_rpc.h"
#include "spm_log.h"
#include "spm_nspe.h"
#include "spm_virq.h"
#include "svc_platform.h"

#ifdef SPM_HOST
/* Stand-ins defined by the host test. */
int plat_hw_uid(uint8_t uid[8]);
uint32_t plat_hw_lifecycle(void);
void plat_hw_pk_hash(uint32_t pk[8]);
const uint8_t *plat_hw_spe_image(size_t *len);
#else
#include "drv_ef_ctrl.h"
#include "ef_data_0_reg.h"

/* LMA end of the last loaded section, ipro7_psa_spe.ld */
extern const uint8_t __spe_image_end[];

/* eFuse chip id; 0 when its zero-bit parity matches */
static int plat_hw_uid(uint8_t uid[8])
{
    return EF_Ctrl_Read_Chip_ID(uid);
}

/* ef_sec_lifecycle, bits 3:0 of eFuse word 0x7C */
static uint32_t plat_hw_lifecycle(void)
{
    uint32_t w = 0;

    EF_Ctrl_Read_Direct_R0(EF_DATA_0_LOCK_OFFSET / 4u, &w, 1);
    return (w & EF_DATA_0_EF_SEC_LIFECYCLE_MSK) >>
           EF_DATA_0_EF_SEC_LIFECYCLE_POS;
}

/* boot2 public key hash: eFuse key slots 0 and 1 */
static void plat_hw_pk_hash(uint32_t pk[8])
{
    EF_Ctrl_Read_AES_Key(0, pk, 4);
    EF_Ctrl_Read_AES_Key(1, pk + 4, 4);
}

static const uint8_t *plat_hw_spe_image(size_t *len)
{
    *len = (size_t)((uintptr_t)__spe_image_end - PSA_SPE_FLASH_BASE);
    return (const uint8_t *)(uintptr_t)PSA_SPE_FLASH_BASE;
}
#endif

/* IAK signatures: RFC 6979 nonces, no TRNG output in k */
#define PLAT_ECDSA PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256)

/* ef_sec_lifecycle -> PSA lifecycle; unlisted values are UNKNOWN */
static const uint16_t plat_lifecycle[16] = {
    [0] = PSA_LIFECYCLE_ASSEMBLY_AND_TEST,
    [1] = PSA_LIFECYCLE_PSA_ROT_PROVISIONING,
    [7] = PSA_LIFECYCLE_SECURED,
    [15] = PSA_LIFECYCLE_DECOMMISSIONED,
};

/* IAK seeds in order; the next one when a scalar is out of range */
static const char *const plat_iak_labels[] = {
    KDF_LABEL_IAK, KDF_LABEL_IAK2, KDF_LABEL_IAK3,
};
#define PLAT_IAK_SEEDS (sizeof(plat_iak_labels) / sizeof(plat_iak_labels[0]))

/* out size of each fn and its status while the source is missing */
static const struct {
    uint8_t size;
    psa_status_t missing;
} plat_fns[PLATFORM_FN_GET_STATS + 1] = {
    [PLATFORM_FN_GET_UID] = { PLATFORM_UID_SIZE,
                              PSA_ERROR_SERVICE_FAILURE },
    [PLATFORM_FN_GET_LIFECYCLE] = { sizeof(uint32_t),
                                    PSA_ERROR_SERVICE_FAILURE },
    [PLATFORM_FN_GET_IAK_PUB] = { PLATFORM_IAK_PUB_SIZE,
                                  PSA_ERROR_SERVICE_FAILURE },
    [PLATFORM_FN_GET_NSPE_MEASUREMENT] = { PLATFORM_MEASUREMENT_SIZE,
                                           PSA_ERROR_SERVICE_FAILURE },
    [PLATFORM_FN_GET_STATS] = { sizeof(struct psa_nspe_ctrl),
                                PSA_ERROR_BAD_STATE },
};

static struct {
    struct svc_platform_id id;
    mbedtls_svc_key_id_t iak;
    int ready;
} plat;

static psa_status_t plat_sha256(const void *p, size_t n, uint8_t out[32])
{
    return mbedtls_sha256_ret(p, n, out, 0) == 0 ? PSA_SUCCESS
                                                  : PSA_ERROR_GENERIC_ERROR;
}

static void plat_hex(const uint8_t *p, size_t n, char *s)
{
    static const char digits[] = "0123456789abcdef";

    for (size_t i = 0; i < n; i++) {
        s[2u * i] = digits[p[i] >> 4];
        s[2u * i + 1u] = digits[p[i] & 15u];
    }
    s[2u * n] = '\0';
}

static psa_status_t plat_iak_import(void)
{
    psa_key_attributes_t a = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t st = PSA_ERROR_INVALID_ARGUMENT;
    uint8_t d[32];

    psa_set_key_type(&a, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&a, 256);
    psa_set_key_usage_flags(&a, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&a, PLAT_ECDSA);
    for (size_t i = 0; st == PSA_ERROR_INVALID_ARGUMENT &&
                       i < PLAT_IAK_SEEDS; i++) {
        st = kdf_derive(plat_iak_labels[i], d, sizeof(d)) == 0
                 ? psa_import_key(&a, d, sizeof(d), &plat.iak)
                 : PSA_ERROR_HARDWARE_FAILURE;
    }
    mbedtls_platform_zeroize(d, sizeof(d));
    if (st == PSA_ERROR_INVALID_ARGUMENT) {
        SPM_LOG("no IAK seed gives a P-256 scalar");
        spm_fatal();
    }
    return st;
}

/*
 * IAK: volatile P-256 key pair imported by the SPE; an NSPE crypto
 * call that names it gets PSA_ERROR_INVALID_HANDLE. Fills the public
 * key and the instance id.
 */
static psa_status_t plat_iak_init(struct svc_platform_id *id)
{
    size_t n = 0;
    psa_status_t st = plat_iak_import();

    if (st == PSA_SUCCESS) {
        st = psa_export_public_key(plat.iak, id->iak_pub,
                                   sizeof(id->iak_pub), &n);
    }
    if (st == PSA_SUCCESS && n != sizeof(id->iak_pub)) {
        st = PSA_ERROR_GENERIC_ERROR;
    }
    if (st == PSA_SUCCESS) {
        id->instance_id[0] = 0x01;
        st = plat_sha256(id->iak_pub, n, &id->instance_id[1]);
    }
    return st;
}

psa_status_t svc_platform_init(void)
{
    struct svc_platform_id *id = &plat.id;
    uint32_t pk[8];
    const uint8_t *spe;
    size_t spe_len = 0;
    psa_status_t st;

    plat.ready = 0;
    psa_destroy_key(plat.iak);
    plat.iak = 0;
    memset(id, 0, sizeof(*id));
    if (plat_hw_uid(id->uid) != 0) {
        SPM_LOG("UID parity mismatch");
    }
    id->lifecycle = huk_is_dev() ? PSA_LIFECYCLE_UNKNOWN
                                 : plat_lifecycle[plat_hw_lifecycle() & 15u];
    plat_hw_pk_hash(pk);
    plat_hex((const uint8_t *)pk, sizeof(pk), id->signer_hex);
    spe = plat_hw_spe_image(&spe_len);
    st = plat_sha256(pk, sizeof(pk), id->impl_id);
    if (st == PSA_SUCCESS) {
        st = plat_sha256(spe, spe_len, id->spe_hash);
    }
    if (st == PSA_SUCCESS) {
        st = psa_generate_random(id->boot_seed, sizeof(id->boot_seed));
    }
    if (st == PSA_SUCCESS) {
        st = plat_iak_init(id);
    }
    plat.ready = st == PSA_SUCCESS;
    return st;
}

const struct svc_platform_id *svc_platform_id(void)
{
    return plat.ready ? &plat.id : NULL;
}

psa_status_t svc_platform_sign(const uint8_t hash[32], uint8_t sig[64])
{
    size_t n = 0;
    psa_status_t st;

    if (!plat.ready) {
        return PSA_ERROR_SERVICE_FAILURE;
    }
    st = psa_sign_hash(plat.iak, PLAT_ECDSA, hash, 32, sig, 64, &n);
    return st == PSA_SUCCESS && n != 64 ? PSA_ERROR_GENERIC_ERROR : st;
}

void svc_platform_client_reset(void)
{
}

/* bytes of fn type; NULL while the source is missing */
static const void *plat_src(int32_t type)
{
    switch (type) {
    case PLATFORM_FN_GET_UID:
        return plat.id.uid;
    case PLATFORM_FN_GET_LIFECYCLE:
        return &plat.id.lifecycle;
    case PLATFORM_FN_GET_IAK_PUB:
        return plat.ready ? plat.id.iak_pub : NULL;
    case PLATFORM_FN_GET_NSPE_MEASUREMENT:
        return spm_nspe_measurement;
    case PLATFORM_FN_GET_STATS:
        return (const void *)spm_virq_ctrl();
    default:
        return NULL;
    }
}

psa_status_t svc_platform_call(int32_t type, const psa_invec *in,
                               size_t in_len, psa_outvec *out,
                               size_t out_len)
{
    psa_status_t st = PSA_ERROR_NOT_SUPPORTED;
    const void *src = NULL;
    size_t n = 0;

    (void)in;
    if (type >= PLATFORM_FN_GET_UID && type <= PLATFORM_FN_GET_STATS) {
        n = plat_fns[type].size;
        st = in_len == 0 && out_len == 1 && out[0].len == n
                 ? PSA_SUCCESS : PSA_ERROR_INVALID_ARGUMENT;
    }
    if (st == PSA_SUCCESS) {
        src = plat_src(type);
        st = src != NULL ? PSA_SUCCESS : plat_fns[type].missing;
    }
    for (size_t i = 0; i < out_len; i++) {
        out[i].len = 0;
    }
    if (st == PSA_SUCCESS) {
        memcpy(out[0].base, src, n);
        out[0].len = n;
    }
    return st;
}
