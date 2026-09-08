/*
 * Hardware-accelerated modular exponentiation for IPRO7 PKA engine.
 *
 * Two HW paths:
 *   - Montgomery (default): GF2Mont -> hardware MEXP -> Mont2GF
 *     Single PKA instruction for the entire exponentiation.
 *   - Binary (fallback): software-controlled MSQR/MMUL loop.
 *
 * Falls back to software for operands > 2048 bits.
 */

#include <string.h>
#include <mbedtls/bignum.h>
#include <mbedtls/platform.h>

#include "hal_sec_dsa.h"
#include "drv_sec_eng.h"
#include "sec_mutex.h"

#define MBEDTLS_INTERNAL_VALIDATE_RET(cond, ret) \
    do {                                         \
        if (!(cond))                             \
            return (ret);                        \
    } while (0)

#define MPI_VALIDATE_RET(cond) \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_MPI_BAD_INPUT_DATA)

#define PKA_MAX_BITS 2048

extern int mbedtls_mpi_exp_mod_original(mbedtls_mpi *X, const mbedtls_mpi *A,
                                        const mbedtls_mpi *E,
                                        const mbedtls_mpi *N,
                                        mbedtls_mpi *prec_RR);

static uint32_t bitlen_to_pka_bits(size_t bitlen)
{
    if (bitlen <= 64)   return 64;
    if (bitlen <= 128)  return 128;
    if (bitlen <= 256)  return 256;
    if (bitlen <= 512)  return 512;
    if (bitlen <= 768)  return 768;
    if (bitlen <= 1024) return 1024;
    if (bitlen <= 1536) return 1536;
    if (bitlen <= 2048) return 2048;
    return 0;
}

/*
 * Truncate MPI to low n bits: X = X mod 2^n
 */
static int mpi_mod_2n(mbedtls_mpi *X, size_t n)
{
    const size_t limb_bits = sizeof(mbedtls_mpi_uint) * 8;
    size_t limb_idx = n / limb_bits;
    size_t bit_off = n % limb_bits;

    for (size_t i = limb_idx + (bit_off ? 1 : 0); i < X->n; i++)
        X->p[i] = 0;

    if (bit_off && limb_idx < X->n)
        X->p[limb_idx] &= ((mbedtls_mpi_uint)1 << bit_off) - 1;

    if (X->s < 0) {
        mbedtls_mpi R;
        mbedtls_mpi_init(&R);
        mbedtls_mpi_set_bit(&R, n, 1);
        mbedtls_mpi_add_mpi(X, X, &R);
        if (limb_idx < X->n)
            X->p[limb_idx] &= ((mbedtls_mpi_uint)1 << bit_off) - 1;
        for (size_t i = limb_idx + 1; i < X->n; i++)
            X->p[i] = 0;
        X->s = 1;
        mbedtls_mpi_free(&R);
    }

    return 0;
}

/*
 * Hensel quadratic lifting: compute primeN = -N^(-1) mod 2^m
 *
 * Uses Newton's method to double precision each iteration:
 *   U_{k+1} = U_k * (2 - a * U_k) mod 2^(2k)
 * Then negate: primeN = -U mod 2^m
 */
static int mpi_hensel_quad_mod_inv_prime_n(mbedtls_mpi *U,
                                           const mbedtls_mpi *a,
                                           size_t m)
{
    int ret;
    mbedtls_mpi tmp;

    if (!U || !a || !m)
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;

    if ((a->p[0] & 1) == 0)
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;

    mbedtls_mpi_init(&tmp);

    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(U, 1));

    for (size_t i = 2; i < m; i <<= 1) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, U, U));
        mpi_mod_2n(&tmp, i);
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, &tmp, a));
        mpi_mod_2n(&tmp, i);
        MBEDTLS_MPI_CHK(mbedtls_mpi_shift_l(U, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(U, U, &tmp));
    }

    /* Final iteration */
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, U, U));
    mpi_mod_2n(&tmp, m);
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, &tmp, a));
    mpi_mod_2n(&tmp, m);
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_l(U, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(U, U, &tmp));
    mpi_mod_2n(U, m);

    /* Negate: primeN = -U mod 2^m */
    mbedtls_mpi zero;
    mbedtls_mpi_uint z_ = 0;
    zero.s = 1;
    zero.n = 1;
    zero.p = &z_;
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(U, &zero, U));
    mpi_mod_2n(U, m);

