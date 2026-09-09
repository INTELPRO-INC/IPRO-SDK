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

/*
 * IPRO6 USB 2.0 (USB v2 IP) CherryUSB device-controller (DCD).
 *
 * Implements the CherryUSB usb_dc.h device interface on top of the IPRO6
 * USB v2 register layer (drv_usb.c / USB_*() API). The controller moves data
 * through a virtual-DMA (vDMA) engine per FIFO and reports events through a
 * five-group interrupt structure.
 *
 * Clock and PHY bring-up happens in usb_dc_init():
 *   - GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU)  enable the USB v2 AHB clock
 *   - GLB_Set_USB_CLK_From_WIFIPLL(1)                route the USB clock from WIFIPLL
 *   - PDS_Turn_On_USB(1)                             PHY power-on + reset sequence
 * The WIFIPLL must already be running because it is the USB clock source.
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "usbd_core.h"
#include "ipro6.h"
#include "drv_common.h"
#include "drv_usb.h"
#include "drv_glb.h"
#include "drv_pds.h"

/* Local FIFO identifiers: F0..F3 are the data FIFOs, CXF is the control FIFO. */
#define USB_FIFO_F0  0
#define USB_FIFO_F1  1
#define USB_FIFO_F2  2
#define USB_FIFO_F3  3
#define USB_FIFO_CXF 0xff

/* FIFO/endpoint map value meaning "no endpoint assigned". */
#define USB_FIFO_UNMAPPED 15

#define USB_NUM_BIDIR_ENDPOINTS 5

/* Per-endpoint software state. */
struct ipro6_ep_state {
    uint16_t ep_mps;    /* max packet size */
    uint8_t  ep_type;   /* transfer type */
    uint8_t  ep_stalled;
    uint8_t  ep_enable;
    bool     ep_active;
    uint8_t *xfer_buf;
    uint32_t xfer_len;
    uint32_t actual_xfer_len;
};

struct ipro6_udc {
    struct ipro6_ep_state in_ep[USB_NUM_BIDIR_ENDPOINTS];
    struct ipro6_ep_state out_ep[USB_NUM_BIDIR_ENDPOINTS];
} g_ipro6_udc;

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_setup_buffer[8];

void USBD_IRQHandler(void);

/* Translate a local FIFO id into a vDMA target FIFO selector. */
static USB_VDMA_TARGET_FIFO_Type ipro6_usb_vdma_target(uint8_t fifo)
{
    if (fifo == USB_FIFO_CXF) {
        return USB_VDMA_TARGET_FIFO_CTRL;
    }

    return (USB_VDMA_TARGET_FIFO_Type)(USB_VDMA_TARGET_FIFO_0 + fifo);
}

static void ipro6_usb_reset_fifo(uint8_t fifo)
{
    if (fifo == USB_FIFO_CXF) {
        USB_Clear_CTRL_FIFO();
    } else {
        USB_Reset_FIFO((USB_FIFO_Type)fifo);
    }
}

static void ipro6_usb_vdma_start_write(uint8_t fifo, const uint8_t *data, uint32_t len)
{
    USB_VDMA_TARGET_FIFO_Type target = ipro6_usb_vdma_target(fifo);
    USB_VDMA_Cfg_Type cfg = {
        .dir     = USB_VDMA_TRANS_DIR_MEM_2_FIFO,
        .memAddr = (uint32_t)data,
        .length  = len,
    };

    USB_Set_VDMA_Config(target, &cfg);
    USB_Set_VDMA_Start(target);
}

static void ipro6_usb_vdma_start_read(uint8_t fifo, uint8_t *data, uint32_t len)
{
    USB_VDMA_TARGET_FIFO_Type target = ipro6_usb_vdma_target(fifo);
    USB_VDMA_Cfg_Type cfg = {
        .dir     = USB_VDMA_TRANS_DIR_FIFO_2_MEM,
        .memAddr = (uint32_t)data,
        .length  = len,
    };

    USB_Set_VDMA_Config(target, &cfg);
    USB_Set_VDMA_Start(target);
}

