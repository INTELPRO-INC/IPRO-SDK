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
 * @file reg_crm.h
 * @brief Definitions of the CRM HW block registers and register access functions.
 *
 * @defgroup REG_CRM REG_CRM
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the CRM HW block registers and register access functions.
 */
#ifndef _REG_CRM_H_
#define _REG_CRM_H_

#include "co_int.h"
#include "_reg_crm.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_CRM peripheral.
 */
#define REG_CRM_COUNT 210

/** @brief Decoding mask of the REG_CRM peripheral registers from the CPU point of view.
 */
#define REG_CRM_DECODING_MASK 0x000003FF

/**
 * @name CLKRST_CNTL register definitions
 * <table>
 * <caption>CLKRST_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:24 <td>      LACLK_FREQ_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 23:22 <td>  LDPCTXCLK_FREQ_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 21:20 <td>  LDPCRXCLK_FREQ_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 19:18 <td>     VTBCLK_FREQ_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 17:16 <td>    MPIFCLK_FREQ_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 13:12 <td>  MACWTCLK_RATIO_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 11:08 <td> MACCORECLK_FREQ_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 07:06 <td>     DACCLK_FREQ_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 05:04 <td>      FECLK_FREQ_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 00    <td>          PLFCLK_SEL <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the CLKRST_CNTL register
#define CRM_CLKRST_CNTL_ADDR   0x24940008
/// Offset of the CLKRST_CNTL register from the base address
#define CRM_CLKRST_CNTL_OFFSET 0x00000008
/// Index of the CLKRST_CNTL register
#define CRM_CLKRST_CNTL_INDEX  0x00000002
/// Reset value of the CLKRST_CNTL register
#define CRM_CLKRST_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKRST_CNTL register.
 * The CLKRST_CNTL register will be read and its value returned.
 * @return The current value of the CLKRST_CNTL register.
 */
__INLINE uint32_t crm_clkrst_cntl_get(void)
{
    return REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
}

/**
 * @brief Sets the CLKRST_CNTL register to a value.
 * The CLKRST_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_clkrst_cntl_set(uint32_t value)
{
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, value);
}

// field definitions
/// LACLK_FREQ_SEL field mask
#define CRM_LACLK_FREQ_SEL_MASK        ((uint32_t)0x03000000)
/// LACLK_FREQ_SEL field LSB position
#define CRM_LACLK_FREQ_SEL_LSB         24
/// LACLK_FREQ_SEL field width
#define CRM_LACLK_FREQ_SEL_WIDTH       ((uint32_t)0x00000002)
/// LDPCTXCLK_FREQ_SEL field mask
#define CRM_LDPCTXCLK_FREQ_SEL_MASK    ((uint32_t)0x00C00000)
/// LDPCTXCLK_FREQ_SEL field LSB position
#define CRM_LDPCTXCLK_FREQ_SEL_LSB     22
/// LDPCTXCLK_FREQ_SEL field width
#define CRM_LDPCTXCLK_FREQ_SEL_WIDTH   ((uint32_t)0x00000002)
/// LDPCRXCLK_FREQ_SEL field mask
#define CRM_LDPCRXCLK_FREQ_SEL_MASK    ((uint32_t)0x00300000)
/// LDPCRXCLK_FREQ_SEL field LSB position
#define CRM_LDPCRXCLK_FREQ_SEL_LSB     20
/// LDPCRXCLK_FREQ_SEL field width
#define CRM_LDPCRXCLK_FREQ_SEL_WIDTH   ((uint32_t)0x00000002)
/// VTBCLK_FREQ_SEL field mask
#define CRM_VTBCLK_FREQ_SEL_MASK       ((uint32_t)0x000C0000)
/// VTBCLK_FREQ_SEL field LSB position
#define CRM_VTBCLK_FREQ_SEL_LSB        18
/// VTBCLK_FREQ_SEL field width
#define CRM_VTBCLK_FREQ_SEL_WIDTH      ((uint32_t)0x00000002)
/// MPIFCLK_FREQ_SEL field mask
#define CRM_MPIFCLK_FREQ_SEL_MASK      ((uint32_t)0x00030000)
/// MPIFCLK_FREQ_SEL field LSB position
#define CRM_MPIFCLK_FREQ_SEL_LSB       16
/// MPIFCLK_FREQ_SEL field width
#define CRM_MPIFCLK_FREQ_SEL_WIDTH     ((uint32_t)0x00000002)
/// MACWTCLK_RATIO_SEL field mask
#define CRM_MACWTCLK_RATIO_SEL_MASK    ((uint32_t)0x00003000)
/// MACWTCLK_RATIO_SEL field LSB position
#define CRM_MACWTCLK_RATIO_SEL_LSB     12
/// MACWTCLK_RATIO_SEL field width
#define CRM_MACWTCLK_RATIO_SEL_WIDTH   ((uint32_t)0x00000002)
/// MACCORECLK_FREQ_SEL field mask
#define CRM_MACCORECLK_FREQ_SEL_MASK   ((uint32_t)0x00000F00)
/// MACCORECLK_FREQ_SEL field LSB position
#define CRM_MACCORECLK_FREQ_SEL_LSB    8
/// MACCORECLK_FREQ_SEL field width
#define CRM_MACCORECLK_FREQ_SEL_WIDTH  ((uint32_t)0x00000004)
/// DACCLK_FREQ_SEL field mask
#define CRM_DACCLK_FREQ_SEL_MASK       ((uint32_t)0x000000C0)
/// DACCLK_FREQ_SEL field LSB position
#define CRM_DACCLK_FREQ_SEL_LSB        6
/// DACCLK_FREQ_SEL field width
#define CRM_DACCLK_FREQ_SEL_WIDTH      ((uint32_t)0x00000002)
/// FECLK_FREQ_SEL field mask
#define CRM_FECLK_FREQ_SEL_MASK        ((uint32_t)0x00000030)
/// FECLK_FREQ_SEL field LSB position
#define CRM_FECLK_FREQ_SEL_LSB         4
/// FECLK_FREQ_SEL field width
#define CRM_FECLK_FREQ_SEL_WIDTH       ((uint32_t)0x00000002)
/// PLFCLK_SEL field bit
#define CRM_PLFCLK_SEL_BIT             ((uint32_t)0x00000001)
/// PLFCLK_SEL field position
#define CRM_PLFCLK_SEL_POS             0

/// LACLK_FREQ_SEL field reset value
#define CRM_LACLK_FREQ_SEL_RST         0x0
/// LDPCTXCLK_FREQ_SEL field reset value
#define CRM_LDPCTXCLK_FREQ_SEL_RST     0x0
/// LDPCRXCLK_FREQ_SEL field reset value
#define CRM_LDPCRXCLK_FREQ_SEL_RST     0x0
/// VTBCLK_FREQ_SEL field reset value
#define CRM_VTBCLK_FREQ_SEL_RST        0x0
/// MPIFCLK_FREQ_SEL field reset value
#define CRM_MPIFCLK_FREQ_SEL_RST       0x0
/// MACWTCLK_RATIO_SEL field reset value
#define CRM_MACWTCLK_RATIO_SEL_RST     0x0
/// MACCORECLK_FREQ_SEL field reset value
#define CRM_MACCORECLK_FREQ_SEL_RST    0x0
/// DACCLK_FREQ_SEL field reset value
#define CRM_DACCLK_FREQ_SEL_RST        0x0
/// FECLK_FREQ_SEL field reset value
#define CRM_FECLK_FREQ_SEL_RST         0x0
/// PLFCLK_SEL field reset value
#define CRM_PLFCLK_SEL_RST             0x0

/**
 * @brief Constructs a value for the CLKRST_CNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] laclkfreqsel - The value to use for the LACLK_FREQ_SEL field.
 * @param[in] ldpctxclkfreqsel - The value to use for the LDPCTXCLK_FREQ_SEL field.
 * @param[in] ldpcrxclkfreqsel - The value to use for the LDPCRXCLK_FREQ_SEL field.
 * @param[in] vtbclkfreqsel - The value to use for the VTBCLK_FREQ_SEL field.
 * @param[in] mpifclkfreqsel - The value to use for the MPIFCLK_FREQ_SEL field.
 * @param[in] macwtclkratiosel - The value to use for the MACWTCLK_RATIO_SEL field.
 * @param[in] maccoreclkfreqsel - The value to use for the MACCORECLK_FREQ_SEL field.
 * @param[in] dacclkfreqsel - The value to use for the DACCLK_FREQ_SEL field.
 * @param[in] feclkfreqsel - The value to use for the FECLK_FREQ_SEL field.
 * @param[in] plfclksel - The value to use for the PLFCLK_SEL field.
 */
__INLINE void crm_clkrst_cntl_pack(uint8_t laclkfreqsel, uint8_t ldpctxclkfreqsel, uint8_t ldpcrxclkfreqsel, uint8_t vtbclkfreqsel, uint8_t mpifclkfreqsel, uint8_t macwtclkratiosel, uint8_t maccoreclkfreqsel, uint8_t dacclkfreqsel, uint8_t feclkfreqsel, uint8_t plfclksel)
{
    ASSERT_ERR((((uint32_t)laclkfreqsel << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpctxclkfreqsel << 22) & ~((uint32_t)0x00C00000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcrxclkfreqsel << 20) & ~((uint32_t)0x00300000)) == 0);
    ASSERT_ERR((((uint32_t)vtbclkfreqsel << 18) & ~((uint32_t)0x000C0000)) == 0);
    ASSERT_ERR((((uint32_t)mpifclkfreqsel << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)macwtclkratiosel << 12) & ~((uint32_t)0x00003000)) == 0);
    ASSERT_ERR((((uint32_t)maccoreclkfreqsel << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)dacclkfreqsel << 6) & ~((uint32_t)0x000000C0)) == 0);
    ASSERT_ERR((((uint32_t)feclkfreqsel << 4) & ~((uint32_t)0x00000030)) == 0);
    ASSERT_ERR((((uint32_t)plfclksel << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR,  ((uint32_t)laclkfreqsel << 24) | ((uint32_t)ldpctxclkfreqsel << 22) | ((uint32_t)ldpcrxclkfreqsel << 20) | ((uint32_t)vtbclkfreqsel << 18) | ((uint32_t)mpifclkfreqsel << 16) | ((uint32_t)macwtclkratiosel << 12) | ((uint32_t)maccoreclkfreqsel << 8) | ((uint32_t)dacclkfreqsel << 6) | ((uint32_t)feclkfreqsel << 4) | ((uint32_t)plfclksel << 0));
}

/**
 * @brief Unpacks CLKRST_CNTL's fields from current value of the CLKRST_CNTL register.
 *
 * Reads the CLKRST_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] laclkfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] ldpctxclkfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] ldpcrxclkfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] vtbclkfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] mpifclkfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] macwtclkratiosel - Will be populated with the current value of this field from the register.
 * @param[out] maccoreclkfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] dacclkfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] feclkfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] plfclksel - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_clkrst_cntl_unpack(uint8_t* laclkfreqsel, uint8_t* ldpctxclkfreqsel, uint8_t* ldpcrxclkfreqsel, uint8_t* vtbclkfreqsel, uint8_t* mpifclkfreqsel, uint8_t* macwtclkratiosel, uint8_t* maccoreclkfreqsel, uint8_t* dacclkfreqsel, uint8_t* feclkfreqsel, uint8_t* plfclksel)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);

    *laclkfreqsel = (localVal & ((uint32_t)0x03000000)) >> 24;
    *ldpctxclkfreqsel = (localVal & ((uint32_t)0x00C00000)) >> 22;
    *ldpcrxclkfreqsel = (localVal & ((uint32_t)0x00300000)) >> 20;
    *vtbclkfreqsel = (localVal & ((uint32_t)0x000C0000)) >> 18;
    *mpifclkfreqsel = (localVal & ((uint32_t)0x00030000)) >> 16;
    *macwtclkratiosel = (localVal & ((uint32_t)0x00003000)) >> 12;
    *maccoreclkfreqsel = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *dacclkfreqsel = (localVal & ((uint32_t)0x000000C0)) >> 6;
    *feclkfreqsel = (localVal & ((uint32_t)0x00000030)) >> 4;
    *plfclksel = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the LACLK_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the LACLK_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the LACLK_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_laclk_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Sets the LACLK_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] laclkfreqsel - The value to set the field to.
 */
__INLINE void crm_laclk_freq_sel_setf(uint8_t laclkfreqsel)
{
    ASSERT_ERR((((uint32_t)laclkfreqsel << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)laclkfreqsel << 24));
}

/**
 * @brief Returns the current value of the LDPCTXCLK_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the LDPCTXCLK_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the LDPCTXCLK_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_ldpctxclk_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00C00000)) >> 22);
}

/**
 * @brief Sets the LDPCTXCLK_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpctxclkfreqsel - The value to set the field to.
 */
__INLINE void crm_ldpctxclk_freq_sel_setf(uint8_t ldpctxclkfreqsel)
{
    ASSERT_ERR((((uint32_t)ldpctxclkfreqsel << 22) & ~((uint32_t)0x00C00000)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00C00000)) | ((uint32_t)ldpctxclkfreqsel << 22));
}

