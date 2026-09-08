/**
 * @file remote_app.c
 * @brief BLE Remote Control Application Implementation
 *
 * Main application logic for BLE remote control demo
 * Uses Zephyr blestack directly (without ipro_ble_lib)
 *
 * Copyright (C) IPRO SDK 2025
 */

#include "remote_app.h"
#include "remote_services.h"
#include "pir_image_service.h"
#include "image_upload_service.h"
#if defined(CONFIG_IPRO_BLE_REMOTE_OTA)
#include "ota_service.h"
#endif
#include "ipro_log.h"
#include "hal_gpio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <ble_lib_api.h>
#include <hci_driver_ipro_ble_ctlr.h>
#include <ipro_ble_controller.h>

#define LOG_TAG "REMOTE_APP"

/*
 * GPIO PIN DEFINITIONS
 ****************************************************************************************
 */

#define LED_GPIO_PIN            28
#define BUTTON_GPIO_PIN         32
#define BUTTON_POLL_INTERVAL_MS 50
#define BUTTON_DEBOUNCE_MS      50
#define BUTTON_LONG_PRESS_MS    1000

static TaskHandle_t g_button_task_handle = NULL;

/*
 * ADVERTISING DATA
 ****************************************************************************************
 */

#define DEVICE_NAME "IPRO-Remote"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/* Advertising data using Zephyr bt_data format */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

/* Scan response data */
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_TX_POWER, 0x00),
    BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0x80, 0x01),  /* Generic Remote Control */
};

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

static remote_app_context_t g_app_ctx = {0};
static TimerHandle_t g_env_timer = NULL;
static bool g_pir_service_enabled = false;
static struct bt_conn *g_conn = NULL;

/*
 * PIR IMAGE EVENT CALLBACK
 ****************************************************************************************
 */

static void pir_img_event_callback(const pir_img_event_data_t *event, void *user_data)
{
    (void)user_data;

    switch (event->event) {
        case PIR_IMG_EVENT_MOTION_DETECTED:
            IPRO_LOGI(LOG_TAG, "[PIR] Motion #%lu", event->data.pir.event_count);
            /* Send UART notification about motion */
            if (g_app_ctx.connected) {
                char msg[64];
                snprintf(msg, sizeof(msg), "Motion detected! (count: %lu)\r\n",
                         event->data.pir.event_count);
                remote_uart_send_string(g_conn, msg);
            }
            break;

        case PIR_IMG_EVENT_MOTION_CLEARED:
            /* Motion cleared */
            break;

        case PIR_IMG_EVENT_CAPTURE_REQUEST:
            IPRO_LOGI(LOG_TAG, "[IMG] Capture requested");
            /* Trigger image capture (auto-starts transfer if connection ready) */
            image_capture();
            break;

        case PIR_IMG_EVENT_TRANSFER_START:
            IPRO_LOGI(LOG_TAG, "[IMG] Transfer started");
            if (g_app_ctx.connected) {
                remote_uart_send_string(g_conn, "Image transfer started...\r\n");
            }
            break;

        case PIR_IMG_EVENT_TRANSFER_COMPLETE:
            IPRO_LOGI(LOG_TAG, "[IMG] Transfer complete");
            if (g_app_ctx.connected) {
                remote_uart_send_string(g_conn, "Image transfer complete!\r\n");
            }
            break;

        case PIR_IMG_EVENT_TRANSFER_ABORT:
            IPRO_LOGI(LOG_TAG, "[IMG] Transfer aborted");
            break;

        default:
            break;
    }
}

/*
 * TIMER CALLBACK
 ****************************************************************************************
 */

/* Flag to indicate env notify is pending (set by timer, processed by task) */
static volatile bool g_env_notify_pending = false;

static void env_timer_callback(TimerHandle_t xTimer)
{
    (void)xTimer;

    if (!g_app_ctx.connected || !g_app_ctx.env_notify_running) {
        return;
    }

    /* Just set flag - actual notification will be sent from BLE task context */
    g_env_notify_pending = true;
}

