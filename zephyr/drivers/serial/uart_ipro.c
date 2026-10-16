/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 UART driver.
 *
 * Layering: Zephyr uart_driver_api -> this file -> SDK LL drv_uart.c
 * (UART_Init / UART_IntMask / FIFO accessors). The SDK's device-level
 * hal_uart.c is deliberately bypassed: it bundles configuration, IRQ
 * registration and a callback model that Zephyr's own API replaces, while
 * every register access it would make lives in drv_uart.c anyway.
 *
 * Hardware: 16-byte TX/RX FIFOs, thresholds, RX timeout (RTO) interrupt
 * for trailing bytes, RTS/CTS, 5-8 data bits, 0.5/1/1.5/2 stop bits.
 *
 * Async API (CONFIG_UART_ASYNC_API, node with dmas = <&dma0 ch REQ_TX>,
 * <&dma0 ch REQ_RX>): TX is one memory->FIFO DMA block; RX is one
 * peripheral->memory DMA block into the caller's buffer, with the RTO
 * (receiver idle) interrupt delivering UART_RX_RDY for whatever the DMA has
 * written so far (live remaining count), DMA completion delivering the rest
 * and switching to the next buffer, and the Zephyr-side timeout handled by
 * the RTO hardware (rx_timeout_bits) rather than a software timer. The
 * caller's RX buffer is D-cache invalidated before each delivery.
 */

#define DT_DRV_COMPAT intelpro_ipro_uart

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/irq.h>
#include <soc.h>
#include <ipro_zephyr/ipro_hal_glue.h>

#include "ipro_soc_regs.h"
#include "drv_uart.h"
#include "drv_glb.h"

#define UART_IPRO_FIFO_DEPTH 16

struct uart_ipro_config {
	UART_ID_Type id;
	uint32_t base;
	const struct pinctrl_dev_config *pincfg;
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
	uint8_t rx_timeout_bits;
#if defined(CONFIG_UART_INTERRUPT_DRIVEN) || defined(CONFIG_UART_ASYNC_API)
	void (*irq_config_func)(const struct device *dev);
#endif
#ifdef CONFIG_UART_ASYNC_API
	const struct device *dma_dev;
	uint8_t tx_dma_ch, rx_dma_ch;
	uint8_t tx_dma_slot, rx_dma_slot;
	bool has_dma;
#endif
};

struct uart_ipro_data {
	struct uart_config cfg;
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	uart_irq_callback_user_data_t cb;
	void *cb_data;
	bool tx_irq_enabled;
#endif
#ifdef CONFIG_UART_ASYNC_API
	const struct device *dev;
	uart_callback_t async_cb;
	void *async_user;
	struct k_spinlock lock;
	/* TX */
	const uint8_t *tx_buf;
	size_t tx_len;
	struct k_work_delayable tx_timeout_work;
	/* RX */
	uint8_t *rx_buf;
	size_t rx_len;
	size_t rx_offset;   /* bytes already reported */
	uint8_t *rx_next_buf;
	size_t rx_next_len;
	bool rx_enabled;
	bool rx_dma_active;
#endif
};