/**
 * @brief Returns the current value of the LDPCRXCLK_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the LDPCRXCLK_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the LDPCRXCLK_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_ldpcrxclk_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

/**
 * @brief Sets the LDPCRXCLK_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcrxclkfreqsel - The value to set the field to.
 */
__INLINE void crm_ldpcrxclk_freq_sel_setf(uint8_t ldpcrxclkfreqsel)
{
    ASSERT_ERR((((uint32_t)ldpcrxclkfreqsel << 20) & ~((uint32_t)0x00300000)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00300000)) | ((uint32_t)ldpcrxclkfreqsel << 20));
}

/**
 * @brief Returns the current value of the VTBCLK_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the VTBCLK_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the VTBCLK_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_vtbclk_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x000C0000)) >> 18);
}

/**
 * @brief Sets the VTBCLK_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vtbclkfreqsel - The value to set the field to.
 */
__INLINE void crm_vtbclk_freq_sel_setf(uint8_t vtbclkfreqsel)
{
    ASSERT_ERR((((uint32_t)vtbclkfreqsel << 18) & ~((uint32_t)0x000C0000)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x000C0000)) | ((uint32_t)vtbclkfreqsel << 18));
}

/**
 * @brief Returns the current value of the MPIFCLK_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the MPIFCLK_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the MPIFCLK_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_mpifclk_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the MPIFCLK_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mpifclkfreqsel - The value to set the field to.
 */
__INLINE void crm_mpifclk_freq_sel_setf(uint8_t mpifclkfreqsel)
{
    ASSERT_ERR((((uint32_t)mpifclkfreqsel << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)mpifclkfreqsel << 16));
}

/**
 * @brief Returns the current value of the MACWTCLK_RATIO_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the MACWTCLK_RATIO_SEL field's value will be returned.
 *
 * @return The current value of the MACWTCLK_RATIO_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_macwtclk_ratio_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

/**
 * @brief Sets the MACWTCLK_RATIO_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macwtclkratiosel - The value to set the field to.
 */
__INLINE void crm_macwtclk_ratio_sel_setf(uint8_t macwtclkratiosel)
{
    ASSERT_ERR((((uint32_t)macwtclkratiosel << 12) & ~((uint32_t)0x00003000)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00003000)) | ((uint32_t)macwtclkratiosel << 12));
}

/**
 * @brief Returns the current value of the MACCORECLK_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the MACCORECLK_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the MACCORECLK_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_maccoreclk_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the MACCORECLK_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccoreclkfreqsel - The value to set the field to.
 */
__INLINE void crm_maccoreclk_freq_sel_setf(uint8_t maccoreclkfreqsel)
{
    ASSERT_ERR((((uint32_t)maccoreclkfreqsel << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)maccoreclkfreqsel << 8));
}

/**
 * @brief Returns the current value of the DACCLK_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the DACCLK_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the DACCLK_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_dacclk_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x000000C0)) >> 6);
}

/**
 * @brief Sets the DACCLK_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacclkfreqsel - The value to set the field to.
 */
__INLINE void crm_dacclk_freq_sel_setf(uint8_t dacclkfreqsel)
{
    ASSERT_ERR((((uint32_t)dacclkfreqsel << 6) & ~((uint32_t)0x000000C0)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x000000C0)) | ((uint32_t)dacclkfreqsel << 6));
}

/**
 * @brief Returns the current value of the FECLK_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the FECLK_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the FECLK_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_feclk_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

/**
 * @brief Sets the FECLK_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] feclkfreqsel - The value to set the field to.
 */
__INLINE void crm_feclk_freq_sel_setf(uint8_t feclkfreqsel)
{
    ASSERT_ERR((((uint32_t)feclkfreqsel << 4) & ~((uint32_t)0x00000030)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00000030)) | ((uint32_t)feclkfreqsel << 4));
}

/**
 * @brief Returns the current value of the PLFCLK_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the PLFCLK_SEL field's value will be returned.
 *
 * @return The current value of the PLFCLK_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_plfclk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the PLFCLK_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] plfclksel - The value to set the field to.
 */
__INLINE void crm_plfclk_sel_setf(uint8_t plfclksel)
{
    ASSERT_ERR((((uint32_t)plfclksel << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)plfclksel << 0));
}

/// @}

/**
 * @name CLKRST_STAT register definitions
 * <table>
 * <caption>CLKRST_STAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 10:04 <td> MMC_LOCK <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the CLKRST_STAT register
#define CRM_CLKRST_STAT_ADDR   0x2494000C
/// Offset of the CLKRST_STAT register from the base address
#define CRM_CLKRST_STAT_OFFSET 0x0000000C
/// Index of the CLKRST_STAT register
#define CRM_CLKRST_STAT_INDEX  0x00000003
/// Reset value of the CLKRST_STAT register
#define CRM_CLKRST_STAT_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKRST_STAT register.
 * The CLKRST_STAT register will be read and its value returned.
 * @return The current value of the CLKRST_STAT register.
 */
__INLINE uint32_t crm_clkrst_stat_get(void)
{
    return REG_PL_RD(CRM_CLKRST_STAT_ADDR);
}

// field definitions
/// MMC_LOCK field mask
#define CRM_MMC_LOCK_MASK   ((uint32_t)0x000007F0)
/// MMC_LOCK field LSB position
#define CRM_MMC_LOCK_LSB    4
/// MMC_LOCK field width
#define CRM_MMC_LOCK_WIDTH  ((uint32_t)0x00000007)

/// MMC_LOCK field reset value
#define CRM_MMC_LOCK_RST    0x0

/**
 * @brief Returns the current value of the MMC_LOCK field in the CLKRST_STAT register.
 *
 * The CLKRST_STAT register will be read and the MMC_LOCK field's value will be returned.
 *
 * @return The current value of the MMC_LOCK field in the CLKRST_STAT register.
 */
__INLINE uint8_t crm_mmc_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_STAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000007F0)) == 0);
    return (localVal >> 4);
}

/// @}

/**
 * @name CLKGATEPHYFCTRL0 register definitions
 * <table>
 * <caption>CLKGATEPHYFCTRL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31 <td>   PHYTXCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 29 <td>  AGCMEMCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 28 <td>     AGCCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 27 <td>      RCCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 24 <td>      FECLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 23 <td>     FDOCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 22 <td>     EQUCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 20 <td>  PHYSVDCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 18 <td>  TDCOMPCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 17 <td> TDFOESTCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 16 <td>     TBECLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 09 <td>     FFTCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 06 <td>   CHESTCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 01 <td>     VTBCLKFORCE <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the CLKGATEPHYFCTRL0 register
#define CRM_CLKGATEPHYFCTRL0_ADDR   0x24940010
/// Offset of the CLKGATEPHYFCTRL0 register from the base address
#define CRM_CLKGATEPHYFCTRL0_OFFSET 0x00000010
/// Index of the CLKGATEPHYFCTRL0 register
#define CRM_CLKGATEPHYFCTRL0_INDEX  0x00000004
/// Reset value of the CLKGATEPHYFCTRL0 register
#define CRM_CLKGATEPHYFCTRL0_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKGATEPHYFCTRL0 register.
 * The CLKGATEPHYFCTRL0 register will be read and its value returned.
 * @return The current value of the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint32_t crm_clkgatephyfctrl0_get(void)
{
    return REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
}

/**
 * @brief Sets the CLKGATEPHYFCTRL0 register to a value.
 * The CLKGATEPHYFCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_clkgatephyfctrl0_set(uint32_t value)
{
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, value);
}

// field definitions
/// PHYTXCLKFORCE field bit
#define CRM_PHYTXCLKFORCE_BIT      ((uint32_t)0x80000000)
/// PHYTXCLKFORCE field position
#define CRM_PHYTXCLKFORCE_POS      31
/// AGCMEMCLKFORCE field bit
#define CRM_AGCMEMCLKFORCE_BIT     ((uint32_t)0x20000000)
/// AGCMEMCLKFORCE field position
#define CRM_AGCMEMCLKFORCE_POS     29
/// AGCCLKFORCE field bit
#define CRM_AGCCLKFORCE_BIT        ((uint32_t)0x10000000)
/// AGCCLKFORCE field position
#define CRM_AGCCLKFORCE_POS        28
/// RCCLKFORCE field bit
#define CRM_RCCLKFORCE_BIT         ((uint32_t)0x08000000)
/// RCCLKFORCE field position
#define CRM_RCCLKFORCE_POS         27
/// FECLKFORCE field bit
#define CRM_FECLKFORCE_BIT         ((uint32_t)0x01000000)
/// FECLKFORCE field position
#define CRM_FECLKFORCE_POS         24
/// FDOCLKFORCE field bit
#define CRM_FDOCLKFORCE_BIT        ((uint32_t)0x00800000)
/// FDOCLKFORCE field position
#define CRM_FDOCLKFORCE_POS        23
/// EQUCLKFORCE field bit
#define CRM_EQUCLKFORCE_BIT        ((uint32_t)0x00400000)
/// EQUCLKFORCE field position
#define CRM_EQUCLKFORCE_POS        22
/// PHYSVDCLKFORCE field bit
#define CRM_PHYSVDCLKFORCE_BIT     ((uint32_t)0x00100000)
/// PHYSVDCLKFORCE field position
#define CRM_PHYSVDCLKFORCE_POS     20
/// TDCOMPCLKFORCE field bit
#define CRM_TDCOMPCLKFORCE_BIT     ((uint32_t)0x00040000)
/// TDCOMPCLKFORCE field position
#define CRM_TDCOMPCLKFORCE_POS     18
/// TDFOESTCLKFORCE field bit
#define CRM_TDFOESTCLKFORCE_BIT    ((uint32_t)0x00020000)
/// TDFOESTCLKFORCE field position
#define CRM_TDFOESTCLKFORCE_POS    17
/// TBECLKFORCE field bit
#define CRM_TBECLKFORCE_BIT        ((uint32_t)0x00010000)
/// TBECLKFORCE field position
#define CRM_TBECLKFORCE_POS        16
/// FFTCLKFORCE field bit
#define CRM_FFTCLKFORCE_BIT        ((uint32_t)0x00000200)
/// FFTCLKFORCE field position
#define CRM_FFTCLKFORCE_POS        9
/// CHESTCLKFORCE field bit
#define CRM_CHESTCLKFORCE_BIT      ((uint32_t)0x00000040)
/// CHESTCLKFORCE field position
#define CRM_CHESTCLKFORCE_POS      6
/// VTBCLKFORCE field bit
#define CRM_VTBCLKFORCE_BIT        ((uint32_t)0x00000002)
/// VTBCLKFORCE field position
#define CRM_VTBCLKFORCE_POS        1

/// PHYTXCLKFORCE field reset value
#define CRM_PHYTXCLKFORCE_RST      0x0
/// AGCMEMCLKFORCE field reset value
#define CRM_AGCMEMCLKFORCE_RST     0x0
/// AGCCLKFORCE field reset value
#define CRM_AGCCLKFORCE_RST        0x0
/// RCCLKFORCE field reset value
#define CRM_RCCLKFORCE_RST         0x0
/// FECLKFORCE field reset value
#define CRM_FECLKFORCE_RST         0x0
/// FDOCLKFORCE field reset value
#define CRM_FDOCLKFORCE_RST        0x0
/// EQUCLKFORCE field reset value
#define CRM_EQUCLKFORCE_RST        0x0
/// PHYSVDCLKFORCE field reset value
#define CRM_PHYSVDCLKFORCE_RST     0x0
/// TDCOMPCLKFORCE field reset value
#define CRM_TDCOMPCLKFORCE_RST     0x0
/// TDFOESTCLKFORCE field reset value
#define CRM_TDFOESTCLKFORCE_RST    0x0
/// TBECLKFORCE field reset value
#define CRM_TBECLKFORCE_RST        0x0
/// FFTCLKFORCE field reset value
#define CRM_FFTCLKFORCE_RST        0x0
/// CHESTCLKFORCE field reset value
#define CRM_CHESTCLKFORCE_RST      0x0
/// VTBCLKFORCE field reset value
#define CRM_VTBCLKFORCE_RST        0x0

/**
 * @brief Constructs a value for the CLKGATEPHYFCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] phytxclkforce - The value to use for the PHYTXCLKFORCE field.
 * @param[in] agcmemclkforce - The value to use for the AGCMEMCLKFORCE field.
 * @param[in] agcclkforce - The value to use for the AGCCLKFORCE field.
 * @param[in] rcclkforce - The value to use for the RCCLKFORCE field.
 * @param[in] feclkforce - The value to use for the FECLKFORCE field.
 * @param[in] fdoclkforce - The value to use for the FDOCLKFORCE field.
 * @param[in] equclkforce - The value to use for the EQUCLKFORCE field.
 * @param[in] physvdclkforce - The value to use for the PHYSVDCLKFORCE field.
 * @param[in] tdcompclkforce - The value to use for the TDCOMPCLKFORCE field.
 * @param[in] tdfoestclkforce - The value to use for the TDFOESTCLKFORCE field.
 * @param[in] tbeclkforce - The value to use for the TBECLKFORCE field.
 * @param[in] fftclkforce - The value to use for the FFTCLKFORCE field.
 * @param[in] chestclkforce - The value to use for the CHESTCLKFORCE field.
 * @param[in] vtbclkforce - The value to use for the VTBCLKFORCE field.
 */
