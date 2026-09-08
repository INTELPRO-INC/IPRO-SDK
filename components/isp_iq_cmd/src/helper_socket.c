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
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

//#include <aos/kernel.h>
//#include <cli.h>
//#include <errno.h>
//#include <core_rv32.h>

//#include <lwip/sockets.h>
//#include <lwip/tcp.h>

#include "hal_uart.h"

//#include "sample_common.h"
#include "helper_socket.h"
#include "isp_cmd_parse.h"
#include "img_struct.h"
//#include "img_param.h"
//#include<json-c/json.h>

#define UART_RX_BUFSZ   512
#define UART_TX_BUFSZ   512
#define UART_PORT  0   /* UART0，依實際板子修改 */

static uint8_t uart_rx_buf[UART_RX_BUFSZ];
static uint8_t uart_tx_buf[UART_TX_BUFSZ];

#define IPERF_BUFSZ     (4 * 1300)
#define PSOT 6666

#define PREFIX_BYTES    5
#define VERSION_BYTES   1
#define CMDIDX_BYTES    2
#define PAYLOADLEN_BYTES    8
#define PAYLOAD_BYTES       256
#define CHECKSUM_BYTES  1

#define PREFIX "IPROC"

/* iproc_cmd */
enum iproc_cmd {
    IPROC_MONITOR =  0,
    IPROC_GET,
    IPROC_SET,
    IPROC_DUMP,
    IPROC_LOAD,
    IPROC_RET,
    IPROC_RET_JSTR,
    IPROC_SIZE,
};

#define MIN(a,b) ((a)<(b) ? (a):(b))
#define MAX(a,b) ((a)>(b) ? (a):(b))

//static char re_message[512] = {'\0'};
static char cmd_version0_message[512] = {'\0'};

/* Pluggable transport write: defaults to UART, overridden by CDC */
static iproc_write_fn_t s_iproc_write_fn = NULL;

void iproc_set_write_fn(iproc_write_fn_t fn)
{
    s_iproc_write_fn = fn;
}

static int _uart_write(const void *buffer, uint32_t size)
{
    if (s_iproc_write_fn)
        return s_iproc_write_fn(buffer, size);
    return uart_write(UART_PORT, buffer, size);
}

static int _uart_read(void *buffer, uint32_t size)
{
    return uart_read(UART_PORT, buffer, size);
}


#if 1
static int add_formate(char *re_data,
                       unsigned long payload_len,
                       int iproc_cmd,
                       char *re_message)
{
    char *p = re_message;
    int written;

    /* basic sanity check */
    if (!re_message)
        return -1;

    if (iproc_cmd != IPROC_SIZE && !re_data)
        return -1;

    /* clear output buffer */
    memset(re_message, 0, 512);

    /* PREFIX: "IPROC" (5 bytes, no NUL) */
    memcpy(p, PREFIX, PREFIX_BYTES);
    p += PREFIX_BYTES;

    /* VERSION: "0" (1 byte) */
    memcpy(p, "0", VERSION_BYTES);
    p += VERSION_BYTES;

    /* CMD INDEX: 2 hex chars */
    written = snprintf(p, CMDIDX_BYTES + 1, "%02x", iproc_cmd);
    if (written != CMDIDX_BYTES)
        return -1;
    p += CMDIDX_BYTES;

    /* PAYLOAD LENGTH: 8 hex chars */
    written = snprintf(p, PAYLOADLEN_BYTES + 1, "%08lX", payload_len);
    if (written != PAYLOADLEN_BYTES)
        return -1;
    p += PAYLOADLEN_BYTES;

    /* PAYLOAD */
    if (iproc_cmd != IPROC_SIZE) {
        memcpy(p, re_data, payload_len);
        p += payload_len;
    }

    /* return total message size */
    return (int)(p - re_message);
}


#else
static char buf_temp[512] = {'\0'};

