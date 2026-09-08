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
 * @file vidplay.h
 * @brief Play a packed clip off the SD card: RGB565 to the panel, PCM to the DAC.
 *
 * The board has no video decoder and the panel is behind a 16 MHz SPI bus, so
 * nothing is decoded here. tools/mkvid.py does the scaling, the RGB565
 * conversion and the byte order ahead of time, and what is on the card is
 * exactly what gets pushed to the panel - playback is a read and a blit.
 *
 * That SPI bus is the ceiling: 320x240x16 bits is 1.23 Mbit, so a full frame
 * cannot be sent faster than about 77 ms, which puts the panel's own limit near
 * 13 fps before anything else is counted.
 */
#ifndef VIDPLAY_H
#define VIDPLAY_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool     playing;
    uint16_t width, height, fps, arate;
    uint32_t frames_total;
    uint32_t frames_shown;
    uint32_t late;        /* frames that missed their slot            */
    uint32_t achieved_mfps; /* measured rate, milli-fps               */
    uint32_t read_us;     /* average card read per frame              */
    uint32_t blit_us;     /* average panel push per frame             */
} vidplay_status_t;

/** @brief Play "<n>.ipv" from the card root. Mounts if needed. */
int  vidplay_start(int index);
void vidplay_stop(void);

/** @brief Free the cached payload buffer. It is kept between plays on purpose
 *         - see vid_reserve() - so this is only for reclaiming the PSRAM. */
void vidplay_release(void);

/** @brief Watch the card-detect pin; on insertion, mount and play the clips.
 *         Enabled by default - main() starts the watcher at boot. */
void vidplay_auto_enable(bool on);
bool vidplay_auto_enabled(void);

/** @brief Keep replaying the list while the card stays in. On by default. */
void vidplay_auto_loop(bool on);

/** @brief Pin autoplay to one clip index, or -1 to walk the whole list.
 *         Defaults to CONFIG_EVK_VIDEO_CLIP when the build sets it. */
void vidplay_set_clip(int index);
int  vidplay_get_clip(void);

/** @brief Play with no sound: the amplifier stays off and the DAC is fed
 *         silence. The DAC is still opened and written to, because its ring is
 *         what paces the frame loop. Defaults to CONFIG_EVK_VIDEO_MUTE. */
void vidplay_set_mute(bool on);
bool vidplay_muted(void);
bool vidplay_running(void);
void vidplay_get_status(vidplay_status_t *out);

#endif /* VIDPLAY_H */
