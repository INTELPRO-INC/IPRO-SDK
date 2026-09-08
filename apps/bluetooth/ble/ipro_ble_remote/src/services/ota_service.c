/**
 * @file ota_service.c
 * @brief BLE OTA Service Implementation
 *
 * Implements secure firmware update over BLE using dual-partition boot.
 *
 * OTA Process Flow:
 * 1. Phone sends START command with firmware size
 * 2. Phone sends firmware data in chunks via DATA characteristic
 * 3. Phone sends END command with CRC32
 * 4. Device verifies CRC32
 * 5. Phone sends REBOOT command
 * 6. Device switches partition and reboots
 *
 * Copyright (C) IPRO SDK 2025
 */

#include "ota_service.h"
#include "ipro_log.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// Zephyr BLE includes
#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// HAL includes for flash and boot management
#include "hal_boot2.h"
#include "hal_flash.h"
#include "hal_sys.h"
#include "soc_base.h"
#include "drv_glb.h"

#define LOG_TAG "OTA_SVC"

/*
 * ZEPHYR BLE UUID DEFINITIONS
 ****************************************************************************************
 */

// OTA Service UUID (16-bit: 0xFF60)
#define BT_UUID_OTA_SVC_VAL     BT_UUID_16_ENCODE(OTA_SVC_UUID16)
#define BT_UUID_OTA_SVC         BT_UUID_DECLARE_16(OTA_SVC_UUID16)

// OTA Control Characteristic UUID (16-bit: 0xFF61)
#define BT_UUID_OTA_CTRL_VAL    BT_UUID_16_ENCODE(OTA_CTRL_CHAR_UUID16)
#define BT_UUID_OTA_CTRL        BT_UUID_DECLARE_16(OTA_CTRL_CHAR_UUID16)

// OTA Data Characteristic UUID (16-bit: 0xFF62)
#define BT_UUID_OTA_DATA_VAL    BT_UUID_16_ENCODE(OTA_DATA_CHAR_UUID16)
#define BT_UUID_OTA_DATA        BT_UUID_DECLARE_16(OTA_DATA_CHAR_UUID16)

// OTA Status Characteristic UUID (16-bit: 0xFF63)
#define BT_UUID_OTA_STATUS_VAL  BT_UUID_16_ENCODE(OTA_STATUS_CHAR_UUID16)
#define BT_UUID_OTA_STATUS      BT_UUID_DECLARE_16(OTA_STATUS_CHAR_UUID16)

/*
 * OTA CONFIGURATION
 ****************************************************************************************
 */

// Maximum attribute sizes
#define OTA_MAX_CTRL_SIZE        6      // Command (1 byte) + Size/CRC (4 bytes) + Mode (1 byte, optional)
#define OTA_PACKET_PAYLOAD_SIZE  512    // Maximum payload size (when MTU=517)
#define OTA_MAX_DATA_SIZE        (OTA_PACKET_PAYLOAD_SIZE + 6)  // 518 bytes (payload + 6 bytes header)
#define OTA_MAX_STATUS_SIZE      128    // Status notification
#define OTA_MIN_PAYLOAD_SIZE     20     // Minimum payload size (for MTU=23)
#define OTA_PACKET_HEADER_SIZE   6      // Packet header: seq(2) + payload_size(2) + flags(2)
#define ATT_HEADER_SIZE          3      // ATT protocol header overhead

// Packet configuration
#define OTA_MAX_PACKETS          2048   // Maximum packets (support up to 1024KB firmware = 2048 * 512)

// Flash write buffer optimization
#define OTA_FLASH_BUFFER_SIZE    4096   // 4KB buffer for batching flash writes

/*
 * OTA DATA PACKET FORMAT (for WRITE_NO_RESPONSE reliable transfer)
 ****************************************************************************************
 */

/// OTA data packet header
typedef struct __attribute__((packed)) {
    uint16_t seq_number;    ///< Packet sequence number (0 to N-1)
    uint16_t payload_size;  ///< Actual payload size in this packet
    uint16_t reserved;      ///< Reserved for future use (e.g., checksum)
    uint8_t  payload[];     ///< Firmware data (variable length)
} ota_packet_t;

/*
 * OTA FLASH BUFFERING STATE
 ****************************************************************************************
 */

/// Flash write buffer for sector-aligned batching (ACK-per-packet mode)
typedef struct {
    uint8_t  *data;                              ///< 4KB buffer (dynamically allocated)
    uint32_t sector_base_addr;                   ///< Base flash address of this sector
    uint16_t packets_in_sector;                  ///< Number of packets that fit in this sector
    uint32_t received_mask;                      ///< Bitmap: which packets in sector are received
    uint16_t received_count;                     ///< Count of received packets in this sector
    bool     active;                             ///< True if buffer is tracking a sector
    uint8_t  pending_data[OTA_PACKET_PAYLOAD_SIZE]; ///< Pending bytes from cross-boundary packet
    uint16_t pending_size;                       ///< Size of pending data (0 if none)
    uint32_t start_time_ms;                      ///< Transfer start time (for speed calculation)
} ota_flash_buffer_t;

