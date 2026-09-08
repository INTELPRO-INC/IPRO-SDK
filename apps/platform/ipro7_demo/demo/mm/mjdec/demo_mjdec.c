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

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "system_soc.h"
#include "hal_mjdec.h"
#include "demo_mjdec_cfg.h"


#if !defined(_DECODE_TEST_SAMPLE_VERIFICATION_) /*Decode Test sample*/
    #define MJDEC_TEST_YUV_MODE                       MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422
    #define MJDEC_TEST_JPG_BIT_STREAM_BUF_SIZE        ( 0x100000 )
    #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR        ( 0x1EC00000 - MJDEC_TEST_JPG_BIT_STREAM_BUF_SIZE)
#else
    /* Buffer size for test samples (max 64KB should cover all test JPEGs) */
    #ifndef MJDEC_TEST_JPG_BIT_STREAM_BUF_SIZE
    #define MJDEC_TEST_JPG_BIT_STREAM_BUF_SIZE        ( 0x10000 )
    #endif
#endif



#define YUV422_FRAME_SIZE(X, Y)        ( (X) * (Y) * 2 )
#define YUV420_Y_FRAME_SIZE(X, Y)      ( (X) * (Y) )
#define YUV420_UV_FRAME_SIZE(X, Y)     ( (X) * (Y) / 2 )
#define YUV400_FRAME_SIZE(X, Y)        ( (X) * (Y) )


//static SemaphoreHandle_t g_tMJDecNotify;
static volatile int gJDECDone = 0;
static mjdec_config_t g_tMjdecCfg;


//static void mjdec_event_cb(unsigned int_t event)
//{
//    if (event == MJENC_HAL_INT_NORMAL)
//        xSemaphoreGiveFromISR(g_tMJDecNotify, NULL);
//    else
//        printf("Unknown jenc event %lu\r\n", event);
//}

/*-------------------------------------------------------------------------*/
static void GetCheckSumXOR(int addr, unsigned int size, unsigned int *a_pudSum, unsigned int *a_pudXor)
{
#if 0   /*byte access*/
    unsigned int  i;
    char    *addr1;
    char    cBuf[4];
    unsigned int  ud4ByteAlignSize = 0;

    if (a_pudSum==NULL)
        return;
    if (a_pudXor==NULL)
        return;

    *a_pudSum=*a_pudXor=0;
    addr1 = (char*)(addr);


    //Main loop for calculating Sum/Xor
    for(i=0; i<size/4; i++)
    {
        *a_pudSum += (*(addr1) + (*(addr1+1)<<8) + (*(addr1+2)<<16) + (*(addr1+3)<<24));
        *a_pudXor ^= (*(addr1) + (*(addr1+1)<<8) + (*(addr1+2)<<16) + (*(addr1+3)<<24));
        addr1+=4;
    }

    /*Handle no 4 alignment case*/
    ud4ByteAlignSize = ((size%4)>0)?(4*(size/4+1)):(size);
    if(ud4ByteAlignSize > size)
    {
        for(i = 0; i<4; i++)
            cBuf[i] = *(addr1 + i);
        for(i = 0; i<(ud4ByteAlignSize - size); i++)
            cBuf[3-i] = 0x00;

        *a_pudSum += *((unsigned int *)cBuf);
        *a_pudXor ^= *((unsigned int *)cBuf);
    }
#else   /*4byte access for M2 use USPI interface read OCRAM.*/
    unsigned int  i;
    unsigned int  *addr1;
    char    cBuf[4];
    unsigned int  ud4ByteAlignSize = 0;

    if (a_pudSum==NULL)
        return;
    if (a_pudXor==NULL)
        return;

    *a_pudSum=*a_pudXor=0;
    addr1 = (unsigned int*)(addr);
    ud4ByteAlignSize = ((size%4)>0)?(4*(size/4+1)):(size);

    //Main loop for calculating Sum/Xor
    for(i=0; i<size/4; i++)
    {
        *a_pudSum += *addr1;
        *a_pudXor ^= *addr1;
        addr1++;
    }

    /*Handle no 4 alignment case*/
    if(ud4ByteAlignSize > size)
    {
        /*Get last 4bytes*/
        *((unsigned int*)&cBuf[0]) = *addr1;

        /*Clear invalid bytes to 0.*/
        for(i = 0; i<(ud4ByteAlignSize - size); i++)
            cBuf[3-i] = 0x00;

        *a_pudSum += *((unsigned int *)cBuf);
        *a_pudXor ^= *((unsigned int *)cBuf);
    }
#endif
    return ;
}


