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

#include "drv_mjpeg.h"
#include "drv_glb.h"
#include "hal_mjpeg.h"

#define BLOCK_NUM           2
#define ROW_NUM             (8 * BLOCK_NUM)
#define MJPEG_BUFFER_SIZE   (512 * 1024)//(1 * 1024 * 1024)

static void (*g_jencCallback)(uint32_t event);
static uint32_t g_udJencSwapINTOcurrCnt;

/**********************************************************/
/*workaround for input ring mjpeg stuck at 255 frame issue*/
#if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
    #define MJPEG_OUT_INFO_MAX_VALID_CNT 2
    typedef struct {
        uint32_t udOutAddress[MJPEG_OUT_INFO_MAX_VALID_CNT];
        uint32_t udOutLen[MJPEG_OUT_INFO_MAX_VALID_CNT];
        uint32_t udOutFifoAddress;
        uint32_t udOutBufSize;
        uint32_t udOutInfoWriteIndex;
        uint32_t udOutInfoReadIndex;
    }MJPEG_OUT_INFO;
    static MJPEG_OUT_INFO g_tMjpegOutInfo;

    static uint32_t    g_udCurrOpMode;          /*1: sw mode. 0: hw auto mode*/
    static uint32_t    g_udNeedRestartHW = 0;   /*1: need to restart HW after enter IDLE.*/
    static uint8_t     g_ucHwModeSShotEn = 0;   /*1: don't restart HW after enter IDLE. 
                                                     Suppose JPEG only do one shot. Don't start HW to avoid output next frame JPEG header to memory.
                                                */
    static struct drv_mjpeg_config_t g_tMjpegCfg;


#endif
/**********************************************************/
extern uint32_t JpegHeadCreate(uint8_t type, uint8_t q, int width, int height, uint8_t *out);

/*table for generating JPEG header, considerred zigzag seq.*/
static const uint8_t tableQy[] = {
    16, 11, 12, 14, 12, 10, 16, 14,
    13, 14, 18, 17, 16, 19, 24, 40,
    26, 24, 22, 22, 24, 49, 35, 37,
    29, 40, 58, 51, 61, 60, 57, 51,
    56, 55, 64, 72, 92, 78, 64, 68,
    87, 69, 55, 56, 80, 109, 81, 87,
    95, 98, 103, 104, 103, 62, 77, 113,
    121, 112, 100, 120, 92, 101, 103, 99
};

static const uint8_t tableQuv[] = {
    17, 18, 18, 24, 21, 24, 47, 26,
    26, 47, 99, 66, 56, 66, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};


static const uint8_t tableHuffman[] = {
    0xFF, 0xC4, 0x00, 0x1F, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B,

    0xFF, 0xC4, 0x00, 0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04,
    0x04, 0x00, 0x00, 0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41,
    0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1,
    0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44,
    0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64,
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84,
    0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2,
    0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9,
    0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3,
    0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA,

    0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B,

    0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04,
    0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12,
    0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1,
    0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1,
    0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0X2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43,
    0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63,
    0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82,
    0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
    0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
    0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5,
    0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3,
    0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA
};

