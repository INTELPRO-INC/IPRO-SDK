/* SPDX-License-Identifier: Apache-2.0 */
/* Opt-in BLE 5.3 peripheral channel-classification test control. */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "ipro_ble_ctlr_channel_classification_test.h"
#include "ipro_log.h"
#include "shell.h"

#define TAG "CH_CLASS_TEST"

static int command(int argc, char **argv)
{
    int rc;

    if ((argc == 2) && (strcmp(argv[1], "clear") == 0)) {
        rc = ipro_ble_ctlr_ch_class_test_clear();
        IPRO_LOGI(TAG, "PERIPHERAL clear rc=%d controller_reads=%lu", rc,
                  (unsigned long)ipro_ble_ctlr_ch_class_test_get_count());
        return rc;
    }
    if ((argc == 2) && (strcmp(argv[1], "status") == 0)) {
        IPRO_LOGI(TAG, "PERIPHERAL status controller_reads=%lu",
                  (unsigned long)ipro_ble_ctlr_ch_class_test_get_count());
        return 0;
    }
    if ((argc == 3) && (strcmp(argv[1], "bad") == 0)) {
        char *end = NULL;
        unsigned long channel;

        errno = 0;
        channel = strtoul(argv[2], &end, 10);
        if ((errno != 0) || (end == argv[2]) || (*end != '\0') ||
            (channel > 36U)) {
            return -EINVAL;
        }
        rc = ipro_ble_ctlr_ch_class_test_set_bad((uint8_t)channel);
        IPRO_LOGI(TAG, "PERIPHERAL force_bad=%lu rc=%d controller_reads=%lu",
                  channel, rc,
                  (unsigned long)ipro_ble_ctlr_ch_class_test_get_count());
        return rc;
    }
    return -EINVAL;
}

SHELL_CMD_EXPORT_ALIAS(command, chclass, BLE 5.3 peripheral channel classification test);