__INLINE void crm_clkgatephyfctrl0_pack(uint8_t phytxclkforce, uint8_t agcmemclkforce, uint8_t agcclkforce, uint8_t rcclkforce, uint8_t feclkforce, uint8_t fdoclkforce, uint8_t equclkforce, uint8_t physvdclkforce, uint8_t tdcompclkforce, uint8_t tdfoestclkforce, uint8_t tbeclkforce, uint8_t fftclkforce, uint8_t chestclkforce, uint8_t vtbclkforce)
{
    ASSERT_ERR((((uint32_t)phytxclkforce << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)agcmemclkforce << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)agcclkforce << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)rcclkforce << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)feclkforce << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)fdoclkforce << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)equclkforce << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)physvdclkforce << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)tdcompclkforce << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)tdfoestclkforce << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)tbeclkforce << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)fftclkforce << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)chestclkforce << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)vtbclkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR,  ((uint32_t)phytxclkforce << 31) | ((uint32_t)agcmemclkforce << 29) | ((uint32_t)agcclkforce << 28) | ((uint32_t)rcclkforce << 27) | ((uint32_t)feclkforce << 24) | ((uint32_t)fdoclkforce << 23) | ((uint32_t)equclkforce << 22) | ((uint32_t)physvdclkforce << 20) | ((uint32_t)tdcompclkforce << 18) | ((uint32_t)tdfoestclkforce << 17) | ((uint32_t)tbeclkforce << 16) | ((uint32_t)fftclkforce << 9) | ((uint32_t)chestclkforce << 6) | ((uint32_t)vtbclkforce << 1));
}

/**
 * @brief Unpacks CLKGATEPHYFCTRL0's fields from current value of the CLKGATEPHYFCTRL0 register.
 *
 * Reads the CLKGATEPHYFCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] phytxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] agcmemclkforce - Will be populated with the current value of this field from the register.
 * @param[out] agcclkforce - Will be populated with the current value of this field from the register.
 * @param[out] rcclkforce - Will be populated with the current value of this field from the register.
 * @param[out] feclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fdoclkforce - Will be populated with the current value of this field from the register.
 * @param[out] equclkforce - Will be populated with the current value of this field from the register.
 * @param[out] physvdclkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdcompclkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdfoestclkforce - Will be populated with the current value of this field from the register.
 * @param[out] tbeclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fftclkforce - Will be populated with the current value of this field from the register.
 * @param[out] chestclkforce - Will be populated with the current value of this field from the register.
 * @param[out] vtbclkforce - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_clkgatephyfctrl0_unpack(uint8_t* phytxclkforce, uint8_t* agcmemclkforce, uint8_t* agcclkforce, uint8_t* rcclkforce, uint8_t* feclkforce, uint8_t* fdoclkforce, uint8_t* equclkforce, uint8_t* physvdclkforce, uint8_t* tdcompclkforce, uint8_t* tdfoestclkforce, uint8_t* tbeclkforce, uint8_t* fftclkforce, uint8_t* chestclkforce, uint8_t* vtbclkforce)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);

    *phytxclkforce = (localVal & ((uint32_t)0x80000000)) >> 31;
    *agcmemclkforce = (localVal & ((uint32_t)0x20000000)) >> 29;
    *agcclkforce = (localVal & ((uint32_t)0x10000000)) >> 28;
    *rcclkforce = (localVal & ((uint32_t)0x08000000)) >> 27;
    *feclkforce = (localVal & ((uint32_t)0x01000000)) >> 24;
    *fdoclkforce = (localVal & ((uint32_t)0x00800000)) >> 23;
    *equclkforce = (localVal & ((uint32_t)0x00400000)) >> 22;
    *physvdclkforce = (localVal & ((uint32_t)0x00100000)) >> 20;
    *tdcompclkforce = (localVal & ((uint32_t)0x00040000)) >> 18;
    *tdfoestclkforce = (localVal & ((uint32_t)0x00020000)) >> 17;
    *tbeclkforce = (localVal & ((uint32_t)0x00010000)) >> 16;
    *fftclkforce = (localVal & ((uint32_t)0x00000200)) >> 9;
    *chestclkforce = (localVal & ((uint32_t)0x00000040)) >> 6;
    *vtbclkforce = (localVal & ((uint32_t)0x00000002)) >> 1;
}

/**
 * @brief Returns the current value of the PHYTXCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the PHYTXCLKFORCE field's value will be returned.
 *
 * @return The current value of the PHYTXCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_phytxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the PHYTXCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phytxclkforce - The value to set the field to.
 */
__INLINE void crm_phytxclkforce_setf(uint8_t phytxclkforce)
{
    ASSERT_ERR((((uint32_t)phytxclkforce << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)phytxclkforce << 31));
}

/**
 * @brief Returns the current value of the AGCMEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the AGCMEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the AGCMEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_agcmemclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the AGCMEMCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcmemclkforce - The value to set the field to.
 */
__INLINE void crm_agcmemclkforce_setf(uint8_t agcmemclkforce)
{
    ASSERT_ERR((((uint32_t)agcmemclkforce << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)agcmemclkforce << 29));
}

/**
 * @brief Returns the current value of the AGCCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the AGCCLKFORCE field's value will be returned.
 *
 * @return The current value of the AGCCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_agcclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the AGCCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcclkforce - The value to set the field to.
 */
__INLINE void crm_agcclkforce_setf(uint8_t agcclkforce)
{
    ASSERT_ERR((((uint32_t)agcclkforce << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)agcclkforce << 28));
}

/**
 * @brief Returns the current value of the RCCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the RCCLKFORCE field's value will be returned.
 *
 * @return The current value of the RCCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_rcclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the RCCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rcclkforce - The value to set the field to.
 */
__INLINE void crm_rcclkforce_setf(uint8_t rcclkforce)
{
    ASSERT_ERR((((uint32_t)rcclkforce << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)rcclkforce << 27));
}

/**
 * @brief Returns the current value of the FECLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FECLKFORCE field's value will be returned.
 *
 * @return The current value of the FECLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_feclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the FECLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] feclkforce - The value to set the field to.
 */
__INLINE void crm_feclkforce_setf(uint8_t feclkforce)
{
    ASSERT_ERR((((uint32_t)feclkforce << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)feclkforce << 24));
}

/**
 * @brief Returns the current value of the FDOCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FDOCLKFORCE field's value will be returned.
 *
 * @return The current value of the FDOCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_fdoclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the FDOCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fdoclkforce - The value to set the field to.
 */
__INLINE void crm_fdoclkforce_setf(uint8_t fdoclkforce)
{
    ASSERT_ERR((((uint32_t)fdoclkforce << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)fdoclkforce << 23));
}

/**
 * @brief Returns the current value of the EQUCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the EQUCLKFORCE field's value will be returned.
 *
 * @return The current value of the EQUCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_equclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the EQUCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] equclkforce - The value to set the field to.
 */
__INLINE void crm_equclkforce_setf(uint8_t equclkforce)
{
    ASSERT_ERR((((uint32_t)equclkforce << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)equclkforce << 22));
}

/**
 * @brief Returns the current value of the PHYSVDCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the PHYSVDCLKFORCE field's value will be returned.
 *
 * @return The current value of the PHYSVDCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_physvdclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the PHYSVDCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] physvdclkforce - The value to set the field to.
 */
__INLINE void crm_physvdclkforce_setf(uint8_t physvdclkforce)
{
    ASSERT_ERR((((uint32_t)physvdclkforce << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)physvdclkforce << 20));
}

/**
 * @brief Returns the current value of the TDCOMPCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the TDCOMPCLKFORCE field's value will be returned.
 *
 * @return The current value of the TDCOMPCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_tdcompclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the TDCOMPCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcompclkforce - The value to set the field to.
 */
__INLINE void crm_tdcompclkforce_setf(uint8_t tdcompclkforce)
{
    ASSERT_ERR((((uint32_t)tdcompclkforce << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)tdcompclkforce << 18));
}

/**
 * @brief Returns the current value of the TDFOESTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the TDFOESTCLKFORCE field's value will be returned.
 *
 * @return The current value of the TDFOESTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_tdfoestclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the TDFOESTCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfoestclkforce - The value to set the field to.
 */
__INLINE void crm_tdfoestclkforce_setf(uint8_t tdfoestclkforce)
{
    ASSERT_ERR((((uint32_t)tdfoestclkforce << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)tdfoestclkforce << 17));
}

