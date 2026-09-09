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

#include <rtos_al.h>

#include "sigma_dut.h"
#include "sigma_dut_priv.h"

/* MAC layer ke_msg API for TWT.
 *
 * We use ke_msg (TWT_SETUP_REQ/TWT_TEARDOWN_REQ) instead of the
 * wpa_supplicant TWT path because:
 *
 * 1. ke_msg updates MAC TWT flow state (twt_add_flow/twt_delete_flow)
 *    AND sends the action frame via twt_send_setup/teardown_frame.
 *
 * 2. wpa_supplicant path (TWT_SETUP/TWT_TEARDOWN ctrl_iface commands)
 *    only sends the action frame via wpa_drv_send_action WITHOUT
 *    updating MAC TWT state. After TWT_TEARDOWN, the MAC still thinks
 *    TWT is active and keeps the DUT in TWT sleep schedule, causing
 *    link loss.
 *
 * The txl_frame pool (NX_TXFRAME_CNT=2) is NOT exhausted — earlier
 * assumption was wrong. ke_msg path works reliably.
 */
#include <ke/ke_msg.h>
#include <ke/ke_task.h>
#include <fhost.h>
#include <export/export_macsw.h>
#include <shell.h>

/* Local copies of twt_task.h structures */
struct sigma_twt_conf {
    uint8_t flow_type;
    uint8_t wake_int_exp;
    uint8_t wake_dur_unit;
    uint8_t min_twt_wake_dur;
    uint16_t wake_int_mantissa;
};

struct sigma_twt_setup_req {
    uint8_t vif_idx;
    uint8_t setup_cmd;
    uint8_t flow_id;
    struct sigma_twt_conf conf;
};

struct sigma_twt_teardown_req {
    uint8_t neg_type;
    uint8_t all_twt;
    uint8_t id;
    uint8_t vif_idx;
};

#define SIGMA_TWT_SETUP_REQ     KE_FIRST_MSG(TASK_TWT)
#define SIGMA_TWT_TEARDOWN_REQ  (KE_FIRST_MSG(TASK_TWT) + 3)

/* Get the MAC VIF index for the STA interface (fhost VIF 0 -> mac VIF index) */
static uint8_t sigma_get_sta_vif_idx(void)
{
    void *mac_vif = fhost_to_mac_vif(0);
    if (!mac_vif)
        return 0;
    return mac_vif_get_index(mac_vif);
}

static int cmd_sta_set_rfeature(struct sigma_conn *conn,
                                const struct sigma_cmd *cmd)
{
    const char *twt_setup = sigma_cmd_get_param(cmd, "twt_setup");
    const char *twt_teardown = sigma_cmd_get_param(cmd, "twt_teardown");

    /* TWT Setup via MAC ke_msg */
    if (twt_setup) {
        const char *setup_cmd_str = twt_setup;
        const char *flow_type = sigma_cmd_get_param(cmd, "flow_type");
        const char *wake_dur = sigma_cmd_get_param(cmd, "wake_duration");
        const char *wake_int = sigma_cmd_get_param(cmd, "wake_interval");
        const char *wake_int_exp = sigma_cmd_get_param(cmd, "wake_intvl_exp");
        const char *wake_int_man = sigma_cmd_get_param(cmd, "wake_intvl_mantissa");
        const char *min_wake = sigma_cmd_get_param(cmd, "min_wake_dur");

        /* mac_twt_setup_commands: REQUEST=0, SUGGEST=1, DEMAND=2 */
        uint8_t cmd_val = 0;
        if (strcasecmp(setup_cmd_str, "request") == 0)
            cmd_val = 0;
        else if (strcasecmp(setup_cmd_str, "suggest") == 0)
            cmd_val = 1;
        else if (strcasecmp(setup_cmd_str, "demand") == 0)
            cmd_val = 2;
        else
            cmd_val = (uint8_t)atoi(setup_cmd_str);

        struct sigma_twt_conf conf;
        memset(&conf, 0, sizeof(conf));
        conf.flow_type = flow_type ? (uint8_t)atoi(flow_type) : 0;
        conf.wake_int_exp = wake_int_exp ? (uint8_t)atoi(wake_int_exp) : 10;
        conf.min_twt_wake_dur = min_wake ? (uint8_t)atoi(min_wake) :
                                (wake_dur ? (uint8_t)atoi(wake_dur) : 128);
        conf.wake_int_mantissa = wake_int_man ?
            (uint16_t)atoi(wake_int_man) :
            (wake_int ? (uint16_t)atoi(wake_int) : 8192);

        struct sigma_twt_setup_req *req;
        req = KE_MSG_ALLOC(SIGMA_TWT_SETUP_REQ, TASK_TWT, TASK_NONE,
                           sigma_twt_setup_req);
        req->vif_idx = sigma_get_sta_vif_idx();
        req->setup_cmd = cmd_val;
        req->flow_id = 0;
        req->conf = conf;
        ke_msg_send(req);

        return sigma_resp_complete(conn, NULL);
    }

    /* TWT Teardown via MAC ke_msg */
    if (twt_teardown) {
        int all = (strcasecmp(twt_teardown, "all") == 0);
        int flow_id = all ? 0 : atoi(twt_teardown);

        struct sigma_twt_teardown_req *req;
        req = KE_MSG_ALLOC(SIGMA_TWT_TEARDOWN_REQ, TASK_TWT, TASK_NONE,
                           sigma_twt_teardown_req);
        req->vif_idx = sigma_get_sta_vif_idx();
        req->neg_type = 0;
        req->all_twt = all ? 1 : 0;
        req->id = (uint8_t)flow_id;
        ke_msg_send(req);

        return sigma_resp_complete(conn, NULL);
    }

    /* MCS fixed rate */
    const char *mcs = sigma_cmd_get_param(cmd, "mcs_fixedrate");
    if (mcs) {
        extern int wifi_mgmr_rate_config(uint16_t fixed_rate_cfg);
        uint16_t rate = (uint16_t)atoi(mcs);
        wifi_mgmr_rate_config(rate);
        return sigma_resp_complete(conn, NULL);
    }

    /* Guard interval */
    const char *gi = sigma_cmd_get_param(cmd, "gi");
    if (gi) {
        return sigma_resp_complete(conn, NULL);
    }

    return sigma_resp_complete(conn, NULL);
}

const struct sigma_cmd_entry sigma_he_commands[] = {
    { "sta_set_rfeature", cmd_sta_set_rfeature },
    { NULL, NULL }
};

/* Serial shell command: twt_teardown [flow_id | all] */
static int cmd_twt_teardown(int argc, char **argv)
{
    int all = 0;
    int flow_id = 0;

    if (argc >= 2) {
        if (strcmp(argv[1], "all") == 0)
            all = 1;
        else
            flow_id = atoi(argv[1]);
    } else {
        all = 1;
    }

    struct sigma_twt_teardown_req *req;
    req = KE_MSG_ALLOC(SIGMA_TWT_TEARDOWN_REQ, TASK_TWT, TASK_NONE,
                       sigma_twt_teardown_req);
    req->vif_idx = sigma_get_sta_vif_idx();
    req->neg_type = 0;
    req->all_twt = all ? 1 : 0;
    req->id = (uint8_t)flow_id;
    ke_msg_send(req);
    printf("[TWT] teardown sent (all=%d flow_id=%d vif_idx=%d)\n",
           all, flow_id, req->vif_idx);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_twt_teardown, twt_teardown, TWT teardown all or by flow id);
