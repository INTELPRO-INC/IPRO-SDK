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
 * @file throughput_app.h
 * @brief IPRO BLE Throughput Test Application Header
 *
 * Uses native Zephyr bt_* APIs for all BLE operations.
 * One board acts as Master (Central), the other as Slave (Peripheral).
 */

#ifndef THROUGHPUT_APP_H
#define THROUGHPUT_APP_H

#include <stdint.h>
#include <stdbool.h>

struct bt_conn;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * DEFINITIONS
 ****************************************************************************************
 */

/// Device role in throughput test
typedef enum {
    THROUGHPUT_ROLE_IDLE = 0,
    THROUGHPUT_ROLE_SLAVE,      ///< Peripheral/Slave - advertises and receives data
    THROUGHPUT_ROLE_MASTER,     ///< Central/Master - scans, connects, and sends data
} throughput_role_t;

/// Test state
typedef enum {
    THROUGHPUT_STATE_IDLE = 0,
    THROUGHPUT_STATE_ADVERTISING,
    THROUGHPUT_STATE_SCANNING,
    THROUGHPUT_STATE_CONNECTING,
    THROUGHPUT_STATE_CONNECTED,
    THROUGHPUT_STATE_DISCOVERING,
    THROUGHPUT_STATE_READY,
    THROUGHPUT_STATE_TESTING,
} throughput_state_t;

/// Test direction
typedef enum {
    THROUGHPUT_DIR_TX = 0,      ///< This device transmits
    THROUGHPUT_DIR_RX,          ///< This device receives
    THROUGHPUT_DIR_BIDIR,       ///< Bidirectional
} throughput_dir_t;

/// Test configuration
typedef struct {
    uint32_t duration_sec;      ///< Test duration in seconds (0 = continuous)
    uint16_t packet_size;       ///< Packet size (20-244 bytes)
    uint16_t interval_ms;       ///< Packet interval in ms (0 = fastest)
    throughput_dir_t direction; ///< Test direction
    bool use_notification;      ///< Use notification (true) or write_cmd (false)
} throughput_test_config_t;

/// Test statistics
typedef struct {
    uint32_t start_time_ms;     ///< Test start timestamp
    uint32_t duration_ms;       ///< Total test duration
    uint32_t tx_packets;        ///< Number of packets transmitted
    uint32_t tx_bytes;          ///< Total bytes transmitted
    uint32_t rx_packets;        ///< Number of packets received
    uint32_t rx_bytes;          ///< Total bytes received
    uint32_t errors;            ///< Number of errors
    float tx_throughput_kbps;   ///< TX throughput in Kbps
    float rx_throughput_kbps;   ///< RX throughput in Kbps
} throughput_stats_t;

/// Application context
typedef struct {
    throughput_role_t role;
    throughput_state_t state;
    struct bt_conn *conn;

    // Service handles (for slave - not needed with static service, kept for info)
    uint16_t tx_char_handle;    ///< TX characteristic (notify to master)
    uint16_t rx_char_handle;    ///< RX characteristic (receive from master)

    // Client handles (for master - discovered from slave)
    uint16_t client_tx_handle;  ///< Handle to write to slave
    uint16_t client_rx_handle;  ///< Handle to receive notifications from slave
    uint16_t client_cccd_handle; ///< CCCD handle for notifications

    // Test configuration and stats
    throughput_test_config_t test_config;
    throughput_stats_t stats;
    bool test_running;

    // Target device for master
    bool target_found;
    bool has_target_addr;

    // Connection parameters
    uint16_t conn_interval;     ///< Current connection interval (1.25ms units)
    uint16_t mtu_size;          ///< Current MTU size
    uint8_t phy;                ///< Current PHY (1=1M, 2=2M, 4=Coded)

    // TX buffer
    uint8_t tx_buffer[256];
    uint16_t tx_seq_num;
} throughput_app_ctx_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

int throughput_app_init(void);
int throughput_app_start_slave(void);
int throughput_app_start_master(const char *target_addr);
int throughput_app_stop(void);
int throughput_app_start_test(const throughput_test_config_t *config);
int throughput_app_stop_test(void);
int throughput_app_get_stats(throughput_stats_t *stats);
void throughput_app_print_stats(void);
throughput_app_ctx_t *throughput_app_get_ctx(void);
int throughput_app_update_conn_params(uint16_t interval_min, uint16_t interval_max,
                                       uint16_t latency, uint16_t timeout);
int throughput_app_set_tx_interval(uint32_t interval_ms);
int throughput_app_set_phy(uint8_t phy);
void throughput_app_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif // THROUGHPUT_APP_H
