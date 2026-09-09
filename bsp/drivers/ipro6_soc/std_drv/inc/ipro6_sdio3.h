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

#ifndef __IPRO6_SDIO3_H__
#define __IPRO6_SDIO3_H__

/*
 * SMID (SDIO 3.0 / SD3.0 / eMMC device controller) software driver header
 * for IPRO6.
 *
 * IP block: SMID, per the SMID user guide rev 3.58. This header holds the
 * configuration / data-structure / enum / software-protocol definitions; the
 * pure hardware register map lives in regs/ipro6_sdio3_reg.h.
 *
 * SOFTWARE-PROTOCOL WARNING:
 *   The IPRO6_SDIO3_CUSTOM_REG_* layout and the per-element queue encodings are
 *   a HOST<->DEVICE FIRMWARE CONTRACT. The byte offsets below are reconstructed
 *   from driver usage and are self-consistent, but the EXACT values MUST be
 *   locked against the real SDIO host driver / firmware image before P3
 *   (on-silicon data path). Do NOT treat these as datasheet-fixed.
 */

#include <stdint.h>
#include "drv_common.h"                  /* IRQn_Type, getreg/putreg, ATTR_NOCACHE_RAM_SECTION */
#include "ipro6_sdio3_reg.h"

/* ========================================================================= */
/* Build toggles                                                             */
/* ========================================================================= */

/* IPRO6_SDIO3_DEBUG : leave undefined for production. When defined, the .c
 * emits printf trace via IPRO6_SDIO3_DRV_DBG(). */
/* #define IPRO6_SDIO3_DEBUG */

/* IPRO6_SDIO3_DMA1_MODE_ENABLE : 0 = use ADMA2 (default), 1 = legacy DMA1 page
 * mode. Keep 0 on IPRO6 (ADMA single-shot is the validated path). */
#ifndef IPRO6_SDIO3_DMA1_MODE_ENABLE
#define IPRO6_SDIO3_DMA1_MODE_ENABLE 0
#endif

/* IPRO6_SDIO3_DMA1_PAGA_SIZE : DMA1 page granularity, only consulted when
 * IPRO6_SDIO3_DMA1_MODE_ENABLE == 1. Power-of-two multiple of 4096. */
#ifndef IPRO6_SDIO3_DMA1_PAGA_SIZE
#define IPRO6_SDIO3_DMA1_PAGA_SIZE (4096)
#endif

/* ========================================================================= */
/* Sizing / topology                                                         */
/* ========================================================================= */

/* Number of SDIO I/O functions this device exposes. 1 = WiFi-only. */
#define IPRO6_SDIO3_FUNC_NUM_MAX        (1)

/* Per-function transfer-queue depth. MUST be a power of two and <= 16. */
#define IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX  (8)

/* ========================================================================= */
/* Device handle                                                             */
/* ========================================================================= */

/* Minimal device handle. The .c only reads dev->reg_base and dev->irq_num. */
typedef struct {
    uint32_t  reg_base;     /* SMID CSR base (== SMID_BASE / SDU_BASE) */
    IRQn_Type irq_num;      /* SDU/SMID ECLIC interrupt number */
} ipro6_smid_dev_t;

/* ========================================================================= */
/* Capability flags (cfg->cap_flag bitmask, software-side selectors)         */
/* ========================================================================= */
#define IPRO6_SDIO3_CAP_FLAG_SAI        (1u << 0)   /* async interrupt    -> CCCR SAI    */
#define IPRO6_SDIO3_CAP_FLAG_DDR50      (1u << 1)   /* DDR50              -> CCCR DDR50  */
#define IPRO6_SDIO3_CAP_FLAG_SDR104     (1u << 2)   /* SDR104             -> CCCR SDR104 */
#define IPRO6_SDIO3_CAP_FLAG_SDR50      (1u << 3)   /* SDR50              -> CCCR SDR50  */
#define IPRO6_SDIO3_CAP_FLAG_SIG1V8     (1u << 4)   /* 1.8V signalling    -> OCR  switch */
#define IPRO6_SDIO3_CAP_FLAG_RD_WAIT    (1u << 5)   /* read-wait          -> CCCR SRW    */
#define IPRO6_SDIO3_CAP_FLAG_GAP_INT    (1u << 6)   /* block-gap interrupt-> CCCR S4MI   */

/* ========================================================================= */
/* Status flags (per-function STA_FLAG byte; HOST-FIRMWARE CONTRACT)         */
/* ========================================================================= */
#define IPRO6_SDIO3_STA_FLAG_APP_RUN             (1u << 0)  /* app firmware running     */
#define IPRO6_SDIO3_STA_FLAG_SDIO_BOOT           (1u << 1)  /* bootrom SDIO-boot mode   */
#define IPRO6_SDIO3_STA_FLAG_RD_LEN_COMPRESS_SUP (1u << 2)  /* read-len compress support*/

