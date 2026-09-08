/*
 * demo_esp32_at.c - ESP32-AT network application demo suite.
 *
 * Five shell commands showing customers how to build network applications
 * on top of the ESP32 AT client:
 *
 *   demo_wifi_scan                      - Scan and pretty-print nearby APs
 *   demo_net_info                       - Show link info (SSID, RSSI, IP, DNS)
 *   demo_net_ping <host> [count]        - Ping over the ESP32 link
 *   demo_http_get <url>                 - Fetch a URL via AT+HTTPCGET
 *   demo_reconnect_loop <iter>          - Repeatedly disconnect + reconnect,
 *                                         report success rate via event CB
 *
 * All demos rely on the public APIs in esp32_wifi_at.h. Customers can copy
 * any of these functions into their own application as a starting point.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "shell.h"
#include "esp32_wifi_at.h"
#include "mjpeg_frames.h"

#define DEMO_RESP_BUF   3072        /* large enough for AT+CWLAP (~15 APs) */
#define DEMO_URL_MAX    256

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static int demo_require_link(void)
{
    esp32_wifi_state_t st = esp32_wifi_get_state();
    if (st == ESP32_WIFI_STATE_OFF) {
        printf("[demo] ESP32 not initialized. Run: esp32_wifi_init\r\n");
        return -1;
    }
    return 0;
}

