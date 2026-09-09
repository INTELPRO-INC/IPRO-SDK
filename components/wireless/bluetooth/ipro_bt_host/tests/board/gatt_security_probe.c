/* SPDX-License-Identifier: Apache-2.0 */
/* Opt-in GATT Security Levels (UUID 0x2BF5) over-the-air test fixture. */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/att.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include "ipro_log.h"
#include "shell.h"

#define TAG "GSL_TEST"
#define READ_TIMEOUT_MS 3000U

#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_POLICY_TEST)
static const uint8_t expected_value[] = {1U, 4U};
#else
static const uint8_t expected_value[] = {1U, 4U, 2U, 2U};
#endif
static struct bt_gatt_read_params read_params;
static volatile bool read_done;
static volatile bool read_active;
static uint8_t read_error;
static uint8_t read_value[4U];
static uint16_t read_length;
static uint16_t value_count;
static uint32_t pass_count;
static uint32_t fail_count;

#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_POLICY_TEST)
#define GSL_POLICY_UUID_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x1f87e9b1, 0x4d8f, 0x49b8, 0xa2c1, 0x4f7786bdc201)
#define GSL_POLICY_UUID_VALUE_VAL \
    BT_UUID_128_ENCODE(0x1f87e9b2, 0x4d8f, 0x49b8, 0xa2c1, 0x4f7786bdc201)

static struct bt_uuid_128 gsl_policy_value_uuid =
    BT_UUID_INIT_128(GSL_POLICY_UUID_VALUE_VAL);
static struct bt_uuid_128 gsl_policy_service_uuid =
    BT_UUID_INIT_128(GSL_POLICY_UUID_SERVICE_VAL);
static const uint8_t gsl_policy_value[] = {
    0x49U, 0x50U, 0x52U, 0x4fU, 0x2dU, 0x53U, 0x43U, 0x34U,
};
static struct bt_gatt_read_params policy_read_params;
static volatile bool policy_read_done;
static uint8_t policy_read_error;
static uint8_t policy_read_value[sizeof(gsl_policy_value)];
static uint16_t policy_read_length;

static ssize_t read_policy_value(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, gsl_policy_value,
                             sizeof(gsl_policy_value));
}

BT_GATT_SERVICE_DEFINE(gsl_policy_service,
    BT_GATT_PRIMARY_SERVICE(&gsl_policy_service_uuid.uuid),
    BT_GATT_CHARACTERISTIC(&gsl_policy_value_uuid.uuid, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_READ_LESC,
                           read_policy_value, NULL, NULL)
);
#endif

static void pick_connected_le(struct bt_conn *conn, void *user_data)
{
    struct bt_conn **result = user_data;
    struct bt_conn_info info;

    if ((*result == NULL) && (bt_conn_get_info(conn, &info) == 0) &&
        (info.state == BT_CONN_STATE_CONNECTED)) {
        *result = bt_conn_ref(conn);
    }
}

static uint8_t read_callback(struct bt_conn *conn, uint8_t err,
                             struct bt_gatt_read_params *params,
                             const void *data, uint16_t length)
{
    (void)conn;
    (void)params;

    if (err != 0U) {
        /* Read By Type terminates a successful range walk with Attribute Not
         * Found once no later matching UUID exists.  It is success only after
         * at least one value was returned; the same error on the first request
         * still means that the mandatory test characteristic is absent. */
        if (!((err == BT_ATT_ERR_ATTRIBUTE_NOT_FOUND) && (value_count > 0U))) {
            read_error = err;
        }
        read_done = true;
        return BT_GATT_ITER_STOP;
    }
    if (data == NULL) {
        read_done = true;
        return BT_GATT_ITER_STOP;
    }

    value_count++;
    if ((value_count != 1U) || (length > sizeof(read_value))) {
        read_error = BT_ATT_ERR_INVALID_ATTRIBUTE_LEN;
        read_done = true;
        return BT_GATT_ITER_STOP;
    }

    memcpy(read_value, data, length);
    read_length = length;
    return BT_GATT_ITER_CONTINUE;
}

static int read_security_levels(void)
{
    struct bt_conn *conn = NULL;
    uint32_t waited_ms = 0U;
    int rc;

    if (read_active) {
        return -EBUSY;
    }
    bt_conn_foreach(BT_CONN_TYPE_LE, pick_connected_le, &conn);
    if (conn == NULL) {
        return -ENOTCONN;
    }

    read_active = true;
    read_done = false;
    read_error = 0U;
    read_length = 0U;
    value_count = 0U;
    memset(read_value, 0, sizeof(read_value));
    memset(&read_params, 0, sizeof(read_params));
    read_params.func = read_callback;
    read_params.handle_count = 0U;
    read_params.by_uuid.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
    read_params.by_uuid.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
    read_params.by_uuid.uuid = BT_UUID_GATT_SL;

    rc = bt_gatt_read(conn, &read_params);
    if (rc == 0) {
        while (!read_done && (waited_ms < READ_TIMEOUT_MS)) {
            k_sleep(K_MSEC(10));
            waited_ms += 10U;
        }
        if (!read_done) {
            rc = -ETIMEDOUT;
        } else if (read_error != 0U) {
            rc = -EIO;
        } else if ((value_count != 1U) ||
                   (read_length != sizeof(expected_value)) ||
                   (memcmp(read_value, expected_value, sizeof(expected_value)) != 0)) {
            rc = -EBADMSG;
        }
    }

    if (rc == 0) {
        pass_count++;
    } else {
        fail_count++;
    }
    IPRO_LOGI(TAG,
              "READ %s rc=%d att_err=0x%02x sec=%u values=%u len=%u data=%02x%02x%02x%02x pass=%lu fail=%lu",
              (rc == 0) ? "PASS" : "FAIL", rc, read_error,
              (unsigned int)bt_conn_get_security(conn), value_count, read_length,
              read_value[0], read_value[1], read_value[2], read_value[3],
              (unsigned long)pass_count, (unsigned long)fail_count);
    bt_conn_unref(conn);
    read_active = false;
    return rc;
}

