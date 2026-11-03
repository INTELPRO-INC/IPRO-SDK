/*
 * IAK public key for the Initial Attestation suite: the SEC1
 * uncompressed P-256 point the SPE platform service reports.
 */
#include <string.h>

#include "pal_attestation_crypto.h"
#include "psa_ipro/platform_api.h"

int32_t tfm_initial_attest_get_public_key(uint8_t *public_key_buff,
                                          size_t public_key_buf_size,
                                          size_t *public_key_len,
                                          psa_ecc_family_t *elliptic_family_type)
{
    uint8_t key[PLATFORM_IAK_PUB_SIZE];

    if (public_key_buff == NULL || public_key_len == NULL ||
        elliptic_family_type == NULL) {
        return PAL_ATTEST_ERR_KEY_FAIL;
    }
    if (public_key_buf_size < sizeof(key)) {
        return PAL_ATTEST_ERR_SMALL_BUFFER;
    }
    if (psa_platform_get_iak_pub(key) != PSA_SUCCESS || key[0] != 0x04) {
        return PAL_ATTEST_ERR_KEY_FAIL;
    }
    memcpy(public_key_buff, key, sizeof(key));
    *public_key_len = sizeof(key);
    *elliptic_family_type = PSA_ECC_FAMILY_SECP_R1;
    return PSA_SUCCESS;
}