/* Pending status of a source group, with masked bits removed. */
static uint32_t ipro6_usb_get_source_group_intstatus(uint8_t group)
{
    switch (group) {
        case 0:
            return USB_Get_Sub_Group_0_IntStatus() & ~USB_Get_Sub_Group_0_IntMask();
        case 1:
            return USB_Get_Sub_Group_1_IntStatus() & ~USB_Get_Sub_Group_1_IntMask();
        case 2:
            return USB_Get_Sub_Group_2_IntStatus() & ~USB_Get_Sub_Group_2_IntMask();
        case 3:
            return USB_Get_Sub_Group_3_IntStatus() & ~USB_Get_Sub_Group_3_IntMask();
        case 4:
            return USB_Get_Sub_Group_4_IntStatus() & ~USB_Get_Sub_Group_4_IntMask();
        default:
            return 0;
    }
}

static void ipro6_usb_source_group_int_clear(uint8_t group, uint32_t int_clear)
{
    switch (group) {
        case 0:
            USB_Get_Sub_Group_0_IntClear(int_clear);
            break;
        case 1:
            USB_Get_Sub_Group_1_IntClear(int_clear);
            break;
        case 2:
            USB_Get_Sub_Group_2_IntClear(int_clear);
            break;
        case 3:
            USB_Get_Sub_Group_3_IntClear(int_clear);
            break;
        case 4:
            USB_Get_Sub_Group_4_IntClear(int_clear);
            break;
        default:
            break;
    }
}

/* Select which data FIFO carries a given endpoint's transfers. Endpoints that
 * span a FIFO pair -- a >512 merged packet, or a bulk double-block pipeline --
 * use the pair's base FIFO for the vDMA: ep1 -> F0, ep2 -> F2. */
static uint8_t ipro6_usb_get_transfer_fifo(uint8_t ep_idx)
{
    bool dual = (g_ipro6_udc.out_ep[ep_idx].ep_mps > 512) ||
                (g_ipro6_udc.in_ep[ep_idx].ep_mps > 512) ||
                (g_ipro6_udc.in_ep[ep_idx].ep_type == USB_FIFO_TRANSFER_BULK) ||
                (g_ipro6_udc.out_ep[ep_idx].ep_type == USB_FIFO_TRANSFER_BULK);

    if (dual && (ep_idx == 1 || ep_idx == 2)) {
        return (ep_idx == 1) ? USB_FIFO_F0 : USB_FIFO_F2;
    }

    return (uint8_t)(ep_idx - 1);
}

int usb_dc_init(void)
{
    USB_NORMAL_Cfg_Type normal_cfg = {
        .forceFullSpeed = 0,
        .chipEn         = 1,
        .swRst          = 1,
        .remoteWakeupEn = 0,
        .rstTimeOut     = 0x7fffffff,
    };
    uint32_t tmpVal;

    /* Release the PDS power-down gate on the USB PLL before any clock work.
     *
     * A PDS31 wake cold-boots, but PDS_CTL is NOT restored: it still holds what
     * PDS_Enable() wrote from pdsCfgLevel15 on the way into the sleep, so
     * CTRL_USBPLL_PD (pdsCfgLevel15.usbpllOff) stays asserted long after the
     * sleep ended. It is a LIVE gate sitting above CCI_WIFI_PLL_CFG10, so
     * GLB_Set_USB_CLK_From_WIFIPLL() below cannot override it -- the mmdiv reads
     * back byte-identical to a working POR boot (PU_MMDIV=1, RSTB=1,
     * SDMIN=0x28000) and still produces no clock. The USB slave then has no
     * clock to answer on and the first USB_BASE access stalls the AHB forever:
     * the core freezes mid-transaction and even the debug module wedges
     * (abstractcs busy stuck high), which is what made this look like a hang
     * with no stack to inspect.
     *
     * Costs nothing in power: the PLL is still powered down for the whole
     * sleep, we merely stop holding it down once we are awake and want USB.
     * Measured 2026-08-17, alternating A/B on a PDS31 wake: gate cleared 2/2
     * reach the register, gate left set 2/2 stall. */
    tmpVal = readl(PDS_BASE + PDS_CTL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_CTRL_USBPLL_PD);
    writel(tmpVal, PDS_BASE + PDS_CTL_OFFSET);

    /* USB v2 clock + PHY bring-up (WIFIPLL must already be running). */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU);
    GLB_Set_USB_CLK_From_WIFIPLL(1);
    PDS_Turn_On_USB(1);

    Interrupt_Handler_Register(USB_IRQn, USBD_IRQHandler);
    CPU_Interrupt_Enable(USB_IRQn);

    /* Keep the device disconnected and quiet while configuring it. */
    USB_Global_IntEnable(0);
    USB_SoftDetach_Enable();

    /* Chip enable + software reset (USB_Set_Normal_Config waits for self-clear). */
    USB_Set_Normal_Config(&normal_cfg);

    /* No non-control transfers until an interface is configured. */
    USB_Non_Ctrl_Transfer_Disable();