/* This function should be called from BLE task context (e.g., in event loop) */
void remote_app_process_env_notify(void)
{
    if (!g_env_notify_pending) {
        return;
    }
    g_env_notify_pending = false;

    if (!g_app_ctx.connected || !g_app_ctx.env_notify_running) {
        return;
    }

    /* Simulate sensor data variation */
    g_app_ctx.sim_temperature += (rand() % 21) - 10;  /* -10 to +10 */
    if (g_app_ctx.sim_temperature < 1500) g_app_ctx.sim_temperature = 1500;  /* Min 15C */
    if (g_app_ctx.sim_temperature > 3500) g_app_ctx.sim_temperature = 3500;  /* Max 35C */

    g_app_ctx.sim_humidity += (rand() % 5) - 2;  /* -2 to +2 */
    if (g_app_ctx.sim_humidity < 20) g_app_ctx.sim_humidity = 20;
    if (g_app_ctx.sim_humidity > 90) g_app_ctx.sim_humidity = 90;

    /* Send notification */
    env_data_t env = {
        .temperature = g_app_ctx.sim_temperature,
        .humidity = g_app_ctx.sim_humidity
    };
    remote_notify_environment(g_conn, &env);

    IPRO_LOGD(LOG_TAG, "Env notify: temp=%d.%02dC humid=%d%%",
              env.temperature / 100, env.temperature % 100, env.humidity);
}

/*
 * BUTTON POLLING TASK
 ****************************************************************************************
 */

static void button_poll_task(void *pvParameters)
{
    (void)pvParameters;

    int last_state = 1;
    TickType_t press_start_tick = 0;
    bool long_press_notified = false;

    printf("[BTN] Task started GPIO%d\r\n", BUTTON_GPIO_PIN);

    for (;;) {
        int current_state = gpio_read(BUTTON_GPIO_PIN);

        /* Detect press (falling edge) */
        if (last_state == 1 && current_state == 0) {
            press_start_tick = xTaskGetTickCount();
            long_press_notified = false;
            IPRO_LOGD(LOG_TAG, "[BTN] Pressed\r\n");

            if (g_app_ctx.connected) {
                g_app_ctx.btn_press_count++;
                remote_notify_button(g_conn, BTN_EVENT_PRESSED);
            }
        }
        /* Detect release (rising edge) */
        else if (last_state == 0 && current_state == 1) {
            TickType_t press_duration = xTaskGetTickCount() - press_start_tick;
            IPRO_LOGD(LOG_TAG, "[BTN] Released\r\n");

            if (g_app_ctx.connected) {
                if (press_duration >= pdMS_TO_TICKS(BUTTON_LONG_PRESS_MS) && !long_press_notified) {
                    remote_notify_button(g_conn, BTN_EVENT_LONG_PRESS);
                }
                remote_notify_button(g_conn, BTN_EVENT_RELEASED);
            }
        }
        /* Detect long press while holding */
        else if (current_state == 0 && !long_press_notified) {
            TickType_t press_duration = xTaskGetTickCount() - press_start_tick;
            if (press_duration >= pdMS_TO_TICKS(BUTTON_LONG_PRESS_MS)) {
                IPRO_LOGD(LOG_TAG, "[BTN] Long press\r\n");
                if (g_app_ctx.connected) {
                    remote_notify_button(g_conn, BTN_EVENT_LONG_PRESS);
                }
                long_press_notified = true;
            }
        }

        last_state = current_state;
        vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_INTERVAL_MS));
    }
}

/*
 * GPIO INITIALIZATION
 ****************************************************************************************
 */

static void gpio_hw_init(void)
{
    /* LED: output push-pull with pull-up (active high) */
    gpio_set_mode(LED_GPIO_PIN, GPIO_OUTPUT_PP_MODE);
    gpio_write(LED_GPIO_PIN, 0);  /* LED off initially */
    IPRO_LOGI(LOG_TAG, "[GPIO] LED initialized (GPIO%d)", LED_GPIO_PIN);

    /* Button: input with pull-up (active low) */
    gpio_set_mode(BUTTON_GPIO_PIN, GPIO_INPUT_PD_MODE);
    IPRO_LOGI(LOG_TAG, "[GPIO] Button initialized (GPIO%d)", BUTTON_GPIO_PIN);
}

