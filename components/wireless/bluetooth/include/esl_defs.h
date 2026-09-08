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
 * @file esl_defs.h
 * @brief Shared ESL (Electronic Shelf Label) protocol definitions
 *
 * Common opcodes, response codes, and PAwR packet formats used by
 * both ESL Tag and ESL Gateway (Access Point).
 */

#ifndef ESL_DEFS_H
#define ESL_DEFS_H

#include <stdint.h>

/* Auto-pull a per-app PAwR sizing override if one is on the include
 * path. Each ESL app family ships its own `<family>_pawr_cfg.h`
 * (added to the include search via CMakeLists.txt). Defining the
 * sizing constants there before this file is parsed lets the
 * `#ifndef` guards below skip the legacy defaults.
 *
 * Examples:
 *   apps/bluetooth/esl/sig_esl_common/sig_esl_pawr_cfg.h  (SIG ESL pair)
 *   apps/turnkey/robust/.../phm_pawr_cfg.h                 (PHM pair)
 *
 * Apps that don't ship a cfg header fall back to the 4-sub × 1-slot
 * legacy layout defined below — same as the SDK has always used. */
#if defined(__has_include)
#  if __has_include("sig_esl_pawr_cfg.h")
#    include "sig_esl_pawr_cfg.h"
#  elif __has_include("phm_pawr_cfg.h")
#    include "phm_pawr_cfg.h"
#  elif __has_include("esl_app_pawr_cfg.h")
#    include "esl_app_pawr_cfg.h"
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * ESL Service UUID
 ******************************************************************************/

#define ESL_SERVICE_UUID            0x1857

/*******************************************************************************
 * ESL Control Point Opcodes
 *
 * Layout aligned with the Bluetooth SIG ESL Profile v1.0.1. Both the GATT
 * ECP characteristic (0x2BFE) and PAwR subevent payloads use this same
 * opcode set. Vendor-specific extensions live in the 0xC0..0xFF range so
 * they cannot collide with future SIG-reserved values.
 *
 * SIG-standard (verified against Nordic nrf-esl-bluetooth/service/esl_internal.h):
 ******************************************************************************/

/* --- SIG-standard ESL opcodes ----------------------------------------- */
#define ESL_OP_PING                 0x00
#define ESL_OP_UNASSOCIATE          0x01
#define ESL_OP_SERVICE_RESET        0x02
#define ESL_OP_FACTORY_RESET        0x03
#define ESL_OP_UPDATE_COMPLETE      0x04
#define ESL_OP_READ_SENSOR          0x10
#define ESL_OP_REFRESH_DISPLAY      0x11
#define ESL_OP_DISPLAY_IMAGE        0x20    /* payload: [img_idx 1B] */
#define ESL_OP_DISPLAY_TIMED_IMAGE  0x60    /* payload: [img_idx 1B][abs_time 4B] */
#define ESL_OP_LED_CONTROL          0xB0
#define ESL_OP_LED_TIMED_CONTROL    0xF0

/* --- Vendor-specific (IPRO) ------------------------------------------- */
/* PROVISION carries the AP_Sync_Key/Response_Key/abs_time/esl_addr in a
 * single PAwR broadcast PDU, allowing connectionless join. Keep the
 * existing 0x30 — SIG profile mandates GATT-based provisioning, so this
 * opcode in the PAwR direction is genuinely vendor-only. */
#define ESL_OP_PROVISION            0x30

/* SET_PRICE renders a template+price string on the e-paper. Moved to
 * vendor range so future SIG assignments at 0x21 (currently unused but
 * inside the Display opcode group 0x20..0x5F) cannot conflict. */
#define ESL_OP_SET_PRICE            0xC1    /* payload: [tpl_idx 1B][seq 1B][price_str N B] */

/*******************************************************************************
 * ESL Response Codes
 ******************************************************************************/

#define ESL_RSP_SUCCESS             0x00
#define ESL_RSP_INVALID_OPCODE      0x01
#define ESL_RSP_INVALID_STATE       0x02
#define ESL_RSP_INVALID_PARAM       0x03
#define ESL_RSP_NOT_AVAILABLE       0x04
#define ESL_RSP_RETRY               0x05

