/**
 * @file hal_emac.c
 * @brief
 *
 * Copyright (c) 2021 IntelPro team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "drv_common.h"
#include "drv_emac.h"
#include "drv_glb.h"
#include "hal_emac.h"
#include "platform_common.h"
#include <generated/autoconf.h>
#if CTX_TYPE
#include <FreeRTOS.h>
#include <task.h>
#endif

#define EMAC_USE_INSIDE_CLOCK   (0)
#define TAG                     "EMAC_BD: "
#define EMAC_USED_ID            EMAC0_ID

static const uint32_t emacAddr[EMAC_ID_MAX] = { EMAC_BASE };

//#define EMAC_DO_FLUSH_DATA      1

#if CTX_TYPE
extern TaskHandle_t DequeueTaskHandle;
#endif
static EMAC_Handle_Type ethHandle;
EMAC_Handle_Type *thiz = NULL;

/**
 * @brief
 *
 */
static void emac_gpio_init(void)
{
#ifdef CONFIG_IPRO7
    uint8_t emacPins[] = {GLB_GPIO_PIN_3, GLB_GPIO_PIN_4, GLB_GPIO_PIN_5, GLB_GPIO_PIN_6, GLB_GPIO_PIN_7,
        GLB_GPIO_PIN_36, GLB_GPIO_PIN_37, GLB_GPIO_PIN_38, GLB_GPIO_PIN_39};
#else
    uint8_t emacPins[] = {GLB_GPIO_PIN_3, GLB_GPIO_PIN_4, GLB_GPIO_PIN_5, GLB_GPIO_PIN_6, GLB_GPIO_PIN_7};
#endif
    GLB_GPIO_Cfg_Type cfg;

    /* EMAC Pinmux */
    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.gpioFun = GPIO_FUN_ETHER_MAC;
    cfg.drive = 3;
    cfg.smtCtrl = 1;
    for(int i=0;i<sizeof(emacPins)/sizeof(emacPins[0]);i++){
        cfg.gpioPin=emacPins[i];
        GLB_GPIO_Init(&cfg);
    }

#if (EMAC_USE_INSIDE_CLOCK == 1)
    /* GPIO11 as clk_out (function 0) for PHY reference clock */
    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_DOWN;
    cfg.gpioFun = GPIO_FUN_CLK_OUT;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_11;
    GLB_GPIO_Init(&cfg);

    /* Set cam_ref_clk source to CLKPLL_POSTDIV_OUTCLK (sel=3), div=0, enable */
    GLB_Set_CAM_CLK(1, GLB_CAM_CLK_CLKPLL_POSTDIV_OUTCLK, 0);

    /* Enable chip_clk_out_en bit 3 for clk_out_3 */
    uint32_t tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);
    tmpVal |= (1 << 13);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG4_OFFSET);
#endif

    cfg.gpioMode=GPIO_MODE_OUTPUT;
    cfg.pullType=GPIO_PULL_UP;
    cfg.drive=1;
    cfg.smtCtrl=1;
    cfg.gpioPin=40;
    cfg.gpioFun=GPIO_FUN_GPIO;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(40, 0);
    arch_delay_ms(10);
    GLB_GPIO_Write(40, 1);
    arch_delay_ms(150);
}

/**
 * @brief
 *
 * @param bdt
 * @return int
 */
static uint32_t emac_bd_get_cur_active(EMAC_BD_TYPE_e bdt)
{
    uint32_t bd = 0;

    EMAC_GetBD(EMAC_USED_ID, &bd);

    if (bdt == EMAC_BD_TYPE_TX) {
        bd &= EMAC_TXBDPTR_MSK;
        bd >>= EMAC_TXBDPTR_POS;
        if (bd == 0)
            bd = thiz->txBuffLimit;
        else
            bd--;
    }

    if (bdt == EMAC_BD_TYPE_RX) {
        bd &= EMAC_RXBDPTR_MSK;
        bd >>= EMAC_RXBDPTR_POS;
        if (bd == thiz->txBuffLimit+1)
            bd = thiz->rxBuffLimit;
        else
            bd--;

    }

    return bd;
}

/**
 * @brief
 *
 * @param index
 * @return int
 */
static int emac_bd_rx_enqueue(uint32_t index) __attribute__((unused));
static int emac_bd_rx_enqueue(uint32_t index)
{
    int err = SUCCESS;

    thiz->rxIndexEMAC = index;

    return err;
}

