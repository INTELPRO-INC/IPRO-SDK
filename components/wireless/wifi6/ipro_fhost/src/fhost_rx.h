/**
 ****************************************************************************************
 *
 * @file fhost_rx.h
 *
 * @brief Definitions of the fully hosted RX task.
 *
 * Copyright (C) RivieraWaves 2017-2021
 *
 ****************************************************************************************
 */

#ifndef _FHOST_RX_H_
#define _FHOST_RX_H_

/**
 ****************************************************************************************
 * @defgroup FHOST_RX FHOST_RX
 * @ingroup FHOST
 * @brief Fully Hosted RX task implementation.
 * This module creates a task that will be used to handle the RX descriptors passed by
 * the WiFi task.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "fhost_rx_def.h"
#include "net_def.h"
#include "export/adapter/rtos.h"
#include "cfgrwnx.h"
#include "fhost_rx_def.h"
/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/// Rx Buffer action status
enum rx_buf_action_status
{
    // forwarded to upper layer
    RX_BUF_FORWARD = CO_BIT(0),
    // resent on wireless interface
    RX_BUF_RESEND = CO_BIT(1),
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
struct rx_vector_1_pad{
    uint32_t pad[4];
};
struct rx_vector_2_pad{
    uint32_t pad[2];
};
/// Structure containing information about the received frame (length, timestamp, rate, etc.)
struct rx_vector
{
    /// Total length of the received MPDU
    uint16_t            frmlen;
    /// AMPDU status information
    uint16_t            ampdu_stat_info;
    /// TSF Low
    uint32_t            tsflo;
    /// TSF High
    uint32_t            tsfhi;
    /// Receive Vector 1
    struct rx_vector_1_pad  rx_vec_1;
    /// Receive Vector 2
    struct rx_vector_2_pad  rx_vec_2;
    /// MPDU status information
    uint32_t            statinfo;
};

/// Structure containing the information about the received payload
struct rx_info
{
    /// Rx header descriptor (this element MUST be the first of the structure)
    struct rx_vector vect;
    /// Structure containing the information about the PHY channel that was used for this RX
    struct phy_channel_info phy_info;
    /// Word containing some SW flags about the RX packet
    uint32_t flags;
    #if NX_AMSDU_DEAGG
    /// Array of host buffer identifiers for the other A-MSDU subframes
    uint32_t amsdu_hostids[NX_MAX_MSDU_PER_RX_AMSDU - 1];
    /// Array of A-MSDU subframe lengths (must match rx_dmadesc layout)
    uint16_t amsdu_len[NX_MAX_MSDU_PER_RX_AMSDU];
    #endif
    /// Spare room for LMAC FW to write a pattern when last DMA is sent
    uint32_t pattern;
};

/// Structure containing the information about the payload that should be resend
struct tx_info
{
    /// Network stack buffer element
    net_al_tx_t net_buf;
};

/// FHOST RX environment structure
struct fhost_rx_buf_tag
{
    /// Network stack buffer element - Must be the first field in the structure because of the implicit cast
    /// by net_al when calling @ref fhost_rx_buf_push (via free_fn) to free a net_al_rx_t!!!
    // net_al_rx_t net_buf;
    uint8_t fnet_buf[64];
    /// Structure containing the information to resend on Tx
    struct tx_info info_tx;
    /// Number of reference to Rx buffer
    uint8_t ref;
    /// Structure containing the information about the received payload - Payload must be just after !!!
    struct rx_info info;
    /// Payload buffer space - must be after rx_info  !!!
    uint32_t payload[CO_ALIGN4_HI(FHOST_RX_BUF_SIZE)/sizeof(uint32_t)];
    /// Phase 2A.10 Counter I: us timestamp at macif_rx_buf_get; 0 = idle.
    /// Placed at struct end so the info->payload relative offset is unchanged
    /// (HW DMA writes the payload via the &buf->info pointer in buf_desc.addr).
    uint32_t lease_us;
    /// Phase 2A.12 Counter L0: us timestamp set at macif_rx_desc_upload
    /// just before queue_desc enqueue. (lease_us -> mac_dispatch_us)
    /// bisects HW DMA + rxl_dma_evt + rxu_cntrl_evt + BA reorder.
    /// (mac_dispatch_us -> mac_to_fhost_us) bisects queue_desc dwell +
    /// fhost_rx_task wakeup latency.
    uint32_t mac_dispatch_us;
    /// Phase 2A.11 Counter K0: us timestamp set at fhost_rx_task entry to
    /// processing (just after rtos_queue_read pops msg from queue_desc).
    /// (lease_us -> mac_to_fhost_us) bisects macsw RX path: HW DMA fill +
    /// rxl_dma_evt dispatch + BA reorder hold + queue_desc enqueue.
    uint32_t mac_to_fhost_us;
    /// Phase 2A.11 Counter K1: us timestamp at fhost_rx_buf_send entry (just
    /// before net_al_input). (mac_to_fhost_us -> fhost_to_lwip_us) bisects
    /// fhost_rx_thread queue wait + RX_STAT processing + buf_forward.
    /// (fhost_to_lwip_us -> push) covers lwIP tcpip_input + eth_input_hook
    /// + bl_dual_stack_peer_input + USB pipe (Counter J already isolates
    /// the USB segment at avg=103us).
    uint32_t fhost_to_lwip_us;
};
#if NX_UF_EN
/// FHOST UF environment structure
struct fhost_rx_uf_buf_tag
{
    /// Network stack buffer element - must be the first one in this structure !!!
    uint8_t fnet_buf[64];
    /// Structure containing the information about the received payload
    struct rx_info info;
    /// Payload buffer space (empty in case of UF buffer)
    uint32_t payload[0];
};

/// Structure used, when receiving UF, for the buffer elements exchanged between the FHOST RX and the MAC thread
struct fhost_rx_uf_buf_desc_tag
{
    /// Id of the RX buffer
    uint32_t host_id;
};
#endif // NX_UF_EN

/// Structure used for the inter-task communication
struct fhost_rx_msg_tag
{
    /// RX descriptor structure
    struct rxu_stat_val desc;
};

/// FHOST RX environment structure
struct fhost_rx_env_tag
{
    /// Queue used for the inter-task communication (RX descriptors)
    rtos_queue queue_desc;
    /// Queue used for the inter-task communication (RX buffers)
    rtos_queue queue_buf;
    /// Queue used for the inter-task communication (unsupported frames)
    rtos_queue queue_uf_buf;
    /// Management frame Callback function pointer
    cb_fhost_rx mgmt_cb;
    /// Management Callback parameter
    void *mgmt_cb_arg;
    /// Monitor Callback function pointer
    cb_fhost_rx monitor_cb;
    /// Monitor Callback parameter
    void *monitor_cb_arg;
};

/// Structure used for the buffer elements exchanged between the FHOST RX and the MAC thread
struct fhost_rx_buf_desc_tag
{
    /// Id of the RX buffer
    uint32_t host_id;
    /// Address of the payload inside the buffer
    uint32_t addr;
};


#if NX_FHOST_RX_STATS
/// Structure containing RX statistics
struct fhost_rx_stats_sta
{
    /// Last Received RX vector 1
    struct rx_vector_1  last_rx;
    /// Number of frames received
    int cpt;
    ////Table indicating how many frame has been receive for each rate index
    uint32_t table[];
};
#endif

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
#if NX_FHOST_RX_STATS
/// Pool of RX Statistics
extern struct fhost_rx_stats_sta *rx_stats[CFG_STA_MAX];
#endif // NX_FHOST_RX_STATS

/// FHOST RX environment
extern struct fhost_rx_env_tag fhost_rx_env;

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Push a RX buffer to the WiFi task.
 * This buffer can then be used by the WiFi task to copy a received MPDU.
 *
 * @param[in] net_buf   Pointer to the RX buffer to push
 ****************************************************************************************
 */
