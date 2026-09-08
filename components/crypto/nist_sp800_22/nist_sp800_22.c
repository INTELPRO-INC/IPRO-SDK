/**
 * @file nist_sp800_22.c
 * @brief NIST SP 800-22 Rev 1a Statistical Test Suite Implementation
 *
 * Implements all 15 statistical tests for random number generator validation.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 IPRO Technology Co., Ltd.
 */

#include "nist_sp800_22.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include "compiler/common.h"

/* ============================================================================
 * Mathematical Constants
 * ============================================================================ */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

#define MACHEP  1.11022302462515654042E-16   /* 2**-53 */
#define MAXLOG  7.09782712893383996732E2     /* log(DBL_MAX) */
#define MAXNUM  1.79769313486231570815E308   /* DBL_MAX */
#define MINLOG  (-7.451332191019412076235E2) /* log(DBL_MIN) */

/* ============================================================================
 * Internal Helper Macros
 * ============================================================================ */

/** Get bit at position pos from data array (MSB first within each byte) */
#define GET_BIT(data, pos) (((data)[(pos) >> 3] >> (7 - ((pos) & 7))) & 1)

/** Minimum of two values */
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/** Maximum of two values */
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/* ============================================================================
 * Debug Buffer System - Only print debug info on test failure
 * ============================================================================ */

#define NIST_DBG_BUF_SIZE 512
static ATTR_PSRAM_SECTION char g_nist_dbg_buf[NIST_DBG_BUF_SIZE];
static int g_nist_dbg_pos = 0;

/** Clear debug buffer */
static void nist_dbg_clear(void)
{
    g_nist_dbg_pos = 0;
    g_nist_dbg_buf[0] = '\0';
}

/** Append to debug buffer (like snprintf) */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
__attribute__((format(printf, 1, 2)))
static void nist_dbg_printf(const char *fmt, ...)
{
    va_list ap;
    int len;

    if (g_nist_dbg_pos >= NIST_DBG_BUF_SIZE - 1) {
        return;
    }

    va_start(ap, fmt);
    len = vsnprintf(g_nist_dbg_buf + g_nist_dbg_pos,
                    NIST_DBG_BUF_SIZE - g_nist_dbg_pos, fmt, ap);
    va_end(ap);

    if (len > 0) {
        g_nist_dbg_pos += len;
        if (g_nist_dbg_pos >= NIST_DBG_BUF_SIZE) {
            g_nist_dbg_pos = NIST_DBG_BUF_SIZE - 1;
        }
    }
}
#pragma GCC diagnostic pop

/** Print debug buffer if test failed */
static void nist_dbg_flush_on_fail(int ret)
{
    if (ret != NIST_SUCCESS && g_nist_dbg_pos > 0) {
        printf("%s", g_nist_dbg_buf);
    }
    nist_dbg_clear();
}

/**
 * @brief Print P-value with appropriate format based on magnitude
 */
static void print_pvalue(double p, int ret, const char *pass_str, const char *fail_str)
{
    const char *status = (ret == NIST_SUCCESS) ? pass_str : fail_str;

    /* Flush debug buffer if test failed */
    nist_dbg_flush_on_fail(ret);

    /* Check for invalid values: NaN fails p == p, Inf is outside [0,1] */
    if (p != p || p < 0.0 || p > 1.0) {
        printf(" P=INVALID %s\n", status);
    } else if (p < 1e-10) {
        printf(" P<1e-10 %s\n", status);
    } else if (p < 0.0001) {
        printf(" P=%.2e %s\n", p, status);
    } else {
        printf(" P=%.4f %s\n", p, status);
    }
}

/* ============================================================================
 * Mathematical Helper Functions
 * ============================================================================ */

/**
 * @brief Complementary error function (erfc)
 */
static double nist_erfc(double x)
{
    static const double a1 =  0.254829592;
    static const double a2 = -0.284496736;
    static const double a3 =  1.421413741;
    static const double a4 = -1.453152027;
    static const double a5 =  1.061405429;
    static const double p  =  0.3275911;

    int sign = (x < 0) ? -1 : 1;
    x = fabs(x);

    double t = 1.0 / (1.0 + p * x);
    double t2 = t * t;
    double t3 = t2 * t;
    double t4 = t3 * t;
    double t5 = t4 * t;

    double y = 1.0 - (a1*t + a2*t2 + a3*t3 + a4*t4 + a5*t5) * exp(-x*x);

    return (sign == 1) ? (1.0 - y) : (1.0 + y);
}

/**
 * @brief Standard normal CDF (Phi function)
 */
static double nist_phi(double x)
{
    return 0.5 * nist_erfc(-x / M_SQRT2);
}

/**
 * @brief Log gamma function using Lanczos approximation
 */
static double nist_lgamma(double x)
{
    static const double c[8] = {
        676.5203681218851,
        -1259.1392167224028,
        771.32342877765313,
        -176.61502916214059,
        12.507343278686905,
        -0.13857109526572012,
        9.9843695780195716e-6,
        1.5056327351493116e-7
    };

    if (x < 0.5) {
        return log(M_PI / sin(M_PI * x)) - nist_lgamma(1.0 - x);
    }

    x -= 1.0;
    double a = 0.99999999999980993;
    for (int i = 0; i < 8; i++) {
        a += c[i] / (x + i + 1);
    }

    double t = x + 7.5;
    return 0.5 * log(2.0 * M_PI) + (x + 0.5) * log(t) - t + log(a);
}

/**
 * @brief Incomplete gamma function series expansion
 */
static double nist_igam_series(double a, double x)
{
    double sum, term, pn;

    if (x == 0.0) return 0.0;

    double ax = a * log(x) - x - nist_lgamma(a);
    if (ax < MINLOG) return 0.0;
    ax = exp(ax);

    sum = 1.0 / a;
    term = sum;
    pn = a;

    /*
     * Iterate to full convergence like the Cephes reference. Near x ~ a
     * with large a (e.g. the Serial test: a = 2^14) the term ratio
     * x/(a+k) stays close to 1 for thousands of iterations; a small
     * iteration cap silently truncates the series and skews the P-value
     * by ~1e-3.
     */
    for (int i = 0; i < 500000; i++) {
        pn += 1.0;
        term *= x / pn;
        sum += term;
        if (fabs(term) < fabs(sum) * MACHEP) break;
    }

    return sum * ax;
}

/**
 * @brief Incomplete gamma function continued fraction
 */
static double nist_igamc_cf(double a, double x)
{
    double ax = a * log(x) - x - nist_lgamma(a);
    if (ax < MINLOG) return 0.0;
    ax = exp(ax);

    double y = 1.0 - a;
    double z = x + y + 1.0;
    double c = 0.0;
    double pkm2 = 1.0;
    double qkm2 = x;
    double pkm1 = x + 1.0;
    double qkm1 = z * x;
    double ans = pkm1 / qkm1;

    /* Iterate to full convergence (see nist_igam_series) */
    for (int i = 0; i < 500000; i++) {
        c += 1.0;
        y += 1.0;
        z += 2.0;
        double yc = y * c;
        double pk = pkm1 * z - pkm2 * yc;
        double qk = qkm1 * z - qkm2 * yc;

        if (qk != 0.0) {
            double r = pk / qk;
            double t = fabs((ans - r) / r);
            ans = r;
            if (t < MACHEP) break;
        }

        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;

        if (fabs(pk) > 1e30) {
            pkm2 *= MACHEP;
            pkm1 *= MACHEP;
            qkm2 *= MACHEP;
            qkm1 *= MACHEP;
        }
    }

    return ans * ax;
}

/**
 * @brief Regularized incomplete gamma function (upper) Q(a,x) = 1 - P(a,x)
 */
static double nist_igamc(double a, double x)
{
    if (x <= 0.0 || a <= 0.0) return 1.0;
    if (x < 1.0 || x < a) return 1.0 - nist_igam_series(a, x);
    return nist_igamc_cf(a, x);
}

/**
 * @brief Regularized incomplete gamma function (lower) P(a,x)
 * @note Currently unused but kept for completeness and future tests
 */
__attribute__((unused))
static double nist_igam(double a, double x)
{
    if (x <= 0.0 || a <= 0.0) return 0.0;
    if (x < 1.0 || x < a) return nist_igam_series(a, x);
    return 1.0 - nist_igamc_cf(a, x);
}

/* ============================================================================
 * Bit Manipulation Helpers
 * ============================================================================ */

/**
 * @brief Count number of ones in bit sequence
 */
static uint32_t count_ones(const uint8_t *data, uint32_t n_bits)
{
    uint32_t count = 0;
    uint32_t full_bytes = n_bits >> 3;
    uint32_t remaining = n_bits & 7;

    /* Count full bytes using lookup or popcount */
    for (uint32_t i = 0; i < full_bytes; i++) {
        uint8_t b = data[i];
        /* Brian Kernighan's algorithm */
        while (b) {
            count++;
            b &= b - 1;
        }
    }

    /* Count remaining bits */
    if (remaining > 0) {
        uint8_t mask = (uint8_t)(0xFF << (8 - remaining));
        uint8_t b = data[full_bytes] & mask;
        while (b) {
            count++;
            b &= b - 1;
        }
    }

    return count;
}

/**
 * @brief Get pattern value from bit stream
 */
static uint32_t get_pattern(const uint8_t *data, uint32_t start_bit, uint32_t len)
{
    uint32_t pattern = 0;
    for (uint32_t i = 0; i < len; i++) {
        pattern = (pattern << 1) | GET_BIT(data, start_bit + i);
    }
    return pattern;
}

/* ============================================================================
 * Matrix Operations (for Binary Matrix Rank Test)
 * ============================================================================ */

#define MATRIX_SIZE 32

/**
 * @brief Compute rank of a binary matrix using Gaussian elimination
 */
static int compute_matrix_rank(uint32_t matrix[MATRIX_SIZE], int rows, int cols)
{
    int rank = 0;
    int piv_row, piv_col;

    for (piv_col = cols - 1, piv_row = 0; piv_col >= 0 && piv_row < rows; piv_col--) {
        /* Find pivot */
        int found = 0;
        for (int i = piv_row; i < rows; i++) {
            if ((matrix[i] >> piv_col) & 1) {
                /* Swap rows */
                if (i != piv_row) {
                    uint32_t temp = matrix[piv_row];
                    matrix[piv_row] = matrix[i];
                    matrix[i] = temp;
                }
                found = 1;
                break;
            }
        }

        if (!found) continue;

        rank++;

        /* Eliminate */
        for (int i = piv_row + 1; i < rows; i++) {
            if ((matrix[i] >> piv_col) & 1) {
                matrix[i] ^= matrix[piv_row];
            }
        }

        piv_row++;
    }

    return rank;
}

/* ============================================================================
 * FFT for Spectral Test
 * ============================================================================ */

/**
 * @brief In-place iterative radix-2 complex FFT (decimation in time)
 *
 * re[]/im[] hold N complex values; N must be a power of 2. Twiddles come
 * from a precomputed table (w^j for j < N/2, built with a double-precision
 * recurrence re-anchored from sin/cos every 256 steps), and the butterfly
 * loops walk memory sequentially (block-outer): the multi-MB working set
 * lives in PSRAM, where the previous twiddle-outer strided walk amplified
 * cache traffic ~8x and dominated the test's runtime.
 *
 * @return NIST_SUCCESS, or NIST_ERROR_NO_MEMORY if the table allocation fails
 */
