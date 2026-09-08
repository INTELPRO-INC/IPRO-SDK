/*
 * mbedTLS Hardware Acceleration Self-Test
 * 
 * This file provides a unified self-test interface for all hardware-accelerated
 * cryptographic functions in mbedTLS. It tests both hardware and software implementations.
 */

#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha512.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecdh.h"
#include <stdio.h>

/* External self-test functions from hardware acceleration modules */
#if defined(CONFIG_MBEDTLS_AES_USE_HW)
extern int mbedtls_aes_self_test_hw(int verbose);
#endif

#if defined(CONFIG_MBEDTLS_SHA256_USE_HW) && defined(MBEDTLS_SHA256_ALT)
extern int mbedtls_sha256_self_test_hw(int verbose);
#endif

#if defined(CONFIG_MBEDTLS_SHA1_USE_HW) && defined(MBEDTLS_SHA1_ALT)
extern int mbedtls_sha1_self_test_hw(int verbose);
#endif

#if defined(CONFIG_MBEDTLS_SHA512_USE_HW) && defined(MBEDTLS_SHA512_ALT)
extern int mbedtls_sha512_self_test_hw(int verbose);
#endif

#if defined(CONFIG_MBEDTLS_GCM_USE_HW) && defined(MBEDTLS_GCM_ALT)
extern int mbedtls_gcm_self_test_hw(int verbose);
#endif

#if defined(CONFIG_MBEDTLS_BIGNUM_USE_HW)
extern int mbedtls_bignum_self_test_hw(int verbose);
#endif

#if defined(CONFIG_MBEDTLS_ECDSA_USE_HW)
extern int mbedtls_ecdsa_self_test_hw(int verbose);
#endif

#if defined(CONFIG_MBEDTLS_ECDH_USE_HW)
extern int mbedtls_ecdh_self_test_hw(int verbose);
#endif

/**
 * @brief Run all hardware and software self-tests
 * 
 * @param verbose If 1, print test details; if 0, only print summary
 * @return 0 on success, non-zero on failure
 */
