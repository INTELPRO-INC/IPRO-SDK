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

/* Public, test-only key material. Never use these values in a product. */
#include <errno.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/ead.h>
#include <zephyr/sys/atomic.h>
#include "ipro_log.h"
#include "shell.h"
#define TAG "EAD_TEST"
static const uint8_t key[16] = {0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,
                                0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf};
static const uint8_t iv[8] = {0xfb,0x56,0xe1,0xda,0xdc,0x7e,0xad,0xf5};
static const uint8_t plain[8] = {7,0xff,0x60,0,'E','A','D','1'};
/* Independently generated with Python cryptography AESCCM, AAD=EA,
 * nonce=randomizer||IV, tag_length=4; not a SIG qualification vector. */
static const uint8_t known[17] = {1,2,3,4,0x85,0xa4,0x98,0x50,0xbc,
                                 0x5a,0x0a,0x49,0x90,0x68,0x10,0xd3,0xab};
static atomic_t received, rejected, mismatch;
static bool scan_registered;
static int selftest(void)
{
    uint8_t out[8] = {0}, encrypted[17] = {0}, other[17] = {0}, wrong[16];
    unsigned checks = 0, failures = 0;
#define CHECK(expr) do { checks++; if (!(expr)) failures++; } while (0)
    CHECK(bt_ead_decrypt(key, iv, known, sizeof(known), out) == 0);
    CHECK(memcmp(out, plain, sizeof(out)) == 0);
    memcpy(other, known, sizeof(other));
    for (unsigned i = 0; i < sizeof(other); ++i) {
        other[i] ^= 1;
        CHECK(bt_ead_decrypt(key, iv, other, sizeof(other), out) != 0);
        other[i] ^= 1;
    }
    memcpy(wrong, key, sizeof(wrong)); wrong[0] ^= 1;
    CHECK(bt_ead_decrypt(wrong, iv, known, sizeof(known), out) != 0);
    memcpy(wrong, iv, sizeof(iv)); wrong[0] ^= 1;
    CHECK(bt_ead_decrypt(key, wrong, known, sizeof(known), out) != 0);
    CHECK(bt_ead_decrypt(key, iv, known, 8, out) == -EINVAL);
    CHECK(bt_ead_decrypt(NULL, iv, known, sizeof(known), out) == -EINVAL);
    CHECK(bt_ead_encrypt(key, iv, NULL, 8, encrypted) == -EINVAL);
    CHECK(bt_ead_encrypt(key, iv, plain, sizeof(plain), encrypted) == 0);
    CHECK((encrypted[4] & 0x80) != 0);
    CHECK(bt_ead_decrypt(key, iv, encrypted, sizeof(encrypted), out) == 0);
    CHECK(memcmp(out, plain, sizeof(out)) == 0);
    CHECK(bt_ead_encrypt(key, iv, plain, sizeof(plain), other) == 0);
    CHECK(memcmp(encrypted, other, BT_EAD_RANDOMIZER_SIZE) != 0);
    IPRO_LOGI(TAG, "SELFTEST %s checks=%u failures=%u", failures ? "FAIL" : "PASS",
              checks, failures);
    return failures ? -EIO : 0;
#undef CHECK
}
static bool parse(struct bt_data *data, void *user)
{
    (void)user;
    if (data->type != BT_DATA_ENCRYPTED_AD_DATA || data->data_len != sizeof(known))
        return true;
    uint8_t out[8];
    int err = bt_ead_decrypt(key, iv, data->data, data->data_len, out);
    if (err != 0) {
        if (atomic_inc(&rejected) == 0) IPRO_LOGI(TAG, "AIR_REJECT rc=%d", err);
    } else if (memcmp(out, plain, sizeof(out)) != 0) {
        atomic_inc(&mismatch);
    } else {
        if (atomic_inc(&received) == 0) IPRO_LOGI(TAG, "AIR_PASS len=8 exact_content=1");
    }
    return true;
}
static void scan_recv(const struct bt_le_scan_recv_info *info, struct net_buf_simple *buf)
{
    (void)info;
    bt_data_parse(buf, parse, NULL);
}
static struct bt_le_scan_cb scan_cb = {.recv = scan_recv};
static int command(int argc, char **argv)
{
    int err = 0;
    if (argc != 2) return -EINVAL;
    if (strcmp(argv[1], "test") == 0) return selftest();
    if (strcmp(argv[1], "adv") == 0 || strcmp(argv[1], "bad") == 0) {
        uint8_t encrypted[sizeof(known)];
        err = bt_ead_encrypt(key, iv, plain, sizeof(plain), encrypted);
        if (err) return err;
        if (strcmp(argv[1], "bad") == 0) encrypted[sizeof(encrypted)-1] ^= 1;
        struct bt_data ad = BT_DATA(BT_DATA_ENCRYPTED_AD_DATA, encrypted, sizeof(encrypted));
        (void)bt_le_adv_stop();
        err = bt_le_adv_start(BT_LE_ADV_NCONN, &ad, 1, NULL, 0);
    } else if (strcmp(argv[1], "scan") == 0) {
        if (!scan_registered) {
            err = bt_le_scan_cb_register(&scan_cb);
            if (err) return err;
            scan_registered = true;
        }
        err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, NULL);
        if (err == -EALREADY) err = 0;
    } else if (strcmp(argv[1], "stop") == 0) {
        err = bt_le_adv_stop();
    } else if (strcmp(argv[1], "status") != 0) return -EINVAL;
    IPRO_LOGI(TAG, "%s rc=%d good=%ld rejected=%ld mismatch=%ld", argv[1], err,
              (long)atomic_get(&received), (long)atomic_get(&rejected),
              (long)atomic_get(&mismatch));
    return err;
}
SHELL_CMD_EXPORT_ALIAS(command, ead, Test only encrypted advertising);
