/**
 * @file btdm_cli.c
 * @brief BTDM CLI Commands
 *
 * This file implements CLI commands for testing the BTDM controller.
 * BLE-specific commands use "le_" prefix.
 *
 * Copyright (c) 2024 IPRO
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/autoconf.h>

#if defined(CONFIG_BTDM_ENABLE) && defined(CONFIG_SHELL)

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"
#include "btdm_sdk.h"
#include "ipro_ble_ctlr_hcitl.h"
#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
#include "ipro6_sco_i2s_tx.h"
#endif

#if defined(CONFIG_BTDM_HOST_BRINGUP)
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/net_buf.h>
#endif

#ifdef CONFIG_RF_AD9361
#include "ad9361_rf_private.h"
#include "ad9361_reg_access.h"
#endif

/* External app functions */
extern int btdm_app_init(void);
extern int btdm_app_start_advertising(void);
extern int btdm_app_stop_advertising(void);
extern int btdm_app_start_scanning(void);
extern int btdm_app_stop_scanning(void);
extern void btdm_app_print_status(void);
#if defined(CONFIG_BTDM_HOST_BRINGUP) && defined(CONFIG_BT_BREDR)
extern int ipro6s_host_set_br_discoverable(bool enable);
#endif
#if defined(CONFIG_BTDM_HOST_BRINGUP) && defined(CONFIG_BT_SETTINGS)
extern int ipro6s_host_clear_bonds(void);
#endif
#if defined(CONFIG_BTDM_HOST_BRINGUP) && defined(CONFIG_BTDM_HOST_LE_ADV)
extern int ipro6s_host_set_le_advertising(bool enable);
#endif

/* Track last LE test type for le_test_end display */
static enum { LE_TEST_NONE = 0, LE_TEST_TX, LE_TEST_RX } last_le_test_type = LE_TEST_NONE;

/*
 * ============================================================================
 * General BTDM Commands
 * ============================================================================
 */

/**
 * @brief btdm init - Initialize BTDM controller
 */
