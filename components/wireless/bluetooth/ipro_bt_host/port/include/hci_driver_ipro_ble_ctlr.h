/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_HCI_DRIVER_H_
#define IPRO_BT_HOST_HCI_DRIVER_H_

#include <stdbool.h>
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

#if defined(CONFIG_BT_CTLR_AIC8800_SDIO)
struct aic_hci_stats;
/* AIC8800 SDIO mailbox transport (port/hci_aic_sdio.c). True once the host has
 * attached and the transport has not reported a fault; after a fault the
 * Controller needs a real re-initialisation (module power cycle + aic_start). */
bool ipro_bt_hci_aic_opened(void);
/* The vendor transport's own counters (see aic_hci_transport.h). */
void ipro_bt_hci_aic_stats(struct aic_hci_stats *out);
/* Bring-up capture: record the next `frames` ACL frames (both directions) into
 * a RAM ring without printing; dump them later from a shell context. */
void ipro_bt_hci_aic_trace_arm(uint32_t frames);
void ipro_bt_hci_aic_trace_dump(void);
#endif

extern volatile uint32_t ipro_bt_host_h4_sco_rx_count;
extern volatile uint32_t ipro_bt_host_h4_sco_rx_bytes;
extern volatile uint32_t ipro_bt_host_h4_sco_rx_no_callback_count;
extern volatile uint32_t ipro_bt_host_h4_sco_tx_count;
extern volatile uint32_t ipro_bt_host_h4_sco_tx_bytes;

#endif
