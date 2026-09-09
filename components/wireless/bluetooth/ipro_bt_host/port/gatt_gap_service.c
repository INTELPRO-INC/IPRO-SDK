/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/sys/byteorder.h>
#include "ipro_bt_host/gatt_security_levels.h"

#define SECURITY_REQUIREMENTS_MAX 16
static uint8_t security_levels[SECURITY_REQUIREMENTS_MAX * 2] = {1, 1};
static uint16_t security_levels_length = 2;

/* Referenced by the HCI port so the static-service iterable is not discarded
 * when ipro_bt_host is linked as an archive. */
void ipro_bt_gatt_gap_service_link_anchor(void) {}

int ipro_bt_gatt_security_levels_set(
    const struct ipro_bt_gatt_security_requirement *requirements, size_t count)
{
    uint8_t encoded[sizeof(security_levels)];
    if (bt_is_ready()) return -EBUSY;
    int length = ipro_bt_gatt_security_levels_encode(requirements, count,
                                                      encoded, sizeof(encoded));
    if (length < 0) return length;
    memcpy(security_levels, encoded, (size_t)length);
    security_levels_length = (uint16_t)length;
    return 0;
}

static ssize_t read_name(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, uint16_t len, uint16_t offset)
{
    const char *name = bt_get_name();
    return bt_gatt_attr_read(conn, attr, buf, len, offset, name, strlen(name));
}

static ssize_t read_appearance(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len, uint16_t offset)
{
    uint8_t appearance[2];
    sys_put_le16(CONFIG_BT_DEVICE_APPEARANCE, appearance);
    return bt_gatt_attr_read(conn, attr, buf, len, offset, appearance, sizeof(appearance));
}

static ssize_t read_security_levels(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                    void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, security_levels,
                             security_levels_length);
}

BT_GATT_SERVICE_DEFINE(ipro_gap_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_GAP),
    BT_GATT_CHARACTERISTIC(BT_UUID_GAP_DEVICE_NAME, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, read_name, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_GAP_APPEARANCE, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, read_appearance, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_GATT_SL, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, read_security_levels, NULL, NULL)
);
