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

#ifndef __HAL_ISP_H__
#define __HAL_ISP_H__

#include "hal_common.h"
#include "board_config.h"

#define ISP_MAX_IN_WIDTH   (1280)
#define ISP_MAX_IN_HEIGHT  (720)

#define ISP_MAX_OUT_WIDTH  (1920)
#define ISP_MAX_OUT_HEIGHT (1080)

#define ISP_MAX_BUFFER_NUM (2)

/*============================ TYPES ======================================*/

/**
 * @brief ISP channel index
 */
enum {
    ISP_CHANNEL_0,
    ISP_CHANNEL_1,
    ISP_CHANNEL_EXT,
    ISP_CHANNEL_MAX_NUM,
};

/**
 * @brief ISP image source
 */
typedef enum {
    ISP_HAL_SRC_SPI2DVP = 0,      /*!< ISP from SPI2DVP, directly mapping to hw value */
    ISP_HAL_SRC_DVP_AS  = 1,      /*!< ISP from DVP_AS, directly mapping to hw value */
    ISP_HAL_SRC_TSRC    = 9,      /*!< ISP from TSRC, directly mapping to hw value */
}isp_hal_src_e;

/**
 * @brief ISP output channel 0 supported format
 */
typedef enum {
    ISP_HAL_OUT_FMT0_YUYV,        /*!< YUYV format for scaler output 0 */
    ISP_HAL_OUT_FMT0_YUV422SP,    /*!< YUV422 semi-planar format for scaler output 0 */
    ISP_HAL_OUT_FMT0_NV12,        /*!< NV12 format for scaler output 0 */
    ISP_HAL_OUT_FMT0_Y_ONLY,      /*!< Y only format for scaler output 0 */
}isp_hal_out_format0_e;

/**
 * @brief ISP output channel 1 supported format
 */
typedef enum {
    ISP_HAL_OUT_FMT1_RGB,         /*!< RGB format for scaler output 1 */
    ISP_HAL_OUT_FMT1_ARGB,        /*!< ARGB format for scaler output 1 */
}isp_hal_out_format1_e;

/**
 * @brief ISP output channel extend supported source
 */
typedef enum {
    ISP_HAL_EXT_SRC_RAW,          /*!< extend channel output source from raw image */
    ISP_HAL_EXT_SRC_ISP_OUT,      /*!< extend channel output source from isp output */
    ISP_HAL_EXT_SRC_OUT_CHN0_IN,  /*!< extend channel output source from scaler out channel 0, make sure channel 0 enable */
    ISP_HAL_EXT_SRC_OUT_CHN1_IN,  /*!< extend channel output source from scaler out channel 1, make sure channel 1 enable */
}isp_hal_ext_src_e;

/**
 * @brief ISP output channel extend supported format
 */
typedef enum {
    ISP_HAL_OUT_FMT_EXT_YUYV,     /*!< YUYV format for extend output, avaliable for all extend src */
    ISP_HAL_OUT_FMT_EXT_Y_ONLY,   /*!< Y only format for extend output, avaliable for all extend src */
    ISP_HAL_OUT_FMT_EXT_RGB,      /*!< RGB only format for extend output avaliable when extend src is ISP_HAL_EXT_SRC_OUT_CHN1_IN */
    ISP_HAL_OUT_FMT_EXT_ARGB,     /*!< ARGB only format for extend output avaliable when extend src is ISP_HAL_EXT_SRC_OUT_CHN1_IN */
}isp_hal_out_format_ext_e;

/**
 * @brief ISP callback event
 */
enum {
    ISP_HAL_INT_NORMAL,           /*!< Normal write interrupt */
    ISP_HAL_INT_FIFO_OVERWRITE,   /*!< Fifo overwrite interrupt */
    ISP_HAL_INT_VSYNC_CNT_ERROR,  /*!< Vsync valid line count non-match interrupt */
    ISP_HAL_INT_HSYNC_CNT_ERROR,  /*!< Hsync valid pixel count non-match interrupt */
    ISP_HAL_INT_ALL,              /*!< All of interrupt */
};

/**
 *  @brief SPI2DVP port select
 */
typedef enum {
    ISP_HAL_SPI2DVP_SEL_SPI_0, /*!< Use SPI_0 */
    ISP_HAL_SPI2DVP_SEL_SPI_1, /*!< Use SPI_1 */
}isp_hal_spi2dvp_sel_e;

/**
 *  @brief SPI2DVP data order
 */
typedef enum {
    ISP_HAL_SPI2DVP_ORDER_UV_FIRST, /*!< Input first data UV */
    ISP_HAL_SPI2DVP_ORDER_Y_FIRST,  /*!< Input first data Y */
} isp_hal_spi2dvp_data_order_e;

/**
 *  @brief SPI2DVP input format
 */
typedef enum {
    ISP_HAL_SPI2DVP_FORMAT_YUV422, /*!< Input format YUYV */
    ISP_HAL_SPI2DVP_FORMAT_Y_ONLY, /*!< Input format Y-only */
} isp_hal_spi2dvp_data_format_e;

