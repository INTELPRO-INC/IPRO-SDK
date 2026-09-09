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

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
/* DEBUG 2026-07-24: CONFIG_COMPONENTS_* visibility for the SDIO-only DEBUG
 * cmds below (the USB build has no sdiowifi component -> link errors). */
#include <generated/autoconf.h>
#include "wifi_mgmr_cli.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include "utils_getopt.h"
#include "utils_hex.h"
/* utils_parse_number[_adv] are declared here and called by the
 * utils_al_parse_number* wrappers below, which compile unconditionally — so the
 * declaration must be visible regardless of IOT_SDK_ADAPTER. */
#include "utils_string.h"
#ifdef IOT_SDK_ADAPTER
#include "cli.h"
#include "utils_hexdump.h"
//#include "bl_os_system.h"
#endif
#include "fhost.h"
#include "fhost_ipc.h"
#include "net_iperf_al_priv.h"
#include "net_ping_al_priv.h"
#include "net_tg_al_priv.h"
#include "arpa/inet.h"
#ifdef CFG_IPV6
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#endif
/* Outside the CFG_IPV6 block on purpose: the `arp` command needs the v4 ARP
 * table regardless of whether IPv6 is built. */
#include "lwip/etharp.h"
#if defined(CFG_CSIDMA_CLI) && CFG_CSIDMA_CLI
#include "export/csidma_cmd.h"
#endif

void utils_al_parse_number_adv(const char *str, char sep, uint8_t *buf, int buflen, int base, int *count)
{
    utils_parse_number_adv(str, sep, buf, buflen, base, count);
}

void utils_al_parse_number(const char *str, char sep, uint8_t *buf, int buflen, int base)
{
    utils_parse_number(str, sep, buf, buflen, base);
}

int utils_al_getopt_init(getopt_env_t *env, int opterr)
{
    return utils_getopt_init(env, opterr);
}

int utils_al_getopt(getopt_env_t *env, int argc, char * const argv[], const char *optstring)
{
    return utils_getopt(env, argc, argv, optstring);
}

char *utils_al_bin2hex(char *dst, const void *src, size_t count)
{
    return utils_bin2hex(dst, src, count);
}

void cmd_rw_iperf(int argc, char **argv)
{
    char conv, *token, *substr;
    struct fhost_iperf_settings iperf_settings;
    bool client_server_set = 0;
    int opt;
    getopt_env_t getopt_env;
    rtos_task_handle iperf_handle;

    if(argc >= 2 && !strcmp("stop", argv[1])) {
        if((iperf_handle = fhost_iperf_msg_handle_get()) != NULL) {
            fhost_iperf_sigkill_handler(iperf_handle);
            return;
        }
        printf("havn't start iperf \r\n");
        return;
    }

    fhost_iperf_settings_init(&iperf_settings);
    utils_al_getopt_init(&getopt_env, 0);
    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "b:w:c:f:i:l:n:p:sut:S:T:XI:h")) != -1) {
        switch (opt) {
        case 'b': // UDP bandwidth
        case 'w': // TCP window size
        {
            char *decimal_str;
            int decimal = 0;
            uint64_t value;

            token = getopt_env.optarg;
            decimal_str = strchr(token, '.');
            if (decimal_str) {
                int fact = 100;
                decimal_str++;
                while (*decimal_str >= '0' && *decimal_str <= '9') {
                    decimal += (*decimal_str - '0') * fact;
                    if (fact == 1)
                        break;
                    fact = fact / 10;
                    decimal_str++;
                }
            }

            value = atoi(token);
            conv = token[strlen(token) - 1];

            // convert according to [Gg Mm Kk]
            switch (conv) {
            case 'G':
            case 'g':
                value *= 1000000000;
                value += decimal * 1000000;
                break;
            case 'M':
            case 'm':
                value *= 1000000;
                value += decimal * 1000;
                break;
            case 'K':
            case 'k':
                value *= 1000;
                value += decimal;
                break;
            default:
                break;
            }

            if (opt == 'b') {
                iperf_settings.udprate = value;
                iperf_settings.flags.is_udp = true;
                iperf_settings.flags.is_bw_set = true;
                // if -l has already been processed, is_buf_len_set is true so don't overwrite that value.
                if (!iperf_settings.flags.is_buf_len_set)
                    iperf_settings.buf_len = FHOST_IPERF_DEFAULT_UDPBUFLEN;
            }
            else {
                // TCP window is ignored for now
            }
            break;
        }
        case 'c': // Client mode with server host to connect to
        {
            if (client_server_set)
                goto help;

            iperf_settings.flags.is_server = 0;
            client_server_set = true;

            if (fhost_ipc_parse_ip4(getopt_env.optarg, &iperf_settings.host_ip, NULL)) {
                printf("invalid IP address %s\n", getopt_env.optarg);
                return;  //FHOST_IPC_ERROR;
            }
            break;
        }
        case 'f': // format to print in
        {
            iperf_settings.format = getopt_env.optarg[0];
            break;
        }
        case 'i': // Interval between periodic reports
        {
            uint32_t interval = 0;
            substr = strchr(getopt_env.optarg, '.');

            if (substr) {
                *substr++ = '\0';
                interval += atoi(substr);
            }

            interval += atoi(getopt_env.optarg) * 10;
            if (interval < 5) {
                printf("interval must be greater than or "
                            "equal to 0.5. Interval set to 0.5\n");
                interval = 5;
            }

            iperf_settings.interval.sec = interval / 10;
            iperf_settings.interval.usec = 100000 * (interval - (iperf_settings.interval.sec * 10));
            iperf_settings.flags.show_int_stats = true;
            break;
        }
        case 'l': //Length of each buffer
        {
            uint32_t udp_min_size = sizeof(struct iperf_UDP_datagram);

            iperf_settings.buf_len = atoi( getopt_env.optarg );
            iperf_settings.flags.is_buf_len_set = true;
            if (iperf_settings.flags.is_udp && iperf_settings.buf_len < udp_min_size) {
                iperf_settings.buf_len = udp_min_size;
                printf("buffer length must be greater than or "
                            "equal to %d in UDP\n", udp_min_size);
            }
            break;
        }
        case 'n': // amount mode (instead of time mode)
        {
            iperf_settings.flags.is_time_mode = false;
            iperf_settings.amount = atoi( getopt_env.optarg );
            break;
        }
        case 'p': //server port
        {
            iperf_settings.port = atoi( getopt_env.optarg );
            break;
        }
        case 's': // server mode
        {
            if (client_server_set)
                goto help;
            iperf_settings.flags.is_server = 1;
            client_server_set = true;
            break;
        }
        case 't': // time mode (instead of amount mode)
        {
            iperf_settings.flags.is_time_mode = true;
            iperf_settings.amount = 0;
            substr = strchr(getopt_env.optarg, '.');
            if (substr) {
                *substr++ = '\0';
                iperf_settings.amount += atoi(substr);
            }

            iperf_settings.amount += atoi(getopt_env.optarg) * 10;
            break;
        }
        case 'u': // UDP instead of TCP
        {
            // if -b has already been processed, UDP rate will be non-zero, so don't overwrite that value
            if (!iperf_settings.flags.is_udp) {
                iperf_settings.flags.is_udp = true;
                iperf_settings.udprate = FHOST_IPERF_DEFAULT_UDPRATE;
            }

            // if -l has already been processed, is_buf_len_set is true, so don't overwrite that value.
            if (!iperf_settings.flags.is_buf_len_set) {
                iperf_settings.buf_len = FHOST_IPERF_DEFAULT_UDPBUFLEN;
            }
            break;
        }
        case 'S': // IP type-of-service
        {
            // the zero base allows the user to specify
            // hexadecimals: "0x#"
            // octals: "0#"
            // decimal numbers: "#"
            iperf_settings.tos = strtol( getopt_env.optarg, NULL, 0 );
            break;
        }
        case 'T': // TTL
        {
            iperf_settings.ttl = atoi(getopt_env.optarg);
            break;
        }
        case 'X': // Peer version detect
        {
            iperf_settings.flags.is_peer_ver = true;
            break;
        }
        case 'I': // vif num
        {
            uint16_t vif_num;
            vif_num = atoi(getopt_env.optarg);
            if (vif_num == MGMR_VIF_STA) {
                iperf_settings.vif_num = MGMR_VIF_STA;
            } else if (vif_num == MGMR_VIF_AP) {
                iperf_settings.vif_num = MGMR_VIF_AP;
            } else {
                printf(iperf_long_help);
                return; //FHOST_IPC_ERROR;
            }
            break;
        }
        case 'h': // Long Help
        {
        help:
            printf(iperf_long_help);
            return;
        }
        default:
        {
            goto help;
        }
        }
    }

    if (!client_server_set)
        goto help;

    fhost_iperf_start(&iperf_settings);
}

#define TG_USAGE0    \
 "TG command can be used for the traffic generation and reception\r\n"  \
 " First a stream must be configured with the following parameter:\r\n"   \
 " - id (< @ref FHOST_TG_MAX_TRAFFIC_STREAMS)\r\n"    \
 " - profile (@ref profile_id)\r\n"   \
 " - direction (@ref FHOST_TG_DIRECT_SEND, @ref FHOST_TG_DIRECT_RECV)\r\n"    \
 " - remote ip and port\r\n"  \
 " - local ip and port\r\n"   \
 " - rate (in pkt/s, 0 for RX)\r\n"   \
 " - packet size (in bytes, 0 for RX)\r\n"    \
 " - duration (in seconds, 0 for RX)\r\n"
#define TG_USAGE1    \
 " - tos (Type Of Service) (basically 'tid << 5', 0 for RX).\r\n" \
 " Once a stream has been configured, it can be started/stopped\r\n" \
 "   tg config <id> <profile> <direction> <remote_ip> <remote_port> <local_ip> <local_port>\r\n" \
 "             <rate> <pksize> <duration> <tos>\r\n" \
 "   tg start  <id1> [<id2> ... <id8>]\r\n" \
 "   tg stop   <id1> [<id2> ... <id8>]\r\n"

void cmd_tg(int argc, char **argv) {
    char *str = NULL;
    uint32_t i, len = 0;

    // 计算字符串总长度
    for (i = 1; i < argc; i++) {
        len += strlen(argv[i]) + 1; // 加上空格
    }

    // 分配内存
    str = rtos_malloc(1 + len * sizeof(char));
    if (!str) {
        fhost_printf("cmd_tg alloc failed\r\n");
        return;
    }

    // 连接字符串
    str[0] = '\0';
    for (i = 1; i < argc; i++) {
        strcat(str, argv[i]);
        if (i < argc - 1) {
            strcat(str, " ");
        }
    }
    //fhost_printf("tg cmd：%s\n", str);

    char *token, *next = str;

    token = fhost_ipc_next_token(&next);
    if (!strcmp("config", token))
    {
        char *id, *prof, *direction, *remote_ip, *remote_port, *local_ip,
        *local_port, *rate, *pksize, *duration, *tos;
        uint32_t stream_id;
        uint32_t rip, lip;
        char fmt[] = "Configure stream %d for %s\n";

        id = fhost_ipc_next_token(&next);
        prof = fhost_ipc_next_token(&next);
        direction = fhost_ipc_next_token(&next);
        remote_ip = fhost_ipc_next_token(&next);
        remote_port = fhost_ipc_next_token(&next);
        local_ip = fhost_ipc_next_token(&next);
        local_port = fhost_ipc_next_token(&next);
        rate = fhost_ipc_next_token(&next);
        pksize = fhost_ipc_next_token(&next);
        duration = fhost_ipc_next_token(&next);
        tos = fhost_ipc_next_token(&next);

        if (!id || !prof || !direction ||
            !remote_ip || fhost_ipc_parse_ip4(remote_ip, &rip, NULL) || !remote_port ||
            !local_ip || fhost_ipc_parse_ip4(local_ip, &lip, NULL) || !local_port ||
            !pksize || !duration || !tos) {
            //show usage
            fhost_printf("invaild args, usage:\r\n %s", TG_USAGE0);
            fhost_printf("%s", TG_USAGE1);
            goto exit;// FHOST_IPC_SHOW_USAGE;
        }

        stream_id = atoi(id);

        if (fhost_tg_config(stream_id, atoi(prof), atoi(direction), rip, atoi(remote_port),
                            lip, atoi(local_port), atoi(rate), atoi(pksize), atoi(duration),
                            atoi(tos)))
        {
            fhost_print(RTOS_TASK_NULL, "Configuration error\n");
            goto exit; // FHOST_IPC_ERROR;
        }

        if (atoi(direction) == FHOST_TG_DIRECT_SEND)
        {
            fhost_print(RTOS_TASK_NULL, fmt, stream_id, "sending\n");
        }
        else
        {
            fhost_print(RTOS_TASK_NULL, fmt, stream_id, "receiving\n");
        }
    }
    else if (!strcmp("start", token))
    {
        char *id[FHOST_TG_MAX_TRAFFIC_STREAMS + 1];
        uint32_t stream_id;
        char fmt[40] = "Start stream id : ";
        int i = 0;
        int j = 0;

        id[i] = fhost_ipc_next_token(&next);
        // If there's the first ID, try to collect all the rest
        while (id[i] != NULL && i < FHOST_TG_MAX_TRAFFIC_STREAMS)
        {
            id[++i] = fhost_ipc_next_token(&next);
        }

        if (!id[0])
        {
            fhost_printf("tg start failed %d\r\n", __LINE__);
            goto exit;
        }

        // In this loop, we count all the IDs of streams to start
        for (j = 0; j <= i - 1; j++)
        {
            stream_id = atoi(id[j]);
            if (fhost_tg_start(stream_id))
            {
                fhost_print(RTOS_TASK_NULL, "Can't start stream : %d\n", stream_id);
                goto exit;
            }
            strcat(strcat(fmt, id[j]), " ");
            // If not, ASSERT (!hal_machw_time_past(value)) at mm_timer.c:116
            rtos_task_suspend(500);
        }
        strcat(fmt, "\n");
        fhost_print(RTOS_TASK_NULL, fmt);
    }
    else if (!strcmp("stop", token))
    {
        uint32_t stream_id;
        struct fhost_tg_stats *stats = NULL;

        token = fhost_ipc_next_token(&next);
        if (!token)
        {
            fhost_printf("tg stop failed %d \r\n", __LINE__);
            goto exit;
        }

        do
        {
            stream_id = atoi(token);
            stats = fhost_tg_stop(stream_id);

            if (stats == NULL)
            {
                fhost_print(RTOS_TASK_NULL, "Can't stop stream : %d\n", stream_id);
                goto exit;
            }

            rtos_task_suspend(1);
            fhost_print(RTOS_TASK_NULL,
                        "STOP stream ID : %d\n"
                        "TX frames : %d\n"
                        "RX frames : %d\n"
                        "TX bytes : %d\n"
                        "RX bytes : %d\n",
                        stream_id,
                        stats->tx_frames,
                        stats->rx_frames,
                        stats->tx_bytes,
                        stats->rx_bytes);
            rtos_task_suspend(10);
            fhost_print(RTOS_TASK_NULL,
                        "Throughput : %d bps\n"
                        "Expected : %d\n"
                        "Lost : %d\n"
                        "Unordered : %d\n"
                        "RTT : %d us\n",
                        stats->throughput,
                        stats->expected,
                        stats->lost,
                        stats->unordered,
                        stats->rt_time);
        } while((token = fhost_ipc_next_token(&next)));
    }
    else
    {
        //show usage
        fhost_printf("invaild args, usage:\r\n %s", TG_USAGE0);
        fhost_printf("%s", TG_USAGE1);
        goto exit;
    }

exit:
    rtos_free(str);
    return;// FHOST_IPC_SUCCESS;
}

