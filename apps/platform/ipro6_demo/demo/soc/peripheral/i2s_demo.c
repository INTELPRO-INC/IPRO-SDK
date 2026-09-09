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

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "shell.h"
#include <hal_dma.h>
#include <hal_i2c.h>
#include <hal_i2s.h>
/* Filesystem - EVFS POSIX API */
#include "evfs.h"
#include "evfs_posix.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SUPPORT_I2S_TEST

#ifdef SUPPORT_I2S_TEST

/* ADAU internal registers */
#define LINE_IN 0
#define MIC 1
#define LINE_MIC 2

enum audio_regs {
	R0_CLOCK_CONTROL								= 0x4000,
	R1_PLL_CONTROL 									= 0x4002,
	R2_DIGITAL_MIC_JACK_DETECTION_CONTROL 			= 0x4008,
	R3_RECORD_POWER_MANAGEMENT						= 0x4009,
	R4_RECORD_MIXER_LEFT_CONTROL_0 					= 0x400A,
	R5_RECORD_MIXER_LEFT_CONTROL_1 					= 0x400B,
	R6_RECORD_MIXER_RIGHT_CONTROL_0 				= 0x400C,
	R7_RECORD_MIXER_RIGHT_CONTROL_1 				= 0x400D,
	R8_LEFT_DIFFERENTIAL_INPUT_VOLUME_CONTROL 		= 0x400E,
	R9_RIGHT_DIFFERENTIAL_INPUT_VOLUME_CONTROL 		= 0x400F,
	R10_RECORD_MICROPHONE_BIAS_CONTROL 				= 0x4010,
	R11_ALC_CONTROL_0								= 0x4011,
	R12_ALC_CONTROL_1								= 0x4012,
	R13_ALC_CONTROL_2								= 0x4013,
	R14_ALC_CONTROL_3								= 0x4014,
	R15_SERIAL_PORT_CONTROL_0 						= 0x4015,
	R16_SERIAL_PORT_CONTROL_1 						= 0x4016,
	R17_CONVERTER_CONTROL_0 						= 0x4017,
	R18_CONVERTER_CONTROL_1 						= 0x4018,
	R19_ADC_CONTROL									= 0x4019,
	R20_LEFT_INPUT_DIGITAL_VOLUME 					= 0x401A,
	R21_RIGHT_INPUT_DIGITAL_VOLUME 					= 0x401B,
	R22_PLAYBACK_MIXER_LEFT_CONTROL_0 				= 0x401C,
	R23_PLAYBACK_MIXER_LEFT_CONTROL_1 				= 0x401D,
	R24_PLAYBACK_MIXER_RIGHT_CONTROL_0 				= 0x401E,
	R25_PLAYBACK_MIXER_RIGHT_CONTROL_1 				= 0x401F,
	R26_PLAYBACK_LR_MIXER_LEFT_LINE_OUTPUT_CONTROL 	= 0x4020,
	R27_PLAYBACK_LR_MIXER_RIGHT_LINE_OUTPUT_CONTROL = 0x4021,
	R28_PLAYBACK_LR_MIXER_MONO_OUTPUT_CONTROL 		= 0x4022,
	R29_PLAYBACK_HEADPHONE_LEFT_VOLUME_CONTROL 		= 0x4023,
	R30_PLAYBACK_HEADPHONE_RIGHT_VOLUME_CONTROL 	= 0x4024,
	R31_PLAYBACK_LINE_OUTPUT_LEFT_VOLUME_CONTROL 	= 0x4025,
	R32_PLAYBACK_LINE_OUTPUT_RIGHT_VOLUME_CONTROL 	= 0x4026,
	R33_PLAYBACK_MONO_OUTPUT_CONTROL 				= 0x4027,
	R34_PLAYBACK_POP_CLICK_SUPPRESSION 				= 0x4028,
	R35_PLAYBACK_POWER_MANAGEMENT 					= 0x4029,
	R36_DAC_CONTROL_0 								= 0x402A,
	R37_DAC_CONTROL_1 								= 0x402B,
	R38_DAC_CONTROL_2 								= 0x402C,
	R39_SERIAL_PORT_PAD_CONTROL 					= 0x402D,
	R40_CONTROL_PORT_PAD_CONTROL_0 					= 0x402F,
	R41_CONTROL_PORT_PAD_CONTROL_1 					= 0x4030,
	R42_JACK_DETECT_PIN_CONTROL 					= 0x4031,
	R67_DEJITTER_CONTROL 							= 0x4036,
	R48_GPIO0_PIN_CONTROL 							= 0x40C6,
	R58_SERIAL_INPUT_ROUTE_CONTROL					= 0x40F2,
	R59_SERIAL_OUTPUT_ROUTE_CONTROL					= 0x40F3,
	R60_SERIAL_DATA_GPIO_PIN_CONFIGURATION			= 0x40F4,
	R61_DSP_ENABLE									= 0x40F5,
	R62_DSP_RUN										= 0x40F6,
	R63_DSP_SLEW_MODES								= 0x40F7,
	R64_SERIAL_PORT_SAMPLING_RATE 					= 0x40F8,
	R65_CLOCK_ENABLE_0 								= 0x40F9,
	R66_CLOCK_ENABLE_1 								= 0x40FA
};