cleanup:
    mbedtls_mpi_free(&tmp);
    return ret;
}

/*
 * Hardware-accelerated modular exponentiation: X = A^E mod N
 *
 * Uses Montgomery domain with hardware MEXP instruction (single PKA op)
 * for the entire exponentiation, instead of software MSQR/MMUL loop.
 *
 * Data format: mbedtls_mpi_write_binary() produces big-endian byte arrays.
 * On LE RISC-V, each uint32_t word is byte-swapped relative to true BE.
 * Sec_Eng_PKA_BigEndian_Enable() makes PKA byte-swap each word on write,
 * so the two swaps cancel out and PKA sees correct values.
 */
int mbedtls_mpi_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A,
                         const mbedtls_mpi *E, const mbedtls_mpi *N,
                         mbedtls_mpi *prec_RR)
{
    int ret;
    uint32_t pka_bits;
    size_t buf_bytes;

    MPI_VALIDATE_RET(X != NULL);
    MPI_VALIDATE_RET(A != NULL);
    MPI_VALIDATE_RET(E != NULL);
    MPI_VALIDATE_RET(N != NULL);

    if (mbedtls_mpi_cmp_int(N, 0) <= 0 || (N->p[0] & 1) == 0)
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;

    if (mbedtls_mpi_cmp_int(E, 0) < 0)
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;

    if (mbedtls_mpi_cmp_int(E, 0) == 0)
        return mbedtls_mpi_lset(X, 1);

    size_t a_bits = mbedtls_mpi_bitlen(A);
    size_t e_bits = mbedtls_mpi_bitlen(E);
    size_t n_bits = mbedtls_mpi_bitlen(N);
    size_t max_bits = a_bits;
    if (e_bits > max_bits) max_bits = e_bits;
    if (n_bits > max_bits) max_bits = n_bits;

    pka_bits = bitlen_to_pka_bits(max_bits);

    if (pka_bits == 0 || pka_bits > PKA_MAX_BITS)
        return mbedtls_mpi_exp_mod_original(X, A, E, N, prec_RR);

    buf_bytes = pka_bits / 8;

    uint8_t *buf_a = mbedtls_calloc(1, buf_bytes);
    uint8_t *buf_e = mbedtls_calloc(1, buf_bytes);
    uint8_t *buf_n = mbedtls_calloc(1, buf_bytes);
    uint8_t *buf_r = mbedtls_calloc(1, buf_bytes);
    uint8_t *buf_primeN = NULL;
    uint8_t *buf_invR = NULL;

    if (!buf_a || !buf_e || !buf_n || !buf_r) {
        ret = MBEDTLS_ERR_MPI_ALLOC_FAILED;
        goto cleanup;
    }

    ret = mbedtls_mpi_write_binary(A, buf_a, buf_bytes);
    if (ret != 0) goto cleanup;

    ret = mbedtls_mpi_write_binary(E, buf_e, buf_bytes);
    if (ret != 0) goto cleanup;

    ret = mbedtls_mpi_write_binary(N, buf_n, buf_bytes);
    if (ret != 0) goto cleanup;

    /*
     * Montgomery path: compute primeN and invR, then use
     * hardware MEXP instruction (single PKA operation).
     *
     * Skip for 2048-bit: HW MEXP returns zero at this register size.
     * Fall through to binary path which works correctly.
     *
     * primeN = -N^(-1) mod 2^pka_bits  (Hensel lifting)
     * invR   = R^(-1) mod N            (extended GCD, R = 2^pka_bits)
     */
    if (pka_bits >= 2048)
        goto use_binary;

    mbedtls_mpi primeN_mpi, invR_mpi, R_mpi;
    mbedtls_mpi_init(&primeN_mpi);
    mbedtls_mpi_init(&invR_mpi);
    mbedtls_mpi_init(&R_mpi);

    ret = mpi_hensel_quad_mod_inv_prime_n(&primeN_mpi, N, pka_bits);
    if (ret != 0)
        goto mont_fail;

    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&R_mpi, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_l(&R_mpi, pka_bits));
    ret = mbedtls_mpi_inv_mod(&invR_mpi, &R_mpi, N);
    if (ret != 0)
        goto mont_fail;

    buf_primeN = mbedtls_calloc(1, buf_bytes);
    buf_invR = mbedtls_calloc(1, buf_bytes);
    if (!buf_primeN || !buf_invR) {
        ret = MBEDTLS_ERR_MPI_ALLOC_FAILED;
        goto mont_fail;
    }

    ret = mbedtls_mpi_write_binary(&primeN_mpi, buf_primeN, buf_bytes);
    if (ret != 0) goto mont_fail;

    ret = mbedtls_mpi_write_binary(&invR_mpi, buf_invR, buf_bytes);
    if (ret != 0) goto mont_fail;

    ipro_sec_pka_mutex_take();
    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    ret = sec_dsa_mexp_mont(pka_bits,
                             (uint32_t *)buf_a, (uint32_t *)buf_e,
                             (uint32_t *)buf_n, (uint32_t *)buf_invR,
                             (uint32_t *)buf_primeN, (uint32_t *)buf_r);

    ipro_sec_pka_mutex_give();

    mbedtls_mpi_free(&primeN_mpi);
    mbedtls_mpi_free(&invR_mpi);
    mbedtls_mpi_free(&R_mpi);

    if (ret != 0) {
        ret = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
        goto cleanup;
    }
    goto read_result;

