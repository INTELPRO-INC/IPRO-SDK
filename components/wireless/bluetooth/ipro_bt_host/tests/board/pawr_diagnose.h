/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_PAWR_DIAGNOSE_H
#define IPRO_PAWR_DIAGNOSE_H
#include "pawr_pattern.h"
struct pawr_diagnosis {
    unsigned length, expected_length, encoded_sub, sequence, header_length;
    bool self_valid;
    int first_diff;
    unsigned actual, expected;
};
static inline struct pawr_diagnosis pawr_diagnose(const uint8_t *bytes,
                                                unsigned length, unsigned sub)
{
    struct pawr_diagnosis d = {
        .length = length, .expected_length = pawr_pattern_size(sub),
        .encoded_sub = 255, .first_diff = -1,
    };
    if (!bytes || length < 4) return d;
    d.sequence = bytes[2] | ((unsigned)bytes[3] << 8);
    d.header_length = bytes[0] | ((unsigned)bytes[1] << 8);
    d.encoded_sub = d.sequence & 3;
    /* Independent check: could an intact packet be attributed to the wrong
     * subevent? This diagnostic never changes the original acceptance gate. */
    d.self_valid = pawr_pattern_check(bytes, length, d.encoded_sub);
    for (unsigned i = 0; i < length && i < PAWR_PATTERN_MAX; i++) {
        unsigned expected = coc_pattern_byte(i, d.expected_length, d.sequence);
        if (bytes[i] != expected) {
            d.first_diff = (int)i;
            d.actual = bytes[i];
            d.expected = expected;
            break;
        }
    }
    return d;
}
#endif
