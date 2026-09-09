/* SPDX-License-Identifier: Apache-2.0 */
/* Bench-only bounded trace. Helpers do no MMIO or logging. */
#ifndef IPRO_PAWR_RADIO_TRACE_H
#define IPRO_PAWR_RADIO_TRACE_H
#include <stdint.h>
struct ipro_pawr_trace_row {
    uint16_t seq, event;
    uint8_t sub, phases, irq, channel;
    uint16_t length;
    uint8_t type, packets;
    uint32_t start_hs, ref_hs, window_hus;
    uint16_t start_hus, ref_hus;
    uint8_t time_valid;
};
struct ipro_pawr_trace {
    struct ipro_pawr_trace_row rows[64];
    unsigned count, dropped;
    int current;
};
/* Software scheduling snapshot, not an observed RF timestamp. hs=312.5us,
 * hus=0.5us elapsed. Reference semantics are role-specific; never subtract
 * absolute clocks from different boards. */
static inline void ipro_pawr_trace_time(struct ipro_pawr_trace *t,
    uint32_t hs, uint16_t hus, uint32_t ref_hs, uint16_t ref_hus, uint32_t window_hus)
{
    if (!t->count || t->current < 0) return;
    struct ipro_pawr_trace_row *r = &t->rows[t->current];
    r->start_hs=hs; r->start_hus=hus;
    r->ref_hs=ref_hs; r->ref_hus=ref_hus;
    r->window_hus=window_hus; r->time_valid=1;
}
static inline void ipro_pawr_trace_begin(struct ipro_pawr_trace *t,
                                        uint16_t seq, uint16_t event, uint8_t sub)
{
    t->current = -1;
    if (t->count == 64) { t->dropped++; return; }
    t->current = t->count++;
    t->rows[t->current] = (struct ipro_pawr_trace_row){
        .seq=seq, .event=event, .sub=sub, .phases=1, .irq=255, .channel=255
    };
}
/* First descriptor metadata only; status remains OR across all descriptors.
 * packets > 1 means status cannot be attributed to this descriptor alone. */
static inline void ipro_pawr_trace_packet(struct ipro_pawr_trace *t,
                                         uint16_t length, uint8_t type, uint8_t channel)
{
    if (!t->count || t->current < 0) return;
    struct ipro_pawr_trace_row *r = &t->rows[t->current];
    if (!r->packets) { r->length=length; r->type=type; r->channel=channel; }
    if (r->packets != 255) r->packets++;
}
static inline void ipro_pawr_trace_mark(struct ipro_pawr_trace *t, uint8_t phase, uint8_t irq)
{
    if (!t->count || t->current < 0) return;
    t->rows[t->current].phases |= phase;
    if (phase == 8) t->rows[t->current].irq = irq;
}
/* Advertiser trace reuses seq for OR of already-read RX status words.
 * 32=descriptor observed;64=at least one descriptor rejected by existing gate. */
static inline void ipro_pawr_trace_rx(struct ipro_pawr_trace *t, uint16_t status, int rejected)
{
    if (!t->count || t->current < 0) return;
    struct ipro_pawr_trace_row *row = &t->rows[t->current];
    row->seq = (row->phases & 32) ? (row->seq | status) : status;
    row->phases |= 32 | (rejected ? 64 : 0);
}
int ipro_pawr_advrx_trace_get(unsigned index, struct ipro_pawr_trace_row *row, unsigned *dropped);
void ipro_pawr_advrx_trace_reset(void);
/* Call only from the stopped bench fixture. Returns1 for a row,0 for end.
 * phase bits:1 accepted,2 start,4 canceled,8 frame callback,16 TX callback.
 * A frame callback is NOT proof of successful RF transmission. */
int ipro_pawr_radio_trace_get(unsigned index, struct ipro_pawr_trace_row *row,
                             unsigned *dropped);
void ipro_pawr_radio_trace_reset(void);
#endif
