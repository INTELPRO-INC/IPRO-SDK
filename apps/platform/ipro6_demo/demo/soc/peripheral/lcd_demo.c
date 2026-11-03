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
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lcd/lcd.h"
#include "drv_glb.h"
#include "hal_mjdec.h"

/*--------------------------------------------------*/
/*--- for jpeg band decode and band display test ---*/
/*--------------------------------------------------*/
#define LCD_DEMO2_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_LCD_DEMO_RSVD_MEM_START_ADDR

#if !defined(CONFIG_FPGA)   /*for real chip verification.*/
#define REALCHIP_LCD_DEMO_RSVD_MEM_START_ADDR     (0x11000000 + (320-79)*1024)   /*reserve 79K for jpeg bit stream source*/
#endif
#define YUV422_FRAME_SIZE(X, Y)        ( (X) * (Y) * 2 )
#define YUV420_Y_FRAME_SIZE(X, Y)      ( (X) * (Y) )
#define YUV420_UV_FRAME_SIZE(X, Y)     ( (X) * (Y) / 4 * 2)
#define YUV400_FRAME_SIZE(X, Y)        ( (X) * (Y) )

uint32_t g_udLcdDemo2aRGBBufAddr=0;
volatile int    gLcdDemo2JDECDone = 0;
volatile int    gLcdDemo2JDECKickDone = 0;
volatile int    gLcdDemo2JDECKickDoneCnt = 0;
mjdec_config_t  g_tLcdDemo2MjdecCfg;
static volatile int    gLcdDemo2ShowAndKick = 0;
/*--------------------------------------------------*/


/*abgr image address for disp_show_pic_loop*/
uint8_t *g_pucAddrBass = (uint8_t *)REALCHIP_LCD_DEMO_RSVD_MEM_START_ADDR;
uint8_t *g_pucAddrCurr = (uint8_t *)REALCHIP_LCD_DEMO_RSVD_MEM_START_ADDR;
uint8_t *g_pucAddrNext = (uint8_t *)REALCHIP_LCD_DEMO_RSVD_MEM_START_ADDR;
SemaphoreHandle_t g_tDbiShowPicNotify = NULL;
static volatile uint32_t *gp_disp_drv_cb = NULL;
int     lcd_draw_cnt=0;/*abner test*/
int     flush_async_callback_cnt=0;/*abner test*/
volatile int g_stop_showing_pic_flg=0;
TaskHandle_t    gvTaskHandle, gvTaskHandle2, gvTaskHandle3;


#if defined(ILI9488_DBI_ROTATE_FOR_480x320)
    #define LCD_ROTATE_FOR_480x320      ILI9488_DBI_ROTATE_FOR_480x320
    /* ILI9488 LCD width */
    #define LCD_W ILI9488_DBI_W
    /* ILI9488 LCD height */
    #define LCD_H ILI9488_DBI_H

#elif defined(ILI9488_SPI_ROTATE_FOR_480x320)
    #define LCD_ROTATE_FOR_480x320      ILI9488_SPI_ROTATE_FOR_480x320
    /* ILI9488 LCD width */
    #define LCD_W ILI9488_SPI_W
    /* ILI9488 LCD height */
    #define LCD_H ILI9488_SPI_H
#else
    #define LCD_ROTATE_FOR_480x320      0
#endif    

void DBI_ShowMjdecBand_Task( void * arg );
int jpg_dec_and_disp(void);
void NV12_To_aRGB8888_Integer(const uint8_t* y_plane, const uint8_t* uv_plane, unsigned char* rgb_out, int width, int height);

void flush_async_callback()
{
    flush_async_callback_cnt++;/*abner test*/
    if (gp_disp_drv_cb != NULL) {
        //lv_disp_flush_ready((lv_disp_drv_t *)p_disp_drv_cb);
        gp_disp_drv_cb = NULL;
    }
}
void flush_async_callback2()
{
    flush_async_callback_cnt++;/*abner test*/
    if (gp_disp_drv_cb != NULL) {
        //lv_disp_flush_ready((lv_disp_drv_t *)p_disp_drv_cb);
        gp_disp_drv_cb = NULL;
    }
}


void lcd_chg_show_pic_addr(uint8_t* a_pucAddr)    /*abner test*/
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    g_pucAddrNext = a_pucAddr;
    if(g_tDbiShowPicNotify != NULL)
        xSemaphoreGiveFromISR(g_tDbiShowPicNotify, &xHigherPriorityTaskWoken);

}

