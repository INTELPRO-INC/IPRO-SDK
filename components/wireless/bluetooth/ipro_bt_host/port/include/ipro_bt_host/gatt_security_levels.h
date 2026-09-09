/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_GATT_SECURITY_LEVELS_H_
#define IPRO_BT_HOST_GATT_SECURITY_LEVELS_H_
#include <stddef.h>
#include <stdint.h>

struct ipro_bt_gatt_security_requirement {
    uint8_t mode;
    uint8_t level;
};

int ipro_bt_gatt_security_levels_encode(
    const struct ipro_bt_gatt_security_requirement *requirements,
    size_t count, uint8_t *output, size_t output_size);

/* Configure the static characteristic value before bt_enable(). */
int ipro_bt_gatt_security_levels_set(
    const struct ipro_bt_gatt_security_requirement *requirements,
    size_t count);
#endif