#define I2C_SLV_ADDR    (0x3b)
#define AUD_I2C_ID  (I2C1_INDEX)

static int i2c_read(uint32_t sub_addr, uint8_t *data, uint16_t size)
{
    i2c_msg_t i2c_msg;

    i2c_msg.buf = data;
    i2c_msg.flags = SUB_ADDR_2BYTE | I2C_RD;
    i2c_msg.len = size;
    i2c_msg.slaveaddr = I2C_SLV_ADDR;
    i2c_msg.subaddr = (sub_addr >> 8) | ((sub_addr & 0xff) << 8);

    if (i2c_transfer(AUD_I2C_ID, &i2c_msg, 1) != 0) {
        printf("I2C Read Fail\r\n");
        return 1;
    }
#if 0
    printf("Read 0x%lx, Result: %x", sub_addr, data[0]);
    for (int i = 1; i < size; i++)
        printf(", %x", data[i]);
    printf("\r\n");
#endif
    return 0;
}

static int Adau1761_RegWrite(uint32_t sub_addr, uint8_t *data, uint16_t size)
{
    i2c_msg_t i2c_msg;

    i2c_msg.buf = data;
    i2c_msg.flags = SUB_ADDR_2BYTE | I2C_WR;
    i2c_msg.len = size;
    i2c_msg.slaveaddr = I2C_SLV_ADDR;
    i2c_msg.subaddr = (sub_addr >> 8) | ((sub_addr & 0xff) << 8);

    if (i2c_transfer(AUD_I2C_ID, &i2c_msg, 1) != 0) {
        printf("I2C Write Fail\r\n");
        return 1;
    }
#if 0
    uint8_t i2c_data_rxbuf[1];
    printf("write %lx\r\n", sub_addr);
    arch_delay_ms(50);

    i2c_read(sub_addr, i2c_data_rxbuf, 1);
    printf("[0x%lx] after write 0x%x rb = 0x%x\r\n", sub_addr, data[0], i2c_data_rxbuf[0]);
#endif
    return 0;
}