static void lcd_show_pic_loop(void) /*abner test*/
{
    int toggle=0;
    uint32_t temp_disp_drv;

    g_tDbiShowPicNotify = xSemaphoreCreateCounting(1, 0);

    while(g_stop_showing_pic_flg != 1)
    {
        toggle++;
        toggle %= 2; /*total frames of picture to show*/
        lcd_draw_cnt++;

        #if (LCD_ROTATE_FOR_480x320 == 0)
            lcd_draw_picture_nonblocking(0, 0, 319, 479, (lcd_color_t *)(g_pucAddrCurr /*+toggle*320*480*4*/));
        #else
            #if 0
                lcd_draw_picture_nonblocking(0, 0, 479, 319, (lcd_color_t *)(g_pucAddrCurr + toggle*480*320*4));
            #else
                lcd_draw_picture_nonblocking(0, 25, 479, 25+269, (lcd_color_t *)(g_pucAddrCurr /*+toggle*480*270*4*/));
            #endif
        #endif
        gp_disp_drv_cb=&temp_disp_drv;
        while(gp_disp_drv_cb!= NULL)
        {
            vTaskDelay(100);
        }
        printf("flush_async_callback_cnt=%d\r\n", (unsigned int)flush_async_callback_cnt);

        /*display done wait next trigger for display*/
        #if 0   /*use externa trigger show pic with new address*/
            xSemaphoreTake(g_tDbiShowPicNotify, portMAX_DELAY);

        #else
            #if 1
                g_pucAddrNext = g_pucAddrBass +toggle*480*320*4;
            #else
                g_pucAddrNext = g_pucAddrBass +toggle*480*270*4;
            #endif
        #endif

        if(g_pucAddrCurr != g_pucAddrNext)
            g_pucAddrCurr = g_pucAddrNext;
    }

    vSemaphoreDelete(g_tDbiShowPicNotify);
    g_tDbiShowPicNotify = NULL;

}

int lcd_draw_area_in_buf(LCD_DRAW_IN_BUF_CFG a_tDrawCfg)
{
    uint32_t i, j;
    lcd_color_t *pUpdateBufAddr;
    /*draw a rectangle*/
    int start_x = a_tDrawCfg.line_x1;
    int start_y = a_tDrawCfg.line_y1;
    int end_x   = a_tDrawCfg.line_x2;
    int end_y   = a_tDrawCfg.line_y2;


    //if (a_tDrawCfg.buffer_addr == NULL) {
    //    return -1;
    //}
    //else
    //{
    //    if ((a_tDrawCfg.line_x1 >= a_tDrawCfg.buffer_width)  ||
    //        (a_tDrawCfg.line_y1 >= a_tDrawCfg.buffer_height) ||
    //        (a_tDrawCfg.line_x2 >= a_tDrawCfg.buffer_width)  ||
    //        (a_tDrawCfg.line_y2 >= a_tDrawCfg.buffer_height)    )
    //        {
    //            return -2;
    //        }
    //}

    /*draw line in buffer*/
    for(j=start_y; j<end_y; j++)
    {
        pUpdateBufAddr = a_tDrawCfg.buffer_addr + j*a_tDrawCfg.buffer_width + start_x;
        for(i=start_x; i<end_x; i++)
        {
            pUpdateBufAddr++;
            *pUpdateBufAddr = a_tDrawCfg.color;
            //printf("i=%d, j=%d, addr=0x%08x\r\n", (int)i, (int)j, (unsigned int)pUpdateBufAddr);
        }
    }

    return 0;
}