static ota_flash_buffer_t g_flash_buffer = {0};

/*
 * OTA SERVICE STATE
 ****************************************************************************************
 */

static ota_context_t g_ota_ctx = {
    .active = false,
    .status = OTA_STATUS_IDLE,
    .error = OTA_ERR_NONE,
    .progress = 0,
};

static bool g_ota_status_ntf_enabled = false;
static struct bt_conn *g_ota_conn = NULL;

// MTU tracking for dynamic packet size adjustment
static uint16_t g_current_mtu = 23;  // Default minimum MTU
static uint16_t g_max_payload_size = OTA_MIN_PAYLOAD_SIZE;

/*
 * OTA TASK FOR ASYNC FLASH OPERATIONS
 ****************************************************************************************
 */
#define OTA_TASK_STACK_SIZE 1024
#define OTA_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)

static TaskHandle_t g_ota_task_handle = NULL;
static QueueHandle_t g_ota_cmd_queue = NULL;

typedef enum {
    OTA_TASK_CMD_ERASE,
    OTA_TASK_CMD_STOP,
    OTA_TASK_CMD_WRITE_DATA,
    OTA_TASK_CMD_VERIFY,
} ota_task_cmd_type_t;

typedef struct {
    ota_task_cmd_type_t cmd;
    uint32_t addr;
    uint32_t size;
    uint16_t seq_number;
    uint16_t payload_size;
    uint8_t  *payload;
} ota_task_cmd_t;

/*
 * FORWARD DECLARATIONS
 ****************************************************************************************
 */

static int ota_switch_partition(void);
static void ota_update_packet_size_from_mtu(uint16_t mtu);
static uint32_t crc32_calculate(uint32_t crc, const uint8_t *data, uint32_t length);

static void ota_set_status(ota_status_t status, ota_error_t error, uint8_t progress);
static void ota_reset_context(void);

static void ota_flash_buffer_init(void);
static void ota_flash_buffer_deinit(void);
static int ota_flash_buffer_add_packet(uint16_t seq_number, const uint8_t *data, uint16_t size);
static int ota_flash_buffer_flush_if_complete(void);
static int ota_flash_buffer_flush_final(void);

static int ota_handle_start_cmd(const uint8_t *data, uint16_t length);
static int ota_handle_data(const uint8_t *data, uint16_t length);
static int ota_handle_end_cmd(const uint8_t *data, uint16_t length);
static int ota_handle_verify_cmd(void);
static int ota_handle_reboot_cmd(void);

/*
 * ZEPHYR GATT CALLBACKS
 ****************************************************************************************
 */

static ssize_t ota_ctrl_write(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr,
                              const void *buf, uint16_t len,
                              uint16_t offset, uint8_t flags)
{
    const uint8_t *data = buf;

    if (offset != 0) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    if (len == 0) {
        return len;
    }

    // Store connection
    g_ota_conn = conn;

    // Process OTA command
    ota_cmd_t cmd = (ota_cmd_t)data[0];

    switch (cmd) {
        case OTA_CMD_START:
            ota_handle_start_cmd(data, len);
            break;

        case OTA_CMD_END:
            ota_handle_end_cmd(data, len);
            break;

        case OTA_CMD_ABORT:
            IPRO_LOGI(LOG_TAG, "[OTA] Aborted");
            ota_reset_context();
            break;

        case OTA_CMD_VERIFY:
            ota_handle_verify_cmd();
            break;

        case OTA_CMD_REBOOT:
            ota_handle_reboot_cmd();
            break;

        default:
            IPRO_LOGW(LOG_TAG, "Unknown OTA command: 0x%02X", cmd);
            break;
    }

    return len;
}

static ssize_t ota_data_write(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr,
                              const void *buf, uint16_t len,
                              uint16_t offset, uint8_t flags)
{
    (void)attr;
    (void)offset;
    (void)flags;

    // Store connection
    g_ota_conn = conn;

    // Handle firmware data
    ota_handle_data(buf, len);

    return len;
}

static ssize_t ota_status_read(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len, uint16_t offset)
{
    static ota_status_notif_t status_data;

    status_data.status = g_ota_ctx.status;
    status_data.error = g_ota_ctx.error;
    status_data.progress = g_ota_ctx.progress;
    status_data.reserved = 0;

    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             &status_data, sizeof(status_data));
}

static void ota_status_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_ota_status_ntf_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "OTA status notifications %s",
              g_ota_status_ntf_enabled ? "enabled" : "disabled");
}

/*
 * ZEPHYR GATT SERVICE DEFINITION
 ****************************************************************************************
 */

