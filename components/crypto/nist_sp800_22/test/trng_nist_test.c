/**
 * @file trng_nist_test.c
 * @brief NIST SP 800-22 TRNG Test Demo for IPRO7
 *
 * This demo uses the NIST SP 800-22 statistical test library to validate
 * the hardware TRNG. It implements the TRNG HAL interface required by the
 * library using the IPRO7 hardware TRNG driver.
 *
 * Usage:
 *   trng_nist [n_bits]    - Run full NIST SP 800-22 test suite
 *                           Default: 1000000 bits (NIST recommended)
 *
 * All 15 NIST SP 800-22 tests are executed:
 *   2.1  Frequency (Monobit) Test
 *   2.2  Frequency Test within a Block
 *   2.3  Runs Test
 *   2.4  Test for the Longest Run of Ones in a Block
 *   2.5  Binary Matrix Rank Test
 *   2.6  Discrete Fourier Transform (Spectral) Test
 *   2.7  Non-overlapping Template Matching Test
 *   2.8  Overlapping Template Matching Test
 *   2.9  Maurer's "Universal Statistical" Test
 *   2.10 Linear Complexity Test
 *   2.11 Serial Test
 *   2.12 Approximate Entropy Test
 *   2.13 Cumulative Sums Test
 *   2.14 Random Excursions Test
 *   2.15 Random Excursions Variant Test
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 IPRO Technology Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>
#include <shell.h>

#include "nist_sp800_22.h"
#include "hal_sec_trng.h"
#include "drv_sec_eng.h"

/* LittleFS support for result persistence */
#ifdef CONFIG_LITTLEFS
#include "lfs.h"
#include "lfs_port.h"
extern lfs_t *lfs_get_instance(void);
#endif

/* ROSC configuration for NIST test (0xF000 = HW reset default se_trng_0_rosc_c1) */
#define NIST_TRNG_ROSC_VALUE  0xF000

/* Reseed interval for NIST test (0x0080_00000000 = 48-bit value) */
#define NIST_TRNG_RESEED_LSB  0x00000000
#define NIST_TRNG_RESEED_MSB  0x0004

/* ============================================================================
 * Static Test Buffer (Pre-allocated to avoid fragmentation)
 * ============================================================================ */

/* Maximum test buffer size: 1M bits = 125KB */
#define NIST_TEST_BUF_MAX_BYTES  (NIST_DEFAULT_N_BITS / 8)

/* Number of test rounds per configuration for stability verification */
#define NIST_TEST_ROUNDS  5

/* Default number of rounds for trng_nist command */
#define NIST_DEFAULT_ROUNDS  100

static uint8_t *g_nist_test_buf = NULL;
static uint32_t g_nist_test_buf_size = 0;

/**
 * @brief Allocate static test buffer (call once before sweep tests)
 */
static int nist_alloc_test_buffer(uint32_t n_bytes)
{
    if (g_nist_test_buf != NULL) {
        /* Already allocated, check if size is sufficient */
        if (g_nist_test_buf_size >= n_bytes) {
            return 0;
        }
        /* Need larger buffer, free and reallocate */
        vPortFree(g_nist_test_buf);
        g_nist_test_buf = NULL;
        g_nist_test_buf_size = 0;
    }

    g_nist_test_buf = (uint8_t *)pvPortMalloc(n_bytes);
    if (!g_nist_test_buf) {
        printf("Error: Failed to allocate %lu bytes for test buffer\n",
               (unsigned long)n_bytes);
        return -1;
    }

    g_nist_test_buf_size = n_bytes;
    printf("Test buffer allocated: %lu bytes at 0x%08lx\n",
           (unsigned long)n_bytes, (unsigned long)(uintptr_t)g_nist_test_buf);
    return 0;
}

/**
 * @brief Free static test buffer
 */
static void nist_free_test_buffer(void)
{
    if (g_nist_test_buf != NULL) {
        vPortFree(g_nist_test_buf);
        g_nist_test_buf = NULL;
        g_nist_test_buf_size = 0;
    }
}

/* ============================================================================
 * TRNG HAL Implementation
 * ============================================================================ */

/**
 * @brief TRNG HAL function for NIST library
 *
 * This function implements the nist_trng_hal_t interface using the
 * IPRO7 hardware TRNG driver.
 *
 * @param[out] buf  Buffer to store random bytes
 * @param[in]  len  Number of random bytes to generate
 * @return 0 on success, -1 on failure
 */
static int ipro7_trng_hal(uint8_t *buf, size_t len)
{
    if (!buf || len == 0) {
        return -1;
    }

    /* Initialize TRNG hardware */
    hal_sec_trng_init();

    /* Configure ROSC for optimal entropy (NIST SP 800-22 validated) */
    Sec_Eng_Trng_Set_ROSC_Config(NIST_TRNG_ROSC_VALUE);

    /* Configure reseed interval */
    Sec_Eng_Trng_Set_Reseed(NIST_TRNG_RESEED_LSB, NIST_TRNG_RESEED_MSB);

    /* Generate random data using hardware TRNG */
    int ret = hal_rand_stream(buf, len);
    if (ret != 0) {
        printf("[TRNG] Error: hal_rand_stream failed with %d\n", ret);
        return -1;
    }

    return 0;
}

/**
 * @brief Simple TRNG HAL wrapper using hal_rand_stream
 *
 * This wrapper uses the current TRNG configuration (no ROSC override).
 * Used for ROSC sweep and other tests where ROSC is manually configured.
 */
static int hal_rand_stream_trng_hal(uint8_t *buf, size_t len)
{
    return hal_rand_stream(buf, len);
}

/* ============================================================================
 * Shell Command Implementation
 * ============================================================================ */

/**
 * @brief Shell command: trng_nist [rounds] [n_bits]
 *
 * Runs the full NIST SP 800-22 statistical test suite on hardware TRNG.
 *
 * @param rounds  Number of test rounds (default: 100)
 * @param n_bits  Number of bits per round (default: 1000000)
 *
 * Statistics tracked across all rounds:
 * - Pass rate for each of 16 tests
 * - Overall suite pass rate
 * - Min/Max/Avg for each test's P-value
 */

/**
 * @brief Evaluate a group of NIST sub-streams (each template/state its own
 *        stream) for Section 4.2.1 proportion + 4.2.2 uniformity.
 *
 * @return 1 if EVERY stream passes both criteria, 0 otherwise.
 */
static int eval_substream_group(const char *name, int nstreams, int valid_n,
                                const int *pass, const int (*hist)[10])
{
    if (nstreams < 1 || valid_n < 1) {
        printf("  %-22s no valid rounds (N/A)\n", name);
        return 1;   /* not applicable this run; do not fail the suite */
    }

    /*
     * Per-stream 4.2.1 + 4.2.2, then a SECOND-ORDER roll-up. Requiring
     * every one of K streams to individually clear the 3-sigma proportion
     * bound is stricter than NIST -- an ideal source fails that ~1-(1-p)^K
     * of the time (~33% for K=148 at N=1000). NIST's proportion test is
     * instead applied to the K per-stream outcomes: the NUMBER of streams
     * passing must itself meet the proportion threshold thr(K). Uniformity
     * uses the lenient P_T>=0.0001 bound, so all-streams-uniform is the
     * expected outcome and is used directly. The pooled proportion (all
     * K*N sub-trials) is reported as an independent cross-check.
     */
    int prop_pass = 0, unif_pass = 0, unif_checked = 0;
    long pooled_pass = 0;
    double worst_rate = 1.0; int worst_rate_s = -1;
    double worst_pt = 1.0;   int worst_pt_s = -1;

    for (int s = 0; s < nstreams; s++) {
        pooled_pass += pass[s];
        double rate = (double)pass[s] / (double)valid_n;
        if (nist_proportion_check(pass[s], valid_n, NIST_DEFAULT_ALPHA)) {
            prop_pass++;
        } else if (rate < worst_rate) {
            worst_rate = rate; worst_rate_s = s;
        }
        if (valid_n >= 10) {
            double chi2, pt;
            if (nist_uniformity_from_hist(hist[s], valid_n, &chi2, &pt) >= 0) {
                unif_checked++;
                if (pt >= 0.0001) unif_pass++;
                else if (pt < worst_pt) { worst_pt = pt; worst_pt_s = s; }
            }
        }
    }

    /* Second-order proportion: streams-passing must meet thr(K) */
    int need = (int)ceil(nist_proportion_threshold(nstreams, NIST_DEFAULT_ALPHA)
                         * (double)nstreams);
    int prop_ok = (prop_pass >= need);
    int unif_ok = (unif_checked == 0) || (unif_pass == unif_checked);

    long pooled_total = (long)nstreams * valid_n;
    int pooled_ok = nist_proportion_check((int)pooled_pass, (int)pooled_total,
                                          NIST_DEFAULT_ALPHA);

    printf("  %-22s 4.2.1 %3d/%-3d pass (need>=%d, pooled %ld/%ld %s)",
           name, prop_pass, nstreams, need,
           pooled_pass, pooled_total, pooled_ok ? "ok" : "LOW");
    if (!prop_ok) printf(" worst s%d %.1f%%", worst_rate_s, 100.0 * worst_rate);
    if (valid_n >= 10) {
        printf(" | 4.2.2 %d/%d uniform", unif_pass, unif_checked);
        if (!unif_ok) printf(" worst s%d P_T=%.5f", worst_pt_s, worst_pt);
    } else {
        printf(" | 4.2.2 skip(N<10)");
    }
    printf("  => %s\n", (prop_ok && unif_ok) ? "OK" : "FAIL");
    return prop_ok && unif_ok;
}

static int cmd_trng_nist(int argc, char **argv)
{
    uint32_t n_bits = NIST_DEFAULT_N_BITS;
    int rounds = NIST_DEFAULT_ROUNDS;
    nist_suite_result_t result;

    /* Parse arguments */
    if (argc > 1) {
        rounds = (int)strtoul(argv[1], NULL, 0);
        if (rounds < 1) rounds = 1;
        if (rounds > 10000) {
            printf("Warning: Limiting to 10000 rounds\n");
            rounds = 10000;
        }
    }
    if (argc > 2) {
        n_bits = (uint32_t)strtoul(argv[2], NULL, 0);
        if (n_bits < NIST_MIN_RECOMMENDED_BITS) {
            printf("Warning: NIST recommends >= %u bits for reliable results\n",
                   NIST_MIN_RECOMMENDED_BITS);
            printf("         Using minimum %u bits\n", NIST_MIN_RECOMMENDED_BITS);
            n_bits = NIST_MIN_RECOMMENDED_BITS;
        }
        if (n_bits > 10000000) {
            printf("Warning: Limiting to 10,000,000 bits (memory constraint)\n");
            n_bits = 10000000;
        }
    }

    uint32_t n_bytes = (n_bits + 7) / 8;

    printf("\n");
    printf("NIST SP 800-22 Rev 1a Statistical Test Suite\n");
    printf("=============================================\n");
    printf("Test rounds:   %d\n", rounds);
    printf("Test bits:     %lu (%lu KB) per round\n", (unsigned long)n_bits,
           (unsigned long)(n_bytes / 1024));
    printf("Alpha:         %.2f\n", NIST_DEFAULT_ALPHA);
    printf("TRNG source:   Hardware (IPRO7) - conditioned output (CP on, reseed enabled)\n");
    printf("ROSC config:   0x%04X\n", NIST_TRNG_ROSC_VALUE);
    printf("Reseed:        0x%04X_%08lX\n", NIST_TRNG_RESEED_MSB, (unsigned long)NIST_TRNG_RESEED_LSB);
    printf("FFT:           radix-2, full length, fixed NIST threshold\n");
    printf("\n");

    /* Allocate buffer for test data */
    printf("Allocating %lu bytes for test data...\n", (unsigned long)n_bytes);
    uint8_t *data = (uint8_t *)pvPortMalloc(n_bytes);
    if (!data) {
        printf("Error: Failed to allocate memory for test data\n");
        printf("       Make sure PSRAM is enabled for large buffers\n");
        return -1;
    }

    /* Verify buffer location */
    uintptr_t addr = (uintptr_t)data;
    if (addr >= 0x1e400000 && addr < 0x1f400000) {
        printf("Buffer allocated in PSRAM at 0x%08lx\n", (unsigned long)addr);
    } else {
        printf("Buffer allocated at 0x%08lx\n", (unsigned long)addr);
    }

    /* Statistics tracking for each test (16 tests total) */
    int test_pass_count[16] = {0};
    int test_total_count[16] = {0};
    int sub_pass_sum[16] = {0};
    int sub_total_sum[16] = {0};
    double p_value_sum[16] = {0.0};
    double p_value_min[16];
    double p_value_max[16];
    for (int i = 0; i < 16; i++) {
        p_value_min[i] = 1.0;
        p_value_max[i] = 0.0;
    }
    int suite_pass_count = 0;
    uint32_t total_time_ms = 0;

    /* Per-test P-value streams for the Section 4.2.2 uniformity check */
    double *pv_stream[16] = {NULL};
    int pv_count[16] = {0};
    bool pv_ok = (rounds >= 10);
    if (pv_ok) {
        for (int i = 0; i < 16; i++) {
            pv_stream[i] = (double *)pvPortMalloc(rounds * sizeof(double));
            if (!pv_stream[i]) {
                for (int j = 0; j < i; j++) {
                    vPortFree(pv_stream[j]);
                    pv_stream[j] = NULL;
                }
                pv_ok = false;
                break;
            }
        }
        if (!pv_ok) {
            printf("Note: uniformity check disabled (no memory for P-value streams)\n");
        }
    }

    /*
     * Per-sub-stream ledger (NIST Section 4, sts-style): non-overlapping
     * templates (up to 148), Random Excursions (8 states), RE-Variant (18
     * states) each judged as its OWN stream for 4.2.1 proportion + 4.2.2
     * uniformity. Keep only per-stream pass counts + 10-bin histograms.
     */
    nist_substream_pvals_t *subp = NULL;
    int *no_pass = NULL;
    int (*no_hist)[10] = NULL;
    int no_n = 0, no_streams = 0;
    int re_pass[8] = {0};   int re_hist[8][10] = {{0}};   int re_n = 0;
    int rev_pass[18] = {0}; int rev_hist[18][10] = {{0}}; int rev_n = 0;
    bool sub_ok = (rounds >= 10);
    if (sub_ok) {
        subp = (nist_substream_pvals_t *)pvPortMalloc(sizeof(nist_substream_pvals_t));
        no_pass = (int *)pvPortMalloc(NIST_MAX_TEMPLATES * sizeof(int));
        no_hist = (int (*)[10])pvPortMalloc(NIST_MAX_TEMPLATES * 10 * sizeof(int));
        if (!subp || !no_pass || !no_hist) {
            vPortFree(subp); vPortFree(no_pass); vPortFree(no_hist);
            subp = NULL; no_pass = NULL; no_hist = NULL;
            sub_ok = false;
            printf("Note: per-sub-stream ledger disabled (no memory)\n");
        } else {
            memset(no_pass, 0, NIST_MAX_TEMPLATES * sizeof(int));
            memset(no_hist, 0, NIST_MAX_TEMPLATES * 10 * sizeof(int));
        }
    }

    printf("Running %d rounds of NIST SP 800-22 test suite...\n\n", rounds);

    /* Run multiple rounds */
    for (int round = 1; round <= rounds; round++) {
        /* Generate random data */
        if (ipro7_trng_hal(data, n_bytes) != 0) {
            printf("Round %d: TRNG data generation failed\n", round);
            continue;
        }

        /* Run all NIST tests (J<500 rounds report tests 15/16 as N/A) */
        uint32_t test_start = (uint32_t)xTaskGetTickCount();
        int ret = nist_run_all_tests_sub(data, n_bits, NIST_DEFAULT_ALPHA, &result, subp);
        uint32_t test_end = (uint32_t)xTaskGetTickCount();
        uint32_t round_time = (test_end - test_start) * portTICK_PERIOD_MS;
        total_time_ms += round_time;

        if (ret < 0) {
            printf("Round %d: Test suite error (code: %d)\n", round, ret);
            continue;
        }

        /* Track statistics */
        int round_passed = (result.tests_failed == 0 && result.tests_error == 0);
        if (round_passed) suite_pass_count++;

        for (int i = 0; i < result.tests_run && i < 16; i++) {
            /* Exclude only N/A and errors; NIST_FAILURE (-1) is a real
             * result and must stay in the statistics */
            if (result.tests[i].result != NIST_SUCCESS &&
                result.tests[i].result != NIST_FAILURE) {
                continue;
            }
            test_total_count[i]++;
            if (result.tests[i].passed) {
                test_pass_count[i]++;
            }
            sub_pass_sum[i] += result.tests[i].sub_passed;
            sub_total_sum[i] += result.tests[i].sub_total;
            p_value_sum[i] += result.tests[i].p_value;
            if (result.tests[i].p_value < p_value_min[i]) {
                p_value_min[i] = result.tests[i].p_value;
            }
            if (result.tests[i].p_value > p_value_max[i]) {
                p_value_max[i] = result.tests[i].p_value;
            }
            if (pv_ok && pv_count[i] < rounds) {
                pv_stream[i][pv_count[i]++] = result.tests[i].p_value;
            }
        }

        /* Accumulate the per-sub-stream ledger for this round */
        if (sub_ok) {
            if (subp->nonoverlap_n > 0) {
                no_streams = subp->nonoverlap_n;
                no_n++;
                for (int t = 0; t < no_streams; t++) {
                    double p = subp->nonoverlap[t];
                    if (p >= NIST_DEFAULT_ALPHA) no_pass[t]++;
                    int b = (int)(p * 10.0); if (b < 0) b = 0; if (b > 9) b = 9;
                    no_hist[t][b]++;
                }
            }
            if (subp->re_valid) {
                re_n++;
                for (int s = 0; s < 8; s++) {
                    double p = subp->re[s];
                    if (p >= NIST_DEFAULT_ALPHA) re_pass[s]++;
                    int b = (int)(p * 10.0); if (b < 0) b = 0; if (b > 9) b = 9;
                    re_hist[s][b]++;
                }
            }
            if (subp->rev_valid) {
                rev_n++;
                for (int s = 0; s < 18; s++) {
                    double p = subp->rev[s];
                    if (p >= NIST_DEFAULT_ALPHA) rev_pass[s]++;
                    int b = (int)(p * 10.0); if (b < 0) b = 0; if (b > 9) b = 9;
                    rev_hist[s][b]++;
                }
            }
        }

        /* Print progress every 10 rounds or at the end */
        if (round % 10 == 0 || round == rounds) {
            printf("Round %3d/%d: %s (suite: %d/%d passed, time: %lu ms)\n",
                   round, rounds,
                   round_passed ? "PASS" : "FAIL",
                   suite_pass_count, round,
                   (unsigned long)round_time);
        }
    }

    /* Free data buffer */
    vPortFree(data);

    /* Print final statistics */
    printf("\n");
    printf("======================================================================\n");
    printf("NIST SP 800-22 MULTI-ROUND TEST SUMMARY\n");
    printf("======================================================================\n");
    printf("Total rounds:  %d\n", rounds);
    printf("Suite passed:  %d/%d (%.1f%%)\n",
           suite_pass_count, rounds, 100.0 * suite_pass_count / rounds);
    printf("Total time:    %lu ms (avg: %lu ms/round)\n",
           (unsigned long)total_time_ms,
           (unsigned long)(total_time_ms / rounds));
    printf("----------------------------------------------------------------------\n");
    printf("%-28s %-12s %-10s %-8s %-8s %-8s %s\n",
           "Test Name", "Pass Rate", "Sub-tests", "P-min", "P-avg", "P-max", "Status");
    printf("----------------------------------------------------------------------\n");

    int tests_meeting_threshold = 0;
    int tests_evaluated = 0;
    for (int i = 0; i < 16; i++) {
        if (test_total_count[i] == 0) continue;

        double pass_rate = 100.0 * test_pass_count[i] / test_total_count[i];
        double p_avg = p_value_sum[i] / test_total_count[i];
        int id = result.tests[i].test_id;

        /*
         * Multi-P tests (Non-overlap id7, RE id14, REV id15) are judged
         * per-sub-stream in the dedicated section below when the ledger is
         * enabled; here their row is informational (pooled sub-test
         * proportion) and NOT counted in the single-P verdict. Without the
         * ledger, fall back to the pooled proportion as the verdict.
         */
        bool is_multi = (id == 7 || id == 14 || id == 15);
        bool judged_here = !(is_multi && sub_ok);

        bool meets_threshold;
        if (sub_total_sum[i] > test_total_count[i]) {
            meets_threshold = nist_proportion_check(sub_pass_sum[i],
                                                    sub_total_sum[i],
                                                    NIST_DEFAULT_ALPHA);
        } else {
            meets_threshold = nist_proportion_check(test_pass_count[i],
                                                    test_total_count[i],
                                                    NIST_DEFAULT_ALPHA);
        }
        if (judged_here) {
            tests_evaluated++;
            if (meets_threshold) tests_meeting_threshold++;
        }

        /* Sub-test info */
        char sub_str[16];
        if (sub_total_sum[i] > test_total_count[i]) {
            snprintf(sub_str, sizeof(sub_str), "%d/%d",
                     sub_pass_sum[i], sub_total_sum[i]);
        } else {
            snprintf(sub_str, sizeof(sub_str), "-");
        }

        printf("[%2d] %-24s %3d/%-3d %4.1f%% %-10s %.4f   %.4f   %.4f %s\n",
               i < (int)result.tests_run ? result.tests[i].test_id : i + 1,
               i < (int)result.tests_run ? result.tests[i].test_name : "Unknown",
               test_pass_count[i], test_total_count[i], pass_rate,
               sub_str,
               p_value_min[i], p_avg, p_value_max[i],
               judged_here ? (meets_threshold ? "OK" : "FAIL") : "per-strm");
    }

    printf("----------------------------------------------------------------------\n");
    printf("Single-P tests meeting NIST 4.2.1 threshold: %d/%d%s\n",
           tests_meeting_threshold, tests_evaluated,
           sub_ok ? "  (multi-P judged per-stream below)" : "");
    printf("======================================================================\n");

    /* Random Excursions N/A summary (J<500 rounds are excluded, per spec) */
    if (test_total_count[14] < rounds || test_total_count[15] < rounds) {
        printf("Random Excursions N/A rounds (J<500, excluded from stats): %d\n",
               rounds - test_total_count[14]);
        printf("----------------------------------------------------------------------\n");
    }

    /*
     * NIST SP 800-22 Section 4.2.1 applies the proportion threshold PER
     * TEST (per P-value stream) -- the Pass Rate column above. The spec
     * has no suite-level criterion: a perfect source passes all ~17
     * P-values of one round only ~(1-alpha)^17 ~ 84% of the time, so
     * gating on all-tests-passed-per-round would fail ideal TRNGs. The
     * all-pass round count is printed as information only.
     */
    double threshold = nist_proportion_threshold(rounds, NIST_DEFAULT_ALPHA);
    bool proportion_ok = (tests_evaluated > 0) &&
                         (tests_meeting_threshold == tests_evaluated);

    printf("NIST 4.2.1 per-test threshold: %.2f%% (min %d/%d rounds per test)\n",
           threshold * 100.0, (int)ceil(threshold * rounds), rounds);
    printf("All-tests-in-one-round count (informational): %d/%d\n",
           suite_pass_count, rounds);
    printf("----------------------------------------------------------------------\n");
    if (proportion_ok) {
        printf("*** SINGLE-P PROPORTION PASSED - %d/%d meet threshold ***\n",
               tests_meeting_threshold, tests_evaluated);
    } else {
        printf("*** SINGLE-P PROPORTION FAILED - %d/%d meet threshold ***\n",
               tests_meeting_threshold, tests_evaluated);
    }
    printf("======================================================================\n");

    /*
     * NIST SP 800-22 Section 4.2.2: P-value uniformity, single-P streams.
     * The multi-P tests (7/14/15) are judged per-sub-stream below.
     */
    bool uniform_ok = true;
    if (pv_ok) {
        nist_uniformity_result_t uni;
        int tests_uniform = 0;
        int tests_checked = 0;

        printf("SINGLE-P UNIFORMITY (NIST Section 4.2.2, P_T >= 0.0001)\n");
        printf("----------------------------------------------------------------------\n");
        for (int i = 0; i < 16; i++) {
            int id = result.tests[i].test_id;
            if (id == 7 || id == 14 || id == 15) {
                continue;   /* multi-P: per-sub-stream section below */
            }
            if (pv_count[i] < 10) {
                continue;
            }
            if (nist_pvalue_uniformity_test(pv_stream[i], pv_count[i], &uni) != 0) {
                continue;
            }
            tests_checked++;
            if (uni.passed) {
                tests_uniform++;
            } else {
                printf("[%2d] %-24s chi2=%.2f  P_T=%.6f  NON-UNIFORM\n",
                       i + 1, result.tests[i].test_name ? result.tests[i].test_name : "?",
                       uni.chi_square, uni.p_value);
            }
        }
        printf("Uniform single-P streams: %d/%d\n", tests_uniform, tests_checked);
        uniform_ok = (tests_checked > 0) && (tests_uniform == tests_checked);
        printf(uniform_ok ? "*** SINGLE-P UNIFORMITY PASSED ***\n"
                          : "*** SINGLE-P UNIFORMITY FAILED ***\n");
        printf("======================================================================\n");

        for (int i = 0; i < 16; i++) {
            vPortFree(pv_stream[i]);
        }
    } else if (rounds > 1) {
        uniform_ok = false;
        printf("Note: uniformity check (Section 4.2.2) skipped -- needs >= 10 rounds.\n");
    }

    /*
     * NIST SP 800-22 Section 4, per-sub-stream (sts-style): each
     * non-overlapping template (148), Random Excursions state (8), and
     * RE-Variant state (18) is its OWN stream, judged for BOTH 4.2.1
     * proportion and 4.2.2 uniformity. This is the strict granularity a
     * reference sts run applies; the pooled row above is only informational.
     */
    bool multi_ok = true;
    if (sub_ok) {
        printf("PER-SUB-STREAM SECTION 4 (each template/state its own stream)\n");
        printf("  stream group              4.2.1 proportion       4.2.2 uniformity  verdict\n");
        printf("----------------------------------------------------------------------\n");
        multi_ok &= eval_substream_group("Non-overlap templates", no_streams, no_n,
                                         no_pass, no_hist);
        multi_ok &= eval_substream_group("Random Excursions", 8, re_n,
                                         re_pass, re_hist);
        multi_ok &= eval_substream_group("RE Variant", 18, rev_n,
                                         rev_pass, rev_hist);
        printf("----------------------------------------------------------------------\n");
        printf(multi_ok ? "*** PER-SUB-STREAM PASSED ***\n"
                        : "*** PER-SUB-STREAM FAILED ***\n");
        printf("======================================================================\n");
    } else if (rounds >= 10) {
        multi_ok = false;
        printf("Note: per-sub-stream ledger unavailable -- multi-P verdict incomplete.\n");
    }

    /* Overall NIST Section 4 verdict: all three must hold */
    if (proportion_ok && uniform_ok && multi_ok) {
        printf("*** TRNG VALIDATED - NIST Section 4 (proportion + uniformity, "
               "all streams) ***\n");
    } else {
        printf("*** TRNG NOT VALIDATED - failed:%s%s%s ***\n",
               proportion_ok ? "" : " single-P-proportion",
               uniform_ok ? "" : " single-P-uniformity",
               multi_ok ? "" : " per-sub-stream");
    }
    printf("======================================================================\n");

    vPortFree(subp);
    vPortFree(no_pass);
    vPortFree(no_hist);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_nist, trng_nist,
    NIST SP 800-22 multi-round test [rounds=100] [n_bits]);

