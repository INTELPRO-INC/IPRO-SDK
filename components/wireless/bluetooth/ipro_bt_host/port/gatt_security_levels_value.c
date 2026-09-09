/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include "ipro_bt_host/gatt_security_levels.h"

static int requirement_valid(const struct ipro_bt_gatt_security_requirement *requirement)
{
    return (requirement->mode == 1 && requirement->level >= 1 && requirement->level <= 4) ||
           (requirement->mode == 2 && requirement->level >= 1 && requirement->level <= 2);
}

int ipro_bt_gatt_security_levels_encode(
    const struct ipro_bt_gatt_security_requirement *requirements,
    size_t count, uint8_t *output, size_t output_size)
{
    if (!requirements || !count || !output) return -EINVAL;
    if (count > output_size / 2) return -ENOSPC;
    for (size_t i = 0; i < count; i++) {
        if (!requirement_valid(&requirements[i])) return -EINVAL;
    }
    for (size_t i = 0; i < count; i++) {
        output[i * 2] = requirements[i].mode;
        output[i * 2 + 1] = requirements[i].level;
    }
    return (int)(count * 2);
}