static int demo_require_ip(void)
{
    if (demo_require_link() != 0)
        return -1;
    if (esp32_wifi_get_state() != ESP32_WIFI_STATE_GOT_IP) {
        printf("[demo] No IP yet. Run: esp32_wifi_connect <ssid> <pw>\r\n");
        return -1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Case 1: Wi-Fi scan                                                  */
/* ------------------------------------------------------------------ */

static int cmd_demo_wifi_scan(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (demo_require_link() != 0)
        return -1;

    static char resp[DEMO_RESP_BUF];
    printf("[demo] Issuing AT+CWLAP (scan may take up to 10 s)...\r\n");
    int rc = esp32_wifi_at_cmd("AT+CWLAP", resp, sizeof(resp), 10000);
    if (rc != 0) {
        printf("[demo] Scan failed (rc=%d): %s\r\n", rc, resp);
        return rc;
    }

    /* Each line looks like:
     *   +CWLAP:(<ecn>,"<ssid>",<rssi>,"<bssid>",<ch>,...)
     * ecn values: 0=OPEN 1=WEP 2=WPA 3=WPA2 4=WPA/WPA2 5=WPA2-Ent 6=WPA3 7=WPA2/WPA3
     */
    static const char *enc[] = {
        "OPEN", "WEP", "WPA", "WPA2", "WPA/WPA2", "WPA2E", "WPA3", "WPA2/3"
    };

    printf("\r\n  # | %-24s | %-17s | CH | RSSI | SEC\r\n", "SSID", "BSSID");
    printf("----+--------------------------+-------------------+----+------+--------\r\n");

    int n = 0;
    const char *p = resp;
    while ((p = strstr(p, "+CWLAP:(")) != NULL) {
        p += 8;
        int ecn = 0, rssi = 0, ch = 0;
        char ssid[64] = {0}, bssid[32] = {0};
        /* strict parse of first five fields */
        if (sscanf(p, "%d,\"%63[^\"]\",%d,\"%31[^\"]\",%d",
                   &ecn, ssid, &rssi, bssid, &ch) >= 4) {
            const char *sec = (ecn >= 0 && ecn < (int)(sizeof(enc)/sizeof(enc[0])))
                              ? enc[ecn] : "?";
            printf(" %2d | %-24s | %-17s | %2d | %4d | %s\r\n",
                   ++n, ssid[0] ? ssid : "<hidden>", bssid, ch, rssi, sec);
        }
    }
    printf("\r\nTotal: %d AP(s)\r\n", n);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Case 2: Network info (SSID / RSSI / IP / DNS)                       */
/* ------------------------------------------------------------------ */

static int cmd_demo_net_info(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (demo_require_ip() != 0)
        return -1;

    char resp[512];

    /* AT+CWJAP? returns: +CWJAP:"ssid","bssid",ch,rssi,... */
    if (esp32_wifi_at_cmd("AT+CWJAP?", resp, sizeof(resp), 2000) == 0) {
        const char *p = strstr(resp, "+CWJAP:");
        if (p) {
            char ssid[64] = {0}, bssid[32] = {0};
            int ch = 0, rssi = 0;
            if (sscanf(p + 7, "\"%63[^\"]\",\"%31[^\"]\",%d,%d",
                       ssid, bssid, &ch, &rssi) >= 4) {
                printf("Associated : SSID=%s  BSSID=%s  CH=%d  RSSI=%d dBm\r\n",
                       ssid, bssid, ch, rssi);
            }
        }
    }

    esp32_wifi_ip_t ip;
    if (esp32_wifi_get_ip(&ip) == 0) {
        printf("Address    : IP=%s  GW=%s  MASK=%s\r\n",
               ip.ip, ip.gateway, ip.netmask);
    }

    /* AT+CIPDNS? returns: +CIPDNS:<enable>,"<dns1>"[,"<dns2>"] */
    if (esp32_wifi_at_cmd("AT+CIPDNS?", resp, sizeof(resp), 2000) == 0) {
        const char *p = strstr(resp, "+CIPDNS:");
        if (p) printf("DNS        : %s\r\n", p + 8);
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Case 3: Ping                                                        */
/* ------------------------------------------------------------------ */

static int cmd_demo_net_ping(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: demo_net_ping <host> [count]\r\n");
        printf("Example: demo_net_ping 8.8.8.8 4\r\n");
        return -1;
    }
    if (demo_require_ip() != 0)
        return -1;

    int count = (argc >= 3) ? atoi(argv[2]) : 4;
    if (count < 1) count = 1;
    if (count > 20) count = 20;

    const char *host = argv[1];
    char cmd[128];
    char resp[256];
    int ok = 0, fail = 0;

    printf("\r\nPinging %s x%d via ESP32...\r\n", host, count);
    for (int i = 1; i <= count; i++) {
        snprintf(cmd, sizeof(cmd), "AT+PING=\"%s\"", host);
        int rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 5000);
        if (rc == 0) {
            /* Response: +PING:<rtt_ms> */
            const char *p = strstr(resp, "+PING:");
            int rtt = p ? atoi(p + 6) : -1;
            printf("  [%d] %-20s OK   rtt=%d ms\r\n", i, host, rtt);
            ok++;
        } else {
            printf("  [%d] %-20s %s\r\n", i, host,
                   rc == -2 ? "TIMEOUT" : "ERROR");
            fail++;
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    printf("--- %s ping statistics ---\r\n", host);
    printf("%d transmitted, %d received, %d%% loss\r\n",
           count, ok, (int)(100 * fail / count));
    return (fail == 0) ? 0 : -1;
}

/* ------------------------------------------------------------------ */
/* Case 4: HTTP GET via AT+HTTPCGET                                    */
/* ------------------------------------------------------------------ */

static int cmd_demo_http_get(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: demo_http_get <url>\r\n");
        printf("Example: demo_http_get http://httpbin.org/ip\r\n");
        return -1;
    }
    if (demo_require_ip() != 0)
        return -1;

    const char *url = argv[1];
    if (strlen(url) > DEMO_URL_MAX - 32) {
        printf("[demo] URL too long (max %d)\r\n", DEMO_URL_MAX - 32);
        return -1;
    }

    static char cmd[DEMO_URL_MAX + 32];
    static char resp[DEMO_RESP_BUF];

    /* AT+HTTPCGET="<url>"[,<tx_size>,<rx_size>,<timeout>] */
    snprintf(cmd, sizeof(cmd), "AT+HTTPCGET=\"%s\"", url);
    printf("\r\n[demo] GET %s\r\n", url);

    int rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 20000);
    if (rc != 0) {
        printf("[demo] HTTP failed (rc=%d)\r\n", rc);
        return rc;
    }

    /* Response contains lines like:
     *   +HTTPCGET:<len>,<body_chunk>
     * followed by OK.
     */
    const char *body_start = strstr(resp, "+HTTPCGET:");
    if (!body_start) {
        printf("[demo] Unexpected reply:\r\n%s\r\n", resp);
        return -1;
    }
    /* Skip "+HTTPCGET:<len>," prefix to get the body chunk. */
    const char *comma = strchr(body_start, ',');
    const char *body  = comma ? comma + 1 : body_start;

    printf("----- response body -----\r\n%s\r\n-------------------------\r\n", body);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Case 5: Reconnect loop with event callback                          */
/* ------------------------------------------------------------------ */

static SemaphoreHandle_t s_reconn_sem;
static volatile int      s_reconn_got_ip;

static void reconn_event_cb(esp32_wifi_state_t st, void *ctx)
{
    (void)ctx;
    if (st == ESP32_WIFI_STATE_GOT_IP) {
        s_reconn_got_ip = 1;
        if (s_reconn_sem)
            xSemaphoreGive(s_reconn_sem);
    }
}

static int cmd_demo_reconnect_loop(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: demo_reconnect_loop <iterations>\r\n");
        printf("Note: you must already be connected once — this loop will\r\n"
               "      reuse the stored credentials on the ESP32.\r\n");
        return -1;
    }
    if (demo_require_ip() != 0)
        return -1;

    int iter = atoi(argv[1]);
    if (iter < 1)  iter = 1;
    if (iter > 50) iter = 50;

    if (!s_reconn_sem) {
        s_reconn_sem = xSemaphoreCreateBinary();
        if (!s_reconn_sem) {
            printf("[demo] sem alloc failed\r\n");
            return -1;
        }
    }
    esp32_wifi_set_event_cb(reconn_event_cb, NULL);

    int ok = 0;
    uint32_t total_ms = 0, worst_ms = 0;

    for (int i = 1; i <= iter; i++) {
        s_reconn_got_ip = 0;
        xSemaphoreTake(s_reconn_sem, 0);

        printf("\r\n[iter %d/%d] disconnect...\r\n", i, iter);
        esp32_wifi_disconnect();
        vTaskDelay(pdMS_TO_TICKS(500));

        printf("[iter %d/%d] reconnect (AT+CWJAP)...\r\n", i, iter);
        TickType_t t0 = xTaskGetTickCount();

        /* Re-issue CWJAP with stored config. On ESP-AT the last-used creds
         * are persisted, so AT+CWJAP with no args re-uses them. */
        char resp[128];
        int rc = esp32_wifi_at_cmd("AT+CWJAP", resp, sizeof(resp), 20000);
        if (rc != 0) {
            printf("  CWJAP failed rc=%d\r\n", rc);
            continue;
        }

        /* Wait for GOT_IP URC */
        if (xSemaphoreTake(s_reconn_sem, pdMS_TO_TICKS(10000)) != pdTRUE) {
            printf("  no GOT_IP URC within 10 s\r\n");
            continue;
        }

        uint32_t dt = (xTaskGetTickCount() - t0) * portTICK_PERIOD_MS;
        total_ms += dt;
        if (dt > worst_ms) worst_ms = dt;
        ok++;
        printf("  reconnected in %u ms\r\n", (unsigned)dt);
    }

    esp32_wifi_set_event_cb(NULL, NULL);

    printf("\r\n=== Reconnect summary ===\r\n");
    printf("  success : %d / %d (%d%%)\r\n", ok, iter, (int)(100 * ok / iter));
    if (ok > 0) {
        printf("  avg time: %u ms\r\n", (unsigned)(total_ms / ok));
        printf("  worst   : %u ms\r\n", (unsigned)worst_ms);
    }
    return (ok == iter) ? 0 : -1;
}

/* ------------------------------------------------------------------ */
/* Case 6 (Tier 3): TCP client (active receive) + CIPSEND prompt       */
/* ------------------------------------------------------------------ */

static SemaphoreHandle_t s_ipd_sem;
static int               s_ipd_len;
static char              s_ipd_data[128];

/*
 * ESP-AT active-mode URC: "+IPD,<len>:<data>"
 *   The first line delivered to us already contains the data payload
 *   inline. at_client strips trailing CR/LF when forming the line so
 *   single-line text responses land here complete.
 */
static void tcp_ipd_urc(const char *line, void *ctx)
{
    (void)ctx;
    const char *colon = strchr(line, ':');
    const char *comma = strchr(line, ',');
    if (!comma) return;

    s_ipd_len = atoi(comma + 1);
    if (colon && colon > comma) {
        size_t n = strlen(colon + 1);
        if (n >= sizeof(s_ipd_data))
            n = sizeof(s_ipd_data) - 1;
        memcpy(s_ipd_data, colon + 1, n);
        s_ipd_data[n] = '\0';
    } else {
        s_ipd_data[0] = '\0';
    }
    if (s_ipd_sem)
        xSemaphoreGive(s_ipd_sem);
}

static int cmd_demo_tcp_client(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: demo_tcp_client <host> <port>\r\n");
        printf("Example: demo_tcp_client tcpbin.com 4242   (echo server)\r\n");
        return -1;
    }
    if (demo_require_ip() != 0)
        return -1;

    at_client_t *at = esp32_wifi_get_at_client();
    if (!at) {
        printf("[demo] at_client not ready\r\n");
        return -1;
    }

    const char *host = argv[1];
    int port = atoi(argv[2]);
    char cmd[160];
    char resp[512];
    int rc;

    if (!s_ipd_sem)
        s_ipd_sem = xSemaphoreCreateBinary();
    xSemaphoreTake(s_ipd_sem, 0);
    s_ipd_len = 0;
    s_ipd_data[0] = '\0';

    printf("\r\n[demo] TCP client flow:\r\n");
    printf("        (1) AT+CIPMUX=0            (single connection)\r\n");
    printf("        (2) register +IPD URC      (active receive)\r\n");
    printf("        (3) AT+CIPSTART TCP %s:%d\r\n", host, port);
    printf("        (4) AT+CIPSEND=<len> + raw payload\r\n");
    printf("        (5) wait +IPD,<len>:<data> URC\r\n");
    printf("        (6) AT+CIPCLOSE\r\n\r\n");

    /* (1) single-connection mode */
    esp32_wifi_at_cmd("AT+CIPMUX=0", resp, sizeof(resp), 2000);

    /* (2) register URC for active-mode data delivery */
    at_client_register_urc(at, "+IPD,", tcp_ipd_urc, NULL);

    /* (3) connect */
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", host, port);
    rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 10000);
    if (rc != 0) {
        printf("[demo] CIPSTART failed: %s\r\n", resp);
        goto cleanup_urc;
    }
    printf("[demo] connected\r\n");

    /* (4) CIPSEND: ESP replies "OK\r\n>" so at_client_send_cmd returns on
     *     OK; the ">" byte sits in line_buf. We then raw-write the payload.
     *     ESP emits "Recv N bytes" + "SEND OK" which we don't explicitly
     *     wait for — we rely on the subsequent +IPD URC for the echo. */
    const char *payload = "hello from ipro7\r\n";
    int plen = (int)strlen(payload);

    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", plen);
    rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 2000);
    if (rc != 0) {
        printf("[demo] CIPSEND prep failed\r\n");
        goto cleanup_close;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    at_client_write(at, payload, plen);
    printf("[demo] sent %d bytes: %s", plen, payload);

    /* (5) wait for active-mode data */
    if (xSemaphoreTake(s_ipd_sem, pdMS_TO_TICKS(5000)) != pdTRUE) {
        printf("[demo] no +IPD URC within 5s\r\n");
        rc = -2;
        goto cleanup_close;
    }
    printf("[demo] received %d bytes:\r\n    \"%s\"\r\n",
           s_ipd_len, s_ipd_data);
    rc = 0;

cleanup_close:
    esp32_wifi_at_cmd("AT+CIPCLOSE", resp, sizeof(resp), 2000);
cleanup_urc:
    at_client_unregister_urc(at, "+IPD,");
    return rc;
}

/* ------------------------------------------------------------------ */
/* Case 7 (Tier 4): MQTT pub/sub round-trip                            */
/* ------------------------------------------------------------------ */

static SemaphoreHandle_t s_mqtt_sem;

static void mqtt_recv_urc(const char *line, void *ctx)
{
    (void)ctx;
    /* line: +MQTTSUBRECV:<link>,"<topic>",<len>,<data> */
    printf("[MQTT RECV] %s\r\n", line);
    if (s_mqtt_sem)
        xSemaphoreGive(s_mqtt_sem);
}

static int cmd_demo_mqtt_pubsub(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (demo_require_ip() != 0)
        return -1;

    at_client_t *at = esp32_wifi_get_at_client();
    if (!at) {
        printf("[demo] at_client not ready\r\n");
        return -1;
    }

    char cmd[200];
    char resp[512];
    char cid[32];
    char topic[64];
    int rc;

    /* Unique client ID + topic per run so broker doesn't reject duplicates */
    unsigned tick = (unsigned)xTaskGetTickCount();
    snprintf(cid,   sizeof(cid),   "ipro7_%u", tick);
    snprintf(topic, sizeof(topic), "/ipro7/test/%u", tick);

    if (!s_mqtt_sem)
        s_mqtt_sem = xSemaphoreCreateBinary();
    xSemaphoreTake(s_mqtt_sem, 0);

    printf("\r\n[demo] MQTT pubsub on broker.hivemq.com\r\n");
    printf("        client : %s\r\n", cid);
    printf("        topic  : %s\r\n\r\n", topic);

    /* MQTTUSERCFG=<link>,<scheme>,<cid>,<user>,<pass>,<cert>,<CA>,<path>
     * scheme=1 -> MQTT over TCP, no TLS */
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"", cid);
    rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 3000);
    if (rc != 0) { printf("[demo] USERCFG fail\r\n"); goto done; }

    rc = esp32_wifi_at_cmd("AT+MQTTCONN=0,\"broker.hivemq.com\",1883,0",
                           resp, sizeof(resp), 15000);
    if (rc != 0) { printf("[demo] CONN fail: %s\r\n", resp); goto done; }
    printf("[demo] connected to broker\r\n");

    at_client_register_urc(at, "+MQTTSUBRECV:", mqtt_recv_urc, NULL);

    snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=0,\"%s\",1", topic);
    rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 5000);
    if (rc != 0) { printf("[demo] SUB fail\r\n"); goto cleanup; }
    printf("[demo] subscribed\r\n");

    /* Publish a message that the broker will echo back to our own sub */
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTPUB=0,\"%s\",\"hello from ipro7\",1,0", topic);
    rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 5000);
    if (rc != 0) { printf("[demo] PUB fail\r\n"); goto cleanup; }
    printf("[demo] published, waiting for echo...\r\n");

    if (xSemaphoreTake(s_mqtt_sem, pdMS_TO_TICKS(8000)) == pdTRUE) {
        printf("[demo] round-trip OK\r\n");
        rc = 0;
    } else {
        printf("[demo] no echo within 8s\r\n");
        rc = -2;
    }