static int uart_ipro_apply(const struct device *dev, const struct uart_config *cfg)
{
	const struct uart_ipro_config *config = dev->config;
	UART_CFG_Type ll = {
		.baudRate = cfg->baudrate,
		.ctsFlowControl = (cfg->flow_ctrl == UART_CFG_FLOW_CTRL_RTS_CTS) ? ENABLE : DISABLE,
		.rxDeglitch = ENABLE,
		.rtsSoftwareControl = DISABLE,
		.txSoftwareControl = DISABLE,
		.txLinMode = DISABLE,
		.rxLinMode = DISABLE,
		.txBreakBitCnt = 0,
		.byteBitInverse = UART_LSB_FIRST,
	};
	UART_FifoCfg_Type fifo = {
		.txFifoDmaThreshold = 0,
		.rxFifoDmaThreshold = 0,
		.txFifoDmaEnable = DISABLE,
		.rxFifoDmaEnable = DISABLE,
	};
	uint32_t clk;

	switch (cfg->data_bits) {
	case UART_CFG_DATA_BITS_5:
		ll.dataBits = UART_DATABITS_5;
		break;
	case UART_CFG_DATA_BITS_6:
		ll.dataBits = UART_DATABITS_6;
		break;
	case UART_CFG_DATA_BITS_7:
		ll.dataBits = UART_DATABITS_7;
		break;
	case UART_CFG_DATA_BITS_8:
		ll.dataBits = UART_DATABITS_8;
		break;
	default:
		return -ENOTSUP;
	}

	switch (cfg->stop_bits) {
	case UART_CFG_STOP_BITS_0_5:
		ll.stopBits = UART_STOPBITS_0_5;
		break;
	case UART_CFG_STOP_BITS_1:
		ll.stopBits = UART_STOPBITS_1;
		break;
	case UART_CFG_STOP_BITS_1_5:
		ll.stopBits = UART_STOPBITS_1_5;
		break;
	case UART_CFG_STOP_BITS_2:
		ll.stopBits = UART_STOPBITS_2;
		break;
	default:
		return -ENOTSUP;
	}

	switch (cfg->parity) {
	case UART_CFG_PARITY_NONE:
		ll.parity = UART_PARITY_NONE;
		break;
	case UART_CFG_PARITY_ODD:
		ll.parity = UART_PARITY_ODD;
		break;
	case UART_CFG_PARITY_EVEN:
		ll.parity = UART_PARITY_EVEN;
		break;
	default:
		return -ENOTSUP;
	}

	if (cfg->flow_ctrl != UART_CFG_FLOW_CTRL_NONE && cfg->flow_ctrl != UART_CFG_FLOW_CTRL_RTS_CTS) {
		return -ENOTSUP;
	}

	if (clock_control_get_rate(config->clock_dev, config->clock_id, &clk) != 0 || clk == 0) {
		return -EIO;
	}
	ll.uartClk = clk;

	UART_IntMask(config->id, UART_INT_ALL, MASK);
	UART_Disable(config->id, UART_TXRX);

	/*
	 * UART0's APB clock only runs while the register interface is
	 * clocked: work-around from hal_uart.c (IPRO7 GLB 0x3000006c[11]).
	 */
#ifdef CONFIG_SOC_SERIES_IPRO7
	if (config->id == UART0_ID) {
		*(volatile uint32_t *)0x3000006cU |= BIT(11);
	}
#endif /* IPRO6: same address is GLB_CORE_CFG23; bit 11 corrupts SF_Ctrl (hal_uart.c) */

	if (UART_Init(config->id, &ll) != 0) {
		return -EIO;
	}
	UART_TxFreeRun(config->id, ENABLE);
	UART_SetDeglitchCount(config->id, 2);
	UART_SetRxTimeoutValue(config->id, config->rx_timeout_bits);
	UART_FifoConfig(config->id, &fifo);
	UART_TxFifoClear(config->id);
	UART_RxFifoClear(config->id);
	UART_Enable(config->id, UART_TXRX);
	return 0;
}

static int uart_ipro_poll_in(const struct device *dev, unsigned char *c)
{
	const struct uart_ipro_config *config = dev->config;

	if (UART_GetRxFifoCount(config->id) == 0) {
		return -1;
	}
	*c = *(volatile uint8_t *)(config->base + UART_FIFO_RDATA_OFFSET);
	return 0;
}

static void uart_ipro_poll_out(const struct device *dev, unsigned char c)
{
	const struct uart_ipro_config *config = dev->config;

	/* tx_fifo_cnt is the number of FREE entries. */
	while (UART_GetTxFifoCount(config->id) == 0) {
	}
	*(volatile uint8_t *)(config->base + UART_FIFO_WDATA_OFFSET) = c;
}

static int uart_ipro_err_check(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	int err = 0;

	if (UART_GetOverflowStatus(config->id, UART_RX_OVERFLOW) == SET) {
		err |= UART_ERROR_OVERRUN;
	}
	if (UART_GetIntStatus(config->id, UART_INT_PCE) == SET) {
		err |= UART_ERROR_PARITY;
		UART_IntClear(config->id, UART_INT_PCE);
	}
	if (UART_GetIntStatus(config->id, UART_INT_RX_FER) == SET) {
		err |= UART_ERROR_OVERRUN;
		UART_IntClear(config->id, UART_INT_RX_FER);
		UART_RxFifoClear(config->id);
	}
	return err;
}

#ifdef CONFIG_UART_USE_RUNTIME_CONFIGURE
static int uart_ipro_configure(const struct device *dev, const struct uart_config *cfg)
{
	struct uart_ipro_data *data = dev->data;
	int ret = uart_ipro_apply(dev, cfg);

	if (ret == 0) {
		data->cfg = *cfg;
	}
	return ret;
}

