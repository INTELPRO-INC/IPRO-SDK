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



#include "drv_romapi_patch.h"
#include "drv_romdriver.h"
#include "softcrc.h"

/* Anchor: force this translation unit into the link. The drv_romapi.c ROM
   wrappers are weak; a static-archive member that only supplies weak defs is
   not auto-extracted, so the strong overrides below would never enter the link.
   Referencing this symbol via -u__drv_romapi_patch_anchor pulls the whole TU,
   after which each strong override wins over the corresponding weak ROM wrapper. */
int __attribute__((used)) __drv_romapi_patch_anchor = 1;

/* Keep every other peripheral on the ROM implementation, but make the I2S
 * result deterministic and consistent with the SDK-driver path by deriving
 * I2S_REF_CLK from the 960 MHz Wi-Fi PLL VCO, its post-divider and the GLB I2S
 * reference divider.  CCI validation with the real I2S_TX DMA request showed
 * that postdiv=20, refdiv=0 and a divisor of 34 produce 44.1 kHz stereo PCM
 * pacing. */
uint32_t Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_Type type)
{
    uint32_t postdiv;
    uint32_t refdiv;

    if (type != DRV_PERIPHERAL_CLOCK_I2S) {
        return RomDriver_Clock_Peripheral_Clock_Get(type);
    }

    postdiv = reg_get_bits(readl(CCI_BASE + CCI_WIFI_PLL_CFG1_OFFSET),
                           CCI_WIFIPLL_POSTDIV);
    if (postdiv == 0u) {
        return 0u;
    }

    refdiv = reg_get_bits(readl(GLB_BASE + GLB_I2S_CFG0_OFFSET),
                          GLB_REG_I2S_REF_CLK_DIV);
    return (960u * 1000u * 1000u) / postdiv / (refdiv + 1u);
}

#ifndef BOOTROM
#define EF_CTRL_LOAD_BEFORE_READ_R0 EF_Ctrl_Load_Efuse_R0()
#else
#define EF_CTRL_LOAD_BEFORE_READ_R0
#endif

/* WiFi PLL Config*/
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_32M_38P4M_40M = {
    .clkpllRefdivRatio = 2,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 0,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 0,        /*!< pll_icp_1u */
    .clkpllIcp5u = 2,        /*!< pll_icp_5u */
    .clkpllRz = 3,           /*!< pll_rz */
    .clkpllCz = 1,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 1,      /*!< pll_r4_short */
    .clkpllC4En = 0,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 1,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 1,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_24M = {
    .clkpllRefdivRatio = 1,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 0,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 0,        /*!< pll_icp_1u */
    .clkpllIcp5u = 2,        /*!< pll_icp_5u */
    .clkpllRz = 3,           /*!< pll_rz */
    .clkpllCz = 1,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 1,      /*!< pll_r4_short */
    .clkpllC4En = 0,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 1,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 1,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_26M = {
    .clkpllRefdivRatio = 1,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 1,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 1,        /*!< pll_icp_1u */
    .clkpllIcp5u = 0,        /*!< pll_icp_5u */
    .clkpllRz = 5,           /*!< pll_rz */
    .clkpllCz = 2,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 0,      /*!< pll_r4_short */
    .clkpllC4En = 1,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 0,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 0,    /*!< pll_sdm_bypass */
};

const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION wifiPllCfg_960M[GLB_XTAL_MAX] = {
    { NULL, 0x0, 0 },                                 /*!< XTAL is None */
    { &wifiPllBasicCfg_24M, 0x1400000, 0 },           /*!< XTAL is 24M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1E00000, 0 }, /*!< XTAL is 32M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1900000, 0 }, /*!< XTAL is 38.4M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1800000, 0 }, /*!< XTAL is 40M */
    { &wifiPllBasicCfg_26M, 0x1276276, 0 },           /*!< XTAL is 26M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1E00000, 0 }, /*!< XTAL is RC32M */
};

const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION wifiPllCfg_960M_Fast[1] = {
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1800000, 0 }, /*!< XTAL is 40M */
};

const GLB_SLAVE_GRP_0_TBL_Type ATTR_CLOCK_CONST_SECTION glb_slave_grp_0_table[GLB_SLAVE_GRP_0_MAX] = {
    { GLB_ADC_CFG0_OFFSET, GLB_GPADC_32M_DIV_EN_POS, GLB_GPADC_32M_CLK_SEL_POS, GLB_GPADC_32M_CLK_DIV_POS, GLB_GPADC_32M_DIV_EN_LEN, GLB_GPADC_32M_CLK_SEL_LEN, GLB_GPADC_32M_CLK_DIV_LEN },
    { GLB_I2C_CFG0_OFFSET, GLB_I2C_CLK_EN_POS, GLB_I2C_CLK_SEL_POS, GLB_I2C_CLK_DIV_POS, GLB_I2C_CLK_EN_LEN, GLB_I2C_CLK_SEL_LEN, GLB_I2C_CLK_DIV_LEN },
    { GLB_SPI_CFG0_OFFSET, GLB_SPI_CLK_EN_POS, GLB_SPI_CLK_SEL_POS, GLB_SPI_CLK_DIV_POS, GLB_SPI_CLK_EN_LEN, GLB_SPI_CLK_SEL_LEN, GLB_SPI_CLK_DIV_LEN },
    { GLB_DBI_CFG0_OFFSET, GLB_DBI_CLK_EN_POS, GLB_DBI_CLK_SEL_POS, GLB_DBI_CLK_DIV_POS, GLB_DBI_CLK_EN_LEN, GLB_DBI_CLK_SEL_LEN, GLB_DBI_CLK_DIV_LEN },
    { GLB_AUDIO_CFG0_OFFSET, GLB_REG_AUDIO_AUTO_DIV_EN_POS, 0, 0, GLB_REG_AUDIO_AUTO_DIV_EN_LEN, 0, 0 },
    { GLB_AUDIO_CFG0_OFFSET, GLB_REG_AUDIO_ADC_CLK_EN_POS, 0, GLB_REG_AUDIO_ADC_CLK_DIV_POS, GLB_REG_AUDIO_ADC_CLK_EN_LEN, 0, GLB_REG_AUDIO_ADC_CLK_DIV_LEN },
    { GLB_AUDIO_CFG1_OFFSET, GLB_REG_AUDIO_SOLO_CLK_EN_POS, 0, GLB_REG_AUDIO_SOLO_CLK_DIV_POS, GLB_REG_AUDIO_SOLO_CLK_EN_LEN, 0, GLB_REG_AUDIO_SOLO_CLK_DIV_LEN },
    { GLB_CAM_CFG0_OFFSET, GLB_REG_CAM_REF_CLK_EN_POS, GLB_REG_CAM_REF_CLK_SRC_SEL_POS, GLB_REG_CAM_REF_CLK_DIV_POS, GLB_REG_CAM_REF_CLK_EN_LEN, GLB_REG_CAM_REF_CLK_SRC_SEL_LEN, GLB_REG_CAM_REF_CLK_DIV_LEN },
    { GLB_PSRAM_CFG0_OFFSET, GLB_REG_PSRAMB_CLK_EN_POS, GLB_REG_PSRAMB_CLK_SEL_POS, GLB_REG_PSRAMB_CLK_DIV_POS, GLB_REG_PSRAMB_CLK_EN_LEN, GLB_REG_PSRAMB_CLK_SEL_LEN, GLB_REG_PSRAMB_CLK_DIV_LEN },
};

static const uint32_t uartAddr[UART_ID_MAX] = { UART0_BASE, UART1_BASE };
static intCallback_Type *uartIntCbfArra[UART_ID_MAX][UART_INT_ALL] = {
    { NULL }
};

typedef struct
{
    uint32_t jedecID;
    char *name;
    const SPI_Flash_Cfg_Type *cfg;
} Flash_Info_t;

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_GD_LQ64E = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xa0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3d,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 5000,
    .timeE32k = 4000,
    .timeE64k = 4000,
    .timePagePgm = 5,
    .timeCe = 65 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_GD_Q64E = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_16JV = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01, /*Q08BV,Q16DV: 0x02.Q32FW,Q32FV: 0x01 */
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xa0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3d,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_64JV = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 0,
    .cReadMode = 0xFF,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 400,
    .timeE32k = 1600,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_256FV = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xa0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = 0x24,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 400,
    .timeE32k = 1600,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Mxic_25U256 = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc2,

    .deBurstWrapCmd = 0xC0,
    .deBurstWrapCmdDmyClk = 0x00,
    .deBurstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .deBurstWrapData = 0x10,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0,
    .qeBit = 0x06,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x15,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xA5,

    .burstWrapCmd = 0xC0,
    .burstWrapCmdDmyClk = 0x00,
    .burstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .burstWrapData = 0x02,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x38,
    .qppAddrMode = SF_CTRL_ADDR_4_LINES,

    .ioMode = (SF_CTRL_QIO_MODE | 0x20),
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 400,
    .timeE32k = 1000,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Puya_Q32H = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0x85,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0x01,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,
    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 8,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION Flash_Info_t flashInfos[] = {
    {
        .jedecID = 0x1740ef,
        //.name="WB_64JV_64_33",
        .cfg = &flashCfg_Winb_64JV,
    },
    {
        .jedecID = 0x1940ef,
        //.name="WB_256FV_256_33",
        .cfg = &flashCfg_Winb_256FV,
    },
    {
        .jedecID = 0x1760ef,
        //.name="WB_64JW_64_18",
        .cfg = &flashCfg_Winb_64JV,
    },
    {
        .jedecID = 0x1570ef,
        //.name="WB_16JV_16_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x1740c8,
        //.name="GD_64E_64_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x1760c8,
        //.name="GD_LQ64E_64_18",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x1860c8,
        //.name="GD_LQ128E_128_18",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x1865C8,
        //.name="GD_WQ128E_128_1833",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x15345e,
        //.name="ZB_WQ16A_16_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x15405e,
        //.name="ZB_16B_16_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x16405e,
        //.name="ZB_32B_32_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x17405e,
        //.name="ZB_VQ64_64_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x18405e,
        //.name="ZB_VQ128_128_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x15605e,
        //.name="ZB_VQ16_16_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x3925c2,
        //.name="MX_U25643G_256_18",
        .cfg = &flashCfg_Mxic_25U256,
    },
    {
        .jedecID = 0x144020,
        //.name="XM_QH80_08_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x154020,
        //.name="XM_QH16_16_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x164020,
        //.name="XM_QH32_32_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x174020,
        //.name="XM_QH64_64_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x184020,
        //.name="XM_QH128C_128_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x165020,
        //.name="XM_LU32_32_18",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x174220,
        //.name="XM_QW64_64_1833",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x17400b,
        //.name="XT_F64B_64_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x176085,
        //.name="Puya_Q64H_64_33",
        .cfg = &flashCfg_Puya_Q32H,
    },
    {
        .jedecID = 0x186085,
        //.name="Puya_Q128H_128_33",
        .cfg = &flashCfg_Puya_Q32H,
    },
    {
        .jedecID = 0x1540a1,
        //.name="FM25Q_16A_16_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x1640a1,
        //.name="FM25Q_32_32_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x1740a1,
        //.name="FM25Q_64_64_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x1840a1,
        //.name="FM25Q_128_128_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x1828a1,
        //.name="FM25W_128_128_1833",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x1560c4,
        //.name="gt25q16_16_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x1660c4,
        //.name="gt25q32_32_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x1760c4,
        //.name="gt25q64_64_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x142085,
        //.name="py25q80hb_80_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x152085,
        //.name="py25q16hb_16_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x162085,
        //.name="py25q32hb_32_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x172085,
        //.name="py25q64ha_64_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x182085,
        //.name="py25q128ha_128_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x192085,
        //.name="py25q256hb_256_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x166125,
        //.name="sk25e032_32_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
};

//UART
#if (defined BOOTROM) || (!defined IPRO_USE_HAL_DRIVER)
static void UART_IntHandler(uint8_t uartId)
{
    uint32_t tmpVal = 0;
    uint32_t maskVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_INT_STS_OFFSET);
    maskVal = readl(UARTx + UART_INT_MASK_OFFSET);

    /* Length of uart tx data transfer arrived interrupt */
    if (reg_is_bit_set(tmpVal, UART_UTX_END_INT) && !reg_is_bit_set(maskVal, UART_CR_UTX_END_MASK)) {
        writel(1 << UART_CR_UTX_END_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_TX_END] != NULL) {
            uartIntCbfArra[uartId][UART_INT_TX_END]();
        }
    }

    /* Length of uart rx data transfer arrived interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_END_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_END_MASK)) {
        writel(1 << UART_CR_URX_END_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_RX_END] != NULL) {
            uartIntCbfArra[uartId][UART_INT_RX_END]();
        }
    }

    /* Tx fifo ready interrupt,auto-cleared when data is pushed */
    if (reg_is_bit_set(tmpVal, UART_UTX_FRDY_INT) && !reg_is_bit_set(maskVal, UART_CR_UTX_FRDY_MASK)) {
        if (uartIntCbfArra[uartId][UART_INT_TX_FIFO_REQ] != NULL) {
            uartIntCbfArra[uartId][UART_INT_TX_FIFO_REQ]();
        }
    }

    /* Rx fifo ready interrupt,auto-cleared when data is popped */
    if (reg_is_bit_set(tmpVal, UART_URX_FRDY_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_FRDY_MASK)) {
        if (uartIntCbfArra[uartId][UART_INT_RX_FIFO_REQ] != NULL) {
            uartIntCbfArra[uartId][UART_INT_RX_FIFO_REQ]();
        }
    }

    /* Rx time-out interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_RTO_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_RTO_MASK)) {
        writel(1 << UART_CR_URX_RTO_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_RTO] != NULL) {
            uartIntCbfArra[uartId][UART_INT_RTO]();
        }
    }

    /* Rx parity check error interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_PCE_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_PCE_MASK)) {
        writel(1 << UART_CR_URX_PCE_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_PCE] != NULL) {
            uartIntCbfArra[uartId][UART_INT_PCE]();
        }
    }

    /* Tx fifo overflow/underflow error interrupt */
    if (reg_is_bit_set(tmpVal, UART_UTX_FER_INT) && !reg_is_bit_set(maskVal, UART_CR_UTX_FER_MASK)) {
        if (uartIntCbfArra[uartId][UART_INT_TX_FER] != NULL) {
            uartIntCbfArra[uartId][UART_INT_TX_FER]();
        }
    }

    /* Rx fifo overflow/underflow error interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_FER_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_FER_MASK)) {
        if (uartIntCbfArra[uartId][UART_INT_RX_FER] != NULL) {
            uartIntCbfArra[uartId][UART_INT_RX_FER]();
        }
    }

    /* Rx lin mode sync field error interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_LSE_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_LSE_MASK)) {
        writel(1 << UART_CR_URX_LSE_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_LSE] != NULL) {
            uartIntCbfArra[uartId][UART_INT_LSE]();
        }
    }

    /* Rx byte count reached interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_BCR_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_BCR_MASK)) {
        writel(1 << UART_CR_URX_BCR_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_BCR] != NULL) {
            uartIntCbfArra[uartId][UART_INT_BCR]();
        }
    }

    /* Rx auto baud rate detection finish interrupt using start bit */
    if (reg_is_bit_set(tmpVal, UART_URX_ADS_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_ADS_MASK)) {
        writel(1 << UART_CR_URX_ADS_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_STARTBIT] != NULL) {
            uartIntCbfArra[uartId][UART_INT_STARTBIT]();
        }
    }

    /* Rx auto baud rate detection finish interrupt using codeword 0x55 */
    if (reg_is_bit_set(tmpVal, UART_URX_AD5_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_AD5_MASK)) {
        writel(1 << UART_CR_URX_AD5_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_0X55] != NULL) {
            uartIntCbfArra[uartId][UART_INT_0X55]();
        }
    }
}