void Adau1761_Codec_Init(void)
{
	static unsigned char u8TxData[8], u8RxData[6];

	// Disable Core Clock
	u8TxData[0] = 0x0E;
	Adau1761_RegWrite(R0_CLOCK_CONTROL, u8TxData, 1);

	/* 	MCLK = 24 MHz
		R = 0100 = 4

		PLL required output = 1024x48 KHz
		(PLLout)			= 49.152 MHz

		PLLout/MCLK			= 49.152 MHz/24 MHz
							= 2 */

	// Write 6 bytes to R1 @ CODEC's Config Register address: 0x4002
	u8TxData[0] = 0x00; // byte 6 - M[15:8]
	u8TxData[1] = 0x00; // byte 5 - M[7:0]
	u8TxData[2] = 0x00; // byte 4 - N[15:8]
	u8TxData[3] = 0x00; // byte 3 - N[7:0]
	u8TxData[4] = 0x10; // byte 2 - 7 = reserved, bits 6:3 = R[3:0], 2:1 = X[1:0], 0 = PLL operation mode
	u8TxData[5] = 0x01; // byte 1 - 7:2 = reserved, 1 = PLL Lock, 0 = Core clock enable

	// Write bytes to PLL Control register R1 @ 0x4002
	Adau1761_RegWrite(R1_PLL_CONTROL, u8TxData, 6);

	// Poll PLL Lock bit
	do {
		i2c_read(R1_PLL_CONTROL, u8RxData, 6);
        vTaskDelay(100);
	}
	while((u8RxData[5] & 0x02) == 0); // while not locked

    u8TxData[0] = 0x0F;
	Adau1761_RegWrite(R0_CLOCK_CONTROL, u8TxData, 1);	// 1111
												// bit 3:		CLKSRC = PLL Clock input
												// bits 2:1:	INFREQ = 1024 x fs
												// bit 0:		COREN = Core Clock enabled

	//Initialize ADAU1761 control registers. (Refer to Page 51 of the ADAU1761 data sheet)

	/*Initialize CODEC I2S port*/
    u8TxData[0] = 0x00;
	Adau1761_RegWrite(R16_SERIAL_PORT_CONTROL_1, u8TxData, 1);

	/* Set ADC/DAC sampling rate - 16kHz*/
    u8TxData[0] = 0x03;
	Adau1761_RegWrite(R17_CONVERTER_CONTROL_0, u8TxData, 1);
	Adau1761_RegWrite(R64_SERIAL_PORT_SAMPLING_RATE, u8TxData, 1);

	/*ADC/DAC CNTL - 2 ADC/DAC enabled; others set to default */
    u8TxData[0] = 0x13;
	Adau1761_RegWrite(R19_ADC_CONTROL, u8TxData, 1);
    u8TxData[0] = 0x03;
	Adau1761_RegWrite(R36_DAC_CONTROL_0, u8TxData, 1);

	/* No POWER MANAGEMENT set here - all enabled for now */
    u8TxData[0] = 0x03;
	Adau1761_RegWrite(R35_PLAYBACK_POWER_MANAGEMENT, u8TxData, 1);

    /* Input/Output routes of ADC/DAC, clock control */
    u8TxData[0] = 0x01;
	Adau1761_RegWrite(R58_SERIAL_INPUT_ROUTE_CONTROL, u8TxData, 1);
    u8TxData[0] = 0x01;
	Adau1761_RegWrite(R59_SERIAL_OUTPUT_ROUTE_CONTROL, u8TxData, 1);
    u8TxData[0] = 0x7F;
	Adau1761_RegWrite(R65_CLOCK_ENABLE_0, u8TxData, 1);
    u8TxData[0] = 0x03;
	Adau1761_RegWrite(R66_CLOCK_ENABLE_1, u8TxData, 1);


	/* Mixer - Enable's sources that influence the play back/Audio Input path - Refer Page 29 and 35 */

	/* Audio Input Mixer */
    u8TxData[0] = 0x01;
	Adau1761_RegWrite(R4_RECORD_MIXER_LEFT_CONTROL_0, u8TxData, 1); //Mixer 1 Enable.
    u8TxData[0] = 0x01;
	Adau1761_RegWrite(R6_RECORD_MIXER_RIGHT_CONTROL_0, u8TxData, 1);//Mixer 2 Enable.

	/* Play back Path Mixer to the DAC's */
    u8TxData[0] = 0x21;
	Adau1761_RegWrite(R22_PLAYBACK_MIXER_LEFT_CONTROL_0, u8TxData, 1);
    u8TxData[0] = 0x41;
	Adau1761_RegWrite(R24_PLAYBACK_MIXER_RIGHT_CONTROL_0, u8TxData, 1);
    u8TxData[0] = 0x03;
	Adau1761_RegWrite(R26_PLAYBACK_LR_MIXER_LEFT_LINE_OUTPUT_CONTROL, u8TxData, 1);
    u8TxData[0] = 0x09;
	Adau1761_RegWrite(R27_PLAYBACK_LR_MIXER_RIGHT_LINE_OUTPUT_CONTROL, u8TxData, 1);

	/* Volume Control Options */

	/* HPH OUT Vol Control - Range: 0x03 - 0xFF (-57dB to 6dB) */
    u8TxData[0] = 0xE7;
	Adau1761_RegWrite(R29_PLAYBACK_HEADPHONE_LEFT_VOLUME_CONTROL, u8TxData, 1);
    u8TxData[0] = 0xE7;
	Adau1761_RegWrite(R30_PLAYBACK_HEADPHONE_RIGHT_VOLUME_CONTROL, u8TxData, 1);

	/* LINE OUT Vol Control - Range: 0x03 - 0xFF (-57dB to 6dB) */
    u8TxData[0] = 0xFF;
	Adau1761_RegWrite(R31_PLAYBACK_LINE_OUTPUT_LEFT_VOLUME_CONTROL, u8TxData, 1);
    u8TxData[0] = 0xFF;
	Adau1761_RegWrite(R32_PLAYBACK_LINE_OUTPUT_RIGHT_VOLUME_CONTROL, u8TxData, 1);

    printf("adau1761 codec init done\r\n");
}