int mjdec_test_frame_done(void *usrdata, mjdec_frame_desc_t *frame_desc)
{
    //csi_dcache_invalid_range(frame_desc->frame_start_addr, frame_desc->frame_size);
    printf("mjdec done, y buf: 0x%x 0x%x, uv buf: 0x%x 0x%x\r\n", 
            (unsigned int)frame_desc->frame_start_addr_yy, (unsigned int)frame_desc->frame_start_addr_yy +  (unsigned int)frame_desc->frame_size_yy,
            (unsigned int)frame_desc->frame_start_addr_uv, (unsigned int)frame_desc->frame_start_addr_uv +  (unsigned int)frame_desc->frame_size_uv);
    gJDECDone = 1;
    return 0;
}

int sample_jpg_dec(void)
{
    int ret=0;
    int frm_size = 0;
    int hw_out_buf_size = 0, final_out_buf_size = 0;
    int img_width, img_height;

    mjdec_config_t config;
    uint8_t *alloc_buf_addr;
    uint8_t *alloc_hw_out_buf_addr;
    uint8_t *align_hw_out_buf_addr;
    uint8_t *final_out_buf_addr;

#if defined(_DECODE_TEST_SAMPLE_VERIFICATION_)
    config.width  = MJDEC_TEST_IMG_W;
    config.height = MJDEC_TEST_IMG_H;
#else
    config.width  = 1280;
    config.height = 720;
#endif
    //config.quality = 50;  /*n/a for new JPEG combo HW decoder*/
    config.is_skip_header = 1;
    config.yuv_mode = MJDEC_TEST_YUV_MODE                                  ;
    config.out_u_order_even = MJDEC_TEST_U_ORDER_EVEN;
    config.input_buf_addr  = (uint8_t*)MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR;

    img_width  = COMMON_AlignCeil(config.width, 16);
    img_height = COMMON_AlignCeil(config.height, 16);
    if (config.yuv_mode == MJDEC_HAL_FORMAT_YUV422_SP)   frm_size = YUV422_FRAME_SIZE(    img_width, img_height);
    else if (config.yuv_mode == MJDEC_HAL_FORMAT_YUV400) frm_size = YUV400_FRAME_SIZE(    img_width, img_height);
    else frm_size = YUV420_Y_FRAME_SIZE(img_width, img_height)+YUV420_UV_FRAME_SIZE(img_width, img_height);

    /* JPEG output address must be 16 aligned, or it will not output first 8 byte*/

    /*portBYTE_ALIGNMENT_MASK is 0x7 ==> malloc at least will be 8byte aligned, add extra 24byte for achieving 32byte (CPU cache line size) align.*/
    alloc_buf_addr = pvPortMalloc(frm_size+32-8);    /*returned addr already aligned to portBYTE_ALIGNMENT_MASK, which is 0x7 for ipro6 & ipro7)*/

    //Frame Mode ==>  HW out and final out buffer (for output checksum) are the same.

    hw_out_buf_size    = frm_size;

    alloc_hw_out_buf_addr = alloc_buf_addr;
    config.output_buf_addr_yy = (uint8_t *)COMMON_AlignCeil((unsigned long)alloc_hw_out_buf_addr, 32);
    align_hw_out_buf_addr = config.output_buf_addr_yy;

    final_out_buf_addr    = config.output_buf_addr_yy;  /*HW direct output to final_out_buf_addr for calculating checksum.*/
    final_out_buf_size    = frm_size - (final_out_buf_addr - alloc_buf_addr);
    alloc_buf_addr = config.output_buf_addr_yy;
    config.output_buf_addr_yy = (uint8_t *)COMMON_AlignCeil((unsigned long)config.output_buf_addr_yy, 32);
    config.output_buf_addr_uv = (uint8_t *)((uint32_t)config.output_buf_addr_yy + YUV400_FRAME_SIZE(img_width, img_height));

    
    memset((void*)config.output_buf_addr_yy, 0xa5, hw_out_buf_size);
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)config.output_buf_addr_yy, (unsigned long)hw_out_buf_size);
    if(final_out_buf_addr != config.output_buf_addr_yy)
    {
        memset((void*)final_out_buf_addr, 0xa5, final_out_buf_size);
        L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)final_out_buf_addr, (unsigned long)final_out_buf_size);
    }

    printf("MJDEC Case ID %d Info === %s\r\n", MJDEC_TEST_CASE_ID, MJDEC_TEST_CASE_INFO);
    printf("config.width              = %d\r\n",     config.width           );
    printf("config.height             = %d\r\n",     config.height          );
    printf("config.is_skip_header     = %d\r\n",     config.is_skip_header  );
    printf("config.yuv_mode           = %d\r\n",     config.yuv_mode        );
    printf("config.out_u_order_even   = %d\r\n",     config.out_u_order_even);
    printf("config.yuv_mode           = %d\r\n",     config.yuv_mode        );
    printf("config.output_buf_addr_yy = 0x%08x\r\n", (unsigned int)config.output_buf_addr_yy);
    printf("config.output_buf_addr_uv = 0x%08x\r\n", (unsigned int)config.output_buf_addr_uv);
    printf("config.input_buf_addr     = 0x%08x\r\n", (unsigned int)config.input_buf_addr    );
    printf("final_out_buf_addr        = 0x%08x\r\n", (unsigned int)final_out_buf_addr   );
    printf("final_out_buf_size        = 0x%08x\r\n", (unsigned int)final_out_buf_size   );
    printf("align_hw_out_buf_addr     = 0x%08x\r\n", (unsigned int)align_hw_out_buf_addr);
    printf("hw_out_buf_size           = 0x%08x\r\n", (unsigned int)hw_out_buf_size      );
    
    memcpy((void*)&g_tMjdecCfg, (void*)&config, sizeof(mjdec_config_t));

    gJDECDone = 0;

    /*out_put_fmt:yuv420:yyy...yyuv...uv(NV12)
      out_put_fmt:yuv400:yyy...yy
      out_put_fmt:yuv422:yyy...yyuv...uv*/
    ret = mjdec_init(&config);
    if(ret<0)
    {
        printf("[mjdec][ERR] Initialization error occurred!!!\r\n");

        return ret;
    }

    mjdec_set_callback(mjdec_test_frame_done, NULL);
    /* Clean cache for input buffer (PSRAM) before hardware DMA read */
    L1C_DCACHE_CLEAN_RANGE((unsigned long)config.input_buf_addr, (unsigned long)MJDEC_TEST_JPG_BIT_STREAM_BUF_SIZE);
    mjdec_start_decoder(config.input_buf_addr);

    while (gJDECDone == 0)
        vTaskDelay(1);

    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)final_out_buf_addr, (unsigned long)final_out_buf_size);

    /*check result*/