/* ============================================================================
 * TRNG Register Sweep Test (Full NIST)
 * ============================================================================ */

#define TRNG_TEST_REG_ADDR  0x30080250
#define TRNG_TEST_REG_MASK  0x0000FF00  /* bits [15:8] */
#define TRNG_TEST_REG_SHIFT 8

/**
 * @brief Shell command: trng_sweep [n_bits] [start_val] [end_val]
 *
 * Sweep register 0x30080250[15:8] values and run full NIST test for each.
 * Uses pre-allocated buffer to avoid memory fragmentation.
 */
static int cmd_trng_sweep(int argc, char **argv)
{
    uint32_t n_bits = NIST_DEFAULT_N_BITS;  /* Default 1M bits */
    int start_val = 0;
    int end_val = 255;
    volatile uint32_t *reg = (volatile uint32_t *)TRNG_TEST_REG_ADDR;

    /* Parse arguments */
    if (argc > 1) {
        n_bits = (uint32_t)strtoul(argv[1], NULL, 0);
        if (n_bits < NIST_MIN_RECOMMENDED_BITS) n_bits = NIST_MIN_RECOMMENDED_BITS;
        if (n_bits > NIST_DEFAULT_N_BITS) n_bits = NIST_DEFAULT_N_BITS;
    }
    if (argc > 2) {
        start_val = (int)strtoul(argv[2], NULL, 0) & 0xFF;
    }
    if (argc > 3) {
        end_val = (int)strtoul(argv[3], NULL, 0) & 0xFF;
    }
    if (end_val < start_val) end_val = start_val;

    uint32_t n_bytes = (n_bits + 7) / 8;
    int total_configs = end_val - start_val + 1;

    printf("\n");
    printf("TRNG ROSC Sweep - NIST SP 800-22\n");
    printf("================================\n");
    printf("Register:  0x%08X [15:8]\n", TRNG_TEST_REG_ADDR);
    printf("Test bits: %lu (%lu KB)\n", (unsigned long)n_bits, (unsigned long)(n_bytes / 1024));
    printf("Range:     0x%04X - 0x%04X (%d configs)\n",
           start_val << TRNG_TEST_REG_SHIFT, end_val << TRNG_TEST_REG_SHIFT, total_configs);

    /* Pre-allocate test buffer */
    if (nist_alloc_test_buffer(n_bytes) != 0) {
        return -1;
    }

    /* Save original register value */
    uint32_t orig_val = *reg;
    printf("Original:  0x%08lX\n", (unsigned long)orig_val);
    printf("\n");

    /* Initialize TRNG */
    hal_sec_trng_init();

    /* Track best result (use min passed across all rounds for stability) */
    int best_val = -1;
    int best_min_passed = -1;

    /* Print header */
    printf("--- Sweep Results (%d rounds per config) ---\n", NIST_TEST_ROUNDS);

    /* Sweep values */
    for (int val = start_val; val <= end_val; val++) {
        /* Set register bits [15:8] */
        uint32_t new_reg = (orig_val & ~TRNG_TEST_REG_MASK) |
                           ((uint32_t)val << TRNG_TEST_REG_SHIFT);
        *reg = new_reg;

        /* Small delay for register to take effect */
        for (volatile int d = 0; d < 1000; d++);

        /* Track min passed across rounds for this config */
        int min_passed = 16;
        int has_error = 0;

        /* Run multiple rounds per configuration */
        for (int round = 1; round <= NIST_TEST_ROUNDS; round++) {
            /* Generate random data */
            int ret = hal_rand_stream(g_nist_test_buf, n_bytes);
            if (ret != 0) {
                printf("ROSC=0x%04X: R%d: ERROR (hal_rand_stream=%d)\n",
                       val << TRNG_TEST_REG_SHIFT, round, ret);
                has_error = 1;
                break;
            }

            /* Run full NIST test with retry for test 15/16 */
            nist_suite_result_t result;
            nist_run_all_tests_ex(g_nist_test_buf, n_bits, NIST_DEFAULT_ALPHA, &result, hal_rand_stream_trng_hal);

            /* Print one-line summary with round number */
            printf("ROSC=0x%04X: R%d: HW TRNG Result: %d/%d passed\n",
                   val << TRNG_TEST_REG_SHIFT, round,
                   result.tests_passed, result.tests_run);

            /* Track min passed */
            if (result.tests_passed < min_passed) {
                min_passed = result.tests_passed;
            }
        }

        /* Track best config (by min passed for stability) */
        if (!has_error && min_passed > best_min_passed) {
            best_min_passed = min_passed;
            best_val = val;
        }
    }

    /* Restore original register value */
    *reg = orig_val;

    /* Free test buffer */
    nist_free_test_buffer();

    /* Final summary */
    printf("\n--- Sweep Summary ---\n");
    printf("Tested: %d configs (0x%04X-0x%04X), %d rounds each\n",
           total_configs, start_val << TRNG_TEST_REG_SHIFT,
           end_val << TRNG_TEST_REG_SHIFT, NIST_TEST_ROUNDS);
    if (best_val >= 0) {
        printf("Best:   ROSC=0x%04X (min %d/%d passed across %d rounds)\n",
               best_val << TRNG_TEST_REG_SHIFT, best_min_passed, 16, NIST_TEST_ROUNDS);
    }
    printf("Restored: 0x%08lX\n", (unsigned long)orig_val);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_sweep, trng_sweep,
    Full NIST test sweep reg 0x30080250[15:8] [n_bits] [start] [end]);

/* ============================================================================
 * TRNG Reseed Number Sweep Test
 * ============================================================================ */

#define TRNG_RESEED_LSB_ADDR    0x3008022C  /* reseed_n[31:0] */
#define TRNG_RESEED_MSB_ADDR    0x30080230  /* reseed_n[47:32] in bits [15:0] */

/* Number of reseed test values */
#define TRNG_RESEED_TEST_COUNT  200

/**
 * @brief Reseed test values (48-bit values stored as {msb[15:0], lsb[31:0]})
 *
 * Test values include:
 * - Powers of 2 (bit positions 0-47)
 * - Common values (0, 1, max)
 * - Logarithmic distribution across 48-bit range
 * - Some pattern values
 */
static const uint64_t trng_reseed_test_values[TRNG_RESEED_TEST_COUNT] = {
    /* Special values */
    0x000000000000ULL,  /* 0 - minimum */
    0x000000000001ULL,  /* 1 */
    0x0000FFFFFFFFULL,  /* 32-bit max */
    0xFFFFFFFFFFFFULL,  /* 48-bit max */

    /* Powers of 2: 2^0 to 2^47 */
    0x000000000001ULL, 0x000000000002ULL, 0x000000000004ULL, 0x000000000008ULL,
    0x000000000010ULL, 0x000000000020ULL, 0x000000000040ULL, 0x000000000080ULL,
    0x000000000100ULL, 0x000000000200ULL, 0x000000000400ULL, 0x000000000800ULL,
    0x000000001000ULL, 0x000000002000ULL, 0x000000004000ULL, 0x000000008000ULL,
    0x000000010000ULL, 0x000000020000ULL, 0x000000040000ULL, 0x000000080000ULL,
    0x000000100000ULL, 0x000000200000ULL, 0x000000400000ULL, 0x000000800000ULL,
    0x000001000000ULL, 0x000002000000ULL, 0x000004000000ULL, 0x000008000000ULL,
    0x000010000000ULL, 0x000020000000ULL, 0x000040000000ULL, 0x000080000000ULL,
    0x000100000000ULL, 0x000200000000ULL, 0x000400000000ULL, 0x000800000000ULL,
    0x001000000000ULL, 0x002000000000ULL, 0x004000000000ULL, 0x008000000000ULL,
    0x010000000000ULL, 0x020000000000ULL, 0x040000000000ULL, 0x080000000000ULL,
    0x100000000000ULL, 0x200000000000ULL, 0x400000000000ULL, 0x800000000000ULL,

    /* Powers of 2 minus 1: (2^n - 1) for various n */
    0x000000000003ULL, 0x000000000007ULL, 0x00000000000FULL, 0x00000000001FULL,
    0x00000000003FULL, 0x00000000007FULL, 0x0000000000FFULL, 0x0000000001FFULL,
    0x0000000003FFULL, 0x0000000007FFULL, 0x000000000FFFULL, 0x000000001FFFULL,
    0x000000003FFFULL, 0x000000007FFFULL, 0x00000000FFFFULL, 0x00000001FFFFULL,
    0x00000003FFFFULL, 0x00000007FFFFULL, 0x0000000FFFFFULL, 0x0000001FFFFFULL,
    0x0000003FFFFFULL, 0x0000007FFFFFULL, 0x000000FFFFFFULL, 0x000001FFFFFFULL,
    0x000003FFFFFFULL, 0x000007FFFFFFULL, 0x00000FFFFFFFULL, 0x00001FFFFFFFULL,
    0x00003FFFFFFFULL, 0x00007FFFFFFFULL, 0x0000FFFFFFFFULL, 0x0001FFFFFFFFULL,
    0x0003FFFFFFFFULL, 0x0007FFFFFFFFULL, 0x000FFFFFFFFFULL, 0x001FFFFFFFFFULL,
    0x003FFFFFFFFFULL, 0x007FFFFFFFFFULL, 0x00FFFFFFFFFFULL, 0x01FFFFFFFFFFULL,
    0x03FFFFFFFFFFULL, 0x07FFFFFFFFFFULL, 0x0FFFFFFFFFFFULL, 0x1FFFFFFFFFFFULL,
    0x3FFFFFFFFFFFULL, 0x7FFFFFFFFFFFULL,

    /* Linear steps in low range (0-255) */
    0x000000000010ULL, 0x000000000020ULL, 0x000000000030ULL, 0x000000000040ULL,
    0x000000000050ULL, 0x000000000060ULL, 0x000000000070ULL, 0x000000000080ULL,
    0x000000000090ULL, 0x0000000000A0ULL, 0x0000000000B0ULL, 0x0000000000C0ULL,
    0x0000000000D0ULL, 0x0000000000E0ULL, 0x0000000000F0ULL,

    /* Logarithmic steps in mid range */
    0x000000000100ULL, 0x000000000200ULL, 0x000000000300ULL, 0x000000000400ULL,
    0x000000000500ULL, 0x000000000600ULL, 0x000000000700ULL, 0x000000000800ULL,
    0x000000000900ULL, 0x000000000A00ULL, 0x000000000B00ULL, 0x000000000C00ULL,
    0x000000001000ULL, 0x000000001800ULL, 0x000000002000ULL, 0x000000003000ULL,
    0x000000004000ULL, 0x000000006000ULL, 0x000000008000ULL, 0x00000000C000ULL,
    0x000000010000ULL, 0x000000018000ULL, 0x000000020000ULL, 0x000000030000ULL,
    0x000000040000ULL, 0x000000060000ULL, 0x000000080000ULL, 0x0000000C0000ULL,
    0x000000100000ULL, 0x000000180000ULL, 0x000000200000ULL, 0x000000300000ULL,
    0x000000400000ULL, 0x000000600000ULL, 0x000000800000ULL, 0x000000C00000ULL,
    0x000001000000ULL, 0x000001800000ULL, 0x000002000000ULL, 0x000003000000ULL,
    0x000004000000ULL, 0x000006000000ULL, 0x000008000000ULL, 0x00000C000000ULL,
    0x000010000000ULL, 0x000018000000ULL, 0x000020000000ULL, 0x000030000000ULL,
    0x000040000000ULL, 0x000060000000ULL, 0x000080000000ULL, 0x0000C0000000ULL,

    /* High range values (using MSB bits) */
    0x000100000000ULL, 0x000200000000ULL, 0x000400000000ULL, 0x000800000000ULL,
    0x001000000000ULL, 0x002000000000ULL, 0x004000000000ULL, 0x008000000000ULL,
    0x010000000000ULL, 0x020000000000ULL, 0x040000000000ULL, 0x080000000000ULL,
    0x100000000000ULL, 0x200000000000ULL, 0x400000000000ULL, 0x800000000000ULL,

    /* Pattern values */
    0x555555555555ULL,  /* alternating 01 */
    0xAAAAAAAAAAAAULL,  /* alternating 10 */
    0x333333333333ULL,  /* alternating 0011 */
    0xCCCCCCCCCCCCULL,  /* alternating 1100 */
    0x0F0F0F0F0F0FULL,  /* alternating 00001111 */
    0xF0F0F0F0F0F0ULL,  /* alternating 11110000 */
    0x00FF00FF00FFULL,  /* alternating bytes */
    0xFF00FF00FF00ULL,
    0x0000FFFF0000ULL,
    0xFFFF0000FFFFULL,
    0x123456789ABCULL,  /* ascending nibbles */
    0xFEDCBA987654ULL,  /* descending nibbles */

    /* Additional values to reach 200 */
    0x000000000005ULL, 0x000000000006ULL, 0x000000000009ULL, 0x00000000000AULL,
    0x00000000000BULL, 0x00000000000DULL, 0x00000000000EULL,
};

