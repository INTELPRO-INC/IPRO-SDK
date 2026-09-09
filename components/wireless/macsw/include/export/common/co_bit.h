/**
 ****************************************************************************************
 *
 * @file co_bit.h
 *
 * @brief Provide bit manipulation macros
 *
 * Copyright (C) RivieraWaves 2011-2022
 *
 *
 ****************************************************************************************
 */

#ifndef _CO_BIT_H_
#define _CO_BIT_H_
#include "export/compiler.h"

/**
 ****************************************************************************************
 * @addtogroup CO_BIT
 * @ingroup COMMON
 * @brief Bit manipulation useful macro
 *
 * @{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Return value with one bit set.
 *
 * @param[in] pos Position of the bit to set.
 *
 * @return Value with one bit set.
 ****************************************************************************************
 */
#define CO_BIT(pos) (1UL << (pos))

/**
 ****************************************************************************************
 * @brief Return a all 1's mask of a certain width, from a certain position.
 *
 * @param[in] width Width of the mask.
 * @param[in] lsb Position of the lsb of the mask.
 *
 * @return Mask value.
 ****************************************************************************************
 */
#define CO_MASK(width, lsb) (((1UL << (width)) - 1) << lsb)

/**
 ****************************************************************************************
 * @brief Define mask and offset value for a given field of a bitfield structure
 *
 * This defines the values `<bf>_<field>_MSK`, `<bf>_<field>_OFT` and
 * `<bf>_<field>_WIDTH`.\n
 * This macro is intended to be used inside an enum, so that all fields of a given
 * bitfield are defined at the same place.\n
 * Here is an example, for a bitfield REG that is composed of 3 fields (F1, F2 and F3):
 * @code{.c}
 * enum REG_BF
 * {
 *     BF_FIELD(REG, F1, 0, 12),
 *     BF_FIELD(REG, F2, 12, 12),
 *     BF_FIELD(REG, F3, 24, 8).
 * };
 * @endcode
 *
 * Once defined using BF_FIELD, a bitfield structure can be manipulated using @ref BF_GET,
 * @ref BF_SET and @ref BF_VAL macros.\n
 *
 * @param[in] bf       Name of the bitfield
 * @param[in] field    Name of the field
 * @param[in] lsb      Bit index at which field starts
 * @param[in] bit_len  Length, in bits, of the field
 ****************************************************************************************
 */
#define BF_FIELD(bf, field, lsb, bit_len)       \
    bf##_##field##_MSK = CO_MASK(bit_len, lsb), \
    bf##_##field##_WIDTH = bit_len,             \
    bf##_##field##_OFT = lsb

/**
 ****************************************************************************************
 * @brief Extract field value from a bitfield structure
 *
 * This macro is intended to be used for field defined by @ref BF_FIELD.
 *
 * @param[in] bf       Name of the bitfield (same as the one used in @ref BF_FIELD)
 * @param[in] field    Name of the field (same as the one used in @ref BF_FIELD)
 * @param[in] bf_val   Bitfield global value from which to extract field value
 * @return The right shifted value of the requested field
 ****************************************************************************************
 */
#define BF_GET(bf, field, bf_val) ((bf##_##field##_MSK & (bf_val)) >> bf##_##field##_OFT)

/**
 ****************************************************************************************
 * @brief Test whether a field value from a bitfield structure is null or not
 *
 * This macro is intended to be used for field defined by @ref BF_FIELD.
 * This is mainly intended for field that are defined on a single bit, to avoid the
 * unnecessary right shift of @ref BF_GET.
 *
 * @param[in] bf       Name of the bitfield (same as the one used in @ref BF_FIELD)
 * @param[in] field    Name of the field (same as the one used in @ref BF_FIELD)
 * @param[in] bf_val   Bitfield global value from which to extract field value
 * @return A Boolean indicating whether the requested field value is 0 or not
 ****************************************************************************************
 */
#define BF_IS_SET(bf, field, bf_val) ((bf##_##field##_MSK & (bf_val)) != 0)

/**
 ****************************************************************************************
 * @brief Set field of a bitfield structure to a given variable
 *
 * This macro is intended to be used for field defined by @ref BF_FIELD.
 * @note: If value is too large for the field it is truncated.
 *
 * @param[in]     bf         Name of the bitfield (same as the one used in @ref BF_FIELD)
 * @param[in]     field      Name of the field (same as the one used in @ref BF_FIELD)
 * @param[in,out] bf_var     Variable containing bitfield global value to update
 * @param[in]     field_val  Value to set for the field
 ****************************************************************************************
 */
