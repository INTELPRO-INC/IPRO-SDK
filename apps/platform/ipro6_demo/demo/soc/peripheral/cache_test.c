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

#include <generated/autoconf.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#if defined(IPRO7)
#include "ipro7.h"
#elif defined(IPRO6)
#include "ipro6.h"
#endif
#include "hal_mtimer.h"
#include "system_soc.h"
#include "shell.h"

#ifdef CONFIG_DEMO_CACHE_TEST

#define FILLER1 asm("nop")
#define FILLER2 FILLER1;FILLER1;FILLER1;FILLER1;FILLER1;FILLER1;FILLER1
#define FILLER3 FILLER2;FILLER2;FILLER2;FILLER2;FILLER2;FILLER2;FILLER2
#define FILLER4 FILLER3;FILLER3;FILLER3;FILLER3;FILLER3;FILLER3;FILLER3
#define FILLER5 FILLER4;FILLER4;FILLER4;FILLER4;FILLER4;FILLER4;FILLER4

#define NUMPRIMES 50
#define CNT 100
int primes_src[10000];
int *primes;

void findPrimes() {
	int primeIndex;
	int candidatePrime;
	int divisorIndex;

	for (primeIndex=0; primeIndex<NUMPRIMES; primeIndex++) {
	    // Add nop filler, so that findPrimes() will not fit within the cache.
		// This is necessary to demonstrate that changing the cache size will affect the cycle count
		FILLER4;

	    for (candidatePrime=2; 1; candidatePrime++) {
	    	for (divisorIndex=0; divisorIndex<primeIndex; divisorIndex++) {
	    		if ((candidatePrime - primes[divisorIndex])==0) {
	    		break;
	    		}
	    	}
	    	if (divisorIndex==primeIndex) {
	    		primes[primeIndex] = candidatePrime;
	    		break;
	    	}
	    }
	}
	for (primeIndex=0; primeIndex<NUMPRIMES; primeIndex++) {
		    // Add nop filler, so that findPrimes() will not fit within the cache.
			// This is necessary to demonstrate that changing the cache size will affect the cycle count
			FILLER4;

		    for (candidatePrime=2; 1; candidatePrime++) {
		    	for (divisorIndex=0; divisorIndex<primeIndex; divisorIndex++) {
		    		if ((candidatePrime - primes[divisorIndex])==0) {
		    		break;
		    		}
		    	}
		    	if (divisorIndex==primeIndex) {
		    		primes[primeIndex] = candidatePrime;
		    		break;
		    	}
		    }
		}
	for (primeIndex=0; primeIndex<NUMPRIMES; primeIndex++) {
		    // Add nop filler, so that findPrimes() will not fit within the cache.
			// This is necessary to demonstrate that changing the cache size will affect the cycle count
			FILLER4;

		    for (candidatePrime=2; 1; candidatePrime++) {
		    	for (divisorIndex=0; divisorIndex<primeIndex; divisorIndex++) {
		    		if ((candidatePrime - primes[divisorIndex])==0) {
		    		break;
		    		}
		    	}
		    	if (divisorIndex==primeIndex) {
		    		primes[primeIndex] = candidatePrime;
		    		break;
		    	}
		    }
		}
	for (primeIndex=0; primeIndex<3000; primeIndex++) {
		//printf("%d ",primes[primeIndex]);
		primes[primeIndex] = primes[primeIndex] +1;
	}

	//printf("\n");
}

static void cache_test(void)
{
#ifndef CONFIG_DCACHE_DISABLE
	int counter = 0;
	uint32_t total_cycle;
	uint32_t start_cycle;

    //primes = (int *)((uint32_t)primes_src | 0x20000000);
    printf("Using array at %p\r\n", primes);
    MFlushInvalDCache();
    DisableICache();
    DisableDCache();
	counter = 0;
    printf("Icache Disable, Dcache Disable: ");
    start_cycle = mtimer_get_time_ms();
	while (counter++ < CNT)
	{
//		start_cycle = mtimer_get_time_ms();
		findPrimes();
//		total_cycle = mtimer_get_time_ms() - start_cycle;
//		printf("run function findPrimes count:%d takes cycles = %d\n", counter, total_cycle);
	}
    total_cycle = mtimer_get_time_ms() - start_cycle;
    printf("run function findPrimes count:%d takes %lu ms\r\n", counter, total_cycle);

    DisableICache();
    EnableDCache();
    printf("Icache Disable, Dcache Enable: ");

    counter = 0;
    start_cycle = mtimer_get_time_ms();
    while (counter++ < CNT)
    {
//      start_cycle = mtimer_get_time_ms();
        findPrimes();
//      total_cycle = mtimer_get_time_ms() - start_cycle;
//      printf("run function findPrimes count:%d takes cycles = %d\n", counter, total_cycle);
    }
    total_cycle = mtimer_get_time_ms() - start_cycle;
    printf("run function findPrimes count:%d takes %lu ms\r\n", counter, total_cycle);

    EnableICache();
    DisableDCache();
    printf("Icache Enable, Dcache Disable: ");
	counter = 0;
    start_cycle = mtimer_get_time_ms();
	while (counter++ < CNT)
	{
//		start_cycle = mtimer_get_time_ms();
		findPrimes();
//		total_cycle = mtimer_get_time_ms() - start_cycle;
//		printf("run function findPrimes count:%d takes cycles = %d\n", counter, total_cycle);
	}
    total_cycle = mtimer_get_time_ms() - start_cycle;
    printf("run function findPrimes count:%d takes %lu ms\r\n", counter, total_cycle);

    EnableICache();
    EnableDCache();
    printf("Icache Enable, Dcache Enable: ");
    counter = 0;
    start_cycle = mtimer_get_time_ms();
    while (counter++ < CNT)
    {
        findPrimes();
    }
    total_cycle = mtimer_get_time_ms() - start_cycle;
    printf("run function findPrimes count:%d takes %lu ms\r\n", counter, total_cycle);
#else
    printf("cache disabled\r\n");
#endif
}

