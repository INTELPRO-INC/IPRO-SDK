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


#if defined(IPRO6LE)
#include "drv_sdu.h"
#include "drv_gpio.h"
#include "drv_glb.h"
#include "drv_glb_gpio.h"
#else
#include <bl616_sdu.h>
#include <bl616_gpio.h>
#include <bl616_glb_gpio.h>
#endif

#include "sdio_port.h"
#include "trcver_sdio.h"

#include <sdiowifi_platform_adapt.h>

#if defined(IPRO6LE)
/* Allocate RX ring in ram_wifi via .wifibss.sdiowifi section (caught by
 * `*(.wifibss.*)` in linker). Direct .wifi_ram name loses to linker
 * orphan-placement which puts it in ram_tcm, breaking SDIO_MAWOC alias
 * (0x22fdxxxx is past ram_nocache 64KB). Aligned 32 for DMA + alias. */
/* TX aggregation: each download port buffer holds up to ~2 packed frames in
 * one CMD53 write, so the IPRO6LE ring uses SDIO_DNLD_AGG_BUF_SIZE (NOT the 2048
 * SDIO_RX_BUF_SIZE). The pointer stride must match the storage stride.
 * Placed in ram_nocache (.noncacheable -> 0x22FC0000, 64KB free) instead of
 * ram_wifi: ram_wifi is full (.wifibss alone ~157KB/160KB). ram_nocache is
 * DMA-reachable and uncached, so the SDU writes are coherent without the
 * SDIO_MAWOC cached->uncached alias (MAWOC is identity on a 0x22xxxxxx addr). */
static uint32_t __attribute__((aligned(32), section(".noncacheable")))
    sdio_readbuf_storage[SDIO_RX_MAX_PORT_NUM][SDIO_DNLD_AGG_BUF_SIZE / 4];
uint32_t (*sdio_readbuf)[SDIO_DNLD_AGG_BUF_SIZE / 4] = sdio_readbuf_storage;
#elif defined(CFG_IPRO_WIFI)
uint32_t (*sdio_readbuf)[SDIO_RX_BUF_SIZE / 4] = (void *)SDIO_RX_BUF_START;
#else
struct {
    uint32_t buf[SDIO_RX_MAX_PORT_NUM][SDIO_RX_BUF_SIZE / 4];
} __attribute__((aligned(32))) sdio_readbuf_wrapper;
uint32_t (*sdio_readbuf)[SDIO_RX_BUF_SIZE / 4] = (void *)&sdio_readbuf_wrapper;
#endif

void sdio_glb_reset(void)
{
#ifdef CFG_IPRO_WIFI
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDU);
#else
    GLB_AHB_Slave1_Reset(IPRO_AHB_SLAVE1_SDU);
#endif
}

void sdio_gpio_init(void)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 1;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;

    for (int i = 10; i < 16; i++) {
        gpio_cfg.gpioPin = i;
        gpio_cfg.gpioFun = GPIO_FUN_SDIO;
        GLB_GPIO_Init(&gpio_cfg);
    }
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU);
}

#define WL_REGS8(x)         (*(volatile unsigned char *)(x))
uint8_t sdu_read_s_reg(uint32_t offset)
{
    return WL_REGS8(BL_FUNC_SCRATCH_BASE + offset);
}

void sdu_write_s_reg(uint32_t offset, uint8_t value)
{
    WL_REGS8(BL_FUNC_SCRATCH_BASE + offset) = value;
}

uint32_t sdio_get_rxbuf(uint8_t port_num)
{
    return (uint32_t)SDIO_MAWOC(&sdio_readbuf[port_num]);
}

void sdio_attach_rxbuf(uint8_t fn_num, uint8_t port_num)
{
    SdioFuncReg[fn_num]->WrIdx = port_num;
    SdioFuncReg[fn_num]->SqWriteBase = sdio_get_rxbuf(port_num);
    SdioFuncReg[fn_num]->WrBitMap = (1 << port_num);
}

void sdio_host_check_init(void)
{
    HR_LOGI("sdio_host_check_init\r\n");

    /* clear power value */
    WL_REGS8(BL_FUNC_SCRATCH_BASE)=0;    //clear power value;

    /* reset rd and wr bitmap */
    SdioFuncReg[FUNC_WIFI]->RdBitMap = 0x0;
    SdioFuncReg[FUNC_WIFI]->WrBitMap = 0x0;//reset rd and wr bitmap

    /* toggle SDIO_CCR_CIC_DnLdOvr on WL_SDIO_CCR_CARD_INT_CAUSE */
    SdioFuncReg[FUNC_WIFI]->CardIntStatus = SDIO_CCR_CIC_DnLdOvr;//?
    SdioFuncReg[FUNC_WIFI]->CardIntStatus = 0;///clear int

    SdioFuncReg[FUNC_WIFI]->Config2 |= CONFIG2_MSK ;
    SdioFuncReg[FUNC_WIFI]->Config |= 0x00000010;

    /* unmask the interrupts */
    //SdioFuncReg[FUNC_WIFI]->CardIntMask = SDIO_CCR_CIM_MASK;
    SdioFuncReg[FUNC_WIFI]->CardIntMask = 0x0067;
   /* select interrupt reset mode */
    SdioFuncReg[FUNC_WIFI]->CardIntMode = 0;
}

void gen_card2host_int(void)
{
    //sdio_GEN_CARD2HOST_INT(0,(SDIO_CCR_CS_ReadCISRdy | SDIO_CCR_CS_DnLdRdy  | SDIO_CCR_CS_IORdy));
    //GLB_GPIO_Write(GLB_GPIO_PIN_3, 1);
    sdio_GEN_CARD2HOST_INT(0, SDIO_CCR_CS_UpLdRdy);
    //sdio_GEN_CARD2HOST_INT(1, SDIO_CCR_CS_UpLdRdy);
    //GLB_GPIO_Write(GLB_GPIO_PIN_3, 0);


    return;
}