#define FHOST_PING_USAGE \
    "ping <dst_ip> [-s pksize (bytes)] [-r rate (pkt/sec)] [-d duration (sec)] [-Q tos]\r\n"    \
    "ping stop <id1> [<id2> ... <id8>]\r\n"

/**
 ****************************************************************************************
 * @brief Process function for 'ping' command
 *
 * Ping command can be used to test the reachability of a host on an IP network.
 *
   @verbatim
   ping <dst_ip> [-s pksize (bytes)] [-r rate (pkt/sec)] [-d duration (sec)] [-Q tos]
   ping stop <id1> [<id2> ... <id8>]
   @endverbatim
 *
 * Note that -s, -r, -d, -t are options for ping command. We could choose any of them to
 * configure. If not configured, it will set the default values at layer net_tg_al.
 *
 * @param[in] params ping command above
 *
 * @return 0 on success and !=0 if error occurred
 ****************************************************************************************
 */
#define IPC_MSG_ID_NULL 0
void cmd_fhost_ping(int argc, char **argv)
{
    char *str = NULL;
    uint32_t i, len = 0;

    for (i = 1; i < argc; i++) {
        len += strlen(argv[i]) + 1;
    }

    str = rtos_malloc(1 + len * sizeof(char));
    if (!str) {
        fhost_printf("cmd_fhost_ping alloc failed\r\n");
        return;
    }

    str[0] = '\0';
    for (i = 1; i < argc; i++) {
        strcat(str, argv[i]);
        if (i < argc - 1) {
            strcat(str, " ");
        }
    }
    fhost_printf("ping cmd：%s\n", str);

    char *token, *next = str;
    uint32_t rip = 0, ret;
    token = fhost_ipc_next_token(&next);
    uint32_t rate = 0, pksize = 0, duration = 0, tos = 0;
    //struct fhost_task_msg_id *task_hdl_msg_ptr;
    bool background = false;
    struct fhost_ping_task_args args;

    if (!strcmp("stop", token))
    {
        uint32_t stream_id;
        struct fhost_ping_stream* ping_stream;

        token = fhost_ipc_next_token(&next);
        if (!token)
            goto usage;

        stream_id = atoi(token);
        ping_stream = fhost_ping_find_stream_profile(stream_id);

        if (ping_stream)
        {
#if 0
            if (ping_stream->background)
            {
                task_hdl_msg_ptr = fhost_search_task_hdl_msg(ping_stream->ping_handle);
                task_hdl_msg_ptr->msg_id = IPC_MSG_ID_NULL;
                ret = FHOST_IPC_NO_RESP;
            }
#endif
            fhost_ping_stop(ping_stream);
        }
        else
        {
            fhost_print(RTOS_TASK_NULL, "Invalid stream_id %d", stream_id);
        }
        goto exit;
    }

    do
    {
        // Handle all options of ping command
        if (token[0] == '-')
        {
            switch(token[1])
            {
                case ('s'):
                    token = fhost_ipc_next_token(&next);
                    if (!token)
                        goto usage;
                    pksize = atoi(token);
                    break;
                case ('r'):
                    token = fhost_ipc_next_token(&next);
                    if (!token)
                        goto usage;
                    rate = atoi(token);
                    break;
                case ('d'):
                    token = fhost_ipc_next_token(&next);
                    if (!token)
                        goto usage;
                    duration = atoi(token);
                    break;
                case ('Q'):
                    token = fhost_ipc_next_token(&next);
                    if (!token)
                        goto usage;
                    tos = atoi(token);
                    break;
                case ('G'):
                    //background = true;
                    fhost_printf("Not supported\r\n");
                    break;
                default:
                    goto usage;
            }
        }
        // If it's neither options, nor IP address, then the input is wrong
        else if (fhost_ipc_parse_ip4(token, &rip, NULL))
        {
            fhost_print(RTOS_TASK_NULL, "Invalid IP address: %s\n", token);
            goto exit;
        }
    } while ((token = fhost_ipc_next_token(&next)));

    // IP destination should be set by the command
    if (rip == 0)
        goto usage;

    args.rip = rip;
    args.rate = rate;
    args.pksize = pksize;
    args.duration = duration;
    args.tos = tos;
    args.background = background;
    ret = fhost_ipc_start_task(IPC_MSG_ID_NULL, fhost_ping_start, &args, fhost_ping_sigkill_handler);

    if (ret == FHOST_IPC_ERROR)
    {
        fhost_print(RTOS_TASK_NULL, "Send ping error\n");
    }
    goto exit;

usage:
    fhost_printf("%s", FHOST_PING_USAGE);

exit:
    rtos_free(str);
    return;
}

#define SET_IPV4_USAGE                    \
    "set_ipv4 [ip] [dns] [gw] [mask]\r\n"   \
    "\t ip: set static ip\r\n" \
    "\t dns: set local host dns\r\n" \
    "\t gw: set local host gate way. default is 255.255.255.0\r\n" \
    "\t mask: set local host mask. default is 255.255.255.0\r\n"

void wifi_sta_static_ipv4(int argc, char **argv)
{
    char *addr, *mask, *gw, *dns;
    if (argc < 3) {
        printf("%s", SET_IPV4_USAGE);
        return;
    }

    /* ip addr */
    if (argc > 1) {
        addr = argv[1];
    } else {
        addr = "127.0.0.1";
    }

    /* ip dns */
    if (argc > 2) {
        dns = argv[2];
    } else {
        dns = "0.0.0.0";
    }

    /* ip gw */
    if (argc > 3) {
        gw = argv[3];
    } else {
        gw = "255.255.255.0";
    }
    /* ip mask */
    if (argc > 4) {
        mask = argv[4];
    } else {
        mask = "255.255.255.0";
    }
    printf("addr:%s,mask:%s,gw:%s,dns:%s\r\n", addr, mask, gw, dns);

    wifi_mgmr_sta_ip_set(inet_addr(addr), inet_addr(mask), inet_addr(gw), inet_addr(dns));

    return;

}

static void _print_sta_pwr(uint8_t numb, int8_t *power_table)
{
    uint8_t i;
    for (i = 0; i<numb; i++) {
        printf("%3u ", power_table[i]);
    }
    printf("\r\n");
}

void wifi_sta_info_cmd(int argc, char **argv)
{
    ip4_addr_t addr, mask, gw, dns;
    int rssi = 0;
    rf_pwr_table_t power_table;
    char country_code[3] = {0};
    int channel_nums = 0;

    memset(&addr, 0, sizeof(ip4_addr_t));
    memset(&mask, 0, sizeof(ip4_addr_t));
    memset(&gw, 0, sizeof(ip4_addr_t));
    memset(&dns, 0, sizeof(ip4_addr_t));
    memset(power_table.pwr_11b, 0, sizeof(rf_pwr_table_t));

    wifi_mgmr_get_country_code(country_code);
    channel_nums = wifi_mgmr_get_channel_nums(country_code);
    wifi_sta_ip4_addr_get(&addr.addr, &mask.addr, &gw.addr, &dns.addr);
    wifi_mgmr_sta_rssi_get(&rssi);
    wifi_mgmr_tpc_pwr_get(&power_table);
    fhost_print(RTOS_TASK_NULL, "================================================================\r\n");
    fhost_print(RTOS_TASK_NULL, "RSSI:    %ddbm\r\n", rssi);
    fhost_print(RTOS_TASK_NULL, "IP  :    %s \r\n", ip4addr_ntoa(&addr));
    fhost_print(RTOS_TASK_NULL, "MASK:    %s \r\n", ip4addr_ntoa(&mask));
    fhost_print(RTOS_TASK_NULL, "GW  :    %s \r\n", ip4addr_ntoa(&gw));
    fhost_print(RTOS_TASK_NULL, "DNS :    %s \r\n", ip4addr_ntoa(&dns));
    fhost_print(RTOS_TASK_NULL, "Country: %s \r\n", country_code);
    fhost_print(RTOS_TASK_NULL, "Channel: %d \r\n", channel_nums);
    #ifdef CFG_IPV6
    struct netif *nif;

    NETIF_FOREACH(nif) {
        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++)
        {
            const ip6_addr_t * ip6addr = netif_ip6_addr(nif, i);
            if (!ip6_addr_isany(ip6addr))
            {
                fhost_print(RTOS_TASK_NULL, "IPv6:    %s, state %x\r\n", ip6addr_ntoa(ip6addr), netif_ip6_addr_state(nif, i));
            }
        }
    }
    #endif
    fhost_print(RTOS_TASK_NULL,   "Power Table (dbm):\r\n");
    fhost_print(RTOS_TASK_NULL,   "-----------------------------------------------------------\r\n");
    fhost_print(RTOS_TASK_NULL, "  11b:         ");
    _print_sta_pwr(4,  power_table.pwr_11b);
    fhost_print(RTOS_TASK_NULL, "  11g:         ");
    _print_sta_pwr(8,  power_table.pwr_11g);
    fhost_print(RTOS_TASK_NULL, "  11n(ht20):   ");
    _print_sta_pwr(8,  power_table.pwr_11n_ht20);
    fhost_print(RTOS_TASK_NULL, "  11n(ht40):   ");
    _print_sta_pwr(8,  power_table.pwr_11n_ht40);
    fhost_print(RTOS_TASK_NULL, "  11ac(vht20): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht20);
    fhost_print(RTOS_TASK_NULL, "  11ac(vht40): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht40);
#if 0
    fhost_print(RTOS_TASK_NULL, "  11ac(vht80): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht80);
#endif
    fhost_print(RTOS_TASK_NULL, "  11ax(he20):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he20);
    fhost_print(RTOS_TASK_NULL, "  11ax(he40):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he40);
#if 0
    fhost_print(RTOS_TASK_NULL, "  11ax(he80):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he80);
    fhost_print(RTOS_TASK_NULL, "  11ax(he160): ");
    _print_sta_pwr(12, power_table.pwr_11ax_he160);
#endif
    fhost_print(RTOS_TASK_NULL, "================================================================\r\n");
}

/* DEBUG: drive ALL per-rate TX-power targets to <dbm> through the real per-rate
 * path (wifi_mgmr_tpc_pwr_set -> bl_tpc_update_power_table -> pwrtarget +
 * wl_wlan_power_table_update). Lets us test whether provisioning the per-rate
 * table (not a forced gain) restores uplink power, and expose any pwrlim clamp.
 * NOT for production. Read back with `wifi_sta_info`. */
void wifi_tpc_pwr_set_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: wifi_tpc_pwr_set <dbm>   (fills every per-rate target, applied via wifi_mgmr_tpc_pwr_set)\r\n");
        return;
    }
    int8_t dbm = (int8_t)atoi(argv[1]);
    rf_pwr_table_t t;
    memset(&t, dbm, sizeof(t));   /* every int8_t entry (incl reserved) = dbm */
    wifi_mgmr_tpc_pwr_set(&t);
    printf("wifi_tpc_pwr_set: all per-rate targets = %d dBm applied\r\n", (int)dbm);
}

void wifi_mgmr_ap_start_cmd(int argc, char **argv)
{
    getopt_env_t getopt_env;
    int opt;
    wifi_mgmr_ap_params_t config;

    if (argc < 2) {
        goto _ERROUT;
    }

    memset(&config, 0, sizeof(config));

    // default use_ipcfg use_dhcpd
    config.use_ipcfg = true;
    config.use_dhcpd = true;

    utils_al_getopt_init(&getopt_env, 0);
    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "b:s:k:c:a:t:h:i:g:d:I:S:L:n:")) != -1) {
        switch (opt) {
	case 'b':
	    config.type = (uint8_t)atoi(getopt_env.optarg);
	    break;

	case 's':
	    config.ssid = getopt_env.optarg;
	    break;

	case 'k':
	    config.key = getopt_env.optarg;
	    break;

	case 'c':
	    config.channel = (uint8_t)atoi(getopt_env.optarg);
	    break;

	case 'a':
	    config.akm = getopt_env.optarg;
	    break;

    case 't':
	    config.ap_max_inactivity = (uint32_t)atoi(getopt_env.optarg);
	    break;

    case 'h':
	    config.hidden_ssid = (uint32_t)atoi(getopt_env.optarg);
	    break;

    case 'i':
	    config.isolation = (uint32_t)atoi(getopt_env.optarg);
	    break;
	case 'g':
	    config.use_ipcfg = atoi(getopt_env.optarg);
	    break;
	case 'd':
	    config.use_dhcpd = atoi(getopt_env.optarg);
	    break;
	case 'I':
	    config.ap_ipaddr = inet_addr(getopt_env.optarg);
	    config.ap_mask = inet_addr("255.255.255.0");
            if((config.ap_ipaddr & 0xff000000)  != 0x01000000)
                printf("ap mode ipaddr is not x.x.x.1 \r\n");
	    break;

    case 'S':
	    config.start = atoi(getopt_env.optarg);
	    break;

	case 'L':
	    config.limit = atoi(getopt_env.optarg);
	    break;

    case 'n':
        config.bcn_interval = atoi(getopt_env.optarg);
        break;

	case '?':
	    printf("unknow option: %c \r\n", getopt_env.optopt);
	    goto _ERROUT;

        }
    }

    if (config.ssid == NULL) {
        goto _ERROUT;
    }

    wifi_mgmr_ap_start(&config);

    return;

 _ERROUT:
    printf("[USAGE]: %s -s <ssid> [-k <key>] [-c <channel>] [-a <akm>] [-I <ipv4_addr>] [-S <dhcpd_start>] [-L <dhcpd_limit>] [-n <bcn_interval>]\r\n", argv[0]);
    return;
}

void wifi_mgmr_ap_stop_cmd(int argc, char **argv)
{
    wifi_mgmr_ap_stop();
}

