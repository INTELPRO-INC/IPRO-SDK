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
 * @file reg_la.h
 * @brief Definitions of the LA HW block registers and register access functions.
 *
 * @defgroup REG_LA REG_LA
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the LA HW block registers and register access functions.
 */
#ifndef _REG_LA_H_
#define _REG_LA_H_

#include "co_int.h"
#include "_reg_la.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_LA peripheral.
 */
#define REG_LA_COUNT 19

/** @brief Decoding mask of the REG_LA peripheral registers from the CPU point of view.
 */
#define REG_LA_DECODING_MASK 0x0000007F

/**
 * @name ID_LOW register definitions
 * <table>
 * <caption>ID_LOW bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> id_low <td> W <td> R <td> 0x656E6562
 * </table>
 *
 * @{
 */

/// Address of the ID_LOW register
#define LA_ID_LOW_ADDR   0x24E00000
/// Offset of the ID_LOW register from the base address
#define LA_ID_LOW_OFFSET 0x00000000
/// Index of the ID_LOW register
#define LA_ID_LOW_INDEX  0x00000000
/// Reset value of the ID_LOW register
#define LA_ID_LOW_RESET  0x656E6562

/**
 * @brief Returns the current value of the ID_LOW register.
 * The ID_LOW register will be read and its value returned.
 * @return The current value of the ID_LOW register.
 */
__INLINE uint32_t la_id_low_get(void)
{
    return REG_PL_RD(LA_ID_LOW_ADDR);
}

// field definitions
/// ID_LOW field mask
#define LA_ID_LOW_MASK   ((uint32_t)0xFFFFFFFF)
/// ID_LOW field LSB position
#define LA_ID_LOW_LSB    0
/// ID_LOW field width
#define LA_ID_LOW_WIDTH  ((uint32_t)0x00000020)

/// ID_LOW field reset value
#define LA_ID_LOW_RST    0x656E6562

/**
 * @brief Returns the current value of the id_low field in the ID_LOW register.
 *
 * The ID_LOW register will be read and the id_low field's value will be returned.
 *
 * @return The current value of the id_low field in the ID_LOW register.
 */
__INLINE uint32_t la_id_low_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_ID_LOW_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name ID_HIGH register definitions
 * <table>
 * <caption>ID_HIGH bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 23:16 <td> la_width <td> W <td> R <td> 0x0
 * <tr><td> 15:00 <td> la_depth <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the ID_HIGH register
#define LA_ID_HIGH_ADDR   0x24E00004
/// Offset of the ID_HIGH register from the base address
#define LA_ID_HIGH_OFFSET 0x00000004
/// Index of the ID_HIGH register
#define LA_ID_HIGH_INDEX  0x00000001
/// Reset value of the ID_HIGH register
#define LA_ID_HIGH_RESET  0x00000000

/**
 * @brief Returns the current value of the ID_HIGH register.
 * The ID_HIGH register will be read and its value returned.
 * @return The current value of the ID_HIGH register.
 */
__INLINE uint32_t la_id_high_get(void)
{
    return REG_PL_RD(LA_ID_HIGH_ADDR);
}

// field definitions
/// LA_WIDTH field mask
#define LA_LA_WIDTH_MASK   ((uint32_t)0x00FF0000)
/// LA_WIDTH field LSB position
#define LA_LA_WIDTH_LSB    16
/// LA_WIDTH field width
#define LA_LA_WIDTH_WIDTH  ((uint32_t)0x00000008)
/// LA_DEPTH field mask
#define LA_LA_DEPTH_MASK   ((uint32_t)0x0000FFFF)
/// LA_DEPTH field LSB position
#define LA_LA_DEPTH_LSB    0
/// LA_DEPTH field width
#define LA_LA_DEPTH_WIDTH  ((uint32_t)0x00000010)

/// LA_WIDTH field reset value
#define LA_LA_WIDTH_RST    0x0
/// LA_DEPTH field reset value
#define LA_LA_DEPTH_RST    0x0

/**
 * @brief Unpacks ID_HIGH's fields from current value of the ID_HIGH register.
 *
 * Reads the ID_HIGH register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] lawidth - Will be populated with the current value of this field from the register.
 * @param[out] ladepth - Will be populated with the current value of this field from the register.
 */
__INLINE void la_id_high_unpack(uint8_t* lawidth, uint16_t* ladepth)
{
    uint32_t localVal = REG_PL_RD(LA_ID_HIGH_ADDR);

    *lawidth = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *ladepth = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the la_width field in the ID_HIGH register.
 *
 * The ID_HIGH register will be read and the la_width field's value will be returned.
 *
 * @return The current value of the la_width field in the ID_HIGH register.
 */
__INLINE uint8_t la_la_width_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_ID_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the la_depth field in the ID_HIGH register.
 *
 * The ID_HIGH register will be read and the la_depth field's value will be returned.
 *
 * @return The current value of the la_depth field in the ID_HIGH register.
 */
__INLINE uint16_t la_la_depth_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_ID_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name VERSION register definitions
 * <table>
 * <caption>VERSION bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:24 <td>  samplingFreq <td> W <td> R <td> 0x0
 * <tr><td> 23:00 <td>    la_version <td> W <td> R <td> 0x50000
 * </table>
 *
 * @{
 */

/// Address of the VERSION register
#define LA_VERSION_ADDR   0x24E00008
/// Offset of the VERSION register from the base address
#define LA_VERSION_OFFSET 0x00000008
/// Index of the VERSION register
#define LA_VERSION_INDEX  0x00000002
/// Reset value of the VERSION register
#define LA_VERSION_RESET  0x00050000

/**
 * @brief Returns the current value of the VERSION register.
 * The VERSION register will be read and its value returned.
 * @return The current value of the VERSION register.
 */
__INLINE uint32_t la_version_get(void)
{
    return REG_PL_RD(LA_VERSION_ADDR);
}

// field definitions
/// SAMPLING_FREQ field mask
#define LA_SAMPLING_FREQ_MASK   ((uint32_t)0xFF000000)
/// SAMPLING_FREQ field LSB position
#define LA_SAMPLING_FREQ_LSB    24
/// SAMPLING_FREQ field width
#define LA_SAMPLING_FREQ_WIDTH  ((uint32_t)0x00000008)
/// LA_VERSION field mask
#define LA_LA_VERSION_MASK      ((uint32_t)0x00FFFFFF)
/// LA_VERSION field LSB position
#define LA_LA_VERSION_LSB       0
/// LA_VERSION field width
#define LA_LA_VERSION_WIDTH     ((uint32_t)0x00000018)

/// SAMPLING_FREQ field reset value
#define LA_SAMPLING_FREQ_RST    0x0
/// LA_VERSION field reset value
#define LA_LA_VERSION_RST       0x50000

/**
 * @brief Unpacks VERSION's fields from current value of the VERSION register.
 *
 * Reads the VERSION register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] samplingfreq - Will be populated with the current value of this field from the register.
 * @param[out] laversion - Will be populated with the current value of this field from the register.
 */
__INLINE void la_version_unpack(uint8_t* samplingfreq, uint32_t* laversion)
{
    uint32_t localVal = REG_PL_RD(LA_VERSION_ADDR);

    *samplingfreq = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *laversion = (localVal & ((uint32_t)0x00FFFFFF)) >> 0;
}

/**
 * @brief Returns the current value of the samplingFreq field in the VERSION register.
 *
 * The VERSION register will be read and the samplingFreq field's value will be returned.
 *
 * @return The current value of the samplingFreq field in the VERSION register.
 */
__INLINE uint8_t la_sampling_freq_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_VERSION_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the la_version field in the VERSION register.
 *
 * The VERSION register will be read and the la_version field's value will be returned.
 *
 * @return The current value of the la_version field in the VERSION register.
 */
__INLINE uint32_t la_la_version_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_VERSION_ADDR);
    return ((localVal & ((uint32_t)0x00FFFFFF)) >> 0);
}

