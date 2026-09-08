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

#include "hal_cam.h"
#include "drv_dvp2axi.h"
#include "drv_glb.h"
#include "drv_mm_misc.h"
#include "drv_clock.h"

#define DVP_AS_THRESHOLD_MARGIN        (10)

#define CHANNEL_NUM       (1)

enum
{
    CAM_UPDATE_FLAG_FRC,
    CAM_UPDATE_FLAG_MAX,
};

// Keep this size to be reference
static cam_hal_frame_rate_t frc_tmp[CHANNEL_NUM] = {0};

static uint8_t update_flag[CAM_UPDATE_FLAG_MAX][CHANNEL_NUM] = {0};

static cam_hal_out_format_e out_format_tmp;

static uint8_t pp_num_tmp[CHANNEL_NUM] = {0};
static uint8_t pp_idx[CHANNEL_NUM] = {0};
static uint32_t out_addr_tmp[CHANNEL_NUM][CAM_MAX_BUFFER_NUM] = {0};

static void (*g_camCallback)(cam_out_frame_info_t info);

static cam_hal_src_e cam_src_tmp;

// ============================================================================
// private function declare
// ============================================================================
static DVP2AXI_PORT_E get_dvp_port_by_output(uint8_t output);
static void update_next_buffer(uint8_t output);
static void update_fps_cfg(void);
static int check_fps_cfg(uint8_t src_fps, uint8_t dst_fps);
static int check_out_crop_cfg(cam_hal_cfg_t *cam_hal_cfg);

static void cam_isr(uint8_t output);
static void cam_channel_ISR(void);
static uint32_t get_cam_clk(void);
static uint32_t cam_cal_dvp_as_threshold(uint32_t width, uint32_t cam_clk, uint32_t sns_pclk);
static uint32_t cam_cal_total_pix(uint32_t total_width, uint32_t cam_clk, uint32_t sns_pclk);
static void cam_spi2dvp_cfg(cam_hal_cfg_t *cam_hal_cfg);
static void cam_dvp_as_cfg(cam_hal_cfg_t *cam_hal_cfg);
static void cam_restore_parameter(cam_hal_cfg_t *cam_hal_cfg);
static void cam_dvp2axi_set_callback(void);
static void cam_dvp2axi_clr_callback(void);
static void cam_set_channel_frame_rate(uint8_t src_fps, uint8_t dst_fps, uint32_t *bitwise_vld_frm, uint8_t *period_cnt);
static void cam_dvp2axi_cfg(cam_hal_cfg_t *cam_hal_cfg);

// ============================================================================
// private function
// ============================================================================
static DVP2AXI_PORT_E get_dvp_port_by_output(uint8_t output)
{
    return DVP2AXI_PORT_A;
}

static void update_next_buffer(uint8_t output)
{
    uint32_t frameAddr, byteCnt = 0;
    DVP2AXI_PORT_E port = get_dvp_port_by_output(output);

    if (pp_num_tmp[output] > 1) // do ping-pong if num > 1
    {
        pp_idx[output] = (pp_idx[output] + 1) % pp_num_tmp[output];
        DVP2AXI_SetAddress(port, out_addr_tmp[output][pp_idx[output]]);
        if (out_format_tmp == CAM_HAL_OUT_FMT_YUV422SP)
        {
            DVP2AXI_GetFrameBuffer(port, &frameAddr, &byteCnt);
            DVP2AXI_SetAddress(DVP2AXI_PORT_B, out_addr_tmp[output][pp_idx[output]] + byteCnt);
        }
    }
}

static void update_fps_cfg(void)
{
    uint8_t i;
    uint8_t period_cnt = 0;
    uint32_t vld_frm = 0;

    for (i = 0; i < CHANNEL_NUM; i++)
    {
        if (update_flag[CAM_UPDATE_FLAG_FRC][i])
        {
            cam_set_channel_frame_rate(frc_tmp[i].src_fps, frc_tmp[i].dst_fps, &vld_frm, &period_cnt);
            DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_A, vld_frm, (period_cnt - 1));
            if (out_format_tmp == CAM_HAL_OUT_FMT_YUV422SP)
                DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_B, vld_frm, (period_cnt - 1));
            update_flag[CAM_UPDATE_FLAG_FRC][i] = 0;
        }
    }
}

