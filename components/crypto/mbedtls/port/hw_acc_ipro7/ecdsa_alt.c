/**
 * @file ecdsa_alt.c
 * @brief ECDSA hardware acceleration using IPRO7 PKA engine
 *
 * Provides hardware-accelerated ECDSA sign and verify operations.
 * - Uses HAL sec_ecdsa API for supported curves (secp256r1, secp256k1)
 * - Falls back to software implementation for unsupported curves
 * - Stack-based allocation (no PSRAM required)
 */

#include "mbedtls/config.h"

#if defined(MBEDTLS_ECDSA_SIGN_ALT) || defined(MBEDTLS_ECDSA_VERIFY_ALT)

#include "mbedtls/ecdsa.h"
#include "mbedtls/ecp.h"
#include "mbedtls/bignum.h"
#include "hal_sec_ecdsa.h"
#include <string.h>

/* Global handle aligned for hardware access */
static sec_ecdsa_handle_t g_ecdsa_handle __attribute__((aligned(32)));

/**
 * @brief Convert mbedTLS curve ID to HAL ECP type
 */
static int mbedtls_to_hal_curve(mbedtls_ecp_group_id gid, sec_ecp_type *ecp_type)
{
    switch (gid) {
        case MBEDTLS_ECP_DP_SECP256R1:
            *ecp_type = ECP_SECP256R1;
            return 0;
        case MBEDTLS_ECP_DP_SECP256K1:
            *ecp_type = ECP_SECP256K1;
            return 0;
        default:
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }
}

/**
 * @brief Export mbedTLS MPI to word array for PKA hardware
 */
static int mpi_to_words(const mbedtls_mpi *X, uint32_t *words, size_t word_count)
{
    size_t byte_len = word_count * 4;

    return mbedtls_mpi_write_binary(X, (unsigned char *)words, byte_len);
}

/**
 * @brief Import word array from PKA hardware to mbedTLS MPI
 */
static int words_to_mpi(mbedtls_mpi *X, const uint32_t *words, size_t word_count)
{
    size_t byte_len = word_count * 4;

    return mbedtls_mpi_read_binary(X, (const unsigned char *)words, byte_len);
}

/**
 * @brief Derive MPI from hash buffer (SEC1 4.1.3 step 5)
 *
 * Truncates hash to curve order size and reduces modulo N.
 */
static int derive_mpi(const mbedtls_ecp_group *grp, mbedtls_mpi *x,
                      const unsigned char *buf, size_t blen)
{
    int ret;
    size_t n_size = (grp->nbits + 7) / 8;
    size_t use_size = blen > n_size ? n_size : blen;

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(x, buf, use_size));
    if (use_size * 8 > grp->nbits)
        MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(x, use_size * 8 - grp->nbits));

    if (mbedtls_mpi_cmp_mpi(x, &grp->N) >= 0)
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(x, x, &grp->N));

cleanup:
    return ret;
}

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
/**
 * @brief Software ECDSA sign fallback for unsupported curves
 */
static int ecdsa_sign_sw(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                         const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                         int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret, key_tries, sign_tries;
    mbedtls_ecp_point R;
    mbedtls_mpi k, e, t;

    if (grp->N.p == NULL)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    if (mbedtls_mpi_cmp_int(d, 1) < 0 || mbedtls_mpi_cmp_mpi(d, &grp->N) >= 0)
        return MBEDTLS_ERR_ECP_INVALID_KEY;

    mbedtls_ecp_point_init(&R);
    mbedtls_mpi_init(&k);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&t);

    sign_tries = 0;
    do {
        if (++sign_tries > 10) {
            ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
            goto cleanup;
        }

        key_tries = 0;
        do {
            if (++key_tries > 10) {
                ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
                goto cleanup;
            }

            MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(grp, &k, f_rng, p_rng));
            MBEDTLS_MPI_CHK(mbedtls_ecp_mul(grp, &R, &k, &grp->G,
                                             f_rng, p_rng));
            MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(r, &R.X, &grp->N));
        } while (mbedtls_mpi_cmp_int(r, 0) == 0);

        /* e = hash mod N */
        MBEDTLS_MPI_CHK(derive_mpi(grp, &e, buf, blen));

        /* Blinding: t = random, s = (e + r*d) * t / (k*t) mod N */
        MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(grp, &t, f_rng, p_rng));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(s, r, d));
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&e, &e, s));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&e, &e, &t));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&k, &k, &t));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&k, &k, &grp->N));
        MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(s, &k, &grp->N));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(s, s, &e));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(s, s, &grp->N));
    } while (mbedtls_mpi_cmp_int(s, 0) == 0);

cleanup:
    mbedtls_ecp_point_free(&R);
    mbedtls_mpi_free(&k);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&t);

    return ret;
}