/*
 * SERVICE EVENT CALLBACK
 ****************************************************************************************
 */

static void service_event_callback(const remote_event_data_t *event, void *user_data)
{
    (void)user_data;

    switch (event->event) {
        case REMOTE_EVENT_LED_STATE_CHANGED:
            IPRO_LOGI(LOG_TAG, "[LED] %s (GPIO%d)", event->data.led.on ? "ON" : "OFF", LED_GPIO_PIN);
            gpio_write(LED_GPIO_PIN, event->data.led.on ? 1 : 0);
            break;

        case REMOTE_EVENT_LED_BRIGHT_CHANGED:
            IPRO_LOGI(LOG_TAG, "[LED] Brightness: %d%%", event->data.led.brightness);
            /* Note: PWM not implemented, using on/off based on brightness threshold */
            gpio_write(LED_GPIO_PIN, event->data.led.brightness > 0 ? 1 : 0);
            break;

        case REMOTE_EVENT_LED_MODE_CHANGED:
            IPRO_LOGI(LOG_TAG, "[LED] Mode: %d", event->data.led.mode);
            break;

        case REMOTE_EVENT_BTN_NOTIFY_ENABLED:
            /* Button notifications enabled */
            break;

        case REMOTE_EVENT_BTN_NOTIFY_DISABLED:
            /* Button notifications disabled */
            break;

        case REMOTE_EVENT_ENV_TEMP_NOTIFY_ENABLED:
            /* Temperature notifications enabled */
            g_app_ctx.env_notify_running = true;
            if (g_env_timer != NULL) {
                xTimerStart(g_env_timer, 0);
            }
            break;

        case REMOTE_EVENT_ENV_TEMP_NOTIFY_DISABLED:
            /* Temperature notifications disabled */
            g_app_ctx.env_notify_running = false;
            if (g_env_timer != NULL) {
                xTimerStop(g_env_timer, 0);
            }
            break;

        case REMOTE_EVENT_UART_TX_ENABLED:
            IPRO_LOGI(LOG_TAG, "[UART] RX enabled");
            /* Send welcome message */
            if (g_app_ctx.connected) {
                remote_uart_send_string(g_conn, "Welcome to IPRO Remote!\r\n");
            }
            break;

        case REMOTE_EVENT_UART_TX_DISABLED:
            /* UART RX disabled */
            break;

        case REMOTE_EVENT_UART_RX_DATA:
            IPRO_LOGI(LOG_TAG, "[UART] RX: %d bytes", event->data.uart.length);

            /* Echo back received data */
            char response[300];
            int len = event->data.uart.length < 200 ? event->data.uart.length : 200;
            snprintf(response, sizeof(response), "Echo: %.*s\r\n", len, (char*)event->data.uart.data);
            remote_uart_send_string(g_conn, response);

            /* Print to console */
            printf("[UART RX] %.*s\n", len, (char*)event->data.uart.data);
            break;

        default:
            break;
    }
}

/*
 * CONNECTION CALLBACKS (Zephyr blestack)
 ****************************************************************************************
 */

/* MTU exchange */
static struct bt_gatt_exchange_params g_mtu_exchange_params;

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err,
                            struct bt_gatt_exchange_params *params)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "MTU exchange failed (err %d)", err);
    } else {
        IPRO_LOGI(LOG_TAG, "MTU exchange complete: MTU=%u", bt_gatt_get_mtu(conn));
    }
}

static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "Connection failed (err 0x%02x)", err);
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "Connected! Peer: %s", addr_str);

    g_conn = bt_conn_ref(conn);
    g_app_ctx.state = REMOTE_STATE_CONNECTED;
    g_app_ctx.connected = true;
    g_app_ctx.conn_id = bt_conn_index(conn);

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
    /* Request DLE for larger PDUs (251 bytes) through the public Host API. */
    static const struct bt_conn_le_data_len_param data_len =
        BT_CONN_LE_DATA_LEN_PARAM_INIT(251, 2120);
    bt_conn_le_data_len_update(conn, &data_len);
