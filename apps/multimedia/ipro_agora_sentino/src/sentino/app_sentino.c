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
 * @file app_sentino.c
 * @brief Sentino MQTT client — connect, subscribe, publish, reconnect
 *
 * Uses lwIP MQTT client to connect to Sentino IoT platform.
 * HMAC-SHA1 authentication via sentino_auth module (ts=0, per reference SDK).
 * Auto-reconnects on disconnect with exponential backoff.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"  /* private: static mqtt_client_t + debug access */
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/tcpip.h"

#include "app_sentino.h"
#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif
#ifdef CONFIG_SHELL
#include "shell.h"
#endif
#include "sentino_auth.h"
#include "sentino_protocol.h"

#include "app_log.h"

#define LOG_TAG "SENTINO"

/* ========== Device Triple ========== */
/*
 * Per board, in PSM (the EasyFlash env), never in the source: set it on the console with
 * `sentino_triple set <pid> <uuid> <secret>` (it survives a firmware flash). Each board needs its
 * own triple: two boards with one triple fight over the same MQTT identity.
 */
#define TRIPLE_KEY_PID      "sentino_pid"
#define TRIPLE_KEY_UUID     "sentino_uuid"
#define TRIPLE_KEY_SECRET   "sentino_secret"
#define TRIPLE_PID_MAX      16      /* the BLE provisioning ADV carries pid and uuid in 16 bytes each */
#define TRIPLE_UUID_MAX     16
#define TRIPLE_SECRET_MAX   64      /* the HMAC key; the platform's are 32 hex characters */

static char g_pid[TRIPLE_PID_MAX + 1];
static char g_uuid[TRIPLE_UUID_MAX + 1];
static char g_secret[TRIPLE_SECRET_MAX + 1];
static const sentino_device_triple_t g_triple = { .uuid = g_uuid, .secret = g_secret, .pid = g_pid };

static const char *const k_triple_keys[3] = { TRIPLE_KEY_PID, TRIPLE_KEY_UUID, TRIPLE_KEY_SECRET };
static char *const k_triple_vals[3] = { g_pid, g_uuid, g_secret };
static const size_t k_triple_max[3] = { TRIPLE_PID_MAX, TRIPLE_UUID_MAX, TRIPLE_SECRET_MAX };

/* Field i is 1..k_triple_max[i] printable characters without spaces. */
static bool triple_field_ok(int i, const char *v)
{
    size_t n = v ? strlen(v) : 0;
    if (n == 0 || n > k_triple_max[i]) {
        return false;
    }
    for (size_t i = 0; i < n; i++) {
        if (v[i] <= ' ' || v[i] > '~') {
            return false;
        }
    }
    return true;
}

/* Reads the triple from PSM into g_pid / g_uuid / g_secret; false if any field is missing. */
static bool triple_load(void)
{
#ifdef CONFIG_EASYFLASH4
    for (int i = 0; i < 3; i++) {
        const char *v = ef_get_env(k_triple_keys[i]);    /* a shared buffer: copy it at once */
        if (!triple_field_ok(i, v)) {
            k_triple_vals[i][0] = '\0';
            return false;
        }
        memcpy(k_triple_vals[i], v, strlen(v) + 1);
    }
    return true;
#else
    return false;
#endif
}

/* ========== State ========== */
static mqtt_client_t *g_mqtt_client = NULL;
static volatile int g_connected = 0;
static TaskHandle_t g_mqtt_task = NULL;

/* Topic buffers: "rlink/v2/<pid>/<uuid>/report_response" at the longest pid and uuid */
#define TOPIC_MAX (sizeof("rlink/v2///report_response") + TRIPLE_PID_MAX + TRIPLE_UUID_MAX)
static char g_topic_report[TOPIC_MAX];
static char g_topic_report_resp[TOPIC_MAX];
static char g_topic_issue[TOPIC_MAX];
static char g_topic_issue_resp[TOPIC_MAX];

/* Message handlers */
static sentino_msg_handler_t g_issue_handler = NULL;
static sentino_msg_handler_t g_response_handler = NULL;