BT_GATT_SERVICE_DEFINE(ota_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_OTA_SVC),
    // Control characteristic - Write
    BT_GATT_CHARACTERISTIC(BT_UUID_OTA_CTRL,
                           BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_WRITE,
                           NULL, ota_ctrl_write, NULL),
    // Data characteristic - Write + Write Without Response
    BT_GATT_CHARACTERISTIC(BT_UUID_OTA_DATA,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, ota_data_write, NULL),
    // Status characteristic - Read/Notify
    BT_GATT_CHARACTERISTIC(BT_UUID_OTA_STATUS,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           ota_status_read, NULL, NULL),
    BT_GATT_CCC(ota_status_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/*
 * FLASH WRITE BUFFER MANAGEMENT
 ****************************************************************************************
 */

static void ota_flash_buffer_init(void)
{
    if (g_flash_buffer.data != NULL) {
        vPortFree(g_flash_buffer.data);
    }

    g_flash_buffer.data = (uint8_t *)pvPortMalloc(OTA_FLASH_BUFFER_SIZE);
    if (g_flash_buffer.data == NULL) {
        IPRO_LOGE(LOG_TAG, "[OTA] Failed to allocate 4KB buffer!");
        return;
    }

    memset(g_flash_buffer.data, 0xFF, OTA_FLASH_BUFFER_SIZE);
    g_flash_buffer.sector_base_addr = 0;
    g_flash_buffer.packets_in_sector = OTA_FLASH_BUFFER_SIZE / g_max_payload_size;
    g_flash_buffer.received_mask = 0;
    g_flash_buffer.received_count = 0;
    g_flash_buffer.active = false;
    g_flash_buffer.pending_size = 0;
    g_flash_buffer.start_time_ms = 0;
}

static void ota_flash_buffer_deinit(void)
{
    if (g_flash_buffer.data != NULL) {
        vPortFree(g_flash_buffer.data);
        g_flash_buffer.data = NULL;
    }
    g_flash_buffer.active = false;
}

static int ota_flash_buffer_add_packet(uint16_t seq_number, const uint8_t *data, uint16_t size)
{
    if (g_flash_buffer.data == NULL) {
        IPRO_LOGE(LOG_TAG, "[OTA] Buffer not initialized!");
        return -1;
    }

    uint32_t packet_flash_addr = g_ota_ctx.backup_partition_addr + (seq_number * g_max_payload_size);
    uint32_t sector_addr = (packet_flash_addr / OTA_FLASH_BUFFER_SIZE) * OTA_FLASH_BUFFER_SIZE;
    uint16_t packet_in_sector = (packet_flash_addr - sector_addr) / g_max_payload_size;
    uint32_t offset_in_sector = packet_flash_addr - sector_addr;

    if (g_flash_buffer.active && sector_addr != g_flash_buffer.sector_base_addr) {
        int flush_ret = ota_flash_buffer_flush_if_complete();
        if (flush_ret == -1) {
            return -1;
        }

        if (flush_ret == 1 && g_flash_buffer.received_count > 0) {
            int ret = flash_write(g_flash_buffer.sector_base_addr, g_flash_buffer.data, OTA_FLASH_BUFFER_SIZE);
            if (ret != 0) {
                IPRO_LOGE(LOG_TAG, "[OTA] Sector write failed: %d", ret);
                return -1;
            }
        }

        memset(g_flash_buffer.data, 0xFF, OTA_FLASH_BUFFER_SIZE);
        g_flash_buffer.sector_base_addr = sector_addr;
        g_flash_buffer.received_mask = 0;
        g_flash_buffer.received_count = 0;
    }

    if (!g_flash_buffer.active) {
        g_flash_buffer.sector_base_addr = sector_addr;
        g_flash_buffer.active = true;
    }

    if (packet_in_sector < 32 && (g_flash_buffer.received_mask & (1U << packet_in_sector))) {
        return 0;
    }

    uint32_t write_offset = offset_in_sector;
    if (g_flash_buffer.pending_size > 0) {
        memcpy(g_flash_buffer.data, g_flash_buffer.pending_data, g_flash_buffer.pending_size);
        if (offset_in_sector < g_flash_buffer.pending_size) {
            write_offset = g_flash_buffer.pending_size;
        }
        g_flash_buffer.pending_size = 0;
    }

    if (write_offset + size > OTA_FLASH_BUFFER_SIZE) {
        uint32_t bytes_in_current = OTA_FLASH_BUFFER_SIZE - write_offset;
        uint32_t bytes_overflow = size - bytes_in_current;
        memcpy(g_flash_buffer.data + write_offset, data, bytes_in_current);
        memcpy(g_flash_buffer.pending_data, data + bytes_in_current, bytes_overflow);
        g_flash_buffer.pending_size = bytes_overflow;
    } else {
        memcpy(g_flash_buffer.data + write_offset, data, size);
    }

    if (packet_in_sector < 32) {
        g_flash_buffer.received_mask |= (1U << packet_in_sector);
    }
    g_flash_buffer.received_count++;

    return 0;
}

static int ota_flash_buffer_flush_if_complete(void)
{
    if (!g_flash_buffer.active || g_flash_buffer.data == NULL) {
        return 1;
    }

    uint32_t sector_start = g_flash_buffer.sector_base_addr - g_ota_ctx.backup_partition_addr;
    uint32_t sector_end = sector_start + OTA_FLASH_BUFFER_SIZE;
    uint32_t firmware_end = g_ota_ctx.firmware_size;

    uint16_t expected_packets;
    if (sector_end > firmware_end) {
        uint32_t bytes_in_sector = firmware_end - sector_start;
        expected_packets = (bytes_in_sector + g_max_payload_size - 1) / g_max_payload_size;
    } else {
        expected_packets = g_flash_buffer.packets_in_sector;
    }

    if (g_flash_buffer.received_count < expected_packets) {
        return 1;
    }

    int ret = flash_write(g_flash_buffer.sector_base_addr, g_flash_buffer.data, OTA_FLASH_BUFFER_SIZE);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "[OTA] Sector write failed: %d", ret);
        return -1;
    }

    g_flash_buffer.sector_base_addr += OTA_FLASH_BUFFER_SIZE;
    memset(g_flash_buffer.data, 0xFF, OTA_FLASH_BUFFER_SIZE);
    g_flash_buffer.received_mask = 0;
    g_flash_buffer.received_count = 0;

    return 0;
}