/*******************************************************************************
 * PAwR Addressing
 *
 * Per Bluetooth SIG ESL Profile §3.7 the 16-bit ESL address is split:
 *
 *   bit 15      : RFU (must be 0 on assigned addresses)
 *   bits 14..8  : Group ID (0..127)
 *   bits 7..0   : Fixed Address within group (0..254; 0xFF reserved for
 *                 group broadcast — "all tags in this group")
 *
 *   0xFFFF      : Global broadcast (every tag in every group)
 *   0xFFFE      : Unassigned (our own sentinel for "not yet provisioned")
 *
 * Examples:
 *   0x0000      : group 0, addr 0    (single tag)
 *   0x00FF      : group 0, broadcast (all tags in group 0)
 *   0x0301      : group 3, addr 1
 *   0x03FF      : group 3, broadcast (all tags in group 3)
 ******************************************************************************/

#define ESL_ADDR_GLOBAL_BROADCAST   0xFFFF
#define ESL_ADDR_UNASSIGNED         0xFFFE
#define ESL_ADDR_GROUP_BCAST_FIXED  0xFF

#define ESL_ADDR_GROUP(addr)        (((addr) >> 8) & 0x7F)
#define ESL_ADDR_FIXED(addr)        ((addr) & 0xFF)
#define ESL_ADDR_MAKE(grp, fix)     ((uint16_t)((((grp) & 0x7F) << 8) | ((fix) & 0xFF)))
#define ESL_ADDR_IS_GROUP_BCAST(addr) (ESL_ADDR_FIXED(addr) == ESL_ADDR_GROUP_BCAST_FIXED && \
                                       (addr) != ESL_ADDR_GLOBAL_BROADCAST)

/* Legacy aliases — keep until call sites migrate. */
#define ESL_PAWR_ADDR_BROADCAST     ESL_ADDR_GLOBAL_BROADCAST
#define ESL_PAWR_ADDR_UNASSIGNED    ESL_ADDR_UNASSIGNED

/*******************************************************************************
 * PAwR Parameters (BLE 5.4)
 *
 * 32 sensors, 1-second polling cycle:
 *   interval      = 800  (1000 ms)
 *   32 subevents, subevent_interval = 24 (30 ms)
 *   response_slot_delay = 12 (15 ms) — RF turnaround; host uses evt+1 retry
 *   response_slot_spacing = 8 (10 ms) — fits 66-byte PHM payload at 1Mbps
 *   4 response slots per subevent (push model: up to 4 sensors per subevent)
 *
 * Budget per subevent: indication(~0.5ms) + delay(15ms) + 4×slot(40ms) = 55.5ms < 30ms
 * NOTE: With 4 slots the subevent budget is tight — increase subevent_interval if needed
 ******************************************************************************/

/* PAwR sizing — DEPLOYMENT-SPECIFIC defaults.
 *
 * Each ESL family (SIG pricetag, PHM sensor, IMU streamer, …) has its
 * own optimal sizing, so every constant here is `#ifndef`-guarded.
 * The defaults below correspond to the legacy 4-subevent layout used
 * by the older ipro_esl_* apps and the LP-spike PHM bring-up.
 *
 * To use a different sizing in your app, define the constants BEFORE
 * including esl_defs.h (typically via a per-app cfg header that you
 * include first). Example for the SIG ESL pricetag pair that supports
 * 128 tags:
 *
 *     #include "sig_esl_pawr_cfg.h"   // overrides ESL_PAWR_*
 *     #include "esl_defs.h"           // protocol bits only
 *
 * AP and Tag of the same pair MUST agree on every constant or PAwR
 * timing diverges.
 *
 * Real production deployments where tags sleep >95% of the time
 * should push ESL_PAWR_INTERVAL toward 4000 (5s) or 8000 (10s) for
 * battery-life targets; the floor is dictated by airtime
 * (NUM_SUBEVENTS × SUBEVENT_INTERVAL + AP RX margin).
 */
