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

#ifndef __HAL_DMA__H__
#define __HAL_DMA__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"
#include "board_config.h"

enum {
    DMA_CTRL_SET_INT = 0,
    DMA_CTRL_CLEAR_INT,
    DMA_CTRL_CHANNEL_LLI_UPDATE,
    DMA_CTRL_CHANNEL_TRANS_PARAM_UPDATE,
    DMA_CTRL_CHANNEL_GET_STATUS,
    DMA_CTRL_CHANNEL_START,
    DMA_CTRL_CHANNEL_STOP,
    DMA_CTRL_SET_CALLBACK,

};

enum dma_index_type {
    DMA0_CH0_INDEX = 0,
    DMA0_CH1_INDEX,
    DMA0_CH2_INDEX,
    DMA0_CH3_INDEX,
    DMA0_CH4_INDEX,
    DMA0_CH5_INDEX,
    DMA0_CH6_INDEX,
    DMA0_CH7_INDEX,
    DMA_MAX_INDEX
};

enum {
    DMA0_ID = 0,
    DMA_ID_MAX,
};

#define dma_channel_start(id, chn)              dma_ioctl(id, chn, DMA_CTRL_CHANNEL_START, NULL)
#define dma_channel_stop(id, chn)               dma_ioctl(id, chn, DMA_CTRL_CHANNEL_STOP, NULL)
#define dma_channel_lli_update(id, chn, list)   dma_ioctl(id, chn, DMA_CTRL_CHANNEL_LLI_UPDATE, list)
#define dma_channel_check_busy(id, chn)         dma_ioctl(id, chn, DMA_CTRL_CHANNEL_GET_STATUS, NULL)

#define DMA_MAX_TRANSFER_PER_LLI    (4096 - 32)

#define DMA_LLI_ONCE_MODE       0
#define DMA_LLI_CYCLE_MODE      1
#define DMA_LLI_PINGPONG_MODE   2
#define DMA_LLI_CYCLE_N_MODE    3   // N-buffer circular mode (N specified by lli_count)

#define DMA_ADDR_INCREMENT_DISABLE 0 /*!< Addr increment mode disable */
#define DMA_ADDR_INCREMENT_ENABLE  1 /*!< Addr increment mode enable  */

#define DMA_TRANSFER_WIDTH_8BIT  0
#define DMA_TRANSFER_WIDTH_16BIT 1
#define DMA_TRANSFER_WIDTH_32BIT 2
#define DMA_TRANSFER_WIDTH_MAX   3

#define DMA_BURST_INCR1  0
#define DMA_BURST_INCR4  1
#define DMA_BURST_INCR8  2
#define DMA_BURST_INCR16 3

#define DMA_ADDR_UART0_TDR (UART0_BASE + 0x88)
#define DMA_ADDR_UART0_RDR (UART0_BASE + 0x8C)
#define DMA_ADDR_UART1_TDR (UART1_BASE + 0x88)
#define DMA_ADDR_UART1_RDR (UART1_BASE + 0x8C)
#define DMA_ADDR_I2C_TDR   (I2C_BASE + 0x88)
#define DMA_ADDR_I2C_RDR   (I2C_BASE + 0x8C)
#define DMA_ADDR_SPI_TDR   (SPI_BASE + 0x88)
#define DMA_ADDR_SPI_RDR   (SPI_BASE + 0x8C)
#define DMA_ADDR_SPI1_TDR  (SPI1_BASE + 0x88)
#define DMA_ADDR_SPI1_RDR  (SPI1_BASE + 0x8C)
#define DMA_ADDR_I2S_TDR   (I2S_BASE + 0x88)
#define DMA_ADDR_I2S_RDR   (I2S_BASE + 0x8C)
#define DMA_ADDR_ADC_RDR   (GPIP_BASE + 0x04)
#define DMA_ADDR_DAC_TDR   (GPIP_BASE + 0X48)
#define DMA_ADDR_AUD_RDR   (AUD_BASE + 0xC88)
#define DMA_ADDR_DBI_TDR   (DBI_BASE   + 0x88)