/**
 * @brief
 *
 * @param index
 * @return int
 */
static void emac_bd_rx_on_err(uint32_t index)
{
#if CTX_TYPE
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
        xTaskNotifyFromISR(DequeueTaskHandle, 0x02, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#else
    MSG("emac_bd_rx_on_err, bd[%d] %08x, %08x\r\n", index, (unsigned int)thiz->bd[index].C_S_L, (unsigned int)readl(EMAC_BASE + EMAC_INT_SOURCE_OFFSET));
    for (int i = 6; i < 12; i++)
        MSG("BD[%d] 0x%lx, 0x%lx\r\n", i, readl(0x20083400 + i*8), readl(0x20083404 + i*8));

    /* handle error */
    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_OR)) {
        MSG("EMAC RX OR Error at %s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_RE)) {
        MSG("MAC RX RE Error at %s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_DN)) {
        MSG("MAC RX DN Error at %s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_TL)) {
        MSG("MAC RX TL Error at %s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_CRC)) {
        MSG("MAC RX CRC Error at %s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_LC)) {
        MSG("MAC RX LC Error at %s:%d\r\n", __func__, __LINE__);
    }

    thiz->bd[index].C_S_L &= ~0xffff;
    /* RX BD is ready for RX */
    thiz->bd[index].C_S_L |= EMAC_BD_FIELD_MSK(RX_E);
#endif
}

/**
 * @brief this func will be called in ISR
 *
 * @param index
 * @return int
 */
static int emac_bd_tx_dequeue(uint32_t index) __attribute__((unused));
static int emac_bd_tx_dequeue(uint32_t index)
{
    int err = SUCCESS;
    EMAC_BD_Desc_Type *DMADesc;

    thiz->txIndexEMAC = index;
    DMADesc = &thiz->bd[thiz->txIndexEMAC];
    /* release this tx BD to SW (HW will do this) */
    DMADesc->C_S_L &= EMAC_BD_FIELD_UMSK(TX_RD);

    return err;
}

/**
 * @brief
 *
 * @param index
 * @return int
 */
static void emac_bd_tx_on_err(uint32_t index)
{
    /* handle error */
    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_UR)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_RTRY)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_RL)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_LC)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_DF)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_CS)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    thiz->bd[index].C_S_L &= ~(0x1ff);
}

/**
 * @brief
 *
 */
__WEAK void emac_tx_done_callback_app(void)
{
}

/**
 * @brief
 *
 */
