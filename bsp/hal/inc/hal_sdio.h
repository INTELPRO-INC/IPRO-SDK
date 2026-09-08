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

#ifndef __HAL_SDIO_H
#define __HAL_SDIO_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_sdh.h"

/*
 * SDIO host stack on top of hal_sdh / drv_sdh.
 *
 * Implements the SDIO Simplified Specification (CMD0/3/5/7/52/53) so that an
 * external SDIO device (Wi-Fi/BT/etc.) can be enumerated and accessed over the
 * SDH peripheral. hal_sdh handles the underlying SD command transport; this
 * module layers SDIO IO semantics, CCCR/FBR/CIS parsing, function management
 * and DAT1-line interrupt dispatch on top.
 *
 * Threading: APIs below are synchronous and serialize on a per-host mutex.
 * Callers must NOT hold the mutex across blocking transfers.
 */

/* ------------------------------------------------------------------ */
/* Status codes (MAKE_STATUS group reuses StatusGroup_SDH from hal_sdh) */
/* ------------------------------------------------------------------ */

/** @brief SDIO host status codes. Use Status_Success on OK. */
typedef enum {
    Status_SDIO_Success                 = 0,
    Status_SDIO_NoCard                  = MAKE_STATUS(StatusGroup_SDH, 100U),
    Status_SDIO_NotSDIOCard             = MAKE_STATUS(StatusGroup_SDH, 101U),
    Status_SDIO_FuncNotSupported        = MAKE_STATUS(StatusGroup_SDH, 102U),
    Status_SDIO_FuncEnableFail          = MAKE_STATUS(StatusGroup_SDH, 103U),
    Status_SDIO_BlockSizeReject         = MAKE_STATUS(StatusGroup_SDH, 104U),
    Status_SDIO_BusWidthReject          = MAKE_STATUS(StatusGroup_SDH, 105U),
    Status_SDIO_HighSpeedReject         = MAKE_STATUS(StatusGroup_SDH, 106U),
    Status_SDIO_CmdTimeout              = MAKE_STATUS(StatusGroup_SDH, 107U),
    Status_SDIO_CmdCRCError             = MAKE_STATUS(StatusGroup_SDH, 108U),
    Status_SDIO_DataTimeout             = MAKE_STATUS(StatusGroup_SDH, 109U),
    Status_SDIO_DataCRCError            = MAKE_STATUS(StatusGroup_SDH, 110U),
    Status_SDIO_R5Error                 = MAKE_STATUS(StatusGroup_SDH, 111U),
    Status_SDIO_InvalidArg              = MAKE_STATUS(StatusGroup_SDH, 112U),
    Status_SDIO_BufferAlignment         = MAKE_STATUS(StatusGroup_SDH, 113U),
    Status_SDIO_TupleNotFound           = MAKE_STATUS(StatusGroup_SDH, 114U),
} SDIO_Status_Type;

/* ------------------------------------------------------------------ */
/* SDIO Specification constants                                        */
/* ------------------------------------------------------------------ */

#define SDIO_FUNC_0                     0U      /*!< Common (CCCR) function */
#define SDIO_FUNC_1                     1U
#define SDIO_FUNC_2                     2U
#define SDIO_MAX_FUNCS                  7U      /*!< Max IO functions per card */

/* CCCR register offsets (function 0 address space) */
#define SDIO_CCCR_REVISION              0x00U
#define SDIO_CCCR_SD_SPEC_REVISION      0x01U
#define SDIO_CCCR_IO_ENABLE             0x02U
#define SDIO_CCCR_IO_READY              0x03U
#define SDIO_CCCR_INT_ENABLE            0x04U
#define SDIO_CCCR_INT_PENDING           0x05U
#define SDIO_CCCR_IO_ABORT              0x06U
#define SDIO_CCCR_BUS_INTERFACE_CTRL    0x07U
#define SDIO_CCCR_CARD_CAPABILITY       0x08U
#define SDIO_CCCR_COMMON_CIS_PTR        0x09U   /* 3 bytes LE */
#define SDIO_CCCR_BUS_SUSPEND           0x0CU
#define SDIO_CCCR_FUNC_SELECT           0x0DU
#define SDIO_CCCR_EXEC_FLAGS            0x0EU
#define SDIO_CCCR_READY_FLAGS           0x0FU
#define SDIO_CCCR_FN0_BLOCK_SIZE        0x10U   /* 2 bytes LE */
#define SDIO_CCCR_POWER_CONTROL         0x12U
#define SDIO_CCCR_HIGHSPEED             0x13U

