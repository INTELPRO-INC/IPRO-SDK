/**
 * @file ecdsa_alt.h
 * @brief ECDSA hardware acceleration using IPRO7 PKA engine
 * 
 * This file provides hardware-accelerated ECDSA operations for mbedTLS
 * using the IPRO7 secure engine PKA (Public Key Accelerator).
 */

#ifndef MBEDTLS_ECDSA_ALT_H
#define MBEDTLS_ECDSA_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run ECDSA hardware acceleration self-test
 * 
 * Tests both sign and verify operations using NIST P-256 test vectors
 * 
 * @param verbose  If non-zero, print detailed test results
 * @return 0 on success, non-zero on failure
 */
int mbedtls_ecdsa_self_test_hw(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_ECDSA_ALT_H */
