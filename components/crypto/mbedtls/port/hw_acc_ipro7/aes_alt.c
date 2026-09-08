/*
 *  FIPS-197 compliant AES implementation for IPRO7 using HAL API
 */

#include "common.h"

#if defined(MBEDTLS_AES_C)

#include <string.h>
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#if defined(MBEDTLS_AES_ALT)

#include "hal_sec_aes.h"

/* Global static handle to avoid large stack allocation */
static sec_aes_handle_t g_aes_handle __attribute__((aligned(32)));

#define AES_VALIDATE_RET(cond) \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA)
#define AES_VALIDATE(cond) \
    MBEDTLS_INTERNAL_VALIDATE(cond)

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    AES_VALIDATE(ctx != NULL);
    memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    if (ctx == NULL)
        return;
    
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_aes_context));
}

int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key,
                             unsigned int keybits)
{
    AES_VALIDATE_RET(ctx != NULL);
    AES_VALIDATE_RET(key != NULL);

    if (keybits != 128 && keybits != 192 && keybits != 256)
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;

    ctx->keybits = keybits;
    memcpy(ctx->key, key, keybits / 8);
    return 0;
}

int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key,
                             unsigned int keybits)
{
    return mbedtls_aes_setkey_enc(ctx, key, keybits);
}

int mbedtls_internal_aes_encrypt(mbedtls_aes_context *ctx,
                                   const unsigned char input[16],
                                   unsigned char output[16])
{
    AES_VALIDATE_RET(ctx != NULL);
    AES_VALIDATE_RET(input != NULL);
    AES_VALIDATE_RET(output != NULL);
    
    uint8_t key_type;
    switch (ctx->keybits) {
        case 128: key_type = SEC_AES_KEY_128; break;
        case 192: key_type = SEC_AES_KEY_192; break;
        case 256: key_type = SEC_AES_KEY_256; break;
        default: return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }
    
    int ret = sec_aes_init_ex(&g_aes_handle, SEC_MODE_LINK, SEC_AES_ECB, key_type);
    if (ret != 0)
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    
    ret = sec_aes_setkey(&g_aes_handle, ctx->key, ctx->keybits / 8, NULL, SEC_AES_DIR_ENCRYPT);
    if (ret != 0) {
        sec_aes_deinit(&g_aes_handle);
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }
    
    ret = sec_aes_encrypt(&g_aes_handle, input, 16, 0, output);
    sec_aes_deinit(&g_aes_handle);
    
    return (ret == 0) ? 0 : MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
}

int mbedtls_internal_aes_decrypt(mbedtls_aes_context *ctx,
                                   const unsigned char input[16],
                                   unsigned char output[16])
{
    AES_VALIDATE_RET(ctx != NULL);
    AES_VALIDATE_RET(input != NULL);
    AES_VALIDATE_RET(output != NULL);
    
    uint8_t key_type;
    switch (ctx->keybits) {
        case 128: key_type = SEC_AES_KEY_128; break;
        case 192: key_type = SEC_AES_KEY_192; break;
        case 256: key_type = SEC_AES_KEY_256; break;
        default: return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }
    
    int ret = sec_aes_init_ex(&g_aes_handle, SEC_MODE_LINK, SEC_AES_ECB, key_type);
    if (ret != 0)
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    
    ret = sec_aes_setkey(&g_aes_handle, ctx->key, ctx->keybits / 8, NULL, SEC_AES_DIR_DECRYPT);
    if (ret != 0) {
        sec_aes_deinit(&g_aes_handle);
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }
    
    ret = sec_aes_decrypt(&g_aes_handle, input, 16, 0, output);
    sec_aes_deinit(&g_aes_handle);
    
    return (ret == 0) ? 0 : MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
}