void lcd_draw_rec_in_buf(LCD_DRAW_IN_BUF_CFG a_tDrawCfg)
{
    LCD_DRAW_IN_BUF_CFG tDrawCfg;
    int start_x = a_tDrawCfg.line_x1;
    int start_y = a_tDrawCfg.line_y1;
    int end_x   = a_tDrawCfg.line_x2;
    int end_y   = a_tDrawCfg.line_y2;
    int thickness = a_tDrawCfg.thickness;

    //if (a_tDrawCfg.buffer_addr == NULL) {
    //    return -1;
    //}
    //else
    //{
    //    if ((a_tDrawCfg.line_x1 >= a_tDrawCfg.buffer_width)  ||
    //        (a_tDrawCfg.line_y1 >= a_tDrawCfg.buffer_height) ||
    //        (a_tDrawCfg.line_x2 >= a_tDrawCfg.buffer_width)  ||
    //        (a_tDrawCfg.line_y2 >= a_tDrawCfg.buffer_height)    )
    //        {
    //            return -2;
    //        }
    //}

    memcpy((void*)&tDrawCfg, (void*)&a_tDrawCfg, sizeof(LCD_DRAW_IN_BUF_CFG));


    /*draw rec in buffer*/
    tDrawCfg.line_x1 = start_x;
    tDrawCfg.line_y1 = start_y;
    tDrawCfg.line_x2 = end_x;
    tDrawCfg.line_y2 = start_y+thickness;
    lcd_draw_area_in_buf(tDrawCfg);

    tDrawCfg.line_x1 = start_x;
    tDrawCfg.line_y1 = end_y-thickness;
    tDrawCfg.line_x2 = end_x;
    tDrawCfg.line_y2 = end_y;
    lcd_draw_area_in_buf(tDrawCfg);

    tDrawCfg.line_x1 = start_x;
    tDrawCfg.line_y1 = start_y+thickness;
    tDrawCfg.line_x2 = start_x+thickness;
    tDrawCfg.line_y2 = end_y-thickness;
    lcd_draw_area_in_buf(tDrawCfg);

    tDrawCfg.line_x1 = end_x-thickness;
    tDrawCfg.line_y1 = start_y+thickness;
    tDrawCfg.line_x2 = end_x;
    tDrawCfg.line_y2 = end_y-thickness;
    lcd_draw_area_in_buf(tDrawCfg);

    //return 0;
}

