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
#include <stdlib.h>
#include <string.h>
#include "compiler/common.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "system_soc.h"
#include "hal_mjpeg.h"
#include "demo_mjpeg_cfg.h"
#include "isp_tsrc_sample.h"


static uint8_t ATTR_PSRAM_SECTION jpg_head_buf[800] = { 0 };
static uint32_t jpg_head_len;

static SemaphoreHandle_t gJencNotify;

static uint32_t g_udJencCopiedOutBlockCnt;
static uint32_t g_udJencOutBufSize;
static uint8_t *g_pucJencOutBufAddr;
static uint32_t g_udJencStreamBufSize;
static uint8_t *g_pucJencStreamBufAddr;

static uint32_t g_udJencStreamBufWriteOffset;

volatile int32_t g_udJencMenualPopFrameCnt=0;

static char * g_apcJpegIntInfo[] ={
    "MJENC_HAL_INT_NORMAL",
    "MJENC_HAL_INT_IDLE",
    "MJENC_HAL_INT_SWAP",
    "MJENC_HAL_INT_ERR_CAM_OVERWRITE",
    "MJENC_HAL_INT_ERR_MEM_OVERWRITE",
    "MJENC_HAL_INT_ERR_FRAME_OVERWRITE"
};

/*------------------------------------------------------------------------------

    Dummy Printf fucnction

------------------------------------------------------------------------------*/
void dummyprintf(const char * format, ... ) 
{}


