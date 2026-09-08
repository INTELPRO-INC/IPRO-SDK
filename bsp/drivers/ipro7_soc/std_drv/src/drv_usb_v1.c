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

#include "ipro7.h"
#include "drv_common.h"

#include "drv_glb.h"
#include "drv_dma.h"
#include "hal_dma.h"
#include "usbd_core.h"
#include "usb_v1_reg.h"

#define _GLB_USB_XCVR_OFFSET        (0x228) /* usb_xcvr */
#define _GLB_USB_XCVR_CONFIG_OFFSET (0x22C) /* usb_xcvr_config */

/* 0x228 : usb_xcvr */
#define _GLB_USB_LDO_VFB_SHIFT   (0U)
#define _GLB_USB_LDO_VFB_MASK    (0x7 << _GLB_USB_LDO_VFB_SHIFT)
#define _GLB_PU_USB_LDO          (1 << 3U)
#define _GLB_USB_ROUT_NMOS_SHIFT (4U)
#define _GLB_USB_ROUT_NMOS_MASK  (0x7 << _GLB_USB_ROUT_NMOS_SHIFT)
#define _GLB_USB_ROUT_PMOS_SHIFT (8U)
#define _GLB_USB_ROUT_PMOS_MASK  (0x7 << _GLB_USB_ROUT_PMOS_SHIFT)
#define _GLB_USB_OEB_SEL         (1 << 12U)
#define _GLB_USB_OEB_REG         (1 << 13U)
#define _GLB_USB_OEB             (1 << 14U)
#define _GLB_USB_DATA_CONVERT    (1 << 16U)
#define _GLB_USB_ENUM            (1 << 20U)
#define _GLB_USB_SPD             (1 << 21U)
#define _GLB_USB_SUS             (1 << 22U)
#define _GLB_PU_USB              (1 << 23U)
#define _GLB_USB_BD              (1 << 24U)
#define _GLB_USB_VIM             (1 << 25U)
#define _GLB_USB_VIP             (1 << 26U)
#define _GLB_USB_RCV             (1 << 27U)

/* 0x22C : usb_xcvr_config */
#define _GLB_USB_V_HYS_M_SHIFT         (0U)
#define _GLB_USB_V_HYS_M_MASK          (0x3 << _GLB_USB_V_HYS_M_SHIFT)
#define _GLB_USB_V_HYS_P_SHIFT         (2U)
#define _GLB_USB_V_HYS_P_MASK          (0x3 << _GLB_USB_V_HYS_P_SHIFT)
#define _GLB_USB_BD_VTH_SHIFT          (4U)
#define _GLB_USB_BD_VTH_MASK           (0x7 << _GLB_USB_BD_VTH_SHIFT)
#define _GLB_REG_USB_USE_XCVR          (1 << 7U)
#define _GLB_USB_STR_DRV_SHIFT         (8U)
#define _GLB_USB_STR_DRV_MASK          (0x7 << _GLB_USB_STR_DRV_SHIFT)
#define _GLB_REG_USB_USE_CTRL          (1 << 11U)
#define _GLB_USB_RES_PULLUP_TUNE_SHIFT (12U)
#define _GLB_USB_RES_PULLUP_TUNE_MASK  (0x7 << _GLB_USB_RES_PULLUP_TUNE_SHIFT)
#define _GLB_USB_SLEWRATE_M_FALL_SHIFT (16U)
#define _GLB_USB_SLEWRATE_M_FALL_MASK  (0x7 << _GLB_USB_SLEWRATE_M_FALL_SHIFT)
#define _GLB_USB_SLEWRATE_M_RISE_SHIFT (20U)
#define _GLB_USB_SLEWRATE_M_RISE_MASK  (0x7 << _GLB_USB_SLEWRATE_M_RISE_SHIFT)
#define _GLB_USB_SLEWRATE_P_FALL_SHIFT (24U)
#define _GLB_USB_SLEWRATE_P_FALL_MASK  (0x7 << _GLB_USB_SLEWRATE_P_FALL_SHIFT)
#define _GLB_USB_SLEWRATE_P_RISE_SHIFT (28U)
#define _GLB_USB_SLEWRATE_P_RISE_MASK  (0x7 << _GLB_USB_SLEWRATE_P_RISE_SHIFT)

#define USB_NUM_BIDIR_ENDPOINTS 8

/* Endpoint state */
struct ep_state {
    uint16_t ep_mps;    /* Endpoint max packet size */
    uint8_t ep_type;    /* Endpoint type */
    uint8_t ep_stalled; /* Endpoint stall flag */
    uint8_t ep_enable;  /* Endpoint enable */
    uint8_t *xfer_buf;
    uint32_t xfer_len;
    uint32_t actual_xfer_len;
};

ATTR_NOCACHE_RAM_SECTION struct udc_t {
    struct usb_setup_packet setup;
    struct ep_state in_ep[USB_NUM_BIDIR_ENDPOINTS];  /*!< IN endpoint parameters             */
    struct ep_state out_ep[USB_NUM_BIDIR_ENDPOINTS]; /*!< OUT endpoint parameters            */
} g_udc;

static bool g_vbus_last; /* Last known VBUS state for edge dedup */

/* USB interrupt counters for diagnostics (updated in ISR, read from task) */
volatile usb_irq_stats_t g_usb_irq_stats;

/* Per-endpoint DMA RX state for hardware-handshake FIFO reads */
static struct {
    uint8_t ch;            /* DMA channel index */
    uint8_t enabled;       /* 1 = DMA P2M active for this EP */
    uint8_t raw;           /* 1 = raw bulk (skip NCM/NTB framing checks in the ISR) */
    uint32_t last_remain;  /* TransferSize after last EP_DONE */
} g_ep_dma_rx[USB_NUM_BIDIR_ENDPOINTS];

/* Per-endpoint DMA TX state for hardware-handshake FIFO writes (M2P). When enabled,
 * the IN EP-done ISR re-readies the EP per packet but does NOT mem2fifo — the DMA fills
 * the FIFO via the dma_tx_req handshake; the ISR only tracks bytes for completion. */
static struct {
    uint8_t ch;            /* DMA channel index */
    uint8_t enabled;       /* 1 = DMA M2P active for this EP */
} g_ep_dma_tx[USB_NUM_BIDIR_ENDPOINTS];

/* DMA ISR trace buffer for diagnostics */
#define DMA_TRACE_SIZE 64
volatile struct {
    uint32_t last_remain;
    uint32_t remain;
    uint32_t pkt_bytes;
    uint32_t xfer_len;
    uint8_t  branch;  /* 0=chunk_done, 1=short, 2=more */
    uint8_t  fifo_to; /* FIFO empty timeout flag */
    uint8_t  rx_cnt;  /* FIFO RX byte count at entry */
} g_dma_trace[DMA_TRACE_SIZE];
volatile uint32_t g_dma_trace_idx;

void USBD_IRQHandler(void);

static void drv_usb_mem2fifo(uint8_t ep_idx, uint8_t *data, uint32_t length)
{
    uint32_t fifo_addr = USB_BASE + USB_EP0_TX_FIFO_WDATA_OFFSET + ep_idx * 0x10;

    volatile uint8_t *p = (volatile uint8_t *)fifo_addr;
    uint8_t *q = data;

    while (length--) {
        *p = *q++;
    }
}

static void drv_usb_fifo2mem(uint8_t ep_idx, uint8_t *data, uint32_t length)
{
    uint32_t fifo_addr = USB_BASE + USB_EP0_RX_FIFO_RDATA_OFFSET + ep_idx * 0x10;

    volatile uint8_t *p = (volatile uint8_t *)fifo_addr;
    uint8_t *q = data;

    while (length--) {
        *q++ = *p;
    }
}

