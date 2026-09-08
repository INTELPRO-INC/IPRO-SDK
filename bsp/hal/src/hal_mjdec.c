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

#include "drv_mjdec.h"
#include "drv_glb.h"
#include "hal_mjdec.h"


#define NV12_IMAGE_SIZE(width,height)       ((width)*(height)*3/2)
#define YUV422_IMAGE_SIZE(width,height)     ((width)*(height)*2)
#define YUV400_IMAGE_SIZE(width,height)     ((width)*(height))
#define NV12_IMAGE_UV_SIZE(width,height)    ((width)*(height)/2)
#define YUV422_IMAGE_UV_SIZE(width,height)  ((width)*(height))


typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t yuv_mode;
    uint8_t *jpeg_buf_addr;
    uint8_t *out_yy_buf_addr;
    uint8_t *out_uv_buf_addr;
#if defined(CONFIG_IPRO6)
    uint8_t out_kick_mode_en;
    uint8_t out_kick_mode_sel;
#endif
} mjdec_local_config_t;

typedef struct {
    mjdec_frame_cb frame_cb;
    void *userData;
} mjdec_handle_cfg_type;


static mjdec_local_config_t  g_mjdec_local_config;
static mjdec_handle_cfg_type g_mjdec_handle_cfg;
#if defined(CONFIG_IPRO6)
static uint32_t g_mjdec_kick_int_cnt;
#endif
static void do_handle_decoded_frame_done(void)
{
    mjdec_frame_desc_t frame_desc;
    uint8_t *frame_start_addr_yy;
    uint8_t *frame_start_addr_uv;
    uint32_t frame_size_yy;
    uint32_t frame_size_uv;

    frame_start_addr_yy = g_mjdec_local_config.out_yy_buf_addr;
    frame_start_addr_uv = g_mjdec_local_config.out_uv_buf_addr;


    frame_size_yy = YUV400_IMAGE_SIZE(g_mjdec_local_config.width, 8);
    if (g_mjdec_local_config.yuv_mode == MJDEC_YUV422_SP) {
        frame_size_uv = YUV422_IMAGE_UV_SIZE(g_mjdec_local_config.width, g_mjdec_local_config.height);
    } else if (g_mjdec_local_config.yuv_mode == MJDEC_HAL_FORMAT_YUV400) {
        frame_size_uv = 0;
    } else {
        frame_size_uv = NV12_IMAGE_UV_SIZE(g_mjdec_local_config.width, g_mjdec_local_config.height);
    }

    frame_desc.frame_start_addr_yy = frame_start_addr_yy;
    frame_desc.frame_start_addr_uv = frame_start_addr_uv;
    frame_desc.frame_size_yy = frame_size_yy;
    frame_desc.frame_size_uv = frame_size_uv;
    frame_desc.ucIntType = MJDEC_HAL_INT_NORMAL;
        
    if (g_mjdec_handle_cfg.frame_cb) {

        g_mjdec_handle_cfg.frame_cb(g_mjdec_handle_cfg.userData, &frame_desc);
    }

    drv_mjdec_disable();

}

#if defined(CONFIG_IPRO6)
static void do_handle_decoded_kick_done(uint8_t int_type)
{
    mjdec_frame_desc_t frame_desc;
    uint8_t *frame_start_addr_yy;
    uint8_t *frame_start_addr_uv;
    uint32_t frame_size_yy;
    uint32_t frame_size_uv;
    uint32_t out_width;
    uint32_t total_block_of_line, block_num_of_line;

    frame_start_addr_yy = g_mjdec_local_config.out_yy_buf_addr;
    frame_start_addr_uv = g_mjdec_local_config.out_uv_buf_addr;

    if (g_mjdec_local_config.out_kick_mode_sel == 0)    /*single block out*/
    {
        if(g_mjdec_local_config.width < 128)
            out_width = g_mjdec_local_config.width;
        else
        {
            total_block_of_line = g_mjdec_local_config.width/128 + ((g_mjdec_local_config.width &(128-1)) != 0);
            block_num_of_line = g_mjdec_kick_int_cnt%total_block_of_line;
            if(block_num_of_line == (total_block_of_line-1))   /*last block of a block line*/
            {
                out_width = g_mjdec_local_config.width - block_num_of_line*128;
            }
            else
                out_width = 128;
        }
    }
    else    /*block line out*/
        out_width = g_mjdec_local_config.width;

    frame_size_yy = YUV400_IMAGE_SIZE(out_width, 8);
    if (g_mjdec_local_config.yuv_mode == MJDEC_YUV422_SP) {
        frame_size_uv = YUV422_IMAGE_UV_SIZE(out_width, 8);
    } else if (g_mjdec_local_config.yuv_mode == MJDEC_HAL_FORMAT_YUV400) {
        frame_size_uv = 0;
    } else {
        frame_size_uv = NV12_IMAGE_UV_SIZE(out_width,16);
        frame_size_yy = YUV400_IMAGE_SIZE(out_width, 16);
    }

    frame_desc.frame_start_addr_yy = frame_start_addr_yy;
    frame_desc.frame_start_addr_uv = frame_start_addr_uv;

    if(int_type == MJDEC_HAL_INT_NORMAL)
    {
        frame_desc.frame_size_yy = 0;
        frame_desc.frame_size_uv = 0;
    }
    else    /*for MJDEC_HAL_INT_KICK only*/
    {
        frame_desc.frame_size_yy = frame_size_yy;
        frame_desc.frame_size_uv = frame_size_uv;
        g_mjdec_kick_int_cnt++;
    }
    frame_desc.ucIntType = int_type;

    if (g_mjdec_handle_cfg.frame_cb) {
        g_mjdec_handle_cfg.frame_cb(g_mjdec_handle_cfg.userData, &frame_desc);
    }

}
#endif