void UART1_IRQHandler(void)
{
    UART_IntHandler(UART1_ID);
}
#endif

int UART_Int_Callback_Install(uint8_t uartId, uint8_t intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    uartIntCbfArra[uartId][intType] = cbFun;

    return SUCCESS;
}

int UART_SetBaudRate(uint8_t uartId, uint32_t baudRate)
{
    uint32_t uartClk = 0;
    uint32_t fraction = 0;
    uint32_t baudRateDivisor = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Get uart clk */
    if (uartId == UART0_ID || uartId == UART1_ID) {
        uartClk = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0);
    } else {
        return ERROR;
    }

    /* Cal the baud rate divisor */
    fraction = uartClk * 10 / baudRate % 10;
    baudRateDivisor = uartClk / baudRate;

    if (fraction >= 5) {
        ++baudRateDivisor;
    }

    /* Set the baud rate register value */
    writel(((baudRateDivisor - 1) << 0x10) | ((baudRateDivisor - 1) & 0xFFFF), UARTx + UART_BIT_PRD_OFFSET);

    return SUCCESS;
}
//CLOCK
//EFUSE
//PDS

//FLASH

int ATTR_TCM_SECTION SFlash_Blk32_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum)
{
    uint32_t cnt = 0;
    uint8_t is32BitsAddr = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    int stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uintptr_t)&flashCmd) % 4 == 0) {
        ARCH_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        ARCH_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    is32BitsAddr = (flashCfg->ioMode & 0x20);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    if (is32BitsAddr > 0) {
        flashCmd.addrSize++;
        flashCmd.cmdBuf[0] = (flashCfg->blk32EraseCmd << 24) | ((IPRO_SPIFLASH_BLK32K_SIZE * blkNum) >> 8);
        flashCmd.cmdBuf[1] = ((IPRO_SPIFLASH_BLK32K_SIZE * blkNum) << 24);
    } else {
        flashCmd.cmdBuf[0] = (flashCfg->blk32EraseCmd << 24) | (IPRO_SPIFLASH_BLK32K_SIZE * blkNum);
    }

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(500);
        cnt++;

        if (cnt > flashCfg->timeE32k * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr)
{
    uint32_t len = 0;
    uint32_t eraseLen = 0;
    int ret = SUCCESS;

    if (startaddr > endaddr) {
        return ERROR;
    }

    while (startaddr <= endaddr) {
        len = endaddr - startaddr + 1;

        if (flashCfg->blk64EraseCmd != IPRO_SPIFLASH_CMD_INVALID &&
            (startaddr & (IPRO_SPIFLASH_BLK64K_SIZE - 1)) == 0 &&
            len > (IPRO_SPIFLASH_BLK64K_SIZE - flashCfg->sectorSize * 1024)) {
            /* 64K margin address,and length > 64K-sector size, erase one first */
            ret = SFlash_Blk64_Erase(flashCfg, startaddr / IPRO_SPIFLASH_BLK64K_SIZE);
            eraseLen = IPRO_SPIFLASH_BLK64K_SIZE;
        } else if (flashCfg->blk32EraseCmd != IPRO_SPIFLASH_CMD_INVALID &&
                   (startaddr & (IPRO_SPIFLASH_BLK32K_SIZE - 1)) == 0 &&
                   len > (IPRO_SPIFLASH_BLK32K_SIZE - flashCfg->sectorSize * 1024)) {
            /* 32K margin address,and length > 32K-sector size, erase one first */
            ret = SFlash_Blk32_Erase(flashCfg, startaddr / IPRO_SPIFLASH_BLK32K_SIZE);
            eraseLen = IPRO_SPIFLASH_BLK32K_SIZE;
        } else {
            /* Sector erase */
            startaddr = ((startaddr) & (~(flashCfg->sectorSize * 1024 - 1)));
            ret = SFlash_Sector_Erase(flashCfg, startaddr / flashCfg->sectorSize / 1024);
            eraseLen = flashCfg->sectorSize * 1024;
        }

        startaddr += eraseLen;

        if (ret != SUCCESS) {
            return ERROR;
        }
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *flashCfg)
{
    uint32_t ret = 0;
    uint32_t qeValue = 0;
    uint32_t regValue = 0;
    uint32_t readValue = 0;
    uint8_t readRegValue0 = 0;
    uint8_t readRegValue1 = 0;

    if((flashCfg->ioMode&0xf)==SF_CTRL_QO_MODE || (flashCfg->ioMode&0xf)==SF_CTRL_QIO_MODE){
        qeValue = 1;
    }

    SFlash_Read_Reg(flashCfg, 0, (uint8_t *)&readRegValue0, 1);
    SFlash_Read_Reg(flashCfg, 1, (uint8_t *)&readRegValue1, 1);
    readValue = (readRegValue0|(readRegValue1<<8));
    if ((readValue & (~((1<<(flashCfg->qeIndex*8+flashCfg->qeBit)) |
                        (1<<(flashCfg->busyIndex*8+flashCfg->busyBit)) |
                        (1<<(flashCfg->wrEnableIndex*8+flashCfg->wrEnableBit))))) == 0){
        return SUCCESS;
    }

    ret = SFlash_Write_Enable(flashCfg);
    if (SUCCESS != ret) {
        return ERROR;
    }
    if (flashCfg->qeWriteRegLen == 2) {
        regValue = (qeValue<<(flashCfg->qeIndex*8+flashCfg->qeBit));
        SFlash_Write_Reg(flashCfg, 0, (uint8_t *)&regValue, 2);
    } else {
        if (flashCfg->qeIndex == 0) {
            regValue = (qeValue<<flashCfg->qeBit);
        } else {
            regValue = 0;
        }
        SFlash_Write_Reg(flashCfg, 0, (uint8_t *)&regValue, 1);
        ret = SFlash_Write_Enable(flashCfg);
        if (SUCCESS != ret) {
            return ERROR;
        }
        if (flashCfg->qeIndex == 1) {
            regValue = (qeValue<<flashCfg->qeBit);
        } else {
            regValue = 0;
        }
        SFlash_Write_Reg(flashCfg, 1, (uint8_t *)&regValue, 1);
    }
    return SUCCESS;
}

int ATTR_TCM_SECTION SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t i;
    uint8_t buf[sizeof(SPI_Flash_Cfg_Type) + 8];
    uint32_t crc, *pCrc;
    uint32_t xipOffset;
    char flashCfgMagic[] = "FCFG";

    if (flashID == 0) {
        xipOffset = SF_Ctrl_Get_Flash_Image_Offset(0, SF_CTRL_FLASH_BANK0);
        SF_Ctrl_Set_Flash_Image_Offset(0, 0, SF_CTRL_FLASH_BANK0);
        XIP_SFlash_Read_Via_Cache_Need_Lock(8 + IPRO6_FLASH_XIP_BASE, buf, sizeof(SPI_Flash_Cfg_Type) + 8);
        SF_Ctrl_Set_Flash_Image_Offset(xipOffset, 0, SF_CTRL_FLASH_BANK0);

        if (ARCH_MemCmp(buf, flashCfgMagic, 4) == 0) {
            crc = IPRO_Soft_CRC32((uint8_t *)buf + 4, sizeof(SPI_Flash_Cfg_Type));
            pCrc = (uint32_t *)(buf + 4 + sizeof(SPI_Flash_Cfg_Type));

            if (*pCrc == crc) {
                ARCH_MemCpy_Fast(pFlashCfg, (uint8_t *)buf + 4, sizeof(SPI_Flash_Cfg_Type));
                return SUCCESS;
            }
        }
    } else {
        if (RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock(flashID, pFlashCfg) == SUCCESS) {
            return SUCCESS;
        }
        for (i = 0; i < sizeof(flashInfos) / sizeof(flashInfos[0]); i++) {
            if (flashInfos[i].jedecID == flashID) {
                ARCH_MemCpy_Fast(pFlashCfg, flashInfos[i].cfg, sizeof(SPI_Flash_Cfg_Type));
                return SUCCESS;
            }
        }
    }

    return ERROR;
}

uint32_t ATTR_TCM_SECTION SF_Cfg_Flash_Identify_Ext(uint8_t callFromFlash, uint8_t flashPinCfg,
                                                    uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank)
{
    uint32_t jdecId = 0;
    uint32_t i = 0;
    uint32_t ret = 0;

    ret = SF_Cfg_Flash_Identify(callFromFlash, flashPinCfg, restoreDefault, pFlashCfg, group, bank);
    if (callFromFlash) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, pFlashCfg->ioMode & 0xf, 1, 0, 32, bank);
    }
    if ((ret & IPRO_FLASH_ID_VALID_FLAG) != 0) {
        return ret;
    }

    jdecId = (ret & 0xffffff);
    for (i = 0; i < sizeof(flashInfos) / sizeof(flashInfos[0]); i++) {
        if (flashInfos[i].jedecID == jdecId) {
            ARCH_MemCpy_Fast(pFlashCfg, flashInfos[i].cfg, sizeof(SPI_Flash_Cfg_Type));
            break;
        }
    }
    if (i == sizeof(flashInfos) / sizeof(flashInfos[0])) {
        return jdecId;
    } else {
        return (jdecId | IPRO_FLASH_ID_VALID_FLAG);
    }
}