/**
 *  @brief SPI2DVP mode (aka SPI mode)
 */
typedef enum {
    ISP_HAL_SPI2DVP_MODE_0, /*!< SPI mode 0, phase = 0, polarity = 0  */
    ISP_HAL_SPI2DVP_MODE_1, /*!< SPI mode 1, phase = 1, polarity = 0  */
    ISP_HAL_SPI2DVP_MODE_2, /*!< SPI mode 2, phase = 0, polarity = 1  */
    ISP_HAL_SPI2DVP_MODE_3, /*!< SPI mode 3, phase = 1, polarity = 1  */
} isp_hal_spi2dvp_mode_e;

/**
 *  @brief SPI2DVP endian
 */
typedef enum {
    ISP_HAL_SPI2DVP_ENDIAN_LSB, /*!< Input endian (LSB) */
    ISP_HAL_SPI2DVP_ENDIAN_MSB, /*!< Input endian (MSB) */
} isp_hal_spi2dvp_data_endian_e;

/**
 *  @brief SPI2DVP configuration strcut definition
 */
typedef struct {
    isp_hal_spi2dvp_data_order_e  order;   /*!< SPI order, Y first or UV first */
    isp_hal_spi2dvp_data_format_e format;  /*!< SPI format, YUYV or Y only */
    isp_hal_spi2dvp_mode_e        mode;    /*!< SPI mode, reference SPI phase and polarity */
    isp_hal_spi2dvp_data_endian_e endian;  /*!< SPI endian, LSB or MSB */
    isp_hal_spi2dvp_sel_e         spi_sel; /*!< SPI select, SPI_0 or SPI_1 */
}isp_hal_spi2dvp_input_t;

/**
 *  @brief Input V/H sync active level type definition
 */
typedef enum {
    ISP_HAL_DVP_ACTIVE_HIGH, /*!< Input is active-high */
    ISP_HAL_DVP_ACTIVE_LOW,  /*!< Input is active-low */
}isp_hal_dvp_active_type_e;

/**
 *  @brief Input data order type definition
 */
typedef enum {
    ISP_HAL_DATA_BYTE_LOWER,  /*!< The first data is lower-byte */
    ISP_HAL_DATA_BYTE_HIGHER, /*!< The first data is higher-byte */
}isp_hal_dvp_data_byte_t;

/**
 * @brief DVP input configuration
 */
typedef struct {
    isp_hal_dvp_active_type_e hsync;        /*!< DVP HSync active level */
    isp_hal_dvp_active_type_e vsync;        /*!< DVP VSync active level */
    isp_hal_dvp_data_byte_t   order;        /*!< DVP data order, lower byte first or higher first */
    uint32_t                  pclk;         /*!< Sensor pixel clock */
    uint16_t                  totalHPixels; /*!< Sensor width with hblank, pixel base */
}isp_hal_dvp_input_t;

/**
 * @brief TSRC input configuration
 */
typedef struct {
    uint32_t input_addr; /*!< isp input address when source from tsrc */
    bool     tsrc_conti; /*!< tsrc continue read without trigger, avaiable when source from tsrc */
}isp_hal_tsrc_input_t;

/**
 * @brief ISP input source configuration
 */
typedef union {
    isp_hal_spi2dvp_input_t spi2dvp_input; /*!< isp config when input from spi2dvp */
    isp_hal_dvp_input_t     dvp_input;     /*!< isp config when input from dvp */
    isp_hal_tsrc_input_t    tsrc_input;    /*!< isp config when input from tsrc */
}isp_hal_src_cfg_t;

/**
 * @brief image size
 */
typedef struct {
    uint16_t width;  /*!< image width */
    uint16_t height; /*!< image height */
}isp_hal_size_t;

/**
 * @brief image crop size, include start x/y
 */
typedef struct {
    uint16_t start_x; /*!< image crop start point X */
    uint16_t start_y; /*!< image crop start point Y */
    uint16_t width;   /*!< image width */
    uint16_t height;  /*!< image height */
}isp_hal_crop_t;

/**
 * @brief ISP output channel in/out size
 */
typedef struct {
    isp_hal_crop_t in;  /*!< scaler input config for digital zoom */
    isp_hal_size_t out; /*!< scaler output config */
}isp_hal_scaler_cfg_t;

/**
 * @brief ISP output channel frame rate config
 */
typedef struct {
    uint8_t src_fps; /*!< source frame rate of output channel. value should be larger than 0 (src_fps > 1) */
    uint8_t dst_fps; /*!< destination frame rate of output channel. 0 means always drop frame */
}isp_hal_frame_rate_t;

/**
 * @brief ISP callback frame info
 */
typedef struct {
    uint32_t event;    /*!< report event, see ISP_HAL_INT_XXX */
    uint32_t out_port; /*!< report port, 0 for channel 0, 1 for channel 1  */
    uint32_t out_addr; /*!< report address, indicate current output address */
    uint32_t out_size; /*!< report size, indicate current output size, byte unit */
}isp_out_frame_info_t;

