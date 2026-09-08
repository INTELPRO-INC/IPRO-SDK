/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <tinycrypt/constants.h>
#include <tinycrypt/ecc.h>
#include <tinycrypt/ecc_dh.h>

#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/slist.h>

#include "ecc.h"
#include "hci_core.h"

static uint8_t public_key[BT_PUB_KEY_LEN];
static sys_slist_t public_key_callbacks;
static bt_dh_key_cb_t dh_key_callback;

static struct {
	uint8_t private_key_be[BT_PRIV_KEY_LEN];
	uint8_t peer_or_public_key_be[BT_PUB_KEY_LEN];
} ecc;

enum {
	PENDING_PUBLIC_KEY,
	PENDING_DH_KEY,
	ECC_FLAG_COUNT,
};

static ATOMIC_DEFINE(ecc_flags, ECC_FLAG_COUNT);

static const uint8_t debug_public_key[BT_PUB_KEY_LEN] = {
	0xe6, 0x9d, 0x35, 0x0e, 0x48, 0x01, 0x03, 0xcc,
	0xdb, 0xfd, 0xf4, 0xac, 0x11, 0x91, 0xf4, 0xef,
	0xb9, 0xa5, 0xf9, 0xe9, 0xa7, 0x83, 0x2c, 0x5e,
	0x2c, 0xbe, 0x97, 0xf2, 0xd2, 0x03, 0xb0, 0x20,
	0x8b, 0xd2, 0x89, 0x15, 0xd0, 0x8e, 0x1c, 0x74,
	0x24, 0x30, 0xed, 0x8f, 0xc2, 0x45, 0x63, 0x76,
	0x5c, 0x15, 0x52, 0x5a, 0xbf, 0x9a, 0x32, 0x63,
	0x6d, 0xeb, 0x2a, 0x65, 0x49, 0x9c, 0x80, 0xdc,
};

static const uint8_t debug_private_key_be[BT_PRIV_KEY_LEN] = {
	0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38,
	0x74, 0xc9, 0xb3, 0xe3, 0xd2, 0x10, 0x3f, 0x50,
	0x4a, 0xff, 0x60, 0x7b, 0xeb, 0x40, 0xb7, 0x99,
	0x58, 0x99, 0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd,
};

static void generate_public_key(struct k_work *work);
static void generate_dh_key(struct k_work *work);
K_WORK_DEFINE(public_key_work, generate_public_key);
K_WORK_DEFINE(dh_key_work, generate_dh_key);

bool bt_pub_key_is_debug(uint8_t *key)
{
	return memcmp(key, debug_public_key, sizeof(debug_public_key)) == 0;
}

bool bt_pub_key_is_valid(const uint8_t key[BT_PUB_KEY_LEN])
{
	uint8_t key_be[BT_PUB_KEY_LEN];

	sys_memcpy_swap(key_be, key, BT_PUB_KEY_COORD_LEN);
	sys_memcpy_swap(key_be + BT_PUB_KEY_COORD_LEN,
			key + BT_PUB_KEY_COORD_LEN, BT_PUB_KEY_COORD_LEN);
	return uECC_valid_public_key(key_be, &curve_secp256r1) >= 0;
}

static void generate_public_key(struct k_work *work)
{
	struct bt_pub_key_cb *cb;
	int ok;

	(void)work;
	do {
		ok = uECC_make_key(ecc.peer_or_public_key_be, ecc.private_key_be,
				   &curve_secp256r1);
	} while (ok == TC_CRYPTO_SUCCESS &&
		 memcmp(ecc.private_key_be, debug_private_key_be,
			sizeof(debug_private_key_be)) == 0);

	if (ok == TC_CRYPTO_SUCCESS) {
		sys_memcpy_swap(public_key, ecc.peer_or_public_key_be,
				BT_PUB_KEY_COORD_LEN);
		sys_memcpy_swap(public_key + BT_PUB_KEY_COORD_LEN,
				ecc.peer_or_public_key_be + BT_PUB_KEY_COORD_LEN,
				BT_PUB_KEY_COORD_LEN);
		atomic_set_bit(bt_dev.flags, BT_DEV_HAS_PUB_KEY);
	}

	atomic_clear_bit(ecc_flags, PENDING_PUBLIC_KEY);
	k_sched_lock();
	SYS_SLIST_FOR_EACH_CONTAINER(&public_key_callbacks, cb, node) {
		if (cb->func != NULL) {
			cb->func(ok == TC_CRYPTO_SUCCESS ? public_key : NULL);
		}
	}
	sys_slist_init(&public_key_callbacks);
	k_sched_unlock();
}