static void QCalc(const uint8_t *in, uint8_t *out, uint8_t q)
{
    float tq, result;
    int i;

    if (q < 1)
        tq = 5000.0f;
    else if (q <= 50)
        tq = 5000.0f / q;
    else if (q <= 100)
        tq = 200.0f - 2.0f * q;
    else
        tq = 0.0f;
    tq = tq / 100.0f;

    for (i = 0; i < 64; i++)
    {
        result = in[i] * tq;
        if (result > 255.0f)
            out[i] = 255;
        else if (result < 1.0f)
            out[i] = 1;
        else
            out[i] = (uint8_t)(result + 0.5f);
    }
}
//#define MARKER_ADD_APP1_EXIF
uint32_t mjpeg_headerCreate(uint8_t type, uint8_t q, int width, int height, uint8_t *out)
{
    uint32_t index = 0;
    uint32_t i;
#if defined(MARKER_ADD_APP1_EXIF)	
    uint8_t pad_zeros_size = 0;
#endif
    /* start of jpeg file */
    out[index++] = 0xFF;
    out[index++] = 0xD8;

    /* define quality table */
    out[index++] = 0xFF;
    out[index++] = 0xDB;
    out[index++] = 0x00;
    out[index++] = 0x43;        //=3+N, N=64
    out[index++] = 0x00;
    QCalc(tableQy, out + index, q);        //Y quality table
    index += 64;
    out[index++] = 0xFF;
    out[index++] = 0xDB;
    out[index++] = 0x00;
    out[index++] = 0x43;        //=3+N, N=64
    out[index++] = 0x01;
    QCalc(tableQuv, out + index, q);    //UV quality table
    index += 64;

#if defined(MARKER_ADD_APP1_EXIF)	
    /*JPEG MARKER for APP1 EXIF info sample*/
    /*
        0xFF, 0xE1, 0x00, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    */
    out[index++] = 0xFF;
    out[index++] = 0xE1;
    out[index++] = 0x00;
    if (type == YUV_MODE_400) {
        pad_zeros_size = 87;
    } else if (type == YUV_MODE_420) {
        pad_zeros_size = 77;
    } else if (type == YUV_MODE_422) {
        pad_zeros_size = 77;
    }
    out[index++] = pad_zeros_size + 2;
    index += pad_zeros_size;
#endif

    /* basic information of mjpeg */
    out[index++] = 0xFF;
    out[index++] = 0xC0;
    out[index++] = 0x00;
    if(type==YUV_MODE_400)
        out[index++] = 0x0B;    //length:2 + bits:1 + height:2 + width:2 + num:1 + 3 * part=1
    else
        out[index++] = 0x11;    //length:2 + bits:1 + height:2 + width:2 + num:1 + 3 * part=3
    out[index++] = 0x08;
    out[index++] = (uint8_t)((height >> 8) & 0xFF);
    out[index++] = (uint8_t)(height & 0xFF);
    out[index++] = (uint8_t)((width >> 8) & 0xFF);
    out[index++] = (uint8_t)(width & 0xFF);
    if (type == YUV_MODE_400)
    {
        out[index++] = 0x01;    //number of part
        out[index++] = 0x01;    //the first part id
        out[index++] = 0x11;    //[0:3]vertical sample coefficient, [4:7]horizontal sample coefficient
        out[index++] = 0x00;    //id of quality tab
    }
    else if (type == YUV_MODE_420)
    {
        out[index++] = 0x03;    //number of part
        out[index++] = 0x01;    //the first part id
        out[index++] = 0x22;    //[0:3]vertical sample coefficient, [4:7]horizontal sample coefficient
        out[index++] = 0x00;    //id of quality tab
        out[index++] = 0x02;    //the second part id
        out[index++] = 0x11;    //[0:3]vertical sample coefficient, [4:7]horizontal sample coefficient
        out[index++] = 0x01;    //id of quality tab
        out[index++] = 0x03;    //the third part id
        out[index++] = 0x11;    //[0:3]vertical sample coefficient, [4:7]horizontal sample coefficient
        out[index++] = 0x01;    //id of quality tab
    }
    else if (type == YUV_MODE_422)
    {
        out[index++] = 0x03;    //number of part
        out[index++] = 0x01;    //the first part id
        out[index++] = 0x21;    //[0:3]vertical sample coefficient, [4:7]horizontal sample coefficient
        out[index++] = 0x00;    //id of quality tab
        out[index++] = 0x02;    //the second part id
        out[index++] = 0x11;    //[0:3]vertical sample coefficient, [4:7]horizontal sample coefficient
        out[index++] = 0x01;    //id of quality tab
        out[index++] = 0x03;    //the third part id
        out[index++] = 0x11;    //[0:3]vertical sample coefficient, [4:7]horizontal sample coefficient
        out[index++] = 0x01;    //id of quality tab
    }

    /* define Huffman table */
    for (i = 0; i < sizeof(tableHuffman); i++)
    {
        out[index++] = tableHuffman[i];
    }

    /* start of scan*/
    out[index++] = 0xFF;
    out[index++] = 0xDA;
    if (type == YUV_MODE_400)
    {
        out[index++] = 0x00;
        out[index++] = 0x08;    //length
        out[index++] = 0x01;
        out[index++] = 0x01;
        out[index++] = 0x00;
        out[index++] = 0x00;    /*spectra sel, sync CModel output*/
        out[index++] = 0x3F;
        out[index++] = 0x00;
    }
    else
    {
        out[index++] = 0x00;
        out[index++] = 0x0C;    //length
        out[index++] = 0x03;
        out[index++] = 0x01;
        out[index++] = 0x00;
        out[index++] = 0x02;
        out[index++] = 0x11;
        out[index++] = 0x03;
        out[index++] = 0x11;
        out[index++] = 0x00;
        out[index++] = 0x3F;
        out[index++] = 0x00;
    }
    return index;
}

