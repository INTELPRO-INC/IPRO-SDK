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

/**
 * NPU Hardware Abstraction Layer Implementation for IPRO7
 */

#include <stdbool.h>
#include "hal_npu.h"
#include "iproai_reg.h"
#include "drv_glb.h"
#include "glb_reg.h"
#include "compiler/common.h"

static bool g_bNPUStarted = false;

__attribute__((weak)) void CLKRST_NPU_SW_Reset(void)
{
}

__attribute__((weak)) void CLKRST_NPU_CLK_Sel(uint8_t clk)
{
    (void)clk;
}

__attribute__((weak)) void CLKRST_NPU_CLK_En(uint8_t en)
{
    (void)en;
}

/* Stub for imgBuffer - user should define in their application */
__attribute__((weak)) char *imgBuffer = NULL;

void NPU_Bus_Setting(void)
{
	uint32_t tmp;
	tmp = readl(IPROAI_BASE + IPROAI4_IPROAI_BUS_OFFSET);
	tmp = reg_set_bits(tmp, IPROAI4_REG_R_OSTD_LMT, 3);
	tmp = reg_set_bits(tmp, IPROAI4_REG_W_OSTD_LMT, 3);
	tmp = reg_set_bits(tmp, IPROAI4_REG_RCMD_LEN, 0x3f);
	tmp = reg_set_bits(tmp, IPROAI4_REG_WCMD_LEN, 0x3f);
	writel(tmp, IPROAI_BASE + IPROAI4_IPROAI_BUS_OFFSET);

}


void NPU_Clk_Div(int n)
{
	uint32_t tmp;
	tmp = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
	tmp = reg_set_bits(tmp, GLB_REG_IPROAI_CLK_SEL, 3);
	tmp = reg_set_bits(tmp, GLB_REG_IPROAI_CLK_DIV, n);
	writel(tmp, GLB_BASE + GLB_CLK_CFG5_OFFSET);

}

/**
 * Enable/Disable NPU Clock
 */
void hal_npu_clk_enable(uint8_t enable)
{
    if (enable) {
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
        CLKRST_NPU_CLK_En(1);
    } else {
        CLKRST_NPU_SW_Reset();
        CLKRST_NPU_CLK_En(0);
    }
}

/**
 * Acknowledge NPU Interrupt
 */
void hal_npu_ack_irq(void)
{
    uint32_t tmp;

    /* Read current interrupt config */
    tmp = readl(IPROAI_BASE + IPROAI4_IPROAI_INT_CFG_OFFSET);

    /* Set interrupt clear bit */
    tmp = reg_set_bit(tmp, IPROAI4_REG_INT_CLR);

    /* Write back */
    writel(tmp, IPROAI_BASE + IPROAI4_IPROAI_INT_CFG_OFFSET);
}

/**
 * Setup NPU Layer Configuration
 */
void hal_npu_layer_setup(uint8_t *inst_buf, uint8_t *weights_buf, uint8_t *bias_buf, uint8_t *mult_shift_buf)
{
    /* Configure NPU memory buffers */
    writel((uint32_t)inst_buf, IPROAI_BASE + IPROAI4_IPROAI_INST_ADDR_OFFSET);
    writel((uint32_t)weights_buf, IPROAI_BASE + IPROAI4_IPROAI_WEIGHT_ADDR_OFFSET);
    writel((uint32_t)bias_buf, IPROAI_BASE + IPROAI4_IPROAI_BIAS_ADDR_OFFSET);
    writel((uint32_t)mult_shift_buf, IPROAI_BASE + IPROAI4_IPROAI_TFPM_ADDR_OFFSET);
}

/**
 * Set NPU Input Buffer
 */
void hal_npu_set_input_buffer(uint8_t *input_buf, int patch_size)
{
    /* Configure input image buffer address */
    writel((uint32_t)input_buf, IPROAI_BASE + IPROAI4_APU_DM0_ADDR_OFFSET);

    /* Configure image memory segment size */
    writel(patch_size, IPROAI_BASE + IPROAI4_APU_DM1_ADDR_OFFSET);
}

/**
 * Set NPU Network Parameters
 */