void lcd_demo( void * arg )
{

    lcd_init();
    lcd_async_callback_register(flush_async_callback);

    //lcd_clear(LCD_COLOR_RGB(0x00, 0X00, 0X00));
    lcd_clear(LCD_COLOR_RGB(128, 0, 255));
    //for(int i=0;i<1000000;i++);
    vTaskDelay(500);
    printf("flush_async_callback_cnt=%d\r\n", (unsigned int)flush_async_callback_cnt);

    #if 1
    //while(1)
    {
        lcd_clear(LCD_COLOR_RGB(0xff, 0x55, 0));
        vTaskDelay(500);
        printf("flush_async_callback_cnt=%d\r\n", (unsigned int)flush_async_callback_cnt);
        lcd_clear(LCD_COLOR_RGB(0, 255, 0));
        vTaskDelay(500);
        printf("flush_async_callback_cnt=%d\r\n", (unsigned int)flush_async_callback_cnt);
        lcd_clear(LCD_COLOR_RGB(0, 0, 255));
        vTaskDelay(500);
        printf("flush_async_callback_cnt=%d\r\n", (unsigned int)flush_async_callback_cnt);
    }
    #endif

    //arch_delay_ms(1000);
  #if 1 /*draw char*/
    unsigned char aucTemp[]="string\r\n        !@#$\%^&";
    //while(1)
    {
        lcd_draw_str_ascii16(0,26, LCD_COLOR_RGB(  0, 255,   0), LCD_COLOR_RGB( 0, 0, 0), aucTemp, sizeof(aucTemp));
        vTaskDelay(3000);
    }

 #endif

    #if 1 /*draw area test*/
        uint32_t temp;
        uint32_t udarraysize;
        struct RGBColorTest {
            uint8_t R;
            uint8_t G;
            uint8_t B;
        };

        struct RGBColorTest aRGBColorTest[] =
        {
            {255,   0,   0},
            {255, 128,   0},
            {255, 255,   0},
            {128, 255,   0},
            {  0, 255,   0},
            {  0, 255, 128},
            {  0, 255, 255},
            {  0, 128, 255},
            {  0,   0, 255},
            {128,   0, 255},
            {255,   0, 255},
            {  0,   0, 128},
        };
        udarraysize = (sizeof(aRGBColorTest)/sizeof(struct RGBColorTest));
        for(int i=0; i<0xfffffff; i++) {
            #if (LCD_ROTATE_FOR_480x320 == 0)
                /*LCD no rotation, 320x480*/
                lcd_draw_area_nonblocking(0, 0, LCD_W -1, LCD_H -1, LCD_COLOR_RGB(aRGBColorTest[i%udarraysize].R, aRGBColorTest[i%udarraysize].G, aRGBColorTest[i%udarraysize].B));
            #else
                /*LCD with rotation, 480x320*/
                lcd_draw_area_nonblocking(0, 0, LCD_H -1, LCD_W -1 , LCD_COLOR_RGB(aRGBColorTest[i%udarraysize].B, aRGBColorTest[i%udarraysize].G, aRGBColorTest[i%udarraysize].R));
            #endif
            gp_disp_drv_cb=&temp;
            while(gp_disp_drv_cb!= NULL)
            {
                vTaskDelay(10);
            }
            printf("draw_area_nonblocking test=> flush_async_callback_cnt=%d)\r\n", (unsigned int)flush_async_callback_cnt);
            //vTaskDelay(100);

          #if 0 /*draw one frame*//*not enough memory for 480x320 pic ARGB source.*/
            #if (LCD_ROTATE_FOR_480x320 == 0)
                lcd_draw_picture_nonblocking(0, 0, 319, 479, (lcd_color_t *)(g_pucAddrCurr /*+toggle*320*480*4*/));
            #else
                #if 0
                    lcd_draw_picture_nonblocking(0, 0, 479, 319, (lcd_color_t *)(g_pucAddrCurr + toggle*480*320*4));
                #else
                    lcd_draw_picture_nonblocking(0, 25, 479, 25+269, (lcd_color_t *)(g_pucAddrCurr /*+toggle*480*270*4*/));
                #endif
            #endif
            gp_disp_drv_cb=&temp;
            while(gp_disp_drv_cb!= NULL)
            {
                vTaskDelay(10);
            }
            printf("draw_picture_nonblocking test=> flush_async_callback_cnt=%d\r\n", (unsigned int)flush_async_callback_cnt);
            //vTaskDelay(100);
          #endif

          #if 0 /*draw line by line*/
            #if (LCD_ROTATE_FOR_480x320 == 0)
                /*display line by line test*/
                for(int j=0;j<480; j++)
                {
                    gp_disp_drv_cb=&temp;
                    lcd_draw_picture_nonblocking(0, j, 320, j+1, (lcd_color_t *)(g_pucAddrCurr /*+toggle*320*480*4*/ + j*320*4));
                    while(gp_disp_drv_cb!= NULL)
                        vTaskDelay(1);
                }
            #else
                #if 0
                    lcd_draw_picture_nonblocking(0, 0, 479, 319, (lcd_color_t *)(g_pucAddrCurr + toggle*480*320*4));
                #else
                    //lcd_draw_picture_nonblocking(0, 25, 479, 25+269, (lcd_color_t *)(g_pucAddrCurr /*+toggle*480*270*4*/));
                    for(int j=0;j<270; j++)
                    {
                        gp_disp_drv_cb=&temp;
                        lcd_draw_picture_nonblocking(0, 25+j, 479, 25+j, (lcd_color_t *)(g_pucAddrCurr /*+toggle*320*480*4*/ + j*480*4));
                        while(gp_disp_drv_cb!= NULL)
                            vTaskDelay(1);
                    }

                #endif
            #endif
            printf("draw_picture_nonblocking line by line test=> flush_async_callback_cnt=%d\r\n", (unsigned int)flush_async_callback_cnt);
            vTaskDelay(100);
          #endif

          #if 0 /*draw rectangular to argb image buffer*/
            LCD_DRAW_IN_BUF_CFG tDrawCfg;

            tDrawCfg.buffer_addr = (lcd_color_t*)g_pucAddrCurr;
            #if (LCD_ROTATE_FOR_480x320 == 0)
                tDrawCfg.buffer_width = 320;
                tDrawCfg.buffer_height= 480;
            #else
                tDrawCfg.buffer_width = 480;
                tDrawCfg.buffer_height= 270;
            #endif
            tDrawCfg.line_x1 = tDrawCfg.buffer_width /2-60-1 + i%100;
            tDrawCfg.line_y1 = tDrawCfg.buffer_height/2-60-1 + i%100;
            tDrawCfg.line_x2 = tDrawCfg.buffer_width /2+60-1 + i%100;
            tDrawCfg.line_y2 = tDrawCfg.buffer_height/2+60-1 + i%100;

            tDrawCfg.thickness = 3;
            tDrawCfg.color = LCD_COLOR_RGB(aRGBColorTest[i%udarraysize].R, aRGBColorTest[i%udarraysize].G, aRGBColorTest[i%udarraysize].B);
            lcd_draw_rec_in_buf(tDrawCfg);
          #endif



        }
        printf("lcd_draw_area_nonblocking test done~ (flush_async_callback_cnt=%d)\r\n", (unsigned int)flush_async_callback_cnt);
    #endif

    lcd_show_pic_loop();

    return;
}