#if (MJENC_TEST_OUT_SWAP_MODE_EN == 1)
static void mjpeg_test_enc_swap_mode_cb(uint32_t event)
{
    drv_mjpeg_swap_sts_info tSwapSts;
    //uint32_t udHWOutBlockCnt;
    uint32_t udCopyByteSize;
    uint32_t udCopySrcAddr;
    uint32_t udCopyDstAddr;

    //udHWOutBlockCnt=0;
    if ((event == MJENC_HAL_INT_SWAP) /*|| (event == MJENC_HAL_INT_NORMAL)*/) /*ignore INT_NORMAL when in swap mode*/
    {
        drv_mjpeg_get_swap_status(&tSwapSts);
        //udHWOutBlockCnt = mjpeg_get_swap_occur_cnt();

        //not print due to cause case44 cam overwrite. //DEMO_MJPEG_PRINTF("swap_mode_cb => mjpeg_get_swap_occur_cnt()%d, swap0_full %d, swap1_full  %d, swap_idx %d, swap_fstart %d, swap_fend %d, swap_end_bit_cnt 0x%08x\r\n",
        //not print due to cause case44 cam overwrite. //    (unsigned int)udHWOutBlockCnt,
        //not print due to cause case44 cam overwrite. //    (unsigned int)tSwapSts.jenc_csr_7.BF.sts_enc_swap0_full,
        //not print due to cause case44 cam overwrite. //    (unsigned int)tSwapSts.jenc_csr_7.BF.sts_enc_swap1_full,
        //not print due to cause case44 cam overwrite. //    (unsigned int)tSwapSts.jenc_csr_7.BF.sts_enc_read_swap_idx,
        //not print due to cause case44 cam overwrite. //    (unsigned int)tSwapSts.jenc_csr_7.BF.sts_enc_swap_fstart,
        //not print due to cause case44 cam overwrite. //    (unsigned int)tSwapSts.jenc_csr_7.BF.sts_enc_swap_fend,
        //not print due to cause case44 cam overwrite. //    (unsigned int)tSwapSts.jenc_csr_8.BF.sts_enc_frame_swap_end_bit_cnt);

        if(tSwapSts.jenc_csr_7.BF.sts_enc_swap_fstart)
        {
            g_udJencCopiedOutBlockCnt = 0;
            g_udJencStreamBufWriteOffset = 0;
        }

        if(tSwapSts.jenc_csr_7.BF.sts_enc_swap_fend)
        {
            udCopyByteSize = COMMON_AlignCeil(tSwapSts.jenc_csr_8.BF.sts_enc_frame_swap_end_bit_cnt, 8)/8;
            udCopySrcAddr = (uint32_t)g_pucJencOutBufAddr + (tSwapSts.jenc_csr_7.BF.sts_enc_read_swap_idx)*(g_udJencOutBufSize>>1);
            udCopyDstAddr = (uint32_t)g_pucJencStreamBufAddr + g_udJencStreamBufWriteOffset;
            L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)udCopySrcAddr, (unsigned long)udCopyByteSize);
            memcpy((void*)udCopyDstAddr, (void*)udCopySrcAddr, udCopyByteSize);
            g_udJencStreamBufWriteOffset += udCopyByteSize;
            DEMO_MJPEG_PRINTF("swap_mode_cb - fend => g_udJencCopiedOutBlockCnt %d, g_pucJencStreamBufAddr 0x%08x, g_udJencStreamBufWriteOffset 0x%08x, swap_idx %d, udCopyByteSize %d bytes\r\n",
                       (unsigned int)g_udJencCopiedOutBlockCnt, (unsigned int)g_pucJencStreamBufAddr, (unsigned int)g_udJencStreamBufWriteOffset, tSwapSts.jenc_csr_7.BF.sts_enc_read_swap_idx, (unsigned int)udCopyByteSize);
            xSemaphoreGiveFromISR(gJencNotify, NULL); /*frame done*/
        }
        else
        {
            udCopyByteSize = g_udJencOutBufSize>>1;
            udCopySrcAddr = (uint32_t)g_pucJencOutBufAddr + ((tSwapSts.jenc_csr_7.BF.sts_enc_read_swap_idx))*udCopyByteSize;
            udCopyDstAddr = (uint32_t)g_pucJencStreamBufAddr + g_udJencStreamBufWriteOffset;
            L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)udCopySrcAddr, (unsigned long)udCopyByteSize);
            memcpy((void*)udCopyDstAddr, (void*)udCopySrcAddr, udCopyByteSize);
            g_udJencStreamBufWriteOffset += udCopyByteSize;
            g_udJencCopiedOutBlockCnt++;
            DEMO_MJPEG_PRINTF("swap_mode_cb => g_udJencCopiedOutBlockCnt %d, g_udJencStreamBufWriteOffset 0x%08x, swap_idx %d, udCopyByteSize %d bytes\r\n",
                        (unsigned int)g_udJencCopiedOutBlockCnt, (unsigned int)g_udJencStreamBufWriteOffset, tSwapSts.jenc_csr_7.BF.sts_enc_read_swap_idx, (unsigned int)udCopyByteSize);
        }

        /*free swap memory blcok after copied to stream buffer*/
        mjpeg_free_swap_memory();
    }
    else
        printf("Unknown jenc event %lu (%s)\r\n", event, g_apcJpegIntInfo[event]);
}
#else
static void mjpeg_test_enc_event_cb(uint32_t event)
{
    if (event == MJENC_HAL_INT_NORMAL)
        xSemaphoreGiveFromISR(gJencNotify, NULL);
    else
        printf("Unknown jenc event %lu (%s)\r\n", event, g_apcJpegIntInfo[event]);
}
#endif


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