static void drv_usb_xcvr_config(void)
{
    uint32_t regval;

    regval = getreg32(GLB_BASE + _GLB_USB_XCVR_OFFSET);
    regval |= _GLB_PU_USB;
    putreg32(regval, GLB_BASE + _GLB_USB_XCVR_OFFSET);

    regval = getreg32(GLB_BASE + _GLB_USB_XCVR_OFFSET);
    regval &= ~_GLB_USB_SUS;
    regval |= _GLB_USB_SPD; /* 0 for 1.1 ls,1 for 1.1 fs */
    regval &= ~_GLB_USB_DATA_CONVERT;
    regval &= ~_GLB_USB_OEB_SEL;
    regval &= ~_GLB_USB_ROUT_PMOS_MASK;
    regval &= ~_GLB_USB_ROUT_NMOS_MASK;
    regval |= (3 << _GLB_USB_ROUT_PMOS_SHIFT);
    regval |= (3 << _GLB_USB_ROUT_NMOS_SHIFT);
    putreg32(regval, GLB_BASE + _GLB_USB_XCVR_OFFSET);

    regval = 0;
    regval |= (2 << _GLB_USB_V_HYS_M_SHIFT);
    regval |= (2 << _GLB_USB_V_HYS_P_SHIFT);
    regval |= (7 << _GLB_USB_BD_VTH_SHIFT);
    regval |= _GLB_REG_USB_USE_XCVR;
    regval |= _GLB_REG_USB_USE_CTRL;
    regval |= (0 << _GLB_USB_STR_DRV_SHIFT);
    regval |= (5 << _GLB_USB_RES_PULLUP_TUNE_SHIFT);
    regval |= (2 << _GLB_USB_SLEWRATE_M_FALL_SHIFT);
    regval |= (2 << _GLB_USB_SLEWRATE_M_RISE_SHIFT);
    regval |= (2 << _GLB_USB_SLEWRATE_P_FALL_SHIFT);
    regval |= (2 << _GLB_USB_SLEWRATE_P_RISE_SHIFT);
    putreg32(regval, GLB_BASE + _GLB_USB_XCVR_CONFIG_OFFSET);

    regval = getreg32(GLB_BASE + _GLB_USB_XCVR_OFFSET);
    regval |= _GLB_USB_ENUM;
    putreg32(regval, GLB_BASE + _GLB_USB_XCVR_OFFSET);
}

static void drv_usb_ep0_size(uint8_t ep_mps)
{
    uint32_t regval;

    regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
    regval &= ~USB_CR_USB_EP0_SW_SIZE_MASK;
    regval |= (ep_mps << USB_CR_USB_EP0_SW_SIZE_SHIFT);
    putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);
}

static void drv_usb_ep_config(uint8_t ep_idx, uint8_t ep_type, uint8_t dir, uint16_t ep_mps)
{
    uint32_t regval;

    regval = getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
    regval &= ~USB_CR_EP1_SIZE_MASK;
    regval &= ~USB_CR_EP1_TYPE_MASK;
    regval &= ~USB_CR_EP1_DIR_MASK;
    regval |= (ep_mps << USB_CR_EP1_SIZE_SHIFT);
    regval |= (dir << USB_CR_EP1_DIR_SHIFT);
    regval |= (ep_type << USB_CR_EP1_TYPE_SHIFT);
    putreg32(regval, USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
}

static void drv_usb_ep_set_nak(uint8_t ep_idx)
{
    uint32_t regval;

    if (ep_idx == 0) {
        regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
        regval |= USB_CR_USB_EP0_SW_NACK_OUT;
        regval |= USB_CR_USB_EP0_SW_NACK_IN;
        putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);
    } else {
        regval = getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
        regval |= USB_CR_EP1_NACK;
        regval &= ~USB_CR_EP1_STALL;
        putreg32(regval, USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
    }
}

static void drv_usb_ep_set_ready(uint8_t ep_idx)
{
    uint32_t regval;

    if (ep_idx == 0) {
        regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
        regval |= USB_CR_USB_EP0_SW_RDY;
        regval |= USB_CR_USB_EP0_SW_NACK_OUT;
        regval |= USB_CR_USB_EP0_SW_NACK_IN;
        regval &= ~USB_CR_USB_EP0_SW_STALL;
        putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);
    } else {
        regval = getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
        regval |= USB_CR_EP1_RDY;
        regval |= USB_CR_EP1_NACK;
        regval &= ~USB_CR_EP1_STALL;
        putreg32(regval, USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
    }
}

static uint16_t drv_usb_get_rxcount(uint8_t ep_idx)
{
    return (getreg32(USB_BASE + USB_EP0_FIFO_STATUS_OFFSET + 0x10 * ep_idx) & USB_EP0_RX_FIFO_CNT_MASK) >> USB_EP0_RX_FIFO_CNT_SHIFT;
}

/**
 * Clear RX FIFO for the given endpoint (pulse the RX_FIFO_CLR bit).
 * All EPx share the same bit position (bit 3) in their FIFO_CONFIG register.
 */
static void drv_usb_ep_fifo_rx_clear(uint8_t ep_idx)
{
    uint32_t addr = USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx;
    uint32_t regval = getreg32(addr);
    regval |= (1 << 3U);   /* EPx_RX_FIFO_CLR */
    putreg32(regval, addr);
    regval &= ~(1 << 3U);
    putreg32(regval, addr);
}

/**
 * Clear an endpoint's hardware FIFO; the direction comes from the EP address
 * (IN -> TX FIFO, bit 2; OUT -> RX FIFO, bit 3 -- same positions on every EP).
 * Public so class glue can start a NEW connection from an empty FIFO: neither
 * usbd_ep_open() nor the bus-reset path clears the hardware FIFOs, so a loaded
 * IN packet the old host never polled out -- or an OUT packet a completion
 * mishap left parked -- would otherwise leak into the next session as its
 * first packet. A plain register pulse, callable from any context.
 */
int usbd_ep_fifo_clear(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t addr, regval, bit;

    if (ep_idx >= USB_NUM_BIDIR_ENDPOINTS) {
        return -1;
    }
    addr = USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx;
    bit = USB_EP_DIR_IS_IN(ep) ? (1U << 2) : (1U << 3);
    regval = getreg32(addr);
    putreg32(regval | bit, addr);
    putreg32(regval & ~bit, addr);
    return 0;
}

// static uint16_t drv_usb_get_txcount(uint8_t ep_idx)
// {
//     return (getreg32(USB_BASE + USB_EP0_FIFO_STATUS_OFFSET + 0x10 * ep_idx) & USB_EP0_TX_FIFO_CNT_MASK) >> USB_EP0_TX_FIFO_CNT_SHIFT;
// }

static bool drv_usb_ep_isbusy(uint8_t ep_idx)
{
    uint32_t regval;

    if (ep_idx == 0) {
        regval = (getreg32(USB_BASE + USB_CONFIG_OFFSET) & USB_STS_USB_EP0_SW_RDY);
    } else {
        regval = (getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1)) & USB_STS_EP1_RDY);
    }
    return regval ? true : false;
}

static uint32_t drv_usb_get_intstatus(void)
{
    uint32_t regval;

    regval = getreg32(USB_BASE + USB_INT_STS_OFFSET);
    regval &= ~getreg32(USB_BASE + USB_INT_MASK_OFFSET);
    regval &= getreg32(USB_BASE + USB_INT_EN_OFFSET);
    return regval;
}

static void drv_usb_int_clear(uint32_t int_clear)
{
    uint32_t regval;
    regval = getreg32(USB_BASE + USB_INT_CLEAR_OFFSET);
    regval |= int_clear;
    putreg32(regval, USB_BASE + USB_INT_CLEAR_OFFSET);
}

