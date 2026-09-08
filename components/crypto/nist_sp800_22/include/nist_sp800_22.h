/**
 * @file nist_sp800_22.h
 * @brief NIST SP 800-22 Rev 1a Statistical Test Suite for Random Number Generators
 *
 * This library implements all 15 statistical tests defined in NIST Special Publication
 * 800-22 Revision 1a "A Statistical Test Suite for Random and Pseudorandom Number
 * Generators for Cryptographic Applications".
 *
 * Reference: https://csrc.nist.gov/publications/detail/sp/800-22/rev-1a/final
 *
 * @note This is a hardware-independent implementation. Users must provide a TRNG HAL
 *       function to generate random bytes for testing.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 IPRO Technology Co., Ltd.
 */

#ifndef NIST_SP800_22_H
#define NIST_SP800_22_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nist_sp800_22 NIST SP 800-22 Statistical Tests
 * @{
 */

/* ============================================================================
 * Configuration
 * ============================================================================ */

/** Default significance level (alpha) for hypothesis testing */
#define NIST_DEFAULT_ALPHA          0.01

/**
 * Default number of bits for testing: 2^20 = 1048576.
 *
 * NIST recommends at least 1 million bits; 2^20 satisfies that and, being a
 * power of 2, lets the DFT (Spectral) test run its radix-2 FFT over the whole
 * sequence instead of a power-of-2 prefix.
 */
#define NIST_DEFAULT_N_BITS         1048576

/** Minimum recommended bits for reliable results */
#define NIST_MIN_RECOMMENDED_BITS   100000

/* ============================================================================
 * Test Result Codes
 * ============================================================================ */

/** Test passed (P-value >= alpha) */
#define NIST_SUCCESS                0

/** Test failed (P-value < alpha) */
#define NIST_FAILURE                (-1)

/** Invalid input parameters */
#define NIST_ERROR_INVALID_PARAM    (-2)

/** Memory allocation failed */
#define NIST_ERROR_NO_MEMORY        (-3)

/** Insufficient data for the test */
#define NIST_ERROR_INSUFFICIENT_DATA (-4)

/** Test precondition not met (e.g., proportion check failed) */
#define NIST_ERROR_PRECONDITION     (-5)

/* ============================================================================
 * TRNG HAL Interface
 * ============================================================================ */

/**
 * @brief TRNG Hardware Abstraction Layer function type
 *
 * Users must implement this function to provide random bytes from their hardware.
 *
 * @param[out] buf    Buffer to store random bytes
 * @param[in]  len    Number of random bytes to generate
 * @return 0 on success, negative error code on failure
 */
typedef int (*nist_trng_hal_t)(uint8_t *buf, size_t len);

/* ============================================================================
 * Test Result Structure
 * ============================================================================ */

/**
 * @brief Result structure for a single NIST test
 */
typedef struct {
    const char *test_name;      /**< Name of the test */
    int         test_id;        /**< Test ID (2.1 - 2.15) */
    int         result;         /**< NIST_SUCCESS or error code */
    double      p_value;        /**< Computed P-value */
    double      p_value2;       /**< Secondary P-value (for tests with two values) */
    bool        passed;         /**< true if P-value >= alpha */
    const char *details;        /**< Optional details string */
    int         sub_passed;     /**< Number of sub-tests passed (for multi-state tests) */
    int         sub_total;      /**< Total number of sub-tests (for multi-state tests) */
} nist_test_result_t;

/**
 * @brief Summary result for all 15 NIST tests
 *
 * Note: Array size is 16 because Section 2.13 (Cumulative Sums) has both
 * forward and backward modes which are reported separately.
 */
typedef struct {
    nist_test_result_t tests[16];   /**< Individual test results (16 entries) */
    int     tests_run;              /**< Number of tests executed */
    int     tests_passed;           /**< Number of tests passed */
    int     tests_failed;           /**< Number of tests failed */
    int     tests_error;            /**< Number of tests with errors */
    double  alpha;                  /**< Significance level used */
    uint32_t n_bits;                /**< Number of bits tested */
    uint32_t elapsed_ms;            /**< Total execution time in ms */
    int     excursions_retries;     /**< Deprecated, always 0 (J<500 now reports N/A, no retry) */
    int     tests_na;               /**< Tests not applicable (e.g. Random Excursions J<500);
                                         excluded from pass/fail statistics */
} nist_suite_result_t;

/** Max non-overlapping templates (full NIST m=9 set) */
#define NIST_MAX_TEMPLATES  148

/**
 * @brief Per-sub-stream P-values for the multi-P tests of one round
 *
 * NIST SP 800-22 Section 4 treats each non-overlapping template (148),
 * each Random Excursions state (8), and each RE-Variant state (18) as its
 * OWN P-value stream, evaluated independently for 4.2.1 proportion and
 * 4.2.2 uniformity. This carries those per-round sub-stream P-values out of
 * the suite runner so a multi-round harness can build the full per-stream
 * ledger (188 streams with Serial's two) rather than only a min-P.
 */
typedef struct {
    double nonoverlap[NIST_MAX_TEMPLATES]; /**< Per-template P-values */
    int    nonoverlap_n;                   /**< Templates evaluated (148/10), 0 on error */
    double re[8];                          /**< Per-state RE P-values */
    int    re_valid;                       /**< 1 if J>=500 this round, else 0 */
    double rev[18];                        /**< Per-state REV P-values */
    int    rev_valid;                      /**< 1 if J>=500 this round, else 0 */
} nist_substream_pvals_t;

/* ============================================================================
 * Individual Test Functions (NIST SP 800-22 Section 2.1 - 2.15)
 * ============================================================================ */

/**
 * @brief 2.1 Frequency (Monobit) Test
 *
 * Tests if the proportion of ones in the sequence is approximately 1/2.
 *
 * @param[in]  data     Bit sequence (each byte contains 8 bits, MSB first)
 * @param[in]  n_bits   Number of bits to test (must be >= 100)
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_frequency_test(const uint8_t *data, uint32_t n_bits, double *p_value);

/**
 * @brief 2.2 Frequency Test within a Block
 *
 * Tests if the proportion of ones in M-bit blocks is approximately M/2.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test
 * @param[in]  M        Block size in bits (recommended: M >= 20, M > 0.01*n)
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_block_frequency_test(const uint8_t *data, uint32_t n_bits, uint32_t M, double *p_value);

/**
 * @brief 2.3 Runs Test
 *
 * Tests if the number of runs (uninterrupted sequences of identical bits)
 * is as expected for a random sequence.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_runs_test(const uint8_t *data, uint32_t n_bits, double *p_value);

/**
 * @brief 2.4 Test for the Longest Run of Ones in a Block
 *
 * Tests if the longest run of ones within M-bit blocks is consistent
 * with what is expected for a random sequence.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test (minimum depends on M)
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_longest_run_test(const uint8_t *data, uint32_t n_bits, double *p_value);

/**
 * @brief 2.5 Binary Matrix Rank Test
 *
 * Tests the rank of disjoint sub-matrices of the entire sequence.
 * Focuses on linear dependence among fixed-length substrings.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test (minimum 38912 for 32x32 matrices)
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_matrix_rank_test(const uint8_t *data, uint32_t n_bits, double *p_value);

/**
 * @brief 2.6 Discrete Fourier Transform (Spectral) Test
 *
 * Tests the peak heights in the DFT of the sequence against the fixed
 * theoretical threshold T = sqrt(ln(1/0.05) * n). Detects periodic
 * features that would indicate a deviation from randomness.
 *
 * The full-length DFT is computed with a radix-2 FFT, which needs a
 * power-of-2 length: the largest power-of-2 prefix of the sequence is
 * tested. Pass a power-of-2 n_bits (e.g. the default 1048576) to use
 * every bit.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test (minimum 1000)
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_spectral_test(const uint8_t *data, uint32_t n_bits, double *p_value);

/**
 * @brief 2.7 Non-overlapping Template Matching Test
 *
 * Tests the number of occurrences of pre-specified target strings.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test
 * @param[in]  m        Template length in bits; must be 9 (both template
 *                      sets are official NIST m=9 aperiodic templates)
 * @param[out] p_value  Computed P-value (minimum across all templates)
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_non_overlapping_template_test(const uint8_t *data, uint32_t n_bits,
                                        uint32_t m, double *p_value);

/**
 * @brief 2.7 Non-overlapping Template Matching Test (Extended version)
 *
 * Extended version with early exit option and pass count reporting.
 *
 * @param[in]  data          Input bit sequence (MSB first)
 * @param[in]  n_bits        Length of sequence in bits
 * @param[in]  m             Template length; must be 9
 * @param[in]  early_exit    If true, exit on first template failure
 * @param[out] p_value       Minimum P-value across all templates
 * @param[out] passed_count  Number of templates that passed (can be NULL)
 * @param[out] total_count   Total number of templates tested (can be NULL)
 * @return NIST_SUCCESS on all pass, NIST_FAILURE if any fail, or negative error code
 */
int nist_non_overlapping_template_test_ex(const uint8_t *data, uint32_t n_bits,
                                          uint32_t m, int early_exit,
                                          double *p_value,
                                          int *passed_count, int *total_count);

/**
 * @brief 2.8 Overlapping Template Matching Test
 *
 * Tests the number of occurrences of the m-bit run of ones.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test
 * @param[in]  m        Template length in bits; must be 9 (the only length
 *                      with published NIST parameters M=1032, K=5, pi[])
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_overlapping_template_test(const uint8_t *data, uint32_t n_bits,
                                    uint32_t m, double *p_value);

/**
 * @brief 2.9 Maurer's "Universal Statistical" Test
 *
 * Tests the number of bits between matching patterns. Detects whether
 * the sequence can be significantly compressed.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test (minimum depends on L)
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_universal_test(const uint8_t *data, uint32_t n_bits, double *p_value);

/**
 * @brief 2.10 Linear Complexity Test
 *
 * Tests if the sequence is complex enough to be considered random.
 * Uses Berlekamp-Massey algorithm to determine the shortest LFSR.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test
 * @param[in]  M        Block size (500 <= M <= 5000, default 500)
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_linear_complexity_test(const uint8_t *data, uint32_t n_bits,
                                 uint32_t M, double *p_value);

/**
 * @brief 2.11 Serial Test
 *
 * Tests the frequency of all possible overlapping m-bit patterns.
 *
 * @param[in]  data      Bit sequence
 * @param[in]  n_bits    Number of bits to test
 * @param[in]  m         Pattern length (m < floor(log2(n)) - 2)
 * @param[out] p_value1  P-value for first statistic
 * @param[out] p_value2  P-value for second statistic
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_serial_test(const uint8_t *data, uint32_t n_bits, uint32_t m,
                      double *p_value1, double *p_value2);

/**
 * @brief 2.12 Approximate Entropy Test
 *
 * Compares the frequency of overlapping blocks of two consecutive lengths.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test
 * @param[in]  m        Block length (m < floor(log2(n)) - 5)
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_approximate_entropy_test(const uint8_t *data, uint32_t n_bits,
                                   uint32_t m, double *p_value);

/**
 * @brief 2.13 Cumulative Sums Test (Forward)
 *
 * Tests the maximal excursion of the random walk defined by the cumulative sum.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_cusum_forward_test(const uint8_t *data, uint32_t n_bits, double *p_value);

/**
 * @brief 2.13 Cumulative Sums Test (Backward)
 *
 * Same as forward test but processes the sequence in reverse.
 *
 * @param[in]  data     Bit sequence
 * @param[in]  n_bits   Number of bits to test
 * @param[out] p_value  Computed P-value
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_cusum_backward_test(const uint8_t *data, uint32_t n_bits, double *p_value);

/**
 * @brief 2.14 Random Excursions Test
 *
 * Tests the number of cycles having exactly K visits in a cumulative sum random walk.
 *
 * @param[in]  data      Bit sequence
 * @param[in]  n_bits    Number of bits to test
 * @param[out] p_values  Array of 8 P-values for states {-4,-3,-2,-1,+1,+2,+3,+4}
 * @param[out] min_pval  Minimum P-value across all states
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_random_excursions_test(const uint8_t *data, uint32_t n_bits,
                                 double p_values[8], double *min_pval);

/**
 * @brief 2.15 Random Excursions Variant Test
 *
 * Tests the total number of times a particular state is visited in a random walk.
 *
 * @param[in]  data      Bit sequence
 * @param[in]  n_bits    Number of bits to test
 * @param[out] p_values  Array of 18 P-values for states {-9...-1,+1...+9}
 * @param[out] min_pval  Minimum P-value across all states
 * @return NIST_SUCCESS on pass, NIST_FAILURE on fail, or negative error code
 */
int nist_random_excursions_variant_test(const uint8_t *data, uint32_t n_bits,
                                         double p_values[18], double *min_pval);

/**
 * @brief Count zero crossings (J) for Random Excursions precondition check
 *
 * This function computes the number of zero crossings in the cumulative sum
 * sequence without running the full test. Useful for checking if data meets
 * the J >= 500 precondition before running the actual tests.
 *
 * @param[in]  data    Bit sequence
 * @param[in]  n_bits  Number of bits to test
 * @return Number of zero crossings (J), or 0 on error
 */
uint32_t nist_count_zero_crossings(const uint8_t *data, uint32_t n_bits);

/* ============================================================================
 * Full Test Suite Functions
 * ============================================================================ */

/**
 * @brief Run all 15 NIST SP 800-22 tests on provided data
 *
 * @param[in]  data     Bit sequence to test
 * @param[in]  n_bits   Number of bits in the sequence
 * @param[in]  alpha    Significance level (use NIST_DEFAULT_ALPHA for 0.01)
 * @param[out] result   Suite result structure
 * @return Number of tests passed, or negative error code
 *
 * @note Random Excursions tests with J < 500 are reported N/A (tests_na)
 *       and excluded from pass/fail statistics, per the spec.
 */
int nist_run_all_tests(const uint8_t *data, uint32_t n_bits, double alpha,
                        nist_suite_result_t *result);

/**
 * @brief Run all 15 NIST SP 800-22 tests (same behavior as nist_run_all_tests)
 *
 * All tests are applied to the one provided sequence. When Random
 * Excursions (15/16) hit J < 500, they are reported N/A per the spec
 * (tests_na) and excluded from pass/fail statistics -- data is never
 * regenerated mid-round, since all 15 tests must see the same sequence.
 *
 * @param[in]     data      Bit sequence to test
 * @param[in]     n_bits    Number of bits in the sequence
 * @param[in]     alpha     Significance level (use NIST_DEFAULT_ALPHA for 0.01)
 * @param[out]    result    Suite result structure
 * @param[in]     trng_hal  Unused; retained for API compatibility
 * @return Number of tests passed, or negative error code
 */
int nist_run_all_tests_ex(uint8_t *data, uint32_t n_bits, double alpha,
                           nist_suite_result_t *result, nist_trng_hal_t trng_hal);

/**
 * @brief Run all tests and also emit per-sub-stream P-values
 *
 * Same as nist_run_all_tests but additionally fills @p sub with the
 * per-template / per-state P-values of the multi-P tests (non-overlap,
 * Random Excursions, RE-Variant) so a multi-round caller can evaluate
 * Section 4.2.1 + 4.2.2 per stream (full sts-style 188-stream ledger).
 *
 * @param[out] sub  Per-sub-stream P-values for this round (may be NULL)
 * @return Number of tests passed, or negative error code
 */
int nist_run_all_tests_sub(uint8_t *data, uint32_t n_bits, double alpha,
                            nist_suite_result_t *result,
                            nist_substream_pvals_t *sub);

/**
 * @brief Run all 15 NIST tests using TRNG HAL to generate data
 *
 * This function allocates a buffer, generates random data using the provided
 * TRNG HAL function, runs all tests, and frees the buffer.
 *
 * @param[in]  trng_hal  TRNG hardware abstraction function
 * @param[in]  n_bits    Number of bits to test
 * @param[in]  alpha     Significance level
 * @param[out] result    Suite result structure
 * @return Number of tests passed, or negative error code
 */
int nist_run_suite_with_hal(nist_trng_hal_t trng_hal, uint32_t n_bits,
                             double alpha, nist_suite_result_t *result);

/**
 * @brief Print test results to console
 *
 * @param[in] result  Suite result to print
 */
void nist_print_results(const nist_suite_result_t *result);

/**
 * @brief Get error string for result code
 *
 * @param[in] code  Result code
 * @return Human-readable error string
 */
const char *nist_strerror(int code);

/**
 * @brief Calculate NIST SP 800-22 proportion test threshold
 *
 * Per NIST SP 800-22 Section 4.2.1, the minimum acceptable proportion of
 * sequences passing a test is:
 *   threshold = (1 - alpha) - 3 * sqrt(alpha * (1 - alpha) / n)
 *
 * @param[in] n      Number of sequences/samples
 * @param[in] alpha  Significance level (use NIST_DEFAULT_ALPHA for 0.01)
 * @return Minimum acceptable pass ratio (0.0 to 1.0)
 */
double nist_proportion_threshold(int n, double alpha);

/**
 * @brief Check if pass ratio meets NIST proportion test requirement
 *
 * @param[in] passed  Number of sequences that passed
 * @param[in] total   Total number of sequences tested
 * @param[in] alpha   Significance level
 * @return true if pass ratio >= threshold, false otherwise
 */
bool nist_proportion_check(int passed, int total, double alpha);

/* ============================================================================
 * P-value Uniformity Test (NIST SP 800-22 Section 4.2.2)
 * ============================================================================ */

/**
 * @brief Result structure for P-value uniformity test
 */
typedef struct {
    double chi_square;          /**< Chi-square statistic */
    double p_value;             /**< P-value of uniformity test */
    bool   passed;              /**< true if P-value >= alpha */
    int    bins[10];            /**< Histogram bins [0.0-0.1), [0.1-0.2), ..., [0.9-1.0] */
    int    total_samples;       /**< Total number of P-values tested */
    double expected_per_bin;    /**< Expected count per bin (total/10) */
} nist_uniformity_result_t;

/**
 * @brief Perform P-value uniformity test (NIST SP 800-22 Section 4.2.2)
 *
 * This test checks whether the P-values from multiple test runs are
 * uniformly distributed in the interval [0, 1]. The test divides [0,1]
 * into 10 sub-intervals and uses chi-square goodness-of-fit test.
 *
 * Per NIST: "If the P-value is less than 0.0001, then the sequences
 * can be considered to be non-random."
 *
 * @param[in]  p_values     Array of P-values from multiple test runs
 * @param[in]  count        Number of P-values in the array
 * @param[out] result       Uniformity test result
 * @return NIST_SUCCESS if uniformity test passes, NIST_FAILURE otherwise
 */
int nist_pvalue_uniformity_test(const double *p_values, int count,
                                 nist_uniformity_result_t *result);

/**
 * @brief Section 4.2.2 uniformity from a pre-binned 10-bin histogram
 *
 * Same chi-square (df=9) / P_T = igamc(9/2, chi2/2) as
 * nist_pvalue_uniformity_test, but takes an accumulated histogram instead
 * of the raw P-values -- for a multi-round caller keeping only per-stream
 * bin counts (188-stream ledger) rather than every sub-stream P-value.
 *
 * @param[in]  bins        10-bin histogram [0,0.1)...[0.9,1.0]
 * @param[in]  total       sum of bins (>= 10)
 * @param[out] chi_square  computed chi-square
 * @param[out] p_value     P_T
 * @return NIST_SUCCESS if P_T >= 0.0001, NIST_FAILURE otherwise, or error
 */
int nist_uniformity_from_hist(const int bins[10], int total,
                              double *chi_square, double *p_value);

/**
 * @brief Print P-value uniformity test result
 *
 * @param[in] result  Uniformity test result to print
 */
void nist_print_uniformity_result(const nist_uniformity_result_t *result);

/** @} */ /* end of nist_sp800_22 group */

#ifdef __cplusplus
}
#endif

#endif /* NIST_SP800_22_H */