int ATTR_TCM_SECTION XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t offset;

    addr = addr & (IPRO6_FLASH_XIP_END - IPRO6_FLASH_XIP_BASE - 1);
    addr |= IPRO6_FLASH_XIP_BASE;

    offset = SF_Ctrl_Get_Flash_Image_Offset(0, SF_CTRL_FLASH_BANK0);
    SF_Ctrl_Set_Flash_Image_Offset(0, 0, SF_CTRL_FLASH_BANK0);
    /* Flash read */
    ARCH_MemCpy_Fast(data, (void *)(uintptr_t)(addr - SF_Ctrl_Get_Flash_Image_Offset(0, SF_CTRL_FLASH_BANK0)), len);
    SF_Ctrl_Set_Flash_Image_Offset(offset, 0, SF_CTRL_FLASH_BANK0);

    return SUCCESS;
}

int ATTR_TCM_SECTION XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset,
                                                   uint8_t group, uint8_t bank)
{
    /* XIP_SFlash_Delay */
    volatile uint32_t i = 32 * 2;

    while (i--)
        ;

    if (bank == SF_CTRL_FLASH_BANK1) {
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
    }
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    /* Exit form continous read for accepting command */
    SFlash_Reset_Continue_Read(pFlashCfg);
    /* For disable command that is setting register instaed of send command, we need write enable */
    SFlash_DisableBurstWrap(pFlashCfg);
    /* Enable 32Bits addr mode again in case reset command make it reset */
    SFlash_Set32BitsAddrMode(pFlashCfg, ENABLE);
    if ((pFlashCfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        /* Enable QE again in case reset command make it reset */
        SFlash_Qspi_Enable(pFlashCfg);
    }
    /* Deburst again to make sure */
    SFlash_DisableBurstWrap(pFlashCfg);

    /* Clear offset setting*/
    *offset = SF_Ctrl_Get_Flash_Image_Offset(group, bank);
    SF_Ctrl_Set_Flash_Image_Offset(0, group, bank);

    return SUCCESS;
}

int ATTR_TCM_SECTION XIP_SFlash_Erase_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t startaddr, int len,
                                                        uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Erase(pFlashCfg, startaddr, startaddr + len - 1);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

int ATTR_TCM_SECTION XIP_SFlash_Write_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len,
                                                        uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Program(pFlashCfg, ioMode, addr, data, len);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

int ATTR_TCM_SECTION XIP_SFlash_Read_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len,
                                                       uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Read(pFlashCfg, ioMode, 0, addr, data, len);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

int ATTR_TCM_SECTION XIP_SFlash_GetJedecId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data,
                                                             uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetJedecId(pFlashCfg, data);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}