/// @}

/**
 * @name CNTRL register definitions
 * <table>
 * <caption>CNTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 14    <td>    trigger_int_en <td> R <td> R/W <td> 0
 * <tr><td> 13    <td> trigger_int_clear <td> R <td> R/W <td> 0
 * <tr><td> 12    <td>   trigger_int_set <td> R <td> R/W <td> 0
 * <tr><td> 07:04 <td>    ext_trigger_en <td> R <td> R/W <td> 0xF
 * <tr><td> 03    <td>        sw_trigger <td> R <td> R/W <td> 0
 * <tr><td> 02    <td>             reset <td> R <td> R/W <td> 0
 * <tr><td> 01    <td>              stop <td> R <td> R/W <td> 0
 * <tr><td> 00    <td>             start <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the CNTRL register
#define LA_CNTRL_ADDR   0x24E0000C
/// Offset of the CNTRL register from the base address
#define LA_CNTRL_OFFSET 0x0000000C
/// Index of the CNTRL register
#define LA_CNTRL_INDEX  0x00000003
/// Reset value of the CNTRL register
#define LA_CNTRL_RESET  0x000000F0

/**
 * @brief Returns the current value of the CNTRL register.
 * The CNTRL register will be read and its value returned.
 * @return The current value of the CNTRL register.
 */
__INLINE uint32_t la_cntrl_get(void)
{
    return REG_PL_RD(LA_CNTRL_ADDR);
}

/**
 * @brief Sets the CNTRL register to a value.
 * The CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void la_cntrl_set(uint32_t value)
{
    REG_PL_WR(LA_CNTRL_ADDR, value);
}

// field definitions
/// TRIGGER_INT_EN field bit
#define LA_TRIGGER_INT_EN_BIT       ((uint32_t)0x00004000)
/// TRIGGER_INT_EN field position
#define LA_TRIGGER_INT_EN_POS       14
/// TRIGGER_INT_CLEAR field bit
#define LA_TRIGGER_INT_CLEAR_BIT    ((uint32_t)0x00002000)
/// TRIGGER_INT_CLEAR field position
#define LA_TRIGGER_INT_CLEAR_POS    13
/// TRIGGER_INT_SET field bit
#define LA_TRIGGER_INT_SET_BIT      ((uint32_t)0x00001000)
/// TRIGGER_INT_SET field position
#define LA_TRIGGER_INT_SET_POS      12
/// EXT_TRIGGER_EN field mask
#define LA_EXT_TRIGGER_EN_MASK      ((uint32_t)0x000000F0)
/// EXT_TRIGGER_EN field LSB position
#define LA_EXT_TRIGGER_EN_LSB       4
/// EXT_TRIGGER_EN field width
#define LA_EXT_TRIGGER_EN_WIDTH     ((uint32_t)0x00000004)
/// SW_TRIGGER field bit
#define LA_SW_TRIGGER_BIT           ((uint32_t)0x00000008)
/// SW_TRIGGER field position
#define LA_SW_TRIGGER_POS           3
/// RESET field bit
#define LA_RESET_BIT                ((uint32_t)0x00000004)
/// RESET field position
#define LA_RESET_POS                2
/// STOP field bit
#define LA_STOP_BIT                 ((uint32_t)0x00000002)
/// STOP field position
#define LA_STOP_POS                 1
/// START field bit
#define LA_START_BIT                ((uint32_t)0x00000001)
/// START field position
#define LA_START_POS                0

/// TRIGGER_INT_EN field reset value
#define LA_TRIGGER_INT_EN_RST       0x0
/// TRIGGER_INT_CLEAR field reset value
#define LA_TRIGGER_INT_CLEAR_RST    0x0
/// TRIGGER_INT_SET field reset value
#define LA_TRIGGER_INT_SET_RST      0x0
/// EXT_TRIGGER_EN field reset value
#define LA_EXT_TRIGGER_EN_RST       0xF
/// SW_TRIGGER field reset value
#define LA_SW_TRIGGER_RST           0x0
/// RESET field reset value
#define LA_RESET_RST                0x0
/// STOP field reset value
#define LA_STOP_RST                 0x0
/// START field reset value
#define LA_START_RST                0x0

/**
 * @brief Constructs a value for the CNTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] triggerinten - The value to use for the trigger_int_en field.
 * @param[in] triggerintclear - The value to use for the trigger_int_clear field.
 * @param[in] triggerintset - The value to use for the trigger_int_set field.
 * @param[in] exttriggeren - The value to use for the ext_trigger_en field.
 * @param[in] swtrigger - The value to use for the sw_trigger field.
 * @param[in] reset - The value to use for the reset field.
 * @param[in] stop - The value to use for the stop field.
 * @param[in] start - The value to use for the start field.
 */
__INLINE void la_cntrl_pack(uint8_t triggerinten, uint8_t triggerintclear, uint8_t triggerintset, uint8_t exttriggeren, uint8_t swtrigger, uint8_t reset, uint8_t stop, uint8_t start)
{
    ASSERT_ERR((((uint32_t)triggerinten << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)triggerintclear << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)triggerintset << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)exttriggeren << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)swtrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)reset << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)stop << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)start << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR,  ((uint32_t)triggerinten << 14) | ((uint32_t)triggerintclear << 13) | ((uint32_t)triggerintset << 12) | ((uint32_t)exttriggeren << 4) | ((uint32_t)swtrigger << 3) | ((uint32_t)reset << 2) | ((uint32_t)stop << 1) | ((uint32_t)start << 0));
}

/**
 * @brief Unpacks CNTRL's fields from current value of the CNTRL register.
 *
 * Reads the CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] triggerinten - Will be populated with the current value of this field from the register.
 * @param[out] triggerintclear - Will be populated with the current value of this field from the register.
 * @param[out] triggerintset - Will be populated with the current value of this field from the register.
 * @param[out] exttriggeren - Will be populated with the current value of this field from the register.
 * @param[out] swtrigger - Will be populated with the current value of this field from the register.
 * @param[out] reset - Will be populated with the current value of this field from the register.
 * @param[out] stop - Will be populated with the current value of this field from the register.
 * @param[out] start - Will be populated with the current value of this field from the register.
 */
__INLINE void la_cntrl_unpack(uint8_t* triggerinten, uint8_t* triggerintclear, uint8_t* triggerintset, uint8_t* exttriggeren, uint8_t* swtrigger, uint8_t* reset, uint8_t* stop, uint8_t* start)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);

    *triggerinten = (localVal & ((uint32_t)0x00004000)) >> 14;
    *triggerintclear = (localVal & ((uint32_t)0x00002000)) >> 13;
    *triggerintset = (localVal & ((uint32_t)0x00001000)) >> 12;
    *exttriggeren = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *swtrigger = (localVal & ((uint32_t)0x00000008)) >> 3;
    *reset = (localVal & ((uint32_t)0x00000004)) >> 2;
    *stop = (localVal & ((uint32_t)0x00000002)) >> 1;
    *start = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the trigger_int_en field in the CNTRL register.
 *
 * The CNTRL register will be read and the trigger_int_en field's value will be returned.
 *
 * @return The current value of the trigger_int_en field in the CNTRL register.
 */
