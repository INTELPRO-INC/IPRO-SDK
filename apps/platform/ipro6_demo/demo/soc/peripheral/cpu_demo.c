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

#include <generated/autoconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include <FreeRTOS.h>
#include <hal_mtimer.h>
#include <task.h>

//#define OPUS_PERF_TEST
#ifdef OPUS_PERF_TEST
#include "include/test.h"
#include "opus.h"

static uint8_t *opus_enc_buffer = NULL;
static opus_int16 *opus_dec_buffer = NULL;
static int16_t src_ptr[320];
static uint8_t enc_ptr[640];

static void int_to_char(opus_uint32 i, unsigned char ch[4])
{
    ch[0] = i>>24;
    ch[1] = (i>>16)&0xFF;
    ch[2] = (i>>8)&0xFF;
    ch[3] = i&0xFF;
}

__attribute__((unused)) static opus_uint32 char_to_int(unsigned char ch[4])
{
    return ((opus_uint32)ch[0]<<24) | ((opus_uint32)ch[1]<<16)
         | ((opus_uint32)ch[2]<< 8) |  (opus_uint32)ch[3];
}

void audio_opus_enc_proc(void *arg)
{
    printf("Encode pcm to opus\r\n");
    OpusEncoder *enc;
    OpusDecoder *dec;
    int err;
    //L1C_Set_Way_Disable(0xFF);

    __disable_irq();

    dec = opus_decoder_create(16000, 1, &err);
    if(err!=OPUS_OK || dec==NULL)
        printf("decoder create fail\r\n");

    //Create a new opus encoder state
    enc = opus_encoder_create(16000, 1, OPUS_APPLICATION_RESTRICTED_LOWDELAY , &err);
    if(err!=OPUS_OK || enc==NULL)
        printf("encoder create fail\r\n");
    //Perform a CTL function on an Opus encoder.
    //;
    opus_encoder_ctl(enc, OPUS_SET_BITRATE(16000));
    opus_encoder_ctl(enc, OPUS_SET_BANDWIDTH(OPUS_AUTO));
    opus_encoder_ctl(enc, OPUS_SET_VBR(1));
    opus_encoder_ctl(enc, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    opus_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(0));
    opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(0));
    opus_encoder_ctl(enc, OPUS_SET_INBAND_FEC(0));
    opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(1));
    opus_encoder_ctl(enc, OPUS_SET_DTX(0));
    opus_encoder_ctl(enc, OPUS_SET_PACKET_LOSS_PERC(0));
    opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_20_MS));
    opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(16));

    if (opus_dec_buffer)
        vPortFree(opus_dec_buffer);
    opus_dec_buffer = pvPortMalloc(audio_32k_len);

    if (opus_enc_buffer)
        vPortFree(opus_enc_buffer);
    opus_enc_buffer = pvPortMalloc(audio_32k_len);
    memset(opus_dec_buffer, 0, audio_32k_len);

   if (opus_dec_buffer) {
        int frame_remain = audio_32k_len;
        int write_size = 0;
        int dec_len = 0;
        int dec_size = 0;
        opus_uint32 enc_final_range;
        opus_int16 *cur_ptr = (opus_int16 *)audio_32k;
        opus_int16 *wdecptr = opus_dec_buffer;
        uint8_t *wencptr = opus_enc_buffer;
        uint32_t start_tick_enc, end_tick_enc;
        uint32_t start_tick_dec, end_tick_dec;
        int write_len = 0;
        while (frame_remain >= 640) {
            memcpy(src_ptr, cur_ptr, 640);
            start_tick_enc = mtimer_get_time_us();
            write_len = opus_encode(enc, src_ptr, 320, enc_ptr, 640);
            end_tick_enc = mtimer_get_time_us();
            opus_encoder_ctl(enc, OPUS_GET_FINAL_RANGE(&enc_final_range));
            int_to_char(write_len, wencptr);
            wencptr += 4;
            int_to_char(enc_final_range, wencptr);
            wencptr += 4;
            memcpy(wencptr, enc_ptr, write_len);
            start_tick_dec = mtimer_get_time_us();
            dec_len = opus_decode(dec, enc_ptr, write_len, src_ptr, 320, 0);
            if(dec_len!=320) {
               printf("dec_len = %d\r\n", dec_len);
               break;
            }
            end_tick_dec = mtimer_get_time_us();
            memcpy(wdecptr, src_ptr, dec_len);

            write_size += write_len;
            dec_size += dec_len;
            cur_ptr += 320;
            frame_remain -= 640;
            wencptr += write_len;
            wdecptr += dec_len;
            printf("encode frame size %d, frame_remain = %d, total = %u, time enc: %lu, time dec: %lu\r\n", write_len, frame_remain, write_size, end_tick_enc - start_tick_enc, end_tick_dec - start_tick_dec);
        }
        printf("dump binary memory src.raw 0x%lx 0x%lx\r\n", (uint32_t) audio_32k, (uint32_t) audio_32k + audio_32k_len);
        printf("dump binary memory test.opus 0x%lx 0x%lx\r\n", (uint32_t) opus_enc_buffer, (uint32_t) opus_enc_buffer + write_size);
        printf("dump binary memory test.raw 0x%lx 0x%lx\r\n", (uint32_t) opus_dec_buffer, (uint32_t) opus_dec_buffer + dec_size);
    } else {
        printf("malloc psram fail\r\n");
    }
    __enable_irq();
//     printf("opus encode time: %lu us, data size: %u\r\n", end_tick - start_tick, audio_buf_pos);
    vTaskDelete(NULL);
}


static int audio_opus_enc(int argc, char **argv)
{
    xTaskCreate(audio_opus_enc_proc, "au_opus", 8192, NULL, 10, NULL);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(audio_opus_enc, opus, opus Test);
#endif

#ifdef CONFIG_DEMO_DHRYSTONE
static int cmd_dhrystone_test(int argc, char **argv)
{
    extern void dhrystone_test(void);
    dhrystone_test();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dhrystone_test, dhrystone, dhrystone test);
#endif

/* Note: cache_test command is registered in cache_test.c */