int usb_dc_init(void)
{
    uint32_t regval;
    uint8_t pinList[2] = { GLB_GPIO_PIN_8, GLB_GPIO_PIN_9 };

    GLB_GPIO_Func_Init(GPIO_FUN_ANALOG, pinList, 2);

    drv_usb_xcvr_config();

    /* disable USB first */
    regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
    regval &= ~USB_CR_USB_EN;
    putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);

    /* USB ep0 config */
    regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
    regval |= USB_CR_USB_EP0_SW_CTRL;
    regval &= ~USB_CR_USB_EP0_SW_ADDR_MASK;
    regval &= ~USB_CR_USB_EP0_SW_SIZE_MASK;
    regval |= (64 << USB_CR_USB_EP0_SW_SIZE_SHIFT);
    regval |= USB_CR_USB_EP0_SW_NACK_IN;
    regval |= USB_CR_USB_EP0_SW_NACK_OUT;
    regval &= ~USB_CR_USB_ROM_DCT_EN;
    putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);

    regval = 0;
    regval |= USB_CR_USB_RESET_EN;
    regval |= USB_CR_VBUS_TGL_EN;
    regval |= USB_CR_EP0_SETUP_DONE_EN;
    regval |= USB_CR_EP0_IN_DONE_EN;
    regval |= USB_CR_EP0_OUT_DONE_EN;
    regval |= USB_CR_USB_REND_EN;
    putreg32(regval, USB_BASE + USB_INT_EN_OFFSET);

    regval = 0xffffffff;
    regval &= ~USB_CR_USB_RESET_MASK;
    regval &= ~USB_CR_VBUS_TGL_MASK;
    regval &= ~USB_CR_EP0_SETUP_DONE_MASK;
    regval &= ~USB_CR_EP0_IN_DONE_MASK;
    regval &= ~USB_CR_EP0_OUT_DONE_MASK;
    regval &= ~USB_CR_USB_REND_MASK;
    putreg32(regval, USB_BASE + USB_INT_MASK_OFFSET);

    putreg32(0xffffffff, USB_BASE + USB_INT_CLEAR_OFFSET);

    /* Use higher priority (7) for USB to avoid interference from BLE stack
     * which frequently disables/enables global interrupts */
    ECLIC_Register_IRQ(USB_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER,
                       1, 7, USBD_IRQHandler);
    CPU_Interrupt_Enable(USB_IRQn);

    /* enable usb */
    regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
    regval |= USB_CR_USB_EN;
    putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);

    return 0;
}

int usb_dc_deinit(void)
{
    uint32_t regval;

    regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
    regval &= ~USB_CR_USB_EN;
    putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);

    regval = getreg32(GLB_BASE + _GLB_USB_XCVR_OFFSET);
    regval &= ~_GLB_USB_ENUM;
    putreg32(regval, GLB_BASE + _GLB_USB_XCVR_OFFSET);

    return 0;
}

int usbd_set_address(const uint8_t addr)
{
    uint32_t regval;

    regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
    regval &= ~USB_CR_USB_EP0_SW_ADDR_MASK;
    regval |= (addr << USB_CR_USB_EP0_SW_ADDR_SHIFT);
    putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);

    return 0;
}

uint8_t usbd_get_port_speed(const uint8_t port)
{
    return USB_SPEED_FULL;
}

int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg)
{
    uint8_t ep;
    uint8_t ep_type;
    uint8_t dir;
    uint32_t regval;

    ep = ep_cfg->ep_addr;

    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (ep_idx > USB_NUM_BIDIR_ENDPOINTS) {
        return -1;
    }

    if (USB_EP_DIR_IS_OUT(ep)) {
        g_udc.out_ep[ep_idx].ep_mps = ep_cfg->ep_mps;
        g_udc.out_ep[ep_idx].ep_type = ep_cfg->ep_type;
        g_udc.out_ep[ep_idx].ep_enable = 1U;
        dir = 2;
    } else {
        g_udc.in_ep[ep_idx].ep_mps = ep_cfg->ep_mps;
        g_udc.in_ep[ep_idx].ep_type = ep_cfg->ep_type;
        g_udc.in_ep[ep_idx].ep_enable = 1U;
        dir = 1;
    }

    if (ep_idx != 0) {
        switch (ep_cfg->ep_type) {
            case 1:
                ep_type = 2;
                break;
            case 2:
                ep_type = 4;
                break;
            case 3:
                ep_type = 0;
                break;

            default:
                ep_type = 4;
                break;
        }
        drv_usb_ep_config(ep_idx, ep_type, dir, ep_cfg->ep_mps);

        regval = getreg32(USB_BASE + USB_INT_EN_OFFSET);
        regval |= (1 << (9 + ep_idx * 2));
        putreg32(regval, USB_BASE + USB_INT_EN_OFFSET);

        regval = getreg32(USB_BASE + USB_INT_MASK_OFFSET);
        regval &= ~(1 << (9 + ep_idx * 2));
        putreg32(regval, USB_BASE + USB_INT_MASK_OFFSET);

        drv_usb_ep_set_nak(ep_idx);
    }
    return 0;
}

int usbd_ep_close(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t regval;

    if (ep_idx == 0)
        return 0;

    /* Disable EP_DONE interrupt */
    regval = getreg32(USB_BASE + USB_INT_EN_OFFSET);
    regval &= ~(1 << (9 + ep_idx * 2));
    putreg32(regval, USB_BASE + USB_INT_EN_OFFSET);

    regval = getreg32(USB_BASE + USB_INT_MASK_OFFSET);
    regval |= (1 << (9 + ep_idx * 2));
    putreg32(regval, USB_BASE + USB_INT_MASK_OFFSET);

    /* Force NAK so host sees NAK immediately */
    drv_usb_ep_set_nak(ep_idx);

    /* Stop DMA before clearing FIFO to prevent DMA from reading
     * bytes in flight during the FIFO clear pulse.  Without this,
     * DMA can consume 1-2 bytes from the pipeline, causing the
     * next NTB to start at an offset (observed as 2-byte shift). */
    if (USB_EP_DIR_IS_OUT(ep) && g_ep_dma_rx[ep_idx].enabled) {
        uint8_t dch = g_ep_dma_rx[ep_idx].ch;
        DMA_Channel_Disable(DMA0_ID, dch);
        /* Wait for DMA_A (Active) bit to clear */
        uint32_t dma_cfg_addr = DMA_BASE + 0x110 + dch * 0x100;
        for (volatile int i = 0; i < 500; i++) {
            if (!(getreg32(dma_cfg_addr) & (1U << 17)))
                break;
        }
        g_ep_dma_rx[ep_idx].last_remain = 0;
    }

    /* Clear RX FIFO to discard stale data */
    drv_usb_ep_fifo_rx_clear(ep_idx);

    /* Mark endpoint as disabled */
    if (USB_EP_DIR_IS_IN(ep)) {
        g_udc.in_ep[ep_idx].ep_enable = false;
        g_udc.in_ep[ep_idx].xfer_buf = NULL;
        g_udc.in_ep[ep_idx].xfer_len = 0;
        g_udc.in_ep[ep_idx].actual_xfer_len = 0;
    } else {
        g_udc.out_ep[ep_idx].ep_enable = false;
        g_udc.out_ep[ep_idx].xfer_buf = NULL;
        g_udc.out_ep[ep_idx].xfer_len = 0;
        g_udc.out_ep[ep_idx].actual_xfer_len = 0;
    }

    return 0;
}

int usbd_ep_set_stall(const uint8_t ep)
{
    uint32_t regval;

    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (ep_idx == 0) {
        regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
        regval |= USB_CR_USB_EP0_SW_RDY;
        regval |= USB_CR_USB_EP0_SW_NACK_OUT;
        regval |= USB_CR_USB_EP0_SW_NACK_IN;
        regval |= USB_CR_USB_EP0_SW_STALL;
        putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);

    } else {
        regval = getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
        regval |= USB_CR_EP1_STALL;
        putreg32(regval, USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
    }

    return 0;
}