/* Incoming message reassembly buffer */
#define MQTT_RX_BUF_SIZE 2048
static char g_rx_buf[MQTT_RX_BUF_SIZE];
static uint16_t g_rx_len = 0;
static char g_rx_topic[80];
static volatile int g_rx_ready = 0; /* set in callback, cleared after processing */

/* Publish mutex */
static SemaphoreHandle_t g_pub_mutex = NULL;

/* Subscribe confirmation counter — wait for both subs before reporting ready */
static volatile int g_sub_ack_count = 0;
#define SENTINO_REQUIRED_SUBS 2

/* ========== Callbacks ========== */

/* Forward declarations for callbacks referenced in mqtt_connection_cb */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);

static void mqtt_sub_request_cb(void *arg, err_t result) {
    (void)arg;
    if (result == ERR_OK) {
        int count = __atomic_add_fetch(&g_sub_ack_count, 1, __ATOMIC_RELEASE);
        IPRO_LOGI(LOG_TAG, "Subscribe ACK received (%d/%d)", count, SENTINO_REQUIRED_SUBS);
        if (count >= SENTINO_REQUIRED_SUBS) {
            __atomic_store_n(&g_connected, 1, __ATOMIC_RELEASE);
            IPRO_LOGI(LOG_TAG, "All subscriptions confirmed, ready");
        }
    } else {
        IPRO_LOGE(LOG_TAG, "Subscribe NACK: %d", (int)result);
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
                               mqtt_connection_status_t status) {
    (void)arg;

    if (status == MQTT_CONNECT_ACCEPTED) {
        IPRO_LOGI(LOG_TAG, "Connected to %s:%d", SENTINO_MQTT_HOST, SENTINO_MQTT_PORT);

        /* Set callbacks AFTER connect (mqtt_client_connect does memset(0) on the struct!) */
        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb,
                                mqtt_incoming_data_cb, NULL);

        IPRO_LOGI(LOG_TAG, "DEBUG: client=%p data_cb=%p pub_cb=%p",
                  (void *)client, (void *)(uintptr_t)client->data_cb,
                  (void *)(uintptr_t)client->pub_cb);

        /* Reset sub ACK counter — don't mark g_connected until both subs confirmed */
        __atomic_store_n(&g_sub_ack_count, 0, __ATOMIC_RELEASE);

        /* Subscribe with QoS 1 and track ACKs (per reference SDK) */
        mqtt_subscribe(client, g_topic_report_resp, 1, mqtt_sub_request_cb, NULL);
        mqtt_subscribe(client, g_topic_issue, 1, mqtt_sub_request_cb, NULL);

        IPRO_LOGI(LOG_TAG, "Subscribing: %s (QoS 1)", g_topic_report_resp);
        IPRO_LOGI(LOG_TAG, "Subscribing: %s (QoS 1)", g_topic_issue);
    } else {
        __atomic_store_n(&g_connected, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_sub_ack_count, 0, __ATOMIC_RELEASE);
        if (status == MQTT_CONNECT_DISCONNECTED) {
            IPRO_LOGW(LOG_TAG, "Disconnected");
        } else {
            IPRO_LOGE(LOG_TAG, "Connection failed: status=%d", (int)status);
        }
    }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic,
                                     u32_t tot_len) {
    (void)arg;

    /* Save topic for data callback */
    strncpy(g_rx_topic, topic, sizeof(g_rx_topic) - 1);
    g_rx_topic[sizeof(g_rx_topic) - 1] = '\0';
    g_rx_len = 0;

    if (tot_len > MQTT_RX_BUF_SIZE - 1) {
        IPRO_LOGW(LOG_TAG, "RX message too large: %lu bytes", (unsigned long)tot_len);
    }
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data,
                                  u16_t len, u8_t flags) {
    (void)arg;

    /* Append to reassembly buffer */
    uint16_t copy_len = len;
    if (g_rx_len + copy_len >= MQTT_RX_BUF_SIZE) {
        copy_len = MQTT_RX_BUF_SIZE - 1 - g_rx_len;
    }
    if (copy_len > 0) {
        memcpy(&g_rx_buf[g_rx_len], data, copy_len);
        g_rx_len += copy_len;
    }

    /* Last fragment — signal task to process (don't parse in tcpip_thread!) */
    if (flags & MQTT_DATA_FLAG_LAST) {
        g_rx_buf[g_rx_len] = '\0';
        g_rx_ready = 1;

        /* Wake up the sentino_mqtt task to process the message */
        if (g_mqtt_task) {
            xTaskNotifyGive(g_mqtt_task);
        }
    }
}

