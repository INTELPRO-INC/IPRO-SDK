/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_HCI_DRIVER_H_
#define IPRO_BT_HOST_HCI_DRIVER_H_

#include <stdint.h>

/* Zephyr 4.4 binds the HCI device at build time. Kept for source compatibility
 * with applications that explicitly initialized the legacy registered driver. */
int hci_driver_ipro_ble_ctlr_init(void);

/**
 * Callback for one complete HCI Synchronous Data packet received from the
 * controller. The payload pointer remains valid only until the callback
 * returns. Keep the callback bounded; it runs in the HCI RX adapter task.
 */
typedef void (*ipro_bt_hci_sco_rx_cb_t)(uint16_t handle,
                                       uint8_t packet_status,
                                       const uint8_t *payload,
                                       uint8_t len,
                                       void *user_data);

/** Register or clear the application-owned SCO/eSCO audio receive callback. */
int ipro_bt_hci_sco_rx_register(ipro_bt_hci_sco_rx_cb_t callback,
                                void *user_data);

/**
 * Send one complete HCI Synchronous Data packet to the controller.
 *
 * @param handle        12-bit synchronous connection handle.
 * @param packet_status HCI packet-status flag (0..3).
 * @param payload       Audio payload, or NULL when len is zero.
 * @param len           Payload length (0..255).
 */
int ipro_bt_hci_sco_send(uint16_t handle, uint8_t packet_status,
                         const uint8_t *payload, uint8_t len);

extern volatile uint32_t ipro_bt_host_h4_sco_rx_count;
extern volatile uint32_t ipro_bt_host_h4_sco_rx_bytes;
extern volatile uint32_t ipro_bt_host_h4_sco_rx_no_callback_count;
extern volatile uint32_t ipro_bt_host_h4_sco_tx_count;
extern volatile uint32_t ipro_bt_host_h4_sco_tx_bytes;

#endif
