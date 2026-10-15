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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include <rnm_server.h>
#include <rnm_ota.h>
#include "hal_sys.h"
#include "rnm_ota_core.h"
#include "rnm_ota_flash.h"

#define OTA_REQ_MIN    offsetof(rnm_ota_msg_t, program_part)
#define OTA_PART_HDR   offsetof(rnm_ota_msg_t, program_part.data)
#define OTA_RSP_LEN    (offsetof(rnm_ota_msg_t, status) + \
                        sizeof(struct ipro_ota_status))
#define OTA_REBOOT_MS  50

_Static_assert((int)RNM_OTA_E_STATE == (int)IPRO_OTA_ERROR_STATUS_INCORRECT,
               "ota code");
_Static_assert((int)RNM_OTA_E_MISC == (int)IPRO_OTA_ERROR_MISC, "ota code");
_Static_assert(RNM_OTA_CHUNK_MAX == RNM_OTA_PART_SIZE, "ota chunk");

static rnm_ota_core_t *s_ota;

static void ota_reply(rnms_t *rnm, rnm_msg_t *cmd, uint16_t ack,
                      uint16_t code, uint32_t val)
{
    rnm_ota_msg_t rsp;

    memset(&rsp, 0, sizeof(rsp));
    rnms_msg_fill_common(rnm, &rsp, cmd);
    rsp.msg_type = IPRO_OTA_MSG_STATUS;
    if (code != 0) {
        rsp.status.status_type = IPRO_OTA_ERROR;
        rsp.status.error.code = code;
    } else {
        rsp.status.status_type = ack;
        rsp.status.program_ack.offset = val;
    }
    rnms_msg_output(rnm, &rsp, OTA_RSP_LEN);
}

static uint16_t ota_start(const rnm_ota_msg_t *m, uint16_t len)
{
    if (len < OTA_PART_HDR) {
        return RNM_OTA_E_SIZE;
    }
    if (s_ota == NULL) {
        s_ota = malloc(sizeof(*s_ota));
        if (s_ota == NULL) {
            return RNM_OTA_E_MISC;
        }
    }
    return rnm_ota_core_start(s_ota, rnm_ota_flash_store(), NULL,
                              m->program_part.data_len);
}

static uint16_t ota_program(const rnm_ota_msg_t *m, uint16_t len)
{
    if (s_ota == NULL) {
        return RNM_OTA_E_STATE;
    }
    if (len < OTA_PART_HDR ||
        m->program_part.data_len != (uint32_t)(len - OTA_PART_HDR)) {
        rnm_ota_core_abort(s_ota);
        return RNM_OTA_E_SIZE;
    }
    return rnm_ota_core_program(s_ota, m->program_part.offset,
                                m->program_part.data,
                                m->program_part.data_len);
}

void rnms_handle_ota(rnms_t *rnm, rnm_msg_t *cmd)
{
    const rnm_ota_msg_t *m = cmd->data;
    uint16_t len = cmd->data_len;
    uint16_t ack = IPRO_OTA_NONE;
    uint16_t code;
    uint32_t val;
    int pending = 0;

    if (m == NULL || len < OTA_REQ_MIN) {
        return;
    }
    switch (m->msg_type) {
    case IPRO_OTA_MSG_START:
        ack = IPRO_OTA_START_ACK;
        code = ota_start(m, len);
        break;
    case IPRO_OTA_MSG_PROGRAM_PART:
        ack = IPRO_OTA_PROGRAM_ACK;
        code = ota_program(m, len);
        break;
    case IPRO_OTA_MSG_COMMIT:
        ack = IPRO_OTA_COMMIT_ACK;
        code = (s_ota != NULL) ? rnm_ota_core_commit(s_ota) : RNM_OTA_E_STATE;
        break;
    case IPRO_OTA_MSG_ABORT:
        ack = IPRO_OTA_ABORT_ACK;
        code = 0;
        if (s_ota != NULL) {
            rnm_ota_core_abort(s_ota);
        }
        break;
    case IPRO_OTA_MSG_STATUS:
        ack = IPRO_OTA_FIN_ACK;
        pending = rnm_ota_flash_pending();
        code = (pending < 0) ? RNM_OTA_E_MISC : 0;
        break;
    default:
        code = RNM_OTA_E_STATE;
        break;
    }
    if (ack == IPRO_OTA_FIN_ACK) {
        /* [7:0] FW activeIndex, [15:8] boot2's reason for not installing */
        val = (uint32_t)pending | ((uint32_t)rnm_ota_boot2_result() << 8);
    } else {
        val = (s_ota != NULL) ? s_ota->next_off : 0;
    }
    ota_reply(rnm, cmd, ack, code, val);
    if (ack == IPRO_OTA_COMMIT_ACK && code == 0) {
        printf("[ota] commit ok, rnms stack free %lu B\r\n",
               (unsigned long)(uxTaskGetStackHighWaterMark(NULL) *
                               sizeof(StackType_t)));
        vTaskDelay(pdMS_TO_TICKS(OTA_REBOOT_MS));
        hal_reboot_media();
    }
    if (s_ota != NULL && !s_ota->active) {
        free(s_ota);
        s_ota = NULL;
    }
}