/**
 * @brief Returns the current value of the TBECLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the TBECLKFORCE field's value will be returned.
 *
 * @return The current value of the TBECLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_tbeclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the TBECLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbeclkforce - The value to set the field to.
 */
__INLINE void crm_tbeclkforce_setf(uint8_t tbeclkforce)
{
    ASSERT_ERR((((uint32_t)tbeclkforce << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)tbeclkforce << 16));
}

/**
 * @brief Returns the current value of the FFTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FFTCLKFORCE field's value will be returned.
 *
 * @return The current value of the FFTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_fftclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the FFTCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fftclkforce - The value to set the field to.
 */
__INLINE void crm_fftclkforce_setf(uint8_t fftclkforce)
{
    ASSERT_ERR((((uint32_t)fftclkforce << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)fftclkforce << 9));
}

/**
 * @brief Returns the current value of the CHESTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the CHESTCLKFORCE field's value will be returned.
 *
 * @return The current value of the CHESTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_chestclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the CHESTCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chestclkforce - The value to set the field to.
 */
__INLINE void crm_chestclkforce_setf(uint8_t chestclkforce)
{
    ASSERT_ERR((((uint32_t)chestclkforce << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)chestclkforce << 6));
}

/**
 * @brief Returns the current value of the VTBCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the VTBCLKFORCE field's value will be returned.
 *
 * @return The current value of the VTBCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_vtbclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the VTBCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vtbclkforce - The value to set the field to.
 */
__INLINE void crm_vtbclkforce_setf(uint8_t vtbclkforce)
{
    ASSERT_ERR((((uint32_t)vtbclkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)vtbclkforce << 1));
}

/// @}

/**
 * @name CLKGATEPHYFCTRL1 register definitions
 * <table>
 * <caption>CLKGATEPHYFCTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 08 <td>       LACLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 05 <td> RADARTIMCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 04 <td>   MDMBTXCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 03 <td>   MDMBRXCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 02 <td>     MDMBCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 01 <td>  LDPCENCCLKFORCE <td> R <td> R/W <td> 0
 * <tr><td> 00 <td>  LDPCDECCLKFORCE <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the CLKGATEPHYFCTRL1 register
#define CRM_CLKGATEPHYFCTRL1_ADDR   0x24940014
/// Offset of the CLKGATEPHYFCTRL1 register from the base address
#define CRM_CLKGATEPHYFCTRL1_OFFSET 0x00000014
/// Index of the CLKGATEPHYFCTRL1 register
#define CRM_CLKGATEPHYFCTRL1_INDEX  0x00000005
/// Reset value of the CLKGATEPHYFCTRL1 register
#define CRM_CLKGATEPHYFCTRL1_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKGATEPHYFCTRL1 register.
 * The CLKGATEPHYFCTRL1 register will be read and its value returned.
 * @return The current value of the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint32_t crm_clkgatephyfctrl1_get(void)
{
    return REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
}

/**
 * @brief Sets the CLKGATEPHYFCTRL1 register to a value.
 * The CLKGATEPHYFCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_clkgatephyfctrl1_set(uint32_t value)
{
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, value);
}

// field definitions
/// LACLKFORCE field bit
#define CRM_LACLKFORCE_BIT          ((uint32_t)0x00000100)
/// LACLKFORCE field position
#define CRM_LACLKFORCE_POS          8
/// RADARTIMCLKFORCE field bit
#define CRM_RADARTIMCLKFORCE_BIT    ((uint32_t)0x00000020)
/// RADARTIMCLKFORCE field position
#define CRM_RADARTIMCLKFORCE_POS    5
/// MDMBTXCLKFORCE field bit
#define CRM_MDMBTXCLKFORCE_BIT      ((uint32_t)0x00000010)
/// MDMBTXCLKFORCE field position
#define CRM_MDMBTXCLKFORCE_POS      4
/// MDMBRXCLKFORCE field bit
#define CRM_MDMBRXCLKFORCE_BIT      ((uint32_t)0x00000008)
/// MDMBRXCLKFORCE field position
#define CRM_MDMBRXCLKFORCE_POS      3
/// MDMBCLKFORCE field bit
#define CRM_MDMBCLKFORCE_BIT        ((uint32_t)0x00000004)
/// MDMBCLKFORCE field position
#define CRM_MDMBCLKFORCE_POS        2
/// LDPCENCCLKFORCE field bit
#define CRM_LDPCENCCLKFORCE_BIT     ((uint32_t)0x00000002)
/// LDPCENCCLKFORCE field position
#define CRM_LDPCENCCLKFORCE_POS     1
/// LDPCDECCLKFORCE field bit
#define CRM_LDPCDECCLKFORCE_BIT     ((uint32_t)0x00000001)
/// LDPCDECCLKFORCE field position
#define CRM_LDPCDECCLKFORCE_POS     0

/// LACLKFORCE field reset value
#define CRM_LACLKFORCE_RST          0x0
/// RADARTIMCLKFORCE field reset value
#define CRM_RADARTIMCLKFORCE_RST    0x0
/// MDMBTXCLKFORCE field reset value
#define CRM_MDMBTXCLKFORCE_RST      0x0
/// MDMBRXCLKFORCE field reset value
#define CRM_MDMBRXCLKFORCE_RST      0x0
/// MDMBCLKFORCE field reset value
#define CRM_MDMBCLKFORCE_RST        0x0
/// LDPCENCCLKFORCE field reset value
#define CRM_LDPCENCCLKFORCE_RST     0x0
/// LDPCDECCLKFORCE field reset value
#define CRM_LDPCDECCLKFORCE_RST     0x0

/**
 * @brief Constructs a value for the CLKGATEPHYFCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] laclkforce - The value to use for the LACLKFORCE field.
 * @param[in] radartimclkforce - The value to use for the RADARTIMCLKFORCE field.
 * @param[in] mdmbtxclkforce - The value to use for the MDMBTXCLKFORCE field.
 * @param[in] mdmbrxclkforce - The value to use for the MDMBRXCLKFORCE field.
 * @param[in] mdmbclkforce - The value to use for the MDMBCLKFORCE field.
 * @param[in] ldpcencclkforce - The value to use for the LDPCENCCLKFORCE field.
 * @param[in] ldpcdecclkforce - The value to use for the LDPCDECCLKFORCE field.
 */
__INLINE void crm_clkgatephyfctrl1_pack(uint8_t laclkforce, uint8_t radartimclkforce, uint8_t mdmbtxclkforce, uint8_t mdmbrxclkforce, uint8_t mdmbclkforce, uint8_t ldpcencclkforce, uint8_t ldpcdecclkforce)
{
    ASSERT_ERR((((uint32_t)laclkforce << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)radartimclkforce << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)mdmbtxclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)mdmbrxclkforce << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)mdmbclkforce << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)ldpcencclkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecclkforce << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR,  ((uint32_t)laclkforce << 8) | ((uint32_t)radartimclkforce << 5) | ((uint32_t)mdmbtxclkforce << 4) | ((uint32_t)mdmbrxclkforce << 3) | ((uint32_t)mdmbclkforce << 2) | ((uint32_t)ldpcencclkforce << 1) | ((uint32_t)ldpcdecclkforce << 0));
}

/**
 * @brief Unpacks CLKGATEPHYFCTRL1's fields from current value of the CLKGATEPHYFCTRL1 register.
 *
 * Reads the CLKGATEPHYFCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] laclkforce - Will be populated with the current value of this field from the register.
 * @param[out] radartimclkforce - Will be populated with the current value of this field from the register.
 * @param[out] mdmbtxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] mdmbrxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] mdmbclkforce - Will be populated with the current value of this field from the register.
 * @param[out] ldpcencclkforce - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecclkforce - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_clkgatephyfctrl1_unpack(uint8_t* laclkforce, uint8_t* radartimclkforce, uint8_t* mdmbtxclkforce, uint8_t* mdmbrxclkforce, uint8_t* mdmbclkforce, uint8_t* ldpcencclkforce, uint8_t* ldpcdecclkforce)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);

    *laclkforce = (localVal & ((uint32_t)0x00000100)) >> 8;
    *radartimclkforce = (localVal & ((uint32_t)0x00000020)) >> 5;
    *mdmbtxclkforce = (localVal & ((uint32_t)0x00000010)) >> 4;
    *mdmbrxclkforce = (localVal & ((uint32_t)0x00000008)) >> 3;
    *mdmbclkforce = (localVal & ((uint32_t)0x00000004)) >> 2;
    *ldpcencclkforce = (localVal & ((uint32_t)0x00000002)) >> 1;
    *ldpcdecclkforce = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the LACLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the LACLKFORCE field's value will be returned.
 *
 * @return The current value of the LACLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_laclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the LACLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] laclkforce - The value to set the field to.
 */
__INLINE void crm_laclkforce_setf(uint8_t laclkforce)
{
    ASSERT_ERR((((uint32_t)laclkforce << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)laclkforce << 8));
}

/**
 * @brief Returns the current value of the RADARTIMCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the RADARTIMCLKFORCE field's value will be returned.
 *
 * @return The current value of the RADARTIMCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_radartimclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the RADARTIMCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] radartimclkforce - The value to set the field to.
 */
__INLINE void crm_radartimclkforce_setf(uint8_t radartimclkforce)
{
    ASSERT_ERR((((uint32_t)radartimclkforce << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)radartimclkforce << 5));
}

/**
 * @brief Returns the current value of the MDMBTXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the MDMBTXCLKFORCE field's value will be returned.
 *
 * @return The current value of the MDMBTXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_mdmbtxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the MDMBTXCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbtxclkforce - The value to set the field to.
 */
__INLINE void crm_mdmbtxclkforce_setf(uint8_t mdmbtxclkforce)
{
    ASSERT_ERR((((uint32_t)mdmbtxclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)mdmbtxclkforce << 4));
}

/**
 * @brief Returns the current value of the MDMBRXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the MDMBRXCLKFORCE field's value will be returned.
 *
 * @return The current value of the MDMBRXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_mdmbrxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the MDMBRXCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbrxclkforce - The value to set the field to.
 */
__INLINE void crm_mdmbrxclkforce_setf(uint8_t mdmbrxclkforce)
{
    ASSERT_ERR((((uint32_t)mdmbrxclkforce << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)mdmbrxclkforce << 3));
}

/**
 * @brief Returns the current value of the MDMBCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the MDMBCLKFORCE field's value will be returned.
 *
 * @return The current value of the MDMBCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_mdmbclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the MDMBCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbclkforce - The value to set the field to.
 */
__INLINE void crm_mdmbclkforce_setf(uint8_t mdmbclkforce)
{
    ASSERT_ERR((((uint32_t)mdmbclkforce << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)mdmbclkforce << 2));
}

/**
 * @brief Returns the current value of the LDPCENCCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the LDPCENCCLKFORCE field's value will be returned.
 *
 * @return The current value of the LDPCENCCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_ldpcencclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the LDPCENCCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcencclkforce - The value to set the field to.
 */
__INLINE void crm_ldpcencclkforce_setf(uint8_t ldpcencclkforce)
{
    ASSERT_ERR((((uint32_t)ldpcencclkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)ldpcencclkforce << 1));
}

/**
 * @brief Returns the current value of the LDPCDECCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the LDPCDECCLKFORCE field's value will be returned.
 *
 * @return The current value of the LDPCDECCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_ldpcdecclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the LDPCDECCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecclkforce - The value to set the field to.
 */
__INLINE void crm_ldpcdecclkforce_setf(uint8_t ldpcdecclkforce)
{
    ASSERT_ERR((((uint32_t)ldpcdecclkforce << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)ldpcdecclkforce << 0));
}