int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode,
                           const unsigned char input[16],
                           unsigned char output[16])
{
    AES_VALIDATE_RET(ctx != NULL);
    AES_VALIDATE_RET(input != NULL);
    AES_VALIDATE_RET(output != NULL);
    AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT || mode == MBEDTLS_AES_DECRYPT);

    if (mode == MBEDTLS_AES_ENCRYPT)
        return mbedtls_internal_aes_encrypt(ctx, input, output);
    else
        return mbedtls_internal_aes_decrypt(ctx, input, output);
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx, int mode, size_t length,
                           unsigned char iv[16], const unsigned char *input,
                           unsigned char *output)
{
    AES_VALIDATE_RET(ctx != NULL);
    AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT || mode == MBEDTLS_AES_DECRYPT);
    AES_VALIDATE_RET(iv != NULL);
    AES_VALIDATE_RET(input != NULL);
    AES_VALIDATE_RET(output != NULL);
    
    /* Length must be multiple of 16 bytes for CBC */
    if (length == 0 || (length % 16) != 0)
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    
    uint8_t key_type;
    switch (ctx->keybits) {
        case 128: key_type = SEC_AES_KEY_128; break;
        case 192: key_type = SEC_AES_KEY_192; break;
        case 256: key_type = SEC_AES_KEY_256; break;
        default: return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }
    
    sec_aes_dir_type dir = (mode == MBEDTLS_AES_ENCRYPT) ? SEC_AES_DIR_ENCRYPT : SEC_AES_DIR_DECRYPT;
    
    int ret = sec_aes_init_ex(&g_aes_handle, SEC_MODE_LINK, SEC_AES_CBC, key_type);
    if (ret != 0)
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    
    ret = sec_aes_setkey(&g_aes_handle, ctx->key, ctx->keybits / 8, iv, dir);
    if (ret != 0) {
        sec_aes_deinit(&g_aes_handle);
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }
    
    /* Process data in 16-byte blocks */
    for (size_t offset = 0; offset < length; offset += 16) {
        if (mode == MBEDTLS_AES_ENCRYPT)
            ret = sec_aes_encrypt(&g_aes_handle, input + offset, 16, 0, output + offset);
        else
            ret = sec_aes_decrypt(&g_aes_handle, input + offset, 16, 0, output + offset);
        
        if (ret != 0) {
            sec_aes_deinit(&g_aes_handle);
            return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
        }
    }
    
    sec_aes_deinit(&g_aes_handle);
    return 0;
}
#endif

