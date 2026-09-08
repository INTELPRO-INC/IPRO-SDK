/*
 * Copyright (c) 2025, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_cdc_ncm.h"
#include <FreeRTOS.h>
#include <task.h>

/* Cache operations for DMA coherency on cacheable PSRAM buffers.
 * Init:      clean+invalidate  (flush .bss dirty lines, evict from cache)
 * RX (DMA→CPU): invalidate     (discard stale cache, read fresh DMA data)
 * TX (CPU→DMA): clean          (write CPU data to PSRAM for DMA to read) */
#if defined(USB_DCACHE_FLUSH_INVAL)
#define NCM_DCACHE_FLUSH_INVAL(addr, size) USB_DCACHE_FLUSH_INVAL(addr, size)
#else
#define NCM_DCACHE_FLUSH_INVAL(addr, size) ((void)0)
#endif

#if defined(USB_DCACHE_INVAL)
#define NCM_DCACHE_INVAL(addr, size) USB_DCACHE_INVAL(addr, size)
#else
#define NCM_DCACHE_INVAL(addr, size) ((void)0)
#endif

#if defined(USB_DCACHE_FLUSH)
#define NCM_DCACHE_FLUSH(addr, size) USB_DCACHE_FLUSH(addr, size)
#else
#define NCM_DCACHE_FLUSH(addr, size) ((void)0)
#endif

#define CDC_NCM_OUT_EP_IDX 0
#define CDC_NCM_IN_EP_IDX  1
#define CDC_NCM_INT_EP_IDX 2

static struct usbd_endpoint cdc_ncm_ep_data[3];

#ifdef CONFIG_USB_HS
#define CDC_NCM_MAX_PACKET_SIZE 512
#else
#define CDC_NCM_MAX_PACKET_SIZE 64
#endif

/* NTH16 signature "NCMH" */
#define CDC_NCM_NTH16_SIGNATURE 0x484D434E
/* NDP16 signature "NCM0" (no CRC) */
#define CDC_NCM_NDP16_SIGNATURE 0x304D434E

#ifndef CONFIG_CDC_NCM_MAX_DATAGRAMS
#define CONFIG_CDC_NCM_MAX_DATAGRAMS  6
#endif

/* NTB parameters returned for GET_NTB_PARAMETERS */
static const struct cdc_ncm_ntb_parameters g_ntb_parameters = {
    .wLength                = sizeof(struct cdc_ncm_ntb_parameters),
    .bmNtbFormatsSupported  = 0x0001, /* NTH16 only */
    .dwNtbInMaxSize         = CONFIG_CDC_NCM_NTB_MAX_SIZE,
    .wNdbInDivisor          = 4,
    .wNdbInPayloadRemainder = 0,
    .wNdbInAlignment        = 4,
    .wReserved              = 0,
    .dwNtbOutMaxSize        = CONFIG_CDC_NCM_NTB_MAX_SIZE,
    .wNdbOutDivisor         = 4,
    .wNdbOutPayloadRemainder = 0,
    .wNdbOutAlignment       = 4,
    .wNtbOutMaxDatagrams    = CONFIG_CDC_NCM_MAX_DATAGRAMS,
};

static __attribute__((aligned(32)))
    uint8_t g_ncm_rx_buffers[2][CONFIG_CDC_NCM_NTB_MAX_SIZE];
static volatile uint8_t g_ncm_rx_fill_idx;  /* ISR fills here (0 or 1) */
/* TX buffers stay in PSRAM — TX path is not latency-sensitive
 * because the DUT controls the send rate. */
static __attribute__((aligned(32))) uint8_t g_ncm_tx_buffers[2][CONFIG_CDC_NCM_NTB_MAX_SIZE];
static volatile uint8_t g_ncm_tx_fill_idx;  /* buffer being filled (0 or 1) */
/* Notify buffer is small — keep in OCRAM noncacheable for simplicity */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_ncm_notify_buf[16];

/* RX per-slot metadata (ISR-owned: sets count/ndp_offset on receive) */
struct ncm_rx_slot {
    volatile uint16_t dgram_count;  /* >0 = pending data, 0 = free */
    uint16_t ndp_offset;
    uint16_t base_offset;  /* NTB start within buffer (>0 after resync) */
};
static struct ncm_rx_slot g_ncm_rx_slots[2];

/* RX task-owned cursor (never touched by ISR) */
static uint8_t g_ncm_rx_task_idx;     /* which slot task is reading */
static uint16_t g_ncm_rx_task_dgram;  /* current datagram index in that slot */
/* Backpressure: ISR sets when fill slot is occupied; task clears + re-arms */
static volatile uint8_t g_ncm_rx_ep_stalled;

/* TX: busy flag (non-zero while USB hardware transmits) */
static volatile uint32_t g_ncm_tx_data_length = 0;
static volatile TickType_t g_ncm_tx_start_tick = 0;

/* TX stale recovery diagnostics */
volatile uint32_t ncm_diag_tx_stale_recover = 0;
/* TX ZLP and completion diagnostics */
volatile uint32_t ncm_diag_tx_zlp = 0;       /* ZLP sent */
volatile uint32_t ncm_diag_tx_last_nbytes = 0; /* last bulk_in nbytes */
volatile uint32_t ncm_diag_tx_last_len = 0;    /* last flush block_len */

/* TX multi-datagram aggregation */
struct ncm_tx_dgram_entry {
    uint16_t offset;
    uint16_t length;
};
static struct ncm_tx_dgram_entry g_ncm_tx_dgram_table[CONFIG_CDC_NCM_MAX_DATAGRAMS];
static uint16_t g_ncm_tx_dgram_count;
static uint16_t g_ncm_tx_next_offset;  /* next write position in TX buffer */