/* ========================================================================= */
/* Transfer-state machine (sdio3_ctrl.tran_sta)                              */
/* ========================================================================= */
#define IPRO6_SDIO3_TRAN_STA_IDLE       (0)
#define IPRO6_SDIO3_TRAN_STA_READY      (1)
#define IPRO6_SDIO3_TRAN_STA_BUSY_DNLD  (2)
#define IPRO6_SDIO3_TRAN_STA_BUSY_UPLD  (3)

/* ========================================================================= */
/* feature_control() command codes (arg = function number for per-func cmds) */
/* ========================================================================= */
#define IPRO6_SDIO3_CMD_INIT_READY               (0)
#define IPRO6_SDIO3_CMD_GET_CARD_STA             (1)
#define IPRO6_SDIO3_CMD_GET_BUS_WIDTH            (2)
#define IPRO6_SDIO3_CMD_GET_FUNC_BLK_SIZE        (3)
#define IPRO6_SDIO3_CMD_GET_FUNC_HOST_READY      (4)
#define IPRO6_SDIO3_CMD_SET_FUNC_CARD_READY      (5)
#define IPRO6_SDIO3_CMD_GET_DNLD_MAX_SIZE        (6)
#define IPRO6_SDIO3_CMD_GET_UPLD_MAX_SIZE        (7)
#define IPRO6_SDIO3_CMD_GET_DNLD_QUEUE_AVAILABLE (8)
#define IPRO6_SDIO3_CMD_GET_UPLD_QUEUE_AVAILABLE (9)
#define IPRO6_SDIO3_CMD_GET_DNLD_QUEUE_WAIT_NUM  (10)
#define IPRO6_SDIO3_CMD_GET_UPLD_QUEUE_WAIT_NUM  (11)

/* ========================================================================= */
/* IRQ event codes (passed as `event` to the user callback)                   */
/* ========================================================================= */
#define IPRO6_SDIO3_IRQ_EVENT_DNLD_CPL          (0)  /* download complete;  data = &trans_desc */
#define IPRO6_SDIO3_IRQ_EVENT_UPLD_CPL          (1)  /* upload   complete;  data = &trans_desc */
#define IPRO6_SDIO3_IRQ_EVENT_SOFT_RESET        (2)  /* CMD0/CMD52 soft reset; data = NULL */
#define IPRO6_SDIO3_IRQ_EVENT_FUNC1_RESET       (3)  /* Function1 reset; data = NULL */
#define IPRO6_SDIO3_IRQ_EVENT_FUNC2_RESET       (4)  /* Function2 reset; data = NULL */
#define IPRO6_SDIO3_IRQ_EVENT_SIG1V8_SWITCH     (5)  /* CMD11 1.8V clk-stop; data = NULL */
#define IPRO6_SDIO3_IRQ_EVENT_ERR_CRC           (6)  /* CRC/end-bit error; data = &trans_desc */
#define IPRO6_SDIO3_IRQ_EVENT_ERR_ADMA          (7)  /* ADMA error; data = NULL */
#define IPRO6_SDIO3_IRQ_EVENT_ERR_DNLD_ABORT    (8)  /* dnld aborted; data = &trans_desc */
#define IPRO6_SDIO3_IRQ_EVENT_ERR_UPLD_ABORT    (9)  /* upld aborted; data = &trans_desc */
#define IPRO6_SDIO3_IRQ_EVENT_ERR_DNLD_QUEUE    (10) /* write-start, dnld queue empty; data = NULL */
#define IPRO6_SDIO3_IRQ_EVENT_ERR_UPLD_QUEUE    (11) /* read-start, upld queue empty; data = NULL */
#define IPRO6_SDIO3_IRQ_EVENT_ERR_UNKNOW        (12) /* unexpected/unhandled int; data = NULL */

/* ========================================================================= */
/* Custom-register software-protocol layout  (HOST-FIRMWARE CONTRACT)         */
/* ========================================================================= */
/*
 * The SMID exposes an indirect-access RAM window (addr @0x48, data @0x4C)
 * shared with the SDIO host. The driver lays out one fixed-size block per
 * function. LOCK ALL VALUES BELOW against the real host firmware before P3.
 *
 * Per-function block layout (byte offsets within a function block):
 *   0x00 BLOCK_SIZE u16 | 0x02 CARD_READY u8 | 0x03 HOST_READY u8
 *   0x04 DNLD_MAX_SIZE u16 | 0x06 UPLD_MAX_SIZE u16 | 0x08 QUEUE_MAX_DEPTH u8
 *   0x09 STA_FLAG u8 | 0x0A DNLD_QUEUE_IN u8 | 0x0B DNLD_QUEUE_OUT u8
 *   0x0C UPLD_QUEUE_IN u8 | 0x0D UPLD_QUEUE_OUT u8
 *   0x10 DNLD_QUEUE u16[QUEUE_NUM_MAX] | then UPLD_QUEUE u16[QUEUE_NUM_MAX]
 */