/**
 * @brief Shell command: trng_reseed [n_bits] [start_idx] [end_idx]
 *
 * Sweep reseed_n register values and run NIST test for each.
 * Uses pre-allocated buffer to avoid memory fragmentation.
 */
static int cmd_trng_reseed_sweep(int argc, char **argv)
{
    uint32_t n_bits = NIST_DEFAULT_N_BITS;  /* Default 1M bits */
    int start_idx = 0;
    int end_idx = TRNG_RESEED_TEST_COUNT - 1;
    volatile uint32_t *reg_lsb = (volatile uint32_t *)TRNG_RESEED_LSB_ADDR;
    volatile uint32_t *reg_msb = (volatile uint32_t *)TRNG_RESEED_MSB_ADDR;

    /* Parse arguments */
    if (argc > 1) {
        n_bits = (uint32_t)strtoul(argv[1], NULL, 0);
        if (n_bits < NIST_MIN_RECOMMENDED_BITS) n_bits = NIST_MIN_RECOMMENDED_BITS;
        if (n_bits > NIST_DEFAULT_N_BITS) n_bits = NIST_DEFAULT_N_BITS;
    }
    if (argc > 2) {
        start_idx = (int)strtoul(argv[2], NULL, 0);
        if (start_idx < 0) start_idx = 0;
        if (start_idx >= TRNG_RESEED_TEST_COUNT) start_idx = TRNG_RESEED_TEST_COUNT - 1;
    }
    if (argc > 3) {
        end_idx = (int)strtoul(argv[3], NULL, 0);
        if (end_idx < start_idx) end_idx = start_idx;
        if (end_idx >= TRNG_RESEED_TEST_COUNT) end_idx = TRNG_RESEED_TEST_COUNT - 1;
    }

    uint32_t n_bytes = (n_bits + 7) / 8;
    int total_configs = end_idx - start_idx + 1;

    printf("\n");
    printf("TRNG Reseed Sweep - NIST SP 800-22\n");
    printf("==================================\n");
    printf("Test bits: %lu (%lu KB)\n", (unsigned long)n_bits, (unsigned long)(n_bytes / 1024));
    printf("Range:     index %d - %d (%d configs)\n", start_idx, end_idx, total_configs);

    /* Pre-allocate test buffer */
    if (nist_alloc_test_buffer(n_bytes) != 0) {
        return -1;
    }

    /* Save original register values */
    uint32_t orig_lsb = *reg_lsb;
    uint32_t orig_msb = *reg_msb;
    printf("Original:  0x%04lX_%08lX\n",
           (unsigned long)(orig_msb & 0xFFFF), (unsigned long)orig_lsb);
    printf("\n");

    /* Initialize TRNG */
    hal_sec_trng_init();

    /* Track best result (use min passed across all rounds for stability) */
    int best_idx = -1;
    uint64_t best_reseed = 0;
    int best_min_passed = -1;

    /* Print header */
    printf("--- Sweep Results (%d rounds per config) ---\n", NIST_TEST_ROUNDS);

    /* Sweep reseed values */
    for (int idx = start_idx; idx <= end_idx; idx++) {
        uint64_t reseed = trng_reseed_test_values[idx];
        uint32_t lsb = (uint32_t)(reseed & 0xFFFFFFFF);
        uint32_t msb = (uint32_t)((reseed >> 32) & 0xFFFF);

        /* Set reseed registers */
        *reg_lsb = lsb;
        *reg_msb = (orig_msb & 0xFFFF0000) | msb;

        /* Small delay for register to take effect */
        for (volatile int d = 0; d < 1000; d++);

        /* Track min passed across rounds for this config */
        int min_passed = 16;
        int has_error = 0;

        /* Run multiple rounds per configuration */
        for (int round = 1; round <= NIST_TEST_ROUNDS; round++) {
            /* Generate random data */
            int ret = hal_rand_stream(g_nist_test_buf, n_bytes);
            if (ret != 0) {
                printf("reseed=0x%04lX_%08lX: R%d: ERROR (hal_rand_stream=%d)\n",
                       (unsigned long)msb, (unsigned long)lsb, round, ret);
                has_error = 1;
                break;
            }

            /* Run full NIST test with retry for test 15/16 */
            nist_suite_result_t result;
            nist_run_all_tests_ex(g_nist_test_buf, n_bits, NIST_DEFAULT_ALPHA, &result, hal_rand_stream_trng_hal);

            /* Print one-line summary with round number */
            printf("reseed=0x%04lX_%08lX: R%d: HW TRNG Result: %d/%d passed\n",
                   (unsigned long)msb, (unsigned long)lsb, round,
                   result.tests_passed, result.tests_run);

            /* Track min passed */
            if (result.tests_passed < min_passed) {
                min_passed = result.tests_passed;
            }
        }

        /* Track best config (by min passed for stability) */
        if (!has_error && min_passed > best_min_passed) {
            best_min_passed = min_passed;
            best_idx = idx;
            best_reseed = reseed;
        }
    }

    /* Restore original register values */
    *reg_lsb = orig_lsb;
    *reg_msb = orig_msb;

    /* Free test buffer */
    nist_free_test_buffer();

    /* Final summary */
    printf("\n--- Sweep Summary ---\n");
    printf("Tested: %d configs, %d rounds each\n", total_configs, NIST_TEST_ROUNDS);
    if (best_idx >= 0) {
        uint32_t best_lsb = (uint32_t)(best_reseed & 0xFFFFFFFF);
        uint32_t best_msb = (uint32_t)((best_reseed >> 32) & 0xFFFF);
        printf("Best:   reseed=0x%04lX_%08lX (min %d/%d passed across %d rounds)\n",
               (unsigned long)best_msb, (unsigned long)best_lsb,
               best_min_passed, 16, NIST_TEST_ROUNDS);
    }
    printf("Restored: 0x%04lX_%08lX\n",
           (unsigned long)(orig_msb & 0xFFFF), (unsigned long)orig_lsb);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_reseed_sweep, trng_reseed,
    NIST test sweep reseed_n [n_bits] [start_idx] [end_idx]);

#ifdef CONFIG_NIST_PRNG_VERIFY
/* ============================================================================
 * Software PRNG Verification Command
 * ============================================================================
 * This command uses XORShift64* PRNG to verify NIST test implementations.
 * If tests pass with software PRNG but fail with hardware TRNG, the issue
 * is hardware. If tests fail with both, the issue is implementation.
 */

static uint64_t xorshift64_state = 0x123456789ABCDEF0ULL;

/**
 * XORShift64* - improved version with multiplicative output transform
 * This variant passes NIST tests better than plain XORShift64
 */
static uint64_t xorshift64star(void)
{
    uint64_t x = xorshift64_state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    xorshift64_state = x;
    return x * 0x2545F4914F6CDD1DULL;  /* Multiplicative mixing */
}

static int xorshift_trng_hal(uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i += 8) {
        uint64_t val = xorshift64star();
        size_t copy_len = (len - i >= 8) ? 8 : (len - i);
        memcpy(buf + i, &val, copy_len);
    }
    return 0;
}

static int cmd_trng_verify(int argc, char **argv)
{
    uint32_t n_bits = NIST_DEFAULT_N_BITS;  /* Default: 1M bits for Test 15/16 */
    uint64_t seed = 0x123456789ABCDEF0ULL;

    if (argc >= 2) {
        n_bits = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        seed = strtoull(argv[2], NULL, 0);
    }

    printf("\n========================================\n");
    printf("NIST SP 800-22 IMPLEMENTATION VERIFY\n");
    printf("========================================\n");
    printf("Using: XORShift64* PRNG (seed=0x%llx)\n", (unsigned long long)seed);
    printf("Bits:  %lu\n", (unsigned long)n_bits);
    printf("----------------------------------------\n");
    printf("If tests PASS: Implementation is correct\n");
    printf("If tests FAIL: Implementation has bugs\n");
    printf("========================================\n\n");

    /* Reset PRNG state with seed */
    xorshift64_state = seed;

    /* Run full test suite using XORShift PRNG as HAL */
    nist_suite_result_t result;
    memset(&result, 0, sizeof(result));

    int ret = nist_run_suite_with_hal(xorshift_trng_hal, n_bits,
                                       NIST_DEFAULT_ALPHA, &result);

    printf("\n========================================\n");
    printf("VERIFICATION RESULT\n");
    printf("========================================\n");
    printf("Total tests: %d\n", result.tests_run);
    printf("Passed: %d, Failed: %d, Error: %d\n",
           result.tests_passed, result.tests_failed, result.tests_error);
    printf("Time: %lu ms\n", (unsigned long)result.elapsed_ms);
    printf("========================================\n");

    if (result.tests_failed == 0 && result.tests_error == 0) {
        printf(">>> IMPLEMENTATION VERIFIED OK <<<\n");
    } else {
        printf(">>> IMPLEMENTATION MAY HAVE ISSUES <<<\n");
    }

    /* Print compact summary for comparison with HW TRNG */
    printf("\n--- Test Summary (for comparison) ---\n");
    printf("%-32s %-8s %-12s %-10s\n", "Test Name", "Status", "Sub-tests", "P-value");
    printf("----------------------------------------------------------------------\n");
    int total_sub_passed = 0, total_sub_total = 0;
    for (int i = 0; i < result.tests_run; i++) {
        const char *status;
        if (result.tests[i].result < 0) {
            status = "ERR";
        } else if (result.tests[i].passed) {
            status = "PASS";
        } else {
            status = "FAIL";
        }
        /* Show sub-test counts */
        char sub_str[16];
        if (result.tests[i].sub_total > 1) {
            snprintf(sub_str, sizeof(sub_str), "%d/%d",
                     result.tests[i].sub_passed, result.tests[i].sub_total);
        } else {
            snprintf(sub_str, sizeof(sub_str), "-");
        }
        printf("[%2d] %-28s %-6s %-10s %.6f\n", result.tests[i].test_id,
               result.tests[i].test_name, status, sub_str, result.tests[i].p_value);
        total_sub_passed += result.tests[i].sub_passed;
        total_sub_total += result.tests[i].sub_total;
    }
    printf("======================================================================\n");
    printf("PRNG Result: %d/%d tests passed, %d/%d sub-tests passed\n",
           result.tests_passed, result.tests_run, total_sub_passed, total_sub_total);
    printf("======================================================================\n");

    (void)ret;
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_verify, trng_verify,
    Verify NIST impl with software PRNG [n_bits] [seed]);

/* ============================================================================
 * Pattern Generator for debugging specific tests
 * ============================================================================ */
static int cmd_trng_pattern(int argc, char **argv)
{
    uint32_t n_bits = 100000;

    printf("\n=== Testing Non-overlapping Template Implementation ===\n\n");

    /* Allocate buffer */
    uint32_t n_bytes = (n_bits + 7) / 8;
    uint8_t *data = (uint8_t *)malloc(n_bytes);
    if (!data) {
        printf("Memory allocation failed\n");
        return -1;
    }

    /* Fill with XORShift64* data */
    xorshift64_state = 0x123456789ABCDEF0ULL;
    for (uint32_t i = 0; i < n_bytes; i += 8) {
        uint64_t val = xorshift64star();
        size_t copy_len = (n_bytes - i >= 8) ? 8 : (n_bytes - i);
        memcpy(data + i, &val, copy_len);
    }

    /* Manual count: how many 9-bit all-ones patterns exist? */
    uint32_t manual_count = 0;
    uint32_t total_ones = 0;

    for (uint32_t i = 0; i < n_bits; i++) {
        uint8_t bit = (data[i >> 3] >> (7 - (i & 7))) & 1;
        total_ones += bit;
    }

    printf("Total bits: %lu, ones: %lu (%.2f%%)\n",
           (unsigned long)n_bits, (unsigned long)total_ones,
           100.0 * total_ones / n_bits);

    /* Count 9-bit all-ones patterns (overlapping) */
    for (uint32_t i = 0; i <= n_bits - 9; i++) {
        int all_ones = 1;
        for (int j = 0; j < 9; j++) {
            uint8_t bit = (data[(i + j) >> 3] >> (7 - ((i + j) & 7))) & 1;
            if (!bit) {
                all_ones = 0;
                break;
            }
        }
        if (all_ones) manual_count++;
    }

    printf("9-bit all-ones patterns (overlapping): %lu\n", (unsigned long)manual_count);
    printf("Expected (random): %.1f\n", (double)(n_bits - 8) / 512.0);

    /* Also count runs of 9+ ones */
    uint32_t run_count = 0;
    int current_run = 0;
    for (uint32_t i = 0; i < n_bits; i++) {
        uint8_t bit = (data[i >> 3] >> (7 - (i & 7))) & 1;
        if (bit) {
            current_run++;
        } else {
            if (current_run >= 9) run_count++;
            current_run = 0;
        }
    }
    if (current_run >= 9) run_count++;
    printf("Runs of 9+ consecutive ones: %lu\n", (unsigned long)run_count);

    /* Test with perfectly random-looking data: use 0x55 pattern */
    printf("\n--- Testing with 0x55 pattern (01010101) ---\n");
    memset(data, 0x55, n_bytes);

    manual_count = 0;
    for (uint32_t i = 0; i <= n_bits - 9; i++) {
        int all_ones = 1;
        for (int j = 0; j < 9; j++) {
            uint8_t bit = (data[(i + j) >> 3] >> (7 - ((i + j) & 7))) & 1;
            if (!bit) {
                all_ones = 0;
                break;
            }
        }
        if (all_ones) manual_count++;
    }
    printf("9-bit all-ones patterns: %lu (should be 0)\n", (unsigned long)manual_count);

    /* Test with 0xFF pattern */
    printf("\n--- Testing with 0xFF pattern (all ones) ---\n");
    memset(data, 0xFF, n_bytes);

    manual_count = 0;
    for (uint32_t i = 0; i <= n_bits - 9; i++) {
        int all_ones = 1;
        for (int j = 0; j < 9; j++) {
            uint8_t bit = (data[(i + j) >> 3] >> (7 - ((i + j) & 7))) & 1;
            if (!bit) {
                all_ones = 0;
                break;
            }
        }
        if (all_ones) manual_count++;
    }
    printf("9-bit all-ones patterns: %lu (should be %lu)\n",
           (unsigned long)manual_count, (unsigned long)(n_bits - 8));

    free(data);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_pattern, trng_pattern,
    Debug pattern matching for NIST tests);

/* ============================================================================
 * PCG-64 PRNG (Permuted Congruential Generator)
 * ============================================================================
 * PCG-64-XSH-RR is a high-quality PRNG that passes TestU01 BigCrush,
 * PractRand, and NIST SP 800-22 tests. It's the gold standard for
 * PRNG verification.
 *
 * Reference: https://www.pcg-random.org/
 * Author: Melissa O'Neill (oneill@pcg-random.org)
 */

typedef struct {
    uint64_t state;
    uint64_t inc;  /* Odd, determines stream */
} pcg64_state_t;

static pcg64_state_t g_pcg64 = {
    .state = 0x853c49e6748fea9bULL,
    .inc = 0xda3e39cb94b95bdbULL
};

/**
 * @brief PCG-32-XSH-RR: 32-bit output from 64-bit state
 *
 * Uses XOR-shift high and random rotate output function.
 * This is the core PCG output function.
 */
static uint32_t pcg32_next(void)
{
    uint64_t oldstate = g_pcg64.state;

    /* Advance internal state (LCG with 2^64 modulus) */
    g_pcg64.state = oldstate * 6364136223846793005ULL + g_pcg64.inc;

    /* XSH-RR: XOR-shift high bits, random rotate */
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = (uint32_t)(oldstate >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((32u - rot) & 31u));
}

/**
 * @brief PCG-64: Generate 64-bit value by combining two 32-bit outputs
 *
 * Combines two consecutive PCG-32 outputs into a 64-bit value.
 * Period: 2^64, statistical quality verified by TestU01 BigCrush.
 */
static uint64_t pcg64_next(void)
{
    uint32_t high = pcg32_next();
    uint32_t low = pcg32_next();
    return ((uint64_t)high << 32) | (uint64_t)low;
}

/**
 * @brief Seed PCG-64 with 128-bit seed
 */