#endif

    /* Fix 1: Exchange MTU for larger GATT payloads */
    g_mtu_exchange_params.func = mtu_exchange_cb;
    int mtu_ret = bt_gatt_exchange_mtu(conn, &g_mtu_exchange_params);
    if (mtu_ret) {
        IPRO_LOGW(LOG_TAG, "MTU exchange request failed: %d", mtu_ret);
    }

#if defined(CONFIG_BT_USER_PHY_UPDATE)
    /* Request 2M PHY for higher throughput. */
    bt_conn_le_phy_update(conn, BT_CONN_LE_PHY_PARAM_2M);
#endif

    /* Fix 5: Request shorter connection interval (7.5ms) */
    struct bt_le_conn_param ci_param = {
        .interval_min = 6,    /* 7.5ms */
        .interval_max = 6,    /* 7.5ms */
        .latency = 0,
        .timeout = 100,       /* 1s */
    };
    bt_conn_le_param_update(conn, &ci_param);

    /* Get connection info */
    struct bt_conn_info info;
    if (bt_conn_get_info(conn, &info) == 0) {
        uint16_t conn_interval_ms = (info.le.interval * 125) / 100;
        IPRO_LOGI(LOG_TAG, "Params: interval=%u ms, latency=%u, timeout=%u ms",
                 conn_interval_ms, info.le.latency, info.le.timeout * 10);

        /* Update transfer rate based on connection interval */
        if (g_pir_service_enabled) {
            pir_image_set_transfer_rate(conn_interval_ms);
        }
    }

    /* Set PIR image service connection for notifications */
    if (g_pir_service_enabled) {
        pir_image_service_set_conn(conn);
    }

#if defined(CONFIG_IPRO_BLE_REMOTE_OTA)
    /* Set OTA service connection */
    ota_service_set_conn(conn);
#endif

    /* Set Image Upload service connection */
    image_upload_set_conn(conn);
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "Disconnected: %s (reason 0x%02x)", addr_str, reason);

    /* Notify services before clearing state */
    if (g_pir_service_enabled) {
        pir_image_service_set_conn(NULL);
    }
#if defined(CONFIG_IPRO_BLE_REMOTE_OTA)
    ota_service_set_conn(NULL);
#endif
    image_upload_set_conn(NULL);

    if (g_conn) {
        bt_conn_unref(g_conn);
        g_conn = NULL;
    }

    /* Clear app state */
    g_app_ctx.state = REMOTE_STATE_READY;
    g_app_ctx.connected = false;
    g_app_ctx.conn_id = 0;
    g_app_ctx.env_notify_running = false;

    if (g_env_timer != NULL) {
        xTimerStop(g_env_timer, 0);
    }

    /* Auto-restart advertising */
    vTaskDelay(pdMS_TO_TICKS(500));
    remote_app_start_advertising();
}

static void le_param_updated_cb(struct bt_conn *conn, uint16_t interval,
                                uint16_t latency, uint16_t timeout)
{
    uint16_t interval_ms = (interval * 125) / 100;
    IPRO_LOGI(LOG_TAG, "Params updated: interval=%u ms, latency=%u, timeout=%u ms",
             interval_ms, latency, timeout * 10);

    /* Update transfer rate based on new connection interval */
    if (g_pir_service_enabled) {
        pir_image_set_transfer_rate(interval_ms);
    }
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
    .le_param_updated = le_param_updated_cb,
};

/*
 * SMP AUTHENTICATION CALLBACKS
 ****************************************************************************************
 */

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "[SMP] Passkey for %s: %06u", addr_str, passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "[SMP] Confirm passkey for %s: %06u", addr_str, passkey);
    /* Auto-confirm for "Just Works" pairing */
    bt_conn_auth_passkey_confirm(conn);
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing confirm request from %s", addr_str);
    /* Auto-accept pairing request */
    bt_conn_auth_pairing_confirm(conn);
}