#if WIFI_STATISTIC_ENABLE
void cmd_fw_dbg(int argc, char **argv)
{
    struct cfgrwnx_me_param cmd;
    struct cfgrwnx_me_param_resp resp;
    struct fhost_cntrl_link *fw_dbg;

    uint32_t param_tx_ampdu_retry_cnt_limit;
    uint32_t param_tx_ampdu_protect_enable;
    uint32_t param_tx_ampdu_drop_to_singleton_retrycnt_threshold;

    uint16_t id;
    uint32_t value;

    if ((1 != argc) && (2 != argc) && (4 != argc)) {
        fhost_printf("fw_dbg get|set [id:[%d:%d]] [value]\r\n",
                CFGRWNX_ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT,
                CFGRWNX_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD);
        goto cmd_fw_dbg_err;
    }

#if 0
    fw_dbg = fhost_cntrl_cfgrwnx_link_open();
    if (NULL == fw_dbg) {
        fhost_printf("fw_dbg NULL\r\n");
        return;
    }
#else
    fw_dbg = cntrl_link;
#endif

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_ME_PARAM_CMD;
    cmd.hdr.resp_queue = fw_dbg->queue;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_ME_PARAM_RESP;

    if ((4 == argc) && (strncmp(argv[1], "set", 3) == 0)) {
        id = atoi(argv[2]);
        if (id > CFGRWNX_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD) {
            goto cmd_fw_dbg_err;
        }
        value = atoi(argv[3]);
        cmd.id  = id;
        cmd.cmd = CFGRWNX_ME_PARAM_CMD_SET;
        memcpy(cmd.data, &value, sizeof(uint32_t));

        fhost_printf("set id:%d, cmd:%d, value:%d\r\n", id, cmd, value);
        if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr)) {
            fhost_printf("fhost_cntrl_cfgrwnx_cmd_send error\r\n");
            goto cmd_fw_dbg_err;
        }
    } else if ((2 == argc) && (strncmp(argv[1], "get", 3) == 0)) {
        // get
        cmd.id  = CFGRWNX_ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT;
        cmd.cmd = CFGRWNX_ME_PARAM_CMD_GET;
        if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr)) {
            fhost_printf("fhost_cntrl_cfgrwnx_cmd_send error\r\n");
            goto cmd_fw_dbg_err;
        }
        memcpy(&param_tx_ampdu_retry_cnt_limit, resp.data, sizeof(uint32_t));
        fhost_printf("param_tx_ampdu_retry_cnt_limit                      = %d\r\n", param_tx_ampdu_retry_cnt_limit);

        // get
        cmd.id  = CFGRWNX_ME_PARAM_ID_TX_AMPDU_PROTECT_ENABLE;
        cmd.cmd = CFGRWNX_ME_PARAM_CMD_GET;
        if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr)) {
            fhost_printf("fhost_cntrl_cfgrwnx_cmd_send error\r\n");
            goto cmd_fw_dbg_err;
        }
        memcpy(&param_tx_ampdu_protect_enable, resp.data, sizeof(uint32_t));
        fhost_printf("param_tx_ampdu_protect_enable                       = %d\r\n", param_tx_ampdu_protect_enable);

        // get
        cmd.id  = CFGRWNX_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD;
        cmd.cmd = CFGRWNX_ME_PARAM_CMD_GET;
        if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr)) {
            fhost_printf("fhost_cntrl_cfgrwnx_cmd_send error\r\n");
            goto cmd_fw_dbg_err;
        }
        memcpy(&param_tx_ampdu_drop_to_singleton_retrycnt_threshold , resp.data, sizeof(uint32_t));
        fhost_printf("param_tx_ampdu_drop_to_singleton_retrycnt_threshold = %d\r\n", param_tx_ampdu_drop_to_singleton_retrycnt_threshold);

        // fhost_statistic
        fhost_printf("tx_ampdu_retry_limit_count = %d\r\n", fhost_statistic.tx_ampdu_retry_limit_count);
        fhost_printf("tx_ampdu_lft_expired_count = %d\r\n", fhost_statistic.tx_ampdu_lft_expired_count);
        fhost_printf("tx_ampdu_sw_retry          = %d\r\n", fhost_statistic.tx_ampdu_sw_retry);
        fhost_printf("tx_ampdu_unvalid_ba        = %d\r\n", fhost_statistic.tx_ampdu_unvalid_ba);

        // Phase 2 instrumentation
        fhost_printf("--- Phase 2 counters ---\r\n");
        // 1. Confirmation latency (us)
        fhost_printf("cfm_lat avg=%lu max=%lu cnt=%lu\r\n",
            fhost_statistic.cfm_latency_cnt ?
                fhost_statistic.cfm_latency_sum / fhost_statistic.cfm_latency_cnt : 0,
            fhost_statistic.cfm_latency_max,
            fhost_statistic.cfm_latency_cnt);
        // 2. AMPDU close source
        // The first four tokens keep their name, order and position so the bench
        // scripts' `agg_close macif=.. cfm=.. fexch=.. push=..` regex still matches;
        // the site-attributed sources are appended. See RC_P9_0809.md.
        fhost_printf("agg_close macif=%lu cfm=%lu fexch=%lu push=%lu"
                     " maxcnt=%lu maxlen=%lu incompat=%lu timer=%lu incsta=%lu\r\n",
            fhost_statistic.agg_close_macif,
            fhost_statistic.agg_close_cfm,
            fhost_statistic.agg_close_fexch,
            fhost_statistic.agg_close_push,
            fhost_statistic.agg_close_maxcnt,
            fhost_statistic.agg_close_maxlen,
            fhost_statistic.agg_close_incompat,
            fhost_statistic.agg_close_timer,
            fhost_statistic.agg_close_incsta);
        // 3. DMA idle ratio
        // New LINE, not an edit of agg_close - the bench regexes key on that one.
    fhost_printf("rc_trial set=%lu first=%lu\r\n",
        fhost_statistic.rc_trial_set, fhost_statistic.rc_trial_first);
    fhost_printf("dma newtail=%lu newhead=%lu lfe_stale=%lu\r\n",
            fhost_statistic.dma_newtail_cnt,
            fhost_statistic.dma_newhead_cnt,
            fhost_statistic.lfe_stale_cnt);
        // 4. AMPDU size histogram
        fhost_printf("ampdu_hist 1=%lu 2-4=%lu 5-8=%lu 9-16=%lu 17+=%lu\r\n",
            fhost_statistic.ampdu_hist_1,
            fhost_statistic.ampdu_hist_2_4,
            fhost_statistic.ampdu_hist_5_8,
            fhost_statistic.ampdu_hist_9_16,
            fhost_statistic.ampdu_hist_17p);
        // 5. TX push flow
        fhost_printf("--- TX push flow ---\r\n");
        fhost_printf("macif_tx_evt=%lu pushed=%lu data_ind=%lu\r\n",
            fhost_statistic.macif_tx_evt_cnt,
            fhost_statistic.macif_tx_evt_pushed,
            fhost_statistic.macif_tx_data_ind_cnt);
        fhost_printf("tx_sched=%lu sched_pushed=%lu cfm=%lu\r\n",
            fhost_statistic.fhost_tx_sched_cnt,
            fhost_statistic.fhost_tx_sched_pushed,
            fhost_statistic.fhost_tx_cfm_cnt);
        // 6. TCP RX diagnostics
        fhost_printf("--- TCP RX diag ---\r\n");
        fhost_printf("tx_preempt=%lu single_push=%lu ppdu_leak_reset=%lu\r\n",
            fhost_statistic.macif_tx_preempt_cnt,
            fhost_statistic.macif_tx_single_push,
            fhost_statistic.ppdu_leak_reset_cnt);
        fhost_printf("agg_check ppdu0=%lu ppduN=%lu\r\n",
            fhost_statistic.agg_check_ppdu0,
            fhost_statistic.agg_check_ppduN);
        // 7. Pipeline depth
        fhost_printf("ppdu_cnt max=%lu avg=%lu samples=%lu\r\n",
            fhost_statistic.ppdu_cnt_max,
            fhost_statistic.ppdu_cnt_samples ?
                fhost_statistic.ppdu_cnt_sum / fhost_statistic.ppdu_cnt_samples : 0,
            fhost_statistic.ppdu_cnt_samples);
    } else {
        goto cmd_fw_dbg_err;
    }

#if 0
    fhost_cntrl_cfgrwnx_link_close(fw_dbg);
#endif

    return;
cmd_fw_dbg_err:
    fhost_printf("fw_dbg get|set [id:[%d:%d]] [value]\r\n",
            CFGRWNX_ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT,
            CFGRWNX_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD);
}
#endif

void cmd_ntr(int argc, char **argv)
{
    extern void net_al_trace_print(void);
    net_al_trace_print();
}

/* DEBUG (agg-hold lever): tune the A-MPDU hold-down (txl_agg_hold_*, defaults
 * skip_n=8/max_cnt=32/us=1000). Test whether a fatter hold window raises the
 * on-air A-MPDU depth beyond ~1 on the full-queue SDIO path, or whether depth
 * stays 1 (push-rate-limited = platform floor). Usage: agghold [skip_n max_cnt us] */
void cmd_agghold(int argc, char **argv)
{
    extern volatile uint32_t txl_agg_hold_skip_n, txl_agg_hold_us;
    extern volatile uint8_t  txl_agg_hold_max_cnt;
    if (argc >= 4) {
        txl_agg_hold_skip_n  = (uint32_t)atoi(argv[1]);
        txl_agg_hold_max_cnt = (uint8_t)atoi(argv[2]);
        txl_agg_hold_us      = (uint32_t)atoi(argv[3]);
    }
    fhost_printf("agg_hold skip_n=%lu max_cnt=%u us=%lu\r\n",
        (unsigned long)txl_agg_hold_skip_n, (unsigned)txl_agg_hold_max_cnt,
        (unsigned long)txl_agg_hold_us);
}

/* DEBUG (P15 A/B): move A-MPDU descriptors between the free pool and the reserve.
 *
 * `txstat`'s `inc sites: 5=` counts push_pool_empty - the frame that found no free
 * A-MPDU descriptor, was demoted to TX_AGG_MPDU_NONE and went out alone. It is 13.65 %
 * of closes free-running against 0.02 % pinned over 82 legs, so the pool depth is the
 * lever this A/B moves. The reserve is empty unless the image was built with
 * TX_AGG_POOL_EXTRA > 0, in which case `aggpool` is a no-op that still prints the depth.
 *
 * The achieved depth is PRINTED rather than assumed: descriptors in flight are in
 * neither list, so a set issued under load lands short. Set it between legs and gate on
 * the echo, the same discipline `agghold` needs.
 *
 * Usage: aggpool [queue depth] */
void cmd_aggpool(int argc, char **argv)
{
    extern int txl_agg_pool_set(uint8_t queue, int target, int *reserve);
    extern int txl_agg_pool_phys(uint8_t queue);
    if (argc >= 3) {
        int q = atoi(argv[1]);
        int got = txl_agg_pool_set((uint8_t)q, atoi(argv[2]), NULL);
        if (got < 0) {
            fhost_printf("aggpool: queue %d out of range\r\n", q);
            return;
        }
    }
    fhost_printf("aggpool");
    for (int q = 0; ; q++) {
        int rsv = 0;
        int got = txl_agg_pool_set((uint8_t)q, 0, &rsv);
        if (got < 0)
            break;
        fhost_printf(" q%d=%d+%d/%d", q, got, rsv, txl_agg_pool_phys((uint8_t)q));
    }
    fhost_printf("\r\n");
}

/* DEBUG (P27 A/B): retry budget for an RX descriptor whose RBD contents are not there.
 *
 * `txstat`'s `rxl_ring embed_bad=` counts descriptors refused with a CORRECT `upatternrx`
 * but a `datastartptr` / `dataendptr` outside both RX buffers. Measured 2026-08-10:
 * `first_pbd_ptr == buf->read + sizeof(struct rxdesc)` exactly in 5 of 5 well-posed
 * events, so the header descriptor is right and only the slot it points at is still
 * empty - the RBD had not been written when SW walked in. Skipping such a descriptor
 * (`buf->read = hw_wr`) throws away a frame that was about to become valid.
 *
 * n = how many EXTRA looks it gets, one per RX event, before the ring is re-synced past
 * it anyway. n=0 is the pre-2026-08-10 behaviour exactly and is the control arm.
 * `healed` vs `gaveup` in `txstat` then answers it from behaviour rather than from the
 * shape of a pointer.
 *
 * Global, and a reboot reverts it to the default - re-apply per leg and gate on the echo,
 * the same discipline `agghold` and `aggpool` need.
 *
 * Usage: rxhold [n]   (n clamped to [0..64]) */
void cmd_rxhold(int argc, char **argv)
{
    extern volatile uint8_t macsw_rxl_desc_retry_max;

    if (argc >= 2) {
        int n = atoi(argv[1]);
        if (n < 0)  n = 0;
        if (n > 64) n = 64;
        macsw_rxl_desc_retry_max = (uint8_t)n;
    }
    fhost_printf("rxhold retry_max=%u\r\n", (unsigned)macsw_rxl_desc_retry_max);
}

/* DEBUG (F1 chain-behind lever): tune the A-MPDU close-gate pipeline depth
 * budget (txl_agg_pipeline_max, default 2). 1 = legacy one-A-MPDU-in-flight,
 * 2 = allow chaining the next A-MPDU behind one in-flight exchange.
 * Usage: aggpipe [n]  (n clamped to [1..4]) */
void cmd_aggpipe(int argc, char **argv)
{
    extern volatile uint32_t txl_agg_pipeline_max;
    if (argc >= 2) {
        int n = atoi(argv[1]);
        if (n < 1) n = 1;
        if (n > 4) n = 4;
        txl_agg_pipeline_max = (uint32_t)n;
    }
    fhost_printf("agg_pipeline_max=%lu\r\n",
        (unsigned long)txl_agg_pipeline_max);
}

/* Link governor: one knob that switches the WHOLE joint policy set, which is the
 * only way an A/B on it means anything. `lgov 0` is not "governor in a neutral
 * setting", it is every hook returning its caller's pre-governor value.
 *
 * Usage:
 *   lgov                     show state, sensors, policy table
 *   lgov 0 | 1               master A/B switch (default 0)
 *   lgov force clean|noisy|busy|auto     pin a state (reverse validation)
 *   lgov set <state> <floor%> <cnt> <prot>   edit one policy row live
 *   lgov thr <ack_noisy> <ack_clean> <fcs/s> <cca%>   move the classifier
 *
 * All of it is idle-safe to write: the policy is read when the next A-MPDU is built,
 * not latched into the policy table, so unlike fix_mcs / wifi_tpc_pwr_set this does
 * not have to wait for the link to go quiet. */
void cmd_lgov(int argc, char **argv)
{
    /* Parsing and printing live in macsw/ip/lmac/src/rc/lgov.c, next to the policy
     * structures it edits - those are private macsw types and have no business
     * crossing into the lwip adapter. Same shape as the other macsw knobs here. */
    extern void lgov_cmd(int argc, char **argv);

    lgov_cmd(argc, argv);
}

/* fix_mcs now lives in wifi_mgmr_cli.c (upstream 853541565). Our local copy
 * was removed on the 07-25 rebase: both defined a non-static cmd_fix_mcs, so
 * keeping both is a duplicate symbol at link time. The two are functionally
 * equivalent - same `fix_mcs {off|0-9 [gi]}` syntax and the same rc_rate_bf
 * encoding (HE-SU format 5, MCS<<4, NSS1, GI<<11, 20MHz) - so every bench
 * script keeps working unchanged. */

/* DEBUG: dump the live rate-control sample table for the connected STA. */
void cmd_rc_dump(int argc, char **argv)
{
    extern void rc_tx_dump_stats(void);
    (void)argc;
    (void)argv;
    rc_tx_dump_stats();
}

/* Rate-controller anti-thrash knobs, both default off so this is a knob flip A/B out of
 * one build rather than two images:
 *   rc_tune hyst <pct>      step-0 hysteresis margin
 *   rc_tune keeptop <n>     rank-protected top slots instead of the absolute 50% bar
 * Parsing lives in macsw/ip/lmac/src/rc/rc.c next to the variables, like lgov above. */
void cmd_rc_tune(int argc, char **argv)
{
    extern void rc_tune_cmd(int argc, char **argv);

    rc_tune_cmd(argc, argv);
}

/* DEBUG: A-MSDU TX batching tuner + stats (ported from ipro6le_demo main.c,
 * Phase 10P Path A). `amsdu en 1` opt-in; stats include the per-condition
 * eligibility rejection counters to diagnose non-engagement on bridge traffic. */
