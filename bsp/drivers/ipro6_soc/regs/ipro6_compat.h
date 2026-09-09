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
 * @file ipro6_compat.h
 * @brief IPRO6 Compatibility Stubs
 *
 * This header provides stub definitions for peripherals/features
 * that exist in other chips (IPRO7, IPRO6LE) but not in IPRO6.
 * This allows shared driver code to compile without modification.
 */

#ifndef __IPRO6_COMPAT_H__
#define __IPRO6_COMPAT_H__

/*============================================================================
 * Audio PLL - Not available on IPRO6
 * (IPRO6 only has WIFI PLL)
 *============================================================================*/
#define CCI_AUDIO_PLL_CFG0_OFFSET   (0xFFFF)  /* Invalid offset - not available */
#define CCI_AUDIO_PLL_CFG1_OFFSET   (0xFFFF)
#define CCI_AUDIO_PLL_CFG6_OFFSET   (0xFFFF)

#define CCI_AUPLL_SDMIN             (0)
#define CCI_AUPLL_SDMIN_POS         (0U)
#define CCI_AUPLL_SDMIN_LEN         (1U)
#define CCI_AUPLL_SDMIN_MSK         (0)
#define CCI_AUPLL_SDMIN_UMSK        (0xFFFFFFFF)

#define CCI_AUPLL_POSTDIV           (0)
#define CCI_AUPLL_POSTDIV_POS       (0U)
#define CCI_AUPLL_POSTDIV_LEN       (1U)
#define CCI_AUPLL_POSTDIV_MSK       (0)
#define CCI_AUPLL_POSTDIV_UMSK      (0xFFFFFFFF)


/*============================================================================
 * Audio PLL - CCI_AUPLL_* stubs (not available on IPRO6)
 *============================================================================*/
#define CCI_PU_AUPLL                    (0)
#define CCI_PU_AUPLL_POS                (0U)
#define CCI_PU_AUPLL_LEN                (1U)
#define CCI_PU_AUPLL_MSK                (0)
#define CCI_PU_AUPLL_UMSK               (0xFFFFFFFF)

#define CCI_PU_AUPLL_SFREG              (0)
#define CCI_PU_AUPLL_SFREG_POS          (0U)
#define CCI_PU_AUPLL_SFREG_LEN          (1U)
#define CCI_PU_AUPLL_SFREG_MSK          (0)
#define CCI_PU_AUPLL_SFREG_UMSK         (0xFFFFFFFF)

#define CCI_AUPLL_REFCLK_SEL            (0)
#define CCI_AUPLL_REFCLK_SEL_POS        (0U)
#define CCI_AUPLL_REFCLK_SEL_LEN        (1U)
#define CCI_AUPLL_REFCLK_SEL_MSK        (0)
#define CCI_AUPLL_REFCLK_SEL_UMSK       (0xFFFFFFFF)

#define CCI_AUPLL_REFDIV_RATIO          (0)
#define CCI_AUPLL_REFDIV_RATIO_POS      (0U)
#define CCI_AUPLL_REFDIV_RATIO_LEN      (1U)
#define CCI_AUPLL_REFDIV_RATIO_MSK      (0)
#define CCI_AUPLL_REFDIV_RATIO_UMSK     (0xFFFFFFFF)

#define CCI_AUPLL_INT_FRAC_SW           (0)
#define CCI_AUPLL_INT_FRAC_SW_POS       (0U)
#define CCI_AUPLL_INT_FRAC_SW_LEN       (1U)
#define CCI_AUPLL_INT_FRAC_SW_MSK       (0)
#define CCI_AUPLL_INT_FRAC_SW_UMSK      (0xFFFFFFFF)

#define CCI_AUPLL_ICP_1U                (0)
#define CCI_AUPLL_ICP_1U_POS            (0U)
#define CCI_AUPLL_ICP_1U_LEN            (1U)
#define CCI_AUPLL_ICP_1U_MSK            (0)
#define CCI_AUPLL_ICP_1U_UMSK           (0xFFFFFFFF)

#define CCI_AUPLL_ICP_5U                (0)
#define CCI_AUPLL_ICP_5U_POS            (0U)
#define CCI_AUPLL_ICP_5U_LEN            (1U)
#define CCI_AUPLL_ICP_5U_MSK            (0)
#define CCI_AUPLL_ICP_5U_UMSK           (0xFFFFFFFF)

#define CCI_AUPLL_RZ                    (0)
#define CCI_AUPLL_RZ_POS                (0U)
#define CCI_AUPLL_RZ_LEN                (1U)
#define CCI_AUPLL_RZ_MSK                (0)
#define CCI_AUPLL_RZ_UMSK               (0xFFFFFFFF)

#define CCI_AUPLL_CZ                    (0)
#define CCI_AUPLL_CZ_POS                (0U)
#define CCI_AUPLL_CZ_LEN                (1U)
#define CCI_AUPLL_CZ_MSK                (0)
#define CCI_AUPLL_CZ_UMSK               (0xFFFFFFFF)

#define CCI_AUPLL_C3                    (0)
#define CCI_AUPLL_C3_POS                (0U)
#define CCI_AUPLL_C3_LEN                (1U)
#define CCI_AUPLL_C3_MSK                (0)
#define CCI_AUPLL_C3_UMSK               (0xFFFFFFFF)

#define CCI_AUPLL_R4_SHORT              (0)
#define CCI_AUPLL_R4_SHORT_POS          (0U)
#define CCI_AUPLL_R4_SHORT_LEN          (1U)
#define CCI_AUPLL_R4_SHORT_MSK          (0)
#define CCI_AUPLL_R4_SHORT_UMSK         (0xFFFFFFFF)

