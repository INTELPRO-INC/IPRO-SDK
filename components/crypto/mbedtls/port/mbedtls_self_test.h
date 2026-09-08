/*
 * mbedTLS Hardware Acceleration Self-Test Header
 */

#ifndef MBEDTLS_SELF_TEST_H
#define MBEDTLS_SELF_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run all hardware acceleration self-tests
 * 
 * This function runs self-tests for all enabled hardware-accelerated
 * cryptographic functions (SHA-1, SHA-256, SHA-512, AES, ECC, etc.)
 * 
 * @param verbose If 1, print detailed test results; if 0, print summary only
 * @return 0 on success (all tests passed), non-zero on failure
 * 
 * @note Tests that are known to have issues (e.g., large data handling)
 *       may print warnings but won't fail the overall test suite
 */
int mbedtls_hardware_self_test(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_SELF_TEST_H */