void cmd_amsdu(int argc, char **argv)
{
    extern volatile uint32_t fhost_amsdu_timeout_us;
    extern volatile uint8_t  fhost_amsdu_max_batch_run;
    extern volatile uint8_t  fhost_amsdu_enabled;
    extern volatile uint32_t fhost_amsdu_committed;
    extern volatile uint32_t fhost_amsdu_size_hist[];
    extern volatile uint32_t fhost_amsdu_close_full;
    extern volatile uint32_t fhost_amsdu_close_timer;
    extern volatile uint32_t fhost_amsdu_drop;
    extern volatile uint32_t fhost_amsdu_release_cnt;
    extern volatile uint32_t fhost_amsdu_rej_disabled;
    extern volatile uint32_t fhost_amsdu_rej_type;
    extern volatile uint32_t fhost_amsdu_rej_flags;
    extern volatile uint32_t fhost_amsdu_rej_tid;
    extern volatile uint32_t fhost_amsdu_rej_sta;
    extern volatile uint32_t fhost_amsdu_rej_vif;
    extern volatile uint32_t fhost_amsdu_rej_eth;
    extern volatile uint32_t fhost_amsdu_rej_busy;
    extern volatile uint32_t fhost_amsdu_rej_full;
    extern volatile uint32_t fhost_amsdu_rej_size;
    extern volatile uint32_t fhost_amsdu_pass;

    extern volatile uint32_t fhost_amsdu_payl_max;

    if (argc >= 3 && !strcmp(argv[1], "en"))
        fhost_amsdu_enabled = (uint8_t)(atoi(argv[2]) != 0);
    else if (argc >= 3 && !strcmp(argv[1], "max"))
        fhost_amsdu_max_batch_run = (uint8_t)atoi(argv[2]);
    else if (argc >= 3 && !strcmp(argv[1], "to"))
        fhost_amsdu_timeout_us = (uint32_t)atoi(argv[2]);
    else if (argc >= 3 && !strcmp(argv[1], "payl"))
        fhost_amsdu_payl_max = (uint32_t)atoi(argv[2]);
    printf("  payl_max=%lu\r\n", (unsigned long)fhost_amsdu_payl_max);
    printf("amsdu en=%u max=%u to=%luus committed=%lu drop=%lu rel=%lu cl_full=%lu cl_to=%lu\r\n",
        (unsigned)fhost_amsdu_enabled,
        (unsigned)fhost_amsdu_max_batch_run,
        (unsigned long)fhost_amsdu_timeout_us,
        (unsigned long)fhost_amsdu_committed,
        (unsigned long)fhost_amsdu_drop,
        (unsigned long)fhost_amsdu_release_cnt,
        (unsigned long)fhost_amsdu_close_full,
        (unsigned long)fhost_amsdu_close_timer);
    printf("  hist[1..4]=%lu %lu %lu %lu pass=%lu\r\n",
        (unsigned long)fhost_amsdu_size_hist[1],
        (unsigned long)fhost_amsdu_size_hist[2],
        (unsigned long)fhost_amsdu_size_hist[3],
        (unsigned long)fhost_amsdu_size_hist[4],
        (unsigned long)fhost_amsdu_pass);
    printf("  rej dis=%lu typ=%lu flg=%lu tid=%lu sta=%lu vif=%lu eth=%lu busy=%lu full=%lu size=%lu\r\n",
        (unsigned long)fhost_amsdu_rej_disabled,
        (unsigned long)fhost_amsdu_rej_type,
        (unsigned long)fhost_amsdu_rej_flags,
        (unsigned long)fhost_amsdu_rej_tid,
        (unsigned long)fhost_amsdu_rej_sta,
        (unsigned long)fhost_amsdu_rej_vif,
        (unsigned long)fhost_amsdu_rej_eth,
        (unsigned long)fhost_amsdu_rej_busy,
        (unsigned long)fhost_amsdu_rej_full,
        (unsigned long)fhost_amsdu_rej_size);
}

#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
/* DEBUG 2026-07-23: unified TX-path profiler dump. `txprof` prints the three
 * existing per-frame instrumentations end-to-end; `txprof r` resets them.
 *  - rxprof:   SDIO worker inject path (sd_cb/dispatch/alloc/memcpy/wifi_tx)
 *  - tx_trace: fhost_tx_start lock-wait vs body split + call intervals
 *  - txu prof: WiFi-task push split (umacdesc_prep+bam / frame_build / lmac) */
void cmd_txprof(int argc, char **argv)
{
    extern void rxprof_print(void);
    extern void rxprof_reset(void);
    extern void fhost_tx_trace_print(void);
    extern void fhost_tx_trace_reset(void);
    extern void txu_cntrl_push_prof_print(void);
    extern void txu_cntrl_push_prof_reset(void);
    extern void txl_push_prof_print(void);
    extern void txl_push_prof_reset(void);
    extern void macif_tx_evt_prof_print(void);
    extern void macif_tx_evt_prof_reset(void);
    extern void txl_cfm_prof_print(void);
    extern void txl_cfm_prof_reset(void);

    if (argc >= 2 && argv[1][0] == 'r') {
        rxprof_reset();
        fhost_tx_trace_reset();
        txu_cntrl_push_prof_reset();
        txl_push_prof_reset();
        macif_tx_evt_prof_reset();
        txl_cfm_prof_reset();
        return;
    }
    rxprof_print();
    fhost_tx_trace_print();
    txu_cntrl_push_prof_print();
    txl_push_prof_print();
    macif_tx_evt_prof_print();
    txl_cfm_prof_print();
}
#endif /* CONFIG_COMPONENTS_SDIOWIFI_ENABLE */

/* DEBUG 2026-07-23: 32-bit memory poke for runtime A/B toggles (e.g.
 * g_sdio_credit_scratch_en, fhost_tx_sched_batch_en). poke <hex_addr> <hex_val>
 * with optional width suffix: pokeb writes one byte. Read back via `p`. */
void cmd_poke(int argc, char **argv)
{
    uint32_t addr, val;

    /* One argument reads instead of writing. The MAC's own status registers
     * are the only place it says why it refused a descriptor, and the AC
     * watchdog dump prints them ONLY on a timeout -- so a wedged reading had
     * nothing healthy to be compared against. DMA_STATUS_2 in particular is
     * HW-write/SW-read and latches, so "bit set" means it happened at some
     * point since the last MAC reset, not that it is happening now; without a
     * healthy sample the bit cannot be interpreted at all. */
    if (argc == 2) {
        addr = (uint32_t)strtoul(argv[1], NULL, 16);
        printf("rd 0x%08lx = 0x%08lx\r\n", (unsigned long)addr,
               (unsigned long)*(volatile uint32_t *)addr);
        return;
    }
    if (argc < 3) {
        printf("usage: poke <hex_addr> [<hex_val>]   (one arg reads)\r\n");
        return;
    }
    addr = (uint32_t)strtoul(argv[1], NULL, 16);
    val = (uint32_t)strtoul(argv[2], NULL, 16);
    if (argv[0][4] == 'b') {
        *(volatile uint8_t *)addr = (uint8_t)val;
        printf("pokeb 0x%08lx <- 0x%02lx\r\n", (unsigned long)addr, (unsigned long)(val & 0xFF));
    } else {
        *(volatile uint32_t *)addr = val;
        printf("poke 0x%08lx <- 0x%08lx\r\n", (unsigned long)addr, (unsigned long)val);
    }
}

#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
/* DEBUG 2026-07-23 T1: gated SDIO->WiFi inject-copy mode (g_sdio_dmacpy_mode).
 *   dmacpy            show counters
 *   dmacpy r          reset counters
 *   dmacpy test       init + self-test (legacy vs cached-read vs DMA)
 *   dmacpy 0|1|2      set mode 0 legacy / 1 DMA-overlap / 2 cached-read
 * Mode 1 needs the DMA0 channel; init runs first and the switch is refused if
 * init fails. Symbols live in the sdiowifi component (extern, like wprio). */
void cmd_dmacpy(int argc, char **argv)
{
    extern volatile uint32_t g_sdio_dmacpy_mode;
    extern int  sdio_dmacpy_init(void);
    extern void sdio_dmacpy_selftest(void);
    extern void sdio_dmacpy_stats(int reset);
    extern void sdio_dmacpy_set_burst(uint32_t b);
    extern void sdio_dmacpy_set_width(uint32_t w);
    uint32_t mode;

    if (argc < 2) {
        sdio_dmacpy_stats(0);
        return;
    }
    if (argv[1][0] == 'b' && argc >= 3) {
        sdio_dmacpy_set_burst((uint32_t)strtoul(argv[2], NULL, 0));
        return;
    }
    if (argv[1][0] == 'w' && argc >= 3) {
        sdio_dmacpy_set_width((uint32_t)strtoul(argv[2], NULL, 0));
        return;
    }
    if (argv[1][0] == 'r') {
        sdio_dmacpy_stats(1);
        return;
    }
    if (argv[1][0] == 't') {
        if (sdio_dmacpy_init() != 0) {
            printf("dmacpy: init failed\r\n");
            return;
        }
        sdio_dmacpy_selftest();
        return;
    }

    mode = (uint32_t)strtoul(argv[1], NULL, 0);
    if (mode > 2) {
        printf("usage: dmacpy [r|test|0|1|2]\r\n");
        return;
    }
    if (mode == 1 && sdio_dmacpy_init() != 0) {
        printf("dmacpy: init failed, staying mode %lu\r\n",
               (unsigned long)g_sdio_dmacpy_mode);
        return;
    }
    g_sdio_dmacpy_mode = mode;
    printf("dmacpy mode -> %lu (0 legacy / 1 DMA / 2 cached-read)\r\n",
           (unsigned long)mode);
}

/* DEBUG 2026-07-23 F6: runtime SMID worker priority override for A/B.
 * Default 31 outranks every WiFi data task (fhost_tx 29, rx 28, wifi 27);
 * after F5 the worker's admitted-inject bursts starve cfm/push/close (t6
 * ring trace: 6ms cfm-B starvation). wprio 26 drops it below all of them. */
void cmd_smid_wprio(int argc, char **argv)
{
    extern void smid_worker_prio_set(uint32_t prio);

    if (argc < 2) {
        printf("usage: wprio <prio 1..31> (default 31; try 26)\r\n");
        return;
    }
    uint32_t p = (uint32_t)strtoul(argv[1], NULL, 0);
    smid_worker_prio_set(p);
    printf("smid_worker prio -> %lu\r\n", (unsigned long)p);
}

/* DEBUG 2026-07-24 Phase-Z v2b: bench-side zero-copy opt-in (same effect as the
 * host's in-band USER_EXT "ZCEN1"). ONLY safe when the host is already in
 * single-frame TX mode (ipronet tx_agg=0 or zc mode) — arming the 1604-capped
 * slots under an aggregating host reproduces the -110 card-removed cascade. */
void cmd_sdio_zc(int argc, char **argv)
{
    extern int smid_zc_host_enable(void);
    extern volatile uint32_t g_sdio_zerocopy;

    if (argc >= 2 && argv[1][0] == '1') {
        int armed = smid_zc_host_enable();
        printf("sdio_zc: armed %d slots, gate=1\r\n", armed);
    } else {
        printf("sdio_zc: gate=%lu (usage: sdio_zc 1 — host MUST be single-frame)\r\n",
               (unsigned long)g_sdio_zerocopy);
    }
}
#endif /* CONFIG_COMPONENTS_SDIOWIFI_ENABLE */

/* DEBUG 2026-07-23 F2b: view/override per-AC hwq credits (outstanding-MPDU
 * cap). `credits` prints all 4 ACs; `credits <ac> <val>` sets one — only
 * poke while idle (live counter under traffic). */
void cmd_credits(int argc, char **argv)
{
    extern int16_t fhost_tx_hwq_credits_get(int ac);
    extern void fhost_tx_hwq_credits_set(int ac, int16_t v);

    if (argc >= 3) {
        fhost_tx_hwq_credits_set(atoi(argv[1]), (int16_t)atoi(argv[2]));
    }
    printf("hwq_credits: bk=%d be=%d vi=%d vo=%d\r\n",
        fhost_tx_hwq_credits_get(0), fhost_tx_hwq_credits_get(1),
        fhost_tx_hwq_credits_get(2), fhost_tx_hwq_credits_get(3));
}

