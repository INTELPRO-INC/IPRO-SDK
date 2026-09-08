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

/*abgr image address for disp_show_pic_loop*/
uint8_t *g_pucAddrBass = (uint8_t *)0x1e900000;
uint8_t *g_pucAddrCurr = (uint8_t *)0x1e900000;
uint8_t *g_pucAddrNext = (uint8_t *)0x1e900000;
SemaphoreHandle_t g_tDbiShowPicNotify = NULL;
static volatile uint32_t *gp_disp_drv_cb = NULL;
int     lcd_draw_cnt=0;/*abner test*/
int     flush_async_callback_cnt=0;/*abner test*/
volatile int g_stop_showing_pic_flg=0;
TaskHandle_t    gvTaskHandle;

void flush_async_callback()
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

        #if (ILI9488_SPI_ROTATE_FOR_480x320 == 0)
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

    #if 0
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
            #if (ILI9488_SPI_ROTATE_FOR_480x320 == 0)
                /*LCD no rotation, 320x480*/
                lcd_draw_area_nonblocking(0, 0, ILI9488_SPI_W-1, ILI9488_SPI_H-1, LCD_COLOR_RGB(aRGBColorTest[i%udarraysize].R, aRGBColorTest[i%udarraysize].G, aRGBColorTest[i%udarraysize].B));
            #else
                /*LCD with rotation, 480x320*/
                lcd_draw_area_nonblocking(0, 0, ILI9488_SPI_H-1, ILI9488_SPI_W-1 , LCD_COLOR_RGB(aRGBColorTest[i%udarraysize].B, aRGBColorTest[i%udarraysize].G, aRGBColorTest[i%udarraysize].R));
            #endif
            gp_disp_drv_cb=&temp;
            while(gp_disp_drv_cb!= NULL)
            {
                vTaskDelay(10);
            }
            printf("draw_area_nonblocking test=> flush_async_callback_cnt=%d)\r\n", (unsigned int)flush_async_callback_cnt);
            //vTaskDelay(100);

          #if 1 /*draw one frame*/
            #if (ILI9488_SPI_ROTATE_FOR_480x320 == 0)
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
            #if (ILI9488_SPI_ROTATE_FOR_480x320 == 0)
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

          #if 1
            LCD_DRAW_IN_BUF_CFG tDrawCfg;

            tDrawCfg.buffer_addr = (lcd_color_t*)g_pucAddrCurr;
            #if (ILI9488_SPI_ROTATE_FOR_480x320 == 0)
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



static int cmd_lcd_demo(int argc, char **argv)
{
    xTaskCreate(lcd_demo, (char*)"lcd_demo task", 5*1024, NULL, 10, &gvTaskHandle);
    //lcd_demo();

    printf("lcd demo done\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_lcd_demo, lcd_demo, LCD Demo);
