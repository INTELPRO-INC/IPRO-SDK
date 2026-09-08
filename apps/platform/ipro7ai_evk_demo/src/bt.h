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
 * @file bt.h
 * @brief Bluetooth LE bring-up: new controller + new host, with LE Audio.
 *
 * Controller: components/wireless/bluetooth/ipro_ble_controller - RivieraWaves
 * v14.0.2 with the upstream tree kept read-only, built from source here
 * (CONFIG_IPRO_BLE_CTLR_FORCE_SOURCE_BUILD), talking H4 byte-stream HCI.
 * Host: components/wireless/bluetooth/ipro_bt_host - upstream Zephyr v4.4.0
 * (CONFIG_BT_HOST_IPRO_V44).
 *
 * The LE Audio side registers as a unicast SINK (the headset role): PACS
 * publishing an LC3 sink capability, and a BAP unicast server with ASCS so a
 * phone can configure and start a stream to this device.
 *
 * The whole module compiles away when CONFIG_BLUETOOTH_LE_ENABLE is off.
 */
#ifndef BT_H
#define BT_H

#include <stdint.h>
#include <stdbool.h>

#define BT_APP_OK          0
#define BT_APP_ERR_STATE  -1
#define BT_APP_ERR_HW     -2

/** Where the LE Audio sink stream currently is. */
typedef enum {
    BT_STREAM_IDLE = 0,
    BT_STREAM_CONFIGURED,
    BT_STREAM_QOS_SET,
    BT_STREAM_ENABLED,
    BT_STREAM_STREAMING,
    BT_STREAM_STOPPED,
} bt_stream_state_t;

typedef struct {
    bool     enabled;          /* bt_enable() completed                 */
    bool     advertising;
    bool     connected;
    char     addr[32];         /* peer address, when connected          */
    uint16_t conn_interval;    /* in 1.25 ms units                      */
    uint16_t conn_latency;
    uint16_t conn_timeout;     /* in 10 ms units                        */
    uint8_t  own_addr[6];
    bt_stream_state_t stream;
    uint32_t rx_frames;        /* LC3 frames delivered by the stack     */
    uint32_t rx_bytes;
    uint32_t rx_empty;         /* SDUs concealed: empty or flagged bad  */
    uint32_t rx_flagged;       /* of those, ones the controller flagged */
} bt_status_t;

/** @brief Bring up the controller and host. Safe to call twice. */
int bt_app_init(void);

/** @brief Bring the radio up at boot on a detached task, so the remote clients
 *         have something to connect to without a serial console first. */
void bt_app_autostart(void);

/** @brief Connectable advertising, with the LE Audio service UUIDs. */
int bt_app_adv_start(void);
int bt_app_adv_stop(void);

/** @brief Drop the current connection, if any. */
int bt_app_disconnect(void);

/** @brief Advertised name. Takes effect on the next advertising start. */
int  bt_app_set_name(const char *name);
const char *bt_app_get_name(void);

void bt_app_get_status(bt_status_t *out);

/** @brief Human-readable stream state. */
const char *bt_app_stream_state_name(bt_stream_state_t s);

/**
 * @brief Restrict the LC3 sink rates published in PACS.
 *
 * A bring-up lever: pinning the sink to 16 kHz takes this app's whole DSP
 * chain out of the path - the decoder upsamples to the DAC rate internally,
 * so no anti-alias FIR runs and the rate matcher sits at ~1:1. If the audio is
 * still wrong at 16 kHz, the conversion is not what is wrong with it.
 *
 * @param khz  16, 24, 32, 48, or 0 for all of them
 * @return 0 on success
 */
int bt_app_set_sink_rate(unsigned khz);

/**
 * @brief Drive the VCP volume locally, as if a remote controller had set it.
 *
 * Separates "the phone is not speaking VCP" from "the volume path does not
 * work" - if this changes the level and the phone's slider does not, the
 * renderer is fine and the phone simply is not using it.
 *
 * @param volume 0-255
 */
void bt_app_set_volume(uint8_t volume);

#endif /* BT_H */