/* NTB input size (host-configurable) */
static uint32_t g_ntb_input_size = CONFIG_CDC_NCM_NTB_MAX_SIZE;
/* NTB format: 0 = NTH16, 1 = NTH32 (we only support NTH16) */
static uint16_t g_ntb_format = 0;
/* TX sequence number */
static uint16_t g_tx_sequence = 0;

static volatile uint8_t g_current_net_status = 0;
static volatile uint8_t g_cmd_intf = 0;

static uint32_t g_connect_speed_table[2] = { CDC_ECM_CONNECT_SPEED_UPSTREAM,
                                             CDC_ECM_CONNECT_SPEED_DOWNSTREAM };

/* Diagnostic counters (accessible from application) */
volatile uint32_t ncm_diag_rx_ntb;       /* NTBs received (ncm_bulk_out called) */
volatile uint32_t ncm_diag_rx_restart;   /* NTBs dropped (goto restart_rx) */
volatile uint32_t ncm_diag_rx_dgrams;    /* datagrams extracted */
volatile uint32_t ncm_diag_tx_flush;     /* ncm_tx_flush called with data */
volatile uint32_t ncm_diag_tx_done;      /* TX completions */
volatile uint32_t ncm_diag_rx_stall;     /* ISR backpressure: fill slot busy */
volatile uint32_t ncm_diag_set_intf;     /* SET_INTERFACE events */
volatile uint8_t  ncm_current_alt;       /* Current alt setting on data intf */
volatile uint32_t ncm_diag_rx_eth_call;  /* eth_rx called */
volatile uint32_t ncm_diag_rx_eth_ok;    /* eth_rx returned non-NULL pbuf */
volatile uint32_t ncm_diag_rx_rearm;     /* task re-armed endpoint (stall recovery) */
volatile uint32_t ncm_diag_rx_eth_hit;   /* eth_rx found data (cnt>0 && cur<cnt) */
volatile uint32_t ncm_diag_rx_pbuf_fail; /* pbuf_alloc returned NULL */
volatile uint32_t ncm_diag_rx_isr_rearm; /* ISR non-stall re-arm */
volatile uint32_t ncm_diag_max_stall_ticks; /* max stall duration (OS ticks) */
static volatile uint32_t g_ncm_stall_start_tick;
/* Per-path restart reason counters */
volatile uint32_t ncm_diag_rst_short;    /* nbytes too short */
volatile uint32_t ncm_diag_rst_nth_sig;  /* NTH signature mismatch */
volatile uint32_t ncm_diag_rst_blklen;   /* block_len validation fail */
volatile uint32_t ncm_diag_rst_ndpidx;   /* NDP index out of range */
volatile uint32_t ncm_diag_rst_ndp_sig;  /* NDP signature mismatch */
volatile uint32_t ncm_diag_rst_ndplen;   /* NDP length validation fail */
volatile uint32_t ncm_diag_rst_nocnt;    /* count == 0 */
volatile uint32_t ncm_diag_rst_dglen;    /* datagram too large */
volatile uint32_t ncm_diag_resync;       /* NTB resync: found NCMH at non-zero offset */
volatile uint32_t ncm_diag_notify_ok;
volatile uint32_t ncm_diag_notify_fail;

void usbd_cdc_ncm_reset_diag(void)
{
    ncm_diag_rx_ntb = 0;
    ncm_diag_rx_restart = 0;
    ncm_diag_rx_dgrams = 0;
    ncm_diag_tx_flush = 0;
    ncm_diag_tx_done = 0;
    ncm_diag_rx_stall = 0;
    ncm_diag_rx_eth_call = 0;
    ncm_diag_rx_eth_ok = 0;
    ncm_diag_rx_rearm = 0;
    ncm_diag_rst_short = 0;
    ncm_diag_rst_nth_sig = 0;
    ncm_diag_rst_blklen = 0;
    ncm_diag_rst_ndpidx = 0;
    ncm_diag_rst_ndp_sig = 0;
    ncm_diag_rst_ndplen = 0;
    ncm_diag_rst_nocnt = 0;
    ncm_diag_rst_dglen = 0;
    ncm_diag_resync = 0;
    ncm_diag_rx_eth_hit = 0;
    ncm_diag_rx_pbuf_fail = 0;
    ncm_diag_rx_isr_rearm = 0;
    ncm_diag_max_stall_ticks = 0;
    ncm_diag_tx_stale_recover = 0;
    ncm_diag_notify_ok = 0;
    ncm_diag_notify_fail = 0;
}

/* Forward declarations for TX aggregation */
static void ncm_tx_reset_aggregation(void);
static int ncm_tx_flush(void);