static int add_formate(char* re_data, unsigned long payload_len, int iproc_cmd, char *re_message)
{
    //char buf[512] = {'\0'};
    int buf_size = 0;
    
    memset(buf_temp, '\0', sizeof(char) * 512);

    strncat(buf_temp, "IPROC\0", PREFIX_BYTES);
    //printf("2buf: %s\r\n", buf);

    strncat(buf_temp, "0\0", VERSION_BYTES);
    //printf("3buf: %s\r\n", buf);

    char iproc_cmd_str[3] = {'\0'};
    sprintf(iproc_cmd_str, "%02x", iproc_cmd);
    strncat(buf_temp, iproc_cmd_str, sizeof(char) * CMDIDX_BYTES);
    //printf("4buf: %s\r\n", buf);

    char payload_len_str[9] = {'\0'};
    sprintf(payload_len_str, "%08lX", payload_len);
    //printf("payload_len_str: %s\r\n", payload_len_str);
    strncat(buf_temp, payload_len_str, sizeof(char) * PAYLOADLEN_BYTES);
    //printf("5buf: %s\r\n", buf);

    buf_size = PREFIX_BYTES + VERSION_BYTES + CMDIDX_BYTES + PAYLOADLEN_BYTES;

    if (iproc_cmd != IPROC_SIZE) {
        buf_size += payload_len;
        strncat(buf_temp, re_data, sizeof(char) * payload_len);
        //printf("6buf: %s\r\n", buf);
    }

    memcpy(re_message, buf_temp, sizeof(char) * buf_size);
    //printf("8buf: %s\r\n", re_message);

    return 0;
}
#endif
static int parseFormat(char* recv_data, int* cmd_version)
{
    char cmd_version_str[4] = "0x0";

    strncpy(&cmd_version_str[2], recv_data, VERSION_BYTES);
    *cmd_version = strtoul(cmd_version_str, NULL, 0);// must with '0x  atoi(cmd_version_str);
    recv_data+=VERSION_BYTES;

    return VERSION_BYTES;
}

static int replyClient(int connected, int reply_val, char* message, char* re_message)
{
    // the ret_message less then 512
    int payload_len = 0;
    int transfer_size = 0;
    int message_len = strlen(message);
    char payload_buf[512] = {'\0'};
    int fmt_size = 0;

    if (message_len >= 512) return -1;

    /* build payload in a separate buffer to avoid aliasing with re_message */
    if (reply_val == 0) strcat(payload_buf, "[OK]");
    else strcat(payload_buf, "[NG]");
    strncat(payload_buf, message, message_len);
    payload_len = strlen(payload_buf);

    fmt_size = add_formate(payload_buf, payload_len, IPROC_RET, re_message);
    if (fmt_size < 0) return -1;

    transfer_size = _uart_write((uint8_t *)re_message, fmt_size);
    if (transfer_size != fmt_size) return -1;

    return 0;
}

static int sendData(int connected, uint8_t *buf_addr, uint32_t buf_size)
{
    int cnt = 0;
    int total_size = buf_size;
    int reast_size = 0;
    int transfer_size = 0;

    while (total_size > 0) {
        reast_size = MIN(1024, total_size);
        //transfer_size = send(connected, buf_addr, reast_size, 0);
        transfer_size = _uart_write((uint8_t *)buf_addr, reast_size);
        if (transfer_size < 0) {
            printf("[uart] write error\r\n");
            return -1;
        } else if (transfer_size == 0) {
            continue;
        } else {
            total_size -= transfer_size;
            buf_addr += transfer_size; 
            //printf("total_size: %d, reast_size: %d \r\n", total_size, reast_size);
            if ((cnt++ % 50) == 0) printf("[uart] remaining data: %d KB\r\n", total_size>>10);
        }
    }
    return 0;
}

