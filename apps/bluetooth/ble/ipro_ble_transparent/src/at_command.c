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

/**
 * @file at_command.c
 * @brief AT command parser for BLE transparent module
 */

#include "at_command.h"
#include "ble_transparent.h"
#include "uart_bridge.h"
#include "app_config.h"
#include "ipro_log.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>

#define LOG_TAG "AT_CMD"

static at_resp_fn g_resp_fn;

static void respond(const char *fmt, ...)
{
    char buf[AT_RESP_MAX_LEN];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (g_resp_fn) {
        g_resp_fn(buf);
    } else {
        uart_bridge_send_str(buf);
    }
}

/* --- Hex utility --- */
static int hex_char_to_nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static int hex_str_to_bytes(const char *hex, uint8_t *out, uint16_t max_len)
{
    uint16_t len = 0;
    while (*hex && *(hex + 1) && len < max_len) {
        int hi = hex_char_to_nibble(*hex);
        int lo = hex_char_to_nibble(*(hex + 1));
        if (hi < 0 || lo < 0) return -1;
        out[len++] = (uint8_t)((hi << 4) | lo);
        hex += 2;
    }
    if (*hex) return -1; /* Odd length */
    return (int)len;
}

/* ==================== AT command handlers ==================== */

static int cmd_at(const char *param)
{
    respond("OK\r\n");
    return 0;
}

static int cmd_reset(const char *param)
{
    respond("OK\r\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    extern void hal_sys_reset(void);
    hal_sys_reset();
    return 0;
}

static int cmd_version_query(const char *param)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    ble_transparent_get_addr(addr_str, sizeof(addr_str));
    respond("+VERSION:%s,%s\r\nOK\r\n", APP_VERSION, addr_str);
    return 0;
}

static int cmd_restore(const char *param)
{
#ifdef CONFIG_EASYFLASH4
    ef_env_set_default();
#endif
    respond("OK\r\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    extern void hal_sys_reset(void);
    hal_sys_reset();
    return 0;
}

static int cmd_name_set(const char *param)
{
    if (!param || strlen(param) == 0) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_name(param);
    if (ret) {
        respond("ERROR\r\n");
        return -1;
    }

#ifdef CONFIG_EASYFLASH4
    ef_set_env(EF_KEY_DEVICE_NAME, param);
#endif

    respond("OK\r\n");
    return 0;
}

static int cmd_name_query(const char *param)
{
    respond("+NAME:%s\r\nOK\r\n", ble_transparent_get_name());
    return 0;
}

static int cmd_adv_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    int val = atoi(param);
    int ret;
    if (val) {
        ret = ble_transparent_adv_start();
    } else {
        ret = ble_transparent_adv_stop();
    }

    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_connint_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    unsigned int min_val, max_val;
    if (sscanf(param, "%u,%u", &min_val, &max_val) != 2) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_conn_params((uint16_t)min_val, (uint16_t)max_val);
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_mtu_query(const char *param)
{
    uint16_t mtu = ble_transparent_get_mtu();
    respond("+MTU:%u\r\nOK\r\n", mtu);
    return 0;
}

static int cmd_state_query(const char *param)
{
    respond("+STATE:%d\r\nOK\r\n", (int)ble_transparent_get_state());
    return 0;
}

static int cmd_disc(const char *param)
{
    int ret = ble_transparent_disconnect();
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_mode_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    int val = atoi(param);
    if (val != 0 && val != 1) {
        respond("ERROR\r\n");
        return -1;
    }

    respond("OK\r\n");
    uart_bridge_set_mode(val ? BRIDGE_MODE_DATA : BRIDGE_MODE_COMMAND);
    return 0;
}

static int cmd_mode_query(const char *param)
{
    respond("+MODE:%d\r\nOK\r\n", (int)uart_bridge_get_mode());
    return 0;
}

static int cmd_baud_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    uint32_t baud = (uint32_t)atoi(param);
    if (baud < 9600 || baud > 921600) {
        respond("ERROR\r\n");
        return -1;
    }

    respond("OK\r\n");
    vTaskDelay(pdMS_TO_TICKS(50)); /* Let response flush */
    uart_bridge_set_baudrate(baud);

#ifdef CONFIG_EASYFLASH4
    char buf[16];
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)baud);
    ef_set_env(EF_KEY_UART_BAUDRATE, buf);
#endif

    return 0;
}

static int cmd_baud_query(const char *param)
{
    respond("+BAUD:%lu\r\nOK\r\n", (unsigned long)uart_bridge_get_baudrate());
    return 0;
}

