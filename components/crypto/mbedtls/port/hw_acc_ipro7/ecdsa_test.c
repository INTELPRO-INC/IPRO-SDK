/**
 * @file ecdsa_test.c
 * @brief Test program for ECDSA hardware acceleration
 * 
 * Validates ECDSA sign/verify operations with NIST P-256 test vectors.
 */

#include <stdio.h>
#include <string.h>
#include "ecdsa_alt.h"

int main(void)
{
    int ret;
    
    printf("\n");
    printf("========================================\n");
    printf("ECDSA Hardware Acceleration Test\n");
    printf("========================================\n");
    
    ret = mbedtls_ecdsa_self_test_hw(1);
    
    printf("========================================\n");
    if (ret == 0) {
        printf("Result: ALL TESTS PASSED\n");
    } else {
        printf("Result: TESTS FAILED\n");
    }
    printf("========================================\n");
    
    return ret;
}