/* ------------------------------------------------------------------ */
/* Notification (reuses ECM notification mechanism)                    */
/* ------------------------------------------------------------------ */
int usbd_cdc_ncm_send_notify(uint8_t notifycode, uint8_t value, uint32_t *speed)
{
    struct cdc_eth_notification *notify = (struct cdc_eth_notification *)g_ncm_notify_buf;
    uint8_t bytes2send = 0;

    notify->bmRequestType = CDC_ECM_BMREQUEST_TYPE_ECM;
    notify->bNotificationType = notifycode;

    switch (notifycode) {
    case CDC_ECM_NOTIFY_CODE_NETWORK_CONNECTION:
        notify->wValue = value;
        notify->wIndex = g_cmd_intf;
        notify->wLength = 0U;
        for (uint8_t i = 0U; i < 8U; i++)
            notify->data[i] = 0U;
        bytes2send = 8U;
        break;
    case CDC_ECM_NOTIFY_CODE_CONNECTION_SPEED_CHANGE:
        notify->wValue = 0U;
        notify->wIndex = g_cmd_intf;
        notify->wLength = 0x0008U;
        bytes2send = 16U;
        memcpy(notify->data, speed, 8);
        break;
    default:
        return -1;
    }

    if (bytes2send) {
        int ret = usbd_ep_start_write(cdc_ncm_ep_data[CDC_NCM_INT_EP_IDX].ep_addr,
                                      g_ncm_notify_buf, bytes2send);
        if (ret < 0) {
            ncm_diag_notify_fail++;
            USB_LOG_WRN("NCM notify FAIL: code=0x%02x ret=%d\r\n",
                        notifycode, ret);
            return ret;
        }
        ncm_diag_notify_ok++;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Class request handler                                              */
/* ------------------------------------------------------------------ */
static uint16_t g_max_datagram_size = CONFIG_CDC_NCM_ETH_MAX_SEGSZE;

static int ncm_class_interface_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USB_LOG_DBG("NCM req: 0x%02x wValue=0x%04x wIndex=0x%04x wLen=%u\r\n",
                setup->bRequest, setup->wValue, setup->wIndex, setup->wLength);

    g_cmd_intf = LO_BYTE(setup->wIndex);

    switch (setup->bRequest) {
    case CDC_REQUEST_GET_NTB_PARAMETERS:
        *data = (uint8_t *)&g_ntb_parameters;
        *len = sizeof(g_ntb_parameters);
        break;
    case CDC_REQUEST_GET_NTB_INPUT_SIZE:
        *data = (uint8_t *)&g_ntb_input_size;
        *len = 4;
        break;
    case CDC_REQUEST_SET_NTB_INPUT_SIZE: {
        uint32_t req_size;
        if (*len >= 4) {
            memcpy(&req_size, *data, 4);
            if (req_size >= sizeof(struct cdc_ncm_nth16) &&
                req_size <= CONFIG_CDC_NCM_NTB_MAX_SIZE)
                g_ntb_input_size = req_size;
        }
        break;
    }
    case CDC_REQUEST_GET_NTB_FORMAT:
        *data = (uint8_t *)&g_ntb_format;
        *len = 2;
        break;
    case CDC_REQUEST_SET_NTB_FORMAT:
        if (setup->wValue == 0)
            g_ntb_format = 0; /* NTH16 */
        else
            return -1; /* NTH32 not supported */
        break;
    case CDC_REQUEST_GET_MAX_DATAGRAM_SIZE:
        *data = (uint8_t *)&g_max_datagram_size;
        *len = 2;
        break;
    case CDC_REQUEST_SET_MAX_DATAGRAM_SIZE:
        if (*len >= 2)
            memcpy(&g_max_datagram_size, *data, 2);
        break;
    case CDC_REQUEST_SET_ETHERNET_PACKET_FILTER:
        break;
    case CDC_REQUEST_GET_NET_ADDRESS:
    case CDC_REQUEST_SET_NET_ADDRESS:
    case CDC_REQUEST_GET_CRC_MODE:
    case CDC_REQUEST_SET_CRC_MODE:
        /* Accept silently - return zero data for GET, ignore SET */
        break;
    default:
        USB_LOG_WRN("Unhandled NCM bRequest 0x%02x\r\n", setup->bRequest);
        return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------ */
/* USB event / endpoint callbacks                                     */
/* ------------------------------------------------------------------ */
static void ncm_notify_handler(uint8_t event, void *arg)
{
    switch (event) {
    case USBD_EVENT_RESET:
        ncm_current_alt = 0;
        g_current_net_status = 0;
        g_ncm_rx_slots[0].dgram_count = 0;
        g_ncm_rx_slots[1].dgram_count = 0;
        g_ncm_rx_fill_idx = 0;
        g_ncm_rx_task_idx = 0;
        g_ncm_rx_task_dgram = 0;
        g_ncm_rx_ep_stalled = 0;
        g_ncm_tx_data_length = 0;
        g_ncm_tx_fill_idx = 0;
        g_ncm_tx_dgram_count = 0;
        g_ncm_tx_next_offset = sizeof(struct cdc_ncm_nth16);
        g_tx_sequence = 0;
        break;
    case USBD_EVENT_CONFIGURED:
        /* NCM data interface starts inactive (alt 0, no endpoints).
         * Bulk reads begin when host selects alt 1 via SET_INTERFACE. */
        break;
    case USBD_EVENT_SET_INTERFACE: {
        /* Only activate data path when host selects alt 1 (bulk endpoints).
         * alt 0 means deactivate — no bulk endpoints, no notification.
         * Communication interface (no alt settings) is ignored. */
        struct usb_interface_descriptor *desc =
            (struct usb_interface_descriptor *)arg;
        ncm_diag_set_intf++;
        if (!desc || desc->bAlternateSetting != 1)
            break;
        g_ncm_rx_slots[0].dgram_count = 0;
        g_ncm_rx_slots[1].dgram_count = 0;
        g_ncm_rx_fill_idx = 0;
        g_ncm_rx_task_idx = 0;
        g_ncm_rx_task_dgram = 0;
        g_ncm_rx_ep_stalled = 0;
        NCM_DCACHE_INVAL(g_ncm_rx_buffers[0],
                         CONFIG_CDC_NCM_NTB_MAX_SIZE);
        usbd_ep_start_read(cdc_ncm_ep_data[CDC_NCM_OUT_EP_IDX].ep_addr,
                           g_ncm_rx_buffers[0],
                           CONFIG_CDC_NCM_NTB_MAX_SIZE);
        usbd_cdc_ncm_set_interface_done();
        break;
    }
    default:
        break;
    }
}

/**
 * RX bulk OUT callback — ISR context, once per complete NTB.
 *
 * Double-buffer: parse NTB from current fill buffer, hand off to task
 * via read_idx, swap fill_idx, re-arm endpoint with the other buffer
 * immediately.  Task reads from read buffer while DMA fills the other.
 */
static void ncm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    struct cdc_ncm_nth16 *nth;
    struct cdc_ncm_ndp16 *ndp;
    uint16_t block_len, dg_len, count;
    uint8_t cur_idx = g_ncm_rx_fill_idx;
    uint8_t *rxbuf = g_ncm_rx_buffers[cur_idx];

    ncm_diag_rx_ntb++;

    if (nbytes < sizeof(struct cdc_ncm_nth16)) {
        ncm_diag_rst_short++;
        goto restart_rx;
    }

    nth = (struct cdc_ncm_nth16 *)rxbuf;

    if (nth->dwSignature != CDC_NCM_NTH16_SIGNATURE) {
        /* Diagnostic: dump first 16 bytes when NTH signature mismatches */
        if (ncm_diag_rst_nth_sig < 5) {
            USB_LOG_ERR("NTH sig BAD: nb=%lu sig=%08x buf=%p "
                        "hex=[%02x %02x %02x %02x %02x %02x %02x %02x "
                        "%02x %02x %02x %02x %02x %02x %02x %02x]\r\n",
                        (unsigned long)nbytes,
                        (unsigned)nth->dwSignature,
                        rxbuf,
                        rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3],
                        rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7],
                        rxbuf[8], rxbuf[9], rxbuf[10], rxbuf[11],
                        rxbuf[12], rxbuf[13], rxbuf[14], rxbuf[15]);
        }
        /* NTB boundary misaligned (e.g. after FIFO_CLR lost data).
         * Scan buffer for next valid NCMH signature to resync. */
        uint32_t found = 0;
        for (uint32_t off = 4; off + 28 <= nbytes; off += 4) {
            if (*(volatile uint32_t *)(rxbuf + off) ==
                CDC_NCM_NTH16_SIGNATURE) {
                struct cdc_ncm_nth16 *nth2 =
                    (struct cdc_ncm_nth16 *)(rxbuf + off);
                uint16_t blen2 = nth2->wBlockLength;
                if (blen2 >= 28 &&
                    blen2 <= CONFIG_CDC_NCM_NTB_MAX_SIZE &&
                    off + blen2 <= nbytes) {
                    /* Complete NTB at offset — adjust pointers */
                    rxbuf += off;
                    nbytes -= off;
                    nth = nth2;
                    ncm_diag_resync++;
                    found = 1;
                    break;
                }
            }
        }
        if (!found) {
            ncm_diag_rst_nth_sig++;
            goto restart_rx;
        }
    }

    block_len = nth->wBlockLength;

    if (block_len < sizeof(struct cdc_ncm_nth16) ||
        block_len > CONFIG_CDC_NCM_NTB_MAX_SIZE ||
        nbytes < block_len) {
        ncm_diag_rst_blklen++;
        if (ncm_diag_rst_blklen <= 3)
            USB_LOG_ERR("NTB blklen: blen=%u nb=%lu max=%u seq=%u "
                        "nth[sig=%08x seq=%u blen=%u ndp=%u] buf=%p idx=%u\r\n",
                        (unsigned)block_len,
                        (unsigned long)nbytes,
                        (unsigned)CONFIG_CDC_NCM_NTB_MAX_SIZE,
                        (unsigned)nth->wSequence,
                        (unsigned)nth->dwSignature,
                        (unsigned)nth->wSequence,
                        (unsigned)nth->wBlockLength,
                        (unsigned)nth->wNdpIndex,
                        (void *)rxbuf,
                        (unsigned)cur_idx);
        goto restart_rx;
    }

    if (nth->wNdpIndex == 0 || nth->wNdpIndex + 16 > block_len) {
        ncm_diag_rst_ndpidx++;
        goto restart_rx;
    }

    ndp = (struct cdc_ncm_ndp16 *)&rxbuf[nth->wNdpIndex];

    if (ndp->dwSignature != CDC_NCM_NDP16_SIGNATURE) {
        ncm_diag_rst_ndp_sig++;
        goto restart_rx;
    }

    if (ndp->wLength < 16 || nth->wNdpIndex + ndp->wLength > block_len) {
        ncm_diag_rst_ndplen++;
        goto restart_rx;
    }

    /* Count valid datagrams in NDP16 (stop at {0,0} terminator) */
    count = 0;
    uint16_t max_entries = (ndp->wLength - 8) / 4;
    for (uint16_t i = 0; i < max_entries; i++) {
        if (ndp->datagram[i].wDatagramIndex == 0 &&
            ndp->datagram[i].wDatagramLength == 0)
            break;
        if (ndp->datagram[i].wDatagramIndex +
            ndp->datagram[i].wDatagramLength > block_len)
            break;
        count++;
    }

    if (count == 0) {
        ncm_diag_rst_nocnt++;
        goto restart_rx;
    }

    dg_len = ndp->datagram[0].wDatagramLength;
    if (dg_len > CONFIG_CDC_NCM_ETH_MAX_SEGSZE) {
        ncm_diag_rst_dglen++;
        goto restart_rx;
    }

    /* Publish per-slot metadata, swap fill index */
    g_ncm_rx_slots[cur_idx].base_offset =
        (uint16_t)(rxbuf - g_ncm_rx_buffers[cur_idx]);
    g_ncm_rx_slots[cur_idx].ndp_offset = nth->wNdpIndex;
    g_ncm_rx_slots[cur_idx].dgram_count = count;  /* publish last */

    g_ncm_rx_fill_idx ^= 1;

    /* Re-arm only if the new fill slot is free (task consumed it).
     * Otherwise backpressure: host sees NAK until task frees a slot.
     * Invalidate cache before re-arm: discard stale lines from previous
     * NTB so next DMA completion reads fresh data from PSRAM. */
    if (g_ncm_rx_slots[g_ncm_rx_fill_idx].dgram_count == 0) {
        NCM_DCACHE_INVAL(g_ncm_rx_buffers[g_ncm_rx_fill_idx],
                         CONFIG_CDC_NCM_NTB_MAX_SIZE);
        usbd_ep_start_read(ep, g_ncm_rx_buffers[g_ncm_rx_fill_idx],
                           CONFIG_CDC_NCM_NTB_MAX_SIZE);
        ncm_diag_rx_isr_rearm++;
    } else {
        g_ncm_rx_ep_stalled = 1;
        g_ncm_stall_start_tick = xTaskGetTickCountFromISR();
        ncm_diag_rx_stall++;
    }

    ncm_diag_rx_dgrams += count;
    usbd_cdc_ncm_data_recv_done(&rxbuf[ndp->datagram[0].wDatagramIndex],
                                dg_len);
    return;

restart_rx:
    ncm_diag_rx_restart++;
    NCM_DCACHE_INVAL(g_ncm_rx_buffers[g_ncm_rx_fill_idx],
                     CONFIG_CDC_NCM_NTB_MAX_SIZE);
    usbd_ep_start_read(ep, g_ncm_rx_buffers[g_ncm_rx_fill_idx],
                       CONFIG_CDC_NCM_NTB_MAX_SIZE);
}