/* ========== Deferred RX Processing (runs in sentino_mqtt task, not tcpip_thread) ========== */

static void process_rx_message(void) {
    if (!g_rx_ready) return;
    g_rx_ready = 0;

    IPRO_LOGI(LOG_TAG, "RX [%s] %u bytes", g_rx_topic, g_rx_len);

    /* Determine message type from topic suffix.
     * IMPORTANT: sentino_proto_parse_message uses json_extract_string which
     * inserts null terminators into the buffer. Parse code from a COPY so
     * the original g_rx_buf stays intact for the handler. */
    if (strstr(g_rx_topic, "/report_response")) {
        if (g_response_handler) {
            char parse_buf[128];
            uint16_t copy_len = (g_rx_len < sizeof(parse_buf) - 1)
                                ? g_rx_len : sizeof(parse_buf) - 1;
            memcpy(parse_buf, g_rx_buf, copy_len);
            parse_buf[copy_len] = '\0';

            const char *code = NULL;
            sentino_proto_parse_message(parse_buf, NULL, &code, NULL);
            if (code) {
                char code_buf[32] = {0};
                strncpy(code_buf, code, sizeof(code_buf) - 1);
                g_response_handler(code_buf, g_rx_buf);
            }
        }
    } else if (strstr(g_rx_topic, "/issue")) {
        /* Always log issue payload for debug (handler may not be registered) */
        IPRO_LOGI(LOG_TAG, "Issue RX %u bytes: %.*s",
                  g_rx_len, (int)(g_rx_len > 200 ? 200 : g_rx_len), g_rx_buf);
        if (g_issue_handler) {
            char parse_buf[128];
            uint16_t copy_len = (g_rx_len < sizeof(parse_buf) - 1)
                                ? g_rx_len : sizeof(parse_buf) - 1;
            memcpy(parse_buf, g_rx_buf, copy_len);
            parse_buf[copy_len] = '\0';

            const char *parsed_code = NULL;
            sentino_proto_parse_message(parse_buf, NULL, &parsed_code, NULL);
            if (parsed_code) {
                char code_buf[32] = {0};
                strncpy(code_buf, parsed_code, sizeof(code_buf) - 1);
                g_issue_handler(code_buf, g_rx_buf);
            }
        }
    }

    g_rx_len = 0;
}

/* ========== MQTT Connect ========== */