static int uart_ipro_config_get(const struct device *dev, struct uart_config *cfg)
{
	struct uart_ipro_data *data = dev->data;

	*cfg = data->cfg;
	return 0;
}
#endif /* CONFIG_UART_USE_RUNTIME_CONFIGURE */

#ifdef CONFIG_UART_INTERRUPT_DRIVEN

static int uart_ipro_fifo_fill(const struct device *dev, const uint8_t *tx_data, int len)
{
	const struct uart_ipro_config *config = dev->config;
	int n = 0;

	while (n < len && UART_GetTxFifoCount(config->id) > 0) {
		*(volatile uint8_t *)(config->base + UART_FIFO_WDATA_OFFSET) = tx_data[n++];
	}
	return n;
}

static int uart_ipro_fifo_read(const struct device *dev, uint8_t *rx_data, const int size)
{
	const struct uart_ipro_config *config = dev->config;
	int n = 0;

	while (n < size && UART_GetRxFifoCount(config->id) > 0) {
		rx_data[n++] = *(volatile uint8_t *)(config->base + UART_FIFO_RDATA_OFFSET);
	}
	return n;
}

static void uart_ipro_irq_tx_enable(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;

	data->tx_irq_enabled = true;
	/* TX_FIFO_REQ fires while free entries > threshold (0): i.e. room. */
	UART_IntMask(config->id, UART_INT_TX_FIFO_REQ, UNMASK);
}

static void uart_ipro_irq_tx_disable(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;

	data->tx_irq_enabled = false;
	UART_IntMask(config->id, UART_INT_TX_FIFO_REQ, MASK);
}

static int uart_ipro_irq_tx_ready(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;

	return data->tx_irq_enabled && UART_GetTxFifoCount(config->id) > 0;
}

static int uart_ipro_irq_tx_complete(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;

	return UART_GetTxFifoCount(config->id) == UART_IPRO_FIFO_DEPTH &&
	       UART_GetTxBusBusyStatus(config->id) == RESET;
}

static void uart_ipro_irq_rx_enable(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;

	UART_IntMask(config->id, UART_INT_RX_FIFO_REQ, UNMASK);
	UART_IntMask(config->id, UART_INT_RTO, UNMASK);
}

static void uart_ipro_irq_rx_disable(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;

	UART_IntMask(config->id, UART_INT_RX_FIFO_REQ, MASK);
	UART_IntMask(config->id, UART_INT_RTO, MASK);
}

static int uart_ipro_irq_rx_ready(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;

	return UART_GetRxFifoCount(config->id) > 0;
}

static void uart_ipro_irq_err_enable(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;

	UART_IntMask(config->id, UART_INT_PCE, UNMASK);
	UART_IntMask(config->id, UART_INT_RX_FER, UNMASK);
	UART_IntMask(config->id, UART_INT_TX_FER, UNMASK);
}

static void uart_ipro_irq_err_disable(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;

	UART_IntMask(config->id, UART_INT_PCE, MASK);
	UART_IntMask(config->id, UART_INT_RX_FER, MASK);
	UART_IntMask(config->id, UART_INT_TX_FER, MASK);
}

static int uart_ipro_irq_is_pending(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;

	return (data->tx_irq_enabled && UART_GetTxFifoCount(config->id) > 0) ||
	       UART_GetRxFifoCount(config->id) > 0 ||
	       UART_GetIntStatus(config->id, UART_INT_RTO) == SET;
}

static int uart_ipro_irq_update(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;

	/* RTO/RX_END/PCE/FER are sticky and must be acknowledged;
	 * the FIFO request bits clear themselves. */
	if (UART_GetIntStatus(config->id, UART_INT_RTO) == SET) {
		UART_IntClear(config->id, UART_INT_RTO);
	}
	if (UART_GetIntStatus(config->id, UART_INT_RX_END) == SET) {
		UART_IntClear(config->id, UART_INT_RX_END);
	}
	if (UART_GetIntStatus(config->id, UART_INT_TX_END) == SET) {
		UART_IntClear(config->id, UART_INT_TX_END);
	}
	return 1;
}

static void uart_ipro_irq_callback_set(const struct device *dev, uart_irq_callback_user_data_t cb,
				       void *user_data)
{
	struct uart_ipro_data *data = dev->data;

	data->cb = cb;
	data->cb_data = user_data;
}