static int ota_flash_buffer_flush_final(void)
{
    if (!g_flash_buffer.active || g_flash_buffer.data == NULL) {
        return 0;
    }

    if (g_flash_buffer.received_count == 0) {
        g_flash_buffer.active = false;
        return 0;
    }

    int ret = flash_write(g_flash_buffer.sector_base_addr, g_flash_buffer.data, OTA_FLASH_BUFFER_SIZE);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "[OTA] Final sector write failed: %d", ret);
        return -1;
    }

    g_flash_buffer.active = false;
    return 0;
}

/*
 * CRC32 CALCULATION
 ****************************************************************************************
 */

static uint32_t crc32_table[256];
static bool crc32_table_initialized = false;

static void crc32_init_table(void)
{
    if (crc32_table_initialized) {
        return;
    }

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
    crc32_table_initialized = true;
}

static uint32_t crc32_calculate(uint32_t crc, const uint8_t *data, uint32_t length)
{
    crc = ~crc;
    for (uint32_t i = 0; i < length; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }
    return ~crc;
}

/*
 * OTA BACKGROUND TASK
 ****************************************************************************************
 */

static void ota_task(void *param)
{
    (void)param;
    ota_task_cmd_t cmd;

    while (1) {
        if (xQueueReceive(g_ota_cmd_queue, &cmd, portMAX_DELAY) == pdTRUE) {
            if (cmd.cmd == OTA_TASK_CMD_ERASE) {
                IPRO_LOGI(LOG_TAG, "[OTA] Erasing %lu KB...", cmd.size / 1024);

                #define ERASE_CHUNK_SIZE (4 * 1024)
                uint32_t remaining = cmd.size;
                uint32_t offset = 0;
                uint8_t last_reported_pct = 0;
                bool error = false;

                while (remaining > 0 && !error) {
                    uint32_t chunk = remaining > ERASE_CHUNK_SIZE ? ERASE_CHUNK_SIZE : remaining;
                    uint32_t addr = cmd.addr + offset;

                    int ret = flash_erase(addr, chunk);
                    if (ret != 0) {
                        IPRO_LOGE(LOG_TAG, "Flash erase failed at 0x%08lx: %d", addr, ret);
                        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_ERASE_FAILED, 0);
                        error = true;
                        break;
                    }

                    offset += chunk;
                    remaining -= chunk;

                    uint8_t pct = (uint8_t)((offset * 100) / cmd.size);
                    if (pct >= last_reported_pct + 25 || remaining == 0) {
                        IPRO_LOGI(LOG_TAG, "[OTA] Erase: %d%%", pct);
                        last_reported_pct = pct;
                    }

                    vTaskDelay(pdMS_TO_TICKS(5));
                }

                if (!error) {
                    crc32_init_table();
                    ota_flash_buffer_init();
                    g_ota_ctx.active = true;
                    g_ota_ctx.bytes_received = 0;
                    g_ota_ctx.flash_offset = 0;
                    g_ota_ctx.calculated_crc32 = 0;

                    ota_set_status(OTA_STATUS_RECEIVING, OTA_ERR_NONE, 0);
                }
            }
            else if (cmd.cmd == OTA_TASK_CMD_STOP) {
                IPRO_LOGI(LOG_TAG, "OTA task stopping");
                break;
            }
            else if (cmd.cmd == OTA_TASK_CMD_WRITE_DATA) {
                int ret = ota_flash_buffer_add_packet(cmd.seq_number, cmd.payload, cmd.payload_size);

                if (cmd.payload != NULL) {
                    vPortFree(cmd.payload);
                }

                if (ret != 0) {
                    IPRO_LOGE(LOG_TAG, "Failed to buffer packet seq=%u: %d", cmd.seq_number, ret);
                } else {
                    ota_flash_buffer_flush_if_complete();
                }
            }
            else if (cmd.cmd == OTA_TASK_CMD_VERIFY) {
                int flush_ret = ota_flash_buffer_flush_final();
                if (flush_ret != 0) {
                    IPRO_LOGE(LOG_TAG, "[OTA] Failed to flush final sector: %d", flush_ret);
                    ota_set_status(OTA_STATUS_ERROR, OTA_ERR_WRITE_FAILED, 100);
                    ota_reset_context();
                    continue;
                }

                if (g_ota_ctx.expected_crc32 == 0) {
                    IPRO_LOGW(LOG_TAG, "[OTA] No CRC32 to verify");
                    continue;
                }

                g_ota_ctx.calculated_crc32 = 0;
                uint32_t flash_addr = g_ota_ctx.backup_partition_addr;
                uint32_t remaining = g_ota_ctx.firmware_size;
                uint8_t buffer[256];
                bool read_error = false;

                while (remaining > 0) {
                    uint32_t chunk = (remaining > sizeof(buffer)) ? sizeof(buffer) : remaining;
                    if (flash_read(flash_addr, buffer, chunk) != 0) {
                        IPRO_LOGE(LOG_TAG, "[OTA] Flash read failed");
                        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_VERIFY_FAILED, 100);
                        ota_reset_context();
                        read_error = true;
                        break;
                    }
                    g_ota_ctx.calculated_crc32 = crc32_calculate(g_ota_ctx.calculated_crc32, buffer, chunk);
                    flash_addr += chunk;
                    remaining -= chunk;
                }

                if (read_error) {
                    continue;
                }

                if (g_ota_ctx.calculated_crc32 != g_ota_ctx.expected_crc32) {
                    IPRO_LOGE(LOG_TAG, "[OTA] CRC mismatch: 0x%08lX != 0x%08lX",
                              g_ota_ctx.calculated_crc32, g_ota_ctx.expected_crc32);
                    ota_set_status(OTA_STATUS_ERROR, OTA_ERR_VERIFY_FAILED, 100);
                    ota_reset_context();
                } else {
                    IPRO_LOGI(LOG_TAG, "[OTA] CRC32 OK! Switching partition...");
                    ota_set_status(OTA_STATUS_READY, OTA_ERR_NONE, 100);

                    if (ota_switch_partition() != 0) {
                        IPRO_LOGE(LOG_TAG, "[OTA] Partition switch failed");
                        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_UPDATE_FAILED, 100);
                        ota_reset_context();
                    } else {
                        IPRO_LOGI(LOG_TAG, "[OTA] Ready to reboot");
                        ota_set_status(OTA_STATUS_COMPLETE, OTA_ERR_NONE, 100);
                    }
                }
            }
        }
    }

    g_ota_task_handle = NULL;
    vTaskDelete(NULL);
}