void fhost_rx_buf_push(void *net_buf);

/**
 ****************************************************************************************
 * @brief Initialization of the RX task.
 * This function initializes the different data structures used for the RX and creates the
 * RTOS task dedicated to the RX processing.
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_rx_init(void);

/**
 ****************************************************************************************
 * @brief Set the callback to call when receiving management frames (i.e. they have
 * not been processed by the wifi task).
 *
 * @attention The callback is called with a @ref fhost_frame_info parameter that is only
 * valid during the callback. If needed the callback is responsible to save the frame for
 * futher processing.
 *
 * @param[in] cb   Callback function pointer
 * @param[in] arg  Callback parameter (NULL if not needed)
 ****************************************************************************************
 */
void fhost_rx_set_mgmt_cb(void *arg, cb_fhost_rx cb);

/**
 ****************************************************************************************
 * @brief Set the callback to call when receiving packets in monitor mode.
 *
 * @attention The callback is called with a @ref fhost_frame_info parameter that is only
 * valid during the callback. If needed the callback is responsible to save the frame for
 * futher processing.
 *
 * @param[in] cb   Callback function pointer
 * @param[in] arg  Callback parameter (NULL if not needed)
 ****************************************************************************************
 */
void fhost_rx_set_monitor_cb(void *arg, cb_fhost_rx cb);

/// @}

#endif // _FHOST_RX_H_