void lcd_demo2( void * arg )
{

    uint32_t udarraysize;
    struct RGBColorTest {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    };

    struct RGBColorTest aRGBColorTest[] =
    {
        {255,   0,   0},
        {255, 128,   0},
        {255, 255,   0},
        {128, 255,   0},
        {  0, 255,   0},
        {  0, 255, 128},
        {  0, 255, 255},
        {  0, 128, 255},
        {  0,   0, 255},
        {128,   0, 255},
        {255,   0, 255},
        {  0,   0, 128},
    };

    lcd_init();
    lcd_async_callback_register(flush_async_callback2);

    g_stop_showing_pic_flg = 0;
    gLcdDemo2JDECKickDone = 0;
    gLcdDemo2JDECKickDoneCnt = 0;
    gLcdDemo2ShowAndKick = 0;
    xTaskCreate(DBI_ShowMjdecBand_Task, (char*)"DBI_ShowMjdecBand_Task - showing mjdec decoded & converted aRGB888 band to DBI", 5*1024, NULL, 10, &gvTaskHandle3);

    printf("\r\n/********************************************************************/\r\n");
    printf("Please load 320x480 YCC420 jpeg stream to 0x%08x for lcd_demo2!\r\n", REALCHIP_LCD_DEMO_RSVD_MEM_START_ADDR);
    printf("/********************************************************************/\r\n\r\n");
    
    udarraysize = (sizeof(aRGBColorTest)/sizeof(struct RGBColorTest));
    for(int i=0; i<0xfffffff; i++) {
        #if (LCD_ROTATE_FOR_480x320 == 0)
            /*LCD no rotation, 320x480*/
            lcd_draw_area_nonblocking(0, 0, LCD_W -1, LCD_H -1, LCD_COLOR_RGB(aRGBColorTest[i%udarraysize].R, aRGBColorTest[i%udarraysize].G, aRGBColorTest[i%udarraysize].B));
        #else
            /*LCD with rotation, 480x320*/
            lcd_draw_area_nonblocking(0, 0, LCD_H -1, LCD_W -1 , LCD_COLOR_RGB(aRGBColorTest[i%udarraysize].B, aRGBColorTest[i%udarraysize].G, aRGBColorTest[i%udarraysize].R));
        #endif
        gp_disp_drv_cb=(volatile uint32_t *)flush_async_callback2;
        while(gp_disp_drv_cb!= NULL)
        {
            vTaskDelay(10);
        }
        //printf("draw_area_nonblocking test=> flush_async_callback_cnt=%d)\r\n", (unsigned int)flush_async_callback_cnt);

        jpg_dec_and_disp();
        //vTaskDelay(10);

    }

    g_stop_showing_pic_flg = 1;
    vTaskDelay(100);
    
    printf("lcd_demo2 done ~\r\n");



    return;
}