static void cam_isr(uint8_t output)
{
    cam_out_frame_info_t frame_info;
    uint32_t frameAddr, byteCnt = 0;
    uint32_t frameAddr1, byteCnt1 = 0;

    DVP2AXI_PORT_E port = get_dvp_port_by_output(output);

    DVP2AXI_GetFrameBuffer(port, &frameAddr, &byteCnt);
    if (out_format_tmp == CAM_HAL_OUT_FMT_YUV422SP)
        DVP2AXI_GetFrameBuffer(DVP2AXI_PORT_B, &frameAddr1, &byteCnt1);

    frame_info.out_addr = frameAddr;
    frame_info.out_size = byteCnt + byteCnt1;

    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_NORMAL)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_NORMAL);
        frame_info.event = CAM_HAL_INT_NORMAL;
        update_next_buffer(output);
        if (g_camCallback)
            g_camCallback(frame_info);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_FIFO_OVERWRITE)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_FIFO_OVERWRITE);
        // reset dvp2axi
        DVP2AXI_Disable(port);
        DVP2AXI_Enable(port);
        frame_info.event = CAM_HAL_INT_FIFO_OVERWRITE;
        if (g_camCallback)
            g_camCallback(frame_info);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_HSYNC_CNT_ERROR)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_HSYNC_CNT_ERROR);
        frame_info.event = CAM_HAL_INT_HSYNC_CNT_ERROR;
        if (g_camCallback)
            g_camCallback(frame_info);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_VSYNC_CNT_ERROR)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_VSYNC_CNT_ERROR);
        frame_info.event = CAM_HAL_INT_VSYNC_CNT_ERROR;
        if (g_camCallback)
            g_camCallback(frame_info);
    }
}

static void cam_channel_ISR(void)
{
    cam_isr(0); // for output channel 0 (callback from dvp2axi_a)
}

static uint32_t get_cam_clk(void)
{
#if defined(CONFIG_FPGA)
    // FPGA is 50Mhz
    return 50000000;
#else
    // return 96000000;
#ifdef CONFIG_IPRO7
    return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK);
#endif
#ifdef CONFIG_IPRO6
    return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_BCLK);
#endif
#endif
}

static uint32_t cam_cal_dvp_as_threshold(uint32_t width, uint32_t cam_clk, uint32_t sns_pclk)
{
    /* FIFO_thresh = width x (cam_clk - pclk>>1) / cam_clk + margin, yuyv 2T for 1 pixel */
    return (uint64_t)width * (cam_clk - (sns_pclk>>1)) / cam_clk + DVP_AS_THRESHOLD_MARGIN;
}

static uint32_t cam_cal_total_pix(uint32_t total_width, uint32_t cam_clk, uint32_t sns_pclk)
{
    /* total pix = sensor total pix x (cam_clk /(pclk>>1)) + half sensor total pix, yuyv 2T for 1 pixel */
    return (uint64_t)total_width * cam_clk / (sns_pclk>>1) + (total_width >> 1);
}

static void cam_spi2dvp_cfg(cam_hal_cfg_t *cam_hal_cfg)
{
    MM_MISC_SPI2DVP_Cfg cfg;

    // below setting by spi sensor
    cfg.dataOrder   = cam_hal_cfg->src_cfg.spi2dvp_input.order;
    cfg.dataFormat  = cam_hal_cfg->src_cfg.spi2dvp_input.format;
    cfg.spiMode     = cam_hal_cfg->src_cfg.spi2dvp_input.mode;
    cfg.dataEndian  = cam_hal_cfg->src_cfg.spi2dvp_input.endian;

#ifdef CONFIG_IPRO7
    MM_MISC_Set_DVP_AS_SRAM_Mode(MM_MISC_SHARED_SRAM_MODE_SPI2DVP);
    MM_MISC_Set_SPI2DVP_Select(MM_MISC_SPI2DVP_SEL_SPI);
#endif
#ifdef CONFIG_IPRO6
    GLB_Set_SPI2DVP_State(ENABLE);
#endif

    MM_MISC_SPI2DVP_Init(cfg);
    MM_MISC_SPI2DVP_INT_Enable();
}