static void emac_tx_done_callback(void)
{
#if CTX_TYPE
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(DequeueTaskHandle, 0x01, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#else
    uint32_t index = 0;
    index = emac_bd_get_cur_active(EMAC_BD_TYPE_TX);

    emac_bd_tx_dequeue(index);

    emac_tx_done_callback_app();
#endif
}

/**
 * @brief
 *
 */
__WEAK void emac_tx_error_callback_app(void)
{
    puts("Tx error\r\n");
}

/**
 * @brief
 *
 */
static void emac_tx_error_callback(void)
{
    uint32_t index = 0;

    index = emac_bd_get_cur_active(EMAC_BD_TYPE_TX);
    emac_bd_tx_on_err(index);
    emac_tx_error_callback_app();
}

/**
 * @brief
 *
 */
__WEAK void emac_rx_done_callback_app(void)
{
}

/**
 * @brief
 *
 */
static void emac_rx_done_callback(void)
{
#if CTX_TYPE
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(DequeueTaskHandle, 0x02, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#else
    uint32_t index = 0;

    index = emac_bd_get_cur_active(EMAC_BD_TYPE_RX);

    emac_bd_rx_enqueue(index);

    emac_rx_done_callback_app();
#endif
}

/**
 * @brief
 *
 */
__WEAK void emac_rx_error_callback_app(void)
{
    puts("Rx error\r\n");
}

/**
 * @brief
 *
 */
static void emac_rx_error_callback(void)
{
    uint32_t index;

    index = emac_bd_get_cur_active(EMAC_BD_TYPE_RX);

    emac_bd_rx_on_err(index);

    emac_rx_error_callback_app();
}

/**
 * @brief
 *
 */
__WEAK void emac_rx_busy_callback_app(void)
{
}

/**
 * @brief
 *
 */
static void emac_rx_busy_callback(void)
{
    //MSG("EMAC Rx busy at %s:%d\r\n", __func__, __LINE__);
#if CTX_TYPE
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(DequeueTaskHandle, 0x02, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#else
    emac_rx_busy_callback_app();
#endif
}

void emac_irq_process(void)
{
    EMAC_ID_Type emacId = EMAC_USED_ID;
    uint32_t tmpVal;
    uint32_t EMACx = emacAddr[emacId];
    
    tmpVal = readl(EMACx + EMAC_INT_MASK_OFFSET);
//    printf("emac irq status = 0x%lx\r\n", readl(EMACx + EMAC_INT_SOURCE_OFFSET));
    if (SET == EMAC_GetIntStatus(emacId,EMAC_INT_TX_DONE) && !reg_is_bit_set(tmpVal, EMAC_TXB_M)) {
        EMAC_ClrIntStatus(emacId,EMAC_INT_TX_DONE);
        EMAC_IntMask(emacId, EMAC_INT_TX_DONE, MASK);
        emac_tx_done_callback();
    }

    if (SET == EMAC_GetIntStatus(emacId,EMAC_INT_TX_ERROR) && !reg_is_bit_set(tmpVal, EMAC_TXE_M)) {
        EMAC_ClrIntStatus(emacId,EMAC_INT_TX_ERROR);
        emac_tx_error_callback();
    }

    if (SET == EMAC_GetIntStatus(emacId,EMAC_INT_RX_DONE) && !reg_is_bit_set(tmpVal, EMAC_RXB_M)) {
        EMAC_ClrIntStatus(emacId,EMAC_INT_RX_DONE);
        EMAC_IntMask(emacId, EMAC_INT_RX_DONE, MASK);
        emac_rx_done_callback();
    }

    if (SET == EMAC_GetIntStatus(emacId,EMAC_INT_RX_ERROR) && !reg_is_bit_set(tmpVal, EMAC_RXE_M)) {
        EMAC_ClrIntStatus(emacId,EMAC_INT_RX_ERROR);
        emac_rx_error_callback();
    }

    if (SET == EMAC_GetIntStatus(emacId,EMAC_INT_RX_BUSY) && !reg_is_bit_set(tmpVal, EMAC_BUSY_M)) {
        EMAC_ClrIntStatus(emacId,EMAC_INT_RX_BUSY);
        emac_rx_busy_callback();
    }

    if (SET == EMAC_GetIntStatus(emacId,EMAC_INT_TX_CTRL) && !reg_is_bit_set(tmpVal, EMAC_TXC_M)) {
        EMAC_ClrIntStatus(emacId,EMAC_INT_TX_CTRL);
    }

    if (SET == EMAC_GetIntStatus(emacId,EMAC_INT_RX_CTRL) && !reg_is_bit_set(tmpVal, EMAC_RXC_M)) {
        EMAC_ClrIntStatus(emacId,EMAC_INT_RX_CTRL);
    }
}

void EMAC_Interrupt_Init(void)
{
    MSG("EMAC_Interrupt_Init.\r\n");
    Interrupt_Handler_Register(EMAC_IRQn, emac_irq_process);
    CPU_Interrupt_Enable(EMAC_IRQn);

    EMAC_ClrIntStatus(EMAC_USED_ID,EMAC_INT_ALL);
    EMAC_IntMask(EMAC_USED_ID,EMAC_INT_ALL, UNMASK);
    MSG("emac_intmask.\r\n");
}
/**
 * @brief
 *
 * @param emac_cfg
 * @return int
 */
int emac_init(emac_device_t *emac_cfg)
{
    EMAC_CFG_Type emacCfg = {
        .recvSmallFrame = ENABLE,           /*!< Receive small frmae or not */
        .recvHugeFrame = DISABLE,           /*!< Receive huge frmae(>64K bytes) or not */
        .padEnable = ENABLE,                /*!< Enable padding for frame which is less than MINFL or not */
        .crcEnable = ENABLE,                /*!< Enable hardware CRC or not */
        .noPreamble = DISABLE,              /*!< Enable preamble or not */
        .recvBroadCast = ENABLE,            /*!< Receive broadcast frame or not */
        .interFrameGapCheck = ENABLE,       /*!< Check inter frame gap or not */
        .miiNoPreamble = DISABLE,            /*!< Enable MII interface preamble or not */
        .miiClkDiv = 49,                    /*!< MII interface clock divider from bus clock */
        .maxTxRetry = 16,                   /*!< Maximum tx retry count */
#ifdef CONFIG_IPRO7
        .speed_mode = SPEED_SEL_MODE,
#endif
        .interFrameGapValue = 24,           /*!< Inter frame gap vaule in clock cycles(default 24)*/
        .minFrameLen = 64,                  /*!< Minimum frame length */
        .maxFrameLen = ETH_MAX_PACKET_SIZE, /*!< Maximum frame length */
        .collisionValid = 16,               /*!< Collision valid value */
        .macAddr[0] = 0x18,                 /*!< MAC Address */
        .macAddr[1] = 0xB0,
        .macAddr[2] = 0x09,
        .macAddr[3] = 0x00,
        .macAddr[4] = 0x12,
        .macAddr[5] = 0x34,
    };
    int err = SUCCESS;

    /* init emac giio */
    emac_gpio_init();

    memcpy(emacCfg.macAddr, emac_cfg->mac_addr, 6);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_EMAC);
#if EMAC_USE_INSIDE_CLOCK
    /* 50MHZ clock is from uhs pll */
    GLB_Set_ETH_REF_O_CLK_Sel(GLB_ETH_REF_CLK_OUT_INSIDE_50M);
#else
    GLB_Set_ETH_REF_O_CLK_Sel(GLB_ETH_REF_CLK_OUT_OUTSIDE_50M);
#endif
    //GLB_Invert_ETH_RX_CLK(1);
    //GLB_Invert_ETH_REF_O_CLK(0);
    EMAC_Init(EMAC_USED_ID,&emacCfg);

    EMAC_Interrupt_Init();
    return err;
}

/**
 * @brief
 *
 * @param eth_tx_buff
 * @param tx_buf_count
 * @param eth_rx_buff
 * @param rx_buf_count
 * @return int
 */
int emac_bd_init(uint8_t *eth_tx_buff, uint8_t tx_buf_count, uint8_t *eth_rx_buff, uint8_t rx_buf_count)
{
    int err = SUCCESS;
    thiz = &ethHandle;

    /* init the BDs in emac with buffer address */
    err = EMAC_DMADescListInit(EMAC_USED_ID, thiz, (uint8_t *)eth_tx_buff, tx_buf_count,
                               (uint8_t *)eth_rx_buff, rx_buf_count);
    return err;
}

/**
 * @brief
 *
 * @param none
 * @return int
 */
int emac_bd_fragment_support(void){
    return 1;
}

/**
 * @brief
 *
 * @param flags
 * @param len
 * @param data_in
 * @return int
 */
//uint32_t tmpbuf[400];
int emac_bd_tx_enqueue(uint32_t flags, uint32_t len, const uint8_t *data_in)
{
    int err = SUCCESS;
    EMAC_BD_Desc_Type *DMADesc;
    uint32_t tx_flags=EMAC_TX_COMMON_FLAGS;
    DMADesc = &thiz->bd[thiz->txIndexCPU];

    if (flags&EMAC_FRAGMENT_PACKET) {
        /* Fragment packet, clear EOF */
        tx_flags &= EMAC_BD_FIELD_UMSK(TX_EOF);
    }

    if (DMADesc->C_S_L & EMAC_BD_FIELD_MSK(TX_RD)) {
        /* no free BD, lost sync with DMA TX? */
        err = NORESC;
        //MSG(TAG"%s:%d\n", __func__, __LINE__);
    } else {
        __DSB();
        // MSG("tx q flags:%d,len:%d,data:0x%x\r\n", flags, len, data_in);
        if (flags&EMAC_NOCOPY_PACKET) {
            DMADesc->Buffer = (uint32_t)data_in;
        }else{
            arch_memcpy_fast((void *)DMADesc->Buffer, data_in, len);
        }

        /* following two lines is for cache test since tmpbuf is in cache range */
        //ARCH_MemCpy_Fast(tmpbuf, data_in, len);        
        //DMADesc->Buffer = (uint32_t)tmpbuf;
#ifdef EMAC_DO_FLUSH_DATA
        if(L1C_Is_DCache_Range((uintptr_t)DMADesc->Buffer)){
            L1C_DCache_Clean_By_Addr((uintptr_t)DMADesc->Buffer,len);
        }
#endif
        DMADesc->C_S_L = tx_flags | (len << BD_TX_LEN_POS);

        /* move to next TX BD */
        if ((++thiz->txIndexCPU) > thiz->txBuffLimit) {
            /* the last BD */
            DMADesc->C_S_L |= EMAC_BD_FIELD_MSK(TX_WR);
            /* wrap back */
            thiz->txIndexCPU = 0;
        }
    }

    return err;
}

/**
 * @brief
 *
 * @param flags
 * @param len
 * @param data_out
 * @return int
 */
int emac_bd_rx_dequeue(uint32_t flags, uint32_t *len, uint8_t *data_out)
{
    int err = SUCCESS;
    EMAC_BD_Desc_Type *DMADesc;

    DMADesc = &thiz->bd[thiz->rxIndexCPU];

    if (DMADesc->C_S_L & EMAC_BD_FIELD_MSK(RX_E)) {
        /* current RX BD is empty */
        err = NORESC;
        *len = 0;
    } else {
        *len = (thiz->bd[thiz->rxIndexCPU].C_S_L & EMAC_BD_FIELD_MSK(RX_LEN)) >> BD_RX_LEN_POS;
#ifdef EMAC_DO_FLUSH_DATA
        if(L1C_Is_DCache_Range((uintptr_t)DMADesc->Buffer)){
            L1C_DCache_Invalid_By_Addr((uintptr_t)DMADesc->Buffer, *len);
        }
#endif
        if (data_out) {
            arch_memcpy_fast(data_out, (const void *)(uintptr_t)DMADesc->Buffer, *len);
        }

        /* RX BD can be used for another receive */
        DMADesc->C_S_L |= EMAC_BD_FIELD_MSK(RX_E);

        /* move to next RX BD */
        if ((++thiz->rxIndexCPU) > thiz->rxBuffLimit) {
            /* the last BD */
            DMADesc->C_S_L |= EMAC_BD_FIELD_MSK(RX_WR);
            /* wrap back */
            thiz->rxIndexCPU = thiz->txBuffLimit + 1;
        }
    }

    return err;
}

/**
 * @brief
 *
 * @param phyAddress
 * @return int
 */
int emac_phy_set_address(uint16_t phyAddress)
{
    EMAC_Phy_SetAddress(EMAC_USED_ID,phyAddress);

    return 0;
}

/**
 * @brief
 *
 * @param fullDuplex
 * @return int
 */
int emac_phy_config_full_duplex(uint8_t fullDuplex)
{
    EMAC_Phy_Set_Full_Duplex(EMAC_USED_ID,fullDuplex);

    return 0;
}

/**
 * @brief
 *
 * @param phyReg
 * @param regValue
 * @return int
 */
int emac_phy_reg_read(uint16_t phyReg, uint16_t *regValue)
{
    if (EMAC_Phy_Read(EMAC_USED_ID,phyReg, regValue) != SUCCESS) {
        return -1;
    }

    return 0;
}

/**
 * @brief
 *
 * @param phyReg
 * @param regValue
 * @return int
 */
int emac_phy_reg_write(uint16_t phyReg, uint16_t regValue)
{
    if (EMAC_Phy_Write(EMAC_USED_ID,phyReg, regValue) != SUCCESS) {
        return -1;
    }

    return 0;
}

int emac_stop(void)
{
    return EMAC_Disable(EMAC_USED_ID);
}

int emac_start(void)
{
    EMAC_Enable(EMAC_USED_ID);
    return 0;
}

int emac_start_tx(void)
{
    return EMAC_Enable_TX(EMAC_USED_ID);
}

int emac_stop_tx(void)
{
    return EMAC_Disable_TX(EMAC_USED_ID);
}

int emac_start_rx(void)
{
    return EMAC_Enable_RX(EMAC_USED_ID);
}

int emac_stop_rx(void)
{
    return EMAC_Disable_RX(EMAC_USED_ID);
}

int emac_intmask(EMAC_INT_Type intType, int intMask)
{
    return EMAC_IntMask(EMAC_USED_ID,intType, intMask);
}

int emac_get_fram_len(uint16_t *max, uint16_t *min)
{
  return EMAC_GetFramLen(EMAC_USED_ID, max, min);
}

