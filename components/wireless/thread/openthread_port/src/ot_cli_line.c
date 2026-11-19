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

#include "ot_cli_line.h"

void ot_cli_line_flush(struct ot_cli_line *line, void (*emit)(const char *))
{
    if (line->used == 0) return;
    line->data[line->used] = '\0';
    emit(line->data);
    line->used = 0;
}

void ot_cli_line_write(struct ot_cli_line *line, const char *text,
                       void (*emit)(const char *))
{
    while (*text) {
        char ch = *text++;
        line->data[line->used++] = ch;
        if (ch == '\n') {
            ot_cli_line_flush(line, emit);
        } else if (line->used == sizeof(line->data) - 1) {
            line->overflow_flushes++;
            ot_cli_line_flush(line, emit);
        }
    }
}