#ifndef ESL_PAWR_INTERVAL
#define ESL_PAWR_INTERVAL               800   /* 1000 ms */
#endif
#ifndef ESL_PAWR_NUM_SUBEVENTS
#define ESL_PAWR_NUM_SUBEVENTS          4
#endif
#ifndef ESL_PAWR_SUBEVENT_INTERVAL
#define ESL_PAWR_SUBEVENT_INTERVAL      24    /* 30 ms */
#endif
#ifndef ESL_PAWR_RESPONSE_SLOT_DELAY
#define ESL_PAWR_RESPONSE_SLOT_DELAY    12    /* 15 ms — RF turnaround + margin */
#endif
#ifndef ESL_PAWR_RESPONSE_SLOT_SPACING
#define ESL_PAWR_RESPONSE_SLOT_SPACING  8     /* 10 ms */
#endif
#ifndef ESL_PAWR_NUM_RESPONSE_SLOTS
#define ESL_PAWR_NUM_RESPONSE_SLOTS     1
#endif

/*******************************************************************************
 * PAwR Packet Formats
 *
 * Subevent data (AP → Tag):
 *   [target_esl_addr (2 bytes LE)] [opcode (1 byte)] [payload (0..N bytes)]
 *
 * Response data (Tag → AP):
 *   [source_esl_addr (2 bytes LE)] [status (1 byte)] [payload (0..N bytes)]
 ******************************************************************************/

#define ESL_PAWR_PDU_HEADER_SIZE    3   /* addr(2) + opcode/status(1) */
#define ESL_PAWR_MAX_PAYLOAD        248 /* BLE max periodic adv data - header */

/** PAwR subevent PDU: AP sends to Tag */
typedef struct __attribute__((packed)) {
    uint16_t target_addr;   /**< Target ESL address (LE), or BROADCAST */
    uint8_t  opcode;        /**< ESL opcode */
    uint8_t  payload[];     /**< Variable-length payload */
} esl_pawr_subevent_pdu_t;

/** PAwR response PDU: Tag sends to AP */
typedef struct __attribute__((packed)) {
    uint16_t source_addr;   /**< Source ESL address (LE) */
    uint8_t  status;        /**< ESL response code */
    uint8_t  payload[];     /**< Variable-length payload */
} esl_pawr_response_pdu_t;

/*******************************************************************************
 * ESL Sensor Types
 ******************************************************************************/

#define ESL_SENSOR_TYPE_BATTERY     0x01
#define ESL_SENSOR_TYPE_TEMPERATURE 0x02
#define ESL_SENSOR_TYPE_HUMIDITY    0x03
#define ESL_SENSOR_TYPE_LIGHT       0x04
#define ESL_SENSOR_TYPE_PRESSURE    0x05

/** Single sensor reading: type (1B) + value (4B) = 5 bytes */
typedef struct __attribute__((packed)) {
    uint8_t  type;      /**< ESL_SENSOR_TYPE_* */
    uint32_t value;     /**< Raw sensor value (type-dependent units) */
} esl_sensor_reading_t;

/** Single-sensor response payload: 10 bytes */
typedef struct __attribute__((packed)) {
    uint8_t  type;          /**< ESL_SENSOR_TYPE_* */
    uint32_t value;         /**< Sensor value */
    uint8_t  battery;       /**< Battery level (0–100 %) */
    uint32_t timestamp;     /**< Seconds since epoch / boot */
} esl_sensor_single_rsp_t;

/** Multi-sensor response payload: 1 + N*5 + 4 bytes */
typedef struct __attribute__((packed)) {
    uint8_t              count;         /**< Number of entries */
    esl_sensor_reading_t entries[];     /**< Sensor entries (count × 5B) */
    /* uint32_t timestamp follows entries[] — access via pointer arithmetic */
} esl_sensor_multi_rsp_t;

/*******************************************************************************
 * ESL Uplink (Tag → Gateway) via Non-connectable Ext Adv
 *
 * Since the controller doesn't support PAwR response slots, tags send
 * responses/sensor reports via non-connectable ext adv with service data.
 *
 * AD format: [len][type=0x16 Service Data][UUID_LE(2)][msg_type(1)][esl_addr(2)][payload]
 ******************************************************************************/