/*
 * PARTITION MANAGEMENT
 ****************************************************************************************
 */

static int ota_get_backup_partition(uint32_t *addr, uint32_t *size)
{
    int ret = hal_boot2_partition_addr_inactive("FW", addr, size);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to get backup partition: %d", ret);
        return -1;
    }
    return 0;
}

static int ota_switch_partition(void)
{
    int ret;
    PtTable_Entry_Config pt_entry;

    ret = hal_boot2_get_active_entries(PT_ENTRY_FW_CPU0, &pt_entry);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to get partition entry: %d", ret);
        return -1;
    }

    if (g_ota_ctx.firmware_size == 0 || g_ota_ctx.firmware_size > pt_entry.maxLen[!pt_entry.activeIndex]) {
        IPRO_LOGE(LOG_TAG, "Invalid firmware size: %lu (max: %lu)",
                  g_ota_ctx.firmware_size, pt_entry.maxLen[!pt_entry.activeIndex]);
        return -1;
    }

    pt_entry.len = g_ota_ctx.firmware_size;

    ret = hal_boot2_update_ptable(&pt_entry);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to update partition table: %d", ret);
        return -1;
    }

    return 0;
}

/*
 * OTA STATUS MANAGEMENT
 ****************************************************************************************
 */

static void ota_set_status(ota_status_t status, ota_error_t error, uint8_t progress)
{
    g_ota_ctx.status = status;
    g_ota_ctx.error = error;
    g_ota_ctx.progress = progress;

    if (g_ota_ctx.event_cb) {
        g_ota_ctx.event_cb(status, error, progress);
    }

    // Send notification via Zephyr API
    if (g_ota_status_ntf_enabled && g_ota_conn) {
        ota_status_notif_t status_data = {
            .status = (uint8_t)status,
            .error = (uint8_t)error,
            .progress = progress,
            .reserved = 0,
        };

        bt_gatt_notify(g_ota_conn, &ota_svc.attrs[5], &status_data, sizeof(status_data));
    }
}

static void ota_reset_context(void)
{
    ota_flash_buffer_deinit();

    g_ota_ctx.active = false;
    g_ota_ctx.firmware_size = 0;
    g_ota_ctx.bytes_received = 0;
    g_ota_ctx.flash_offset = 0;
    g_ota_ctx.calculated_crc32 = 0;
    g_ota_ctx.expected_crc32 = 0;
    g_ota_ctx.backup_partition_addr = 0;
    g_ota_ctx.backup_partition_size = 0;
    g_ota_ctx.status = OTA_STATUS_IDLE;
    g_ota_ctx.error = OTA_ERR_NONE;
    g_ota_ctx.progress = 0;
}

/*
 * OTA COMMAND HANDLERS
 ****************************************************************************************
 */