void mjpeg_irq_handler(void)
{
    uint32_t int_stat = drv_mjpeg_get_intstatus();
    drv_mjpeg_swap_sts_info tSwapSts;
    drv_mjpeg_int_clear(int_stat);

    if (int_stat & MJENC_INT_STAT_SWAP) {
        g_udJencSwapINTOcurrCnt++;
        g_jencCallback(MJENC_HAL_INT_SWAP);
        drv_mjpeg_get_swap_status(&tSwapSts);
        if(tSwapSts.jenc_csr_7.BF.sts_enc_swap_fend) {
                    g_udJencSwapINTOcurrCnt = 0;
        }
    }
    if (int_stat & MJENC_INT_STAT_NORMAL) {
        #if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
            if((g_udCurrOpMode == 0)&&(g_tMjpegCfg.output_swap_mode_en == 0)) {/*HW auto mode & no enable swamp mode*/
                /*disable/enable HW for fixing mjpeg stuck after running 255 frames*/
                if(((g_tMjpegOutInfo.udOutInfoWriteIndex +1)%MJPEG_OUT_INFO_MAX_VALID_CNT) != g_tMjpegOutInfo.udOutInfoReadIndex)  /*next index position is free to use*/
                {
                    g_tMjpegOutInfo.udOutLen[g_tMjpegOutInfo.udOutInfoWriteIndex] = drv_mjpeg_get_frame_info(&g_tMjpegOutInfo.udOutFifoAddress);
                    g_tMjpegOutInfo.udOutInfoWriteIndex++;
                    g_tMjpegOutInfo.udOutInfoWriteIndex %= MJPEG_OUT_INFO_MAX_VALID_CNT;
                    g_jencCallback(MJENC_HAL_INT_NORMAL);
                }
                else
                {   /*do nothing */   }

                drv_mjpeg_stop();
                /*udpate new size & address*/
                drv_mjpeg_set_out_size_addr(g_tMjpegOutInfo.udOutBufSize, g_tMjpegOutInfo.udOutAddress[g_tMjpegOutInfo.udOutInfoWriteIndex]);
                //drv_mjpeg_start(); //move to IDLE INT, must restart HW onlay after enterred IDLE.
                g_udNeedRestartHW = 1;
                if(drv_mjpeg_get_hw_mode_swen()==0){
                    drv_mjpeg_set_hw_mode_swen(1);  /*manual disable hw_mode_swen after first frame done due to workaound method need to restart hw every frame */
                }
            } else if((g_udCurrOpMode == 0)&&(g_tMjpegCfg.output_swap_mode_en == 1)) {/*HW auto mode & enable swamp mode*/
                drv_mjpeg_stop();
                g_udNeedRestartHW = 1;
                g_jencCallback(MJENC_HAL_INT_NORMAL);
                if(drv_mjpeg_get_hw_mode_swen()==0){
                    drv_mjpeg_set_hw_mode_swen(1);  /*manual disable hw_mode_swen after first frame done due to workaound method need to restart hw every frame */
                }
            } else {
                g_jencCallback(MJENC_HAL_INT_NORMAL);
            }
        #else
            g_jencCallback(MJENC_HAL_INT_NORMAL);
        #endif
    }
    if (int_stat & MJENC_INT_STAT_CAM_OV) {
        g_jencCallback(MJENC_HAL_INT_ERR_CAM_OVERWRITE);
    }
    if (int_stat & MJENC_INT_STAT_MEM_OV) {
        g_jencCallback(MJENC_HAL_INT_ERR_MEM_OVERWRITE);
    }
    if (int_stat & MJENC_INT_STAT_FRM_OV) {
        g_jencCallback(MJENC_HAL_INT_ERR_FRAME_OVERWRITE);
    }
    if (int_stat & MJENC_INT_STAT_IDLE) {
        #if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
            if((g_udCurrOpMode== 0) && (g_udNeedRestartHW == 1)) {/*HW auto mode*/
                /*disable/enable HW for fixing mjpeg stuck after running 255 frames*/
                if(g_ucHwModeSShotEn == 0)  /*Re-start HW will output next frame JPEG header to memory immediately.*/
                    drv_mjpeg_start();
                g_udNeedRestartHW = 0;
            }
        #endif
        g_jencCallback(MJENC_HAL_INT_IDLE);
    }
}