static int mqtt_do_connect(void) {
    /* Clean up old client — must disconnect TCP before free to prevent
     * stale lwIP callbacks firing on the freed client struct.
     * mqtt_disconnect() unconditionally: mqtt_client_is_connected() is true only
     * once CONNACK arrived, and a client freed while still connecting leaves its
     * pcb open and its cyclic timer armed on memory that mqtt_client_new() hands
     * straight back. The stale timer then times out the NEW connection
     * (status 257) and the old pcb's next segment trips
     * "mqtt_tcp_recv_cb: client->conn == pcb". It is a no-op when already closed. */
    if (g_mqtt_client) {
        LOCK_TCPIP_CORE();
        mqtt_disconnect(g_mqtt_client);
        mqtt_client_free(g_mqtt_client);
        UNLOCK_TCPIP_CORE();
        g_mqtt_client = NULL;
        /* Let lwIP process the TCP close before creating new client */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    LOCK_TCPIP_CORE();
    g_mqtt_client = mqtt_client_new();
    UNLOCK_TCPIP_CORE();
    if (!g_mqtt_client) {
        IPRO_LOGE(LOG_TAG, "Failed to create MQTT client");
        return -1;
    }

    /* Compute credentials (uses fixed ts=0 per reference SDK) */
    sentino_mqtt_creds_t creds;
    int ret = sentino_auth_compute(&g_triple, &creds);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Auth compute failed: %d", ret);
        return ret;
    }

    IPRO_LOGI(LOG_TAG, "Connecting as %s", creds.client_id);

    /* Resolve hostname */
    ip_addr_t broker_ip;
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_INET;
    ret = lwip_getaddrinfo(SENTINO_MQTT_HOST, NULL, &hints, &res);
    if (ret != 0 || !res) {
        IPRO_LOGE(LOG_TAG, "DNS resolve failed for %s", SENTINO_MQTT_HOST);
        return -2;
    }
    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    ip_addr_set_ip4_u32(&broker_ip, addr->sin_addr.s_addr);
    lwip_freeaddrinfo(res);

    IPRO_LOGI(LOG_TAG, "Resolved %s -> %s", SENTINO_MQTT_HOST, ipaddr_ntoa(&broker_ip));

    /* Configure MQTT connection */
    struct mqtt_connect_client_info_t ci = {0};
    ci.client_id = creds.client_id;
    ci.client_user = creds.username;
    ci.client_pass = creds.password;
    ci.keep_alive = SENTINO_MQTT_KEEPALIVE;

    /* Connect (callbacks set in mqtt_connection_cb AFTER connect succeeds,
     * because mqtt_client_connect does memset(0) on the struct) */
    LOCK_TCPIP_CORE();
    err_t err = mqtt_client_connect(g_mqtt_client, &broker_ip,
                                    SENTINO_MQTT_PORT,
                                    mqtt_connection_cb, NULL, &ci);
    UNLOCK_TCPIP_CORE();

    if (err != ERR_OK) {
        IPRO_LOGE(LOG_TAG, "mqtt_client_connect failed: %d", (int)err);
        return -3;
    }

    return 0;
}

/* ========== MQTT Task ========== */

static void sentino_mqtt_task(void *param) {
    (void)param;

    uint32_t reconnect_delay_ms = 3000;
    const uint32_t max_delay_ms = 30000;

    while (1) {
        if (!__atomic_load_n(&g_connected, __ATOMIC_ACQUIRE)) {
            IPRO_LOGI(LOG_TAG, "Attempting connection...");
            int ret = mqtt_do_connect();
            if (ret == 0) {
                /* mqtt_client_connect is non-blocking — wait for callback */
                IPRO_LOGI(LOG_TAG, "Connect initiated, waiting for callback...");
                vTaskDelay(pdMS_TO_TICKS(10000)); /* 10s for TCP + MQTT handshake */
                reconnect_delay_ms = 5000; /* reset on successful initiation */
                continue;
            }

            IPRO_LOGW(LOG_TAG, "Connect failed (%d), retry in %lu ms",
                      ret, (unsigned long)reconnect_delay_ms);
            vTaskDelay(pdMS_TO_TICKS(reconnect_delay_ms));

            /* Exponential backoff */
            reconnect_delay_ms *= 2;
            if (reconnect_delay_ms > max_delay_ms) {
                reconnect_delay_ms = max_delay_ms;
            }
        }

        /* Wait for notification (RX message) or timeout (reconnect check) */
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000));

        /* Process any deferred RX message */
        process_rx_message();
    }
}

/* ========== Public API ========== */