static void uart_ipro_isr(const void *arg)
{
	const struct device *dev = arg;
	struct uart_ipro_data *data = dev->data;

	if (data->cb != NULL) {
		data->cb(dev, data->cb_data);
	} else {
		/* Nobody listening: acknowledge sticky sources to leave the ISR. */
		uart_ipro_irq_update(dev);
	}
}

#endif /* CONFIG_UART_INTERRUPT_DRIVEN */


#ifdef CONFIG_UART_ASYNC_API

/* The FIFO's DMA request lines are only enabled while a transfer runs, so
 * polling/interrupt users of the same UART (the console) are unaffected. */
static void uart_ipro_fifo_dma(const struct device *dev, bool tx, bool rx)
{
	const struct uart_ipro_config *config = dev->config;
	UART_FifoCfg_Type fifo = {
		.txFifoDmaThreshold = 0,
		.rxFifoDmaThreshold = 0,
		.txFifoDmaEnable = tx ? ENABLE : DISABLE,
		.rxFifoDmaEnable = rx ? ENABLE : DISABLE,
	};

	UART_FifoConfig(config->id, &fifo);
}

static void uart_ipro_async_event(struct uart_ipro_data *data, struct uart_event *evt)
{
	if (data->async_cb != NULL) {
		data->async_cb(data->dev, evt, data->async_user);
	}
}

static int uart_ipro_callback_set(const struct device *dev, uart_callback_t cb, void *user_data)
{
	struct uart_ipro_data *data = dev->data;

	data->async_cb = cb;
	data->async_user = user_data;
	return 0;
}

static void uart_ipro_tx_dma_cb(const struct device *dma, void *user, uint32_t ch, int status)
{
	const struct device *dev = user;
	struct uart_ipro_data *data = dev->data;
	struct uart_event evt = {.type = status == 0 ? UART_TX_DONE : UART_TX_ABORTED};
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	ARG_UNUSED(dma);
	ARG_UNUSED(ch);
	(void)k_work_cancel_delayable(&data->tx_timeout_work);
	uart_ipro_fifo_dma(dev, false, data->rx_enabled);
	evt.data.tx.buf = data->tx_buf;
	evt.data.tx.len = status == 0 ? data->tx_len : 0;
	data->tx_buf = NULL;
	k_spin_unlock(&data->lock, key);
	uart_ipro_async_event(data, &evt);
}

static int uart_ipro_tx(const struct device *dev, const uint8_t *buf, size_t len, int32_t timeout)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;
	struct dma_block_config blk = {
		.source_address = (uint32_t)buf,
		.dest_address = config->base + UART_FIFO_WDATA_OFFSET,
		.block_size = len,
		.source_addr_adj = DMA_ADDR_ADJ_INCREMENT,
		.dest_addr_adj = DMA_ADDR_ADJ_NO_CHANGE,
	};
	struct dma_config cfg = {
		.dma_slot = config->tx_dma_slot,
		.channel_direction = MEMORY_TO_PERIPHERAL,
		.source_data_size = 1,
		.dest_data_size = 1,
		.source_burst_length = 1,
		.dest_burst_length = 1,
		.block_count = 1,
		.head_block = &blk,
		.dma_callback = uart_ipro_tx_dma_cb,
		.user_data = (void *)dev,
	};
	int ret;

	if (!config->has_dma) {
		return -ENOTSUP;
	}
	if (len == 0U || len > 4095U) {
		return -EINVAL;
	}

	k_spinlock_key_t key = k_spin_lock(&data->lock);

	if (data->tx_buf != NULL) {
		k_spin_unlock(&data->lock, key);
		return -EBUSY;
	}
	data->tx_buf = buf;
	data->tx_len = len;
	k_spin_unlock(&data->lock, key);

	uart_ipro_fifo_dma(dev, true, data->rx_enabled);
	ret = dma_config(config->dma_dev, config->tx_dma_ch, &cfg);
	if (ret == 0) {
		ret = dma_start(config->dma_dev, config->tx_dma_ch);
	}
	if (ret != 0) {
		data->tx_buf = NULL;
		return ret;
	}
	if (timeout != SYS_FOREVER_US) {
		k_work_schedule(&data->tx_timeout_work, K_USEC(timeout));
	}
	return 0;
}

