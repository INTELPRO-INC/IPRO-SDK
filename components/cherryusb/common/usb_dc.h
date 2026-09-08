/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USB_DC_H
#define USB_DC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USB Endpoint Configuration.
 *
 * Structure containing the USB endpoint configuration.
 */
struct usbd_endpoint_cfg {
    uint8_t ep_addr; /* Endpoint addr with direction */
    uint8_t ep_type; /* Endpoint type */
    uint16_t ep_mps; /* Endpoint max packet size */
    uint8_t ep_mult; /* Endpoint additional transcations in micro frame */
};

/**
 * @brief init device controller registers.
 * @return On success will return 0, and others indicate fail.
 */
int usb_dc_init(void);

/**
 * @brief deinit device controller registers.
 * @return On success will return 0, and others indicate fail.
 */
int usb_dc_deinit(void);

/**
 * @brief Set USB device address
 *
 * @param[in] addr Device address
 *
 * @return On success will return 0, and others indicate fail.
 */
int usbd_set_address(const uint8_t addr);

/**
 * @brief Get USB device speed
 *
 * @param[in] port port index
 *
 * @return port speed, USB_SPEED_LOW or USB_SPEED_FULL or USB_SPEED_HIGH
 */
uint8_t usbd_get_port_speed(const uint8_t port);

/**
 * @brief configure and enable endpoint.
 *
 * @param [in]  ep_cfg Endpoint config.
 *
 * @return On success will return 0, and others indicate fail.
 */
int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg);

/**
 * @brief Disable the selected endpoint
 *
 * @param[in] ep Endpoint address
 *
 * @return On success will return 0, and others indicate fail.
 */
int usbd_ep_close(const uint8_t ep);

/**
 * @brief Set stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address
 *
 *
 * @return On success will return 0, and others indicate fail.
 */
int usbd_ep_set_stall(const uint8_t ep);

/**
 * @brief Clear stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return On success will return 0, and others indicate fail.
 */
int usbd_ep_clear_stall(const uint8_t ep);

/**
 * @brief Check if the selected endpoint is stalled
 *
 * @param[in]  ep       Endpoint address
 *
 * @param[out] stalled  Endpoint stall status
 *
 * @return On success will return 0, and others indicate fail.
 */
int usbd_ep_is_stalled(const uint8_t ep, uint8_t *stalled);

/**
 * @brief Setup in ep transfer setting and start transfer.
 *
 * This function is asynchronous.
 * This function is similar to uart with tx dma.
 *
 * This function is called to write data to the specified endpoint. The
 * supplied usbd_endpoint_callback function will be called when data is transmitted
 * out.
 *
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  data      Pointer to data to write
 * @param[in]  data_len  Length of the data requested to write. This may
 *                       be zero for a zero length status packet.
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_start_write(const uint8_t ep, const uint8_t *data, uint32_t data_len);

/**
 * @brief Setup out ep transfer setting and start transfer.
 *
 * This function is asynchronous.
 * This function is similar to uart with rx dma.
 *
 * This function is called to read data to the specified endpoint. The
 * supplied usbd_endpoint_callback function will be called when data is received
 * in.
 *
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  data      Pointer to data to read
 * @param[in]  data_len  Max length of the data requested to read.
 *
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_start_read(const uint8_t ep, uint8_t *data, uint32_t data_len);

/* usb dcd irq callback */

/**
 * @brief Usb connect irq callback.
 */
void usbd_event_connect_handler(void);

/**
 * @brief Usb disconnect irq callback.
 */
void usbd_event_disconnect_handler(void);

/**
 * @brief Usb resume irq callback.
 */
void usbd_event_resume_handler(void);

/**
 * @brief Usb suspend irq callback.
 */
void usbd_event_suspend_handler(void);

/**
 * @brief Usb reset irq callback.
 */
void usbd_event_reset_handler(void);

/**
 * @brief Usb setup packet recv irq callback.
 * @param[in]  psetup  setup packet.
 */
void usbd_event_ep0_setup_complete_handler(uint8_t *psetup);

/**
 * @brief In ep transfer complete irq callback.
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  nbytes    How many nbytes have transferred.
 */