static void cam_dvp_as_cfg(cam_hal_cfg_t *cam_hal_cfg)
{
    MM_MISC_DVP_AS_TG_Cfg dvp_as_tg = {0};
    MM_MISC_DVP_AS_Cfg dvp_as = {0};
    MM_MISC_S2P_Cfg_Type dvp_s2p = {0};
    uint32_t cam_clk = 0;

    cam_clk = get_cam_clk();
#ifdef CONFIG_IPRO7
    dvp_as.hSyncLevel = MM_MISC_ACTIVE_HIGH;
    dvp_as.vSyncLevel = MM_MISC_ACTIVE_HIGH;
#endif
    dvp_as.fifoThreshold = cam_cal_dvp_as_threshold(cam_hal_cfg->in_size.width, cam_clk, cam_hal_cfg->src_cfg.dvp_input.pclk);

    dvp_as_tg.totalHPixels = cam_cal_total_pix(cam_hal_cfg->src_cfg.dvp_input.totalHPixels, cam_clk, cam_hal_cfg->src_cfg.dvp_input.pclk);
    dvp_as_tg.actStartPixel = 0;
    dvp_as_tg.actPixelWidth = cam_hal_cfg->in_size.width;
    dvp_as_tg.actStartLine = 0;
    dvp_as_tg.actLineHeight = cam_hal_cfg->in_size.height;

    dvp_s2p.hSyncLevel = cam_hal_cfg->src_cfg.dvp_input.hsync;
    dvp_s2p.vSyncLevel = cam_hal_cfg->src_cfg.dvp_input.vsync;
    dvp_s2p.dataOrder = cam_hal_cfg->src_cfg.dvp_input.order;

#ifdef CONFIG_IPRO7
    MM_MISC_Set_DVP_AS_SRAM_Mode(MM_MISC_SHARED_SRAM_MODE_DVP_AS);
#endif
#ifdef CONFIG_IPRO6
    GLB_Set_SPI2DVP_State(DISABLE);
#endif
    MM_MISC_S2P_Init(&dvp_s2p);
    MM_MISC_Set_S2P_State(ENABLE);
    MM_MISC_Set_DVP_AS_TG_Cfg(&dvp_as_tg);
    MM_MISC_Set_DVP_AS_TG_State(ENABLE);
    MM_MISC_DVP_AS_Set_Cfg(&dvp_as);
}

static void cam_restore_parameter(cam_hal_cfg_t *cam_hal_cfg)
{
    uint8_t i;

    // restore isp source
    cam_src_tmp = cam_hal_cfg->cam_src;

    // restore Callback function
    g_camCallback = cam_hal_cfg->out_callback;

    // restore out port 0 format
    out_format_tmp = cam_hal_cfg->output_cfg.format;

    // check buffer pingpong number
    if (0 == cam_hal_cfg->output_cfg.pp_num) cam_hal_cfg->output_cfg.pp_num = 1; // force to set 1 when 0
    if (CAM_MAX_BUFFER_NUM < cam_hal_cfg->output_cfg.pp_num) cam_hal_cfg->output_cfg.pp_num = CAM_MAX_BUFFER_NUM; // force to set max buffer when overflow

    // restore out port pp number and address
    pp_num_tmp[0] = (cam_hal_cfg->output_cfg.ring_line)?1:cam_hal_cfg->output_cfg.pp_num;

    // reset pp_idx
    memset((void*)&pp_idx[0], 0, sizeof(pp_idx));

    for (i = 0; i < CAM_MAX_BUFFER_NUM; i++)
    {
        out_addr_tmp[0][i] = cam_hal_cfg->output_cfg.addr[i];
    }

    // reset update flag
    memset((void*)&update_flag[0][0], 0, sizeof(update_flag));
}

