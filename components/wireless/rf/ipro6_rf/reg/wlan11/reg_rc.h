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
 * @file reg_rc.h
 * @brief Definitions of the RC HW block registers and register access functions.
 *
 * @defgroup REG_RC REG_RC
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the RC HW block registers and register access functions.
 */
#ifndef _REG_RC_H_
#define _REG_RC_H_

#include "co_int.h"
#include "_reg_rc.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_RC peripheral.
 */
#define REG_RC_COUNT 16

/** @brief Decoding mask of the REG_RC peripheral registers from the CPU point of view.
 */
#define REG_RC_DECODING_MASK 0x0000003F

/**
 * @name STATIC_CFG register definitions
 * <table>
 * <caption>STATIC_CFG bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:28 <td>    HW_PRESCALER <td> R <td> R/W <td> 0x0
 * <tr><td> 24    <td> AUTO_TX_GAIN_EN <td> R <td> R/W <td> 0
 * <tr><td> 20    <td>   RF_FORCE_TXON <td> R <td> R/W <td> 0
 * <tr><td> 16    <td>     RF_TESTMODE <td> R <td> R/W <td> 0
 * <tr><td> 08    <td>      RF_SEL_2G4 <td> R <td> R/W <td> 0
 * <tr><td> 04    <td>     SAMEANTCTRL <td> R <td> R/W <td> 1
 * <tr><td> 00    <td>      RF_RESET_N <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the STATIC_CFG register
#define RC_STATIC_CFG_ADDR   0x24C0C000
/// Offset of the STATIC_CFG register from the base address
#define RC_STATIC_CFG_OFFSET 0x00000000
/// Index of the STATIC_CFG register
#define RC_STATIC_CFG_INDEX  0x00000000
/// Reset value of the STATIC_CFG register
#define RC_STATIC_CFG_RESET  0x00000010

/**
 * @brief Returns the current value of the STATIC_CFG register.
 * The STATIC_CFG register will be read and its value returned.
 * @return The current value of the STATIC_CFG register.
 */
__INLINE uint32_t rc_static_cfg_get(void)
{
    return REG_PL_RD(RC_STATIC_CFG_ADDR);
}

/**
 * @brief Sets the STATIC_CFG register to a value.
 * The STATIC_CFG register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_static_cfg_set(uint32_t value)
{
    REG_PL_WR(RC_STATIC_CFG_ADDR, value);
}

// field definitions
/// HW_PRESCALER field mask
#define RC_HW_PRESCALER_MASK      ((uint32_t)0xF0000000)
/// HW_PRESCALER field LSB position
#define RC_HW_PRESCALER_LSB       28
/// HW_PRESCALER field width
#define RC_HW_PRESCALER_WIDTH     ((uint32_t)0x00000004)
/// AUTO_TX_GAIN_EN field bit
#define RC_AUTO_TX_GAIN_EN_BIT    ((uint32_t)0x01000000)
/// AUTO_TX_GAIN_EN field position
#define RC_AUTO_TX_GAIN_EN_POS    24
/// RF_FORCE_TXON field bit
#define RC_RF_FORCE_TXON_BIT      ((uint32_t)0x00100000)
/// RF_FORCE_TXON field position
#define RC_RF_FORCE_TXON_POS      20
/// RF_TESTMODE field bit
#define RC_RF_TESTMODE_BIT        ((uint32_t)0x00010000)
/// RF_TESTMODE field position
#define RC_RF_TESTMODE_POS        16
/// RF_SEL_2G4 field bit
#define RC_RF_SEL_2G4_BIT         ((uint32_t)0x00000100)
/// RF_SEL_2G4 field position
#define RC_RF_SEL_2G4_POS         8
/// SAMEANTCTRL field bit
#define RC_SAMEANTCTRL_BIT        ((uint32_t)0x00000010)
/// SAMEANTCTRL field position
#define RC_SAMEANTCTRL_POS        4
/// RF_RESET_N field bit
#define RC_RF_RESET_N_BIT         ((uint32_t)0x00000001)
/// RF_RESET_N field position
#define RC_RF_RESET_N_POS         0

/// HW_PRESCALER field reset value
#define RC_HW_PRESCALER_RST       0x0
/// AUTO_TX_GAIN_EN field reset value
#define RC_AUTO_TX_GAIN_EN_RST    0x0
/// RF_FORCE_TXON field reset value
#define RC_RF_FORCE_TXON_RST      0x0
/// RF_TESTMODE field reset value
#define RC_RF_TESTMODE_RST        0x0
/// RF_SEL_2G4 field reset value
#define RC_RF_SEL_2G4_RST         0x0
/// SAMEANTCTRL field reset value
#define RC_SAMEANTCTRL_RST        0x1
/// RF_RESET_N field reset value
#define RC_RF_RESET_N_RST         0x0

/**
 * @brief Constructs a value for the STATIC_CFG register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] hwprescaler - The value to use for the HW_PRESCALER field.
 * @param[in] autotxgainen - The value to use for the AUTO_TX_GAIN_EN field.
 * @param[in] rfforcetxon - The value to use for the RF_FORCE_TXON field.
 * @param[in] rftestmode - The value to use for the RF_TESTMODE field.
 * @param[in] rfsel2g4 - The value to use for the RF_SEL_2G4 field.
 * @param[in] sameantctrl - The value to use for the SAMEANTCTRL field.
 * @param[in] rfresetn - The value to use for the RF_RESET_N field.
 */
__INLINE void rc_static_cfg_pack(uint8_t hwprescaler, uint8_t autotxgainen, uint8_t rfforcetxon, uint8_t rftestmode, uint8_t rfsel2g4, uint8_t sameantctrl, uint8_t rfresetn)
{
    ASSERT_ERR((((uint32_t)hwprescaler << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)autotxgainen << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)rfforcetxon << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)rftestmode << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)rfsel2g4 << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)sameantctrl << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)rfresetn << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_STATIC_CFG_ADDR,  ((uint32_t)hwprescaler << 28) | ((uint32_t)autotxgainen << 24) | ((uint32_t)rfforcetxon << 20) | ((uint32_t)rftestmode << 16) | ((uint32_t)rfsel2g4 << 8) | ((uint32_t)sameantctrl << 4) | ((uint32_t)rfresetn << 0));
}

/**
 * @brief Unpacks STATIC_CFG's fields from current value of the STATIC_CFG register.
 *
 * Reads the STATIC_CFG register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] hwprescaler - Will be populated with the current value of this field from the register.
 * @param[out] autotxgainen - Will be populated with the current value of this field from the register.
 * @param[out] rfforcetxon - Will be populated with the current value of this field from the register.
 * @param[out] rftestmode - Will be populated with the current value of this field from the register.
 * @param[out] rfsel2g4 - Will be populated with the current value of this field from the register.
 * @param[out] sameantctrl - Will be populated with the current value of this field from the register.
 * @param[out] rfresetn - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_static_cfg_unpack(uint8_t* hwprescaler, uint8_t* autotxgainen, uint8_t* rfforcetxon, uint8_t* rftestmode, uint8_t* rfsel2g4, uint8_t* sameantctrl, uint8_t* rfresetn)
{
    uint32_t localVal = REG_PL_RD(RC_STATIC_CFG_ADDR);

    *hwprescaler = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *autotxgainen = (localVal & ((uint32_t)0x01000000)) >> 24;
    *rfforcetxon = (localVal & ((uint32_t)0x00100000)) >> 20;
    *rftestmode = (localVal & ((uint32_t)0x00010000)) >> 16;
    *rfsel2g4 = (localVal & ((uint32_t)0x00000100)) >> 8;
    *sameantctrl = (localVal & ((uint32_t)0x00000010)) >> 4;
    *rfresetn = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the HW_PRESCALER field in the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read and the HW_PRESCALER field's value will be returned.
 *
 * @return The current value of the HW_PRESCALER field in the STATIC_CFG register.
 */
__INLINE uint8_t rc_hw_prescaler_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_STATIC_CFG_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the HW_PRESCALER field of the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hwprescaler - The value to set the field to.
 */