int ATTR_TCM_SECTION XIP_SFlash_GetDeviceId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, int is32BitsAddr, uint8_t *data,
                                                              uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetDeviceId(data, is32BitsAddr);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}

int ATTR_TCM_SECTION XIP_SFlash_GetUniqueId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t idLen,
                                                              uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetUniqueId(data, idLen);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}

__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg,
                                                                        uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    uint8_t ioMode = (uint8_t)pFlashCfg->ioMode&0xf;

    stat=XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat=SFlash_Clear_Status_Register(pFlashCfg);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    return stat;
}

int ATTR_CLOCK_SECTION GLB_Config_WIFI_PLL(uint8_t xtalType, const GLB_WA_PLL_Cfg_Type *pllCfgList)
{
    uint8_t refClk;

    if (xtalType == GLB_XTAL_RC32M) {
        refClk = GLB_PLL_REFCLK_RC32M;
    } else {
        refClk = GLB_PLL_REFCLK_XTAL;
    }

    GLB_Power_Off_WIFIPLL();
    GLB_WIFIPLL_Ref_Clk_Sel(refClk);
    GLB_Power_On_WIFIPLL(&(pllCfgList[xtalType]), 1);

    return SUCCESS;
}

int GLB_Set_Chip_Clock_Out0_Sel(GLB_CHIP_CLK_OUT_0_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    /* set clk out0 output enable */
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_0_EN, 1);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_0_SEL, clkOutType);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Clock_Out1_Sel(GLB_CHIP_CLK_OUT_1_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    /* set clk out1 output enable */
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_1_EN, 1);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_1_SEL, clkOutType);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Clock_Out2_Sel(GLB_CHIP_CLK_OUT_2_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    /* set clk out2 output enable */
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_2_EN, 1);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_2_SEL, clkOutType);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    /* set clk out3 output enable */
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_3_EN, 1);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_3_SEL, clkOutType);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_Flash_Id_Value(uint32_t idValue)
{
    writel(((idValue&0xFFFFFF)|0x5A000000), GLB_BASE + GLB_HW_RSV1_OFFSET);

    return SUCCESS;
}

uint32_t GLB_Get_Flash_Id_Value(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_HW_RSV1_OFFSET);
    if ((tmpVal&0x7F000000) == 0x5A000000) {
        return (tmpVal&0x00FFFFFF);
    }

    return 0x00000000;
}

void ATTR_TCM_SECTION GLB_Power_Down_Ldo18ioVout(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_LDO18IO_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_PU_LDO18IO);
    writel(tmpVal, GLB_BASE + GLB_LDO18IO_OFFSET);
}

uint8_t ATTR_TCM_SECTION GLB_Get_Package_Type(void)
{
    uint32_t tmpVal = 0;
    uint8_t package_type = 0;

    /* get device_info[1:0] from efuse */
    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);

    package_type = (uint8_t)((tmpVal >> 22) & 0x3);

    return package_type;
}

int ATTR_TCM_SECTION GLB_Get_PAD_Bonging_to_GND_Sts(void)
{
    uint32_t tmpVal = 0;
    uint8_t package_cfg = 0;

    /* get package_cfg[2:0] from efuse */
    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_10_W0_OFFSET);

    package_cfg = (uint8_t)((tmpVal >> 28) & 0x7);

    if (0 == package_cfg) {
        return RESET;
    } else {
        return SET;
    }
}

int ATTR_TCM_SECTION GLB_GPIO_Pad_LeadOut_Sts(uint8_t gpioPin)
{
    uint8_t package_type;

    package_type = GLB_Get_Package_Type();

    if ( GLB_PACKAGE_TYPE_QFN56 == package_type ){
        return SET;
    }

    /* */
    if ((( gpioPin >= GLB_GPIO_PIN_0) && (gpioPin < GLB_GPIO_PIN_4))
    || (( gpioPin > GLB_GPIO_PIN_9) && (gpioPin < GLB_GPIO_PIN_18))
    || (( gpioPin > GLB_GPIO_PIN_19) && (gpioPin < GLB_GPIO_PIN_23))
    || (( gpioPin > GLB_GPIO_PIN_26) && (gpioPin < GLB_GPIO_PIN_31))) {
        return SET;
    }

    return RESET;

}

int ATTR_CLOCK_SECTION PDS_Pu_PLL_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    writel(tmpVal, PDS_BASE + PDS_PU_RST_CLKPLL_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION PDS_Pu_PLL_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    writel(tmpVal, PDS_BASE + PDS_PU_RST_CLKPLL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt)
{
    uint32_t tmpVal = 0;
    /* sleepForever set 1, chip will sleep forever */
    /* PDS sleep time 1~PDS_WARMUP_LATENCY_CNT <=> error */
    /* PDS sleep time >PDS_WARMUP_LATENCY_CNT <=> correct */
    if ((pdsSleepCnt) && (pdsSleepCnt <= PDS_WARMUP_LATENCY_CNT)) {
        return ERROR;
    } else if (pdsSleepCnt > PDS_WARMUP_LATENCY_CNT) {
        writel(pdsSleepCnt - PDS_WARMUP_LATENCY_CNT, PDS_BASE + PDS_TIME1_OFFSET);
    } else {
    }

    /* PDS_CTL4 config */
    writel(*(uint32_t *)cfg4, PDS_BASE + PDS_CTL4_OFFSET);

    /* PDS_CTL config */
    if (cfg->pdsStart) {
        /* clear pds int */
        tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
        tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_INT_CLR);
        writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

        tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_INT_CLR);
        writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

        writel((*(uint32_t *)cfg & ~(1 << 0)), PDS_BASE + PDS_CTL_OFFSET);
        writel((*(uint32_t *)cfg | (1 << 0)), PDS_BASE + PDS_CTL_OFFSET);
    } else {
        writel(*(uint32_t *)cfg, PDS_BASE + PDS_CTL_OFFSET);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt)
{
    PDS_Force_Config((PDS_CTL2_Type *)&(defaultLvCfg->pdsCtl2), (PDS_CTL3_Type *)&(defaultLvCfg->pdsCtl3), (PDS_CTL5_Type *)&(defaultLvCfg->pdsCtl5));
    PDS_Enable((PDS_CTL_Type *)&(defaultLvCfg->pdsCtl), (PDS_CTL4_Type *)&(defaultLvCfg->pdsCtl4), pdsSleepCnt);

    return SUCCESS;
}

int PDS_Reset_Low_Power_USB_Ctrl(void)
{
    uint32_t tmpVal = 0;

    /* 2000E000[19]    = 0 */
    /* 2000E00C[18:17] = 0 */
    /* 2000E014[29]    = 0 */
    /* 2000E018[21:20] = 0 */
    /* 2000E024[8]     = 0 */

    tmpVal = readl(PDS_BASE + PDS_CTL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_CTRL_USBPLL_PD);
    writel(tmpVal, PDS_BASE + PDS_CTL_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal &= ~(0x3<<17);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_CTL3_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_USB_ISO_EN);
    writel(tmpVal, PDS_BASE + PDS_CTL3_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_CTL4_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_USB_PWR_OFF);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_USB_RESET);
    writel(tmpVal, PDS_BASE + PDS_CTL4_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_CTL5_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_CTRL_USB33);
    writel(tmpVal, PDS_BASE + PDS_CTL5_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Get_Xtal_Value(uint32_t *xtalVal)
{
    uint32_t tmpVal = 0;
    uint8_t xtalType = 0;

    if (NULL == xtalVal) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    if (HBN_XTAL_FLAG_VALUE == reg_get_bits(tmpVal, HBN_XTAL_STS)) {
        xtalType = reg_get_bits(tmpVal, HBN_XTAL_TYPE);
        switch (xtalType) {
            case GLB_XTAL_NONE:
                *xtalVal = 0;
                break;
            case GLB_XTAL_24M:
                *xtalVal = 24000000;
                break;
            case GLB_XTAL_32M:
                *xtalVal = 32000000;
                break;
            case GLB_XTAL_38P4M:
                *xtalVal = 38400000;
                break;
            case GLB_XTAL_40M:
                *xtalVal = 40000000;
                break;
            case GLB_XTAL_26M:
                *xtalVal = 26000000;
                break;
            case GLB_XTAL_RC32M:
                *xtalVal = 32000000;
                break;
            default:
                *xtalVal = 0;
                break;
        }
        return SUCCESS;
    }

    *xtalVal = 0;
    return ERROR;
}

int HBN_Enable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    if (acompId == AON_ACOMP0_ID) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP0_EN);
        tmpVal2 = tmpVal2 | edge;
        tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    } else if (acompId == AON_ACOMP1_ID) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP1_EN);
        tmpVal2 = tmpVal2 | edge;
        tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    }

    return SUCCESS;
}