#if defined(MBEDTLS_CIPHER_MODE_CFB)
int mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx, int mode, size_t length,
                               size_t *iv_off, unsigned char iv[16],
                               const unsigned char *input, unsigned char *output)
{
    (void)ctx; (void)mode; (void)length; (void)iv_off; (void)iv; (void)input; (void)output;
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_aes_crypt_cfb8(mbedtls_aes_context *ctx, int mode, size_t length,
                            unsigned char iv[16], const unsigned char *input,
                            unsigned char *output)
{
    (void)ctx; (void)mode; (void)length; (void)iv; (void)input; (void)output;
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}
#endif

#if defined(MBEDTLS_CIPHER_MODE_OFB)
int mbedtls_aes_crypt_ofb(mbedtls_aes_context *ctx, size_t length, size_t *iv_off,
                           unsigned char iv[16], const unsigned char *input,
                           unsigned char *output)
{
    (void)ctx; (void)length; (void)iv_off; (void)iv; (void)input; (void)output;
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}
#endif

#if defined(MBEDTLS_CIPHER_MODE_CTR)
int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx, size_t length, size_t *nc_off,
                           unsigned char nonce_counter[16], unsigned char stream_block[16],
                           const unsigned char *input, unsigned char *output)
{
    AES_VALIDATE_RET(ctx != NULL);
    AES_VALIDATE_RET(nc_off != NULL);
    AES_VALIDATE_RET(nonce_counter != NULL);
    AES_VALIDATE_RET(stream_block != NULL);
    AES_VALIDATE_RET(input != NULL);
    AES_VALIDATE_RET(output != NULL);

    if (length == 0)
        return 0;

    uint8_t key_type;
    switch (ctx->keybits) {
        case 128: key_type = SEC_AES_KEY_128; break;
        case 192: key_type = SEC_AES_KEY_192; break;
        case 256: key_type = SEC_AES_KEY_256; break;
        default: return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    size_t n = *nc_off;

    /* Process any remaining bytes from previous call */
    while (length > 0 && n > 0) {
        *output++ = *input++ ^ stream_block[n];
        n = (n + 1) % 16;
        length--;
    }

    /* Process complete 16-byte blocks with hardware CTR */
    if (length >= 16) {
        size_t blocks = length / 16;
        size_t block_bytes = blocks * 16;

        int ret = sec_aes_init_ex(&g_aes_handle, SEC_MODE_LINK, SEC_AES_CTR, key_type);
        if (ret != 0)
            return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;

        ret = sec_aes_setkey(&g_aes_handle, ctx->key, ctx->keybits / 8,
                             nonce_counter, SEC_AES_DIR_ENCRYPT);
        if (ret != 0) {
            sec_aes_deinit(&g_aes_handle);
            return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
        }

        ret = sec_aes_encrypt(&g_aes_handle, input, block_bytes, 0, output);
        sec_aes_deinit(&g_aes_handle);

        if (ret != 0)
            return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;

        /* Update counter for processed blocks */
        for (size_t i = 0; i < blocks; i++) {
            for (int j = 15; j >= 0; j--) {
                if (++nonce_counter[j] != 0)
                    break;
            }
        }

        input += block_bytes;
        output += block_bytes;
        length -= block_bytes;
    }

    /* Process remaining bytes: generate keystream via HW ECB */
    while (length > 0) {
        if (n == 0) {
            int ret = sec_aes_init_ex(&g_aes_handle, SEC_MODE_LINK, SEC_AES_CTR, key_type);
            if (ret != 0)
                return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;

            ret = sec_aes_setkey(&g_aes_handle, ctx->key, ctx->keybits / 8,
                                 nonce_counter, SEC_AES_DIR_ENCRYPT);
            if (ret != 0) {
                sec_aes_deinit(&g_aes_handle);
                return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
            }

            unsigned char zero_block[16] = {0};
            ret = sec_aes_encrypt(&g_aes_handle, zero_block, 16, 0, stream_block);
            sec_aes_deinit(&g_aes_handle);

            if (ret != 0)
                return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;

            for (int j = 15; j >= 0; j--) {
                if (++nonce_counter[j] != 0)
                    break;
            }
        }

        *output++ = *input++ ^ stream_block[n];
        n = (n + 1) % 16;
        length--;
    }

    *nc_off = n;
    return 0;
}
#endif

#if defined(MBEDTLS_CIPHER_MODE_XTS)
void mbedtls_aes_xts_init(mbedtls_aes_xts_context *ctx)
{
    AES_VALIDATE(ctx != NULL);
    mbedtls_aes_init(&ctx->crypt);
    mbedtls_aes_init(&ctx->tweak);
}

void mbedtls_aes_xts_free(mbedtls_aes_xts_context *ctx)
{
    if (ctx == NULL)
        return;
    mbedtls_aes_free(&ctx->crypt);
    mbedtls_aes_free(&ctx->tweak);
}

int mbedtls_aes_xts_setkey_enc(mbedtls_aes_xts_context *ctx,
                                 const unsigned char *key,
                                 unsigned int keybits)
{
    (void)ctx; (void)key; (void)keybits;
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_aes_xts_setkey_dec(mbedtls_aes_xts_context *ctx,
                                 const unsigned char *key,
                                 unsigned int keybits)
{
    (void)ctx; (void)key; (void)keybits;
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_aes_crypt_xts(mbedtls_aes_xts_context *ctx, int mode, size_t length,
                           const unsigned char data_unit[16],
                           const unsigned char *input, unsigned char *output)
{
    (void)ctx; (void)mode; (void)length; (void)data_unit; (void)input; (void)output;
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}
#endif

int mbedtls_aes_self_test_hw(int verbose)
{
    int ret = 0;
    mbedtls_aes_context ctx;
    unsigned char output[64];
    
    /* NIST SP 800-38A test vectors */
    static const unsigned char aes128_key[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };
    static const unsigned char aes128_plaintext[16] = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
    };
    
    /* ECB test vector */
    static const unsigned char aes128_ecb_ciphertext[16] = {
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
    };
    
    /* CBC test vectors */
    static const unsigned char aes_cbc_iv[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };
    static const unsigned char aes128_cbc_ciphertext[16] = {
        0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
        0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
    };
    
    /* CTR test vectors */
    static const unsigned char aes_ctr_nonce[16] = {
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
    };
    static const unsigned char aes128_ctr_ciphertext[16] = {
        0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
        0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce
    };
    
    mbedtls_aes_init(&ctx);
    
    /* ==================== ECB Mode Test ==================== */
    if (verbose != 0) {
        printf("  AES-128 ECB: ");
    }
    
    ret = mbedtls_aes_setkey_enc(&ctx, aes128_key, 128);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (setkey_enc)\n");
        goto exit;
    }
    
    ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, aes128_plaintext, output);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (encrypt)\n");
        goto exit;
    }
    
    if (memcmp(output, aes128_ecb_ciphertext, 16) != 0) {
        ret = 1;
        if (verbose != 0) printf("FAILED (encrypt mismatch)\n");
        goto exit;
    }
    
    ret = mbedtls_aes_setkey_dec(&ctx, aes128_key, 128);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (setkey_dec)\n");
        goto exit;
    }
    
    ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, aes128_ecb_ciphertext, output);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (decrypt)\n");
        goto exit;
    }
    
    if (memcmp(output, aes128_plaintext, 16) != 0) {
        ret = 1;
        if (verbose != 0) printf("FAILED (decrypt mismatch)\n");
        goto exit;
    }
    
    if (verbose != 0) printf("PASSED\n");
    
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    /* ==================== CBC Mode Test ==================== */
    if (verbose != 0) {
        printf("  AES-128 CBC: ");
    }
    
    unsigned char iv[16];
    memcpy(iv, aes_cbc_iv, 16);
    
    ret = mbedtls_aes_setkey_enc(&ctx, aes128_key, 128);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (setkey_enc)\n");
        goto exit;
    }
    
    ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, 16, iv, aes128_plaintext, output);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (encrypt)\n");
        goto exit;
    }
    
    if (memcmp(output, aes128_cbc_ciphertext, 16) != 0) {
        ret = 1;
        if (verbose != 0) printf("FAILED (encrypt mismatch)\n");
        goto exit;
    }
    
    memcpy(iv, aes_cbc_iv, 16);
    ret = mbedtls_aes_setkey_dec(&ctx, aes128_key, 128);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (setkey_dec)\n");
        goto exit;
    }
    
    ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, 16, iv, aes128_cbc_ciphertext, output);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (decrypt)\n");
        goto exit;
    }
    
    if (memcmp(output, aes128_plaintext, 16) != 0) {
        ret = 1;
        if (verbose != 0) printf("FAILED (decrypt mismatch)\n");
        goto exit;
    }
    
    if (verbose != 0) printf("PASSED\n");
