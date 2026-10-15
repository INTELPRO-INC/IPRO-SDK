/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#ifndef AIC_HCI_TRANSPORT_H
#define AIC_HCI_TRANSPORT_H
#include <stdint.h>

/* HCI packet bytes EXCLUDE the H4 type byte. No L2CAP reassembly here.
 * All callbacks run on the transport worker, never an ISR; RX bytes are
 * borrowed until callback returns. Copy them if retaining. Callbacks must
 * not block, detach, or call synchronous diagnostic APIs. send() is allowed.
 * A successful send() copies bytes; tx_done reports mailbox submission,
 * NOT air delivery. Controller completion events remain visible unchanged.
 * APIs are task-context only. Init/attach/detach must be serialized by owner.
 */
#define AIC_HCI_COMMAND 1
#define AIC_HCI_ACL 2
#define AIC_HCI_EVENT 4
#define AIC_HCI_MAX_PACKET 1023
/* SDIO mailbox has a 1024-byte H4 envelope: effective ACL data MTU 1019.
 * Host adapter must clamp its ACL fragment size even if Controller reports
 * 1021; driver never secretly fragments (would break Host credit counts). */
#define AIC_HCI_MAX_ACL_DATA (AIC_HCI_MAX_PACKET-4)
#define AIC_HCI_TX_DEPTH 16
enum aic_hci_flow { AIC_HCI_FLOW_HOST, AIC_HCI_FLOW_DRIVER };
enum aic_hci_error { AIC_HCI_INVALID=-1, AIC_HCI_BUSY=-2,
    AIC_HCI_TIMEOUT=-3, AIC_HCI_DISCONNECTED=-4, AIC_HCI_FAULT=-5,
    AIC_HCI_CANCELLED=-6, AIC_HCI_OVERFLOW=-7 };
struct aic_hci_callbacks {
    void (*rx)(void *ctx, uint8_t type, const uint8_t *packet, uint16_t len);
    void (*tx_done)(void *ctx, uint32_t token, int status);
    void (*fault)(void *ctx, int status);
};
struct aic_hci_stats {
    uint32_t tx_packets, tx_bytes, rx_packets, rx_bytes;
    uint32_t rejected, expired, faults;
    uint16_t queued, outstanding, acl_available, acl_max_len;
    uint8_t attached, faulted, flow;
};
int aic_hci_transport_attach(const struct aic_hci_callbacks *cb, void *ctx,
                             enum aic_hci_flow flow);
/* Cancels queued packets and joins callback activity; no Controller reset.
 * Outstanding Controller state/active links make detach BUSY until
 * disconnect/reset (HOST mode relies on Host to await command completions).
 * On fault: detach is permitted, but MUST reset hardware/Controller before
 * attaching again. Do not invent returned credits after a timeout. */
int aic_hci_transport_detach(void);
int aic_hci_transport_send(uint8_t type, const uint8_t *packet, uint16_t len,
                           uint32_t token, uint32_t timeout_ms);
/* DRIVER mode: configure an exclusively BR/EDR ACL pool after reading
 * Controller buffer size, before sending ACL. LE/separate pools unsupported.
 * HOST mode: Host stack handles Command/ACL flow control itself. */
int aic_hci_transport_buffers(uint16_t acl_max_len, uint16_t acl_packets);
void aic_hci_transport_stats(struct aic_hci_stats *out);

#endif
