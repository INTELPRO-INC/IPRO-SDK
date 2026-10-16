/**
 * @file ieee802154_frame.h
 * @brief IEEE 802.15.4-2013 Frame Format and Utilities
 * 
 * Frame parsing, building, and manipulation utilities
 * Supports 802.15.4-2013 frame formats including:
 * - Legacy frames (2003/2006)
 * - Enhanced frames (2013+)
 * - Multipurpose frames
 * 
 * @copyright Copyright (c) 2025 INTELPRO Inc.
 */

#ifndef __IEEE802154_FRAME_H__
#define __IEEE802154_FRAME_H__

#include "ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup IEEE802154_FRAME Frame Format
 * @{
 */

/**
 * @brief Frame Control Field (FCF) structure
 */
typedef struct {
    uint16_t frame_type : 3;        /**< Frame type */
    uint16_t security_enabled : 1;  /**< Security enabled */
    uint16_t frame_pending : 1;     /**< Frame pending */
    uint16_t ack_request : 1;       /**< ACK request */
    uint16_t pan_id_compress : 1;   /**< PAN ID compression */
    uint16_t reserved : 1;          /**< Reserved */
    uint16_t seq_num_suppress : 1;  /**< Sequence number suppression (2013+) */
    uint16_t ie_present : 1;        /**< IE present (2013+) */
    uint16_t dst_addr_mode : 2;     /**< Destination addressing mode */
    uint16_t frame_version : 2;     /**< Frame version */
    uint16_t src_addr_mode : 2;     /**< Source addressing mode */
} ieee802154_fcf_t;

/**
 * @brief Frame version values
 */
#define IEEE802154_FRAME_VERSION_2003  0
#define IEEE802154_FRAME_VERSION_2006  1
#define IEEE802154_FRAME_VERSION_2015  2  /**< 2013/2015 enhanced frames */

/**
 * @brief Security Control Field
 */
typedef struct {
    uint8_t security_level : 3;     /**< Security level */
    uint8_t key_id_mode : 2;        /**< Key identifier mode */
    uint8_t reserved : 3;           /**< Reserved */
} ieee802154_sec_ctrl_t;

/**
 * @brief Security levels
 */
typedef enum {
    IEEE802154_SEC_NONE = 0,        /**< No security */
    IEEE802154_SEC_MIC_32 = 1,      /**< MIC-32 */
    IEEE802154_SEC_MIC_64 = 2,      /**< MIC-64 */
    IEEE802154_SEC_MIC_128 = 3,     /**< MIC-128 */
    IEEE802154_SEC_ENC = 4,         /**< Encryption only */
    IEEE802154_SEC_ENC_MIC_32 = 5,  /**< Encryption + MIC-32 */
    IEEE802154_SEC_ENC_MIC_64 = 6,  /**< Encryption + MIC-64 */
    IEEE802154_SEC_ENC_MIC_128 = 7, /**< Encryption + MIC-128 */
} ieee802154_sec_level_t;

/**
 * @brief Address information
 */
typedef struct {
    uint16_t pan_id;                /**< PAN ID */
    ieee802154_addr_mode_t mode;    /**< Address mode */
    union {
        uint16_t short_addr;
        ieee802154_ext_addr_t ext_addr;
    } addr;
} ieee802154_address_t;

/**
 * @brief Parsed frame structure
 */
typedef struct {
    /* Frame Control */
    ieee802154_fcf_t fcf;
    
    /* Addressing */
    uint8_t seq_num;                    /**< Sequence number */
    ieee802154_address_t dst;           /**< Destination address */
    ieee802154_address_t src;           /**< Source address */
    
    /* Security */
    ieee802154_sec_ctrl_t sec_ctrl;
    uint32_t frame_counter;
    uint8_t key_id[9];                  /**< Variable length key identifier */
    uint8_t key_id_len;
    
    /* Payload */
    uint8_t *payload;                   /**< Pointer to payload in PSDU */
    uint8_t payload_len;
    
    /* FCS */
    uint16_t fcs;
    
    /* Frame metadata */
    uint8_t header_len;                 /**< MAC header length */
    uint8_t *psdu;                      /**< Pointer to complete PSDU */
    uint8_t psdu_len;                   /**< Total PSDU length */
} ieee802154_frame_info_t;

/**
 * @brief Parse a received frame
 * 
 * @param frame Raw frame data
 * @param info Parsed frame information
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_frame_parse(const ieee802154_frame_t *frame,
                                           ieee802154_frame_info_t *info);

/**
 * @brief Build a frame for transmission
 * 
 * @param info Frame information
 * @param frame Output frame buffer
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_frame_build(const ieee802154_frame_info_t *info,
                                           ieee802154_frame_t *frame);

/**
 * @brief Calculate FCS (Frame Check Sequence)
 * 
 * @param data Frame data
 * @param length Data length
 * @return 16-bit FCS value
 */
uint16_t ieee802154_frame_calc_fcs(const uint8_t *data, uint8_t length);

/**
 * @brief Verify FCS
 * 
 * @param frame Frame to verify
 * @return true if FCS is valid, false otherwise
 */
bool ieee802154_frame_verify_fcs(const ieee802154_frame_t *frame);

/**
 * @brief Get frame type string
 * 
 * @param type Frame type
 * @return String representation
 */
const char *ieee802154_frame_type_str(ieee802154_frame_type_t type);

/**
 * @brief Check if frame requires ACK
 * 
 * @param info Parsed frame info
 * @return true if ACK is required, false otherwise
 */
bool ieee802154_frame_needs_ack(const ieee802154_frame_info_t *info);

/**
 * @brief Build ACK frame
 * 
 * @param seq_num Sequence number to acknowledge
 * @param frame_pending Frame pending bit
 * @param frame Output ACK frame
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_frame_build_ack(uint8_t seq_num,
                                               bool frame_pending,
                                               ieee802154_frame_t *frame);

/**
 * @brief Check if frame passes address filter
 * 
 * @param info Parsed frame info
 * @param pan_id Our PAN ID
 * @param short_addr Our short address
 * @param ext_addr Our extended address
 * @param promiscuous Promiscuous mode enabled
 * @return true if frame should be accepted, false otherwise
 */
bool ieee802154_frame_filter(const ieee802154_frame_info_t *info,
                              uint16_t pan_id,
                              uint16_t short_addr,
                              const ieee802154_ext_addr_t *ext_addr,
                              bool promiscuous);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __IEEE802154_FRAME_H__ */