int app_sentino_init(void) {
    if (g_mqtt_client) {
        return 0; /* already initialized */
    }
    if (!triple_load()) {
        IPRO_LOGE(LOG_TAG, "no device triple in PSM: set it with `sentino_triple set <pid> <uuid> <secret>`");
        return -4;
    }

    /* Build topic strings */
    sentino_proto_build_report_topic(g_triple.pid, g_triple.uuid,
                                   g_topic_report, sizeof(g_topic_report));

    snprintf(g_topic_report_resp, sizeof(g_topic_report_resp),
             "rlink/v2/%s/%s/report_response", g_triple.pid, g_triple.uuid);
    snprintf(g_topic_issue, sizeof(g_topic_issue),
             "rlink/v2/%s/%s/issue", g_triple.pid, g_triple.uuid);

    sentino_proto_build_issue_response_topic(g_triple.pid, g_triple.uuid,
                                           g_topic_issue_resp,
                                           sizeof(g_topic_issue_resp));

    IPRO_LOGI(LOG_TAG, "Report topic: %s", g_topic_report);
    IPRO_LOGI(LOG_TAG, "Issue topic: %s", g_topic_issue);

    /* Create publish mutex */
    g_pub_mutex = xSemaphoreCreateMutex();
    if (!g_pub_mutex) {
        IPRO_LOGE(LOG_TAG, "Failed to create mutex");
        return -1;
    }

    /* Create MQTT client */
    LOCK_TCPIP_CORE();
    g_mqtt_client = mqtt_client_new();
    UNLOCK_TCPIP_CORE();
    if (!g_mqtt_client) {
        IPRO_LOGE(LOG_TAG, "Failed to create MQTT client");
        return -2;
    }

    /* Create MQTT task */
    BaseType_t xret = xTaskCreate(sentino_mqtt_task, "sentino_mqtt", 4096,
                                  NULL, tskIDLE_PRIORITY + 3, &g_mqtt_task);
    if (xret != pdPASS) {
        IPRO_LOGE(LOG_TAG, "Failed to create MQTT task");
        g_mqtt_client = NULL;
        return -3;
    }

    IPRO_LOGI(LOG_TAG, "Initialized (uuid=%s, pid=%s)", g_triple.uuid, g_triple.pid);
    return 0;
}

int app_sentino_is_connected(void) {
    return __atomic_load_n(&g_connected, __ATOMIC_ACQUIRE);
}

int app_sentino_issue_response(const char *issue_payload) {
    if (!g_mqtt_client || !app_sentino_is_connected() || !issue_payload) return -1;

    /* Extract id and code from issue payload using a copy */
    char parse_buf[128];
    uint16_t plen = (uint16_t)strlen(issue_payload);
    uint16_t copy_len = (plen < sizeof(parse_buf) - 1) ? plen : sizeof(parse_buf) - 1;
    memcpy(parse_buf, issue_payload, copy_len);
    parse_buf[copy_len] = '\0';

    const char *id = NULL;
    const char *code = NULL;
    sentino_proto_parse_message(parse_buf, &id, &code, NULL);

    char id_buf[32] = {0};
    char code_buf[32] = {0};
    if (id) strncpy(id_buf, id, sizeof(id_buf) - 1);
    if (code) strncpy(code_buf, code, sizeof(code_buf) - 1);

    /* Build response: echo back with res=0
     * Format: {"res":0,"id":"xxx","ts":NNN,"code":"property_set","data":{}} */
    char buf[256];
    uint32_t ts = (uint32_t)time(NULL);
    int n = snprintf(buf, sizeof(buf),
                     "{\"res\":0,\"id\":\"%s\",\"ts\":%lu,\"code\":\"%s\",\"data\":{}}",
                     id_buf, (unsigned long)ts, code_buf);
    if (n < 0 || (size_t)n >= sizeof(buf)) return -2;

    IPRO_LOGI(LOG_TAG, "Issue response: %s", buf);
    return app_sentino_publish(g_topic_issue_resp, buf);
}

int app_sentino_report(const char *code, const char *data_json, int ack) {
    if (!g_mqtt_client || !app_sentino_is_connected()) return -1;

    char buf[512];
    int n = sentino_proto_build_report(code, data_json, ack, buf, sizeof(buf));
    if (n < 0) return n;

    return app_sentino_publish(g_topic_report, buf);
}