/* Refer Page 29 - Record Signal Path */
void Adau1761_InSelect(unsigned short In_Sel, unsigned short L_In_Vol, unsigned short R_In_Vol){

    uint8_t val;
	switch(In_Sel){

	case MIC:
			/* MIC configurations - Refer Page 30 */
            val = L_In_Vol;
			Adau1761_RegWrite(R8_LEFT_DIFFERENTIAL_INPUT_VOLUME_CONTROL, &val, 1); //Set Input Volume - Check datasheet for more options.
            val = R_In_Vol;
			Adau1761_RegWrite(R9_RIGHT_DIFFERENTIAL_INPUT_VOLUME_CONTROL, &val, 1);
            val = 0x1;
			Adau1761_RegWrite(R10_RECORD_MICROPHONE_BIAS_CONTROL, &val, 1); // Bias Control enabled and set to default.
            val = 0x13;
			Adau1761_RegWrite(R11_ALC_CONTROL_0, &val, 1); // ALC controls PGA - Here its set to stereo.
            val = 0x10;
			Adau1761_RegWrite(R5_RECORD_MIXER_LEFT_CONTROL_1, &val, 1); //20dB LDBOOST, Line In Disabled.
            val = 0x10;
			Adau1761_RegWrite(R7_RECORD_MIXER_RIGHT_CONTROL_1, &val, 1);//20dB LD Boost, Line In Disabled.
			break;
	case LINE_IN:
			/* Line IN and LD Boost (output of PGA) Configurations */
            val = 0x7;
			Adau1761_RegWrite(R5_RECORD_MIXER_LEFT_CONTROL_1, &val, 1); //Mute Mic, Enable Line In.
            val = 0x7;
			Adau1761_RegWrite(R7_RECORD_MIXER_RIGHT_CONTROL_1, &val, 1);//Mute Mic, Enable Line In.
			break;

	case LINE_MIC:
			/* MIC configurations - Refer Page 30 */
            val = L_In_Vol;
			Adau1761_RegWrite(R8_LEFT_DIFFERENTIAL_INPUT_VOLUME_CONTROL, &val, 1);
            val = R_In_Vol;
			Adau1761_RegWrite(R9_RIGHT_DIFFERENTIAL_INPUT_VOLUME_CONTROL, &val, 1);
            val = 0x1;
			Adau1761_RegWrite(R10_RECORD_MICROPHONE_BIAS_CONTROL, &val, 1); // Bias Control enabled and set to default.
            val = 0x3;
			Adau1761_RegWrite(R11_ALC_CONTROL_0, &val, 1); // ALC controls PGA - Here its set to stereo.
			/* Line IN and LD Boost (output of PGA) Configurations */
            val = 0x17;
			Adau1761_RegWrite(R5_RECORD_MIXER_LEFT_CONTROL_1, &val, 1); //Zero gain LD Boost, Enable Line In.
            val = 0x17;
			Adau1761_RegWrite(R7_RECORD_MIXER_RIGHT_CONTROL_1, &val, 1);//Zero gain LD Boost, Enable Line In.
			break;

	default:
		printf("Input Path Select Exception\n");
	}
}