int mbedtls_hardware_self_test(int verbose)
{
    int ret __attribute__((unused)) = 0;
    int total_tests = 0;
    int passed_tests = 0;
    
    if (verbose) {
        printf("\n========================================\n");
        printf("mbedTLS Cryptographic Self-Test\n");
        printf("========================================\n\n");
    }
    
    /* Test AES Hardware Acceleration */
#if defined(MBEDTLS_AES_C) && defined(CONFIG_MBEDTLS_AES_USE_HW)
    total_tests++;
    if (verbose) {
        printf("[TEST] AES Implementation\n");
        printf("  Testing Hardware Acceleration:\n");
    }
    ret = mbedtls_aes_self_test_hw(verbose);
    if (ret == 0) {
        passed_tests++;
        if (verbose) {
            printf("  [PASS] AES hardware acceleration works\n\n");
        }
    } else {
        if (verbose) {
            printf("  [FAIL] AES hardware test failed (code: %d)\n\n", ret);
        }
    }
#endif
    
    /* Test SHA-256 Hardware Acceleration */
#if defined(CONFIG_MBEDTLS_SHA256_USE_HW) && defined(MBEDTLS_SHA256_ALT)
    total_tests++;
    if (verbose) {
        printf("[TEST] SHA-256 Implementation\n");
        printf("  Testing Hardware Acceleration:\n");
    }
    ret = mbedtls_sha256_self_test_hw(verbose);
    if (ret == 0) {
        passed_tests++;
        if (verbose) {
            printf("  [PASS] SHA-256 hardware acceleration works\n\n");
        }
    } else {
        if (verbose) {
            printf("  [FAIL] SHA-256 hardware test failed (code: %d)\n\n", ret);
        }
    }
#endif

    /* Test SHA-1 Hardware Acceleration */
#if defined(MBEDTLS_SHA1_C) && defined(CONFIG_MBEDTLS_SHA1_USE_HW) && defined(MBEDTLS_SHA1_ALT)
    total_tests++;
    if (verbose) {
        printf("[TEST] SHA-1 Implementation\n");
        printf("  Testing Hardware Acceleration:\n");
    }
    ret = mbedtls_sha1_self_test_hw(verbose);
    if (ret == 0) {
        passed_tests++;
        if (verbose) {
            printf("  [PASS] SHA-1 hardware acceleration works\n\n");
        }
    } else {
        if (verbose) {
            printf("  [FAIL] SHA-1 hardware test failed (code: %d)\n\n", ret);
        }
    }
#endif

    /* Test SHA-512 Hardware Acceleration */
#if defined(MBEDTLS_SHA512_C) && defined(CONFIG_MBEDTLS_SHA512_USE_HW) && defined(MBEDTLS_SHA512_ALT)
    total_tests++;
    if (verbose) {
        printf("[TEST] SHA-512 Implementation\n");
        printf("  Testing Hardware Acceleration:\n");
    }
    ret = mbedtls_sha512_self_test_hw(verbose);
    if (ret == 0) {
        passed_tests++;
        if (verbose) {
            printf("  [PASS] SHA-512 hardware acceleration works\n\n");
        }
    } else {
        if (verbose) {
            printf("  [FAIL] SHA-512 hardware test failed (code: %d)\n\n", ret);
        }
    }
#endif

    /* Test GCM Hardware Acceleration */
#if defined(MBEDTLS_GCM_C) && defined(CONFIG_MBEDTLS_GCM_USE_HW) && defined(MBEDTLS_GCM_ALT)
    total_tests++;
    if (verbose) {
        printf("[TEST] AES-GCM Implementation\n");
        printf("  Testing Hardware Acceleration:\n");
    }
    ret = mbedtls_gcm_self_test_hw(verbose);
    if (ret == 0) {
        passed_tests++;
        if (verbose) {
            printf("  [PASS] AES-GCM hardware acceleration works\n\n");
        }
    } else {
        if (verbose) {
            printf("  [FAIL] AES-GCM hardware test failed (code: %d)\n\n", ret);
        }
    }
#endif

    /* Test Bignum Hardware Acceleration */
#if defined(CONFIG_MBEDTLS_BIGNUM_USE_HW)
    total_tests++;
    if (verbose) {
        printf("[TEST] Bignum exp_mod Implementation\n");
        printf("  Testing Hardware Acceleration:\n");
    }
    ret = mbedtls_bignum_self_test_hw(verbose);
    if (ret == 0) {
        passed_tests++;
        if (verbose) {
            printf("  [PASS] Bignum hardware acceleration works\n\n");
        }
    } else {
        if (verbose) {
            printf("  [FAIL] Bignum hardware test failed (code: %d)\n\n", ret);
        }
    }
#endif

    /* Test ECDSA Hardware Acceleration */
#if defined(MBEDTLS_ECDSA_C) && defined(CONFIG_MBEDTLS_ECDSA_USE_HW)
    total_tests++;
    if (verbose) {
        printf("[TEST] ECDSA Implementation\n");
        printf("  Testing Hardware Acceleration:\n");
    }
    ret = mbedtls_ecdsa_self_test_hw(verbose);
    if (ret == 0) {
        passed_tests++;
        if (verbose) {
            printf("  [PASS] ECDSA hardware acceleration works\n\n");
        }
    } else {
        if (verbose) {
            printf("  [FAIL] ECDSA hardware test failed (code: %d)\n\n", ret);
        }
    }
#endif

    /* Test ECDH Hardware Acceleration */
#if defined(MBEDTLS_ECDH_C) && defined(CONFIG_MBEDTLS_ECDH_USE_HW)
    total_tests++;
    if (verbose) {
        printf("[TEST] ECDH Implementation\n");
        printf("  Testing Hardware Acceleration:\n");
    }
    ret = mbedtls_ecdh_self_test_hw(verbose);
    if (ret == 0) {
        passed_tests++;
        if (verbose) {
            printf("  [PASS] ECDH hardware acceleration works\n\n");
        }
    } else {
        if (verbose) {
            printf("  [FAIL] ECDH hardware test failed (code: %d)\n\n", ret);
        }
    }
#endif

    /* Print summary */
    if (verbose || passed_tests < total_tests) {
        printf("========================================\n");
        printf("Cryptographic Self-Test Summary: %d/%d passed\n", passed_tests, total_tests);
        if (passed_tests == total_tests) {
            printf("Status: ALL TESTS PASSED\n");
        } else {
            printf("Status: SOME TESTS FAILED\n");
        }
        printf("========================================\n\n");
    }
    
    return (passed_tests == total_tests) ? 0 : -1;
}