/* --- Phase 1: Device info & link quality --- */

static int cmd_addr_query(const char *param)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    if (ble_transparent_get_addr(addr_str, sizeof(addr_str))) {
        respond("ERROR\r\n");
        return -1;
    }
    respond("+ADDR:%s\r\nOK\r\n", addr_str);
    return 0;
}

static int cmd_rssi_query(const char *param)
{
    int8_t rssi;
    if (ble_transparent_get_rssi(&rssi)) {
        respond("ERROR\r\n");
        return -1;
    }
    respond("+RSSI:%d\r\nOK\r\n", (int)rssi);
    return 0;
}

static int cmd_conninfo_query(const char *param)
{
    uint16_t interval, latency, timeout, mtu;
    if (ble_transparent_get_conn_info(&interval, &latency, &timeout, &mtu)) {
        respond("ERROR\r\n");
        return -1;
    }
    respond("+CONNINFO:%u,%u,%u,%u\r\nOK\r\n", interval, latency, timeout, mtu);
    return 0;
}

static int cmd_txpwr_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    int val = atoi(param);
    if (val < -20 || val > 20) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_tx_power((int8_t)val);
    if (ret) {
        respond("ERROR\r\n");
        return -1;
    }

#ifdef CONFIG_EASYFLASH4
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", val);
    ef_set_env(EF_KEY_TX_POWER, buf);
#endif

    respond("OK\r\n");
    return 0;
}

static int cmd_txpwr_query(const char *param)
{
    respond("+TXPWR:%d\r\nOK\r\n", (int)ble_transparent_get_tx_power());
    return 0;
}

static int cmd_send(const char *param)
{
    if (!param || strlen(param) == 0) {
        respond("ERROR\r\n");
        return -1;
    }

    uint8_t data[BLE_TX_BUF_SIZE];
    int len = hex_str_to_bytes(param, data, sizeof(data));
    if (len <= 0) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_send(data, (uint16_t)len);
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_advint_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    unsigned int val = (unsigned int)atoi(param);
    int ret = ble_transparent_set_adv_interval((uint16_t)val);
    if (ret) {
        respond("ERROR\r\n");
        return -1;
    }

#ifdef CONFIG_EASYFLASH4
    char buf[8];
    snprintf(buf, sizeof(buf), "%u", val);
    ef_set_env(EF_KEY_ADV_INTERVAL, buf);
#endif

    respond("OK\r\n");
    return 0;
}

static int cmd_advint_query(const char *param)
{
    respond("+ADVINT:%u\r\nOK\r\n", ble_transparent_get_adv_interval());
    return 0;
}

static int cmd_sleep(const char *param)
{
    respond("OK\r\n");
    vTaskDelay(pdMS_TO_TICKS(50));
    ble_transparent_enter_sleep();
    return 0;
}

/* --- Phase 2: Security & pairing --- */

static int cmd_bond_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    int val = atoi(param);
    if (val != 0 && val != 1) {
        respond("ERROR\r\n");
        return -1;
    }

    ble_transparent_set_bondable(val ? true : false);

#ifdef CONFIG_EASYFLASH4
    ef_set_env(EF_KEY_BONDABLE, val ? "1" : "0");
#endif

    respond("OK\r\n");
    return 0;
}

static int cmd_bond_query(const char *param)
{
    char buf[512];
    int count = ble_transparent_get_bonds(buf, sizeof(buf));
    if (count > 0) {
        respond("%sOK\r\n", buf);
    } else {
        respond("+BOND:0\r\nOK\r\n");
    }
    return 0;
}

static int cmd_bondclr(const char *param)
{
    int ret = ble_transparent_clear_bonds();
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_pin_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    unsigned int val = (unsigned int)atoi(param);
    if (val > 999999) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_passkey(val);
    if (ret) {
        respond("ERROR\r\n");
        return -1;
    }

#ifdef CONFIG_EASYFLASH4
    char buf[8];
    snprintf(buf, sizeof(buf), "%u", val);
    ef_set_env(EF_KEY_PASSKEY, buf);
#endif

    respond("OK\r\n");
    return 0;
}

static int cmd_pin_query(const char *param)
{
    respond("+PIN:%06u\r\nOK\r\n", (unsigned int)ble_transparent_get_passkey());
    return 0;
}

static int cmd_sec_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    int val = atoi(param);
    if (val < 1 || val > 4) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_security((uint8_t)val);
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

/* --- Phase 3: Advanced --- */

