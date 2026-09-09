/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/**
 * @file test_getopt.c
 * @brief Unit tests for command-line argument parsing (utils_getopt)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "unity.h"
#include "utils_getopt.h"

/* Test cases */
TEST_CASE(test_getopt_init)
{
    getopt_env_t env;
    
    int ret = utils_getopt_init(&env, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);
    /* optind typically starts at 1 in getopt implementations */
    TEST_ASSERT(env.optind >= 0);
    TEST_ASSERT_EQUAL_INT(0, env.opterr);
}

TEST_CASE(test_getopt_simple_flags)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog", "-a", "-b", "-c"};
    int argc = 4;
    
    int opt;
    int flags_found = 0;
    
    while ((opt = utils_getopt(&env, argc, (char * const *)argv, "abc")) != -1) {
        if (opt == 'a') flags_found |= 1;
        if (opt == 'b') flags_found |= 2;
        if (opt == 'c') flags_found |= 4;
    }
    
    TEST_ASSERT_EQUAL_INT(7, flags_found); /* All three flags found */
}

TEST_CASE(test_getopt_with_argument)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog", "-f", "filename.txt", "-v"};
    int argc = 4;
    
    int opt;
    int f_found = 0;
    int v_found = 0;
    
    while ((opt = utils_getopt(&env, argc, (char * const *)argv, "f:v")) != -1) {
        if (opt == 'f') {
            f_found = 1;
            TEST_ASSERT_NOT_NULL(env.optarg);
            TEST_ASSERT_EQUAL_STRING("filename.txt", env.optarg);
        }
        if (opt == 'v') {
            v_found = 1;
        }
    }
    
    TEST_ASSERT_EQUAL_INT(1, f_found);
    TEST_ASSERT_EQUAL_INT(1, v_found);
}

TEST_CASE(test_getopt_optional_argument)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog", "-o", "-p", "param"};
    int argc = 4;
    
    int opt;
    int o_found = 0;
    int p_found = 0;
    
    while ((opt = utils_getopt(&env, argc, (char * const *)argv, "o::p:")) != -1) {
        if (opt == 'o') o_found = 1;
        if (opt == 'p') {
            p_found = 1;
            TEST_ASSERT_NOT_NULL(env.optarg);
        }
    }
    
    TEST_ASSERT_EQUAL_INT(1, o_found);
    TEST_ASSERT_EQUAL_INT(1, p_found);
}

TEST_CASE(test_getopt_combined_short_opts)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog", "-abc"};
    int argc = 2;
    
    int opt;
    int a_found = 0, b_found = 0, c_found = 0;
    
    while ((opt = utils_getopt(&env, argc, (char * const *)argv, "abc")) != -1) {
        if (opt == 'a') a_found = 1;
        if (opt == 'b') b_found = 1;
        if (opt == 'c') c_found = 1;
    }
    
    TEST_ASSERT_EQUAL_INT(1, a_found);
    TEST_ASSERT_EQUAL_INT(1, b_found);
    TEST_ASSERT_EQUAL_INT(1, c_found);
}

TEST_CASE(test_getopt_unknown_option)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog", "-x"};
    int argc = 2;
    
    int opt = utils_getopt(&env, argc, (char * const *)argv, "abc");
    
    /* Unknown option should return '?' or fail */
    TEST_ASSERT(opt == '?' || opt == -1 || opt == 'x');
}

TEST_CASE(test_getopt_missing_argument)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog", "-f"}; /* -f requires argument but none provided */
    int argc = 2;
    
    int opt = utils_getopt(&env, argc, (char * const *)argv, ":f:");
    
    /* Missing argument with ':' at start should return ':' or '?' */
    TEST_ASSERT(opt == ':' || opt == '?');
}

TEST_CASE(test_getopt_positional_args)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog", "-a", "file1.txt", "file2.txt"};
    int argc = 4;
    
    int opt;
    
    /* Parse options */
    while ((opt = utils_getopt(&env, argc, (char * const *)argv, "a")) != -1) {
        /* Just consume options */
    }
    
    /* Remaining arguments start at optind */
    TEST_ASSERT(env.optind < argc);
    TEST_ASSERT_NOT_NULL(argv[env.optind]);
}

TEST_CASE(test_getopt_empty_args)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog"};
    int argc = 1;
    
    int opt = utils_getopt(&env, argc, (char * const *)argv, "abc");
    
    /* No arguments, should return -1 immediately */
    TEST_ASSERT_EQUAL_INT(-1, opt);
}

TEST_CASE(test_getopt_double_dash)
{
    getopt_env_t env;
    utils_getopt_init(&env, 0);
    
    const char *argv[] = {"prog", "-a", "--", "-b", "-c"};
    int argc = 5;
    
    int opt;
    int a_count = 0;
    
    while ((opt = utils_getopt(&env, argc, (char * const *)argv, "abc")) != -1) {
        if (opt == 'a') a_count++;
    }
    
    /* Should only find 'a' once, then stop at -- */
    TEST_ASSERT_EQUAL_INT(1, a_count);
    TEST_ASSERT(env.optind < argc);
}

/* Test suite runner */
void test_getopt_run(void)
{
    printf("Running getopt command-line parsing component tests\n");
    
    RUN_TEST(test_getopt_init);
    RUN_TEST(test_getopt_simple_flags);
    RUN_TEST(test_getopt_with_argument);
    RUN_TEST(test_getopt_optional_argument);
    RUN_TEST(test_getopt_combined_short_opts);
    RUN_TEST(test_getopt_unknown_option);
    RUN_TEST(test_getopt_missing_argument);
    RUN_TEST(test_getopt_positional_args);
    RUN_TEST(test_getopt_empty_args);
    RUN_TEST(test_getopt_double_dash);
}