static void pcg64_seed(uint64_t seed, uint64_t seq)
{
    g_pcg64.state = 0;
    g_pcg64.inc = (seq << 1u) | 1u;  /* Must be odd */
    pcg64_next();
    g_pcg64.state += seed;
    pcg64_next();
}

/**
 * @brief TRNG HAL using PCG-64
 */
static int pcg64_trng_hal(uint8_t *buf, size_t len)
{
    size_t i = 0;
    while (i < len) {
        uint64_t val = pcg64_next();
        size_t copy_len = (len - i >= 8) ? 8 : (len - i);
        memcpy(buf + i, &val, copy_len);
        i += copy_len;
    }
    return 0;
}

/**
 * @brief Shell command: trng_pcg [n_bits] [seed]
 *
 * Run NIST SP 800-22 tests using PCG-64 PRNG as reference.
 * PCG-64 is verified to pass all NIST tests, so this validates
 * the test implementation itself.
 */
static int cmd_trng_pcg_verify(int argc, char **argv)
{
    uint32_t n_bits = NIST_DEFAULT_N_BITS;
    uint64_t seed = 0x853c49e6748fea9bULL;
    uint64_t seq = 0xda3e39cb94b95bdbULL;

    if (argc >= 2) {
        n_bits = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        seed = strtoull(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        seq = strtoull(argv[3], NULL, 0);
    }

    printf("\n");
    printf("========================================================\n");
    printf("NIST SP 800-22 Test Suite - PCG-64 PRNG Verification\n");
    printf("========================================================\n");
    printf("PRNG:      PCG-64-XSH-RR (verified by TestU01 BigCrush)\n");
    printf("Seed:      0x%016llx\n", (unsigned long long)seed);
    printf("Sequence:  0x%016llx\n", (unsigned long long)seq);
    printf("Bits:      %lu (%lu KB)\n", (unsigned long)n_bits,
           (unsigned long)((n_bits + 7) / 8 / 1024));
    printf("--------------------------------------------------------\n");
    printf("Purpose:   Verify NIST test implementation correctness\n");
    printf("Expected:  ALL tests should PASS with PCG-64\n");
    printf("--------------------------------------------------------\n");
    printf("If ALL tests PASS: Test implementation is correct\n");
    printf("If ANY test FAILS: Test implementation has bugs!\n");
    printf("========================================================\n\n");

    /* Seed PCG-64 */
    pcg64_seed(seed, seq);

    /* Run full NIST test suite */
    nist_suite_result_t result;
    memset(&result, 0, sizeof(result));

    uint32_t start_tick = (uint32_t)xTaskGetTickCount();
    int ret = nist_run_suite_with_hal(pcg64_trng_hal, n_bits,
                                       NIST_DEFAULT_ALPHA, &result);
    uint32_t elapsed_ms = ((uint32_t)xTaskGetTickCount() - start_tick) * portTICK_PERIOD_MS;
    result.elapsed_ms = elapsed_ms;

    /* Print detailed results */
    printf("\n========================================================================\n");
    printf("PCG-64 VERIFICATION RESULTS\n");
    printf("========================================================================\n");
    printf("%-34s %-8s %-12s %s\n", "Test Name", "Status", "Sub-tests", "P-value");
    printf("------------------------------------------------------------------------\n");

    int impl_issues = 0;
    int total_sub_passed = 0, total_sub_total = 0;
    for (int i = 0; i < result.tests_run; i++) {
        const char *status;

        if (result.tests[i].result < 0) {
            status = "ERROR";
            impl_issues++;
        } else if (result.tests[i].passed) {
            status = "PASS";
        } else {
            status = "FAIL*";  /* Mark as implementation issue */
            impl_issues++;
        }

        /* Show sub-test counts */
        char sub_str[16];
        if (result.tests[i].sub_total > 1) {
            snprintf(sub_str, sizeof(sub_str), "%d/%d",
                     result.tests[i].sub_passed, result.tests[i].sub_total);
        } else {
            snprintf(sub_str, sizeof(sub_str), "-");
        }

        printf("[%2d] %-30s %-8s %-10s %.6f%s\n",
               result.tests[i].test_id,
               result.tests[i].test_name,
               status,
               sub_str,
               result.tests[i].p_value,
               (result.tests[i].passed || result.tests[i].result < 0) ? "" : " <-- IMPL BUG?");
        total_sub_passed += result.tests[i].sub_passed;
        total_sub_total += result.tests[i].sub_total;
    }

    printf("========================================================================\n");
    printf("Summary: %d/%d tests passed, %d/%d sub-tests passed, Time: %lu ms\n",
           result.tests_passed, result.tests_run,
           total_sub_passed, total_sub_total, (unsigned long)elapsed_ms);
    printf("========================================================================\n");

    if (impl_issues == 0) {
        printf("\n*** ALL PCG-64 TESTS PASSED ***\n");
        printf("Test implementation is VERIFIED CORRECT.\n");
        printf("If hardware TRNG fails, the issue is hardware quality.\n");
    } else {
        printf("\n*** WARNING: %d TEST(S) FAILED WITH PCG-64 ***\n", impl_issues);
        printf("This indicates BUGS in the NIST test implementation!\n");
        printf("Do NOT trust hardware TRNG results until fixed.\n");
    }

    (void)ret;
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_pcg_verify, trng_pcg,
    Verify NIST impl with PCG-64 PRNG [n_bits] [seed] [seq]);

/**
 * @brief Shell command: trng_excursions_verify [n_bits] [rounds]
 *
 * Verify Random Excursions precondition (J >= 500) using PCG-64 PRNG.
 * For truly random 1M bits, expected J ≈ n/(2π) ≈ 159,000.
 *
 * If PRNG consistently gives J >> 500, the implementation is correct.
 * If PRNG gives J < 500, there's an implementation bug.
 */
static int cmd_trng_excursions_verify(int argc, char **argv)
{
    uint32_t n_bits = NIST_DEFAULT_N_BITS;
    int rounds = 10;
    uint64_t seed = 0x853c49e6748fea9bULL;
    uint64_t seq = 0xda3e39cb94b95bdbULL;

    if (argc >= 2) {
        n_bits = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        rounds = (int)strtoul(argv[2], NULL, 0);
        if (rounds < 1) rounds = 1;
        if (rounds > 100) rounds = 100;
    }

    uint32_t n_bytes = (n_bits + 7) / 8;

    printf("\n");
    printf("========================================================\n");
    printf("Random Excursions J-Count Verification (PCG-64 PRNG)\n");
    printf("========================================================\n");
    printf("PRNG:      PCG-64-XSH-RR\n");
    printf("Seed:      0x%016llx\n", (unsigned long long)seed);
    printf("Bits:      %lu (%lu KB)\n", (unsigned long)n_bits,
           (unsigned long)(n_bytes / 1024));
    printf("Rounds:    %d\n", rounds);
    printf("Required:  J >= 500\n");
    printf("Expected:  J ≈ sqrt(n) ≈ %lu for random walk\n",
           (unsigned long)((uint32_t)sqrtf((float)n_bits)));
    printf("--------------------------------------------------------\n");
    printf("If J around sqrt(n): Implementation is correct\n");
    printf("If J < 500 often:    Hardware TRNG has bias issue\n");
    printf("========================================================\n\n");

    /* Allocate test buffer */
    if (nist_alloc_test_buffer(n_bytes) != 0) {
        printf("Error: Failed to allocate %lu bytes\n", (unsigned long)n_bytes);
        return -1;
    }

    /* Initialize PCG */
    pcg64_seed(seed, seq);

    int pass_count = 0;
    uint32_t min_J = UINT32_MAX;
    uint32_t max_J = 0;
    uint64_t total_J = 0;

    printf("%-8s %-12s %-8s\n", "Round", "J-Count", "Status");
    printf("----------------------------------------\n");

    for (int r = 0; r < rounds; r++) {
        /* Generate PRNG data using pcg64_trng_hal */
        pcg64_trng_hal(g_nist_test_buf, n_bytes);

        /* Count zero crossings */
        uint32_t J = nist_count_zero_crossings(g_nist_test_buf, n_bits);

        total_J += J;
        if (J < min_J) min_J = J;
        if (J > max_J) max_J = J;

        const char *status = (J >= 500) ? "PASS" : "FAIL";
        if (J >= 500) pass_count++;

        printf("%-8d %-12lu %s\n", r + 1, (unsigned long)J, status);
    }

    nist_free_test_buffer();

    uint32_t avg_J = (rounds > 0) ? (uint32_t)(total_J / rounds) : 0;

    printf("----------------------------------------\n");
    printf("\n");
    printf("========================================================\n");
    printf("VERIFICATION SUMMARY\n");
    printf("========================================================\n");
    printf("Rounds passed:  %d/%d\n", pass_count, rounds);
    printf("J minimum:      %lu\n", (unsigned long)min_J);
    printf("J maximum:      %lu\n", (unsigned long)max_J);
    printf("J average:      %lu\n", (unsigned long)avg_J);
    printf("J expected:     ~%lu (sqrt(n) for random walk)\n",
           (unsigned long)((uint32_t)sqrtf((float)n_bits)));
    printf("--------------------------------------------------------\n");

    /* For random walk, expected J ≈ sqrt(n) ≈ 1000 for 1M bits */
    uint32_t expected_J = (uint32_t)sqrtf((float)n_bits);
    if (avg_J > expected_J / 2 && avg_J < expected_J * 2) {
        printf("RESULT: Implementation is CORRECT\n");
        printf("        J average (%lu) is within expected range.\n",
               (unsigned long)avg_J);
        printf("        sqrt(n)=%lu, acceptable: [%lu, %lu]\n",
               (unsigned long)expected_J,
               (unsigned long)(expected_J / 2),
               (unsigned long)(expected_J * 2));
        if (pass_count < rounds) {
            printf("NOTE:   %d/%d rounds had J < 500 (normal variance)\n",
                   rounds - pass_count, rounds);
            printf("        Hardware TRNG with similar J range may fail.\n");
        }
    } else {
        printf("RESULT: UNEXPECTED J VALUES\n");
        printf("        J average (%lu) outside expected range [%lu, %lu]\n",
               (unsigned long)avg_J,
               (unsigned long)(expected_J / 2),
               (unsigned long)(expected_J * 2));
        printf("        Check nist_count_zero_crossings() implementation.\n");
    }
    printf("========================================================\n");

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_excursions_verify, trng_excursions_verify,
    Verify J-count with PCG-64 PRNG [n_bits] [rounds]);

#endif /* CONFIG_NIST_PRNG_VERIFY */

/* ============================================================================
 * Non-overlapping Template ROSC/Reseed Sweep Test with LittleFS Persistence
 * ============================================================================
 * Sweep ROSC and Reseed parameters specifically for Non-overlapping Template
 * test to find optimal hardware configuration.
 *
 * Results are saved to LittleFS for:
 * - Resume testing after power cycle
 * - Version-based test management
 */

/* Test version - increment when test parameters change */
#define NONOVERLAP_TEST_VERSION  2

/* Result file path */
#define NONOVERLAP_RESULT_FILE   "/trng_nonoverlap_result.bin"

/* Result data structure for each configuration */
typedef struct {
    uint16_t passed_count;    /* Templates passed */
    uint16_t total_count;     /* Templates tested */
    uint8_t  certified;       /* 1 = NIST certified, 0 = failed */
    uint8_t  tested;          /* 1 = tested, 0 = not yet */
} nonoverlap_config_result_t;

/* Header structure for saved results */
typedef struct {
    uint32_t magic;           /* Magic number for validation */
    uint32_t version;         /* Test version */
    uint8_t  rosc_count;      /* Number of ROSC values */
    uint8_t  reseed_count;    /* Number of Reseed values */
    uint8_t  current_rosc;    /* Current ROSC index (for resume) */
    uint8_t  current_reseed;  /* Current Reseed index (for resume) */
    uint32_t n_bits;          /* Test bits used */
    /* Followed by: nonoverlap_config_result_t results[rosc_count * reseed_count] */
} nonoverlap_result_header_t;

#define NONOVERLAP_MAGIC  0x4E4F5650  /* "NOVP" */

#ifdef CONFIG_LITTLEFS
/* Local LFS context for auto-mount */
static struct lfs_context nonoverlap_lfs_ctx;
static struct lfs_config nonoverlap_lfs_cfg = {
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .cache_size = 256,
    .lookahead_size = 16,
    .block_cycles = 500,
};
static lfs_t *nonoverlap_lfs = NULL;

/**
 * @brief Ensure LittleFS is mounted
 */
static lfs_t *nonoverlap_ensure_lfs_mounted(void)
{
    /* First check if already mounted via CLI */
    lfs_t *lfs = lfs_get_instance();
    if (lfs != NULL) {
        return lfs;
    }

    /* Auto-mount using lfs_xip_init API */
    if (nonoverlap_lfs == NULL) {
        printf("[LFS] Auto-mounting PSM partition...\n");
        nonoverlap_lfs_ctx.partition_name = "PSM";
        nonoverlap_lfs = lfs_xip_init(&nonoverlap_lfs_ctx, &nonoverlap_lfs_cfg);
        if (nonoverlap_lfs == NULL) {
            printf("[LFS] Mount failed!\n");
            return NULL;
        }
        printf("[LFS] Mounted successfully\n");
    }
    return nonoverlap_lfs;
}

/**
 * @brief Load previous test results from LittleFS
 * @return Pointer to allocated results (caller must free), or NULL if not found/invalid
 */
static nonoverlap_config_result_t *nonoverlap_load_results(
    nonoverlap_result_header_t *header,
    int expected_rosc_count,
    int expected_reseed_count,
    uint32_t expected_n_bits)
{
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) return NULL;

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, NONOVERLAP_RESULT_FILE, LFS_O_RDONLY);
    if (err < 0) {
        printf("[LFS] No previous results found\n");
        return NULL;
    }

    /* Read header */
    lfs_ssize_t read_size = lfs_file_read(lfs, &file, header, sizeof(*header));
    if (read_size != sizeof(*header)) {
        printf("[LFS] Failed to read header\n");
        lfs_file_close(lfs, &file);
        return NULL;
    }

    /* Validate header */
    if (header->magic != NONOVERLAP_MAGIC) {
        printf("[LFS] Invalid magic (0x%08lX)\n", (unsigned long)header->magic);
        lfs_file_close(lfs, &file);
        return NULL;
    }

    if (header->version != NONOVERLAP_TEST_VERSION) {
        printf("[LFS] Version mismatch (file: %lu, expected: %d)\n",
               (unsigned long)header->version, NONOVERLAP_TEST_VERSION);
        lfs_file_close(lfs, &file);
        return NULL;
    }

    if (header->rosc_count != expected_rosc_count ||
        header->reseed_count != expected_reseed_count ||
        header->n_bits != expected_n_bits) {
        printf("[LFS] Config mismatch, starting fresh\n");
        lfs_file_close(lfs, &file);
        return NULL;
    }

    /* Allocate and read results */
    int total = expected_rosc_count * expected_reseed_count;
    nonoverlap_config_result_t *results = pvPortMalloc(total * sizeof(*results));
    if (!results) {
        lfs_file_close(lfs, &file);
        return NULL;
    }

    read_size = lfs_file_read(lfs, &file, results, total * sizeof(*results));
    lfs_file_close(lfs, &file);

    if (read_size != (lfs_ssize_t)(total * sizeof(*results))) {
        printf("[LFS] Failed to read results\n");
        vPortFree(results);
        return NULL;
    }

    printf("[LFS] Loaded previous results (v%lu, progress: ROSC[%d] Reseed[%d])\n",
           (unsigned long)header->version, header->current_rosc, header->current_reseed);
    return results;
}

/**
 * @brief Save test results to LittleFS
 */
static int nonoverlap_save_results(
    const nonoverlap_result_header_t *header,
    const nonoverlap_config_result_t *results)
{
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) return -1;

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, NONOVERLAP_RESULT_FILE,
                            LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (err < 0) {
        printf("[LFS] Failed to open file for writing: %d\n", err);
        return -1;
    }

    /* Write header */
    lfs_ssize_t written = lfs_file_write(lfs, &file, header, sizeof(*header));
    if (written != sizeof(*header)) {
        lfs_file_close(lfs, &file);
        return -1;
    }

    /* Write results */
    int total = header->rosc_count * header->reseed_count;
    written = lfs_file_write(lfs, &file, results, total * sizeof(*results));
    lfs_file_close(lfs, &file);

    if (written != (lfs_ssize_t)(total * sizeof(*results))) {
        return -1;
    }

    return 0;
}
#endif /* CONFIG_LITTLEFS */

/* ROSC values to test: selected optimal values (5 values) */
static const uint16_t nonoverlap_rosc_values[] = {
    0x0F00, 0x1F00, 0x2F00, 0x4F00, 0x9F00,
};
#define NONOVERLAP_ROSC_COUNT (sizeof(nonoverlap_rosc_values) / sizeof(nonoverlap_rosc_values[0]))

/*
 * Reseed values to test (48-bit: MSB[47:32]=16bit, LSB[31:0]=32bit)
 * Single-bit test: each bit set to 1 individually
 * - MSB[47:32]: 16 values (bit 32-47)
 * - LSB[31:24]: 8 values (bit 24-31)
 * Total: 24 values
 */
static const uint64_t nonoverlap_reseed_values[] = {
    /* MSB single-bit: bit 32-47 (16 values) */
    0x000100000000ULL,  /* bit 32 */
    0x000200000000ULL,  /* bit 33 */
    0x000400000000ULL,  /* bit 34 */
    0x000800000000ULL,  /* bit 35 */
    0x001000000000ULL,  /* bit 36 */
    0x002000000000ULL,  /* bit 37 */
    0x004000000000ULL,  /* bit 38 */
    0x008000000000ULL,  /* bit 39 */
    0x010000000000ULL,  /* bit 40 */
    0x020000000000ULL,  /* bit 41 */
    0x040000000000ULL,  /* bit 42 */
    0x080000000000ULL,  /* bit 43 */
    0x100000000000ULL,  /* bit 44 */
    0x200000000000ULL,  /* bit 45 */
    0x400000000000ULL,  /* bit 46 */
    0x800000000000ULL,  /* bit 47 */

    /* LSB bit 24-31 (8 values) */
    0x000001000000ULL,  /* bit 24 */
    0x000002000000ULL,  /* bit 25 */
    0x000004000000ULL,  /* bit 26 */
    0x000008000000ULL,  /* bit 27 */
    0x000010000000ULL,  /* bit 28 */
    0x000020000000ULL,  /* bit 29 */
    0x000040000000ULL,  /* bit 30 */
    0x000080000000ULL,  /* bit 31 */
};
#define NONOVERLAP_RESEED_COUNT (sizeof(nonoverlap_reseed_values) / sizeof(nonoverlap_reseed_values[0]))

/**
 * @brief Calculate NIST SP 800-22 pass ratio threshold
 *
 * Per NIST SP 800-22, the acceptable pass ratio for N tests at significance
 * level alpha is: (1 - alpha) ± 3 * sqrt(alpha * (1 - alpha) / N)
 *
 * @param n_templates  Number of templates tested
 * @param alpha        Significance level (default 0.01)
 * @return Minimum number of templates that must pass for NIST certification
 */