/* Per-function FBR region offset (function N at 0x100 * N) */
#define SDIO_FBR_BASE(func)             (0x100U * (func))
#define SDIO_FBR_CSA_PTR_OFFSET         0x0BU   /* 3 bytes LE */
#define SDIO_FBR_CIS_PTR_OFFSET         0x09U   /* 3 bytes LE */
#define SDIO_FBR_BLOCK_SIZE_OFFSET      0x10U   /* 2 bytes LE */

/* Common CIS tuple codes */
#define SDIO_CISTPL_NULL                0x00U
#define SDIO_CISTPL_VERS_1              0x15U
#define SDIO_CISTPL_MANFID              0x20U
#define SDIO_CISTPL_FUNCID              0x21U
#define SDIO_CISTPL_FUNCE               0x22U
#define SDIO_CISTPL_END                 0xFFU

/* Bus interface control */
#define SDIO_BUS_WIDTH_1BIT             0x00U
#define SDIO_BUS_WIDTH_4BIT             0x02U
#define SDIO_BUS_ECSI_MASK              0x20U   /* enable continuous SPI int */
#define SDIO_BUS_SCSI_MASK              0x40U
#define SDIO_BUS_CD_DISABLE             0x80U   /* pull-up disable on DAT3 */

/* CMD53 argument flags */
#define SDIO_CMD53_RW_FLAG              (1U << 31)
#define SDIO_CMD53_FUNC_SHIFT           28
#define SDIO_CMD53_FUNC_MASK            (0x7U << 28)
#define SDIO_CMD53_BLOCK_MODE           (1U << 27)
#define SDIO_CMD53_OP_INC_ADDR          (1U << 26)
#define SDIO_CMD53_REG_ADDR_SHIFT       9
#define SDIO_CMD53_REG_ADDR_MASK        (0x1FFFFU << 9)
#define SDIO_CMD53_BYTE_BLOCK_COUNT_MAX 0x1FFU  /* 9-bit count field */

/* ------------------------------------------------------------------ */
/* Public types                                                        */
/* ------------------------------------------------------------------ */

/**
 * @brief Per-function metadata populated during enumeration.
 */
typedef struct {
    uint8_t  func_num;          /*!< 1..SDIO_MAX_FUNCS (0 reserved for CCCR) */
    uint8_t  func_class;        /*!< CIS FUNCID class (0x0C = SDIO_CLASS_WLAN) */
    uint8_t  enabled;           /*!< Set after sdio_enable_func() */
    uint16_t cur_blk_size;      /*!< Currently programmed block size */
    uint16_t max_blk_size;      /*!< CIS-reported maximum block size */
    uint32_t cis_ptr;           /*!< Function-specific CIS chain start */
    uint16_t manf_id;           /*!< From CISTPL_MANFID */
    uint16_t prod_id;           /*!< From CISTPL_MANFID */
} SDIO_Func_Info_Type;

/**
 * @brief SDIO interrupt callback. Invoked from SDH ISR context.
 *
 * The host clears the int-pending bit before calling. Callback should
 * defer heavy work to a task; minimal work allowed inline.
 */
typedef void (*SDIO_IRQ_Handler)(uint8_t func_num, void *arg);

/**
 * @brief SDIO host descriptor. One per SDH controller instance.
 *
 * Caller allocates; pass to sdio_host_init() before any other API.
 */