#endif
    
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    /* ==================== CTR Mode Test ==================== */
    if (verbose != 0) {
        printf("  AES-128 CTR: ");
    }
    
    unsigned char nonce[16];
    unsigned char stream_block[16] = {0};  /* Initialize stream_block */
    size_t nc_off = 0;
    
    memcpy(nonce, aes_ctr_nonce, 16);
    
    ret = mbedtls_aes_setkey_enc(&ctx, aes128_key, 128);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (setkey_enc)\n");
        goto exit;
    }
    
    ret = mbedtls_aes_crypt_ctr(&ctx, 16, &nc_off, nonce, stream_block, aes128_plaintext, output);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (encrypt, code: %d)\n", ret);
        goto exit;
    }
    
    if (memcmp(output, aes128_ctr_ciphertext, 16) != 0) {
        ret = 1;
        if (verbose != 0) printf("FAILED (encrypt mismatch)\n");
        goto exit;
    }
    
    /* CTR decrypt (same as encrypt) */
    memcpy(nonce, aes_ctr_nonce, 16);
    memset(stream_block, 0, 16);  /* Reset stream_block */
    nc_off = 0;
    
    ret = mbedtls_aes_crypt_ctr(&ctx, 16, &nc_off, nonce, stream_block, aes128_ctr_ciphertext, output);
    if (ret != 0) {
        if (verbose != 0) printf("FAILED (decrypt, code: %d)\n", ret);
        goto exit;
    }
    
    if (memcmp(output, aes128_plaintext, 16) != 0) {
        ret = 1;
        if (verbose != 0) printf("FAILED (decrypt mismatch)\n");
        goto exit;
    }
    
    if (verbose != 0) printf("PASSED\n");
#endif
    
    ret = 0;
    
exit:
    mbedtls_aes_free(&ctx);
    return ret;
}

#endif /* MBEDTLS_AES_ALT */
#endif /* MBEDTLS_AES_C */
