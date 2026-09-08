#include "common.h"
#include "system_soc.h"

#include "hal_sec_trng.h"
#include "hal_mtimer.h"

#if defined(MBEDTLS_ENTROPY_C)

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    uintptr_t flag;
    flag = __irq_save();
    hal_rand_stream( output, len );
    //memset(output, 0, len); /* test for ecp */
    *olen = len;
    __irq_restore(flag);
    return( 0 );
}
#endif

// Platform entropy source (used when MBEDTLS_ENTROPY_HARDWARE_ALT is not defined)
int mbedtls_platform_entropy_poll(void *data, unsigned char *output, 
                                  size_t len, size_t *olen)
{
    (void)data;
    
    // Use hardware TRNG
    uintptr_t flag = __irq_save();
    hal_rand_stream(output, len);
    __irq_restore(flag);
    
    *olen = len;
    return 0;
}

// Hardclock-based entropy source (weak entropy, use with caution)
// Weak definition - can be overridden by other implementations (e.g., crypto_mbedtls.c)
__attribute__((weak))
int mbedtls_hardclock_poll(void *data, unsigned char *output,
                           size_t len, size_t *olen)
{
    (void)data;
    
    // Mix hardware timer with existing entropy
    // This provides weak entropy and should not be sole entropy source
    uint64_t timer = mtimer_get_time_us();  // Get microsecond timer
    
    for (size_t i = 0; i < len; i++) {
        // Mix timer value (rotated by byte position) with output
        output[i] ^= (unsigned char)((timer >> ((i % 8) * 8)) & 0xFF);
        timer = (timer * 1103515245 + 12345);  // Simple LCG for variation
    }
    
    *olen = len;
    return 0;
}

#endif /* MBEDTLS_ENTROPY_C */