static int uart_ipro_tx_abort(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;
	struct uart_event evt = {.type = UART_TX_ABORTED};
	size_t remaining;
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	if (data->tx_buf == NULL) {
		k_spin_unlock(&data->lock, key);
		return -EFAULT;
	}
	remaining = ipro_hal_dma_channel_remaining(config->tx_dma_ch);
	(void)dma_stop(config->dma_dev, config->tx_dma_ch);
	uart_ipro_fifo_dma(dev, false, data->rx_enabled);
	(void)k_work_cancel_delayable(&data->tx_timeout_work);
	evt.data.tx.buf = data->tx_buf;
	evt.data.tx.len = remaining <= data->tx_len ? data->tx_len - remaining : 0;
	data->tx_buf = NULL;
	k_spin_unlock(&data->lock, key);
	uart_ipro_async_event(data, &evt);
	return 0;
}

static void uart_ipro_tx_timeout(struct k_work *work)
{
	struct k_work_delayable *dw = k_work_delayable_from_work(work);
	struct uart_ipro_data *data = CONTAINER_OF(dw, struct uart_ipro_data, tx_timeout_work);

	(void)uart_ipro_tx_abort(data->dev);
}

/* Report bytes the DMA has written since the last report. Called with the
 * lock held; the event is delivered after unlocking by the caller. */
static bool uart_ipro_rx_collect(const struct device *dev, size_t written, struct uart_event *evt)
{
	struct uart_ipro_data *data = dev->data;

	if (written <= data->rx_offset || data->rx_buf == NULL) {
		return false;
	}
	ipro_hal_dma_cache_invalidate((uint32_t)data->rx_buf + data->rx_offset,
				      written - data->rx_offset);
	evt->type = UART_RX_RDY;
	evt->data.rx.buf = data->rx_buf;
	evt->data.rx.offset = data->rx_offset;
	evt->data.rx.len = written - data->rx_offset;
	data->rx_offset = written;
	return true;
}

static int uart_ipro_rx_dma_start(const struct device *dev, uint8_t *buf, size_t len)
{
	const struct uart_ipro_config *config = dev->config;
	struct dma_block_config blk = {
		.source_address = config->base + UART_FIFO_RDATA_OFFSET,
		.dest_address = (uint32_t)buf,
		.block_size = len,
		.source_addr_adj = DMA_ADDR_ADJ_NO_CHANGE,
		.dest_addr_adj = DMA_ADDR_ADJ_INCREMENT,
	};
	struct dma_config cfg = {
		.dma_slot = config->rx_dma_slot,
		.channel_direction = PERIPHERAL_TO_MEMORY,
		.source_data_size = 1,
		.dest_data_size = 1,
		.source_burst_length = 1,
		.dest_burst_length = 1,
		.block_count = 1,
		.head_block = &blk,
		.dma_callback = NULL, /* set below */
		.user_data = (void *)dev,
	};
	extern void uart_ipro_rx_dma_cb(const struct device *, void *, uint32_t, int);
	int ret;

	cfg.dma_callback = uart_ipro_rx_dma_cb;
	if (k_is_in_isr()) {
		/* Buffer switch from the completion ISR: the channel keeps its
		 * configuration, only the destination changes (dma_reload is
		 * ISR-safe in dma_ipro and leaves the channel running). */
		return dma_reload(config->dma_dev, config->rx_dma_ch,
				  config->base + UART_FIFO_RDATA_OFFSET, (uint32_t)buf, len);
	}
	ret = dma_config(config->dma_dev, config->rx_dma_ch, &cfg);
	if (ret == 0) {
		ret = dma_start(config->dma_dev, config->rx_dma_ch);
	}
	return ret;
}

void uart_ipro_rx_dma_cb(const struct device *dma, void *user, uint32_t ch, int status)
{
	const struct device *dev = user;
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;
	struct uart_event rdy, released = {.type = UART_RX_BUF_RELEASED};
	struct uart_event req = {.type = UART_RX_BUF_REQUEST};
	struct uart_event disabled = {.type = UART_RX_DISABLED};
	bool have_rdy, have_next, stopped = false;
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	ARG_UNUSED(dma);
	ARG_UNUSED(ch);
	if (data->rx_buf == NULL) {
		k_spin_unlock(&data->lock, key);
		return;
	}
	have_rdy = (status == 0) && uart_ipro_rx_collect(dev, data->rx_len, &rdy);
	released.data.rx_buf.buf = data->rx_buf;

	have_next = data->rx_next_buf != NULL && data->rx_enabled;
	if (have_next) {
		data->rx_buf = data->rx_next_buf;
		data->rx_len = data->rx_next_len;
		data->rx_next_buf = NULL;
		data->rx_offset = 0;
		if (uart_ipro_rx_dma_start(dev, data->rx_buf, data->rx_len) != 0) {
			have_next = false;
		}
	}
	if (!have_next) {
		data->rx_buf = NULL;
		data->rx_dma_active = false;
		data->rx_enabled = false;
		UART_IntMask(config->id, UART_INT_RTO, MASK);
		uart_ipro_fifo_dma(dev, data->tx_buf != NULL, false);
		stopped = true;
	}
	k_spin_unlock(&data->lock, key);

	if (have_rdy) {
		uart_ipro_async_event(data, &rdy);
	}
	uart_ipro_async_event(data, &released);
	if (have_next) {
		uart_ipro_async_event(data, &req);
	}
	if (stopped) {
		uart_ipro_async_event(data, &disabled);
	}
}