static void auth_cancel(struct bt_conn *conn)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing cancelled: %s", addr_str);
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing complete: %s, bonded=%d", addr_str, bonded);
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGE(LOG_TAG, "[SMP] Pairing failed: %s, reason=%d", addr_str, reason);
}

static struct bt_conn_auth_cb auth_callbacks = {
    .passkey_display = auth_passkey_display,
    .passkey_confirm = auth_passkey_confirm,
    .pairing_confirm = auth_pairing_confirm,
    .cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb auth_info_callbacks = {
    .pairing_complete = auth_pairing_complete,
    .pairing_failed = auth_pairing_failed,
};

/*
 * BLE READY CALLBACK
 ****************************************************************************************
 */

static void bt_ready_cb(int err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "Bluetooth init failed (err %d)", err);
        g_app_ctx.state = REMOTE_STATE_ERROR;
        return;
    }

    IPRO_LOGI(LOG_TAG, "[BLE] Initialized");
    g_app_ctx.state = REMOTE_STATE_READY;

    /* Set device name */
    bt_set_name(DEVICE_NAME);

    /* Register connection callbacks */
    bt_conn_cb_register(&conn_callbacks);

    /* Register SMP authentication callbacks */
    int auth_err = bt_conn_auth_cb_register(&auth_callbacks);
    if (auth_err) {
        IPRO_LOGE(LOG_TAG, "Failed to register auth callbacks (err %d)", auth_err);
    } else {
        IPRO_LOGI(LOG_TAG, "[BLE] Auth callbacks registered");
    }
    auth_err = bt_conn_auth_info_cb_register(&auth_info_callbacks);
    if (auth_err) {
        IPRO_LOGE(LOG_TAG, "Failed to register auth info callbacks (err %d)", auth_err);
    } else {
        IPRO_LOGI(LOG_TAG, "[BLE] Auth info callbacks registered");
    }

    /* Initialize services */
    int status = remote_services_init(service_event_callback, NULL);
    if (status == 0) {
        IPRO_LOGI(LOG_TAG, "[BLE] Services ready");

        /* PIR Image service (Zephyr blestack) */
        status = pir_image_service_init(pir_img_event_callback, NULL);
        if (status == 0) {
            IPRO_LOGI(LOG_TAG, "[PIR] Service ready");
            g_pir_service_enabled = true;
        } else {
            IPRO_LOGW(LOG_TAG, "PIR Image service init failed: %d", status);
        }

        /* Image Upload service (Zephyr blestack) */
        status = image_upload_service_init(NULL, NULL);
        if (status == 0) {
            IPRO_LOGI(LOG_TAG, "[UPLOAD] Service ready");
        } else {
            IPRO_LOGW(LOG_TAG, "Image Upload service init failed: %d", status);
        }

        /* OTA service (Zephyr blestack).  Disabled by default in the new-stack
         * interoperability image because OTA needs a PSM/backup partition
         * layout; BLE scan/connect/GATT gates must not be blocked by storage.
         */
#if defined(CONFIG_IPRO_BLE_REMOTE_OTA)
        if (ota_service_init(NULL) == 0) {
            if (ota_service_register_zephyr() == 0) {
                IPRO_LOGI(LOG_TAG, "[OTA] Service ready");
            } else {
                IPRO_LOGW(LOG_TAG, "OTA service registration failed");
            }
        } else {
            IPRO_LOGW(LOG_TAG, "OTA service init failed");
        }
#else
        IPRO_LOGI(LOG_TAG, "[OTA] Service disabled in new-stack smoke image");
#endif

        IPRO_LOGI(LOG_TAG, "[BLE] All services initialized");

        /* Start advertising */
        int ret = remote_app_start_advertising();
        if (ret != 0) {
            /* First advertising attempt failed, retrying... */
            vTaskDelay(pdMS_TO_TICKS(500));
            remote_app_start_advertising();
        }
    } else {
        IPRO_LOGE(LOG_TAG, "Failed to init services: %d", status);
        g_app_ctx.state = REMOTE_STATE_ERROR;
    }
}