void usbd_event_ep_in_complete_handler(uint8_t ep, uint32_t nbytes);

/**
 * @brief Out ep transfer complete irq callback.
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  nbytes    How many nbytes have transferred.
 */
void usbd_event_ep_out_complete_handler(uint8_t ep, uint32_t nbytes);

/**
 * @brief Enable DMA TX for the specified endpoint
 * @param[in]  ep  Endpoint address
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_enable_dma_tx(const uint8_t ep);

/**
 * @brief Disable DMA TX for the specified endpoint (clear DMA_TX_EN)
 * @param[in]  ep  Endpoint address
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_disable_dma_tx(const uint8_t ep);

/**
 * @brief Register a DMA channel for M2P (mem->FIFO) writes on a BULK IN EP.
 * @param[in]  ep      Endpoint address
 * @param[in]  dma_ch  DMA channel index (routes via the channel's dstPeriph)
 */
void usbd_ep_setup_dma_tx(const uint8_t ep, uint8_t dma_ch);

/**
 * @brief Arm a BULK IN transfer whose FIFO is filled by DMA (not CPU mem2fifo).
 *        Caller must arm the DMA channel (reload + start) BEFORE calling this.
 *        Completion fires via the IN EP-done ISR -> usbd_event_ep_in_complete_handler.
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_start_write_dma(const uint8_t ep, const uint8_t *data, uint32_t data_len);

/**
 * @brief Toggle DMA TX enable (0->1) to recover from HW stall
 * @param[in]  ep  Endpoint address
 */
void usbd_ep_toggle_dma_tx(const uint8_t ep);

/**
 * @brief Trigger endpoint ready to start transmission
 * @param[in]  ep  Endpoint address
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_trigger_ready(const uint8_t ep);

/**
 * @brief Check if endpoint is ready
 * @param[in]  ep  Endpoint address
 * @return 1 if ready, 0 if not ready, negative errno code on fail.
 */
int usbd_ep_is_ready(const uint8_t ep);

/**
 * @brief Get TX FIFO address for the specified endpoint
 * @param[in]  ep  Endpoint address
 * @return Physical address of the TX FIFO
 */
uint32_t usbd_ep_get_tx_fifo_addr(const uint8_t ep);

/**
 * @brief Get RX FIFO address for the specified endpoint
 * @param[in]  ep  Endpoint address
 * @return Physical address of the RX FIFO
 */
uint32_t usbd_ep_get_rx_fifo_addr(const uint8_t ep);

/**
 * @brief Enable DMA RX hardware handshake for the specified endpoint
 * @param[in]  ep  Endpoint address
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_enable_dma_rx(const uint8_t ep);

/**
 * @brief Register DMA channel for endpoint RX and enable hardware handshake
 * @param[in]  ep       Endpoint address
 * @param[in]  dma_ch   DMA channel index (e.g. DMA0_CH2_INDEX)
 */
void usbd_ep_setup_dma_rx(const uint8_t ep, uint8_t dma_ch);

/**
 * @brief Mark an OUT EP's DMA RX as raw bulk (skip NCM/NTB framing checks in the ISR).
 * @param[in]  ep  Endpoint address
 */
void usbd_ep_dma_rx_set_raw(const uint8_t ep);

/**
 * @brief USB IRQ statistics (updated in ISR, safe to read from task)
 */