#if defined(_DECODE_TEST_SAMPLE_VERIFICATION_)
    unsigned int udCheckSumValue=0;
    unsigned int udCheckXORValue=0;
    unsigned int udSumY_Curr=0;
    unsigned int udXorY_Curr=0;
    unsigned int udSumCbCr_Curr=0;
    unsigned int udXorCbCr_Curr=0;
    unsigned int udTempW=0;
    unsigned int udTempH=0;
    unsigned int udTempAddr=0;
    bool         bErrStatusFlag=0;


    udTempW=config.width;
    udTempH=config.height;
    udTempAddr=(unsigned int)final_out_buf_addr;

    GetCheckSumXOR((int) udTempAddr, udTempW*udTempH, &udSumY_Curr, &udXorY_Curr);
    udCheckSumValue += udSumY_Curr;
    udCheckXORValue ^= udXorY_Curr;

    if (config.yuv_mode != MJDEC_HAL_FORMAT_YUV400)
    {
        if (config.yuv_mode == MJDEC_HAL_FORMAT_YUV420_SP)
        {
            udTempW=config.width /2*2;
            udTempH=config.height/2;
        }
        else if (config.yuv_mode == MJDEC_HAL_FORMAT_YUV422_SP)
        {
            udTempW=config.width /2*2;
            udTempH=config.height;
        }
        udTempAddr=(unsigned int)final_out_buf_addr + COMMON_AlignCeil(config.width, 16)*COMMON_AlignCeil(config.height, 16); /*only support 16 aligned width/height.*/
        GetCheckSumXOR((int) udTempAddr, udTempW*udTempH, &udSumCbCr_Curr, &udXorCbCr_Curr);
        udCheckSumValue += udSumCbCr_Curr;
        udCheckXORValue ^= udXorCbCr_Curr;
    }


    printf("======================================\r\n");
    printf("CaseID %d %s decode output check Sum Xor @ 0x%08x ~ 0x%08x: \r\n", MJDEC_TEST_CASE_ID, MJDEC_TEST_CASE_INFO, (unsigned int)final_out_buf_addr,  (unsigned int)final_out_buf_addr+final_out_buf_size);
    printf("Tar SUM: 0x%08x, Curr SUM: 0x%08x => %s\r\n", MJDEC_TEST_SUM_TARGET,
                                                   udCheckSumValue,
                                                   (MJDEC_TEST_SUM_TARGET == udCheckSumValue)?("V"):("XX") );

    printf("Tar XOR: 0x%08x, Curr XOR: 0x%08x => %s\r\n", MJDEC_TEST_XOR_TARGET,
                                                   udCheckXORValue,
                                                   (MJDEC_TEST_XOR_TARGET == udCheckXORValue)?("V"):("XX") );
    printf("======================================\r\n");

    if ( (MJDEC_TEST_SUM_TARGET == udCheckSumValue) &&
         (MJDEC_TEST_XOR_TARGET == udCheckXORValue) )
    {
        printf("Identity...\r\n");
    }
    else
    {
        printf("NOT Identity!!!\r\n");
        bErrStatusFlag = 1;
    }

    if(bErrStatusFlag == 1)
    {
        printf("\r\n\r\n\r\n");
        printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXX\r\n");
        printf("X      CheckSum Failed     X\r\n");
        printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXX\r\n");
        printf("\07\07\07\07\07\r\n\r\n");
    }
    else
    {
        printf("\r\n\r\n\r\n");
        printf("VVVVVVVVVVVVVVVVVVVVVVVVVVVV\r\n");
        printf("V      CheckSum Passed     V\r\n");
        printf("VVVVVVVVVVVVVVVVVVVVVVVVVVVV\r\n");
        printf("\07\r\n\r\n");
    }

#endif


    //free(config.output_buf_addr);
    if(alloc_buf_addr != alloc_hw_out_buf_addr)
        free(alloc_hw_out_buf_addr);
    free(alloc_buf_addr);
    
    mjdec_disable();
    ret = mjdec_deinit();

    return ret;
}

void mjdec_test(void)
{
    //g_tMJDecNotify = xSemaphoreCreateCounting(1, 0);

    sample_jpg_dec();
}

