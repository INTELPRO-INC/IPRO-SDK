#include "sec_mutex.h"

#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include "semphr.h"
#include "hal_sys.h"  /* For hal_clock_enable */
#include "drv_sec_eng.h"  /* For Sec_Eng_SHA_Enable_Link */
#include <stdio.h>  /* For printf debug output */

static inline void enable_sha_link_mode(void)
{
    /* Enable SHA Link Mode for hardware acceleration (like IntelPro reference) */
    Sec_Eng_SHA_Enable_Link(SEC_ENG_SHA_ID0);
}

/* These HW-crypto mutex backing stores MUST survive PDS31 deep sleep:
 * they are persistent sync objects taken on every AES/SHA/PKA op (the
 * SED fast-poll CCM path takes aes_mutex every poll). Pin them to
 * .ocram_bss so that on LP builds with CONFIG_OCRAM_NORET_SPLIT they land
 * in the HW-retained 64 KB region (banks 0-3) instead of the power-gated
 * ocram_noret region — otherwise a retention bit-flip in the semaphore's
 * uxItemSize trips configASSERT in xQueueSemaphoreTake (queue.c:1670)
 * after enough PDS cycles. On non-split builds .ocram_bss is just regular
 * OCRAM, so this pin is harmless there. */
#define SEC_RETAIN __attribute__((section(".ocram_bss")))
static SEC_RETAIN StaticSemaphore_t aes_mutex_buf;
static SemaphoreHandle_t aes_mutex = NULL;

static SEC_RETAIN StaticSemaphore_t sha_mutex_buf;
static SemaphoreHandle_t sha_mutex = NULL;

static SEC_RETAIN StaticSemaphore_t pka_mutex_buf;
static SemaphoreHandle_t pka_mutex = NULL;

static volatile int mutex_initialized = 0;

void ipro_sec_mutex_init(void)
{
    if (mutex_initialized) {
        return;
    }

    /* Enable SEC_ENG clock before any crypto operations */
    hal_clock_enable(HAL_MODULE_SEC_ENG);
    
    /* Request SHA access permission (like IntelPro reference implementation) */
    // int ret = Sec_Eng_Group0_Request_SHA_Access();
    // printf("[SEC_MUTEX] SHA access request: %s\n", (ret == SUCCESS) ? "SUCCESS" : "FAILED");
    // if (ret != SUCCESS) {
    //     printf("[SEC_MUTEX] WARNING: SHA access request failed! HW may not work correctly.\n");
    // }
    
    // /* Enable SHA Link Mode (required for IPRO7 hardware) */
    // enable_sha_link_mode();
    // printf("[SEC_MUTEX] SHA Link Mode enabled\n");
    
    aes_mutex = xSemaphoreCreateMutexStatic(&aes_mutex_buf);
    sha_mutex = xSemaphoreCreateMutexStatic(&sha_mutex_buf);
    pka_mutex = xSemaphoreCreateMutexStatic(&pka_mutex_buf);
    mutex_initialized = 1;
}

int ipro_sec_aes_mutex_take(void)
{
    if (!mutex_initialized) {
        ipro_sec_mutex_init();
    }
    if (pdPASS != xSemaphoreTake(aes_mutex, portMAX_DELAY)) {
        return -1;
    }
    return 0;
}

int ipro_sec_aes_mutex_give(void)
{
    if (pdPASS != xSemaphoreGive(aes_mutex)) {
        return -1;
    }
    return 0;
}

int ipro_sec_sha_mutex_take(void)
{
    if (!mutex_initialized) {
        ipro_sec_mutex_init();
    }
    if (pdPASS != xSemaphoreTake(sha_mutex, portMAX_DELAY)) {
        return -1;
    }
    
    /* NOTE: Don't request SHA access here!
     * Access should be requested once at init and maintained.
     * The hardware Sec_Eng_SHA256_Finish() disables SHA engine but doesn't release access.
     * Requesting access multiple times may fail due to hardware protection mechanism. */
    
    return 0;
}

int ipro_sec_sha_mutex_give(void)
{
    if (pdPASS != xSemaphoreGive(sha_mutex)) {
        return -1;
    }
    return 0;
}

int ipro_sec_pka_mutex_take(void)
{
    if (!mutex_initialized) {
        ipro_sec_mutex_init();
    }
    if (pdPASS != xSemaphoreTake(pka_mutex, portMAX_DELAY)) {
        return -1;
    }
    return 0;
}

int ipro_sec_pka_mutex_give(void)
{
    if (pdPASS != xSemaphoreGive(pka_mutex)) {
        return -1;
    }
    return 0;
}
#else
void ipro_sec_mutex_init(void)
{
    return;
}

int ipro_sec_aes_mutex_take(void)
{
    return 0;
}

int ipro_sec_aes_mutex_give(void)
{
    return 0;
}

int ipro_sec_sha_mutex_take(void)
{
    return 0;
}

int ipro_sec_sha_mutex_give(void)
{
    return 0;
}

int ipro_sec_pka_mutex_take(void)
{
    return 0;
}

int ipro_sec_pka_mutex_give(void)
{
    return 0;
}
#endif