int HBN_Disable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    if (acompId == (uint8_t)AON_ACOMP0_ID) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP0_EN);
        tmpVal2 = tmpVal2 & (~edge);
        tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    } else if (acompId == (uint8_t)AON_ACOMP1_ID) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP1_EN);
        tmpVal2 = tmpVal2 & (~edge);
        tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    }

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_32K_Sel(uint8_t clkType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_F32K_SEL, clkType);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal;

    /* disable ADC clock first */
    tmpVal = readl(GLB_BASE + GLB_ADC_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    writel(tmpVal, GLB_BASE + GLB_ADC_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_ADC_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPADC_32M_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_GPADC_32M_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_ADC_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_ADC_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_ADC_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_I2C_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_I2C_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_I2C_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_I2C_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_I2C_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_I2C_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_I2C_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_I2C_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_I2C_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_I2C_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_I2C_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_SPI_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_SPI_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_SPI_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_SPI_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_SPI_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_SPI_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_DBI_CLK(uint8_t enable, GLB_DBI_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_DBI_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_DBI_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_DBI_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DBI_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_DBI_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_DBI_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_DBI_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DBI_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_DBI_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DBI_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_DBI_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_Audio_AUTO_CLK(uint8_t divEn)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG0_OFFSET);
    if (divEn) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_AUDIO_ADC_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG1_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_AUDIO_SOLO_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG1_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG1_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG1_OFFSET);

    return SUCCESS;
}

int GLB_Set_CAM_CLK(uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CAM_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_CAM_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CAM_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_CAM_REF_CLK_SRC_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_CAM_REF_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CAM_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CAM_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CAM_CFG0_OFFSET);

    return SUCCESS;
}


int GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, GLB_PSRAMB_PLL_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PSRAM_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_PSRAM_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_PSRAM_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_PSRAMB_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_PSRAMB_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_PSRAM_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_PSRAM_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_PSRAM_CFG0_OFFSET);

    return SUCCESS;
}

void ATTR_TCM_SECTION HBN_Mode_Enter(HBN_APP_CFG_Type *cfg)
{
    uint32_t valLow = 0, valHigh = 0;
    uint64_t val;

    if (cfg->useXtal32k) {
        HBN_32K_Sel(HBN_32K_XTAL);
    } else {
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
    }

    /* always disable HBN pin pull up/down to reduce PDS/HBN current, 0x4000F014[16]=0 */
    HBN_Hw_Pu_Pd_Cfg(DISABLE);

    HBN_Pin_WakeUp_Mask(~(cfg->gpioWakeupSrc));
    if (cfg->gpioWakeupSrc != 0) {
        HBN_GPIO_INT_Enable(cfg->gpioTrigType);
    }

    /* HBN RTC config and enable */
    HBN_Clear_RTC_Counter();
    if (cfg->sleepTime != 0) {
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        val = valLow + ((uint64_t)valHigh << 32);
        val += cfg->sleepTime;
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, val & 0xffffffff, val >> 32, HBN_RTC_COMP_BIT0_39);
        HBN_Enable_RTC_Counter();
    }

    HBN_Power_Down_Flash(cfg->flashCfg);

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);

    HBN_Enable(cfg->gpioWakeupSrc, cfg->ldoLevel, cfg->hbnLevel, cfg->dcdcPuSeq);
}

void ATTR_TCM_SECTION HBN_Power_Down_Flash(SPI_Flash_Cfg_Type *flashCfg)
{
    SPI_Flash_Cfg_Type bhFlashCfg;

    if (flashCfg == NULL) {
        L1C_DCache_Invalid_By_Addr(IPRO6_FLASH_XIP_BASE + 8 + 4, sizeof(SPI_Flash_Cfg_Type));
        XIP_SFlash_Read_Via_Cache_Need_Lock(IPRO6_FLASH_XIP_BASE + 8 + 4, (uint8_t *)(&bhFlashCfg), sizeof(SPI_Flash_Cfg_Type));
        L1C_DCache_Invalid_By_Addr(IPRO6_FLASH_XIP_BASE + 8 + 4, sizeof(SPI_Flash_Cfg_Type));

        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        SFlash_Reset_Continue_Read(&bhFlashCfg);
    } else {
        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        SFlash_Reset_Continue_Read(flashCfg);
    }

    SFlash_Powerdown();
}

void ATTR_TCM_SECTION HBN_Enable(uint32_t aGPIOIeCfg, uint8_t ldoLevel, HBN_LEVEL_Type hbnLevel, uint8_t dcdcPuSeq)
{
    uint32_t tmpVal;

    /* Setting from guide */
    /* HBN_SRAM Retion */
    tmpVal = readl(HBN_BASE + HBN_SRAM_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_RETRAM_RET);
    tmpVal = reg_clr_bit(tmpVal, HBN_RETRAM_SLP);
    writel(tmpVal, HBN_BASE + HBN_SRAM_OFFSET);

    /* AON GPIO IE */
    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_REG_EN_HW_PU_PD);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    /* HBN mode LDO level */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO11_AON_VOUT_SEL, ldoLevel);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO11_RT_VOUT_SEL, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* power on dcdc18 sequence  */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    if (dcdcPuSeq) {
        tmpVal = reg_set_bit(tmpVal, HBN_PU_DCDC18_AON);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_PU_DCDC18_AON);
    }
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* Select RC32M */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, 0);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Set HBN flag */
    writel(HBN_STATUS_ENTER_FLAG, HBN_BASE + HBN_RSV0_OFFSET);

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbnLevel) {
        case HBN_LEVEL_0:
            tmpVal = reg_clr_bit(tmpVal, HBN_PWRDN_HBN_CORE);
            break;

        case HBN_LEVEL_1:
            tmpVal = reg_set_bit(tmpVal, HBN_PWRDN_HBN_CORE);
            break;

        default:
            break;
    }
    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = reg_clr_bit(tmpVal, HBN_PWR_ON_OPTION);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* Enable HBN mode */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_MODE);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    while (1) {
        arch_delay_ms(1000);
    }
}

int HBN_PIR_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_PIR_EN);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);

    return SUCCESS;
}

int HBN_PIR_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_PIR_EN);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);

    return SUCCESS;
}

int HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg)
{
    uint32_t tmpVal;
    uint32_t bit4 = 0;
    uint32_t bit5 = 0;
    uint32_t bitVal = 0;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);

    /* low trigger interrupt */
    if (pirIntCfg->lowIntEn == ENABLE) {
        bit5 = 0;
    } else {
        bit5 = 1;
    }

    /* high trigger interrupt */
    if (pirIntCfg->highIntEn == ENABLE) {
        bit4 = 0;
    } else {
        bit4 = 1;
    }

    bitVal = bit4 | (bit5 << 1);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_DIS, bitVal);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);

    return SUCCESS;
}

int HBN_PIR_LPF_Sel(HBN_PIR_LPF_Type lpf)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_LPF_SEL, lpf);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);

    return SUCCESS;
}

int HBN_PIR_HPF_Sel(HBN_PIR_HPF_Type hpf)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_HPF_SEL, hpf);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);

    return SUCCESS;
}

int HBN_Set_PIR_Threshold(uint16_t threshold)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_VTH_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_VTH, threshold);
    writel(tmpVal, HBN_BASE + HBN_PIR_VTH_OFFSET);

    return SUCCESS;
}

int HBN_Set_PIR_Interval(uint16_t interval)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_INTERVAL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_INTERVAL, interval);
    writel(tmpVal, HBN_BASE + HBN_PIR_INTERVAL_OFFSET);

    return SUCCESS;
}