/**
 * @brief ISP output channel common usage
 */
typedef struct {
    uint8_t               pp_num;                      /*!< ping pong number, value 1 ~ ISP_MAX_BUFFER_NUM, 0 would be same 1 */
    uint32_t              addr[ISP_MAX_BUFFER_NUM];    /*!< output address, always start from addr[0] */
    isp_hal_frame_rate_t  fr_ctrl;                     /*!< channel frame rate control, src_fps/dst_fps = 30/15, means output 15 fps */
    isp_hal_crop_t        out_crop;                    /*!< channel final output crop size */
}isp_hal_out_chn_comm_t;

/**
 * @brief ISP output channel 0 config
 */
typedef struct {
    uint8_t                enable;                     /*!< channel output enable */
    isp_hal_scaler_cfg_t   scaler_cfg;                 /*!< scaler in/out config */
    isp_hal_out_format0_e  format;                     /*!< format for output 0 */
    uint16_t               ring_line;                  /*!< ring line for output 0, it may handshake with mjpg, when > 0, force pp_num = 1 */
    isp_hal_out_chn_comm_t chn_common;                 /*!< output channel common configuration */
}isp_hal_out_chnl_0_t;

/**
 * @brief ISP output channel 1 config
 */
typedef struct {
    uint8_t                enable;                     /*!< channel output enable */
    isp_hal_scaler_cfg_t   scaler_cfg;                 /*!< scaler in/out config */
    isp_hal_out_format1_e  format;                     /*!< format for output 1 */
    isp_hal_out_chn_comm_t chn_common;                 /*!< output channel common configuration */
}isp_hal_out_chnl_1_t;

/**
 * @brief ISP output channel extend config
 */
typedef struct {
    uint8_t                  enable;                   /*!< channel output enable */
    isp_hal_ext_src_e        ext_src;                  /*!< extend channel output source */
    isp_hal_out_format_ext_e format;                   /*!< format for output extend */
    isp_hal_out_chn_comm_t   chn_common;               /*!< output channel common configuration */
}isp_hal_out_chnl_ext_t;

/**
 * @brief ISP full config
 */
typedef struct {
    isp_hal_src_e          isp_src;                    /*!< isp source select */
    isp_hal_size_t         in_size;                    /*!< isp input size */
    isp_hal_src_cfg_t      src_cfg;                    /*!< isp source configuration */
    isp_hal_crop_t         glb_crop;                   /*!< isp output global crop before scaler */
    uint8_t                yuv_adj_en;                 /*!< yuv adjust functino enable */
    isp_hal_out_chnl_0_t   out_ch_0;                   /*!< isp output channel 0, avaliable ocram and psram */
    isp_hal_out_chnl_1_t   out_ch_1;                   /*!< isp output channel 1, avaliable psram only */
    isp_hal_out_chnl_ext_t out_ch_ext;                 /*!< isp output channel extend, avaliable ocram and psram and ch0 format should not be 422SP and NV12 */
    void (*out_callback)(isp_out_frame_info_t info);   /*!< callback for output channel */
}isp_hal_cfg_t;

/*============================ API ========================================*/

/**
 * @brief Initialize isp module
 * @param isp_hal_cfg isp configuration
 * @return 0 on success, negative on error
 */
int isp_init(isp_hal_cfg_t *isp_hal_cfg);

/**
 * @brief start isp module and ready to receive image
 * @return 0 on success, negative on error
 */
int isp_start(void);

/**
 * @brief De-initialize isp module
 * @return 0 on success, negative on error
 */
int isp_deinit(void);

/**
 * @brief Trigger isp_tsrc to send single image. available when isp source from ISP_HAL_SRC_TSRC with tsrc_conti = 0
 * @return 0 on success, negative on error
 */
int isp_tsrc_trigger(void);

/**
 * @brief Run-time update channel output digital zoom
 * @param channel channel id, reference ISP_CHANNEL_XXX
 * @param scaler_in scaler configuration for input crop
 * @return 0 on success, negative on error
 */
int isp_zoom_cfg(uint8_t channel, isp_hal_crop_t scaler_in);

/**
 * @brief Run-time update channel output frame rate, it may do drop when dst_fps = 0
 * @param channel channel id, reference ISP_CHANNEL_XXX
 * @param frc frame rate configuration
 * @return 0 on success, negative on error
 */
int isp_fps_cfg(uint8_t channel, isp_hal_frame_rate_t frc);

/**
 * @brief Run-time update channel 0 output configuration. Call this API after SoF for safety.
 * @param chn0_cfg channel 0 configuration
 * @return 0 on success, negative on error
 */
int isp_update_chn0_cfg(isp_hal_out_chnl_0_t chn0_cfg);

/**
 * @brief Run-time update channel 1 output configuration. Call this API after SoF for safety.
 * @param chn1_cfg channel 1 configuration
 * @return 0 on success, negative on error
 */
int isp_update_chn1_cfg(isp_hal_out_chnl_1_t chn1_cfg);
void isp_get_input_resolution(uint16_t *width, uint16_t *height);

#endif
