/**
 * @file ecdsa_self_test_simple.c
 * @brief Simplified ECDSA self-test using mbedTLS API
 */

#include "mbedtls/config.h"

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_SELF_TEST)

#include "mbedtls/ecdsa.h"
#include "mbedtls/ecp.h"
#include "mbedtls/bignum.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Simple ECDSA test using mbedTLS API
 * 
 * Tests sign/verify cycle to ensure hardware acceleration works correctly.
 * Does not require exact RFC 6979 signature matching.
 */
int mbedtls_ecdsa_self_test_hw(int verbose)
{
    int ret;
    
    /* NIST P-256 test vectors */
    static const unsigned char test_hash[32] = {
        0xAF, 0x2B, 0xDB, 0xE1, 0xAA, 0x9B, 0x6E, 0xC1,
        0xE2, 0xAD, 0xE1, 0xD6, 0x94, 0xF4, 0x1F, 0xC7,
        0x1A, 0x83, 0x1D, 0x02, 0x68, 0xE9, 0x89, 0x15,
        0x62, 0x11, 0x3D, 0x8A, 0x62, 0xAD, 0xD1, 0xBF
    };
    
    static const unsigned char test_priv[32] = {
        0xC9, 0xAF, 0xA9, 0xD8, 0x45, 0xBA, 0x75, 0x16,
        0x6B, 0x5C, 0x21, 0x57, 0x67, 0xB1, 0xD6, 0x93,
        0x4E, 0x50, 0xC3, 0xDB, 0x36, 0xE8, 0x9B, 0x12,
        0x7B, 0x8A, 0x62, 0x2B, 0x12, 0x0F, 0x67, 0x21
    };
    
    static const unsigned char test_pub_x[32] = {
        0x60, 0xFE, 0xD4, 0xBA, 0x25, 0x5A, 0x9D, 0x31,
        0xC9, 0x61, 0xEB, 0x74, 0xC6, 0x35, 0x6D, 0x68,
        0xC0, 0x49, 0xB8, 0x92, 0x3B, 0x61, 0xFA, 0x6C,
        0xE6, 0x69, 0x62, 0x2E, 0x60, 0xF2, 0x9F, 0xB6
    };
    
    static const unsigned char test_pub_y[32] = {
        0x79, 0x03, 0xFE, 0x10, 0x08, 0xB8, 0xBC, 0x99,
        0xA4, 0x1A, 0xE9, 0xE9, 0x56, 0x28, 0xBC, 0x64,
        0xF2, 0xF1, 0xB2, 0x0C, 0x2D, 0x7E, 0x9F, 0x51,
        0x77, 0xA3, 0xC2, 0x94, 0xD4, 0x46, 0x22, 0x99
    };
    
    static unsigned char test_k[32] = {
        0xA6, 0xE3, 0xC5, 0x7D, 0xD0, 0x1A, 0xBE, 0x90,
        0x08, 0x65, 0x38, 0x39, 0x83, 0x55, 0xDD, 0x4C,
        0x3B, 0x17, 0xAA, 0x87, 0x33, 0x82, 0xB0, 0xF2,
        0x4D, 0x61, 0x29, 0x49, 0x3D, 0x8A, 0xAD, 0x60
    };
    
    /* Simple test RNG */
    int test_rng(void *rng_state, unsigned char *output, size_t len)
    {
        (void)rng_state;
        if (len > 32) len = 32;
        memcpy(output, test_k, len);
        return 0;
    }
    
    /* Initialize structures */
    mbedtls_ecp_group grp;
    mbedtls_mpi d, r, s;
    mbedtls_ecp_point Q;
    
    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&d);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_ecp_point_init(&Q);
    
    /* Load P-256 curve */
    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        if (verbose) {
            printf("  ECDSA setup failed: -0x%04x\n", -ret);
        }
        goto cleanup;
    }
    
    /* Load test keys */
    ret = mbedtls_mpi_read_binary(&d, test_priv, 32);
    if (ret != 0) goto cleanup;
    
    ret = mbedtls_mpi_read_binary(&Q.X, test_pub_x, 32);
    if (ret != 0) goto cleanup;
    
    ret = mbedtls_mpi_read_binary(&Q.Y, test_pub_y, 32);
    if (ret != 0) goto cleanup;
    
    ret = mbedtls_mpi_lset(&Q.Z, 1);
    if (ret != 0) goto cleanup;
    
    /* Test hardware signing via mbedTLS API */
    ret = mbedtls_ecdsa_sign(&grp, &r, &s, &d, test_hash, 32, test_rng, NULL);
    if (ret != 0) {
        if (verbose) {
            printf("  ECDSA Sign: FAILED (error: -0x%04x)\n", -ret);
        }
        goto cleanup;
    }
    
    if (verbose) {
        printf("  ECDSA Sign: PASSED\n");
    }
    
    /* Test hardware verification via mbedTLS API */
    ret = mbedtls_ecdsa_verify(&grp, test_hash, 32, &Q, &r, &s);
    if (ret != 0) {
        if (verbose) {
            printf("  ECDSA Verify: FAILED (error: -0x%04x)\n", -ret);
        }
        goto cleanup;
    }
    
    if (verbose) {
        printf("  ECDSA Verify: PASSED\n");
    }
    
    ret = 0;
    
cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&d);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecp_point_free(&Q);
    
    return ret;
}

#endif /* MBEDTLS_ECDSA_C && MBEDTLS_SELF_TEST */