/// @}

/**
 * @name RSTCTRL register definitions
 * <table>
 * <caption>RSTCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 08 <td>  RCSWRESET <td> R <td> R/W <td> 0
 * <tr><td> 04 <td> AGCSWRESET <td> R <td> R/W <td> 0
 * <tr><td> 00 <td> PHYSWRESET <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the RSTCTRL register
#define CRM_RSTCTRL_ADDR   0x24940018
/// Offset of the RSTCTRL register from the base address
#define CRM_RSTCTRL_OFFSET 0x00000018
/// Index of the RSTCTRL register
#define CRM_RSTCTRL_INDEX  0x00000006
/// Reset value of the RSTCTRL register
#define CRM_RSTCTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the RSTCTRL register.
 * The RSTCTRL register will be read and its value returned.
 * @return The current value of the RSTCTRL register.
 */
__INLINE uint32_t crm_rstctrl_get(void)
{
    return REG_PL_RD(CRM_RSTCTRL_ADDR);
}

/**
 * @brief Sets the RSTCTRL register to a value.
 * The RSTCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_rstctrl_set(uint32_t value)
{
    REG_PL_WR(CRM_RSTCTRL_ADDR, value);
}

// field definitions
/// RCSWRESET field bit
#define CRM_RCSWRESET_BIT     ((uint32_t)0x00000100)
/// RCSWRESET field position
#define CRM_RCSWRESET_POS     8
/// AGCSWRESET field bit
#define CRM_AGCSWRESET_BIT    ((uint32_t)0x00000010)
/// AGCSWRESET field position
#define CRM_AGCSWRESET_POS    4
/// PHYSWRESET field bit
#define CRM_PHYSWRESET_BIT    ((uint32_t)0x00000001)
/// PHYSWRESET field position
#define CRM_PHYSWRESET_POS    0

/// RCSWRESET field reset value
#define CRM_RCSWRESET_RST     0x0
/// AGCSWRESET field reset value
#define CRM_AGCSWRESET_RST    0x0
/// PHYSWRESET field reset value
#define CRM_PHYSWRESET_RST    0x0

/**
 * @brief Constructs a value for the RSTCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rcswreset - The value to use for the RCSWRESET field.
 * @param[in] agcswreset - The value to use for the AGCSWRESET field.
 * @param[in] physwreset - The value to use for the PHYSWRESET field.
 */
__INLINE void crm_rstctrl_pack(uint8_t rcswreset, uint8_t agcswreset, uint8_t physwreset)
{
    ASSERT_ERR((((uint32_t)rcswreset << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)agcswreset << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)physwreset << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_RSTCTRL_ADDR,  ((uint32_t)rcswreset << 8) | ((uint32_t)agcswreset << 4) | ((uint32_t)physwreset << 0));
}

/**
 * @brief Unpacks RSTCTRL's fields from current value of the RSTCTRL register.
 *
 * Reads the RSTCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rcswreset - Will be populated with the current value of this field from the register.
 * @param[out] agcswreset - Will be populated with the current value of this field from the register.
 * @param[out] physwreset - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_rstctrl_unpack(uint8_t* rcswreset, uint8_t* agcswreset, uint8_t* physwreset)
{
    uint32_t localVal = REG_PL_RD(CRM_RSTCTRL_ADDR);

    *rcswreset = (localVal & ((uint32_t)0x00000100)) >> 8;
    *agcswreset = (localVal & ((uint32_t)0x00000010)) >> 4;
    *physwreset = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RCSWRESET field in the RSTCTRL register.
 *
 * The RSTCTRL register will be read and the RCSWRESET field's value will be returned.
 *
 * @return The current value of the RCSWRESET field in the RSTCTRL register.
 */
__INLINE uint8_t crm_rcswreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_RSTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the RCSWRESET field of the RSTCTRL register.
 *
 * The RSTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rcswreset - The value to set the field to.
 */