mont_fail:
    /* Montgomery setup failed, fall back to binary method */
    mbedtls_mpi_free(&primeN_mpi);
    mbedtls_mpi_free(&invR_mpi);
    mbedtls_mpi_free(&R_mpi);

use_binary:
    ipro_sec_pka_mutex_take();
    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    ret = sec_dsa_mexp_binary(pka_bits,
                               (uint32_t *)buf_a, (uint32_t *)buf_e,
                               (uint32_t *)buf_n, (uint32_t *)buf_r);

    ipro_sec_pka_mutex_give();

    if (ret != 0) {
        ret = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
        goto cleanup;
    }

read_result:
    ret = mbedtls_mpi_read_binary(X, buf_r, buf_bytes);
    if (ret != 0)
        goto cleanup;

    if (A->s == -1 && (E->p[0] & 1) != 0) {
        X->s = -1;
        ret = mbedtls_mpi_add_mpi(X, N, X);
    } else {
        X->s = 1;
    }

cleanup:
    mbedtls_free(buf_a);
    mbedtls_free(buf_e);
    mbedtls_free(buf_n);
    mbedtls_free(buf_r);
    mbedtls_free(buf_primeN);
    mbedtls_free(buf_invR);
    return ret;
}

/*
 * Bignum HW Self-Test — verify modular exponentiation against known values.
 *
 * Tests cover multiple PKA register sizes and edge cases:
 *   #1: 7^13 mod 11 = 2                (64-bit regs, trivial)
 *   #2: 2^256 mod 257 = 1              (64-bit regs, Fermat prime)
 *   #3: (-23)^13 mod 29 = 5            (64-bit regs, negative base)
 *   #4: 512-bit HW vs SW cross-check   (512-bit regs)
 *   #5: 256-bit HW vs SW cross-check   (256-bit regs)
 *   #6: 1024-bit HW vs SW cross-check  (1024-bit regs)
 *   #7: 2048-bit HW vs SW cross-check  (2048-bit regs, RSA-2048 size)
 */

/* Helper: run one exp_mod test with known expected result */
static int bignum_test_kat(int verbose, const char *label,
                           const mbedtls_mpi *A, const mbedtls_mpi *E,
                           const mbedtls_mpi *N, const mbedtls_mpi *expected)
{
    int ret;
    mbedtls_mpi X;
    mbedtls_mpi_init(&X);

    if (verbose)
        printf("  %s: ", label);

    ret = mbedtls_mpi_exp_mod(&X, A, E, N, NULL);
    if (ret != 0 || mbedtls_mpi_cmp_mpi(&X, expected) != 0)
        ret = 1;

    if (verbose)
        printf("%s\n", ret ? "FAILED" : "PASSED");

    mbedtls_mpi_free(&X);
    return ret;
}