#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_POLICY_TEST)
static uint8_t policy_read_callback(struct bt_conn *conn, uint8_t err,
                                    struct bt_gatt_read_params *params,
                                    const void *data, uint16_t length)
{
    (void)conn;
    (void)params;
    policy_read_error = err;
    if ((err == 0U) && (data != NULL)) {
        if (length <= sizeof(policy_read_value)) {
            memcpy(policy_read_value, data, length);
            policy_read_length = length;
        } else {
            policy_read_error = BT_ATT_ERR_INVALID_ATTRIBUTE_LEN;
        }
    }
    policy_read_done = true;
    return BT_GATT_ITER_STOP;
}

static int read_policy_characteristic(void)
{
    struct bt_conn *conn = NULL;
    uint32_t waited_ms = 0U;
    int rc;
    bool exact;

    bt_conn_foreach(BT_CONN_TYPE_LE, pick_connected_le, &conn);
    if (conn == NULL) {
        return -ENOTCONN;
    }
    policy_read_done = false;
    policy_read_error = 0U;
    policy_read_length = 0U;
    memset(policy_read_value, 0, sizeof(policy_read_value));
    memset(&policy_read_params, 0, sizeof(policy_read_params));
    policy_read_params.func = policy_read_callback;
    policy_read_params.handle_count = 0U;
    policy_read_params.by_uuid.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
    policy_read_params.by_uuid.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
    policy_read_params.by_uuid.uuid = &gsl_policy_value_uuid.uuid;
    rc = bt_gatt_read(conn, &policy_read_params);
    if (rc == 0) {
        while (!policy_read_done && (waited_ms < READ_TIMEOUT_MS)) {
            k_sleep(K_MSEC(10));
            waited_ms += 10U;
        }
        if (!policy_read_done) {
            rc = -ETIMEDOUT;
        }
    }
    exact = (policy_read_error == 0U) &&
            (policy_read_length == sizeof(gsl_policy_value)) &&
            (memcmp(policy_read_value, gsl_policy_value,
                    sizeof(gsl_policy_value)) == 0);
    IPRO_LOGI(TAG,
              "PROTECTED rc=%d att_err=0x%02x sec=%u len=%u exact=%u data=%02x%02x%02x%02x%02x%02x%02x%02x",
              rc, policy_read_error, (unsigned int)bt_conn_get_security(conn),
              policy_read_length, exact ? 1U : 0U,
              policy_read_value[0], policy_read_value[1],
              policy_read_value[2], policy_read_value[3],
              policy_read_value[4], policy_read_value[5],
              policy_read_value[6], policy_read_value[7]);
    bt_conn_unref(conn);
    return rc;
}

static int set_security(unsigned long level)
{
    struct bt_conn *conn = NULL;
    int rc;

    if ((level < BT_SECURITY_L1) || (level > BT_SECURITY_L4)) {
        return -EINVAL;
    }
    bt_conn_foreach(BT_CONN_TYPE_LE, pick_connected_le, &conn);
    if (conn == NULL) {
        return -ENOTCONN;
    }
    rc = bt_conn_set_security(conn, (bt_security_t)level);
    IPRO_LOGI(TAG, "SECURITY request=%lu rc=%d current=%u", level, rc,
              (unsigned int)bt_conn_get_security(conn));
    bt_conn_unref(conn);
    return rc;
}
#endif

static int command(int argc, char **argv)
{
    if ((argc == 2) && (strcmp(argv[1], "read") == 0)) {
        return read_security_levels();
    }
    if ((argc == 2) && (strcmp(argv[1], "status") == 0)) {
        IPRO_LOGI(TAG, "STATUS active=%u pass=%lu fail=%lu",
                  read_active ? 1U : 0U,
                  (unsigned long)pass_count, (unsigned long)fail_count);
        return 0;
    }
#if defined(CONFIG_IPRO_BT_HOST_GATT_SECURITY_POLICY_TEST)
    if ((argc == 2) && (strcmp(argv[1], "protected") == 0)) {
        return read_policy_characteristic();
    }
    if ((argc == 3) && (strcmp(argv[1], "secure") == 0)) {
        char *end = NULL;
        unsigned long level;

        errno = 0;
        level = strtoul(argv[2], &end, 10);
        if ((errno != 0) || (end == argv[2]) || (*end != '\0')) {
            return -EINVAL;
        }
        return set_security(level);
    }
    if ((argc == 2) && (strcmp(argv[1], "unpair") == 0)) {
        int rc = bt_unpair(BT_ID_DEFAULT, NULL);

        IPRO_LOGI(TAG, "UNPAIR rc=%d", rc);
        return rc;
    }
#endif
    return -EINVAL;
}

SHELL_CMD_EXPORT_ALIAS(command, gsl, GATT Security Levels read test);