int usbd_ep_clear_stall(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (ep_idx == 0) {
    } else {
        uint32_t addr = USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1);
        uint32_t regval = getreg32(addr);

        /* Clear STALL, keep NACK (idle state), do NOT trigger RDY.
         * The old code called drv_usb_ep_set_ready() which wrote
         * CR_RDY=1 — this armed an empty TX FIFO, causing the host
         * to receive a spurious ZLP/underflow and re-HALT the EP.
         * Correct behavior: just clear STALL and wait for the next
         * usbd_ep_start_write() to fill FIFO and trigger RDY. */
        regval &= ~USB_CR_EP1_STALL;
        regval |= USB_CR_EP1_NACK;
        regval &= ~USB_CR_EP1_RDY;
        putreg32(regval, addr);

        USB_LOG_DBG("clear_stall ep:%02x idx=%u cfg=0x%08lx\r\n",
                    ep, ep_idx, (unsigned long)getreg32(addr));
    }

    return 0;
}

int usbd_ep_is_stalled(const uint8_t ep, uint8_t *stalled)
{
    return 0;
}

int usbd_ep_start_write(const uint8_t ep, const uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!data && data_len) {
        return -1;
    }

    if (!g_udc.in_ep[ep_idx].ep_enable) {
        return -2;
    }

    g_udc.in_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_udc.in_ep[ep_idx].xfer_len = data_len;
    g_udc.in_ep[ep_idx].actual_xfer_len = 0;

    data_len = MIN(data_len, g_udc.in_ep[ep_idx].ep_mps);

    if (ep_idx == 0) {
        if (data_len == 1) {
            drv_usb_ep0_size(1);
        } else {
            drv_usb_ep0_size(g_udc.in_ep[ep_idx].ep_mps);
        }
    }

    drv_usb_mem2fifo(ep_idx, g_udc.in_ep[ep_idx].xfer_buf, data_len);
    drv_usb_ep_set_ready(ep_idx);

    return 0;
}

/* Register a DMA channel for M2P (mem->FIFO) writes on a BULK IN EP. */
void usbd_ep_setup_dma_tx(const uint8_t ep, uint8_t dma_ch)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    g_ep_dma_tx[ep_idx].ch = dma_ch;
    g_ep_dma_tx[ep_idx].enabled = 1;
}

/* Arm a BULK IN transfer whose FIFO is filled by DMA (not CPU mem2fifo). The caller
 * must have armed the DMA channel (reload + start) BEFORE calling this. This sets the
 * EP transfer state, enables DMA_TX_EN so the EP drives the dma_tx_req handshake, and
 * readies the EP. Completion fires via the IN EP-done ISR (xfer_len -> 0) -> the
 * usbd_event_ep_in_complete_handler callback (like the CPU path), NOT the DMA IRQ. */
int usbd_ep_start_write_dma(const uint8_t ep, const uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t regval;

    if (!data && data_len) {
        return -1;
    }
    if (!g_udc.in_ep[ep_idx].ep_enable) {
        return -2;
    }

    g_udc.in_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_udc.in_ep[ep_idx].xfer_len = data_len;
    g_udc.in_ep[ep_idx].actual_xfer_len = 0;

    /* DMA_TX_EN: the EP asserts dma_tx_req whenever the TX FIFO has room, and the DMA
     * (dstPeriph = this EP) services it — filling the FIFO with no CPU copy. */
    regval = getreg32(USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx);
    regval |= (1 << 0U);
    putreg32(regval, USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx);

    drv_usb_ep_set_ready(ep_idx);   /* EP_RDY: send the FIFO packets on host IN tokens */
    return 0;
}

int usbd_ep_start_iso_write(const uint8_t ep, const uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!data && data_len) {
        return -1;
    }

    if (!g_udc.in_ep[ep_idx].ep_enable) {
        return -2;
    }

    g_udc.in_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_udc.in_ep[ep_idx].xfer_len = data_len;
    g_udc.in_ep[ep_idx].actual_xfer_len = 0;

    uint32_t fifo_addr = USB_BASE + USB_EP0_TX_FIFO_WDATA_OFFSET + ep_idx * 0x10;
    uint8_t fifo_full;
    uint8_t start_trans = 0;

    uint8_t *p = (uint8_t *)fifo_addr;
    const uint8_t *q = data;

    while (1) {
        fifo_full = ((USB_BASE + USB_EP0_FIFO_STATUS_OFFSET + ep_idx * 0x10) >> 15) & 0x1;
        if (fifo_full == 0) {
            data_len--;
            *p = *q++;
        } else {
            if (start_trans) {
                arch_delay_ms(1);
            } else {
                drv_usb_ep_set_ready(ep_idx);
                start_trans = 1;
            }
        }
    }

    usbd_event_ep_in_complete_handler(ep_idx | 0x80, g_udc.in_ep[ep_idx].xfer_len);

    return 0;
}

int usbd_ep_start_read(const uint8_t ep, uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!data && data_len) {
        return -1;
    }

    if (!g_udc.out_ep[ep_idx].ep_enable) {
        return -2;
    }

    g_udc.out_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_udc.out_ep[ep_idx].xfer_len = data_len;
    g_udc.out_ep[ep_idx].actual_xfer_len = 0;

    if (g_ep_dma_rx[ep_idx].enabled) {
        uint8_t ch = g_ep_dma_rx[ep_idx].ch;
        uint32_t dma_len = (data_len > DMA_MAX_TRANSFER_PER_LLI)
                           ? DMA_MAX_TRANSFER_PER_LLI : data_len;
        /* Align DMA chunk to ep_mps so that chunk boundaries never
         * leave residual bytes in FIFO.  Without alignment, the
         * first EP_DONE after a chunk boundary sees residual +
         * new-packet bytes, causing false short-packet detection. */
        uint16_t mps = g_udc.out_ep[ep_idx].ep_mps;
        if (mps && dma_len > mps)
            dma_len = (dma_len / mps) * mps;
        uint32_t fifo_addr = USB_BASE +
            USB_EP0_RX_FIFO_RDATA_OFFSET + ep_idx * 0x10;
        uint32_t dst = (uint32_t)(uintptr_t)data;
        DMA_Channel_Disable(DMA0_ID, ch);
        /* Flush stale FIFO bytes from previous transfer to prevent
         * residual bytes bleeding into the new DMA transfer. */
        drv_usb_ep_fifo_rx_clear(ep_idx);
        DMA_Channel_Update_SrcMemcfg(DMA0_ID, ch, fifo_addr, dma_len);
        DMA_Channel_Update_DstMemcfg(DMA0_ID, ch, dst, dma_len);
        DMA_Channel_Enable(DMA0_ID, ch);
        g_ep_dma_rx[ep_idx].last_remain = dma_len;
    }

    drv_usb_ep_set_ready(ep_idx);

    return 0;
}

/*
 * Maximum busy-wait iterations inside ISR before giving up.
 * With the xfer_buf ghost guard, stale interrupts are caught before
 * reaching wait_ready.  The remaining timeout covers only genuine HW
 * register latency (EP_RDY clear after EP_DONE), which resolves in
 * ~10 clock cycles.  200 iterations ≈ 1us @ 200MHz — sufficient.
 */
#define USB_EP_BUSY_TIMEOUT     5000

static inline bool drv_usb_ep_wait_ready(uint8_t ep_idx)
{
    for (int i = 0; i < USB_EP_BUSY_TIMEOUT; i++) {
        if (!drv_usb_ep_isbusy(ep_idx))
            return true;
    }
    return false;
}

