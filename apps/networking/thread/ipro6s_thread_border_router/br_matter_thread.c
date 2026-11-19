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

#include <ipro_matter_br_thread.h>
#include <openthread/border_agent.h>
#include <openthread/ip6.h>
#include <openthread/thread.h>
#include "openthread_port.h"
#include "br_thread_activation.h"
#include <string.h>

static void wipe(void *data, size_t length)
{
    volatile unsigned char *p = data;
    while (length--) *p++ = 0;
}

static otError acquire(otInstance **instance)
{
    *instance = otrGetInstanceIfReady();
    if (!*instance) return OT_ERROR_INVALID_STATE;
    return otrTryLock() ? OT_ERROR_NONE : OT_ERROR_BUSY;
}

otError ipro_matter_br_thread_read(ipro_matter_br_thread_state *output)
{
    if (!output) return OT_ERROR_INVALID_ARGS;
    memset(output, 0, sizeof(*output));
    otInstance *instance;
    otError error = acquire(&instance);
    if (error != OT_ERROR_NONE) return error;
    otBorderAgentId id = {0};
    error = otBorderAgentGetId(instance, &id);
    const char *name = NULL;
    size_t name_length = 0;
    if (error == OT_ERROR_NONE) {
        name = otBorderAgentGetMeshCoPServiceName(instance);
        if (name) while (name_length < sizeof(output->service_name) && name[name_length]) ++name_length;
        if (!name_length || name_length >= sizeof(output->service_name)) error = OT_ERROR_PARSE;
    }
    if (error == OT_ERROR_NONE) {
        output->version = otThreadGetVersion();
        output->interface_enabled = otIp6IsEnabled(instance);
        _Static_assert(sizeof(id.mId) == sizeof(output->border_agent_id), "Border Agent ID ABI");
        memcpy(output->border_agent_id, id.mId, sizeof(id.mId));
        memcpy(output->service_name, name, name_length);
    }
    otrUnlock();
    return error;
}

otError ipro_matter_br_thread_dataset_read(bool pending, otOperationalDatasetTlvs *output)
{
    if (!output) return OT_ERROR_INVALID_ARGS;
    wipe(output, sizeof(*output));
    otInstance *instance;
    otError error = acquire(&instance);
    if (error != OT_ERROR_NONE) return error;
    error = pending ? otDatasetGetPendingTlvs(instance, output) : otDatasetGetActiveTlvs(instance, output);
    otrUnlock();
    if (error == OT_ERROR_NONE && output->mLength > sizeof(output->mTlvs)) error = OT_ERROR_PARSE;
    if (error != OT_ERROR_NONE) wipe(output, sizeof(*output));
    else wipe(output->mTlvs + output->mLength, sizeof(output->mTlvs) - output->mLength);
    return error;
}

otError ipro_matter_br_thread_dataset_set_pending(const otOperationalDatasetTlvs *input)
{
    if (!input || input->mLength == 0 || input->mLength > sizeof(input->mTlvs)) return OT_ERROR_INVALID_ARGS;
    otInstance *instance;
    otError error = acquire(&instance);
    if (error != OT_ERROR_NONE) return error;
    if (!br_thread_activation_pending_allowed()) { otrUnlock(); return OT_ERROR_BUSY; }

    otOperationalDatasetTlvs copy = {0};
    otOperationalDataset parsed = {0};
    copy.mLength = input->mLength;
    memcpy(copy.mTlvs, input->mTlvs, copy.mLength);
    error = otDatasetParseTlvs(&copy, &parsed);
    if (error == OT_ERROR_NONE && (!parsed.mComponents.mIsActiveTimestampPresent ||
        !parsed.mComponents.mIsPendingTimestampPresent || !parsed.mComponents.mIsDelayPresent)) {
        error = OT_ERROR_INVALID_ARGS;
    }
    if (error == OT_ERROR_NONE && !otDatasetIsCommissioned(instance)) error = OT_ERROR_INVALID_STATE;
    if (error == OT_ERROR_NONE) error = otDatasetSetPendingTlvs(instance, &copy);
    wipe(&parsed, sizeof(parsed));
    wipe(&copy, sizeof(copy));
    otrUnlock();
    return error;
}