int mjpeg_init(struct mjpeg_hal_cfg_t *mjpeg_hal_cfg)
{
    int    ret=0;
    struct drv_mjpeg_config_t mjpeg_cfg;

    mjpeg_cfg.format = mjpeg_hal_cfg->format;
    mjpeg_cfg.quality = mjpeg_hal_cfg->quality;
    mjpeg_cfg.rows = mjpeg_hal_cfg->rows;
    mjpeg_cfg.resolution_x = mjpeg_hal_cfg->resolution_x;
    mjpeg_cfg.resolution_y = mjpeg_hal_cfg->resolution_y;
    mjpeg_cfg.input_bufaddr0 = mjpeg_hal_cfg->input_bufaddr0;
    mjpeg_cfg.input_bufaddr1 = mjpeg_hal_cfg->input_bufaddr1;
    mjpeg_cfg.output_bufaddr = mjpeg_hal_cfg->output_bufaddr;
    mjpeg_cfg.output_bufsize = mjpeg_hal_cfg->output_bufsize;
    mjpeg_cfg.input_yy_table = NULL;
    mjpeg_cfg.input_uv_table = NULL;

    mjpeg_cfg.output_swap_mode_en = mjpeg_hal_cfg->output_swap_mode_en;
    //removed //mjpeg_cfg.hw_kick_mode_en     = mjpeg_hal_cfg->hw_kick_mode_en;
    mjpeg_cfg.hw_mode_swen_en     = mjpeg_hal_cfg->hw_mode_swen_en;
    //removed //mjpeg_cfg.yy_dvp2axi_sel      = mjpeg_hal_cfg->yy_dvp2axi_sel;
    //removed //mjpeg_cfg.uv_dvp2axi_sel      = mjpeg_hal_cfg->uv_dvp2axi_sel;
    g_jencCallback = mjpeg_hal_cfg->callback;
    g_udJencSwapINTOcurrCnt = 0;

    #if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
        memset((void*)&g_tMjpegOutInfo, 0, sizeof(MJPEG_OUT_INFO_MAX_VALID_CNT));
        g_ucHwModeSShotEn = mjpeg_hal_cfg->hw_mode_sshot_en;
    #endif

    ret = drv_mjpeg_init(&mjpeg_cfg);
    if(ret<0)
    {
        printf("Error! Current MJCOMBO_HW_STATE is for decode.\r\n");
        return ret;
    }

    #if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
        memcpy((void*)&g_tMjpegCfg, (void*)&mjpeg_cfg, sizeof(struct drv_mjpeg_config_t));
    #endif

    drv_mjpeg_tcint_mask(true); /*default mask normal INT*/
    drv_mjpeg_errint_mask(true);/*default mask error INTs*/
    drv_mjpeg_swapint_mask(!mjpeg_cfg.output_swap_mode_en);  /*enable swap INT*/

#if defined(CONFIG_IPRO6)
    Interrupt_Handler_Register(JCOMB_IRQn, mjpeg_irq_handler);
    CPU_Interrupt_Enable(JCOMB_IRQn);
#endif
#if defined(CONFIG_IPRO6LE)
    Interrupt_Handler_Register(MJPEG_IRQn, mjpeg_irq_handler);
    CPU_Interrupt_Enable(MJPEG_IRQn);
#endif
#if defined(CONFIG_IPRO7)
    Interrupt_Handler_Register(JPEG_IRQn, mjpeg_irq_handler);
    CPU_Interrupt_Enable(JPEG_IRQn);
#endif

    return ret;
}