/**
 * TX bulk IN callback: handle ZLP if needed.
 * Does NOT auto-flush pending datagrams here — ncm_tx_flush() touches
 * aggregation state that is shared with task-context callers protected
 * by taskENTER_CRITICAL().  On ECLIC/BASEPRI ports the critical section
 * may not mask the USB ISR, so we let the 1ms flush timer handle it.
 */
volatile uint32_t ncm_tx_done_cnt;

static void ncm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    ncm_diag_tx_last_nbytes = nbytes;
    if ((nbytes % CDC_NCM_MAX_PACKET_SIZE) == 0 && nbytes) {
        ncm_diag_tx_zlp++;
        usbd_ep_start_write(ep, NULL, 0);
    } else {
        ncm_tx_done_cnt++;
        ncm_diag_tx_done++;
        g_ncm_tx_data_length = 0;
        usbd_cdc_ncm_data_send_done();
    }
}

/**
 * INT IN callback: continue connection notification sequence.
 */
static void ncm_int_in(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_DBG("NCM int_in: nb=%lu st=%u\r\n",
                (unsigned long)nbytes, g_current_net_status);
    if (g_current_net_status == 1) {
        int ret = usbd_cdc_ncm_send_notify(
            CDC_ECM_NOTIFY_CODE_CONNECTION_SPEED_CHANGE,
            0, g_connect_speed_table);
        if (ret == 0)
            g_current_net_status = 2;
        /* If write failed, stay at state 1 — task will retry */
    }
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */
int usbd_cdc_ncm_send_connect_status(bool connected)
{
    int ret;

    if (connected) {
        ret = usbd_cdc_ncm_send_notify(CDC_ECM_NOTIFY_CODE_NETWORK_CONNECTION,
                                       CDC_ECM_NET_CONNECTED, NULL);
        if (ret == 0)
            g_current_net_status = 1;
    } else {
        ret = usbd_cdc_ncm_send_notify(CDC_ECM_NOTIFY_CODE_NETWORK_CONNECTION,
                                       CDC_ECM_NET_DISCONNECTED, NULL);
        g_current_net_status = 0;
    }
    return ret;
}

uint8_t usbd_cdc_ncm_get_net_status(void)
{
    return g_current_net_status;
}

void usbd_cdc_ncm_recover_stale_rx(void)
{
    uint8_t ep = cdc_ncm_ep_data[CDC_NCM_OUT_EP_IDX].ep_addr;

    ncm_diag_rx_restart++;

    /* Reset both slot metadata so bulk_out callback re-arms cleanly */
    g_ncm_rx_slots[0].dgram_count = 0;
    g_ncm_rx_slots[1].dgram_count = 0;
    g_ncm_rx_fill_idx = 0;
    g_ncm_rx_task_idx = 0;
    g_ncm_rx_task_dgram = 0;
    g_ncm_rx_ep_stalled = 0;

    NCM_DCACHE_INVAL(g_ncm_rx_buffers[0], CONFIG_CDC_NCM_NTB_MAX_SIZE);
    usbd_ep_start_read(ep, g_ncm_rx_buffers[0],
                       CONFIG_CDC_NCM_NTB_MAX_SIZE);
}

uint8_t *usbd_cdc_ncm_get_tx_buffer(void)
{
    /* Return NC pointer at current fill position in active fill buffer */
    return &g_ncm_tx_buffers[g_ncm_tx_fill_idx][g_ncm_tx_next_offset];
}

/**
 * Build NTH16+NDP16 wrapper around Ethernet frame and send immediately.
 * Legacy single-datagram API — still works for non-lwIP callers.
 * Uses the fill buffer and swaps to the other on completion.
 */
int usbd_cdc_ncm_send_packet(uint8_t *buf, uint32_t len)
{
    if (g_ncm_tx_data_length > 0)
        return -USB_ERR_BUSY;

    uint8_t *txbuf = g_ncm_tx_buffers[g_ncm_tx_fill_idx];
    uint32_t total = (uint32_t)CDC_NCM_NTB_OVERHEAD + len;
    total = (total + 3u) & ~3u;
    if (total > CONFIG_CDC_NCM_NTB_MAX_SIZE)
        return -USB_ERR_INVAL;
    uint16_t block_len = (uint16_t)total;

    /* Build NTH16 at offset 0 */
    struct cdc_ncm_nth16 *nth = (struct cdc_ncm_nth16 *)txbuf;
    nth->dwSignature = CDC_NCM_NTH16_SIGNATURE;
    nth->wHeaderLength = sizeof(struct cdc_ncm_nth16);
    nth->wSequence = g_tx_sequence++;
    nth->wBlockLength = block_len;
    nth->wNdpIndex = sizeof(struct cdc_ncm_nth16); /* 12 */

    /* Build NDP16 at offset 12 */
    struct cdc_ncm_ndp16 *ndp =
        (struct cdc_ncm_ndp16 *)&txbuf[sizeof(struct cdc_ncm_nth16)];
    ndp->dwSignature = CDC_NCM_NDP16_SIGNATURE;
    ndp->wLength = 16; /* 8 (header) + 4 (datagram entry) + 4 (terminator) */
    ndp->wNextNdpIndex = 0;
    ndp->datagram[0].wDatagramIndex = CDC_NCM_NTB_OVERHEAD; /* 28 */
    ndp->datagram[0].wDatagramLength = len;
    ndp->datagram[1].wDatagramIndex = 0;
    ndp->datagram[1].wDatagramLength = 0;

    g_ncm_tx_data_length = block_len;
    g_ncm_tx_start_tick = xTaskGetTickCount();

    /* Swap to other buffer for future fills */
    g_ncm_tx_fill_idx ^= 1;
    ncm_tx_reset_aggregation();

    return usbd_ep_start_write(cdc_ncm_ep_data[CDC_NCM_IN_EP_IDX].ep_addr,
                               txbuf, block_len);
}

/* ------------------------------------------------------------------ */
/* TX multi-datagram aggregation                                       */
/* ------------------------------------------------------------------ */

static void ncm_tx_reset_aggregation(void)
{
    g_ncm_tx_dgram_count = 0;
    /* Datagrams start right after NTH16 (NDP16 goes at the end) */
    g_ncm_tx_next_offset = sizeof(struct cdc_ncm_nth16);
}

/**
 * Flush aggregated datagrams as a single NTB.
 * NTB layout: [NTH16] [dgram0 4-aligned] [dgram1 4-aligned] ... [NDP16]
 * Uses ping-pong double buffer: flushes from fill buffer, then swaps
 * fill index so new datagrams go to the other buffer.
 * Returns 0 on success, negative on error, 1 if nothing to flush.
 */
static int ncm_tx_flush(void)
{
    uint16_t count = g_ncm_tx_dgram_count;

    if (count == 0)
        return 1;

    if (g_ncm_tx_data_length > 0)
        return -USB_ERR_BUSY;

    uint8_t *buf = g_ncm_tx_buffers[g_ncm_tx_fill_idx];

    /* NDP16 goes after last datagram, 4-byte aligned */
    uint16_t ndp_offset = (g_ncm_tx_next_offset + 3u) & ~3u;
    /* NDP16 size: 8 (header) + 4*(count+1) (entries + terminator) */
    uint16_t ndp_size = 8 + 4 * (count + 1);
    uint16_t block_len = ndp_offset + ndp_size;

    if (block_len > CONFIG_CDC_NCM_NTB_MAX_SIZE) {
        ncm_tx_reset_aggregation();
        return -USB_ERR_INVAL;
    }

    /* Build NTH16 at offset 0 */
    struct cdc_ncm_nth16 *nth = (struct cdc_ncm_nth16 *)buf;
    nth->dwSignature = CDC_NCM_NTH16_SIGNATURE;
    nth->wHeaderLength = sizeof(struct cdc_ncm_nth16);
    nth->wSequence = g_tx_sequence++;
    nth->wBlockLength = block_len;
    nth->wNdpIndex = ndp_offset;

    /* Build NDP16 */
    struct cdc_ncm_ndp16 *ndp = (struct cdc_ncm_ndp16 *)&buf[ndp_offset];
    ndp->dwSignature = CDC_NCM_NDP16_SIGNATURE;
    ndp->wLength = ndp_size;
    ndp->wNextNdpIndex = 0;

    for (uint16_t i = 0; i < count; i++) {
        ndp->datagram[i].wDatagramIndex = g_ncm_tx_dgram_table[i].offset;
        ndp->datagram[i].wDatagramLength = g_ncm_tx_dgram_table[i].length;
    }
    /* Terminator */
    ndp->datagram[count].wDatagramIndex = 0;
    ndp->datagram[count].wDatagramLength = 0;

    g_ncm_tx_data_length = block_len;
    g_ncm_tx_start_tick = xTaskGetTickCount();
    ncm_diag_tx_flush++;
    ncm_diag_tx_last_len = block_len;

    /* Swap to other buffer for filling while USB transmits this one */
    g_ncm_tx_fill_idx ^= 1;
    ncm_tx_reset_aggregation();

    return usbd_ep_start_write(cdc_ncm_ep_data[CDC_NCM_IN_EP_IDX].ep_addr,
                               buf, block_len);
}

int usbd_cdc_ncm_tx_flush(void)
{
    return ncm_tx_flush();
}

uint8_t usbd_cdc_ncm_get_rx_stalled(void)
{
    return g_ncm_rx_ep_stalled;
}

uint8_t usbd_cdc_ncm_get_rx_fill_idx(void)
{
    return g_ncm_rx_fill_idx;
}

void usbd_cdc_ncm_get_slot_info(uint8_t slot, uint16_t *dgram_count)
{
    if (slot < 2 && dgram_count)
        *dgram_count = g_ncm_rx_slots[slot].dgram_count;
}

uint32_t usbd_cdc_ncm_get_tx_data_length(void)
{
    return g_ncm_tx_data_length;
}

/**
 * Recover stale TX transfer.
 * Called from task context (e.g. RX task loop).
 * If g_ncm_tx_data_length > 0 for longer than timeout_ms,
 * the transfer is assumed dead (e.g. EP stall cleared by host
 * without completing the transfer).  Drop it and allow new TX.
 * Returns 1 if recovery happened, 0 otherwise.
 */
int usbd_cdc_ncm_tx_recover_stale(uint32_t timeout_ms)
{
    if (g_ncm_tx_data_length == 0)
        return 0;

    TickType_t elapsed = xTaskGetTickCount() - g_ncm_tx_start_tick;
    if (elapsed < pdMS_TO_TICKS(timeout_ms))
        return 0;

    {
        /* Dump EP4 IN hardware state at stale recovery */
        uint8_t in_ep_addr = cdc_ncm_ep_data[CDC_NCM_IN_EP_IDX].ep_addr;
        uint8_t eidx = in_ep_addr & 0x7F;
        uint32_t ep_cfg = *(volatile uint32_t *)(0x2000D000 + 0x40 + 4 * (eidx - 1));
        uint32_t fifo_sts = *(volatile uint32_t *)(0x2000D000 + 0x104 + 0x10 * eidx);
        USB_LOG_WRN("NCM TX stale: len=%lu elapsed=%lums "
                    "ep:%02x cfg=0x%08lx fifo=0x%08lx, recovering\r\n",
                    (unsigned long)g_ncm_tx_data_length,
                    (unsigned long)(elapsed * portTICK_PERIOD_MS),
                    in_ep_addr,
                    (unsigned long)ep_cfg, (unsigned long)fifo_sts);
    }
    g_ncm_tx_data_length = 0;
    ncm_diag_tx_stale_recover++;
    return 1;
}

void usbd_cdc_ncm_start_read_next(void)
{
    g_ncm_rx_slots[0].dgram_count = 0;
    g_ncm_rx_slots[1].dgram_count = 0;
    g_ncm_rx_fill_idx = 0;
    g_ncm_rx_task_idx = 0;
    g_ncm_rx_task_dgram = 0;
    g_ncm_rx_ep_stalled = 0;
    NCM_DCACHE_INVAL(g_ncm_rx_buffers[0], CONFIG_CDC_NCM_NTB_MAX_SIZE);
    usbd_ep_start_read(cdc_ncm_ep_data[CDC_NCM_OUT_EP_IDX].ep_addr,
                       g_ncm_rx_buffers[0],
                       CONFIG_CDC_NCM_NTB_MAX_SIZE);
}

#ifdef CONFIG_USBDEV_CDC_NCM_USING_LWIP
/**
 * Task-context: extract next datagram from the read buffer as a pbuf.
 * Uses task-owned cursor (task_idx/task_dgram) — safe from ISR races.
 * When current slot is exhausted, checks the other slot for pending data.
 */
struct pbuf *usbd_cdc_ncm_eth_rx(void)
{
    uint8_t idx = g_ncm_rx_task_idx;
    uint16_t cur = g_ncm_rx_task_dgram;
    struct ncm_rx_slot *slot = &g_ncm_rx_slots[idx];
    uint16_t cnt = slot->dgram_count;

    ncm_diag_rx_eth_call++;

    /* Current slot exhausted — mark free, unstall EP if needed, check other */
    if (cnt == 0 || cur >= cnt) {
        if (cnt > 0) {
            slot->dgram_count = 0;  /* release to ISR */

            /* If ISR stalled (fill slot was busy), re-arm now.
             * Use global interrupt disable — taskENTER_CRITICAL uses
             * ECLIC threshold which may not mask high-priority USB ISR. */
            if (g_ncm_rx_ep_stalled) {
                unsigned long _mstatus;
                uint32_t dur = xTaskGetTickCount() - g_ncm_stall_start_tick;
                if (dur > ncm_diag_max_stall_ticks)
                    ncm_diag_max_stall_ticks = dur;
                NCM_DCACHE_INVAL(g_ncm_rx_buffers[g_ncm_rx_fill_idx],
                                 CONFIG_CDC_NCM_NTB_MAX_SIZE);
                __asm volatile("csrrc %0, mstatus, 8" : "=r"(_mstatus));
                g_ncm_rx_ep_stalled = 0;
                usbd_ep_start_read(
                    cdc_ncm_ep_data[CDC_NCM_OUT_EP_IDX].ep_addr,
                    g_ncm_rx_buffers[g_ncm_rx_fill_idx],
                    CONFIG_CDC_NCM_NTB_MAX_SIZE);
                if (_mstatus & 8)
                    __asm volatile("csrs mstatus, 8");
                ncm_diag_rx_rearm++;
            }

            idx ^= 1;
            g_ncm_rx_task_idx = idx;
            g_ncm_rx_task_dgram = 0;
            slot = &g_ncm_rx_slots[idx];
            cnt = slot->dgram_count;
            cur = 0;
        }
        if (cnt == 0 || cur >= cnt)
            return NULL;
    }

    ncm_diag_rx_eth_hit++;
    uint8_t *rxbuf = g_ncm_rx_buffers[idx] + slot->base_offset;
    struct cdc_ncm_ndp16 *ndp =
        (struct cdc_ncm_ndp16 *)&rxbuf[slot->ndp_offset];

    /* Find next valid datagram (skip oversized/zero entries) */
    uint16_t dg_len;
    while (cur < cnt) {
        dg_len = ndp->datagram[cur].wDatagramLength;
        if (dg_len > 0 && dg_len <= CONFIG_CDC_NCM_ETH_MAX_SEGSZE)
            break;
        cur++;
    }

    if (cur >= cnt) {
        g_ncm_rx_task_dgram = cur;
        return NULL;
    }

    struct pbuf *p = pbuf_alloc(PBUF_RAW, dg_len, PBUF_POOL);
    if (p == NULL) {
        ncm_diag_rx_pbuf_fail++;
        g_ncm_rx_task_dgram = cnt;
        return NULL;
    }

    /* Use pbuf_take to correctly distribute data across the pbuf chain.
     * pbuf_alloc(PBUF_POOL) may create a chain when dg_len exceeds a
     * single pool buffer (PBUF_POOL_BUFSIZE).  A plain memcpy into
     * p->payload would overflow the first buffer and corrupt the heap. */
    pbuf_take(p, &rxbuf[ndp->datagram[cur].wDatagramIndex], dg_len);

    ncm_diag_rx_eth_ok++;
    g_ncm_rx_task_dgram = cur + 1;
    return p;
}

int usbd_cdc_ncm_eth_tx(struct pbuf *p)
{
    struct pbuf *q;
    uint8_t *buffer;
    uint32_t frame_len;
    uint8_t *txbuf = g_ncm_tx_buffers[g_ncm_tx_fill_idx];

    if (p->tot_len > CONFIG_CDC_NCM_ETH_MAX_SEGSZE)
        return -USB_ERR_INVAL;

    frame_len = p->tot_len;
    if (frame_len < 60)
        frame_len = 60;

    /* 4-byte align the datagram offset */
    uint16_t dg_offset = (g_ncm_tx_next_offset + 3u) & ~3u;

    /* Check if this datagram fits. Account for NDP16 at the end:
     * NDP16 = 8 + 4*(count+1+1) (entries + new + terminator) */
    uint16_t ndp_size = 8 + 4 * (g_ncm_tx_dgram_count + 2);
    uint16_t needed = dg_offset + frame_len + ndp_size;

    if (needed > CONFIG_CDC_NCM_NTB_MAX_SIZE ||
        g_ncm_tx_dgram_count >= CONFIG_CDC_NCM_MAX_DATAGRAMS) {
        /* Auto-flush current aggregation before adding this frame.
         * This swaps fill buffer, so txbuf may change. */
        int ret = ncm_tx_flush();
        if (ret < 0)
            return ret;
        /* Re-read fill buffer after swap */
        txbuf = g_ncm_tx_buffers[g_ncm_tx_fill_idx];
        dg_offset = (g_ncm_tx_next_offset + 3u) & ~3u;
    }

    /* Copy pbuf chain into fill buffer */
    buffer = &txbuf[dg_offset];
    for (q = p; q != NULL; q = q->next) {
        memcpy(buffer, q->payload, q->len);
        buffer += q->len;
    }

    /* Pad short frames */
    if (p->tot_len < 60)
        memset(&txbuf[dg_offset + p->tot_len], 0, 60 - p->tot_len);

    /* Record in aggregation table */
    g_ncm_tx_dgram_table[g_ncm_tx_dgram_count].offset = dg_offset;
    g_ncm_tx_dgram_table[g_ncm_tx_dgram_count].length = frame_len;
    g_ncm_tx_dgram_count++;
    g_ncm_tx_next_offset = dg_offset + frame_len;

    return 0;
}
#endif

struct usbd_interface *usbd_cdc_ncm_init_intf(struct usbd_interface *intf,
                                              const uint8_t int_ep,
                                              const uint8_t out_ep,
                                              const uint8_t in_ep)
{
    intf->class_interface_handler = ncm_class_interface_request_handler;
    intf->class_endpoint_handler = NULL;
    intf->vendor_handler = NULL;
    intf->notify_handler = ncm_notify_handler;

    /* Clean+invalidate .bss dirty cache lines for NTB buffers.
     * .bss zeroing leaves dirty lines in cache; clean writes them to PSRAM,
     * invalidate evicts them so subsequent CPU access refetches from PSRAM. */
    NCM_DCACHE_FLUSH_INVAL(g_ncm_rx_buffers, sizeof(g_ncm_rx_buffers));
    NCM_DCACHE_FLUSH_INVAL(g_ncm_tx_buffers, sizeof(g_ncm_tx_buffers));
    g_ncm_rx_fill_idx = 0;
    g_ncm_rx_slots[0].dgram_count = 0;
    g_ncm_rx_slots[1].dgram_count = 0;
    g_ncm_rx_task_idx = 0;
    g_ncm_rx_task_dgram = 0;
    g_ncm_rx_ep_stalled = 0;

    USB_LOG_INFO("NCM buffers: rx0=%p rx1=%p tx0=%p tx1=%p\r\n",
                 (void *)g_ncm_rx_buffers[0], (void *)g_ncm_rx_buffers[1],
                 (void *)g_ncm_tx_buffers[0], (void *)g_ncm_tx_buffers[1]);

    ncm_tx_reset_aggregation();

    cdc_ncm_ep_data[CDC_NCM_OUT_EP_IDX].ep_addr = out_ep;
    cdc_ncm_ep_data[CDC_NCM_OUT_EP_IDX].ep_cb = ncm_bulk_out;
    cdc_ncm_ep_data[CDC_NCM_IN_EP_IDX].ep_addr = in_ep;
    cdc_ncm_ep_data[CDC_NCM_IN_EP_IDX].ep_cb = ncm_bulk_in;
    cdc_ncm_ep_data[CDC_NCM_INT_EP_IDX].ep_addr = int_ep;
    cdc_ncm_ep_data[CDC_NCM_INT_EP_IDX].ep_cb = ncm_int_in;

    usbd_add_endpoint(&cdc_ncm_ep_data[CDC_NCM_OUT_EP_IDX]);
    usbd_add_endpoint(&cdc_ncm_ep_data[CDC_NCM_IN_EP_IDX]);
    usbd_add_endpoint(&cdc_ncm_ep_data[CDC_NCM_INT_EP_IDX]);

    return intf;
}

void usbd_cdc_ncm_set_connect_speed(uint32_t speed[2])
{
    memcpy(g_connect_speed_table, speed, 8);
}

__WEAK void usbd_cdc_ncm_data_recv_done(uint8_t *buf, uint32_t len)
{
}

__WEAK void usbd_cdc_ncm_data_send_done(void)
{
}

__WEAK void usbd_cdc_ncm_set_interface_done(void)
{
}