#ifdef CONFIG_USB_HS
    USB_SOF_Mask_Time_HighSpeed();
#else
    USB_SOF_Mask_Time_FullSpeed();
#endif

    /*
     * Mask every source interrupt first, then unmask only the ones the ISR
     * services. Writing the all-masked baseline (instead of relying on the
     * MISG reset value) keeps the enable set well defined across re-init.
     */
    writel(0xffffffff, USB_BASE + USB_DEV_MISG0_OFFSET);
    writel(0xffffffff, USB_BASE + USB_DEV_MISG1_OFFSET);
    writel(0xffffffff, USB_BASE + USB_DEV_MISG2_OFFSET);
    writel(0xffffffff, USB_BASE + USB_DEV_MISG3_OFFSET);
    writel(0xffffffff, USB_BASE + USB_DEV_MISG4_OFFSET);

    /* Group 0: only the control-endpoint SETUP interrupt. */
    USB_Sub_Group_IntUnmask(USB_INT_CX_SETUP);

    /* Group 1: per-FIFO interrupts stay masked (completion comes via vDMA group 3). */

    /* Group 2: bus reset / suspend / resume / zero-length-packet. */
    USB_Sub_Group_IntUnmask(USB_INT_RESET | USB_INT_SUSPEND | USB_INT_RESUME |
                            USB_INT_TX0BYTE | USB_INT_RX0BYTE);

    /* Group 3: vDMA completion only (vDMA errors stay masked). */
    USB_Sub_Group_IntUnmask(USB_INT_VDMA_CMPLT_CXF | USB_INT_VDMA_CMPLT_F0 | USB_INT_VDMA_CMPLT_F1 |
                            USB_INT_VDMA_CMPLT_F2 | USB_INT_VDMA_CMPLT_F3);

    /* Enable all interrupt groups. */
    USB_Group_IntMask(USB_GRP_INT_G0, UNMASK);
    USB_Group_IntMask(USB_GRP_INT_G1, UNMASK);
    USB_Group_IntMask(USB_GRP_INT_G2, UNMASK);
    USB_Group_IntMask(USB_GRP_INT_G3, UNMASK);
    USB_Group_IntMask(USB_GRP_INT_G4, UNMASK);

    /* Device mode: enable the device interrupt, mask host and OTG. */
    USB_Global_IntMask(USB_GLOBAL_INT_HC, MASK);
    USB_Global_IntMask(USB_GLOBAL_INT_OTG, MASK);
    USB_Global_IntMask(USB_GLOBAL_INT_DEV, UNMASK);

    /* Clear any pending group 2 / group 3 status. */
    USB_Get_Sub_Group_2_IntClear(0x3ff);
    USB_Get_Sub_Group_3_IntClear(0xffffffff);

    /* Unmap every endpoint until usbd_ep_open() assigns a FIFO. */
    for (uint8_t i = 1; i < 9; i++) {
        USB_Set_FIFO_Of_Endpoint_IN((USB_Endpoint_Type)i, (USB_FIFO_Type)USB_FIFO_UNMAPPED);
        USB_Set_FIFO_Of_Endpoint_OUT((USB_Endpoint_Type)i, (USB_FIFO_Type)USB_FIFO_UNMAPPED);
    }

    USB_Set_VDMA_Enable();

    /* Connect to the host and arm interrupts. */
    USB_SoftDetach_Disable();
    USB_Global_IntEnable(1);

    return 0;
}

int usb_dc_deinit(void)
{
    USB_Global_IntEnable(0);
    USB_SoftDetach_Enable();
    PDS_Turn_Off_USB();

    return 0;
}

int usbd_set_address(const uint8_t addr)
{
    USB_Set_Device_Address(addr);

    return 0;
}

