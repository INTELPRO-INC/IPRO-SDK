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

#ifdef CONFIG_COMPONENTS_SDIOWIFI_ENABLE
/* Phase-Z v2b (DEBUG, sdiowifi trcver_sdio_ipro6.c): host opt-in to zero-copy
 * dnld slots. The host sends USER_EXT "ZCEN1" AFTER switching itself to
 * single-frame TX writes; only then may the device arm its 1604-byte-capped
 * zero-copy slots into the dnld queue (a host aggregate landing on one fails
 * the CMD53 -> -110 card-removed cascade). */
extern int smid_zc_host_enable(void);
/* ZCEN2-stride (DEBUG): host opt-in to multi-frame CMD53 at a fixed wire
 * stride, landed by an ADMA scatter chain of zc slots (zero-copy preserved).
 * Reply "ZCOK2 <nslot> <stride>"; nslot=0 = stride refused (gate off / v2b
 * session already armed) -> the host stays on the zc_single contract, with
 * the v2b slots armed so zero-copy still works. */
extern int smid_zc2_host_enable(int *nslot_out, int *stride_out);
/* ZCEN3 (DEBUG): ZCEN2 semantics PLUS device->host upld aggregation (chained
 * upld transfers on a [2B len]-prefixed wire). Reply "ZCOK3 <nslot> <stride>
 * <uslots>"; uslots=0 = upld agg refused -> plain ZCEN2 behavior. When
 * granted, the upld wire flips ONLY after the ZCOK3 response is pushed
 * (smid_zc3_wire_arm) so the response itself still parses on the old wire. */
extern int smid_zc3_host_enable(int *nslot_out, int *stride_out, int *uslots_out);
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
    /* parse order: exact-match ZCEN3 before ZCEN2 before ZCEN1 */
    if (pld_len >= 5 && memcmp(msg->payload, "ZCEN3", 5) == 0) {
        int nslot = 0, stride = 0, uslots = 0;
        if (smid_zc3_host_enable(&nslot, &stride, &uslots) < 0) {
            /* dnld stride refused: arm the v2b single-frame slots instead so
             * the host (already committed to zc_single) keeps zero-copy */
            smid_zc_host_enable();
        } else if (uslots > 0) {
            zc3_arm = 1;    /* flip the upld wire AFTER the response goes out */
        }
        snprintf(zc_rsp, sizeof(zc_rsp), "ZCOK3 %d %d %d", nslot, stride, uslots);
        rnm_warning("user_ext: ZCEN3 -> %s\r\n", zc_rsp);
        rsp_src = zc_rsp;
        rsp_len = strlen(zc_rsp) + 1;
        goto respond;
    }
    if (pld_len >= 5 && memcmp(msg->payload, "ZCEN2", 5) == 0) {
        int nslot, stride;
        if (smid_zc2_host_enable(&nslot, &stride) < 0) {
            /* stride refused: arm the v2b single-frame slots instead so the
             * host (already committed to zc_single) still gets zero-copy */
            smid_zc_host_enable();
        }
        snprintf(zc_rsp, sizeof(zc_rsp), "ZCOK2 %d %d", nslot, stride);
        rnm_warning("user_ext: ZCEN2 -> %s\r\n", zc_rsp);
        rsp_src = zc_rsp;
        rsp_len = strlen(zc_rsp) + 1;
        goto respond;
    }
    if (pld_len >= 5 && memcmp(msg->payload, "ZCEN1", 5) == 0) {
        int armed = smid_zc_host_enable();
        snprintf(zc_rsp, sizeof(zc_rsp), "ZCOK%d", armed);
        rnm_warning("user_ext: ZCEN1 -> %s\r\n", zc_rsp);
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
    /* ZCEN3 grant: the ZCOK3 response must go out on the OLD upld wire, and
     * everything after it prefixed -- the strict upld FIFO makes this push the
     * format boundary. Only arm when the push succeeded; a failed push means
     * the host never sees the grant, and the contract must not flip. */
    if (rnms_msg_output(rnm, rsp, sizeof(*rsp) + rsp_len) == 0 && zc3_arm) {
        smid_zc3_wire_arm();
    }
#else
    rnms_msg_output(rnm, rsp, sizeof(*rsp) + rsp_len);
#endif
}
