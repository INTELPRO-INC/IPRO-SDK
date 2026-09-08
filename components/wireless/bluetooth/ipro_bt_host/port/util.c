/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/sys/crc.h>

int char2hex(char c, uint8_t *x)
{
	if (x == NULL) {
		return -EINVAL;
	}

	if ((c >= '0') && (c <= '9')) {
		*x = (uint8_t)(c - '0');
	} else if ((c >= 'a') && (c <= 'f')) {
		*x = (uint8_t)(c - 'a' + 10);
	} else if ((c >= 'A') && (c <= 'F')) {
		*x = (uint8_t)(c - 'A' + 10);
	} else {
		return -EINVAL;
	}

	return 0;
}

int hex2char(uint8_t x, char *c)
{
	if (c == NULL) {
		return -EINVAL;
	}

	if (x <= 9U) {
		*c = (char)(x + '0');
	} else if (x <= 15U) {
		*c = (char)(x - 10U + 'a');
	} else {
		return -EINVAL;
	}

	return 0;
}

size_t bin2hex(const uint8_t *buf, size_t buflen, char *hex, size_t hexlen)
{
	if ((buf == NULL) || (hex == NULL) || (hexlen < ((buflen * 2U) + 1U))) {
		return 0U;
	}

	for (size_t i = 0U; i < buflen; i++) {
		if (hex2char((uint8_t)(buf[i] >> 4), &hex[2U * i]) < 0) {
			return 0U;
		}
		if (hex2char((uint8_t)(buf[i] & 0x0fU), &hex[(2U * i) + 1U]) < 0) {
			return 0U;
		}
	}

	hex[2U * buflen] = '\0';
	return 2U * buflen;
}

size_t hex2bin(const char *hex, size_t hexlen, uint8_t *buf, size_t buflen)
{
	uint8_t dec;
	size_t out_len = 0U;

	if ((hex == NULL) || (buf == NULL) ||
	    (buflen < ((hexlen / 2U) + (hexlen % 2U)))) {
		return 0U;
	}

	if ((hexlen % 2U) != 0U) {
		if (char2hex(hex[0], &dec) < 0) {
			return 0U;
		}
		buf[out_len++] = dec;
		hex++;
		hexlen--;
	}

	for (size_t i = 0U; i < (hexlen / 2U); i++) {
		if (char2hex(hex[2U * i], &dec) < 0) {
			return 0U;
		}
		buf[out_len] = (uint8_t)(dec << 4);

		if (char2hex(hex[(2U * i) + 1U], &dec) < 0) {
			return 0U;
		}
		buf[out_len] = (uint8_t)(buf[out_len] | dec);
		out_len++;
	}

	return out_len;
}

uint8_t u8_to_dec(char *buf, uint8_t buflen, uint8_t value)
{
	uint8_t divisor = 100U;
	uint8_t num_digits = 0U;

	while ((buflen > 0U) && (divisor > 0U)) {
		uint8_t digit = (uint8_t)(value / divisor);

		if ((digit != 0U) || (divisor == 1U) || (num_digits != 0U)) {
			*buf = (char)(digit + '0');
			buf++;
			buflen--;
			num_digits++;
		}

		value = (uint8_t)(value - (digit * divisor));
		divisor = (uint8_t)(divisor / 10U);
	}

	if (buflen > 0U) {
		*buf = '\0';
	}

	return num_digits;
}

uint16_t crc16_reflect(uint16_t poly, uint16_t seed, const uint8_t *src,
		       size_t len)
{
	while (len-- > 0U) {
		seed ^= *src++;
		for (unsigned int bit = 0U; bit < 8U; bit++) {
			seed = (seed & 1U) != 0U ? (seed >> 1) ^ poly : seed >> 1;
		}
	}

	return seed;
}
