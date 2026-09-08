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

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

/* Firmware version */
#define APP_VERSION             "1.0.0"

/* BLE settings */
#define DEFAULT_DEVICE_NAME     "IPRO-Trans"
#define MAX_DEVICE_NAME_LEN     20
#define DEFAULT_ADV_INT_MIN     160   /* 100ms (N * 0.625ms) */
#define DEFAULT_ADV_INT_MAX     320   /* 200ms */
#define DEFAULT_CONN_INT_MIN    12    /* 15ms  (N * 1.25ms) */
#define DEFAULT_CONN_INT_MAX    24    /* 30ms  */
#define DEFAULT_TX_POWER        0     /* 0 dBm */
#define DEFAULT_PASSKEY         123456
#define DEFAULT_BONDABLE        true

/* Transport selection (from Kconfig or compile definitions) */
/* CONFIG_AT_TRANSPORT_CDC - defined in CMakeLists.txt when CDC is selected */
/* CONFIG_AT_TRANSPORT_UART - defined in CMakeLists.txt when UART is selected (default) */

/* UART settings (when UART transport) */
#define BRIDGE_UART_ID          UART1_INDEX
#define DEFAULT_UART_BAUDRATE   115200

/* CDC settings (when CDC transport) */
#define CDC_IN_EP               0x81
#define CDC_OUT_EP              0x02
#define CDC_INT_EP              0x83
#define CDC_MAX_MPS             64

/* Buffer settings */
#define AT_CMD_MAX_LEN          128
#define AT_RESP_MAX_LEN         256
#define UART_RX_RING_SIZE       1024
#define UART_TX_RING_SIZE       1024
#define BLE_TX_BUF_SIZE         512

/* Data mode settings */
#define DATA_MODE_PACK_TIMEOUT_MS   20   /* Flush after 20ms idle */
#define DATA_MODE_ESCAPE_CHAR       '+'
#define DATA_MODE_ESCAPE_COUNT      3    /* +++ to escape */
#define DATA_MODE_ESCAPE_GUARD_MS   500  /* Guard silence before/after +++ */

/* EasyFlash keys for persistent config */
#define EF_KEY_DEVICE_NAME      "bt_name"
#define EF_KEY_UART_BAUDRATE    "uart_baud"
#define EF_KEY_TX_POWER         "tx_pwr"
#define EF_KEY_ADV_INTERVAL     "adv_int"
#define EF_KEY_PASSKEY          "passkey"
#define EF_KEY_BONDABLE         "bondable"

#endif /* __APP_CONFIG_H__ */
