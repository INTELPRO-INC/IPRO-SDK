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

#ifndef __HAL_CAM_H__
#define __HAL_CAM_H__

#include "hal_common.h"
#include "board_config.h"

#define CAM_MAX_BUFFER_NUM (2)

/*============================ TYPES ======================================*/

/**
 * @brief CAM image source
 */
typedef enum {
    CAM_HAL_SRC_SPI2DVP = 0,      /*!< CAM from SPI2DVP, directly mapping to hw value */
    CAM_HAL_SRC_DVP_AS  = 1,      /*!< CAM from DVP_AS, directly mapping to hw value */
}cam_hal_src_e;

/**
 * @brief CAM output supported format
 */
typedef enum {
    CAM_HAL_OUT_FMT_YUYV,         /*!< YUYV format */
    CAM_HAL_OUT_FMT_YUV422SP,     /*!< YUV422 semi-planar format */
    CAM_HAL_OUT_FMT_Y_ONLY,       /*!< Y only format */
}cam_hal_out_format_e;

/**
 * @brief CAM callback event
 */
enum {
    CAM_HAL_INT_NORMAL,           /*!< Normal write interrupt */
    CAM_HAL_INT_FIFO_OVERWRITE,   /*!< Fifo overwrite interrupt */
    CAM_HAL_INT_VSYNC_CNT_ERROR,  /*!< Vsync valid line count non-match interrupt */
    CAM_HAL_INT_HSYNC_CNT_ERROR,  /*!< Hsync valid pixel count non-match interrupt */
    CAM_HAL_INT_ALL,              /*!< All of interrupt */
};

/**
 *  @brief SPI2DVP data order
 */
typedef enum {
    CAM_HAL_SPI2DVP_ORDER_UV_FIRST, /*!< Input first data UV */
    CAM_HAL_SPI2DVP_ORDER_Y_FIRST,  /*!< Input first data Y */
} cam_hal_spi2dvp_data_order_e;

/**
 *  @brief SPI2DVP input format
 */
typedef enum {
    CAM_HAL_SPI2DVP_FORMAT_YUV422, /*!< Input format YUYV */
    CAM_HAL_SPI2DVP_FORMAT_Y_ONLY, /*!< Input format Y-only */
} cam_hal_spi2dvp_data_format_e;

/**
 *  @brief SPI2DVP mode (aka SPI mode)
 */
typedef enum {
    CAM_HAL_SPI2DVP_MODE_0, /*!< SPI mode 0, phase = 0, polarity = 0  */
    CAM_HAL_SPI2DVP_MODE_1, /*!< SPI mode 1, phase = 1, polarity = 0  */
    CAM_HAL_SPI2DVP_MODE_2, /*!< SPI mode 2, phase = 0, polarity = 1  */
    CAM_HAL_SPI2DVP_MODE_3, /*!< SPI mode 3, phase = 1, polarity = 1  */
} cam_hal_spi2dvp_mode_e;

/**
 *  @brief SPI2DVP endian
 */
typedef enum {
    CAM_HAL_SPI2DVP_ENDIAN_LSB, /*!< Input endian (LSB) */
    CAM_HAL_SPI2DVP_ENDIAN_MSB, /*!< Input endian (MSB) */
} cam_hal_spi2dvp_data_endian_e;

/**
 *  @brief SPI2DVP configuration strcut definition
 */
typedef struct {
    cam_hal_spi2dvp_data_order_e  order;   /*!< SPI order, Y first or UV first */
    cam_hal_spi2dvp_data_format_e format;  /*!< SPI format, YUYV or Y only */
    cam_hal_spi2dvp_mode_e        mode;    /*!< SPI mode, reference SPI phase and polarity */
    cam_hal_spi2dvp_data_endian_e endian;  /*!< SPI endian, LSB or MSB */
}cam_hal_spi2dvp_input_t;

/**
 *  @brief Input V/H sync active level type definition
 */
typedef enum {
    CAM_HAL_DVP_ACTIVE_HIGH, /*!< Input is active-high */
    CAM_HAL_DVP_ACTIVE_LOW,  /*!< Input is active-low */
}cam_hal_dvp_active_type_e;

/**
 *  @brief Input data order type definition
 */
