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

#include <rnm_server.h>
#include <stdio.h>
#include <string.h>

#if __has_include(<generated/autoconf.h>)
#include <generated/autoconf.h>
#endif

#ifdef CONFIG_RNM_REBOOT_DL
#include <FreeRTOS.h>
#include <task.h>
#include "hal_sys.h"
#endif

#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
/* Host "ZCEN3" -> reply "ZCOK3 <nslot> <stride> <uslots>". The upld wire
 * flips to [2B len] framing only after this reply has been pushed. */
extern void smid_zc3_host_enable(int *nslot_out, int *stride_out, int *uslots_out);
extern void smid_zc3_wire_arm(void);
#endif

void rnms_handle_user_ext(rnms_t *rnm, rnm_msg_t *cmd)
{
    uint8_t msg_buf[256];
    rnm_user_ext_msg_t *rsp = (rnm_user_ext_msg_t *)msg_buf;
    rnm_user_ext_msg_t *msg = cmd->data;
    const char rsp_str[] = "response from device";
    const char *rsp_src = rsp_str;
    size_t rsp_len = sizeof(rsp_str);
    size_t pld_len;

    if (!msg || cmd->data_len < sizeof(*msg)) {
        return;
    }
    pld_len = cmd->data_len - sizeof(*msg);

#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
    static char zc_rsp[24];
    int zc3_arm = 0;
#ifdef CONFIG_RNM_REBOOT_DL
    int reboot_dl = 0;
    if (pld_len >= 4 && memcmp(msg->payload, "I6RB", 4) == 0) {
        reboot_dl = 1;
        rsp_src = "I6OK";
        rsp_len = 5;
        goto respond;
    }
#endif
    if (pld_len >= 5 && memcmp(msg->payload, "ZCEN3", 5) == 0) {
        int nslot, stride, uslots;
        smid_zc3_host_enable(&nslot, &stride, &uslots);
        zc3_arm = 1;
        snprintf(zc_rsp, sizeof(zc_rsp), "ZCOK3 %d %d %d", nslot, stride, uslots);
        rnm_warning("user_ext: ZCEN3 -> %s\r\n", zc_rsp);
        rsp_src = zc_rsp;
        rsp_len = strlen(zc_rsp) + 1;
        goto respond;
    }
#endif

    rnm_warning("Recv user ext (%u bytes)\r\n", (unsigned)pld_len);

#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
respond:
#endif
    memset(rsp, 0, sizeof(*rsp));
    // field cmd, session_id should be set
    // flag should be RNM_MSG_FLAG_ACK
    rnms_msg_fill_common(rnm, rsp, cmd);
    memcpy(rsp->payload, rsp_src, rsp_len);

#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
    /* The upld FIFO makes this push the wire boundary; a failed push means
     * the host never saw the grant, so the wire stays as it is. */
    if (rnms_msg_output(rnm, rsp, sizeof(*rsp) + rsp_len) == 0 && zc3_arm) {
        smid_zc3_wire_arm();
    }
#ifdef CONFIG_RNM_REBOOT_DL
    /* Reboots to download mode 50 ms after the reply */
    if (reboot_dl) {
        vTaskDelay(pdMS_TO_TICKS(50));
        hal_reboot_download();
    }
#endif
#else
    rnms_msg_output(rnm, rsp, sizeof(*rsp) + rsp_len);
#endif
}