int app_sentino_publish(const char *topic, const char *payload) {
    if (!g_mqtt_client || !topic || !payload) return -1;
    if (!app_sentino_is_connected()) return -2;

    xSemaphoreTake(g_pub_mutex, portMAX_DELAY);

    /* Must hold tcpip core lock — lwIP MQTT is not thread-safe.
     * Without this, tcp_output() from publish can trigger tcp_input()
     * reentrancy when broker responds in the same TCP exchange. */
    LOCK_TCPIP_CORE();
    err_t err = mqtt_publish(g_mqtt_client, topic, payload,
                             (u16_t)strlen(payload), 0, 0, NULL, NULL);
    UNLOCK_TCPIP_CORE();

    xSemaphoreGive(g_pub_mutex);

    if (err != ERR_OK) {
        IPRO_LOGE(LOG_TAG, "Publish failed: topic=%s err=%d", topic, (int)err);
        return -3;
    }

    return 0;
}

void app_sentino_set_issue_handler(sentino_msg_handler_t handler) {
    g_issue_handler = handler;
}

void app_sentino_set_response_handler(sentino_msg_handler_t handler) {
    g_response_handler = handler;
}

void app_sentino_get_device_info(const char **pid, const char **uuid) {
    if (!g_uuid[0]) {
        (void)triple_load();    /* BLE provisioning asks before app_sentino_init() runs */
    }
    if (pid) *pid = g_triple.pid;
    if (uuid) *uuid = g_triple.uuid;
}

void app_sentino_deinit(void) {
    if (g_mqtt_task) {
        vTaskDelete(g_mqtt_task);
        g_mqtt_task = NULL;
    }
    if (g_mqtt_client) {
        /* Same as mqtt_do_connect(): close in any state, under the core lock. */
        LOCK_TCPIP_CORE();
        mqtt_disconnect(g_mqtt_client);
        mqtt_client_free(g_mqtt_client);
        UNLOCK_TCPIP_CORE();
        g_mqtt_client = NULL;
    }
    if (g_pub_mutex) {
        vSemaphoreDelete(g_pub_mutex);
        g_pub_mutex = NULL;
    }
    __atomic_store_n(&g_connected, 0, __ATOMIC_RELEASE);
}

/* ========== Shell ========== */
#ifdef CONFIG_SHELL
/* sentino_triple                              shows the triple (the secret only as set / not set)
 * sentino_triple set <pid> <uuid> <secret>    saves it to PSM; reboot to connect with it
 * sentino_triple clear                        removes it */
static int cmd_sentino_triple(int argc, char **argv)
{
#ifdef CONFIG_EASYFLASH4
    if (argc == 5 && strcmp(argv[1], "set") == 0) {
        const char *in[3] = { argv[2], argv[3], argv[4] };
        for (int i = 0; i < 3; i++) {
            if (!triple_field_ok(i, in[i])) {
                printf("%s: 1..%u characters, no spaces\r\n", k_triple_keys[i], (unsigned)k_triple_max[i]);
                return -1;
            }
        }
        for (int i = 0; i < 3; i++) {
            if (ef_set_env(k_triple_keys[i], in[i]) != EF_NO_ERR) {
                printf("saving %s failed\r\n", k_triple_keys[i]);
                return -1;
            }
        }
        printf("saved; reboot to connect with it\r\n");
        return 0;
    }
    if (argc == 2 && strcmp(argv[1], "clear") == 0) {
        for (int i = 0; i < 3; i++) {
            (void)ef_del_env(k_triple_keys[i]);
        }
        printf("cleared\r\n");
        return 0;
    }
    if (argc == 1) {
        const bool ok = triple_load();
        printf("pid %s, uuid %s, secret %s\r\n", g_pid[0] ? g_pid : "(not set)", g_uuid[0] ? g_uuid : "(not set)",
               g_secret[0] ? "set" : "(not set)");
        printf("%s\r\n", ok ? "complete" : "incomplete: sentino_triple set <pid> <uuid> <secret>");
        return 0;
    }
    printf("usage: sentino_triple [set <pid> <uuid> <secret> | clear]\r\n");
    return -1;
#else
    (void)argc;
    (void)argv;
    printf("no PSM in this build (CONFIG_EASYFLASH4)\r\n");
    return -1;
#endif
}
SHELL_CMD_EXPORT_ALIAS(cmd_sentino_triple, sentino_triple, "sentino_triple [set <pid> <uuid> <secret> | clear]: device triple in PSM");
#endif
