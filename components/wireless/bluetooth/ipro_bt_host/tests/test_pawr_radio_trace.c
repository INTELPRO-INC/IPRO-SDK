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

#include <assert.h>
#include "../../ipro_ble_controller/include/ipro_pawr_radio_trace.h"
int main(void) {
    struct ipro_pawr_trace t = {0};
    ipro_pawr_trace_time(&t, 1, 2, 3, 4, 5);
    ipro_pawr_trace_packet(&t, 247, 7, 36);
    ipro_pawr_trace_mark(&t, 2, 0);
    assert(t.count == 0);
    ipro_pawr_trace_begin(&t, 372, 13, 0);
    assert(t.rows[0].time_valid == 0);
    ipro_pawr_trace_time(&t, 20, 624, 10, 1, 100);
    assert(t.rows[0].time_valid && t.rows[0].start_hs == 20);
    assert(t.rows[0].start_hus == 624 && t.rows[0].ref_hs == 10);
    assert(t.rows[0].ref_hus == 1 && t.rows[0].window_hus == 100);
    assert(t.rows[0].channel == 255);
    ipro_pawr_trace_packet(&t, 247, 7, 36);
    ipro_pawr_trace_packet(&t, 0, 0, 1);
    assert(t.rows[0].length == 247 && t.rows[0].type == 7 && t.rows[0].channel == 36);
    assert(t.rows[0].packets == 2);
    ipro_pawr_trace_mark(&t, 2, 0);
    ipro_pawr_trace_mark(&t, 8, 3);
    assert(t.rows[0].seq == 372 && t.rows[0].event == 13);
    assert(t.rows[0].phases == 11 && t.rows[0].irq == 3);
    ipro_pawr_trace_mark(&t,16,0);
    assert(t.rows[0].phases == 27);
    ipro_pawr_trace_begin(&t,373,13,1);
    ipro_pawr_trace_mark(&t,4,0);
    assert(t.rows[1].phases == 5 && t.rows[1].irq == 255);
    for (unsigned i=2;i<64;i++) ipro_pawr_trace_begin(&t,i,i,3);
    ipro_pawr_trace_begin(&t,999,999,0);
    ipro_pawr_trace_time(&t, 1, 2, 3, 4, 5);
    assert(t.rows[63].time_valid == 0);
    ipro_pawr_trace_packet(&t, 999, 7, 36);
    assert(t.rows[63].packets == 0);
    ipro_pawr_trace_mark(&t,4,0);
    assert(t.count==64 && t.dropped==1 && t.rows[63].phases==1);
    ipro_pawr_trace_rx(&t, 0x20, 1);
    assert(t.rows[63].seq == 63);
    t.count=0; t.current=-1;
    ipro_pawr_trace_rx(&t, 0x20, 1);
    ipro_pawr_trace_begin(&t,65535,12,2);
    ipro_pawr_trace_rx(&t,0,0);
    assert(t.rows[0].seq==0 && t.rows[0].phases==33);
    ipro_pawr_trace_rx(&t,0x20,1);
    assert(t.rows[0].seq==0x20 && t.rows[0].phases==97);
    return 0;
}