static int uart_ipro_rx_enable(const struct device *dev, uint8_t *buf, size_t len, int32_t timeout)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;
	struct uart_event req = {.type = UART_RX_BUF_REQUEST};
	int ret;

	ARG_UNUSED(timeout); /* the RTO hardware timeout (rx_timeout_bits) applies */
	if (!config->has_dma) {
		return -ENOTSUP;
	}
	if (buf == NULL || len == 0U || len > 4095U) {
		return -EINVAL;
	}

	k_spinlock_key_t key = k_spin_lock(&data->lock);

	if (data->rx_enabled) {
		k_spin_unlock(&data->lock, key);
		return -EBUSY;
	}
	data->rx_buf = buf;
	data->rx_len = len;
	data->rx_offset = 0;
	data->rx_next_buf = NULL;
	data->rx_enabled = true;
	UART_RxFifoClear(config->id);
	uart_ipro_fifo_dma(dev, data->tx_buf != NULL, true);
	ret = uart_ipro_rx_dma_start(dev, buf, len);
	if (ret != 0) {
		data->rx_enabled = false;
		data->rx_buf = NULL;
		uart_ipro_fifo_dma(dev, data->tx_buf != NULL, false);
		k_spin_unlock(&data->lock, key);
		return ret;
	}
	data->rx_dma_active = true;
	UART_IntClear(config->id, UART_INT_RTO);
	UART_IntMask(config->id, UART_INT_RTO, UNMASK);
	k_spin_unlock(&data->lock, key);

	uart_ipro_async_event(data, &req);
	return 0;
}

static int uart_ipro_rx_buf_rsp(const struct device *dev, uint8_t *buf, size_t len)
{
	struct uart_ipro_data *data = dev->data;
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	if (!data->rx_enabled) {
		k_spin_unlock(&data->lock, key);
		return -EACCES;
	}
	if (data->rx_next_buf != NULL) {
		k_spin_unlock(&data->lock, key);
		return -EBUSY;
	}
	data->rx_next_buf = buf;
	data->rx_next_len = len;
	k_spin_unlock(&data->lock, key);
	return 0;
}

static int uart_ipro_rx_disable(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;
	struct uart_event rdy, released = {.type = UART_RX_BUF_RELEASED};
	struct uart_event next_released = {.type = UART_RX_BUF_RELEASED};
	struct uart_event disabled = {.type = UART_RX_DISABLED};
	bool have_rdy, have_next;
	size_t written;
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	if (!data->rx_enabled) {
		k_spin_unlock(&data->lock, key);
		return -EFAULT;
	}
	UART_IntMask(config->id, UART_INT_RTO, MASK);
	written = data->rx_len - ipro_hal_dma_channel_remaining(config->rx_dma_ch);
	(void)dma_stop(config->dma_dev, config->rx_dma_ch);
	have_rdy = uart_ipro_rx_collect(dev, written, &rdy);
	released.data.rx_buf.buf = data->rx_buf;
	have_next = data->rx_next_buf != NULL;
	next_released.data.rx_buf.buf = data->rx_next_buf;
	data->rx_buf = NULL;
	data->rx_next_buf = NULL;
	data->rx_enabled = false;
	data->rx_dma_active = false;
	uart_ipro_fifo_dma(dev, data->tx_buf != NULL, false);
	k_spin_unlock(&data->lock, key);

	if (have_rdy) {
		uart_ipro_async_event(data, &rdy);
	}
	uart_ipro_async_event(data, &released);
	if (have_next) {
		uart_ipro_async_event(data, &next_released);
	}
	uart_ipro_async_event(data, &disabled);
	return 0;
}