void cmd_txstat(int argc, char **argv)
{
    fhost_printf("--- TX stat ---\r\n");
    fhost_printf("cfm_lat avg=%lu max=%lu cnt=%lu\r\n",
        fhost_statistic.cfm_latency_cnt ?
            fhost_statistic.cfm_latency_sum / fhost_statistic.cfm_latency_cnt : 0,
        fhost_statistic.cfm_latency_max,
        fhost_statistic.cfm_latency_cnt);
    // PM19 instrument: cfm_lat breakdown.
    // air_time   = done_time - chain_time  (HW: chain to BAR_DONE_HW)
    // sched_delay = now - done_time        (SW: BAR-done ISR to cfm_evt process)
    // cfm_lat    = air_time + sched_delay
    fhost_printf("  air_time avg=%lu max=%lu  sched_delay avg=%lu max=%lu\r\n",
        fhost_statistic.cfm_latency_cnt ?
            fhost_statistic.air_time_sum / fhost_statistic.cfm_latency_cnt : 0,
        fhost_statistic.air_time_max,
        fhost_statistic.cfm_latency_cnt ?
            fhost_statistic.sched_delay_sum / fhost_statistic.cfm_latency_cnt : 0,
        fhost_statistic.sched_delay_max);
    // Old four tokens keep name, order and position so the bench regex
    // `agg_close macif=.. cfm=.. fexch=.. push=..` still matches. RC_P9_0809.md.
    fhost_printf("agg_close macif=%lu cfm=%lu fexch=%lu push=%lu"
                 " maxcnt=%lu maxlen=%lu incompat=%lu timer=%lu incsta=%lu\r\n",
        fhost_statistic.agg_close_macif,
        fhost_statistic.agg_close_cfm,
        fhost_statistic.agg_close_fexch,
        fhost_statistic.agg_close_push,
        fhost_statistic.agg_close_maxcnt,
        fhost_statistic.agg_close_maxlen,
        fhost_statistic.agg_close_incompat,
        fhost_statistic.agg_close_timer,
        fhost_statistic.agg_close_incsta);
    // New LINE, not an edit of agg_close - the bench regexes key on that one.
    fhost_printf("rc_trial set=%lu first=%lu\r\n",
        fhost_statistic.rc_trial_set, fhost_statistic.rc_trial_first);
    fhost_printf("dma newtail=%lu newhead=%lu lfe_stale=%lu\r\n",
        fhost_statistic.dma_newtail_cnt,
        fhost_statistic.dma_newhead_cnt,
        fhost_statistic.lfe_stale_cnt);
    {
        /* to_lmac against the dma counters above places a TX loss on one side
         * or the other of the UMAC->LMAC handoff: climbing while newhead and
         * newtail stand still means the frames die below txu_cntrl_push().
         * Standing still with the netif still feeding means one of that
         * function's gates is rejecting them, and bail[] says which -- they all
         * discard silently, so nothing downstream records the loss. */
        extern uint32_t txu_cntrl_to_lmac;
        extern uint32_t txu_cntrl_bail_cnt[];
        fhost_printf("txu to_lmac=%lu bail vif=%lu sta=%lu chan=%lu port=%lu\r\n",
                     (unsigned long)txu_cntrl_to_lmac,
                     (unsigned long)txu_cntrl_bail_cnt[1],
                     (unsigned long)txu_cntrl_bail_cnt[2],
                     (unsigned long)txu_cntrl_bail_cnt[3],
                     (unsigned long)txu_cntrl_bail_cnt[4]);
    }
    fhost_printf("ampdu_hist 1=%lu 2-4=%lu 5-8=%lu 9-16=%lu 17+=%lu\r\n",
        fhost_statistic.ampdu_hist_1,
        fhost_statistic.ampdu_hist_2_4,
        fhost_statistic.ampdu_hist_5_8,
        fhost_statistic.ampdu_hist_9_16,
        fhost_statistic.ampdu_hist_17p);
    {
        /* Why each received frame did or did not go up. Read del_bcn_same
         * first: on a STA it is routine beacon suppression, not loss, and a
         * raw "delete" total is dominated by it. */
        extern volatile uint32_t macsw_rxl_del_nolen, macsw_rxl_del_bcn_same;
        extern volatile uint32_t macsw_rxl_del_bfmer, macsw_rxl_del_datamgt;
        extern volatile uint32_t macsw_rxl_del_other, macsw_rxl_up_bcn_new;
        extern volatile uint32_t macsw_rxl_up_datamgt;
        extern volatile uint32_t macsw_rxl_up_nosucc, macsw_rxl_up_nokey;
        extern volatile uint32_t macsw_rxl_up_badsta, macsw_rxl_up_novif;
        fhost_printf("rxl_up   datamgt=%lu bcn_new=%lu | nosucc=%lu nokey=%lu "
                     "badsta=%lu novif=%lu\r\n",
                     (unsigned long)macsw_rxl_up_datamgt,
                     (unsigned long)macsw_rxl_up_bcn_new,
                     (unsigned long)macsw_rxl_up_nosucc,
                     (unsigned long)macsw_rxl_up_nokey,
                     (unsigned long)macsw_rxl_up_badsta,
                     (unsigned long)macsw_rxl_up_novif);
        fhost_printf("rxl_del  bcn_same=%lu datamgt=%lu nolen=%lu bfmer=%lu other=%lu\r\n",
                     (unsigned long)macsw_rxl_del_bcn_same,
                     (unsigned long)macsw_rxl_del_datamgt,
                     (unsigned long)macsw_rxl_del_nolen,
                     (unsigned long)macsw_rxl_del_bfmer,
                     (unsigned long)macsw_rxl_del_other);
        /* RX ring desync guards. `ptr_bad` is the one that has to be watched: it counts
         * read pointers found OUTSIDE their own buffer, which used to reach the
         * dereference and fault (2026-08-09, reproduced 3/3 by reset+re-associate
         * churn). `pattern_bad` is the older in-range guard. Neither was printed
         * anywhere before, so a fault was the only way to learn either had happened. */
        extern volatile uint32_t macsw_rxl_desc_ptr_bad, macsw_rxl_desc_pattern_bad;
        extern volatile uint32_t macsw_rxl_embedded_ptr_bad;
        /* `healed` / `gaveup` split `embed_bad` by what the retry answered: healed = the
         * RBD contents turned up on a later look, so skipping the descriptor would have
         * dropped a frame that was arriving; gaveup = it never did and the ring was
         * re-synced past it, which is what this driver always did. Appended AFTER the
         * old three tokens so regexes written against them still match. */
        extern volatile uint32_t macsw_rxl_embed_healed, macsw_rxl_embed_gaveup;
        extern volatile uint8_t macsw_rxl_desc_retry_max;
        fhost_printf("rxl_ring ptr_bad=%lu pattern_bad=%lu embed_bad=%lu healed=%lu "
                     "gaveup=%lu retry_max=%u\r\n",
                     (unsigned long)macsw_rxl_desc_ptr_bad,
                     (unsigned long)macsw_rxl_desc_pattern_bad,
                     (unsigned long)macsw_rxl_embedded_ptr_bad,
                     (unsigned long)macsw_rxl_embed_healed,
                     (unsigned long)macsw_rxl_embed_gaveup,
                     (unsigned)macsw_rxl_desc_retry_max);
#if RXL_DESYNC_LOG
        /* The first few rejections WHOLE, because the counters cannot say HOW the ring
         * got out of step. A `found` one descriptor past `end` is this driver losing
         * count; a `found` outside [start,end) altogether is stale content from a
         * previous incarnation of the ring. Those need different fixes.
         * Opt-in (`RXL_DESYNC_LOG=4 make`): defconfig_ipro6_sdio has no room for it.
         *
         * site 0/1/2 = the READ pointer was out of range, in rxdesc_get / ih_drain /
         * ih_imm; `found` is that pointer and `pbd` is 0. site 11..14 = the read pointer
         * was fine and the DESCRIPTOR was refused, and `found` is the offending value:
         * 11 upatternrx, 12 first_pbd_ptr, 13 datastartptr, 14 dataendptr.
         *
         * `pbd - read == rxdesc_sz` means first_pbd_ptr is where a freshly written
         * descriptor puts it, so the RBD was simply not written yet; anything else means
         * it is stale and pointing at payload bytes. That is the whole question as of
         * 2026-08-10 and it is why these two columns replaced the constant start/end. */
        {
            extern volatile uint32_t macsw_rxl_desync_n, macsw_rxl_rxdesc_size;
            extern volatile uint32_t macsw_rxl_desync_log[RXL_DESYNC_LOG][5];
            /* `moved` separates the two ways `gaveup` can dominate: it counts held
             * descriptors whose offending word CHANGED between the first look and the
             * last. Non-zero proves SW is reading fresh memory, so the content genuinely
             * never became valid; zero leaves "SW re-read a stale copy" open. */
            extern volatile uint32_t macsw_rxl_embed_moved;
            /* `pbdpat ok/bad` counts the payload descriptor's OWN pattern word on every
             * descriptor that reaches the check. It decides nothing - it exists to say
             * whether this chip populates the field at all, which has to be known before
             * an absent RBD can be detected by its pattern instead of by range-checking
             * the garbage inside it. ok >> 0 with bad tracking embed_bad is the result
             * that would make the pattern the right guard. */
            extern volatile uint32_t macsw_rxl_pbd_pat_ok, macsw_rxl_pbd_pat_bad;
            /* RX buffer 1's real bounds. Every site-13/14 event measured on 2026-08-10
             * read its garbage from ONE address, 0x21058000; `buf1` turns a fault address
             * into an OFFSET, which is what separates "something writes that address"
             * from "the association frame sequence always lands there". */
            extern volatile uint32_t macsw_rxl_buf1_start, macsw_rxl_buf1_end;

            fhost_printf("rxl_desync n=%lu rxdesc_sz=%lu moved=%lu pbdpat_ok=%lu "
                         "pbdpat_bad=%lu buf1=%08lx-%08lx\r\n",
                         (unsigned long)macsw_rxl_desync_n,
                         (unsigned long)macsw_rxl_rxdesc_size,
                         (unsigned long)macsw_rxl_embed_moved,
                         (unsigned long)macsw_rxl_pbd_pat_ok,
                         (unsigned long)macsw_rxl_pbd_pat_bad,
                         (unsigned long)macsw_rxl_buf1_start,
                         (unsigned long)macsw_rxl_buf1_end);
            for (unsigned i = 0; i < RXL_DESYNC_LOG && i < macsw_rxl_desync_n; i++)
                fhost_printf("rxl_desync[%u] found=%08lx hw_wr=%08lx read=%08lx "
                             "pbd=%08lx site=%lu\r\n", i,
                             (unsigned long)macsw_rxl_desync_log[i][0],
                             (unsigned long)macsw_rxl_desync_log[i][1],
                             (unsigned long)macsw_rxl_desync_log[i][2],
                             (unsigned long)macsw_rxl_desync_log[i][3],
                             (unsigned long)macsw_rxl_desync_log[i][4]);
        }
#endif
    }
    {
        /* What the HW made of every frame it confirmed, per AC. Immune to the
         * sampling problem that makes a sub-millisecond TX invisible to a JTAG
         * poller: ok vs retry_limit answers "keyed but unacknowledged", and
         * nostatus (statinfo never written) answers "never keyed at all". */
        extern volatile uint32_t macsw_cfm_total[], macsw_cfm_ok[];
        extern volatile uint32_t macsw_cfm_retry_limit[], macsw_cfm_lifetime[];
        extern volatile uint32_t macsw_cfm_nostatus[], macsw_cfm_retries[];
        extern volatile uint32_t macsw_cfm_last_status;
        static const char *acn[] = {"BK", "BE", "VI", "VO", "BCN"};
        fhost_printf("--- cfm status census ---\r\n");
        fhost_printf("AC   total     ok  rtylim  lifetm  nostat\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++)
            fhost_printf("%-3s %6lu %6lu  %6lu  %6lu  %6lu\r\n",
                         i < 5 ? acn[i] : "??",
                         (unsigned long)macsw_cfm_total[i],
                         (unsigned long)macsw_cfm_ok[i],
                         (unsigned long)macsw_cfm_retry_limit[i],
                         (unsigned long)macsw_cfm_lifetime[i],
                         (unsigned long)macsw_cfm_nostatus[i]);
        fhost_printf("cfm_retries 0=%lu 1=%lu 2=%lu 3=%lu 4=%lu 5=%lu 6=%lu 7+=%lu "
                     "last_status=%08lx\r\n",
                     (unsigned long)macsw_cfm_retries[0],
                     (unsigned long)macsw_cfm_retries[1],
                     (unsigned long)macsw_cfm_retries[2],
                     (unsigned long)macsw_cfm_retries[3],
                     (unsigned long)macsw_cfm_retries[4],
                     (unsigned long)macsw_cfm_retries[5],
                     (unsigned long)macsw_cfm_retries[6],
                     (unsigned long)macsw_cfm_retries[7],
                     (unsigned long)macsw_cfm_last_status);
    }
    fhost_printf("macif_tx_evt=%lu pushed=%lu data_ind=%lu\r\n",
        fhost_statistic.macif_tx_evt_cnt,
        fhost_statistic.macif_tx_evt_pushed,
        fhost_statistic.macif_tx_data_ind_cnt);
    fhost_printf("tx_sched=%lu sched_pushed=%lu cfm=%lu\r\n",
        fhost_statistic.fhost_tx_sched_cnt,
        fhost_statistic.fhost_tx_sched_pushed,
        fhost_statistic.fhost_tx_cfm_cnt);
    fhost_printf("repush calls=%lu deferred=%lu drained=%lu wake=%lu\r\n",
        fhost_statistic.tx_repush_cnt,
        fhost_statistic.tx_repush_deferred,
        fhost_statistic.tx_repush_deferred_drained,
        fhost_statistic.tx_repush_deferred_wake);
    {
        extern volatile uint8_t g_txl_repush_defer;
        fhost_printf("repush_defer_mode=%u\r\n",
                     (unsigned)g_txl_repush_defer);
    }
    fhost_printf("tx_preempt=%lu single_push=%lu ppdu_leak=%lu underflow=%lu\r\n",
        fhost_statistic.macif_tx_preempt_cnt,
        fhost_statistic.macif_tx_single_push,
        fhost_statistic.ppdu_leak_reset_cnt,
        fhost_statistic.ppdu_underflow_cnt);
    if (fhost_statistic.ppdu_underflow_cnt) {
        fhost_printf("underflow sites:");
        for (int i = 0; i < 16; i++) {
            if (fhost_statistic.ppdu_underflow_site[i])
                fhost_printf(" %d=%u", i, fhost_statistic.ppdu_underflow_site[i]);
        }
        fhost_printf("\r\n");
    }
    // Per-site ppdu_cnt++ tracker (sites: 0=cntrl_push 1=he_agg_finish
    // 2=reformat 3=agg_finish 4=push_sglt 5=push_pool_empty
    // 6=reconfig_last 7=reconfig_loop 8=he_tb_retry)
    fhost_printf("inc sites:");
    for (int i = 0; i < 16; i++) {
        if (fhost_statistic.ppdu_inc_site[i])
            fhost_printf(" %d=%lu", i, (unsigned long)fhost_statistic.ppdu_inc_site[i]);
    }
    fhost_printf("\r\n");
    // PM33: per-site ppdu_cnt_dec mirror (paired with inc sites above).
    // dec sites: 0=he_tb_cat_ampdu_mpdu 1=he_tb_cat_mpdu_ampdu 2=he_tb_cat_ampdu
    //   3=ampdu_status_handle 4=he_tb_free_done 5=he_tb_transmit_cancelled
    //   6=he_tb_transmit_trigger 7=mpdu_status_done_sw_singleton
    //   8=mpdu_status_done_sw_agg 9=mpdu_status_singleton_done
    //   10=fexch_done_singleton 11=fexch_done_bar 12=recover_bar_stuck
    //   13=recover_dma_idle 14=recover_done_hw 15=recover_orphan
    //   16=PM32_force_drain 17=PM34_v3c_aggregates_direct_release
    //   18=PM34_root_cause_cfm_evt_extract
    // Note inc and dec site enumerations are independent — pairing is
    // semantic (e.g. inc site 5 push_pool_empty ⇔ dec via site 7/9/10/14).
    fhost_printf("dec sites:");
    for (int i = 0; i < 20; i++) {
        if (fhost_statistic.ppdu_dec_site[i])
            fhost_printf(" %d=%lu", i, (unsigned long)fhost_statistic.ppdu_dec_site[i]);
    }
    fhost_printf("\r\n");
    fhost_printf("agg_finish entered=%lu skipped=%lu emitted=%lu\r\n",
        (unsigned long)fhost_statistic.agg_finish_entered,
        (unsigned long)fhost_statistic.agg_finish_skipped,
        (unsigned long)fhost_statistic.agg_finish_emitted);
    {
        extern void macsw_tx_dma_dead_snapshot_print(void);
        macsw_tx_dma_dead_snapshot_print();
    }
    fhost_printf("agg_check ppdu0=%lu ppduN=%lu\r\n",
        fhost_statistic.agg_check_ppdu0,
        fhost_statistic.agg_check_ppduN);
    // PM15: AMPDU hold mechanism (txl_agg_hold_*).
    // hold_skipped: close_pending deferred (good — building bigger AMPDU)
    // closed_full:  close because curr_cnt hit max_cnt (24)
    // closed_age:   close because skip_n budget exhausted
    // timer_fired:  close because 500us hold timer expired (idle)
    // timer_cancel: timer cancelled by natural close
    {
        extern volatile uint32_t txl_agg_hold_skip_n, txl_agg_hold_us;
        extern volatile uint8_t  txl_agg_hold_max_cnt;
        extern volatile uint32_t txl_agg_hold_skipped, txl_agg_hold_closed_full;
        extern volatile uint32_t txl_agg_hold_closed_age, txl_agg_hold_timer_fired;
        extern volatile uint32_t txl_agg_hold_timer_cancel;
        extern volatile uint32_t txl_agg_pipeline_max;
        fhost_printf("agg_hold cfg skip_n=%lu max_cnt=%u us=%lu pipe=%lu\r\n",
            (unsigned long)txl_agg_hold_skip_n,
            (unsigned)txl_agg_hold_max_cnt,
            (unsigned long)txl_agg_hold_us,
            (unsigned long)txl_agg_pipeline_max);
        fhost_printf("agg_hold cnt skipped=%lu full=%lu age=%lu tmr_fired=%lu tmr_cncl=%lu\r\n",
            (unsigned long)txl_agg_hold_skipped,
            (unsigned long)txl_agg_hold_closed_full,
            (unsigned long)txl_agg_hold_closed_age,
            (unsigned long)txl_agg_hold_timer_fired,
            (unsigned long)txl_agg_hold_timer_cancel);
    }
    fhost_printf("ppdu_cnt max=%lu avg=%lu samples=%lu\r\n",
        fhost_statistic.ppdu_cnt_max,
        fhost_statistic.ppdu_cnt_samples ?
            fhost_statistic.ppdu_cnt_sum / fhost_statistic.ppdu_cnt_samples : 0,
        fhost_statistic.ppdu_cnt_samples);
    fhost_printf("mpdu total=%lu ok=%lu ba_valid=%lu ba_inv=%lu\r\n",
        fhost_statistic.tx_mpdu_total,
        fhost_statistic.tx_mpdu_ok,
        fhost_statistic.tx_ampdu_ba_valid,
        fhost_statistic.tx_ampdu_ba_invalid);
    // The two silent branches of txl_ba_extract_ack. When an uplink leg shows
    // mpdu ok delta 0 while total keeps moving, this line says which one ate
    // the confirmations: below_win = we advanced the BA window past the frame
    // (SSN desync), out_bmap = SN landed past the end of the bitmap.
    fhost_printf("mpdu drop below_win=%lu out_bmap=%lu allack=%lu walk=%lu\r\n",
        fhost_statistic.ba_sn_below_win,
        fhost_statistic.ba_sn_out_of_bmap,
        fhost_statistic.ba_null_at_cfm,
        fhost_statistic.ba_extract_ack_cnt);
    // WHICH of the five checks threw a BlockAck out, in the order they are
    // tested (txl_agg_ba_decode, then txl_agg_decode_compressed_ba). ba_inv
    // above counts A-MPDUs confirmed with no valid BA and is the cleanest
    // collapse signal on this box -- per-leg it reads 17-37 on a leg that
    // collapses and exactly 0 on one that does not -- but it cannot say why.
    // These five can, and none of them had a reader.
    fhost_printf("ba_rej keyidx=%lu sta=%lu addr=%lu len=%lu tid=%lu\r\n",
        fhost_statistic.ba_fail_keyidx,
        fhost_statistic.ba_fail_sta,
        fhost_statistic.ba_fail_addr,
        fhost_statistic.ba_fail_len,
        fhost_statistic.ba_fail_tid);
    // The confirmation census for AC_BE. nostatus separates a frame that was
    // keyed and lost from one the HW never reported on at all, which is the
    // distinction every "TX that never arrives" question turns on.
    {
        extern volatile uint32_t macsw_cfm_total[], macsw_cfm_ok[];
        extern volatile uint32_t macsw_cfm_retry_limit[], macsw_cfm_lifetime[];
        extern volatile uint32_t macsw_cfm_nostatus[];

        fhost_printf("cfm[BE] tot=%lu ok=%lu retry_lim=%lu lifetime=%lu nostatus=%lu\r\n",
            (unsigned long)macsw_cfm_total[1], (unsigned long)macsw_cfm_ok[1],
            (unsigned long)macsw_cfm_retry_limit[1],
            (unsigned long)macsw_cfm_lifetime[1],
            (unsigned long)macsw_cfm_nostatus[1]);
    }
    fhost_printf("bar done=%lu ok=%lu fail=%lu ba_fail=%lu\r\n",
        fhost_statistic.bar_done_cnt,
        fhost_statistic.bar_success_cnt,
        fhost_statistic.bar_fail_cnt,
        fhost_statistic.ba_fail_cnt);
    // BAR-on-drop: drop-hole flagged / 1-MPDU AMPDU+BAR kept / healed by BAR
    {
        extern volatile uint32_t bam_n_bar_pend_set, bam_n_bar_kept,
                                 bam_n_bar_heal, bam_n_bar_ssn_bump;
        fhost_printf("barpend set=%lu kept=%lu heal=%lu ssn_bump=%lu\r\n",
            (unsigned long)bam_n_bar_pend_set,
            (unsigned long)bam_n_bar_kept,
            (unsigned long)bam_n_bar_heal,
            (unsigned long)bam_n_bar_ssn_bump);
    }
    // PM12: per-AC aggregates push/pop asymmetry diagnostic.
    // Hypothesis: recover_stale case 1/2/4 dec ppdu_cnt without popping
    // owning agg_desc from txl->aggregates → orphan accumulates.
    // Expected: push - pop ~= agg=N seen in txl_dump_queues post-wedge.
    {
        static const char *acn[NX_TXQ_CNT] = {
            #if NX_TXQ_CNT > 0
            "BK",
            #endif
            #if NX_TXQ_CNT > 1
            "BE",
            #endif
            #if NX_TXQ_CNT > 2
            "VI",
            #endif
            #if NX_TXQ_CNT > 3
            "VO",
            #endif
            #if NX_TXQ_CNT > 4
            "BCN",
            #endif
        };
        // PM34 v3c columns:
        //   stale = aging-scan direct release (class e orphan drained)
        //   sk_hw = aging-scan skipped because HW DMA on this AMPDU's THDs
        //   sk_uc = aging-scan skipped because user_cnt != 1
        // PM36 column:
        //   nopg  = subset of stale where chain_time gate dropped to 10ms
        //           because tx_mpdu_ok was static for >= 1s (no-progress)
        fhost_printf("--- agg push/pop ---\r\n");
        fhost_printf("AC  push  pop   ba   bar_ok bar_fl bar_st bar_rc stale sk_hw sk_uc nopg  leak\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            int leak = (int)fhost_statistic.aggregates_push_cnt[i]
                     - (int)fhost_statistic.aggregates_pop_cnt[i];
            fhost_printf("%-3s %5u %5u %5u %5u %5u %5u %5u %5u %5u %5u %5u %5d\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.aggregates_push_cnt[i],
                fhost_statistic.aggregates_pop_cnt[i],
                fhost_statistic.aggregates_pop_ba[i],
                fhost_statistic.aggregates_pop_bar_ok[i],
                fhost_statistic.aggregates_pop_bar_fail[i],
                fhost_statistic.aggregates_pop_bar_stuck[i],
                fhost_statistic.aggregates_pop_recover[i],
                fhost_statistic.aggregates_stale_drain[i],
                fhost_statistic.aggregates_stale_skip_hw[i],
                fhost_statistic.aggregates_stale_skip_uc[i],
                fhost_statistic.no_progress_force[i],
                leak);
        }
        // PM34 root-cause: cfm_evt is now the canonical class (e) release.
        // skip_pr (skip_pending_release): cfm_evt deferred to pending_release_drain
        //   path (UNLINK queued the desc). Normal coordination handshake.
        // extract (cfm_aggregates_extract): cfm_evt popped agg_desc from
        //   txl->aggregates before release. Fires for class (e) wedges where
        //   HW halt mid-AMPDU never popped via happy / BAR / case 0 paths.
        //   v3c stale_drain count should drop near 0 if this picks up the work.
        fhost_printf("--- cfm_evt release coord (PM34 root-cause) ---\r\n");
        fhost_printf("AC  skip_pr extract\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %7u %7u\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.cfm_skip_pending_release[i],
                fhost_statistic.cfm_aggregates_extract[i]);
        }
        fhost_printf("--- recover_stale drains ---\r\n");
        fhost_printf("AC  c4_fire c4_mpdu c4_1st  c12_mpdu c12_1st\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %7u %7u %6u  %8u %7u\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.case4_drain_cnt[i],
                fhost_statistic.case4_drain_mpdus[i],
                fhost_statistic.case4_first_agg_cnt[i],
                fhost_statistic.case12_drain_mpdus[i],
                fhost_statistic.case12_first_agg_cnt[i]);
        }
        fhost_printf("--- pending_release (Phase 5) ---\r\n");
        fhost_printf("AC  queued drained skipped held\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            int held = (int)fhost_statistic.pending_release_queued[i]
                     - (int)fhost_statistic.pending_release_drained[i];
            fhost_printf("%-3s %6u %7u %7u %4d\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.pending_release_queued[i],
                fhost_statistic.pending_release_drained[i],
                fhost_statistic.pending_release_skip_busy[i],
                held);
        }
        // PM14 instrument: per-AC timer arming. PM11e/PM13 observation
        // is that AC_BE timer "never fires" while AC_VO does. fired=0 with
        // start>0 means the timer was armed but the deadline never hit
        // (HW timer disabled or deadline drift); move_skip>0 means
        // txl_timer_move ran while the AC bit was masked (silent no-op).
        fhost_printf("--- timer arming (PM14) ---\r\n");
        fhost_printf("AC  fired  start  clear  move  m_skip  bstop\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %5u %6u %6u %5u %6u %6u\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.timer_fired[i],
                fhost_statistic.timer_arm_start[i],
                fhost_statistic.timer_arm_clear[i],
                fhost_statistic.timer_arm_move[i],
                fhost_statistic.timer_move_skip[i],
                fhost_statistic.timer_backstop_arm[i]);
        }
        // PM27: agg_desc unlink path breakdown.
        // skip_gen: agg_desc recycled — HW-safe leak (expected).
        // ex_nhd:   extracted at non-head (PM27 fix succeeded).
        // skip_hd:  agg_desc not in aggregates list at all (residual leak).
        fhost_printf("--- unlink (PM27) ---\r\n");
        fhost_printf("AC  skip_gen ex_nhd  skip_hd\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %8u %6u %8u\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.unlink_skip_gen[i],
                fhost_statistic.unlink_extract_nonhead[i],
                fhost_statistic.unlink_skip_head[i]);
        }
        // PM28: HW DMA stall classifier (residual agg_desc leak path).
        // path_a: case4 entered, head si!=0 → no drain progress.
        // path_b: case4 NOT entered (dma_state==2), popped==0 stale_fail.
        // last_si / last_mci2 / last_dma capture the offending head
        // state — match against TX_STATUS_* bits to design force-drain.
        fhost_printf("--- stuck head (PM28) ---\r\n");
        fhost_printf("AC  path_a path_b   last_si    last_mci2 dma\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %6u %6u 0x%08lx 0x%08lx %3u\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.case_stuck_a[i],
                fhost_statistic.case_stuck_b[i],
                (unsigned long)fhost_statistic.case_stuck_si_last[i],
                (unsigned long)fhost_statistic.case_stuck_mci2_last[i],
                (unsigned)fhost_statistic.case_stuck_dma_last[i]);
        }
        // PM29: consecutive-same-head stale_fail run length (lifetime
        // max). Permanent wedge (HW state=2 forever) accumulates here;
        // transient stuck resolved by BA/BAR stays low (typically 0-1).
        // gen_int_status snapshot taken at last stuck capture — bit 14
        // = ac1_tx_dma_dead (HW's own acknowledgment of DMA death).
        fhost_printf("--- consec stuck (PM29) ---\r\n");
        fhost_printf("AC  now  max\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %4u %4u\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.consec_stuck_now[i],
                fhost_statistic.consec_stuck_max[i]);
        }
        fhost_printf("gen_int_status (at last stuck): 0x%08lx\r\n",
            (unsigned long)fhost_statistic.gen_int_status_at_stuck);
        // PM32: class (c) force-drain hits — path_b saw consec_stuck >=
        // STABLE_STUCK_THRESHOLD (50 ~= 1.5s) and force-popped the wedged
        // head with newhead restart. Each fire = one wedged AMPDU evicted.
        fhost_printf("--- force-drain (PM32) ---\r\n");
        fhost_printf("AC  force\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %5u\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.case_stuck_force[i]);
        }
        // PM30: close-path forensics. Class (d) wedge interpretation:
        //  H1 fingerprint: cp_calls == 0 post-burst → close path never invoked
        //  H2 fingerprint: cp_defer > 0 then cb_total > 0 with cb_guard_fail++
        //  H4 fingerprint: push_retry == a0_cnt (build came from repush drain)
        // Live state columns: armed / skip_count expose static internals so
        // we can tell whether a timer is still pending for the stuck build.
        fhost_printf("--- close-path forensics (PM30) ---\r\n");
        fhost_printf("AC  cp_call cp_null cp_defr cp_forc push_rty armed sk_cnt\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %7lu %7lu %7lu %7lu %8lu %5u %6lu\r\n",
                acn[i] ? acn[i] : "??",
                (unsigned long)fhost_statistic.agg_cp_calls[i],
                (unsigned long)fhost_statistic.agg_cp_skip_null[i],
                (unsigned long)fhost_statistic.agg_cp_defer[i],
                (unsigned long)fhost_statistic.agg_cp_force[i],
                (unsigned long)fhost_statistic.agg_push_retry[i],
                (unsigned)txl_agg_hold_armed_get((uint8_t)i),
                (unsigned long)txl_agg_hold_skip_count_get((uint8_t)i));
        }
        fhost_printf("agg_hold_cb total=%lu guard_fail=%lu (ppdu=%lu null=%lu) "
            "rearm now=%lu max=%lu\r\n",
            (unsigned long)fhost_statistic.agg_hold_cb_total,
            (unsigned long)fhost_statistic.agg_hold_cb_guard_fail,
            (unsigned long)fhost_statistic.agg_hold_cb_gf_ppdu,
            (unsigned long)fhost_statistic.agg_hold_cb_gf_null,
            (unsigned long)fhost_statistic.agg_hold_cb_rearm_now,
            (unsigned long)fhost_statistic.agg_hold_cb_rearm_max);
        /* DEBUG 2026-07-25 wedge #2: the newtail/newhead accounting was never
         * printed, and it is exactly what the aggpipe=2 (chain-behind) A/B
         * points at. newtail_idle counts the "DMA went idle under our tail
         * append" fallback in txl_cntrl_push; the pending queue is the
         * Option-B retry FIFO for a refused newhead. */
        fhost_printf("--- chain path (newtail/newhead) ---\r\n");
        fhost_printf("newtail=%lu newhead=%lu newtail_idle=%lu lfe_stale=%lu\r\n",
            (unsigned long)fhost_statistic.dma_newtail_cnt,
            (unsigned long)fhost_statistic.dma_newhead_cnt,
            (unsigned long)fhost_statistic.newtail_idle_cnt,
            (unsigned long)fhost_statistic.lfe_stale_cnt);
        {
            extern volatile uint8_t g_txl_newhead_st0;
            fhost_printf("newhead skip_st2=%lu skip_st3=%lu (st0_only gate=%u)\r\n",
                (unsigned long)fhost_statistic.newhead_skip_cnt,
                (unsigned long)fhost_statistic.newhead_st3_skip_cnt,
                (unsigned)g_txl_newhead_st0);
        }
        fhost_printf("AC  pend_q pend_drn pend_stl pend_ovf pend_maxd drain_call\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %6u %8u %8u %8u %9u %10u\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.pending_newhead_queued[i],
                fhost_statistic.pending_newhead_drained[i],
                fhost_statistic.pending_newhead_stale[i],
                fhost_statistic.pending_newhead_overflow[i],
                fhost_statistic.pending_newhead_max_depth[i],
                fhost_statistic.pending_newhead_drain_call[i]);
        }
        // PM45: HW TX FSM no-advance detector. assert_rec_fired counts
        // KE_EVT_RESET triggers attributable to PM45 (MAC reset on
        // HW state=3 wedge). no_advance_max is the highest run length
        // of consecutive AC_TMO fires with no head_ptr progress AND no
        // tx_mpdu_ok progress observed (one entry = ~635ms). Threshold
        // is 5 (~3s); see PM45_NO_ADVANCE_THRESHOLD in txl_cntrl.c.
        fhost_printf("--- PM45 hw-no-advance ---\r\n");
        fhost_printf("AC  now  max  assert_rec_fired  hw_head_prev\r\n");
        for (int i = 0; i < NX_TXQ_CNT; i++) {
            fhost_printf("%-3s %4u %4u %17u  0x%08lx\r\n",
                acn[i] ? acn[i] : "??",
                fhost_statistic.pm45_no_advance_now[i],
                fhost_statistic.pm45_no_advance_max[i],
                fhost_statistic.pm45_assert_rec_fired[i],
                (unsigned long)fhost_statistic.pm45_hw_head_prev[i]);
        }
    }
    // Internal frame pool. It is only NX_TXFRAME_CNT deep and is the sole
    // source for PS NULL / QoS NULL / PS-Poll, so a descriptor that is handed
    // out and never confirmed silences power-save entirely while the data path
    // (which uses a different pool) keeps working -- the failure then shows up
    // as an "alloc failed" in a caller with no view of who is holding it.
    {
        extern void txl_frame_dump(void);
        txl_frame_dump();
    }
    // Why a connect failed. The host is told MAC_ST_FAILURE in both cases, so
    // only these separate "our Auth/AssocReq never got out of the AC before the
    // 500 TU response timeout" (tmo_tx_pending, then cfm_late) from "it went out
    // clean and the AP never answered" (tmo_no_answer). repush counts the
    // unbounded retry loop that runs while the AC is not draining.
    {
        extern volatile uint32_t macsw_sm_cfm_late, macsw_sm_cfm_late_status;
        extern volatile uint32_t macsw_sm_cfm_intime, macsw_sm_tx_repush;
        extern volatile uint32_t macsw_sm_tx_repush_status;
        extern volatile uint32_t macsw_sm_rsp_tmo_tx_pending;
        extern volatile uint32_t macsw_sm_rsp_tmo_no_answer;
        fhost_printf("sm_connect  cfm_intime=%lu cfm_late=%lu (status=%08lx)  "
                     "repush=%lu (status=%08lx)\r\n",
                     (unsigned long)macsw_sm_cfm_intime,
                     (unsigned long)macsw_sm_cfm_late,
                     (unsigned long)macsw_sm_cfm_late_status,
                     (unsigned long)macsw_sm_tx_repush,
                     (unsigned long)macsw_sm_tx_repush_status);
        fhost_printf("sm_rsp_tmo  tx_pending=%lu (our TX stuck)  no_answer=%lu "
                     "(AP silent)\r\n",
                     (unsigned long)macsw_sm_rsp_tmo_tx_pending,
                     (unsigned long)macsw_sm_rsp_tmo_no_answer);
        // Push-to-confirm latency of the Auth/AssocReq itself. The response
        // timeout is a fixed 500 TU = 512 ms, so this distribution is what
        // decides whether a connect can succeed -- an EDCA path that is merely
        // healthy puts everything in the first bucket.
        {
            static const char *lbl[8] = {"<0.1ms", "<1ms", "<10ms", "<50ms",
                                         "<100ms", "<250ms", "<500ms", ">=500ms"};
            extern volatile uint32_t macsw_sm_lat_hist[8];
            extern volatile uint32_t macsw_sm_lat_last_us, macsw_sm_lat_max_us;
            fhost_printf("sm_lat      ");
            for (int i = 0; i < 8; i++)
                fhost_printf("%s=%lu ", lbl[i],
                             (unsigned long)macsw_sm_lat_hist[i]);
            fhost_printf("| last=%luus max=%luus\r\n",
                         (unsigned long)macsw_sm_lat_last_us,
                         (unsigned long)macsw_sm_lat_max_us);
            // Split at the NEW_HEAD write: sw = we had not told the HW yet,
            // hw = the HW has the descriptor and is not sending it. arm_skips
            // counts NEW_HEAD attempts refused because the DMA read busy.
            {
                extern volatile uint32_t macsw_sm_lat_sw_hist[8];
                extern volatile uint32_t macsw_sm_lat_hw_hist[8];
                extern volatile uint32_t macsw_sm_lat_last_sw_us;
                extern volatile uint32_t macsw_sm_lat_last_hw_us;
                extern volatile uint32_t macsw_sm_arm_skips;
                fhost_printf("sm_lat_sw   ");
                for (int i = 0; i < 8; i++)
                    fhost_printf("%s=%lu ", lbl[i],
                                 (unsigned long)macsw_sm_lat_sw_hist[i]);
                fhost_printf("| last=%luus\r\n",
                             (unsigned long)macsw_sm_lat_last_sw_us);
                fhost_printf("sm_lat_hw   ");
                for (int i = 0; i < 8; i++)
                    fhost_printf("%s=%lu ", lbl[i],
                                 (unsigned long)macsw_sm_lat_hw_hist[i]);
                fhost_printf("| last=%luus arm_skips=%lu\r\n",
                             (unsigned long)macsw_sm_lat_last_hw_us,
                             (unsigned long)macsw_sm_arm_skips);
            }
            // Was the HW simply on another channel context? If the confirm
            // lands right after a switch, the frame was waiting for the MAC to
            // come back, not for the medium.
            {
                extern volatile uint32_t macsw_chan_switch_cnt, macsw_chan_cur_idx;
                extern volatile uint32_t macsw_sm_chan_sw_in_win;
                extern volatile uint32_t macsw_sm_lat_after_chan_us;
                extern volatile uint32_t macsw_sm_cfm_chan_idx;
                fhost_printf("sm_chan     switches_total=%lu now_idx=%lu | "
                             "in_window=%lu cfm_after_switch=%luus cfm_idx=%lu\r\n",
                             (unsigned long)macsw_chan_switch_cnt,
                             (unsigned long)macsw_chan_cur_idx,
                             (unsigned long)macsw_sm_chan_sw_in_win,
                             (unsigned long)macsw_sm_lat_after_chan_us,
                             (unsigned long)macsw_sm_cfm_chan_idx);
            }
        }
    }
    // pck_cnt accounting: inc sites 0=discard 1=push_chain 2=push_other
    // 3=txu_inc_helper, dec sites 4=int_frame_evt 5=macif_cfm_push.
    // sum(inc) - sum(dec) must equal pck_cnt; a gap names the unbalanced site.
    {
        extern volatile uint32_t txl_pck_site[];
        extern uint32_t txl_cntrl_pck_cnt_get(void);
        unsigned long inc = 0, dec = 0;
        for (int i = 0; i < 4; i++) inc += txl_pck_site[i];
        for (int i = 4; i < 6; i++) dec += txl_pck_site[i];
        fhost_printf("pck_site inc %lu/%lu/%lu/%lu dec %lu/%lu  sum_inc=%lu "
                     "sum_dec=%lu delta=%ld pck_cnt=%lu\r\n",
                     (unsigned long)txl_pck_site[0], (unsigned long)txl_pck_site[1],
                     (unsigned long)txl_pck_site[2], (unsigned long)txl_pck_site[3],
                     (unsigned long)txl_pck_site[4], (unsigned long)txl_pck_site[5],
                     inc, dec, (long)(inc - dec),
                     (unsigned long)txl_cntrl_pck_cnt_get());
    }
    // First arming attempts after a PDS resume: what the HW held right after
    // SW wrote the head pointer and the NEW_HEAD bit. st 0=HALTED 1=PASSIVE
    // 2=ACTIVE 3=DEAD; hd is the HW's own head pointer, which should match
    // desc. s2 non-zero names a DMA-side rejection.
    {
        struct txl_rsm_cap_pr { uint32_t desc, hd, s1, s2, s4;
                                uint8_t ac, st_before, st_after; };
        extern struct txl_rsm_cap_pr txl_rsm_cap[];
        extern volatile uint32_t txl_rsm_cap_n, txl_rsm_newtail_after_wake;
        extern volatile uint32_t txl_newtail_passive_cnt;
        fhost_printf("rsm_arm n=%lu newtail_after_wake=%lu passive_rearm=%lu\r\n",
                     (unsigned long)txl_rsm_cap_n,
                     (unsigned long)txl_rsm_newtail_after_wake,
                     (unsigned long)txl_newtail_passive_cnt);
        for (unsigned i = 0; i < txl_rsm_cap_n && i < 6; i++) {
            fhost_printf("  [%u] ac=%u st %u->%u desc=%08lx hd=%08lx "
                         "s1=%08lx s2=%08lx s4=%08lx\r\n",
                         i, txl_rsm_cap[i].ac, txl_rsm_cap[i].st_before,
                         txl_rsm_cap[i].st_after,
                         (unsigned long)txl_rsm_cap[i].desc,
                         (unsigned long)txl_rsm_cap[i].hd,
                         (unsigned long)txl_rsm_cap[i].s1,
                         (unsigned long)txl_rsm_cap[i].s2,
                         (unsigned long)txl_rsm_cap[i].s4);
        }
    }
    // PDS entry gate: how often the chip powered down while macsw's own
    // doze conditions said it should not have.
    {
        extern volatile uint32_t rwnxl_pds_gate_total, rwnxl_pds_gate_busy,
                                 rwnxl_pds_gate_last, rwnxl_pds_gate_hits[],
                                 rwnxl_pds_gate_pck_max;
        fhost_printf("pds_gate total=%lu busy=%lu last=0x%lx "
                     "ps=%lu tx=%lu timer=%lu kevt=%lu qsent=%lu qsaved=%lu "
                     "pck_max=%lu\r\n",
                     (unsigned long)rwnxl_pds_gate_total,
                     (unsigned long)rwnxl_pds_gate_busy,
                     (unsigned long)rwnxl_pds_gate_last,
                     (unsigned long)rwnxl_pds_gate_hits[0],
                     (unsigned long)rwnxl_pds_gate_hits[1],
                     (unsigned long)rwnxl_pds_gate_hits[2],
                     (unsigned long)rwnxl_pds_gate_hits[3],
                     (unsigned long)rwnxl_pds_gate_hits[4],
                     (unsigned long)rwnxl_pds_gate_hits[5],
                     (unsigned long)rwnxl_pds_gate_pck_max);
    }
    if (argc > 1 && !strcmp(argv[1], "reset"))
        memset(&fhost_statistic, 0, sizeof(fhost_statistic));
}