/*
 * API IMPLEMENTATION
 ****************************************************************************************
 */

int remote_app_init(void)
{
    int err;

    IPRO_LOGI(LOG_TAG, "========================================");
    IPRO_LOGI(LOG_TAG, "   BLE Remote Control Demo");
    IPRO_LOGI(LOG_TAG, "   (Using IPRO Zephyr 4.4 host + ipro_ble_controller)");
    IPRO_LOGI(LOG_TAG, "========================================");

    memset(&g_app_ctx, 0, sizeof(g_app_ctx));
    g_app_ctx.state = REMOTE_STATE_INITIALIZING;

    /* Initialize GPIO for LED and Button */
    gpio_hw_init();

    /* Initialize simulated sensor data */
    g_app_ctx.sim_temperature = 2500;  /* 25.00C */
    g_app_ctx.sim_humidity = 50;       /* 50% */

    /* Create environment notification timer (1 second interval) */
    g_env_timer = xTimerCreate("env_timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, env_timer_callback);
    if (g_env_timer == NULL) {
        IPRO_LOGE(LOG_TAG, "Failed to create env timer");
    }

    /* Create button polling task (1024 words = 8KB on RV64) */
    BaseType_t ret = xTaskCreate(
        button_poll_task,
        "btn_poll",
        1024,  /* Stack size (words) */
        NULL,
        tskIDLE_PRIORITY + 1,
        &g_button_task_handle
    );
    if (ret != pdPASS) {
        IPRO_LOGE(LOG_TAG, "Failed to create button polling task");
    }

    /* Initialize BLE controller (must be done first) */
    IPRO_LOGI(LOG_TAG, "Initializing BLE controller...");
    err = ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
    if (err) {
        IPRO_LOGE(LOG_TAG, "controller init failed: %d", err);
        g_app_ctx.state = REMOTE_STATE_ERROR;
        return -1;
    }
    
    /* Give controller task time to start and initialize */
    vTaskDelay(pdMS_TO_TICKS(50));

    /* Initialize HCI driver (must be done before bt_enable) */
    IPRO_LOGI(LOG_TAG, "Initializing HCI driver...");
    err = hci_driver_ipro_ble_ctlr_init();
    if (err) {
        IPRO_LOGE(LOG_TAG, "HCI driver init failed: %d", err);
        g_app_ctx.state = REMOTE_STATE_ERROR;
        return -1;
    }
    
    /* Give HCI driver time to register */
    vTaskDelay(pdMS_TO_TICKS(50));

    /* Initialize BLE stack */
    IPRO_LOGI(LOG_TAG, "Initializing BLE stack...");
    err = bt_enable(bt_ready_cb);
    if (err) {
        IPRO_LOGE(LOG_TAG, "BLE init failed: %d", err);
        g_app_ctx.state = REMOTE_STATE_ERROR;
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "BLE initializing...");
    return 0;
}

int remote_app_start_advertising(void)
{
    if (g_app_ctx.state == REMOTE_STATE_ADVERTISING) {
        IPRO_LOGW(LOG_TAG, "Already advertising");
        return 0;
    }

    if (g_app_ctx.state == REMOTE_STATE_CONNECTED) {
        IPRO_LOGW(LOG_TAG, "Already connected");
        return -1;
    }

    /* Use Zephyr advertising API */
    struct bt_le_adv_param adv_param = {
        .id = BT_ID_DEFAULT,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = 160,  /* 100ms (N * 0.625ms) */
        .interval_max = 320,  /* 200ms */
    };

    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        IPRO_LOGE(LOG_TAG, "Start advertising failed: %d", err);
        return -1;
    }

    g_app_ctx.state = REMOTE_STATE_ADVERTISING;
    IPRO_LOGI(LOG_TAG, "[BLE] Advertising: %s", DEVICE_NAME);
    return 0;
}