/* Stride between consecutive function blocks (>= 0x10 + 2*(QUEUE_NUM_MAX*2)). */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_STRIDE      (0x40)
/* Block 0 reserved/global; function f (1-based) block base = f * STRIDE. */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(f)   ((uint16_t)((f) * IPRO6_SDIO3_CUSTOM_REG_FUNC_STRIDE))

#define IPRO6_SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE      (0x00)  /* u16 */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_CARD_READY      (0x02)  /* u8  */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_HOST_READY      (0x03)  /* u8  */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE   (0x04)  /* u16 */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_MAX_SIZE   (0x06)  /* u16 */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH (0x08)  /* u8  */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_STA_FLAG        (0x09)  /* u8  */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN   (0x0A)  /* u8  */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT  (0x0B)  /* u8  */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN   (0x0C)  /* u8  */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT  (0x0D)  /* u8  */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE      (0x10)  /* u16[QUEUE_NUM_MAX] */
#define IPRO6_SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE      \
    (IPRO6_SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE + IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX * 2)

/* Total custom-register window size (bounds-check in custom_reg_read/write). */
#define IPRO6_SDIO3_CUSTOM_REG_SIZE \
    (IPRO6_SDIO3_CUSTOM_REG_FUNC_OFFSET(IPRO6_SDIO3_FUNC_NUM_MAX) + IPRO6_SDIO3_CUSTOM_REG_FUNC_STRIDE)

/* ========================================================================= */
/* Data structures                                                           */
/* ========================================================================= */

/* SDIO Standard Function Interface Code (FBRx Control reg [3:0]); the host reads
 * it as the function's class. The SDIO CIS vendor/device is silicon-baked and not
 * settable here, so the std-IF/class is the only identity the device can set. */
#define IPRO6_SDIO3_STD_IF_NONE         (0x00)
#define IPRO6_SDIO3_STD_IF_UART         (0x01)
#define IPRO6_SDIO3_STD_IF_WLAN         (0x07)  /* SDIO WLAN interface */

/* Driver configuration, passed to ipro6_sdio3_init(). */
typedef struct {
    uint8_t  func_num;              /* functions to enable (1..FUNC_NUM_MAX) */
    uint8_t  func1_if_code;         /* FBR1 SDIO std-IF code (IPRO6_SDIO3_STD_IF_*) */
    uint32_t cap_flag;              /* OR of IPRO6_SDIO3_CAP_FLAG_* */
    uint32_t ocr;                   /* OCR voltage window -> Card OCR reg [23:0] */
    uint16_t func1_dnld_size_max;   /* Function1 max download payload */
    uint16_t func2_dnld_size_max;   /* Function2 max download payload */
} ipro6_sdio3_config_s;

/* One queued transfer descriptor. func is 1-based; buff is the DMA target.
 * With IPRO6_SDIO3_TDF_SG set (dnld only), buff points to an
 * ipro6_sdio3_sg_desc_t owned by the caller (alive until the CPL/abort
 * callback) and data_len is the chain's total receive capacity. */
typedef struct {
    uint8_t  func;          /* function number (1-based) */
    uint8_t  flags;         /* IPRO6_SDIO3_TDF_* */
    uint16_t data_len;      /* payload length in bytes */
    void    *buff;          /* data buffer (DMA source/sink) */
} ipro6_sdio3_trans_desc_t;

#define IPRO6_SDIO3_TDF_SG      (1u << 0)   /* buff = ipro6_sdio3_sg_desc_t* */

/* Scatter (ZCEN2-stride) dnld descriptor: the host writes cnt strides of
 * `stride` bytes each in one CMD53; stride k lands its first slot_cap bytes at
 * slot[k] and the remaining stride tail (host zero-pad) is sunk into the SAME
 * slot's headroom at slot[k] - pad_off (rebuilt later by the TX path, so
 * clobbering it mid-transfer is harmless). Requires slot_cap + pad_off ==
 * stride so the pad can never overrun the headroom. */