/**
 * @brief ECDSA signature generation — HW accelerated with SW fallback
 */
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret;
    sec_ecp_type ecp_type;
    uint32_t hash_words[8];
    uint32_t priv_words[8];
    uint32_t k_words[8];
    uint32_t r_words[8];
    uint32_t s_words[8];
    size_t hash_len_words;

    /* Software fallback for unsupported curves */
    ret = mbedtls_to_hal_curve(grp->id, &ecp_type);
    if (ret != 0)
        return ecdsa_sign_sw(grp, r, s, d, buf, blen, f_rng, p_rng);

    /* Initialize hardware */
    ret = sec_ecdsa_init(&g_ecdsa_handle, ecp_type);
    if (ret != 0)
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;

    /* Convert private key to words */
    ret = mpi_to_words(d, priv_words, 8);
    if (ret != 0) {
        sec_ecdsa_deinit(&g_ecdsa_handle);
        return ret;
    }

    /* Copy hash bytes directly — PKA BigEndian mode handles byte-swap */
    memset(hash_words, 0, sizeof(hash_words));
    hash_len_words = (blen + 3) / 4;
    if (hash_len_words > 8)
        hash_len_words = 8;
    memcpy(hash_words, buf, blen > 32 ? 32 : blen);

    /* Generate random k */
    if (f_rng != NULL) {
        unsigned char k_bytes[32];
        ret = f_rng(p_rng, k_bytes, 32);
        if (ret != 0) {
            sec_ecdsa_deinit(&g_ecdsa_handle);
            return ret;
        }
        memcpy(k_words, k_bytes, 32);
    } else {
        sec_ecdsa_deinit(&g_ecdsa_handle);
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* Set up handle with keys */
    g_ecdsa_handle.privateKey = priv_words;

    /* Perform hardware signing */
    ret = sec_ecdsa_sign(&g_ecdsa_handle, k_words, hash_words,
                         hash_len_words, r_words, s_words);
    if (ret != 0) {
        sec_ecdsa_deinit(&g_ecdsa_handle);
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }

    /* Convert results back to MPIs */
    ret = words_to_mpi(r, r_words, 8);
    if (ret != 0) {
        sec_ecdsa_deinit(&g_ecdsa_handle);
        return ret;
    }

    ret = words_to_mpi(s, s_words, 8);
    if (ret != 0) {
        sec_ecdsa_deinit(&g_ecdsa_handle);
        return ret;
    }

    sec_ecdsa_deinit(&g_ecdsa_handle);
    return 0;
}
#endif /* MBEDTLS_ECDSA_SIGN_ALT */

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
/**
 * @brief Software ECDSA verify fallback for unsupported curves
 *
 * Implements SEC1 4.1.4: u1 = e/s, u2 = r/s, R = u1*G + u2*Q, check R.x == r
 */
static int ecdsa_verify_sw(mbedtls_ecp_group *grp,
                           const unsigned char *buf, size_t blen,
                           const mbedtls_ecp_point *Q,
                           const mbedtls_mpi *r, const mbedtls_mpi *s)
{
    int ret;
    mbedtls_mpi e, s_inv, u1, u2;
    mbedtls_ecp_point R;

    if (grp->N.p == NULL)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    mbedtls_ecp_point_init(&R);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&s_inv);
    mbedtls_mpi_init(&u1);
    mbedtls_mpi_init(&u2);

    /* Step 1: check r, s in [1, n-1] */
    if (mbedtls_mpi_cmp_int(r, 1) < 0 || mbedtls_mpi_cmp_mpi(r, &grp->N) >= 0 ||
        mbedtls_mpi_cmp_int(s, 1) < 0 || mbedtls_mpi_cmp_mpi(s, &grp->N) >= 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    /* Step 3: e = hash mod N */
    MBEDTLS_MPI_CHK(derive_mpi(grp, &e, buf, blen));

    /* Step 4: u1 = e * s^-1 mod N, u2 = r * s^-1 mod N */
    MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&s_inv, s, &grp->N));

    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&u1, &e, &s_inv));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&u1, &u1, &grp->N));

    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&u2, r, &s_inv));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&u2, &u2, &grp->N));

    /* Step 5: R = u1*G + u2*Q */
    MBEDTLS_MPI_CHK(mbedtls_ecp_muladd(grp, &R, &u1, &grp->G, &u2, Q));

    if (mbedtls_ecp_is_zero(&R)) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    /* Step 6-8: check R.x mod N == r */
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&R.X, &R.X, &grp->N));

    if (mbedtls_mpi_cmp_mpi(&R.X, r) != 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

cleanup:
    mbedtls_ecp_point_free(&R);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&s_inv);
    mbedtls_mpi_free(&u1);
    mbedtls_mpi_free(&u2);

    return ret;
}

/**
 * @brief ECDSA signature verification — HW accelerated with SW fallback
 */
int mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r, const mbedtls_mpi *s)
{
    /* Use software verify for all curves.
     * HW PKA verify needs further validation before enabling. */
    return ecdsa_verify_sw(grp, buf, blen, Q, r, s);
}
#endif /* MBEDTLS_ECDSA_VERIFY_ALT */

#endif /* MBEDTLS_ECDSA_SIGN_ALT || MBEDTLS_ECDSA_VERIFY_ALT */