#define BF_SET(bf, field, bf_var, field_val)     \
    bf_var = (((bf_var) & ~bf##_##field##_MSK) | \
              (bf##_##field##_MSK & ((field_val) << bf##_##field##_OFT)))

/**
 ****************************************************************************************
 * @brief Return the 'left shifted' value for a field of a bitfield structure
 *
 * This macro is intended to be used for field defined by @ref BF_FIELD.
 * This can be used instead of @ref BF_SET when there is no need to clear the field first.
 * @note: If value is too large for the field it is truncated.
 *
 * @param[in] bf         Name of the bitfield (same as the one used in @ref BF_FIELD)
 * @param[in] field      Name of the field (same as the one used in @ref BF_FIELD)
 * @param[in] field_val  Value to set for the field
 * @return Value of the bitfield structure with only field `<field>` set to `field_val`
 ****************************************************************************************
 */
#define BF_VAL(bf, field, field_val) \
    (bf##_##field##_MSK & ((field_val) << bf##_##field##_OFT))

/**
 ****************************************************************************************
 * @brief Get raw mask for a given field
 *
 * This macro is intended to be used for field defined by @ref BF_FIELD.
 * For more complicated test than on a single field, it may be required to access the
 * mask value
 *
 * @param[in] bf         Name of the bitfield (same as the one used in @ref BF_FIELD)
 * @param[in] field      Name of the field (same as the one used in @ref BF_FIELD)
 * @return Value of mask for the requested field
 ****************************************************************************************
 */
#define BF_MASK(bf, field) bf##_##field##_MSK

/**
 ****************************************************************************************
 * @brief Extract field value from a bitfield structure at specific index
 *
 * This macro is intended to be used for field defined by @ref BF_FIELD when the field
 * is repeated several times.\n
 * For example consider the 32bits bitfield defined as:\n
 * - b0..7:   reg1
 * - b8..15:  reg2
 * - b16..23: reg3
 * - b24..31: reg4
 * where value regx have the same 'type'.
 * One could define each subfield and access them using BF_GET
 * @code{.c}
 * enum REGS_BF
 * {
 *    BF_FIELD(REGS, REG1, 0, 8),
 *    BF_FIELD(REGS, REG2, 8, 8),
 *    BF_FIELD(REGS, REG3, 16, 8),
 *    BF_FIELD(REGS, REG4, 24, 8),
 * };
 *
 * uint32_t regs_value;
 * int reg1 = BG_GET(REGS, REG1, regs_value);
 * int reg2 = BG_GET(REGS, REG2, regs_value);
 * @endcode
 *
 * Or define only one subfield and use BF_IDX_GET to access all subfields.
 * @code{.c}
 * enum REGS_BF
 * {
 *    BF_FIELD(REGS, REGx, 0, 8),
 * };
 *
 * uint32_t regs_value;
 * int reg1 = BG_IDX_GET(REGS, REGx, 0, regs_value);
 * int reg2 = BG_IDX_GET(REGS, REGx, 1, regs_value);
 * @endcode
 *
 * @param[in] bf       Name of the bitfield (same as the one used in @ref BF_FIELD)
 * @param[in] field    Name of the field (same as the one used in @ref BF_FIELD)
 * @param[in] idx      Index to extract
 * @param[in] bf_val   Bitfield global value from which to extract field value
 * @return The right shifted value of the requested field
 ****************************************************************************************
 */
#define BF_IDX_GET(bf, field, idx, bf_val)                               \
    (((bf##_##field##_MSK << ((idx) * bf##_##field##_WIDTH)) & (bf_val)) \
     >> (bf##_##field##_OFT + ((idx) * bf##_##field##_WIDTH)))

/**
 ****************************************************************************************
 * @brief Set field of a bitfield structure to a given variable at specific index
 *
 * This macro is intended to be used for field defined by @ref BF_FIELD when the field
 * is repeated several times. See @ref BF_IDX_GET, for an example.\n
 * @note: If value is too large for the field it is truncated.
 *
 * @param[in]     bf         Name of the bitfield (same as the one used in @ref BF_FIELD)
 * @param[in]     field      Name of the field (same as the one used in @ref BF_FIELD)
 * @param[in]     idx        Index to set
 * @param[in,out] bf_var     Variable containing bitfield global value to update
 * @param[in]     field_val  Value to set for the field
 ****************************************************************************************
 */
#define BF_IDX_SET(bf, field, idx, bf_var, field_val)                                \
    bf_var = (((bf_var) & ~(bf##_##field##_MSK << ((idx) * bf##_##field##_WIDTH))) | \
              ((bf##_##field##_MSK << ((idx) * bf##_##field##_WIDTH)) &              \
               ((field_val) << (bf##_##field##_OFT + ((idx) * bf##_##field##_WIDTH)))))

/**
 ****************************************************************************************
 * @brief Return the 'left shifted' value for a field of a bitfield structure
 *
 * This macro is intended to be used for field defined by @ref BF_FIELD when the field
 * is repeated several times. See @ref BF_IDX_GET, for an example.\n.
 * This can be used instead of @ref BF_SET when there is no need to clear the field first.
 * @note: If value is too large for the field it is truncated.
 *
 * @param[in] bf         Name of the bitfield (same as the one used in @ref BF_FIELD)
 * @param[in] field      Name of the field (same as the one used in @ref BF_FIELD)
 * @param[in] idx        Index to set
 * @param[in] field_val  Value to set for the field
 * @return Value of the bitfield structure with only field `<field>` set to `field_val`
 ****************************************************************************************
 */
#define BF_IDX_VAL(bf, field, idx, field_val)                 \
    ((bf##_##field##_MSK << ((idx) * bf##_##field##_WIDTH)) & \
     ((field_val) << (bf##_##field##_OFT + ((idx) * bf##_##field##_WIDTH))))


/**
 ****************************************************************************************
 * @brief Define mask and offset value for a given field of a long bitfield structure
 *
 * Contrary with simple bitfield (@ref BF_FIELD) whose all subfields are located in a
 * single word, Long bifitfield spread across larger memory zone (always represented as
 * an array of bytes).
 * Usage is the same as @ref BF_FIELD, the only difference is that the enum value for
 * the mask `<bf>_<field>_MSK` is not defined.
 * Use @ref LBF_GET and @ref LBF_SET macros to manipulate a long bitfield structure.
 *
 * @note Current implementation only support up to 8 bits long subfield.
 *
 * @param[in] bf       Name of the bitfield
 * @param[in] field    Name of the field
 * @param[in] lsb      Bit index at which field starts
 * @param[in] bit_len  Length, in bits, of the field
 ****************************************************************************************
 */
#define LBF_FIELD(bf, field, lsb, bit_len) \
    bf##_##field##_WIDTH = bit_len,        \
    bf##_##field##_OFT = lsb

/**
 ****************************************************************************************
 * @brief Extract field value from a long bitfield structure
 *
 * This macro is intended to be used for field defined by @ref LBF_FIELD.
 *
 * @param[in] bf       Name of the bitfield (same as the one used in @ref LBF_FIELD)
 * @param[in] field    Name of the field (same as the one used in @ref LBF_FIELD)
 * @param[in] array    Array containing the Long Bitfield value
 * @return The value of the requested field
 ****************************************************************************************
 */
#define LBF_GET(bf, field, array) \
    co_bits_get(array, bf##_##field##_OFT, bf##_##field##_WIDTH, 0)

/**
 ****************************************************************************************
 * @brief Extract indexed field value from a long bitfield structure
 *
 * Same as @ref LBF_GET but for indexed field (see @ref BF_IDX_GET for example of
 * indexed field)
 *
 * @param[in] bf       Name of the bitfield (same as the one used in @ref LBF_FIELD)
 * @param[in] field    Name of the field (same as the one used in @ref LBF_FIELD)
 * @param[in] idx      Index to set
 * @param[in] array    Array containing the Long Bitfield value
 * @return The value of the requested field
 ****************************************************************************************
 */
#define LBF_IDX_GET(bf, field, idx, array)                                \
    co_bits_get(array, bf##_##field##_OFT + (idx * bf##_##field##_WIDTH), \
                bf##_##field##_WIDTH, 0)

/**
 ****************************************************************************************
 * @brief Set field of a bitfield structure to a given value
 *
 * This macro is intended to be used for field defined by @ref LBF_FIELD.
 * @note: If value is too large for the field it is truncated.
 *
 * @param[in]     bf       Name of the bitfield (same as the one used in @ref LBF_FIELD)
 * @param[in]     field    Name of the field (same as the one used in @ref LBF_FIELD)
 * @param[in,out] array    Array containing the Long Bitfield value
 * @param[in]     val      Value to set for the field
 ****************************************************************************************
 */
#define LBF_SET(bf, field, array, val) \
    co_bits_set(array, bf##_##field##_OFT, bf##_##field##_WIDTH, val)

/**
 ****************************************************************************************
 * @brief Set indexed field of a bitfield structure to a given value
 *
 * Same as @ref LBF_SET but for indexed field (see @ref BF_IDX_GET for example of
 * indexed field)
 *
 * @param[in]     bf       Name of the bitfield (same as the one used in @ref LBF_FIELD)
 * @param[in]     field    Name of the field (same as the one used in @ref LBF_FIELD)
 * @param[in]     idx      Index to set
 * @param[in,out] array    Array containing the Long Bitfield value
 * @param[in]     val      Value to set for the field
 ****************************************************************************************
 */
#define LBF_IDX_SET(bf, field, idx, array, val)                           \
    co_bits_set(array, bf##_##field##_OFT + (idx * bf##_##field##_WIDTH), \
                bf##_##field##_WIDTH, val)

/**
 ****************************************************************************************
 * @brief Extract specific bits from an array of bits
 *
 * This is the process function for @ref LBF_GET macro.
 * @note Because it is an inline function, when called with hardcoded value (i.e. via
 * @ref LBF_GET) compiler are smart enough to only include the necessary code.
 *
 * @param[in] array      Array of bits
 * @param[in] lsb        Position of the LSB of the field inside the array of bits
 * @param[in] width      Width of the field
 * @param[in] array_len  If not 0, the size of the array in bytes.
 *                       If bit asked is out of bound, immediately return 0.
 *
 * @return The requested bits
 ****************************************************************************************
 */
__INLINE uint8_t co_bits_get(uint8_t const array[], int lsb, int width, int array_len)
{
    uint8_t res;
    int byte_idx = lsb / 8;
    int byte_oft = lsb % 8;
    int byte_len;

    if (array_len && (byte_idx >= array_len))
        return 0;

    if (width == 1)
        return (array[byte_idx] & CO_BIT(byte_oft)) >> byte_oft;

    byte_len = byte_oft + width;
    if (byte_len <= 8)
    {
        uint8_t mask = CO_BIT(width) - 1;
        res = (array[byte_idx] >> byte_oft) & mask;
    }
    else
    {
        uint8_t mask;
        int width2 = byte_len - 8;
        width -= width2;
        mask = CO_BIT(width) - 1;
        res = (array[byte_idx++] >> byte_oft) & mask;
        mask = CO_BIT(width2) - 1;
        res |= (array[byte_idx] & mask) << width;
    }

    return res;
}


/**
 ****************************************************************************************
 * @brief Set specific bits in an array of bits
 *
 * This is the process function for @ref LBF_SET macro.\n
 * Attention that the val is first mask with the maximum width so that it doesn't
 * overflow the requested width.
 *
 * @note Because it is an inline function, when called with hardcoded value (i.e. via
 * @ref LBF_SET) compiler are smart enough to only include the necessary code.
 *
 * @param[in] array  Array of bits
 * @param[in] lsb    Position of the LSB of the field inside the array of bits
 * @param[in] width  Width of the field
 * @param[in] val    Value to set (masked with maximum value allowed by @p width first)
 ****************************************************************************************
 */
__INLINE void co_bits_set(uint8_t array[], int lsb, int width, uint8_t val)
{
    int byte_idx = lsb / 8;
    int byte_oft = lsb % 8;
    int byte_len;

    val &= (CO_BIT(width) - 1);

    if (width == 1)
    {
        if (val)
            array[byte_idx] |= CO_BIT(byte_oft);
        else
            array[byte_idx] &= ~CO_BIT(byte_oft);
        return;
    }

    byte_len = byte_oft + width;
    if (byte_len <= 8)
    {
        uint8_t mask = (CO_BIT(width) - 1) << byte_oft;
        array[byte_idx] &= ~mask;
        array[byte_idx] |= val << byte_oft;
    }
    else
    {
        uint8_t mask;
        int width2 = byte_len - 8;
        width -= width2;
        mask = (CO_BIT(width) - 1) << byte_oft;
        array[byte_idx] &= ~mask;
        array[byte_idx] |= val << byte_oft;
        byte_idx++;
        mask = CO_BIT(width2) - 1;
        array[byte_idx] &= ~mask;
        array[byte_idx] |= val >> width;
    }
}

/** @} */
#endif // _CO_BIT_H_