static int cmd_phy_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    uint8_t phy;
    if (strcasecmp(param, "1M") == 0) {
        phy = 0x01;
    } else if (strcasecmp(param, "2M") == 0) {
        phy = 0x02;
    } else if (strcasecmp(param, "CODED") == 0) {
        phy = 0x04;
    } else {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_phy(phy);
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_phy_query(const char *param)
{
    uint8_t tx_phy, rx_phy;
    if (ble_transparent_get_phy(&tx_phy, &rx_phy)) {
        respond("ERROR\r\n");
        return -1;
    }

    static const char *phy_str[] = { "?", "1M", "2M", "?", "CODED" };
    const char *tx_s = (tx_phy < 5) ? phy_str[tx_phy] : "?";
    const char *rx_s = (rx_phy < 5) ? phy_str[rx_phy] : "?";
    respond("+PHY:%s,%s\r\nOK\r\n", tx_s, rx_s);
    return 0;
}

static int cmd_dle_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    unsigned int octets, time;
    if (sscanf(param, "%u,%u", &octets, &time) != 2) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_data_length((uint16_t)octets, (uint16_t)time);
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_advdata_set(const char *param)
{
    if (!param || strlen(param) == 0) {
        respond("ERROR\r\n");
        return -1;
    }

    uint8_t data[31];
    int len = hex_str_to_bytes(param, data, sizeof(data));
    if (len <= 0) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_adv_data(data, (uint16_t)len);
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_srdata_set(const char *param)
{
    if (!param || strlen(param) == 0) {
        respond("ERROR\r\n");
        return -1;
    }

    uint8_t data[31];
    int len = hex_str_to_bytes(param, data, sizeof(data));
    if (len <= 0) {
        respond("ERROR\r\n");
        return -1;
    }

    int ret = ble_transparent_set_scan_rsp_data(data, (uint16_t)len);
    respond(ret == 0 ? "OK\r\n" : "ERROR\r\n");
    return ret;
}

static int cmd_gpio_set(const char *param)
{
    if (!param) {
        respond("ERROR\r\n");
        return -1;
    }

    unsigned int pin, val;
    if (sscanf(param, "%u,%u", &pin, &val) == 2) {
        extern void gpio_write(uint32_t pin, uint32_t value);
        gpio_write(pin, val);
        respond("OK\r\n");
        return 0;
    }

    if (sscanf(param, "%u", &pin) == 1) {
        extern int gpio_read(uint32_t pin);
        int v = gpio_read(pin);
        respond("+GPIO:%d\r\nOK\r\n", v);
        return 0;
    }

    respond("ERROR\r\n");
    return -1;
}

/* ==================== Command table ==================== */

typedef struct {
    const char *name;
    int (*handler)(const char *param);
    int (*query)(const char *param);
} at_cmd_entry_t;

static const at_cmd_entry_t cmd_table[] = {
    /* Basic */
    { "RESET",     cmd_reset,        NULL },
    { "VERSION",   NULL,             cmd_version_query },
    { "RESTORE",   cmd_restore,      NULL },
    { "NAME",      cmd_name_set,     cmd_name_query },
    { "BAUD",      cmd_baud_set,     cmd_baud_query },
    /* BLE control */
    { "ADV",       cmd_adv_set,      NULL },
    { "ADVINT",    cmd_advint_set,   cmd_advint_query },
    { "ADVDATA",   cmd_advdata_set,  NULL },
    { "SRDATA",    cmd_srdata_set,   NULL },
    { "TXPWR",     cmd_txpwr_set,    cmd_txpwr_query },
    { "STATE",     NULL,             cmd_state_query },
    /* Connection */
    { "CONNINT",   cmd_connint_set,  NULL },
    { "CONNINFO",  NULL,             cmd_conninfo_query },
    { "MTU",       NULL,             cmd_mtu_query },
    { "DISC",      cmd_disc,         NULL },
    { "RSSI",      NULL,             cmd_rssi_query },
    /* Data transfer */
    { "MODE",      cmd_mode_set,     cmd_mode_query },
    { "SEND",      cmd_send,         NULL },
    /* Device info */
    { "ADDR",      NULL,             cmd_addr_query },
    /* Security */
    { "BONDCLR",   cmd_bondclr,      NULL },
    { "BOND",      cmd_bond_set,     cmd_bond_query },
    { "PIN",       cmd_pin_set,      cmd_pin_query },
    { "SEC",       cmd_sec_set,      NULL },
    /* RF parameters */
    { "PHY",       cmd_phy_set,      cmd_phy_query },
    { "DLE",       cmd_dle_set,      NULL },
    /* System */
    { "SLEEP",     cmd_sleep,        NULL },
    { "GPIO",      cmd_gpio_set,     NULL },
};

#define CMD_TABLE_SIZE (sizeof(cmd_table) / sizeof(cmd_table[0]))

int at_command_init(at_resp_fn resp_fn)
{
    g_resp_fn = resp_fn;
    IPRO_LOGI(LOG_TAG, "AT command parser initialized (%d commands)", (int)CMD_TABLE_SIZE);
    return 0;
}

int at_command_process(const char *line)
{
    if (!line) return -1;

    /* Skip leading whitespace */
    while (*line == ' ' || *line == '\t') line++;

    /* Check bare "AT" */
    if (strcasecmp(line, "AT") == 0) {
        return cmd_at(NULL);
    }

    /* Must start with "AT+" */
    if (strncasecmp(line, "AT+", 3) != 0) {
        respond("ERROR\r\n");
        return -1;
    }

    const char *cmd_start = line + 3;

    for (size_t i = 0; i < CMD_TABLE_SIZE; i++) {
        size_t name_len = strlen(cmd_table[i].name);

        if (strncasecmp(cmd_start, cmd_table[i].name, name_len) != 0) {
            continue;
        }

        char next = cmd_start[name_len];

        /* Query: AT+CMD? */
        if (next == '?' && cmd_table[i].query) {
            return cmd_table[i].query(NULL);
        }

        /* Set: AT+CMD=param */
        if (next == '=' && cmd_table[i].handler) {
            return cmd_table[i].handler(cmd_start + name_len + 1);
        }

        /* Execute: AT+CMD (no param) */
        if (next == '\0' && cmd_table[i].handler) {
            return cmd_table[i].handler(NULL);
        }
    }

    respond("ERROR\r\n");
    return -1;
}

int at_command_load_config(void)
{
#ifdef CONFIG_EASYFLASH4
    char *val;

    val = ef_get_env(EF_KEY_DEVICE_NAME);
    if (val && strlen(val) > 0) {
        ble_transparent_set_name(val);
        IPRO_LOGI(LOG_TAG, "Loaded name: %s", val);
    }

    val = ef_get_env(EF_KEY_UART_BAUDRATE);
    if (val && strlen(val) > 0) {
        uint32_t baud = (uint32_t)atoi(val);
        if (baud >= 9600 && baud <= 921600) {
            uart_bridge_set_baudrate(baud);
            IPRO_LOGI(LOG_TAG, "Loaded baud: %lu", (unsigned long)baud);
        }
    }

    val = ef_get_env(EF_KEY_TX_POWER);
    if (val && strlen(val) > 0) {
        ble_transparent_set_tx_power((int8_t)atoi(val));
    }

    val = ef_get_env(EF_KEY_ADV_INTERVAL);
    if (val && strlen(val) > 0) {
        unsigned int ms = (unsigned int)atoi(val);
        if (ms >= 20 && ms <= 10240) {
            ble_transparent_set_adv_interval((uint16_t)ms);
        }
    }

    val = ef_get_env(EF_KEY_PASSKEY);
    if (val && strlen(val) > 0) {
        unsigned int pk = (unsigned int)atoi(val);
        if (pk <= 999999) {
            ble_transparent_set_passkey(pk);
        }
    }

    val = ef_get_env(EF_KEY_BONDABLE);
    if (val && strlen(val) > 0) {
        ble_transparent_set_bondable(atoi(val) ? true : false);
    }
#endif
    return 0;
}

int at_command_save_config(void)
{
#ifdef CONFIG_EASYFLASH4
    ef_set_env(EF_KEY_DEVICE_NAME, ble_transparent_get_name());

    char buf[16];
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)uart_bridge_get_baudrate());
    ef_set_env(EF_KEY_UART_BAUDRATE, buf);

    snprintf(buf, sizeof(buf), "%d", (int)ble_transparent_get_tx_power());
    ef_set_env(EF_KEY_TX_POWER, buf);

    snprintf(buf, sizeof(buf), "%u", ble_transparent_get_adv_interval());
    ef_set_env(EF_KEY_ADV_INTERVAL, buf);

    snprintf(buf, sizeof(buf), "%u", (unsigned int)ble_transparent_get_passkey());
    ef_set_env(EF_KEY_PASSKEY, buf);

    ef_set_env(EF_KEY_BONDABLE, ble_transparent_get_bondable() ? "1" : "0");
#endif
    return 0;
}
