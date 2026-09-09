/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_PAWR_LEDGER_H
#define IPRO_PAWR_LEDGER_H
#include <stdint.h>
#include <string.h>
enum { PAWR_LEDGER_TX, PAWR_LEDGER_RSP, PAWR_LEDGER_RX, PAWR_LEDGER_EMPTY,
       PAWR_LEDGER_KINDS, PAWR_LEDGER_CAPACITY = 64 };
struct pawr_ledger_entry { uint16_t seq, event; int16_t rc; uint8_t sub; };
struct pawr_ledger {
    struct pawr_ledger_entry rows[PAWR_LEDGER_KINDS][PAWR_LEDGER_CAPACITY];
    unsigned count[PAWR_LEDGER_KINDS], dropped[PAWR_LEDGER_KINDS];
};
/* Each kind has one serialized callback writer; read/reset only after both
 * fixture admission gates are closed and drained. Freeze, never overwrite. */
static inline void pawr_ledger_reset(struct pawr_ledger *ledger)
{
    memset(ledger, 0, sizeof(*ledger));
}
static inline void pawr_ledger_add(struct pawr_ledger *ledger, unsigned kind,
                                  uint16_t seq, uint16_t event, uint8_t sub, int16_t rc)
{
    if (kind >= PAWR_LEDGER_KINDS) return;
    unsigned index = ledger->count[kind];
    if (index == PAWR_LEDGER_CAPACITY) { ledger->dropped[kind]++; return; }
    ledger->rows[kind][index] = (struct pawr_ledger_entry){seq, event, rc, sub};
    ledger->count[kind] = index + 1;
}
#endif
