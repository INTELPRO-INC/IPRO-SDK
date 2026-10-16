/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 DMA peripheral request lines (bsp/hal/inc/hal_dma.h DMA_REQUEST_*),
 * used as the "slot" cell of an intelpro,ipro-dma consumer:
 *   dmas = <&dma0 CHANNEL SLOT>;
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_DMA_IPRO7_DMA_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_DMA_IPRO7_DMA_H_

#define IPRO7_DMA_REQ_NONE     0
#define IPRO7_DMA_REQ_UART0_RX 0
#define IPRO7_DMA_REQ_UART0_TX 1
#define IPRO7_DMA_REQ_UART1_RX 2
#define IPRO7_DMA_REQ_UART1_TX 3
#define IPRO7_DMA_REQ_I2C0_RX  4
#define IPRO7_DMA_REQ_I2C0_TX  5
#define IPRO7_DMA_REQ_SPI0_RX  6
#define IPRO7_DMA_REQ_SPI0_TX  7
#define IPRO7_DMA_REQ_IR_TX    8
#define IPRO7_DMA_REQ_AUD_ADC  9
#define IPRO7_DMA_REQ_I2S_RX   10
#define IPRO7_DMA_REQ_I2S_TX   11
#define IPRO7_DMA_REQ_ADC0     12
#define IPRO7_DMA_REQ_DAC0     13
#define IPRO7_DMA_REQ_SPI1_RX  14
#define IPRO7_DMA_REQ_SPI1_TX  15
#define IPRO7_DMA_REQ_USB_EP0  24
#define IPRO7_DMA_REQ_USB_EP1  25
#define IPRO7_DMA_REQ_USB_EP2  26
#define IPRO7_DMA_REQ_USB_EP3  27
#define IPRO7_DMA_REQ_USB_EP4  28
#define IPRO7_DMA_REQ_USB_EP5  29
#define IPRO7_DMA_REQ_USB_EP6  30
#define IPRO7_DMA_REQ_USB_EP7  31

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_DMA_IPRO7_DMA_H_ */