#define AUD_RECV_BUF_LEN   (16*1024*2*4*10)
#define AUD_BUFFER_LEN     (4096)
static uint8_t *gAudReceived;
static uint8_t *gAudTXBuf, *gAudRXBuf;
static int gAudRecvLen = 0;
static int gAudRecvFull = 0;
static int gAudSent = 0;
static int gAudLimit = 0;
static uint8_t *gAudioSrc = NULL;
static void i2s_aud_cb_tx(void *dev, uint32_t flag)
{
#if 1
    static int aud_tx_offset = 0;
    uint8_t *audptr = gAudTXBuf + aud_tx_offset;

#ifdef SUPPORT_I2S_TEST
    if (gAudSent + AUD_BUFFER_LEN > gAudLimit)
        gAudSent = 0;
    memcpy(audptr, gAudioSrc + gAudSent, AUD_BUFFER_LEN);
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)audptr, AUD_BUFFER_LEN);
    gAudSent += AUD_BUFFER_LEN;
    if (aud_tx_offset == 0)
        aud_tx_offset = aud_tx_offset + AUD_BUFFER_LEN;
    else
        aud_tx_offset = 0;
#endif
#else
    printf("tx callback\r\n");
#endif
}


static void i2s_aud_cb_rx(void *dev, uint32_t flag)
{
    static int aud_rx_offset = 0;
    uint8_t *audptr = gAudRXBuf + aud_rx_offset;
    if (gAudRecvLen + AUD_BUFFER_LEN < AUD_RECV_BUF_LEN) {
        L1C_DCACHE_INVALID_RANGE((uintptr_t)audptr, AUD_BUFFER_LEN);
        memcpy(gAudReceived+gAudRecvLen, audptr, AUD_BUFFER_LEN);
        gAudRecvLen += AUD_BUFFER_LEN;
    } else if (gAudRecvFull == 0) {
        gAudRecvFull = 1;
        printf("Audio RX Full: 0x%lx 0x%lx (%d)\r\n", (uint32_t)gAudReceived, (uint32_t)gAudReceived + gAudRecvLen, gAudRecvLen);
        printf("dump binary memory rec.pcm 0x%lx 0x%lx\r\n", (uint32_t)gAudReceived, (uint32_t)gAudReceived + gAudRecvLen);
    }
    if (aud_rx_offset == 0)
        aud_rx_offset = aud_rx_offset + AUD_BUFFER_LEN;
    else
        aud_rx_offset = 0;
}

