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
 * @file reg_board_if.h
 * @brief Definitions of the BOARD HW block registers and register access functions.
 *
 * @defgroup REG_BOARD_IF REG_BOARD_IF
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the BOARD HW block registers and register access functions.
 */
#ifndef _REG_BOARD_IF_H_
#define _REG_BOARD_IF_H_

#include "co_int.h"
#include "_reg_board_if.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_BOARD_IF peripheral.
 */
#define REG_BOARD_IF_COUNT 326

/** @brief Decoding mask of the REG_BOARD_IF peripheral registers from the CPU point of view.
 */
#define REG_BOARD_IF_DECODING_MASK 0x000007FF

/**
 * @name SIGNATURE register definitions
 * <table>
 * <caption>SIGNATURE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> FPGA_SIGNATURE <td> R/W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SIGNATURE register
#define BOARD_SIGNATURE_ADDR   0x24F10000
/// Offset of the SIGNATURE register from the base address
#define BOARD_SIGNATURE_OFFSET 0x00000000
/// Index of the SIGNATURE register
#define BOARD_SIGNATURE_INDEX  0x00000000
/// Reset value of the SIGNATURE register
#define BOARD_SIGNATURE_RESET  0x00000000

/**
 * @brief Returns the current value of the SIGNATURE register.
 * The SIGNATURE register will be read and its value returned.
 * @return The current value of the SIGNATURE register.
 */
__INLINE uint32_t board_signature_get(void)
{
    return REG_PL_RD(BOARD_SIGNATURE_ADDR);
}

// field definitions
/// FPGA_SIGNATURE field mask
#define BOARD_FPGA_SIGNATURE_MASK   ((uint32_t)0xFFFFFFFF)
/// FPGA_SIGNATURE field LSB position
#define BOARD_FPGA_SIGNATURE_LSB    0
/// FPGA_SIGNATURE field width
#define BOARD_FPGA_SIGNATURE_WIDTH  ((uint32_t)0x00000020)

/// FPGA_SIGNATURE field reset value
#define BOARD_FPGA_SIGNATURE_RST    0x0

/**
 * @brief Returns the current value of the FPGA_SIGNATURE field in the SIGNATURE register.
 *
 * The SIGNATURE register will be read and the FPGA_SIGNATURE field's value will be returned.
 *
 * @return The current value of the FPGA_SIGNATURE field in the SIGNATURE register.
 */
__INLINE uint32_t board_fpga_signature_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_SIGNATURE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DATE register definitions
 * <table>
 * <caption>DATE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> FPGA_DATE <td> R <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DATE register
#define BOARD_DATE_ADDR   0x24F10004
/// Offset of the DATE register from the base address
#define BOARD_DATE_OFFSET 0x00000004
/// Index of the DATE register
#define BOARD_DATE_INDEX  0x00000001
/// Reset value of the DATE register
#define BOARD_DATE_RESET  0x00000000

/**
 * @brief Returns the current value of the DATE register.
 * The DATE register will be read and its value returned.
 * @return The current value of the DATE register.
 */
__INLINE uint32_t board_date_get(void)
{
    return REG_PL_RD(BOARD_DATE_ADDR);
}

// field definitions
/// FPGA_DATE field mask
#define BOARD_FPGA_DATE_MASK   ((uint32_t)0xFFFFFFFF)
/// FPGA_DATE field LSB position
#define BOARD_FPGA_DATE_LSB    0
/// FPGA_DATE field width
#define BOARD_FPGA_DATE_WIDTH  ((uint32_t)0x00000020)

/// FPGA_DATE field reset value
#define BOARD_FPGA_DATE_RST    0x0

/**
 * @brief Returns the current value of the FPGA_DATE field in the DATE register.
 *
 * The DATE register will be read and the FPGA_DATE field's value will be returned.
 *
 * @return The current value of the FPGA_DATE field in the DATE register.
 */
__INLINE uint32_t board_fpga_date_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DATE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DIAGPORT_CONF1 register definitions
 * <table>
 * <caption>DIAGPORT_CONF1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:08 <td> MICTOR2_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 07:00 <td> MICTOR1_SEL <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAGPORT_CONF1 register
#define BOARD_DIAGPORT_CONF1_ADDR   0x24F10010
/// Offset of the DIAGPORT_CONF1 register from the base address
#define BOARD_DIAGPORT_CONF1_OFFSET 0x00000010
/// Index of the DIAGPORT_CONF1 register
#define BOARD_DIAGPORT_CONF1_INDEX  0x00000004
/// Reset value of the DIAGPORT_CONF1 register
#define BOARD_DIAGPORT_CONF1_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAGPORT_CONF1 register.
 * The DIAGPORT_CONF1 register will be read and its value returned.
 * @return The current value of the DIAGPORT_CONF1 register.
 */
__INLINE uint32_t board_diagport_conf1_get(void)
{
    return REG_PL_RD(BOARD_DIAGPORT_CONF1_ADDR);
}

/**
 * @brief Sets the DIAGPORT_CONF1 register to a value.
 * The DIAGPORT_CONF1 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_diagport_conf1_set(uint32_t value)
{
    REG_PL_WR(BOARD_DIAGPORT_CONF1_ADDR, value);
}

// field definitions
/// MICTOR2_SEL field mask
#define BOARD_MICTOR2_SEL_MASK   ((uint32_t)0x0000FF00)
/// MICTOR2_SEL field LSB position
#define BOARD_MICTOR2_SEL_LSB    8
/// MICTOR2_SEL field width
#define BOARD_MICTOR2_SEL_WIDTH  ((uint32_t)0x00000008)
/// MICTOR1_SEL field mask
#define BOARD_MICTOR1_SEL_MASK   ((uint32_t)0x000000FF)
/// MICTOR1_SEL field LSB position
#define BOARD_MICTOR1_SEL_LSB    0
/// MICTOR1_SEL field width
#define BOARD_MICTOR1_SEL_WIDTH  ((uint32_t)0x00000008)

/// MICTOR2_SEL field reset value
#define BOARD_MICTOR2_SEL_RST    0x0
/// MICTOR1_SEL field reset value
#define BOARD_MICTOR1_SEL_RST    0x0

/**
 * @brief Constructs a value for the DIAGPORT_CONF1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] mictor2sel - The value to use for the MICTOR2_SEL field.
 * @param[in] mictor1sel - The value to use for the MICTOR1_SEL field.
 */