static void cam_dvp2axi_set_callback(void)
{
#ifdef CONFIG_IPRO7
    Interrupt_Handler_Register(D2XA_IRQn, cam_channel_ISR);
    CPU_Interrupt_Enable(D2XA_IRQn);
#endif
#ifdef CONFIG_IPRO6
    Interrupt_Handler_Register(DVP2BUS_INT0_IRQn, cam_channel_ISR);
    CPU_Interrupt_Enable(DVP2BUS_INT0_IRQn);
#endif
}

static void cam_dvp2axi_clr_callback(void)
{
#ifdef CONFIG_IPRO7
    CPU_Interrupt_Disable(D2XA_IRQn);
    Interrupt_Handler_Register(D2XA_IRQn, NULL);
#endif
#ifdef CONFIG_IPRO6
    CPU_Interrupt_Disable(DVP2BUS_INT0_IRQn);
    Interrupt_Handler_Register(DVP2BUS_INT0_IRQn, NULL);
#endif
}

static int check_fps_cfg(uint8_t src_fps, uint8_t dst_fps)
{
    if ((src_fps < 1) || (dst_fps > src_fps))
        return 1; // invalid fps configuration
    else
        return 0;
}

static void cam_set_channel_frame_rate(uint8_t src_fps, uint8_t dst_fps, uint32_t *bitwise_vld_frm, uint8_t *period_cnt)
{
    uint32_t bitwise_valid_frm = 0;
    int frm_cnt = 0;
    int frm_period_cnt = 0;

    // output drop
    if (0 == dst_fps)
    {
        *bitwise_vld_frm = 0;
        *period_cnt = 1;
        return;
    }

    // do frame rate
    while (1) {
        bitwise_valid_frm <<= 1;
        frm_cnt += dst_fps;
        frm_period_cnt++;
        if (frm_cnt >= src_fps) {
            bitwise_valid_frm |= 1;
            frm_cnt -= src_fps;
            if (frm_cnt == 0) {
                break;
            }
        }
    }
    *bitwise_vld_frm = bitwise_valid_frm;
    *period_cnt = frm_period_cnt;
}

static int check_out_crop_cfg(cam_hal_cfg_t *cam_hal_cfg)
{
    int ret = 0;
    cam_hal_crop_t *out_crop;
    uint16_t width, height;

    out_crop = &cam_hal_cfg->output_cfg.out_crop;
    width = cam_hal_cfg->in_size.width;
    height = cam_hal_cfg->in_size.height;

    // check crop config valid or not
    if (out_crop->start_x & 0x1)
    {
        printf("out crop start_x should be even\r\n");
        ret = 1;
    }
    if (out_crop->width & 0x1)
    {
        printf("out crop width should be even\r\n");
        ret = 1;
    }

    if (out_crop->start_x > out_crop->width)
    {
        printf("out crop error start_x\r\n");
        ret = 1;
    }
    if (out_crop->start_y > out_crop->height)
    {
        printf("out crop error start_y\r\n");
        ret = 1;
    }
    if ((out_crop->start_x + out_crop->width) > width)
    {
        printf("out crop error start_x + width\r\n");
        ret = 1;
    }
    if ((out_crop->start_y + out_crop->height) > height)
    {
        printf("out crop error start_y + height\r\n");
        ret = 1;
    }

    return ret;
}

