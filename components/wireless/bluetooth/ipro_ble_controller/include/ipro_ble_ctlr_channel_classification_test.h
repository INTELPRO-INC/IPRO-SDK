/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BLE_CTLR_CHANNEL_CLASSIFICATION_TEST_H_
#define IPRO_BLE_CTLR_CHANNEL_CLASSIFICATION_TEST_H_

#include <stddef.h>
#include <stdint.h>

/* Test-only deterministic input for the BLE 5.3 peripheral channel
 * classification procedure. This API is absent unless the corresponding
 * opt-in test source is linked; it is not part of customer controller ABI. */
int ipro_ble_ctlr_ch_class_test_set_bad(uint8_t channel);
int ipro_ble_ctlr_ch_class_test_clear(void);
int ipro_ble_ctlr_ch_class_test_get(uint8_t *classification, size_t length);
uint32_t ipro_ble_ctlr_ch_class_test_get_count(void);

#endif
