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
#include <errno.h>
#include <stdint.h>
#include "ipro_bt_host/gatt_security_levels.h"

int main(void)
{
    const struct ipro_bt_gatt_security_requirement valid[] = {
        {1, 1}, {1, 4}, {2, 1}, {2, 2},
    };
    uint8_t encoded[sizeof(valid)] = {0};
    assert(ipro_bt_gatt_security_levels_encode(valid, 4, encoded, sizeof(encoded)) == 8);
    for (unsigned i = 0; i < 4; i++) {
        assert(encoded[i * 2] == valid[i].mode);
        assert(encoded[i * 2 + 1] == valid[i].level);
    }
    assert(ipro_bt_gatt_security_levels_encode(NULL, 1, encoded, sizeof(encoded)) == -EINVAL);
    assert(ipro_bt_gatt_security_levels_encode(valid, 0, encoded, sizeof(encoded)) == -EINVAL);
    assert(ipro_bt_gatt_security_levels_encode(valid, 4, NULL, sizeof(encoded)) == -EINVAL);
    assert(ipro_bt_gatt_security_levels_encode(valid, 4, encoded, 7) == -ENOSPC);
    const struct ipro_bt_gatt_security_requirement bad[] = {
        {0, 1}, {1, 0}, {1, 5}, {2, 0}, {2, 3}, {3, 1},
    };
    for (unsigned i = 0; i < sizeof(bad) / sizeof(bad[0]); i++)
        assert(ipro_bt_gatt_security_levels_encode(&bad[i], 1, encoded, sizeof(encoded)) == -EINVAL);
    return 0;
}