__INLINE void rc_hw_prescaler_setf(uint8_t hwprescaler)
{
    ASSERT_ERR((((uint32_t)hwprescaler << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(RC_STATIC_CFG_ADDR, (REG_PL_RD(RC_STATIC_CFG_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)hwprescaler << 28));
}

/**
 * @brief Returns the current value of the AUTO_TX_GAIN_EN field in the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read and the AUTO_TX_GAIN_EN field's value will be returned.
 *
 * @return The current value of the AUTO_TX_GAIN_EN field in the STATIC_CFG register.
 */
__INLINE uint8_t rc_auto_tx_gain_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_STATIC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the AUTO_TX_GAIN_EN field of the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autotxgainen - The value to set the field to.
 */
__INLINE void rc_auto_tx_gain_en_setf(uint8_t autotxgainen)
{
    ASSERT_ERR((((uint32_t)autotxgainen << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(RC_STATIC_CFG_ADDR, (REG_PL_RD(RC_STATIC_CFG_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)autotxgainen << 24));
}

/**
 * @brief Returns the current value of the RF_FORCE_TXON field in the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read and the RF_FORCE_TXON field's value will be returned.
 *
 * @return The current value of the RF_FORCE_TXON field in the STATIC_CFG register.
 */
__INLINE uint8_t rc_rf_force_txon_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_STATIC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the RF_FORCE_TXON field of the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfforcetxon - The value to set the field to.
 */
__INLINE void rc_rf_force_txon_setf(uint8_t rfforcetxon)
{
    ASSERT_ERR((((uint32_t)rfforcetxon << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(RC_STATIC_CFG_ADDR, (REG_PL_RD(RC_STATIC_CFG_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)rfforcetxon << 20));
}

/**
 * @brief Returns the current value of the RF_TESTMODE field in the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read and the RF_TESTMODE field's value will be returned.
 *
 * @return The current value of the RF_TESTMODE field in the STATIC_CFG register.
 */
__INLINE uint8_t rc_rf_testmode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_STATIC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the RF_TESTMODE field of the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rftestmode - The value to set the field to.
 */
__INLINE void rc_rf_testmode_setf(uint8_t rftestmode)
{
    ASSERT_ERR((((uint32_t)rftestmode << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(RC_STATIC_CFG_ADDR, (REG_PL_RD(RC_STATIC_CFG_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)rftestmode << 16));
}

/**
 * @brief Returns the current value of the RF_SEL_2G4 field in the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read and the RF_SEL_2G4 field's value will be returned.
 *
 * @return The current value of the RF_SEL_2G4 field in the STATIC_CFG register.
 */
__INLINE uint8_t rc_rf_sel_2g4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_STATIC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the RF_SEL_2G4 field of the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfsel2g4 - The value to set the field to.
 */
__INLINE void rc_rf_sel_2g4_setf(uint8_t rfsel2g4)
{
    ASSERT_ERR((((uint32_t)rfsel2g4 << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(RC_STATIC_CFG_ADDR, (REG_PL_RD(RC_STATIC_CFG_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)rfsel2g4 << 8));
}

/**
 * @brief Returns the current value of the SAMEANTCTRL field in the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read and the SAMEANTCTRL field's value will be returned.
 *
 * @return The current value of the SAMEANTCTRL field in the STATIC_CFG register.
 */
__INLINE uint8_t rc_sameantctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_STATIC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the SAMEANTCTRL field of the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sameantctrl - The value to set the field to.
 */
__INLINE void rc_sameantctrl_setf(uint8_t sameantctrl)
{
    ASSERT_ERR((((uint32_t)sameantctrl << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(RC_STATIC_CFG_ADDR, (REG_PL_RD(RC_STATIC_CFG_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)sameantctrl << 4));
}

/**
 * @brief Returns the current value of the RF_RESET_N field in the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read and the RF_RESET_N field's value will be returned.
 *
 * @return The current value of the RF_RESET_N field in the STATIC_CFG register.
 */
__INLINE uint8_t rc_rf_reset_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_STATIC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the RF_RESET_N field of the STATIC_CFG register.
 *
 * The STATIC_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfresetn - The value to set the field to.
 */
__INLINE void rc_rf_reset_n_setf(uint8_t rfresetn)
{
    ASSERT_ERR((((uint32_t)rfresetn << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_STATIC_CFG_ADDR, (REG_PL_RD(RC_STATIC_CFG_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rfresetn << 0));
}

/// @}

/**
 * @name SW_CTRL register definitions
 * <table>
 * <caption>SW_CTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:28 <td> SW_PRESCALER <td> R <td> R/W <td> 0x0
 * <tr><td> 27    <td>   START_DONE <td> R/W <td> R/W <td> 0
 * <tr><td> 26    <td> READNOTWRITE <td> R <td> R/W <td> 0
 * <tr><td> 25:16 <td>      ADDRESS <td> R <td> R/W <td> 0x0
 * <tr><td> 15:00 <td>         DATA <td> R/W <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SW_CTRL register
#define RC_SW_CTRL_ADDR   0x24C0C004
/// Offset of the SW_CTRL register from the base address
#define RC_SW_CTRL_OFFSET 0x00000004
/// Index of the SW_CTRL register
#define RC_SW_CTRL_INDEX  0x00000001
/// Reset value of the SW_CTRL register
#define RC_SW_CTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the SW_CTRL register.
 * The SW_CTRL register will be read and its value returned.
 * @return The current value of the SW_CTRL register.
 */
__INLINE uint32_t rc_sw_ctrl_get(void)
{
    return REG_PL_RD(RC_SW_CTRL_ADDR);
}

/**
 * @brief Sets the SW_CTRL register to a value.
 * The SW_CTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_sw_ctrl_set(uint32_t value)
{
    REG_PL_WR(RC_SW_CTRL_ADDR, value);
}

// field definitions
/// SW_PRESCALER field mask
#define RC_SW_PRESCALER_MASK   ((uint32_t)0xF0000000)
/// SW_PRESCALER field LSB position
#define RC_SW_PRESCALER_LSB    28
/// SW_PRESCALER field width
#define RC_SW_PRESCALER_WIDTH  ((uint32_t)0x00000004)
/// START_DONE field bit
#define RC_START_DONE_BIT      ((uint32_t)0x08000000)
/// START_DONE field position
#define RC_START_DONE_POS      27
/// READNOTWRITE field bit
#define RC_READNOTWRITE_BIT    ((uint32_t)0x04000000)
/// READNOTWRITE field position
#define RC_READNOTWRITE_POS    26
/// ADDRESS field mask
#define RC_ADDRESS_MASK        ((uint32_t)0x03FF0000)
/// ADDRESS field LSB position
#define RC_ADDRESS_LSB         16
/// ADDRESS field width
#define RC_ADDRESS_WIDTH       ((uint32_t)0x0000000A)
/// DATA field mask
#define RC_DATA_MASK           ((uint32_t)0x0000FFFF)
/// DATA field LSB position
#define RC_DATA_LSB            0
/// DATA field width
#define RC_DATA_WIDTH          ((uint32_t)0x00000010)

/// SW_PRESCALER field reset value
#define RC_SW_PRESCALER_RST    0x0
/// START_DONE field reset value
#define RC_START_DONE_RST      0x0
/// READNOTWRITE field reset value
#define RC_READNOTWRITE_RST    0x0
/// ADDRESS field reset value
#define RC_ADDRESS_RST         0x0
/// DATA field reset value
#define RC_DATA_RST            0x0

/**
 * @brief Constructs a value for the SW_CTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] swprescaler - The value to use for the SW_PRESCALER field.
 * @param[in] startdone - The value to use for the START_DONE field.
 * @param[in] readnotwrite - The value to use for the READNOTWRITE field.
 * @param[in] address - The value to use for the ADDRESS field.
 * @param[in] data - The value to use for the DATA field.
 */
__INLINE void rc_sw_ctrl_pack(uint8_t swprescaler, uint8_t startdone, uint8_t readnotwrite, uint16_t address, uint16_t data)
{
    ASSERT_ERR((((uint32_t)swprescaler << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)startdone << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)readnotwrite << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)address << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)data << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(RC_SW_CTRL_ADDR,  ((uint32_t)swprescaler << 28) | ((uint32_t)startdone << 27) | ((uint32_t)readnotwrite << 26) | ((uint32_t)address << 16) | ((uint32_t)data << 0));
}

/**
 * @brief Unpacks SW_CTRL's fields from current value of the SW_CTRL register.
 *
 * Reads the SW_CTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] swprescaler - Will be populated with the current value of this field from the register.
 * @param[out] startdone - Will be populated with the current value of this field from the register.
 * @param[out] readnotwrite - Will be populated with the current value of this field from the register.
 * @param[out] address - Will be populated with the current value of this field from the register.
 * @param[out] data - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_sw_ctrl_unpack(uint8_t* swprescaler, uint8_t* startdone, uint8_t* readnotwrite, uint16_t* address, uint16_t* data)
{
    uint32_t localVal = REG_PL_RD(RC_SW_CTRL_ADDR);

    *swprescaler = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *startdone = (localVal & ((uint32_t)0x08000000)) >> 27;
    *readnotwrite = (localVal & ((uint32_t)0x04000000)) >> 26;
    *address = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *data = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the SW_PRESCALER field in the SW_CTRL register.
 *
 * The SW_CTRL register will be read and the SW_PRESCALER field's value will be returned.
 *
 * @return The current value of the SW_PRESCALER field in the SW_CTRL register.
 */
__INLINE uint8_t rc_sw_prescaler_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_SW_CTRL_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the SW_PRESCALER field of the SW_CTRL register.
 *
 * The SW_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprescaler - The value to set the field to.
 */
__INLINE void rc_sw_prescaler_setf(uint8_t swprescaler)
{
    ASSERT_ERR((((uint32_t)swprescaler << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(RC_SW_CTRL_ADDR, (REG_PL_RD(RC_SW_CTRL_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)swprescaler << 28));
}

/**
 * @brief Returns the current value of the START_DONE field in the SW_CTRL register.
 *
 * The SW_CTRL register will be read and the START_DONE field's value will be returned.
 *
 * @return The current value of the START_DONE field in the SW_CTRL register.
 */
__INLINE uint8_t rc_start_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_SW_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the START_DONE field of the SW_CTRL register.
 *
 * The SW_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] startdone - The value to set the field to.
 */
__INLINE void rc_start_done_setf(uint8_t startdone)
{
    ASSERT_ERR((((uint32_t)startdone << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(RC_SW_CTRL_ADDR, (REG_PL_RD(RC_SW_CTRL_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)startdone << 27));
}

/**
 * @brief Returns the current value of the READNOTWRITE field in the SW_CTRL register.
 *
 * The SW_CTRL register will be read and the READNOTWRITE field's value will be returned.
 *
 * @return The current value of the READNOTWRITE field in the SW_CTRL register.
 */
__INLINE uint8_t rc_readnotwrite_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_SW_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the READNOTWRITE field of the SW_CTRL register.
 *
 * The SW_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] readnotwrite - The value to set the field to.
 */
__INLINE void rc_readnotwrite_setf(uint8_t readnotwrite)
{
    ASSERT_ERR((((uint32_t)readnotwrite << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(RC_SW_CTRL_ADDR, (REG_PL_RD(RC_SW_CTRL_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)readnotwrite << 26));
}

/**
 * @brief Returns the current value of the ADDRESS field in the SW_CTRL register.
 *
 * The SW_CTRL register will be read and the ADDRESS field's value will be returned.
 *
 * @return The current value of the ADDRESS field in the SW_CTRL register.
 */
__INLINE uint16_t rc_address_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_SW_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the ADDRESS field of the SW_CTRL register.
 *
 * The SW_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] address - The value to set the field to.
 */
__INLINE void rc_address_setf(uint16_t address)
{
    ASSERT_ERR((((uint32_t)address << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_SW_CTRL_ADDR, (REG_PL_RD(RC_SW_CTRL_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)address << 16));
}

/**
 * @brief Returns the current value of the DATA field in the SW_CTRL register.
 *
 * The SW_CTRL register will be read and the DATA field's value will be returned.
 *
 * @return The current value of the DATA field in the SW_CTRL register.
 */
__INLINE uint16_t rc_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_SW_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the DATA field of the SW_CTRL register.
 *
 * The SW_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] data - The value to set the field to.
 */
__INLINE void rc_data_setf(uint16_t data)
{
    ASSERT_ERR((((uint32_t)data << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(RC_SW_CTRL_ADDR, (REG_PL_RD(RC_SW_CTRL_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)data << 0));
}

/// @}

/**
 * @name HW_TRX0_MODE register definitions
 * <table>
 * <caption>HW_TRX0_MODE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:16 <td> TX0PAMODE <td> R <td> R/W <td> 0x4
 * <tr><td> 15:12 <td>   TX0MODE <td> R <td> R/W <td> 0x3
 * <tr><td> 11:08 <td> TX0LOMODE <td> R <td> R/W <td> 0x1
 * <tr><td> 07:04 <td>   RX0MODE <td> R <td> R/W <td> 0x2
 * <tr><td> 03:00 <td> RX0LOMODE <td> R <td> R/W <td> 0x1
 * </table>
 *
 * @{
 */

/// Address of the HW_TRX0_MODE register
#define RC_HW_TRX0_MODE_ADDR   0x24C0C008
/// Offset of the HW_TRX0_MODE register from the base address
#define RC_HW_TRX0_MODE_OFFSET 0x00000008
/// Index of the HW_TRX0_MODE register
#define RC_HW_TRX0_MODE_INDEX  0x00000002
/// Reset value of the HW_TRX0_MODE register
#define RC_HW_TRX0_MODE_RESET  0x00043121

/**
 * @brief Returns the current value of the HW_TRX0_MODE register.
 * The HW_TRX0_MODE register will be read and its value returned.
 * @return The current value of the HW_TRX0_MODE register.
 */
__INLINE uint32_t rc_hw_trx0_mode_get(void)
{
    return REG_PL_RD(RC_HW_TRX0_MODE_ADDR);
}

/**
 * @brief Sets the HW_TRX0_MODE register to a value.
 * The HW_TRX0_MODE register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_trx0_mode_set(uint32_t value)
{
    REG_PL_WR(RC_HW_TRX0_MODE_ADDR, value);
}

// field definitions
/// TX0PAMODE field mask
#define RC_TX0PAMODE_MASK   ((uint32_t)0x000F0000)
/// TX0PAMODE field LSB position
#define RC_TX0PAMODE_LSB    16
/// TX0PAMODE field width
#define RC_TX0PAMODE_WIDTH  ((uint32_t)0x00000004)
/// TX0MODE field mask
#define RC_TX0MODE_MASK     ((uint32_t)0x0000F000)
/// TX0MODE field LSB position
#define RC_TX0MODE_LSB      12
/// TX0MODE field width
#define RC_TX0MODE_WIDTH    ((uint32_t)0x00000004)
/// TX0LOMODE field mask
#define RC_TX0LOMODE_MASK   ((uint32_t)0x00000F00)
/// TX0LOMODE field LSB position
#define RC_TX0LOMODE_LSB    8
/// TX0LOMODE field width
#define RC_TX0LOMODE_WIDTH  ((uint32_t)0x00000004)
/// RX0MODE field mask
#define RC_RX0MODE_MASK     ((uint32_t)0x000000F0)
/// RX0MODE field LSB position
#define RC_RX0MODE_LSB      4
/// RX0MODE field width
#define RC_RX0MODE_WIDTH    ((uint32_t)0x00000004)
/// RX0LOMODE field mask
#define RC_RX0LOMODE_MASK   ((uint32_t)0x0000000F)
/// RX0LOMODE field LSB position
#define RC_RX0LOMODE_LSB    0
/// RX0LOMODE field width
#define RC_RX0LOMODE_WIDTH  ((uint32_t)0x00000004)

/// TX0PAMODE field reset value
#define RC_TX0PAMODE_RST    0x4
/// TX0MODE field reset value
#define RC_TX0MODE_RST      0x3
/// TX0LOMODE field reset value
#define RC_TX0LOMODE_RST    0x1
/// RX0MODE field reset value
#define RC_RX0MODE_RST      0x2
/// RX0LOMODE field reset value
#define RC_RX0LOMODE_RST    0x1

/**
 * @brief Constructs a value for the HW_TRX0_MODE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tx0pamode - The value to use for the TX0PAMODE field.
 * @param[in] tx0mode - The value to use for the TX0MODE field.
 * @param[in] tx0lomode - The value to use for the TX0LOMODE field.
 * @param[in] rx0mode - The value to use for the RX0MODE field.
 * @param[in] rx0lomode - The value to use for the RX0LOMODE field.
 */
__INLINE void rc_hw_trx0_mode_pack(uint8_t tx0pamode, uint8_t tx0mode, uint8_t tx0lomode, uint8_t rx0mode, uint8_t rx0lomode)
{
    ASSERT_ERR((((uint32_t)tx0pamode << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)tx0mode << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)tx0lomode << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)rx0mode << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)rx0lomode << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(RC_HW_TRX0_MODE_ADDR,  ((uint32_t)tx0pamode << 16) | ((uint32_t)tx0mode << 12) | ((uint32_t)tx0lomode << 8) | ((uint32_t)rx0mode << 4) | ((uint32_t)rx0lomode << 0));
}

/**
 * @brief Unpacks HW_TRX0_MODE's fields from current value of the HW_TRX0_MODE register.
 *
 * Reads the HW_TRX0_MODE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tx0pamode - Will be populated with the current value of this field from the register.
 * @param[out] tx0mode - Will be populated with the current value of this field from the register.
 * @param[out] tx0lomode - Will be populated with the current value of this field from the register.
 * @param[out] rx0mode - Will be populated with the current value of this field from the register.
 * @param[out] rx0lomode - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_hw_trx0_mode_unpack(uint8_t* tx0pamode, uint8_t* tx0mode, uint8_t* tx0lomode, uint8_t* rx0mode, uint8_t* rx0lomode)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX0_MODE_ADDR);

    *tx0pamode = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *tx0mode = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *tx0lomode = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *rx0mode = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *rx0lomode = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the TX0PAMODE field in the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read and the TX0PAMODE field's value will be returned.
 *
 * @return The current value of the TX0PAMODE field in the HW_TRX0_MODE register.
 */
__INLINE uint8_t rc_tx0pamode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX0_MODE_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the TX0PAMODE field of the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx0pamode - The value to set the field to.
 */
__INLINE void rc_tx0pamode_setf(uint8_t tx0pamode)
{
    ASSERT_ERR((((uint32_t)tx0pamode << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(RC_HW_TRX0_MODE_ADDR, (REG_PL_RD(RC_HW_TRX0_MODE_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)tx0pamode << 16));
}

/**
 * @brief Returns the current value of the TX0MODE field in the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read and the TX0MODE field's value will be returned.
 *
 * @return The current value of the TX0MODE field in the HW_TRX0_MODE register.
 */
__INLINE uint8_t rc_tx0mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX0_MODE_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the TX0MODE field of the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx0mode - The value to set the field to.
 */
__INLINE void rc_tx0mode_setf(uint8_t tx0mode)
{
    ASSERT_ERR((((uint32_t)tx0mode << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(RC_HW_TRX0_MODE_ADDR, (REG_PL_RD(RC_HW_TRX0_MODE_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)tx0mode << 12));
}

/**
 * @brief Returns the current value of the TX0LOMODE field in the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read and the TX0LOMODE field's value will be returned.
 *
 * @return The current value of the TX0LOMODE field in the HW_TRX0_MODE register.
 */
__INLINE uint8_t rc_tx0lomode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX0_MODE_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the TX0LOMODE field of the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx0lomode - The value to set the field to.
 */
__INLINE void rc_tx0lomode_setf(uint8_t tx0lomode)
{
    ASSERT_ERR((((uint32_t)tx0lomode << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(RC_HW_TRX0_MODE_ADDR, (REG_PL_RD(RC_HW_TRX0_MODE_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)tx0lomode << 8));
}

/**
 * @brief Returns the current value of the RX0MODE field in the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read and the RX0MODE field's value will be returned.
 *
 * @return The current value of the RX0MODE field in the HW_TRX0_MODE register.
 */
__INLINE uint8_t rc_rx0mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX0_MODE_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the RX0MODE field of the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rx0mode - The value to set the field to.
 */
__INLINE void rc_rx0mode_setf(uint8_t rx0mode)
{
    ASSERT_ERR((((uint32_t)rx0mode << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(RC_HW_TRX0_MODE_ADDR, (REG_PL_RD(RC_HW_TRX0_MODE_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)rx0mode << 4));
}

/**
 * @brief Returns the current value of the RX0LOMODE field in the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read and the RX0LOMODE field's value will be returned.
 *
 * @return The current value of the RX0LOMODE field in the HW_TRX0_MODE register.
 */
__INLINE uint8_t rc_rx0lomode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX0_MODE_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the RX0LOMODE field of the HW_TRX0_MODE register.
 *
 * The HW_TRX0_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rx0lomode - The value to set the field to.
 */
__INLINE void rc_rx0lomode_setf(uint8_t rx0lomode)
{
    ASSERT_ERR((((uint32_t)rx0lomode << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(RC_HW_TRX0_MODE_ADDR, (REG_PL_RD(RC_HW_TRX0_MODE_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)rx0lomode << 0));
}

/// @}

/**
 * @name HW_TRX1_MODE register definitions
 * <table>
 * <caption>HW_TRX1_MODE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:16 <td> TX1PAMODE <td> R <td> R/W <td> 0x4
 * <tr><td> 15:12 <td>   TX1MODE <td> R <td> R/W <td> 0x3
 * <tr><td> 11:08 <td> TX1LOMODE <td> R <td> R/W <td> 0x1
 * <tr><td> 07:04 <td>   RX1MODE <td> R <td> R/W <td> 0x2
 * <tr><td> 03:00 <td> RX1LOMODE <td> R <td> R/W <td> 0x1
 * </table>
 *
 * @{
 */

/// Address of the HW_TRX1_MODE register
#define RC_HW_TRX1_MODE_ADDR   0x24C0C00C
/// Offset of the HW_TRX1_MODE register from the base address
#define RC_HW_TRX1_MODE_OFFSET 0x0000000C
/// Index of the HW_TRX1_MODE register
#define RC_HW_TRX1_MODE_INDEX  0x00000003
/// Reset value of the HW_TRX1_MODE register
#define RC_HW_TRX1_MODE_RESET  0x00043121

/**
 * @brief Returns the current value of the HW_TRX1_MODE register.
 * The HW_TRX1_MODE register will be read and its value returned.
 * @return The current value of the HW_TRX1_MODE register.
 */
__INLINE uint32_t rc_hw_trx1_mode_get(void)
{
    return REG_PL_RD(RC_HW_TRX1_MODE_ADDR);
}

/**
 * @brief Sets the HW_TRX1_MODE register to a value.
 * The HW_TRX1_MODE register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_trx1_mode_set(uint32_t value)
{
    REG_PL_WR(RC_HW_TRX1_MODE_ADDR, value);
}

// field definitions
/// TX1PAMODE field mask
#define RC_TX1PAMODE_MASK   ((uint32_t)0x000F0000)
/// TX1PAMODE field LSB position
#define RC_TX1PAMODE_LSB    16
/// TX1PAMODE field width
#define RC_TX1PAMODE_WIDTH  ((uint32_t)0x00000004)
/// TX1MODE field mask
#define RC_TX1MODE_MASK     ((uint32_t)0x0000F000)
/// TX1MODE field LSB position
#define RC_TX1MODE_LSB      12
/// TX1MODE field width
#define RC_TX1MODE_WIDTH    ((uint32_t)0x00000004)
/// TX1LOMODE field mask
#define RC_TX1LOMODE_MASK   ((uint32_t)0x00000F00)
/// TX1LOMODE field LSB position
#define RC_TX1LOMODE_LSB    8
/// TX1LOMODE field width
#define RC_TX1LOMODE_WIDTH  ((uint32_t)0x00000004)
/// RX1MODE field mask
#define RC_RX1MODE_MASK     ((uint32_t)0x000000F0)
/// RX1MODE field LSB position
#define RC_RX1MODE_LSB      4
/// RX1MODE field width
#define RC_RX1MODE_WIDTH    ((uint32_t)0x00000004)
/// RX1LOMODE field mask
#define RC_RX1LOMODE_MASK   ((uint32_t)0x0000000F)
/// RX1LOMODE field LSB position
#define RC_RX1LOMODE_LSB    0
/// RX1LOMODE field width
#define RC_RX1LOMODE_WIDTH  ((uint32_t)0x00000004)

/// TX1PAMODE field reset value
#define RC_TX1PAMODE_RST    0x4
/// TX1MODE field reset value
#define RC_TX1MODE_RST      0x3
/// TX1LOMODE field reset value
#define RC_TX1LOMODE_RST    0x1
/// RX1MODE field reset value
#define RC_RX1MODE_RST      0x2
/// RX1LOMODE field reset value
#define RC_RX1LOMODE_RST    0x1

/**
 * @brief Constructs a value for the HW_TRX1_MODE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tx1pamode - The value to use for the TX1PAMODE field.
 * @param[in] tx1mode - The value to use for the TX1MODE field.
 * @param[in] tx1lomode - The value to use for the TX1LOMODE field.
 * @param[in] rx1mode - The value to use for the RX1MODE field.
 * @param[in] rx1lomode - The value to use for the RX1LOMODE field.
 */
__INLINE void rc_hw_trx1_mode_pack(uint8_t tx1pamode, uint8_t tx1mode, uint8_t tx1lomode, uint8_t rx1mode, uint8_t rx1lomode)
{
    ASSERT_ERR((((uint32_t)tx1pamode << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)tx1mode << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)tx1lomode << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)rx1mode << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)rx1lomode << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(RC_HW_TRX1_MODE_ADDR,  ((uint32_t)tx1pamode << 16) | ((uint32_t)tx1mode << 12) | ((uint32_t)tx1lomode << 8) | ((uint32_t)rx1mode << 4) | ((uint32_t)rx1lomode << 0));
}

/**
 * @brief Unpacks HW_TRX1_MODE's fields from current value of the HW_TRX1_MODE register.
 *
 * Reads the HW_TRX1_MODE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tx1pamode - Will be populated with the current value of this field from the register.
 * @param[out] tx1mode - Will be populated with the current value of this field from the register.
 * @param[out] tx1lomode - Will be populated with the current value of this field from the register.
 * @param[out] rx1mode - Will be populated with the current value of this field from the register.
 * @param[out] rx1lomode - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_hw_trx1_mode_unpack(uint8_t* tx1pamode, uint8_t* tx1mode, uint8_t* tx1lomode, uint8_t* rx1mode, uint8_t* rx1lomode)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX1_MODE_ADDR);

    *tx1pamode = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *tx1mode = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *tx1lomode = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *rx1mode = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *rx1lomode = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the TX1PAMODE field in the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read and the TX1PAMODE field's value will be returned.
 *
 * @return The current value of the TX1PAMODE field in the HW_TRX1_MODE register.
 */
__INLINE uint8_t rc_tx1pamode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX1_MODE_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the TX1PAMODE field of the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx1pamode - The value to set the field to.
 */
__INLINE void rc_tx1pamode_setf(uint8_t tx1pamode)
{
    ASSERT_ERR((((uint32_t)tx1pamode << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(RC_HW_TRX1_MODE_ADDR, (REG_PL_RD(RC_HW_TRX1_MODE_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)tx1pamode << 16));
}

/**
 * @brief Returns the current value of the TX1MODE field in the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read and the TX1MODE field's value will be returned.
 *
 * @return The current value of the TX1MODE field in the HW_TRX1_MODE register.
 */
__INLINE uint8_t rc_tx1mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX1_MODE_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the TX1MODE field of the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx1mode - The value to set the field to.
 */
__INLINE void rc_tx1mode_setf(uint8_t tx1mode)
{
    ASSERT_ERR((((uint32_t)tx1mode << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(RC_HW_TRX1_MODE_ADDR, (REG_PL_RD(RC_HW_TRX1_MODE_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)tx1mode << 12));
}

/**
 * @brief Returns the current value of the TX1LOMODE field in the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read and the TX1LOMODE field's value will be returned.
 *
 * @return The current value of the TX1LOMODE field in the HW_TRX1_MODE register.
 */
__INLINE uint8_t rc_tx1lomode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX1_MODE_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the TX1LOMODE field of the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx1lomode - The value to set the field to.
 */
__INLINE void rc_tx1lomode_setf(uint8_t tx1lomode)
{
    ASSERT_ERR((((uint32_t)tx1lomode << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(RC_HW_TRX1_MODE_ADDR, (REG_PL_RD(RC_HW_TRX1_MODE_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)tx1lomode << 8));
}

/**
 * @brief Returns the current value of the RX1MODE field in the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read and the RX1MODE field's value will be returned.
 *
 * @return The current value of the RX1MODE field in the HW_TRX1_MODE register.
 */
__INLINE uint8_t rc_rx1mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX1_MODE_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the RX1MODE field of the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rx1mode - The value to set the field to.
 */
__INLINE void rc_rx1mode_setf(uint8_t rx1mode)
{
    ASSERT_ERR((((uint32_t)rx1mode << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(RC_HW_TRX1_MODE_ADDR, (REG_PL_RD(RC_HW_TRX1_MODE_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)rx1mode << 4));
}

/**
 * @brief Returns the current value of the RX1LOMODE field in the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read and the RX1LOMODE field's value will be returned.
 *
 * @return The current value of the RX1LOMODE field in the HW_TRX1_MODE register.
 */
__INLINE uint8_t rc_rx1lomode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TRX1_MODE_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the RX1LOMODE field of the HW_TRX1_MODE register.
 *
 * The HW_TRX1_MODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rx1lomode - The value to set the field to.
 */
__INLINE void rc_rx1lomode_setf(uint8_t rx1lomode)
{
    ASSERT_ERR((((uint32_t)rx1lomode << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(RC_HW_TRX1_MODE_ADDR, (REG_PL_RD(RC_HW_TRX1_MODE_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)rx1lomode << 0));
}

/// @}

/**
 * @name HW_TX_GAIN_SPI_ADDR register definitions
 * <table>
 * <caption>HW_TX_GAIN_SPI_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:28 <td> TX1GAINOFFSET <td> R <td> R/W <td> 0x0
 * <tr><td> 27:24 <td> TX0GAINOFFSET <td> R <td> R/W <td> 0x0
 * <tr><td> 21:12 <td>   TX1GAINADDR <td> R <td> R/W <td> 0x263
 * <tr><td> 09:00 <td>   TX0GAINADDR <td> R <td> R/W <td> 0x63
 * </table>
 *
 * @{
 */

/// Address of the HW_TX_GAIN_SPI_ADDR register
#define RC_HW_TX_GAIN_SPI_ADDR_ADDR   0x24C0C010
/// Offset of the HW_TX_GAIN_SPI_ADDR register from the base address
#define RC_HW_TX_GAIN_SPI_ADDR_OFFSET 0x00000010
/// Index of the HW_TX_GAIN_SPI_ADDR register
#define RC_HW_TX_GAIN_SPI_ADDR_INDEX  0x00000004
/// Reset value of the HW_TX_GAIN_SPI_ADDR register
#define RC_HW_TX_GAIN_SPI_ADDR_RESET  0x00263063

/**
 * @brief Returns the current value of the HW_TX_GAIN_SPI_ADDR register.
 * The HW_TX_GAIN_SPI_ADDR register will be read and its value returned.
 * @return The current value of the HW_TX_GAIN_SPI_ADDR register.
 */
__INLINE uint32_t rc_hw_tx_gain_spi_addr_get(void)
{
    return REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR);
}

/**
 * @brief Sets the HW_TX_GAIN_SPI_ADDR register to a value.
 * The HW_TX_GAIN_SPI_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_tx_gain_spi_addr_set(uint32_t value)
{
    REG_PL_WR(RC_HW_TX_GAIN_SPI_ADDR_ADDR, value);
}

// field definitions
/// TX1GAINOFFSET field mask
#define RC_TX1GAINOFFSET_MASK   ((uint32_t)0xF0000000)
/// TX1GAINOFFSET field LSB position
#define RC_TX1GAINOFFSET_LSB    28
/// TX1GAINOFFSET field width
#define RC_TX1GAINOFFSET_WIDTH  ((uint32_t)0x00000004)
/// TX0GAINOFFSET field mask
#define RC_TX0GAINOFFSET_MASK   ((uint32_t)0x0F000000)
/// TX0GAINOFFSET field LSB position
#define RC_TX0GAINOFFSET_LSB    24
/// TX0GAINOFFSET field width
#define RC_TX0GAINOFFSET_WIDTH  ((uint32_t)0x00000004)
/// TX1GAINADDR field mask
#define RC_TX1GAINADDR_MASK     ((uint32_t)0x003FF000)
/// TX1GAINADDR field LSB position
#define RC_TX1GAINADDR_LSB      12
/// TX1GAINADDR field width
#define RC_TX1GAINADDR_WIDTH    ((uint32_t)0x0000000A)
/// TX0GAINADDR field mask
#define RC_TX0GAINADDR_MASK     ((uint32_t)0x000003FF)
/// TX0GAINADDR field LSB position
#define RC_TX0GAINADDR_LSB      0
/// TX0GAINADDR field width
#define RC_TX0GAINADDR_WIDTH    ((uint32_t)0x0000000A)

/// TX1GAINOFFSET field reset value
#define RC_TX1GAINOFFSET_RST    0x0
/// TX0GAINOFFSET field reset value
#define RC_TX0GAINOFFSET_RST    0x0
/// TX1GAINADDR field reset value
#define RC_TX1GAINADDR_RST      0x263
/// TX0GAINADDR field reset value
#define RC_TX0GAINADDR_RST      0x63

/**
 * @brief Constructs a value for the HW_TX_GAIN_SPI_ADDR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tx1gainoffset - The value to use for the TX1GAINOFFSET field.
 * @param[in] tx0gainoffset - The value to use for the TX0GAINOFFSET field.
 * @param[in] tx1gainaddr - The value to use for the TX1GAINADDR field.
 * @param[in] tx0gainaddr - The value to use for the TX0GAINADDR field.
 */
__INLINE void rc_hw_tx_gain_spi_addr_pack(uint8_t tx1gainoffset, uint8_t tx0gainoffset, uint16_t tx1gainaddr, uint16_t tx0gainaddr)
{
    ASSERT_ERR((((uint32_t)tx1gainoffset << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)tx0gainoffset << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)tx1gainaddr << 12) & ~((uint32_t)0x003FF000)) == 0);
    ASSERT_ERR((((uint32_t)tx0gainaddr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_TX_GAIN_SPI_ADDR_ADDR,  ((uint32_t)tx1gainoffset << 28) | ((uint32_t)tx0gainoffset << 24) | ((uint32_t)tx1gainaddr << 12) | ((uint32_t)tx0gainaddr << 0));
}

/**
 * @brief Unpacks HW_TX_GAIN_SPI_ADDR's fields from current value of the HW_TX_GAIN_SPI_ADDR register.
 *
 * Reads the HW_TX_GAIN_SPI_ADDR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tx1gainoffset - Will be populated with the current value of this field from the register.
 * @param[out] tx0gainoffset - Will be populated with the current value of this field from the register.
 * @param[out] tx1gainaddr - Will be populated with the current value of this field from the register.
 * @param[out] tx0gainaddr - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_hw_tx_gain_spi_addr_unpack(uint8_t* tx1gainoffset, uint8_t* tx0gainoffset, uint16_t* tx1gainaddr, uint16_t* tx0gainaddr)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR);

    *tx1gainoffset = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *tx0gainoffset = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *tx1gainaddr = (localVal & ((uint32_t)0x003FF000)) >> 12;
    *tx0gainaddr = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the TX1GAINOFFSET field in the HW_TX_GAIN_SPI_ADDR register.
 *
 * The HW_TX_GAIN_SPI_ADDR register will be read and the TX1GAINOFFSET field's value will be returned.
 *
 * @return The current value of the TX1GAINOFFSET field in the HW_TX_GAIN_SPI_ADDR register.
 */
__INLINE uint8_t rc_tx1gainoffset_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the TX1GAINOFFSET field of the HW_TX_GAIN_SPI_ADDR register.
 *
 * The HW_TX_GAIN_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx1gainoffset - The value to set the field to.
 */
__INLINE void rc_tx1gainoffset_setf(uint8_t tx1gainoffset)
{
    ASSERT_ERR((((uint32_t)tx1gainoffset << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(RC_HW_TX_GAIN_SPI_ADDR_ADDR, (REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)tx1gainoffset << 28));
}

/**
 * @brief Returns the current value of the TX0GAINOFFSET field in the HW_TX_GAIN_SPI_ADDR register.
 *
 * The HW_TX_GAIN_SPI_ADDR register will be read and the TX0GAINOFFSET field's value will be returned.
 *
 * @return The current value of the TX0GAINOFFSET field in the HW_TX_GAIN_SPI_ADDR register.
 */
__INLINE uint8_t rc_tx0gainoffset_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the TX0GAINOFFSET field of the HW_TX_GAIN_SPI_ADDR register.
 *
 * The HW_TX_GAIN_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx0gainoffset - The value to set the field to.
 */
__INLINE void rc_tx0gainoffset_setf(uint8_t tx0gainoffset)
{
    ASSERT_ERR((((uint32_t)tx0gainoffset << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(RC_HW_TX_GAIN_SPI_ADDR_ADDR, (REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)tx0gainoffset << 24));
}

/**
 * @brief Returns the current value of the TX1GAINADDR field in the HW_TX_GAIN_SPI_ADDR register.
 *
 * The HW_TX_GAIN_SPI_ADDR register will be read and the TX1GAINADDR field's value will be returned.
 *
 * @return The current value of the TX1GAINADDR field in the HW_TX_GAIN_SPI_ADDR register.
 */
__INLINE uint16_t rc_tx1gainaddr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x003FF000)) >> 12);
}

/**
 * @brief Sets the TX1GAINADDR field of the HW_TX_GAIN_SPI_ADDR register.
 *
 * The HW_TX_GAIN_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx1gainaddr - The value to set the field to.
 */
__INLINE void rc_tx1gainaddr_setf(uint16_t tx1gainaddr)
{
    ASSERT_ERR((((uint32_t)tx1gainaddr << 12) & ~((uint32_t)0x003FF000)) == 0);
    REG_PL_WR(RC_HW_TX_GAIN_SPI_ADDR_ADDR, (REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR) & ~((uint32_t)0x003FF000)) | ((uint32_t)tx1gainaddr << 12));
}

/**
 * @brief Returns the current value of the TX0GAINADDR field in the HW_TX_GAIN_SPI_ADDR register.
 *
 * The HW_TX_GAIN_SPI_ADDR register will be read and the TX0GAINADDR field's value will be returned.
 *
 * @return The current value of the TX0GAINADDR field in the HW_TX_GAIN_SPI_ADDR register.
 */
__INLINE uint16_t rc_tx0gainaddr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the TX0GAINADDR field of the HW_TX_GAIN_SPI_ADDR register.
 *
 * The HW_TX_GAIN_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx0gainaddr - The value to set the field to.
 */
__INLINE void rc_tx0gainaddr_setf(uint16_t tx0gainaddr)
{
    ASSERT_ERR((((uint32_t)tx0gainaddr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_TX_GAIN_SPI_ADDR_ADDR, (REG_PL_RD(RC_HW_TX_GAIN_SPI_ADDR_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)tx0gainaddr << 0));
}

/// @}

/**
 * @name HW_RX_GAIN_SPI_ADDR register definitions
 * <table>
 * <caption>HW_RX_GAIN_SPI_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:12 <td> RX1GAINADDR <td> R <td> R/W <td> 0x260
 * <tr><td> 09:00 <td> RX0GAINADDR <td> R <td> R/W <td> 0x60
 * </table>
 *
 * @{
 */

/// Address of the HW_RX_GAIN_SPI_ADDR register
#define RC_HW_RX_GAIN_SPI_ADDR_ADDR   0x24C0C014
/// Offset of the HW_RX_GAIN_SPI_ADDR register from the base address
#define RC_HW_RX_GAIN_SPI_ADDR_OFFSET 0x00000014
/// Index of the HW_RX_GAIN_SPI_ADDR register
#define RC_HW_RX_GAIN_SPI_ADDR_INDEX  0x00000005
/// Reset value of the HW_RX_GAIN_SPI_ADDR register
#define RC_HW_RX_GAIN_SPI_ADDR_RESET  0x00260060

/**
 * @brief Returns the current value of the HW_RX_GAIN_SPI_ADDR register.
 * The HW_RX_GAIN_SPI_ADDR register will be read and its value returned.
 * @return The current value of the HW_RX_GAIN_SPI_ADDR register.
 */
__INLINE uint32_t rc_hw_rx_gain_spi_addr_get(void)
{
    return REG_PL_RD(RC_HW_RX_GAIN_SPI_ADDR_ADDR);
}

/**
 * @brief Sets the HW_RX_GAIN_SPI_ADDR register to a value.
 * The HW_RX_GAIN_SPI_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_rx_gain_spi_addr_set(uint32_t value)
{
    REG_PL_WR(RC_HW_RX_GAIN_SPI_ADDR_ADDR, value);
}

// field definitions
/// RX1GAINADDR field mask
#define RC_RX1GAINADDR_MASK   ((uint32_t)0x003FF000)
/// RX1GAINADDR field LSB position
#define RC_RX1GAINADDR_LSB    12
/// RX1GAINADDR field width
#define RC_RX1GAINADDR_WIDTH  ((uint32_t)0x0000000A)
/// RX0GAINADDR field mask
#define RC_RX0GAINADDR_MASK   ((uint32_t)0x000003FF)
/// RX0GAINADDR field LSB position
#define RC_RX0GAINADDR_LSB    0
/// RX0GAINADDR field width
#define RC_RX0GAINADDR_WIDTH  ((uint32_t)0x0000000A)

/// RX1GAINADDR field reset value
#define RC_RX1GAINADDR_RST    0x260
/// RX0GAINADDR field reset value
#define RC_RX0GAINADDR_RST    0x60

/**
 * @brief Constructs a value for the HW_RX_GAIN_SPI_ADDR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rx1gainaddr - The value to use for the RX1GAINADDR field.
 * @param[in] rx0gainaddr - The value to use for the RX0GAINADDR field.
 */
__INLINE void rc_hw_rx_gain_spi_addr_pack(uint16_t rx1gainaddr, uint16_t rx0gainaddr)
{
    ASSERT_ERR((((uint32_t)rx1gainaddr << 12) & ~((uint32_t)0x003FF000)) == 0);
    ASSERT_ERR((((uint32_t)rx0gainaddr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_RX_GAIN_SPI_ADDR_ADDR,  ((uint32_t)rx1gainaddr << 12) | ((uint32_t)rx0gainaddr << 0));
}

/**
 * @brief Unpacks HW_RX_GAIN_SPI_ADDR's fields from current value of the HW_RX_GAIN_SPI_ADDR register.
 *
 * Reads the HW_RX_GAIN_SPI_ADDR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rx1gainaddr - Will be populated with the current value of this field from the register.
 * @param[out] rx0gainaddr - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_hw_rx_gain_spi_addr_unpack(uint16_t* rx1gainaddr, uint16_t* rx0gainaddr)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_GAIN_SPI_ADDR_ADDR);

    *rx1gainaddr = (localVal & ((uint32_t)0x003FF000)) >> 12;
    *rx0gainaddr = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the RX1GAINADDR field in the HW_RX_GAIN_SPI_ADDR register.
 *
 * The HW_RX_GAIN_SPI_ADDR register will be read and the RX1GAINADDR field's value will be returned.
 *
 * @return The current value of the RX1GAINADDR field in the HW_RX_GAIN_SPI_ADDR register.
 */
__INLINE uint16_t rc_rx1gainaddr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_GAIN_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x003FF000)) >> 12);
}

/**
 * @brief Sets the RX1GAINADDR field of the HW_RX_GAIN_SPI_ADDR register.
 *
 * The HW_RX_GAIN_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rx1gainaddr - The value to set the field to.
 */
__INLINE void rc_rx1gainaddr_setf(uint16_t rx1gainaddr)
{
    ASSERT_ERR((((uint32_t)rx1gainaddr << 12) & ~((uint32_t)0x003FF000)) == 0);
    REG_PL_WR(RC_HW_RX_GAIN_SPI_ADDR_ADDR, (REG_PL_RD(RC_HW_RX_GAIN_SPI_ADDR_ADDR) & ~((uint32_t)0x003FF000)) | ((uint32_t)rx1gainaddr << 12));
}

/**
 * @brief Returns the current value of the RX0GAINADDR field in the HW_RX_GAIN_SPI_ADDR register.
 *
 * The HW_RX_GAIN_SPI_ADDR register will be read and the RX0GAINADDR field's value will be returned.
 *
 * @return The current value of the RX0GAINADDR field in the HW_RX_GAIN_SPI_ADDR register.
 */
__INLINE uint16_t rc_rx0gainaddr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_GAIN_SPI_ADDR_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the RX0GAINADDR field of the HW_RX_GAIN_SPI_ADDR register.
 *
 * The HW_RX_GAIN_SPI_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rx0gainaddr - The value to set the field to.
 */
__INLINE void rc_rx0gainaddr_setf(uint16_t rx0gainaddr)
{
    ASSERT_ERR((((uint32_t)rx0gainaddr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_RX_GAIN_SPI_ADDR_ADDR, (REG_PL_RD(RC_HW_RX_GAIN_SPI_ADDR_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)rx0gainaddr << 0));
}

/// @}

/**
 * @name HW_RX_DELAY register definitions
 * <table>
 * <caption>HW_RX_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> RXON_2_LO_DELAY <td> R <td> R/W <td> 0x5
 * <tr><td> 09:00 <td> LO_2_RXON_DELAY <td> R <td> R/W <td> 0x50
 * </table>
 *
 * @{
 */

/// Address of the HW_RX_DELAY register
#define RC_HW_RX_DELAY_ADDR   0x24C0C018
/// Offset of the HW_RX_DELAY register from the base address
#define RC_HW_RX_DELAY_OFFSET 0x00000018
/// Index of the HW_RX_DELAY register
#define RC_HW_RX_DELAY_INDEX  0x00000006
/// Reset value of the HW_RX_DELAY register
#define RC_HW_RX_DELAY_RESET  0x00050050

/**
 * @brief Returns the current value of the HW_RX_DELAY register.
 * The HW_RX_DELAY register will be read and its value returned.
 * @return The current value of the HW_RX_DELAY register.
 */
__INLINE uint32_t rc_hw_rx_delay_get(void)
{
    return REG_PL_RD(RC_HW_RX_DELAY_ADDR);
}

/**
 * @brief Sets the HW_RX_DELAY register to a value.
 * The HW_RX_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_rx_delay_set(uint32_t value)
{
    REG_PL_WR(RC_HW_RX_DELAY_ADDR, value);
}

// field definitions
/// RXON_2_LO_DELAY field mask
#define RC_RXON_2_LO_DELAY_MASK   ((uint32_t)0x03FF0000)
/// RXON_2_LO_DELAY field LSB position
#define RC_RXON_2_LO_DELAY_LSB    16
/// RXON_2_LO_DELAY field width
#define RC_RXON_2_LO_DELAY_WIDTH  ((uint32_t)0x0000000A)
/// LO_2_RXON_DELAY field mask
#define RC_LO_2_RXON_DELAY_MASK   ((uint32_t)0x000003FF)
/// LO_2_RXON_DELAY field LSB position
#define RC_LO_2_RXON_DELAY_LSB    0
/// LO_2_RXON_DELAY field width
#define RC_LO_2_RXON_DELAY_WIDTH  ((uint32_t)0x0000000A)

/// RXON_2_LO_DELAY field reset value
#define RC_RXON_2_LO_DELAY_RST    0x5
/// LO_2_RXON_DELAY field reset value
#define RC_LO_2_RXON_DELAY_RST    0x50

/**
 * @brief Constructs a value for the HW_RX_DELAY register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxon2lodelay - The value to use for the RXON_2_LO_DELAY field.
 * @param[in] lo2rxondelay - The value to use for the LO_2_RXON_DELAY field.
 */
__INLINE void rc_hw_rx_delay_pack(uint16_t rxon2lodelay, uint16_t lo2rxondelay)
{
    ASSERT_ERR((((uint32_t)rxon2lodelay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)lo2rxondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_RX_DELAY_ADDR,  ((uint32_t)rxon2lodelay << 16) | ((uint32_t)lo2rxondelay << 0));
}

/**
 * @brief Unpacks HW_RX_DELAY's fields from current value of the HW_RX_DELAY register.
 *
 * Reads the HW_RX_DELAY register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxon2lodelay - Will be populated with the current value of this field from the register.
 * @param[out] lo2rxondelay - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_hw_rx_delay_unpack(uint16_t* rxon2lodelay, uint16_t* lo2rxondelay)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_DELAY_ADDR);

    *rxon2lodelay = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *lo2rxondelay = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXON_2_LO_DELAY field in the HW_RX_DELAY register.
 *
 * The HW_RX_DELAY register will be read and the RXON_2_LO_DELAY field's value will be returned.
 *
 * @return The current value of the RXON_2_LO_DELAY field in the HW_RX_DELAY register.
 */
__INLINE uint16_t rc_rxon_2_lo_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the RXON_2_LO_DELAY field of the HW_RX_DELAY register.
 *
 * The HW_RX_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxon2lodelay - The value to set the field to.
 */
__INLINE void rc_rxon_2_lo_delay_setf(uint16_t rxon2lodelay)
{
    ASSERT_ERR((((uint32_t)rxon2lodelay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_HW_RX_DELAY_ADDR, (REG_PL_RD(RC_HW_RX_DELAY_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)rxon2lodelay << 16));
}

/**
 * @brief Returns the current value of the LO_2_RXON_DELAY field in the HW_RX_DELAY register.
 *
 * The HW_RX_DELAY register will be read and the LO_2_RXON_DELAY field's value will be returned.
 *
 * @return The current value of the LO_2_RXON_DELAY field in the HW_RX_DELAY register.
 */
__INLINE uint16_t rc_lo_2_rxon_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the LO_2_RXON_DELAY field of the HW_RX_DELAY register.
 *
 * The HW_RX_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lo2rxondelay - The value to set the field to.
 */
__INLINE void rc_lo_2_rxon_delay_setf(uint16_t lo2rxondelay)
{
    ASSERT_ERR((((uint32_t)lo2rxondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_RX_DELAY_ADDR, (REG_PL_RD(RC_HW_RX_DELAY_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)lo2rxondelay << 0));
}

/// @}

/**
 * @name HW_TX_DELAY register definitions
 * <table>
 * <caption>HW_TX_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> TXON_2_LO_DELAY <td> R <td> R/W <td> 0x5
 * <tr><td> 09:00 <td> LO_2_TXON_DELAY <td> R <td> R/W <td> 0x50
 * </table>
 *
 * @{
 */

/// Address of the HW_TX_DELAY register
#define RC_HW_TX_DELAY_ADDR   0x24C0C01C
/// Offset of the HW_TX_DELAY register from the base address
#define RC_HW_TX_DELAY_OFFSET 0x0000001C
/// Index of the HW_TX_DELAY register
#define RC_HW_TX_DELAY_INDEX  0x00000007
/// Reset value of the HW_TX_DELAY register
#define RC_HW_TX_DELAY_RESET  0x00050050

/**
 * @brief Returns the current value of the HW_TX_DELAY register.
 * The HW_TX_DELAY register will be read and its value returned.
 * @return The current value of the HW_TX_DELAY register.
 */
__INLINE uint32_t rc_hw_tx_delay_get(void)
{
    return REG_PL_RD(RC_HW_TX_DELAY_ADDR);
}

/**
 * @brief Sets the HW_TX_DELAY register to a value.
 * The HW_TX_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_tx_delay_set(uint32_t value)
{
    REG_PL_WR(RC_HW_TX_DELAY_ADDR, value);
}

// field definitions
/// TXON_2_LO_DELAY field mask
#define RC_TXON_2_LO_DELAY_MASK   ((uint32_t)0x03FF0000)
/// TXON_2_LO_DELAY field LSB position
#define RC_TXON_2_LO_DELAY_LSB    16
/// TXON_2_LO_DELAY field width
#define RC_TXON_2_LO_DELAY_WIDTH  ((uint32_t)0x0000000A)
/// LO_2_TXON_DELAY field mask
#define RC_LO_2_TXON_DELAY_MASK   ((uint32_t)0x000003FF)
/// LO_2_TXON_DELAY field LSB position
#define RC_LO_2_TXON_DELAY_LSB    0
/// LO_2_TXON_DELAY field width
#define RC_LO_2_TXON_DELAY_WIDTH  ((uint32_t)0x0000000A)

/// TXON_2_LO_DELAY field reset value
#define RC_TXON_2_LO_DELAY_RST    0x5
/// LO_2_TXON_DELAY field reset value
#define RC_LO_2_TXON_DELAY_RST    0x50

/**
 * @brief Constructs a value for the HW_TX_DELAY register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txon2lodelay - The value to use for the TXON_2_LO_DELAY field.
 * @param[in] lo2txondelay - The value to use for the LO_2_TXON_DELAY field.
 */
__INLINE void rc_hw_tx_delay_pack(uint16_t txon2lodelay, uint16_t lo2txondelay)
{
    ASSERT_ERR((((uint32_t)txon2lodelay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)lo2txondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_TX_DELAY_ADDR,  ((uint32_t)txon2lodelay << 16) | ((uint32_t)lo2txondelay << 0));
}

/**
 * @brief Unpacks HW_TX_DELAY's fields from current value of the HW_TX_DELAY register.
 *
 * Reads the HW_TX_DELAY register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txon2lodelay - Will be populated with the current value of this field from the register.
 * @param[out] lo2txondelay - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_hw_tx_delay_unpack(uint16_t* txon2lodelay, uint16_t* lo2txondelay)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TX_DELAY_ADDR);

    *txon2lodelay = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *lo2txondelay = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the TXON_2_LO_DELAY field in the HW_TX_DELAY register.
 *
 * The HW_TX_DELAY register will be read and the TXON_2_LO_DELAY field's value will be returned.
 *
 * @return The current value of the TXON_2_LO_DELAY field in the HW_TX_DELAY register.
 */
__INLINE uint16_t rc_txon_2_lo_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TX_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the TXON_2_LO_DELAY field of the HW_TX_DELAY register.
 *
 * The HW_TX_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txon2lodelay - The value to set the field to.
 */
__INLINE void rc_txon_2_lo_delay_setf(uint16_t txon2lodelay)
{
    ASSERT_ERR((((uint32_t)txon2lodelay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_HW_TX_DELAY_ADDR, (REG_PL_RD(RC_HW_TX_DELAY_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)txon2lodelay << 16));
}

/**
 * @brief Returns the current value of the LO_2_TXON_DELAY field in the HW_TX_DELAY register.
 *
 * The HW_TX_DELAY register will be read and the LO_2_TXON_DELAY field's value will be returned.
 *
 * @return The current value of the LO_2_TXON_DELAY field in the HW_TX_DELAY register.
 */
__INLINE uint16_t rc_lo_2_txon_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_TX_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the LO_2_TXON_DELAY field of the HW_TX_DELAY register.
 *
 * The HW_TX_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lo2txondelay - The value to set the field to.
 */
__INLINE void rc_lo_2_txon_delay_setf(uint16_t lo2txondelay)
{
    ASSERT_ERR((((uint32_t)lo2txondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_TX_DELAY_ADDR, (REG_PL_RD(RC_HW_TX_DELAY_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)lo2txondelay << 0));
}

/// @}

/**
 * @name HW_PA_DELAY register definitions
 * <table>
 * <caption>HW_PA_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> PAOFF_DELAY <td> R <td> R/W <td> 0x5
 * <tr><td> 09:00 <td>  PAON_DELAY <td> R <td> R/W <td> 0x50
 * </table>
 *
 * @{
 */

/// Address of the HW_PA_DELAY register
#define RC_HW_PA_DELAY_ADDR   0x24C0C020
/// Offset of the HW_PA_DELAY register from the base address
#define RC_HW_PA_DELAY_OFFSET 0x00000020
/// Index of the HW_PA_DELAY register
#define RC_HW_PA_DELAY_INDEX  0x00000008
/// Reset value of the HW_PA_DELAY register
#define RC_HW_PA_DELAY_RESET  0x00050050

/**
 * @brief Returns the current value of the HW_PA_DELAY register.
 * The HW_PA_DELAY register will be read and its value returned.
 * @return The current value of the HW_PA_DELAY register.
 */
__INLINE uint32_t rc_hw_pa_delay_get(void)
{
    return REG_PL_RD(RC_HW_PA_DELAY_ADDR);
}

/**
 * @brief Sets the HW_PA_DELAY register to a value.
 * The HW_PA_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_pa_delay_set(uint32_t value)
{
    REG_PL_WR(RC_HW_PA_DELAY_ADDR, value);
}

// field definitions
/// PAOFF_DELAY field mask
#define RC_PAOFF_DELAY_MASK   ((uint32_t)0x03FF0000)
/// PAOFF_DELAY field LSB position
#define RC_PAOFF_DELAY_LSB    16
/// PAOFF_DELAY field width
#define RC_PAOFF_DELAY_WIDTH  ((uint32_t)0x0000000A)
/// PAON_DELAY field mask
#define RC_PAON_DELAY_MASK    ((uint32_t)0x000003FF)
/// PAON_DELAY field LSB position
#define RC_PAON_DELAY_LSB     0
/// PAON_DELAY field width
#define RC_PAON_DELAY_WIDTH   ((uint32_t)0x0000000A)

/// PAOFF_DELAY field reset value
#define RC_PAOFF_DELAY_RST    0x5
/// PAON_DELAY field reset value
#define RC_PAON_DELAY_RST     0x50

/**
 * @brief Constructs a value for the HW_PA_DELAY register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] paoffdelay - The value to use for the PAOFF_DELAY field.
 * @param[in] paondelay - The value to use for the PAON_DELAY field.
 */
__INLINE void rc_hw_pa_delay_pack(uint16_t paoffdelay, uint16_t paondelay)
{
    ASSERT_ERR((((uint32_t)paoffdelay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)paondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_PA_DELAY_ADDR,  ((uint32_t)paoffdelay << 16) | ((uint32_t)paondelay << 0));
}

/**
 * @brief Unpacks HW_PA_DELAY's fields from current value of the HW_PA_DELAY register.
 *
 * Reads the HW_PA_DELAY register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] paoffdelay - Will be populated with the current value of this field from the register.
 * @param[out] paondelay - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_hw_pa_delay_unpack(uint16_t* paoffdelay, uint16_t* paondelay)
{
    uint32_t localVal = REG_PL_RD(RC_HW_PA_DELAY_ADDR);

    *paoffdelay = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *paondelay = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the PAOFF_DELAY field in the HW_PA_DELAY register.
 *
 * The HW_PA_DELAY register will be read and the PAOFF_DELAY field's value will be returned.
 *
 * @return The current value of the PAOFF_DELAY field in the HW_PA_DELAY register.
 */
__INLINE uint16_t rc_paoff_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_PA_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the PAOFF_DELAY field of the HW_PA_DELAY register.
 *
 * The HW_PA_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] paoffdelay - The value to set the field to.
 */
__INLINE void rc_paoff_delay_setf(uint16_t paoffdelay)
{
    ASSERT_ERR((((uint32_t)paoffdelay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_HW_PA_DELAY_ADDR, (REG_PL_RD(RC_HW_PA_DELAY_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)paoffdelay << 16));
}

/**
 * @brief Returns the current value of the PAON_DELAY field in the HW_PA_DELAY register.
 *
 * The HW_PA_DELAY register will be read and the PAON_DELAY field's value will be returned.
 *
 * @return The current value of the PAON_DELAY field in the HW_PA_DELAY register.
 */
__INLINE uint16_t rc_paon_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_PA_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the PAON_DELAY field of the HW_PA_DELAY register.
 *
 * The HW_PA_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] paondelay - The value to set the field to.
 */
__INLINE void rc_paon_delay_setf(uint16_t paondelay)
{
    ASSERT_ERR((((uint32_t)paondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_PA_DELAY_ADDR, (REG_PL_RD(RC_HW_PA_DELAY_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)paondelay << 0));
}

/// @}

/**
 * @name HW_RX2TX_DELAY register definitions
 * <table>
 * <caption>HW_RX2TX_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 09:00 <td> RX2TXON_DELAY <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the HW_RX2TX_DELAY register
#define RC_HW_RX2TX_DELAY_ADDR   0x24C0C024
/// Offset of the HW_RX2TX_DELAY register from the base address
#define RC_HW_RX2TX_DELAY_OFFSET 0x00000024
/// Index of the HW_RX2TX_DELAY register
#define RC_HW_RX2TX_DELAY_INDEX  0x00000009
/// Reset value of the HW_RX2TX_DELAY register
#define RC_HW_RX2TX_DELAY_RESET  0x00000000

/**
 * @brief Returns the current value of the HW_RX2TX_DELAY register.
 * The HW_RX2TX_DELAY register will be read and its value returned.
 * @return The current value of the HW_RX2TX_DELAY register.
 */
__INLINE uint32_t rc_hw_rx2tx_delay_get(void)
{
    return REG_PL_RD(RC_HW_RX2TX_DELAY_ADDR);
}

/**
 * @brief Sets the HW_RX2TX_DELAY register to a value.
 * The HW_RX2TX_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_rx2tx_delay_set(uint32_t value)
{
    REG_PL_WR(RC_HW_RX2TX_DELAY_ADDR, value);
}

// field definitions
/// RX2TXON_DELAY field mask
#define RC_RX2TXON_DELAY_MASK   ((uint32_t)0x000003FF)
/// RX2TXON_DELAY field LSB position
#define RC_RX2TXON_DELAY_LSB    0
/// RX2TXON_DELAY field width
#define RC_RX2TXON_DELAY_WIDTH  ((uint32_t)0x0000000A)

/// RX2TXON_DELAY field reset value
#define RC_RX2TXON_DELAY_RST    0x0

/**
 * @brief Returns the current value of the RX2TXON_DELAY field in the HW_RX2TX_DELAY register.
 *
 * The HW_RX2TX_DELAY register will be read and the RX2TXON_DELAY field's value will be returned.
 *
 * @return The current value of the RX2TXON_DELAY field in the HW_RX2TX_DELAY register.
 */
__INLINE uint16_t rc_rx2txon_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX2TX_DELAY_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000003FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the RX2TXON_DELAY field of the HW_RX2TX_DELAY register.
 *
 * The HW_RX2TX_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rx2txondelay - The value to set the field to.
 */
__INLINE void rc_rx2txon_delay_setf(uint16_t rx2txondelay)
{
    ASSERT_ERR((((uint32_t)rx2txondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_HW_RX2TX_DELAY_ADDR, (uint32_t)rx2txondelay << 0);
}

/// @}

/**
 * @name EXTPA_SEQ_ON_DELAY register definitions
 * <table>
 * <caption>EXTPA_SEQ_ON_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:08 <td> EXTPA1_SEQ_ON_DELAY <td> R <td> R/W <td> 0x0
 * <tr><td> 07:00 <td> EXTPA0_SEQ_ON_DELAY <td> R <td> R/W <td> 0x50
 * </table>
 *
 * @{
 */

/// Address of the EXTPA_SEQ_ON_DELAY register
#define RC_EXTPA_SEQ_ON_DELAY_ADDR   0x24C0C028
/// Offset of the EXTPA_SEQ_ON_DELAY register from the base address
#define RC_EXTPA_SEQ_ON_DELAY_OFFSET 0x00000028
/// Index of the EXTPA_SEQ_ON_DELAY register
#define RC_EXTPA_SEQ_ON_DELAY_INDEX  0x0000000A
/// Reset value of the EXTPA_SEQ_ON_DELAY register
#define RC_EXTPA_SEQ_ON_DELAY_RESET  0x00000050

/**
 * @brief Returns the current value of the EXTPA_SEQ_ON_DELAY register.
 * The EXTPA_SEQ_ON_DELAY register will be read and its value returned.
 * @return The current value of the EXTPA_SEQ_ON_DELAY register.
 */
__INLINE uint32_t rc_extpa_seq_on_delay_get(void)
{
    return REG_PL_RD(RC_EXTPA_SEQ_ON_DELAY_ADDR);
}

/**
 * @brief Sets the EXTPA_SEQ_ON_DELAY register to a value.
 * The EXTPA_SEQ_ON_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_extpa_seq_on_delay_set(uint32_t value)
{
    REG_PL_WR(RC_EXTPA_SEQ_ON_DELAY_ADDR, value);
}

// field definitions
/// EXTPA1_SEQ_ON_DELAY field mask
#define RC_EXTPA1_SEQ_ON_DELAY_MASK   ((uint32_t)0x0000FF00)
/// EXTPA1_SEQ_ON_DELAY field LSB position
#define RC_EXTPA1_SEQ_ON_DELAY_LSB    8
/// EXTPA1_SEQ_ON_DELAY field width
#define RC_EXTPA1_SEQ_ON_DELAY_WIDTH  ((uint32_t)0x00000008)
/// EXTPA0_SEQ_ON_DELAY field mask
#define RC_EXTPA0_SEQ_ON_DELAY_MASK   ((uint32_t)0x000000FF)
/// EXTPA0_SEQ_ON_DELAY field LSB position
#define RC_EXTPA0_SEQ_ON_DELAY_LSB    0
/// EXTPA0_SEQ_ON_DELAY field width
#define RC_EXTPA0_SEQ_ON_DELAY_WIDTH  ((uint32_t)0x00000008)

/// EXTPA1_SEQ_ON_DELAY field reset value
#define RC_EXTPA1_SEQ_ON_DELAY_RST    0x0
/// EXTPA0_SEQ_ON_DELAY field reset value
#define RC_EXTPA0_SEQ_ON_DELAY_RST    0x50

/**
 * @brief Constructs a value for the EXTPA_SEQ_ON_DELAY register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] extpa1seqondelay - The value to use for the EXTPA1_SEQ_ON_DELAY field.
 * @param[in] extpa0seqondelay - The value to use for the EXTPA0_SEQ_ON_DELAY field.
 */
__INLINE void rc_extpa_seq_on_delay_pack(uint8_t extpa1seqondelay, uint8_t extpa0seqondelay)
{
    ASSERT_ERR((((uint32_t)extpa1seqondelay << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)extpa0seqondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_EXTPA_SEQ_ON_DELAY_ADDR,  ((uint32_t)extpa1seqondelay << 8) | ((uint32_t)extpa0seqondelay << 0));
}

/**
 * @brief Unpacks EXTPA_SEQ_ON_DELAY's fields from current value of the EXTPA_SEQ_ON_DELAY register.
 *
 * Reads the EXTPA_SEQ_ON_DELAY register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] extpa1seqondelay - Will be populated with the current value of this field from the register.
 * @param[out] extpa0seqondelay - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_extpa_seq_on_delay_unpack(uint8_t* extpa1seqondelay, uint8_t* extpa0seqondelay)
{
    uint32_t localVal = REG_PL_RD(RC_EXTPA_SEQ_ON_DELAY_ADDR);

    *extpa1seqondelay = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *extpa0seqondelay = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the EXTPA1_SEQ_ON_DELAY field in the EXTPA_SEQ_ON_DELAY register.
 *
 * The EXTPA_SEQ_ON_DELAY register will be read and the EXTPA1_SEQ_ON_DELAY field's value will be returned.
 *
 * @return The current value of the EXTPA1_SEQ_ON_DELAY field in the EXTPA_SEQ_ON_DELAY register.
 */
__INLINE uint8_t rc_extpa1_seq_on_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_EXTPA_SEQ_ON_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the EXTPA1_SEQ_ON_DELAY field of the EXTPA_SEQ_ON_DELAY register.
 *
 * The EXTPA_SEQ_ON_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] extpa1seqondelay - The value to set the field to.
 */
__INLINE void rc_extpa1_seq_on_delay_setf(uint8_t extpa1seqondelay)
{
    ASSERT_ERR((((uint32_t)extpa1seqondelay << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(RC_EXTPA_SEQ_ON_DELAY_ADDR, (REG_PL_RD(RC_EXTPA_SEQ_ON_DELAY_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)extpa1seqondelay << 8));
}

/**
 * @brief Returns the current value of the EXTPA0_SEQ_ON_DELAY field in the EXTPA_SEQ_ON_DELAY register.
 *
 * The EXTPA_SEQ_ON_DELAY register will be read and the EXTPA0_SEQ_ON_DELAY field's value will be returned.
 *
 * @return The current value of the EXTPA0_SEQ_ON_DELAY field in the EXTPA_SEQ_ON_DELAY register.
 */
__INLINE uint8_t rc_extpa0_seq_on_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_EXTPA_SEQ_ON_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the EXTPA0_SEQ_ON_DELAY field of the EXTPA_SEQ_ON_DELAY register.
 *
 * The EXTPA_SEQ_ON_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] extpa0seqondelay - The value to set the field to.
 */
__INLINE void rc_extpa0_seq_on_delay_setf(uint8_t extpa0seqondelay)
{
    ASSERT_ERR((((uint32_t)extpa0seqondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_EXTPA_SEQ_ON_DELAY_ADDR, (REG_PL_RD(RC_EXTPA_SEQ_ON_DELAY_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)extpa0seqondelay << 0));
}

/// @}

/**
 * @name EXTPA_SEQ_OFF_DELAY register definitions
 * <table>
 * <caption>EXTPA_SEQ_OFF_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:08 <td> EXTPA1_SEQ_OFF_DELAY <td> R <td> R/W <td> 0x0
 * <tr><td> 07:00 <td> EXTPA0_SEQ_OFF_DELAY <td> R <td> R/W <td> 0x8
 * </table>
 *
 * @{
 */

/// Address of the EXTPA_SEQ_OFF_DELAY register
#define RC_EXTPA_SEQ_OFF_DELAY_ADDR   0x24C0C02C
/// Offset of the EXTPA_SEQ_OFF_DELAY register from the base address
#define RC_EXTPA_SEQ_OFF_DELAY_OFFSET 0x0000002C
/// Index of the EXTPA_SEQ_OFF_DELAY register
#define RC_EXTPA_SEQ_OFF_DELAY_INDEX  0x0000000B
/// Reset value of the EXTPA_SEQ_OFF_DELAY register
#define RC_EXTPA_SEQ_OFF_DELAY_RESET  0x00000008

/**
 * @brief Returns the current value of the EXTPA_SEQ_OFF_DELAY register.
 * The EXTPA_SEQ_OFF_DELAY register will be read and its value returned.
 * @return The current value of the EXTPA_SEQ_OFF_DELAY register.
 */
__INLINE uint32_t rc_extpa_seq_off_delay_get(void)
{
    return REG_PL_RD(RC_EXTPA_SEQ_OFF_DELAY_ADDR);
}

/**
 * @brief Sets the EXTPA_SEQ_OFF_DELAY register to a value.
 * The EXTPA_SEQ_OFF_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_extpa_seq_off_delay_set(uint32_t value)
{
    REG_PL_WR(RC_EXTPA_SEQ_OFF_DELAY_ADDR, value);
}

// field definitions
/// EXTPA1_SEQ_OFF_DELAY field mask
#define RC_EXTPA1_SEQ_OFF_DELAY_MASK   ((uint32_t)0x0000FF00)
/// EXTPA1_SEQ_OFF_DELAY field LSB position
#define RC_EXTPA1_SEQ_OFF_DELAY_LSB    8
/// EXTPA1_SEQ_OFF_DELAY field width
#define RC_EXTPA1_SEQ_OFF_DELAY_WIDTH  ((uint32_t)0x00000008)
/// EXTPA0_SEQ_OFF_DELAY field mask
#define RC_EXTPA0_SEQ_OFF_DELAY_MASK   ((uint32_t)0x000000FF)
/// EXTPA0_SEQ_OFF_DELAY field LSB position
#define RC_EXTPA0_SEQ_OFF_DELAY_LSB    0
/// EXTPA0_SEQ_OFF_DELAY field width
#define RC_EXTPA0_SEQ_OFF_DELAY_WIDTH  ((uint32_t)0x00000008)

/// EXTPA1_SEQ_OFF_DELAY field reset value
#define RC_EXTPA1_SEQ_OFF_DELAY_RST    0x0
/// EXTPA0_SEQ_OFF_DELAY field reset value
#define RC_EXTPA0_SEQ_OFF_DELAY_RST    0x8

/**
 * @brief Constructs a value for the EXTPA_SEQ_OFF_DELAY register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] extpa1seqoffdelay - The value to use for the EXTPA1_SEQ_OFF_DELAY field.
 * @param[in] extpa0seqoffdelay - The value to use for the EXTPA0_SEQ_OFF_DELAY field.
 */
__INLINE void rc_extpa_seq_off_delay_pack(uint8_t extpa1seqoffdelay, uint8_t extpa0seqoffdelay)
{
    ASSERT_ERR((((uint32_t)extpa1seqoffdelay << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)extpa0seqoffdelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_EXTPA_SEQ_OFF_DELAY_ADDR,  ((uint32_t)extpa1seqoffdelay << 8) | ((uint32_t)extpa0seqoffdelay << 0));
}

/**
 * @brief Unpacks EXTPA_SEQ_OFF_DELAY's fields from current value of the EXTPA_SEQ_OFF_DELAY register.
 *
 * Reads the EXTPA_SEQ_OFF_DELAY register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] extpa1seqoffdelay - Will be populated with the current value of this field from the register.
 * @param[out] extpa0seqoffdelay - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_extpa_seq_off_delay_unpack(uint8_t* extpa1seqoffdelay, uint8_t* extpa0seqoffdelay)
{
    uint32_t localVal = REG_PL_RD(RC_EXTPA_SEQ_OFF_DELAY_ADDR);

    *extpa1seqoffdelay = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *extpa0seqoffdelay = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the EXTPA1_SEQ_OFF_DELAY field in the EXTPA_SEQ_OFF_DELAY register.
 *
 * The EXTPA_SEQ_OFF_DELAY register will be read and the EXTPA1_SEQ_OFF_DELAY field's value will be returned.
 *
 * @return The current value of the EXTPA1_SEQ_OFF_DELAY field in the EXTPA_SEQ_OFF_DELAY register.
 */
__INLINE uint8_t rc_extpa1_seq_off_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_EXTPA_SEQ_OFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the EXTPA1_SEQ_OFF_DELAY field of the EXTPA_SEQ_OFF_DELAY register.
 *
 * The EXTPA_SEQ_OFF_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] extpa1seqoffdelay - The value to set the field to.
 */
__INLINE void rc_extpa1_seq_off_delay_setf(uint8_t extpa1seqoffdelay)
{
    ASSERT_ERR((((uint32_t)extpa1seqoffdelay << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(RC_EXTPA_SEQ_OFF_DELAY_ADDR, (REG_PL_RD(RC_EXTPA_SEQ_OFF_DELAY_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)extpa1seqoffdelay << 8));
}

/**
 * @brief Returns the current value of the EXTPA0_SEQ_OFF_DELAY field in the EXTPA_SEQ_OFF_DELAY register.
 *
 * The EXTPA_SEQ_OFF_DELAY register will be read and the EXTPA0_SEQ_OFF_DELAY field's value will be returned.
 *
 * @return The current value of the EXTPA0_SEQ_OFF_DELAY field in the EXTPA_SEQ_OFF_DELAY register.
 */
__INLINE uint8_t rc_extpa0_seq_off_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_EXTPA_SEQ_OFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the EXTPA0_SEQ_OFF_DELAY field of the EXTPA_SEQ_OFF_DELAY register.
 *
 * The EXTPA_SEQ_OFF_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] extpa0seqoffdelay - The value to set the field to.
 */
__INLINE void rc_extpa0_seq_off_delay_setf(uint8_t extpa0seqoffdelay)
{
    ASSERT_ERR((((uint32_t)extpa0seqoffdelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_EXTPA_SEQ_OFF_DELAY_ADDR, (REG_PL_RD(RC_EXTPA_SEQ_OFF_DELAY_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)extpa0seqoffdelay << 0));
}

/// @}

/**
 * @name TXSWITCH_ONOFF_DELAY register definitions
 * <table>
 * <caption>TXSWITCH_ONOFF_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> TXSWITCH_OFF_DELAY <td> R <td> R/W <td> 0xA0
 * <tr><td> 09:00 <td>  TXSWITCH_ON_DELAY <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TXSWITCH_ONOFF_DELAY register
#define RC_TXSWITCH_ONOFF_DELAY_ADDR   0x24C0C030
/// Offset of the TXSWITCH_ONOFF_DELAY register from the base address
#define RC_TXSWITCH_ONOFF_DELAY_OFFSET 0x00000030
/// Index of the TXSWITCH_ONOFF_DELAY register
#define RC_TXSWITCH_ONOFF_DELAY_INDEX  0x0000000C
/// Reset value of the TXSWITCH_ONOFF_DELAY register
#define RC_TXSWITCH_ONOFF_DELAY_RESET  0x00A00000

/**
 * @brief Returns the current value of the TXSWITCH_ONOFF_DELAY register.
 * The TXSWITCH_ONOFF_DELAY register will be read and its value returned.
 * @return The current value of the TXSWITCH_ONOFF_DELAY register.
 */
__INLINE uint32_t rc_txswitch_onoff_delay_get(void)
{
    return REG_PL_RD(RC_TXSWITCH_ONOFF_DELAY_ADDR);
}

/**
 * @brief Sets the TXSWITCH_ONOFF_DELAY register to a value.
 * The TXSWITCH_ONOFF_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_txswitch_onoff_delay_set(uint32_t value)
{
    REG_PL_WR(RC_TXSWITCH_ONOFF_DELAY_ADDR, value);
}

// field definitions
/// TXSWITCH_OFF_DELAY field mask
#define RC_TXSWITCH_OFF_DELAY_MASK   ((uint32_t)0x03FF0000)
/// TXSWITCH_OFF_DELAY field LSB position
#define RC_TXSWITCH_OFF_DELAY_LSB    16
/// TXSWITCH_OFF_DELAY field width
#define RC_TXSWITCH_OFF_DELAY_WIDTH  ((uint32_t)0x0000000A)
/// TXSWITCH_ON_DELAY field mask
#define RC_TXSWITCH_ON_DELAY_MASK    ((uint32_t)0x000003FF)
/// TXSWITCH_ON_DELAY field LSB position
#define RC_TXSWITCH_ON_DELAY_LSB     0
/// TXSWITCH_ON_DELAY field width
#define RC_TXSWITCH_ON_DELAY_WIDTH   ((uint32_t)0x0000000A)

/// TXSWITCH_OFF_DELAY field reset value
#define RC_TXSWITCH_OFF_DELAY_RST    0xA0
/// TXSWITCH_ON_DELAY field reset value
#define RC_TXSWITCH_ON_DELAY_RST     0x0

/**
 * @brief Constructs a value for the TXSWITCH_ONOFF_DELAY register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txswitchoffdelay - The value to use for the TXSWITCH_OFF_DELAY field.
 * @param[in] txswitchondelay - The value to use for the TXSWITCH_ON_DELAY field.
 */
__INLINE void rc_txswitch_onoff_delay_pack(uint16_t txswitchoffdelay, uint16_t txswitchondelay)
{
    ASSERT_ERR((((uint32_t)txswitchoffdelay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)txswitchondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TXSWITCH_ONOFF_DELAY_ADDR,  ((uint32_t)txswitchoffdelay << 16) | ((uint32_t)txswitchondelay << 0));
}

/**
 * @brief Unpacks TXSWITCH_ONOFF_DELAY's fields from current value of the TXSWITCH_ONOFF_DELAY register.
 *
 * Reads the TXSWITCH_ONOFF_DELAY register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txswitchoffdelay - Will be populated with the current value of this field from the register.
 * @param[out] txswitchondelay - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_txswitch_onoff_delay_unpack(uint16_t* txswitchoffdelay, uint16_t* txswitchondelay)
{
    uint32_t localVal = REG_PL_RD(RC_TXSWITCH_ONOFF_DELAY_ADDR);

    *txswitchoffdelay = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *txswitchondelay = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the TXSWITCH_OFF_DELAY field in the TXSWITCH_ONOFF_DELAY register.
 *
 * The TXSWITCH_ONOFF_DELAY register will be read and the TXSWITCH_OFF_DELAY field's value will be returned.
 *
 * @return The current value of the TXSWITCH_OFF_DELAY field in the TXSWITCH_ONOFF_DELAY register.
 */
__INLINE uint16_t rc_txswitch_off_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TXSWITCH_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the TXSWITCH_OFF_DELAY field of the TXSWITCH_ONOFF_DELAY register.
 *
 * The TXSWITCH_ONOFF_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txswitchoffdelay - The value to set the field to.
 */
__INLINE void rc_txswitch_off_delay_setf(uint16_t txswitchoffdelay)
{
    ASSERT_ERR((((uint32_t)txswitchoffdelay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_TXSWITCH_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_TXSWITCH_ONOFF_DELAY_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)txswitchoffdelay << 16));
}

/**
 * @brief Returns the current value of the TXSWITCH_ON_DELAY field in the TXSWITCH_ONOFF_DELAY register.
 *
 * The TXSWITCH_ONOFF_DELAY register will be read and the TXSWITCH_ON_DELAY field's value will be returned.
 *
 * @return The current value of the TXSWITCH_ON_DELAY field in the TXSWITCH_ONOFF_DELAY register.
 */
__INLINE uint16_t rc_txswitch_on_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TXSWITCH_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the TXSWITCH_ON_DELAY field of the TXSWITCH_ONOFF_DELAY register.
 *
 * The TXSWITCH_ONOFF_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txswitchondelay - The value to set the field to.
 */
__INLINE void rc_txswitch_on_delay_setf(uint16_t txswitchondelay)
{
    ASSERT_ERR((((uint32_t)txswitchondelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TXSWITCH_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_TXSWITCH_ONOFF_DELAY_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)txswitchondelay << 0));
}

/// @}

/**
 * @name SPI_RD_DELAY register definitions
 * <table>
 * <caption>SPI_RD_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 05:00 <td> SPI_RD_DELAY <td> R <td> R/W <td> 0x1A
 * </table>
 *
 * @{
 */

/// Address of the SPI_RD_DELAY register
#define RC_SPI_RD_DELAY_ADDR   0x24C0C034
/// Offset of the SPI_RD_DELAY register from the base address
#define RC_SPI_RD_DELAY_OFFSET 0x00000034
/// Index of the SPI_RD_DELAY register
#define RC_SPI_RD_DELAY_INDEX  0x0000000D
/// Reset value of the SPI_RD_DELAY register
#define RC_SPI_RD_DELAY_RESET  0x0000001A

/**
 * @brief Returns the current value of the SPI_RD_DELAY register.
 * The SPI_RD_DELAY register will be read and its value returned.
 * @return The current value of the SPI_RD_DELAY register.
 */
__INLINE uint32_t rc_spi_rd_delay_get(void)
{
    return REG_PL_RD(RC_SPI_RD_DELAY_ADDR);
}

/**
 * @brief Sets the SPI_RD_DELAY register to a value.
 * The SPI_RD_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_spi_rd_delay_set(uint32_t value)
{
    REG_PL_WR(RC_SPI_RD_DELAY_ADDR, value);
}

// field definitions
/// SPI_RD_DELAY field mask
#define RC_SPI_RD_DELAY_MASK   ((uint32_t)0x0000003F)
/// SPI_RD_DELAY field LSB position
#define RC_SPI_RD_DELAY_LSB    0
/// SPI_RD_DELAY field width
#define RC_SPI_RD_DELAY_WIDTH  ((uint32_t)0x00000006)

/// SPI_RD_DELAY field reset value
#define RC_SPI_RD_DELAY_RST    0x1A

/**
 * @brief Returns the current value of the SPI_RD_DELAY field in the SPI_RD_DELAY register.
 *
 * The SPI_RD_DELAY register will be read and the SPI_RD_DELAY field's value will be returned.
 *
 * @return The current value of the SPI_RD_DELAY field in the SPI_RD_DELAY register.
 */
__INLINE uint8_t rc_spi_rd_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_SPI_RD_DELAY_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000003F)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the SPI_RD_DELAY field of the SPI_RD_DELAY register.
 *
 * The SPI_RD_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] spirddelay - The value to set the field to.
 */
__INLINE void rc_spi_rd_delay_setf(uint8_t spirddelay)
{
    ASSERT_ERR((((uint32_t)spirddelay << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(RC_SPI_RD_DELAY_ADDR, (uint32_t)spirddelay << 0);
}

/// @}

/**
 * @name STATUS register definitions
 * <table>
 * <caption>STATUS bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 02:00 <td> RC_STATE <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the STATUS register
#define RC_STATUS_ADDR   0x24C0C038
/// Offset of the STATUS register from the base address
#define RC_STATUS_OFFSET 0x00000038
/// Index of the STATUS register
#define RC_STATUS_INDEX  0x0000000E
/// Reset value of the STATUS register
#define RC_STATUS_RESET  0x00000000

/**
 * @brief Returns the current value of the STATUS register.
 * The STATUS register will be read and its value returned.
 * @return The current value of the STATUS register.
 */
__INLINE uint32_t rc_status_get(void)
{
    return REG_PL_RD(RC_STATUS_ADDR);
}

// field definitions
/// RC_STATE field mask
#define RC_RC_STATE_MASK   ((uint32_t)0x00000007)
/// RC_STATE field LSB position
#define RC_RC_STATE_LSB    0
/// RC_STATE field width
#define RC_RC_STATE_WIDTH  ((uint32_t)0x00000003)

/// RC_STATE field reset value
#define RC_RC_STATE_RST    0x0

/**
 * @brief Returns the current value of the RC_STATE field in the STATUS register.
 *
 * The STATUS register will be read and the RC_STATE field's value will be returned.
 *
 * @return The current value of the RC_STATE field in the STATUS register.
 */
__INLINE uint8_t rc_rc_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_STATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000007)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name HW_RX_AGCGAINREF register definitions
 * <table>
 * <caption>HW_RX_AGCGAINREF bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 22:16 <td> RXAGCGAININDMAX <td> R <td> R/W <td> 0x42
 * <tr><td> 06:00 <td> RXAGCGAININDMIN <td> R <td> R/W <td> 0x3
 * </table>
 *
 * @{
 */

/// Address of the HW_RX_AGCGAINREF register
#define RC_HW_RX_AGCGAINREF_ADDR   0x24C0C03C
/// Offset of the HW_RX_AGCGAINREF register from the base address
#define RC_HW_RX_AGCGAINREF_OFFSET 0x0000003C
/// Index of the HW_RX_AGCGAINREF register
#define RC_HW_RX_AGCGAINREF_INDEX  0x0000000F
/// Reset value of the HW_RX_AGCGAINREF register
#define RC_HW_RX_AGCGAINREF_RESET  0x00420003

/**
 * @brief Returns the current value of the HW_RX_AGCGAINREF register.
 * The HW_RX_AGCGAINREF register will be read and its value returned.
 * @return The current value of the HW_RX_AGCGAINREF register.
 */
__INLINE uint32_t rc_hw_rx_agcgainref_get(void)
{
    return REG_PL_RD(RC_HW_RX_AGCGAINREF_ADDR);
}

/**
 * @brief Sets the HW_RX_AGCGAINREF register to a value.
 * The HW_RX_AGCGAINREF register will be written.
 * @param value - The value to write.
 */
__INLINE void rc_hw_rx_agcgainref_set(uint32_t value)
{
    REG_PL_WR(RC_HW_RX_AGCGAINREF_ADDR, value);
}

// field definitions
/// RXAGCGAININDMAX field mask
#define RC_RXAGCGAININDMAX_MASK   ((uint32_t)0x007F0000)
/// RXAGCGAININDMAX field LSB position
#define RC_RXAGCGAININDMAX_LSB    16
/// RXAGCGAININDMAX field width
#define RC_RXAGCGAININDMAX_WIDTH  ((uint32_t)0x00000007)
/// RXAGCGAININDMIN field mask
#define RC_RXAGCGAININDMIN_MASK   ((uint32_t)0x0000007F)
/// RXAGCGAININDMIN field LSB position
#define RC_RXAGCGAININDMIN_LSB    0
/// RXAGCGAININDMIN field width
#define RC_RXAGCGAININDMIN_WIDTH  ((uint32_t)0x00000007)

/// RXAGCGAININDMAX field reset value
#define RC_RXAGCGAININDMAX_RST    0x42
/// RXAGCGAININDMIN field reset value
#define RC_RXAGCGAININDMIN_RST    0x3

/**
 * @brief Constructs a value for the HW_RX_AGCGAINREF register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxagcgainindmax - The value to use for the RXAGCGAININDMAX field.
 * @param[in] rxagcgainindmin - The value to use for the RXAGCGAININDMIN field.
 */
__INLINE void rc_hw_rx_agcgainref_pack(uint8_t rxagcgainindmax, uint8_t rxagcgainindmin)
{
    ASSERT_ERR((((uint32_t)rxagcgainindmax << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)rxagcgainindmin << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(RC_HW_RX_AGCGAINREF_ADDR,  ((uint32_t)rxagcgainindmax << 16) | ((uint32_t)rxagcgainindmin << 0));
}

/**
 * @brief Unpacks HW_RX_AGCGAINREF's fields from current value of the HW_RX_AGCGAINREF register.
 *
 * Reads the HW_RX_AGCGAINREF register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxagcgainindmax - Will be populated with the current value of this field from the register.
 * @param[out] rxagcgainindmin - Will be populated with the current value of this field from the register.
 */
__INLINE void rc_hw_rx_agcgainref_unpack(uint8_t* rxagcgainindmax, uint8_t* rxagcgainindmin)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_AGCGAINREF_ADDR);

    *rxagcgainindmax = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *rxagcgainindmin = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the RXAGCGAININDMAX field in the HW_RX_AGCGAINREF register.
 *
 * The HW_RX_AGCGAINREF register will be read and the RXAGCGAININDMAX field's value will be returned.
 *
 * @return The current value of the RXAGCGAININDMAX field in the HW_RX_AGCGAINREF register.
 */
__INLINE uint8_t rc_rxagcgainindmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_AGCGAINREF_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the RXAGCGAININDMAX field of the HW_RX_AGCGAINREF register.
 *
 * The HW_RX_AGCGAINREF register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxagcgainindmax - The value to set the field to.
 */
__INLINE void rc_rxagcgainindmax_setf(uint8_t rxagcgainindmax)
{
    ASSERT_ERR((((uint32_t)rxagcgainindmax << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(RC_HW_RX_AGCGAINREF_ADDR, (REG_PL_RD(RC_HW_RX_AGCGAINREF_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)rxagcgainindmax << 16));
}

/**
 * @brief Returns the current value of the RXAGCGAININDMIN field in the HW_RX_AGCGAINREF register.
 *
 * The HW_RX_AGCGAINREF register will be read and the RXAGCGAININDMIN field's value will be returned.
 *
 * @return The current value of the RXAGCGAININDMIN field in the HW_RX_AGCGAINREF register.
 */
__INLINE uint8_t rc_rxagcgainindmin_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_HW_RX_AGCGAINREF_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the RXAGCGAININDMIN field of the HW_RX_AGCGAINREF register.
 *
 * The HW_RX_AGCGAINREF register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxagcgainindmin - The value to set the field to.
 */
__INLINE void rc_rxagcgainindmin_setf(uint8_t rxagcgainindmin)
{
    ASSERT_ERR((((uint32_t)rxagcgainindmin << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(RC_HW_RX_AGCGAINREF_ADDR, (REG_PL_RD(RC_HW_RX_AGCGAINREF_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)rxagcgainindmin << 0));
}

/// @}


#endif // _REG_RC_H_

/// @}