uint8_t usbd_get_port_speed(const uint8_t port)
{
    uint8_t speed;

    (void)port;

    /* OTG_CSR.SPD_TYP: 0 = full, 1 = low, 2 = high. */
    speed = reg_get_bits(readl(USB_BASE + USB_OTG_CSR_OFFSET), USB_SPD_TYP_HOV_POV);

    switch (speed) {
        case 0:
            return USB_SPEED_FULL;
        case 1:
            return USB_SPEED_LOW;
        case 2:
            return USB_SPEED_HIGH;
        default:
            return USB_SPEED_HIGH;
    }
}

int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg)
{
    uint8_t ep = ep_cfg->ep_addr;
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if ((ep_idx > 4) && (ep_idx < 9)) {
        return 0;
    }

    if (USB_EP_DIR_IS_OUT(ep)) {
        g_ipro6_udc.out_ep[ep_idx].ep_mps = ep_cfg->ep_mps;
        g_ipro6_udc.out_ep[ep_idx].ep_type = ep_cfg->ep_type;
        g_ipro6_udc.out_ep[ep_idx].ep_enable = 1U;
        USB_Set_Endpoint_OUT_MaxPacketSize((USB_Endpoint_Type)ep_idx, ep_cfg->ep_mps);
    } else {
        g_ipro6_udc.in_ep[ep_idx].ep_mps = ep_cfg->ep_mps;
        g_ipro6_udc.in_ep[ep_idx].ep_type = ep_cfg->ep_type;
        g_ipro6_udc.in_ep[ep_idx].ep_enable = 1U;
        USB_Set_Endpoint_IN_MaxPacketSize((USB_Endpoint_Type)ep_idx, ep_cfg->ep_mps);
    }

    if (ep_idx != 0) {
        USB_FIFO_Cfg_Type fifo_cfg = {
            .enable    = 1,
            .dir       = USB_FIFO_DIR_BID,
            .epID      = (USB_Endpoint_Type)ep_idx,
            .blockType = USB_FIFO_BLOCK_CNT_SINGLE_BLOCK,
            .fifoType  = (USB_FIFO_TRANSFER_Type)ep_cfg->ep_type,
        };

        if (ep_cfg->ep_mps > 512) {
            /* >512 packet (isoc/HS-interrupt): merge an adjacent 512-byte block
             * into a 1024-byte one -- ep1 -> F0(+F1), ep2 -> F2(+F3). */
            USB_FIFO_Type primary;
            USB_FIFO_Type secondary;

            if (ep_idx == 1) {
                primary   = USB_FIFO_0;
                secondary = USB_FIFO_1;
            } else if (ep_idx == 2) {
                primary   = USB_FIFO_2;
                secondary = USB_FIFO_3;
            } else {
                return -1;
            }

            fifo_cfg.blockSize = USB_FIFO_BLOCK_MAX_SIZE_1024;

            /* Isoc FIFO block count is coupled to the descriptor's transactions
             * per microframe (high-bandwidth mult): blockType must equal mult
             * (0/1/2 -> SINGLE/DOUBLE/TRIPLE). Setting it higher than mult+1
             * desyncs the HW and the device streams zero-length packets. So a
             * deeper isoc FIFO comes only with a matching high-bandwidth
             * descriptor (TP_ISOC_MULT). Bulk/Tier-1 stay SINGLE. */
            fifo_cfg.blockType = (USB_FIFO_BLOCK_CNT_Type)ep_cfg->ep_mult;

            USB_Set_FIFO_Of_Endpoint_IN((USB_Endpoint_Type)ep_idx, primary);
            USB_Set_FIFO_Of_Endpoint_OUT((USB_Endpoint_Type)ep_idx, primary);

            fifo_cfg.enable = 1;
            USB_Set_FIFO_Config(primary, &fifo_cfg);
            fifo_cfg.enable = 0;
            USB_Set_FIFO_Config(secondary, &fifo_cfg);
        } else if (ep_cfg->ep_type == USB_FIFO_TRANSFER_BULK &&
                   (ep_idx == 1 || ep_idx == 2)) {
            /* Bulk: a non-overlapping DOUBLE block (2-stage pipeline) pair so IN
             * (ep1) and OUT (ep2) can run simultaneously within the 4-block FIFO
             * SRAM -- ep1 -> F0(+F1), ep2 -> F2(+F3). A single 512-byte block
             * underruns on IN (the FIFO empties before the vDMA refills and the
             * device NAKs the next IN token) and back-pressures OUT, capping
             * throughput. A triple block would buffer one more packet but needs
             * 3 of the 4 blocks per endpoint, so the two endpoints' blocks would
             * overlap and corrupt simultaneous bidirectional transfers. The base
             * FIFO is enabled and the concatenated one disabled, both carrying
             * the same block count (the IP's FIFO-concatenation rule). */
            USB_FIFO_Type primary   = (ep_idx == 1) ? USB_FIFO_0 : USB_FIFO_2;
            USB_FIFO_Type secondary = (ep_idx == 1) ? USB_FIFO_1 : USB_FIFO_3;

            fifo_cfg.blockSize = USB_FIFO_BLOCK_MAX_SIZE_512;
            fifo_cfg.blockType = USB_FIFO_BLOCK_CNT_DOUBLE_BLOCKS;

            USB_Set_FIFO_Of_Endpoint_IN((USB_Endpoint_Type)ep_idx, primary);
            USB_Set_FIFO_Of_Endpoint_OUT((USB_Endpoint_Type)ep_idx, primary);

            fifo_cfg.enable = 1;
            USB_Set_FIFO_Config(primary, &fifo_cfg);
            fifo_cfg.enable = 0;
            USB_Set_FIFO_Config(secondary, &fifo_cfg);
        } else {
            /* One 512-byte FIFO per endpoint: ep_n -> F(n-1). */
            USB_FIFO_Type fifo = (USB_FIFO_Type)(ep_idx - 1);

            fifo_cfg.blockSize = USB_FIFO_BLOCK_MAX_SIZE_512;

            USB_Set_FIFO_Of_Endpoint_IN((USB_Endpoint_Type)ep_idx, fifo);
            USB_Set_FIFO_Of_Endpoint_OUT((USB_Endpoint_Type)ep_idx, fifo);
            USB_Set_FIFO_Config(fifo, &fifo_cfg);
        }

        USB_Non_Ctrl_Transfer_Enable();
    }

    return 0;
}