#define IPRO6_SDIO3_SG_MAX      (4)
typedef struct {
    uint8_t  cnt;           /* chain length (1..IPRO6_SDIO3_SG_MAX) */
    uint8_t  rsvd;
    uint16_t stride;        /* wire bytes per stride */
    uint16_t slot_cap;      /* data bytes stored at slot[k] per stride */
    uint16_t pad_off;       /* pad sink = slot[k] - pad_off */
    void    *slot[IPRO6_SDIO3_SG_MAX];
} ipro6_sdio3_sg_desc_t;

/* ADMA2 hardware descriptor. MUST be exactly 8 bytes (asserted in the .c). */
typedef struct {
    uint16_t attribute;     /* offset 0: ADMA attr (IPRO6_SDIO3_ADMA_ATTR_*) */
    uint16_t length;        /* offset 2: transfer length for this line */
    uint32_t address;       /* offset 4: data address */
} ipro6_sdio3_adma_hw_desc_s;

/* IRQ event callback: arg = registered opaque, event = IPRO6_SDIO3_IRQ_EVENT_*,
 * data = ipro6_sdio3_trans_desc_t* for the CPL / ABORT / ERR_CRC events,
 * NULL otherwise. */
typedef void (*ipro6_sdio3_irq_cb_t)(void *arg, int event, void *data);

/* NOTE: struct ipro6_sdio3_queue_ctrl_s stays defined in the .c (private). */

/* ========================================================================= */
/* Public API                                                                */
/* ========================================================================= */

int ipro6_sdio3_init(ipro6_smid_dev_t *dev, ipro6_sdio3_config_s *cfg);
int ipro6_sdio3_deinit(ipro6_smid_dev_t *dev);

/* Re-assert Card-Init-Done + per-function IOREADY if a host IO-reset cleared them.
 * ipro6s raises no interrupt for that reset, so callers must poll this to keep the
 * device enumerable across the host's re-init attempts. */
void ipro6_sdio3_keep_ready(ipro6_smid_dev_t *dev);

int ipro6_sdio3_custom_reg_write(ipro6_smid_dev_t *dev, uint16_t reg_offset, void *buff, uint16_t len);
int ipro6_sdio3_custom_reg_read(ipro6_smid_dev_t *dev, uint16_t reg_offset, void *buff, uint16_t len);

int ipro6_sdio3_dnld_push(ipro6_smid_dev_t *dev, ipro6_sdio3_trans_desc_t *trans_desc);
int ipro6_sdio3_upld_push(ipro6_smid_dev_t *dev, ipro6_sdio3_trans_desc_t *trans_desc);
int ipro6_sdio3_dnld_pop(ipro6_smid_dev_t *dev, ipro6_sdio3_trans_desc_t *trans_desc, uint8_t func);
int ipro6_sdio3_upld_pop(ipro6_smid_dev_t *dev, ipro6_sdio3_trans_desc_t *trans_desc, uint8_t func);

int ipro6_sdio3_feature_control(ipro6_smid_dev_t *dev, int cmd, uintptr_t arg);

int ipro6_sdio3_irq_attach(ipro6_smid_dev_t *dev, ipro6_sdio3_irq_cb_t irq_event_cb, void *arg);

/* Re-sync the transport to the post-init empty-queue state after a host reset
 * (soft / function reset): stop DMA, clear the upld doorbell, and reset the dnld/
 * upld queue in/out pointers (driver + custom-reg). The reset ISR calls this; the
 * transport layer then re-seeds + re-arms the RX buffer pool (smid_software_reset).
 * Without it a reset left the upld queue desynced -> host reads phantom garbage. */
void ipro6_sdio3_reset_resync(ipro6_smid_dev_t *dev);

/* dnld length-decode diagnostic; read-only, for stats. Counts WRITE_START
 * decodes whose Argument-derived byte count was out of range (clamped). */
extern volatile uint32_t ipro6_sdio3_dnld_len_fallback;
/* dnld flow-control diagnostic; read-only, for stats. Counts how many dnld
 * completions withheld PROGRAM_DONE (held DAT0 busy) to back-pressure the host
 * because the buffer pool was momentarily drained. */
extern volatile uint32_t ipro6_sdio3_dnld_backpressure;
/* dnld flow-control LIVE state; read-only. Nonzero while PROGRAM_DONE is
 * being withheld (DAT0 held busy, host CMD53 blocked) waiting for a buffer
 * to be re-armed. The trcver's stall-bound watchdog polls this to cap how
 * long the host is ever held (a stall past the host's mmc timeout is a fatal
 * EBUSY cascade, bench-proven on bad air). Cleared by ipro6_sdio3_dnld_push. */
extern volatile uint8_t ipro6_sdio3_dnld_pgmdone_deferred;

#endif /* __IPRO6_SDIO3_H__ */