void mjpeg_start(int is_sw_mode)
{
    drv_mjpeg_tcint_mask(false);
    drv_mjpeg_errint_mask(false);

    #if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
        uint32_t    i;
        g_udCurrOpMode = is_sw_mode;
    #endif

    if (is_sw_mode)
        drv_mjpeg_sw_run();
    else
    {
        #if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
            /*update g_tMjpegOutInfo and new output size & address for fixing mjpeg stuck at 255 frame issue*/
            g_tMjpegOutInfo.udOutBufSize = ((g_tMjpegCfg.output_bufsize>>7)/MJPEG_OUT_INFO_MAX_VALID_CNT) <<7;
            g_tMjpegOutInfo.udOutInfoWriteIndex = 0;
            g_tMjpegOutInfo.udOutInfoReadIndex  = 0;

            for(i=0; i<MJPEG_OUT_INFO_MAX_VALID_CNT; i++)
            {
                g_tMjpegOutInfo.udOutAddress[i]= g_tMjpegCfg.output_bufaddr + i * g_tMjpegOutInfo.udOutBufSize;
                g_tMjpegOutInfo.udOutLen[i]=0;
            }
            drv_mjpeg_set_out_size_addr(g_tMjpegOutInfo.udOutBufSize, g_tMjpegOutInfo.udOutAddress[g_tMjpegOutInfo.udOutInfoWriteIndex]);
            drv_mjpeg_idleint_mask(false);/*get idle INT for HW auto mode to get stop HW OK and ready to restrat HW*/
            // printf("abner test g_tMjpegOutInfo.udOutAddress[%d] = 0x%08x, reg off 0x14 = 0x%08x\r\n",
            //                     (unsigned int)g_tMjpegOutInfo.udOutInfoWriteIndex,
            //                     (unsigned int)g_tMjpegOutInfo.udOutAddress[g_tMjpegOutInfo.udOutInfoWriteIndex],
            //                     *((volatile unsigned int *)0x30049014));
        #endif

        drv_mjpeg_start();
    }
}

void mjpeg_set_mode(int is_sw_mode, int is_frame_mode,int frm_cnt)
{
    if (is_sw_mode)
        drv_mjpeg_sw_enable(frm_cnt, (is_frame_mode)?MJPEG_SW_FRAME_MODE:MJPEG_SW_KICK_MODE);
}

int mjpeg_deinit(void)
{
    return drv_mjpeg_deinit();
}

void mjpeg_stop(void)
{
    drv_mjpeg_stop();
}

void mjpeg_sw_run_next(void)
{
    drv_mjpeg_sw_run();
}

void mjpeg_sw_run_abort(void)
{
    drv_mjpeg_sw_run_abort();
}

void mjpeg_sw_kick_block(uint16_t kick_count)
{
    drv_mjpeg_sw_kick_block(kick_count);
}

void mjpeg_sw_kick_stop(void)
{
    drv_mjpeg_sw_kick_stop();
}

void mjpeg_sw_kick(void)
{
    drv_mjpeg_sw_kick();
}

