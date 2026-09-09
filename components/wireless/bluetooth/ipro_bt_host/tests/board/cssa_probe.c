/* SPDX-License-Identifier: Apache-2.0 */
/* Opt-in bench commands, not part of the customer Host library. */
#include <errno.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include "ipro_log.h"
#include "shell.h"
static struct bt_le_ext_adv *cssa_adv;
static int cssa_command(int argc, char **argv)
{
    if (argc != 2) return -EINVAL;
    int rc = 0;
    if (!strcmp(argv[1], "caps")) {
        struct bt_le_local_features features;
        rc = bt_le_get_local_features(&features);
        if (!rc) IPRO_LOGI("CSSA_TEST", "caps controller=%u coded=%u",
            !!BT_FEAT_LE_ADV_CODING_SEL(features.features),
            !!BT_FEAT_LE_PHY_CODED(features.features));
    } else if (!strcmp(argv[1], "stop")) {
        if (cssa_adv) {
            rc = bt_le_ext_adv_stop(cssa_adv);
            if (!rc) rc = bt_le_ext_adv_delete(cssa_adv);
            if (!rc) cssa_adv = NULL;
        }
    } else if (!strcmp(argv[1], "s2") || !strcmp(argv[1], "s8")) {
        if (cssa_adv) return -EALREADY;
        struct bt_le_adv_param param = {
            .id = BT_ID_DEFAULT, .sid = 4,
            .options = BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_CODED |
                       BT_LE_ADV_OPT_USE_IDENTITY |
                       (!strcmp(argv[1], "s2") ? BT_LE_ADV_OPT_REQUIRE_S2_CODING :
                                                BT_LE_ADV_OPT_REQUIRE_S8_CODING),
            .interval_min = 320, .interval_max = 320,
        };
        static const uint8_t name[] = "IPRO-CSSA";
        struct bt_data data = BT_DATA(BT_DATA_NAME_COMPLETE, name, sizeof(name) - 1);
        rc = bt_le_ext_adv_create(&param, NULL, &cssa_adv);
        if (!rc) rc = bt_le_ext_adv_set_data(cssa_adv, &data, 1, NULL, 0);
        if (!rc) rc = bt_le_ext_adv_start(cssa_adv, BT_LE_EXT_ADV_START_DEFAULT);
        if (rc && cssa_adv) {
            (void)bt_le_ext_adv_stop(cssa_adv);
            if (!bt_le_ext_adv_delete(cssa_adv)) cssa_adv = NULL;
        }
    } else return -EINVAL;
    IPRO_LOGI("CSSA_TEST", "%s rc=%d", argv[1], rc);
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cssa_command, cssa, Coding selection bench);