void USBD_IRQHandler(void)
{
    uint32_t regval;
    uint32_t intstatus;
    uint16_t rx_count;
    uint16_t tx_count;

    intstatus = drv_usb_get_intstatus();
    drv_usb_int_clear(intstatus);

    g_usb_irq_stats.total++;
    if (intstatus & USB_RESET_INT)          g_usb_irq_stats.reset++;
    if (intstatus & USB_VBUS_TGL_INT)       g_usb_irq_stats.vbus_tgl++;
    if (intstatus & USB_EP0_SETUP_DONE_INT) g_usb_irq_stats.ep0_setup++;
    if (intstatus & USB_EP0_IN_DONE_INT)    g_usb_irq_stats.ep0_in++;
    if (intstatus & USB_EP0_OUT_DONE_INT)   g_usb_irq_stats.ep0_out++;
    if (intstatus & USB_REND_INT)           g_usb_irq_stats.rend++;
    for (uint8_t i = 1; i < USB_NUM_BIDIR_ENDPOINTS; i++) {
        if (intstatus & (1 << (9 + 2 * i)))
            g_usb_irq_stats.ep_done[i - 1]++;
    }

    for (uint8_t ep_idx = 1; ep_idx < USB_NUM_BIDIR_ENDPOINTS; ep_idx++) {
        if (intstatus & (1 << (9 + 2 * ep_idx))) {
            if (g_udc.in_ep[ep_idx].ep_enable) {
                tx_count = MIN(g_udc.in_ep[ep_idx].xfer_len, g_udc.in_ep[ep_idx].ep_mps);
                g_udc.in_ep[ep_idx].xfer_buf += tx_count;
                g_udc.in_ep[ep_idx].xfer_len -= tx_count;
                g_udc.in_ep[ep_idx].actual_xfer_len += tx_count;

                if (g_udc.in_ep[ep_idx].xfer_len == 0) {
                    usbd_event_ep_in_complete_handler(ep_idx | 0x80, g_udc.in_ep[ep_idx].actual_xfer_len);
                } else if (g_ep_dma_tx[ep_idx].enabled) {
                    /* DMA M2P fills the FIFO via the dma_tx_req handshake — do NOT
                     * mem2fifo here; just re-ready the EP for the next packet. */
                    drv_usb_ep_set_ready(ep_idx);
                } else {
                    tx_count = MIN(g_udc.in_ep[ep_idx].xfer_len, g_udc.in_ep[ep_idx].ep_mps);
                    drv_usb_mem2fifo(ep_idx, g_udc.in_ep[ep_idx].xfer_buf, tx_count);
                    drv_usb_ep_set_ready(ep_idx);
                }
            } else {

                g_usb_irq_stats.out_entry++;

                /* Part B: skip stale/ghost EP_DONE when no transfer active. */
                if (g_udc.out_ep[ep_idx].xfer_buf == NULL) {
                    /* Count-only: is the FIFO actually empty here? See the note on
                     * out_ghost_data -- if it is not, this skip is discarding a packet
                     * the host already had ACKed, which breaks NAK backpressure. */
                    if (drv_usb_get_rxcount(ep_idx))
                        g_usb_irq_stats.out_ghost_data++;
                    else
                        g_usb_irq_stats.out_ghost_empty++;
                    continue;
                }

                if (g_ep_dma_rx[ep_idx].enabled) {
                    /* ---- DMA path: hardware handshake reads FIFO ---- */

                    uint8_t dch = g_ep_dma_rx[ep_idx].ch;
                    uint32_t fifo_sts_addr = USB_BASE +
                        USB_EP0_FIFO_STATUS_OFFSET + ep_idx * 0x10;
                    uint32_t fifo_rdata_addr = USB_BASE +
                        USB_EP0_RX_FIFO_RDATA_OFFSET + ep_idx * 0x10;
                    uint8_t entry_rx_cnt =
                        (getreg32(fifo_sts_addr) >> 16) & 0x7F;

                    /* Wait for DMA to drain FIFO completely */
                    int fifo_timeout = 1;
                    for (volatile int i = 0; i < 2000; i++) {
                        if (getreg32(fifo_sts_addr) & (1U << 30)) {
                            fifo_timeout = 0;
                            break;
                        }
                    }
                    /* Wait for DMA to finish writing to memory (DMA_A
                     * clears when the DMA pipeline drains), THEN disable.
                     * Disabling while DMA_A is set truncates writes. */
                    {
                        uint32_t dma_cfg = DMA_BASE + 0x110 + dch * 0x100;
                        for (volatile int i = 0; i < 2000; i++) {
                            if (!(getreg32(dma_cfg) & (1U << 17)))
                                break;
                        }
                    }
                    DMA_Channel_Disable(DMA0_ID, dch);
                    /* Read TransferSize — DMA stopped, value stable */
                    uint32_t remain =
                        DMA_Channel_TranferSize(DMA0_ID, dch);
                    uint32_t saved_lr = g_ep_dma_rx[ep_idx].last_remain;
                    uint32_t pkt_bytes = saved_lr - remain;

                    if (pkt_bytes == 0) {
                        g_usb_irq_stats.out_rxcount_zero++;
                        /* Record rxz trace (br=3) for diagnostics */
                        {
                            uint32_t ti = g_dma_trace_idx++ & (DMA_TRACE_SIZE - 1);
                            g_dma_trace[ti].last_remain = saved_lr;
                            g_dma_trace[ti].remain = remain;
                            g_dma_trace[ti].pkt_bytes = 0;
                            g_dma_trace[ti].xfer_len =
                                g_udc.out_ep[ep_idx].xfer_len;
                            g_dma_trace[ti].branch = 3;
                            g_dma_trace[ti].fifo_to = fifo_timeout;
                            g_dma_trace[ti].rx_cnt = entry_rx_cnt;
                        }
                        DMA_Channel_Enable(DMA0_ID, dch);
                        drv_usb_ep_set_ready(ep_idx);
                        continue;
                    }

                    g_udc.out_ep[ep_idx].actual_xfer_len += pkt_bytes;
                    g_udc.out_ep[ep_idx].xfer_buf += pkt_bytes;
                    g_udc.out_ep[ep_idx].xfer_len -= pkt_bytes;

                    uint16_t ep_mps = g_udc.out_ep[ep_idx].ep_mps;
                    uint8_t br = 255;
                    if (remain == 0 &&
                        g_udc.out_ep[ep_idx].xfer_len > 0) {
                        br = 0;
                        /* DMA chunk done, arm next chunk.
                         * DMA already disabled above. */
                        uint32_t next = g_udc.out_ep[ep_idx].xfer_len;
                        if (next > DMA_MAX_TRANSFER_PER_LLI)
                            next = DMA_MAX_TRANSFER_PER_LLI;
                        /* Align to ep_mps — same as usbd_ep_start_read */
                        if (ep_mps && next > ep_mps)
                            next = (next / ep_mps) * ep_mps;
                        DMA_Channel_Update_SrcMemcfg(DMA0_ID, dch,
                            fifo_rdata_addr, next);
                        DMA_Channel_Update_DstMemcfg(DMA0_ID, dch,
                            (uint32_t)(uintptr_t)
                            g_udc.out_ep[ep_idx].xfer_buf, next);
                        DMA_Channel_Enable(DMA0_ID, dch);
                        g_ep_dma_rx[ep_idx].last_remain = next;
                        drv_usb_ep_set_ready(ep_idx);
                    } else if ((pkt_bytes % ep_mps) != 0 ||
                               g_udc.out_ep[ep_idx].xfer_len == 0) {
                        /* Apparent short packet or buffer full.
                         * TOCTOU race between wait-DMA_A and disable
                         * can cause wrong pkt_bytes.  Cross-check
                         * against NTH16 wBlockLength before declaring
                         * the NTB complete. */
                        uint32_t fifo_sts = getreg32(
                            USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET +
                            0x10 * ep_idx + 4);
                        uint8_t fifo_cnt =
                            (fifo_sts >> 16) & 0x7f;
                        uint32_t actual =
                            g_udc.out_ep[ep_idx].actual_xfer_len;
                        uint8_t *buf_start =
                            g_udc.out_ep[ep_idx].xfer_buf - actual;
                        int ntb_incomplete = 0;

                        if (fifo_cnt > 0 &&
                            g_udc.out_ep[ep_idx].xfer_len > 0) {
                            /* FIFO not drained — re-arm. */
                            ntb_incomplete = 1;
                            br = 4;
                        } else if (!g_ep_dma_rx[ep_idx].raw &&
                                   actual < 12 &&
                                   g_udc.out_ep[ep_idx].xfer_len > 0) {
                            /* Too small for a valid NTH16 (12 bytes).
                             * This is a TOCTOU fragment — re-arm. (NCM only:
                             * a raw-bulk EP legitimately sends <12B packets,
                             * e.g. our 4-byte [u32 ncells], so it must NOT
                             * re-arm here — that corrupts the byte stream.) */
                            ntb_incomplete = 1;
                            br = 6;
                        } else if (!g_ep_dma_rx[ep_idx].raw &&
                                   actual >= 12 &&
                                   g_udc.out_ep[ep_idx].xfer_len > 0) {
                            /* Enough data for NTH16 — peek at header
                             * to validate NTB boundary.  Invalidate
                             * only the 12-byte header region. */
                            for (volatile int w = 0; w < 200; w++) {}
                            MFlushInvalDCacheRange(
                                (uintptr_t)buf_start, 12);
                            uint32_t sig =
                                *(volatile uint32_t *)buf_start;
                            if (sig == 0x484D434E) { /* "NCMH" */
                                uint16_t blen =
                                    *(volatile uint16_t *)
                                    (buf_start + 8);
                                if (blen > actual) {
                                    /* NTB not fully received —
                                     * false short pkt from TOCTOU.
                                     * Re-arm DMA. */
                                    ntb_incomplete = 1;
                                    br = 5;
                                }
                            }
                        }

                        if (ntb_incomplete) {
                            g_ep_dma_rx[ep_idx].last_remain = remain;
                            DMA_Channel_Enable(DMA0_ID, dch);
                            drv_usb_ep_set_ready(ep_idx);
                        } else {
                            br = 1;
                            /* Wait for pending PSRAM writes, then
                             * invalidate D-cache before CPU reads. NCM skips
                             * the first 12B (NTH16 header, peeked above); a
                             * raw EP has REAL data there, so invalidate the
                             * whole buffer (the +12 skip left the batch's
                             * first cells stale -> flaky boundary cells). */
                            for (volatile int i = 0; i < 2000; i++) {}
                            if (g_ep_dma_rx[ep_idx].raw)
                                MFlushInvalDCacheRange((uintptr_t)buf_start, actual);
                            else if (actual > 12)
                                MFlushInvalDCacheRange(
                                    (uintptr_t)(buf_start + 12),
                                    actual - 12);
                            g_udc.out_ep[ep_idx].xfer_buf = NULL;
                            g_udc.out_ep[ep_idx].xfer_len = 0;
                            g_udc.out_ep[ep_idx].actual_xfer_len = 0;
                            g_usb_irq_stats.out_short_pkt++;
                            usbd_event_ep_out_complete_handler(
                                ep_idx, actual);
                        }
                    } else {
                        br = 2;
                        g_ep_dma_rx[ep_idx].last_remain = remain;
                        DMA_Channel_Enable(DMA0_ID, dch);
                        drv_usb_ep_set_ready(ep_idx);
                    }
                    /* Record trace entry (ring buffer — keeps the LAST 64) */
                    {
                        uint32_t ti = g_dma_trace_idx++ & (DMA_TRACE_SIZE - 1);
                        g_dma_trace[ti].last_remain = saved_lr;
                        g_dma_trace[ti].remain = remain;
                        g_dma_trace[ti].pkt_bytes = pkt_bytes;
                        g_dma_trace[ti].xfer_len =
                            g_udc.out_ep[ep_idx].xfer_len;
                        g_dma_trace[ti].branch = br;
                        g_dma_trace[ti].fifo_to = fifo_timeout;
                        g_dma_trace[ti].rx_cnt = entry_rx_cnt;
                    }
                    continue;
                }

                /* ---- CPU path: manual FIFO read (non-DMA endpoints) ---- */

                /* Skip EP_RDY wait for bulk endpoints — the IPRO7 USB
                 * controller sometimes delays clearing STS_EP_RDY for
                 * tens of microseconds after EP_DONE, but FIFO data is
                 * already valid.  Waiting caused 1-4% packet loss and
                 * permanently blocked the OUT pipe.  Read FIFO directly;
                 * if empty, treat as ghost interrupt. */
                if (g_udc.out_ep[ep_idx].ep_type == 1) { /* isochronous */
                    /* ISO: data is ready when interrupt fires, no wait */
                } else if (!drv_usb_ep_isbusy(ep_idx)) {
                    /* Normal: EP_RDY already cleared — fast path */
                } else {
                    /* EP_RDY still set — brief wait then proceed */
                    g_usb_irq_stats.out_busy_timeout++;
                    for (volatile int i = 0; i < 200; i++) {}
                }

                rx_count = drv_usb_get_rxcount(ep_idx);

                /* RX_FIFO_CNT is NOT final when EP_DONE fires: the count
                 * climbs as bytes commit (~1.5 B/µs at FS wire rate, plus a
                 * tens-of-µs lag on this controller — same family of quirk
                 * as the STS_EP_RDY lag above). Two historical bugs lived
                 * here:
                 *   1. Reading the count ONCE and trusting it: an early read
                 *      returns a PARTIAL count; fifo2mem then takes only part
                 *      of the packet and leaves its tail in the FIFO — the
                 *      byte-stream goes permanently out of phase and every
                 *      later frame is garbage.
                 *   2. The ~1µs ghost retry + "defensive" fifo_rx_clear:
                 *      under-waiting the lag declared a live packet a ghost
                 *      and the clear DESTROYED it after the host already got
                 *      its ACK (silent loss; on CDC-ECM -> truncated frames,
                 *      lwIP ip.lenerr, worse under 802.15.4 IRQ load).
                 * Robust rule: accept the count only after two consecutive
                 * equal NON-ZERO reads ~1µs apart. Equal across 1µs means no
                 * byte arrived in that window — at FS wire rate the packet is
                 * fully committed. The host cannot start the next packet
                 * until set_ready, so the count cannot over-run into frame
                 * N+1. Budget ~128µs (> 42µs max packet wire time + lag). */
                {
                    uint32_t first = rx_count;
                    uint32_t prev  = rx_count;
                    int stable = 0;
                    if (first == 0) g_usb_irq_stats.out_rxcount_zero++;
                    for (int retry = 0; retry < 128; retry++) {
                        for (volatile int i = 0; i < 100; i++) {}
                        uint32_t c = drv_usb_get_rxcount(ep_idx);
                        if (c != 0 && c == prev) { rx_count = c; stable = 1; break; }
                        prev = c;
                    }
                    if (!stable) {
                        g_usb_irq_stats.out_rxz_still++;
                        /* Nothing committed within the budget: spurious
                         * EP_DONE. Re-arm WITHOUT clearing the FIFO — a
                         * clear races a late commit and wipes an ACKed
                         * packet; an empty FIFO needs no clearing. */
                        drv_usb_ep_set_ready(ep_idx);
                        continue;
                    }
                    /* "recovered" = the first read would have lied (zero or
                     * partial) and the stability wait saved the packet. */
                    if (first != rx_count) g_usb_irq_stats.out_rxz_recovered++;
                }

                /* Never copy more than the transfer armed for. rx_count above
                 * xfer_len means the FIFO holds MORE than one armed packet -- a
                 * packet a previous mis-judged "spurious EP_DONE" left behind,
                 * coalesced with the new one. Without the clamp, fifo2mem
                 * overruns xfer_buf and the subtraction below underflows
                 * xfer_len to ~4G, after which the completion handler never
                 * fires again (every packet takes the more-data branch) -- a
                 * permanent, silent OUT wedge. Clamped, the overflow bytes stay
                 * in the FIFO and the event is COUNTED; the stream may be
                 * mis-framed but the endpoint stays alive and the counter names
                 * the culprit. */
                if (rx_count > g_udc.out_ep[ep_idx].xfer_len) {
                    g_usb_irq_stats.out_rxcount_over++;
                    rx_count = g_udc.out_ep[ep_idx].xfer_len;
                }

                /* Read data from FIFO */
                drv_usb_fifo2mem(ep_idx, g_udc.out_ep[ep_idx].xfer_buf, rx_count);

                g_udc.out_ep[ep_idx].xfer_buf += rx_count;
                g_udc.out_ep[ep_idx].xfer_len -= rx_count;
                g_udc.out_ep[ep_idx].actual_xfer_len += rx_count;

                if ((rx_count < g_udc.out_ep[ep_idx].ep_mps) ||
                    (g_udc.out_ep[ep_idx].xfer_len == 0)) {
                    uint32_t actual = g_udc.out_ep[ep_idx].actual_xfer_len;
                    if (g_udc.out_ep[ep_idx].xfer_len == 0 && rx_count == g_udc.out_ep[ep_idx].ep_mps)
                        g_usb_irq_stats.out_xferlen_zero++;
                    else
                        g_usb_irq_stats.out_short_pkt++;

                    /* Part A: reset transfer state BEFORE completion handler.
                     * The handler (ncm_bulk_out) may call usbd_ep_start_read()
                     * to re-arm, which sets xfer_buf to the new buffer.
                     * If it stalls, xfer_buf stays NULL → ghost guard works. */
                    g_udc.out_ep[ep_idx].xfer_buf = NULL;
                    g_udc.out_ep[ep_idx].xfer_len = 0;
                    g_udc.out_ep[ep_idx].actual_xfer_len = 0;

                    usbd_event_ep_out_complete_handler(ep_idx, actual);
                } else {
                    g_usb_irq_stats.out_more_data++;
                    drv_usb_ep_set_ready(ep_idx);
                }
            }
        }
    }

    if (intstatus & USB_EP0_SETUP_DONE_INT) {
        if (!drv_usb_ep_wait_ready(0)) {
            g_usb_irq_stats.ep0_busy_timeout++;
            goto ep0_setup_recover;
        }

        /* Retry reading FIFO count with short delay for hardware stabilization */
        int retry = 3;
        do {
            rx_count = drv_usb_get_rxcount(0);
            if (rx_count == 8) {
                break;
            }
            /* Small delay for hardware to stabilize */
            for (volatile int i = 0; i < 100; i++) {}
        } while (--retry > 0);

        if (rx_count != 8) {
            g_usb_irq_stats.setup_fail++;
ep0_setup_recover:
            /* Clear EP0 RX FIFO to recover from error state */
            {
                uint32_t fifo_cfg = getreg32(USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET);
                fifo_cfg |= USB_EP0_RX_FIFO_CLR;
                putreg32(fifo_cfg, USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET);
                fifo_cfg &= ~USB_EP0_RX_FIFO_CLR;
                putreg32(fifo_cfg, USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET);
                /* Set EP0 ready for next setup packet */
                drv_usb_ep_set_ready(0);
            }
            return;
        }
        drv_usb_fifo2mem(0, (uint8_t *)&g_udc.setup, 8);
        usbd_event_ep0_setup_complete_handler((uint8_t *)&g_udc.setup);
    }
    if (intstatus & USB_EP0_IN_DONE_INT) {
        tx_count = MIN(g_udc.in_ep[0].xfer_len, g_udc.in_ep[0].ep_mps);
        g_udc.in_ep[0].xfer_buf += tx_count;
        g_udc.in_ep[0].xfer_len -= tx_count;
        g_udc.in_ep[0].actual_xfer_len += tx_count;

        usbd_event_ep_in_complete_handler(0 | 0x80, g_udc.in_ep[0].actual_xfer_len);

        if (g_udc.setup.wLength == 0) {
            /* In status, start reading setup */
            drv_usb_ep_set_ready(0);
        } else if (g_udc.setup.wLength && ((g_udc.setup.bmRequestType & USB_REQUEST_DIR_MASK) == USB_REQUEST_DIR_OUT)) {
            /* In status, start reading setup */
            drv_usb_ep_set_ready(0);
        }
    }
    if (intstatus & USB_EP0_OUT_DONE_INT) {
        if (!drv_usb_ep_wait_ready(0)) {
            g_usb_irq_stats.ep0_busy_timeout++;
            return;
        }

        rx_count = drv_usb_get_rxcount(0);

        if (g_udc.out_ep[0].xfer_buf) {
            drv_usb_fifo2mem(0, g_udc.out_ep[0].xfer_buf, rx_count);
            g_udc.out_ep[0].xfer_buf += rx_count;
            g_udc.out_ep[0].xfer_len -= rx_count;
            g_udc.out_ep[0].actual_xfer_len += rx_count;
        }

        usbd_event_ep_out_complete_handler(0, g_udc.out_ep[0].actual_xfer_len);

        if (rx_count == 0) {
            /* Out status, start reading setup */
            drv_usb_ep_set_ready(0);
        }
    }

    if (intstatus & USB_RESET_INT) {
        memset(&g_udc, 0, sizeof(g_udc));

        regval = 0;
        regval |= USB_CR_USB_RESET_EN;
        regval |= USB_CR_VBUS_TGL_EN;
        regval |= USB_CR_EP0_SETUP_DONE_EN;
        regval |= USB_CR_EP0_IN_DONE_EN;
        regval |= USB_CR_EP0_OUT_DONE_EN;
        regval |= USB_CR_USB_REND_EN;
        putreg32(regval, USB_BASE + USB_INT_EN_OFFSET);

        regval = 0xffffffff;
        regval &= ~USB_CR_USB_RESET_MASK;
        regval &= ~USB_CR_VBUS_TGL_MASK;
        regval &= ~USB_CR_EP0_SETUP_DONE_MASK;
        regval &= ~USB_CR_EP0_IN_DONE_MASK;
        regval &= ~USB_CR_EP0_OUT_DONE_MASK;
        regval &= ~USB_CR_USB_REND_MASK;
        putreg32(regval, USB_BASE + USB_INT_MASK_OFFSET);

        usbd_event_reset_handler();
    }
    if (intstatus & USB_REND_INT) {
        drv_usb_ep_set_ready(0);
    }
    if (intstatus & USB_VBUS_TGL_INT) {
        uint32_t xcvr = getreg32(USB_BASE + USB_XCVR_IF_CONFIG_OFFSET);
        bool vbus_now = (xcvr & USB_STS_VBUS_DET) ? true : false;
        if (vbus_now != g_vbus_last) {
            g_vbus_last = vbus_now;
            if (vbus_now) {
                usbd_event_connect_handler();
            } else {
                usbd_event_disconnect_handler();
            }
        }
    }
}