int cmd_i2s_test(int argc, char **argv)
{
    i2s_device_t i2sDev = {
        .id = 0,
        .iis_mode = I2S_MODE_MASTER,
        .interface_mode = I2S_MODE_STD,
        .sampl_freq_hz = 16 * 1000,
        .channel_num = I2S_FS_CHANNELS_NUM_2,
        .frame_size = I2S_FRAME_LEN_32,
        .data_size = I2S_DATA_LEN_24,
        .fifo_threshold = 3,
    };
    dma_device_t i2s_rx_dma_dev = {
        .id = DMA0_ID,
        .ch = DMA0_CH0_INDEX,
        .transfer_mode = DMA_LLI_PINGPONG_MODE,
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_I2S_RX,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR4,
        .dst_burst_size = DMA_BURST_INCR4,
        .src_width = DMA_TRANSFER_WIDTH_32BIT,
        .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .lli_cfg = NULL,
        .callback = i2s_aud_cb_rx,
    };
    dma_device_t i2s_tx_dma_dev = {
        .id = DMA0_ID,
        .ch = DMA0_CH1_INDEX,
        .transfer_mode = DMA_LLI_PINGPONG_MODE,
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_I2S_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_size = DMA_BURST_INCR4,
        .dst_burst_size = DMA_BURST_INCR4,
        .src_width = DMA_TRANSFER_WIDTH_32BIT,
        .dst_width = DMA_TRANSFER_WIDTH_32BIT,
        .lli_cfg = NULL,
        .callback = i2s_aud_cb_tx,
    };

    Adau1761_Codec_Init();

    Adau1761_InSelect(MIC, 0xFF, 0xFF);

    arch_delay_ms(1000);
#ifdef SUPPORT_I2S_TEST
    hal_i2s_setConfig(&i2sDev);
    hal_i2s_init(0, COMMON_CAPS_DMA_TX | COMMON_CAPS_DMA_RX);

    dma_setConfig(&i2s_rx_dma_dev);
    dma_init(DMA0_ID, i2s_rx_dma_dev.ch);
    dma_ioctl(DMA0_ID, i2s_rx_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    hal_i2s_control(0, I2S_CTRL_ATTACH_RX_DMA, (void *)(uintptr_t)i2s_rx_dma_dev.ch);

    dma_setConfig(&i2s_tx_dma_dev);
    dma_init(DMA0_ID, i2s_tx_dma_dev.ch);
    dma_ioctl(DMA0_ID, i2s_tx_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    hal_i2s_control(0, I2S_CTRL_ATTACH_TX_DMA, (void *)(uintptr_t)i2s_tx_dma_dev.ch);
    memset(gAudReceived, 0, AUD_RECV_BUF_LEN);

    struct stat s;
    int ret = stat("/sdcard/test_audio.pcm", &s);
    if (ret != 0) {
        printf("stat audio file fail, ret = %d\r\n", ret);
        return 0;
    }
    int fd = open("/sdcard/test_audio.pcm", O_RDONLY);
    if (fd < 0) {
        printf("Cannot read audio file from sdcard\r\n");
        return 0;
    }
    gAudLimit = s.st_size;
    gAudioSrc = pvPortMalloc(gAudLimit);
    printf("Reading %u bytes from file...\r\n", gAudLimit);
    int read_len = read(fd, gAudioSrc, gAudLimit);
    if (read_len != gAudLimit) {
        printf("read fail, target: %u real: %u\r\n", gAudLimit, read_len);
        vPortFree(gAudioSrc);
        close(fd);
        return 0;
    }

    memcpy(gAudTXBuf, gAudioSrc, AUD_BUFFER_LEN*2);
    gAudSent = AUD_BUFFER_LEN*2;

    hal_i2s_read(0, gAudRXBuf, AUD_BUFFER_LEN);
    hal_i2s_write(0, gAudTXBuf, AUD_BUFFER_LEN);

    // IPRO8 DMA/I2S Init
#endif
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_test, i2s_test, I2S Test);

int cmd_i2s_vol_test(int argc, char **argv)
{
    int vol = 0xE7;
    if (argc == 2) {
        vol = atoi(argv[1]);
        if (vol > 63)
            vol = 63;
        vol = (vol << 2) | 0x3;
    }
    /* HPH OUT Vol Control - Range: 0x03 - 0xFF (-57dB to 6dB) */
    Adau1761_RegWrite(R29_PLAYBACK_HEADPHONE_LEFT_VOLUME_CONTROL, (uint8_t *)&vol, 1);
    Adau1761_RegWrite(R30_PLAYBACK_HEADPHONE_RIGHT_VOLUME_CONTROL, (uint8_t *)&vol, 1);
    printf("Set vol to 0x%x\r\n", vol);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_vol_test, i2s_vol, I2S Vol Test);

int cmd_i2s_rx_test(int argc, char **argv)
{
    i2s_device_t i2sDev = {
        .id = 0,
        .iis_mode = I2S_MODE_MASTER,
        .interface_mode = I2S_MODE_STD,
        .sampl_freq_hz = 16 * 1000,
        .channel_num = I2S_FS_CHANNELS_NUM_MONO,
        .frame_size = I2S_FRAME_LEN_16,
        .data_size = I2S_DATA_LEN_16,
        .fifo_threshold = 3,
    };
    dma_device_t i2s_rx_dma_dev = {
        .id = DMA0_ID,
        .ch = DMA0_CH0_INDEX,
        .transfer_mode = DMA_LLI_PINGPONG_MODE,
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_I2S_RX,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR4,
        .dst_burst_size = DMA_BURST_INCR4,
        .src_width = DMA_TRANSFER_WIDTH_16BIT,
        .dst_width = DMA_TRANSFER_WIDTH_16BIT,
        .lli_cfg = NULL,
        .callback = i2s_aud_cb_rx,
    };
    dma_device_t i2s_tx_dma_dev = {
        .id = DMA0_ID,
        .ch = DMA0_CH1_INDEX,
        .transfer_mode = DMA_LLI_PINGPONG_MODE,
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_I2S_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_size = DMA_BURST_INCR4,
        .dst_burst_size = DMA_BURST_INCR4,
        .src_width = DMA_TRANSFER_WIDTH_16BIT,
        .dst_width = DMA_TRANSFER_WIDTH_16BIT,
        .lli_cfg = NULL,
        .callback = i2s_aud_cb_tx,
    };

    Adau1761_Codec_Init();

    Adau1761_InSelect(MIC, 0xFF, 0xFF);

    arch_delay_ms(1000);
#ifdef SUPPORT_I2S_TEST
    hal_i2s_setConfig(&i2sDev);
    hal_i2s_init(0, COMMON_CAPS_DMA_TX | COMMON_CAPS_DMA_RX);

    dma_setConfig(&i2s_rx_dma_dev);
    dma_init(DMA0_ID, i2s_rx_dma_dev.ch);
    dma_ioctl(DMA0_ID, i2s_rx_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    hal_i2s_control(0, I2S_CTRL_ATTACH_RX_DMA, (void *)(uintptr_t)i2s_rx_dma_dev.ch);

    dma_setConfig(&i2s_tx_dma_dev);
    dma_init(DMA0_ID, i2s_tx_dma_dev.ch);
    dma_ioctl(DMA0_ID, i2s_tx_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    hal_i2s_control(0, I2S_CTRL_ATTACH_TX_DMA, (void *)(uintptr_t)i2s_tx_dma_dev.ch);

    memset(gAudReceived, 0, AUD_RECV_BUF_LEN);

    hal_i2s_read(0, gAudRXBuf, AUD_BUFFER_LEN);
#endif
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_rx_test, i2s_rx_test, I2S RX Test);

int cmd_i2s_tx_stop(int argc, char **argv)
{
//    bl_i2s_control(0, 0);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_tx_stop, i2s_tx_stop, I2S TX Stop);

int cmd_i2s_tx_start(int argc, char **argv)
{
//    bl_i2s_control(0, 1);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_tx_start, i2s_tx_start, I2S TX Start);

int cmd_i2s_rx_stop(int argc, char **argv)
{
//    bl_i2s_control(1, 0);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_rx_stop, i2s_rx_stop, I2S RX Stop);

int cmd_i2s_rx_start(int argc, char **argv)
{
    gAudRecvLen = 0;
    gAudRecvFull = 0;
//    bl_i2s_control(1, 1);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_rx_start, i2s_rx_start, I2S RX Start);

int cmd_i2s_rx_restart(int argc, char **argv)
{
    gAudRecvLen = 0;
    gAudRecvFull = 0;
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_rx_restart, i2s_rx_restart, I2S RX Restart);

int cmd_i2s_tx_src(int argc, char **argv)
{
//    bl_i2s_control(0, 0);
    gAudLimit = gAudRecvLen;
    vPortFree(gAudioSrc);
    gAudioSrc = pvPortMalloc(gAudLimit);
    memcpy(gAudioSrc, gAudReceived, gAudLimit);
//    bl_i2s_control(0, 1);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_tx_src, i2s_tx_src, I2S TX Source);

#endif

void i2s_demo_init(void)
{
    gAudTXBuf = malloc(AUD_BUFFER_LEN*2);
    gAudRXBuf = malloc(AUD_BUFFER_LEN*2);
    gAudReceived = malloc(AUD_RECV_BUF_LEN);
    i2c_init(AUD_I2C_ID);
}