int remote_app_stop_advertising(void)
{
    if (g_app_ctx.state != REMOTE_STATE_ADVERTISING) {
        return 0;
    }

    int err = bt_le_adv_stop();
    if (err) {
        IPRO_LOGE(LOG_TAG, "Stop advertising failed: %d", err);
        return -1;
    }

    g_app_ctx.state = REMOTE_STATE_READY;
    return 0;
}

const remote_app_context_t *remote_app_get_context(void)
{
    return &g_app_ctx;
}

struct bt_conn *remote_app_get_conn(void)
{
    return g_conn;
}

int remote_app_simulate_button(btn_event_t event)
{
    if (!g_app_ctx.connected) {
        IPRO_LOGW(LOG_TAG, "Not connected");
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "Simulating button event: %d", event);

    int status = remote_notify_button(g_conn, event);
    return (status == 0) ? 0 : -1;
}

int remote_app_update_environment(int16_t temperature, uint8_t humidity)
{
    g_app_ctx.sim_temperature = temperature;
    g_app_ctx.sim_humidity = humidity;

    if (!g_app_ctx.connected) {
        return 0;  /* Just update local values */
    }

    env_data_t env = {
        .temperature = temperature,
        .humidity = humidity
    };

    int status = remote_notify_environment(g_conn, &env);
    return (status == 0) ? 0 : -1;
}

int remote_app_send_uart(const char *message)
{
    if (!g_app_ctx.connected) {
        IPRO_LOGW(LOG_TAG, "Not connected");
        return -1;
    }

    int status = remote_uart_send_string(g_conn, message);
    return (status == 0) ? 0 : -1;
}

int remote_app_start_env_notify(uint32_t interval_ms)
{
    if (g_env_timer == NULL) {
        return -1;
    }

    xTimerChangePeriod(g_env_timer, pdMS_TO_TICKS(interval_ms), 0);
    g_app_ctx.env_notify_running = true;
    xTimerStart(g_env_timer, 0);

    IPRO_LOGI(LOG_TAG, "Environment notify started (interval: %lu ms)", interval_ms);
    return 0;
}

int remote_app_stop_env_notify(void)
{
    if (g_env_timer == NULL) {
        return -1;
    }

    g_app_ctx.env_notify_running = false;
    xTimerStop(g_env_timer, 0);

    IPRO_LOGI(LOG_TAG, "Environment notify stopped");
    return 0;
}

int remote_app_set_led(bool on, uint8_t brightness)
{
    led_state_t state;
    remote_get_led_state(&state);
    state.on = on;
    state.brightness = brightness;
    remote_set_led_state(&state);

    /* Control actual GPIO (PWM not implemented, use threshold) */
    gpio_write(LED_GPIO_PIN, (on && brightness > 0) ? 1 : 0);

    IPRO_LOGI(LOG_TAG, "[LED] Set: %s, brightness=%d%% (GPIO%d)", on ? "ON" : "OFF", brightness, LED_GPIO_PIN);
    return 0;
}

void remote_app_print_status(void)
{
    const char *state_str[] = {
        "IDLE", "INITIALIZING", "READY", "ADVERTISING", "CONNECTED", "ERROR"
    };

    printf("\n========== Remote App Status ==========\n");
    printf("State: %s\n", state_str[g_app_ctx.state]);
    printf("Connected: %s\n", g_app_ctx.connected ? "Yes" : "No");

    if (g_app_ctx.connected && g_conn) {
        printf("Connection ID: %d\n", g_app_ctx.conn_id);
        printf("MTU: %d\n", bt_gatt_get_mtu(g_conn));
    }

    led_state_t led;
    remote_get_led_state(&led);
    printf("LED: %s, Brightness: %d%%, Mode: %d\n",
           led.on ? "ON" : "OFF", led.brightness, led.mode);

    printf("Temperature: %d.%02dC\n",
           g_app_ctx.sim_temperature / 100, abs(g_app_ctx.sim_temperature % 100));
    printf("Humidity: %d%%\n", g_app_ctx.sim_humidity);
    printf("Env Notify: %s\n", g_app_ctx.env_notify_running ? "Running" : "Stopped");

    const remote_svc_handles_t *handles = remote_services_get_handles();
    printf("\n--- Notification States ---\n");
    printf("Button: %s\n", handles->btn_notify_enabled ? "Enabled" : "Disabled");
    printf("Temperature: %s\n", handles->temp_notify_enabled ? "Enabled" : "Disabled");
    printf("Humidity: %s\n", handles->humid_notify_enabled ? "Enabled" : "Disabled");
    printf("UART: %s\n", handles->uart_notify_enabled ? "Enabled" : "Disabled");
    printf("=========================================\n\n");
}