int HBN_Set_BOD_Config(uint8_t enable, HBN_BOD_THRES_Type threshold, HBN_BOD_MODE_Type mode)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_BOR_CFG_OFFSET);
    if (enable) {
        tmpVal = reg_set_bits(tmpVal, HBN_PU_BOD, 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, HBN_PU_BOD, 0);
    }
    tmpVal = reg_set_bits(tmpVal, HBN_BOD_VTH, threshold);
    tmpVal = reg_set_bits(tmpVal, HBN_BOD_SEL, mode);
    writel(tmpVal, HBN_BASE + HBN_BOR_CFG_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Keep_On_RC32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_RC32K);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Gpio_Keep(uint8_t gpioKeep)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_GPIO_KEEP_STS, HBN_GPIO_KEEP_FLAG);
    tmpVal = reg_set_bits(tmpVal, HBN_GPIO_KEEP_PIN, gpioKeep);
    writel(tmpVal, HBN_BASE + HBN_RSV3_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_GPIO_INT_Enable(uint8_t gpioIntTrigType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MODE, gpioIntTrigType);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_GPIO_INT_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MASK, 0);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2)
{
    writel(aonPadCtl1, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    writel(aonPadCtl2, HBN_BASE + HBN_PAD_CTRL_1_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, uint8_t aonGpio)
{
    uint32_t tmpVal;
    uint32_t enAonCtrlGpio;

    /* hbn_pad_ctrl_0 */
    tmpVal = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    enAonCtrlGpio = reg_get_bits(tmpVal, HBN_REG_EN_AON_CTRL_GPIO);

    if (aonPadHwCtrlEn) {
        enAonCtrlGpio |= (1 << aonGpio);
    } else {
        enAonCtrlGpio &= ~(1 << aonGpio);
    }
    tmpVal = reg_set_bits(tmpVal, HBN_REG_EN_AON_CTRL_GPIO, enAonCtrlGpio);

    writel(tmpVal, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);

    return SUCCESS;
}

int HBN_Enable_BOD_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_IRQ_BOR_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Disable_BOD_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_IRQ_BOR_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Aon_Pad_WakeUpCfg(int puPdEn, uint8_t trigMode, uint32_t maskVal, int dlyEn, uint8_t dlySec)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_EN, dlyEn);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_SEL, dlySec);
    tmpVal = reg_set_bits(tmpVal, HBN_REG_EN_HW_PU_PD, puPdEn);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MODE, trigMode);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg)
{
    uint32_t tmpVal = 0;

    if (cfg->enableBodInt) {
        HBN_Enable_BOD_IRQ();
    } else {
        HBN_Disable_BOD_IRQ();
    }

    tmpVal = readl(HBN_BASE + HBN_BOR_CFG_OFFSET);

    tmpVal = reg_set_bits(tmpVal, HBN_BOD_VTH, cfg->bodThreshold);
    if (cfg->enablePorInBod) {
        tmpVal = reg_set_bit(tmpVal, HBN_BOD_SEL);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_BOD_SEL);
    }

    if (cfg->enableBod) {
        tmpVal = reg_set_bit(tmpVal, HBN_PU_BOD);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_PU_BOD);
    }

    writel(tmpVal, HBN_BASE + HBN_BOR_CFG_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_Float_LDO15_RF(void)
{
    uint32_t tmpVal=0;

    /* ldo15rf pulldown select by aon */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    /* ldo15rf float output */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_LDO15RF_PULLDOWN_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_Pulldown_LDO15_RF(void)
{
    uint32_t tmpVal=0;

    /* ldo15rf pulldown select by aon */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    /* ldo15rf pulldown output to ground */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_LDO15RF_PULLDOWN_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_LDO15_RF_Ctrl_by_Pd(void)
{
    uint32_t tmpVal=0;

    /* output ldo15rf pulldown select by pd signal */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

#define GLB_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }
const uint32_t ATTR_CLOCK_CONST_SECTION usbPllSdmin_12M = 0x28000;
const uint32_t ATTR_CLOCK_CONST_SECTION sscDivSdmin_24M = 0x28000;

void glb_40M_delay_us(uint32_t us)
{
    for (uint32_t i = 0; i < us; i++) {
        GLB_CLK_SET_DUMMY_WAIT;
        GLB_CLK_SET_DUMMY_WAIT;
        GLB_CLK_SET_DUMMY_WAIT;
    }
}

int ATTR_CLOCK_SECTION GLB_Fast_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable)
{
    uint32_t REG_PLL_BASE_ADDRESS = 0;
    uint32_t tmpVal = 0;

    REG_PLL_BASE_ADDRESS = CCI_BASE + CCI_WIFI_PLL_CFG0_OFFSET;

    /* Step1:config parameter */
    /* cfg1:Set wifipll_refclk_sel and wifipll_refdiv_ratio */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 1);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_REFDIV_RATIO, cfg->basicCfg->clkpllRefdivRatio);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 1);

    /* cfg2:Set wifipll_int_frac_sw,wifipll_icp_1u,wifipll_icp_5u */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 2);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_INT_FRAC_SW, cfg->basicCfg->clkpllIntFracSw);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_ICP_1U, cfg->basicCfg->clkpllIcp1u);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_ICP_5U, cfg->basicCfg->clkpllIcp5u);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 2);

    /* cfg3:Set wifipll_rz,wifipll_cz,wifipll_c3,wifipll_r4_short,wifipll_r4_en */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 3);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_RZ, cfg->basicCfg->clkpllRz);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_CZ, cfg->basicCfg->clkpllCz);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_C3, cfg->basicCfg->clkpllC3);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_R4_SHORT, cfg->basicCfg->clkpllR4Short);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_C4_EN, cfg->basicCfg->clkpllC4En);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 3);

    /* cfg4:Set wifipll_sel_sample_clk */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 4);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SEL_SAMPLE_CLK, cfg->basicCfg->clkpllSelSampleClk);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 4);

    /* cfg5:Set wifipll_vco_speed */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 5);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_VCO_SPEED, cfg->basicCfg->clkpllVcoSpeed);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 5);

    /* cfg6:Set wifipll_sdm_bypass,wifipll_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 6);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_CTRL_HW, cfg->basicCfg->clkpllSdmCtrlHw);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_BYPASS, cfg->basicCfg->clkpllSdmBypass);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDMIN, cfg->clkpllSdmin);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 6);

    /* cfg10:always set usbpll_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 10);
    tmpVal = reg_set_bits(tmpVal, CCI_USBPLL_SDMIN, usbPllSdmin_12M);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 10);

    /* cfg12:always set sscdiv_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 12);
    tmpVal = reg_set_bits(tmpVal, CCI_SSCDIV_SDMIN, sscDivSdmin_24M);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 12);

    /* Step2:config pu */
    /* cfg0 : pu_wifipll_sfreg=1 */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_WIFIPLL_SFREG, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* delay > 2us */
    glb_40M_delay_us(3);

    /* cfg0 : pu_wifipll=1 */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_WIFIPLL, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* delay > 2us */
    glb_40M_delay_us(3);

    /* toggle sdm_reset (pulse 0 > 1us) */
    /* cfg0 : wifipll_sdm_reset */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    glb_40M_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 0);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    glb_40M_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* Step3:reset pll */
    /* cfg0 : toggle wifipll_reset_fbdv, pulse 0 > 1us */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    glb_40M_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 0);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    glb_40M_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* Step4:enable output clock */
    /* cfg8 : wifipll clock enable */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 8);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV3);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV4);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV5);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV6);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV8);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV10);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV12);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV20);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV30);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 8);

    if (waitStable) {
        /* Wait 1.5*30us    */
        glb_40M_delay_us(45);
    }

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Fast_Power_On_XTAL_40M_And_WIFIPLL(void)
{
    uint32_t tmpVal;
    volatile uint8_t refClk;

    refClk = GLB_PLL_REFCLK_XTAL;

    /* power on xtal first */
    tmpVal = readl(AON_BASE + AON_TSEN_OFFSET);
    if (!reg_is_bit_set(tmpVal, AON_XTAL_RDY) ){
        AON_Power_On_XTAL();
    }

    // HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    //HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);

    //HBN_Set_Xtal_Type(GLB_XTAL_40M);

    /* power on wifipll */
    GLB_Power_Off_WIFIPLL();
    GLB_WIFIPLL_Ref_Clk_Sel(refClk);
    GLB_Fast_Power_On_WIFIPLL(&wifiPllCfg_960M_Fast[0], 0);

    glb_40M_delay_us(30);

    /* if power on xtal, always set xclk from xtal */
    // HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);

    /* enable all PLL clock output */
    /* GLB reg_pll_en = 1, cannot be zero */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_PLL_EN);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Fast_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout = 1024;

    /* config hclk_div=mcuClkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_HCLK_DIV, mcuClkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);

    /* config bclk_div=mcuPBclkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV, mcuPBclkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);
    /* bclk act pulse */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG1_OFFSET);

    timeout = 1024;
    do {
        tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
        tmpVal = reg_get_bits(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Fast_Set_MCU_System_CLK(uint8_t clkFreq)
{
    uint32_t tmpVal;

    // 
    /* get xclk&&rootclk clock */
    // mcuXclkSel = HBN_Get_MCU_XCLK_Sel();

    // /* change root clock to rc32m */
    // HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    // HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    // GLB_Set_MCU_System_CLK_Div(0, 0);

    /* select pll output clock before select root clock */
    tmpVal = readl(PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);
#if 0
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 3);
            break;
        default:
            break;
    }
#else
    tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 3);
#endif
    writel(tmpVal, PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);

#if 0
    /* select root clock */
    switch (clkFreq) {
        // case GLB_MCU_SYS_CLK_RC32M:
        //     GLB_Set_MCU_System_CLK_Div(0, 0);
        //     HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
        //     HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
        //     break;
        // case GLB_MCU_SYS_CLK_XTAL:
        //     GLB_Set_MCU_System_CLK_Div(0, 0);
        //     HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
        //     HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
        //     break;
        // case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
        //     /* For high speed, set DIV first */
        //     GLB_Set_MCU_System_CLK_Div(0, 2);
        //     /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
        //     /* MCU_MISC_IROM_2T_Access_Set(0); */
        //     /* unuseful for mcu, useful for dsp, just for safe */
        //     GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_240M);
        //     HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
        //     /* recover xclk */
        //     HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
        //     break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(3, 0);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            // HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        default:
            break;
    }
#else
    /* For high speed, set DIV first */
    GLB_Fast_Set_MCU_System_CLK_Div(3, 0);
    /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
    /* MCU_MISC_IROM_2T_Access_Set(0); */
    //GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
    writel(readl(0x2000058C)|(1<<14), 0x2000058C);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
    writel(readl(0x2000f030)|(1<<1), 0x2000f030);

