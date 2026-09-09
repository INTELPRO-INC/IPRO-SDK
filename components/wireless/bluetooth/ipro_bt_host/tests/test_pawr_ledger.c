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
#include "board/pawr_ledger.h"
int main(void)
{
    struct pawr_ledger ledger = {0};
    for (unsigned i = 0; i < 70; i++)
        pawr_ledger_add(&ledger, PAWR_LEDGER_RX, i, i + 100, i % 4, -5);
    assert(ledger.count[PAWR_LEDGER_RX] == 64);
    assert(ledger.dropped[PAWR_LEDGER_RX] == 6);
    for (unsigned i = 0; i < 64; i++) {
        const struct pawr_ledger_entry *e = &ledger.rows[PAWR_LEDGER_RX][i];
        assert(e->seq == i && e->event == i + 100 && e->sub == i % 4 && e->rc == -5);
    }
    pawr_ledger_add(&ledger, PAWR_LEDGER_RSP, 65535, 65535, 3, 0);
    assert(ledger.count[PAWR_LEDGER_RSP] == 1);
    assert(ledger.rows[PAWR_LEDGER_RSP][0].seq == 65535);
    pawr_ledger_add(&ledger, PAWR_LEDGER_KINDS, 1, 1, 0, 0);
    pawr_ledger_reset(&ledger);
    for (unsigned i = 0; i < PAWR_LEDGER_KINDS; i++)
        assert(ledger.count[i] == 0 && ledger.dropped[i] == 0);
    return 0;
}
