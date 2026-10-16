/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Symbols the SDK Wi-Fi sources reference that have no counterpart in this
 * Zephyr build: WPA3/SAE (the supplicant runs on its internal crypto,
 * WPA2-PSK only), the SDK shell CLI, and the POSIX file API the
 * supplicant's config code links against but never exercises.
 */
#include <zephyr/kernel.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

/* ---- SAE (WPA3) is compiled out: every entry fails cleanly ------------- */
struct sae_data;
struct wpabuf;
struct crypto_bignum;
struct sae_pt;

int sae_set_group(struct sae_data *sae, int group) { ARG_UNUSED(sae); ARG_UNUSED(group); return -1; }
void sae_clear_temp_data(struct sae_data *sae) { ARG_UNUSED(sae); }
void sae_clear_data(struct sae_data *sae) { ARG_UNUSED(sae); }
int sae_prepare_commit(const uint8_t *addr1, const uint8_t *addr2, const uint8_t *password,
		       size_t password_len, struct sae_data *sae)
{
	ARG_UNUSED(addr1); ARG_UNUSED(addr2); ARG_UNUSED(password); ARG_UNUSED(password_len);
	ARG_UNUSED(sae);
	return -1;
}
int sae_prepare_commit_pt(struct sae_data *sae, const struct sae_pt *pt, const uint8_t *addr1,
			  const uint8_t *addr2, int *rejected_groups, const void *pk)
{
	ARG_UNUSED(sae); ARG_UNUSED(pt); ARG_UNUSED(addr1); ARG_UNUSED(addr2);
	ARG_UNUSED(rejected_groups); ARG_UNUSED(pk);
	return -1;
}
int sae_process_commit(struct sae_data *sae) { ARG_UNUSED(sae); return -1; }
int sae_write_commit(struct sae_data *sae, struct wpabuf *buf, const struct wpabuf *token,
		     const char *identifier)
{
	ARG_UNUSED(sae); ARG_UNUSED(buf); ARG_UNUSED(token); ARG_UNUSED(identifier);
	return -1;
}
uint16_t sae_parse_commit(struct sae_data *sae, const uint8_t *data, size_t len,
			  const uint8_t **token, size_t *token_len, int *allowed_groups,
			  int h2e, int *ie_offset)
{
	ARG_UNUSED(sae); ARG_UNUSED(data); ARG_UNUSED(len); ARG_UNUSED(token);
	ARG_UNUSED(token_len); ARG_UNUSED(allowed_groups); ARG_UNUSED(h2e); ARG_UNUSED(ie_offset);
	return 1; /* WLAN_STATUS_UNSPECIFIED_FAILURE */
}
int sae_write_confirm(struct sae_data *sae, struct wpabuf *buf) { ARG_UNUSED(sae); ARG_UNUSED(buf); return -1; }
int sae_check_confirm(struct sae_data *sae, const uint8_t *data, size_t len)
{
	ARG_UNUSED(sae); ARG_UNUSED(data); ARG_UNUSED(len);
	return -1;
}
const char *sae_state_txt(int state) { ARG_UNUSED(state); return "SAE-UNSUPPORTED"; }
struct sae_pt *sae_derive_pt(int *groups, const uint8_t *ssid, size_t ssid_len,
			     const uint8_t *password, size_t password_len, const char *identifier)
{
	ARG_UNUSED(groups); ARG_UNUSED(ssid); ARG_UNUSED(ssid_len); ARG_UNUSED(password);
	ARG_UNUSED(password_len); ARG_UNUSED(identifier);
	return NULL;
}
void sae_deinit_pt(struct sae_pt *pt) { ARG_UNUSED(pt); }
int sae_group_allowed(struct sae_data *sae, int *allowed_groups, int group)
{
	ARG_UNUSED(sae); ARG_UNUSED(allowed_groups); ARG_UNUSED(group);
	return 1; /* WLAN_STATUS_UNSPECIFIED_FAILURE */
}
void crypto_bignum_deinit(struct crypto_bignum *n, int clear) { ARG_UNUSED(n); ARG_UNUSED(clear); }

/* ---- SDK shell CLI -------------------------------------------------------- */
int wifi6_cli_init(void) { return 0; }

/* ---- POSIX file API (supplicant config file code, unused here) ---------- */
int open(const char *path, int flags, ...) { ARG_UNUSED(path); ARG_UNUSED(flags); return -1; }
int close(int fd) { ARG_UNUSED(fd); return -1; }
long read(int fd, void *buf, size_t n) { ARG_UNUSED(fd); ARG_UNUSED(buf); ARG_UNUSED(n); return -1; }
long write(int fd, const void *buf, size_t n) { ARG_UNUSED(fd); ARG_UNUSED(buf); ARG_UNUSED(n); return -1; }
long lseek(int fd, long off, int whence) { ARG_UNUSED(fd); ARG_UNUSED(off); ARG_UNUSED(whence); return -1; }
