/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_HCI_EVENT_BOUNDS_H
#define IPRO_HCI_EVENT_BOUNDS_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Tailroom is measured after bt_buf_get_evt added the H4 type byte.
 * The two-byte event header and full parameter payload still need space. */
static inline bool ipro_hci_event_fits(size_t tailroom, uint8_t payload_len)
{
    return tailroom >= 2u + (size_t)payload_len;
}
#endif