cleanup:
    at_client_unregister_urc(at, "+MQTTSUBRECV:");
    esp32_wifi_at_cmd("AT+MQTTCLEAN=0", resp, sizeof(resp), 3000);
done:
    return rc;
}

/* ------------------------------------------------------------------ */
/* Case 8 (Tier 3+): MJPEG-over-HTTP server (RTSP alternative)         */
/* ------------------------------------------------------------------ */

/*
 * Serves a multipart/x-mixed-replace MJPEG stream from CIPSERVER so any
 * browser can open http://<ipro7_ip>:<port>/ and see animated frames.
 *
 * Production apps replace the frame source (mjpeg_frames[]) with live
 * camera capture — the rest of the flow is identical.
 */

static SemaphoreHandle_t s_srv_sem;
static volatile int      s_srv_link_id;
static volatile int      s_srv_closed;

static void srv_ipd_urc(const char *line, void *ctx)
{
    (void)ctx;
    /* "+IPD,<link>,<len>:..." — we only care that some client sent data */
    const char *p = strchr(line, ',');
    if (!p) return;
    s_srv_link_id = atoi(p + 1);
    if (s_srv_sem)
        xSemaphoreGive(s_srv_sem);
}

static void srv_closed_urc(const char *line, void *ctx)
{
    (void)line; (void)ctx;
    s_srv_closed = 1;
}