#define DMA_REQUEST_NONE     0x00000000 /*!< DMA request peripheral:None */
#define DMA_REQUEST_UART0_RX 0x00000000 /*!< DMA request peripheral:UART0 RX */
#define DMA_REQUEST_UART0_TX 0x00000001 /*!< DMA request peripheral:UART0 TX */
#define DMA_REQUEST_UART1_RX 0x00000002 /*!< DMA request peripheral:UART1 RX */
#define DMA_REQUEST_UART1_TX 0x00000003 /*!< DMA request peripheral:UART1 TX */
#define DMA_REQUEST_I2C0_RX  0x00000004 /*!< DMA request peripheral:I2C RX */
#define DMA_REQUEST_I2C0_TX  0x00000005 /*!< DMA request peripheral:I2C TX */
#define DMA_REQUEST_SPI0_RX  0x00000006 /*!< DMA request peripheral:SPI RX */
#define DMA_REQUEST_SPI0_TX  0x00000007 /*!< DMA request peripheral:SPI TX */
#define DMA_REQUEST_IR_TX    0x00000008 /*!< DMA request peripheral:IR TX */
#define DMA_REQUEST_AUD_ADC  0x00000009 /*!< DMA request peripheral:AUD ADC */

/* IPRO6/IPRO6LE use the BL616-class DMA mux, where I2S is request 16/17.
 * IPRO7 uses request 10/11.  The 20/21 values in the IPRO7 drv_dma.h enum
 * are stale legacy values and do not match the implemented peripheral mux. */
#if defined(CONFIG_IPRO6) || defined(IPRO6) || defined(CONFIG_IPRO6LE) || defined(IPRO6LE)
#define DMA_REQUEST_I2S_RX   0x00000010 /*!< IPRO6 DMA request peripheral:I2S RX */
#define DMA_REQUEST_I2S_TX   0x00000011 /*!< IPRO6 DMA request peripheral:I2S TX */
#else
#define DMA_REQUEST_I2S_RX   0x0000000A /*!< IPRO7 DMA mux: I2S RX */
#define DMA_REQUEST_I2S_TX   0x0000000B /*!< IPRO7 DMA mux: I2S TX */
#endif
#define DMA_REQUEST_ADC0     0x0000000C /*!< DMA request peripheral:ADC0 */
#define DMA_REQUEST_DAC0     0x0000000D /*!< DMA request peripheral:DAC0 */
/* IPRO7 DMA peripheral mux (vault IPRO7/Reference/SoC/IPRO7 DMA Peripheral ID.md):
 * 14 = spi_1_rx, 15 = spi_1_tx. drv_dma.h's SPI_RX=10/SPI_TX=11 enum is
 * stale legacy; the actual IPRO7 mux puts SPI0 at 6/7 and SPI1 at 14/15. */
#define DMA_REQUEST_SPI1_RX  0x0000000E /*!< IPRO7 DMA mux: spi_1_rx */
#define DMA_REQUEST_SPI1_TX  0x0000000F /*!< IPRO7 DMA mux: spi_1_tx */
#define DMA_REQUEST_DBI_TX   0x00000014 /*!< IPRO6 DMA request peripheral:DBI TX*/
#define DMA_REQUEST_USB_EP0  0x00000018 /*!< DMA request peripheral:USB EP0*/
#define DMA_REQUEST_USB_EP1  0x00000019 /*!< DMA request peripheral:USB EP1*/
#define DMA_REQUEST_USB_EP2  0x0000001A /*!< DMA request peripheral:USB EP2*/
#define DMA_REQUEST_USB_EP3  0x0000001B /*!< DMA request peripheral:USB EP3*/
#define DMA_REQUEST_USB_EP4  0x0000001C /*!< DMA request peripheral:USB EP4*/
#define DMA_REQUEST_USB_EP5  0x0000001D /*!< DMA request peripheral:USB EP5*/
#define DMA_REQUEST_USB_EP6  0x0000001E /*!< DMA request peripheral:USB EP6*/
#define DMA_REQUEST_USB_EP7  0x0000001F /*!< DMA request peripheral:USB EP7 */