#if defined(CONFIG_IPRO6)
uint8_t mjpeg_get_one_frame(uint32_t *addr, uint32_t *len, uint32_t *sec_half_addr, uint32_t *sec_half_len)
{
#if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
    *sec_half_addr = 0; /*sec half output start address*/
    *sec_half_len  = 0; /*sec half output size*/

    if(g_udCurrOpMode== 0) {/*HW auto mode*/
        /*work around: sw output pingopong for fixing mjpeg stuck after running 255 frames*/
        if(g_tMjpegOutInfo.udOutInfoReadIndex == g_tMjpegOutInfo.udOutInfoWriteIndex) /*no valid output*/
            return 1;

        *len  = g_tMjpegOutInfo.udOutLen[    g_tMjpegOutInfo.udOutInfoReadIndex];
        *addr = g_tMjpegOutInfo.udOutAddress[g_tMjpegOutInfo.udOutInfoReadIndex];

        return 0;
    }
    else {
        if (drv_mjpeg_get_frame_count() > 0) {
            *len = drv_mjpeg_get_frame_info(addr);
            return 0;
        } else {
            return 1;
        }
    }
    return 0;
#else
    uint32_t bstOutSize, bstOutAddr, bstOutLeftSize;

    if (drv_mjpeg_get_frame_count() > 0) {
        *len = drv_mjpeg_get_frame_info(addr);
        drv_mjpeg_get_out_size_addr_info(&bstOutSize, &bstOutAddr);

        bstOutLeftSize = (bstOutAddr + bstOutSize) - *addr;
        if((*len) > bstOutLeftSize)   /*wrap aruond*/
        {
            *sec_half_addr = bstOutAddr;            /*sec half output start address*/
            *sec_half_len  = *len - bstOutLeftSize; /*sec half output size*/
            *len           = bstOutLeftSize;
        }
        else
        {
            *sec_half_addr = 0; /*sec half output start address*/
            *sec_half_len  = 0; /*sec half output size*/
        }
        return 0;
    } else {
        return 1;
    }
#endif
}
#endif
#if defined(CONFIG_IPRO7)
uint8_t mjpeg_get_one_frame(uint32_t *addr, uint32_t *len)
{
#if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
    if(g_udCurrOpMode== 0) {/*HW auto mode*/
        /*work around: sw output pingopong for fixing mjpeg stuck after running 255 frames*/
        if(g_tMjpegOutInfo.udOutInfoReadIndex == g_tMjpegOutInfo.udOutInfoWriteIndex) /*no valid output*/
            return 1;

        *len = g_tMjpegOutInfo.udOutLen[    g_tMjpegOutInfo.udOutInfoReadIndex];
        *addr= g_tMjpegOutInfo.udOutAddress[g_tMjpegOutInfo.udOutInfoReadIndex];

        return 0;
    }
    else {
        if (drv_mjpeg_get_frame_count() > 0) {
            *len = drv_mjpeg_get_frame_info(addr);
            return 0;
        } else {
            return 1;
        }
    }
    return 0;
#else
    if (drv_mjpeg_get_frame_count() > 0) {
        *len = drv_mjpeg_get_frame_info(addr);
        return 0;
    } else {
        return 1;
    }
#endif
}
#endif
void mjpeg_drop_one_frame(void)
{
#if defined(_MJPEG_SW_OUT_PING_PONG_MODE_)
    if(g_udCurrOpMode== 0) /*HW auto mode*/
    {   /*work around: sw output pingopong for fixing mjpeg stuck after running 255 frames*/
        if(g_tMjpegOutInfo.udOutInfoReadIndex != g_tMjpegOutInfo.udOutInfoWriteIndex) /*there are valid output*/
        {
            g_tMjpegOutInfo.udOutInfoReadIndex++;
            g_tMjpegOutInfo.udOutInfoReadIndex %= MJPEG_OUT_INFO_MAX_VALID_CNT;
        }
    }
    else
    {
        drv_mjpeg_pop_one_frame();
    }
#else
    drv_mjpeg_pop_one_frame();
#endif
}

void mjpeg_fill_header_tail(uint8_t *header, uint32_t header_len)
{
    drv_mjpeg_fill_jpeg_header_tail(header, header_len);
}

void mjpeg_set_input_addr(int type, uint32_t addr)
{
    drv_mjpeg_set_input_addr(type, addr);
}

uint32_t mjpeg_get_swap_occur_cnt(void)
{
    return g_udJencSwapINTOcurrCnt;
}
void mjpeg_free_swap_memory(void)
{
    drv_mjpeg_free_swap_memory();
}