static void generate_dh_key(struct k_work *work)
{
	uint8_t dh_key_be[BT_DH_KEY_LEN];
	uint8_t dh_key[BT_DH_KEY_LEN];
	bt_dh_key_cb_t cb;
	int ok;

	(void)work;
	ok = uECC_valid_public_key(ecc.peer_or_public_key_be, &curve_secp256r1);
	if (ok >= 0) {
		ok = uECC_shared_secret(ecc.peer_or_public_key_be,
					ecc.private_key_be, dh_key_be,
					&curve_secp256r1);
	}
	if (ok == TC_CRYPTO_SUCCESS) {
		sys_memcpy_swap(dh_key, dh_key_be, sizeof(dh_key));
	}

	k_sched_lock();
	cb = dh_key_callback;
	dh_key_callback = NULL;
	atomic_clear_bit(ecc_flags, PENDING_DH_KEY);
	if (cb != NULL) {
		cb(ok == TC_CRYPTO_SUCCESS ? dh_key : NULL);
	}
	k_sched_unlock();
}

int bt_pub_key_gen(struct bt_pub_key_cb *new_cb)
{
	struct bt_pub_key_cb *cb;

	if (new_cb == NULL || new_cb->func == NULL) {
		return -EINVAL;
	}
	SYS_SLIST_FOR_EACH_CONTAINER(&public_key_callbacks, cb, node) {
		if (cb == new_cb) {
			return -EALREADY;
		}
	}
	if (atomic_test_bit(ecc_flags, PENDING_DH_KEY)) {
		return -EBUSY;
	}
	sys_slist_prepend(&public_key_callbacks, &new_cb->node);
	if (atomic_test_and_set_bit(ecc_flags, PENDING_PUBLIC_KEY)) {
		return 0;
	}
	atomic_clear_bit(bt_dev.flags, BT_DEV_HAS_PUB_KEY);
	k_work_submit(&public_key_work);
	return 0;
}

void bt_pub_key_hci_disrupted(void)
{
	struct bt_pub_key_cb *cb;

	atomic_clear_bit(ecc_flags, PENDING_PUBLIC_KEY);
	SYS_SLIST_FOR_EACH_CONTAINER(&public_key_callbacks, cb, node) {
		if (cb->func != NULL) {
			cb->func(NULL);
		}
	}
	sys_slist_init(&public_key_callbacks);
}

const uint8_t *bt_pub_key_get(void)
{
	return atomic_test_bit(bt_dev.flags, BT_DEV_HAS_PUB_KEY) ? public_key : NULL;
}

int bt_dh_key_gen(const uint8_t remote_key[BT_PUB_KEY_LEN], bt_dh_key_cb_t cb)
{
	if (cb == NULL) {
		return -EINVAL;
	}
	if (!atomic_test_bit(bt_dev.flags, BT_DEV_HAS_PUB_KEY)) {
		return -EADDRNOTAVAIL;
	}
	if (dh_key_callback == cb) {
		return -EALREADY;
	}
	if (dh_key_callback != NULL ||
	    atomic_test_bit(ecc_flags, PENDING_PUBLIC_KEY) ||
	    atomic_test_and_set_bit(ecc_flags, PENDING_DH_KEY)) {
		return -EBUSY;
	}

	dh_key_callback = cb;
	sys_memcpy_swap(ecc.peer_or_public_key_be, remote_key,
			BT_PUB_KEY_COORD_LEN);
	sys_memcpy_swap(ecc.peer_or_public_key_be + BT_PUB_KEY_COORD_LEN,
			remote_key + BT_PUB_KEY_COORD_LEN, BT_PUB_KEY_COORD_LEN);
	k_work_submit(&dh_key_work);
	return 0;
}

int default_CSPRNG(uint8_t *dest, unsigned int size)
{
	extern int bt_rand(void *buf, size_t len);

	return bt_rand(dest, size) == 0;
}
