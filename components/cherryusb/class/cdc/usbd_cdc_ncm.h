/*
 * Copyright (c) 2025, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBD_CDC_NCM_H
#define USBD_CDC_NCM_H

#include "usb_cdc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* NTB overhead: NTH16(12) + NDP16(16) = 28 bytes */
#define CDC_NCM_NTB_OVERHEAD    28
/* Ethernet Maximum Segment size, typically 1514 bytes */
#define CONFIG_CDC_NCM_ETH_MAX_SEGSZE 1514U
/* NTB max size: configurable, default 8K for multi-datagram aggregation */
#ifndef CONFIG_CDC_NCM_NTB_MAX_SIZE
#define CONFIG_CDC_NCM_NTB_MAX_SIZE   8192U
#endif

// #define CONFIG_USBDEV_CDC_NCM_USING_LWIP

/* Init cdc ncm interface driver */
struct usbd_interface *usbd_cdc_ncm_init_intf(struct usbd_interface *intf,
                                              const uint8_t int_ep,
                                              const uint8_t out_ep,
                                              const uint8_t in_ep);

/* Setup request command callback api */
void usbd_cdc_ncm_set_connect_speed(uint32_t speed[2]);

int usbd_cdc_ncm_send_connect_status(bool connected);
uint8_t *usbd_cdc_ncm_get_tx_buffer(void);
int usbd_cdc_ncm_send_packet(uint8_t *buf, uint32_t len);
void usbd_cdc_ncm_data_send_done(void);
void usbd_cdc_ncm_data_recv_done(uint8_t *buf, uint32_t len);
void usbd_cdc_ncm_set_interface_done(void);
void usbd_cdc_ncm_start_read_next(void);
int usbd_cdc_ncm_tx_flush(void);
int usbd_cdc_ncm_tx_recover_stale(uint32_t timeout_ms);

#ifdef CONFIG_USBDEV_CDC_NCM_USING_LWIP
#include <lwip/pbuf.h>
struct pbuf *usbd_cdc_ncm_eth_rx(void);
int usbd_cdc_ncm_eth_tx(struct pbuf *p);
#endif

#ifdef __cplusplus
}
#endif

#endif /* USBD_CDC_NCM_H */