#define ESL_UPLINK_MSG_CMD_RSP      0x01    /**< Command response */
#define ESL_UPLINK_MSG_SENSOR_RPT   0x02    /**< Periodic sensor report */

/** Uplink PDU header: msg_type(1) + esl_addr(2) = 3 bytes (after UUID) */
#define ESL_UPLINK_HEADER_SIZE      3

/** Uplink PDU (in service data, after UUID16 LE) */
typedef struct __attribute__((packed)) {
    uint8_t  msg_type;      /**< ESL_UPLINK_MSG_* */
    uint16_t esl_addr;      /**< Source ESL address (LE) */
    uint8_t  payload[];     /**< Response status or sensor data */
} esl_uplink_pdu_t;

/** Tag response adv duration in ms */
#define ESL_UPLINK_ADV_DURATION_MS  500

/** Periodic sensor report interval in seconds */
#define ESL_SENSOR_REPORT_INTERVAL_S  30

/** Gateway scan window for uplink (ms) */
#define ESL_GW_SCAN_WINDOW_MS       100
#define ESL_GW_SCAN_INTERVAL_MS     200

/*******************************************************************************
 * PAwR Provisioning (AP → Tag via PAwR subevent, Tag → AP via response slot)
 *
 * The gateway uses ESL_OP_PROVISION (vendor-specific, defined above with the
 * other opcodes) to push all provisioning material (ESL address, sync key,
 * response key, time) to a tag in a single PAwR subevent, replacing the
 * multi-step GATT write sequence the SIG profile mandates. The tag
 * acknowledges via the PAwR response slot.
 ******************************************************************************/

/** Uplink ACK for provisioning — payload is the assigned ESL address (2B) */
#define ESL_UPLINK_MSG_PROVISION_ACK    0x03

/** Uplink JOIN request — sent by unprovisioned tags to announce presence.
 *  Payload: 6-byte BLE MAC address (little-endian).
 *  Gateway auto-assigns an ESL address and sends ESL_OP_PROVISION in response. */
#define ESL_UPLINK_MSG_JOIN_REQ         0x05

/** PAwR provisioning PDU: AP → Tag (44 bytes total)
 *
 *  target_mac is the tag's BLE MAC (little-endian, the same byte order the
 *  controller reports it). On the provisioning channel (subevent 0,
 *  target_addr=ESL_PAWR_ADDR_UNASSIGNED), every unprovisioned tag receives
 *  the broadcast, but only the tag whose MAC matches target_mac applies the
 *  payload — making provisioning safely directed without occupying an
 *  esl_addr-keyed subevent before the tag actually has an address. */
typedef struct __attribute__((packed)) {
    uint8_t  target_mac[6];     /**< 6B — target tag's MAC (LE byte order) */
    uint16_t esl_addr;          /**< 2B — assigned ESL address */
    uint8_t  ap_sync_key[16];   /**< 16B — AP sync key material */
    uint8_t  response_key[16];  /**< 16B — response key material */
    uint32_t abs_time;          /**< 4B — absolute time sync */
} esl_provision_pdu_t;          /**< Total: 44 bytes */

/*******************************************************************************
 * ESL GATT Characteristic UUIDs (for reference)
 ******************************************************************************/

#define ESL_UUID_ADDR               0x2BF6
#define ESL_UUID_AP_SYNC_KEY        0x2BF7
#define ESL_UUID_RESP_KEY           0x2BF8
#define ESL_UUID_ABS_TIME           0x2BF9
#define ESL_UUID_DISPLAY_INFO       0x2BFA
#define ESL_UUID_IMAGE_INFO         0x2BFB
#define ESL_UUID_SENSOR_INFO        0x2BFC
#define ESL_UUID_LED_INFO           0x2BFD
#define ESL_UUID_CONTROL_POINT      0x2BFE

#ifdef __cplusplus
}
#endif

#endif /* ESL_DEFS_H */