/* Send a CIPSEND-framed blob. ESP-AT's single CIPSEND is capped (2048 B in
 * default builds), so we chunk larger buffers. Each chunk: AT+CIPSEND=<link>,<n>
 * → wait prompt → raw write. */
#define SRV_CIPSEND_MAX  2048

static int srv_send_chunk(at_client_t *at, int link,
                          const void *data, int len)
{
    char cmd[48];
    char resp[64];
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d,%d", link, len);
    int rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 3000);
    if (rc != 0) return rc;
    vTaskDelay(pdMS_TO_TICKS(80));
    at_client_write(at, data, (uint32_t)len);
    /* leave enough time for ESP to ship and emit SEND OK before next cmd */
    vTaskDelay(pdMS_TO_TICKS(180));
    return 0;
}

static int srv_send(at_client_t *at, int link,
                    const void *data, int len)
{
    const uint8_t *p = (const uint8_t *)data;
    int remaining = len;
    while (remaining > 0) {
        int n = remaining > SRV_CIPSEND_MAX ? SRV_CIPSEND_MAX : remaining;
        int rc = srv_send_chunk(at, link, p, n);
        if (rc != 0) return rc;
        p         += n;
        remaining -= n;
    }
    return 0;
}

static int cmd_demo_mjpeg_server(int argc, char **argv)
{
    int port   = (argc >= 2) ? atoi(argv[1]) : 8080;
    int nframe = (argc >= 3) ? atoi(argv[2]) : 30;
    if (nframe < 1)   nframe = 1;
    if (nframe > 300) nframe = 300;

    if (demo_require_ip() != 0)
        return -1;

    at_client_t *at = esp32_wifi_get_at_client();
    if (!at) return -1;

    esp32_wifi_ip_t ip;
    esp32_wifi_get_ip(&ip);

    char cmd[96], resp[128];
    int rc;

    if (!s_srv_sem)
        s_srv_sem = xSemaphoreCreateBinary();
    xSemaphoreTake(s_srv_sem, 0);
    s_srv_link_id = -1;
    s_srv_closed  = 0;

    printf("\r\n[demo] MJPEG-over-HTTP server\r\n");
    printf("       open http://%s:%d/  in a browser\r\n", ip.ip, port);
    printf("       will serve %d frames then stop\r\n\r\n", nframe);

    /* Multi-conn + start server */
    esp32_wifi_at_cmd("AT+CIPMUX=1", resp, sizeof(resp), 2000);
    snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=1,%d", port);
    rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 3000);
    if (rc != 0) {
        printf("[demo] CIPSERVER failed: %s\r\n", resp);
        goto cleanup_mux;
    }

    at_client_register_urc(at, "+IPD,",    srv_ipd_urc,    NULL);
    at_client_register_urc(at, "0,CLOSED", srv_closed_urc, NULL);

    /* Wait for HTTP GET from first client */
    printf("[demo] waiting for client (timeout 60s)...\r\n");
    if (xSemaphoreTake(s_srv_sem, pdMS_TO_TICKS(60000)) != pdTRUE) {
        printf("[demo] no client connected\r\n");
        rc = -2;
        goto cleanup;
    }
    int link = s_srv_link_id;
    printf("[demo] client on link %d, streaming...\r\n", link);

    /* HTTP/1.0 response header announcing multipart stream */
    const char *hdr =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: close\r\n\r\n";
    if (srv_send(at, link, hdr, (int)strlen(hdr)) != 0) {
        printf("[demo] send header failed\r\n");
        goto cleanup_close;
    }

    /* Per-frame: part header + JPEG bytes + CRLF */
    for (int i = 0; i < nframe; i++) {
        if (s_srv_closed) {
            printf("[demo] client closed at frame %d\r\n", i);
            break;
        }
        int idx = i % MJPEG_FRAME_COUNT;
        const uint8_t *jpeg = mjpeg_frames[idx];
        int jlen = (int)mjpeg_frames_len[idx];

        char part[128];
        int plen = snprintf(part, sizeof(part),
                            "--frame\r\n"
                            "Content-Type: image/jpeg\r\n"
                            "Content-Length: %d\r\n\r\n", jlen);

        if (srv_send(at, link, part, plen) != 0) break;
        if (srv_send(at, link, jpeg, jlen) != 0) break;
        if (srv_send(at, link, "\r\n", 2)    != 0) break;

        if ((i & 3) == 0)
            printf("[demo] frame %d/%d\r\n", i + 1, nframe);
    }

    printf("[demo] streaming complete\r\n");

