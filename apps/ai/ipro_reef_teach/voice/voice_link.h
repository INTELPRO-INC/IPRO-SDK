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
 * @file voice_link.h
 * @brief The Teach page's line protocol over a CDC-ACM port on the Reef's USB-C.
 *
 * Host lines (UTF-8, '\n'-terminated) become voice_*() requests; the voice
 * task's event lines go back the same way. EMBED <bytes> is followed by that
 * many bytes of raw 16 kHz int16 PCM. The commands and replies are listed in
 * README.md.
 */

#ifndef VOICE_LINK_H
#define VOICE_LINK_H

#include <stdbool.h>

/** Register the CDC-ACM device and start the link task. 0, or -1. */
int voice_link_start(void);

/** One line to the host; dropped when no host has the port open. */
void voice_link_send(const char *line);

void voice_link_print(void);

#endif /* VOICE_LINK_H */