int usbd_ep_close(const uint8_t ep)
{
    (void)ep;

    return 0;
}

int usbd_ep_set_stall(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (ep_idx == 0) {
        USB_Set_CTRL_Endpoint_Stall_Once();
    } else if (USB_EP_DIR_IS_OUT(ep)) {
        USB_Set_Endpoint_OUT_Stall((USB_Endpoint_Type)ep_idx, 1);
    } else {
        USB_Set_Endpoint_IN_Stall((USB_Endpoint_Type)ep_idx, 1);
    }

    return 0;
}

int usbd_ep_clear_stall(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (ep_idx == 0) {
        return 0;
    }

    if (USB_EP_DIR_IS_OUT(ep)) {
        USB_Set_Endpoint_OUT_Stall((USB_Endpoint_Type)ep_idx, 0);
    } else {
        USB_Set_Endpoint_IN_Stall((USB_Endpoint_Type)ep_idx, 0);
    }

    return 0;
}

int usbd_ep_is_stalled(const uint8_t ep, uint8_t *stalled)
{
    (void)ep;
    (void)stalled;

    return 0;
}

int usbd_ep_start_write(const uint8_t ep, const uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!data && data_len) {
        return -1;
    }

    if (!g_ipro6_udc.in_ep[ep_idx].ep_enable) {
        return -2;
    }

    g_ipro6_udc.in_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_ipro6_udc.in_ep[ep_idx].xfer_len = data_len;
    g_ipro6_udc.in_ep[ep_idx].actual_xfer_len = 0;
    g_ipro6_udc.in_ep[ep_idx].ep_active = true;

    if (ep_idx == 0) {
        if (data_len == 0) {
            g_ipro6_udc.in_ep[ep_idx].ep_active = false;
            USB_CTRL_Endpoint_Data_Transfer_Done();
        } else {
            data_len = MIN(data_len, g_ipro6_udc.in_ep[ep_idx].ep_mps);
            g_ipro6_udc.in_ep[ep_idx].xfer_len = data_len;
            ipro6_usb_vdma_start_write(USB_FIFO_CXF, data, data_len);
        }
    } else {
        if (data_len == 0) {
            USB_Endpoint_Transmit_Zero_Length_Packet((USB_Endpoint_Type)ep_idx);
        } else {
            ipro6_usb_vdma_start_write(ipro6_usb_get_transfer_fifo(ep_idx), data, data_len);
        }
    }

    return 0;
}

