/**
  ******************************************************************************
  * @file    drv_mjdec.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2025 INTELPRO </center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of IntelPro nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __DRV_MJDEC_H__
#define __DRV_MJDEC_H__

#include "mjcomb_csr_reg.h"
#include "mjpeg_q_reg.h"
#include "drv_common.h"

/** @addtogroup  Peripheral_Driver
 *  @{
 */

/** @addtogroup  MJDEC
 *  @{
 */

/** @defgroup  MJDEC_Public_Types
 *  @{
 */

/**
 *  @brief MJDEC YUV format definition
 */
typedef enum {
    MJDEC_YUV420_SP,    /*!< MJDEC YUV420 semi-planar mode */
    MJDEC_YUV400,       /*!< MJDEC YUV400 grey scale mode */
    MJDEC_YUV422_SP,    /*!< MJDEC YUV422 semi-planar mode */
} MJDEC_YUV_Mode_Type;

/**
 *  @brief MJDEC burst type definition
 */
typedef enum {
    MJDEC_BURST_SINGLE, /*!< MJDEC burst single */
    MJDEC_BURST_INCR4,  /*!< MJDEC burst incr4 */
    MJDEC_BURST_INCR8,  /*!< MJDEC burst incr8 */
    MJDEC_BURST_INCR16, /*!< MJDEC burst incr16 */
} MJDEC_Burst_Type;

/**
 *  @brief MJDEC interrupt type definition
 */
typedef enum {
    MJDEC_INT_NORMAL,    /*!< MJDEC normal write interrupt */
    MJDEC_INT_BACK_IDLE, /*!< MJDEC back idle interrupt */
    MJDEC_INT_ALL,       /*!< MJDEC all interrupt type */
} MJDEC_INT_Type;

/**
 *  @brief MJDEC configuration strcut definition
 */
typedef struct {
    MJDEC_Burst_Type burstRead;     /*!< MJDEC burst read type */
    MJDEC_Burst_Type burstWrite;    /*!< MJDEC burst write type */
    MJDEC_YUV_Mode_Type yuv;        /*!< MJDEC control YUV mode */
    uint8_t intCnt;                 /*!< Set frame threshold to issue interrupt */
    uint32_t bufferFrameYY;         /*!< YUV frame buffer address of YY frame */
    uint32_t bufferFrameUV;         /*!< YUV frame buffer address of UV frame */
    uint32_t bufferStream;          /*!< JPEG stream buffer address */
    uint16_t resolutionX;           /*!< YUV frame RESOLUTION X */
    uint16_t resolutionY;           /*!< YUV frame RESOLUTION Y */
    uint16_t headByte;              /*!< Skip JPEG stream header byte */
    Fun_Type headSkip;              /*!< Skip JPEG stream header, 1: input contains header. 0: input contains no header. */
    Fun_Type evenOrderEnable;       /*!< Enable:U is even byte of UV frame and V is odd byte of UV frame */
    Fun_Type dmyBlock;              /*!< MJPEG last half block with dummy data 8'h80 */
    Fun_Type dmyVertical;           /*!< MJPEG last half vertical block drop */
    Fun_Type dmyHorizational;       /*!< MJPEG last half horizational block drop */
    //--N/A for new HW. //Fun_Type swapMode;              /*!< MJDEC YUV Memory swap mode */
    //--N/A for new HW. //uint8_t picQuality;             /*!< Quality must be in 1-75 or 100 */
} MJDEC_CFG_Type;

/**
 *  @brief MJDEC frame information strcut definition
 */
typedef struct
{
    uint8_t validFrames;    /*!< Valid frames */
    uint32_t curFrameAddr;  /*!< Current frame address */
} MJDEC_Frame_Info;


/*@} end of group MJDEC_Public_Types */

/** @defgroup  MJDEC_Public_Constants
 *  @{
 */

#define MAX_MJPG_PIC_WIDTH   1920
#define MAX_MJPG_PIC_HEIGHT  1088

#define MAX_MJDEC_HEADER_BYTES 714
/** @defgroup  MJDEC_YUV_MODE_TYPE
 *  @{
 */
#define IS_MJDEC_YUV_MODE_TYPE(type) (((type) == MJDEC_YUV420) || \
                                      ((type) == MJDEC_YUV400) || \
                                      ((type) == MJDEC_YUV422_PLANAR))

/** @defgroup  MJDEC_BURST_TYPE
 *  @{
 */
#define IS_MJDEC_BURST_TYPE(type) (((type) == MJDEC_BURST_SINGLE) || \
                                   ((type) == MJDEC_BURST_INCR4) ||  \
                                   ((type) == MJDEC_BURST_INCR8) ||  \
                                   ((type) == MJDEC_BURST_INCR16))

/** @defgroup  MJDEC_INT_TYPE
 *  @{
 */
#define IS_MJDEC_INT_TYPE(type) (((type) == MJDEC_INT_NORMAL) ||    \
                                 ((type) == MJDEC_INT_BACK_IDLE) || \
                                 ((type) == MJDEC_INT_ALL))

/*@} end of group MJDEC_Public_Constants */

/** @defgroup  MJDEC_Public_Macros
 *  @{
 */
#define MJDEC_BASE MJPEG_DEC_BASE
/*@} end of group MJDEC_Public_Macros */

/** @defgroup  MJDEC_Public_Functions
 *  @{
 */
#if 0   /*not use ==> use hal driver to handle IRQ*/
void drv_mjdec_irqhandler(void);
#endif
int  drv_mjdec_init(MJDEC_CFG_Type *cfg);
int  drv_mjdec_deinit(void);
void drv_mjdec_enable(void);
void drv_mjdec_disable(void);
void drv_mjdec_get_frame_info(MJDEC_Frame_Info *info);
uint8_t drv_mjdec_get_frame_count(void);
Err_Type drv_mjdec_push_frame(uint32_t bufferMjpeg);
void drv_mjdec_pop_frame(void);
void drv_mjdec_set_frame_threshold(uint8_t count);
void drv_mjdec_set_q_value(uint8_t q);
void drv_mjdec_int_mask(MJDEC_INT_Type intType, Mask_Type intMask);
void drv_mjdec_int_clr(MJDEC_INT_Type intType);

#if 0   /*not use ==> use hal driver to handle IRQ*/
void drv_mjdec_int_callback_install(MJDEC_INT_Type intType,intCallback_Type* cbFun);
#endif

#if 0   /*not use ==> use hal driver to handle IRQ*/
void drv_mjdec_irqhandler(void);
#endif

uint32_t drv_mjdec_get_intstatus(void);
/*@} end of group MJDEC_Public_Functions */

/*@} end of group MJDEC */

/*@} end of group Peripheral_Driver */

#endif /* __DRV_MJDEC_H__ */
