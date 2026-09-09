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
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include <lwip/inet.h>
#include <lwip/sockets.h>

#include "sigma_dut.h"
#include "sigma_dut_priv.h"

#ifdef CFG_TG
#include "net_tg_al_priv.h"
#include "net_ping_al_priv.h"
#endif

#define SIGMA_MAX_STREAMS    8

struct sigma_stream {
    uint32_t id;
    int configured;
    int active;

    /* Profile config */
    int profile;        /* 1=FILE_TX, 4=TRANSC */
    int direction;      /* 1=send, 2=recv */
    uint32_t dst_ip;
    uint16_t dst_port;
    uint16_t src_port;
    uint32_t rate;
    uint32_t payload_size;
    uint32_t duration;
    uint8_t tos;

#ifdef CFG_TG
    uint32_t tg_stream_id;
#endif
};

static struct sigma_stream streams[SIGMA_MAX_STREAMS];
static uint32_t next_stream_id = 1;

static struct sigma_stream *find_stream(uint32_t id)
{
    for (int i = 0; i < SIGMA_MAX_STREAMS; i++) {
        if (streams[i].configured && streams[i].id == id)
            return &streams[i];
    }
    return NULL;
}

static struct sigma_stream *alloc_stream(void)
{
    for (int i = 0; i < SIGMA_MAX_STREAMS; i++) {
        if (!streams[i].configured) {
            memset(&streams[i], 0, sizeof(streams[i]));
            streams[i].id = next_stream_id++;
            streams[i].configured = 1;
            return &streams[i];
        }
    }
    return NULL;
}

static int cmd_traffic_agent_config(struct sigma_conn *conn,
                                    const struct sigma_cmd *cmd)
{
    struct sigma_stream *s = alloc_stream();
    if (!s)
        return sigma_resp_error(conn, "errorCode,NoFreeStream");

    const char *profile = sigma_cmd_get_param(cmd, "profile");
    const char *direction = sigma_cmd_get_param(cmd, "direction");
    const char *dst = sigma_cmd_get_param(cmd, "destination");
    const char *dport = sigma_cmd_get_param(cmd, "destinationport");
    const char *sport = sigma_cmd_get_param(cmd, "sourceport");
    const char *rate = sigma_cmd_get_param(cmd, "framerate");
    const char *size = sigma_cmd_get_param(cmd, "payloadsize");
    const char *dur = sigma_cmd_get_param(cmd, "duration");
    const char *tos = sigma_cmd_get_param(cmd, "trafficclass");

    if (profile) {
        if (strcasecmp(profile, "file_transfer") == 0)
            s->profile = 1;
        else if (strcasecmp(profile, "multicast") == 0)
            s->profile = 2;
        else if (strcasecmp(profile, "iptv") == 0)
            s->profile = 3;
        else if (strcasecmp(profile, "transaction") == 0)
            s->profile = 4;
        else
            s->profile = atoi(profile);
    } else {
        s->profile = 1; /* default: file transfer */
    }

    if (direction) {
        if (strcasecmp(direction, "send") == 0)
            s->direction = 1;
        else if (strcasecmp(direction, "receive") == 0)
            s->direction = 2;
    }

    if (dst)   s->dst_ip      = ipaddr_addr(dst);
    if (dport) s->dst_port    = (uint16_t)atoi(dport);
    if (sport) s->src_port    = (uint16_t)atoi(sport);
    if (rate)  s->rate         = (uint32_t)atoi(rate);
    if (size)  s->payload_size = (uint32_t)atoi(size);
    if (dur)   s->duration     = (uint32_t)atoi(dur);
    if (tos)   s->tos          = (uint8_t)atoi(tos);

    /* Defaults */
    if (!s->dst_port)    s->dst_port = 9000;
    if (!s->src_port)    s->src_port = s->dst_port;
    if (!s->rate)        s->rate = 50;
    if (!s->payload_size) s->payload_size = 1470;
    if (!s->duration)    s->duration = 10;

    return sigma_resp_complete(conn, "streamID,%u", s->id);
}

static int cmd_traffic_agent_send(struct sigma_conn *conn,
                                  const struct sigma_cmd *cmd)
{
    const char *sid = sigma_cmd_get_param(cmd, "streamid");
    if (!sid)
        return sigma_resp_error(conn, "errorCode,MissingStreamID");

    uint32_t id = (uint32_t)atoi(sid);
    struct sigma_stream *s = find_stream(id);
    if (!s)
        return sigma_resp_error(conn, "errorCode,InvalidStreamID");

#ifdef CFG_TG
    /* Use fhost_tg infrastructure */
    uint32_t local_ip = 0;
    {
        uint32_t dummy;
        extern int wifi_sta_ip4_addr_get(uint32_t *, uint32_t *, uint32_t *, uint32_t *);
        wifi_sta_ip4_addr_get(&local_ip, &dummy, &dummy, &dummy);
    }

    /* Find a free TG stream slot */
    extern struct fhost_tg_stream g_streams[];
    int tg_id = -1;
    for (int i = 0; i < FHOST_TG_MAX_TRAFFIC_STREAMS; i++) {
        if (!g_streams[i].active) {
            tg_id = i;
            break;
        }
    }
    if (tg_id < 0)
        return sigma_resp_error(conn, "errorCode,NoFreeTGStream");

    s->tg_stream_id = tg_id;

    int ret = fhost_tg_config(tg_id, s->profile, FHOST_TG_DIRECT_SEND,
                              s->dst_ip, s->dst_port, local_ip, s->src_port,
                              s->rate, s->payload_size, s->duration, s->tos);
    if (ret)
        return sigma_resp_error(conn, "errorCode,TGConfigFailed");

    ret = fhost_tg_start(tg_id);
    if (ret)
        return sigma_resp_error(conn, "errorCode,TGStartFailed");

    s->active = 1;
#endif /* CFG_TG */

    return sigma_resp_complete(conn, "streamID,%u", s->id);
}

