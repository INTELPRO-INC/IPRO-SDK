/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ADI_PATTERN_H_
#define IPRO_BT_HOST_ADI_PATTERN_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define IPRO_ADI_PATTERN_LEN 24U

static inline void ipro_adi_pattern_fill(uint8_t payload[IPRO_ADI_PATTERN_LEN],
                                         uint16_t sequence)
{
    payload[0] = 'I';
    payload[1] = 'A';
    payload[2] = 'D';
    payload[3] = 'I';
    payload[4] = (uint8_t)sequence;
    payload[5] = (uint8_t)(sequence >> 8);
    for (unsigned int i = 6; i < IPRO_ADI_PATTERN_LEN; i++) {
        payload[i] = (uint8_t)(0x5aU ^ i ^ sequence ^ (sequence >> 8));
    }
}

static inline bool ipro_adi_pattern_check(const uint8_t *payload, size_t length,
                                          uint16_t *sequence)
{
    uint8_t expected[IPRO_ADI_PATTERN_LEN];

    if ((payload == NULL) || (sequence == NULL) ||
        (length != IPRO_ADI_PATTERN_LEN) ||
        (memcmp(payload, "IADI", 4U) != 0)) {
        return false;
    }
    *sequence = (uint16_t)payload[4] | ((uint16_t)payload[5] << 8);
    ipro_adi_pattern_fill(expected, *sequence);
    return memcmp(payload, expected, sizeof(expected)) == 0;
}

#endif
