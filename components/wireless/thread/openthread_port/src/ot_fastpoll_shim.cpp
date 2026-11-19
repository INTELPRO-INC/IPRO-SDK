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

/**
 * @file ot_fastpoll_shim.cpp
 * @brief Bridge from C fast-poll module into OT internals
 *
 * See ot_fastpoll_shim.h for the locking contract. Caller MUST hold the
 * OT external lock; we do not take it here. (Initial Phase 1 design did
 * take it internally, but `ot_extLock` is a non-recursive mutex held by
 * the OT task across tasklet+driver processing, so any call from a
 * state-changed callback or tasklet — which is the most natural entry
 * point for the SED fast-path — would deadlock. Concretely observed on
 * 2026-05-19: smoke dump in `ot_state_changed_cb` hung silently after
 * `Thread role: Child`.)
 *
 * Plan A reference: .claude/plans/sed-fastpoll-bypass-ot-mac.md
 *
 * Built in literal-key mode only. If
 * OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE is ever turned on
 * for this build, the snapshot path needs to ExtractKey() via a key
 * usage policy that allows export — out of scope here.
 */

/* OPENTHREAD_FTD / OPENTHREAD_MTD / OPENTHREAD_RADIO are set via the
 * openthread_port CMakeLists.txt based on Kconfig. */

#include "ot_fastpoll_shim.h"

#include <stdio.h>
#include <string.h>

#include "instance/instance.hpp"
#include "thread/key_manager.hpp"
#include "thread/mle.hpp"
#include "mac/mac.hpp"
#include "mac/sub_mac.hpp"
#include "mac/data_poll_sender.hpp"
#include "common/as_core_type.hpp"

#include <openthread/instance.h>
#include <openthread/thread.h>

#include "openthread_port.h"

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
#error "ot_fastpoll_shim.cpp assumes literal MAC keys (PSA key refs not supported here)"
#endif

#if !OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
#error "ot_fastpoll_shim.cpp requires 802.15.4 radio link"
#endif

using namespace ot;

extern "C" otInstance *otrGetInstance(void);

namespace {

inline Instance *FastpollInstance(void)
{
    otInstance *otInst = otrGetInstance();
    if (otInst == nullptr) {
        return nullptr;
    }
    return &AsCoreType(otInst);
}

} // namespace

extern "C" int ot_fastpoll_snapshot(ot_fastpoll_state_t *out)
{
    if (out == nullptr) {
        return -1;
    }

    Instance *inst = FastpollInstance();
    if (inst == nullptr) {
        return -1;
    }

    Mac::Mac   &mac        = inst->Get<Mac::Mac>();
    KeyManager &keyManager = inst->Get<KeyManager>();
    Mle::Mle   &mle        = inst->Get<Mle::Mle>();

    /* Identity */
    out->pan_id    = mac.GetPanId();
    out->our_short = mac.GetShortAddress();
    memcpy(out->our_ext, mac.GetExtAddress().m8, 8);

    /* Parent — only valid in Child role with a kStateValid parent. */
    out->parent_valid = false;
    out->parent_short = 0xfffe; /* invalid sentinel */
    memset(out->parent_ext, 0, 8);

    if (mle.IsChild()) {
        const Parent &parent = mle.GetParent();
        if (parent.IsStateValid()) {
            out->parent_valid = true;
            out->parent_short = parent.GetRloc16();
            memcpy(out->parent_ext, parent.GetExtAddress().m8, 8);
        }
    }

    /* NOTE: For class-layout correctness across CHIP-built OT core and
     * SDK-built shim, the shim is compiled with -std=gnu++17
     * -fno-short-enums (see openthread_port/CMakeLists.txt). SDK
     * defaults `-fshort-enums` globally; CHIP does not. Different
     * enum-storage rules silently shifted member offsets in Mle/Mac/
     * Neighbor — `Get<Type>()` returned wrong sub-object pointers and
     * `mle.GetRloc16()` read random bytes. Identified 2026-05-28. */

    /* Key material — literal 16-byte bytes (non-PSA build). The live
     * 802.15.4 MAC key lives in `SubMac::mCurrKey` (kept in sync with
     * KeyManager via `KeyManager::UpdateKeyMaterial` → `SubMac::SetMacKey`).
     * KeyManager itself does not expose a `GetCurrentMacKey` for 802.15.4
     * (it only has `GetCurrentTrelMacKey` / `GetCurrentMleKey`). */
    const Mac::KeyMaterial &curKey = inst->Get<Mac::SubMac>().GetCurrentMacKey();
    memcpy(out->mac_key, curKey.GetKey().GetBytes(), 16);
    out->key_sequence = keyManager.GetCurrentKeySequence();
    out->key_id       = static_cast<uint8_t>((out->key_sequence & 0x7f) + 1);

    /* Frame counter — next value to use. */
    out->mac_frame_counter = keyManager.Get154MacFrameCounter();

    /* Channel from OT's PAN channel. */
    out->channel = mac.GetPanChannel();

    return 0;
}

extern "C" int ot_fastpoll_commit_counter(uint32_t used_counter)
{
    Instance *inst = FastpollInstance();
    if (inst == nullptr) {
        return -1;
    }

    /* KeyManager::MacFrameCounterUsed bumps the authoritative counter
     * (next-to-use = used+1) and persists if it has crossed the stored
     * threshold. It is the same entry point SubMac::SignalFrameCounterUsed
     * eventually calls via the callback chain. */
    inst->Get<KeyManager>().MacFrameCounterUsed(used_counter);

    /* KeyManager::MacFrameCounterUsed does NOT update SubMac::mFrameCounter
     * (the value OT's own TX path reads when stamping outgoing frames).
     * Bump it explicitly with set-if-larger so we stay in sync without
     * regressing OT-initiated TXes that may have advanced it further. */
    inst->Get<Mac::SubMac>().SetFrameCounter(used_counter + 1, /*aSetIfLarger=*/true);

    return 0;
}

extern "C" bool ot_fastpoll_is_ready(void)
{
    Instance *inst = FastpollInstance();
    if (inst == nullptr) {
        return false;
    }

    Mle::Mle &mle = inst->Get<Mle::Mle>();
    return mle.IsChild() && mle.GetParent().IsStateValid();
}

extern "C" void ot_fastpoll_stop_ot_poll_sender(void)
{
    Instance *inst = FastpollInstance();
    if (inst == nullptr) {
        return;
    }
    inst->Get<DataPollSender>().StopPolling();
}

extern "C" void ot_fastpoll_start_ot_poll_sender(void)
{
    Instance *inst = FastpollInstance();
    if (inst == nullptr) {
        return;
    }
    /* StartPolling asserts !IsRxOnWhenIdle — only valid on a SED. */
    Mle::Mle &mle = inst->Get<Mle::Mle>();
    if (mle.IsRxOnWhenIdle()) {
        return;
    }
    inst->Get<DataPollSender>().StartPolling();
}
