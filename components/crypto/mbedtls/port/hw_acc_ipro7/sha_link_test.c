/*
 * SHA Link Mode Test - Following IntelPro reference implementation
 */

#include "drv_sec_eng.h"
#include "glb_reg.h"
#include "ipro7.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define IPRO_SHA_ID SEC_ENG_SHA_ID0

// Test vector from IntelPro
static const char tc_hash_input[] = "The quick brown fox jumps over the lazy dog";

void test_sha256_link_mode(void)
{
    printf("\n[LINK MODE TEST] Testing SHA-256 with Link API...\n");
    
    // Allocate aligned buffers
    uint32_t tmp_buf[16] __attribute__((aligned(4)));
    uint32_t padding[16] __attribute__((aligned(4)));
    SEC_Eng_SHA256_Link_Ctx ctx;
    uint8_t output[32];
    
    // Link configuration structure
    SEC_Eng_SHA_Link_Config_Type link_cfg;
    memset(&link_cfg, 0, sizeof(link_cfg));
    link_cfg.shaMode = SEC_ENG_SHA256;
    link_cfg.shaIntSet = 1;
    link_cfg.shaIntClr = 1;
    
    // Clear buffers
    memset(&ctx, 0, sizeof(ctx));
    memset(tmp_buf, 0, sizeof(tmp_buf));
    memset(padding, 0, sizeof(padding));
    memset(output, 0, sizeof(output));
    
    // Enable Link Mode first!
    printf("  Enabling SHA Link Mode...\n");
    Sec_Eng_SHA_Enable_Link(IPRO_SHA_ID);
    
    // Initialize Link Mode context
    printf("  Link_Init...\n");
    Sec_Eng_SHA256_Link_Init(&ctx, IPRO_SHA_ID, (uint32_t)&link_cfg, tmp_buf, padding);
    
    // Update with test data
    size_t len = sizeof(tc_hash_input) - 1;
    printf("  Link_Update with %zu bytes: \"%s\"\n", len, tc_hash_input);
    int ret = Sec_Eng_SHA256_Link_Update(&ctx, IPRO_SHA_ID, (const uint8_t *)tc_hash_input, len);
    printf("  Link_Update returned: %d\n", ret);
    
    // Finish
    printf("  Link_Finish...\n");
    ret = Sec_Eng_SHA256_Link_Finish(&ctx, IPRO_SHA_ID, output);
    printf("  Link_Finish returned: %d\n", ret);
    
    // Expected result (from IntelPro test)
    const uint8_t expected[32] = {
        0xd7, 0xa8, 0xfb, 0xb3, 0x07, 0xd7, 0x80, 0x94,
        0x69, 0xca, 0x9a, 0xbc, 0xb0, 0x08, 0x2e, 0x4f,
        0x8d, 0x56, 0x51, 0xe4, 0x6d, 0x3c, 0xdb, 0x76,
        0x2d, 0x02, 0xd0, 0xbf, 0x37, 0xc9, 0xe5, 0x92
    };
    
    printf("\n  Raw output:     ");
    for (int i = 0; i < 32; i++) printf("%02X", output[i]);
    printf("\n  Expected (BE):  ");
    for (int i = 0; i < 32; i++) printf("%02X", expected[i]);
    printf("\n");
    
    int match = (memcmp(output, expected, 32) == 0);
    printf("  Result: %s\n\n", match ? "PASS ✓" : "FAIL ✗");
    
    // Disable Link Mode after test
    Sec_Eng_SHA_Disable_Link(IPRO_SHA_ID);
}

void test_sha256_basic_mode(void)
{
    printf("[BASIC MODE TEST] Testing SHA-256 with Basic API...\n");
    
    // Allocate aligned buffers
    uint32_t tmp_buf[16] __attribute__((aligned(4)));
    uint32_t padding[16] __attribute__((aligned(4)));
    SEC_Eng_SHA256_Ctx ctx;
    uint8_t output[32];
    uint8_t output_raw[32];  // Raw output before any byte swapping
    
    // Clear buffers
    memset(&ctx, 0, sizeof(ctx));
    memset(tmp_buf, 0, sizeof(tmp_buf));
    memset(padding, 0, sizeof(padding));
    memset(output, 0, sizeof(output));
    memset(output_raw, 0, sizeof(output_raw));
    
    // Initialize Basic Mode context
    printf("  SHA256_Init...\n");
    Sec_Eng_SHA256_Init(&ctx, IPRO_SHA_ID, SEC_ENG_SHA256, tmp_buf, padding);
    
    // Start SHA engine
    printf("  SHA_Start...\n");
    Sec_Eng_SHA_Start(IPRO_SHA_ID);
    
    // Update with test data
    size_t len = sizeof(tc_hash_input) - 1;
    printf("  SHA256_Update with %zu bytes: \"%s\"\n", len, tc_hash_input);
    int ret = Sec_Eng_SHA256_Update(&ctx, IPRO_SHA_ID, (const uint8_t *)tc_hash_input, len);
    printf("  SHA256_Update returned: %d, shaFeed=%d, total=%lu\n", ret, ctx.shaFeed, ctx.total[0]);
    
    // Finish - get raw output WITHOUT byte swapping first
    printf("  SHA256_Finish...\n");
    ret = Sec_Eng_SHA256_Finish(&ctx, IPRO_SHA_ID, output_raw);
    printf("  SHA256_Finish returned: %d\n", ret);
    
    // Copy to output and apply byte swap
    memcpy(output, output_raw, 32);
    for (int i = 0; i < 8; i++) {
        uint8_t b0 = output[i*4+0];
        uint8_t b1 = output[i*4+1];
        uint8_t b2 = output[i*4+2];
        uint8_t b3 = output[i*4+3];
        output[i*4+0] = b3;
        output[i*4+1] = b2;
        output[i*4+2] = b1;
        output[i*4+3] = b0;
    }
    
    // Expected result
    const uint8_t expected[32] = {
        0xd7, 0xa8, 0xfb, 0xb3, 0x07, 0xd7, 0x80, 0x94,
        0x69, 0xca, 0x9a, 0xbc, 0xb0, 0x08, 0x2e, 0x4f,
        0x8d, 0x56, 0x51, 0xe4, 0x6d, 0x3c, 0xdb, 0x76,
        0x2d, 0x02, 0xd0, 0xbf, 0x37, 0xc9, 0xe5, 0x92
    };
    
    printf("\n  Raw HW (LE):    ");
    for (int i = 0; i < 32; i++) printf("%02X", output_raw[i]);
    printf("\n  After swap(BE): ");
    for (int i = 0; i < 32; i++) printf("%02X", output[i]);
    printf("\n  Expected (BE):  ");
    for (int i = 0; i < 32; i++) printf("%02X", expected[i]);
    printf("\n");
    
    int match_raw = (memcmp(output_raw, expected, 32) == 0);
    int match_swapped = (memcmp(output, expected, 32) == 0);
    printf("  Match raw: %s, Match swapped: %s\n", 
           match_raw ? "YES" : "NO",
           match_swapped ? "YES" : "NO");
    printf("  Result: %s\n\n", match_swapped ? "PASS ✓" : "FAIL ✗");
}