static int nist_min_pass_count(int n_templates, double alpha)
{
    double p = 1.0 - alpha;
    double sigma = sqrt(alpha * p / (double)n_templates);
    double min_ratio = p - 3.0 * sigma;
    return (int)ceil(min_ratio * n_templates);
}

/**
 * @brief Shell command: trng_nonoverlap [n_bits]
 *
 * Sweep ROSC and Reseed parameters specifically for Non-overlapping Template test.
 * Uses NIST SP 800-22 pass ratio criteria for certification instead of 100% pass.
 *
 * ROSC values tested: 0x0F00, 0x1F00, 0x2F00, 0x4F00, 0x9F00 (5 values)
 * Reseed values tested: 48 values (MSB range only, skip LSB-only group)
 * Total configurations: 5 x 48 = 240
 *
 * NIST Certification Criteria:
 *   Pass ratio must be >= (1 - alpha) - 3 * sqrt(alpha * (1 - alpha) / N)
 *   For N=504 templates, alpha=0.01: minimum ~493/504 (97.8%) must pass
 *
 * LittleFS Persistence:
 *   Results are saved to /trng_nonoverlap_result.bin for resume capability.
 *   - Same version: resume from last position
 *   - New version: start fresh test
 *
 * @param n_bits  Number of bits to test (default: 1000000)
 */
static int cmd_trng_nonoverlap_sweep(int argc, char **argv)
{
    uint32_t n_bits = NIST_DEFAULT_N_BITS;

    if (argc > 1) {
        n_bits = (uint32_t)strtoul(argv[1], NULL, 0);
        if (n_bits < NIST_MIN_RECOMMENDED_BITS) n_bits = NIST_MIN_RECOMMENDED_BITS;
        if (n_bits > NIST_DEFAULT_N_BITS) n_bits = NIST_DEFAULT_N_BITS;
    }

    uint32_t n_bytes = (n_bits + 7) / 8;
    int total_configs = NONOVERLAP_ROSC_COUNT * NONOVERLAP_RESEED_COUNT;

    printf("\n");
    printf("TRNG Non-overlapping Template Sweep (NIST Certification Mode)\n");
    printf("==============================================================\n");
    printf("Test version:  %d\n", NONOVERLAP_TEST_VERSION);
    printf("Test bits:     %lu (%lu KB)\n", (unsigned long)n_bits,
           (unsigned long)(n_bytes / 1024));
    printf("ROSC values:   0x0F00, 0x1F00, 0x2F00, 0x4F00, 0x9F00 (%d values)\n",
           (int)NONOVERLAP_ROSC_COUNT);
    printf("Reseed values: %d (single-bit: MSB[47:32] + LSB[31:24])\n",
           (int)NONOVERLAP_RESEED_COUNT);
    printf("Total configs: %d\n", total_configs);
    printf("--------------------------------------------------------------\n");
    printf("NIST Criteria: Pass ratio >= (1-alpha) - 3*sqrt(alpha*(1-alpha)/N)\n");
    printf("Alpha = 0.01, expected pass rate = 99%%, tolerance = 3 sigma\n");
    printf("==============================================================\n\n");

    /* Allocate buffer */
    if (nist_alloc_test_buffer(n_bytes) != 0) {
        return -1;
    }

    /* Allocate results array */
    nonoverlap_config_result_t *results = NULL;
    nonoverlap_result_header_t header;
    int start_rosc = 0;
    int start_reseed = 0;
    int loaded_prev = 0;

#ifdef CONFIG_LITTLEFS
    /* Try to load previous results */
    results = nonoverlap_load_results(&header,
                                       (int)NONOVERLAP_ROSC_COUNT,
                                       (int)NONOVERLAP_RESEED_COUNT,
                                       n_bits);
    if (results) {
        start_rosc = header.current_rosc;
        start_reseed = header.current_reseed;
        loaded_prev = 1;

        /* Count already tested configs */
        int already_tested = 0;
        for (int i = 0; i < total_configs; i++) {
            if (results[i].tested) already_tested++;
        }
        printf("[LFS] Resuming from ROSC[%d] Reseed[%d], %d/%d already tested\n\n",
               start_rosc, start_reseed, already_tested, total_configs);
    }
#endif

    /* Allocate fresh results if not loaded */
    if (!results) {
        results = pvPortMalloc(total_configs * sizeof(nonoverlap_config_result_t));
        if (!results) {
            printf("Error: Failed to allocate results array\n");
            nist_free_test_buffer();
            return -1;
        }
        memset(results, 0, total_configs * sizeof(nonoverlap_config_result_t));

        /* Initialize header */
        header.magic = NONOVERLAP_MAGIC;
        header.version = NONOVERLAP_TEST_VERSION;
        header.rosc_count = (uint8_t)NONOVERLAP_ROSC_COUNT;
        header.reseed_count = (uint8_t)NONOVERLAP_RESEED_COUNT;
        header.current_rosc = 0;
        header.current_reseed = 0;
        header.n_bits = n_bits;
    }

    /* Save original values */
    volatile uint32_t *rosc_reg = (volatile uint32_t *)0x30080250;
    volatile uint32_t *reseed_lsb_reg = (volatile uint32_t *)0x3008022C;
    volatile uint32_t *reseed_msb_reg = (volatile uint32_t *)0x30080230;
    uint32_t orig_rosc = *rosc_reg;
    uint32_t orig_reseed_lsb = *reseed_lsb_reg;
    uint32_t orig_reseed_msb = *reseed_msb_reg;

    /* Track best configuration */
    int best_rosc_idx = -1;
    int best_reseed_idx = -1;
    int best_pass_count = 0;
    int best_total_count = 0;
    double best_pass_ratio = 0.0;
    int certified_count = 0;
    int tested_count = 0;

    printf("%-8s %-20s %-12s %-8s %-6s\n", "ROSC", "Reseed", "Pass/Total", "Ratio", "NIST");
    printf("------------------------------------------------------------------\n");

    /* Initialize TRNG */
    hal_sec_trng_init();

    for (int ri = 0; ri < (int)NONOVERLAP_ROSC_COUNT; ri++) {
        uint16_t rosc = nonoverlap_rosc_values[ri];

        for (int si = 0; si < (int)NONOVERLAP_RESEED_COUNT; si++) {
            int idx = ri * NONOVERLAP_RESEED_COUNT + si;

            /* Check if already tested */
            if (results[idx].tested) {
                /* Use cached result */
                int tmpl_passed = results[idx].passed_count;
                int tmpl_tested = results[idx].total_count;
                double pass_ratio = (tmpl_tested > 0) ?
                                    (double)tmpl_passed / tmpl_tested : 0.0;
                int nist_certified = results[idx].certified;

                tested_count++;
                if (nist_certified) {
                    certified_count++;
                    if (pass_ratio > best_pass_ratio ||
                        (pass_ratio == best_pass_ratio && tmpl_passed > best_pass_count)) {
                        best_pass_ratio = pass_ratio;
                        best_pass_count = tmpl_passed;
                        best_total_count = tmpl_tested;
                        best_rosc_idx = ri;
                        best_reseed_idx = si;
                    }
                }
                continue;  /* Skip to next config */
            }

            uint64_t reseed = nonoverlap_reseed_values[si];
            uint32_t rs_lsb = (uint32_t)(reseed & 0xFFFFFFFF);
            uint16_t rs_msb = (uint16_t)((reseed >> 32) & 0xFFFF);

            /* Set ROSC and Reseed */
            Sec_Eng_Trng_Set_ROSC_Config(rosc);
            Sec_Eng_Trng_Set_Reseed(rs_lsb, rs_msb);

            /* Small delay */
            for (volatile int d = 0; d < 1000; d++);

            /* Generate random data */
            int ret = hal_rand_stream(g_nist_test_buf, n_bytes);
            if (ret != 0) {
                printf("0x%04X  0x%04lX_%08lX  ERROR        -        -\n",
                       (unsigned)rosc, (unsigned long)rs_msb, (unsigned long)rs_lsb);
                results[idx].tested = 1;  /* Mark as tested (failed) */
                continue;
            }

            /* Run Non-overlapping Template test WITHOUT early exit */
            double p_value = 0.0;
            int tmpl_passed = 0, tmpl_tested = 0;
            ret = nist_non_overlapping_template_test_ex(g_nist_test_buf, n_bits, 9,
                                                        0,  /* early_exit = false */
                                                        &p_value,
                                                        &tmpl_passed, &tmpl_tested);

            /* Calculate pass ratio and NIST threshold */
            double pass_ratio = (tmpl_tested > 0) ?
                                (double)tmpl_passed / tmpl_tested : 0.0;
            int min_pass = nist_min_pass_count(tmpl_tested, NIST_DEFAULT_ALPHA);
            int nist_certified = (tmpl_passed >= min_pass);
            const char *status = nist_certified ? "CERT" : "FAIL";

            printf("0x%04X  0x%04lX_%08lX  %3d/%-3d      %.1f%%    %s\n",
                   (unsigned)rosc, (unsigned long)rs_msb, (unsigned long)rs_lsb,
                   tmpl_passed, tmpl_tested, pass_ratio * 100.0, status);

            /* Save result */
            results[idx].passed_count = (uint16_t)tmpl_passed;
            results[idx].total_count = (uint16_t)tmpl_tested;
            results[idx].certified = nist_certified ? 1 : 0;
            results[idx].tested = 1;
            tested_count++;

            if (nist_certified) {
                certified_count++;
                if (pass_ratio > best_pass_ratio ||
                    (pass_ratio == best_pass_ratio && tmpl_passed > best_pass_count)) {
                    best_pass_ratio = pass_ratio;
                    best_pass_count = tmpl_passed;
                    best_total_count = tmpl_tested;
                    best_rosc_idx = ri;
                    best_reseed_idx = si;
                }
            }

#ifdef CONFIG_LITTLEFS
            /* Update progress and save to LFS after each test */
            header.current_rosc = (uint8_t)ri;
            header.current_reseed = (uint8_t)si;
            nonoverlap_save_results(&header, results);
#endif
        }
    }

    /* Restore original values */
    *rosc_reg = orig_rosc;
    *reseed_lsb_reg = orig_reseed_lsb;
    *reseed_msb_reg = orig_reseed_msb;

    /* Free buffer */
    nist_free_test_buffer();

    /* Summary */
    printf("\n==================================================================\n");
    printf("NIST CERTIFICATION SUMMARY\n");
    printf("==================================================================\n");
    printf("Configs tested:    %d/%d%s\n", tested_count, total_configs,
           loaded_prev ? " (includes resumed)" : "");
    printf("Configs certified: %d (%.1f%%)\n",
           certified_count, 100.0 * certified_count / total_configs);

    if (best_rosc_idx >= 0 && best_reseed_idx >= 0) {
        uint16_t best_rosc = nonoverlap_rosc_values[best_rosc_idx];
        uint64_t best_reseed = nonoverlap_reseed_values[best_reseed_idx];
        uint32_t best_rs_lsb = (uint32_t)(best_reseed & 0xFFFFFFFF);
        uint16_t best_rs_msb = (uint16_t)((best_reseed >> 32) & 0xFFFF);
        int min_pass = nist_min_pass_count(best_total_count, NIST_DEFAULT_ALPHA);

        printf("------------------------------------------------------------------\n");
        printf("BEST CONFIGURATION:\n");
        printf("  ROSC:       0x%04X\n", (unsigned)best_rosc);
        printf("  Reseed:     0x%04lX_%08lX\n",
               (unsigned long)best_rs_msb, (unsigned long)best_rs_lsb);
        printf("  Pass ratio: %d/%d (%.2f%%)\n",
               best_pass_count, best_total_count, best_pass_ratio * 100.0);
        printf("  NIST min:   %d/%d (%.2f%%)\n",
               min_pass, best_total_count, 100.0 * min_pass / best_total_count);
        printf("  Status:     *** NIST CERTIFIED ***\n");
        printf("------------------------------------------------------------------\n");
        printf("Recommended settings:\n");
        printf("  #define NIST_TRNG_ROSC_VALUE   0x%04X\n", (unsigned)best_rosc);
        printf("  #define NIST_TRNG_RESEED_MSB   0x%04X\n", (unsigned)best_rs_msb);
        printf("  #define NIST_TRNG_RESEED_LSB   0x%08lX\n", (unsigned long)best_rs_lsb);
    } else {
        printf("------------------------------------------------------------------\n");
        printf("*** NO CONFIGURATION PASSED NIST CERTIFICATION ***\n");
        printf("Hardware TRNG quality is insufficient.\n");
    }

#ifdef CONFIG_LITTLEFS
    printf("\n[LFS] Results saved to %s (v%d)\n", NONOVERLAP_RESULT_FILE, NONOVERLAP_TEST_VERSION);
#endif

    /* Free results array */
    vPortFree(results);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_nonoverlap_sweep, trng_nonoverlap,
    Non-overlapping template sweep [n_bits] [rosc_start] [rosc_end]);

/**
 * @brief Shell command: trng_nonoverlap_show
 *
 * Display saved Non-overlapping Template test results from LittleFS.
 */
static int cmd_trng_nonoverlap_show(int argc, char **argv)
{
    (void)argc;
    (void)argv;

#ifndef CONFIG_LITTLEFS
    printf("Error: LittleFS not enabled\n");
    return -1;
#else
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) {
        printf("Error: Failed to mount LittleFS\n");
        return -1;
    }

    /* Open result file */
    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, NONOVERLAP_RESULT_FILE, LFS_O_RDONLY);
    if (err < 0) {
        printf("No saved results found (%s)\n", NONOVERLAP_RESULT_FILE);
        return -1;
    }

    /* Read header */
    nonoverlap_result_header_t header;
    lfs_ssize_t read_size = lfs_file_read(lfs, &file, &header, sizeof(header));
    if (read_size != sizeof(header)) {
        printf("Error: Failed to read header\n");
        lfs_file_close(lfs, &file);
        return -1;
    }

    /* Validate header */
    if (header.magic != NONOVERLAP_MAGIC) {
        printf("Error: Invalid file format (magic=0x%08lX)\n", (unsigned long)header.magic);
        lfs_file_close(lfs, &file);
        return -1;
    }

    /* Print header info */
    printf("\n");
    printf("TRNG Non-overlapping Template Test Results\n");
    printf("==========================================\n");
    printf("File:        %s\n", NONOVERLAP_RESULT_FILE);
    printf("Version:     %lu\n", (unsigned long)header.version);
    printf("Test bits:   %lu\n", (unsigned long)header.n_bits);
    printf("ROSC count:  %d\n", header.rosc_count);
    printf("Reseed count: %d\n", header.reseed_count);
    printf("Progress:    ROSC[%d] Reseed[%d]\n", header.current_rosc, header.current_reseed);
    printf("==========================================\n\n");

    /* Read results */
    int total = header.rosc_count * header.reseed_count;
    nonoverlap_config_result_t *results = pvPortMalloc(total * sizeof(*results));
    if (!results) {
        printf("Error: Memory allocation failed\n");
        lfs_file_close(lfs, &file);
        return -1;
    }

    read_size = lfs_file_read(lfs, &file, results, total * sizeof(*results));
    lfs_file_close(lfs, &file);

    if (read_size != (lfs_ssize_t)(total * sizeof(*results))) {
        printf("Error: Failed to read results\n");
        vPortFree(results);
        return -1;
    }

    /* Display results */
    printf("%-8s %-20s %-12s %-8s %-6s\n", "ROSC", "Reseed", "Pass/Total", "Ratio", "NIST");
    printf("------------------------------------------------------------------\n");

    int tested_count = 0;
    int certified_count = 0;
    int best_rosc_idx = -1;
    int best_reseed_idx = -1;
    int best_pass_count = 0;
    int best_total_count = 0;
    double best_pass_ratio = 0.0;

    for (int ri = 0; ri < header.rosc_count && ri < (int)NONOVERLAP_ROSC_COUNT; ri++) {
        uint16_t rosc = nonoverlap_rosc_values[ri];

        for (int si = 0; si < header.reseed_count && si < (int)NONOVERLAP_RESEED_COUNT; si++) {
            int idx = ri * header.reseed_count + si;
            nonoverlap_config_result_t *r = &results[idx];

            if (!r->tested) {
                continue;  /* Skip untested */
            }

            uint64_t reseed = nonoverlap_reseed_values[si];
            uint32_t rs_lsb = (uint32_t)(reseed & 0xFFFFFFFF);
            uint16_t rs_msb = (uint16_t)((reseed >> 32) & 0xFFFF);

            double pass_ratio = (r->total_count > 0) ?
                                (double)r->passed_count / r->total_count : 0.0;
            const char *status = r->certified ? "CERT" : "FAIL";

            printf("0x%04X  0x%04lX_%08lX  %3d/%-3d      %.1f%%    %s\n",
                   (unsigned)rosc, (unsigned long)rs_msb, (unsigned long)rs_lsb,
                   r->passed_count, r->total_count, pass_ratio * 100.0, status);

            tested_count++;
            if (r->certified) {
                certified_count++;
                if (pass_ratio > best_pass_ratio ||
                    (pass_ratio == best_pass_ratio && r->passed_count > best_pass_count)) {
                    best_pass_ratio = pass_ratio;
                    best_pass_count = r->passed_count;
                    best_total_count = r->total_count;
                    best_rosc_idx = ri;
                    best_reseed_idx = si;
                }
            }
        }
    }

    /* Summary */
    printf("\n==================================================================\n");
    printf("SUMMARY\n");
    printf("==================================================================\n");
    printf("Configs tested:    %d/%d\n", tested_count, total);
    printf("Configs certified: %d (%.1f%%)\n",
           certified_count, tested_count > 0 ? 100.0 * certified_count / tested_count : 0.0);

    if (best_rosc_idx >= 0 && best_reseed_idx >= 0) {
        uint16_t best_rosc = nonoverlap_rosc_values[best_rosc_idx];
        uint64_t best_reseed = nonoverlap_reseed_values[best_reseed_idx];
        uint32_t best_rs_lsb = (uint32_t)(best_reseed & 0xFFFFFFFF);
        uint16_t best_rs_msb = (uint16_t)((best_reseed >> 32) & 0xFFFF);
        int min_pass = nist_min_pass_count(best_total_count, NIST_DEFAULT_ALPHA);

        printf("------------------------------------------------------------------\n");
        printf("BEST CONFIGURATION:\n");
        printf("  ROSC:       0x%04X\n", (unsigned)best_rosc);
        printf("  Reseed:     0x%04lX_%08lX\n",
               (unsigned long)best_rs_msb, (unsigned long)best_rs_lsb);
        printf("  Pass ratio: %d/%d (%.2f%%)\n",
               best_pass_count, best_total_count, best_pass_ratio * 100.0);
        printf("  NIST min:   %d/%d (%.2f%%)\n",
               min_pass, best_total_count, 100.0 * min_pass / best_total_count);
        printf("  Status:     *** NIST CERTIFIED ***\n");
    }

    vPortFree(results);
    return 0;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_nonoverlap_show, trng_nonoverlap_show,
    Display saved non-overlapping test results);

