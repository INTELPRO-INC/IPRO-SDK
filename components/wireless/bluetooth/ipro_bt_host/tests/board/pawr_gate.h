/* SPDX-License-Identifier: Apache-2.0 */
#ifndef PAWR_GATE_H
#define PAWR_GATE_H
#ifndef PAWR_GATE_ATOMICS_PROVIDED
#include <zephyr/sys/atomic.h>
#endif
/* Bit 0 closes admission; each admitted callback contributes two. The shell
 * closes, drains, then issues stop/delete. Never wait in a Host callback. */
#define PAWR_GATE_CLOSED 1
static inline bool pawr_gate_enter(atomic_t *gate)
{
    atomic_val_t state = atomic_get(gate);
    while (!(state & PAWR_GATE_CLOSED)) {
        if (atomic_cas(gate, state, state + 2)) return true;
        state = atomic_get(gate);
    }
    return false;
}
static inline void pawr_gate_leave(atomic_t *gate) { atomic_sub(gate, 2); }
static inline void pawr_gate_close(atomic_t *gate) { atomic_or(gate, PAWR_GATE_CLOSED); }
static inline bool pawr_gate_drained(atomic_t *gate)
{ return atomic_get(gate) == PAWR_GATE_CLOSED; }
static inline bool pawr_gate_open(atomic_t *gate)
{ return atomic_cas(gate, PAWR_GATE_CLOSED, 0); }
#endif