typedef enum {
    CAM_HAL_DATA_BYTE_LOWER,  /*!< The first data is lower-byte */
    CAM_HAL_DATA_BYTE_HIGHER, /*!< The first data is higher-byte */
}cam_hal_dvp_data_byte_t;

/**
 * @brief DVP input configuration
 */
typedef struct {
    cam_hal_dvp_active_type_e hsync;        /*!< DVP HSync active level */
    cam_hal_dvp_active_type_e vsync;        /*!< DVP VSync active level */
    cam_hal_dvp_data_byte_t   order;        /*!< DVP data order, lower byte first or higher first */
    uint32_t                  pclk;         /*!< Sensor pixel clock */
    uint16_t                  totalHPixels; /*!< Sensor width with hblank, pixel base */
}cam_hal_dvp_input_t;

/**
 * @brief input configuration
 */
typedef union {
    cam_hal_spi2dvp_input_t spi2dvp_input; /*!< cam config when input from spi2dvp */
    cam_hal_dvp_input_t     dvp_input;     /*!< cam config when input from dvp */
}cam_hal_src_cfg_t;

/**
 * @brief image size
 */
typedef struct {
    uint16_t width;  /*!< image width */
    uint16_t height; /*!< image height */
}cam_hal_size_t;

/**
 * @brief image crop size, include start x/y
 */
typedef struct {
    uint16_t start_x; /*!< image crop start point X */
    uint16_t start_y; /*!< image crop start point Y */
    uint16_t width;   /*!< image width */
    uint16_t height;  /*!< image height */
}cam_hal_crop_t;

/**
 * @brief CAM output channel frame rate config
 */
typedef struct {
    uint8_t src_fps; /*!< source frame rate of output channel. value should be larger than 0 (src_fps > 1) */
    uint8_t dst_fps; /*!< destination frame rate of output channel. 0 means always drop frame */
}cam_hal_frame_rate_t;

/**
 * @brief CAM callback frame info
 */
typedef struct {
    uint32_t event;    /*!< report event, see CAM_HAL_INT_XXX */
    uint32_t out_addr; /*!< report address, indicate current output address */
    uint32_t out_size; /*!< report size, indicate current output size, byte unit */
}cam_out_frame_info_t;

/**
 * @brief CAM output channel 0 config
 */
typedef struct {
    cam_hal_out_format_e  format;                      /*!< format */
    uint16_t              ring_line;                   /*!< ring line, it may handshake with mjpg, when > 0, force pp_num = 1 */
    uint8_t               pp_num;                      /*!< ping pong number, value 1 ~ CAM_MAX_BUFFER_NUM, 0 would be same 1 */
    uint32_t              addr[CAM_MAX_BUFFER_NUM];    /*!< output address, always start from addr[0] */
    cam_hal_frame_rate_t  fr_ctrl;                     /*!< channel frame rate control, src_fps/dst_fps = 30/15, means output 15 fps */
    cam_hal_crop_t        out_crop;                    /*!< channel final output crop size */
}cam_hal_out_t;

/**
 * @brief CAM full config
 */
typedef struct {
    cam_hal_src_e          cam_src;                    /*!< cam source select */
    cam_hal_size_t         in_size;                    /*!< cam input size */
    cam_hal_src_cfg_t      src_cfg;                    /*!< cam source configuration */
    cam_hal_out_t          output_cfg;                 /*!< cam output configuration */
    void (*out_callback)(cam_out_frame_info_t info);   /*!< callback for output channel */
}cam_hal_cfg_t;

/*============================ API ========================================*/

/**
 * @brief Initialize cam module
 * @param cam_hal_cfg cam configuration
 * @return 0 on success, negative on error
 */
int cam_init(cam_hal_cfg_t *cam_hal_cfg);

/**
 * @brief start cam module and ready to receive image
 * @return 0 on success, negative on error
 */
int cam_start(void);

/**
 * @brief De-initialize cam module
 * @return 0 on success, negative on error
 */
int cam_deinit(void);

/**
 * @brief Run-time update channel output frame rate, it may do drop when dst_fps = 0
 * @param frc frame rate configuration
 * @return 0 on success, negative on error
 */
int cam_fps_cfg(cam_hal_frame_rate_t frc);

#endif