static int _is_cache_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    if ((a & 0xF0000000UL) == 0x10000000UL)
        return 1;
    else
        return 0;
}

void *_get_noncache_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    if (!_is_cache_addr(addr)) {
        return (void *)addr;
    }

    return (void *)((a & ~0xF0000000UL) | 0x20000000UL);
}

static void dcache_test(void)
{
    int pass = 0;
    int test_size = 32*1024 / 4;
    primes = (int *)malloc(test_size * sizeof(int));

    int *primes_noncache = _get_noncache_addr(primes);

    printf("init cacheable addr\r\n");
    for (int i = 0; i < test_size; i++)
        primes[i] = i;

    printf("compare cacheable and non-cacheable addr\r\n");
    pass = 0;
    for (int i = 0; i < test_size; i++) {
        if (primes[i] != primes_noncache[i]) {
            printf("value differ at %p / %p, %d %d\r\n", &primes[i], &primes_noncache[i], primes[i], primes_noncache[i]);
            pass = 1;
            break;
        }
    }
    if (pass)
        printf("Test Pass\r\n");
    else
        printf("Test Fail\r\n");

    printf("cache clean\r\n");
    L1C_DCACHE_CLEAN_RANGE((unsigned long)primes, test_size*4);
    printf("compare cacheable and non-cacheable addr\r\n");
    pass = 1;
    for (int i = 0; i < test_size; i++) {
        if (primes[i] != primes_noncache[i]) {
            printf("value differ at %p / %p, %d %d\r\n", &primes[i], &primes_noncache[i], primes[i], primes_noncache[i]);
            pass = 0;
            break;
        }
    }
    if (pass)
        printf("Test Pass\r\n");
    else
        printf("Test Fail\r\n");


    printf("init noncacheable addr\r\n");
    for (int i = 0; i < test_size; i++)
        primes_noncache[i] = i + 2;

    printf("compare cacheable and non-cacheable addr\r\n");
    pass = 0;
    for (int i = 0; i < test_size; i++) {
        if (primes[i] != primes_noncache[i]) {
            printf("value differ at %p / %p, %d %d\r\n", &primes[i], &primes_noncache[i], primes[i], primes_noncache[i]);
            pass = 1;
            break;
        }
    }
    if (pass)
        printf("Test Pass\r\n");
    else
        printf("Test Fail\r\n");

    printf("cache invalid\r\n");
    L1C_DCACHE_INVALID_RANGE((unsigned long)primes, test_size*4);
    printf("compare cacheable and non-cacheable addr\r\n");
    pass = 1;
    for (int i = 0; i < test_size; i++) {
        if (primes[i] != primes_noncache[i]) {
            printf("value differ at %p / %p, %d %d\r\n", &primes[i], &primes_noncache[i], primes[i], primes_noncache[i]);
            pass = 0;
            break;
        }
    }

    if (pass)
        printf("Test Pass\r\n");
    else
        printf("Test Fail\r\n");

}

int cmd_cache_test(int argc, char **argv)
{
    primes = primes_src;
    cache_test();
    dcache_test();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_cache_test, cache_test, Cache test);

#else /* !CONFIG_DEMO_CACHE_TEST */

#include "shell.h"

int cmd_cache_test(int argc, char **argv)
{
    printf("cache test disabled (CONFIG_DEMO_CACHE_TEST=n)\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_cache_test, cache_test, Cache test);

#endif /* CONFIG_DEMO_CACHE_TEST */