static void cam_dvp2axi_cfg(cam_hal_cfg_t *cam_hal_cfg)
{
    DVP2AXI_INIT_CFG_S dvp2axi_cfg = {0};
    uint16_t width, height;
    uint32_t vld_frm = 0;
    uint8_t period_cnt = 0;
    uint16_t h_start, h_end, v_start, v_end; // for dvp2axi output crop

    // common config
    dvp2axi_cfg.framePol = DVP2AXI_DATA_ACTIVE_HIGH;
    dvp2axi_cfg.linePol = DVP2AXI_DATA_ACTIVE_HIGH;
    dvp2axi_cfg.swIntCnt = 1;
    dvp2axi_cfg.sensorMode = DVP2AXI_DATA_VALID_V_AND_H;

    if (check_fps_cfg(cam_hal_cfg->output_cfg.fr_ctrl.src_fps, cam_hal_cfg->output_cfg.fr_ctrl.dst_fps))
    {
        // invalid setting, always output
        vld_frm = 1;
        period_cnt = 1;
    }
    else
        cam_set_channel_frame_rate(cam_hal_cfg->output_cfg.fr_ctrl.src_fps, cam_hal_cfg->output_cfg.fr_ctrl.dst_fps, &vld_frm, &period_cnt);

    if (check_out_crop_cfg(cam_hal_cfg)) // invalid cfg, no crop, dvp2axi out size refer scaler out size
    {
        width = cam_hal_cfg->in_size.width;
        height = cam_hal_cfg->in_size.height;
        h_start = 0;
        h_end   = 0x3FFF;
        v_start = 0;
        v_end   = 0x3FFF;
    }
    else // valid cfg, dvp2axi out size refer out crop size
    {
        width = cam_hal_cfg->output_cfg.out_crop.width;
        height = cam_hal_cfg->output_cfg.out_crop.height;
        h_start = cam_hal_cfg->output_cfg.out_crop.start_x;
        h_end   = cam_hal_cfg->output_cfg.out_crop.start_x + cam_hal_cfg->output_cfg.out_crop.width;
        v_start = cam_hal_cfg->output_cfg.out_crop.start_y;
        v_end   = cam_hal_cfg->output_cfg.out_crop.start_y + cam_hal_cfg->output_cfg.out_crop.height;
    }

#ifdef CONFIG_IPRO7
    if (CAM_HAL_SRC_SPI2DVP == cam_hal_cfg->cam_src)
    {
        MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_DVP2AXI_SRC_SPI2DVP);
        MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_B, MM_MISC_DVP2AXI_SRC_SPI2DVP);
    }
    else
    {
        MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_DVP2AXI_SRC_DVP_AS);
        MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_B, MM_MISC_DVP2AXI_SRC_DVP_AS);
    }