/* RTO: the receiver went idle after data. Report what the DMA wrote so far. */
static void uart_ipro_async_rto(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;
	struct uart_event rdy;
	bool have_rdy = false;
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	UART_IntClear(config->id, UART_INT_RTO);
	if (data->rx_enabled && data->rx_dma_active) {
		size_t written = data->rx_len - ipro_hal_dma_channel_remaining(config->rx_dma_ch);

		have_rdy = uart_ipro_rx_collect(dev, written, &rdy);
	}
	k_spin_unlock(&data->lock, key);
	if (have_rdy) {
		uart_ipro_async_event(data, &rdy);
	}
}

#endif /* CONFIG_UART_ASYNC_API */

static void uart_ipro_isr_common(const void *arg)
{
	const struct device *dev = arg;
	const struct uart_ipro_config *config = dev->config;

	ARG_UNUSED(config); /* interrupt-driven only: uart_ipro_isr() reads it itself */
#ifdef CONFIG_UART_ASYNC_API
	struct uart_ipro_data *adata = dev->data;

	if (adata->rx_enabled && UART_GetIntStatus(config->id, UART_INT_RTO) == SET) {
		uart_ipro_async_rto(dev);
		return;
	}
#endif
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	uart_ipro_isr(arg);
#else
	/* Nothing else listening: acknowledge sticky sources. */
	if (UART_GetIntStatus(config->id, UART_INT_RTO) == SET) {
		UART_IntClear(config->id, UART_INT_RTO);
	}
	if (UART_GetIntStatus(config->id, UART_INT_RX_END) == SET) {
		UART_IntClear(config->id, UART_INT_RX_END);
	}
	if (UART_GetIntStatus(config->id, UART_INT_TX_END) == SET) {
		UART_IntClear(config->id, UART_INT_TX_END);
	}
#endif
}

static int uart_ipro_init(const struct device *dev)
{
	const struct uart_ipro_config *config = dev->config;
	struct uart_ipro_data *data = dev->data;
	int ret;

	ret = clock_control_on(config->clock_dev, config->clock_id);
	if (ret != 0) {
		return ret;
	}

	ret = pinctrl_apply_state(config->pincfg, PINCTRL_STATE_DEFAULT);
	if (ret != 0) {
		return ret;
	}

	ret = uart_ipro_apply(dev, &data->cfg);
	if (ret != 0) {
		return ret;
	}

#ifdef CONFIG_UART_ASYNC_API
	data->dev = dev;
	k_work_init_delayable(&data->tx_timeout_work, uart_ipro_tx_timeout);
#endif
#if defined(CONFIG_UART_INTERRUPT_DRIVEN) || defined(CONFIG_UART_ASYNC_API)
	config->irq_config_func(dev);
#endif
	return 0;
}

static DEVICE_API(uart, uart_ipro_api) = {
	.poll_in = uart_ipro_poll_in,
	.poll_out = uart_ipro_poll_out,
	.err_check = uart_ipro_err_check,
#ifdef CONFIG_UART_USE_RUNTIME_CONFIGURE
	.configure = uart_ipro_configure,
	.config_get = uart_ipro_config_get,
#endif
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	.fifo_fill = uart_ipro_fifo_fill,
	.fifo_read = uart_ipro_fifo_read,
	.irq_tx_enable = uart_ipro_irq_tx_enable,
	.irq_tx_disable = uart_ipro_irq_tx_disable,
	.irq_tx_ready = uart_ipro_irq_tx_ready,
	.irq_rx_enable = uart_ipro_irq_rx_enable,
	.irq_rx_disable = uart_ipro_irq_rx_disable,
	.irq_tx_complete = uart_ipro_irq_tx_complete,
	.irq_rx_ready = uart_ipro_irq_rx_ready,
	.irq_err_enable = uart_ipro_irq_err_enable,
	.irq_err_disable = uart_ipro_irq_err_disable,
	.irq_is_pending = uart_ipro_irq_is_pending,
	.irq_update = uart_ipro_irq_update,
	.irq_callback_set = uart_ipro_irq_callback_set,
#endif
#ifdef CONFIG_UART_ASYNC_API
	.callback_set = uart_ipro_callback_set,
	.tx = uart_ipro_tx,
	.tx_abort = uart_ipro_tx_abort,
	.rx_enable = uart_ipro_rx_enable,
	.rx_buf_rsp = uart_ipro_rx_buf_rsp,
	.rx_disable = uart_ipro_rx_disable,
#endif
};