typedef struct {
    SD_ID_Type           sd_id;             /*!< Underlying SDH controller ID */
    uint16_t             rca;               /*!< Relative card address from CMD3 */
    uint8_t              num_funcs;         /*!< Discovered IO function count */
    uint8_t              bus_width;         /*!< SDIO_BUS_WIDTH_{1,4}BIT */
    uint8_t              high_speed;        /*!< 1 if HS enabled */
    uint32_t             ocr;               /*!< Operating Conditions Register */
    uint32_t             common_cis_ptr;    /*!< CIS pointer for function 0 */
    SDIO_Func_Info_Type  funcs[SDIO_MAX_FUNCS + 1];
    SDIO_IRQ_Handler     irq_handlers[SDIO_MAX_FUNCS + 1];
    void                *irq_args[SDIO_MAX_FUNCS + 1];
    void                *mutex;             /*!< Opaque OS mutex handle (recursive) */
    void                *card_int_sem;      /*!< Binary sem given by SDH ISR on DAT1 IRQ */
} SDIO_Host_Type;

/* ------------------------------------------------------------------ */
/* Initialization / enumeration                                        */
/* ------------------------------------------------------------------ */

/**
 * @brief Initialize host descriptor and bind to SDH controller. No card I/O.
 */
SDIO_Status_Type sdio_host_init(SDIO_Host_Type *host, SD_ID_Type sd_id);

/**
 * @brief Power-cycle the SDIO bus (drive CLK and reset state machine).
 *
 * Some devices (incl. AIC8800 series) require external GPIO control of
 * RESET#/POWER_EN pins; this only handles SDH-side reset. Caller must
 * sequence external power separately.
 */
SDIO_Status_Type sdio_host_reset(SDIO_Host_Type *host);

/**
 * @brief Run full SDIO enumeration: CMD0 -> CMD8 -> CMD5 -> CMD3 -> CMD7,
 * read CCCR, parse common CIS, fill host->num_funcs and per-func info.
 *
 * After return, all functions are present-but-disabled. Caller must
 * sdio_set_block_size()/sdio_enable_func() per function.
 */
SDIO_Status_Type sdio_card_enumerate(SDIO_Host_Type *host);

/**
 * @brief Switch bus width. Card must support 4-bit (most do).
 */
SDIO_Status_Type sdio_set_bus_width(SDIO_Host_Type *host, uint8_t width);

/**
 * @brief Enable High Speed (50 MHz) if both host and card support it.
 */
SDIO_Status_Type sdio_set_high_speed(SDIO_Host_Type *host, bool enable);

/**
 * @brief Set SDH transfer clock. Caller responsible for staying within
 * card capability (default-speed: 25 MHz, high-speed: 50 MHz).
 */
SDIO_Status_Type sdio_set_clock(SDIO_Host_Type *host, uint32_t hz);

/**
 * @brief Enable(1)/disable(0) routing small CMD53 transfers through PIO instead of
 * ADMA2 (workaround for tiny ADMA descriptors that fail to engage the data engine at
 * HS/4-bit). Bulk transfers stay on ADMA. Default disabled.
 */
void sdio_set_small_pio(int enable);

/**
 * @brief Select the delayed (io_dly) sampling clock for reads (HOST_CTRL_2 bit7).
 * Required for the GLB smih input-delay taps to affect the read capture point.
 */
void sdio_set_sampling_clk_sel(int enable);

/**
 * @brief Accept READ data when the SDH flags DATA_CRC (false-CRC workaround for the
 * 4-bit extra-nibble data-phase bug). Does not affect data-timeout. Default off.
 */
void sdio_set_ignore_rd_crc(int enable);

/**
 * @brief Route all READ transfers at 1-bit (clean, avoids the 4-bit read false-CRC) while
 * WRITES stay at the configured width. Switch is atomic under the SDIO lock. Default off.
 */
void sdio_set_reads_1bit(int enable);

/**
 * @brief One SDH sampling-phase setting: the full GLB smih_0_io_dly_0 register word
 * (input-delay taps) plus HOST_CTRL_2 bit7 SAMPLING_CLK_SEL.
 */
typedef struct {
    uint32_t io_dly;    /*!< full GLB smih_0_io_dly_0 (GLB_BASE+0x438) register value */
    uint8_t  samp_sel;  /*!< SDH HOST_CTRL_2 bit7 SAMPLING_CLK_SEL */
} SDIO_Phase_Type;

/**
 * @brief Per-transfer-direction sampling phase. Applied inside sdio_cmd53_xfer's mutex,
 * keyed on transfer direction, with a last-applied cache (consecutive same-direction
 * transfers cost one compare, zero register writes). Needed when the read-optimal and
 * write-optimal phases differ (shifted read capture breaks the post-WRITE CRC-status
 * token sampling). Composable with sdio_set_reads_1bit (a 1-bit read still samples at
 * the rd_phase). enable=0 disables; the caller restores its preferred global phase.
 * Default off: zero behavior change.
 */