#define DMA_PARAM_UPDATE_SRC_WIDTH     (1 << 0)
#define DMA_PARAM_UPDATE_SRC_BURST     (1 << 1)
#define DMA_PARAM_UPDATE_DST_WIDTH     (1 << 2)
#define DMA_PARAM_UPDATE_DST_BURST     (1 << 3)
#define DMA_PARAM_UPDATE_SRC_ADDR_INC  (1 << 4)
#define DMA_PARAM_UPDATE_DST_ADDR_INC  (1 << 5)
#define DMA_PARAM_UPDATE_WIDTH         (DMA_PARAM_UPDATE_SRC_WIDTH | DMA_PARAM_UPDATE_DST_WIDTH)
#define DMA_PARAM_UPDATE_BURST         (DMA_PARAM_UPDATE_SRC_BURST | DMA_PARAM_UPDATE_DST_BURST)
#define DMA_PARAM_UPDATE_ALL           (0xFF)


typedef enum {
    DMA_MEMORY_TO_MEMORY = 0, /*!< DMA transfer type:memory to memory */
    DMA_MEMORY_TO_PERIPH,     /*!< DMA transfer type:memory to peripheral */
    DMA_PERIPH_TO_MEMORY,     /*!< DMA transfer type:peripheral to memory */
    DMA_PERIPH_TO_PERIPH,     /*!< DMA transfer type:peripheral to peripheral */
} dma_transfer_dir_type;

enum {
    HAL_DMA_INT_COMPLETE = 0,
    HAL_DMA_INT_ERR,
};

typedef union {
    struct
    {
        uint32_t TransferSize : 12; /* [11: 0],        r/w,        0x0 */
        uint32_t SBSize       : 2;  /* [13:12],        r/w,        0x1 */
        uint32_t dst_min_mode : 1;  /* [   14],        r/w,        0x0 */
        uint32_t DBSize       : 2;  /* [16:15],        r/w,        0x1 */
        uint32_t dst_add_mode : 1;  /* [   17],        r/w,        0x0 */
        uint32_t SWidth       : 2;  /* [19:18],        r/w,        0x2 */
        uint32_t reserved_20  : 1;  /* [   20],       rsvd,        0x0 */
        uint32_t DWidth       : 2;  /* [22:21],        r/w,        0x2 */
        uint32_t fix_cnt      : 2;  /* [24:23],        r/w,        0x0 */
        uint32_t SLargerD     : 1;  /* [   25],        r/w,        0x0 */
        uint32_t SI           : 1;  /* [   26],        r/w,        0x1 */
        uint32_t DI           : 1;  /* [   27],        r/w,        0x1 */
        uint32_t Prot         : 3;  /* [30:28],        r/w,        0x0 */
        uint32_t I            : 1;  /* [   31],        r/w,        0x0 */
    } bits;
    uint32_t WORD;
} dma_control_data_t;

typedef struct
{
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t nextlli;
    dma_control_data_t cfg;
} dma_lli_ctrl_t;

typedef struct
{
    uint8_t map;
    uint8_t src_width;
    uint8_t src_burst;
    uint8_t dst_width;
    uint8_t dst_burst;
    uint8_t src_addr_inc;
    uint8_t dst_addr_inc;
} dma_trans_param_t;

typedef struct dma_device {
    uint8_t id;
    uint8_t ch;
    uint8_t transfer_mode;
    uint8_t direction;
    uint32_t src_req;
    uint32_t dst_req;
    uint8_t src_addr_inc;
    uint8_t dst_addr_inc;
    uint8_t src_burst_size;
    uint8_t dst_burst_size;
    uint8_t src_width;
    uint8_t dst_width;
    uint8_t lli_count;       /* Number of buffers for DMA_LLI_CYCLE_N_MODE (2=pingpong, 3=triple, etc.) */
    dma_lli_ctrl_t *lli_cfg; /* private param*/
    void (*callback)(void *dev, uint32_t event);
} dma_device_t;