cleanup_close:
    snprintf(cmd, sizeof(cmd), "AT+CIPCLOSE=%d", link);
    esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 2000);
cleanup:
    at_client_unregister_urc(at, "+IPD,");
    at_client_unregister_urc(at, "0,CLOSED");
    esp32_wifi_at_cmd("AT+CIPSERVER=0", resp, sizeof(resp), 2000);
cleanup_mux:
    esp32_wifi_at_cmd("AT+CIPMUX=0", resp, sizeof(resp), 2000);
    return rc;
}

/* ------------------------------------------------------------------ */
/* Case 9: MJPEG-over-HTTP with LIVE camera frames (via ISP pipeline)  */
/* ------------------------------------------------------------------ */
/*
 * Taps into the ISP + MJPEG encoder pipeline already running inside
 * demo/mm/stream_control.c. User must start the pipeline first with
 * the `dvp_sensor` shell command. An observer callback registered into
 * stream_control copies each freshly-encoded JPEG into a staging
 * buffer; the HTTP server task pulls from the staging buffer on every
 * multipart iteration.
 */

#define DEMO_LIVE_JPG_CAP   (64 * 1024)

/* Forward declaration of stream_control's observer registration. */
extern void strmctrl_set_jpeg_observer(
    void (*cb)(const void *buf, uint32_t size, void *ctx), void *ctx);

