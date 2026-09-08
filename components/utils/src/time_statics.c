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

#include <stdint.h>
#include <string.h>

#include "time_statics.h"

#if defined (TICKLESS_RECORD)
uint64_t g_ts_record[TS_MAX][5] = {0};
#endif

#if defined (TICKLESS_RECORD)
#define DUMP_RECORD_ITEM(x) printf( "[TSR]: %s:%lld,%lld,%lld,%lld,%lld \r\n", #x, g_ts_record[x][0], g_ts_record[x][1], g_ts_record[x][2], g_ts_record[x][3], g_ts_record[x][4])
#else
#define DUMP_RECORD_ITEM(x)
#endif

void time_static_record_dump(void) {
#define _X(x) DUMP_RECORD_ITEM(x);
  TS_STUBS
#undef _X
#ifdef TICKLESS_RECORD
  memset(g_ts_record, 0, sizeof(g_ts_record));
#endif
}
