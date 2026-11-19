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

/* Test-only HDLC corruption scheduler, used only by the opt-in fault profile.
 * Caller owns complete encoded frames and serializes access (e.g. OT lock).
 * Counters describe submitted frames, not independent wire delivery. */
#ifndef IPRO_SPINEL_FAULT_INJECTOR_HPP
#define IPRO_SPINEL_FAULT_INJECTOR_HPP
#include <stddef.h>
#include <stdint.h>

class IproSpinelFaultInjector
{
public:
    struct Stats {
        bool active = false;
        uint32_t seed = 0;
        uint64_t startedUs = 0;
        uint64_t deadlineUs = 0;
        uint64_t stoppedObservedUs = 0;
        uint64_t frames = 0;
        uint64_t corrupted = 0;
        uint64_t malformed = 0;
    };
    static constexpr uint64_t kDurationUs = UINT64_C(300000000);
    bool Start(uint64_t now, uint32_t seed)
    {
        Refresh(now);
        if (mStats.active || now > UINT64_MAX - kDurationUs) return false;
        mStats = Stats{};
        mStats.active = true;
        mStats.seed = seed;
        mStats.startedUs = now;
        mStats.deadlineUs = now + kDurationUs;
        return true;
    }
    void Stop(uint64_t now)
    {
        if (mStats.active) {
            mStats.active = false;
            mStats.stoppedObservedUs = now;
        }
    }
    Stats Snapshot(uint64_t now) { Refresh(now); return mStats; }

    // Input must be the completed output of an HDLC encoder. Changes a single
    // encoded data/FCS bit while retaining flag and escape structure. No FCS
    // recomputation. Phase seed%10 selects exactly one frame in each group of
    // ten; partial groups are explicitly represented by the counters.
    bool Apply(uint8_t *frame, size_t length, uint64_t now)
    {
        Refresh(now);
        if (!mStats.active) return false;
        if (!frame || length < 4 || frame[0] != 0x7e || frame[length - 1] != 0x7e) {
            ++mStats.malformed;
            return false;
        }
        size_t index = frame[1] == 0x7d ? 2 : 1;
        if (index >= length - 1 || Reserved(frame[index])) {
            ++mStats.malformed;
            return false;
        }
        ++mStats.frames;
        if ((mStats.frames - 1 + mStats.seed % 10) % 10 != 9) return false;
        for (unsigned bit = 0; bit < 8; ++bit) {
            uint8_t changed = static_cast<uint8_t>(frame[index] ^ (1u << bit));
            if (!Reserved(changed)) {
                frame[index] = changed;
                ++mStats.corrupted;
                return true;
            }
        }
        return false; // At most five reserved values cannot cover eight flips.
    }
private:
    static bool Reserved(uint8_t byte)
    {
        return byte == 0x7e || byte == 0x7d || byte == 0x11 || byte == 0x13 || byte == 0xf8;
    }
    void Refresh(uint64_t now)
    {
        if (mStats.active && now >= mStats.deadlineUs) Stop(now);
    }
    Stats mStats;
};
#endif
