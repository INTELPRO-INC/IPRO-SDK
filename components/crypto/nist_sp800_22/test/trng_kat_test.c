/**
 * @file trng_kat_test.c
 * @brief NIST SP 800-22 Known-Answer Test against the sts-2.1.2 reference
 *
 * Runs the full suite on a fixed input (first 2^20 bits of e's binary
 * expansion) and compares every P-value against golden values produced by
 * the official NIST sts-2.1.2 reference implementation on the same bits.
 *
 * This validates the TEST IMPLEMENTATION itself, independent of TRNG
 * quality: statistical self-checks (running a good PRNG through the suite)
 * cannot detect a test whose decision statistic has silently lost its
 * discriminating power, a fixed-input KAT can.
 *
 * Usage: trng_kat
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>
#include <shell.h>

#include "nist_sp800_22.h"
#include "nist_kat_data.h"

static int cmd_trng_kat(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    nist_suite_result_t result;
    uint32_t n_bits = nist_kat_n_bits;
    uint32_t n_bytes = n_bits / 8;

    printf("\n");
    printf("NIST SP 800-22 KNOWN-ANSWER TEST\n");
    printf("================================\n");
    printf("Input:     first %lu bits of e (binary expansion)\n",
           (unsigned long)n_bits);
    printf("Reference: NIST sts-2.1.2 on identical bits\n");
#ifdef CONFIG_NIST_FULL_TEST
    printf("Templates: full (148 official m=9)\n");
#else
    printf("Templates: quick (10-of-148 subset)\n");
#endif
    printf("\n");

    /* Copy from flash rodata to PSRAM: the suite reads every bit many times */
    uint8_t *data = (uint8_t *)pvPortMalloc(n_bytes);
    if (!data) {
        printf("Error: no memory for KAT buffer\n");
        return -1;
    }
    memcpy(data, nist_kat_e_bits, n_bytes);

    uint32_t t0 = (uint32_t)xTaskGetTickCount();
    int ret = nist_run_all_tests(data, n_bits, NIST_DEFAULT_ALPHA, &result);
    uint32_t t1 = (uint32_t)xTaskGetTickCount();

    vPortFree(data);

    if (ret < 0) {
        printf("Error: suite failed to run (%d)\n", ret);
        return -1;
    }

    printf("\n");
    printf("======================================================================\n");
    printf("KAT COMPARISON (tolerance per test)\n");
    printf("======================================================================\n");
    printf("%-26s %10s %10s %10s  %s\n",
           "Test Name", "Expected", "Actual", "Delta", "Verdict");
    printf("----------------------------------------------------------------------\n");

    int mismatches = 0;
    int compared = 0;

    for (int i = 0; i < result.tests_run && i < 16; i++) {
        const nist_kat_expect_t *e = &nist_kat_expect[i];
        double expected = e->p1;

        /* Non-overlapping template: golden depends on template set */
        if (expected < 0) {
#ifdef CONFIG_NIST_FULL_TEST
            expected = nist_kat_nonoverlap_full_minp;
#else
            expected = nist_kat_nonoverlap_quick_minp;
#endif
        }

        if (result.tests[i].result != NIST_SUCCESS &&
            result.tests[i].result != NIST_FAILURE) {
            /* The e-expansion stream satisfies every precondition (J >= 500),
             * so any N/A or error here is itself a KAT failure. */
            printf("%-26s %10.6f %10s %10s  MISMATCH (result=%d)\n",
                   e->name, expected, "---", "---", result.tests[i].result);
            mismatches++;
            continue;
        }

        double delta = fabs(result.tests[i].p_value - expected);
        int ok = (delta <= e->tol);
        compared++;
        if (!ok) mismatches++;
        printf("%-26s %10.6f %10.6f %10.6f  %s\n",
               e->name, expected, result.tests[i].p_value, delta,
               ok ? "OK" : "MISMATCH");

        if (e->p2 >= 0) {
            double delta2 = fabs(result.tests[i].p_value2 - e->p2);
            int ok2 = (delta2 <= e->tol);
            compared++;
            if (!ok2) mismatches++;
            printf("%-26s %10.6f %10.6f %10.6f  %s\n",
                   "  (secondary)", e->p2, result.tests[i].p_value2, delta2,
                   ok2 ? "OK" : "MISMATCH");
        }
    }

    printf("----------------------------------------------------------------------\n");
    printf("Compared: %d P-values, mismatches: %d, time: %lu ms\n",
           compared, mismatches,
           (unsigned long)((t1 - t0) * portTICK_PERIOD_MS));
    if (mismatches == 0) {
        printf("*** KAT PASSED - implementation matches NIST sts-2.1.2 ***\n");
    } else {
        printf("*** KAT FAILED - implementation deviates from reference ***\n");
    }
    printf("======================================================================\n");

    return (mismatches == 0) ? 0 : -1;
}

SHELL_CMD_EXPORT_ALIAS(cmd_trng_kat, trng_kat,
    NIST SP 800-22 known-answer test vs sts-2.1.2 reference);
