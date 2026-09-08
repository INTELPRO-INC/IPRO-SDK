/**
 * @file audio_net_demo.c
 * @brief Transfer captured audio over ESP32-AT WiFi
 *
 * Shell commands:
 *   audio_net_upload <file> <host_ip> <port>
 *       - Read <file> from SD, open TCP to host:port, send raw bytes.
 *       - Host side:  nc -l <port> > out.pcm
 *
 *   audio_net_download <host_ip> <port> <file>
 *       - Open TCP to host:port, drain incoming bytes to <file> on SD.
 *       - Host side:  cat in.pcm | nc -l <port>
 *
 * Intended use: automated loopback verification. After audio_duplex
 * produces /sdcard/audio/loopback_{src,rec}.pcm, upload rec.pcm to a
 * host script that compares it against src.pcm (correlation, SNR, etc).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <generated/autoconf.h>

#ifdef CONFIG_DEMO_ESP32_AT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "shell.h"

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "esp32_wifi_at.h"
#include "at_client.h"

/* ESP-AT default CIPSEND max is 2048 bytes per transaction */
#define CIPSEND_CHUNK   2048

/* Send a single CIPSEND chunk: AT+CIPSEND=<n> -> prompt -> raw bytes */
static int cipsend_chunk(at_client_t *at, const void *data, int len)
{
    char cmd[48];
    char resp[64];

    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", len);
    int rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 3000);
    if (rc != 0) return rc;

    /* ESP-AT emits '>' prompt then expects exactly <len> raw bytes.
     * esp32_wifi_at_cmd() has already waited for OK; the prompt arrives
     * slightly after. Small sleep then push bytes. */
    vTaskDelay(pdMS_TO_TICKS(80));
    at_client_write(at, data, (uint32_t)len);
    /* Give ESP time to forward over WiFi and emit SEND OK before next cmd */
    vTaskDelay(pdMS_TO_TICKS(180));
    return 0;
}

static int cmd_audio_net_upload(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: audio_net_upload <file> <host_ip> <port>\r\n");
        printf("   ex: audio_net_upload /sdcard/audio/loopback_rec.pcm "
               "192.168.1.10 8000\r\n");
        printf("   host: nc -l 8000 > rec.pcm\r\n");
        return 0;
    }
    const char *path    = argv[1];
    const char *host_ip = argv[2];
    int         port    = atoi(argv[3]);

    /* Verify file */
    struct stat st;
    if (stat(path, &st) != 0) {
        printf("net_upload: stat %s failed\r\n", path);
        return 0;
    }
    uint32_t file_size = (uint32_t)st.st_size;
    if (file_size == 0) {
        printf("net_upload: %s is empty\r\n", path);
        return 0;
    }

    /* Verify WiFi */
    at_client_t *at = esp32_wifi_get_at_client();
    if (!at) {
        printf("net_upload: ESP32-AT not initialized\r\n");
        return 0;
    }

    /* Open file */
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("net_upload: open %s failed\r\n", path);
        return 0;
    }

    /* Allocate chunk buffer on heap (keeps stack small) */
    uint8_t *chunk = pvPortMalloc(CIPSEND_CHUNK);
    if (!chunk) {
        printf("net_upload: chunk alloc failed\r\n");
        close(fd);
        return 0;
    }

    printf("net_upload: %s (%lu bytes) -> %s:%d\r\n",
           path, (unsigned long)file_size, host_ip, port);

    /* Single-connection mode, CIPSTART TCP */
    char cmd[128], resp[256];
    esp32_wifi_at_cmd("AT+CIPMUX=0", resp, sizeof(resp), 2000);

    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", host_ip, port);
    int rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 8000);
    if (rc != 0) {
        printf("net_upload: CIPSTART failed: %s\r\n", resp);
        goto done;
    }

    /* Stream the file in chunks */
    uint32_t sent = 0;
    TickType_t start_tick = xTaskGetTickCount();
    while (sent < file_size) {
        uint32_t remain = file_size - sent;
        int n = (remain > CIPSEND_CHUNK) ? CIPSEND_CHUNK : (int)remain;

        int got = read(fd, chunk, n);
        if (got != n) {
            printf("net_upload: short read %d/%d at offset %lu\r\n",
                   got, n, (unsigned long)sent);
            rc = -1;
            break;
        }

        rc = cipsend_chunk(at, chunk, n);
        if (rc != 0) {
            printf("net_upload: CIPSEND failed at offset %lu: rc=%d\r\n",
                   (unsigned long)sent, rc);
            break;
        }
        sent += n;

        /* Progress every 16 KB */
        if ((sent & 0x3FFF) == 0 || sent == file_size) {
            uint32_t elapsed_ms = (xTaskGetTickCount() - start_tick) *
                                  portTICK_PERIOD_MS;
            uint32_t kbps = elapsed_ms ? (sent * 8 / elapsed_ms) : 0;
            printf("net_upload: %lu/%lu bytes (~%lu kbps)\r\n",
                   (unsigned long)sent, (unsigned long)file_size,
                   (unsigned long)kbps);
        }
    }

    esp32_wifi_at_cmd("AT+CIPCLOSE", resp, sizeof(resp), 3000);

    if (rc == 0) {
        printf("net_upload: done, %lu bytes sent\r\n", (unsigned long)sent);
    }

done:
    vPortFree(chunk);
    close(fd);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_audio_net_upload, audio_net_upload,
                       Upload audio file to host via TCP);

/* ---------------------------------------------------------------------------
 * Download direction (host -> DUT)
 *
 * ESP-AT passive receive: AT+CIPRECVTYPE=1 puts the link into passive mode,
 * +IPD URC only tells us data is available, and AT+CIPRECVDATA=<len> pulls
 * it. We don't use that path here because parsing the response stream
 * (mixed +CIPRECVDATA: headers + binary body + OK terminator) needs work
 * that's beyond the scope of this first cut. Instead use active mode:
 *   AT+CIPSTART -> +IPD,<len>:<bytes> URC arrives -> at_client callback
 *   must intercept and strip the header.
 *
 * For today's auto-test, upload is the useful direction (verify what the
 * mic heard). Host->DUT (pre-load test signal) can be done by writing the
 * file over UART via the shell's copy/hexdump mechanisms or by pre-staging
 * on SD; full WiFi download is a followup if the simple path is
 * insufficient.
 * ------------------------------------------------------------------------- */

#endif /* CONFIG_DEMO_ESP32_AT */
