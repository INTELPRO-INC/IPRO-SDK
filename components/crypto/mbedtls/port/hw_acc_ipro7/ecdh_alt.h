/**
 * @file ecdh_alt.h
 * @brief ECDH hardware acceleration using IPRO7 PKA engine
 * 
 * This file provides hardware-accelerated ECDH operations for mbedTLS
 * using the IPRO7 secure engine PKA (Public Key Accelerator).
 */

#ifndef MBEDTLS_ECDH_ALT_H
#define MBEDTLS_ECDH_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run ECDH hardware acceleration self-test
 * 
 * Tests both key generation and shared secret computation using NIST P-256
 * 
 * @param verbose  If non-zero, print detailed test results
 * @return 0 on success, non-zero on failure
 */
int mbedtls_ecdh_self_test_hw(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_ECDH_ALT_H */