static int cmd_lcd_demo(int argc, char **argv)
{
    xTaskCreate(lcd_demo, (char*)"lcd_demo task", 5*1024, NULL, 10, &gvTaskHandle);
    //lcd_demo();

    printf("lcd demo done\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_lcd_demo, lcd_demo, LCD Demo);

static int cmd_lcd_demo2(int argc, char **argv)
{
    xTaskCreate(lcd_demo2, (char*)"lcd_demo2 task using mjdec band decode", 5*1024, NULL, 10, &gvTaskHandle2);
    //lcd_demo();

    printf("lcd demo2 done\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_lcd_demo2, lcd_demo2, LCD Demo2- jpeg decode and display);

void demo_lcd_init(void)
{
    #if defined(CONFIG_IPRO6)
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DBI);
    #endif
}



int lcd_demo2_mjdec_dec_kick_done(void *usrdata, mjdec_frame_desc_t *frame_desc)
{
    if(frame_desc->ucIntType == MJDEC_HAL_INT_NORMAL)
    {
        gLcdDemo2JDECKickDone = 1;
        return 0;
    }


    /*block line mode*/
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)frame_desc->frame_start_addr_yy, (unsigned long)frame_desc->frame_size_yy);
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)frame_desc->frame_start_addr_uv, (unsigned long)frame_desc->frame_size_uv);

    //printf("mjdec block line kick int cnt %d, type %d, y buf: 0x%x 0x%x y buf size: %d, uv buf: 0x%x 0x%x uv buf size: %d\r\n", gLcdDemo2JDECKickDoneCnt, (unsigned int)frame_desc->ucIntType, 
    //    (unsigned int)frame_desc->frame_start_addr_yy, (unsigned int)frame_desc->frame_start_addr_yy +  (unsigned int)frame_desc->frame_size_yy, (unsigned int)frame_desc->frame_size_yy,
    //    (unsigned int)frame_desc->frame_start_addr_uv, (unsigned int)frame_desc->frame_start_addr_uv +  (unsigned int)frame_desc->frame_size_uv, (unsigned int)frame_desc->frame_size_uv);

    NV12_To_aRGB8888_Integer((uint8_t*) frame_desc->frame_start_addr_yy, (uint8_t*) frame_desc->frame_start_addr_uv, 
                                    (unsigned char*)g_udLcdDemo2aRGBBufAddr, LCD_W, 16); 
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)g_udLcdDemo2aRGBBufAddr, (unsigned long)LCD_W*16*4);
    gLcdDemo2JDECKickDoneCnt++;


    if(frame_desc->ucIntType == MJDEC_HAL_INT_NORMAL)
    {
        gLcdDemo2JDECKickDone = 1;
        //printf("KickDone 2, block line kick int cnt %d ok\r\n", gLcdDemo2JDECKickDoneCnt);
    }
    else    /*kick next round*/
    {
        //printf("ShowAndKick block line kick int cnt %d ok\r\n", gLcdDemo2JDECKickDoneCnt);
        gLcdDemo2ShowAndKick=1;
        //mjdec_kick();
    }
    return 0;
}

void DBI_ShowMjdecBand_Task( void * arg )
{
    while(g_stop_showing_pic_flg == 0)
    {
        while((gLcdDemo2JDECKickDoneCnt>0) && (gLcdDemo2JDECKickDoneCnt<=(LCD_H/16)))
        {
            if(gLcdDemo2ShowAndKick != 0)
            {
                gLcdDemo2ShowAndKick = 0;

                lcd_draw_picture_blocking(0, (gLcdDemo2JDECKickDoneCnt-1)*16, LCD_W-1, (gLcdDemo2JDECKickDoneCnt)*16-1, (lcd_color_t *)g_udLcdDemo2aRGBBufAddr);
                //printf("display block line kick int cnt %d ok\r\n", gLcdDemo2JDECKickDoneCnt);

                /*kick until last band*/
                if(gLcdDemo2JDECKickDoneCnt < (LCD_H/16))
                {
                    mjdec_kick();
                }
            }
            vTaskDelay(10);
        }
        vTaskDelay(10);
    }
    
    vTaskDelete(NULL); 
}