void hal_npu_set_net_param(uint8_t unsign_input, uint8_t relu_n, uint8_t use_tflite, uint32_t mode)
{
    uint32_t tmp;

    /* Configure general settings */
    tmp = readl(IPROAI_BASE + IPROAI4_BALI_GENERAL_CFG_OFFSET);

    /* Set unsigned input flag */
    if (unsign_input) {
        tmp = reg_set_bit(tmp, IPROAI4_REG_IMGI_UNSGN);
    } else {
        tmp = reg_clr_bit(tmp, IPROAI4_REG_IMGI_UNSGN);
    }

    /* Set input mode */
    tmp = reg_set_bits(tmp, IPROAI4_REG_IMG_IN_MODE, mode);

    writel(tmp, IPROAI_BASE + IPROAI4_BALI_GENERAL_CFG_OFFSET);

    /* Configure TensorFlow Lite parameter loading */
    tmp = readl(IPROAI_BASE + IPROAI4_IPROAI_TF_CFG0_OFFSET);
    if (use_tflite) {
        tmp = reg_set_bit(tmp, IPROAI4_REG_TF_PARAM_FROM_MEM);
    } else {
        tmp = reg_clr_bit(tmp, IPROAI4_REG_TF_PARAM_FROM_MEM);
    }
    writel(tmp, IPROAI_BASE + IPROAI4_IPROAI_TF_CFG0_OFFSET);
}

/**
 * Reset NPU Unsigned Input Flag
 */
void hal_npu_reset_unsign(void)
{
    uint32_t tmp;

    tmp = readl(IPROAI_BASE + IPROAI4_BALI_GENERAL_CFG_OFFSET);
    tmp = reg_clr_bit(tmp, IPROAI4_REG_IMGI_UNSGN);
    writel(tmp, IPROAI_BASE + IPROAI4_BALI_GENERAL_CFG_OFFSET);
}

/**
 * Start NPU Execution
 */

void NPU_Resume(void)
{
    uint32_t tmp;
    tmp = readl(IPROAI_BASE + IPROAI4_IPROAI_INT_CFG_OFFSET);
    tmp = reg_set_bit(tmp, IPROAI4_REG_IPROAI_RESUME);
	writel(tmp, IPROAI_BASE + IPROAI4_IPROAI_INT_CFG_OFFSET);
}

void NPU_Start(void)
{
    uint32_t tmp;
    tmp = readl(IPROAI_BASE + IPROAI4_IPROAI_INT_CFG_OFFSET);
    tmp = reg_set_bit(tmp, IPROAI4_REG_IPROAI_START);
	writel(tmp, IPROAI_BASE + IPROAI4_IPROAI_INT_CFG_OFFSET);
}

void hal_npu_start(void)
{

    if (g_bNPUStarted){
		NPU_Resume();
    } else {
        NPU_Start();
        g_bNPUStarted = true;
    }
}


/**
 * Stop NPU Execution
 */
void hal_npu_stop(void)
{
    uint32_t tmp;

    tmp = readl(IPROAI_BASE + IPROAI4_IPROAI_INT_CFG_OFFSET);
    tmp = reg_set_bit(tmp, IPROAI4_REG_IPROAI_STOP);
    writel(tmp, IPROAI_BASE + IPROAI4_IPROAI_INT_CFG_OFFSET);

    g_bNPUStarted = false;
}

/**
 * Configure NPU Image External Address
 */
void NPU_Img_Ext_Addr_Cfg(uint32_t addr)
{
    /* Configure external image address (APU_DM2_ADDR) */
    writel(addr, IPROAI_BASE + IPROAI4_APU_DM2_ADDR_OFFSET);
}

/**
 * Get NPU Image External Address
 */
uint32_t NPU_Img_Ext_Addr_Get(void)
{
    /* Read external image address from APU_DM2_ADDR register */
    return readl(IPROAI_BASE + IPROAI4_APU_DM2_ADDR_OFFSET);
}

void NPU_SRAM_EN()
{
    uint32_t tmp;
	tmp = readl(GLB_BASE + GLB_SRAM_MISC_OFFSET);
	tmp = reg_set_bit(tmp, GLB_CR_IPROAI_EN);
    writel(tmp, GLB_BASE + GLB_SRAM_MISC_OFFSET);
}


/**
 * Configure NPU Bus Delays
 */
void NPU_Bus_Delay(uint8_t enable, uint8_t rcmd, uint8_t wcmd)
{
    uint32_t tmp;

    tmp = readl(IPROAI_BASE + IPROAI4_IPROAI_BUS_OFFSET);

    /* Configure read command length */
    tmp = reg_set_bits(tmp, IPROAI4_REG_RCMD_LEN, rcmd);

    /* Configure write command length */
    tmp = reg_set_bits(tmp, IPROAI4_REG_WCMD_LEN, wcmd);

    writel(tmp, IPROAI_BASE + IPROAI4_IPROAI_BUS_OFFSET);
}