/* Helper: run one exp_mod test comparing HW vs SW */
static int bignum_test_cross(int verbose, const char *label,
                              const mbedtls_mpi *A, const mbedtls_mpi *E,
                              const mbedtls_mpi *N)
{
    int ret;
    mbedtls_mpi X_hw, X_sw;
    mbedtls_mpi_init(&X_hw);
    mbedtls_mpi_init(&X_sw);

    if (verbose)
        printf("  %s: ", label);

    ret = mbedtls_mpi_exp_mod(&X_hw, A, E, N, NULL);
    if (ret != 0)
        goto done;

    ret = mbedtls_mpi_exp_mod_original(&X_sw, A, E, N, NULL);
    if (ret != 0)
        goto done;

    if (mbedtls_mpi_cmp_mpi(&X_hw, &X_sw) != 0)
        ret = 1;

done:
    if (verbose)
        printf("%s\n", ret ? "FAILED" : "PASSED");

    mbedtls_mpi_free(&X_hw);
    mbedtls_mpi_free(&X_sw);
    return ret;
}

int mbedtls_bignum_self_test_hw(int verbose)
{
    int ret = 0;
    mbedtls_mpi A, E, N, expected;

    mbedtls_mpi_init(&A);
    mbedtls_mpi_init(&E);
    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&expected);

    /* Test #1: 7^13 mod 11 = 2 (64-bit PKA regs, trivial) */
    mbedtls_mpi_lset(&A, 7);
    mbedtls_mpi_lset(&E, 13);
    mbedtls_mpi_lset(&N, 11);
    mbedtls_mpi_lset(&expected, 2);
    ret = bignum_test_kat(verbose, "Bignum test #1 (7^13 mod 11)",
                          &A, &E, &N, &expected);
    if (ret != 0)
        goto exit;

    /* Test #2: 2^256 mod 257 = 1 (Fermat prime) */
    mbedtls_mpi_lset(&A, 2);
    mbedtls_mpi_lset(&E, 256);
    mbedtls_mpi_lset(&N, 257);
    mbedtls_mpi_lset(&expected, 1);
    ret = bignum_test_kat(verbose, "Bignum test #2 (Fermat 2^256 mod 257)",
                          &A, &E, &N, &expected);
    if (ret != 0)
        goto exit;

    /* Test #3: (-23)^13 mod 29 = 5 (negative base, sign handling) */
    mbedtls_mpi_lset(&A, -23);
    mbedtls_mpi_lset(&E, 13);
    mbedtls_mpi_lset(&N, 29);
    mbedtls_mpi_lset(&expected, 5);
    ret = bignum_test_kat(verbose, "Bignum test #3 (neg base -23^13 mod 29)",
                          &A, &E, &N, &expected);
    if (ret != 0)
        goto exit;

    /* Test #4: 512-bit HW vs SW cross-check */
    mbedtls_mpi_read_string(&A, 16,
        "EFE021C2645FD1DC586E69184AF4A31E"
        "D5F53E93B5F123FA41680867BA110131"
        "944FE7952E2517337780CB0DB80E61AA"
        "E7C8DDC6C5C6AADEB34EB38A2F40D5E6");
    mbedtls_mpi_lset(&E, 65537);
    mbedtls_mpi_read_string(&N, 16,
        "0066A198186C18C10B2F5ED9B522752A"
        "9830B69916E535C8F047518A889A43A5"
        "94B6BED27A168D31D4A52F88925AA8F5");
    ret = bignum_test_cross(verbose, "Bignum test #4 (512-bit HW vs SW)",
                             &A, &E, &N);
    if (ret != 0)
        goto exit;

    /* Test #5: 256-bit HW vs SW cross-check */
    mbedtls_mpi_read_string(&A, 16,
        "B4BCD52DCB4D3689BE256743B443BAE1"
        "3F2694D7C5993B3AEDE483FF4D1E1E67");
    mbedtls_mpi_lset(&E, 65537);
    mbedtls_mpi_read_string(&N, 16,
        "D4BCD52DCB4D3689BE256743B443BAE1"
        "3F2694D7C5993B3AEDE483FF4D1E1E63");
    ret = bignum_test_cross(verbose, "Bignum test #5 (256-bit HW vs SW)",
                             &A, &E, &N);
    if (ret != 0)
        goto exit;

    /* Test #6: 1024-bit HW vs SW cross-check (large PKA regs) */
    mbedtls_mpi_read_string(&A, 16,
        "B4BCD52DCB4D3689BE256743B443BAE1"
        "3F2694D7C5993B3AEDE483FF4D1E1E67"
        "A1B2C3D4E5F6A7B8C9D0E1F2A3B4C5D6"
        "E7F8091A2B3C4D5E6F708192A3B4C5D6"
        "1234567890ABCDEF1234567890ABCDEF"
        "FEDCBA0987654321FEDCBA0987654321"
        "AABBCCDD11223344AABBCCDD11223344"
        "5566778899AABBCC5566778899AABBCF");
    mbedtls_mpi_read_string(&E, 16,
        "010001");
    mbedtls_mpi_read_string(&N, 16,
        "D4BCD52DCB4D3689BE256743B443BAE1"
        "3F2694D7C5993B3AEDE483FF4D1E1E63"
        "C1D2E3F4A5B6C7D8E9F0A1B2C3D4E5F6"
        "A7B8C9D0E1F2A3B4C5D6E7F8091A2B3D"
        "FEDCBA0987654321FEDCBA0987654321"
        "1234567890ABCDEF1234567890ABCDEF"
        "AABBCCDD11223344AABBCCDD11223344"
        "5566778899AABBCC5566778899AABBC9");
    ret = bignum_test_cross(verbose, "Bignum test #6 (1024-bit HW vs SW)",
                             &A, &E, &N);
    if (ret != 0)
        goto exit;

    /* Test #7: 2048-bit HW vs SW cross-check (RSA-2048 size) */
    mbedtls_mpi_read_string(&A, 16,
        "B4BCD52DCB4D3689BE256743B443BAE1"
        "3F2694D7C5993B3AEDE483FF4D1E1E67"
        "A1B2C3D4E5F6A7B8C9D0E1F2A3B4C5D6"
        "E7F8091A2B3C4D5E6F708192A3B4C5D6"
        "1234567890ABCDEF1234567890ABCDEF"
        "FEDCBA0987654321FEDCBA0987654321"
        "AABBCCDD11223344AABBCCDD11223344"
        "5566778899AABBCC5566778899AABBCF"
        "A1B2C3D4E5F6A7B8C9D0E1F2A3B4C5D6"
        "E7F8091A2B3C4D5E6F708192A3B4C5D6"
        "1234567890ABCDEF1234567890ABCDEF"
        "FEDCBA0987654321FEDCBA0987654321"
        "AABBCCDD11223344AABBCCDD11223344"
        "5566778899AABBCC5566778899AABBCF"
        "1234567890ABCDEF1234567890ABCDEF"
        "FEDCBA0987654321FEDCBA09876542FF");
    mbedtls_mpi_read_string(&E, 16,
        "010001");
    mbedtls_mpi_read_string(&N, 16,
        "D4BCD52DCB4D3689BE256743B443BAE1"
        "3F2694D7C5993B3AEDE483FF4D1E1E63"
        "C1D2E3F4A5B6C7D8E9F0A1B2C3D4E5F6"
        "A7B8C9D0E1F2A3B4C5D6E7F8091A2B3D"
        "FEDCBA0987654321FEDCBA0987654321"
        "1234567890ABCDEF1234567890ABCDEF"
        "AABBCCDD11223344AABBCCDD11223344"
        "5566778899AABBCC5566778899AABBC9"
        "C1D2E3F4A5B6C7D8E9F0A1B2C3D4E5F6"
        "A7B8C9D0E1F2A3B4C5D6E7F8091A2B3D"
        "FEDCBA0987654321FEDCBA0987654321"
        "1234567890ABCDEF1234567890ABCDEF"
        "AABBCCDD11223344AABBCCDD11223344"
        "5566778899AABBCC5566778899AABBC9"
        "1234567890ABCDEF1234567890ABCDEF"
        "FEDCBA0987654321FEDCBA09876542C1");
    ret = bignum_test_cross(verbose, "Bignum test #7 (2048-bit HW vs SW)",
                             &A, &E, &N);
    if (ret != 0)
        goto exit;

exit:
    mbedtls_mpi_free(&A);
    mbedtls_mpi_free(&E);
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&expected);
    return ret;
}