void cmd_txl_dump_queues(int argc, char **argv)
{
    (void)argc; (void)argv;
    txl_dump_queues();
}

/* DEBUG 2026-07-25: drive the STA auto-reconnect mechanism directly.
 *
 * An unsolicited deauth is not something the bench can produce on demand - the
 * shared LAB02 AP sends one only sometimes under over-offer, and the NUC radio
 * is held by someone else's hostapd. This exercises everything except the AP:
 * the gate values, the transient task, and whether the stored credentials are
 * good enough to re-associate. wifi_sta_disconnect remains the test for the
 * suppression side. */
void cmd_reconn_arm(int argc, char **argv)
{
    extern void net_al_ext_sta_reconnect_arm(void);
    (void)argc; (void)argv;
    net_al_ext_sta_reconnect_arm();
}

/* DEBUG 2026-07-25 wedge #2: per-descriptor census of the LMAC transmitting
 * list, plus an on-demand newhead re-issue.
 *
 *   txkick            dump AC_BE
 *   txkick <ac>       dump one AC (0=BK 1=BE 2=VI 3=VO)
 *   txkick <ac> kick  dump, then re-issue ONE newhead on the head descriptor
 *
 * The kick is the decisive test: if the AC restarts, the wedge is a lost SW
 * kick (a newtail the DMA never consumed) and belongs in txl_cntrl_push; if
 * the AC stays in state 3, the HW is latched and only its default changes. */