/* Live VBUS presence from the transceiver status bit (USB_STS_VBUS_DET).
 * True for ANY powered USB source — enumerated host or dumb wall charger —
 * so apps can use it as a charger-attached signal. Valid once the PHY is
 * initialized (usb_dc_init); a plain register read, safe from task context. */
bool drv_usb_vbus_present(void)
{
    return (getreg32(USB_BASE + USB_XCVR_IF_CONFIG_OFFSET) & USB_STS_VBUS_DET) != 0;
}

/* New APIs for USB endpoint DMA control */
int usbd_ep_enable_dma_tx(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t regval;

    if (ep_idx >= USB_NUM_BIDIR_ENDPOINTS) {
        return -1;
    }

    regval = getreg32(USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx);
    regval |= (1 << 0U); /* USB_EPx_DMA_TX_EN */
    putreg32(regval, USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx);

    return 0;
}

int usbd_ep_disable_dma_tx(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t regval;

    if (ep_idx >= USB_NUM_BIDIR_ENDPOINTS) {
        return -1;
    }

    regval = getreg32(USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx);
    regval &= ~(1 << 0U); /* clear USB_EPx_DMA_TX_EN */
    putreg32(regval, USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx);

    return 0;
}

void usbd_ep_toggle_dma_tx(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t fifo_cfg_addr = USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx;
    uint32_t regval;

    /* Disable DMA TX */
    regval = getreg32(fifo_cfg_addr);
    regval &= ~(1 << 0U);
    putreg32(regval, fifo_cfg_addr);

    /* Re-enable DMA TX */
    regval |= (1 << 0U);
    putreg32(regval, fifo_cfg_addr);
}