static uint8_t          *s_live_buf;
static volatile uint32_t s_live_size;
static SemaphoreHandle_t s_live_sem;
static volatile bool     s_live_active;

static void live_jpeg_observer(const void *buf, uint32_t size, void *ctx)
{
    (void)ctx;
    if (!s_live_active) return;
    if (size == 0 || size > DEMO_LIVE_JPG_CAP) return;
    /* Drop frames while server is still shipping the previous one — no
     * pipelining, HTTP is slower than the camera. */
    if (s_live_size != 0) return;
    memcpy(s_live_buf, buf, size);
    s_live_size = size;
    if (s_live_sem)
        xSemaphoreGive(s_live_sem);
}

static int cmd_demo_mjpeg_live(int argc, char **argv)
{
    int port   = (argc >= 2) ? atoi(argv[1]) : 8080;
    int nframe = (argc >= 3) ? atoi(argv[2]) : 60;
    if (nframe < 1)    nframe = 1;
    if (nframe > 1000) nframe = 1000;

    if (demo_require_ip() != 0)
        return -1;

    at_client_t *at = esp32_wifi_get_at_client();
    if (!at) return -1;

    esp32_wifi_ip_t ip;
    esp32_wifi_get_ip(&ip);

    /* One-time staging buffer for the latest JPEG */
    if (!s_live_buf) {
        s_live_buf = pvPortMalloc(DEMO_LIVE_JPG_CAP);
        if (!s_live_buf) {
            printf("[demo] failed to alloc live buffer\r\n");
            return -1;
        }
    }
    if (!s_live_sem)
        s_live_sem = xSemaphoreCreateBinary();
    if (!s_srv_sem)
        s_srv_sem = xSemaphoreCreateBinary();

    char cmd[96], resp[128];
    int rc;

    xSemaphoreTake(s_srv_sem,  0);
    xSemaphoreTake(s_live_sem, 0);
    s_srv_link_id = -1;
    s_srv_closed  = 0;
    s_live_size   = 0;
    s_live_active = true;

    /* Subscribe to the camera pipeline */
    strmctrl_set_jpeg_observer(live_jpeg_observer, NULL);

    printf("\r\n[demo] MJPEG-over-HTTP server (LIVE camera)\r\n");
    printf("       open http://%s:%d/  in a browser or curl\r\n", ip.ip, port);
    printf("       prerequisite: run `dvp_sensor` once to bring up ISP\r\n");
    printf("       will serve up to %d frames then stop\r\n\r\n", nframe);

    /* Same CIPSERVER setup as demo_mjpeg_server */
    esp32_wifi_at_cmd("AT+CIPMUX=1", resp, sizeof(resp), 2000);
    snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=1,%d", port);
    rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 3000);
    if (rc != 0) {
        printf("[demo] CIPSERVER failed: %s\r\n", resp);
        goto cleanup_mux;
    }

    at_client_register_urc(at, "+IPD,",    srv_ipd_urc,    NULL);
    at_client_register_urc(at, "0,CLOSED", srv_closed_urc, NULL);

    printf("[demo] waiting for client (timeout 60s)...\r\n");
    if (xSemaphoreTake(s_srv_sem, pdMS_TO_TICKS(60000)) != pdTRUE) {
        printf("[demo] no client connected\r\n");
        rc = -2;
        goto cleanup;
    }
    int link = s_srv_link_id;
    printf("[demo] client on link %d, streaming live...\r\n", link);

    const char *hdr =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: close\r\n\r\n";
    if (srv_send(at, link, hdr, (int)strlen(hdr)) != 0) {
        printf("[demo] send header failed\r\n");
        goto cleanup_close;
    }

    int sent_ok = 0;
    for (int i = 0; i < nframe; i++) {
        if (s_srv_closed) {
            printf("[demo] client closed at frame %d\r\n", i);
            break;
        }

        /* Block until the ISP pipeline coughs up a frame. */
        if (xSemaphoreTake(s_live_sem, pdMS_TO_TICKS(5000)) != pdTRUE) {
            printf("[demo] no live frame within 5s - is `dvp_sensor` running?\r\n");
            rc = -3;
            break;
        }

        uint32_t jlen = s_live_size;
        /* While we ship this frame, allow the observer to accept the next. */

        char part[128];
        int plen = snprintf(part, sizeof(part),
                            "--frame\r\n"
                            "Content-Type: image/jpeg\r\n"
                            "Content-Length: %u\r\n\r\n", (unsigned)jlen);

        if (srv_send(at, link, part,        plen)      != 0) { s_live_size = 0; break; }
        if (srv_send(at, link, s_live_buf,  (int)jlen) != 0) { s_live_size = 0; break; }
        if (srv_send(at, link, "\r\n",      2)         != 0) { s_live_size = 0; break; }

        /* Release slot for the next observer callback. */
        s_live_size = 0;
        sent_ok++;

        if ((sent_ok & 3) == 1)
            printf("[demo] live frame %d/%d (%u B)\r\n", sent_ok, nframe, (unsigned)jlen);
    }

    printf("[demo] live streaming complete (%d/%d frames)\r\n", sent_ok, nframe);