static void cmd_btdm_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Initializing BTDM controller...\r\n");
    int ret = btdm_app_init();
    if (ret == 0) {
        printf("BTDM initialized successfully\r\n");
    } else {
        printf("BTDM init failed: %d\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_btdm_init, btdm_init, Initialize BTDM controller);

/**
 * @brief ble_enable - Enable BLE subsystem (clocks + DFE BZ mode)
 */
static void cmd_ble_enable(int argc, char **argv)
{
    uint8_t en = 1;
    if (argc >= 2 && strcmp(argv[1], "0") == 0) {
        en = 0;
    }

    /* Enable/disable BLE clocks */
    extern void btdm_enable_ble_clk(uint8_t enable);
    btdm_enable_ble_clk(en);

    if (en) {
        /* DFE mode: switch to BZ (Bluetooth/Zigbee) mode
         * Register 0x20001220 bits [27:26]:
         *   bit 27 = force enable
         *   bit 26 = 1: BZ mode, 0: WiFi mode
         */
        volatile uint32_t *dfe = (volatile uint32_t *)0x20001220;
        uint32_t dfe_val = *dfe;
        dfe_val |= (3U << 26);  /* [27:26] = 0x3: force enable + BZ mode */
        *dfe = dfe_val;

        /* Clear rx_adc_4s_i_en (bit 11) and rx_adc_4s_q_en (bit 27) in RF_DFE_CTRL_3.
         * When set, these force the I/Q ADC paths to static values, blocking RX.
         */
        volatile uint32_t *dfe_ctrl3 = (volatile uint32_t *)0x2000160c;
        uint32_t ctrl3_val = *dfe_ctrl3;
        ctrl3_val &= ~(1U << 11);  /* rx_adc_4s_i_en = 0 */
        ctrl3_val &= ~(1U << 27);  /* rx_adc_4s_q_en = 0 */
        *dfe_ctrl3 = ctrl3_val;

#ifdef CONFIG_RF_AD9361
        /* AD9361 zero-IF configuration: disable DDC and DC cancellation.
         * phy_rx_dfe_0 (0x20003810):
         *   bit[2] rx_dfe_ddc_en  = 0  (no DDC needed, AD9361 outputs zero-IF)
         *   bit[8] rx_dfe_dccan_en = 0  (DC cancel would strip the signal at zero-IF)
         */
        volatile uint32_t *phy_rx_dfe0 = (volatile uint32_t *)0x20003810;
        uint32_t rx_dfe0_val = *phy_rx_dfe0;
        rx_dfe0_val &= ~(1U << 2);   /* rx_dfe_ddc_en = 0 */
        rx_dfe0_val &= ~(1U << 8);   /* rx_dfe_dccan_en = 0 */
        *phy_rx_dfe0 = rx_dfe0_val;
        printf("BLE enabled (clocks + DFE BZ + ADC cleared + zero-IF)\r\n");
#else
        printf("BLE enabled (clocks + DFE BZ mode + ADC path cleared)\r\n");
#endif
    } else {
        printf("BLE disabled (clocks off)\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_enable, ble_enable, Enable BLE subsystem (clocks + DFE));

/**
 * @brief btdm reset - HCI Reset
 */
static void cmd_btdm_reset(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Sending HCI Reset...\r\n");
    int ret = btdm_hci_reset();
    if (ret == 0) {
        printf("HCI Reset completed\r\n");
    } else {
        printf("HCI Reset failed: %d\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_btdm_reset, btdm_reset, Send HCI Reset command);

/**
 * @brief btdm tx_power - Set TX power
 */
static void cmd_btdm_tx_power(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: btdm_tx_power <dbm>\r\n");
        printf("  dbm: TX power in dBm (-20 to 10)\r\n");
        return;
    }

    int8_t power_dbm = (int8_t)atoi(argv[1]);

    printf("Setting TX power to %d dBm...\r\n", power_dbm);

    int ret = btdm_set_tx_power(power_dbm);
    if (ret == 0) {
        printf("TX power set successfully\r\n");
    } else {
        printf("Failed to set TX power: %d\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_btdm_tx_power, btdm_tx_power, Set TX power);

/**
 * @brief btdm status - Show controller status
 */
static void cmd_btdm_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    extern void ipro6s_bringup_status_print(void);
    ipro6s_bringup_status_print();
    btdm_app_print_status();
}
SHELL_CMD_EXPORT_ALIAS(cmd_btdm_status, btdm_status, Show BTDM status);

#if defined(CONFIG_BTDM_VOICE_OVER_HCI)
static void cmd_sco_i2s(int argc, char **argv)
{
    ipro6_sco_i2s_stats_t stats;

    if (argc >= 2) {
        if (strcmp(argv[1], "tone") == 0) {
            if (argc < 3) {
                printf("Usage: sco_i2s tone on|off\r\n");
                return;
            }
            ipro6_sco_i2s_set_tone(strcmp(argv[2], "on") == 0);
        } else if (strcmp(argv[1], "status") != 0) {
            printf("Usage: sco_i2s [status|tone on|off]\r\n");
            return;
        }
    }

    ipro6_sco_i2s_get_stats(&stats);
    printf("BT audio I2S: a2dp/sco/tone/prime=%u/%u/%u/%u rate/ch/dma=%lu/%u/%u "
           "input_pkt/bytes/drop=%lu/%lu/%lu dma/underrun/error=%lu/%lu/%lu "
           "ring=%lu hash=%08lx\r\n",
           stats.a2dp_active, stats.sco_active, stats.tone_active,
           stats.primed, (unsigned long)stats.sample_rate_hz,
           stats.channels, stats.dma_bytes,
           (unsigned long)stats.input_packets,
           (unsigned long)stats.input_bytes,
           (unsigned long)stats.input_drops,
           (unsigned long)stats.dma_frames,
           (unsigned long)stats.dma_underruns,
           (unsigned long)stats.dma_errors,
           (unsigned long)stats.ring_bytes,
           (unsigned long)stats.tx_hash);
    printf("BT audio DMA callbacks: active_ch=%u ch0..7=%lu/%lu/%lu/%lu/%lu/%lu/%lu/%lu\r\n",
           stats.dma_channel,
           (unsigned long)stats.dma_callback_ch[0],
           (unsigned long)stats.dma_callback_ch[1],
           (unsigned long)stats.dma_callback_ch[2],
           (unsigned long)stats.dma_callback_ch[3],
           (unsigned long)stats.dma_callback_ch[4],
           (unsigned long)stats.dma_callback_ch[5],
           (unsigned long)stats.dma_callback_ch[6],
           (unsigned long)stats.dma_callback_ch[7]);
    printf("BT audio DMA LLI: change=%lu repeat=%lu last=0x%08lx\r\n",
           (unsigned long)stats.dma_lli_changes,
           (unsigned long)stats.dma_lli_repeats,
           (unsigned long)stats.dma_last_lli);
}
SHELL_CMD_EXPORT_ALIAS(cmd_sco_i2s, sco_i2s,
                       SCO PCM I2S output status and 1kHz tone);
#endif

#if defined(CONFIG_BTDM_HOST_BRINGUP) && defined(CONFIG_BT_SETTINGS)
/**
 * @brief bt clear_bonds - Remove all persisted LE and BR/EDR bonds
 */
static void cmd_bt_clear_bonds(int argc, char **argv)
{
    int ret;

    (void)argc;
    (void)argv;

    ret = ipro6s_host_clear_bonds();
    printf("[BTDM_SECURITY] clear bonds rc=%d\r\n", ret);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_clear_bonds, bt_clear_bonds,
                       Remove all persisted LE and BR/EDR bonds);
#endif

/**
 * @brief btdm mac - Show/Set BD address
 */
static void cmd_btdm_mac(int argc, char **argv)
{
    uint8_t mac[6];

    if (argc >= 2) {
        /* Set MAC address */
        if (strlen(argv[1]) != 17) {
            printf("Invalid MAC format. Use XX:XX:XX:XX:XX:XX\r\n");
            return;
        }

        if (sscanf(argv[1], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                   &mac[5], &mac[4], &mac[3], &mac[2], &mac[1], &mac[0]) != 6) {
            printf("Invalid MAC format. Use XX:XX:XX:XX:XX:XX\r\n");
            return;
        }

        int ret = btdm_set_bd_addr(mac);
        if (ret == 0) {
            printf("BD address set to %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                   mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
        } else {
            printf("Failed to set BD address: %d\r\n", ret);
        }
    } else {
        /* Get MAC address */
        int ret = btdm_get_bd_addr(mac);
        if (ret == 0) {
            printf("BD Address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                   mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
        } else {
            printf("Failed to get BD address: %d\r\n", ret);
        }
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_btdm_mac, btdm_mac, Show/Set BD address);

/*
 * Controller-only H4 command path.  This intentionally stays below any host
 * stack so the first IPRO6S Classic gate can distinguish controller/HW
 * failures from host integration failures.
 */
static int btdm_h4_command(uint16_t opcode, const uint8_t *params,
                           uint8_t param_len, uint8_t *response,
                           uint16_t response_size, uint16_t *response_len)
{
    uint8_t command[4 + 255];
    uint8_t discard[64];
    TickType_t deadline;
    uint16_t len = 0;

    while (ipro_ble_ctlr_hcitl_host_read(discard, sizeof(discard)) != 0) {
    }

    command[0] = 0x01;
    command[1] = (uint8_t)opcode;
    command[2] = (uint8_t)(opcode >> 8);
    command[3] = param_len;
    if (param_len != 0) {
        memcpy(&command[4], params, param_len);
    }

    if (ipro_ble_ctlr_hcitl_host_write_space() < (uint32_t)param_len + 4u ||
        ipro_ble_ctlr_hcitl_host_write(command, (uint32_t)param_len + 4u) !=
            (uint32_t)param_len + 4u) {
        return -1;
    }

    deadline = xTaskGetTickCount() + pdMS_TO_TICKS(2000);
    while ((int32_t)(xTaskGetTickCount() - deadline) < 0) {
        if (len < response_size) {
            len += (uint16_t)ipro_ble_ctlr_hcitl_host_read(
                &response[len], response_size - len);
        }

        for (uint16_t offset = 0; offset + 7u <= len; offset++) {
            const uint8_t *evt = &response[offset];

            if (evt[0] != 0x04) {
                continue;
            }
            if (evt[1] == 0x0e && evt[2] >= 4 &&
                evt[4] == (uint8_t)opcode &&
                evt[5] == (uint8_t)(opcode >> 8)) {
                *response_len = len;
                return evt[6] == 0 ? 0 : -(int)evt[6];
            }
            if (evt[1] == 0x0f && evt[2] == 4 && evt[5] == (uint8_t)opcode &&
                evt[6] == (uint8_t)(opcode >> 8)) {
                *response_len = len;
                return evt[3] == 0 ? 0 : -(int)evt[3];
            }
        }
        vTaskDelay(1);
    }

    *response_len = len;
    return -2;
}

static void btdm_h4_response_print(uint16_t opcode, int rc,
                                   const uint8_t *response, uint16_t len)
{
    printf("[BTDM_H4] opcode=0x%04x rc=%d len=%u data=", opcode, rc, len);
    for (uint16_t i = 0; i < len; i++) {
        printf("%02x", response[i]);
    }
    printf("\r\n");
}

static void cmd_bt_hci(int argc, char **argv)
{
    uint8_t params[255];
    uint8_t response[260];
    uint16_t response_len = 0;
    unsigned long opcode;
    int rc;

    if (argc < 2 || argc > 257) {
        printf("Usage: bt_hci <opcode_hex> [param_byte_hex ...]\r\n");
        return;
    }

    opcode = strtoul(argv[1], NULL, 16);
    if (opcode > 0xffffu) {
        printf("Invalid HCI opcode: %s\r\n", argv[1]);
        return;
    }
    for (int i = 2; i < argc; i++) {
        unsigned long value = strtoul(argv[i], NULL, 16);
        if (value > 0xffu) {
            printf("Invalid parameter byte: %s\r\n", argv[i]);
            return;
        }
        params[i - 2] = (uint8_t)value;
    }

    rc = btdm_h4_command((uint16_t)opcode, params, (uint8_t)(argc - 2),
                         response, sizeof(response), &response_len);
    btdm_h4_response_print((uint16_t)opcode, rc, response, response_len);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_hci, bt_hci,
                       Send raw controller-only HCI command);

#if defined(CONFIG_BTDM_HOST_BRINGUP)
static void cmd_bt_host_hci(int argc, char **argv)
{
    struct net_buf *buf = NULL;
    struct net_buf *rsp = NULL;
    unsigned long opcode;
    int rc;

    if (argc < 2 || argc > 257) {
        printf("Usage: bt_host_hci <opcode_hex> [param_byte_hex ...]\r\n");
        return;
    }

    opcode = strtoul(argv[1], NULL, 16);
    if (opcode > 0xffffu) {
        printf("Invalid HCI opcode: %s\r\n", argv[1]);
        return;
    }
    if (argc > 2) {
        buf = bt_hci_cmd_alloc(K_FOREVER);
        if (buf == NULL) {
            printf("[BTDM_HOST_HCI] opcode=0x%04lx rc=-12\r\n", opcode);
            return;
        }
        for (int i = 2; i < argc; i++) {
            unsigned long value = strtoul(argv[i], NULL, 16);

            if (value > 0xffu) {
                printf("Invalid parameter byte: %s\r\n", argv[i]);
                net_buf_unref(buf);
                return;
            }
            net_buf_add_u8(buf, (uint8_t)value);
        }
    }

    rc = bt_hci_cmd_send_sync((uint16_t)opcode, buf, &rsp);
    printf("[BTDM_HOST_HCI] opcode=0x%04lx rc=%d len=%u data=",
           opcode, rc, rsp != NULL ? rsp->len : 0u);
    if (rsp != NULL) {
        for (uint16_t i = 0; i < rsp->len; i++) {
            printf("%02x", rsp->data[i]);
        }
        net_buf_unref(rsp);
    }
    printf("\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_host_hci, bt_host_hci,
                       Send HCI command through the active Zephyr Host);

/* Write Simple Pairing Debug Mode is a testing command (OGF 0x06, OCF 0x04)
 * that is intentionally absent from Zephyr's public opcode list.  Send it
 * through the active Host command path so the Host, rather than this CLI,
 * owns and consumes Command Complete. */
#define IPRO_HCI_OP_WRITE_SP_DEBUG_MODE 0x1804u

static void cmd_bt_ssp_debug(int argc, char **argv)
{
    struct net_buf *buf;
    uint8_t enable;
    int rc;

    if (argc != 2 || (strcmp(argv[1], "0") != 0 &&
                      strcmp(argv[1], "1") != 0)) {
        printf("Usage: bt_ssp_debug <0|1>\r\n");
        return;
    }

    enable = (uint8_t)(argv[1][0] == '1');
    buf = bt_hci_cmd_alloc(K_FOREVER);
    if (buf == NULL) {
        printf("[BTDM_SSP] debug=%u rc=-12\r\n", enable);
        return;
    }

    net_buf_add_u8(buf, enable);
    rc = bt_hci_cmd_send_sync(IPRO_HCI_OP_WRITE_SP_DEBUG_MODE, buf, NULL);
    printf("[BTDM_SSP] debug=%u rc=%d\r\n", enable, rc);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_ssp_debug, bt_ssp_debug,
                       Set controller SSP P192 debug-key mode);
#endif

/* Drain controller-to-host H4 bytes without issuing another command.  This
 * is required for controller-only Classic bring-up because Connection
 * Request/Complete and disconnection events are asynchronous. */
static void cmd_bt_hci_rx(int argc, char **argv)
{
    uint8_t data[260];
    uint32_t wait_ms = 1000;
    TickType_t deadline;
    uint32_t total = 0;

    if (argc >= 2) {
        wait_ms = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    deadline = xTaskGetTickCount() + pdMS_TO_TICKS(wait_ms);
    while ((int32_t)(xTaskGetTickCount() - deadline) < 0) {
        uint32_t len = ipro_ble_ctlr_hcitl_host_read(data, sizeof(data));

        if (len == 0) {
            vTaskDelay(1);
            continue;
        }
        printf("[BTDM_H4_RX] len=%lu data=", (unsigned long)len);
        for (uint32_t i = 0; i < len; i++) {
            printf("%02x", data[i]);
        }
        printf("\r\n");
        total += len;
    }
    printf("[BTDM_H4_RX] done total=%lu wait_ms=%lu\r\n",
           (unsigned long)total, (unsigned long)wait_ms);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_hci_rx, bt_hci_rx,
                       Drain asynchronous controller H4 events);

static void cmd_bt_h4_tx(int argc, char **argv)
{
    uint8_t packet[260];
    uint32_t len;
    uint32_t written;

    if (argc < 2 || argc > (int)(sizeof(packet) + 1u)) {
        printf("Usage: bt_h4_tx <h4_byte_hex ...>\r\n");
        return;
    }
    len = (uint32_t)(argc - 1);
    for (uint32_t i = 0; i < len; i++) {
        unsigned long value = strtoul(argv[i + 1], NULL, 16);

        if (value > 0xffu) {
            printf("Invalid H4 byte: %s\r\n", argv[i + 1]);
            return;
        }
        packet[i] = (uint8_t)value;
    }
    if (ipro_ble_ctlr_hcitl_host_write_space() < len) {
        printf("[BTDM_H4_TX] no_space len=%lu space=%lu\r\n",
               (unsigned long)len,
               (unsigned long)ipro_ble_ctlr_hcitl_host_write_space());
        return;
    }
    written = ipro_ble_ctlr_hcitl_host_write(packet, len);
    printf("[BTDM_H4_TX] len=%lu written=%lu data=",
           (unsigned long)len, (unsigned long)written);
    for (uint32_t i = 0; i < len; i++) {
        printf("%02x", packet[i]);
    }
    printf("\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_h4_tx, bt_h4_tx,
                       Inject a raw host-to-controller H4 packet);

static void cmd_bt_discoverable(int argc, char **argv)
{
#if defined(CONFIG_BTDM_HOST_BRINGUP) && defined(CONFIG_BT_BREDR)
    bool enable = true;
    int rc;

    if (argc >= 2) {
        if (strcmp(argv[1], "off") == 0) {
            enable = false;
        } else if (strcmp(argv[1], "on") != 0) {
            printf("Usage: bt_discoverable [on|off]\r\n");
            return;
        }
    }
    rc = ipro6s_host_set_br_discoverable(enable);
    printf("[BTDM_CLASSIC] discoverable=%s rc=%d name=%s cod=0x%06x\r\n",
           enable ? "on" : "off", rc, CONFIG_BT_DEVICE_NAME,
           (unsigned int)CONFIG_BT_COD);
#else
    static const uint8_t class_of_device[] = { 0x04, 0x04, 0x24 };
    static const uint8_t scan_enable[] = { 0x03 };
    uint8_t local_name[248] = { 0 };
    uint8_t response[260];
    uint16_t response_len;
    const char *name = (argc >= 2) ? argv[1] : "IPRO6S-BTDM";
    int rc;

    strncpy((char *)local_name, name, sizeof(local_name) - 1u);

    rc = btdm_h4_command(0x0c13, local_name, sizeof(local_name), response,
                         sizeof(response), &response_len);
    btdm_h4_response_print(0x0c13, rc, response, response_len);
    if (rc != 0) {
        return;
    }
    rc = btdm_h4_command(0x0c24, class_of_device, sizeof(class_of_device),
                         response, sizeof(response), &response_len);
    btdm_h4_response_print(0x0c24, rc, response, response_len);
    if (rc != 0) {
        return;
    }
    rc = btdm_h4_command(0x0c1a, scan_enable, sizeof(scan_enable), response,
                         sizeof(response), &response_len);
    btdm_h4_response_print(0x0c1a, rc, response, response_len);
    printf("[BTDM_CLASSIC] discoverable=%s name=%s\r\n",
           rc == 0 ? "yes" : "no", name);
#endif
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_discoverable, bt_discoverable,
                       Enable Classic inquiry and page scan);

/*
 * ============================================================================
 * BLE (LE) Specific Commands
 * ============================================================================
 */

/**
 * @brief le_adv - Control BLE advertising
 */
static void cmd_le_adv(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: le_adv <start|stop>\r\n");
        return;
    }

    if (strcmp(argv[1], "start") == 0) {
#if defined(CONFIG_BTDM_HOST_BRINGUP) && defined(CONFIG_BTDM_HOST_LE_ADV)
        int ret = ipro6s_host_set_le_advertising(true);
#else
        int ret = btdm_app_start_advertising();
#endif
        if (ret == 0) {
            printf("LE advertising started\r\n");
        } else {
            printf("Failed to start LE advertising: %d\r\n", ret);
        }
    } else if (strcmp(argv[1], "stop") == 0) {
#if defined(CONFIG_BTDM_HOST_BRINGUP) && defined(CONFIG_BTDM_HOST_LE_ADV)
        int ret = ipro6s_host_set_le_advertising(false);
#else
        int ret = btdm_app_stop_advertising();
#endif
        if (ret == 0) {
            printf("LE advertising stopped\r\n");
        } else {
            printf("Failed to stop LE advertising: %d\r\n", ret);
        }
    } else {
        printf("Unknown subcommand: %s\r\n", argv[1]);
        printf("Usage: le_adv <start|stop>\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_le_adv, le_adv, Control LE advertising (start/stop));

/**
 * @brief le_scan - Control BLE scanning
 */
static void cmd_le_scan(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: le_scan <start|stop>\r\n");
        return;
    }

    if (strcmp(argv[1], "start") == 0) {
        int ret = btdm_app_start_scanning();
        if (ret == 0) {
            printf("LE scanning started\r\n");
        } else {
            printf("Failed to start LE scanning: %d\r\n", ret);
        }
    } else if (strcmp(argv[1], "stop") == 0) {
        int ret = btdm_app_stop_scanning();
        if (ret == 0) {
            printf("LE scanning stopped\r\n");
        } else {
            printf("Failed to stop LE scanning: %d\r\n", ret);
        }
    } else {
        printf("Unknown subcommand: %s\r\n", argv[1]);
        printf("Usage: le_scan <start|stop>\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_le_scan, le_scan, Control LE scanning (start/stop));

/**
 * @brief le_tx_test - BLE TX test mode
 */
static void cmd_le_tx_test(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: le_tx_test <channel> <length> <payload> [phy] [tx_power]\r\n");
        printf("  channel:  0-39 (BLE channel index)\r\n");
        printf("  length:   0-255 (payload length)\r\n");
        printf("  payload:  0-7 (payload type)\r\n");
        printf("  phy:      1=1M(default), 2=2M, 3=Coded S=8, 4=Coded S=2\r\n");
        printf("  tx_power: -127~+20 dBm, 0x7E=min, 0x7F=max(default)\r\n");
        return;
    }

    uint8_t channel = (uint8_t)atoi(argv[1]);
    uint8_t length = (uint8_t)atoi(argv[2]);
    uint8_t payload = (uint8_t)atoi(argv[3]);
    uint8_t phy = (argc > 4) ? (uint8_t)atoi(argv[4]) : 1;
    int8_t tx_power = (argc > 5) ? (int8_t)atoi(argv[5]) : 0x7F;

    if (channel > 39) {
        printf("Invalid channel: %d (must be 0-39)\r\n", channel);
        return;
    }

    printf("Starting LE TX test: ch=%d, len=%d, payload=%d, phy=%d, tx_pwr=%d\r\n",
           channel, length, payload, phy, tx_power);

    int ret = btdm_hci_le_tx_test(channel, length, payload, phy, tx_power);
    if (ret == 0) {
        last_le_test_type = LE_TEST_TX;
        printf("LE TX test started\r\n");
    } else {
        printf("LE TX test failed: %d\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_le_tx_test, le_tx_test, LE TX test mode);

/**
 * @brief le_rx_test - BLE RX test mode
 */
static void cmd_le_rx_test(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: le_rx_test <channel> [phy] [mod_idx]\r\n");
        printf("  channel:  0-39 (BLE channel index)\r\n");
        printf("  phy:      1=1M(default), 2=2M, 3=Coded\r\n");
        printf("  mod_idx:  0=standard(default), 1=stable\r\n");
        return;
    }

    uint8_t channel = (uint8_t)atoi(argv[1]);
    uint8_t phy = (argc > 2) ? (uint8_t)atoi(argv[2]) : 1;
    uint8_t mod_idx = (argc > 3) ? (uint8_t)atoi(argv[3]) : 0;

    if (channel > 39) {
        printf("Invalid channel: %d (must be 0-39)\r\n", channel);
        return;
    }

    printf("Starting LE RX test: ch=%d, phy=%d, mod_idx=%d\r\n", channel, phy, mod_idx);

    int ret = btdm_hci_le_rx_test(channel, phy, mod_idx);
    if (ret == 0) {
        last_le_test_type = LE_TEST_RX;
        printf("LE RX test started\r\n");
    } else {
        printf("LE RX test failed: %d\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_le_rx_test, le_rx_test, LE RX test mode);

/**
 * @brief le_test_end - End BLE test mode
 */
static void cmd_le_test_end(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Ending LE test mode...\r\n");

    uint16_t num_packets = 0;
    int ret = btdm_hci_le_test_end(&num_packets);
    if (ret == 0) {
        if (last_le_test_type == LE_TEST_TX) {
            printf("LE TX test ended. Packets transmitted: %d\r\n", num_packets);
        } else if (last_le_test_type == LE_TEST_RX) {
            printf("LE RX test ended. Packets received: %d\r\n", num_packets);
        } else {
            printf("LE test ended. Packet count: %d\r\n", num_packets);
        }
        last_le_test_type = LE_TEST_NONE;
    } else {
        printf("LE test end failed: %d\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_le_test_end, le_test_end, End LE test mode);

/*
 * ============================================================================
 * BR/EDR Non-Signaling Test Commands
 * ============================================================================
 * The bt_hci commands above drive the controller the way a host would, which
 * is what signaling and audio work needs. These are the other half: no paging,
 * no connection, nothing on the far end - the controller keys the transmitter
 * or opens the receiver on one channel. That is what the RF measurements want,
 * and it is the only way to exercise the radio before anything can connect.
 *
 * They go through btdm_h4_command() rather than calling the link driver, for
 * the same reason bt_hci does: the driver posts kernel messages that the
 * controller task owns, so reaching into it from the shell task wedges the
 * system. The transport hands the command to the right task and brings the
 * completion back, packet count included.
 *
 * Vendor-specific opcodes, wire format per the HCI command table:
 *   0xFC71 TX   BH4B  channel, length(16), payload, pkt_type, tx_pwr, infinite
 *   0xFC70 RX   3B    channel, pkt_type, infinite
 *   0xFC72 END  -     answers with status and a 16-bit packet count
 */
#ifdef CONFIG_BTDM_BT_ENABLE

#define BT_VS_TX_TEST_OPCODE    0xFC71u
#define BT_VS_RX_TEST_OPCODE    0xFC70u
#define BT_VS_TEST_END_OPCODE   0xFC72u

/* Shared-RF ownership, as the LE test path does it (btdm_sdk.c). The
 * controller's port header is a private include dir, so declare the two hooks
 * here - btdm_app.c forward-declares rf_bz_enter() the same way. Without this
 * the BR/EDR tests run with the RF still owned by whoever had it: the AGC
 * never engages and the BT demodulator stays clock-gated. */
void btdm_rf_enter_bz(void);
void btdm_rf_exit_bz(void);

/* Which test ran, so bt_test_stop can label the count. Doubles as the
 * RF-ownership flag, so enter/exit stay paired and never nest. */
static enum { BT_TEST_NONE = 0, BT_TEST_TX, BT_TEST_RX } last_bt_test_type = BT_TEST_NONE;

/* btdm_h4_command() folds the completion status into its return value as
 * -status, so a non-zero return is an HCI error code, not a transport fault.
 * The bytes it leaves in the buffer are the raw event stream; this digs the
 * completion parameters out of it for the callers that need them. */
static const uint8_t *bt_test_cmd_cmp_params(uint16_t opcode, const uint8_t *buf,
                                             uint16_t len, uint16_t *param_len)
{
    for (uint16_t off = 0; off + 7u <= len; off++) {
        const uint8_t *evt = &buf[off];

        if (evt[0] == 0x04 && evt[1] == 0x0e && evt[2] >= 4 &&
            evt[4] == (uint8_t)opcode && evt[5] == (uint8_t)(opcode >> 8)) {
            *param_len = (uint16_t)(evt[2] - 3u);   /* after num_cmd + opcode */
            return &evt[6];                          /* status first */
        }
    }
    *param_len = 0;
    return NULL;
}

/* The controller refuses a test while it believes something else is running.
 * A reset puts the link manager back to idle; do it once, the first time a
 * test is asked for, the same way the LE path does. */
static bool bt_test_reset_done;

static bool bt_test_prepare(void)
{
    uint8_t response[64];
    uint16_t response_len = 0;
    int rc;

    if (bt_test_reset_done) {
        return true;
    }

    rc = btdm_h4_command(0x0C03u /* HCI Reset */, NULL, 0,
                         response, sizeof(response), &response_len);
    if (rc != 0) {
        printf("HCI reset before test failed: status=0x%02X\r\n", (unsigned)(-rc));
        return false;
    }

    bt_test_reset_done = true;
    return true;
}


static void cmd_bt_tx_test(int argc, char **argv)
{
    uint8_t params[7];
    uint8_t response[64];
    uint16_t response_len = 0;
    int rc;

    if (argc < 4) {
        printf("Usage: bt_tx_test <ch> <payload> <pkt_type> [tx_pwr] [len] [inf]\r\n");
        printf("  ch       0-78    F = 2402 + 2*ch MHz (0=2402, 39=2480, 78=2558)\r\n");
        printf("  payload  0-7     0=PRBS9 1=11110000 2=10101010 3=PRBS15\r\n");
        printf("                   4=all-1 5=all-0    6=00001111 7=01010101\r\n");
        printf("  pkt_type 0-11    0=DM1 1=DH1 2=DM3 3=DH3 4=DM5 5=DH5\r\n");
        printf("                   6=2-DH1 7=3-DH1 8=2-DH3 9=3-DH3 10=2-DH5 11=3-DH5\r\n");
        printf("  tx_pwr   dBm     -127..20, 126=min, 127=max (default 127)\r\n");
        printf("  len      0-1021  payload bytes, capped by pkt_type (default 27)\r\n");
        printf("  inf      0|1     1 = continuous, no packet gating (default 0)\r\n");
        printf("Modulation: payload=1 for Df1avg, payload=2 for Df2max\r\n");
        return;
    }

    uint32_t channel  = (uint32_t)atoi(argv[1]);
    uint32_t payload  = (uint32_t)atoi(argv[2]);
    uint32_t pkt_type = (uint32_t)atoi(argv[3]);
    int32_t  tx_pwr   = (argc > 4) ? atoi(argv[4]) : 127;
    uint32_t length   = (argc > 5) ? (uint32_t)atoi(argv[5]) : 27;
    uint32_t infinite = (argc > 6) ? (uint32_t)atoi(argv[6]) : 0;

    if (channel > 78 || payload > 7 || length > 1021 || infinite > 1) {
        printf("Parameter out of range\r\n");
        return;
    }
    /* Types above 3-DH5 are the synchronous ones, which need a synchronous
     * link this image does not create. */
    if (pkt_type > 11) {
        printf("pkt_type %lu is synchronous, not usable here\r\n", (unsigned long)pkt_type);
        return;
    }

    params[0] = (uint8_t)channel;
    params[1] = (uint8_t)(length & 0xFFu);
    params[2] = (uint8_t)((length >> 8) & 0xFFu);
    params[3] = (uint8_t)payload;
    params[4] = (uint8_t)pkt_type;
    params[5] = (uint8_t)(int8_t)tx_pwr;
    params[6] = (uint8_t)infinite;

    if (!bt_test_prepare()) {
        return;
    }

    /* Take the shared RF for BZ before the controller keys the radio. */
    if (last_bt_test_type == BT_TEST_NONE) {
        btdm_rf_enter_bz();
    }

    rc = btdm_h4_command(BT_VS_TX_TEST_OPCODE, params, sizeof(params),
                         response, sizeof(response), &response_len);
    if (rc != 0) {
        printf("BT TX test failed: status=0x%02X\r\n", (unsigned)(-rc));
        if (last_bt_test_type == BT_TEST_NONE) {
            btdm_rf_exit_bz();
        }
        return;
    }

    last_bt_test_type = BT_TEST_TX;
    printf("BT TX test started: ch=%lu (%lu MHz), len=%lu, payload=%lu, "
           "type=%lu, pwr=%ld, infinite=%lu\r\n",
           (unsigned long)channel, (unsigned long)(2402u + 2u * channel),
           (unsigned long)length, (unsigned long)payload,
           (unsigned long)pkt_type, (long)tx_pwr, (unsigned long)infinite);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_tx_test, bt_tx_test, BR/EDR non-signaling TX test);

static void cmd_bt_rx_test(int argc, char **argv)
{
    uint8_t params[3];
    uint8_t response[64];
    uint16_t response_len = 0;
    int rc;

    if (argc < 3) {
        printf("Usage: bt_rx_test <ch> <pkt_type> [inf]\r\n");
        printf("  ch, pkt_type as for bt_tx_test; inf 0|1 = continuous\r\n");
        return;
    }

    uint32_t channel  = (uint32_t)atoi(argv[1]);
    uint32_t pkt_type = (uint32_t)atoi(argv[2]);
    uint32_t infinite = (argc > 3) ? (uint32_t)atoi(argv[3]) : 0;

    if (channel > 78 || infinite > 1) {
        printf("Parameter out of range\r\n");
        return;
    }
    if (pkt_type > 11) {
        printf("pkt_type %lu is synchronous, not usable here\r\n", (unsigned long)pkt_type);
        return;
    }

    params[0] = (uint8_t)channel;
    params[1] = (uint8_t)pkt_type;
    params[2] = (uint8_t)infinite;

    if (!bt_test_prepare()) {
        return;
    }

    /* Take the shared RF for BZ; without this the AGC never engages. */
    if (last_bt_test_type == BT_TEST_NONE) {
        btdm_rf_enter_bz();
    }

    rc = btdm_h4_command(BT_VS_RX_TEST_OPCODE, params, sizeof(params),
                         response, sizeof(response), &response_len);
    if (rc != 0) {
        printf("BT RX test failed: status=0x%02X\r\n", (unsigned)(-rc));
        if (last_bt_test_type == BT_TEST_NONE) {
            btdm_rf_exit_bz();
        }
        return;
    }

    last_bt_test_type = BT_TEST_RX;
    printf("BT RX test started: ch=%lu (%lu MHz), type=%lu, infinite=%lu\r\n",
           (unsigned long)channel, (unsigned long)(2402u + 2u * channel),
           (unsigned long)pkt_type, (unsigned long)infinite);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_rx_test, bt_rx_test, BR/EDR non-signaling RX test);

static void cmd_bt_test_stop(int argc, char **argv)
{
    uint8_t response[64];
    uint16_t response_len = 0;
    uint16_t num_packets = 0;
    int rc;

    (void)argc;
    (void)argv;

    rc = btdm_h4_command(BT_VS_TEST_END_OPCODE, NULL, 0,
                         response, sizeof(response), &response_len);
    if (rc != 0) {
        printf("BT test end failed: status=0x%02X\r\n", (unsigned)(-rc));
        return;
    }

    {
        uint16_t param_len = 0;
        const uint8_t *p = bt_test_cmd_cmp_params(BT_VS_TEST_END_OPCODE, response,
                                                  response_len, &param_len);
        /* status, then a 16-bit little-endian count */
        if (p != NULL && param_len >= 3u) {
            num_packets = (uint16_t)(p[1] | ((uint16_t)p[2] << 8));
        }
    }

    if (last_bt_test_type == BT_TEST_TX) {
        printf("BT TX test ended. Packets transmitted: %u\r\n", num_packets);
    } else if (last_bt_test_type == BT_TEST_RX) {
        printf("BT RX test ended. Packets received:    %u\r\n", num_packets);
    } else {
        printf("BT test ended. Packet count: %u\r\n", num_packets);
    }
    if (last_bt_test_type != BT_TEST_NONE) {
        /* Hand the shared RF back to its previous owner */
        btdm_rf_exit_bz();
    }
    last_bt_test_type = BT_TEST_NONE;
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_test_stop, bt_test_stop, End BR/EDR non-signaling test);

/* The test-mode registers, read directly. A run should be judged from these
 * rather than only from the number the stack reports - and in continuous mode
 * the packet counter stays at zero by nature, so the spectrum and these bits
 * are all there is to go on. Read by address: this file does not pull in the
 * controller's register headers. */
static void cmd_bt_test_stat(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("BT test registers:\r\n");
    printf("  RWBTCNTL     @0x28000400 = 0x%08lX\r\n", (unsigned long)*(volatile uint32_t *)0x28000400UL);
    printf("  RFTESTCNTL   @0x280004D0 = 0x%08lX  (bit15 infiniteTX, bit31 infiniteRX)\r\n",
           (unsigned long)*(volatile uint32_t *)0x280004D0UL);
    printf("  RFTESTFREQ   @0x280004D4 = 0x%08lX\r\n", (unsigned long)*(volatile uint32_t *)0x280004D4UL);
    printf("  RFTESTTXSTAT @0x280004D8 = 0x%08lX\r\n", (unsigned long)*(volatile uint32_t *)0x280004D8UL);
    printf("  RFTESTRXSTAT @0x280004DC = 0x%08lX\r\n", (unsigned long)*(volatile uint32_t *)0x280004DCUL);
    printf("  INTSTAT0     @0x28000410 = 0x%08lX\r\n", (unsigned long)*(volatile uint32_t *)0x28000410UL);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_test_stat, bt_test_stat, Dump BR/EDR test registers);

#endif /* CONFIG_BTDM_BT_ENABLE */

/*
 * ============================================================================
 * AD9361 TX Power Control Commands
 * ============================================================================
 */
#ifdef CONFIG_RF_AD9361

/**
 * @brief 9361_tx_atten - Set AD9361 TX attenuation directly
 */
static void cmd_9361_tx_atten(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: 9361_tx_atten <atten_dB>\r\n");
        printf("  atten_dB: 0-89.75 (e.g., 10, 32.5)\r\n");
        /* Show current value */
        uint32_t cur = spi_9361_get_tx_atten();
        printf("  Current: %lu.%02lu dB\r\n",
               (unsigned long)(cur / 1000),
               (unsigned long)((cur % 1000) / 10));
        return;
    }

    /* Parse float-ish input: convert to milli-dB */
    double atten_db = strtod(argv[1], NULL);
    uint32_t atten_mdB = (uint32_t)(atten_db * 1000.0);

    spi_9361_set_tx_atten(atten_mdB);
}
SHELL_CMD_EXPORT_ALIAS(cmd_9361_tx_atten, 9361_tx_atten, Set AD9361 TX attenuation (dB));

/**
 * @brief 9361_tx_power - Set AD9361 TX power in dBm
 */
static void cmd_9361_tx_power(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: 9361_tx_power <power_dBm>\r\n");
        printf("  power_dBm: integer (e.g., 0, -10, -20)\r\n");
        return;
    }

    int8_t power_dbm = (int8_t)atoi(argv[1]);
    spi_9361_set_tx_power_dbm(power_dbm);
}
SHELL_CMD_EXPORT_ALIAS(cmd_9361_tx_power, 9361_tx_power, Set AD9361 TX power (dBm));

/**
 * @brief 9361_tx_atten_rd - Read current AD9361 TX attenuation
 */
static void cmd_9361_tx_atten_rd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uint32_t atten_mdB = spi_9361_get_tx_atten();
    uint16_t reg_lo = spi_9361_rd_cr(0x073) & 0xFF;
    uint16_t reg_hi = spi_9361_rd_cr(0x074) & 0x01;
    uint16_t reg_val = (reg_hi << 8) | reg_lo;

    printf("AD9361 TX Attenuation: %lu.%02lu dB (reg=0x%03x, raw=%u)\r\n",
           (unsigned long)(atten_mdB / 1000),
           (unsigned long)((atten_mdB % 1000) / 10),
           reg_val, reg_val);
}
SHELL_CMD_EXPORT_ALIAS(cmd_9361_tx_atten_rd, 9361_tx_atten_rd, Read AD9361 TX attenuation);

#endif /* CONFIG_RF_AD9361 */

/*
 * ============================================================================
 * T2 RF Commands (CCI register access via ipro6_rf)
 * ============================================================================
 */
#ifdef CONFIG_RF_T2

extern uint32_t rf_pri_read(uint32_t a);
extern void rf_pri_write(uint32_t a, uint32_t d);
extern void rf_pri_txtest_start(void);
extern void rf_pri_txtest_stop(void);
extern uint32_t rf_pri_check_lo_lock(void);
extern void rf_pri_txcal(void);

static void cmd_t2_rr(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: t2_rr <addr>  (hex, 0x optional)\r\n");
        return;
    }
    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 16);
    uint32_t val = rf_pri_read(addr);
    printf("T2 [0x%03lX] = 0x%08lX\r\n", (unsigned long)addr, (unsigned long)val);
}
SHELL_CMD_EXPORT_ALIAS(cmd_t2_rr, t2_rr, Read T2 RF register via CCI);

static void cmd_t2_wr(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: t2_wr <addr> <val>  (hex, 0x optional)\r\n");
        return;
    }
    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 16);
    uint32_t val = (uint32_t)strtoul(argv[2], NULL, 16);
    rf_pri_write(addr, val);
    printf("T2 [0x%03lX] <= 0x%08lX\r\n", (unsigned long)addr, (unsigned long)val);
}
SHELL_CMD_EXPORT_ALIAS(cmd_t2_wr, t2_wr, Write T2 RF register via CCI);

static void cmd_t2_tx_start(int argc, char **argv)
{
    (void)argc; (void)argv;
    rf_pri_txtest_start();
    printf("T2 TX test started\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_t2_tx_start, t2_tx_start, Start T2 TX test tone);

static void cmd_t2_tx_stop(int argc, char **argv)
{
    (void)argc; (void)argv;
    rf_pri_txtest_stop();
    printf("T2 TX test stopped\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_t2_tx_stop, t2_tx_stop, Stop T2 TX test tone);

static void cmd_t2_lo_lock(int argc, char **argv)
{
    (void)argc; (void)argv;
    uint32_t locked = rf_pri_check_lo_lock();
    printf("T2 LO lock: %s (%lu)\r\n", locked ? "LOCKED" : "UNLOCKED",
           (unsigned long)locked);
}
SHELL_CMD_EXPORT_ALIAS(cmd_t2_lo_lock, t2_lo_lock, Check T2 LO lock status);

static void cmd_t2_txcal(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("Running T2 TX calibration...\r\n");
    rf_pri_txcal();
    printf("T2 TX calibration done\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_t2_txcal, t2_txcal, Run T2 TX calibration);

#endif /* CONFIG_RF_T2 */

/*
 * ============================================================================
 * Register Read/Write Commands
 * ============================================================================
 */

#ifndef CONFIG_PHY_CLI
/* rr/wr/rb/wb register CLIs: when the PHY CLI is present it provides the
 * same commands (with bit-31-safe masking), so skip these to avoid
 * duplicate shell-symbol definitions. */
/**
 * @brief rr - Read 32-bit register
 */
static void cmd_rr(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: rr <address>\r\n");
        printf("Example: rr 30001400  (0x prefix optional)\r\n");
        return;
    }

    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 16);
    volatile uint32_t *reg = (volatile uint32_t *)addr;
    uint32_t val = *reg;

    printf("[0x%08lX] = 0x%08lX\r\n", addr, val);
}
SHELL_CMD_EXPORT_ALIAS(cmd_rr, rr, Read 32-bit register);

/**
 * @brief wr - Write 32-bit register
 */
static void cmd_wr(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: wr <address> <value>\r\n");
        printf("Example: wr 30001400 01d0ff20  (0x prefix optional)\r\n");
        return;
    }

    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 16);
    uint32_t value = (uint32_t)strtoul(argv[2], NULL, 16);
    volatile uint32_t *reg = (volatile uint32_t *)addr;
    *reg = value;

    printf("[0x%08lX] <= 0x%08lX\r\n", addr, value);
}
SHELL_CMD_EXPORT_ALIAS(cmd_wr, wr, Write 32-bit register);

/**
 * @brief rb - Read bits from register
 */
static void cmd_rb(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: rb <address> <msb> <lsb>\r\n");
        printf("Example: rb 30001400 23 16  (addr hex, msb/lsb decimal)\r\n");
        return;
    }

    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 16);
    uint32_t msb = (uint32_t)strtoul(argv[2], NULL, 10);
    uint32_t lsb = (uint32_t)strtoul(argv[3], NULL, 10);

    if (msb > 31 || lsb > 31 || msb < lsb) {
        printf("Error: Invalid bit positions (MSB=%lu, LSB=%lu)\r\n", msb, lsb);
        return;
    }

    uint32_t bit_width = msb - lsb + 1;
    uint32_t mask = ((1UL << bit_width) - 1);

    volatile uint32_t *reg = (volatile uint32_t *)addr;
    uint32_t reg_value = *reg;
    uint32_t bit_value = (reg_value >> lsb) & mask;

    printf("[0x%08lX][%lu:%lu] = 0x%lX (%lu)  reg=0x%08lX\r\n",
           addr, msb, lsb, bit_value, bit_value, reg_value);
}
SHELL_CMD_EXPORT_ALIAS(cmd_rb, rb, Read bits from register);

/**
 * @brief wb - Write bits to register
 */
static void cmd_wb(int argc, char **argv)
{
    if (argc < 5) {
        printf("Usage: wb <address> <msb> <lsb> <value>\r\n");
        printf("Example: wb 30001400 23 23 1  (addr/val hex, msb/lsb decimal)\r\n");
        return;
    }

    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 16);
    uint32_t msb = (uint32_t)strtoul(argv[2], NULL, 10);
    uint32_t lsb = (uint32_t)strtoul(argv[3], NULL, 10);
    uint32_t value = (uint32_t)strtoul(argv[4], NULL, 16);

    if (msb > 31 || lsb > 31 || msb < lsb) {
        printf("Error: Invalid bit positions (MSB=%lu, LSB=%lu)\r\n", msb, lsb);
        return;
    }

    uint32_t bit_width = msb - lsb + 1;
    uint32_t mask = ((1UL << bit_width) - 1) << lsb;
    uint32_t shifted_value = (value << lsb) & mask;

    volatile uint32_t *reg = (volatile uint32_t *)addr;
    uint32_t old_value = *reg;
    uint32_t new_value = (old_value & ~mask) | shifted_value;
    *reg = new_value;

    printf("[0x%08lX][%lu:%lu] <= 0x%lX  old=0x%08lX new=0x%08lX\r\n",
           addr, msb, lsb, value, old_value, new_value);
}
SHELL_CMD_EXPORT_ALIAS(cmd_wb, wb, Write bits to register);
#endif /* !CONFIG_PHY_CLI */

/*
 * ============================================================================
 * Debug Command
 * ============================================================================
 */

extern void btdm_dbg_dump(void);

/**
 * @brief btdm_debug - Dump debug counters and registers
 */
static void cmd_btdm_debug(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    btdm_dbg_dump();
}
SHELL_CMD_EXPORT_ALIAS(cmd_btdm_debug, btdm_debug, Dump BTDM debug info);

/*
 * ============================================================================
 * Help Command
 * ============================================================================
 */

/**
 * @brief btdm help - Show available commands
 */
static void cmd_btdm_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("BTDM CLI Commands:\r\n");
    printf("\r\n");
    printf("General Commands:\r\n");
    printf("  btdm_init              - Initialize BTDM controller\r\n");
    printf("  ble_enable [0]         - Enable BLE (clocks + DFE), 0=disable\r\n");
    printf("  btdm_reset             - Send HCI Reset\r\n");
    printf("  btdm_tx_power <dbm>    - Set TX power\r\n");
    printf("  btdm_status            - Show controller status\r\n");
    printf("  btdm_mac [addr]        - Show/Set BD address\r\n");
    printf("  btdm_debug             - Dump debug counters/registers\r\n");
    printf("  btdm_help              - Show this help\r\n");
    printf("\r\n");
    printf("Register Commands:\r\n");
    printf("  rr <addr>              - Read 32-bit register\r\n");
    printf("  wr <addr> <val>        - Write 32-bit register\r\n");
    printf("  rb <addr> <msb> <lsb>  - Read bits from register\r\n");
    printf("  wb <addr> <msb> <lsb> <val> - Write bits to register\r\n");
    printf("\r\n");
    printf("LE (BLE) Commands:\r\n");
    printf("  le_adv <start|stop>    - Control LE advertising\r\n");
    printf("  le_scan <start|stop>   - Control LE scanning\r\n");
    printf("  le_tx_test <ch> <len> <type> [phy] [tx_pwr] - LE TX test\r\n");
    printf("  le_rx_test <ch>        - LE RX test\r\n");
    printf("  le_test_end            - End LE test mode\r\n");
#ifdef CONFIG_BTDM_BT_ENABLE
    printf("\r\n");
    printf("BR/EDR Non-Signaling Test:\r\n");
    printf("  bt_tx_test <ch> <payload> <type> [pwr] [len] [inf] - TX test\r\n");
    printf("  bt_rx_test <ch> <type> [inf]  - RX test\r\n");
    printf("  bt_test_stop           - End BR/EDR test\r\n");
    printf("  bt_test_stat           - Dump BR/EDR test registers\r\n");
#endif
#ifdef CONFIG_RF_T2
    printf("\r\n");
    printf("T2 RF Commands:\r\n");
    printf("  t2_rr <addr>           - Read T2 RF register (CCI)\r\n");
    printf("  t2_wr <addr> <val>     - Write T2 RF register (CCI)\r\n");
    printf("  t2_tx_start            - Start T2 TX test tone\r\n");
    printf("  t2_tx_stop             - Stop T2 TX test tone\r\n");
    printf("  t2_lo_lock             - Check T2 LO lock status\r\n");
    printf("  t2_txcal               - Run T2 TX calibration\r\n");
#endif
#ifdef CONFIG_RF_AD9361
    printf("\r\n");
    printf("AD9361 Power Control:\r\n");
    printf("  9361_tx_atten <dB>     - Set TX attenuation (0-89.75)\r\n");
    printf("  9361_tx_power <dBm>    - Set TX power in dBm\r\n");
    printf("  9361_tx_atten_rd       - Read current TX attenuation\r\n");
#endif
}
SHELL_CMD_EXPORT_ALIAS(cmd_btdm_help, btdm_help, Show BTDM CLI help);

/**
 * @brief Initialize CLI commands
 */
void btdm_cli_init(void)
{
    printf("[BTDM_CLI] CLI commands registered\r\n");
}

#endif /* CONFIG_BTDM_ENABLE && CONFIG_SHELL */

/**
 * @brief btdm_probe - non-destructive per-block known-value probe
 *
 * Bring-up rule: prove "address decodes + clock runs" for every block BEFORE
 * trusting any readout from it. A void address reads 0/garbage and swallows
 * writes without any error. Each access announces itself first, so if the
 * bus hangs the console shows exactly which address is the culprit.
 *
 * Bases per the SoC memory map (REG_all MemMap tab):
 *   EM 0x28010000 / BLE core 0x28000800 / BT core 0x28000400
 *   BZ_PHY 0x20003000 / BZ_AGC 0x20004000
 *
 * Usage: btdm_probe        (run after btdm_init; BZ items also need the
 *                           BZ phy clocks enabled, otherwise they read 0
 *                           and are reported as SKIP-judgement)
 */
#define PROBE_EM_BASE        0x28010000UL
#define PROBE_BLE_VERSION    0x28000804UL
#define PROBE_BLE_VER_EXP    0x0B001200UL   /* reg_blecore.h BLE_VERSION_RESET */
#define PROBE_BT_BASE        0x28000400UL   /* RWBTCNTL */
#define PROBE_BT_VERSION     0x28000404UL
#define PROBE_BT_VER_EXP     0x0B000600UL   /* reg_btcore.h BT_VERSION_RESET */
#define PROBE_BT_RWBTCONF    0x28000408UL   /* read-only HW configuration */
#define PROBE_GLB_SRAM_CFG3  0x2000060CUL   /* [3:0] em_sel: WRAM/EM split */
#define PROBE_BZ_PHY_TOP0    0x20003800UL   /* PHY_TOP_0 */
#define PROBE_BZ_BT_CTRL0    0x200038C0UL   /* PHY_BT_CTRL_0: [25:16] programmed=716 */
#define PROBE_BZ_BT_AC_TH    716UL          /* bz_bt_init() production setting */
#define PROBE_BZ_AGC_CTRL0   0x20004C00UL   /* AGC_CTRL_0 */

static uint32_t probe_rd(uint32_t addr)
{
    printf("  rd  0x%08lX ... ", addr);
    uint32_t v = *(volatile uint32_t *)addr;
    printf("0x%08lX\r\n", v);
    return v;
}

static void cmd_btdm_probe(int argc, char **argv)
{
    (void)argc; (void)argv;
    int pass = 0, fail = 0;

    printf("=== BTDM non-destructive block probe ===\r\n");

    /* EM is live controller state once RW is initialized.  Never perform a
     * write/readback test here: EM[0] can contain the active advertising
     * descriptor and corrupting it stops radio scheduling.  Record two
     * aligned words only; destructive bus-width qualification belongs in a
     * dedicated pre-controller manufacturing image. */
    printf("[EM] live-state read only (not a RAM integrity test)\r\n");
    probe_rd(PROBE_EM_BASE);
    probe_rd(PROBE_EM_BASE + 0x1000);

    /* 3: BLE core version (RO, known reset value) */
    printf("[BLE] version @0x%08lX (expect 0x%08lX)\r\n",
           PROBE_BLE_VERSION, PROBE_BLE_VER_EXP);
    if (probe_rd(PROBE_BLE_VERSION) == PROBE_BLE_VER_EXP) {
        printf("[BLE] PASS\r\n"); pass++;
    } else {
        printf("[BLE] FAIL\r\n"); fail++;
    }

    /* 4: BT core. Reads only, in keeping with the rule above - these answer
     * whether the block decodes, whether it is the generation the register
     * headers describe, and whether this part was built dual-mode at all.
     * A zero dual-mode bit means there is no classic radio to bring up. */
    printf("[BT] version @0x%08lX (expect 0x%08lX)\r\n",
           PROBE_BT_VERSION, PROBE_BT_VER_EXP);
    if (probe_rd(PROBE_BT_VERSION) == PROBE_BT_VER_EXP) {
        printf("[BT] PASS\r\n"); pass++;
    } else {
        printf("[BT] FAIL (decode/clock/reset, or a different register generation)\r\n");
        fail++;
    }

    {
        uint32_t conf = probe_rd(PROBE_BT_RWBTCONF);
        printf("[BT] RWBTCONF: DMMODE=%lu WLANCOEX=%lu RFIF=0x%02lX CLK_SEL=%lu ADDR_WIDTH=%lu\r\n",
               (unsigned long)((conf >> 31) & 0x1UL),
               (unsigned long)((conf >> 29) & 0x1UL),
               (unsigned long)((conf >> 16) & 0x7FUL),
               (unsigned long)((conf >> 8) & 0x3FUL),
               (unsigned long)(conf & 0x1FUL));
        if ((conf >> 31) & 0x1UL) {
            printf("[BT] dual-mode core present\r\n"); pass++;
        } else {
            printf("[BT] FAIL: DMMODE=0, no dual-mode core on this part\r\n");
            fail++;
        }
    }

    /* The WRAM/exchange-memory split, decoded. Programmed at boot from the
     * span the linker reserved; if the two ever disagree the top of the WiFi
     * RAM window overlaps exchange memory and the damage surfaces elsewhere.
     * Reading it costs nothing and puts the number on the console. */
    {
        uint32_t em_sel = probe_rd(PROBE_GLB_SRAM_CFG3) & 0xFUL;
        uint32_t em_kb = 0, i;

        for (i = 0; i < 4; i++) {          /* each bit carves 16 KB */
            if (em_sel & (1UL << i)) {
                em_kb += 16;
            }
        }
        printf("[EM] em_sel=0x%lX -> %lu KB carved from the top of WRAM\r\n",
               (unsigned long)em_sel, (unsigned long)em_kb);
        if (em_kb == 0) {
            printf("[EM] FAIL: nothing carved, exchange memory is unmapped\r\n");
            fail++;
        }
    }

    /* 5/6: BZ PHY / BZ AGC — need BZ phy clocks; all-zero => likely clock off */
    printf("[BZ_PHY] PHY_TOP_0 + PHY_BT_CTRL_0[25:16] "
           "(expect %lu/0x%03lX after bz_phy_reset)\r\n",
           PROBE_BZ_BT_AC_TH, PROBE_BZ_BT_AC_TH);
    uint32_t top0 = probe_rd(PROBE_BZ_PHY_TOP0);
    uint32_t acth = (probe_rd(PROBE_BZ_BT_CTRL0) >> 16) & 0x3FF;
    if (acth == PROBE_BZ_BT_AC_TH) {
        printf("[BZ_PHY] PASS (ac_th programmed value matched)\r\n"); pass++;
    } else if (top0 == 0 && acth == 0) {
        printf("[BZ_PHY] SKIP-judgement (reads 0: clock likely off — retry after clk_phy_en)\r\n");
    } else {
        printf("[BZ_PHY] FAIL (unexpected values)\r\n"); fail++;
    }

    /* BZ_AGC: known-value check - AGC_FE_LOOP_1_BLEC (0x20004C84) reset is
     * the production-tuned FE loop word (cross-checked against the older
     * chip golden which programs the same fields by FW). Valid only after
     * BZ clocks are on; all-zero reads report SKIP. */
    {
        uint32_t v = probe_rd(0x20004C84UL);
        printf("[BZ_AGC] AGC_FE_LOOP_1_BLEC (expect 0x12081029 after clk on)\r\n");
        if (v == 0x12081029UL) { printf("[BZ_AGC] PASS\r\n"); pass++; }
        else if (v == 0)         printf("[BZ_AGC] SKIP (BZ clocks off?)\r\n");
        else                   { printf("[BZ_AGC] FAIL (got 0x%08lX)\r\n", v); fail++; }
    }
    printf("=== probe done: PASS=%d FAIL=%d ===\r\n", pass, fail);
    if (fail) {
        printf("!! stop here and check decode/clock/reset before trusting this block\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_btdm_probe, btdm_probe, Per-block address/clock probe);