static int recvData(int connected, uint8_t *buf_addr, uint32_t buf_size)
{
    int total_size = buf_size;
    int reast_size = 0;
    int transfer_size = 0;

    total_size = buf_size;
    while (total_size > 0) {
        reast_size = MIN(1024, total_size);
        //transfer_size = recv(connected, buf_addr, reast_size, 0);
        transfer_size = _uart_read((uint8_t *)buf_addr, reast_size);
        printf("recved: %d\r\n", transfer_size);
        if (transfer_size < 0){
             printf("[uart] read error\r\n");
             return -1;
        } else if (transfer_size == 0) {
             continue;
        } else {
            total_size -= transfer_size;
            buf_addr += transfer_size; 
            //printf("total_size: %d, reast_size: %d \r\n", total_size, reast_size);
         }
    }
    return 0;
}

static int get_ISP_reg(int connected, char *recv_data_ptr, int payload_len, char* re_message)
{
    char *token;
    const char delim[2] = " ";
    char module[32] = {'\0'};
    char reg[32] = {'\0'};
    int reply_val = 0;

    token = strtok(recv_data_ptr, delim);
    sprintf(module, token);
    token = strtok(NULL, delim);
    sprintf(reg, token);

    reply_val = parse_isp_cmd(module, reg, NULL, payload_len, re_message);
    //printf("module:%s, reg:%s, re_message:%s\r\n", module, reg, re_message);

    return reply_val;
}

static int set_ISP_reg(int connected, char *recv_data_ptr, int payload_len, char* re_message)
{
    char *token;
    const char delim[2] = " ";
    char module[32] = {'\0'};
    char reg[32] = {'\0'};
    int reply_val = 0;

    token = strtok(recv_data_ptr, delim);
    sprintf(module, token);
    token = strtok(NULL, delim);
    sprintf(reg, token);
    token = strtok(NULL, delim);

    reply_val = parse_isp_cmd(module, reg, token, payload_len, re_message);
    //printf("module:%s, reg:%s, val:%s, re_message:%s\r\n", module, reg, token, re_message);

    return reply_val;
}

static int ret_False(int connected, char* re_message)
{
    int transfer_size = 0;

    memset(re_message, '\0', sizeof(char) * 512);
    add_formate(NULL, 0, IPROC_SIZE, re_message);
    strcat(re_message, "[NG]");
    //transfer_size = send(connected, re_message, strlen(re_message), 0);
    transfer_size = _uart_write((uint8_t *)re_message, strlen(re_message));
    if (transfer_size != strlen(re_message)) return -1;

    return 0;
}

static int dump_ISP_buff(int connected, char *recv_data_ptr, int payload_len, char* re_message)
{
    char *token;
    char *ret_val_ptr;
    const char delim[2] = " ";
    const char delim_colon[2] = ":";
    const char delim_comma[2] = ",";
    char module[32] = {'\0'};
    char reg[32] = {'\0'};
    //char addr_str[16] = {'\0'};
    //char size_str[16] = {'\0'};
    uint8_t *buf_addr = NULL;
    uint32_t buf_size;
    int reply_val =  0;
    int transfer_size = 0;

    // parse addr_cmd 
    token = strtok(recv_data_ptr, delim);
    sprintf(module, token);
    token = strtok(NULL, delim);
    sprintf(reg, token);
    token = strtok(NULL, delim);
    //printf("module:%s, reg:%s, token:%s\r\n", module, reg, token);

    // ISP_Dump_Parse
    reply_val = parse_isp_cmd(module, reg, token, payload_len, re_message);
    if (reply_val < 0) {
        ret_False(connected, re_message);
        return -1;
    }
    
    ret_val_ptr =  strtok(re_message, delim_colon);
    ret_val_ptr =  strtok(NULL, delim_colon);
    
    token =  strtok(ret_val_ptr, delim_comma);
    buf_addr = (uint8_t *)strtoul(token, NULL, 0);// must with '0x'
    
    token =  strtok(NULL, delim_comma);
    buf_size = strtoul(token, NULL, 0);// must with '0x'
    //printf("DUMP addr: 0x%x, size: %u\r\n", buf_addr, buf_size);

    // return mem size
    memset(re_message, '\0', sizeof(char) * 512);
    add_formate(NULL, buf_size, IPROC_SIZE, re_message);
    strcat(re_message, "[OK]");
    //printf("re_message: %s@, strlen(re_message): %d\r\n", re_message, strlen(re_message));
    //transfer_size = send(connected, re_message, strlen(re_message), 0);
    transfer_size = _uart_write((uint8_t *)re_message, strlen(re_message));
    if (transfer_size != strlen(re_message)) return -1;

    reply_val = sendData(connected, buf_addr, buf_size);

    return reply_val;
}