void sdio_set_dir_phase(const SDIO_Phase_Type *rd_phase,
                        const SDIO_Phase_Type *wr_phase, int enable);

/**
 * @brief Re-run the SDH controller HW bring-up (reset/Ctrl_Init/timeout/HOST_CTRL_2=0/
 * Powon) and clear enumeration state WITHOUT re-creating mutex/semaphore or re-registering
 * the ISR. For repeated re-enumeration (e.g. phase calibration sweeps) — sdio_host_init()
 * leaks RTOS objects if called twice. Host must have been sdio_host_init'ed once.
 */
SDIO_Status_Type sdio_host_reinit(SDIO_Host_Type *host);

/* ------------------------------------------------------------------ */
/* Per-function management                                             */
/* ------------------------------------------------------------------ */

/** @brief Set function block size (CCCR FN0_BLOCK_SIZE or FBR per-func). */
SDIO_Status_Type sdio_set_block_size(SDIO_Host_Type *host,
                                     uint8_t func_num,
                                     uint16_t block_size);

/** @brief Set IO Enable bit, poll IO Ready until function is ready. */
SDIO_Status_Type sdio_enable_func(SDIO_Host_Type *host, uint8_t func_num);

/** @brief Clear IO Enable bit. */
SDIO_Status_Type sdio_disable_func(SDIO_Host_Type *host, uint8_t func_num);

/* ------------------------------------------------------------------ */
/* Host bus lock (recursive)                                           */
/* ------------------------------------------------------------------ */

/**
 * @brief Claim exclusive ownership of the SDIO host for a sequence of
 * back-to-back CMDs (recursive). Use to span multiple sdio_readb/writeb
 * /read_blocks/write_blocks calls without releasing the bus in between
 * — e.g. the ipronet TX BULK loop holds the host across credit-poll
 * CMD52s and the CMD53 data write. Each sdio_claim_host MUST be paired
 * with a sdio_release_host on the same task.
 *
 * Calling these is OPTIONAL: every sdio_readb/writeb/read_blocks/
 * write_blocks already claims/releases internally. Nesting works
 * because the host mutex is recursive.
 */
SDIO_Status_Type sdio_claim_host(SDIO_Host_Type *host);
SDIO_Status_Type sdio_release_host(SDIO_Host_Type *host);

/* ------------------------------------------------------------------ */
/* CMD52: IO_RW_DIRECT — single byte                                   */
/* ------------------------------------------------------------------ */

/** @brief Read one byte from func/addr. */
SDIO_Status_Type sdio_readb(SDIO_Host_Type *host,
                            uint8_t func_num,
                            uint32_t addr,
                            uint8_t *out_val);

/** @brief Write one byte. */
SDIO_Status_Type sdio_writeb(SDIO_Host_Type *host,
                             uint8_t func_num,
                             uint32_t addr,
                             uint8_t val);

/**
 * @brief Read-after-write (RAW): write val, return previous value via out_old.
 * Used for atomic flag toggling on registers like INT_PENDING.
 */
SDIO_Status_Type sdio_writeb_readback(SDIO_Host_Type *host,
                                      uint8_t func_num,
                                      uint32_t addr,
                                      uint8_t val,
                                      uint8_t *out_old);

/* ------------------------------------------------------------------ */
/* CMD53: IO_RW_EXTENDED — multi-byte / block                          */
/* ------------------------------------------------------------------ */

/**
 * @brief Byte-mode CMD53 (count is byte length, max 511 — 0 means 512).
 *
 * fixed_addr: if true, addr is not auto-incremented (FIFO access pattern).
 * Buffer must be 4-byte aligned for ADMA2.
 */
SDIO_Status_Type sdio_read_bytes(SDIO_Host_Type *host,
                                 uint8_t func_num,
                                 uint32_t addr,
                                 uint8_t *buf,
                                 uint16_t count,
                                 bool fixed_addr);