/*
 * PIR + IMAGE API IMPLEMENTATION
 ****************************************************************************************
 */

int remote_app_start_pir(uint8_t gpio_pin)
{
    if (!g_pir_service_enabled) {
        IPRO_LOGW(LOG_TAG, "PIR service not enabled");
        return -1;
    }

    int status = pir_sensor_start(gpio_pin);
    if (status == 0) {
        IPRO_LOGI(LOG_TAG, "PIR sensor started on GPIO%d", gpio_pin);
        return 0;
    }
    return -1;
}

int remote_app_stop_pir(void)
{
    if (!g_pir_service_enabled) {
        return -1;
    }

    pir_sensor_stop();
    IPRO_LOGI(LOG_TAG, "PIR sensor stopped");
    return 0;
}

int remote_app_pir_trigger(void)
{
    if (!g_pir_service_enabled) {
        IPRO_LOGW(LOG_TAG, "PIR service not enabled");
        return -1;
    }

    int status = pir_simulate_trigger();
    return (status == 0) ? 0 : -1;
}

int remote_app_capture_image(void)
{
    if (!g_pir_service_enabled) {
        IPRO_LOGW(LOG_TAG, "PIR/Image service not enabled");
        return -1;
    }

    int status = image_capture();
    if (status == 0) {
        img_info_t info;
        image_get_info(&info);
        IPRO_LOGI(LOG_TAG, "Image captured: %lu bytes, %d chunks",
                  info.image_size, info.chunk_count);
        return 0;
    }
    return -1;
}

int remote_app_start_image_transfer(void)
{
    if (!g_pir_service_enabled) {
        IPRO_LOGW(LOG_TAG, "PIR/Image service not enabled");
        return -1;
    }

    if (!g_app_ctx.connected) {
        IPRO_LOGW(LOG_TAG, "Not connected");
        return -1;
    }

    int status = image_start_transfer(g_app_ctx.conn_id);
    return (status == 0) ? 0 : -1;
}

void remote_app_print_pir_status(void)
{
    if (!g_pir_service_enabled) {
        printf("PIR/Image service: Not enabled\n");
        return;
    }

    pir_status_t pir;
    pir_get_status(&pir);

    img_info_t img;
    image_get_info(&img);

    const pir_img_handles_t *handles = pir_image_service_get_handles();

    const char *img_state_str[] = {"IDLE", "CAPTURING", "READY", "TRANSFERRING", "ERROR"};

    printf("\n========== PIR/Image Status ==========\n");
    printf("PIR Motion: %s\n", pir.motion_detected ? "YES" : "No");
    printf("PIR Events: %lu\n", pir.event_count);
    printf("Last Motion: %lu ms ago\n", (uint32_t)((xTaskGetTickCount() * portTICK_PERIOD_MS) - pir.timestamp));
    printf("\n");
    printf("Image State: %s\n", img_state_str[img.status]);
    printf("Image Size: %lu bytes\n", img.image_size);
    printf("Chunk Count: %d\n", img.chunk_count);
    printf("Chunk Size: %d bytes\n", img.chunk_size);
    printf("Transfer: %s\n", image_transfer_in_progress() ? "In Progress" : "Idle");
    printf("\n--- Notification States ---\n");
    printf("PIR Status: %s\n", handles->pir_notify_enabled ? "Enabled" : "Disabled");
    printf("Image Data: %s\n", handles->img_data_notify_enabled ? "Enabled" : "Disabled");
    printf("Image Info: %s\n", handles->img_info_notify_enabled ? "Enabled" : "Disabled");
    printf("=======================================\n\n");
}