int mjdec_init(mjdec_config_t *config)
{
    int ret=0;
    MJDEC_CFG_Type tMJDecCfgType;


    tMJDecCfgType.burstRead         = MJDEC_BURST_INCR16;                       /*!< MJDEC burst read type */
    tMJDecCfgType.burstWrite        = MJDEC_BURST_INCR16;                       /*!< MJDEC burst write type */
    tMJDecCfgType.yuv               = config->yuv_mode;                         /*!< MJDEC control YUV mode */
    tMJDecCfgType.intCnt            = 1;                                        /*!< Set frame threshold to issue interrupt */
    tMJDecCfgType.bufferFrameYY     = (uint32_t)config->output_buf_addr_yy;     /*!< YUV frame buffer address of YY frame */
    tMJDecCfgType.bufferFrameUV     = (uint32_t)config->output_buf_addr_uv; /*!< YUV frame buffer address of UV frame */
    tMJDecCfgType.bufferStream      = (uint32_t)config->input_buf_addr;         /*!< JPEG stream buffer address */
    tMJDecCfgType.resolutionX       = config->width;                            /*!< YUV frame RESOLUTION X */
    tMJDecCfgType.resolutionY       = config->height;                           /*!< YUV frame RESOLUTION Y */
    tMJDecCfgType.headByte          = 0;                                        /*!< Skip JPEG stream header byte */
    tMJDecCfgType.headSkip          = config->is_skip_header;                   /*!< Skip JPEG stream header */
    tMJDecCfgType.evenOrderEnable   = config->out_u_order_even;                 /*!< Enable:U is even byte of UV frame and V is odd byte of UV frame */
    tMJDecCfgType.dmyBlock          = ((config->width%16)!=0) | ((config->height%16)!=0); /*!< MJPEG last half block with dummy data 8'h80 */
    tMJDecCfgType.dmyVertical       = ((config->height%16)!=0);                 /*!< MJPEG last half vertical block drop */
    tMJDecCfgType.dmyHorizational   = ((config->width%16)!=0);                  /*!< MJPEG last half horizational block drop */
#if defined(CONFIG_IPRO6)
    tMJDecCfgType.kickModeEn        = config->out_kick_mode_en;                 /*!< MJDEC kick mode enable*/
    tMJDecCfgType.kickModeSel       = config->out_kick_mode_sel;                /*!< MJDEC kick mode unit sel */
#endif

    if (config->is_skip_header == 1)
    {
        if (config->yuv_mode == MJDEC_HAL_FORMAT_YUV420_SP)
            tMJDecCfgType.headByte = 714;   /*TBD: according to ip85 JPEG_420_Header_Type headerByte=714*/
        if (config->yuv_mode == MJDEC_HAL_FORMAT_YUV400)
            tMJDecCfgType.headByte = 599;       /*TBD: according to ip85 JPEG_400_Header_Type headerByte=595*/
        if (config->yuv_mode == MJDEC_HAL_FORMAT_YUV422_SP)
            tMJDecCfgType.headByte = 605;       /*TBD: according to ip85 JPEG_422_Header_Type headerByte=605*/
    }

    /* save the resolution info */
    g_mjdec_local_config.width              = config->width;
    g_mjdec_local_config.height             = config->height;
    g_mjdec_local_config.yuv_mode           = config->yuv_mode;
    g_mjdec_local_config.jpeg_buf_addr      = config->input_buf_addr;
    g_mjdec_local_config.out_yy_buf_addr    = (uint8_t *)tMJDecCfgType.bufferFrameYY;
    g_mjdec_local_config.out_uv_buf_addr    = (uint8_t *)tMJDecCfgType.bufferFrameUV;
    #if defined(CONFIG_IPRO6)
        g_mjdec_local_config.out_kick_mode_en   = config->out_kick_mode_en;
        g_mjdec_local_config.out_kick_mode_sel  = config->out_kick_mode_sel;
    #endif
    
    ret = drv_mjdec_init(&tMJDecCfgType);
    if(ret<0)
    {
        printf("Error! Current MJCOMBO_HW_STATE is for encode.\r\n");
        return ret;
    }

#if defined(CONFIG_IPRO6)
    Interrupt_Handler_Register(JCOMB_IRQn, mjdec_irq_handler);
    CPU_Interrupt_Enable(JCOMB_IRQn);
#endif  
#if defined(CONFIG_IPRO6LE) /*HW not support MJDEC?*/
    //Interrupt_Handler_Register(MJPEG_IRQn, mjdec_irq_handler);
    //CPU_Interrupt_Enable(MJPEG_IRQn);
#endif
#if defined(CONFIG_IPRO7)
    Interrupt_Handler_Register(MJDEC_IRQn, mjdec_irq_handler);
    CPU_Interrupt_Enable(MJDEC_IRQn);
#endif
    return ret;
}