void cmd_txkick(int argc, char **argv)
{
    extern void txl_wedge_dump(uint8_t ac);
    extern int txl_wedge_kick(uint8_t ac);
    uint8_t ac = 1; /* AC_BE */

    if (argc > 1)
        ac = (uint8_t)atoi(argv[1]);
    txl_wedge_dump(ac);
    if (argc > 2 && !strcmp(argv[2], "kick"))
        fhost_printf("[WEDGE] kick result=%d\r\n", txl_wedge_kick(ac));
}

/* DEBUG 2026-07-25 P1: fhost-side companion to txl_dump_queues. The
 * over-offer zero-copy wedge leaves the LMAC queues provably EMPTY, so the
 * held frames are on the fhost side - this names the TXQ and shows whether
 * the credit accounting explains why it can never be scheduled again. */
void cmd_fhost_census(int argc, char **argv)
{
    extern int fhost_tx_txq_probe(int idx, uint16_t *nb_ready, int8_t *credits,
                                  uint8_t *status, uint8_t *ac, uint32_t *first);
    extern volatile uint32_t fhost_cred_pushed;
    extern volatile uint32_t fhost_cred_returned;
    extern volatile uint32_t fhost_cred_leak_nodesc;
    extern volatile uint32_t fhost_cred_leak_txqdis;
    extern volatile uint32_t fhost_cred_leak_status;
    extern volatile uint32_t fhost_cred_upd_dropped;
    extern volatile int32_t  fhost_cred_upd_pos;
    extern volatile int32_t  fhost_cred_upd_neg;
    extern volatile uint32_t fhost_cred_upd_n;
    extern volatile uint32_t fhost_cred_repairs;
    extern volatile uint32_t fhost_cred_repaired;
    extern int16_t fhost_tx_hwq_credits_get(int ac);

    int in_flight_ok = 1;
    int nq = fhost_tx_txq_probe(-1, NULL, NULL, NULL, NULL, NULL);

    (void)argc; (void)argv;

    /* Deadlock fingerprint: a TXQ with status 0x82 (ENABLED|STOP_FULL),
     * nb_ready > 0 and credits <= 0 while hwq_credits[ac] sits at its full
     * value (i.e. nothing in flight anywhere). Such a queue can never be
     * scheduled again - no push, so no cfm, so no credit, forever - and it
     * pins every pbuf on its ready list, including the SMID zero-copy slots.
     * That is what kills the host datapath while the link stays associated.
     *
     * `pushed - returned` is the conservation check: with hwq_credits full it
     * must be 0. Anything else is a one-way credit leak. */
    fhost_printf("--- fhost TXQ census ---\r\n");
    fhost_printf("idx nb_ready cred stat ac  ready.first\r\n");
    for (int i = 0; i < nq; i++)
    {
        uint16_t nb_ready; int8_t credits; uint8_t status, ac; uint32_t first;

        if (!fhost_tx_txq_probe(i, &nb_ready, &credits, &status, &ac, &first))
            continue;
        if (!nb_ready && !credits && !status)
            continue;
        /* 0x80 = TXQ_ENABLED, 0x02 = TXQ_STOP_FULL (fhost_txq_flags). */
        fhost_printf("%3d %8u %4d 0x%02x %2u  0x%08lx%s\r\n",
                     i, nb_ready, credits, status, ac, (unsigned long)first,
                     (nb_ready && credits <= 0 && (status & 0x80))
                         ? "  <== WEDGED" : "");
    }
    fhost_printf("hwq_credits:");
    for (int ac = 0; ac < NX_TXQ_CNT; ac++)
    {
        fhost_printf(" %d/%d", fhost_tx_hwq_credits_get(ac), get_nx_txdesc_cnt(ac));
        if (fhost_tx_hwq_credits_get(ac) != get_nx_txdesc_cnt(ac))
            in_flight_ok = 0;
    }
    fhost_printf("\r\n");
    fhost_printf("credit pushed=%lu returned=%lu delta=%ld (in_flight %s)\r\n",
                 (unsigned long)fhost_cred_pushed,
                 (unsigned long)fhost_cred_returned,
                 (long)(fhost_cred_pushed - fhost_cred_returned),
                 in_flight_ok ? "0 => delta MUST be 0" : "nonzero");
    fhost_printf("credit leak: nodesc=%lu txq_dis=%lu bad_status=%lu\r\n",
                 (unsigned long)fhost_cred_leak_nodesc,
                 (unsigned long)fhost_cred_leak_txqdis,
                 (unsigned long)fhost_cred_leak_status);
    fhost_printf("BA offsets: n=%lu pos=%ld neg=%ld net=%ld dropped=%lu\r\n",
                 (unsigned long)fhost_cred_upd_n,
                 (long)fhost_cred_upd_pos,
                 (long)fhost_cred_upd_neg,
                 (long)(fhost_cred_upd_pos + fhost_cred_upd_neg),
                 (unsigned long)fhost_cred_upd_dropped);
    /* Both must stay 0 in a correct system: every repair is a credit drift
     * that would otherwise have wedged the queue permanently. */
    fhost_printf("credit repairs=%lu restored=%lu\r\n",
                 (unsigned long)fhost_cred_repairs,
                 (unsigned long)fhost_cred_repaired);
}

