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

#include <assert.h>
#include "../upstream/zephyr/subsys/bluetooth/host/pawr_reassembly.h"
int main(void)
{
    struct bt_pawr_reassembly state = {0};
    uint8_t bytes[251], *out;
    uint16_t length;
    for (unsigned i = 0; i < sizeof(bytes); i++) bytes[i] = i;
    assert(bt_pawr_fragment(&state, 1, 0, 9, 1, bytes, 244, &out, &length) == 0);
    assert(bt_pawr_fragment(&state, 1, 1, 9, 1, bytes, 120, &out, &length) == 0);
    assert(bt_pawr_fragment(&state, 1, 0, 9, 0, bytes + 244, 7, &out, &length) == 1);
    assert(length == 251 && !memcmp(out, bytes, 251));
    assert(bt_pawr_fragment(&state, 1, 1, 9, 0, bytes + 120, 131, &out, &length) == 1);
    assert(length == 251 && !memcmp(out, bytes, 251));
    assert(bt_pawr_fragment(&state, 0, 0, 0, 0, bytes, 0, &out, &length) == 1 && length == 0);
    assert(bt_pawr_fragment(&state, 0, 0, 0, 1, bytes, 244, &out, &length) == 0);
    assert(bt_pawr_fragment(&state, 0, 0, 0, 0, bytes, 8, &out, &length) == -1);
    assert(bt_pawr_fragment(&state, 0, 0, 0, 0, bytes, 7, &out, &length) == -1);
    bt_pawr_reassembly_reset(&state);
    assert(bt_pawr_fragment(&state, 0, 0, 0, 1, bytes, 244, &out, &length) == 0);
    assert(bt_pawr_fragment(&state, 0, 0, 0, 255, NULL, 0, &out, &length) == 2);
    assert(bt_pawr_fragment(&state, 0, 0, 0, 0, bytes, 7, &out, &length) == 1 && length == 7);
    assert(bt_pawr_fragment(&state, 0, 0, 1, 1, bytes, 244, &out, &length) == 0);
    assert(bt_pawr_fragment(&state, 0, 0, 2, 0, bytes, 7, &out, &length) == -1);
    bt_pawr_reassembly_reset(&state);
    for (unsigned slot = 0; slot < BT_PAWR_REASSEMBLY_CONTEXTS; slot++)
        assert(bt_pawr_fragment(&state, 0, slot, 0, 1, bytes, 1, &out, &length) == 0);
    assert(bt_pawr_fragment(&state, 0, 99, 0, 1, bytes, 1, &out, &length) == -1);
    bt_pawr_reassembly_reset(&state);
    assert(bt_pawr_fragment(&state, 0, 0, 0, 2, bytes, 1, &out, &length) == -1);
    return 0;
}