static int ota_handle_start_cmd(const uint8_t *data, uint16_t length)
{
    if (length < 5) {
        IPRO_LOGE(LOG_TAG, "Invalid START command length: %d", length);
        return -1;
    }

    uint32_t fw_size = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);

    IPRO_LOGI(LOG_TAG, "[OTA] START: %lu bytes, ACK-per-packet mode", fw_size);

    // Get MTU from connection
    uint16_t current_mtu = 23;
    if (g_ota_conn) {
        current_mtu = bt_gatt_get_mtu(g_ota_conn);
    }

    IPRO_LOGI(LOG_TAG, "[OTA] Current MTU: %d bytes", current_mtu);
    ota_update_packet_size_from_mtu(current_mtu);

    uint16_t total_packets = (fw_size + g_max_payload_size - 1) / g_max_payload_size;
    IPRO_LOGI(LOG_TAG, "[OTA] START: %lu bytes, %u packets (%u bytes/pkt, MTU=%u)",
              fw_size, total_packets, g_max_payload_size, current_mtu);

    if (fw_size == 0 || fw_size > 1024 * 1024) {
        IPRO_LOGE(LOG_TAG, "Invalid firmware size: %lu", fw_size);
        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_INVALID_SIZE, 0);
        return -1;
    }

    uint32_t backup_addr, backup_size;
    if (ota_get_backup_partition(&backup_addr, &backup_size) != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to get backup partition");
        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_PARTITION, 0);
        return -1;
    }

    if (fw_size > backup_size) {
        IPRO_LOGE(LOG_TAG, "Firmware too large: %lu > %lu", fw_size, backup_size);
        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_NO_SPACE, 0);
        return -1;
    }

    g_ota_ctx.firmware_size = fw_size;
    g_ota_ctx.backup_partition_addr = backup_addr;
    g_ota_ctx.backup_partition_size = backup_size;

    ota_task_cmd_t erase_cmd = {
        .cmd = OTA_TASK_CMD_ERASE,
        .addr = backup_addr,
        .size = backup_size
    };

    if (xQueueSend(g_ota_cmd_queue, &erase_cmd, 0) != pdTRUE) {
        IPRO_LOGE(LOG_TAG, "Failed to queue erase command");
        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_ERASE_FAILED, 0);
        return -1;
    }

    return 0;
}

static int ota_handle_data(const uint8_t *data, uint16_t length)
{
    if (!g_ota_ctx.active) {
        IPRO_LOGE(LOG_TAG, "No active OTA session");
        return -1;
    }

    if (g_ota_ctx.status != OTA_STATUS_RECEIVING) {
        IPRO_LOGE(LOG_TAG, "Invalid state for data: %d", g_ota_ctx.status);
        return -1;
    }

    if (length > sizeof(ota_packet_t)) {
        ota_packet_t *pkt = (ota_packet_t *)data;

        if (pkt->seq_number == 0 && g_ota_ctx.bytes_received == 0) {
            if (pkt->payload_size != g_max_payload_size) {
                IPRO_LOGI(LOG_TAG, "[OTA] Auto-adjusting payload: %u -> %u bytes",
                          g_max_payload_size, pkt->payload_size);
                g_max_payload_size = pkt->payload_size;
                ota_flash_buffer_deinit();
                ota_flash_buffer_init();
            }
            g_flash_buffer.start_time_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
        }

        uint16_t total_packets = (g_ota_ctx.firmware_size + g_max_payload_size - 1) / g_max_payload_size;

        if (pkt->seq_number >= total_packets) {
            IPRO_LOGE(LOG_TAG, "Invalid sequence number: %u (max: %u)",
                      pkt->seq_number, total_packets - 1);
            return -1;
        }

        uint16_t expected_size = g_max_payload_size;
        if (pkt->seq_number == total_packets - 1) {
            uint32_t total_data = g_ota_ctx.firmware_size;
            uint32_t full_packets = (total_packets - 1) * g_max_payload_size;
            expected_size = total_data - full_packets;
        }

        if (pkt->payload_size > expected_size) {
            IPRO_LOGE(LOG_TAG, "Invalid payload size: %u (expected: %u)",
                      pkt->payload_size, expected_size);
            return -1;
        }

        uint8_t *payload_buf = (uint8_t *)pvPortMalloc(pkt->payload_size);
        if (payload_buf == NULL) {
            IPRO_LOGE(LOG_TAG, "Failed to allocate %u bytes", pkt->payload_size);
            return -1;
        }
        memcpy(payload_buf, pkt->payload, pkt->payload_size);

        ota_task_cmd_t write_cmd = {
            .cmd = OTA_TASK_CMD_WRITE_DATA,
            .addr = g_ota_ctx.backup_partition_addr + (pkt->seq_number * g_max_payload_size),
            .size = 0,
            .seq_number = pkt->seq_number,
            .payload_size = pkt->payload_size,
            .payload = payload_buf,
        };

        if (xQueueSend(g_ota_cmd_queue, &write_cmd, pdMS_TO_TICKS(100)) != pdTRUE) {
            IPRO_LOGE(LOG_TAG, "Flash write queue full! Packet %u dropped", pkt->seq_number);
            vPortFree(payload_buf);
            return -1;
        }

        g_ota_ctx.bytes_received += pkt->payload_size;
        uint8_t progress = (uint8_t)((pkt->seq_number * 100) / total_packets);

        if (progress >= g_ota_ctx.progress + 5) {
            ota_set_status(OTA_STATUS_RECEIVING, OTA_ERR_NONE, progress);
            IPRO_LOGI(LOG_TAG, "[OTA] Progress: %d%% (%u/%u)",
                      progress, pkt->seq_number + 1, total_packets);
        }

        bool is_last_packet = (pkt->seq_number + 1) >= total_packets;

        if (is_last_packet) {
            uint32_t elapsed_ms = (xTaskGetTickCount() * portTICK_PERIOD_MS) - g_flash_buffer.start_time_ms;
            uint32_t speed_bps = (elapsed_ms > 0) ? (g_ota_ctx.bytes_received * 1000 / elapsed_ms) : 0;
            IPRO_LOGI(LOG_TAG, "[OTA] Transfer complete: %lu bytes in %lu ms (%lu bytes/s)",
                      g_ota_ctx.bytes_received, elapsed_ms, speed_bps);
            ota_set_status(OTA_STATUS_RECEIVING, OTA_ERR_NONE, 100);

            if (g_ota_ctx.expected_crc32 != 0) {
                IPRO_LOGI(LOG_TAG, "[OTA] Queuing verification...");

                ota_task_cmd_t verify_cmd = {0};
                verify_cmd.cmd = OTA_TASK_CMD_VERIFY;

                if (xQueueSend(g_ota_cmd_queue, &verify_cmd, pdMS_TO_TICKS(100)) != pdPASS) {
                    IPRO_LOGE(LOG_TAG, "Failed to queue VERIFY command!");
                    ota_set_status(OTA_STATUS_ERROR, OTA_ERR_VERIFY_FAILED, 100);
                    ota_reset_context();
                }
            }
        }

        return 0;
    }

    return 0;
}