int usbd_ep_start_read(const uint8_t ep, uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!data && data_len) {
        return -1;
    }

    if (!g_ipro6_udc.out_ep[ep_idx].ep_enable) {
        return -2;
    }

    if (data_len == 0) {
        return 0;
    }

    g_ipro6_udc.out_ep[ep_idx].xfer_buf = data;
    g_ipro6_udc.out_ep[ep_idx].xfer_len = data_len;
    g_ipro6_udc.out_ep[ep_idx].actual_xfer_len = 0;
    g_ipro6_udc.out_ep[ep_idx].ep_active = true;

    if (ep_idx == 0) {
        data_len = MIN(data_len, g_ipro6_udc.out_ep[ep_idx].ep_mps);
        g_ipro6_udc.out_ep[ep_idx].xfer_len = data_len;
        ipro6_usb_vdma_start_read(USB_FIFO_CXF, data, data_len);
    } else {
        ipro6_usb_vdma_start_read(ipro6_usb_get_transfer_fifo(ep_idx), data, data_len);
    }

    return 0;
}

__WEAK void usbd_event_suspend_callback(void)
{
}

void USBD_IRQHandler(void)
{
    uint32_t subgroup;
    uint8_t  ep_idx;

    if (!USB_Global_IntStatus(USB_GLOBAL_INT_DEV)) {
        return;
    }

    /* Group 0: control-endpoint SETUP packet. */
    if (USB_Group_IntStatus(USB_GRP_INT_G0)) {
        subgroup = ipro6_usb_get_source_group_intstatus(0);

        if (subgroup & USB_SUB_GROUP_0_CX_SETUP_BIT_MUSK) {
            ipro6_usb_vdma_start_read(USB_FIFO_CXF, g_setup_buffer, 8);
            while (USB_Get_VDMA_Start(USB_VDMA_TARGET_FIFO_CTRL)) {
            }

            /* The setup read raised a control-FIFO vDMA completion; drop it. */
            ipro6_usb_source_group_int_clear(3, USB_SUB_GROUP_3_VDMA_CMPLT_CXF_BIT_MUSK);

            usbd_event_ep0_setup_complete_handler(g_setup_buffer);
        }
    }

    /* Group 2: bus status and zero-length-packet completions. */
    if (USB_Group_IntStatus(USB_GRP_INT_G2)) {
        subgroup = ipro6_usb_get_source_group_intstatus(2);

        if (subgroup & USB_SUB_GROUP_2_SUSPEND_BIT_MUSK) {
            ipro6_usb_source_group_int_clear(2, USB_SUB_GROUP_2_SUSPEND_BIT_MUSK);

            ipro6_usb_reset_fifo(USB_FIFO_F0);
            ipro6_usb_reset_fifo(USB_FIFO_F1);
            ipro6_usb_reset_fifo(USB_FIFO_F2);
            ipro6_usb_reset_fifo(USB_FIFO_F3);
            ipro6_usb_reset_fifo(USB_FIFO_CXF);

            usbd_event_suspend_callback();
        }

        if (subgroup & USB_SUB_GROUP_2_RESUME_BIT_MUSK) {
            ipro6_usb_source_group_int_clear(2, USB_SUB_GROUP_2_RESUME_BIT_MUSK);
        }

        if (subgroup & USB_SUB_GROUP_2_TX0BYTE_BIT_MUSK) {
            for (uint8_t i = 1; i < 5; i++) {
                if (USB_Is_Endpoint_Transfer_Zero_Length_Packet((USB_Endpoint_Type)i)) {
                    USB_Clear_Endpoint_Transfer_Zero_Length_Packet_Status((USB_Endpoint_Type)i);
                    usbd_event_ep_in_complete_handler(i | 0x80, 0);
                }
            }
            ipro6_usb_source_group_int_clear(2, USB_SUB_GROUP_2_TX0BYTE_BIT_MUSK);
        }

        if (subgroup & USB_SUB_GROUP_2_RX0BYTE_BIT_MUSK) {
            for (uint8_t i = 1; i < 5; i++) {
                if (USB_Is_Endpoint_Receive_Zero_Length_Packet((USB_Endpoint_Type)i)) {
                    USB_Clear_Endpoint_Receive_Zero_Length_Packet_Status((USB_Endpoint_Type)i);
                    usbd_event_ep_out_complete_handler(i, 0);
                }
            }
            ipro6_usb_source_group_int_clear(2, USB_SUB_GROUP_2_RX0BYTE_BIT_MUSK);
        }

        if (subgroup & USB_SUB_GROUP_2_RESET_BIT_MUSK) {
            ipro6_usb_source_group_int_clear(2, USB_SUB_GROUP_2_RESET_BIT_MUSK);

            ipro6_usb_reset_fifo(USB_FIFO_F0);
            ipro6_usb_reset_fifo(USB_FIFO_F1);
            ipro6_usb_reset_fifo(USB_FIFO_F2);
            ipro6_usb_reset_fifo(USB_FIFO_F3);
            ipro6_usb_reset_fifo(USB_FIFO_CXF);

#ifdef CONFIG_USB_HS
            USB_SOF_Mask_Time_HighSpeed();
#else
            USB_SOF_Mask_Time_FullSpeed();
#endif

            memset(&g_ipro6_udc, 0, sizeof(g_ipro6_udc));

            usbd_event_reset_handler();
        }
    }

    /* Group 3: vDMA transfer completions. */
    if (USB_Group_IntStatus(USB_GRP_INT_G3)) {
        subgroup = ipro6_usb_get_source_group_intstatus(3);
        ipro6_usb_source_group_int_clear(3, subgroup);

        if (subgroup & USB_SUB_GROUP_3_VDMA_CMPLT_CXF_BIT_MUSK) {
            if (g_ipro6_udc.in_ep[0].ep_active) {
                g_ipro6_udc.in_ep[0].ep_active = false;
                g_ipro6_udc.in_ep[0].actual_xfer_len =
                    g_ipro6_udc.in_ep[0].xfer_len - USB_Get_VDMA_Remaining(USB_VDMA_TARGET_FIFO_CTRL);
                if (g_ipro6_udc.in_ep[0].actual_xfer_len < g_ipro6_udc.in_ep[0].ep_mps) {
                    USB_CTRL_Endpoint_Data_Transfer_Done();
                }
                usbd_event_ep_in_complete_handler(0x80, g_ipro6_udc.in_ep[0].actual_xfer_len);
            } else {
                g_ipro6_udc.out_ep[0].ep_active = false;
                g_ipro6_udc.out_ep[0].actual_xfer_len =
                    g_ipro6_udc.out_ep[0].xfer_len - USB_Get_VDMA_Remaining(USB_VDMA_TARGET_FIFO_CTRL);
                usbd_event_ep_out_complete_handler(0x00, g_ipro6_udc.out_ep[0].actual_xfer_len);
            }
        }

        for (uint8_t i = 0; i < 4; i++) {
            /* Data-FIFO completion bits are F0..F3 at positions 1..4. */
            if (subgroup & (1 << (i + 1))) {
                ep_idx = USB_Get_FIFO_Endpoint((USB_FIFO_Type)i);
                if (ep_idx >= USB_NUM_BIDIR_ENDPOINTS) {
                    continue;
                }

                if (g_ipro6_udc.in_ep[ep_idx].ep_active) {
                    g_ipro6_udc.in_ep[ep_idx].ep_active = 0;
                    g_ipro6_udc.in_ep[ep_idx].actual_xfer_len =
                        g_ipro6_udc.in_ep[ep_idx].xfer_len - USB_Get_VDMA_Remaining(ipro6_usb_vdma_target(i));
                    usbd_event_ep_in_complete_handler(ep_idx | 0x80, g_ipro6_udc.in_ep[ep_idx].actual_xfer_len);
                } else if (g_ipro6_udc.out_ep[ep_idx].ep_active) {
                    g_ipro6_udc.out_ep[ep_idx].ep_active = 0;
                    g_ipro6_udc.out_ep[ep_idx].actual_xfer_len =
                        g_ipro6_udc.out_ep[ep_idx].xfer_len - USB_Get_VDMA_Remaining(ipro6_usb_vdma_target(i));
                    usbd_event_ep_out_complete_handler(ep_idx & 0x7f, g_ipro6_udc.out_ep[ep_idx].actual_xfer_len);
                }
            }
        }
    }
}