SDIO_Status_Type sdio_write_bytes(SDIO_Host_Type *host,
                                  uint8_t func_num,
                                  uint32_t addr,
                                  const uint8_t *buf,
                                  uint16_t count,
                                  bool fixed_addr);

/**
 * @brief Block-mode CMD53. Function must have block size set first.
 *
 * count is block count (max 511); total bytes = count * blk_size.
 * Buffer must be 4-byte aligned.
 */
SDIO_Status_Type sdio_read_blocks(SDIO_Host_Type *host,
                                  uint8_t func_num,
                                  uint32_t addr,
                                  uint8_t *buf,
                                  uint16_t count,
                                  bool fixed_addr);

SDIO_Status_Type sdio_write_blocks(SDIO_Host_Type *host,
                                   uint8_t func_num,
                                   uint32_t addr,
                                   const uint8_t *buf,
                                   uint16_t count,
                                   bool fixed_addr);

/* ------------------------------------------------------------------ */
/* IRQ                                                                  */
/* ------------------------------------------------------------------ */

/**
 * @brief Register a per-function ISR. Must be called BEFORE
 * sdio_enable_irq() or sdio_enable_func().
 */
SDIO_Status_Type sdio_claim_irq(SDIO_Host_Type *host,
                                uint8_t func_num,
                                SDIO_IRQ_Handler handler,
                                void *arg);

/** @brief Unregister callback. Safe to call from non-ISR context only. */
SDIO_Status_Type sdio_release_irq(SDIO_Host_Type *host, uint8_t func_num);

/**
 * @brief Enable DAT1-line interrupt at the SDH controller and set
 * CCCR INT_ENABLE for registered functions. Call once after all
 * sdio_claim_irq() calls.
 */
SDIO_Status_Type sdio_enable_irq(SDIO_Host_Type *host);

SDIO_Status_Type sdio_disable_irq(SDIO_Host_Type *host);

/* Run `task`'s CMD53 data-phase polls at IDLE+1 and restore `base_prio`
 * afterwards (see SDH_WaitDataHook in hal_sdio.c). NULL disables. */
void sdio_set_wait_demotion(void *task, unsigned base_prio);

/**
 * @brief Poll SDH for SDIO card interrupt and dispatch to registered handlers.
 *
 * Legacy non-blocking dispatcher. Reads SDH_INT_CARD status bit (latched
 * by the ISR via xSemaphoreGiveFromISR on its sem), reads CCCR INT_PENDING
 * over CMD52, and invokes registered per-function handlers. Returns
 * immediately whether or not any work was found.
 *
 * Prefer sdio_wait_irq() for ipronet-style consumers — it blocks until the
 * ISR signals, eliminating the 1 ms polling cadence and its inherent
 * latency floor.
 */
SDIO_Status_Type sdio_poll_irq(SDIO_Host_Type *host);

/**
 * @brief Block on the SDH card-interrupt semaphore (given by the ISR),
 * then read CCCR INT_PENDING over CMD52 and dispatch to registered
 * per-function handlers.
 *
 * @param host     SDIO host descriptor
 * @param ticks    RTOS ticks to wait; portMAX_DELAY = forever
 * @return Status_SDIO_Success if a handler ran (or no handlers registered),
 *         Status_Fail-class on sem-take timeout / CMD52 failure.
 */
SDIO_Status_Type sdio_wait_irq(SDIO_Host_Type *host, uint32_t ticks);

/* ------------------------------------------------------------------ */
/* CIS access (rarely needed once enumeration done)                    */
/* ------------------------------------------------------------------ */

/**
 * @brief Read a CIS tuple chain. Walks tuples starting at start_ptr and
 * invokes cb() per tuple. cb returning non-zero stops iteration.
 *
 * Used during enumeration; exposed for advanced device-specific quirks.
 */
typedef int (*SDIO_CIS_Tuple_CB)(uint8_t code, uint8_t link,
                                 const uint8_t *body, uint8_t body_len,
                                 void *arg);

SDIO_Status_Type sdio_walk_cis(SDIO_Host_Type *host,
                               uint8_t func_num,
                               uint32_t start_ptr,
                               SDIO_CIS_Tuple_CB cb,
                               void *arg);

#endif /* __HAL_SDIO_H */