static int ota_handle_end_cmd(const uint8_t *data, uint16_t length)
{
    if (!g_ota_ctx.active) {
        IPRO_LOGE(LOG_TAG, "No active OTA session");
        return -1;
    }

    if (length < sizeof(ota_end_cmd_t)) {
        IPRO_LOGE(LOG_TAG, "Invalid END command length: %d", length);
        return -1;
    }

    ota_end_cmd_t *cmd = (ota_end_cmd_t *)data;
    g_ota_ctx.expected_crc32 = cmd->crc32;

    IPRO_LOGI(LOG_TAG, "[OTA] END: CRC32=0x%08lx, %lu/%lu bytes",
              g_ota_ctx.expected_crc32, g_ota_ctx.bytes_received, g_ota_ctx.firmware_size);

    if (g_ota_ctx.bytes_received != g_ota_ctx.firmware_size) {
        IPRO_LOGE(LOG_TAG, "Incomplete transfer: %lu/%lu bytes",
                  g_ota_ctx.bytes_received, g_ota_ctx.firmware_size);
        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_INVALID_SIZE, g_ota_ctx.progress);
        ota_reset_context();
        return -1;
    }

    ota_set_status(OTA_STATUS_VERIFYING, OTA_ERR_NONE, 100);

    ota_task_cmd_t verify_cmd = {0};
    verify_cmd.cmd = OTA_TASK_CMD_VERIFY;

    if (xQueueSend(g_ota_cmd_queue, &verify_cmd, pdMS_TO_TICKS(100)) != pdPASS) {
        IPRO_LOGE(LOG_TAG, "Failed to queue VERIFY command!");
        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_VERIFY_FAILED, 100);
        ota_reset_context();
        return -1;
    }

    return 0;
}

static int ota_handle_verify_cmd(void)
{
    if (!g_ota_ctx.active) {
        IPRO_LOGE(LOG_TAG, "No active OTA session");
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "Re-verifying firmware from flash...");

    uint32_t crc = 0;
    uint32_t remaining = g_ota_ctx.firmware_size;
    uint32_t offset = 0;
    uint8_t buffer[256];

    crc32_init_table();

    while (remaining > 0) {
        uint32_t chunk = remaining > sizeof(buffer) ? sizeof(buffer) : remaining;
        uint32_t addr = g_ota_ctx.backup_partition_addr + offset;

        int ret = flash_read(addr, buffer, chunk);
        if (ret != 0) {
            IPRO_LOGE(LOG_TAG, "Flash read failed at 0x%08lx", addr);
            ota_set_status(OTA_STATUS_ERROR, OTA_ERR_CRC_FAILED, 100);
            return -1;
        }

        crc = crc32_calculate(crc, buffer, chunk);
        offset += chunk;
        remaining -= chunk;
    }

    IPRO_LOGI(LOG_TAG, "Flash CRC32: calculated=0x%08lx, expected=0x%08lx", crc, g_ota_ctx.expected_crc32);

    if (crc != g_ota_ctx.expected_crc32) {
        IPRO_LOGE(LOG_TAG, "Flash verification failed!");
        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_CRC_FAILED, 100);
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "Flash verification successful!");
    return 0;
}