cleanup_close:
    snprintf(cmd, sizeof(cmd), "AT+CIPCLOSE=%d", link);
    esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 2000);
cleanup:
    at_client_unregister_urc(at, "+IPD,");
    at_client_unregister_urc(at, "0,CLOSED");
    esp32_wifi_at_cmd("AT+CIPSERVER=0", resp, sizeof(resp), 2000);
cleanup_mux:
    esp32_wifi_at_cmd("AT+CIPMUX=0", resp, sizeof(resp), 2000);
    s_live_active = false;
    strmctrl_set_jpeg_observer(NULL, NULL);
    return rc;
}

/* ------------------------------------------------------------------ */
/* Shell registration                                                  */
/* ------------------------------------------------------------------ */

SHELL_CMD_EXPORT_ALIAS(cmd_demo_wifi_scan,       demo_wifi_scan,
                       Scan Wi-Fi APs via AT+CWLAP);
SHELL_CMD_EXPORT_ALIAS(cmd_demo_net_info,        demo_net_info,
                       Show link info (SSID/RSSI/IP/DNS));
SHELL_CMD_EXPORT_ALIAS(cmd_demo_net_ping,        demo_net_ping,
                       Ping host via AT+PING);
SHELL_CMD_EXPORT_ALIAS(cmd_demo_http_get,        demo_http_get,
                       HTTP GET via AT+HTTPCGET);
SHELL_CMD_EXPORT_ALIAS(cmd_demo_reconnect_loop,  demo_reconnect_loop,
                       Reconnect soak test);
SHELL_CMD_EXPORT_ALIAS(cmd_demo_tcp_client,      demo_tcp_client,
                       TCP client passive-receive demo);
SHELL_CMD_EXPORT_ALIAS(cmd_demo_mqtt_pubsub,     demo_mqtt_pubsub,
                       MQTT pub-sub round-trip demo);
SHELL_CMD_EXPORT_ALIAS(cmd_demo_mjpeg_server,    demo_mjpeg_server,
                       MJPEG over HTTP server (RTSP alternative));
SHELL_CMD_EXPORT_ALIAS(cmd_demo_mjpeg_live,      demo_mjpeg_live,
                       MJPEG over HTTP with live camera via ISP);