__INLINE void board_diagport_conf1_pack(uint8_t mictor2sel, uint8_t mictor1sel)
{
    ASSERT_ERR((((uint32_t)mictor2sel << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)mictor1sel << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(BOARD_DIAGPORT_CONF1_ADDR,  ((uint32_t)mictor2sel << 8) | ((uint32_t)mictor1sel << 0));
}

/**
 * @brief Unpacks DIAGPORT_CONF1's fields from current value of the DIAGPORT_CONF1 register.
 *
 * Reads the DIAGPORT_CONF1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mictor2sel - Will be populated with the current value of this field from the register.
 * @param[out] mictor1sel - Will be populated with the current value of this field from the register.
 */
__INLINE void board_diagport_conf1_unpack(uint8_t* mictor2sel, uint8_t* mictor1sel)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAGPORT_CONF1_ADDR);

    *mictor2sel = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *mictor1sel = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the MICTOR2_SEL field in the DIAGPORT_CONF1 register.
 *
 * The DIAGPORT_CONF1 register will be read and the MICTOR2_SEL field's value will be returned.
 *
 * @return The current value of the MICTOR2_SEL field in the DIAGPORT_CONF1 register.
 */
__INLINE uint8_t board_mictor2_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAGPORT_CONF1_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the MICTOR2_SEL field of the DIAGPORT_CONF1 register.
 *
 * The DIAGPORT_CONF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mictor2sel - The value to set the field to.
 */
__INLINE void board_mictor2_sel_setf(uint8_t mictor2sel)
{
    ASSERT_ERR((((uint32_t)mictor2sel << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(BOARD_DIAGPORT_CONF1_ADDR, (REG_PL_RD(BOARD_DIAGPORT_CONF1_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)mictor2sel << 8));
}

/**
 * @brief Returns the current value of the MICTOR1_SEL field in the DIAGPORT_CONF1 register.
 *
 * The DIAGPORT_CONF1 register will be read and the MICTOR1_SEL field's value will be returned.
 *
 * @return The current value of the MICTOR1_SEL field in the DIAGPORT_CONF1 register.
 */
__INLINE uint8_t board_mictor1_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAGPORT_CONF1_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the MICTOR1_SEL field of the DIAGPORT_CONF1 register.
 *
 * The DIAGPORT_CONF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mictor1sel - The value to set the field to.
 */
__INLINE void board_mictor1_sel_setf(uint8_t mictor1sel)
{
    ASSERT_ERR((((uint32_t)mictor1sel << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(BOARD_DIAGPORT_CONF1_ADDR, (REG_PL_RD(BOARD_DIAGPORT_CONF1_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)mictor1sel << 0));
}

/// @}

/**
 * @name DIAGPORT_CONF2 register definitions
 * <table>
 * <caption>DIAGPORT_CONF2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td> MICTOR0_SEL <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAGPORT_CONF2 register
#define BOARD_DIAGPORT_CONF2_ADDR   0x24F10014
/// Offset of the DIAGPORT_CONF2 register from the base address
#define BOARD_DIAGPORT_CONF2_OFFSET 0x00000014
/// Index of the DIAGPORT_CONF2 register
#define BOARD_DIAGPORT_CONF2_INDEX  0x00000005
/// Reset value of the DIAGPORT_CONF2 register
#define BOARD_DIAGPORT_CONF2_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAGPORT_CONF2 register.
 * The DIAGPORT_CONF2 register will be read and its value returned.
 * @return The current value of the DIAGPORT_CONF2 register.
 */
__INLINE uint32_t board_diagport_conf2_get(void)
{
    return REG_PL_RD(BOARD_DIAGPORT_CONF2_ADDR);
}

/**
 * @brief Sets the DIAGPORT_CONF2 register to a value.
 * The DIAGPORT_CONF2 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_diagport_conf2_set(uint32_t value)
{
    REG_PL_WR(BOARD_DIAGPORT_CONF2_ADDR, value);
}

// field definitions
/// MICTOR0_SEL field mask
#define BOARD_MICTOR0_SEL_MASK   ((uint32_t)0x0000FFFF)
/// MICTOR0_SEL field LSB position
#define BOARD_MICTOR0_SEL_LSB    0
/// MICTOR0_SEL field width
#define BOARD_MICTOR0_SEL_WIDTH  ((uint32_t)0x00000010)

/// MICTOR0_SEL field reset value
#define BOARD_MICTOR0_SEL_RST    0x0

/**
 * @brief Returns the current value of the MICTOR0_SEL field in the DIAGPORT_CONF2 register.
 *
 * The DIAGPORT_CONF2 register will be read and the MICTOR0_SEL field's value will be returned.
 *
 * @return The current value of the MICTOR0_SEL field in the DIAGPORT_CONF2 register.
 */
__INLINE uint16_t board_mictor0_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAGPORT_CONF2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the MICTOR0_SEL field of the DIAGPORT_CONF2 register.
 *
 * The DIAGPORT_CONF2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mictor0sel - The value to set the field to.
 */
__INLINE void board_mictor0_sel_setf(uint16_t mictor0sel)
{
    ASSERT_ERR((((uint32_t)mictor0sel << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_DIAGPORT_CONF2_ADDR, (uint32_t)mictor0sel << 0);
}

/// @}

/**
 * @name FB_CNTL register definitions
 * <table>
 * <caption>FB_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>   FB_CAPTURE_DONE <td> R/W <td> R <td> 0
 * <tr><td> 21:20 <td>     FB_CAPTURE_EN <td> R <td> R/W <td> 0x0
 * <tr><td> 17:16 <td>    FB_PLAYBACK_EN <td> R <td> R/W <td> 0x0
 * <tr><td> 15:00 <td> FB_PLAYBACK_COUNT <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FB_CNTL register
#define BOARD_FB_CNTL_ADDR   0x24F10020
/// Offset of the FB_CNTL register from the base address
#define BOARD_FB_CNTL_OFFSET 0x00000020
/// Index of the FB_CNTL register
#define BOARD_FB_CNTL_INDEX  0x00000008
/// Reset value of the FB_CNTL register
#define BOARD_FB_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the FB_CNTL register.
 * The FB_CNTL register will be read and its value returned.
 * @return The current value of the FB_CNTL register.
 */
__INLINE uint32_t board_fb_cntl_get(void)
{
    return REG_PL_RD(BOARD_FB_CNTL_ADDR);
}

/**
 * @brief Sets the FB_CNTL register to a value.
 * The FB_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void board_fb_cntl_set(uint32_t value)
{
    REG_PL_WR(BOARD_FB_CNTL_ADDR, value);
}

// field definitions
/// FB_CAPTURE_DONE field bit
#define BOARD_FB_CAPTURE_DONE_BIT      ((uint32_t)0x80000000)
/// FB_CAPTURE_DONE field position
#define BOARD_FB_CAPTURE_DONE_POS      31
/// FB_CAPTURE_EN field mask
#define BOARD_FB_CAPTURE_EN_MASK       ((uint32_t)0x00300000)
/// FB_CAPTURE_EN field LSB position
#define BOARD_FB_CAPTURE_EN_LSB        20
/// FB_CAPTURE_EN field width
#define BOARD_FB_CAPTURE_EN_WIDTH      ((uint32_t)0x00000002)
/// FB_PLAYBACK_EN field mask
#define BOARD_FB_PLAYBACK_EN_MASK      ((uint32_t)0x00030000)
/// FB_PLAYBACK_EN field LSB position
#define BOARD_FB_PLAYBACK_EN_LSB       16
/// FB_PLAYBACK_EN field width
#define BOARD_FB_PLAYBACK_EN_WIDTH     ((uint32_t)0x00000002)
/// FB_PLAYBACK_COUNT field mask
#define BOARD_FB_PLAYBACK_COUNT_MASK   ((uint32_t)0x0000FFFF)
/// FB_PLAYBACK_COUNT field LSB position
#define BOARD_FB_PLAYBACK_COUNT_LSB    0
/// FB_PLAYBACK_COUNT field width
#define BOARD_FB_PLAYBACK_COUNT_WIDTH  ((uint32_t)0x00000010)

/// FB_CAPTURE_DONE field reset value
#define BOARD_FB_CAPTURE_DONE_RST      0x0
/// FB_CAPTURE_EN field reset value
#define BOARD_FB_CAPTURE_EN_RST        0x0
/// FB_PLAYBACK_EN field reset value
#define BOARD_FB_PLAYBACK_EN_RST       0x0
/// FB_PLAYBACK_COUNT field reset value
#define BOARD_FB_PLAYBACK_COUNT_RST    0x0

/**
 * @brief Constructs a value for the FB_CNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fbcaptureen - The value to use for the FB_CAPTURE_EN field.
 * @param[in] fbplaybacken - The value to use for the FB_PLAYBACK_EN field.
 * @param[in] fbplaybackcount - The value to use for the FB_PLAYBACK_COUNT field.
 */
__INLINE void board_fb_cntl_pack(uint8_t fbcaptureen, uint8_t fbplaybacken, uint16_t fbplaybackcount)
{
    ASSERT_ERR((((uint32_t)fbcaptureen << 20) & ~((uint32_t)0x00300000)) == 0);
    ASSERT_ERR((((uint32_t)fbplaybacken << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)fbplaybackcount << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_FB_CNTL_ADDR,  ((uint32_t)fbcaptureen << 20) | ((uint32_t)fbplaybacken << 16) | ((uint32_t)fbplaybackcount << 0));
}

/**
 * @brief Unpacks FB_CNTL's fields from current value of the FB_CNTL register.
 *
 * Reads the FB_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fbcapturedone - Will be populated with the current value of this field from the register.
 * @param[out] fbcaptureen - Will be populated with the current value of this field from the register.
 * @param[out] fbplaybacken - Will be populated with the current value of this field from the register.
 * @param[out] fbplaybackcount - Will be populated with the current value of this field from the register.
 */
__INLINE void board_fb_cntl_unpack(uint8_t* fbcapturedone, uint8_t* fbcaptureen, uint8_t* fbplaybacken, uint16_t* fbplaybackcount)
{
    uint32_t localVal = REG_PL_RD(BOARD_FB_CNTL_ADDR);

    *fbcapturedone = (localVal & ((uint32_t)0x80000000)) >> 31;
    *fbcaptureen = (localVal & ((uint32_t)0x00300000)) >> 20;
    *fbplaybacken = (localVal & ((uint32_t)0x00030000)) >> 16;
    *fbplaybackcount = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the FB_CAPTURE_DONE field in the FB_CNTL register.
 *
 * The FB_CNTL register will be read and the FB_CAPTURE_DONE field's value will be returned.
 *
 * @return The current value of the FB_CAPTURE_DONE field in the FB_CNTL register.
 */
__INLINE uint8_t board_fb_capture_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_FB_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the FB_CAPTURE_EN field in the FB_CNTL register.
 *
 * The FB_CNTL register will be read and the FB_CAPTURE_EN field's value will be returned.
 *
 * @return The current value of the FB_CAPTURE_EN field in the FB_CNTL register.
 */
__INLINE uint8_t board_fb_capture_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_FB_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

/**
 * @brief Sets the FB_CAPTURE_EN field of the FB_CNTL register.
 *
 * The FB_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fbcaptureen - The value to set the field to.
 */
__INLINE void board_fb_capture_en_setf(uint8_t fbcaptureen)
{
    ASSERT_ERR((((uint32_t)fbcaptureen << 20) & ~((uint32_t)0x00300000)) == 0);
    REG_PL_WR(BOARD_FB_CNTL_ADDR, (REG_PL_RD(BOARD_FB_CNTL_ADDR) & ~((uint32_t)0x00300000)) | ((uint32_t)fbcaptureen << 20));
}

/**
 * @brief Returns the current value of the FB_PLAYBACK_EN field in the FB_CNTL register.
 *
 * The FB_CNTL register will be read and the FB_PLAYBACK_EN field's value will be returned.
 *
 * @return The current value of the FB_PLAYBACK_EN field in the FB_CNTL register.
 */
__INLINE uint8_t board_fb_playback_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_FB_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the FB_PLAYBACK_EN field of the FB_CNTL register.
 *
 * The FB_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fbplaybacken - The value to set the field to.
 */
__INLINE void board_fb_playback_en_setf(uint8_t fbplaybacken)
{
    ASSERT_ERR((((uint32_t)fbplaybacken << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(BOARD_FB_CNTL_ADDR, (REG_PL_RD(BOARD_FB_CNTL_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)fbplaybacken << 16));
}

/**
 * @brief Returns the current value of the FB_PLAYBACK_COUNT field in the FB_CNTL register.
 *
 * The FB_CNTL register will be read and the FB_PLAYBACK_COUNT field's value will be returned.
 *
 * @return The current value of the FB_PLAYBACK_COUNT field in the FB_CNTL register.
 */
__INLINE uint16_t board_fb_playback_count_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_FB_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the FB_PLAYBACK_COUNT field of the FB_CNTL register.
 *
 * The FB_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fbplaybackcount - The value to set the field to.
 */
__INLINE void board_fb_playback_count_setf(uint16_t fbplaybackcount)
{
    ASSERT_ERR((((uint32_t)fbplaybackcount << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_FB_CNTL_ADDR, (REG_PL_RD(BOARD_FB_CNTL_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)fbplaybackcount << 0));
}

/// @}

/**
 * @name FB_PATHMUX register definitions
 * <table>
 * <caption>FB_PATHMUX bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 03:00 <td> FB_PATHMUX_SEL <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FB_PATHMUX register
#define BOARD_FB_PATHMUX_ADDR   0x24F10024
/// Offset of the FB_PATHMUX register from the base address
#define BOARD_FB_PATHMUX_OFFSET 0x00000024
/// Index of the FB_PATHMUX register
#define BOARD_FB_PATHMUX_INDEX  0x00000009
/// Reset value of the FB_PATHMUX register
#define BOARD_FB_PATHMUX_RESET  0x00000000

/**
 * @brief Returns the current value of the FB_PATHMUX register.
 * The FB_PATHMUX register will be read and its value returned.
 * @return The current value of the FB_PATHMUX register.
 */
__INLINE uint32_t board_fb_pathmux_get(void)
{
    return REG_PL_RD(BOARD_FB_PATHMUX_ADDR);
}

/**
 * @brief Sets the FB_PATHMUX register to a value.
 * The FB_PATHMUX register will be written.
 * @param value - The value to write.
 */
__INLINE void board_fb_pathmux_set(uint32_t value)
{
    REG_PL_WR(BOARD_FB_PATHMUX_ADDR, value);
}

// field definitions
/// FB_PATHMUX_SEL field mask
#define BOARD_FB_PATHMUX_SEL_MASK   ((uint32_t)0x0000000F)
/// FB_PATHMUX_SEL field LSB position
#define BOARD_FB_PATHMUX_SEL_LSB    0
/// FB_PATHMUX_SEL field width
#define BOARD_FB_PATHMUX_SEL_WIDTH  ((uint32_t)0x00000004)

/// FB_PATHMUX_SEL field reset value
#define BOARD_FB_PATHMUX_SEL_RST    0x0

/**
 * @brief Returns the current value of the FB_PATHMUX_SEL field in the FB_PATHMUX register.
 *
 * The FB_PATHMUX register will be read and the FB_PATHMUX_SEL field's value will be returned.
 *
 * @return The current value of the FB_PATHMUX_SEL field in the FB_PATHMUX register.
 */
__INLINE uint8_t board_fb_pathmux_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_FB_PATHMUX_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000000F)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the FB_PATHMUX_SEL field of the FB_PATHMUX register.
 *
 * The FB_PATHMUX register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fbpathmuxsel - The value to set the field to.
 */
__INLINE void board_fb_pathmux_sel_setf(uint8_t fbpathmuxsel)
{
    ASSERT_ERR((((uint32_t)fbpathmuxsel << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(BOARD_FB_PATHMUX_ADDR, (uint32_t)fbpathmuxsel << 0);
}

/// @}

/**
 * @name ADC_SPI_CNTL register definitions
 * <table>
 * <caption>ADC_SPI_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>      ADC_SPI_DONE <td> R/W <td> R <td> 0
 * <tr><td> 12:08 <td> ADC_SPI_PRESCALER <td> R <td> R/W <td> 0x0
 * <tr><td> 01    <td> ADC_SPI_AUTOPOWER <td> R <td> R/W <td> 0
 * <tr><td> 00    <td>     ADC_SPI_START <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the ADC_SPI_CNTL register
#define BOARD_ADC_SPI_CNTL_ADDR   0x24F10100
/// Offset of the ADC_SPI_CNTL register from the base address
#define BOARD_ADC_SPI_CNTL_OFFSET 0x00000100
/// Index of the ADC_SPI_CNTL register
#define BOARD_ADC_SPI_CNTL_INDEX  0x00000040
/// Reset value of the ADC_SPI_CNTL register
#define BOARD_ADC_SPI_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the ADC_SPI_CNTL register.
 * The ADC_SPI_CNTL register will be read and its value returned.
 * @return The current value of the ADC_SPI_CNTL register.
 */
__INLINE uint32_t board_adc_spi_cntl_get(void)
{
    return REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR);
}

/**
 * @brief Sets the ADC_SPI_CNTL register to a value.
 * The ADC_SPI_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void board_adc_spi_cntl_set(uint32_t value)
{
    REG_PL_WR(BOARD_ADC_SPI_CNTL_ADDR, value);
}

// field definitions
/// ADC_SPI_DONE field bit
#define BOARD_ADC_SPI_DONE_BIT         ((uint32_t)0x80000000)
/// ADC_SPI_DONE field position
#define BOARD_ADC_SPI_DONE_POS         31
/// ADC_SPI_PRESCALER field mask
#define BOARD_ADC_SPI_PRESCALER_MASK   ((uint32_t)0x00001F00)
/// ADC_SPI_PRESCALER field LSB position
#define BOARD_ADC_SPI_PRESCALER_LSB    8
/// ADC_SPI_PRESCALER field width
#define BOARD_ADC_SPI_PRESCALER_WIDTH  ((uint32_t)0x00000005)
/// ADC_SPI_AUTOPOWER field bit
#define BOARD_ADC_SPI_AUTOPOWER_BIT    ((uint32_t)0x00000002)
/// ADC_SPI_AUTOPOWER field position
#define BOARD_ADC_SPI_AUTOPOWER_POS    1
/// ADC_SPI_START field bit
#define BOARD_ADC_SPI_START_BIT        ((uint32_t)0x00000001)
/// ADC_SPI_START field position
#define BOARD_ADC_SPI_START_POS        0

/// ADC_SPI_DONE field reset value
#define BOARD_ADC_SPI_DONE_RST         0x0
/// ADC_SPI_PRESCALER field reset value
#define BOARD_ADC_SPI_PRESCALER_RST    0x0
/// ADC_SPI_AUTOPOWER field reset value
#define BOARD_ADC_SPI_AUTOPOWER_RST    0x0
/// ADC_SPI_START field reset value
#define BOARD_ADC_SPI_START_RST        0x0

/**
 * @brief Constructs a value for the ADC_SPI_CNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] adcspiprescaler - The value to use for the ADC_SPI_PRESCALER field.
 * @param[in] adcspiautopower - The value to use for the ADC_SPI_AUTOPOWER field.
 * @param[in] adcspistart - The value to use for the ADC_SPI_START field.
 */
__INLINE void board_adc_spi_cntl_pack(uint8_t adcspiprescaler, uint8_t adcspiautopower, uint8_t adcspistart)
{
    ASSERT_ERR((((uint32_t)adcspiprescaler << 8) & ~((uint32_t)0x00001F00)) == 0);
    ASSERT_ERR((((uint32_t)adcspiautopower << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)adcspistart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_CNTL_ADDR,  ((uint32_t)adcspiprescaler << 8) | ((uint32_t)adcspiautopower << 1) | ((uint32_t)adcspistart << 0));
}

/**
 * @brief Unpacks ADC_SPI_CNTL's fields from current value of the ADC_SPI_CNTL register.
 *
 * Reads the ADC_SPI_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] adcspidone - Will be populated with the current value of this field from the register.
 * @param[out] adcspiprescaler - Will be populated with the current value of this field from the register.
 * @param[out] adcspiautopower - Will be populated with the current value of this field from the register.
 * @param[out] adcspistart - Will be populated with the current value of this field from the register.
 */
__INLINE void board_adc_spi_cntl_unpack(uint8_t* adcspidone, uint8_t* adcspiprescaler, uint8_t* adcspiautopower, uint8_t* adcspistart)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR);

    *adcspidone = (localVal & ((uint32_t)0x80000000)) >> 31;
    *adcspiprescaler = (localVal & ((uint32_t)0x00001F00)) >> 8;
    *adcspiautopower = (localVal & ((uint32_t)0x00000002)) >> 1;
    *adcspistart = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the ADC_SPI_DONE field in the ADC_SPI_CNTL register.
 *
 * The ADC_SPI_CNTL register will be read and the ADC_SPI_DONE field's value will be returned.
 *
 * @return The current value of the ADC_SPI_DONE field in the ADC_SPI_CNTL register.
 */
__INLINE uint8_t board_adc_spi_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the ADC_SPI_PRESCALER field in the ADC_SPI_CNTL register.
 *
 * The ADC_SPI_CNTL register will be read and the ADC_SPI_PRESCALER field's value will be returned.
 *
 * @return The current value of the ADC_SPI_PRESCALER field in the ADC_SPI_CNTL register.
 */
__INLINE uint8_t board_adc_spi_prescaler_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00001F00)) >> 8);
}

/**
 * @brief Sets the ADC_SPI_PRESCALER field of the ADC_SPI_CNTL register.
 *
 * The ADC_SPI_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcspiprescaler - The value to set the field to.
 */
__INLINE void board_adc_spi_prescaler_setf(uint8_t adcspiprescaler)
{
    ASSERT_ERR((((uint32_t)adcspiprescaler << 8) & ~((uint32_t)0x00001F00)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_CNTL_ADDR, (REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR) & ~((uint32_t)0x00001F00)) | ((uint32_t)adcspiprescaler << 8));
}

/**
 * @brief Returns the current value of the ADC_SPI_AUTOPOWER field in the ADC_SPI_CNTL register.
 *
 * The ADC_SPI_CNTL register will be read and the ADC_SPI_AUTOPOWER field's value will be returned.
 *
 * @return The current value of the ADC_SPI_AUTOPOWER field in the ADC_SPI_CNTL register.
 */
__INLINE uint8_t board_adc_spi_autopower_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the ADC_SPI_AUTOPOWER field of the ADC_SPI_CNTL register.
 *
 * The ADC_SPI_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcspiautopower - The value to set the field to.
 */
__INLINE void board_adc_spi_autopower_setf(uint8_t adcspiautopower)
{
    ASSERT_ERR((((uint32_t)adcspiautopower << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_CNTL_ADDR, (REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)adcspiautopower << 1));
}

/**
 * @brief Returns the current value of the ADC_SPI_START field in the ADC_SPI_CNTL register.
 *
 * The ADC_SPI_CNTL register will be read and the ADC_SPI_START field's value will be returned.
 *
 * @return The current value of the ADC_SPI_START field in the ADC_SPI_CNTL register.
 */
__INLINE uint8_t board_adc_spi_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the ADC_SPI_START field of the ADC_SPI_CNTL register.
 *
 * The ADC_SPI_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcspistart - The value to set the field to.
 */
__INLINE void board_adc_spi_start_setf(uint8_t adcspistart)
{
    ASSERT_ERR((((uint32_t)adcspistart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_CNTL_ADDR, (REG_PL_RD(BOARD_ADC_SPI_CNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)adcspistart << 0));
}

/// @}

/**
 * @name ADC_SPI_ADDR register definitions
 * <table>
 * <caption>ADC_SPI_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>   ADC_SPI_WE <td> R <td> R/W <td> 0
 * <tr><td> 19:16 <td>  ADC_SPI_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 15:00 <td> ADC_SPI_ADDR <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the ADC_SPI_ADDR register
#define BOARD_ADC_SPI_ADDR_ADDR   0x24F10104
/// Offset of the ADC_SPI_ADDR register from the base address
#define BOARD_ADC_SPI_ADDR_OFFSET 0x00000104
/// Index of the ADC_SPI_ADDR register
#define BOARD_ADC_SPI_ADDR_INDEX  0x00000041
/// Reset value of the ADC_SPI_ADDR register
#define BOARD_ADC_SPI_ADDR_RESET  0x00000000

/**
 * @brief Returns the current value of the ADC_SPI_ADDR register.
 * The ADC_SPI_ADDR register will be read and its value returned.
 * @return The current value of the ADC_SPI_ADDR register.
 */
__INLINE uint32_t board_adc_spi_addr_get(void)
{
    return REG_PL_RD(BOARD_ADC_SPI_ADDR_ADDR);
}

/**
 * @brief Sets the ADC_SPI_ADDR register to a value.
 * The ADC_SPI_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void board_adc_spi_addr_set(uint32_t value)
{
    REG_PL_WR(BOARD_ADC_SPI_ADDR_ADDR, value);
}

// field definitions
/// ADC_SPI_WE field bit
#define BOARD_ADC_SPI_WE_BIT      ((uint32_t)0x80000000)
/// ADC_SPI_WE field position
#define BOARD_ADC_SPI_WE_POS      31
/// ADC_SPI_SEL field mask
#define BOARD_ADC_SPI_SEL_MASK    ((uint32_t)0x000F0000)
/// ADC_SPI_SEL field LSB position
#define BOARD_ADC_SPI_SEL_LSB     16
/// ADC_SPI_SEL field width
#define BOARD_ADC_SPI_SEL_WIDTH   ((uint32_t)0x00000004)
/// ADC_SPI_ADDR field mask
#define BOARD_ADC_SPI_ADDR_MASK   ((uint32_t)0x0000FFFF)
/// ADC_SPI_ADDR field LSB position
#define BOARD_ADC_SPI_ADDR_LSB    0
/// ADC_SPI_ADDR field width
#define BOARD_ADC_SPI_ADDR_WIDTH  ((uint32_t)0x00000010)

/// ADC_SPI_WE field reset value
#define BOARD_ADC_SPI_WE_RST      0x0
/// ADC_SPI_SEL field reset value
#define BOARD_ADC_SPI_SEL_RST     0x0
/// ADC_SPI_ADDR field reset value
#define BOARD_ADC_SPI_ADDR_RST    0x0

/**
 * @brief Constructs a value for the ADC_SPI_ADDR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] adcspiwe - The value to use for the ADC_SPI_WE field.
 * @param[in] adcspisel - The value to use for the ADC_SPI_SEL field.
 * @param[in] adcspiaddr - The value to use for the ADC_SPI_ADDR field.
 */
__INLINE void board_adc_spi_addr_pack(uint8_t adcspiwe, uint8_t adcspisel, uint16_t adcspiaddr)
{
    ASSERT_ERR((((uint32_t)adcspiwe << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)adcspisel << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)adcspiaddr << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_ADDR_ADDR,  ((uint32_t)adcspiwe << 31) | ((uint32_t)adcspisel << 16) | ((uint32_t)adcspiaddr << 0));
}

/**
 * @brief Unpacks ADC_SPI_ADDR's fields from current value of the ADC_SPI_ADDR register.
 *
 * Reads the ADC_SPI_ADDR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] adcspiwe - Will be populated with the current value of this field from the register.
 * @param[out] adcspisel - Will be populated with the current value of this field from the register.
 * @param[out] adcspiaddr - Will be populated with the current value of this field from the register.
 */
__INLINE void board_adc_spi_addr_unpack(uint8_t* adcspiwe, uint8_t* adcspisel, uint16_t* adcspiaddr)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_ADDR_ADDR);

    *adcspiwe = (localVal & ((uint32_t)0x80000000)) >> 31;
    *adcspisel = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *adcspiaddr = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the ADC_SPI_WE field in the ADC_SPI_ADDR register.
 *
 * The ADC_SPI_ADDR register will be read and the ADC_SPI_WE field's value will be returned.
 *
 * @return The current value of the ADC_SPI_WE field in the ADC_SPI_ADDR register.
 */
__INLINE uint8_t board_adc_spi_we_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the ADC_SPI_WE field of the ADC_SPI_ADDR register.
 *
 * The ADC_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcspiwe - The value to set the field to.
 */
__INLINE void board_adc_spi_we_setf(uint8_t adcspiwe)
{
    ASSERT_ERR((((uint32_t)adcspiwe << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_ADC_SPI_ADDR_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)adcspiwe << 31));
}

/**
 * @brief Returns the current value of the ADC_SPI_SEL field in the ADC_SPI_ADDR register.
 *
 * The ADC_SPI_ADDR register will be read and the ADC_SPI_SEL field's value will be returned.
 *
 * @return The current value of the ADC_SPI_SEL field in the ADC_SPI_ADDR register.
 */
__INLINE uint8_t board_adc_spi_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the ADC_SPI_SEL field of the ADC_SPI_ADDR register.
 *
 * The ADC_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcspisel - The value to set the field to.
 */
__INLINE void board_adc_spi_sel_setf(uint8_t adcspisel)
{
    ASSERT_ERR((((uint32_t)adcspisel << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_ADC_SPI_ADDR_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)adcspisel << 16));
}

/**
 * @brief Returns the current value of the ADC_SPI_ADDR field in the ADC_SPI_ADDR register.
 *
 * The ADC_SPI_ADDR register will be read and the ADC_SPI_ADDR field's value will be returned.
 *
 * @return The current value of the ADC_SPI_ADDR field in the ADC_SPI_ADDR register.
 */
__INLINE uint16_t board_adc_spi_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the ADC_SPI_ADDR field of the ADC_SPI_ADDR register.
 *
 * The ADC_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcspiaddr - The value to set the field to.
 */
__INLINE void board_adc_spi_addr_setf(uint16_t adcspiaddr)
{
    ASSERT_ERR((((uint32_t)adcspiaddr << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_ADC_SPI_ADDR_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)adcspiaddr << 0));
}

/// @}

/**
 * @name ADC_SPI_DATA register definitions
 * <table>
 * <caption>ADC_SPI_DATA bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td> ADC_SPI_RDATA <td> R/W <td> R <td> 0x0
 * <tr><td> 15:00 <td> ADC_SPI_WDATA <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the ADC_SPI_DATA register
#define BOARD_ADC_SPI_DATA_ADDR   0x24F10108
/// Offset of the ADC_SPI_DATA register from the base address
#define BOARD_ADC_SPI_DATA_OFFSET 0x00000108
/// Index of the ADC_SPI_DATA register
#define BOARD_ADC_SPI_DATA_INDEX  0x00000042
/// Reset value of the ADC_SPI_DATA register
#define BOARD_ADC_SPI_DATA_RESET  0x00000000

/**
 * @brief Returns the current value of the ADC_SPI_DATA register.
 * The ADC_SPI_DATA register will be read and its value returned.
 * @return The current value of the ADC_SPI_DATA register.
 */
__INLINE uint32_t board_adc_spi_data_get(void)
{
    return REG_PL_RD(BOARD_ADC_SPI_DATA_ADDR);
}

/**
 * @brief Sets the ADC_SPI_DATA register to a value.
 * The ADC_SPI_DATA register will be written.
 * @param value - The value to write.
 */
__INLINE void board_adc_spi_data_set(uint32_t value)
{
    REG_PL_WR(BOARD_ADC_SPI_DATA_ADDR, value);
}

// field definitions
/// ADC_SPI_RDATA field mask
#define BOARD_ADC_SPI_RDATA_MASK   ((uint32_t)0xFFFF0000)
/// ADC_SPI_RDATA field LSB position
#define BOARD_ADC_SPI_RDATA_LSB    16
/// ADC_SPI_RDATA field width
#define BOARD_ADC_SPI_RDATA_WIDTH  ((uint32_t)0x00000010)
/// ADC_SPI_WDATA field mask
#define BOARD_ADC_SPI_WDATA_MASK   ((uint32_t)0x0000FFFF)
/// ADC_SPI_WDATA field LSB position
#define BOARD_ADC_SPI_WDATA_LSB    0
/// ADC_SPI_WDATA field width
#define BOARD_ADC_SPI_WDATA_WIDTH  ((uint32_t)0x00000010)

/// ADC_SPI_RDATA field reset value
#define BOARD_ADC_SPI_RDATA_RST    0x0
/// ADC_SPI_WDATA field reset value
#define BOARD_ADC_SPI_WDATA_RST    0x0

/**
 * @brief Unpacks ADC_SPI_DATA's fields from current value of the ADC_SPI_DATA register.
 *
 * Reads the ADC_SPI_DATA register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] adcspirdata - Will be populated with the current value of this field from the register.
 * @param[out] adcspiwdata - Will be populated with the current value of this field from the register.
 */
__INLINE void board_adc_spi_data_unpack(uint16_t* adcspirdata, uint16_t* adcspiwdata)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_DATA_ADDR);

    *adcspirdata = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *adcspiwdata = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the ADC_SPI_RDATA field in the ADC_SPI_DATA register.
 *
 * The ADC_SPI_DATA register will be read and the ADC_SPI_RDATA field's value will be returned.
 *
 * @return The current value of the ADC_SPI_RDATA field in the ADC_SPI_DATA register.
 */
__INLINE uint16_t board_adc_spi_rdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_DATA_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the ADC_SPI_WDATA field in the ADC_SPI_DATA register.
 *
 * The ADC_SPI_DATA register will be read and the ADC_SPI_WDATA field's value will be returned.
 *
 * @return The current value of the ADC_SPI_WDATA field in the ADC_SPI_DATA register.
 */
__INLINE uint16_t board_adc_spi_wdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_ADC_SPI_DATA_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the ADC_SPI_WDATA field of the ADC_SPI_DATA register.
 *
 * The ADC_SPI_DATA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcspiwdata - The value to set the field to.
 */
__INLINE void board_adc_spi_wdata_setf(uint16_t adcspiwdata)
{
    ASSERT_ERR((((uint32_t)adcspiwdata << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_ADC_SPI_DATA_ADDR, (uint32_t)adcspiwdata << 0);
}

/// @}

/**
 * @name DAC_SPI_CNTL register definitions
 * <table>
 * <caption>DAC_SPI_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>      DAC_SPI_DONE <td> R/W <td> R <td> 0
 * <tr><td> 12:08 <td> DAC_SPI_PRESCALER <td> R <td> R/W <td> 0x0
 * <tr><td> 02    <td>         DAC_RESET <td> R <td> R/W <td> 0
 * <tr><td> 01    <td> DAC_SPI_AUTOPOWER <td> R <td> R/W <td> 0
 * <tr><td> 00    <td>     DAC_SPI_START <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the DAC_SPI_CNTL register
#define BOARD_DAC_SPI_CNTL_ADDR   0x24F10200
/// Offset of the DAC_SPI_CNTL register from the base address
#define BOARD_DAC_SPI_CNTL_OFFSET 0x00000200
/// Index of the DAC_SPI_CNTL register
#define BOARD_DAC_SPI_CNTL_INDEX  0x00000080
/// Reset value of the DAC_SPI_CNTL register
#define BOARD_DAC_SPI_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the DAC_SPI_CNTL register.
 * The DAC_SPI_CNTL register will be read and its value returned.
 * @return The current value of the DAC_SPI_CNTL register.
 */
__INLINE uint32_t board_dac_spi_cntl_get(void)
{
    return REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR);
}

/**
 * @brief Sets the DAC_SPI_CNTL register to a value.
 * The DAC_SPI_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void board_dac_spi_cntl_set(uint32_t value)
{
    REG_PL_WR(BOARD_DAC_SPI_CNTL_ADDR, value);
}

// field definitions
/// DAC_SPI_DONE field bit
#define BOARD_DAC_SPI_DONE_BIT         ((uint32_t)0x80000000)
/// DAC_SPI_DONE field position
#define BOARD_DAC_SPI_DONE_POS         31
/// DAC_SPI_PRESCALER field mask
#define BOARD_DAC_SPI_PRESCALER_MASK   ((uint32_t)0x00001F00)
/// DAC_SPI_PRESCALER field LSB position
#define BOARD_DAC_SPI_PRESCALER_LSB    8
/// DAC_SPI_PRESCALER field width
#define BOARD_DAC_SPI_PRESCALER_WIDTH  ((uint32_t)0x00000005)
/// DAC_RESET field bit
#define BOARD_DAC_RESET_BIT            ((uint32_t)0x00000004)
/// DAC_RESET field position
#define BOARD_DAC_RESET_POS            2
/// DAC_SPI_AUTOPOWER field bit
#define BOARD_DAC_SPI_AUTOPOWER_BIT    ((uint32_t)0x00000002)
/// DAC_SPI_AUTOPOWER field position
#define BOARD_DAC_SPI_AUTOPOWER_POS    1
/// DAC_SPI_START field bit
#define BOARD_DAC_SPI_START_BIT        ((uint32_t)0x00000001)
/// DAC_SPI_START field position
#define BOARD_DAC_SPI_START_POS        0

/// DAC_SPI_DONE field reset value
#define BOARD_DAC_SPI_DONE_RST         0x0
/// DAC_SPI_PRESCALER field reset value
#define BOARD_DAC_SPI_PRESCALER_RST    0x0
/// DAC_RESET field reset value
#define BOARD_DAC_RESET_RST            0x0
/// DAC_SPI_AUTOPOWER field reset value
#define BOARD_DAC_SPI_AUTOPOWER_RST    0x0
/// DAC_SPI_START field reset value
#define BOARD_DAC_SPI_START_RST        0x0

/**
 * @brief Constructs a value for the DAC_SPI_CNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dacspiprescaler - The value to use for the DAC_SPI_PRESCALER field.
 * @param[in] dacreset - The value to use for the DAC_RESET field.
 * @param[in] dacspiautopower - The value to use for the DAC_SPI_AUTOPOWER field.
 * @param[in] dacspistart - The value to use for the DAC_SPI_START field.
 */
__INLINE void board_dac_spi_cntl_pack(uint8_t dacspiprescaler, uint8_t dacreset, uint8_t dacspiautopower, uint8_t dacspistart)
{
    ASSERT_ERR((((uint32_t)dacspiprescaler << 8) & ~((uint32_t)0x00001F00)) == 0);
    ASSERT_ERR((((uint32_t)dacreset << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)dacspiautopower << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)dacspistart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_CNTL_ADDR,  ((uint32_t)dacspiprescaler << 8) | ((uint32_t)dacreset << 2) | ((uint32_t)dacspiautopower << 1) | ((uint32_t)dacspistart << 0));
}

/**
 * @brief Unpacks DAC_SPI_CNTL's fields from current value of the DAC_SPI_CNTL register.
 *
 * Reads the DAC_SPI_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dacspidone - Will be populated with the current value of this field from the register.
 * @param[out] dacspiprescaler - Will be populated with the current value of this field from the register.
 * @param[out] dacreset - Will be populated with the current value of this field from the register.
 * @param[out] dacspiautopower - Will be populated with the current value of this field from the register.
 * @param[out] dacspistart - Will be populated with the current value of this field from the register.
 */
__INLINE void board_dac_spi_cntl_unpack(uint8_t* dacspidone, uint8_t* dacspiprescaler, uint8_t* dacreset, uint8_t* dacspiautopower, uint8_t* dacspistart)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR);

    *dacspidone = (localVal & ((uint32_t)0x80000000)) >> 31;
    *dacspiprescaler = (localVal & ((uint32_t)0x00001F00)) >> 8;
    *dacreset = (localVal & ((uint32_t)0x00000004)) >> 2;
    *dacspiautopower = (localVal & ((uint32_t)0x00000002)) >> 1;
    *dacspistart = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the DAC_SPI_DONE field in the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read and the DAC_SPI_DONE field's value will be returned.
 *
 * @return The current value of the DAC_SPI_DONE field in the DAC_SPI_CNTL register.
 */
__INLINE uint8_t board_dac_spi_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the DAC_SPI_PRESCALER field in the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read and the DAC_SPI_PRESCALER field's value will be returned.
 *
 * @return The current value of the DAC_SPI_PRESCALER field in the DAC_SPI_CNTL register.
 */
__INLINE uint8_t board_dac_spi_prescaler_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00001F00)) >> 8);
}

/**
 * @brief Sets the DAC_SPI_PRESCALER field of the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacspiprescaler - The value to set the field to.
 */
__INLINE void board_dac_spi_prescaler_setf(uint8_t dacspiprescaler)
{
    ASSERT_ERR((((uint32_t)dacspiprescaler << 8) & ~((uint32_t)0x00001F00)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_CNTL_ADDR, (REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR) & ~((uint32_t)0x00001F00)) | ((uint32_t)dacspiprescaler << 8));
}

/**
 * @brief Returns the current value of the DAC_RESET field in the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read and the DAC_RESET field's value will be returned.
 *
 * @return The current value of the DAC_RESET field in the DAC_SPI_CNTL register.
 */
__INLINE uint8_t board_dac_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the DAC_RESET field of the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacreset - The value to set the field to.
 */
__INLINE void board_dac_reset_setf(uint8_t dacreset)
{
    ASSERT_ERR((((uint32_t)dacreset << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_CNTL_ADDR, (REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)dacreset << 2));
}

/**
 * @brief Returns the current value of the DAC_SPI_AUTOPOWER field in the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read and the DAC_SPI_AUTOPOWER field's value will be returned.
 *
 * @return The current value of the DAC_SPI_AUTOPOWER field in the DAC_SPI_CNTL register.
 */
__INLINE uint8_t board_dac_spi_autopower_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the DAC_SPI_AUTOPOWER field of the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacspiautopower - The value to set the field to.
 */
__INLINE void board_dac_spi_autopower_setf(uint8_t dacspiautopower)
{
    ASSERT_ERR((((uint32_t)dacspiautopower << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_CNTL_ADDR, (REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)dacspiautopower << 1));
}

/**
 * @brief Returns the current value of the DAC_SPI_START field in the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read and the DAC_SPI_START field's value will be returned.
 *
 * @return The current value of the DAC_SPI_START field in the DAC_SPI_CNTL register.
 */
__INLINE uint8_t board_dac_spi_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the DAC_SPI_START field of the DAC_SPI_CNTL register.
 *
 * The DAC_SPI_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacspistart - The value to set the field to.
 */
__INLINE void board_dac_spi_start_setf(uint8_t dacspistart)
{
    ASSERT_ERR((((uint32_t)dacspistart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_CNTL_ADDR, (REG_PL_RD(BOARD_DAC_SPI_CNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)dacspistart << 0));
}

/// @}

/**
 * @name DAC_SPI_ADDR register definitions
 * <table>
 * <caption>DAC_SPI_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>   DAC_SPI_WE <td> R <td> R/W <td> 0
 * <tr><td> 19:16 <td>  DAC_SPI_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 15:00 <td> DAC_SPI_ADDR <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DAC_SPI_ADDR register
#define BOARD_DAC_SPI_ADDR_ADDR   0x24F10204
/// Offset of the DAC_SPI_ADDR register from the base address
#define BOARD_DAC_SPI_ADDR_OFFSET 0x00000204
/// Index of the DAC_SPI_ADDR register
#define BOARD_DAC_SPI_ADDR_INDEX  0x00000081
/// Reset value of the DAC_SPI_ADDR register
#define BOARD_DAC_SPI_ADDR_RESET  0x00000000

/**
 * @brief Returns the current value of the DAC_SPI_ADDR register.
 * The DAC_SPI_ADDR register will be read and its value returned.
 * @return The current value of the DAC_SPI_ADDR register.
 */
__INLINE uint32_t board_dac_spi_addr_get(void)
{
    return REG_PL_RD(BOARD_DAC_SPI_ADDR_ADDR);
}

/**
 * @brief Sets the DAC_SPI_ADDR register to a value.
 * The DAC_SPI_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void board_dac_spi_addr_set(uint32_t value)
{
    REG_PL_WR(BOARD_DAC_SPI_ADDR_ADDR, value);
}

// field definitions
/// DAC_SPI_WE field bit
#define BOARD_DAC_SPI_WE_BIT      ((uint32_t)0x80000000)
/// DAC_SPI_WE field position
#define BOARD_DAC_SPI_WE_POS      31
/// DAC_SPI_SEL field mask
#define BOARD_DAC_SPI_SEL_MASK    ((uint32_t)0x000F0000)
/// DAC_SPI_SEL field LSB position
#define BOARD_DAC_SPI_SEL_LSB     16
/// DAC_SPI_SEL field width
#define BOARD_DAC_SPI_SEL_WIDTH   ((uint32_t)0x00000004)
/// DAC_SPI_ADDR field mask
#define BOARD_DAC_SPI_ADDR_MASK   ((uint32_t)0x0000FFFF)
/// DAC_SPI_ADDR field LSB position
#define BOARD_DAC_SPI_ADDR_LSB    0
/// DAC_SPI_ADDR field width
#define BOARD_DAC_SPI_ADDR_WIDTH  ((uint32_t)0x00000010)

/// DAC_SPI_WE field reset value
#define BOARD_DAC_SPI_WE_RST      0x0
/// DAC_SPI_SEL field reset value
#define BOARD_DAC_SPI_SEL_RST     0x0
/// DAC_SPI_ADDR field reset value
#define BOARD_DAC_SPI_ADDR_RST    0x0

/**
 * @brief Constructs a value for the DAC_SPI_ADDR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dacspiwe - The value to use for the DAC_SPI_WE field.
 * @param[in] dacspisel - The value to use for the DAC_SPI_SEL field.
 * @param[in] dacspiaddr - The value to use for the DAC_SPI_ADDR field.
 */
__INLINE void board_dac_spi_addr_pack(uint8_t dacspiwe, uint8_t dacspisel, uint16_t dacspiaddr)
{
    ASSERT_ERR((((uint32_t)dacspiwe << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)dacspisel << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)dacspiaddr << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_ADDR_ADDR,  ((uint32_t)dacspiwe << 31) | ((uint32_t)dacspisel << 16) | ((uint32_t)dacspiaddr << 0));
}

/**
 * @brief Unpacks DAC_SPI_ADDR's fields from current value of the DAC_SPI_ADDR register.
 *
 * Reads the DAC_SPI_ADDR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dacspiwe - Will be populated with the current value of this field from the register.
 * @param[out] dacspisel - Will be populated with the current value of this field from the register.
 * @param[out] dacspiaddr - Will be populated with the current value of this field from the register.
 */
__INLINE void board_dac_spi_addr_unpack(uint8_t* dacspiwe, uint8_t* dacspisel, uint16_t* dacspiaddr)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_ADDR_ADDR);

    *dacspiwe = (localVal & ((uint32_t)0x80000000)) >> 31;
    *dacspisel = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *dacspiaddr = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the DAC_SPI_WE field in the DAC_SPI_ADDR register.
 *
 * The DAC_SPI_ADDR register will be read and the DAC_SPI_WE field's value will be returned.
 *
 * @return The current value of the DAC_SPI_WE field in the DAC_SPI_ADDR register.
 */
__INLINE uint8_t board_dac_spi_we_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the DAC_SPI_WE field of the DAC_SPI_ADDR register.
 *
 * The DAC_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacspiwe - The value to set the field to.
 */
__INLINE void board_dac_spi_we_setf(uint8_t dacspiwe)
{
    ASSERT_ERR((((uint32_t)dacspiwe << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_DAC_SPI_ADDR_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)dacspiwe << 31));
}

/**
 * @brief Returns the current value of the DAC_SPI_SEL field in the DAC_SPI_ADDR register.
 *
 * The DAC_SPI_ADDR register will be read and the DAC_SPI_SEL field's value will be returned.
 *
 * @return The current value of the DAC_SPI_SEL field in the DAC_SPI_ADDR register.
 */
__INLINE uint8_t board_dac_spi_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the DAC_SPI_SEL field of the DAC_SPI_ADDR register.
 *
 * The DAC_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacspisel - The value to set the field to.
 */
__INLINE void board_dac_spi_sel_setf(uint8_t dacspisel)
{
    ASSERT_ERR((((uint32_t)dacspisel << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_DAC_SPI_ADDR_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)dacspisel << 16));
}

/**
 * @brief Returns the current value of the DAC_SPI_ADDR field in the DAC_SPI_ADDR register.
 *
 * The DAC_SPI_ADDR register will be read and the DAC_SPI_ADDR field's value will be returned.
 *
 * @return The current value of the DAC_SPI_ADDR field in the DAC_SPI_ADDR register.
 */
__INLINE uint16_t board_dac_spi_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the DAC_SPI_ADDR field of the DAC_SPI_ADDR register.
 *
 * The DAC_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacspiaddr - The value to set the field to.
 */
__INLINE void board_dac_spi_addr_setf(uint16_t dacspiaddr)
{
    ASSERT_ERR((((uint32_t)dacspiaddr << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_DAC_SPI_ADDR_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)dacspiaddr << 0));
}

/// @}

/**
 * @name DAC_SPI_DATA register definitions
 * <table>
 * <caption>DAC_SPI_DATA bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td> DAC_SPI_RDATA <td> R/W <td> R <td> 0x0
 * <tr><td> 15:00 <td> DAC_SPI_WDATA <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DAC_SPI_DATA register
#define BOARD_DAC_SPI_DATA_ADDR   0x24F10208
/// Offset of the DAC_SPI_DATA register from the base address
#define BOARD_DAC_SPI_DATA_OFFSET 0x00000208
/// Index of the DAC_SPI_DATA register
#define BOARD_DAC_SPI_DATA_INDEX  0x00000082
/// Reset value of the DAC_SPI_DATA register
#define BOARD_DAC_SPI_DATA_RESET  0x00000000

/**
 * @brief Returns the current value of the DAC_SPI_DATA register.
 * The DAC_SPI_DATA register will be read and its value returned.
 * @return The current value of the DAC_SPI_DATA register.
 */
__INLINE uint32_t board_dac_spi_data_get(void)
{
    return REG_PL_RD(BOARD_DAC_SPI_DATA_ADDR);
}

/**
 * @brief Sets the DAC_SPI_DATA register to a value.
 * The DAC_SPI_DATA register will be written.
 * @param value - The value to write.
 */
__INLINE void board_dac_spi_data_set(uint32_t value)
{
    REG_PL_WR(BOARD_DAC_SPI_DATA_ADDR, value);
}

// field definitions
/// DAC_SPI_RDATA field mask
#define BOARD_DAC_SPI_RDATA_MASK   ((uint32_t)0xFFFF0000)
/// DAC_SPI_RDATA field LSB position
#define BOARD_DAC_SPI_RDATA_LSB    16
/// DAC_SPI_RDATA field width
#define BOARD_DAC_SPI_RDATA_WIDTH  ((uint32_t)0x00000010)
/// DAC_SPI_WDATA field mask
#define BOARD_DAC_SPI_WDATA_MASK   ((uint32_t)0x0000FFFF)
/// DAC_SPI_WDATA field LSB position
#define BOARD_DAC_SPI_WDATA_LSB    0
/// DAC_SPI_WDATA field width
#define BOARD_DAC_SPI_WDATA_WIDTH  ((uint32_t)0x00000010)

/// DAC_SPI_RDATA field reset value
#define BOARD_DAC_SPI_RDATA_RST    0x0
/// DAC_SPI_WDATA field reset value
#define BOARD_DAC_SPI_WDATA_RST    0x0

/**
 * @brief Unpacks DAC_SPI_DATA's fields from current value of the DAC_SPI_DATA register.
 *
 * Reads the DAC_SPI_DATA register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dacspirdata - Will be populated with the current value of this field from the register.
 * @param[out] dacspiwdata - Will be populated with the current value of this field from the register.
 */
__INLINE void board_dac_spi_data_unpack(uint16_t* dacspirdata, uint16_t* dacspiwdata)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_DATA_ADDR);

    *dacspirdata = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *dacspiwdata = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the DAC_SPI_RDATA field in the DAC_SPI_DATA register.
 *
 * The DAC_SPI_DATA register will be read and the DAC_SPI_RDATA field's value will be returned.
 *
 * @return The current value of the DAC_SPI_RDATA field in the DAC_SPI_DATA register.
 */
__INLINE uint16_t board_dac_spi_rdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_DATA_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the DAC_SPI_WDATA field in the DAC_SPI_DATA register.
 *
 * The DAC_SPI_DATA register will be read and the DAC_SPI_WDATA field's value will be returned.
 *
 * @return The current value of the DAC_SPI_WDATA field in the DAC_SPI_DATA register.
 */
__INLINE uint16_t board_dac_spi_wdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_DATA_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the DAC_SPI_WDATA field of the DAC_SPI_DATA register.
 *
 * The DAC_SPI_DATA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacspiwdata - The value to set the field to.
 */
__INLINE void board_dac_spi_wdata_setf(uint16_t dacspiwdata)
{
    ASSERT_ERR((((uint32_t)dacspiwdata << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_DATA_ADDR, (uint32_t)dacspiwdata << 0);
}

/// @}

/**
 * @name DAC_SPI_AUTOPOWER register definitions
 * <table>
 * <caption>DAC_SPI_AUTOPOWER bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td>   DAC_POWERUP <td> R <td> R/W <td> 0x0
 * <tr><td> 15:00 <td> DAC_POWERDOWN <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DAC_SPI_AUTOPOWER register
#define BOARD_DAC_SPI_AUTOPOWER_ADDR   0x24F1020C
/// Offset of the DAC_SPI_AUTOPOWER register from the base address
#define BOARD_DAC_SPI_AUTOPOWER_OFFSET 0x0000020C
/// Index of the DAC_SPI_AUTOPOWER register
#define BOARD_DAC_SPI_AUTOPOWER_INDEX  0x00000083
/// Reset value of the DAC_SPI_AUTOPOWER register
#define BOARD_DAC_SPI_AUTOPOWER_RESET  0x00000000

/**
 * @brief Returns the current value of the DAC_SPI_AUTOPOWER register.
 * The DAC_SPI_AUTOPOWER register will be read and its value returned.
 * @return The current value of the DAC_SPI_AUTOPOWER register.
 */
__INLINE uint32_t board_dac_spi_autopower_get(void)
{
    return REG_PL_RD(BOARD_DAC_SPI_AUTOPOWER_ADDR);
}

/**
 * @brief Sets the DAC_SPI_AUTOPOWER register to a value.
 * The DAC_SPI_AUTOPOWER register will be written.
 * @param value - The value to write.
 */
__INLINE void board_dac_spi_autopower_set(uint32_t value)
{
    REG_PL_WR(BOARD_DAC_SPI_AUTOPOWER_ADDR, value);
}

// field definitions
/// DAC_POWERUP field mask
#define BOARD_DAC_POWERUP_MASK     ((uint32_t)0xFFFF0000)
/// DAC_POWERUP field LSB position
#define BOARD_DAC_POWERUP_LSB      16
/// DAC_POWERUP field width
#define BOARD_DAC_POWERUP_WIDTH    ((uint32_t)0x00000010)
/// DAC_POWERDOWN field mask
#define BOARD_DAC_POWERDOWN_MASK   ((uint32_t)0x0000FFFF)
/// DAC_POWERDOWN field LSB position
#define BOARD_DAC_POWERDOWN_LSB    0
/// DAC_POWERDOWN field width
#define BOARD_DAC_POWERDOWN_WIDTH  ((uint32_t)0x00000010)

/// DAC_POWERUP field reset value
#define BOARD_DAC_POWERUP_RST      0x0
/// DAC_POWERDOWN field reset value
#define BOARD_DAC_POWERDOWN_RST    0x0

/**
 * @brief Constructs a value for the DAC_SPI_AUTOPOWER register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dacpowerup - The value to use for the DAC_POWERUP field.
 * @param[in] dacpowerdown - The value to use for the DAC_POWERDOWN field.
 */
__INLINE void board_dac_spi_autopower_pack(uint16_t dacpowerup, uint16_t dacpowerdown)
{
    ASSERT_ERR((((uint32_t)dacpowerup << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)dacpowerdown << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_AUTOPOWER_ADDR,  ((uint32_t)dacpowerup << 16) | ((uint32_t)dacpowerdown << 0));
}

/**
 * @brief Unpacks DAC_SPI_AUTOPOWER's fields from current value of the DAC_SPI_AUTOPOWER register.
 *
 * Reads the DAC_SPI_AUTOPOWER register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dacpowerup - Will be populated with the current value of this field from the register.
 * @param[out] dacpowerdown - Will be populated with the current value of this field from the register.
 */
__INLINE void board_dac_spi_autopower_unpack(uint16_t* dacpowerup, uint16_t* dacpowerdown)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_AUTOPOWER_ADDR);

    *dacpowerup = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *dacpowerdown = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the DAC_POWERUP field in the DAC_SPI_AUTOPOWER register.
 *
 * The DAC_SPI_AUTOPOWER register will be read and the DAC_POWERUP field's value will be returned.
 *
 * @return The current value of the DAC_POWERUP field in the DAC_SPI_AUTOPOWER register.
 */
__INLINE uint16_t board_dac_powerup_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_AUTOPOWER_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Sets the DAC_POWERUP field of the DAC_SPI_AUTOPOWER register.
 *
 * The DAC_SPI_AUTOPOWER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacpowerup - The value to set the field to.
 */
__INLINE void board_dac_powerup_setf(uint16_t dacpowerup)
{
    ASSERT_ERR((((uint32_t)dacpowerup << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_AUTOPOWER_ADDR, (REG_PL_RD(BOARD_DAC_SPI_AUTOPOWER_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)dacpowerup << 16));
}

/**
 * @brief Returns the current value of the DAC_POWERDOWN field in the DAC_SPI_AUTOPOWER register.
 *
 * The DAC_SPI_AUTOPOWER register will be read and the DAC_POWERDOWN field's value will be returned.
 *
 * @return The current value of the DAC_POWERDOWN field in the DAC_SPI_AUTOPOWER register.
 */
__INLINE uint16_t board_dac_powerdown_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DAC_SPI_AUTOPOWER_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the DAC_POWERDOWN field of the DAC_SPI_AUTOPOWER register.
 *
 * The DAC_SPI_AUTOPOWER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dacpowerdown - The value to set the field to.
 */
__INLINE void board_dac_powerdown_setf(uint16_t dacpowerdown)
{
    ASSERT_ERR((((uint32_t)dacpowerdown << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_DAC_SPI_AUTOPOWER_ADDR, (REG_PL_RD(BOARD_DAC_SPI_AUTOPOWER_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)dacpowerdown << 0));
}

/// @}

/**
 * @name DIAG0_STAT register definitions
 * <table>
 * <caption>DIAG0_STAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> DIAG0_STAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAG0_STAT register
#define BOARD_DIAG0_STAT_ADDR   0x24F1004C
/// Offset of the DIAG0_STAT register from the base address
#define BOARD_DIAG0_STAT_OFFSET 0x0000004C
/// Index of the DIAG0_STAT register
#define BOARD_DIAG0_STAT_INDEX  0x00000013
/// Reset value of the DIAG0_STAT register
#define BOARD_DIAG0_STAT_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAG0_STAT register.
 * The DIAG0_STAT register will be read and its value returned.
 * @return The current value of the DIAG0_STAT register.
 */
__INLINE uint32_t board_diag0_stat_get(void)
{
    return REG_PL_RD(BOARD_DIAG0_STAT_ADDR);
}

// field definitions
/// DIAG0_STAT field mask
#define BOARD_DIAG0_STAT_MASK   ((uint32_t)0xFFFFFFFF)
/// DIAG0_STAT field LSB position
#define BOARD_DIAG0_STAT_LSB    0
/// DIAG0_STAT field width
#define BOARD_DIAG0_STAT_WIDTH  ((uint32_t)0x00000020)

/// DIAG0_STAT field reset value
#define BOARD_DIAG0_STAT_RST    0x0

/**
 * @brief Returns the current value of the DIAG0_STAT field in the DIAG0_STAT register.
 *
 * The DIAG0_STAT register will be read and the DIAG0_STAT field's value will be returned.
 *
 * @return The current value of the DIAG0_STAT field in the DIAG0_STAT register.
 */
__INLINE uint32_t board_diag0_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAG0_STAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DIAG1_STAT register definitions
 * <table>
 * <caption>DIAG1_STAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> DIAG1_STAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAG1_STAT register
#define BOARD_DIAG1_STAT_ADDR   0x24F10050
/// Offset of the DIAG1_STAT register from the base address
#define BOARD_DIAG1_STAT_OFFSET 0x00000050
/// Index of the DIAG1_STAT register
#define BOARD_DIAG1_STAT_INDEX  0x00000014
/// Reset value of the DIAG1_STAT register
#define BOARD_DIAG1_STAT_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAG1_STAT register.
 * The DIAG1_STAT register will be read and its value returned.
 * @return The current value of the DIAG1_STAT register.
 */
__INLINE uint32_t board_diag1_stat_get(void)
{
    return REG_PL_RD(BOARD_DIAG1_STAT_ADDR);
}

// field definitions
/// DIAG1_STAT field mask
#define BOARD_DIAG1_STAT_MASK   ((uint32_t)0xFFFFFFFF)
/// DIAG1_STAT field LSB position
#define BOARD_DIAG1_STAT_LSB    0
/// DIAG1_STAT field width
#define BOARD_DIAG1_STAT_WIDTH  ((uint32_t)0x00000020)

/// DIAG1_STAT field reset value
#define BOARD_DIAG1_STAT_RST    0x0

/**
 * @brief Returns the current value of the DIAG1_STAT field in the DIAG1_STAT register.
 *
 * The DIAG1_STAT register will be read and the DIAG1_STAT field's value will be returned.
 *
 * @return The current value of the DIAG1_STAT field in the DIAG1_STAT register.
 */
__INLINE uint32_t board_diag1_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAG1_STAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DIAG2_STAT register definitions
 * <table>
 * <caption>DIAG2_STAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> DIAG2_STAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAG2_STAT register
#define BOARD_DIAG2_STAT_ADDR   0x24F10054
/// Offset of the DIAG2_STAT register from the base address
#define BOARD_DIAG2_STAT_OFFSET 0x00000054
/// Index of the DIAG2_STAT register
#define BOARD_DIAG2_STAT_INDEX  0x00000015
/// Reset value of the DIAG2_STAT register
#define BOARD_DIAG2_STAT_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAG2_STAT register.
 * The DIAG2_STAT register will be read and its value returned.
 * @return The current value of the DIAG2_STAT register.
 */
__INLINE uint32_t board_diag2_stat_get(void)
{
    return REG_PL_RD(BOARD_DIAG2_STAT_ADDR);
}

// field definitions
/// DIAG2_STAT field mask
#define BOARD_DIAG2_STAT_MASK   ((uint32_t)0xFFFFFFFF)
/// DIAG2_STAT field LSB position
#define BOARD_DIAG2_STAT_LSB    0
/// DIAG2_STAT field width
#define BOARD_DIAG2_STAT_WIDTH  ((uint32_t)0x00000020)

/// DIAG2_STAT field reset value
#define BOARD_DIAG2_STAT_RST    0x0

/**
 * @brief Returns the current value of the DIAG2_STAT field in the DIAG2_STAT register.
 *
 * The DIAG2_STAT register will be read and the DIAG2_STAT field's value will be returned.
 *
 * @return The current value of the DIAG2_STAT field in the DIAG2_STAT register.
 */
__INLINE uint32_t board_diag2_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAG2_STAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DIAG3_STAT register definitions
 * <table>
 * <caption>DIAG3_STAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> DIAG3_STAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAG3_STAT register
#define BOARD_DIAG3_STAT_ADDR   0x24F10058
/// Offset of the DIAG3_STAT register from the base address
#define BOARD_DIAG3_STAT_OFFSET 0x00000058
/// Index of the DIAG3_STAT register
#define BOARD_DIAG3_STAT_INDEX  0x00000016
/// Reset value of the DIAG3_STAT register
#define BOARD_DIAG3_STAT_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAG3_STAT register.
 * The DIAG3_STAT register will be read and its value returned.
 * @return The current value of the DIAG3_STAT register.
 */
__INLINE uint32_t board_diag3_stat_get(void)
{
    return REG_PL_RD(BOARD_DIAG3_STAT_ADDR);
}

// field definitions
/// DIAG3_STAT field mask
#define BOARD_DIAG3_STAT_MASK   ((uint32_t)0xFFFFFFFF)
/// DIAG3_STAT field LSB position
#define BOARD_DIAG3_STAT_LSB    0
/// DIAG3_STAT field width
#define BOARD_DIAG3_STAT_WIDTH  ((uint32_t)0x00000020)

/// DIAG3_STAT field reset value
#define BOARD_DIAG3_STAT_RST    0x0

/**
 * @brief Returns the current value of the DIAG3_STAT field in the DIAG3_STAT register.
 *
 * The DIAG3_STAT register will be read and the DIAG3_STAT field's value will be returned.
 *
 * @return The current value of the DIAG3_STAT field in the DIAG3_STAT register.
 */
__INLINE uint32_t board_diag3_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAG3_STAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DIAG4_STAT register definitions
 * <table>
 * <caption>DIAG4_STAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> DIAG4_STAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAG4_STAT register
#define BOARD_DIAG4_STAT_ADDR   0x24F1005C
/// Offset of the DIAG4_STAT register from the base address
#define BOARD_DIAG4_STAT_OFFSET 0x0000005C
/// Index of the DIAG4_STAT register
#define BOARD_DIAG4_STAT_INDEX  0x00000017
/// Reset value of the DIAG4_STAT register
#define BOARD_DIAG4_STAT_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAG4_STAT register.
 * The DIAG4_STAT register will be read and its value returned.
 * @return The current value of the DIAG4_STAT register.
 */
__INLINE uint32_t board_diag4_stat_get(void)
{
    return REG_PL_RD(BOARD_DIAG4_STAT_ADDR);
}

// field definitions
/// DIAG4_STAT field mask
#define BOARD_DIAG4_STAT_MASK   ((uint32_t)0xFFFFFFFF)
/// DIAG4_STAT field LSB position
#define BOARD_DIAG4_STAT_LSB    0
/// DIAG4_STAT field width
#define BOARD_DIAG4_STAT_WIDTH  ((uint32_t)0x00000020)

/// DIAG4_STAT field reset value
#define BOARD_DIAG4_STAT_RST    0x0

/**
 * @brief Returns the current value of the DIAG4_STAT field in the DIAG4_STAT register.
 *
 * The DIAG4_STAT register will be read and the DIAG4_STAT field's value will be returned.
 *
 * @return The current value of the DIAG4_STAT field in the DIAG4_STAT register.
 */
__INLINE uint32_t board_diag4_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_DIAG4_STAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name INTERRUPT_SET register definitions
 * <table>
 * <caption>INTERRUPT_SET bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> DOORBELL_INT_SET <td> R/W <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the INTERRUPT_SET register
#define BOARD_INTERRUPT_SET_ADDR   0x24F10380
/// Offset of the INTERRUPT_SET register from the base address
#define BOARD_INTERRUPT_SET_OFFSET 0x00000380
/// Index of the INTERRUPT_SET register
#define BOARD_INTERRUPT_SET_INDEX  0x000000E0
/// Reset value of the INTERRUPT_SET register
#define BOARD_INTERRUPT_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the INTERRUPT_SET register.
 * The INTERRUPT_SET register will be read and its value returned.
 * @return The current value of the INTERRUPT_SET register.
 */
__INLINE uint32_t board_interrupt_set_get(void)
{
    return REG_PL_RD(BOARD_INTERRUPT_SET_ADDR);
}

/**
 * @brief Sets the INTERRUPT_SET register to a value.
 * The INTERRUPT_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void board_interrupt_set_set(uint32_t value)
{
    REG_PL_WR(BOARD_INTERRUPT_SET_ADDR, value);
}

// field definitions
/// DOORBELL_INT_SET field bit
#define BOARD_DOORBELL_INT_SET_BIT    ((uint32_t)0x00000001)
/// DOORBELL_INT_SET field position
#define BOARD_DOORBELL_INT_SET_POS    0

/// DOORBELL_INT_SET field reset value
#define BOARD_DOORBELL_INT_SET_RST    0x0

/**
 * @brief Returns the current value of the DOORBELL_INT_SET field in the INTERRUPT_SET register.
 *
 * The INTERRUPT_SET register will be read and the DOORBELL_INT_SET field's value will be returned.
 *
 * @return The current value of the DOORBELL_INT_SET field in the INTERRUPT_SET register.
 */
__INLINE uint8_t board_doorbell_int_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_INTERRUPT_SET_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the DOORBELL_INT_SET field of the INTERRUPT_SET register.
 *
 * The INTERRUPT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] doorbellintset - The value to set the field to.
 */
__INLINE void board_doorbell_int_set_setf(uint8_t doorbellintset)
{
    ASSERT_ERR((((uint32_t)doorbellintset << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_INTERRUPT_SET_ADDR, (uint32_t)doorbellintset << 0);
}

/// @}

/**
 * @name INTERRUPT_CLEAR register definitions
 * <table>
 * <caption>INTERRUPT_CLEAR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> DOORBELL_INT_CLEAR <td> R/W <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the INTERRUPT_CLEAR register
#define BOARD_INTERRUPT_CLEAR_ADDR   0x24F10384
/// Offset of the INTERRUPT_CLEAR register from the base address
#define BOARD_INTERRUPT_CLEAR_OFFSET 0x00000384
/// Index of the INTERRUPT_CLEAR register
#define BOARD_INTERRUPT_CLEAR_INDEX  0x000000E1
/// Reset value of the INTERRUPT_CLEAR register
#define BOARD_INTERRUPT_CLEAR_RESET  0x00000000

/**
 * @brief Returns the current value of the INTERRUPT_CLEAR register.
 * The INTERRUPT_CLEAR register will be read and its value returned.
 * @return The current value of the INTERRUPT_CLEAR register.
 */
__INLINE uint32_t board_interrupt_clear_get(void)
{
    return REG_PL_RD(BOARD_INTERRUPT_CLEAR_ADDR);
}

/**
 * @brief Sets the INTERRUPT_CLEAR register to a value.
 * The INTERRUPT_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void board_interrupt_clear_set(uint32_t value)
{
    REG_PL_WR(BOARD_INTERRUPT_CLEAR_ADDR, value);
}

// field definitions
/// DOORBELL_INT_CLEAR field bit
#define BOARD_DOORBELL_INT_CLEAR_BIT    ((uint32_t)0x00000001)
/// DOORBELL_INT_CLEAR field position
#define BOARD_DOORBELL_INT_CLEAR_POS    0

/// DOORBELL_INT_CLEAR field reset value
#define BOARD_DOORBELL_INT_CLEAR_RST    0x0

/**
 * @brief Returns the current value of the DOORBELL_INT_CLEAR field in the INTERRUPT_CLEAR register.
 *
 * The INTERRUPT_CLEAR register will be read and the DOORBELL_INT_CLEAR field's value will be returned.
 *
 * @return The current value of the DOORBELL_INT_CLEAR field in the INTERRUPT_CLEAR register.
 */
__INLINE uint8_t board_doorbell_int_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_INTERRUPT_CLEAR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the DOORBELL_INT_CLEAR field of the INTERRUPT_CLEAR register.
 *
 * The INTERRUPT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] doorbellintclear - The value to set the field to.
 */
__INLINE void board_doorbell_int_clear_setf(uint8_t doorbellintclear)
{
    ASSERT_ERR((((uint32_t)doorbellintclear << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_INTERRUPT_CLEAR_ADDR, (uint32_t)doorbellintclear << 0);
}

/// @}

/**
 * @name INTERRUPT_UNMASK register definitions
 * <table>
 * <caption>INTERRUPT_UNMASK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> DOORBELL_INT_MASK <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the INTERRUPT_UNMASK register
#define BOARD_INTERRUPT_UNMASK_ADDR   0x24F10388
/// Offset of the INTERRUPT_UNMASK register from the base address
#define BOARD_INTERRUPT_UNMASK_OFFSET 0x00000388
/// Index of the INTERRUPT_UNMASK register
#define BOARD_INTERRUPT_UNMASK_INDEX  0x000000E2
/// Reset value of the INTERRUPT_UNMASK register
#define BOARD_INTERRUPT_UNMASK_RESET  0x00000000

/**
 * @brief Returns the current value of the INTERRUPT_UNMASK register.
 * The INTERRUPT_UNMASK register will be read and its value returned.
 * @return The current value of the INTERRUPT_UNMASK register.
 */
__INLINE uint32_t board_interrupt_unmask_get(void)
{
    return REG_PL_RD(BOARD_INTERRUPT_UNMASK_ADDR);
}

/**
 * @brief Sets the INTERRUPT_UNMASK register to a value.
 * The INTERRUPT_UNMASK register will be written.
 * @param value - The value to write.
 */
__INLINE void board_interrupt_unmask_set(uint32_t value)
{
    REG_PL_WR(BOARD_INTERRUPT_UNMASK_ADDR, value);
}

// field definitions
/// DOORBELL_INT_MASK field bit
#define BOARD_DOORBELL_INT_MASK_BIT    ((uint32_t)0x00000001)
/// DOORBELL_INT_MASK field position
#define BOARD_DOORBELL_INT_MASK_POS    0

/// DOORBELL_INT_MASK field reset value
#define BOARD_DOORBELL_INT_MASK_RST    0x0

/**
 * @brief Returns the current value of the DOORBELL_INT_MASK field in the INTERRUPT_UNMASK register.
 *
 * The INTERRUPT_UNMASK register will be read and the DOORBELL_INT_MASK field's value will be returned.
 *
 * @return The current value of the DOORBELL_INT_MASK field in the INTERRUPT_UNMASK register.
 */
__INLINE uint8_t board_doorbell_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_INTERRUPT_UNMASK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the DOORBELL_INT_MASK field of the INTERRUPT_UNMASK register.
 *
 * The INTERRUPT_UNMASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] doorbellintmask - The value to set the field to.
 */
__INLINE void board_doorbell_int_mask_setf(uint8_t doorbellintmask)
{
    ASSERT_ERR((((uint32_t)doorbellintmask << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_INTERRUPT_UNMASK_ADDR, (uint32_t)doorbellintmask << 0);
}

/// @}

/**
 * @name INTERRUPT_STATUS register definitions
 * <table>
 * <caption>INTERRUPT_STATUS bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> DOORBELL_INT_STATUS <td> W <td> R <td> 0
 * </table>
 *
 * @{
 */

/// Address of the INTERRUPT_STATUS register
#define BOARD_INTERRUPT_STATUS_ADDR   0x24F1038C
/// Offset of the INTERRUPT_STATUS register from the base address
#define BOARD_INTERRUPT_STATUS_OFFSET 0x0000038C
/// Index of the INTERRUPT_STATUS register
#define BOARD_INTERRUPT_STATUS_INDEX  0x000000E3
/// Reset value of the INTERRUPT_STATUS register
#define BOARD_INTERRUPT_STATUS_RESET  0x00000000

/**
 * @brief Returns the current value of the INTERRUPT_STATUS register.
 * The INTERRUPT_STATUS register will be read and its value returned.
 * @return The current value of the INTERRUPT_STATUS register.
 */
__INLINE uint32_t board_interrupt_status_get(void)
{
    return REG_PL_RD(BOARD_INTERRUPT_STATUS_ADDR);
}

// field definitions
/// DOORBELL_INT_STATUS field bit
#define BOARD_DOORBELL_INT_STATUS_BIT    ((uint32_t)0x00000001)
/// DOORBELL_INT_STATUS field position
#define BOARD_DOORBELL_INT_STATUS_POS    0

/// DOORBELL_INT_STATUS field reset value
#define BOARD_DOORBELL_INT_STATUS_RST    0x0

/**
 * @brief Returns the current value of the DOORBELL_INT_STATUS field in the INTERRUPT_STATUS register.
 *
 * The INTERRUPT_STATUS register will be read and the DOORBELL_INT_STATUS field's value will be returned.
 *
 * @return The current value of the DOORBELL_INT_STATUS field in the INTERRUPT_STATUS register.
 */
__INLINE uint8_t board_doorbell_int_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_INTERRUPT_STATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name V7_GPIO register definitions
 * <table>
 * <caption>V7_GPIO bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31 <td> V7_GPIO_EN <td> R <td> R/W <td> 0
 * <tr><td> 06 <td>   V7_GPIO6 <td> R <td> R/W <td> 0
 * <tr><td> 05 <td>   V7_GPIO5 <td> R <td> R/W <td> 0
 * <tr><td> 03 <td>   V7_GPIO3 <td> W <td> R <td> 0
 * <tr><td> 02 <td>   V7_GPIO2 <td> W <td> R <td> 0
 * <tr><td> 01 <td>   V7_GPIO1 <td> W <td> R <td> 0
 * <tr><td> 00 <td>   V7_GPIO0 <td> W <td> R <td> 0
 * </table>
 *
 * @{
 */

/// Address of the V7_GPIO register
#define BOARD_V7_GPIO_ADDR   0x24F10390
/// Offset of the V7_GPIO register from the base address
#define BOARD_V7_GPIO_OFFSET 0x00000390
/// Index of the V7_GPIO register
#define BOARD_V7_GPIO_INDEX  0x000000E4
/// Reset value of the V7_GPIO register
#define BOARD_V7_GPIO_RESET  0x00000000

/**
 * @brief Returns the current value of the V7_GPIO register.
 * The V7_GPIO register will be read and its value returned.
 * @return The current value of the V7_GPIO register.
 */
__INLINE uint32_t board_v7_gpio_get(void)
{
    return REG_PL_RD(BOARD_V7_GPIO_ADDR);
}

/**
 * @brief Sets the V7_GPIO register to a value.
 * The V7_GPIO register will be written.
 * @param value - The value to write.
 */
__INLINE void board_v7_gpio_set(uint32_t value)
{
    REG_PL_WR(BOARD_V7_GPIO_ADDR, value);
}

// field definitions
/// V7_GPIO_EN field bit
#define BOARD_V7_GPIO_EN_BIT    ((uint32_t)0x80000000)
/// V7_GPIO_EN field position
#define BOARD_V7_GPIO_EN_POS    31
/// V7_GPIO6 field bit
#define BOARD_V7_GPIO6_BIT      ((uint32_t)0x00000040)
/// V7_GPIO6 field position
#define BOARD_V7_GPIO6_POS      6
/// V7_GPIO5 field bit
#define BOARD_V7_GPIO5_BIT      ((uint32_t)0x00000020)
/// V7_GPIO5 field position
#define BOARD_V7_GPIO5_POS      5
/// V7_GPIO3 field bit
#define BOARD_V7_GPIO3_BIT      ((uint32_t)0x00000008)
/// V7_GPIO3 field position
#define BOARD_V7_GPIO3_POS      3
/// V7_GPIO2 field bit
#define BOARD_V7_GPIO2_BIT      ((uint32_t)0x00000004)
/// V7_GPIO2 field position
#define BOARD_V7_GPIO2_POS      2
/// V7_GPIO1 field bit
#define BOARD_V7_GPIO1_BIT      ((uint32_t)0x00000002)
/// V7_GPIO1 field position
#define BOARD_V7_GPIO1_POS      1
/// V7_GPIO0 field bit
#define BOARD_V7_GPIO0_BIT      ((uint32_t)0x00000001)
/// V7_GPIO0 field position
#define BOARD_V7_GPIO0_POS      0

/// V7_GPIO_EN field reset value
#define BOARD_V7_GPIO_EN_RST    0x0
/// V7_GPIO6 field reset value
#define BOARD_V7_GPIO6_RST      0x0
/// V7_GPIO5 field reset value
#define BOARD_V7_GPIO5_RST      0x0
/// V7_GPIO3 field reset value
#define BOARD_V7_GPIO3_RST      0x0
/// V7_GPIO2 field reset value
#define BOARD_V7_GPIO2_RST      0x0
/// V7_GPIO1 field reset value
#define BOARD_V7_GPIO1_RST      0x0
/// V7_GPIO0 field reset value
#define BOARD_V7_GPIO0_RST      0x0

/**
 * @brief Constructs a value for the V7_GPIO register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] v7gpioen - The value to use for the V7_GPIO_EN field.
 * @param[in] v7gpio6 - The value to use for the V7_GPIO6 field.
 * @param[in] v7gpio5 - The value to use for the V7_GPIO5 field.
 */
__INLINE void board_v7_gpio_pack(uint8_t v7gpioen, uint8_t v7gpio6, uint8_t v7gpio5)
{
    ASSERT_ERR((((uint32_t)v7gpioen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)v7gpio6 << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)v7gpio5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(BOARD_V7_GPIO_ADDR,  ((uint32_t)v7gpioen << 31) | ((uint32_t)v7gpio6 << 6) | ((uint32_t)v7gpio5 << 5));
}

/**
 * @brief Unpacks V7_GPIO's fields from current value of the V7_GPIO register.
 *
 * Reads the V7_GPIO register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] v7gpioen - Will be populated with the current value of this field from the register.
 * @param[out] v7gpio6 - Will be populated with the current value of this field from the register.
 * @param[out] v7gpio5 - Will be populated with the current value of this field from the register.
 * @param[out] v7gpio3 - Will be populated with the current value of this field from the register.
 * @param[out] v7gpio2 - Will be populated with the current value of this field from the register.
 * @param[out] v7gpio1 - Will be populated with the current value of this field from the register.
 * @param[out] v7gpio0 - Will be populated with the current value of this field from the register.
 */
__INLINE void board_v7_gpio_unpack(uint8_t* v7gpioen, uint8_t* v7gpio6, uint8_t* v7gpio5, uint8_t* v7gpio3, uint8_t* v7gpio2, uint8_t* v7gpio1, uint8_t* v7gpio0)
{
    uint32_t localVal = REG_PL_RD(BOARD_V7_GPIO_ADDR);

    *v7gpioen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *v7gpio6 = (localVal & ((uint32_t)0x00000040)) >> 6;
    *v7gpio5 = (localVal & ((uint32_t)0x00000020)) >> 5;
    *v7gpio3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *v7gpio2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *v7gpio1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *v7gpio0 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the V7_GPIO_EN field in the V7_GPIO register.
 *
 * The V7_GPIO register will be read and the V7_GPIO_EN field's value will be returned.
 *
 * @return The current value of the V7_GPIO_EN field in the V7_GPIO register.
 */
__INLINE uint8_t board_v7_gpio_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_V7_GPIO_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the V7_GPIO_EN field of the V7_GPIO register.
 *
 * The V7_GPIO register will be read, modified to contain the new field value, and written.
 *
 * @param[in] v7gpioen - The value to set the field to.
 */
__INLINE void board_v7_gpio_en_setf(uint8_t v7gpioen)
{
    ASSERT_ERR((((uint32_t)v7gpioen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(BOARD_V7_GPIO_ADDR, (REG_PL_RD(BOARD_V7_GPIO_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)v7gpioen << 31));
}

/**
 * @brief Returns the current value of the V7_GPIO6 field in the V7_GPIO register.
 *
 * The V7_GPIO register will be read and the V7_GPIO6 field's value will be returned.
 *
 * @return The current value of the V7_GPIO6 field in the V7_GPIO register.
 */
__INLINE uint8_t board_v7_gpio6_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_V7_GPIO_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the V7_GPIO6 field of the V7_GPIO register.
 *
 * The V7_GPIO register will be read, modified to contain the new field value, and written.
 *
 * @param[in] v7gpio6 - The value to set the field to.
 */
__INLINE void board_v7_gpio6_setf(uint8_t v7gpio6)
{
    ASSERT_ERR((((uint32_t)v7gpio6 << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(BOARD_V7_GPIO_ADDR, (REG_PL_RD(BOARD_V7_GPIO_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)v7gpio6 << 6));
}

/**
 * @brief Returns the current value of the V7_GPIO5 field in the V7_GPIO register.
 *
 * The V7_GPIO register will be read and the V7_GPIO5 field's value will be returned.
 *
 * @return The current value of the V7_GPIO5 field in the V7_GPIO register.
 */
__INLINE uint8_t board_v7_gpio5_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_V7_GPIO_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the V7_GPIO5 field of the V7_GPIO register.
 *
 * The V7_GPIO register will be read, modified to contain the new field value, and written.
 *
 * @param[in] v7gpio5 - The value to set the field to.
 */
__INLINE void board_v7_gpio5_setf(uint8_t v7gpio5)
{
    ASSERT_ERR((((uint32_t)v7gpio5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(BOARD_V7_GPIO_ADDR, (REG_PL_RD(BOARD_V7_GPIO_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)v7gpio5 << 5));
}

/**
 * @brief Returns the current value of the V7_GPIO3 field in the V7_GPIO register.
 *
 * The V7_GPIO register will be read and the V7_GPIO3 field's value will be returned.
 *
 * @return The current value of the V7_GPIO3 field in the V7_GPIO register.
 */
__INLINE uint8_t board_v7_gpio3_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_V7_GPIO_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the V7_GPIO2 field in the V7_GPIO register.
 *
 * The V7_GPIO register will be read and the V7_GPIO2 field's value will be returned.
 *
 * @return The current value of the V7_GPIO2 field in the V7_GPIO register.
 */
__INLINE uint8_t board_v7_gpio2_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_V7_GPIO_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the V7_GPIO1 field in the V7_GPIO register.
 *
 * The V7_GPIO register will be read and the V7_GPIO1 field's value will be returned.
 *
 * @return The current value of the V7_GPIO1 field in the V7_GPIO register.
 */
__INLINE uint8_t board_v7_gpio1_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_V7_GPIO_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the V7_GPIO0 field in the V7_GPIO register.
 *
 * The V7_GPIO register will be read and the V7_GPIO0 field's value will be returned.
 *
 * @return The current value of the V7_GPIO0 field in the V7_GPIO register.
 */
__INLINE uint8_t board_v7_gpio0_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_V7_GPIO_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name IODELAY_CNTL register definitions
 * <table>
 * <caption>IODELAY_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31 <td>  IODELAY_DONE <td> R/W <td> R <td> 0
 * <tr><td> 00 <td> IODELAY_START <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the IODELAY_CNTL register
#define BOARD_IODELAY_CNTL_ADDR   0x24F10400
/// Offset of the IODELAY_CNTL register from the base address
#define BOARD_IODELAY_CNTL_OFFSET 0x00000400
/// Index of the IODELAY_CNTL register
#define BOARD_IODELAY_CNTL_INDEX  0x00000100
/// Reset value of the IODELAY_CNTL register
#define BOARD_IODELAY_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the IODELAY_CNTL register.
 * The IODELAY_CNTL register will be read and its value returned.
 * @return The current value of the IODELAY_CNTL register.
 */
__INLINE uint32_t board_iodelay_cntl_get(void)
{
    return REG_PL_RD(BOARD_IODELAY_CNTL_ADDR);
}

/**
 * @brief Sets the IODELAY_CNTL register to a value.
 * The IODELAY_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iodelay_cntl_set(uint32_t value)
{
    REG_PL_WR(BOARD_IODELAY_CNTL_ADDR, value);
}

// field definitions
/// IODELAY_DONE field bit
#define BOARD_IODELAY_DONE_BIT     ((uint32_t)0x80000000)
/// IODELAY_DONE field position
#define BOARD_IODELAY_DONE_POS     31
/// IODELAY_START field bit
#define BOARD_IODELAY_START_BIT    ((uint32_t)0x00000001)
/// IODELAY_START field position
#define BOARD_IODELAY_START_POS    0

/// IODELAY_DONE field reset value
#define BOARD_IODELAY_DONE_RST     0x0
/// IODELAY_START field reset value
#define BOARD_IODELAY_START_RST    0x0

/**
 * @brief Unpacks IODELAY_CNTL's fields from current value of the IODELAY_CNTL register.
 *
 * Reads the IODELAY_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] iodelaydone - Will be populated with the current value of this field from the register.
 * @param[out] iodelaystart - Will be populated with the current value of this field from the register.
 */
__INLINE void board_iodelay_cntl_unpack(uint8_t* iodelaydone, uint8_t* iodelaystart)
{
    uint32_t localVal = REG_PL_RD(BOARD_IODELAY_CNTL_ADDR);

    *iodelaydone = (localVal & ((uint32_t)0x80000000)) >> 31;
    *iodelaystart = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the IODELAY_DONE field in the IODELAY_CNTL register.
 *
 * The IODELAY_CNTL register will be read and the IODELAY_DONE field's value will be returned.
 *
 * @return The current value of the IODELAY_DONE field in the IODELAY_CNTL register.
 */
__INLINE uint8_t board_iodelay_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IODELAY_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the IODELAY_START field in the IODELAY_CNTL register.
 *
 * The IODELAY_CNTL register will be read and the IODELAY_START field's value will be returned.
 *
 * @return The current value of the IODELAY_START field in the IODELAY_CNTL register.
 */
__INLINE uint8_t board_iodelay_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IODELAY_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the IODELAY_START field of the IODELAY_CNTL register.
 *
 * The IODELAY_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] iodelaystart - The value to set the field to.
 */
__INLINE void board_iodelay_start_setf(uint8_t iodelaystart)
{
    ASSERT_ERR((((uint32_t)iodelaystart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_IODELAY_CNTL_ADDR, (uint32_t)iodelaystart << 0);
}

/// @}

/**
 * @name IODELAY_ADC0 register definitions
 * <table>
 * <caption>IODELAY_ADC0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> IODELAY_ADC0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the IODELAY_ADC0 register
#define BOARD_IODELAY_ADC0_ADDR   0x24F10404
/// Offset of the IODELAY_ADC0 register from the base address
#define BOARD_IODELAY_ADC0_OFFSET 0x00000404
/// Index of the IODELAY_ADC0 register
#define BOARD_IODELAY_ADC0_INDEX  0x00000101
/// Reset value of the IODELAY_ADC0 register
#define BOARD_IODELAY_ADC0_RESET  0x00000000

/**
 * @brief Returns the current value of the IODELAY_ADC0 register.
 * The IODELAY_ADC0 register will be read and its value returned.
 * @return The current value of the IODELAY_ADC0 register.
 */
__INLINE uint32_t board_iodelay_adc0_get(void)
{
    return REG_PL_RD(BOARD_IODELAY_ADC0_ADDR);
}

/**
 * @brief Sets the IODELAY_ADC0 register to a value.
 * The IODELAY_ADC0 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iodelay_adc0_set(uint32_t value)
{
    REG_PL_WR(BOARD_IODELAY_ADC0_ADDR, value);
}

// field definitions
/// IODELAY_ADC0 field mask
#define BOARD_IODELAY_ADC0_MASK   ((uint32_t)0x000000FF)
/// IODELAY_ADC0 field LSB position
#define BOARD_IODELAY_ADC0_LSB    0
/// IODELAY_ADC0 field width
#define BOARD_IODELAY_ADC0_WIDTH  ((uint32_t)0x00000008)

/// IODELAY_ADC0 field reset value
#define BOARD_IODELAY_ADC0_RST    0x0

/**
 * @brief Returns the current value of the IODELAY_ADC0 field in the IODELAY_ADC0 register.
 *
 * The IODELAY_ADC0 register will be read and the IODELAY_ADC0 field's value will be returned.
 *
 * @return The current value of the IODELAY_ADC0 field in the IODELAY_ADC0 register.
 */
__INLINE uint8_t board_iodelay_adc0_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IODELAY_ADC0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the IODELAY_ADC0 field of the IODELAY_ADC0 register.
 *
 * The IODELAY_ADC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] iodelayadc0 - The value to set the field to.
 */
__INLINE void board_iodelay_adc0_setf(uint8_t iodelayadc0)
{
    ASSERT_ERR((((uint32_t)iodelayadc0 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(BOARD_IODELAY_ADC0_ADDR, (uint32_t)iodelayadc0 << 0);
}

/// @}

/**
 * @name IODELAY_DAC0 register definitions
 * <table>
 * <caption>IODELAY_DAC0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> IODELAY_DAC0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the IODELAY_DAC0 register
#define BOARD_IODELAY_DAC0_ADDR   0x24F10408
/// Offset of the IODELAY_DAC0 register from the base address
#define BOARD_IODELAY_DAC0_OFFSET 0x00000408
/// Index of the IODELAY_DAC0 register
#define BOARD_IODELAY_DAC0_INDEX  0x00000102
/// Reset value of the IODELAY_DAC0 register
#define BOARD_IODELAY_DAC0_RESET  0x00000000

/**
 * @brief Returns the current value of the IODELAY_DAC0 register.
 * The IODELAY_DAC0 register will be read and its value returned.
 * @return The current value of the IODELAY_DAC0 register.
 */
__INLINE uint32_t board_iodelay_dac0_get(void)
{
    return REG_PL_RD(BOARD_IODELAY_DAC0_ADDR);
}

/**
 * @brief Sets the IODELAY_DAC0 register to a value.
 * The IODELAY_DAC0 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iodelay_dac0_set(uint32_t value)
{
    REG_PL_WR(BOARD_IODELAY_DAC0_ADDR, value);
}

// field definitions
/// IODELAY_DAC0 field mask
#define BOARD_IODELAY_DAC0_MASK   ((uint32_t)0x000000FF)
/// IODELAY_DAC0 field LSB position
#define BOARD_IODELAY_DAC0_LSB    0
/// IODELAY_DAC0 field width
#define BOARD_IODELAY_DAC0_WIDTH  ((uint32_t)0x00000008)

/// IODELAY_DAC0 field reset value
#define BOARD_IODELAY_DAC0_RST    0x0

/**
 * @brief Returns the current value of the IODELAY_DAC0 field in the IODELAY_DAC0 register.
 *
 * The IODELAY_DAC0 register will be read and the IODELAY_DAC0 field's value will be returned.
 *
 * @return The current value of the IODELAY_DAC0 field in the IODELAY_DAC0 register.
 */
__INLINE uint8_t board_iodelay_dac0_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IODELAY_DAC0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the IODELAY_DAC0 field of the IODELAY_DAC0 register.
 *
 * The IODELAY_DAC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] iodelaydac0 - The value to set the field to.
 */
__INLINE void board_iodelay_dac0_setf(uint8_t iodelaydac0)
{
    ASSERT_ERR((((uint32_t)iodelaydac0 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(BOARD_IODELAY_DAC0_ADDR, (uint32_t)iodelaydac0 << 0);
}

/// @}

/**
 * @name IODELAY_ADC1 register definitions
 * <table>
 * <caption>IODELAY_ADC1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> IODELAY_ADC1 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the IODELAY_ADC1 register
#define BOARD_IODELAY_ADC1_ADDR   0x24F1040C
/// Offset of the IODELAY_ADC1 register from the base address
#define BOARD_IODELAY_ADC1_OFFSET 0x0000040C
/// Index of the IODELAY_ADC1 register
#define BOARD_IODELAY_ADC1_INDEX  0x00000103
/// Reset value of the IODELAY_ADC1 register
#define BOARD_IODELAY_ADC1_RESET  0x00000000

/**
 * @brief Returns the current value of the IODELAY_ADC1 register.
 * The IODELAY_ADC1 register will be read and its value returned.
 * @return The current value of the IODELAY_ADC1 register.
 */
__INLINE uint32_t board_iodelay_adc1_get(void)
{
    return REG_PL_RD(BOARD_IODELAY_ADC1_ADDR);
}

/**
 * @brief Sets the IODELAY_ADC1 register to a value.
 * The IODELAY_ADC1 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iodelay_adc1_set(uint32_t value)
{
    REG_PL_WR(BOARD_IODELAY_ADC1_ADDR, value);
}

// field definitions
/// IODELAY_ADC1 field mask
#define BOARD_IODELAY_ADC1_MASK   ((uint32_t)0x000000FF)
/// IODELAY_ADC1 field LSB position
#define BOARD_IODELAY_ADC1_LSB    0
/// IODELAY_ADC1 field width
#define BOARD_IODELAY_ADC1_WIDTH  ((uint32_t)0x00000008)

/// IODELAY_ADC1 field reset value
#define BOARD_IODELAY_ADC1_RST    0x0

/**
 * @brief Returns the current value of the IODELAY_ADC1 field in the IODELAY_ADC1 register.
 *
 * The IODELAY_ADC1 register will be read and the IODELAY_ADC1 field's value will be returned.
 *
 * @return The current value of the IODELAY_ADC1 field in the IODELAY_ADC1 register.
 */
__INLINE uint8_t board_iodelay_adc1_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IODELAY_ADC1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the IODELAY_ADC1 field of the IODELAY_ADC1 register.
 *
 * The IODELAY_ADC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] iodelayadc1 - The value to set the field to.
 */
__INLINE void board_iodelay_adc1_setf(uint8_t iodelayadc1)
{
    ASSERT_ERR((((uint32_t)iodelayadc1 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(BOARD_IODELAY_ADC1_ADDR, (uint32_t)iodelayadc1 << 0);
}

/// @}

/**
 * @name IODELAY_DAC1 register definitions
 * <table>
 * <caption>IODELAY_DAC1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> IODELAY_DAC1 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the IODELAY_DAC1 register
#define BOARD_IODELAY_DAC1_ADDR   0x24F10410
/// Offset of the IODELAY_DAC1 register from the base address
#define BOARD_IODELAY_DAC1_OFFSET 0x00000410
/// Index of the IODELAY_DAC1 register
#define BOARD_IODELAY_DAC1_INDEX  0x00000104
/// Reset value of the IODELAY_DAC1 register
#define BOARD_IODELAY_DAC1_RESET  0x00000000

/**
 * @brief Returns the current value of the IODELAY_DAC1 register.
 * The IODELAY_DAC1 register will be read and its value returned.
 * @return The current value of the IODELAY_DAC1 register.
 */
__INLINE uint32_t board_iodelay_dac1_get(void)
{
    return REG_PL_RD(BOARD_IODELAY_DAC1_ADDR);
}

/**
 * @brief Sets the IODELAY_DAC1 register to a value.
 * The IODELAY_DAC1 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iodelay_dac1_set(uint32_t value)
{
    REG_PL_WR(BOARD_IODELAY_DAC1_ADDR, value);
}

// field definitions
/// IODELAY_DAC1 field mask
#define BOARD_IODELAY_DAC1_MASK   ((uint32_t)0x000000FF)
/// IODELAY_DAC1 field LSB position
#define BOARD_IODELAY_DAC1_LSB    0
/// IODELAY_DAC1 field width
#define BOARD_IODELAY_DAC1_WIDTH  ((uint32_t)0x00000008)

/// IODELAY_DAC1 field reset value
#define BOARD_IODELAY_DAC1_RST    0x0

/**
 * @brief Returns the current value of the IODELAY_DAC1 field in the IODELAY_DAC1 register.
 *
 * The IODELAY_DAC1 register will be read and the IODELAY_DAC1 field's value will be returned.
 *
 * @return The current value of the IODELAY_DAC1 field in the IODELAY_DAC1 register.
 */
__INLINE uint8_t board_iodelay_dac1_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IODELAY_DAC1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the IODELAY_DAC1 field of the IODELAY_DAC1 register.
 *
 * The IODELAY_DAC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] iodelaydac1 - The value to set the field to.
 */
__INLINE void board_iodelay_dac1_setf(uint8_t iodelaydac1)
{
    ASSERT_ERR((((uint32_t)iodelaydac1 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(BOARD_IODELAY_DAC1_ADDR, (uint32_t)iodelaydac1 << 0);
}

/// @}

/**
 * @name IQCOMP_ADC0 register definitions
 * <table>
 * <caption>IQCOMP_ADC0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> GCOS_ADC0 <td> R <td> R/W <td> 0x100
 * <tr><td> 09:00 <td> GSIN_ADC0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the IQCOMP_ADC0 register
#define BOARD_IQCOMP_ADC0_ADDR   0x24F10480
/// Offset of the IQCOMP_ADC0 register from the base address
#define BOARD_IQCOMP_ADC0_OFFSET 0x00000480
/// Index of the IQCOMP_ADC0 register
#define BOARD_IQCOMP_ADC0_INDEX  0x00000120
/// Reset value of the IQCOMP_ADC0 register
#define BOARD_IQCOMP_ADC0_RESET  0x01000000

/**
 * @brief Returns the current value of the IQCOMP_ADC0 register.
 * The IQCOMP_ADC0 register will be read and its value returned.
 * @return The current value of the IQCOMP_ADC0 register.
 */
__INLINE uint32_t board_iqcomp_adc0_get(void)
{
    return REG_PL_RD(BOARD_IQCOMP_ADC0_ADDR);
}

/**
 * @brief Sets the IQCOMP_ADC0 register to a value.
 * The IQCOMP_ADC0 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iqcomp_adc0_set(uint32_t value)
{
    REG_PL_WR(BOARD_IQCOMP_ADC0_ADDR, value);
}

// field definitions
/// GCOS_ADC0 field mask
#define BOARD_GCOS_ADC0_MASK   ((uint32_t)0x03FF0000)
/// GCOS_ADC0 field LSB position
#define BOARD_GCOS_ADC0_LSB    16
/// GCOS_ADC0 field width
#define BOARD_GCOS_ADC0_WIDTH  ((uint32_t)0x0000000A)
/// GSIN_ADC0 field mask
#define BOARD_GSIN_ADC0_MASK   ((uint32_t)0x000003FF)
/// GSIN_ADC0 field LSB position
#define BOARD_GSIN_ADC0_LSB    0
/// GSIN_ADC0 field width
#define BOARD_GSIN_ADC0_WIDTH  ((uint32_t)0x0000000A)

/// GCOS_ADC0 field reset value
#define BOARD_GCOS_ADC0_RST    0x100
/// GSIN_ADC0 field reset value
#define BOARD_GSIN_ADC0_RST    0x0

/**
 * @brief Constructs a value for the IQCOMP_ADC0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] gcosadc0 - The value to use for the GCOS_ADC0 field.
 * @param[in] gsinadc0 - The value to use for the GSIN_ADC0 field.
 */
__INLINE void board_iqcomp_adc0_pack(uint16_t gcosadc0, uint16_t gsinadc0)
{
    ASSERT_ERR((((uint32_t)gcosadc0 << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)gsinadc0 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(BOARD_IQCOMP_ADC0_ADDR,  ((uint32_t)gcosadc0 << 16) | ((uint32_t)gsinadc0 << 0));
}

/**
 * @brief Unpacks IQCOMP_ADC0's fields from current value of the IQCOMP_ADC0 register.
 *
 * Reads the IQCOMP_ADC0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] gcosadc0 - Will be populated with the current value of this field from the register.
 * @param[out] gsinadc0 - Will be populated with the current value of this field from the register.
 */
__INLINE void board_iqcomp_adc0_unpack(uint16_t* gcosadc0, uint16_t* gsinadc0)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_ADC0_ADDR);

    *gcosadc0 = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *gsinadc0 = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the GCOS_ADC0 field in the IQCOMP_ADC0 register.
 *
 * The IQCOMP_ADC0 register will be read and the GCOS_ADC0 field's value will be returned.
 *
 * @return The current value of the GCOS_ADC0 field in the IQCOMP_ADC0 register.
 */
__INLINE uint16_t board_gcos_adc0_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_ADC0_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the GCOS_ADC0 field of the IQCOMP_ADC0 register.
 *
 * The IQCOMP_ADC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gcosadc0 - The value to set the field to.
 */
__INLINE void board_gcos_adc0_setf(uint16_t gcosadc0)
{
    ASSERT_ERR((((uint32_t)gcosadc0 << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(BOARD_IQCOMP_ADC0_ADDR, (REG_PL_RD(BOARD_IQCOMP_ADC0_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)gcosadc0 << 16));
}

/**
 * @brief Returns the current value of the GSIN_ADC0 field in the IQCOMP_ADC0 register.
 *
 * The IQCOMP_ADC0 register will be read and the GSIN_ADC0 field's value will be returned.
 *
 * @return The current value of the GSIN_ADC0 field in the IQCOMP_ADC0 register.
 */
__INLINE uint16_t board_gsin_adc0_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_ADC0_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the GSIN_ADC0 field of the IQCOMP_ADC0 register.
 *
 * The IQCOMP_ADC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gsinadc0 - The value to set the field to.
 */
__INLINE void board_gsin_adc0_setf(uint16_t gsinadc0)
{
    ASSERT_ERR((((uint32_t)gsinadc0 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(BOARD_IQCOMP_ADC0_ADDR, (REG_PL_RD(BOARD_IQCOMP_ADC0_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)gsinadc0 << 0));
}

/// @}

/**
 * @name IQCOMP_DAC0 register definitions
 * <table>
 * <caption>IQCOMP_DAC0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> GCOS_DAC0 <td> R <td> R/W <td> 0x100
 * <tr><td> 09:00 <td> GSIN_DAC0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the IQCOMP_DAC0 register
#define BOARD_IQCOMP_DAC0_ADDR   0x24F10484
/// Offset of the IQCOMP_DAC0 register from the base address
#define BOARD_IQCOMP_DAC0_OFFSET 0x00000484
/// Index of the IQCOMP_DAC0 register
#define BOARD_IQCOMP_DAC0_INDEX  0x00000121
/// Reset value of the IQCOMP_DAC0 register
#define BOARD_IQCOMP_DAC0_RESET  0x01000000

/**
 * @brief Returns the current value of the IQCOMP_DAC0 register.
 * The IQCOMP_DAC0 register will be read and its value returned.
 * @return The current value of the IQCOMP_DAC0 register.
 */
__INLINE uint32_t board_iqcomp_dac0_get(void)
{
    return REG_PL_RD(BOARD_IQCOMP_DAC0_ADDR);
}

/**
 * @brief Sets the IQCOMP_DAC0 register to a value.
 * The IQCOMP_DAC0 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iqcomp_dac0_set(uint32_t value)
{
    REG_PL_WR(BOARD_IQCOMP_DAC0_ADDR, value);
}

// field definitions
/// GCOS_DAC0 field mask
#define BOARD_GCOS_DAC0_MASK   ((uint32_t)0x03FF0000)
/// GCOS_DAC0 field LSB position
#define BOARD_GCOS_DAC0_LSB    16
/// GCOS_DAC0 field width
#define BOARD_GCOS_DAC0_WIDTH  ((uint32_t)0x0000000A)
/// GSIN_DAC0 field mask
#define BOARD_GSIN_DAC0_MASK   ((uint32_t)0x000003FF)
/// GSIN_DAC0 field LSB position
#define BOARD_GSIN_DAC0_LSB    0
/// GSIN_DAC0 field width
#define BOARD_GSIN_DAC0_WIDTH  ((uint32_t)0x0000000A)

/// GCOS_DAC0 field reset value
#define BOARD_GCOS_DAC0_RST    0x100
/// GSIN_DAC0 field reset value
#define BOARD_GSIN_DAC0_RST    0x0

/**
 * @brief Constructs a value for the IQCOMP_DAC0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] gcosdac0 - The value to use for the GCOS_DAC0 field.
 * @param[in] gsindac0 - The value to use for the GSIN_DAC0 field.
 */
__INLINE void board_iqcomp_dac0_pack(uint16_t gcosdac0, uint16_t gsindac0)
{
    ASSERT_ERR((((uint32_t)gcosdac0 << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)gsindac0 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(BOARD_IQCOMP_DAC0_ADDR,  ((uint32_t)gcosdac0 << 16) | ((uint32_t)gsindac0 << 0));
}

/**
 * @brief Unpacks IQCOMP_DAC0's fields from current value of the IQCOMP_DAC0 register.
 *
 * Reads the IQCOMP_DAC0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] gcosdac0 - Will be populated with the current value of this field from the register.
 * @param[out] gsindac0 - Will be populated with the current value of this field from the register.
 */
__INLINE void board_iqcomp_dac0_unpack(uint16_t* gcosdac0, uint16_t* gsindac0)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_DAC0_ADDR);

    *gcosdac0 = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *gsindac0 = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the GCOS_DAC0 field in the IQCOMP_DAC0 register.
 *
 * The IQCOMP_DAC0 register will be read and the GCOS_DAC0 field's value will be returned.
 *
 * @return The current value of the GCOS_DAC0 field in the IQCOMP_DAC0 register.
 */
__INLINE uint16_t board_gcos_dac0_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_DAC0_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the GCOS_DAC0 field of the IQCOMP_DAC0 register.
 *
 * The IQCOMP_DAC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gcosdac0 - The value to set the field to.
 */
__INLINE void board_gcos_dac0_setf(uint16_t gcosdac0)
{
    ASSERT_ERR((((uint32_t)gcosdac0 << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(BOARD_IQCOMP_DAC0_ADDR, (REG_PL_RD(BOARD_IQCOMP_DAC0_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)gcosdac0 << 16));
}

/**
 * @brief Returns the current value of the GSIN_DAC0 field in the IQCOMP_DAC0 register.
 *
 * The IQCOMP_DAC0 register will be read and the GSIN_DAC0 field's value will be returned.
 *
 * @return The current value of the GSIN_DAC0 field in the IQCOMP_DAC0 register.
 */
__INLINE uint16_t board_gsin_dac0_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_DAC0_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the GSIN_DAC0 field of the IQCOMP_DAC0 register.
 *
 * The IQCOMP_DAC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gsindac0 - The value to set the field to.
 */
__INLINE void board_gsin_dac0_setf(uint16_t gsindac0)
{
    ASSERT_ERR((((uint32_t)gsindac0 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(BOARD_IQCOMP_DAC0_ADDR, (REG_PL_RD(BOARD_IQCOMP_DAC0_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)gsindac0 << 0));
}

/// @}

/**
 * @name IQCOMP_ADC1 register definitions
 * <table>
 * <caption>IQCOMP_ADC1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> GCOS_ADC1 <td> R <td> R/W <td> 0x100
 * <tr><td> 09:00 <td> GSIN_ADC1 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the IQCOMP_ADC1 register
#define BOARD_IQCOMP_ADC1_ADDR   0x24F10488
/// Offset of the IQCOMP_ADC1 register from the base address
#define BOARD_IQCOMP_ADC1_OFFSET 0x00000488
/// Index of the IQCOMP_ADC1 register
#define BOARD_IQCOMP_ADC1_INDEX  0x00000122
/// Reset value of the IQCOMP_ADC1 register
#define BOARD_IQCOMP_ADC1_RESET  0x01000000

/**
 * @brief Returns the current value of the IQCOMP_ADC1 register.
 * The IQCOMP_ADC1 register will be read and its value returned.
 * @return The current value of the IQCOMP_ADC1 register.
 */
__INLINE uint32_t board_iqcomp_adc1_get(void)
{
    return REG_PL_RD(BOARD_IQCOMP_ADC1_ADDR);
}

/**
 * @brief Sets the IQCOMP_ADC1 register to a value.
 * The IQCOMP_ADC1 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iqcomp_adc1_set(uint32_t value)
{
    REG_PL_WR(BOARD_IQCOMP_ADC1_ADDR, value);
}

// field definitions
/// GCOS_ADC1 field mask
#define BOARD_GCOS_ADC1_MASK   ((uint32_t)0x03FF0000)
/// GCOS_ADC1 field LSB position
#define BOARD_GCOS_ADC1_LSB    16
/// GCOS_ADC1 field width
#define BOARD_GCOS_ADC1_WIDTH  ((uint32_t)0x0000000A)
/// GSIN_ADC1 field mask
#define BOARD_GSIN_ADC1_MASK   ((uint32_t)0x000003FF)
/// GSIN_ADC1 field LSB position
#define BOARD_GSIN_ADC1_LSB    0
/// GSIN_ADC1 field width
#define BOARD_GSIN_ADC1_WIDTH  ((uint32_t)0x0000000A)

/// GCOS_ADC1 field reset value
#define BOARD_GCOS_ADC1_RST    0x100
/// GSIN_ADC1 field reset value
#define BOARD_GSIN_ADC1_RST    0x0

/**
 * @brief Constructs a value for the IQCOMP_ADC1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] gcosadc1 - The value to use for the GCOS_ADC1 field.
 * @param[in] gsinadc1 - The value to use for the GSIN_ADC1 field.
 */
__INLINE void board_iqcomp_adc1_pack(uint16_t gcosadc1, uint16_t gsinadc1)
{
    ASSERT_ERR((((uint32_t)gcosadc1 << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)gsinadc1 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(BOARD_IQCOMP_ADC1_ADDR,  ((uint32_t)gcosadc1 << 16) | ((uint32_t)gsinadc1 << 0));
}

/**
 * @brief Unpacks IQCOMP_ADC1's fields from current value of the IQCOMP_ADC1 register.
 *
 * Reads the IQCOMP_ADC1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] gcosadc1 - Will be populated with the current value of this field from the register.
 * @param[out] gsinadc1 - Will be populated with the current value of this field from the register.
 */
__INLINE void board_iqcomp_adc1_unpack(uint16_t* gcosadc1, uint16_t* gsinadc1)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_ADC1_ADDR);

    *gcosadc1 = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *gsinadc1 = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the GCOS_ADC1 field in the IQCOMP_ADC1 register.
 *
 * The IQCOMP_ADC1 register will be read and the GCOS_ADC1 field's value will be returned.
 *
 * @return The current value of the GCOS_ADC1 field in the IQCOMP_ADC1 register.
 */
__INLINE uint16_t board_gcos_adc1_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_ADC1_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the GCOS_ADC1 field of the IQCOMP_ADC1 register.
 *
 * The IQCOMP_ADC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gcosadc1 - The value to set the field to.
 */
__INLINE void board_gcos_adc1_setf(uint16_t gcosadc1)
{
    ASSERT_ERR((((uint32_t)gcosadc1 << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(BOARD_IQCOMP_ADC1_ADDR, (REG_PL_RD(BOARD_IQCOMP_ADC1_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)gcosadc1 << 16));
}

/**
 * @brief Returns the current value of the GSIN_ADC1 field in the IQCOMP_ADC1 register.
 *
 * The IQCOMP_ADC1 register will be read and the GSIN_ADC1 field's value will be returned.
 *
 * @return The current value of the GSIN_ADC1 field in the IQCOMP_ADC1 register.
 */
__INLINE uint16_t board_gsin_adc1_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_ADC1_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the GSIN_ADC1 field of the IQCOMP_ADC1 register.
 *
 * The IQCOMP_ADC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gsinadc1 - The value to set the field to.
 */
__INLINE void board_gsin_adc1_setf(uint16_t gsinadc1)
{
    ASSERT_ERR((((uint32_t)gsinadc1 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(BOARD_IQCOMP_ADC1_ADDR, (REG_PL_RD(BOARD_IQCOMP_ADC1_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)gsinadc1 << 0));
}

/// @}

/**
 * @name IQCOMP_DAC1 register definitions
 * <table>
 * <caption>IQCOMP_DAC1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> GCOS_DAC1 <td> R <td> R/W <td> 0x100
 * <tr><td> 09:00 <td> GSIN_DAC1 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the IQCOMP_DAC1 register
#define BOARD_IQCOMP_DAC1_ADDR   0x24F1048C
/// Offset of the IQCOMP_DAC1 register from the base address
#define BOARD_IQCOMP_DAC1_OFFSET 0x0000048C
/// Index of the IQCOMP_DAC1 register
#define BOARD_IQCOMP_DAC1_INDEX  0x00000123
/// Reset value of the IQCOMP_DAC1 register
#define BOARD_IQCOMP_DAC1_RESET  0x01000000

/**
 * @brief Returns the current value of the IQCOMP_DAC1 register.
 * The IQCOMP_DAC1 register will be read and its value returned.
 * @return The current value of the IQCOMP_DAC1 register.
 */
__INLINE uint32_t board_iqcomp_dac1_get(void)
{
    return REG_PL_RD(BOARD_IQCOMP_DAC1_ADDR);
}

/**
 * @brief Sets the IQCOMP_DAC1 register to a value.
 * The IQCOMP_DAC1 register will be written.
 * @param value - The value to write.
 */
__INLINE void board_iqcomp_dac1_set(uint32_t value)
{
    REG_PL_WR(BOARD_IQCOMP_DAC1_ADDR, value);
}

// field definitions
/// GCOS_DAC1 field mask
#define BOARD_GCOS_DAC1_MASK   ((uint32_t)0x03FF0000)
/// GCOS_DAC1 field LSB position
#define BOARD_GCOS_DAC1_LSB    16
/// GCOS_DAC1 field width
#define BOARD_GCOS_DAC1_WIDTH  ((uint32_t)0x0000000A)
/// GSIN_DAC1 field mask
#define BOARD_GSIN_DAC1_MASK   ((uint32_t)0x000003FF)
/// GSIN_DAC1 field LSB position
#define BOARD_GSIN_DAC1_LSB    0
/// GSIN_DAC1 field width
#define BOARD_GSIN_DAC1_WIDTH  ((uint32_t)0x0000000A)

/// GCOS_DAC1 field reset value
#define BOARD_GCOS_DAC1_RST    0x100
/// GSIN_DAC1 field reset value
#define BOARD_GSIN_DAC1_RST    0x0

/**
 * @brief Constructs a value for the IQCOMP_DAC1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] gcosdac1 - The value to use for the GCOS_DAC1 field.
 * @param[in] gsindac1 - The value to use for the GSIN_DAC1 field.
 */
__INLINE void board_iqcomp_dac1_pack(uint16_t gcosdac1, uint16_t gsindac1)
{
    ASSERT_ERR((((uint32_t)gcosdac1 << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)gsindac1 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(BOARD_IQCOMP_DAC1_ADDR,  ((uint32_t)gcosdac1 << 16) | ((uint32_t)gsindac1 << 0));
}

/**
 * @brief Unpacks IQCOMP_DAC1's fields from current value of the IQCOMP_DAC1 register.
 *
 * Reads the IQCOMP_DAC1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] gcosdac1 - Will be populated with the current value of this field from the register.
 * @param[out] gsindac1 - Will be populated with the current value of this field from the register.
 */
__INLINE void board_iqcomp_dac1_unpack(uint16_t* gcosdac1, uint16_t* gsindac1)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_DAC1_ADDR);

    *gcosdac1 = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *gsindac1 = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the GCOS_DAC1 field in the IQCOMP_DAC1 register.
 *
 * The IQCOMP_DAC1 register will be read and the GCOS_DAC1 field's value will be returned.
 *
 * @return The current value of the GCOS_DAC1 field in the IQCOMP_DAC1 register.
 */
__INLINE uint16_t board_gcos_dac1_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_DAC1_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the GCOS_DAC1 field of the IQCOMP_DAC1 register.
 *
 * The IQCOMP_DAC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gcosdac1 - The value to set the field to.
 */
__INLINE void board_gcos_dac1_setf(uint16_t gcosdac1)
{
    ASSERT_ERR((((uint32_t)gcosdac1 << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(BOARD_IQCOMP_DAC1_ADDR, (REG_PL_RD(BOARD_IQCOMP_DAC1_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)gcosdac1 << 16));
}

/**
 * @brief Returns the current value of the GSIN_DAC1 field in the IQCOMP_DAC1 register.
 *
 * The IQCOMP_DAC1 register will be read and the GSIN_DAC1 field's value will be returned.
 *
 * @return The current value of the GSIN_DAC1 field in the IQCOMP_DAC1 register.
 */
__INLINE uint16_t board_gsin_dac1_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_IQCOMP_DAC1_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the GSIN_DAC1 field of the IQCOMP_DAC1 register.
 *
 * The IQCOMP_DAC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gsindac1 - The value to set the field to.
 */
__INLINE void board_gsin_dac1_setf(uint16_t gsindac1)
{
    ASSERT_ERR((((uint32_t)gsindac1 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(BOARD_IQCOMP_DAC1_ADDR, (REG_PL_RD(BOARD_IQCOMP_DAC1_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)gsindac1 << 0));
}

/// @}

/**
 * @name RF_CNTRL register definitions
 * <table>
 * <caption>RF_CNTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>      RF_SPI_DONE <td> R/W <td> R <td> 0
 * <tr><td> 11:08 <td> RF_SPI_PRESCALER <td> R <td> R/W <td> 0x0
 * <tr><td> 00    <td>     RF_SPI_START <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the RF_CNTRL register
#define BOARD_RF_CNTRL_ADDR   0x24F10500
/// Offset of the RF_CNTRL register from the base address
#define BOARD_RF_CNTRL_OFFSET 0x00000500
/// Index of the RF_CNTRL register
#define BOARD_RF_CNTRL_INDEX  0x00000140
/// Reset value of the RF_CNTRL register
#define BOARD_RF_CNTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the RF_CNTRL register.
 * The RF_CNTRL register will be read and its value returned.
 * @return The current value of the RF_CNTRL register.
 */
__INLINE uint32_t board_rf_cntrl_get(void)
{
    return REG_PL_RD(BOARD_RF_CNTRL_ADDR);
}

/**
 * @brief Sets the RF_CNTRL register to a value.
 * The RF_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void board_rf_cntrl_set(uint32_t value)
{
    REG_PL_WR(BOARD_RF_CNTRL_ADDR, value);
}

// field definitions
/// RF_SPI_DONE field bit
#define BOARD_RF_SPI_DONE_BIT         ((uint32_t)0x80000000)
/// RF_SPI_DONE field position
#define BOARD_RF_SPI_DONE_POS         31
/// RF_SPI_PRESCALER field mask
#define BOARD_RF_SPI_PRESCALER_MASK   ((uint32_t)0x00000F00)
/// RF_SPI_PRESCALER field LSB position
#define BOARD_RF_SPI_PRESCALER_LSB    8
/// RF_SPI_PRESCALER field width
#define BOARD_RF_SPI_PRESCALER_WIDTH  ((uint32_t)0x00000004)
/// RF_SPI_START field bit
#define BOARD_RF_SPI_START_BIT        ((uint32_t)0x00000001)
/// RF_SPI_START field position
#define BOARD_RF_SPI_START_POS        0

/// RF_SPI_DONE field reset value
#define BOARD_RF_SPI_DONE_RST         0x0
/// RF_SPI_PRESCALER field reset value
#define BOARD_RF_SPI_PRESCALER_RST    0x0
/// RF_SPI_START field reset value
#define BOARD_RF_SPI_START_RST        0x0

/**
 * @brief Constructs a value for the RF_CNTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rfspiprescaler - The value to use for the RF_SPI_PRESCALER field.
 * @param[in] rfspistart - The value to use for the RF_SPI_START field.
 */
__INLINE void board_rf_cntrl_pack(uint8_t rfspiprescaler, uint8_t rfspistart)
{
    ASSERT_ERR((((uint32_t)rfspiprescaler << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)rfspistart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_RF_CNTRL_ADDR,  ((uint32_t)rfspiprescaler << 8) | ((uint32_t)rfspistart << 0));
}

/**
 * @brief Unpacks RF_CNTRL's fields from current value of the RF_CNTRL register.
 *
 * Reads the RF_CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rfspidone - Will be populated with the current value of this field from the register.
 * @param[out] rfspiprescaler - Will be populated with the current value of this field from the register.
 * @param[out] rfspistart - Will be populated with the current value of this field from the register.
 */
__INLINE void board_rf_cntrl_unpack(uint8_t* rfspidone, uint8_t* rfspiprescaler, uint8_t* rfspistart)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_CNTRL_ADDR);

    *rfspidone = (localVal & ((uint32_t)0x80000000)) >> 31;
    *rfspiprescaler = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *rfspistart = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RF_SPI_DONE field in the RF_CNTRL register.
 *
 * The RF_CNTRL register will be read and the RF_SPI_DONE field's value will be returned.
 *
 * @return The current value of the RF_SPI_DONE field in the RF_CNTRL register.
 */
__INLINE uint8_t board_rf_spi_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the RF_SPI_PRESCALER field in the RF_CNTRL register.
 *
 * The RF_CNTRL register will be read and the RF_SPI_PRESCALER field's value will be returned.
 *
 * @return The current value of the RF_SPI_PRESCALER field in the RF_CNTRL register.
 */
__INLINE uint8_t board_rf_spi_prescaler_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the RF_SPI_PRESCALER field of the RF_CNTRL register.
 *
 * The RF_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfspiprescaler - The value to set the field to.
 */
__INLINE void board_rf_spi_prescaler_setf(uint8_t rfspiprescaler)
{
    ASSERT_ERR((((uint32_t)rfspiprescaler << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(BOARD_RF_CNTRL_ADDR, (REG_PL_RD(BOARD_RF_CNTRL_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)rfspiprescaler << 8));
}

/**
 * @brief Returns the current value of the RF_SPI_START field in the RF_CNTRL register.
 *
 * The RF_CNTRL register will be read and the RF_SPI_START field's value will be returned.
 *
 * @return The current value of the RF_SPI_START field in the RF_CNTRL register.
 */
__INLINE uint8_t board_rf_spi_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the RF_SPI_START field of the RF_CNTRL register.
 *
 * The RF_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfspistart - The value to set the field to.
 */
__INLINE void board_rf_spi_start_setf(uint8_t rfspistart)
{
    ASSERT_ERR((((uint32_t)rfspistart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(BOARD_RF_CNTRL_ADDR, (REG_PL_RD(BOARD_RF_CNTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rfspistart << 0));
}

/// @}

/**
 * @name RF_SPI_ADDR register definitions
 * <table>
 * <caption>RF_SPI_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>   RF_SPI_WE <td> R <td> R/W <td> 0
 * <tr><td> 19:16 <td>  RF_SPI_SEL <td> R <td> R/W <td> 0x0
 * <tr><td> 15:00 <td> RF_SPI_ADDR <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RF_SPI_ADDR register
#define BOARD_RF_SPI_ADDR_ADDR   0x24F10504
/// Offset of the RF_SPI_ADDR register from the base address
#define BOARD_RF_SPI_ADDR_OFFSET 0x00000504
/// Index of the RF_SPI_ADDR register
#define BOARD_RF_SPI_ADDR_INDEX  0x00000141
/// Reset value of the RF_SPI_ADDR register
#define BOARD_RF_SPI_ADDR_RESET  0x00000000

/**
 * @brief Returns the current value of the RF_SPI_ADDR register.
 * The RF_SPI_ADDR register will be read and its value returned.
 * @return The current value of the RF_SPI_ADDR register.
 */
__INLINE uint32_t board_rf_spi_addr_get(void)
{
    return REG_PL_RD(BOARD_RF_SPI_ADDR_ADDR);
}

/**
 * @brief Sets the RF_SPI_ADDR register to a value.
 * The RF_SPI_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void board_rf_spi_addr_set(uint32_t value)
{
    REG_PL_WR(BOARD_RF_SPI_ADDR_ADDR, value);
}

// field definitions
/// RF_SPI_WE field bit
#define BOARD_RF_SPI_WE_BIT      ((uint32_t)0x80000000)
/// RF_SPI_WE field position
#define BOARD_RF_SPI_WE_POS      31
/// RF_SPI_SEL field mask
#define BOARD_RF_SPI_SEL_MASK    ((uint32_t)0x000F0000)
/// RF_SPI_SEL field LSB position
#define BOARD_RF_SPI_SEL_LSB     16
/// RF_SPI_SEL field width
#define BOARD_RF_SPI_SEL_WIDTH   ((uint32_t)0x00000004)
/// RF_SPI_ADDR field mask
#define BOARD_RF_SPI_ADDR_MASK   ((uint32_t)0x0000FFFF)
/// RF_SPI_ADDR field LSB position
#define BOARD_RF_SPI_ADDR_LSB    0
/// RF_SPI_ADDR field width
#define BOARD_RF_SPI_ADDR_WIDTH  ((uint32_t)0x00000010)

/// RF_SPI_WE field reset value
#define BOARD_RF_SPI_WE_RST      0x0
/// RF_SPI_SEL field reset value
#define BOARD_RF_SPI_SEL_RST     0x0
/// RF_SPI_ADDR field reset value
#define BOARD_RF_SPI_ADDR_RST    0x0

/**
 * @brief Constructs a value for the RF_SPI_ADDR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rfspiwe - The value to use for the RF_SPI_WE field.
 * @param[in] rfspisel - The value to use for the RF_SPI_SEL field.
 * @param[in] rfspiaddr - The value to use for the RF_SPI_ADDR field.
 */
__INLINE void board_rf_spi_addr_pack(uint8_t rfspiwe, uint8_t rfspisel, uint16_t rfspiaddr)
{
    ASSERT_ERR((((uint32_t)rfspiwe << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)rfspisel << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)rfspiaddr << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_RF_SPI_ADDR_ADDR,  ((uint32_t)rfspiwe << 31) | ((uint32_t)rfspisel << 16) | ((uint32_t)rfspiaddr << 0));
}

/**
 * @brief Unpacks RF_SPI_ADDR's fields from current value of the RF_SPI_ADDR register.
 *
 * Reads the RF_SPI_ADDR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rfspiwe - Will be populated with the current value of this field from the register.
 * @param[out] rfspisel - Will be populated with the current value of this field from the register.
 * @param[out] rfspiaddr - Will be populated with the current value of this field from the register.
 */
__INLINE void board_rf_spi_addr_unpack(uint8_t* rfspiwe, uint8_t* rfspisel, uint16_t* rfspiaddr)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_SPI_ADDR_ADDR);

    *rfspiwe = (localVal & ((uint32_t)0x80000000)) >> 31;
    *rfspisel = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *rfspiaddr = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the RF_SPI_WE field in the RF_SPI_ADDR register.
 *
 * The RF_SPI_ADDR register will be read and the RF_SPI_WE field's value will be returned.
 *
 * @return The current value of the RF_SPI_WE field in the RF_SPI_ADDR register.
 */
__INLINE uint8_t board_rf_spi_we_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the RF_SPI_WE field of the RF_SPI_ADDR register.
 *
 * The RF_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfspiwe - The value to set the field to.
 */
__INLINE void board_rf_spi_we_setf(uint8_t rfspiwe)
{
    ASSERT_ERR((((uint32_t)rfspiwe << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(BOARD_RF_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_RF_SPI_ADDR_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)rfspiwe << 31));
}

/**
 * @brief Returns the current value of the RF_SPI_SEL field in the RF_SPI_ADDR register.
 *
 * The RF_SPI_ADDR register will be read and the RF_SPI_SEL field's value will be returned.
 *
 * @return The current value of the RF_SPI_SEL field in the RF_SPI_ADDR register.
 */
__INLINE uint8_t board_rf_spi_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the RF_SPI_SEL field of the RF_SPI_ADDR register.
 *
 * The RF_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfspisel - The value to set the field to.
 */
__INLINE void board_rf_spi_sel_setf(uint8_t rfspisel)
{
    ASSERT_ERR((((uint32_t)rfspisel << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(BOARD_RF_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_RF_SPI_ADDR_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)rfspisel << 16));
}

/**
 * @brief Returns the current value of the RF_SPI_ADDR field in the RF_SPI_ADDR register.
 *
 * The RF_SPI_ADDR register will be read and the RF_SPI_ADDR field's value will be returned.
 *
 * @return The current value of the RF_SPI_ADDR field in the RF_SPI_ADDR register.
 */
__INLINE uint16_t board_rf_spi_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the RF_SPI_ADDR field of the RF_SPI_ADDR register.
 *
 * The RF_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfspiaddr - The value to set the field to.
 */
__INLINE void board_rf_spi_addr_setf(uint16_t rfspiaddr)
{
    ASSERT_ERR((((uint32_t)rfspiaddr << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_RF_SPI_ADDR_ADDR, (REG_PL_RD(BOARD_RF_SPI_ADDR_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)rfspiaddr << 0));
}

/// @}

/**
 * @name RF_SPI_DATA register definitions
 * <table>
 * <caption>RF_SPI_DATA bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td> RF_SPI_RDATA <td> R/W <td> R <td> 0x0
 * <tr><td> 15:00 <td> RF_SPI_WDATA <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RF_SPI_DATA register
#define BOARD_RF_SPI_DATA_ADDR   0x24F10508
/// Offset of the RF_SPI_DATA register from the base address
#define BOARD_RF_SPI_DATA_OFFSET 0x00000508
/// Index of the RF_SPI_DATA register
#define BOARD_RF_SPI_DATA_INDEX  0x00000142
/// Reset value of the RF_SPI_DATA register
#define BOARD_RF_SPI_DATA_RESET  0x00000000

/**
 * @brief Returns the current value of the RF_SPI_DATA register.
 * The RF_SPI_DATA register will be read and its value returned.
 * @return The current value of the RF_SPI_DATA register.
 */
__INLINE uint32_t board_rf_spi_data_get(void)
{
    return REG_PL_RD(BOARD_RF_SPI_DATA_ADDR);
}

/**
 * @brief Sets the RF_SPI_DATA register to a value.
 * The RF_SPI_DATA register will be written.
 * @param value - The value to write.
 */
__INLINE void board_rf_spi_data_set(uint32_t value)
{
    REG_PL_WR(BOARD_RF_SPI_DATA_ADDR, value);
}

// field definitions
/// RF_SPI_RDATA field mask
#define BOARD_RF_SPI_RDATA_MASK   ((uint32_t)0xFFFF0000)
/// RF_SPI_RDATA field LSB position
#define BOARD_RF_SPI_RDATA_LSB    16
/// RF_SPI_RDATA field width
#define BOARD_RF_SPI_RDATA_WIDTH  ((uint32_t)0x00000010)
/// RF_SPI_WDATA field mask
#define BOARD_RF_SPI_WDATA_MASK   ((uint32_t)0x0000FFFF)
/// RF_SPI_WDATA field LSB position
#define BOARD_RF_SPI_WDATA_LSB    0
/// RF_SPI_WDATA field width
#define BOARD_RF_SPI_WDATA_WIDTH  ((uint32_t)0x00000010)

/// RF_SPI_RDATA field reset value
#define BOARD_RF_SPI_RDATA_RST    0x0
/// RF_SPI_WDATA field reset value
#define BOARD_RF_SPI_WDATA_RST    0x0

/**
 * @brief Unpacks RF_SPI_DATA's fields from current value of the RF_SPI_DATA register.
 *
 * Reads the RF_SPI_DATA register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rfspirdata - Will be populated with the current value of this field from the register.
 * @param[out] rfspiwdata - Will be populated with the current value of this field from the register.
 */
__INLINE void board_rf_spi_data_unpack(uint16_t* rfspirdata, uint16_t* rfspiwdata)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_SPI_DATA_ADDR);

    *rfspirdata = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *rfspiwdata = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the RF_SPI_RDATA field in the RF_SPI_DATA register.
 *
 * The RF_SPI_DATA register will be read and the RF_SPI_RDATA field's value will be returned.
 *
 * @return The current value of the RF_SPI_RDATA field in the RF_SPI_DATA register.
 */
__INLINE uint16_t board_rf_spi_rdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_SPI_DATA_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the RF_SPI_WDATA field in the RF_SPI_DATA register.
 *
 * The RF_SPI_DATA register will be read and the RF_SPI_WDATA field's value will be returned.
 *
 * @return The current value of the RF_SPI_WDATA field in the RF_SPI_DATA register.
 */
__INLINE uint16_t board_rf_spi_wdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_SPI_DATA_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the RF_SPI_WDATA field of the RF_SPI_DATA register.
 *
 * The RF_SPI_DATA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfspiwdata - The value to set the field to.
 */
__INLINE void board_rf_spi_wdata_setf(uint16_t rfspiwdata)
{
    ASSERT_ERR((((uint32_t)rfspiwdata << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_RF_SPI_DATA_ADDR, (uint32_t)rfspiwdata << 0);
}

/// @}

/**
 * @name RF_FORCE register definitions
 * <table>
 * <caption>RF_FORCE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td> RF_FORCE_EN <td> R <td> R/W <td> 0x0
 * <tr><td> 15:00 <td>    RF_FORCE <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RF_FORCE register
#define BOARD_RF_FORCE_ADDR   0x24F1050C
/// Offset of the RF_FORCE register from the base address
#define BOARD_RF_FORCE_OFFSET 0x0000050C
/// Index of the RF_FORCE register
#define BOARD_RF_FORCE_INDEX  0x00000143
/// Reset value of the RF_FORCE register
#define BOARD_RF_FORCE_RESET  0x00000000

/**
 * @brief Returns the current value of the RF_FORCE register.
 * The RF_FORCE register will be read and its value returned.
 * @return The current value of the RF_FORCE register.
 */
__INLINE uint32_t board_rf_force_get(void)
{
    return REG_PL_RD(BOARD_RF_FORCE_ADDR);
}

/**
 * @brief Sets the RF_FORCE register to a value.
 * The RF_FORCE register will be written.
 * @param value - The value to write.
 */
__INLINE void board_rf_force_set(uint32_t value)
{
    REG_PL_WR(BOARD_RF_FORCE_ADDR, value);
}

// field definitions
/// RF_FORCE_EN field mask
#define BOARD_RF_FORCE_EN_MASK   ((uint32_t)0xFFFF0000)
/// RF_FORCE_EN field LSB position
#define BOARD_RF_FORCE_EN_LSB    16
/// RF_FORCE_EN field width
#define BOARD_RF_FORCE_EN_WIDTH  ((uint32_t)0x00000010)
/// RF_FORCE field mask
#define BOARD_RF_FORCE_MASK      ((uint32_t)0x0000FFFF)
/// RF_FORCE field LSB position
#define BOARD_RF_FORCE_LSB       0
/// RF_FORCE field width
#define BOARD_RF_FORCE_WIDTH     ((uint32_t)0x00000010)

/// RF_FORCE_EN field reset value
#define BOARD_RF_FORCE_EN_RST    0x0
/// RF_FORCE field reset value
#define BOARD_RF_FORCE_RST       0x0

/**
 * @brief Constructs a value for the RF_FORCE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rfforceen - The value to use for the RF_FORCE_EN field.
 * @param[in] rfforce - The value to use for the RF_FORCE field.
 */
__INLINE void board_rf_force_pack(uint16_t rfforceen, uint16_t rfforce)
{
    ASSERT_ERR((((uint32_t)rfforceen << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)rfforce << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_RF_FORCE_ADDR,  ((uint32_t)rfforceen << 16) | ((uint32_t)rfforce << 0));
}

/**
 * @brief Unpacks RF_FORCE's fields from current value of the RF_FORCE register.
 *
 * Reads the RF_FORCE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rfforceen - Will be populated with the current value of this field from the register.
 * @param[out] rfforce - Will be populated with the current value of this field from the register.
 */
__INLINE void board_rf_force_unpack(uint16_t* rfforceen, uint16_t* rfforce)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_FORCE_ADDR);

    *rfforceen = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *rfforce = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the RF_FORCE_EN field in the RF_FORCE register.
 *
 * The RF_FORCE register will be read and the RF_FORCE_EN field's value will be returned.
 *
 * @return The current value of the RF_FORCE_EN field in the RF_FORCE register.
 */
__INLINE uint16_t board_rf_force_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_FORCE_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Sets the RF_FORCE_EN field of the RF_FORCE register.
 *
 * The RF_FORCE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfforceen - The value to set the field to.
 */
__INLINE void board_rf_force_en_setf(uint16_t rfforceen)
{
    ASSERT_ERR((((uint32_t)rfforceen << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_PL_WR(BOARD_RF_FORCE_ADDR, (REG_PL_RD(BOARD_RF_FORCE_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)rfforceen << 16));
}

/**
 * @brief Returns the current value of the RF_FORCE field in the RF_FORCE register.
 *
 * The RF_FORCE register will be read and the RF_FORCE field's value will be returned.
 *
 * @return The current value of the RF_FORCE field in the RF_FORCE register.
 */
__INLINE uint16_t board_rf_force_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the RF_FORCE field of the RF_FORCE register.
 *
 * The RF_FORCE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfforce - The value to set the field to.
 */
__INLINE void board_rf_force_setf(uint16_t rfforce)
{
    ASSERT_ERR((((uint32_t)rfforce << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(BOARD_RF_FORCE_ADDR, (REG_PL_RD(BOARD_RF_FORCE_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)rfforce << 0));
}

/// @}

/**
 * @name RF_GPIO register definitions
 * <table>
 * <caption>RF_GPIO bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> RF_GPIO <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RF_GPIO register
#define BOARD_RF_GPIO_ADDR   0x24F10510
/// Offset of the RF_GPIO register from the base address
#define BOARD_RF_GPIO_OFFSET 0x00000510
/// Index of the RF_GPIO register
#define BOARD_RF_GPIO_INDEX  0x00000144
/// Reset value of the RF_GPIO register
#define BOARD_RF_GPIO_RESET  0x00000000

/**
 * @brief Returns the current value of the RF_GPIO register.
 * The RF_GPIO register will be read and its value returned.
 * @return The current value of the RF_GPIO register.
 */
__INLINE uint32_t board_rf_gpio_get(void)
{
    return REG_PL_RD(BOARD_RF_GPIO_ADDR);
}

/**
 * @brief Sets the RF_GPIO register to a value.
 * The RF_GPIO register will be written.
 * @param value - The value to write.
 */
__INLINE void board_rf_gpio_set(uint32_t value)
{
    REG_PL_WR(BOARD_RF_GPIO_ADDR, value);
}

// field definitions
/// RF_GPIO field mask
#define BOARD_RF_GPIO_MASK   ((uint32_t)0x000000FF)
/// RF_GPIO field LSB position
#define BOARD_RF_GPIO_LSB    0
/// RF_GPIO field width
#define BOARD_RF_GPIO_WIDTH  ((uint32_t)0x00000008)

/// RF_GPIO field reset value
#define BOARD_RF_GPIO_RST    0x0

/**
 * @brief Returns the current value of the RF_GPIO field in the RF_GPIO register.
 *
 * The RF_GPIO register will be read and the RF_GPIO field's value will be returned.
 *
 * @return The current value of the RF_GPIO field in the RF_GPIO register.
 */
__INLINE uint8_t board_rf_gpio_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_GPIO_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the RF_GPIO field of the RF_GPIO register.
 *
 * The RF_GPIO register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfgpio - The value to set the field to.
 */
__INLINE void board_rf_gpio_setf(uint8_t rfgpio)
{
    ASSERT_ERR((((uint32_t)rfgpio << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(BOARD_RF_GPIO_ADDR, (uint32_t)rfgpio << 0);
}

/// @}

/**
 * @name RF_IRQ register definitions
 * <table>
 * <caption>RF_IRQ bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> RF_IRQ <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RF_IRQ register
#define BOARD_RF_IRQ_ADDR   0x24F10514
/// Offset of the RF_IRQ register from the base address
#define BOARD_RF_IRQ_OFFSET 0x00000514
/// Index of the RF_IRQ register
#define BOARD_RF_IRQ_INDEX  0x00000145
/// Reset value of the RF_IRQ register
#define BOARD_RF_IRQ_RESET  0x00000000

/**
 * @brief Returns the current value of the RF_IRQ register.
 * The RF_IRQ register will be read and its value returned.
 * @return The current value of the RF_IRQ register.
 */
__INLINE uint32_t board_rf_irq_get(void)
{
    return REG_PL_RD(BOARD_RF_IRQ_ADDR);
}

// field definitions
/// RF_IRQ field mask
#define BOARD_RF_IRQ_MASK   ((uint32_t)0x000000FF)
/// RF_IRQ field LSB position
#define BOARD_RF_IRQ_LSB    0
/// RF_IRQ field width
#define BOARD_RF_IRQ_WIDTH  ((uint32_t)0x00000008)

/// RF_IRQ field reset value
#define BOARD_RF_IRQ_RST    0x0

/**
 * @brief Returns the current value of the RF_IRQ field in the RF_IRQ register.
 *
 * The RF_IRQ register will be read and the RF_IRQ field's value will be returned.
 *
 * @return The current value of the RF_IRQ field in the RF_IRQ register.
 */
__INLINE uint8_t board_rf_irq_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_RF_IRQ_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name SVNREV_MODEM register definitions
 * <table>
 * <caption>SVNREV_MODEM bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> SVNREV_MODEM <td> R/W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SVNREV_MODEM register
#define BOARD_SVNREV_MODEM_ADDR   0x24F10064
/// Offset of the SVNREV_MODEM register from the base address
#define BOARD_SVNREV_MODEM_OFFSET 0x00000064
/// Index of the SVNREV_MODEM register
#define BOARD_SVNREV_MODEM_INDEX  0x00000019
/// Reset value of the SVNREV_MODEM register
#define BOARD_SVNREV_MODEM_RESET  0x00000000

/**
 * @brief Returns the current value of the SVNREV_MODEM register.
 * The SVNREV_MODEM register will be read and its value returned.
 * @return The current value of the SVNREV_MODEM register.
 */
__INLINE uint32_t board_svnrev_modem_get(void)
{
    return REG_PL_RD(BOARD_SVNREV_MODEM_ADDR);
}

// field definitions
/// SVNREV_MODEM field mask
#define BOARD_SVNREV_MODEM_MASK   ((uint32_t)0xFFFFFFFF)
/// SVNREV_MODEM field LSB position
#define BOARD_SVNREV_MODEM_LSB    0
/// SVNREV_MODEM field width
#define BOARD_SVNREV_MODEM_WIDTH  ((uint32_t)0x00000020)

/// SVNREV_MODEM field reset value
#define BOARD_SVNREV_MODEM_RST    0x0

/**
 * @brief Returns the current value of the SVNREV_MODEM field in the SVNREV_MODEM register.
 *
 * The SVNREV_MODEM register will be read and the SVNREV_MODEM field's value will be returned.
 *
 * @return The current value of the SVNREV_MODEM field in the SVNREV_MODEM register.
 */
__INLINE uint32_t board_svnrev_modem_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_SVNREV_MODEM_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name SVNREV_FPGAB register definitions
 * <table>
 * <caption>SVNREV_FPGAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> SVNREV_FPGAB <td> R/W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SVNREV_FPGAB register
#define BOARD_SVNREV_FPGAB_ADDR   0x24F10068
/// Offset of the SVNREV_FPGAB register from the base address
#define BOARD_SVNREV_FPGAB_OFFSET 0x00000068
/// Index of the SVNREV_FPGAB register
#define BOARD_SVNREV_FPGAB_INDEX  0x0000001A
/// Reset value of the SVNREV_FPGAB register
#define BOARD_SVNREV_FPGAB_RESET  0x00000000

/**
 * @brief Returns the current value of the SVNREV_FPGAB register.
 * The SVNREV_FPGAB register will be read and its value returned.
 * @return The current value of the SVNREV_FPGAB register.
 */
__INLINE uint32_t board_svnrev_fpgab_get(void)
{
    return REG_PL_RD(BOARD_SVNREV_FPGAB_ADDR);
}

// field definitions
/// SVNREV_FPGAB field mask
#define BOARD_SVNREV_FPGAB_MASK   ((uint32_t)0xFFFFFFFF)
/// SVNREV_FPGAB field LSB position
#define BOARD_SVNREV_FPGAB_LSB    0
/// SVNREV_FPGAB field width
#define BOARD_SVNREV_FPGAB_WIDTH  ((uint32_t)0x00000020)

/// SVNREV_FPGAB field reset value
#define BOARD_SVNREV_FPGAB_RST    0x0

/**
 * @brief Returns the current value of the SVNREV_FPGAB field in the SVNREV_FPGAB register.
 *
 * The SVNREV_FPGAB register will be read and the SVNREV_FPGAB field's value will be returned.
 *
 * @return The current value of the SVNREV_FPGAB field in the SVNREV_FPGAB register.
 */
__INLINE uint32_t board_svnrev_fpgab_getf(void)
{
    uint32_t localVal = REG_PL_RD(BOARD_SVNREV_FPGAB_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}


#endif // _REG_BOARD_IF_H_

/// @}