__INLINE uint8_t la_trigger_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the trigger_int_en field of the CNTRL register.
 *
 * The CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggerinten - The value to set the field to.
 */
__INLINE void la_trigger_int_en_setf(uint8_t triggerinten)
{
    ASSERT_ERR((((uint32_t)triggerinten << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR, (REG_PL_RD(LA_CNTRL_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)triggerinten << 14));
}

/**
 * @brief Returns the current value of the trigger_int_clear field in the CNTRL register.
 *
 * The CNTRL register will be read and the trigger_int_clear field's value will be returned.
 *
 * @return The current value of the trigger_int_clear field in the CNTRL register.
 */
__INLINE uint8_t la_trigger_int_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the trigger_int_clear field of the CNTRL register.
 *
 * The CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggerintclear - The value to set the field to.
 */
__INLINE void la_trigger_int_clear_setf(uint8_t triggerintclear)
{
    ASSERT_ERR((((uint32_t)triggerintclear << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR, (REG_PL_RD(LA_CNTRL_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)triggerintclear << 13));
}

/**
 * @brief Returns the current value of the trigger_int_set field in the CNTRL register.
 *
 * The CNTRL register will be read and the trigger_int_set field's value will be returned.
 *
 * @return The current value of the trigger_int_set field in the CNTRL register.
 */
__INLINE uint8_t la_trigger_int_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the trigger_int_set field of the CNTRL register.
 *
 * The CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggerintset - The value to set the field to.
 */
__INLINE void la_trigger_int_set_setf(uint8_t triggerintset)
{
    ASSERT_ERR((((uint32_t)triggerintset << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR, (REG_PL_RD(LA_CNTRL_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)triggerintset << 12));
}

/**
 * @brief Returns the current value of the ext_trigger_en field in the CNTRL register.
 *
 * The CNTRL register will be read and the ext_trigger_en field's value will be returned.
 *
 * @return The current value of the ext_trigger_en field in the CNTRL register.
 */
__INLINE uint8_t la_ext_trigger_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the ext_trigger_en field of the CNTRL register.
 *
 * The CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] exttriggeren - The value to set the field to.
 */
__INLINE void la_ext_trigger_en_setf(uint8_t exttriggeren)
{
    ASSERT_ERR((((uint32_t)exttriggeren << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR, (REG_PL_RD(LA_CNTRL_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)exttriggeren << 4));
}

/**
 * @brief Returns the current value of the sw_trigger field in the CNTRL register.
 *
 * The CNTRL register will be read and the sw_trigger field's value will be returned.
 *
 * @return The current value of the sw_trigger field in the CNTRL register.
 */
__INLINE uint8_t la_sw_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the sw_trigger field of the CNTRL register.
 *
 * The CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swtrigger - The value to set the field to.
 */
__INLINE void la_sw_trigger_setf(uint8_t swtrigger)
{
    ASSERT_ERR((((uint32_t)swtrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR, (REG_PL_RD(LA_CNTRL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)swtrigger << 3));
}

/**
 * @brief Returns the current value of the reset field in the CNTRL register.
 *
 * The CNTRL register will be read and the reset field's value will be returned.
 *
 * @return The current value of the reset field in the CNTRL register.
 */
__INLINE uint8_t la_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the reset field of the CNTRL register.
 *
 * The CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reset - The value to set the field to.
 */
__INLINE void la_reset_setf(uint8_t reset)
{
    ASSERT_ERR((((uint32_t)reset << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR, (REG_PL_RD(LA_CNTRL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)reset << 2));
}

/**
 * @brief Returns the current value of the stop field in the CNTRL register.
 *
 * The CNTRL register will be read and the stop field's value will be returned.
 *
 * @return The current value of the stop field in the CNTRL register.
 */
__INLINE uint8_t la_stop_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the stop field of the CNTRL register.
 *
 * The CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stop - The value to set the field to.
 */
__INLINE void la_stop_setf(uint8_t stop)
{
    ASSERT_ERR((((uint32_t)stop << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR, (REG_PL_RD(LA_CNTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)stop << 1));
}

/**
 * @brief Returns the current value of the start field in the CNTRL register.
 *
 * The CNTRL register will be read and the start field's value will be returned.
 *
 * @return The current value of the start field in the CNTRL register.
 */
__INLINE uint8_t la_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the start field of the CNTRL register.
 *
 * The CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] start - The value to set the field to.
 */
__INLINE void la_start_setf(uint8_t start)
{
    ASSERT_ERR((((uint32_t)start << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(LA_CNTRL_ADDR, (REG_PL_RD(LA_CNTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)start << 0));
}

/// @}

/**
 * @name STATUS register definitions
 * <table>
 * <caption>STATUS bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 23:08 <td>               writeaddr <td> W <td> R <td> 0x0
 * <tr><td> 07:04 <td>      ext_trigger_status <td> W <td> R <td> 0x0
 * <tr><td> 03    <td>       sw_trigger_status <td> W <td> R <td> 0
 * <tr><td> 02    <td> internal_trigger_status <td> W <td> R <td> 0
 * <tr><td> 01    <td>               triggered <td> W <td> R <td> 0
 * <tr><td> 00    <td>                 started <td> W <td> R <td> 0
 * </table>
 *
 * @{
 */

/// Address of the STATUS register
#define LA_STATUS_ADDR   0x24E00010
/// Offset of the STATUS register from the base address
#define LA_STATUS_OFFSET 0x00000010
/// Index of the STATUS register
#define LA_STATUS_INDEX  0x00000004
/// Reset value of the STATUS register
#define LA_STATUS_RESET  0x00000000

/**
 * @brief Returns the current value of the STATUS register.
 * The STATUS register will be read and its value returned.
 * @return The current value of the STATUS register.
 */
__INLINE uint32_t la_status_get(void)
{
    return REG_PL_RD(LA_STATUS_ADDR);
}

// field definitions
/// WRITEADDR field mask
#define LA_WRITEADDR_MASK                 ((uint32_t)0x00FFFF00)
/// WRITEADDR field LSB position
#define LA_WRITEADDR_LSB                  8
/// WRITEADDR field width
#define LA_WRITEADDR_WIDTH                ((uint32_t)0x00000010)
/// EXT_TRIGGER_STATUS field mask
#define LA_EXT_TRIGGER_STATUS_MASK        ((uint32_t)0x000000F0)
/// EXT_TRIGGER_STATUS field LSB position
#define LA_EXT_TRIGGER_STATUS_LSB         4
/// EXT_TRIGGER_STATUS field width
#define LA_EXT_TRIGGER_STATUS_WIDTH       ((uint32_t)0x00000004)
/// SW_TRIGGER_STATUS field bit
#define LA_SW_TRIGGER_STATUS_BIT          ((uint32_t)0x00000008)
/// SW_TRIGGER_STATUS field position
#define LA_SW_TRIGGER_STATUS_POS          3
/// INTERNAL_TRIGGER_STATUS field bit
#define LA_INTERNAL_TRIGGER_STATUS_BIT    ((uint32_t)0x00000004)
/// INTERNAL_TRIGGER_STATUS field position
#define LA_INTERNAL_TRIGGER_STATUS_POS    2
/// TRIGGERED field bit
#define LA_TRIGGERED_BIT                  ((uint32_t)0x00000002)
/// TRIGGERED field position
#define LA_TRIGGERED_POS                  1
/// STARTED field bit
#define LA_STARTED_BIT                    ((uint32_t)0x00000001)
/// STARTED field position
#define LA_STARTED_POS                    0

/// WRITEADDR field reset value
#define LA_WRITEADDR_RST                  0x0
/// EXT_TRIGGER_STATUS field reset value
#define LA_EXT_TRIGGER_STATUS_RST         0x0
/// SW_TRIGGER_STATUS field reset value
#define LA_SW_TRIGGER_STATUS_RST          0x0
/// INTERNAL_TRIGGER_STATUS field reset value
#define LA_INTERNAL_TRIGGER_STATUS_RST    0x0
/// TRIGGERED field reset value
#define LA_TRIGGERED_RST                  0x0
/// STARTED field reset value
#define LA_STARTED_RST                    0x0

/**
 * @brief Unpacks STATUS's fields from current value of the STATUS register.
 *
 * Reads the STATUS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] writeaddr - Will be populated with the current value of this field from the register.
 * @param[out] exttriggerstatus - Will be populated with the current value of this field from the register.
 * @param[out] swtriggerstatus - Will be populated with the current value of this field from the register.
 * @param[out] internaltriggerstatus - Will be populated with the current value of this field from the register.
 * @param[out] triggered - Will be populated with the current value of this field from the register.
 * @param[out] started - Will be populated with the current value of this field from the register.
 */
__INLINE void la_status_unpack(uint16_t* writeaddr, uint8_t* exttriggerstatus, uint8_t* swtriggerstatus, uint8_t* internaltriggerstatus, uint8_t* triggered, uint8_t* started)
{
    uint32_t localVal = REG_PL_RD(LA_STATUS_ADDR);

    *writeaddr = (localVal & ((uint32_t)0x00FFFF00)) >> 8;
    *exttriggerstatus = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *swtriggerstatus = (localVal & ((uint32_t)0x00000008)) >> 3;
    *internaltriggerstatus = (localVal & ((uint32_t)0x00000004)) >> 2;
    *triggered = (localVal & ((uint32_t)0x00000002)) >> 1;
    *started = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the writeaddr field in the STATUS register.
 *
 * The STATUS register will be read and the writeaddr field's value will be returned.
 *
 * @return The current value of the writeaddr field in the STATUS register.
 */
__INLINE uint16_t la_writeaddr_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00FFFF00)) >> 8);
}

/**
 * @brief Returns the current value of the ext_trigger_status field in the STATUS register.
 *
 * The STATUS register will be read and the ext_trigger_status field's value will be returned.
 *
 * @return The current value of the ext_trigger_status field in the STATUS register.
 */
__INLINE uint8_t la_ext_trigger_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Returns the current value of the sw_trigger_status field in the STATUS register.
 *
 * The STATUS register will be read and the sw_trigger_status field's value will be returned.
 *
 * @return The current value of the sw_trigger_status field in the STATUS register.
 */
__INLINE uint8_t la_sw_trigger_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the internal_trigger_status field in the STATUS register.
 *
 * The STATUS register will be read and the internal_trigger_status field's value will be returned.
 *
 * @return The current value of the internal_trigger_status field in the STATUS register.
 */
__INLINE uint8_t la_internal_trigger_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the triggered field in the STATUS register.
 *
 * The STATUS register will be read and the triggered field's value will be returned.
 *
 * @return The current value of the triggered field in the STATUS register.
 */
__INLINE uint8_t la_triggered_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the started field in the STATUS register.
 *
 * The STATUS register will be read and the started field's value will be returned.
 *
 * @return The current value of the started field in the STATUS register.
 */
__INLINE uint8_t la_started_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name SAMPLING_MASK0 register definitions
 * <table>
 * <caption>SAMPLING_MASK0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  sampling_mask0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SAMPLING_MASK0 register
#define LA_SAMPLING_MASK0_ADDR   0x24E00014
/// Offset of the SAMPLING_MASK0 register from the base address
#define LA_SAMPLING_MASK0_OFFSET 0x00000014
/// Index of the SAMPLING_MASK0 register
#define LA_SAMPLING_MASK0_INDEX  0x00000005
/// Reset value of the SAMPLING_MASK0 register
#define LA_SAMPLING_MASK0_RESET  0x00000000

/**
 * @brief Returns the current value of the SAMPLING_MASK0 register.
 * The SAMPLING_MASK0 register will be read and its value returned.
 * @return The current value of the SAMPLING_MASK0 register.
 */
__INLINE uint32_t la_sampling_mask0_get(void)
{
    return REG_PL_RD(LA_SAMPLING_MASK0_ADDR);
}

/**
 * @brief Sets the SAMPLING_MASK0 register to a value.
 * The SAMPLING_MASK0 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_sampling_mask0_set(uint32_t value)
{
    REG_PL_WR(LA_SAMPLING_MASK0_ADDR, value);
}

// field definitions
/// SAMPLING_MASK_0 field mask
#define LA_SAMPLING_MASK_0_MASK   ((uint32_t)0xFFFFFFFF)
/// SAMPLING_MASK_0 field LSB position
#define LA_SAMPLING_MASK_0_LSB    0
/// SAMPLING_MASK_0 field width
#define LA_SAMPLING_MASK_0_WIDTH  ((uint32_t)0x00000020)

/// SAMPLING_MASK_0 field reset value
#define LA_SAMPLING_MASK_0_RST    0x0

/**
 * @brief Returns the current value of the sampling_mask0 field in the SAMPLING_MASK0 register.
 *
 * The SAMPLING_MASK0 register will be read and the sampling_mask0 field's value will be returned.
 *
 * @return The current value of the sampling_mask0 field in the SAMPLING_MASK0 register.
 */
__INLINE uint32_t la_sampling_mask_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_SAMPLING_MASK0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the sampling_mask0 field of the SAMPLING_MASK0 register.
 *
 * The SAMPLING_MASK0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] samplingmask0 - The value to set the field to.
 */
__INLINE void la_sampling_mask_0_setf(uint32_t samplingmask0)
{
    ASSERT_ERR((((uint32_t)samplingmask0 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_SAMPLING_MASK0_ADDR, (uint32_t)samplingmask0 << 0);
}

/// @}

/**
 * @name SAMPLING_MASK1 register definitions
 * <table>
 * <caption>SAMPLING_MASK1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  sampling_mask1 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SAMPLING_MASK1 register
#define LA_SAMPLING_MASK1_ADDR   0x24E00018
/// Offset of the SAMPLING_MASK1 register from the base address
#define LA_SAMPLING_MASK1_OFFSET 0x00000018
/// Index of the SAMPLING_MASK1 register
#define LA_SAMPLING_MASK1_INDEX  0x00000006
/// Reset value of the SAMPLING_MASK1 register
#define LA_SAMPLING_MASK1_RESET  0x00000000

/**
 * @brief Returns the current value of the SAMPLING_MASK1 register.
 * The SAMPLING_MASK1 register will be read and its value returned.
 * @return The current value of the SAMPLING_MASK1 register.
 */
__INLINE uint32_t la_sampling_mask1_get(void)
{
    return REG_PL_RD(LA_SAMPLING_MASK1_ADDR);
}

/**
 * @brief Sets the SAMPLING_MASK1 register to a value.
 * The SAMPLING_MASK1 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_sampling_mask1_set(uint32_t value)
{
    REG_PL_WR(LA_SAMPLING_MASK1_ADDR, value);
}

// field definitions
/// SAMPLING_MASK_1 field mask
#define LA_SAMPLING_MASK_1_MASK   ((uint32_t)0xFFFFFFFF)
/// SAMPLING_MASK_1 field LSB position
#define LA_SAMPLING_MASK_1_LSB    0
/// SAMPLING_MASK_1 field width
#define LA_SAMPLING_MASK_1_WIDTH  ((uint32_t)0x00000020)

/// SAMPLING_MASK_1 field reset value
#define LA_SAMPLING_MASK_1_RST    0x0

/**
 * @brief Returns the current value of the sampling_mask1 field in the SAMPLING_MASK1 register.
 *
 * The SAMPLING_MASK1 register will be read and the sampling_mask1 field's value will be returned.
 *
 * @return The current value of the sampling_mask1 field in the SAMPLING_MASK1 register.
 */
__INLINE uint32_t la_sampling_mask_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_SAMPLING_MASK1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the sampling_mask1 field of the SAMPLING_MASK1 register.
 *
 * The SAMPLING_MASK1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] samplingmask1 - The value to set the field to.
 */
__INLINE void la_sampling_mask_1_setf(uint32_t samplingmask1)
{
    ASSERT_ERR((((uint32_t)samplingmask1 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_SAMPLING_MASK1_ADDR, (uint32_t)samplingmask1 << 0);
}

/// @}

/**
 * @name SAMPLING_MASK2 register definitions
 * <table>
 * <caption>SAMPLING_MASK2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  sampling_mask2 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SAMPLING_MASK2 register
#define LA_SAMPLING_MASK2_ADDR   0x24E0001C
/// Offset of the SAMPLING_MASK2 register from the base address
#define LA_SAMPLING_MASK2_OFFSET 0x0000001C
/// Index of the SAMPLING_MASK2 register
#define LA_SAMPLING_MASK2_INDEX  0x00000007
/// Reset value of the SAMPLING_MASK2 register
#define LA_SAMPLING_MASK2_RESET  0x00000000

/**
 * @brief Returns the current value of the SAMPLING_MASK2 register.
 * The SAMPLING_MASK2 register will be read and its value returned.
 * @return The current value of the SAMPLING_MASK2 register.
 */
__INLINE uint32_t la_sampling_mask2_get(void)
{
    return REG_PL_RD(LA_SAMPLING_MASK2_ADDR);
}

/**
 * @brief Sets the SAMPLING_MASK2 register to a value.
 * The SAMPLING_MASK2 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_sampling_mask2_set(uint32_t value)
{
    REG_PL_WR(LA_SAMPLING_MASK2_ADDR, value);
}

// field definitions
/// SAMPLING_MASK_2 field mask
#define LA_SAMPLING_MASK_2_MASK   ((uint32_t)0xFFFFFFFF)
/// SAMPLING_MASK_2 field LSB position
#define LA_SAMPLING_MASK_2_LSB    0
/// SAMPLING_MASK_2 field width
#define LA_SAMPLING_MASK_2_WIDTH  ((uint32_t)0x00000020)

/// SAMPLING_MASK_2 field reset value
#define LA_SAMPLING_MASK_2_RST    0x0

/**
 * @brief Returns the current value of the sampling_mask2 field in the SAMPLING_MASK2 register.
 *
 * The SAMPLING_MASK2 register will be read and the sampling_mask2 field's value will be returned.
 *
 * @return The current value of the sampling_mask2 field in the SAMPLING_MASK2 register.
 */
__INLINE uint32_t la_sampling_mask_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_SAMPLING_MASK2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the sampling_mask2 field of the SAMPLING_MASK2 register.
 *
 * The SAMPLING_MASK2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] samplingmask2 - The value to set the field to.
 */
__INLINE void la_sampling_mask_2_setf(uint32_t samplingmask2)
{
    ASSERT_ERR((((uint32_t)samplingmask2 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_SAMPLING_MASK2_ADDR, (uint32_t)samplingmask2 << 0);
}

/// @}

/**
 * @name TRIGGER_MASK0 register definitions
 * <table>
 * <caption>TRIGGER_MASK0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  trigger_mask0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_MASK0 register
#define LA_TRIGGER_MASK0_ADDR   0x24E00020
/// Offset of the TRIGGER_MASK0 register from the base address
#define LA_TRIGGER_MASK0_OFFSET 0x00000020
/// Index of the TRIGGER_MASK0 register
#define LA_TRIGGER_MASK0_INDEX  0x00000008
/// Reset value of the TRIGGER_MASK0 register
#define LA_TRIGGER_MASK0_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_MASK0 register.
 * The TRIGGER_MASK0 register will be read and its value returned.
 * @return The current value of the TRIGGER_MASK0 register.
 */
__INLINE uint32_t la_trigger_mask0_get(void)
{
    return REG_PL_RD(LA_TRIGGER_MASK0_ADDR);
}

/**
 * @brief Sets the TRIGGER_MASK0 register to a value.
 * The TRIGGER_MASK0 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_mask0_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_MASK0_ADDR, value);
}

// field definitions
/// TRIGGER_MASK_0 field mask
#define LA_TRIGGER_MASK_0_MASK   ((uint32_t)0xFFFFFFFF)
/// TRIGGER_MASK_0 field LSB position
#define LA_TRIGGER_MASK_0_LSB    0
/// TRIGGER_MASK_0 field width
#define LA_TRIGGER_MASK_0_WIDTH  ((uint32_t)0x00000020)

/// TRIGGER_MASK_0 field reset value
#define LA_TRIGGER_MASK_0_RST    0x0

/**
 * @brief Returns the current value of the trigger_mask0 field in the TRIGGER_MASK0 register.
 *
 * The TRIGGER_MASK0 register will be read and the trigger_mask0 field's value will be returned.
 *
 * @return The current value of the trigger_mask0 field in the TRIGGER_MASK0 register.
 */
__INLINE uint32_t la_trigger_mask_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_MASK0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_mask0 field of the TRIGGER_MASK0 register.
 *
 * The TRIGGER_MASK0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggermask0 - The value to set the field to.
 */
__INLINE void la_trigger_mask_0_setf(uint32_t triggermask0)
{
    ASSERT_ERR((((uint32_t)triggermask0 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_MASK0_ADDR, (uint32_t)triggermask0 << 0);
}

/// @}

/**
 * @name TRIGGER_MASK1 register definitions
 * <table>
 * <caption>TRIGGER_MASK1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  trigger_mask1 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_MASK1 register
#define LA_TRIGGER_MASK1_ADDR   0x24E00024
/// Offset of the TRIGGER_MASK1 register from the base address
#define LA_TRIGGER_MASK1_OFFSET 0x00000024
/// Index of the TRIGGER_MASK1 register
#define LA_TRIGGER_MASK1_INDEX  0x00000009
/// Reset value of the TRIGGER_MASK1 register
#define LA_TRIGGER_MASK1_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_MASK1 register.
 * The TRIGGER_MASK1 register will be read and its value returned.
 * @return The current value of the TRIGGER_MASK1 register.
 */
__INLINE uint32_t la_trigger_mask1_get(void)
{
    return REG_PL_RD(LA_TRIGGER_MASK1_ADDR);
}

/**
 * @brief Sets the TRIGGER_MASK1 register to a value.
 * The TRIGGER_MASK1 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_mask1_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_MASK1_ADDR, value);
}

// field definitions
/// TRIGGER_MASK_1 field mask
#define LA_TRIGGER_MASK_1_MASK   ((uint32_t)0xFFFFFFFF)
/// TRIGGER_MASK_1 field LSB position
#define LA_TRIGGER_MASK_1_LSB    0
/// TRIGGER_MASK_1 field width
#define LA_TRIGGER_MASK_1_WIDTH  ((uint32_t)0x00000020)

/// TRIGGER_MASK_1 field reset value
#define LA_TRIGGER_MASK_1_RST    0x0

/**
 * @brief Returns the current value of the trigger_mask1 field in the TRIGGER_MASK1 register.
 *
 * The TRIGGER_MASK1 register will be read and the trigger_mask1 field's value will be returned.
 *
 * @return The current value of the trigger_mask1 field in the TRIGGER_MASK1 register.
 */
__INLINE uint32_t la_trigger_mask_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_MASK1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_mask1 field of the TRIGGER_MASK1 register.
 *
 * The TRIGGER_MASK1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggermask1 - The value to set the field to.
 */
__INLINE void la_trigger_mask_1_setf(uint32_t triggermask1)
{
    ASSERT_ERR((((uint32_t)triggermask1 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_MASK1_ADDR, (uint32_t)triggermask1 << 0);
}

/// @}

/**
 * @name TRIGGER_MASK2 register definitions
 * <table>
 * <caption>TRIGGER_MASK2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  trigger_mask2 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_MASK2 register
#define LA_TRIGGER_MASK2_ADDR   0x24E00028
/// Offset of the TRIGGER_MASK2 register from the base address
#define LA_TRIGGER_MASK2_OFFSET 0x00000028
/// Index of the TRIGGER_MASK2 register
#define LA_TRIGGER_MASK2_INDEX  0x0000000A
/// Reset value of the TRIGGER_MASK2 register
#define LA_TRIGGER_MASK2_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_MASK2 register.
 * The TRIGGER_MASK2 register will be read and its value returned.
 * @return The current value of the TRIGGER_MASK2 register.
 */
__INLINE uint32_t la_trigger_mask2_get(void)
{
    return REG_PL_RD(LA_TRIGGER_MASK2_ADDR);
}

/**
 * @brief Sets the TRIGGER_MASK2 register to a value.
 * The TRIGGER_MASK2 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_mask2_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_MASK2_ADDR, value);
}

// field definitions
/// TRIGGER_MASK_2 field mask
#define LA_TRIGGER_MASK_2_MASK   ((uint32_t)0xFFFFFFFF)
/// TRIGGER_MASK_2 field LSB position
#define LA_TRIGGER_MASK_2_LSB    0
/// TRIGGER_MASK_2 field width
#define LA_TRIGGER_MASK_2_WIDTH  ((uint32_t)0x00000020)

/// TRIGGER_MASK_2 field reset value
#define LA_TRIGGER_MASK_2_RST    0x0

/**
 * @brief Returns the current value of the trigger_mask2 field in the TRIGGER_MASK2 register.
 *
 * The TRIGGER_MASK2 register will be read and the trigger_mask2 field's value will be returned.
 *
 * @return The current value of the trigger_mask2 field in the TRIGGER_MASK2 register.
 */
__INLINE uint32_t la_trigger_mask_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_MASK2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_mask2 field of the TRIGGER_MASK2 register.
 *
 * The TRIGGER_MASK2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggermask2 - The value to set the field to.
 */
__INLINE void la_trigger_mask_2_setf(uint32_t triggermask2)
{
    ASSERT_ERR((((uint32_t)triggermask2 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_MASK2_ADDR, (uint32_t)triggermask2 << 0);
}

/// @}

/**
 * @name TRIGGER_VALUE0 register definitions
 * <table>
 * <caption>TRIGGER_VALUE0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  trigger_value0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_VALUE0 register
#define LA_TRIGGER_VALUE0_ADDR   0x24E0002C
/// Offset of the TRIGGER_VALUE0 register from the base address
#define LA_TRIGGER_VALUE0_OFFSET 0x0000002C
/// Index of the TRIGGER_VALUE0 register
#define LA_TRIGGER_VALUE0_INDEX  0x0000000B
/// Reset value of the TRIGGER_VALUE0 register
#define LA_TRIGGER_VALUE0_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_VALUE0 register.
 * The TRIGGER_VALUE0 register will be read and its value returned.
 * @return The current value of the TRIGGER_VALUE0 register.
 */
__INLINE uint32_t la_trigger_value0_get(void)
{
    return REG_PL_RD(LA_TRIGGER_VALUE0_ADDR);
}

/**
 * @brief Sets the TRIGGER_VALUE0 register to a value.
 * The TRIGGER_VALUE0 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_value0_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_VALUE0_ADDR, value);
}

// field definitions
/// TRIGGER_VALUE_0 field mask
#define LA_TRIGGER_VALUE_0_MASK   ((uint32_t)0xFFFFFFFF)
/// TRIGGER_VALUE_0 field LSB position
#define LA_TRIGGER_VALUE_0_LSB    0
/// TRIGGER_VALUE_0 field width
#define LA_TRIGGER_VALUE_0_WIDTH  ((uint32_t)0x00000020)

/// TRIGGER_VALUE_0 field reset value
#define LA_TRIGGER_VALUE_0_RST    0x0

/**
 * @brief Returns the current value of the trigger_value0 field in the TRIGGER_VALUE0 register.
 *
 * The TRIGGER_VALUE0 register will be read and the trigger_value0 field's value will be returned.
 *
 * @return The current value of the trigger_value0 field in the TRIGGER_VALUE0 register.
 */
__INLINE uint32_t la_trigger_value_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_VALUE0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_value0 field of the TRIGGER_VALUE0 register.
 *
 * The TRIGGER_VALUE0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggervalue0 - The value to set the field to.
 */
__INLINE void la_trigger_value_0_setf(uint32_t triggervalue0)
{
    ASSERT_ERR((((uint32_t)triggervalue0 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_VALUE0_ADDR, (uint32_t)triggervalue0 << 0);
}

/// @}

/**
 * @name TRIGGER_VALUE1 register definitions
 * <table>
 * <caption>TRIGGER_VALUE1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  trigger_value1 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_VALUE1 register
#define LA_TRIGGER_VALUE1_ADDR   0x24E00030
/// Offset of the TRIGGER_VALUE1 register from the base address
#define LA_TRIGGER_VALUE1_OFFSET 0x00000030
/// Index of the TRIGGER_VALUE1 register
#define LA_TRIGGER_VALUE1_INDEX  0x0000000C
/// Reset value of the TRIGGER_VALUE1 register
#define LA_TRIGGER_VALUE1_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_VALUE1 register.
 * The TRIGGER_VALUE1 register will be read and its value returned.
 * @return The current value of the TRIGGER_VALUE1 register.
 */
__INLINE uint32_t la_trigger_value1_get(void)
{
    return REG_PL_RD(LA_TRIGGER_VALUE1_ADDR);
}

/**
 * @brief Sets the TRIGGER_VALUE1 register to a value.
 * The TRIGGER_VALUE1 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_value1_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_VALUE1_ADDR, value);
}

// field definitions
/// TRIGGER_VALUE_1 field mask
#define LA_TRIGGER_VALUE_1_MASK   ((uint32_t)0xFFFFFFFF)
/// TRIGGER_VALUE_1 field LSB position
#define LA_TRIGGER_VALUE_1_LSB    0
/// TRIGGER_VALUE_1 field width
#define LA_TRIGGER_VALUE_1_WIDTH  ((uint32_t)0x00000020)

/// TRIGGER_VALUE_1 field reset value
#define LA_TRIGGER_VALUE_1_RST    0x0

/**
 * @brief Returns the current value of the trigger_value1 field in the TRIGGER_VALUE1 register.
 *
 * The TRIGGER_VALUE1 register will be read and the trigger_value1 field's value will be returned.
 *
 * @return The current value of the trigger_value1 field in the TRIGGER_VALUE1 register.
 */
__INLINE uint32_t la_trigger_value_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_VALUE1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_value1 field of the TRIGGER_VALUE1 register.
 *
 * The TRIGGER_VALUE1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggervalue1 - The value to set the field to.
 */
__INLINE void la_trigger_value_1_setf(uint32_t triggervalue1)
{
    ASSERT_ERR((((uint32_t)triggervalue1 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_VALUE1_ADDR, (uint32_t)triggervalue1 << 0);
}

/// @}

/**
 * @name TRIGGER_VALUE2 register definitions
 * <table>
 * <caption>TRIGGER_VALUE2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td>  trigger_value2 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_VALUE2 register
#define LA_TRIGGER_VALUE2_ADDR   0x24E00034
/// Offset of the TRIGGER_VALUE2 register from the base address
#define LA_TRIGGER_VALUE2_OFFSET 0x00000034
/// Index of the TRIGGER_VALUE2 register
#define LA_TRIGGER_VALUE2_INDEX  0x0000000D
/// Reset value of the TRIGGER_VALUE2 register
#define LA_TRIGGER_VALUE2_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_VALUE2 register.
 * The TRIGGER_VALUE2 register will be read and its value returned.
 * @return The current value of the TRIGGER_VALUE2 register.
 */
__INLINE uint32_t la_trigger_value2_get(void)
{
    return REG_PL_RD(LA_TRIGGER_VALUE2_ADDR);
}

/**
 * @brief Sets the TRIGGER_VALUE2 register to a value.
 * The TRIGGER_VALUE2 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_value2_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_VALUE2_ADDR, value);
}

// field definitions
/// TRIGGER_VALUE_2 field mask
#define LA_TRIGGER_VALUE_2_MASK   ((uint32_t)0xFFFFFFFF)
/// TRIGGER_VALUE_2 field LSB position
#define LA_TRIGGER_VALUE_2_LSB    0
/// TRIGGER_VALUE_2 field width
#define LA_TRIGGER_VALUE_2_WIDTH  ((uint32_t)0x00000020)

/// TRIGGER_VALUE_2 field reset value
#define LA_TRIGGER_VALUE_2_RST    0x0

/**
 * @brief Returns the current value of the trigger_value2 field in the TRIGGER_VALUE2 register.
 *
 * The TRIGGER_VALUE2 register will be read and the trigger_value2 field's value will be returned.
 *
 * @return The current value of the trigger_value2 field in the TRIGGER_VALUE2 register.
 */
__INLINE uint32_t la_trigger_value_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_VALUE2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_value2 field of the TRIGGER_VALUE2 register.
 *
 * The TRIGGER_VALUE2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggervalue2 - The value to set the field to.
 */
__INLINE void la_trigger_value_2_setf(uint32_t triggervalue2)
{
    ASSERT_ERR((((uint32_t)triggervalue2 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_VALUE2_ADDR, (uint32_t)triggervalue2 << 0);
}

/// @}

/**
 * @name TRIGGER_POINT register definitions
 * <table>
 * <caption>TRIGGER_POINT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td> trigger_point <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_POINT register
#define LA_TRIGGER_POINT_ADDR   0x24E00038
/// Offset of the TRIGGER_POINT register from the base address
#define LA_TRIGGER_POINT_OFFSET 0x00000038
/// Index of the TRIGGER_POINT register
#define LA_TRIGGER_POINT_INDEX  0x0000000E
/// Reset value of the TRIGGER_POINT register
#define LA_TRIGGER_POINT_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_POINT register.
 * The TRIGGER_POINT register will be read and its value returned.
 * @return The current value of the TRIGGER_POINT register.
 */
__INLINE uint32_t la_trigger_point_get(void)
{
    return REG_PL_RD(LA_TRIGGER_POINT_ADDR);
}

/**
 * @brief Sets the TRIGGER_POINT register to a value.
 * The TRIGGER_POINT register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_point_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_POINT_ADDR, value);
}

// field definitions
/// TRIGGER_POINT field mask
#define LA_TRIGGER_POINT_MASK   ((uint32_t)0x0000FFFF)
/// TRIGGER_POINT field LSB position
#define LA_TRIGGER_POINT_LSB    0
/// TRIGGER_POINT field width
#define LA_TRIGGER_POINT_WIDTH  ((uint32_t)0x00000010)

/// TRIGGER_POINT field reset value
#define LA_TRIGGER_POINT_RST    0x0

/**
 * @brief Returns the current value of the trigger_point field in the TRIGGER_POINT register.
 *
 * The TRIGGER_POINT register will be read and the trigger_point field's value will be returned.
 *
 * @return The current value of the trigger_point field in the TRIGGER_POINT register.
 */
__INLINE uint16_t la_trigger_point_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_POINT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_point field of the TRIGGER_POINT register.
 *
 * The TRIGGER_POINT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggerpoint - The value to set the field to.
 */
__INLINE void la_trigger_point_setf(uint16_t triggerpoint)
{
    ASSERT_ERR((((uint32_t)triggerpoint << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_POINT_ADDR, (uint32_t)triggerpoint << 0);
}

/// @}

/**
 * @name FIRSTSAMPLE register definitions
 * <table>
 * <caption>FIRSTSAMPLE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td> firstsample <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FIRSTSAMPLE register
#define LA_FIRSTSAMPLE_ADDR   0x24E0003C
/// Offset of the FIRSTSAMPLE register from the base address
#define LA_FIRSTSAMPLE_OFFSET 0x0000003C
/// Index of the FIRSTSAMPLE register
#define LA_FIRSTSAMPLE_INDEX  0x0000000F
/// Reset value of the FIRSTSAMPLE register
#define LA_FIRSTSAMPLE_RESET  0x00000000

/**
 * @brief Returns the current value of the FIRSTSAMPLE register.
 * The FIRSTSAMPLE register will be read and its value returned.
 * @return The current value of the FIRSTSAMPLE register.
 */
__INLINE uint32_t la_firstsample_get(void)
{
    return REG_PL_RD(LA_FIRSTSAMPLE_ADDR);
}

// field definitions
/// FIRSTSAMPLE field mask
#define LA_FIRSTSAMPLE_MASK   ((uint32_t)0x0000FFFF)
/// FIRSTSAMPLE field LSB position
#define LA_FIRSTSAMPLE_LSB    0
/// FIRSTSAMPLE field width
#define LA_FIRSTSAMPLE_WIDTH  ((uint32_t)0x00000010)

/// FIRSTSAMPLE field reset value
#define LA_FIRSTSAMPLE_RST    0x0

/**
 * @brief Returns the current value of the firstsample field in the FIRSTSAMPLE register.
 *
 * The FIRSTSAMPLE register will be read and the firstsample field's value will be returned.
 *
 * @return The current value of the firstsample field in the FIRSTSAMPLE register.
 */
__INLINE uint16_t la_firstsample_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_FIRSTSAMPLE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name SAMPLING_MASK3 register definitions
 * <table>
 * <caption>SAMPLING_MASK3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td>  sampling_mask3 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SAMPLING_MASK3 register
#define LA_SAMPLING_MASK3_ADDR   0x24E00040
/// Offset of the SAMPLING_MASK3 register from the base address
#define LA_SAMPLING_MASK3_OFFSET 0x00000040
/// Index of the SAMPLING_MASK3 register
#define LA_SAMPLING_MASK3_INDEX  0x00000010
/// Reset value of the SAMPLING_MASK3 register
#define LA_SAMPLING_MASK3_RESET  0x00000000

/**
 * @brief Returns the current value of the SAMPLING_MASK3 register.
 * The SAMPLING_MASK3 register will be read and its value returned.
 * @return The current value of the SAMPLING_MASK3 register.
 */
__INLINE uint32_t la_sampling_mask3_get(void)
{
    return REG_PL_RD(LA_SAMPLING_MASK3_ADDR);
}

/**
 * @brief Sets the SAMPLING_MASK3 register to a value.
 * The SAMPLING_MASK3 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_sampling_mask3_set(uint32_t value)
{
    REG_PL_WR(LA_SAMPLING_MASK3_ADDR, value);
}

// field definitions
/// SAMPLING_MASK_3 field mask
#define LA_SAMPLING_MASK_3_MASK   ((uint32_t)0x0000FFFF)
/// SAMPLING_MASK_3 field LSB position
#define LA_SAMPLING_MASK_3_LSB    0
/// SAMPLING_MASK_3 field width
#define LA_SAMPLING_MASK_3_WIDTH  ((uint32_t)0x00000010)

/// SAMPLING_MASK_3 field reset value
#define LA_SAMPLING_MASK_3_RST    0x0

/**
 * @brief Returns the current value of the sampling_mask3 field in the SAMPLING_MASK3 register.
 *
 * The SAMPLING_MASK3 register will be read and the sampling_mask3 field's value will be returned.
 *
 * @return The current value of the sampling_mask3 field in the SAMPLING_MASK3 register.
 */
__INLINE uint16_t la_sampling_mask_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_SAMPLING_MASK3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the sampling_mask3 field of the SAMPLING_MASK3 register.
 *
 * The SAMPLING_MASK3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] samplingmask3 - The value to set the field to.
 */
__INLINE void la_sampling_mask_3_setf(uint16_t samplingmask3)
{
    ASSERT_ERR((((uint32_t)samplingmask3 << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(LA_SAMPLING_MASK3_ADDR, (uint32_t)samplingmask3 << 0);
}

/// @}

/**
 * @name TRIGGER_MASK3 register definitions
 * <table>
 * <caption>TRIGGER_MASK3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td>  trigger_mask3 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_MASK3 register
#define LA_TRIGGER_MASK3_ADDR   0x24E00044
/// Offset of the TRIGGER_MASK3 register from the base address
#define LA_TRIGGER_MASK3_OFFSET 0x00000044
/// Index of the TRIGGER_MASK3 register
#define LA_TRIGGER_MASK3_INDEX  0x00000011
/// Reset value of the TRIGGER_MASK3 register
#define LA_TRIGGER_MASK3_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_MASK3 register.
 * The TRIGGER_MASK3 register will be read and its value returned.
 * @return The current value of the TRIGGER_MASK3 register.
 */
__INLINE uint32_t la_trigger_mask3_get(void)
{
    return REG_PL_RD(LA_TRIGGER_MASK3_ADDR);
}

/**
 * @brief Sets the TRIGGER_MASK3 register to a value.
 * The TRIGGER_MASK3 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_mask3_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_MASK3_ADDR, value);
}

// field definitions
/// TRIGGER_MASK_3 field mask
#define LA_TRIGGER_MASK_3_MASK   ((uint32_t)0x0000FFFF)
/// TRIGGER_MASK_3 field LSB position
#define LA_TRIGGER_MASK_3_LSB    0
/// TRIGGER_MASK_3 field width
#define LA_TRIGGER_MASK_3_WIDTH  ((uint32_t)0x00000010)

/// TRIGGER_MASK_3 field reset value
#define LA_TRIGGER_MASK_3_RST    0x0

/**
 * @brief Returns the current value of the trigger_mask3 field in the TRIGGER_MASK3 register.
 *
 * The TRIGGER_MASK3 register will be read and the trigger_mask3 field's value will be returned.
 *
 * @return The current value of the trigger_mask3 field in the TRIGGER_MASK3 register.
 */
__INLINE uint16_t la_trigger_mask_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_MASK3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_mask3 field of the TRIGGER_MASK3 register.
 *
 * The TRIGGER_MASK3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggermask3 - The value to set the field to.
 */
__INLINE void la_trigger_mask_3_setf(uint16_t triggermask3)
{
    ASSERT_ERR((((uint32_t)triggermask3 << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_MASK3_ADDR, (uint32_t)triggermask3 << 0);
}

/// @}

/**
 * @name TRIGGER_VALUE3 register definitions
 * <table>
 * <caption>TRIGGER_VALUE3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td>  trigger_value3 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER_VALUE3 register
#define LA_TRIGGER_VALUE3_ADDR   0x24E00048
/// Offset of the TRIGGER_VALUE3 register from the base address
#define LA_TRIGGER_VALUE3_OFFSET 0x00000048
/// Index of the TRIGGER_VALUE3 register
#define LA_TRIGGER_VALUE3_INDEX  0x00000012
/// Reset value of the TRIGGER_VALUE3 register
#define LA_TRIGGER_VALUE3_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER_VALUE3 register.
 * The TRIGGER_VALUE3 register will be read and its value returned.
 * @return The current value of the TRIGGER_VALUE3 register.
 */
__INLINE uint32_t la_trigger_value3_get(void)
{
    return REG_PL_RD(LA_TRIGGER_VALUE3_ADDR);
}

/**
 * @brief Sets the TRIGGER_VALUE3 register to a value.
 * The TRIGGER_VALUE3 register will be written.
 * @param value - The value to write.
 */
__INLINE void la_trigger_value3_set(uint32_t value)
{
    REG_PL_WR(LA_TRIGGER_VALUE3_ADDR, value);
}

// field definitions
/// TRIGGER_VALUE_3 field mask
#define LA_TRIGGER_VALUE_3_MASK   ((uint32_t)0x0000FFFF)
/// TRIGGER_VALUE_3 field LSB position
#define LA_TRIGGER_VALUE_3_LSB    0
/// TRIGGER_VALUE_3 field width
#define LA_TRIGGER_VALUE_3_WIDTH  ((uint32_t)0x00000010)

/// TRIGGER_VALUE_3 field reset value
#define LA_TRIGGER_VALUE_3_RST    0x0

/**
 * @brief Returns the current value of the trigger_value3 field in the TRIGGER_VALUE3 register.
 *
 * The TRIGGER_VALUE3 register will be read and the trigger_value3 field's value will be returned.
 *
 * @return The current value of the trigger_value3 field in the TRIGGER_VALUE3 register.
 */
__INLINE uint16_t la_trigger_value_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(LA_TRIGGER_VALUE3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the trigger_value3 field of the TRIGGER_VALUE3 register.
 *
 * The TRIGGER_VALUE3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggervalue3 - The value to set the field to.
 */
__INLINE void la_trigger_value_3_setf(uint16_t triggervalue3)
{
    ASSERT_ERR((((uint32_t)triggervalue3 << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(LA_TRIGGER_VALUE3_ADDR, (uint32_t)triggervalue3 << 0);
}

/// @}


#endif // _REG_LA_H_

/// @}