int usbd_ep_trigger_ready(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t regval;

    if (ep_idx >= USB_NUM_BIDIR_ENDPOINTS) {
        return -1;
    }

    if (ep_idx == 0) {
        regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
        regval |= USB_CR_USB_EP0_SW_RDY;
        putreg32(regval, USB_BASE + USB_CONFIG_OFFSET);
    } else {
        regval = getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
        regval |= USB_CR_EP1_RDY;
        putreg32(regval, USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
    }

    return 0;
}

int usbd_ep_is_ready(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t regval;

    if (ep_idx >= USB_NUM_BIDIR_ENDPOINTS) {
        return -1;
    }

    if (ep_idx == 0) {
        regval = getreg32(USB_BASE + USB_CONFIG_OFFSET);
        return (regval & USB_STS_USB_EP0_SW_RDY) ? 1 : 0;
    } else {
        regval = getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET + 4 * (ep_idx - 1));
        return (regval & USB_STS_EP1_RDY) ? 1 : 0;
    }
}

uint32_t usbd_ep_get_tx_fifo_addr(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    return (USB_BASE + USB_EP0_TX_FIFO_WDATA_OFFSET + ep_idx * 0x10);
}

uint32_t usbd_ep_get_rx_fifo_addr(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    return (USB_BASE + USB_EP0_RX_FIFO_RDATA_OFFSET + ep_idx * 0x10);
}