static int load_ISP_buff(int connected, char *recv_data_ptr, int payload_len, char* re_message)
{
    char *token;
    char *ret_val_ptr;
    const char delim[2] = " ";
    const char delim_colon[2] = ":";
    const char delim_comma[2] = ",";
    char module[32] = {'\0'};
    char reg[32] = {'\0'};
    int reply_val = 0;
    int transfer_size = 0;
    //char addr_str[16] = {'\0'};
    //char size_str[16] = {'\0'};
    uint8_t *buf_addr = 0;
    uint32_t buf_size = 0;
    //char hex_value[] = "0x00000000";

    if (payload_len == 0){
        ret_False(connected, re_message);
        return -1;
    }

   // parse addr_cmd 
   token = strtok(recv_data_ptr, delim);
   sprintf(module, token);
   token = strtok(NULL, delim);
   sprintf(reg, token);
   token = strtok(NULL, delim);
   if (strstr(reg, "gamma_curve") != NULL) {
        //buf_size = sizeof(gamma_cal_t);
   } else if (strstr(reg, "wdr_tone_curve") != NULL) {
        //buf_size = sizeof(ISP_WDR_TONE_CURVE_CONF);
   } else {
        buf_size = strtoul(token, NULL, 0);// must with '0x'
   }
   //printf("module:%s, reg:%s, buf_size:%d\r\n", module, reg, buf_size);

   add_formate(NULL, buf_size, IPROC_SIZE, re_message);
   strcat(re_message, "[OK]");
   //transfer_size = send(connected, re_message, strlen(re_message), 0);
   transfer_size = _uart_write((uint8_t *)re_message, strlen(re_message));
   if (transfer_size != strlen(re_message)) return -1;

   // ISP_Load_Parse
   memset(re_message, '\0', sizeof(char) * 512);
   reply_val = parse_isp_cmd(module, reg, token, payload_len, re_message);
   //printf("re_message:%s@\r\n",re_message);
   if (reply_val < 0) return -1;
   ret_val_ptr =  strtok(re_message, delim_colon);
   ret_val_ptr =  strtok(NULL, delim_colon);
   //printf("ret_val_ptr:%s@\r\n",ret_val_ptr);

   token =  strtok(ret_val_ptr, delim_comma);
   buf_addr = (uint8_t *)strtoul(token, NULL, 0);// must with '0x'
    //printf("buf_addr:%d, buf_size: 0x%x, \r\n", (uint32_t)buf_addr, buf_size);
    reply_val = recvData(connected, buf_addr, buf_size);
    //csi_dcache_clean_range(buf_addr, buf_size);

    return reply_val;
}