#define CCI_AUPLL_R4_EN                 (0)
#define CCI_AUPLL_R4_EN_POS             (0U)
#define CCI_AUPLL_R4_EN_LEN             (1U)
#define CCI_AUPLL_R4_EN_MSK             (0)
#define CCI_AUPLL_R4_EN_UMSK            (0xFFFFFFFF)

#define CCI_AUPLL_SEL_SAMPLE_CLK        (0)
#define CCI_AUPLL_SEL_SAMPLE_CLK_POS    (0U)
#define CCI_AUPLL_SEL_SAMPLE_CLK_LEN    (1U)
#define CCI_AUPLL_SEL_SAMPLE_CLK_MSK    (0)
#define CCI_AUPLL_SEL_SAMPLE_CLK_UMSK   (0xFFFFFFFF)

#define CCI_AUPLL_VCO_SPEED             (0)
#define CCI_AUPLL_VCO_SPEED_POS         (0U)
#define CCI_AUPLL_VCO_SPEED_LEN         (1U)
#define CCI_AUPLL_VCO_SPEED_MSK         (0)
#define CCI_AUPLL_VCO_SPEED_UMSK        (0xFFFFFFFF)

#define CCI_AUPLL_SDM_RSTB              (0)
#define CCI_AUPLL_SDM_RSTB_POS          (0U)
#define CCI_AUPLL_SDM_RSTB_LEN          (1U)
#define CCI_AUPLL_SDM_RSTB_MSK          (0)
#define CCI_AUPLL_SDM_RSTB_UMSK         (0xFFFFFFFF)

#define CCI_AUPLL_SDM_BYPASS            (0)
#define CCI_AUPLL_SDM_BYPASS_POS        (0U)
#define CCI_AUPLL_SDM_BYPASS_LEN        (1U)
#define CCI_AUPLL_SDM_BYPASS_MSK        (0)
#define CCI_AUPLL_SDM_BYPASS_UMSK       (0xFFFFFFFF)

#define CCI_AUPLL_FBDV_RSTB             (0)
#define CCI_AUPLL_FBDV_RSTB_POS         (0U)
#define CCI_AUPLL_FBDV_RSTB_LEN         (1U)
#define CCI_AUPLL_FBDV_RSTB_MSK         (0)
#define CCI_AUPLL_FBDV_RSTB_UMSK        (0xFFFFFFFF)

#define CCI_AUPLL_EN_DIV1_POS           (0U)
#define CCI_AUPLL_EN_DIV2_POS           (0U)
#define CCI_AUPLL_EN_DIV2P5_POS         (0U)
#define CCI_AUPLL_EN_DIV3_POS           (0U)
#define CCI_AUPLL_EN_DIV4_POS           (0U)
#define CCI_AUPLL_EN_DIV5_POS           (0U)
#define CCI_AUPLL_EN_DIV6_POS           (0U)
#define CCI_AUPLL_EN_DIV10_POS          (0U)
#define CCI_AUPLL_EN_DIV15_POS          (0U)

#define CCI_AUPLL_C4_EN                 (0)
#define CCI_AUPLL_C4_EN_POS             (0U)
#define CCI_AUPLL_C4_EN_LEN             (1U)
#define CCI_AUPLL_C4_EN_MSK             (0)
#define CCI_AUPLL_C4_EN_UMSK            (0xFFFFFFFF)

/*============================================================================
 * SDIO - IPRO6 does not have full SDIO support
 *============================================================================*/
#define GLB_REG_SDIO_INT_SYS_DIS        (0)
#define GLB_REG_SDIO_INT_SYS_DIS_POS    (0U)
#define GLB_REG_SDIO_INT_SYS_DIS_LEN    (1U)
#define GLB_REG_SDIO_INT_SYS_DIS_MSK    (0)
#define GLB_REG_SDIO_INT_SYS_DIS_UMSK   (0xFFFFFFFF)

/*============================================================================
 * GPIP GPADC - FIFO ready interrupt not available on IPRO6
 *============================================================================*/
#define GPIP_GPADC_FIFO_RDY                 (0)
#define GPIP_GPADC_FIFO_RDY_POS             (7U)
#define GPIP_GPADC_FIFO_RDY_LEN             (1U)
#define GPIP_GPADC_FIFO_RDY_MSK             (0)
#define GPIP_GPADC_FIFO_RDY_UMSK            (0xFFFFFFFF)

/*============================================================================
 * HBN RETRAM - old names mapped to new register structure
 * (Old code uses HBN_RETRAM_RET, new has HBN_RETRAM_RET1/RET2)
 *============================================================================*/
#define HBN_RETRAM_RET                  HBN_RETRAM_RET1
#define HBN_RETRAM_RET_POS              HBN_RETRAM_RET1_POS
#define HBN_RETRAM_RET_LEN              HBN_RETRAM_RET1_LEN
#define HBN_RETRAM_RET_MSK              HBN_RETRAM_RET1_MSK
#define HBN_RETRAM_RET_UMSK             HBN_RETRAM_RET1_UMSK

/* HBN_RETRAM_SLP - map to HBN_SRAM_SLP (similar purpose) */
#define HBN_RETRAM_SLP                  HBN_SRAM_SLP
#define HBN_RETRAM_SLP_POS              HBN_SRAM_SLP_POS
#define HBN_RETRAM_SLP_LEN              HBN_SRAM_SLP_LEN
#define HBN_RETRAM_SLP_MSK              HBN_SRAM_SLP_MSK
#define HBN_RETRAM_SLP_UMSK             HBN_SRAM_SLP_UMSK

#endif /* __IPRO6_COMPAT_H__ */