typedef struct {
    uint32_t total;
    uint32_t reset;
    uint32_t vbus_tgl;
    uint32_t ep0_setup;
    uint32_t ep0_in;
    uint32_t ep0_out;
    uint32_t rend;
    uint32_t ep_done[7]; /* EP1..EP7 */
    uint32_t ep0_busy_timeout;  /* EP0 busy-wait timeout count */
    uint32_t setup_fail;        /* Setup packet rx_count != 8 */
    /* OUT path diagnostic counters */
    uint32_t out_entry;         /* OUT path entered */
    uint32_t out_busy_timeout;  /* wait_ready timeout in OUT path */
    uint32_t out_rxcount_zero;  /* rx_count == 0 (first read) */
    uint32_t out_rxz_still;     /* still 0 after retry delay */
    uint32_t out_rxz_recovered; /* non-zero after retry delay */
    uint32_t out_short_pkt;     /* rx_count > 0 && rx_count < ep_mps */
    uint32_t out_xferlen_zero;  /* xfer_len == 0 (natural completion) */
    uint32_t out_more_data;     /* normal set_ready (more data expected) */
    /* EP_DONE seen with no buffer armed (xfer_buf == NULL). The "ghost" guard treats
     * that as a stale interrupt and skips it -- correct only when the FIFO is EMPTY.
     * A class driver that deliberately withholds usbd_ep_start_read() as backpressure
     * (the whole point of NAK flow control) leaves xfer_buf NULL on purpose, and a
     * packet the controller had already ACKed into the FIFO is then discarded. Splitting
     * the count says which one is happening: ghost_data > 0 means real bytes were
     * thrown away. */
    uint32_t out_ghost_empty;   /* no buffer armed, FIFO empty  -> genuinely stale */
    uint32_t out_ghost_data;    /* no buffer armed, FIFO HAS DATA -> packet discarded */
    /* rx_count exceeded the armed xfer_len: the FIFO held a stale packet on top of
     * the live one (see the clamp in the OUT CPU path). The copy is clamped so the
     * EP survives; >0 here means the "spurious EP_DONE" re-arm left data behind. */
    uint32_t out_rxcount_over;
} usb_irq_stats_t;

extern volatile usb_irq_stats_t g_usb_irq_stats;

/**
 * @brief Clear an endpoint's hardware FIFO (IN -> TX FIFO, OUT -> RX FIFO).
 * Neither usbd_ep_open() nor bus reset clears the FIFOs; call this when a new
 * connection must not inherit the previous one's undelivered packet.
 * @param[in] ep  Endpoint address (direction selects which FIFO)
 * @return 0 on success, -1 on a bad endpoint index
 */
int usbd_ep_fifo_clear(const uint8_t ep);

/**
 * @brief Get USB interrupt statistics snapshot
 * @param[out] stats  Destination for stats copy
 */
void usbd_get_irq_stats(usb_irq_stats_t *stats);
void usbd_reset_irq_stats(void);

/**
 * @brief OUT endpoint HW status for diagnostics
 */
typedef struct {
    uint32_t ep_cfg_reg;    /* EPx config register (includes EP_RDY bit) */
    uint16_t fifo_rx_cnt;   /* Current RX FIFO byte count */
    uint8_t  ep_enabled;    /* Software ep_enable flag */
    uint8_t  ep_stalled;    /* Software ep_stalled flag */
    uint8_t *xfer_buf;      /* Current transfer buffer pointer */
    uint32_t xfer_len;      /* Remaining transfer length */
    uint32_t actual_xfer_len; /* Bytes transferred so far */
} usb_ep_out_status_t;

/**
 * @brief Get OUT endpoint hardware and software status
 * @param[in]  ep_idx  Endpoint index (1-7)
 * @param[out] status  Destination for status
 */
void usbd_get_ep_out_status(uint8_t ep_idx, usb_ep_out_status_t *status);

/**
 * @brief IN endpoint HW status for diagnostics
 */
typedef struct {
    uint32_t ep_cfg_reg;    /* EPx config register */
    uint16_t fifo_tx_cnt;   /* Current TX FIFO byte count */
    uint8_t  ep_enabled;    /* Software ep_enable flag */
    uint8_t  ep_stalled;    /* Software ep_stalled flag */
    uint8_t *xfer_buf;      /* Current transfer buffer pointer */
    uint32_t xfer_len;      /* Remaining transfer length */
    uint32_t actual_xfer_len; /* Bytes transferred so far */
} usb_ep_in_status_t;

/**
 * @brief Get IN endpoint hardware and software status
 * @param[in]  ep_idx  Endpoint index (1-7)
 * @param[out] status  Destination for status
 */
void usbd_get_ep_in_status(uint8_t ep_idx, usb_ep_in_status_t *status);

#ifdef __cplusplus
}
#endif

#endif /* USB_DC_H */
