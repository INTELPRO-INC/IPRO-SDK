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

#ifndef IPRO_OT_CLI_LINE_H
#define IPRO_OT_CLI_LINE_H

#include <stddef.h>
#include <stdint.h>

#define OT_CLI_LINE_CAPACITY 512

/* OT-task owned; emit must consume the text synchronously, without reentry.
 * Lines up to capacity-1 bytes (including LF) are emitted in one call.
 * Longer lines are streamed in bounded chunks, not dropped or atomic.
 * Explicit flush makes prompts/partial synchronous output visible. */
struct ot_cli_line {
    char data[OT_CLI_LINE_CAPACITY];
    size_t used;
    uint32_t overflow_flushes;
};

void ot_cli_line_write(struct ot_cli_line *line, const char *text,
                       void (*emit)(const char *));
void ot_cli_line_flush(struct ot_cli_line *line, void (*emit)(const char *));

#endif