static int fft_radix2(float *re, float *im, uint32_t N)
{
    uint32_t half_n = N / 2;

    float *tw_re = (float *)malloc(half_n * sizeof(float));
    float *tw_im = (float *)malloc(half_n * sizeof(float));
    if (!tw_re || !tw_im) {
        free(tw_re);
        free(tw_im);
        return NIST_ERROR_NO_MEMORY;
    }

    /* Twiddle table: w^j = e^{-2*pi*i*j/N} */
    {
        double ang = -2.0 * M_PI / (double)N;
        double step_re = cos(ang), step_im = sin(ang);
        double w_re = 1.0, w_im = 0.0;
        for (uint32_t j = 0; j < half_n; j++) {
            if ((j & 255) == 0) {
                w_re = cos(ang * (double)j);
                w_im = sin(ang * (double)j);
            }
            tw_re[j] = (float)w_re;
            tw_im[j] = (float)w_im;
            double nw_re = w_re * step_re - w_im * step_im;
            w_im = w_re * step_im + w_im * step_re;
            w_re = nw_re;
        }
    }

    /* Bit-reversal permutation */
    for (uint32_t i = 1, j = 0; i < N; i++) {
        uint32_t bit = N >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            float t = re[i]; re[i] = re[j]; re[j] = t;
            t = im[i]; im[i] = im[j]; im[j] = t;
        }
    }

    /* Butterfly stages: block-outer, sequential i/j streams per block */
    for (uint32_t len = 2; len <= N; len <<= 1) {
        uint32_t half = len >> 1;
        uint32_t stride = N / len;

        for (uint32_t base = 0; base < N; base += len) {
            uint32_t ti = 0;
            for (uint32_t k = 0; k < half; k++, ti += stride) {
                uint32_t i = base + k;
                uint32_t j = i + half;
                float wr = tw_re[ti], wi = tw_im[ti];
                float tr = re[j] * wr - im[j] * wi;
                float tim = re[j] * wi + im[j] * wr;
                re[j] = re[i] - tr;
                im[j] = im[i] - tim;
                re[i] += tr;
                im[i] += tim;
            }
        }
    }

    free(tw_re);
    free(tw_im);
    return NIST_SUCCESS;
}

/* ============================================================================
 * Berlekamp-Massey Algorithm (for Linear Complexity Test)
 * ============================================================================ */

/**
 * @brief Berlekamp-Massey algorithm to find linear complexity
 */
static int berlekamp_massey(const uint8_t *data, uint32_t start_bit, uint32_t n)
{
    int L = 0;
    int m = -1;
    int d;

    /* Allocate arrays for LFSR computation */
    int8_t *B = (int8_t *)calloc(n + 1, sizeof(int8_t));
    int8_t *C = (int8_t *)calloc(n + 1, sizeof(int8_t));
    int8_t *T = (int8_t *)calloc(n + 1, sizeof(int8_t));

    if (!B || !C || !T) {
        free(B);
        free(C);
        free(T);
        return -1;
    }

    B[0] = 1;
    C[0] = 1;

    for (uint32_t N = 0; N < n; N++) {
        /* Compute discrepancy */
        d = GET_BIT(data, start_bit + N);
        for (int i = 1; i <= L; i++) {
            d ^= C[i] & GET_BIT(data, start_bit + N - i);
        }

        if (d == 1) {
            memcpy(T, C, (n + 1) * sizeof(int8_t));

            for (int i = 0; i <= (int)n - (int)N + m; i++) {
                C[N - m + i] ^= B[i];
            }

            if (L <= (int)N / 2) {
                L = N + 1 - L;
                m = N;
                memcpy(B, T, (n + 1) * sizeof(int8_t));
            }
        }
    }

    free(B);
    free(C);
    free(T);

    return L;
}

/* ============================================================================
 * Test Implementations
 * ============================================================================ */