#endif

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

void EF_Ctrl_Program_Direct(uint32_t offset, uint32_t *pword, uint32_t count)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + offset);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Add delay for CLK to be stable */
    arch_delay_us(4);

    if (pword != NULL) {
        ARCH_MemCpy4(pEfuseStart0, pword, count);
    }

    EF_Ctrl_Program_Efuse_0();
}

void EF_Ctrl_Read_Direct(uint32_t offset, uint32_t *pword, uint32_t count)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + offset);

    EF_Ctrl_Load_Efuse_R0();

    ARCH_MemCpy4(pword, pEfuseStart0, count);
}

int ATTR_TCM_SECTION HBN_Recal_RC32K(int32_t expected_counter, int32_t actual_counter)
{
    int32_t tmpVal = 0;
    int32_t current = 0;
    int32_t delta = 0;

    delta = actual_counter - expected_counter;

    /* normalize to 1s count */
    delta = (delta * 32768) / expected_counter;

    if ((delta < 32) && (delta > -32)) {
        return -1;
    } if (delta < -320) {
        delta = -320;
    } else if (delta > 320) {
        delta = 320;
    }
    /* we adjust one code for delta 32 */
    delta = delta / 32;

    tmpVal = readl(HBN_BASE + HBN_RC32K_CTRL0_OFFSET);
    current = reg_get_bits(tmpVal, HBN_RC32K_CODE_FR_EXT);
    current += delta;
    current &= ((1U << HBN_RC32K_CODE_FR_EXT_LEN) - 1);
    tmpVal = reg_set_bits(tmpVal, HBN_RC32K_CODE_FR_EXT, current);
    writel(tmpVal, HBN_BASE + HBN_RC32K_CTRL0_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Simple_Set_MCU_System_CLK(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout;

    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);

    /* select pll output clock before select root clock */
    tmpVal = readl(PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 2);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 3);
            break;
        default:
            break;
    }
    writel(tmpVal, PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);

    /* clock div */
    /* config hclk_div=mcuClkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_HCLK_DIV, mcuClkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);
    /* config bclk_div=mcuPBclkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV, mcuPBclkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);
    /* bclk act pulse */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG1_OFFSET);
    timeout = 1024;
    do {
        tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
        tmpVal = reg_get_bits(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    /* select root clock */
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_RC32M:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_240M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        default:
            break;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_Pulldown_DCDC18(void)
{
    uint32_t tmpVal=0;

    tmpVal = readl(AON_BASE + AON_DCDC_TOP_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_DCDC_PULLDOWN_AON);
    writel(tmpVal, AON_BASE + AON_DCDC_TOP_1_OFFSET);
    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_Float_DCDC18(void)
{
    uint32_t tmpVal=0;

    tmpVal = readl(AON_BASE + AON_DCDC_TOP_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_DCDC_PULLDOWN_AON);
    writel(tmpVal, AON_BASE + AON_DCDC_TOP_1_OFFSET);

    return SUCCESS;
}

int PDS_Power_Off_WB(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_FORCE_WB_ISO_EN);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_FORCE_WB_PWR_OFF);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    return SUCCESS;
}

int PDS_Power_On_WB(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_FORCE_WB_PWR_OFF);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_FORCE_WB_ISO_EN);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    return SUCCESS;
}

void HBN_Get_Reset_Event(uint8_t* event)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);

    *event = reg_get_bits(tmpVal, HBN_RESET_EVENT);
}

void HBN_Clr_Reset_Event(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);

    tmpVal |= (1<<13);

    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
}

void EF_Ctrl_Program_Direct_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Add delay for CLK to be stable */
    arch_delay_us(4);

    if (data != NULL) {
        ARCH_MemCpy4(pEfuseStart0 + index, data, len);
    }

    EF_Ctrl_Program_Efuse_0();
}

void EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo)
{
    uint32_t tmpVal;
    uint32_t *p = (uint32_t *)deviceInfo;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);
    *p = tmpVal;
}

void EF_Ctrl_Read_Direct_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);

    EF_CTRL_LOAD_BEFORE_READ_R0;

    ARCH_MemCpy4(data, pEfuseStart0 + index, len);
}

int ATTR_CLOCK_SECTION GLB_Set_Slave_Grp_0_CLK(GLB_SLAVE_GRP_0_Type slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div)
{
    uint32_t tmpVal;
    uint32_t regAddr = GLB_BASE + glb_slave_grp_0_table[slave].clkOffSetAddr;
    uint8_t enPos = glb_slave_grp_0_table[slave].clkEnPos;
    uint8_t selPos = glb_slave_grp_0_table[slave].clkSelPos;
    uint8_t divPos = glb_slave_grp_0_table[slave].clkDivPos;
    uint8_t enLen = glb_slave_grp_0_table[slave].clkEnLen;
    uint8_t selLen = glb_slave_grp_0_table[slave].clkSelLen;
    uint8_t divLen = glb_slave_grp_0_table[slave].clkDivLen;

    /* disable clock first */
    if (enLen) {
        tmpVal = readl(regAddr);
        tmpVal &= ~(((1ULL << enLen) - 1) << enPos);
        writel(tmpVal, regAddr);
    }

    /* set clock sel or div */
    tmpVal = readl(regAddr);
    if (selLen) {
        tmpVal &= ~(((1ULL << selLen) - 1) << selPos);
        tmpVal |= (((clkSel.clkSel) & ((1 << selLen) - 1)) << selPos);
    }
    if (divLen) {
        tmpVal &= ~(((1ULL << divLen) - 1) << divPos);
        tmpVal |= ((div & ((1 << divLen) - 1)) << divPos);
    }
    writel(tmpVal, regAddr);

    /* enable clock */
    if (enLen && enable) {
        tmpVal = readl(regAddr);
        tmpVal |= (((1ULL << enLen) - 1) << enPos);
        writel(tmpVal, regAddr);
    }

    return SUCCESS;
}

int GLB_GPIO_Clr(uint8_t gpioPin)
{
    if (gpioPin < GLB_GPIO_PIN_32) {
        writel(1 << gpioPin, GLB_BASE + GLB_GPIO_CFG140_OFFSET);
    } else {
        writel(1 << (gpioPin - GLB_GPIO_PIN_32), GLB_BASE + GLB_GPIO_CFG141_OFFSET);
    }

    return SUCCESS;
}

int GLB_GPIO_Set(uint8_t gpioPin)
{
    if (RESET == GLB_GPIO_Pad_LeadOut_Sts(gpioPin)) {
        return ERROR;
    }

    if (gpioPin < GLB_GPIO_PIN_32) {
        writel(1 << gpioPin, GLB_BASE + GLB_GPIO_CFG138_OFFSET);
    } else {
        writel(1 << (gpioPin - GLB_GPIO_PIN_32), GLB_BASE + GLB_GPIO_CFG139_OFFSET);
    }

    return SUCCESS;
}