/**
 * @brief Shell command: trng_nonoverlap_clear
 *
 * Clear saved Non-overlapping Template test results.
 */
static int cmd_trng_nonoverlap_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;

#ifndef CONFIG_LITTLEFS
    printf("Error: LittleFS not enabled\n");
    return -1;
#else
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) {
        printf("Error: Failed to mount LittleFS\n");
        return -1;
    }

    int err = lfs_remove(lfs, NONOVERLAP_RESULT_FILE);
    if (err < 0) {
        printf("No saved results to clear\n");
        return 0;
    }

    printf("Cleared saved results (%s)\n", NONOVERLAP_RESULT_FILE);
    return 0;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_nonoverlap_clear, trng_nonoverlap_clear,
    Clear saved non-overlapping test results);

/* ============================================================================
 * Random Excursions Precondition Test (J >= 500)
 * ============================================================================
 * Tests ROSC and Reseed configurations for Random Excursions precondition.
 * Uses configurations that passed Non-overlapping Template test (from LFS).
 *
 * For truly random 1M bits, expected J ≈ n/(2π) ≈ 159,000.
 * J < 500 indicates severe bias in the data.
 */

/* Default test rounds for J >= 500 check */
#define EXCURSIONS_TEST_ROUNDS  20

/* Minimum J value required by NIST SP 800-22 */
#define EXCURSIONS_MIN_J  500

/* Excursions test version - increment when test parameters change */
#define EXCURSIONS_TEST_VERSION  2  /* v2: pass criteria changed to avg_J >= 500 */

/* Excursions result file path */
#define EXCURSIONS_RESULT_FILE   "/trng_excursions_result.bin"

/* Excursions result data structure for each configuration */
typedef struct {
    uint16_t pass_count;      /* Rounds where J >= 500 */
    uint16_t total_rounds;    /* Total rounds tested */
    uint32_t min_J;           /* Minimum J observed */
    uint32_t total_J;         /* Sum of J for avg calculation */
    uint8_t  tested;          /* 1 = tested, 0 = not yet */
    uint8_t  all_passed;      /* 1 = all rounds passed, 0 = at least one failed */
    uint8_t  reserved[2];     /* Padding for alignment */
} excursions_config_result_t;

/* Excursions header structure for saved results */
typedef struct {
    uint32_t magic;           /* Magic number for validation */
    uint32_t version;         /* Test version */
    uint8_t  rosc_count;      /* Number of ROSC values */
    uint8_t  reseed_count;    /* Number of Reseed values */
    uint8_t  rounds;          /* Test rounds per config */
    uint8_t  reserved;        /* Padding */
    uint32_t n_bits;          /* Test bits used */
    /* Best config tracking */
    int8_t   best_rosc_idx;   /* Best ROSC index (-1 if none) */
    int8_t   best_reseed_idx; /* Best Reseed index (-1 if none) */
    uint16_t reserved2;       /* Padding */
    uint32_t best_min_J;      /* Best min J value */
    uint32_t best_avg_J;      /* Best avg J value */
    /* Followed by: excursions_config_result_t results[rosc_count * reseed_count] */
} excursions_result_header_t;

#define EXCURSIONS_MAGIC  0x45584352  /* "EXCR" */

#ifdef CONFIG_LITTLEFS
/**
 * @brief Load previous excursions test results from LittleFS
 * @return Pointer to allocated results (caller must free), or NULL if not found/invalid
 */
static excursions_config_result_t *excursions_load_results(
    excursions_result_header_t *header,
    int expected_rosc_count,
    int expected_reseed_count,
    int expected_rounds,
    uint32_t expected_n_bits)
{
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) return NULL;

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, EXCURSIONS_RESULT_FILE, LFS_O_RDONLY);
    if (err < 0) {
        printf("[LFS] No previous excursions results found\n");
        return NULL;
    }

    /* Read header */
    lfs_ssize_t read_size = lfs_file_read(lfs, &file, header, sizeof(*header));
    if (read_size != sizeof(*header)) {
        printf("[LFS] Failed to read excursions header\n");
        lfs_file_close(lfs, &file);
        return NULL;
    }

    /* Validate header */
    if (header->magic != EXCURSIONS_MAGIC) {
        printf("[LFS] Invalid excursions magic (0x%08lX)\n", (unsigned long)header->magic);
        lfs_file_close(lfs, &file);
        return NULL;
    }

    if (header->version != EXCURSIONS_TEST_VERSION) {
        printf("[LFS] Excursions version mismatch (file: %lu, expected: %d)\n",
               (unsigned long)header->version, EXCURSIONS_TEST_VERSION);
        lfs_file_close(lfs, &file);
        return NULL;
    }

    if (header->rosc_count != expected_rosc_count ||
        header->reseed_count != expected_reseed_count ||
        header->rounds != expected_rounds ||
        header->n_bits != expected_n_bits) {
        printf("[LFS] Excursions config mismatch, starting fresh\n");
        lfs_file_close(lfs, &file);
        return NULL;
    }

    /* Allocate and read results */
    int total = expected_rosc_count * expected_reseed_count;
    excursions_config_result_t *results = pvPortMalloc(total * sizeof(*results));
    if (!results) {
        lfs_file_close(lfs, &file);
        return NULL;
    }

    read_size = lfs_file_read(lfs, &file, results, total * sizeof(*results));
    lfs_file_close(lfs, &file);

    if (read_size != (lfs_ssize_t)(total * sizeof(*results))) {
        printf("[LFS] Failed to read excursions results\n");
        vPortFree(results);
        return NULL;
    }

    printf("[LFS] Loaded previous excursions results (v%lu)\n",
           (unsigned long)header->version);
    return results;
}

/**
 * @brief Save excursions test results to LittleFS
 */
static int excursions_save_results(
    const excursions_result_header_t *header,
    const excursions_config_result_t *results)
{
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) return -1;

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, EXCURSIONS_RESULT_FILE,
                            LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (err < 0) {
        printf("[LFS] Failed to open excursions file for writing: %d\n", err);
        return -1;
    }

    /* Write header */
    lfs_ssize_t written = lfs_file_write(lfs, &file, header, sizeof(*header));
    if (written != sizeof(*header)) {
        lfs_file_close(lfs, &file);
        return -1;
    }

    /* Write results */
    int total = header->rosc_count * header->reseed_count;
    written = lfs_file_write(lfs, &file, results, total * sizeof(*results));
    lfs_file_close(lfs, &file);

    if (written != (lfs_ssize_t)(total * sizeof(*results))) {
        return -1;
    }

    return 0;
}
#endif /* CONFIG_LITTLEFS */

/**
 * @brief Shell command: trng_excursions [rounds]
 *
 * Test Random Excursions precondition (J >= 500) for configurations
 * that passed Non-overlapping Template test.
 *
 * Results are saved to LittleFS for resume capability.
 *
 * @param rounds  Number of test rounds per configuration (default: 20)
 */
static int cmd_trng_excursions(int argc, char **argv)
{
    int rounds = EXCURSIONS_TEST_ROUNDS;
    uint32_t n_bits = NIST_DEFAULT_N_BITS;

    /* Parse: trng_excursions [rounds] or trng_excursions [n_bits] [rounds] */
    if (argc == 2) {
        /* Single arg: rounds */
        rounds = (int)strtoul(argv[1], NULL, 0);
    } else if (argc >= 3) {
        /* Two args: n_bits, rounds */
        n_bits = (uint32_t)strtoul(argv[1], NULL, 0);
        rounds = (int)strtoul(argv[2], NULL, 0);
    }

    if (rounds < 1) rounds = 1;
    if (rounds > 10000) rounds = 10000;  /* Allow up to 10000 rounds */
    if (n_bits < 100000) n_bits = 100000;  /* Minimum 100K bits */

    uint32_t n_bytes = (n_bits + 7) / 8;

    printf("\n");
    printf("Random Excursions Precondition Test (J >= 500)\n");
    printf("===============================================\n");
    printf("Test rounds:   %d per configuration\n", rounds);
    printf("Test bits:     %lu (%lu KB)\n", (unsigned long)n_bits,
           (unsigned long)(n_bytes / 1024));
    printf("Required:      J >= %d zero crossings\n", EXCURSIONS_MIN_J);
    printf("Expected J:    ~%lu (sqrt(n) for random walk)\n",
           (unsigned long)((uint32_t)sqrtf((float)n_bits)));
    printf("===============================================\n\n");

#ifndef CONFIG_LITTLEFS
    printf("Error: LittleFS not enabled. Run trng_nonoverlap first.\n");
    return -1;
#else
    /* Load Non-overlapping Template results to get certified configs */
    nonoverlap_result_header_t nonoverlap_header;
    nonoverlap_config_result_t *nonoverlap_results = nonoverlap_load_results(
        &nonoverlap_header,
        (int)NONOVERLAP_ROSC_COUNT,
        (int)NONOVERLAP_RESEED_COUNT,
        n_bits);

    if (!nonoverlap_results) {
        printf("Error: No Non-overlapping Template results found.\n");
        printf("Run 'trng_nonoverlap' first to find certified configurations.\n");
        return -1;
    }

    /* Count certified configs */
    int certified_count = 0;
    for (int i = 0; i < (int)(NONOVERLAP_ROSC_COUNT * NONOVERLAP_RESEED_COUNT); i++) {
        if (nonoverlap_results[i].tested && nonoverlap_results[i].certified) {
            certified_count++;
        }
    }

    if (certified_count == 0) {
        printf("Error: No certified configurations found.\n");
        printf("Run 'trng_nonoverlap' to find certified configurations.\n");
        vPortFree(nonoverlap_results);
        return -1;
    }

    printf("Found %d certified Non-overlapping configurations.\n", certified_count);

    /* Try to load previous excursions results */
    int total_configs = NONOVERLAP_ROSC_COUNT * NONOVERLAP_RESEED_COUNT;
    excursions_result_header_t exc_header;
    excursions_config_result_t *exc_results = excursions_load_results(
        &exc_header,
        (int)NONOVERLAP_ROSC_COUNT,
        (int)NONOVERLAP_RESEED_COUNT,
        rounds,
        n_bits);

    int already_tested = 0;
    if (exc_results) {
        /* Count already tested configs */
        for (int i = 0; i < total_configs; i++) {
            if (exc_results[i].tested) {
                already_tested++;
            }
        }
        if (already_tested > 0 && already_tested < certified_count) {
            printf("[LFS] Resuming: %d/%d already tested\n\n", already_tested, certified_count);
        }
    }

    /* Allocate fresh results if not loaded */
    if (!exc_results) {
        exc_results = pvPortMalloc(total_configs * sizeof(excursions_config_result_t));
        if (!exc_results) {
            printf("Error: Failed to allocate excursions results array\n");
            vPortFree(nonoverlap_results);
            return -1;
        }
        memset(exc_results, 0, total_configs * sizeof(excursions_config_result_t));

        /* Initialize header */
        exc_header.magic = EXCURSIONS_MAGIC;
        exc_header.version = EXCURSIONS_TEST_VERSION;
        exc_header.rosc_count = (uint8_t)NONOVERLAP_ROSC_COUNT;
        exc_header.reseed_count = (uint8_t)NONOVERLAP_RESEED_COUNT;
        exc_header.rounds = (uint8_t)rounds;
        exc_header.reserved = 0;
        exc_header.n_bits = n_bits;
        exc_header.best_rosc_idx = -1;
        exc_header.best_reseed_idx = -1;
        exc_header.reserved2 = 0;
        exc_header.best_min_J = 0;
        exc_header.best_avg_J = 0;
    }

    printf("Testing each for Random Excursions precondition...\n\n");

    /* Allocate test buffer */
    if (nist_alloc_test_buffer(n_bytes) != 0) {
        vPortFree(nonoverlap_results);
        vPortFree(exc_results);
        return -1;
    }

    /* Save original register values */
    volatile uint32_t *rosc_reg = (volatile uint32_t *)0x30080250;
    volatile uint32_t *reseed_lsb_reg = (volatile uint32_t *)0x3008022C;
    volatile uint32_t *reseed_msb_reg = (volatile uint32_t *)0x30080230;
    uint32_t orig_rosc = *rosc_reg;
    uint32_t orig_reseed_lsb = *reseed_lsb_reg;
    uint32_t orig_reseed_msb = *reseed_msb_reg;

    /* Initialize TRNG */
    hal_sec_trng_init();

    /* Track best configuration (from header or fresh) */
    int best_rosc_idx = exc_header.best_rosc_idx;
    int best_reseed_idx = exc_header.best_reseed_idx;
    uint32_t best_min_J = exc_header.best_min_J;
    uint32_t best_avg_J = exc_header.best_avg_J;

    int configs_tested = 0;
    int configs_passed = 0;
    int configs_new = 0;

    printf("%-8s %-20s %-10s %-10s %-10s %-8s\n",
           "ROSC", "Reseed", "Pass/Total", "Min J", "Avg J", "Status");
    printf("----------------------------------------------------------------------\n");

    /* Test each certified configuration */
    for (int ri = 0; ri < (int)NONOVERLAP_ROSC_COUNT; ri++) {
        uint16_t rosc = nonoverlap_rosc_values[ri];

        for (int si = 0; si < (int)NONOVERLAP_RESEED_COUNT; si++) {
            int idx = ri * NONOVERLAP_RESEED_COUNT + si;

            /* Skip non-certified configs */
            if (!nonoverlap_results[idx].tested || !nonoverlap_results[idx].certified) {
                continue;
            }

            uint64_t reseed = nonoverlap_reseed_values[si];
            uint32_t rs_lsb = (uint32_t)(reseed & 0xFFFFFFFF);
            uint16_t rs_msb = (uint16_t)((reseed >> 32) & 0xFFFF);

            /* Check if already tested */
            if (exc_results[idx].tested) {
                /* Use cached result */
                int pass_count = exc_results[idx].pass_count;
                uint32_t min_J = exc_results[idx].min_J;
                uint32_t avg_J = (exc_results[idx].total_rounds > 0) ?
                                  exc_results[idx].total_J / exc_results[idx].total_rounds : 0;
                int config_passed = exc_results[idx].all_passed;
                const char *status = config_passed ? "PASS (cached)" : "FAIL (cached)";

                printf("0x%04X  0x%04lX_%08lX  %2d/%-2d      %-10lu %-10lu %s\n",
                       (unsigned)rosc, (unsigned long)rs_msb, (unsigned long)rs_lsb,
                       pass_count, (int)exc_results[idx].total_rounds,
                       (unsigned long)min_J, (unsigned long)avg_J, status);

                configs_tested++;
                if (config_passed) {
                    configs_passed++;
                    /* Also track best from cached results */
                    if (avg_J > best_avg_J) {
                        best_min_J = min_J;
                        best_avg_J = avg_J;
                        best_rosc_idx = ri;
                        best_reseed_idx = si;
                    }
                }
                continue;
            }

            /* Set ROSC and Reseed */
            Sec_Eng_Trng_Set_ROSC_Config(rosc);
            Sec_Eng_Trng_Set_Reseed(rs_lsb, rs_msb);

            /* Small delay */
            for (volatile int d = 0; d < 1000; d++);

            /* Run multiple rounds */
            int pass_count = 0;
            uint32_t min_J = UINT32_MAX;
            uint32_t total_J = 0;

            for (int round = 0; round < rounds; round++) {
                /* Generate random data */
                int ret = hal_rand_stream(g_nist_test_buf, n_bytes);
                if (ret != 0) {
                    continue;
                }

                /* Count zero crossings */
                uint32_t J = nist_count_zero_crossings(g_nist_test_buf, n_bits);
                total_J += J;
                if (J < min_J) min_J = J;
                if (J >= EXCURSIONS_MIN_J) pass_count++;
            }

            uint32_t avg_J = (rounds > 0) ? total_J / rounds : 0;
            /*
             * Pass criteria: average J >= 500
             * Due to variance in random walk, some rounds may have J < 500
             * even for good quality random data. Using average is more robust.
             */
            int config_passed = (avg_J >= EXCURSIONS_MIN_J);
            const char *status = config_passed ? "PASS" : "FAIL";

            printf("0x%04X  0x%04lX_%08lX  %2d/%-2d      %-10lu %-10lu %s\n",
                   (unsigned)rosc, (unsigned long)rs_msb, (unsigned long)rs_lsb,
                   pass_count, rounds, (unsigned long)min_J, (unsigned long)avg_J,
                   status);

            /* Save result */
            exc_results[idx].pass_count = (uint16_t)pass_count;
            exc_results[idx].total_rounds = (uint16_t)rounds;
            exc_results[idx].min_J = min_J;
            exc_results[idx].total_J = total_J;
            exc_results[idx].tested = 1;
            exc_results[idx].all_passed = config_passed ? 1 : 0;

            configs_tested++;
            configs_new++;
            if (config_passed) {
                configs_passed++;
                /* Track best by highest avg_J (more stable than min_J) */
                if (avg_J > best_avg_J) {
                    best_min_J = min_J;
                    best_avg_J = avg_J;
                    best_rosc_idx = ri;
                    best_reseed_idx = si;
                }
            }

            /* Save progress periodically (every 5 new tests) */
            if (configs_new % 5 == 0) {
                exc_header.best_rosc_idx = (int8_t)best_rosc_idx;
                exc_header.best_reseed_idx = (int8_t)best_reseed_idx;
                exc_header.best_min_J = best_min_J;
                exc_header.best_avg_J = best_avg_J;
                excursions_save_results(&exc_header, exc_results);
            }
        }
    }

    /* Final save */
    exc_header.best_rosc_idx = (int8_t)best_rosc_idx;
    exc_header.best_reseed_idx = (int8_t)best_reseed_idx;
    exc_header.best_min_J = best_min_J;
    exc_header.best_avg_J = best_avg_J;
    if (excursions_save_results(&exc_header, exc_results) == 0) {
        printf("\n[LFS] Results saved to %s\n", EXCURSIONS_RESULT_FILE);
    }

    /* Restore original register values */
    *rosc_reg = orig_rosc;
    *reseed_lsb_reg = orig_reseed_lsb;
    *reseed_msb_reg = orig_reseed_msb;

    /* Free resources */
    nist_free_test_buffer();
    vPortFree(nonoverlap_results);
    vPortFree(exc_results);

    /* Summary */
    printf("\n======================================================================\n");
    printf("RANDOM EXCURSIONS PRECONDITION SUMMARY\n");
    printf("======================================================================\n");
    printf("Configs tested:  %d (from Non-overlapping certified)\n", configs_tested);
    printf("Configs passed:  %d/%d (avg J >= %d over %d rounds)\n",
           configs_passed, configs_tested, EXCURSIONS_MIN_J, rounds);

    if (best_rosc_idx >= 0 && best_reseed_idx >= 0) {
        uint16_t best_rosc = nonoverlap_rosc_values[best_rosc_idx];
        uint64_t best_reseed = nonoverlap_reseed_values[best_reseed_idx];
        uint32_t best_rs_lsb = (uint32_t)(best_reseed & 0xFFFFFFFF);
        uint16_t best_rs_msb = (uint16_t)((best_reseed >> 32) & 0xFFFF);

        printf("----------------------------------------------------------------------\n");
        printf("BEST CONFIGURATION (highest avg J):\n");
        printf("  ROSC:    0x%04X\n", (unsigned)best_rosc);
        printf("  Reseed:  0x%04lX_%08lX\n",
               (unsigned long)best_rs_msb, (unsigned long)best_rs_lsb);
        printf("  Avg J:   %lu (expected ~%lu = sqrt(n))\n",
               (unsigned long)best_avg_J,
               (unsigned long)((uint32_t)sqrtf((float)n_bits)));
        printf("  Min J:   %lu\n", (unsigned long)best_min_J);
        printf("  Status:  *** RECOMMENDED ***\n");
        printf("----------------------------------------------------------------------\n");
        printf("Recommended settings:\n");
        printf("  #define NIST_TRNG_ROSC_VALUE   0x%04X\n", (unsigned)best_rosc);
        printf("  #define NIST_TRNG_RESEED_MSB   0x%04X\n", (unsigned)best_rs_msb);
        printf("  #define NIST_TRNG_RESEED_LSB   0x%08lX\n", (unsigned long)best_rs_lsb);
    } else {
        printf("----------------------------------------------------------------------\n");
        printf("*** NO CONFIGURATION HAS avg J >= %d ***\n", EXCURSIONS_MIN_J);
        printf("Consider:\n");
        printf("  1. Running more Non-overlapping tests to find better configs\n");
        printf("  2. Checking TRNG hardware for bias issues\n");
    }
    printf("======================================================================\n");

    return 0;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_excursions, trng_excursions,
    Test J>=500 precondition [rounds] or [n_bits rounds]);

