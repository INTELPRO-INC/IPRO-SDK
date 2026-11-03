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
 * @file reef_mode.h
 * @brief Which Teach the combined firmware runs, kept in flash across reboots.
 *
 * The mfg partition of partition_reef_teach.toml (offsets from its start):
 *
 *   0x000000  4 KB   partition header (the flash tool's)
 *   0x001000  4 KB   mode sector: "RTMD", mode, ~mode
 *   0x002000  448 KB voice   slot  (model_b.spkb)          VOICE_MFG_*
 *   0x072000  512 KB image   slot  (.inm + TCH1 labels)    TEACH_MFG_*
 *   0x0F2000  256 KB sensor  slot  (.inm + SNS1 block)     SENSOR_MFG_*
 *   0x132000  140 KB free (the partition is 0x155000)
 *
 * Each slot's last 4 KB is the model store's record (components/ai/store),
 * so a slot takes a model of its size less 4 KB (voice 444 KB: the b3 model
 * is 437 KB).
 *
 * The slot offsets and sizes are passed to the three Teach apps' sources by
 * CMakeLists.txt; on their own they keep using mfg + 0x1000 to the end.
 * A blank or unreadable mode sector means image mode.
 */
#ifndef REEF_MODE_H
#define REEF_MODE_H

typedef enum {
    REEF_MODE_IMAGE = 0,
    REEF_MODE_VOICE,
    REEF_MODE_SENSOR,
    REEF_MODE_COUNT,
} reef_mode_t;

/** The mode stored in flash (read once at boot). */
reef_mode_t reef_mode_load(void);
/** The mode this boot runs (what reef_mode_load() returned). */
reef_mode_t reef_mode_current(void);
const char *reef_mode_name(reef_mode_t mode);

/* The three Teach apps' links call these (weak defaults there):
 * reef_teach_mode()  - the mode running now
 * reef_teach_modes() - "image voice sensor"
 * reef_teach_switch(name) - store the mode and restart into it in ~300 ms;
 *                      0, -2 for an unknown name, -3 if flash failed */
const char *reef_teach_mode(void);
const char *reef_teach_modes(void);
int reef_teach_switch(const char *mode);

#endif /* REEF_MODE_H */
