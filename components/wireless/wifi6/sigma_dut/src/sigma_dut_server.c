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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <FreeRTOS.h>
#include <task.h>

#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "sigma_dut.h"
#include "sigma_dut_priv.h"

#define SIGMA_TASK_NAME     "sigma_dut"
#define SIGMA_TASK_STACK    4096
#define SIGMA_TASK_PRIO     15

/* Command tables — terminated by {NULL, NULL} sentinel */
static const struct sigma_cmd_entry *cmd_tables[] = {
    sigma_device_commands,
    sigma_sta_commands,
    sigma_he_commands,
    sigma_traffic_commands,
    NULL
};

static struct {
    TaskHandle_t task;
    int listen_fd;
    volatile int running;
} sigma_srv;

/* --- Response helpers --- */

static int sigma_send(struct sigma_conn *conn, const char *data, int len)
{
    int sent = 0;
    while (sent < len) {
        int n = send(conn->sockfd, data + sent, len - sent, 0);
        if (n <= 0)
            return -1;
        sent += n;
    }
    return 0;
}

int sigma_resp_raw(struct sigma_conn *conn, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(conn->tx_buf, SIGMA_RESP_MAX_LEN, fmt, ap);
    va_end(ap);

    if (len <= 0)
        return -1;

    return sigma_send(conn, conn->tx_buf, len);
}

int sigma_resp_complete(struct sigma_conn *conn, const char *extra_fmt, ...)
{
    int off = snprintf(conn->tx_buf, SIGMA_RESP_MAX_LEN, "status,COMPLETE");

    if (extra_fmt && extra_fmt[0]) {
        conn->tx_buf[off++] = ',';
        va_list ap;
        va_start(ap, extra_fmt);
        off += vsnprintf(conn->tx_buf + off, SIGMA_RESP_MAX_LEN - off,
                         extra_fmt, ap);
        va_end(ap);
    }

    conn->tx_buf[off++] = '\n';
    return sigma_send(conn, conn->tx_buf, off);
}

int sigma_resp_error(struct sigma_conn *conn, const char *extra_fmt, ...)
{
    int off = snprintf(conn->tx_buf, SIGMA_RESP_MAX_LEN, "status,ERROR");

    if (extra_fmt && extra_fmt[0]) {
        conn->tx_buf[off++] = ',';
        va_list ap;
        va_start(ap, extra_fmt);
        off += vsnprintf(conn->tx_buf + off, SIGMA_RESP_MAX_LEN - off,
                         extra_fmt, ap);
        va_end(ap);
    }

    conn->tx_buf[off++] = '\n';
    return sigma_send(conn, conn->tx_buf, off);
}

/* --- Command dispatch --- */

static sigma_cmd_handler_t sigma_find_handler(const char *name)
{
    for (int t = 0; cmd_tables[t]; t++) {
        for (const struct sigma_cmd_entry *e = cmd_tables[t]; e->name; e++) {
            if (strcasecmp(e->name, name) == 0)
                return e->handler;
        }
    }
    return NULL;
}

static int sigma_handle_line(struct sigma_conn *conn, const char *line)
{
    struct sigma_cmd cmd;

    if (sigma_parse_cmd(line, &cmd) < 0)
        return sigma_resp_error(conn, "errorCode,InvalidCommand");

    sigma_cmd_handler_t handler = sigma_find_handler(cmd.name);
    if (!handler) {
        printf("[sigma] unknown command: %s\n", cmd.name);
        return sigma_resp_error(conn, "errorCode,InvalidCommand");
    }

    return handler(conn, &cmd);
}

/* --- TCP server --- */

static void sigma_handle_client(int client_fd)
{
    struct sigma_conn conn;
    char rx_buf[SIGMA_CMD_MAX_LEN];
    int rx_off = 0;

    conn.sockfd = client_fd;

    printf("[sigma] client connected\n");

    while (sigma_srv.running) {
        int n = recv(client_fd, rx_buf + rx_off,
                     sizeof(rx_buf) - rx_off - 1, 0);
        if (n <= 0)
            break;

        rx_off += n;
        rx_buf[rx_off] = '\0';

        /* Process complete lines */
        char *line_start = rx_buf;
        char *nl;
        while ((nl = strchr(line_start, '\n')) != NULL) {
            *nl = '\0';
            sigma_handle_line(&conn, line_start);
            line_start = nl + 1;
        }

        /* Move remaining partial line to front */
        int remain = rx_off - (line_start - rx_buf);
        if (remain > 0)
            memmove(rx_buf, line_start, remain);
        rx_off = remain;

        /* Overflow protection */
        if (rx_off >= (int)sizeof(rx_buf) - 1) {
            printf("[sigma] rx buffer overflow, dropping\n");
            rx_off = 0;
        }
    }

    printf("[sigma] client disconnected\n");
}

static void sigma_server_task(void *param)
{
    uint16_t port = (uint16_t)(uintptr_t)param;
    struct sockaddr_in addr;
    int opt = 1;

    sigma_srv.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sigma_srv.listen_fd < 0) {
        printf("[sigma] socket create failed\n");
        goto out;
    }

    setsockopt(sigma_srv.listen_fd, SOL_SOCKET, SO_REUSEADDR,
               &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sigma_srv.listen_fd, (struct sockaddr *)&addr,
             sizeof(addr)) < 0) {
        printf("[sigma] bind port %u failed\n", port);
        goto out_close;
    }

    if (listen(sigma_srv.listen_fd, 1) < 0) {
        printf("[sigma] listen failed\n");
        goto out_close;
    }

    printf("[sigma] CAPI agent listening on port %u\n", port);

    while (sigma_srv.running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(sigma_srv.listen_fd,
                               (struct sockaddr *)&client_addr,
                               &client_len);
        if (client_fd < 0) {
            if (!sigma_srv.running)
                break;
            printf("[sigma] accept failed\n");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        sigma_handle_client(client_fd);
        close(client_fd);
    }

out_close:
    close(sigma_srv.listen_fd);
    sigma_srv.listen_fd = -1;
out:
    printf("[sigma] server stopped\n");
    sigma_srv.task = NULL;
    sigma_srv.running = 0;
    vTaskDelete(NULL);
}

/* --- Public API --- */

int sigma_dut_start(uint16_t port)
{
    if (sigma_srv.running) {
        printf("[sigma] already running\n");
        return -1;
    }

    if (port == 0)
        port = SIGMA_DUT_DEFAULT_PORT;

    sigma_srv.running = 1;
    sigma_srv.listen_fd = -1;

    BaseType_t ret = xTaskCreate(
        sigma_server_task,
        SIGMA_TASK_NAME,
        SIGMA_TASK_STACK,
        (void *)(uintptr_t)port,
        SIGMA_TASK_PRIO,
        &sigma_srv.task);

    if (ret != pdPASS) {
        printf("[sigma] task create failed\n");
        sigma_srv.running = 0;
        return -1;
    }

    return 0;
}

int sigma_dut_stop(void)
{
    if (!sigma_srv.running) {
        printf("[sigma] not running\n");
        return -1;
    }

    sigma_srv.running = 0;

    /* Close listen socket to unblock accept() */
    if (sigma_srv.listen_fd >= 0) {
        close(sigma_srv.listen_fd);
        sigma_srv.listen_fd = -1;
    }

    return 0;
}
