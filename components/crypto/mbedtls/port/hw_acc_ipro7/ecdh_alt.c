/**
 * @file ecdh_alt.c
 * @brief ECDH hardware acceleration using IPRO7 PKA engine
 *
 * Provides hardware-accelerated ECDH key generation and shared secret computation.
 * - Uses HAL sec_ecdh API for supported curves (secp256r1, secp256k1)
 * - Falls back to software implementation for unsupported curves
 * - Stack-based allocation (no PSRAM required)
 */

#include "mbedtls/config.h"

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)

#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"
#include "mbedtls/bignum.h"
#include "hal_sec_ecdsa.h"
#include <string.h>

/* Global handle aligned for hardware access */
static sec_ecdh_handle_t g_ecdh_handle __attribute__((aligned(32)));

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

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)
/**
 * @brief ECDH public key generation — HW accelerated with SW fallback
 *
 * Q = d * G (where G is the generator point)
 */
int mbedtls_ecdh_gen_public(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng)
{
    int ret;
    sec_ecp_type ecp_type;
    uint32_t priv_words[8];
    uint32_t pub_x_words[8];
    uint32_t pub_y_words[8];

    /* Software fallback for unsupported curves */
    ret = mbedtls_to_hal_curve(grp->id, &ecp_type);
    if (ret != 0) {
        ret = mbedtls_ecp_gen_privkey(grp, d, f_rng, p_rng);
        if (ret == 0)
            ret = mbedtls_ecp_mul(grp, Q, d, &grp->G, f_rng, p_rng);
        return ret;
    }

    /* Generate private key if not provided */
    if (mbedtls_mpi_size(d) == 0) {
        ret = mbedtls_ecp_gen_privkey(grp, d, f_rng, p_rng);
        if (ret != 0)
            return ret;
    }

    /* Initialize hardware */
    ret = sec_ecdh_init(&g_ecdh_handle, ecp_type);
    if (ret != 0)
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;

    /* Convert private key to words */
    ret = mpi_to_words(d, priv_words, 8);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return ret;
    }

    /* Compute public key: Q = d * G */
    ret = sec_ecdh_get_public_key(&g_ecdh_handle, priv_words, pub_x_words, pub_y_words);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }

    /* Convert results back to MPIs */
    ret = words_to_mpi(&Q->X, pub_x_words, 8);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return ret;
    }

    ret = words_to_mpi(&Q->Y, pub_y_words, 8);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return ret;
    }

    ret = mbedtls_mpi_lset(&Q->Z, 1);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return ret;
    }

    sec_ecdh_deinit(&g_ecdh_handle);
    return 0;
}
#endif /* MBEDTLS_ECDH_GEN_PUBLIC_ALT */

#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
/**
 * @brief ECDH shared secret computation — HW accelerated with SW fallback
 *
 * z = d * Q (only X coordinate returned as shared secret)
 */
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z,
                                const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
    int ret;
    sec_ecp_type ecp_type;
    uint32_t priv_words[8];
    uint32_t peer_x_words[8];
    uint32_t peer_y_words[8];
    uint32_t shared_x_words[8];
    uint32_t shared_y_words[8];

    /* Software fallback for unsupported curves */
    ret = mbedtls_to_hal_curve(grp->id, &ecp_type);
    if (ret != 0) {
        mbedtls_ecp_point P;
        mbedtls_ecp_point_init(&P);

        ret = mbedtls_ecp_mul(grp, &P, d, Q, f_rng, p_rng);
        if (ret == 0) {
            if (mbedtls_ecp_is_zero(&P))
                ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            else
                ret = mbedtls_mpi_copy(z, &P.X);
        }

        mbedtls_ecp_point_free(&P);
        return ret;
    }

    /* Initialize hardware */
    ret = sec_ecdh_init(&g_ecdh_handle, ecp_type);
    if (ret != 0)
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;

    /* Convert private key to words */
    ret = mpi_to_words(d, priv_words, 8);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return ret;
    }

    /* Convert peer's public key to words */
    ret = mpi_to_words(&Q->X, peer_x_words, 8);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return ret;
    }

    ret = mpi_to_words(&Q->Y, peer_y_words, 8);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return ret;
    }

    /* Compute shared secret: shared = d * Q */
    ret = sec_ecdh_get_encrypt_key(&g_ecdh_handle, peer_x_words, peer_y_words,
                                    priv_words, shared_x_words, shared_y_words);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    }

    /* Convert X coordinate to MPI (shared secret) */
    ret = words_to_mpi(z, shared_x_words, 8);
    if (ret != 0) {
        sec_ecdh_deinit(&g_ecdh_handle);
        return ret;
    }

    sec_ecdh_deinit(&g_ecdh_handle);
    return 0;
}
#endif /* MBEDTLS_ECDH_COMPUTE_SHARED_ALT */