int HBN_Disable_AComp0_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_IRQ_ACOMP0_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Disable_AComp1_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_IRQ_ACOMP1_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Enable_AComp0_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_IRQ_ACOMP0_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Enable_AComp1_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_IRQ_ACOMP1_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Reset_Reason(uint16_t rstReason)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_FLASH_POWER_STS, HBN_RESET_REASON_FLAG);
    tmpVal = reg_set_bits(tmpVal, HBN_FLASH_POWER_DLY, rstReason);
    writel(tmpVal, HBN_BASE + HBN_RSV3_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Get_Reset_Reason(uint16_t *rstReason)
{
    uint32_t tmpVal = 0;

    if (NULL == rstReason) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    if (HBN_RESET_REASON_FLAG == reg_get_bits(tmpVal, HBN_FLASH_POWER_STS)) {
        *rstReason = reg_get_bits(tmpVal, HBN_FLASH_POWER_DLY);
        return SUCCESS;
    }

    return ERROR;

}

/* Undef macro from drv_l1c.h to provide a real function symbol for ROM API */
#undef L1C_ICache_Invalid_By_Addr
__WEAK
int ATTR_TCM_SECTION L1C_ICache_Invalid_By_Addr(uintptr_t addr, uint32_t len)
{
#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1) && \
    defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
    MInvalICache();
#endif
    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(uint8_t grp, uint8_t pu, uint8_t pd, uint8_t ie)
{
    uint32_t tmpVal;
    uint32_t tmpValPu;
    uint32_t tmpValPd;
    uint32_t tmpValIe;

    /* pu/pd/ie config */
    tmpVal = readl(PDS_BASE + PDS_GPIO_I_SET_OFFSET);
    tmpValPu = reg_get_bits(tmpVal, PDS_CR_PDS_GPIO_PU_SET);
    if (pu) {
        if (GLB_PACKAGE_TYPE_QFN56 != GLB_Get_Package_Type()) {
            if (SET == GLB_Get_PAD_Bonging_to_GND_Sts()) {
                /* IF it is 40 PACKAGE TYPE and PAD Bonging to GND, Pull-UP is forbidden */
                tmpValPu &= ~(1 << grp);
            } else {
                tmpValPu |= (1 << grp);
            }
        } else {
            tmpValPu |= (1 << grp);
        }
    } else {
        tmpValPu &= ~(1 << grp);
    }
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_GPIO_PU_SET, tmpValPu);
    tmpValPd = reg_get_bits(tmpVal, PDS_CR_PDS_GPIO_PD_SET);
    if (pd) {
        tmpValPd |= (1 << grp);
    } else {
        tmpValPd &= ~(1 << grp);
    }
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_GPIO_PD_SET, tmpValPd);
    tmpValIe = reg_get_bits(tmpVal, PDS_CR_PDS_GPIO_IE_SET);
    if (ie) {
        tmpValIe |= (1 << grp);
    } else {
        tmpValIe &= ~(1 << grp);
    }
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_GPIO_IE_SET, tmpValIe);
    writel(tmpVal, PDS_BASE + PDS_GPIO_I_SET_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMask(uint8_t pad, int intMask)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    /* [15:0] : GPIO0~15   */
    /* [30:16] : GPIO20~34 */
    if (pad <= GLB_GPIO_PIN_15) {
        pos = pad;
    } else if ((pad >= GLB_GPIO_PIN_20) && (pad <= GLB_GPIO_PIN_34)) {
        pos = pad - 4;
    } else {
        return ERROR;
    }

    tmpVal = readl(PDS_BASE + PDS_GPIO_PD_SET_OFFSET);
    if (UNMASK != intMask) {
        /* MASK */
        tmpVal |= (1 << pos);
    } else {
        /* UNMASK */
        tmpVal &= ~(1 << pos);
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_PD_SET_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Get_GPIO_Pad_IntStatus(uint8_t pad)
{
    uint32_t pos = 0;

    /* [15:0]  : GPIO0~15  */
    /* [30:16] : GPIO20~34 */
    if (pad <= GLB_GPIO_PIN_15) {
        pos = pad;
    } else if ((pad >= GLB_GPIO_PIN_20) && (pad <= GLB_GPIO_PIN_34)) {
        pos = pad - 4;
    } else {
        return RESET;
    }

    return (readl(PDS_BASE + PDS_GPIO_STAT_OFFSET) & (1 << pos)) ? SET : RESET;
}

int GLB_BMX_TO_Init(BMX_TO_Cfg_Type *BmxCfg)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_BMX_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BMX_TIMEOUT_EN, BmxCfg->timeoutEn);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BMX_ARB_MODE, BmxCfg->arbMod);
    writel(tmpVal, GLB_BASE + GLB_BMX_CFG0_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(BMX_MCU_TO_IRQn, BMX_TO_IRQHandler);
#endif

    return SUCCESS;
}

int UART_ApplyAbrResult(uint8_t uartId, uint8_t autoBaudDet)
{
    uint32_t UARTx = uartAddr[uartId];
    uint16_t tmpVal;

    /* Check the parameters */

    /* Get detection value */
    tmpVal = UART_GetAutoBaudCount(uartId, autoBaudDet);

    /* Set tx baudrate */
    writel(tmpVal << 0x10 | tmpVal, UARTx + UART_BIT_PRD_OFFSET);

    return SUCCESS;
}

void ATTR_TCM_SECTION SF_Ctrl_AES_Get_IV_BE(uint8_t region, uint8_t *iv)
{
    uint32_t regionRegBase = SF_CTRL_BASE + SF_CTRL_AES_REGION_OFFSET + region * 0x80;
    uint32_t iv_w[4] = {0};

    if (iv != NULL) {
        iv_w[0] = readl(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
        iv_w[1] = readl(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET);
        iv_w[2] = readl(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET);
        iv_w[3] = readl(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);
        arch_memcpy(iv, (uint8_t *)&iv_w, sizeof(iv_w));
    }
}

void ATTR_TCM_SECTION SF_Ctrl_AES_Get_IV_LE(uint8_t region, uint8_t *iv)
{
    uint32_t regionRegBase = SF_CTRL_BASE + SF_CTRL_AES_REGION_OFFSET + region * 0x80;
    uint32_t iv_w[4] = {0};

    if (iv != NULL) {
        iv_w[3] = __REV(readl(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET));
        iv_w[2] = __REV(readl(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET));
        iv_w[1] = __REV(readl(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET));
        iv_w[0] = __REV(readl(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET));
        arch_memcpy(iv, (uint8_t *)&iv_w, sizeof(iv_w));
    }
}

int32_t ATTR_TCM_SECTION SF_Ctrl_AES_Set_Decrypt_Region_BE(SF_Ctrl_Decrypt_Type *parm)
{
    uint8_t temp_iv[16] = {0};
    uint8_t *p_iv = NULL;
    uint32_t mstatus = 0;
    uint32_t irq_flag = 0;

    /* assert, sf_ctrl support max flash size 64M */
    if((NULL==parm)||((parm->addr+parm->len)>(64*1024*1024))){
        return -1;
    }

    /* lock */
    mstatus = __get_MSTATUS();
    if(0x8&mstatus){
        irq_flag = 1;
        __disable_irq();
    }

    if(parm->len){
        if(NULL==parm->iv){
            /* get iv from region 0 */
            SF_Ctrl_AES_Get_IV_BE(0, temp_iv);
            p_iv = temp_iv;
        }else{
            p_iv = parm->iv;
        }

        /* set decrypt */
        SF_Ctrl_AES_Disable();
        if(SF_CTRL_AES_XTS_MODE == parm->mode){
            /* aes xts mode */
            SF_Ctrl_Disable_Wrap_Access(0);
            SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_XTS_MODE);
            SF_Ctrl_AES_XTS_Set_Key_BE(parm->aes_region, NULL, parm->type);
            SF_Ctrl_AES_XTS_Set_IV_BE(parm->aes_region, p_iv, parm->addr);
        }else{
            /* aes ctr mode */
            SF_Ctrl_Disable_Wrap_Access(1);
            SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_CTR_MODE);
            SF_Ctrl_AES_Set_Key_BE(parm->aes_region, NULL, parm->type);
            SF_Ctrl_AES_Set_IV_BE(parm->aes_region, p_iv, parm->addr);
        }
        SF_Ctrl_AES_Set_Region(parm->aes_region, 1 /*enable this region*/, 1 /*hardware key*/, parm->addr, parm->addr+parm->len-1, 0 /*lock*/);
        SF_Ctrl_AES_Enable_BE();
        SF_Ctrl_AES_Enable();
    }

    /* unlock */
    if(irq_flag){
        __enable_irq();
    }

    return 0;
}

int32_t ATTR_TCM_SECTION SF_Ctrl_AES_Set_Decrypt_Region_LE(SF_Ctrl_Decrypt_Type *parm)
{
    uint8_t temp_iv[16] = {0};
    uint8_t *p_iv = NULL;
    uint32_t mstatus = 0;
    uint32_t irq_flag = 0;

    /* assert, sf_ctrl support max flash size 64M */
    if((NULL==parm)||((parm->addr+parm->len)>(64*1024*1024))){
        return -1;
    }

    /* lock */
    mstatus = __get_MSTATUS();
    if(0x8&mstatus){
        irq_flag = 1;
        __disable_irq();
    }

    if(parm->len){
        if(NULL==parm->iv){
            /* get iv from region 0 */
            SF_Ctrl_AES_Get_IV_LE(0, temp_iv);
            p_iv = temp_iv;
        }else{
            p_iv = parm->iv;
        }

        /* set decrypt */
        SF_Ctrl_AES_Disable();
        if(SF_CTRL_AES_XTS_MODE == parm->mode){
            /* aes xts mode */
            SF_Ctrl_Disable_Wrap_Access(0);
            SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_XTS_MODE);
            SF_Ctrl_AES_XTS_Set_Key(parm->aes_region, NULL, parm->type);
            SF_Ctrl_AES_XTS_Set_IV(parm->aes_region, p_iv, parm->addr);
        }else{
            /* aes ctr mode */
            SF_Ctrl_Disable_Wrap_Access(1);
            SF_Ctrl_AES_Set_Mode(SF_CTRL_AES_CTR_MODE);
            SF_Ctrl_AES_Set_Key(parm->aes_region, NULL, parm->type);
            SF_Ctrl_AES_Set_IV(parm->aes_region, p_iv, parm->addr);
        }
        SF_Ctrl_AES_Set_Region(parm->aes_region, 1 /*enable this region*/, 1 /*hardware key*/, parm->addr, parm->addr+parm->len-1, 0 /*lock*/);
        SF_Ctrl_AES_Enable_LE();
        SF_Ctrl_AES_Enable();
    }

    /* unlock */
    if(irq_flag){
        __enable_irq();
    }

    return 0;
}

int ATTR_CLOCK_SECTION HBN_Power_On_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_XTAL32K_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, HBN_XTAL32K_HIZ_EN);
    tmpVal = reg_set_bits(tmpVal, HBN_XTAL32K_INV_STRE, 3);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_XTAL32K);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_XTAL32K_BUF);
    writel(tmpVal, HBN_BASE + HBN_XTAL32K_OFFSET);

    /* Delay >1s */
    arch_delay_us(1100);

    return SUCCESS;
}