int cmd_version0(int connected, char* recv_data, char*re_message)
{
    //version tpye 0:{cmd_idx}{payload_en}{paylen}, paylen: {module} {reg} {val}
    char *recv_data_ptr = recv_data;
    int iproc_cmd;
    unsigned long payload_len = 0;
    int reply_val = 0;
    char iproc_cmd_str[5] = "0x00";
    char hex_value[11] = "0x00000000";

    memset(cmd_version0_message, '\0', sizeof(char) * 512);

    strncpy(&iproc_cmd_str[2], recv_data_ptr, CMDIDX_BYTES);
    iproc_cmd = strtoul(iproc_cmd_str, NULL, 0);// must with '0x  atoi(iproc_cmd_str);
    recv_data_ptr+=CMDIDX_BYTES;

    strncpy(&hex_value[2], recv_data_ptr, PAYLOADLEN_BYTES);
    payload_len = strtoul(hex_value, NULL, 0);// must with '0x'
    recv_data_ptr+=PAYLOADLEN_BYTES;

    if(iproc_cmd == IPROC_MONITOR) {
       sprintf(cmd_version0_message, "IPROC_MONITOR");
    } else if (iproc_cmd == IPROC_GET) {
       reply_val = get_ISP_reg(connected,recv_data_ptr, payload_len, cmd_version0_message);
    } else if (iproc_cmd == IPROC_SET) {
       reply_val = set_ISP_reg(connected,recv_data_ptr, payload_len, cmd_version0_message);
    } else if(iproc_cmd == IPROC_DUMP) {
       reply_val = dump_ISP_buff(connected, recv_data_ptr, payload_len, cmd_version0_message);
       memset(cmd_version0_message, '\0', sizeof(char) * 512);
    } else if(iproc_cmd == IPROC_LOAD) {
       reply_val = load_ISP_buff(connected, recv_data_ptr, payload_len, cmd_version0_message);
       memset(cmd_version0_message, '\0', sizeof(char) * 512);
    }
    if (iproc_cmd == IPROC_DUMP) return 0;
    reply_val = replyClient(connected, reply_val, cmd_version0_message, re_message);

    return reply_val;
}
#if 0
static int set_block_mode(int fd, int mode)
{
    int flags = 0;

    if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
        printf("Get flags of socket error");
        return -1;
    }

    flags = mode ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    if (fcntl(fd, F_SETFL, flags) < 0) {
        printf("Set flags of socket error");
        return -1;
    }
    return 0;
}
#endif
#if 1
void uart_server_task(void *pvParameters)
{
    int rx_len;
    int offset;
    int cmd_version;
    uint8_t *rx_ptr;

    printf("[UART] server start\n");

    while (1)
    {
        /* accumulate bytes until we have at least a full IPROC header */
        int total = 0;
        int header_len = PREFIX_BYTES + VERSION_BYTES + CMDIDX_BYTES + PAYLOADLEN_BYTES;
        int idle = 0;

        while (total < header_len) {
            rx_len = _uart_read(uart_rx_buf + total, UART_RX_BUFSZ - total);
            if (rx_len > 0) {
                total += rx_len;
                idle = 0;
            } else {
                vTaskDelay(pdMS_TO_TICKS(1));
                if (++idle > 200) break;  /* 200ms timeout, reset */
            }
        }

        if (total <= 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        rx_len = total;

        printf("[UART] RX %d bytes\n", rx_len);

        /* simple PREFIX check */
        if (rx_len < PREFIX_BYTES)
            continue;

        if (memcmp(uart_rx_buf, PREFIX, PREFIX_BYTES) != 0) {
            printf("[UART] invalid prefix\n");
            continue;
        }

        /* move pointer after PREFIX */
        rx_ptr = uart_rx_buf + PREFIX_BYTES;

        /* parse header */
        offset = parseFormat((char *)rx_ptr, &cmd_version);
        if (offset < 0) {
            printf("[UART] parseFormat failed\n");
            continue;
        }

        rx_ptr += offset;

        /* clear reply buffer */
        memset(uart_tx_buf, 0, UART_TX_BUFSZ);

        /* process command (reply is sent inside cmd_version0 via replyClient) */
        if (cmd_version == 0) {
            cmd_version0(-1, (char *)rx_ptr, (char *)uart_tx_buf);
        } else {
            printf("[UART] unsupported version %d\n", cmd_version);
            continue;
        }
    }

    printf("[UART] server stop\n");
}
/* Process one complete IPROC packet (called from CDC bulk_out or UART task) */
void iproc_process_bulk(uint8_t *data, uint32_t len)
{
    static uint8_t tx_buf[512];
    int offset, cmd_version;
    uint8_t *rx_ptr;

    if (len < PREFIX_BYTES) {
        printf("[IPROC] packet too short (%lu)\n", (unsigned long)len);
        return;
    }

    if (memcmp(data, PREFIX, PREFIX_BYTES) != 0) {
        printf("[IPROC] invalid prefix\n");
        return;
    }

    rx_ptr = data + PREFIX_BYTES;
    offset = parseFormat((char *)rx_ptr, &cmd_version);
    if (offset < 0) {
        printf("[IPROC] parseFormat failed\n");
        return;
    }
    rx_ptr += offset;

    memset(tx_buf, 0, sizeof(tx_buf));

    if (cmd_version == 0) {
        cmd_version0(-1, (char *)rx_ptr, (char *)tx_buf);
    } else {
        printf("[IPROC] unsupported version %d\n", cmd_version);
    }
}

void socket_server_entry(void)
{
    xTaskCreate(uart_server_task, "uart_server", 4096, NULL, 10, NULL);
}

#else
void socket_server(void *pvParameters)
{
    char *recv_data = NULL;
    uint32_t sin_size;
    char *recv_data_ptr;
    char iproc_key[6] = {'\0'};
    int cmd_version = 0;
    int reply_val __attribute__((unused)) = 0;
    int offset = 0;

    int sock = -1, connected, bytes_received = 0;
    struct sockaddr_in server_addr, client_addr;

    recv_data = (char *)pvPortMalloc(IPERF_BUFSZ);
    if (recv_data == NULL)
    {
        printf("No memory\r\n");
        goto __exit;
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket error\r\n");
        goto __exit;
    }

    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PSOT);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("Unable to bind\r\n");
        goto __exit;
    }

    if (listen(sock, 5) == -1) {
        printf("Listen error\r\n");
        goto __exit;
    }
    printf("wait to listen...\r\n");

    sin_size = sizeof(struct sockaddr_in);
    while(1) {
        connected = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *)&sin_size);
        if (connected < 0) {
               printf("accept error\r\n");
               goto __exit;
        }
        printf("new client connected from (%s, %d)\r\n",
                  inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

        while (1) {
            bytes_received = recv(connected, recv_data, IPERF_BUFSZ, 0);
            if (bytes_received <= 0) {
                printf("recv: %s (%d)\n", strerror(errno), errno);
                break;
            }
            printf("=========================================================\r\n");
            printf("Get:%s@ %d\r\n",recv_data, bytes_received);

            strncpy(iproc_key, recv_data, PREFIX_BYTES);
            if (strcmp(iproc_key, "IPROC") != 0) {
                memset(iproc_key, '\0', sizeof(char) * (PREFIX_BYTES + 1));
                break;
            }
            recv_data+=PREFIX_BYTES;

            offset = parseFormat(recv_data, &cmd_version);
            recv_data_ptr = recv_data + offset;
            if (cmd_version == 0) {
                memset(re_message, '\0', sizeof(char) * 512);
                reply_val = cmd_version0(connected, recv_data_ptr, re_message);
            } else{
                 reply_val = -1;
            }
       }

        if (connected >= 0) closesocket(connected);
        connected = -1;
    }

__exit:
    if (sock >= 0) closesocket(sock);
    if (recv_data) vPortFree(recv_data);

    //return 0;
}

void socket_server_entry(void)
{
    /*int host_len;
    char *host;
    const char name[] = "socket_server";
    host_len = strlen(name) + 4;
    host = pvPortMalloc(host_len);//mem will be free in tcpc_entry
    strcpy(host, name);
    aos_task_new("isp_socket", socket_server, host, 4096);*/
    
    xTaskCreate(socket_server, "socket_server", 4096, NULL, 10, NULL);
}
#endif
#if 0
static void socket_reg_cmd(char *buf, int len, int argc, char **argv)
{
    socket_server_entry();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "socket", "socket related register", socket_reg_cmd },
};

int helper_socket_cli_init(void)
{
    return 0;
}
#endif