#if defined(MBEDTLS_SELF_TEST)

#include <stdio.h>

/**
 * @brief ECDH hardware acceleration self-test
 */
int mbedtls_ecdh_self_test_hw(int verbose)
{
    int ret;
    mbedtls_ecp_group grp;
    mbedtls_mpi dA, dB;
    mbedtls_ecp_point QA, QB;
    mbedtls_mpi zA, zB;

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&dA);
    mbedtls_mpi_init(&dB);
    mbedtls_ecp_point_init(&QA);
    mbedtls_ecp_point_init(&QB);
    mbedtls_mpi_init(&zA);
    mbedtls_mpi_init(&zB);

    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        if (verbose)
            printf("  ECDH setup failed: -0x%04x\n", -ret);
        goto cleanup;
    }

    /* Test key generation for party A */
    {
        sec_ecdh_handle_t handle;
        uint32_t priv_a[8] = {
            0x12345678, 0x9ABCDEF0, 0x13579BDF, 0x2468ACE0,
            0xFEDCBA98, 0x76543210, 0xECA86420, 0x13579BDF
        };
        uint32_t pub_ax[8], pub_ay[8];

        ret = sec_ecdh_init(&handle, ECP_SECP256R1);
        if (ret != 0) {
            if (verbose)
                printf("  ECDH Gen A: FAILED (init error)\n");
            ret = -1;
            goto cleanup;
        }

        ret = sec_ecdh_get_public_key(&handle, priv_a, pub_ax, pub_ay);
        sec_ecdh_deinit(&handle);

        if (ret != 0) {
            if (verbose)
                printf("  ECDH Gen A: FAILED (gen error)\n");
            ret = -1;
            goto cleanup;
        }

        ret = words_to_mpi(&dA, priv_a, 8);
        if (ret != 0) goto cleanup;

        ret = words_to_mpi(&QA.X, pub_ax, 8);
        if (ret != 0) goto cleanup;

        ret = words_to_mpi(&QA.Y, pub_ay, 8);
        if (ret != 0) goto cleanup;

        ret = mbedtls_mpi_lset(&QA.Z, 1);
        if (ret != 0) goto cleanup;

        if (verbose)
            printf("  ECDH Gen A: PASSED\n");
    }

    /* Test key generation for party B */
    {
        sec_ecdh_handle_t handle;
        uint32_t priv_b[8] = {
            0xFDB97531, 0x86420ECA, 0xBDF13579, 0xACE02468,
            0x98765432, 0x10FEDCBA, 0x20ECA864, 0xBDF13579
        };
        uint32_t pub_bx[8], pub_by[8];

        ret = sec_ecdh_init(&handle, ECP_SECP256R1);
        if (ret != 0) {
            if (verbose)
                printf("  ECDH Gen B: FAILED (init error)\n");
            ret = -1;
            goto cleanup;
        }

        ret = sec_ecdh_get_public_key(&handle, priv_b, pub_bx, pub_by);
        sec_ecdh_deinit(&handle);

        if (ret != 0) {
            if (verbose)
                printf("  ECDH Gen B: FAILED (gen error)\n");
            ret = -1;
            goto cleanup;
        }

        ret = words_to_mpi(&dB, priv_b, 8);
        if (ret != 0) goto cleanup;

        ret = words_to_mpi(&QB.X, pub_bx, 8);
        if (ret != 0) goto cleanup;

        ret = words_to_mpi(&QB.Y, pub_by, 8);
        if (ret != 0) goto cleanup;

        ret = mbedtls_mpi_lset(&QB.Z, 1);
        if (ret != 0) goto cleanup;

        if (verbose)
            printf("  ECDH Gen B: PASSED\n");
    }

    /* Test shared secret computation: A's view */
    {
        sec_ecdh_handle_t handle;
        uint32_t priv_a[8], peer_bx[8], peer_by[8];
        uint32_t shared_ax[8], shared_ay[8];

        ret = mpi_to_words(&dA, priv_a, 8);
        if (ret != 0) goto cleanup;

        ret = mpi_to_words(&QB.X, peer_bx, 8);
        if (ret != 0) goto cleanup;

        ret = mpi_to_words(&QB.Y, peer_by, 8);
        if (ret != 0) goto cleanup;

        ret = sec_ecdh_init(&handle, ECP_SECP256R1);
        if (ret != 0) {
            if (verbose)
                printf("  ECDH Shared A: FAILED (init error)\n");
            ret = -1;
            goto cleanup;
        }

        ret = sec_ecdh_get_encrypt_key(&handle, peer_bx, peer_by, priv_a,
                                        shared_ax, shared_ay);
        sec_ecdh_deinit(&handle);

        if (ret != 0) {
            if (verbose)
                printf("  ECDH Shared A: FAILED (compute error)\n");
            ret = -1;
            goto cleanup;
        }

        ret = words_to_mpi(&zA, shared_ax, 8);
        if (ret != 0) goto cleanup;

        if (verbose)
            printf("  ECDH Shared A: PASSED\n");
    }

    /* Test shared secret computation: B's view */
    {
        sec_ecdh_handle_t handle;
        uint32_t priv_b[8], peer_ax[8], peer_ay[8];
        uint32_t shared_bx[8], shared_by[8];

        ret = mpi_to_words(&dB, priv_b, 8);
        if (ret != 0) goto cleanup;

        ret = mpi_to_words(&QA.X, peer_ax, 8);
        if (ret != 0) goto cleanup;

        ret = mpi_to_words(&QA.Y, peer_ay, 8);
        if (ret != 0) goto cleanup;

        ret = sec_ecdh_init(&handle, ECP_SECP256R1);
        if (ret != 0) {
            if (verbose)
                printf("  ECDH Shared B: FAILED (init error)\n");
            ret = -1;
            goto cleanup;
        }

        ret = sec_ecdh_get_encrypt_key(&handle, peer_ax, peer_ay, priv_b,
                                        shared_bx, shared_by);
        sec_ecdh_deinit(&handle);

        if (ret != 0) {
            if (verbose)
                printf("  ECDH Shared B: FAILED (compute error)\n");
            ret = -1;
            goto cleanup;
        }

        ret = words_to_mpi(&zB, shared_bx, 8);
        if (ret != 0) goto cleanup;

        if (verbose)
            printf("  ECDH Shared B: PASSED\n");
    }

    /* Verify both parties computed the same shared secret */
    if (mbedtls_mpi_cmp_mpi(&zA, &zB) != 0) {
        if (verbose)
            printf("  ECDH Verify: FAILED (secrets don't match)\n");
        ret = -1;
        goto cleanup;
    }

    if (verbose)
        printf("  ECDH Verify: PASSED\n");

    ret = 0;

cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&dA);
    mbedtls_mpi_free(&dB);
    mbedtls_ecp_point_free(&QA);
    mbedtls_ecp_point_free(&QB);
    mbedtls_mpi_free(&zA);
    mbedtls_mpi_free(&zB);

    return ret;
}

#endif /* MBEDTLS_SELF_TEST */

#endif /* MBEDTLS_ECDH_GEN_PUBLIC_ALT || MBEDTLS_ECDH_COMPUTE_SHARED_ALT */
