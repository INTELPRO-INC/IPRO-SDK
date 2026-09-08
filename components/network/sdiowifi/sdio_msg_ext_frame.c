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

#include "sdiowifi_config.h"
#include "sdio_msg_ext_frame.h"
#include "sdiowifi_tty.h"

#include "sdio_port.h"

void sdiowifi_ext_frame_process(sdiowifi_mgmr_t *sdm, const uint8_t *frame, const uint16_t frame_len)
{
    sdiowifi_ext_frame_common_hdr_t *hdr = (sdiowifi_ext_frame_common_hdr_t *)frame;

    switch (hdr->type) {
#if SDIOWIFI_TTY
    case SDIOWIFI_EXT_FRAME_TYPE_TTY:
    {
        sdiowifi_ext_frame_tty_msg_t *msg = (sdiowifi_ext_frame_tty_msg_t *)frame;
        const uint16_t tty_id = msg->tty_id;
        const int msg_len = frame_len - sizeof(sdiowifi_ext_frame_tty_msg_t);
        sdiowifi_ext_frame_handle_tty_rx(sdm, tty_id, msg->data, msg_len);
        break;
    }
#endif
    default:
        HR_LOGE("unknow ext frame\r\n");
        break;
    }
}