int sample_jpg_enc(void)
{
    int     ret=0;
    uint8_t type=0;
    uint8_t temp=0;
    uint32_t loop_cnt=0;
    struct mjpeg_hal_cfg_t mjpeg_cfg;
    uint8_t *alloc_out_buf_addr;
        
    #if (MJENC_TEST_HW_AUTO_MODE_EN == 0)
        uint8_t bFirst = 1;
        #if (MJENC_TEST_RUN_SW_MODE_SEL == 1)
        uint8_t ucKickCnt = 0;
        uint8_t ucHBlkCnt = 0;
        #endif
    #endif

    g_udJencCopiedOutBlockCnt = 0;
    gJencNotify = xSemaphoreCreateCounting(1, 0);


    #if (MJENC_TEST_OUT_SWAP_MODE_EN == 1)
        g_udJencOutBufSize = MJENC_TEST_SWAP_MODE_JPG_OUT_BUF_SIZE;
    #else
        g_udJencOutBufSize = MJENC_TEST_NORM_MODE_JPG_OUT_BUF_SIZE;
    #endif
    alloc_out_buf_addr = (uint8_t *) pvPortMalloc( g_udJencOutBufSize+32-8 );   /*add extra 24byte for 32byte align to avoid cache line 32byte boundary issue*/
    g_pucJencOutBufAddr = alloc_out_buf_addr;
    if (!g_pucJencOutBufAddr) {
        printf("%s[%d] error: memory malloc for jenc out buffer  failed! (size: %lu)\r\n", __func__, __LINE__, g_udJencOutBufSize);
        return -1;
    }
    else
        printf("memory malloc for jenc out buffer OK! (addr: 0x%08x, size: %lu)\r\n",(unsigned int)g_pucJencOutBufAddr, g_udJencOutBufSize);
    memset((void*)g_pucJencOutBufAddr, 0x0, g_udJencOutBufSize);

    /*do 32byte align to avoid cache line 32byte boundary issue*/
    g_pucJencOutBufAddr = (uint8_t *)COMMON_AlignCeil((unsigned long)g_pucJencOutBufAddr, 32);
    printf("32byte aling jenc out buffer to addr: 0x%08x\r\n",(unsigned int)g_pucJencOutBufAddr);

    
    #if (MJENC_TEST_OUT_SWAP_MODE_EN == 1)
        /*HW output to g_pucJencOutBufAddr then band copy from swap out buffer to g_pucJencStreamBufAddr.*/
        g_udJencStreamBufSize = MJENC_TEST_SWAP_MODE_STREAM_OUT_BUF_SIZE;
        g_pucJencStreamBufAddr = (uint8_t *) pvPortMalloc( g_udJencStreamBufSize );
        if (!g_pucJencOutBufAddr) {
            vPortFree(g_pucJencOutBufAddr);
            printf("%s[%d] error: memory malloc for stream buffer failed! (size: %lu)\r\n", __func__, __LINE__, g_udJencStreamBufSize);
            return -1;
        }
        printf("memory malloc for stream buffer OK! (addr: 0x%08x, size: %lu)\r\n",(unsigned int)g_pucJencStreamBufAddr, g_udJencStreamBufSize);
        g_udJencStreamBufWriteOffset = 0;
        memset((void*)g_pucJencStreamBufAddr, 0x0, g_udJencStreamBufSize);

    #else
        g_udJencStreamBufSize = g_udJencOutBufSize;
        g_pucJencStreamBufAddr = g_pucJencOutBufAddr;
        g_udJencStreamBufWriteOffset = 0;
        printf("stream buffer info (addr: 0x%08x, size: %lu)\r\n",(unsigned int)g_pucJencStreamBufAddr, g_udJencStreamBufSize);
    #endif
     


    mjpeg_cfg.format         = MJENC_TEST_JPG_ENC_YUV_MODE;
    mjpeg_cfg.quality        = MJENC_TEST_JPG_ENC_Q_VALUIE;
    if ( (mjpeg_cfg.format==MJPEG_HAL_FORMAT_YUV420SP_NV12) ||
         (mjpeg_cfg.format==MJPEG_HAL_FORMAT_YUV420SP_NV21)     )
        temp = 16;
    else
        temp = 8;
    #if (MJENC_TEST_HW_AUTO_MODE_EN == 1)
        mjpeg_cfg.rows           = COMMON_AlignCeil(TSRC_OUT_RING_LINE, temp);  /*input memory buffer height must be 8 alignment for YCC400/YCC422, 16 alignement for YCC420*/
        mjpeg_cfg.hw_mode_swen_en = 0;  /*0: wait first frame done*/
    #else
        mjpeg_cfg.rows           = COMMON_AlignCeil(MJENC_TEST_IMG_H, temp);  /*input memory buffer height must be 8 alignment for YCC400/YCC422, 16 alignement for YCC420*/
        mjpeg_cfg.hw_mode_swen_en = 1;
    #endif
    mjpeg_cfg.resolution_x   = MJENC_TEST_IMG_W;
    mjpeg_cfg.resolution_y   = MJENC_TEST_IMG_H;
    mjpeg_cfg.input_bufaddr0 = MJENC_TEST_YUV_BUF_ADDR;
    #if defined(TSRC_OUT_RING_LINE)
    mjpeg_cfg.input_bufaddr1 = MJENC_TEST_YUV_BUF_ADDR + MJENC_TEST_IMG_W*TSRC_OUT_RING_LINE;
    #else
    mjpeg_cfg.input_bufaddr1 = MJENC_TEST_YUV_BUF_ADDR + MJENC_TEST_IMG_W*MJENC_TEST_IMG_H;
    #endif
    mjpeg_cfg.output_bufaddr = (uint32_t)g_pucJencOutBufAddr;
    mjpeg_cfg.output_bufsize = g_udJencOutBufSize;
    mjpeg_cfg.input_yy_table = NULL;
    mjpeg_cfg.input_uv_table = NULL;
    mjpeg_cfg.output_swap_mode_en = MJENC_TEST_OUT_SWAP_MODE_EN;    /*enable encode output buffer swap mode, 0 frame mode, 1 swap mode.*/

    
    printf("mjpeg enc test info:\r\n");
    printf("  mjpeg_cfg.format         = %d\r\n",      (unsigned int)mjpeg_cfg.format             );
    printf("  mjpeg_cfg.quality        = %d\r\n",      (unsigned int)mjpeg_cfg.quality            );
    printf("  mjpeg_cfg.rows           = %d\r\n",      (unsigned int)mjpeg_cfg.rows               );
    printf("  mjpeg_cfg.resolution_x   = %d\r\n",      (unsigned int)mjpeg_cfg.resolution_x       );
    printf("  mjpeg_cfg.resolution_y   = %d\r\n",      (unsigned int)mjpeg_cfg.resolution_y       );
    printf("  mjpeg_cfg.input_bufaddr0 = 0x%08x\r\n",  (unsigned int)mjpeg_cfg.input_bufaddr0     );
    printf("  mjpeg_cfg.input_bufaddr1 = 0x%08x\r\n",  (unsigned int)mjpeg_cfg.input_bufaddr1     );
    printf("  mjpeg_cfg.output_bufaddr = 0x%08x\r\n",  (unsigned int)mjpeg_cfg.output_bufaddr     );
    printf("  mjpeg_cfg.output_bufsize = %d\r\n",      (unsigned int)mjpeg_cfg.output_bufsize     );
    printf("  mjpeg_cfg.input_yy_table = 0x%08x\r\n",  (unsigned int)mjpeg_cfg.input_yy_table     );
    printf("  mjpeg_cfg.input_uv_table = 0x%08x\r\n",  (unsigned int)mjpeg_cfg.input_uv_table     );
    printf("  mjpeg_cfg.output_swap_mode_en = %d\r\n", (unsigned int)mjpeg_cfg.output_swap_mode_en);
    #if defined(CONFIG_IPRO7) 
        mjpeg_cfg.hw_mode_sshot_en     = 0;     /*for HW auto mode, only single shot HW and not auto restart HW after frame done.*/
        printf("  mjpeg_cfg.hw_mode_sshot_en = %d\r\n", (unsigned int)mjpeg_cfg.hw_mode_sshot_en);
    #endif
    
    #if (MJENC_TEST_OUT_SWAP_MODE_EN == 1)
        mjpeg_cfg.callback = mjpeg_test_enc_swap_mode_cb;
    #else
        mjpeg_cfg.callback = mjpeg_test_enc_event_cb;
    #endif

    mjpeg_init(&mjpeg_cfg);


    switch(mjpeg_cfg.format)
    {
        case MJPEG_HAL_FORMAT_YUV422_YUYV  :
        case MJPEG_HAL_FORMAT_YUV422_YVYU  :
        case MJPEG_HAL_FORMAT_YUV422_UYVY  :
        case MJPEG_HAL_FORMAT_YUV422_VYUY  :
        case MJPEG_HAL_FORMAT_YUV422SP_NV16:
        case MJPEG_HAL_FORMAT_YUV422SP_NV61:
            type = YUV_MODE_422;
            break;
        case MJPEG_HAL_FORMAT_YUV420SP_NV12:
        case MJPEG_HAL_FORMAT_YUV420SP_NV21:
            type = YUV_MODE_420;
            break;
        case MJPEG_HAL_FORMAT_GRAY:
            type = YUV_MODE_400;
            break;
        default:
            printf("%s[%d] error: invalid YUV format %d\r\n", __func__, __LINE__, mjpeg_cfg.format);
            return -1;
            break;
    }

    jpg_head_len = mjpeg_headerCreate(type, mjpeg_cfg.quality, mjpeg_cfg.resolution_x, mjpeg_cfg.resolution_y, jpg_head_buf);
    mjpeg_fill_header_tail(jpg_head_buf, jpg_head_len);


//void drv_mjpeg_sw_enable(uint8_t frame_count, drv_mjpeg_sw_mode_e mode)
//void drv_mjpeg_sw_run(void)
//void drv_mjpeg_sw_run_abort(void)
//void drv_mjpeg_sw_kick_block(uint16_t kick_count)

#if (MJENC_TEST_HW_AUTO_MODE_EN == 0)
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)MJENC_TEST_YUV_BUF_ADDR, (unsigned long)MJENC_TEST_YUV_BUF_SIZE);
#else
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)TSRC_IN_ADDR, (unsigned long)(TSRC_WIDTH*TSRC_HEIGHT*2));
#endif
    
    //while (1)
    if(1)
    {

        #if (MJENC_TEST_HW_AUTO_MODE_EN == 0)
            /*run SW mode*/
            #if (MJENC_TEST_RUN_SW_MODE_SEL == 0)
                 /*run SW frame mode*/
                if (bFirst) {
                    mjpeg_set_mode(1, 1, 1);
                    mjpeg_start(1);
                    bFirst = 0;
                } else {
                    mjpeg_sw_run_next();
                }
            #endif
            #if (MJENC_TEST_RUN_SW_MODE_SEL == 1)
                /*run SW kick mode*/
                if (bFirst) {
                    if((mjpeg_cfg.format == MJPEG_HAL_FORMAT_YUV420SP_NV12) || 
                       (mjpeg_cfg.format == MJPEG_HAL_FORMAT_YUV420SP_NV21)  )
                       ucHBlkCnt = mjpeg_cfg.resolution_y/16;
                    else
                       ucHBlkCnt = mjpeg_cfg.resolution_y/8;
                       
                    mjpeg_sw_kick_block(COMMON_AlignCeil(ucHBlkCnt, 4)/4);   /*kick to run 1/4 height of a frame*/
                    mjpeg_set_mode(1, 0, 1);
                    mjpeg_start(1);
                    bFirst = 0;
                } else {
                    mjpeg_sw_run_next();
                }

                for(ucKickCnt=0;ucKickCnt<4;ucKickCnt++)
                {
                    printf("SW kick mode kick time cnt = %d\r\n", ucKickCnt);
                    mjpeg_sw_kick();
                    vTaskDelay(1000);
                }

            #endif
        #else
            /*HW auto mode*/
            //MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_Y, MM_MISC_DVP2AXI_PORT_A);   // move to tsrc tsrc_init()
            //MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_C, MM_MISC_DVP2AXI_PORT_B);   // move to tsrc tsrc_init()
            mjpeg_start(0);
        #endif

        #if defined(MJENC_TEST_HW_AUTO_MODE_FOREVERLOOP_EN)
        while(1)
        #endif
        {
            xSemaphoreTake(gJencNotify, portMAX_DELAY); /*frame done*/
            loop_cnt++;
            printf("Frame done count %u\r\n", (unsigned int)loop_cnt);
            
            #if (MJENC_TEST_OUT_SWAP_MODE_EN == 1)
                printf("Swap mode output Frame ready 0x%lx 0x%lx\r\n", (uint32_t)g_pucJencStreamBufAddr, (uint32_t)g_pucJencStreamBufAddr + g_udJencStreamBufWriteOffset);
            #else
                ret = mjpeg_get_one_frame((uint32_t *)&g_pucJencStreamBufAddr, (uint32_t *)&g_udJencStreamBufWriteOffset);
                if (ret == 0) {
                    printf("Frame ready 0x%lx 0x%lx\r\n", (uint32_t)g_pucJencStreamBufAddr, (uint32_t)g_pucJencStreamBufAddr + g_udJencStreamBufWriteOffset);
                }
            #endif
            
            #if defined(MJENC_TEST_MENUAL_POP_FRAME_EN)
                while(g_udJencMenualPopFrameCnt >= 0)
                {   
                    while(g_udJencMenualPopFrameCnt > 0)
                    {
                        mjpeg_drop_one_frame();
                        g_udJencMenualPopFrameCnt--;
                        vTaskDelay(10);
                    }
                    vTaskDelay(10);
                }
            #else
                mjpeg_drop_one_frame();
            #endif
        
            /*check result*/
            #if defined(_ENCODE_TEST_SAMPLE_VERIFICATION_) /*Encode Test sample*/
                unsigned int udCheckSumValue=0;
                unsigned int udCheckXORValue=0;

                bool         bErrStatusFlag=0;
                
                L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)g_pucJencStreamBufAddr, (unsigned long)g_udJencStreamBufWriteOffset);
                GetCheckSumXOR((int) g_pucJencStreamBufAddr, g_udJencStreamBufWriteOffset, &udCheckSumValue, &udCheckXORValue);

                printf("======================================\r\n");
                printf("CaseID %d %s encode output check Sum Xor @ 0x%08x: \r\n", MJENC_TEST_CASE_ID, MJENC_TEST_CASE_INFO, (unsigned int)g_pucJencStreamBufAddr);
                printf("Tar SUM: 0x%08x, Curr SUM: 0x%08x => %s\r\n", MJENC_TEST_SUM_TARGET,
                                                               udCheckSumValue,
                                                               (MJENC_TEST_SUM_TARGET == udCheckSumValue)?("V"):("XX") );
    
                printf("Tar XOR: 0x%08x, Curr XOR: 0x%08x => %s\r\n", MJENC_TEST_XOR_TARGET,
                                                               udCheckXORValue,
                                                               (MJENC_TEST_XOR_TARGET == udCheckXORValue)?("V"):("XX") );
                printf("======================================\r\n");
    
                if ( (MJENC_TEST_SUM_TARGET == udCheckSumValue) &&
                     (MJENC_TEST_XOR_TARGET == udCheckXORValue) )
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
            
            #if defined(MJENC_TEST_HW_AUTO_MODE_FOREVERLOOP_EN)
            //printf("0x30049088 = 0x%08x\r\n", *((volatile uint32_t*)0x30049088));
            tsrc_trigger();   /*abner test*/
            #endif
        }
    }
    
    #if (MJENC_TEST_HW_AUTO_MODE_EN == 0)
        #if (MJENC_TEST_RUN_SW_MODE_SEL == 0)
            /*sw frame mode*/
            mjpeg_sw_run_abort();
        #endif
        #if (MJENC_TEST_RUN_SW_MODE_SEL == 1)
            /*sw kick mode*/
            mjpeg_sw_kick_stop();
        #endif
    #else
        mjpeg_stop();
    #endif
    ret = mjpeg_deinit();

    /*free aloocated memory*/
    vPortFree(alloc_out_buf_addr);
    #if (MJENC_TEST_OUT_SWAP_MODE_EN == 1)
        vPortFree(g_pucJencStreamBufAddr);
    #endif
    
    return ret;
}

void mjpeg_test(void)
{
    //g_tMJDecNotify = xSemaphoreCreateCounting(1, 0);

    sample_jpg_enc();
}

