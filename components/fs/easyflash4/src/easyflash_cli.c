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

#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <portable.h>
#ifdef CONFIG_EASYFLASH4_CLI
#include <cli.h>
#endif
#include <easyflash.h>

#define log_info(...)   printf(__VA_ARGS__)
#define log_warn(...)   printf(__VA_ARGS__)
#define log_error(...)  printf(__VA_ARGS__)

static int psm_initialized = 0;

static void psm_init_cmd(char *buf, int len, int argc, char **argv)
{
    if (psm_initialized) {
        printf("PSM already initialized\r\n");
        return;
    }

    EfErrCode ret = easyflash_init();
    if (ret != EF_NO_ERR) {
        printf("PSM init failed, error: %d\r\n", ret);
        return;
    }

    psm_initialized = 1;
    printf("PSM initialized successfully\r\n");
}

static void psm_set_cmd(char *buf, int len, int argc, char **argv)
{
    if (!psm_initialized) {
        printf("PSM not initialized. Run 'psm_init' first.\r\n");
        return;
    }

    if (argc != 3) {
        printf("usage: psm_set <key> <value>\r\n");
        return;
    }
    ef_set_env(argv[1], argv[2]);
    ef_save_env();
    printf("Set %s = %s\r\n", argv[1], argv[2]);
}

static void psm_get_cmd(char *buf, int len, int argc, char **argv)
{
    if (!psm_initialized) {
        printf("PSM not initialized. Run 'psm_init' first.\r\n");
        return;
    }

    if (argc != 2) {
        printf("usage: psm_get <key>\r\n");
        return;
    }

    char *value = ef_get_env(argv[1]);
    if (value) {
        printf("%s = %s\r\n", argv[1], value);
    } else {
        printf("Key '%s' not found\r\n", argv[1]);
    }
}

static void psm_unset_cmd(char *buf, int len, int argc, char **argv)
{
    if (!psm_initialized) {
        printf("PSM not initialized. Run 'psm_init' first.\r\n");
        return;
    }

    if (argc != 2) {
        printf("usage: psm_unset <key>\r\n");
        return;
    }
    ef_del_env(argv[1]);
    ef_save_env();
    printf("Deleted key '%s'\r\n", argv[1]);
}

static void psm_dump_cmd(char *buf, int len, int argc, char **argv)
{
    if (!psm_initialized) {
        printf("PSM not initialized. Run 'psm_init' first.\r\n");
        return;
    }

    printf("=== PSM Environment Variables ===\r\n");
    ef_print_env();
    printf("=== End of PSM dump ===\r\n");
}

static void psm_erase_cmd(char *buf, int len, int argc, char **argv)
{
    if (!psm_initialized) {
        printf("PSM not initialized. Run 'psm_init' first.\r\n");
        return;
    }

    ef_env_set_default();
    printf("PSM erased to default\r\n");
}

static void psm_test_cmd(char *buf, int len, int argc, char **argv)
{
    if (!psm_initialized) {
        printf("PSM not initialized. Run 'psm_init' first.\r\n");
        return;
    }

    const char *def_name = "1234567890123456789012345678901234567890123456789012345678901234";
    uint8_t *data_src = NULL;
    uint8_t *data_buf = NULL;
    uint32_t data_len = 0;
    uint32_t tbuf[] = {1, 1024, 2048, 3978, 3979, 3980};
    uint32_t i, j;

    size_t read_len;
    uint32_t res1, res2;

    for (j = 0; j < sizeof(tbuf)/sizeof(tbuf[0]); j++) {
        data_len = tbuf[j];
        read_len = 0;

        data_src = pvPortMalloc(data_len + 1);
        data_buf = pvPortMalloc(data_len + 1);
        if ((NULL == data_src) || (NULL == data_buf)) {
            log_info("kvbin malloc %lu byte error\r\n", (unsigned long)data_len);
            goto kvbin_exit;
        }

        memset(data_src, 0, data_len + 1);  /* for get string */
        memset(data_buf, 0, data_len + 1);  /* for get string */
        for (i = 0; i < data_len; i++) {
            data_src[i] = 'v';
        }

        /* set */
        res1 = ef_set_env_blob(def_name, data_src, data_len);

        /* get */
        res2 = ef_get_env_blob(def_name, data_buf, data_len, &read_len);
        if ((res1 != 0) || (res2 != data_len)) {
            log_warn("kvbin set/get %lu byte error, res1 = %lu, res2 = %lu.\r\n",
                     (unsigned long)data_len, (unsigned long)res1, (unsigned long)res2);
            goto kvbin_exit;
        }

        if (memcmp(data_buf, data_src, data_len) != 0) {
            log_error("kvbin set/get %lu byte, res1 = %lu, res2 = %lu. memcmp error\r\n",
                      (unsigned long)data_len, (unsigned long)res1, (unsigned long)res2);
            goto kvbin_exit;
        }

        log_info("kvbin set %lu byte bin -> read %lu byte -> memcmp success.\r\n",
                 (unsigned long)data_len, (unsigned long)data_len);
kvbin_exit:
        if (data_src) {
            vPortFree(data_src);
            data_src = NULL;
        }
        if (data_buf) {
            vPortFree(data_buf);
            data_buf = NULL;
        }
    }

    ef_del_env(def_name);
    printf("PSM test completed\r\n");
}

#ifdef CONFIG_EASYFLASH4_CLI
/* STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static */
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "psm_init", "psm init", psm_init_cmd },
    { "psm_set", "psm set <key> <value>", psm_set_cmd },
    { "psm_get", "psm get <key>", psm_get_cmd },
    { "psm_unset", "psm unset <key>", psm_unset_cmd },
    { "psm_dump", "psm dump all", psm_dump_cmd },
    { "psm_erase", "psm erase to default", psm_erase_cmd },
    { "psm_test", "psm test", psm_test_cmd },
};
#endif

int easyflash_cli_init(void)
{
    /* static command(s) do NOT need to call aos_cli_register_command(s) to register. */
    return 0;
}