dma_device_t* dma_getDevice(int dma_id, int dma_ch);
int dma_ioctl(int dma_id, int dma_ch, int cmd, void *args);
int dma_reload(int dma_id, int dma_ch, uint32_t src_addr, uint32_t dst_addr, uint32_t transfer_size, dma_lli_ctrl_t **lliCfg);
int dma_init(int dma_id, int dma_ch);
void dma_setConfig(dma_device_t *dma_device);
int dma_close(uint32_t dma_id, int dma_ch);

/*
 * One-shot DMA controller bring-up: DMA_Enable, IRQ vector and CPU
 * interrupt enable. Safe to call repeatedly; only the first call has
 * effect. All users (allocator or raw API) share the same controller.
 */
void dma_controller_init(int dma_id);

/*
 * Per-channel attach. Configures the channel-level registers from the
 * device previously installed via dma_setConfig(), enables the
 * per-channel DMA clock (GLB_Set_DMA_CLK), and wires the callback.
 *
 * CRITICAL: unlike the legacy dma_init(), this does NOT call
 * DMA_Disable/Enable on the controller. Initialising channel N therefore
 * does not abort an in-flight transfer on channel M. This is the
 * mechanism that unblocks full-duplex audio.
 *
 * Returns 0 on success.
 */
int dma_channel_attach(int dma_id, int dma_ch);

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
/*
 * Runtime DMA channel allocator — see bsp/Kconfig:DMA_CHANNEL_ALLOCATOR
 * and claude_code_todo/dma_channel_allocator_design.md for rationale.
 */

typedef struct dma_channel_handle *dma_ch_handle_t;

/*
 * Acquire a DMA channel routed to the given peripheral request line.
 *
 * @param req    Peripheral request (DMA_REQUEST_DAC0, DMA_REQUEST_AUD_ADC,
 *               ...). Use DMA_REQUEST_NONE for memory-to-memory.
 * @param dir    DMA_MEMORY_TO_PERIPH, DMA_PERIPH_TO_MEMORY,
 *               DMA_MEMORY_TO_MEMORY.
 * @param owner  Static string tag for logs/debug ("audio_dac",
 *               "auadc_amic", "uart1_tx", ...). MUST outlive the
 *               handle (usually a literal).
 *
 * @return Handle on success. NULL if all channels are in use, or if
 *         `req` is already bound to a live channel by a different
 *         owner.
 *
 * Idempotent within an owner: calling twice from the same owner for
 * the same req returns the same handle (refcounted). The caller must
 * release the same number of times it acquired.
 *
 * Task context only — takes an internal mutex. Do not call from ISR.
 */
dma_ch_handle_t dma_request_channel(uint32_t req, uint8_t dir,
                                    const char *owner);

/*
 * Release a channel. After the final release, the channel is stopped,
 * its LLI chain is freed, and the slot returns to the free pool.
 */
void dma_release_channel(dma_ch_handle_t h);

/*
 * Raw channel index from a handle — for passing through dma_reload,
 * dma_channel_start and other low-level APIs during migration.
 */
int dma_handle_to_ch(dma_ch_handle_t h);

/*
 * Debug: dump the full allocation table to the console. Safe to call
 * from task context.
 */
void dma_dump_allocation(void);

/*
 * Runtime introspection: returns 1 if the slot for (dma_id, dma_ch)
 * is allocator-owned (claimed via dma_request_channel). Raw-API
 * clients can use this to detect conflict with an allocator client.
 */
int dma_channel_is_allocator_owned(int dma_id, int dma_ch);
#endif /* CONFIG_DMA_CHANNEL_ALLOCATOR */

#ifdef __cplusplus
}
#endif

#endif