int mjdec_start_decoder(uint8_t *jpeg_buf_addr)
{
    drv_mjdec_enable();
    drv_mjdec_push_frame((uint32_t)jpeg_buf_addr);

#if defined(CONFIG_IPRO6)
    if(g_mjdec_local_config.out_kick_mode_en)
    {
        g_mjdec_kick_int_cnt = 0;
        drv_mjdec_kick();
    }
#endif

    return 0;
}

int mjdec_deinit(void)
{
    return drv_mjdec_deinit();
}
void mjdec_disable(void)
{
    drv_mjdec_disable();
}
void mjdec_irq_handler(void)
{
    uint32_t tmpVal;

    tmpVal = drv_mjdec_get_intstatus(); /*get INT status*/
    drv_mjdec_int_clr(MJDEC_INT_ALL);   /*clear pending INT status*/

#if defined(CONFIG_IPRO6)
    /*handle kick first before normal INT*/
    if(g_mjdec_local_config.out_kick_mode_en)
    {
        if( reg_get_bits(tmpVal, MJCOMB_CSR_STS_DEC_KICK_INT) ){
            //--can't print in ISR //printf("MJDEC enter kick INT.\r\n");
            do_handle_decoded_kick_done(MJDEC_HAL_INT_KICK);
        }
        if( reg_get_bits(tmpVal, MJCOMB_CSR_STS_DEC_NORMAL_INT) ){
            do_handle_decoded_kick_done(MJDEC_HAL_INT_NORMAL);
            //--can't print in ISR //printf("MJDEC frame done.\r\n");
        }
    }
    else
    {
        if( reg_get_bits(tmpVal, MJCOMB_CSR_STS_DEC_NORMAL_INT) ){
            do_handle_decoded_frame_done();
            //--can't print in ISR //printf("MJDEC frame done.\r\n");
        }
    }
#endif
#if defined(CONFIG_IPRO7)
    if( reg_get_bits(tmpVal, MJCOMB_CSR_STS_DEC_NORMAL_INT) ){
        do_handle_decoded_frame_done();
        //--can't print in ISR //printf("MJDEC frame done.\r\n");
    }
#endif
    if( reg_get_bits(tmpVal, MJCOMB_CSR_STS_DEC_IDLE_INT) ){
        //--can't print in ISR //printf("MJDEC enter idle.\r\n");
    }
    

}
int mjdec_set_callback(mjdec_frame_cb frame_cb, void *usrdata)
{
    g_mjdec_handle_cfg.frame_cb = frame_cb;
    g_mjdec_handle_cfg.userData = usrdata;

    return 0;
}

#if defined(CONFIG_IPRO6)
void mjdec_kick(void)
{
    drv_mjdec_kick();
}
void mjdec_set_out_addr(uint32_t out_yy_addr, uint32_t out_uv_addr)
{
    drv_mjdec_set_out_addr(out_yy_addr, out_uv_addr);
}
#endif