static int cmd_traffic_agent_receive(struct sigma_conn *conn,
                                     const struct sigma_cmd *cmd)
{
    const char *sid = sigma_cmd_get_param(cmd, "streamid");
    if (!sid)
        return sigma_resp_error(conn, "errorCode,MissingStreamID");

    uint32_t id = (uint32_t)atoi(sid);
    struct sigma_stream *s = find_stream(id);
    if (!s)
        return sigma_resp_error(conn, "errorCode,InvalidStreamID");

#ifdef CFG_TG
    uint32_t local_ip = 0;
    {
        uint32_t dummy;
        extern int wifi_sta_ip4_addr_get(uint32_t *, uint32_t *, uint32_t *, uint32_t *);
        wifi_sta_ip4_addr_get(&local_ip, &dummy, &dummy, &dummy);
    }

    extern struct fhost_tg_stream g_streams[];
    int tg_id = -1;
    for (int i = 0; i < FHOST_TG_MAX_TRAFFIC_STREAMS; i++) {
        if (!g_streams[i].active) {
            tg_id = i;
            break;
        }
    }
    if (tg_id < 0)
        return sigma_resp_error(conn, "errorCode,NoFreeTGStream");

    s->tg_stream_id = tg_id;

    int ret = fhost_tg_config(tg_id, s->profile, FHOST_TG_DIRECT_RECV,
                              s->dst_ip, s->dst_port, local_ip, s->src_port,
                              s->rate, s->payload_size, s->duration, s->tos);
    if (ret)
        return sigma_resp_error(conn, "errorCode,TGConfigFailed");

    ret = fhost_tg_start(tg_id);
    if (ret)
        return sigma_resp_error(conn, "errorCode,TGStartFailed");

    s->active = 1;
#endif /* CFG_TG */

    return sigma_resp_complete(conn, "streamID,%u", s->id);
}

static int cmd_traffic_agent_reset(struct sigma_conn *conn,
                                   const struct sigma_cmd *cmd)
{
#ifdef CFG_TG
    for (int i = 0; i < SIGMA_MAX_STREAMS; i++) {
        if (streams[i].active) {
            fhost_tg_stop(streams[i].tg_stream_id);
        }
    }
#endif

    memset(streams, 0, sizeof(streams));
    next_stream_id = 1;

    return sigma_resp_complete(conn, NULL);
}

static int cmd_traffic_send_ping(struct sigma_conn *conn,
                                 const struct sigma_cmd *cmd)
{
#ifdef CFG_TG
    const char *dst = sigma_cmd_get_param(cmd, "destination");
    const char *cnt = sigma_cmd_get_param(cmd, "framecount");
    const char *size = sigma_cmd_get_param(cmd, "framesize");
    const char *rate_s = sigma_cmd_get_param(cmd, "framerate");

    if (!dst)
        return sigma_resp_error(conn, "errorCode,MissingDestination");
    struct fhost_ping_task_args args;
    memset(&args, 0, sizeof(args));

    args.rip = ipaddr_addr(dst);
    args.pksize = size ? (uint32_t)atoi(size) : 64;
    args.rate = rate_s ? (uint32_t)atoi(rate_s) : 1;

    uint32_t count = cnt ? (uint32_t)atoi(cnt) : 3;
    /* duration = count / rate (at least 1 second) */
    args.duration = (count + args.rate - 1) / args.rate;
    if (args.duration == 0)
        args.duration = 1;

    args.background = false;

    rtos_task_handle h = fhost_ping_start(&args);
    if (!h)
        return sigma_resp_error(conn, "errorCode,PingFailed");

    /* Wait for ping task to finish */
    vTaskDelay(pdMS_TO_TICKS(args.duration * 1000 + 2000));

    /* Find the ping stream and report */
    extern struct fhost_ping_stream p_streams[];
    int tx = 0, rx = 0;
    for (int i = 0; i < FHOST_PING_MAX_STREAMS; i++) {
        if (p_streams[i].ping_handle == h) {
            tx = p_streams[i].stats.tx_frames;
            rx = p_streams[i].stats.rx_frames;
            break;
        }
    }

    return sigma_resp_complete(conn, "sent,%d,replies,%d", tx, rx);
#else
    return sigma_resp_error(conn, "errorCode,TG_NOT_ENABLED");
#endif
}

const struct sigma_cmd_entry sigma_traffic_commands[] = {
    { "traffic_agent_config",   cmd_traffic_agent_config },
    { "traffic_agent_send",     cmd_traffic_agent_send },
    { "traffic_agent_receive",  cmd_traffic_agent_receive },
    { "traffic_agent_reset",    cmd_traffic_agent_reset },
    { "traffic_send_ping",      cmd_traffic_send_ping },
    { NULL, NULL }
};
