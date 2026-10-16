/**
 * @file ieee802154.h
 * @brief IEEE 802.15.4-2013 PHY/MAC Driver API
 * 
 * Unified driver for IEEE 802.15.4 wireless communication
 * Supports OpenThread, Zigbee, and custom applications
 * 
 * @copyright Copyright (c) 2025 INTELPRO Inc.
 */

#ifndef __IEEE802154_H__
#define __IEEE802154_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup IEEE802154_VERSION Driver Version
 * @{
 */
#define IEEE802154_VERSION_MAJOR    1
#define IEEE802154_VERSION_MINOR    0
#define IEEE802154_VERSION_PATCH    0
/** @} */

/**
 * @defgroup IEEE802154_CONSTANTS Protocol Constants
 * @{
 */
#define IEEE802154_MAX_PHY_PACKET_SIZE      127  /**< Maximum PHY payload size */
#define IEEE802154_MIN_PHY_PACKET_SIZE      5    /**< Minimum PHY payload size */
#define IEEE802154_FCS_SIZE                 2    /**< Frame Check Sequence size */
#define IEEE802154_MAX_IE_SIZE              64   /**< Maximum IE size for Enhanced ACK */
#define IEEE802154_MAX_CHANNELS             27   /**< Total number of channels */
#define IEEE802154_CHANNEL_MIN              11   /**< Minimum channel number */
#define IEEE802154_CHANNEL_MAX              26   /**< Maximum channel number */
#define IEEE802154_EXT_ADDR_SIZE            8    /**< Extended address size */
#define IEEE802154_SHORT_ADDR_SIZE          2    /**< Short address size */
#define IEEE802154_PAN_ID_SIZE              2    /**< PAN ID size */
#define IEEE802154_MAX_PHY_PACKET_SIZE_HIGH_RATE 255 /**< PSDU limit at 500K / 1M / 2M */

/**
 * Size of ieee802154_frame_t.psdu. CONFIG_IEEE802154_HIGH_RATE=y makes room
 * for the 255-byte frames of the 500K / 1M / 2M rates (+128 B per frame
 * buffer); without it a longer frame is dropped on RX and refused on TX.
 */
#ifdef CONFIG_IEEE802154_HIGH_RATE
#define IEEE802154_PSDU_BUF_SIZE            IEEE802154_MAX_PHY_PACKET_SIZE_HIGH_RATE
#else
#define IEEE802154_PSDU_BUF_SIZE            IEEE802154_MAX_PHY_PACKET_SIZE
#endif

/** @} */

/**
 * @defgroup IEEE802154_TYPES Data Types
 * @{
 */

/**
 * @brief Error codes
 */
/* Public ABI widths must not depend on the caller's enum compiler flag.
 * These widths preserve the SDK's existing -fshort-enums representation. */
typedef uint8_t ieee802154_error_t;
enum {
    IEEE802154_OK = 0,                /**< Success */
    IEEE802154_ERROR_INVALID_PARAM,   /**< Invalid parameter */
    IEEE802154_ERROR_NOT_INITIALIZED, /**< Driver not initialized */
    IEEE802154_ERROR_ALREADY_INITIALIZED, /**< Driver already initialized */
    IEEE802154_ERROR_BUSY,            /**< Device busy */
    IEEE802154_ERROR_NO_ACK,          /**< No ACK received */
    IEEE802154_ERROR_CHANNEL_BUSY,    /**< Channel busy (CCA failed) */
    IEEE802154_ERROR_TIMEOUT,         /**< Operation timeout */
    IEEE802154_ERROR_HARDWARE,        /**< Hardware error */
    IEEE802154_ERROR_NOT_SUPPORTED,   /**< Feature not supported */
    IEEE802154_ERROR_NO_MEM,          /**< Out of memory */
    IEEE802154_ERROR_SECURITY,        /**< Security/MIC verification failed */
    IEEE802154_ERROR_INVALID_STATE,   /**< Invalid state for operation */
};

/**
 * @brief Radio states
 */
typedef uint8_t ieee802154_state_t;
enum {
    IEEE802154_STATE_DISABLED = 0,    /**< Radio disabled */
    IEEE802154_STATE_SLEEP,           /**< Sleep mode */
    IEEE802154_STATE_IDLE,            /**< Idle, ready to TX/RX */
    IEEE802154_STATE_RX,              /**< Receiving */
    IEEE802154_STATE_TX,              /**< Transmitting */
};

/**
 * @brief TX power levels (dBm)
 */
typedef int8_t ieee802154_tx_power_t;
enum {
    IEEE802154_TX_POWER_N20_DBM = -20,
    IEEE802154_TX_POWER_N10_DBM = -10,
    IEEE802154_TX_POWER_0_DBM = 0,
    IEEE802154_TX_POWER_5_DBM = 5,
    IEEE802154_TX_POWER_10_DBM = 10,
    IEEE802154_TX_POWER_15_DBM = 15,
    IEEE802154_TX_POWER_20_DBM = 20,
};

/**
 * @brief CCA (Clear Channel Assessment) modes
 */
typedef uint8_t ieee802154_cca_mode_t;
enum {
    IEEE802154_CCA_MODE_ENERGY = 1,       /**< Energy above threshold */
    IEEE802154_CCA_MODE_CARRIER = 2,      /**< Carrier sense only */
    IEEE802154_CCA_MODE_CARRIER_OR_ENERGY = 3,  /**< Carrier OR energy */
    IEEE802154_CCA_MODE_CARRIER_AND_ENERGY = 4, /**< Carrier AND energy */
};

/**
 * @brief PHY data rate
 *
 * 250K is the IEEE 802.15.4 2.4 GHz O-QPSK PHY. 500K / 1M / 2M are IPRO
 * proprietary modes: only another IPRO radio receives them. The rate is
 * carried in the SFD, so a receiver with rx_multi_rate on takes all four
 * without being told which one is coming. The values are the M154
 * tx_mode / rx_mode field encoding.
 */
typedef uint8_t ieee802154_data_rate_t;
enum {
    IEEE802154_DATA_RATE_250K = 0,        /**< IEEE 802.15.4 (default) */
    IEEE802154_DATA_RATE_500K = 1,        /**< Proprietary, PSDU up to 255 B */
    IEEE802154_DATA_RATE_1M = 2,          /**< Proprietary, PSDU up to 255 B */
    IEEE802154_DATA_RATE_2M = 3,          /**< Proprietary, PSDU up to 255 B */
};

/**
 * @brief Frame type (802.15.4-2013)
 */
typedef uint8_t ieee802154_frame_type_t;
enum {
    IEEE802154_FRAME_TYPE_BEACON = 0,     /**< Beacon frame */
    IEEE802154_FRAME_TYPE_DATA = 1,       /**< Data frame */
    IEEE802154_FRAME_TYPE_ACK = 2,        /**< Acknowledgment frame */
    IEEE802154_FRAME_TYPE_MAC_CMD = 3,    /**< MAC command frame */
    IEEE802154_FRAME_TYPE_MULTIPURPOSE = 5, /**< Multipurpose frame (2013+) */
};

/**
 * @brief Address mode
 */
typedef uint8_t ieee802154_addr_mode_t;
enum {
    IEEE802154_ADDR_MODE_NONE = 0,        /**< No address */
    IEEE802154_ADDR_MODE_SHORT = 2,       /**< 16-bit short address */
    IEEE802154_ADDR_MODE_EXTENDED = 3,    /**< 64-bit extended address */
};

/**
 * @brief Extended address (64-bit)
 */
typedef union {
    uint8_t m8[8];
    uint16_t m16[4];
    uint32_t m32[2];
    uint64_t m64;
} ieee802154_ext_addr_t;

/**
 * @brief CSL (Coordinated Sampled Listening) configuration (IEEE 802.15.4e)
 */
typedef struct {
    bool enabled;                       /**< CSL enabled */
    uint16_t period;                    /**< CSL period in 10-symbol units (160µs) */
    uint16_t phase;                     /**< CSL phase offset */
    uint8_t accuracy;                   /**< CSL timing accuracy in ±10 µs units */
} ieee802154_csl_config_t;

/**
 * @brief Enhanced ACK configuration (IEEE 802.15.4-2015)
 */
typedef struct {
    bool enabled;                       /**< Enhanced ACK enabled */
    uint8_t ie_data[IEEE802154_MAX_IE_SIZE]; /**< Information Elements data */
    uint8_t ie_len;                     /**< IE data length */
} ieee802154_enh_ack_config_t;

/**
 * @brief Radio capabilities
 */
typedef struct {
    uint8_t hw_aes : 1;             /**< Hardware AES encryption */
    uint8_t auto_ack : 1;           /**< Automatic ACK */
    uint8_t auto_csma : 1;          /**< Automatic CSMA-CA */
    uint8_t frame_filtering : 1;    /**< Hardware frame filtering */
    uint8_t energy_detect : 1;      /**< Energy detection */
    uint8_t rx_on_when_idle : 1;    /**< RX always on */
    uint8_t reserved : 2;
} ieee802154_caps_t;

/**
 * @brief Radio configuration
 */
typedef struct {
    uint8_t channel;                /**< Channel number (11-26) */
    int8_t tx_power;                /**< TX power in dBm */
    uint16_t pan_id;                /**< PAN ID */
    uint16_t short_addr;            /**< Short address */
    ieee802154_ext_addr_t ext_addr; /**< Extended address */
    bool promiscuous;               /**< Promiscuous mode */
    bool auto_ack;                  /**< Enable auto ACK */
    ieee802154_cca_mode_t cca_mode; /**< CCA mode */
    int8_t cca_threshold;           /**< CCA threshold in dBm */
    ieee802154_csl_config_t csl;    /**< CSL configuration */
    ieee802154_enh_ack_config_t enh_ack; /**< Enhanced ACK configuration */
    ieee802154_data_rate_t data_rate; /**< TX data rate (0 = 250K, the standard PHY) */
    bool rx_multi_rate;             /**< RX takes all four rates, PSDU up to 255 B */
} ieee802154_config_t;

/**
 * @brief TX frame metadata
 */
typedef struct {
    uint8_t channel;                /**< TX channel */
    int8_t tx_power;                /**< TX power */
    bool ack_request : 1;           /**< ACK requested */
    bool csma_ca_enabled : 1;       /**< Use CSMA-CA */
    bool frame_pending : 1;         /**< Frame pending bit */
    uint8_t max_csma_backoffs;      /**< Max CSMA backoffs (0..5, also when CSMA is disabled) */
    uint8_t max_frame_retries;      /**< Max frame retries */
    uint32_t timestamp;             /**< TX timestamp (us) */
} ieee802154_tx_info_t;

/**
 * @brief RX frame metadata
 */
typedef struct {
    uint8_t channel;                /**< RX channel */
    int8_t rssi;                    /**< RSSI in dBm */
    uint8_t lqi;                    /**< Link Quality Indicator */
    uint32_t timestamp;             /**< RX timestamp (us) */
    bool ack_request : 1;           /**< ACK was requested */
    bool frame_pending : 1;         /**< Frame pending bit */
    bool security_enabled : 1;      /**< Security enabled */
    bool ack_secured : 1;           /**< answered with a secured Enh-ACK */
    uint8_t ack_key_index;          /**< that Enh-ACK's key index */
    ieee802154_data_rate_t data_rate; /**< Rate the frame arrived at */
    uint32_t ack_frame_counter;     /**< that Enh-ACK's frame counter */
} ieee802154_rx_info_t;

/**
 * @brief TX/RX frame buffer
 *
 * length: on TX the MPDU without FCS (the MAC appends it), on RX the PHR
 * value, FCS included.
 */
typedef struct {
    uint8_t psdu[IEEE802154_PSDU_BUF_SIZE]; /**< PHY payload */
    uint8_t length;                                 /**< Payload length */
    union {
        ieee802154_tx_info_t tx;
        ieee802154_rx_info_t rx;
    } info;
} ieee802154_frame_t;

/**
 * @defgroup IEEE802154_RATE Data rate helpers
 * @{
 */

static inline bool ieee802154_data_rate_valid(ieee802154_data_rate_t rate)
{
    return rate <= IEEE802154_DATA_RATE_2M;
}

/** Air time of one byte at @p rate in microseconds; 0 for an invalid rate. */
static inline uint32_t ieee802154_data_rate_us_per_byte(ieee802154_data_rate_t rate)
{
    return ieee802154_data_rate_valid(rate) ? (32u >> rate) : 0u;
}

/** Largest PSDU (FCS included) the PHY carries at @p rate; 0 for an invalid rate. */
static inline uint16_t ieee802154_data_rate_max_psdu(ieee802154_data_rate_t rate)
{
    if (!ieee802154_data_rate_valid(rate)) {
        return 0;
    }
    return (rate == IEEE802154_DATA_RATE_250K) ? IEEE802154_MAX_PHY_PACKET_SIZE
                                               : IEEE802154_MAX_PHY_PACKET_SIZE_HIGH_RATE;
}

/** Largest TX ieee802154_frame_t.length (MPDU without FCS) at @p rate that
 *  also fits this build's psdu buffer; 0 for an invalid rate. */
static inline uint16_t ieee802154_max_tx_mpdu(ieee802154_data_rate_t rate)
{
    uint16_t psdu = ieee802154_data_rate_max_psdu(rate);

    if (psdu == 0) {
        return 0;
    }
    if (psdu > IEEE802154_PSDU_BUF_SIZE) {
        psdu = IEEE802154_PSDU_BUF_SIZE;
    }
    return (uint16_t)(psdu - IEEE802154_FCS_SIZE);
}

/** Air time from the end of the SFD to the end of the frame (PHR + PSDU). */
static inline uint32_t ieee802154_data_rate_air_us(ieee802154_data_rate_t rate, uint8_t psdu_len)
{
    return ((uint32_t)psdu_len + 1u) * ieee802154_data_rate_us_per_byte(rate);
}

/** "250k", "500k", "1m", "2m", or "?" for an invalid rate. */
static inline const char *ieee802154_data_rate_name(ieee802154_data_rate_t rate)
{
    static const char *const names[] = { "250k", "500k", "1m", "2m" };

    return ieee802154_data_rate_valid(rate) ? names[rate] : "?";
}

/** Parse a rate name (as printed by ieee802154_data_rate_name(), any case).
 *  @p out is written only on success. */
static inline bool ieee802154_data_rate_parse(const char *s, ieee802154_data_rate_t *out)
{
    for (uint8_t r = IEEE802154_DATA_RATE_250K; s && r <= IEEE802154_DATA_RATE_2M; r++) {
        const char *n = ieee802154_data_rate_name(r);
        size_t i = 0;

        while (n[i] != '\0' && (s[i] == n[i] || s[i] == (char)(n[i] - 'a' + 'A'))) {
            i++;
        }
        if (n[i] == '\0' && s[i] == '\0') {
            *out = r;
            return true;
        }
    }
    return false;
}

/** @} */

/**
 * @brief Event types
 */
typedef uint8_t ieee802154_event_type_t;
enum {
    IEEE802154_EVENT_TX_DONE,       /**< TX completed successfully */
    IEEE802154_EVENT_TX_NO_ACK,     /**< TX done but no ACK received */
    IEEE802154_EVENT_TX_CHANNEL_BUSY, /**< TX failed due to CCA */
    IEEE802154_EVENT_RX_DONE,       /**< Frame received */
    IEEE802154_EVENT_RX_FILTERED,   /**< Frame filtered */
    IEEE802154_EVENT_ENERGY_SCAN_DONE, /**< Energy scan completed */
};

/**
 * @brief Event callback function type
 * 
 * @param event Event type
 * @param frame Pointer to frame data (or NULL)
 * @param context User context
 */
typedef void (*ieee802154_event_cb_t)(ieee802154_event_type_t event, 
                                       ieee802154_frame_t *frame, 
                                       void *context);

/** @} */

/**
 * @defgroup IEEE802154_API Driver API
 * @{
 */

/**
 * @brief Initialize IEEE 802.15.4 driver
 * 
 * @param config Initial configuration
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_init(const ieee802154_config_t *config);

/**
 * @brief Deinitialize driver
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_deinit(void);

/**
 * @brief Get driver capabilities
 * 
 * @param caps Pointer to capabilities structure
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_get_caps(ieee802154_caps_t *caps);

/**
 * @brief Set radio channel
 * 
 * @param channel Channel number (11-26)
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_channel(uint8_t channel);

/**
 * @brief Get current channel
 * 
 * @param channel Pointer to store channel number
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_get_channel(uint8_t *channel);

/**
 * @brief Set TX power
 * 
 * @param power TX power in dBm
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_tx_power(int8_t power);

/**
 * @brief Get TX power
 * 
 * @param power Pointer to store TX power
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_get_tx_power(int8_t *power);

/**
 * @brief Set PAN ID
 * 
 * @param pan_id PAN identifier
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_pan_id(uint16_t pan_id);

/**
 * @brief Set short address
 * 
 * @param short_addr 16-bit short address
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_short_addr(uint16_t short_addr);

/**
 * @brief Set extended address
 * 
 * @param ext_addr 64-bit extended address
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_ext_addr(const ieee802154_ext_addr_t *ext_addr);

/**
 * @brief Get extended address
 * 
 * @param ext_addr Pointer to store extended address
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_get_ext_addr(ieee802154_ext_addr_t *ext_addr);

/**
 * @brief Enable/disable promiscuous mode
 * 
 * @param enable true to enable, false to disable
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_promiscuous(bool enable);

/**
 * @brief Enable/disable auto ACK
 * 
 * @param enable true to enable, false to disable
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_auto_ack(bool enable);

/**
 * @brief Set CCA mode and threshold
 * 
 * @param mode CCA mode
 * @param threshold Energy threshold in dBm
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_cca(ieee802154_cca_mode_t mode, int8_t threshold);

/**
 * @brief Set the TX data rate
 *
 * 500K / 1M / 2M are IPRO proprietary: a standard 802.15.4 device (and the
 * Thread network) cannot receive them. Frames longer than 127 bytes also
 * need CONFIG_IEEE802154_HIGH_RATE=y. Transmitting above 250K does not
 * change what the receiver accepts; see ieee802154_set_rx_multi_rate().
 *
 * @param rate IEEE802154_DATA_RATE_*
 * @return IEEE802154_ERROR_INVALID_PARAM for an unknown rate
 */
ieee802154_error_t ieee802154_set_data_rate(ieee802154_data_rate_t rate);

/**
 * @brief Current TX data rate
 */
ieee802154_data_rate_t ieee802154_get_data_rate(void);

/**
 * @brief Receive all four data rates
 *
 * The receiver tells the rate from the SFD and takes PSDUs up to 255 bytes;
 * 250K frames keep arriving as before. The rate of each frame is in
 * ieee802154_frame_t.info.rx.data_rate.
 *
 * @param enable true to accept 500K / 1M / 2M frames as well
 */
ieee802154_error_t ieee802154_set_rx_multi_rate(bool enable);

/**
 * @brief Enable radio and start receiving
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_enable(void);

/**
 * @brief Disable radio
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_disable(void);

/**
 * @brief Enter sleep mode
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_sleep(void);

/**
 * @brief Start receiving on configured channel
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_receive(void);

/**
 * @brief Transmit a frame
 * 
 * @param frame Pointer to frame to transmit
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_transmit(const ieee802154_frame_t *frame);

/**
 * @brief Transmit a frame at base_time_us + delay_us on the MAC symbol-counter
 *        clock (see ieee802154_hal_get_timestamp()); base 0 means "now".
 *        No CSMA is performed for a timed frame.
 */
ieee802154_error_t ieee802154_transmit_delay(const ieee802154_frame_t *frame,
                                             uint64_t base_time_us, uint32_t delay_us);

/**
 * @brief Perform energy detection on channel
 * 
 * @param channel Channel to scan
 * @param duration_ms Scan duration in milliseconds
 * @param energy Pointer to store detected energy level
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_energy_detect(uint8_t channel, uint16_t duration_ms, int8_t *energy);

/**
 * @brief Get current RSSI
 * 
 * @param rssi Pointer to store RSSI value
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_get_rssi(int8_t *rssi);

/**
 * @brief Get current radio state
 * 
 * @param state Pointer to store state
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_get_state(ieee802154_state_t *state);

/**
 * @brief Register event callback
 *
 * @param callback Callback function
 * @param context User context passed to callback
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_register_callback(ieee802154_event_cb_t callback, void *context);

/**
 * @brief Register an additional event observer (fired from ISR before the main callback)
 *
 * A second, optional callback slot used by fast-path TX code (e.g. the SED
 * data-poll fast-path) to capture TX completion + ACK FP results synchronously
 * from the ISR without taking over the main callback that OT (or any other
 * primary client) installs via ieee802154_register_callback().
 *
 * The observer is called BEFORE the primary callback for every event, and is
 * called even when the primary callback is unset. Set to NULL to clear.
 *
 * Designed for short, ISR-safe code paths — must not block.
 *
 * @param observer Observer function, or NULL to clear
 * @param context  Context passed to observer
 * @return IEEE802154_OK / IEEE802154_ERROR_NOT_INITIALIZED
 */
ieee802154_error_t ieee802154_set_tx_observer(ieee802154_event_cb_t observer, void *context);

/**
 * @brief Get high-resolution timestamp
 * 
 * @return Current timestamp in microseconds
 */
uint64_t ieee802154_get_timestamp(void);

/**
 * @brief Set frame pending for address
 * 
 * Used for indirect transmission (coordinator side). When a device with
 * this address sends a data request, the ACK will have frame pending bit set.
 * 
 * @param addr Device address (2 bytes for short, 8 bytes for extended)
 * @param addr_mode Address mode (short or extended)
 * @param pending true if frame is pending for this address
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_frame_pending(const uint8_t *addr,
                                                  ieee802154_addr_mode_t addr_mode,
                                                  bool pending);

/**
 * @brief Clear all frame pending entries
 * 
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_clear_frame_pending(void);

/**
 * @brief Configure CSL (Coordinated Sampled Listening)
 * 
 * Enables low-power listening mode per IEEE 802.15.4e.
 * When enabled, the device wakes up periodically to listen for transmissions.
 * 
 * @param config CSL configuration
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_configure_csl(const ieee802154_csl_config_t *config);

/**
 * @brief Get current CSL configuration
 * 
 * @param config Pointer to store CSL configuration
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_get_csl_config(ieee802154_csl_config_t *config);

/**
 * @brief Configure Enhanced ACK
 * 
 * Enables Enhanced ACK frames that can carry Information Elements (IE)
 * per IEEE 802.15.4-2015. Enhanced ACKs allow piggybacking data in ACK frames.
 * 
 * @param config Enhanced ACK configuration
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_configure_enh_ack(const ieee802154_enh_ack_config_t *config);

/**
 * @brief Set Enhanced ACK IE data for next ACK
 * 
 * Sets Information Elements to be included in the next Enhanced ACK.
 * This allows dynamic IE content per transmission.
 * 
 * @param ie_data IE data buffer
 * @param ie_len IE data length (max IEEE802154_MAX_IE_SIZE)
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_set_ack_ie_data(const uint8_t *ie_data, uint8_t ie_len);

/**
 * @brief Reset IEEE 802.15.4 driver and radio hardware
 *
 * Resets the radio to initial state. Does not change configuration.
 *
 * @return ieee802154_error_t Error code
 */
ieee802154_error_t ieee802154_reset(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __IEEE802154_H__ */