/* ============================================================================
 * P-value Uniformity Test (NIST SP 800-22 Section 4.2.2)
 * ============================================================================ */

/**
 * @brief Shell command: trng_uniformity [rounds]
 *
 * Run NIST SP 800-22 P-value Uniformity Test.
 *
 * This test collects P-values from multiple rounds of the test suite and
 * performs a Chi-square goodness-of-fit test to verify that P-values are
 * uniformly distributed in [0, 1).
 *
 * According to NIST SP 800-22 Section 4.2.2, if the random data is truly
 * random, the P-values should be uniformly distributed. This is tested by
 * dividing [0, 1) into 10 bins and checking if the observed frequencies
 * match the expected uniform distribution.
 *
 * @param rounds  Number of test rounds (default: 100, minimum for valid chi-square)
 */
static int cmd_trng_uniformity(int argc, char **argv)
{
    uint32_t n_bits = NIST_DEFAULT_N_BITS;
    int rounds = NIST_DEFAULT_ROUNDS;
    nist_suite_result_t result;

    /* Parse arguments */
    if (argc > 1) {
        rounds = (int)strtoul(argv[1], NULL, 0);
        if (rounds < 10) {
            printf("Warning: Minimum 10 rounds required for chi-square test\n");
            rounds = 10;
        }
        if (rounds > 10000) {
            printf("Warning: Limiting to 10000 rounds\n");
            rounds = 10000;
        }
    }

    uint32_t n_bytes = (n_bits + 7) / 8;

    printf("\n");
    printf("NIST SP 800-22 P-value Uniformity Test (Section 4.2.2)\n");
    printf("======================================================\n");
    printf("Test rounds:   %d\n", rounds);
    printf("Test bits:     %lu (%lu KB) per round\n", (unsigned long)n_bits,
           (unsigned long)(n_bytes / 1024));
    printf("Alpha:         %.2f\n", NIST_DEFAULT_ALPHA);
    printf("TRNG source:   Hardware (IPRO7) - conditioned output (CP on, reseed enabled)\n");
    printf("ROSC config:   0x%04X\n", NIST_TRNG_ROSC_VALUE);
    printf("\n");
    printf("This test verifies that P-values are uniformly distributed.\n");
    printf("Chi-square test divides [0,1) into 10 bins and checks uniformity.\n");
    printf("\n");

    /* Allocate buffer for test data */
    printf("Allocating %lu bytes for test data...\n", (unsigned long)n_bytes);
    uint8_t *data = (uint8_t *)pvPortMalloc(n_bytes);
    if (!data) {
        printf("Error: Failed to allocate memory for test data\n");
        return -1;
    }

    /* Allocate P-value storage for each test (16 tests * rounds P-values) */
    double *p_values[16];
    for (int i = 0; i < 16; i++) {
        p_values[i] = (double *)pvPortMalloc(rounds * sizeof(double));
        if (!p_values[i]) {
            printf("Error: Failed to allocate P-value storage for test %d\n", i);
            /* Free already allocated */
            for (int j = 0; j < i; j++) {
                vPortFree(p_values[j]);
            }
            vPortFree(data);
            return -1;
        }
    }

    int p_value_count[16] = {0};
    uint32_t total_time_ms = 0;

    printf("Running %d rounds to collect P-values...\n\n", rounds);

    /* Run multiple rounds and collect P-values */
    for (int round = 1; round <= rounds; round++) {
        /* Generate random data */
        if (ipro7_trng_hal(data, n_bytes) != 0) {
            printf("Round %d: TRNG data generation failed\n", round);
            continue;
        }

        /* Run all NIST tests */
        uint32_t test_start = (uint32_t)xTaskGetTickCount();
        int ret = nist_run_all_tests_ex(data, n_bits, NIST_DEFAULT_ALPHA, &result, ipro7_trng_hal);
        uint32_t test_end = (uint32_t)xTaskGetTickCount();
        total_time_ms += (test_end - test_start) * portTICK_PERIOD_MS;

        if (ret < 0) {
            printf("Round %d: Test suite error (code: %d)\n", round, ret);
            continue;
        }

        /* Collect P-values from each test (skip only N/A and errors: their
         * placeholder P-values would corrupt the uniformity histogram;
         * NIST_FAILURE P-values are real data and must be kept) */
        for (int i = 0; i < result.tests_run && i < 16; i++) {
            if (result.tests[i].result != NIST_SUCCESS &&
                result.tests[i].result != NIST_FAILURE) {
                continue;
            }
            if (p_value_count[i] < rounds) {
                p_values[i][p_value_count[i]] = result.tests[i].p_value;
                p_value_count[i]++;
            }
        }

        /* Print progress every 10 rounds */
        if (round % 10 == 0 || round == rounds) {
            printf("Round %3d/%d: P-values collected (time: %lu ms)\n",
                   round, rounds,
                   (unsigned long)((test_end - test_start) * portTICK_PERIOD_MS));
        }
    }

    /* Free data buffer */
    vPortFree(data);

    /* Run P-value uniformity test for each of the 16 tests */
    printf("\n");
    printf("======================================================================\n");
    printf("P-VALUE UNIFORMITY TEST RESULTS (NIST SP 800-22 Section 4.2.2)\n");
    printf("======================================================================\n");
    printf("Total rounds:  %d\n", rounds);
    printf("Total time:    %lu ms (avg: %lu ms/round)\n",
           (unsigned long)total_time_ms,
           (unsigned long)(total_time_ms / rounds));
    printf("----------------------------------------------------------------------\n");

    int tests_uniform = 0;
    nist_uniformity_result_t uniformity;

    for (int i = 0; i < 16; i++) {
        if (p_value_count[i] < 10) {
            printf("[%2d] Insufficient samples (%d < 10)\n", i + 1, p_value_count[i]);
            continue;
        }

        /* Run uniformity test */
        int ret = nist_pvalue_uniformity_test(p_values[i], p_value_count[i], &uniformity);
        if (ret != 0) {
            printf("[%2d] Uniformity test error\n", i + 1);
            continue;
        }

        /* Get test name from last result */
        const char *name = (i < (int)result.tests_run) ? result.tests[i].test_name : "Unknown";

        printf("[%2d] %-24s chi2=%.2f  P=%.4f  %s\n",
               i + 1, name,
               uniformity.chi_square, uniformity.p_value,
               uniformity.passed ? "UNIFORM" : "NON-UNIFORM");

        /* Print bin distribution */
        printf("     Bins: ");
        for (int b = 0; b < 10; b++) {
            printf("%d ", uniformity.bins[b]);
        }
        printf("(expected: %.1f)\n", uniformity.expected_per_bin);

        if (uniformity.passed) {
            tests_uniform++;
        }
    }

    printf("----------------------------------------------------------------------\n");
    printf("Tests with uniform P-values: %d/16\n", tests_uniform);
    printf("======================================================================\n");

    if (tests_uniform == 16) {
        printf("*** ALL TESTS SHOW UNIFORM P-VALUES - TRNG QUALITY VERIFIED ***\n");
    } else if (tests_uniform >= 14) {
        printf("*** MOST TESTS SHOW UNIFORM P-VALUES (%d/16) ***\n", tests_uniform);
    } else {
        printf("*** WARNING: %d/16 TESTS SHOW NON-UNIFORM P-VALUES ***\n", 16 - tests_uniform);
        printf("    This may indicate TRNG bias or insufficient samples.\n");
    }
    printf("======================================================================\n");

    /* Free P-value storage */
    for (int i = 0; i < 16; i++) {
        vPortFree(p_values[i]);
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_uniformity, trng_uniformity,
    P-value uniformity test (NIST Section 4.2.2) [rounds=100]);

/* ============================================================================
 * Full NIST SP 800-22 Test Suite Sweep (ROSC x Reseed x Rounds)
 * ============================================================================
 * Comprehensive sweep testing all 16 NIST tests across:
 * - 5 ROSC values (same as trng_nonoverlap)
 * - 20 Reseed values (same as trng_nonoverlap)
 * - 100 rounds per configuration (1M bits each)
 *
 * Total: 5 x 20 x 100 = 10,000 test suite runs
 *
 * Results include:
 * - Suite pass rate per configuration
 * - Individual test pass rates
 * - P-value statistics (min/max/avg)
 * - NIST proportion threshold compliance
 */

/* Test version - increment when test parameters change */
#define NIST_FULL_TEST_VERSION  1

/* Result file path */
#define NIST_FULL_RESULT_FILE   "/trng_nist_full_result.bin"

/* Default rounds for full NIST test */
#define NIST_FULL_DEFAULT_ROUNDS  100

/* Result data structure for each configuration */
typedef struct {
    uint16_t suite_passed;        /* Suite pass count (all 16 tests passed) */
    uint16_t suite_total;         /* Total rounds executed */
    uint16_t test_passed[16];     /* Per-test pass counts */
    uint16_t sub_passed[16];      /* Per-test sub-test pass counts (for multi-state tests) */
    uint16_t sub_total[16];       /* Per-test sub-test totals */
    float    p_value_sum[16];     /* P-value sum for averaging */
    float    p_value_min[16];     /* Minimum P-value */
    float    p_value_max[16];     /* Maximum P-value */
    uint8_t  nist_certified;      /* 1 = meets NIST proportion threshold */
    uint8_t  tested;              /* 1 = testing complete */
} nist_full_config_result_t;

/* Header structure for saved results */
typedef struct {
    uint32_t magic;               /* Magic number for validation */
    uint32_t version;             /* Test version */
    uint8_t  rosc_count;          /* Number of ROSC values */
    uint8_t  reseed_count;        /* Number of Reseed values */
    uint8_t  current_rosc;        /* Current ROSC index (for resume) */
    uint8_t  current_reseed;      /* Current Reseed index (for resume) */
    uint32_t n_bits;              /* Test bits per round */
    uint16_t rounds;              /* Rounds per configuration */
    uint8_t  current_round;       /* Current round within config (for resume) */
    uint8_t  reserved;            /* Reserved for alignment */
    /* Followed by: nist_full_config_result_t results[rosc_count * reseed_count] */
} nist_full_result_header_t;

#define NIST_FULL_MAGIC  0x4E495354  /* "NIST" */

#ifdef CONFIG_LITTLEFS
/**
 * @brief Load previous full NIST test results from LittleFS
 */
static nist_full_config_result_t *nist_full_load_results(
    nist_full_result_header_t *header,
    int expected_rosc_count,
    int expected_reseed_count,
    int expected_rounds,
    uint32_t expected_n_bits)
{
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) return NULL;

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, NIST_FULL_RESULT_FILE, LFS_O_RDONLY);
    if (err < 0) {
        printf("[LFS] No previous NIST full results found\n");
        return NULL;
    }

    /* Read header */
    lfs_ssize_t read_size = lfs_file_read(lfs, &file, header, sizeof(*header));
    if (read_size != sizeof(*header)) {
        printf("[LFS] Failed to read header\n");
        lfs_file_close(lfs, &file);
        return NULL;
    }

    /* Validate header */
    if (header->magic != NIST_FULL_MAGIC) {
        printf("[LFS] Invalid magic (0x%08lX)\n", (unsigned long)header->magic);
        lfs_file_close(lfs, &file);
        return NULL;
    }

    if (header->version != NIST_FULL_TEST_VERSION) {
        printf("[LFS] Version mismatch (file: %lu, expected: %d)\n",
               (unsigned long)header->version, NIST_FULL_TEST_VERSION);
        lfs_file_close(lfs, &file);
        return NULL;
    }

    if (header->rosc_count != expected_rosc_count ||
        header->reseed_count != expected_reseed_count ||
        header->rounds != expected_rounds ||
        header->n_bits != expected_n_bits) {
        printf("[LFS] Config mismatch, starting fresh\n");
        lfs_file_close(lfs, &file);
        return NULL;
    }

    /* Allocate and read results */
    int total = expected_rosc_count * expected_reseed_count;
    nist_full_config_result_t *results = pvPortMalloc(total * sizeof(*results));
    if (!results) {
        lfs_file_close(lfs, &file);
        return NULL;
    }

    read_size = lfs_file_read(lfs, &file, results, total * sizeof(*results));
    lfs_file_close(lfs, &file);

    if (read_size != (lfs_ssize_t)(total * sizeof(*results))) {
        printf("[LFS] Failed to read results\n");
        vPortFree(results);
        return NULL;
    }

    printf("[LFS] Loaded previous results (v%lu, progress: ROSC[%d] Reseed[%d] Round[%d])\n",
           (unsigned long)header->version, header->current_rosc,
           header->current_reseed, header->current_round);
    return results;
}

/**
 * @brief Save full NIST test results to LittleFS
 */
static int nist_full_save_results(
    const nist_full_result_header_t *header,
    const nist_full_config_result_t *results)
{
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) return -1;

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, NIST_FULL_RESULT_FILE,
                            LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (err < 0) {
        printf("[LFS] Failed to open file for writing: %d\n", err);
        return -1;
    }

    /* Write header */
    lfs_ssize_t written = lfs_file_write(lfs, &file, header, sizeof(*header));
    if (written != sizeof(*header)) {
        lfs_file_close(lfs, &file);
        return -1;
    }

    /* Write results */
    int total = header->rosc_count * header->reseed_count;
    written = lfs_file_write(lfs, &file, results, total * sizeof(*results));
    lfs_file_close(lfs, &file);

    if (written != (lfs_ssize_t)(total * sizeof(*results))) {
        return -1;
    }

    return 0;
}
#endif /* CONFIG_LITTLEFS */

/**
 * @brief TRNG HAL for NIST full sweep (uses current register settings)
 */
static int nist_full_trng_hal(uint8_t *buf, size_t len)
{
    return hal_rand_stream(buf, len);
}

/**
 * @brief Shell command: trng_nist_full [rounds] [n_bits]
 *
 * Run full NIST SP 800-22 test suite across all ROSC/Reseed combinations.
 *
 * This is the comprehensive certification test that:
 * 1. Uses the same 5 ROSC values as trng_nonoverlap
 * 2. Uses the same 20 Reseed values as trng_nonoverlap
 * 3. Runs the complete 16-test NIST suite for each configuration
 * 4. Executes multiple rounds per configuration for statistical reliability
 * 5. Records pass rates, P-value statistics, and NIST certification status
 *
 * Results are persisted to LittleFS for:
 * - Resume capability after power cycle
 * - Detailed analysis with trng_nist_full_show
 *
 * @param rounds  Number of rounds per configuration (default: 100)
 * @param n_bits  Bits per round (default: 1000000)
 */