__INLINE void crm_rcswreset_setf(uint8_t rcswreset)
{
    ASSERT_ERR((((uint32_t)rcswreset << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(CRM_RSTCTRL_ADDR, (REG_PL_RD(CRM_RSTCTRL_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)rcswreset << 8));
}

/**
 * @brief Returns the current value of the AGCSWRESET field in the RSTCTRL register.
 *
 * The RSTCTRL register will be read and the AGCSWRESET field's value will be returned.
 *
 * @return The current value of the AGCSWRESET field in the RSTCTRL register.
 */
__INLINE uint8_t crm_agcswreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_RSTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the AGCSWRESET field of the RSTCTRL register.
 *
 * The RSTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcswreset - The value to set the field to.
 */
__INLINE void crm_agcswreset_setf(uint8_t agcswreset)
{
    ASSERT_ERR((((uint32_t)agcswreset << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(CRM_RSTCTRL_ADDR, (REG_PL_RD(CRM_RSTCTRL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)agcswreset << 4));
}

/**
 * @brief Returns the current value of the PHYSWRESET field in the RSTCTRL register.
 *
 * The RSTCTRL register will be read and the PHYSWRESET field's value will be returned.
 *
 * @return The current value of the PHYSWRESET field in the RSTCTRL register.
 */
__INLINE uint8_t crm_physwreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_RSTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the PHYSWRESET field of the RSTCTRL register.
 *
 * The RSTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] physwreset - The value to set the field to.
 */
__INLINE void crm_physwreset_setf(uint8_t physwreset)
{
    ASSERT_ERR((((uint32_t)physwreset << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_RSTCTRL_ADDR, (REG_PL_RD(CRM_RSTCTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)physwreset << 0));
}

/// @}

/**
 * @name LA_SAMPLING_FREQ register definitions
 * <table>
 * <caption>LA_SAMPLING_FREQ bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> LA_SAMPLING_FREQ <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the LA_SAMPLING_FREQ register
#define CRM_LA_SAMPLING_FREQ_ADDR   0x2494006C
/// Offset of the LA_SAMPLING_FREQ register from the base address
#define CRM_LA_SAMPLING_FREQ_OFFSET 0x0000006C
/// Index of the LA_SAMPLING_FREQ register
#define CRM_LA_SAMPLING_FREQ_INDEX  0x0000001B
/// Reset value of the LA_SAMPLING_FREQ register
#define CRM_LA_SAMPLING_FREQ_RESET  0x00000000

/**
 * @brief Returns the current value of the LA_SAMPLING_FREQ register.
 * The LA_SAMPLING_FREQ register will be read and its value returned.
 * @return The current value of the LA_SAMPLING_FREQ register.
 */
__INLINE uint32_t crm_la_sampling_freq_get(void)
{
    return REG_PL_RD(CRM_LA_SAMPLING_FREQ_ADDR);
}

/**
 * @brief Sets the LA_SAMPLING_FREQ register to a value.
 * The LA_SAMPLING_FREQ register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_la_sampling_freq_set(uint32_t value)
{
    REG_PL_WR(CRM_LA_SAMPLING_FREQ_ADDR, value);
}

// field definitions
/// LA_SAMPLING_FREQ field mask
#define CRM_LA_SAMPLING_FREQ_MASK   ((uint32_t)0x000000FF)
/// LA_SAMPLING_FREQ field LSB position
#define CRM_LA_SAMPLING_FREQ_LSB    0
/// LA_SAMPLING_FREQ field width
#define CRM_LA_SAMPLING_FREQ_WIDTH  ((uint32_t)0x00000008)

/// LA_SAMPLING_FREQ field reset value
#define CRM_LA_SAMPLING_FREQ_RST    0x0

/**
 * @brief Returns the current value of the LA_SAMPLING_FREQ field in the LA_SAMPLING_FREQ register.
 *
 * The LA_SAMPLING_FREQ register will be read and the LA_SAMPLING_FREQ field's value will be returned.
 *
 * @return The current value of the LA_SAMPLING_FREQ field in the LA_SAMPLING_FREQ register.
 */
__INLINE uint8_t crm_la_sampling_freq_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_LA_SAMPLING_FREQ_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the LA_SAMPLING_FREQ field of the LA_SAMPLING_FREQ register.
 *
 * The LA_SAMPLING_FREQ register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lasamplingfreq - The value to set the field to.
 */
__INLINE void crm_la_sampling_freq_setf(uint8_t lasamplingfreq)
{
    ASSERT_ERR((((uint32_t)lasamplingfreq << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(CRM_LA_SAMPLING_FREQ_ADDR, (uint32_t)lasamplingfreq << 0);
}

/// @}

/**
 * @name DYNCFGMMC_CNTL register definitions
 * <table>
 * <caption>DYNCFGMMC_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31 <td>  DYNCFGMMC_DONE <td> W <td> R <td> 0
 * <tr><td> 30 <td>     FMETRE_DONE <td> W <td> R <td> 0
 * <tr><td> 01 <td>    FMETRE_START <td> R <td> R/W <td> 0
 * <tr><td> 00 <td> DYNCFGMMC_START <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the DYNCFGMMC_CNTL register
#define CRM_DYNCFGMMC_CNTL_ADDR   0x24940300
/// Offset of the DYNCFGMMC_CNTL register from the base address
#define CRM_DYNCFGMMC_CNTL_OFFSET 0x00000300
/// Index of the DYNCFGMMC_CNTL register
#define CRM_DYNCFGMMC_CNTL_INDEX  0x000000C0
/// Reset value of the DYNCFGMMC_CNTL register
#define CRM_DYNCFGMMC_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the DYNCFGMMC_CNTL register.
 * The DYNCFGMMC_CNTL register will be read and its value returned.
 * @return The current value of the DYNCFGMMC_CNTL register.
 */
__INLINE uint32_t crm_dyncfgmmc_cntl_get(void)
{
    return REG_PL_RD(CRM_DYNCFGMMC_CNTL_ADDR);
}

/**
 * @brief Sets the DYNCFGMMC_CNTL register to a value.
 * The DYNCFGMMC_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_dyncfgmmc_cntl_set(uint32_t value)
{
    REG_PL_WR(CRM_DYNCFGMMC_CNTL_ADDR, value);
}

// field definitions
/// DYNCFGMMC_DONE field bit
#define CRM_DYNCFGMMC_DONE_BIT     ((uint32_t)0x80000000)
/// DYNCFGMMC_DONE field position
#define CRM_DYNCFGMMC_DONE_POS     31
/// FMETRE_DONE field bit
#define CRM_FMETRE_DONE_BIT        ((uint32_t)0x40000000)
/// FMETRE_DONE field position
#define CRM_FMETRE_DONE_POS        30
/// FMETRE_START field bit
#define CRM_FMETRE_START_BIT       ((uint32_t)0x00000002)
/// FMETRE_START field position
#define CRM_FMETRE_START_POS       1
/// DYNCFGMMC_START field bit
#define CRM_DYNCFGMMC_START_BIT    ((uint32_t)0x00000001)
/// DYNCFGMMC_START field position
#define CRM_DYNCFGMMC_START_POS    0

/// DYNCFGMMC_DONE field reset value
#define CRM_DYNCFGMMC_DONE_RST     0x0
/// FMETRE_DONE field reset value
#define CRM_FMETRE_DONE_RST        0x0
/// FMETRE_START field reset value
#define CRM_FMETRE_START_RST       0x0
/// DYNCFGMMC_START field reset value
#define CRM_DYNCFGMMC_START_RST    0x0

/**
 * @brief Constructs a value for the DYNCFGMMC_CNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fmetrestart - The value to use for the FMETRE_START field.
 * @param[in] dyncfgmmcstart - The value to use for the DYNCFGMMC_START field.
 */
__INLINE void crm_dyncfgmmc_cntl_pack(uint8_t fmetrestart, uint8_t dyncfgmmcstart)
{
    ASSERT_ERR((((uint32_t)fmetrestart << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)dyncfgmmcstart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_DYNCFGMMC_CNTL_ADDR,  ((uint32_t)fmetrestart << 1) | ((uint32_t)dyncfgmmcstart << 0));
}

/**
 * @brief Unpacks DYNCFGMMC_CNTL's fields from current value of the DYNCFGMMC_CNTL register.
 *
 * Reads the DYNCFGMMC_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dyncfgmmcdone - Will be populated with the current value of this field from the register.
 * @param[out] fmetredone - Will be populated with the current value of this field from the register.
 * @param[out] fmetrestart - Will be populated with the current value of this field from the register.
 * @param[out] dyncfgmmcstart - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_dyncfgmmc_cntl_unpack(uint8_t* dyncfgmmcdone, uint8_t* fmetredone, uint8_t* fmetrestart, uint8_t* dyncfgmmcstart)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_CNTL_ADDR);

    *dyncfgmmcdone = (localVal & ((uint32_t)0x80000000)) >> 31;
    *fmetredone = (localVal & ((uint32_t)0x40000000)) >> 30;
    *fmetrestart = (localVal & ((uint32_t)0x00000002)) >> 1;
    *dyncfgmmcstart = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the DYNCFGMMC_DONE field in the DYNCFGMMC_CNTL register.
 *
 * The DYNCFGMMC_CNTL register will be read and the DYNCFGMMC_DONE field's value will be returned.
 *
 * @return The current value of the DYNCFGMMC_DONE field in the DYNCFGMMC_CNTL register.
 */
__INLINE uint8_t crm_dyncfgmmc_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the FMETRE_DONE field in the DYNCFGMMC_CNTL register.
 *
 * The DYNCFGMMC_CNTL register will be read and the FMETRE_DONE field's value will be returned.
 *
 * @return The current value of the FMETRE_DONE field in the DYNCFGMMC_CNTL register.
 */
__INLINE uint8_t crm_fmetre_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Returns the current value of the FMETRE_START field in the DYNCFGMMC_CNTL register.
 *
 * The DYNCFGMMC_CNTL register will be read and the FMETRE_START field's value will be returned.
 *
 * @return The current value of the FMETRE_START field in the DYNCFGMMC_CNTL register.
 */
__INLINE uint8_t crm_fmetre_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the FMETRE_START field of the DYNCFGMMC_CNTL register.
 *
 * The DYNCFGMMC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fmetrestart - The value to set the field to.
 */
__INLINE void crm_fmetre_start_setf(uint8_t fmetrestart)
{
    ASSERT_ERR((((uint32_t)fmetrestart << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(CRM_DYNCFGMMC_CNTL_ADDR, (REG_PL_RD(CRM_DYNCFGMMC_CNTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)fmetrestart << 1));
}

/**
 * @brief Returns the current value of the DYNCFGMMC_START field in the DYNCFGMMC_CNTL register.
 *
 * The DYNCFGMMC_CNTL register will be read and the DYNCFGMMC_START field's value will be returned.
 *
 * @return The current value of the DYNCFGMMC_START field in the DYNCFGMMC_CNTL register.
 */
__INLINE uint8_t crm_dyncfgmmc_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the DYNCFGMMC_START field of the DYNCFGMMC_CNTL register.
 *
 * The DYNCFGMMC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dyncfgmmcstart - The value to set the field to.
 */
__INLINE void crm_dyncfgmmc_start_setf(uint8_t dyncfgmmcstart)
{
    ASSERT_ERR((((uint32_t)dyncfgmmcstart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_DYNCFGMMC_CNTL_ADDR, (REG_PL_RD(CRM_DYNCFGMMC_CNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)dyncfgmmcstart << 0));
}

/// @}

/**
 * @name DYNCFGMMC_ADDR register definitions
 * <table>
 * <caption>DYNCFGMMC_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>   DYNCFGMMC_WE <td> R <td> R/W <td> 0
 * <tr><td> 19:16 <td>  DYNCFGMMC_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 15:00 <td> DYNCFGMMC_ADDR <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DYNCFGMMC_ADDR register
#define CRM_DYNCFGMMC_ADDR_ADDR   0x24940304
/// Offset of the DYNCFGMMC_ADDR register from the base address
#define CRM_DYNCFGMMC_ADDR_OFFSET 0x00000304
/// Index of the DYNCFGMMC_ADDR register
#define CRM_DYNCFGMMC_ADDR_INDEX  0x000000C1
/// Reset value of the DYNCFGMMC_ADDR register
#define CRM_DYNCFGMMC_ADDR_RESET  0x00000000

/**
 * @brief Returns the current value of the DYNCFGMMC_ADDR register.
 * The DYNCFGMMC_ADDR register will be read and its value returned.
 * @return The current value of the DYNCFGMMC_ADDR register.
 */
__INLINE uint32_t crm_dyncfgmmc_addr_get(void)
{
    return REG_PL_RD(CRM_DYNCFGMMC_ADDR_ADDR);
}

/**
 * @brief Sets the DYNCFGMMC_ADDR register to a value.
 * The DYNCFGMMC_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_dyncfgmmc_addr_set(uint32_t value)
{
    REG_PL_WR(CRM_DYNCFGMMC_ADDR_ADDR, value);
}

// field definitions
/// DYNCFGMMC_WE field bit
#define CRM_DYNCFGMMC_WE_BIT      ((uint32_t)0x80000000)
/// DYNCFGMMC_WE field position
#define CRM_DYNCFGMMC_WE_POS      31
/// DYNCFGMMC_SEL field mask
#define CRM_DYNCFGMMC_SEL_MASK    ((uint32_t)0x000F0000)
/// DYNCFGMMC_SEL field LSB position
#define CRM_DYNCFGMMC_SEL_LSB     16
/// DYNCFGMMC_SEL field width
#define CRM_DYNCFGMMC_SEL_WIDTH   ((uint32_t)0x00000004)
/// DYNCFGMMC_ADDR field mask
#define CRM_DYNCFGMMC_ADDR_MASK   ((uint32_t)0x0000FFFF)
/// DYNCFGMMC_ADDR field LSB position
#define CRM_DYNCFGMMC_ADDR_LSB    0
/// DYNCFGMMC_ADDR field width
#define CRM_DYNCFGMMC_ADDR_WIDTH  ((uint32_t)0x00000010)

/// DYNCFGMMC_WE field reset value
#define CRM_DYNCFGMMC_WE_RST      0x0
/// DYNCFGMMC_SEL field reset value
#define CRM_DYNCFGMMC_SEL_RST     0x0
/// DYNCFGMMC_ADDR field reset value
#define CRM_DYNCFGMMC_ADDR_RST    0x0

/**
 * @brief Constructs a value for the DYNCFGMMC_ADDR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dyncfgmmcwe - The value to use for the DYNCFGMMC_WE field.
 * @param[in] dyncfgmmcsel - The value to use for the DYNCFGMMC_SEL field.
 * @param[in] dyncfgmmcaddr - The value to use for the DYNCFGMMC_ADDR field.
 */
__INLINE void crm_dyncfgmmc_addr_pack(uint8_t dyncfgmmcwe, uint8_t dyncfgmmcsel, uint16_t dyncfgmmcaddr)
{
    ASSERT_ERR((((uint32_t)dyncfgmmcwe << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)dyncfgmmcsel << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)dyncfgmmcaddr << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(CRM_DYNCFGMMC_ADDR_ADDR,  ((uint32_t)dyncfgmmcwe << 31) | ((uint32_t)dyncfgmmcsel << 16) | ((uint32_t)dyncfgmmcaddr << 0));
}

/**
 * @brief Unpacks DYNCFGMMC_ADDR's fields from current value of the DYNCFGMMC_ADDR register.
 *
 * Reads the DYNCFGMMC_ADDR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dyncfgmmcwe - Will be populated with the current value of this field from the register.
 * @param[out] dyncfgmmcsel - Will be populated with the current value of this field from the register.
 * @param[out] dyncfgmmcaddr - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_dyncfgmmc_addr_unpack(uint8_t* dyncfgmmcwe, uint8_t* dyncfgmmcsel, uint16_t* dyncfgmmcaddr)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_ADDR_ADDR);

    *dyncfgmmcwe = (localVal & ((uint32_t)0x80000000)) >> 31;
    *dyncfgmmcsel = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *dyncfgmmcaddr = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the DYNCFGMMC_WE field in the DYNCFGMMC_ADDR register.
 *
 * The DYNCFGMMC_ADDR register will be read and the DYNCFGMMC_WE field's value will be returned.
 *
 * @return The current value of the DYNCFGMMC_WE field in the DYNCFGMMC_ADDR register.
 */
__INLINE uint8_t crm_dyncfgmmc_we_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the DYNCFGMMC_WE field of the DYNCFGMMC_ADDR register.
 *
 * The DYNCFGMMC_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dyncfgmmcwe - The value to set the field to.
 */
__INLINE void crm_dyncfgmmc_we_setf(uint8_t dyncfgmmcwe)
{
    ASSERT_ERR((((uint32_t)dyncfgmmcwe << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(CRM_DYNCFGMMC_ADDR_ADDR, (REG_PL_RD(CRM_DYNCFGMMC_ADDR_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)dyncfgmmcwe << 31));
}

/**
 * @brief Returns the current value of the DYNCFGMMC_SEL field in the DYNCFGMMC_ADDR register.
 *
 * The DYNCFGMMC_ADDR register will be read and the DYNCFGMMC_SEL field's value will be returned.
 *
 * @return The current value of the DYNCFGMMC_SEL field in the DYNCFGMMC_ADDR register.
 */
__INLINE uint8_t crm_dyncfgmmc_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the DYNCFGMMC_SEL field of the DYNCFGMMC_ADDR register.
 *
 * The DYNCFGMMC_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dyncfgmmcsel - The value to set the field to.
 */
__INLINE void crm_dyncfgmmc_sel_setf(uint8_t dyncfgmmcsel)
{
    ASSERT_ERR((((uint32_t)dyncfgmmcsel << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(CRM_DYNCFGMMC_ADDR_ADDR, (REG_PL_RD(CRM_DYNCFGMMC_ADDR_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)dyncfgmmcsel << 16));
}

/**
 * @brief Returns the current value of the DYNCFGMMC_ADDR field in the DYNCFGMMC_ADDR register.
 *
 * The DYNCFGMMC_ADDR register will be read and the DYNCFGMMC_ADDR field's value will be returned.
 *
 * @return The current value of the DYNCFGMMC_ADDR field in the DYNCFGMMC_ADDR register.
 */
__INLINE uint16_t crm_dyncfgmmc_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the DYNCFGMMC_ADDR field of the DYNCFGMMC_ADDR register.
 *
 * The DYNCFGMMC_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dyncfgmmcaddr - The value to set the field to.
 */
__INLINE void crm_dyncfgmmc_addr_setf(uint16_t dyncfgmmcaddr)
{
    ASSERT_ERR((((uint32_t)dyncfgmmcaddr << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(CRM_DYNCFGMMC_ADDR_ADDR, (REG_PL_RD(CRM_DYNCFGMMC_ADDR_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)dyncfgmmcaddr << 0));
}

/// @}

/**
 * @name DYNCFGMMC_DATA register definitions
 * <table>
 * <caption>DYNCFGMMC_DATA bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td> DYNCFGMMC_RDATA <td> R/W <td> R <td> 0x0
 * <tr><td> 15:00 <td> DYNCFGMMC_WDATA <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DYNCFGMMC_DATA register
#define CRM_DYNCFGMMC_DATA_ADDR   0x24940308
/// Offset of the DYNCFGMMC_DATA register from the base address
#define CRM_DYNCFGMMC_DATA_OFFSET 0x00000308
/// Index of the DYNCFGMMC_DATA register
#define CRM_DYNCFGMMC_DATA_INDEX  0x000000C2
/// Reset value of the DYNCFGMMC_DATA register
#define CRM_DYNCFGMMC_DATA_RESET  0x00000000

/**
 * @brief Returns the current value of the DYNCFGMMC_DATA register.
 * The DYNCFGMMC_DATA register will be read and its value returned.
 * @return The current value of the DYNCFGMMC_DATA register.
 */
__INLINE uint32_t crm_dyncfgmmc_data_get(void)
{
    return REG_PL_RD(CRM_DYNCFGMMC_DATA_ADDR);
}

/**
 * @brief Sets the DYNCFGMMC_DATA register to a value.
 * The DYNCFGMMC_DATA register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_dyncfgmmc_data_set(uint32_t value)
{
    REG_PL_WR(CRM_DYNCFGMMC_DATA_ADDR, value);
}

// field definitions
/// DYNCFGMMC_RDATA field mask
#define CRM_DYNCFGMMC_RDATA_MASK   ((uint32_t)0xFFFF0000)
/// DYNCFGMMC_RDATA field LSB position
#define CRM_DYNCFGMMC_RDATA_LSB    16
/// DYNCFGMMC_RDATA field width
#define CRM_DYNCFGMMC_RDATA_WIDTH  ((uint32_t)0x00000010)
/// DYNCFGMMC_WDATA field mask
#define CRM_DYNCFGMMC_WDATA_MASK   ((uint32_t)0x0000FFFF)
/// DYNCFGMMC_WDATA field LSB position
#define CRM_DYNCFGMMC_WDATA_LSB    0
/// DYNCFGMMC_WDATA field width
#define CRM_DYNCFGMMC_WDATA_WIDTH  ((uint32_t)0x00000010)

/// DYNCFGMMC_RDATA field reset value
#define CRM_DYNCFGMMC_RDATA_RST    0x0
/// DYNCFGMMC_WDATA field reset value
#define CRM_DYNCFGMMC_WDATA_RST    0x0

/**
 * @brief Unpacks DYNCFGMMC_DATA's fields from current value of the DYNCFGMMC_DATA register.
 *
 * Reads the DYNCFGMMC_DATA register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dyncfgmmcrdata - Will be populated with the current value of this field from the register.
 * @param[out] dyncfgmmcwdata - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_dyncfgmmc_data_unpack(uint16_t* dyncfgmmcrdata, uint16_t* dyncfgmmcwdata)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_DATA_ADDR);

    *dyncfgmmcrdata = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *dyncfgmmcwdata = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the DYNCFGMMC_RDATA field in the DYNCFGMMC_DATA register.
 *
 * The DYNCFGMMC_DATA register will be read and the DYNCFGMMC_RDATA field's value will be returned.
 *
 * @return The current value of the DYNCFGMMC_RDATA field in the DYNCFGMMC_DATA register.
 */
__INLINE uint16_t crm_dyncfgmmc_rdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_DATA_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the DYNCFGMMC_WDATA field in the DYNCFGMMC_DATA register.
 *
 * The DYNCFGMMC_DATA register will be read and the DYNCFGMMC_WDATA field's value will be returned.
 *
 * @return The current value of the DYNCFGMMC_WDATA field in the DYNCFGMMC_DATA register.
 */
__INLINE uint16_t crm_dyncfgmmc_wdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_DYNCFGMMC_DATA_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the DYNCFGMMC_WDATA field of the DYNCFGMMC_DATA register.
 *
 * The DYNCFGMMC_DATA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dyncfgmmcwdata - The value to set the field to.
 */
__INLINE void crm_dyncfgmmc_wdata_setf(uint16_t dyncfgmmcwdata)
{
    ASSERT_ERR((((uint32_t)dyncfgmmcwdata << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(CRM_DYNCFGMMC_DATA_ADDR, (uint32_t)dyncfgmmcwdata << 0);
}

/// @}

/**
 * @name FMETRE_REF44 register definitions
 * <table>
 * <caption>FMETRE_REF44 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_REF44 <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_REF44 register
#define CRM_FMETRE_REF44_ADDR   0x24940310
/// Offset of the FMETRE_REF44 register from the base address
#define CRM_FMETRE_REF44_OFFSET 0x00000310
/// Index of the FMETRE_REF44 register
#define CRM_FMETRE_REF44_INDEX  0x000000C4
/// Reset value of the FMETRE_REF44 register
#define CRM_FMETRE_REF44_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_REF44 register.
 * The FMETRE_REF44 register will be read and its value returned.
 * @return The current value of the FMETRE_REF44 register.
 */
__INLINE uint32_t crm_fmetre_ref44_get(void)
{
    return REG_PL_RD(CRM_FMETRE_REF44_ADDR);
}

// field definitions
/// FMETRE_REF44 field mask
#define CRM_FMETRE_REF44_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_REF44 field LSB position
#define CRM_FMETRE_REF44_LSB    0
/// FMETRE_REF44 field width
#define CRM_FMETRE_REF44_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_REF44 field reset value
#define CRM_FMETRE_REF44_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_REF44 field in the FMETRE_REF44 register.
 *
 * The FMETRE_REF44 register will be read and the FMETRE_REF44 field's value will be returned.
 *
 * @return The current value of the FMETRE_REF44 field in the FMETRE_REF44 register.
 */
__INLINE uint32_t crm_fmetre_ref44_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_REF44_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_LDPCRX register definitions
 * <table>
 * <caption>FMETRE_LDPCRX bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_LDPCRX <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_LDPCRX register
#define CRM_FMETRE_LDPCRX_ADDR   0x24940314
/// Offset of the FMETRE_LDPCRX register from the base address
#define CRM_FMETRE_LDPCRX_OFFSET 0x00000314
/// Index of the FMETRE_LDPCRX register
#define CRM_FMETRE_LDPCRX_INDEX  0x000000C5
/// Reset value of the FMETRE_LDPCRX register
#define CRM_FMETRE_LDPCRX_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_LDPCRX register.
 * The FMETRE_LDPCRX register will be read and its value returned.
 * @return The current value of the FMETRE_LDPCRX register.
 */
__INLINE uint32_t crm_fmetre_ldpcrx_get(void)
{
    return REG_PL_RD(CRM_FMETRE_LDPCRX_ADDR);
}

// field definitions
/// FMETRE_LDPCRX field mask
#define CRM_FMETRE_LDPCRX_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_LDPCRX field LSB position
#define CRM_FMETRE_LDPCRX_LSB    0
/// FMETRE_LDPCRX field width
#define CRM_FMETRE_LDPCRX_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_LDPCRX field reset value
#define CRM_FMETRE_LDPCRX_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_LDPCRX field in the FMETRE_LDPCRX register.
 *
 * The FMETRE_LDPCRX register will be read and the FMETRE_LDPCRX field's value will be returned.
 *
 * @return The current value of the FMETRE_LDPCRX field in the FMETRE_LDPCRX register.
 */
__INLINE uint32_t crm_fmetre_ldpcrx_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_LDPCRX_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_REF40 register definitions
 * <table>
 * <caption>FMETRE_REF40 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_REF40 <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_REF40 register
#define CRM_FMETRE_REF40_ADDR   0x2494031C
/// Offset of the FMETRE_REF40 register from the base address
#define CRM_FMETRE_REF40_OFFSET 0x0000031C
/// Index of the FMETRE_REF40 register
#define CRM_FMETRE_REF40_INDEX  0x000000C7
/// Reset value of the FMETRE_REF40 register
#define CRM_FMETRE_REF40_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_REF40 register.
 * The FMETRE_REF40 register will be read and its value returned.
 * @return The current value of the FMETRE_REF40 register.
 */
__INLINE uint32_t crm_fmetre_ref40_get(void)
{
    return REG_PL_RD(CRM_FMETRE_REF40_ADDR);
}

// field definitions
/// FMETRE_REF40 field mask
#define CRM_FMETRE_REF40_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_REF40 field LSB position
#define CRM_FMETRE_REF40_LSB    0
/// FMETRE_REF40 field width
#define CRM_FMETRE_REF40_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_REF40 field reset value
#define CRM_FMETRE_REF40_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_REF40 field in the FMETRE_REF40 register.
 *
 * The FMETRE_REF40 register will be read and the FMETRE_REF40 field's value will be returned.
 *
 * @return The current value of the FMETRE_REF40 field in the FMETRE_REF40 register.
 */
__INLINE uint32_t crm_fmetre_ref40_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_REF40_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_REF80 register definitions
 * <table>
 * <caption>FMETRE_REF80 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_REF80 <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_REF80 register
#define CRM_FMETRE_REF80_ADDR   0x24940320
/// Offset of the FMETRE_REF80 register from the base address
#define CRM_FMETRE_REF80_OFFSET 0x00000320
/// Index of the FMETRE_REF80 register
#define CRM_FMETRE_REF80_INDEX  0x000000C8
/// Reset value of the FMETRE_REF80 register
#define CRM_FMETRE_REF80_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_REF80 register.
 * The FMETRE_REF80 register will be read and its value returned.
 * @return The current value of the FMETRE_REF80 register.
 */
__INLINE uint32_t crm_fmetre_ref80_get(void)
{
    return REG_PL_RD(CRM_FMETRE_REF80_ADDR);
}

// field definitions
/// FMETRE_REF80 field mask
#define CRM_FMETRE_REF80_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_REF80 field LSB position
#define CRM_FMETRE_REF80_LSB    0
/// FMETRE_REF80 field width
#define CRM_FMETRE_REF80_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_REF80 field reset value
#define CRM_FMETRE_REF80_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_REF80 field in the FMETRE_REF80 register.
 *
 * The FMETRE_REF80 register will be read and the FMETRE_REF80 field's value will be returned.
 *
 * @return The current value of the FMETRE_REF80 field in the FMETRE_REF80 register.
 */
__INLINE uint32_t crm_fmetre_ref80_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_REF80_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_FE register definitions
 * <table>
 * <caption>FMETRE_FE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_FE <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_FE register
#define CRM_FMETRE_FE_ADDR   0x24940324
/// Offset of the FMETRE_FE register from the base address
#define CRM_FMETRE_FE_OFFSET 0x00000324
/// Index of the FMETRE_FE register
#define CRM_FMETRE_FE_INDEX  0x000000C9
/// Reset value of the FMETRE_FE register
#define CRM_FMETRE_FE_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_FE register.
 * The FMETRE_FE register will be read and its value returned.
 * @return The current value of the FMETRE_FE register.
 */
__INLINE uint32_t crm_fmetre_fe_get(void)
{
    return REG_PL_RD(CRM_FMETRE_FE_ADDR);
}

// field definitions
/// FMETRE_FE field mask
#define CRM_FMETRE_FE_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_FE field LSB position
#define CRM_FMETRE_FE_LSB    0
/// FMETRE_FE field width
#define CRM_FMETRE_FE_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_FE field reset value
#define CRM_FMETRE_FE_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_FE field in the FMETRE_FE register.
 *
 * The FMETRE_FE register will be read and the FMETRE_FE field's value will be returned.
 *
 * @return The current value of the FMETRE_FE field in the FMETRE_FE register.
 */
__INLINE uint32_t crm_fmetre_fe_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_FE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_VTB register definitions
 * <table>
 * <caption>FMETRE_VTB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_VTB <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_VTB register
#define CRM_FMETRE_VTB_ADDR   0x24940328
/// Offset of the FMETRE_VTB register from the base address
#define CRM_FMETRE_VTB_OFFSET 0x00000328
/// Index of the FMETRE_VTB register
#define CRM_FMETRE_VTB_INDEX  0x000000CA
/// Reset value of the FMETRE_VTB register
#define CRM_FMETRE_VTB_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_VTB register.
 * The FMETRE_VTB register will be read and its value returned.
 * @return The current value of the FMETRE_VTB register.
 */
__INLINE uint32_t crm_fmetre_vtb_get(void)
{
    return REG_PL_RD(CRM_FMETRE_VTB_ADDR);
}

// field definitions
/// FMETRE_VTB field mask
#define CRM_FMETRE_VTB_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_VTB field LSB position
#define CRM_FMETRE_VTB_LSB    0
/// FMETRE_VTB field width
#define CRM_FMETRE_VTB_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_VTB field reset value
#define CRM_FMETRE_VTB_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_VTB field in the FMETRE_VTB register.
 *
 * The FMETRE_VTB register will be read and the FMETRE_VTB field's value will be returned.
 *
 * @return The current value of the FMETRE_VTB field in the FMETRE_VTB register.
 */
__INLINE uint32_t crm_fmetre_vtb_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_VTB_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_PHY register definitions
 * <table>
 * <caption>FMETRE_PHY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_PHY <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_PHY register
#define CRM_FMETRE_PHY_ADDR   0x2494032C
/// Offset of the FMETRE_PHY register from the base address
#define CRM_FMETRE_PHY_OFFSET 0x0000032C
/// Index of the FMETRE_PHY register
#define CRM_FMETRE_PHY_INDEX  0x000000CB
/// Reset value of the FMETRE_PHY register
#define CRM_FMETRE_PHY_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_PHY register.
 * The FMETRE_PHY register will be read and its value returned.
 * @return The current value of the FMETRE_PHY register.
 */
__INLINE uint32_t crm_fmetre_phy_get(void)
{
    return REG_PL_RD(CRM_FMETRE_PHY_ADDR);
}

// field definitions
/// FMETRE_PHY field mask
#define CRM_FMETRE_PHY_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_PHY field LSB position
#define CRM_FMETRE_PHY_LSB    0
/// FMETRE_PHY field width
#define CRM_FMETRE_PHY_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_PHY field reset value
#define CRM_FMETRE_PHY_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_PHY field in the FMETRE_PHY register.
 *
 * The FMETRE_PHY register will be read and the FMETRE_PHY field's value will be returned.
 *
 * @return The current value of the FMETRE_PHY field in the FMETRE_PHY register.
 */
__INLINE uint32_t crm_fmetre_phy_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_PHY_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_MPIF register definitions
 * <table>
 * <caption>FMETRE_MPIF bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_MPIF <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_MPIF register
#define CRM_FMETRE_MPIF_ADDR   0x24940330
/// Offset of the FMETRE_MPIF register from the base address
#define CRM_FMETRE_MPIF_OFFSET 0x00000330
/// Index of the FMETRE_MPIF register
#define CRM_FMETRE_MPIF_INDEX  0x000000CC
/// Reset value of the FMETRE_MPIF register
#define CRM_FMETRE_MPIF_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_MPIF register.
 * The FMETRE_MPIF register will be read and its value returned.
 * @return The current value of the FMETRE_MPIF register.
 */
__INLINE uint32_t crm_fmetre_mpif_get(void)
{
    return REG_PL_RD(CRM_FMETRE_MPIF_ADDR);
}

// field definitions
/// FMETRE_MPIF field mask
#define CRM_FMETRE_MPIF_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_MPIF field LSB position
#define CRM_FMETRE_MPIF_LSB    0
/// FMETRE_MPIF field width
#define CRM_FMETRE_MPIF_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_MPIF field reset value
#define CRM_FMETRE_MPIF_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_MPIF field in the FMETRE_MPIF register.
 *
 * The FMETRE_MPIF register will be read and the FMETRE_MPIF field's value will be returned.
 *
 * @return The current value of the FMETRE_MPIF field in the FMETRE_MPIF register.
 */
__INLINE uint32_t crm_fmetre_mpif_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_MPIF_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_LA register definitions
 * <table>
 * <caption>FMETRE_LA bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_LA <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_LA register
#define CRM_FMETRE_LA_ADDR   0x24940334
/// Offset of the FMETRE_LA register from the base address
#define CRM_FMETRE_LA_OFFSET 0x00000334
/// Index of the FMETRE_LA register
#define CRM_FMETRE_LA_INDEX  0x000000CD
/// Reset value of the FMETRE_LA register
#define CRM_FMETRE_LA_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_LA register.
 * The FMETRE_LA register will be read and its value returned.
 * @return The current value of the FMETRE_LA register.
 */
__INLINE uint32_t crm_fmetre_la_get(void)
{
    return REG_PL_RD(CRM_FMETRE_LA_ADDR);
}

// field definitions
/// FMETRE_LA field mask
#define CRM_FMETRE_LA_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_LA field LSB position
#define CRM_FMETRE_LA_LSB    0
/// FMETRE_LA field width
#define CRM_FMETRE_LA_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_LA field reset value
#define CRM_FMETRE_LA_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_LA field in the FMETRE_LA register.
 *
 * The FMETRE_LA register will be read and the FMETRE_LA field's value will be returned.
 *
 * @return The current value of the FMETRE_LA field in the FMETRE_LA register.
 */
__INLINE uint32_t crm_fmetre_la_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_LA_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_MACCORE register definitions
 * <table>
 * <caption>FMETRE_MACCORE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_MACCORE <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_MACCORE register
#define CRM_FMETRE_MACCORE_ADDR   0x24940338
/// Offset of the FMETRE_MACCORE register from the base address
#define CRM_FMETRE_MACCORE_OFFSET 0x00000338
/// Index of the FMETRE_MACCORE register
#define CRM_FMETRE_MACCORE_INDEX  0x000000CE
/// Reset value of the FMETRE_MACCORE register
#define CRM_FMETRE_MACCORE_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_MACCORE register.
 * The FMETRE_MACCORE register will be read and its value returned.
 * @return The current value of the FMETRE_MACCORE register.
 */
__INLINE uint32_t crm_fmetre_maccore_get(void)
{
    return REG_PL_RD(CRM_FMETRE_MACCORE_ADDR);
}

// field definitions
/// FMETRE_MACCORE field mask
#define CRM_FMETRE_MACCORE_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_MACCORE field LSB position
#define CRM_FMETRE_MACCORE_LSB    0
/// FMETRE_MACCORE field width
#define CRM_FMETRE_MACCORE_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_MACCORE field reset value
#define CRM_FMETRE_MACCORE_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_MACCORE field in the FMETRE_MACCORE register.
 *
 * The FMETRE_MACCORE register will be read and the FMETRE_MACCORE field's value will be returned.
 *
 * @return The current value of the FMETRE_MACCORE field in the FMETRE_MACCORE register.
 */
__INLINE uint32_t crm_fmetre_maccore_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_MACCORE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_MACWT register definitions
 * <table>
 * <caption>FMETRE_MACWT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_MACWT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_MACWT register
#define CRM_FMETRE_MACWT_ADDR   0x2494033C
/// Offset of the FMETRE_MACWT register from the base address
#define CRM_FMETRE_MACWT_OFFSET 0x0000033C
/// Index of the FMETRE_MACWT register
#define CRM_FMETRE_MACWT_INDEX  0x000000CF
/// Reset value of the FMETRE_MACWT register
#define CRM_FMETRE_MACWT_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_MACWT register.
 * The FMETRE_MACWT register will be read and its value returned.
 * @return The current value of the FMETRE_MACWT register.
 */
__INLINE uint32_t crm_fmetre_macwt_get(void)
{
    return REG_PL_RD(CRM_FMETRE_MACWT_ADDR);
}

// field definitions
/// FMETRE_MACWT field mask
#define CRM_FMETRE_MACWT_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_MACWT field LSB position
#define CRM_FMETRE_MACWT_LSB    0
/// FMETRE_MACWT field width
#define CRM_FMETRE_MACWT_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_MACWT field reset value
#define CRM_FMETRE_MACWT_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_MACWT field in the FMETRE_MACWT register.
 *
 * The FMETRE_MACWT register will be read and the FMETRE_MACWT field's value will be returned.
 *
 * @return The current value of the FMETRE_MACWT field in the FMETRE_MACWT register.
 */
__INLINE uint32_t crm_fmetre_macwt_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_MACWT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_PLF register definitions
 * <table>
 * <caption>FMETRE_PLF bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_PLF <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_PLF register
#define CRM_FMETRE_PLF_ADDR   0x24940340
/// Offset of the FMETRE_PLF register from the base address
#define CRM_FMETRE_PLF_OFFSET 0x00000340
/// Index of the FMETRE_PLF register
#define CRM_FMETRE_PLF_INDEX  0x000000D0
/// Reset value of the FMETRE_PLF register
#define CRM_FMETRE_PLF_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_PLF register.
 * The FMETRE_PLF register will be read and its value returned.
 * @return The current value of the FMETRE_PLF register.
 */
__INLINE uint32_t crm_fmetre_plf_get(void)
{
    return REG_PL_RD(CRM_FMETRE_PLF_ADDR);
}

// field definitions
/// FMETRE_PLF field mask
#define CRM_FMETRE_PLF_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_PLF field LSB position
#define CRM_FMETRE_PLF_LSB    0
/// FMETRE_PLF field width
#define CRM_FMETRE_PLF_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_PLF field reset value
#define CRM_FMETRE_PLF_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_PLF field in the FMETRE_PLF register.
 *
 * The FMETRE_PLF register will be read and the FMETRE_PLF field's value will be returned.
 *
 * @return The current value of the FMETRE_PLF field in the FMETRE_PLF register.
 */
__INLINE uint32_t crm_fmetre_plf_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_PLF_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FMETRE_DAC register definitions
 * <table>
 * <caption>FMETRE_DAC bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> FMETRE_DAC <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FMETRE_DAC register
#define CRM_FMETRE_DAC_ADDR   0x24940344
/// Offset of the FMETRE_DAC register from the base address
#define CRM_FMETRE_DAC_OFFSET 0x00000344
/// Index of the FMETRE_DAC register
#define CRM_FMETRE_DAC_INDEX  0x000000D1
/// Reset value of the FMETRE_DAC register
#define CRM_FMETRE_DAC_RESET  0x00000000

/**
 * @brief Returns the current value of the FMETRE_DAC register.
 * The FMETRE_DAC register will be read and its value returned.
 * @return The current value of the FMETRE_DAC register.
 */
__INLINE uint32_t crm_fmetre_dac_get(void)
{
    return REG_PL_RD(CRM_FMETRE_DAC_ADDR);
}

// field definitions
/// FMETRE_DAC field mask
#define CRM_FMETRE_DAC_MASK   ((uint32_t)0x000FFFFF)
/// FMETRE_DAC field LSB position
#define CRM_FMETRE_DAC_LSB    0
/// FMETRE_DAC field width
#define CRM_FMETRE_DAC_WIDTH  ((uint32_t)0x00000014)

/// FMETRE_DAC field reset value
#define CRM_FMETRE_DAC_RST    0x0

/**
 * @brief Returns the current value of the FMETRE_DAC field in the FMETRE_DAC register.
 *
 * The FMETRE_DAC register will be read and the FMETRE_DAC field's value will be returned.
 *
 * @return The current value of the FMETRE_DAC field in the FMETRE_DAC register.
 */
__INLINE uint32_t crm_fmetre_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_FMETRE_DAC_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}


#endif // _REG_CRM_H_

/// @}