int usbd_ep_enable_dma_rx(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t regval;

    if (ep_idx >= USB_NUM_BIDIR_ENDPOINTS)
        return -1;

    regval = getreg32(USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx);
    regval |= (1 << 1U); /* USB_EPx_DMA_RX_EN */
    putreg32(regval, USB_BASE + USB_EP0_FIFO_CONFIG_OFFSET + 0x10 * ep_idx);

    return 0;
}

void usbd_ep_setup_dma_rx(const uint8_t ep, uint8_t dma_ch)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    g_ep_dma_rx[ep_idx].ch = dma_ch;
    g_ep_dma_rx[ep_idx].enabled = 1;
    g_ep_dma_rx[ep_idx].raw = 0;
    g_ep_dma_rx[ep_idx].last_remain = 0;
    usbd_ep_enable_dma_rx(ep);
}

/* Mark an OUT EP's DMA RX as raw bulk: the ISR skips the NCM/NTB framing checks
 * (NTH16 signature / wBlockLength / <12B fragment re-arm) that corrupt a raw byte
 * stream which legitimately contains sub-12-byte packets (e.g. a 4-byte length word). */
void usbd_ep_dma_rx_set_raw(const uint8_t ep)
{
    g_ep_dma_rx[USB_EP_GET_IDX(ep)].raw = 1;
}

void usbd_get_irq_stats(usb_irq_stats_t *stats)
{
    if (stats) *stats = *(const usb_irq_stats_t *)&g_usb_irq_stats;
}

void usbd_reset_irq_stats(void)
{
    memset((void *)&g_usb_irq_stats, 0, sizeof(g_usb_irq_stats));
    g_dma_trace_idx = 0;
}

void usbd_get_ep_out_status(uint8_t ep_idx, usb_ep_out_status_t *status)
{
    if (!status || ep_idx == 0 || ep_idx >= USB_NUM_BIDIR_ENDPOINTS)
        return;

    status->ep_cfg_reg = getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET +
                                  4 * (ep_idx - 1));
    status->fifo_rx_cnt = (getreg32(USB_BASE + USB_EP0_FIFO_STATUS_OFFSET +
                                    0x10 * ep_idx) &
                           USB_EP0_RX_FIFO_CNT_MASK) >>
                          USB_EP0_RX_FIFO_CNT_SHIFT;
    status->ep_enabled = g_udc.out_ep[ep_idx].ep_enable;
    status->ep_stalled = g_udc.out_ep[ep_idx].ep_stalled;
    status->xfer_buf = g_udc.out_ep[ep_idx].xfer_buf;
    status->xfer_len = g_udc.out_ep[ep_idx].xfer_len;
    status->actual_xfer_len = g_udc.out_ep[ep_idx].actual_xfer_len;
}

/* Diagnostic: print the OUT-path counters + the last OUT-ISR trace entries.
 * Used to pinpoint a bulk-OUT (RX) wedge — which branch the DMA ISR last took,
 * and whether out_entry / ep_done stopped advancing (ISR not firing) vs a bad
 * branch. Call from task context (uses printf). */
void drv_usb_dump_out_trace(void)
{
    uint32_t n = g_dma_trace_idx;
    uint32_t start = (n > 24) ? (n - 24) : 0;
    printf("[DCD] out_entry=%lu rxz=%lu short=%lu more=%lu xlz=%lu ep2done=%lu tn=%lu\r\n",
           (unsigned long)g_usb_irq_stats.out_entry,
           (unsigned long)g_usb_irq_stats.out_rxcount_zero,
           (unsigned long)g_usb_irq_stats.out_short_pkt,
           (unsigned long)g_usb_irq_stats.out_more_data,
           (unsigned long)g_usb_irq_stats.out_xferlen_zero,
           (unsigned long)g_usb_irq_stats.ep_done[1],
           (unsigned long)n);
    for (uint32_t k = start; k < n; k++) {
        uint32_t ti = k & (DMA_TRACE_SIZE - 1);
        printf("  #%lu br=%u pkt=%lu rem=%lu lr=%lu rxc=%u fto=%u xl=%lu\r\n",
               (unsigned long)k, g_dma_trace[ti].branch,
               (unsigned long)g_dma_trace[ti].pkt_bytes,
               (unsigned long)g_dma_trace[ti].remain,
               (unsigned long)g_dma_trace[ti].last_remain,
               g_dma_trace[ti].rx_cnt, g_dma_trace[ti].fifo_to,
               (unsigned long)g_dma_trace[ti].xfer_len);
    }
}

void usbd_get_ep_in_status(uint8_t ep_idx, usb_ep_in_status_t *status)
{
    if (!status || ep_idx == 0 || ep_idx >= USB_NUM_BIDIR_ENDPOINTS)
        return;

    status->ep_cfg_reg = getreg32(USB_BASE + USB_EP1_CONFIG_OFFSET +
                                  4 * (ep_idx - 1));
    status->fifo_tx_cnt = (getreg32(USB_BASE + USB_EP0_FIFO_STATUS_OFFSET +
                                    0x10 * ep_idx) &
                           USB_EP0_TX_FIFO_CNT_MASK) >>
                          USB_EP0_TX_FIFO_CNT_SHIFT;
    status->ep_enabled = g_udc.in_ep[ep_idx].ep_enable;
    status->ep_stalled = g_udc.in_ep[ep_idx].ep_stalled;
    status->xfer_buf = g_udc.in_ep[ep_idx].xfer_buf;
    status->xfer_len = g_udc.in_ep[ep_idx].xfer_len;
    status->actual_xfer_len = g_udc.in_ep[ep_idx].actual_xfer_len;
}