int jpg_dec_and_disp(void)
{
    int ret=0;
    int frm_size = 0;
    int hw_out_buf_size = 0;
    int img_width, img_height;

    mjdec_config_t config;
    uint8_t *alloc_buf_addr;
    uint8_t *alloc_hw_out_buf_addr;
    uint8_t *final_argb_out_buf_addr;


    config.width  = LCD_W;
    config.height = LCD_H;


    config.is_skip_header   = 1;
    config.yuv_mode         = MJDEC_HAL_FORMAT_YUV420_SP;
    config.out_u_order_even = 1;
    config.out_kick_mode_en = 1; 
    config.out_kick_mode_sel= 1;  /*0: single block mode. 1: block line mode*/
    config.input_buf_addr  = (uint8_t*)LCD_DEMO2_JPG_BIT_STREAM_BUF_ADDR;

#if 0    /*cpoy bit stream to tcm range because JPEG HW cannot access flash & wram address range.*/
    printf("Copy JPEG bit stream with size %d bytes from 0x%08x to 0x%08x!\r\n", sizeof(Avalokitasvara_320x480_420), 
                                                                                 (unsigned int)&Avalokitasvara_320x480_420[0],
                                                                                 (unsigned int)REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR);
    memcpy((void*)REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR, (void*)&Avalokitasvara_320x480_420[0], sizeof(Avalokitasvara_320x480_420));
#endif

    img_width  = COMMON_AlignCeil(config.width, 16);
    img_height = COMMON_AlignCeil(config.height, 16);
    frm_size = img_width * 16 * 4;   /*aRGB band output buffer size for block line mode with YUV420 output.*/

    /*portBYTE_ALIGNMENT_MASK is 0x7 ==> malloc at least will be 8byte aligned, add extra 24byte for achieving 32byte (CPU cache line size) align.*/
    alloc_buf_addr = pvPortMalloc(frm_size+32-8);    /*returned addr already aligned to portBYTE_ALIGNMENT_MASK, which is 0x7 for ipro6 & ipro7)*/


    //Kick Band Mode ==>  HW out and final out buffer (for output checksum) are different
    final_argb_out_buf_addr    = alloc_buf_addr;
    g_udLcdDemo2aRGBBufAddr = (uint32_t)final_argb_out_buf_addr;


    /*alloc buffer for kick mode mjdec HW output*/
    if(config.out_kick_mode_sel == 0)   /*single block out per kick*/
        img_width = 128;
    //else    /*1 block line out per kick*/
    //    img_width = img_width;

    img_height = 8;
    if (config.yuv_mode == MJDEC_HAL_FORMAT_YUV422_SP)   frm_size = YUV422_FRAME_SIZE( img_width, 8);
    else if (config.yuv_mode == MJDEC_HAL_FORMAT_YUV400) frm_size = YUV400_FRAME_SIZE( img_width, 8);
    else { frm_size = YUV420_Y_FRAME_SIZE(img_width, 16)+YUV420_UV_FRAME_SIZE(img_width, 16); img_height = 16;}

    hw_out_buf_size    = frm_size;

    alloc_hw_out_buf_addr = pvPortMalloc(frm_size+32-8);    /*returned addr already aligned to portBYTE_ALIGNMENT_MASK, which is 0x7 for ipro6 & ipro7)*/
    config.output_buf_addr_yy = (uint8_t *)COMMON_AlignCeil((unsigned long)alloc_hw_out_buf_addr, 32);
    config.output_buf_addr_uv = (uint8_t *)((uint32_t)config.output_buf_addr_yy + YUV400_FRAME_SIZE(img_width, img_height));

    
    //memset((void*)config.output_buf_addr_yy, 0xa5, hw_out_buf_size);
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)config.output_buf_addr_yy, (unsigned long)hw_out_buf_size);

    #if 1
        printf("=== LCD Demo2 Decode Info ===\r\n");
        printf("config.width              = %d\r\n",     config.width           );
        printf("config.height             = %d\r\n",     config.height          );
        printf("config.is_skip_header     = %d\r\n",     config.is_skip_header  );
        printf("config.yuv_mode           = %d\r\n",     config.yuv_mode        );
        printf("config.out_u_order_even   = %d\r\n",     config.out_u_order_even);
        printf("config.yuv_mode           = %d\r\n",     config.yuv_mode        );
        printf("config.output_buf_addr_yy = 0x%08x\r\n", (unsigned int)config.output_buf_addr_yy);
        printf("config.output_buf_addr_uv = 0x%08x\r\n", (unsigned int)config.output_buf_addr_uv);
        printf("config.input_buf_addr     = 0x%08x\r\n", (unsigned int)config.input_buf_addr    );
        printf("config.out_kick_mode_en   = 0x%08x\r\n", (unsigned int)config.out_kick_mode_en  );
        printf("config.out_kick_mode_sel  = 0x%08x\r\n", (unsigned int)config.out_kick_mode_sel );
        printf("final_argb_out_buf_addr   = 0x%08x\r\n", (unsigned int)final_argb_out_buf_addr   );
        printf("align_hw_out_buf_addr     = 0x%08x\r\n", (unsigned int)config.output_buf_addr_yy);
        printf("hw_out_buf_size           = 0x%08x\r\n", (unsigned int)hw_out_buf_size      );
    #endif
    memcpy((void*)&g_tLcdDemo2MjdecCfg, (void*)&config, sizeof(mjdec_config_t));

    gLcdDemo2JDECDone = 0;
    gLcdDemo2JDECKickDone = 0;
    gLcdDemo2JDECKickDoneCnt = 0;
    gLcdDemo2ShowAndKick = 0;

    /*out_put_fmt:yuv420:yyy...yyuv...uv(NV12)
      out_put_fmt:yuv400:yyy...yy
      out_put_fmt:yuv422:yyy...yyuv...uv*/
    ret = mjdec_init(&config);
    if(ret<0)
    {
        printf("[mjdec][ERR] Initialization error occurred!!!\r\n");

        return ret;
    }


    #if 0    /*HW direct to final output buffer*/
        /*set next block line start address*/
        mjdec_set_out_addr(g_udLcdDemo2CpyDestStartAddr_yy, g_udLcdDemo2CpyDestStartAddr_uv);
    #endif

    mjdec_set_callback(lcd_demo2_mjdec_dec_kick_done, NULL);

    /* Clean cache for input buffer (PSRAM) before hardware DMA read */
    L1C_DCACHE_CLEAN_RANGE((unsigned long)config.input_buf_addr, (unsigned long)LCD_DEMO2_JPG_BIT_STREAM_BUF_ADDR);
    mjdec_start_decoder(config.input_buf_addr);

    while (gLcdDemo2JDECKickDone == 0)
    {
        vTaskDelay(100);
    }



    if(alloc_buf_addr != alloc_hw_out_buf_addr)
        free(alloc_hw_out_buf_addr);
    free(alloc_buf_addr);
    
    mjdec_disable();
    ret = mjdec_deinit();

    return ret;
}