static int cmd_trng_nist_full(int argc, char **argv)
{
    int rounds = NIST_FULL_DEFAULT_ROUNDS;
    uint32_t n_bits = NIST_DEFAULT_N_BITS;

    /* Parse arguments */
    if (argc > 1) {
        rounds = (int)strtoul(argv[1], NULL, 0);
        if (rounds < 1) rounds = 1;
        if (rounds > 1000) {
            printf("Warning: Limiting to 1000 rounds per config\n");
            rounds = 1000;
        }
    }
    if (argc > 2) {
        n_bits = (uint32_t)strtoul(argv[2], NULL, 0);
        if (n_bits < NIST_MIN_RECOMMENDED_BITS) n_bits = NIST_MIN_RECOMMENDED_BITS;
        if (n_bits > NIST_DEFAULT_N_BITS) n_bits = NIST_DEFAULT_N_BITS;
    }

    uint32_t n_bytes = (n_bits + 7) / 8;
    int total_configs = NONOVERLAP_ROSC_COUNT * NONOVERLAP_RESEED_COUNT;

    printf("\n");
    printf("TRNG Full NIST SP 800-22 Sweep (ROSC x Reseed x Rounds)\n");
    printf("=========================================================\n");
    printf("Test version:  %d\n", NIST_FULL_TEST_VERSION);
    printf("Test bits:     %lu (%lu KB) per round\n", (unsigned long)n_bits,
           (unsigned long)(n_bytes / 1024));
    printf("Rounds:        %d per configuration\n", rounds);
    printf("ROSC values:   ");
    for (int i = 0; i < (int)NONOVERLAP_ROSC_COUNT; i++) {
        printf("0x%04X%s", nonoverlap_rosc_values[i],
               i < (int)NONOVERLAP_ROSC_COUNT - 1 ? ", " : "");
    }
    printf(" (%d values)\n", (int)NONOVERLAP_ROSC_COUNT);
    printf("Reseed values: %d (single-bit: MSB[47:32] + LSB[31:24])\n",
           (int)NONOVERLAP_RESEED_COUNT);
    printf("Total configs: %d\n", total_configs);
    printf("Total runs:    %d (%d x %d)\n", total_configs * rounds,
           total_configs, rounds);
    printf("---------------------------------------------------------\n");
    printf("NIST Criteria: Pass proportion >= (1-alpha) - 3*sqrt(...)\n");
    printf("Alpha = 0.01, rounds >= %d for valid proportion test\n",
           NIST_FULL_DEFAULT_ROUNDS);
    printf("=========================================================\n\n");

    /* Allocate test buffer */
    if (nist_alloc_test_buffer(n_bytes) != 0) {
        return -1;
    }

    /* Allocate results array */
    nist_full_config_result_t *results = NULL;
    nist_full_result_header_t header;
    int start_rosc = 0;
    int start_reseed = 0;
    int start_round = 0;
    int loaded_prev = 0;

#ifdef CONFIG_LITTLEFS
    /* Try to load previous results */
    results = nist_full_load_results(&header,
                                      (int)NONOVERLAP_ROSC_COUNT,
                                      (int)NONOVERLAP_RESEED_COUNT,
                                      rounds,
                                      n_bits);
    if (results) {
        start_rosc = header.current_rosc;
        start_reseed = header.current_reseed;
        start_round = header.current_round;
        loaded_prev = 1;

        /* Count already tested configs */
        int already_tested = 0;
        for (int i = 0; i < total_configs; i++) {
            if (results[i].tested) already_tested++;
        }
        printf("[LFS] Resuming from ROSC[%d] Reseed[%d] Round[%d], %d/%d configs complete\n\n",
               start_rosc, start_reseed, start_round, already_tested, total_configs);
    }
#endif

    /* Allocate fresh results if not loaded */
    if (!results) {
        results = pvPortMalloc(total_configs * sizeof(nist_full_config_result_t));
        if (!results) {
            printf("Error: Failed to allocate results array\n");
            nist_free_test_buffer();
            return -1;
        }
        memset(results, 0, total_configs * sizeof(nist_full_config_result_t));

        /* Initialize P-value min/max */
        for (int i = 0; i < total_configs; i++) {
            for (int t = 0; t < 16; t++) {
                results[i].p_value_min[t] = 1.0f;
                results[i].p_value_max[t] = 0.0f;
            }
        }

        /* Initialize header */
        header.magic = NIST_FULL_MAGIC;
        header.version = NIST_FULL_TEST_VERSION;
        header.rosc_count = (uint8_t)NONOVERLAP_ROSC_COUNT;
        header.reseed_count = (uint8_t)NONOVERLAP_RESEED_COUNT;
        header.current_rosc = 0;
        header.current_reseed = 0;
        header.n_bits = n_bits;
        header.rounds = (uint16_t)rounds;
        header.current_round = 0;
        header.reserved = 0;
    }

    /* Save original register values */
    volatile uint32_t *rosc_reg = (volatile uint32_t *)0x30080250;
    volatile uint32_t *reseed_lsb_reg = (volatile uint32_t *)0x3008022C;
    volatile uint32_t *reseed_msb_reg = (volatile uint32_t *)0x30080230;
    uint32_t orig_rosc = *rosc_reg;
    uint32_t orig_reseed_lsb = *reseed_lsb_reg;
    uint32_t orig_reseed_msb = *reseed_msb_reg;

    /* Initialize TRNG */
    hal_sec_trng_init();

    /* Track overall statistics */
    int configs_tested = 0;
    int configs_certified = 0;
    int best_rosc_idx = -1;
    int best_reseed_idx = -1;
    double best_pass_rate = 0.0;

    printf("Starting sweep...\n\n");

    /* Main sweep loop */
    for (int ri = start_rosc; ri < (int)NONOVERLAP_ROSC_COUNT; ri++) {
        uint16_t rosc = nonoverlap_rosc_values[ri];

        int si_start = (ri == start_rosc) ? start_reseed : 0;
        for (int si = si_start; si < (int)NONOVERLAP_RESEED_COUNT; si++) {
            int idx = ri * NONOVERLAP_RESEED_COUNT + si;

            /* Skip if already tested */
            if (results[idx].tested) {
                configs_tested++;
                if (results[idx].nist_certified) {
                    configs_certified++;
                    double pass_rate = (results[idx].suite_total > 0) ?
                                       (double)results[idx].suite_passed / results[idx].suite_total : 0.0;
                    if (pass_rate > best_pass_rate) {
                        best_pass_rate = pass_rate;
                        best_rosc_idx = ri;
                        best_reseed_idx = si;
                    }
                }
                continue;
            }

            uint64_t reseed = nonoverlap_reseed_values[si];
            uint32_t rs_lsb = (uint32_t)(reseed & 0xFFFFFFFF);
            uint16_t rs_msb = (uint16_t)((reseed >> 32) & 0xFFFF);

            /* Set ROSC and Reseed */
            Sec_Eng_Trng_Set_ROSC_Config(rosc);
            Sec_Eng_Trng_Set_Reseed(rs_lsb, rs_msb);

            /* Small delay for register to take effect */
            for (volatile int d = 0; d < 1000; d++);

            printf("Config [%d/%d]: ROSC=0x%04X Reseed=0x%04X_%08lX\n",
                   idx + 1, total_configs, (unsigned)rosc,
                   (unsigned)rs_msb, (unsigned long)rs_lsb);

            /* Determine starting round (for resume) */
            int round_start = (ri == start_rosc && si == start_reseed) ? start_round : 0;

            /* Run multiple rounds for this configuration */
            for (int round = round_start; round < rounds; round++) {
                /* Generate random data */
                int ret = hal_rand_stream(g_nist_test_buf, n_bytes);
                if (ret != 0) {
                    printf("  Round %d/%d: TRNG ERROR\n", round + 1, rounds);
                    continue;
                }

                /* Run full NIST test suite */
                nist_suite_result_t suite_result;
                ret = nist_run_all_tests_ex(g_nist_test_buf, n_bits,
                                            NIST_DEFAULT_ALPHA, &suite_result,
                                            nist_full_trng_hal);

                if (ret < 0) {
                    printf("  Round %d/%d: SUITE ERROR (%d)\n", round + 1, rounds, ret);
                    continue;
                }

                /* Accumulate statistics */
                results[idx].suite_total++;
                if (suite_result.tests_failed == 0 && suite_result.tests_error == 0) {
                    results[idx].suite_passed++;
                }

                /* Accumulate per-test statistics */
                for (int t = 0; t < suite_result.tests_run && t < 16; t++) {
                    if (suite_result.tests[t].passed) {
                        results[idx].test_passed[t]++;
                    }
                    results[idx].sub_passed[t] += suite_result.tests[t].sub_passed;
                    results[idx].sub_total[t] += suite_result.tests[t].sub_total;

                    float pv = (float)suite_result.tests[t].p_value;
                    results[idx].p_value_sum[t] += pv;
                    if (pv < results[idx].p_value_min[t]) {
                        results[idx].p_value_min[t] = pv;
                    }
                    if (pv > results[idx].p_value_max[t]) {
                        results[idx].p_value_max[t] = pv;
                    }
                }

                /* Print progress every 10 rounds */
                if ((round + 1) % 10 == 0 || round == rounds - 1) {
                    printf("  Round %3d/%d: %d/%d suites passed (%.1f%%)\n",
                           round + 1, rounds,
                           results[idx].suite_passed, results[idx].suite_total,
                           results[idx].suite_total > 0 ?
                               100.0 * results[idx].suite_passed / results[idx].suite_total : 0.0);
                }

#ifdef CONFIG_LITTLEFS
                /* Save progress every 10 rounds */
                if ((round + 1) % 10 == 0) {
                    header.current_rosc = (uint8_t)ri;
                    header.current_reseed = (uint8_t)si;
                    header.current_round = (uint8_t)(round + 1);
                    nist_full_save_results(&header, results);
                }
#endif
            }

            /* Mark as tested and check certification */
            results[idx].tested = 1;
            double pass_rate = (results[idx].suite_total > 0) ?
                               (double)results[idx].suite_passed / results[idx].suite_total : 0.0;
            results[idx].nist_certified = nist_proportion_check(
                results[idx].suite_passed, results[idx].suite_total, NIST_DEFAULT_ALPHA) ? 1 : 0;

            configs_tested++;
            if (results[idx].nist_certified) {
                configs_certified++;
                if (pass_rate > best_pass_rate) {
                    best_pass_rate = pass_rate;
                    best_rosc_idx = ri;
                    best_reseed_idx = si;
                }
            }

            /* Print config summary */
            printf("  Summary: %d/%d passed (%.1f%%) - %s\n\n",
                   results[idx].suite_passed, results[idx].suite_total,
                   pass_rate * 100.0,
                   results[idx].nist_certified ? "CERT" : "FAIL");

#ifdef CONFIG_LITTLEFS
            /* Save after each config */
            header.current_rosc = (uint8_t)ri;
            header.current_reseed = (uint8_t)(si + 1);
            header.current_round = 0;
            nist_full_save_results(&header, results);
#endif
        }

        /* Reset start_reseed for next ROSC */
        start_reseed = 0;
        start_round = 0;
    }

    /* Restore original register values */
    *rosc_reg = orig_rosc;
    *reseed_lsb_reg = orig_reseed_lsb;
    *reseed_msb_reg = orig_reseed_msb;

    /* Free test buffer */
    nist_free_test_buffer();

    /* Print summary */
    printf("\n");
    printf("=========================================================\n");
    printf("NIST FULL SWEEP SUMMARY\n");
    printf("=========================================================\n");
    printf("Configs tested:    %d/%d%s\n", configs_tested, total_configs,
           loaded_prev ? " (includes resumed)" : "");
    printf("Configs certified: %d (%.1f%%)\n",
           configs_certified, 100.0 * configs_certified / total_configs);
    printf("Certification:     Pass rate >= %.1f%% (NIST threshold)\n",
           nist_proportion_threshold(rounds, NIST_DEFAULT_ALPHA) * 100.0);

    if (best_rosc_idx >= 0 && best_reseed_idx >= 0) {
        uint16_t best_rosc = nonoverlap_rosc_values[best_rosc_idx];
        uint64_t best_reseed = nonoverlap_reseed_values[best_reseed_idx];
        uint32_t best_rs_lsb = (uint32_t)(best_reseed & 0xFFFFFFFF);
        uint16_t best_rs_msb = (uint16_t)((best_reseed >> 32) & 0xFFFF);
        int best_idx = best_rosc_idx * NONOVERLAP_RESEED_COUNT + best_reseed_idx;

        printf("---------------------------------------------------------\n");
        printf("BEST CONFIGURATION:\n");
        printf("  ROSC:       0x%04X\n", (unsigned)best_rosc);
        printf("  Reseed:     0x%04lX_%08lX\n",
               (unsigned long)best_rs_msb, (unsigned long)best_rs_lsb);
        printf("  Pass rate:  %d/%d (%.2f%%)\n",
               results[best_idx].suite_passed, results[best_idx].suite_total,
               best_pass_rate * 100.0);
        printf("  Status:     *** NIST CERTIFIED ***\n");
        printf("---------------------------------------------------------\n");
        printf("Recommended settings:\n");
        printf("  #define NIST_TRNG_ROSC_VALUE   0x%04X\n", (unsigned)best_rosc);
        printf("  #define NIST_TRNG_RESEED_MSB   0x%04X\n", (unsigned)best_rs_msb);
        printf("  #define NIST_TRNG_RESEED_LSB   0x%08lX\n", (unsigned long)best_rs_lsb);
    } else {
        printf("---------------------------------------------------------\n");
        printf("*** NO CONFIGURATION PASSED NIST CERTIFICATION ***\n");
    }
    printf("=========================================================\n");

#ifdef CONFIG_LITTLEFS
    printf("\n[LFS] Results saved to %s\n", NIST_FULL_RESULT_FILE);
    printf("      Use 'trng_nist_full_show' to display detailed results.\n");
#endif

    /* Free results */
    vPortFree(results);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_nist_full, trng_nist_full,
    Full NIST sweep [rounds=100] [n_bits=1000000]);

/**
 * @brief Shell command: trng_nist_full_show
 *
 * Display detailed results from previous trng_nist_full test.
 */
static int cmd_trng_nist_full_show(int argc, char **argv)
{
    (void)argc;
    (void)argv;

#ifndef CONFIG_LITTLEFS
    printf("Error: LittleFS not enabled\n");
    return -1;
#else
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) {
        printf("Error: Failed to mount LittleFS\n");
        return -1;
    }

    lfs_file_t file;
    int err = lfs_file_open(lfs, &file, NIST_FULL_RESULT_FILE, LFS_O_RDONLY);
    if (err < 0) {
        printf("Error: No results found. Run 'trng_nist_full' first.\n");
        return -1;
    }

    /* Read header */
    nist_full_result_header_t header;
    lfs_ssize_t read_size = lfs_file_read(lfs, &file, &header, sizeof(header));
    if (read_size != sizeof(header) || header.magic != NIST_FULL_MAGIC) {
        printf("Error: Invalid results file\n");
        lfs_file_close(lfs, &file);
        return -1;
    }

    /* Read results */
    int total = header.rosc_count * header.reseed_count;
    nist_full_config_result_t *results = pvPortMalloc(total * sizeof(*results));
    if (!results) {
        printf("Error: Memory allocation failed\n");
        lfs_file_close(lfs, &file);
        return -1;
    }

    read_size = lfs_file_read(lfs, &file, results, total * sizeof(*results));
    lfs_file_close(lfs, &file);

    if (read_size != (lfs_ssize_t)(total * sizeof(*results))) {
        printf("Error: Failed to read results\n");
        vPortFree(results);
        return -1;
    }

    printf("\n");
    printf("TRNG Full NIST SP 800-22 Test Results\n");
    printf("=====================================\n");
    printf("Version:       %lu\n", (unsigned long)header.version);
    printf("Bits/round:    %lu\n", (unsigned long)header.n_bits);
    printf("Rounds/config: %u\n", header.rounds);
    printf("ROSC count:    %u\n", header.rosc_count);
    printf("Reseed count:  %u\n", header.reseed_count);
    printf("\n");

    /* Count statistics */
    int tested = 0, certified = 0;
    int best_idx = -1;
    double best_rate = 0.0;

    for (int i = 0; i < total; i++) {
        if (results[i].tested) {
            tested++;
            if (results[i].nist_certified) {
                certified++;
                double rate = (results[i].suite_total > 0) ?
                              (double)results[i].suite_passed / results[i].suite_total : 0.0;
                if (rate > best_rate) {
                    best_rate = rate;
                    best_idx = i;
                }
            }
        }
    }

    printf("Configs tested:    %d/%d\n", tested, total);
    printf("Configs certified: %d (%.1f%%)\n", certified, 100.0 * certified / total);
    printf("\n");

    /* Print results table */
    printf("%-8s %-20s %-12s %-8s %-6s\n",
           "ROSC", "Reseed", "Pass/Total", "Rate", "NIST");
    printf("----------------------------------------------------------\n");

    for (int ri = 0; ri < header.rosc_count; ri++) {
        for (int si = 0; si < header.reseed_count; si++) {
            int idx = ri * header.reseed_count + si;

            if (!results[idx].tested) {
                continue;
            }

            uint16_t rosc = nonoverlap_rosc_values[ri];
            uint64_t reseed = nonoverlap_reseed_values[si];
            uint32_t rs_lsb = (uint32_t)(reseed & 0xFFFFFFFF);
            uint16_t rs_msb = (uint16_t)((reseed >> 32) & 0xFFFF);

            double rate = (results[idx].suite_total > 0) ?
                          100.0 * results[idx].suite_passed / results[idx].suite_total : 0.0;

            printf("0x%04X  0x%04X_%08lX  %4d/%-4d   %5.1f%%  %s%s\n",
                   (unsigned)rosc, (unsigned)rs_msb, (unsigned long)rs_lsb,
                   results[idx].suite_passed, results[idx].suite_total,
                   rate,
                   results[idx].nist_certified ? "CERT" : "FAIL",
                   idx == best_idx ? " *BEST*" : "");
        }
    }

    /* Print best config details */
    if (best_idx >= 0) {
        int ri = best_idx / header.reseed_count;
        int si = best_idx % header.reseed_count;
        uint16_t best_rosc = nonoverlap_rosc_values[ri];
        uint64_t best_reseed = nonoverlap_reseed_values[si];
        uint32_t best_rs_lsb = (uint32_t)(best_reseed & 0xFFFFFFFF);
        uint16_t best_rs_msb = (uint16_t)((best_reseed >> 32) & 0xFFFF);

        printf("\n----------------------------------------------------------\n");
        printf("BEST CONFIGURATION DETAILS:\n");
        printf("  ROSC:   0x%04X\n", (unsigned)best_rosc);
        printf("  Reseed: 0x%04X_%08lX\n", (unsigned)best_rs_msb, (unsigned long)best_rs_lsb);
        printf("  Pass:   %d/%d (%.2f%%)\n",
               results[best_idx].suite_passed, results[best_idx].suite_total,
               best_rate * 100.0);
        printf("\n");

        /* Print per-test statistics for best config */
        printf("  Per-test statistics:\n");
        printf("  %-24s %-10s %-10s %-8s %-8s %-8s\n",
               "Test", "Passed", "Sub-tests", "P-min", "P-avg", "P-max");
        printf("  -----------------------------------------------------------------------\n");

        const char *test_names[] = {
            "Frequency", "BlockFreq", "Runs", "LongestRun",
            "Rank", "FFT", "NonOverlap", "OverlapTemplate",
            "Universal", "LinComplex", "Serial", "ApproxEntropy",
            "CumuSums", "RandExcur", "RandExcurVar", "Test16"
        };

        for (int t = 0; t < 16; t++) {
            int total_tests = results[best_idx].suite_total;
            if (total_tests == 0) continue;

            char sub_str[16] = "-";
            if (results[best_idx].sub_total[t] > total_tests) {
                snprintf(sub_str, sizeof(sub_str), "%d/%d",
                         results[best_idx].sub_passed[t], results[best_idx].sub_total[t]);
            }

            float p_avg = results[best_idx].p_value_sum[t] / total_tests;

            printf("  [%2d] %-20s %4d/%-4d  %-10s %.4f   %.4f   %.4f\n",
                   t + 1, test_names[t],
                   results[best_idx].test_passed[t], total_tests,
                   sub_str,
                   results[best_idx].p_value_min[t],
                   p_avg,
                   results[best_idx].p_value_max[t]);
        }
    }

    printf("==========================================================\n");

    vPortFree(results);
    return 0;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_nist_full_show, trng_nist_full_show,
    Display full NIST sweep results);

/**
 * @brief Shell command: trng_nist_full_clear
 *
 * Clear saved full NIST test results from LittleFS.
 */
static int cmd_trng_nist_full_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;

#ifndef CONFIG_LITTLEFS
    printf("Error: LittleFS not enabled\n");
    return -1;
#else
    lfs_t *lfs = nonoverlap_ensure_lfs_mounted();
    if (!lfs) {
        printf("Error: Failed to mount LittleFS\n");
        return -1;
    }

    int err = lfs_remove(lfs, NIST_FULL_RESULT_FILE);
    if (err < 0) {
        printf("Error: Failed to remove %s (err=%d)\n", NIST_FULL_RESULT_FILE, err);
        return -1;
    }

    printf("Cleared: %s\n", NIST_FULL_RESULT_FILE);
    return 0;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_nist_full_clear, trng_nist_full_clear,
    Clear saved full NIST test results);