static int ota_handle_reboot_cmd(void)
{
    if (!g_ota_ctx.active) {
        IPRO_LOGE(LOG_TAG, "No active OTA session");
        return -1;
    }

    if (g_ota_ctx.status != OTA_STATUS_READY && g_ota_ctx.status != OTA_STATUS_COMPLETE) {
        IPRO_LOGE(LOG_TAG, "Not ready to reboot, status=%d", g_ota_ctx.status);
        ota_set_status(OTA_STATUS_ERROR, OTA_ERR_INVALID_STATE, 100);
        return -1;
    }

    if (g_ota_ctx.status == OTA_STATUS_READY) {
        if (ota_switch_partition() != 0) {
            ota_set_status(OTA_STATUS_ERROR, OTA_ERR_PARTITION, 100);
            return -1;
        }
    }

    ota_reset_context();

    vTaskDelay(pdMS_TO_TICKS(100));

    IPRO_LOGI(LOG_TAG, "[OTA] Rebooting...");
    GLB_SW_POR_Reset();

    return 0;
}

/*
 * MTU UPDATE
 ****************************************************************************************
 */

static void ota_update_packet_size_from_mtu(uint16_t mtu)
{
    g_current_mtu = mtu;

    uint16_t available = (mtu > (ATT_HEADER_SIZE + OTA_PACKET_HEADER_SIZE))
                         ? (mtu - ATT_HEADER_SIZE - OTA_PACKET_HEADER_SIZE)
                         : OTA_MIN_PAYLOAD_SIZE;

    if (available > OTA_PACKET_PAYLOAD_SIZE) {
        available = OTA_PACKET_PAYLOAD_SIZE;
    } else if (available < OTA_MIN_PAYLOAD_SIZE) {
        available = OTA_MIN_PAYLOAD_SIZE;
    }

    g_max_payload_size = available;

    IPRO_LOGI(LOG_TAG, "[OTA] MTU updated: %d bytes -> max payload: %d bytes",
              mtu, g_max_payload_size);
}

/*
 * PUBLIC API IMPLEMENTATION
 ****************************************************************************************
 */

int ota_service_init(ota_event_cb_t event_cb)
{
    IPRO_LOGI(LOG_TAG, "Initializing OTA service...");

    memset(&g_ota_ctx, 0, sizeof(g_ota_ctx));
    g_ota_ctx.conidx = 0xFF;
    g_ota_ctx.event_cb = event_cb;
    g_ota_ctx.status = OTA_STATUS_IDLE;

    crc32_init_table();

    if (g_ota_task_handle == NULL) {
        g_ota_cmd_queue = xQueueCreate(20, sizeof(ota_task_cmd_t));
        if (g_ota_cmd_queue == NULL) {
            IPRO_LOGE(LOG_TAG, "Failed to create OTA queue");
            return -1;
        }

        BaseType_t ret = xTaskCreate(ota_task, "OTA_Task", OTA_TASK_STACK_SIZE,
                                      NULL, OTA_TASK_PRIORITY, &g_ota_task_handle);
        if (ret != pdPASS) {
            IPRO_LOGE(LOG_TAG, "Failed to create OTA task");
            vQueueDelete(g_ota_cmd_queue);
            g_ota_cmd_queue = NULL;
            return -1;
        }

        IPRO_LOGI(LOG_TAG, "OTA task and queue created");
    }

    IPRO_LOGI(LOG_TAG, "OTA service initialized");
    return 0;
}

int ota_service_register_zephyr(void)
{
    // Service is auto-registered via BT_GATT_SERVICE_DEFINE
    IPRO_LOGI(LOG_TAG, "OTA service registered (UUID=0x%04X)", OTA_SVC_UUID16);
    return 0;
}

int ota_service_get_status(ota_status_t *status, ota_error_t *error, uint8_t *progress)
{
    if (status) {
        *status = g_ota_ctx.status;
    }
    if (error) {
        *error = g_ota_ctx.error;
    }
    if (progress) {
        *progress = g_ota_ctx.progress;
    }
    return 0;
}

int ota_service_abort(void)
{
    IPRO_LOGI(LOG_TAG, "Aborting OTA session");
    ota_reset_context();
    return 0;
}

int ota_service_notify_status(uint8_t conidx)
{
    (void)conidx;

    if (!g_ota_status_ntf_enabled || !g_ota_conn) {
        return 0;
    }

    ota_status_notif_t notif = {
        .status = g_ota_ctx.status,
        .error = g_ota_ctx.error,
        .progress = g_ota_ctx.progress,
        .reserved = 0,
    };

    return bt_gatt_notify(g_ota_conn, &ota_svc.attrs[5], &notif, sizeof(notif));
}

void ota_service_update_mtu(uint16_t mtu)
{
    ota_update_packet_size_from_mtu(mtu);
}

void ota_service_set_conn(struct bt_conn *conn)
{
    g_ota_conn = conn;
    if (!conn && g_ota_ctx.active) {
        IPRO_LOGW(LOG_TAG, "Connection lost during OTA, aborting...");
        ota_service_abort();
    }
}

const ota_context_t *ota_get_context(void)
{
    return &g_ota_ctx;
}
