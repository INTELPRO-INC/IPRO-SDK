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
 * @file reg_sysctrl.h
 * @brief Definitions of the SYSCTRL HW block registers and register access functions.
 *
 * @defgroup REG_SYSCTRL REG_SYSCTRL
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the SYSCTRL HW block registers and register access functions.
 */
#ifndef _REG_SYSCTRL_H_
#define _REG_SYSCTRL_H_

#include "co_int.h"
#include "_reg_sysctrl.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_SYSCTRL peripheral.
 */
#define REG_SYSCTRL_COUNT 57

/** @brief Decoding mask of the REG_SYSCTRL peripheral registers from the CPU point of view.
 */
#define REG_SYSCTRL_DECODING_MASK 0x000000FF

/**
 * @name SIGNATURE register definitions
 * <table>
 * <caption>SIGNATURE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> fpga_signature <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SIGNATURE register
#define SYSCTRL_SIGNATURE_ADDR   0x24900000
/// Offset of the SIGNATURE register from the base address
#define SYSCTRL_SIGNATURE_OFFSET 0x00000000
/// Index of the SIGNATURE register
#define SYSCTRL_SIGNATURE_INDEX  0x00000000
/// Reset value of the SIGNATURE register
#define SYSCTRL_SIGNATURE_RESET  0x00000000

/**
 * @brief Returns the current value of the SIGNATURE register.
 * The SIGNATURE register will be read and its value returned.
 * @return The current value of the SIGNATURE register.
 */
__INLINE uint32_t sysctrl_signature_get(void)
{
    return REG_PL_RD(SYSCTRL_SIGNATURE_ADDR);
}

// field definitions
/// FPGA_SIGNATURE field mask
#define SYSCTRL_FPGA_SIGNATURE_MASK   ((uint32_t)0xFFFFFFFF)
/// FPGA_SIGNATURE field LSB position
#define SYSCTRL_FPGA_SIGNATURE_LSB    0
/// FPGA_SIGNATURE field width
#define SYSCTRL_FPGA_SIGNATURE_WIDTH  ((uint32_t)0x00000020)

/// FPGA_SIGNATURE field reset value
#define SYSCTRL_FPGA_SIGNATURE_RST    0x0

/**
 * @brief Returns the current value of the fpga_signature field in the SIGNATURE register.
 *
 * The SIGNATURE register will be read and the fpga_signature field's value will be returned.
 *
 * @return The current value of the fpga_signature field in the SIGNATURE register.
 */
__INLINE uint32_t sysctrl_fpga_signature_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_SIGNATURE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DATE register definitions
 * <table>
 * <caption>DATE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> fpga_date <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DATE register
#define SYSCTRL_DATE_ADDR   0x24900004
/// Offset of the DATE register from the base address
#define SYSCTRL_DATE_OFFSET 0x00000004
/// Index of the DATE register
#define SYSCTRL_DATE_INDEX  0x00000001
/// Reset value of the DATE register
#define SYSCTRL_DATE_RESET  0x00000000

/**
 * @brief Returns the current value of the DATE register.
 * The DATE register will be read and its value returned.
 * @return The current value of the DATE register.
 */
__INLINE uint32_t sysctrl_date_get(void)
{
    return REG_PL_RD(SYSCTRL_DATE_ADDR);
}

// field definitions
/// FPGA_DATE field mask
#define SYSCTRL_FPGA_DATE_MASK   ((uint32_t)0xFFFFFFFF)
/// FPGA_DATE field LSB position
#define SYSCTRL_FPGA_DATE_LSB    0
/// FPGA_DATE field width
#define SYSCTRL_FPGA_DATE_WIDTH  ((uint32_t)0x00000020)

/// FPGA_DATE field reset value
#define SYSCTRL_FPGA_DATE_RST    0x0

/**
 * @brief Returns the current value of the fpga_date field in the DATE register.
 *
 * The DATE register will be read and the fpga_date field's value will be returned.
 *
 * @return The current value of the fpga_date field in the DATE register.
 */
__INLINE uint32_t sysctrl_fpga_date_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DATE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name TIME register definitions
 * <table>
 * <caption>TIME bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> fpga_time <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TIME register
#define SYSCTRL_TIME_ADDR   0x24900008
/// Offset of the TIME register from the base address
#define SYSCTRL_TIME_OFFSET 0x00000008
/// Index of the TIME register
#define SYSCTRL_TIME_INDEX  0x00000002
/// Reset value of the TIME register
#define SYSCTRL_TIME_RESET  0x00000000

/**
 * @brief Returns the current value of the TIME register.
 * The TIME register will be read and its value returned.
 * @return The current value of the TIME register.
 */
__INLINE uint32_t sysctrl_time_get(void)
{
    return REG_PL_RD(SYSCTRL_TIME_ADDR);
}

// field definitions
/// FPGA_TIME field mask
#define SYSCTRL_FPGA_TIME_MASK   ((uint32_t)0xFFFFFFFF)
/// FPGA_TIME field LSB position
#define SYSCTRL_FPGA_TIME_LSB    0
/// FPGA_TIME field width
#define SYSCTRL_FPGA_TIME_WIDTH  ((uint32_t)0x00000020)

/// FPGA_TIME field reset value
#define SYSCTRL_FPGA_TIME_RST    0x0

/**
 * @brief Returns the current value of the fpga_time field in the TIME register.
 *
 * The TIME register will be read and the fpga_time field's value will be returned.
 *
 * @return The current value of the fpga_time field in the TIME register.
 */
__INLINE uint32_t sysctrl_fpga_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_TIME_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name SVNREV_MAC register definitions
 * <table>
 * <caption>SVNREV_MAC bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> svnrev_mac <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SVNREV_MAC register
#define SYSCTRL_SVNREV_MAC_ADDR   0x2490000C
/// Offset of the SVNREV_MAC register from the base address
#define SYSCTRL_SVNREV_MAC_OFFSET 0x0000000C
/// Index of the SVNREV_MAC register
#define SYSCTRL_SVNREV_MAC_INDEX  0x00000003
/// Reset value of the SVNREV_MAC register
#define SYSCTRL_SVNREV_MAC_RESET  0x00000000

/**
 * @brief Returns the current value of the SVNREV_MAC register.
 * The SVNREV_MAC register will be read and its value returned.
 * @return The current value of the SVNREV_MAC register.
 */
__INLINE uint32_t sysctrl_svnrev_mac_get(void)
{
    return REG_PL_RD(SYSCTRL_SVNREV_MAC_ADDR);
}

// field definitions
/// SVNREV_MAC field mask
#define SYSCTRL_SVNREV_MAC_MASK   ((uint32_t)0xFFFFFFFF)
/// SVNREV_MAC field LSB position
#define SYSCTRL_SVNREV_MAC_LSB    0
/// SVNREV_MAC field width
#define SYSCTRL_SVNREV_MAC_WIDTH  ((uint32_t)0x00000020)

/// SVNREV_MAC field reset value
#define SYSCTRL_SVNREV_MAC_RST    0x0

/**
 * @brief Returns the current value of the svnrev_mac field in the SVNREV_MAC register.
 *
 * The SVNREV_MAC register will be read and the svnrev_mac field's value will be returned.
 *
 * @return The current value of the svnrev_mac field in the SVNREV_MAC register.
 */
__INLINE uint32_t sysctrl_svnrev_mac_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_SVNREV_MAC_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name SVNREV_FPGAA register definitions
 * <table>
 * <caption>SVNREV_FPGAA bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> svnrev_fpgaa <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SVNREV_FPGAA register
#define SYSCTRL_SVNREV_FPGAA_ADDR   0x24900010
/// Offset of the SVNREV_FPGAA register from the base address
#define SYSCTRL_SVNREV_FPGAA_OFFSET 0x00000010
/// Index of the SVNREV_FPGAA register
#define SYSCTRL_SVNREV_FPGAA_INDEX  0x00000004
/// Reset value of the SVNREV_FPGAA register
#define SYSCTRL_SVNREV_FPGAA_RESET  0x00000000

/**
 * @brief Returns the current value of the SVNREV_FPGAA register.
 * The SVNREV_FPGAA register will be read and its value returned.
 * @return The current value of the SVNREV_FPGAA register.
 */
__INLINE uint32_t sysctrl_svnrev_fpgaa_get(void)
{
    return REG_PL_RD(SYSCTRL_SVNREV_FPGAA_ADDR);
}

// field definitions
/// SVNREV_FPGAA field mask
#define SYSCTRL_SVNREV_FPGAA_MASK   ((uint32_t)0xFFFFFFFF)
/// SVNREV_FPGAA field LSB position
#define SYSCTRL_SVNREV_FPGAA_LSB    0
/// SVNREV_FPGAA field width
#define SYSCTRL_SVNREV_FPGAA_WIDTH  ((uint32_t)0x00000020)

/// SVNREV_FPGAA field reset value
#define SYSCTRL_SVNREV_FPGAA_RST    0x0

/**
 * @brief Returns the current value of the svnrev_fpgaa field in the SVNREV_FPGAA register.
 *
 * The SVNREV_FPGAA register will be read and the svnrev_fpgaa field's value will be returned.
 *
 * @return The current value of the svnrev_fpgaa field in the SVNREV_FPGAA register.
 */
__INLINE uint32_t sysctrl_svnrev_fpgaa_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_SVNREV_FPGAA_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name TIMER register definitions
 * <table>
 * <caption>TIMER bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> timer_value <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TIMER register
#define SYSCTRL_TIMER_ADDR   0x24900040
/// Offset of the TIMER register from the base address
#define SYSCTRL_TIMER_OFFSET 0x00000040
/// Index of the TIMER register
#define SYSCTRL_TIMER_INDEX  0x00000010
/// Reset value of the TIMER register
#define SYSCTRL_TIMER_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMER register.
 * The TIMER register will be read and its value returned.
 * @return The current value of the TIMER register.
 */
__INLINE uint32_t sysctrl_timer_get(void)
{
    return REG_PL_RD(SYSCTRL_TIMER_ADDR);
}

/**
 * @brief Sets the TIMER register to a value.
 * The TIMER register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_timer_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_TIMER_ADDR, value);
}

// field definitions
/// TIMER_VALUE field mask
#define SYSCTRL_TIMER_VALUE_MASK   ((uint32_t)0xFFFFFFFF)
/// TIMER_VALUE field LSB position
#define SYSCTRL_TIMER_VALUE_LSB    0
/// TIMER_VALUE field width
#define SYSCTRL_TIMER_VALUE_WIDTH  ((uint32_t)0x00000020)

/// TIMER_VALUE field reset value
#define SYSCTRL_TIMER_VALUE_RST    0x0

/**
 * @brief Returns the current value of the timer_value field in the TIMER register.
 *
 * The TIMER register will be read and the timer_value field's value will be returned.
 *
 * @return The current value of the timer_value field in the TIMER register.
 */
__INLINE uint32_t sysctrl_timer_value_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_TIMER_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the timer_value field of the TIMER register.
 *
 * The TIMER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timervalue - The value to set the field to.
 */