#if defined(CONFIG_UART_INTERRUPT_DRIVEN) || defined(CONFIG_UART_ASYNC_API)
#define UART_IPRO_IRQ_CONFIG(n)                                                                    \
	static void uart_ipro_irq_config_##n(const struct device *dev)                             \
	{                                                                                          \
		IRQ_CONNECT(DT_INST_IRQN(n), DT_INST_IRQ(n, priority), uart_ipro_isr_common,       \
			    DEVICE_DT_INST_GET(n), 0);                                             \
		irq_enable(DT_INST_IRQN(n));                                                       \
	}
#define UART_IPRO_IRQ_CONFIG_INIT(n) .irq_config_func = uart_ipro_irq_config_##n,
#else
#define UART_IPRO_IRQ_CONFIG(n)
#define UART_IPRO_IRQ_CONFIG_INIT(n)
#endif

#ifdef CONFIG_UART_ASYNC_API
#define UART_IPRO_DMA_INIT(n)                                                                      \
	.dma_dev = COND_CODE_1(DT_INST_NODE_HAS_PROP(n, dmas),                                     \
			       (DEVICE_DT_GET(DT_INST_DMAS_CTLR_BY_NAME(n, tx))), (NULL)),         \
	.tx_dma_ch = COND_CODE_1(DT_INST_NODE_HAS_PROP(n, dmas),                                   \
				 (DT_INST_DMAS_CELL_BY_NAME(n, tx, channel)), (0)),                \
	.rx_dma_ch = COND_CODE_1(DT_INST_NODE_HAS_PROP(n, dmas),                                   \
				 (DT_INST_DMAS_CELL_BY_NAME(n, rx, channel)), (0)),                \
	.tx_dma_slot = COND_CODE_1(DT_INST_NODE_HAS_PROP(n, dmas),                                 \
				   (DT_INST_DMAS_CELL_BY_NAME(n, tx, slot)), (0)),                 \
	.rx_dma_slot = COND_CODE_1(DT_INST_NODE_HAS_PROP(n, dmas),                                 \
				   (DT_INST_DMAS_CELL_BY_NAME(n, rx, slot)), (0)),                 \
	.has_dma = DT_INST_NODE_HAS_PROP(n, dmas),
#else
#define UART_IPRO_DMA_INIT(n)
#endif

#define UART_IPRO_INIT(n)                                                                          \
	PINCTRL_DT_INST_DEFINE(n);                                                                 \
	UART_IPRO_IRQ_CONFIG(n)                                                                    \
	static const struct uart_ipro_config uart_ipro_cfg_##n = {                                 \
		.id = (UART_ID_Type)(DT_INST_REG_ADDR(n) == UART0_BASE ? UART0_ID : UART1_ID),     \
		.base = DT_INST_REG_ADDR(n),                                                       \
		.pincfg = PINCTRL_DT_INST_DEV_CONFIG_GET(n),                                       \
		.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(n)),                                \
		.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(n, id),                    \
		.rx_timeout_bits = DT_INST_PROP(n, rx_timeout_bits),                               \
		UART_IPRO_IRQ_CONFIG_INIT(n) UART_IPRO_DMA_INIT(n)};                                                     \
	static struct uart_ipro_data uart_ipro_data_##n = {                                        \
		.cfg = {                                                                           \
			.baudrate = DT_INST_PROP(n, current_speed),                                \
			.parity = DT_INST_ENUM_IDX_OR(n, parity, UART_CFG_PARITY_NONE),            \
			.stop_bits = DT_INST_ENUM_IDX_OR(n, stop_bits, UART_CFG_STOP_BITS_1),      \
			.data_bits = DT_INST_ENUM_IDX_OR(n, data_bits, UART_CFG_DATA_BITS_8),      \
			.flow_ctrl = DT_INST_PROP(n, hw_flow_control) ? UART_CFG_FLOW_CTRL_RTS_CTS  \
								      : UART_CFG_FLOW_CTRL_NONE,     \
		}};                                                                                \
	DEVICE_DT_INST_DEFINE(n, uart_ipro_init, NULL, &uart_ipro_data_##n, &uart_ipro_cfg_##n,    \
			      PRE_KERNEL_1, CONFIG_SERIAL_INIT_PRIORITY, &uart_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(UART_IPRO_INIT)