/**
 * @brief Converts NV12 (Semi-Planar YUV420) to aRGB8888 using pure integer fixed-point math.
 * @param y_plane   Pointer to the input Y plane (Size: width * height bytes).
 * @param uv_plane  Pointer to the input interleaved UV plane (Size: width * height / 2 bytes).
 *                  Memory layout must be: U0, V0, U1, V1, U2, V2...
 * @param rgb_out   Pointer to the output 4-byte aRGB buffer (Size: width * height * 4 bytes).
 *                  Memory sequence: [Alpha, R, G, B, Alpha, R, G, B, ...]
 * @param width     Width of the image.
 * @param height    Height of the image.
 */
void NV12_To_aRGB8888_Integer(const uint8_t* y_plane, const uint8_t* uv_plane,
                              unsigned char* rgb_out, int width, int height) 
{
    int rgb_index = 0;

    for (int y = 0; y < height; ++y) {
        // Find the start index of the current row in the Y plane
        int y_row_idx = y * width;
        
        // Find the start index of the current row in the UV plane.
        // Since 2 vertical Y pixels share 1 UV line, we use (y / 2).
        // Since UV is interleaved, one row still spans 'width' bytes in memory.
        int uv_row_idx = (y / 2) * width;

        for (int x = 0; x < width; ++x) {
            int y_idx = y_row_idx + x;
            
            // 2 horizontal Y pixels share 1 pair of U/V.
            // The bitwise operation '(x & ~1)' forces the index to an even number (U position).
            int uv_idx = uv_row_idx + (x & ~1);

            int Y = y_plane[y_idx];
            int U = uv_plane[uv_idx] - 128;     // U is at the even byte
            int V = uv_plane[uv_idx + 1] - 128; // V is at the odd byte

            // Fixed-point scaling (Scale up by 1024 / Left shift by 10 bits)
            // BT.601 constants scaled by 1024: 1.402->1436, 0.344->352, 0.714->731, 1.772->1815
            int y_scaled = Y << 10;

            int r_scaled = y_scaled + (1436 * V);
            int g_scaled = y_scaled - (352 * U) - (731 * V);
            int b_scaled = y_scaled + (1815 * U);

            // Shift right by 10 bits to scale back down, then clamp to 0 - 255
            int r = r_scaled >> 10;
            int g = g_scaled >> 10;
            int b = b_scaled >> 10;

            if (r < 0) r = 0; else if (r > 255) r = 255;
            if (g < 0) g = 0; else if (g > 255) g = 255;
            if (b < 0) b = 0; else if (b > 255) b = 255;

            // Write 4 bytes in aRGB sequence
            rgb_out[rgb_index++] = (unsigned char)b;   // Blue
            rgb_out[rgb_index++] = (unsigned char)g;   // Green
            rgb_out[rgb_index++] = (unsigned char)r;   // Red
            rgb_out[rgb_index++] = 255;                // Alpha channel (Fully opaque)
        }
    }
}