int nist_frequency_test(const uint8_t *data, uint32_t n_bits, double *p_value)
{
    if (!data || n_bits < 100 || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    uint32_t ones = count_ones(data, n_bits);
    int32_t S_n = (int32_t)(2 * ones) - (int32_t)n_bits;
    double s_obs = fabs((double)S_n) / sqrt((double)n_bits);

    *p_value = nist_erfc(s_obs / M_SQRT2);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_block_frequency_test(const uint8_t *data, uint32_t n_bits, uint32_t M, double *p_value)
{
    if (!data || n_bits < 100 || M < 20 || M > n_bits || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    uint32_t N = n_bits / M;
    if (N < 1) return NIST_ERROR_INSUFFICIENT_DATA;

    double chi_sq = 0.0;

    for (uint32_t i = 0; i < N; i++) {
        uint32_t block_ones = 0;
        uint32_t start = i * M;

        for (uint32_t j = 0; j < M; j++) {
            block_ones += GET_BIT(data, start + j);
        }

        double pi = (double)block_ones / M;
        chi_sq += (pi - 0.5) * (pi - 0.5);
    }

    chi_sq *= 4.0 * M;

    *p_value = nist_igamc((double)N / 2.0, chi_sq / 2.0);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_runs_test(const uint8_t *data, uint32_t n_bits, double *p_value)
{
    if (!data || n_bits < 100 || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Pre-test: frequency test must pass first */
    uint32_t ones = count_ones(data, n_bits);
    double pi = (double)ones / n_bits;

    double tau = 2.0 / sqrt((double)n_bits);
    if (fabs(pi - 0.5) >= tau) {
        *p_value = 0.0;
        return NIST_ERROR_PRECONDITION;
    }

    /* Count runs */
    uint32_t V_n = 1;
    for (uint32_t i = 1; i < n_bits; i++) {
        if (GET_BIT(data, i) != GET_BIT(data, i - 1)) {
            V_n++;
        }
    }

    double num = fabs((double)V_n - 2.0 * n_bits * pi * (1.0 - pi));
    double den = 2.0 * sqrt(2.0 * n_bits) * pi * (1.0 - pi);

    *p_value = nist_erfc(num / den);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_longest_run_test(const uint8_t *data, uint32_t n_bits, double *p_value)
{
    if (!data || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Select parameters based on n_bits (NIST Table 2.4) */
    int M, K, N;
    const double *pi_table;
    int v_offset;

    /* Table for M=8 (n >= 128) */
    static const double pi_8[] = {0.2148, 0.3672, 0.2305, 0.1875};

    /* Table for M=128 (n >= 6272) */
    static const double pi_128[] = {0.1174, 0.2430, 0.2493, 0.1752, 0.1027, 0.1124};

    /* Table for M=10000 (n >= 750000) */
    static const double pi_10000[] = {0.0882, 0.2092, 0.2483, 0.1933, 0.1208, 0.0675, 0.0727};

    if (n_bits < 128) {
        return NIST_ERROR_INSUFFICIENT_DATA;
    } else if (n_bits < 6272) {
        M = 8;
        K = 3;
        pi_table = pi_8;
        v_offset = 1;
    } else if (n_bits < 750000) {
        M = 128;
        K = 5;
        pi_table = pi_128;
        v_offset = 4;
    } else {
        M = 10000;
        K = 6;
        pi_table = pi_10000;
        v_offset = 10;
    }

    N = n_bits / M;

    /* Count longest runs in each block */
    uint32_t *v = (uint32_t *)calloc(K + 1, sizeof(uint32_t));
    if (!v) return NIST_ERROR_NO_MEMORY;

    for (int i = 0; i < N; i++) {
        uint32_t start = i * M;
        int longest = 0, current = 0;

        for (int j = 0; j < M; j++) {
            if (GET_BIT(data, start + j)) {
                current++;
                if (current > longest) longest = current;
            } else {
                current = 0;
            }
        }

        /* Classify into categories */
        int category;
        if (longest <= v_offset) {
            category = 0;
        } else if (longest >= v_offset + K) {
            category = K;
        } else {
            category = longest - v_offset;
        }
        v[category]++;
    }

    /* Compute chi-squared statistic */
    double chi_sq = 0.0;
    for (int i = 0; i <= K; i++) {
        double expected = N * pi_table[i];
        double diff = (double)v[i] - expected;
        chi_sq += (diff * diff) / expected;
    }

    free(v);

    *p_value = nist_igamc((double)K / 2.0, chi_sq / 2.0);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_matrix_rank_test(const uint8_t *data, uint32_t n_bits, double *p_value)
{
    if (!data || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    const int M = MATRIX_SIZE;
    const int Q = MATRIX_SIZE;
    uint32_t bits_per_matrix = M * Q;
    uint32_t N = n_bits / bits_per_matrix;

    if (N < 38) {
        return NIST_ERROR_INSUFFICIENT_DATA;
    }

    /* Count matrices by rank */
    uint32_t F_M = 0;      /* Full rank (32) */
    uint32_t F_M1 = 0;     /* Rank 31 */
    uint32_t F_other = 0;  /* Rank <= 30 */

    uint32_t matrix[MATRIX_SIZE];

    for (uint32_t k = 0; k < N; k++) {
        uint32_t bit_offset = k * bits_per_matrix;

        /* Fill matrix row by row */
        for (int i = 0; i < M; i++) {
            matrix[i] = 0;
            for (int j = 0; j < Q; j++) {
                matrix[i] = (matrix[i] << 1) | GET_BIT(data, bit_offset + i * Q + j);
            }
        }

        int rank = compute_matrix_rank(matrix, M, Q);

        if (rank == M) {
            F_M++;
        } else if (rank == M - 1) {
            F_M1++;
        } else {
            F_other++;
        }
    }

    /*
     * Exact rank probabilities for 32x32 matrices, computed with the
     * product formula the sts reference uses (rank.c). The old 4-digit
     * literals (0.2888/0.5776/0.1336) shifted the P-value by ~2e-3.
     *   p_r = 2^(r*(64-r)-1024) * prod_{i=0}^{r-1} (1-2^(i-32))^2/(1-2^(i-r))
     */
    double p_32 = 1.0;
    for (int i = 0; i < 32; i++) {
        p_32 *= 1.0 - pow(2.0, i - 32);       /* r=32: exponent term is 2^0 */
    }
    double p_31 = 0.5;                        /* r=31: 2^(31*33-1024) = 2^-1 */
    for (int i = 0; i < 31; i++) {
        double num = 1.0 - pow(2.0, i - 32);
        p_31 *= num * num / (1.0 - pow(2.0, i - 31));
    }
    double p_30 = 1.0 - (p_32 + p_31);

    /* Chi-squared statistic */
    double chi_sq = 0.0;
    chi_sq += pow((double)F_M - N * p_32, 2) / (N * p_32);
    chi_sq += pow((double)F_M1 - N * p_31, 2) / (N * p_31);
    chi_sq += pow((double)F_other - N * p_30, 2) / (N * p_30);

    *p_value = exp(-chi_sq / 2.0);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

/**
 * @brief 2.6 DFT (Spectral) Test - full-length FFT with the fixed NIST threshold
 *
 * Computes the DFT of the whole sequence (packed-real radix-2 FFT) and counts
 * peaks below the fixed theoretical threshold T = sqrt(ln(1/0.05) * n), per
 * NIST SP 800-22 Rev 1a Section 2.6 / sts-2.1.2 reference code.
 *
 * A radix-2 FFT needs a power-of-2 length, so the largest power-of-2 prefix
 * of the sequence is tested. With the default n_bits = 1048576 (2^20) every
 * bit is used; for other lengths the effective n is reported via debug output.
 */
int nist_spectral_test(const uint8_t *data, uint32_t n_bits, double *p_value)
{
    if (!data || n_bits < 1000 || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Largest power of 2 <= n_bits */
    uint32_t n = n_bits;
    while (n & (n - 1)) {
        n &= n - 1;
    }

    uint32_t N = n / 2;   /* complex length for the packed real FFT */

    float *re = (float *)malloc(N * sizeof(float));
    float *im = (float *)malloc(N * sizeof(float));
    if (!re || !im) {
        free(re);
        free(im);
        return NIST_ERROR_NO_MEMORY;
    }

    /* Pack the +/-1 sequence: even samples -> re, odd samples -> im */
    for (uint32_t i = 0; i < N; i++) {
        re[i] = GET_BIT(data, 2 * i) ? 1.0f : -1.0f;
        im[i] = GET_BIT(data, 2 * i + 1) ? 1.0f : -1.0f;
    }

    int fft_ret = fft_radix2(re, im, N);
    if (fft_ret != NIST_SUCCESS) {
        free(re);
        free(im);
        return fft_ret;
    }

    /*
     * Untangle the packed transform. With E/O the DFTs of the even/odd
     * subsequences (Z_k = E_k + i*O_k, Z_N == Z_0):
     *   E_k = (Z_k + conj(Z_{N-k})) / 2
     *   O_k = -i * (Z_k - conj(Z_{N-k})) / 2
     *   X_k = E_k + e^{-2*pi*i*k/n} * O_k,  k = 0 .. N-1
     * Count |X_k| < T over the first n/2 bins (DC included, as in sts).
     */
    double T = sqrt(2.995732274 * (double)n);   /* sqrt(ln(1/0.05) * n) */
    uint32_t N_0 = 0;

    double ang = -2.0 * M_PI / (double)n;
    double wstep_re = cos(ang), wstep_im = sin(ang);
    double w_re = 1.0, w_im = 0.0;

    for (uint32_t k = 0; k < N; k++) {
        uint32_t kr = (k == 0) ? 0 : (N - k);
        double zr = re[k], zi = im[k];
        double yr = re[kr], yi = -im[kr];        /* conj(Z_{N-k}) */
        double er = 0.5 * (zr + yr);
        double ei = 0.5 * (zi + yi);
        double odr = 0.5 * (zi - yi);            /* Re(O_k) */
        double odi = -0.5 * (zr - yr);           /* Im(O_k) */
        double xr = er + w_re * odr - w_im * odi;
        double xi = ei + w_re * odi + w_im * odr;

        if (sqrt(xr * xr + xi * xi) < T) N_0++;

        double nw_re = w_re * wstep_re - w_im * wstep_im;
        w_im = w_re * wstep_im + w_im * wstep_re;
        w_re = nw_re;
    }

    free(re);
    free(im);

    /* d = (N_0 - 0.95*n/2) / sqrt(n * 0.95 * 0.05 / 4), Section 2.6 (5) */
    double expected_N0 = 0.95 * (double)N;
    double d = ((double)N_0 - expected_N0) / sqrt((double)n * 0.95 * 0.05 / 4.0);
    *p_value = nist_erfc(fabs(d) / M_SQRT2);

    nist_dbg_printf("\n  [DFT] n_used=%lu/%lu, T=%.2f, N_0=%lu/%lu, d=%.4f\n",
           (unsigned long)n, (unsigned long)n_bits, T,
           (unsigned long)N_0, (unsigned long)N, d);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

/**
 * @brief Count greedy non-overlapping matches of every template in one pass
 *
 * Slides one m-bit window across each M-bit block and resolves the window
 * value to a template via a 2^m lookup table (templates are distinct m-bit
 * values), so ALL templates are counted in a single sweep of the data
 * instead of one full scan per template. The reference semantics -- greedy
 * left-to-right matching where a match consumes m bits (scan resumes at
 * j+m) -- are preserved exactly per template with a per-template
 * next-allowed position: checking every j but suppressing matches with
 * j < next_allowed is identical to the skip-on-match walk.
 *
 * W is laid out as W[t * N + blk] and must hold num_templates * N entries.
 */
static int non_overlap_count_all(const uint8_t *data, uint32_t m,
                                 uint32_t M, uint32_t N,
                                 const uint16_t *templates, int num_templates,
                                 uint32_t *W)
{
    uint32_t lut_size = 1U << m;

    int16_t *lut = (int16_t *)malloc(lut_size * sizeof(int16_t));
    uint32_t *next_ok = (uint32_t *)malloc((size_t)num_templates * sizeof(uint32_t));
    if (!lut || !next_ok) {
        free(lut);
        free(next_ok);
        return NIST_ERROR_NO_MEMORY;
    }

    for (uint32_t i = 0; i < lut_size; i++) {
        lut[i] = -1;
    }
    for (int t = 0; t < num_templates; t++) {
        lut[templates[t]] = (int16_t)t;
    }

    memset(W, 0, (size_t)num_templates * N * sizeof(uint32_t));
    uint32_t mask = lut_size - 1;

    for (uint32_t blk = 0; blk < N; blk++) {
        uint32_t start = blk * M;
        memset(next_ok, 0, (size_t)num_templates * sizeof(uint32_t));

        /* Prime the window with the first m-1 bits of the block */
        uint32_t win = 0;
        for (uint32_t b = 0; b + 1 < m; b++) {
            win = (win << 1) | GET_BIT(data, start + b);
        }

        for (uint32_t j = 0; j + m <= M; j++) {
            win = ((win << 1) | GET_BIT(data, start + j + m - 1)) & mask;
            int t = lut[win];
            if (t >= 0 && j >= next_ok[t]) {
                W[(uint32_t)t * N + blk]++;
                next_ok[t] = j + m;
            }
        }
    }

    free(lut);
    free(next_ok);
    return NIST_SUCCESS;
}

#ifdef CONFIG_NIST_FULL_TEST
/**
 * @brief NIST SP 800-22 official aperiodic templates for m=9
 *
 * These 148 templates are the official NIST aperiodic templates from
 * the NIST Statistical Test Suite (sts/templates/template9).
 * They are NOT simply all aperiodic patterns, but a specific subset
 * selected by NIST for the non-overlapping template matching test.
 */
static const uint16_t nist_templates_m9[148] = {
    /* Group 1: Patterns ending in 1 (74 templates) */
    0x001, 0x003, 0x005, 0x007, 0x009, 0x00B, 0x00D, 0x00F,
    0x011, 0x013, 0x015, 0x017, 0x019, 0x01B, 0x01D, 0x01F,
    0x023, 0x025, 0x027, 0x029, 0x02B, 0x02D, 0x02F, 0x033,
    0x035, 0x037, 0x039, 0x03B, 0x03D, 0x03F, 0x043, 0x045,
    0x047, 0x04B, 0x04D, 0x04F, 0x053, 0x055, 0x057, 0x05B,
    0x05D, 0x05F, 0x065, 0x067, 0x06B, 0x06D, 0x06F, 0x075,
    0x077, 0x07B, 0x07D, 0x07F, 0x083, 0x087, 0x08B, 0x08F,
    0x093, 0x097, 0x09B, 0x09F, 0x0A3, 0x0A7, 0x0AB, 0x0AF,
    0x0B3, 0x0B7, 0x0BB, 0x0BF, 0x0C7, 0x0CF, 0x0D7, 0x0DF,
    0x0EF, 0x0FF,
    /* Group 2: Patterns starting with 1, ending in 0 (74 templates) */
    0x100, 0x110, 0x120, 0x128, 0x130, 0x138, 0x140, 0x144,
    0x148, 0x14C, 0x150, 0x154, 0x158, 0x15C, 0x160, 0x164,
    0x168, 0x16C, 0x170, 0x174, 0x178, 0x17C, 0x180, 0x182,
    0x184, 0x188, 0x18A, 0x190, 0x192, 0x194, 0x198, 0x19A,
    0x1A0, 0x1A2, 0x1A4, 0x1A8, 0x1AA, 0x1AC, 0x1B0, 0x1B2,
    0x1B4, 0x1B8, 0x1BA, 0x1BC, 0x1C0, 0x1C2, 0x1C4, 0x1C6,
    0x1C8, 0x1CA, 0x1CC, 0x1D0, 0x1D2, 0x1D4, 0x1D6, 0x1D8,
    0x1DA, 0x1DC, 0x1E0, 0x1E2, 0x1E4, 0x1E6, 0x1E8, 0x1EA,
    0x1EC, 0x1EE, 0x1F0, 0x1F2, 0x1F4, 0x1F6, 0x1F8, 0x1FA,
    0x1FC, 0x1FE
};
#define NIST_TEMPLATES_M9_COUNT 148
#endif /* CONFIG_NIST_FULL_TEST */

/**
 * @brief Non-overlapping template worker with caller-selectable alpha
 *
 * The public nist_non_overlapping_template_test_ex() wraps this with
 * NIST_DEFAULT_ALPHA; the suite runner passes the caller's alpha through.
 */
static int non_overlapping_template_run(const uint8_t *data, uint32_t n_bits,
                                        uint32_t m, int early_exit, double alpha,
                                        double *p_value,
                                        int *out_passed_count, int *out_total_count,
                                        double *out_pvals)
{
    /*
     * Both template sets are 9-bit values from the official NIST m=9
     * aperiodic template file; running them against any other window
     * length would be statistically meaningless, so only m=9 is accepted.
     */
    if (!data || !p_value || m != 9) {
        if (p_value) *p_value = 0.0;
        if (out_passed_count) *out_passed_count = 0;
        if (out_total_count) *out_total_count = 0;
        return NIST_ERROR_INVALID_PARAM;
    }

#ifdef CONFIG_NIST_FULL_TEST
    /* Full test: all 148 official NIST m=9 aperiodic templates */
    const uint16_t *templates = nist_templates_m9;
    const int num_templates = NIST_TEMPLATES_M9_COUNT;
#else
    /*
     * Quick test mode: 10 representative templates, all taken from the
     * official NIST m=9 aperiodic template set (template9). Former entries
     * 0x095/0x12B were NOT aperiodic (prefix == suffix self-overlap), which
     * invalidates the test's mu/sigma^2 theory for them -- replaced by the
     * official 0x093/0x128.
     */
    static const uint16_t quick_templates[] = {
        0x001, 0x003, 0x005, 0x00B, 0x00D,
        0x013, 0x025, 0x04B, 0x093, 0x128,
    };
    const uint16_t *templates = quick_templates;
    const int num_templates = (int)(sizeof(quick_templates) / sizeof(quick_templates[0]));
#endif

    /*
     * NIST recommends M such that N >= 8 blocks.
     */
    uint32_t M = 131072;  /* Default block size: 2^17 = 131072 */
    if (n_bits < M * 8) {
        M = n_bits / 8;
        if (M < 1024) M = 1024;
    }
    if (M > n_bits) M = n_bits;

    uint32_t N = n_bits / M;
    if (N < 1 || M < m) return NIST_ERROR_INSUFFICIENT_DATA;

    /* Theoretical values per NIST SP 800-22 Section 2.7 */
    double mu = (double)(M - m + 1) / pow(2.0, m);
    double sigma_sq = M * (1.0 / pow(2.0, m) - (2.0 * m - 1.0) / pow(2.0, 2.0 * m));

    if (sigma_sq <= 0.0) {
        *p_value = 0.0;
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Count every template's per-block matches in one pass over the data */
    uint32_t *W = (uint32_t *)malloc((size_t)num_templates * N * sizeof(uint32_t));
    if (!W) {
        return NIST_ERROR_NO_MEMORY;
    }
    int cret = non_overlap_count_all(data, m, M, N, templates, num_templates, W);
    if (cret != NIST_SUCCESS) {
        free(W);
        return cret;
    }

    /* Evaluate each template, track minimum P-value */
    double min_pvalue = 1.0;
    uint32_t min_template = 0;
    int failed_count = 0;
    int tested_count = 0;

    nist_dbg_printf("\n  [DBG] Non-overlap: N=%lu, M=%lu, m=%lu, templates=%d%s\n",
           (unsigned long)N, (unsigned long)M, (unsigned long)m, num_templates,
           early_exit ? " (early-exit)" : "");
    nist_dbg_printf("  [DBG] mu=%.2f, sigma_sq=%.2f\n", mu, sigma_sq);

    for (int t = 0; t < num_templates; t++) {
        uint32_t B = templates[t];
        double chi_sq = 0.0;
        for (uint32_t blk = 0; blk < N; blk++) {
            double d = (double)W[(uint32_t)t * N + blk] - mu;
            chi_sq += d * d / sigma_sq;
        }
        double pval = nist_igamc((double)N / 2.0, chi_sq / 2.0);
        tested_count++;
        if (out_pvals) out_pvals[t] = pval;

        if (pval < min_pvalue) {
            min_pvalue = pval;
            min_template = B;
        }

        if (pval < alpha) {
            failed_count++;
            nist_dbg_printf("  [DBG] Template 0x%03lx: P=%.6f FAIL (chi_sq=%.2f)\n",
                   (unsigned long)B, pval, chi_sq);
            /* Early exit on first failure if requested */
            if (early_exit) {
                nist_dbg_printf("  [DBG] Early exit: %d/%d passed before failure\n",
                       tested_count - failed_count, tested_count);
                break;
            }
        } else if (tested_count <= 5) {
            nist_dbg_printf("  [DBG] Template 0x%03lx: P=%.6f PASS\n",
                   (unsigned long)B, pval);
        }
    }

    free(W);

    int pass_count = tested_count - failed_count;
    nist_dbg_printf("  [DBG] Result: %d/%d passed, min P=%.6f (template 0x%03lx)\n",
           pass_count, tested_count, min_pvalue, (unsigned long)min_template);

    *p_value = min_pvalue;
    if (out_passed_count) *out_passed_count = pass_count;
    if (out_total_count) *out_total_count = tested_count;

    /*
     * NIST SP 800-22 Section 4.2.1: Proportion of Sequences Passing a Test
     *
     * The acceptable proportion is: p_hat = (1 - alpha) +/- 3 * sqrt(alpha * (1 - alpha) / N)
     * For alpha = 0.01 and N = 148 templates:
     *   threshold = 0.99 - 3 * sqrt(0.01 * 0.99 / 148)
     *             = 0.99 - 3 * sqrt(0.0000669)
     *             = 0.99 - 3 * 0.00818
     *             = 0.99 - 0.0245
     *             = 0.9655 (96.55%)
     *
     * So for 148 templates, at least 143 must pass (148 * 0.9655 = 142.9)
     */
    double threshold = (1.0 - alpha) - 3.0 * sqrt(alpha * (1.0 - alpha) / (double)tested_count);
    double pass_ratio = (double)pass_count / (double)tested_count;

    nist_dbg_printf("  [DBG] Pass ratio: %.4f, threshold: %.4f\n", pass_ratio, threshold);

    return (pass_ratio >= threshold) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_non_overlapping_template_test_ex(const uint8_t *data, uint32_t n_bits,
                                          uint32_t m, int early_exit,
                                          double *p_value,
                                          int *out_passed_count, int *out_total_count)
{
    return non_overlapping_template_run(data, n_bits, m, early_exit,
                                        NIST_DEFAULT_ALPHA, p_value,
                                        out_passed_count, out_total_count, NULL);
}

int nist_non_overlapping_template_test(const uint8_t *data, uint32_t n_bits,
                                        uint32_t m, double *p_value)
{
    return nist_non_overlapping_template_test_ex(data, n_bits, m, 0, p_value, NULL, NULL);
}

/**
 * @brief P(exactly u overlapping-template hits in a block), sts Pr(u, eta)
 *
 * Polya-Aeppli distribution used by the sts-2.1.2 reference to compute the
 * overlapping-template class probabilities at runtime (its hardcoded pi[]
 * initializer is dead code that gets overwritten by these values).
 */
static double overlap_pr(int u, double eta)
{
    if (u == 0) {
        return exp(-eta);
    }
    double sum = 0.0;
    for (int l = 1; l <= u; l++) {
        sum += exp(-eta - u * log(2.0) + l * log(eta)
                   - nist_lgamma((double)l + 1.0) + nist_lgamma((double)u)
                   - nist_lgamma((double)l) - nist_lgamma((double)(u - l + 1)));
    }
    return sum;
}

int nist_overlapping_template_test(const uint8_t *data, uint32_t n_bits,
                                    uint32_t m, double *p_value)
{
    if (!data || !p_value || m < 2 || m > 21) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Template B = 111...1 (m ones) - per NIST SP 800-22 Section 2.8 */
    uint32_t B = (1U << m) - 1;

    /*
     * The theoretical probabilities pi[] and block size M = 1032 below are
     * only valid for m = 9 (NIST SP 800-22 Section 2.8). Using them for any
     * other m would produce a statistically meaningless P-value, so reject
     * instead of silently reusing the m=9 constants.
     */
    if (m != 9) {
        return NIST_ERROR_INVALID_PARAM;
    }

    uint32_t M = 1032;  /* Recommended block size for m=9 */
    uint32_t N = n_bits / M;
    if (N < 1) return NIST_ERROR_INSUFFICIENT_DATA;

    int K = 5;  /* Number of categories (0, 1, 2, 3, 4, >=5) */

    /*
     * Class probabilities computed at runtime from lambda/eta, matching the
     * sts-2.1.2 reference (which overwrites its hardcoded pi[] with these).
     */
    double lambda = (double)(M - m + 1) / pow(2.0, m);
    double eta = lambda / 2.0;
    double pi[6];
    double pi_sum = 0.0;
    for (int i = 0; i < K; i++) {
        pi[i] = overlap_pr(i, eta);
        pi_sum += pi[i];
    }
    pi[K] = 1.0 - pi_sum;

    uint32_t *v = (uint32_t *)calloc(K + 1, sizeof(uint32_t));
    if (!v) return NIST_ERROR_NO_MEMORY;

    for (uint32_t i = 0; i < N; i++) {
        uint32_t start = i * M;
        uint32_t W = 0;

        /* Count overlapping occurrences */
        for (uint32_t j = 0; j <= M - m; j++) {
            uint32_t pattern = get_pattern(data, start + j, m);
            if (pattern == B) W++;
        }

        /* Classify */
        if (W <= (uint32_t)K) {
            v[W]++;
        } else {
            v[K]++;
        }
    }

    /* Chi-squared statistic */
    double chi_sq = 0.0;
    for (int i = 0; i <= K; i++) {
        double expected = N * pi[i];
        chi_sq += pow((double)v[i] - expected, 2) / expected;
    }

    free(v);

    *p_value = nist_igamc((double)K / 2.0, chi_sq / 2.0);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_universal_test(const uint8_t *data, uint32_t n_bits, double *p_value)
{
    if (!data || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /*
     * Select L based on n_bits (NIST Table 2.9). Largest threshold first --
     * each entry is the minimum n for that L, so the chain must be checked
     * in descending order.
     */
    int L;
    if (n_bits >= 107560960) L = 13;
    else if (n_bits >= 49643520) L = 12;
    else if (n_bits >= 22753280) L = 11;
    else if (n_bits >= 10342400) L = 10;
    else if (n_bits >= 4654080) L = 9;
    else if (n_bits >= 2068480) L = 8;
    else if (n_bits >= 904960) L = 7;
    else {
        /* n >= 387840 recommended for L=6; smaller n rejected below via K < 1 */
        L = 6;
    }

    /* Q = 10 * 2^L initialization blocks, K = test blocks */
    uint32_t Q = 10 * (1U << L);
    uint32_t K = (n_bits / L) - Q;

    if (K < 1) return NIST_ERROR_INSUFFICIENT_DATA;

    /* Expected value and variance (NIST Table 2.9) */
    static const double expected_L[] = {
        0, 0, 0, 0, 0, 0,
        5.2177052, 6.1962507, 7.1836656, 8.1764248, 9.1723243,
        10.170032, 11.168765, 12.168070
    };
    static const double variance_L[] = {
        0, 0, 0, 0, 0, 0,
        2.954, 3.125, 3.238, 3.311, 3.356,
        3.384, 3.401, 3.410
    };

    double expected = expected_L[L];
    double variance = variance_L[L];
    double c = 0.7 - 0.8 / L + (4.0 + 32.0 / L) * pow((double)K, -3.0 / L) / 15.0;
    double sigma = c * sqrt(variance / K);

    /* Allocate table for last occurrence */
    uint32_t table_size = 1U << L;
    uint32_t *T = (uint32_t *)calloc(table_size, sizeof(uint32_t));
    if (!T) return NIST_ERROR_NO_MEMORY;

    /* Initialization: first Q blocks */
    for (uint32_t i = 0; i < Q; i++) {
        uint32_t pattern = get_pattern(data, i * L, L);
        T[pattern] = i + 1;
    }

    /* Test phase */
    double sum = 0.0;
    for (uint32_t i = Q; i < Q + K; i++) {
        uint32_t pattern = get_pattern(data, i * L, L);
        sum += log2((double)(i + 1 - T[pattern]));
        T[pattern] = i + 1;
    }

    double fn = sum / K;

    free(T);

    *p_value = nist_erfc(fabs(fn - expected) / (M_SQRT2 * sigma));

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_linear_complexity_test(const uint8_t *data, uint32_t n_bits,
                                 uint32_t M, double *p_value)
{
    if (!data || !p_value || M < 500 || M > 5000) {
        return NIST_ERROR_INVALID_PARAM;
    }

    uint32_t N = n_bits / M;
    if (N < 1) return NIST_ERROR_INSUFFICIENT_DATA;

    /*
     * Theoretical mean, mu = M/2 + (9 + (-1)^(M+1))/36 - (M/3 + 2/9)/2^M.
     * (-1)^(M+1) is +1 for even M (sts linearComplexity.c agrees); the sign
     * was previously inverted here, shifting borderline T classifications.
     */
    double mu = (double)M / 2.0 + (9.0 + (M % 2 ? -1.0 : 1.0)) / 36.0
                - ((double)M / 3.0 + 2.0 / 9.0) / pow(2.0, M);

    /* Category counts */
    int K = 6;
    uint32_t *v = (uint32_t *)calloc(K + 1, sizeof(uint32_t));
    if (!v) return NIST_ERROR_NO_MEMORY;

    /* Theoretical probabilities */
    static const double pi[] = {0.010417, 0.031250, 0.125000, 0.500000,
                                 0.250000, 0.062500, 0.020833};

    for (uint32_t i = 0; i < N; i++) {
        int L = berlekamp_massey(data, i * M, M);
        if (L < 0) {
            free(v);
            return NIST_ERROR_NO_MEMORY;
        }

        /* T = (-1)^M * (L - mu) + 2/9, per NIST SP 800-22 Section 2.10 */
        double T = (M % 2 ? -1.0 : 1.0) * ((double)L - mu) + 2.0 / 9.0;

        /* Classify */
        int category;
        if (T <= -2.5) category = 0;
        else if (T <= -1.5) category = 1;
        else if (T <= -0.5) category = 2;
        else if (T <= 0.5) category = 3;
        else if (T <= 1.5) category = 4;
        else if (T <= 2.5) category = 5;
        else category = 6;

        v[category]++;
    }

    /* Chi-squared statistic */
    double chi_sq = 0.0;
    for (int i = 0; i <= K; i++) {
        double expected = N * pi[i];
        if (expected > 0) {
            chi_sq += pow((double)v[i] - expected, 2) / expected;
        }
    }

    /* Debug output */
    nist_dbg_printf("\n  [DBG] Linear Complexity: N=%lu, M=%lu, mu=%.2f\n",
           (unsigned long)N, (unsigned long)M, mu);
    nist_dbg_printf("  [DBG] v[0..6]=%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
           (unsigned long)v[0], (unsigned long)v[1], (unsigned long)v[2],
           (unsigned long)v[3], (unsigned long)v[4], (unsigned long)v[5],
           (unsigned long)v[6]);
    free(v);

    *p_value = nist_igamc((double)K / 2.0, chi_sq / 2.0);
    nist_dbg_printf("  [DBG] chi_sq=%.2f, p_value=%.6f\n", chi_sq, *p_value);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_serial_test(const uint8_t *data, uint32_t n_bits, uint32_t m,
                      double *p_value1, double *p_value2)
{
    if (!data || !p_value1 || !p_value2 || m < 2) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* m should be less than floor(log2(n)) - 2 */
    uint32_t max_m = 0;
    uint32_t temp = n_bits;
    while (temp > 1) { max_m++; temp >>= 1; }
    if (m >= max_m - 2) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Compute psi-squared for m, m-1, m-2 */
    double psi_m = 0.0, psi_m1 = 0.0, psi_m2 = 0.0;

    /* Pattern counts for m-bit patterns */
    uint32_t num_patterns_m = 1U << m;
    uint32_t num_patterns_m1 = 1U << (m - 1);
    uint32_t num_patterns_m2 = (m >= 2) ? (1U << (m - 2)) : 1;

    uint32_t *count_m = (uint32_t *)calloc(num_patterns_m, sizeof(uint32_t));
    uint32_t *count_m1 = (uint32_t *)calloc(num_patterns_m1, sizeof(uint32_t));
    uint32_t *count_m2 = (uint32_t *)calloc(num_patterns_m2, sizeof(uint32_t));

    if (!count_m || !count_m1 || !count_m2) {
        free(count_m);
        free(count_m1);
        free(count_m2);
        return NIST_ERROR_NO_MEMORY;
    }

    /* Count overlapping m-bit patterns (with wraparound) */
    for (uint32_t i = 0; i < n_bits; i++) {
        uint32_t pattern = 0;
        for (uint32_t j = 0; j < m; j++) {
            pattern = (pattern << 1) | GET_BIT(data, (i + j) % n_bits);
        }
        count_m[pattern]++;
    }

    /* Count (m-1)-bit patterns */
    for (uint32_t i = 0; i < n_bits; i++) {
        uint32_t pattern = 0;
        for (uint32_t j = 0; j < m - 1; j++) {
            pattern = (pattern << 1) | GET_BIT(data, (i + j) % n_bits);
        }
        count_m1[pattern]++;
    }

    /* Count (m-2)-bit patterns */
    if (m >= 2) {
        for (uint32_t i = 0; i < n_bits; i++) {
            uint32_t pattern = 0;
            for (uint32_t j = 0; j < m - 2; j++) {
                pattern = (pattern << 1) | GET_BIT(data, (i + j) % n_bits);
            }
            count_m2[pattern]++;
        }
    }

    /* Compute psi-squared values */
    for (uint32_t i = 0; i < num_patterns_m; i++) {
        psi_m += (double)count_m[i] * count_m[i];
    }
    psi_m = psi_m * pow(2.0, m) / n_bits - n_bits;

    for (uint32_t i = 0; i < num_patterns_m1; i++) {
        psi_m1 += (double)count_m1[i] * count_m1[i];
    }
    psi_m1 = psi_m1 * pow(2.0, m - 1) / n_bits - n_bits;

    if (m >= 2) {
        for (uint32_t i = 0; i < num_patterns_m2; i++) {
            psi_m2 += (double)count_m2[i] * count_m2[i];
        }
        psi_m2 = psi_m2 * pow(2.0, m - 2) / n_bits - n_bits;
    }

    free(count_m);
    free(count_m1);
    free(count_m2);

    /* Compute delta statistics */
    double delta1 = psi_m - psi_m1;
    double delta2 = psi_m - 2.0 * psi_m1 + psi_m2;

    /* P-values */
    *p_value1 = nist_igamc(pow(2.0, m - 2), delta1 / 2.0);
    *p_value2 = nist_igamc(pow(2.0, m - 3), delta2 / 2.0);

    int result1 = (*p_value1 >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
    int result2 = (*p_value2 >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;

    return (result1 == NIST_SUCCESS && result2 == NIST_SUCCESS) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_approximate_entropy_test(const uint8_t *data, uint32_t n_bits,
                                   uint32_t m, double *p_value)
{
    if (!data || !p_value || m < 2) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* m should be less than floor(log2(n)) - 5 */
    uint32_t max_m = 0;
    uint32_t temp = n_bits;
    while (temp > 1) { max_m++; temp >>= 1; }
    if (m >= max_m - 5) {
        m = max_m - 6;
        if (m < 2) return NIST_ERROR_INSUFFICIENT_DATA;
    }

    /* Compute phi for m and m+1 */
    double phi_m = 0.0, phi_m1 = 0.0;

    uint32_t num_patterns_m = 1U << m;
    uint32_t num_patterns_m1 = 1U << (m + 1);

    uint32_t *count_m = (uint32_t *)calloc(num_patterns_m, sizeof(uint32_t));
    uint32_t *count_m1 = (uint32_t *)calloc(num_patterns_m1, sizeof(uint32_t));

    if (!count_m || !count_m1) {
        free(count_m);
        free(count_m1);
        return NIST_ERROR_NO_MEMORY;
    }

    /* Count m-bit overlapping patterns (with wraparound) */
    for (uint32_t i = 0; i < n_bits; i++) {
        uint32_t pattern = 0;
        for (uint32_t j = 0; j < m; j++) {
            pattern = (pattern << 1) | GET_BIT(data, (i + j) % n_bits);
        }
        count_m[pattern]++;
    }

    /* Count (m+1)-bit overlapping patterns */
    for (uint32_t i = 0; i < n_bits; i++) {
        uint32_t pattern = 0;
        for (uint32_t j = 0; j < m + 1; j++) {
            pattern = (pattern << 1) | GET_BIT(data, (i + j) % n_bits);
        }
        count_m1[pattern]++;
    }

    /* Compute phi values */
    for (uint32_t i = 0; i < num_patterns_m; i++) {
        if (count_m[i] > 0) {
            double C_i = (double)count_m[i] / n_bits;
            phi_m += C_i * log(C_i);
        }
    }

    for (uint32_t i = 0; i < num_patterns_m1; i++) {
        if (count_m1[i] > 0) {
            double C_i = (double)count_m1[i] / n_bits;
            phi_m1 += C_i * log(C_i);
        }
    }

    free(count_m);
    free(count_m1);

    /* Approximate entropy */
    double ApEn = phi_m - phi_m1;

    /* Chi-squared statistic */
    double chi_sq = 2.0 * n_bits * (log(2.0) - ApEn);

    *p_value = nist_igamc(pow(2.0, m - 1), chi_sq / 2.0);

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_cusum_forward_test(const uint8_t *data, uint32_t n_bits, double *p_value)
{
    if (!data || n_bits < 100 || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Compute cumulative sum and find max absolute value */
    int32_t S = 0;
    int32_t z = 0;

    for (uint32_t i = 0; i < n_bits; i++) {
        S += GET_BIT(data, i) ? 1 : -1;
        if (abs(S) > z) z = abs(S);
    }

    if (z == 0) {
        *p_value = 1.0;
        return NIST_SUCCESS;
    }

    /* Compute P-value using NIST formula */
    double n_d = (double)n_bits;
    double z_d = (double)z;
    double sqrt_n = sqrt(n_d);

    double sum1 = 0.0, sum2 = 0.0;

    int k_start = (int)floor((-n_d / z_d + 1.0) / 4.0);
    int k_end = (int)floor((n_d / z_d - 1.0) / 4.0);

    for (int k = k_start; k <= k_end; k++) {
        double arg1 = (4.0 * k + 1.0) * z_d / sqrt_n;
        double arg2 = (4.0 * k - 1.0) * z_d / sqrt_n;
        sum1 += nist_phi(arg1) - nist_phi(arg2);
    }

    k_start = (int)floor((-n_d / z_d - 3.0) / 4.0);
    k_end = (int)floor((n_d / z_d - 1.0) / 4.0);

    for (int k = k_start; k <= k_end; k++) {
        double arg1 = (4.0 * k + 3.0) * z_d / sqrt_n;
        double arg2 = (4.0 * k + 1.0) * z_d / sqrt_n;
        sum2 += nist_phi(arg1) - nist_phi(arg2);
    }

    *p_value = 1.0 - sum1 + sum2;

    if (*p_value < 0.0) *p_value = 0.0;
    if (*p_value > 1.0) *p_value = 1.0;

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_cusum_backward_test(const uint8_t *data, uint32_t n_bits, double *p_value)
{
    if (!data || n_bits < 100 || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Compute cumulative sum backward and find max absolute value */
    int32_t S = 0;
    int32_t z = 0;

    for (int32_t i = n_bits - 1; i >= 0; i--) {
        S += GET_BIT(data, i) ? 1 : -1;
        if (abs(S) > z) z = abs(S);
    }

    if (z == 0) {
        *p_value = 1.0;
        return NIST_SUCCESS;
    }

    /* Same P-value formula as forward */
    double n_d = (double)n_bits;
    double z_d = (double)z;
    double sqrt_n = sqrt(n_d);

    double sum1 = 0.0, sum2 = 0.0;

    int k_start = (int)floor((-n_d / z_d + 1.0) / 4.0);
    int k_end = (int)floor((n_d / z_d - 1.0) / 4.0);

    for (int k = k_start; k <= k_end; k++) {
        double arg1 = (4.0 * k + 1.0) * z_d / sqrt_n;
        double arg2 = (4.0 * k - 1.0) * z_d / sqrt_n;
        sum1 += nist_phi(arg1) - nist_phi(arg2);
    }

    k_start = (int)floor((-n_d / z_d - 3.0) / 4.0);
    k_end = (int)floor((n_d / z_d - 1.0) / 4.0);

    for (int k = k_start; k <= k_end; k++) {
        double arg1 = (4.0 * k + 3.0) * z_d / sqrt_n;
        double arg2 = (4.0 * k + 1.0) * z_d / sqrt_n;
        sum2 += nist_phi(arg1) - nist_phi(arg2);
    }

    *p_value = 1.0 - sum1 + sum2;

    if (*p_value < 0.0) *p_value = 0.0;
    if (*p_value > 1.0) *p_value = 1.0;

    return (*p_value >= NIST_DEFAULT_ALPHA) ? NIST_SUCCESS : NIST_FAILURE;
}

uint32_t nist_count_zero_crossings(const uint8_t *data, uint32_t n_bits)
{
    if (!data || n_bits == 0) {
        return 0;
    }

    /*
     * Count cycles in the cumulative sum random walk: one per zero
     * crossing, plus the tail when the walk does not end at zero (same
     * semantics as the Random Excursions tests / sts reference).
     * Uses O(1) memory by computing S incrementally.
     */
    int32_t S = 0;
    uint32_t J = 0;

    for (uint32_t i = 0; i < n_bits; i++) {
        S += GET_BIT(data, i) ? 1 : -1;
        if (S == 0) J++;
    }
    if (S != 0) J++;

    return J;
}

int nist_random_excursions_test(const uint8_t *data, uint32_t n_bits,
                                 double p_values[8], double *min_pval)
{
    if (!data || n_bits < 1000000 || !p_values || !min_pval) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* States to test: -4, -3, -2, -1, +1, +2, +3, +4 */
    static const int states[8] = {-4, -3, -2, -1, 1, 2, 3, 4};

    /* Build cumulative sum sequence and count cycles */
    int32_t *S = (int32_t *)malloc((n_bits + 1) * sizeof(int32_t));
    if (!S) return NIST_ERROR_NO_MEMORY;

    S[0] = 0;
    for (uint32_t i = 0; i < n_bits; i++) {
        S[i + 1] = S[i] + (GET_BIT(data, i) ? 1 : -1);
    }

    /*
     * Count cycles: one per zero crossing, plus one for the tail after the
     * last zero when the walk does not end at zero (sts reference counts
     * that partial excursion as a cycle too).
     */
    uint32_t J = 0;
    int32_t S_min = 0, S_max = 0;
    for (uint32_t i = 1; i <= n_bits; i++) {
        if (S[i] == 0) J++;
        if (S[i] < S_min) S_min = S[i];
        if (S[i] > S_max) S_max = S[i];
    }
    if (S[n_bits] != 0) J++;

    /* Debug output */
    nist_dbg_printf("\n  [DBG] Random Excursions: n=%lu, J=%lu (need>=500)\n",
           (unsigned long)n_bits, (unsigned long)J);
    nist_dbg_printf("  [DBG] S_final=%ld, S_min=%ld, S_max=%ld\n",
           (long)S[n_bits], (long)S_min, (long)S_max);

    if (J < 500) {
        nist_dbg_printf("  [DBG] PRECONDITION FAIL: J=%lu < 500\n", (unsigned long)J);
        free(S);
        *min_pval = 0.0;
        return NIST_ERROR_PRECONDITION;
    }

    /* Count visits to each state within each cycle */
    /* v_k[state_idx][k] = number of cycles with exactly k visits to state */
    uint32_t v_k[8][6] = {{0}};

    uint32_t cycle_start = 0;
    for (uint32_t i = 1; i <= n_bits; i++) {
        /* A cycle ends at each zero, and at sequence end (tail cycle) */
        if (S[i] == 0 || i == n_bits) {
            for (int s = 0; s < 8; s++) {
                int state = states[s];
                uint32_t visits = 0;

                for (uint32_t j = cycle_start + 1; j <= i; j++) {
                    if (S[j] == state) visits++;
                }

                int k = (visits >= 5) ? 5 : visits;
                v_k[s][k]++;
            }
            cycle_start = i;
        }
    }

    free(S);

    /* Compute chi-squared for each state */
    *min_pval = 1.0;
    int states_passed = 0;

    for (int s = 0; s < 8; s++) {
        int x = abs(states[s]);
        double chi_sq = 0.0;

        /*
         * Calculate pi values dynamically per NIST SP 800-22:
         * k=0: pi = 1 - 1/(2|x|)
         * k=1..4: pi = (1/(4x^2)) * (1 - 1/(2|x|))^(k-1)
         * k>=5: pi = (1/(2|x|)) * (1 - 1/(2|x|))^4
         */
        double base = 1.0 - 1.0 / (2.0 * x);  /* (1 - 1/(2|x|)) */
        double factor = 1.0 / (4.0 * x * x);   /* 1/(4x^2) */

        for (int k = 0; k <= 5; k++) {
            double pi_k;
            if (k == 0) {
                pi_k = 1.0 - 1.0 / (2.0 * x);
            } else if (k <= 4) {
                pi_k = factor * pow(base, k - 1);
            } else {
                pi_k = (1.0 / (2.0 * x)) * pow(base, 4);
            }

            double expected = J * pi_k;
            if (expected > 0) {
                chi_sq += pow((double)v_k[s][k] - expected, 2) / expected;
            }
        }

        p_values[s] = nist_igamc(2.5, chi_sq / 2.0);

        if (p_values[s] >= NIST_DEFAULT_ALPHA) {
            states_passed++;
        }
        if (p_values[s] < *min_pval) {
            *min_pval = p_values[s];
        }
    }

    /*
     * NIST SP 800-22 Section 4.2.1: Use proportion test for multi-state tests.
     * For 8 states with alpha=0.01, threshold = 0.99 - 3*sqrt(0.0099/8) = 0.8845
     * So at least 8 * 0.8845 = 7.08 -> 7 states must pass.
     */
    return nist_proportion_check(states_passed, 8, NIST_DEFAULT_ALPHA)
           ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_random_excursions_variant_test(const uint8_t *data, uint32_t n_bits,
                                         double p_values[18], double *min_pval)
{
    if (!data || n_bits < 1000000 || !p_values || !min_pval) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* States to test: -9...-1, +1...+9 */
    static const int states[18] = {-9, -8, -7, -6, -5, -4, -3, -2, -1,
                                    1, 2, 3, 4, 5, 6, 7, 8, 9};

    /* Build cumulative sum and count zero crossings */
    int32_t S = 0;
    uint32_t J = 0;
    int32_t max_excursion = 0;

    /* First pass: find J and max excursion. The tail after the last zero
     * counts as one more cycle when the walk does not end at zero (sts). */
    for (uint32_t i = 0; i < n_bits; i++) {
        S += GET_BIT(data, i) ? 1 : -1;
        if (S == 0) J++;
        if (abs(S) > abs(max_excursion)) max_excursion = S;
    }
    if (S != 0) J++;

    /* Debug output */
    nist_dbg_printf("\n  [DBG] RE Variant: n=%lu, J=%lu (need>=500)\n",
           (unsigned long)n_bits, (unsigned long)J);
    nist_dbg_printf("  [DBG] S_final=%ld, max_excursion=%ld\n",
           (long)S, (long)max_excursion);

    if (J < 500) {
        nist_dbg_printf("  [DBG] PRECONDITION FAIL: J=%lu < 500\n", (unsigned long)J);
        *min_pval = 0.0;
        return NIST_ERROR_PRECONDITION;
    }

    /* Count total visits to each state */
    int32_t state_count[19] = {0};  /* Index 0..18 for states -9..+9 (skip 0) */

    S = 0;
    for (uint32_t i = 0; i < n_bits; i++) {
        S += GET_BIT(data, i) ? 1 : -1;
        if (S >= -9 && S <= 9 && S != 0) {
            int idx = (S < 0) ? (S + 9) : (S + 8);
            state_count[idx]++;
        }
    }

    /* Compute P-values */
    *min_pval = 1.0;
    int states_passed = 0;

    for (int s = 0; s < 18; s++) {
        int x = states[s];
        int idx = (x < 0) ? (x + 9) : (x + 8);
        int32_t xi = state_count[idx];

        /* P-value formula from NIST SP 800-22 Section 2.15 */
        double num = fabs((double)xi - (double)J);
        double den = sqrt(2.0 * J * (4.0 * abs(x) - 2.0));

        p_values[s] = nist_erfc(num / den);

        if (p_values[s] >= NIST_DEFAULT_ALPHA) {
            states_passed++;
        }
        if (p_values[s] < *min_pval) {
            *min_pval = p_values[s];
        }
    }

    /*
     * NIST SP 800-22 Section 4.2.1: Use proportion test for multi-state tests.
     * For 18 states with alpha=0.01, threshold = 0.99 - 3*sqrt(0.0099/18) = 0.9196
     * So at least 18 * 0.9196 = 16.55 -> 17 states must pass.
     */
    return nist_proportion_check(states_passed, 18, NIST_DEFAULT_ALPHA)
           ? NIST_SUCCESS : NIST_FAILURE;
}

/* ============================================================================
 * Full Test Suite Functions
 * ============================================================================ */

/**
 * @brief Internal function to run all tests on one fixed sequence
 *
 * Every test (including 15/16 Random Excursions) is applied to the same
 * sequence, per the NIST methodology. When the Random Excursions
 * precondition J >= 500 is not met, the two tests are reported N/A
 * (counted in tests_na) and excluded from pass/fail statistics -- exactly
 * how the sts reference code treats such sequences. No data regeneration
 * happens here: mixing sequences within one round would invalidate the
 * single-sequence premise of the suite.
 */
static int nist_run_all_tests_internal(uint8_t *data, uint32_t n_bits, double alpha,
                                        nist_suite_result_t *result,
                                        nist_substream_pvals_t *sub)
{
    if (!data || !result || n_bits < NIST_MIN_RECOMMENDED_BITS) {
        return NIST_ERROR_INVALID_PARAM;
    }

    memset(result, 0, sizeof(nist_suite_result_t));
    result->alpha = alpha;
    result->n_bits = n_bits;

    if (sub) {
        sub->nonoverlap_n = 0;
        sub->re_valid = 0;
        sub->rev_valid = 0;
    }

    int test_idx = 0;
    int ret;
    double p_value, p_value2;

    /* Default block sizes */
    uint32_t block_freq_M = 128;
    uint32_t template_m = 9;
    uint32_t serial_m = 16;
    uint32_t apen_m = 10;
    uint32_t linear_M = 500;

    /* Adjust parameters for smaller n */
    if (n_bits < 1000000) {
        serial_m = 8;
        apen_m = 5;
    }

    /* 2.1 Frequency Test */
    printf("[1/16] Frequency (Monobit)...");
    result->tests[test_idx].test_name = "Frequency (Monobit)";
    result->tests[test_idx].test_id = 1;
    ret = nist_frequency_test(data, n_bits, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", "FAIL");
    test_idx++;

    /* 2.2 Block Frequency Test */
    printf("[2/16] Block Frequency...");
    result->tests[test_idx].test_name = "Block Frequency";
    result->tests[test_idx].test_id = 2;
    ret = nist_block_frequency_test(data, n_bits, block_freq_M, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", "FAIL");
    test_idx++;

    /* 2.3 Runs Test */
    printf("[3/16] Runs...");
    result->tests[test_idx].test_name = "Runs";
    result->tests[test_idx].test_id = 3;
    ret = nist_runs_test(data, n_bits, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", "FAIL");
    test_idx++;

    /* 2.4 Longest Run Test */
    printf("[4/16] Longest Run of Ones...");
    result->tests[test_idx].test_name = "Longest Run of Ones";
    result->tests[test_idx].test_id = 4;
    ret = nist_longest_run_test(data, n_bits, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", "FAIL");
    test_idx++;

    /* 2.5 Binary Matrix Rank Test */
    printf("[5/16] Binary Matrix Rank...");
    result->tests[test_idx].test_name = "Binary Matrix Rank";
    result->tests[test_idx].test_id = 5;
    ret = nist_matrix_rank_test(data, n_bits, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", "FAIL");
    test_idx++;

    /* 2.6 DFT (Spectral) Test */
    printf("[6/16] DFT (Spectral)...");
    result->tests[test_idx].test_name = "DFT (Spectral)";
    result->tests[test_idx].test_id = 6;
    ret = nist_spectral_test(data, n_bits, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", ret < 0 ? nist_strerror(ret) : "FAIL");
    test_idx++;

    /* 2.7 Non-overlapping Template Test */
    printf("[7/16] Non-overlapping Template...");
    result->tests[test_idx].test_name = "Non-overlapping Template";
    result->tests[test_idx].test_id = 7;
    {
        int tmpl_passed = 0, tmpl_total = 0;
        ret = non_overlapping_template_run(data, n_bits, template_m, 0, alpha,
                                           &p_value, &tmpl_passed, &tmpl_total,
                                           sub ? sub->nonoverlap : NULL);
        result->tests[test_idx].result = ret;
        result->tests[test_idx].p_value = p_value;
        result->tests[test_idx].passed = (ret == NIST_SUCCESS);
        result->tests[test_idx].sub_total = tmpl_total;
        result->tests[test_idx].sub_passed = tmpl_passed;
        if (sub) sub->nonoverlap_n = (ret == NIST_SUCCESS) ? tmpl_total : 0;
    }
    print_pvalue(p_value, ret, "PASS", ret < 0 ? nist_strerror(ret) : "FAIL");
    test_idx++;

    /* 2.8 Overlapping Template Test */
    printf("[8/16] Overlapping Template...");
    result->tests[test_idx].test_name = "Overlapping Template";
    result->tests[test_idx].test_id = 8;
    ret = nist_overlapping_template_test(data, n_bits, template_m, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", ret < 0 ? nist_strerror(ret) : "FAIL");
    test_idx++;

    /* 2.9 Universal Test */
    printf("[9/16] Maurer's Universal...");
    result->tests[test_idx].test_name = "Maurer's Universal";
    result->tests[test_idx].test_id = 9;
    ret = nist_universal_test(data, n_bits, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", ret < 0 ? nist_strerror(ret) : "FAIL");
    test_idx++;

    /* 2.10 Linear Complexity Test */
    printf("[10/16] Linear Complexity...");
    result->tests[test_idx].test_name = "Linear Complexity";
    result->tests[test_idx].test_id = 10;
    ret = nist_linear_complexity_test(data, n_bits, linear_M, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", ret < 0 ? nist_strerror(ret) : "FAIL");
    test_idx++;

    /* 2.11 Serial Test (2 sub-tests: P-value1 and P-value2) */
    printf("[11/16] Serial...");
    result->tests[test_idx].test_name = "Serial";
    result->tests[test_idx].test_id = 11;
    ret = nist_serial_test(data, n_bits, serial_m, &p_value, &p_value2);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].p_value2 = p_value2;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 2;
    {
        int sub_pass = 0;
        if (p_value >= alpha) sub_pass++;
        if (p_value2 >= alpha) sub_pass++;
        result->tests[test_idx].sub_passed = sub_pass;
    }
    /* Serial test has two P-values, handle specially */
    {
        const char *status = (ret == NIST_SUCCESS) ? "PASS" : (ret < 0 ? nist_strerror(ret) : "FAIL");
        double min_p = (p_value < p_value2) ? p_value : p_value2;
        if (min_p != min_p || min_p < 1e-10) {
            printf(" P<1e-10 %s\n", status);
        } else if (min_p < 0.0001) {
            printf(" P=%.2e/%.2e %s\n", p_value, p_value2, status);
        } else {
            printf(" P=%.4f/%.4f %s\n", p_value, p_value2, status);
        }
    }
    test_idx++;

    /* 2.12 Approximate Entropy Test */
    printf("[12/16] Approximate Entropy...");
    result->tests[test_idx].test_name = "Approximate Entropy";
    result->tests[test_idx].test_id = 12;
    ret = nist_approximate_entropy_test(data, n_bits, apen_m, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", ret < 0 ? nist_strerror(ret) : "FAIL");
    test_idx++;

    /* 2.13 Cumulative Sums (Forward) */
    printf("[13/16] Cumulative Sums (Fwd)...");
    result->tests[test_idx].test_name = "Cumulative Sums (Fwd)";
    result->tests[test_idx].test_id = 13;
    ret = nist_cusum_forward_test(data, n_bits, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", ret < 0 ? nist_strerror(ret) : "FAIL");
    test_idx++;

    /* 2.13 Cumulative Sums (Backward) - counted as part of test 13 */
    printf("[14/16] Cumulative Sums (Bwd)...");
    result->tests[test_idx].test_name = "Cumulative Sums (Bwd)";
    result->tests[test_idx].test_id = 13;
    ret = nist_cusum_backward_test(data, n_bits, &p_value);
    result->tests[test_idx].result = ret;
    result->tests[test_idx].p_value = p_value;
    result->tests[test_idx].passed = (ret == NIST_SUCCESS);
    result->tests[test_idx].sub_total = 1;
    result->tests[test_idx].sub_passed = (ret == NIST_SUCCESS) ? 1 : 0;
    print_pvalue(p_value, ret, "PASS", ret < 0 ? nist_strerror(ret) : "FAIL");
    test_idx++;

    /*
     * 2.14 & 2.15 Random Excursions Tests (requires 1M+ bits)
     *
     * Both tests share the J (zero crossings) >= 500 precondition. Per the
     * spec, a sequence with J < 500 is "not applicable" for these tests --
     * it is reported N/A and excluded from the statistics, never
     * regenerated (all 15 tests must see the same sequence).
     */
    if (n_bits >= 1000000) {
        double re_pvalues[8];
        double rev_pvalues[18];
        double re_min_pval, rev_min_pval;
        int re_ret, rev_ret;

        re_ret = nist_random_excursions_test(data, n_bits, re_pvalues, &re_min_pval);
        rev_ret = nist_random_excursions_variant_test(data, n_bits, rev_pvalues, &rev_min_pval);

        /* Record test 15 results */
        printf("[15/16] Random Excursions...");
        result->tests[test_idx].test_name = "Random Excursions";
        result->tests[test_idx].test_id = 14;
        result->tests[test_idx].result = re_ret;
        result->tests[test_idx].p_value = re_min_pval;
        result->tests[test_idx].sub_total = 8;

        if (re_ret == NIST_ERROR_PRECONDITION) {
            result->tests[test_idx].passed = true;
            result->tests[test_idx].sub_passed = 0;
            result->tests[test_idx].details = "J<500 (test N/A)";
            printf(" N/A (J<500, insufficient cycles)\n");
        } else {
            result->tests[test_idx].passed = (re_ret == NIST_SUCCESS);
            int sub_pass = 0;
            for (int s = 0; s < 8; s++) {
                if (re_pvalues[s] >= alpha) sub_pass++;
            }
            result->tests[test_idx].sub_passed = sub_pass;
            if (sub) {
                for (int s = 0; s < 8; s++) sub->re[s] = re_pvalues[s];
                sub->re_valid = 1;
            }
            print_pvalue(re_min_pval, re_ret, "PASS", re_ret < 0 ? nist_strerror(re_ret) : "FAIL");
        }
        test_idx++;

        /* Record test 16 results */
        printf("[16/16] Random Excursions Variant...");
        result->tests[test_idx].test_name = "Random Excursions Variant";
        result->tests[test_idx].test_id = 15;
        result->tests[test_idx].result = rev_ret;
        result->tests[test_idx].p_value = rev_min_pval;
        result->tests[test_idx].sub_total = 18;

        if (rev_ret == NIST_ERROR_PRECONDITION) {
            result->tests[test_idx].passed = true;
            result->tests[test_idx].sub_passed = 0;
            result->tests[test_idx].details = "J<500 (test N/A)";
            printf(" N/A (J<500, insufficient cycles)\n");
        } else {
            result->tests[test_idx].passed = (rev_ret == NIST_SUCCESS);
            int sub_pass = 0;
            for (int s = 0; s < 18; s++) {
                if (rev_pvalues[s] >= alpha) sub_pass++;
            }
            result->tests[test_idx].sub_passed = sub_pass;
            if (sub) {
                for (int s = 0; s < 18; s++) sub->rev[s] = rev_pvalues[s];
                sub->rev_valid = 1;
            }
            print_pvalue(rev_min_pval, rev_ret, "PASS", rev_ret < 0 ? nist_strerror(rev_ret) : "FAIL");
        }
        test_idx++;
    } else {
        /* Insufficient bits for Random Excursions tests */
        printf("[15/16] Random Excursions...");
        result->tests[test_idx].test_name = "Random Excursions";
        result->tests[test_idx].test_id = 14;
        result->tests[test_idx].result = NIST_ERROR_INSUFFICIENT_DATA;
        result->tests[test_idx].passed = true;  /* N/A (not enough bits) */
        result->tests[test_idx].details = "Requires >= 1M bits";
        result->tests[test_idx].sub_total = 8;
        result->tests[test_idx].sub_passed = 0;
        printf(" SKIP (need >= 1M bits)\n");
        test_idx++;

        printf("[16/16] Random Excursions Variant...");
        result->tests[test_idx].test_name = "Random Excursions Variant";
        result->tests[test_idx].test_id = 15;
        result->tests[test_idx].result = NIST_ERROR_INSUFFICIENT_DATA;
        result->tests[test_idx].passed = true;  /* N/A (not enough bits) */
        result->tests[test_idx].details = "Requires >= 1M bits";
        result->tests[test_idx].sub_total = 18;
        result->tests[test_idx].sub_passed = 0;
        printf(" SKIP (need >= 1M bits)\n");
        test_idx++;
    }

    /*
     * Re-derive pass/fail with the caller's alpha. The individual test
     * functions' return codes are fixed to NIST_DEFAULT_ALPHA; the recorded
     * P-values (and the alpha-based sub_passed counts above) are the source
     * of truth here.
     */
    for (int i = 0; i < test_idx; i++) {
        nist_test_result_t *t = &result->tests[i];
        if (t->result != NIST_SUCCESS && t->result != NIST_FAILURE) {
            continue;   /* error or N/A: keep as recorded */
        }
        if (t->test_id == 11) {                 /* Serial: both P-values */
            t->passed = (t->p_value >= alpha) && (t->p_value2 >= alpha);
        } else if (t->sub_total > 1) {          /* multi-state: proportion */
            t->passed = nist_proportion_check(t->sub_passed, t->sub_total, alpha);
        } else {
            t->passed = (t->p_value >= alpha);
            t->sub_passed = t->passed ? 1 : 0;
        }
        t->result = t->passed ? NIST_SUCCESS : NIST_FAILURE;
    }

    /*
     * Count results. N/A (precondition or insufficient data, reported with
     * passed=true) is neither pass, fail nor error -- it must not fail a
     * round, and callers must exclude it from per-test statistics.
     */
    result->tests_run = test_idx;
    for (int i = 0; i < test_idx; i++) {
        int rc = result->tests[i].result;
        if (rc >= 0) {
            if (result->tests[i].passed) {
                result->tests_passed++;
            } else {
                result->tests_failed++;
            }
        } else if (rc == NIST_ERROR_PRECONDITION || rc == NIST_ERROR_INSUFFICIENT_DATA) {
            result->tests_na++;
        } else {
            result->tests_error++;
        }
    }

    return result->tests_passed;
}

int nist_run_all_tests(const uint8_t *data, uint32_t n_bits, double alpha,
                        nist_suite_result_t *result)
{
    return nist_run_all_tests_internal((uint8_t *)data, n_bits, alpha, result, NULL);
}

int nist_run_all_tests_ex(uint8_t *data, uint32_t n_bits, double alpha,
                           nist_suite_result_t *result, nist_trng_hal_t trng_hal)
{
    /*
     * The trng_hal parameter is retained for API compatibility. It is no
     * longer used: the old J<500 retry regenerated data mid-round, which
     * mixed different sequences within one suite result. Sequences with
     * J < 500 are now reported N/A per the spec.
     */
    (void)trng_hal;
    return nist_run_all_tests_internal(data, n_bits, alpha, result, NULL);
}

int nist_run_all_tests_sub(uint8_t *data, uint32_t n_bits, double alpha,
                            nist_suite_result_t *result,
                            nist_substream_pvals_t *sub)
{
    return nist_run_all_tests_internal(data, n_bits, alpha, result, sub);
}

int nist_run_suite_with_hal(nist_trng_hal_t trng_hal, uint32_t n_bits,
                             double alpha, nist_suite_result_t *result)
{
    if (!trng_hal || !result) {
        return NIST_ERROR_INVALID_PARAM;
    }

    /* Calculate buffer size */
    uint32_t n_bytes = (n_bits + 7) / 8;

    /* Allocate buffer */
    uint8_t *data = (uint8_t *)malloc(n_bytes);
    if (!data) {
        return NIST_ERROR_NO_MEMORY;
    }

    /* Generate random data */
    int ret = trng_hal(data, n_bytes);
    if (ret != 0) {
        free(data);
        return ret;
    }

    ret = nist_run_all_tests_internal(data, n_bits, alpha, result, NULL);

    free(data);
    return ret;
}

void nist_print_results(const nist_suite_result_t *result)
{
    if (!result) return;

    printf("\n");
    printf("============================================================\n");
    printf("       NIST SP 800-22 Statistical Test Suite Results\n");
    printf("============================================================\n");
    printf("Bits tested: %lu   Alpha: %.4f   Time: %lu ms\n",
           (unsigned long)result->n_bits, result->alpha, (unsigned long)result->elapsed_ms);
    printf("------------------------------------------------------------\n");
    printf("%-28s %10s %8s\n", "Test Name", "P-value", "Result");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < result->tests_run; i++) {
        const nist_test_result_t *t = &result->tests[i];

        const char *status;
        if (t->result >= 0) {
            status = t->passed ? "PASS" : "FAIL";
        } else {
            status = nist_strerror(t->result);
        }

        if (t->p_value2 > 0) {
            printf("%-28s %10.6f %8s\n", t->test_name, t->p_value, status);
            printf("  (secondary)                %10.6f\n", t->p_value2);
        } else if (t->result >= 0) {
            printf("%-28s %10.6f %8s\n", t->test_name, t->p_value, status);
        } else {
            printf("%-28s %10s %8s\n", t->test_name, "---", status);
        }
    }

    printf("------------------------------------------------------------\n");
    printf("Summary: %d/%d passed, %d failed, %d N/A, %d errors\n",
           result->tests_passed,
           result->tests_run - result->tests_na - result->tests_error,
           result->tests_failed, result->tests_na, result->tests_error);
    printf("============================================================\n");
}

const char *nist_strerror(int code)
{
    switch (code) {
        case NIST_SUCCESS:              return "Success";
        case NIST_FAILURE:              return "Failed";
        case NIST_ERROR_INVALID_PARAM:  return "Invalid param";
        case NIST_ERROR_NO_MEMORY:      return "No memory";
        case NIST_ERROR_INSUFFICIENT_DATA: return "Need more data";
        case NIST_ERROR_PRECONDITION:   return "Precond fail";
        default:                        return "Unknown";
    }
}

double nist_proportion_threshold(int n, double alpha)
{
    if (n <= 0 || alpha <= 0.0 || alpha >= 1.0) {
        return 1.0;  /* Invalid input, require 100% pass */
    }

    /*
     * NIST SP 800-22 Section 4.2.1: Proportion of Sequences Passing a Test
     *
     * The minimum acceptable proportion is:
     *   threshold = (1 - alpha) - 3 * sqrt(alpha * (1 - alpha) / n)
     *
     * For alpha = 0.01:
     *   n=100:  threshold = 0.99 - 3*sqrt(0.0099/100)  = 0.99 - 0.0298 = 0.9602 (96.0%)
     *   n=1000: threshold = 0.99 - 3*sqrt(0.0099/1000) = 0.99 - 0.0094 = 0.9806 (98.1%)
     */
    double threshold = (1.0 - alpha) - 3.0 * sqrt(alpha * (1.0 - alpha) / (double)n);

    /* Clamp to reasonable range */
    if (threshold < 0.0) threshold = 0.0;
    if (threshold > 1.0) threshold = 1.0;

    return threshold;
}

bool nist_proportion_check(int passed, int total, double alpha)
{
    if (total <= 0) return false;

    double pass_ratio = (double)passed / (double)total;
    double threshold = nist_proportion_threshold(total, alpha);

    return (pass_ratio >= threshold);
}

/* ============================================================================
 * P-value Uniformity Test (NIST SP 800-22 Section 4.2.2)
 * ============================================================================ */

int nist_pvalue_uniformity_test(const double *p_values, int count,
                                 nist_uniformity_result_t *result)
{
    if (!p_values || !result || count < 10) {
        return NIST_ERROR_INVALID_PARAM;
    }

    memset(result, 0, sizeof(nist_uniformity_result_t));

    /*
     * NIST SP 800-22 Section 4.2.2: Uniformity of P-values
     *
     * Divide [0,1] into 10 sub-intervals: [0,0.1), [0.1,0.2), ..., [0.9,1.0]
     * Count number of P-values in each sub-interval.
     * Use chi-square goodness-of-fit test with expected count = count/10.
     */

    /* Count P-values in each bin */
    int valid_count = 0;
    for (int i = 0; i < count; i++) {
        double p = p_values[i];

        /* Skip invalid P-values (NaN, Inf) */
        if (!isfinite(p)) continue;

        /* Clamp P-value to [0, 1] range */
        if (p < 0.0) p = 0.0;
        if (p > 1.0) p = 1.0;

        /* Determine bin index (0-9) */
        int bin = (int)(p * 10.0);
        if (bin >= 10) bin = 9;  /* Handle p = 1.0 case */

        result->bins[bin]++;
        valid_count++;
    }

    /* Check if we have enough valid samples */
    if (valid_count < 10) {
        return NIST_ERROR_INVALID_PARAM;
    }
    result->total_samples = valid_count;

    /* Calculate chi-square statistic */
    result->expected_per_bin = (double)valid_count / 10.0;
    result->chi_square = 0.0;

    for (int i = 0; i < 10; i++) {
        double observed = (double)result->bins[i];
        double diff = observed - result->expected_per_bin;
        result->chi_square += (diff * diff) / result->expected_per_bin;
    }

    /*
     * P-value for chi-square with 9 degrees of freedom (10 bins - 1)
     * Using incomplete gamma function: P = igamc(df/2, chi_sq/2)
     */
    result->p_value = nist_igamc(4.5, result->chi_square / 2.0);

    /*
     * NIST criteria: P-value >= 0.0001 for the sequence to be considered random.
     * This is more stringent than the usual alpha = 0.01.
     */
    result->passed = (result->p_value >= 0.0001);

    return result->passed ? NIST_SUCCESS : NIST_FAILURE;
}

int nist_uniformity_from_hist(const int bins[10], int total,
                              double *chi_square, double *p_value)
{
    if (!bins || total < 10 || !chi_square || !p_value) {
        return NIST_ERROR_INVALID_PARAM;
    }
    double expected = (double)total / 10.0;
    double chi2 = 0.0;
    for (int i = 0; i < 10; i++) {
        double diff = (double)bins[i] - expected;
        chi2 += (diff * diff) / expected;
    }
    *chi_square = chi2;
    *p_value = nist_igamc(4.5, chi2 / 2.0);
    return (*p_value >= 0.0001) ? NIST_SUCCESS : NIST_FAILURE;
}

void nist_print_uniformity_result(const nist_uniformity_result_t *result)
{
    if (!result) return;

    printf("\n");
    printf("============================================================\n");
    printf("    P-value Uniformity Test (NIST SP 800-22 Section 4.2.2)\n");
    printf("============================================================\n");
    printf("Samples: %d   Expected/bin: %.1f\n",
           result->total_samples, result->expected_per_bin);
    printf("------------------------------------------------------------\n");
    printf("  Interval     Count    Expected    Diff\n");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < 10; i++) {
        double diff = (double)result->bins[i] - result->expected_per_bin;
        printf("  [%.1f - %.1f)   %4d      %5.1f     %+.1f\n",
               i * 0.1, (i + 1) * 0.1,
               result->bins[i], result->expected_per_bin, diff);
    }

    printf("------------------------------------------------------------\n");
    printf("Chi-square: %.4f   P-value: %.6f\n",
           result->chi_square, result->p_value);
    printf("------------------------------------------------------------\n");

    if (result->passed) {
        printf("*** UNIFORMITY TEST PASSED (P-value >= 0.0001) ***\n");
    } else {
        printf("*** UNIFORMITY TEST FAILED (P-value < 0.0001) ***\n");
        printf("    P-values are NOT uniformly distributed.\n");
        printf("    This indicates potential non-randomness.\n");
    }
    printf("============================================================\n");
}