#endif

    // yuyv format and use dvp2axi_A only
    if ((CAM_HAL_OUT_FMT_YUYV == cam_hal_cfg->output_cfg.format) || (CAM_HAL_OUT_FMT_Y_ONLY == cam_hal_cfg->output_cfg.format))
    {
        if (CAM_HAL_OUT_FMT_YUYV == cam_hal_cfg->output_cfg.format)
        {
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_16_BIT;
        }
        else // Y only
        {
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
        }

        dvp2axi_cfg.subSampleEn = 0;
        dvp2axi_cfg.wrapEn = (0 == cam_hal_cfg->output_cfg.ring_line)?0:1;
        dvp2axi_cfg.wrapLine = cam_hal_cfg->output_cfg.ring_line;
        dvp2axi_cfg.byte_sel = DVP2AXI_SEL_LOW_BYTE; // YUYV don't care, Y only select low byte
        dvp2axi_cfg.memStart = cam_hal_cfg->output_cfg.addr[0];
        dvp2axi_cfg.imgWidth = width;
        dvp2axi_cfg.imgHeight = height;

        DVP2AXI_Init(DVP2AXI_PORT_A, &dvp2axi_cfg);
        DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_A, cam_hal_cfg->in_size.width, cam_hal_cfg->in_size.height);

        DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_A, vld_frm, (period_cnt - 1));

        DVP2AXI_IntMask(DVP2AXI_PORT_A, DVP2AXI_INT_ALL, UNMASK);

        DVP2AXI_Crop(DVP2AXI_PORT_A, h_start, h_end, v_start, v_end);

        DVP2AXI_Enable(DVP2AXI_PORT_A);
    }
    else // 422SP format and use dvp2axi_A for Y and dvp2axi_B for UV
    {
        // Config dvp2axi a
        dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
        dvp2axi_cfg.subSampleEn = 0;
        dvp2axi_cfg.wrapEn = (0 == cam_hal_cfg->output_cfg.ring_line)?0:1;
        dvp2axi_cfg.wrapLine = cam_hal_cfg->output_cfg.ring_line;
        dvp2axi_cfg.byte_sel = DVP2AXI_SEL_LOW_BYTE;
        dvp2axi_cfg.memStart = cam_hal_cfg->output_cfg.addr[0];
        dvp2axi_cfg.imgWidth = width;
        dvp2axi_cfg.imgHeight = height;

        DVP2AXI_Init(DVP2AXI_PORT_A, &dvp2axi_cfg);
        DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_A, cam_hal_cfg->in_size.width, cam_hal_cfg->in_size.height);

        DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_A, vld_frm, (period_cnt - 1));

        DVP2AXI_IntMask(DVP2AXI_PORT_A, DVP2AXI_INT_ALL, UNMASK);

        DVP2AXI_Crop(DVP2AXI_PORT_A, h_start, h_end, v_start, v_end);

        // Config dvp2axi b
        dvp2axi_cfg.byte_sel = DVP2AXI_SEL_HIGH_BYTE;

        if (0 == dvp2axi_cfg.wrapEn)
            dvp2axi_cfg.memStart = cam_hal_cfg->output_cfg.addr[0] + DVP2AXI_GetLineOffset(width) * height;
        else
            dvp2axi_cfg.memStart = cam_hal_cfg->output_cfg.addr[0] + DVP2AXI_GetLineOffset(width) * cam_hal_cfg->output_cfg.ring_line;

        DVP2AXI_Init(DVP2AXI_PORT_B, &dvp2axi_cfg);
        DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_B, cam_hal_cfg->in_size.width, cam_hal_cfg->in_size.height);

        DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_B, vld_frm, (period_cnt - 1));

        DVP2AXI_IntMask(DVP2AXI_PORT_B, DVP2AXI_INT_ALL, UNMASK);

        DVP2AXI_Crop(DVP2AXI_PORT_B, h_start, h_end, v_start, v_end);

        DVP2AXI_Enable(DVP2AXI_PORT_A);
        DVP2AXI_Enable(DVP2AXI_PORT_B);
    }
}

// ============================================================================
// public function
// ============================================================================
int cam_init(cam_hal_cfg_t *cam_hal_cfg)
{
    // restore some setting
    cam_restore_parameter(cam_hal_cfg);

    // set isp input path
    if (CAM_HAL_SRC_DVP_AS == cam_hal_cfg->cam_src)
        cam_dvp_as_cfg(cam_hal_cfg);
    else //(CAM_HAL_SRC_SPI2DVP == cam_hal_cfg->cam_src)
        cam_spi2dvp_cfg(cam_hal_cfg);

    // set dvp2axi A & B
    cam_dvp2axi_cfg(cam_hal_cfg);
    cam_dvp2axi_set_callback();

    return 0;
}

int cam_start(void)
{
    if (CAM_HAL_SRC_SPI2DVP == cam_src_tmp)
        MM_MISC_Set_SPI2DVP_State(1);
    else
        MM_MISC_Set_DVP_AS_State(ENABLE);

    return 0;
}

int cam_deinit(void)
{
    // clear callback function
    cam_dvp2axi_clr_callback();

    return 0;
}

int cam_fps_cfg(cam_hal_frame_rate_t frc)
{
    int ret = 0;

    ret = check_fps_cfg(frc.src_fps, frc.dst_fps);

    if (0 == ret)
    {
        frc_tmp[0].src_fps = frc.src_fps;
        frc_tmp[0].dst_fps = frc.dst_fps;
        update_flag[CAM_UPDATE_FLAG_FRC][0] = 1;
    }

    // direct set this value
    update_fps_cfg();
    return ret;
}
