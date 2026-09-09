/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_PAWR_PATTERN_H
#define IPRO_PAWR_PATTERN_H
#include "coc_pattern.h"
#define PAWR_PATTERN_SUBEVENTS 4
#define PAWR_PATTERN_MAX 247
static bool pawr_long_payloads;
static inline unsigned pawr_pattern_size(unsigned sub)
{
    static const unsigned lengths[PAWR_PATTERN_SUBEVENTS] = {24, 40, 64, 120};
    return sub < PAWR_PATTERN_SUBEVENTS ?
        (pawr_long_payloads ? 244 + sub : lengths[sub]) : 0;
}
static inline bool pawr_pattern_fill(uint8_t *data, unsigned capacity,
                                     unsigned sub, uint16_t generation)
{
    unsigned length = pawr_pattern_size(sub);
    if (!data || !length || capacity < length) return false;
    coc_pattern_fill(data, length, (uint16_t)((generation << 2) | sub));
    return true;
}
static inline bool pawr_pattern_check(const uint8_t *data, unsigned length, unsigned sub)
{
    unsigned expected = pawr_pattern_size(sub);
    if (!data || !expected || length != expected) return false;
    uint16_t sequence = (uint16_t)(data[2] | ((uint16_t)data[3] << 8));
    return (sequence & 3) == sub && coc_pattern_check(data, length, sequence);
}
#endif