#if defined(CFG_REC) && WIFI_STATISTIC_ENABLE
int cmd_fw_dbg_rec(int argc, char **argv)
{
    extern char _ld_bl_static_fw_dbg_entry_start, _ld_bl_static_fw_dbg_entry_end;
    extern char _ld_bl_static_fw_dbg_count_start, _ld_bl_static_fw_dbg_count_end;

    fw_dbg_t *slot;
    uint32_t *p_count;
    int sum1, sum2, i;

    sum1 = ((fw_dbg_t *)&_ld_bl_static_fw_dbg_entry_end -
            (fw_dbg_t *)&_ld_bl_static_fw_dbg_entry_start);
    sum2 = ((uint32_t *)&_ld_bl_static_fw_dbg_count_end -
            (uint32_t *)&_ld_bl_static_fw_dbg_count_start);
    slot = (fw_dbg_t *)&_ld_bl_static_fw_dbg_entry_start;
    p_count = (uint32_t *)&_ld_bl_static_fw_dbg_count_start;

    if (sum1 != sum2) {
        fhost_printf("never here, assert %d,%d\r\n", sum1, sum2);
        return -1;
    }

    for (i = 0; i < sum1; i++) {
        fhost_printf("[idex:%4d] [count:%6d] %s:%d\r\n", i, *p_count, slot->name, slot->line);
        slot += 1;
        p_count += 1;
    }

    return 0;
}
#endif

#ifdef CONFIG_MAT
static int cmd_mat(int argc, char *argv[])
{
    void mat_print_mib(void);
    mat_print_mib();
}
#endif

// Manual MAC reset: queue KE_EVT_RESET (full rwnxl_reset_evt — machw/rxl/
// txl/mm reset, every in-flight frame cfm-flushed). Escape hatch for a
// wedged TX pipeline whose AC watchdog died; recovery runs in the WiFi
// task, so association drops and the STA must reconnect afterwards.
void cmd_mac_reset(int argc, char **argv)
{
    extern void dbg_mac_reset_request(void);
    (void)argc; (void)argv;
    dbg_mac_reset_request();
    fhost_printf("mac_reset: KE_EVT_RESET queued\r\n");
}

// Advertise or hide HE in the association request.
//
// The APs that matter here are production kit we cannot reconfigure, so dropping
// the HE capability IE from our own assoc request is how we get a format A/B from
// the DUT side against any AP.
//
// Two things this comment used to claim, both measured wrong on 2026-08-01:
//
//   "this part loses 27-44% of HE PPDUs on RX at any MCS while HT RX is clean"
//   RETRACTED. Both formats missed about equally. The real cause was RX buffer
//   sizing (RXL_BUFFER1_SIZE was derived from the max A-MSDU, not the max A-MPDU),
//   fixed separately; format was never the ceiling.
//
//   "On 2.4 GHz there is no VHT, so clearing HE means the AP must use HT"
//   WRONG on these APs. LAB03 answers a non-HE assoc request with vendor 256-QAM
//   VHT on 2.4 GHz -- measured, VHT share 0.909. Note CFG_HE force-defines CFG_VHT
//   (rwnx_config.h), so this part advertises VHT even though it is 2.4 GHz only.
//   Clearing HE therefore selects VHT, not HT, and a sniffer that cannot decode
//   vendor 256-QAM returns an empty capture that reads like a dead device.
//
// In the shipping config the AP picks HE (share 0.90-0.91, VHT 0.000); VHT only
// appears once HE is cleared here.
//
// Usage: hecap [on|off]   -- takes effect at the NEXT wifi_sta_connect
void cmd_hecap(int argc, char **argv)
{
    int cur = macsw_local_he_capa_get();

    if (cur < 0) {
        fhost_printf("hecap: this build has no HE\r\n");
        return;
    }
    if (argc > 1) {
        int want = !strcmp(argv[1], "on") ? 1 : (!strcmp(argv[1], "off") ? 0 : -1);

        if (want < 0) {
            fhost_printf("usage: hecap [on|off]\r\n");
            return;
        }
        macsw_local_he_capa_set(want);
        fhost_printf("hecap: %s -> %s (applies at the next connect)\r\n",
                     cur ? "on" : "off", want ? "on" : "off");
        return;
    }
    fhost_printf("hecap: %s\r\n", cur ? "on" : "off");
}

// Dump the lwIP ARP table.
//
// There was no way to see it from the console, which matters because
// etharp_find_entry() returns ERR_MEM -- surfacing as `connect ... failed
// err=12` in the low-power tickless client -- only when ALL ARP_TABLE_SIZE
// entries are PENDING. It can always recycle the oldest STABLE entry, so a
// table full of stable entries never fails.
//
// etharp_get_entry() reports STABLE entries only; pending ones read as absent.
// That is exactly the state of interest, so read this by elimination: a low
// stable count at the moment of an err=12 means the rest are pending.
void cmd_arp(int argc, char **argv)
{
    int stable = 0;
    size_t i;

    (void)argc; (void)argv;
    fhost_printf("ARP table (%d slots, STABLE entries only):\r\n", ARP_TABLE_SIZE);
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
        ip4_addr_t *ip = NULL;
        struct netif *nif = NULL;
        struct eth_addr *mac = NULL;

        if (!etharp_get_entry(i, &ip, &nif, &mac))
            continue;
        stable++;
        fhost_printf("  [%u] %u.%u.%u.%u  %02x:%02x:%02x:%02x:%02x:%02x  %c%c%u\r\n",
                     (unsigned)i,
                     (unsigned)ip4_addr1(ip), (unsigned)ip4_addr2(ip),
                     (unsigned)ip4_addr3(ip), (unsigned)ip4_addr4(ip),
                     mac->addr[0], mac->addr[1], mac->addr[2],
                     mac->addr[3], mac->addr[4], mac->addr[5],
                     nif ? nif->name[0] : '?', nif ? nif->name[1] : '?',
                     nif ? (unsigned)nif->num : 0u);
    }
    fhost_printf("  stable=%d of %d  -> %d slot(s) empty or PENDING\r\n",
                 stable, ARP_TABLE_SIZE, ARP_TABLE_SIZE - stable);
}

// BAR-on-drop debug: force bar_pending on an active TX BA agreement so the
// 1-MPDU AMPDU+BAR keep path can be exercised without a real drop event.
// Usage: bam_barpend [sta_idx] [tid]   (defaults 0 0)
void cmd_bam_barpend(int argc, char **argv)
{
    extern bool bam_bar_pending_force(uint16_t sta_idx, uint8_t tid);
    uint16_t sta_idx = (argc > 1) ? (uint16_t)atoi(argv[1]) : 0;
    uint8_t tid = (argc > 2) ? (uint8_t)atoi(argv[2]) : 0;

    if (bam_bar_pending_force(sta_idx, tid))
        fhost_printf("bar_pending forced sta=%u tid=%u\r\n", sta_idx, tid);
    else
        fhost_printf("no active TX BA agreement sta=%u tid=%u\r\n", sta_idx, tid);
}

#ifdef IOT_SDK_ADAPTER
#define SHELL_CMD_EXPORT_ALIAS(func, name, desc)                                                                \
    static void func##_adapter (char *buf, int len, int argc, char **argv)                                    \
    {                                                                                                           \
        func(argc, argv);                                                                                       \
                                                                                                                \
    }                                                                                                           \
    const static struct cli_command name##cli[] STATIC_CLI_CMD_ATTRIBUTE = {                                    \
                                                                            {#name, #desc , func##_adapter}   \
    }
#else
#include "shell.h"
#endif

SHELL_CMD_EXPORT_ALIAS(cmd_hello, hello, test cli hello);
SHELL_CMD_EXPORT_ALIAS(cmd_phy, phy, test cli hello);
#ifdef CMD_TXL_CNTRL_PUSH_AC_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_ac_set, ac_set, access_category config);
#endif
#ifdef CFG_FOR_COEXISTENCE_TEST_STOPAP_PATCH
SHELL_CMD_EXPORT_ALIAS(cmd_ap_stop, ap_stop, ap stop);
#endif
SHELL_CMD_EXPORT_ALIAS(wifi_scan_cmd, wifi_scan, wifi scan);
SHELL_CMD_EXPORT_ALIAS(wifi_connect_cmd, wifi_sta_connect, wifi station connect);
SHELL_CMD_EXPORT_ALIAS(wifi_disconnect_cmd, wifi_sta_disconnect, wifi station disconnect);
SHELL_CMD_EXPORT_ALIAS(lwip_cmd, lwip, show stats);
#ifdef CFG_FHOST_MONITOR
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sniffer_on, wifi_sniffer_on, wifi sniffer on);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sniffer_off, wifi_sniffer_off, wifi sniffer off);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_state_get, wifi_state, get wifi state);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sta_rssi_get, wifi_sta_rssi, get wifi sya rssi);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sta_channel_get, wifi_sta_channel, get wifi channel);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sta_ssid_passphr_get, wifi_sta_ssid_passphr_get, get wifi ssid password);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sta_mac_get, wifi_sta_mac_get, get wifi sta mac);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ht40, wifi_ht40, permit HT40 - set before stack_wifi);
SHELL_CMD_EXPORT_ALIAS(wifi_enable_autoreconnect_cmd, wifi_sta_autoconnect_enable, wifi station enable auto reconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_disable_autoreconnect_cmd, wifi_sta_autoconnect_disable, wifi station disable auto reconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_ps_on_cmd, wifi_sta_ps_on, wifi sta powersave mode on);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_ps_off_cmd, wifi_sta_ps_off, wifi sta powersave mode off);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_info_cmd, wifi_sta_info, wifi sta info);
SHELL_CMD_EXPORT_ALIAS(wifi_tpc_pwr_set_cmd, wifi_tpc_pwr_set, DEBUG set all per-rate tx power dbm);
SHELL_CMD_EXPORT_ALIAS(wifi_ap_sta_list_get_cmd, wifi_sta_list, get sta list in AP mode);
SHELL_CMD_EXPORT_ALIAS(wifi_ap_sta_delete_cmd, wifi_sta_del, delete one sta in AP mode);
SHELL_CMD_EXPORT_ALIAS(wifi_mgmr_ap_start_cmd, wifi_ap_start, start AP mode);
SHELL_CMD_EXPORT_ALIAS(wifi_mgmr_ap_stop_cmd, wifi_ap_stop, stop AP mode);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ap_mac_get, wifi_ap_mac_get, get wifi ap mac);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ap_conf_max_sta, wifi_ap_conf_max_sta, config AP mac sta);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_raw_send, wifi_raw_send, wifi raw send test);
#ifdef CFG_TG
SHELL_CMD_EXPORT_ALIAS(cmd_tg, tg, wifi tg);
#endif
#ifdef CFG_TG
SHELL_CMD_EXPORT_ALIAS(cmd_fhost_ping, fhost_ping, fhost ping);
#endif
#ifdef CONFIG_CLI_WIFI_DUBUG
SHELL_CMD_EXPORT_ALIAS(cmd_wifi, wifi, wifi);
#endif
SHELL_CMD_EXPORT_ALIAS(wifi_sta_static_ipv4, set_ipv4, ipc task set);
#ifdef CFG_IPERF
// Renamed alias from "iperf" to "rw_iperf" so esp_iperf's SHELL_CMD_EXPORT_ALIAS
// (components/3rdparty/esp/esp_iperf/iperf_cli.c:100) owns the "iperf" name.
SHELL_CMD_EXPORT_ALIAS(cmd_rw_iperf, rw_iperf, RivieraWaves native iperf);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_rc, rc, Print the Rate Control Table);
SHELL_CMD_EXPORT_ALIAS(cmd_fix_mcs, fix_mcs, bind a fixed HE MCS via the rate controller);
SHELL_CMD_EXPORT_ALIAS(cmd_rate, rate, set g_fw_rate);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_mode_set, wifi_mode_set, set ap/sta mode);
SHELL_CMD_EXPORT_ALIAS(cmd_txstat, txstat, TX statistics dump);
SHELL_CMD_EXPORT_ALIAS(cmd_ntr, ntr, netif TX trace counters);
SHELL_CMD_EXPORT_ALIAS(cmd_agghold, agghold, DEBUG tune A-MPDU hold-down);
SHELL_CMD_EXPORT_ALIAS(cmd_aggpipe, aggpipe, DEBUG tune A-MPDU close-gate pipeline depth);
SHELL_CMD_EXPORT_ALIAS(cmd_aggpool, aggpool, DEBUG A-MPDU descriptor pool depth per queue);
SHELL_CMD_EXPORT_ALIAS(cmd_rxhold, rxhold, DEBUG RX descriptor retry budget 0 disables);
SHELL_CMD_EXPORT_ALIAS(cmd_lgov, lgov, link governor - sensed link state joint TX policy);
SHELL_CMD_EXPORT_ALIAS(cmd_rc_dump, rc_dump, dump live RC table);
SHELL_CMD_EXPORT_ALIAS(cmd_rc_tune, rc_tune, RC step0 hysteresis and eviction bar);
SHELL_CMD_EXPORT_ALIAS(cmd_amsdu, amsdu, A-MSDU TX batching tuner);
#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_txprof, txprof, TX path per-frame profiler dump);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_poke, poke, write 32-bit memory word);
SHELL_CMD_EXPORT_ALIAS(cmd_poke, pokeb, write 8-bit memory byte);
#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_dmacpy, dmacpy, gated SDIO inject copy mode DMA/cached);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_credits, credits, view or set per-AC hwq credits);
#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_smid_wprio, wprio, set smid worker task priority);
SHELL_CMD_EXPORT_ALIAS(cmd_sdio_zc, sdio_zc, arm zero-copy dnld slots + open gate);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_bam_barpend, bam_barpend, force BAR-on-drop pending flag);
SHELL_CMD_EXPORT_ALIAS(cmd_mac_reset, mac_reset, manual MAC reset via KE_EVT_RESET recovery);
SHELL_CMD_EXPORT_ALIAS(cmd_hecap, hecap, advertise HE in assoc req: on|off);
SHELL_CMD_EXPORT_ALIAS(cmd_arp, arp, dump the lwIP ARP table);
/* `coex` moved to the ipro_coex component (components/wireless/ipro_coex). */
SHELL_CMD_EXPORT_ALIAS(cmd_txl_dump_queues, txl_dump_queues, dump per-AC TXL queue state);
SHELL_CMD_EXPORT_ALIAS(cmd_txkick, txkick, DEBUG transmitting-list census + newhead re-issue);
SHELL_CMD_EXPORT_ALIAS(cmd_reconn_arm, reconn_arm, DEBUG drive the STA auto-reconnect backoff);
SHELL_CMD_EXPORT_ALIAS(cmd_fhost_census, fhostq, dump fhost TXQ census + credit conservation);
#if WIFI_STATISTIC_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_fw_dbg, fw_dbg, fw debug param);
#endif
#if defined(CFG_REC) && WIFI_STATISTIC_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_fw_dbg_rec, fw_dbg_rec, fw debug param);
#endif
#if defined(CFG_CSIDMA_CLI) && CFG_CSIDMA_CLI
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_start, csidma_start, start csidma);
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_stop, csidma_stop, stop csidma);
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_ready, csidma_ready, ready csidma);
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_debug, csidma_debug, debug csidma);
#ifdef CSIDMA_FTM_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_force_ftm_mac_set, csidma_force_ftm_mac_set, csidma force ftm mac set);
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_force_ftm, csidma_force_ftm, csidma force ftm);
#endif
#endif
#ifdef CONFIG_MAT
SHELL_CMD_EXPORT_ALIAS(cmd_mat, mat, show MAT information);
#endif

int wifi6_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}