__INLINE void sysctrl_timer_value_setf(uint32_t timervalue)
{
    ASSERT_ERR((((uint32_t)timervalue << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(SYSCTRL_TIMER_ADDR, (uint32_t)timervalue << 0);
}

/// @}

/**
 * @name TICK_TIMER_CNT register definitions
 * <table>
 * <caption>TICK_TIMER_CNT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> tick_timer_cnt <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TICK_TIMER_CNT register
#define SYSCTRL_TICK_TIMER_CNT_ADDR   0x24900044
/// Offset of the TICK_TIMER_CNT register from the base address
#define SYSCTRL_TICK_TIMER_CNT_OFFSET 0x00000044
/// Index of the TICK_TIMER_CNT register
#define SYSCTRL_TICK_TIMER_CNT_INDEX  0x00000011
/// Reset value of the TICK_TIMER_CNT register
#define SYSCTRL_TICK_TIMER_CNT_RESET  0x00000000

/**
 * @brief Returns the current value of the TICK_TIMER_CNT register.
 * The TICK_TIMER_CNT register will be read and its value returned.
 * @return The current value of the TICK_TIMER_CNT register.
 */
__INLINE uint32_t sysctrl_tick_timer_cnt_get(void)
{
    return REG_PL_RD(SYSCTRL_TICK_TIMER_CNT_ADDR);
}

/**
 * @brief Sets the TICK_TIMER_CNT register to a value.
 * The TICK_TIMER_CNT register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_tick_timer_cnt_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_TICK_TIMER_CNT_ADDR, value);
}

// field definitions
/// TICK_TIMER_CNT field mask
#define SYSCTRL_TICK_TIMER_CNT_MASK   ((uint32_t)0xFFFFFFFF)
/// TICK_TIMER_CNT field LSB position
#define SYSCTRL_TICK_TIMER_CNT_LSB    0
/// TICK_TIMER_CNT field width
#define SYSCTRL_TICK_TIMER_CNT_WIDTH  ((uint32_t)0x00000020)

/// TICK_TIMER_CNT field reset value
#define SYSCTRL_TICK_TIMER_CNT_RST    0x0

/**
 * @brief Returns the current value of the tick_timer_cnt field in the TICK_TIMER_CNT register.
 *
 * The TICK_TIMER_CNT register will be read and the tick_timer_cnt field's value will be returned.
 *
 * @return The current value of the tick_timer_cnt field in the TICK_TIMER_CNT register.
 */
__INLINE uint32_t sysctrl_tick_timer_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_TICK_TIMER_CNT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the tick_timer_cnt field of the TICK_TIMER_CNT register.
 *
 * The TICK_TIMER_CNT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ticktimercnt - The value to set the field to.
 */
__INLINE void sysctrl_tick_timer_cnt_setf(uint32_t ticktimercnt)
{
    ASSERT_ERR((((uint32_t)ticktimercnt << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(SYSCTRL_TICK_TIMER_CNT_ADDR, (uint32_t)ticktimercnt << 0);
}

/// @}

/**
 * @name TICK_TIMER_DURATION register definitions
 * <table>
 * <caption>TICK_TIMER_DURATION bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> tick_timer_duration <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TICK_TIMER_DURATION register
#define SYSCTRL_TICK_TIMER_DURATION_ADDR   0x24900048
/// Offset of the TICK_TIMER_DURATION register from the base address
#define SYSCTRL_TICK_TIMER_DURATION_OFFSET 0x00000048
/// Index of the TICK_TIMER_DURATION register
#define SYSCTRL_TICK_TIMER_DURATION_INDEX  0x00000012
/// Reset value of the TICK_TIMER_DURATION register
#define SYSCTRL_TICK_TIMER_DURATION_RESET  0x00000000

/**
 * @brief Returns the current value of the TICK_TIMER_DURATION register.
 * The TICK_TIMER_DURATION register will be read and its value returned.
 * @return The current value of the TICK_TIMER_DURATION register.
 */
__INLINE uint32_t sysctrl_tick_timer_duration_get(void)
{
    return REG_PL_RD(SYSCTRL_TICK_TIMER_DURATION_ADDR);
}

/**
 * @brief Sets the TICK_TIMER_DURATION register to a value.
 * The TICK_TIMER_DURATION register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_tick_timer_duration_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_TICK_TIMER_DURATION_ADDR, value);
}

// field definitions
/// TICK_TIMER_DURATION field mask
#define SYSCTRL_TICK_TIMER_DURATION_MASK   ((uint32_t)0xFFFFFFFF)
/// TICK_TIMER_DURATION field LSB position
#define SYSCTRL_TICK_TIMER_DURATION_LSB    0
/// TICK_TIMER_DURATION field width
#define SYSCTRL_TICK_TIMER_DURATION_WIDTH  ((uint32_t)0x00000020)

/// TICK_TIMER_DURATION field reset value
#define SYSCTRL_TICK_TIMER_DURATION_RST    0x0

/**
 * @brief Returns the current value of the tick_timer_duration field in the TICK_TIMER_DURATION register.
 *
 * The TICK_TIMER_DURATION register will be read and the tick_timer_duration field's value will be returned.
 *
 * @return The current value of the tick_timer_duration field in the TICK_TIMER_DURATION register.
 */
__INLINE uint32_t sysctrl_tick_timer_duration_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_TICK_TIMER_DURATION_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the tick_timer_duration field of the TICK_TIMER_DURATION register.
 *
 * The TICK_TIMER_DURATION register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ticktimerduration - The value to set the field to.
 */
__INLINE void sysctrl_tick_timer_duration_setf(uint32_t ticktimerduration)
{
    ASSERT_ERR((((uint32_t)ticktimerduration << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(SYSCTRL_TICK_TIMER_DURATION_ADDR, (uint32_t)ticktimerduration << 0);
}

/// @}

/**
 * @name TICK_TIMER_IRQ_SET register definitions
 * <table>
 * <caption>TICK_TIMER_IRQ_SET bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> tick_timer_irq <td> R <td> S <td> 0
 * </table>
 *
 * @{
 */

/// Address of the TICK_TIMER_IRQ_SET register
#define SYSCTRL_TICK_TIMER_IRQ_SET_ADDR   0x2490004C
/// Offset of the TICK_TIMER_IRQ_SET register from the base address
#define SYSCTRL_TICK_TIMER_IRQ_SET_OFFSET 0x0000004C
/// Index of the TICK_TIMER_IRQ_SET register
#define SYSCTRL_TICK_TIMER_IRQ_SET_INDEX  0x00000013
/// Reset value of the TICK_TIMER_IRQ_SET register
#define SYSCTRL_TICK_TIMER_IRQ_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the TICK_TIMER_IRQ_SET register.
 * The TICK_TIMER_IRQ_SET register will be read and its value returned.
 * @return The current value of the TICK_TIMER_IRQ_SET register.
 */
__INLINE uint32_t sysctrl_tick_timer_irq_get(void)
{
    return REG_PL_RD(SYSCTRL_TICK_TIMER_IRQ_SET_ADDR);
}

/**
 * @brief Sets the TICK_TIMER_IRQ_SET register to a value.
 * The TICK_TIMER_IRQ_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_tick_timer_irq_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_TICK_TIMER_IRQ_SET_ADDR, value);
}

// field definitions
/// TICK_TIMER_IRQ field bit
#define SYSCTRL_TICK_TIMER_IRQ_BIT    ((uint32_t)0x00000001)
/// TICK_TIMER_IRQ field position
#define SYSCTRL_TICK_TIMER_IRQ_POS    0

/// TICK_TIMER_IRQ field reset value
#define SYSCTRL_TICK_TIMER_IRQ_RST    0x0

/**
 * @brief Returns the current value of the tick_timer_irq field in the TICK_TIMER_IRQ_SET register.
 *
 * The TICK_TIMER_IRQ_SET register will be read and the tick_timer_irq field's value will be returned.
 *
 * @return The current value of the tick_timer_irq field in the TICK_TIMER_IRQ_SET register.
 */
__INLINE uint8_t sysctrl_tick_timer_irq_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_TICK_TIMER_IRQ_SET_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the tick_timer_irq field of the TICK_TIMER_IRQ_SET register.
 *
 * The TICK_TIMER_IRQ_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ticktimerirq - The value to set the field to.
 */
__INLINE void sysctrl_tick_timer_irq_setf(uint8_t ticktimerirq)
{
    ASSERT_ERR((((uint32_t)ticktimerirq << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_TICK_TIMER_IRQ_SET_ADDR, (uint32_t)ticktimerirq << 0);
}

/// @}

/**
 * @name TICK_TIMER_IRQ_CLEAR register definitions
 * <table>
 * <caption>TICK_TIMER_IRQ_CLEAR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> tick_timer_irq <td> R <td> C <td> 0
 * </table>
 *
 * @{
 */

/// Address of the TICK_TIMER_IRQ_CLEAR register
#define SYSCTRL_TICK_TIMER_IRQ_CLEAR_ADDR   0x24900050
/// Offset of the TICK_TIMER_IRQ_CLEAR register from the base address
#define SYSCTRL_TICK_TIMER_IRQ_CLEAR_OFFSET 0x00000050
/// Index of the TICK_TIMER_IRQ_CLEAR register
#define SYSCTRL_TICK_TIMER_IRQ_CLEAR_INDEX  0x00000014
/// Reset value of the TICK_TIMER_IRQ_CLEAR register
#define SYSCTRL_TICK_TIMER_IRQ_CLEAR_RESET  0x00000000

/**
 * @brief Sets the TICK_TIMER_IRQ_CLEAR register to a value.
 * The TICK_TIMER_IRQ_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_tick_timer_irq_clear(uint32_t value)
{
    REG_PL_WR(SYSCTRL_TICK_TIMER_IRQ_CLEAR_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Sets the tick_timer_irq field of the TICK_TIMER_IRQ_CLEAR register.
 *
 * The TICK_TIMER_IRQ_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ticktimerirq - The value to set the field to.
 */
__INLINE void sysctrl_tick_timer_irq_clearf(uint8_t ticktimerirq)
{
    ASSERT_ERR((((uint32_t)ticktimerirq << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_TICK_TIMER_IRQ_CLEAR_ADDR, (uint32_t)ticktimerirq << 0);
}

/// @}

/**
 * @name TICK_TIMER_IRQ_CONTROL register definitions
 * <table>
 * <caption>TICK_TIMER_IRQ_CONTROL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 01 <td>     tick_timer_en <td> R <td> R/W <td> 0
 * <tr><td> 00 <td> tick_timer_irq_en <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the TICK_TIMER_IRQ_CONTROL register
#define SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR   0x24900054
/// Offset of the TICK_TIMER_IRQ_CONTROL register from the base address
#define SYSCTRL_TICK_TIMER_IRQ_CONTROL_OFFSET 0x00000054
/// Index of the TICK_TIMER_IRQ_CONTROL register
#define SYSCTRL_TICK_TIMER_IRQ_CONTROL_INDEX  0x00000015
/// Reset value of the TICK_TIMER_IRQ_CONTROL register
#define SYSCTRL_TICK_TIMER_IRQ_CONTROL_RESET  0x00000000

/**
 * @brief Returns the current value of the TICK_TIMER_IRQ_CONTROL register.
 * The TICK_TIMER_IRQ_CONTROL register will be read and its value returned.
 * @return The current value of the TICK_TIMER_IRQ_CONTROL register.
 */
__INLINE uint32_t sysctrl_tick_timer_irq_control_get(void)
{
    return REG_PL_RD(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR);
}

/**
 * @brief Sets the TICK_TIMER_IRQ_CONTROL register to a value.
 * The TICK_TIMER_IRQ_CONTROL register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_tick_timer_irq_control_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR, value);
}

// field definitions
/// TICK_TIMER_EN field bit
#define SYSCTRL_TICK_TIMER_EN_BIT        ((uint32_t)0x00000002)
/// TICK_TIMER_EN field position
#define SYSCTRL_TICK_TIMER_EN_POS        1
/// TICK_TIMER_IRQ_EN field bit
#define SYSCTRL_TICK_TIMER_IRQ_EN_BIT    ((uint32_t)0x00000001)
/// TICK_TIMER_IRQ_EN field position
#define SYSCTRL_TICK_TIMER_IRQ_EN_POS    0

/// TICK_TIMER_EN field reset value
#define SYSCTRL_TICK_TIMER_EN_RST        0x0
/// TICK_TIMER_IRQ_EN field reset value
#define SYSCTRL_TICK_TIMER_IRQ_EN_RST    0x0

/**
 * @brief Constructs a value for the TICK_TIMER_IRQ_CONTROL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ticktimeren - The value to use for the tick_timer_en field.
 * @param[in] ticktimerirqen - The value to use for the tick_timer_irq_en field.
 */
__INLINE void sysctrl_tick_timer_irq_control_pack(uint8_t ticktimeren, uint8_t ticktimerirqen)
{
    ASSERT_ERR((((uint32_t)ticktimeren << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)ticktimerirqen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR,  ((uint32_t)ticktimeren << 1) | ((uint32_t)ticktimerirqen << 0));
}

/**
 * @brief Unpacks TICK_TIMER_IRQ_CONTROL's fields from current value of the TICK_TIMER_IRQ_CONTROL register.
 *
 * Reads the TICK_TIMER_IRQ_CONTROL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ticktimeren - Will be populated with the current value of this field from the register.
 * @param[out] ticktimerirqen - Will be populated with the current value of this field from the register.
 */
__INLINE void sysctrl_tick_timer_irq_control_unpack(uint8_t* ticktimeren, uint8_t* ticktimerirqen)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR);

    *ticktimeren = (localVal & ((uint32_t)0x00000002)) >> 1;
    *ticktimerirqen = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the tick_timer_en field in the TICK_TIMER_IRQ_CONTROL register.
 *
 * The TICK_TIMER_IRQ_CONTROL register will be read and the tick_timer_en field's value will be returned.
 *
 * @return The current value of the tick_timer_en field in the TICK_TIMER_IRQ_CONTROL register.
 */
__INLINE uint8_t sysctrl_tick_timer_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the tick_timer_en field of the TICK_TIMER_IRQ_CONTROL register.
 *
 * The TICK_TIMER_IRQ_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ticktimeren - The value to set the field to.
 */
__INLINE void sysctrl_tick_timer_en_setf(uint8_t ticktimeren)
{
    ASSERT_ERR((((uint32_t)ticktimeren << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR, (REG_PL_RD(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)ticktimeren << 1));
}

/**
 * @brief Returns the current value of the tick_timer_irq_en field in the TICK_TIMER_IRQ_CONTROL register.
 *
 * The TICK_TIMER_IRQ_CONTROL register will be read and the tick_timer_irq_en field's value will be returned.
 *
 * @return The current value of the tick_timer_irq_en field in the TICK_TIMER_IRQ_CONTROL register.
 */
__INLINE uint8_t sysctrl_tick_timer_irq_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the tick_timer_irq_en field of the TICK_TIMER_IRQ_CONTROL register.
 *
 * The TICK_TIMER_IRQ_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ticktimerirqen - The value to set the field to.
 */
__INLINE void sysctrl_tick_timer_irq_en_setf(uint8_t ticktimerirqen)
{
    ASSERT_ERR((((uint32_t)ticktimerirqen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR, (REG_PL_RD(SYSCTRL_TICK_TIMER_IRQ_CONTROL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)ticktimerirqen << 0));
}

/// @}

/**
 * @name SOFT_IRQ_SET register definitions
 * <table>
 * <caption>SOFT_IRQ_SET bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> soft_irq <td> R <td> S <td> 0
 * </table>
 *
 * @{
 */

/// Address of the SOFT_IRQ_SET register
#define SYSCTRL_SOFT_IRQ_SET_ADDR   0x24900058
/// Offset of the SOFT_IRQ_SET register from the base address
#define SYSCTRL_SOFT_IRQ_SET_OFFSET 0x00000058
/// Index of the SOFT_IRQ_SET register
#define SYSCTRL_SOFT_IRQ_SET_INDEX  0x00000016
/// Reset value of the SOFT_IRQ_SET register
#define SYSCTRL_SOFT_IRQ_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the SOFT_IRQ_SET register.
 * The SOFT_IRQ_SET register will be read and its value returned.
 * @return The current value of the SOFT_IRQ_SET register.
 */
__INLINE uint32_t sysctrl_soft_irq_get(void)
{
    return REG_PL_RD(SYSCTRL_SOFT_IRQ_SET_ADDR);
}

/**
 * @brief Sets the SOFT_IRQ_SET register to a value.
 * The SOFT_IRQ_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_soft_irq_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_SOFT_IRQ_SET_ADDR, value);
}

// field definitions
/// SOFT_IRQ field bit
#define SYSCTRL_SOFT_IRQ_BIT    ((uint32_t)0x00000001)
/// SOFT_IRQ field position
#define SYSCTRL_SOFT_IRQ_POS    0

/// SOFT_IRQ field reset value
#define SYSCTRL_SOFT_IRQ_RST    0x0

/**
 * @brief Returns the current value of the soft_irq field in the SOFT_IRQ_SET register.
 *
 * The SOFT_IRQ_SET register will be read and the soft_irq field's value will be returned.
 *
 * @return The current value of the soft_irq field in the SOFT_IRQ_SET register.
 */
__INLINE uint8_t sysctrl_soft_irq_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_SOFT_IRQ_SET_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the soft_irq field of the SOFT_IRQ_SET register.
 *
 * The SOFT_IRQ_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] softirq - The value to set the field to.
 */
__INLINE void sysctrl_soft_irq_setf(uint8_t softirq)
{
    ASSERT_ERR((((uint32_t)softirq << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_SOFT_IRQ_SET_ADDR, (uint32_t)softirq << 0);
}

/// @}

/**
 * @name SOFT_IRQ_CLEAR register definitions
 * <table>
 * <caption>SOFT_IRQ_CLEAR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> soft_irq <td> R <td> C <td> 0
 * </table>
 *
 * @{
 */

/// Address of the SOFT_IRQ_CLEAR register
#define SYSCTRL_SOFT_IRQ_CLEAR_ADDR   0x2490005C
/// Offset of the SOFT_IRQ_CLEAR register from the base address
#define SYSCTRL_SOFT_IRQ_CLEAR_OFFSET 0x0000005C
/// Index of the SOFT_IRQ_CLEAR register
#define SYSCTRL_SOFT_IRQ_CLEAR_INDEX  0x00000017
/// Reset value of the SOFT_IRQ_CLEAR register
#define SYSCTRL_SOFT_IRQ_CLEAR_RESET  0x00000000

/**
 * @brief Sets the SOFT_IRQ_CLEAR register to a value.
 * The SOFT_IRQ_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_soft_irq_clear(uint32_t value)
{
    REG_PL_WR(SYSCTRL_SOFT_IRQ_CLEAR_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Sets the soft_irq field of the SOFT_IRQ_CLEAR register.
 *
 * The SOFT_IRQ_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] softirq - The value to set the field to.
 */
__INLINE void sysctrl_soft_irq_clearf(uint8_t softirq)
{
    ASSERT_ERR((((uint32_t)softirq << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_SOFT_IRQ_CLEAR_ADDR, (uint32_t)softirq << 0);
}

/// @}

/**
 * @name DIAG_CONF0 register definitions
 * <table>
 * <caption>DIAG_CONF0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 29:24 <td>  diag_presel1 <td> R <td> R/W <td> 0x21
 * <tr><td> 17:16 <td>     diag_sel1 <td> R <td> R/W <td> 0x0
 * <tr><td> 13:08 <td>  diag_presel0 <td> R <td> R/W <td> 0x20
 * <tr><td> 01:00 <td>     diag_sel0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAG_CONF0 register
#define SYSCTRL_DIAG_CONF0_ADDR   0x24900064
/// Offset of the DIAG_CONF0 register from the base address
#define SYSCTRL_DIAG_CONF0_OFFSET 0x00000064
/// Index of the DIAG_CONF0 register
#define SYSCTRL_DIAG_CONF0_INDEX  0x00000019
/// Reset value of the DIAG_CONF0 register
#define SYSCTRL_DIAG_CONF0_RESET  0x21002000

/**
 * @brief Returns the current value of the DIAG_CONF0 register.
 * The DIAG_CONF0 register will be read and its value returned.
 * @return The current value of the DIAG_CONF0 register.
 */
__INLINE uint32_t sysctrl_diag_conf0_get(void)
{
    return REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR);
}

/**
 * @brief Sets the DIAG_CONF0 register to a value.
 * The DIAG_CONF0 register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_diag_conf0_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_DIAG_CONF0_ADDR, value);
}

// field definitions
/// DIAG_PRESEL_1 field mask
#define SYSCTRL_DIAG_PRESEL_1_MASK   ((uint32_t)0x3F000000)
/// DIAG_PRESEL_1 field LSB position
#define SYSCTRL_DIAG_PRESEL_1_LSB    24
/// DIAG_PRESEL_1 field width
#define SYSCTRL_DIAG_PRESEL_1_WIDTH  ((uint32_t)0x00000006)
/// DIAG_SEL_1 field mask
#define SYSCTRL_DIAG_SEL_1_MASK      ((uint32_t)0x00030000)
/// DIAG_SEL_1 field LSB position
#define SYSCTRL_DIAG_SEL_1_LSB       16
/// DIAG_SEL_1 field width
#define SYSCTRL_DIAG_SEL_1_WIDTH     ((uint32_t)0x00000002)
/// DIAG_PRESEL_0 field mask
#define SYSCTRL_DIAG_PRESEL_0_MASK   ((uint32_t)0x00003F00)
/// DIAG_PRESEL_0 field LSB position
#define SYSCTRL_DIAG_PRESEL_0_LSB    8
/// DIAG_PRESEL_0 field width
#define SYSCTRL_DIAG_PRESEL_0_WIDTH  ((uint32_t)0x00000006)
/// DIAG_SEL_0 field mask
#define SYSCTRL_DIAG_SEL_0_MASK      ((uint32_t)0x00000003)
/// DIAG_SEL_0 field LSB position
#define SYSCTRL_DIAG_SEL_0_LSB       0
/// DIAG_SEL_0 field width
#define SYSCTRL_DIAG_SEL_0_WIDTH     ((uint32_t)0x00000002)

/// DIAG_PRESEL_1 field reset value
#define SYSCTRL_DIAG_PRESEL_1_RST    0x21
/// DIAG_SEL_1 field reset value
#define SYSCTRL_DIAG_SEL_1_RST       0x0
/// DIAG_PRESEL_0 field reset value
#define SYSCTRL_DIAG_PRESEL_0_RST    0x20
/// DIAG_SEL_0 field reset value
#define SYSCTRL_DIAG_SEL_0_RST       0x0

/**
 * @brief Constructs a value for the DIAG_CONF0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] diagpresel1 - The value to use for the diag_presel1 field.
 * @param[in] diagsel1 - The value to use for the diag_sel1 field.
 * @param[in] diagpresel0 - The value to use for the diag_presel0 field.
 * @param[in] diagsel0 - The value to use for the diag_sel0 field.
 */
__INLINE void sysctrl_diag_conf0_pack(uint8_t diagpresel1, uint8_t diagsel1, uint8_t diagpresel0, uint8_t diagsel0)
{
    ASSERT_ERR((((uint32_t)diagpresel1 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)diagsel1 << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)diagpresel0 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)diagsel0 << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF0_ADDR,  ((uint32_t)diagpresel1 << 24) | ((uint32_t)diagsel1 << 16) | ((uint32_t)diagpresel0 << 8) | ((uint32_t)diagsel0 << 0));
}

/**
 * @brief Unpacks DIAG_CONF0's fields from current value of the DIAG_CONF0 register.
 *
 * Reads the DIAG_CONF0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] diagpresel1 - Will be populated with the current value of this field from the register.
 * @param[out] diagsel1 - Will be populated with the current value of this field from the register.
 * @param[out] diagpresel0 - Will be populated with the current value of this field from the register.
 * @param[out] diagsel0 - Will be populated with the current value of this field from the register.
 */
__INLINE void sysctrl_diag_conf0_unpack(uint8_t* diagpresel1, uint8_t* diagsel1, uint8_t* diagpresel0, uint8_t* diagsel0)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR);

    *diagpresel1 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *diagsel1 = (localVal & ((uint32_t)0x00030000)) >> 16;
    *diagpresel0 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *diagsel0 = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the diag_presel1 field in the DIAG_CONF0 register.
 *
 * The DIAG_CONF0 register will be read and the diag_presel1 field's value will be returned.
 *
 * @return The current value of the diag_presel1 field in the DIAG_CONF0 register.
 */
__INLINE uint8_t sysctrl_diag_presel_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the diag_presel1 field of the DIAG_CONF0 register.
 *
 * The DIAG_CONF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagpresel1 - The value to set the field to.
 */
__INLINE void sysctrl_diag_presel_1_setf(uint8_t diagpresel1)
{
    ASSERT_ERR((((uint32_t)diagpresel1 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF0_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)diagpresel1 << 24));
}

/**
 * @brief Returns the current value of the diag_sel1 field in the DIAG_CONF0 register.
 *
 * The DIAG_CONF0 register will be read and the diag_sel1 field's value will be returned.
 *
 * @return The current value of the diag_sel1 field in the DIAG_CONF0 register.
 */
__INLINE uint8_t sysctrl_diag_sel_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the diag_sel1 field of the DIAG_CONF0 register.
 *
 * The DIAG_CONF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagsel1 - The value to set the field to.
 */
__INLINE void sysctrl_diag_sel_1_setf(uint8_t diagsel1)
{
    ASSERT_ERR((((uint32_t)diagsel1 << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF0_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)diagsel1 << 16));
}

/**
 * @brief Returns the current value of the diag_presel0 field in the DIAG_CONF0 register.
 *
 * The DIAG_CONF0 register will be read and the diag_presel0 field's value will be returned.
 *
 * @return The current value of the diag_presel0 field in the DIAG_CONF0 register.
 */
__INLINE uint8_t sysctrl_diag_presel_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the diag_presel0 field of the DIAG_CONF0 register.
 *
 * The DIAG_CONF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagpresel0 - The value to set the field to.
 */
__INLINE void sysctrl_diag_presel_0_setf(uint8_t diagpresel0)
{
    ASSERT_ERR((((uint32_t)diagpresel0 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF0_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)diagpresel0 << 8));
}

/**
 * @brief Returns the current value of the diag_sel0 field in the DIAG_CONF0 register.
 *
 * The DIAG_CONF0 register will be read and the diag_sel0 field's value will be returned.
 *
 * @return The current value of the diag_sel0 field in the DIAG_CONF0 register.
 */
__INLINE uint8_t sysctrl_diag_sel_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the diag_sel0 field of the DIAG_CONF0 register.
 *
 * The DIAG_CONF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagsel0 - The value to set the field to.
 */
__INLINE void sysctrl_diag_sel_0_setf(uint8_t diagsel0)
{
    ASSERT_ERR((((uint32_t)diagsel0 << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF0_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF0_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)diagsel0 << 0));
}

/// @}

/**
 * @name DIAG_CONF1 register definitions
 * <table>
 * <caption>DIAG_CONF1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>   diag_sel_en <td> R <td> R/W <td> 0
 * <tr><td> 29:24 <td>  diag_presel3 <td> R <td> R/W <td> 0x25
 * <tr><td> 17:16 <td>     diag_sel3 <td> R <td> R/W <td> 0x1
 * <tr><td> 15    <td>      diag_mux <td> R <td> R/W <td> 0
 * <tr><td> 13:08 <td>  diag_presel2 <td> R <td> R/W <td> 0x1C
 * <tr><td> 01:00 <td>     diag_sel2 <td> R <td> R/W <td> 0x1
 * </table>
 *
 * @{
 */

/// Address of the DIAG_CONF1 register
#define SYSCTRL_DIAG_CONF1_ADDR   0x24900068
/// Offset of the DIAG_CONF1 register from the base address
#define SYSCTRL_DIAG_CONF1_OFFSET 0x00000068
/// Index of the DIAG_CONF1 register
#define SYSCTRL_DIAG_CONF1_INDEX  0x0000001A
/// Reset value of the DIAG_CONF1 register
#define SYSCTRL_DIAG_CONF1_RESET  0x25011C01

/**
 * @brief Returns the current value of the DIAG_CONF1 register.
 * The DIAG_CONF1 register will be read and its value returned.
 * @return The current value of the DIAG_CONF1 register.
 */
__INLINE uint32_t sysctrl_diag_conf1_get(void)
{
    return REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR);
}

/**
 * @brief Sets the DIAG_CONF1 register to a value.
 * The DIAG_CONF1 register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_diag_conf1_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_DIAG_CONF1_ADDR, value);
}

// field definitions
/// DIAG_SEL_EN field bit
#define SYSCTRL_DIAG_SEL_EN_BIT      ((uint32_t)0x80000000)
/// DIAG_SEL_EN field position
#define SYSCTRL_DIAG_SEL_EN_POS      31
/// DIAG_PRESEL_3 field mask
#define SYSCTRL_DIAG_PRESEL_3_MASK   ((uint32_t)0x3F000000)
/// DIAG_PRESEL_3 field LSB position
#define SYSCTRL_DIAG_PRESEL_3_LSB    24
/// DIAG_PRESEL_3 field width
#define SYSCTRL_DIAG_PRESEL_3_WIDTH  ((uint32_t)0x00000006)
/// DIAG_SEL_3 field mask
#define SYSCTRL_DIAG_SEL_3_MASK      ((uint32_t)0x00030000)
/// DIAG_SEL_3 field LSB position
#define SYSCTRL_DIAG_SEL_3_LSB       16
/// DIAG_SEL_3 field width
#define SYSCTRL_DIAG_SEL_3_WIDTH     ((uint32_t)0x00000002)
/// DIAG_MUX field bit
#define SYSCTRL_DIAG_MUX_BIT         ((uint32_t)0x00008000)
/// DIAG_MUX field position
#define SYSCTRL_DIAG_MUX_POS         15
/// DIAG_PRESEL_2 field mask
#define SYSCTRL_DIAG_PRESEL_2_MASK   ((uint32_t)0x00003F00)
/// DIAG_PRESEL_2 field LSB position
#define SYSCTRL_DIAG_PRESEL_2_LSB    8
/// DIAG_PRESEL_2 field width
#define SYSCTRL_DIAG_PRESEL_2_WIDTH  ((uint32_t)0x00000006)
/// DIAG_SEL_2 field mask
#define SYSCTRL_DIAG_SEL_2_MASK      ((uint32_t)0x00000003)
/// DIAG_SEL_2 field LSB position
#define SYSCTRL_DIAG_SEL_2_LSB       0
/// DIAG_SEL_2 field width
#define SYSCTRL_DIAG_SEL_2_WIDTH     ((uint32_t)0x00000002)

/// DIAG_SEL_EN field reset value
#define SYSCTRL_DIAG_SEL_EN_RST      0x0
/// DIAG_PRESEL_3 field reset value
#define SYSCTRL_DIAG_PRESEL_3_RST    0x25
/// DIAG_SEL_3 field reset value
#define SYSCTRL_DIAG_SEL_3_RST       0x1
/// DIAG_MUX field reset value
#define SYSCTRL_DIAG_MUX_RST         0x0
/// DIAG_PRESEL_2 field reset value
#define SYSCTRL_DIAG_PRESEL_2_RST    0x1C
/// DIAG_SEL_2 field reset value
#define SYSCTRL_DIAG_SEL_2_RST       0x1

/**
 * @brief Constructs a value for the DIAG_CONF1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] diagselen - The value to use for the diag_sel_en field.
 * @param[in] diagpresel3 - The value to use for the diag_presel3 field.
 * @param[in] diagsel3 - The value to use for the diag_sel3 field.
 * @param[in] diagmux - The value to use for the diag_mux field.
 * @param[in] diagpresel2 - The value to use for the diag_presel2 field.
 * @param[in] diagsel2 - The value to use for the diag_sel2 field.
 */
__INLINE void sysctrl_diag_conf1_pack(uint8_t diagselen, uint8_t diagpresel3, uint8_t diagsel3, uint8_t diagmux, uint8_t diagpresel2, uint8_t diagsel2)
{
    ASSERT_ERR((((uint32_t)diagselen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)diagpresel3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)diagsel3 << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)diagmux << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)diagpresel2 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)diagsel2 << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF1_ADDR,  ((uint32_t)diagselen << 31) | ((uint32_t)diagpresel3 << 24) | ((uint32_t)diagsel3 << 16) | ((uint32_t)diagmux << 15) | ((uint32_t)diagpresel2 << 8) | ((uint32_t)diagsel2 << 0));
}

/**
 * @brief Unpacks DIAG_CONF1's fields from current value of the DIAG_CONF1 register.
 *
 * Reads the DIAG_CONF1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] diagselen - Will be populated with the current value of this field from the register.
 * @param[out] diagpresel3 - Will be populated with the current value of this field from the register.
 * @param[out] diagsel3 - Will be populated with the current value of this field from the register.
 * @param[out] diagmux - Will be populated with the current value of this field from the register.
 * @param[out] diagpresel2 - Will be populated with the current value of this field from the register.
 * @param[out] diagsel2 - Will be populated with the current value of this field from the register.
 */
__INLINE void sysctrl_diag_conf1_unpack(uint8_t* diagselen, uint8_t* diagpresel3, uint8_t* diagsel3, uint8_t* diagmux, uint8_t* diagpresel2, uint8_t* diagsel2)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR);

    *diagselen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *diagpresel3 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *diagsel3 = (localVal & ((uint32_t)0x00030000)) >> 16;
    *diagmux = (localVal & ((uint32_t)0x00008000)) >> 15;
    *diagpresel2 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *diagsel2 = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the diag_sel_en field in the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read and the diag_sel_en field's value will be returned.
 *
 * @return The current value of the diag_sel_en field in the DIAG_CONF1 register.
 */
__INLINE uint8_t sysctrl_diag_sel_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the diag_sel_en field of the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagselen - The value to set the field to.
 */
__INLINE void sysctrl_diag_sel_en_setf(uint8_t diagselen)
{
    ASSERT_ERR((((uint32_t)diagselen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF1_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)diagselen << 31));
}

/**
 * @brief Returns the current value of the diag_presel3 field in the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read and the diag_presel3 field's value will be returned.
 *
 * @return The current value of the diag_presel3 field in the DIAG_CONF1 register.
 */
__INLINE uint8_t sysctrl_diag_presel_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the diag_presel3 field of the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagpresel3 - The value to set the field to.
 */
__INLINE void sysctrl_diag_presel_3_setf(uint8_t diagpresel3)
{
    ASSERT_ERR((((uint32_t)diagpresel3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF1_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)diagpresel3 << 24));
}

/**
 * @brief Returns the current value of the diag_sel3 field in the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read and the diag_sel3 field's value will be returned.
 *
 * @return The current value of the diag_sel3 field in the DIAG_CONF1 register.
 */
__INLINE uint8_t sysctrl_diag_sel_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the diag_sel3 field of the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagsel3 - The value to set the field to.
 */
__INLINE void sysctrl_diag_sel_3_setf(uint8_t diagsel3)
{
    ASSERT_ERR((((uint32_t)diagsel3 << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF1_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)diagsel3 << 16));
}

/**
 * @brief Returns the current value of the diag_mux field in the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read and the diag_mux field's value will be returned.
 *
 * @return The current value of the diag_mux field in the DIAG_CONF1 register.
 */
__INLINE uint8_t sysctrl_diag_mux_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the diag_mux field of the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagmux - The value to set the field to.
 */
__INLINE void sysctrl_diag_mux_setf(uint8_t diagmux)
{
    ASSERT_ERR((((uint32_t)diagmux << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF1_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)diagmux << 15));
}

/**
 * @brief Returns the current value of the diag_presel2 field in the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read and the diag_presel2 field's value will be returned.
 *
 * @return The current value of the diag_presel2 field in the DIAG_CONF1 register.
 */
__INLINE uint8_t sysctrl_diag_presel_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the diag_presel2 field of the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagpresel2 - The value to set the field to.
 */
__INLINE void sysctrl_diag_presel_2_setf(uint8_t diagpresel2)
{
    ASSERT_ERR((((uint32_t)diagpresel2 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF1_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)diagpresel2 << 8));
}

/**
 * @brief Returns the current value of the diag_sel2 field in the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read and the diag_sel2 field's value will be returned.
 *
 * @return The current value of the diag_sel2 field in the DIAG_CONF1 register.
 */
__INLINE uint8_t sysctrl_diag_sel_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the diag_sel2 field of the DIAG_CONF1 register.
 *
 * The DIAG_CONF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagsel2 - The value to set the field to.
 */
__INLINE void sysctrl_diag_sel_2_setf(uint8_t diagsel2)
{
    ASSERT_ERR((((uint32_t)diagsel2 << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF1_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF1_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)diagsel2 << 0));
}

/// @}

/**
 * @name DIAG_STAT register definitions
 * <table>
 * <caption>DIAG_STAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> diag_value <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAG_STAT register
#define SYSCTRL_DIAG_STAT_ADDR   0x2490006C
/// Offset of the DIAG_STAT register from the base address
#define SYSCTRL_DIAG_STAT_OFFSET 0x0000006C
/// Index of the DIAG_STAT register
#define SYSCTRL_DIAG_STAT_INDEX  0x0000001B
/// Reset value of the DIAG_STAT register
#define SYSCTRL_DIAG_STAT_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAG_STAT register.
 * The DIAG_STAT register will be read and its value returned.
 * @return The current value of the DIAG_STAT register.
 */
__INLINE uint32_t sysctrl_diag_stat_get(void)
{
    return REG_PL_RD(SYSCTRL_DIAG_STAT_ADDR);
}

// field definitions
/// DIAG_VALUE field mask
#define SYSCTRL_DIAG_VALUE_MASK   ((uint32_t)0xFFFFFFFF)
/// DIAG_VALUE field LSB position
#define SYSCTRL_DIAG_VALUE_LSB    0
/// DIAG_VALUE field width
#define SYSCTRL_DIAG_VALUE_WIDTH  ((uint32_t)0x00000020)

/// DIAG_VALUE field reset value
#define SYSCTRL_DIAG_VALUE_RST    0x0

/**
 * @brief Returns the current value of the diag_value field in the DIAG_STAT register.
 *
 * The DIAG_STAT register will be read and the diag_value field's value will be returned.
 *
 * @return The current value of the diag_value field in the DIAG_STAT register.
 */
__INLINE uint32_t sysctrl_diag_value_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_STAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DIAG_TRIGGER register definitions
 * <table>
 * <caption>DIAG_TRIGGER bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> diag_trigger <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the DIAG_TRIGGER register
#define SYSCTRL_DIAG_TRIGGER_ADDR   0x24900070
/// Offset of the DIAG_TRIGGER register from the base address
#define SYSCTRL_DIAG_TRIGGER_OFFSET 0x00000070
/// Index of the DIAG_TRIGGER register
#define SYSCTRL_DIAG_TRIGGER_INDEX  0x0000001C
/// Reset value of the DIAG_TRIGGER register
#define SYSCTRL_DIAG_TRIGGER_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAG_TRIGGER register.
 * The DIAG_TRIGGER register will be read and its value returned.
 * @return The current value of the DIAG_TRIGGER register.
 */
__INLINE uint32_t sysctrl_diag_trigger_get(void)
{
    return REG_PL_RD(SYSCTRL_DIAG_TRIGGER_ADDR);
}

/**
 * @brief Sets the DIAG_TRIGGER register to a value.
 * The DIAG_TRIGGER register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_diag_trigger_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_DIAG_TRIGGER_ADDR, value);
}

// field definitions
/// DIAG_TRIGGER field bit
#define SYSCTRL_DIAG_TRIGGER_BIT    ((uint32_t)0x00000001)
/// DIAG_TRIGGER field position
#define SYSCTRL_DIAG_TRIGGER_POS    0

/// DIAG_TRIGGER field reset value
#define SYSCTRL_DIAG_TRIGGER_RST    0x0

/**
 * @brief Returns the current value of the diag_trigger field in the DIAG_TRIGGER register.
 *
 * The DIAG_TRIGGER register will be read and the diag_trigger field's value will be returned.
 *
 * @return The current value of the diag_trigger field in the DIAG_TRIGGER register.
 */
__INLINE uint8_t sysctrl_diag_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_TRIGGER_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the diag_trigger field of the DIAG_TRIGGER register.
 *
 * The DIAG_TRIGGER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagtrigger - The value to set the field to.
 */
__INLINE void sysctrl_diag_trigger_setf(uint8_t diagtrigger)
{
    ASSERT_ERR((((uint32_t)diagtrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_TRIGGER_ADDR, (uint32_t)diagtrigger << 0);
}

/// @}

/**
 * @name DIAG_CONF2 register definitions
 * <table>
 * <caption>DIAG_CONF2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 29:24 <td>  diag_presel5 <td> R <td> R/W <td> 0x1F
 * <tr><td> 17:16 <td>     diag_sel5 <td> R <td> R/W <td> 0x1
 * <tr><td> 13:08 <td>  diag_presel4 <td> R <td> R/W <td> 0x1E
 * <tr><td> 01:00 <td>     diag_sel4 <td> R <td> R/W <td> 0x1
 * </table>
 *
 * @{
 */

/// Address of the DIAG_CONF2 register
#define SYSCTRL_DIAG_CONF2_ADDR   0x24900074
/// Offset of the DIAG_CONF2 register from the base address
#define SYSCTRL_DIAG_CONF2_OFFSET 0x00000074
/// Index of the DIAG_CONF2 register
#define SYSCTRL_DIAG_CONF2_INDEX  0x0000001D
/// Reset value of the DIAG_CONF2 register
#define SYSCTRL_DIAG_CONF2_RESET  0x1F011E01

/**
 * @brief Returns the current value of the DIAG_CONF2 register.
 * The DIAG_CONF2 register will be read and its value returned.
 * @return The current value of the DIAG_CONF2 register.
 */
__INLINE uint32_t sysctrl_diag_conf2_get(void)
{
    return REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR);
}

/**
 * @brief Sets the DIAG_CONF2 register to a value.
 * The DIAG_CONF2 register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_diag_conf2_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_DIAG_CONF2_ADDR, value);
}

// field definitions
/// DIAG_PRESEL_5 field mask
#define SYSCTRL_DIAG_PRESEL_5_MASK   ((uint32_t)0x3F000000)
/// DIAG_PRESEL_5 field LSB position
#define SYSCTRL_DIAG_PRESEL_5_LSB    24
/// DIAG_PRESEL_5 field width
#define SYSCTRL_DIAG_PRESEL_5_WIDTH  ((uint32_t)0x00000006)
/// DIAG_SEL_5 field mask
#define SYSCTRL_DIAG_SEL_5_MASK      ((uint32_t)0x00030000)
/// DIAG_SEL_5 field LSB position
#define SYSCTRL_DIAG_SEL_5_LSB       16
/// DIAG_SEL_5 field width
#define SYSCTRL_DIAG_SEL_5_WIDTH     ((uint32_t)0x00000002)
/// DIAG_PRESEL_4 field mask
#define SYSCTRL_DIAG_PRESEL_4_MASK   ((uint32_t)0x00003F00)
/// DIAG_PRESEL_4 field LSB position
#define SYSCTRL_DIAG_PRESEL_4_LSB    8
/// DIAG_PRESEL_4 field width
#define SYSCTRL_DIAG_PRESEL_4_WIDTH  ((uint32_t)0x00000006)
/// DIAG_SEL_4 field mask
#define SYSCTRL_DIAG_SEL_4_MASK      ((uint32_t)0x00000003)
/// DIAG_SEL_4 field LSB position
#define SYSCTRL_DIAG_SEL_4_LSB       0
/// DIAG_SEL_4 field width
#define SYSCTRL_DIAG_SEL_4_WIDTH     ((uint32_t)0x00000002)

/// DIAG_PRESEL_5 field reset value
#define SYSCTRL_DIAG_PRESEL_5_RST    0x1F
/// DIAG_SEL_5 field reset value
#define SYSCTRL_DIAG_SEL_5_RST       0x1
/// DIAG_PRESEL_4 field reset value
#define SYSCTRL_DIAG_PRESEL_4_RST    0x1E
/// DIAG_SEL_4 field reset value
#define SYSCTRL_DIAG_SEL_4_RST       0x1

/**
 * @brief Constructs a value for the DIAG_CONF2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] diagpresel5 - The value to use for the diag_presel5 field.
 * @param[in] diagsel5 - The value to use for the diag_sel5 field.
 * @param[in] diagpresel4 - The value to use for the diag_presel4 field.
 * @param[in] diagsel4 - The value to use for the diag_sel4 field.
 */
__INLINE void sysctrl_diag_conf2_pack(uint8_t diagpresel5, uint8_t diagsel5, uint8_t diagpresel4, uint8_t diagsel4)
{
    ASSERT_ERR((((uint32_t)diagpresel5 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)diagsel5 << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)diagpresel4 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)diagsel4 << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF2_ADDR,  ((uint32_t)diagpresel5 << 24) | ((uint32_t)diagsel5 << 16) | ((uint32_t)diagpresel4 << 8) | ((uint32_t)diagsel4 << 0));
}

/**
 * @brief Unpacks DIAG_CONF2's fields from current value of the DIAG_CONF2 register.
 *
 * Reads the DIAG_CONF2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] diagpresel5 - Will be populated with the current value of this field from the register.
 * @param[out] diagsel5 - Will be populated with the current value of this field from the register.
 * @param[out] diagpresel4 - Will be populated with the current value of this field from the register.
 * @param[out] diagsel4 - Will be populated with the current value of this field from the register.
 */
__INLINE void sysctrl_diag_conf2_unpack(uint8_t* diagpresel5, uint8_t* diagsel5, uint8_t* diagpresel4, uint8_t* diagsel4)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR);

    *diagpresel5 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *diagsel5 = (localVal & ((uint32_t)0x00030000)) >> 16;
    *diagpresel4 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *diagsel4 = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the diag_presel5 field in the DIAG_CONF2 register.
 *
 * The DIAG_CONF2 register will be read and the diag_presel5 field's value will be returned.
 *
 * @return The current value of the diag_presel5 field in the DIAG_CONF2 register.
 */
__INLINE uint8_t sysctrl_diag_presel_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the diag_presel5 field of the DIAG_CONF2 register.
 *
 * The DIAG_CONF2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagpresel5 - The value to set the field to.
 */
__INLINE void sysctrl_diag_presel_5_setf(uint8_t diagpresel5)
{
    ASSERT_ERR((((uint32_t)diagpresel5 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF2_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)diagpresel5 << 24));
}

/**
 * @brief Returns the current value of the diag_sel5 field in the DIAG_CONF2 register.
 *
 * The DIAG_CONF2 register will be read and the diag_sel5 field's value will be returned.
 *
 * @return The current value of the diag_sel5 field in the DIAG_CONF2 register.
 */
__INLINE uint8_t sysctrl_diag_sel_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the diag_sel5 field of the DIAG_CONF2 register.
 *
 * The DIAG_CONF2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagsel5 - The value to set the field to.
 */
__INLINE void sysctrl_diag_sel_5_setf(uint8_t diagsel5)
{
    ASSERT_ERR((((uint32_t)diagsel5 << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF2_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)diagsel5 << 16));
}

/**
 * @brief Returns the current value of the diag_presel4 field in the DIAG_CONF2 register.
 *
 * The DIAG_CONF2 register will be read and the diag_presel4 field's value will be returned.
 *
 * @return The current value of the diag_presel4 field in the DIAG_CONF2 register.
 */
__INLINE uint8_t sysctrl_diag_presel_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the diag_presel4 field of the DIAG_CONF2 register.
 *
 * The DIAG_CONF2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagpresel4 - The value to set the field to.
 */
__INLINE void sysctrl_diag_presel_4_setf(uint8_t diagpresel4)
{
    ASSERT_ERR((((uint32_t)diagpresel4 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF2_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)diagpresel4 << 8));
}

/**
 * @brief Returns the current value of the diag_sel4 field in the DIAG_CONF2 register.
 *
 * The DIAG_CONF2 register will be read and the diag_sel4 field's value will be returned.
 *
 * @return The current value of the diag_sel4 field in the DIAG_CONF2 register.
 */
__INLINE uint8_t sysctrl_diag_sel_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the diag_sel4 field of the DIAG_CONF2 register.
 *
 * The DIAG_CONF2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagsel4 - The value to set the field to.
 */
__INLINE void sysctrl_diag_sel_4_setf(uint8_t diagsel4)
{
    ASSERT_ERR((((uint32_t)diagsel4 << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF2_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF2_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)diagsel4 << 0));
}

/// @}

/**
 * @name DIAG_CONF3 register definitions
 * <table>
 * <caption>DIAG_CONF3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 13:08 <td>  diag_presel6 <td> R <td> R/W <td> 0x26
 * <tr><td> 01:00 <td>     diag_sel6 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DIAG_CONF3 register
#define SYSCTRL_DIAG_CONF3_ADDR   0x24900078
/// Offset of the DIAG_CONF3 register from the base address
#define SYSCTRL_DIAG_CONF3_OFFSET 0x00000078
/// Index of the DIAG_CONF3 register
#define SYSCTRL_DIAG_CONF3_INDEX  0x0000001E
/// Reset value of the DIAG_CONF3 register
#define SYSCTRL_DIAG_CONF3_RESET  0x00002600

/**
 * @brief Returns the current value of the DIAG_CONF3 register.
 * The DIAG_CONF3 register will be read and its value returned.
 * @return The current value of the DIAG_CONF3 register.
 */
__INLINE uint32_t sysctrl_diag_conf3_get(void)
{
    return REG_PL_RD(SYSCTRL_DIAG_CONF3_ADDR);
}

/**
 * @brief Sets the DIAG_CONF3 register to a value.
 * The DIAG_CONF3 register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_diag_conf3_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_DIAG_CONF3_ADDR, value);
}

// field definitions
/// DIAG_PRESEL_6 field mask
#define SYSCTRL_DIAG_PRESEL_6_MASK   ((uint32_t)0x00003F00)
/// DIAG_PRESEL_6 field LSB position
#define SYSCTRL_DIAG_PRESEL_6_LSB    8
/// DIAG_PRESEL_6 field width
#define SYSCTRL_DIAG_PRESEL_6_WIDTH  ((uint32_t)0x00000006)
/// DIAG_SEL_6 field mask
#define SYSCTRL_DIAG_SEL_6_MASK      ((uint32_t)0x00000003)
/// DIAG_SEL_6 field LSB position
#define SYSCTRL_DIAG_SEL_6_LSB       0
/// DIAG_SEL_6 field width
#define SYSCTRL_DIAG_SEL_6_WIDTH     ((uint32_t)0x00000002)

/// DIAG_PRESEL_6 field reset value
#define SYSCTRL_DIAG_PRESEL_6_RST    0x26
/// DIAG_SEL_6 field reset value
#define SYSCTRL_DIAG_SEL_6_RST       0x0

/**
 * @brief Constructs a value for the DIAG_CONF3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] diagpresel6 - The value to use for the diag_presel6 field.
 * @param[in] diagsel6 - The value to use for the diag_sel6 field.
 */
__INLINE void sysctrl_diag_conf3_pack(uint8_t diagpresel6, uint8_t diagsel6)
{
    ASSERT_ERR((((uint32_t)diagpresel6 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)diagsel6 << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF3_ADDR,  ((uint32_t)diagpresel6 << 8) | ((uint32_t)diagsel6 << 0));
}

/**
 * @brief Unpacks DIAG_CONF3's fields from current value of the DIAG_CONF3 register.
 *
 * Reads the DIAG_CONF3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] diagpresel6 - Will be populated with the current value of this field from the register.
 * @param[out] diagsel6 - Will be populated with the current value of this field from the register.
 */
__INLINE void sysctrl_diag_conf3_unpack(uint8_t* diagpresel6, uint8_t* diagsel6)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF3_ADDR);

    *diagpresel6 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *diagsel6 = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the diag_presel6 field in the DIAG_CONF3 register.
 *
 * The DIAG_CONF3 register will be read and the diag_presel6 field's value will be returned.
 *
 * @return The current value of the diag_presel6 field in the DIAG_CONF3 register.
 */
__INLINE uint8_t sysctrl_diag_presel_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF3_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the diag_presel6 field of the DIAG_CONF3 register.
 *
 * The DIAG_CONF3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagpresel6 - The value to set the field to.
 */
__INLINE void sysctrl_diag_presel_6_setf(uint8_t diagpresel6)
{
    ASSERT_ERR((((uint32_t)diagpresel6 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF3_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF3_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)diagpresel6 << 8));
}

/**
 * @brief Returns the current value of the diag_sel6 field in the DIAG_CONF3 register.
 *
 * The DIAG_CONF3 register will be read and the diag_sel6 field's value will be returned.
 *
 * @return The current value of the diag_sel6 field in the DIAG_CONF3 register.
 */
__INLINE uint8_t sysctrl_diag_sel_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_CONF3_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the diag_sel6 field of the DIAG_CONF3 register.
 *
 * The DIAG_CONF3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagsel6 - The value to set the field to.
 */
__INLINE void sysctrl_diag_sel_6_setf(uint8_t diagsel6)
{
    ASSERT_ERR((((uint32_t)diagsel6 << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_CONF3_ADDR, (REG_PL_RD(SYSCTRL_DIAG_CONF3_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)diagsel6 << 0));
}

/// @}

/**
 * @name DIAG_MPIF_CONF register definitions
 * <table>
 * <caption>DIAG_MPIF_CONF bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 09:04 <td> diag_mpif_hdr_nbr <td> R <td> R/W <td> 0x0
 * <tr><td> 00    <td> diag_mpif_mask_en <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the DIAG_MPIF_CONF register
#define SYSCTRL_DIAG_MPIF_CONF_ADDR   0x2490007C
/// Offset of the DIAG_MPIF_CONF register from the base address
#define SYSCTRL_DIAG_MPIF_CONF_OFFSET 0x0000007C
/// Index of the DIAG_MPIF_CONF register
#define SYSCTRL_DIAG_MPIF_CONF_INDEX  0x0000001F
/// Reset value of the DIAG_MPIF_CONF register
#define SYSCTRL_DIAG_MPIF_CONF_RESET  0x00000000

/**
 * @brief Returns the current value of the DIAG_MPIF_CONF register.
 * The DIAG_MPIF_CONF register will be read and its value returned.
 * @return The current value of the DIAG_MPIF_CONF register.
 */
__INLINE uint32_t sysctrl_diag_mpif_conf_get(void)
{
    return REG_PL_RD(SYSCTRL_DIAG_MPIF_CONF_ADDR);
}

/**
 * @brief Sets the DIAG_MPIF_CONF register to a value.
 * The DIAG_MPIF_CONF register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_diag_mpif_conf_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_DIAG_MPIF_CONF_ADDR, value);
}

// field definitions
/// DIAG_MPIF_HDR_NBR field mask
#define SYSCTRL_DIAG_MPIF_HDR_NBR_MASK   ((uint32_t)0x000003F0)
/// DIAG_MPIF_HDR_NBR field LSB position
#define SYSCTRL_DIAG_MPIF_HDR_NBR_LSB    4
/// DIAG_MPIF_HDR_NBR field width
#define SYSCTRL_DIAG_MPIF_HDR_NBR_WIDTH  ((uint32_t)0x00000006)
/// DIAG_MPIF_MASK_EN field bit
#define SYSCTRL_DIAG_MPIF_MASK_EN_BIT    ((uint32_t)0x00000001)
/// DIAG_MPIF_MASK_EN field position
#define SYSCTRL_DIAG_MPIF_MASK_EN_POS    0

/// DIAG_MPIF_HDR_NBR field reset value
#define SYSCTRL_DIAG_MPIF_HDR_NBR_RST    0x0
/// DIAG_MPIF_MASK_EN field reset value
#define SYSCTRL_DIAG_MPIF_MASK_EN_RST    0x0

/**
 * @brief Constructs a value for the DIAG_MPIF_CONF register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] diagmpifhdrnbr - The value to use for the diag_mpif_hdr_nbr field.
 * @param[in] diagmpifmasken - The value to use for the diag_mpif_mask_en field.
 */
__INLINE void sysctrl_diag_mpif_conf_pack(uint8_t diagmpifhdrnbr, uint8_t diagmpifmasken)
{
    ASSERT_ERR((((uint32_t)diagmpifhdrnbr << 4) & ~((uint32_t)0x000003F0)) == 0);
    ASSERT_ERR((((uint32_t)diagmpifmasken << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_MPIF_CONF_ADDR,  ((uint32_t)diagmpifhdrnbr << 4) | ((uint32_t)diagmpifmasken << 0));
}

/**
 * @brief Unpacks DIAG_MPIF_CONF's fields from current value of the DIAG_MPIF_CONF register.
 *
 * Reads the DIAG_MPIF_CONF register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] diagmpifhdrnbr - Will be populated with the current value of this field from the register.
 * @param[out] diagmpifmasken - Will be populated with the current value of this field from the register.
 */
__INLINE void sysctrl_diag_mpif_conf_unpack(uint8_t* diagmpifhdrnbr, uint8_t* diagmpifmasken)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_MPIF_CONF_ADDR);

    *diagmpifhdrnbr = (localVal & ((uint32_t)0x000003F0)) >> 4;
    *diagmpifmasken = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the diag_mpif_hdr_nbr field in the DIAG_MPIF_CONF register.
 *
 * The DIAG_MPIF_CONF register will be read and the diag_mpif_hdr_nbr field's value will be returned.
 *
 * @return The current value of the diag_mpif_hdr_nbr field in the DIAG_MPIF_CONF register.
 */
__INLINE uint8_t sysctrl_diag_mpif_hdr_nbr_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_MPIF_CONF_ADDR);
    return ((localVal & ((uint32_t)0x000003F0)) >> 4);
}

/**
 * @brief Sets the diag_mpif_hdr_nbr field of the DIAG_MPIF_CONF register.
 *
 * The DIAG_MPIF_CONF register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagmpifhdrnbr - The value to set the field to.
 */
__INLINE void sysctrl_diag_mpif_hdr_nbr_setf(uint8_t diagmpifhdrnbr)
{
    ASSERT_ERR((((uint32_t)diagmpifhdrnbr << 4) & ~((uint32_t)0x000003F0)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_MPIF_CONF_ADDR, (REG_PL_RD(SYSCTRL_DIAG_MPIF_CONF_ADDR) & ~((uint32_t)0x000003F0)) | ((uint32_t)diagmpifhdrnbr << 4));
}

/**
 * @brief Returns the current value of the diag_mpif_mask_en field in the DIAG_MPIF_CONF register.
 *
 * The DIAG_MPIF_CONF register will be read and the diag_mpif_mask_en field's value will be returned.
 *
 * @return The current value of the diag_mpif_mask_en field in the DIAG_MPIF_CONF register.
 */
__INLINE uint8_t sysctrl_diag_mpif_mask_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_DIAG_MPIF_CONF_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the diag_mpif_mask_en field of the DIAG_MPIF_CONF register.
 *
 * The DIAG_MPIF_CONF register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diagmpifmasken - The value to set the field to.
 */
__INLINE void sysctrl_diag_mpif_mask_en_setf(uint8_t diagmpifmasken)
{
    ASSERT_ERR((((uint32_t)diagmpifmasken << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_DIAG_MPIF_CONF_ADDR, (REG_PL_RD(SYSCTRL_DIAG_MPIF_CONF_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)diagmpifmasken << 0));
}

/// @}

/**
 * @name GPIO_OEN register definitions
 * <table>
 * <caption>GPIO_OEN bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> gpio_oen <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the GPIO_OEN register
#define SYSCTRL_GPIO_OEN_ADDR   0x24900080
/// Offset of the GPIO_OEN register from the base address
#define SYSCTRL_GPIO_OEN_OFFSET 0x00000080
/// Index of the GPIO_OEN register
#define SYSCTRL_GPIO_OEN_INDEX  0x00000020
/// Reset value of the GPIO_OEN register
#define SYSCTRL_GPIO_OEN_RESET  0x00000000

/**
 * @brief Returns the current value of the GPIO_OEN register.
 * The GPIO_OEN register will be read and its value returned.
 * @return The current value of the GPIO_OEN register.
 */
__INLINE uint32_t sysctrl_gpio_oen_get(void)
{
    return REG_PL_RD(SYSCTRL_GPIO_OEN_ADDR);
}

/**
 * @brief Sets the GPIO_OEN register to a value.
 * The GPIO_OEN register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_gpio_oen_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_GPIO_OEN_ADDR, value);
}

// field definitions
/// GPIO_OEN field mask
#define SYSCTRL_GPIO_OEN_MASK   ((uint32_t)0xFFFFFFFF)
/// GPIO_OEN field LSB position
#define SYSCTRL_GPIO_OEN_LSB    0
/// GPIO_OEN field width
#define SYSCTRL_GPIO_OEN_WIDTH  ((uint32_t)0x00000020)

/// GPIO_OEN field reset value
#define SYSCTRL_GPIO_OEN_RST    0x0

/**
 * @brief Returns the current value of the gpio_oen field in the GPIO_OEN register.
 *
 * The GPIO_OEN register will be read and the gpio_oen field's value will be returned.
 *
 * @return The current value of the gpio_oen field in the GPIO_OEN register.
 */
__INLINE uint32_t sysctrl_gpio_oen_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_GPIO_OEN_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the gpio_oen field of the GPIO_OEN register.
 *
 * The GPIO_OEN register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gpiooen - The value to set the field to.
 */
__INLINE void sysctrl_gpio_oen_setf(uint32_t gpiooen)
{
    ASSERT_ERR((((uint32_t)gpiooen << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(SYSCTRL_GPIO_OEN_ADDR, (uint32_t)gpiooen << 0);
}

/// @}

/**
 * @name GPIO_OUT register definitions
 * <table>
 * <caption>GPIO_OUT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> gpio_out <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the GPIO_OUT register
#define SYSCTRL_GPIO_OUT_ADDR   0x24900084
/// Offset of the GPIO_OUT register from the base address
#define SYSCTRL_GPIO_OUT_OFFSET 0x00000084
/// Index of the GPIO_OUT register
#define SYSCTRL_GPIO_OUT_INDEX  0x00000021
/// Reset value of the GPIO_OUT register
#define SYSCTRL_GPIO_OUT_RESET  0x00000000

/**
 * @brief Returns the current value of the GPIO_OUT register.
 * The GPIO_OUT register will be read and its value returned.
 * @return The current value of the GPIO_OUT register.
 */
__INLINE uint32_t sysctrl_gpio_out_get(void)
{
    return REG_PL_RD(SYSCTRL_GPIO_OUT_ADDR);
}

/**
 * @brief Sets the GPIO_OUT register to a value.
 * The GPIO_OUT register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_gpio_out_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_GPIO_OUT_ADDR, value);
}

// field definitions
/// GPIO_OUT field mask
#define SYSCTRL_GPIO_OUT_MASK   ((uint32_t)0xFFFFFFFF)
/// GPIO_OUT field LSB position
#define SYSCTRL_GPIO_OUT_LSB    0
/// GPIO_OUT field width
#define SYSCTRL_GPIO_OUT_WIDTH  ((uint32_t)0x00000020)

/// GPIO_OUT field reset value
#define SYSCTRL_GPIO_OUT_RST    0x0

/**
 * @brief Returns the current value of the gpio_out field in the GPIO_OUT register.
 *
 * The GPIO_OUT register will be read and the gpio_out field's value will be returned.
 *
 * @return The current value of the gpio_out field in the GPIO_OUT register.
 */
__INLINE uint32_t sysctrl_gpio_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_GPIO_OUT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the gpio_out field of the GPIO_OUT register.
 *
 * The GPIO_OUT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gpioout - The value to set the field to.
 */
__INLINE void sysctrl_gpio_out_setf(uint32_t gpioout)
{
    ASSERT_ERR((((uint32_t)gpioout << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(SYSCTRL_GPIO_OUT_ADDR, (uint32_t)gpioout << 0);
}

/// @}

/**
 * @name GPIO_IN register definitions
 * <table>
 * <caption>GPIO_IN bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> gpio_in <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the GPIO_IN register
#define SYSCTRL_GPIO_IN_ADDR   0x24900088
/// Offset of the GPIO_IN register from the base address
#define SYSCTRL_GPIO_IN_OFFSET 0x00000088
/// Index of the GPIO_IN register
#define SYSCTRL_GPIO_IN_INDEX  0x00000022
/// Reset value of the GPIO_IN register
#define SYSCTRL_GPIO_IN_RESET  0x00000000

/**
 * @brief Returns the current value of the GPIO_IN register.
 * The GPIO_IN register will be read and its value returned.
 * @return The current value of the GPIO_IN register.
 */
__INLINE uint32_t sysctrl_gpio_in_get(void)
{
    return REG_PL_RD(SYSCTRL_GPIO_IN_ADDR);
}

// field definitions
/// GPIO_IN field mask
#define SYSCTRL_GPIO_IN_MASK   ((uint32_t)0xFFFFFFFF)
/// GPIO_IN field LSB position
#define SYSCTRL_GPIO_IN_LSB    0
/// GPIO_IN field width
#define SYSCTRL_GPIO_IN_WIDTH  ((uint32_t)0x00000020)

/// GPIO_IN field reset value
#define SYSCTRL_GPIO_IN_RST    0x0

/**
 * @brief Returns the current value of the gpio_in field in the GPIO_IN register.
 *
 * The GPIO_IN register will be read and the gpio_in field's value will be returned.
 *
 * @return The current value of the gpio_in field in the GPIO_IN register.
 */
__INLINE uint32_t sysctrl_gpio_in_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_GPIO_IN_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name MISC_CNTL register definitions
 * <table>
 * <caption>MISC_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31 <td>                   nmb_io_busy <td> W <td> R <td> 0
 * <tr><td> 20 <td>   sec_mac_pi_tx_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 19 <td>   sec_mac_crypt_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 18 <td> sec_mac_core_tx_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 17 <td>        sec_mpif_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 16 <td>          mac_pi_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 15 <td>       mac_pi_tx_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 14 <td>       mac_pi_rx_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 13 <td>        mac_core_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 12 <td>       mac_crypt_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 11 <td>     mac_core_tx_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 10 <td>     mac_core_rx_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 09 <td>          mac_wt_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 08 <td>            mpif_clk_gating_en <td> R <td> R/W <td> 0
 * <tr><td> 04 <td>                bootrom_enable <td> R <td> R/W <td> 0
 * <tr><td> 01 <td>               fpgab_reset_req <td> R <td> R/W <td> 0
 * <tr><td> 00 <td>                soft_reset_req <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the MISC_CNTL register
#define SYSCTRL_MISC_CNTL_ADDR   0x249000E0
/// Offset of the MISC_CNTL register from the base address
#define SYSCTRL_MISC_CNTL_OFFSET 0x000000E0
/// Index of the MISC_CNTL register
#define SYSCTRL_MISC_CNTL_INDEX  0x00000038
/// Reset value of the MISC_CNTL register
#define SYSCTRL_MISC_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the MISC_CNTL register.
 * The MISC_CNTL register will be read and its value returned.
 * @return The current value of the MISC_CNTL register.
 */
__INLINE uint32_t sysctrl_misc_cntl_get(void)
{
    return REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
}

/**
 * @brief Sets the MISC_CNTL register to a value.
 * The MISC_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void sysctrl_misc_cntl_set(uint32_t value)
{
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, value);
}

// field definitions
/// NMB_IO_BUSY field bit
#define SYSCTRL_NMB_IO_BUSY_BIT                      ((uint32_t)0x80000000)
/// NMB_IO_BUSY field position
#define SYSCTRL_NMB_IO_BUSY_POS                      31
/// SEC_MAC_PI_TX_CLK_GATING_EN field bit
#define SYSCTRL_SEC_MAC_PI_TX_CLK_GATING_EN_BIT      ((uint32_t)0x00100000)
/// SEC_MAC_PI_TX_CLK_GATING_EN field position
#define SYSCTRL_SEC_MAC_PI_TX_CLK_GATING_EN_POS      20
/// SEC_MAC_CRYPT_CLK_GATING_EN field bit
#define SYSCTRL_SEC_MAC_CRYPT_CLK_GATING_EN_BIT      ((uint32_t)0x00080000)
/// SEC_MAC_CRYPT_CLK_GATING_EN field position
#define SYSCTRL_SEC_MAC_CRYPT_CLK_GATING_EN_POS      19
/// SEC_MAC_CORE_TX_CLK_GATING_EN field bit
#define SYSCTRL_SEC_MAC_CORE_TX_CLK_GATING_EN_BIT    ((uint32_t)0x00040000)
/// SEC_MAC_CORE_TX_CLK_GATING_EN field position
#define SYSCTRL_SEC_MAC_CORE_TX_CLK_GATING_EN_POS    18
/// SEC_MPIF_CLK_GATING_EN field bit
#define SYSCTRL_SEC_MPIF_CLK_GATING_EN_BIT           ((uint32_t)0x00020000)
/// SEC_MPIF_CLK_GATING_EN field position
#define SYSCTRL_SEC_MPIF_CLK_GATING_EN_POS           17
/// MAC_PI_CLK_GATING_EN field bit
#define SYSCTRL_MAC_PI_CLK_GATING_EN_BIT             ((uint32_t)0x00010000)
/// MAC_PI_CLK_GATING_EN field position
#define SYSCTRL_MAC_PI_CLK_GATING_EN_POS             16
/// MAC_PI_TX_CLK_GATING_EN field bit
#define SYSCTRL_MAC_PI_TX_CLK_GATING_EN_BIT          ((uint32_t)0x00008000)
/// MAC_PI_TX_CLK_GATING_EN field position
#define SYSCTRL_MAC_PI_TX_CLK_GATING_EN_POS          15
/// MAC_PI_RX_CLK_GATING_EN field bit
#define SYSCTRL_MAC_PI_RX_CLK_GATING_EN_BIT          ((uint32_t)0x00004000)
/// MAC_PI_RX_CLK_GATING_EN field position
#define SYSCTRL_MAC_PI_RX_CLK_GATING_EN_POS          14
/// MAC_CORE_CLK_GATING_EN field bit
#define SYSCTRL_MAC_CORE_CLK_GATING_EN_BIT           ((uint32_t)0x00002000)
/// MAC_CORE_CLK_GATING_EN field position
#define SYSCTRL_MAC_CORE_CLK_GATING_EN_POS           13
/// MAC_CRYPT_CLK_GATING_EN field bit
#define SYSCTRL_MAC_CRYPT_CLK_GATING_EN_BIT          ((uint32_t)0x00001000)
/// MAC_CRYPT_CLK_GATING_EN field position
#define SYSCTRL_MAC_CRYPT_CLK_GATING_EN_POS          12
/// MAC_CORE_TX_CLK_GATING_EN field bit
#define SYSCTRL_MAC_CORE_TX_CLK_GATING_EN_BIT        ((uint32_t)0x00000800)
/// MAC_CORE_TX_CLK_GATING_EN field position
#define SYSCTRL_MAC_CORE_TX_CLK_GATING_EN_POS        11
/// MAC_CORE_RX_CLK_GATING_EN field bit
#define SYSCTRL_MAC_CORE_RX_CLK_GATING_EN_BIT        ((uint32_t)0x00000400)
/// MAC_CORE_RX_CLK_GATING_EN field position
#define SYSCTRL_MAC_CORE_RX_CLK_GATING_EN_POS        10
/// MAC_WT_CLK_GATING_EN field bit
#define SYSCTRL_MAC_WT_CLK_GATING_EN_BIT             ((uint32_t)0x00000200)
/// MAC_WT_CLK_GATING_EN field position
#define SYSCTRL_MAC_WT_CLK_GATING_EN_POS             9
/// MPIF_CLK_GATING_EN field bit
#define SYSCTRL_MPIF_CLK_GATING_EN_BIT               ((uint32_t)0x00000100)
/// MPIF_CLK_GATING_EN field position
#define SYSCTRL_MPIF_CLK_GATING_EN_POS               8
/// BOOTROM_ENABLE field bit
#define SYSCTRL_BOOTROM_ENABLE_BIT                   ((uint32_t)0x00000010)
/// BOOTROM_ENABLE field position
#define SYSCTRL_BOOTROM_ENABLE_POS                   4
/// FPGAB_RESET_REQ field bit
#define SYSCTRL_FPGAB_RESET_REQ_BIT                  ((uint32_t)0x00000002)
/// FPGAB_RESET_REQ field position
#define SYSCTRL_FPGAB_RESET_REQ_POS                  1
/// SOFT_RESET_REQ field bit
#define SYSCTRL_SOFT_RESET_REQ_BIT                   ((uint32_t)0x00000001)
/// SOFT_RESET_REQ field position
#define SYSCTRL_SOFT_RESET_REQ_POS                   0

/// NMB_IO_BUSY field reset value
#define SYSCTRL_NMB_IO_BUSY_RST                      0x0
/// SEC_MAC_PI_TX_CLK_GATING_EN field reset value
#define SYSCTRL_SEC_MAC_PI_TX_CLK_GATING_EN_RST      0x0
/// SEC_MAC_CRYPT_CLK_GATING_EN field reset value
#define SYSCTRL_SEC_MAC_CRYPT_CLK_GATING_EN_RST      0x0
/// SEC_MAC_CORE_TX_CLK_GATING_EN field reset value
#define SYSCTRL_SEC_MAC_CORE_TX_CLK_GATING_EN_RST    0x0
/// SEC_MPIF_CLK_GATING_EN field reset value
#define SYSCTRL_SEC_MPIF_CLK_GATING_EN_RST           0x0
/// MAC_PI_CLK_GATING_EN field reset value
#define SYSCTRL_MAC_PI_CLK_GATING_EN_RST             0x0
/// MAC_PI_TX_CLK_GATING_EN field reset value
#define SYSCTRL_MAC_PI_TX_CLK_GATING_EN_RST          0x0
/// MAC_PI_RX_CLK_GATING_EN field reset value
#define SYSCTRL_MAC_PI_RX_CLK_GATING_EN_RST          0x0
/// MAC_CORE_CLK_GATING_EN field reset value
#define SYSCTRL_MAC_CORE_CLK_GATING_EN_RST           0x0
/// MAC_CRYPT_CLK_GATING_EN field reset value
#define SYSCTRL_MAC_CRYPT_CLK_GATING_EN_RST          0x0
/// MAC_CORE_TX_CLK_GATING_EN field reset value
#define SYSCTRL_MAC_CORE_TX_CLK_GATING_EN_RST        0x0
/// MAC_CORE_RX_CLK_GATING_EN field reset value
#define SYSCTRL_MAC_CORE_RX_CLK_GATING_EN_RST        0x0
/// MAC_WT_CLK_GATING_EN field reset value
#define SYSCTRL_MAC_WT_CLK_GATING_EN_RST             0x0
/// MPIF_CLK_GATING_EN field reset value
#define SYSCTRL_MPIF_CLK_GATING_EN_RST               0x0
/// BOOTROM_ENABLE field reset value
#define SYSCTRL_BOOTROM_ENABLE_RST                   0x0
/// FPGAB_RESET_REQ field reset value
#define SYSCTRL_FPGAB_RESET_REQ_RST                  0x0
/// SOFT_RESET_REQ field reset value
#define SYSCTRL_SOFT_RESET_REQ_RST                   0x0

/**
 * @brief Constructs a value for the MISC_CNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] secmacpitxclkgatingen - The value to use for the sec_mac_pi_tx_clk_gating_en field.
 * @param[in] secmaccryptclkgatingen - The value to use for the sec_mac_crypt_clk_gating_en field.
 * @param[in] secmaccoretxclkgatingen - The value to use for the sec_mac_core_tx_clk_gating_en field.
 * @param[in] secmpifclkgatingen - The value to use for the sec_mpif_clk_gating_en field.
 * @param[in] macpiclkgatingen - The value to use for the mac_pi_clk_gating_en field.
 * @param[in] macpitxclkgatingen - The value to use for the mac_pi_tx_clk_gating_en field.
 * @param[in] macpirxclkgatingen - The value to use for the mac_pi_rx_clk_gating_en field.
 * @param[in] maccoreclkgatingen - The value to use for the mac_core_clk_gating_en field.
 * @param[in] maccryptclkgatingen - The value to use for the mac_crypt_clk_gating_en field.
 * @param[in] maccoretxclkgatingen - The value to use for the mac_core_tx_clk_gating_en field.
 * @param[in] maccorerxclkgatingen - The value to use for the mac_core_rx_clk_gating_en field.
 * @param[in] macwtclkgatingen - The value to use for the mac_wt_clk_gating_en field.
 * @param[in] mpifclkgatingen - The value to use for the mpif_clk_gating_en field.
 * @param[in] bootromenable - The value to use for the bootrom_enable field.
 * @param[in] fpgabresetreq - The value to use for the fpgab_reset_req field.
 * @param[in] softresetreq - The value to use for the soft_reset_req field.
 */
__INLINE void sysctrl_misc_cntl_pack(uint8_t secmacpitxclkgatingen, uint8_t secmaccryptclkgatingen, uint8_t secmaccoretxclkgatingen, uint8_t secmpifclkgatingen, uint8_t macpiclkgatingen, uint8_t macpitxclkgatingen, uint8_t macpirxclkgatingen, uint8_t maccoreclkgatingen, uint8_t maccryptclkgatingen, uint8_t maccoretxclkgatingen, uint8_t maccorerxclkgatingen, uint8_t macwtclkgatingen, uint8_t mpifclkgatingen, uint8_t bootromenable, uint8_t fpgabresetreq, uint8_t softresetreq)
{
    ASSERT_ERR((((uint32_t)secmacpitxclkgatingen << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)secmaccryptclkgatingen << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)secmaccoretxclkgatingen << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)secmpifclkgatingen << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)macpiclkgatingen << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)macpitxclkgatingen << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)macpirxclkgatingen << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)maccoreclkgatingen << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)maccryptclkgatingen << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)maccoretxclkgatingen << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)maccorerxclkgatingen << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)macwtclkgatingen << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)mpifclkgatingen << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)bootromenable << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)fpgabresetreq << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)softresetreq << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR,  ((uint32_t)secmacpitxclkgatingen << 20) | ((uint32_t)secmaccryptclkgatingen << 19) | ((uint32_t)secmaccoretxclkgatingen << 18) | ((uint32_t)secmpifclkgatingen << 17) | ((uint32_t)macpiclkgatingen << 16) | ((uint32_t)macpitxclkgatingen << 15) | ((uint32_t)macpirxclkgatingen << 14) | ((uint32_t)maccoreclkgatingen << 13) | ((uint32_t)maccryptclkgatingen << 12) | ((uint32_t)maccoretxclkgatingen << 11) | ((uint32_t)maccorerxclkgatingen << 10) | ((uint32_t)macwtclkgatingen << 9) | ((uint32_t)mpifclkgatingen << 8) | ((uint32_t)bootromenable << 4) | ((uint32_t)fpgabresetreq << 1) | ((uint32_t)softresetreq << 0));
}

/**
 * @brief Unpacks MISC_CNTL's fields from current value of the MISC_CNTL register.
 *
 * Reads the MISC_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nmbiobusy - Will be populated with the current value of this field from the register.
 * @param[out] secmacpitxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] secmaccryptclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] secmaccoretxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] secmpifclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] macpiclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] macpitxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] macpirxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] maccoreclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] maccryptclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] maccoretxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] maccorerxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] macwtclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] mpifclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] bootromenable - Will be populated with the current value of this field from the register.
 * @param[out] fpgabresetreq - Will be populated with the current value of this field from the register.
 * @param[out] softresetreq - Will be populated with the current value of this field from the register.
 */
__INLINE void sysctrl_misc_cntl_unpack(uint8_t* nmbiobusy, uint8_t* secmacpitxclkgatingen, uint8_t* secmaccryptclkgatingen, uint8_t* secmaccoretxclkgatingen, uint8_t* secmpifclkgatingen, uint8_t* macpiclkgatingen, uint8_t* macpitxclkgatingen, uint8_t* macpirxclkgatingen, uint8_t* maccoreclkgatingen, uint8_t* maccryptclkgatingen, uint8_t* maccoretxclkgatingen, uint8_t* maccorerxclkgatingen, uint8_t* macwtclkgatingen, uint8_t* mpifclkgatingen, uint8_t* bootromenable, uint8_t* fpgabresetreq, uint8_t* softresetreq)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);

    *nmbiobusy = (localVal & ((uint32_t)0x80000000)) >> 31;
    *secmacpitxclkgatingen = (localVal & ((uint32_t)0x00100000)) >> 20;
    *secmaccryptclkgatingen = (localVal & ((uint32_t)0x00080000)) >> 19;
    *secmaccoretxclkgatingen = (localVal & ((uint32_t)0x00040000)) >> 18;
    *secmpifclkgatingen = (localVal & ((uint32_t)0x00020000)) >> 17;
    *macpiclkgatingen = (localVal & ((uint32_t)0x00010000)) >> 16;
    *macpitxclkgatingen = (localVal & ((uint32_t)0x00008000)) >> 15;
    *macpirxclkgatingen = (localVal & ((uint32_t)0x00004000)) >> 14;
    *maccoreclkgatingen = (localVal & ((uint32_t)0x00002000)) >> 13;
    *maccryptclkgatingen = (localVal & ((uint32_t)0x00001000)) >> 12;
    *maccoretxclkgatingen = (localVal & ((uint32_t)0x00000800)) >> 11;
    *maccorerxclkgatingen = (localVal & ((uint32_t)0x00000400)) >> 10;
    *macwtclkgatingen = (localVal & ((uint32_t)0x00000200)) >> 9;
    *mpifclkgatingen = (localVal & ((uint32_t)0x00000100)) >> 8;
    *bootromenable = (localVal & ((uint32_t)0x00000010)) >> 4;
    *fpgabresetreq = (localVal & ((uint32_t)0x00000002)) >> 1;
    *softresetreq = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the nmb_io_busy field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the nmb_io_busy field's value will be returned.
 *
 * @return The current value of the nmb_io_busy field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_nmb_io_busy_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the sec_mac_pi_tx_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the sec_mac_pi_tx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the sec_mac_pi_tx_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_sec_mac_pi_tx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the sec_mac_pi_tx_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secmacpitxclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_sec_mac_pi_tx_clk_gating_en_setf(uint8_t secmacpitxclkgatingen)
{
    ASSERT_ERR((((uint32_t)secmacpitxclkgatingen << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)secmacpitxclkgatingen << 20));
}

/**
 * @brief Returns the current value of the sec_mac_crypt_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the sec_mac_crypt_clk_gating_en field's value will be returned.
 *
 * @return The current value of the sec_mac_crypt_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_sec_mac_crypt_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the sec_mac_crypt_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secmaccryptclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_sec_mac_crypt_clk_gating_en_setf(uint8_t secmaccryptclkgatingen)
{
    ASSERT_ERR((((uint32_t)secmaccryptclkgatingen << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)secmaccryptclkgatingen << 19));
}

/**
 * @brief Returns the current value of the sec_mac_core_tx_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the sec_mac_core_tx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the sec_mac_core_tx_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_sec_mac_core_tx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the sec_mac_core_tx_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secmaccoretxclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_sec_mac_core_tx_clk_gating_en_setf(uint8_t secmaccoretxclkgatingen)
{
    ASSERT_ERR((((uint32_t)secmaccoretxclkgatingen << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)secmaccoretxclkgatingen << 18));
}

/**
 * @brief Returns the current value of the sec_mpif_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the sec_mpif_clk_gating_en field's value will be returned.
 *
 * @return The current value of the sec_mpif_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_sec_mpif_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the sec_mpif_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secmpifclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_sec_mpif_clk_gating_en_setf(uint8_t secmpifclkgatingen)
{
    ASSERT_ERR((((uint32_t)secmpifclkgatingen << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)secmpifclkgatingen << 17));
}

/**
 * @brief Returns the current value of the mac_pi_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mac_pi_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_pi_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mac_pi_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the mac_pi_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macpiclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mac_pi_clk_gating_en_setf(uint8_t macpiclkgatingen)
{
    ASSERT_ERR((((uint32_t)macpiclkgatingen << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)macpiclkgatingen << 16));
}

/**
 * @brief Returns the current value of the mac_pi_tx_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mac_pi_tx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_pi_tx_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mac_pi_tx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the mac_pi_tx_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macpitxclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mac_pi_tx_clk_gating_en_setf(uint8_t macpitxclkgatingen)
{
    ASSERT_ERR((((uint32_t)macpitxclkgatingen << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)macpitxclkgatingen << 15));
}

/**
 * @brief Returns the current value of the mac_pi_rx_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mac_pi_rx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_pi_rx_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mac_pi_rx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the mac_pi_rx_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macpirxclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mac_pi_rx_clk_gating_en_setf(uint8_t macpirxclkgatingen)
{
    ASSERT_ERR((((uint32_t)macpirxclkgatingen << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)macpirxclkgatingen << 14));
}

/**
 * @brief Returns the current value of the mac_core_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mac_core_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_core_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mac_core_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the mac_core_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccoreclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mac_core_clk_gating_en_setf(uint8_t maccoreclkgatingen)
{
    ASSERT_ERR((((uint32_t)maccoreclkgatingen << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)maccoreclkgatingen << 13));
}

/**
 * @brief Returns the current value of the mac_crypt_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mac_crypt_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_crypt_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mac_crypt_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the mac_crypt_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccryptclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mac_crypt_clk_gating_en_setf(uint8_t maccryptclkgatingen)
{
    ASSERT_ERR((((uint32_t)maccryptclkgatingen << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)maccryptclkgatingen << 12));
}

/**
 * @brief Returns the current value of the mac_core_tx_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mac_core_tx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_core_tx_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mac_core_tx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the mac_core_tx_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccoretxclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mac_core_tx_clk_gating_en_setf(uint8_t maccoretxclkgatingen)
{
    ASSERT_ERR((((uint32_t)maccoretxclkgatingen << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)maccoretxclkgatingen << 11));
}

/**
 * @brief Returns the current value of the mac_core_rx_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mac_core_rx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_core_rx_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mac_core_rx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the mac_core_rx_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccorerxclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mac_core_rx_clk_gating_en_setf(uint8_t maccorerxclkgatingen)
{
    ASSERT_ERR((((uint32_t)maccorerxclkgatingen << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)maccorerxclkgatingen << 10));
}

/**
 * @brief Returns the current value of the mac_wt_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mac_wt_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_wt_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mac_wt_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the mac_wt_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macwtclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mac_wt_clk_gating_en_setf(uint8_t macwtclkgatingen)
{
    ASSERT_ERR((((uint32_t)macwtclkgatingen << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)macwtclkgatingen << 9));
}

/**
 * @brief Returns the current value of the mpif_clk_gating_en field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the mpif_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mpif_clk_gating_en field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_mpif_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the mpif_clk_gating_en field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mpifclkgatingen - The value to set the field to.
 */
__INLINE void sysctrl_mpif_clk_gating_en_setf(uint8_t mpifclkgatingen)
{
    ASSERT_ERR((((uint32_t)mpifclkgatingen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)mpifclkgatingen << 8));
}

/**
 * @brief Returns the current value of the bootrom_enable field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the bootrom_enable field's value will be returned.
 *
 * @return The current value of the bootrom_enable field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_bootrom_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the bootrom_enable field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bootromenable - The value to set the field to.
 */
__INLINE void sysctrl_bootrom_enable_setf(uint8_t bootromenable)
{
    ASSERT_ERR((((uint32_t)bootromenable << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)bootromenable << 4));
}

/**
 * @brief Returns the current value of the fpgab_reset_req field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the fpgab_reset_req field's value will be returned.
 *
 * @return The current value of the fpgab_reset_req field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_fpgab_reset_req_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the fpgab_reset_req field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fpgabresetreq - The value to set the field to.
 */
__INLINE void sysctrl_fpgab_reset_req_setf(uint8_t fpgabresetreq)
{
    ASSERT_ERR((((uint32_t)fpgabresetreq << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)fpgabresetreq << 1));
}

/**
 * @brief Returns the current value of the soft_reset_req field in the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read and the soft_reset_req field's value will be returned.
 *
 * @return The current value of the soft_reset_req field in the MISC_CNTL register.
 */
__INLINE uint8_t sysctrl_soft_reset_req_getf(void)
{
    uint32_t localVal = REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the soft_reset_req field of the MISC_CNTL register.
 *
 * The MISC_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] softresetreq - The value to set the field to.
 */
__INLINE void sysctrl_soft_reset_req_setf(uint8_t softresetreq)
{
    ASSERT_ERR((((uint32_t)softresetreq << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(SYSCTRL_MISC_CNTL_ADDR, (REG_PL_RD(SYSCTRL_MISC_CNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)softresetreq << 0));
}

/// @}


#endif // _REG_SYSCTRL_H_

/// @}

