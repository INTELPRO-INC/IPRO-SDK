/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <hal_sec_trng.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/cmac_mode.h>
#include <tinycrypt/constants.h>

#undef STRINGIFY
#undef likely
#undef unlikely

#include <zephyr/sys/byteorder.h>

int bt_crypto_init(void)
{
	/* The IPRO TRNG and TinyCrypt backend require no retained software state. */
	return 0;
}

int bt_rand(void *buf, size_t len)
{
	uint8_t *out = buf;
	while (len != 0U) {
		uint32_t word = hal_sec_get_random_word();
		for (unsigned int i = 0; i < sizeof(word) && len != 0U; i++) {
			*out++ = (uint8_t)word;
			word >>= 8;
			len--;
		}
	}
	return 0;
}

int bt_encrypt_be(const uint8_t key[16], const uint8_t plaintext[16],
		  uint8_t enc_data[16])
{
	struct tc_aes_key_sched_struct sched;
	if (tc_aes128_set_encrypt_key(&sched, key) == TC_CRYPTO_FAIL ||
	    tc_aes_encrypt(enc_data, plaintext, &sched) == TC_CRYPTO_FAIL) {
		return -EIO;
	}
	return 0;
}

int bt_encrypt_le(const uint8_t key[16], const uint8_t plaintext[16],
		  uint8_t enc_data[16])
{
	uint8_t key_be[16];
	uint8_t plain_be[16];
	uint8_t result_be[16];
	sys_memcpy_swap(key_be, key, sizeof(key_be));
	sys_memcpy_swap(plain_be, plaintext, sizeof(plain_be));
	if (bt_encrypt_be(key_be, plain_be, result_be) != 0) {
		return -EIO;
	}
	sys_memcpy_swap(enc_data, result_be, sizeof(result_be));
	return 0;
}

int bt_crypto_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len,
		       uint8_t *out)
{
	struct tc_aes_key_sched_struct sched;
	struct tc_cmac_struct state;
	if (tc_cmac_setup(&state, key, &sched) == TC_CRYPTO_FAIL ||
	    tc_cmac_update(&state, in, len) == TC_CRYPTO_FAIL ||
	    tc_cmac_final(out, &state) == TC_CRYPTO_FAIL) {
		return -EIO;
	}
	return 0;
}
